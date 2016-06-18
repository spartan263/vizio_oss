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
 * $RCSfile: gfx_cmdque.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_cmdque.c
 *  Brief of file gfx_cmdque.c.
 *  Details of file gfx_cmdque.c (optional).
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "gfx_if.h"
#include "gfx_drvif.h"
#include "gfx_cmdque.h"
//#include "gfx_dif.h"
#include "gfx_hw.h"
#include "gfx_debug.h"

#include "mt53_fb.h"
#include <sys/ioctl.h>
#include <linux/errno.h>
#include <fcntl.h>

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

// que capacity
#define E_GFX_CQ_QCAPACITY      (_rGfxCmdQue->i4QueCapacity)

/// que align mask
//  gfx command que should align on dram word (128 bits = 16 bytes)
//#define E_GFX_CQ_ALIGN          4096 //16
// min of que capacity is 2 (mt5351 is 4)
#define GFX_CMQ_MIN_SIZE        2

#define GFX_CMD_MARGIN          2

#define GFX_ONE_CMD_SIZE        8   // one cmd = 8 bytes

#define GFX_ONE_CMD_MAX_SIZE    1024

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
static INT32 _i4GfxCmdqConfig = (INT32) E_GFX_CMDQ_CFG_256KB; //8K

static INT32 _i4GfxCmdqNewConfig = (INT32) E_GFX_CMDQ_CFG_256KB; //8K

/* Allocated in Kernel, mapped and set in User */
static volatile UINT64 *_pu8GfxCmdqueBuf = NULL;
static volatile GFX_CMDQUE_T *_rGfxCmdQue = NULL;

/*For Virtual cmd que*/
//static volatile UINT64       *_pu8GfxVirtualCmdBuf = NULL;
//static volatile GFX_CMDQUE_T *_rGfxVirtualCmdQue      = NULL;
static volatile UINT64 _u8GfxVirtualCmdBuf[256 * 1024];
static volatile GFX_CMDQUE_T _GfxVirtualCmdQue;

#if defined(GFX_DEBUG_MODE)
static GFX_DBG_ENUM2TEXT_T _rGfxDbgQueCapEnum2TextTbl[] =
{
    {   (INT32)E_GFX_CMDQ_CAP_2KB, "CMDQ_CFG_2KB"},
    {   (INT32)E_GFX_CMDQ_CAP_4KB, "CMDQ_CFG_4KB"},
    {   (INT32)E_GFX_CMDQ_CAP_8KB, "CMDQ_CFG_8KB"},
    {   (INT32)E_GFX_CMDQ_CAP_16KB, "CMDQ_CFG_16KB"},
    {   (INT32)E_GFX_CMDQ_CAP_32KB, "CMDQ_CFG_32KB"},
    {   (INT32)E_GFX_CMDQ_CAP_64KB, "CMDQ_CFG_64KB"},
    {   (INT32)E_GFX_CMDQ_CAP_128KB, "CMDQ_CFG_128KB"},
    {   (INT32)E_GFX_CMDQ_CAP_256KB, "CMDQ_CFG_256KB"},

    {   (INT32)E_GFX_CMDQ_CAP_MAX, NULL}
};
#endif // #if defined(GFX_DEBUG_MODE)

#if defined(GFX_ENABLE_SW_MODE)
static volatile MI_DIF_UNION_T *_prRegBase = NULL;
#endif  //#if defined(GFX_ENABLE_SW_MODE)

#if defined(GFX_RISC_MODE)  // risc mode   
static INT32 _i4GfxCmdqueBufExist = (INT32)TRUE;
#else                       // cmdque mode
static INT32 _i4GfxCmdqueBufExist = (INT32) FALSE;
#endif

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------


// Virtual cmd que functions
void GFX_VirtualCmdQueInit(void)
{
    _GfxVirtualCmdQue.i4QueSize = 0;
    //_rGfxVirtualCmdQue =&_GfxVirtualCmdQue;
    //_pu8GfxVirtualCmdBuf =_u8GfxVirtualCmdBuf;
    //_rGfxVirtualCmdQue->i4QueSize=0;
}

int GFX_GetVirtualCmdQueSize(void)
{
    return _GfxVirtualCmdQue.i4QueSize;
}

void GFX_VirtualCmdQueAction(void)
{
    UINT32 i;
    if ((_rGfxCmdQue->i4QueSize + GFX_CMD_MARGIN) >= _rGfxCmdQue->i4QueCapacity)
    {
        D_BUG("GFX_VirtualCmdQueAction() Error!!!\n");
    }

    for (i = 0; i < _GfxVirtualCmdQue.i4QueSize; i++)
    {
        _rGfxCmdQue->i4QueSize++;
        _pu8GfxCmdqueBuf[_rGfxCmdQue->i4WriteIndex++] = _u8GfxVirtualCmdBuf[i];

        if (_rGfxCmdQue->i4WriteIndex >= E_GFX_CQ_QCAPACITY)
        {
            _rGfxCmdQue->i4WriteIndex = 0;
        }
    }
    _GfxVirtualCmdQue.i4QueSize = 0;

    return;
}

//-------------------------------------------------------------------------
/** GFX_CmdQueInit
 *  init gfx command queue
 *    allocate cmdq buffer
 *    get hw register address (sw mod)
 *    set command queue info
 *    reset command queue hw
 */
//-------------------------------------------------------------------------
void GFX_CmdQueInit(volatile GFX_CMDQUE_T *rGfxCmdQue,
        volatile UINT64 **cmdQueBuf)
{
    D_INFO("MediaTek/Driver: %s( %p, %p )\n", __FUNCTION__, rGfxCmdQue,
            cmdQueBuf);

    /* Kernel AND User Function! */

    _rGfxCmdQue = rGfxCmdQue;

    if (_i4GfxCmdqConfig == _i4GfxCmdqNewConfig)
    {
        // allocate cmdque buffer by means of default config
        if (_i4GfxCmdqueBufExist == (INT32) FALSE)
        {
            _pu8GfxCmdqueBuf = *cmdQueBuf;
            VERIFY(_pu8GfxCmdqueBuf != NULL);

            _i4GfxCmdqueBufExist = (INT32) TRUE;
        }
    }
    else // re-allocate cmdque buffer
    {

        _pu8GfxCmdqueBuf = *cmdQueBuf;

        VERIFY(_pu8GfxCmdqueBuf != NULL);

        _i4GfxCmdqConfig = _i4GfxCmdqNewConfig;
    }

    GFX_VirtualCmdQueInit();
}

//-------------------------------------------------------------------------
/** GFX_CmdQuePushBack (Cmdque Mode)
 *  push back one register value into cmdque
 *  @param u4Reg indicates which register
 *  @param u4Val indicates the value of the register
 */
//-------------------------------------------------------------------------

void GFX_CmdQuePushBack(UINT32 u4Reg, UINT32 u4Val)
{
    UINT32 u4RegOffset = ((u4Reg << 2) & 0xfff);

    //make sure comd que size won't exceed limit
    if ((_GfxVirtualCmdQue.i4QueSize + GFX_CMD_MARGIN)
            >= _rGfxCmdQue->i4QueCapacity)
    {
        //printf("****VCMDQUE: PushFail!\n");
        ASSERT_USER();
    }

    // calculate GFX register address
    u4Reg = (GFX_ADDR) | (u4RegOffset);

    _u8GfxVirtualCmdBuf[_GfxVirtualCmdQue.i4QueSize++] = ((((UINT64) u4Reg)
            << 32) | u4Val);

    if (_GfxVirtualCmdQue.i4QueSize >= E_GFX_CQ_QCAPACITY)
    {
        ASSERT_USER();
    }

}

void VGFX_CmdQuePushBack(UINT32 u4Reg, UINT32 u4Val)
{
    UINT32 u4RegOffset;

    u4RegOffset = ((u4Reg << 2) & 0xfff);

    if ((_GfxVirtualCmdQue.i4QueSize + GFX_CMD_MARGIN)
            >= _rGfxCmdQue->i4QueCapacity)
    {
        //printf("****VCMDQUE: PushFail!\n");
        ASSERT_USER();
    }

    u4Reg = (VGFX_CMDQUE_ADDR) | (u4RegOffset);

    // write one cmd to cmdque buffer
    _u8GfxVirtualCmdBuf[_GfxVirtualCmdQue.i4QueSize++] = ((((UINT64) u4Reg)
            << 32) | u4Val);

    if (_GfxVirtualCmdQue.i4QueSize >= E_GFX_CQ_QCAPACITY)
    {
        ASSERT_USER();
    }
}


bool GFX_CmdQueIsTH(void)
{
    if ((_GfxVirtualCmdQue.i4QueSize + GFX_CMD_MARGIN + GFX_ONE_CMD_MAX_SIZE)
            >= _rGfxCmdQue->i4QueCapacity)
    {
        return true;
    }

    return false;
}

// for debug use
#if defined(GFX_DEBUG_MODE)
//-------------------------------------------------------------------------
/** GFX_CmdQueDbgInfo
 *  dump previously executed gfx command queue
 *  debug dump directly to console rs232 port
 *  start from previous index (in gfx command queue structure)
 *  till write index
 */
//-------------------------------------------------------------------------
void GFX_CmdQueDbgInfo(void)
{
    CHAR szQueCfgText[GFX_STR_SIZE];

    LOG(5, "\n--- gfx cmdque dump ---- (begin)\n");

    LOG(5, "   ShortQue    = %d (%s)\n", _rGfxCmdQue->i4ShortQue,
            (_rGfxCmdQue->i4ShortQue) ? "enabled" : "disabled");

    // get cmdq config text from cmdq capacity
    GFX_DbgEnum2Text(_rGfxCmdQue->i4QueCapacity, szQueCfgText,
            _rGfxDbgQueCapEnum2TextTbl, (INT32)E_GFX_CMDQ_CAP_MAX);
    LOG(5, "   QueConfig   = %d (%s)\n", _rGfxCmdQue->i4QueConfig, szQueCfgText);

    LOG(5, "   QueCapacity = %d\n", _rGfxCmdQue->i4QueCapacity);
    LOG(5, "   QueSize     = %d\n", _rGfxCmdQue->i4QueSize);
    LOG(5, "   PrevIndex   = %d\n", _rGfxCmdQue->i4PrevIndex);
    LOG(5, "   ReadIndex   = %d\n", _rGfxCmdQue->i4ReadIndex);
    LOG(5, "   WriteIndex  = %d\n", _rGfxCmdQue->i4WriteIndex);
    LOG(5, "   QueFlush    = %d\n", _rGfxCmdQue->i4QueFlushCount);
    LOG(5, "   QueIntrupt  = %d\n", _rGfxCmdQue->i4QueIntCount);

    _rGfxCmdQue->pu1PrevAddr = (UINT8 *)(((UINT32)_rGfxCmdQue->pu8QueTop) +
            ((UINT32)(_rGfxCmdQue->i4PrevIndex * GFX_ONE_CMD_SIZE)));
    _rGfxCmdQue->pu1ReadAddr = (UINT8 *)(((UINT32)_rGfxCmdQue->pu8QueTop) +
            ((UINT32)(_rGfxCmdQue->i4ReadIndex * GFX_ONE_CMD_SIZE)));
    _rGfxCmdQue->pu1WriteAddr = (UINT8 *)(((UINT32)_rGfxCmdQue->pu8QueTop) +
            ((UINT32)(_rGfxCmdQue->i4WriteIndex * GFX_ONE_CMD_SIZE)));

    LOG(5, "   PrevAddr    = 0x%08x\n", _rGfxCmdQue->pu1PrevAddr);
    LOG(5, "   ReadAddr    = 0x%08x\n", _rGfxCmdQue->pu1ReadAddr);
    LOG(5, "   WriteAddr   = 0x%08x\n", _rGfxCmdQue->pu1WriteAddr);

    if (_rGfxCmdQue->pu8QueTop == NULL)
    {
        LOG(5, "   QueTop      = 0x%08x (risc mode)\n", _rGfxCmdQue->pu8QueTop);
    }
    else
    {
        LOG(5, "   QueTop      = 0x%08x (cmdque mode)\n", _rGfxCmdQue->pu8QueTop);
    }

    LOG(5, "--- gfx cmdque dump ---- (end)\n\n");
}

//-------------------------------------------------------------------------
/** GFX_CmdQueSetNewConfig
 *  set cmdque new config
 *
 */
//-------------------------------------------------------------------------
void GFX_CmdQueSetNewConfig(INT32 i4NewConfig)
{
    _i4GfxCmdqNewConfig = i4NewConfig;
    {
        static volatile GFX_CMDQUE_T *_shm_cmd_que;
        static volatile UINT64 *_cmd_que_buf;
        GFX_Init( _shm_cmd_que, &_cmd_que_buf );
    }

}

//-------------------------------------------------------------------------
/** GFX_CmdQueFlushCount
 *  increase the cmdque flush count
 *
 */
//-------------------------------------------------------------------------
void GFX_CmdQueFlushCount(void)
{
    _rGfxCmdQue->i4QueFlushCount++;
}

//-------------------------------------------------------------------------
/** GFX_CmdQueIntCount
 *  increase the cmdque interrrupt count
 *
 */
//-------------------------------------------------------------------------
void GFX_CmdQueIntCount(void)
{
    _rGfxCmdQue->i4QueIntCount++;
}
#endif //#if defined(GFX_DEBUG_MODE)

