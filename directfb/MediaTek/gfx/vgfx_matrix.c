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

#include "gfx_if.h"
#include "gfx_drvif.h"
#include "gfx_dif.h"
#include "gfx_hw.h"
#include "gfx_cmdque.h"
#include "gfx_debug.h"

#ifdef CC_FLIP_MIRROR_SUPPORT
#include "eeprom_if.h"
#include "drv_display.h"
#endif

#define VG_INVERSE_8    8
/*-----------------------------------------------------------------------------
 data declarations
 ----------------------------------------------------------------------------*/

#define ORG_SIGN    0x80000000
#define ORG_EXP     0x7f800000
#define ORG_MAN     0x007fffff

#define REDUCE_SIGN 0x80000000
#define REDUCE_EXP  0x7e000000
#define REDUCE_MAN  0x01fffe00
#define MAN_MASK    0x007fff80

#define REDUCE_NAN  0x7f000000
#define REDUCE_INF  0x7e000000

typedef union
{
    unsigned int i;
    float f;
} fnum;

//solve AX = B
void gaussian_elimination(double A[][8], double X[], double B[])
{
    int i, j, k;
    double ratio;
    int first_nonzero_row;
    double temp;

    int column_all_zero[8];

    for (j = 0; j < VG_INVERSE_8; j++)
    {
        column_all_zero[j] = 1;

        for (i = j; i < VG_INVERSE_8; i++)
        {
            if (A[i][j] != 0)
            {
                column_all_zero[j] = 0;
                first_nonzero_row = i;

                if (first_nonzero_row != j)
                {
                    for (k = 0; k < VG_INVERSE_8; k++)
                    {
                        temp = A[j][k];
                        A[j][k] = A[first_nonzero_row][k];
                        A[first_nonzero_row][k] = temp;
                    }
                    temp = B[j];
                    B[j] = B[first_nonzero_row];
                    B[first_nonzero_row] = temp;
                }

                break;
            }
        }

        if (!column_all_zero[j])
        {
            for (i = j + 1; i < VG_INVERSE_8; i++)
            {
                ratio = A[i][j] / A[j][j];

                for (k = j; k < VG_INVERSE_8; k++)
                {
                    A[i][k] -= A[j][k] * ratio;
                }
                B[i] -= B[j] * ratio;
            }
        }
    }

    for (i = VG_INVERSE_8 - 1; i >= 0; i--)
    {
        if (column_all_zero[i] == 1)
        {
            X[i] = 0;
        }
        else
        {
            for (j = i + 1; j < VG_INVERSE_8; j++)
            {
                B[i] -= A[i][j] * X[j];
            }
            X[i] = B[i] / A[i][i];
        }
    }
}

void derive_inverse_transform(float src_x[], float src_y[], float dst_x[],
        float dst_y[], float inv_matrix[][3])
{
    int i;

    double A[8][8];
    double X[8];
    double B[8];

    for (i = 0; i < 4; i++)
    {
        A[i][0] = dst_x[i];
        A[i][1] = dst_y[i];
        A[i][2] = 1;
        A[i][3] = 0;
        A[i][4] = 0;
        A[i][5] = 0;
        A[i][6] = -(dst_x[i] * src_x[i]);
        A[i][7] = -(dst_y[i] * src_x[i]);

        B[i] = src_x[i];

        A[i + 4][0] = 0;
        A[i + 4][1] = 0;
        A[i + 4][2] = 0;
        A[i + 4][3] = dst_x[i];
        A[i + 4][4] = dst_y[i];
        A[i + 4][5] = 1;
        A[i + 4][6] = -(dst_x[i] * src_y[i]);
        A[i + 4][7] = -(dst_y[i] * src_y[i]);

        B[i + 4] = src_y[i];
    }

    gaussian_elimination(A, X, B);

    for (i = 0; i < 8; i++)
    {
        inv_matrix[i / 3][i % 3] = (float) X[i];
    }
    inv_matrix[2][2] = 1;
}

unsigned int fnum_converter(float in)
{
    fnum fnum_in;
    fnum_in.f = in;

    unsigned int out;
    unsigned int sign;
    int exp_tmp;
    unsigned int man;
    unsigned int exp;

    fnum_in.f = in;
    sign = fnum_in.i & ORG_SIGN;
    //NAN or INF
    if ((fnum_in.i & ORG_EXP) == ORG_EXP)
    {
        if (fnum_in.i & ORG_MAN) //NAN
            out = REDUCE_NAN;
        else
            //INF
            out = REDUCE_INF | sign;

        return out;
    }

    exp_tmp = (((int) (fnum_in.i) & ORG_EXP) >> 23) - 96; //old bias 127, new bias31

    if (exp_tmp >= 63) //6 bits exponential = 63
    {
        if (sign)
            out = REDUCE_SIGN | REDUCE_INF;

        else
            out = REDUCE_INF;

        return out;
    }
    else if (exp_tmp <= 0) //min tie to 0, no denormal mode
        return 0;

    man = fnum_in.i & ORG_MAN;

    //Rounding
    if (man & 0x40)
        man += 0x80;

    man = (man & MAN_MASK) << 2; //original mantissa masked by 16 bits then left shit 2 bits to match 6.16

    exp = exp_tmp << 25;

    out = sign + exp + man;

    return out;
}
