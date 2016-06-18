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
 * $RCSfile: gfx_sw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_sw.c
 *  gfx driver software module interfaces
 *  init, get register base, isr, callback, op count and action
 */

/*
 lint inhibition(s)
 971 - Use of 'char' without 'signed' or 'unsigned'
 961 - #undef (used in middleware u_common.h and gfx engine workaround
 740 - this software engine collected bytes to 32bits to accelerate
 826 - pointer to pointer conversion used in frame buffer fill
 701 - yes, here needs several signed shift
 703 - Shift left of signed quantity (long)
 704 - Shift right of signed quantity (long)
 644 - Variable 'ai4ColorCompMax' may not have been initialized
 613 - Possible use of null pointer 'pu1SrcBase' in left argument
 to operator 'ptr+int'
 522 - Expected void type, assignment, increment or decrement
 834 - Operator '-' followed by operator '-' is confusing
 613 - Possible use of null pointer
 794 - Conceivable use of null pointer
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

/*lint -save -e961 -e971 -e10 -e18 -e19 */
#include "x_debug.h"

#include "gfx_if.h"
#include "gfx_drvif.h"
#include "gfx_sw.h"
#include "gfx_dif.h"
/*lint -restore */

#if defined(GFX_ENABLE_SW_MODE)
//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

// for Alpha Composition Loop Mode use
#define COMP_NUM            4   // A,R,G,B
#define DIV_SHT_1           4
#define TABLE_SIZE          4096
#define PASS3_SHT_NUM_1     12
#define ALPHA_SHT           8
#define ALPHA_SHT_16BPP     (ALPHA_SHT+4)

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

static void *_pvGfxSwCallBackTag;

static UINT32 _au4GfxSwReg[GREG_FILE_SIZE];

static UINT32 _u4GfxSwActionCount;

static const UINT8 _au1sPixelLgDwn[] =
{
    // lg(pixel_perl_dw)
    4, //CM_YCbCr_CLUT2
    3, //CM_YCbCr_CLUT4
    2, //CM_YCbCr_CLUT8
    0, //CM_Reserved0
    1, //CM_CbYCrY422_DIRECT16
    1, //CM_YCbYCr422_DIRECT16
    0, //CM_AYCbCr8888_DIRECT32
    0, //CM_Reserved1
    4, //CM_RGB_CLUT2
    3, //CM_RGB_CLUT4
    2, //CM_RGB_CLUT8
    1, //CM_RGB565_DIRECT16
    1, //CM_ARGB1555_DIRECT16
    1, //CM_ARGB4444_DIRECT16
    0, //CM_ARGB8888_DIRECT32
    0 //CM_Reserved2
};

/** division reference table (value)
 *  for alpha composition use
 *  
 */
static const INT32 _i4sAlphaInvTbl[] =
{
    0, 2048, 2048, 1365, 2048, 1638, 1365, 1170,
    2048, 1820, 1638, 1489, 1365, 1260, 1170, 1092,
    2048, 1928, 1820, 1725, 1638, 1560, 1489, 1425,
    1365, 1311, 1260, 1214, 1170, 1130, 1092, 1057,
    2048, 1986, 1928, 1872, 1820, 1771, 1725, 1680,
    1638, 1598, 1560, 1524, 1489, 1456, 1425, 1394,
    1365, 1337, 1311, 1285, 1260, 1237, 1214, 1192,
    1170, 1150, 1130, 1111, 1092, 1074, 1057, 1040,
    2048, 2016, 1986, 1956, 1928, 1900, 1872, 1846,
    1820, 1796, 1771, 1748, 1725, 1702, 1680, 1659,
    1638, 1618, 1598, 1579, 1560, 1542, 1524, 1507,
    1489, 1473, 1456, 1440, 1425, 1409, 1394, 1380,
    1365, 1351, 1337, 1324, 1311, 1298, 1285, 1273,
    1260, 1248, 1237, 1225, 1214, 1202, 1192, 1181,
    1170, 1160, 1150, 1140, 1130, 1120, 1111, 1101,
    1092, 1083, 1074, 1066, 1057, 1049, 1040, 1032,
    2048, 2032, 2016, 2001, 1986, 1971, 1956, 1942,
    1928, 1913, 1900, 1886, 1872, 1859, 1846, 1833,
    1820, 1808, 1796, 1783, 1771, 1759, 1748, 1736,
    1725, 1713, 1702, 1691, 1680, 1670, 1659, 1649,
    1638, 1628, 1618, 1608, 1598, 1589, 1579, 1570,
    1560, 1551, 1542, 1533, 1524, 1515, 1507, 1498,
    1489, 1481, 1473, 1464, 1456, 1448, 1440, 1432,
    1425, 1417, 1409, 1402, 1394, 1387, 1380, 1372,
    1365, 1358, 1351, 1344, 1337, 1331, 1324, 1317,
    1311, 1304, 1298, 1291, 1285, 1279, 1273, 1266,
    1260, 1254, 1248, 1242, 1237, 1231, 1225, 1219,
    1214, 1208, 1202, 1197, 1192, 1186, 1181, 1176,
    1170, 1165, 1160, 1155, 1150, 1145, 1140, 1135,
    1130, 1125, 1120, 1116, 1111, 1106, 1101, 1097,
    1092, 1088, 1083, 1079, 1074, 1070, 1066, 1061,
    1057, 1053, 1049, 1044, 1040, 1036, 1032, 1028
};

/** division reference table (shift)
 *  for alpha composition use
 *  
 */
static const INT32 _i4sAlphaInvShtTbl[] =
{
    0, 0, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7
};

static UINT32 _u4IsTableExist = (UINT32)FALSE;

static unsigned long _inverse_table_value[TABLE_SIZE+1];
static unsigned long _inverse_table_slope[TABLE_SIZE+1];
static unsigned long _inverse_table_shift[TABLE_SIZE+1];
static double _alpha_error_distr[TABLE_SIZE+1];

static INT32 _i4MemCompFlag = 0;

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** _pfnGfxSwCallBack
 *
 */
//-------------------------------------------------------------------------
static void (*_pfnGfxSwCallBack)(void *pvTag);

//-------------------------------------------------------------------------
/** _GfxSwColorExpansion
 *
 */
//-------------------------------------------------------------------------
static UINT32 _GfxSwColorExpansion(UINT32 u4Color, UINT32 u4ColorMode)
{
    switch (_au1sPixelLgDwn[u4ColorMode])
    {
        case 1:
        u4Color = (u4Color << 16) | (u4Color & 0xffff);
        break;

        case 2:
        u4Color = (u4Color << 8) | (u4Color & 0xff);
        u4Color = (u4Color << 16) | (u4Color & 0xffff);
        break;

        case 3:
        u4Color = (u4Color << 4) | (u4Color & 0xf);
        u4Color = (u4Color << 8) | (u4Color & 0xff);
        u4Color = (u4Color << 16) | (u4Color & 0xffff);
        break;

        case 4:
        u4Color = (u4Color << 2) | (u4Color & 0x3);
        u4Color = (u4Color << 4) | (u4Color & 0xf);
        u4Color = (u4Color << 8) | (u4Color & 0xff);
        u4Color = (u4Color << 16) | (u4Color & 0xffff);
        break;

        default:
        break;
    }

    return u4Color;
}

//-------------------------------------------------------------------------
/** _GfxSwTextBlt
 *
 */
//-------------------------------------------------------------------------
static INT32 _GfxSwTextBlt(const MI_DIF_FIELD_T *prsGfxReg)
{
    UNUSED(prsGfxReg);

    return -(INT32)E_GFX_INV_ARG;

}

//-------------------------------------------------------------------------
/** _GfxSwRectFill
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
static INT32 _GfxSwRectFill(const MI_DIF_FIELD_T *prsGfxReg)
{
    UINT8 *pu1DstBase, *pu1Write, *pu1DstLine;
    UINT32 *pu4Color;
    UINT32 u4Width, u4Height, u4DstPitch, u4Color, u4DstCM;
    UINT32 x, y;
    INT32 ai4RectColor[4] =
    {   0};

    switch (prsGfxReg->fg_OP_MODE)
    {
        case OP_DRAW_HLINE:
        u4Width = prsGfxReg->fg_SRC_WIDTH;
        u4Height = 1;
        break;

        case OP_DRAW_VLINE:
        u4Width = 1;
        u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;
        break;

        default:
        u4Width = prsGfxReg->fg_SRC_WIDTH;
        u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;
        break;
    }

    u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4; // 128bit aligned
    u4DstCM = prsGfxReg->fg_CM;
    u4Color = prsGfxReg->fg_RECT_COLOR;
    pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;

    // virtual address
    pu1DstBase = VIRTUAL(pu1DstBase);

    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    pu4Color = &u4Color;
    u4Color = _GfxSwColorExpansion(u4Color, u4DstCM);
    GFX_SwGetColorComponent((UINT8 **)&pu4Color, u4DstCM, ai4RectColor);

    for (y = 0; y < u4Height; y++)
    {
        pu1Write = pu1DstLine;

        for (x = 0; x < u4Width; x++)
        {
            // write rect color to dst
            GFX_SwSetColorComponent(&pu1Write, u4DstCM, ai4RectColor);
        }

        pu1DstLine += u4DstPitch;
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwDrawHline
 *
 */
//-------------------------------------------------------------------------
static INT32 _GfxSwDrawHline(const MI_DIF_FIELD_T *prsGfxReg)
{
    return _GfxSwRectFill(prsGfxReg);
}

//-------------------------------------------------------------------------
/** _GfxSwDrawVline
 *
 */
//-------------------------------------------------------------------------
static INT32 _GfxSwDrawVline(const MI_DIF_FIELD_T *prsGfxReg)
{
    return _GfxSwRectFill(prsGfxReg);
}

//-------------------------------------------------------------------------
/** _GfxSwGradFill
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwGradFill(const MI_DIF_FIELD_T *prsGfxReg)
{
    GFX_GRADIENT_DATA_T rData;

    rData.u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4; // 128bit aligned
    rData.u4DstCM = prsGfxReg->fg_CM;
    rData.u4RectColor = prsGfxReg->fg_RECT_COLOR;
    rData.pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    rData.u4Width = prsGfxReg->fg_SRC_WIDTH;
    rData.u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;
    rData.u4X_Pix_Inc = prsGfxReg->fg_GRAD_X_PIX_INC;
    rData.u4Y_Pix_Inc = prsGfxReg->fg_GRAD_Y_PIX_INC;
    rData.u4GradMode = prsGfxReg->fg_GRAD_MODE;

    rData.u1Delta_X_C0 = prsGfxReg->fg_DELTA_X_C0;
    rData.u1Delta_X_C1 = prsGfxReg->fg_DELTA_X_C1;
    rData.u1Delta_X_C2 = prsGfxReg->fg_DELTA_X_C2;
    rData.u1Delta_X_C3 = prsGfxReg->fg_DELTA_X_C3;

    rData.u1Delta_Y_C0 = prsGfxReg->fg_DELTA_Y_C0;
    rData.u1Delta_Y_C1 = prsGfxReg->fg_DELTA_Y_C1;
    rData.u1Delta_Y_C2 = prsGfxReg->fg_DELTA_Y_C2;
    rData.u1Delta_Y_C3 = prsGfxReg->fg_DELTA_Y_C3;

    // virtual address
    rData.pu1DstBase = VIRTUAL(rData.pu1DstBase);

    return GFX_SwGradientFill(&rData);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwBitBlt
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwBitBlt(const MI_DIF_FIELD_T *prsGfxReg)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Color, u4ColorMin, u4ColorMax;
    UINT32 u4Width, u4Height;
    UINT32 u4TransEn, u4KeynotEn, u4ColchgEn, u4KeySdSel;

    // get source & destination info
    pu1SrcBase = (UINT8 *)prsGfxReg->fg_SRC_BSAD;
    u4SrcPitch = prsGfxReg->fg_SRC_PITCH << 4;
    u4SrcCM = prsGfxReg->fg_SRC_CM;
    pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4;
    u4DstCM = prsGfxReg->fg_CM;
    u4Width = prsGfxReg->fg_SRC_WIDTH;
    u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;
    u4TransEn = prsGfxReg->fg_TRANS_ENA;
    u4KeynotEn = prsGfxReg->fg_KEYNOT_ENA;
    u4ColchgEn = prsGfxReg->fg_COLCHG_ENA;
    u4KeySdSel = prsGfxReg->fg_KEYSDSEL;
    u4Color = prsGfxReg->fg_RECT_COLOR;
    u4ColorMin = prsGfxReg->fg_COLOR_KEY_MIN;
    u4ColorMax = prsGfxReg->fg_COLOR_KEY_MAX;

    // virtual address
    pu1SrcBase = VIRTUAL(pu1SrcBase);
    // virtual address
    pu1DstBase = VIRTUAL(pu1DstBase);

    return GFX_SwBitBlt(pu1SrcBase, pu1DstBase, u4SrcPitch, u4DstPitch,
            u4SrcCM, u4DstCM, u4Width, u4Height, u4ColorMin, u4ColorMax,
            u4TransEn, u4KeynotEn, u4ColchgEn, u4KeySdSel, u4Color);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwDma
 *  1-D bitblt
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwDma(const MI_DIF_FIELD_T *prsGfxReg)
{
    UINT32 *pu4Src, *pu4Dst;
    UINT32 u4NDW;

    pu4Src = (UINT32 *)prsGfxReg->fg_SRC_BSAD;
    pu4Dst = (UINT32 *)prsGfxReg->fg_DST_BSAD;

    u4NDW = (prsGfxReg->fg_SRC_HEIGHT << 11) | prsGfxReg->fg_SRC_WIDTH;

    while (u4NDW-- > 0)
    {
        *pu4Dst++ = *pu4Src++;
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwAlphaBitBlt
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwAlphaBitBlt(const MI_DIF_FIELD_T *prsGfxReg)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4DstCM, u4Alpha;
    UINT32 u4Width, u4Height;

    // get source & destination info
    //
    pu1SrcBase = (UINT8 *)prsGfxReg->fg_SRC_BSAD;
    u4SrcPitch = prsGfxReg->fg_SRC_PITCH << 4;
    pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4;
    u4DstCM = prsGfxReg->fg_CM;
    u4Alpha = prsGfxReg->fg_ALPHA_VALUE;
    u4Width = prsGfxReg->fg_SRC_WIDTH;
    u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;

    // virtual address
    pu1SrcBase = VIRTUAL(pu1SrcBase);
    // virtual address
    pu1DstBase = VIRTUAL(pu1DstBase);

    return GFX_SwAlphaBlending_5381(pu1SrcBase, pu1DstBase, u4SrcPitch, u4DstPitch,
            u4DstCM, u4Alpha, u4Width, u4Height);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwAlphaComposition
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwAlphaComposition(const MI_DIF_FIELD_T *prsGfxReg)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4DstCM, u4Alpha;
    UINT32 u4Width, u4Height, u4Alcom;

    // get source & destination info
    pu1SrcBase = (UINT8 *)prsGfxReg->fg_SRC_BSAD;
    u4SrcPitch = prsGfxReg->fg_SRC_PITCH << 4;
    pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4;
    u4DstCM = prsGfxReg->fg_CM;
    u4Alpha = prsGfxReg->fg_ALPHA_VALUE;
    u4Width = prsGfxReg->fg_SRC_WIDTH;
    u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;
    u4Alcom = prsGfxReg->fg_ALCOM_PASS;

    // virtual address
    pu1SrcBase = VIRTUAL(pu1SrcBase);
    // virtual address
    pu1DstBase = VIRTUAL(pu1DstBase);

    return GFX_SwAlphaComposePass_5381(pu1SrcBase, pu1DstBase, u4SrcPitch,
            u4DstPitch, u4DstCM, u4DstCM, u4Width, u4Height, u4Alpha, u4Alcom);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwYCrCb2Rgb
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwYCrCb2Rgb(const MI_DIF_FIELD_T *prsGfxReg)
{
    GFX_YCBCR2RGB_DATA_T rData;

    // get source & destination info
    //
    rData.pu1LumaBase = (UINT8 *)prsGfxReg->fg_SRC_BSAD;
    rData.u4LumaPitch = prsGfxReg->fg_SRC_PITCH << 4;
    rData.pu1ChromaBase = (UINT8 *)prsGfxReg->fg_SRCCBCR_BSAD;
    rData.u4ChromaPitch = prsGfxReg->fg_SRCCBCR_PITCH << 4;
    rData.pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    rData.u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4;
    rData.u4DstCM = prsGfxReg->fg_CM;
    rData.u4Width = prsGfxReg->fg_SRC_WIDTH;
    rData.u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;
    rData.u4YCFormat = prsGfxReg->fg_YC_FMT;
    rData.u4VideoStd = prsGfxReg->fg_VSTD;
    rData.u4VideoSys = prsGfxReg->fg_VSYS;
    rData.u4VideoClip = prsGfxReg->fg_VSCLIP;
    rData.u4PicSelect = prsGfxReg->fg_FLD_PIC;
    rData.u4SwapMode = prsGfxReg->fg_SWAP_MODE;

    // virtual address
    rData.pu1LumaBase = VIRTUAL(rData.pu1LumaBase);
    // virtual address
    rData.pu1ChromaBase = VIRTUAL(rData.pu1ChromaBase);
    // virtual address
    rData.pu1DstBase = VIRTUAL(rData.pu1DstBase);

    return GFX_SwYCbCr2RGB(&rData);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwAlphaMapBitBlt
 *  Alpha map bitblt
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwAlphaMapBitBlt(const MI_DIF_FIELD_T *prsGfxReg)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;

    // get source & destination info
    pu1SrcBase = (UINT8 *)prsGfxReg->fg_SRC_BSAD;
    u4SrcPitch = prsGfxReg->fg_SRC_PITCH << 4;
    u4SrcCM = prsGfxReg->fg_SRC_CM;
    pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4;
    u4DstCM = prsGfxReg->fg_CM;
    u4Width = prsGfxReg->fg_SRC_WIDTH;
    u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;

    // virtual address
    pu1SrcBase = VIRTUAL(pu1SrcBase);
    // virtual address
    pu1DstBase = VIRTUAL(pu1DstBase);

    return GFX_SwAlphaMapBitBlt(pu1SrcBase, pu1DstBase, u4SrcPitch, u4DstPitch,
            u4SrcCM, u4DstCM, u4Width, u4Height);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwAlphaCompositionLoop
 *  Alpha composition loop mode
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwAlphaCompositionLoop(const MI_DIF_FIELD_T *prsGfxReg)
{
    GFX_ACLM_DATA_T rData;

    // get source & destination info
    rData.pu1SrcBase = (UINT8 *)prsGfxReg->fg_SRC_BSAD;
    rData.u4SrcPitch = prsGfxReg->fg_SRC_PITCH << 4; // 128bit aligned
    rData.pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    rData.u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4; // 128bit aligned
    rData.u4SrcCM = prsGfxReg->fg_SRC_CM;
    rData.u4DstCM = prsGfxReg->fg_CM;
    rData.u4Width = prsGfxReg->fg_SRC_WIDTH;
    rData.u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;

    rData.u4AlComAr = prsGfxReg->fg_ALCOM_AR;
    rData.u4AlComOpCode = prsGfxReg->fg_ALCOM_OPCODE;
    rData.u4AlComRectSrc = prsGfxReg->fg_ALCOM_RECT_SRC;
    rData.u4RectColor = prsGfxReg->fg_RECT_COLOR;

    // virtual address
    rData.pu1SrcBase = VIRTUAL(rData.pu1SrcBase);
    // virtual address
    rData.pu1DstBase = VIRTUAL(rData.pu1DstBase);

    return GFX_SwAlphaCompositionLoop(&rData);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwRopBitBlt
 *  Raster OPeration bitblt
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwRopBitBlt(const MI_DIF_FIELD_T *prsGfxReg)
{
    GFX_ROP_DATA_T rData;

    // get source & destination info
    rData.pu1SrcBase = (UINT8 *)prsGfxReg->fg_SRC_BSAD;
    rData.u4SrcPitch = prsGfxReg->fg_SRC_PITCH << 4; // 128bit aligned
    rData.pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    rData.u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4; // 128bit aligned
    rData.u4SrcCM = prsGfxReg->fg_SRC_CM;
    rData.u4DstCM = prsGfxReg->fg_CM;
    rData.u4Width = prsGfxReg->fg_SRC_WIDTH;
    rData.u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;

    rData.u4RopOpCode = prsGfxReg->fg_ROP_OPCODE;
    rData.u4RopNoWr = prsGfxReg->fg_ROP_NO_WR;

    // virtual address
    rData.pu1SrcBase = VIRTUAL(rData.pu1SrcBase);
    // virtual address
    rData.pu1DstBase = VIRTUAL(rData.pu1DstBase);

    if ((rData.u4RopNoWr == 1) &&
            (rData.u4RopOpCode == (UINT32)E_ROP_SRC_XOR_DST))
    {
        return GFX_SwMemCompare(&rData);
    }
    else
    {
        return GFX_SwRopBitBlt(&rData);
    }
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwIndexToDirectBitBlt
 *  Raster OPeration bitblt
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwIndexToDirectBitBlt(const MI_DIF_FIELD_T *prsGfxReg)
{
    GFX_IDX2DIR_DATA_T rData;

    // get source & destination info
    rData.pu1SrcBase = (UINT8 *)prsGfxReg->fg_SRC_BSAD;
    rData.u4SrcPitch = prsGfxReg->fg_SRC_PITCH << 4; // 128bit aligned
    rData.pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    rData.u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4; // 128bit aligned
    rData.u4CharCM = prsGfxReg->fg_CHAR_CM;
    rData.u4DstCM = prsGfxReg->fg_CM;
    rData.u4Width = prsGfxReg->fg_SRC_WIDTH;
    rData.u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;

    rData.pu1PaleBase = (UINT8 *)prsGfxReg->fg_PAL_BSAD;
    rData.u4MsbLeft = prsGfxReg->fg_MSB_LEFT;
    rData.u4ByteAlign = prsGfxReg->fg_LN_ST_BYTE_AL;

    // virtual address
    rData.pu1SrcBase = VIRTUAL(rData.pu1SrcBase);
    // virtual address
    rData.pu1DstBase = VIRTUAL(rData.pu1DstBase);
    // virtual address
    rData.pu1PaleBase = VIRTUAL(rData.pu1PaleBase);

    return GFX_SwIndexToDirectBitBlt(&rData);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** _GfxSwHoriLineToVertLine
 *  Horizontal line to vertical line
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
static INT32 _GfxSwHoriLineToVertLine(const MI_DIF_FIELD_T *prsGfxReg)
{
    GFX_HORI2VERT_DATA_T rData;

    // get source & destination info
    rData.pu1SrcBase = (UINT8 *)prsGfxReg->fg_SRC_BSAD;
    rData.u4SrcPitch = prsGfxReg->fg_SRC_PITCH << 4; // 128bit aligned
    rData.pu1DstBase = (UINT8 *)prsGfxReg->fg_DST_BSAD;
    rData.u4DstPitch = prsGfxReg->fg_OSD_WIDTH << 4; // 128bit aligned
    rData.u4SrcCM = prsGfxReg->fg_SRC_CM;
    rData.u4DstCM = prsGfxReg->fg_CM;
    rData.u4Width = prsGfxReg->fg_SRC_WIDTH;
    rData.u4Height = prsGfxReg->fg_SRC_HEIGHT + 1;

    rData.u4DstPitchDec = prsGfxReg->fg_DSTPITCH_DEC;
    rData.u4StrDstWidth = prsGfxReg->fg_STR_DST_WIDTH;
    rData.u4StrDstHeight = prsGfxReg->fg_STR_DST_HEIGHT + 1;

    // virtual address
    rData.pu1SrcBase = VIRTUAL(rData.pu1SrcBase);
    // virtual address
    rData.pu1DstBase = VIRTUAL(rData.pu1DstBase);

    return GFX_SwHoriLineToVertLine(&rData);
}
/*lint -restore */

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** GFX_SwInit
 *
 */
//-------------------------------------------------------------------------
void GFX_SwInit(void)
{
    _pfnGfxSwCallBack = NULL;
    _pvGfxSwCallBackTag = NULL;
    _u4GfxSwActionCount = 0;

    GFX_UNUSED_RET(x_memset(_au4GfxSwReg, 0, sizeof(_au4GfxSwReg)))
}

//-------------------------------------------------------------------------
/** GFX_SwGetRegBase
 *
 */
//-------------------------------------------------------------------------
void GFX_SwGetRegBase(UINT32 **ppu4RegBase)
{
    GFX_ASSERT(ppu4RegBase != NULL);
    GFX_ASSERT(*ppu4RegBase != NULL);

    *ppu4RegBase = (UINT32 *)_au4GfxSwReg;
}

//-------------------------------------------------------------------------
/** GFX_SwISR
 *
 */
//-------------------------------------------------------------------------
void GFX_SwISR(void)
{
    if (_pfnGfxSwCallBack != NULL)
    {
        _pfnGfxSwCallBack(_pvGfxSwCallBackTag);
    }
}

//-------------------------------------------------------------------------
/** GFX_SwSetCallBack
 *
 */
//-------------------------------------------------------------------------
INT32 GFX_SwSetCallBack(void (*pfnCallBack)(void *pvTag), void *pvTag)
{
    if (pfnCallBack != NULL)
    {
        _pfnGfxSwCallBack = pfnCallBack;
        _pvGfxSwCallBackTag = pvTag;
        return (INT32)E_GFX_OK;
    }

    return -(INT32)E_GFX_INV_ARG;
}

//-------------------------------------------------------------------------
/** GFX_SwGetOpCount
 *
 */
//-------------------------------------------------------------------------
UINT32 GFX_SwGetOpCount(void)
{
    return _u4GfxSwActionCount;
}

//-------------------------------------------------------------------------
/** GFX_SwAction
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e740 -e826 */
void GFX_SwAction(void)
{
    INT32 i4Ret;
    MI_DIF_FIELD_T *prGfxReg;

    prGfxReg = (MI_DIF_FIELD_T *)_au4GfxSwReg;

    switch (prGfxReg->fg_OP_MODE)
    {
        case OP_TEXT_BLT:
        i4Ret = _GfxSwTextBlt(prGfxReg);
        break;

        case OP_RECT_FILL:
        i4Ret = _GfxSwRectFill(prGfxReg);
        break;

        case OP_DRAW_HLINE:
        i4Ret = _GfxSwDrawHline(prGfxReg);
        break;

        case OP_DRAW_VLINE:
        i4Ret = _GfxSwDrawVline(prGfxReg);
        break;

        case OP_GRAD_FILL:
        i4Ret = _GfxSwGradFill(prGfxReg);
        break;

        case OP_BITBLT:
        i4Ret = _GfxSwBitBlt(prGfxReg);
        break;

        case OP_DMA:
        i4Ret = _GfxSwDma(prGfxReg);
        break;

        case OP_ALPHA_BLEND:
        i4Ret = _GfxSwAlphaBitBlt(prGfxReg);
        break;

        case OP_ALPHA_COMPOSE:
        i4Ret = _GfxSwAlphaComposition(prGfxReg);
        break;

        case OP_YCBCR_TO_RGB:
        i4Ret = _GfxSwYCrCb2Rgb(prGfxReg);
        break;

        case OP_ALPHA_MAP_BITBLT:
        i4Ret = _GfxSwAlphaMapBitBlt(prGfxReg);
        break;

        case OP_ALPHA_COMPOSE_LOOP:
        i4Ret = _GfxSwAlphaCompositionLoop(prGfxReg);
        break;

        case OP_ROP_BITBLT:
        i4Ret = _GfxSwRopBitBlt(prGfxReg);
        break;

        case OP_IDX_TO_DIR_BITBLT:
        i4Ret = _GfxSwIndexToDirectBitBlt(prGfxReg);
        break;

        case OP_LINE_HORI_TO_VERT:
        i4Ret = _GfxSwHoriLineToVertLine(prGfxReg);
        break;

        default:
        i4Ret = -(INT32)E_GFX_INV_ARG;
        break;
    }

    VERIFY(i4Ret == (INT32)E_GFX_OK);
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwSetColorComponent
 *  gfx alpha-map bitblt
 *  set color components to frame buffer, 
 *  advance frame buffer point for 1 pixel
 */
//-------------------------------------------------------------------------
/*lint -save -e703 -e704 -e826 -e961 */
void GFX_SwSetColorComponent(UINT8 **ppu1DestFb, UINT32 u4CM,
        const INT32 *pi4sColorComponent)
{
    INT32 i4Q, i4W, i4E, i4R;
    UINT32 u4Color;
    UINT16 u2Color;
    UINT8 u1Color;

    if ((ppu1DestFb == NULL) || (pi4sColorComponent == NULL))
    {
        return;
    }

    i4Q = pi4sColorComponent[0];
    i4W = pi4sColorComponent[1];
    i4E = pi4sColorComponent[2];
    i4R = pi4sColorComponent[3];

    i4Q = (i4Q > 255) ? 255 :
    (i4Q < 0) ? 0 : i4Q;

    i4W = (i4W > 255) ? 255 :
    (i4W < 0) ? 0 : i4W;

    i4E = (i4E > 255) ? 255 :
    (i4E < 0) ? 0 : i4E;

    i4R = (i4R > 255) ? 255 :
    (i4R < 0) ? 0 : i4R;

    switch (u4CM)
    {
        case CM_RGB_CLUT8:
        u1Color = i4R & 0xff;
        *((UINT8 *)*ppu1DestFb) = u1Color;
        *ppu1DestFb += 1;
        break;

        case CM_RGB565_DIRECT16:
        u2Color = ((i4W & 0xf8) << 8) |
        ((i4E & 0xfc) << 3) |
        (i4R >> 3);
        *((UINT16 *)*ppu1DestFb) = u2Color;
        *ppu1DestFb += 2;
        break;

        case CM_ARGB1555_DIRECT16:
        u2Color = (i4Q ? 0x8000 : 0) |
        ((i4W & 0xf8) << 7) |
        ((i4E & 0xf8) << 2) |
        (i4R >> 3);
        *((UINT16 *)*ppu1DestFb) = u2Color;
        *ppu1DestFb += 2;
        break;

        case CM_ARGB4444_DIRECT16:
        u2Color = ((i4Q & 0xf0) << 8) |
        ((i4W & 0xf0) << 4) |
        (i4E & 0xf0) |
        (i4R >> 4);
        *((UINT16 *)*ppu1DestFb) = u2Color;
        *ppu1DestFb += 2;
        break;

        case CM_ARGB8888_DIRECT32:
        u4Color = (i4Q << 24) |
        (i4W << 16) |
        (i4E << 8) |
        i4R;
        *((UINT32 *)*ppu1DestFb) = u4Color;
        *ppu1DestFb += 4;
        break;

        default:
        return;
    }

    return;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwGetColorComponent
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e826 -e961 */
void GFX_SwGetColorComponent(UINT8 **ppu1DestFb, UINT32 u4CM,
        INT32 *pi4ColorComponent)
{
    // get color components from frame buffer, advance frame buffer point for 1 pixel
    INT32 i4Q, i4W, i4E, i4R;
    UINT32 u4Color;
    UINT16 u2Color;
    UINT8 u1Color;

    if ((ppu1DestFb == NULL) || (pi4ColorComponent == NULL))
    {
        return;
    }

    switch (u4CM)
    {
        case CM_RGB_CLUT8:
        u1Color = *((UINT8 *)*ppu1DestFb);
        i4Q = u1Color & 0xff;
        i4W = u1Color & 0xff;
        i4E = u1Color & 0xff;
        i4R = u1Color & 0xff;
        *ppu1DestFb += 1;
        break;

        case CM_RGB565_DIRECT16:
        u2Color = *((UINT16 *)*ppu1DestFb);
        i4Q = 0xff;
        i4W = (u2Color >> 8) & 0xf8;
        i4E = (u2Color >> 3) & 0xfc;
        i4R = (u2Color << 3) & 0xf8;

        i4W = i4W | (i4W >> 5);
        i4E = i4E | (i4E >> 6);
        i4R = i4R | (i4R >> 5);

        *ppu1DestFb += 2;
        break;

        case CM_ARGB1555_DIRECT16:
        u2Color = *((UINT16 *)*ppu1DestFb);
        i4Q = (u2Color & 0x8000) ? 0xff : 0;
        i4W = (u2Color >> 7) & 0xf8;
        i4E = (u2Color >> 2) & 0xf8;
        i4R = (u2Color << 3) & 0xf8;

        i4W = i4W | (i4W >> 5);
        i4E = i4E | (i4E >> 5);
        i4R = i4R | (i4R >> 5);

        *ppu1DestFb += 2;
        break;

        case CM_ARGB4444_DIRECT16:
        u2Color = *((UINT16 *)*ppu1DestFb);
        i4Q = (u2Color >> 8) & 0xf0;
        i4W = (u2Color >> 4) & 0xf0;
        i4E = u2Color & 0xf0;
        i4R = (u2Color << 4) & 0xf0;

        i4Q = i4Q | (i4Q >> 4);
        i4W = i4W | (i4W >> 4);
        i4E = i4E | (i4E >> 4);
        i4R = i4R | (i4R >> 4);

        *ppu1DestFb += 2;
        break;

        case CM_ARGB8888_DIRECT32:
        u4Color = *((UINT32 *)*ppu1DestFb);
        i4Q = u4Color >> 24;
        i4W = (u4Color >> 16) & 0xff;
        i4E = (u4Color >> 8) & 0xff;
        i4R = u4Color & 0xff;
        *ppu1DestFb += 4;
        break;

        default:
        return;
    }

    pi4ColorComponent[0] = i4Q;
    pi4ColorComponent[1] = i4W;
    pi4ColorComponent[2] = i4E;
    pi4ColorComponent[3] = i4R;

    return;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwGetColorComponent_5381
 *  only for "Alpha-Composition" and "Alpha-Blending"
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e826 -e961 */
void GFX_SwGetColorComponent_5381(UINT8 **ppu1DestFb, UINT32 u4CM,
        INT32 *pi4ColorComponent)
{
    // get color components from frame buffer, advance frame buffer point for 1 pixel
    INT32 i4Q, i4W, i4E, i4R;
    UINT32 u4Color;
    UINT16 u2Color;
    UINT8 u1Color;

    if ((ppu1DestFb == NULL) || (pi4ColorComponent == NULL))
    {
        return;
    }

    switch (u4CM)
    {
        case CM_RGB_CLUT8:
        u1Color = *((UINT8 *)*ppu1DestFb);
        i4Q = u1Color & 0xff;
        i4W = u1Color & 0xff;
        i4E = u1Color & 0xff;
        i4R = u1Color & 0xff;
        *ppu1DestFb += 1;
        break;

        case CM_RGB565_DIRECT16:
        u2Color = *((UINT16 *)*ppu1DestFb);
        i4Q = 0xff;
        i4W = (u2Color >> 8) & 0xf8;
        i4E = (u2Color >> 3) & 0xfc;
        i4R = (u2Color << 3) & 0xf8;

        i4W = i4W | (i4W >> 5);
        i4E = i4E | (i4E >> 6);
        i4R = i4R | (i4R >> 5);

        *ppu1DestFb += 2;
        break;

        case CM_ARGB1555_DIRECT16:
        u2Color = *((UINT16 *)*ppu1DestFb);
        i4Q = (u2Color & 0x8000) ? 0xff : 0;
        i4W = (u2Color >> 7) & 0xf8;
        i4E = (u2Color >> 2) & 0xf8;
        i4R = (u2Color << 3) & 0xf8;

        i4W = i4W | (i4W >> 5);
        i4E = i4E | (i4E >> 5);
        i4R = i4R | (i4R >> 5);

        *ppu1DestFb += 2;
        break;

        case CM_ARGB4444_DIRECT16:
        u2Color = *((UINT16 *)*ppu1DestFb);
        i4Q = (u2Color >> 8) & 0xf0;
        i4W = (u2Color >> 4) & 0xf0;
        i4E = u2Color & 0xf0;
        i4R = (u2Color << 4) & 0xf0;

        // if (As or Ad == 15) repeat MSB, else no repeat
        // Cs and Cd, no repeat
        if (i4Q == 0xF0)
        {
            i4Q = i4Q | (i4Q >> 4);
        }

        *ppu1DestFb += 2;
        break;

        case CM_ARGB8888_DIRECT32:
        u4Color = *((UINT32 *)*ppu1DestFb);
        i4Q = u4Color >> 24;
        i4W = (u4Color >> 16) & 0xff;
        i4E = (u4Color >> 8) & 0xff;
        i4R = u4Color & 0xff;
        *ppu1DestFb += 4;
        break;

        default:
        return;
    }

    pi4ColorComponent[0] = i4Q;
    pi4ColorComponent[1] = i4W;
    pi4ColorComponent[2] = i4E;
    pi4ColorComponent[3] = i4R;

    return;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwSetRopColorComp
 *  for ROP bitblt use
 */
//-------------------------------------------------------------------------
/*lint -save -e703 -e704 -e826 -e961 */
void GFX_SwSetRopColorComp(UINT8 **ppu1DestFb, UINT32 u4CM, const UINT32 *pu4sColorComp)
{
    UINT32 u4Q, u4W, u4E, u4R;
    UINT32 u4Color;
    UINT16 u2Color;
    UINT8 u1Color;

    if ((ppu1DestFb == NULL) || (pu4sColorComp == NULL))
    {
        return;
    }

    u4Q = pu4sColorComp[0] & 0xFF;
    u4W = pu4sColorComp[1] & 0xFF;
    u4E = pu4sColorComp[2] & 0xFF;
    u4R = pu4sColorComp[3] & 0xFF;

    switch (u4CM)
    {
        case CM_RGB_CLUT8:
        u1Color = u4R & 0xff;
        *((UINT8 *)*ppu1DestFb) = u1Color;
        *ppu1DestFb += 1;
        break;

        case CM_RGB565_DIRECT16:
        u2Color = ((u4W & 0xf8) << 8) |
        ((u4E & 0xfc) << 3) |
        (u4R >> 3);
        *((UINT16 *)*ppu1DestFb) = u2Color;
        *ppu1DestFb += 2;
        break;

        case CM_ARGB1555_DIRECT16:
        u2Color = (u4Q ? 0x8000 : 0) |
        ((u4W & 0xf8) << 7) |
        ((u4E & 0xf8) << 2) |
        (u4R >> 3);
        *((UINT16 *)*ppu1DestFb) = u2Color;
        *ppu1DestFb += 2;
        break;

        case CM_ARGB4444_DIRECT16:
        u2Color = ((u4Q & 0xf0) << 8) |
        ((u4W & 0xf0) << 4) |
        (u4E & 0xf0) |
        (u4R >> 4);
        *((UINT16 *)*ppu1DestFb) = u2Color;
        *ppu1DestFb += 2;
        break;

        case CM_ARGB8888_DIRECT32:
        u4Color = (u4Q << 24) |
        (u4W << 16) |
        (u4E << 8) |
        u4R;
        *((UINT32 *)*ppu1DestFb) = u4Color;
        *ppu1DestFb += 4;
        break;

        default:
        return;
    }

    return;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwGetRopColorComp
 *  for ROP bitblt use
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e826 -e961 */
void GFX_SwGetRopColorComp(UINT8 **ppu1DestFb, UINT32 u4CM, UINT32 *pu4ColorComp)
{
    // get color components from frame buffer, advance frame buffer point for 1 pixel
    UINT32 u4Q, u4W, u4E, u4R;
    UINT32 u4Color;
    UINT16 u2Color;
    UINT8 u1Color;

    if ((ppu1DestFb == NULL) || (pu4ColorComp == NULL))
    {
        return;
    }

    switch (u4CM)
    {
        case CM_RGB_CLUT8:
        u1Color = *((UINT8 *)*ppu1DestFb);
        u4Q = u1Color & 0xff;
        u4W = u1Color & 0xff;
        u4E = u1Color & 0xff;
        u4R = u1Color & 0xff;
        *ppu1DestFb += 1;
        break;

        case CM_RGB565_DIRECT16:
        u2Color = *((UINT16 *)*ppu1DestFb);
        u4Q = 0xff;
        u4W = (u2Color >> 8) & 0xf8;
        u4E = (u2Color >> 3) & 0xfc;
        u4R = (u2Color << 3) & 0xf8;

        u4W = u4W | (u4W >> 5);
        u4E = u4E | (u4E >> 6);
        u4R = u4R | (u4R >> 5);

        *ppu1DestFb += 2;
        break;

        case CM_ARGB1555_DIRECT16:
        u2Color = *((UINT16 *)*ppu1DestFb);
        u4Q = (u2Color & 0x8000) ? 0xff : 0;
        u4W = (u2Color >> 7) & 0xf8;
        u4E = (u2Color >> 2) & 0xf8;
        u4R = (u2Color << 3) & 0xf8;

        u4W = u4W | (u4W >> 5);
        u4E = u4E | (u4E >> 5);
        u4R = u4R | (u4R >> 5);

        *ppu1DestFb += 2;
        break;

        case CM_ARGB4444_DIRECT16:
        u2Color = *((UINT16 *)*ppu1DestFb);
        u4Q = (u2Color >> 8) & 0xf0;
        u4W = (u2Color >> 4) & 0xf0;
        u4E = u2Color & 0xf0;
        u4R = (u2Color << 4) & 0xf0;

        u4Q = u4Q | (u4Q >> 4);
        u4W = u4W | (u4W >> 4);
        u4E = u4E | (u4E >> 4);
        u4R = u4R | (u4R >> 4);

        *ppu1DestFb += 2;
        break;

        case CM_ARGB8888_DIRECT32:
        u4Color = *((UINT32 *)*ppu1DestFb);
        u4Q = u4Color >> 24;
        u4W = (u4Color >> 16) & 0xff;
        u4E = (u4Color >> 8) & 0xff;
        u4R = u4Color & 0xff;
        *ppu1DestFb += 4;
        break;

        default:
        return;
    }

    pu4ColorComp[0] = u4Q;
    pu4ColorComp[1] = u4W;
    pu4ColorComp[2] = u4E;
    pu4ColorComp[3] = u4R;

    return;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwBitBlt
 *  gfx normal bitblt
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e740 -e613 */
INT32 GFX_SwBitBlt(UINT8 *pu1SrcBase, UINT8 *pu1DstBase, UINT32 u4SrcPitch,
        UINT32 u4DstPitch, UINT32 u4SrcCM, UINT32 u4DstCM, UINT32 u4Width,
        UINT32 u4Height, UINT32 u4ColorMin, UINT32 u4ColorMax, UINT32 u4TransEn,
        UINT32 u4KeynotEn, UINT32 u4ColchgEn, UINT32 u4KeySdSel, UINT32 u4Color)
{
    INT32 ai4ColorCompMin[4] =
    {   0};
    INT32 ai4ColorCompMax[4] =
    {   0};
    INT32 ai4SrcColorComp[4] =
    {   0};
    INT32 ai4DstColorComp[4] =
    {   0};
    INT32 ai4RectColor[4] =
    {   0};
    UINT32 *pu4Color;
    UINT32 x, y;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1SrcRead, *pu1DstRead, *pu1DstWrite;

    // protection
    if (u4TransEn && u4ColchgEn)
    {
        return -(INT32)E_GFX_INV_ARG;
    }

    pu1SrcLine = pu1SrcBase;
    pu1DstLine = pu1DstBase;

    if (u4TransEn || u4ColchgEn)
    {
        ai4ColorCompMin[0] = (INT32) (u4ColorMin >> 24);
        ai4ColorCompMin[1] = (INT32)((u4ColorMin >> 16) & 0xFF);
        ai4ColorCompMin[2] = (INT32)((u4ColorMin >> 8) & 0xFF);
        ai4ColorCompMin[3] = (INT32)( u4ColorMin & 0xFF);

        ai4ColorCompMax[0] = (INT32) (u4ColorMax >> 24);
        ai4ColorCompMax[1] = (INT32)((u4ColorMax >> 16) & 0xFF);
        ai4ColorCompMax[2] = (INT32)((u4ColorMax >> 8) & 0xFF);
        ai4ColorCompMax[3] = (INT32)( u4ColorMax & 0xFF);

        pu4Color = &u4Color;
        u4Color = _GfxSwColorExpansion(u4Color, u4DstCM);

        GFX_SwGetColorComponent((UINT8 **)&pu4Color, u4DstCM, ai4RectColor);
    }

    for (y = 0; y < u4Height; y++)
    {
        pu1SrcRead = pu1SrcLine;

        pu1DstRead = pu1DstLine;
        pu1DstWrite = pu1DstLine;

        for (x = 0; x < u4Width; x++)
        {
            GFX_SwGetColorComponent(&pu1SrcRead, u4SrcCM, ai4SrcColorComp);
            GFX_SwGetColorComponent(&pu1DstRead, u4DstCM, ai4DstColorComp);

            // the document says COLCHA_ENA and TRANS_ENA should not
            // set at the same time, but if we set them both, which
            // one would process by hardware ? Check it.
            //
            // hardware can NOT work, software must keep it off
            //
            //---------------------------------------------------
            if (u4KeySdSel) // key compared with destination pixel (dst color key)

            { //---------------------------------------------------
                if ((u4SrcCM == u4DstCM) && ((UINT32)CM_RGB_CLUT8 == u4SrcCM))
                {
                    if (u4TransEn)
                    {
                        if (u4KeynotEn)
                        {
                            // exactly equal color_key_min[7:0]
                            if (ai4DstColorComp[0] == ai4ColorCompMin[3])
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                            else
                            {
                                // keep dst color
                                pu1DstWrite += 1;
                            }
                        }
                        else // u4KeynotEn = 0

                        {
                            // exactly equal color_key_min[7:0]
                            if (ai4DstColorComp[0] == ai4ColorCompMin[3])
                            {
                                // keep dst color
                                pu1DstWrite += 1;
                            }
                            else
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                        }
                    }
                    else if (u4ColchgEn)
                    {
                        if (u4KeynotEn)
                        {
                            // exactly equal color_key_min[7:0]
                            if (ai4DstColorComp[0] == ai4ColorCompMin[3])
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                            else
                            {
                                // write rect color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4RectColor);
                            }
                        }
                        else // u4KeynotEn = 0

                        {
                            // exactly equal color_key_min[7:0]
                            if (ai4DstColorComp[0] == ai4ColorCompMin[3])
                            {
                                // write rect color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4RectColor);
                            }
                            else
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                        }
                    }
                    else // u4TransEn = 0 and u4ColchgEn = 0

                    { // write src color to dst
                        GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM, ai4SrcColorComp);
                    }

                }
                else // 32bpp and 16bpp

                {
                    if (u4TransEn)
                    {
                        if (u4KeynotEn)
                        {
                            if ((ai4DstColorComp[0] >= ai4ColorCompMin[0]) &&
                                    (ai4DstColorComp[0] <= ai4ColorCompMax[0]) &&
                                    (ai4DstColorComp[1] >= ai4ColorCompMin[1]) &&
                                    (ai4DstColorComp[1] <= ai4ColorCompMax[1]) &&
                                    (ai4DstColorComp[2] >= ai4ColorCompMin[2]) &&
                                    (ai4DstColorComp[2] <= ai4ColorCompMax[2]) &&
                                    (ai4DstColorComp[3] >= ai4ColorCompMin[3]) &&
                                    (ai4DstColorComp[3] <= ai4ColorCompMax[3]))
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                            else
                            {
                                // keep dst color
                                if ((UINT32)CM_ARGB8888_DIRECT32 == u4DstCM)
                                {
                                    pu1DstWrite += 4;
                                }
                                else if (((UINT32)CM_ARGB4444_DIRECT16 == u4DstCM) ||
                                        ((UINT32)CM_ARGB1555_DIRECT16 == u4DstCM) ||
                                        ((UINT32)CM_RGB565_DIRECT16 == u4DstCM))
                                {
                                    pu1DstWrite += 2;
                                }
                                else // for lint happy

                                {
                                    ;
                                }
                            }
                        }
                        else // u4KeynotEn = 0

                        {
                            if ((ai4DstColorComp[0] >= ai4ColorCompMin[0]) &&
                                    (ai4DstColorComp[0] <= ai4ColorCompMax[0]) &&
                                    (ai4DstColorComp[1] >= ai4ColorCompMin[1]) &&
                                    (ai4DstColorComp[1] <= ai4ColorCompMax[1]) &&
                                    (ai4DstColorComp[2] >= ai4ColorCompMin[2]) &&
                                    (ai4DstColorComp[2] <= ai4ColorCompMax[2]) &&
                                    (ai4DstColorComp[3] >= ai4ColorCompMin[3]) &&
                                    (ai4DstColorComp[3] <= ai4ColorCompMax[3]))
                            {
                                // keep dst color
                                if ((UINT32)CM_ARGB8888_DIRECT32 == u4DstCM)
                                {
                                    pu1DstWrite += 4;
                                }
                                else if (((UINT32)CM_ARGB4444_DIRECT16 == u4DstCM) ||
                                        ((UINT32)CM_ARGB1555_DIRECT16 == u4DstCM) ||
                                        ((UINT32)CM_RGB565_DIRECT16 == u4DstCM))
                                {
                                    pu1DstWrite += 2;
                                }
                                else // for lint happy

                                {
                                    ;
                                }
                            }
                            else
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                        }
                    }
                    else if (u4ColchgEn)
                    {
                        if (u4KeynotEn)
                        {
                            if ((ai4DstColorComp[0] >= ai4ColorCompMin[0]) &&
                                    (ai4DstColorComp[0] <= ai4ColorCompMax[0]) &&
                                    (ai4DstColorComp[1] >= ai4ColorCompMin[1]) &&
                                    (ai4DstColorComp[1] <= ai4ColorCompMax[1]) &&
                                    (ai4DstColorComp[2] >= ai4ColorCompMin[2]) &&
                                    (ai4DstColorComp[2] <= ai4ColorCompMax[2]) &&
                                    (ai4DstColorComp[3] >= ai4ColorCompMin[3]) &&
                                    (ai4DstColorComp[3] <= ai4ColorCompMax[3]))
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                            else
                            {
                                // write rect color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4RectColor);
                            }
                        }
                        else // u4KeynotEn = 0

                        {
                            if ((ai4DstColorComp[0] >= ai4ColorCompMin[0]) &&
                                    (ai4DstColorComp[0] <= ai4ColorCompMax[0]) &&
                                    (ai4DstColorComp[1] >= ai4ColorCompMin[1]) &&
                                    (ai4DstColorComp[1] <= ai4ColorCompMax[1]) &&
                                    (ai4DstColorComp[2] >= ai4ColorCompMin[2]) &&
                                    (ai4DstColorComp[2] <= ai4ColorCompMax[2]) &&
                                    (ai4DstColorComp[3] >= ai4ColorCompMin[3]) &&
                                    (ai4DstColorComp[3] <= ai4ColorCompMax[3]))
                            {
                                // write rect color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4RectColor);
                            }
                            else
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                        }
                    }
                    else // u4TransEn = 0 and u4ColchgEn = 0

                    { // write src color to dst
                        GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM, ai4SrcColorComp);
                    }
                }
            } //---------------------------------------------------

            else // key compared with source pixel (src color key)

            { //---------------------------------------------------
                if ((u4SrcCM == u4DstCM) && ((UINT32)CM_RGB_CLUT8 == u4SrcCM))
                {
                    if (u4TransEn)
                    {
                        if (u4KeynotEn)
                        {
                            // exactly equal color_key_min[7:0]
                            if (ai4SrcColorComp[0] == ai4ColorCompMin[3])
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                            else
                            {
                                // keep dst color
                                pu1DstWrite += 1;
                            }
                        }
                        else // u4KeynotEn = 0

                        {
                            // exactly equal color_key_min[7:0]
                            if (ai4SrcColorComp[0] == ai4ColorCompMin[3])
                            {
                                // keep dst color
                                pu1DstWrite += 1;
                            }
                            else
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                        }
                    }
                    else if (u4ColchgEn)
                    {
                        if (u4KeynotEn)
                        {
                            // exactly equal color_key_min[7:0]
                            if (ai4SrcColorComp[0] == ai4ColorCompMin[3])
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                            else
                            {
                                // write rect color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4RectColor);
                            }
                        }
                        else // u4KeynotEn = 0

                        {
                            // exactly equal color_key_min[7:0]
                            if (ai4SrcColorComp[0] == ai4ColorCompMin[3])
                            {
                                // write rect color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4RectColor);
                            }
                            else
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                        }
                    }
                    else // u4TransEn = 0 and u4ColchgEn = 0

                    { // write src color to dst
                        GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM, ai4SrcColorComp);
                    }

                }
                else // 32bpp and 16bpp

                {
                    if (u4TransEn)
                    {
                        if (u4KeynotEn)
                        {
                            if ((ai4SrcColorComp[0] >= ai4ColorCompMin[0]) &&
                                    (ai4SrcColorComp[0] <= ai4ColorCompMax[0]) &&
                                    (ai4SrcColorComp[1] >= ai4ColorCompMin[1]) &&
                                    (ai4SrcColorComp[1] <= ai4ColorCompMax[1]) &&
                                    (ai4SrcColorComp[2] >= ai4ColorCompMin[2]) &&
                                    (ai4SrcColorComp[2] <= ai4ColorCompMax[2]) &&
                                    (ai4SrcColorComp[3] >= ai4ColorCompMin[3]) &&
                                    (ai4SrcColorComp[3] <= ai4ColorCompMax[3]))
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                            else
                            {
                                // keep dst color
                                if ((UINT32)CM_ARGB8888_DIRECT32 == u4DstCM)
                                {
                                    pu1DstWrite += 4;
                                }
                                else if (((UINT32)CM_ARGB4444_DIRECT16 == u4DstCM) ||
                                        ((UINT32)CM_ARGB1555_DIRECT16 == u4DstCM) ||
                                        ((UINT32)CM_RGB565_DIRECT16 == u4DstCM))
                                {
                                    pu1DstWrite += 2;
                                }
                                else // for lint happy

                                {
                                    ;
                                }
                            }
                        }
                        else // u4KeynotEn = 0

                        {
                            if ((ai4SrcColorComp[0] >= ai4ColorCompMin[0]) &&
                                    (ai4SrcColorComp[0] <= ai4ColorCompMax[0]) &&
                                    (ai4SrcColorComp[1] >= ai4ColorCompMin[1]) &&
                                    (ai4SrcColorComp[1] <= ai4ColorCompMax[1]) &&
                                    (ai4SrcColorComp[2] >= ai4ColorCompMin[2]) &&
                                    (ai4SrcColorComp[2] <= ai4ColorCompMax[2]) &&
                                    (ai4SrcColorComp[3] >= ai4ColorCompMin[3]) &&
                                    (ai4SrcColorComp[3] <= ai4ColorCompMax[3]))
                            {
                                // keep dst color
                                if ((UINT32)CM_ARGB8888_DIRECT32 == u4DstCM)
                                {
                                    pu1DstWrite += 4;
                                }
                                else if (((UINT32)CM_ARGB4444_DIRECT16 == u4DstCM) ||
                                        ((UINT32)CM_ARGB1555_DIRECT16 == u4DstCM) ||
                                        ((UINT32)CM_RGB565_DIRECT16 == u4DstCM))
                                {
                                    pu1DstWrite += 2;
                                }
                                else // for lint happy

                                {
                                    ;
                                }
                            }
                            else
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                        }
                    }
                    else if (u4ColchgEn)
                    {
                        if (u4KeynotEn)
                        {
                            if ((ai4SrcColorComp[0] >= ai4ColorCompMin[0]) &&
                                    (ai4SrcColorComp[0] <= ai4ColorCompMax[0]) &&
                                    (ai4SrcColorComp[1] >= ai4ColorCompMin[1]) &&
                                    (ai4SrcColorComp[1] <= ai4ColorCompMax[1]) &&
                                    (ai4SrcColorComp[2] >= ai4ColorCompMin[2]) &&
                                    (ai4SrcColorComp[2] <= ai4ColorCompMax[2]) &&
                                    (ai4SrcColorComp[3] >= ai4ColorCompMin[3]) &&
                                    (ai4SrcColorComp[3] <= ai4ColorCompMax[3]))
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                            else
                            {
                                // write rect color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4RectColor);
                            }
                        }
                        else // u4KeynotEn = 0

                        {
                            if ((ai4SrcColorComp[0] >= ai4ColorCompMin[0]) &&
                                    (ai4SrcColorComp[0] <= ai4ColorCompMax[0]) &&
                                    (ai4SrcColorComp[1] >= ai4ColorCompMin[1]) &&
                                    (ai4SrcColorComp[1] <= ai4ColorCompMax[1]) &&
                                    (ai4SrcColorComp[2] >= ai4ColorCompMin[2]) &&
                                    (ai4SrcColorComp[2] <= ai4ColorCompMax[2]) &&
                                    (ai4SrcColorComp[3] >= ai4ColorCompMin[3]) &&
                                    (ai4SrcColorComp[3] <= ai4ColorCompMax[3]))
                            {
                                // write rect color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4RectColor);
                            }
                            else
                            {
                                // write src color to dst
                                GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM,
                                        ai4SrcColorComp);
                            }
                        }
                    }
                    else // u4TransEn = 0 and u4ColchgEn = 0

                    { // write src color to dst
                        GFX_SwSetColorComponent(&pu1DstWrite, u4DstCM, ai4SrcColorComp);
                    }
                }
            }
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;

    } // ~for

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwAlphaMapBitBlt
 *  SRC CM = RGB8 (4'b1010)
 *  DST CM = ARGB8888/ARGB4444/ARGB1555
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
INT32 GFX_SwAlphaMapBitBlt(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        UINT32 u4SrcPitch, UINT32 u4DstPitch, UINT32 u4SrcCM, UINT32 u4DstCM,
        UINT32 u4Width, UINT32 u4Height)
{
    UINT8 u1Color, u1Alpha;
    UINT8 *pu1SrcPtr, *pu1DstPtr;
    UINT32 u4DstPW; // dst pixel width
    UINT32 i, j;

    // check parameters' error
    VERIFY(pu1SrcBase != NULL);
    VERIFY(pu1DstBase != NULL);

    if (u4SrcCM != (UINT32)CM_RGB_CLUT8)
    {
        return -(INT32)E_GFX_INV_ARG;
    }

    switch (u4DstCM)
    {
        case CM_ARGB8888_DIRECT32:
        u4DstPW = 4; // 1 pixel = 4 bytes
        for (j = 0; j < u4Height; j++)
        {
            pu1SrcPtr = (UINT8 *)(pu1SrcBase + (j * u4SrcPitch));
            pu1DstPtr = (UINT8 *)(pu1DstBase + (j * u4DstPitch));

            for (i = 0; i < u4Width; i++)
            {
                *(pu1DstPtr + (i * u4DstPW) + 3) = *(pu1SrcPtr + i);
            }
        }
        break;

        case CM_ARGB4444_DIRECT16:
        u4DstPW = 2; // 1 pixel = 2 bytes
        for (j = 0; j < u4Height; j++)
        {
            pu1SrcPtr = (UINT8 *)(pu1SrcBase + (j * u4SrcPitch));
            pu1DstPtr = (UINT8 *)(pu1DstBase + (j * u4DstPitch));

            for (i = 0; i < u4Width; i++)
            {
                // preserve MSB 4-bit
                u1Alpha = *(pu1SrcPtr + i) & 0xF0;
                u1Color = *(pu1DstPtr + (i * u4DstPW) + 1) & 0x0F;
                *(pu1DstPtr + (i * u4DstPW) + 1) = (u1Alpha | u1Color);
            }
        }
        break;

        case CM_ARGB1555_DIRECT16:
        u4DstPW = 2; // 1 pixel = 2 bytes
        for (j = 0; j < u4Height; j++)
        {
            pu1SrcPtr = (UINT8 *)(pu1SrcBase + (j * u4SrcPitch));
            pu1DstPtr = (UINT8 *)(pu1DstBase + (j * u4DstPitch));

            for (i = 0; i < u4Width; i++)
            {
                u1Alpha = (*(pu1SrcPtr + i) == 0) ? 0x0 : 0x80;
                u1Color = *(pu1DstPtr + (i * u4DstPW) + 1) & 0x7F;
                *(pu1DstPtr + (i * u4DstPW) + 1) = (u1Alpha | u1Color);
            }
        }
        break;

        default:
        return -(INT32)E_GFX_INV_ARG;

    } // end of switch (u4DstCM)

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwAlphaComposePass
 *  gfx alpha composition
 *  Pass0 ~ Pass3
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e613 */
INT32 GFX_SwAlphaComposePass_5381(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        UINT32 u4SrcPitch, UINT32 u4DstPitch, UINT32 u4SrcCM, UINT32 u4DstCM,
        UINT32 u4Width, UINT32 u4Height, UINT32 u4AlphaValue, UINT32 u4AlcomPass)
{
    UINT32 u4Alpha, u4Fs, u4Fd, x, y;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    INT32 ai4SrcColor[4], ai4DstColor[4], i;
    INT32 i4As, i4Ad, i4TmpValue;

    UNUSED(u4SrcCM);

    u4Alpha = u4AlphaValue; // Ar
    u4Alpha = (u4Alpha == 255) ? 256 : u4Alpha;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    switch (u4AlcomPass)
    {
        case 0: // Pass 0 (As * Ar)
        for (y = 0; y < u4Height; y++)
        {
            for (x = 0; x < u4Width; x++)
            {
                GFX_SwGetColorComponent_5381(&pu1Read, u4DstCM, ai4SrcColor);

                if (ai4SrcColor[0] == 255)
                {
                    i4As = 256;
                }
                else
                {
                    i4As = ai4SrcColor[0];
                }

                ai4DstColor[0] = i4As * (INT32)u4Alpha; // As * Ar
                ai4DstColor[1] = ai4SrcColor[1];
                ai4DstColor[2] = ai4SrcColor[2];
                ai4DstColor[3] = ai4SrcColor[3];

                // Rounding
                if (u4DstCM == CM_ARGB4444_DIRECT16)
                {
                    if ((ai4DstColor[0] >> 11) & 1)
                    {
                        ai4DstColor[0] = (ai4DstColor[0] >> 12) + 1;
                        ai4DstColor[0] = (ai4DstColor[0] << 4);
                    }
                    else
                    {
                        ai4DstColor[0] = (ai4DstColor[0] >> 8);
                    }
                }
                else
                {
                    if ((ai4DstColor[0] >> 7) & 1)
                    {
                        ai4DstColor[0] = (ai4DstColor[0] >> 8) + 1;
                    }
                    else
                    {
                        ai4DstColor[0] = (ai4DstColor[0] >> 8);
                    }
                }

                GFX_SwSetColorComponent(&pu1Write, u4DstCM, ai4DstColor);
            }

            pu1SrcLine += u4SrcPitch;
            pu1DstLine += u4DstPitch;
            pu1Read = pu1SrcLine;
            pu1Write = pu1DstLine;
        }
        break;

        case 1: // Pass 1
        for (y = 0; y < u4Height; y++)
        {
            for (x = 0; x < u4Width; x++)
            {
                GFX_SwGetColorComponent_5381(&pu1Read, u4DstCM, ai4SrcColor);

                if (ai4SrcColor[0] == 255)
                {
                    i4As = 256;
                }
                else
                {
                    i4As = ai4SrcColor[0];
                }

                ai4DstColor[0] = ai4SrcColor[0];

                for (i = 1; i < 4; i++)
                {
                    // As' * Cs
                    ai4DstColor[i] = i4As * ai4SrcColor[i];

                    // Rounding
                    if (u4DstCM == CM_ARGB4444_DIRECT16)
                    {
                        if ((ai4DstColor[i] >> 11) & 1)
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 12) + 1;
                            ai4DstColor[i] = (ai4DstColor[i] << 4);
                        }
                        else
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 8);
                        }
                    }
                    else
                    {
                        if ((ai4DstColor[i] >> 7) & 1)
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 8) + 1;
                        }
                        else
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 8);
                        }
                    }
                }

                GFX_SwSetColorComponent(&pu1Write, u4DstCM, ai4DstColor);
            }

            pu1SrcLine += u4SrcPitch;
            pu1DstLine += u4DstPitch;
            pu1Read = pu1SrcLine;
            pu1Write = pu1DstLine;
        }
        break;

        case 2: // Pass 2
        for (y = 0; y < u4Height; y++)
        {
            for (x = 0; x < u4Width; x++)
            {
                pu1Dst = pu1Write;

                GFX_SwGetColorComponent_5381(&pu1Read, u4DstCM, ai4SrcColor);
                GFX_SwGetColorComponent_5381(&pu1Dst, u4DstCM, ai4DstColor);

                // As
                if (ai4SrcColor[0] == 255)
                {
                    i4As = 256;
                }
                else
                {
                    i4As = ai4SrcColor[0];
                }

                // Ad
                if (ai4DstColor[0] == 255)
                {
                    i4Ad = 256;
                }
                else
                {
                    i4Ad = ai4DstColor[0];
                }

                // PD rules
                switch (u4Alpha)
                {
                    case E_AC_CLEAR: // 0
                    u4Fs = 0;
                    u4Fd = 0;
                    break;

                    case E_AC_DST_IN: // 1
                    u4Fs = 0;
                    u4Fd = (UINT32)(i4As);
                    break;

                    case E_AC_DST_OUT: // 2
                    u4Fs = 0;
                    u4Fd = (UINT32)(256 - i4As);
                    break;

                    case E_AC_DST_OVER: // 3
                    u4Fs = (UINT32)(256 - i4Ad);
                    u4Fd = 256;
                    break;

                    case E_AC_SRC: // 4
                    u4Fs = 256;
                    u4Fd = 0;
                    break;

                    case E_AC_SRC_IN: // 5
                    u4Fs = (UINT32)(i4Ad);
                    u4Fd = 0;
                    break;

                    case E_AC_SRC_OUT: // 6
                    u4Fs = (UINT32)(256 - i4Ad);
                    u4Fd = 0;
                    break;

                    case E_AC_SRC_OVER: // 7
                    u4Fs = 256;
                    u4Fd = (UINT32)(256 - i4As);
                    break;

                    default:
                    return -(INT32)E_GFX_INV_ARG;
                }

                for(i = 0; i < 4; i++)
                {
                    if (i == 0)
                    {
                        ai4DstColor[i] = ((INT32)u4Fs * i4As) +
                        ((INT32)u4Fd * i4Ad);
                    }
                    else
                    {
                        ai4DstColor[i] = ((INT32)u4Fs * ai4SrcColor[i]) +
                        ((INT32)u4Fd * ai4DstColor[i]);
                    }

                    // Rounding
                    if (u4DstCM == CM_ARGB4444_DIRECT16)
                    {
                        if ((ai4DstColor[i] >> 11) & 1)
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 12) + 1;
                            ai4DstColor[i] = (ai4DstColor[i] << 4);
                        }
                        else
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 8);
                        }
                    }
                    else
                    {
                        if ((ai4DstColor[i] >> 7) & 1)
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 8) + 1;
                        }
                        else
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 8);
                        }
                    }
                }

                GFX_SwSetColorComponent(&pu1Write, u4DstCM, ai4DstColor);
            }

            pu1SrcLine += u4SrcPitch;
            pu1DstLine += u4DstPitch;
            pu1Read = pu1SrcLine;
            pu1Write = pu1DstLine;
        }
        break;

        case 3: // Pass 3
        for (y = 0; y < u4Height; y++)
        {
            for (x = 0; x < u4Width; x++)
            {
                GFX_SwGetColorComponent_5381(&pu1Read, u4DstCM, ai4SrcColor);

                // As
                if (ai4SrcColor[0] == 255)
                {
                    i4As = 256;
                }
                else
                {
                    i4As = ai4SrcColor[0];
                }

                ai4DstColor[0] = ai4SrcColor[0]; // Ad = As

                for (i = 1; i < 4; i++)
                {
                    if (i4As == 256)
                    {
                        ai4DstColor[i] = ai4SrcColor[i];
                    }
                    else if (i4As)
                    {
                        i4TmpValue = (ai4SrcColor[i] * _i4sAlphaInvTbl[i4As]);
                        ai4DstColor[i] = (i4TmpValue >> _i4sAlphaInvShtTbl[i4As]);

                        // No Rounding (because rounding in _i4sAlphaInvTbl)
                        //    ai4DstColor[i] = (ai4DstColor[i] >> 3);
#if 1
                        if ((ai4DstColor[i] >> 2) & 1)
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 3) + 1;
                        }
                        else
                        {
                            ai4DstColor[i] = (ai4DstColor[i] >> 3);
                        }

                        // rounding for 8-bpp to 4-bpp
                        if ((u4DstCM == (UINT32)CM_ARGB4444_DIRECT16) ||
                                (u4DstCM == (UINT32)CM_ARGB1555_DIRECT16) ||
                                (u4DstCM == (UINT32)CM_RGB565_DIRECT16))
                        {
                            if ((ai4DstColor[i] >> 3) & 1)
                            {
                                ai4DstColor[i] = ai4DstColor[i] + (0x1 << 4);
                            }
                        }
#endif
                    }
                    else
                    {
                        ai4DstColor[i] = 0;
                    }
                }

                GFX_SwSetColorComponent(&pu1Write, u4DstCM, ai4DstColor);
            }

            pu1SrcLine += u4SrcPitch;
            pu1DstLine += u4DstPitch;
            pu1Read = pu1SrcLine;
            pu1Write = pu1DstLine;
        }
        break;

        case 4: // Pass 4   (Alpha-BitBlt)
        for (y = 0; y < u4Height; y++)
        {
            for (x = 0; x < u4Width; x++)
            {
                pu1Dst = pu1Write;

                GFX_SwGetColorComponent(&pu1Read, u4DstCM, ai4SrcColor);
                GFX_SwGetColorComponent(&pu1Dst, u4DstCM, ai4DstColor);

                ai4DstColor[0] = ai4SrcColor[0]; // Ad = As

                GFX_SwSetColorComponent(&pu1Write, u4DstCM, ai4DstColor);
            }

            pu1SrcLine += u4SrcPitch;
            pu1DstLine += u4DstPitch;
            pu1Read = pu1SrcLine;
            pu1Write = pu1DstLine;
        }
        break;

        case 5: // Pass 5   (Color-BitBlt)
        for (y = 0; y < u4Height; y++)
        {
            for (x = 0; x < u4Width; x++)
            {
                pu1Dst = pu1Write;

                GFX_SwGetColorComponent(&pu1Read, u4DstCM, ai4SrcColor);
                GFX_SwGetColorComponent(&pu1Dst, u4DstCM, ai4DstColor);

                for (i = 1; i < 4; i++)
                {
                    ai4DstColor[i] = ai4SrcColor[i]; // Cd = Cs
                }

                GFX_SwSetColorComponent(&pu1Write, u4DstCM, ai4DstColor);
            }

            pu1SrcLine += u4SrcPitch;
            pu1DstLine += u4DstPitch;
            pu1Read = pu1SrcLine;
            pu1Write = pu1DstLine;
        }
        break;

        default:
        return -(INT32)E_GFX_INV_ARG;
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwAlphaCompositionLoop
 *  gfx alpha composition (loop mode)
 *
 */
//-------------------------------------------------------------------------
unsigned long round_u_long(unsigned long i, int sht)
{
    unsigned long result;
    result = i;
    result = result >> (sht-1);
    if ((result % 2) == 1)
    {
        result = (result/2) + 1;
    }
    else
    {
        result = (result/2);
    }

    return result;
}

UINT64 round_u_long_long(UINT64 i, int sht)
{
    UINT64 result;
    result = i;
    result = result >> (sht - 1);
    if ((result % 2) == 1)
    {
        result = (result/2) + 1;
    }
    else
    {
        result = (result/2);
    }

    return result;
}

/*lint -save -e704 -e613 */
INT32 GFX_SwAlphaCompositionLoop(const GFX_ACLM_DATA_T *prsData)
{
    int i, h, w, m;
    int op_code;
    int pass1_sht_num = 8;
    int pass2_sht_num = 16;
    int height, width;
    int src_p_zero = 0;
    int dst_p_zero = 0;
    int color_mode;
    int S_D_SCAN_SHT;
    int PASS3_SHT_NUM_REAL;
    int PASS3_ROUND_LEVEL;
    int ALPHA_SHT_LEVEL;

    unsigned long tmp1, tmp3, tmp4, tmp5, tmp6;
    unsigned long pixel_s[4], pixel_d[4];
    unsigned long csn_pass1[3], cdn_pass1[3];
    unsigned long As, Ad;
    unsigned long Ar = 65; //setting range in 0~256
    unsigned long Fs, Fd;
    unsigned long F_1 = 65536;
    unsigned long A_1 = 256;
    unsigned long An_long;
    unsigned long An_long_index;

    unsigned long pixel_res[4];
    unsigned long MAX_ALPHA_VALUE;
    unsigned long An_step;
    unsigned long CLAMP_LEVEL;

    unsigned long long cdn_pass2[3];
    unsigned long long cdn_pass3[3];
    unsigned long long An;
    unsigned long long tmp2, An_approxi, tmp_cdn_pass2[3];

    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4RectSrc, u4RectColor;
    INT32 ai4SrcColor[COMP_NUM];
    INT32 ai4DstColor[COMP_NUM];
    INT32 ai4RectColor[COMP_NUM];
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 *pu4Color;

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    width = prsData->u4Width;
    height = prsData->u4Height;
    Ar = prsData->u4AlComAr;
    op_code = prsData->u4AlComOpCode;
    u4RectSrc = prsData->u4AlComRectSrc;
    u4RectColor = prsData->u4RectColor;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    // --------------- linear approximation start ---------------
    if (_u4IsTableExist == (UINT32)FALSE)
    {
        _inverse_table_value[0] = 0;
        _inverse_table_slope[0] = 0;
        _inverse_table_shift[0] = PASS3_SHT_NUM_1;
        _alpha_error_distr[0] = 0.0;

        for (m = 1; m < (TABLE_SIZE+1); m++)
        {
            _alpha_error_distr[m] = 0.0;
            i = 0;
            tmp1 = F_1 / m;
            tmp4 = F_1;
            while (tmp1 <= F_1/2)
            {
                tmp4 = tmp4 << 1;
                tmp1 = tmp4 / m;
                i++;
            }

#if 0
            //=============================================
            tmp4 = (F_1<<(i+1))/m;
            _inverse_table_value[m] = round_u_long(tmp4, 1);
            _inverse_table_shift[m] = i+PASS3_SHT_NUM_1;
            tmp5 = (F_1<<(i+1))/m;
            tmp6 = (F_1<<(i+1))/(m+1);
            tmp4 = (tmp5-tmp6)>>4;
            _inverse_table_slope[m] = round_u_long(tmp4, 1);
            //=============================================
#else
            //=============================================
            tmp4 = (F_1 << (i+2)) / m; //one more bit
            _inverse_table_value[m] = round_u_long(tmp4, 1);
            _inverse_table_shift[m] = i + PASS3_SHT_NUM_1;
            tmp5 = (F_1 << (i+2)) / m;
            tmp6 = (F_1 << (i+2)) / (m+1);
            tmp4 = (tmp5 - tmp6) >> 4;
            _inverse_table_slope[m] = round_u_long(tmp4, 1);
            //=============================================
#endif
        }

        _u4IsTableExist = (UINT32)TRUE;
    }
    // --------------- linear approximation end ---------------

    if (u4DstCM == CM_ARGB8888_DIRECT32)
    {
        color_mode = 0;
    }
    else
    {
        color_mode = 1;
    }

    if (color_mode == 0) //32bpp

    {
        S_D_SCAN_SHT = 0;
        MAX_ALPHA_VALUE = 255;
        PASS3_SHT_NUM_REAL = PASS3_SHT_NUM_1+12;
        PASS3_ROUND_LEVEL = 0;
        ALPHA_SHT_LEVEL = ALPHA_SHT;
        CLAMP_LEVEL = 255;
    }

    if (color_mode == 1) //16bpp

    {
        S_D_SCAN_SHT = 4;
        MAX_ALPHA_VALUE = 240;
        PASS3_SHT_NUM_REAL = PASS3_SHT_NUM_1+16;
        PASS3_ROUND_LEVEL = 4;
        ALPHA_SHT_LEVEL = ALPHA_SHT_16BPP;
        CLAMP_LEVEL = 15;
    }

    if (Ar == 255)
    {
        Ar = 256;
    }

    if (u4RectSrc) // use Rect_Color[31:0] as Src pixel values

    {
        pu4Color = &(u4RectColor);
        GFX_SwGetColorComponent_5381((UINT8 **)&pu4Color, u4DstCM, ai4RectColor);
    }

    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            pu1Dst = pu1Write;

            if (u4RectSrc)
            {
                // use Rect Color as Src pixel values
                pixel_s[3] = (UINT32)(ai4RectColor[0]);
                pixel_s[2] = (UINT32)(ai4RectColor[1]);
                pixel_s[1] = (UINT32)(ai4RectColor[2]);
                pixel_s[0] = (UINT32)(ai4RectColor[3]);
            }
            else
            {
                // Src pixel values
                GFX_SwGetColorComponent_5381(&pu1Read, u4DstCM, ai4SrcColor);
                pixel_s[3] = (UINT32)(ai4SrcColor[0]);
                pixel_s[2] = (UINT32)(ai4SrcColor[1]);
                pixel_s[1] = (UINT32)(ai4SrcColor[2]);
                pixel_s[0] = (UINT32)(ai4SrcColor[3]);
            }

            // Dst pixel values
            GFX_SwGetColorComponent_5381(&pu1Dst, u4DstCM, ai4DstColor);
            pixel_d[3] = (UINT32)(ai4DstColor[0]);
            pixel_d[2] = (UINT32)(ai4DstColor[1]);
            pixel_d[1] = (UINT32)(ai4DstColor[2]);
            pixel_d[0] = (UINT32)(ai4DstColor[3]);

            if (pixel_s[3] >= MAX_ALPHA_VALUE)
            {
                pixel_s[3] = A_1; //only alpha pushed to 1.0
            }

            if (pixel_d[3] >= MAX_ALPHA_VALUE)
            {
                pixel_d[3] = A_1; //only alpha pushed to 1.0
            }

            // Common operation
            As = pixel_s[3];
            Ad = pixel_d[3];

            // pass 0, As*Ar
            As = As * Ar;
            Ad = Ad * A_1; //9x9, 17-bit data, no loss

            if (As == 0)
            {
                src_p_zero = 1;
            }
            else
            {
                src_p_zero = 0; //src zero flag
            }

            if (Ad == 0)
            {
                dst_p_zero = 1;
            }
            else
            {
                dst_p_zero = 0; //dst zero flag
            }

            // pass 1, color * Fs, Fd
            ////////////////////////////////
            ///CLEAR
            if (op_code == 0)
            {
                Fs = 0;
                Fd = 0;
            } //end of op_code

            ////////////////////////////////
            ///DST_IN
            if (op_code == 1)
            {
                Fs = 0;
                Fd = As;
            } //end of op_code

            ////////////////////////////////
            ///DST_OUT
            if (op_code == 2)
            {
                Fs = 0;
                Fd = F_1 - As;
            } //end of op_code

            ////////////////////////////////
            ///DST_OVER
            if (op_code == 3)
            {
                Fs = F_1 - Ad;
                Fd = F_1;
            } //end of op_code

            ////////////////////////////////
            ///SRC
            if (op_code == 4)
            {
                Fs = F_1;
                Fd = 0;
            } //end of op_code

            ////////////////////////////////
            ///SRC_IN
            if (op_code == 5)
            {
                Fs = Ad;
                Fd = 0;
            } //end of op_code

            ////////////////////////////////
            ///SRC_OUT
            if (op_code == 6)
            {
                Fs = F_1 - Ad;
                Fd = 0;
            } //end of op_code

            ////////////////////////////////
            ///SRC_OVER
            if (op_code == 7)
            {
                Fs = F_1;
                Fd = F_1 - As;
            } //end of op_code

            if (Fs == 0)
            {
                src_p_zero = 0x1 | src_p_zero; //src zero flag
            }

            if (Fd == 0)
            {
                dst_p_zero = 0x1 | dst_p_zero; //dst zero flag
            }

            for (i = 0; i < 3; i++)
            {
                if (dst_p_zero)
                {
                    tmp1 = F_1 * pixel_s[i]; //17x9, 25-bit data
                }
                else
                {
                    tmp1 = Fs * pixel_s[i]; //17x9, 25-bit data
                }
                csn_pass1[i] = round_u_long(tmp1, pass1_sht_num); //25->17bit

                if (src_p_zero)
                {
                    tmp1 = F_1 * pixel_d[i]; //17x9, 25-bit data
                }
                else
                {
                    tmp1 = Fd * pixel_d[i]; //17x9, 25-bit data
                }
                cdn_pass1[i] = round_u_long(tmp1, pass1_sht_num); //25->17bit

            }

            // pass 2, color tmp * alpha tmp
            An = (unsigned long long) Fs*As + (unsigned long long) Fd*Ad; //17x17, 33+1 bit
            An = An >> pass2_sht_num; //34(33) bit->17bit

            for (i = 0; i < 3; i++)
            {
                if (dst_p_zero)
                {
                    tmp1 = F_1;
                }
                else
                {
                    tmp1 = As;
                }

                if (src_p_zero)
                {
                    tmp3 = F_1;
                }
                else
                {
                    tmp3 = Ad;
                }
                tmp_cdn_pass2[i] = (unsigned long long) tmp1*csn_pass1[i] + (unsigned long long) tmp3*cdn_pass1[i]; //17x17, 33+1 bit

                cdn_pass2[i] = round_u_long_long(tmp_cdn_pass2[i], pass2_sht_num); //34->17bit
            }

            // pass 3
            An_long = An; //17-bit
            An_step = An_long & 0xf; //last 4-bit
            An_long_index = An_long >> DIV_SHT_1;

            if (An_long_index >= TABLE_SIZE)
            {
                An_long_index = TABLE_SIZE;
            }

            for (i = 0; i < 3; i++)
            {
                if (src_p_zero || dst_p_zero)
                {
                    if (An_long_index == 0)
                    {
                        tmp2 = 0; //17x17, 33 bit
                    }
                    else
                    {
                        tmp2 = F_1 * cdn_pass2[i]; //17x17, 33 bit
                    }
                }
                else
                {
                    An_approxi = (_inverse_table_value[An_long_index]-_inverse_table_slope[An_long_index]*An_step);
                    An_approxi = An_approxi >> 1;
                    tmp2 = An_approxi * cdn_pass2[i]; //17x17, 33 bit
                }

                if (src_p_zero || dst_p_zero)
                {
                    cdn_pass3[i] = tmp2 >> PASS3_SHT_NUM_REAL;
                }
                else
                {
                    cdn_pass3[i] = round_u_long_long(tmp2, _inverse_table_shift[An_long_index]+PASS3_ROUND_LEVEL); //33->9bit
                }
            }

            An_long = round_u_long(An_long, ALPHA_SHT_LEVEL);
            if (An_long >= CLAMP_LEVEL)
            {
                An_long = CLAMP_LEVEL;
            }

            pixel_res[3] = An_long;

            for (i = 0; i < 3; i++)
            {
                pixel_res[i] = cdn_pass3[i];
                if (pixel_res[i] >= CLAMP_LEVEL)
                {
                    pixel_res[i] = CLAMP_LEVEL;
                }
            }

            if (pixel_res[3] == 0)
            {
                pixel_res[2] = 0;
                pixel_res[1] = 0;
                pixel_res[0] = 0;
            }

            // Dst pixel values
            ai4DstColor[0] = (INT32)(pixel_res[3] << S_D_SCAN_SHT);
            ai4DstColor[1] = (INT32)(pixel_res[2] << S_D_SCAN_SHT);
            ai4DstColor[2] = (INT32)(pixel_res[1] << S_D_SCAN_SHT);
            ai4DstColor[3] = (INT32)(pixel_res[0] << S_D_SCAN_SHT);

            GFX_SwSetColorComponent(&pu1Write, u4DstCM, ai4DstColor);

        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;

    } //~for

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwBlock2Linear
 *  re-arrange memory position from block mode to linear mode
 *  "Linear" means YC 420/422 separate raster
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
INT32 GFX_SwBlock2Linear(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        INT32 i4Width, INT32 i4Height, INT32 i4MBWidth, INT32 i4MBHeight)
{
    UINT8 *pu1SrcPtr, *pu1DstPtr, *pu1Anchor;
    INT32 i4MBSize, i4Vert_MB_Num, i4Hori_MB_Num;
    INT32 i, j, l;

    // check parameters' error
    VERIFY(pu1SrcBase != NULL);
    VERIFY(pu1DstBase != NULL);

    i4MBSize = i4MBWidth * i4MBHeight;
    i4Hori_MB_Num = i4Width / i4MBWidth;
    i4Vert_MB_Num = i4Height / i4MBHeight;
    pu1DstPtr = pu1DstBase;

    for (j = 0; j < i4Vert_MB_Num; j++)
    {
        pu1Anchor = pu1SrcBase + (j * i4Hori_MB_Num * i4MBSize);

        for(l = 0; l < i4MBHeight; l++)
        {
            pu1SrcPtr = pu1Anchor + (l * i4MBWidth);

            for (i = 0; i < i4Hori_MB_Num; i++)
            {
                GFX_UNUSED_RET(x_memcpy(pu1DstPtr, pu1SrcPtr,
                                (SIZE_T)i4MBWidth))

                pu1DstPtr += i4MBWidth;
                pu1SrcPtr += i4MBSize;
            }
        }
    } // ~for (j = 0; j < i4Vert_MB_Num; j++)

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwBlock2Swap
 *  re-arrange memory position from block mode to swap mode
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
INT32 GFX_SwBlock2Swap(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        INT32 i4Width, INT32 i4Height, INT32 i4MBWidth, INT32 i4MBHeight)
{
    INT32 i, j;
    INT32 i4MBSize, i4Vert_MB_Num, i4Hori_MB_Num;
    UINT8 *pu1SrcPtr, *pu1DstPtr, *pu1Anchor;

    // check parameters' error
    VERIFY(pu1SrcBase != NULL);
    VERIFY(pu1DstBase != NULL);

    i4MBSize = i4MBWidth * i4MBHeight;
    i4Hori_MB_Num = i4Width / i4MBWidth;
    i4Vert_MB_Num = i4Height / i4MBHeight;
    pu1DstPtr = pu1DstBase;

    for (j = 0; j < i4Vert_MB_Num; j++)
    {
        pu1Anchor = pu1SrcBase + (j * i4Hori_MB_Num * i4MBSize);

        for (i = 0; i < i4Hori_MB_Num; i+=2)
        {
            pu1SrcPtr = pu1Anchor + ((i + 1) * i4MBSize);
            GFX_UNUSED_RET(x_memcpy(pu1DstPtr, pu1SrcPtr, (SIZE_T)i4MBSize))
            pu1DstPtr += i4MBSize;

            pu1SrcPtr = pu1Anchor + ((i + 0) * i4MBSize);
            GFX_UNUSED_RET(x_memcpy(pu1DstPtr, pu1SrcPtr, (SIZE_T)i4MBSize))
            pu1DstPtr += i4MBSize;
        }
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwBlock2Mergetop
 *  re-arrange memory position from block mode to mergetop mode
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e794 */
INT32 GFX_SwBlock2Mergetop(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        INT32 i4Width, INT32 i4Height, INT32 i4MBWidth, INT32 i4MBHeight)
{
    INT32 i, j, l;
    INT32 i4MBSize, i4Vert_MB_Num, i4Hori_MB_Num;
    UINT8 *pu1SrcPtr, *pu1DstPtr, *pu1Anchor, *pu1TempPtr;

    // check parameters' error
    VERIFY(pu1SrcBase != NULL);
    VERIFY(pu1DstBase != NULL);

    i4MBSize = i4MBWidth * i4MBHeight;
    i4Hori_MB_Num = i4Width / i4MBWidth;
    i4Vert_MB_Num = i4Height / i4MBHeight;
    pu1DstPtr = pu1DstBase;

    for (j = 0; j < i4Vert_MB_Num; j++)
    {
        pu1Anchor = pu1SrcBase + (j * i4Hori_MB_Num * i4MBSize);

        for (i = 0; i < i4Hori_MB_Num; i++)
        {
            pu1SrcPtr = pu1Anchor + (i * i4MBSize);

            // process odd lines
            for (l = 0; l < i4MBHeight; l+=2)
            {
                pu1TempPtr = pu1SrcPtr + (l * i4MBWidth);

                GFX_UNUSED_RET(x_memcpy(pu1DstPtr, pu1TempPtr,
                                (SIZE_T)i4MBWidth))

                pu1DstPtr += i4MBWidth;
            }

            // process even lines
            for (l = 1; l < i4MBHeight; l+=2)
            {
                pu1TempPtr = pu1SrcPtr + (l * i4MBWidth);

                GFX_UNUSED_RET(x_memcpy(pu1DstPtr, pu1TempPtr,
                                (SIZE_T)i4MBWidth))

                pu1DstPtr += i4MBWidth;
            }
        }
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwYCbCrLinearScan
 *  do YCbCr 420/422 linear scan
 *  (Y, Cb, Cr range from 16 to 235)
 */
//-------------------------------------------------------------------------
/*lint -save -e522 -e613 -e961 */
INT32 GFX_SwYCbCrLinearScan(const GFX_YCBCR2RGB_DATA_T *prsData)
{
    UINT8 *pLumaBase, *pLumaPtr;
    UINT8 *pChromaBase, *pChromaPtr;
    UINT8 *pDstBase, *pDstPtr;
    UINT32 u4LumaPitch, u4ChromaPitch, u4DstPitch;
    UINT32 u4DstCM, i, j;
    UINT32 u4Width, u4Height;
    INT32 ai4Argb8888[4];
    INT32 (*pfnEquation)(const GFX_YCBCR_T *prsSrc, INT32 *pi4ColorComponent);
    GFX_YCBCR_T ycbcr1, ycbcr2;
    UINT32 u4YCFormat, u4Divisor;

    pLumaBase = prsData->pu1LumaBase;
    pChromaBase = prsData->pu1ChromaBase;
    pDstBase = prsData->pu1DstBase;
    u4LumaPitch = prsData->u4LumaPitch;
    u4ChromaPitch = prsData->u4ChromaPitch;
    u4DstPitch = prsData->u4DstPitch;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;
    u4YCFormat = prsData->u4YCFormat;

    // check parameters' error
    VERIFY(pLumaBase != NULL);
    VERIFY(pChromaBase != NULL);
    VERIFY(pDstBase != NULL);
    VERIFY(u4LumaPitch == u4ChromaPitch);

    // choose one equation
    if ((UINT32)E_VSTD_BT601 == prsData->u4VideoStd)
    {
        if ((UINT32)E_VSYS_VID == prsData->u4VideoSys)
        {
            pfnEquation = GFX_SwYCbCr601toArgbVideoEqu;
        }
        else if ((UINT32)E_VSYS_COMP == prsData->u4VideoSys)
        {
            pfnEquation = GFX_SwYCbCr601toArgbCompEqu;
        }
        else
        {
            return -(INT32)E_GFX_INV_ARG;
        }
    }
    else if ((UINT32)E_VSTD_BT709 == prsData->u4VideoStd)
    {
        if ((UINT32)E_VSYS_VID == prsData->u4VideoSys)
        {
            pfnEquation = GFX_SwYCbCr709toArgbVideoEqu;
        }
        else if ((UINT32)E_VSYS_COMP == prsData->u4VideoSys)
        {
            pfnEquation = GFX_SwYCbCr709toArgbCompEqu;
        }
        else
        {
            return -(INT32)E_GFX_INV_ARG;
        }
    }
    else
    {
        return -(INT32)E_GFX_INV_ARG;
    }

    if (u4YCFormat == (UINT32)E_YCFMT_420LINEAR)
    {
        u4Divisor = 2;
    }

    if (u4YCFormat == (UINT32)E_YCFMT_422LINEAR)
    {
        u4Divisor = 1;
    }

    for (j = 0; j < u4Height; j++)
    {
        pLumaPtr = pLumaBase + (j * u4LumaPitch);
        pChromaPtr = pChromaBase + ((j / u4Divisor) * u4ChromaPitch);
        pDstPtr = pDstBase + (j * u4DstPitch);

        for (i = 0; i < u4Width; i += 2)
        {
            ycbcr2.cb = ycbcr1.cb = *(pChromaPtr + i + 0);
            ycbcr2.cr = ycbcr1.cr = *(pChromaPtr + i + 1);

            ycbcr1.y = *(pLumaPtr + i + 0);
            ycbcr2.y = *(pLumaPtr + i + 1);

            if (GFX_ENABLE == prsData->u4VideoClip)
            {
                // Y1
                ycbcr1.y = (ycbcr1.y > 235) ? 235 :
                (ycbcr1.y < 16) ? 16 : ycbcr1.y;
                ycbcr1.cb = (ycbcr1.cb > 240) ? 240 :
                (ycbcr1.cb < 16) ? 16 : ycbcr1.cb;
                ycbcr1.cr = (ycbcr1.cr > 240) ? 240 :
                (ycbcr1.cr < 16) ? 16 : ycbcr1.cr;
                // Y2
                ycbcr2.y = (ycbcr2.y > 235) ? 235 :
                (ycbcr2.y < 16) ? 16 : ycbcr2.y;
                ycbcr2.cb = (ycbcr2.cb > 240) ? 240 :
                (ycbcr2.cb < 16) ? 16 : ycbcr2.cb;
                ycbcr2.cr = (ycbcr2.cr > 240) ? 240 :
                (ycbcr2.cr < 16) ? 16 : ycbcr2.cr;
            }
            else if ((UINT32)E_VSYS_COMP == prsData->u4VideoSys)
            {
                // Y1
                //ycbcr1.y  = (ycbcr1.y  <  16) ? 16 : ycbcr1.y;
                if (ycbcr1.y < 16)
                {
                    ycbcr1.y = 16;
                }

                // Y2
                //ycbcr2.y  = (ycbcr2.y  <  16) ? 16 : ycbcr2.y;
                if (ycbcr2.y < 16)
                {
                    ycbcr2.y = 16;
                }
            }
            else // for lint happy

            {
                ;
            }

            (*pfnEquation)(&ycbcr1, ai4Argb8888);
            GFX_SwSetColorComponent(&pDstPtr, u4DstCM, ai4Argb8888);

            (*pfnEquation)(&ycbcr2, ai4Argb8888);
            GFX_SwSetColorComponent(&pDstPtr, u4DstCM, ai4Argb8888);
        }
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwYCbCr601toArgbVideoEqu
 *  Y(601)CbCr -> ARGB video system equation
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e834 -e961 -e613 */
INT32 GFX_SwYCbCr601toArgbVideoEqu(const GFX_YCBCR_T *prsSrc,
        INT32 *pi4ColorComponent)
{
    INT32 i4Temp = 0;
    INT32 i4Y, i4Cb, i4Cr;

    i4Y = (INT32)prsSrc->y;
    i4Cb = (INT32)prsSrc->cb;
    i4Cr = (INT32)prsSrc->cr;
    pi4ColorComponent[0] = (INT32)0xFF;

    // Integer Version (like HW)
    i4Temp = (128 * i4Y) + (175 * (i4Cr - 128));
    i4Temp = i4Temp >> 7; // i4Temp / 128
    pi4ColorComponent[1] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    i4Temp = (128 * i4Y) - (43 * (i4Cb - 128)) - (89 * (i4Cr - 128));
    i4Temp = i4Temp >> 7; // i4Temp / 128
    pi4ColorComponent[2] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    i4Temp = (128 * i4Y) + (222 * (i4Cb - 128));
    i4Temp = i4Temp >> 7; // i4temp / 128
    pi4ColorComponent[3] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwYCbCr601toArgbCompEqu
 *  Y(601)CbCr -> ARGB computer system equation
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e834 -e961 -e613 */
INT32 GFX_SwYCbCr601toArgbCompEqu(const GFX_YCBCR_T *prsSrc,
        INT32 *pi4ColorComponent)
{
    INT32 i4Temp = 0;
    INT32 i4Y, i4Cb, i4Cr;

    i4Y = (INT32)prsSrc->y;
    i4Cb = (INT32)prsSrc->cb;
    i4Cr = (INT32)prsSrc->cr;
    pi4ColorComponent[0] = (INT32)0xFF;

    // Integer Version (like HW)
    i4Temp = (149 * (i4Y - 16)) + (204 * (i4Cr - 128));
    i4Temp = i4Temp >> 7; // i4Temp / 128
    pi4ColorComponent[1] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    i4Temp = (149 * (i4Y - 16)) - (50 * (i4Cb - 128)) - (104 * (i4Cr - 128));
    i4Temp = i4Temp >> 7; // i4Temp / 128
    pi4ColorComponent[2] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    i4Temp = (149 * (i4Y - 16)) + (258 * (i4Cb - 128));
    i4Temp = i4Temp >> 7; // i4Temp / 128
    pi4ColorComponent[3] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwYCbCr709toArgbVideoEqu
 *  Y(709)CbCr -> ARGB video system equation
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e834 -e961 -e613 */
INT32 GFX_SwYCbCr709toArgbVideoEqu(const GFX_YCBCR_T *prsSrc,
        INT32 *pi4ColorComponent)
{
    INT32 i4Temp = 0;
    INT32 i4Y, i4Cb, i4Cr;

    i4Y = (INT32)prsSrc->y;
    i4Cb = (INT32)prsSrc->cb;
    i4Cr = (INT32)prsSrc->cr;
    pi4ColorComponent[0] = (INT32)0xFF;

    // Integer Version (like HW)
    i4Temp = (128 * i4Y) + (197 * (i4Cr - 128));
    i4Temp = i4Temp >> 7; // i4Temp / 128
    pi4ColorComponent[1] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    i4Temp = (128 * i4Y) - (23 * (i4Cb - 128)) - (59 * (i4Cr - 128));
    i4Temp = i4Temp >> 7; // i4Temp / 128
    pi4ColorComponent[2] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    i4Temp = (128 * i4Y) + (232 * (i4Cb - 128));
    i4Temp = i4Temp >> 7; // i4Temp / 128
    pi4ColorComponent[3] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwYCbCr709toArgbCompEqu
 *  Y(709)CbCr -> ARGB computer system equation
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e834 -e961 -e613 */
INT32 GFX_SwYCbCr709toArgbCompEqu(const GFX_YCBCR_T *prsSrc,
        INT32 *pi4ColorComponent)
{
    INT32 i4Temp = 0;
    INT32 i4Y, i4Cb, i4Cr;

    i4Y = (INT32)prsSrc->y;
    i4Cb = (INT32)prsSrc->cb;
    i4Cr = (INT32)prsSrc->cr;
    pi4ColorComponent[0] = (INT32)0xFF;

    // Integer Version (like HW)
    i4Temp = (149 * (i4Y - 16)) + (230 * (i4Cr - 128));
    i4Temp = i4Temp >> 7; // ftemp / 128
    pi4ColorComponent[1] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    i4Temp = (149 * (i4Y - 16)) - (27 * (i4Cb - 128)) - (68 * (i4Cr - 128));
    i4Temp = i4Temp >> 7; // ftemp / 128
    pi4ColorComponent[2] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    i4Temp = (149 * (i4Y - 16)) + (271 * (i4Cb - 128));
    i4Temp = i4Temp >> 7; // ftemp / 128
    pi4ColorComponent[3] = (i4Temp > 255) ? 255 :
    (i4Temp < 0) ? 0 : i4Temp;

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwYCbCr2RGB
 *  do YCbCr to RGB
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 */
INT32 GFX_SwYCbCr2RGB(const GFX_YCBCR2RGB_DATA_T *prsData)
{
    // check parameters' error
    VERIFY(prsData != NULL);
    VERIFY(prsData->pu1LumaBase != NULL);
    VERIFY(prsData->pu1ChromaBase != NULL);
    VERIFY(prsData->pu1DstBase != NULL);

    // MT537x and MT5381 does NOT support 422 Linear Scan
    // MT5382 supports 422 Linear Scan
    if ((prsData->u4YCFormat == (UINT32)E_YCFMT_420LINEAR) ||
            (prsData->u4YCFormat == (UINT32)E_YCFMT_422LINEAR))
    {
        GFX_UNUSED_RET(GFX_SwYCbCrLinearScan(prsData))
    }
    else if (prsData->u4YCFormat == (UINT32)E_YCFMT_420MB)
    {
        ;
    }
    else // for lint happy

    {
        ;
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwAlphaBlending
 *  for 5371 use
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e613 */
INT32 GFX_SwAlphaBlending(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        UINT32 u4SrcPitch, UINT32 u4DstPitch, UINT32 u4Dst_CM, UINT32 u4Alpha,
        UINT32 u4Width, UINT32 u4Height)
{
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 u4Alpha1, x, y;
    INT32 ai4SrcColor[4], ai4DstColor[4], i;

    // get source & destination info
    u4Alpha = (u4Alpha == 255) ? 256 : u4Alpha; // Ar
    u4Alpha1 = (u4Alpha) ? (256 - u4Alpha) : 256; // (1-Ar)

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    VERIFY(pu1SrcBase != NULL);
    VERIFY(pu1DstBase != NULL);

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetColorComponent(&pu1Read, u4Dst_CM, ai4SrcColor);
            GFX_SwGetColorComponent(&pu1Dst, u4Dst_CM, ai4DstColor);

            for (i = 0; i < 4; i++)
            {
                ai4SrcColor[i] = (ai4SrcColor[i] * (INT32)u4Alpha);
                ai4DstColor[i] = (ai4DstColor[i] * (INT32)u4Alpha1);

                ai4DstColor[i] += ai4SrcColor[i];

                // Rounding
                if ((ai4DstColor[i] >> 7) & 1)
                {
                    ai4DstColor[i] = (ai4DstColor[i] >> 8) + 1;
                }
                else
                {
                    ai4DstColor[i] = (ai4DstColor[i] >> 8);
                }
            }

            GFX_SwSetColorComponent(&pu1Write, u4Dst_CM, ai4DstColor);
        }

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwAlphaBlending_5381
 *  for 5381 use
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e613 */
INT32 GFX_SwAlphaBlending_5381(UINT8 *pu1SrcBase, UINT8 *pu1DstBase,
        UINT32 u4SrcPitch, UINT32 u4DstPitch, UINT32 u4Dst_CM, UINT32 u4Alpha,
        UINT32 u4Width, UINT32 u4Height)
{
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 u4Alpha1, x, y;
    INT32 ai4SrcColor[4], ai4DstColor[4], i;
    INT32 i4As, i4Ad;

    // get source & destination info
    u4Alpha = (u4Alpha == 255) ? 256 : u4Alpha; // Ar
    u4Alpha1 = (u4Alpha) ? (256 - u4Alpha) : 256; // (1-Ar)

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    VERIFY(pu1SrcBase != NULL);
    VERIFY(pu1DstBase != NULL);

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetColorComponent_5381(&pu1Read, u4Dst_CM, ai4SrcColor);
            GFX_SwGetColorComponent_5381(&pu1Dst, u4Dst_CM, ai4DstColor);

            for (i = 0; i < 4; i++)
            {
                if (i == 0) // As, Ad

                {
                    if (ai4SrcColor[i] == 255)
                    {
                        i4As = 256;
                    }
                    else
                    {
                        i4As = ai4SrcColor[i];
                    }

                    if (ai4DstColor[i] == 255)
                    {
                        i4Ad = 256;
                    }
                    else
                    {
                        i4Ad = ai4DstColor[i];
                    }

                    ai4SrcColor[i] = (i4As * (INT32)u4Alpha);
                    ai4DstColor[i] = (i4Ad * (INT32)u4Alpha1);
                }
                else
                {
                    ai4SrcColor[i] = (ai4SrcColor[i] * (INT32)u4Alpha);
                    ai4DstColor[i] = (ai4DstColor[i] * (INT32)u4Alpha1);
                }

                ai4DstColor[i] += ai4SrcColor[i];

                // Rounding
                if (u4Dst_CM == CM_ARGB4444_DIRECT16)
                {
                    if ((ai4DstColor[i] >> 11) & 1)
                    {
                        ai4DstColor[i] = (ai4DstColor[i] >> 12) + 1;
                        ai4DstColor[i] = (ai4DstColor[i] << 4);
                    }
                    else
                    {
                        ai4DstColor[i] = (ai4DstColor[i] >> 8);
                    }
                }
                else
                {
                    if ((ai4DstColor[i] >> 7) & 1)
                    {
                        ai4DstColor[i] = (ai4DstColor[i] >> 8) + 1;
                    }
                    else
                    {
                        ai4DstColor[i] = (ai4DstColor[i] >> 8);
                    }
                }
            }

            GFX_SwSetColorComponent(&pu1Write, u4Dst_CM, ai4DstColor);
        }

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwBigEndianAndLittleEndianConversion
 *
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e826 -e613 */
INT32 GFX_SwBigEndianAndLittleEndianConversion(UINT8 *pu1DstAddr,
        UINT8 *pu1SrcAddr, UINT32 u4TotalPixels, UINT32 u4ColorMode)
{
    UINT32 p, u4Color;
    UINT32 u4Byte0, u4Byte1, u4Byte2, u4Byte3;
    UINT32 *pu4DstPtr, *pu4SrcPtr;

    UINT16 u2Color;
    UINT16 *pu2DstPtr, *pu2SrcPtr;

    switch (u4ColorMode)
    {
        case CM_RGB565_DIRECT16:
        break;

        case CM_ARGB1555_DIRECT16:
        break;

        case CM_ARGB4444_DIRECT16:
        pu2DstPtr = ((UINT16 *)pu1DstAddr);
        pu2SrcPtr = ((UINT16 *)pu1SrcAddr);

        for (p = 0; p < u4TotalPixels; p++)
        {
            u2Color = *pu2SrcPtr;

            // Seperate
            u4Byte0 = u2Color >> 12;
            u4Byte1 = (u2Color >> 8) & 0xf;
            u4Byte2 = (u2Color >> 4) & 0xf;
            u4Byte3 = u2Color & 0xf;

            // Merge
            u2Color = ((u4Byte3 << 12) | (u4Byte2 << 8) |
                    (u4Byte1 << 4) | (u4Byte0));

            *pu2DstPtr = u2Color;

            pu2DstPtr++;
            pu2SrcPtr++;
        }
        break;

        case CM_ARGB8888_DIRECT32:
        pu4DstPtr = ((UINT32 *)pu1DstAddr);
        pu4SrcPtr = ((UINT32 *)pu1SrcAddr);

        for (p = 0; p < u4TotalPixels; p++)
        {
            u4Color = *pu4SrcPtr;

            // Seperate
            u4Byte0 = u4Color >> 24;
            u4Byte1 = (u4Color >> 16) & 0xff;
            u4Byte2 = (u4Color >> 8) & 0xff;
            u4Byte3 = u4Color & 0xff;

            // Merge
            u4Color = ((u4Byte3 << 24) | (u4Byte2 << 16) |
                    (u4Byte1 << 8) | (u4Byte0));

            *pu4DstPtr = u4Color;

            pu4DstPtr++;
            pu4SrcPtr++;
        }
        break;

        default:
        return -(INT32)E_GFX_INV_ARG;
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwGradientFill
 *
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
INT32 GFX_SwGradientFill(const GFX_GRADIENT_DATA_T *prsData)
{
    UINT8 *pu1Write;
    UINT32 *pu4Color;
    UINT32 u4XInc, u4YInc;
    UINT32 u4Width, u4Height;
    UINT32 u4DstCM, u4RectColor, u4GradMode;
    INT32 i4ColorComponent[4], i4TempColor[4];
    INT32 i4DeltaX[4], i4DeltaY[4], i4Temp[4];
    INT32 i, j, xinc = 0, yinc = 0;

    // check parameters' error
    VERIFY(prsData != NULL);
    VERIFY(prsData->pu1DstBase != NULL);

    pu1Write = prsData->pu1DstBase;
    u4XInc = prsData->u4X_Pix_Inc;
    u4YInc = prsData->u4Y_Pix_Inc;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;
    u4DstCM = prsData->u4DstCM;
    u4RectColor = prsData->u4RectColor;
    u4GradMode = prsData->u4GradMode;

    pu4Color = &u4RectColor;
    u4RectColor = _GfxSwColorExpansion(u4RectColor, u4DstCM);

    GFX_SwGetColorComponent((UINT8 **)&pu4Color, u4DstCM, i4ColorComponent);

    i4DeltaX[0] = ((INT8)prsData->u1Delta_X_C0) / 2; // B
    i4DeltaX[1] = ((INT8)prsData->u1Delta_X_C1) / 2; // G
    i4DeltaX[2] = ((INT8)prsData->u1Delta_X_C2) / 2; // R
    i4DeltaX[3] = ((INT8)prsData->u1Delta_X_C3) / 2; // A

    i4DeltaY[0] = ((INT8)prsData->u1Delta_Y_C0) / 2; // B
    i4DeltaY[1] = ((INT8)prsData->u1Delta_Y_C1) / 2; // G
    i4DeltaY[2] = ((INT8)prsData->u1Delta_Y_C2) / 2; // R
    i4DeltaY[3] = ((INT8)prsData->u1Delta_Y_C3) / 2; // A

    for (j = 0; j < u4Height; j++)
    {
        yinc = (j / u4YInc);
        pu1Write = prsData->pu1DstBase + (j * prsData->u4DstPitch);

        for (i = 0; i < u4Width; i++)
        {
            xinc = (i / u4XInc);

            i4TempColor[0] = i4ColorComponent[0];
            i4TempColor[1] = i4ColorComponent[1];
            i4TempColor[2] = i4ColorComponent[2];
            i4TempColor[3] = i4ColorComponent[3];

            // calculate vertical direction first
            if (u4GradMode & (UINT32)E_GRAD_VER)
            {
                i4Temp[3] = (i4DeltaY[3] * yinc); // A
                i4Temp[2] = (i4DeltaY[2] * yinc); // R
                i4Temp[1] = (i4DeltaY[1] * yinc); // G
                i4Temp[0] = (i4DeltaY[0] * yinc); // B

                i4TempColor[0] = GFX_SwClipping(i4TempColor[0] + i4Temp[3]);
                i4TempColor[1] = GFX_SwClipping(i4TempColor[1] + i4Temp[2]);
                i4TempColor[2] = GFX_SwClipping(i4TempColor[2] + i4Temp[1]);
                i4TempColor[3] = GFX_SwClipping(i4TempColor[3] + i4Temp[0]);
            }

            if (u4GradMode & (UINT32)E_GRAD_HOR)
            {
                i4Temp[3] = (i4DeltaX[3] * xinc); // A
                i4Temp[2] = (i4DeltaX[2] * xinc); // R
                i4Temp[1] = (i4DeltaX[1] * xinc); // G
                i4Temp[0] = (i4DeltaX[0] * xinc); // B

                i4TempColor[0] = GFX_SwClipping(i4TempColor[0] + i4Temp[3]);
                i4TempColor[1] = GFX_SwClipping(i4TempColor[1] + i4Temp[2]);
                i4TempColor[2] = GFX_SwClipping(i4TempColor[2] + i4Temp[1]);
                i4TempColor[3] = GFX_SwClipping(i4TempColor[3] + i4Temp[0]);
            }

            // write value to dst
            GFX_SwSetColorComponent(&pu1Write, u4DstCM, i4TempColor);
        }
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_NotSrc
 *  ROP #04 : ~SRC
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_NotSrc(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            //GFX_SwGetRopColorComp(&pu1Dst,  u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = ~src
                au4DstColor[i] = (~au4SrcColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_NotDst
 *  ROP #05 : ~DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_NotDst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            //GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = ~dst
                au4DstColor[i] = (~au4DstColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_Src_Xor_Dst
 *  ROP #06 : SRC XOR DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_Src_Xor_Dst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = src ^ dst
                au4DstColor[i] = (au4SrcColor[i] ^ au4DstColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_Src_XNor_Dst
 *  ROP #07 : SRC XNOR DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_Src_XNor_Dst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = ~(src ^ dst)
                au4DstColor[i] = ~(au4SrcColor[i] ^ au4DstColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_Src_And_Dst
 *  ROP #08 : SRC & DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_Src_And_Dst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = src & dst
                au4DstColor[i] = (au4SrcColor[i] & au4DstColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_NotSrc_And_Dst
 *  ROP #09 : ~SRC & DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_NotSrc_And_Dst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = ~src & dst
                au4DstColor[i] = (~au4SrcColor[i]) & au4DstColor[i];
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_Src_And_NotDst
 *  ROP #10 : SRC & ~DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_Src_And_NotDst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = src & ~dst
                au4DstColor[i] = au4SrcColor[i] & (~au4DstColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_NotSrc_And_NotDst
 *  ROP #11 : ~SRC & ~DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_NotSrc_And_NotDst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = ~src & ~dst
                au4DstColor[i] = (~au4SrcColor[i]) & (~au4DstColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_Src_Or_Dst
 *  ROP #12 : SRC | DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_Src_Or_Dst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = src | dst
                au4DstColor[i] = (au4SrcColor[i] | au4DstColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_NotSrc_Or_Dst
 *  ROP #13 : ~SRC & DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_NotSrc_Or_Dst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = ~src | dst
                au4DstColor[i] = (~au4SrcColor[i]) | au4DstColor[i];
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_Src_Or_NotDst
 *  ROP #14 : SRC | ~DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_Src_Or_NotDst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = src | ~dst
                au4DstColor[i] = au4SrcColor[i] | (~au4DstColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRop_NotSrc_Or_NotDst
 *  ROP #15 : ~SRC | ~DST
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
void GFX_SwRop_NotSrc_Or_NotDst(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y, i;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write, *pu1Dst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            pu1Dst = pu1Write;

            GFX_SwGetRopColorComp(&pu1Read, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1Dst, u4DstCM, au4DstColor);

            for (i = 0; i < 4; i++)
            {
                // dst = ~src | ~dst
                au4DstColor[i] = (~au4SrcColor[i]) | (~au4DstColor[i]);
            }

            GFX_SwSetRopColorComp(&pu1Write, u4DstCM, au4DstColor);
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1Read = pu1SrcLine;
        pu1Write = pu1DstLine;
    } // ~for
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwRopBitBlt
 *  ROP: raster operation (bit-wise)
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
INT32 GFX_SwRopBitBlt(const GFX_ROP_DATA_T *prsData)
{
    // check parameters' error
    VERIFY(prsData != NULL);
    VERIFY(prsData->pu1SrcBase != NULL);
    VERIFY(prsData->pu1DstBase != NULL);

    switch (prsData->u4RopOpCode)
    {
        // rop #04
        case E_ROP_NOT_SRC:
        GFX_SwRop_NotSrc(prsData);
        break;

        // rop #05
        case E_ROP_NOT_DST:
        GFX_SwRop_NotDst(prsData);
        break;

        // rop #06
        case E_ROP_SRC_XOR_DST:
        GFX_SwRop_Src_Xor_Dst(prsData);
        break;

        // rop #07
        case E_ROP_SRC_XNOR_DST:
        GFX_SwRop_Src_XNor_Dst(prsData);
        break;

        // rop #08
        case E_ROP_SRC_AND_DST:
        GFX_SwRop_Src_And_Dst(prsData);
        break;

        // rop #09
        case E_ROP_NOT_SRC_AND_DST:
        GFX_SwRop_NotSrc_And_Dst(prsData);
        break;

        // rop #10
        case E_ROP_SRC_AND_NOT_DST:
        GFX_SwRop_Src_And_NotDst(prsData);
        break;

        // rop #11
        case E_ROP_NOT_SRC_AND_NOT_DST:
        GFX_SwRop_NotSrc_And_NotDst(prsData);
        break;

        // rop #12
        case E_ROP_SRC_OR_DST:
        GFX_SwRop_Src_Or_Dst(prsData);
        break;

        // rop #13
        case E_ROP_NOT_SRC_OR_DST:
        GFX_SwRop_NotSrc_Or_Dst(prsData);
        break;

        // rop #14
        case E_ROP_SRC_OR_NOT_DST:
        GFX_SwRop_Src_Or_NotDst(prsData);
        break;

        // rop #15
        case E_ROP_NOT_SRC_OR_NOT_DST:
        GFX_SwRop_NotSrc_Or_NotDst(prsData);
        break;

        default:
        return (INT32)E_GFX_INV_ARG;
    }

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwMemCompare
 *  memory compare operation
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
INT32 GFX_SwMemCompare(const GFX_ROP_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 x, y;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1ReadSrc, *pu1ReadDst;
    UINT32 au4SrcColor[4], au4DstColor[4];

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1ReadSrc = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1ReadDst = pu1DstBase;
    pu1DstLine = pu1DstBase;

    VERIFY(u4SrcCM == (UINT32)CM_RGB_CLUT8);
    VERIFY(u4DstCM == (UINT32)CM_RGB_CLUT8);

    GFX_SwSetMemCompareResult(0);

    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            GFX_SwGetRopColorComp(&pu1ReadSrc, u4SrcCM, au4SrcColor);
            GFX_SwGetRopColorComp(&pu1ReadDst, u4DstCM, au4DstColor);

            // memory compare
            if (au4SrcColor[0] != au4DstColor[0])
            {
                LOG(5, "[Cm] Memory Compare Error : \n");
                LOG(5, "the first error pixel -> ");
                LOG(5, "DstX = %u, DstY = %u\n", x, y);
                LOG(5, "Src Color = 0x%02x, Dst Color = 0x%02x\n",
                        au4SrcColor[0], au4DstColor[0]);

                GFX_SwSetMemCompareResult(1);

                return (INT32)E_GFX_OK;
            }
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1DstLine += u4DstPitch;
        pu1ReadSrc = pu1SrcLine;
        pu1ReadDst = pu1DstLine;
    } // ~for

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwGetMemCompareResult
 *  gfx get memory compare result
 *  if no error, return 0
 */
//-------------------------------------------------------------------------
void GFX_SwSetMemCompareResult(INT32 i4Val)
{
    _i4MemCompFlag = i4Val;
}

//-------------------------------------------------------------------------
/** GFX_SwGetMemCompareResult
 *  gfx get memory compare result
 *  if no error, return 0
 */
//-------------------------------------------------------------------------
INT32 GFX_SwGetMemCompareResult(void)
{
    return _i4MemCompFlag;
}

//-------------------------------------------------------------------------
/** GFX_SwSetIdx2DirComp
 *  for index to direct color bitblt use
 */
//-------------------------------------------------------------------------
void GFX_SwSetIdx2DirComp(UINT8 **ppu1DestFb, UINT32 u4DstCM,
        const UINT8 *pu1PaleBase, UINT32 u4PaleIdx)
{
    UINT32 *pu4PaleTable;
    UINT16 *pu2PaleTable;
    UINT32 u4Color;
    UINT16 u2Color;

    ASSERT(ppu1DestFb != NULL);
    ASSERT(pu1PaleBase != NULL);

    switch (u4DstCM)
    {
        case CM_RGB565_DIRECT16:
        case CM_ARGB1555_DIRECT16:
        case CM_ARGB4444_DIRECT16:
        pu2PaleTable = (UINT16 *)pu1PaleBase;
        u2Color = pu2PaleTable[u4PaleIdx];
        *((UINT16 *)*ppu1DestFb) = u2Color;
        *ppu1DestFb += 2;
        break;

        case CM_ARGB8888_DIRECT32:
        pu4PaleTable = (UINT32 *)pu1PaleBase;
        u4Color = pu4PaleTable[u4PaleIdx];
        *((UINT32 *)*ppu1DestFb) = u4Color;
        *ppu1DestFb += 4;
        break;

        default:
        return;
    }

    return;
}

//-------------------------------------------------------------------------
/** GFX_SwGetIdx2DirComp
 *  for index to direct color bitblt use
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e826 -e961 */
void GFX_SwGetIdx2DirComp(UINT8 **ppu1DestFb, UINT8 *pu1SrcByte)
{
    ASSERT(ppu1DestFb != NULL);
    ASSERT(pu1SrcByte != NULL);

    *pu1SrcByte = *((UINT8 *)*ppu1DestFb);
    *ppu1DestFb += 1;

    return;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwGetPaleIdx
 *  get palette table index
 *  for index to direct color bitblt use
 */
//-------------------------------------------------------------------------
UINT32 GFX_SwGetPaleIdx(UINT32 u4CharCM, UINT32 u4ShiftBit, UINT8 u1SrcByte)
{
    UINT32 u4Mask;
    UINT32 u4PaleIdx;

    switch (u4CharCM)
    {
        // src_cm = 1 bpp
        case E_BMP_CM_1BIT:
        u4Mask = 0x1;
        break;

        // src_cm = 2 bpp
        case E_BMP_CM_2BIT:
        u4Mask = 0x3;
        break;

        // src_cm = 4 bpp
        case E_BMP_CM_4BIT:
        u4Mask = 0xF;
        break;

        // src_cm = 8 bpp
        case E_BMP_CM_8BIT:
        u4Mask = 0xFF;
        break;

        default:
        return 0xFFFFFFFF;
    } // ~switch

    u4PaleIdx = (((UINT32)u1SrcByte >> u4ShiftBit) & u4Mask);

    // for debug use
    switch (u4CharCM)
    {
        // src_cm = 1 bpp
        case E_BMP_CM_1BIT:
        if (u4PaleIdx >= 2)
        {
            LOG(5, "Error: (u4PaleIdx >= 2)\n");
        }
        break;

        // src_cm = 2 bpp
        case E_BMP_CM_2BIT:
        if (u4PaleIdx >= 4)
        {
            LOG(5, "Error: (u4PaleIdx >= 4)\n");
        }
        break;

        // src_cm = 4 bpp
        case E_BMP_CM_4BIT:
        if (u4PaleIdx >= 16)
        {
            LOG(5, "Error: (u4PaleIdx >= 16)\n");
        }
        break;

        // src_cm = 8 bpp
        case E_BMP_CM_8BIT:
        if (u4PaleIdx >= 256)
        {
            LOG(5, "Error: (u4PaleIdx >= 256)\n");
        }
        break;

        default:
        return 0xFFFFFFFF;
    } // ~switch

    return u4PaleIdx;
}

//-------------------------------------------------------------------------
/** GFX_SwIndexToDirectBitBlt
 *  Index to Direct Color Bitblt
 *  CharCM = 1, 2, 4, 8 bpp
 *  DstCM  = 16, 32 bpp
 */
//-------------------------------------------------------------------------
/*lint -save -e704 -e613 */
INT32 GFX_SwIndexToDirectBitBlt(const GFX_IDX2DIR_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase, *pu1PaleBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4CharCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 u4ByteAlign, u4MsbLeft;
    UINT32 i, y, g, sht;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write;
    UINT32 u4BitNumPerByte, u4BitNumPerPix;
    UINT32 u4PixPerByte, u4PixGroup, u4Remainder;
    UINT32 u4PaleIdx;
    UINT8 u1SrcByte;

    // check parameters' error
    VERIFY(prsData != NULL);
    VERIFY(prsData->pu1SrcBase != NULL);
    VERIFY(prsData->pu1DstBase != NULL);

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch; // Ignore it.
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4CharCM = prsData->u4CharCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    pu1PaleBase = prsData->pu1PaleBase;
    u4MsbLeft = prsData->u4MsbLeft;
    u4ByteAlign = prsData->u4ByteAlign; // LN_ST_BYTE_AL

    u4BitNumPerByte = 8; // bit_num / byte

    switch (prsData->u4CharCM)
    {
        // src_cm = 1 bpp
        case E_BMP_CM_1BIT:
        u4BitNumPerPix = 1; // bit_num / pix
        break;

        // src_cm = 2 bpp
        case E_BMP_CM_2BIT:
        u4BitNumPerPix = 2; // bit_num / pix
        break;

        // src_cm = 4 bpp
        case E_BMP_CM_4BIT:
        u4BitNumPerPix = 4; // bit_num / pix
        break;

        // src_cm = 8 bpp
        case E_BMP_CM_8BIT:
        u4BitNumPerPix = 8; // bit_num / pix
        break;

        default:
        return (INT32)E_GFX_INV_ARG;
    } // ~switch

    // pixels / byte = (bit_num/byte) / (bit_num/pix)
    u4PixPerByte = u4BitNumPerByte / u4BitNumPerPix;

    u4Remainder = 0;
    u4Remainder = (u4Width % u4PixPerByte);
    u4PixGroup = (u4Width / u4PixPerByte);

    if (u4Remainder > 0)
    {
        if (u4ByteAlign != 1)
        {
            LOG(5, "Error: (u4Remainder > 0), but (u4ByteAlign != 1)\n");
        }
    }

    pu1Read = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    for (y = 0; y < u4Height; y++)
    {
        for (g = 0; g < u4PixGroup; g++)
        {
            GFX_SwGetIdx2DirComp(&pu1Read, &u1SrcByte);

            if (u4MsbLeft == 1)
            {
                for (i = 0, sht = u4BitNumPerByte - u4BitNumPerPix;
                        i < u4PixPerByte;
                        i++, sht -= u4BitNumPerPix)
                {
                    u4PaleIdx = GFX_SwGetPaleIdx(u4CharCM, sht, u1SrcByte);

                    GFX_SwSetIdx2DirComp(&pu1Write, u4DstCM, pu1PaleBase, u4PaleIdx);
                } // ~for
            }
            else
            {
                for (i = 0, sht = 0;
                        i < u4PixPerByte;
                        i++, sht += u4BitNumPerPix)
                {
                    u4PaleIdx = GFX_SwGetPaleIdx(u4CharCM, sht, u1SrcByte);

                    GFX_SwSetIdx2DirComp(&pu1Write, u4DstCM, pu1PaleBase, u4PaleIdx);
                } // ~for
            }
        } // ~for

        if (u4ByteAlign == 1)
        {
            GFX_SwGetIdx2DirComp(&pu1Read, &u1SrcByte);

            // deal with the remainder pixels
            if (u4MsbLeft == 1)
            {
                for (i = 0, sht = u4BitNumPerByte - u4BitNumPerPix;
                        i < u4Remainder;
                        i++, sht -= u4BitNumPerPix)
                {
                    u4PaleIdx = GFX_SwGetPaleIdx(u4CharCM, sht, u1SrcByte);

                    GFX_SwSetIdx2DirComp(&pu1Write, u4DstCM, pu1PaleBase, u4PaleIdx);
                } // ~for
            }
            else
            {
                for (i = 0, sht = 0;
                        i < u4Remainder;
                        i++, sht += u4BitNumPerPix)
                {
                    u4PaleIdx = GFX_SwGetPaleIdx(u4CharCM, sht, u1SrcByte);

                    GFX_SwSetIdx2DirComp(&pu1Write, u4DstCM, pu1PaleBase, u4PaleIdx);
                } // ~for
            }
        }

        pu1DstLine += u4DstPitch;
        pu1Write = pu1DstLine;
    } // ~for

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwHoriLineToVertLine
 *  Horizontal Line to Vertical Line
 *
 */
//-------------------------------------------------------------------------
/*lint -save -e613 -e740 */
INT32 GFX_SwHoriLineToVertLine(const GFX_HORI2VERT_DATA_T *prsData)
{
    UINT8 *pu1SrcBase, *pu1DstBase;
    UINT32 u4SrcPitch, u4DstPitch;
    UINT32 u4SrcCM, u4DstCM;
    UINT32 u4Width, u4Height;
    UINT32 u4DstPitchDec;
    UINT32 x, y;
    UINT8 *pu1SrcLine, *pu1DstLine;
    UINT8 *pu1Read, *pu1Write;
    UINT32 au4SrcColor[4];

    // check parameters' error
    VERIFY(prsData != NULL);
    VERIFY(prsData->pu1SrcBase != NULL);
    VERIFY(prsData->pu1DstBase != NULL);
    VERIFY(prsData->u4Height == 1);
    VERIFY(prsData->u4StrDstWidth == 1);

    pu1SrcBase = prsData->pu1SrcBase;
    u4SrcPitch = prsData->u4SrcPitch;
    pu1DstBase = prsData->pu1DstBase;
    u4DstPitch = prsData->u4DstPitch;
    u4SrcCM = prsData->u4SrcCM;
    u4DstCM = prsData->u4DstCM;
    u4Width = prsData->u4Width;
    u4Height = prsData->u4Height;

    u4DstPitchDec = prsData->u4DstPitchDec;

    pu1Read = pu1SrcBase;
    pu1SrcLine = pu1SrcBase;
    pu1Write = pu1DstBase;
    pu1DstLine = pu1DstBase;

    // u4Height must be equal to 1
    for (y = 0; y < u4Height; y++)
    {
        for (x = 0; x < u4Width; x++)
        {
            GFX_SwGetColorComponent(&pu1Read, u4SrcCM, au4SrcColor);

            GFX_SwSetColorComponent(&pu1Write, u4DstCM, au4SrcColor);

            if (u4DstPitchDec == (UINT32)TRUE)
            {
                pu1DstLine -= u4DstPitch;
                pu1Write = pu1DstLine;
            }
            else
            {
                pu1DstLine += u4DstPitch;
                pu1Write = pu1DstLine;
            }
        } // ~for

        pu1SrcLine += u4SrcPitch;
        pu1Read = pu1SrcLine;
    } // ~for

    return (INT32)E_GFX_OK;
}
/*lint -restore */

//-------------------------------------------------------------------------
/** GFX_SwClipping
 *  Clip value within 0 ~ 255
 *
 */
//-------------------------------------------------------------------------
INT32 GFX_SwClipping(INT32 i4Value)
{
    if (i4Value > 255)
    {
        return 255;
    }
    else if (i4Value < 0)
    {
        return 0;
    }
    else
    {
        return i4Value;
    }
}

#endif  //#if defined(GFX_ENABLE_SW_MODE)

