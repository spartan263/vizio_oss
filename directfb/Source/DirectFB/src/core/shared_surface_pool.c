/*
   (c) Copyright 2001-2009  The world wide DirectFB Open Source Community (directfb.org)
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
#ifdef CC_DFB_SHARE_MEMORY
#include <config.h>

#include <string.h>
#include <unistd.h>

#include <direct/debug.h>

#include <fusion/conf.h>
#include <fusion/shmalloc.h>
#include <fusion/shm/pool.h>

#include <core/core.h>
#include <core/surface_pool.h>

#include <misc/conf.h>
#include "x_typedef.h"

/**********************************************************************************************************************/

typedef enum
{
    E_DSSP_ALLOC = 0,
    E_DSSP_SH,
    E_DSSP_SH2,
    E_DSSP_SH3,
    E_DSSP_MAX,
} E_DFB_SSP;

typedef struct {
     FusionSHMPoolShared *shmpool;
} SharedPoolData;

typedef struct {
     CoreDFB     *core;
     FusionWorld *world;
} SharedPoolLocalData;

typedef struct {
     void *addr;
     void *aligned_addr;
     int   pitch;
     int   size;
     int   pid[E_DSSP_MAX];
     DFB_IOMMU_PIN_RANGE_T pin_range[E_DSSP_MAX];
} SharedAllocationData;

/**********************************************************************************************************************/

static void shared_check_mmu_pin_mem(SharedAllocationData *alloc)
{
    E_DFB_SSP e_sh = E_DSSP_SH;
    
    if(!alloc || (0x0 == alloc->pin_range[E_DSSP_ALLOC].start))
    {
        D_ERROR("%s[pid = %d] Fail\n",__FUNCTION__,getpid());
        return ;
    }
    
    if(alloc->pin_range[E_DSSP_SH].start)
    {
        e_sh = E_DSSP_SH2;
    }
    else if(alloc->pin_range[E_DSSP_SH2].start)
    {
        e_sh = E_DSSP_SH3;
    }
    else if(alloc->pin_range[E_DSSP_SH3].start)
    {
        D_ERROR("%s [pid = %d][%d,%d,%d] Fail\n",__FUNCTION__,getpid(),alloc->pid[E_DSSP_SH],alloc->pid[E_DSSP_SH2],alloc->pid[E_DSSP_SH3]);
        return ;
    }
    
    alloc->pid[e_sh] = getpid();
    alloc->pin_range[e_sh].start = alloc->pin_range[E_DSSP_ALLOC].start;
    alloc->pin_range[e_sh].size  = alloc->pin_range[E_DSSP_ALLOC].size;
    
    mlock((void*)alloc->pin_range[e_sh].start,alloc->pin_range[e_sh].size);
    system_mmu_pin_mem(&alloc->pin_range[e_sh]);
    
    return ;
}

static void shared_check_mmu_unpin_mem(SharedAllocationData *alloc)
{
    E_DFB_SSP e_sh = E_DSSP_SH;
    
    if(!alloc)
    {
        D_ERROR("%s[pid = %d] Fail\n",__FUNCTION__,getpid());
        return ;
    }

    if(alloc->pid[E_DSSP_SH] == getpid())
    {
        e_sh = E_DSSP_SH;
        if(0x0 == alloc->pin_range[E_DSSP_SH].start)
        {
            D_ERROR("%s E_DSSP_SH[pid = %d][%d,%d,%d] Fail\n",__FUNCTION__,getpid(),alloc->pid[E_DSSP_SH],alloc->pid[E_DSSP_SH2],alloc->pid[E_DSSP_SH3]);
            return ;
        }
    }
    else if (alloc->pid[E_DSSP_SH2] == getpid())
    {
        e_sh = E_DSSP_SH2;
        D_ERROR("%s E_DSSP_SH2 [pid = %d] \n",__FUNCTION__,getpid());
        if(0x0 == alloc->pin_range[E_DSSP_SH2].start)
        {
            D_ERROR("%s E_DSSP_SH2[pid = %d][%d,%d,%d] Fail\n",__FUNCTION__,getpid(),alloc->pid[E_DSSP_SH],alloc->pid[E_DSSP_SH2],alloc->pid[E_DSSP_SH3]);
            return ;
        }
    }
    else if (alloc->pid[E_DSSP_SH3] == getpid())
    {
        e_sh = E_DSSP_SH3;
        D_ERROR("%s E_DSSP_SH2 [pid = %d] \n",__FUNCTION__,getpid());
        if(0x0 == alloc->pin_range[E_DSSP_SH3].start)
        {
            D_ERROR("%s E_DSSP_SH2[pid = %d][%d,%d,%d] Fail\n",__FUNCTION__,getpid(),alloc->pid[E_DSSP_SH],alloc->pid[E_DSSP_SH2],alloc->pid[E_DSSP_SH3]);
            return ;
        }
    }
    else
    {
        D_ERROR("%s[pid = %d][%d,%d,%d] Fail\n",__FUNCTION__,getpid(),alloc->pid[E_DSSP_SH],alloc->pid[E_DSSP_SH2],alloc->pid[E_DSSP_SH3]);
        return ;
    }
    
    mt53EngineSync(NULL,NULL);
    
    system_mmu_unpin_mem(&alloc->pin_range[e_sh]);
    alloc->pin_range[e_sh].start    = 0x0;
    alloc->pin_range[e_sh].size     = 0x0;
    alloc->pid[e_sh]                = 0x0;
    
    return ;
}


static int
sharedPoolDataSize( void )
{
     return sizeof(SharedPoolData);
}

static int
sharedPoolLocalDataSize( void )
{
     return sizeof(SharedPoolLocalData);
}

static int
sharedAllocationDataSize( void )
{
     return sizeof(SharedAllocationData);
}

static DFBResult
sharedInitPool( CoreDFB                    *core,
                CoreSurfacePool            *pool,
                void                       *pool_data,
                void                       *pool_local,
                void                       *system_data,
                CoreSurfacePoolDescription *ret_desc )
{
     DFBResult            ret;
     SharedPoolData      *data  = pool_data;
     SharedPoolLocalData *local = pool_local;
     int i4_acces;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_ASSERT( ret_desc != NULL );

     local->core  = core;
     local->world = dfb_core_world( core );

     ret = fusion_shm_pool_create( local->world, "Surface Memory Pool", dfb_config->surface_shmpool_size,
                                   fusion_config->debugshm, &data->shmpool );
     if (ret)
          return ret;
     
     if(DFB_SYSTEMONLY)
     {
        ret_desc->caps   = CSPCAPS_ALL;
        ret_desc->scaps  = DSCAPS_ALL&(~(DSCAPS_VIDEO));
        ret_desc->types  = CSTF_ALL&(~(SURFACE_TYPE_VIDEO));
        ret_desc->priority          = CSPP_ULTIMATE;
        for(i4_acces = CSAID_NONE;i4_acces<_CSAID_NUM;i4_acces++)
        {
            ret_desc->access[i4_acces] = CSAF_ALL;
        }
     }
     else
     {
        ret_desc->caps              = CSPCAPS_NONE;
        ret_desc->access[CSAID_CPU] = CSAF_READ | CSAF_WRITE | CSAF_SHARED;
        ret_desc->types             = CSTF_LAYER | CSTF_WINDOW | CSTF_CURSOR | CSTF_FONT | CSTF_SHARED | CSTF_INTERNAL;
        ret_desc->priority          = CSPP_DEFAULT;
     }

     snprintf( ret_desc->name, DFB_SURFACE_POOL_DESC_NAME_LENGTH, "Shared Memory" );

     return DFB_OK;
}

static DFBResult
sharedDestroyPool( CoreSurfacePool *pool,
                   void            *pool_data,
                   void            *pool_local )
{
     SharedPoolData      *data  = pool_data;
     SharedPoolLocalData *local = pool_local;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     
     mt53EngineSync(NULL,NULL);
     
     fusion_shm_pool_destroy( local->world, data->shmpool );

     return DFB_OK;
}

static DFBResult
sharedAllocateBuffer( CoreSurfacePool       *pool,
                      void                  *pool_data,
                      void                  *pool_local,
                      CoreSurfaceBuffer     *buffer,
                      CoreSurfaceAllocation *allocation,
                      void                  *alloc_data )
{
    CoreSurface          *surface;
    SharedPoolData       *data  = pool_data;
    SharedAllocationData *alloc = alloc_data;
    int byte_align  =64;

    D_MAGIC_ASSERT( pool, CoreSurfacePool );
    D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

    surface = buffer->surface;
    D_MAGIC_ASSERT( surface, CoreSurface );
    memset(alloc,0x0,sizeof(SharedAllocationData));
    dfb_surface_calc_buffer_size( surface,byte_align,0,&alloc->pitch, &alloc->size );
    alloc->size += byte_align;
    alloc->addr = SHMALLOC( data->shmpool, alloc->size);
    if ( !alloc->addr )
    return D_OOSHM();

    /* Calculate the aligned address. */

    unsigned long addr           = (unsigned long) alloc->addr;
    unsigned long aligned_offset = (byte_align - (addr % byte_align ));

    alloc->aligned_addr = (void*) (addr + aligned_offset);
    if(aligned_offset != byte_align)
    {
        alloc->aligned_addr = (void*) (addr + aligned_offset);
    }
    else
    {
      alloc->aligned_addr = (void*) (addr);
    }
    alloc->pin_range[E_DSSP_ALLOC].start = (unsigned long)alloc->addr;
    alloc->pin_range[E_DSSP_ALLOC].size  = alloc->size;
    alloc->pid[E_DSSP_ALLOC] = getpid();
    
    mlock(alloc->addr,alloc->size);
    system_mmu_pin_mem(&alloc->pin_range[E_DSSP_ALLOC]);

    allocation->flags = CSALF_VOLATILE;
    allocation->size  = alloc->size;

    if(surface->type  & CSTF_WINDOW )
    {
       mt53_fill_rect((unsigned int)alloc->addr,alloc->pitch,surface->config.size.h);
    }
    else
    {
  //     mt53FlushInvalidCache((unsigned int)alloc->addr,alloc->size);
    }

   // direct_log_printf(NULL,"%s[0x%x,0x%x]\n",__FUNCTION__,alloc->pin_range.start,alloc->pin_range.size);

    return DFB_OK;
}

static DFBResult
sharedDeallocateBuffer( CoreSurfacePool       *pool,
                        void                  *pool_data,
                        void                  *pool_local,
                        CoreSurfaceBuffer     *buffer,
                        CoreSurfaceAllocation *allocation,
                        void                  *alloc_data )
{
     SharedPoolData       *data  = pool_data;
     SharedAllocationData *alloc = alloc_data;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
     system_mmu_unpin_mem(&alloc->pin_range[E_DSSP_ALLOC]);

     SHFREE( data->shmpool, alloc->addr );

     return DFB_OK;
}

static DFBResult
sharedLock( CoreSurfacePool       *pool,
            void                  *pool_data,
            void                  *pool_local,
            CoreSurfaceAllocation *allocation,
            void                  *alloc_data,
            CoreSurfaceBufferLock *lock )
{
     SharedAllocationData *alloc = alloc_data;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );
     D_MAGIC_ASSERT( lock, CoreSurfaceBufferLock );
     
     if (alloc->aligned_addr)
     {
          lock->addr = alloc->aligned_addr;
          lock->phys = (unsigned long)alloc->aligned_addr;
     }
     else
     {
          lock->addr = alloc->addr;
          lock->phys = (unsigned long)alloc->addr;
     }
     lock->pitch = alloc->pitch;
 //    lock->size  = alloc->size;

     if(alloc->pid[E_DSSP_ALLOC] != getpid())
     {
        shared_check_mmu_pin_mem(alloc);
     }
     
     return DFB_OK;
}

static DFBResult
sharedUnlock( CoreSurfacePool       *pool,
              void                  *pool_data,
              void                  *pool_local,
              CoreSurfaceAllocation *allocation,
              void                  *alloc_data,
              CoreSurfaceBufferLock *lock )
{
    SharedAllocationData *alloc = alloc_data;
    D_MAGIC_ASSERT( pool, CoreSurfacePool );
    D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );
    D_MAGIC_ASSERT( lock, CoreSurfaceBufferLock );
    
    if(alloc->pid[E_DSSP_ALLOC] != getpid())
    {
        shared_check_mmu_unpin_mem(alloc);
    }

     return DFB_OK;
}

const SurfacePoolFuncs sharedSurfacePoolFuncs = {
     .PoolDataSize       = sharedPoolDataSize,
     .PoolLocalDataSize  = sharedPoolLocalDataSize,
     .AllocationDataSize = sharedAllocationDataSize,
     .InitPool           = sharedInitPool,
     .DestroyPool        = sharedDestroyPool,

     .AllocateBuffer     = sharedAllocateBuffer,
     .DeallocateBuffer   = sharedDeallocateBuffer,

     .Lock               = sharedLock,
     .Unlock             = sharedUnlock,
};

#else
#include <config.h>

#include <direct/debug.h>

#include <fusion/fusion.h>
#include <fusion/shm/shm_internal.h>

#include <core/core.h>
#include <core/surface_pool.h>
#include <core/system.h>

#include <misc/conf.h>

/**********************************************************************************************************************/

typedef struct {
     char         tmpfs_dir[FUSION_SHM_TMPFS_PATH_NAME_LEN + 20];

     u64          indices;
} SharedPoolData;

typedef struct {
     CoreDFB     *core;
     FusionWorld *world;
} SharedPoolLocalData;

typedef struct {
     u64          index;
     int          pitch;
     int          size;
} SharedAllocationData;

/**********************************************************************************************************************/

static int
sharedPoolDataSize( void )
{
     return sizeof(SharedPoolData);
}

static int
sharedPoolLocalDataSize( void )
{
     return sizeof(SharedPoolLocalData);
}

static int
sharedAllocationDataSize( void )
{
     return sizeof(SharedAllocationData);
}

static DFBResult
sharedInitPool( CoreDFB                    *core,
                CoreSurfacePool            *pool,
                void                       *pool_data,
                void                       *pool_local,
                void                       *system_data,
                CoreSurfacePoolDescription *ret_desc )
{
     SharedPoolData      *data  = pool_data;
     SharedPoolLocalData *local = pool_local;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_ASSERT( ret_desc != NULL );

     ret_desc->caps              = CSPCAPS_VIRTUAL;
     ret_desc->access[CSAID_CPU] = CSAF_READ | CSAF_WRITE | CSAF_SHARED;
     ret_desc->types             = CSTF_LAYER | CSTF_WINDOW | CSTF_CURSOR | CSTF_FONT | CSTF_SHARED | CSTF_INTERNAL;// | CSTF_EXTERNAL;
     ret_desc->priority          = CSPP_DEFAULT;

     snprintf( ret_desc->name, DFB_SURFACE_POOL_DESC_NAME_LENGTH, "Shared Memory" );

     local->core  = core;
     local->world = dfb_core_world( core );

     snprintf( data->tmpfs_dir, sizeof(data->tmpfs_dir), "%s/dfb.%d",
               fusion_get_tmpfs( local->world ), fusion_world_index( local->world ) );

     if (mkdir( data->tmpfs_dir, 0750 ) < 0) {
          DIR           *dir;
          struct dirent *entry = NULL;
          struct dirent  tmp;

          if (errno != EEXIST) {
               D_PERROR( "Core/Surface/SHM: Could not create '%s'!\n", data->tmpfs_dir );
               return DFB_IO;
          }

          D_WARN( "%s exists, cleaning up", data->tmpfs_dir );

          dir = opendir( data->tmpfs_dir );
          if (!dir) {
               D_PERROR( "Core/Surface/SHM: Could not open '%s'!\n", data->tmpfs_dir );
               return DFB_IO;
          }

          while (readdir_r( dir, &tmp, &entry ) == 0 && entry) {
               char buf[FUSION_SHM_TMPFS_PATH_NAME_LEN + 99];

               if (!direct_strcmp( entry->d_name, "." ) ||
                   !direct_strcmp( entry->d_name, ".." ))
                    continue;

               snprintf( buf, sizeof(buf), "%s/%s", data->tmpfs_dir, entry->d_name );

               if (unlink( buf ) < 0) {
                    D_PERROR( "Core/Surface/SHM: Could not remove '%s'!\n", buf );
                    closedir( dir );
                    return DFB_IO;
               }
          }

          closedir( dir );
     }

     return DFB_OK;
}

static DFBResult
sharedDestroyPool( CoreSurfacePool *pool,
                   void            *pool_data,
                   void            *pool_local )
{
     SharedPoolData *data = pool_data;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );

     if (rmdir( data->tmpfs_dir ) < 0)
          D_PERROR( "Core/Surface/SHM: Could not remove '%s'!\n", data->tmpfs_dir );

     return DFB_OK;
}

static DFBResult
sharedAllocateBuffer( CoreSurfacePool       *pool,
                      void                  *pool_data,
                      void                  *pool_local,
                      CoreSurfaceBuffer     *buffer,
                      CoreSurfaceAllocation *allocation,
                      void                  *alloc_data )
{
     CoreSurface          *surface;
     SharedPoolData       *data  = pool_data;
     SharedAllocationData *alloc = alloc_data;
     char                  buf[FUSION_SHM_TMPFS_PATH_NAME_LEN + 99];
     int                   fd;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

     surface = buffer->surface;
     D_MAGIC_ASSERT( surface, CoreSurface );

     dfb_surface_calc_buffer_size( surface, 8, 0, &alloc->pitch, &alloc->size );

     alloc->index = ++(data->indices);

     snprintf( buf, sizeof(buf), "%s/surface_0x%08x_shared_allocation_%zu", data->tmpfs_dir, surface->object.id, alloc->index );

     fd = open( buf, O_RDWR | O_CREAT | O_EXCL, 0660 );
     if (fd < 0) {
          D_PERROR( "Core/Surface/SHM: Could not create '%s'!\n", buf );
          return DFB_IO;
     }

     fchmod( fd, 0660 );

     if (ftruncate( fd, alloc->size ) < 0) {
          D_PERROR( "Core/Surface/SHM: Setting file size for '%s' to %d failed!\n", buf, alloc->size );
          unlink( buf );
          return DFB_IO;
     }

     close( fd );

     allocation->flags = CSALF_VOLATILE;
     allocation->size  = alloc->size;

     return DFB_OK;
}

static DFBResult
sharedDeallocateBuffer( CoreSurfacePool       *pool,
                        void                  *pool_data,
                        void                  *pool_local,
                        CoreSurfaceBuffer     *buffer,
                        CoreSurfaceAllocation *allocation,
                        void                  *alloc_data )
{
     CoreSurface          *surface;
     SharedPoolData       *data  = pool_data;
     SharedAllocationData *alloc = alloc_data;
     char                  buf[FUSION_SHM_TMPFS_PATH_NAME_LEN + 99];

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

     surface = buffer->surface;
     D_MAGIC_ASSERT( surface, CoreSurface );

     snprintf( buf, sizeof(buf), "%s/surface_0x%08x_shared_allocation_%zu", data->tmpfs_dir, surface->object.id, alloc->index );

     if (unlink( buf ) < 0) {
          D_PERROR( "Core/Surface/SHM: Could not remove '%s'!\n", buf );
          return DFB_IO;
     }

     return DFB_OK;
}

static DFBResult
sharedLock( CoreSurfacePool       *pool,
            void                  *pool_data,
            void                  *pool_local,
            CoreSurfaceAllocation *allocation,
            void                  *alloc_data,
            CoreSurfaceBufferLock *lock )
{
     CoreSurface          *surface;
     CoreSurfaceBuffer    *buffer;
     SharedPoolData       *data  = pool_data;
     SharedAllocationData *alloc = alloc_data;
     char                  buf[FUSION_SHM_TMPFS_PATH_NAME_LEN + 99];
     int                   fd;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );
     D_MAGIC_ASSERT( lock, CoreSurfaceBufferLock );

     buffer = lock->buffer;
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

     surface = buffer->surface;
     D_MAGIC_ASSERT( surface, CoreSurface );

     snprintf( buf, sizeof(buf), "%s/surface_0x%08x_shared_allocation_%zu", data->tmpfs_dir, surface->object.id, alloc->index );

     fd = open( buf, O_RDWR | O_CREAT | O_EXCL, 0660 );//open( buf, O_RDWR );
     if (fd < 0) {
          D_PERROR( "Core/Surface/SHM: Could not open '%s'!\n", buf );
          return DFB_IO;
     }

     fchmod( fd, 0660 );

     if (ftruncate( fd, alloc->size ) < 0) {
          D_PERROR( "Core/Surface/SHM: Setting file size for '%s' to %d failed!\n", buf, alloc->size );
          unlink( buf );
          return DFB_IO;
     }	 


     lock->addr = mmap( NULL, alloc->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

     close( fd );

     if (lock->addr == MAP_FAILED) {
          D_PERROR( "Core/Surface/SHM: Could not mmap '%s'!\n", buf );
          return DFB_IO;
     }


     lock->pitch = alloc->pitch;

     return DFB_OK;
}

static DFBResult
sharedUnlock( CoreSurfacePool       *pool,
              void                  *pool_data,
              void                  *pool_local,
              CoreSurfaceAllocation *allocation,
              void                  *alloc_data,
              CoreSurfaceBufferLock *lock )
{
     SharedAllocationData *alloc = alloc_data;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );
     D_MAGIC_ASSERT( lock, CoreSurfaceBufferLock );

     munmap( lock->addr, alloc->size );

     return DFB_OK;
}

const SurfacePoolFuncs sharedSurfacePoolFuncs = {
     .PoolDataSize       = sharedPoolDataSize,
     .PoolLocalDataSize  = sharedPoolLocalDataSize,
     .AllocationDataSize = sharedAllocationDataSize,
     .InitPool           = sharedInitPool,
     .DestroyPool        = sharedDestroyPool,

     .AllocateBuffer     = sharedAllocateBuffer,
     .DeallocateBuffer   = sharedDeallocateBuffer,

     .Lock               = sharedLock,
     .Unlock             = sharedUnlock,
};
#endif
