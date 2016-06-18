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
/*-----------------------------------------------------------------------------
 *
 * $Author: dtvbm11 $
 * $Date: 2016/03/31 $
 * $RCSfile: x_mid.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file x_mid.h
 *  x_mid.h Memory intrusion detection module
 */

#ifndef X_MID_H
#define X_MID_H

#include "x_typedef.h"
#include "x_hal_5381.h"

//=====================================================================
// Constant definitions

#define MID_MAX_RW_REGIONS              3   // Regions allowing one agent to write
#define MID_MAX_RO_REGIONS              2   // Read-only regions
#define MID_MAX_REGIONS                 (MID_MAX_RW_REGIONS + MID_MAX_RO_REGIONS)

#define MID_ADDR_ALIGN_SHIFT            3
#define MID_ADDR_ALIGNMENT              (1 << MID_ADDR_ALIGN_SHIFT)
#define GROUP2_FIRST_AGENT_ID           16
#define GROUP3_FIRST_AGENT_ID           23

//===========================================================================
// Macros for register read/write

#define DRAM_READ8(offset)			IO_READ8(DRAM_BASE, (offset))
#define DRAM_READ16(offset)			IO_READ16(DRAM_BASE, (offset))
#define DRAM_READ32(offset)			IO_READ32(DRAM_BASE, (offset))

#define DRAM_WRITE8(offset, value)	IO_WRITE8(DRAM_BASE, (offset), (value))
#define DRAM_WRITE16(offset, value)	IO_WRITE16(DRAM_BASE, (offset), (value))
#define DRAM_WRITE32(offset, value)	IO_WRITE32(DRAM_BASE, (offset), (value))

#define DRAM_REG8(offset)			IO_REG8(DRAM_BASE, (offset))
#define DRAM_REG16(offset)			IO_REG16(DRAM_BASE, (offset))
#define DRAM_REG32(offset)			IO_REG32(DRAM_BASE, (offset))

//=====================================================================
// Register definitions

#define REG_REGION0UPPER        0x5c
    #define REGION_SEL_INV      (1U << 31)
    #define REGION_ENABLE       (1U << 30)
    #define AGENT_ID_SHIFT      25
    #define AGENT_ID_MASK       0x1fU
    #define INTRU_ADDR_SHIFT    MID_ADDR_ALIGN_SHIFT
    #define INTRU_ADDR_MASK     0x0fffffffU      // Address mask, 256MB

#define REG_REGION0LOWER        0x60
    #define REGION_CPU_ACCESS   (1U << 31)

#define REG_REGION1UPPER        0x64
#define REG_REGION1LOWER        0x68
#define REG_REGION2UPPER        0x6d
#define REG_REGION2LOWER        0x70

#define REG_REGION3UPPER        0x74
#define REG_REGION3LOWER        0x78
#define REG_REGION4UPPER        0xc4
#define REG_REGION4LOWER        0xc8

#define REG_DRAMSTAT_EN         0x80
    #define GPAG1_EN            (1U << 12)
    #define GPAG2_EN            (1U << 13)
    #define GPAG3_EN            (1U << 14)
    #define GPAG1_MASK          (0xfU << 8)
    #define GPAG2_MASK          (0xfU << 4)
    #define GPAG3_MASK          (0xfU << 0)
    #define GPAG1_ID(x)         (((x) & 0xf) << 8)
    #define GPAG2_ID(x)         (((x) & 0xf) << 4)
    #define GPAG3_ID(x)         (((x) & 0xf) << 0)
#define REG_DRAMSTAT_CLKLEN     0x84
#define REG_GPAG1_STAT          0x3c8
#define REG_GPAG2_STAT          0x3cc
#define REG_GPAG3_STAT          0x3d0

#define REG_INTRUDER_0          0x3ec           // Region 0 - 3
    #define INTRU_INTRUDED      (1U << 29)       // Intruded status
    #define INTRU_AGENTID_SHIFT 25              // Agent ID shift bits
    #define INTRU_AGENTID_MASK  0xf             // Agent ID mask bits

#define REG_INTRUDER_4          0x3d4           // Region 4 intruded status

#define REG_INTRUDER_EXT        0x3e8           // More ID info of group 2/3
    #define G23_AGENT_ID_MASK   0x7             // Agent ID mask, 3 bits
    #define G2_REGION0_SHIFT    28
    #define G2_REGION1_SHIFT    24
    #define G2_REGION2_SHIFT    20
    #define G2_REGION3_SHIFT    16
    #define G3_REGION0_SHIFT    12
    #define G3_REGION1_SHIFT    8
    #define G3_REGION2_SHIFT    4
    #define G3_REGION3_SHIFT    0

//=====================================================================
// Type definitions

/*

typedef enum
{
    G1_AGENT_AUDIO = 0,
    G1_AGENT_DEMUX = 1,
    G1_AGENT_VDOIN = 2,
    G1_AGENT_1394 = 3,
    G1_AGENT_FCI = 4,
    G1_AGENT_OSD = 5,
    G1_AGENT_VDOOUT = 6,
    G1_AGENT_RESERVED = 7,
    G1_AGENT_CPU = 8,
    G1_AGENT_TEST0 = 9,
    G1_AGENT_GROUP2 = 10,
    G1_AGENT_GROUP3 = 11
} G1_AGENT_ID_T;

typedef enum
{
    G2_AGENT_GFX = 0,
    G2_AGENT_MPEG1 = 1,
    G2_AGENT_MPEG2 = 2,
    G2_AGENT_JPEG = 3,
    G2_AGENT_PARSER = 4,
    G2_AGENT_DSP = 5,
    G2_AGENT_TEST1 = 6
} G2_AGENT_ID_T;

typedef enum
{
    G3_AGENT_IDE = 0,
    G3_AGENT_1394 = 1,
    G3_AGENT_POD = 2,
    GE_AGENT_TCM = 3,
    G3_AGENT_IDETEST = 4,
    G3_AGENT_RS232 = 5,
    G3_AGENT_TEST2 = 6
} G3_AGENT_ID_T;

*/

typedef enum
{
    // Group 1
    MID_AGENT_AUDIO = 0,
    MID_AGENT_DEMUX = 1,
    MID_AGENT_VDOIN = 2,
    MID_AGENT_MJC = 3,
    MID_AGENT_OSD = 4,
    MID_AGENT_PSCAN = 5,
    MID_AGENT_B2R = 6,
    MID_AGENT_MJCO = 7,
    MID_AGENT_CPU = 8,
    MID_AGENT_SCPOS = 9,
    MID_AGENT_MPEG1 = 10,
    MID_AGENT_JPEG = 11,
    MID_AGENT_TEST0 = 12,
    MID_AGENT_GROUP2 = 13,
    MID_AGENT_GROUP3 = 14,

    // Group 2
    MID_AGENT_USB1 = GROUP2_FIRST_AGENT_ID,
    MID_AGENT_ADSP = 17,
    MID_AGENT_SDIO = 18,
    MID_AGENT_ETHERNET = 19,
    MID_AGENT_IMGRZ = 20,
    MID_AGENT_DDI = 21,
    MID_AGENT_TEST1 = 22,

    // Group 3
    MID_AGENT_GFX = GROUP3_FIRST_AGENT_ID,
    MID_AGENT_TCM = 24,
    MID_AGENT_RS232 = 25,
    MID_AGENT_IDETEST = 26,
    MID_AGENT_PCMCIA = 27,
    MID_AGENT_TEST2 = 28,

    // None, for read-only region
    MID_AGENT_NONE = 30,

    // Special IDs
    MID_AGENT_MAX = 31,
    MID_AGENT_INVALID = 32
} MID_AGENT_ID_T;

#define MID_FLAG_INVERSE    (1U << 0)
#define MID_FLAG_CPUACCESS  (1U << 1)

typedef struct
{
    BOOL fgReadOnly;
    BOOL fgSet;
    BOOL fgEnabled;
    UINT32 u4LowAddr;
    UINT32 u4HighAddr;
    UINT32 u4Flags;
    MID_AGENT_ID_T eAgentId;
} REGION_STATE_T;

typedef void (*PFN_MID_CALLBACK)(UINT32 u4Region, MID_AGENT_ID_T eAgentId,
    UINT32 u4Addr);

//=====================================================================
// Function declarations

EXTERN BOOL MID_Init(void);

EXTERN BOOL MID_Exit(void);

EXTERN BOOL MID_Reset(void);

EXTERN PFN_MID_CALLBACK MID_RegisterHandler(PFN_MID_CALLBACK pfnHandler);

EXTERN BOOL MID_SetRegion(UINT32 u4Region, MID_AGENT_ID_T eAllowedAgentId,
    UINT32 u4LowAddr, UINT32 u4HighAddr);

EXTERN BOOL MID_SetRegionEx(UINT32 u4Region, MID_AGENT_ID_T eAllowedAgentId,
    UINT32 u4LowAddr, UINT32 u4HighAddr, UINT32 u4Flags);

EXTERN BOOL MID_ResetRegion(UINT32 u4Region);

EXTERN BOOL MID_AllocateFreeRegion(BOOL fgReadOnly, UINT32* pu4Region);

EXTERN BOOL MID_EnableRegionProtect(UINT32 u4Region);

EXTERN BOOL MID_DisableRegionProtect(UINT32 u4Region);

EXTERN BOOL MID_GetRegionState(UINT32 u4Region, REGION_STATE_T* prRegion);

EXTERN const CHAR* MID_AgentIdToString(MID_AGENT_ID_T eAgentId);

EXTERN MID_AGENT_ID_T MID_StringToAgentId(const CHAR* szAgent);

EXTERN UINT32 MID_CntBandWidthSet(UINT32 fgEnable, const CHAR *szAgentName,
                UINT32 u4Clk);

EXTERN UINT32 MID_CntBandWidthGet(UINT32 u4GrpId, CHAR *szAgentName,
                UINT32 *pu4AgentId, UINT32 *pu4Enable);
//---------------------------------------------------------------------


#endif  // X_MID_H

