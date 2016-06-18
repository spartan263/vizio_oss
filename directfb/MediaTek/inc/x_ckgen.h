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

#if CKGENHW_DEBUG

// implement at idehw_reg.c
EXTERN VOID CKGENDB_IoWr(UINT32 u4Offset, UINT32 u4Value, INT8 *szFile, INT32 Line);
EXTERN UINT32 CKGENDB_IoRd(UINT32 u4Offset, INT8 *szFile, INT32 Line);

#define CKGEN_WRITE32(offset, value)      CKGENDB_IoWr(offset, value, __FILE__, __LINE__)
#define CKGEN_READ32(offset)              CKGENDB_IoRd(offset, __FILE__, __LINE__)

#else /* CKGENHW_DEBUG */

#define CKGEN_WRITE32(offset, value)      IO_WRITE32(CKGEN_BASE, (offset), (value))
#define CKGEN_READ32(offset)              IO_READ32(CKGEN_BASE, (offset))

#endif /* CKGENHW_DEBUG */


//----------------------------------------------------------------------------
// PLL Register definitions
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// XTAL config
#define XTAL_STRAP_MODE     (CKGEN_READ32(0) & 0x60)
    #define SET_XTAL_27MHZ  (0U << 5)
    #define SET_XTAL_60MHZ  (1U << 5)
    #define SET_XTAL_48MHZ  (2U << 5)
    #define SET_XTAL_54MHZ  (3U << 5)
    #define IS_XTAL_27MHZ() (XTAL_STRAP_MODE == SET_XTAL_27MHZ)
    #define IS_XTAL_60MHZ() (XTAL_STRAP_MODE == SET_XTAL_60MHZ)
    #define IS_XTAL_48MHZ() (XTAL_STRAP_MODE == SET_XTAL_48MHZ)
    #define IS_XTAL_54MHZ() (XTAL_STRAP_MODE == SET_XTAL_54MHZ)
    #define GET_XTAL_CLK()  (IS_XTAL_27MHZ() ? (CLK_27MHZ) :    \
                            (IS_XTAL_60MHZ() ? (CLK_60MHZ) :    \
                            (IS_XTAL_48MHZ() ? (CLK_48MHZ) : (CLK_54MHZ))))

#define REG_XTALCFG             0x0080
    #define XTAL_BS_MODE        (1U << 0)
    #define XTAL_NORMAL_MODE    (0U << 0)

#if 0
#define REG_APLL1_CFG0          (0x0100)    // Audio PLL 1 Configuration 0
    #define RG_APLL1_MODIN      (0x1ffffffU << 0)   // APLL1 modulator input
    #define RG_APLL1_BIASOPTEN  (1U << 26)  // APLL1 bandgap EXTERNal enable
    #define RG_APLL1_CAL_EN     (1U << 27)  // APLL1 output clkdiv reset
    #define RG_APLL1_CHP_B      (0x0fU << 28)   // APLL1 scf divider select
#define REG_APLL1_CFG1          (0x0104)    // Audio PLL 1 Configuration 1
    #define RG_APLL1_ENABLE     (1U << 0)   // APLL1 enable
    #define RG_APLL1_FREFSEL    (1U << 1)   // APLL1 reference clk select
    #define RG_APLL1_LPF_R_B    (1U << 2)   // APLL1 lpf resistor value
    #define RG_APLL1_PREDIV     (7U << 4)   // APLL1 predivider code select
    #define APLL1_PREDIV_SHIFT  (4)
    #define RG_APLL1_PREDIV_EN  (1U << 7)   // APLL1 predivider enable
    #define RG_APLL1_TEST_B_0   (1U << 8)   // APLL1 test select
    #define RG_APLL1_TEST_EN    (1U << 9)   // APLL1 test mode enable
    #define RG_APLL1_V2I_RB     (3U << 10)  // APLL1 v2i resistor value
    #define RG_APLL1_VCOG_B     (7U << 12)  // APLL1 vco gain control
    #define APLL1_VCOG_B_SHIFT  (12)
    #define DEFAULT_VCOG_B      (2)
    #define RG_APLL1_VCO_BAND   (7U << 16)  // APLL1 vco band select
    #define APLL1_VCO_BAND_SHIFT    (16)
    #define RG_APLL1_VCTR_SEL   (1U << 20)  // APLL1 output control voltage select
    #define RG_APPL1_TESTCLK_EN (1U << 21)  // APPL1 test mode enable
    #define RG_APLL1_VCOINIT_ENB    (1U << 22)  // APLL1 Trigger vco to oscillate by a falling
    #define RG_APLL1_RESET_B_CORE   (1U << 24)  // APLL1 modulator and post divider reset
    #define RG_APLL1_MUTE       (1U << 25)  // APLL1 modulator and post divider mute
    #define RG_APLL1_DELAY      (1U << 26)  // APLL1 post divider delay
    #define RG_APLL1_POSDIVSEL  (1U << 27)  // APLL1 post divider select
    #define RG_APLL1_SCFDIV     (0xfU << 28)    // APLL1 scf divider select
#define REG_APLL1_CFG2          (0x0108)    // Audio PLL 1 Configuration 2
    #define RG_APLL1_K1            (1U << 0)   // APLL1 K1 select, clk_k1 is APLL_CK/2(0), APLL_CK/3(1)
    #define RG_APLL1_K2            (0x3fU << 8)    // APLL1 K2 select, clk_k2 is clk_k1/apll_k2.
    #define RG_APLL1_BP            (1U << 30)  // APLL1 Bypass, 0:normal, 1:bypass(from pin)
    #define RG_APLL1_DIV_RST       (1U << 31)  // APLL1 Divider counter reset, 0:normal, 1:reset
#define REG_APLL1_CFG3          (0x010c)    // Audio PLL 1 Configuration 3
    #define RG_APPL1_MODDIV     (0xffU << 0)    // APPL1 post divider code select
    #define RG_APLL1_LPFDIV     (0xffU << 8)    // APLL1 lpf clk select
    #define RG_APLL1_RESET_B_CLKDIV (1U << 16)  // APLL1 output clkdiv reset
    #define RG_APLL1_TCLKSEL    (1U << 17)  // APLL1 ADAC lpf clk select
    #define RG_APLL1_MODCLK_SEL (1U << 18)  // APLL1 mod clk select
    #define RG_APPL1_REV        (0xffU << 24)   // APPL1 reserved register

//----------------------------------------------------------------------------
// DMPLL
#define REG_DMPLLCFG0           0x0110      // DRAM PLL Configuration 0
    #define RG_DMPLL_DIVI12     (1U << 1)   // default on.
    #define RG_DMPLL_ENABLE     (1U << 2)   // PLL enable
    #define RG_DMPLL_ENABLE_FAK (1U << 3)   // PLL enable
    #define RG_DMPLL_D_MASK     (0x3fU << 8)
    #define RG_DMPLL_D(x)       ((x) << 8)  // DMPLL = 27MHz*DM_D/(DIV12+1)
    #define RG_DMPLL_VCOG_SEL   (7U << 20)
    #define VCOG_300_600MHZ     (5U << 20)  // 450MHz < DMPLL
    #define VCOG_250_550MHZ     (4U << 20)  // 360MHz < DMPLL <= 450MHz
    #define VCOG_200_400MHZ     (3U << 20)  // 260MHz < DMPLL <= 360MHz
    #define VCOG_200_300MHZ     (2U << 20)  // DMPLL <= 260MHz
    #define BANDGAP_BIAS_MASK   (7U << 26)  // recommend DEF_BANDGAP_BIAS, default 0
    #define DEF_BANDGAP_BIAS    (4U << 26)
    #define DMPLL_OUT_DIV       (1U << 29)  // recommend off, default off.

//----------------------------------------------------------------------------
// MEMPLL
#define REG_MEMPLLCFG0          0x011c
    #define RG_MEMPLL_PD        (1U << 20)
    #define RG_MEMPLL_DIV2      (1U << 21)
    #define RG_MEMPLL_BAND_MASK (3U << 22)
    #define RG_MEMPLL_BAND_LARGE    (0U << 22)  // 250MHz ~ 400MHz  270MHz < MEMPLL
    #define RG_MEMPLL_BAND_MEDIUM   (1U << 22)  // 150MHz ~ 300MHz  170MHz < MEMPLL <= 270MHz
    #define RG_MEMPLL_BAND_SMALL    (2U << 22)  // 100MHz ~ 200MHz, MEMPLL <= 170MHz
#define REG_MEMPLLCFG1          0x0120
    #define RG_MEMPLL_LDO_MASK  (3U << 0)
    #define RG_MEMPLL_LDO_115V  (0U << 0)
    #define RG_MEMPLL_LDO_12V   (1U << 0)
    #define RG_MEMPLL_LDO_125V  (2U << 0)
    #define RG_MEMPLL_LDO_13V   (3U << 0)

// #define REG_DSSTOP_CFG0         0x01b0      // DDSTOP Configuration 0
    #define DDS_FBK_SEL         (3U << 6)   // DDS feedback singla select Tomson
    #define DDS_HSYNC_SEL       (1U << 5)   // DDS hsync input polarity control Tomson
    #define DDS_HSYNC_POL       (1U << 8)   // DDS hsync input polarity cnotrol Tomson
    #define RSYNC_POL           (1U << 9)   // Pix_clock sync polarity control Tomson
    #define ADCPLL_IN_DIV       (1U <<1)    // ADCPLL input frequency/2 Tomson
    #define ADCPLLIN_SEL        (1U << 29)  // o from 27M , 1 from DDS Tomson
#endif

#define REG_VOPLL_CFG0          0x0144      // VPLL Configuration 0
    #define VPLL_BAND           (3 << 28)
        #define VPLL_BAND_LARGE     (0 << 28)
        #define VPLL_BAND_MEDIUM    (1 << 28)
        #define VPLL_BAND_SMALL     (3 << 28)
    #define VPLL_DIV            (0xf << 24)
        #define VPLL_DIV_SHIFT      (24)
    #define VPLL_DIV12          (1 << 22)
        #define VPLL_DIV12_SHIFT    (22)
    #define VPLL_PD             (1 << 21)
    #define VPLL_LVDS_CLK_DIV2  (1 << 22)
    #define VPLL_PHASE_RST      (1 << 17)
#define REG_VOPLL_CFG1          0x0144      // VPLL Configuration 1
    #define VPLL_LPFR           (0x7 << 4)
        #define VPLL_LPFR_4K    (0x4 << 4)
    #define VPLL_ICP            (0x7 << 8)
        #define VPLL_ICP_20UA   (0x3 << 8)
    #define VPLL_FBDIV1         (1 << 15)
/*
    #define VPLL_VCOG_B         (0xfU << 0)
    #define VPLL_CHP_B          (0xfU << 4)
    #define VPLL_VCO_BAND       (7U << 8)
    #define VPLL_DPIX_DIV2      (1U << 12)
    #define VPLL_DPIX_X2        (1U << 13)
    #define VPLL_TSTD_XTALO     (1U << 15)
    #define VPLL_TEST_B         (7U << 20)
    #define VPLL_REV            (0xffU << 24)
    #define VPLL_REV_PWN_BB     (1U << 26)
*/
#define REG_VOPLL_CFG2          0x0148      // VPLL Configuration 2
    #define VPLL_BP             (1U << 30)
    #define VPLL_DIV_RST        (1U << 31)

#if 0
#define REG_APLL2_CFG0          (0x01a0)    // APLL2 Configuration 0
    #define RG_APLL2_MODIN      (0x1ffffffU << 0)   // APLL2 modulator input
    #define RG_APLL2_BIASOPTEN  (1U << 26)  // APLL2 bandgap EXTERNal enable
    #define RG_APLL2_CAL_EN     (1U << 27)  // APLL2 output clkdiv reset
    #define RG_APLL2_CHP_B      (0x0fU << 28)   // APLL2 scf divider select
#define REG_APLL2_CFG1          (0x01a4)    // APLL2 Configuration 1
    #define RG_APLL2_ENABLE     (1U << 0)   // APLL2 enable
    #define RG_APLL2_FREFSEL    (1U << 1)   // APLL2 reference clk select
    #define RG_APLL2_LPF_R_B    (1U << 2)   // APLL2 lpf resistor value
    #define RG_APLL2_PREDIV     (7U << 4)   // APLL2 predivider code select
    #define APLL2_PREDIV_SHIFT  (4)
    #define RG_APLL2_PREDIV_EN  (1U << 7)   // APLL2 predivider enable
    #define RG_APLL2_TEST_B_0   (1U << 8)   // APLL2 test select
    #define RG_APLL2_TEST_EN    (1U << 9)   // APLL2 test mode enable
    #define RG_APLL2_V2I_RB     (3U << 10)  // APLL2 v2i resistor value
    #define RG_APLL2_VCOG_B     (7U << 12)  // APLL2 vco gain control
    #define APLL2_VCOG_B_SHIFT  (12)
    #define RG_APLL2_VCO_BAND   (7U << 16)  // APLL2 vco band select
    #define APLL2_VCO_BAND_SHIFT    (16)
    #define RG_APLL2_VCTR_SEL   (1U << 20)  // APLL2 output control voltage select
    #define RG_APLL2_TESTCLK_EN (1U << 21)  // APLL2 test mode enable
    #define RG_APLL2_VCOINIT_ENB    (1U << 22)  // APLL2 Trigger vco to oscillate by a falling
    #define RG_APLL2_RESET_B_CORE   (1U << 24)  // APLL2 modulator and post divider reset
    #define RG_APLL2_MUTE       (1U << 25)  // APLL2 modulator and post divider mute
    #define RG_APLL2_DELAY      (1U << 26)  // APLL2 post divider delay
    #define RG_APLL2_POSDIVSEL  (1U << 27)  // APLL2 post divider select
    #define RG_APLL2_SCFDIV     (0xfU << 28)    // APLL2 scf divider select
#define REG_APLL2_CFG2          (0x01a8)    // APLL2 Configuration 2
    #define RG_APLL2_K1            (1U << 0)   // APLL2 K1 select, clk_k1 is APLL_CK/2(0), APLL_CK/3(1)
    #define RG_APLL2_K2            (0x3fU << 8)    // APLL2 K2 select, clk_k2 is clk_k1/apll_k2.
    #define RG_APLL2_BP            (1U << 30)  // APLL2 Bypass, 0:normal, 1:bypass(from pin)
    #define RG_APLL2_DIV_RST       (1U << 31)  // APLL2 Divider counter reset, 0:normal, 1:reset
#define REG_APLL2_CFG3          (0x01ac)    // APLL2 Configuration 3
    #define RG_APLL2_MODDIV     (0xffU << 0)    // APLL2 post divider code select
    #define RG_APLL2_LPFDIV     (0xffU << 8)    // APLL2 lpf clk select
    #define RG_APLL2_RESET_B_CLKDIV (1U << 16)  // APLL2 output clkdiv reset
    #define RG_APLL2_TCLKSEL    (1U << 17)  // APLL2 ADAC lpf clk select
    #define RG_APLL2_MODCLK_SEL (1U << 18)  // APLL2 mod clk select
    #define RG_APLL2_REV        (0xffU << 24)   // APLL2 reserved register
#endif

// -- MT5391 start
#define REG_PSPLL_CFG0          0x0130      // PSCAN PLL Configuration 0
    #define PS_CKSEL            (3U << 0)
        #define SYS_27MHZ_SRC   (0U << 0)
        #define MAIN_CLK_SRC    (1U << 0)
    #define PS_CP_BW_CTRL       (7U << 5)
    #define DEF_PS_BW_CTRL      (0U << 5)
    #define PS_DIV_REF          (0xff << 8)
    #define PS_DIV_REF_CNT(x)   (((x) << 8) & PS_DIV_REF)
    #define PS_PODIV            (7U << 25)
    #define PS_PODIV_1          (0U << 25)
    #define PS_PODIV_2          (1U << 25)
    #define PS_PODIV_4          (2U << 25)
    #define PS_PODIV_8          (3U << 25)
    #define PS_PODIV_16         (4U << 25)
    #define PS_PODIV_32         (5U << 25)

    #define CAL_EN_PS           (1U << 0)
    #define RESET_PS            (1U << 3)
    #define TEST_JITTER_PS      (1U << 4)
    #define TEST_EN_PS          (1U << 5)
    #define PD_PSPLL            (1U << 6)
    #define VCON_SEL_PS         (1U << 7)
    #define PSPLL_DIV_4         2
    #define CKSEL_0_PS          (1U << 12)
    #define CKSEL_1_PS          (1U << 13)
    #define VCO_GAIN_PS         (7U << 16)
    #define VCO_GAIN_PS_SHIFT   (16)
    #define V2I_RB_PS           (3U << 28)
    #define V2I_RB_PS_SHIFT     (28)

#define REG_PSPLL_CFG1          0x0134      // PSCAN PLL Configuration 1
    #define PS_VCOCAL_EN        (1U << 0)
    #define PS_RSTB             (1U << 6)
    #define PS_PWD              (1U << 7)
    #define PS_MONCLKEN         (1U << 17)

    #define CHP_PS              (0xfU << 4)
    #define CHP_PS_SHIFT        (4)
    #define VCO_BAND_PS         (7U << 8)
    #define VCO_BAND_PS_SHIFT   (8)
    #define PS_REV              (0xffU << 24)
    #define PS_REV_SHIFT        (24)

#define REG_PSPLL_CFG2          0x0138      // PSCAN PLL Configuration 2
    #define PSPLL_BP            (1U << 30)
    #define PSPLL_DIV_RST       (1U << 31)

#define REG_SYSPLL_CFG0         (0x0160)
    #define VPLL_FROM_DDDS      (1U << 22)
    #define PSPLL_FROM_PSDDS    (1U << 21)
    #define DMPLL_FROM_DMSS     (1U << 20)

#define REG_PLL_TOPCFG0         (0x01b0)
    #define RG_PLL_BG_PWD       (1U << 10)

#define REG_PLL_CALIB           0x01c0      // PLL Calibration
    #define RG_PLL_CAL_CNT      (0xfffU)    // Calibration counter
    #define RG_PLL_CAL_TRIGGER  (1U << 15)  // Calibration trigger
    #define RG_PLL_CAL_SELECT   (0x1fU << 16)   // Selection of Clock for Calibration
    #define PLL_CAL_SEL(x)      ((((UINT32)x) << 16) & RG_PLL_CAL_SELECT)

#define IS_TVDPLL_540MHZ(clk)   (((CLK_540MHZ + (4*(GET_XTAL_CLK()/256))) > clk) && \
                                ((CLK_540MHZ - (4*(GET_XTAL_CLK()/256))) < clk))
#define IS_DTDPLL_600MHZ(clk)   (((CLK_600MHZ + (4*(GET_XTAL_CLK()/256))) > clk) && \
                                ((CLK_600MHZ - (4*(GET_XTAL_CLK()/256))) < clk))
#define IS_DTDPLL_432MHZ(clk)   (((CLK_432MHZ + (4*(GET_XTAL_CLK()/256))) > clk) && \
                                ((CLK_432MHZ - (4*(GET_XTAL_CLK()/256))) < clk))
#define IS_8297_PLL_INCLK(clk, target) (((target + (GET_XTAL_CLK()/256)) > clk) && \
                                        ((target - (GET_XTAL_CLK()/256)) < clk))

//----------------------------------------------------------------------------
// Clock settings
#define REG_MISC_CKCFG          (0x0200)
    #define SLOW_CK_SEL_SHIFT   (0)
    #define SLOW_CK_SEL_MASK    (7U << 0)
    #define BIST_CK_SEL_SHIFT   (4)
    #define BIST_CK_SEL_MASK    (3U << 4)
// -- MT5391 end
#define REG_XTAL_CKCFG          (0x0204)
    #define RG_C27M_CK          (3U << 8)
    #define DEFAULT_C27M        (1U << 8)
    #define RG_VCXO27_CK        (3U << 12)
    #define DEFAULT_VCXO        (2U << 12)
#define REG_CPU_CKCFG           (0x0208)
    #define AXI_CK_SEL          (1U << 16)
    #define BUS_CK_SHIFT        (8)
    #define BUS_CK_MASK         (7U << 8)
    #define BUS_CK_162MHZ       (3U << 8)
    #define CPU_CK_SEL_MASK     (0xfU << 0)
    #define CPU_CK_CPUPLL       (1U << 0)
#define REG_MEM_CKCFG           (0x020c)
    #define NORMAL_MEM_CFG      (0)

#define REG_MS_CKCFG            0x0250      // MS_CK Configuration
    #define MS_CK_SEL           (0xFU << 0)   // MS_CK Clock source selection
    #define MS_CK_TST           (7U << 4)   // MS_CK test clock source selection
    #define MS_CK_PD            (1U << 7)   // MS_CK clock power down
#define REG_SD_CKCFG            0x0254      // SD_CK Configuration
    #define SD_CK_SEL           (0xFU << 0)   // SD_CK Clock source selection
    #define SD_CK_TST           (7U << 4)   // SD_CK test clock source selection
    #define SD_CK_PD            (1U << 7)   // SD_CK clock power down
    #define SD_CK_D2_SEL        (1U << 8)   // SD_CK div2 selection


// MT5391 ready start

//MT5391 RMII clock source configure
#define REG_RMII_CKCFG            0x0264      // RMII_CK Configuration
    #define RMII_RXCK_SEL           (0x3U << 12)   // RMII_CK Clock source selection
    #define RMII_RXCK_PAD_RXCK (0x3U << 12)   // RMII_CK Clock source selection
    #define RMII_TXCK_SEL           (0x3U << 8)   // RMII_CK test clock source selection
    #define RMII_TXCK_PAD_TXCK (0x2U << 8)   // RMII_CK test clock source selection
    #define RMII_CK_PD            (1U << 15)   // RMII_CK clock power down

#define REG_VPCLK_STOP          (0x029c)
    #define CK_MAIN_CLK_STOP    (1U << 8)

#define REG_PINMUX_SEL0         (0x0400)    // Pin function multiplexer selection 0
#define REG_PINMUX_SEL1         (0x0404)    // Pin function multiplexer selection 1
#define REG_PINMUX_SEL2         (0x0408)    // Pin function multiplexer selection 2
#define REG_PINMUX_SEL3         (0x040c)    // Pin function multiplexer selection 3
#define REG_PINMUX_SEL4         (0x0410)    // Pin function multiplexer selection 4
#define REG_PINMUX_MISC         (0x0420)    // Pin function multiplexer misc

#define REG_GPIO_OUT0           (0x0500)    // GPIOOUT0
#define REG_GPIO_OUT1           (0x0504)    // GPIOOUT1
#define REG_GPIO_OUT2           (0x0508)    // GPIOOUT2
#define REG_GPIO_OUT3           (0x050c)    // GPIOOUT3
#define REG_GPIO_OUT4           (0x0510)    // GPIOOUT4
#define REG_GPIO_OUT5           (0x0514)    // GPIOOUT5

#define REG_GPIO_EN0            (0x0520)    // GPIOEN0
#define REG_GPIO_EN1            (0x0524)    // GPIOEN1
#define REG_GPIO_EN2            (0x0528)    // GPIOEN2
#define REG_GPIO_EN3            (0x052c)    // GPIOEN3
#define REG_GPIO_EN4            (0x0530)    // GPIOEN4
#define REG_GPIO_EN5            (0x0534)    // GPIOEN5

#define REG_GPIO_IN0            (0x0540)    // GPIOIN0
#define REG_GPIO_IN1            (0x0544)    // GPIOIN1
#define REG_GPIO_IN2            (0x0548)    // GPIOIN2
#define REG_GPIO_IN3            (0x054c)    // GPIOIN3
#define REG_GPIO_IN4            (0x0550)    // GPIOIN4
#define REG_GPIO_IN5            (0x0554)    // GPIOIN5

#define REG_GPIO_INTR0          (0x0560)    // Level triggered GPIO interrupt enable 0
#define REG_GPIO_INTR1          (0x0564)    // Level triggered GPIO interrupt enable 1
#define REG_GPIO_INTR2          (0x0568)    // Level triggered GPIO interrupt enable 2
#define REG_GPIO_INTR3          (0x056c)    // Level triggered GPIO interrupt enable 3
#define REG_GPIO_INTR4          (0x0570)    // Level triggered GPIO interrupt enable 4
#define REG_GPIO_INTR5          (0x0574)    // Level triggered GPIO interrupt enable 5

#define REG_GPIO_RISING_INTR0   (0x0580)    // Rising edge triggered interrupt Enable 0
#define REG_GPIO_FALLING_INTR0  (0x05a0)    // Falling edge triggered interrupt Enable 0
#define REG_GPIO_EDGE_ST_INTR0  (0x05c0)    // Edge triggered interrupt status 0

// MT5391 ready end

//============================================================================
// MT537x register
#define REG_ADCPLL_CFG0         0x0150      // ADC PLL Configuration 0
    #define CAL_EN_ADC          (1U << 0)
    #define CKSEL_ADC           (1U << 1)
    #define RESET_ADC           (1U << 3)
    #define TEST_JITTER_ADC     (1U << 4)
    #define TEST_EN_ADC         (1U << 5)
    #define PD_ADCPLL           (1U << 6)
    #define VCON_SEL_ADC        (1U << 7)
    #define DIV_SEL_ADC         (3U << 8)
    #define DIV_SEL_ADC_SHIFT   (8)
    #define VCO_GAIN_ADC        (7U << 16)
    #define VCO_GAIN_ADC_SHIFT  (16)
    #define V2I_RB_ADC          (3U << 28)
    #define V2I_RB_ADC_SHIFT    (28)
#define REG_ADCPLL_CFG1         0x0154      // ADC PLL Configuration 1
    #define CHP_ADC             (0xfU << 4)
    #define VCO_BAND_ADC        (7U << 8)
    #define ADC_PS              (0x1fU << 11)
    #define ADC_NS              (0x1fU << 16)
    #define ADC_NS_SHIFT        (16)
    #define ADC_REV             (0xffU << 24)
    #define ADC_REV_SHIFT       (24)
    #define ADC_EN_SEFBIA       (1U << 29)
#define REG_ADCPLL_CFG2         0x0158      // ADC PLL Configuration 2
    #define ADCPLL_BP           (1U << 30)
    #define ADCPLL_DIV_RST      (1U << 31)

#define REG_TEST_MODE	0x01c4	// 
    #define C_VBI_VGA_CK_DIV	(1U << 27)	// divide frequency

#if 0
#define REG_SC_CKCFG            0x0204      // SC_CK Configuration
    #define SC_CK_SEL           (3U << 0)   // SC_CK Clock source selection,
                                            // 0:xtal_ck, 1:sysdll_d6_ck, 2:syspll_d8_ck, 3:slow_cpu_ck
    #define SC_CK_TST           (7U << 4)   // SC_CK test clock source selection,
                                            // 001:Test clock from XGPIO6, 010:Test clock from XGPIO7,
                                            // 100:Test clock from XT0DATA7, others:Normal clock
    #define SC_CK_PD            (1U << 7)   // SC_CK clock power down, 0:Normal clock, 1:Power down
#define REG_B2R_CKCFG           0x0208      // B2R_CK Configuration
    #define B2R_CK_SEL          (3U << 0)   // B2R_CK Clock source selection, 
                                            // 0:xtal_ck, 1:b2rpll_d1_ck, 2:dmpll_d1_ck, 3:slow_cpu_ck
    #define B2R_CK_TST          (7U << 4)   // B2R_CK test clock source selection
                                            // 001:Test clock from XGPIO6, 010:Test clock from XGPIO7,
                                            // 100:Test clock from XT0DATA3, others:Normal clock
    #define B2R_CK_PD           (1U << 7)   // B2R_CK clock power down, 0:Normal clock, 1:Power down
    #define B2R_CK_DLY          (0x7fU << 8)    // B2R_CK Clock root delay
#define REG_OSD_CKCFG           0x020c      // OSD_CK Configuration
    #define OSD_CK_SEL          (7U << 0)   // OSD_CK Clock source selection, 0:xtal_ck,
                                            // 1:syspll_d2_ck, 2:oclk, 3:apll2_d2_ck, 4:dmpll_d1_ck,
                                            // 5:cpupll_d3_ck, 6:syspll_d3_ck, 7:slow_cpu_ck
    #define OSD_CK_TST          (7U << 4)   // OSD_CK test clock source selection,
                                            // 001:Test clock from XGPIO6, 010:Test clock from XGPIO7,
                                            // 100:Test clock from XT0DATA7, others:Normal clock
    #define OSD_CK_PD           (1U << 7)   // OSD_CK clock power down 0:Normal clock, 1:Power down
    #define OSD_CK_DLY          (0x7fU << 8)    // OSD_CK Clock root delay
#endif

#define REG_MDEC_CKCFG          0x0210      // MDEC_CK Configuration
    #define MDEC_CK_SEL         (7U << 0)   // MDEC_CK Clock source selection, 0:xtal_ck, 1:syspll_d2_ck,
                                            // 2:b2rpll_d1_ck, 3:apll2_d2_ck, 4:dmpll_d1_ck,
                                            // 5:cpupll_d3_ck, 6:syspll_d3_ck, 7:slow_cpu_ck
    #define MDEC_CK_TST         (7U << 4)   // MDEC_CK test clock soruce selection
                                            // 001:Test clock from XGPIO6, 010:Test clock from XGPIO7,
                                            // 100:Test clock from XT0DATA7, others:Normal clock
    #define MDEC_CK_PD          (1U << 7)   // MDEC_CK clock power down, 0:Normal clock, 1:Power down
    #define MDEC_CK_DLY         (0x7fU << 8)    // MDEC_CK Clock root delay
#define REG_DMX_CKCFG           0x0214      // DMX_CK Configuration
    #define DMX_CK_SEL          (7U << 0)   // DMX_CK Clock source selection, 0:xtal_ck,1:syspll_d2_ck,
                                            // 2:b2rpll_d1_ck, 3:apll2_d2_ck, 4:dmpll_d1_ck,
                                            // 5:cpupll_d3_ck, 6:syspll_d3_ck, 7:slow_cpu_ck
    #define DMX_CK_TST          (7U << 4)   // DMX_CK test clock source selection
                                            // 001:Test clock from XGPIO6, 010:Test clock from XGPIO7,
                                            // 100:Test clock from XT0DATA7, others:Normal clock
    #define DMX_CK_PD           (1U << 7)   // DMX_CK clock power down
    #define DMX_CK_DLY          (0x7fU << 8)    // DMX_CK Clock root delay
#define REG_POD_CKCFG           0x0218      // POD_CK Configuration
    #define POD_CK_SEL          (7U << 0)   // POD_CK Clock source selection, 0:xtal_ck, 1:syspll_d2_ck,
                                            // 2:b2rpll_d1_ck, 3:apll2_d2_ck, 4:dmpll_d1_ck,
                                            // 5:cpupll_d3_ck, 6:sysplld3_ck, 7:slow_cpu_ck,
    #define POD_CK_TST          (7U << 4)   // POD_CK test clock source selection
                                            // 001:Test clock from XGPIO6, 010:Test clock from XGPIO7,
                                            // 100:Test clock from XT0DATA7, others:Normal clock
    #define POD_CK_PD           (1U << 7)   // POD_CK clock power down, 0:Normal clock, 1:Power down
    #define POD_CK_DLY          (0x7fU << 8)    // POD_CK Clock root delay
#define REG_GRA_CKCFG           0x0224      // GRA_CK Configuration
    #define GRA_CK_SEL          (7U << 0)   // GRA_CK Clock source selection, 0:xtal_ck, 1:syspll_d2_ck,
                                            // 2:b2rpll_d1_ck, 3:apll2_d2_ck, 4:dmpll_d1_ck
                                            // 5:cpupll_d3_ck, 6:syspll_d3_ck, 7:slow_cpu_ck
    #define GRA_CK_TST          (7U << 4)   // GRA_CK test clock source selection
                                            // 001:Test clock from XGPIO6, 010:Test clock from XGPIO7,
                                            // 100:Test clock from XT0DATA7, others:Normal clock
    #define GRA_CK_PD           (1U << 7)   // GRA_CK clock power down
    #define GRA_CK_DLY          (0x7fU << 8)    // GRA_CK Clock root delay
#define REG_DSP_CKCFG           0x0220      // DSP_CK Configuration
    #define DSP_CK_SEL          (7U << 0)   // DSP_CK Clock source selection, 0:xtal_ck, 1:syspll_d2_ck,
                                            // 2:b2rpll_d1_ck, 3:apll2_d2_ck, 4:dmpll_d1_ck,
                                            // 5:cpupll_d3_ck, 6:syspll_d3_ck, 7:slow_cpu_ck
    #define DSP_CK_TST          (7U << 4)   // DSP_CK test clock source selection
                                            // 001:Test clock from XGPIO6, 010:Test clock from XGPIO7,
                                            // 100:Test clock from XT0DATA7, others:Normal clock
    #define DSP_CK_PD           (1U << 7)   // DSP_CK clock power down, 0:Normal clock, 1:Power down
    #define DSP_CK_DLY          (0x7fU << 8)    // DSP_CK Clock root delay

#define REG_TS0_CKCFG           0x028c      // TS0_CK Configuration
    #define TS0_CK_SEL          (1U << 0)   // TS0_CK Clock source selection, 0:from pin XTCLK,
                                            // 1:from internal test clock - IDE_TS_CLK
#define REG_TS1_CKCFG           0x0290      // TS1_CK Configuration
    #define TS1_CK_SEL          (1U << 0)   // TS1_CK Clock source selection, 0:M,
                                            // 1:from internal test cock - IDE_TS_CLK
#define REG_DMX_BIST_CKCFG      0x0298      // DMX_BIST_CK Configuration
    #define DMX_BIST_CK         (3U << 0)   // DMX_BIST_CK Clock source selection, 0:bus_ck, 1:mem_ck,
                                            // 2:dmx_ck, 3:from pin XGPIO5
// #define REG_VPCLK_STOP          0x029c      // VPCLK_STOP Configuration
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
    #define C_VBI_VGACK_DIV     (1U << 16)
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

#define PAD_NO_PULL             0
#define PAD_PULL_DOWN           1
#define PAD_PULL_UP             2
#define PAD_BUS_HOLD            3
#define PADDRV_2MA              0
#define PADDRV_4MA              1
#define PADDRV_6MA              2
#define PADDRV_8MA              3

#define REG_PADPU_CFG0          0x0300      // PAD PULL Configuration 0
    #define PAD0_JTAG1          (3U << 0)   // JTAG group 1
    #define PAD0_JTAG2          (3U << 2)   // JTAG group 2
    #define PAD0_PBIC           (3U << 4)   // PBI Control and Address group
    #define PAD0_PBID           (3U << 6)   // PBI Data group
    #define PAD0_UART2          (3U << 8)   // UART group 2
    #define PAD0_IIC            (3U << 10)  // IIC group
    #define PAD0_PER            (3U << 12)  // Peripheral group
    #define PAD0_STRAP          (3U << 14)  // Strap group
    #define PAD0_GPIO0          (3U << 16)  // GPIO group0
    #define PAD0_GPIO1          (3U << 18)  // GPIO group1
    #define PAD0_ATN            (3U << 20)  // ATN group
    #define PAD0_TUNER          (3U << 22)  // TUNER group
    #define PAD0_AGC            (3U << 24)  // AGC group
    #define PAD0_BNDOP          (3U << 30)  // Bonding option group
#define REG_PADPU_CFG1          0x0304      // PAD PULL Configuration 1
    #define PAD1_GPIO0          (3U << 0)   // GPIO group 0
    #define PAD1_GPIO1          (3U << 2)   // GPIO group 1
    #define PAD1_DVIC           (3U << 4)   // DVI Control group
    #define PAD1_DVID0          (3U << 6)   // DVI Data group 0
    #define PAD1_DVID1          (3U << 8)   // DVI Data group 1
    #define PAD1_BNDOP          (3U << 30)  // Bonding option group
#define REG_PADSMT_CFG0         0x0340      // PAD Smith trigger Configuration 0
    #define PADSMT_JTAG1        (1U << 0)
    #define PADSMT_JTAG2        (1U << 1)
    #define PADSMT_PBICA        (1U << 2)
    #define PADSMT_PBID         (1U << 3)
    #define PADSMT_POD1         (1U << 4)
    #define PADSMT_POD2         (1U << 5)
    #define PADSMT_UART2        (1U << 6)
    #define PADSMT_IIC          (1U << 7)
    #define PADSMT_PER          (1U << 8)
    #define PADSMT_I1394D       (1U << 9)
    #define PADSMT_I1394C       (1U << 10)
    #define PADSMT_STRAP        (1U << 11)
    #define PADSMT_MS           (1U << 12)
    #define PADSMT_SDIO         (1U << 13)
    #define PADSMT_TSD          (1U << 14)
    #define PADSMT_TSC          (1U << 15)
    #define PADSMT_GPIO0        (1U << 16)
    #define PADSMT_GPIO1        (1U << 17)
    #define PADSMT_DVIC         (1U << 18)
    #define PADSMT_DVID0        (1U << 19)
    #define PADSMT_DVID1        (1U << 20)
#define REG_PADDRV_CFG0         0x0320      // PAD Drive Configuration 0, Set PAD Driving capability
    #define ASPDF_SHFT          (30)
    #define AOSD_SHFT           (28)
    #define AOLRCK_SHFT         (26)
    #define AOBCK_SHFT          (24)
    #define AOMCLK_SHFT         (22)
    #define PADDRV_ASPDF        (3U << ASPDF_SHFT)
    #define PADDRV_AOSD         (3U << AOSD_SHFT)
    #define PADDRV_AOLRCK       (3U << AOLRCK_SHFT)
    #define PADDRV_AOBCK        (3U << AOBCK_SHFT)
    #define PADDRV_AOMCLK       (3U << AOMCLK_SHFT)
    #define PADDRV_GPIO         (3U << 14)  // GPIO group
/*
    #define PADDRV_JTAG         (3U << 0)   // JTAG group
    #define PADDRV_PBIC         (3U << 2)   // PBI Control group
    #define PADDRV_PBID         (3U << 4)   // PBI Data group
    #define PADDRV_PBIA         (3U << 6)   // PBI Address group
    #define PADDRV_POD          (3U << 8)   // POD group
    #define PADDRV_UART         (3U << 10)  // UART group
    #define PADDRV_IIC          (3U << 12)  // IIC group
    #define PADDRV_PWM          (3U << 14)  // PWM group
    #define PADDRV_IR           (3U << 16)  // IR group
    #define PADDRV_I1394D       (3U << 18)  // I1394 Data group
    #define PADDRV_I1394C       (3U << 20)  // I1394 Control group
    #define PADDRV_FCI          (3U << 22)  // FCI gorup
    #define PADDRV_SDIO         (3U << 24)  // SDIO group
    #define PADDRV_TS           (3U << 26)  // Transport Stream group
    #define PADDRV_GPIO         (3U << 28)  // GPIO group
    #define PADDRV_DVI          (3U << 30)  // DVI group
*/


//=====================================================================
// Constant definitions Given by ACD analog designer
#define PLL_BAND_NUM           8


//----------------------------------------------------------------------------
// clock const definitions
#define CLK_13_5MHZ             13500000    // 13.5 MHz
#define CLK_20MHZ               20000000    // 20 MHz
#define CLK_25MHZ               25000000    // 25 MHz
#define CLK_27MHZ               27000000    // 27 MHz
#define CLK_30MHZ               30000000    // 30 MHz
#define CLK_40MHZ               40000000    // 40 MHz
#define CLK_48MHZ               48000000    // 48 MHz
#define CLK_50MHZ               50000000    // 50 MHz
#define CLK_54MHZ               54000000    // 54 MHz
#define CLK_60MHZ               60000000    // 60 MHz
#define CLK_81MHZ               81000000    // 81 MHz
#define CLK_100MHZ              100000000   // 100 MHz
#define CLK_120MHZ              120000000   // 120 MHz
#define CLK_200MHZ              200000000   // 200 MHz
#define CLK_216MHZ              216000000   // 216 MHz
#define CLK_231_43MHZ           231430000   // 231.43 MHz
#define CLK_243MHZ              243000000   // 243 MHz
#define CLK_246_86MHZ           246860000   // 246.86 MHz
#define CLK_259_2MHZ            259200000   // 259.2 MHz
#define CLK_324MHZ              324000000   // 324 MHz
#define CLK_337_5MHZ            337500000   // 337.5 MHz
#define CLK_351MHZ              351000000   // 351 MHz
#define CLK_405MHZ              405000000   // 405 MHz
#define CLK_432MHZ              432000000   // 432 MHz
#define CLK_540MHZ              540000000   // 540 MHz
#define CLK_600MHZ              600000000   // 600 MHz
#define CLK_APLL294MHZ          294912000   // 294.912 MHz
#define CLK_APLL270MHZ          270950400   // 270.9404 MHz

//===========================================================================
// PLL default clock settings

#ifndef DMPLL_DEFAULT_CLOCK
#define DMPLL_DEFAULT_CLOCK     CLK_337_5MHZ  // M_CLK is 168.75 MHz
#endif /* DMPLL_DEFAULT_CLOCK */

#ifndef CPUPLL_DEFAULT_CLOCK
#define CPUPLL_DEFAULT_CLOCK    CLK_324MHZ  // CPU_CK is 162 MHZ
#endif /* CPUPLL_DEFAULT_CLOCK */

#define SYSPLL_DEFAULT_CLOCK    CLK_324MHZ  // SYSPLL 324 MHZ
#define APLL1_DEFAULT_CLOCK     CLK_APLL294MHZ  // 270.9504M
#define PSPLL_DEFAULT_CLOCK     148500000   // 148.5 MHz
#define VPLL_DEFAULT_CLOCK      CLK_81MHZ   // 81 MHz
#define ADCPLL_DEFAULT_CLOCK    CLK_81MHZ   // 81 MHz
#define B2RPLL_DEFAULT_CLOCK    148352000   // 148.352M
#define APLL2_DEFAULT_CLOCK     CLK_APLL270MHZ  // 294.912M

//=====================================================================
// Type definitions

typedef enum
{
    CAL_SRC_APLL1,
    CAL_SRC_DMPLL,
    CAL_SRC_TVDPLL,
    CAL_SRC_DTDPLL,
    CAL_SRC_PSPLL,
    CAL_SRC_VOPLL,
    CAL_SRC_ADCPLL,
    CAL_SRC_SYSPLL,
    CAL_SRC_APLL2,
    CAL_SRC_HDMIPLL,
    CAL_SRC_CPUPLL,
    CAL_SRC_DSPPLL,
    CAL_SRC_USBPLL,
    CAL_SRC_HAPLL,
    CAL_8297_SYSPLL1,
    CAL_8297_SYSPLL2,
    CAL_8297_APLL1,
    CAL_8297_APLL2,
    SRC_CPU_CLK,
    SRC_MEM_CLK,
    SRC_BUS_CLK
} CAL_SRC_T;

enum CLK_8297_SEL
{
    SYSPLL1_8297_D1_CLK = 0,
    SYSPLL1_8297_D2_CLK = 1,
    SYSPLL2_8297_D1_CLK = 2,
    SYSPLL2_8297_D2_CLK = 3,
    APLL1_8297_D1_CLK = 4,
    APLL1_8297_D2_CLK = 5,
    APLL2_8297_D1_CLK = 6,
    APLL2_8297_D2_CLK = 7,
    VADC_8297_D1_CLK = 8,
    VADC_8297_D2_CLK = 9,
    SYS270_8297_D1_CLK = 10,
    SYS270_8297_D2_CLK = 11,
};

enum CLK_SEL
{
    TVDPLL_D3_CLK = 0x00,
    DTDPLL_D3_CLK = 0x01,
    SYSPLL_D4_CLK = 0x02,
    PSPLL_D1_CLK = 0x03,
    CPUPLL_D3_CLK = 0x04,
    VPLL_D1_CLK = 0x05,
    ADCPLL_D1_CLK = 0x06,
    M_CLK_D1_CLK = 0x07,
    HDMIPLL_D4_CLK = 0x08,
    DSPPLL_D1_CLK = 0x09,
    USBPLL_D1_CLK = 0x0A,
    FBOUT_D1_CLK = 0x0B,
    // Below is reserved number. just for backward compatible.
    HDMI_FBOUT_D1_CLK = 0x0C,
    HDMI_CKOUT_D1_CLK = 0x0D,
    APLL2_D1_CLK = 0x0E,
    APLL1_D1_CLK = 0x0F,

    TVDPLL_D2_CLK = 0x10,
    DTDPLL_D2_CLK = 0x11,
    SYSPLL_D2_CLK = 0x12,
    PSPLL_D2_CLK = 0x13,
    CPUPLL_D2_CLK = 0x14,
    VPLL_D2_CLK = 0x15,
    ADCPLL_D2_CLK = 0x16,
    M_CLK_D2_CLK = 0x17,
    HDMIPLL_D2_CLK = 0x18,
    DSPPLL_D2_CLK = 0x19,
    USBPLL_D2_CLK = 0x1A,
    CKOUT_D1_CLK = 0x1B,
    //  Below is reserved number. just for backward compatible
    HDMI_FBOUT_D2_CLK = 0x1C,
    HDMI_CKOUT_D2_CLK = 0x1D,
    APLL2_D2_CLK = 0x1E,
    APLL1_D2_CLK = 0x1F,
    LAST_CLK_ITEM = 100
};


//=====================================================================
// Interface

#define BSP_GetDomainClock(eSrc)    BSP_GetClock(eSrc, NULL, NULL, NULL, NULL)
#define BSP_GetPLLClock(eSrc)       BSP_GetClock(eSrc, NULL, NULL, NULL, NULL)

EXTERN UINT32 CKGEN_GetSlowClock(VOID);
EXTERN UINT32 CKGEN_GetBistClock(VOID);
EXTERN UINT32 CKGEN_GetXtalClock(VOID);
EXTERN VOID BSP_CkgenInit(VOID);

EXTERN BOOL BSP_Calibrate(CAL_SRC_T eSource, UINT32 u4Clock);

EXTERN UINT32 BSP_GetClock(CAL_SRC_T eSrc, UINT8* pu1Band, UINT16* pu2MS,
    UINT16* pu2NS, UINT16* pu2Counter);

EXTERN VOID vDrvVOPLLSet(UINT32 u4CLK);
EXTERN VOID vDrvADCPLLSetFreq(UINT8 bADCCLK);
EXTERN VOID CKGEN_SetVOPLLFreq(UINT32 u4Freq);
EXTERN VOID CKGEN_SetVOPLLInputAsDDDS(BOOL fgDDDS);
EXTERN VOID CKGEN_SetADCPLLFreq(UINT32 u4Freq);
EXTERN VOID CKGEN_SetPSPLLMainClk(UINT32 u4Multiplier, UINT32 u4Divisor);
EXTERN VOID CKGEN_SetPSPLLFreq(UINT32 u4Freq);

#endif  // X_CKGEN_H

