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
 * $RCSfile: gfx_if.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_if.h
 *  gfx driver main interfaces
 *  
 */

#ifndef GFX_IF_H
#define GFX_IF_H

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <string.h>
#include <direct/messages.h>
#endif

/*lint -save -e961 */
#include "x_common.h"
#include "x_typedef.h"
#include "x_timer.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_clksrc.h"
#include "x_hal_5381.h"
/*lint -restore */

#include "gfx_cmdque.h"

#include "gfx_common_if.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

#ifdef __KERNEL__
#define ASSERT_KERNEL()  do {} while (0)
#define ASSERT_USER()    do { printk( KERN_ERR "%s: user space function called!\n" ); } while (0)
#else
#define ASSERT_USER()    do {} while (0)
#define ASSERT_KERNEL()  do { D_BUG( "kernel space function called \n" ); } while (0)
#endif

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  GFX_Init
//
/** Initialize gfx driver.
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Init(volatile GFX_CMDQUE_T *rGfxCmdQue,
        volatile UINT64 **cmdQueBuf);

//---------------------------------------------------------------------------
//  GFX_Reset
//
/** Reset engine.
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Reset(void);

//---------------------------------------------------------------------------
//  GFX_Wait
//
/** Wait for flushing engine completely.
 *  
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Wait(void);

//---------------------------------------------------------------------------
//  GFX_Wait_Vdp
//
/** Wait for Vdp completely.
 *  
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Wait_Vdp(void);

//---------------------------------------------------------------------------
//  GFX_MwFlush
//
/** Flush cmdque and force engine to do operations (for MW use).
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_MwFlush(void);

//---------------------------------------------------------------------------
//  GFX_Flush
//
/** Flush cmdque and force engine to do operations (for Drv use).
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Flush(void);

//---------------------------------------------------------------------------
//  GFX_Lock
//
/** Take the gfx resource.
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Lock(void);

//---------------------------------------------------------------------------
//  GFX_TryLock
//
/** Try to take the gfx resource.
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_TryLock(void);

//---------------------------------------------------------------------------
//  GFX_Unlock
//
/** Release the gfx resource.
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Unlock(void);

//---------------------------------------------------------------------------
//  GFX_LockCmdque
//
/** Take the gfx-cmdq resource.
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_LockCmdque(void);

//---------------------------------------------------------------------------
//  GFX_UnlockCmdque
//
/** Release the gfx-cmdq resource.
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_UnlockCmdque(void);

//---------------------------------------------------------------------------
//  GFX_QueryHwIdle
//
/** Query engine status.
 *
 *  @retval 1   Idle
 *  @retval 0   Busy
 */
//---------------------------------------------------------------------------
extern INT32 GFX_QueryHwIdle(void);

//---------------------------------------------------------------------------
//  GFX_SetNotify
//
/** Set notify function (for MW use).
 *
 *  @param  pfnNotifyFunc   The callback function pointer
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetNotify(void(*pfnNotifyFunc)(UINT32));

//---------------------------------------------------------------------------
//  GFX_SetDst
//
/** Set destination region.
 *
 *  @param  pu1Base     The base address of the dst region
 *  @param  u4ColorMode The color mode of the dst region
 *  @param  u4Pitch     The pitch of the dst region
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetDst(UINT8 *pu1Base, UINT32 u4ColorMode, UINT32 u4Pitch);

//---------------------------------------------------------------------------
//  GFX_SetSrc
//
/** Set source region.
 *
 *  @param  pu1Base     The base address of the src region
 *  @param  u4ColorMode The color mode of the src region
 *  @param  u4Pitch     The pitch of the src region
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetSrc(UINT8 *pu1Base, UINT32 u4ColorMode, UINT32 u4Pitch);

//---------------------------------------------------------------------------
//  GFX_SetAlpha
//
/** Set global alpha.
 *
 *  @param  u4Alpha     The global alpha
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetAlpha(UINT32 u4Alpha);

//---------------------------------------------------------------------------
//  GFX_SetColor
//
/** Set rectangular color.
 *
 *  @param  u4Color     The rectangular color
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetColor(UINT32 u4Color);

//---------------------------------------------------------------------------
//  GFX_Fill
//
/** Fill rectangle with rectangular color.
 *
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the rectangle
 *  @param  u4Height    The height of the rectangle
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Fill(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width,
        UINT32 u4Height);

//---------------------------------------------------------------------------
//  GFX_HLine
//
/** Draw horizontal line.
 *
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the line
 * 
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_HLine(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width);

//---------------------------------------------------------------------------
//  GFX_VLine
//
/** Draw vertical line.
 *
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Height    The height of the line
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_VLine(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Height);

//---------------------------------------------------------------------------
//  GFX_SetBltOpt
//
/** Set bitblt option.
 *
 *  @param  u4Switch    The option flag of BitBlt
 *  @param  u4ColorMin  The dst color key min
 *  @param  u4ColorMax  The dst color key max
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void
        GFX_SetBltOpt(UINT32 u4Switch, UINT32 u4ColorMin, UINT32 u4ColorMax);

//---------------------------------------------------------------------------
//  GFX_BitBlt
//
/** Bit blit operation.
 *  
 *  @param  u4SrcX      The x coordinate of the src region
 *  @param  u4SrcY      The y coordinate of the src region
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the extent
 *  @param  u4Height    The height of the extent
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_BitBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

extern void GFX_Set_Src_ColorKey(UINT32 u4SrcColorKey);

//---------------------------------------------------------------------------
//  GFX_Blend
//
/** Alpha blending operation.
 *  
 *  @param  u4SrcX      The x coordinate of the src region
 *  @param  u4SrcY      The y coordinate of the src region
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the extent
 *  @param  u4Height    The height of the extent
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Blend(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

//---------------------------------------------------------------------------
//  GFX_AlphaComposePass
//
/** Alpha compose pass operation.
 *  
 *  @param  u4SrcX      The x coordinate of the src region
 *  @param  u4SrcY      The y coordinate of the src region
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the extent
 *  @param  u4Height    The height of the extent
 *  @param  u4Pass      The pass method
 *  @param  u4Param     The global alpha
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_AlphaComposePass(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pass,
        UINT32 u4Param);

//---------------------------------------------------------------------------
//  GFX_SetColCnvFmt
//
/** Set ycbcr to rgb color conversion format.
 *
 *  @param  u4YCFmt     The yc format
 *  @param  u4SwapMode  The swap mode
 *  @param  u4VidStd    The video standard
 *  @param  u4VidSys    The video system
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetColCnvFmt(UINT32 u4YCFmt, UINT32 u4SwapMode,
        UINT32 u4VidStd, UINT32 u4VidSys);

//---------------------------------------------------------------------------
//  GFX_SetColCnvSrc
//
/** Set color conversion source region.
 *
 *  @param  pu1SrcLuma          The base address of the luma region
 *  @param  u4SrcLumaPitch      The pitch of the luma region
 *  @param  pu1SrcChroma        The base address of the chroma region
 *  @param  u4SrcChromaPitch    The pitch of the chroma region
 *  @param  u4FieldPic          The field mode
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetColCnvSrc(UINT8 *pu1SrcLuma, UINT32 u4SrcLumaPitch,
        UINT8 *pu1SrcChroma, UINT32 u4SrcChromaPitch, UINT32 u4FieldPic);

//---------------------------------------------------------------------------
//  GFX_ColConv
//
/** Convert ycbcr to rgb.
 *
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the extent
 *  @param  u4Height    The height of the extent
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_ColConv(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width,
        UINT32 u4Height);

//---------------------------------------------------------------------------
//  GFX_StretchBlt
//
/** Stretch bitblt (repeat or drop).
 *  
 *  @param  u4SrcX      The x coordinate of the src region
 *  @param  u4SrcY      The y coordinate of the src region
 *  @param  u4SrcW      The width of the src extent
 *  @param  u4SrcH      The height of the src extent
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4DstW      The width of the dst extent
 *  @param  u4DstH      The height of the dst extent
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_StretchBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4SrcW,
        UINT32 u4SrcH, UINT32 u4DstX, UINT32 u4DstY, UINT32 u4DstW,
        UINT32 u4DstH);

//---------------------------------------------------------------------------
//  GFX_AlphaMapBitBlt
//
/** Alpha-map bitblt operation.
 *  
 *  @param  u4SrcX      The x coordinate of the src region
 *  @param  u4SrcY      The y coordinate of the src region
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the extent
 *  @param  u4Height    The height of the extent
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_AlphaMapBitBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

//---------------------------------------------------------------------------
//  GFX_SetRopOpt
//
/** Set rop bitblt option.
 *
 *  @param  u4RopCode       The rop code
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetRopOpt(UINT32 u4RopCode);

//---------------------------------------------------------------------------
//  GFX_RopBitBlt
//
/** Rop bitblt operation.
 *  
 *  @param  u4SrcX      The x coordinate of the src region
 *  @param  u4SrcY      The y coordinate of the src region
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the extent
 *  @param  u4Height    The height of the extent
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_RopBitBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

//---------------------------------------------------------------------------
//  GFX_ComposeLoop
//
/** Loop mode alpha composition.
 *  
 *  @param  u4SrcX      The x coordinate of the src region
 *  @param  u4SrcY      The y coordinate of the src region
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the extent
 *  @param  u4Height    The height of the extent
 *  @param  u4Ar        The global alpha
 *  @param  u4OpCode    The PD rules
 *  @param  u4RectSrc   The flag of src extent is filled rectangular color
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_ComposeLoop(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Ar,
        UINT32 u4OpCode, UINT32 u4RectSrc);

//---------------------------------------------------------------------------
//  GFX_SetIdx2DirSrc
//
/** Set index to direct color bitblt source region.
 *
 *  @param  pu1Base     The base address of the src region
 *  @param  u4SrcCM     The color mode of the src region
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetIdx2DirSrc(UINT8 *pu1Base, UINT32 u4SrcCM);

//---------------------------------------------------------------------------
//  GFX_SetIdx2DirOpt
//
/** Set index to direct color bitblt option.
 *
 *  @param  pu1PaletteBase      The base address of the palette
 *  @param  u4MsbLeft           The flag of left pixel location in one byte
 *  @param  u4StartByteAligned  The flag of src line start byte aligned
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetIdx2DirOpt(UINT8 *pu1PaletteBase, UINT32 u4MsbLeft,
        UINT32 u4StartByteAligned);

//---------------------------------------------------------------------------
//  GFX_Idx2DirBitBlt
//
/** index to direct color bitblt operation.
 *
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the extent
 *  @param  u4Height    The height of the extent
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_Idx2DirBitBlt(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width,
        UINT32 u4Height);

//---------------------------------------------------------------------------
//  GFX_2DMemCompare
//
/** 2D memory compare operation.
 *  
 *  @param  u4SrcX      The x coordinate of the src region
 *  @param  u4SrcY      The y coordinate of the src region
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the extent
 *  @param  u4Height    The height of the extent
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_2DMemCompare(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

//---------------------------------------------------------------------------
//  GFX_GetMemCompareResult
//
/** Get memory compare result.
 *
 *  @retval 1   With difference
 *  @retval 0   No difference
 */
//---------------------------------------------------------------------------
extern INT32 GFX_GetMemCompareResult(void);

//---------------------------------------------------------------------------
//  GFX_SetHoriToVertLineOpt
//
/** Set horizontal line to vertical line option.
 *
 *  @param  u4Is90dCcw      The flag of 90" clockwise or counterclockwise
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_SetHoriToVertLineOpt(UINT32 u4Is90dCcw);

//---------------------------------------------------------------------------
//  GFX_HoriToVertLine
//
/** Horizontal line to vertical line operation.
 *  
 *  @param  u4SrcX      The x coordinate of the src region
 *  @param  u4SrcY      The y coordinate of the src region
 *  @param  u4DstX      The x coordinate of the dst region
 *  @param  u4DstY      The y coordinate of the dst region
 *  @param  u4Width     The width of the horizontal line
 *
 *  @retval void
 */
//---------------------------------------------------------------------------
extern void GFX_HoriToVertLine(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4HoriLineWidth);

extern void GFX_ResetSrcDstParam(void);

extern void GFX_StretchAlphaCom(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4SrcW,
        UINT32 u4SrcH, UINT32 u4DstX, UINT32 u4DstY, UINT32 u4DstW,
        UINT32 u4DstH, UINT32 u4Ar, UINT32 u4OpCode);

extern void GFX_SetPremult(UINT32 u4PremultSrcR, UINT32 u4PremultDstR,
        UINT32 u4PremultDstW, UINT32 u4PremultOvrflw);

extern void VGFX_Scaler(VGFX_PARAM_SET_T *vgParam);

extern void VGFX_Scaler_Matrix(VGFX_PARAM_SET_T *pt_vgfx);
extern int GFX_GetVirtualCmdQueSize(void);

#endif // GFX_IF_H

