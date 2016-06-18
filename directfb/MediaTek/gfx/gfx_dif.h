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
 * $RCSfile: gfx_dif.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_dif.h
 *  gfx driver data interface
 *  include common interface of hardware and software engine
 *  get/set register base address
 *  get/set idle state
 *  init modules
 */

#ifndef GFX_DIF_H
#define GFX_DIF_H

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "gfx_dif_reg.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

// u4Config
#define MA_DIF_HAVE_GFX_SW              \
    (GFX_DifGetData()->u4Config & GFX_HAVE_SW_MOD)

#define MA_DIF_HAVE_GFX_HW          \
    (GFX_DifGetData()->u4Config & GFX_HAVE_HW_MOD)

#define MA_DIF_HAVE_GFX_FB              \
    (GFX_DifGetData()->u4Config & GFX_HAVE_FB_MOD)

// u4ModInit
#define MA_DIF_GFX_SW_MOD_OK()          \
    (GFX_DifGetData()->u4ModInit |= GFX_HAVE_SW_MOD)

#define MA_DIF_GFX_HW_MOD_OK()      \
    (GFX_DifGetData()->u4ModInit |= GFX_HAVE_HW_MOD)

#define MA_DIF_GFX_FB_MOD_OK()          \
    (GFX_DifGetData()->u4ModInit |= GFX_HAVE_FB_MOD)

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

// gfx system capabilities
//  uiConfig - SW_MOD, HW_MOD, etc
//  uiModInit - init bit flag
typedef struct _MI_DIF_T
{
    UINT32 u4Config; // configuration bits, modules
    UINT32 u4GfxMode; // operation mode / module
    UINT32 u4ModInit; // configuration bits, modules status
    UINT32 *pu4CrBase; // control register base address
} MI_DIF_T;

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

extern void GFX_DifInit(void);

extern void GFX_DifSetRegBase(UINT32 *pu4Base);

extern void GFX_DifGetRegBase(UINT32 *pu4Base);

extern void GFX_DifReset(void);

extern void GFX_DifSetMode(UINT32 u4GfxMode);

extern void GFX_DifSetNotify(void(*pfnNotify)(UINT32));

extern MI_DIF_T *GFX_DifGetData(void);

extern void (*pfnGFX_DifAction)(void);

extern void (*pfnGFX_DifFinNotify)(UINT32);

#endif // GFX_DIF_H

