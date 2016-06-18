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

#ifndef X_CKGEN_H
#define X_CKGEN_H

#include "x_hal_5381.h"


#define MT537X_A        0           // after ECO, it should be 0.
#define CKGENHW_DEBUG   0

//===========================================================================
// Macros for register read/write

#define CKGEN_READ8(offset)			    IO_READ8(CKGEN_BASE, (offset))
#define CKGEN_READ16(offset)			IO_READ16(CKGEN_BASE, (offset))
// #define CKGEN_READ32(offset)			IO_READ32(CKGEN_BASE, (offset))

#define CKGEN_WRITE8(offset, value)	    IO_WRITE8(CKGEN_BASE, (offset), (value))
#define CKGEN_WRITE16(offset, value)	IO_WRITE16(CKGEN_BASE, (offset), (value))
// #define CKGEN_WRITE32(offset, value)	IO_WRITE32(CKGEN_BASE, (offset), (value))

#define CKGEN_REG8(offset)			    IO_REG8(CKGEN_BASE, (offset))
#define CKGEN_REG16(offset)			    IO_REG16(CKGEN_BASE, (offset))
#define CKGEN_REG32(offset)			    IO_REG32(CKGEN_BASE, (offset))


//=====================================================================
// Register definitions
#define REG_APLL_CFG0           0x0100      // Audio PLL 1 Configuration 0
    #define APLL1_MODIN         (0x1ffffffU << 0)   // APLL1 modulator input
    #define APLL1_BIASOPTEN     (1U << 26)  // APLL1 bandgap external enable
    #define APLL1_RESET_B_CLKDIV    (1U << 27)  // APLL1 output clkdiv reset
    #define APLL1_SCFDIV        (0x0fU << 28)   // APLL1 scf divider select
#define REG_APLL_CFG1           0x0104      // Audio PLL 1 Configuration 1
    #define APLL1_ENABLE        (1U << 0)   // APLL1 enable
    #define APLL1_CAL_EN        (1U << 1)   // APLL1 calibration enable
    #define APLL1_TEST_EN       (1U << 2)   // APLL1 test enable
    #define APLL1_LPF_R_B       (1U << 3)   // APLL1 lpf resistor value
    #define APLL1_V2I_RB        (3U << 4)   // APLL1 v2i resistor value
    #define APLL1_VCOG_B        (7U << 8)   // APLL1 vco gain control
    #define APLL1_VCOG_B_SHIFT  (8)
    #define APLL1_CHP_B         (0xfU << 12)    // APLL1 charge pump current select
    #define APLL1_VCO_BAND      (7U << 16)  // APLL1 vco band select
    #define APLL1_VCO_BAND_SHIFT    (16)
    #define APLL1_PREDIV        (7U << 20)  // APLL1 predivider code select
    #define APLL1_PREDIV_SHIFT  (20)
    #define APLL1_VCTR_SEL      (1U << 24)  // APLL1 output control voltage select
    #define APLL1_PREDIVEN      (1U << 25)  // APLL1 predivider enable
    #define APLL1_FREFSEL       (1U << 26)  // APLL1 reference clk select, 0: from ACLK, 1: from SYSPLL
    #define APLL1_RESET_B_CORE  (1U << 27)  // APLL1 modulator and post divider reset
    #define APLL1_DELAY         (1U << 28)  // APLL1 post divider delay
    #define APLL1_MUTE          (1U << 29)  // APLL1 modulator and post divider mute
    #define APLL1_POSDIVSEL     (1U << 30)  // APLL1 post divider select
    #define APLL1_TCLKSEL       (1U << 31)  // APLL1 ADAC lpf clk select
#define REG_APLL_CFG2           0x0108      // Audio PLL 1 Configuration 2
    #define APLL1_K1            (1U << 0)   // APLL1 K1 select, clk_k1 is APLL_CK/2(0), APLL_CK/3(1)
    #define APLL1_K2            (0x3fU << 8)    // APLL1 K2 select, clk_k2 is clk_k1/apll_k2.
    #define APLL1_BP            (1U << 30)  // APLL1 Bypass, 0:normal, 1:bypass(from pin)
    #define APLL1_DIV_RST       (1U << 31)  // APLL1 Divider counter reset, 0:normal, 1:reset
#define REG_APLL_CFG3           0x010c      // Audio PLL 1 Configuration 3
    #define APLL1_LPFDIV        (0xffU << 0)    // APLL1 lpf clk select
    #define APLL1_TEST_B        (7U << 8)   // APLL1 test select
    #define APPL1_TESTCLK_EN    (1U << 12)  // APPL1 test mode enable
    #define APPL1_REV           (0xffU << 16)   // APPL1 reserved register
    #define APPL1_MODDIV        (0xffU << 24)   // APPL1 post divider code select
#define REG_PSPLL_CFG0          0x0130      // PSCAN PLL Configuration 0
    #define CAL_EN_PS           (1U << 0)
    #define RESET_PS            (1U << 3)
    #define TEST_JITTER_PS      (1U << 4)
    #define TEST_EN_PS          (1U << 5)
    #define PD_PSPLL            (1U << 6)
    #define VCON_SEL_PS         (1U << 7)
    #define DIV_SEL_PS          (3U << 8)
    #define DIV_SEL_PS_SHIFT    (8)
    #define PSPLL_DIV_1         2
    #define PSPLL_DIV_2         1
    #define PSPLL_DIV_4         0
    #define CKSEL_0_PS          (1U << 12)
    #define CKSEL_1_PS          (1U << 13)
    #define VCO_GAIN_PS         (7U << 16)
    #define VCO_GAIN_PS_SHIFT   (16)
    #define V2I_RB_PS           (3U << 28)
    #define V2I_RB_PS_SHIFT     (28)
#define REG_PSPLL_CFG1          0x0134      // PSCAN PLL Configuration 1
    #define CHP_PS              (0xfU << 4)
    #define CHP_PS_SHIFT        (4)
    #define VCO_BAND_PS         (7U << 8)
    #define VCO_BAND_PS_SHIFT   (8)
    #define PS_REV              (0xffU << 24)
    #define PS_REV_SHIFT        (24)
#define REG_PSPLL_CFG2          0x0138      // PSCAN PLL Configuration 2
    #define PSPLL_BP            (1U << 30)
    #define PSPLL_DIV_RST       (1U << 31)
#define REG_APLL2_CFG0          0x01a0      // Audio PLL 2 Configuration 0
    #define APLL2_MODIN         (0x1ffffffU)
    #define APLL2_BIASOPTEN     (1U << 26)
    #define APLL2_RESET_B_CLKDIV    (1U << 27)
    #define APLL2_SCFDIV        (0xfU << 28)
#define REG_APLL2_CFG1          0x01a4      // Audio PLL 2 Configuration 1
    #define APLL2_ENABLE        (1U << 0)
    #define APLL2_CAL_EN        (1U << 1)
    #define APLL2_TEST_EN       (1U << 2)
    #define APLL2_LPF_R_B       (1U << 3)
    #define APLL2_V2I_RB        (3U << 4)
    #define APLL2_VCOG_B        (7U << 8)
    #define APLL2_VCOG_B_SHIFT  (8)
    #define APLL2_CHP_B         (0xfU << 12)
    #define APLL2_VCO_BAND      (7U << 16)
    #define APLL2_VCO_BAND_SHIFT    (16)
    #define APLL2_PREDIV        (7U << 20)
    #define APLL2_PREDIV_SHIFT  (20)
    #define APLL2_VCTR_SEL      (1U << 24)
    #define APLL2_PREDIVEN      (1U << 25)
    #define APLL2_FREFSEL       (1U << 26)  // APLL2 reference clk select, 0:from ACLK, 1:from SYSPLL
    #define APLL2_RESET_B_CORE  (1U << 27)
    #define APLL2_DELAY         (1U << 28)
    #define APLL2_MUTE          (1U << 29)
    #define APLL2_POSDIVSEL     (1U << 30)
    #define APLL2_TCLKSEL       (1U << 31)
#define REG_APLL2_CFG2          0x01a8      // Audio PLL 2 Configuration 2
    #define APLL2_K1            (1U << 0)
    #define APLL2_K2            (0x3fU << 8)
    #define APLL2_BP            (1U << 30)
    #define APLL2_DIV_RST       (1U << 31)
#define REG_APLL2_CFG3          0x01ac      // Audio PLL 2 Configuration 3
    #define APLL2_LPFDIV        (0xffU << 0)
    #define APLL2_TEST_B        (7U << 8)
    #define APLL2_TESTCLK_EN    (1U << 12)
    #define APLL2_REV           (0xffU << 16)
    #define APLL2_MODDIV        (0xffU << 24)
#define REG_DSSTOP_CFG0         0x01b0      // DDSTOP Configuration 0
    #define MON_JIT             (7U << 0)   // Jitter test monitor control
    #define DDS_FBK_SEL         (3U << 4)   // DDS feedback singla select
    #define DDS_HSYNC_SEL       (1U << 8)   // DDS hsync input polarity control
    #define DDS_HSYNC_POL       (1U << 9)   // DDS hsync input polarity cnotrol
    #define RSYNC_POL           (1U << 10)  // Pix_clock sync polarity control
    #define SSDDS_PSENB         (1U << 11)  // To_lvds signal output select
    #define ADCPLL_IN_SEL       (1U << 12)  // ADCPLL input reference from 0:27M or 1:SDDS
    #define CVBS_CLK_EN         (1U << 13)  // CVS clock enable 0:enable, 1:disable
    #define MON_JIG_EN          (1U << 14)  // Jitter test monitor circuit enable
    #define RESET_NCO           (1U << 15)  // Reset of NCO in APLL 0:reset, 1:normal
    #define RESET_DDDS_VPLL     (1U << 16)  // Reset of digital in VPLL 0:reset, 1:normal
    #define RESET_DDS           (1U << 17)  // Reset of digital in DDS 0:reset, 1:normal
    #define RESET_CVBS          (1U << 18)  // Reset CVS ADC clock 0:reset, 1:normal
    #define PD_BIAS_DDS         (1U << 19)  // Power down of bias1 0:power down, 1:normal.
    #define AUADC_CLK_EN        (1U << 20)  // Enable Audio ADC clock 0:reset, 1:normal
    #define VPLL_IN_DIV         (1U << 21)  // VPLL input frequency/2
    #define ADCPLL_IN_DIV       (1U << 22)  // ADCPLL input frequency/2
    #define ADCCLK_SEL          (1U << 23)  // All ADC normal input & xta
    #define CVBS_DIV            (3U << 24)  // CVS clock divider
    #define IBEN0               (0xfU << 28)    // Bias current control
    #define SYS_APLL_SEL        (1U << 28)  // APLL reference clk, 0 from 27M, 1 from syspll.
    #define SYS_APLL2_SEL       (1U << 29)  // APLL2 reference clk, 0 from 27M, 1 from syspll.
    #define SYS_AUADCCK_SEL0    (1U << 30)  // AUADCCK, 0 from 27M , 1 from 54M
    #define SYS_AUADCCK_SEL1    (1U << 31)  // AUADCCK, 0 from TO_DIGITAL_AUDIO, 1 from syspll.
#define REG_PLL_CALIB           0x01c0      // PLL Calibration
    #define PLL_CAL_CNT         (0xfffU)    // Calibration counter
    #define PLL_CAL_TRIGGER     (1U << 15)  // Calibration trigger
    #define PLL_CAL_SELECT      (0x1fU << 16)   // Selection of Clock for Calibration
    #define PLL_CAL_SEL(x)      ((((UINT32)x) << 16) & PLL_CAL_SELECT)
#define REG_TEST_MODE	0x01c4	// 
    #define C_VBI_VGA_CK_DIV	(1U << 27)	// divide frequency

#define REG_CPU_CKCFG           0x0200      // CPU_CK Configuration

#define REG_VPCLK_STOP          0x029c      // VPCLK_STOP Configuration
    #define TVD3D_CLK_STOP      (1U << 0)   // TVD3D_CLK stop, 0:Normal clock, 1:clock stops
    #define TVD_CLK54_CLK_STOP  (1U << 1)   // TVD_CK54_CLK stop, 0:Normal clock, 1:clock stops
    #define HDTV_CLK_STOP       (1U << 2)   // HDTV_CLK stop, 0:Normal clock, 1:clock stops
    #define VGA_CLK_STOP        (1U << 3)   // VGA_CLK stop, 0:Normal clock, 1:clock stops
    #define DVI_CLK_STOP        (1U << 4)   // DVI_CLK stop, 0:Normal clock, 1:clock stops
    #define CCH_CLK_STOP        (1U << 5)   // CCH_CLK stop, 0:Normal clock, 1:clock stops
    #define VBI_CLK_STOP        (1U << 6)   // VBI_CLK stop, 0:Normal clock, 1:clock stops
    #define VBI2_CLK_STOP       (1U << 7)   // VBI2_CLK stop, 0:Normal clock, 1:clock stops
    #define MAIN_CLK_STOP       (1U << 8)   // MAIN_CLK stop, 0:Normal clock, 1:clock stops
    #define PIP_CLK_STOP        (1U << 9)   // PIP_CLK stop, 0:Normal clock, 1:clock stops
    #define PSW_HSD_CLK_STOP    (1U << 11)  // PSW_HSD_CLK stop, 0:Normal clock, 1:clock stops
    #define PSW_SD_CLK_STOP     (1U << 12)  // PSW_SD_CLK stop, 0:Normal clock, 1:clock stops
#define REG_VPCLK_CFG           0x02a0      // VPCLK_CFG Configuration
    #define MAINX2_SEL          (3U << 0)   // MAIN_CLK_PSCAN(CLK2X_MANI_PSCAN) selection
    #define HDTV_CLK_SEL        (1U << 4)   // HDTV_CLK selection
    #define PIX_CLK_SEL         (1U << 5)   // PIX_CLK selection
    #define PIX_CK208_SEL       (1U << 6)   // PIX_CK208 selection
    #define OCLK2_SEL           (3U << 8)   // OCLK2 selection
    #define OCLK_SEL            (3U << 12)  // OCLK selection
    #define CCD_REV             (1U << 16)  // LLC_CLK inversion selection
    #define TVD_CK54_SEL        (1U << 20)  // TVD_CK54 selection
    #define TVD_CLK_SEL         (3U << 24)  // TVD_CLK selection
    #define LVDS_DPIX_SEL       (1U << 28)  // lvds_dpix_clk_post selection
#define REG_RGBCLK_CFG          0x02a4      // RGBCLK_CFG Configuration
    #define RGB_DELAY_SET       (0x7fU)     // RGB_CLK_DELAY delay setting
    #define RGB_CLK_SEL         (3U << 8)   // RGB_CLK selection
    #define RGB_INV_SEL         (1U << 12)  // RGB_CLK inversion selection
    #define RBG_PS_SEL          (1U << 14)  // RGB_PS_CLK selection
    #define RBG_DUTY_SET        (0x7fU << 16)   // RGB_CLKX2 duty cycle setting
    #define RGB_DVI2X_SEL       (1U << 24)  // PRE_CLK_2XMAIN selection
    #define PRE_RGB_CLK_SEL     (1U << 28)  // PRE_RGB_CLK selection
#define REG_PSWCLK_CFG          0x02a8      // PSWCLK_CFG Configuration
    #define PSW_HSD_CLK_SEL     (3U << 0)   // PSW_HSD_CLK selection
    #define PSW_SD_CLK_SEL      (1U << 4)   // PSW_SD_CLK selection
    #define PIP_OCLK_SEL        (1U << 8)   // OCLK for PIP_CLK
    #define PRE_LLC_CLK_SEL     (1U << 12)  // PRE_LLC_CLK selection
#define REG_OCLK_TEST           0x02ac      // OCLK_TEST Configuration
    #define OCLK_TST_SEL        (3U << 0)   // Test clock for OCLK selection
    #define OCLK2_TST_SEL       (3U << 4)   // Test clock for OCLK2 selection
    #define DVICK_IN_SEL        (3U << 8)   // DVICK_IN selection
    #define VID_RESV            (0xffU << 16)   // Reserved control bit for VID input
#define REG_AUD_CKCFG           0x02b0      // AUDIO Clock Configuration
    #define AIN_CK_SEL          (1U << 0)   // AIN_CK selection
    #define LPF_TCLK_SEL        (3U << 4)   // Test clock for LPF_CLK selection
    #define MOD_TCLK_SEL        (3U << 8)   // Test clock for MOD_CLK selection
    #define SCF_TCLK_SEL        (3U << 12)  // Test clock for SCF_CLK selection

#define REG_PINMUX_SEL0         0x0400      // Pin function multiplexer selection 0
#define REG_PINMUX_SEL1         0x0404      // Pin function multiplexer selection 1
#define REG_PINMUX_SEL2         0x0408      // Pin function multiplexer selection 2

#define REG_GPIO_OUT0           0x0500      // GPIOOUT0
#define REG_GPIO_OUT1           0x0504      // GPIOOUT1
#define REG_GPIO_OUT2           0x0508      // GPIOOUT2
#define REG_GPIO_OUT3           0x050c      // GPIOOUT3
#define REG_GPIO_OUT4           0x0510      // GPIOOUT4

#define REG_GPIO_EN0            0x0520      // GPIOEN0
#define REG_GPIO_EN1            0x0524      // GPIOEN1
#define REG_GPIO_EN2            0x0528      // GPIOEN2
#define REG_GPIO_EN3            0x052c      // GPIOEN3
#define REG_GPIO_EN4            0x0530      // GPIOEN4

#define REG_GPIO_IN0            0x0540      // GPIOIN0
#define REG_GPIO_IN1            0x0544      // GPIOIN1
#define REG_GPIO_IN2            0x0548      // GPIOIN2
#define REG_GPIO_IN3            0x054c      // GPIOIN3
#define REG_GPIO_IN4            0x0550      // GPIOIN4

#define REG_GPIO_INTR0          0x0560      // Level triggered GPIO interrupt enable 0
#define REG_GPIO_INTR1          0x0564      // Level triggered GPIO interrupt enable 1
#define REG_GPIO_INTR2          0x0568      // Level triggered GPIO interrupt enable 2
#define REG_GPIO_INTR3          0x056c      // Level triggered GPIO interrupt enable 3
#define REG_GPIO_INTR4          0x0570      // Level triggered GPIO interrupt enable 4

#define REG_GPIO_RISING_INTR0   0x0580      // Rising edge triggered interrupt Enable 0
#define REG_GPIO_FALLING_INTR0  0x05a0      // Falling edge triggered interrupt Enable 0
#define REG_GPIO_EDGE_ST_INTR0  0x05c0      // Edge triggered interrupt status 0


//=====================================================================
// Constant definitions Given by ACD analog designer
#define PLL_BAND_NUM           8


//----------------------------------------------------------------------------
// clock const definitions
#define CLK_20MHZ               20000000    // 20 MHz
#define CLK_27MHZ               27000000    // 27 MHz
#define CLK_30MHZ               30000000    // 30 MHz
#define CLK_40MHZ               40000000    // 40 MHz
#define CLK_50MHZ               50000000    // 50 MHz
#define CLK_54MHZ               54000000    // 54 MHz
#define CLK_60MHZ               60000000    // 60 MHz
#define CLK_81MHZ               81000000    // 81 MHz
#define CLK_100MHZ              100000000   // 100 MHz
#define CLK_120MHZ              120000000   // 120 MHz
#define CLK_324MHZ              324000000   // 324 MHz
#define CLK_337_5MHZ            337500000   // 337.5 MHz
#define CLK_351MHZ              351000000   // 351 MHz
#define CLK_405MHZ              405000000   // 405 MHz
#define CLK_432MHZ              432000000   // 432 MHz
#define CLK_APLL294MHZ          294912000   // 294.912 MHz
#define CLK_APLL270MHZ          270950400   // 270.9404 MHz

//===========================================================================
// PLL default clock settings

#define CRYSTAL_CLOCK           CLK_27MHZ
#define APLL0_DEFAULT_CLOCK     CLK_APLL270MHZ  // 270.9504M
#define PSPLL_DEFAULT_CLOCK     148500000   // 148.5 MHz
#define APLL1_DEFAULT_CLOCK     CLK_APLL294MHZ  // 294.912M

//=====================================================================
// Type definitions

typedef enum
{
    CAL_SRC_APLL0,
    CAL_SRC_DMPLL,
    CAL_SRC_CPUPLL,
    CAL_SRC_PSPLL,
    CAL_SRC_VOPLL,
    CAL_SRC_ADCPLL,
    CAL_SRC_SYSPLL,
    CAL_SRC_B2RPLL,
    CAL_SRC_APLL1,
    CAL_SRC_HDMIPLL,
    SRC_CPU_CLK,
    SRC_MEM_CLK,
    SRC_BUS_CLK
} CAL_SRC_T;

enum CLK_SEL
{
    APLL0_D1_CLK = 0,
    DMPLL_D1_CLK = 1,
    CPUPLL_D1_CLK = 2,
    PSPLL_D1_CLK = 3,
    VPLL_D1_CLK = 4,
    ADCPLL_D1_CLK = 5,
    SYSPLL_D1_CLK = 6,
    B2RPLL_D1_CLK = 7,
    HDMIPLL_D1_CLK = 8,
    APLL1_D1_CLK = 9,

    APLL0_D2_CLK = 16,
    DMPLL_D2_CLK = 17,
    CPUPLL_D4_CLK = 18,
    PSPLL_D2_CLK = 19,
    VPLL_D2_CLK = 20,
    ADCPLL_D2_CLK = 21,
    SYSPLL_D2_CLK = 22,
    B2RPLL_D2_CLK = 23,
    HDMIPLL_D2_CLK = 24,
    APLL1_D2_CLK = 25
};


#if CKGENHW_DEBUG

// implement at idehw_reg.c
EXTERN void CKGENDB_IoWr(UINT32 u4Offset, UINT32 u4Value, INT8 *szFile, INT32 Line);
EXTERN UINT32 CKGENDB_IoRd(UINT32 u4Offset, INT8 *szFile, INT32 Line);

#define CKGEN_WRITE32(offset, value)      CKGENDB_IoWr(offset, value, __FILE__, __LINE__)
#define CKGEN_READ32(offset)              CKGENDB_IoRd(offset, __FILE__, __LINE__)

#else /* CKGENHW_DEBUG */

#define CKGEN_WRITE32(offset, value)      IO_WRITE32(CKGEN_BASE, (offset), (value))
#define CKGEN_READ32(offset)              IO_READ32(CKGEN_BASE, (offset))

#endif /* CKGENHW_DEBUG */


//=====================================================================
// Interface

#define BSP_GetDomainClock(eSrc)    BSP_GetClock(eSrc, NULL, NULL, NULL, NULL)
#define BSP_GetPLLClock(eSrc)       BSP_GetClock(eSrc, NULL, NULL, NULL, NULL)

extern void BSP_CkgenInit(void);

extern BOOL BSP_Calibrate(CAL_SRC_T eSource, UINT32 u4Clock);

extern UINT32 BSP_GetClock(CAL_SRC_T eSrc, UINT8* pu1Band, UINT16* pu2MS,
    UINT16* pu2NS, UINT16* pu2Counter);

extern void vDrvVOPLLSet(UINT32 u4CLK, BOOL fgLVDSDiv2);
extern void vDrvADCPLLSetFreq(UINT8 bADCCLK);
extern void CKGEN_SetVOPLLFreq(UINT32 u4Freq);
extern void CKGEN_SetADCPLLFreq(UINT32 u4Freq);
extern void CKGEN_SetPSPLLFreq(UINT32 u4Freq);
extern void CKGEN_SetB2RPLLFreq(UINT32 u4Freq);
extern BOOL BSP_ApllFineTune(CAL_SRC_T eSource);

#endif  // X_CKGEN_H

