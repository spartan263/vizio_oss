/*
   (c) Copyright 2001-2011  The world wide DirectFB Open Source Community (directfb.org)
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

#include <core/CoreDFB.h>
#include <core/CoreGraphicsState.h>

extern "C" {
#include <directfb.h>

#include <direct/debug.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreDFB, "DirectFB/Core", "DirectFB Core" );

/*********************************************************************************************************************/

namespace DirectFB {


DFBResult
ICore_Real::CreateSurface( const CoreSurfaceConfig  *config,
                           CoreSurfaceTypeFlags      type,
                           u64                       resource_id,
                           CorePalette              *palette,
                           CoreSurface             **ret_surface )
{
     D_DEBUG_AT( DirectFB_CoreDFB, "ICore_Real::%s( %p )\n", __FUNCTION__, core );

     D_MAGIC_ASSERT( obj, CoreDFB );
     D_ASSERT( config != NULL );
     D_ASSERT( ret_surface != NULL );

     CoreSurfaceConfig config_copy = *config;

     // FIXME: handle local / preallocated surfaces
     if (fusion_config->secure_fusion && !dfb_core_is_master( core_dfb ))
          config_copy.flags = (CoreSurfaceConfigFlags)(config_copy.flags & ~CSCONF_PREALLOCATED);

     return dfb_surface_create( obj, &config_copy, type, resource_id, palette, ret_surface );
}

DFBResult
ICore_Real::CreatePalette( u32           size,
                           CorePalette **ret_palette )
{
     D_DEBUG_AT( DirectFB_CoreDFB, "ICore_Real::%s( %p )\n", __FUNCTION__, core );

     D_MAGIC_ASSERT( obj, CoreDFB );
     D_ASSERT( ret_palette != NULL );

     return dfb_palette_create( obj, size, ret_palette );
}

DFBResult
ICore_Real::CreateState(
                    CoreGraphicsState                        **ret_state
)
{
    D_DEBUG_AT( DirectFB_CoreDFB, "ICore_Requestor::%s()\n", __FUNCTION__ );

    D_MAGIC_ASSERT( obj, CoreDFB );
    D_ASSERT( ret_state != NULL );

    return dfb_graphics_state_create( core, ret_state );
}

DFBResult
ICore_Real::WaitIdle(

)
{
    D_DEBUG_AT( DirectFB_CoreDFB, "ICore_Requestor::%s()\n", __FUNCTION__ );

    D_MAGIC_ASSERT( obj, CoreDFB );

    return dfb_gfxcard_sync();
}


}

