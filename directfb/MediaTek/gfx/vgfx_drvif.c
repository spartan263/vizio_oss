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
#include "gfx_debug.h"
#include "vgfx_drvif.h"
#include "vgfx_dif_reg.h"
#include "vgfx_matrix.h"

#ifdef CC_FLIP_MIRROR_SUPPORT
#include "eeprom_if.h"
#include "drv_display.h"
#endif

// acronym of gfx register name in register file
#define VGREG(XREG)       _prVgfxRegFile->rField.XREG

// push back relative register value in register file.
#if defined(GFX_RISC_MODE)  // risc mode
#define VGFX_CMDENQ(XREG, INDEX)        \
    (VGFX_RiscPushBack((UINT32)(XREG), _au4VgfxRegFile[(UINT32)(INDEX)]))
#else                       // cmdque mode
#define VGFX_CMDENQ(XREG, INDEX)        \
    (VGFX_CmdQuePushBack((UINT32)(XREG), _au4VgfxRegFile[(UINT32)(INDEX)]))

#endif // #if defined(GFX_RISC_MODE)

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


static UINT32 _au4VgfxRegFile[VGREG_LAST_NUM];

/*lint -save -e826 */
static MI_DIF_VGFX_UNION_T *_prVgfxRegFile =
        (MI_DIF_VGFX_UNION_T *) _au4VgfxRegFile;
/*lint -restore */

//-------------------------------------------------------------------------
/** _GFX_SetColCnvSrc
 *  set color conversion source.
 *  luma input address, line size.
 *  chroma input address, linesize.
 *  field/frame mode.
 */
//-------------------------------------------------------------------------
#if 1
void _VGFX_Init(void)
{
    GFX_UNUSED_RET(memset(_au4VgfxRegFile, 0, sizeof(_au4VgfxRegFile)));
}

void _VGFX_SetWb(UINT32 u4BaseAddr, UINT32 u4ColrMode, UINT32 u4GlAr,
        UINT32 u4Pitch)
{
    VGREG(vfg_WB_BSAD) = (UINT32)((u4BaseAddr + 0x3f) & 0xFFFFFFC0);
    VGREG(vfg_WB_CM) = (UINT32) u4ColrMode;
    VGREG(vfg_WB_GLOBAL_ALPHA) = (UINT32) u4GlAr;
    VGREG(vfg_WB_PITCH) = (UINT32)((u4Pitch + 0x3f) & 0x1FC0);
    VGFX_CMDENQ(VGREG_WB_BSAD, VGREG_WB_BSAD_NUM);
    VGFX_CMDENQ(VGREG_WB_MODE, VGREG_WB_MODE_NUM);
    VGFX_CMDENQ(VGREG_WB_PITCH, VGREG_WB_PITCH_NUM);
}

void _VGFX_SetImgage(UINT32 u4BaseAddr, UINT32 u4ColrMode, UINT32 u4GlAr,
        UINT32 u4Pitch)
{
    VGREG(vfg_IMG_BSAD) = (UINT32)((u4BaseAddr + 0xf) & 0xFFFFFFF0);
    VGREG(vfg_IMG_CM) = (UINT32) u4ColrMode;
    VGREG(vfg_IMG_GL_ALPHA) = (UINT32) u4GlAr;
    VGREG(vfg_IMG_PITCH) = (UINT32) u4Pitch;
    VGFX_CMDENQ(VGREG_IMG_BSAD, VGREG_IMG_BSAD_NUM);
    VGFX_CMDENQ(VGREG_IMG_PITCH, VGREG_IMG_PITCH_NUM);
    VGFX_CMDENQ(VGREG_IMG_CONFIG, VGREG_IMG_CONFIG_NUM);
}

void _VGFX_SetTexture(UINT32 u4StartX, UINT32 u4StartY, UINT32 u4EndX,
        UINT32 u4EndY)
{
    VGREG(vfg_TXT_HSTART) = (UINT32) u4StartX;
    VGREG(vfg_TXT_VSTART) = (UINT32) u4StartY;
    VGREG(vfg_TXT_HEND) = (UINT32) u4EndX;
    VGREG(vfg_TXT_VEND) = (UINT32) u4EndY;
    VGFX_CMDENQ(VGREG_TXT_START, VGREG_TXT_START_NUM);
    VGFX_CMDENQ(VGREG_TXT_END, VGREG_TXT_END_NUM);
}

void _VGFX_SetBb(UINT32 u4StartX, UINT32 u4StartY, UINT32 u4Width,
        UINT32 u4Height)
{
    VGREG(vfg_BB_TL_X) = (UINT32) u4StartX;
    VGREG(vfg_BB_TL_Y) = (UINT32) u4StartY;
    VGREG(vfg_BB_WIDTH) = (UINT32) u4Width;
    VGREG(vfg_BB_HEIGHT) = (UINT32)(u4Height - 1);
    VGFX_CMDENQ(VGREG_BB_START, VGREG_BB_START_NUM);
    VGFX_CMDENQ(VGREG_BB_SIZE, VGREG_BB_SIZE_NUM);
}

void _VGFX_SetMatrixCoef(VGFX_MATRIX_T* pt_matrix)
{
    VGREG(vfg_COEF_A) = pt_matrix->u4_a;
    VGREG(vfg_COEF_B) = pt_matrix->u4_b;
    VGREG(vfg_COEF_C) = pt_matrix->u4_c;
    VGREG(vfg_COEF_D) = pt_matrix->u4_d;
    VGREG(vfg_COEF_E) = pt_matrix->u4_e;
    VGREG(vfg_COEF_F) = pt_matrix->u4_f;
    VGREG(vfg_COEF_G) = pt_matrix->u4_g;
    VGREG(vfg_COEF_H) = pt_matrix->u4_h;
    VGREG(vfg_COEF_I) = pt_matrix->u4_i;
    VGFX_CMDENQ(VGREG_COEF_A, VGREG_COEF_A_NUM);
    VGFX_CMDENQ(VGREG_COEF_B, VGREG_COEF_B_NUM);
    VGFX_CMDENQ(VGREG_COEF_C, VGREG_COEF_C_NUM);
    VGFX_CMDENQ(VGREG_COEF_D, VGREG_COEF_D_NUM);
    VGFX_CMDENQ(VGREG_COEF_E, VGREG_COEF_E_NUM);
    VGFX_CMDENQ(VGREG_COEF_F, VGREG_COEF_F_NUM);
    VGFX_CMDENQ(VGREG_COEF_G, VGREG_COEF_G_NUM);
    VGFX_CMDENQ(VGREG_COEF_H, VGREG_COEF_H_NUM);
    VGFX_CMDENQ(VGREG_COEF_I, VGREG_COEF_I_NUM);
}

void _VGFX_SetAlcom(UINT32 u4AlcomEn)
{
    if (u4AlcomEn)
        VGREG(vfg_ALU_EN) = 1;
    else
        VGREG(vfg_ALU_EN) = 0;
    VGREG(vfg_ALCOM_LOOP) = (UINT32) u4AlcomEn;
    VGFX_CMDENQ(VGREG_ALCOM_LOOP, VGREG_ALCOM_LOOP_NUM);
}

void _VGFX_SetQualityEnv(UINT32 u4WbExpMode, UINT32 u4ImgExpMode,
        UINT32 u4AntiAliasEn, UINT32 u4FilterEn, UINT32 u4AlMode)
{
    VGREG(vfg_WB_EXP) = (UINT32) u4WbExpMode;
    VGREG(vfg_IMG_EXP_MODE) = (UINT32) u4ImgExpMode;
    VGREG(vfg_ANTI_ALIAS_EN) = (UINT32) u4AntiAliasEn;
    VGREG(vfg_FILTER_EN) = (UINT32) u4FilterEn;
    VGREG(vfg_ALPHA_EDGE_MODE) = (UINT32) u4AlMode;
    VGFX_CMDENQ(VGREG_WB_EXP, VGREG_WB_EXP_NUM);
    VGFX_CMDENQ(VGREG_FILTER_CONFIG, VGREG_FILTER_CONFIG_NUM);
}

void _VGFX_SetPostWrEnv(UINT32 u4PostWriteThreshold, UINT32 fgWdleEnable,
        UINT32 fgWBRndEn)
{
    UINT32 u4PostDepth = 0x3f;

    VGREG(vfg_POST_THRS) = (UINT32) u4PostWriteThreshold;
    VGREG(vfg_POST_DEPTH) = (UINT32) u4PostDepth;
    VGREG(vfg_WDLE_EN) = (UINT32) fgWdleEnable;
    VGREG(vfg_WB_RND_EN) = (UINT32) fgWBRndEn;
    VGFX_CMDENQ(VGREG_PCTCTL_FIFO, VGREG_PCTCTL_FIFO_NUM);
    VGFX_CMDENQ(VGREG_PCTCTL_END, VGREG_PCTCTL_END_NUM);
    VGFX_CMDENQ(VGREG_WB_RND, VGREG_WB_RND_NUM);
}

void _VGFX_SetWriteArgbOrder(UINT32 u4Aorder, UINT32 u4Rorder, UINT32 u4Gorder,
        UINT32 u4Border)
{

    VGREG(vfg_B_SEL) = u4Border;
    VGREG(vfg_G_SEL) = u4Gorder;
    VGREG(vfg_R_SEL) = u4Rorder;
    VGREG(vfg_A_SEL) = u4Aorder;
    VGFX_CMDENQ(VGREG_PSTCTL_ARGB, VGREG_PSTCTL_ARGB_NUM);

}

UINT32 _VGFX_Color_Size(UINT32 u4_wb_clr_mode)
{
    UINT32 u4_clr_size = 0x0;

    switch (u4_wb_clr_mode)
    {
    case CM_RGB565_DIRECT16:
        u4_clr_size = 2;
        break;
    case CM_ARGB1555_DIRECT16:
        u4_clr_size = 2;
        break;
    case CM_ARGB4444_DIRECT16:
        u4_clr_size = 2;
        break;
    case CM_ARGB8888_DIRECT32:
        u4_clr_size = 4;
        break;
    default:
        return;
    }

    return u4_clr_size;
}

void _VGFX_Scaler(VGFX_PARAM_SET_T *pt_vgfx)
{
    _VGFX_SetImgage(pt_vgfx->u4ImgAddr, pt_vgfx->u4ImgClrMode,
            pt_vgfx->u4ImgGlobalAlpha, pt_vgfx->u4ImgPitch);

    _VGFX_SetWb(pt_vgfx->u4WBAddr, pt_vgfx->u4WBClrMode,
            pt_vgfx->u4WBGlobalAlpha, pt_vgfx->u4WBPitch);

    _VGFX_SetTexture(pt_vgfx->u4TextureHStart, pt_vgfx->u4TextureVStart,
            pt_vgfx->u4TextureHStart + pt_vgfx->u4TextureWidth - 1,
            pt_vgfx->u4TextureVStart + pt_vgfx->u4TextureHeight - 1);

    _VGFX_SetBb(pt_vgfx->u4BBXStart, pt_vgfx->u4BBYStart, pt_vgfx->u4BBWidth,
            pt_vgfx->u4BBHeight);
    _VGFX_SetQualityEnv(VG_CLR_EXP_REPEAT_MSB, VG_CLR_EXP_REPEAT_MSB,
            pt_vgfx->fgAntiAliasing, pt_vgfx->fgFilterEnable,
            pt_vgfx->u4AlphaEdgeMode);
    _VGFX_SetMatrixCoef(&pt_vgfx->t_matrix);
    _VGFX_SetAlcom(pt_vgfx->fgAlphaCompEn);
    if (pt_vgfx->fgAlphaCompEn)
    {
        _VGFX_GeneralAlphaComSet(pt_vgfx->u4ImgClrMode, pt_vgfx->u4WBClrMode,
                pt_vgfx->u4AlphaCompAr, pt_vgfx->u4AlphaCompOpCode,
                pt_vgfx->fgAlphaCompNormal, 0, 0, 0, 1);
    }

    _VGFX_Enable();
}

BOOL _VGfx_JugRect(const VGFX_QUADRILATERAL_SET_T _Quadrilateral)
{
    if ((_Quadrilateral.x1 == _Quadrilateral.x4) && (_Quadrilateral.x2
            == _Quadrilateral.x3) && (_Quadrilateral.y1 == _Quadrilateral.y2)
            && (_Quadrilateral.y3 == _Quadrilateral.y4) && (_Quadrilateral.x1
            < _Quadrilateral.x2) && (_Quadrilateral.y1 < _Quadrilateral.y4))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void _VGfx_SetVGfxScaler(const VGFX_SCALER_PARAM_SET_T *prCompBlt)
{
    /*  VGFX_TEXTURE_SET_T _SetTexture;
     VGFX_RECT_SET_T _TextRect;
     VGFX_PARAM_SET_T _SetVGFXSc_T;

     GFX_CHK_16B_ALIGN(prCompBlt->_IMAGType.u4BaseAddr);
     GFX_CHK_64B_ALIGN(prCompBlt->_WBType.u4BaseAddr);
     GFX_CHK_64B_PITCH(prCompBlt->_WBType.u4Pitch);
     GFX_ASSERT(prCompBlt->_BBType.u4StartX < prCompBlt->_WBType.u4WBWidth);
     GFX_ASSERT(prCompBlt->_BBType.u4StartY < prCompBlt->_WBType.u4WBHeight);

     if( _VGfx_JugRect(prCompBlt->_SrcQuard) )
     {
     return;
     }
     else
     {
     _SetTexture.u4StartX =prCompBlt->_SrcQuard.x1;
     _SetTexture.u4StartY =prCompBlt->_SrcQuard.y1;
     _SetTexture.u4EndX =prCompBlt->_SrcQuard.x3;
     _SetTexture.u4EndY =prCompBlt->_SrcQuard.y3;

     _TextRect.u4StartX = _SetTexture.u4StartX;
     _TextRect.u4StartY = _SetTexture.u4StartY;
     _TextRect.u4Width = _SetTexture.u4EndX - _SetTexture.u4StartX;
     _TextRect.u4Height = _SetTexture.u4EndY - _SetTexture.u4StartY;
     }

     if( prCompBlt->_SetAlphaComp.u4AlphaCompEn)
     {
     _SetVGFXSc_T.fgAlphaCompEn = 1;
     _SetVGFXSc_T.fgAlphaCompNormal = prCompBlt->_SetAlphaComp.fgAlphaCompNormal;
     _SetVGFXSc_T.u4AlphaCompAr = prCompBlt->_SetAlphaComp.u4AlphaCompAr;
     _SetVGFXSc_T.u4AlphaCompOpCode = prCompBlt->_SetAlphaComp.u4AlphaCompOpCode;
     _SetVGFXSc_T.u4AlphaEdgeMode = (ALPHA_EDGE_MD)prCompBlt->_SetAlphaComp._AlphaEdgeMd;
     _SetVGFXSc_T.u4PreDstRd = prCompBlt->_SetAlphaComp.u4PreDstRd;
     _SetVGFXSc_T.u4PreDstWt = prCompBlt->_SetAlphaComp.u4PreDstWt;
     _SetVGFXSc_T.u4PreSrcRd = prCompBlt->_SetAlphaComp.u4PreSrcRd;

     }
     else
     {
     _SetVGFXSc_T.fgAlphaCompEn = 0;
     _SetVGFXSc_T.fgAlphaCompNormal = prCompBlt->_SetAlphaComp.fgAlphaCompNormal;
     _SetVGFXSc_T.u4AlphaCompAr = prCompBlt->_SetAlphaComp.u4AlphaCompAr;
     _SetVGFXSc_T.u4AlphaCompOpCode = prCompBlt->_SetAlphaComp.u4AlphaCompOpCode;
     _SetVGFXSc_T.u4AlphaEdgeMode = (ALPHA_EDGE_MD)prCompBlt->_SetAlphaComp._AlphaEdgeMd;
     _SetVGFXSc_T.u4PreDstRd = prCompBlt->_SetAlphaComp.u4PreDstRd;
     _SetVGFXSc_T.u4PreDstWt = prCompBlt->_SetAlphaComp.u4PreDstWt;
     _SetVGFXSc_T.u4PreSrcRd = prCompBlt->_SetAlphaComp.u4PreSrcRd;

     }

     _SetVGFXSc_T.fgAntiAliasing = prCompBlt->u4AntiAliasing;
     _SetVGFXSc_T.fgFilterEnable = prCompBlt->_SetFilter.u4FilterEn;


     _SetVGFXSc_T.u4BBHeight = prCompBlt->_BBType.u4Height ;
     _SetVGFXSc_T.u4BBWidth = prCompBlt->_BBType.u4Width ;
     _SetVGFXSc_T.u4BBXStart = prCompBlt->_BBType.u4StartX ;
     _SetVGFXSc_T.u4BBYStart = prCompBlt->_BBType.u4StartY ;

     _SetVGFXSc_T.u4ImgAddr = prCompBlt->_IMAGType.u4BaseAddr ;
     _SetVGFXSc_T.u4ImgClrMode = prCompBlt->_IMAGType.u4ColrMode ;
     _SetVGFXSc_T.u4ImgGlobalAlpha = prCompBlt->_IMAGType.u4GlAr ;
     _SetVGFXSc_T.u4ImgPitch = prCompBlt->_IMAGType.u4Pitch;

     _SetVGFXSc_T.u4TextureHStart = _TextRect.u4StartX;
     _SetVGFXSc_T.u4TextureVStart = _TextRect.u4StartY;
     _SetVGFXSc_T.u4TextureWidth = _TextRect.u4Width;
     _SetVGFXSc_T.u4TextureHeight = _TextRect.u4Height;

     _SetVGFXSc_T.u4WBAddr = prCompBlt->_WBType.u4BaseAddr;
     _SetVGFXSc_T.u4WBClrMode = prCompBlt->_WBType.u4ColrMode;
     _SetVGFXSc_T.u4WBGlobalAlpha = prCompBlt->_WBType.u4GlAr;
     _SetVGFXSc_T.u4WBPitch = prCompBlt->_WBType.u4Pitch;
     _SetVGFXSc_T.u4WBHeight = prCompBlt->_WBType.u4WBHeight;
     _SetVGFXSc_T.u4WBWidth = prCompBlt->_WBType.u4WBWidth;

     _SetVGFXSc_T.t_matrix.u4_a = prCompBlt->_SetMartix.u4_a;
     _SetVGFXSc_T.t_matrix.u4_b = prCompBlt->_SetMartix.u4_b;
     _SetVGFXSc_T.t_matrix.u4_c = prCompBlt->_SetMartix.u4_c;
     _SetVGFXSc_T.t_matrix.u4_d = prCompBlt->_SetMartix.u4_d;
     _SetVGFXSc_T.t_matrix.u4_e = prCompBlt->_SetMartix.u4_e;
     _SetVGFXSc_T.t_matrix.u4_f = prCompBlt->_SetMartix.u4_f;
     _SetVGFXSc_T.t_matrix.u4_g = prCompBlt->_SetMartix.u4_g;
     _SetVGFXSc_T.t_matrix.u4_h = prCompBlt->_SetMartix.u4_h;
     _SetVGFXSc_T.t_matrix.u4_i = prCompBlt->_SetMartix.u4_i;

     if( prCompBlt->_SetReflect.u4RefEn)
     {
     _VGFX_SetReflect(TRUE,
     prCompBlt->_SetReflect.u4RefOnly,
     prCompBlt->_SetReflect.u4TextTureHeight,
     prCompBlt->_SetReflect.u4RefRatioInit,
     prCompBlt->_SetReflect.u4RatioLineStep,
     prCompBlt->_SetReflect.u4RefAAEn);
     }
     else
     {
     _VGFX_SetReflect(FALSE,
     prCompBlt->_SetReflect.u4RefOnly,
     prCompBlt->_SetReflect.u4TextTureHeight,
     prCompBlt->_SetReflect.u4RefRatioInit,
     prCompBlt->_SetReflect.u4RatioLineStep,
     prCompBlt->_SetReflect.u4RefAAEn);

     }

     _VGFX_Scaler(&_SetVGFXSc_T);*/
    return;
}

#endif
