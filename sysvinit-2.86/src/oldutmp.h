/*
 * oldutmp.h	Definition of the old libc5 utmp structure.
 *
 * Version:	@(#)oldutmp.h  1.00  29-Mar-1998  miquels@cistron.nl
 *
 */
#ifndef OLD_UTMP_H
#define OLD_UTMP_H

#define OLD_LINESIZE		12
#define OLD_NAMESIZE		8
#define OLD_HOSTSIZE		16

struct oldutmp {
	short	ut_type;
	int	ut_pid;
	char	ut_line[OLD_LINESIZE];
	char	ut_id[4];
	long	ut_oldtime;
	char	ut_user[OLD_NAMESIZE];
	char	ut_host[OLD_HOSTSIZE];
	long	ut_oldaddr;
};

#endif
