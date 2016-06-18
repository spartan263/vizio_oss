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

#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>
#include <direct/system.h>
#include <direct/util.h>

#include <fusion/arena.h>
#include <fusion/fusion.h>
#include <fusion/reactor.h>
#include <fusion/shmalloc.h>

#include <core/core.h>
#include <core/layers.h>
#include <core/gfxcard.h>
#include <core/screens.h>
#include <core/surface_pool.h>

#include <misc/conf.h>
#include <misc/util.h>

#include "mt53.h"


#include <core/core_system.h>

DFB_CORE_SYSTEM( mt53sys )

MT53 *dfb_mt53 = NULL;

#ifdef CC_B2R44K2K_SUPPORT
int _B2RBufNum = 0;
unsigned int _B2RBufPhyAddr[10];
unsigned int _B2RBufVirAddr[10];
unsigned int _B2RBufCPhyAddr[10];
#endif


/**********************************************************************************************************************/

#ifdef CC_B2R44K2K_SUPPORT
static void system_mmap_b2r44K2Kbuf_addr(void)
{
    int n;
	if (ioctl( dfb_mt53->fd, FBIO_B2R44K2K_GETBUFFER, dfb_mt53->m_B2RSysBuf) < 0) {
		 D_PERROR( "DirectFB/MT53: FBIO_VDP_GETBUFFER failed!\n" );
		 return;
	}

	for(n = 0; n < B2R_BUFFER_NUM; n++)
	{
	    dfb_mt53->m_B2RDfbBuf[n].u4PhyAddr = dfb_mt53->m_B2RSysBuf[n].m_YAddr;
		dfb_mt53->m_B2RDfbBuf[n].u4Size = dfb_mt53->m_B2RSysBuf[n].m_Size;
	}

	int offset = direct_page_align( dfb_mt53->fix.smem_len ) + direct_pagesize() + 256 * 1024;
#ifdef CC_OSD_USE_FBM
	offset += dfb_mt53->imagebuf.u4Size;
#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB
	offset += dfb_mt53->imagebuf2.u4Size;
#endif
#endif
#ifdef CC_DFB_SUPPORT_VDP_LAYER
	offset += dfb_mt53->vdpbuf.u4Size;
#endif

	for(n = 0; n < B2R_BUFFER_NUM; n++)
	{
		 offset += (n > 0) ? (dfb_mt53->m_B2RDfbBuf[n - 1].u4Size) : 0;
	
		 D_INFO("##########dfb_mt53->mpegbuf[%d] [0x%x] [%d] \n", n, dfb_mt53->m_B2RDfbBuf[n].u4PhyAddr, dfb_mt53->m_B2RDfbBuf[n].u4Size);
		 
		 dfb_mt53->m_B2RDfbBuf[n].u4VirtAddr = (unsigned int)mmap( 
			 (void*)dfb_mt53->m_B2RDfbBuf[n].u4PhyAddr,
			 dfb_mt53->m_B2RDfbBuf[n].u4Size, 
			 PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, 
			 dfb_mt53->fd, 
			 offset
			 );
	
		 if (dfb_mt53->m_B2RDfbBuf[n].u4VirtAddr == (unsigned int)MAP_FAILED) {
			  D_PERROR( "DirectFB/MT53: Could not mmap the MPEG2 OSD buffer For 4k2k!\n");
			  dfb_mt53->m_B2RDfbBuf[n].u4VirtAddr = 0;
		 }

		_B2RBufVirAddr[n] = dfb_mt53->m_B2RDfbBuf[n].u4VirtAddr;
		_B2RBufPhyAddr[n] = dfb_mt53->m_B2RDfbBuf[n].u4PhyAddr;
		_B2RBufCPhyAddr[n] = dfb_mt53->m_B2RSysBuf[n].m_CAddr;		 
	}

	_B2RBufNum = B2R_BUFFER_NUM;
}
#endif


static void
system_get_info( CoreSystemInfo *info )
{
     info->type = CORE_ANY;
     info->caps = CSCAPS_ACCELERATION;

     snprintf( info->name, DFB_CORE_SYSTEM_INFO_NAME_LENGTH, "MediaTek 53xx" );
}

static DFBResult
system_initialize( CoreDFB *core, void **data )
{
     DFBResult                 ret = DFB_INIT;
     struct fb_var_screeninfo  var;
     FusionSHMPoolShared      *pool;
     CoreScreen               *screen;
     MT53Shared               *shared = NULL;
#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB    
     char str[50];
     char size[50];
#endif
     (void)screen;

     D_ASSERT( dfb_mt53 == NULL );

     pool = dfb_core_shmpool( core );

     dfb_mt53 = (MT53 *)D_CALLOC( 1, sizeof(MT53) );
     if (!dfb_mt53)
          return D_OOM();

     dfb_mt53->fd = -1;

     shared = (MT53Shared*) SHCALLOC( pool, 1, sizeof(MT53Shared) );
     if (!shared) {
          ret = D_OOSHM();
          goto error;
     }

     shared->shmpool = pool;

     fusion_arena_add_shared_field( dfb_core_arena( core ), "mt53", shared );

     dfb_mt53->core   = core;
     dfb_mt53->shared = shared;

     dfb_mt53->fd = direct_try_open( "/dev/fb0", "/dev/fb/0", O_RDWR, true );
     if (dfb_mt53->fd < 0)
          goto error;

     ret = DFB_INIT;

     /* Initialization of mt53fb */
     var.xres           = MT53_PRIMARY_DEFAULT_WIDTH;
     var.yres           = MT53_PRIMARY_DEFAULT_HEIGHT;
     var.xres_virtual   = MT53_PRIMARY_DEFAULT_WIDTH;
     var.yres_virtual   = MT53_PRIMARY_DEFAULT_HEIGHT * 4;
     var.xoffset        = 0;
     var.yoffset        = 0;
     var.bits_per_pixel = 32;
     var.vmode = CM_ARGB8888_DIRECT32;

     if (ioctl( dfb_mt53->fd, FBIO_WAITLOGO, &var ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_WAITLOGO failed!\n" );
          goto error;
     }

     if (ioctl( dfb_mt53->fd, FBIO_INIT, &var ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_INIT failed!\n" );
          goto error;
     }

     if (ioctl( dfb_mt53->fd, FBIOGET_FSCREENINFO, &dfb_mt53->fix ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIOGET_FSCREENINFO failed!\n" );
          goto error;
     }

     if (ioctl( dfb_mt53->fd, FBIO_GETIMAGEBUFFER, &dfb_mt53->imagebuf ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_GETIMAGEBUFFER failed!\n" );
          goto error;
     }

     #ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB 
     if (ioctl( dfb_mt53->fd, FBIO_GETIMAGEBUFFER2, &dfb_mt53->imagebuf2 ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_GETIMAGEBUFFER2 failed!\n" );
          goto error;
     }
    #endif
#ifdef CC_DFB_SUPPORT_VDP_LAYER
     if (ioctl( dfb_mt53->fd, FBIO_VDP_GETBUFFER, &dfb_mt53->vdpbuf ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_VDP_GETBUFFER failed!\n" );
          goto error;
     }
#endif

     /* Map the framebuffer */
     dfb_mt53->mem = mmap( (void*)dfb_mt53->fix.smem_start, dfb_mt53->fix.smem_len, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, dfb_mt53->fd, 0 );

     if (dfb_mt53->mem == MAP_FAILED) {
          D_PERROR( "DirectFB/MT53: Could not mmap the framebuffer!\n");
          dfb_mt53->mem = NULL;
          goto error;
     }
    D_INFO( "DirectFB/MT53_NO CC_DFB_SHARE_MEMORY: DFB[vir=0x%x,phy=0x%x,size=0x%x]\n",
            (int)dfb_mt53->mem,dfb_mt53->fix.smem_start,dfb_mt53->fix.smem_len);	 	 
#ifdef CC_DFB_SHARE_MEMORY	 
    D_INFO( "DirectFB/MT53: DFB[vir=0x%x,phy=0x%x,size=0x%x]\n",
            (int)dfb_mt53->mem,dfb_mt53->fix.smem_start,dfb_mt53->fix.smem_len);	 
	
    // system_mmap_crsr_addr();  
#endif
#ifdef CC_OSD_USE_FBM
     /* Map FBM JPEG_OSD */
     dfb_mt53->imagebuf.u4VirtAddr = (unsigned int)mmap( (void*)dfb_mt53->imagebuf.u4PhyAddr,
         dfb_mt53->imagebuf.u4Size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, dfb_mt53->fd, direct_page_align( dfb_mt53->fix.smem_len ) + direct_pagesize() + 256 * 1024);

     if (dfb_mt53->imagebuf.u4VirtAddr == (unsigned int)MAP_FAILED) {
          D_PERROR( "DirectFB/MT53: Could not mmap the framebuffer!\n");
          dfb_mt53->imagebuf.u4VirtAddr = 0;
          goto error;
     }
#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB 
               /* Map FBM 3DMM */
          dfb_mt53->imagebuf2.u4VirtAddr = (unsigned int)mmap( (void*)dfb_mt53->imagebuf2.u4PhyAddr,
              dfb_mt53->imagebuf2.u4Size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, dfb_mt53->fd, direct_page_align( dfb_mt53->fix.smem_len ) + direct_pagesize() + 256 * 1024 + dfb_mt53->imagebuf.u4Size);
     
          if (dfb_mt53->imagebuf2.u4VirtAddr == (unsigned int)MAP_FAILED) {
               D_PERROR( "DirectFB/MT53: Could not 3DMM the framebuffer!\n");
               dfb_mt53->imagebuf2.u4VirtAddr = 0;
               goto error;
          }
#endif /* CC_FBM_TWO_FBP_SHARED_WITH_DFB */ 

#endif
#ifdef CC_DFB_SUPPORT_VDP_LAYER
     /* Map FBM JPEG VDP */
     dfb_mt53->vdpbuf.u4VirtAddr = (unsigned int)mmap( 
         (void*)dfb_mt53->vdpbuf.u4PhyAddr,
         dfb_mt53->vdpbuf.u4Size, 
         PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, 
         dfb_mt53->fd, 
         direct_page_align( dfb_mt53->fix.smem_len ) + 
         direct_pagesize() + 256 * 1024 + 
         dfb_mt53->imagebuf.u4Size  
         #ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB 
         + dfb_mt53->imagebuf2.u4Size
         #endif
         );

     if (dfb_mt53->vdpbuf.u4VirtAddr == (unsigned int)MAP_FAILED) {
          D_PERROR( "DirectFB/MT53: Could not mmap the JPEG OSD buffer!\n");
          dfb_mt53->vdpbuf.u4VirtAddr = 0;
          goto error;
     }
#endif

#ifdef CC_B2R44K2K_SUPPORT
	 system_mmap_b2r44K2Kbuf_addr();
#endif	 

     dfb_surface_pool_initialize( core, &mt53SurfacePoolFuncs, &dfb_mt53->shared->pool );

     *data = dfb_mt53;
#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB 
     sprintf(str,"%d",dfb_mt53->imagebuf2.u4VirtAddr);
     sprintf(size,"%d",dfb_mt53->imagebuf2.u4Size);  
     printf("======DFB 3DMM u4VirtAddr : 0x%x, and DFB 3DMM u4Size : 0x%x======\n", dfb_mt53->imagebuf2.u4VirtAddr,dfb_mt53->imagebuf2.u4Size);

     if (setenv("DFB_3DMM_VIRTUAL_SIZE", size, 1) == -1)
     {
         D_PERROR( "DirectFB/MT53: Set DFB_3DMM_VIRTUAL_SIZE env error!\n");
         putenv("DFB_3DMM_VIRTUAL_SIZE=0");
     }
     
     if (setenv("DFB_3DMM_VIRTUAL_ADDR", str, 1) == -1)
     {
         D_PERROR( "DirectFB/MT53: Set DFB_3DMM_VIRTUAL_ADDR env error!\n");
         putenv("DFB_3DMM_VIRTUAL_ADDR=0");
     }
#endif
     return DFB_OK;


error:
     if (shared)
          SHFREE( pool, shared );

     if (dfb_mt53->mem)
          munmap( dfb_mt53->mem, dfb_mt53->fix.smem_len );

     if (dfb_mt53->fd >= 0)
          close( dfb_mt53->fd );

     D_FREE( dfb_mt53 );
     dfb_mt53 = NULL;

     return ret;
}

static DFBResult
system_join( CoreDFB *core, void **data )
{
     DFBResult   ret = DFB_INIT;
     struct fb_var_screeninfo  var;
     CoreScreen *screen;
     void       *shared;
     
     D_ASSERT( dfb_mt53 == NULL );

     dfb_mt53 = D_CALLOC( 1, sizeof(MT53) );
     if (!dfb_mt53)
          return D_OOM();

     fusion_arena_get_shared_field( dfb_core_arena( core ), "mt53", &shared );

     dfb_mt53->core   = core;
     dfb_mt53->shared = shared;

     dfb_mt53->fd = direct_try_open( "/dev/fb0", "/dev/fb/0", O_RDWR, true );
     if (dfb_mt53->fd < 0)
          goto error;

     if (ioctl( dfb_mt53->fd, FBIO_WAITLOGO, &var ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_WAITLOGO failed!\n" );
          goto error;
     }

     if (ioctl( dfb_mt53->fd, FBIOGET_FSCREENINFO, &dfb_mt53->fix ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIOGET_FSCREENINFO failed!\n" );
          goto error;
     }

     if (ioctl( dfb_mt53->fd, FBIO_GETIMAGEBUFFER, &dfb_mt53->imagebuf ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_GETIMAGEBUFFER failed!\n" );
          goto error;
     }

     #ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB 
     if (ioctl( dfb_mt53->fd, FBIO_GETIMAGEBUFFER2, &dfb_mt53->imagebuf2 ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_GETIMAGEBUFFER failed!\n" );
          goto error;
     }
    #endif
#ifdef CC_DFB_SUPPORT_VDP_LAYER
     if (ioctl( dfb_mt53->fd, FBIO_VDP_GETBUFFER, &dfb_mt53->vdpbuf ) < 0) {
          D_PERROR( "DirectFB/MT53: FBIO_VDP_GETBUFFER failed!\n" );
          goto error;
     }
#endif


     /* Map the framebuffer */
     dfb_mt53->mem = mmap( (void*)dfb_mt53->fix.smem_start, dfb_mt53->fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, dfb_mt53->fd, 0 );
     if (dfb_mt53->mem == MAP_FAILED) {
          D_PERROR( "DirectFB/MT53: Could not mmap the framebuffer!\n");
          dfb_mt53->mem = NULL;
          goto error;
     }
     
#ifdef CC_OSD_USE_FBM
     /* Map FBM JPEG_OSD */
     dfb_mt53->imagebuf.u4VirtAddr = (unsigned int)mmap( (void*)dfb_mt53->imagebuf.u4PhyAddr,
         dfb_mt53->imagebuf.u4Size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, dfb_mt53->fd, direct_page_align( dfb_mt53->fix.smem_len ) + direct_pagesize() + 256 * 1024);

     if (dfb_mt53->imagebuf.u4VirtAddr == (unsigned int)MAP_FAILED) {
          D_PERROR( "DirectFB/MT53: Could not mmap the JPEG OSD buffer!\n");
          dfb_mt53->imagebuf.u4VirtAddr = 0;
          goto error;
     }
     
#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB 
      /* Map FBM 3DMM */
    
    dfb_mt53->imagebuf2.u4VirtAddr = (unsigned int)mmap( (void*)dfb_mt53->imagebuf2.u4PhyAddr,
        dfb_mt53->imagebuf2.u4Size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, dfb_mt53->fd, direct_page_align( dfb_mt53->fix.smem_len ) + direct_pagesize() + 256 * 1024 + dfb_mt53->imagebuf.u4Size);
 
    if (dfb_mt53->imagebuf2.u4VirtAddr == (unsigned int)MAP_FAILED) {
         D_PERROR( "DirectFB/MT53: Could not mmap the 3DMM buffer!\n");
         dfb_mt53->imagebuf2.u4VirtAddr = 0;
         goto error;
     }
#endif 
#endif
#ifdef CC_DFB_SUPPORT_VDP_LAYER
     /* Map FBM JPEG VDP */

    dfb_mt53->vdpbuf.u4VirtAddr = (unsigned int)mmap( 
                                (void*)dfb_mt53->vdpbuf.u4PhyAddr,
                                dfb_mt53->vdpbuf.u4Size, 
                                PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, 
                                dfb_mt53->fd, 
        direct_page_align( dfb_mt53->fix.smem_len ) + 
        direct_pagesize() + 
        256 * 1024 + 
        dfb_mt53->imagebuf.u4Size 
        #ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB 
        + dfb_mt53->imagebuf2.u4Size
        #endif
                                );

     if (dfb_mt53->vdpbuf.u4VirtAddr == (unsigned int)MAP_FAILED) {
          D_PERROR( "DirectFB/MT53: Could not mmap the JPEG OSD buffer!\n");
          dfb_mt53->vdpbuf.u4VirtAddr = 0;
          goto error;
     }
#endif

#ifdef CC_B2R44K2K_SUPPORT
     system_mmap_b2r44K2Kbuf_addr();
#endif
//#ifdef CC_DFB_SHARE_MEMORY
//     system_mmap_dfb_addr();
//#endif
     dfb_surface_pool_join( core, dfb_mt53->shared->pool, &mt53SurfacePoolFuncs );

     (void)screen;

     *data = dfb_mt53;

     return DFB_OK;
error:
     if (dfb_mt53->mem)
          munmap( dfb_mt53->mem, dfb_mt53->fix.smem_len );

     if (dfb_mt53->fd >= 0)
          close( dfb_mt53->fd );

     D_FREE( dfb_mt53 );
     dfb_mt53 = NULL;

     return ret;
}

static DFBResult
system_shutdown( bool emergency )
{
     MT53Shared          *shared;
     FusionSHMPoolShared *pool;

     D_ASSERT( dfb_mt53 != NULL );

     shared = dfb_mt53->shared;
     D_ASSERT( shared != NULL );

     pool = shared->shmpool;
     D_ASSERT( pool != NULL );

     dfb_surface_pool_destroy( shared->pool );

     SHFREE( pool, shared );


     munmap( dfb_mt53->mem, dfb_mt53->fix.smem_len );

     close( dfb_mt53->fd );

     D_FREE( dfb_mt53 );
     dfb_mt53 = NULL;

     return DFB_OK;
}

static DFBResult
system_leave( bool emergency )
{
     D_ASSERT( dfb_mt53 != NULL );

     munmap( dfb_mt53->mem, dfb_mt53->fix.smem_len );

     close( dfb_mt53->fd );

     D_FREE( dfb_mt53 );
     dfb_mt53 = NULL;

     return DFB_OK;
}

static DFBResult
system_suspend()
{
     return DFB_OK;
}

static DFBResult
system_resume()
{
     return DFB_OK;
}

/******************************************************************************/

static volatile void *
system_map_mmio( unsigned int    offset,
                 int             length )
{
     return NULL;
}

static void
system_unmap_mmio( volatile void  *addr,
                   int             length )
{
}

int system_get_accelerator()
{
     return FB_ACCEL_MEDIATEK_53XX;
}

static VideoMode *
system_get_modes()
{
     return NULL;
}

static VideoMode *
system_get_current_mode()
{
     return NULL;
}

static DFBResult
system_thread_init()
{
     return DFB_OK;
}

static bool
system_input_filter( CoreInputDevice *device,
                     DFBInputEvent   *event )
{
     return false;
}

unsigned long system_video_memory_physical( unsigned int offset )
{
     return dfb_mt53->fix.smem_start + offset;
}

void* system_video_memory_virtual( unsigned int offset )
{
     return dfb_mt53->mem + offset;
}

unsigned int system_videoram_length()
{
     return dfb_mt53->fix.smem_len;
}

unsigned long system_aux_memory_physical( unsigned int offset )
{
#ifdef CC_OSD_USE_FBM
     return dfb_mt53->imagebuf.u4PhyAddr;
#else
     return NULL;
#endif
}

void *system_aux_memory_virtual( unsigned int offset )
{
#ifdef CC_OSD_USE_FBM
     return (void*)dfb_mt53->imagebuf.u4VirtAddr;
#else
     return NULL;
#endif
}

unsigned int system_auxram_length()
{
#ifdef CC_OSD_USE_FBM
     return dfb_mt53->imagebuf.u4Size;
#else
     return 0;
#endif
}

void
system_get_busid( int *ret_bus, int *ret_dev, int *ret_func )
{
}

void
system_get_deviceid( unsigned int *ret_vendor_id,
                     unsigned int *ret_device_id )
{
}

int system_surface_data_size( void )
{
    return 0x0;
}

void system_surface_data_init( CoreSurface *surface, void *data )
{
    return;
}

void system_surface_data_destroy( CoreSurface *surface, void *data )
{
    return;
}
int system_mmu_pin_mem( DFB_IOMMU_PIN_RANGE_T *pt_page )
{
    if(ioctl( dfb_mt53->fd, FBIO_MMU_PINMEM, pt_page ) < 0)
    {
        direct_log_printf(NULL, "mmu_pin_mem Fail![0x%x,0x%x]\n",pt_page->start,pt_page->size);
    }

    return 0x0;
}

int system_mmu_unpin_mem( DFB_IOMMU_PIN_RANGE_T *pt_page )
{
    if(ioctl( dfb_mt53->fd, FBIO_MMU_UNPINMEM, pt_page ) < 0)
    {
        direct_log_printf(NULL, "mmu_unpin_mem Fail![0x%x,0x%x]\n",pt_page->start,pt_page->size);
    }

    return 0x0;
}

void system_hide_logo(void)
{
#ifdef LONG_TIME_LOADER_LOGO
    if(!dfb_mt53)
    {
        return;
    }

    if(ioctl(dfb_mt53->fd, FBIO_HIDE_LOGO) < 0) 
    {
        D_PERROR( "%s[%d] failed!\n",__FUNCTION__,__LINE__);
    }
#endif
     return;
}
#ifdef CC_DFB_SHARE_MEMORY
#if 0
void system_mmap_dfb_addr(void)
{
    if(ioctl( dfb_mt53->fd, FBIOGET_FSCREENINFO, &dfb_mt53->fix ) < 0) 
    {
        D_PERROR( "DirectFB/MT53: FBIOGET_FSCREENINFO failed!\n" );
        return ;
    }
    
    dfb_mt53->mem = mmap( (void*)dfb_mt53->fix.smem_start, 
                                dfb_mt53->fix.smem_len, 
                                PROT_READ | PROT_WRITE, 
                                MAP_SHARED | MAP_FIXED, 
                                dfb_mt53->fd, 
                                0x0);

    if(dfb_mt53->mem == MAP_FAILED) 
    {
        D_PERROR( "DirectFB/MT53: Could not mmap the DFB  buffer!\n");
    }
    else
    {
        D_INFO( "DirectFB/MT53: DFB[vir=0x%x,phy=0x%x,size=0x%x]\n",
            (int)dfb_mt53->mem,dfb_mt53->fix.smem_start,dfb_mt53->fix.smem_len);
    }
    
    return ;
}
#endif
void system_mmap_crsr_addr(void)
{
    do
    {
        if( dfb_mt53 == NULL )
        {
             break;
        }
        
        if (ioctl( dfb_mt53->fd, FBIO_CRSR_INFO, &dfb_mt53->t_crsr ) < 0) 
        {
            D_PERROR( "DirectFB/MT53: FBIO_CRSR_INFO failed!\n" );
            break;
        }
        
        dfb_mt53->t_crsr.u4VirtAddr = (unsigned int)mmap( 
                             (void*)dfb_mt53->t_crsr.u4PhyAddr, 
                             dfb_mt53->t_crsr.u4Size, 
                             PROT_READ | PROT_WRITE, 
                             MAP_SHARED | MAP_FIXED, 
                             dfb_mt53->fd, 
                             dfb_mt53->t_crsr.u4PhyAddr
                             );
        if(dfb_mt53->t_crsr.u4VirtAddr == (unsigned int)MAP_FAILED) 
        {
            D_PERROR( "DirectFB/MT53: Could not mmap the t_crsr  buffer!\n");
            break;
        }
        else
        {
            D_INFO( "DirectFB/MT53: t_crsr[vir=0x%x,phy=0x%x,size=0x%x]\n",
            dfb_mt53->t_crsr.u4VirtAddr,dfb_mt53->t_crsr.u4PhyAddr,dfb_mt53->t_crsr.u4Size);
        }

        return ;
    }while(0);

    return ;
}


void* system_get_crsr_addr(void)
{
    if( dfb_mt53 == NULL )
    {
         return 0x0;
    }
    
    return (void*)dfb_mt53->t_crsr.u4VirtAddr;
}


unsigned int system_get_crsr_phy(void)
{
    if( dfb_mt53 == NULL )
    {
         return 0x0;
    }
    
    return dfb_mt53->t_crsr.u4PhyAddr;
}

unsigned int system_get_crsr_size(void)
{
    if( dfb_mt53 == NULL )
    {
         return 0x0;
    }
    
    return dfb_mt53->t_crsr.u4Size;
}
#endif
