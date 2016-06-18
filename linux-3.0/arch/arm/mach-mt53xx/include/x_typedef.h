/*
 * linux/arch/arm/mach-mt53xx/include/x_typedef.h
 *
 * Copyright (c) 2008-2012 MediaTek Inc.
 * $Author: dtvbm11 $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

#ifndef X_TYPEDEF_H
#define X_TYPEDEF_H


#ifdef __KERNEL__
#include <linux/types.h>
#endif


/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

#ifndef VOID
#define VOID void
#endif

#if !defined (_NO_TYPEDEF_BYTE_) && !defined (_TYPEDEF_BYTE_)
typedef unsigned char BYTE;
#define _TYPEDEF_BYTE_
#endif

#if !defined (_NO_TYPEDEF_UCHAR_) && !defined (_TYPEDEF_UCHAR_)
typedef unsigned char  UCHAR;
#define _TYPEDEF_UCHAR_
#endif

#if !defined (_NO_TYPEDEF_UINT8_) && !defined (_TYPEDEF_UINT8_)
typedef unsigned char  UINT8;
#define _TYPEDEF_UINT8_
#endif

#if !defined (_NO_TYPEDEF_UINT16_) && !defined (_TYPEDEF_UINT16_)
typedef unsigned short  UINT16;
#define _TYPEDEF_UINT16_
#endif

#if !defined (_NO_TYPEDEF_UINT32_) && !defined (_TYPEDEF_UINT32_)
typedef unsigned int  UINT32;
#define _TYPEDEF_UINT32_
#endif

#if !defined (_NO_TYPEDEF_UINT64_) && !defined (_TYPEDEF_UINT64_)
typedef unsigned long long  UINT64;
#define _TYPEDEF_UINT64_
#endif

#if !defined (_NO_TYPEDEF_CHAR_) && !defined (_TYPEDEF_CHAR_)
typedef char  CHAR;     // Debug, should be 'signed char'
#define _TYPEDEF_CHAR_
#endif

#if !defined (_NO_TYPEDEF_INT8_) && !defined (_TYPEDEF_INT8_)
typedef signed char  INT8;
#define _TYPEDEF_INT8_
#endif

#if !defined (_NO_TYPEDEF_INT16_) && !defined (_TYPEDEF_INT16_)
typedef signed short  INT16;
#define _TYPEDEF_INT16_
#endif

#if !defined (_NO_TYPEDEF_INT32_) && !defined (_TYPEDEF_INT32_)
typedef signed int  INT32;
#define _TYPEDEF_INT32_
#endif

#if !defined (_NO_TYPEDEF_INT64_) && !defined (_TYPEDEF_INT64_)
typedef signed long long  INT64;
#define _TYPEDEF_INT64_
#endif

/* Define a boolean as 8 bits. */
#if !defined (_NO_TYPEDEF_BOOL_) && !defined (_TYPEDEF_BOOL_)
typedef UINT8  BOOL;
#define _TYPEDEF_BOOL_
#endif

#if !defined (_NO_TYPEDEF_FLOAT_) && !defined (_TYPEDEF_FLOAT_)
typedef float  FLOAT;
#define _TYPEDEF_FLOAT_
#endif

#if !defined (_NO_TYPEDEF_DOUBLE_)  && !defined (_TYPEDEF_DOUBLE_)
typedef double  DOUBLE;
#define _TYPEDEF_DOUBLE_
#endif

#ifndef UNUSED
#define UNUSED(x)               (void)x
#endif

#ifndef MIN
#define MIN(x, y)               (((x) < (y)) ? (x) : (y))
#endif

#ifndef MSX
#define MAX(x, y)               (((x) > (y)) ? (x) : (y))
#endif

#ifndef ABS
#define ABS(x)                  (((x) >= 0) ? (x) : -(x))
#endif

#ifndef DIFF
#define DIFF(x, y)              (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))
#endif

#ifndef NULL
    #define NULL                0
#endif  // NULL

#ifndef TRUE
    #define TRUE                (0 == 0)
#endif  // TRUE

#ifndef FALSE
    #define FALSE               (0 != 0)
#endif  // FALSE


#ifdef __arm
    #define INLINE              __inline
    #define IRQ                 __irq
    #define FIQ                 __irq
#else
    #define INLINE
    #define IRQ
    #define FIQ
#endif // __arm


#ifndef externC
    #ifdef __cplusplus
        #define externC         extern "C"
    #else
        #define externC         extern
    #endif
#endif  // externC


#ifndef EXTERN
    #ifdef __cplusplus
        #define EXTERN          extern "C"
    #else
        #define EXTERN          extern
    #endif
#endif  // EXTERN


#endif // X_TYPEDEF_H
