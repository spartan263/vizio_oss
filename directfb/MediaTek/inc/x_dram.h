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
#ifndef X_DRAM_H
#define X_DRAM_H

#ifndef CC_ENABLE_DRAM_NO_CHANGE

#ifdef NDEBUG

#define CC_ENABLE_DRAM_NO_CHANGE       1

#else /* NDEBUG */

#if defined(CC_5391_PRELOADER) && defined(NDEBUG)
#define CC_ENABLE_DRAM_NO_CHANGE       1
#else /* defined(CC_5391_PRELOADER) && defined(NDEBUG) */
#define CC_ENABLE_DRAM_NO_CHANGE       0
#endif /* defined(CC_5391_PRELOADER) && defined(NDEBUG) */

#endif /* NDEBUG */
#endif /* CC_ENABLE_DRAM_NO_CHANGE */

// DRAM data read/write macros
#define INIT_DRAM_B_BASE    0x10000000

#define MEM_READ32(offset)              IO_READ32(INIT_DRAM_B_BASE, (offset))
#define MEM_WRITE32(offset, value)      IO_WRITE32(INIT_DRAM_B_BASE, (offset), (value))

// DRAM controller register read/write macros
#define DRAM_READ8(offset)              IO_READ8(DRAM_BASE, (offset))
#define DRAM_READ16(offset)             IO_READ16(DRAM_BASE, (offset))
#define DRAM_READ32(offset)             IO_READ32(DRAM_BASE, (offset))

#define DRAM_WRITE8(offset, value)      IO_WRITE8(DRAM_BASE, (offset), (value))
#define DRAM_WRITE16(offset, value)     IO_WRITE16(DRAM_BASE, (offset), (value))
#define DRAM_WRITE32(offset, value)     IO_WRITE32(DRAM_BASE, (offset), (value))

#define DRAM_REG8(offset)               IO_REG8(DRAM_BASE, (offset))
#define DRAM_REG16(offset)              IO_REG16(DRAM_BASE, (offset))
#define DRAM_REG32(offset)              IO_REG32(DRAM_BASE, (offset))

//----------------------------------------------------------------------------
// DRAM controller register map
#define REG_DRAM_DDR2CTRL               0x7c
    #define FREQ_DIV_2                  (1U << 0)

#define REG_DRAM_MODE                   4

    #define MODE_TURBO_MASK             0x3
    #define MODE_TURBO_SHIFT            4
    #define MODE_TURBO_TURBO            3
    #define MODE_TURBO_NORMAL           0

    #define MODE_BUS_MASK               1
    #define MODE_BUS_SHIFT              0
    #define MODE_BUS_64BIT              1
    #define MODE_BUS_32BIT              0

//----------------------------------------------------------------------------
// DQS definitions
//----------------------------------------------------------------------------
#define GRAY_BIT_NUM        32
#define MAX_DQS_DELAY       128
#define MAX_DQ_DELAY        32
#define CALIB_START         0x100000
#define CALIB_LEN           0x100

//----------------------------------------------------------------------------
// DDR type & PDWNC status auto identify code
// TCM_DRAM_FLAGS  - ((x & 0x3f) * 13.5MHz) is the DDR clock and other flags
// TCM_WAKE_STATUS - The same as REG_RO_PDWAKE_STAT of PDWNC
//----------------------------------------------------------------------------
#define TCM_SRAM_ADDR       (0x20008100)
#define TCM_DRAM_FLAGS      (*((volatile UINT32 *)BIMSWAP(TCM_SRAM_ADDR + 0)))
#define DRAM_CLOCK_MASK     (0x0000003fU)
#define DRAM_DDR_CL_MASK    (0x000003C0U)
#define DRAM_DDR_13_5MHZ    (1U << 22)
#define SYSTEM_AC_ON        (1U << 23)
#define DRAM_FORCE32        (1U << 24)
#define DRAM_FULLFREQ       (1U << 25)
#define DRAM_DDR_QFP        (1U << 26)
#define DRAM_SINGLEDQS      (1U << 27)
#define COLADDR_SHIFT       (28)
#define DRAM_COL_ADDR       (0x0fU << COLADDR_SHIFT)

#define BASE_DDR_CLK        ((TCM_DRAM_FLAGS & DRAM_DDR_13_5MHZ) ? CLK_13_5MHZ : CLK_27MHZ)

#if CC_ENABLE_DRAM_NO_CHANGE
#define TCMGET_DDR_CLK()    (DRVCUST_InitGet(eDdrClock))
#define TCMGET_DDR_CL()     (DRVCUST_InitGet(eDdrCL))
#define IS_SYS_ACON()       ((TCM_DRAM_FLAGS & SYSTEM_AC_ON) ? 1 : 0)
#define IS_FORCE32()        (DRVCUST_InitGet(eFlagForce32bit))
#define IS_FULLFREQ()       (DRVCUST_InitGet(eFlagFullFreq))
#define IS_DDR_QFP()        (DRVCUST_InitGet(eFlagDDRQfp))
#define IS_SINGLEDQS()      (DRVCUST_InitGet(eFlagSingleDQS))
#define TCMGET_COLADDR()    (DRVCUST_InitGet(eDramColAddr))
#else
#define TCMGET_DDR_CLK()    ((TCM_DRAM_FLAGS & DRAM_CLOCK_MASK) * BASE_DDR_CLK)
#define TCMGET_DDR_CL()     ((TCM_DRAM_FLAGS & DRAM_DDR_CL_MASK) >> 6)
#define IS_SYS_ACON()       ((TCM_DRAM_FLAGS & SYSTEM_AC_ON) ? 1 : 0)
#define IS_FORCE32()        ((TCM_DRAM_FLAGS & DRAM_FORCE32) ? 1 : 0)
#define IS_FULLFREQ()       ((TCM_DRAM_FLAGS & DRAM_FULLFREQ) ? 1 : 0)
#define IS_DDR_QFP()        ((TCM_DRAM_FLAGS & DRAM_DDR_QFP) ? 1 : 0)
#define IS_SINGLEDQS()      ((TCM_DRAM_FLAGS & DRAM_SINGLEDQS) ? 1 : 0)
#define TCMGET_COLADDR()    ((TCM_DRAM_FLAGS & DRAM_COL_ADDR) >> COLADDR_SHIFT)
#endif

//lint -emacro({717}, ADD_DDR_CLK)  to disable "do {...} while (0)" lint warning
#define ADD_DDR_CLK()                                                       \
            do {                                                            \
                UINT32 u4AddClk;                                            \
                u4AddClk = (TCM_DRAM_FLAGS & DRAM_CLOCK_MASK);              \
                if ((TCM_DRAM_FLAGS & DRAM_DDR_13_5MHZ) &&                  \
                    (u4AddClk >= 32)) {     /* >= 432MHZ */                 \
                TCM_DRAM_FLAGS &= ~(DRAM_CLOCK_MASK);                       \
                    TCM_DRAM_FLAGS |= 17;   /* Set 459MHZ */                \
                    TCM_DRAM_FLAGS &= ~(DRAM_DDR_13_5MHZ);                  \
                } else if (u4AddClk < DRAM_CLOCK_MASK) {                    \
                    TCM_DRAM_FLAGS &= ~(DRAM_CLOCK_MASK);                   \
                    TCM_DRAM_FLAGS |= ((u4AddClk+1) & DRAM_CLOCK_MASK);     \
                }                                                           \
                TCMSET_CHECKSUM();                                          \
            } while (0)

//lint -emacro({717}, SUB_DDR_CLK)  to disable "do {...} while (0)" lint warning
#define SUB_DDR_CLK()                                                       \
            do {                                                            \
                UINT32 u4SubClk;                                            \
                u4SubClk = (TCM_DRAM_FLAGS & DRAM_CLOCK_MASK);              \
                if (((TCM_DRAM_FLAGS & DRAM_DDR_13_5MHZ)==0) &&             \
                    (u4SubClk <= 17)) {     /* <= 459MHZ */                 \
                    TCM_DRAM_FLAGS &= ~(DRAM_CLOCK_MASK);                   \
                    TCM_DRAM_FLAGS |= 32;   /* Set 432MHZ */                \
                    TCM_DRAM_FLAGS |= (DRAM_DDR_13_5MHZ);                   \
                } else if (u4SubClk > 0) {                                  \
                    TCM_DRAM_FLAGS &= ~(DRAM_CLOCK_MASK);                   \
                    TCM_DRAM_FLAGS |= ((u4SubClk - 1) & DRAM_CLOCK_MASK);   \
                }                                                           \
                TCMSET_CHECKSUM();                                          \
            } while (0)

//lint -emacro({717}, TCMSET_DDR_CLK)  to disable "do {...} while (0)" lint warning
/*
#define TCMSET_DDR_CLK(x)                                                   \
            do {                                                            \
                if ((x) > CLK_405MHZ) {                                     \
                    TCM_DRAM_FLAGS &= ~(DRAM_DDR_13_5MHZ);                  \
                } else {                                                    \
                    TCM_DRAM_FLAGS |= DRAM_DDR_13_5MHZ;                     \
                }                                                           \
                TCM_DRAM_FLAGS &= ~(DRAM_CLOCK_MASK);                       \
                TCM_DRAM_FLAGS |= (((x) / BASE_DDR_CLK) & DRAM_CLOCK_MASK); \
                TCMSET_CHECKSUM();                                          \
            } while (0)
*/

//lint -emacro({717}, TCMSET_SYSACON)  to disable "do {...} while (0)" lint warning
/*
#define TCMSET_SYSACON(x)                               \
            do {                                        \
                if (x) {                                \
                    TCM_DRAM_FLAGS |= SYSTEM_AC_ON;     \
                } else {                                \
                    TCM_DRAM_FLAGS &= ~(SYSTEM_AC_ON);  \
                }                                       \
                TCMSET_CHECKSUM();                      \
            } while (0)
*/

//lint -emacro({717}, TCMSET_FORCE32)  to disable "do {...} while (0)" lint warning
/*
#define TCMSET_FORCE32(x)                               \
            do {                                        \
                if (x) {                                \
                    TCM_DRAM_FLAGS |= DRAM_FORCE32;     \
                } else {                                \
                    TCM_DRAM_FLAGS &= ~(DRAM_FORCE32);  \
                }                                       \
                TCMSET_CHECKSUM();                      \
            } while (0)
*/
//lint -emacro({717}, TCMSET_FULLFREQ)  to disable "do {...} while (0)" lint warning
/*
#define TCMSET_FULLFREQ(x)                              \
            do {                                        \
                if (x) {                                \
                    TCM_DRAM_FLAGS |= DRAM_FULLFREQ;    \
                } else {                                \
                    TCM_DRAM_FLAGS &= ~(DRAM_FULLFREQ); \
                }                                       \
                TCMSET_CHECKSUM();                      \
            } while (0)
*/

//lint -emacro({717}, TCMSET_DDR_QFP)  to disable "do {...} while (0)" lint warning
/*
#define TCMSET_DDR_QFP(x)                               \
            do {                                        \
                if (x) {                                \
                    TCM_DRAM_FLAGS |= DRAM_DDR_QFP;     \
                } else {                                \
                    TCM_DRAM_FLAGS &= ~(DRAM_DDR_QFP);  \
                }                                       \
                TCMSET_CHECKSUM();                      \
            } while (0)
*/

//lint -emacro({717}, TCMSET_SINGLEDQS)  to disable "do {...} while (0)" lint warning
/*
#define TCMSET_SINGLEDQS(x)                             \
            do {                                        \
                if (x) {                                \
                    TCM_DRAM_FLAGS |= DRAM_SINGLEDQS;   \
                } else {                                \
                    TCM_DRAM_FLAGS &= ~(DRAM_SINGLEDQS);    \
                }                                       \
                TCMSET_CHECKSUM();                      \
            } while (0)
*/

//lint -emacro({717}, TCMSET_COLADDR)  to disable "do {...} while (0)" lint warning
/*
#define TCMSET_COLADDR(x)                               \
            do {                                        \
                TCM_DRAM_FLAGS &= ~(DRAM_COL_ADDR);     \
                TCM_DRAM_FLAGS |= (((x) << COLADDR_SHIFT) & DRAM_COL_ADDR); \
                TCMSET_CHECKSUM();                      \
            } while (0)
*/

#define TCM_DRAM_TYPE       (*((volatile UINT32 *)BIMSWAP(TCM_SRAM_ADDR + 4)))
#define DRAM_TYPE_MASK      (0x000000ffU)
#define TYPE_SHIFT          (0)

#if CC_ENABLE_DRAM_NO_CHANGE
#define TCMGET_DRAMTYPE()   DRVCUST_InitGet(eDramType)
#else
#define TCMGET_DRAMTYPE()   ((TCM_DRAM_TYPE & DRAM_TYPE_MASK) >> TYPE_SHIFT)
#endif

//lint -emacro({717}, TCMSET_DRAMTYPE)  to disable "do {...} while (0)" lint warning
/*
#define TCMSET_DRAMTYPE(x)                              \
        do {                                            \
            TCM_DRAM_TYPE &= ~(DRAM_TYPE_MASK);         \
            TCM_DRAM_TYPE |= (x);                       \
            TCMSET_CHECKSUM();                          \
        } while (0)
*/

#define TCM_WAKEUP_STATUS   (*((volatile UINT32 *)BIMSWAP(TCM_SRAM_ADDR + 8)))

#define TCM_CHECKSUM        (*((volatile UINT32 *)BIMSWAP(TCM_SRAM_ADDR + 12)))
#define IS_TCM_CHECKSUM_OK()    (TCM_CHECKSUM == (((TCM_DRAM_FLAGS + TCM_DRAM_TYPE) + TCM_WAKEUP_STATUS) + 0x53815381))

/*
#define TCMSET_CHECKSUM()                               \
        do {                                            \
            TCM_CHECKSUM = ((TCM_DRAM_FLAGS + TCM_DRAM_TYPE) + TCM_WAKEUP_STATUS);    \
        } while (0)
*/


// DDR related functions.
extern CHAR * DDR_DramTypeString(UINT32 u4Type);
extern UINT32 DDR_IsTestDdrOk(void);
extern UINT32 DDR_DetectDDRType(UINT32 u4Clk);
extern UINT32 DDR_CalibrateDqs(void);
extern UINT32 DDR_ResetBits(UINT32 u4Clk, UINT32 fgDDR2,
                UINT32 fgFullFreq, UINT32 fgForce32);
extern void DDR_SetDramController(UINT32 u4Clk, UINT32 fgFull,
                UINT32 fgDDR2, UINT32 fg32Bit, UINT32 fgDQS, UINT32 u4ChipNum);
extern void DDR_SetDdrMode(UINT32 u4Bits, UINT32 fgTurbo);
extern void DDR_SetAgentPriority(const UINT8 *pu1Prio);
extern void DDR_SetBurstLen(UINT32 u4TimeSlot1, UINT32 u4TimeSlot2);
extern void DDR_SetArbiterTime(UINT8 u1Group, UINT8 u1Time);

#endif /* X_DRAM_H */
