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
 * $RCSfile: gfx_debug.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_debug.c
 *  gfx driver debug interface
 *  
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "gfx_if.h"
#include "gfx_drvif.h"
//#include "gfx_dif.h"
#include "gfx_hw.h"
#include "gfx_cmdque.h"
#include "gfx_debug.h"

#if defined(GFX_DEBUG_MODE)
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

// gfx statistics for debug use
static GFX_DBG_INFO_T _rGfxDbgInfo;

static HAL_TIME_T _rGfxDbgFlushStartTime;

static GFX_DBG_ENUM2TEXT_T _rGfxDbgClkEnum2TextTbl[] =
{
    {   (INT32)E_GFX_CLK_XTAL_CK, "xtal_clk"},
    {   (INT32)E_GFX_CLK_SYSPLL_D2, "syspll / 2"},
    {   (INT32)E_GFX_CLK_TVDPLL_D3, "tvdpll / 3"},
    {   (INT32)E_GFX_CLK_TVDPLL_D5, "tvdpll / 5"},
    {   (INT32)E_GFX_CLK_DTDPLL_D3, "dtdpll / 3"},
    {   (INT32)E_GFX_CLK_DTDPLL_D4, "dtdpll / 4"},
    {   (INT32)E_GFX_CLK_PSPLL_D1, "pspll / 1"},
    {   (INT32)E_GFX_CLK_MEM, "mempll"},

    {   (INT32)E_GFX_CLK_MAX, NULL}
};

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
/** GFX_DbgInit
 *  
 */
//-------------------------------------------------------------------------
void GFX_DbgInit(void)
{
    _rGfxDbgInfo.i4DrvFlushCount = 0;
    _rGfxDbgInfo.i4DrvIntCount = 0;
    _rGfxDbgInfo.i4MwFlushCount = 0;
    _rGfxDbgInfo.i4MwIntCount = 0;
    _rGfxDbgInfo.i4CmFlushCount = 0;
    _rGfxDbgInfo.i4CmIntCount = 0;
}

//-------------------------------------------------------------------------
/** GFX_DbgIncDrvFlushCount
 *  increase driver flush count
 */
//-------------------------------------------------------------------------
void GFX_DbgIncDrvFlushCount(void)
{
    _rGfxDbgInfo.i4DrvFlushCount++;
}

//-------------------------------------------------------------------------
/** GFX_DbgIncDrvIntCount
 *  increase driver interrupt count
 */
//-------------------------------------------------------------------------
void GFX_DbgIncDrvIntCount(void)
{
    _rGfxDbgInfo.i4DrvIntCount++;
}

//-------------------------------------------------------------------------
/** GFX_DbgIncMwFlushCount
 *  increase middleware flush count
 */
//-------------------------------------------------------------------------
void GFX_DbgIncMwFlushCount(void)
{
    _rGfxDbgInfo.i4MwFlushCount++;
}

//-------------------------------------------------------------------------
/** GFX_DbgIncMwIntCount
 *  increase middleware interrupt count
 */
//-------------------------------------------------------------------------
void GFX_DbgIncMwIntCount(void)
{
    _rGfxDbgInfo.i4MwIntCount++;
}

//-------------------------------------------------------------------------
/** GFX_DbgIncCmFlushCount
 *  increase c-model flush count
 */
//-------------------------------------------------------------------------
void GFX_DbgIncCmFlushCount(void)
{
    _rGfxDbgInfo.i4CmFlushCount++;
}

//-------------------------------------------------------------------------
/** GFX_DbgIncCmIntCount
 *  increase c-model interrupt count
 */
//-------------------------------------------------------------------------
void GFX_DbgIncCmIntCount(void)
{
    _rGfxDbgInfo.i4CmIntCount++;
}

//-------------------------------------------------------------------------
/** GFX_DbgIncEngineCrashCount
 *  increase engine crash count
 */
//-------------------------------------------------------------------------
void GFX_DbgIncEngineCrashCount(void)
{
    _rGfxDbgInfo.i4EngineCrashCount++;
}

//-------------------------------------------------------------------------
/** GFX_DbgDumpInfo
 *  dump gfx debug information
 */
//-------------------------------------------------------------------------
void GFX_DbgDumpInfo(void)
{
    CHAR szClkSrcText[GFX_STR_SIZE];

    LOG(1, "\n--- gfx dbg report ---- (begin)\n");

    LOG(1, "   driver     flush     count = %d\n", _rGfxDbgInfo.i4DrvFlushCount);
    LOG(1, "   driver     interrupt count = %d\n", _rGfxDbgInfo.i4DrvIntCount);
    LOG(1, "   middleware flush     count = %d\n", _rGfxDbgInfo.i4MwFlushCount);
    LOG(1, "   middleware interrupt count = %d\n", _rGfxDbgInfo.i4MwIntCount);
    LOG(1, "   c-model    flush     count = %d\n", _rGfxDbgInfo.i4CmFlushCount);
    LOG(1, "   c-model    interrupt count = %d\n", _rGfxDbgInfo.i4CmIntCount);
    LOG(1, "   engine     crash     count = %d\n", _rGfxDbgInfo.i4EngineCrashCount);

    switch (_rGfxDbgInfo.i4DrvCurrentStatus)
    {
        case ((INT32)E_GFX_IDLE):
        LOG(1, "   driver     current  status = IDLE\n"); break;
        case ((INT32)E_GFX_BUSY):
        LOG(1, "   driver     current  status = BUSY\n"); break;
        case ((INT32)E_GFX_SEMIIDLE):
        LOG(1, "   driver     current  status = SEMIIDLE\n"); break;
        case ((INT32)E_GFX_CRASHED):
        LOG(1, "   driver     current  status = CRASHED\n"); break;
        case ((INT32)E_GFX_UNKNOWN):
        LOG(1, "   driver     current  status = UNKNOWN\n"); break;
        default:
        break;
    } // ~switch


    if (_rGfxDbgInfo.i4EngineExecuteMod == (INT32)E_GFX_SW_MOD)
    {
        LOG(1, "   engine     execute     mod = Software (C-model)\n");
    }
    else
    {
        LOG(1, "   engine     execute     mod = Hardware\n");
    }

    // get engine clock source text
    GFX_DbgEnum2Text(_rGfxDbgInfo.i4EngineClockSrc, szClkSrcText,
            _rGfxDbgClkEnum2TextTbl, (INT32)E_GFX_CLK_MAX);
    LOG(1, "   engine     clock    source = (%s)\n", szClkSrcText);

    LOG(1, "   gfx engine fire count = %u\n",
            (GFX_READ32(GFX_REG_0x40E0) & 0x0000ffff));
    LOG(1, "   gfx engine cmdq index = %u\n",
            ((GFX_READ32(GFX_REG_0x40E4) & 0x3fff0000) >> 16));
    LOG(1, "   gfx engine read  count of dram word = %u\n", GFX_READ32(GFX_REG_0x40EC));
    LOG(1, "   gfx engine write count of dram word = %u\n", GFX_READ32(GFX_REG_0x40F0));

    LOG(1, "--- gfx dbg report ---- (end)\n\n");
}

//-------------------------------------------------------------------------
/** GFX_DbgRecCurrentStatus
 *  record driver code current status
 */
//-------------------------------------------------------------------------
void GFX_DbgRecCurrentStatus(INT32 i4Status)
{
    _rGfxDbgInfo.i4DrvCurrentStatus = i4Status;
}

//-------------------------------------------------------------------------
/** GFX_DbgRecoverEngine
 *  Recover gfx engine when gfx engine crashed
 */
//-------------------------------------------------------------------------
void GFX_DbgRecoverEngine(void)
{
    GFX_DbgIncEngineCrashCount();
    GFX_DbgRecCurrentStatus((INT32)E_GFX_CRASHED);

    GFX_DbgShowBusyTime();

#if !defined(GFX_RISC_MODE) // cmdque mode
    GFX_HwISR();
#endif

    GFX_Init();

    // unlock cmdque resource
    GFX_UnlockCmdque();
}

//-------------------------------------------------------------------------
/** GFX_DbgRecFlushTime
 *  record flush start time
 */
//-------------------------------------------------------------------------
void GFX_DbgRecFlushTime(void)
{
    HAL_GetTime(&_rGfxDbgFlushStartTime);
}

//-------------------------------------------------------------------------
/** GFX_DbgShowBusyTime
 *  show busy working time
 */
//-------------------------------------------------------------------------
void GFX_DbgShowBusyTime(void)
{
    HAL_TIME_T rCurrentTime;
    HAL_TIME_T rDiffTime;

    HAL_GetTime(&rCurrentTime);
    HAL_GetDeltaTime(&rDiffTime, &_rGfxDbgFlushStartTime, &rCurrentTime);

    LOG(1, "GFX engine busy working time = %u sec\n", rDiffTime.u4Seconds);
    LOG(1, "-- flush   time = %u sec\n", _rGfxDbgFlushStartTime.u4Seconds);
    LOG(1, "-- current time = %u sec\n", rCurrentTime.u4Seconds);
}

//-------------------------------------------------------------------------
/** GFX_DbgRecEngExecuteMod
 *  record engine mode, HW-engine or SW-cmodel
 */
//-------------------------------------------------------------------------
void GFX_DbgRecEngExecuteMod(INT32 i4Mod)
{
    _rGfxDbgInfo.i4EngineExecuteMod = i4Mod;
}

//-------------------------------------------------------------------------
/** GFX_DbgEnum2Text
 *  translate enum into text
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e818 -e830 -e831  */
void GFX_DbgEnum2Text(INT32 i4Enum, CHAR *szText,
        GFX_DBG_ENUM2TEXT_T *prEnum2Text, INT32 i4Stop)
{
    GFX_ASSERT(prEnum2Text != NULL);
    GFX_ASSERT(szText != NULL);

    while (1)
    {
        if (prEnum2Text->i4Value == i4Stop)
        {
            LOG(0, "Error: Enum can NOT be translated into Text\n");
            GFX_ASSERT(prEnum2Text->i4Value != i4Stop);
        }

        if (i4Enum == prEnum2Text->i4Value)
        {
            break;
        }
        else
        {
            prEnum2Text++;
        }
    }

    GFX_UNUSED_RET(x_strcpy(szText, prEnum2Text->szText));
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_DbgRecEngClkSrc
 *  record engine clock source
 */
//-------------------------------------------------------------------------
void GFX_DbgRecEngClkSrc(INT32 i4ClkSrc)
{
    _rGfxDbgInfo.i4EngineClockSrc = i4ClkSrc;
}

//-------------------------------------------------------------------------
/** GFX_DbgCheckEngIdle
 *  check engine idle
 */
//-------------------------------------------------------------------------
void GFX_DbgCheckEngIdle(void)
{
    GFX_ASSERT(GFX_HwGetIdle());
    GFX_ASSERT(_rGfxDbgInfo.i4DrvFlushCount == _rGfxDbgInfo.i4DrvIntCount);
}
#endif // #if defined(GFX_DEBUG_MODE)

