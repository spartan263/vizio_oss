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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "gfxcmdparser.h"

#define SELF_DEBUG  0

int main(int argc, char **argv)
{
	FILE *ifp;
	FILE *ofp;
	GFX_CMD_T *psGfxCmdBuf;
	GFX_REG_FIELD_T sGrf; // Gfx Register Field

	int i4GfxCmdCount, i4FileSize, i4FireCount;
	int i4ReadBytes, i;

#if !SELF_DEBUG
	if (argc < 2)
	{
		printf("argv: filename\n");
		exit(-1);
	}
#endif

	// get gfx command queue file size
#if SELF_DEBUG
	ifp = fopen(MEM_DUMP_FNAME, "rb");
#else
	ifp = fopen(argv[1], "rb");
#endif

	if (ifp == NULL)
	{
		printf("Error: can NOT find %s\n", argv[1]);
		exit(-1);
	}
	fseek(ifp, 0, SEEK_END);
	i4FileSize = ftell(ifp);
	fclose(ifp);

	// calculate # of gfx command
	// one command = 8 bytes
	if ((i4FileSize % GFX_CMD_SIZE) != 0)
	{
		printf("file size error !!\n");
		printf("(fsize %c %d) = %d\n", 37, GFX_CMD_SIZE,
				(i4FileSize % GFX_CMD_SIZE));
		exit(-1);
	}
	i4GfxCmdCount = (i4FileSize / GFX_CMD_SIZE);

	// allocate buffer
	psGfxCmdBuf = (GFX_CMD_T *) malloc(i4GfxCmdCount * sizeof(GFX_CMD_T));
	assert(psGfxCmdBuf != NULL);

	// read file into buffer
#if SELF_DEBUG
	ifp = fopen(MEM_DUMP_FNAME, "rb");
#else
	ifp = fopen(argv[1], "rb");
#endif
	assert(ifp != NULL);
	i4ReadBytes = fread(psGfxCmdBuf, 1, i4FileSize, ifp);
	assert(i4ReadBytes != 0);
	fclose(ifp);

	// write total gfx command count to file
	ofp = fopen(REPORT_FNAME, "w");
	assert(ofp != NULL);
	fprintf(ofp, "# of Total Gfx Command = %d\n", i4GfxCmdCount);
	fclose(ofp);

	// clear sGrf structure to avoid memory violation
	memset(&sGrf, 0, sizeof(GFX_REG_FIELD_T));

	// parse all gfx commands
	i4FireCount = 0;
	for (i = 0; i < i4GfxCmdCount; i++)
	{
		if ((psGfxCmdBuf[i].u4Addr & GFX_BSAD_MASK) != GFX_BASE_ADDR)
		{
			printf("Error: 0x%x NOT gfx register address\n",
					psGfxCmdBuf[i].u4Addr);
			exit(-1);
		}

		switch (psGfxCmdBuf[i].u4Addr & (~GFX_BSAD_MASK))
		{
		//case G_CONFIG: break;

		//case G_STATUS: break;

		//case DRAMQ_STAD: break;

		//case DRAMQ_LEN: break;

		case G_MODE:
			sGrf.u4DstCm = (psGfxCmdBuf[i].u4Value & M_CM) >> SH_CM;
			sGrf.u4OpMode = (psGfxCmdBuf[i].u4Value & M_OP_MODE) >> SH_OP_MODE;
			sGrf.u4Fire = (psGfxCmdBuf[i].u4Value & M_FIRE) >> SH_FIRE;

			report_action(&sGrf, ++i4FireCount);

			break;

		case RECT_COLOR:
			sGrf.u4RectColor = (psGfxCmdBuf[i].u4Value & M_RECT_COLOR)
					>> SH_RECT_COLOR;
			break;

		case SRC_BSAD:
			sGrf.u4SrcBsad = (psGfxCmdBuf[i].u4Value & M_SRC_BSAD)
					>> SH_SRC_BSAD;
			sGrf.u4CharCm = (psGfxCmdBuf[i].u4Value & M_CHAR_CM) >> SH_CHAR_CM;
			break;

		case DST_BSAD:
			sGrf.u4DstBsad = (psGfxCmdBuf[i].u4Value & M_DST_BSAD)
					>> SH_DST_BSAD;
			break;

			//case SRC_XY: break;

			//case DST_XY: break;

		case SRC_SIZE:
			sGrf.u4Width = (psGfxCmdBuf[i].u4Value & M_SRC_WIDTH)
					>> SH_SRC_WIDTH;
			sGrf.u4RlDec = (psGfxCmdBuf[i].u4Value & M_RL_DEC) >> SH_RL_DEC;
			sGrf.u4Height = (psGfxCmdBuf[i].u4Value & M_SRC_HEIGHT)
					>> SH_SRC_HEIGHT;
			sGrf.u4SrcCm = (psGfxCmdBuf[i].u4Value & M_SRC_CM) >> SH_SRC_CM;

			// real height = register height + 1
			sGrf.u4Height++;
			break;

		case S_OSD_WIDTH:
			sGrf.u4DstPitch = (psGfxCmdBuf[i].u4Value & M_OSD_WIDTH)
					>> SH_OSD_WIDTH;
			sGrf.u4SrcPitch = (psGfxCmdBuf[i].u4Value & M_SRC_PITCH)
					>> SH_SRC_PITCH;

			// unit is 16 bytes
			sGrf.u4DstPitch = sGrf.u4DstPitch * 16;
			sGrf.u4SrcPitch = sGrf.u4SrcPitch * 16;
			break;

			//case CLIP_BR: break;

			//case CLIP_TL: break;

		case GRAD_X_DELTA:
			sGrf.u4DeltaXC0 = (psGfxCmdBuf[i].u4Value & M_DELTA_X_C0)
					>> SH_DELTA_X_C0;
			sGrf.u4DeltaXC1 = (psGfxCmdBuf[i].u4Value & M_DELTA_X_C1)
					>> SH_DELTA_X_C1;
			sGrf.u4DeltaXC2 = (psGfxCmdBuf[i].u4Value & M_DELTA_X_C2)
					>> SH_DELTA_X_C2;
			sGrf.u4DeltaXC3 = (psGfxCmdBuf[i].u4Value & M_DELTA_X_C3)
					>> SH_DELTA_X_C3;
			break;

		case GRAD_Y_DELTA:
			sGrf.u4DeltaYC0 = (psGfxCmdBuf[i].u4Value & M_DELTA_Y_C0)
					>> SH_DELTA_Y_C0;
			sGrf.u4DeltaYC1 = (psGfxCmdBuf[i].u4Value & M_DELTA_Y_C1)
					>> SH_DELTA_Y_C1;
			sGrf.u4DeltaYC2 = (psGfxCmdBuf[i].u4Value & M_DELTA_Y_C2)
					>> SH_DELTA_Y_C2;
			sGrf.u4DeltaYC3 = (psGfxCmdBuf[i].u4Value & M_DELTA_Y_C3)
					>> SH_DELTA_Y_C3;
			break;

		case GRAD_XY_INC:
			sGrf.u4GradXPixInc = (psGfxCmdBuf[i].u4Value & M_GRAD_X_PIX_INC)
					>> SH_GRAD_X_PIX_INC;
			sGrf.u4GradYPixInc = (psGfxCmdBuf[i].u4Value & M_GRAD_Y_PIX_INC)
					>> SH_GRAD_Y_PIX_INC;
			sGrf.u4GradMode = (psGfxCmdBuf[i].u4Value & M_GRAD_MODE)
					>> SH_GRAD_MODE;
			break;

		case BITBLT_MODE:
			sGrf.u4TransEna = (psGfxCmdBuf[i].u4Value & M_TRANS_ENA)
					>> SH_TRANS_ENA;
			sGrf.u4KeynotEna = (psGfxCmdBuf[i].u4Value & M_KEYNOT_ENA)
					>> SH_KEYNOT_ENA;
			sGrf.u4ColchgEna = (psGfxCmdBuf[i].u4Value & M_COLCHG_ENA)
					>> SH_COLCHG_ENA;
			sGrf.u4ClipEna = (psGfxCmdBuf[i].u4Value & M_CLIP_ENA)
					>> SH_CLIP_ENA;
			sGrf.u4FmtchgEna = (psGfxCmdBuf[i].u4Value & M_CFMT_ENA)
					>> SH_CFMT_ENA;
			sGrf.u4Blt4Bpp = (psGfxCmdBuf[i].u4Value & M_BLT_4BPP)
					>> SH_BLT_4BPP;
			sGrf.u4AlphaValue = (psGfxCmdBuf[i].u4Value & M_ALPHA_VALUE)
					>> SH_ALPHA_VALUE;
			sGrf.u4AlcomPass = (psGfxCmdBuf[i].u4Value & M_ALCOM_PASS)
					>> SH_ALCOM_PASS;
			break;

		case KEY_DATA0:
			sGrf.u4ColorKeyMin = (psGfxCmdBuf[i].u4Value & M_COLOR_KEY_MIN)
					>> SH_COLOR_KEY_MIN;
			break;

		case KEY_DATA1:
			sGrf.u4ColorKeyMax = (psGfxCmdBuf[i].u4Value & M_COLOR_KEY_MAX)
					>> SH_COLOR_KEY_MAX;
			break;

		case SRCCBCR_BSAD:
			sGrf.u4SrccbcrBsad = (psGfxCmdBuf[i].u4Value & M_SRCCBCR_BSAD)
					>> SH_SRCCBCR_BSAD;
			sGrf.u4YcFmt = (psGfxCmdBuf[i].u4Value & M_YC_FMT) >> SH_YC_FMT;
			break;

		case SRCCBCR_PITCH:
			sGrf.u4SrccbcrPitch = (psGfxCmdBuf[i].u4Value & M_SRCCBCR_PITCH)
					>> SH_SRCCBCR_PITCH;
			sGrf.u4Vstd = (psGfxCmdBuf[i].u4Value & M_VSTD) >> SH_VSTD;
			sGrf.u4Vsys = (psGfxCmdBuf[i].u4Value & M_VSYS) >> SH_VSYS;
			sGrf.u4VsClip = (psGfxCmdBuf[i].u4Value & M_VSCLIP) >> SH_VSCLIP;
			sGrf.u4FldPic = (psGfxCmdBuf[i].u4Value & M_FLD_PIC) >> SH_FLD_PIC;
			sGrf.u4SwapMode = (psGfxCmdBuf[i].u4Value & M_SWAP_MODE)
					>> SH_SWAP_MODE;
			break;

		case DSTCBCR_BSAD:
			sGrf.u4DstcbcrBsad = (psGfxCmdBuf[i].u4Value & M_DSTCBCR_BSAD)
					>> SH_DSTCBCR_BSAD;
			break;

		case F_COLOR:
			sGrf.u4ForeColor = (psGfxCmdBuf[i].u4Value & M_FORE_COLOR)
					>> SH_FORE_COLOR;
			break;

		case B_COLOR:
			sGrf.u4BackColor = (psGfxCmdBuf[i].u4Value & M_BACK_COLOR)
					>> SH_BACK_COLOR;
			break;

		case COLOR_TRAN0:
			sGrf.u4ColorTrans0 = (psGfxCmdBuf[i].u4Value & M_COLOR_TRAN0)
					>> SH_COLOR_TRAN0;
			break;

		case COLOR_TRAN1:
			sGrf.u4ColorTrans1 = (psGfxCmdBuf[i].u4Value & M_COLOR_TRAN1)
					>> SH_COLOR_TRAN1;
			break;

		case COLOR_TRAN2:
			sGrf.u4ColorTrans2 = (psGfxCmdBuf[i].u4Value & M_COLOR_TRAN2)
					>> SH_COLOR_TRAN2;
			break;

		case COLOR_TRAN3:
			sGrf.u4ColorTrans3 = (psGfxCmdBuf[i].u4Value & M_COLOR_TRAN3)
					>> SH_COLOR_TRAN3;
			break;

		case COLOR_TRAN4:
			sGrf.u4ColorTrans4 = (psGfxCmdBuf[i].u4Value & M_COLOR_TRAN4)
					>> SH_COLOR_TRAN4;
			break;

		case COLOR_TRAN5:
			sGrf.u4ColorTrans5 = (psGfxCmdBuf[i].u4Value & M_COLOR_TRAN5)
					>> SH_COLOR_TRAN5;
			break;

		case COLOR_TRAN6:
			sGrf.u4ColorTrans6 = (psGfxCmdBuf[i].u4Value & M_COLOR_TRAN6)
					>> SH_COLOR_TRAN6;
			break;

		case COLOR_TRAN7:
			sGrf.u4ColorTrans7 = (psGfxCmdBuf[i].u4Value & M_COLOR_TRAN7)
					>> SH_COLOR_TRAN7;
			break;

		case STR_BLT_H:
			sGrf.u4StrBltH = (psGfxCmdBuf[i].u4Value & M_STR_BLT_H)
					>> SH_STR_BLT_H;
			break;

		case STR_BLT_V:
			sGrf.u4StrBltV = (psGfxCmdBuf[i].u4Value & M_STR_BLT_V)
					>> SH_STR_BLT_V;
			break;

		case STR_DST_SIZE:
			sGrf.u4StrDstWidth = (psGfxCmdBuf[i].u4Value & M_STR_DST_WIDTH)
					>> SH_STR_DST_WIDTH;
			sGrf.u4StrDstHeight = (psGfxCmdBuf[i].u4Value & M_STR_DST_HEIGHT)
					>> SH_STR_DST_HEIGHT;
			break;

		case DUMMY:
			sGrf.u4Dummy = (psGfxCmdBuf[i].u4Value & M_DUMMY) >> SH_DUMMY;
			break;

		default:
			ofp = fopen(UNKNOWN_FNAME, "a");
			assert(ofp != NULL);
			fprintf(ofp, "(%5d) Unknown Register Address : 0x%x\n", i,
					psGfxCmdBuf[i].u4Addr);
			fclose(ofp);
			break;
		} // ~switch
	} // ~for

	free(psGfxCmdBuf);

	// write total fire count
	ofp = fopen(REPORT_FNAME, "a");
	assert(ofp != NULL);
	fprintf(ofp, "\n# of Total Fire Count = %d\n", i4FireCount);
	fclose(ofp);

#if SELF_DEBUG
	printf("\nParsing [%s] to [%s] .... OK\n\n", MEM_DUMP_FNAME, REPORT_FNAME);
#else
	printf("\nParsing [%s] to [%s] .... OK\n\n", argv[1], REPORT_FNAME);
#endif

	return 0;
}

//-----------------------------------------------------------------------------
// write each gfx operation to file
void report_action(GFX_REG_FIELD_T *psGrf, int i4FireCount)
{
	FILE *ofp = fopen(REPORT_FNAME, "a");
	assert(ofp != NULL);

	fprintf(
			ofp,
			"\n----------------------------------------------------------------------------------\n");

	if ((psGrf->u4OpMode < OP_TEXT_BITMAP) || (psGrf->u4OpMode > OP_ALPHA_MAP))
	{
		fprintf(ofp, "Error: not support operation = %u\n", psGrf->u4OpMode);
		fclose(ofp);
		exit(-1);
	}

	fprintf(ofp, "( %5d )  [ %s ]\n", i4FireCount,
			_aszGfxOpMode[psGrf->u4OpMode]);
	switch (psGrf->u4OpMode)
	{
	case OP_TEXT_BITMAP:
		// ???
		break;

	case OP_RECT_FILL:
	case OP_HOR_LINE:
	case OP_VER_LINE:
		fprintf(
				ofp,
				"Dst Color Mode = %s, Dst Base Addr = 0x%08x, Dst Pitch = %u\n",
				_aszGfxCm[psGrf->u4DstCm], psGrf->u4DstBsad, psGrf->u4DstPitch);
		fprintf(ofp, "Width = %u, Height = %u\n", psGrf->u4Width,
				psGrf->u4Height);
		fprintf(ofp, "Rect Color = 0x%x\n", psGrf->u4RectColor);
		break;

	case OP_GRAD_FILL:
		fprintf(
				ofp,
				"Dst Color Mode = %s, Dst Base Addr = 0x%08x, Dst Pitch = %u\n",
				_aszGfxCm[psGrf->u4DstCm], psGrf->u4DstBsad, psGrf->u4DstPitch);
		fprintf(ofp, "Width = %u, Height = %u\n", psGrf->u4Width,
				psGrf->u4Height);
		fprintf(ofp, "Rect Color = 0x%08x\n", psGrf->u4RectColor);
		fprintf(
				ofp,
				"X Direction : Delta0 = %u, Delta1 = %u, Delta2 = %u, Delta3 = %u\n",
				psGrf->u4DeltaXC0, psGrf->u4DeltaXC1, psGrf->u4DeltaXC2,
				psGrf->u4DeltaXC3);
		fprintf(
				ofp,
				"Y Direction : Delta0 = %u, Delta1 = %u, Delta2 = %u, Delta3 = %u\n",
				psGrf->u4DeltaYC0, psGrf->u4DeltaYC1, psGrf->u4DeltaYC2,
				psGrf->u4DeltaYC3);
		fprintf(ofp, "X Direction : Pix Inc = %u\n", psGrf->u4GradXPixInc);
		fprintf(ofp, "Y Direction : Pix Inc = %u\n", psGrf->u4GradYPixInc);
		fprintf(ofp, "Gradient Mode = %s\n", _aszGradMode[psGrf->u4GradMode]);
		break;

	case OP_BITBLT:
		fprintf(
				ofp,
				"Src Color Mode = %s, Src Base Addr = 0x%08x, Src Pitch = %u\n",
				_aszGfxCm[psGrf->u4SrcCm], psGrf->u4SrcBsad, psGrf->u4SrcPitch);
		fprintf(
				ofp,
				"Dst Color Mode = %s, Dst Base Addr = 0x%08x, Dst Pitch = %u\n",
				_aszGfxCm[psGrf->u4DstCm], psGrf->u4DstBsad, psGrf->u4DstPitch);
		fprintf(ofp, "Width = %u, Height = %u\n", psGrf->u4Width,
				psGrf->u4Height);
		fprintf(ofp, "Rect Color = 0x%08x, BitBlt Mode :", psGrf->u4RectColor);
		if ((psGrf->u4TransEna) || (psGrf->u4ColchgEna) || (psGrf->u4KeynotEna)
				|| (psGrf->u4ClipEna))
		{
			if (psGrf->u4TransEna)
			{
				fprintf(ofp, " [TRANS_EN]");
			}

			if (psGrf->u4ColchgEna)
			{
				fprintf(ofp, " [COLOR_CHG_EN]");
			}

			if (psGrf->u4KeynotEna)
			{
				fprintf(ofp, " [KEY_NOT_EN]");
			}

			if (psGrf->u4ClipEna)
			{
				fprintf(ofp, " [CLIP_EN]");
			}

			fprintf(ofp, "\n");
			fprintf(ofp, "Color Key Min = 0x%08x, Color Key Max = 0x%08x\n",
					psGrf->u4ColorKeyMin, psGrf->u4ColorKeyMax);
		}
		else
		{
			fprintf(ofp, " [Normal]\n");
		}
		break;

	case OP_DMA:
		// ???
		break;

	case OP_ALPHA_BLENDING:
		fprintf(
				ofp,
				"Src Color Mode = %s, Src Base Addr = 0x%08x, Src Pitch = %u\n",
				_aszGfxCm[psGrf->u4SrcCm], psGrf->u4SrcBsad, psGrf->u4SrcPitch);
		fprintf(
				ofp,
				"Dst Color Mode = %s, Dst Base Addr = 0x%08x, Dst Pitch = %u\n",
				_aszGfxCm[psGrf->u4DstCm], psGrf->u4DstBsad, psGrf->u4DstPitch);
		fprintf(ofp, "Width = %u, Height = %u\n", psGrf->u4Width,
				psGrf->u4Height);
		fprintf(ofp, "Alpha Value = %u\n", psGrf->u4AlphaValue);
		break;

	case OP_ALPHA_COMPOSE:
		fprintf(
				ofp,
				"Src Color Mode = %s, Src Base Addr = 0x%08x, Src Pitch = %u\n",
				_aszGfxCm[psGrf->u4SrcCm], psGrf->u4SrcBsad, psGrf->u4SrcPitch);
		fprintf(
				ofp,
				"Dst Color Mode = %s, Dst Base Addr = 0x%08x, Dst Pitch = %u\n",
				_aszGfxCm[psGrf->u4DstCm], psGrf->u4DstBsad, psGrf->u4DstPitch);
		fprintf(ofp, "Width = %u, Height = %u\n", psGrf->u4Width,
				psGrf->u4Height);
		fprintf(ofp, "Pass Stage = %s\n",
				_aszComposePassMode[psGrf->u4AlcomPass]);
		if (psGrf->u4AlcomPass == 2) // pass 2
		{
			fprintf(ofp, "P-D rule : [ %s ]\n",
					_aszPorterDuffRule[psGrf->u4AlphaValue]);
		}
		else
		{
			fprintf(ofp, "Alpha Value = %u\n", psGrf->u4AlphaValue);
		}
		break;

	case OP_YCBCR_TO_RGB:
		// ???
		break;

	case OP_STRETCH:
		fprintf(
				ofp,
				"Src Color Mode = %s, Src Base Addr = 0x%08x, Src Pitch = %u\n",
				_aszGfxCm[psGrf->u4SrcCm], psGrf->u4SrcBsad, psGrf->u4SrcPitch);
		fprintf(
				ofp,
				"Dst Color Mode = %s, Dst Base Addr = 0x%08x, Dst Pitch = %u\n",
				_aszGfxCm[psGrf->u4DstCm], psGrf->u4DstBsad, psGrf->u4DstPitch);
		fprintf(ofp, "Src Width = %u, Src Height = %u\n", psGrf->u4Width,
				psGrf->u4Height);
		fprintf(ofp, "Dst Width = %u, Dst Height = %u\n", psGrf->u4StrDstWidth,
				psGrf->u4StrDstHeight);
		fprintf(ofp, "H-scale Ratio = 0x%08x, V-scale Ratio = 0x%08x\n",
				psGrf->u4StrBltH, psGrf->u4StrBltV);
		break;

	case OP_ALPHA_MAP:
		fprintf(
				ofp,
				"Src Color Mode = %s, Src Base Addr = 0x%08x, Src Pitch = %u\n",
				_aszGfxCm[psGrf->u4SrcCm], psGrf->u4SrcBsad, psGrf->u4SrcPitch);
		fprintf(
				ofp,
				"Dst Color Mode = %s, Dst Base Addr = 0x%08x, Dst Pitch = %u\n",
				_aszGfxCm[psGrf->u4DstCm], psGrf->u4DstBsad, psGrf->u4DstPitch);
		fprintf(ofp, "Width = %u, Height = %u\n", psGrf->u4Width,
				psGrf->u4Height);
		break;

	default:
		break;
	} // ~switch

	fclose(ofp);
}
