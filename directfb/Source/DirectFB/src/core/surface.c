/*
   (c) Copyright 2001-2010  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <config.h>

#include <direct/debug.h>

#include <core/core.h>
#include <core/palette.h>
#include <core/surface.h>

#include <core/system.h>
#include <fusion/shmalloc.h>

#include <core/layers_internal.h>
#include <core/windows_internal.h>

#include <core/CoreDFB.h>
#include <core/CoreSurface.h>

#include <gfx/convert.h>

#ifdef CC_DFB_DEBUG_SUPPORT
#include <execinfo.h>
#endif

D_DEBUG_DOMAIN( Core_Surface, "Core/Surface", "DirectFB Core Surface" );

/**********************************************************************************************************************/

static const ReactionFunc dfb_surface_globals[] = {
/* 0 */   _dfb_layer_region_surface_listener,
/* 1 */   _dfb_windowstack_background_image_listener,
          NULL
};

static void
surface_destructor( FusionObject *object, bool zombie, void *ctx )
{
     int          i;
     CoreSurface *surface = (CoreSurface*) object;

     D_MAGIC_ASSERT( surface, CoreSurface );

     D_DEBUG_AT( Core_Surface, "destroying %p (%dx%d%s)\n", surface,
                 surface->config.size.w, surface->config.size.h, zombie ? " ZOMBIE" : "");

     CoreSurface_Deinit_Dispatch( &surface->call );
     
     dfb_surface_lock( surface );

     surface->state |= CSSF_DESTROYED;

     /* announce surface destruction */
     dfb_surface_notify( surface, CSNF_DESTROY );

     /* unlink palette */
     if (surface->palette) {
          dfb_palette_detach_global( surface->palette, &surface->palette_reaction );
          dfb_palette_unlink( &surface->palette );
     }

     /* destroy buffers */
     for (i=0; i<MAX_SURFACE_BUFFERS; i++) {
          if (surface->buffers[i])
               dfb_surface_buffer_destroy( surface->buffers[i] );
     }

     dfb_system_surface_data_destroy( surface, surface->data );

     /* release the system driver specific surface data */
     if (surface->data) {
          SHFREE( surface->shmpool, surface->data );
          surface->data = NULL;
     }

     direct_serial_deinit( &surface->serial );

     dfb_surface_unlock( surface );

     fusion_skirmish_destroy( &surface->lock );

     D_MAGIC_CLEAR( surface );

     fusion_object_destroy( object );
}

FusionObjectPool *
dfb_surface_pool_create( const FusionWorld *world )
{
     FusionObjectPool *pool;

     pool = fusion_object_pool_create( "Surface Pool",
                                       sizeof(CoreSurface),
                                       sizeof(CoreSurfaceNotification),
                                       surface_destructor, NULL, world );

     return pool;
}

/**********************************************************************************************************************/

DFBResult
dfb_surface_create( CoreDFB                  *core,
                    const CoreSurfaceConfig  *config,
                    CoreSurfaceTypeFlags      type,
                    unsigned long             resource_id,
                    CorePalette              *palette,
                    CoreSurface             **ret_surface )
{
     DFBResult           ret = DFB_BUG;
     int                 i;
     int                 buffers;
     CoreSurface *       surface;
     char                buf[64];
     int                 num_eyes;
     DFBSurfaceStereoEye eye;

     D_ASSERT( core != NULL );
     D_FLAGS_ASSERT( type, CSTF_ALL );
     D_MAGIC_ASSERT_IF( palette, CorePalette );
     D_ASSERT( ret_surface != NULL );

     D_DEBUG_AT( Core_Surface, "dfb_surface_create( %p, %p, %p )\n", core, config, ret_surface );

     surface = dfb_core_create_surface( core );
     if (!surface)
          return DFB_FUSION;

     surface->data = NULL;

#ifdef CC_DFB_DEBUG_SUPPORT
     void *array[20];
     char **strings;

     surface->pid = getpid();
     surface->stack_size = backtrace(array, 20);
     strings = backtrace_symbols(array, surface->stack_size);

     if(surface->stack_size > 0 && strings != NULL)
     {
          for(i = 0; i < surface->stack_size; i++)
          {
               strcpy(surface->stack_string[i], strings[i]);
          }
     }
     else
     {
          surface->stack_size = 0;
     }
#endif
     if (config) {
          D_FLAGS_ASSERT( config->flags, CSCONF_ALL );

          surface->config.flags = config->flags;

          if (config->flags & CSCONF_SIZE) {
               D_DEBUG_AT( Core_Surface, "  -> %dx%d\n", config->size.w, config->size.h );

               surface->config.size = config->size;
          }

          if (config->flags & CSCONF_FORMAT) {
               D_DEBUG_AT( Core_Surface, "  -> %s\n", dfb_pixelformat_name( config->format ) );

               surface->config.format = config->format;
          }

          if (config->flags & CSCONF_COLORSPACE) {
               D_DEBUG_AT( Core_Surface, "  -> %s\n", dfb_colorspace_name( config->colorspace ) );

               surface->config.colorspace = config->colorspace;
          }

          if (config->flags & CSCONF_CAPS) {
               D_DEBUG_AT( Core_Surface, "  -> caps 0x%08x\n", config->caps );

               if (config->caps & DSCAPS_ROTATED)
                    D_UNIMPLEMENTED();

               surface->config.caps = config->caps & ~DSCAPS_ROTATED;
          }

          if (config->flags & CSCONF_PREALLOCATED) {
               D_DEBUG_AT( Core_Surface, "  -> prealloc %p [%d]\n",
                           config->preallocated[0].addr,
                           config->preallocated[0].pitch );

               direct_memcpy( surface->config.preallocated, config->preallocated, sizeof(config->preallocated) );

               type |= CSTF_PREALLOCATED;
          }
     }

     if (surface->config.caps & DSCAPS_SYSTEMONLY)
          surface->type = (type & ~CSTF_EXTERNAL) | CSTF_INTERNAL;
     else if (surface->config.caps & DSCAPS_VIDEOONLY)
          surface->type = (type & ~CSTF_INTERNAL) | CSTF_EXTERNAL;
     else
          surface->type = type & ~(CSTF_INTERNAL | CSTF_EXTERNAL);

     if (surface->config.caps & DSCAPS_SHARED)
          surface->type |= CSTF_SHARED;

     surface->resource_id = resource_id;

     if (surface->config.caps & DSCAPS_TRIPLE)
          buffers = 3;
     else if (surface->config.caps & DSCAPS_DOUBLE)
          buffers = 2;
     else {
          buffers = 1;

          surface->config.caps &= ~DSCAPS_ROTATED;
     }

     surface->notifications = CSNF_ALL & ~CSNF_FLIP;

     surface->alpha_ramp[0] = 0x00;
     surface->alpha_ramp[1] = 0x55;
     surface->alpha_ramp[2] = 0xaa;
     surface->alpha_ramp[3] = 0xff;


     if (surface->config.caps & DSCAPS_STATIC_ALLOC)
          surface->config.min_size = surface->config.size;

     surface->shmpool = dfb_core_shmpool( core );

     direct_serial_init( &surface->serial );

     snprintf( buf, sizeof(buf), "Surface %dx%d %s %s", surface->config.size.w,
               surface->config.size.h, dfb_pixelformat_name(surface->config.format),
               dfb_colorspace_name(surface->config.colorspace) );

     fusion_ref_set_name( &surface->object.ref, buf );

     fusion_skirmish_init( &surface->lock, buf, dfb_core_world(core) );

     fusion_object_set_lock( &surface->object, &surface->lock );

     D_MAGIC_SET( surface, CoreSurface );


     if (dfb_config->warn.flags & DCWF_CREATE_SURFACE &&
         dfb_config->warn.create_surface.min_size.w <= surface->config.size.w &&
         dfb_config->warn.create_surface.min_size.h <= surface->config.size.h)
          D_WARN( "create-surface  %4dx%4d %6s, buffers %d, caps 0x%08x, type 0x%08x",
                  surface->config.size.w, surface->config.size.h, dfb_pixelformat_name(surface->config.format),
                  buffers, surface->config.caps, surface->type );


     if (palette) {
          dfb_surface_set_palette( surface, palette );
     }
     else if (DFB_PIXELFORMAT_IS_INDEXED( surface->config.format )) {
          ret = dfb_surface_init_palette( core, surface );
          if (ret)
               goto error;
     }

     /* Create the system driver specific surface data information */
     int data_size = dfb_system_surface_data_size();

     if (data_size) {
          surface->data = SHCALLOC( surface->shmpool, 1, data_size );
          if (!surface->data) {
              ret = D_OOSHM();
              goto error;
          }
     }

     dfb_system_surface_data_init(surface,surface->data);
#ifdef CC_DFB_SHARE_MEMORY
     if( DFB_SYSTEMONLY && !DSCAPS_FROM_VIDEO(surface->config.caps) && !(SURFACE_TYPE_VIDEO&type))
     {
         //direct_log_printf(NULL,"%s DFB_SYSTEMONLY[pid = %d][0x%x] \n",__FUNCTION__,getpid(),surface->config.caps);
         surface->config.caps &= ~DSCAPS_VIDEOONLY;
         surface->config.caps |= DSCAPS_SYSTEMONLY;
     }
     else
     {
         surface->config.caps &= ~DSCAPS_SYSTEMONLY;
         surface->config.caps |= DSCAPS_VIDEOONLY;
         //direct_log_printf(NULL,"%s [pid = %d][0x%x] \n",__FUNCTION__,getpid(),surface->config.caps);
     }
#endif
     /* Create the Surface Buffers. */
     num_eyes = config->caps & DSCAPS_STEREO ? 2 : 1;
     for (eye=DSSE_LEFT; num_eyes>0; num_eyes--, eye=DSSE_RIGHT) {
          dfb_surface_set_stereo_eye(surface, eye);
          for (i=0; i<buffers; i++) {
               CoreSurfaceBuffer *buffer;
     
               ret = dfb_surface_buffer_new( surface, CSBF_NONE, &buffer );
               if (ret) {
                    D_DERROR( ret, "Core/Surface: Error creating surface buffer!\n" );
                    goto error;
               }
     
               surface->buffers[i] = buffer;
               if (eye == DSSE_LEFT)
                    surface->num_buffers++;
     
               switch (i) {
                    case 0:
                         surface->buffer_indices[CSBR_FRONT] = i;
                    case 1:
                         surface->buffer_indices[CSBR_BACK] = i;
                    case 2:
                         surface->buffer_indices[CSBR_IDLE] = i;
               }
          }
     }
     dfb_surface_set_stereo_eye(surface, DSSE_LEFT);

     CoreSurface_Init_Dispatch( core, surface, &surface->call );

     fusion_object_activate( &surface->object );

     *ret_surface = surface;

     return DFB_OK;

error:
     D_MAGIC_CLEAR( surface );

     num_eyes = config->caps & DSCAPS_STEREO ? 2 : 1;
     for (eye=DSSE_LEFT; num_eyes>0; num_eyes--, eye=DSSE_RIGHT) {
          dfb_surface_set_stereo_eye(surface, eye);
          for (i=0; i<MAX_SURFACE_BUFFERS; i++) {
               if (surface->buffers[i])
                    dfb_surface_buffer_destroy( surface->buffers[i] );
          }
     }
     dfb_surface_set_stereo_eye(surface, DSSE_LEFT);

     /* release the system driver specific surface data */
     if (surface->data) {
         dfb_system_surface_data_destroy( surface, surface->data );
         SHFREE( surface->shmpool, surface->data );
         surface->data = NULL;
     }

     fusion_skirmish_destroy( &surface->lock );

     direct_serial_deinit( &surface->serial );

     fusion_object_destroy( &surface->object );

     return ret;
}

DFBResult
dfb_surface_create_simple ( CoreDFB                 *core,
                            int                      width,
                            int                      height,
                            DFBSurfacePixelFormat    format,
                            DFBSurfaceColorSpace     colorspace,
                            DFBSurfaceCapabilities   caps,
                            CoreSurfaceTypeFlags     type,
                            unsigned long            resource_id,
                            CorePalette             *palette,
                            CoreSurface            **ret_surface )
{
     CoreSurfaceConfig config;

     D_DEBUG_AT( Core_Surface, "%s( %p, %dx%d %s, %p )\n", __FUNCTION__, core, width, height,
                 dfb_pixelformat_name( format ), ret_surface );

     D_ASSERT( core != NULL );
     D_ASSERT( ret_surface != NULL );

     config.flags        = CSCONF_SIZE | CSCONF_FORMAT | CSCONF_COLORSPACE | CSCONF_CAPS;
     config.size.w       = width;
     config.size.h       = height;
     config.format       = format;
     config.colorspace   = colorspace;
     config.caps         = caps;

     //return dfb_surface_create( core, &config, type, resource_id, palette, ret_surface );
     return CoreDFB_CreateSurface( core, &config, type, resource_id, palette, ret_surface );
}

DFBResult
dfb_surface_init_palette( CoreDFB     *core,
                          CoreSurface *surface )
{
     DFBResult    ret;
     CorePalette *palette;

     ret = dfb_palette_create( core,
                               1 << DFB_COLOR_BITS_PER_PIXEL( surface->config.format ),
                               &palette );
     if (ret) {
          D_DERROR( ret, "Core/Surface: Error creating palette!\n" );
          return ret;
     }

     switch (surface->config.format) {
          case DSPF_LUT8:
               dfb_palette_generate_rgb332_map( palette );
               break;

          case DSPF_ALUT44:
               dfb_palette_generate_rgb121_map( palette );
               break;

          default:
               break;
     }

     dfb_surface_set_palette( surface, palette );

     dfb_palette_unref( palette );

     return DFB_OK;
}


DFBResult
dfb_surface_notify( CoreSurface                  *surface,
                    CoreSurfaceNotificationFlags  flags)
{
     CoreSurfaceNotification notification;

     D_MAGIC_ASSERT( surface, CoreSurface );
     FUSION_SKIRMISH_ASSERT( &surface->lock );
     D_FLAGS_ASSERT( flags, CSNF_ALL );

     direct_serial_increase( &surface->serial );

     if (!(surface->state & CSSF_DESTROYED)) {
          if (!(surface->notifications & flags))
               return DFB_OK;
     }

     notification.flags   = flags;
     notification.surface = surface;

     return dfb_surface_dispatch( surface, &notification, dfb_surface_globals );
}

DFBResult
dfb_surface_notify_display( CoreSurface       *surface,
                            CoreSurfaceBuffer *buffer )
{
     CoreSurfaceNotification notification;

     D_MAGIC_ASSERT( surface, CoreSurface );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
     FUSION_SKIRMISH_ASSERT( &surface->lock );

     direct_serial_increase( &surface->serial );

     notification.flags   = CSNF_DISPLAY;
     notification.surface = surface;
     notification.index   = dfb_surface_buffer_index( buffer );

     return dfb_surface_dispatch( surface, &notification, dfb_surface_globals );
}

DFBResult
dfb_surface_flip( CoreSurface *surface, bool swap )
{
     unsigned int back, front;

     D_MAGIC_ASSERT( surface, CoreSurface );

     if (surface->num_buffers == 0)
          return DFB_SUSPENDED;
     
     FUSION_SKIRMISH_ASSERT( &surface->lock );

     back  = (surface->flips + CSBR_BACK)  % surface->num_buffers;
     front = (surface->flips + CSBR_FRONT) % surface->num_buffers;

     D_ASSERT( surface->buffer_indices[back]  < surface->num_buffers );
     D_ASSERT( surface->buffer_indices[front] < surface->num_buffers );

     if (surface->buffers[surface->buffer_indices[back]]->policy !=
         surface->buffers[surface->buffer_indices[front]]->policy || (surface->config.caps & DSCAPS_ROTATED))
          return DFB_UNSUPPORTED;

     if (swap) {
          int tmp = surface->buffer_indices[back];
          surface->buffer_indices[back] = surface->buffer_indices[front];
          surface->buffer_indices[front] = tmp;
     }
     else
          surface->flips++;

     dfb_surface_notify( surface, CSNF_FLIP );

     return DFB_OK;
}

DFBResult
dfb_surface_reconfig( CoreSurface             *surface,
                      const CoreSurfaceConfig *config )
{
     int i, buffers;
     DFBResult ret;
     DFBSurfaceStereoEye eye;
     int num_eyes;
	 int keep_front = 0, keep_back = 0, back_indices = 0;

     D_DEBUG_AT( Core_Surface, "%s( %p, %dx%d %s -> %dx%d %s )\n", __FUNCTION__, surface,
                 surface->config.size.w, surface->config.size.h, dfb_pixelformat_name( surface->config.format ),
                 (config->flags & CSCONF_SIZE) ? config->size.w : surface->config.size.w,
                 (config->flags & CSCONF_SIZE) ? config->size.h : surface->config.size.h,
                 (config->flags & CSCONF_FORMAT) ? dfb_pixelformat_name( config->format ) :
                                                   dfb_pixelformat_name( surface->config.format ) );

     D_MAGIC_ASSERT( surface, CoreSurface );
     D_ASSERT( config != NULL );

     if (surface->type & CSTF_PREALLOCATED)
          return DFB_UNSUPPORTED;

     if (config->flags & CSCONF_PREALLOCATED)
          return DFB_UNSUPPORTED;

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     if (  (config->flags == CSCONF_SIZE ||
          ((config->flags == (CSCONF_SIZE | CSCONF_FORMAT)) && (config->format == surface->config.format)))  &&
         config->size.w <= surface->config.min_size.w &&
         config->size.h <= surface->config.min_size.h)
     {
          surface->config.size = config->size;

          fusion_skirmish_dismiss( &surface->lock );
          return DFB_OK;
     }

	// Check if width, height, and color mode do not change but DSCAPS changes to/from DSCAPS_DOUBLE
	if ((config->flags == surface->config.flags && config->flags == (CSCONF_SIZE | CSCONF_FORMAT | CSCONF_CAPS | CSCONF_COLORSPACE)) &&
		(config->size.w == surface->config.size.w) &&
		(config->size.h == surface->config.size.h) &&
		(config->format == surface->config.format) &&
		(surface->num_buffers > 0) &&
		(((surface->config.caps & DSCAPS_DOUBLE) != 0 && (config->caps & (DSCAPS_VIDEOONLY | DSCAPS_SYSTEMONLY)) != 0) ||
		 ((surface->config.caps & (DSCAPS_VIDEOONLY | DSCAPS_SYSTEMONLY)) != 0 && (config->caps & DSCAPS_DOUBLE) != 0))
	   ) 
	{
		keep_front = 1;

		if((surface->config.caps & DSCAPS_DOUBLE) != 0 && (config->caps & DSCAPS_DOUBLE) != 0)
		{
		     keep_back = 1;
		}
	}

#if 1
     /* Precheck the Surface Buffers. */
     num_eyes = surface->config.caps & DSCAPS_STEREO ? 2 : 1;
     for (eye=DSSE_LEFT; num_eyes>0; num_eyes--, eye=DSSE_RIGHT) {
          dfb_surface_set_stereo_eye(surface, eye);
          for (i=0; i<surface->num_buffers; i++) {
               if (surface->buffers[i]->locked) {
                    fusion_skirmish_dismiss( &surface->lock );
                    return DFB_LOCKED;
               }
          }
     }
     dfb_surface_set_stereo_eye(surface, DSSE_LEFT);
#endif

     /* Destroy the Surface Buffers. */
     num_eyes = surface->config.caps & DSCAPS_STEREO ? 2 : 1;
     for (eye=DSSE_LEFT; num_eyes>0; num_eyes--, eye=DSSE_RIGHT) {
          dfb_surface_set_stereo_eye(surface, eye);
          for (i=0; i<surface->num_buffers; i++) {
	           if (i == 0 && keep_front != 0)
               {
                  continue;
               }	
	           if (i == 1 && keep_back != 0)
               {
                  continue;
               }				   
               dfb_surface_buffer_destroy( surface->buffers[i] );
               surface->buffers[i] = NULL;
          }
     }
     dfb_surface_set_stereo_eye(surface, DSSE_LEFT);

     surface->num_buffers = 0;

     if (keep_back == 0 && keep_front == 1 && (config->caps & DSCAPS_DOUBLE) != 0)
     {
        surface->flips = 0;
     }	 

     if (config->flags & CSCONF_SIZE)
          surface->config.size = config->size;

     if (config->flags & CSCONF_FORMAT)
          surface->config.format = config->format;

     if (config->flags & CSCONF_CAPS) {
          if (config->caps & DSCAPS_ROTATED)
               D_UNIMPLEMENTED();

          surface->config.caps = config->caps & ~DSCAPS_ROTATED;
     }

     if (surface->config.caps & DSCAPS_SYSTEMONLY)
          surface->type = (surface->type & ~CSTF_EXTERNAL) | CSTF_INTERNAL;
     else if (surface->config.caps & DSCAPS_VIDEOONLY)
          surface->type = (surface->type & ~CSTF_INTERNAL) | CSTF_EXTERNAL;
     else
          surface->type = surface->type & ~(CSTF_INTERNAL | CSTF_EXTERNAL);

     if (surface->config.caps & DSCAPS_TRIPLE)
          buffers = 3;
     else if (surface->config.caps & DSCAPS_DOUBLE)
          buffers = 2;
     else {
          buffers = 1;

          surface->config.caps &= ~DSCAPS_ROTATED;
     }

     back_indices = surface->buffer_indices[CSBR_BACK];

     /* Recreate the Surface Buffers. */
     num_eyes = config->caps & DSCAPS_STEREO ? 2 : 1;
     for (eye=DSSE_LEFT; num_eyes>0; num_eyes--, eye=DSSE_RIGHT) {
          dfb_surface_set_stereo_eye(surface, eye);
          for (i=0; i<buffers; i++) {
               CoreSurfaceBuffer *buffer;
               if (i == 0 && keep_front != 0)
               {
                   buffer = surface->buffers[surface->buffer_indices[CSBR_FRONT]];
                   ret = 0;
               }
               else if (i == 1 && keep_back != 0)
               {
                   buffer = surface->buffers[back_indices];
                   ret = 0;
               }
               else
               {
                   ret = dfb_surface_buffer_new( surface, CSBF_NONE, &buffer );
               }
               if (ret) {
                    D_DERROR( ret, "Core/Surface: Error creating surface buffer!\n" );
                    goto error;
               }
     
               surface->buffers[i] = buffer;
               if (eye == DSSE_LEFT)
                    surface->num_buffers++;
     
               switch (i) {
                    case 0:
                         surface->buffer_indices[CSBR_FRONT] = i;
                    case 1:
                         surface->buffer_indices[CSBR_BACK] = i;
                    case 2:
                         surface->buffer_indices[CSBR_IDLE] = i;
               }
          }
     }
     dfb_surface_set_stereo_eye(surface, DSSE_LEFT);

     dfb_surface_notify( surface, CSNF_SIZEFORMAT );

     fusion_skirmish_dismiss( &surface->lock );

     return DFB_OK;

error:
     D_UNIMPLEMENTED();

     fusion_skirmish_dismiss( &surface->lock );

     return ret;
}

DFBResult
dfb_surface_destroy_buffers( CoreSurface *surface )
{
     int i;

     D_MAGIC_ASSERT( surface, CoreSurface );

     if (surface->type & CSTF_PREALLOCATED)
          return DFB_UNSUPPORTED;

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     /* Destroy the Surface Buffers. */
     for (i=0; i<surface->num_buffers; i++) {
          dfb_surface_buffer_destroy( surface->buffers[i] );
          surface->buffers[i] = NULL;
     }

     surface->num_buffers = 0;

     fusion_skirmish_dismiss( &surface->lock );

     return DFB_OK;
}

DFBResult
dfb_surface_lock_buffer( CoreSurface            *surface,
                         CoreSurfaceBufferRole   role,
                         CoreSurfaceAccessorID   accessor,
                         CoreSurfaceAccessFlags  access,
                         CoreSurfaceBufferLock  *ret_lock )
{
     DFBResult          ret;
     CoreSurfaceBuffer *buffer;

     D_MAGIC_ASSERT( surface, CoreSurface );

     if (surface->num_buffers == 0)
          return DFB_SUSPENDED;

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     buffer = dfb_surface_get_buffer( surface, role );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

     ret = dfb_surface_buffer_lock( buffer, accessor, access, ret_lock );

     fusion_skirmish_dismiss( &surface->lock );

     return ret;
}

DFBResult
dfb_surface_unlock_buffer( CoreSurface           *surface,
                           CoreSurfaceBufferLock *lock )
{
     DFBResult ret;

     D_MAGIC_ASSERT( surface, CoreSurface );

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     ret = dfb_surface_buffer_unlock( lock );

     fusion_skirmish_dismiss( &surface->lock );

     return ret;
}

DFBResult
dfb_surface_read_buffer( CoreSurface            *surface,
                         CoreSurfaceBufferRole   role,
                         void                   *destination,
                         int                     pitch,
                         const DFBRectangle     *rect )
{
     DFBResult          ret;
     CoreSurfaceBuffer *buffer;

     D_MAGIC_ASSERT( surface, CoreSurface );
     D_ASSERT( destination != NULL );
     D_ASSERT( pitch > 0 );
     DFB_RECTANGLE_ASSERT_IF( rect );

     if (surface->num_buffers == 0)
          return DFB_SUSPENDED;

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     buffer = dfb_surface_get_buffer( surface, role );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

     ret = dfb_surface_buffer_read( buffer, destination, pitch, rect );

     fusion_skirmish_dismiss( &surface->lock );

     return ret;
}

DFBResult
dfb_surface_write_buffer( CoreSurface            *surface,
                          CoreSurfaceBufferRole   role,
                          const void             *source,
                          int                     pitch,
                          const DFBRectangle     *rect )
{
     DFBResult          ret;
     CoreSurfaceBuffer *buffer;

     D_MAGIC_ASSERT( surface, CoreSurface );
     D_ASSERT( source != NULL );
     D_ASSERT( pitch > 0 );
     DFB_RECTANGLE_ASSERT_IF( rect );

     if (surface->num_buffers == 0)
          return DFB_SUSPENDED;

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     buffer = dfb_surface_get_buffer( surface, role );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

     ret = dfb_surface_buffer_write( buffer, source, pitch, rect );

     fusion_skirmish_dismiss( &surface->lock );

     return ret;
}

DFBResult
dfb_surface_dump_buffer( CoreSurface           *surface,
                         CoreSurfaceBufferRole  role,
                         const char            *path,
                         const char            *prefix )
{
     DFBResult          ret;
     CoreSurfaceBuffer *buffer;

     D_MAGIC_ASSERT( surface, CoreSurface );
     D_ASSERT( path != NULL );
     D_ASSERT( prefix != NULL );

     if (surface->num_buffers == 0)
          return DFB_SUSPENDED;

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     buffer = dfb_surface_get_buffer( surface, role );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

     ret = dfb_surface_buffer_dump( buffer, path, prefix );

     fusion_skirmish_dismiss( &surface->lock );

     return ret;
}

DFBResult
dfb_surface_set_palette( CoreSurface *surface,
                         CorePalette *palette )
{
     D_MAGIC_ASSERT( surface, CoreSurface );
     D_MAGIC_ASSERT_IF( palette, CorePalette );

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     if (surface->palette != palette) {
          if (surface->palette) {
               dfb_palette_detach_global( surface->palette, &surface->palette_reaction );
               dfb_palette_unlink( &surface->palette );
          }

          if (palette) {
               dfb_palette_link( &surface->palette, palette );
               dfb_palette_attach_global( palette, DFB_SURFACE_PALETTE_LISTENER,
                                          surface, &surface->palette_reaction );
          }

          dfb_surface_notify( surface, CSNF_PALETTE_CHANGE );
     }

     fusion_skirmish_dismiss( &surface->lock );

     return DFB_OK;
}

DFBResult
dfb_surface_set_field( CoreSurface *surface,
                       int          field )
{
     D_MAGIC_ASSERT( surface, CoreSurface );

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     surface->field = field;

     dfb_surface_notify( surface, CSNF_FIELD );

     fusion_skirmish_dismiss( &surface->lock );

     return DFB_OK;
}

DFBResult
dfb_surface_set_alpha_ramp( CoreSurface *surface,
                            u8           a0,
                            u8           a1,
                            u8           a2,
                            u8           a3 )
{
     D_MAGIC_ASSERT( surface, CoreSurface );

     if (fusion_skirmish_prevail( &surface->lock ))
          return DFB_FUSION;

     surface->alpha_ramp[0] = a0;
     surface->alpha_ramp[1] = a1;
     surface->alpha_ramp[2] = a2;
     surface->alpha_ramp[3] = a3;

     dfb_surface_notify( surface, CSNF_ALPHA_RAMP );

     fusion_skirmish_dismiss( &surface->lock );

     return DFB_OK;
}

ReactionResult
_dfb_surface_palette_listener( const void *msg_data,
                               void       *ctx )
{
     const CorePaletteNotification *notification = msg_data;
     CoreSurface                   *surface      = ctx;

     if (notification->flags & CPNF_DESTROY)
          return RS_REMOVE;

     if (notification->flags & CPNF_ENTRIES) {
          if (fusion_skirmish_prevail( &surface->lock ))
               return RS_OK;

          dfb_surface_notify( surface, CSNF_PALETTE_UPDATE );

          fusion_skirmish_dismiss( &surface->lock );
     }

     return RS_OK;
}

