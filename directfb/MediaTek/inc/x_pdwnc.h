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
 * $RCSfile: x_pdwnc.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file x_pdwnc.h
 *  x_pdwnc.h The PDWNC (Power Down Interface Module) interface header file
 */


#ifndef X_PDWNC_H
#define X_PDWNC_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_hal_5381.h"
#include "x_gpio.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN
#ifdef CC_5391_LOADER
#ifndef CC_UBOOT
typedef VOID (*x_os_isr_fct) (UINT16  ui2_vector_id);
#endif /* CC_UBOOT */
#else
#include "x_os.h"
#endif
LINT_EXT_HEADER_END

//===========================================================================
// Macros for register read/write

#define PDWNC_READ8(offset)         IO_READ8(PDWNC_BASE, (offset))
#define PDWNC_READ16(offset)        IO_READ16(PDWNC_BASE, (offset))
#define PDWNC_READ32(offset)        IO_READ32(PDWNC_BASE, (offset))

#define PDWNC_WRITE8(offset, value) IO_WRITE8(PDWNC_BASE, (offset), (value))
#define PDWNC_WRITE16(offset,value) IO_WRITE16(PDWNC_BASE, (offset), (value))
#define PDWNC_WRITE32(offset,value) IO_WRITE32(PDWNC_BASE, (offset), (value))

#define PDWNC_REG8(offset)          IO_REG8(PDWNC_BASE, (offset))
#define PDWNC_REG16(offset)         IO_REG16(PDWNC_BASE, (offset))
#define PDWNC_REG32(offset)         IO_REG32(PDWNC_BASE, (offset))


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define PDWNC_OK                0
#define PDWNC_FAIL              1
#define TOTAL_PDWNC_GPIO_NUM    13
#define TOTAL_PDWNC_DEDICATE_GPIO_NUM    8
#define TOTAL_SERVOADC_NUM      7

//===========================================================================
// PDWNC register and bitmap defintions
#define REG_RW_CLKPWD       0x0000      // Clock Power Down Register
    #define PD_IOC_STOP     (1U << 0)   // IO control module clock stop
    #define PD_IRRX_STOP    (1U << 1)   // IR Receiver module clock stop
    #define PD_SRVAD_STOP   (1U << 2)   // Servo ADC interface module clock stop
    #define PD_VGA_STOP     (1U << 3)   // VGA Sync detector module clock stop
    #define PD_RTC_STOP     (1U << 4)   // RTC module clock stop
#define REG_RW_IRCLK_SEL    0x0004      // IR Receiver Clock Selection Register
    #define IRRX_CLK_30MHZ_DIV_1    0   // IRRX clock set in 30MHZ
    #define IRRX_CLK_30MHZ_DIV_2    1   // IRRX clock set in 30MHZ/2
    #define IRRX_CLK_30MHZ_DIV_4    2   // IRRX clock set in 30MHZ/4
    #define IRRX_CLK_30MHZ_DIV_8    3   // IRRX clock set in 30MHZ/8
    #define IRRX_CLK_30MHZ_DIV_16   4   // IRRX clock set in 30MHZ/16
    #define IRRX_CLK_30MHZ_DIV_32   5   // IRRX clock set in 30MHZ/32
    #define IRRX_CLK_30MHZ_DIV_64   6   // IRRX clock set in 30MHZ/64
    #define IRRX_CLK_30MHZ_DIV_128  7   // IRRX clock set in 30MHZ/128
    #define IRRX_CLK_30MHZ_DIV_256  8   // IRRX clock set in 30MHZ/256
    #define IRRX_CLK_30MHZ_DIV_511  9   // IRRX clock set in 30MHZ/511
                                        
#define REG_RW_PDWNC_MISC   0x0008      // PDWNC MISC Register
    #define PD_MISC_SYS_LED         (0x0ffU << 0)   // System LED output selection
    #define PD_MISC_SYSLED_CFG      (1U << 8)       // System LED config, 0: twinkle/1s, 1:twinkle/2s
    #define PD_MISC_PDWNC_POLARITY  (1U << 14)      // PDWNC Polarity, default 0, high active.
    #define PD_MISC_IR_LED          (0x0ffU << 16)  // IR LED output selection

//===========================================================================
// PDWNC Interrupt status/enable/clear registers
#define REG_RO_PDWNC_INTST  0x0040      // PDWNC Interrupt Status Register
    #define PINTNO_GPIO0    0           // OPCTRL 0 interrupt number
    #define PINTNO_GPIO1    1           // OPCTRL 1 interrupt number
    #define PINTNO_GPIO2    2           // OPCRTL 2 interrupt number
    #define PINTNO_GPIO3    3           // OPCTRL 3 interrupt number
    #define PINTNO_GPIO4    4           // OPCTRL 4 interrupt number
    #define PINTNO_GPIO5    5           // OPCTRL 5 interrupt number
    #define PINTNO_GPIO6    6           // OPCTRL 6 interrupt number
    #define PINTNO_GPIO7    7           // OPCTRL 7 interrupt number
    #define PINTNO_UART_DBG 9           // UART debug interrupt number
    #define PINTNO_SIF      10			// SIF interrupt number
    #define PINTNO_CEC      11          // CEC interrupt number
    #define PINTNO_RTC      12          // RTC interrupt number
    #define PINTNO_IRRX     13          // IRRX interrupt number
    #define PINTNO_OPWR0_5V 14          // OPWR0 interrupt number
    #define PINTNO_OPWR1_5V 15          // OPRW1 interrupt number
    #define PINTNO_OPWR2_5V 16          // OPWR2 interrupt number
    #define PINTNO_OPWM0    17          // OPWM0 interrupt number
    #define PINTNO_T8032    18          // T8032 interrupt number
    #define MAX_PINT_ID     18          // Maximum value of PDWNC interrupt id
    #define _PINT(v)        (1U << (v))
    #define PINT_GPIO0      _PINT(PINTNO_GPIO0)     // OPCTRL 0 interrupt status
    #define PINT_GPIO1      _PINT(PINTNO_GPIO1)     // OPCTRL 1 interrupt status
    #define PINT_GPIO2      _PINT(PINTNO_GPIO2)     // OPCTRL 2 interrupt status
    #define PINT_GPIO3      _PINT(PINTNO_GPIO3)     // OPCTRL 3 interrupt status
    #define PINT_GPIO4      _PINT(PINTNO_GPIO4)     // OPCTRL 4 interrupt status
    #define PINT_GPIO5      _PINT(PINTNO_GPIO5)     // OPCTRL 5 interrupt status
    #define PINT_GPIO6      _PINT(PINTNO_GPIO6)     // OPCTRL 6 interrupt status
    #define PINT_GPIO7      _PINT(PINTNO_GPIO7)     // OPCTRL 7 interrupt status 
    #define PINT_UART_DBG   _PINT(PINTNO_UART_DBG)  // UART debug interrupt status
    #define PINT_SIF        _PINT(PINTNO_SIF)       // SIF interrupt status
    #define PINT_CEC        _PINT(PINTNO_CEC)       // CEC interrupt status
    #define PINT_RTC        _PINT(PINTNO_RTC)       // RTC interrupt status
    #define PINT_IRRX       _PINT(PINTNO_IRRX)      // IRRX interrupt status bit
    #define PINT_OPWR0_5V   _PINT(PINTNO_OPWR0_5V)  // OPWR0 interrupt status
    #define PINT_OPWR1_5V   _PINT(PINTNO_OPWR1_5V)  // OPWR1 interrupt status
    #define PINT_OPWR2_5V   _PINT(PINTNO_OPWR2_5V)  // OPWR2 interrupt status    
    #define PINT_OPWM0      _PINT(PINTNO_OPWM0)     // OPWM0 interrupt status
    
#define REG_RW_PDWNC_INTEN  0x0044      // PDWNC Interrupt Enable Register
    #define OPCTRL_INTEN(x)  ((x<=TOTAL_PDWNC_DEDICATE_GPIO_NUM)?(0x1U << (x)):(0x1U << (x+5)))  
#define REG_WO_PDWNC_INTCLR 0x0048      // PDWNC Interrupt Clear Register

//===========================================================================
// PDWNC PAD configuration registers
#define REG_RW_IOPADEN      0x0080                  // IO Pad Output Enable Register
    #define PAD_GPIO_OUT_EN     (0xffU << 0)            // GPIO output enable
    #define PAD_SERVOADC_EN_ALL (0x7fU)        // SERVO ADC PAD output enable
    #define PAD_SERVOADC_EN(x)  (1U << (x))    // SERVO ADC PAD output enable    
    #define PAD_UR0_TX_EN       (1U << 15)              // UART 0 TX PAD output enable
#define REG_RW_IOPADOUT     0x0084                  // IO Pad Output Register
    #define PAD_GPIO_OUT        (0xffU << 0)            // GPIO data output
    #define PAD_SRVIO_OUT_ALL   (0x7fU)// SERVO ADC PAD data output
    #define PAD_SRVIO_OUT(x)    (0x1U << (x))
#define REG_RO_IOPADIN      0x0088      // IO Pad Input Register
    #define PAD_GPIO_IN         (0xffU << 0)// GPIO data input
    #define PAD_SRVIO_IN_ALL    (0x7fU)// SERVO ADC PAD data input
    #define PAD_SRVIO_IN(x)     (0x1U << (x))    
#define REG_RW_PADCFG0      0x008c      // PAD Configuration Register 0
    #define GPIO0_MASK          (0x3fU << 0)
    #define GPIO0_DRV(x)        (((x) & 0x03) << 0) // driving current
    #define GPIO0_SR(x)         (((x) & 0x03) << 2) // slew rate
    #define GPIO0_PD(x)         (((x) & 0x03) << 3) // pull down resistor enable bit
    #define GPIO0_PU(x)         (((x) & 0x03) << 4) // pull up resistor enable bit
    #define GPIO0_SMT(x)        (((x) & 0x03) << 5) // schmitt trigger enable bit
    #define GPIO1_MASK          (0x3fU << 8)
    #define GPIO1_DRV(x)        (((x) & 0x03) << 8) // driving current
    #define GPIO1_SR(x)         (((x) & 0x03) <<10) // slew rate
    #define GPIO1_PD(x)         (((x) & 0x03) <<11) // pull down resistor enable bit
    #define GPIO1_PU(x)         (((x) & 0x03) <<12) // pull up resistor enable bit
    #define GPIO1_SMT(x)        (((x) & 0x03) <<13) // schmitt trigger enable bit
    #define GPIO2_MASK          (0x3fU <<16)
    #define GPIO2_DRV(x)        (((x) & 0x03) <<16) // driving current
    #define GPIO2_SR(x)         (((x) & 0x03) <<18) // slew rate
    #define GPIO2_PD(x)         (((x) & 0x03) <<19) // pull down resistor enable bit
    #define GPIO2_PU(x)         (((x) & 0x03) <<20) // pull up resistor enable bit
    #define GPIO2_SMT(x)        (((x) & 0x03) <<21) // schmitt trigger enable bit
    #define GPIO3_MASK          (0x3fU <<24)
    #define GPIO3_DRV(x)        (((x) & 0x03) <<24) // driving current
    #define GPIO3_SR(x)         (((x) & 0x03) <<26) // slew rate
    #define GPIO3_PD(x)         (((x) & 0x03) <<27) // pull down resistor enable bit
    #define GPIO3_PU(x)         (((x) & 0x03) <<28) // pull up resistor enable bit
    #define GPIO3_SMT(x)        (((x) & 0x03) <<29) // schmitt trigger enable bit
#define REG_RW_PADCFG1      0x0090      // PAD Configuration Register 1
    #define GPIO4_MASK          (0x3fU << 0)
    #define GPIO4_DRV(x)        (((x) & 0x03) << 0) // driving current
    #define GPIO4_SR(x)         (((x) & 0x03) << 2) // slew rate
    #define GPIO4_PD(x)         (((x) & 0x03) << 3) // pull down resistor enable bit
    #define GPIO4_PU(x)         (((x) & 0x03) << 4) // pull up resistor enable bit
    #define GPIO4_SMT(x)        (((x) & 0x03) << 5) // schmitt trigger enable bit
    #define GPIO5_MASK          (0x3fU << 8)
    #define GPIO5_DRV(x)        (((x) & 0x03) << 8) // driving current
    #define GPIO5_SR(x)         (((x) & 0x03) <<10) // slew rate
    #define GPIO5_PD(x)         (((x) & 0x03) <<11) // pull down resistor enable bit
    #define GPIO5_PU(x)         (((x) & 0x03) <<12) // pull up resistor enable bit
    #define GPIO5_SMT(x)        (((x) & 0x03) <<13) // schmitt trigger enable bit
    #define GPIO6_MASK          (0x3fU <<16)
    #define GPIO6_DRV(x)        (((x) & 0x03) <<16) // driving current
    #define GPIO6_SR(x)         (((x) & 0x03) <<18) // slew rate
    #define GPIO6_PD(x)         (((x) & 0x03) <<19) // pull down resistor enable bit
    #define GPIO6_PU(x)         (((x) & 0x03) <<20) // pull up resistor enable bit
    #define GPIO6_SMT(x)        (((x) & 0x03) <<21) // schmitt trigger enable bit
    #define GPIO7_MASK          (0x3fU <<24)
    #define GPIO7_DRV(x)        (((x) & 0x03) <<24) // driving current
    #define GPIO7_SR(x)         (((x) & 0x03) <<26) // slew rate
    #define GPIO7_PD(x)         (((x) & 0x03) <<27) // pull down resistor enable bit
    #define GPIO7_PU(x)         (((x) & 0x03) <<28) // pull up resistor enable bit
    #define GPIO7_SMT(x)        (((x) & 0x03) <<29) // schmitt trigger enable bit
#define PIN_DRV2MA          0U
#define PIN_DRV4MA          1U
#define PIN_DRV6MA          2U
#define PIN_DRV8MA          3U
#define PIN_SLEW_FAST       0U
#define PIN_SLEW_SLOW       1U

#define REG_RW_PADCFG2      0x0094      // PAD Configuration Register 2
    #define PDWN_MASK       (0x3fU << 0)
    #define PDWN_DRV(x)     (((x) & 0x03) << 0) // driving current
    #define PDWN_SR(x)      (((x) & 0x03) << 2) // slew rate
    #define PDWN_PD(x)      (((x) & 0x03) << 3) // pull down resistor enable bit
    #define PDWN_PU(x)      (((x) & 0x03) << 4) // pull up resistor enable bit
    #define PDWN_SMT(x)     (((x) & 0x03) << 5) // schmitt trigger enable bit
    #define IRRX_MASK       (0x3fU << 8)
    #define IRRX_DRV(x)     (((x) & 0x03) << 8) // driving current
    #define IRRX_SR(x)      (((x) & 0x03) <<10) // slew rate
    #define IRRX_PD(x)      (((x) & 0x03) <<11) // pull down resistor enable bit
    #define IRRX_PU(x)      (((x) & 0x03) <<12) // pull up resistor enable bit
    #define IRRX_SMT(x)     (((x) & 0x03) <<13) // schmitt trigger enable bit
    #define U0TX_MASK       (0x3fU <<16)
    #define U0TX_DRV(x)     (((x) & 0x03) <<16) // driving current
    #define U0TX_SR(x)      (((x) & 0x03) <<18) // slew rate
    #define U0TX_PD(x)      (((x) & 0x03) <<19) // pull down resistor enable bit
    #define U0TX_PU(x)      (((x) & 0x03) <<20) // pull up resistor enable bit
    #define U0TX_SMT(x)     (((x) & 0x03) <<21) // schmitt trigger enable bit
    #define U0RX_MASK       (0x3fU <<24)
    #define U0RX_DRV(x)     (((x) & 0x03) <<24) // driving current
    #define U0RX_SR(x)      (((x) & 0x03) <<26) // slew rate
    #define U0RX_PD(x)      (((x) & 0x03) <<27) // pull down resistor enable bit
    #define U0RX_PU(x)      (((x) & 0x03) <<28) // pull up resistor enable bit
    #define U0RX_SMT(x)     (((x) & 0x03) <<29) // schmitt trigger enable bit
#define REG_RW_PWDIOCFG     0x009c      // Power Down IO Configuration Register
    #define GPIO0_POL       (1U << 0)   // GPIO 0 wake-up polarity, 0 active low
    #define GPIO1_POL       (1U << 1)   // GPIO 1 wake-up polarity, 0 active low
    #define GPIO2_POL       (1U << 2)   // GPIO 2 wake-up polarity, 0 active low
    #define GPIO3_POL       (1U << 3)   // GPIO 3 wake-up polarity, 0 active low
    #define GPIO4_POL       (1U << 4)   // GPIO 4 wake-up polarity, 0 active low
    #define GPIO5_POL       (1U << 5)   // GPIO 5 wake-up polarity, 0 active low
    #define GPIO6_POL       (1U << 6)   // GPIO 6 wake-up polarity, 0 active low
    #define GPIO7_POL       (1U << 7)   // GPIO 7 wake-up polarity, 0 active low
    #define GPIOX_POL(x)    (1U << (x)) // GPIO x wake-up polarity
    #define GPIOALL_POL     (0xffU)     // GPIO all wake-up polarity, all active low
    #define GPIO_DEGCNT(x)  (((x) & 0x0ff) << 8)    // debounce period default 60ms   
    #define PDWN_EXTIO0_POL       (1U << 16)   // GPIO 0 wake-up polarity, 0 active low
    #define PDWN_EXTIO1_POL       (1U << 17)   // PDWN_EXTIO 1 wake-up polarity, 0 active low
    #define PDWN_EXTIO2_POL       (1U << 18)   // PDWN_EXTIO 2 wake-up polarity, 0 active low
    #define PDWN_EXTIO3_POL       (1U << 19)   // PDWN_EXTIO 3 wake-up polarity, 0 active low
    #define PDWN_EXTIO4_POL       (1U << 20)   // PDWN_EXTIO 4 wake-up polarity, 0 active low
    #define PDWN_EXTIO5_POL       (1U << 21)   // PDWN_EXTIO 5 wake-up polarity, 0 active low
    #define PDWN_EXTIO6_POL       (1U << 22)   // PDWN_EXTIO 6 wake-up polarity, 0 active low
    #define PDWN_EXTIO7_POL       (1U << 23)   // PDWN_EXTIO 7 wake-up polarity, 0 active low
    #define PDWN_EXTIOX_POL(x)    (1U << ((x) + 16)) // PDWN_EXTIO x wake-up polarity
    #define PDWN_EXTIOALL_POL     (0xffU << 16)     // PDWN_EXTIO all wake-up polarity, all active low
    #define PDWN_EXTIO0_EN       (1U << 24)   // GPIO 0 wake-up polarity, 0 active low
    #define PDWN_EXTIO1_EN       (1U << 25)   // PDWN_EXTIO 1 wake-up polarity, 0 active low
    #define PDWN_EXTIO2_EN       (1U << 26)   // PDWN_EXTIO 2 wake-up polarity, 0 active low
    #define PDWN_EXTIO3_EN       (1U << 27)   // PDWN_EXTIO 3 wake-up polarity, 0 active low
    #define PDWN_EXTIO4_EN       (1U << 28)   // PDWN_EXTIO 4 wake-up polarity, 0 active low
    #define PDWN_EXTIO5_EN       (1U << 29)   // PDWN_EXTIO 5 wake-up polarity, 0 active low
    #define PDWN_EXTIO6_EN       (1U << 30)   // PDWN_EXTIO 6 wake-up polarity, 0 active low
    #define PDWN_EXTIO7_EN       (1U << 31)   // PDWN_EXTIO 7 wake-up polarity, 0 active low
    #define PDWN_EXTIOX_EN(x)    (1U << ((x) + 24)) // PDWN_EXTIO x wake-up polarity
    #define PDWN_EXTIOALL_EN     (0xffU << 24)     // PDWN_EXTIO all wake-up polarity, all active low    
#define REG_RW_PWDIOCNT    0x00a0      // External interrupt Configuration Register    
#define REG_RW_EXTINTCFG    0x00ac      // External interrupt Configuration Register
#define REG_RW_EXTIO2INTCFG 0x00b0
#define REG_RW_PINMUX       0x0b4       // PDWNC PINMUX register
#define REG_RW_SYNCFG0      0x0c0      // Sync Separater Configuration Register 0 /tomson
    #define BYPS_SYNCPROSR  (1U << 1)
#define REG_RW_SYNCFG1      0x0c4      // Sync Separater Configuration Register 1 /tomson
#define REG_RW_SYNCFG2      0x0c8      // Sync Separater Configuration Register 2 /tomson
#define REG_RW_SYNCFG3      0x0cc      // Sync Separater Configuration Register 3 /tomson
    #define SYNC_BIAS_PASV_MASK         (0xfU << 4)
    #define SYNC_BIAS_PASV_SEL(x)       ((x & 0xfU) << 4)
    #define GET_SYNC_BIAS_PASV_SEL(x)   (((x) >> 4) & 0xfU)
    #define SYNC_VLSEL_MASK             (0xfU << 28)
    #define SYNC_VLSEL_VAL(x)           ((x & 0xfU) << 28)
    #define GET_SYNC_VLSEL_VAL(x)       (((x) >> 28) & 7U)
    #define SYNC_VHSEL_MASK             (0xfU << 24)
    #define SYNC_VHSEL_VAL(x)           ((x & 0xfU) << 24)
    #define GET_SYNC_VHSEL_VAL(x)       (((x) >> 24) & 0xfU)
#define REG_RW_HDMICFG0	0x00b4	// HDMI configuration register 0
    #define HDMI_CLK_EN		(1U << 0)
    #define HDMI_CLKBIAS		(3U << 10)    
#define REG_RW_PSAVCFG0	0x00e0
#define REG_RW_WATCHDOG_EN  0x0100      // Watchdog Timer Control Register
#define REG_RW_WATCHDOG_TMR 0x0104      // Watchdog Timer Register
#define REG_RW_WAKE_RSTCNT  0x0108      // Wakeup Reset Counter Register

#define REG_RW_PDWAKE_EN    0x0120      // Power Down Wakeup Enable Register
    #define WAK_GPIO0       (1U << 0)
    #define WAK_GPIO1       (1U << 1)
    #define WAK_GPIO2       (1U << 2)
    #define WAK_GPIO3       (1U << 3)
    #define WAK_GPIO4       (1U << 4)
    #define WAK_GPIO5       (1U << 5)
    #define WAK_GPIO6       (1U << 6)
    #define WAK_GPIO7       (1U << 7)
    #define WAK_IRRX        (1U << 8)
    #define WAK_VGA         (1U << 9)
    #define WAK_RTC         (1U << 15)    
    #define WAK_UART         (1U << 16)        
    #define WAK_CEC         (1U << 17)
    #define WAK_HDMI        (1U << 18)
    #define WAK_UNOR        (1U << 19)

    
    #define WAK_GPIOX(x)    (1U << (x))             // Set GPIO wakeup bit.
    #define GPIO_WAKEN(x)   (((x) & 0xff) << 0)     // GPIO wakeup enable bits
    #define WAK_ALLGPIO     GPIO_WAKEN(0xff)
    #define IRRX_WAKEN(x)   (((x) & 1) << 8)        // IRRX wakeup enable bits
    #define VGA_WAKEN(x)    (((x) & 1) << 9)        // VGA Sync wakeup enable bits
    #define RTC_WAKEN(x)    (((x) & 1) << 15)       // RTC wakeup enable bits
#define REG_RW_PDENTRY_CODE 0x0124      // Power Down Entry Code Register, 0x5381->0x1835 to power down
    #define PWRDWN_CODE1    0x14
    #define PWRDWN_CODE2    0x04
#define REG_RO_PDWAKE_STAT  0x0128      // Power Down Wakeup Status Register
    #define WAK_RS232       (1U << 16)
#define REG_WO_PDWST_CLR    0x012c      // Power Down Wakeup Status Clear Register
    #define IRRX_CLR_PDSTAT (1U << 0)   // Write 1 to clear PDSTAT register

// T8032 register
#define REG_RW_UPWAK	0x130
#define REG_RW_PD_PWM0 0x150
    #define SET_PWMRSN(x) (x << 24)
    #define SET_PWMH(x) ((x & 0xffU) << 16)    
    #define SET_PWMP(x) ((x & 0xffU) << 4)        
    #define SET_PWME(x) (1U << 4)            
    #define UNSET_PWME(x) (0U << 4)                
#define REG_RW_UP_ADDR 0x170
#define REG_RW_UP_DATA 0x174
#define REG_RW_UP_CFG 0x178
    #define UART_EN (1U << 24)
    #define FAST_EN (1U << 20)
    #define ENGINE_EN (1U << 16)
    #define CEC_EN (1U << 12)
    #define VIR_EN (1U << 8)    
    #define RAM_SPL_SEL (3U << 6)    
    #define XDATA_ACC (1U << 4)
    #define RAM_SP_SEL (3U << 2)          
    #define T8032_RST (1U)    
    #define SET_UART_EN(x) ((x & 1U) << 24)
    #define SET_FAST_EN(x) ((x & 1U) << 20)
    #define SET_ENGINE_EN(x) ((x & 1U) << 16)
    #define SET_CEC_EN(x) ((x & 1U) << 12)
    #define SET_VIR_EN(x) ((x & 1U) << 8)    
    #define SET_RAM_SPL_SEL(x) ((x & 3U) << 6)    
    #define SET_XDATA_ACC(x) ((x & 1U) << 4)
    #define SET_RAM_SP_SEL(x) ((x & 3U) << 2)          
    #define SET_T8032_RST(x) (x & 1U)    
// SIF slave
#define REG_RW_SINTEN 0x18c
    #define SET_SIFSR16_EN(x) ((x & 1U) << 5)
    #define SET_SIFSR8_EN(x) ((x & 1U) << 4)    
    #define SET_SIFSR_EN(x) ((x & 1U) << 3)    
    #define SET_SIFSW16_EN(x) ((x & 1U) << 2)    
    #define SET_SIFSW8_EN(x) ((x & 1U) << 1)        
    #define SET_SIFSW_EN(x) (x & 1U)            
#define REG_RW_SINTST 0x190
    #define SIFSR16_CLR (1U << 5)
    #define SIFSR8_CLR (1U << 4)    
    #define SIFSR_CLR (1U << 3)    
    #define SIFSW16_CLR (1U << 2)    
    #define SIFSW8_CLR (1U << 1)        
    #define SIFSW_CLR (1U)            
#define REG_RW_SINTCL 0x194
    #define SET_OUT_OP(x) ((x & 1U) << 9)
    #define SET_DEV_ADDR(x) ((x & 0x7fU) << 1)    
#define REG_RW_SSDEV 0x198
#define REG_RW_SSWORD 0x19c
#define REG_RW_SSWDATA0 0x1a0
#define REG_RW_SSWDATA1 0x1a4
#define REG_RW_SSWDATA2 0x1a8
#define REG_RW_SSWDATA3 0x1ac
#define REG_RW_SSRDATA0 0x1b0
#define REG_RW_SSRDATA1 0x1b4
#define REG_RW_SSRDATA2 0x1b8
#define REG_RW_SSRDATA3 0x1bc

// T8032 indirect I/F
#define REG_RW_ADDR 0x1e0
#define REG_RW_DATA 0x1e4
#define REG_RW_INT 0x1e8
    #define AUXINT_CLR  (1U << 2)
    #define AUXINT_ST  (1U << 2)        
    #define ARM_RW_R    (0U << 1)
    #define ARM_RW_W    (1U << 1)    
    #define ARM_INT     (1U << 0)
#define REG_RW_AUX_DATA 0x1f0
//#define REG_RO_AUX_INT 0x1f4      // Abandoned register

// IRRX readonly registers
#define IRRX_COUNT_HIGH_REG             0x0200
    #define IRRX_CH_BITCNT_MASK         0x3f
    #define IRRX_CH_BITCNT_BITSFT       0
    #define IRRX_CH_1ST_PULSE_MASK      0x0000ff00
    #define IRRX_CH_1ST_PULSE_BITSFT    8
    #define IRRX_CH_2ND_PULSE_MASK      0x00ff0000
    #define IRRX_CH_2ND_PULSE_BITSFT    16
    #define IRRX_CH_3RD_PULSE_MASK      0xff000000
    #define IRRX_CH_3RD_PULSE_BITSFT    24
#define IRRX_COUNT_MID_REG              0x0204
#define IRRX_COUNT_LOW_REG              0x0208

// IRRX read/write configuration registers
#define IRRX_CONFIG_HIGH_REG    0x020c
    #define IRRX_CH_HWIR        (1U << 0)
    #define IRRX_CH_IRI         (1U << 1)
    #define IRRX_CH_RC5         (1U << 2)
    #define IRRX_CH_RC5_1ST     (1U << 3)
    #define IRRX_CH_ORDINV      (1U << 4)
    #define IRRX_CH_IGSYN       (1U << 5)
    #define IRRX_CH_DISCL       (1U << 6)
    #define IRRX_CH_DISCH       (1U << 7)
    #define IRRX_CH_END_7       (0U << 8)
    #define IRRX_CH_END_15      (1U << 8)
    #define IRRX_CH_END_23      (2U << 8)
    #define IRRX_CH_END_31      (3U << 8)
    #define IRRX_CH_END_39      (4U << 8)
    #define IRRX_CH_END_47      (5U << 8)
    #define IRRX_CH_END_55      (6U << 8)
    #define IRRX_CH_END_63      (7U << 8)
    #define IRRX_CH_IGB0        (1U << 14)   // Ignore 0 bit count IR pulse to reduce noise
    #define IRRX_CH_DISPD       (1U << 15)   // IR state machine clear disable at deep power down
#define IRRX_CONFIG_LOW_REG     0x0210
#define IRRX_THRESHOLD_REG      0x0214
    #define IRRX_GD_DEL_MASK    0x0300
    #define IRRX_THRESHOLD_MASK 0x7f
#define IRRX_CLEAR_BUSY_REG     0x0218
    #define IRRX_CLR_BSY_IR     0x01


// IR Expect registers
#define IREXP_EN_REG            0x021c      // IR Expectation Enable Register
    #define IREXP_EN(x)         (1U << x)   // Enable IR Expect value register x
    #define IREXP_BIT_EN        (1U << 8)   // Bit count expectation enable bit
// 5381 New feature: H/W Power down
    #define IREXP_HWPD_EN       (1U << 9)
    #define IREXP_HWPD_EXPEN0   (1U << 10)
    #define IREXP_HWPD_EXPEN1   (1U << 11)

    
#define IREXP_M_BITMASK_REG     0x0220      // IR Expect Value Bit Mask Register
#define IREXP_L_BITMASK_REG     0x0224      // IR Expect Value Bit Mask Register
#define IREXP_M0_REG            0x0228      // IRM Expect Value Register 0
#define IREXP_L0_REG            0x022c      // IRL Expect Value Register 0
#define IREXP_M1_REG            0x0230      // IRM Expect Value Register 1
#define IREXP_L1_REG            0x0234      // IRL Expect Value Register 1
#define IREXP_M2_REG            0x0238      // IRM Expect Value Register 2
#define IREXP_L2_REG            0x023c      // IRL Expect Value Register 2
#define IREXP_M3_REG            0x0240      // IRM Expect Value Register 3
#define IREXP_L3_REG            0x0244      // IRL Expect Value Register 3
#define IREXP_M4_REG            0x0248      // IRM Expect Value Register 4
#define IREXP_L4_REG            0x024c      // IRL Expect Value Register 4
#define IREXP_M5_REG            0x0250      // IRM Expect Value Register 5
#define IREXP_L5_REG            0x0254      // IRL Expect Value Register 5
#define IREXP_M6_REG            0x0258      // IRM Expect Value Register 6
#define IREXP_L6_REG            0x025c      // IRL Expect Value Register 6
#define IREXP_M7_REG            0x0260      // IRM Expect Value Register 7
#define IREXP_L7_REG            0x0264      // IRL Expect Value Register 7
#define IREXP_BITNUM_REG        0x0268      // BITCNT Expected Value Register
#define MAX_EXP_KEY_NUM         (8)
// 5381 New feature: H/W Power down
#define IREXP_M8_REG            0x0270      // IRM Expect Value Register 6
#define IREXP_L8_REG            0x0274      // IRL Expect Value Register 6
#define IREXP_M9_REG            0x0278      // IRM Expect Value Register 7
#define IREXP_L9_REG            0x027c      // IRL Expect Value Register 7
#define IROS_DATA0              0x0290
#define IROS_DATA1              0x0294
#define IROS_DATA2              0x0298
#define IROS_DATA3              0x029c

// VGA Wakeup Control/Status
#define REG_RW_VGA_WAKEUP_CTRL0     0x0400
    #define AUTO_SWITCH_EN          (1U << 27)
    #define C_DGLITCH_EN            (1U << 26)
    #define V_DGLITCH_EN            (1U << 25)
    #define H_DGLITCH_EN            (1U << 24)
    #define HRANGE_L_MASK           (0xfffU << 12)
    #define SET_HRANGE_L(x)         (((x) & 0xfffU) << 12)
    #define GET_HRANGE_L(x)         (((x) >> 12) & 0xfffU)
    #define HRANGE_U_MASK           (0xfffU << 0)
    #define SET_HRANGE_U(x)         (((x) & 0xfffU) << 0)
    #define GET_HRANGE_U(x)         (((x) >> 0) & 0xfffU)
#define REG_RW_VGA_WAKEUP_CTRL1     0x0404
    #define PDN_USE_HLEN_TH         (1U << 26)
    #define VRANGE_L_MASK           (0xfffU << 12)
    #define SET_VRANGE_L(x)         (((x) & 0xfffU) << 12)
    #define GET_VRANGE_L(x)         (((x) >> 12) & 0xfffU)
    #define VRANGE_U_MASK           (0xfffU << 0)
    #define SET_VRANGE_U(x)         (((x) & 0xfffU) << 0)
    #define GET_VRANGE_U(x)         (((x) >> 0) & 0xfffU)
#define REG_RW_VGA_WAKEUP_CTRL2     0x0408
    #define CRANGE_L_MASK           (0xfffU << 12)
    #define SET_CRANGE_L(x)         (((x) & 0xfffU) << 12)
    #define GET_CRANGE_L(x)         (((x) >> 12) & 0xfffU)
    #define CRANGE_U_MASK           (0xfffU << 0)
    #define SET_CRANGE_U(x)         (((x) & 0xfffU) << 0)
    #define GET_CRANGE_U(x)         (((x) >> 0) & 0xfffU)
#define REG_RW_VGA_WAKEUP_CTRL3     0x040c
    #define SET_H_STABLE_TH(x)	(((x) & 0xfffU) << 0)    
    #define SET_V_STABLE_TH(x)	(((x) & 0xfffU) << 12)
    #define SET_H_DIFF_TH(x)		(((x) & 0x3fU) << 24)
#define REG_RW_VGA_WAKEUP_CTRL4     0x0410
    #define SET_C_STABLE_TH(x)	(((x) & 0xfffU) << 0)
    #define SET_VACT_MP_TH(x)	(((x) & 0xfffU) << 12)
    #define SET_V_DIFF_TH(x)		(((x) & 0x3fU) << 24)
#define REG_RW_VGA_WAKEUP_CTRL5     0x0414
    #define CSYNC_CONT_THUL         (0xfffffU << 0)
#define REG_RW_VGA_WAKEUP_CTRL6     0x0418
#define REG_RW_VGA_WAKEUP_CTRL7     0x041c
    #define CSYNC_CONT_THH_7_0      (0xffU << 24)
#define REG_RW_VGA_WAKEUP_CTRL8     0x0420
    #define SET_H_IIR_INIT(x)		(((x) & 0xfffU) << 0)    
    #define SET_V_IIR_INIT(x)		(((x) & 0xfffU) << 12)
    #define CSYNC_CONT_THH_9_8      (3U << 24)
#define REG_RO_VGA_WAKEUP_STATUS0   0x0430
#define REG_RO_VGA_WAKEUP_STATUS1   0x0434
#define REG_RO_VGA_WAKEUP_STATUS2   0x0438

//----------------------------------------------------------------------------
// RTC
#define REG_RW_RTC_YEAR     0x0700      // RTC Year Register
#define REG_RW_RTC_MONTH    0x0704      // RTC Month Register
#define REG_RW_RTC_DAY      0x0708      // RTC Day Register
#define REG_RW_RTC_WEEK     0x070c      // RTC Week Register
#define REG_RW_RTC_HOUR     0x0710      // RTC Hour Register
#define REG_RW_RTC_MIN      0x0714      // RTC Minute Register
#define REG_RW_RTC_SEC      0x0718      // RTC Second Register
#define REG_RW_RTC_AYEAR    0x071c      // RTC Alarm Year Register
#define REG_RW_RTC_AMONTH   0x0720      // RTC Alarm Month Register
#define REG_RW_RTC_ADAY     0x0724      // RTC Alarm Day Register
#define REG_RW_RTC_AWEEK    0x0728      // RTC Alarm Week Register
#define REG_RW_RTC_AHOUR    0x072c      // RTC Alarm Hour Register
#define REG_RW_RTC_AMIN     0x0730      // RTC Alarm Minute Register
#define REG_RW_RTC_ASEC     0x0734      // RTC Alarm Second Register
#define REG_RW_RTC_ALMINT   0x0738      // RTC Alarm Interrupt Enable Register
    #define RTCALM_FULL_EN  (0xffU << 0)
    #define RTCALM_YEAR_EN  (1U << 7)   // Alarm compare YEAR enable
    #define RTCALM_MONTH_EN (1U << 6)   // Alarm compare MONTH enable
    #define RTCALM_DAY_EN   (1U << 5)   // Alarm compare DAY enable
    #define RTCALM_WEEK_EN  (1U << 4)   // Alarm compare WEEK enable
    #define RTCALM_HOUR_EN  (1U << 3)   // Alarm compare HOUR enable
    #define RTCALM_MINU_EN  (1U << 2)   // Alarm compare MINU enable
    #define RTCALM_SEC_EN   (1U << 1)   // Alarm compare SEC enable
    #define RTCALM_INTR_EN  (1U << 0)   // Alarm interrupt enable
#define REG_RW_RTC_CTRL     0x073c      // RTC Control Register
    #define RTCTRL_STOP     (1U << 0)   // Stop RTC, to save power
    #define RTCTRL_H24      (1U << 1)   // 1 for 24-hour clock mode
    #define RTCTRL_SIM      (1U << 4)   // Pass write protection
    #define RTCTRL_PASS     (1U << 8)   // Pass write protection
#define REG_RW_RTC_KEY      0x0740      // RTC Key Register - NOT USED
#define REG_RW_RTC_LEARYEAR 0x0744      // RTC Leap Year    - NOT USED
#define REG_RW_RTC_PROT     0x0748      // RTC Write Protection Register
    #define RTC_WRITE_PROTECT_CODE          0x5381

// MT5391 start
#define REG_RW_RS232_MODE   0x0818
    #define PDGPIO_AS_UART  (1U << 2)
// MT5391 end

//----------------------------------------------------------------------------
// 8297 Servo ADC Interface Configuration Register 
#define REGSIF_RW_SRVCFG0 0xc0
    #define HWEN_SRVCFG         (1U << 0)   // Hardware audo trigger Servo ADC enable bit
    #define SWEN_SRVCFG         (1U << 1)   // Software trigger Servo ADC enable bit
    #define CHNNLEN_SRVCFG(x)   ((UINT32)x)    // Servo ADC channel enable bits
    #define CHNNLSEL_SRVCFG(x)  ((UINT32)x << 5)   // Servo ADC channel select.
#define REGSIF_RW_SRVCFG1 0xc1
    #define SRVCH_EN_SRVCFG     (0xffU)    // Servo ADC channel enable bits.
    #define SRVCH_EN_CH(x)      ((1 << x) & SRVCH_EN_SRVCFG)
#define REGSIF_RW_SRVSWT 0xc2
#define REGSIF_RW_SRVCLR 0xc3
    #define ADCOUT_CLR          (1U << 1)   // Clear ADC output data and channel status
#define REGSIF_RW_SRV_RAT0 0xc4
#define REGSIF_RW_SRV_RAT1 0xc5
#define REGSIF_RW_SRVTOT 0xc6
#define REGSIF_RW_SRVTOTEN 0xc7
#define REGSIF_RW_SRVDAC 0xc8
#define REGSIF_RW_ADCTOP0 0xc9
    #define CHBTM_MASK          (0xffU)    // bottom voltage value
    #define CHBTM_VAL(x)        ((x) & CHBTM_MASK)
    #define GET_CHBTM(x)        ((x) & 0xffU)
#define REGSIF_RW_ADCBOT0 0xca
    #define CHTOP_MASK          (0xffU)    // top voltage value
    #define CHTOP_VAL(x)        ((x) & CHTOP_MASK)
    #define GET_CHTOP(x)        ((x) & 0xffU)
#define REGSIF_RW_WAKCFG0 0xcb    
    #define CH_MASK             (0x7U)   // interrupt or wakeup mask
    #define CH_WAK_TOP_EN       (1U << 2)
    #define CH_WAK_MID_EN       (1U << 1)
    #define CH_WAK_BTM_EN       (1U << 0)
#define REGSIF_RW_ADCTOP1 0xcc
#define REGSIF_RW_ADCBOT1 0xcd
#define REGSIF_RW_WAKCFG1 0xce
#define REGSIF_RW_ADCTOP2 0xcf
#define REGSIF_RW_ADCBOT2 0xd0
#define REGSIF_RW_WAKCFG2 0xd1
#define REGSIF_RW_ADCTOP3 0xd2
#define REGSIF_RW_ADCBOT3 0xd3
#define REGSIF_RW_WAKCFG3 0xd4
#define REGSIF_RW_ADCTOP4 0xd5
#define REGSIF_RW_ADCBOT4 0xd6
#define REGSIF_RW_WAKCFG4 0xd7
#define REGSIF_RW_ADCTOP5 0xd8
#define REGSIF_RW_ADCBOT5 0xd9
#define REGSIF_RW_WAKCFG5 0xda
#define REGSIF_RW_ADCTOP6 0xdb
#define REGSIF_RW_ADCBOT6 0xdc
#define REGSIF_RW_WAKCFG6 0xdd
#define REGSIF_RW_SRVCST 0xde
#define REGSIF_RW_ADOUT0 0xdf
#define REGSIF_RW_ADOUT1 0xe0
#define REGSIF_RW_ADOUT2 0xe1
#define REGSIF_RW_ADOUT3 0xe2
#define REGSIF_RW_ADOUT4 0xe3
#define REGSIF_RW_ADOUT5 0xe4
#define REGSIF_RW_ADOUT6 0xe5
#define REGSIF_RW_ADCFG0 0xe7
#define REGSIF_RW_ADCFG1 0xe8
#define REGSIF_RW_ADPADG 0xe9
    #define SRVIO_PD        (0x1f)
    #define SRVIO_E2        (1U << 5)
    #define SRVIO_E4        (1U << 5)
    #define SRVIO_E8        (1U << 7)
    #define SRVIO_PU        (0x1fU << 8)
    #define SRVIO_SMT       (1U << 13)
    #define SRVIO_SR        (1U << 14)
    #define SRVIO_GP_ALL    (0x7fU)       // Servo ADC PAD used to be GPIO
    #define SRVIO_GP(x)     (1U << (x))  // Servo ADC PAD used to be GPIO    
    #define SRVIO_GP_O(x)     (1U << (x))  // Servo ADC PAD used to be GPIO                  
    #define SRVAD_VREF      (0xfU << 24)        // Servo ADC Voltage reference configuration
    #define SRVAD_BYP       (1U << 28)  // SErvo ADC Bypass
    #define SRVAD_HQB       (1U << 29)  // Servo ADC HQB
    #define SRVAD_PDN       (1U << 31)  // Servo ADC Power Down bit
#define REGSIF_RW_ADPADI 0xea
#define REGSIF_RW_ADPADO 0xeb
#define REGSIF_RW_ADPADE 0xec
#define REGSIF_RW_ADPADSR 0xed
#define REGSIF_RW_STBLDO 0xee
#define REGSIF_RW_ACLDO 0xef
#define REGSIF_RW_CKGEN 0xf0
    #define PDCK_SEL_XTAL_D21 0x0
    #define PDCK_SEL_XTAL_D20 0x1    
    #define PDCK_SEL_XTAL_D18 0x2        
    #define PDCK_SEL_XTAL_D16 0x3        
    #define PDCK_SEL_XTAL_D9 0x4            
#define REGSIF_RW_MISC 0xf1
#define REGSIF_RW_SVOWAKEN 0xf2
    #define WAK_SRVAD0      (1U)
    #define WAK_SRVAD1      (1U << 1)
    #define WAK_SRVAD2      (1U << 2)
    #define WAK_SRVAD3      (1U << 3)
    #define WAK_SRVAD4      (1U << 4)
    #define WAK_SRVAD5      (1U << 5)
    #define WAK_SRVAD6      (1U << 6)
    #define WAK_SRVADX(x)   (1U << (x))             // Set ServoADC wakeup bit.
    #define SRVAD_WAKEN(x)  ((x) & 0xffu)    // Servo ADC wakeup enable bits
    #define WAK_ALLSRVAD    SRVAD_WAKEN(0xffu)    
#define REGSIF_RO_PDSTAT 0xf3
#define REGSIF_RW_PDSTCL 0xf4
#define REGSIF_RW_CORECNT0 0xf5
#define REGSIF_RW_CORECNT1 0xf6
#define REGSIF_RW_CORECNT2 0xf7
#define REGSIF_RW_CORECNT3 0xf8
#define REGSIF_RW_PADWAKCFG 0xf9
#define REGSIF_RW_PADCFG0 0xfa
#define REGSIF_RW_2NDADDR 0xfb
#define REGSIF_RW_2NDDATA 0xfc 
#define REGSIF_RW_XTAL0 0xfd 
#define REGSIF_RW_XTAL1 0xfe
#define REGSIF_RW_XTAL2 0xff

// Error code
#define ERR_NO_ERROR                0
#define ERR_DC_PROTECT              1
#define ERR_POWER_DOWN              2
#define ERR_I2C_STANDBY_CTRL        3
#define ERR_I2C_GENERAL             4
#define ERR_I2C_MT8280              5
#define ERR_I2C_SYS_EEPROM          6
#define ERR_I2C_TUNER               7
#define ERR_I2C_DEMOD               8   // TDA9886
#define ERR_I2C_FPGA                9
#define ERR_I2C_HDMI_SWITCH         13
#define ERR_DRAM_MT8280_1           14
#define ERR_DRAM_MT8280_2           15
#define ERR_I2C_HDMI_EDID           16
#define ERR_I2C_DFI_BOLT_ON         17
#define ERR_I2C_MT5112              18
#define ERR_I2C_PROLDIOM            19
#define ERR_I2C_BDS_BOLT_ON         21

// Power down command with T8032
#define PDWNC_CMD_ARMT8032_UNKNOWN 0
#define PDWNC_CMD_ARMT8032_ENTER_STANDBY 1
#define PDWNC_CMD_ARMT8032_ENTER_NORMAL 2
#define PDWNC_CMD_ARMT8032_QUERY_SW_VERSION 3
#define PDWNC_CMD_ARMT8032_XDATA_SWITCH 4
#define PDWNC_CMD_ARMT8032_READ_WAKEUP_REASON 5

#define PDWNC_CMD_T8032ARM_ENTER_NORMAL_READY 1
#define PDWNC_CMD_T8032ARM_RESPONSE_SW_VERSION 2
#define PDWNC_CMD_T8032ARM_RESPONSE_XDATA_SWITCH 3

// Power down event
typedef enum
{
    PDWNC_POWER_DOWN_UNKNOWN = 0, 
    PDWNC_POWER_DOWN_VGA,
    PDWNC_POWER_DOWN_RTC,
    PDWNC_POWER_DOWN_KEYPAD,
    PDWNC_POWER_DOWN_IRRC,
    PDWNC_POWER_DOWN_AC_POWER = 6
} PDWNC_POWER_DOWN_REASON_T;

typedef enum
{
    PDWNC_WAKE_UP_REASON_UNKNOWN = 0,
    PDWNC_WAKE_UP_REASON_VGA,
    PDWNC_WAKE_UP_REASON_RTC,
    PDWNC_WAKE_UP_REASON_KEYPAD,
    PDWNC_WAKE_UP_REASON_IRRC,
    PDWNC_WAKE_UP_REASON_UART,
    PDWNC_WAKE_UP_REASON_AC_POWER,
    PDWNC_WAKE_UP_REASON_HDMI,
    PDWNC_WAKE_UP_REASON_UART_NORMAL,
    PDWNC_WAKE_UP_REASON_WATCHDOG,
}   PDWNC_WAKE_UP_REASON_T;

typedef enum
{
    PDWNC_NORMAL = 0,
    PDWNC_WAIT_CEC_TRANSACTION,
    PDWNC_CEC_TRANSACTION_FINISH
}   PDWNC_CEC_HANDSHAKE_T;


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

// PDWNC functions.
EXTERN INT32 PDWNC_Init(void);
EXTERN void PDWNC_Reboot(void);
EXTERN void PDWNC_WatchDogReboot(void);
EXTERN void PDWNC_GpioPowerDown(UINT32 fgVga);
EXTERN void PDWNC_IrLedCheck(INT32 fgOn);
EXTERN UINT32 PDWNC_EnterPowerDown(UINT32 u4PowerDownEn, UINT32 u4PowerDownEvent);
EXTERN UINT32 PDWNC_ReadServoADCChannelValue(UINT32 u4Channel);
EXTERN UINT32 PDWNC_IsWakeupByPowerButton(void);
EXTERN UINT32 PDWNC_IsBootByWakeup(void);
EXTERN UINT32 PDWNC_PowerDownByAcCut(void);
EXTERN void PDWNC_CreateWatchDogThread(void);
EXTERN UINT32 PDWNC_ReadWakeupReason(void);
EXTERN UINT32 PDWNC_SetupPseudoWatchdogAssert(UINT32 *pu4Flag);

// Interrupt functions.
EXTERN BOOL PDWNC_ClearInt(UINT16 u2IntNo);
#ifndef CC_UBOOT
EXTERN INT32 PDWNC_RegIsr(UINT16 u2IntNo, x_os_isr_fct pfnIsr,
                            x_os_isr_fct *ppfnOld);
#endif /* CC_UBOOT */

// GPIO functions.
EXTERN INT32 PDWNC_GpioEnable(INT32 i4Gpio, const INT32 *pfgSet);
EXTERN INT32 PDWNC_GpioOutput(INT32 i4Gpio, const INT32 *pfgSet);
EXTERN INT32 PDWNC_GpioInput(INT32 i4Gpio);
EXTERN INT32 PDWNC_GpioIntrq(INT32 i4Gpio, const INT32 *pfgSet);
EXTERN INT32 PDWNC_GpioQuery(INT32 i4Gpio, INT32 *pi4Intr,
                            INT32 *pi4Fall, INT32 *pi4Rise);
EXTERN INT32 PDWNC_GpioReg(INT32 i4Gpio, GPIO_TYPE eType,
                            PFN_GPIO_CALLBACK pfnCallback);

// Servo used as GPIO
EXTERN INT32 PDWNC_ServoGpioEnable(INT32 i4Gpio, const INT32 *pfgSet);
EXTERN INT32 PDWNC_ServoGpioOutput(INT32 i4Gpio, const INT32 *pfgSet);
EXTERN INT32 PDWNC_ServoGpioInput(INT32 i4Gpio);
EXTERN INT32 PDWNC_ServoGpioIntrq(INT32 i4Gpio, const INT32 *pfgSet);
EXTERN INT32 PDWNC_ServoGpioReg(INT32 i4Gpio, GPIO_TYPE eType,
                            PFN_GPIO_CALLBACK pfnCallback);

#ifdef CC_WELTREND_UP_SUPPORT

EXTERN UINT8 PDWNC_WeltrendWrite(UINT8 u1RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount);
EXTERN UINT8 PDWNC_WeltrendRead(UINT8 u1RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount);

#endif /* CC_WELTREND_UP_SUPPORT */

EXTERN INT32 PDWNC_WriteErrorCode(UINT8 u1ErrorCode);
EXTERN void PDWNC_WeltrendSystemRecovery(void);
EXTERN void PDWNC_WeltrendCheckBufClear(void);

EXTERN void PDWNC_SetPowerDownFlag(UINT8 u1Flag);
EXTERN UINT8 PDWNC_GetPowerDownFlag(void);

#endif /* X_PDWNC_H */

