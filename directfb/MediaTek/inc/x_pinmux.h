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

/** @file pinmux.h
 *  pinmux.h declare all pinmux/gpio private related function and parameters.
 */


#ifndef X_PINMUX_H
#define X_PINMUX_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_ckgen.h"
#include "x_lint.h"
#include "x_gpio.h"

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define PINMUX0_OFFSET          REG_PINMUX_SEL0
#define PINMUX1_OFFSET          REG_PINMUX_SEL1
#define PINMUX2_OFFSET          REG_PINMUX_SEL2
#define PINMUX3_OFFSET          REG_PINMUX_SEL3
#define PINMUX4_OFFSET          REG_PINMUX_SEL4
#define PINMUX_MISC_OFFSET      REG_PINMUX_MISC

#define GPIO_OUT0_OFFSET        REG_GPIO_OUT0
#define GPIO_EN0_OFFSET         REG_GPIO_EN0
#define GPIO_IN0_OFFSET         REG_GPIO_IN0
#define GPIO_RQ0_OFFSET         REG_GPIO_INTR0
#define GPIO_RISE0_OFFSET       REG_GPIO_RISING_INTR0
#define GPIO_FALL0_OFFSET       REG_GPIO_FALLING_INTR0
#define GPIO_EDGE0_OFFSET       REG_GPIO_EDGE_ST_INTR0

#define PINMUX_FUNCTION0        0
#define PINMUX_FUNCTION1        1
#define PINMUX_FUNCTION2        2
#define PINMUX_FUNCTION3        3
#define PINMUX_FUNCTION4        4
#define PINMUX_FUNCTION5        5
#define PINMUX_FUNCTION6        6
#define PINMUX_FUNCTION7        7

//===========================================================================
// Macros for register read/write

#define PINMUX0_WRITE(value)            CKGEN_WRITE32(PINMUX0_OFFSET, (value))
#define PINMUX1_WRITE(value)            CKGEN_WRITE32(PINMUX1_OFFSET, (value))
#define PINMUX2_WRITE(value)            CKGEN_WRITE32(PINMUX2_OFFSET, (value))
#define PINMUX_MISC_WRITE(value)        CKGEN_WRITE32(PINMUX_MISC_OFFSET, (value))
#define PINMUX_WRITE(idx, value)        CKGEN_WRITE32((PINMUX0_OFFSET+(idx << 2)), (value))
#define PINMUX0_REG()                   CKGEN_REG32(PINMUX0_OFFSET)
#define PINMUX1_REG()                   CKGEN_REG32(PINMUX1_OFFSET)
#define PINMUX2_REG()                   CKGEN_REG32(PINMUX2_OFFSET)
#define PINMUX_MISC_REG()               CKGEN_REG32(PINMUX_MISC_OFFSET)
#define PINMUX_REG(idx)                 CKGEN_REG32((PINMUX0_OFFSET+(idx << 2)))

#define GPIO_OUT_WRITE(idx,val)         CKGEN_WRITE32((GPIO_OUT0_OFFSET+(4*(idx))), (val))
#define GPIO_EN_WRITE(idx,val)          CKGEN_WRITE32((GPIO_EN0_OFFSET+(4*(idx))), (val))
#define GPIO_IN_WRITE(idx,val)          CKGEN_WRITE32((GPIO_IN0_OFFSET+(4*(idx))), (val))
#define GPIO_RQ_WRITE(idx,val)          CKGEN_WRITE32((GPIO_RQ0_OFFSET+(4*(idx))), (val))
#define GPIO_OUT_REG(idx)               CKGEN_REG32(GPIO_OUT0_OFFSET+(4*(idx)))
#define GPIO_EN_REG(idx)                CKGEN_REG32(GPIO_EN0_OFFSET+(4*(idx)))
#define GPIO_IN_REG(idx)                CKGEN_REG32(GPIO_IN0_OFFSET+(4*(idx)))
#define GPIO_RQ_REG(idx)                CKGEN_REG32(GPIO_RQ0_OFFSET+(4*(idx)))

#define GPIO_RISE_WRITE(idx,val)        CKGEN_WRITE32((GPIO_RISE0_OFFSET+(4*(idx))), (val))
#define GPIO_FALL_WRITE(idx,val)        CKGEN_WRITE32((GPIO_FALL0_OFFSET+(4*(idx))), (val))
#define GPIO_EDGE_WRITE(idx,val)        CKGEN_WRITE32((GPIO_EDGE0_OFFSET+(4*(idx))), (val))
#define GPIO_RISE_REG(idx)              CKGEN_REG32(GPIO_RISE0_OFFSET+(4*(idx)))
#define GPIO_FALL_REG(idx)              CKGEN_REG32(GPIO_FALL0_OFFSET+(4*(idx)))
#define GPIO_EDGE_REG(idx)              CKGEN_REG32(GPIO_EDGE0_OFFSET+(4*(idx)))

//=====================================================================
// Register definitions

#define MX0                     (32*0)
#define MX1                     (32*1)
#define MX2                     (32*2)
#define MX3                     (32*3)
#define MX4                     (32*4)
#define PDMX                    (32*5)
#define PDGPIO                  (32*6)

#define PINMUX_NO_PIN           (31 + MX4)  // no pin
#define PINMUX_GPIO_00          (16 + MX2)  // GPIO 0
#define PINMUX_GPIO_01          (16 + MX2)  // GPIO 1
#define PINMUX_GPIO_02          (16 + MX2)  // GPIO 2
#define PINMUX_GPIO_03          (16 + MX2)  // GPIO 3
#define PINMUX_GPIO_04          (18 + MX2)  // GPIO 4
#define PINMUX_GPIO_05          (21 + MX2)  // GPIO 5
#define PINMUX_GPIO_06          (24 + MX2)  // GPIO 6
#define PINMUX_GPIO_07          (27 + MX2)  // GPIO 7
#define PINMUX_GPIO_08          (27 + MX2)  // GPIO 8
#define PINMUX_GPIO_09          (27 + MX2)  // GPIO 9
#define PINMUX_GPIO_10          (27 + MX2)  // GPIO 10
#define PINMUX_GPIO_11          (29 + MX2)  // GPIO 11
#define PINMUX_GPIO_12          (29 + MX2)  // GPIO 12
#define PINMUX_GPIO_13          (0 + MX3)   // GPIO 13
#define PINMUX_GPIO_14          (2 + MX3)   // GPIO 14
#define PINMUX_GPIO_15          (4 + MX3)   // GPIO 15
#define PINMUX_GPIO_16          (4 + MX3)   // GPIO 16
#define PINMUX_GPIO_17          (6 + MX3)   // GPIO 17
#define PINMUX_GPIO_18          (9 + MX3)   // GPIO 18
#define PINMUX_GPIO_19          (0 + MX4)   // GPIO 19
#define PINMUX_GPIO_20          (4 + MX4)   // GPIO 20
#define PINMUX_GPIO_21          (8 + MX4)   // GPIO 21
#define PINMUX_GPIO_22          (12 + MX4)  // GPIO 22
#define PINMUX_GPIO_23          (12 + MX3)  // GPIO 23
#define PINMUX_GPIO_24          (12 + MX3)  // GPIO 24
#define PINMUX_GPIO_25          (12 + MX3)  // GPIO 25
#define PINMUX_GPIO_26          (14 + MX3)  // GPIO 26
#define POCE0_O                 (2 + MX0)   // GPIO 28
#define POCE1_O                 (4 + MX0)   // GPIO 29
#define POOE_O                  (6 + MX0)   // GPIO 30
#define PARB_I                  (8 + MX0)   // GPIO 31
#define POWE_O                  (10 + MX0)  // GPIO 32
#define PALE_O                  (10 + MX0)  // GPIO 33
#define PCLE_O                  (10 + MX0)  // GPIO 34
#define PDD0_IO                 (12 + MX0)  // GPIO 35
#define PDD1_IO                 (12 + MX0)  // GPIO 36
#define PDD2_IO                 (14 + MX0)  // GPIO 37
#define PDD3_IO                 (14 + MX0)  // GPIO 38
#define PDD4_IO                 (14 + MX0)  // GPIO 39
#define PDD5_IO                 (14 + MX0)  // GPIO 40
#define PDD6_IO                 (14 + MX0)  // GPIO 41
#define PDD7_IO                 (14 + MX0)  // GPIO 42
#define U0RX_I                  (6 + PDMX)  // no gpio
#define U0RX_O                  (6 + PDMX)  // no gpio
#define U2RX_I                  (16 + MX0)  // GPIO 43
#define U2TX_O                  (16 + MX0)  // GPIO 44
#define U2CTS_I                 (18 + MX0)  // GPIO 45
#define U2RTS_O                 (18 + MX0)  // GPIO 46
#define OSDA0_IO                (20 + MX0)  // GPIO 47
#define OSCL0_O                 (20 + MX0)  // GPIO 48
#define OSDA1_IO                (22 + MX0)  // GPIO 49
#define OSCL1_I                 (22 + MX0)  // GPIO 50
// #define OSDA2_IO             XX      // Single Function
// #define OSCL2_O              XX      // Single Function
#define HDCP_SDA_IO             (24 + MX0)  // GPIO 51
#define HDCP_SCL_O              (24 + MX0)  // GPIO 52
#define HDCP_WRP_O              (24 + MX0)  // GPIO 53
#define OPWM0_O                 (4 + PDMX)  // OPCTRL 12
#define OPWM1_O                 (26 + MX0)  // GPIO 54
#define OPWM2_O                 (28 + MX0)  // GPIO 55
// #define OIRI_I               XX      // Single Function
// #define OPWRSB_O             XX      // Single Function
#define PINMUX_OPCTRL_0         (0 + PDMX)  // OPCTRL 0 and CEC_IO
#define PINMUX_OPCTRL_1         (31 + PDMX) // OPCTRL 1 XXX set bit 31 of PDMX zero
#define PINMUX_OPCTRL_2         (31 + PDMX) // OPCTRL 2 XXX set bit 31 of PDMX zero
#define PINMUX_OPCTRL_3         (31 + PDMX) // OPCTRL 3 XXX set bit 31 of PDMX zero
#define PINMUX_OPCTRL_4         (31 + PDMX) // OPCTRL 4 XXX set bit 31 of PDMX zero
#define PINMUX_OPCTRL_5         (31 + PDMX) // OPCTRL 5 XXX set bit 31 of PDMX zero
#define PINMUX_OPCTRL_6         (PDGPIO)    // OPCTRL 6 and U2RTS_O
#define PINMUX_OPCTRL_7         (PDGPIO)    // OPCTRL 7 and USCRS_I
#define PINMUX_OPCTRL_8         (31 + PDMX) // OPCTRL 8 XXX set bit 31 of PDMX zero
#define OPWR0_5V                (1 + PDMX)  // OPCTRL 9
#define OPWR1_5V                (2 + PDMX)  // OPCTRL 10
#define OPWR2_5V                (3 + PDMX)  // OPCTRL 11
#define SDIOCLK_O               (30 + MX0)  // GPIO 56
#define SDIOCMD_O               (30 + MX0)  // GPIO 57
#define SDIOD0_O                (30 + MX0)  // GPIO 58
#define SDIOD1_O                (30 + MX0)  // GPIO 59
#define SDIOD2_O                (30 + MX0)  // GPIO 60
#define SDIOD3_O                (30 + MX0)  // GPIO 61
#define TUNER_CLK_O             (0 + MX1)   // GPIO 62
#define TUNER_DATA_IO           (0 + MX1)   // GPIO 63
#define IF_AGC_O                (2 + MX1)   // GPIO 64
#define TS0_CLK_I               (4 + MX1)   // GPIO 65
#define TS0_VALID_I             (4 + MX1)   // GPIO 66
#define TS0_SYNC_I              (4 + MX1)   // GPIO 67
#define TS0_DATA_I              (4 + MX1)   // GPIO 68
#define AO0P                    (6 + MX1)   // GPIO 69
#define AO0N                    (6 + MX1)   // GPIO 70
#define AO1P                    (6 + MX1)   // GPIO 71
#define AO1N                    (6 + MX1)   // GPIO 72
#define AO2P                    (6 + MX1)   // GPIO 73
#define AO2N                    (6 + MX1)   // GPIO 74
#define AOCKP                   (6 + MX1)   // GPIO 75
#define AOCKN                   (6 + MX1)   // GPIO 76
#define AO3P                    (6 + MX1)   // GPIO 77
#define AO3N                    (6 + MX1)   // GPIO 78
#define AO4P                    (6 + MX1)   // GPIO 79
#define AO4N                    (6 + MX1)   // GPIO 80
#define AE0P                    (8 + MX1)   // GPIO 81
#define AE0N                    (8 + MX1)   // GPIO 82
#define AE1P                    (8 + MX1)   // GPIO 83
#define AE1N                    (8 + MX1)   // GPIO 84
#define AE2P                    (8 + MX1)   // GPIO 85
#define AE2N                    (8 + MX1)   // GPIO 86
#define AECKP                   (8 + MX1)   // GPIO 87
#define AECKN                   (8 + MX1)   // GPIO 88
#define AE3P                    (8 + MX1)   // GPIO 89
#define AE3N                    (8 + MX1)   // GPIO 90
#define AE4P                    (8 + MX1)   // GPIO 91
#define AE4N                    (8 + MX1)   // GPIO 92
#define BO0P                    (10 + MX1)  // GPIO 93
#define BO0N                    (10 + MX1)  // GPIO 94
#define BO1P                    (10 + MX1)  // GPIO 95
#define BO1N                    (10 + MX1)  // GPIO 96
#define BO2P                    (12 + MX1)  // GPIO 97
#define BO2N                    (12 + MX1)  // GPIO 98
#define BOCKP                   (12 + MX1)  // GPIO 99
#define BOCKN                   (12 + MX1)  // GPIO 100
#define BO3P                    (12 + MX1)  // GPIO 101
#define BO3N                    (12 + MX1)  // GPIO 102
#define BO4P                    (12 + MX1)  // GPIO 103
#define BO4N                    (12 + MX1)  // GPIO 104
#define BE0P                    (14 + MX1)  // GPIO 105
#define BE0N                    (14 + MX1)  // GPIO 106
#define BE1P                    (14 + MX1)  // GPIO 107
#define BE1N                    (14 + MX1)  // GPIO 108
#define BE2P                    (14 + MX1)  // GPIO 109
#define BE2N                    (14 + MX1)  // GPIO 110
#define BECKP                   (14 + MX1)  // GPIO 111
#define BECKN                   (14 + MX1)  // GPIO 112
#define BE3P                    (14 + MX1)  // GPIO 113
#define BE3N                    (14 + MX1)  // GPIO 114
#define BE4P                    (14 + MX1)  // GPIO 115
#define BE4N                    (14 + MX1)  // GPIO 116
#define ONDA_00_O               (16 + MX1)  // GPIO 117
#define ONDA_01_O               (16 + MX1)  // GPIO 118
#define ONDA_02_O               (16 + MX1)  // GPIO 119
#define ONDA_03_O               (16 + MX1)  // GPIO 120
#define ONDA_04_O               (16 + MX1)  // GPIO 121
#define ONDA_05_O               (16 + MX1)  // GPIO 122
#define ONDA_06_O               (20 + MX1)  // GPIO 123
#define ONDA_07_O               (20 + MX1)  // GPIO 124
#define ONDA_08_O               (24 + MX1)  // GPIO 125
#define ONDA_09_O               (28 + MX1)  // GPIO 126
#define ONDA_10_O               (28 + MX1)  // GPIO 127
#define ONDA_11_O               (0 + MX2)   // GPIO 128
#define ONDA_12_O               (0 + MX2)   // GPIO 129
#define ONDA_13_O               (0 + MX2)   // GPIO 130
#define ONDD_08_IO              (28 + MX1)  // GPIO 131
#define ONDD_09_IO              (28 + MX1)  // GPIO 132
#define ONDD_10_IO              (0 + MX2)   // GPIO 133
#define ONDD_11_IO              (0 + MX2)   // GPIO 134
#define ONDD_12_IO              (28 + MX1)  // GPIO 135
#define ONDD_13_IO              (28 + MX1)  // GPIO 136
#define ONDD_14_IO              (0 + MX2)   // GPIO 137
#define ONDD_15_IO              (28 + MX1)  // GPIO 138
#define ONDOE_O                 (4 + MX2)   // GPIO 139
#define ONDCE0_O                (8 + MX2)   // GPIO 140
#define ONDCE1_O                (12 + MX2)  // GPIO 141
#define ADPDIF_OUT_O            (17 + MX3)  // GPIO 142
#define VCXO_I                  (18 + MX3)  // GPIO 143
#define LDOSDA0_IO              (20 + MX3)  // GPIO 144
#define LDOSCL0_O               (20 + MX3)  // GPIO 145
#define LDMCLK0_IO              (22 + MX3)  // GPIO 146
#define LDLRCK0_IO              (22 + MX3)  // GPIO 147
#define LDBCK0_IO               (22 + MX3)  // GPIO 148
#define LDMCLK1_IO              (22 + MX3)  // GPIO 149
#define LDLRCK1_IO              (22 + MX3)  // GPIO 150
#define LDBCK1_IO               (22 + MX3)  // GPIO 151
#define LDSDATA0_O              (22 + MX3)  // GPIO 152
#define LDSDATA1_O              (22 + MX3)  // GPIO 153
#define LDSDATA2_O              (22 + MX3)  // GPIO 154
#define LDAIN_I                 (23 + MX3)  // GPIO 155
#define CVBS_DO0_I              (24 + MX3)  // GPIO 156
#define CVBS_DO0A_I             (24 + MX3)  // GPIO 157
#define CVBS_DO1_I              (24 + MX3)  // GPIO 158
#define CVBS_DO1A_I             (24 + MX3)  // GPIO 159
#define CVBS_DO2_I              (24 + MX3)  // GPIO 160
#define CVBS_DO2A_I             (24 + MX3)  // GPIO 161
#define CVBS_DO3_I              (24 + MX3)  // GPIO 162
#define CVBS_DO3A_I             (24 + MX3)  // GPIO 163
#define CVBS_DO4_I              (24 + MX3)  // GPIO 164
#define CVBS_DO4A_I             (24 + MX3)  // GPIO 165
#define CVBS_IDX_I              (24 + MX3)  // GPIO 166
#define CVBS_CLK_I              (24 + MX3)  // GPIO 167
#define CVBS_IF0_O              (24 + MX3)  // GPIO 168
#define CVBS_IF1_O              (24 + MX3)  // GPIO 169
#define CVBS_IF2_O              (24 + MX3)  // GPIO 170
#define CVBS_IF3_O              (24 + MX3)  // GPIO 171
#define DEMOD_DO0_I             (25 + MX3)  // GPIO 172
#define DEMOD_DO0A_I            (25 + MX3)  // GPIO 173
#define DEMOD_DO1_I             (25 + MX3)  // GPIO 174
#define DEMOD_DO1A_I            (25 + MX3)  // GPIO 175
#define DEMOD_DO2_I             (25 + MX3)  // GPIO 176
#define DEMOD_DO2A_I            (25 + MX3)  // GPIO 177
#define DEMOD_DO3_I             (25 + MX3)  // GPIO 178
#define DEMOD_DO3A_I            (25 + MX3)  // GPIO 179
#define DEMOD_DO4_I             (25 + MX3)  // GPIO 180
#define DEMOD_DO4A_I            (25 + MX3)  // GPIO 181
#define DEMOD_DO5_I             (25 + MX3)  // GPIO 182
#define DEMOD_DO5A_I            (25 + MX3)  // GPIO 183
#define DEMOD_CLK_I             (25 + MX3)  // GPIO 184
#define DEMOD_HWIND_I           (25 + MX3)  // GPIO 185
#define DEMOD_ADSFSEL_O         (26 + MX3)  // GPIO 186
#define JTMS_I                  (0 + MX0)   // GPIO 187
#define JTDI_I                  (0 + MX0)   // GPIO 188
#define JTDO_O                  (0 + MX0)   // GPIO 189
#define JTCK_I                  (0 + MX0)   // GPIO 190
#define JTRST_I                 (0 + MX0)   // GPIO 191

#if 0
#define OSDA3_IO                6       // GPIO 44
#define OSCL3_O                 6       // GPIO 45
#define ALIN_I                  60      // GPIO 46
#define AOMCLK_IO               64      // GPIO 47
#define AOLRCK_O                64      // GPIO 48
#define AOBCK_O                 64      // GPIO 49
#define AOSDATA0_O              66      // GPIO 50
#define AOSDATA1_O              68      // GPIO 51
#define AOSDATA2_O              70      // GPIO 52
#define AOSDATA3_O              72      // GPIO 53
#define AOSDATA4_O              74      // GPIO 54
#define ASPDIF                  96      // GPIO 55
#define ATN_TX                 100      // 56+
#define ATN_RX                 100      // 57+
#define ATN_DET                100      // 58+
#define RG_AGC                 104      // 62+
#define AL1                     76      // 63o
#define AR1                     80      // 64o
#define AL2                     84      // 65o
#define AR2                     88      // 66o
#define VCXO                    99      // 67+
#define AIN0_L                 108      // 68i+
#define AIN0_R                 108      // 69i+
#define AIN1_L                 109      // 70i+
#define AIN1_R                 109      // 71i+
#define AIN2_L                 110      // 72i+
#define AIN2_R                 110      // 73i+
#define AIN3_L                 111      // 74i+
#define AIN3_R                 111      // 75i+
#define AIN4_L                 112      // 76i+
#define AIN4_R                 112      // 77i+
#define AIN5_L                 113      // 78i+
#define AIN5_R                 113      // 79i+
#define AIN6_L                 114      // 80i+
#define AIN6_R                 114      // 81i+
#define AIN7_L                 115      // 82i+
#define AIN7_R                 115      // 83i+
#define A0P                     34      // 84+
#define A0N                     34      // 85+
#define A1P                     34      // 86+
#define A1N                     34      // 87+
#define A2P                     34      // 88+
#define A2N                     34      // 89+
#define CK1P                    34      // 90+
#define CK1N                    34      // 91+
#define A3P                     34      // 92+
#define A3N                     34      // 93+
#define A4P                     37      // 94+
#define A4N                     37      // 95+
#define A5P                     37      // 96+
#define A5N                     37      // 97+
#define A6P                     37      // 98+
#define A6N                     37      // 99+
#define CK2P                    37      // 100+
#define CK2N                    37      // 101+
#define A7P                     37      // 102+
#define A7N                     37      // 103+
#define TP2                     62      // 104+
#define TN2                     62      // 105+
#endif

// Total GPIO is 192


//-----------------------------------------------------------------------------
// Public function declaration
//-----------------------------------------------------------------------------

/* Pinmux function. */
extern INT32 BSP_PinSet(INT32 i4PinSel, INT32 i4Func);
extern INT32 BSP_PinGet(INT32 i4PinSel);

/* GPIO functions. */
extern INT32 GPIO_Init(void);
extern INT32 GPIO_Stop(void);

/* Dimming control function. */
extern INT32 GPIO_SetDimming(UINT32 u4Hz, UINT32 u4Duty);

#endif // X_PINMUX_H
