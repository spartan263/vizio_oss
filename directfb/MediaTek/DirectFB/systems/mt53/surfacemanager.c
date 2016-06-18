/*
   (c) Copyright 2001-2007  The DirectFB Organization (directfb.org)
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

#include <fusion/shmalloc.h>

#include <directfb.h>

#include <core/core.h>

#include <core/gfxcard.h>
#include <core/surface.h>
#include <core/surface_buffer.h>

#include <direct/debug.h>
#include <direct/messages.h>
#include <direct/util.h>

#include <gfx/convert.h>

#include "surfacemanager.h"

#include <mt53/mt53.h>


D_DEBUG_DOMAIN( SurfMan, "SurfaceManager", "DirectFB Surface Manager" );

struct _SurfaceManager {
     int                  magic;

     FusionSHMPoolShared *shmpool;

     Chunk               *chunks;

     int                  offset;
     int                  length;         /* length of the heap in bytes */
     int                  avail;          /* amount of available memory in bytes */
     int                  min_toleration;   
     bool                 suspended;
#ifdef CC_DFB_MEM_CHK
     int                  StatisticsTotal;
     int                  StatisticsPeak;
#endif     
};

extern MT53 *dfb_mt53;

static Chunk *split_chunk ( SurfaceManager *manager,
                            Chunk          *chunk,
                            int             length );

static Chunk *free_chunk  ( SurfaceManager *manager,
                            Chunk          *chunk );

static Chunk *occupy_chunk( SurfaceManager     *manager,
                            Chunk              *chunk,
                            CoreSurfaceBuffer  *buffer,
                            int                 length,
                            int                 pitch );

DFBResult dfb_surfacemanager_dump(void);

DFBResult
dfb_surfacemanager_create( CoreDFB         *core,
                           unsigned int     length,
                           SurfaceManager **ret_manager )
{
     FusionSHMPoolShared *pool;
     SurfaceManager      *manager;
     Chunk               *chunk;

     D_DEBUG_AT( SurfMan, "%s( %p, %d )\n", __FUNCTION__, core, length );

     D_ASSERT( core != NULL );
     D_ASSERT( ret_manager != NULL );

     pool = dfb_core_shmpool( core );

     manager = SHCALLOC( pool, 1, sizeof(SurfaceManager) );
     if (!manager)
          return D_OOSHM();

     chunk = SHCALLOC( pool, 1, sizeof(Chunk) );
     if (!chunk) {
          D_OOSHM();
          SHFREE( pool, manager );
          return DFB_NOSHAREDMEMORY;
     }

     manager->shmpool = pool;
     manager->chunks  = chunk;
     manager->offset  = 0;
     manager->length  = length;
     manager->avail   = manager->length - manager->offset;
#ifdef CC_DFB_MEM_CHK
     manager->StatisticsTotal = 0;
     manager->StatisticsPeak = 0;
#endif

     D_MAGIC_SET( manager, SurfaceManager );

     chunk->offset    = manager->offset;
     chunk->length    = manager->avail;

     D_MAGIC_SET( chunk, Chunk );

     D_DEBUG_AT( SurfMan, "  -> %p\n", manager );

     *ret_manager = manager;

     return DFB_OK;
}

void
dfb_surfacemanager_destroy( SurfaceManager *manager )
{
     Chunk *chunk;
     void  *next;

     D_DEBUG_AT( SurfMan, "%s( %p )\n", __FUNCTION__, manager );

     D_MAGIC_ASSERT( manager, SurfaceManager );

     /* Deallocate all video chunks. */
     chunk = manager->chunks;
     while (chunk) {
          next = chunk->next;

          D_MAGIC_CLEAR( chunk );

          SHFREE( manager->shmpool, chunk );

          chunk = next;
     }

     D_MAGIC_CLEAR( manager );

     /* Deallocate manager struct. */
     SHFREE( manager->shmpool, manager );
}

DFBResult dfb_surfacemanager_adjust_heap_offset( SurfaceManager *manager,
                                                 int             offset )
{
     D_MAGIC_ASSERT( manager, SurfaceManager );
     D_ASSERT( offset >= 0 );

     D_DEBUG_AT( SurfMan, "%s( %p, %d )\n", __FUNCTION__, manager, offset );

/*FIXME_SC_2     if (manager->limits.surface_byteoffset_alignment > 1) {
          offset += manager->limits.surface_byteoffset_alignment - 1;
          offset -= offset % manager->limits.surface_byteoffset_alignment;
     }
*/
     /*
      * Adjust the offset of the heap.
      */
     if (manager->chunks->buffer == NULL) {
          /* first chunk is free */
          if (offset <= manager->chunks->offset + manager->chunks->length) {
               /* ok, just recalculate offset and length */
               manager->chunks->length = manager->chunks->offset +
                                         manager->chunks->length - offset;
               manager->chunks->offset = offset;
          }
          else {
               D_WARN("unable to adjust heap offset");
               /* more space needed than free at the beginning */
               /* TODO: move/destroy instances */
          }
     }
     else {
          D_WARN("unable to adjust heap offset");
          /* very rare case that the first chunk is occupied */
          /* TODO: move/destroy instances */
     }

     manager->avail -= offset - manager->offset;
     manager->offset = offset;

     return DFB_OK;
}

void dfb_surfacemanager_pool_printf_info(SurfaceManager     *manager)
{
    Chunk *c = manager->chunks;
    
    direct_log_printf( NULL, "dfb_surfacemanager_pool_printf_info\n");
    while(c) 
    {
        direct_log_printf( NULL, "[len=0x%x,off=0x%x] used[%d]\n", c->length,c->offset,c->buffer?1:0);
        c = c->next;
    }
    direct_log_printf( NULL, "<==\n");
}

/** public functions NOT locking the surfacemanger theirself,
    to be called between lock/unlock of surfacemanager **/


DFBResult dfb_surfacemanager_allocate( CoreDFB           *core,
                                       SurfaceManager     *manager,
                                       CoreSurfaceBuffer  *buffer,
                                       Chunk              **ret_chunk , 
                                       unsigned int       u4_vdo_mem_type)                               
{
    int pitch;
    int length;
    Chunk *c;
     CoreGraphicsDevice *device;

     Chunk *best_free     = NULL;
     Chunk *best_occupied = NULL;

     D_MAGIC_ASSERT( manager, SurfaceManager );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
     D_MAGIC_ASSERT( buffer->surface, CoreSurface );
//NULL means check only     D_ASSERT( ret_chunk != NULL );

     D_DEBUG_AT( SurfMan, "%s( %p ) <- %dx%d %s\n", __FUNCTION__, buffer,
                 buffer->surface->config.size.w, buffer->surface->config.size.h,
                 dfb_pixelformat_name( buffer->surface->config.format ) );

     if (manager->suspended)
          return DFB_SUSPENDED;

     /* FIXME: Only one global device at the moment. */
     device = dfb_core_get_part( core, DFCP_GRAPHICS );
     D_ASSERT( device != NULL );

     dfb_gfxcard_calc_buffer_size( device, buffer, &pitch, &length );

     D_DEBUG_AT( SurfMan, "  -> pitch %d, length %d\n", pitch, length );

     /* examine chunks */
     c = manager->chunks;
     D_MAGIC_ASSERT( c, Chunk );

    /* FIXME_SC_2  Workaround creation happening before graphics driver initialization. */
    if(!c->next)
    {
        int length;
        if(u4_vdo_mem_type&DSCAPS_FROM_FBM)
        {
            length = dfb_gfxcard_auxmemory_length();
	   D_INFO("DSCAPS_FROM_FBM length = %d\n");		
        }
        else if(u4_vdo_mem_type&DSCAPS_FROM_3DMM)
        {
            length = dfb_mt53->imagebuf2.u4Size;
	   D_INFO("DSCAPS_FROM_3DMM length = %d\n");			
        }
        else if(u4_vdo_mem_type&DSCAPS_FROM_CRSR)
        {
            length = dfb_mt53->t_crsr.u4Size;
	   D_INFO("DSCAPS_FROM_CRSR length = %d\n");	 		
        }
        else
        {
            length = dfb_gfxcard_memory_length();
	   D_INFO("else length = %d\n");			
        }

        if(c->length != length - manager->offset)
        {
            D_WARN( "workaround" );

            manager->length = length;
            manager->avail = length - manager->offset;

            c->length = length - manager->offset;
        }
    }

    MTDFB_LOG( DFB_LOG_LVL_3," dfb_surfacemanager_allocate[alloc=0x%x,avl=0x%x,total=0x%x]\n", length,manager->avail, manager->length );

    while(c)
    {
        D_MAGIC_ASSERT( c, Chunk );

        if(!c->buffer && c->length >= length)
        {
            /* NULL means check only. */
            if (!ret_chunk)
            return DFB_OK;

            /* found a nice place to chill */
            if (!best_free || best_free->length > c->length)
            /* first found or better one? */
            best_free = c;

            if (c->length == length)
            break;
          }

        c = c->next;
    }

    /* if we found a place */
    if (best_free)
    {
        /* NULL means check only. */
        if (ret_chunk)
        {
            *ret_chunk = occupy_chunk( manager, best_free, buffer, length, pitch );
        }

        return DFB_OK;
    }

    direct_log_printf( NULL, "  -> failed (%d) (%d/%d avail)\n", length,manager->avail, manager->length );
    dfb_surfacemanager_pool_printf_info( manager);

    /* no luck */
    return DFB_NOVIDEOMEMORY;
}


DFBResult dfb_surfacemanager_deallocate( SurfaceManager *manager,
                                         Chunk            *chunk)
{
     CoreSurfaceBuffer *buffer;

     D_MAGIC_ASSERT( manager, SurfaceManager );
     D_MAGIC_ASSERT( chunk, Chunk );

     buffer = chunk->buffer;
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
     D_MAGIC_ASSERT( buffer->surface, CoreSurface );

     D_DEBUG_AT( SurfMan, "%s( %p ) <- %dx%d %s\n", __FUNCTION__, buffer,
                 buffer->surface->config.size.w, buffer->surface->config.size.h,
                 dfb_pixelformat_name( buffer->surface->config.format ) );

     // direct_log_printf( NULL, "deallocate[len=0x%x,off=0x%x] \n", chunk->length,chunk->offset); 
     
     free_chunk( manager, chunk );

     return DFB_OK;
}

/** internal functions NOT locking the surfacemanager **/

static Chunk *
split_chunk( SurfaceManager *manager, Chunk *c, int length )
{
     Chunk *newchunk;

     D_MAGIC_ASSERT( c, Chunk );

     if (c->length == length)          /* does not need be splitted */
          return c;

     newchunk = (Chunk*) SHCALLOC( manager->shmpool, 1, sizeof(Chunk) );

     /* calculate offsets and lengths of resulting chunks */
     newchunk->offset = c->offset + c->length - length;
     newchunk->length = length;
     c->length -= newchunk->length;

     /* insert newchunk after chunk c */
     newchunk->prev = c;
     newchunk->next = c->next;
     if (c->next)
          c->next->prev = newchunk;
     c->next = newchunk;

     D_MAGIC_SET( newchunk, Chunk );

     return newchunk;
}

static Chunk *
free_chunk( SurfaceManager *manager, Chunk *chunk )
{
     D_MAGIC_ASSERT( manager, SurfaceManager );
     D_MAGIC_ASSERT( chunk, Chunk );

     if (!chunk->buffer) {
          D_BUG( "freeing free chunk" );
          return chunk;
     }
     else {
          D_DEBUG_AT( SurfMan, "Deallocating %d bytes at offset %d.\n", chunk->length, chunk->offset );
     }

     if (chunk->buffer->policy == CSP_VIDEOONLY)
          manager->avail += chunk->length;

     chunk->buffer = NULL;

     manager->min_toleration--;

     if (chunk->prev  &&  !chunk->prev->buffer) {
          Chunk *prev = chunk->prev;

          //D_DEBUG_AT( SurfMan, "  -> merging with previous chunk at %d\n", prev->offset );

          prev->length += chunk->length;

          prev->next = chunk->next;
          if (prev->next)
               prev->next->prev = prev;

          //D_DEBUG_AT( SurfMan, "  -> freeing %p (prev %p, next %p)\n", chunk, chunk->prev, chunk->next);

          D_MAGIC_CLEAR( chunk );

          SHFREE( manager->shmpool, chunk );
          chunk = prev;
     }

     if (chunk->next  &&  !chunk->next->buffer) {
          Chunk *next = chunk->next;

          //D_DEBUG_AT( SurfMan, "  -> merging with next chunk at %d\n", next->offset );

          chunk->length += next->length;

          chunk->next = next->next;
          if (chunk->next)
               chunk->next->prev = chunk;

          D_MAGIC_CLEAR( next );

          SHFREE( manager->shmpool, next );
     }

     return chunk;
}

static Chunk *
occupy_chunk( SurfaceManager *manager, Chunk *chunk, CoreSurfaceBuffer *buffer, int length, int pitch )
{
     D_MAGIC_ASSERT( manager, SurfaceManager );
     D_MAGIC_ASSERT( chunk, Chunk );
     
     if (buffer->policy == CSP_VIDEOONLY)
          manager->avail -= length;

     chunk = split_chunk( manager, chunk, length );

     D_DEBUG_AT( SurfMan, "Allocating %d bytes at offset %d.\n", chunk->length, chunk->offset );

     chunk->buffer = buffer;
     chunk->pitch  = pitch;

     manager->min_toleration++;

     return chunk;
}

DFBResult dfb_surfacemanager_dump(void)                                  
{
     SurfaceManager     *manager;
#ifdef CC_OSD_USE_FBM
     SurfaceManager	*manager_fbm;
#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB 
     SurfaceManager	*manager_3dmm;
#endif
#endif     
     Chunk *c;

     manager = dfb_mt53->shared->manager;
     D_MAGIC_ASSERT( manager, SurfaceManager );

     /* examine chunks */
     c = manager->chunks;
     D_MAGIC_ASSERT( c, Chunk );

     while (c) {
          D_MAGIC_ASSERT( c, Chunk );

          //if (!(c->buffer))
          {
            printf("Chunk addr: 0x%8x, size: 0x%8x, free: %c, mngr: dfb\n", c->offset, c->length, c->buffer?'X':'O');
          }
          c = c->next;
     }

#ifdef CC_OSD_USE_FBM
     manager_fbm = dfb_mt53->shared->manager_fbm;
     D_MAGIC_ASSERT( manager_fbm, SurfaceManager );
     
     /* examine chunks */
     c = manager_fbm->chunks;
     D_MAGIC_ASSERT( c, Chunk );

     while (c) {
          D_MAGIC_ASSERT( c, Chunk );
          //if (!(c->buffer))
          {
            printf("Chunk addr: 0x%8x, size: 0x%8x, free: %c, mngr: fbm\n", c->offset, c->length, c->buffer?'X':'O');
          }

          c = c->next;
     }

#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB 
     manager_3dmm = dfb_mt53->shared->manager_3dmm;
     D_MAGIC_ASSERT( manager_3dmm, SurfaceManager );
     
     /* examine chunks */
     c = manager_3dmm->chunks;
     D_MAGIC_ASSERT( c, Chunk );

     while (c) {
          D_MAGIC_ASSERT( c, Chunk );
          //if (!(c->buffer))
          {
            printf("Chunk addr: 0x%8x, size: 0x%8x, free: %c, mngr: 3dmm\n", c->offset, c->length, c->buffer?'X':'O');
          }

          c = c->next;
     }
#endif
#endif

     /* no luck */
     return 0;
}
