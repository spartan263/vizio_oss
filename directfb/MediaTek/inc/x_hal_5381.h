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
 * Description:
 *
 *---------------------------------------------------------------------------*/

#ifndef X_HAL_5381_H
#define X_HAL_5381_H

#include "x_typedef.h"
#include "x_hal_io.h"

//===========================================================================
// Constant definitions

#define MEMORY_ALIGNMENT            8

//===========================================================================
// Memory mapping

#define DRAM_B_BASE					0x0
#define PBI_B_BASE					0x10000000
#define IO_BASE						0x20000000
#define IO_PHYS                     0x20000000

#ifdef __KERNEL__
#if defined(CC_UBOOT) || defined(CC_5391_LOADER)
#define IO_VIRT                     0x20000000
#else /* CC_UBOOT */
#define IO_VIRT                     0xF0000000
#endif /* CC_UBOOT */
#else
#define IO_VIRT                     0x20000000
#endif

#define PBI_A_BASE					0x28000000
#define DRAM_A_BASE					0x30000000
#define DRAM_C_BASE					0x40000000
#define DRAM_D_BASE					0x50000000

#define IO_ADDR(base)               (IO_VIRT + base)

//===========================================================================
// IO register definitions
#ifndef	CC_MT5391_AUD_SUPPORT
#define PARSER0_BASE				(IO_VIRT + 0x00000)
#endif
#define PARSER1_BASE				(IO_VIRT + 0x01000)     // obsolete
#define VLD0_BASE					(IO_VIRT + 0x02000)
#define DV_BASE                     (IO_VIRT + 0x02200)
#define VLD1_BASE					(IO_VIRT + 0x03000)     // obsolete
#define GRAPH_BASE					(IO_VIRT + 0x04000)
#define AUDIO_BASE					(IO_VIRT + 0x05000)

#define VGRAPH_BASE                 (IO_VIRT + 0x5c000) 
#define VGRAPH_CMDQUE_BASE          (IO_VIRT + 0x5000) 

#ifdef	CC_MT5391_AUD_SUPPORT
#define PARSER0_BASE				(AUDIO_BASE + 0x00700) //added by ling
#define PARSER0_BASE2				(AUDIO_BASE + 0x00600) //added by ling
#endif

#define TV_EOCODER_BASE				(IO_VIRT + 0x06000)     // obsolete
#define DRAM_BASE					(IO_VIRT + 0x07000)
#define BIM_BASE					(IO_VIRT + 0x08000)
#define I1394_BASE					(IO_VIRT + 0x09000)
#define MC0_BASE					(IO_VIRT + 0x0a000)
#define MC1_BASE					(IO_VIRT + 0x0b000)     // obsolete
#define RS232_BASE					(IO_VIRT + 0x0c000)
#define CKGEN_BASE					(IO_VIRT + 0x0d000)
#define DISPLAY_BASE				(IO_VIRT + 0x0e000)     // obsolete
#define ATA_BASE					(IO_VIRT + 0x0f000)
#define TCMGPR0_BASE				(IO_VIRT + 0x10000)
#define TCMGPR1_BASE				(IO_VIRT + 0x11000)
#define FLASH_CARD_BASE				(IO_VIRT + 0x12000)
#define SMART_CARD_BASE				(IO_VIRT + 0x13000)
#define VIDEO_IN_BASE				(IO_VIRT + 0x14000)     // obsolete
#define PVR_BASE					(IO_VIRT + 0x15000)     // obsolete
#define PCMCIA_BASE					(IO_VIRT + 0x16000)
#define SHA1_BASE				    (IO_VIRT + 0x16400)
#define DFAST_BASE				    (IO_VIRT + 0x16800)
#define DEMUX0_BASE					(IO_VIRT + 0x17000)
#define DEMUX1_BASE					(IO_VIRT + 0x18000)
#define DEMUX2_BASE					(IO_VIRT + 0x19000)
#define DEMUX3_BASE					(IO_VIRT + 0x1a000)
#define DEMUX4_BASE					(IO_VIRT + 0x1b000)
#define DEMUX5_BASE					(IO_VIRT + 0x1c000)
#define DEMUX6_BASE					(IO_VIRT + 0x1d000)
#define PIDSWAP_BASE				(IO_VIRT + 0x1e000)
#define IDE_TEST_BASE               (IO_VIRT + 0x1f000)
#define GFX_SCALAR_BASE             (IO_VIRT + 0x20000)
#define IMGRZ_BASE                  (IO_VIRT + 0x20000)
#define OSD_BASE                    (IO_VIRT + 0x21000)
#define VIDEO_IN0_BASE              (IO_VIRT + 0x22000)
#define VIDEO_IN1_BASE              (IO_VIRT + 0x23000)
#define PSCAN_BASE                  (IO_VIRT + 0x24000)
#define SCPOS_BASE                  (IO_VIRT + 0x25000)
#define BLK2RS_BASE                 (IO_VIRT + 0x26000)
#define SDIO_BASE                   (IO_VIRT + 0x27000)
#define PDWNC_BASE                  (IO_VIRT + 0x28000)
#define HPDS_BASE                   (IO_VIRT + 0x29000)
#define POST_PROC_BASE              (IO_VIRT + 0x2d000)

#define MUSB_BASE					(IO_VIRT + 0x29000)
#define MUSB1_BASE					(IO_VIRT + 0x2E000)

#define MUSB_COREBASE					(0x800)
#define MUSB_DMABASE					(0xA00)
#define MUSB_MISCBASE				        (0x600)
#define MUSB_CBUFBASE				        (0x500)
#define MUSB_PHYBASE					(0x400)
#define ETHERNET_BASE               (IO_VIRT + 0x32000)     
#define DTCP_BASE                   (IO_VIRT + 0x33000)     

//===========================================================================
// Macros for register read

#define IO_READ8(base, offset)		HAL_READ8((base) + (offset))
#define IO_READ16(base, offset)		HAL_READ16((base) + (offset))
#define IO_READ32(base, offset)		HAL_READ32((base) + (offset))
#define IO_READ32MSK(base, offset, msk) \
        ((UINT32)HAL_READ32((base) + (offset)) & (UINT32)(msk))
#define IO_READ32MSK_BOOL(base, offset, msk) \
       ( (IO_READ32MSK((base), (offset), (msk)) > 0)?TRUE:FALSE)

//===========================================================================
// Macros for register write

#define IO_WRITE8(base, offset, value)	HAL_WRITE8((base) + (offset), (value))
#define IO_WRITE16(base, offset, value)	HAL_WRITE16((base) + (offset), (value))
#define IO_WRITE32(base, offset, value)	HAL_WRITE32((base) + (offset), (value))
#define IO_WRITE32MSK(base, offset, value, msk)	\
        IO_WRITE32((base), (offset), \
                   (IO_READ32((base), (offset)) & ~(msk)) | (value))


//===========================================================================
// Macros for register read/write access

#define IO_REG8(base, offset)		HAL_REG8((base) + (offset))
#define IO_REG16(base, offset)		HAL_REG16((base) + (offset))
#define IO_REG32(base, offset)		HAL_REG32((base) + (offset))

#define IO_ADDR(base)               (IO_VIRT + base)

//===========================================================================
// Macros for address translation

//#define CACHE(addr)					((addr) & 0x0fffffff)
//#define NONCACHE(addr)				(((addr) & 0x0fffffff) | 0x30000000)
#ifdef __KERNEL__
#ifdef CC_UBOOT
#define PHYSICAL(addr)				((addr) & 0x0fffffff)
#define VIRTUAL(addr)               (addr)
#else
extern UINT32 VIRTUAL(UINT32);
extern UINT32 PHYSICAL(UINT32);
#endif
#else
#define PHYSICAL(addr)				((addr) & 0x0fffffff)
#define VIRTUAL(addr)               (addr)
#endif


//===========================================================================
// Type definitions

typedef enum
{
    IC_VER_FPGA = 0x0000U,           // FPGA

    IC_VER_5381_AA =    0x62381AA0U,        // 5381 AA
    IC_VER_5381_AB =    0x62381AB0U,        // 5381 AB
    IC_VER_5381_AC =    0x62381AC0U,        // 5381 AC
    IC_VER_5381_AD =    0x62381AD0U,        // 5381 AD
    IC_VER_5381_AE =    0x62381AE0U,        // 5381 AE
    IC_VER_5381_AF =    0x62381AF0U,        // 5381 AF
    IC_VER_5381_BA =    0x62381BA0U,        // 5381 BA
    IC_VER_5381_BB =    0x62381BB0U,        // 5381 BB
    IC_VER_5381_BC =    0x62381BC0U,        // 5381 BC
    IC_VER_5381_CA =    0x62381CA0U,        // 5381 CA
    IC_VER_5381_CB =    0x62381CB0U,        // 5381 CB
    IC_VER_5381_CC =    0x62381CC0U,        // 5381 CC

    IC_VER_5382P_AA =   0x62382AA0U,        // 5382P AA
    IC_VER_5382P_AB =   0x62382AB0U,        // 5382P AB
    IC_VER_5382P_AC =   0x62382AC0U,        // 5382P AC
    IC_VER_5382P_AD =   0x62382AD0U,        // 5382P AD
    IC_VER_5382P_AE =   0x62382AE0U,        // 5382P AE
    IC_VER_5382P_AF =   0x62382AF0U,        // 5382P AF
    IC_VER_5382P_BA =   0x62382BA0U,        // 5382P BA
    IC_VER_5382P_BB =   0x62382BB0U,        // 5382P BB
    IC_VER_5382P_BC =   0x62382BC0U,        // 5382P BC
    IC_VER_5382P_CA =   0x62382CA0U,        // 5382P CA
    IC_VER_5382P_CB =   0x62382CB0U,        // 5382P CB
    IC_VER_5382P_CC =   0x62382CC0U,        // 5382P CC

    IC_VER_UNKNOWN = 0xffffffffU     // Unknown version
} IC_VERSION_T;

#define IC_VER_AA       IC_VER_5381_AA
#define IC_VER_AB       IC_VER_5381_AB
#define IC_VER_AC       IC_VER_5381_AC
#define IC_VER_BA       IC_VER_5381_BA
#define IC_VER_BB       IC_VER_5381_BB
#define IC_VER_BC       IC_VER_5381_BC
#define IC_VER_CA       IC_VER_5381_CA
#define IC_VER_CB       IC_VER_5381_CB
#define IC_VER_CC       IC_VER_5381_CC


//===========================================================================
// Exported functions

EXTERN void* BSP_AllocateReserved(UINT32 u4Size);
EXTERN void* BSP_AllocateReservedAlign(UINT32 u4Size, UINT32 u4Align);
EXTERN void BSP_FreeReserved(void* p);
EXTERN void* BSP_MapReservedMem(void *pv_Phys, UINT32 z_Size);
EXTERN BOOL BSP_IsFPGA(void);
EXTERN UINT32 BSP_GetSystemClock(void);
EXTERN void BSP_HaltSystem(void);
EXTERN void* BSP_AllocDmaMemory(UINT32 u4Size);
EXTERN BOOL BSP_FreeDmaMemory(void* p);
EXTERN void* BSP_AllocAlignedDmaMemory(UINT32 u4Size, UINT32 u4Align);
EXTERN BOOL BSP_FreeAlignedDmaMemory(void* p);
EXTERN IC_VERSION_T BSP_GetIcVersion(void);
EXTERN UINT32 BSP_IsSystemAcOn(void);
EXTERN UINT32 BSP_GetFbmMemAddr(void);
EXTERN UINT32 BSP_QuerySetAbortRebootFlag(UINT32 *pu4Flag);
EXTERN UINT32 BSP_GetMemSize(void);
EXTERN UINT32 BSP_GetPanelTableOffset(void);
EXTERN UINT32 BSP_GetPQDataOffset(void);
EXTERN UINT32 BSP_GetPQSmartPicOffset(void);
EXTERN UINT32 BSP_GetAQDataOffset(void);

#endif  // X_HAL_5381_H
