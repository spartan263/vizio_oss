/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
//lint -emacro(534,LOG)

//lint -save
//lint -e960
//lint -e961

#ifndef __X_DEBUG_H__
#define __X_DEBUG_H__

#include "x_typedef.h"
#include "x_common.h"
#include "x_printf.h"

#ifdef SYSTEM_LOG
#include "x_syslog.h"
#endif


/******************************************************************************
*	To define NDEBUG will disable leveling-LOG/Print to console mechanism.
* Instead, LOG level 0, 1, 2, and 3 will print to console directly.  The other
* log level will not exist in binary while compiling with NDEBUG definition.
******************************************************************************/


#ifndef DEFINE_IS_LOG
#define DEFINE_IS_LOG   CLI_IsLog
#endif

#ifndef DEFINE_VLOG
#define DEFINE_VLOG     CLI_vLog
#endif


/******************************************************************************/
#ifdef NDEBUG

#define LOG(level, fmt...)		LOG_##level(fmt)
#define LOG_0(fmt...)			UTIL_Log(fmt)
#define LOG_1(fmt...)
#define LOG_2(fmt...)
#define LOG_3(fmt...)
#define LOG_4(fmt...)
#define LOG_5(fmt...)
#define LOG_6(fmt...)
#define LOG_7(fmt...)
#define LOG_8(fmt...)
#define LOG_9(fmt...)
#define LOG_10(fmt...)
#define LOG_11(fmt...)

#define LOG__R(fmt...)
#define LOG__W(fmt...)
#define LOG__B(fmt...)
#define LOG__I(fmt...)
#define LOG__S(fmt...)

#define LOG_E(fmt...)               UTIL_Log(fmt)
#define LOG_W(fmt...)
#define LOG_I(fmt...)
#define LOG_N(fmt...)
#define LOG_D(fmt...)
#define LOG_V(fmt...)

#define LOG_LOG_ERROR(fmt...)       UTIL_Log(fmt)
#define LOG_LOG_WARN(fmt...)
#define LOG_LOG_INFO(fmt...)
#define LOG_LOG_NOTE(fmt...)
#define LOG_LOG_DEBUG(fmt...)

#else

//lint -emacro({717}, LOG)  to disable "do {...} while (0)" lint warning
#define LOG(level, fmt...)			(DEFINE_IS_LOG(level, fmt))
#define VLOG(level, fmt, valist)	(DEFINE_VLOG(level, fmt, valist))
#define LOG_E(fmt...)               LOG(1, fmt)
#define LOG_W(fmt...)               LOG(3, fmt)
#define LOG_I(fmt...)               LOG(5, fmt)
#define LOG_N(fmt...)               LOG(7, fmt)
#define LOG_D(fmt...)               LOG(9, fmt)
#define LOG_V(fmt...)               LOG(11, fmt)

#endif
/******************************************************************************/


#define LOG_ERROR	1
#define LOG_WARN	3
#define LOG_INFO	5
#define LOG_NOTE	7
#define LOG_DEBUG	9

#define _R			((INT32) 1 << 8)
#define _W			((INT32) 1 << 9)
#define _B			((INT32) 1 << 10)
#define _I			((INT32) 1 << 11)
#define _S			((INT32) 1 << 12)
#define LOG_BIT_MASK		(_R | _W | _B | _I | _S)

#define _M			((INT32) 1 << 16)		/* Enable Thread module name display. */
#define _T			((INT32) 1 << 17)		/* Enable Thread id display. */
#define _L			((INT32) 1 << 18)		/* Enable the log data to log thread. */
#define _X			((INT32) 1 << 19)		/* Enable Time display, 1 unit is 1ms. */
#define LOG_HEADER_MASK		(_M | _T | _X)

/******************************************************************************
*    CLIMOD_DEBUG_CLIENTRY(mod) macro is for a CLI structure to insert its own
* debug level command, debug_on, debug_off, and debug_level.
*    CLIMOD_DEBUG_FUNCTIONS(mod) macro is for a CLI module to implement its own
* debug level setting functions which are mapping to debug_on, debug_off, and
* debug_level commands respectively.
*
*    To use these two macros, user must put CLIMOD_DEBUG_FUNCTIONS(mod) before
* the CLI item list, and then in CLI item list, user can insert CLIMOD_DEBUG_CLIENTRY(mod)
* in the structure.  For example, in file cli_foo.c, which is a CLI program to
* debug the low level module, FOO.
*
* ----------------------------
* ......
* CLIMOD_DEBUG_FUNCTIONS(FOO)
* ......
* CLI_EXEC_T arFOOCmdTbl[] = {
*     { ...... },
*     CLIMOD_DEBUG_CLIENTRY(FOO),
*     { ...... },
* }
* ......
* ----------------------------
*    To support these two macro, the low level module FOO implementation must
* provide a function calls, INT32 FOO_i4LogLvlQuerySet(INT32 *pData), to the
* log level of the low level module FOO.
******************************************************************************/

/**********/
#define CLIMOD_DEBUG_CLIENTRY(mod)																		\
	{ "debug_on", "d_on", _##mod##CLI_DebugOn, NULL, #mod ".d_on", CLI_SUPERVISOR },					\
	{ "debug_off", "d_off", _##mod##CLI_DebugOff, NULL, #mod ".d_off", CLI_SUPERVISOR },				\
	{ "debug_level", "d_l", _##mod##CLI_DebugLevel, NULL, #mod ".d_l", CLI_SUPERVISOR }

/**********/
#define CLIMOD_DEBUG_FUNCTIONS(mod)																		\
																										\
static INT32 _i4##mod##DebugLevel = -1;																	\
																										\
static INT32 _##mod##CLI_DebugOn(INT32 i4Argc, const CHAR ** szArgv)									\
{																										\
	INT32 i4DbgLvl;																						\
																										\
	i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);															\
	if (i4DbgLvl == 0) {																				\
		if (_i4##mod##DebugLevel <= 0) {																\
			/* no value reference, set log level 1. */													\
			i4DbgLvl = 1;																				\
			mod##_i4LogLvlQuerySet(&i4DbgLvl);															\
		} else {																						\
			/* set previous log level. */																\
			mod##_i4LogLvlQuerySet(&_i4##mod##DebugLevel);												\
		}																								\
	} /* else the current status is on. do nothing. */													\
																										\
	Printf(#mod " module debug level is turn on as %s.\n", 												\
			UTILCLI_LogLevelText(mod##_i4LogLvlQuerySet(NULL)));										\
    return 0;																							\
}																										\
																										\
static INT32 _##mod##CLI_DebugOff(INT32 i4Argc, const CHAR ** szArgv)									\
{																										\
	INT32 i4DbgLvl = 0;																					\
																										\
	_i4##mod##DebugLevel = mod##_i4LogLvlQuerySet(NULL);												\
	mod##_i4LogLvlQuerySet(&i4DbgLvl);																	\
	Printf(#mod " module debug level is turn off as 0.\n");												\
    return 0;																							\
}																										\
																										\
static INT32 _##mod##CLI_DebugLevel(INT32 i4Argc, const CHAR ** szArgv)									\
{																										\
/*	INT32 i, i4DbgLvl; */																				\
																										\
	UTILCLI_DebugLevel(mod##_i4LogLvlQuerySet, i4Argc, szArgv);											\
/*	for (i=1; i<i4Argc; i++) {																			\
		if ((szArgv[i][0] == '+') || (szArgv[i][0] == '-')) {											\
			switch(szArgv[i][1]) {																		\
			case 'R': case 'r':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _R) == 0) { i4DbgLvl += _R; }									\
				} else {																				\
						if ((i4DbgLvl & _R) != 0) { i4DbgLvl -= _R; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'W': case 'w':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _W) == 0) { i4DbgLvl += _W; }									\
				} else {																				\
						if ((i4DbgLvl & _W) != 0) { i4DbgLvl -= _W; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'B': case 'b':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _B) == 0) { i4DbgLvl += _B; }									\
				} else {																				\
						if ((i4DbgLvl & _B) != 0) { i4DbgLvl -= _B; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'I': case 'i':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _I) == 0) { i4DbgLvl += _I; }									\
				} else {																				\
						if ((i4DbgLvl & _I) != 0) { i4DbgLvl -= _I; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'M': case 'm':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _M) == 0) { i4DbgLvl += _M; }									\
				} else {																				\
						if ((i4DbgLvl & _M) != 0) { i4DbgLvl -= _M; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'T': case 't':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _T) == 0) { i4DbgLvl += _T; }									\
				} else {																				\
						if ((i4DbgLvl & _T) != 0) { i4DbgLvl -= _T; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			case 'L': case 'l':																			\
				i4DbgLvl = mod##_i4LogLvlQuerySet(NULL);												\
				if (szArgv[i][0] == '+') {																\
						if ((i4DbgLvl & _L) == 0) { i4DbgLvl += _L; }									\
				} else {																				\
						if ((i4DbgLvl & _L) != 0) { i4DbgLvl -= _L; }									\
				}																						\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
				break;																					\
			default:																					\
				Printf(#mod " %s is not support.\n", szArgv[i]);										\
				break;																					\
			}																							\
		} else {																						\
			if (szArgv[i][0] >= '0' && szArgv[i][0] <='9') {											\
				i4DbgLvl = (StrToInt(szArgv[i]) & 0x0ff) | (mod##_i4LogLvlQuerySet(NULL) & 0xffff00);	\
				mod##_i4LogLvlQuerySet(&i4DbgLvl);														\
			}																							\
		}																								\
	} */																								\
	Printf(#mod " module debug level is %s now.\n", 													\
			UTILCLI_LogLevelText(mod##_i4LogLvlQuerySet(NULL)));										\
																										\
    return 0;																							\
}


/******************************************************************************
*      To support these two macro, this file of FOO module needs to define
* FOO_BASE, FOO_REG_LENGTH, and arFOORgtList.
******************************************************************************/

/**********/
#define CLIMOD_REGTST_CLIENTRY(mod)																		\
    { "reg_dft_chk", "regc", _##mod##CLI_RegDftChk, NULL, #mod ".regc", CLI_SUPERVISOR},				\
    { "regrw_test", "regt", _##mod##CLI_RegRWTest, NULL, #mod ".regt", CLI_SUPERVISOR},					\
    { "reg_space_test", "spt", _##mod##CLI_SpaceRWTest, NULL, #mod ".spt", CLI_SUPERVISOR}

/**********/
#define CLIMOD_REGTST_FUNCTIONS(mod)																	\
																										\
static INT32 _##mod##CLI_RegDftChk(INT32 argc, const CHAR **argv)										\
{																										\
    INT32 i4Ret;																						\
																										\
    i4Ret = UTIL_RegDefChk(mod##_BASE, ar##mod##RgtList);												\
	Printf("%s return %d\n", __FUNCTION__, i4Ret);														\
    return i4Ret;																						\
}																										\
																										\
static INT32 _##mod##CLI_SpaceRWTest(INT32 argc, const CHAR **argv)										\
{																										\
    INT32 i4Ret;																						\
																										\
    i4Ret = UTIL_AllSpaceRWTest(mod##_BASE, mod##_REG_LENGTH);										\
	Printf("%s return %d\n", __FUNCTION__, i4Ret);														\
    return i4Ret;																						\
}																										\
																										\
static INT32 _##mod##CLI_RegRWTest(INT32 argc, const CHAR **argv)										\
{																										\
    INT32 i4Ret;																						\
																										\
    i4Ret = UTIL_RegRWTest(mod##_BASE, ar##mod##RgtList);												\
	Printf("%s return %d\n", __FUNCTION__, i4Ret);														\
    return i4Ret;																						\
}

/**********/
#define DECLARE_G_SUBLIST(module, cmd, ab, msg)    \
     {                                             \
        #cmd,                    /* cmd name */    \
        #ab,                     /* cmd abname */  \
        NULL,                    /* function */    \
        ar##module##CmdTbl,      /* sublist */     \
        msg,                     /* msg */         \
        CLI_GUEST                /* permission */  \
    }

/**********/
#define DECLARE_G_CMD(func, cmd, ab, msg)   \
    {                                       \
        #cmd,           /* cmd name */      \
        #ab,            /* cmd abname */    \
        func,           /* function */      \
        NULL,           /* sublist */       \
        msg,            /* msg */           \
        CLI_GUEST       /* permission */    \
    }

/**********/
#define DECLARE_SUBLIST(module, cmd, ab, msg)      \
     {                                             \
        #cmd,                    /* cmd name */    \
        #ab,                     /* cmd abname */  \
        NULL,                    /* function */    \
        ar##module##CmdTbl,      /* sublist */     \
        msg,                     /* msg */         \
        CLI_SUPERVISOR           /* permission */  \
    }

/**********/
#define DECLARE_CMD(func, cmd, ab, msg)     \
    {                                       \
        #cmd,           /* cmd name */      \
        #ab,            /* cmd abname */    \
        func,           /* function */      \
        NULL,           /* sublist */       \
        msg,            /* msg */           \
        CLI_SUPERVISOR  /* permission */    \
    }

/**********/
#define DECLARE_END_ITEM()                              \
    {                                                   \
        NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR    \
    }


/******************************************************************************
* Below are the declarations of Log level access functions of all modules.
******************************************************************************/
typedef INT32 (* FN_LQS_T)(const INT32 *pi4Data);

EXTERN UINT8 *UTILCLI_LogLevelText(INT32 i4LogLevel);
EXTERN void UTILCLI_DebugLevel(FN_LQS_T i4LogLvlQuerySet, INT32 i4Argc, const CHAR ** szArgv);
EXTERN BOOL UTIL_SetLogFlag(BOOL fgSet);
EXTERN INT32 UTIL_Log(const CHAR *szFmt, ...);
EXTERN INT32 UTIL_vLog(const CHAR *szFmt, VA_LIST t_ap);
EXTERN void UTIL_FlushSLog(void);
EXTERN void UTIL_LogFlush(void);

#define LOG_MOD_DECLARE(mod)											\
	EXTERN INT32 mod##_i4LogLvlQuerySet(INT32 const *pi4Data);			\
	EXTERN INT32 mod##_IsLog(INT32 i4Level, const CHAR *szFmt, ...);	\
	EXTERN INT32 mod##_vLog(INT32 i4Level, const CHAR *szFmt, VA_LIST t_ap)		\

LOG_MOD_DECLARE(VDP);
LOG_MOD_DECLARE(AUD);
LOG_MOD_DECLARE(UART);
LOG_MOD_DECLARE(IR);
LOG_MOD_DECLARE(NOR);
LOG_MOD_DECLARE(SIF);
LOG_MOD_DECLARE(CLI);
LOG_MOD_DECLARE(NPTV);


#endif /* __X_DEBUG_H__ */

//lint -restore

