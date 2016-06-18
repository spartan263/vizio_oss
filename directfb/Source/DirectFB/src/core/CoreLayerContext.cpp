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
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreLayerContext, "DirectFB/CoreLayerContext", "DirectFB CoreLayerContext" );

/*********************************************************************************************************************/

DFBResult
CoreLayerContext_CreateWindow(
                    CoreLayerContext                          *obj,
                    const DFBWindowDescription                *description,
                    CoreWindow                                *parent,
                    CoreWindow                                *toplevel,
                    CoreWindow                               **ret_window
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ILayerContext_Real real( core_dfb, obj );

        return real.CreateWindow( description, parent, toplevel, ret_window );
    }

    DirectFB::ILayerContext_Requestor requestor( core_dfb, obj );

    return requestor.CreateWindow( description, parent, toplevel, ret_window );
}

DFBResult
CoreLayerContext_SetConfiguration(
                    CoreLayerContext                          *obj,
                    const DFBDisplayLayerConfig               *config
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ILayerContext_Real real( core_dfb, obj );

        return real.SetConfiguration( config );
    }

    DirectFB::ILayerContext_Requestor requestor( core_dfb, obj );

    return requestor.SetConfiguration( config );
}

DFBResult
CoreLayerContext_GetPrimaryRegion(
                    CoreLayerContext                          *obj,
                    bool                                       create,
                    CoreLayerRegion                          **ret_region
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ILayerContext_Real real( core_dfb, obj );

        return real.GetPrimaryRegion( create, ret_region );
    }

    DirectFB::ILayerContext_Requestor requestor( core_dfb, obj );

    return requestor.GetPrimaryRegion( create, ret_region );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
CoreLayerContext_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::CoreLayerContextDispatch *dispatch = (DirectFB::CoreLayerContextDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *CoreLayerContext_Init_Dispatch(
                    CoreDFB              *core,
                    CoreLayerContext     *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreLayerContext_Dispatch, obj, core->world );

    return NULL;
}

void  CoreLayerContext_Deinit_Dispatch(FusionCall *call)
{
    fusion_call_destroy( call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ILayerContext_Requestor::CreateWindow(
                    const DFBWindowDescription                *description,
                    CoreWindow                                *parent,
                    CoreWindow                                *toplevel,
                    CoreWindow                               **ret_window
)
{
    DFBResult           ret;
    CoreWindow *window = NULL;
    CoreLayerContextCreateWindow       *block = (CoreLayerContextCreateWindow*) alloca( sizeof(CoreLayerContextCreateWindow) );
    CoreLayerContextCreateWindowReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreLayerContext, "ILayerContext_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( description != NULL );
    D_ASSERT( ret_window != NULL );

    block->description = *description;
  if (parent) {
    block->parent_id = CoreWindow_GetID( parent );
    block->parent_set = true;
  }
  else
    block->parent_set = false;
  if (toplevel) {
    block->toplevel_id = CoreWindow_GetID( toplevel );
    block->toplevel_set = true;
  }
  else
    block->toplevel_set = false;

    ret = (DFBResult) CoreLayerContext_Call( obj, FCEF_NONE, CoreLayerContext_CreateWindow, block, sizeof(CoreLayerContextCreateWindow), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayerContext_Call( CoreLayerContext_CreateWindow ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreLayerContext_CreateWindow failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    ret = (DFBResult) CoreWindow_Catch( core, return_block.window_id, &window );
    if (ret) {
         D_DERROR( ret, "%s: Catching window by ID %u failed!\n", __FUNCTION__, return_block.window_id );
         return ret;
    }

    *ret_window = window;

    return DFB_OK;
}


DFBResult
ILayerContext_Requestor::SetConfiguration(
                    const DFBDisplayLayerConfig               *config
)
{
    DFBResult           ret;
    CoreLayerContextSetConfiguration       *block = (CoreLayerContextSetConfiguration*) alloca( sizeof(CoreLayerContextSetConfiguration) );
    CoreLayerContextSetConfigurationReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreLayerContext, "ILayerContext_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    block->config = *config;

    ret = (DFBResult) CoreLayerContext_Call( obj, FCEF_NONE, CoreLayerContext_SetConfiguration, block, sizeof(CoreLayerContextSetConfiguration), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayerContext_Call( CoreLayerContext_SetConfiguration ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreLayerContext_SetConfiguration failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
ILayerContext_Requestor::GetPrimaryRegion(
                    bool                                       create,
                    CoreLayerRegion                          **ret_region
)
{
    DFBResult           ret;
    CoreLayerRegion *region = NULL;
    CoreLayerContextGetPrimaryRegion       *block = (CoreLayerContextGetPrimaryRegion*) alloca( sizeof(CoreLayerContextGetPrimaryRegion) );
    CoreLayerContextGetPrimaryRegionReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreLayerContext, "ILayerContext_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_region != NULL );

    block->create = create;

    ret = (DFBResult) CoreLayerContext_Call( obj, FCEF_NONE, CoreLayerContext_GetPrimaryRegion, block, sizeof(CoreLayerContextGetPrimaryRegion), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayerContext_Call( CoreLayerContext_GetPrimaryRegion ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreLayerContext_GetPrimaryRegion failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    ret = (DFBResult) CoreLayerRegion_Catch( core, return_block.region_id, &region );
    if (ret) {
         D_DERROR( ret, "%s: Catching region by ID %u failed!\n", __FUNCTION__, return_block.region_id );
         return ret;
    }

    *ret_region = region;

    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
CoreLayerContextDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreLayerContext, "CoreLayerContextDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case CoreLayerContext_CreateWindow: {
    CoreWindow *parent = NULL;
    CoreWindow *toplevel = NULL;
    CoreWindow *window = NULL;
            D_UNUSED
            CoreLayerContextCreateWindow       *args        = (CoreLayerContextCreateWindow *) ptr;
            CoreLayerContextCreateWindowReturn *return_args = (CoreLayerContextCreateWindowReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayerContext, "=-> CoreLayerContext_CreateWindow\n" );

            if (args->parent_set) {
                ret = (DFBResult) CoreWindow_Lookup( core, args->parent_id, caller, &parent );
                if (ret) {
                     D_DERROR( ret, "%s: Looking up parent by ID %u failed!\n", __FUNCTION__, args->parent_id );
                     return_args->result = ret;
                     return DFB_OK;
                }
            }

            if (args->toplevel_set) {
                ret = (DFBResult) CoreWindow_Lookup( core, args->toplevel_id, caller, &toplevel );
                if (ret) {
                     D_DERROR( ret, "%s: Looking up toplevel by ID %u failed!\n", __FUNCTION__, args->toplevel_id );
                     return_args->result = ret;
                     return DFB_OK;
                }
            }

            return_args->result = real->CreateWindow( &args->description, args->parent_set ? parent : NULL, args->toplevel_set ? toplevel : NULL, &window );
            if (return_args->result == DFB_OK) {
                CoreWindow_Throw( window, caller, &return_args->window_id );
            }

            *ret_length = sizeof(CoreLayerContextCreateWindowReturn);

            if (parent)
                CoreWindow_Unref( parent );

            if (toplevel)
                CoreWindow_Unref( toplevel );

            return DFB_OK;
        }

        case CoreLayerContext_SetConfiguration: {
            D_UNUSED
            CoreLayerContextSetConfiguration       *args        = (CoreLayerContextSetConfiguration *) ptr;
            CoreLayerContextSetConfigurationReturn *return_args = (CoreLayerContextSetConfigurationReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayerContext, "=-> CoreLayerContext_SetConfiguration\n" );

            return_args->result = real->SetConfiguration( &args->config );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreLayerContextSetConfigurationReturn);

            return DFB_OK;
        }

        case CoreLayerContext_GetPrimaryRegion: {
    CoreLayerRegion *region = NULL;
            D_UNUSED
            CoreLayerContextGetPrimaryRegion       *args        = (CoreLayerContextGetPrimaryRegion *) ptr;
            CoreLayerContextGetPrimaryRegionReturn *return_args = (CoreLayerContextGetPrimaryRegionReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayerContext, "=-> CoreLayerContext_GetPrimaryRegion\n" );

            return_args->result = real->GetPrimaryRegion( args->create, &region );
            if (return_args->result == DFB_OK) {
                CoreLayerRegion_Throw( region, caller, &return_args->region_id );
            }

            *ret_length = sizeof(CoreLayerContextGetPrimaryRegionReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
