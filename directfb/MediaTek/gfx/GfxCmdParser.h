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
//-------------------------------------
// Parse gfx command queue
//      (Use VC++ 6.0)
//-------------------------------------
#ifndef GFXCMDPARSER_H
#define GFXCMDPARSER_H

//-------------------------------------
// common definition
#define GFX_CMD_SIZE    8
#define GFX_BASE_ADDR   0x20004000
#define GFX_BSAD_MASK   0xfffff000
#define MEM_DUMP_FNAME  "cmddump_0512_v2_range.bin"
#define REPORT_FNAME    "cmdque_parse_report.txt"
#define UNKNOWN_FNAME   "unknown_cmd.txt"

//-------------------------------------
// gfx operation
enum GFX_OP_MODE
{
	OP_TEXT_BITMAP = 1,
	OP_RECT_FILL,
	OP_HOR_LINE,
	OP_VER_LINE,
	OP_GRAD_FILL,
	OP_BITBLT,
	OP_DMA,
	OP_ALPHA_BLENDING,
	OP_ALPHA_COMPOSE,
	OP_YCBCR_TO_RGB,
	OP_STRETCH,
	OP_ALPHA_MAP
};

//-------------------------------------
// gfx register address
enum GFX_REG_ADDR
{
	G_CONFIG = 0x00,
	G_STATUS = 0x04,
	DRAMQ_STAD = 0x08,
	DRAMQ_LEN = 0x0C,
	G_MODE = 0x10,
	RECT_COLOR = 0x14,
	SRC_BSAD = 0x18,
	DST_BSAD = 0x1C,
	SRC_XY = 0x20,
	DST_XY = 0x24,
	SRC_SIZE = 0x28,
	S_OSD_WIDTH = 0x2C,
	CLIP_BR = 0x30,
	CLIP_TL = 0x34,
	GRAD_X_DELTA = 0x38,
	GRAD_Y_DELTA = 0x3C,
	GRAD_XY_INC = 0x40,
	BITBLT_MODE = 0x44,
	KEY_DATA0 = 0x48,
	KEY_DATA1 = 0x4C,
	SRCCBCR_BSAD = 0x50,
	SRCCBCR_PITCH = 0x54,
	DSTCBCR_BSAD = 0x58,
	F_COLOR = 0x5C,
	B_COLOR = 0x60,
	COLOR_TRAN0 = 0x64,
	COLOR_TRAN1 = 0x68,
	COLOR_TRAN2 = 0x6C,
	COLOR_TRAN3 = 0x70,
	COLOR_TRAN4 = 0x74,
	COLOR_TRAN5 = 0x78,
	COLOR_TRAN6 = 0x7C,
	COLOR_TRAN7 = 0x80,
	STR_BLT_H = 0x84,
	STR_BLT_V = 0x88,
	STR_DST_SIZE = 0x8C,
	DUMMY = 0x98
};

//-------------------------------------
// gfx register field mask
enum GFX_REG_FIELD_MASK
{
	// G_CONFIG
	// ???

	// G_STATUS
	// ???

	// DRAMQ_STAD
	// ???

	//DRAMQ_LEN
	// ???

	// G_MODE
	M_CM = 0x0000000f,
	M_OP_MODE = 0x000001f0,
	M_FIRE = 0x00000800,

	// RECT_COLOR
	M_RECT_COLOR = 0xffffffff,

	// SRC_BSAD
	M_SRC_BSAD = 0x0fffffff,
	M_CHAR_CM = 0xc0000000,

	// DST_BSAD
	M_DST_BSAD = 0x0fffffff,

	// SRC_XY
	// ???

	// DST_XY
	// ???

	// SRC_SIZE
	M_SRC_WIDTH = 0x00007fff,
	M_RL_DEC = 0x00008000,
	M_SRC_HEIGHT = 0x07ff0000,
	M_SRC_CM = 0x78000000,

	// S_OSD_WIDTH
	M_OSD_WIDTH = 0x0000ffff,
	M_SRC_PITCH = 0xffff0000,

	// CLIP_BR
	// ???

	// CLIP_TL
	// ???

	// GRAD_X_DELTA
	M_DELTA_X_C0 = 0x000000ff,
	M_DELTA_X_C1 = 0x0000ff00,
	M_DELTA_X_C2 = 0x00ff0000,
	M_DELTA_X_C3 = 0xff000000,

	// GRAD_Y_DELTA
	M_DELTA_Y_C0 = 0x000000ff,
	M_DELTA_Y_C1 = 0x0000ff00,
	M_DELTA_Y_C2 = 0x00ff0000,
	M_DELTA_Y_C3 = 0xff000000,

	//GRAD_XY_INC
	M_GRAD_X_PIX_INC = 0x000007ff,
	M_GRAD_Y_PIX_INC = 0x07ff0000,
	M_GRAD_MODE = 0xc0000000,

	// BITBLT_MODE
	M_TRANS_ENA = 0x00000001,
	M_KEYNOT_ENA = 0x00000002,
	M_COLCHG_ENA = 0x00000004,
	M_CLIP_ENA = 0x00000008,
	M_CFMT_ENA = 0x00000010,
	M_BLT_4BPP = 0x00000020,
	M_ALPHA_VALUE = 0x0000ff00,
	M_ALCOM_PASS = 0x00070000,

	// KEY_DATA0
	M_COLOR_KEY_MIN = 0xffffffff,

	// KEY_DATA1
	M_COLOR_KEY_MAX = 0xffffffff,

	// SRCCBCR_BSAD
	M_SRCCBCR_BSAD = 0x0fffffff,
	M_YC_FMT = 0xc0000000,

	// SRCCBCR_PITCH
	M_SRCCBCR_PITCH = 0x0000ffff,
	M_VSTD = 0x00010000,
	M_VSYS = 0x00020000,
	M_VSCLIP = 0x00040000,
	M_FLD_PIC = 0x00080000,
	M_SWAP_MODE = 0x00300000,

	// DSTCBCR_BSAD
	M_DSTCBCR_BSAD = 0x0fffffff,

	// F_COLOR
	M_FORE_COLOR = 0xffffffff,

	// B_COLOR
	M_BACK_COLOR = 0xffffffff,

	// COLOR_TRAN0
	M_COLOR_TRAN0 = 0xffffffff,

	// COLOR_TRAN1
	M_COLOR_TRAN1 = 0xffffffff,

	// COLOR_TRAN2
	M_COLOR_TRAN2 = 0xffffffff,

	// COLOR_TRAN3
	M_COLOR_TRAN3 = 0xffffffff,

	// COLOR_TRAN4
	M_COLOR_TRAN4 = 0xffffffff,

	// COLOR_TRAN5
	M_COLOR_TRAN5 = 0xffffffff,

	// COLOR_TRAN6
	M_COLOR_TRAN6 = 0xffffffff,

	// COLOR_TRAN7
	M_COLOR_TRAN7 = 0xffffffff,

	// STR_BLT_H
	M_STR_BLT_H = 0x00ffffff,

	// STR_BLT_V
	M_STR_BLT_V = 0x00ffffff,

	// STR_DST_SIZE
	M_STR_DST_WIDTH = 0x00007fff,
	M_STR_DST_HEIGHT = 0x07ff0000,

	// DUMMY
	M_DUMMY = 0xffffffff,
};

//-------------------------------------
// gfx register field shift
enum GFX_REG_FIELD_SHIFT
{
	// G_CONFIG
	// ???

	// G_STATUS
	// ???

	// DRAMQ_STAD
	// ???

	//DRAMQ_LEN
	// ???

	// G_MODE
	SH_CM = 0,
	SH_OP_MODE = 4,
	SH_FIRE = 11,

	// RECT_COLOR
	SH_RECT_COLOR = 0,

	// SRC_BSAD
	SH_SRC_BSAD = 0,
	SH_CHAR_CM = 30,

	// DST_BSAD
	SH_DST_BSAD = 0,

	// SRC_XY
	// ???

	// DST_XY
	// ???

	// SRC_SIZE
	SH_SRC_WIDTH = 0,
	SH_RL_DEC = 15,
	SH_SRC_HEIGHT = 16,
	SH_SRC_CM = 27,

	// S_OSD_WIDTH
	SH_OSD_WIDTH = 0,
	SH_SRC_PITCH = 16,

	// CLIP_BR
	// ???

	// CLIP_TL
	// ???

	// GRAD_X_DELTA
	SH_DELTA_X_C0 = 0,
	SH_DELTA_X_C1 = 8,
	SH_DELTA_X_C2 = 16,
	SH_DELTA_X_C3 = 24,

	// GRAD_Y_DELTA
	SH_DELTA_Y_C0 = 0,
	SH_DELTA_Y_C1 = 8,
	SH_DELTA_Y_C2 = 16,
	SH_DELTA_Y_C3 = 24,

	//GRAD_XY_INC
	SH_GRAD_X_PIX_INC = 0,
	SH_GRAD_Y_PIX_INC = 16,
	SH_GRAD_MODE = 30,

	// BITBLT_MODE
	SH_TRANS_ENA = 0,
	SH_KEYNOT_ENA = 1,
	SH_COLCHG_ENA = 2,
	SH_CLIP_ENA = 3,
	SH_CFMT_ENA = 4,
	SH_BLT_4BPP = 5,
	SH_ALPHA_VALUE = 8,
	SH_ALCOM_PASS = 16,

	// KEY_DATA0
	SH_COLOR_KEY_MIN = 0,

	// KEY_DATA1
	SH_COLOR_KEY_MAX = 0,

	// SRCCBCR_BSAD
	SH_SRCCBCR_BSAD = 0,
	SH_YC_FMT = 30,

	// SRCCBCR_PITCH
	SH_SRCCBCR_PITCH = 0,
	SH_VSTD = 16,
	SH_VSYS = 17,
	SH_VSCLIP = 18,
	SH_FLD_PIC = 19,
	SH_SWAP_MODE = 20,

	// DSTCBCR_BSAD
	SH_DSTCBCR_BSAD = 0,

	// F_COLOR
	SH_FORE_COLOR = 0,

	// B_COLOR
	SH_BACK_COLOR = 0,

	// COLOR_TRAN0
	SH_COLOR_TRAN0 = 0,

	// COLOR_TRAN1
	SH_COLOR_TRAN1 = 0,

	// COLOR_TRAN2
	SH_COLOR_TRAN2 = 0,

	// COLOR_TRAN3
	SH_COLOR_TRAN3 = 0,

	// COLOR_TRAN4
	SH_COLOR_TRAN4 = 0,

	// COLOR_TRAN5
	SH_COLOR_TRAN5 = 0,

	// COLOR_TRAN6
	SH_COLOR_TRAN6 = 0,

	// COLOR_TRAN7
	SH_COLOR_TRAN7 = 0,

	// STR_BLT_H
	SH_STR_BLT_H = 0,

	// STR_BLT_V
	SH_STR_BLT_V = 0,

	// STR_DST_SIZE
	SH_STR_DST_WIDTH = 0,
	SH_STR_DST_HEIGHT = 16,

	// DUMMY
	SH_DUMMY = 0,
};

//-------------------------------------
// gfx command
typedef struct GFX_CMD
{
	unsigned int u4Value;
	unsigned int u4Addr;
} GFX_CMD_T;

//-------------------------------------
// gfx register field
typedef struct GFX_REG_FIELD
{
	// G_CONFIG
	// ???

	// G_STATUS
	// ???

	// DRAMQ_STAD
	// ???

	//DRAMQ_LEN
	// ???

	// G_MODE
	unsigned int u4DstCm;
	unsigned int u4OpMode;
	unsigned int u4Fire;

	// RECT_COLOR
	unsigned int u4RectColor;

	// SRC_BSAD
	unsigned int u4SrcBsad;
	unsigned int u4CharCm;

	// DST_BSAD
	unsigned int u4DstBsad;

	// SRC_XY
	// ???

	// DST_XY
	// ???

	// SRC_SIZE
	unsigned int u4Width;
	unsigned int u4RlDec;
	unsigned int u4Height;
	unsigned int u4SrcCm;

	// S_OSD_WIDTH
	unsigned int u4DstPitch;
	unsigned int u4SrcPitch;

	// CLIP_BR
	// ???

	// CLIP_TL
	// ???

	// GRAD_X_DELTA
	unsigned int u4DeltaXC0;
	unsigned int u4DeltaXC1;
	unsigned int u4DeltaXC2;
	unsigned int u4DeltaXC3;

	// GRAD_Y_DELTA
	unsigned int u4DeltaYC0;
	unsigned int u4DeltaYC1;
	unsigned int u4DeltaYC2;
	unsigned int u4DeltaYC3;

	//GRAD_XY_INC
	unsigned int u4GradXPixInc;
	unsigned int u4GradYPixInc;
	unsigned int u4GradMode;

	// BITBLT_MODE
	unsigned int u4TransEna;
	unsigned int u4KeynotEna;
	unsigned int u4ColchgEna;
	unsigned int u4ClipEna;
	unsigned int u4FmtchgEna;
	unsigned int u4Blt4Bpp;
	unsigned int u4AlphaValue;
	unsigned int u4AlcomPass;

	// KEY_DATA0
	unsigned int u4ColorKeyMin;

	// KEY_DATA1
	unsigned int u4ColorKeyMax;

	// SRCCBCR_BSAD
	unsigned int u4SrccbcrBsad;
	unsigned int u4YcFmt;

	// SRCCBCR_PITCH
	unsigned int u4SrccbcrPitch;
	unsigned int u4Vstd;
	unsigned int u4Vsys;
	unsigned int u4VsClip;
	unsigned int u4FldPic;
	unsigned int u4SwapMode;

	// DSTCBCR_BSAD
	unsigned int u4DstcbcrBsad;

	// F_COLOR
	unsigned int u4ForeColor;

	// B_COLOR
	unsigned int u4BackColor;

	// COLOR_TRAN0
	unsigned int u4ColorTrans0;

	// COLOR_TRAN1
	unsigned int u4ColorTrans1;

	// COLOR_TRAN2
	unsigned int u4ColorTrans2;

	// COLOR_TRAN3
	unsigned int u4ColorTrans3;

	// COLOR_TRAN4
	unsigned int u4ColorTrans4;

	// COLOR_TRAN5
	unsigned int u4ColorTrans5;

	// COLOR_TRAN6
	unsigned int u4ColorTrans6;

	// COLOR_TRAN7
	unsigned int u4ColorTrans7;

	// STR_BLT_H
	unsigned int u4StrBltH;

	// STR_BLT_V
	unsigned int u4StrBltV;

	// STR_DST_SIZE
	unsigned int u4StrDstWidth;
	unsigned int u4StrDstHeight;

	// DUMMY
	unsigned int u4Dummy;

} GFX_REG_FIELD_T;

//-------------------------------------
// gfx color mode (string)
static char *_aszGfxCm[] =
{ "Not support", "Not support", "Not support", "Not support", "Not support",
		"Not support", "Not support", "Not support", "Not support",
		"Not support", "RGB-8", "RGB-565", "RGB-1555", "RGB-4444", "RGB-8888",
		"Not support", };

//-------------------------------------
// gfx operation mode (string)
static char *_aszGfxOpMode[] =
{ "Reserved", "Text-Bitmap Mapping", "Rectangle Fill", "Draw Horizontal Line",
		"Draw Vertical Line", "Gradient Fill", "Normal BitBlt", "DMA",
		"Alpha Blending", "Alpha Composition", "YCbCr to RGB", "Stretch",
		"Alpha Map", };

//-------------------------------------
// gradient mode (string)
static char *_aszGradMode[] =
{ "Reserved", "Horizontal Only", "Vertical Only", "Both Hor and Ver", };

//-------------------------------------
// composition pass mode (string)
static char *_aszComposePassMode[] =
{ "Pass 0", "Pass 1", "Pass 2", "Pass 3", "Pass 4", "Pass 5", };

//-------------------------------------
// porter-duff rule (string)
static char *_aszPorterDuffRule[] =
{ "Clear", "Dst In", "Dst Out", "Dst Over", "Src", "Src In", "Src Out",
		"Src Over", };

//-------------------------------------
// write each gfx operation to file
void report_action(GFX_REG_FIELD_T *psGrf, int i4FireCount);

#endif
