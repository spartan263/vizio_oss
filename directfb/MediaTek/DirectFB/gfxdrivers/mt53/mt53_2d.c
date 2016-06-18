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
#include <config.h>

#include <sys/ioctl.h>

#include <assert.h>

#include <directfb.h>

#include <direct/debug.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <core/state.h>
#include <core/surface.h>

#include <gfx/convert.h>

#include <mt53/mt53.h>

#include <gfx_if.h>

#include "mt53_2d.h"
#include "mt53_gfxdriver.h"

#define TMPBUF_WIDTH  1920
#define TMPBUF_HEIGHT 1080

D_DEBUG_DOMAIN( MT53_2D, "MT53/2D", "MT53 2D Acceleration" );

/*
 * State validation flags.
 *
 * There's no prefix because of the macros below.
 */
enum
{
    DESTINATION  = 0x00000001,
    COLOR        = 0x00000002,

    SOURCE       = 0x00000010,
    BLTOPTS_KEY  = 0x00000020,

    ALL          = 0x00000033
};

/*
 * State handling macros.
 */

#define MT53_VALIDATE(flags)        do { mdev->v_flags |=  (flags); } while (0)
#define MT53_INVALIDATE(flags)      do { mdev->v_flags &= ~(flags); } while (0)

#define MT53_CHECK_VALIDATE(flag)   do {                                           \
                                         if (! (mdev->v_flags & flag))             \
                                              mt53_validate_##flag( mdrv, mdev, state );  \
                                    } while (0)

#define MT53_RGB16_TO_ARGB(pixel)   ( 0xFF000000 |                \
                                 ((((pixel) & 0xF800)==0xF800?0xFF00:((pixel) & 0xF800)) << 8) | \
                                 ((((pixel) & 0x07E0)==0x07E0?0x07F8:((pixel) & 0x07E0)) << 5) | \
                                 ((((pixel) & 0x001F)==0x001F?0x00FF:((pixel) & 0x001F)) << 3) )

#define MT53_ARGB4444_TO_ARGB(pixel)  ( ((((pixel) & 0xF000) << 16) | (((pixel) & 0xF000) <<  12)) | \
                    ((((pixel) & 0x0F00) <<  12) | (((pixel) & 0x0F00) <<  8)) | \
                                    ((((pixel) & 0x00F0) <<  8) | (((pixel) & 0x00F0) <<  4)) | \
                                    ((((pixel) & 0x000F) <<  4) | ((pixel) & 0x000F)) )

#define MT53_FLUSH_CMDQUE_IFTH()   do {                                             \
                                        if(true == GFX_CmdQueIsTH())                \
                                            mt53EmitCommands(mdrv, mdev);           \
                                   }while (0)

/**************************************************************************************************/

static bool mt53FillRectangle     ( void *drv, void *dev, DFBRectangle *rect );
static bool mt53FillRectangleBlend( void *drv, void *dev, DFBRectangle *rect );

static bool mt53Blit       ( void *drv, void *dev, DFBRectangle *srect, int dx, int dy );
static bool mt53BlitBlend  ( void *drv, void *dev, DFBRectangle *srect, int dx, int dy );

static bool mt53StretchBlit( void *drv, void *dev, DFBRectangle *srect, DFBRectangle *drect );


static bool mt53StretchBlitBlend( void *drv, void *dev, DFBRectangle *srect, DFBRectangle *drect );

#ifdef CC_B2R44K2K_SUPPORT
static bool mt53B2RStretchBlit( void *drv, void *dev, DFBRectangle *srect, DFBRectangle *drect );
#endif


#ifdef CC_TDTV_3D_OSD_ENABLE
static bool mt53SbsBlit  ( void *drv, void *dev, DFBRectangle *srect, int dx, int dy );  //3D side by side
#endif
#ifdef CC_DFB_SUPPORT_VDP_LAYER
static bool mt53Rotate  ( void *drv, void *dev, DFBRectangle *srect, int dx, int dy );
#endif
/**************************************************************************************************/

static unsigned char _blendSupportArray[DSBF_SRCALPHASAT + 1][DSBF_SRCALPHASAT + 1] =
{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, ROP_CLEAR+1, ROP_DST+1, 0, 0, ROP_DST_IN+1, ROP_DST_OUT+1, 0, 0, 0, 0, 0},
    {0, ROP_SRC+1, 0, 0, 0, 0, ROP_SRC_OVER+1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, ROP_SRC_OVER+1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, ROP_SRC_IN+1, 0, 0, 0, 0, ROP_SRC_ATOP+1, 0, 0, 0, 0, 0},
    {0, ROP_SRC_OUT+1, ROP_DST_OVER+1, 0, 0, ROP_DST_ATOP+1, ROP_XOR+1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};


static u32  _u4PremultSrc = 0;
static u32  _u4PremultDst = 0;
static u32  _u4PremultWDst = 0;

static unsigned int  _u4_render_flag = 0;
static CardState*  _pCardState = NULL;

#ifdef CC_TDTV_3D_OSD_ENABLE
static u32  _u4DfbRegionWidth = 1920;
static u32  _u4DfbRegionHeight = 1080;
#endif

#define MD_TIME_MAX          (100)
#define MD_TIME_CNT_MAX      (3)

static u64 u8_dfb_time[MD_TIME_MAX+1][MD_TIME_CNT_MAX];

static u32 u4_time_st = 0x0;

//bool  dfb_test = 0;
//bool  dfb_test2 = 0;

void md_time_init(void)
{
    memset(u8_dfb_time,0x0,sizeof(u8_dfb_time));
}

void md_time_start( u32 u4_id)
{
    if(!u4_time_st)
    {
        return;
    }
    
    u8_dfb_time[u4_id][1] = mdfb_clock_get_micros();
}

void md_time_end( u32 u4_id)
{
    if(!u4_time_st)
    {
        return;
    }
    u8_dfb_time[u4_id][0] += (mdfb_clock_get_micros() - u8_dfb_time[u4_id][1]);
}

void md_time_sum(void)
{
    u32 u4_lp =0x0;
    
    if(!u4_time_st)
    {
        return;
    }
    
    printf("[md_time_sum:\n");
    
    for(u4_lp =0x0;u4_lp < MD_TIME_MAX;u4_lp++)
    {
        u8_dfb_time[MD_TIME_MAX][0] += u8_dfb_time[u4_lp][0];
        printf("[%d,%lld ]\n",u4_lp,(u8_dfb_time[u4_lp][0]/1000));
    }

    printf("[md_time_sum[%lld]:\n",(u8_dfb_time[MD_TIME_MAX][0]/1000));
}

/*
 * Wait for the blitter to be idle.
 *
 * This function is called before memory that has been written to by the hardware is about to be
 * accessed by the CPU (software driver) or another hardware entity like video encoder (by Flip()).
 * It can also be called by applications explicitly, e.g. at the end of a benchmark loop to include
 * execution time of queued commands in the measurement.
 */
static DFBResult
GfxLock( void *drv)
{
    DFBResult       ret  = DFB_OK;
    return ret;
}

/*
 * Wait for the blitter to be idle.
 *
 * This function is called before memory that has been written to by the hardware is about to be
 * accessed by the CPU (software driver) or another hardware entity like video encoder (by Flip()).
 * It can also be called by applications explicitly, e.g. at the end of a benchmark loop to include
 * execution time of queued commands in the measurement.
 */
static DFBResult
GfxUnlock( void *drv)
{
    DFBResult       ret  = DFB_OK;
    return ret;
}

void mt_set_render_option(unsigned int u4_val)
{
    _u4_render_flag = u4_val;
}

unsigned int mt_get_render_option(void)
{
    return _u4_render_flag;
}

void mt53_gfx_set_mmu(GFX_MMU_T *pt_this)
{
    if(DFB_SYSTEMONLY)
    {
        pt_this->u4_init         =0x1;
        pt_this->u4_enable       =0x1;
        pt_this->u4_op_md        =0x0;
        pt_this->u4_src_rw_mmu   =0x1;
        pt_this->u4_dst_rw_mmu   =0x1;
        pt_this->u4_vgfx_ord     =0x0;
        pt_this->u4_vgfx_slva    =0x0;
        pt_this->u4_pgt          =0x0;
    }
    else
    {
        pt_this->u4_init         =0x1;
        pt_this->u4_enable       =0x0;
        pt_this->u4_op_md        =0x0;
        pt_this->u4_src_rw_mmu   =0x0;
        pt_this->u4_dst_rw_mmu   =0x0;
        pt_this->u4_vgfx_ord     =0x0;
        pt_this->u4_vgfx_slva    =0x0;
        pt_this->u4_pgt          =0x0;
    }
}

unsigned int mt_get_dbg_info( unsigned int u4_type)
{
    unsigned int data[E_DFB_DBG_INFO_MAX+1];
    struct mt53fb_get get;
    if(u4_type > E_DFB_DBG_INFO_MAX)
    {
        return 0x0;
    }
    get.get_type = MT53FB_GET_GFX_DATA;
    get.get_size = E_DFB_DBG_INFO_MAX*sizeof(unsigned int);
    get.get_data = data;
    if (ioctl( dfb_mt53->fd, FBIO_GET, &get ) < 0) 
    {
       D_PERROR( "DirectFB/MT53: mt_get_dbg_info failed!\n" );
       return DFB_IO;
    }
     return data[u4_type];
}

static inline unsigned int _checkAlphaComp(DFBSurfaceBlendFunction src_blend, DFBSurfaceBlendFunction dst_blend)
{
    return _blendSupportArray[src_blend][dst_blend];
}
/*
 * Called by mt53SetState() to ensure that the destination registers are properly set
 * for execution of rendering functions.
 */

static inline void
mt53_validate_DESTINATION( MT53DriverData *mdrv, MT53DeviceData *mdev,
                           CardState      *state )
{
    u32 color_mode = 0;


    /* Remember destination parameters for usage in rendering functions. */
    mdev->dst_addr   = state->dst.addr;
    mdev->dst_phys   = state->dst.phys;
    mdev->dst_pitch  = state->dst.pitch;
    mdev->dst_format = state->dst.buffer->format;
    mdev->dst_bpp    = DFB_BYTES_PER_PIXEL( mdev->dst_format );
    
#ifdef CC_B2R44K2K_SUPPORT
    if(state->destination && (state->destination->config.caps & DSCAPS_FROM_MPEG))
    {
        B2R44K2K_Buffer    *b2rbuf = NULL;
        int n;
        for(n = 0; n < B2R_BUFFER_NUM; n++)
        {
            if( state->dst.phys == dfb_mt53->m_B2RSysBuf[n].m_YAddr)
            {
                b2rbuf = &dfb_mt53->m_B2RSysBuf[n];
            }
        }

        if(b2rbuf)
        {
            mdev->dst_cbcr_offset = b2rbuf->m_CAddr- b2rbuf->m_YAddr;
        }
        else
        {
            mdev->dst_cbcr_offset = state->destination->config.size.h * state->dst.pitch;
        }
     }
#endif  

    switch (mdev->dst_format)
    {
    case DSPF_RGB16:
        color_mode = CM_RGB565_DIRECT16;
        break;

    case DSPF_ARGB4444:
        color_mode = CM_ARGB4444_DIRECT16;
        break;

    case DSPF_ARGB:
        color_mode = CM_ARGB8888_DIRECT32;
        break;

    case DSPF_LUT8:
        color_mode = CM_RGB_CLUT8;
        break;
        /* added by Allen 2009.2.12*/
    case DSPF_AYUV:
               color_mode = CM_AYCbCr8888_DIRECT32;             
               break;

          case DSPF_NV16:
               color_mode = CM_RGB_CLUT8;             
        break;

    default:
        D_BUG( "unexpected destination format 0x%08x", mdev->dst_format );
    }
    
    //if(state->dst.allocation->accessed[CSAID_CPU] & (CSAF_READ|CSAF_WRITE))
    //{
        //mt53FlushTextureCache(mdrv,mdev);
    //}

    GfxLock(mdrv);
    if(DFB_SYSTEMONLY)
    {
        GFX_SetDst( (UINT8*) mdev->dst_addr, color_mode, mdev->dst_pitch );
    }
    else
    {
        GFX_SetDst( (UINT8*) mdev->dst_phys, color_mode, mdev->dst_pitch );
    }
    GfxUnlock(mdrv);

    /* Set the flag. */
    MT53_VALIDATE( DESTINATION );
}

/*
 * Called by mt53SetState() to ensure that the color register is properly set
 * for execution of rendering functions.
 */
static inline void
mt53_validate_COLOR( MT53DriverData *mdrv, MT53DeviceData *mdev,
                     CardState      *state )
{
    switch (mdev->dst_format)
    {
    case DSPF_RGB16:
        mdev->color_pixel = PIXEL_RGB16(state->color.r,
                                        state->color.g,
                                        state->color.b );
        break;
    case DSPF_ARGB4444:
        mdev->color_pixel = PIXEL_ARGB4444( state->color.a,
                                            state->color.r,
                                            state->color.g,
                                            state->color.b );
        break;

    case DSPF_ARGB:
        mdev->color_pixel = PIXEL_ARGB( state->color.a,
                                        state->color.r,
                                        state->color.g,
                                        state->color.b );
        break;

    case DSPF_LUT8:
        mdev->color_pixel = state->color_index;
        break;

        /* added by Allen 2009.2.12*/
    case DSPF_AYUV:
        mdev->color_pixel = PIXEL_AYUV( state->color.a,
                                        state->color.r,
                                        state->color.g,
                                        state->color.b );
        break;
#ifdef CC_DFB_SUPPORT_VDP_LAYER
          case DSPF_NV16:
               mdev->color_pixel = 0;
               break;
#endif

    default:
        D_BUG( "unexpected format %s", dfb_pixelformat_name(mdev->dst_format) );
    }

    GfxLock(mdrv);
    GFX_SetColor( mdev->color_pixel );
    GfxUnlock(mdrv);

    /* Set the flag. */
    MT53_VALIDATE( COLOR );
}

/*
 * Called by mt53SetState() to ensure that the source registers are properly set
 * for execution of blitting functions.
 */
static inline void
mt53_validate_SOURCE( MT53DriverData *mdrv, MT53DeviceData *mdev,
                      CardState      *state )
{
    u32 color_mode = 0;

    /* Remember source parameters for usage in rendering functions. */
    mdev->src_addr   = state->src.addr;
    mdev->src_phys   = state->src.phys;
    mdev->src_pitch  = state->src.pitch;
    mdev->src_format = state->src.buffer->format;
    mdev->src_bpp    = DFB_BYTES_PER_PIXEL( mdev->src_format );

#ifdef CC_DFB_SUPPORT_VDP_LAYER
#ifdef CC_B2R44K2K_SUPPORT
    if(state->source && (state->source->config.caps & DSCAPS_FROM_MPEG))
    {
        B2R44K2K_Buffer  *b2rbuf = NULL;
        int n;
        for(n = 0; n < B2R_BUFFER_NUM; n++)
        {
            if( state->src.phys == dfb_mt53->m_B2RSysBuf[n].m_YAddr)
            {
                b2rbuf = &dfb_mt53->m_B2RSysBuf[n];
            }
        }

        if(b2rbuf)
        {
            mdev->src_cbcr_offset = b2rbuf->m_CAddr- b2rbuf->m_YAddr;
        }
        else
        {
            mdev->src_cbcr_offset = state->source->config.size.h * state->src.pitch;    
        }
    }
    else
#endif      
    {
        mdev->src_cbcr_offset = state->source->config.size.h * state->src.pitch;
        mdev->dst_cbcr_offset = state->destination->config.size.h * state->dst.pitch;
    }
#endif

    switch (mdev->src_format)
    {
    case DSPF_RGB16:
        color_mode = CM_RGB565_DIRECT16;
        break;

    case DSPF_ARGB4444:
        color_mode = CM_ARGB4444_DIRECT16;
        break;

    case DSPF_ARGB:
        color_mode = CM_ARGB8888_DIRECT32;
        break;
    case DSPF_LUT8:
        color_mode = CM_RGB_CLUT8;
        break;
        /* added by Allen 2009.2.12*/
    case DSPF_AYUV:
        color_mode = CM_AYCbCr8888_DIRECT32;
        break;

#ifdef CC_DFB_SUPPORT_VDP_LAYER  
          case DSPF_NV16:
               color_mode = CM_RGB_CLUT8;             
               break;
#endif

    default:
        D_BUG( "unexpected source format 0x%08x", mdev->src_format );
    }
    
    // if(state->src.allocation->accessed[CSAID_CPU] & (CSAF_READ|CSAF_WRITE))
    //{
        // mt53FlushTextureCache(mdrv,mdev);
    //}

    GfxLock(mdrv);
    if(DFB_SYSTEMONLY)
    {
        GFX_SetSrc( (UINT8*) mdev->src_addr, color_mode, mdev->src_pitch );
    }
    else
    {
        GFX_SetSrc( (UINT8*) mdev->src_phys, color_mode, mdev->src_pitch );
    }
    GfxUnlock(mdrv);

    /* Set the flag. */
    MT53_VALIDATE( SOURCE );
}

/*
 * Called by mt53SetState() to ensure that the blitting options and color ke
 * are properly set for execution of blitting functions.
 */
static inline void
mt53_validate_BLTOPTS_KEY( MT53DriverData *mdrv, MT53DeviceData *mdev,
                           CardState      *state )
{
    u32 bltopts = 0;
    u32 key_min = 0;
    u32 key_max = 0;

    if (mdev->src_format != mdev->dst_format)
        bltopts |= D_GFXFLAG_CFMT_ENA;

    if (state->blittingflags & DSBLIT_FLIP_HORIZONTAL)
        bltopts |= D_GFXFLAG_MIRROR;

    if (state->blittingflags & DSBLIT_FLIP_VERTICAL)
        bltopts |= D_GFXFLAG_FLIP;

    if (state->blittingflags & DSBLIT_SRC_COLORKEY)
    {
        DFBColor key;

        bltopts |= D_GFXFLAG_TRANSPARENT;

        dfb_pixel_to_color( mdev->src_format, state->src_colorkey, &key );

        switch (mdev->src_format)
        {
        case DSPF_ARGB4444:
            /*HF modify*/
#ifndef HF_MW_MODIFY
            key_min = MT53_ARGB4444_TO_ARGB(state->src_colorkey);
            key_max = MT53_ARGB4444_TO_ARGB(state->src_colorkey) | 0xFF000000;
#else
            key_min = state->src_colorkey;
            key_max = state->src_colorkey;
#endif
            /*CHUN modify end*/
            break;

        case DSPF_ARGB:
#ifndef HF_MW_MODIFY
            key_min = PIXEL_ARGB( 0x00, key.r, key.g, key.b );
            key_max = PIXEL_ARGB( 0xff, key.r, key.g, key.b );
#else
            key_min = state->src_colorkey;
            key_max = state->src_colorkey;
#endif
            break;

        case DSPF_RGB16:
            key_min = MT53_RGB16_TO_ARGB(state->src_colorkey);
            key_max = MT53_RGB16_TO_ARGB(state->src_colorkey);
            break;
        case DSPF_LUT8:
            key_min = state->src_colorkey;
            key_max = state->src_colorkey;
            //dfb_palette_search(state->source.palette, 0, 0, 0, 0)
            break;

        default:
            D_BUG( "unexpected source format 0x%08x", mdev->src_format );
        }
    }

    //printf("opt: 0x%8x, key_nim: 0x%8x, key_max: 0x%8x\n" , bltopts, key_min, key_max);

    GfxLock(mdrv);
    GFX_SetBltOpt( bltopts, key_min, key_max );
    GfxUnlock(mdrv);

    mdev->bltopts = bltopts;

    /* Set the flag. */
    MT53_VALIDATE( BLTOPTS_KEY );
}

/**************************************************************************************************/

/*
 * after the video memory has been written to by the CPU (e.g. modification
 * of a texture) make sure the accelerator won't use cached texture data
 */
void mt53FlushTextureCache( void *driver_data, void *device_data )
{
    MT53DeviceData *mdev = (MT53DeviceData*) device_data;
    
    if(mdev)
    {
        mdev->ex_data.cache.ui4_flush_fg = DFB_GFX_FLUSH_INVALID_DCACHE;
    }
    
    return;
}


/*
 * After the video memory has been written to by the accelerator
 * make sure the CPU won't read back cached data.
 */
void mt53FlushReadCache( void *driver_data, void *device_data )
{
    MT53DeviceData *mdev = (MT53DeviceData*) device_data;
    
    if(mdev)
    {
        mdev->ex_data.cache.ui4_flush_fg = 0x0;
    }
    
    // Flush Dcache
    if(ioctl( dfb_mt53->fd, FBIO_GFX_FLUSH_DCACHE) < 0)
    {
        D_PERROR( "MediaTek/Driver: mt53FlushTextureCache failed!\n" );
    }

    return;
}



/*
 * Wait for the blitter to be idle.
 *
 * This function is called before memory that has been written to by the hardware is about to be
 * accessed by the CPU (software driver) or another hardware entity like video encoder (by Flip()).
 * It can also be called by applications explicitly, e.g. at the end of a benchmark loop to include
 * execution time of queued commands in the measurement.
 */
DFBResult
mt53EngineSync( void *drv, void *dev )
{
    DFBResult       ret  = DFB_OK;
    
    if(ioctl( dfb_mt53->fd, FBIO_GFX_WAIT, NULL) < 0) 
    {
        D_PERROR( "DirectFB/MT53: FBIO_GFX_WAIT failed!\n" );
        ret  = DFB_FAILURE;
    }
    
    return ret;
}


/*
 * Reset the graphics engine.
 */
void
mt53EngineReset( void *drv, void *dev )
{
    GfxLock(drv);
    GFX_SetAlpha( 0xff );
    GfxUnlock(drv);
}

/*
 * Start processing of queued commands if required.
 *
 * This function is called before returning from the graphics core to the application.
 * Usually that's after each rendering function. The only functions causing multiple commands
 * to be queued with a single emition at the end are DrawString(), TileBlit(), BatchBlit(),
 * DrawLines() and possibly FillTriangle() which is emulated using multiple FillRectangle() calls.
 */
void mt53EmitCommands( void *drv, void *dev )
{
    MT53DriverData *mdrv = (MT53DriverData*) drv;
    MT53DeviceData *mdev = (MT53DeviceData*) dev;

    if(!GFX_GetVirtualCmdQueSize())
    {
        return ;
    }
    
    mt53_gfx_set_mmu((GFX_MMU_T *)&mdev->ex_data.mmu);
    
    if(ioctl(mdrv->fd, FBIO_GFX_LOCK,&mdev->ex_data) < 0)
    {
        D_PERROR( "MediaTek/Driver: FBIO_GFX_LOCK failed!\n" );
    }
    
    GFX_VirtualCmdQueAction();
    // GFX_ResetSrcDstParam();
          
    // Flush current cmd que
    if(ioctl( mdrv->fd, FBIO_GFX_FLUSH,&mdev->ex_data) < 0)
    {
        D_PERROR( "MediaTek/Driver: FBIO_GFX_FLUSH failed!\n" );
    }

    memset(&mdev->ex_data,0x0,sizeof(EX_DeviceData));
    
    return ;
}

static bool mt53_set_src_color_key(unsigned int u4_src_colorkey)
{
    GFX_Set_Src_ColorKey(u4_src_colorkey);
    
    return true;
}

/*
 * Check for acceleration of 'accel' using the given 'state'.
 */
void
mt53CheckState( void                *drv,
                void                *dev,
                CardState           *state,
                DFBAccelerationMask  accel )
{
    D_DEBUG_AT( MT53_2D, "mt53CheckState (state %p, accel 0x%08x) <- dest %p\n",
                state, accel, state->destination );


    /* Return if the desired function is not supported at all. */
    if (accel & ~(MT53_SUPPORTED_DRAWINGFUNCTIONS | MT53_SUPPORTED_BLITTINGFUNCTIONS))
    {
#ifdef CC_GFX_LOG
        printf("  Unsupport accel(0x%x)\n", accel);
#endif

        return;
    }

/*check if hardware support this operation*/
    switch (accel)
    {
        case DFXL_BLIT:
            {
                if (state->blittingflags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA))
                {
                    if(state->source->config.format != state->destination->config.format)
                    {
                        D_DEBUG_AT( MT53_2D, "mt53CheckState (accel 0x%08x [format different] not support )\n", accel);
                        // return;
                    }
                }
            }
            break;
        case DFXL_FILLRECTANGLE:
            {
                if(state->drawingflags & DSDRAW_BLEND && state->destination->config.caps & DSCAPS_PREMULTIPLIED)
                {
                    // return;
                }
            }
            break;
        default:
            break;
    }   

    /* Return if the destination format is not supported. */
    switch (state->destination->config.format)
    {
    case DSPF_RGB16:
    case DSPF_ARGB4444:
    case DSPF_ARGB:
    case DSPF_LUT8:
    case DSPF_AYUV: //added by Allen 2009.2.12
#ifdef CC_DFB_SUPPORT_VDP_LAYER          
          case DSPF_NV16:          
#endif              
        break;

    default:
#ifdef CC_GFX_LOG
        printf("  Unsupport dst format\n");
#endif
        return;
    }

    /* Check if drawing or blitting is requested. */
    if (DFB_DRAWING_FUNCTION( accel ))
    {
        /* Return if unsupported drawing flags are set. */
        if (state->drawingflags & ~MT53_SUPPORTED_DRAWINGFLAGS)
        {
#ifdef CC_GFX_LOG
            printf("  Unsupport drawingflags(0x%x)\n", state->drawingflags);
#endif

            return;
        }

    }
    else    //bitblt functions
    {
        /* Return if the source format is not supported. */
        switch (state->source->config.format)
        {
        case DSPF_RGB16:
        case DSPF_ARGB4444:
        case DSPF_ARGB:
        case DSPF_AYUV: //added by Allen 2009.2.12
            /* FIXME: Conversion only without flags. */
            //if (/*!state->blittingflags ||*/
            //    state->source->config.format == state->destination->config.format)
            break;
        case DSPF_LUT8:
            if (state->destination->config.format !=DSPF_LUT8)
            {
#ifdef CC_GFX_LOG
                printf("  Unsupport src/dst format(0x%x)\n", state->destination->config.format);
#endif
                return;
            }
            else
                break;
#ifdef CC_DFB_SUPPORT_VDP_LAYER                                   
               case DSPF_NV16:
                 /* Check for rotation */
                   if(state->destination->config.format != DSPF_NV16 || !(state->blittingflags & (DSBLIT_ROTATE90 | DSBLIT_ROTATE180 | DSBLIT_ROTATE270)))
                   {
                //printf("  Unsupport src/dst format(0x%x)\n", state->destination->config.format);                   
                //printf("  Unsupport rotate opt: (0x%x)\n", state->blittingflags);                                 
                #ifdef CC_GFX_LOG
                printf("  Unsupport src/dst format(0x%x)\n", state->destination->config.format);
                #endif                                                  
                    return;                       
                   }
                   else
                   {
                       break;
                   }
#endif                  
        default:
#ifdef CC_GFX_LOG
            printf("  Unsupport src format(0x%x)\n", state->source->config.format);
#endif
            return;
        }

        /* Return if unsupported blitting flags are set. */
        if (state->blittingflags & ~MT53_SUPPORTED_BLITTINGFLAGS)
        {
#ifdef CC_GFX_LOG
            printf("  Unsupport blittingflags:%x\n", state->blittingflags);
#endif

            return;
        }
    }


    /* Enable acceleration of the function. */
    state->accel |= accel;
}

/*
 * Make sure that the hardware is programmed for execution of 'accel' according to the 'state'.
 */
void
mt53SetState( void                *drv,
              void                *dev,
              GraphicsDeviceFuncs *funcs,
              CardState           *state,
              DFBAccelerationMask  accel )
{
    MT53DeviceData         *mdev     = (MT53DeviceData*) dev;
    MT53DriverData         *mdrv     = (MT53DriverData*) drv;

    StateModificationFlags  modified = state->mod_hw;

    D_DEBUG_AT( MT53_2D, "mt53SetState (state %p, accel 0x%08x) <- dest %p, modified 0x%08x\n",
                state, accel, state->destination, modified );

    _pCardState = state;
    /*
     * 1) Invalidate hardware states
     *
     * Each modification to the hw independent state invalidates one or more hardware states.
     */

    /* Simply invalidate all? */
    MT53_INVALIDATE( ALL );

    /*
     * 2) Validate hardware states
     *
     * Each function has its own set of states that need to be validated.
     */

    /* Always requiring valid destination... */
    MT53_CHECK_VALIDATE( DESTINATION );

    /* Remember color. */
    mdev->color = state->color;

    mdev->porterduff = _checkAlphaComp(state->src_blend, state->dst_blend);
#ifdef CC_DFB_SUPPORT_VDP_LAYER  
     if(state->blittingflags & DSBLIT_ROTATE90)
     {
         mdev->rotateopt = MT53FB_VDP_ROTATE_90;
     }
     else if(state->blittingflags & DSBLIT_ROTATE180)
     {
         mdev->rotateopt = MT53FB_VDP_ROTATE_180;
     }     
     else if(state->blittingflags & DSBLIT_ROTATE270)
     {
         mdev->rotateopt = MT53FB_VDP_ROTATE_270;
     }          
#endif

    /* Depending on the function... */
    switch (accel)
    {
    case DFXL_FILLRECTANGLE:
        /* support premultiply alpha functions */
#if 0
        if (state->drawingflags & DSDRAW_SRC_PREMULTIPLY)
        {
            u16 ca = state->color.a + 1;
            state->color.r = (state->color.r * ca) >> 8;
            state->color.g = (state->color.g * ca) >> 8;
            state->color.b = (state->color.b * ca) >> 8;
        }
        if (state->drawingflags & DSDRAW_DEMULTIPLY)
        {
            u16 ca = state->color.a + 1;
            state->color.r = (state->color.r << 8) / ca;
            state->color.g = (state->color.g << 8) / ca;
            state->color.b = (state->color.b << 8) / ca;
        }


        /* ...require valid drawing color. */
        MT53_CHECK_VALIDATE( COLOR );

        if (state->drawingflags & DSDRAW_BLEND)
            funcs->FillRectangle = mt53FillRectangleBlend;
        else
            funcs->FillRectangle = mt53FillRectangle;
#else

        if(state->drawingflags & DSDRAW_BLEND)
        {      
            if(state->drawingflags & DSDRAW_SRC_PREMULTIPLY)
            {
                _u4PremultSrc = 0;
            }
            else
            {
                _u4PremultSrc = 1;
            }
        
            if (state->destination->config.caps & DSCAPS_PREMULTIPLIED)
            {
                _u4PremultDst = 1;
                
                _u4PremultWDst = 1;
            }
            else
            {
                if(state->drawingflags & DSDRAW_DST_PREMULTIPLY)
                {
                     _u4PremultDst = 0;
                }
                else
                {
                     _u4PremultDst = 1;
                }

                if(state->drawingflags & DSDRAW_DEMULTIPLY)
                {
                    _u4PremultWDst = 0;
                }
                else
                {
                    _u4PremultWDst = 1;
                }
            }
            
            funcs->FillRectangle = mt53FillRectangleBlend;
        }
        else
        {
            if (state->drawingflags & DSDRAW_SRC_PREMULTIPLY)
            {
                u16 ca = state->color.a + 1;
                state->color.r = (state->color.r * ca) >> 8;
                state->color.g = (state->color.g * ca) >> 8;
                state->color.b = (state->color.b * ca) >> 8;
            }
            
            if (state->drawingflags & DSDRAW_DEMULTIPLY || state->destination->config.caps & DSCAPS_PREMULTIPLIED)
            {
                u16 ca = state->color.a + 1;
                state->color.r = (state->color.r << 8) / ca;
                state->color.g = (state->color.g << 8) / ca;
                state->color.b = (state->color.b << 8) / ca;
            }
            
            funcs->FillRectangle = mt53FillRectangle;
        }

        MT53_CHECK_VALIDATE( COLOR );
#endif
        if (mdev->bltopts)
        {
            mdev->bltopts = 0;

            GfxLock(mdrv);
            GFX_SetBltOpt( 0, 0, 0 );
            GfxUnlock(mdrv);

            MT53_INVALIDATE( BLTOPTS_KEY );
        }

        /*
         * 3) Tell which functions can be called without further validation, i.e. SetState()
         *
         * When the hw independent state is changed, this collection is reset.
         */
        state->set = MT53_SUPPORTED_DRAWINGFUNCTIONS;
        break;

    case DFXL_BLIT:
    case DFXL_STRETCHBLIT:
        /* ...require valid source and blitting options. */
        MT53_CHECK_VALIDATE( SOURCE );
        MT53_CHECK_VALIDATE( BLTOPTS_KEY );

        if (state->source->config.caps & DSCAPS_PREMULTIPLIED)
        {
            _u4PremultSrc = 1;
        }
        else
        {              
            if(state->blittingflags & DSBLIT_SRC_PREMULTIPLY)
            {
                 _u4PremultSrc = 0;
            }
            else
            {
                 _u4PremultSrc = 1;
            }
        }

        if (state->destination->config.caps & DSCAPS_PREMULTIPLIED)
        {
            _u4PremultDst = 1;
            _u4PremultWDst = 1;
        }
        else
        {
            if(state->blittingflags & DSBLIT_DST_PREMULTIPLY)
            {
                 _u4PremultDst = 0;
            }
            else
            {
                 _u4PremultDst = 1;
            }
            
            if(state->blittingflags & DSBLIT_DEMULTIPLY)
            {
                 _u4PremultWDst = 0;
            }
            else
            {
                 _u4PremultWDst = 1;
            }           
        }
        /* Use alpha blending? */
        if (state->blittingflags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA))
        {
            if (!(state->blittingflags & DSBLIT_BLEND_COLORALPHA))
                mdev->color.a = 0xff;

#ifdef CC_TDTV_3D_OSD_ENABLE
            if (state->sbsflag)
            {
                _u4DfbRegionWidth = state->clip.x2 + 1;
                _u4DfbRegionHeight = state->clip.y2 + 1;
                funcs->Blit = mt53SbsBlit;
            }
            else
            {
                funcs->Blit = mt53BlitBlend;
            }
#else
            funcs->Blit = mt53BlitBlend;
#endif

            funcs->StretchBlit = mt53StretchBlitBlend;

        }
#ifdef CC_DFB_SUPPORT_VDP_LAYER                   
               else if(state->blittingflags & (DSBLIT_ROTATE90 | DSBLIT_ROTATE180 | DSBLIT_ROTATE270))
               {
                   funcs->Blit = mt53Rotate;
                   funcs->StretchBlit = mt53StretchBlit;                 
               }               
#endif               
        else
        {
#ifdef CC_TDTV_3D_OSD_ENABLE
            if (state->sbsflag)
            {
                _u4DfbRegionWidth = state->clip.x2 + 1;
                _u4DfbRegionHeight = state->clip.y2 + 1;
                funcs->Blit = mt53SbsBlit;
            }
            else
            {
                funcs->Blit = mt53Blit;
            }
#else
            funcs->Blit = mt53Blit;
#endif
            funcs->StretchBlit = mt53StretchBlit;
        }

        if(state->blittingflags&DSBLIT_SRC_COLORKEY)
        {
            // mt53_set_src_color_key(state->src_colorkey);
            mt53_validate_BLTOPTS_KEY(mdrv, mdev, state);
        }
        
        /*
         * 3) Tell which functions can be called without further validation, i.e. SetState()
         *
         * When the hw independent state is changed, this collection is reset.
         */
        state->set = MT53_SUPPORTED_BLITTINGFUNCTIONS;
        break;

    default:
        D_BUG( "unexpected drawing/blitting function" );
        break;
    }

    /*
     * 4) Clear modification flags
     *
     * All flags have been evaluated in 1) and remembered for further validation.
     * If the hw independent state is not modified, this function won't get called
     * for subsequent rendering functions, unless they aren't defined by 3).
     */
   // state->mod_hw = 0;
}

/*
 * Render a filled rectangle using the current hardware state.
 */
static bool
mt53FillRectangle( void *drv, void *dev, DFBRectangle *rect )
{
    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;
    D_DEBUG_AT( MT53_2D, "%s( %d,%d-%dx%d )\n", __FUNCTION__, DFB_RECTANGLE_VALS( rect ) );

    GfxLock(drv);
    MT53_FLUSH_CMDQUE_IFTH();
    GFX_Fill( DFB_RECTANGLE_VALS( rect ) );
    GfxUnlock(drv);

    return true;
}

static bool
mt53FillRectangleBlend( void *drv, void *dev, DFBRectangle *rect )
{
    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;
    u32 porterduff = mdev->porterduff;

    D_DEBUG_AT( MT53_2D, "%s( %d,%d-%dx%d )\n", __FUNCTION__, DFB_RECTANGLE_VALS( rect ) );

    if (!porterduff)
    {
#ifdef CC_GFX_LOG
        printf("  Unsupport PD_d\n");
#endif
        return false;
    }

    GfxLock(drv);
    MT53_FLUSH_CMDQUE_IFTH();
    GFX_SetPremult(_u4PremultSrc, _u4PremultDst, _u4PremultWDst, !_u4_render_flag);
    GFX_ComposeLoop( 0, 0, DFB_RECTANGLE_VALS( rect ),
                     0xff, porterduff - 1, 1 );
    GfxUnlock(drv);

    return true;
}

#ifdef CC_TDTV_3D_OSD_ENABLE
static bool
mt53SbsBlit( void *drv, void *dev, DFBRectangle *srect, int dx, int dy )
{

    D_DEBUG_AT( MT53_2D, "%s( %d,%d-%dx%d -> %d, %d )\n", __FUNCTION__,
                DFB_RECTANGLE_VALS( srect ), dx, dy );
    u8 u1IsSBSOutput, u1IsTbbOutput, u1OsdShift ;
    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;

    if (ioctl(mdrv->fd, FBIO_GET_SBSOUTPUT, &u1IsSBSOutput  ) < 0)
    {
        D_PERROR( "MediaTek/Driver: FBIO_GET_SBSOUTPUT failed!\n" );
        return false;
    }
    else  if (ioctl(mdrv->fd, FBIO_GET_TBBOUTPUT, &u1IsTbbOutput  ) < 0)
    {
        D_PERROR( "MediaTek/Driver: FBIO_GET_SBSOUTPUT failed!\n" );
        return false;
    }
    if (ioctl(mdrv->fd, FBIO_GET_3DOSD_SHIFT, &u1OsdShift  ) < 0)
    {
        D_PERROR( "MediaTek/Driver: FBIO_GET_3DOSD_SHIFT failed!\n" );
        return false;
    }
    if (u1OsdShift > 0x40) u1OsdShift = 0;

    if (u1IsSBSOutput)
    {
        DFBRectangle temrect = { srect->x / 2, srect->y, srect->w / 2, srect->h };
        DFBRectangle rightrect = { srect->x / 2 + _u4DfbRegionWidth / 2 + u1OsdShift, srect->y, srect->w / 2, srect->h };

        // GFX_BitBlt( srect->x, srect->y, dx, dy, srect->w, srect->h );

        mt53StretchBlit( drv, dev, srect, &temrect );
        mt53StretchBlit( drv, dev, srect, &rightrect );
    }
    else if (u1IsTbbOutput)
    {
        DFBRectangle temrect = { srect->x, srect->y / 2, srect->w, srect->h / 2 };
        DFBRectangle rightrect = { srect->x + u1OsdShift, srect->y / 2 + _u4DfbRegionHeight / 2, srect->w, srect->h / 2 };

        // GFX_BitBlt( srect->x, srect->y, dx, dy, srect->w, srect->h );

        mt53StretchBlit( drv, dev, srect, &temrect );
        mt53StretchBlit( drv, dev, srect, &rightrect );

    }
    else
    {
        mt53Blit(drv, dev, srect, dx, dy);
    }
    return true;
}
#endif
/*
 * Copy a rectangle using the current hardware state.
 */
static bool
mt53Blit( void *drv, void *dev, DFBRectangle *srect, int dx, int dy )
{
    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;
    D_DEBUG_AT( MT53_2D, "%s( %d,%d-%dx%d -> %d, %d )\n", __FUNCTION__,
                DFB_RECTANGLE_VALS( srect ), dx, dy );

    GfxLock(drv);

    MT53_FLUSH_CMDQUE_IFTH();
    if (_u4PremultSrc != _u4PremultDst)
    {
        GFX_SetPremult(_u4PremultSrc, _u4PremultDst, _u4PremultWDst, !_u4_render_flag);
        GFX_ComposeLoop( srect->x, srect->y, dx, dy, srect->w, srect->h,
                         0xff, 4, 0);
    }
    else
    {
        GFX_BitBlt( srect->x, srect->y, dx, dy, srect->w, srect->h );
    }

    GfxUnlock(drv);

    return true;
}

static void mt53_stretch_blit_set_mmu(GFX_STRETCH_BLIT_T *pt_stretch_blit)
{
    pt_stretch_blit->mmu.u4_init         =0x1;
    
    if(DFB_SYSTEMONLY)
    {
        pt_stretch_blit->mmu.u4_enable          =0x1;
        pt_stretch_blit->mmu.u4_src_rw_mmu      =0x1;
        pt_stretch_blit->mmu.u4_dst_rw_mmu      =0x1;
    }
    else
    {
        pt_stretch_blit->mmu.u4_enable          =0x0;
        pt_stretch_blit->mmu.u4_src_rw_mmu      =0x0;
        pt_stretch_blit->mmu.u4_dst_rw_mmu      =0x0;
    }
    
    pt_stretch_blit->mmu.u4_op_md        =0x0;
    pt_stretch_blit->mmu.u4_vgfx_ord     =0x0;
    pt_stretch_blit->mmu.u4_vgfx_slva    =0x0;
    pt_stretch_blit->mmu.u4_pgt          =0x0;

    return;
}

static void mt53_get_quard_max_min_h( VGFX_QUADRILATERAL_SET_T* pt_quard,UINT32 *pu4_max,UINT32 *pu4_min)
{
    UINT32 u4_max=0x0;
    UINT32 u4_min=0x0;

    if(NULL == pt_quard)
    {
        return;
    }

    u4_max = pt_quard->y1;
    u4_min = pt_quard->y1;

    if( pt_quard->y2 > u4_max)
    {
        u4_max = pt_quard->y2;
    }

    if( pt_quard->y2 < u4_min)
    {
        u4_min = pt_quard->y2;
    }

    if( pt_quard->y3 > u4_max)
    {
        u4_max = pt_quard->y3;
    }

    if( pt_quard->y3 < u4_min)
    {
        u4_min = pt_quard->y3;
    }

    if( pt_quard->y4 > u4_max)
    {
        u4_max = pt_quard->y4;
    }

    if( pt_quard->y4 < u4_min)
    {
        u4_min = pt_quard->y4;
    }

    *pu4_max = u4_max;
    *pu4_min = u4_min;

    return;
}

static void mt53_vgfx_scaler_set_mmu(VGFX_SCALER_PARAM_SET_T *pt_vgfx_sc)
{
    UINT32 u4_max=0x0;
    UINT32 u4_min=0x0;
    
    if(NULL == pt_vgfx_sc)
    {
        return;
    }   
    
    if(DFB_SYSTEMONLY)
    {
        pt_vgfx_sc->mmu.u4_init         =0x1;
        pt_vgfx_sc->mmu.u4_enable       =0x1;
        pt_vgfx_sc->mmu.u4_op_md        =0x0;
        pt_vgfx_sc->mmu.u4_src_rw_mmu   =0x1;
        pt_vgfx_sc->mmu.u4_dst_rw_mmu   =0x1;
        pt_vgfx_sc->mmu.u4_vgfx_ord     =0x1;
        pt_vgfx_sc->mmu.u4_pgt          =0x0;
        mt53_get_quard_max_min_h(&pt_vgfx_sc->_SrcQuard,&u4_max,&u4_min);
        pt_vgfx_sc->mmu.u4_vgfx_slva = (pt_vgfx_sc->_IMAGType.u4BaseAddr +
            (u4_max*pt_vgfx_sc->_IMAGType.u4Pitch));

        UNUSED(u4_max);
        UNUSED(u4_min);
    }
    else
    {
        pt_vgfx_sc->mmu.u4_init         =0x1;
        pt_vgfx_sc->mmu.u4_enable       =0x0;
        pt_vgfx_sc->mmu.u4_op_md        =0x0;
        pt_vgfx_sc->mmu.u4_src_rw_mmu   =0x0;
        pt_vgfx_sc->mmu.u4_dst_rw_mmu   =0x0;
        pt_vgfx_sc->mmu.u4_vgfx_ord     =0x0;
        pt_vgfx_sc->mmu.u4_vgfx_slva    =0x0;
        pt_vgfx_sc->mmu.u4_pgt          =0x0;
    }

    return;
}

static bool
mt53VgfxScalerIOCrtl( void *drv,void *dev, void *vgfx)
{
//  MT53DriverData *mdrv = (MT53DriverData*) drv;

    VGFX_SCALER_PARAM_SET_T * _SetVgfxScaler;
    BOOL _bMatrix = FALSE;
    VGFX_MATRIX_T *prMartrix;
    VGFX_RECT_SET_T _TextRect;
    
    float f4Xs;
    float f4Ys;
    float f4Hs;
    float f4Ws;
    float f4Xd;
    float f4Yd;
    float f4Hd;
    float f4Wd;
    float f4Temp;
    
    UINT32 u4MatrixA;
    UINT32 u4MatrixB;
    UINT32 u4MatrixC;
    UINT32 u4MatrixD;
    UINT32 u4MatrixE;
    UINT32 u4MatrixF;
    UINT32 u4MatrixG;
    UINT32 u4MatrixH;
    UINT32 u4MatrixI;

    float fSrcX[4] ,fSrcY[4] ,fDstX[4] ,fDstY[4] ,fIMtrix[3][3];

    _SetVgfxScaler = (VGFX_SCALER_PARAM_SET_T *)vgfx;
    
    if (NULL == _SetVgfxScaler)
    {
        return false;
    }
    

    switch (_SetVgfxScaler->_IMAGType.u4ColrMode)
    {
        case DSPF_RGB16:
        break;

        case DSPF_ARGB4444:
        break;

        case DSPF_ARGB:
        case DSPF_AYUV:
        break;

        default:
        D_BUG( "unexpected src format 0x%08x", _SetVgfxScaler->_IMAGType.u4ColrMode );
        return false;
    }

    switch (_SetVgfxScaler->_WBType.u4ColrMode)
    {
        case DSPF_RGB16:
        break;

        case DSPF_ARGB4444:
        break;

        case DSPF_ARGB:
        case DSPF_AYUV:
        break;

        default:
        D_BUG( "unexpected dst format 0x%08x",_SetVgfxScaler->_WBType.u4ColrMode );
        return false;
    }
            
    if( MTVGfx_JugRect(_SetVgfxScaler->_SrcQuard) )
    {
        return;
    }
    else
    {
        _TextRect.u4StartX = _SetVgfxScaler->_SrcQuard.x1;
        _TextRect.u4StartY = _SetVgfxScaler->_SrcQuard.y1;
        _TextRect.u4Width = _SetVgfxScaler->_SrcQuard.x3 - _SetVgfxScaler->_SrcQuard.x1;
        _TextRect.u4Height = _SetVgfxScaler->_SrcQuard.y3 - _SetVgfxScaler->_SrcQuard.y1;
    }
    _bMatrix = MTVGfx_JugRect(_SetVgfxScaler->_DstQuard);
    if( _bMatrix )
    {
        f4Xs = (float)(_TextRect.u4StartX );
        f4Ys = (float)(_TextRect.u4StartY);
        f4Hs = (float)(_TextRect.u4Width);
        f4Ws = (float)(_TextRect.u4Height);
        f4Xd = (float)(_SetVgfxScaler->_BBType.u4StartX);
        f4Yd = (float)(_SetVgfxScaler->_BBType.u4StartY);
        f4Hd = (float)(_SetVgfxScaler->_BBType.u4Height);
        f4Wd = (float)(_SetVgfxScaler->_BBType.u4Width);
    
        f4Temp = (f4Ws-1)/(f4Wd-1);
        u4MatrixA = fnum_converter(f4Temp) >> 9;
        u4MatrixB = 0;
        f4Temp = f4Xs - f4Xd*(f4Ws-1)/(f4Wd-1);
        u4MatrixC = fnum_converter(f4Temp) >> 9;
        u4MatrixD= 0;
        f4Temp = (f4Hs-1)/(f4Hd-1);
        u4MatrixE =fnum_converter(f4Temp) >> 9;
        f4Temp = f4Ys-f4Yd*(f4Hs-1)/(f4Hd-1);
        u4MatrixF =fnum_converter(f4Temp) >> 9;
        u4MatrixG = 0;
        u4MatrixH = 0;
        f4Temp = 1;
        u4MatrixI = fnum_converter(f4Temp) >> 9; 
    
        _SetVgfxScaler->_SetMartix.u4_a = u4MatrixA;
        _SetVgfxScaler->_SetMartix.u4_b = u4MatrixB;
        _SetVgfxScaler->_SetMartix.u4_c = u4MatrixC;
        _SetVgfxScaler->_SetMartix.u4_d = u4MatrixD;
        _SetVgfxScaler->_SetMartix.u4_e = u4MatrixE;
        _SetVgfxScaler->_SetMartix.u4_f = u4MatrixF;
        _SetVgfxScaler->_SetMartix.u4_g = u4MatrixG;
        _SetVgfxScaler->_SetMartix.u4_h = u4MatrixH;
        _SetVgfxScaler->_SetMartix.u4_i = u4MatrixI;
    }
    else
    {
        fSrcX[0] = (float)(_SetVgfxScaler->_SrcQuard.x1);
        fSrcX[1] = (float)(_SetVgfxScaler->_SrcQuard.x3);
        fSrcX[2] = (float)(_SetVgfxScaler->_SrcQuard.x3);
        fSrcX[3] = (float)(_SetVgfxScaler->_SrcQuard.x1);
        fSrcY[0] = (float)(_SetVgfxScaler->_SrcQuard.y1);
        fSrcY[1] = (float)(_SetVgfxScaler->_SrcQuard.y1);
        fSrcY[2] = (float)(_SetVgfxScaler->_SrcQuard.y3);
        fSrcY[3] = (float)(_SetVgfxScaler->_SrcQuard.y3);
        fDstX[0] = (float)(_SetVgfxScaler->_DstQuard.x1);
        fDstX[1] = (float)(_SetVgfxScaler->_DstQuard.x2);
        fDstX[2] = (float)(_SetVgfxScaler->_DstQuard.x3);
        fDstX[3] = (float)(_SetVgfxScaler->_DstQuard.x4);
        fDstY[0] = (float)(_SetVgfxScaler->_DstQuard.y1);
        fDstY[1] = (float)(_SetVgfxScaler->_DstQuard.y2);
        fDstY[2] = (float)(_SetVgfxScaler->_DstQuard.y3);
        fDstY[3] = (float)(_SetVgfxScaler->_DstQuard.y4);

        derive_inverse_transform(fSrcX, fSrcY, fDstX, fDstY, fIMtrix);

        _SetVgfxScaler->_SetMartix.u4_a = fnum_converter(fIMtrix[0][0]) >> 9;
        _SetVgfxScaler->_SetMartix.u4_b = fnum_converter(fIMtrix[0][1]) >> 9;
        _SetVgfxScaler->_SetMartix.u4_c = fnum_converter(fIMtrix[0][2]) >> 9;
        _SetVgfxScaler->_SetMartix.u4_d = fnum_converter(fIMtrix[1][0]) >> 9;
        _SetVgfxScaler->_SetMartix.u4_e = fnum_converter(fIMtrix[1][1]) >> 9;
        _SetVgfxScaler->_SetMartix.u4_f = fnum_converter(fIMtrix[1][2]) >> 9;
        _SetVgfxScaler->_SetMartix.u4_g = fnum_converter(fIMtrix[2][0]) >> 9;
        _SetVgfxScaler->_SetMartix.u4_h = fnum_converter(fIMtrix[2][1]) >> 9;
        _SetVgfxScaler->_SetMartix.u4_i = fnum_converter(fIMtrix[2][2]) >> 9;

    }
    
    mt53_vgfx_scaler_set_mmu(_SetVgfxScaler);
    
    if (ioctl(dfb_mt53->fd, FBIO_VGFX_SCALER, &_SetVgfxScaler ) < 0)
    {
        return false;
    }

    //GfxUnlock(drv);
    return true;


}

static bool
mt53VgfxScalerCMDQ( void *drv,void *dev, void *vgfx)
{
    VGFX_SCALER_PARAM_SET_T * _SetVgfxScaler;
    BOOL _bMatrix = FALSE;
    VGFX_MATRIX_T *prMartrix;
    VGFX_RECT_SET_T _TextRect;
    
    float f4Xs;
    float f4Ys;
    float f4Hs;
    float f4Ws;
    float f4Xd;
    float f4Yd;
    float f4Hd;
    float f4Wd;
    float f4Temp;
    
    UINT32 u4MatrixA;
    UINT32 u4MatrixB;
    UINT32 u4MatrixC;
    UINT32 u4MatrixD;
    UINT32 u4MatrixE;
    UINT32 u4MatrixF;
    UINT32 u4MatrixG;
    UINT32 u4MatrixH;
    UINT32 u4MatrixI;

    _SetVgfxScaler = (VGFX_SCALER_PARAM_SET_T *)vgfx;
    
    if (NULL == _SetVgfxScaler)
    {
        return false;
    }
    

    switch (_SetVgfxScaler->_IMAGType.u4ColrMode)
    {
        case DSPF_RGB16:
        break;

        case DSPF_ARGB4444:
        break;

        case DSPF_ARGB:
        case DSPF_AYUV:
        break;

        default:
        D_BUG( "unexpected src format 0x%08x", _SetVgfxScaler->_IMAGType.u4ColrMode );
        return false;
    }

    switch (_SetVgfxScaler->_WBType.u4ColrMode)
    {
        case DSPF_RGB16:
        break;

        case DSPF_ARGB4444:
        break;

        case DSPF_ARGB:
        case DSPF_AYUV:
        break;

        default:
        D_BUG( "unexpected dst format 0x%08x",_SetVgfxScaler->_WBType.u4ColrMode );
        return false;
    }
            
    if( MTVGfx_JugRect(_SetVgfxScaler->_SrcQuard) )
    {
        return;
    }
    else
    {
        _TextRect.u4StartX = _SetVgfxScaler->_SrcQuard.x1;
        _TextRect.u4StartY = _SetVgfxScaler->_SrcQuard.y1;
        _TextRect.u4Width = _SetVgfxScaler->_SrcQuard.x3 - _SetVgfxScaler->_SrcQuard.x1;
        _TextRect.u4Height = _SetVgfxScaler->_SrcQuard.y3 - _SetVgfxScaler->_SrcQuard.y1;
    }
    _bMatrix = MTVGfx_JugRect(_SetVgfxScaler->_DstQuard);
    if( _bMatrix )
    {
        f4Xs = (float)(_TextRect.u4StartX );
        f4Ys = (float)(_TextRect.u4StartY);
        f4Hs = (float)(_TextRect.u4Width);
        f4Ws = (float)(_TextRect.u4Height);
        f4Xd = (float)(_SetVgfxScaler->_BBType.u4StartX);
        f4Yd = (float)(_SetVgfxScaler->_BBType.u4StartY);
        f4Hd = (float)(_SetVgfxScaler->_BBType.u4Height);
        f4Wd = (float)(_SetVgfxScaler->_BBType.u4Width);
    
        f4Temp = (f4Ws-1)/(f4Wd-1);
        u4MatrixA = fnum_converter(f4Temp) >> 9;
        u4MatrixB = 0;
        f4Temp = f4Xs - f4Xd*(f4Ws-1)/(f4Wd-1);
        u4MatrixC = fnum_converter(f4Temp) >> 9;
        u4MatrixD= 0;
        f4Temp = (f4Hs-1)/(f4Hd-1);
        u4MatrixE =fnum_converter(f4Temp) >> 9;
        f4Temp = f4Ys-f4Yd*(f4Hs-1)/(f4Hd-1);
        u4MatrixF =fnum_converter(f4Temp) >> 9;
        u4MatrixG = 0;
        u4MatrixH = 0;
        f4Temp = 1;
        u4MatrixI = fnum_converter(f4Temp) >> 9; 
    
        _SetVgfxScaler->_SetMartix.u4_a = u4MatrixA;
        _SetVgfxScaler->_SetMartix.u4_b = u4MatrixB;
        _SetVgfxScaler->_SetMartix.u4_c = u4MatrixC;
        _SetVgfxScaler->_SetMartix.u4_d = u4MatrixD;
        _SetVgfxScaler->_SetMartix.u4_e = u4MatrixE;
        _SetVgfxScaler->_SetMartix.u4_f = u4MatrixF;
        _SetVgfxScaler->_SetMartix.u4_g = u4MatrixG;
        _SetVgfxScaler->_SetMartix.u4_h = u4MatrixH;
        _SetVgfxScaler->_SetMartix.u4_i = u4MatrixI;
    }
    else
    {
        return; //..........................................................................
    }
    //GfxLock(drv);   
    //_VGfx_SetVGfxScaler(_SetVgfxScaler);
    //GfxUnlock(drv);
    return true;
}

static bool
mt53BlitBlend( void *drv, void *dev, DFBRectangle *srect, int dx, int dy )
{
    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;

    u32 porterduff = mdev->porterduff;
    u32 u4GlobalAlpha = 0;

    D_DEBUG_AT( MT53_2D, "%s( %d,%d-%dx%d -> %d, %d )\n", __FUNCTION__,
                DFB_RECTANGLE_VALS( srect ), dx, dy );

    if (!porterduff)
    {
#ifdef CC_GFX_LOG
        printf("  Unsupport PD\n");
#endif
        return false;
    }

#if 0
    if (porterduff == (MT53FB_ROP_SRC+1))
    {
        mt53Blit(drv, dev, srect, dx, dy);
        return true;
    }

    if (porterduff == (MT53FB_ROP_CLEAR+1))
    {
        mt53FillRectangle(drv,dev,srect);
        return true;
    }
#endif

    if ((mdev->src_format == DSPF_ARGB4444)||
        (mdev->src_format == DSPF_ARGB)||
        (mdev->src_format == DSPF_AYUV))
    {
        u4GlobalAlpha = mdev->color.a;
    }
    else
    {
        assert(0);
    }

	if((mdev->src_format != mdev->dst_format )&&(srect->w <= 8))
    {
 	   return false;
    }

    GfxLock(drv);

    MT53_FLUSH_CMDQUE_IFTH();
    GFX_SetPremult(_u4PremultSrc, _u4PremultDst, _u4PremultWDst, !_u4_render_flag);

    GFX_ComposeLoop( srect->x, srect->y, dx, dy, srect->w, srect->h,
                     u4GlobalAlpha, porterduff - 1, 0 );
    GfxUnlock(drv);

    return true;
}

/*
 * Stretch a rectangle using the current hardware state.
 */
static bool
mt53StretchBlit( void *drv, void *dev, DFBRectangle *srect, DFBRectangle *drect )
{
    D_DEBUG_AT( MT53_2D, "%s( %d,%d-%dx%d -> %d, %d )\n", __FUNCTION__,
                DFB_RECTANGLE_VALS( srect ), 0/*dx*/, 0/*dy*/ );

    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;
    GFX_STRETCH_BLIT_T rStretchBlit;
    u32 SrcCm = 0;
    u32 DstCm = 0;
    DFBRegion *prClip = &_pCardState->clip;


    if ((srect->w == 0) || (srect->h == 0)||(drect->w == 0)||(drect->h == 0))
    {
        //printf("stretch zero\n");
        return true;
    }

    switch (mdev->src_format)
    {
    case DSPF_RGB16:
        SrcCm = CM_RGB565_DIRECT16;
        break;

    case DSPF_ARGB4444:
        SrcCm = CM_ARGB4444_DIRECT16;
        break;

    case DSPF_ARGB:
    case DSPF_AYUV:
        SrcCm = CM_ARGB8888_DIRECT32;
        break;

    default:
        D_BUG( "unexpected src format 0x%08x", mdev->src_format );
        return false;
    }

    switch (mdev->dst_format)
    {
    case DSPF_RGB16:
        DstCm = CM_RGB565_DIRECT16;
        break;

    case DSPF_ARGB4444:
        DstCm = CM_ARGB4444_DIRECT16;
        break;

    case DSPF_ARGB:
    case DSPF_AYUV:
        DstCm = CM_ARGB8888_DIRECT32;
        break;

    default:
        D_BUG( "unexpected dst format 0x%08x", mdev->dst_format );
        return false;
    }

#if 0
    fprintf(stderr, "222Clip(%d,%d,%d,%d)", prClip->x1, prClip->y1, prClip->x2, prClip->y2);
    fprintf(stderr, "(%d,%d,%d,%d)", srect->x, srect->y, srect->w, srect->h);
    fprintf(stderr, "(%d,%d,%d,%d)\n", drect->x, drect->y, drect->w, drect->h);
#endif

    if(DFB_SYSTEMONLY)
    {
        rStretchBlit.u4Src = (unsigned int)mdev->src_addr;
        rStretchBlit.u4Dst = (unsigned int)mdev->dst_addr;
    }
    else
    {
        rStretchBlit.u4Src = mdev->src_phys;
        rStretchBlit.u4Dst = mdev->dst_phys;
    }
    
    rStretchBlit.eSrcCm = SrcCm;
    rStretchBlit.eDstCm = DstCm;
    rStretchBlit.u4SrcPitch = mdev->src_pitch;
    rStretchBlit.u4DstPitch = mdev->dst_pitch;
    rStretchBlit.u4SrcX = srect->x;
    rStretchBlit.u4SrcY =  srect->y;
    rStretchBlit.u4SrcWidth = srect->w;
    rStretchBlit.u4SrcHeight = srect->h;

    rStretchBlit.u4DstWidth = drect->w;
    rStretchBlit.u4DstHeight = drect->h;
    rStretchBlit.u4DstX = drect->x;
    rStretchBlit.u4DstY = drect->y;

    rStretchBlit.u4ClipWidth = drect->w;
    rStretchBlit.u4ClipHeight = drect->h;

    if ((prClip->x1 >= drect->x + drect->w) ||
            (prClip->x2 < drect->x) ||
            (prClip->y1 >= drect->y + drect->h) ||
            (prClip->y2 < drect->y))

    {
#ifdef CC_GFX_LOG
        printf("  clip error\n");
#endif
        return false;
    }
    else
    {

        if (prClip->x1 > drect->x)
        {
            rStretchBlit.u4ClipX = prClip->x1 - drect->x;
            rStretchBlit.u4ClipWidth += drect->x -prClip->x1;
            rStretchBlit.u4DstX = prClip->x1;
        }
        else
        {
            rStretchBlit.u4ClipX = 0;
        }

        if (prClip->y1 > drect->y)
        {
            rStretchBlit.u4ClipY = prClip->y1 - drect->y;
            rStretchBlit.u4ClipHeight += drect->y -prClip->y1;
            rStretchBlit.u4DstY = prClip->y1;
        }
        else
        {
            rStretchBlit.u4ClipY = 0;
        }

        if (prClip->x2 < (rStretchBlit.u4DstX + rStretchBlit.u4ClipWidth - 1))
        {
            rStretchBlit.u4ClipWidth = prClip->x2 - rStretchBlit.u4DstX + 1;
        }
        if (prClip->y2 < (rStretchBlit.u4DstY + rStretchBlit.u4ClipHeight - 1))
        {
            rStretchBlit.u4ClipHeight = prClip->y2 - rStretchBlit.u4DstY+ 1;
        }

    }

    MT53_FLUSH_CMDQUE_IFTH();
    if((!mt_get_render_option()||
       (mt_get_render_option()&&(_pCardState->render_options&(DSRO_SMOOTH_UPSCALE|DSRO_SMOOTH_DOWNSCALE)))))
    {
        mt53_gfx_set_mmu(&rStretchBlit.mmu);
        
        if (ioctl(mdrv->fd, FBIO_GFX_STRETCH, &rStretchBlit  ) < 0)
        {
            D_PERROR( "MediaTek/Driver: FBIO_GFX_STRETCH failed!\n" );
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        dfb_clip_stretchblit( prClip, srect, drect );
        GFX_SetBltOpt(0, 0, 0);
        GFX_StretchBlt( srect->x, srect->y, srect->w, srect->h, 
                        drect->x, drect->y, drect->w, drect->h);
    }

    return true;
}

int mt53_support_mmu(void)
{
    #ifdef CC_GFX_MMU
    return 1;
    #else
    return 0;
    #endif
}


static void mt53_vgfx_set_mmu(VGFX_PARAM_SET_T *pt_vgfx)
{
    if(NULL == pt_vgfx)
    {
        return;
    }
    
    if(DFB_SYSTEMONLY)
    {
        pt_vgfx->mmu.u4_init         =0x1;
        pt_vgfx->mmu.u4_enable       =0x1;
        pt_vgfx->mmu.u4_op_md        =0x0;
        pt_vgfx->mmu.u4_src_rw_mmu   =0x1;
        pt_vgfx->mmu.u4_dst_rw_mmu   =0x1;
        pt_vgfx->mmu.u4_vgfx_ord     =0x1;
        pt_vgfx->mmu.u4_pgt          =0x0;
        pt_vgfx->mmu.u4_vgfx_slva = (pt_vgfx->u4ImgAddr +
            ((pt_vgfx->u4TextureVStart+pt_vgfx->u4TextureHeight)*pt_vgfx->u4ImgPitch));
    }
    else
    {
        pt_vgfx->mmu.u4_init         =0x1;
        pt_vgfx->mmu.u4_enable       =0x0;
        pt_vgfx->mmu.u4_op_md        =0x0;
        pt_vgfx->mmu.u4_src_rw_mmu   =0x0;
        pt_vgfx->mmu.u4_dst_rw_mmu   =0x0;
        pt_vgfx->mmu.u4_vgfx_ord     =0x0;
        pt_vgfx->mmu.u4_vgfx_slva    =0x0;
        pt_vgfx->mmu.u4_pgt          =0x0;

    }

    return;
}

static bool mt53_vgfx_get_scaler_info(
    GFX_STRETCH_BLIT_T*             prStretchBlit,
    VGFX_PARAM_SET_T*               pt_vgfx_scaler )
{
    u32  t_sc_test[10] = {0xff,0xff,0xff,1,0,0,0,0,1};
    
    if((NULL == pt_vgfx_scaler)||(NULL == prStretchBlit))
    {
        return false;
    }

    pt_vgfx_scaler->u4ImgAddr         = prStretchBlit->u4Src;
    pt_vgfx_scaler->u4ImgPitch        = prStretchBlit->u4SrcPitch;
    pt_vgfx_scaler->u4ImgClrMode      = prStretchBlit->eSrcCm;
    pt_vgfx_scaler->u4ImgGlobalAlpha  = t_sc_test[0]; 

    pt_vgfx_scaler->u4TextureHStart   = prStretchBlit->u4SrcX;
    pt_vgfx_scaler->u4TextureVStart   = prStretchBlit->u4SrcY;
    pt_vgfx_scaler->u4TextureWidth    = (prStretchBlit->u4SrcWidth);
    pt_vgfx_scaler->u4TextureHeight   = (prStretchBlit->u4SrcHeight);

    pt_vgfx_scaler->u4WBAddr          = prStretchBlit->u4Dst;
    pt_vgfx_scaler->u4WBPitch         = prStretchBlit->u4DstPitch;
    pt_vgfx_scaler->u4WBXStart        = prStretchBlit->u4DstX;
    pt_vgfx_scaler->u4WBYStart        = prStretchBlit->u4DstY;
    pt_vgfx_scaler->u4WBWidth         = prStretchBlit->u4DstWidth;
    pt_vgfx_scaler->u4WBHeight        = prStretchBlit->u4DstHeight;
    pt_vgfx_scaler->u4WBClrMode       = prStretchBlit->eDstCm;
    pt_vgfx_scaler->u4WBGlobalAlpha   = t_sc_test[1]; 

    pt_vgfx_scaler->u4BBXStart        = prStretchBlit->u4ClipX;
    pt_vgfx_scaler->u4BBYStart        = prStretchBlit->u4ClipY;
    pt_vgfx_scaler->u4BBWidth         = prStretchBlit->u4ClipWidth;
    pt_vgfx_scaler->u4BBHeight        = prStretchBlit->u4ClipHeight;
    
    pt_vgfx_scaler->u4AlphaCompAr     = t_sc_test[2]; 
    pt_vgfx_scaler->fgAlphaCompEn     = t_sc_test[3]; 
    pt_vgfx_scaler->u4AlphaCompOpCode = t_sc_test[4]; 
    pt_vgfx_scaler->fgAlphaCompNormal = t_sc_test[5]; 

    pt_vgfx_scaler->u4AlphaEdgeMode   = t_sc_test[6]; 
    pt_vgfx_scaler->fgAntiAliasing    = t_sc_test[7]; 
    pt_vgfx_scaler->fgFilterEnable    = t_sc_test[8]; 
    
    pt_vgfx_scaler->u4FilterType          = (UINT32)VG_FILTER_TYPE_4TAP;
    pt_vgfx_scaler->u4Filter4TapDefault   = (UINT32)1;
    pt_vgfx_scaler->t_reflect.u4RefEn     = (UINT32)0x0;
    pt_vgfx_scaler->u4AlEdgeRGB0          = (UINT32)0x0;
    pt_vgfx_scaler->u4AlEdgeThresHold     = (UINT32)0x0;
        
    VGFX_Scaler_Matrix(pt_vgfx_scaler);     

    return true;
}

bool mt53_ioctl_vgfx_scaler( void *drv, void *dev, DFBRectangle *srect, DFBRectangle *drect )
{
    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;
    GFX_STRETCH_BLIT_T rStretchBlit;
    VGFX_PARAM_SET_T t_vgfx_scaler;
    u32 SrcCm           = 0;
    u32 DstCm           = 0;
    u32 ui4_clip_adj   = 0x2;

    DFBRegion *prClip = &_pCardState->clip;

    if ((srect->w == 0) || (srect->h == 0)||(drect->w == 0)||(drect->h == 0))
    {
        return true;
    }
    
    if( ((prClip->x2 - prClip->x1) ==0x0) || ((prClip->y2 - prClip->y1) ==0x0))
    {
        return true;
    }

    #if 0
    printf("cliprect[%d,%d,%d,%d] \n",
                    (prClip->x1),
                    (prClip->y1),
                    (prClip->x2 - prClip->x1),
                    (prClip->y2 - prClip->y1));
    printf("srect[%d,%d,%d,%d] \n",
                    (srect->x),
                    (srect->y),
                    (srect->w),
                    (srect->h));
    printf("drect[%d,%d,%d,%d] \n",
                    (drect->x),
                    (drect->y),
                    (drect->w),
                    (drect->h));
    #endif
    
    switch (mdev->src_format)
    {
        case DSPF_RGB16:
        SrcCm = CM_RGB565_DIRECT16;
        break;

        case DSPF_ARGB4444:
        SrcCm = CM_ARGB4444_DIRECT16;
        break;

        case DSPF_ARGB:
        case DSPF_AYUV:
        SrcCm = CM_ARGB8888_DIRECT32;
        break;

        default:
        D_BUG( "unexpected src format 0x%08x", mdev->src_format );
        return false;
    }

    switch (mdev->dst_format)
    {
        case DSPF_RGB16:
        DstCm = CM_RGB565_DIRECT16;
        break;

        case DSPF_ARGB4444:
        DstCm = CM_ARGB4444_DIRECT16;
        break;

        case DSPF_ARGB:
        case DSPF_AYUV:
        DstCm = CM_ARGB8888_DIRECT32;
        break;

        default:
        D_BUG( "unexpected dst format 0x%08x", mdev->dst_format );
        return false;
    }

    rStretchBlit.u4Src = mdev->src_phys;
    rStretchBlit.u4Dst = mdev->dst_phys;
    
    if(DFB_SYSTEMONLY)
    {
        rStretchBlit.u4Src = (unsigned int)mdev->src_addr;
        rStretchBlit.u4Dst = (unsigned int)mdev->dst_addr;
    }
    else
    {
        rStretchBlit.u4Src = mdev->src_phys;
        rStretchBlit.u4Dst = mdev->dst_phys;
    }
    
    rStretchBlit.eSrcCm = SrcCm;
    rStretchBlit.eDstCm = DstCm;
    rStretchBlit.u4SrcPitch = mdev->src_pitch;
    rStretchBlit.u4DstPitch = mdev->dst_pitch;
    rStretchBlit.u4SrcX = srect->x;
    rStretchBlit.u4SrcY = srect->y;
    rStretchBlit.u4SrcWidth = srect->w;
    rStretchBlit.u4SrcHeight = srect->h;
    rStretchBlit.u4DstWidth = drect->w;
    rStretchBlit.u4DstHeight = drect->h;
    rStretchBlit.u4DstX = drect->x;
    rStretchBlit.u4DstY = drect->y;
    rStretchBlit.u4ClipWidth = drect->w;
    rStretchBlit.u4ClipHeight = drect->h;

    rStretchBlit.u4Ar       = mdev->color.a;
    rStretchBlit.u4Porterdu = (mdev->porterduff -1);

    if ((prClip->x1 >= drect->x + drect->w) ||
            (prClip->x2 < drect->x) ||
            (prClip->y1 >= drect->y + drect->h) ||
            (prClip->y2 < drect->y))

    {
        return false;
    }
    
    if( (prClip->x1 >= drect->x) && ( prClip->x2 <= (drect->x+drect->w)))
    {
        rStretchBlit.u4ClipX        = prClip->x1;
        rStretchBlit.u4ClipWidth    = (prClip->x2 -prClip->x1);

        if( (drect->x + ui4_clip_adj) <= prClip->x1 )
        {
            rStretchBlit.u4ClipX    -= (ui4_clip_adj);
            rStretchBlit.u4ClipWidth+= (ui4_clip_adj);
        }
        
        if((rStretchBlit.u4ClipX+rStretchBlit.u4ClipWidth + ui4_clip_adj) <= (drect->x+drect->w))
        {
            rStretchBlit.u4ClipWidth+= (ui4_clip_adj);
        }
    }
    else
    {
        rStretchBlit.u4ClipX        = drect->x;
        rStretchBlit.u4ClipWidth    = drect->w;
    }

    if((prClip->y1 >= drect->y)&&( prClip->y2 <= (drect->y+drect->h)))
    {
        rStretchBlit.u4ClipY        = prClip->y1;
        rStretchBlit.u4ClipHeight   = (prClip->y2 - prClip->y1);

        if( (drect->y + ui4_clip_adj) <= prClip->y1 )
        {
            rStretchBlit.u4ClipY     -= (ui4_clip_adj);
            rStretchBlit.u4ClipHeight+= (ui4_clip_adj);
        }
        
        if((rStretchBlit.u4ClipY+rStretchBlit.u4ClipHeight + ui4_clip_adj) <= (drect->y+drect->h))
        {
            rStretchBlit.u4ClipHeight+= (ui4_clip_adj);
        }
    }
    else
    {
        rStretchBlit.u4ClipY        = drect->y;
        rStretchBlit.u4ClipHeight   = drect->h;
    }

    MT53_FLUSH_CMDQUE_IFTH();
    mt53_vgfx_get_scaler_info(&rStretchBlit,&t_vgfx_scaler);
    t_vgfx_scaler.u4AlphaCompAr = mdev->color.a;
    t_vgfx_scaler.u4AlphaCompOpCode = (mdev->porterduff -1);
    t_vgfx_scaler.u4PreSrcRd = _u4PremultSrc;
    t_vgfx_scaler.u4PreDstRd = _u4PremultDst;
    t_vgfx_scaler.u4PreDstWt = _u4PremultWDst;
    t_vgfx_scaler.u4PreOverFlowEn = 0x1;

    mt53_vgfx_set_mmu(&t_vgfx_scaler);

    if(ioctl(mdrv->fd, FBIO_VGFX_STRETCH_ALCOM, &t_vgfx_scaler ) < 0)
    {
        return false;
    }
    return true;
}



static bool mt53VgfxScaler(GFX_STRETCH_BLIT_T* prStretchBlit)
{
    VGFX_PARAM_SET_T t_vgfx_scaler;

    t_vgfx_scaler.u4ImgAddr         = prStretchBlit->u4Src;
    t_vgfx_scaler.u4ImgPitch        = prStretchBlit->u4SrcPitch;
    t_vgfx_scaler.u4ImgClrMode      = prStretchBlit->eSrcCm;
    t_vgfx_scaler.u4ImgGlobalAlpha  = 0;

    t_vgfx_scaler.u4TextureHStart   = prStretchBlit->u4SrcX;
    t_vgfx_scaler.u4TextureVStart   = prStretchBlit->u4SrcY;
    t_vgfx_scaler.u4TextureWidth    = prStretchBlit->u4SrcWidth;
    t_vgfx_scaler.u4TextureHeight   = prStretchBlit->u4SrcHeight;

    t_vgfx_scaler.u4WBAddr          = prStretchBlit->u4Dst;
    t_vgfx_scaler.u4WBPitch         = prStretchBlit->u4DstPitch;
    t_vgfx_scaler.u4WBHeight        = prStretchBlit->u4DstHeight;
    t_vgfx_scaler.u4WBClrMode       = prStretchBlit->eDstCm;
    t_vgfx_scaler.u4WBGlobalAlpha   = 0;

    t_vgfx_scaler.u4BBXStart        = prStretchBlit->u4ClipX;
    t_vgfx_scaler.u4BBYStart        = prStretchBlit->u4ClipY;
    t_vgfx_scaler.u4BBWidth         = prStretchBlit->u4ClipWidth;
    t_vgfx_scaler.u4BBHeight        = prStretchBlit->u4ClipWidth;

    t_vgfx_scaler.u4AlphaCompAr     = 0;
    t_vgfx_scaler.fgAlphaCompEn     = 1;
    t_vgfx_scaler.u4AlphaCompOpCode = 0;
    t_vgfx_scaler.fgAlphaCompNormal = 0;

    t_vgfx_scaler.u4AlphaEdgeMode   = 2;
    t_vgfx_scaler.fgAntiAliasing    = 0;
    t_vgfx_scaler.fgFilterEnable    = 1;


    VGFX_Scaler(&t_vgfx_scaler);

    return true;
}

static bool mt53StretchBlitBlend( void *drv, void *dev, DFBRectangle *srect, DFBRectangle *drect )
{
    #ifdef CC_VGFX_SCALER
    return mt53_ioctl_vgfx_scaler(drv,dev,srect,drect);
    #else
    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;
    GFX_STRETCH_BLIT_T rStretchBlit;

    u32 SrcCm = 0;
    u32 DstCm = 0;
    DFBRegion *prClip = &_pCardState->clip;

    if ((srect->w == 0) || (srect->h == 0)||(drect->w == 0)||(drect->h == 0))
    {
        return true;
    }

    if ((drect->w * drect->h) <= (TMPBUF_WIDTH*TMPBUF_HEIGHT))
    {
        switch (mdev->src_format)
        {
        case DSPF_RGB16:
            SrcCm = CM_RGB565_DIRECT16;
            break;

        case DSPF_ARGB4444:
            SrcCm = CM_ARGB4444_DIRECT16;
            break;

        case DSPF_ARGB:
        case DSPF_AYUV:
            SrcCm = CM_ARGB8888_DIRECT32;
            break;

        default:
            D_BUG( "unexpected src format 0x%08x", mdev->src_format );
            return false;
        }

        switch (mdev->dst_format)
        {
        case DSPF_RGB16:
            DstCm = CM_RGB565_DIRECT16;
            break;

        case DSPF_ARGB4444:
            DstCm = CM_ARGB4444_DIRECT16;
            break;

        case DSPF_ARGB:
        case DSPF_AYUV:
            DstCm = CM_ARGB8888_DIRECT32;
            break;

        default:
            D_BUG( "unexpected dst format 0x%08x", mdev->dst_format );
            return false;
        }

        if(DFB_SYSTEMONLY)
        {
            rStretchBlit.u4Src = (unsigned int)mdev->src_addr;
            rStretchBlit.u4Dst = (unsigned int)mdev->dst_addr;
        }
        else
        {
            rStretchBlit.u4Src = mdev->src_phys;
            rStretchBlit.u4Dst = mdev->dst_phys;
        }
    
        rStretchBlit.eSrcCm = SrcCm;
        rStretchBlit.eDstCm = DstCm;
        rStretchBlit.u4SrcPitch = mdev->src_pitch;
        rStretchBlit.u4DstPitch = mdev->dst_pitch;
        rStretchBlit.u4SrcX = srect->x;
        rStretchBlit.u4SrcY =  srect->y;
        rStretchBlit.u4SrcWidth = srect->w;
        rStretchBlit.u4SrcHeight = srect->h;
        rStretchBlit.u4DstWidth = drect->w;
        rStretchBlit.u4DstHeight = drect->h;
        rStretchBlit.u4DstX = drect->x;
        rStretchBlit.u4DstY = drect->y;
        rStretchBlit.u4ClipWidth = drect->w;
        rStretchBlit.u4ClipHeight = drect->h;

        rStretchBlit.u4Ar = mdev->color.a;
        rStretchBlit.u4Porterdu = (mdev->porterduff -1);

        if ((prClip->x1 >= drect->x + drect->w) ||
                (prClip->x2 < drect->x) ||
                (prClip->y1 >= drect->y + drect->h) ||
                (prClip->y2 < drect->y))

        {
            return false;
        }
        else
        {

            if (prClip->x1 > drect->x)
            {
                rStretchBlit.u4ClipX = prClip->x1 - drect->x;
                rStretchBlit.u4ClipWidth += drect->x -prClip->x1;
                rStretchBlit.u4DstX = prClip->x1;
            }
            else
            {
                rStretchBlit.u4ClipX = 0;
            }

            if (prClip->y1 > drect->y)
            {
                rStretchBlit.u4ClipY = prClip->y1 - drect->y;
                rStretchBlit.u4ClipHeight += drect->y -prClip->y1;
                rStretchBlit.u4DstY = prClip->y1;
            }
            else
            {
                rStretchBlit.u4ClipY = 0;
            }

            if (prClip->x2 < (rStretchBlit.u4DstX + rStretchBlit.u4ClipWidth - 1))
            {
                rStretchBlit.u4ClipWidth = prClip->x2 - rStretchBlit.u4DstX + 1;
            }
            if (prClip->y2 < (rStretchBlit.u4DstY + rStretchBlit.u4ClipHeight - 1))
            {
                rStretchBlit.u4ClipHeight = prClip->y2 - rStretchBlit.u4DstY+ 1;
            }

        }
        MT53_FLUSH_CMDQUE_IFTH();

        if((!mt_get_render_option()||
            (mt_get_render_option()&&(_pCardState->render_options&(DSRO_SMOOTH_UPSCALE|DSRO_SMOOTH_DOWNSCALE)))))
        {
            mt53_stretch_blit_set_mmu(&rStretchBlit);
            rStretchBlit.u4PremultSrc      = _u4PremultSrc;
            rStretchBlit.u4PremultDst      = _u4PremultDst;
            rStretchBlit.u4PremultWDst     = _u4PremultWDst;
            rStretchBlit.u4PremultOvrflw   = !_u4_render_flag;
            if (ioctl(mdrv->fd, FBIO_IMGRZ_STRETCH_ALCOM, &rStretchBlit  ) < 0)
            {
                D_PERROR( "MediaTek/Driver: FBIO_IMGRZ_STRETCH_ALCOM failed!\n" );
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            dfb_clip_stretchblit( prClip, srect, drect );
            GfxLock(drv);
            GFX_SetPremult(_u4PremultSrc, _u4PremultDst, _u4PremultWDst, !_u4_render_flag);
            GFX_StretchAlphaCom( DFB_RECTANGLE_VALS( srect ), DFB_RECTANGLE_VALS( drect ) ,
                                 mdev->color.a, (mdev->porterduff -1));
            GfxUnlock(drv);
            return true;
        }
    }
    else
    {
        dfb_clip_stretchblit( prClip, srect, drect );
        
        GfxLock(drv);

        GFX_SetPremult(_u4PremultSrc, _u4PremultDst, _u4PremultWDst, !_u4_render_flag);
        GFX_StretchAlphaCom( DFB_RECTANGLE_VALS( srect ), DFB_RECTANGLE_VALS( drect ) ,
                             mdev->color.a, (mdev->porterduff -1));
        GfxUnlock(drv);
        return true;
    }
    #endif
}


#ifdef CC_DFB_SUPPORT_VDP_LAYER
static bool
mt53Rotate( void *drv, void *dev, DFBRectangle *srect, int dx, int dy )
{
    struct mt53fb_vdp_rotate rVdpBlit;
    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;    
    rVdpBlit.u4Src = mdev->src_phys;
    rVdpBlit.u4Dst = mdev->dst_phys;
    rVdpBlit.eSrcCm = CM_RGB_CLUT8;
    rVdpBlit.eDstCm = CM_RGB_CLUT8;
    rVdpBlit.u4SrcPitch = mdev->src_pitch;
    rVdpBlit.u4DstPitch = mdev->dst_pitch;
    rVdpBlit.u4SrcX = srect->x;
    rVdpBlit.u4SrcY =  srect->y;
    rVdpBlit.u4Width = srect->w;
    rVdpBlit.u4Height = srect->h;    
    rVdpBlit.u4DstX = dx;   
    rVdpBlit.u4DstY = dy;    
    rVdpBlit.u4RotateOp = mdev->rotateopt;
    rVdpBlit.u4SrcCbCrOffset = mdev->src_cbcr_offset;
    rVdpBlit.u4DstCbCrOffset = mdev->dst_cbcr_offset;
    
    if(mdev->src_format != DSPF_NV16 || mdev->dst_format != DSPF_NV16)
    {   
        return false;
    }
   
    MT53_FLUSH_CMDQUE_IFTH();
    if (ioctl(mdrv->fd, FBIO_VDP_ROTATE, &rVdpBlit  ) < 0)
    {    
          D_PERROR( "MediaTek/Driver: FBIO_VDP_BLIT failed!\n" );
          return false;
    }
    else
    {        
        return true;
    }    

}


#endif
#ifdef CC_B2R44K2K_SUPPORT

static int b2rStretchBlit(mt53fb_b2r_stretch_blit * pt_b2r_stretch_blit)
{
    if (ioctl( dfb_mt53->fd, FBIO_B2R_STRETCH_BLIT, pt_b2r_stretch_blit ) < 0) 
    {
        return DFB_IO;
    }
     
    return DFB_OK;
}

static bool
mt53B2RStretchBlit( void *drv, void *dev, DFBRectangle *srect, DFBRectangle *drect )
{
    D_DEBUG_AT( MT53_2D, "%s( %d,%d-%dx%d -> %d, %d )\n", __FUNCTION__,
                DFB_RECTANGLE_VALS( srect ), 0/*dx*/, 0/*dy*/ );

    MT53DeviceData *mdev = (MT53DeviceData*) dev;
    MT53DriverData *mdrv = (MT53DriverData*) drv;
    mt53fb_b2r_stretch_blit t_b2r_stretch_blit;

    if ((srect->w == 0) || (srect->h == 0)||(drect->w == 0)||(drect->h == 0))
    {
        return true;
    }

    t_b2r_stretch_blit.src_buffer_Y_addr = mdev->src_phys;
    t_b2r_stretch_blit.src_buffer_C_addr = mdev->src_cbcr_offset + t_b2r_stretch_blit.src_buffer_Y_addr;
    t_b2r_stretch_blit.src_buffer_Y_width = srect->w;
    t_b2r_stretch_blit.src_buffer_Y_height = srect->h;
    t_b2r_stretch_blit.src_buffer_Y_pitch = mdev->src_pitch;

    t_b2r_stretch_blit.dest_buffer_Y_addr = mdev->dst_phys;
    t_b2r_stretch_blit.dest_buffer_C_addr = mdev->dst_cbcr_offset + t_b2r_stretch_blit.dest_buffer_Y_addr;
    t_b2r_stretch_blit.dest_buffer_Y_width = drect->w;
    t_b2r_stretch_blit.dest_buffer_Y_height = drect->h;
    t_b2r_stretch_blit.dest_buffer_Y_pitch = mdev->dst_pitch;

    t_b2r_stretch_blit.src_clip_x= srect->x;
    t_b2r_stretch_blit.src_clip_y = srect->y;
    t_b2r_stretch_blit.src_clip_w = srect->w;
    t_b2r_stretch_blit.src_clip_h = srect->h;

    t_b2r_stretch_blit.dest_clip_x= drect->x;
    t_b2r_stretch_blit.dest_clip_y = drect->y;
    t_b2r_stretch_blit.dest_clip_w = drect->w;
    t_b2r_stretch_blit.dest_clip_h = drect->h;


    return b2rStretchBlit(&t_b2r_stretch_blit);
}
#endif

