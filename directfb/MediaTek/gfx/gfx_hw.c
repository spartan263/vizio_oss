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
 * $RCSfile: gfx_hw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_hw.c
 *  gfx hardware interfaces
 *  purely hardware related functions, include crash detection routines,
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "gfx_if.h"
#include "gfx_drvif.h"
#include "gfx_hw.h"
#include "gfx_cmdque.h"
#include "gfx_dif.h"
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

#ifdef __KERNEL__
static INT32 _i4GfxSetupISR = GFX_DISABLE;
#endif

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** _GfxHwRealIsr
 *  gfx interrupt service routine
 *  clear irq, calculate time, trigger hw_isr interface
 */
//-------------------------------------------------------------------------
#ifdef __KERNEL__
static void _GfxHwRealIsr(UINT16 u2Vector)
{
    ASSERT_KERNEL();

    UNUSED(u2Vector); // for lint happy

    if (BIM_ClearIrq((UINT32)VECTOR_GFX))
    {
        GFX_DBG_REC_CUR_STATUS((INT32)E_GFX_IDLE);

#if !defined(GFX_RISC_MODE)     // cmdque mode
        GFX_HwISR();
#endif

        // unlock cmdque resource
        GFX_UnlockCmdque();

        GFX_DBG_INC_DRV_INT_COUNT(); // for debug use
        GFX_CMDQUE_INC_INT_COUNT(); // for debug use
    }
}
#endif

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** GFX_HwInit
 *  init gfx hardware
 *  set control register address to default
 *  set default isr
 *  do hw reset
 */
//-------------------------------------------------------------------------
void GFX_HwInit(void)
{
#ifdef __KERNEL__
    x_os_isr_fct pfnOldIsr;
#endif

    ASSERT_KERNEL();

#ifdef __KERNEL__
    // setup ISR
    if (_i4GfxSetupISR == GFX_DISABLE)
    {
        VERIFY(x_reg_isr((UINT16)VECTOR_GFX, _GfxHwRealIsr, &pfnOldIsr) == OSR_OK);
        _i4GfxSetupISR = GFX_ENABLE;
    }
#endif
}

//-------------------------------------------------------------------------
/** GFX_HwGetRegBase
 *  get gfx hw control register base
 */
//-------------------------------------------------------------------------
void GFX_HwGetRegBase(UINT32 **ppu4RegBase)
{
    ASSERT_KERNEL();

    GFX_ASSERT(ppu4RegBase != NULL);
    GFX_ASSERT(*ppu4RegBase != NULL);

    *ppu4RegBase = (UINT32 *) (GFX_ADDR);
}

//-------------------------------------------------------------------------
/** GFX_HwISR
 *  gfx hw isr
 *  reset crash detection vars
 *  set engine to idle
 *  notify if notification was set
 */
//-------------------------------------------------------------------------
void GFX_HwISR(void)
{
    ASSERT_KERNEL();

    // call call-back function
    if (pfnGFX_DifFinNotify != NULL)
    {
        if (_GFX_IsFlushing()) // if MW uses flush (GFX_MwFlush)
        {
            _GFX_SetFlushStatus(FALSE);

            // notify to middleware
            pfnGFX_DifFinNotify((UINT32) E_GFX_OK);

            GFX_DBG_INC_MW_INT_COUNT(); // for debug use
        }
    }
}

//-------------------------------------------------------------------------
/** GFX_HwReset
 *  hw reset
 *  trigger hw reset
 *  enter any non-zero gfx mode (start clock)
 *  enable interrupt
 *  set register to default
 */
//-------------------------------------------------------------------------
void GFX_HwReset(void)
{
    volatile UINT32 u4Value;
    UINT32 u4ResHi = (UINT32) 0x3;
    UINT32 u4ResLo = (UINT32) 0x0;

    ASSERT_KERNEL();

#ifdef CC_LOW_POWER
    // do GFX module clock selection (162 MHz)
    GFX_HwSetEngClkSrc((INT32)E_GFX_CLK_SYSPLL_D2);
#else
    // do GFX module clock selection (200 MHz)
    GFX_HwSetEngClkSrc((INT32) E_GFX_CLK_DTDPLL_D3);
#endif

    // reset gfx config register
    GFX_WRITE32(GFX_REG_G_CONFIG, 0xA0000000);

    // enable gfx engine clock (set non-zero value)
    GFX_WRITE32(GFX_REG_G_MODE, 0x000001F0); // fg_OP_MODE = 0x1F;

    // reset engine and cmdque :
    // 1. set fg_G_RST to 0x3 and fg_CQ_RST to 0x3
    // 2. set fg_G_RST to 0x0 and fg_CQ_RST to 0x0
    u4Value = GFX_READ32(GFX_REG_G_CONFIG);

    u4Value = (u4Value & (~(GREG_MSK_G_RST | GREG_MSK_CQ_RST)));
    u4Value = (u4Value) | (u4ResHi << GREG_SHT_G_RST) | (u4ResHi
            << GREG_SHT_CQ_RST);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);

    u4Value = (u4Value & (~(GREG_MSK_G_RST | GREG_MSK_CQ_RST)));
    u4Value = (u4Value) | (u4ResLo << GREG_SHT_G_RST) | (u4ResLo
            << GREG_SHT_CQ_RST);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);

    // wait gfx hw reset ready
    for (;;)
    {
        u4Value = GFX_READ32(GFX_REG_G_CONFIG);

        if (((u4Value & GREG_MSK_G_RST) == GFX_G_RST_READY) && ((u4Value
                & GREG_MSK_CQ_RST) == GFX_CQ_RST_READY))
        {
            break;
        }
    }

    // set SDFIFO threshold
    u4Value = (u4Value) & (~(GREG_MSK_SDFIFO_THRS));
    u4Value = (u4Value) | (0x1 << GREG_SHT_SDFIFO_THRS);

    // set CMDFIFO threshold
    u4Value = (u4Value) & (~(GREG_MSK_CMDFIFO_THRS));
    u4Value = (u4Value) | (0x1 << GREG_SHT_CMDFIFO_THRS);

    // set POST threshold
    u4Value = (u4Value) & (~(GREG_MSK_POST_THRS));
    u4Value = (u4Value) | (0x3 << GREG_SHT_POST_THRS);

    // enable gfx interrupt : set fg_INT_MASK to 0x1
    u4Value = (u4Value | GREG_MSK_INT_MASK);

    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);
}

//-------------------------------------------------------------------------
/** GFX_HwGetIdle
 *  get gfx hardware idle status
 */
//-------------------------------------------------------------------------
INT32 GFX_HwGetIdle(void)
{
    volatile UINT32 u4Value;

    ASSERT_KERNEL();

    u4Value = GFX_READ32(GFX_REG_G_STATUS);
    u4Value = ((u4Value & GREG_MSK_IDLE) >> GREG_SHT_IDLE);

    return (INT32)(u4Value);
}

//-------------------------------------------------------------------------
/** GFX_HwAction
 *  start gfx hardware engine
 *  used in interrupt mode only
 *  in command queue mode, use command queue action
 */
//-------------------------------------------------------------------------
void GFX_HwAction(void)
{
    ASSERT_KERNEL();

    return;
}

//-------------------------------------------------------------------------
/** GFX_HwSetRiscMode
 *  disable gfx command que mode
 */
//-------------------------------------------------------------------------
void GFX_HwSetRiscMode(void)
{
    volatile UINT32 u4Value;

    ASSERT_KERNEL();

    u4Value = GFX_READ32(GFX_REG_G_CONFIG);
    u4Value = (u4Value) & (~GREG_MSK_EN_DRAMQ);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);
}

//-------------------------------------------------------------------------
/** GFX_HwSetCmdQMode
 *  re-enable gfx command que mode
 */
//-------------------------------------------------------------------------
void GFX_HwSetCmdQMode(void)
{
    volatile UINT32 u4Value;

    ASSERT_KERNEL();

    u4Value = GFX_READ32(GFX_REG_G_CONFIG);
    u4Value = (u4Value) | (GREG_MSK_EN_DRAMQ);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);
}

//-------------------------------------------------------------------------
/** GFX_HwSetEngClkSrc
 *  set engine clock source
 */
//-------------------------------------------------------------------------
void GFX_HwSetEngClkSrc(INT32 i4ClkSrc)
{
    ASSERT_KERNEL();

#ifdef __KERNEL__
    // GRA_CK : 0x2000D21C (mt537x)
    CLKSRC_ChangeGFXClock(i4ClkSrc);

    GFX_DBG_REC_ENG_CLK_SRC(i4ClkSrc);
#endif
}

//-------------------------------------------------------------------------
/** GFX_HwGetMemCompareResult
 *  get memory compare result from hw register
 *  if no error, return 0
 *  else         return 1
 */
//-------------------------------------------------------------------------
INT32 GFX_HwGetMemCompareResult(void)
{
    volatile UINT32 u4Value;

    ASSERT_KERNEL();

    u4Value = GFX_READ32(GFX_REG_ROP);
    u4Value = (u4Value & GREG_MSK_CMP_FLAG) >> GREG_SHT_CMP_FLAG;

    return (INT32) u4Value;
}

// for debug use
#if defined(GFX_DEBUG_MODE)
//-------------------------------------------------------------------------
/** GFX_HwResetEngine
 *  gfx hw reset engine only (for debug use)
 */
//-------------------------------------------------------------------------
void GFX_HwResetEngine(void)
{
    volatile UINT32 u4Value;
    UINT32 u4ResHi = (UINT32)0x3;
    UINT32 u4ResLo = (UINT32)0x0;

    ASSERT_KERNEL();

    // reset engine :
    // 1. set fg_G_RST to 0x3
    // 2. set fg_G_RST to 0x0
    u4Value = GFX_READ32(GFX_REG_G_CONFIG);

    u4Value = (u4Value) & (~(GREG_MSK_G_RST));
    u4Value = (u4Value) | (u4ResHi << GREG_SHT_G_RST);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);

    u4Value = (u4Value) & (~(GREG_MSK_G_RST));
    u4Value = (u4Value) | (u4ResLo << GREG_SHT_G_RST);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);

    // wait gfx hw reset ready
    for (;; )
    {
        u4Value = GFX_READ32(GFX_REG_G_CONFIG);

        if ((u4Value & GREG_MSK_G_RST) == GFX_G_RST_READY)
        {
            break;
        }
    }
}

//-------------------------------------------------------------------------
/** GFX_HwResetCmdQue
 *  gfx hw reset cmd queue only (for debug use)
 */
//-------------------------------------------------------------------------
void GFX_HwResetCmdQue(void)
{
    volatile UINT32 u4Value;
    UINT32 u4ResHi = (UINT32)0x3;
    UINT32 u4ResLo = (UINT32)0x0;

    ASSERT_KERNEL();

    // reset cmdque :
    // 1. set fg_CQ_RST to 0x3
    // 2. set fg_CQ_RST to 0x0
    u4Value = GFX_READ32(GFX_REG_G_CONFIG);

    u4Value = (u4Value) & (~(GREG_MSK_CQ_RST));
    u4Value = (u4Value) | (u4ResHi << GREG_SHT_CQ_RST);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);

    u4Value = (u4Value) & (~(GREG_MSK_CQ_RST));
    u4Value = (u4Value) | (u4ResLo << GREG_SHT_CQ_RST);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);

    // wait gfx hw reset ready
    for (;; )
    {
        u4Value = GFX_READ32(GFX_REG_G_CONFIG);

        if ((u4Value & GREG_MSK_CQ_RST) == GFX_CQ_RST_READY)
        {
            break;
        }
    }
}

//-------------------------------------------------------------------------
/** GFX_HwGetHwVersion
 *  get gfx hw version (control register)
 */
//-------------------------------------------------------------------------
void GFX_HwGetHwVersion(UINT32 *pu4HwVersion)
{
    volatile UINT32 u4Value;

    ASSERT_KERNEL();

    GFX_ASSERT(pu4HwVersion != NULL);

    u4Value = GFX_READ32(GFX_REG_G_STATUS);
    u4Value = ((u4Value & GREG_MSK_VERSION_ID) >> GREG_SHT_VERSION_ID);

    *pu4HwVersion = u4Value;
}

//-------------------------------------------------------------------------
/** GFX_HwEnableLowPowerMode
 *  enable gfx engine advanced low power mode
 *  legacy, left blanc here for future use
 */
//-------------------------------------------------------------------------
void GFX_HwEnableLowPowerMode(void)
{
    volatile UINT32 u4Value;

    ASSERT_KERNEL();

    u4Value = GFX_READ32(GFX_REG_G_CONFIG);
    u4Value = (u4Value) | (GREG_MSK_ENG_LP);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);
}

//-------------------------------------------------------------------------
/** GFX_HwDisableLowPowerMode
 *  disable gfx engine advanced low power mode
 *  legacy, left blanc here for future use
 */
//-------------------------------------------------------------------------
void GFX_HwDisableLowPowerMode(void)
{
    volatile UINT32 u4Value;

    ASSERT_KERNEL();

    u4Value = GFX_READ32(GFX_REG_G_CONFIG);
    u4Value = (u4Value) & (~GREG_MSK_ENG_LP);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);
}

//-------------------------------------------------------------------------
/** GFX_HwSetSDFifoThreshold
 *  set source/destination fifo request threshold
 */
//-------------------------------------------------------------------------
void GFX_HwSetSDFifoThreshold(UINT32 u4Value)
{
    volatile UINT32 u4RegVal;

    ASSERT_KERNEL();

    u4RegVal = GFX_READ32(GFX_REG_G_CONFIG);
    u4RegVal = (u4RegVal) & (~GREG_MSK_SDFIFO_THRS);
    u4RegVal = (u4RegVal) | (u4Value << GREG_SHT_SDFIFO_THRS);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4RegVal);
}

//-------------------------------------------------------------------------
/** GFX_HwSetCMDFifoThreshold
 *  set command fifo request threshold
 */
//-------------------------------------------------------------------------
void GFX_HwSetCMDFifoThreshold(UINT32 u4Value)
{
    volatile UINT32 u4RegVal;

    ASSERT_KERNEL();

    u4RegVal = GFX_READ32(GFX_REG_G_CONFIG);
    u4RegVal = (u4RegVal) & (~GREG_MSK_CMDFIFO_THRS);
    u4RegVal = (u4RegVal) | (u4Value << GREG_SHT_CMDFIFO_THRS);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4RegVal);
}

//-------------------------------------------------------------------------
/** GFX_HwSetPOSTFifoThreshold
 *  set post-write fifo request threshold
 */
//-------------------------------------------------------------------------
void GFX_HwSetPOSTFifoThreshold(UINT32 u4Value)
{
    volatile UINT32 u4RegVal;

    ASSERT_KERNEL();

    u4RegVal = GFX_READ32(GFX_REG_G_CONFIG);
    u4RegVal = (u4RegVal) & (~GREG_MSK_POST_THRS);
    u4RegVal = (u4RegVal) | (u4Value << GREG_SHT_POST_THRS);
    GFX_WRITE32(GFX_REG_G_CONFIG, u4RegVal);
}

//-------------------------------------------------------------------------
/** GFX_HwSetReqInterval
 *  set REQ_INTERVAL
 *  only impact rectangle fill.
 */
//-------------------------------------------------------------------------
void GFX_HwSetReqInterval(UINT32 u4ReqInterval)
{
    volatile UINT32 u4Value;

    ASSERT_KERNEL();

    u4Value = GFX_READ32(GFX_REG_G_CONFIG);

    // set request interval
    u4Value = (u4Value) & (~(GREG_MSK_REQ_INTVAL));
    u4Value = (u4Value) | (u4ReqInterval << GREG_SHT_REQ_INTVAL);

    GFX_WRITE32(GFX_REG_G_CONFIG, u4Value);
}
#endif // #if defined(GFX_DEBUG_MODE)

