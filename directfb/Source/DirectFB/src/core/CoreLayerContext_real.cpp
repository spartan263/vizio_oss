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

#include "CoreLayerContext.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/messages.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreLayerContext, "DirectFB/CoreLayerContext", "DirectFB CoreLayerContext" );

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ILayerContext_Real::CreateWindow(
                    const DFBWindowDescription                *description,
                    CoreWindow                                *parent,
                    CoreWindow                                *toplevel,
                    CoreWindow                               **ret_window
)
{
    DFBWindowDescription description_copy;

    D_DEBUG_AT( DirectFB_CoreLayerContext, "ILayerContext_Real::%s()\n", __FUNCTION__ );

    D_ASSERT( description != NULL );
    D_ASSERT( ret_window != NULL );

    description_copy = *description;

    description_copy.parent_id   = parent   ? parent->object.id   : 0;
    description_copy.toplevel_id = toplevel ? toplevel->object.id : 0;

    return dfb_layer_context_create_window( core, obj, &description_copy, ret_window );
}

DFBResult
ILayerContext_Real::SetConfiguration(
                    const DFBDisplayLayerConfig               *config
)
{
    D_DEBUG_AT( DirectFB_CoreLayerContext, "ILayerContext_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    return dfb_layer_context_set_configuration( obj, config );
}

DFBResult
ILayerContext_Real::GetPrimaryRegion(
                    bool                                       create,
                    CoreLayerRegion                          **ret_region
)
{
    D_DEBUG_AT( DirectFB_CoreLayerContext, "ILayerContext_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_region != NULL );

    return dfb_layer_context_get_primary_region( obj, create, ret_region );
}


}

