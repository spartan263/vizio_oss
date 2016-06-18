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
 * $RCSfile: gfx_dif.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_dif.c
 *  gfx driver data interface layer
 *  
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "gfx_if.h"
#include "gfx_drvif.h"
#include "gfx_dif.h"
#include "gfx_hw.h"
#include "gfx_sw.h"
#include "gfx_cmdque.h"
#include "gfx_debug.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------


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

static MI_DIF_T _rDifData =
{ (UINT32)(GFX_HAVE_SW_MOD + GFX_HAVE_HW_MOD), (UINT32) E_GFX_HW_MOD,
        (UINT32) 0, (UINT32 *) NULL };

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** pfnGFX_DifAction
 *  gfx command queue action function pointer
 */
//-------------------------------------------------------------------------
void (*pfnGFX_DifAction)(void);

//-------------------------------------------------------------------------
/** pfnGFX_DifFinNotify
 *  gfx complete notification function pointer
 */
//-------------------------------------------------------------------------
void (*pfnGFX_DifFinNotify)( UINT32);

//-------------------------------------------------------------------------
/** GFX_DifGetData
 *  get dif data (pointer)
 */
//-------------------------------------------------------------------------
MI_DIF_T *GFX_DifGetData(void)
{
    ASSERT_KERNEL();

    return &_rDifData;
}

//-------------------------------------------------------------------------
/** GFX_DifSetNotify
 *  set notification function
 */
//-------------------------------------------------------------------------
void GFX_DifSetNotify(void(*pfnNotify)( UINT32))
{
    ASSERT_KERNEL();

    pfnGFX_DifFinNotify = pfnNotify;
}

//-------------------------------------------------------------------------
/** GFX_DifInit
 *  gfx dif init
 *  init software module
 *  init hardware module
 *  set initial state (idle)
 */
//-------------------------------------------------------------------------
void GFX_DifInit(void)
{
    ASSERT_KERNEL();

#if defined(GFX_ENABLE_SW_MODE)
    //-----------------------------------------------------------
    GFX_SwInit();

    GFX_DifSetMode((UINT32)E_GFX_SW_MOD);

    MA_DIF_GFX_SW_MOD_OK();
    //-----------------------------------------------------------
#endif  //#if defined(GFX_ENABLE_SW_MODE)

    // setup ISR
    // reset hw and all counters
    // status check
    if (MA_DIF_HAVE_GFX_HW)
    {
        GFX_HwInit();

        MA_DIF_GFX_HW_MOD_OK();

        GFX_DifSetMode((UINT32) E_GFX_HW_MOD);
    }

    // fb module does not need initialization
    MA_DIF_GFX_FB_MOD_OK();
}

//-------------------------------------------------------------------------
/** GFX_DifSetRegBase
 *  gfx set dif register base address
 */
//-------------------------------------------------------------------------
void GFX_DifSetRegBase(UINT32 *pu4Base)
{
    ASSERT_KERNEL();

    GFX_ASSERT(pu4Base != NULL);

    _rDifData.pu4CrBase = pu4Base;
}

//-------------------------------------------------------------------------
/** GFX_DifGetRegBase
 *  gfx get dif register base address
 */
//-------------------------------------------------------------------------
void GFX_DifGetRegBase(UINT32 *pu4Base)
{
    ASSERT_KERNEL();

    GFX_ASSERT(pu4Base != NULL);

    *pu4Base = (UINT32)(_rDifData.pu4CrBase);
}

//-------------------------------------------------------------------------
/** GFX_DifReset
 *  gfx dif reset
 *  reset hardware module and software module
 */
//-------------------------------------------------------------------------
void GFX_DifReset(void)
{
    ASSERT_KERNEL();

    // if an HW gfx engine is available, just reset it
    if (MA_DIF_HAVE_GFX_HW)
    {
        GFX_HwReset();
    }
    else
    {
        // else erase software control register file
        GFX_UNUSED_RET(x_memset(_rDifData.pu4CrBase, 0,
                        (sizeof(UINT32) * GREG_FILE_SIZE)))
    }
}

//-------------------------------------------------------------------------
/** GFX_DifSetMode
 *  gfx set operation mode (hardware/software)
 *  change function pointers and re-init command queue
 */
//-------------------------------------------------------------------------
void GFX_DifSetMode(UINT32 u4GfxMode)
{
    UINT32 u4GfxRegBase;
    UINT32 *pu4GfxRegBase = &u4GfxRegBase;

    ASSERT_KERNEL();

    _rDifData.u4GfxMode = u4GfxMode;
    GFX_DBG_REC_ENG_EXE_MOD((INT32)u4GfxMode); // for debug use

    // hw mode setting
    if ((UINT32) E_GFX_HW_MOD == u4GfxMode)
    {
        pfnGFX_DifAction = GFX_HwAction;

        GFX_HwGetRegBase(&pu4GfxRegBase);

        GFX_DifSetRegBase(pu4GfxRegBase);
    }

#if defined(GFX_ENABLE_SW_MODE)
    // sw mode setting
    if ((UINT32)E_GFX_SW_MOD == u4GfxMode)
    {
        pfnGFX_DifAction = GFX_SwAction;

        GFX_SwGetRegBase(&pu4GfxRegBase);

        GFX_DifSetRegBase(pu4GfxRegBase);
    }

    // for emu-cli use
    GFX_CmdQueInit();
#endif  //#if defined(GFX_ENABLE_SW_MODE)
}

