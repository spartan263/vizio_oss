/* $Id: //DTV/MP_BR/DTV_X_IDTV0801_002200_2_001_2_001_1210_001/chiling/uboot/u-boot-2011.12/include/bedbug/bedbug.h#1 $ */

#ifndef _BEDBUG_H
#define _BEDBUG_H

#ifndef NULL
#define NULL	0
#endif

#define _USE_PROTOTYPES

#ifndef isblank
#define isblank(c) isspace((int)(c))
#endif

#ifndef __P
#if defined(_USE_PROTOTYPES) && (defined(__STDC__) || defined(__cplusplus))
#define __P(protos)	protos		/* full-blown ANSI C */
#else
#define __P(protos)	()		/* traditional C preprocessor */
#endif
#endif

#endif /* _BEDBUG_H */


/*
 * Copyright (c) 2001 William L. Pitts
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are freely
 * permitted provided that the above copyright notice and this
 * paragraph and the following disclaimer are duplicated in all
 * such forms.
 *
 * This software is provided "AS IS" and without any express or
 * implied warranties, including, without limitation, the implied
 * warranties of merchantability and fitness for a particular
 * purpose.
 */
