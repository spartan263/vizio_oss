/*----------------------------------------------------------------------------
 * No Warranty :  Except  as  may  be  otherwise  agreed  to  in  writing, no *
 * warranties  of  any  kind,  whether  express or  implied, are given by MTK *
 * with  respect  to  any  Confidential  Information  or any use thereof, and *
 * the  Confidential  Information  is  provided  on  an  "AS IS"  basis.  MTK *
 * hereby  expressly  disclaims  all  such  warranties, including any implied *
 * warranties   of  merchantability ,  non-infringement  and  fitness  for  a *
 * particular purpose and any warranties arising out of course of performance *
 * course  of dealing or usage of trade.  Parties further acknowledge that in *
 * connection  with  the Purpose, Company may, either presently and/or in the *
 * future,   instruct   MTK   to   assist  it  in  the  development  and  the *
 * implementation,  in accordance with Company's designs, of certain software *
 * relating  to  Company's  product(s)  (the  "Services").   Except as may be *
 * otherwise agreed to in writing, no warranties of any kind, whether express *
 * or  implied,  are  given by MTK with respect to the Services provided, and *
 * the  Services  are  provided  on  an  "AS  IS"  basis.   Company   further *
 * acknowledges  that  the  Services  may  contain  errors,  which testing is *
 * important  and  it  is  solely  responsible for fully testing the Services *
 * and/or   derivatives   thereof   before  they  are  used,  sublicensed  or *
 * distributed.   Should  there  be any third party action brought again MTK, *
 * arising  out  of  or  relating  to  the  Services,  Company agree to fully *
 * indemnify and hold MTK harmless.                                           *
 *---------------------------------------------------------------------------*/
/***************************************************************************/
/*                                                                         */
/*  ftstdlib.h                                                             */
/*                                                                         */
/*    ANSI-specific library and header configuration file (specification   */
/*    only).                                                               */
/*                                                                         */
/*  Copyright 2002, 2003, 2004 by                                          */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* This file is used to group all #includes to the ANSI C library that   */
  /* FreeType normally requires.  It also defines macros to rename the     */
  /* standard functions within the FreeType source code.                   */
  /*                                                                       */
  /* Load a file which defines __FTSTDLIB_H__ before this one to override  */
  /* it.                                                                   */
  /*                                                                       */
  /*************************************************************************/


#ifndef __FTSTDLIB_H__
#define __FTSTDLIB_H__


  /**********************************************************************/
  /*                                                                    */
  /*                           integer limits                           */
  /*                                                                    */
  /* UINT_MAX and ULONG_MAX are used to automatically compute the size  */
  /* of `int' and `long' in bytes at compile-time.  So far, this works  */
  /* for all platforms the library has been tested on.                  */
  /*                                                                    */
  /* Note that on the extremely rare platforms that do not provide      */
  /* integer types that are _exactly_ 16 and 32 bits wide (e.g. some    */
  /* old Crays where `int' is 36 bits), we do not make any guarantee    */
  /* about the correct behaviour of FT2 with all fonts.                 */
  /*                                                                    */
  /* In these case, "ftconfig.h" will refuse to compile anyway with a   */
  /* message like "couldn't find 32-bit type" or something similar.     */
  /*                                                                    */
  /* IMPORTANT NOTE: We do not define aliases for heap management and   */
  /*                 i/o routines (i.e. malloc/free/fopen/fread/...)    */
  /*                 since these functions should all be encapsulated   */
  /*                 by platform-specific implementations of            */
  /*                 "ftsystem.c".                                      */
  /*                                                                    */
  /**********************************************************************/

/*
#include "x_typedef.h"
#include "x_os.h"
#include "x_libc.h"
*/
#include <stdlib.h>

#include <limits.h>

#define FT_UINT_MAX   UINT_MAX
#define FT_ULONG_MAX  ULONG_MAX


  /**********************************************************************/
  /*                                                                    */
  /*                 character and string processing                    */
  /*                                                                    */
  /**********************************************************************/

/*
#include <ctype.h>

#define ft_isalnum   isalnum
#define ft_isupper   isupper
#define ft_islower   islower
#define ft_isdigit   isdigit
#define ft_isxdigit  isxdigit
*/

/*
#include <string.h>

#define ft_memcmp   memcmp
#define ft_memcpy   memcpy
#define ft_memmove  memmove
#define ft_memset   memset
#define ft_strcat   strcat
#define ft_strcmp   strcmp
#define ft_strcpy   strcpy
#define ft_strlen   strlen
#define ft_strncmp  strncmp
#define ft_strncpy  strncpy
#define ft_strrchr  strrchr
*/
#define ft_memcmp   x_memcmp
#define ft_memcpy   memcpy
#define ft_memmove  x_memmove
#define ft_memset   memset
#define ft_strcat   x_strcat
#define ft_strcmp   strcmp
#define ft_strcpy   x_strcpy
#define ft_strlen   x_strlen
#define ft_strncmp  x_strncmp
#define ft_strncpy  x_strncpy
#define ft_strrchr  x_strrchr

/*
#include <stdio.h>

#define ft_sprintf  sprintf
*/
#define ft_sprintf  sprintf


  /**********************************************************************/
  /*                                                                    */
  /*                             sorting                                */
  /*                                                                    */
  /**********************************************************************/


//#include <stdlib.h>

//#define ft_qsort  qsort
//#define ft_exit   exit    /* only used to exit from unhandled exceptions */

//#define ft_atol   atol
#define ft_qsort  qsort

  /**********************************************************************/
  /*                                                                    */
  /*                         execution control                          */
  /*                                                                    */
  /**********************************************************************/


#include <setjmp.h>

#define ft_jmp_buf  jmp_buf   /* note: this cannot be a typedef since */
                              /*       jmp_buf is defined as a macro  */
                              /*       on certain platforms           */

#define ft_setjmp   setjmp    /* same thing here */
#define ft_longjmp  longjmp   /* "               */


  /* the following is only used for debugging purposes, i.e. when */
  /* FT_DEBUG_LEVEL_ERROR or FT_DEBUG_LEVEL_TRACE are defined     */
  /*                                                              */
#include <stdarg.h>


#endif /* __FTSTDLIB_H__ */


/* END */
