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

#include <pthread.h>
#include <assert.h>

#include <sys/ioctl.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <asm/types.h>

#include <directfb.h>

#include <direct/messages.h>

#include <core/palette.h>
#include <core/surface.h>
#include <core/layers_internal.h>

#include <gfx/convert.h>
#include <gfx_if.h>

#include "mt53.h"

typedef enum
{
    OSD_BM_NONE,
    OSD_BM_PIXEL,
    OSD_BM_REGION,
    OSD_BM_PLANE
} OSD_BLEND_MODE_T;

static void primary_set_region_mmu(GFX_MMU_T *pt_mmu)
{
    #if defined(CONFIG_OSD_COMPRESSION)
    if(DFB_SYSTEMONLY)
    {
        pt_mmu->u4_init           = 0x1;
        pt_mmu->u4_enable         = 0x1;
        pt_mmu->u4_op_md          = OP_NEW_COMPRESS_BITBLT;
        pt_mmu->u4_src_rw_mmu     = 0x1;
        pt_mmu->u4_dst_rw_mmu     = 0x0;
        pt_mmu->u4_vgfx_ord       = 0x0;
        pt_mmu->u4_vgfx_slva      = 0x0;
        pt_mmu->u4_pgt            = 0x0;
    }
    else
    #endif
    {
        pt_mmu->u4_init           = 0x1;
        pt_mmu->u4_enable         = 0x0;
        pt_mmu->u4_op_md          = 0x0;
        pt_mmu->u4_src_rw_mmu     = 0x0;
        pt_mmu->u4_dst_rw_mmu     = 0x0;
        pt_mmu->u4_vgfx_ord       = 0x0;
        pt_mmu->u4_vgfx_slva      = 0x0;
        pt_mmu->u4_pgt            = 0x0;
    }

    return;
}

static unsigned int _mapColorFormat(DFBSurfacePixelFormat format) 
{
    unsigned int mt_format = CM_Reserved0;
    switch(format)
    {
        case DSPF_UNKNOWN:
        case DSPF_ARGB1555:         
            break;
        case DSPF_RGB16:  
            mt_format = CM_RGB565_DIRECT16;
            break;          
        case DSPF_RGB24:
        case DSPF_RGB32:
            break;
        case DSPF_ARGB:
            mt_format = CM_ARGB8888_DIRECT32;
            break;
        case DSPF_A8:
        case DSPF_YUY2:
            mt_format = CM_YCbYCr422_DIRECT16;          
            break;          
        case DSPF_RGB332:
            break;
        case DSPF_UYVY:
            mt_format = CM_CbYCrY422_DIRECT16;          
            break;
        case DSPF_I420:
        case DSPF_YV12:
            break;     
        case DSPF_LUT8:
            mt_format = CM_RGB_CLUT8;           
            break;
        case DSPF_ALUT44:
        case DSPF_AiRGB:
        case DSPF_A1:
        case DSPF_NV12:
        case DSPF_NV16:
        case DSPF_ARGB2554:
            break;
        case DSPF_ARGB4444:
            mt_format = CM_ARGB4444_DIRECT16;
            break;
        case DSPF_NV21:
            break;
        case DSPF_AYUV:
            mt_format = CM_AYCbCr8888_DIRECT32;
            break;          
        case DSPF_A4:
        case DSPF_ARGB1666:
        case DSPF_ARGB6666:
        case DSPF_RGB18:
        case DSPF_LUT2:
        case DSPF_RGB444:
        case DSPF_RGB555:
            break;
      }
    return mt_format;
}

/**********************************************************************************************************************/

static DFBResult
primaryInitScreen( CoreScreen           *screen,
                   CoreGraphicsDevice   *device,
                   void                 *driver_data,
                   void                 *screen_data,
                   DFBScreenDescription *description )
{
     /* Set the screen capabilities. */
     description->caps = DSCCAPS_NONE;

     /* Set the screen name. */
     snprintf( description->name,
               DFB_SCREEN_DESC_NAME_LENGTH, "MediaTek 53xx Primary Screen" );

     return DFB_OK;
}

static DFBResult
primaryGetScreenSize( CoreScreen *screen,
                      void       *driver_data,
                      void       *screen_data,
                      int        *ret_width,
                      int        *ret_height )
{
    unsigned int data[2];
    struct mt53fb_get get;
    get.get_type = MT53FB_GET_PANEL_SIZE;
    get.get_size = 8;
    get.get_data = data;
    if (ioctl( dfb_mt53->fd, FBIO_GET, &get ) < 0) {
       D_PERROR( "DirectFB/MT53: FBIO_SET failed!\n" );
       return DFB_IO;
    }
    
    *ret_width  = data[0];
    *ret_height = data[1];
      
     return DFB_OK;
}

//#ifdef CC_HW_WINDOW_SUPPORT
DFBResult mdfb_wait_for_sync(void)
{
    struct mt53fb_set set;
	
    set.mask                    = MT53FB_SET_MASK_WAITFORSYNC;
   
    if(ioctl( dfb_mt53->fd, FBIO_SET, &set ) < 0) 
    {
        D_PERROR( "DirectFB/MT53: FBIO_SET MT53FB_SET_MASK_WAIT_VSYNC failed!\n" );
        return DFB_IO;
    }

	return DFB_OK;

}
//#endif

static DFBResult primaryWaitVSync( CoreScreen			 *screen,
							           void					 *driver_data,
							           void					 *screen_data )
{
#ifdef CC_HW_WINDOW_SUPPORT
    mdfb_wait_for_sync();
#endif
    return DFB_OK;
}


DFBResult mdfb_check_wait_vsync(CoreLayer* pt_this)
{
    struct mt53fb_set set;
    MT53LayerData  *pt_layer_data = (MT53LayerData  *)pt_this->layer_data;
        
    set.mask                    = MT53FB_SET_MASK_WAIT_VSYNC;
    set.rSetting.u4OsdPlaneID   = pt_layer_data->layer;
    
    if(ioctl( dfb_mt53->fd, FBIO_SET, &set ) < 0) 
    {
        D_PERROR( "DirectFB/MT53: FBIO_SET MT53FB_SET_MASK_WAIT_VSYNC failed!\n" );
        return DFB_IO;
    }

    return DFB_OK;     
}


/**********************************************************************************************************************/

/*const*/ ScreenFuncs mt53PrimaryScreenFuncs = {
     InitScreen:    primaryInitScreen,
     GetScreenSize: primaryGetScreenSize,
     WaitVSync:     primaryWaitVSync
};

/**********************************************************************************************************************/
/**********************************************************************************************************************/

static int
primaryLayerDataSize()
{
     return sizeof(MT53LayerData);
}

static int
primaryRegionDataSize()
{
#ifdef CC_HW_WINDOW_SUPPORT
	 return sizeof(MT53RegionData);
#else
	 return 0;
#endif
}

static DFBResult
primaryInitLayer( CoreLayer                  *layer,
                  void                       *driver_data,
                  void                       *layer_data,
                  DFBDisplayLayerDescription *description,
                  DFBDisplayLayerConfig      *config,
                  DFBColorAdjustment         *adjustment )
{

     MT53DriverData *sdrv = driver_data;
     MT53LayerData  *data = layer_data;

     data->layer = sdrv->layer_plane[sdrv->num_plane++];

     /* set capabilities and type */
#ifdef CC_HW_WINDOW_SUPPORT
	  description->caps = DLCAPS_SURFACE | DLCAPS_SCREEN_POSITION | DLCAPS_SCREEN_SIZE | DLCAPS_LEVELS | DLCAPS_OPACITY | DLCAPS_ALPHACHANNEL | DLCAPS_WINDOWS;
#else
	  description->caps = DLCAPS_SURFACE | DLCAPS_SCREEN_POSITION | DLCAPS_SCREEN_SIZE | DLCAPS_LEVELS | DLCAPS_OPACITY | DLCAPS_ALPHACHANNEL;
#endif
     description->type = DLTF_GRAPHICS;

     /* set name */
     snprintf( description->name,
               DFB_DISPLAY_LAYER_DESC_NAME_LENGTH, "MediaTek 53xx Primary Layer" );

     /* fill out the default configuration */
     config->flags       = DLCONF_WIDTH       | DLCONF_HEIGHT |
                           DLCONF_PIXELFORMAT | DLCONF_BUFFERMODE | DLCONF_OPTIONS;
     config->width       = MT53_PRIMARY_DEFAULT_WIDTH;
     config->height      = MT53_PRIMARY_DEFAULT_HEIGHT;
     config->pixelformat = DSPF_ARGB;
     config->buffermode  = DLBM_FRONTONLY;
     config->options     = DLOP_NONE;
     
     return DFB_OK;
}

static DFBResult
primaryTestRegion( CoreLayer                  *layer,
                   void                       *driver_data,
                   void                       *layer_data,
                   CoreLayerRegionConfig      *config,
                   CoreLayerRegionConfigFlags *failed )
{
     CoreLayerRegionConfigFlags fail = CLRCF_NONE;

     /* check options */
     if (config->options & ~MT53_PRIMARY_SUPPORTED_OPTIONS)
     {
          fail |= CLRCF_OPTIONS;
     }
     
     /* check format */
     switch (config->format) {
          case DSPF_LUT8:
          case DSPF_ARGB4444:
          case DSPF_ARGB:
          case DSPF_RGB16:              
          case DSPF_AYUV:           
          case DSPF_YUY2:
          case DSPF_UYVY:
#ifdef CC_B2R44K2K_SUPPORT	  
          case DSPF_NV16:
#endif		  	
               break;

          default:
               fail |= CLRCF_FORMAT;
               break;
     }
#ifndef CC_B2R44K2K_SUPPORT

     /* check width */
     if (config->width > 1920 || config->width < 2)
          fail |= CLRCF_WIDTH;

     /* check height */
     if (config->height > 1080 || config->height < 2)
          fail |= CLRCF_HEIGHT;
#endif
     /* write back failing fields */
     if (failed)
          *failed = fail;

     /* return failure if any field failed */
     if (fail)
          return DFB_UNSUPPORTED;

     return DFB_OK;
}

static DFBResult
primarySetRegion(  CoreLayer                  *layer,
                    void                       *driver_data,
                    void                       *layer_data,
                    void                       *region_data,
                    CoreLayerRegionConfig      *config,
                    CoreLayerRegionConfigFlags  updated,
                    CoreSurface                *surface,
                    CorePalette                *palette,
                    CoreSurfaceBufferLock      *left_lock, 
                    CoreSurfaceBufferLock      *right_lock )
{
     struct mt53fb_set set;

     MT53LayerData  *data = layer_data;
     CoreSurfaceBufferLock* lock=left_lock;
     assert(data->layer < MT53_MAX_OSD_PLANE);
#ifdef CC_HW_WINDOW_SUPPORT
	 data->curregion = region_data;
#endif

     if (updated & ~CLRCF_PALETTE) {
          set.rSetting.u4Pitch = lock->pitch;
          set.rSetting.u1CM = _mapColorFormat(config->format);
          set.rSetting.u2W       = config->width;
          set.rSetting.u2H       = config->height;

          set.rSetting.u4Base    = lock->phys;
          
          set.rSetting.fgVisible = 1;         
          
          set.rSetting.u2OffsetX = config->dest.x;
          set.rSetting.u2OffsetY = config->dest.y;
          set.rSetting.u2X = config->dest.x;
          set.rSetting.u2Y = config->dest.y;
          
          set.rSetting.u4OsdPlaneID = data->layer;
            
          set.mask = 0;
          
         if(config->options & DLOP_OPACITY)
         {
             set.rSetting.u4Opacity = config->opacity;
             set.mask |= MT53FB_SET_MASK_OPACITY;             
         }
         else
         {
             set.rSetting.u4Opacity = 0xff;
         }
         
         /* Allen Kao 20090928 use only pixel alpha*/
         //if(config->options & DLOP_ALPHACHANNEL)
         //{
             set.rSetting.u4MixSel = OSD_BM_PIXEL;
         //}
         //else
         //{
         //    set.rSetting.u4MixSel = OSD_BM_PLANE;
         //}
         
         if(config->options & DLOP_SRC_COLORKEY)
         {
             unsigned int color_key = 0;
             switch(config->format)
                {
               case DSPF_ARGB4444: /* ??? */
//                    color_key = PIXEL_ARGB4444(0xff, config->src_key.r, config->src_key.g, config->src_key.b);              
                    color_key = PIXEL_ARGB4444(/*0xff*/0, config->src_key.r, config->src_key.g, config->src_key.b);
                     set.mask  |= (MT53FB_SET_MASK_COLORKEY | MT53FB_SET_MASK_COLORKEYEN);                    
                     break;

               case DSPF_ARGB:
                    color_key = PIXEL_ARGB(0xff, config->src_key.r, config->src_key.g, config->src_key.b);
                     set.mask  |= (MT53FB_SET_MASK_COLORKEY | MT53FB_SET_MASK_COLORKEYEN);                                        
                     break;             
               default:
                break;
                }
              set.rSetting.u4ColorKeyEn = 1;                                 
              set.rSetting.u4ColorKey = color_key;                                               
         }
         else
         {
             set.mask  |= MT53FB_SET_MASK_COLORKEYEN;   
             set.rSetting.u4ColorKeyEn = 0;
         }

          set.mask |=  MT53FB_SET_MASK_W |
                     MT53FB_SET_MASK_H |
                     MT53FB_SET_MASK_VIRT_W |
                     MT53FB_SET_MASK_VIRT_H |
                     MT53FB_SET_MASK_VISIBLE |
                     MT53FB_SET_MASK_PITCH |
                     MT53FB_SET_MASK_CM |
                     MT53FB_SET_MASK_BASE |
                     MT53FB_SET_MASK_OFFSET_X |
                     MT53FB_SET_MASK_OFFSET_Y|
                     MT53FB_SET_MASK_X |
                     MT53FB_SET_MASK_Y|

                     MT53FB_SET_MASK_MIXSEL |
                     MT53FB_SET_MASK_PLANE_ID;

          primary_set_region_mmu(&set.rSetting.mmu);
          
          if (ioctl( dfb_mt53->fd, FBIO_SET, &set ) < 0) {
               D_PERROR( "DirectFB/MT53: FBIO_SET failed!\n" );
               return DFB_IO;
          }
     }

     if ((updated & CLRCF_PALETTE) && palette) {
          int i;
          struct mt53fb_palette pale;

          pale.plane_id = data->layer;

          for (i=0; i<256; i++) {
               pale.palette[i] = PIXEL_ARGB( palette->entries[i].a,
                                       palette->entries[i].r,
                                       palette->entries[i].g,
                                       palette->entries[i].b );
          }

          ioctl( dfb_mt53->fd, FBIO_PALETTE, &pale );
     }

     return DFB_OK;
}

static DFBResult
primaryRemoveRegion( CoreLayer             *layer,
                     void                  *driver_data,
                     void                  *layer_data,
                     void                  *region_data )
{
     struct mt53fb_set set;
     MT53LayerData  *data = layer_data;

     assert(data->layer < MT53_MAX_OSD_PLANE);

#ifdef CC_HW_WINDOW_SUPPORT
     if(data->curregion != region_data)
     {
          return DFB_OK;
     }
#endif

     set.rSetting.fgVisible = 0;
     set.rSetting.u4OsdPlaneID = data->layer;
        
     set.mask = MT53FB_SET_MASK_VISIBLE | MT53FB_SET_MASK_PLANE_ID;


     if (ioctl( dfb_mt53->fd, FBIO_SET, &set ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_SET failed!\n" );
          return DFB_IO;
     }

     return DFB_OK;
}


static DFBResult
primaryFlipRegion( CoreLayer                  *layer,
                    void                       *driver_data,
                    void                       *layer_data,
                    void                       *region_data,
                    CoreSurface                *surface,
                    DFBSurfaceFlipFlags         flags,
                    CoreSurfaceBufferLock      *left_lock, 
                    CoreSurfaceBufferLock      *right_lock )
{
     struct mt53fb_set set;
     MT53LayerData  *data = layer_data;
     CoreSurfaceBufferLock *lock=left_lock;

     assert(data->layer < MT53_MAX_OSD_PLANE);

#ifdef CC_HW_WINDOW_SUPPORT
     if(data->curregion != region_data)
     {
          goto _primaryFlipRegion_END;
     }
#endif     
     set.rSetting.u4Base = lock->phys;
     set.rSetting.u4OsdPlaneID = data->layer;
     
     // direct_log_printf( NULL,"primaryFlipRegion[%d,0x%x]\n",set.rSetting.u4OsdPlaneID,set.rSetting.u4Base);
     
     set.mask = MT53FB_SET_MASK_BASE | MT53FB_SET_MASK_PLANE_ID;
     
     primary_set_region_mmu(&set.rSetting.mmu);

     if (ioctl( dfb_mt53->fd, FBIO_SET, &set ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_SET failed!\n" );
          return DFB_IO;
     }
	 
#ifdef CC_HW_WINDOW_SUPPORT
_primaryFlipRegion_END:
#endif

     if(!(flags & DSFLIP_BLIT))
     {
         dfb_surface_flip( surface, false );
     }

     return DFB_OK;
}

DFBResult primaryUpdateRegion( CoreLayer               *layer,
                            void                       *driver_data,
                            void                       *layer_data,
                            void                       *region_data,
                            CoreSurface                *surface,
                            const DFBRegion            *left_update,
                            CoreSurfaceBufferLock      *left_lock, 
                            const DFBRegion            *right_update,
                            CoreSurfaceBufferLock      *right_lock )
{
    return primaryFlipRegion(   layer,driver_data,layer_data,region_data,
                                surface,DSFLIP_BLIT,left_lock,right_lock);
}

static DFBResult 
primaryGetLevel( CoreLayer              *layer,
                                         void                   *driver_data,
                                         void                   *layer_data,
                                         int                    *level )
{
     unsigned int au4Array[MT53_PMX_MAX_INPORT_NS+1];
     unsigned int i;
     MT53LayerData* data = layer_data;

     assert(data->layer < MT53_MAX_OSD_PLANE);
     
     if (ioctl( dfb_mt53->fd, FBIO_GET_PLANE_ORDER_ARRAY, au4Array ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_SET failed!\n" );
          return DFB_IO;
     }
     for(i = 0; i < 5; i++)
     {
/* MHF Linux - Chun */
#if 0
         if(au4Array[i] == data->layer + 2)
#else
         if(au4Array[i] == data->layer + 3)
#endif
/* MHF Linux - Chun end */
         {
             *level = i;
         }
     }         
     
     return DFB_OK;     
}


static DFBResult 
primarySetLevel             ( CoreLayer              *layer,
                                         void                   *driver_data,
                                         void                   *layer_data,
                                         int                     level )
{
     MT53LayerData  *data = layer_data;
     unsigned int i;
     unsigned int j;
     unsigned int au4Array[MT53_PMX_MAX_INPORT_NS+1];
     unsigned int hw_layer_id;
     
     assert(data->layer < MT53_MAX_OSD_PLANE);    

     if (ioctl( dfb_mt53->fd, FBIO_GET_PLANE_ORDER_ARRAY, au4Array ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_SET failed!\n" );
          return DFB_IO;
     }
/* MHF Linux - Chun */
#if 0     
     hw_layer_id = data->layer + 2;
#else
     hw_layer_id = data->layer + 3;
#endif
/* MHF Linux - Chun end */

     for(i = 0; i < 5; i++)
     {
         if(au4Array[i] == hw_layer_id)
         {
             if(level == i)
             {                 
                 //return DFB_OK;
                 break;
             }
             /* MHF Linux - Chun */
             /*CHUN 
               The plane is found.
             */             
             break;
             /* MHF Linux - Chun end */
         }
     }  
/* MHF Linux - Chun */   
     /*HF CHUN for ajust zorder->*/
#if 1
    if (i < 5)
    {
        if(level < i)
        {
            for(j = i; j > level; j --)            
            {
                au4Array[j] = au4Array[j - 1];
            }
        }     
        else 
        if(level > i)
        {
            for(j = i; j < level; j ++)
            {
                au4Array[j] = au4Array[j + 1];
            }
        }
        au4Array[level] = hw_layer_id;
    }
    else
    {
        return DFB_FAILURE;
    }
#else
     au4Array[i] = 0xff; // None
     
     au4Array[level] = hw_layer_id;     
#endif
/* MHF Linux - Chun end */
     if (ioctl( dfb_mt53->fd, FBIO_SET_PLANE_ORDER_ARRAY, au4Array ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_SET failed!\n" );
          return DFB_IO;
     }     

     return DFB_OK;
}


long long mdfb_clock_get_micros()
{   
    struct timeval tv;     
    gettimeofday( &tv, NULL );     
    return (long long)tv.tv_sec * 1000000LL + (long long)tv.tv_usec;
}


/**********************************************************************************************************************/

/*const*/ DisplayLayerFuncs mt53PrimaryLayerFuncs = {
     LayerDataSize:      primaryLayerDataSize,
     RegionDataSize:     primaryRegionDataSize,
     InitLayer:          primaryInitLayer,
     GetLevel:           primaryGetLevel,
     SetLevel:           primarySetLevel,
     TestRegion:         primaryTestRegion,
     SetRegion:          primarySetRegion,
     RemoveRegion:       primaryRemoveRegion,
     FlipRegion:         primaryFlipRegion,
     UpdateRegion:       primaryUpdateRegion
};

