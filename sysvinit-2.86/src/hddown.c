/*
 * hddown.c	Find all disks on the system and
 *		shut them down.
 *
 */
char *v_hddown = "@(#)hddown.c  1.02  22-Apr-2003  miquels@cistron.nl";

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>

#ifdef __linux__

#include <sys/ioctl.h>
#include <linux/hdreg.h>

#define MAX_DISKS	64
#define PROC_IDE	"/proc/ide"
#define DEV_BASE	"/dev"

/*
 *	Find all IDE disks through /proc.
 */
static int find_idedisks(char **dev, int maxdev)
{
	DIR *dd;
	FILE *fp;
	struct dirent *d;
	char buf[256];
	int i = 0;

	if ((dd = opendir(PROC_IDE)) == NULL)
		return -1;

	while ((d = readdir(dd)) != NULL) {
		if (strncmp(d->d_name, "hd", 2) != 0)
			continue;
		buf[0] = 0;
		snprintf(buf, sizeof(buf), PROC_IDE "/%s/media", d->d_name);
		if ((fp = fopen(buf, "r")) == NULL)
			continue;
		if (fgets(buf, sizeof(buf), fp) == 0 ||
		    strcmp(buf, "disk\n") != 0) {
			fclose(fp);
			continue;
		}
		fclose(fp);
		snprintf(buf, sizeof(buf), DEV_BASE "/%s", d->d_name);
		dev[i++] = strdup(buf);
		if (i >= maxdev)
			break;
	}
	closedir(dd);
	if (i < maxdev) dev[i] = NULL;

	return 0;
}

/*
 *	Put an IDE disk in standby mode.
 *	Code stolen from hdparm.c
 */
static int do_standby_idedisk(char *device)
{
#ifndef WIN_STANDBYNOW1
#define WIN_STANDBYNOW1 0xE0
#endif
#ifndef WIN_STANDBYNOW2
#define WIN_STANDBYNOW2 0x94
#endif
	unsigned char args1[4] = {WIN_STANDBYNOW1,0,0,0};
	unsigned char args2[4] = {WIN_STANDBYNOW2,0,0,0};
	int fd;

	if ((fd = open(device, O_RDWR)) < 0)
		return -1;

	if (ioctl(fd, HDIO_DRIVE_CMD, &args1) &&
	    ioctl(fd, HDIO_DRIVE_CMD, &args2))
		return -1;

	return 0;
}

/*
 *	First find all IDE disks, then put them in standby mode.
 *	This has the side-effect of flushing the writecache,
 *	which is exactly what we want on poweroff.
 */
int hddown(void)
{
	char *disks[MAX_DISKS+1];
	int i;

	if (find_idedisks(disks, MAX_DISKS) < 0)
		return -1;

	for (i = 0; disks[i] && i < MAX_DISKS; i++)
		do_standby_idedisk(disks[i]);

	return 0;
}

#else /* __linux__ */

int hddown(void)
{
	return 0;
}

#endif /* __linux__ */

#ifdef STANDALONE
int main(int argc, char **argv)
{
	return (hddown() == 0);
}
#endif

