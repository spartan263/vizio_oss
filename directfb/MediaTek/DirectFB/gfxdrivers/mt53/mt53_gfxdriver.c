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
#include <stdio.h>
#include <sys/mman.h>

#include <directfb.h>

#include <direct/debug.h>
#include <direct/messages.h>
#include <direct/system.h>

#include <core/gfxcard.h>
#include <core/system.h>
#include <core/layers.h>
#include <core/screens.h>

#include <mt53/mt53.h>

#include <gfx_if.h>

#include "mt53_2d.h"
#include "mt53_gfxdriver.h"

#include <core/graphics_driver.h>

DFB_GRAPHICS_DRIVER( mt53gfx )


/**********************************************************************************************************************/

static int
driver_probe( CoreGraphicsDevice *device )
{
     switch (dfb_gfxcard_get_accelerator( device )) {
          case FB_ACCEL_MEDIATEK_53XX:
               return 1;
     }

     return 0;
}

static void
driver_get_info( CoreGraphicsDevice *device,
                 GraphicsDriverInfo *info )
{
     /* fill driver info structure */
     snprintf( info->name,
               DFB_GRAPHICS_DRIVER_INFO_NAME_LENGTH,
               "MediaTek 53xx Driver" );

     snprintf( info->vendor,
               DFB_GRAPHICS_DRIVER_INFO_VENDOR_LENGTH,
               "Denis Oliver Kropp" );

     info->version.major = 0;
     info->version.minor = 3;

     info->driver_data_size = sizeof(MT53DriverData);
     info->device_data_size = sizeof(MT53DeviceData);
}

static DFBResult
driver_init_driver( CoreGraphicsDevice  *device,
                    GraphicsDeviceFuncs *funcs,
                    void                *driver_data,
                    void                *device_data,
                    CoreDFB             *core )
{
     MT53DriverData *mdrv = driver_data;
     MT53           *mt53 = dfb_system_data();
	 int plane_order = 0;

     mdrv->fd = mt53->fd;

     mdrv->num_plane = 0;

     mdrv->shm_cmd_que = mmap( NULL, direct_pagesize(), PROT_READ | PROT_WRITE, MAP_SHARED,
                               mdrv->fd, direct_page_align( mt53->fix.smem_len ) );
     if (mdrv->shm_cmd_que == MAP_FAILED) {
          D_PERROR( "MediaTek/Driver: Mapping shared command queue control structure failed!\n" );
          return DFB_INIT;
     }

     D_INFO( "MediaTek/Driver: Mapped shared command queue control structure to %p\n", mdrv->shm_cmd_que );

     mdrv->cmd_que_buf = mmap( NULL, 256 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED,
                               mdrv->fd, direct_page_align( mt53->fix.smem_len ) + direct_pagesize() );
     if (mdrv->cmd_que_buf == MAP_FAILED) {
          D_PERROR( "MediaTek/Driver: Mapping DMA region failed!\n" );
          munmap( (void*) mdrv->shm_cmd_que, direct_pagesize() );
          return DFB_INIT;
     }

     D_INFO( "MediaTek/Driver: Mapped DMA region to %p\n", mdrv->cmd_que_buf );

     GFX_CmdQueInit( mdrv->shm_cmd_que, &mdrv->cmd_que_buf );

     /* initialize function pointers */
     funcs->EngineSync   = mt53EngineSync;
     funcs->EngineReset  = mt53EngineReset;
     funcs->EmitCommands = mt53EmitCommands;
     funcs->CheckState   = mt53CheckState;
     funcs->SetState     = mt53SetState;

     funcs->FlushTextureCache = mt53FlushTextureCache;
     funcs->FlushReadCache = mt53FlushReadCache;


     /* Register primary screen functions */
     mdrv->screen = dfb_screens_register( device, driver_data, &mt53PrimaryScreenFuncs );

     /* Register primary layer functions */
     mdrv->layer1 = dfb_layers_register( mdrv->screen, driver_data, &mt53PrimaryLayerFuncs );

     mdrv->layer_plane[plane_order++] = MT53_LAYER_OSD1;

#if 1 //enable SUPPORT_2_LAYERS
     /* Register secondary layer functions */
     mdrv->layer2 = dfb_layers_register( mdrv->screen, driver_data, &mt53PrimaryLayerFuncs );
     mdrv->layer_plane[plane_order++] = MT53_LAYER_OSD2;
#endif

#ifdef CC_DFB_SUPPORT_VDP_LAYER
#ifdef CC_B2R44K2K_SUPPORT
     mdrv->layer3 = dfb_layers_register( mdrv->screen, driver_data, &mt53PrimaryLayerFuncs );
#else
     mdrv->layer3 = dfb_layers_register( mdrv->screen, driver_data, &mt53VdpLayerFuncs );
#endif 
     mdrv->layer_plane[plane_order++] = MT53_LAYER_VDP1;
#endif
     mdrv->cursor = dfb_layers_register( mdrv->screen, driver_data, &mt53PrimaryLayerFuncs );
     mdrv->layer_plane[plane_order++] = MT53_LAYER_CURSOR;

     return DFB_OK;
}

static DFBResult
driver_init_device( CoreGraphicsDevice *device,
                    GraphicsDeviceInfo *device_info,
                    void               *driver_data,
                    void               *device_data )
{
     /* fill device info */
     snprintf( device_info->vendor, DFB_GRAPHICS_DEVICE_INFO_VENDOR_LENGTH, "MediaTek" );
     snprintf( device_info->name,   DFB_GRAPHICS_DEVICE_INFO_NAME_LENGTH,   "53xx" );

     /* device limitations */
     device_info->limits.surface_byteoffset_alignment = 64;
     device_info->limits.surface_bytepitch_alignment  = 64;

     device_info->caps.flags    = 0;
     device_info->caps.accel    = MT53_SUPPORTED_DRAWINGFUNCTIONS |
                                  MT53_SUPPORTED_BLITTINGFUNCTIONS;
     device_info->caps.drawing  = MT53_SUPPORTED_DRAWINGFLAGS;
     device_info->caps.blitting = MT53_SUPPORTED_BLITTINGFLAGS;

     return DFB_OK;
}

static void
driver_close_device( CoreGraphicsDevice *device,
                     void               *driver_data,
                     void               *device_data )
{
}

static void
driver_close_driver( CoreGraphicsDevice *device,
                     void               *driver_data )
{
    MT53DriverData *mdrv = driver_data;

    munmap( (void*) mdrv->cmd_que_buf, 256 * 1024 );
    munmap( (void*) mdrv->shm_cmd_que, direct_pagesize() );
}


