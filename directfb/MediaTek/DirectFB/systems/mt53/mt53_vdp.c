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

static int
vdpLayerDataSize()
{
     return sizeof(MT53LayerData);
}

static int
vdpRegionDataSize()
{
     return 0;
}


int vdpSetCropRect(mt53fb_vdp_crop_rect * pt_vdp_crop_rect)
{
    if (ioctl( dfb_mt53->fd, FBIO_VDP_SET_CROP_RECT, pt_vdp_crop_rect ) < 0) 
    {
        return DFB_IO;
    }
     
    return DFB_OK;
}

int vdpGetCropRect(mt53fb_vdp_crop_rect * pt_vdp_crop_rect)
{
    if (ioctl( dfb_mt53->fd, FBIO_VDP_GET_CROP_RECT, pt_vdp_crop_rect ) < 0) 
    {
        return DFB_IO;
    }
     
    return DFB_OK;
}

int vdpStretchBlit(mt53fb_vdp_stretch_blit * pt_vdp_stretch_blit)
{
    if (ioctl( dfb_mt53->fd, FBIO_VDP_STRETCH_BLIT, pt_vdp_stretch_blit ) < 0) 
    {
        return DFB_IO;
    }
     
    return DFB_OK;
}

static DFBResult
vdpInitLayer( CoreLayer                  *layer,
                  void                       *driver_data,
                  void                       *layer_data,
                  DFBDisplayLayerDescription *description,
                  DFBDisplayLayerConfig      *config,
                  DFBColorAdjustment         *adjustment )
{

     MT53DriverData *sdrv = (MT53DriverData *)driver_data;
     MT53LayerData  *data = (MT53LayerData *)layer_data;

     data->layer = (MT53LayerID)sdrv->num_plane;
	 sdrv->num_plane++;

     /* set capabilities and type */
     description->caps = (DFBDisplayLayerCapabilities)(DLCAPS_SURFACE | DLCAPS_SCREEN_POSITION | DLCAPS_SCREEN_SIZE | DLCAPS_LEVELS | DLCAPS_OPACITY | DLCAPS_ALPHACHANNEL);
     description->type = DLTF_GRAPHICS;

     /* set name */
     snprintf( description->name,
               DFB_DISPLAY_LAYER_DESC_NAME_LENGTH, "MediaTek 53xx VDP Layer" );

     /* fill out the default configuration */
     config->flags       = (DFBDisplayLayerConfigFlags)(DLCONF_WIDTH|DLCONF_HEIGHT|
                            DLCONF_PIXELFORMAT | DLCONF_BUFFERMODE | DLCONF_OPTIONS);
     config->width       = 1920;
     config->height      = 1080;
     config->pixelformat = DSPF_NV16;
     config->buffermode  = DLBM_FRONTONLY;
     config->options     = DLOP_NONE;

     // printf( "DirectFB/MT53: vdpInitLayer!\n" );
     
     return DFB_OK;
}

static DFBResult
vdpTestRegion( CoreLayer                  *layer,
                   void                       *driver_data,
                   void                       *layer_data,
                   CoreLayerRegionConfig      *config,
                   CoreLayerRegionConfigFlags *failed )
{
     CoreLayerRegionConfigFlags fail = CLRCF_NONE;

     // printf( "DirectFB/MT53: vdpTestRegion!\n" );
     
     /* check format */
     switch (config->format) {
          case DSPF_NV16:
               break;

          default:
               fail |= CLRCF_FORMAT;
               break;
     }

     /* check width */
     if (config->width > 1920 || config->width < 2)
     	{
          fail |= ((CoreLayerRegionConfigFlags)CLRCF_WIDTH);
     	}

     /* check height */
     if (config->height > 1080 || config->height < 2)
     	{
          fail |= ((CoreLayerRegionConfigFlags)CLRCF_HEIGHT);
     	}

     /* write back failing fields */
     if (failed)
          *failed = fail;

     /* return failure if any field failed */
     if (fail)
          return DFB_UNSUPPORTED;

     return DFB_OK;
}
                                

static DFBResult
vdpSetRegion( CoreLayer                  *layer,
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
     struct mt53fb_vdp_set set;
     CoreSurfaceBufferLock* lock = left_lock;
     MT53LayerData  *data = (MT53LayerData  *)layer_data;
     mt53fb_vdp_crop_rect  t_vdp_crop_rect;

     
     assert(data->layer == MT53_MAX_OSD_PLANE);

     memset(&t_vdp_crop_rect,0x0,sizeof(mt53fb_vdp_crop_rect));
	 t_vdp_crop_rect.ui4_layer = data->layer;
	 
     vdpGetCropRect(&t_vdp_crop_rect);

     if(t_vdp_crop_rect.ui4_3d_mode & DLOP_3D_MODE)
     {
         data->_3dmode = 1;
     }
     else
     {
         data->_3dmode = 0;
     }
     if(updated & CLRCF_3D_CROP_OFFSET)
     {
         data->u4CropBottomOffset = config->crop_bottom_offset;
         data->u4CropTopOffset = config->crop_top_offset;         
         data->u4CropLeftOffset = config->crop_left_offset;         
         data->u4CropRightOffset = config->crop_right_offset;         
     }
     
     data->height = config->height;
     data->pitch = lock->pitch;
   
     set.ucEnable = 1;

     // printf( "DirectFB/MT53: vdpSetRegion!\n" );

     if (ioctl( dfb_mt53->fd, FBIO_VDP_SET, &set ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_VDP_SET failed!\n" );
          return DFB_IO;
     }
     
     return DFB_OK;
}

static DFBResult
vdpRemoveRegion( CoreLayer             *layer,
                     void                  *driver_data,
                     void                  *layer_data,
                     void                  *region_data )
{
     struct mt53fb_vdp_set set;
     MT53LayerData  *data = (MT53LayerData  *)layer_data;

     assert(data->layer == MT53_MAX_OSD_PLANE);
     
     set.ucEnable = 0;

     // printf( "DirectFB/MT53: vdpRemoveRegion!\n" );

     if (ioctl( dfb_mt53->fd, FBIO_VDP_SET, &set ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_VDP_SET failed!\n" );
          return DFB_IO;
     }

     return DFB_OK;
}

static DFBResult
vdpFlipRegion( CoreLayer                  *layer,
				void                       *driver_data,
				void                       *layer_data,
				void                       *region_data,
				CoreSurface                *surface,
				DFBSurfaceFlipFlags         flags,
				CoreSurfaceBufferLock      *left_lock, 
				CoreSurfaceBufferLock      *right_lock )
{
    struct mt53fb_vdp_flip set;
     MT53LayerData          *data = (MT53LayerData  *)layer_data;
     CoreSurfaceBufferLock *lock  = left_lock;
     mt53fb_vdp_crop_rect  t_vdp_crop_rect;

     assert(data->layer == MT53_MAX_OSD_PLANE);
     
     memset(&t_vdp_crop_rect,0x0,sizeof(mt53fb_vdp_crop_rect));

     t_vdp_crop_rect.ui4_layer = data->layer;
      
     vdpGetCropRect(&t_vdp_crop_rect);

     if(t_vdp_crop_rect.ui4_3d_mode & DLOP_3D_MODE)
     {
         data->_3dmode = 1;
     }
     else
     {
         data->_3dmode = 0;
     }
     
     set.pu1SrcYAddr = (unsigned char*)lock->phys;
     set.pu1SrcCAddr =  (unsigned char*)(lock->phys + lock->pitch * surface->config.size.h);
     set.fgIs3DMode = data->_3dmode;
     set.ui4_pitch = lock->pitch;
     set.u4CropBottomOffset = data->u4CropBottomOffset;
     set.u4CropTopOffset = data->u4CropTopOffset;     
     set.u4CropLeftOffset = data->u4CropLeftOffset;          
     set.u4CropRightOffset = data->u4CropRightOffset;               

     if (ioctl( dfb_mt53->fd, FBIO_VDP_FLIP, &set ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_SET failed!\n" );
          return DFB_IO;
     }

     if(!(flags & DSFLIP_BLIT))
     {
         dfb_surface_flip( surface, false );
     }

     return DFB_OK;
}


static DFBResult
vdpUpdateRegion ( CoreLayer                  *layer,
					void                       *driver_data,
					void                       *layer_data,
					void                       *region_data,
					CoreSurface                *surface,
					const DFBRegion            *left_update,
					CoreSurfaceBufferLock      *left_lock, 
					const DFBRegion            *right_update,
					CoreSurfaceBufferLock      *right_lock )
{
    struct mt53fb_vdp_flip set;
     MT53LayerData  *data = (MT53LayerData  *)layer_data;
     CoreSurfaceBufferLock *lock = left_lock;
	 mt53fb_vdp_crop_rect  t_vdp_crop_rect;

     assert(data->layer == MT53_MAX_OSD_PLANE);
	 
     memset(&t_vdp_crop_rect,0x0,sizeof(mt53fb_vdp_crop_rect));

	 t_vdp_crop_rect.ui4_layer = data->layer;
	  
     vdpGetCropRect(&t_vdp_crop_rect);

     if(t_vdp_crop_rect.ui4_3d_mode & DLOP_3D_MODE)
     {
         data->_3dmode = 1;
     }
     else
     {
         data->_3dmode = 0;
     }
	 
     set.pu1SrcYAddr = (unsigned char*)lock->phys;
     set.pu1SrcCAddr =  (unsigned char*)(lock->phys + lock->pitch * data->height);
     set.fgIs3DMode = data->_3dmode;
     set.ui4_pitch = lock->pitch;
     set.u4CropBottomOffset = data->u4CropBottomOffset;
     set.u4CropTopOffset = data->u4CropTopOffset;     
     set.u4CropLeftOffset = data->u4CropLeftOffset;          
     set.u4CropRightOffset = data->u4CropRightOffset;       
     
     // printf( "DirectFB/MT53: vdpUpdateRegion!\n" );

     if (ioctl( dfb_mt53->fd, FBIO_VDP_FLIP, &set ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_FLIP failed!\n" );
          return DFB_IO;
     }

     return DFB_OK;
}

static DFBResult 
vdpGetLevel( CoreLayer              *layer,
                                         void                   *driver_data,
                                         void                   *layer_data,
                                         int                    *level )
{     
     return DFB_OK;     
}


static DFBResult 
vdpSetLevel             ( CoreLayer              *layer,
                                         void                   *driver_data,
                                         void                   *layer_data,
                                         int                     level )
{
     return DFB_OK;
}


/**********************************************************************************************************************/

/*const*/ DisplayLayerFuncs mt53VdpLayerFuncs = {
     LayerDataSize:      vdpLayerDataSize,
     RegionDataSize:     vdpRegionDataSize,
     InitLayer:          vdpInitLayer,
     GetLevel:         vdpGetLevel,
     SetLevel:         vdpSetLevel,
     TestRegion:         vdpTestRegion,
     SetRegion:          vdpSetRegion,
     RemoveRegion:       vdpRemoveRegion,
     FlipRegion:         vdpFlipRegion,
     UpdateRegion:         vdpUpdateRegion,     
};

