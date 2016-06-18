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
 * $RCSfile: gfx_drvif.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_drvif.c
 *  internal public interfaces of gfx driver
 *
 */

/*
 lint inhibition(s)
 506 - Constant value Boolean [MTK Rule 6.1.3]
 534 - Ignoring return value of function
 774 - Boolean within 'right side of && within if' always
 evaluates to True [Reference: file gfx_if.c: lines 919, 935]
 826 - Suspicious pointer-to-pointer conversion (area too small)
 831 - Loss of sign (assignment) (signed char to unsigned long)
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "gfx_if.h"
#include "gfx_drvif.h"
#include "gfx_dif.h"
#include "gfx_hw.h"
#include "gfx_cmdque.h"

#ifdef __KERNEL__
#include "fbm_drvif.h"
#endif

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

/** gfx default internal buffer size (used in alpha composition)
 *  8 lines, 16bpp per pixel, 1920 pixels per line
 */

#define GFX_TIME_OUT        (30 * 1000)     // 30 secs

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------


// acronym of gfx register name in register file
#define REG(XREG)       _prRegFile->rField.XREG

// push back relative register value in register file.
#if defined(GFX_RISC_MODE)  // risc mode
#define GFX_CMDENQ(XREG)        \
    (GFX_RiscPushBack((INT32)(XREG), _au4RegFile[(INT32)(XREG)]))
#else                       // cmdque mode
#define GFX_CMDENQ(XREG)        \
    (GFX_CmdQuePushBack((INT32)(XREG), _au4RegFile[(INT32)(XREG)]))
#endif // #if defined(GFX_RISC_MODE)

static BOOL _fgGfxEnableBurstRead = 0;
static UINT32 _fgGfxBurstReadMode = 0;

//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

static const UINT8 _auColorModeShift[] =
{ // byte_per_pixel
        2, // CM_YCbCr_CLUT2
                1, // CM_YCbCr_CLUT4
                0, // CM_YCbCr_CLUT8
                0, // CM_Reserved0
                1, // CM_CbYCrY422_DIRECT16
                1, // CM_YCbYCr422_DIRECT16
                2, // CM_AYCbCr8888_DIRECT32
                0, // CM_Reserved1
                2, // CM_RGB_CLUT2
                1, // CM_RGB_CLUT4
                0, // CM_RGB_CLUT8
                1, // CM_RGB565_DIRECT16
                1, // CM_ARGB1555_DIRECT16
                1, // CM_ARGB4444_DIRECT16
                2, // CM_ARGB8888_DIRECT32
                0 // CM_Reserved2
        };

// output/input address base
static UINT8 *_pu1DstBase;
static UINT8 *_pu1SrcBase;

// output/input region line size
static UINT32 _u4DstPitch;
static UINT32 _u4SrcPitch;

// output/input region color mode
static UINT32 _u4DstCM;
static UINT32 _u4SrcCM;

static UINT32 _u4DstShift;
static UINT32 _u4SrcShift;

// for "index to direct color bitblt" use
static UINT8 *_pu1Idx2DirSrcBase;
static UINT32 _u4CharCM;

static UINT32 _au4RegFile[GREG_FILE_SIZE];
static UINT32 _u4GfxBltSwitch;

#ifdef __KERNEL__
static HANDLE_T _semGfxOp = NULL_HANDLE;
static HANDLE_T _semGfxCmdque = NULL_HANDLE;
#endif

/*lint -save -e826 */
static MI_DIF_UNION_T *_prRegFile = (MI_DIF_UNION_T *) (void*) _au4RegFile;
/*lint -restore */

// internal working buffer for alpha-composition use
static UINT8 _au1WorkingBuf[GFX_DEF_BUF_SIZE];
static UINT8 *_pu1WorkingBuf = _au1WorkingBuf;
static UINT32 _u4WorkingBufSize = GFX_DEF_BUF_SIZE;
static UINT32 _u4WorkingPitch;

// normal driver
#if !defined(GFX_MINI_DRIVER)
static UINT8* _pu1BmpBase;
static UINT32 _u4BmpWidth;
static UINT32 _u4BmpHeight;
#endif // #if !defined(GFX_MINI_DRIVER)

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** _GfxAlphaCompPass
 *  alpha compose pass.
 */
//-------------------------------------------------------------------------
static void _GfxAlphaCompPass(UINT32 u4Width, UINT32 u4Height)
{
    ASSERT_USER();

    REG(fg_OP_MODE) = (INT32) OP_ALPHA_COMPOSE;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_BITBLT_MODE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GfxAlphaCompPass0
 *  alpha composite pass 0.
 *  apply ext alpha to color.
 */
//-------------------------------------------------------------------------
static void _GfxAlphaCompPass0(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Ar)
{
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    UNUSED(u4DstX);
    UNUSED(u4DstY);

    // pass 0 : apply ar to source (put into working buffer)
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4DstShift));
    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = (UINT32)(_pu1WorkingBuf);
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG((UINT32)(_pu1WorkingBuf));

    REG(fg_ALPHA_VALUE) = u4Ar;
    REG(fg_ALCOM_PASS) = 0;

    REG(fg_OSD_WIDTH) = (_u4WorkingPitch >> 4);
    REG(fg_SRC_PITCH) = (_u4SrcPitch >> 4);

    GFX_CMDENQ(GREG_S_OSD_WIDTH);
}

//-------------------------------------------------------------------------
/** _GfxAlphaCompPass1a
 *  alpha composite pass 1a.
 *  apply pixel alpha to color.
 */
//-------------------------------------------------------------------------
static void _GfxAlphaCompPass1a(void)
{
    ASSERT_USER();

    // pass 1a : apply source alpha to color components
    REG(fg_SRC_BSAD) = (UINT32)(_pu1WorkingBuf);
    REG(fg_DST_BSAD) = (UINT32)(_pu1WorkingBuf);

    REG(fg_ALPHA_VALUE) = 255;
    REG(fg_ALCOM_PASS) = 1;

    REG(fg_OSD_WIDTH) = (_u4WorkingPitch >> 4);
    REG(fg_SRC_PITCH) = (_u4WorkingPitch >> 4);

    GFX_CMDENQ(GREG_S_OSD_WIDTH);
}

//-------------------------------------------------------------------------
/** _GfxAlphaCompPass1b
 *  alpha composite pass 1b.
 *  apply pixel alpha to color.
 */
//-------------------------------------------------------------------------
static void _GfxAlphaCompPass1b(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY)
{
    UINT32 ui4_dst_addr = 0x0;

    ASSERT_USER();

    UNUSED(u4SrcX);
    UNUSED(u4SrcY);

    // pass 1b : apply destination alpha to color components
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));

    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);
    REG(fg_SRC_BSAD) = ui4_dst_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_ALPHA_VALUE) = 255;
    REG(fg_ALCOM_PASS) = 1;

    REG(fg_OSD_WIDTH) = (_u4DstPitch >> 4);
    REG(fg_SRC_PITCH) = (_u4DstPitch >> 4);

    GFX_CMDENQ(GREG_S_OSD_WIDTH);
}

//-------------------------------------------------------------------------
/** _GfxAlphaCompPass2
 *  alpha composite pass 2.
 *
 */
//-------------------------------------------------------------------------
static void _GfxAlphaCompPass2(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Mode)
{
    UINT32 ui4_dst_addr = 0x0;

    ASSERT_USER();

    UNUSED(u4SrcX);
    UNUSED(u4SrcY);

    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));

    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);
    REG(fg_SRC_BSAD) = (UINT32)(_pu1WorkingBuf);
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG((UINT32)(_pu1WorkingBuf));

    REG(fg_ALPHA_VALUE) = u4Mode;
    REG(fg_ALCOM_PASS) = 2;

    REG(fg_OSD_WIDTH) = (_u4DstPitch >> 4);
    REG(fg_SRC_PITCH) = (_u4WorkingPitch >> 4);

    GFX_CMDENQ(GREG_S_OSD_WIDTH);
}

//-------------------------------------------------------------------------
/** _GfxAlphaCompPass3
 *  alpha composite pass 3.
 *  extrace color from composite color.
 */
//-------------------------------------------------------------------------
static void _GfxAlphaCompPass3(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY)
{
    UINT32 ui4_dst_addr = 0x0;

    ASSERT_USER();

    UNUSED(u4SrcX);
    UNUSED(u4SrcY);

    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));

    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);
    REG(fg_SRC_BSAD) = ui4_dst_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_ALPHA_VALUE) = 255;
    REG(fg_ALCOM_PASS) = 3;

    REG(fg_OSD_WIDTH) = (_u4DstPitch >> 4);
    REG(fg_SRC_PITCH) = (_u4DstPitch >> 4);

    GFX_CMDENQ(GREG_S_OSD_WIDTH);
}

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** _GFX_SetDst
 *  set output region.
 *  set output base.
 *  set output color mode.
 *  set output line size.
 *  set output address shift.
 *  enque color mode and line size registers.
 */
//-------------------------------------------------------------------------
void _GFX_SetDst(UINT8 *pu1Base, UINT32 u4ColorMode, UINT32 u4Pitch)
{
    ASSERT_USER();

    // check 128 bits (16 bytes) alignment
    GFX_CHK_PITCH(u4Pitch);

    GFX_ASSERT(pu1Base != NULL);

    _pu1DstBase = pu1Base;
    _u4DstCM = u4ColorMode;
    _u4DstPitch = u4Pitch;
    _u4DstShift = _auColorModeShift[u4ColorMode];

    /*
     direct_log_printf(NULL,"%s,[u4ColorMode=0x%x,u4Pitch=0x%x,pu1Base=0x%x]\n",
     __FUNCTION__,u4ColorMode,u4Pitch,(UINT32)pu1Base);
     */

    REG(fg_CM) = u4ColorMode;
    REG(fg_OSD_WIDTH) = (u4Pitch >> 4);

    GFX_CMDENQ(GREG_S_OSD_WIDTH);
}

//-------------------------------------------------------------------------
/** _GFX_SetSrc
 *  set input region.
 *  set input base.
 *  set input color mode.
 *  set input line size.
 *  set input address shift.
 *  enque color mode and line size registers.
 */
//-------------------------------------------------------------------------
void _GFX_SetSrc(UINT8 *pu1Base, UINT32 u4ColorMode, UINT32 u4Pitch)
{
    ASSERT_USER();

    // check 128 bits (16 bytes) alignment
    GFX_CHK_PITCH(u4Pitch);

    GFX_ASSERT(pu1Base != NULL);

    /*
     direct_log_printf(NULL,"%s,[u4ColorMode=0x%x,u4Pitch=0x%x,pu1Base=0x%x]\n",
     __FUNCTION__,u4ColorMode,u4Pitch,(UINT32)pu1Base);
     */

    _pu1SrcBase = pu1Base;
    _u4SrcCM = u4ColorMode;
    _u4SrcPitch = u4Pitch;
    _u4SrcShift = _auColorModeShift[u4ColorMode];

    REG(fg_SRC_CM) = u4ColorMode;
    REG(fg_SRC_PITCH) = (u4Pitch >> 4);

    GFX_CMDENQ(GREG_S_OSD_WIDTH);
}

void _GFX_ResetSrcDstParam(void)
{
    GFX_CMDENQ(GREG_RECT_COLOR);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);

    GFX_CMDENQ(GREG_BITBLT_MODE);
    GFX_CMDENQ(GREG_ALCOM_LOOP);

    GFX_CMDENQ(GREG_STR_BLT_H);
    GFX_CMDENQ(GREG_STR_BLT_V);
    GFX_CMDENQ(GREG_STR_DST_SIZE);

    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_KEY_DATA0);
    GFX_CMDENQ(GREG_KEY_DATA1);
    REG(fg_FIRE) = 0;
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_SetAlpha
 *  set alpha value.
 *  alpha value used in.
 *  alpha blending.
 *  alpha composition.
 */
//-------------------------------------------------------------------------
void _GFX_SetAlpha(UINT32 u4Alpha)
{
    ASSERT_USER();

    REG(fg_ALPHA_VALUE) = u4Alpha;
    GFX_CMDENQ(GREG_BITBLT_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_SetColor
 *  set color.
 *  color value used in.
 *  rectangle fill.
 *  hline.
 *  vline.
 */
//-------------------------------------------------------------------------
void _GFX_SetColor(UINT32 u4Color)
{
    ASSERT_USER();

    REG(fg_RECT_COLOR) = u4Color;
    GFX_CMDENQ(GREG_RECT_COLOR);
}

//-------------------------------------------------------------------------
/** _GFX_Fill
 *  rectangle fill.
 *  calculate start memory address base + (y * width) + x.
 *  set fill width.
 *  set fill height.
 *  enque output address.
 *  enque input size.
 *  enque gfx mode (include fire control).
 */
//-------------------------------------------------------------------------
void _GFX_Fill(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height)
{
    UINT32 ui4_dst_addr = 0x0;

    ASSERT_USER();

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_RECT_FILL;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    /*
     direct_log_printf(NULL,"%s,[u4DstX=%d,u4DstY=%d,u4Width=%d,u4Height=%d,ui4_dst_addr=0x%x]\n",
     __FUNCTION__,
     u4DstX,u4DstY,u4Width,u4Height,ui4_dst_addr);
     */

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_RECT_COLOR);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_HLine
 *  draw horizontal line.
 *  calculate output address.
 *  set operation mode.
 *  set width.
 *  enque relative registers.
 */
//-------------------------------------------------------------------------
void _GFX_HLine(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width)
{
    UINT32 ui4_dst_addr = 0x0;

    ASSERT_USER();

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_DRAW_HLINE;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_VLine
 *  draw vertical line.
 *  calculate output address.
 *  set operation mode.
 *  set height.
 *  enque relative registers.
 */
//-------------------------------------------------------------------------
void _GFX_VLine(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Height)
{
    UINT32 ui4_dst_addr = 0x0;

    ASSERT_USER();

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_DRAW_VLINE;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_SetBltOpt
 *  set bitblt option.
 *  set transparent enable bit.
 *  set color change enable bit.
 *  set keynot enable bit.
 *  set clip enable bit.
 *  set 4bpp special mode bit.
 *  set color key min.
 *  set color key max.
 *  set color conversion enable to false.
 *  enque color key min and max.
 */
//-------------------------------------------------------------------------
void _GFX_SetBltOpt(UINT32 u4Switch, UINT32 u4ColorMin, UINT32 u4ColorMax)
{
    ASSERT_USER();

    GFX_ASSERT(!((u4Switch & D_GFXFLAG_TRANSPARENT) &&
                    (u4Switch & D_GFXFLAG_COLORCHANGE)));

    REG(fg_TRANS_ENA) = (u4Switch & D_GFXFLAG_TRANSPARENT) ? 1 : 0;
    REG(fg_KEYNOT_ENA) = (u4Switch & D_GFXFLAG_KEYNOT) ? 1 : 0;
    REG(fg_COLCHG_ENA) = (u4Switch & D_GFXFLAG_COLORCHANGE) ? 1 : 0;  
    REG(fg_DSTPITCH_DEC) = (u4Switch & D_GFXFLAG_FLIP) ? 1 : 0;
    REG(fg_MIRROR) = (u4Switch & D_GFXFLAG_MIRROR) ? 1 : 0;
    if (u4Switch & D_GFXFLAG_OVERLAP)
    {
        REG(fg_DSTPITCH_DEC) = (UINT32)1;
        REG(fg_MIRROR) = (UINT32)1;
        REG(fg_SRCPITCH_DEC) = (UINT32) 1;
        REG(fg_SRCMIRROR) = (UINT32) 1;
    }
    REG(fg_COLOR_KEY_MIN) = u4ColorMin;
    REG(fg_COLOR_KEY_MAX) = u4ColorMax;
    REG(fg_CFMT_ENA) = (u4Switch & D_GFXFLAG_CFMT_ENA) ? 1 : 0;

    GFX_CMDENQ(GREG_KEY_DATA0);
    GFX_CMDENQ(GREG_KEY_DATA1);
    _u4GfxBltSwitch = u4Switch;

}

//-------------------------------------------------------------------------
/** _GFX_Set_Src_ColorKey
 *  bitblt.
 *  calculate output address.
 *  calculate input address.
 *  set bitblt op mode.
 *  set color conversion flag.
 *  enque output address register.
 *  enque input address register.
 *  enque size and clip registers.
 *  enque bitblp option register.
 *  enque mode register.
 */
//-------------------------------------------------------------------------
void _GFX_Set_Src_ColorKey(UINT32 u4SrcColorKey)
{
    REG(fg_COLOR_KEY_MIN) = (u4SrcColorKey | 0x00FFFFFF);
    REG(fg_COLOR_KEY_MAX) = (u4SrcColorKey | 0xFF000000);

    GFX_CMDENQ(GREG_KEY_DATA0);
    GFX_CMDENQ(GREG_KEY_DATA1);

    return;
}

//-------------------------------------------------------------------------
/** _GFX_BitBlt
 *  bitblt.
 *  calculate output address.
 *  calculate input address.
 *  set bitblt op mode.
 *  set color conversion flag.
 *  enque output address register.
 *  enque input address register.
 *  enque size and clip registers.
 *  enque bitblp option register.
 *  enque mode register.
 */
//-------------------------------------------------------------------------
void _GFX_BitBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX, UINT32 u4DstY,
        UINT32 u4Width, UINT32 u4Height)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    if ((_u4GfxBltSwitch & D_GFXFLAG_FLIP) &&
            (_u4GfxBltSwitch & D_GFXFLAG_MIRROR))
    {
        ui4_dst_addr = (UINT32)(_pu1DstBase +
                ((u4DstY + u4Height - 1) * _u4DstPitch) +
                ((u4DstX + u4Width - 1) << _u4DstShift));
    }
    else if (_u4GfxBltSwitch & D_GFXFLAG_FLIP)
    {
        ui4_dst_addr = (UINT32)(_pu1DstBase +
                ((u4DstY + u4Height - 1) * _u4DstPitch) +
                (u4DstX << _u4DstShift));
    }
    else if (_u4GfxBltSwitch & D_GFXFLAG_MIRROR)
    {
        ui4_dst_addr = (UINT32)(_pu1DstBase +
                (u4DstY * _u4DstPitch) +
                ((u4DstX + u4Width - 1) << _u4DstShift));
    }
    else if (_u4GfxBltSwitch & D_GFXFLAG_OVERLAP)
    {
        ui4_dst_addr = (UINT32)(_pu1DstBase +
                ((u4DstY + u4Height - 1) * _u4DstPitch) +
                ((u4DstX + u4Width - 1) << _u4DstShift));
    }

    else
    {
        ui4_dst_addr = (UINT32)(_pu1DstBase +
                (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    }

    if (_u4GfxBltSwitch & D_GFXFLAG_OVERLAP)
    {
        ui4_src_addr = (UINT32)(_pu1SrcBase +
                ((u4SrcY + u4Height - 1) * _u4SrcPitch) +
                ((u4SrcX + u4Width - 1) << _u4SrcShift));
    }
    else
    {
        //source base address
        ui4_src_addr = (UINT32)(_pu1SrcBase +
                (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));
    }

    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32)OP_BITBLT;
    REG(fg_CFMT_ENA) = (UINT32)(_u4DstCM != _u4SrcCM);
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    //REG(fg_A1555_EN)       = GFX_ENABLE;
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_SET_BURST_READ();
    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_BITBLT_MODE);
    GFX_CMDENQ(GREG_G_MODE);

    // reset bitblt option
    _GFX_SetBltOpt(0, 0x0, 0xffffffff);
}

//-------------------------------------------------------------------------
/** _GFX_Blend
 *  alpha blending.
 *  calculate input and output address.
 *  set op mode.
 *  set width and height.
 *  enque registers.
 */
//-------------------------------------------------------------------------
void _GFX_Blend(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX, UINT32 u4DstY,
        UINT32 u4Width, UINT32 u4Height)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    GFX_ASSERT(_u4SrcCM == _u4DstCM);

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    //source base address
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));
    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_ALPHA_BLEND;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_BITBLT_MODE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_Compose
 *  alpha composition.
 *  set input gfx region.
 *  set output gfx region.
 *  do pass 0 ~ pass 3.
 */
//-------------------------------------------------------------------------
/*lint -save -e534 -e774 -e831  */
void _GFX_Compose(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX, UINT32 u4DstY,
        UINT32 u4Width, UINT32 u4Height, UINT32 u4Ar, UINT32 u4Mode)
{
    UINT32 u4MaxLines;
    UINT32 u4Line, u4TotalLines;
    INT32 i4MhpPatch = GFX_ENABLE;
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    GFX_ASSERT(_u4SrcCM == _u4DstCM);

    // Add CLEAR, SRC, SRC_OVER patches for MHP project
    // 1. CLEAR    -> use Rect Fill (fill 0x0 in rect_color)
    // 2. SRC      -> use Pass0 and Pass5 of Alpha Composition
    // 3. SRC_OVER -> consider two corner cases
    //          case1: Ar=1.0 & As=1.0
    //          case2: Ar=1.0 & As=0.0
    //          note: Ar=0.0 is the same as case2
    if (((UINT32) CM_ARGB8888_DIRECT32 == _u4SrcCM)
            && ((UINT32) CM_ARGB8888_DIRECT32 == _u4DstCM) && (GFX_ENABLE
            == i4MhpPatch))
    {
        if ((UINT32) E_AC_CLEAR == u4Mode)
        {
            UINT32 u4RectColor = REG(fg_RECT_COLOR);

            GFX_SetColor(0x0);
            GFX_Fill(u4DstX, u4DstY, u4Width, u4Height);

            // restore original value
            GFX_SetColor(u4RectColor);

            return;
        }
        else if ((UINT32) E_AC_SRC == u4Mode)
        {
            if (0 == u4Ar)
            {
                UINT32 u4RectColor = REG(fg_RECT_COLOR);

                GFX_SetColor(0x0);
                GFX_Fill(u4DstX, u4DstY, u4Width, u4Height);

                // restore original value
                GFX_SetColor(u4RectColor);
            }
            else
            {
                // 1. Pass 0
                GFX_AlphaComposePass(u4SrcX, u4SrcY, u4DstX, u4DstY, u4Width,
                        u4Height, 0, u4Ar);
                GFX_Flush(); // flush queue
            }

            return;
        }
        else if (((UINT32) E_AC_SRC_OVER == u4Mode) && (0 == u4Ar))
        {
            UINT32 u4RectColor = REG(fg_RECT_COLOR);
            UINT32 u4MinColor = REG(fg_COLOR_KEY_MIN);
            UINT32 u4MaxColor = REG(fg_COLOR_KEY_MAX);
            UINT32 u4Switch = 0x0;

            if (REG(fg_TRANS_ENA))
            {
                u4Switch = (u4Switch | D_GFXFLAG_TRANSPARENT);
            }

            if (REG(fg_KEYNOT_ENA))
            {
                u4Switch = (u4Switch | D_GFXFLAG_KEYNOT);
            }

            if (REG(fg_COLCHG_ENA))
            {
                u4Switch = (u4Switch | D_GFXFLAG_COLORCHANGE);
            }

            // BitBlt with ColorChange (from Dst to Dst)
            GFX_SetColor(0x0);
            GFX_SetBltOpt(D_GFXFLAG_COLORCHANGE, 0x00000000, 0x00FFFFFF);

            // set new src & dst pitch
            REG(fg_OSD_WIDTH) = _u4DstPitch >> 4;
            REG(fg_SRC_PITCH) = _u4DstPitch >> 4;

            ui4_dst_addr = (UINT32)(
                    _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX
                            << _u4DstShift));
            ui4_src_addr = (UINT32)(
                    _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX
                            << _u4DstShift));
            REG(fg_SRC_BSAD) = ui4_src_addr;
            REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
            REG(fg_DST_BSAD) = ui4_dst_addr;
            REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

            REG(fg_OP_MODE) = (UINT32) OP_BITBLT;
            REG(fg_CFMT_ENA) = (UINT32)(_u4DstCM != _u4SrcCM);
            REG(fg_SRC_WIDTH) = u4Width;
            REG(fg_SRC_HEIGHT) = (u4Height - 1);
            REG(fg_FIRE) = GFX_ENGINE_FIRE;

            GFX_CMDENQ(GREG_S_OSD_WIDTH);
            GFX_CMDENQ(GREG_DST_BSAD);
            GFX_CMDENQ(GREG_S_OSD_WIDTH);
            GFX_CMDENQ(GREG_SRC_BSAD);
            GFX_CMDENQ(GREG_ADDR_MSB1);
            GFX_CMDENQ(GREG_SRC_SIZE);
            GFX_CMDENQ(GREG_BITBLT_MODE);
            GFX_CMDENQ(GREG_G_MODE);

            // restore original value
            REG(fg_OSD_WIDTH) = _u4DstPitch >> 4;
            REG(fg_SRC_PITCH) = _u4SrcPitch >> 4;
            ui4_src_addr = (UINT32)(
                    _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX
                            << _u4SrcShift));
            REG(fg_SRC_BSAD) = ui4_src_addr;
            REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
            GFX_SetColor(u4RectColor);
            GFX_SetBltOpt(u4Switch, u4MinColor, u4MaxColor);
            GFX_CMDENQ(GREG_S_OSD_WIDTH);
            GFX_CMDENQ(GREG_SRC_BSAD);
            GFX_CMDENQ(GREG_ADDR_MSB1);
            GFX_CMDENQ(GREG_BITBLT_MODE);

            return;
        }
        else
        {
            //do nothing, for lint happy
        }
    }

    // calculate how many lines can we handle using working buffer
    _u4WorkingPitch = ((u4Width << _u4DstShift) + 0xf) & ~0xf;
    u4MaxLines = (_u4WorkingBufSize / _u4WorkingPitch);

    if (u4Height > u4MaxLines)
    {
        // break down whole area into sets of lines
        u4TotalLines = (u4Height - u4MaxLines);
        for (u4Line = 0; u4Line < u4TotalLines; u4Line += u4MaxLines)
        {
            _GfxAlphaCompPass0(u4SrcX, (u4SrcY + u4Line), u4DstX,
                    (u4DstY + u4Line), u4Ar);
            _GfxAlphaCompPass(u4Width, u4MaxLines);
            _GfxAlphaCompPass1a();
            _GfxAlphaCompPass(u4Width, u4MaxLines);
            _GfxAlphaCompPass1b(u4SrcX, (u4SrcY + u4Line), u4DstX,
                    (u4DstY + u4Line));
            _GfxAlphaCompPass(u4Width, u4MaxLines);
            _GfxAlphaCompPass2(u4SrcX, (u4SrcY + u4Line), u4DstX,
                    (u4DstY + u4Line), u4Mode);
            _GfxAlphaCompPass(u4Width, u4MaxLines);
            _GfxAlphaCompPass3(u4SrcX, (u4SrcY + u4Line), u4DstX,
                    (u4DstY + u4Line));
            _GfxAlphaCompPass(u4Width, u4MaxLines);
        }
        u4MaxLines = (u4Height - u4Line);
    }
    else
    {
        u4Line = 0;
        u4MaxLines = u4Height;
    }

    // process last few lines
    if (u4MaxLines)
    {
        _GfxAlphaCompPass0(u4SrcX, (u4SrcY + u4Line), u4DstX,
                (u4DstY + u4Line), u4Ar);
        _GfxAlphaCompPass(u4Width, u4MaxLines);
        _GfxAlphaCompPass1a();
        _GfxAlphaCompPass(u4Width, u4MaxLines);
        _GfxAlphaCompPass1b(u4SrcX, (u4SrcY + u4Line), u4DstX,
                (u4DstY + u4Line));
        _GfxAlphaCompPass(u4Width, u4MaxLines);
        _GfxAlphaCompPass2(u4SrcX, (u4SrcY + u4Line), u4DstX,
                (u4DstY + u4Line), u4Mode);
        _GfxAlphaCompPass(u4Width, u4MaxLines);
        _GfxAlphaCompPass3(u4SrcX, (u4SrcY + u4Line), u4DstX, (u4DstY + u4Line));
        _GfxAlphaCompPass(u4Width, u4MaxLines);
    }

    if (((UINT32) CM_ARGB8888_DIRECT32 == _u4SrcCM)
            && ((UINT32) CM_ARGB8888_DIRECT32 == _u4DstCM) && (GFX_ENABLE
            == i4MhpPatch))
    {
        if ((u4Mode == (UINT32) E_AC_SRC_OVER) && (255 == u4Ar))
        {
            UINT32 u4MinColor = REG(fg_COLOR_KEY_MIN);
            UINT32 u4MaxColor = REG(fg_COLOR_KEY_MAX);
            UINT32 u4Switch = 0x0;

            if (REG(fg_TRANS_ENA))
            {
                u4Switch = (u4Switch | D_GFXFLAG_TRANSPARENT);
            }

            if (REG(fg_KEYNOT_ENA))
            {
                u4Switch = (u4Switch | D_GFXFLAG_KEYNOT);
            }

            if (REG(fg_COLCHG_ENA))
            {
                u4Switch = (u4Switch | D_GFXFLAG_COLORCHANGE);
            }

            // restore src & dst pitch
            REG(fg_OSD_WIDTH) = _u4DstPitch >> 4;
            REG(fg_SRC_PITCH) = _u4SrcPitch >> 4;
            GFX_CMDENQ(GREG_S_OSD_WIDTH);

            // do (Transparent & Keynot)-BitBlt
            GFX_SetBltOpt((D_GFXFLAG_TRANSPARENT | D_GFXFLAG_KEYNOT),
                    0xFF000000, 0xFFFFFFFF);
            GFX_BitBlt(u4SrcX, u4SrcY, u4DstX, u4DstY, u4Width, u4Height);

            // restore bltopt
            GFX_SetBltOpt(u4Switch, u4MinColor, u4MaxColor);
            GFX_CMDENQ(GREG_BITBLT_MODE);
        }
    }

    // restore src & dst pitch
    REG(fg_OSD_WIDTH) = (_u4DstPitch >> 4);
    REG(fg_SRC_PITCH) = (_u4SrcPitch >> 4);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GFX_AlphaComposePass
 *  alpha compose pass.
 *  exported for pass 4 copy color and pass 5 copy alpha.
 */
//-------------------------------------------------------------------------
void _GFX_AlphaComposePass(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pass,
        UINT32 u4Param)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;
    ASSERT_USER();

    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4DstShift));
    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_ALCOM_PASS) = u4Pass;
    REG(fg_ALPHA_VALUE) = u4Param;
    REG(fg_OSD_WIDTH) = (_u4DstPitch >> 4);
    REG(fg_SRC_PITCH) = (_u4SrcPitch >> 4);

    REG(fg_OP_MODE) = (UINT32) OP_ALPHA_COMPOSE;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_BITBLT_MODE);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_SetColCnvFmt
 *  set ycbcr to rgb color conversion format.
 *  set yc format.
 *  set video standard.
 *  set video system.
 *  set mtk swap mode.
 *  enable saturate.
 */
//-------------------------------------------------------------------------
void _GFX_SetColCnvFmt(UINT32 u4YCFmt, UINT32 u4SwapMode, UINT32 u4VidStd,
        UINT32 u4VidSys)
{
    ASSERT_USER();

    REG(fg_YC_FMT) = u4YCFmt;
    REG(fg_VSTD) = u4VidStd;
    REG(fg_VSYS) = u4VidSys;
    REG(fg_SWAP_MODE) = u4SwapMode;
    REG(fg_VSCLIP) = GFX_ENABLE;
    REG(fg_ALPHA_VALUE) = 0xFF;
}

//-------------------------------------------------------------------------
/** _GFX_SetColCnvSrc
 *  set color conversion source.
 *  luma input address, line size.
 *  chroma input address, linesize.
 *  field/frame mode.
 */
//-------------------------------------------------------------------------
void _GFX_SetColCnvSrc(UINT8 *pu1SrcLuma, UINT32 u4SrcLumaPitch,
        UINT8 *pu1SrcChroma, UINT32 u4SrcChromaPitch, UINT32 u4FieldPic)
{
    ASSERT_USER();

    REG(fg_SRC_BSAD) = (UINT32) pu1SrcLuma;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG((UINT32)pu1SrcLuma);
    REG(fg_SRC_PITCH) = (u4SrcLumaPitch >> 4);
    REG(fg_SRCCBCR_BSAD) = (UINT32) pu1SrcChroma;
    REG(fg_SRCCBCR_PITCH) = (u4SrcChromaPitch >> 4);
    REG(fg_FLD_PIC) = u4FieldPic;

    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRCCBCR_BSAD);
    GFX_CMDENQ(GREG_SRCCBCR_PITCH);
}

//-------------------------------------------------------------------------
/** _GFX_ColConv
 *  convert ycbcr to rgb.
 *  calculate output address.
 *  set output width height.
 */
//-------------------------------------------------------------------------
void _GFX_ColConv(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height)
{
    UINT32 ui4_dst_addr = 0x0;

    ASSERT_USER();

    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_YCBCR_TO_RGB;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_BITBLT_MODE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_StretchBlt
 *  stretch bitblt.
 *  calculate output address.
 *  calculate input address.
 *  calculate ration.
 *  set input width height.
 *  set output width height.
 *  enque registers.
 */
//-------------------------------------------------------------------------
void _GFX_StretchBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4SrcW,
        UINT32 u4SrcH, UINT32 u4DstX, UINT32 u4DstY, UINT32 u4DstW,
        UINT32 u4DstH)
{
    UINT32 u4MnV, u4MnH;
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    GFX_ASSERT(_u4DstShift == _u4SrcShift);

    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));
    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    // calculate m n ratio for scale up and down
    if ((u4DstW == u4SrcW) || (u4DstW == 1))
    {
        u4MnH = 0x00010000;
        u4SrcW = u4DstW;
    }
    else if (u4SrcW > u4DstW) // scale down
    {
        u4MnH = (u4SrcW << 16) / (u4DstW - 1);
        if (((u4SrcW << 16) % (u4DstW - 1)) == 0)
        {
            u4MnH--;
        }
    }
    else // (u4DstW > u4SrcW)
    {
        UINT32 u4Step;

        u4Step = (u4SrcW << 16) / (u4DstW);
        if ((u4Step * (u4DstW - 1)) < ((u4SrcW - 1) << 16))
        {
            u4MnH = (u4SrcW << 16) / (u4DstW - 1);
            if (u4MnH == 0x10000)
            {
                u4MnH--;
            }
        }
        else
        {
            u4MnH = u4Step;
        }
    }

    if ((u4DstH == u4SrcH) || (u4DstH == 1))
    {
        u4MnV = 0x00010000;
        u4SrcH = u4DstH;
    }
    else
    {
        u4MnV = (u4SrcH << 16) / (u4DstH - 1);
        if (u4MnV == 0x00010000)
        {
            u4MnV = (u4SrcH << 16) / (u4DstH);
        }
    }

    REG(fg_OP_MODE) = (UINT32) OP_STRETCH_BITBLT;
    REG(fg_SRC_WIDTH) = u4SrcW;
    REG(fg_SRC_HEIGHT) = (u4SrcH - 1);
    REG(fg_STR_DST_WIDTH) = u4DstW;
    REG(fg_STR_DST_HEIGHT) = (u4DstH - 1);
    REG(fg_STR_BLT_H) = u4MnH;
    REG(fg_STR_BLT_V) = u4MnV;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_STR_BLT_H);
    GFX_CMDENQ(GREG_STR_BLT_V);
    GFX_CMDENQ(GREG_STR_DST_SIZE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_AlphaMapBitBlt
 *  alpha map bitblt.
 *  calculate output address.
 *  calculate input address.
 *  set alpha map bitblt op mode.
 *  set color conversion flag.
 *  enque output address register.
 *  enque input address register.
 *  enque size registers.
 *  enque mode register.
 */
//-------------------------------------------------------------------------
void _GFX_AlphaMapBitBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    GFX_ASSERT(_u4SrcCM == (UINT32)CM_RGB_CLUT8);

    GFX_ASSERT((_u4DstCM == (UINT32)CM_ARGB8888_DIRECT32) ||
            (_u4DstCM == (UINT32)CM_ARGB4444_DIRECT16) ||
            (_u4DstCM == (UINT32)CM_ARGB1555_DIRECT16));

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    //source base address
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));
    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_ALPHA_MAP_BITBLT;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_SetRopOpt
 *  set rop option.
 *  rop bitblt.
 */
//-------------------------------------------------------------------------
void _GFX_SetRopOpt(UINT32 u4RopCode)
{
    ASSERT_USER();

    REG(fg_ROP_OPCODE) = u4RopCode;
    REG(fg_ROP_NO_WR) = GFX_DISABLE;
    GFX_CMDENQ(GREG_ROP);
}

//-------------------------------------------------------------------------
/** _GFX_RopBitBlt
 *  raster operation.
 *  calculate input and output address.
 *  set rop op code.
 *  set width and height.
 *  enque registers.
 */
//-------------------------------------------------------------------------
void _GFX_RopBitBlt(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX, UINT32 u4DstY,
        UINT32 u4Width, UINT32 u4Height)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    GFX_ASSERT(_u4SrcCM == _u4DstCM);

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    //source base address
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));
    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_ROP_BITBLT;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_ComposeLoop
 *  alpha composition loop mode.
 *  
 */
//-------------------------------------------------------------------------
void _GFX_ComposeLoop(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Ar,
        UINT32 u4OpCode, UINT32 u4RectSrc)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    GFX_ASSERT(_u4SrcCM == _u4DstCM);

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    //source base address
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));
    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_ALPHA_COMPOSE_LOOP;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_ALCOM_AR) = u4Ar;
    REG(fg_ALCOM_OPCODE) = u4OpCode;
    REG(fg_ALCOM_RECT_SRC) = u4RectSrc;
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_ALCOM_LOOP);
    GFX_CMDENQ(GREG_G_MODE);
}

//#ifdef CC_GFX_COMPOSITION_IMPROVE
void _GFX_ComposeLoopImprove(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height, UINT32 u4Ar,
        UINT32 u4OpCode, UINT32 u4RectSrc)
{
    UINT32 u4CMDiff;
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    if (_u4SrcCM != _u4DstCM)
        u4CMDiff = 1;
    else
        u4CMDiff = 0;

    if (u4Ar == 255)
    {
        REG(fg_ALCOM_NORMAL) = GFX_ENABLE;
    }
    else
    {
        REG(fg_ALCOM_NORMAL) = GFX_DISABLE;
    }

    REG(fg_ALCOM_DIFF_CM) = u4CMDiff;

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    //source base address
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));

    
    if(((CM_ARGB4444_DIRECT16 == _u4SrcCM)||(CM_ARGB1555_DIRECT16 == _u4SrcCM)||(CM_RGB565_DIRECT16 == _u4SrcCM))&&
        (u4Width < 8)&&(u4CMDiff))
    {
        REG(fg_SRCOWN) = 0x1;
    }
    else
    {
        REG(fg_SRCOWN) = 0x0;
    }

    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);
    REG(fg_OP_MODE) = (UINT32) OP_ALPHA_COMPOSE_LOOP;
    REG(fg_SRC_WIDTH) = u4Width;
    #if GFX_4KX2K_TEST
    REG(fg_SRC_HEIGHT) = ((u4Height-1) & 0x007FF);
    REG(fg_SRC_HEIGHT_12) = (((u4Height -1)>>11) & 0x1);
    #else
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    #endif
    REG(fg_ALCOM_AR) = u4Ar;
    REG(fg_ALCOM_OPCODE) = u4OpCode;
    REG(fg_ALCOM_RECT_SRC) = u4RectSrc;
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_SET_BURST_READ();
    
    #if GFX_4KX2K_TEST
    REG(fg_SRC_HEIGHT_12) = 0;
    GFX_CMDENQ(GREG_0x4284);
    #endif
    GFX_CMDENQ(GREG_0x40D0);
    GFX_CMDENQ(GREG_0x40D4);
    GFX_CMDENQ(GREG_0x40D8);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_BITBLT_MODE);
    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_ALCOM_LOOP);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_G_MODE);
}
//#endif


//-------------------------------------------------------------------------
/** _GFX_SetIdx2DirSrc
 *  set input region.
 *  set input base.
 *  set input color mode.
 *  set input line size.
 *  set input address shift.
 *  enque line size registers.
 */
//-------------------------------------------------------------------------
void _GFX_SetIdx2DirSrc(UINT8 *pu1Base, UINT32 u4SrcCM)
{
    ASSERT_USER();

    // Src_pitch can be ignored, since gfx hw reads data continuously.
    GFX_ASSERT((u4SrcCM == (UINT32)E_BMP_CM_1BIT) ||
            (u4SrcCM == (UINT32)E_BMP_CM_2BIT) ||
            (u4SrcCM == (UINT32)E_BMP_CM_4BIT) ||
            (u4SrcCM == (UINT32)E_BMP_CM_8BIT));

    _pu1Idx2DirSrcBase = pu1Base;
    _u4CharCM = u4SrcCM;
}

//-------------------------------------------------------------------------
/** _GFX_SetIdx2DirOpt
 *  set index to direct color bitblt option.
 *  set palette start address in bytes in DRAM (must 16 bytes aligned)
 *  set flag of left pixel index location in one byte of source.
 *  set flag of source line start byte aligned.
 *  enque idx2dir registers.
 */
//-------------------------------------------------------------------------
void _GFX_SetIdx2DirOpt(UINT8 *pu1PaletteBase, UINT32 u4MsbLeft,
        UINT32 u4StartByteAligned)
{
    ASSERT_USER();

    // check 128 bits (16 bytes) alignment
    GFX_CHK_PITCH((UINT32)pu1PaletteBase);

    REG(fg_PAL_BSAD) = (UINT32)(pu1PaletteBase);
    REG(fg_MSB_LEFT) = u4MsbLeft;
    REG(fg_LN_ST_BYTE_AL) = u4StartByteAligned;

    GFX_CMDENQ(GREG_IDX2DIR);
}

//-------------------------------------------------------------------------
/** _GFX_Idx2DirBitBlt
 *  index to direct color bitblt.
 *  
 */
//-------------------------------------------------------------------------
void _GFX_Idx2DirBitBlt(UINT32 u4DstX, UINT32 u4DstY, UINT32 u4Width,
        UINT32 u4Height)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    //source base address
    ui4_src_addr = (UINT32)(_pu1Idx2DirSrcBase);

    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_IDX_TO_DIR_BITBLT;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_CHAR_CM) = _u4CharCM; // source color mode
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_2DMemCompare
 *  2D memory compare operation.
 *  calculate input and output address.
 *  set rop op code = XOR.
 *  set NO_WR = 1.
 *  set width and height.
 *  enque registers.
 */
//-------------------------------------------------------------------------
void _GFX_2DMemCompare(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4Width, UINT32 u4Height)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    GFX_ASSERT(_u4SrcCM == _u4DstCM);

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    //source base address
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));
    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_ROP_BITBLT;
    REG(fg_ROP_OPCODE) = (UINT32) E_ROP_SRC_XOR_DST;
    REG(fg_ROP_NO_WR) = GFX_ENABLE;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_ROP);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_SetHoriToVertLineOpt
 *  Set DSTPITCH_DEC 0 or 1 to determine whether the bitmap
 *  rotated by 90" clockwise or Counterclockwise.
 */
//-------------------------------------------------------------------------
void _GFX_SetHoriToVertLineOpt(UINT32 u4Is90dCcw)
{
    ASSERT_USER();

    REG(fg_DSTPITCH_DEC) = u4Is90dCcw;
    GFX_CMDENQ(GREG_BITBLT_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_HoriToVertLine
 *  gfx horizontal line to vertical line operation.
 *  calculate output address.
 *  calculate input address.
 *  enque output address register.
 *  enque input address register.
 *  enque width and height registers.
 *  enque str_width and str_height registers.
 *  enque mode register.
 */
//-------------------------------------------------------------------------
void _GFX_HoriToVertLine(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4DstX,
        UINT32 u4DstY, UINT32 u4HoriLineWidth)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    GFX_ASSERT(u4HoriLineWidth != 0);

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    //source base address
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));
    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_LINE_HORI_TO_VERT;
    REG(fg_SRC_WIDTH) = u4HoriLineWidth;
    REG(fg_SRC_HEIGHT) = 0;
    REG(fg_STR_DST_WIDTH) = 1;
    REG(fg_STR_DST_HEIGHT) = (u4HoriLineWidth - 1);
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_STR_DST_SIZE);
    GFX_CMDENQ(GREG_G_MODE);
}

// normal driver
#if !defined(GFX_MINI_DRIVER)
//-------------------------------------------------------------------------
/** _GFX_SetGradOpt
 *  gfx set gradient options.
 *  x increment pixels.
 *  y increment pixels.
 *  x delta in color components (a, r, g, b), 7.1 format.
 *  y delta in color components.
 */
//-------------------------------------------------------------------------
void _GFX_SetGradOpt(UINT32 u4IncX, UINT32 u4IncY, const UINT32 asu4DeltaX[4],
        const UINT32 asu4DeltaY[4])
{
    ASSERT_USER();

    GFX_ASSERT(asu4DeltaX != NULL);
    GFX_ASSERT(asu4DeltaY != NULL);

    REG(fg_GRAD_X_PIX_INC) = u4IncX;
    REG(fg_GRAD_Y_PIX_INC) = u4IncY;

    REG(fg_DELTA_X_C0) = asu4DeltaX[0] & 0xff;
    REG(fg_DELTA_X_C1) = asu4DeltaX[1] & 0xff;
    REG(fg_DELTA_X_C2) = asu4DeltaX[2] & 0xff;
    REG(fg_DELTA_X_C3) = asu4DeltaX[3] & 0xff;

    REG(fg_DELTA_Y_C0) = asu4DeltaY[0] & 0xff;
    REG(fg_DELTA_Y_C1) = asu4DeltaY[1] & 0xff;
    REG(fg_DELTA_Y_C2) = asu4DeltaY[2] & 0xff;
    REG(fg_DELTA_Y_C3) = asu4DeltaY[3] & 0xff;

    GFX_CMDENQ(GREG_GRAD_X_DELTA);
    GFX_CMDENQ(GREG_GRAD_Y_DELTA);
}

//-------------------------------------------------------------------------
/** _GFX_GradFill
 *  gfx gradient rectangle fill.
 *  calculate output address.
 *  set gradient fill op mode.
 *  set width and height.
 *  enque registers.
 */
//-------------------------------------------------------------------------
void _GFX_GradFill(UINT32 u4X, UINT32 u4Y, UINT32 u4Width, UINT32 u4Height,
        UINT32 u4Mode)
{
    UINT32 ui4_dst_addr = 0x0;

    ASSERT_USER();

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4Y * _u4DstPitch) + (u4X << _u4DstShift));
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_GRAD_FILL;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = (u4Height - 1);
    REG(fg_GRAD_MODE) = u4Mode;
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_GRAD_XY_INC);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_DMA
 *  gfx dram to dram dma function.
 *  32bit align.
 *  set output address.
 *  set input address.
 *  set number of 32bit words.
 */
//-------------------------------------------------------------------------
void _GFX_DMA(UINT32 *pu4Dst, UINT32 *pu4Src, UINT32 u4NumWrd)
{
    UINT32 u4Width, u4Height;

    ASSERT_USER();

    GFX_SetBltOpt(D_GFXFLAG_NONE, 0, 0);

    u4Width = u4NumWrd & 0x7ff;
    u4Height = u4NumWrd >> 11;

    REG(fg_DST_BSAD) = (UINT32)(pu4Dst);
    REG(fg_SRC_BSAD) = (UINT32)(pu4Src);
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(((UINT32)pu4Dst));
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(((UINT32)pu4Dst));

    REG(fg_OP_MODE) = (UINT32) OP_DMA;
    REG(fg_CFMT_ENA) = GFX_DISABLE;
    REG(fg_SRC_WIDTH) = u4Width;
    REG(fg_SRC_HEIGHT) = u4Height;
    REG(fg_CM) = (UINT32) CM_ARGB8888_DIRECT32;
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_BITBLT_MODE);
    GFX_CMDENQ(GREG_G_MODE);

    // restore dst color mode
    REG(fg_CM) = _u4DstCM;
}

//-------------------------------------------------------------------------
/** _GFX_SetBmp
 *  gfx set BITMAP mode
 *
 */
//-------------------------------------------------------------------------
void _GFX_SetBmp(UINT8 *pu1Base, UINT32 u4BmpColorMode, UINT32 u4Width,
        UINT32 u4Height, UINT32 u4RunLen)
{
    ASSERT_USER();

    _pu1BmpBase = pu1Base;
    _u4BmpWidth = u4Width;
    _u4BmpHeight = (u4Height - 1);

    // no support BMP_CM_8BIT now
    GFX_ASSERT((u4BmpColorMode == (UINT32)E_BMP_CM_1BIT) ||
            (u4BmpColorMode == (UINT32)E_BMP_CM_2BIT) ||
            (u4BmpColorMode == (UINT32)E_BMP_CM_4BIT));

    REG(fg_CHAR_CM) = u4BmpColorMode;
    REG(fg_RL_DEC) = (u4RunLen) ? 1 : 0;
}

//-------------------------------------------------------------------------
/** _GFX_Bmp
 *
 *
 */
//-------------------------------------------------------------------------
void _GFX_Bmp(UINT32 u4X, UINT32 u4Y)
{
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    ASSERT_USER();

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4Y * _u4DstPitch) + (u4X << _u4DstShift));
    ui4_src_addr = (UINT32) _pu1BmpBase;

    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_BITMAP_BLT;
    REG(fg_SRC_WIDTH) = _u4BmpWidth;
    REG(fg_SRC_HEIGHT) = _u4BmpHeight;
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_BITBLT_MODE);
    GFX_CMDENQ(GREG_G_MODE);
}

//-------------------------------------------------------------------------
/** _GFX_ColorMap
 *  gfx set COLORMAP mode
 *
 */
//-------------------------------------------------------------------------
void _GFX_ColorMap(UINT32 u4BmpCM, UINT32 u4CM, const UINT32 *psu4ColorMap)
{
    ASSERT_USER();

    GFX_ASSERT(psu4ColorMap != NULL);

    if ((UINT32) E_BMP_CM_2BIT == u4BmpCM)
    {
        switch (u4CM)
        {
        case CM_YCbCr_CLUT8:
        case CM_RGB_CLUT8:
            REG(fg_COLOR_TRANS0) = (psu4ColorMap[0] & 0xff)
                    | ((psu4ColorMap[1] & 0xff) << 8) | ((psu4ColorMap[2]
                    & 0xff) << 16) | ((psu4ColorMap[3] & 0xff) << 24);
            break;

        case CM_RGB565_DIRECT16:
        case CM_ARGB1555_DIRECT16:
        case CM_ARGB4444_DIRECT16:
            REG(fg_COLOR_TRANS0) = (psu4ColorMap[0] & 0xffff)
                    | ((psu4ColorMap[1] & 0xffff) << 16);
            REG(fg_COLOR_TRANS1) = (psu4ColorMap[2] & 0xffff)
                    | ((psu4ColorMap[3] & 0xffff) << 16);
            break;

        case CM_AYCbCr8888_DIRECT32:
        case CM_ARGB8888_DIRECT32:
            REG(fg_COLOR_TRANS0) = psu4ColorMap[0];
            REG(fg_COLOR_TRANS1) = psu4ColorMap[1];
            REG(fg_COLOR_TRANS2) = psu4ColorMap[2];
            REG(fg_COLOR_TRANS3) = psu4ColorMap[3];
            break;

        default:
            return;
        }
    }
    else if ((UINT32) E_BMP_CM_4BIT == u4BmpCM)
    {
        switch (u4CM)
        {
        case CM_YCbCr_CLUT8:
        case CM_RGB_CLUT8:
            REG(fg_COLOR_TRANS0) = (psu4ColorMap[0] & 0xff)
                    | ((psu4ColorMap[1] & 0xff) << 8) | ((psu4ColorMap[2]
                    & 0xff) << 16) | ((psu4ColorMap[3] & 0xff) << 24);
            REG(fg_COLOR_TRANS1) = (psu4ColorMap[4] & 0xff)
                    | ((psu4ColorMap[5] & 0xff) << 8) | ((psu4ColorMap[6]
                    & 0xff) << 16) | ((psu4ColorMap[7] & 0xff) << 24);
            REG(fg_COLOR_TRANS2) = (psu4ColorMap[8] & 0xff)
                    | ((psu4ColorMap[9] & 0xff) << 8) | ((psu4ColorMap[10]
                    & 0xff) << 16) | ((psu4ColorMap[11] & 0xff) << 24);
            REG(fg_COLOR_TRANS3) = (psu4ColorMap[12] & 0xff)
                    | ((psu4ColorMap[13] & 0xff) << 8) | ((psu4ColorMap[14]
                    & 0xff) << 16) | ((psu4ColorMap[15] & 0xff) << 24);
            break;

        case CM_RGB565_DIRECT16:
        case CM_ARGB1555_DIRECT16:
        case CM_ARGB4444_DIRECT16:
            REG(fg_COLOR_TRANS0) = (psu4ColorMap[0] & 0xffff)
                    | ((psu4ColorMap[1] & 0xffff) << 16);
            REG(fg_COLOR_TRANS1) = (psu4ColorMap[2] & 0xffff)
                    | ((psu4ColorMap[3] & 0xffff) << 16);
            REG(fg_COLOR_TRANS2) = (psu4ColorMap[4] & 0xffff)
                    | ((psu4ColorMap[5] & 0xffff) << 16);
            REG(fg_COLOR_TRANS3) = (psu4ColorMap[6] & 0xffff)
                    | ((psu4ColorMap[7] & 0xffff) << 16);
            REG(fg_COLOR_TRANS4) = (psu4ColorMap[8] & 0xffff)
                    | ((psu4ColorMap[9] & 0xffff) << 16);
            REG(fg_COLOR_TRANS5) = (psu4ColorMap[10] & 0xffff)
                    | ((psu4ColorMap[11] & 0xffff) << 16);
            REG(fg_COLOR_TRANS6) = (psu4ColorMap[12] & 0xffff)
                    | ((psu4ColorMap[13] & 0xffff) << 16);
            REG(fg_COLOR_TRANS7) = (psu4ColorMap[14] & 0xffff)
                    | ((psu4ColorMap[15] & 0xffff) << 16);
            break;

        default:
            return;
        }
    }
    else // for lint happy
    {
        ;
    }

    GFX_CMDENQ(GREG_COL_TRAN0);
    GFX_CMDENQ(GREG_COL_TRAN1);
    GFX_CMDENQ(GREG_COL_TRAN2);
    GFX_CMDENQ(GREG_COL_TRAN3);
    GFX_CMDENQ(GREG_COL_TRAN4);
    GFX_CMDENQ(GREG_COL_TRAN5);
    GFX_CMDENQ(GREG_COL_TRAN6);
    GFX_CMDENQ(GREG_COL_TRAN7);
}
#endif // #if !defined(GFX_MINI_DRIVER)

//-------------------------------------------------------------------------
/** _GFX_SetPremult
 *  Indicate Src/Dst Premult R/W 
 *  
 */
//-------------------------------------------------------------------------
void _GFX_SetPremult(UINT32 u4PremultSrcR, UINT32 u4PremultDstR,
        UINT32 u4PremultDstW, UINT32 u4PremultOvrflw)
{

    REG(fg_PREMULT_SRC_R) = u4PremultSrcR;
    REG(fg_PREMULT_DST_R) = u4PremultDstR;
    REG(fg_PREMULT_DST_W) = u4PremultDstW;
    REG(fg_PREMULT_OVERFLOW_EN) = u4PremultOvrflw;

}

//-------------------------------------------------------------------------
/** _GFX_StretchAlphaCom
 *  New compression bitblt.
 *  
 */
//-------------------------------------------------------------------------
void _GFX_StretchAlphaCom(UINT32 u4SrcX, UINT32 u4SrcY, UINT32 u4SrcW,
        UINT32 u4SrcH, UINT32 u4DstX, UINT32 u4DstY, UINT32 u4DstW,
        UINT32 u4DstH, UINT32 u4Ar, UINT32 u4OpCode)
{
    UINT32 u4CMDiff;
    UINT32 u4MnV, u4MnH;
    BOOL fgAlcomNormal;
    UINT32 ui4_dst_addr = 0x0;
    UINT32 ui4_src_addr = 0x0;

    GFX_ASSERT(_u4DstShift == _u4SrcShift);

    if (_u4SrcCM != _u4DstCM)
    {
        u4CMDiff = 1;
    }
    else
    {
        u4CMDiff = 0;
    }

    // calculate m n ratio for scale up and down
    if ((u4DstW == u4SrcW) || (u4DstW == 1))
    {
        u4MnH = 0x00010000;
        u4SrcW = u4DstW;
    }
    else if (u4SrcW > u4DstW) // scale down
    {
        u4MnH = (u4SrcW << 16) / (u4DstW - 1);
        if (((u4SrcW << 16) % (u4DstW - 1)) == 0)
        {
            u4MnH--;
        }
    }
    else // (u4DstW > u4SrcW)
    {
        UINT32 u4Step;

        u4Step = (u4SrcW << 16) / (u4DstW);
        if ((u4Step * (u4DstW - 1)) < ((u4SrcW - 1) << 16))
        {
            u4MnH = (u4SrcW << 16) / (u4DstW - 1);
            if (u4MnH == 0x10000)
            {
                u4MnH--;
            }
        }
        else
        {
            u4MnH = u4Step;
        }
    }

    if ((u4DstH == u4SrcH) || (u4DstH == 1))
    {
        u4MnV = 0x00010000;
        u4SrcH = u4DstH;
    }
    else
    {
        u4MnV = (u4SrcH << 16) / (u4DstH - 1);
        if (u4MnV == 0x00010000)
        {
            u4MnV = (u4SrcH << 16) / (u4DstH);
        }
    }

#ifndef GOLDEN_COMPARE
    if (u4Ar == 255)
    {
        fgAlcomNormal = 1;
    }
    else
    {
        fgAlcomNormal = 0;
    }
#endif    

    REG(fg_ALCOM_NORMAL) = fgAlcomNormal;
    REG(fg_ALCOM_DIFF_CM) = u4CMDiff;

    //destination base address
    ui4_dst_addr = (UINT32)(
            _pu1DstBase + (u4DstY * _u4DstPitch) + (u4DstX << _u4DstShift));
    //source base address
    ui4_src_addr = (UINT32)(
            _pu1SrcBase + (u4SrcY * _u4SrcPitch) + (u4SrcX << _u4SrcShift));

    REG(fg_SRC_BSAD) = ui4_src_addr;
    REG(fg_SRC_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_src_addr);
    REG(fg_DST_BSAD) = ui4_dst_addr;
    REG(fg_DST_BSAD_MSB) = GFX_REG_ADDR_MSG(ui4_dst_addr);

    REG(fg_OP_MODE) = (UINT32) OP_STRETCH_ALCOM;
    REG(fg_ALCOM_AR) = u4Ar;
    REG(fg_ALCOM_OPCODE) = u4OpCode;
    REG(fg_ALCOM_RECT_SRC) = 0;
    REG(fg_SRC_WIDTH) = u4SrcW;
    REG(fg_SRC_HEIGHT) = (u4SrcH - 1);
    REG(fg_STR_DST_WIDTH) = u4DstW;
    REG(fg_STR_DST_HEIGHT) = (u4DstH - 1);
    REG(fg_STR_BLT_H) = u4MnH;
    REG(fg_STR_BLT_V) = u4MnV;
    REG(fg_FIRE) = GFX_ENGINE_FIRE;

    GFX_CMDENQ(GREG_DST_BSAD);
    GFX_CMDENQ(GREG_S_OSD_WIDTH);
    GFX_CMDENQ(GREG_SRC_BSAD);
    GFX_CMDENQ(GREG_ADDR_MSB1);
    GFX_CMDENQ(GREG_SRC_SIZE);
    GFX_CMDENQ(GREG_STR_BLT_H);
    GFX_CMDENQ(GREG_STR_BLT_V);
    GFX_CMDENQ(GREG_STR_DST_SIZE);
    GFX_CMDENQ(GREG_ALCOM_LOOP);
    GFX_CMDENQ(GREG_G_MODE);

}

void _VGFX_Enable(void)
{
    REG(fg_OP_MODE) = (UINT32) OP_SCALER;
    REG(fg_FIRE) = GFX_ENGINE_FIRE;
    GFX_CMDENQ(GREG_G_MODE);

}

void _VGFX_GeneralAlphaComSet(UINT32 u4ImgCm, UINT32 u4WbCm, UINT32 u4Ar,
        UINT32 u4OpCode, BOOL fgAlcomNormal, UINT32 u4PremultSrcR,
        UINT32 u4PremultDstR, UINT32 u4PremultDstW, UINT32 u4PremultOvrflw)
{
    UINT32 u4CMDiff;

    if (u4WbCm == u4ImgCm)
        u4CMDiff = 0;
    else
        u4CMDiff = 1;

    REG(fg_ALCOM_NORMAL) = fgAlcomNormal;
    REG(fg_ALCOM_DIFF_CM) = u4CMDiff;
    REG(fg_ALCOM_AR) = u4Ar;
    REG(fg_ALCOM_OPCODE) = u4OpCode;
    REG(fg_PREMULT_SRC_R) = u4PremultSrcR;
    REG(fg_PREMULT_DST_R) = u4PremultDstR;
    REG(fg_PREMULT_DST_W) = u4PremultDstW;
    REG(fg_PREMULT_OVERFLOW_EN) = u4PremultOvrflw;

    GFX_CMDENQ(GREG_ALCOM_LOOP);
    // GFX_CMDENQ(GREG_G_MODE);   //only fire
}

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

