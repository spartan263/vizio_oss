/*
 * kilall5.c	Kill all processes except processes that have the
 *		same session id, so that the shell that called us
 *		won't be killed. Typically used in shutdown scripts.
 *
 * pidof.c	Tries to get the pid of the process[es] named.
 *
 * Version:	2.86 30-Jul-2004 MvS
 *
 * Usage:	killall5 [-][signal]
 *		pidof [-s] [-o omitpid [-o omitpid]] program [program..]
 *
 * Authors:	Miquel van Smoorenburg, miquels@cistron.nl
 *
 *		Riku Meskanen, <mesrik@jyu.fi>
 *		- return all running pids of given program name
 *		- single shot '-s' option for backwards combatibility
 *		- omit pid '-o' option and %PPID (parent pid metavariable)
 *		- syslog() only if not a connected to controlling terminal
 *		- swapped out programs pids are caught now
 *
 *		This file is part of the sysvinit suite,
 *		Copyright 1991-2004 Miquel van Smoorenburg.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <syslog.h>
#include <getopt.h>
#include <stdarg.h>

char *Version = "@(#)killall5 2.86 31-Jul-2004 miquels@cistron.nl";

#define STATNAMELEN	15

/* Info about a process. */
typedef struct proc {
	char *argv0;		/* Name as found out from argv[0] */
	char *argv0base;	/* `basename argv[1]`		  */
	char *argv1;		/* Name as found out from argv[1] */
	char *argv1base;	/* `basename argv[1]`		  */
	char *statname;		/* the statname without braces    */
	ino_t ino;		/* Inode number			  */
	dev_t dev;		/* Device it is on		  */
	pid_t pid;		/* Process ID.			  */
	int sid;		/* Session ID.			  */
	int kernel;		/* Kernel thread or zombie.	  */
	struct proc *next;	/* Pointer to next struct. 	  */
} PROC;

/* pid queue */

typedef struct pidq {
	PROC		*proc;
	struct pidq	*next;
} PIDQ;

typedef struct {
	PIDQ		*head;
	PIDQ		*tail;
	PIDQ		*next;
} PIDQ_HEAD;

/* List of processes. */
PROC *plist;

/* Did we stop all processes ? */
int sent_sigstop;

int scripts_too = 0;

char *progname;	/* the name of the running program */
#ifdef __GNUC__
__attribute__ ((format (printf, 2, 3)))
#endif
void nsyslog(int pri, char *fmt, ...);

/*
 *	Malloc space, barf if out of memory.
 */
void *xmalloc(int bytes)
{
	void *p;

	if ((p = malloc(bytes)) == NULL) {
		if (sent_sigstop) kill(-1, SIGCONT);
		nsyslog(LOG_ERR, "out of memory");
		exit(1);
	}
	return p;
}

/*
 *	See if the proc filesystem is there. Mount if needed.
 */
int mount_proc(void)
{
	struct stat	st;
	char		*args[] = { "mount", "-t", "proc", "proc", "/proc", 0 };
	pid_t		pid, rc;
	int		wst;
	int		did_mount = 0;

	/* Stat /proc/version to see if /proc is mounted. */
	if (stat("/proc/version", &st) < 0 && errno == ENOENT) {

		/* It's not there, so mount it. */
		if ((pid = fork()) < 0) {
			nsyslog(LOG_ERR, "cannot fork");
			exit(1);
		}
		if (pid == 0) {
			/* Try a few mount binaries. */
			execv("/sbin/mount", args);
			execv("/bin/mount", args);

			/* Okay, I give up. */
			nsyslog(LOG_ERR, "cannot execute mount");
			exit(1);
		}
		/* Wait for child. */
		while ((rc = wait(&wst)) != pid)
			if (rc < 0 && errno == ECHILD)
				break;
		if (rc != pid || WEXITSTATUS(wst) != 0)
			nsyslog(LOG_ERR, "mount returned non-zero exit status");

		did_mount = 1;
	}

	/* See if mount succeeded. */
	if (stat("/proc/version", &st) < 0) {
		if (errno == ENOENT)
			nsyslog(LOG_ERR, "/proc not mounted, failed to mount.");
		else
			nsyslog(LOG_ERR, "/proc unavailable.");
		exit(1);
	}

	return did_mount;
}

int readarg(FILE *fp, char *buf, int sz)
{
	int		c = 0, f = 0;

	while (f < (sz-1) && (c = fgetc(fp)) != EOF && c)
		buf[f++] = c;
	buf[f] = 0;

	return (c == EOF && f == 0) ? c : f;
}

/*
 *	Read the proc filesystem.
 */
int readproc()
{
	DIR		*dir;
	FILE		*fp;
	PROC		*p, *n;
	struct dirent	*d;
	struct stat	st;
	char		path[256];
	char		buf[256];
	char		*s, *q;
	unsigned long	startcode, endcode;
	int		pid, f;

	/* Open the /proc directory. */
	if ((dir = opendir("/proc")) == NULL) {
		nsyslog(LOG_ERR, "cannot opendir(/proc)");
		return -1;
	}

	/* Free the already existing process list. */
	n = plist;
	for (p = plist; n; p = n) {
		n = p->next;
		if (p->argv0) free(p->argv0);
		if (p->argv1) free(p->argv1);
		free(p);
	}
	plist = NULL;

	/* Walk through the directory. */
	while ((d = readdir(dir)) != NULL) {

		/* See if this is a process */
		if ((pid = atoi(d->d_name)) == 0) continue;

		/* Get a PROC struct . */
		p = (PROC *)xmalloc(sizeof(PROC));
		memset(p, 0, sizeof(PROC));

		/* Open the status file. */
		snprintf(path, sizeof(path), "/proc/%s/stat", d->d_name);

		/* Read SID & statname from it. */
 		if ((fp = fopen(path, "r")) != NULL) {
			buf[0] = 0;
			fgets(buf, sizeof(buf), fp);

			/* See if name starts with '(' */
			s = buf;
			while (*s != ' ') s++;
			s++;
			if (*s == '(') {
				/* Read program name. */
				q = strrchr(buf, ')');
				if (q == NULL) {
					p->sid = 0;
					nsyslog(LOG_ERR,
					"can't get program name from %s\n",
						path);
					free(p);
					continue;
				}
				s++;
			} else {
				q = s;
				while (*q != ' ') q++;
			}
			*q++ = 0;
			while (*q == ' ') q++;
			p->statname = (char *)xmalloc(strlen(s)+1);
			strcpy(p->statname, s);

			/* Get session, startcode, endcode. */
			startcode = endcode = 0;
			if (sscanf(q, 	"%*c %*d %*d %d %*d %*d %*u %*u "
					"%*u %*u %*u %*u %*u %*d %*d "
					"%*d %*d %*d %*d %*u %*u %*d "
					"%*u %lu %lu",
					&p->sid, &startcode, &endcode) != 3) {
				p->sid = 0;
				nsyslog(LOG_ERR, "can't read sid from %s\n",
					path);
				free(p);
				continue;
			}
			if (startcode == 0 && endcode == 0)
				p->kernel = 1;
			fclose(fp);
		} else {
			/* Process disappeared.. */
			free(p);
			continue;
		}

		snprintf(path, sizeof(path), "/proc/%s/cmdline", d->d_name);
		if ((fp = fopen(path, "r")) != NULL) {

			/* Now read argv[0] */
			f = readarg(fp, buf, sizeof(buf));

			if (buf[0]) {
				/* Store the name into malloced memory. */
				p->argv0 = (char *)xmalloc(f + 1);
				strcpy(p->argv0, buf);

				/* Get a pointer to the basename. */
				p->argv0base = strrchr(p->argv0, '/');
				if (p->argv0base != NULL)
					p->argv0base++;
				else
					p->argv0base = p->argv0;
			}

			/* And read argv[1] */
			while ((f = readarg(fp, buf, sizeof(buf))) != EOF)
				if (buf[0] != '-') break;

			if (buf[0]) {
				/* Store the name into malloced memory. */
				p->argv1 = (char *)xmalloc(f + 1);
				strcpy(p->argv1, buf);

				/* Get a pointer to the basename. */
				p->argv1base = strrchr(p->argv1, '/');
				if (p->argv1base != NULL)
					p->argv1base++;
				else
					p->argv1base = p->argv1;
			}

			fclose(fp);

		} else {
			/* Process disappeared.. */
			free(p);
			continue;
		}

		/* Try to stat the executable. */
		snprintf(path, sizeof(path), "/proc/%s/exe", d->d_name);
		if (stat(path, &st) == 0) {
			p->dev = st.st_dev;
			p->ino = st.st_ino;
		}

		/* Link it into the list. */
		p->next = plist;
		plist = p;
		p->pid = pid;
	}
	closedir(dir);

	/* Done. */
	return 0;
}

PIDQ_HEAD *init_pid_q(PIDQ_HEAD *q)
{
	q->head =  q->next = q->tail = NULL;
	return q;
}

int empty_q(PIDQ_HEAD *q)
{
	return (q->head == NULL);
}

int add_pid_to_q(PIDQ_HEAD *q, PROC *p)
{
	PIDQ *tmp;

	tmp = (PIDQ *)xmalloc(sizeof(PIDQ));

	tmp->proc = p;
	tmp->next = NULL;

	if (empty_q(q)) {
		q->head = tmp;
		q->tail  = tmp;
	} else {
		q->tail->next = tmp;
		q->tail = tmp;
	}
	return 0;
}

PROC *get_next_from_pid_q(PIDQ_HEAD *q)
{
	PROC		*p;
	PIDQ		*tmp = q->head;

	if (!empty_q(q)) {
		p = q->head->proc;
		q->head = tmp->next;
		free(tmp);
		return p;
	}

	return NULL;
}

/* Try to get the process ID of a given process. */
PIDQ_HEAD *pidof(char *prog)
{
	PROC		*p;
	PIDQ_HEAD	*q;
	struct stat	st;
	char		*s;
	int		dostat = 0;
	int		foundone = 0;
	int		ok = 0;

	/* Try to stat the executable. */
	if (prog[0] == '/' && stat(prog, &st) == 0) dostat++;

	/* Get basename of program. */
	if ((s = strrchr(prog, '/')) == NULL)
		s = prog;
	else
		s++;

	q = (PIDQ_HEAD *)xmalloc(sizeof(PIDQ_HEAD));
	q = init_pid_q(q);

	/* First try to find a match based on dev/ino pair. */
	if (dostat) {
		for (p = plist; p; p = p->next) {
			if (p->dev == st.st_dev && p->ino == st.st_ino) {
				add_pid_to_q(q, p);
				foundone++;
			}
		}
	}

	/* If we didn't find a match based on dev/ino, try the name. */
	if (!foundone) for (p = plist; p; p = p->next) {
		ok = 0;

		/* Compare name (both basename and full path) */
		ok += (p->argv0 && strcmp(p->argv0, prog) == 0);
		ok += (p->argv0 && strcmp(p->argv0base, s) == 0);

		/* For scripts, compare argv[1] as well. */
		if (scripts_too && p->argv1 &&
		    !strncmp(p->statname, p->argv1base, STATNAMELEN)) {
			ok += (strcmp(p->argv1, prog) == 0);
			ok += (strcmp(p->argv1base, s) == 0);
		}

		/*
		 *	if we have a space in argv0, process probably
		 *	used setproctitle so try statname.
		 */
		if (strlen(s) <= STATNAMELEN &&
		    (p->argv0 == NULL ||
		     p->argv0[0] == 0 ||
		     strchr(p->argv0, ' '))) {
			ok += (strcmp(p->statname, s) == 0);
		}
		if (ok) add_pid_to_q(q, p);
	}

	 return q;
}

/* Give usage message and exit. */
void usage(void)
{
	nsyslog(LOG_ERR, "only one argument, a signal number, allowed");
	closelog();
	exit(1);
}

/* write to syslog file if not open terminal */
#ifdef __GNUC__
__attribute__ ((format (printf, 2, 3)))
#endif
void nsyslog(int pri, char *fmt, ...)
{
	va_list  args;

	va_start(args, fmt);

	if (ttyname(0) == NULL) {
		vsyslog(pri, fmt, args);
	} else {
		fprintf(stderr, "%s: ",progname);
		vfprintf(stderr, fmt, args);
		fprintf(stderr, "\n");
	}

	va_end(args);
}

#define PIDOF_SINGLE	0x01
#define PIDOF_OMIT	0x02

#define PIDOF_OMITSZ	5

/*
 *	Pidof functionality.
 */
int main_pidof(int argc, char **argv)
{
	PIDQ_HEAD	*q;
	PROC		*p;
	pid_t		opid[PIDOF_OMITSZ], spid;
	int		f;
	int		first = 1;
	int		i, oind, opt, flags = 0;

	for (oind = PIDOF_OMITSZ-1; oind > 0; oind--)
		opid[oind] = 0;
	opterr = 0;

	while ((opt = getopt(argc,argv,"ho:sx")) != EOF) switch (opt) {
		case '?':
			nsyslog(LOG_ERR,"invalid options on command line!\n");
			closelog();
			exit(1);
		case 'o':
			if (oind >= PIDOF_OMITSZ -1) {
				nsyslog(LOG_ERR,"omit pid buffer size %d "
					"exceeded!\n", PIDOF_OMITSZ);
				closelog();
				exit(1);
			}
			if (strcmp("%PPID",optarg) == 0)
				opid[oind] = getppid();
			else if ((opid[oind] = atoi(optarg)) < 1) {
				nsyslog(LOG_ERR,
					"illegal omit pid value (%s)!\n",
					optarg);
				closelog();
				exit(1);
			}
			oind++;
			flags |= PIDOF_OMIT;
			break;
		case 's':
			flags |= PIDOF_SINGLE;
			break;
		case 'x':
			scripts_too++;
			break;
		default:
			/* Nothing */
			break;
	}
	argc -= optind;
	argv += optind;

	/* Print out process-ID's one by one. */
	readproc();
	for(f = 0; f < argc; f++) {
		if ((q = pidof(argv[f])) != NULL) {
			spid = 0;
			while ((p = get_next_from_pid_q(q))) {
				if (flags & PIDOF_OMIT) {
					for (i = 0; i < oind; i++)
						if (opid[i] == p->pid)
							break;
					/*
					 *	On a match, continue with
					 *	the for loop above.
					 */
					if (i < oind)
						continue;
				}
				if (flags & PIDOF_SINGLE) {
					if (spid)
						continue;
					else
						spid = 1;
				}
				if (!first)
					printf(" ");
				printf("%d", p->pid);
				first = 0;
			}
		}
	}
	printf("\n");
	closelog();
	return(first ? 1 : 0);
}



/* Main for either killall or pidof. */
int main(int argc, char **argv)
{
	PROC		*p;
	int		pid, sid = -1;
	int		sig = SIGKILL;

	/* Get program name. */
	if ((progname = strrchr(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;

	/* Now connect to syslog. */
	openlog(progname, LOG_CONS|LOG_PID, LOG_DAEMON);

	/* Were we called as 'pidof' ? */
	if (strcmp(progname, "pidof") == 0)
		return main_pidof(argc, argv);

	/* Right, so we are "killall". */
	if (argc > 1) {
		if (argc != 2) usage();
		if (argv[1][0] == '-') (argv[1])++;
		if ((sig = atoi(argv[1])) <= 0 || sig > 31) usage();
	}

	/* First get the /proc filesystem online. */
	mount_proc();

	/*
	 *	Ignoring SIGKILL and SIGSTOP do not make sense, but
	 *	someday kill(-1, sig) might kill ourself if we don't
	 *	do this. This certainly is a valid concern for SIGTERM-
	 *	Linux 2.1 might send the calling process the signal too.
	 */
	signal(SIGTERM, SIG_IGN);
	signal(SIGSTOP, SIG_IGN);
	signal(SIGKILL, SIG_IGN);

	/* Now stop all processes. */
	kill(-1, SIGSTOP);
	sent_sigstop = 1;

	/* Read /proc filesystem */
	if (readproc() < 0) {
		kill(-1, SIGCONT);
		exit(1);
	}

	/* Now kill all processes except our session. */
	sid = (int)getsid(0);
	pid = (int)getpid();
	for (p = plist; p; p = p->next)
		if (p->pid != pid && p->sid != sid && !p->kernel)
			kill(p->pid, sig);

	/* And let them continue. */
	kill(-1, SIGCONT);

	/* Done. */
	closelog();

	return 0;
}
