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
 * $RCSfile: gfx_sw.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_sw.h
 *  gfx driver software module interfaces
 *  init, get register base, isr, callback, op count and action
 */

#ifndef GFX_SW_H
#define GFX_SW_H

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "gfx_if.h"
#include "gfx_common_if.h"

#if defined(GFX_ENABLE_SW_MODE)
//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

typedef struct _GFX_YCBCR
{
    UINT8 y;
    UINT8 cb;
    UINT8 cr;
}GFX_YCBCR_T;

typedef struct _GFX_YCBCR2RGB_DATA_T
{
    UINT8 *pu1LumaBase;
    UINT8 *pu1ChromaBase;
    UINT8 *pu1DstBase;
    UINT32 u4LumaPitch;
    UINT32 u4ChromaPitch;
    UINT32 u4DstPitch;
    UINT32 u4DstCM;
    UINT32 u4YCFormat;
    UINT32 u4VideoStd;
    UINT32 u4VideoSys;
    UINT32 u4VideoClip;
    UINT32 u4PicSelect;
    UINT32 u4SwapMode;
    UINT32 u4Width;
    UINT32 u4Height;
}GFX_YCBCR2RGB_DATA_T;

typedef struct _GFX_GRADIENT_DATA_T
{
    UINT8 *pu1DstBase;
    UINT32 u4DstPitch;
    UINT32 u4DstCM;
    UINT32 u4X_Pix_Inc;
    UINT32 u4Y_Pix_Inc;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4GradMode;
    UINT32 u4RectColor;
    UINT8 u1Delta_X_C0;
    UINT8 u1Delta_X_C1;
    UINT8 u1Delta_X_C2;
    UINT8 u1Delta_X_C3;
    UINT8 u1Delta_Y_C0;
    UINT8 u1Delta_Y_C1;
    UINT8 u1Delta_Y_C2;
    UINT8 u1Delta_Y_C3;
}GFX_GRADIENT_DATA_T;

typedef struct _GFX_BITBLT_DATA_T
{
    UINT8 *pu1SrcBase;
    UINT8 *pu1DstBase;
    UINT32 u4SrcPitch;
    UINT32 u4DstPitch;
    UINT32 u4SrcCM;
    UINT32 u4DstCM;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4ColorMin;
    UINT32 u4ColorMax;
    UINT32 u4TransEn;
    UINT32 u4ColchgEn;
    UINT32 u4KeynotEn;
    UINT32 u4Color;
}GFX_BITBLT_DATA_T;

// alpha composition loop mode
typedef struct _GFX_ACLM_DATA_T
{
    UINT8 *pu1SrcBase;
    UINT8 *pu1DstBase;
    UINT32 u4SrcPitch;
    UINT32 u4DstPitch;
    UINT32 u4SrcCM;
    UINT32 u4DstCM;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4AlComAr;
    UINT32 u4AlComOpCode;
    UINT32 u4AlComRectSrc;
    UINT32 u4RectColor;
}GFX_ACLM_DATA_T;

typedef struct _GFX_ROP_DATA_T
{
    UINT8 *pu1SrcBase;
    UINT8 *pu1DstBase;
    UINT32 u4SrcPitch;
    UINT32 u4DstPitch;
    UINT32 u4SrcCM;
    UINT32 u4DstCM;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4RopOpCode;
    UINT32 u4RopNoWr;
}GFX_ROP_DATA_T;

typedef struct _GFX_IDX2DIR_DATA_T
{
    UINT8 *pu1SrcBase;
    UINT8 *pu1DstBase;
    UINT32 u4SrcPitch;
    UINT32 u4DstPitch;
    UINT32 u4CharCM; // src color mode
    UINT32 u4DstCM; // dst color mode
    UINT32 u4Width;
    UINT32 u4Height;
    UINT8 *pu1PaleBase; // palette table address
    UINT32 u4MsbLeft;
    UINT32 u4ByteAlign;
}GFX_IDX2DIR_DATA_T;

typedef struct _GFX_HORI2VERT_DATA_T
{
    UINT8 *pu1SrcBase;
    UINT8 *pu1DstBase;
    UINT32 u4SrcPitch;
    UINT32 u4DstPitch;
    UINT32 u4SrcCM;
    UINT32 u4DstCM;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4DstPitchDec;
    UINT32 u4StrDstWidth;
    UINT32 u4StrDstHeight;
}GFX_HORI2VERT_DATA_T;

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

extern void GFX_SwInit(void);

extern void GFX_SwGetRegBase(UINT32 **ppu4RegBase);

extern void GFX_SwISR(void);

extern INT32 GFX_SwSetCallBack(void (*pfnCallBack)(void *pvTag), void *pvTag);

extern UINT32 GFX_SwGetOpCount(void);

extern void GFX_SwAction(void);

extern void GFX_SwSetColorComponent(UINT8 **ppu1DestFb, UINT32 u4CM,
        const INT32 *pi4sColorComponent);

extern void GFX_SwGetColorComponent(UINT8 **ppu1DestFb, UINT32 u4CM,
        INT32 *pi4ColorComponent);

extern void GFX_SwGetColorComponent_5381(UINT8 **ppu1DestFb, UINT32 u4CM,
        INT32 *pi4ColorComponent);

extern void GFX_SwSetRopColorComp(UINT8 **ppu1DestFb, UINT32 u4CM, const UINT32 *pu4sColorComp);

extern void GFX_SwGetRopColorComp(UINT8 **ppu1DestFb, UINT32 u4CM, UINT32 *pu4ColorComp);

extern INT32 GFX_SwBitBlt(UINT8 *pu1SrcBase, UINT8 *pu1DstBase, UINT32 u4SrcPitch,
        UINT32 u4DstPitch, UINT32 u4SrcCM, UINT32 u4DstCM, UINT32 u4Width,
        UINT32 u4Height, UINT32 u4ColorMin, UINT32 u4ColorMax, UINT32 u4TransEn,
        UINT32 u4KeynotEn, UINT32 u4ColchgEn, UINT32 u4KeySdSel, UINT32 u4Color);

extern INT32 GFX_SwAlphaMapBitBlt(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        UINT32 u4SrcPitch, UINT32 u4DstPitch, UINT32 u4SrcCM, UINT32 u4DstCM,
        UINT32 u4Width, UINT32 u4Height);

extern INT32 GFX_SwAlphaComposePass_5381(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        UINT32 u4SrcPitch, UINT32 u4DstPitch, UINT32 u4SrcCM, UINT32 u4DstCM,
        UINT32 u4Width, UINT32 u4Height, UINT32 u4AlphaValue, UINT32 u4AlcomPass);

extern INT32 GFX_SwAlphaCompositionLoop(const GFX_ACLM_DATA_T *prsData);

extern INT32 GFX_SwBlock2Linear(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        INT32 i4Width, INT32 i4Height, INT32 i4MBWidth, INT32 i4MBHeight);

extern INT32 GFX_SwBlock2Swap(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        INT32 i4Width, INT32 i4Height, INT32 i4MBWidth, INT32 i4MBHeight);

extern INT32 GFX_SwBlock2Mergetop(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        INT32 i4Width, INT32 i4Height, INT32 i4MBWidth, INT32 i4MBHeight);

extern INT32 GFX_SwYCbCrLinearScan(const GFX_YCBCR2RGB_DATA_T *prsData);

extern INT32 GFX_SwYCbCr601toArgbVideoEqu(const GFX_YCBCR_T *prsSrc,
        INT32 *pi4ColorComponent);

extern INT32 GFX_SwYCbCr601toArgbCompEqu(const GFX_YCBCR_T *prsSrc,
        INT32 *pi4ColorComponent);

extern INT32 GFX_SwYCbCr709toArgbVideoEqu(const GFX_YCBCR_T *prsSrc,
        INT32 *pi4ColorComponent);

extern INT32 GFX_SwYCbCr709toArgbCompEqu(const GFX_YCBCR_T *prsSrc,
        INT32 *pi4ColorComponent);

extern INT32 GFX_SwYCbCr2RGB(const GFX_YCBCR2RGB_DATA_T *prsData);

extern INT32 GFX_SwAlphaBlending(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        UINT32 u4SrcPitch, UINT32 u4DstPitch, UINT32 u4Dst_CM, UINT32 u4Alpha,
        UINT32 u4Width, UINT32 u4Height);

extern INT32 GFX_SwAlphaBlending_5381(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        UINT32 u4SrcPitch, UINT32 u4DstPitch, UINT32 u4Dst_CM, UINT32 u4Alpha,
        UINT32 u4Width, UINT32 u4Height);

extern INT32 GFX_SwBigEndianAndLittleEndianConversion(UINT8 *pu1DstAddr,
        UINT8 *pu1SrcAddr, UINT32 u4TotalPixels, UINT32 u4ColorMode);

extern INT32 GFX_SwGradientFill(const GFX_GRADIENT_DATA_T *prsData);

extern void GFX_SwRop_NotSrc(const GFX_ROP_DATA_T *prsData);
extern void GFX_SwRop_NotDst(const GFX_ROP_DATA_T *prsData);

extern void GFX_SwRop_Src_Xor_Dst(const GFX_ROP_DATA_T *prsData);
extern void GFX_SwRop_Src_XNor_Dst(const GFX_ROP_DATA_T *prsData);

extern void GFX_SwRop_Src_And_Dst(const GFX_ROP_DATA_T *prsData);
extern void GFX_SwRop_NotSrc_And_Dst(const GFX_ROP_DATA_T *prsData);
extern void GFX_SwRop_Src_And_NotDst(const GFX_ROP_DATA_T *prsData);
extern void GFX_SwRop_NotSrc_And_NotDst(const GFX_ROP_DATA_T *prsData);

extern void GFX_SwRop_Src_Or_Dst(const GFX_ROP_DATA_T *prsData);
extern void GFX_SwRop_NotSrc_Or_Dst(const GFX_ROP_DATA_T *prsData);
extern void GFX_SwRop_Src_Or_NotDst(const GFX_ROP_DATA_T *prsData);
extern void GFX_SwRop_NotSrc_Or_NotDst(const GFX_ROP_DATA_T *prsData);

extern INT32 GFX_SwRopBitBlt(const GFX_ROP_DATA_T *prsData);

extern INT32 GFX_SwMemCompare(const GFX_ROP_DATA_T *prsData);

extern void GFX_SwSetMemCompareResult(INT32 i4Val);

extern INT32 GFX_SwGetMemCompareResult(void);

extern void GFX_SwSetIdx2DirComp(UINT8 **ppu1DestFb, UINT32 u4DstCM,
        const UINT8 *pu1PaleBase, UINT32 u4PaleIdx);

extern void GFX_SwGetIdx2DirComp(UINT8 **ppu1DestFb, UINT8 *pu1SrcByte);

extern UINT32 GFX_SwGetPaleIdx(UINT32 u4CharCM, UINT32 u4ShiftBit, UINT8 u1SrcByte);

extern INT32 GFX_SwIndexToDirectBitBlt(const GFX_IDX2DIR_DATA_T *prsData);

extern INT32 GFX_SwHoriLineToVertLine(const GFX_HORI2VERT_DATA_T *prsData);

extern INT32 GFX_SwClipping(INT32 i4Value);

#endif //#if defined(GFX_ENABLE_SW_MODE)

#endif // GFX_SW_H

