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
 * $RCSfile: x_bim_ext.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file x_bim.h
 *  x_bim.h The BIM (Bus Interface Module) interface header file
 */


#ifndef X_BIM_H
#define X_BIM_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_hal_5381.h"
#include "x_typedef.h"


//===========================================================================
// Constant definitions

#define MAX_IRQ_VECTOR              31

//===========================================================================
// Macros for register read/write

#if defined(YES_BIMSWAP)
#define BIMSWAP(X) (((((X) & 0xf00)==0x400) || (((X) & 0x200))) ? (X) : (((X) & 0xffffffeb) | (((X) & 0x10) >> 2) | (((X) & 0x4) << 2)))
#else
#define BIMSWAP(X) (X)
#endif

#define BIM_READ8(offset)           IO_READ8(BIM_BASE, BIMSWAP(offset))
#define BIM_READ16(offset)          IO_READ16(BIM_BASE, BIMSWAP(offset))
#define BIM_READ32(offset)          IO_READ32(BIM_BASE, BIMSWAP(offset))

#define BIM_WRITE8(offset, value)   IO_WRITE8(BIM_BASE, BIMSWAP(offset), (value))
#define BIM_WRITE16(offset, value)  IO_WRITE16(BIM_BASE, BIMSWAP(offset), (value))
#define BIM_WRITE32(offset, value)  IO_WRITE32(BIM_BASE, BIMSWAP(offset), (value))

#define BIM_REG8(offset)            IO_REG8(BIM_BASE, BIMSWAP(offset))
#define BIM_REG16(offset)           IO_REG16(BIM_BASE, BIMSWAP(offset))
#define BIM_REG32(offset)           IO_REG32(BIM_BASE, BIMSWAP(offset))


//===========================================================================
// BIM register and bitmap defintions

//----------------------------------------------------------------------------
// PBI bus related registers
#define REG_RW_PBICFG       0x0028      // PBI Configuration Register

//----------------------------------------------------------------------------
// IRQ/FIQ registers
#define REG_IRQST           0x0030      // RISC IRQ Status Register
#define REG_RO_IRQST        0x0030      // RISC IRQ Status Register
#define REG_IRQEN           0x0034      // RISC IRQ Enable Register
#define REG_RW_IRQEN        0x0034      // RISC IRQ Enable Register
#define REG_IRQCL           0x0038      // RISC IRQ Clear Register
#define REG_RW_IRQCLR       0x0038      // RISC IRQ Clear Register
#define REG_FIQST           0x003c      // RISC IRQ Status Register
#define REG_RO_FIQST        0x003c      // RISC IRQ Status Register
#define REG_FIQEN           0x0040      // RISC IRQ Enable Register
#define REG_RW_FIQEN        0x0040      // RISC IRQ Enable Register
#define REG_FIQCL           0x0044      // RISC IRQ Clear Register
#define REG_RW_FIQCLR       0x0044      // RISC IRQ Clear Register
// Interrupt vectors
    #define VECTOR_PDWNC    0           // Power Down module Interrupt
    #define VECTOR_SERIAL   1           // Serial Interface Interrupt
    #define VECTOR_RTCAL    6           // RTC alarm
    #define VECTOR_IRTX     9           // IR Transmitter interrupt
    #define VECTOR_SPDIF    14          // SPDIF in Interrupt
    #define VECTOR_AUDIO    16          // Audio DSP Interrupt
    #define VECTOR_RS232    17          // RS232 Interrupt 1
    #define VECTOR_RS232_1  17          // RS232 Interrupt 1
    #define VECTOR_RS232_2  18          // RS232 Interrupt 2
    #define VECTOR_VDOIN    20          // Video In interrupt (8202 side)
    #define VECTOR_EXT_L2   23          // External interrupt with Level 2
    #define VECTOR_VLD      24          // VLD interrupt
    #define VECTOR_EXT      29          // External interrupt without Level 2
    #define _IRQ(v)         (1U << (v)) // IRQ bit by vector
    #define IRQ_PDWNC       _IRQ(VECTOR_PDWNC)
    #define IRQ_SERIAL      _IRQ(VECTOR_SERIAL)
    #define IRQ_RTCAL       _IRQ(VECTOR_RTCAL)
    #define IRQ_IRTX        _IRQ(VECTOR_IRTX)
    #define IRQ_SPDIF       _IRQ(VECTOR_SPDIF)
    #define IRQ_AUDIO       _IRQ(VECTOR_AUDIO)
    #define IRQ_RS232       _IRQ(VECTOR_RS232_1)
    #define IRQ_RS232_1     _IRQ(VECTOR_RS232_1)
    #define IRQ_RS232_2     _IRQ(VECTOR_RS232_2)
    #define IRQ_VDOIN       _IRQ(VECTOR_VDOIN)
    #define IRQ_EXT_L2      _IRQ(VECTOR_EXT_L2)
    #define IRQ_VLD         _IRQ(VECTOR_VLD)
    #define IRQ_EXT         _IRQ(VECTOR_EXT)

//----------------------------------------------------------------------------
// TCM DMA registers
#define REG_RW_PB0OT        0x0050      // PBI Bank 0 OE Timing Register
#define REG_RW_PB1OT        0x0054      // PBI Bank 1 OE Timing Register
#define REG_RW_PB2OT        0x0058      // PBI Bank 2 OE Timing Register
#define REG_WO_CLRBSY       0x005c      // Clear Busy Register
    #define CLR_CHK_P1H     (1U << 1)   // Clear check point 1 hit
    #define CLR_CHK_P2H     (1U << 2)   // Clear check point 2 hit
    #define CLR_ABT_CHK_P2H (1U << 3)   // Clear abort check point hit
    #define CLR_DINTR_CHK_P2H (1U << 4) // Clear dram interrupt heck point hit

#define REG_RW_DBG_MONSEL   0x0094      // Monitor Select Register

//----------------------------------------------------------------------------
// PBI Timing
#define REG_RW_PB0WT        0x0118      // PBI Bank 0 WE Timing Register
#define REG_RW_PB1WT        0x011c      // PBI Bank 1 WE Timing Register
#define REG_RW_PB2WT        0x0120      // PBI Bank 2 WE Timing Register

//----------------------------------------------------------------------------
// PWM ...
#define REG_RW_PWM0         0x0230      // PWM0 Register
#define REG_RW_PWM1         0x0234      // PWM1 Register
#define REG_RW_PWM2         0x0238      // PWM2 Register
    #define PWME            (1)             // enable
    #define PWMALD          (0x2)           // auto load PWMH from Demux
    #define PWMP_MASK       (0x0000ff00)    // prescaler
    #define PWMH_MASK       (0x00ff0000)    // high period
    #define PWMRSN_MASK     (0xff000000)    // resolution byte

//----------------------------------------------------------------------------
// IRTX read/write configuration registers
#define IRTX_CONFIG_REG             0x0298
    #define IRTX_CONF_BITNUM_MASK   (0x7f00U)
    #define IRTX_CONF_IRNIV         (1U << 7)
    #define IRTX_CONF_IROS          (1U << 6)
    #define IRTX_CONF_RORD          (1U << 5)
    #define IRTX_CONF_BORD          (1U << 4)
    #define IRTX_CONF_MODE_MASK     (0x000eU)
    #define IRTX_INIT_CONF_MASK     (0x00feU)
    #define IRTX_CONF_MODE_SWO_EN   (1U << 3)
    #define IRTX_CONF_MODE_PWC      (0U << 1)
    #define IRTX_CONF_MODE_RC5      (1U << 1)
    #define IRTX_CONF_MODE_RC6      (2U << 1)
    #define IRTX_CONF_MODE_SWO      (3U << 1)
    #define IRTX_CONF_START_SEND    (1U << 0)
#define IRTX_DATA_BYTE_3_0          0x029c      // byte 0 is least significant.
#define IRTX_DATA_BYTE_7_4          0x02a0
#define IRTX_DATA_BYTE_11_8         0x02a4
#define IRTX_SYNC_HIGH_REG          0x02a8
#define IRTX_SYNC_LOW_REG           0x02ac
#define IRTX_DATA0_HIGH_REG         0x02b0
#define IRTX_DATA0_LOW_REG          0x02b4
#define IRTX_DATA1_HIGH_REG         0x02b8
#define IRTX_DATA1_LOW_REG          0x02bc
#define IRTX_MODULATION_REG         0x02c0

//----------------------------------------------------------------------------
// Internal HDCP
#define REG_RW_HDCP_DEV     0x02d0      // Internal HDCP Device Address Register
#define REG_RW_HDCP_ADDR    0x02d4      // Internal HDCP SRAM Address Register
#define REG_RW_HDCP_DATA    0x02d8      // Internal HDCP SRAM Data Register

//----------------------------------------------------------------------------
// RTC
#define REG_RW_RTC_YEAR     0x0400      // RTC Year Register
#define REG_RW_RTC_MONTH    0x0404      // RTC Month Register
#define REG_RW_RTC_DAY      0x0408      // RTC Day Register
#define REG_RW_RTC_WEEK     0x040c      // RTC Week Register
#define REG_RW_RTC_HOUR     0x0410      // RTC Hour Register
#define REG_RW_RTC_MIN      0x0414      // RTC Minute Register
#define REG_RW_RTC_SEC      0x0418      // RTC Second Register
#define REG_RW_RTC_AYEAR    0x041c      // RTC Alarm Year Register
#define REG_RW_RTC_AMONTH   0x0420      // RTC Alarm Month Register
#define REG_RW_RTC_ADAY     0x0424      // RTC Alarm Day Register
#define REG_RW_RTC_AWEEK    0x0428      // RTC Alarm Week Register
#define REG_RW_RTC_AHOUR    0x042c      // RTC Alarm Hour Register
#define REG_RW_RTC_AMIN     0x0430      // RTC Alarm Minute Register
#define REG_RW_RTC_ASEC     0x0434      // RTC Alarm Second Register
#define REG_RW_RTC_ALMINT   0x0438      // RTC Alarm Interrupt Enable Register
    #define RTCALM_FULL_EN  (0xffU << 0)
    #define RTCALM_YEAR_EN  (1U << 7)   // Alarm compare YEAR enable
    #define RTCALM_MONTH_EN (1U << 6)   // Alarm compare MONTH enable
    #define RTCALM_DAY_EN   (1U << 5)   // Alarm compare DAY enable
    #define RTCALM_WEEK_EN  (1U << 4)   // Alarm compare WEEK enable
    #define RTCALM_HOUR_EN  (1U << 3)   // Alarm compare HOUR enable
    #define RTCALM_MINU_EN  (1U << 2)   // Alarm compare MINU enable
    #define RTCALM_SEC_EN   (1U << 1)   // Alarm compare SEC enable
    #define RTCALM_INTR_EN  (1U << 0)   // Alarm interrupt enable
#define REG_RW_RTC_PERINT   0x043c      // RTC Periodic Interrupt Enable Register
#define REG_RW_RTC_CTRL     0x0440      // RTC Control Register
    #define RTCTRL_STOP     (1U << 0)   // Stop RTC, to save power
    #define RTCTRL_H24      (1U << 1)   // 1 for 24-hour clock mode
    #define RTCTRL_SIM      (1U << 4)   // Pass write protection
    #define RTCTRL_PASS     (1U << 8)   // Pass write protection
#define REG_RW_RTC_KEY      0x0444      // RTC Key Register - NOT USED
#define REG_RW_RTC_LEARYEAR 0x0448      // RTC Leap Year    - NOT USED
#define REG_RW_RTC_PROT     0x044c      // RTC Write Protection Register
    #define RTC_WRITE_PROTECT_CODE          0x5371
#define REG_RW_RTC_PADCFG   0x0450      // RTC PAD Configuration Register
    #define RTCPAD_POWERDOWN    (1)     // Power down setting.
    #define RTCPAD_ENABLE       (0)     // Enable RTC Pad

//----------------------------------------------------------------------------
// CC TTX
#define REG_RW_TTX_CFG      0x0600      // CCTTX Configuration Register
	#define TTX_CFG_ODD_CORRECT	0x08	//Put 0x20 in decoded packet if incorrect odd parity 
	#define TTX_CFG_DEC_PKT	0x04	// Decode packet data trigger
	#define TTX_CFG_DEC_HEAD	0x02	// Decode packet data trigger
	
#define REG_RW_TTX_DEC0_CTL 0x0604      // CCTTX Decode Control 0 Register
#define REG_RW_TTX_DEC1_CTL 0x0608      // CCTTX Decode Control 1 Register
#define REG_RW_TTX_PKTDT0   0x060c      // CCTTX Packet Data 0 Register
#define REG_RW_TTX_PKTDT1   0x0610      // CCTTX Packet Data 1 Register
#define REG_RW_TTX_PKTDT2   0x0614      // CCTTX Packet Data 2 Register
#define REG_RW_TTX_PKTDT3   0x0618      // CCTTX Packet Data 3 Register
#define REG_RW_TTX_PKTDT4   0x061c      // CCTTX Packet Data 4 Register
#define REG_RW_TTX_PKTDT5   0x0620      // CCTTX Packet Data 5 Register
#define REG_RW_TTX_PKTDT6   0x0624      // CCTTX Packet Data 6 Register
#define REG_RW_TTX_PKTDT7   0x0628      // CCTTX Packet Data 7 Register
#define REG_RW_TTX_PKTDT8   0x062c      // CCTTX Packet Data 8 Register
#define REG_RW_TTX_PKTDT9   0x0630      // CCTTX Packet Data 9 Register
#define REG_RW_TTX_PKTDTa   0x0634      // CCTTX Packet Data a Register
#define REG_RW_TTX_ERR0     0x0638      // CCTTX Error Flag 0 Register
#define REG_RW_TTX_ERR1     0x063C      // CCTTX Error Flag 1 Register
	#define TTX_ERR1_INDICATION 0x400	// Is there any error in packet decoding
// To be continued...

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

EXTERN BOOL BIM_EnableIrq(UINT32 u4Vector);

EXTERN BOOL BIM_DisableIrq(UINT32 u4Vector);

EXTERN BOOL BIM_IsIrqEnabled(UINT32 u4Vector);

EXTERN BOOL BIM_IsIrqPending(UINT32 u4Vector);

EXTERN BOOL BIM_ClearIrq(UINT32 u4Vector);

EXTERN void BIM_Set7Seg(UINT32 u4Value);

EXTERN UINT32 BIM_GetOscillator(void);

EXTERN void BIM_SetCacheCfg(UINT32 u4Cfg);

EXTERN void BIM_Reboot(void);

EXTERN void BIM_SetTimeLog(UINT32 u4Slot);


#endif  // X_BIM_H

