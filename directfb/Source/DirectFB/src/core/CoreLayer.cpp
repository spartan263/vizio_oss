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

#include "CoreLayer.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreLayer, "DirectFB/CoreLayer", "DirectFB CoreLayer" );

/*********************************************************************************************************************/

DFBResult
CoreLayer_CreateContext(
                    CoreLayer                                 *obj,
                    CoreLayerContext                         **ret_context
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ILayer_Real real( core_dfb, obj );

        return real.CreateContext( ret_context );
    }

    DirectFB::ILayer_Requestor requestor( core_dfb, obj );

    return requestor.CreateContext( ret_context );
}

DFBResult
CoreLayer_ActivateContext(
                    CoreLayer                                 *obj,
                    CoreLayerContext                          *context
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ILayer_Real real( core_dfb, obj );

        return real.ActivateContext( context );
    }

    DirectFB::ILayer_Requestor requestor( core_dfb, obj );

    return requestor.ActivateContext( context );
}

DFBResult
CoreLayer_GetPrimaryContext(
                    CoreLayer                                 *obj,
                    bool                                       activate,
                    CoreLayerContext                         **ret_context
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ILayer_Real real( core_dfb, obj );

        return real.GetPrimaryContext( activate, ret_context );
    }

    DirectFB::ILayer_Requestor requestor( core_dfb, obj );

    return requestor.GetPrimaryContext( activate, ret_context );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
CoreLayer_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::CoreLayerDispatch *dispatch = (DirectFB::CoreLayerDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *CoreLayer_Init_Dispatch(
                    CoreDFB              *core,
                    CoreLayer            *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreLayer_Dispatch, obj, core->world );

    return NULL;
}

void  CoreLayer_Deinit_Dispatch(FusionCall * call)
{
    fusion_call_destroy(call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ILayer_Requestor::CreateContext(
                    CoreLayerContext                         **ret_context
)
{
    DFBResult           ret;
    CoreLayerContext *context = NULL;
    CoreLayerCreateContext       *block = (CoreLayerCreateContext*) alloca( sizeof(CoreLayerCreateContext) );
    CoreLayerCreateContextReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreLayer, "ILayer_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_context != NULL );


    ret = (DFBResult) CoreLayer_Call( obj, FCEF_NONE, CoreLayer_CreateContext, block, sizeof(CoreLayerCreateContext), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayer_Call( CoreLayer_CreateContext ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreLayer_CreateContext failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    ret = (DFBResult) CoreLayerContext_Catch( core, return_block.context_id, &context );
    if (ret) {
         D_DERROR( ret, "%s: Catching context by ID %u failed!\n", __FUNCTION__, return_block.context_id );
         return ret;
    }

    *ret_context = context;

    return DFB_OK;
}


DFBResult
ILayer_Requestor::ActivateContext(
                    CoreLayerContext                          *context
)
{
    DFBResult           ret;
    CoreLayerActivateContext       *block = (CoreLayerActivateContext*) alloca( sizeof(CoreLayerActivateContext) );
    CoreLayerActivateContextReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreLayer, "ILayer_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( context != NULL );

    block->context_id = CoreLayerContext_GetID( context );

    ret = (DFBResult) CoreLayer_Call( obj, FCEF_NONE, CoreLayer_ActivateContext, block, sizeof(CoreLayerActivateContext), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayer_Call( CoreLayer_ActivateContext ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreLayer_ActivateContext failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
ILayer_Requestor::GetPrimaryContext(
                    bool                                       activate,
                    CoreLayerContext                         **ret_context
)
{
    DFBResult           ret;
    CoreLayerContext *context = NULL;
    CoreLayerGetPrimaryContext       *block = (CoreLayerGetPrimaryContext*) alloca( sizeof(CoreLayerGetPrimaryContext) );
    CoreLayerGetPrimaryContextReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreLayer, "ILayer_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_context != NULL );

    block->activate = activate;

    ret = (DFBResult) CoreLayer_Call( obj, FCEF_NONE, CoreLayer_GetPrimaryContext, block, sizeof(CoreLayerGetPrimaryContext), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayer_Call( CoreLayer_GetPrimaryContext ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreLayer_GetPrimaryContext failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    ret = (DFBResult) CoreLayerContext_Catch( core, return_block.context_id, &context );
    if (ret) {
         D_DERROR( ret, "%s: Catching context by ID %u failed!\n", __FUNCTION__, return_block.context_id );
         return ret;
    }

    *ret_context = context;

    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
CoreLayerDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreLayer, "CoreLayerDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case CoreLayer_CreateContext: {
    CoreLayerContext *context = NULL;
            D_UNUSED
            CoreLayerCreateContext       *args        = (CoreLayerCreateContext *) ptr;
            CoreLayerCreateContextReturn *return_args = (CoreLayerCreateContextReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayer, "=-> CoreLayer_CreateContext\n" );

            return_args->result = real->CreateContext( &context );
            if (return_args->result == DFB_OK) {
                CoreLayerContext_Throw( context, caller, &return_args->context_id );
            }

            *ret_length = sizeof(CoreLayerCreateContextReturn);

            return DFB_OK;
        }

        case CoreLayer_ActivateContext: {
    CoreLayerContext *context = NULL;
            D_UNUSED
            CoreLayerActivateContext       *args        = (CoreLayerActivateContext *) ptr;
            CoreLayerActivateContextReturn *return_args = (CoreLayerActivateContextReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayer, "=-> CoreLayer_ActivateContext\n" );

            ret = (DFBResult) CoreLayerContext_Lookup( core, args->context_id, caller, &context );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up context by ID %u failed!\n", __FUNCTION__, args->context_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real->ActivateContext( context );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreLayerActivateContextReturn);

            if (context)
                CoreLayerContext_Unref( context );

            return DFB_OK;
        }

        case CoreLayer_GetPrimaryContext: {
    CoreLayerContext *context = NULL;
            D_UNUSED
            CoreLayerGetPrimaryContext       *args        = (CoreLayerGetPrimaryContext *) ptr;
            CoreLayerGetPrimaryContextReturn *return_args = (CoreLayerGetPrimaryContextReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayer, "=-> CoreLayer_GetPrimaryContext\n" );

            return_args->result = real->GetPrimaryContext( args->activate, &context );
            if (return_args->result == DFB_OK) {
                CoreLayerContext_Throw( context, caller, &return_args->context_id );
            }

            *ret_length = sizeof(CoreLayerGetPrimaryContextReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
