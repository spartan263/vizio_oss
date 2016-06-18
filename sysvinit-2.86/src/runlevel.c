/*
 * runlevel	Prints out the previous and the current runlevel.
 *
 * Version:	@(#)runlevel  1.20  16-Apr-1997  MvS
 *
 *		This file is part of the sysvinit suite,
 *		Copyright 1991-1997 Miquel van Smoorenburg.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

#include <stdio.h>
#include <utmp.h>
#include <time.h>
#include <stdlib.h>

int main(argc, argv)
int argc;
char **argv;
{
  struct utmp *ut;
  char prev;

  if (argc > 1) utmpname(argv[1]);

  setutent();
  while ((ut = getutent()) != NULL) {
	if (ut->ut_type == RUN_LVL) {
		prev = ut->ut_pid / 256;
		if (prev == 0) prev = 'N';
		printf("%c %c\n", prev, ut->ut_pid % 256);
		endutent();
		exit(0);
	}
  }
  
  printf("unknown\n");
  endutent();
  return(1);
}

