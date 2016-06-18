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

#include "CoreDFB.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreDFB, "DirectFB/CoreDFB", "DirectFB CoreDFB" );

/*********************************************************************************************************************/

DFBResult
CoreDFB_CreateSurface(
                    CoreDFB                                   *obj,
                    const CoreSurfaceConfig                   *config,
                    CoreSurfaceTypeFlags                       type,
                    u64                                        resource_id,
                    CorePalette                               *palette,
                    CoreSurface                              **ret_surface
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ICore_Real real( core_dfb, obj );

        return real.CreateSurface( config, type, resource_id, palette, ret_surface );
    }

    DirectFB::ICore_Requestor requestor( core_dfb, obj );

    return requestor.CreateSurface( config, type, resource_id, palette, ret_surface );
}

DFBResult
CoreDFB_CreatePalette(
                    CoreDFB                                   *obj,
                    u32                                        size,
                    CorePalette                              **ret_palette
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ICore_Real real( core_dfb, obj );

        return real.CreatePalette( size, ret_palette );
    }

    DirectFB::ICore_Requestor requestor( core_dfb, obj );

    return requestor.CreatePalette( size, ret_palette );
}

DFBResult
CoreDFB_CreateState(
                    CoreDFB                                   *obj,
                    CoreGraphicsState                        **ret_state
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ICore_Real real( core_dfb, obj );

        return real.CreateState( ret_state );
    }

    DirectFB::ICore_Requestor requestor( core_dfb, obj );

    return requestor.CreateState( ret_state );
}

DFBResult
CoreDFB_WaitIdle(
                    CoreDFB                                   *obj

)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ICore_Real real( core_dfb, obj );

        return real.WaitIdle(  );
    }

    DirectFB::ICore_Requestor requestor( core_dfb, obj );

    return requestor.WaitIdle(  );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
CoreDFB_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::CoreDFBDispatch *dispatch = (DirectFB::CoreDFBDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *CoreDFB_Init_Dispatch(
                    CoreDFB              *core,
                    CoreDFB              *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreDFB_Dispatch, obj, core->world );

    return NULL;
}

void  CoreDFB_Deinit_Dispatch(FusionCall * call)
{
    fusion_call_destroy( call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ICore_Requestor::CreateSurface(
                    const CoreSurfaceConfig                   *config,
                    CoreSurfaceTypeFlags                       type,
                    u64                                        resource_id,
                    CorePalette                               *palette,
                    CoreSurface                              **ret_surface
)
{
    DFBResult           ret;
    CoreSurface *surface = NULL;
    CoreDFBCreateSurface       *block = (CoreDFBCreateSurface*) alloca( sizeof(CoreDFBCreateSurface) );
    CoreDFBCreateSurfaceReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreDFB, "ICore_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );
    D_ASSERT( ret_surface != NULL );

    block->config = *config;
    block->type = type;
    block->resource_id = resource_id;
  if (palette) {
    block->palette_id = CorePalette_GetID( palette );
    block->palette_set = true;
  }
  else
    block->palette_set = false;

    ret = (DFBResult) CoreDFB_Call( obj, FCEF_NONE, CoreDFB_CreateSurface, block, sizeof(CoreDFBCreateSurface), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreDFB_Call( CoreDFB_CreateSurface ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreDFB_CreateSurface failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    ret = (DFBResult) CoreSurface_Catch( core, return_block.surface_id, &surface );
    if (ret) {
         D_DERROR( ret, "%s: Catching surface by ID %u failed!\n", __FUNCTION__, return_block.surface_id );
         return ret;
    }

    *ret_surface = surface;

    return DFB_OK;
}


DFBResult
ICore_Requestor::CreatePalette(
                    u32                                        size,
                    CorePalette                              **ret_palette
)
{
    DFBResult           ret;
    CorePalette *palette = NULL;
    CoreDFBCreatePalette       *block = (CoreDFBCreatePalette*) alloca( sizeof(CoreDFBCreatePalette) );
    CoreDFBCreatePaletteReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreDFB, "ICore_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_palette != NULL );

    block->size = size;

    ret = (DFBResult) CoreDFB_Call( obj, FCEF_NONE, CoreDFB_CreatePalette, block, sizeof(CoreDFBCreatePalette), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreDFB_Call( CoreDFB_CreatePalette ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreDFB_CreatePalette failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    ret = (DFBResult) CorePalette_Catch( core, return_block.palette_id, &palette );
    if (ret) {
         D_DERROR( ret, "%s: Catching palette by ID %u failed!\n", __FUNCTION__, return_block.palette_id );
         return ret;
    }

    *ret_palette = palette;

    return DFB_OK;
}


DFBResult
ICore_Requestor::CreateState(
                    CoreGraphicsState                        **ret_state
)
{
    DFBResult           ret;
    CoreGraphicsState *state = NULL;
    CoreDFBCreateState       *block = (CoreDFBCreateState*) alloca( sizeof(CoreDFBCreateState) );
    CoreDFBCreateStateReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreDFB, "ICore_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_state != NULL );


    ret = (DFBResult) CoreDFB_Call( obj, FCEF_NONE, CoreDFB_CreateState, block, sizeof(CoreDFBCreateState), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreDFB_Call( CoreDFB_CreateState ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreDFB_CreateState failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    ret = (DFBResult) CoreGraphicsState_Catch( core, return_block.state_id, &state );
    if (ret) {
         D_DERROR( ret, "%s: Catching state by ID %u failed!\n", __FUNCTION__, return_block.state_id );
         return ret;
    }

    *ret_state = state;

    return DFB_OK;
}


DFBResult
ICore_Requestor::WaitIdle(

)
{
    DFBResult           ret;
    CoreDFBWaitIdle       *block = (CoreDFBWaitIdle*) alloca( sizeof(CoreDFBWaitIdle) );
    CoreDFBWaitIdleReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreDFB, "ICore_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) CoreDFB_Call( obj, FCEF_NONE, CoreDFB_WaitIdle, block, sizeof(CoreDFBWaitIdle), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreDFB_Call( CoreDFB_WaitIdle ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreDFB_WaitIdle failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
CoreDFBDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreDFB, "CoreDFBDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case CoreDFB_CreateSurface: {
    CorePalette *palette = NULL;
    CoreSurface *surface = NULL;
            D_UNUSED
            CoreDFBCreateSurface       *args        = (CoreDFBCreateSurface *) ptr;
            CoreDFBCreateSurfaceReturn *return_args = (CoreDFBCreateSurfaceReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreDFB, "=-> CoreDFB_CreateSurface\n" );

            if (args->palette_set) {
                ret = (DFBResult) CorePalette_Lookup( core, args->palette_id, caller, &palette );
                if (ret) {
                     D_DERROR( ret, "%s: Looking up palette by ID %u failed!\n", __FUNCTION__, args->palette_id );
                     return_args->result = ret;
                     return DFB_OK;
                }
            }

            return_args->result = real->CreateSurface( &args->config, args->type, args->resource_id, args->palette_set ? palette : NULL, &surface );
            if (return_args->result == DFB_OK) {
                CoreSurface_Throw( surface, caller, &return_args->surface_id );
            }

            *ret_length = sizeof(CoreDFBCreateSurfaceReturn);

            if (palette)
                CorePalette_Unref( palette );

            return DFB_OK;
        }

        case CoreDFB_CreatePalette: {
    CorePalette *palette = NULL;
            D_UNUSED
            CoreDFBCreatePalette       *args        = (CoreDFBCreatePalette *) ptr;
            CoreDFBCreatePaletteReturn *return_args = (CoreDFBCreatePaletteReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreDFB, "=-> CoreDFB_CreatePalette\n" );

            return_args->result = real->CreatePalette( args->size, &palette );
            if (return_args->result == DFB_OK) {
                CorePalette_Throw( palette, caller, &return_args->palette_id );
            }

            *ret_length = sizeof(CoreDFBCreatePaletteReturn);

            return DFB_OK;
        }

        case CoreDFB_CreateState: {
    CoreGraphicsState *state = NULL;
            D_UNUSED
            CoreDFBCreateState       *args        = (CoreDFBCreateState *) ptr;
            CoreDFBCreateStateReturn *return_args = (CoreDFBCreateStateReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreDFB, "=-> CoreDFB_CreateState\n" );

            return_args->result = real->CreateState( &state );
            if (return_args->result == DFB_OK) {
                CoreGraphicsState_Throw( state, caller, &return_args->state_id );
            }

            *ret_length = sizeof(CoreDFBCreateStateReturn);

            return DFB_OK;
        }

        case CoreDFB_WaitIdle: {
            D_UNUSED
            CoreDFBWaitIdle       *args        = (CoreDFBWaitIdle *) ptr;
            CoreDFBWaitIdleReturn *return_args = (CoreDFBWaitIdleReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreDFB, "=-> CoreDFB_WaitIdle\n" );

            return_args->result = real->WaitIdle(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreDFBWaitIdleReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
