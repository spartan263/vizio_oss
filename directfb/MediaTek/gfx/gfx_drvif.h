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
 * $RCSfile: gfx_drvif.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_drvif.h
 *  gfx driver internal public interfaces
 *  
 */

#ifndef GFX_DRVIF_H
#define GFX_DRVIF_H

#include "gfx_common_if.h"

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------

//extern INT32 _GFX_IsFlushing(void);

//extern void _GFX_SetFlushStatus(INT32 i4Status);

extern void _GFX_Init(volatile GFX_CMDQUE_T *rGfxCmdQue,
        volatile UINT64 **cmdQueBuf);

extern void _GFX_Reset(void);

extern void _GFX_Wait(void);

extern void _GFX_Wait_Vdp(void);
extern void _GFX_MwFlush(void);

extern void _GFX_Flush(void);

extern void _GFX_Lock(void);

extern void _GFX_TryLock(void);

extern void _GFX_Unlock(void);

extern void _GFX_LockCmdque(void);

extern void _GFX_UnlockCmdque(void);

extern INT32 _GFX_QueryHwIdle(void);

extern void _GFX_SetNotify(void(*pfnNotifyFunc)(UINT32));

extern void _GFX_SetDst(UINT8 *pu1Base, UINT32 u4ColorMode, UINT32 u4Pitch);

extern void _GFX_SetSrc(UINT8 *pu1Base, UINT32 u4ColorMode, UINT32 u4Pitch);

extern void _GFX_SetAlpha(UINT32 u4Alpha);

extern void _GFX_SetColor(UINT32 u4Color);

extern void _GFX_Fill(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width,
        UINT32 u4Height);

extern void _GFX_HLine(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width);

extern void _GFX_VLine(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Height);

extern void _GFX_SetBltOpt(UINT32 u4Switch, UINT32 u4ColorMin,
        UINT32 u4ColorMax);

extern void _GFX_BitBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

extern void _GFX_Set_Src_ColorKey(UINT32 u4SrcColorKey);

extern void _GFX_Blend(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

extern void _GFX_Compose(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Ar,
        UINT32 u4Mode);

extern void _GFX_AlphaComposePass(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pass,
        UINT32 u4Param);

extern void _GFX_SetColCnvFmt(UINT32 u4YCFmt, UINT32 u4SwapMode,
        UINT32 u4VidStd, UINT32 u4VidSys);

extern void _GFX_SetColCnvSrc(UINT8 *pu1SrcLuma, UINT32 u4SrcLumaPitch,
        UINT8 *pu1SrcChroma, UINT32 u4SrcChromaPitch, UINT32 u4FieldPic);

extern void _GFX_ColConv(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width,
        UINT32 u4Height);

extern void _GFX_StretchBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4SrcW,
        UINT32 u4SrcH, UINT32 u4DstX, UINT32 u4DstY, UINT32 u4DstW,
        UINT32 u4DstH);

extern void _GFX_AlphaMapBitBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

extern void _GFX_SetRopOpt(UINT32 u4RopCode);

extern void _GFX_RopBitBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

extern void _GFX_ComposeLoop(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Ar,
        UINT32 u4OpCode, UINT32 u4RectSrc);

extern void _GFX_ComposeLoopImprove(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Ar,
        UINT32 u4OpCode, UINT32 u4RectSrc);


extern void _GFX_SetIdx2DirSrc(UINT8 *pu1Base, UINT32 u4SrcCM);

extern void _GFX_SetIdx2DirOpt(UINT8 *pu1PaletteBase, UINT32 u4MsbLeft,
        UINT32 u4StartByteAligned);

extern void _GFX_Idx2DirBitBlt(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width,
        UINT32 u4Height);

extern void _GFX_2DMemCompare(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height);

extern INT32 _GFX_GetMemCompareResult(void);

extern void _GFX_SetHoriToVertLineOpt(UINT32 u4Is90dCcw);

extern void _GFX_HoriToVertLine(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4HoriLineWidth);

// normal driver
#if !defined(GFX_MINI_DRIVER)
extern void _GFX_SetGradOpt(UINT32 u4IncX, UINT32 u4IncY,
        const UINT32 asu4DeltaX[4], const UINT32 asu4DeltaY[4]);

extern void _GFX_GradFill(UINT32 u4X, UINT32 u4Y, UINT32 u4Width,
        UINT32 u4Height, UINT32 u4Mode);

extern void _GFX_DMA(UINT32 *pu4Dst, UINT32 *pu4Src, UINT32 u4NumWrd);

extern void _GFX_SetBmp(UINT8 *puu1Base, UINT32 u4BmpColorMode, UINT32 u4Width,
        UINT32 u4Height, UINT32 u4RunLen);

extern void _GFX_Bmp(UINT32 u4X, UINT32 u4Y);

extern void _GFX_ColorMap(UINT32 u4BmpCM, UINT32 u4CM,
        const UINT32 *psu4ColorMap);
#endif // #if !defined(GFX_MINI_DRIVER)

extern void _GFX_SetPremult(UINT32 u4PremultSrcR, UINT32 u4PremultDstR,
        UINT32 u4PremultDstW, UINT32 u4PremultOvrflw);

extern void _GFX_StretchAlphaCom(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4SrcW,
        UINT32 u4SrcH, UINT32 u4DstX, UINT32 u4DstY, UINT32 u4DstW,
        UINT32 u4DstH, UINT32 u4Ar, UINT32 u4OpCode);

//---------------------------------------------------------------------------
// Public functions (high-level APIs)
//---------------------------------------------------------------------------

extern INT32
        GFX_MemCompare(UINT8 *pu1DstBase, UINT8 *pu1SrcBase, UINT32 u4Size);

extern void GFX_RotateBmp(UINT8 *pu1DstBase, UINT32 u4DstPitch,
        UINT8 *pu1SrcBase, UINT32 u4SrcPitch, UINT32 u4Width, UINT32 u4Height,
        UINT32 u4ColorMode, UINT32 u4Is90Ccw);

extern void GFX_Memset(UINT8 *pu1Addr, UINT32 u4Size, UINT32 u4Color);

extern void GFX_CopyTopfieldToBotfield(UINT8 *pu1TopStartAddr,
        UINT32 u4TotalLines, UINT32 u4LineWidth);

extern void _GFX_ResetSrcDstParam(void);
extern void _VGFX_Enable(void);
extern void _VGFX_GeneralAlphaComSet(UINT32 u4ImgCm, UINT32 u4WbCm,
        UINT32 u4Ar, UINT32 u4OpCode, BOOL fgAlcomNormal, UINT32 u4PremultSrcR,
        UINT32 u4PremultDstR, UINT32 u4PremultDstW, UINT32 u4PremultOvrflw);

#endif // GFX_DRVIF_H

