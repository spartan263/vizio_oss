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

#include "CoreSurface.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreSurface, "DirectFB/CoreSurface", "DirectFB CoreSurface" );

/*********************************************************************************************************************/

DFBResult
CoreSurface_SetConfig(
                    CoreSurface                               *obj,
                    const CoreSurfaceConfig                   *config
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISurface_Real real( core_dfb, obj );

        return real.SetConfig( config );
    }

    DirectFB::ISurface_Requestor requestor( core_dfb, obj );

    return requestor.SetConfig( config );
}

DFBResult
CoreSurface_Flip(
                    CoreSurface                               *obj,
                    bool                                       swap
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISurface_Real real( core_dfb, obj );

        return real.Flip( swap );
    }

    DirectFB::ISurface_Requestor requestor( core_dfb, obj );

    return requestor.Flip( swap );
}

DFBResult
CoreSurface_GetPalette(
                    CoreSurface                               *obj,
                    CorePalette                              **ret_palette
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISurface_Real real( core_dfb, obj );

        return real.GetPalette( ret_palette );
    }

    DirectFB::ISurface_Requestor requestor( core_dfb, obj );

    return requestor.GetPalette( ret_palette );
}

DFBResult
CoreSurface_SetPalette(
                    CoreSurface                               *obj,
                    CorePalette                               *palette
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISurface_Real real( core_dfb, obj );

        return real.SetPalette( palette );
    }

    DirectFB::ISurface_Requestor requestor( core_dfb, obj );

    return requestor.SetPalette( palette );
}

DFBResult
CoreSurface_PreLockBuffer(
                    CoreSurface                               *obj,
                    u32                                        buffer_index,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    u32                                       *ret_allocation_index
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISurface_Real real( core_dfb, obj );

        return real.PreLockBuffer( buffer_index, accessor, access, ret_allocation_index );
    }

    DirectFB::ISurface_Requestor requestor( core_dfb, obj );

    return requestor.PreLockBuffer( buffer_index, accessor, access, ret_allocation_index );
}

DFBResult
CoreSurface_PreReadBuffer(
                    CoreSurface                               *obj,
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISurface_Real real( core_dfb, obj );

        return real.PreReadBuffer( buffer_index, rect, ret_allocation_index );
    }

    DirectFB::ISurface_Requestor requestor( core_dfb, obj );

    return requestor.PreReadBuffer( buffer_index, rect, ret_allocation_index );
}

DFBResult
CoreSurface_PreWriteBuffer(
                    CoreSurface                               *obj,
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISurface_Real real( core_dfb, obj );

        return real.PreWriteBuffer( buffer_index, rect, ret_allocation_index );
    }

    DirectFB::ISurface_Requestor requestor( core_dfb, obj );

    return requestor.PreWriteBuffer( buffer_index, rect, ret_allocation_index );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
CoreSurface_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::CoreSurfaceDispatch *dispatch = (DirectFB::CoreSurfaceDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *CoreSurface_Init_Dispatch(
                    CoreDFB              *core,
                    CoreSurface          *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreSurface_Dispatch, obj, core->world );
    return NULL;
}

void  CoreSurface_Deinit_Dispatch(FusionCall *call)
{
    fusion_call_destroy(call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ISurface_Requestor::SetConfig(
                    const CoreSurfaceConfig                   *config
)
{
    DFBResult           ret;
    CoreSurfaceSetConfig       *block = (CoreSurfaceSetConfig*) alloca( sizeof(CoreSurfaceSetConfig) );
    CoreSurfaceSetConfigReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreSurface, "ISurface_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    block->config = *config;

    ret = (DFBResult) CoreSurface_Call( obj, FCEF_NONE, CoreSurface_SetConfig, block, sizeof(CoreSurfaceSetConfig), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSurface_Call( CoreSurface_SetConfig ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreSurface_SetConfig failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
ISurface_Requestor::Flip(
                    bool                                       swap
)
{
    DFBResult           ret;
    CoreSurfaceFlip       *block = (CoreSurfaceFlip*) alloca( sizeof(CoreSurfaceFlip) );
    CoreSurfaceFlipReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreSurface, "ISurface_Requestor::%s()\n", __FUNCTION__ );


    block->swap = swap;

    ret = (DFBResult) CoreSurface_Call( obj, FCEF_NONE, CoreSurface_Flip, block, sizeof(CoreSurfaceFlip), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSurface_Call( CoreSurface_Flip ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreSurface_Flip failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
ISurface_Requestor::GetPalette(
                    CorePalette                              **ret_palette
)
{
    DFBResult           ret;
    CorePalette *palette = NULL;
    CoreSurfaceGetPalette       *block = (CoreSurfaceGetPalette*) alloca( sizeof(CoreSurfaceGetPalette) );
    CoreSurfaceGetPaletteReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreSurface, "ISurface_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_palette != NULL );


    ret = (DFBResult) CoreSurface_Call( obj, FCEF_NONE, CoreSurface_GetPalette, block, sizeof(CoreSurfaceGetPalette), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSurface_Call( CoreSurface_GetPalette ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreSurface_GetPalette failed!\n", __FUNCTION__ );
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
ISurface_Requestor::SetPalette(
                    CorePalette                               *palette
)
{
    DFBResult           ret;
    CoreSurfaceSetPalette       *block = (CoreSurfaceSetPalette*) alloca( sizeof(CoreSurfaceSetPalette) );
    CoreSurfaceSetPaletteReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreSurface, "ISurface_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( palette != NULL );

    block->palette_id = CorePalette_GetID( palette );

    ret = (DFBResult) CoreSurface_Call( obj, FCEF_NONE, CoreSurface_SetPalette, block, sizeof(CoreSurfaceSetPalette), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSurface_Call( CoreSurface_SetPalette ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreSurface_SetPalette failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
ISurface_Requestor::PreLockBuffer(
                    u32                                        buffer_index,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    u32                                       *ret_allocation_index
)
{
    DFBResult           ret;
    CoreSurfacePreLockBuffer       *block = (CoreSurfacePreLockBuffer*) alloca( sizeof(CoreSurfacePreLockBuffer) );
    CoreSurfacePreLockBufferReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreSurface, "ISurface_Requestor::%s()\n", __FUNCTION__ );


    block->buffer_index = buffer_index;
    block->accessor = accessor;
    block->access = access;

    ret = (DFBResult) CoreSurface_Call( obj, FCEF_NONE, CoreSurface_PreLockBuffer, block, sizeof(CoreSurfacePreLockBuffer), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSurface_Call( CoreSurface_PreLockBuffer ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreSurface_PreLockBuffer failed!\n", __FUNCTION__ );
         return return_block.result;
    }

    *ret_allocation_index = return_block.allocation_index;

    return DFB_OK;
}


DFBResult
ISurface_Requestor::PreReadBuffer(
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
)
{
    DFBResult           ret;
    CoreSurfacePreReadBuffer       *block = (CoreSurfacePreReadBuffer*) alloca( sizeof(CoreSurfacePreReadBuffer) );
    CoreSurfacePreReadBufferReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreSurface, "ISurface_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( rect != NULL );

    block->buffer_index = buffer_index;
    block->rect = *rect;

    ret = (DFBResult) CoreSurface_Call( obj, FCEF_NONE, CoreSurface_PreReadBuffer, block, sizeof(CoreSurfacePreReadBuffer), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSurface_Call( CoreSurface_PreReadBuffer ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreSurface_PreReadBuffer failed!\n", __FUNCTION__ );
         return return_block.result;
    }

    *ret_allocation_index = return_block.allocation_index;

    return DFB_OK;
}


DFBResult
ISurface_Requestor::PreWriteBuffer(
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
)
{
    DFBResult           ret;
    CoreSurfacePreWriteBuffer       *block = (CoreSurfacePreWriteBuffer*) alloca( sizeof(CoreSurfacePreWriteBuffer) );
    CoreSurfacePreWriteBufferReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreSurface, "ISurface_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( rect != NULL );

    block->buffer_index = buffer_index;
    block->rect = *rect;

    ret = (DFBResult) CoreSurface_Call( obj, FCEF_NONE, CoreSurface_PreWriteBuffer, block, sizeof(CoreSurfacePreWriteBuffer), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSurface_Call( CoreSurface_PreWriteBuffer ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreSurface_PreWriteBuffer failed!\n", __FUNCTION__ );
         return return_block.result;
    }

    *ret_allocation_index = return_block.allocation_index;

    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
CoreSurfaceDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreSurface, "CoreSurfaceDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case CoreSurface_SetConfig: {
            D_UNUSED
            CoreSurfaceSetConfig       *args        = (CoreSurfaceSetConfig *) ptr;
            CoreSurfaceSetConfigReturn *return_args = (CoreSurfaceSetConfigReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSurface, "=-> CoreSurface_SetConfig\n" );

            return_args->result = real->SetConfig( &args->config );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreSurfaceSetConfigReturn);

            return DFB_OK;
        }

        case CoreSurface_Flip: {
            D_UNUSED
            CoreSurfaceFlip       *args        = (CoreSurfaceFlip *) ptr;
            CoreSurfaceFlipReturn *return_args = (CoreSurfaceFlipReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSurface, "=-> CoreSurface_Flip\n" );

            return_args->result = real->Flip( args->swap );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreSurfaceFlipReturn);

            return DFB_OK;
        }

        case CoreSurface_GetPalette: {
    CorePalette *palette = NULL;
            D_UNUSED
            CoreSurfaceGetPalette       *args        = (CoreSurfaceGetPalette *) ptr;
            CoreSurfaceGetPaletteReturn *return_args = (CoreSurfaceGetPaletteReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSurface, "=-> CoreSurface_GetPalette\n" );

            return_args->result = real->GetPalette( &palette );
            if (return_args->result == DFB_OK) {
                CorePalette_Throw( palette, caller, &return_args->palette_id );
            }

            *ret_length = sizeof(CoreSurfaceGetPaletteReturn);

            return DFB_OK;
        }

        case CoreSurface_SetPalette: {
    CorePalette *palette = NULL;
            D_UNUSED
            CoreSurfaceSetPalette       *args        = (CoreSurfaceSetPalette *) ptr;
            CoreSurfaceSetPaletteReturn *return_args = (CoreSurfaceSetPaletteReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSurface, "=-> CoreSurface_SetPalette\n" );

            ret = (DFBResult) CorePalette_Lookup( core, args->palette_id, caller, &palette );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up palette by ID %u failed!\n", __FUNCTION__, args->palette_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real->SetPalette( palette );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreSurfaceSetPaletteReturn);

            if (palette)
                CorePalette_Unref( palette );

            return DFB_OK;
        }

        case CoreSurface_PreLockBuffer: {
            D_UNUSED
            CoreSurfacePreLockBuffer       *args        = (CoreSurfacePreLockBuffer *) ptr;
            CoreSurfacePreLockBufferReturn *return_args = (CoreSurfacePreLockBufferReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSurface, "=-> CoreSurface_PreLockBuffer\n" );

            return_args->result = real->PreLockBuffer( args->buffer_index, args->accessor, args->access, &return_args->allocation_index );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreSurfacePreLockBufferReturn);

            return DFB_OK;
        }

        case CoreSurface_PreReadBuffer: {
            D_UNUSED
            CoreSurfacePreReadBuffer       *args        = (CoreSurfacePreReadBuffer *) ptr;
            CoreSurfacePreReadBufferReturn *return_args = (CoreSurfacePreReadBufferReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSurface, "=-> CoreSurface_PreReadBuffer\n" );

            return_args->result = real->PreReadBuffer( args->buffer_index, &args->rect, &return_args->allocation_index );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreSurfacePreReadBufferReturn);

            return DFB_OK;
        }

        case CoreSurface_PreWriteBuffer: {
            D_UNUSED
            CoreSurfacePreWriteBuffer       *args        = (CoreSurfacePreWriteBuffer *) ptr;
            CoreSurfacePreWriteBufferReturn *return_args = (CoreSurfacePreWriteBufferReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSurface, "=-> CoreSurface_PreWriteBuffer\n" );

            return_args->result = real->PreWriteBuffer( args->buffer_index, &args->rect, &return_args->allocation_index );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreSurfacePreWriteBufferReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
