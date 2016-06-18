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
 * $RCSfile: x_bim.h,v $
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

#define MAX_IRQ_VECTOR              39  // not 31, with MISC vector.

//===========================================================================
// Macros for register read/write

#if defined(YES_BIMSWAP)
#define BIMSWAP(X) ((((X) & 0xf00) > 0x200) ? (X) : (((X) & 0xffffffeb) | (((X) & 0x10) >> 2) | (((X) & 0x4) << 2)))
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
#define REG_RO_ICE          0x0000      // RISC ICE Register
    #define ICE_MODE_STATUS (1U << 0)   // ICE mode status, 0: disable, 1: enable
#define REG_RW_REMAP        0x001c      // Remap Register
    #define REMAP_ENABLE    (1)
#define REG_RW_TIMEOUT_CTRL 0x0020      // Timeout Control Register
    #define TO_FLASH_CNT    (0xffU << 24)   // Flash timeout count
    #define TO_DRAM_CNT     (0xffU << 16)   // Dram timeout count
    #define TO_IOBUS_CNT    (0xffU << 8)    // IOBus timeout count
    #define TO_FLASH_EN     (1U << 2)       // Flash timeout enable
    #define TO_DRAM_EN      (1U << 1)       // Dram timeout enable
    #define TO_IOBUS_EN     (1U << 0)       // IOBus timeout enable

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
    #define VECTOR_NAND     2           // NAND-Flash interface interrupt
    #define VECTOR_T0       3           // Timer 0
    #define VECTOR_T1       4           // Timer 1
    #define VECTOR_T2       5           // Timer 2
    #define VECTOR_EXENET   6           // External Ethernet interrupt bit
    #define VECTOR_LZHS     7           // LZHS instruction decompression engine
    #define VECTOR_ENET     8           // Ethernet interrupt
    #define VECTOR_DTCP     9           // DCTP interrupt enable bit
    #define VECTOR_PSCAN    10          // PSCAN interrupt
    #define VECTOR_SMARTCARD 11         // Smart Card Interrupt  
    #define VECTOR_IMGRZ    12          // IMGRZ interrupt
    #define VECTOR_GFXSC    12          // IMGRZ interrupt
    #define VECTOR_GDMA     13          // GDMA interrupt
	#define VECTOR_SPDIF    14          // Audio PSR/SPDIF interrupt
	#define VECTOR_PSR      14          // Audio PSR/SPDIF interrupt
    #define VECTOR_USB      15          // USB0 Interrupt
    #define VECTOR_USB0     15          // USB0 Interrupt
	#define VECTOR_AUDIO    16          // Audio DSP interrupt
    #define VECTOR_DSP      16          // Audio DSP Interrupt
    #define VECTOR_RS232    17          // RS232 Interrupt 1
    #define VECTOR_PVR      18          // PVR interrupt
    #define VECTOR_POD      19          // POD interrupt
    #define VECTOR_PCMCIA   19          // PCMCIA interrupt
    #define VECTOR_VDOIN    20          // Video In interrupt (8202 side)
    #define VECTOR_BLK2RS   21          // Block to Raster Interrupt
    #define VECTOR_DISPLAY  21          // Block to Raster Interrupt
    #define VECTOR_FLASH    22          // Serial Flash interrupt
    #define VECTOR_MJC      23          // MJC Interrupt
    #define VECTOR_VDEC     24          // VDEC interrupt
    #define VECTOR_GFX      25          // Graphic Interrupt
    #define VECTOR_DEMUX    26          // Transport demuxer interrupt
    #define VECTOR_DEMOD    27          // Demod interrupt enable bit
    #define VECTOR_USB1     28          // USB1 interrupt
    #define VECTOR_SDIO     29          // SDIO interrupt
    #define VECTOR_DDI      30          // DDI interrupt
    #define VECTOR_MISC     31          // MISC interrupt
    #define VECTOR_DRAMC    32          // DRAM Controller interrupt
    #define VECTOR_EXT1     33          // Dedicated GPIO edge interrupt
    #define VECTOR_EXT2     34          // Polarity/Edge/Level common GPIO interrupt
    #define VECTOR_IDETP    35          // IDE Test Port interrupt
    #define VECTOR_OSD      36          // OSD interrupt
    #define VECTOR_PMU      37          // ARM performance monitor interrupt
    #define VECTOR_MAIN_FDET    38      // Main clock frequency detector interrupt
    #define VECTOR_PIP_FDET 39          // PIP clock frequency detector interrupt
// Interrupt bits
    #define _IRQ(v)         (1U << (v)) // IRQ bit by vector
    #define _MISCIRQ(v)     (1U << (v - 32))    // MISC IRQ bit by vector

#define REG_MISC_IRQST      0x0048      // MISC Interrupt Status Register
#define REG_MISC_IRQEN      0x004C      // MISC Interrupt Enable Register
#define REG_MISC_IRQCLR     0x005C      // MISC Interrupt Clear Register
    #define MAX_MISC_VECTOR 8

//----------------------------------------------------------------------------
// Timer registers
#define REG_RW_TIMER0_LLMT  0x0060      // RISC Timer 0 Low Limit Register
// #define REG_T0LMT           0x0060      // RISC Timer 0 Low Limit Register
#define REG_RW_TIMER0_LOW   0x0064      // RISC Timer 0 Low Register
// #define REG_T0              0x0064      // RISC Timer 0 Low Register
#define REG_RW_TIMER0_HLMT  0x0180      // RISC Timer 0 High Limit Register
#define REG_RW_TIMER0_HIGH  0x0184      // RISC Timer 0 High Register

#define REG_RW_TIMER1_LLMT  0x0068      // RISC Timer 1 Low Limit Register
// #define REG_T1LMT           0x0068      // RISC Timer 1 Low Limit Register
#define REG_RW_TIMER1_LOW   0x006c      // RISC Timer 1 Low Register
// #define REG_T1              0x006c      // RISC Timer 1 Low Register
#define REG_RW_TIMER1_HLMT  0x0188      // RISC Timer 1 High Limit Register
#define REG_RW_TIMER1_HIGH  0x018c      // RISC Timer 1 High Register

#define REG_RW_TIMER2_LLMT  0x0070      // RISC Timer 2 Low Limit Register
// #define REG_T2LMT           0x0070      // RISC Timer 2 Low Limit Register
#define REG_RW_TIMER2_LOW   0x0074      // RISC Timer 2 Low Register
// #define REG_T2              0x0074      // RISC Timer 2 Low Register
#define REG_RW_TIMER2_HLMT  0x0190      // RISC Timer 2 High Limit Register
#define REG_RW_TIMER2_HIGH  0x0194      // RISC Timer 2 High Register

#define REG_RW_TIMER_CTRL   0x0078      // RISC Timer Control Register
// #define REG_TCTL            0x0078      // RISC Timer Control Register
    #define TMR0_CNTDWN_EN  (1U << 0)   // Timer 0 enable to count down
    // #define TCTL_T0EN       (1U << 0)   // Timer 0 enable
    #define TMR0_AUTOLD_EN  (1U << 1)   // Timer 0 auto-load enable
    // #define TCTL_T0AL       (1U << 1)   // Timer 0 auto-load enable
    #define TMR1_CNTDWN_EN  (1U << 8)   // Timer 1 enable to count down
    // #define TCTL_T1EN       (1U << 8)   // Timer 1 enable
    #define TMR1_AUTOLD_EN  (1U << 9)   // Timer 1 auto-load enable
    // #define TCTL_T1AL       (1U << 9)   // Timer 1 auto-load enable
    #define TMR2_CNTDWN_EN  (1U << 16)  // Timer 2 enable to count down
    // #define TCTL_T2EN       (1U << 16)  // Timer 2 enable
    #define TMR2_AUTOLD_EN  (1U << 17)  // Timer 2 auto-load enable
    // #define TCTL_T2AL       (1U << 17)  // Timer 2 auto-load enable
    #define TMR_CNTDWN_EN(x) (1U << (x*8))
    #define TMR_AUTOLD_EN(x) (1U << (1+(x*8)))

//----------------------------------------------------------------------------
// Debug monitor path registers
#define REG_RW_DBG_CTRL     0x007c      // RISC Debug Control Register
    #define DBG_LOG_EN      (1U << 0)    // RISC Debug logging and RISC stop function enable
    #define DBG_FC_CKL_STOP (1U << 1)    // Force RISC clock stopped
    #define DBG_CHK_STOP    (1U << 2)    // While check point is hit, force RISC clock stopped
    #define DBG_CHK_SLOG    (1U << 3)    // While .., the Debug interface stop logging to SRAM
    #define DBG_LOG_MODE    (3U << 4)    // Log data mdoe to SRAM
    #define DBG_LOGWR_EN    (1U << 6)    // 1 Log write to SRAM enable, 0 read enable.
    #define DBG_LADR_INC    (1U << 7)    // Log read addr pointer incremented 1 after read.
    #define DBG_CHK1_FLG    (1U << 8)    // RISC Debug check point 1 hit flag
    #define DBG_CHK2_FLG    (1U << 9)    // RISC Debug check point 2 hit flag
    #define DBG_CHKA_FLAG   (1U << 10)   // Abort hit flag
    #define DBG_CHKD_FLAG   (1U << 11)   // DRAM interrupt hit flag
    #define DBG_ABRT_STOP   (1U << 16)   // Abort occurs, force RISC clock stopped.
    #define DBG_ABRT_SLOG   (1U << 17)   // Abort occurs, stop logging to SRAM
    #define DBG_DRAM_STOP   (1U << 18)   // DRAM interrupt, force RISC clock stopped.
    #define DBG_DRAM_SLOG   (1U << 19)   // DRAM interrupt, stop logging to SRAM.
    #define DBG_PCLGEN      (1U << 24)   // Program Counter Log Enable, 0: AMBA Log.
    #define DBG_PCLG_NSQ    (1U << 25)   // Log PC only when PC non-sequence
    #define DBG_PCLG_TY     (3U << 26)   // PC Log type
        #define DBG_PC_LG_ARM_MODE  (0U << 26) // ARM mode
        #define DBG_PC_LG_THMB_MODE (1U << 26) // Thumb mode
        #define DBG_PC_LG_JAVA_MODE (2U << 26) // Java mode
        #define DBG_PC_LG_AUDO_MODE (3U << 26) // auto-mode
    #define DBG_PCLG_CHK1EN (1U << 28)   // PC Log check 1 pointer enable
    #define DBG_PCLG_CHK2EN (1U << 29)   // PC Log check 2 pointer enable

#define REG_RW_DBG_CP1      0x0080      // two formats when DBG_PCLGEN
#define REG_RW_DBG_CP2      0x0084      // two formats when DBG_PCLGEN
#define REG_RW_DBG_LOGWA    0x0088      // Log Write Address Register
#define REG_RW_DBG_LOGRA    0x008c      // Log Read Address Register
#define REG_RW_DBG_LOGDATA  0x0090      // RISC Debug Log Data Register
#define REG_RW_DBG_MONSEL   0x0094      // Monitor Select Register

//----------------------------------------------------------------------------
// MISC register
#define REG_RW_MISC         0x0098      // MISC. Register
    #define MISC_MI_BP      (1U << 0)    // Multi-ICE JTAG signal by pass mode
    #define MISC_EI_POL     (1U << 1)    // External interrupt polarity 0:low/fall 1:high/rise
    #define MISC_EI_LEV     (1U << 2)    // External interrupt 0:edge(fall/rise), 1:level.
    #define MISC_IRQ_BP     (1U << 3)    // RISC IRQ 0:registered connected/1:by pass
    #define MISC_FIQ_BP     (1U << 4)    // RISC FIQ 0:registered connected/1:by pass 
    #define MISC_INT_ST     (1U << 5)    // Interrupt status bit 0: events & IRQEN/FIQEN, 1:events
    #define MISC_RSREG      (1U << 6)    // RS232 read/write RISC IO register enable
    #define MISC_EI_2ED     (1U << 7)    // * External interrupt double edge sensitive
    #define MISC_MKD        (1U << 8)    // Mask DRAM interface when RISC<->DRAM timeout. 0: clear
    #define MISC_MKDE       (1U << 9)    // DRAM mask enable
    #define MISC_IDEREG     (1U << 10)   // IDE read/write RISC IO register enable
    #define MISC_SW7CE0     (1U << 11)   // Switch 7 segment chip select 0 to HREPS[0]
    #define MISC_SW7CE1     (1U << 12)   // Switch 7 segment chip select 0 to chk1_Exe
    #define MISC_SW7CE2     (1U << 13)   // Switch 7 segment chip select 0 to chk2_exe
    #define MISC_SW7CE3     (1U << 14)   // Switch 7 segment chip select 0 to dramctl_int


//----------------------------------------------------------------------------
// RISC General Purpose Registers
#define REG_RW_GPRB0        0x00e0      // RISC Byte General Purpose Register 0
#define REG_RW_GPRB1        0x00e4      // RISC Byte General Purpose Register 1
#define REG_RW_GPRB2        0x00e8      // RISC Byte General Purpose Register 2
#define REG_RW_GPRB3        0x00ec      // RISC Byte General Purpose Register 3
#define REG_RW_GPRB4        0x00f0      // RISC Byte General Purpose Register 4
#define REG_RW_GPRB5        0x00f4      // RISC Byte General Purpose Register 5
#define REG_RW_GPRB6        0x00f8      // RISC Byte General Purpose Register 6
#define REG_RW_GPRB7        0x00fc      // RISC Byte General Purpose Register 7
#define REG_RW_GPRDW0       0x0100      // RISC Double Word General Purpose Register 0
#define REG_RW_GPRDW1       0x0104      // RISC Double Word General Purpose Register 1
#define REG_RW_GPRDW2       0x0108      // RISC Double Word General Purpose Register 2
#define REG_RW_GPRDW3       0x010c      // RISC Double Word General Purpose Register 3

//----------------------------------------------------------------------------
// PBI Timing
#define REG_RW_PB0WT        0x0118      // PBI Bank 0 WE Timing Register
#define REG_RW_PB1WT        0x011c      // PBI Bank 1 WE Timing Register
#define REG_RW_PB2WT        0x0120      // PBI Bank 2 WE Timing Register

//----------------------------------------------------------------------------
// 7-segment controller
#define REG_RW_SEG7_DATA    0x0124      // 7-segment display data
#define REG_SEG7            0x124       // 7-segment display
#define REG_RW_SEG7_TIMING  0x0128      // 7-segment display timing
    #define SEG7_CT0_MASK   (0xffU << 0) // clk num of wait states from data valid to CE-act
    #define SEG7_CT1_MASK   (0xffU << 8) // clk num of wait states from CE-act to CE-deact
    #define SEG7_CT2_MASK   (0xffU <<16) // clk num of wait states from CE-deact to data valid

//----------------------------------------------------------------------------
// Ring Oscillator
#define REG_RW_ROSCCTL      0x012c      // Ring Oscillator Control Register
    #define ROSC_TRIGGER    (1U << 0)    // Ring-oscillator trigger bit
    #define ROSC_CLR        (1U << 1)    // Clear ring-oscillator counter & ref.cnt.reg
    #define ROSC_SEL        (1U << 2)    // 1 before set TRIG, 0 before set CLR
#define REG_RO_ROSCCNT      0x0130      // Ring Oscillator Reference Counter Register

//----------------------------------------------------------------------------
// NAND DMA ...

//----------------------------------------------------------------------------
// SRAM BIST
#define REG_RW_BISTCTL      0x0150      // SRAM BIST Control Register
#define REG_RW_BISTCTLST    0x0154      // SRAM BIST Controller Status Register
#define REG_RW_BIST0EN      0x0158      // SRAM BIST Enable 0 Register
#define REG_RW_BIST1EN      0x015c      // SRAM BIST Enable 1 Register
#define REG_RW_BIST2EN      0x0160      // SRAM BIST Enable 2 Register
#define REG_RW_BIST3EN      0x0164      // SRAM BIST Enable 3 Register
#define REG_RW_BIST4EN      0x0168      // SRAM BIST Enable 4 Register
#define REG_RW_BIST0ST      0x016c      // SRAM BIST Status 0 Register
#define REG_RW_BIST1ST      0x0170      // SRAM BIST Status 1 Register
#define REG_RW_BIST2ST      0x0174      // SRAM BIST Status 2 Register
#define REG_RW_BIST3ST      0x0178      // SRAM BIST Status 3 Register
#define REG_RW_BIST4ST      0x017c      // SRAM BIST Status 4 Register

//----------------------------------------------------------------------------
// CPI
#define REG_RW_CPI0         0x01a0      // CPI meter 0 Register
#define REG_RW_CPI1         0x01a4      // CPI meter 1 Register
#define REG_RW_CPI2         0x01a8      // CPI meter 2 Register
#define REG_RW_CPI3         0x01ac      // CPI meter 3 Register
#define REG_RW_CPICFG       0x01b0      // CPI meter configuration register
    #define CPICFG_CLR      (1U << 1)   // Clear CPI meter
    #define CPICFG_EN       (1U << 0)   // Enable CPI meter
    
//----------------------------------------------------------------------------
// LZHS
#define REG_LZHS_CTRL		0x01C0
#define REG_LZHS_R_ADDR		0x01C4
#define REG_LZHS_W_ADDR		0x01C8
#define REG_LZHS_W_END_ADDR	0x01CC    
#define REG_LZHS_RAMCK      0x01D0
    #define LZHS_CHGOK      (1U << 5)   // LZHS SRAM change index
    #define SRAM_CLK_SEL    (1U << 4)   // LZHS clock selection
    #define LZHS_SRAM_EN    (1U << 3)   // LZHS SRAM enable bit

//----------------------------------------------------------------------------
// BIM
#define REG_RW_CACHESZCFG   0x01f0      // Cache Size Configuration Register
    #define DCACHE_MASK     0x0f00
    #define DCACHE_VAL(x)   (x << 8)
    #define ICACHE_MASK     0x000f
    #define ICACHE_VAL(x)   (x << 0)
#define REG_WO_BIM_RSTCTL   0x01f4      // BIM Reset Control Register
#define REG_RO_FPGA_ID      0x01f8      // FPGA ID Register
#define REG_RO_CHIP_ID      0x01fc      // Chip ID Register

//----------------------------------------------------------------------------
// PWM ...
#define REG_RW_PWMLVDS      0x022c
#define REG_RW_PWM0         0x0230      // PWM0 Register
#define REG_RW_PWM1         0x0234      // PWM1 Register
#define REG_RW_PWM2         0x0238      // PWM2 Register
    #define PWME            (1)             // enable
    #define PWMALD          (0x2)           // auto load PWMH from Demux
    #define PWMP_MASK       (0x0000ff00)    // prescaler
    #define PWMH_MASK       (0x00ff0000)    // high period
    #define PWMRSN_MASK     (0xff000000)    // resolution byte

//----------------------------------------------------------------------------
// SIF ...

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
#define REG_RW_HDCP_DEV     0x02E0      // Internal HDCP Device Address Register
#define REG_RW_HDCP_ADDR    0x02E4      // Internal HDCP SRAM Address Register
#define REG_RW_HDCP_DATA    0x02E8      // Internal HDCP SRAM Data Register

//----------------------------------------------------------------------------
// NAND ...

//----------------------------------------------------------------------------
// BIST
#define REG_RW_BISTCTRL     0x0500      // SRAM BIST Control Register
    #define BISTCTRL_SLOW           (1U << 8)   // Set 1 to enable BIST slow mode
    #define BISTCTRL_RSTBIST        (1U << 7)   // Reset all BIST Status to 0
    #define BISTCTRL_TESTPAT_UD     (0x3U << 5) // BIST test pattern options for up/down count
    #define BISTCTRL_TESTPAT_SEQ    (0x3U << 3) // BIST test pattern options sequence
    #define BISTCTRL_DIAGEN         (1U << 2)   // Diagnosis enable
    #define BISTCTRL_FORCEFAIL      (1U << 1)   // Force all register into fail status, just for test
    #define BISTCTRL_BISTTRIG       (1U << 0)   // BIST trigger
#define REG_RO_BISTSTAT     0x0504      // SRAM BIST Controller Status Register
    #define BISTSTAT_STATE  (0xfU << 28)
    #define BISTSTAT_RCNT   (3U << 26)
    #define BISTSTAT_WCNT   (3U << 24)
    #define BISTSTAT_VERIFY (1U << 23)
    #define BISTSTAT_PHIW   (1U << 22)
    #define BISTSTAT_PHIR   (1U << 21)
    #define BISTSTAT_PHIV   (1U << 20)
    #define BISTSTAT_EXPD   (3U << 18)
    #define BISTSTAT_UP     (1U << 17)
    #define BISTSTAT_LAST   (1U << 16)
    #define BISTSTAT_FR     (1U << 15)
    #define BISTSTAT_WALKADDR   (0x3ffU)
#define REG_RW_BISTEN0      0x0508      // BIST Enable 0 Register
#define REG_RW_BISTST0      0x050c      // BIST Status 0 Register
    #define BIST0_AUD_WR    (1U << 31)
    #define BIST0_AUD_RD    (1U << 30)
    #define BIST0_VD0IN     (0x1fffff << 9)
    #define BIST0_HDCP      (1U << 8)
    #define BIST0_SDIO      (1U << 7)
    #define BIST0_FCI       (1U << 6)
    #define BIST0_POD_TX    (1U << 5)
    #define BIST0_POD_RX    (1U << 4)
    #define BIST0_DFST      (3U << 2)
    #define BIST0_DTCM      (1U << 1)
    #define BIST0_ITCM      (1U << 0)
#define REG_RW_BISTEN1      0x0510      // BIST Enable 1 Register
#define REG_RW_BISTST1      0x0514      // BIST Status 1 Register
    #define BIST1_MCS_MB    (1U << 31)
    #define BIST1_IDCT_TM   (1U << 30)
    #define BIST1_MC_MB     (1U << 29)
    #define BIST1_MC_OBC    (1U << 28)
    #define BIST1_VLD_JS2   (1U << 27)
    #define BIST1_VLD_JS1   (1U << 26)
    #define BIST1_VLD_MV    (1U << 25)
    #define BIST1_VLD_EC    (1U << 24)
    #define BIST1_VLD_IQ    (1U << 23)
    #define BIST1_VLD_IS    (1U << 22)
    #define BIST1_VLD_VB    (1U << 21)
    #define BIST1_ITXBUF    (1U << 20)
    #define BIST1_ATXBUFHI  (1U << 19)
    #define BIST1_ATXBUFLO  (1U << 18)
    #define BIST1_IRXBUF    (1U << 17)
    #define BIST1_ARXBUF    (1U << 16)
    #define BIST1_AVTXBUF1  (1U << 15)
    #define BIST1_AVTXBUF0  (1U << 14)
    #define BIST1_AVRXBUF1HI    (1U << 13)
    #define BIST1_AVRXBUF0HI    (1U << 12)
    #define BIST1_AVRXBUF1LO    (1U << 11)
    #define BIST1_AVRXBUF0LO    (1U << 10)
    #define BIST1_PID       (3U << 8)
    #define BIST1_AIN       (0x3f << 2)
    #define BIST1_AUD_IC    (1U << 1)
    #define BIST1_AUD_RTCMPT    (1U << 0)
#define REG_RW_BISTEN2      0x0518      // BIST Enable 2 Register
#define REG_RW_BISTST2      0x051c      // BIST Status 2 Register
    #define BIST2_TPPSWF_YL (1U << 31)
    #define BIST2_TPPSWF_CE (1U << 30)
    #define BIST2_TPPSWF_YE (1U << 29)
    #define BIST2_TPPSXF_CL (1U << 28)
    #define BIST2_TPPSXF_YL (1U << 27)
    #define BIST2_YPPSCL2   (1U << 26)
    #define BIST2_TPPSYL2   (1U << 25)
    #define BIST2_TPPSCL    (1U << 24)
    #define BIST2_TPPSYL    (1U << 23)
    #define BIST2_TPPSCS    (1U << 22)
    #define BIST2_TPPSYS    (1U << 21)
    #define BIST2_TMPSSAWCOMB   (1U << 20)
    #define BIST2_TMPSEDGE  (1U << 19)
    #define BIST2_TMPSZF_CS (1U << 18)
    #define BIST2_TMPSZF_YS (1U << 17)
    #define BIST2_TMPSWF_CL (1U << 16)
    #define BIST2_TMPSWF_YL (1U << 15)
    #define BIST2_TMPSWF_CE (1U << 14)
    #define BIST2_TMPSWF_YE (1U << 13)
    #define BIST2_TMPSXF_CL (1U << 12)
    #define BIST2_TMPSXF_YL (1U << 11)
    #define BIST2_TMPSC_L2  (1U << 10)
    #define BIST2_TMPSY_L2  (1U << 9)
    #define BIST2_TMPSC_L   (1U << 8)
    #define BIST2_TMPSY_L   (1U << 7)
    #define BIST2_TMPSCS    (1U << 6)
    #define BIST2_TMPSYS    (1U << 5)
    #define BIST2_B2R0      (1U << 4)
    #define BIST2_B2R1      (1U << 3)
    #define BIST2_B2R2      (1U << 2)
    #define BIST2_B2R3      (1U << 1)
    #define BIST2_MCS_AB    (1U << 0)
#define REG_RW_BISTEN3      0x0520      // BIST Enable 3 Register
#define REG_RW_BISTST3      0x0524      // BIST Status 3 Register
    #define BIST3_SCPOS_4   (1U << 31)
    #define BIST3_SCPOS_5   (1U << 30)
    #define BIST3_SCPOS_6   (1U << 29)
    #define BIST3_SCPOS_7   (1U << 28)
    #define BIST3_SCPOS_8   (1U << 27)
    #define BIST3_SCPOS_9   (1U << 26)
    #define BIST3_SCPOS_10  (1U << 25)
    #define BIST3_SCPOS_11  (1U << 24)
    #define BIST3_SCPOS_12  (1U << 23)
    #define BIST3_OSDCSR    (1U << 22)
    #define BIST3_OSDSCD    (1U << 21)
    #define BIST3_OSDSCC    (1U << 20)
    #define BIST3_OSDSCB    (1U << 19)
    #define BIST3_OSDSCA    (1U << 18)
    #define BIST3_OSD3_P    (1U << 17)
    #define BIST3_OSD3_F    (1U << 16)
    #define BIST3_OSD2_P    (1U << 15)
    #define BIST3_OSD2_F    (1U << 14)
    #define BIST3_OSD1_P    (1U << 13)
    #define BIST3_OSD1_F    (1U << 12)
    #define BIST3_GRASC_SCB (1U << 11)  // GRASC_SCFIF_O_B
    #define BIST3_GRASC_SCA (1U << 10)  // GRASC_SCFIF_O_A
    #define BIST3_GRASC     (1U << 9)
    #define BIST3_GRASC_CMD (1U << 8)
    #define BIST3_GRA       (1U << 7)
    #define BIST3_GRA_CMD   (1U << 6)
    #define BIST3_SHARP     (1U << 5)
    #define BIST3_TPPSSAWCOMB   (1U << 4)
    #define BIST3_TPPSEDGE  (1U << 3)
    #define BIST3_TPPSSZ_CS (1U << 2)
    #define BIST3_TPPSZF_YS (1U << 1)
    #define BIST3_TPPSWF_CL (1U << 0)
#define REG_RW_BISTEN4      0x0528      // BIST Enable 4 Register
#define REG_RW_BISTST4      0x052c      // BIST Status 4 Register
    #define BIST4_ADAC      (1U << 4)
    #define BIST4_SCPOS_0   (1U << 3)
    #define BIST4_SCPOS_1   (1U << 2)
    #define BIST4_SCPOS_2   (1U << 1)
    #define BIST4_SCPOS_3   (1U << 0)
#define REG_RW_BISTEN5      0x0530      // BIST Enable 5 Register
#define REG_RW_BISTST5      0x0534      // BIST Status 5 Register
    #define BIST5_DSP       (7U << 16)
    #define BIST5_CPU       (0x3fffU)
#define REG_RW_BISTEN6      0x0538      // BIST Enable 6 Register
#define REG_RW_BISTST6      0x053c      // BIST Status 6 Register
    #define BIST6_DEMUX     (0x3fffffU)

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
// Constant for TIME Log
//-----------------------------------------------------------------------------

#define BIM_SetTimeLog(x)       BIM_WRITE32((REG_RW_GPRB0 + (x << 2)), BIM_READ32(REG_RW_TIMER2_LOW))


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
// RTC related interfaces moved to rtc_if.h
/*
typedef struct
{
    UINT8   u1Year;
    UINT8   u1Month;
    UINT8   u1Day;
    UINT8   u1Hour;
    UINT8   u1Minute;
    UINT8   u1Second;
} RTC_T;

typedef void (*PFN_RTC_ALERT_HANDLER_T)(void);
*/

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

EXTERN BOOL LZHS_SetIsrEnable(BOOL fgSet);

EXTERN INT32 LZHS_Dec(UINT32 u4SrcAddr, UINT32 u4DestAddr, UINT32 u4Len, UINT8 *pu1CheckSum);

// RTC related interfaces moved to rtc_if.h
/*
EXTERN BOOL RTC_IsAllRegsBcd(void);

EXTERN BOOL RTC_RtcToUtc(UINT64* pu8Utc, const RTC_T* prRtc);

EXTERN BOOL RTC_UtcToRtc(RTC_T* prRtc, UINT64 u8Utc);

EXTERN BOOL RTC_Init(void);

EXTERN BOOL RTC_GetTimeDate(UINT64* prTime);

EXTERN INT32 RTC_SetTimeDate(const UINT64* prTime);

EXTERN void RTC_SetAlertTime(UINT64 u8Utc);

EXTERN void RTC_HookAlertHandler(PFN_RTC_ALERT_HANDLER_T pfnHandler);
*/

#endif  // X_BIM_H

