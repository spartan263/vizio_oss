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
/*************************************************************************/
/*                                                                       */
/*               Copyright Mentor Graphics Corporation 2002              */
/*                         All Rights Reserved.                          */
/*                                                                       */
/* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS  */
/* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS   */
/* SUBJECT TO LICENSE TERMS.                                             */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* FILE NAME                                  VERSION                    */
/*                                                                       */
/*      nucleus.h                     Nucleus PLUS\Integrator\ADS 1.14.8 */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      System Constants                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains system constants common to both the           */
/*      application and the actual Nucleus PLUS components.  This file   */
/*      also contains data structure definitions that hide internal      */
/*      information from the application.                                */
/*                                                                       */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*      NU_DRIVER                           I/O Driver control block     */
/*      NU_EVENT_GROUP                      Event group control block    */
/*      NU_HISR                             HISR control block           */
/*      NU_MAILBOX                          Mailbox control block        */
/*      NU_MEMORY_POOL                      Memory Pool control block    */
/*      NU_PARTITION_POOL                   Partition Pool control block */
/*      NU_PIPE                             Pipe control block           */
/*      NU_QUEUE                            Queue control block          */
/*      NU_SEMAPHORE                        Semaphore control block      */
/*      NU_TASK                             Task control block           */
/*      NU_TIMER                            Timer control block          */
/*      NU_PROTECT                          Protection structure         */
/*                                                                       */
/* FUNCTIONS                                                             */
/*                                                                       */
/*      None                                                             */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*      None                                                             */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         NAME            DATE                    REMARKS               */
/*                                                                       */
/*      D. Phillips     09-15-2000      Initial release for the ARM      */
/*                                      Integrator/AP                    */
/*      D. Driscoll     06-06-2001      Released 1.13.2                  */
/*                                      Fixed SPRs 82, 157, 742 and 843  */
/*      D. Driscoll     01-04-2002      Released version 1.13.3          */
/*      J. Pregeant     03-27-2002      Released version 1.13.4          */
/*      R. Baum         04-17-2002      Released version 1.13.5          */
/*                                      Added Constants for MMU/Module   */
/*                                      products, modified control block */
/*                                      sizes, added BOOLEAN data type,  */
/*                                      and updated functions to reflect */
/*                                      Module API when applicable.      */
/*      R. Baum         05-31-2002      Released version 1.13.6          */
/*      C. Sheppard     08-30-2002      Released version 1.14.7          */
/*      J. Pregeant     12-27-2002      Released version 1.14.8          */
/*************************************************************************/
/* Check to see if this file has been included already.  */

#ifndef         NUCLEUS

#ifdef          __cplusplus
extern  "C" {                               /* C declarations in C++     */
#endif

#define         NUCLEUS

#define         PLUS_1_11             1
#define         PLUS_1_13             2
#define         PLUS_1_14             3
#define         PLUS_VERSION_COMP     PLUS_1_14

#ifndef         NU_SUPERV_USER_MODE
#define         NU_SUPERV_USER_MODE   0
#endif

#ifndef         NU_MODULE_SUPPORT
#define         NU_MODULE_SUPPORT     0
#endif

#ifndef         NU_MMU_MODE
#define         NU_MMU_MODE           0
#endif


//#define         NU_DEBUG

/* Define the clock frequency that is used by the timer control unit (must be defined in hertz */
// #define NU_System_Clock_Frequency       24000000
#define NU_System_Clock_Frequency       20000000

/* Define the number of Nucleus PLUS ticks that will occur every second.
   By default, the PLUS timer goes off every 10ms, so the PLUS System Clock
   ticks 100 times in a second. */
#define NU_PLUS_Ticks_Per_Second        200

/* Define the number of timer ticks that will occur every second.  This is
   usually the frequency of the timer used by the Nucleus clock.  It may
   be modified by a prescalar.  See the INT assembler file for more
   information about the clock */
#define NU_HW_Ticks_Per_Second          (NU_System_Clock_Frequency / 16)

/* The number of timer ticks between Nucleus timer interrupts that increment
   TMD_System_Clock.  See timer initialization code for details about how to
   determine this number. */
#define NU_HW_Ticks_Per_SW_Tick         (NU_HW_Ticks_Per_Second / NU_PLUS_Ticks_Per_Second)

/* Define NU_COUNT_DOWN if the system timer counts down to 0.  Undefine it if
   the system timer counts up from 0. This macro is used to switch between
   formulas to calculate the number of ticks since the systems started in
   NU_Get_Clock */
#define NU_COUNT_DOWN

/* This macro returns the value of the Nucleus timer (not the system clock)
   in the 32-bit variable, ticks.   The ARM Integrator Timer is a 16-bit timer */
// The following hardware-dependent macro should not be defined here
// #define NU_Retrieve_Hardware_Clock(hw_tick) hw_tick = (0x0000FFFF & (*(UINT32 *)(0x13000104)))

/* Define standard data types.  These definitions allow Nucleus PLUS to
   perform in the same manner on different target platforms.  */

typedef unsigned long           UNSIGNED;
typedef long                    SIGNED;
typedef unsigned char           DATA_ELEMENT;
typedef DATA_ELEMENT            OPTION;
typedef DATA_ELEMENT            BOOLEAN;
typedef int                     STATUS;
typedef unsigned char           UNSIGNED_CHAR;
typedef unsigned int            UNSIGNED_INT;
typedef int                     INT;
typedef unsigned long *         UNSIGNED_PTR;
typedef unsigned char *         BYTE_PTR;

#ifndef VOID
#define VOID void
#endif

#if !defined (_NO_TYPEDEF_BYTE_) && !defined (_TYPEDEF_BYTE_)
typedef unsigned char  BYTE;
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
typedef signed char  CHAR;
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


#define HUGE
#define FAR


/* Define register defines.  R1, R2, R3, and R4 are used in the Nucleus PLUS
   source code in front of variables that are referenced often.  In some
   ports, defining them as "register" will improve performance.  */

#define         R1                              register
#define         R2                              register
#define         R3                              register
#define         R4                              register

/* Describe the static base register, needed for calling services */
/* This also avoids allocating SB (=v6) to any other purpose.     */
__global_reg(6) void *sb;

/* Define the number of accesses required to read or write a pointer data
   type.  This value is used to make optimizations in some ports of Nucleus
   PLUS.  */

#define         NU_POINTER_ACCESS               1


/* Define the padding required for usage of the DATA_ELEMENT type within
   Nucleus PLUS structures.  These values insure proper alignment for the
   next structure member.  */

#define         PAD_1                           3
#define         PAD_2                           2
#define         PAD_3                           1



/* Define constants that are target dependent and/or are used for internal
   purposes.  */

#define         NU_MIN_STACK_SIZE               240
#define         NU_MAX_NAME                     8
#define         NU_MAX_VECTORS                  31
#define         NU_MAX_LISRS                    31


/* Define constants for the number of UNSIGNED words in each of the basic
   system data structures.  */

#if ((NU_SUPERV_USER_MODE == 1)||(NU_MODULE_SUPPORT == 1))

#define         NU_TASK_SIZE                    44
#define         NU_HISR_SIZE                    24

#else

#define         NU_TASK_SIZE                    42
#define         NU_HISR_SIZE                    22

#endif

#define         NU_MAILBOX_SIZE                 15
#define         NU_QUEUE_SIZE                   20
#define         NU_PIPE_SIZE                    20
#define         NU_SEMAPHORE_SIZE               12
#define         NU_EVENT_GROUP_SIZE             11
#define         NU_PARTITION_POOL_SIZE          17
#define         NU_MEMORY_POOL_SIZE             19
#define         NU_TIMER_SIZE                   19
#define         NU_PROTECT_SIZE                 4
#define         NU_DRIVER_SIZE                  5

/* Define what an interrupt vector looks like on the target processor.  */

typedef struct NU_VECTOR_STRUCT
{
    VOID       *pointer;
} NU_VECTOR;


/* Define constants for use in service parameters.  */

#define         NU_AND                          2
#define         NU_AND_CONSUME                  3
#define         NU_DISABLE_TIMER                4
#define         NU_ENABLE_TIMER                 5
#define         NU_FALSE                        0
#define         NU_FIFO                         6
#define         NU_FIXED_SIZE                   7
#define         NU_NO_PREEMPT                   8
#define         NU_NO_START                     9
#define         NU_NO_SUSPEND                   0
#define         NU_NULL                         0
#define         NU_OR                           0
#define         NU_OR_CONSUME                   1
#define         NU_PREEMPT                      10
#define         NU_PRIORITY                     11
#define         NU_START                        12
#define         NU_SUSPEND                      0xFFFFFFFFUL
#define         NU_TRUE                         1
#define         NU_VARIABLE_SIZE                13


/* Define interrupt lockout and enable constants.  */

#define         NU_DISABLE_INTERRUPTS           0xC0
#define         NU_ENABLE_INTERRUPTS            0x00


/* Define task suspension constants.  */

#define         NU_DRIVER_SUSPEND               10
#define         NU_EVENT_SUSPEND                7
#define         NU_FINISHED                     11
#define         NU_MAILBOX_SUSPEND              3
#define         NU_MEMORY_SUSPEND               9
#define         NU_PARTITION_SUSPEND            8
#define         NU_PIPE_SUSPEND                 5
#define         NU_PURE_SUSPEND                 1
#define         NU_QUEUE_SUSPEND                4
#define         NU_READY                        0
#define         NU_SEMAPHORE_SUSPEND            6
#define         NU_SLEEP_SUSPEND                2
#define         NU_TERMINATED                   12

/* Define service completion status constants.  */

#define         NU_SUCCESS                      0
#define         NU_END_OF_LOG                   -1
#define         NU_GROUP_DELETED                -2
#define         NU_INVALID_DELETE               -3
#define         NU_INVALID_DRIVER               -4
#define         NU_INVALID_ENABLE               -5
#define         NU_INVALID_ENTRY                -6
#define         NU_INVALID_FUNCTION             -7
#define         NU_INVALID_GROUP                -8
#define         NU_INVALID_HISR                 -9
#define         NU_INVALID_MAILBOX              -10
#define         NU_INVALID_MEMORY               -11
#define         NU_INVALID_MESSAGE              -12
#define         NU_INVALID_OPERATION            -13
#define         NU_INVALID_PIPE                 -14
#define         NU_INVALID_POINTER              -15
#define         NU_INVALID_POOL                 -16
#define         NU_INVALID_PREEMPT              -17
#define         NU_INVALID_PRIORITY             -18
#define         NU_INVALID_QUEUE                -19
#define         NU_INVALID_RESUME               -20
#define         NU_INVALID_SEMAPHORE            -21
#define         NU_INVALID_SIZE                 -22
#define         NU_INVALID_START                -23
#define         NU_INVALID_SUSPEND              -24
#define         NU_INVALID_TASK                 -25
#define         NU_INVALID_TIMER                -26
#define         NU_INVALID_VECTOR               -27
#define         NU_MAILBOX_DELETED              -28
#define         NU_MAILBOX_EMPTY                -29
#define         NU_MAILBOX_FULL                 -30
#define         NU_MAILBOX_RESET                -31
#define         NU_NO_MEMORY                    -32
#define         NU_NO_MORE_LISRS                -33
#define         NU_NO_PARTITION                 -34
#define         NU_NOT_DISABLED                 -35
#define         NU_NOT_PRESENT                  -36
#define         NU_NOT_REGISTERED               -37
#define         NU_NOT_TERMINATED               -38
#define         NU_PIPE_DELETED                 -39
#define         NU_PIPE_EMPTY                   -40
#define         NU_PIPE_FULL                    -41
#define         NU_PIPE_RESET                   -42
#define         NU_POOL_DELETED                 -43
#define         NU_QUEUE_DELETED                -44
#define         NU_QUEUE_EMPTY                  -45
#define         NU_QUEUE_FULL                   -46
#define         NU_QUEUE_RESET                  -47
#define         NU_SEMAPHORE_DELETED            -48
#define         NU_SEMAPHORE_RESET              -49
#define         NU_TIMEOUT                      -50
#define         NU_UNAVAILABLE                  -51
#define         NU_INVALID_DESCRIPTION          -52
#define         NU_INVALID_REGION               -53
#define         NU_MEMORY_CORRUPT               -54
#define         NU_INVALID_DEBUG_ALLOCATION     -55
#define         NU_EMPTY_DEBUG_ALLOCATION_LIST  -56

/* Define system errors.  */

#define         NU_ERROR_CREATING_TIMER_HISR    1
#define         NU_ERROR_CREATING_TIMER_TASK    2
#define         NU_STACK_OVERFLOW               3
#define         NU_UNHANDLED_INTERRUPT          4
#define         NU_NOT_IN_SUPERVISOR_MODE       5
#define         NU_NOT_ENOUGH_DTLBS             6
#define         NU_NOT_ENOUGH_ITLBS             7


/* Define I/O driver constants.  */

#define         NU_IO_ERROR                     -1
#define         NU_INITIALIZE                   1
#define         NU_ASSIGN                       2
#define         NU_RELEASE                      3
#define         NU_INPUT                        4
#define         NU_OUTPUT                       5
#define         NU_STATUS                       6
#define         NU_TERMINATE                    7


/* Define history entry IDs.  */

#define         NU_USER_ID                      1
#define         NU_CREATE_TASK_ID               2
#define         NU_DELETE_TASK_ID               3
#define         NU_RESET_TASK_ID                4
#define         NU_TERMINATE_TASK_ID            5
#define         NU_RESUME_TASK_ID               6
#define         NU_SUSPEND_TASK_ID              7
#define         NU_RELINQUISH_ID                8
#define         NU_SLEEP_ID                     9
#define         NU_CHANGE_PRIORITY_ID           10
#define         NU_CHANGE_PREEMPTION_ID         11
#define         NU_CREATE_MAILBOX_ID            12
#define         NU_DELETE_MAILBOX_ID            13
#define         NU_RESET_MAILBOX_ID             14
#define         NU_SEND_TO_MAILBOX_ID           15
#define         NU_BROADCAST_TO_MAILBOX_ID      16
#define         NU_RECEIVE_FROM_MAILBOX_ID      17
#define         NU_CREATE_QUEUE_ID              18
#define         NU_DELETE_QUEUE_ID              19
#define         NU_RESET_QUEUE_ID               20
#define         NU_SEND_TO_FRONT_OF_QUEUE_ID    21
#define         NU_SEND_TO_QUEUE_ID             22
#define         NU_BROADCAST_TO_QUEUE_ID        23
#define         NU_RECEIVE_FROM_QUEUE_ID        24
#define         NU_CREATE_PIPE_ID               25
#define         NU_DELETE_PIPE_ID               26
#define         NU_RESET_PIPE_ID                27
#define         NU_SEND_TO_FRONT_OF_PIPE_ID     28
#define         NU_SEND_TO_PIPE_ID              29
#define         NU_BROADCAST_TO_PIPE_ID         30
#define         NU_RECEIVE_FROM_PIPE_ID         31
#define         NU_CREATE_SEMAPHORE_ID          32
#define         NU_DELETE_SEMAPHORE_ID          33
#define         NU_RESET_SEMAPHORE_ID           34
#define         NU_OBTAIN_SEMAPHORE_ID          35
#define         NU_RELEASE_SEMAPHORE_ID         36
#define         NU_CREATE_EVENT_GROUP_ID        37
#define         NU_DELETE_EVENT_GROUP_ID        38
#define         NU_SET_EVENTS_ID                39
#define         NU_RETRIEVE_EVENTS_ID           40
#define         NU_CREATE_PARTITION_POOL_ID     41
#define         NU_DELETE_PARTITION_POOL_ID     42
#define         NU_ALLOCATE_PARTITION_ID        43
#define         NU_DEALLOCATE_PARTITION_ID      44
#define         NU_CREATE_MEMORY_POOL_ID        45
#define         NU_DELETE_MEMORY_POOL_ID        46
#define         NU_ALLOCATE_MEMORY_ID           47
#define         NU_DEALLOCATE_MEMORY_ID         48
#define         NU_CONTROL_SIGNALS_ID           49
#define         NU_RECEIVE_SIGNALS_ID           50
#define         NU_REGISTER_SIGNAL_HANDLER_ID   51
#define         NU_SEND_SIGNALS_ID              52
#define         NU_REGISTER_LISR_ID             53
#define         NU_CREATE_HISR_ID               54
#define         NU_DELETE_HISR_ID               55
#define         NU_CREATE_TIMER_ID              56
#define         NU_DELETE_TIMER_ID              57
#define         NU_CONTROL_TIMER_ID             58
#define         NU_RESET_TIMER_ID               59
#define         NU_CREATE_DRIVER_ID             60
#define         NU_DELETE_DRIVER_ID             61
#define         NU_REQUEST_DRIVER_ID            62
#define         NU_RESUME_DRIVER_ID             63
#define         NU_SUSPEND_DRIVER_ID            64
#define         NU_CHANGE_TIME_SLICE_ID         65
#define         NU_ASSERT_ID                    66
#define         NU_ALLOCATE_ALIGNED_ID          67

/* Define the basic data structure templates.  If the NU_DEBUG conditional
   compilation is specified, the actual structure definition is used.  */

#ifdef          NU_DEBUG
#include        "cs_defs.h"
#include        "tm_defs.h"
#include        "tc_defs.h"
#include        "mb_defs.h"
#include        "qu_defs.h"
#include        "pi_defs.h"
#include        "sm_defs.h"
#include        "ev_defs.h"
#include        "pm_defs.h"
#include        "dm_defs.h"
#endif


#ifndef         NU_DEBUG

/* Define task control data structure with all internal information
   hidden.  */
typedef struct NU_TASK_STRUCT
{
    UNSIGNED      words[NU_TASK_SIZE];
} NU_TASK;
#else

/* Define task control data structure with the actual internal data
   structure.  */
typedef TC_TCB      NU_TASK;
#endif


#ifndef         NU_DEBUG

/* Define HISR control data structure with all internal information
   hidden.  */
typedef struct NU_HISR_STRUCT
{
    UNSIGNED      words[NU_HISR_SIZE];
} NU_HISR;
#else

/* Define HISR control data structure with the actual internal data
   structure.  */
typedef TC_HCB      NU_HISR;
#endif


#ifndef         NU_DEBUG

/* Define mailbox control data structure with all internal information
   hidden.  */
typedef struct NU_MAILBOX_STRUCT
{
    UNSIGNED      words[NU_MAILBOX_SIZE];
} NU_MAILBOX;
#else

/* Define mailbox control data structure with the actual internal data
   structure.  */
typedef MB_MCB      NU_MAILBOX;
#endif


#ifndef         NU_DEBUG

/* Define queue control data structure with all internal information
   hidden.  */
typedef struct NU_QUEUE_STRUCT
{
    UNSIGNED      words[NU_QUEUE_SIZE];
} NU_QUEUE;
#else

/* Define queue control data structure with the actual internal data
   structure.  */
typedef QU_QCB      NU_QUEUE;
#endif


#ifndef         NU_DEBUG

/* Define pipe control data structure with all internal information
   hidden.  */
typedef struct NU_PIPE_STRUCT
{
    UNSIGNED      words[NU_PIPE_SIZE];
} NU_PIPE;
#else

/* Define pipe control data structure with the actual internal data
   structure.  */
typedef PI_PCB      NU_PIPE;
#endif


#ifndef         NU_DEBUG

/* Define semaphore control data structure with all internal information
   hidden.  */
typedef struct NU_SEMAPHORE_STRUCT
{
    UNSIGNED       words[NU_SEMAPHORE_SIZE];
} NU_SEMAPHORE;
#else

/* Define semaphore control data structure with the actual internal data
   structure.  */
typedef SM_SCB      NU_SEMAPHORE;
#endif


#ifndef         NU_DEBUG

/* Define event group control data structure with all internal information
   hidden.  */
typedef struct NU_EVENT_GROUP_STRUCT
{
    UNSIGNED       words[NU_EVENT_GROUP_SIZE];
} NU_EVENT_GROUP;
#else

/* Define event group control data structure with the actual internal data
   structure.  */
typedef EV_GCB      NU_EVENT_GROUP;
#endif


#ifndef         NU_DEBUG

/* Define partition pool control data structure with all internal
   information hidden.  */
typedef struct NU_PARTITION_POOL_STRUCT
{
    UNSIGNED       words[NU_PARTITION_POOL_SIZE];
} NU_PARTITION_POOL;
#else

/* Define partition pool control data structure with the actual internal
   data structure.  */
typedef PM_PCB      NU_PARTITION_POOL;
#endif


#ifndef         NU_DEBUG

/* Define memory pool control data structure with all internal information
   hidden.  */
typedef struct NU_MEMORY_POOL_STRUCT
{
    UNSIGNED       words[NU_MEMORY_POOL_SIZE];
} NU_MEMORY_POOL;
#else

/* Define memory pool control data structure with the actual internal data
   structure.  */
typedef DM_PCB      NU_MEMORY_POOL;
#endif


#ifndef         NU_DEBUG

/* Define timer control data structure with all internal information
   hidden.  */
typedef struct NU_TIMER_STRUCT
{
    UNSIGNED       words[NU_TIMER_SIZE];
} NU_TIMER;
#else

/* Define timer control data structure with the actual internal data
   structure.  */
typedef TM_APP_TCB      NU_TIMER;
#endif


#ifndef         NU_DEBUG

/* Define protect control data structure with all internal information
   hidden.  */
typedef struct NU_PROTECT_STRUCT
{
    UNSIGNED       words[NU_PROTECT_SIZE];
} NU_PROTECT;
#else

/* Define protect control data structure with the actual internal data
   structure.  */
typedef TC_PROTECT      NU_PROTECT;
#endif



/* Define I/O driver request structures.  */

struct NU_INITIALIZE_STRUCT
{
    VOID       *nu_io_address;              /* Base IO address          */
    UNSIGNED    nu_logical_units;           /* Number of logical units  */
    VOID       *nu_memory;                  /* Generic memory pointer   */
    INT         nu_vector;                  /* Interrupt vector number  */
};

struct NU_ASSIGN_STRUCT
{
    UNSIGNED    nu_logical_unit;            /* Logical unit number      */
    INT         nu_assign_info;             /* Additional assign info   */
};

struct NU_RELEASE_STRUCT
{
    UNSIGNED    nu_logical_unit;            /* Logical unit number      */
    INT         nu_release_info;            /* Additional release info  */
};

struct NU_INPUT_STRUCT
{
    UNSIGNED    nu_logical_unit;            /* Logical unit number      */
    UNSIGNED    nu_offset;                  /* Offset of input          */
    UNSIGNED    nu_request_size;            /* Requested input size     */
    UNSIGNED    nu_actual_size;             /* Actual input size        */
    VOID       *nu_buffer_ptr;              /* Input buffer pointer     */
};

struct NU_OUTPUT_STRUCT
{
    UNSIGNED    nu_logical_unit;            /* Logical unit number      */
    UNSIGNED    nu_offset;                  /* Offset of output         */
    UNSIGNED    nu_request_size;            /* Requested output size    */
    UNSIGNED    nu_actual_size;             /* Actual output size       */
    VOID       *nu_buffer_ptr;              /* Output buffer pointer    */
};

struct NU_STATUS_STRUCT
{
    UNSIGNED    nu_logical_unit;            /* Logical unit number      */
    VOID       *nu_extra_status;            /* Additional status ptr    */
};

struct NU_TERMINATE_STRUCT
{
    UNSIGNED    nu_logical_unit;            /* Logical unit number      */
};


/* Define a union of all the different types of request structures. */
typedef  union NU_REQUEST_INFO_UNION
{
    struct NU_INITIALIZE_STRUCT     nu_initialize;
    struct NU_ASSIGN_STRUCT         nu_assign;
    struct NU_RELEASE_STRUCT        nu_release;
    struct NU_INPUT_STRUCT          nu_input;
    struct NU_OUTPUT_STRUCT         nu_output;
    struct NU_STATUS_STRUCT         nu_status;
    struct NU_TERMINATE_STRUCT      nu_terminate;
} nu_request_info_union;


typedef struct NU_DRIVER_REQUEST_STRUCT
{
    INT                   nu_function;                /* I/O request function     */
    UNSIGNED              nu_timeout;                 /* Timeout on request       */
    STATUS                nu_status;                  /* Status of request        */
    UNSIGNED              nu_supplemental;            /* Supplemental information */
    VOID                 *nu_supplemental_ptr;        /* Supplemental info pointer*/
    nu_request_info_union nu_request_info;

} NU_DRIVER_REQUEST;

typedef struct NU_DRIVER_STRUCT
{
    UNSIGNED       words[NU_DRIVER_SIZE];      /* CS_NODE_STRUCT */
    CHAR           nu_driver_name[NU_MAX_NAME];
    VOID          *nu_info_ptr;
    UNSIGNED       nu_driver_id;
    VOID           (*nu_driver_entry)(struct NU_DRIVER_STRUCT *,
                                                NU_DRIVER_REQUEST *);
} NU_DRIVER;



/* Define Nucleus PLUS system interfaces.  */

VOID            Application_Initialize(VOID *first_available_memory);


/* The following area is only applicable to application files and is skipped
   during compilation of Nucleus PLUS source files.  */
#ifndef         NU_SOURCE_FILE

/* Re-map task control functions depending on whether or not error checking
   is specified.  */
#ifdef          NU_NO_ERROR_CHECKING
#define         NU_Create_Task                  TCC_Create_Task
#define         NU_Delete_Task                  TCC_Delete_Task
#define         NU_Reset_Task                   TCC_Reset_Task
#define         NU_Terminate_Task               TCC_Terminate_Task
#define         NU_Resume_Task                  TCC_Resume_Service
#define         NU_Suspend_Task                 TCC_Suspend_Service
#define         NU_Relinquish                   TCC_Relinquish
#define         NU_Sleep                        TCC_Task_Sleep
#define         NU_Change_Priority              TCS_Change_Priority
#define         NU_Change_Preemption            TCS_Change_Preemption
#define         NU_Change_Time_Slice            TCS_Change_Time_Slice
#define         NU_Current_Task_Pointer         TCC_Current_Task_Pointer
#define         NU_Established_Tasks            TCF_Established_Tasks
#define         NU_Task_Information             TCF_Task_Information
#define         NU_Task_Pointers                TCF_Task_Pointers
#define         NU_Create_Mailbox               MBC_Create_Mailbox
#define         NU_Delete_Mailbox               MBC_Delete_Mailbox
#define         NU_Reset_Mailbox                MBS_Reset_Mailbox
#define         NU_Send_To_Mailbox              MBC_Send_To_Mailbox
#define         NU_Broadcast_To_Mailbox         MBS_Broadcast_To_Mailbox
#define         NU_Receive_From_Mailbox         MBC_Receive_From_Mailbox
#define         NU_Established_Mailboxes        MBF_Established_Mailboxes
#define         NU_Mailbox_Information          MBF_Mailbox_Information
#define         NU_Mailbox_Pointers             MBF_Mailbox_Pointers
#define         NU_Create_Queue                 QUC_Create_Queue
#define         NU_Delete_Queue                 QUC_Delete_Queue
#define         NU_Reset_Queue                  QUS_Reset_Queue
#define         NU_Send_To_Front_Of_Queue       QUS_Send_To_Front_Of_Queue
#define         NU_Send_To_Queue                QUC_Send_To_Queue
#define         NU_Broadcast_To_Queue           QUS_Broadcast_To_Queue
#define         NU_Receive_From_Queue           QUC_Receive_From_Queue
#define         NU_Established_Queues           QUF_Established_Queues
#define         NU_Queue_Information            QUF_Queue_Information
#define         NU_Queue_Pointers               QUF_Queue_Pointers
#define         NU_Create_Pipe                  PIC_Create_Pipe
#define         NU_Delete_Pipe                  PIC_Delete_Pipe
#define         NU_Reset_Pipe                   PIS_Reset_Pipe
#define         NU_Send_To_Front_Of_Pipe        PIS_Send_To_Front_Of_Pipe
#define         NU_Send_To_Pipe                 PIC_Send_To_Pipe
#define         NU_Broadcast_To_Pipe            PIS_Broadcast_To_Pipe
#define         NU_Receive_From_Pipe            PIC_Receive_From_Pipe
#define         NU_Established_Pipes            PIF_Established_Pipes
#define         NU_Pipe_Information             PIF_Pipe_Information
#define         NU_Pipe_Pointers                PIF_Pipe_Pointers
#define         NU_Create_Semaphore             SMC_Create_Semaphore
#define         NU_Delete_Semaphore             SMC_Delete_Semaphore
#define         NU_Reset_Semaphore              SMS_Reset_Semaphore
#define         NU_Obtain_Semaphore             SMC_Obtain_Semaphore
#define         NU_Release_Semaphore            SMC_Release_Semaphore
#define         NU_Established_Semaphores       SMF_Established_Semaphores
#define         NU_Semaphore_Information        SMF_Semaphore_Information
#define         NU_Semaphore_Pointers           SMF_Semaphore_Pointers
#define         NU_Create_Event_Group           EVC_Create_Event_Group
#define         NU_Delete_Event_Group           EVC_Delete_Event_Group
#define         NU_Set_Events                   EVC_Set_Events
#define         NU_Retrieve_Events              EVC_Retrieve_Events
#define         NU_Established_Event_Groups     EVF_Established_Event_Groups
#define         NU_Event_Group_Information      EVF_Event_Group_Information
#define         NU_Event_Group_Pointers         EVF_Event_Group_Pointers
#define         NU_Create_Partition_Pool        PMC_Create_Partition_Pool
#define         NU_Delete_Partition_Pool        PMC_Delete_Partition_Pool
#define         NU_Allocate_Partition           PMC_Allocate_Partition
#define         NU_Deallocate_Partition         PMC_Deallocate_Partition
#define         NU_Established_Partition_Pools  PMF_Established_Partition_Pools
#define         NU_Partition_Pool_Information   PMF_Partition_Pool_Information
#define         NU_Partition_Pool_Pointers      PMF_Partition_Pool_Pointers
#define         NU_Create_Memory_Pool           DMC_Create_Memory_Pool
#define         NU_Delete_Memory_Pool           DMC_Delete_Memory_Pool
#define         NU_Allocate_Memory              DMC_Allocate_Memory
#define         NU_Deallocate_Memory            DMC_Deallocate_Memory
//#if defined(NU_MMU_MODE) && (NU_MMU_MODE > 0)
#if 1
#define         NU_Allocate_Aligned_Memory      DMS_Allocate_Aligned_Memory
#endif
#define         NU_Established_Memory_Pools     DMF_Established_Memory_Pools
#define         NU_Memory_Pool_Information      DMF_Memory_Pool_Information
#define         NU_Memory_Pool_Pointers         DMF_Memory_Pool_Pointers
#define         NU_Control_Signals              TCS_Control_Signals
#define         NU_Receive_Signals              TCS_Receive_Signals
#define         NU_Register_Signal_Handler      TCS_Register_Signal_Handler
#define         NU_Send_Signals                 TCS_Send_Signals
#define         NU_Create_HISR                  TCC_Create_HISR
#define         NU_Delete_HISR                  TCC_Delete_HISR
#define         NU_Current_HISR_Pointer         TCC_Current_HISR_Pointer
#define         NU_Established_HISRs            TCF_Established_HISRs
#define         NU_HISR_Pointers                TCF_HISR_Pointers
#define         NU_HISR_Information             TCF_HISR_Information
#define         NU_Set_Clock                    TMT_Set_Clock
#define         NU_Retrieve_Clock               TMT_Retrieve_Clock
#define         NU_Create_Timer                 TMS_Create_Timer
#define         NU_Delete_Timer                 TMS_Delete_Timer
#define         NU_Control_Timer                TMS_Control_Timer
#define         NU_Reset_Timer                  TMS_Reset_Timer
#define         NU_Established_Timers           TMF_Established_Timers
#define         NU_Timer_Pointers               TMF_Timer_Pointers
#define         NU_Timer_Information            TMF_Timer_Information
#define         NU_Get_Remaining_Time           TMF_Get_Remaining_Time
#define         NU_Release_Information          RLC_Release_Information
#define         NU_License_Information          LIC_License_Information
#define         NU_Disable_History_Saving       HIC_Disable_History_Saving
#define         NU_Enable_History_Saving        HIC_Enable_History_Saving
#define         NU_Make_History_Entry           HIC_Make_History_Entry_Service
#define         NU_Retrieve_History_Entry       HIC_Retrieve_History_Entry
#define         NU_Create_Driver                IOC_Create_Driver
#define         NU_Delete_Driver                IOC_Delete_Driver
#define         NU_Request_Driver               IOC_Request_Driver
#define         NU_Resume_Driver                IOC_Resume_Driver
#define         NU_Suspend_Driver               IOC_Suspend_Driver
#define         NU_Established_Drivers          IOF_Established_Drivers
#define         NU_Driver_Pointers              IOF_Driver_Pointers

/* The following routines can only be called from Supervisor mode in
   Supervisor/User mode-switching kernels.  */
#define         NU_Register_LISR                TCC_Register_LISR
#define         NU_Control_Interrupts           TCT_Control_Interrupts
#define         NU_Local_Control_Interrupts     TCT_Local_Control_Interrupts
#define         NU_Restore_Interrupts           TCT_Restore_Interrupts

/* For some targets, these routines may require special Supervisor/User
   mode switching wrappers when used with Supervisor/User mode switching
   versions of the Nucleus PLUS kernel.  The MPC8xx does not require these
   wrappers for the following routines. */
#define         NU_Setup_Vector                 INT_Setup_Vector
#define         NU_Retrieve_Vector              INT_Retrieve_Vector
#define         NU_Vectors_Loaded               INT_Vectors_Loaded

#if (!defined(NU_SUPERV_USER_MODE)) || (NU_SUPERV_USER_MODE < 1)

#define         NU_Activate_HISR                TCT_Activate_HISR
#define         NU_Protect                      TCT_Protect
#define         NU_Unprotect                    TCT_Unprotect
#define         NU_Check_Stack                  TCT_Check_Stack

#else /* NU_SUPERV_USER_MODE defined and greater than zero */

/* These routines require a Supervisor mode change on the MPC8xx */
#define         NU_Activate_HISR                SUC_Activate_HISR
#define         NU_Protect                      SUC_Protect
#define         NU_Unprotect                    SUC_Unprotect
#define         NU_Check_Stack                  SUC_Check_Stack

#endif /* NU_SUPERV_USER_MODE */

#else  /* Error checking functions */

#define         NU_Create_Task                  TCCE_Create_Task
#define         NU_Delete_Task                  TCCE_Delete_Task
#define         NU_Reset_Task                   TCCE_Reset_Task
#define         NU_Terminate_Task               TCCE_Terminate_Task
#define         NU_Resume_Task                  TCCE_Resume_Service
#define         NU_Suspend_Task                 TCCE_Suspend_Service
#define         NU_Relinquish                   TCCE_Relinquish
#define         NU_Sleep                        TCCE_Task_Sleep
#define         NU_Change_Priority              TCSE_Change_Priority
#define         NU_Change_Preemption            TCSE_Change_Preemption
#define         NU_Change_Time_Slice            TCSE_Change_Time_Slice
#define         NU_Current_Task_Pointer         TCC_Current_Task_Pointer
#define         NU_Established_Tasks            TCF_Established_Tasks
#define         NU_Task_Information             TCFE_Task_Information
#define         NU_Task_Pointers                TCF_Task_Pointers
#define         NU_Create_Mailbox               MBCE_Create_Mailbox
#define         NU_Delete_Mailbox               MBCE_Delete_Mailbox
#define         NU_Reset_Mailbox                MBSE_Reset_Mailbox
#define         NU_Send_To_Mailbox              MBCE_Send_To_Mailbox
#define         NU_Broadcast_To_Mailbox         MBSE_Broadcast_To_Mailbox
#define         NU_Receive_From_Mailbox         MBCE_Receive_From_Mailbox
#define         NU_Established_Mailboxes        MBF_Established_Mailboxes
#define         NU_Mailbox_Information          MBF_Mailbox_Information
#define         NU_Mailbox_Pointers             MBF_Mailbox_Pointers
#define         NU_Create_Queue                 QUCE_Create_Queue
#define         NU_Delete_Queue                 QUCE_Delete_Queue
#define         NU_Reset_Queue                  QUSE_Reset_Queue
#define         NU_Send_To_Queue                QUCE_Send_To_Queue
#define         NU_Send_To_Front_Of_Queue       QUSE_Send_To_Front_Of_Queue
#define         NU_Broadcast_To_Queue           QUSE_Broadcast_To_Queue
#define         NU_Receive_From_Queue           QUCE_Receive_From_Queue
#define         NU_Established_Queues           QUF_Established_Queues
#define         NU_Queue_Information            QUF_Queue_Information
#define         NU_Queue_Pointers               QUF_Queue_Pointers
#define         NU_Create_Pipe                  PICE_Create_Pipe
#define         NU_Delete_Pipe                  PICE_Delete_Pipe
#define         NU_Reset_Pipe                   PISE_Reset_Pipe
#define         NU_Send_To_Front_Of_Pipe        PISE_Send_To_Front_Of_Pipe
#define         NU_Send_To_Pipe                 PICE_Send_To_Pipe
#define         NU_Broadcast_To_Pipe            PISE_Broadcast_To_Pipe
#define         NU_Receive_From_Pipe            PICE_Receive_From_Pipe
#define         NU_Established_Pipes            PIF_Established_Pipes
#define         NU_Pipe_Information             PIF_Pipe_Information
#define         NU_Pipe_Pointers                PIF_Pipe_Pointers
#define         NU_Create_Semaphore             SMCE_Create_Semaphore
#define         NU_Delete_Semaphore             SMCE_Delete_Semaphore
#define         NU_Reset_Semaphore              SMSE_Reset_Semaphore
#define         NU_Obtain_Semaphore             SMCE_Obtain_Semaphore
#define         NU_Release_Semaphore            SMCE_Release_Semaphore
#define         NU_Established_Semaphores       SMF_Established_Semaphores
#define         NU_Semaphore_Information        SMF_Semaphore_Information
#define         NU_Semaphore_Pointers           SMF_Semaphore_Pointers
#define         NU_Create_Event_Group           EVCE_Create_Event_Group
#define         NU_Delete_Event_Group           EVCE_Delete_Event_Group
#define         NU_Set_Events                   EVCE_Set_Events
#define         NU_Retrieve_Events              EVCE_Retrieve_Events
#define         NU_Established_Event_Groups     EVF_Established_Event_Groups
#define         NU_Event_Group_Information      EVF_Event_Group_Information
#define         NU_Event_Group_Pointers         EVF_Event_Group_Pointers
#define         NU_Create_Partition_Pool        PMCE_Create_Partition_Pool
#define         NU_Delete_Partition_Pool        PMCE_Delete_Partition_Pool
#define         NU_Allocate_Partition           PMCE_Allocate_Partition
#define         NU_Deallocate_Partition         PMCE_Deallocate_Partition
#define         NU_Established_Partition_Pools  PMF_Established_Partition_Pools
#define         NU_Partition_Pool_Information   PMF_Partition_Pool_Information
#define         NU_Partition_Pool_Pointers      PMF_Partition_Pool_Pointers
#define         NU_Create_Memory_Pool           DMCE_Create_Memory_Pool
#define         NU_Delete_Memory_Pool           DMCE_Delete_Memory_Pool
/* The following conditional routes memory allocation calls to functions
   that help track memory leaks. */
#ifdef NU_DEBUG_MEMORY
#define         NU_Allocate_Memory(a,b,c,d)     ERC_Allocate_Memory(a,b,c,d,__LINE__,__FILE__)

#else
#define         NU_Allocate_Memory              DMCE_Allocate_Memory
#endif /* NU_DEBUG_MEMORY */
//#if defined(NU_MMU_MODE) && (NU_MMU_MODE > 0)
#if 1
#define         NU_Allocate_Aligned_Memory      DMS_Allocate_Aligned_Memory
#endif
/* The following conditional routes memory allocation calls to functions
   that help track memory leaks. */
#ifdef NU_DEBUG_MEMORY
#define         NU_Deallocate_Memory            ERC_Deallocate_Memory
#else
#define         NU_Deallocate_Memory            DMCE_Deallocate_Memory
#endif /* NU_DEBUG_MEMORY */
#define         NU_Established_Memory_Pools     DMF_Established_Memory_Pools
#define         NU_Memory_Pool_Information      DMF_Memory_Pool_Information
#define         NU_Memory_Pool_Pointers         DMF_Memory_Pool_Pointers
#define         NU_Control_Signals              TCSE_Control_Signals
#define         NU_Receive_Signals              TCSE_Receive_Signals
#define         NU_Register_Signal_Handler      TCSE_Register_Signal_Handler
#define         NU_Send_Signals                 TCSE_Send_Signals
#define         NU_Create_HISR                  TCCE_Create_HISR
#define         NU_Delete_HISR                  TCCE_Delete_HISR
#define         NU_Current_HISR_Pointer         TCC_Current_HISR_Pointer
#define         NU_Established_HISRs            TCF_Established_HISRs
#define         NU_HISR_Pointers                TCF_HISR_Pointers
#define         NU_HISR_Information             TCF_HISR_Information
#define         NU_Set_Clock                    TMT_Set_Clock
#define         NU_Retrieve_Clock               TMT_Retrieve_Clock
#define         NU_Create_Timer                 TMSE_Create_Timer
#define         NU_Delete_Timer                 TMSE_Delete_Timer
#define         NU_Control_Timer                TMSE_Control_Timer
#define         NU_Reset_Timer                  TMSE_Reset_Timer
#define         NU_Established_Timers           TMF_Established_Timers
#define         NU_Timer_Pointers               TMF_Timer_Pointers
#define         NU_Timer_Information            TMF_Timer_Information
#define         NU_Get_Remaining_Time           TMF_Get_Remaining_Time
#define         NU_Release_Information          RLC_Release_Information
#define         NU_License_Information          LIC_License_Information
#define         NU_Disable_History_Saving       HIC_Disable_History_Saving
#define         NU_Enable_History_Saving        HIC_Enable_History_Saving
#define         NU_Make_History_Entry           HIC_Make_History_Entry_Service
#define         NU_Retrieve_History_Entry       HIC_Retrieve_History_Entry
#define         NU_Create_Driver                IOCE_Create_Driver
#define         NU_Delete_Driver                IOCE_Delete_Driver
#define         NU_Request_Driver               IOCE_Request_Driver
#define         NU_Resume_Driver                IOCE_Resume_Driver
#define         NU_Suspend_Driver               IOCE_Suspend_Driver
#define         NU_Established_Drivers          IOF_Established_Drivers
#define         NU_Driver_Pointers              IOF_Driver_Pointers

/* The following routines can only be called from Supervisor mode in
   Supervisor/User mode-switching kernels.  */
#define         NU_Register_LISR                TCC_Register_LISR
#define         NU_Control_Interrupts           TCT_Control_Interrupts
#define         NU_Local_Control_Interrupts     TCT_Local_Control_Interrupts
#define         NU_Restore_Interrupts           TCT_Restore_Interrupts

/* For some targets, these routines may require special Supervisor/User
   mode switching wrappers when used with Supervisor/User mode switching
   versions of the Nucleus PLUS kernel.  The MPC8xx does not require these
   wrappers for the following routines. */
#define         NU_Setup_Vector                 INT_Setup_Vector
#define         NU_Retrieve_Vector              INT_Retrieve_Vector
#define         NU_Vectors_Loaded               INT_Vectors_Loaded

#if (!defined(NU_SUPERV_USER_MODE)) || (NU_SUPERV_USER_MODE < 1)

#define         NU_Activate_HISR                TCCE_Activate_HISR
#define         NU_Protect                      TCT_Protect
#define         NU_Unprotect                    TCT_Unprotect
#define         NU_Check_Stack                  TCT_Check_Stack

#else /* NU_SUPERV_USER_MODE defined and greater than zero */

/* These routines require a Supervisor mode change on the MPC8xx */
#define         NU_Activate_HISR                SUC_Activate_HISR
#define         NU_Protect                      SUC_Protect
#define         NU_Unprotect                    SUC_Unprotect
#define         NU_Check_Stack                  SUC_Check_Stack

#endif /* NU_SUPERV_USER_MODE */

#endif

/* Define task control functions.  */

STATUS          NU_Create_Task(NU_TASK *task, CHAR *name,
                        VOID (*task_entry)(UNSIGNED, VOID *), UNSIGNED argc,
                        VOID *argv, VOID *stack_address, UNSIGNED stack_size,
                        OPTION priority, UNSIGNED time_slice,
                        OPTION preempt, OPTION auto_start);
STATUS          NU_Delete_Task(NU_TASK *task);
STATUS          NU_Reset_Task(NU_TASK *task, UNSIGNED argc, VOID *argv);
STATUS          NU_Terminate_Task(NU_TASK *task);
STATUS          NU_Resume_Task(NU_TASK *task);
STATUS          NU_Suspend_Task(NU_TASK *task);
VOID            NU_Relinquish(VOID);
VOID            NU_Sleep(UNSIGNED ticks);
OPTION          NU_Change_Priority(NU_TASK *task, OPTION new_priority);
OPTION          NU_Change_Preemption(OPTION preempt);
UNSIGNED        NU_Change_Time_Slice(NU_TASK *task, UNSIGNED time_slice);
UNSIGNED        NU_Check_Stack(VOID);
NU_TASK        *NU_Current_Task_Pointer(VOID);
UNSIGNED        NU_Established_Tasks(VOID);
STATUS          NU_Task_Information(NU_TASK *task, CHAR *name,
                        DATA_ELEMENT *status, UNSIGNED *scheduled_count,
                        OPTION *priority, OPTION *preempt,
                        UNSIGNED *time_slice, VOID **stack_base,
                        UNSIGNED *stack_size, UNSIGNED *minimum_stack);
UNSIGNED        NU_Task_Pointers(NU_TASK **pointer_list,
                                                UNSIGNED maximum_pointers);

/* Define Mailbox management functions.  */

STATUS          NU_Create_Mailbox(NU_MAILBOX *mailbox, CHAR *name,
                                                OPTION suspend_type);
STATUS          NU_Delete_Mailbox(NU_MAILBOX *mailbox);
STATUS          NU_Reset_Mailbox(NU_MAILBOX *mailbox);
STATUS          NU_Send_To_Mailbox(NU_MAILBOX *mailbox, VOID *message,
                                                        UNSIGNED suspend);
STATUS          NU_Broadcast_To_Mailbox(NU_MAILBOX *mailbox, VOID *message,
                                                        UNSIGNED suspend);
STATUS          NU_Receive_From_Mailbox(NU_MAILBOX *mailbox, VOID *message,
                                                        UNSIGNED suspend);
UNSIGNED        NU_Established_Mailboxes(VOID);
STATUS          NU_Mailbox_Information(NU_MAILBOX *mailbox, CHAR *name,
                  OPTION *suspend_type, OPTION *message_present,
                  UNSIGNED *tasks_waiting, NU_TASK **first_task);
UNSIGNED        NU_Mailbox_Pointers(NU_MAILBOX **pointer_list,
                                                UNSIGNED maximum_pointers);

/* Define Queue management functions.  */

STATUS          NU_Create_Queue(NU_QUEUE *queue, CHAR *name,
                      VOID *start_address, UNSIGNED queue_size,
                      OPTION message_type, UNSIGNED message_size,
                      OPTION suspend_type);
STATUS          NU_Delete_Queue(NU_QUEUE *queue);
STATUS          NU_Reset_Queue(NU_QUEUE *queue);
STATUS          NU_Send_To_Front_Of_Queue(NU_QUEUE *queue, VOID *message,
                                        UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Send_To_Queue(NU_QUEUE *queue, VOID *message,
                                        UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Broadcast_To_Queue(NU_QUEUE *queue, VOID *message,
                                        UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Receive_From_Queue(NU_QUEUE *queue, VOID *message,
                      UNSIGNED size, UNSIGNED *actual_size, UNSIGNED suspend);
UNSIGNED        NU_Established_Queues(VOID);
STATUS          NU_Queue_Information(NU_QUEUE *queue, CHAR *name,
                  VOID **start_address, UNSIGNED *queue_size,
                  UNSIGNED *available, UNSIGNED *messages,
                  OPTION *message_type, UNSIGNED *message_size,
                  OPTION *suspend_type, UNSIGNED *tasks_waiting,
                  NU_TASK **first_task);
UNSIGNED        NU_Queue_Pointers(NU_QUEUE **pointer_list,
                                                UNSIGNED maximum_pointers);

/* Define Pipe management functions.  */

STATUS          NU_Create_Pipe(NU_PIPE *pipe, CHAR *name,
                      VOID *start_address, UNSIGNED pipe_size,
                      OPTION message_type, UNSIGNED message_size,
                      OPTION suspend_type);
STATUS          NU_Delete_Pipe(NU_PIPE *pipe);
STATUS          NU_Reset_Pipe(NU_PIPE *pipe);
STATUS          NU_Send_To_Front_Of_Pipe(NU_PIPE *pipe, VOID *message,
                                        UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Send_To_Pipe(NU_PIPE *pipe, VOID *message,
                                        UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Broadcast_To_Pipe(NU_PIPE *pipe, VOID *message,
                                        UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Receive_From_Pipe(NU_PIPE *pipe, VOID *message,
                      UNSIGNED size, UNSIGNED *actual_size, UNSIGNED suspend);
UNSIGNED        NU_Established_Pipes(VOID);
STATUS          NU_Pipe_Information(NU_PIPE *pipe, CHAR *name,
                  VOID **start_address, UNSIGNED *pipe_size,
                  UNSIGNED *available, UNSIGNED *messages,
                  OPTION *message_type, UNSIGNED *message_size,
                  OPTION *suspend_type, UNSIGNED *tasks_waiting,
                  NU_TASK **first_task);
UNSIGNED        NU_Pipe_Pointers(NU_PIPE **pointer_list,
                                                UNSIGNED maximum_pointers);

/* Define Semaphore management functions.  */

STATUS          NU_Create_Semaphore(NU_SEMAPHORE *semaphore, CHAR *name,
                        UNSIGNED initial_count, OPTION suspend_type);
STATUS          NU_Delete_Semaphore(NU_SEMAPHORE *semaphore);
STATUS          NU_Reset_Semaphore(NU_SEMAPHORE *semaphore,
                                        UNSIGNED initial_count);
STATUS          NU_Obtain_Semaphore(NU_SEMAPHORE *semaphore, UNSIGNED suspend);
STATUS          NU_Release_Semaphore(NU_SEMAPHORE *semaphore);
UNSIGNED        NU_Established_Semaphores(VOID);
STATUS          NU_Semaphore_Information(NU_SEMAPHORE *semaphore, CHAR *name,
                  UNSIGNED *current_count, OPTION *suspend_type,
                  UNSIGNED *tasks_waiting, NU_TASK **first_task);
UNSIGNED        NU_Semaphore_Pointers(NU_SEMAPHORE **pointer_list,
                                                UNSIGNED maximum_pointers);

/*
 * The following two functions are added to support binary semaphore.
 * The only differences between counting and binary are the create
 * and release functions only. So other functions except these two reuse
 * originals.
 * -xavier
 */
STATUS          NU_Create_Binary_Semaphore(NU_SEMAPHORE *semaphore, CHAR *name,
                        UNSIGNED initial_count, OPTION suspend_type);
STATUS          NU_Release_Binary_Semaphore(NU_SEMAPHORE *semaphore);

/* Define Event Group management functions.  */

STATUS          NU_Create_Event_Group(NU_EVENT_GROUP *group, CHAR *name);
STATUS          NU_Delete_Event_Group(NU_EVENT_GROUP *group);
STATUS          NU_Set_Events(NU_EVENT_GROUP *group, UNSIGNED events,
                                                OPTION operation);
STATUS          NU_Retrieve_Events(NU_EVENT_GROUP *group,
                        UNSIGNED requested_flags, OPTION operation,
                        UNSIGNED *retrieved_flags, UNSIGNED suspend);
UNSIGNED        NU_Established_Event_Groups(VOID);
STATUS          NU_Event_Group_Information(NU_EVENT_GROUP *group, CHAR *name,
                  UNSIGNED *event_flags, UNSIGNED *tasks_waiting,
                  NU_TASK **first_task);
UNSIGNED        NU_Event_Group_Pointers(NU_EVENT_GROUP **pointer_list,
                                                UNSIGNED maximum_pointers);

/* Define Signal processing functions.  */

UNSIGNED        NU_Control_Signals(UNSIGNED signal_enable_mask);
UNSIGNED        NU_Receive_Signals(VOID);
STATUS          NU_Register_Signal_Handler(VOID (*signal_handler)(UNSIGNED));
STATUS          NU_Send_Signals(NU_TASK *task, UNSIGNED signals);

/* Define Partition memory management functions.  */

STATUS          NU_Create_Partition_Pool(NU_PARTITION_POOL *pool, CHAR *name,
                        VOID *start_address, UNSIGNED pool_size,
                        UNSIGNED partition_size, OPTION suspend_type);
STATUS          NU_Delete_Partition_Pool(NU_PARTITION_POOL *pool);
STATUS          NU_Allocate_Partition(NU_PARTITION_POOL *pool,
                                VOID **return_pointer, UNSIGNED suspend);
STATUS          NU_Deallocate_Partition(VOID *partition);
UNSIGNED        NU_Established_Partition_Pools(VOID);
STATUS          NU_Partition_Pool_Information(NU_PARTITION_POOL *pool,
                        CHAR *name,
                  VOID **start_address, UNSIGNED *pool_size,
                  UNSIGNED *partition_size, UNSIGNED *available,
                  UNSIGNED *allocated, OPTION *suspend_type,
                  UNSIGNED *tasks_waiting, NU_TASK **first_task);
UNSIGNED        NU_Partition_Pool_Pointers(NU_PARTITION_POOL **pointer_list,
                                                UNSIGNED maximum_pointers);

/* Define Dynamic memory management functions.  */

STATUS          NU_Create_Memory_Pool(NU_MEMORY_POOL *pool, CHAR *name,
                        VOID *start_address, UNSIGNED pool_size,
                        UNSIGNED min_allocation, OPTION suspend_type);
STATUS          NU_Delete_Memory_Pool(NU_MEMORY_POOL *pool);
#ifdef NU_DEBUG_MEMORY
STATUS          ERC_Allocate_Memory(NU_MEMORY_POOL *pool, VOID **return_pointer,
                                UNSIGNED size, UNSIGNED suspend,
                                unsigned long line, const char* file);
#else
STATUS          NU_Allocate_Memory(NU_MEMORY_POOL *pool, VOID **return_pointer,
                                UNSIGNED size, UNSIGNED suspend);
#endif /* NU_DEBUG_MEMORY */
//#if defined(NU_MMU_MODE) && (NU_MMU_MODE > 0)
#if 1
STATUS NU_Allocate_Aligned_Memory(NU_MEMORY_POOL *pool_ptr,
                                  VOID **return_pointer, UNSIGNED size,
                                  UNSIGNED alignment, UNSIGNED suspend);
#endif

STATUS          NU_Deallocate_Memory(VOID *memory);
UNSIGNED        NU_Established_Memory_Pools(VOID);
STATUS          NU_Memory_Pool_Information(NU_MEMORY_POOL *pool, CHAR *name,
                  VOID **start_address, UNSIGNED *pool_size,
                  UNSIGNED *min_allocation, UNSIGNED *available,
                  OPTION *suspend_type, UNSIGNED *tasks_waiting,
                  NU_TASK **first_task);
UNSIGNED        NU_Memory_Pool_Pointers(NU_MEMORY_POOL **pointer_list,
                                                UNSIGNED maximum_pointers);

/* Define Interrupt management functions.  */

INT             NU_Control_Interrupts(INT new_level);
INT             NU_Local_Control_Interrupts(INT new_level);
VOID            NU_Restore_Interrupts(VOID);
VOID           *NU_Setup_Vector(INT vector, VOID *new_vector);
STATUS          NU_Register_LISR(INT vector,
                                VOID (*new_lisr)(INT),
                                        VOID (**old_lisr)(INT));
STATUS          NU_Activate_HISR(NU_HISR *hisr);
STATUS          NU_Create_HISR(NU_HISR *hisr, CHAR *name,
                        VOID (*hisr_entry)(VOID), OPTION priority,
                        VOID *stack_address, UNSIGNED stack_size);
STATUS          NU_Delete_HISR(NU_HISR *hisr);
NU_HISR        *NU_Current_HISR_Pointer(VOID);
UNSIGNED        NU_Established_HISRs(VOID);
STATUS          NU_HISR_Information(NU_HISR *hisr, CHAR *name,
                        UNSIGNED *scheduled_count, DATA_ELEMENT *priority,
                        VOID **stack_base, UNSIGNED *stack_size,
                        UNSIGNED *minimum_stack);
UNSIGNED        NU_HISR_Pointers(NU_HISR **pointer_list,
                                                UNSIGNED maximum_pointers);
VOID            NU_Protect(NU_PROTECT *protect_struct);
VOID            NU_Unprotect(VOID);

/* Timer management functions.  */

STATUS          NU_Create_Timer(NU_TIMER *timer, CHAR *name,
                        VOID (*expiration_routine)(UNSIGNED), UNSIGNED id,
                        UNSIGNED initial_time, UNSIGNED reschedule_time,
                        OPTION enable);
STATUS          NU_Delete_Timer(NU_TIMER *timer);
STATUS          NU_Reset_Timer(NU_TIMER *timer,
                        VOID (*expiration_routine)(UNSIGNED),
                        UNSIGNED initial_time, UNSIGNED reschedule_timer,
                        OPTION enable);
STATUS          NU_Control_Timer(NU_TIMER *timer, OPTION enable);
UNSIGNED        NU_Established_Timers(VOID);
STATUS          NU_Timer_Information(NU_TIMER *timer, CHAR *name,
                  OPTION *enable, UNSIGNED *expirations, UNSIGNED *id,
                  UNSIGNED *initial_time, UNSIGNED *reschedule_time);
UNSIGNED        NU_Timer_Pointers(NU_TIMER **pointer_list,
                                                UNSIGNED maximum_pointers);
VOID            NU_Set_Clock(UNSIGNED new_value);
UNSIGNED        NU_Retrieve_Clock(VOID);

/* Development support functions.  */

CHAR           *NU_Release_Information(VOID);
CHAR           *NU_License_Information(VOID);
VOID            NU_Disable_History_Saving(VOID);
VOID            NU_Enable_History_Saving(VOID);
VOID            NU_Make_History_Entry(UNSIGNED param1, UNSIGNED param2,
                                                UNSIGNED param3);
STATUS          NU_Retrieve_History_Entry(DATA_ELEMENT *id,
                        UNSIGNED *param1, UNSIGNED *param2, UNSIGNED *param3,
                        UNSIGNED *time, NU_TASK **task, NU_HISR **hisr);

/* Input/Output Driver functions.  */

STATUS          NU_Create_Driver(NU_DRIVER *driver, CHAR *name,
                    VOID (*driver_entry)(NU_DRIVER *, NU_DRIVER_REQUEST *));
STATUS          NU_Delete_Driver(NU_DRIVER *driver);
STATUS          NU_Request_Driver(NU_DRIVER *driver,
                                                NU_DRIVER_REQUEST *request);
STATUS          NU_Resume_Driver(NU_TASK *task);
STATUS          NU_Suspend_Driver(VOID (*terminate_routine)(VOID *),
                                        VOID *information, UNSIGNED timeout);
UNSIGNED        NU_Established_Drivers(VOID);
UNSIGNED        NU_Driver_Pointers(NU_DRIVER **pointer_list,
                                                UNSIGNED maximum_pointers);


#endif

/* Define Supervisor and User mode functions */
#if (!defined(NU_SUPERV_USER_MODE)) || (NU_SUPERV_USER_MODE < 1)

#define NU_IS_SUPERVISOR_MODE() (NU_TRUE)
#define NU_SUPERVISOR_MODE() ((void) 0)
#define NU_USER_MODE() ((void) 0)
#define NU_SUPERV_USER_VARIABLES    /* Not a Supervisor/User kernel */

#else /* NU_SUPERV_USER_MODE defined */

#include "tc_defs.h"
#include "mmu/inc/su_extr.h"

#endif /* NU_SUPERV_USER_MODE */


#ifdef          __cplusplus
}                                           /* End of C declarations     */
#endif

#endif  /* !NUCLEUS */

