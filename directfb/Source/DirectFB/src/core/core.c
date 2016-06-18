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

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include <pthread.h>

#include <fusion/fusion.h>
#include <fusion/arena.h>
#include <direct/list.h>
#include <fusion/shmalloc.h>
#include <fusion/shm/shm_internal.h>

#include <directfb.h>

#include <core/coredefs.h>
#include <core/coretypes.h>

#include <core/core.h>
#include <core/core_parts.h>
#include <core/fonts.h>
#include <core/graphics_state.h>
#include <core/layer_context.h>
#include <core/layer_region.h>
#include <core/palette.h>
#include <core/surface.h>
#include <core/system.h>
#include <core/windows.h>
#include <core/windows_internal.h>

#include <core/CoreDFB.h>

#include <direct/build.h>
#include <direct/debug.h>
#include <direct/direct.h>
#include <direct/interface.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>
#include <direct/signals.h>
#include <direct/thread.h>
#include <direct/util.h>

#include <fusion/build.h>
#include <fusion/conf.h>

#include <misc/conf.h>
#include <misc/util.h>

#if defined(DFB_DYNAMIC_LINKING) && defined(SOPATH)
#include <dlfcn.h>
#endif

D_DEBUG_DOMAIN( DirectFB_Core, "DirectFB/Core", "DirectFB Core" );

/******************************************************************************/

extern CorePart dfb_clipboard_core;
extern CorePart dfb_colorhash_core;
extern CorePart dfb_graphics_core;
extern CorePart dfb_input_core;
extern CorePart dfb_layer_core;
extern CorePart dfb_screen_core;
extern CorePart dfb_surface_core;
extern CorePart dfb_system_core;
extern CorePart dfb_wm_core;

static CorePart *core_parts[] = {
     &dfb_clipboard_core,
     &dfb_colorhash_core,
     &dfb_surface_core,
     &dfb_system_core,
     &dfb_input_core,
     &dfb_graphics_core,
     &dfb_screen_core,
     &dfb_layer_core,
     &dfb_wm_core
};

void *
dfb_core_get_part( CoreDFB        *core,
                   DFBCorePartID   part_id )
{
     switch (part_id) {
          case DFCP_CLIPBOARD:
               return dfb_clipboard_core.data_local;

          case DFCP_COLORHASH:
               return dfb_colorhash_core.data_local;

          case DFCP_GRAPHICS:
               return dfb_graphics_core.data_local;

          case DFCP_INPUT:
               return dfb_input_core.data_local;

          case DFCP_LAYER:
               return dfb_layer_core.data_local;

          case DFCP_SCREEN:
               return dfb_screen_core.data_local;

          case DFCP_SURFACE:
               return dfb_surface_core.data_local;

          case DFCP_SYSTEM:
               return dfb_system_core.data_local;

          case DFCP_WM:
               return dfb_wm_core.data_local;

          default:
               D_BUG( "unknown core part" );
     }

     return NULL;
}

/******************************************************************************/

/*
 * one entry in the cleanup stack
 */
struct _CoreCleanup {
     DirectLink       link;

     CoreCleanupFunc  func;        /* the cleanup function to be called */
     void            *data;        /* context of the cleanup function */
     bool             emergency;   /* if true, cleanup is also done during
                                      emergency shutdown (from signal hadler) */
};

/******************************************************************************/

/*
 * ckecks if stack is clean, otherwise prints warning, then calls core_deinit()
 */
static void dfb_core_deinit_check( void *ctx );

static void dfb_core_thread_init_handler( DirectThread *thread, void *arg );

static void dfb_core_process_cleanups( CoreDFB *core, bool emergency );

static DirectSignalHandlerResult dfb_core_signal_handler( int   num,
                                                          void *addr,
                                                          void *ctx );

/******************************************************************************/

static int dfb_core_arena_initialize( FusionArena *arena,
                                      void        *ctx );
static int dfb_core_arena_shutdown  ( FusionArena *arena,
                                      void        *ctx,
                                      bool         emergency );
static int dfb_core_arena_join      ( FusionArena *arena,
                                      void        *ctx );
static int dfb_core_arena_leave     ( FusionArena *arena,
                                      void        *ctx,
                                      bool         emergency );

/******************************************************************************/

#if defined(DFB_DYNAMIC_LINKING) && defined(SOPATH)
/*
 * the library handle for dlopen'ing ourselves
 */
static void* dfb_lib_handle = NULL;
#endif

/******************************************************************************/

CoreDFB         *core_dfb      = NULL;
static pthread_mutex_t  core_dfb_lock = PTHREAD_MUTEX_INITIALIZER;

/******************************************************************************/

DFBResult
dfb_core_create( CoreDFB **ret_core )
{
     int      ret;
#if FUSION_BUILD_MULTI
     char     buf[16];
#endif
     CoreDFB       *core   = NULL;
     CoreDFBShared *shared = NULL;

     D_ASSERT( ret_core != NULL );
     D_ASSERT( dfb_config != NULL );

     D_DEBUG_AT( DirectFB_Core, "%s...\n", __FUNCTION__ );

     pthread_mutex_lock( &core_dfb_lock );

     D_ASSERT( core_dfb == NULL || core_dfb->refs > 0 );

     if (core_dfb) {
          D_MAGIC_ASSERT( core_dfb, CoreDFB );

          core_dfb->refs++;

          *ret_core = core_dfb;

          pthread_mutex_unlock( &core_dfb_lock );

          return DFB_OK;
     }

     direct_initialize();


     D_INFO( "DirectFB/Core: %s Application Core. ("BUILDTIME") %s%s\n",
             FUSION_BUILD_MULTI ? "Multi" : "Single",
             DIRECT_BUILD_DEBUG ? "[ DEBUG ]" : "",
             DIRECT_BUILD_TRACE ? "[ TRACE ]" : "" );


#if defined(DFB_DYNAMIC_LINKING) && defined(SOPATH)
     if (!dfb_lib_handle)
#ifdef RTLD_GLOBAL
          dfb_lib_handle = dlopen(SOPATH, RTLD_GLOBAL|RTLD_LAZY);
#else
          /* RTLD_GLOBAL is not defined on OpenBSD */
          dfb_lib_handle = dlopen(SOPATH, RTLD_LAZY);
#endif
#endif

     ret = dfb_system_lookup();
     if (ret)
          goto error;


     /* Allocate local core structure. */
     core = D_CALLOC( 1, sizeof(CoreDFB) );
     if (!core) {
          ret = D_OOM();
          goto error;
     }

     core->refs = 1;

     core->init_handler = direct_thread_add_init_handler( dfb_core_thread_init_handler, core );

#if FUSION_BUILD_MULTI
     dfb_system_thread_init();
#endif

     direct_find_best_memcpy();

     D_MAGIC_SET( core, CoreDFB );

     core_dfb = core;

     ret = fusion_enter( dfb_config->session, DIRECTFB_CORE_ABI, FER_ANY, &core->world );
     if (ret)
          goto error;

     core->fusion_id = fusion_id( core->world );

#if FUSION_BUILD_MULTI
     D_DEBUG_AT( DirectFB_Core, "world %d, fusion id %d\n", fusion_world_index(core->world), core->fusion_id );

     snprintf( buf, sizeof(buf), "%d", fusion_world_index(core->world) );

     setenv( "DIRECTFB_SESSION", buf, true );
#endif

     if (dfb_config->sync) {
          D_INFO( "DirectFB/Core: calling sync()...\n" );
          direct_sync();
     }

     if (dfb_config->core_sighandler)
          direct_signal_handler_add( DIRECT_SIGNAL_ANY, dfb_core_signal_handler, core, &core->signal_handler );

     if (fusion_arena_enter( core->world, "DirectFB/Core",
                             dfb_core_arena_initialize, dfb_core_arena_join,
                             core, &core->arena, &ret ) || ret)
     {
          ret = ret ? ret : DFB_FUSION;
          goto error;
     }

     shared = core->shared;
     D_MAGIC_ASSERT( shared, CoreDFBShared );

     if (dfb_config->block_all_signals)
          direct_signals_block_all();

     if (dfb_config->deinit_check)
          direct_cleanup_handler_add( dfb_core_deinit_check, NULL, &core->cleanup_handler );


     fusion_skirmish_prevail( &shared->lock );

     if (!core->master) {
          while (!shared->active)
               fusion_skirmish_wait( &shared->lock, 0 );
     }

     fusion_skirmish_dismiss( &shared->lock );

     dfb_font_manager_create( core, &core->font_manager );

     *ret_core = core;

     pthread_mutex_unlock( &core_dfb_lock );

     D_DEBUG_AT( DirectFB_Core, "Core successfully created.\n" );

     return DFB_OK;


error:
     if (core) {
          if (core->world)
               fusion_exit( core->world, false );

          if (core->init_handler)
               direct_thread_remove_init_handler( core->init_handler );

          if (core->signal_handler)
               direct_signal_handler_remove( core->signal_handler );

          D_MAGIC_CLEAR( core );

          D_FREE( core );
          core_dfb = NULL;
     }

     pthread_mutex_unlock( &core_dfb_lock );

     direct_shutdown();

     return ret;
}

DFBResult
dfb_core_destroy( CoreDFB *core, bool emergency )
{
     D_MAGIC_ASSERT( core, CoreDFB );
     D_ASSERT( core->refs > 0 );
     D_ASSERT( core == core_dfb );

     D_DEBUG_AT( DirectFB_Core, "%s...\n", __FUNCTION__ );

     if (!emergency) {
          pthread_mutex_lock( &core_dfb_lock );

          if (--core->refs) {
               pthread_mutex_unlock( &core_dfb_lock );
               return DFB_OK;
          }
     }

     dfb_font_manager_destroy( core->font_manager );

     if (core->signal_handler)
          direct_signal_handler_remove( core->signal_handler );
     
     if (core->cleanup_handler)
          direct_cleanup_handler_remove( core->cleanup_handler );

     if (core->master) {
          if (emergency) {
               fusion_kill( core->world, 0, SIGKILL, 1000 );
          }
          else {
               fusion_kill( core->world, 0, SIGTERM, 5000 );
               fusion_kill( core->world, 0, SIGKILL, 2000 );
          }
     }

     dfb_core_process_cleanups( core, emergency );

     while (fusion_arena_exit( core->arena, dfb_core_arena_shutdown,
                               core->master ? NULL : dfb_core_arena_leave,
                               core, emergency, NULL ) == DR_BUSY)
     {
          D_ONCE( "waiting for DirectFB slaves to terminate" );
          direct_thread_sleep( 100000 );
     }

     fusion_exit( core->world, emergency );

     if (!emergency)
          direct_thread_remove_init_handler( core->init_handler );

     D_MAGIC_CLEAR( core );

     D_FREE( core );
     core_dfb = NULL;

     if (!emergency) {
          pthread_mutex_unlock( &core_dfb_lock );

          direct_shutdown();
     }

     return DFB_OK;
}

CoreGraphicsState *
dfb_core_create_graphics_state( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->graphics_state_pool != NULL );

     return (CoreGraphicsState*) fusion_object_create( core->shared->graphics_state_pool, core->world );
}

CoreLayerContext *
dfb_core_create_layer_context( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( shared->layer_context_pool != NULL );

     return (CoreLayerContext*) fusion_object_create( shared->layer_context_pool, core->world );
}

CoreLayerRegion *
dfb_core_create_layer_region( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->layer_region_pool != NULL );

     return (CoreLayerRegion*) fusion_object_create( core->shared->layer_region_pool, core->world );
}

CorePalette *
dfb_core_create_palette( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->palette_pool != NULL );

     return (CorePalette*) fusion_object_create( core->shared->palette_pool, core->world );
}

CoreSurface *
dfb_core_create_surface( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_pool != NULL );

     return (CoreSurface*) fusion_object_create( core->shared->surface_pool, core->world );
}

CoreWindow *
dfb_core_create_window( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->window_pool != NULL );

     return (CoreWindow*) fusion_object_create( core->shared->window_pool, core->world );
}

DFBResult
dfb_core_get_graphics_state( CoreDFB            *core,
                             u32                 object_id,
                             CoreGraphicsState **ret_state )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_state != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->graphics_state_pool != NULL );

     ret = fusion_object_get( core->shared->graphics_state_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_state = (CoreGraphicsState*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_layer_context( CoreDFB           *core,
                            u32                object_id,
                            CoreLayerContext **ret_context )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_context != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->layer_context_pool != NULL );

     ret = fusion_object_get( core->shared->layer_context_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_context = (CoreLayerContext*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_layer_region( CoreDFB          *core,
                           u32               object_id,
                           CoreLayerRegion **ret_region )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_region != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->layer_region_pool != NULL );

     ret = fusion_object_get( core->shared->layer_region_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_region = (CoreLayerRegion*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_palette( CoreDFB      *core,
                      u32           object_id,
                      CorePalette **ret_palette )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_palette != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->palette_pool != NULL );

     ret = fusion_object_get( core->shared->palette_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_palette = (CorePalette*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_surface( CoreDFB      *core,
                      u32           object_id,
                      CoreSurface **ret_surface )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_surface != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_pool != NULL );

     ret = fusion_object_get( core->shared->surface_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_surface = (CoreSurface*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_window( CoreDFB     *core,
                     u32          object_id,
                     CoreWindow **ret_window )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_window != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->window_pool != NULL );

     ret = fusion_object_get( core->shared->window_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_window = (CoreWindow*) object;

     return DFB_OK;
}

DirectResult
dfb_core_enum_surfaces( CoreDFB               *core,
                        FusionObjectCallback   callback,
                        void                  *ctx )
{
     CoreDFBShared *shared;

     D_ASSERT( core != NULL || core_dfb != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return fusion_object_pool_enum( shared->surface_pool, callback, ctx );
}

DirectResult
dfb_core_enum_layer_contexts( CoreDFB               *core,
                              FusionObjectCallback   callback,
                              void                  *ctx )
{
     CoreDFBShared *shared;

     D_ASSERT( core != NULL || core_dfb != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return fusion_object_pool_enum( shared->layer_context_pool, callback, ctx );
}

DirectResult
dfb_core_enum_layer_regions( CoreDFB               *core,
                             FusionObjectCallback   callback,
                             void                  *ctx )
{
     CoreDFBShared *shared;

     D_ASSERT( core != NULL || core_dfb != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return fusion_object_pool_enum( shared->layer_region_pool, callback, ctx );
}

bool
dfb_core_is_master( CoreDFB *core )
{
     D_MAGIC_ASSERT( core, CoreDFB );

     return core->master;
}

void
dfb_core_activate( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;
     D_MAGIC_ASSERT( shared, CoreDFBShared );

     fusion_skirmish_prevail( &shared->lock );

     shared->active = true;

     fusion_skirmish_notify( &shared->lock );

     fusion_skirmish_dismiss( &shared->lock );
}

bool
dfb_core_active( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;
     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return shared->active;
}

FusionWorld *
dfb_core_world( CoreDFB *core )
{
//     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     return core->world;
}

FusionArena *
dfb_core_arena( CoreDFB *core )
{
     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     return core->arena;
}

FusionSHMPoolShared *
dfb_core_shmpool( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return shared->shmpool;
}

FusionSHMPoolShared *
dfb_core_shmpool_data( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return shared->shmpool_data;
}

DFBResult
dfb_core_suspend( CoreDFB *core )
{
     DFBResult ret;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     if (!core->master)
          return DFB_ACCESSDENIED;

     if (core->suspended)
          return DFB_BUSY;

     ret = dfb_input_core.Suspend( dfb_input_core.data_local );
     if (ret)
          goto error_input;

     ret = dfb_layer_core.Suspend( dfb_layer_core.data_local );
     if (ret)
          goto error_layers;

     ret = dfb_screen_core.Suspend( dfb_screen_core.data_local );
     if (ret)
          goto error_screens;

     ret = dfb_graphics_core.Suspend( dfb_graphics_core.data_local );
     if (ret)
          goto error_graphics;

     core->suspended = true;

     return DFB_OK;

error_graphics:
     dfb_screen_core.Resume( dfb_screen_core.data_local );
error_screens:
     dfb_layer_core.Resume( dfb_layer_core.data_local );
error_layers:
     dfb_input_core.Resume( dfb_input_core.data_local );
error_input:
     return ret;
}

DFBResult
dfb_core_resume( CoreDFB *core )
{
     DFBResult ret;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     if (!core->master)
          return DFB_ACCESSDENIED;

     if (!core->suspended)
          return DFB_BUSY;

     ret = dfb_graphics_core.Resume( dfb_graphics_core.data_local );
     if (ret)
          goto error_graphics;

     ret = dfb_screen_core.Resume( dfb_screen_core.data_local );
     if (ret)
          goto error_screens;

     ret = dfb_layer_core.Resume( dfb_layer_core.data_local );
     if (ret)
          goto error_layers;

     ret = dfb_input_core.Resume( dfb_input_core.data_local );
     if (ret)
          goto error_input;

     core->suspended = false;

     return DFB_OK;

error_input:
     dfb_layer_core.Suspend( dfb_layer_core.data_local );
error_layers:
     dfb_screen_core.Suspend( dfb_screen_core.data_local );
error_screens:
     dfb_graphics_core.Suspend( dfb_graphics_core.data_local );
error_graphics:
     return ret;
}

CoreCleanup *
dfb_core_cleanup_add( CoreDFB         *core,
                      CoreCleanupFunc  func,
                      void            *data,
                      bool             emergency )
{
     CoreCleanup *cleanup;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     cleanup = D_CALLOC( 1, sizeof(CoreCleanup) );

     cleanup->func      = func;
     cleanup->data      = data;
     cleanup->emergency = emergency;

     direct_list_prepend( &core->cleanups, &cleanup->link );

     return cleanup;
}

void
dfb_core_cleanup_remove( CoreDFB     *core,
                         CoreCleanup *cleanup )
{
     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     direct_list_remove( &core->cleanups, &cleanup->link );

     D_FREE( cleanup );
}

DFBFontManager *
dfb_core_font_manager( CoreDFB *core )
{
     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     return core->font_manager;
}

/******************************************************************************/

static void
dfb_core_deinit_check( void *ctx )
{
     if (core_dfb && core_dfb->refs) {
          D_WARN( "Application exited without deinitialization of DirectFB!" );
          dfb_core_destroy( core_dfb, true );
     }
}

static void
dfb_core_thread_init_handler( DirectThread *thread, void *arg )
{
     dfb_system_thread_init();
}

static void
dfb_core_process_cleanups( CoreDFB *core, bool emergency )
{
     D_MAGIC_ASSERT( core, CoreDFB );

     while (core->cleanups) {
          CoreCleanup *cleanup = (CoreCleanup*) core->cleanups;

          core->cleanups = core->cleanups->next;

          if (cleanup->emergency || !emergency)
               cleanup->func( cleanup->data, emergency );

          D_FREE( cleanup );
     }
}

static DirectSignalHandlerResult
dfb_core_signal_handler( int   num,
                         void *addr,
                         void *ctx )
{
     CoreDFB *core = ctx;

     D_ASSERT( core == core_dfb );

     dfb_core_destroy( core, true );

     return DSHR_OK;
}

/******************************************************************************/

static int
dfb_core_shutdown( CoreDFB *core, bool emergency )
{
     CoreDFBShared *shared;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     /* Suspend input core to stop all input threads before shutting down. */
     if (dfb_input_core.initialized)
          dfb_input_core.Suspend( dfb_input_core.data_local );

     /* Destroy window objects. */
     fusion_object_pool_destroy( shared->window_pool, core->world );

     /* Close window stacks. */
     if (dfb_wm_core.initialized)
          dfb_wm_close_all_stacks( dfb_wm_core.data_local );

     fusion_stop_dispatcher( core->world, emergency );

     /* Destroy layer context and region objects. */
     fusion_object_pool_destroy( shared->layer_region_pool, core->world );
     fusion_object_pool_destroy( shared->layer_context_pool, core->world );

     /* Shutdown WM core. */
     dfb_core_part_shutdown( core, &dfb_wm_core, emergency );

     /* Shutdown layer core. */
     dfb_core_part_shutdown( core, &dfb_layer_core, emergency );
     dfb_core_part_shutdown( core, &dfb_screen_core, emergency );

     /* Destroy surface and palette objects. */
     fusion_object_pool_destroy( shared->graphics_state_pool, core->world );
     fusion_object_pool_destroy( shared->surface_pool, core->world );
     fusion_object_pool_destroy( shared->palette_pool, core->world );

     /* Destroy remaining core parts. */
     dfb_core_part_shutdown( core, &dfb_graphics_core, emergency );
     dfb_core_part_shutdown( core, &dfb_surface_core, emergency );
     dfb_core_part_shutdown( core, &dfb_input_core, emergency );
     dfb_core_part_shutdown( core, &dfb_system_core, emergency );
     dfb_core_part_shutdown( core, &dfb_colorhash_core, emergency );
     dfb_core_part_shutdown( core, &dfb_clipboard_core, emergency );

     /* Destroy shared memory pool for surface data. */
     fusion_shm_pool_destroy( core->world, shared->shmpool_data );

     return 0;
}

static DFBResult
dfb_core_initialize( CoreDFB *core )
{
     int            i;
     DFBResult      ret;
     CoreDFBShared *shared;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     ret = fusion_shm_pool_create( core->world, "DirectFB Data Pool", 0x1000000,
                                   fusion_config->debugshm, &shared->shmpool_data );
     if (ret)
          return ret;

     shared->graphics_state_pool = dfb_graphics_state_pool_create( core->world );
     shared->layer_context_pool  = dfb_layer_context_pool_create( core->world );
     shared->layer_region_pool   = dfb_layer_region_pool_create( core->world );
     shared->palette_pool        = dfb_palette_pool_create( core->world );
     shared->surface_pool        = dfb_surface_pool_create( core->world );
     shared->window_pool         = dfb_window_pool_create( core->world );

     for (i=0; i<D_ARRAY_SIZE(core_parts); i++) {
          DFBResult ret;

          if ((ret = dfb_core_part_initialize( core, core_parts[i] ))) {
               dfb_core_shutdown( core, true );
               return ret;
          }
     }

     return DFB_OK;
}

static int
dfb_core_leave( CoreDFB *core, bool emergency )
{
     int i;

     D_MAGIC_ASSERT( core, CoreDFB );

     for (i=D_ARRAY_SIZE(core_parts)-1; i>=0; i--)
          dfb_core_part_leave( core, core_parts[i], emergency );

     return DFB_OK;
}

static int
dfb_core_join( CoreDFB *core )
{
     int i;

     D_MAGIC_ASSERT( core, CoreDFB );

     for (i=0; i<D_ARRAY_SIZE(core_parts); i++) {
          DFBResult ret;

          if ((ret = dfb_core_part_join( core, core_parts[i] ))) {
               dfb_core_leave( core, true );
               return ret;
          }
     }

     return DFB_OK;
}

/******************************************************************************/

static int
dfb_core_arena_initialize( FusionArena *arena,
                           void        *ctx )
{
     DFBResult            ret;
     CoreDFB             *core = ctx;
     CoreDFBShared       *shared;
     FusionSHMPoolShared *pool;

     D_MAGIC_ASSERT( core, CoreDFB );

     D_DEBUG_AT( DirectFB_Core, "Initializing...\n" );

     /* Create the shared memory pool first! */
     ret = fusion_shm_pool_create( core->world, "DirectFB Main Pool", 0x400000,
                                   fusion_config->debugshm, &pool );
     if (ret)
          return ret;

     /* Allocate shared structure in the new pool. */
     shared = SHCALLOC( pool, 1, sizeof(CoreDFBShared) );
     if (!shared) {
          fusion_shm_pool_destroy( core->world, pool );
          return D_OOSHM();
     }

     core->shared = shared;
     core->master = true;

     shared->shmpool = pool;

     D_MAGIC_SET( shared, CoreDFBShared );

     /* Initialize. */
     ret = dfb_core_initialize( core );
     if (ret) {
          D_MAGIC_CLEAR( shared );
          SHFREE( pool, shared );
          fusion_shm_pool_destroy( core->world, pool );
          return ret;
     }

     fusion_skirmish_init( &shared->lock, "DirectFB Core", core->world );

     CoreDFB_Init_Dispatch( core, core, &shared->call );

     fusion_call_add_permissions( &shared->call, 0, FUSION_CALL_PERMIT_EXECUTE );

     /* Register shared data. */
     fusion_arena_add_shared_field( arena, "Core/Shared", shared );

     return DFB_OK;
}

static int
dfb_core_arena_shutdown( FusionArena *arena,
                         void        *ctx,
                         bool         emergency)
{
     DFBResult            ret;
     CoreDFB             *core = ctx;
     CoreDFBShared       *shared;
     FusionSHMPoolShared *pool;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     pool = shared->shmpool;

     D_DEBUG_AT( DirectFB_Core, "Shutting down...\n" );

     if (!core->master) {
          D_WARN( "refusing shutdown in slave" );
          return dfb_core_leave( core, emergency );
     }

     CoreDFB_Deinit_Dispatch( &shared->call );
     /* Shutdown. */
     ret = dfb_core_shutdown( core, emergency );

     fusion_skirmish_destroy( &shared->lock );

     D_MAGIC_CLEAR( shared );

     SHFREE( pool, shared );

     fusion_shm_pool_destroy( core->world, pool );

     return ret;
}

static int
dfb_core_arena_join( FusionArena *arena,
                     void        *ctx )
{
     DFBResult  ret;
     CoreDFB   *core = ctx;
     void      *field;

     D_MAGIC_ASSERT( core, CoreDFB );

     D_DEBUG_AT( DirectFB_Core, "Joining...\n" );

     /* Get shared data. */
     if (fusion_arena_get_shared_field( arena, "Core/Shared", &field ))
          return DFB_FUSION;

     core->shared = field;

     /* Join. */
     ret = dfb_core_join( core );
     if (ret)
          return ret;

     return DFB_OK;
}

static int
dfb_core_arena_leave( FusionArena *arena,
                      void        *ctx,
                      bool         emergency)
{
     DFBResult  ret;
     CoreDFB   *core = ctx;

     D_MAGIC_ASSERT( core, CoreDFB );

     D_DEBUG_AT( DirectFB_Core, "Leaving...\n" );

     /* Leave. */
     ret = dfb_core_leave( core, emergency );
     if (ret)
          return ret;

     return DFB_OK;
}

