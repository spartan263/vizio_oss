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

#ifndef _VGFX_DRVIF_H_
#define _VGFX_DRVIF_H_
#include "gfx_common_if.h"

// #define  CC_VGFX_SCALER


extern void gaussian_elimination(double A[][8], double X[], double B[]);
extern void derive_inverse_transform(float src_x[], float src_y[],
        float dst_x[], float dst_y[], float inv_matrix[][3]);
extern unsigned int fnum_converter(float in);

extern void _VGFX_Init(void);

extern void _VGFX_SetWb(UINT32 u4BaseAddr, UINT32 u4ColrMode, UINT32 u4GlAr,
        UINT32 u4Pitch);
extern void _VGFX_SetTexture(UINT32 u4StartX, UINT32 u4StartY, UINT32 u4EndX,
        UINT32 u4EndY);

extern void _VGFX_SetBb(UINT32 u4StartX, UINT32 u4StartY, UINT32 u4Width,
        UINT32 u4Height);

extern void _VGFX_SetImgage(UINT32 u4BaseAddr, UINT32 u4ColrMode,
        UINT32 u4GlAr, UINT32 u4Pitch);

extern void _VGFX_SetMatrixCoef(VGFX_MATRIX_T* pt_matrix);

extern void _VGFX_SetAlcom(UINT32 u4AlcomEn);

extern void _VGFX_Scaler(VGFX_PARAM_SET_T *vgParam);

extern void _VGFX_SetQualityEnv(UINT32 u4WbExpMode, UINT32 u4ImgExpMode,
        UINT32 u4AntiAliasEn, UINT32 u4FilterEn, UINT32 u4AlMode);

extern void _VGFX_SetPostWrEnv(UINT32 u4PostWriteThreshold,
        UINT32 fgWdleEnable, UINT32 fgWBRndEn);

extern void _VGFX_SetWriteArgbOrder(UINT32 u4Aorder, UINT32 u4Rorder,
        UINT32 u4Gorder, UINT32 u4Border);

//extern void _VGFX_SetAlphaEdgeRGB0( UINT32 u4AlEdgeRGB0, UINT32 u4AlEdgeThresHold );
//extern void _VGFX_SetFilterType( UINT32 u4FilterType);

//extern void _VGFX_SetReflect( UINT32 u4RefEn, UINT32 u4RefOnly, UINT32 u4TextTureHeight, UINT32 u4RefRatioInit, UINT32 u4RatioLineStep, UINT32 u4RefAAEn);

//extern  void _VGFX_SetTextureArgbOrder( UINT32 u4Aorder, UINT32 u4Rorder, UINT32 u4Gorder, UINT32 u4Border);

extern BOOL _VGfx_JugRect(const VGFX_QUADRILATERAL_SET_T _Quadrilateral);
extern void _VGfx_SetVGfxScaler(const VGFX_SCALER_PARAM_SET_T *prCompBlt);
#endif

