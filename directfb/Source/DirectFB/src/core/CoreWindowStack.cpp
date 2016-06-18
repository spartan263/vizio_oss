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

#include "CoreWindowStack.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreWindowStack, "DirectFB/CoreWindowStack", "DirectFB CoreWindowStack" );

/*********************************************************************************************************************/

DFBResult
CoreWindowStack_RepaintAll(
                    CoreWindowStack                           *obj

)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.RepaintAll(  );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.RepaintAll(  );
}

DFBResult
CoreWindowStack_CursorEnable(
                    CoreWindowStack                           *obj,
                    bool                                       enable
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.CursorEnable( enable );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.CursorEnable( enable );
}

DFBResult
CoreWindowStack_CursorSetShape(
                    CoreWindowStack                           *obj,
                    CoreSurface                               *shape,
                    const DFBPoint                            *hotspot
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.CursorSetShape( shape, hotspot );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.CursorSetShape( shape, hotspot );
}

DFBResult
CoreWindowStack_CursorSetOpacity(
                    CoreWindowStack                           *obj,
                    u8                                         opacity
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.CursorSetOpacity( opacity );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.CursorSetOpacity( opacity );
}

DFBResult
CoreWindowStack_CursorSetAcceleration(
                    CoreWindowStack                           *obj,
                    u32                                        numerator,
                    u32                                        denominator,
                    u32                                        threshold
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.CursorSetAcceleration( numerator, denominator, threshold );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.CursorSetAcceleration( numerator, denominator, threshold );
}

DFBResult
CoreWindowStack_CursorWarp(
                    CoreWindowStack                           *obj,
                    const DFBPoint                            *position
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.CursorWarp( position );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.CursorWarp( position );
}

DFBResult
CoreWindowStack_CursorGetPosition(
                    CoreWindowStack                           *obj,
                    DFBPoint                                  *ret_position
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.CursorGetPosition( ret_position );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.CursorGetPosition( ret_position );
}

DFBResult
CoreWindowStack_BackgroundSetMode(
                    CoreWindowStack                           *obj,
                    DFBDisplayLayerBackgroundMode              mode
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.BackgroundSetMode( mode );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.BackgroundSetMode( mode );
}

DFBResult
CoreWindowStack_BackgroundSetImage(
                    CoreWindowStack                           *obj,
                    CoreSurface                               *image
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.BackgroundSetImage( image );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.BackgroundSetImage( image );
}

DFBResult
CoreWindowStack_BackgroundSetColor(
                    CoreWindowStack                           *obj,
                    const DFBColor                            *color
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.BackgroundSetColor( color );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.BackgroundSetColor( color );
}

DFBResult
CoreWindowStack_BackgroundSetColorIndex(
                    CoreWindowStack                           *obj,
                    s32                                        index
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindowStack_Real real( core_dfb, obj );

        return real.BackgroundSetColorIndex( index );
    }

    DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

    return requestor.BackgroundSetColorIndex( index );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
CoreWindowStack_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::CoreWindowStackDispatch *dispatch = (DirectFB::CoreWindowStackDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *CoreWindowStack_Init_Dispatch(
                    CoreDFB              *core,
                    CoreWindowStack      *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreWindowStack_Dispatch, obj, core->world );

    return NULL;
}

void  CoreWindowStack_Deinit_Dispatch(FusionCall *call)
{
    fusion_call_destroy( call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
IWindowStack_Requestor::RepaintAll(

)
{
    DFBResult           ret;
    CoreWindowStackRepaintAll       *block = (CoreWindowStackRepaintAll*) alloca( sizeof(CoreWindowStackRepaintAll) );
    CoreWindowStackRepaintAllReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_RepaintAll, block, sizeof(CoreWindowStackRepaintAll), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_RepaintAll ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_RepaintAll failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorEnable(
                    bool                                       enable
)
{
    DFBResult           ret;
    CoreWindowStackCursorEnable       *block = (CoreWindowStackCursorEnable*) alloca( sizeof(CoreWindowStackCursorEnable) );
    CoreWindowStackCursorEnableReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    block->enable = enable;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorEnable, block, sizeof(CoreWindowStackCursorEnable), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorEnable ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_CursorEnable failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorSetShape(
                    CoreSurface                               *shape,
                    const DFBPoint                            *hotspot
)
{
    DFBResult           ret;
    CoreWindowStackCursorSetShape       *block = (CoreWindowStackCursorSetShape*) alloca( sizeof(CoreWindowStackCursorSetShape) );
    CoreWindowStackCursorSetShapeReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( shape != NULL );
    D_ASSERT( hotspot != NULL );

    block->shape_id = CoreSurface_GetID( shape );
    block->hotspot = *hotspot;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorSetShape, block, sizeof(CoreWindowStackCursorSetShape), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorSetShape ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_CursorSetShape failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorSetOpacity(
                    u8                                         opacity
)
{
    DFBResult           ret;
    CoreWindowStackCursorSetOpacity       *block = (CoreWindowStackCursorSetOpacity*) alloca( sizeof(CoreWindowStackCursorSetOpacity) );
    CoreWindowStackCursorSetOpacityReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    block->opacity = opacity;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorSetOpacity, block, sizeof(CoreWindowStackCursorSetOpacity), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorSetOpacity ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_CursorSetOpacity failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorSetAcceleration(
                    u32                                        numerator,
                    u32                                        denominator,
                    u32                                        threshold
)
{
    DFBResult           ret;
    CoreWindowStackCursorSetAcceleration       *block = (CoreWindowStackCursorSetAcceleration*) alloca( sizeof(CoreWindowStackCursorSetAcceleration) );
    CoreWindowStackCursorSetAccelerationReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    block->numerator = numerator;
    block->denominator = denominator;
    block->threshold = threshold;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorSetAcceleration, block, sizeof(CoreWindowStackCursorSetAcceleration), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorSetAcceleration ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_CursorSetAcceleration failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorWarp(
                    const DFBPoint                            *position
)
{
    DFBResult           ret;
    CoreWindowStackCursorWarp       *block = (CoreWindowStackCursorWarp*) alloca( sizeof(CoreWindowStackCursorWarp) );
    CoreWindowStackCursorWarpReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( position != NULL );

    block->position = *position;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorWarp, block, sizeof(CoreWindowStackCursorWarp), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorWarp ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_CursorWarp failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorGetPosition(
                    DFBPoint                                  *ret_position
)
{
    DFBResult           ret;
    CoreWindowStackCursorGetPosition       *block = (CoreWindowStackCursorGetPosition*) alloca( sizeof(CoreWindowStackCursorGetPosition) );
    CoreWindowStackCursorGetPositionReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_position != NULL );


    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorGetPosition, block, sizeof(CoreWindowStackCursorGetPosition), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorGetPosition ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_CursorGetPosition failed!\n", __FUNCTION__ );
         return return_block.result;
    }

    *ret_position = return_block.position;

    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::BackgroundSetMode(
                    DFBDisplayLayerBackgroundMode              mode
)
{
    DFBResult           ret;
    CoreWindowStackBackgroundSetMode       *block = (CoreWindowStackBackgroundSetMode*) alloca( sizeof(CoreWindowStackBackgroundSetMode) );
    CoreWindowStackBackgroundSetModeReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    block->mode = mode;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_BackgroundSetMode, block, sizeof(CoreWindowStackBackgroundSetMode), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_BackgroundSetMode ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_BackgroundSetMode failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::BackgroundSetImage(
                    CoreSurface                               *image
)
{
    DFBResult           ret;
    CoreWindowStackBackgroundSetImage       *block = (CoreWindowStackBackgroundSetImage*) alloca( sizeof(CoreWindowStackBackgroundSetImage) );
    CoreWindowStackBackgroundSetImageReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( image != NULL );

    block->image_id = CoreSurface_GetID( image );

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_BackgroundSetImage, block, sizeof(CoreWindowStackBackgroundSetImage), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_BackgroundSetImage ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_BackgroundSetImage failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::BackgroundSetColor(
                    const DFBColor                            *color
)
{
    DFBResult           ret;
    CoreWindowStackBackgroundSetColor       *block = (CoreWindowStackBackgroundSetColor*) alloca( sizeof(CoreWindowStackBackgroundSetColor) );
    CoreWindowStackBackgroundSetColorReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( color != NULL );

    block->color = *color;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_BackgroundSetColor, block, sizeof(CoreWindowStackBackgroundSetColor), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_BackgroundSetColor ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_BackgroundSetColor failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::BackgroundSetColorIndex(
                    s32                                        index
)
{
    DFBResult           ret;
    CoreWindowStackBackgroundSetColorIndex       *block = (CoreWindowStackBackgroundSetColorIndex*) alloca( sizeof(CoreWindowStackBackgroundSetColorIndex) );
    CoreWindowStackBackgroundSetColorIndexReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    block->index = index;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_BackgroundSetColorIndex, block, sizeof(CoreWindowStackBackgroundSetColorIndex), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_BackgroundSetColorIndex ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindowStack_BackgroundSetColorIndex failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
CoreWindowStackDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "CoreWindowStackDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case CoreWindowStack_RepaintAll: {
            D_UNUSED
            CoreWindowStackRepaintAll       *args        = (CoreWindowStackRepaintAll *) ptr;
            CoreWindowStackRepaintAllReturn *return_args = (CoreWindowStackRepaintAllReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_RepaintAll\n" );

            return_args->result = real->RepaintAll(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackRepaintAllReturn);

            return DFB_OK;
        }

        case CoreWindowStack_CursorEnable: {
            D_UNUSED
            CoreWindowStackCursorEnable       *args        = (CoreWindowStackCursorEnable *) ptr;
            CoreWindowStackCursorEnableReturn *return_args = (CoreWindowStackCursorEnableReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorEnable\n" );

            return_args->result = real->CursorEnable( args->enable );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorEnableReturn);

            return DFB_OK;
        }

        case CoreWindowStack_CursorSetShape: {
    CoreSurface *shape = NULL;
            D_UNUSED
            CoreWindowStackCursorSetShape       *args        = (CoreWindowStackCursorSetShape *) ptr;
            CoreWindowStackCursorSetShapeReturn *return_args = (CoreWindowStackCursorSetShapeReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorSetShape\n" );

            ret = (DFBResult) CoreSurface_Lookup( core, args->shape_id, caller, &shape );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up shape by ID %u failed!\n", __FUNCTION__, args->shape_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real->CursorSetShape( shape, &args->hotspot );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorSetShapeReturn);

            if (shape)
                CoreSurface_Unref( shape );

            return DFB_OK;
        }

        case CoreWindowStack_CursorSetOpacity: {
            D_UNUSED
            CoreWindowStackCursorSetOpacity       *args        = (CoreWindowStackCursorSetOpacity *) ptr;
            CoreWindowStackCursorSetOpacityReturn *return_args = (CoreWindowStackCursorSetOpacityReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorSetOpacity\n" );

            return_args->result = real->CursorSetOpacity( args->opacity );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorSetOpacityReturn);

            return DFB_OK;
        }

        case CoreWindowStack_CursorSetAcceleration: {
            D_UNUSED
            CoreWindowStackCursorSetAcceleration       *args        = (CoreWindowStackCursorSetAcceleration *) ptr;
            CoreWindowStackCursorSetAccelerationReturn *return_args = (CoreWindowStackCursorSetAccelerationReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorSetAcceleration\n" );

            return_args->result = real->CursorSetAcceleration( args->numerator, args->denominator, args->threshold );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorSetAccelerationReturn);

            return DFB_OK;
        }

        case CoreWindowStack_CursorWarp: {
            D_UNUSED
            CoreWindowStackCursorWarp       *args        = (CoreWindowStackCursorWarp *) ptr;
            CoreWindowStackCursorWarpReturn *return_args = (CoreWindowStackCursorWarpReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorWarp\n" );

            return_args->result = real->CursorWarp( &args->position );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorWarpReturn);

            return DFB_OK;
        }

        case CoreWindowStack_CursorGetPosition: {
            D_UNUSED
            CoreWindowStackCursorGetPosition       *args        = (CoreWindowStackCursorGetPosition *) ptr;
            CoreWindowStackCursorGetPositionReturn *return_args = (CoreWindowStackCursorGetPositionReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorGetPosition\n" );

            return_args->result = real->CursorGetPosition( &return_args->position );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorGetPositionReturn);

            return DFB_OK;
        }

        case CoreWindowStack_BackgroundSetMode: {
            D_UNUSED
            CoreWindowStackBackgroundSetMode       *args        = (CoreWindowStackBackgroundSetMode *) ptr;
            CoreWindowStackBackgroundSetModeReturn *return_args = (CoreWindowStackBackgroundSetModeReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_BackgroundSetMode\n" );

            return_args->result = real->BackgroundSetMode( args->mode );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackBackgroundSetModeReturn);

            return DFB_OK;
        }

        case CoreWindowStack_BackgroundSetImage: {
    CoreSurface *image = NULL;
            D_UNUSED
            CoreWindowStackBackgroundSetImage       *args        = (CoreWindowStackBackgroundSetImage *) ptr;
            CoreWindowStackBackgroundSetImageReturn *return_args = (CoreWindowStackBackgroundSetImageReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_BackgroundSetImage\n" );

            ret = (DFBResult) CoreSurface_Lookup( core, args->image_id, caller, &image );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up image by ID %u failed!\n", __FUNCTION__, args->image_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real->BackgroundSetImage( image );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackBackgroundSetImageReturn);

            if (image)
                CoreSurface_Unref( image );

            return DFB_OK;
        }

        case CoreWindowStack_BackgroundSetColor: {
            D_UNUSED
            CoreWindowStackBackgroundSetColor       *args        = (CoreWindowStackBackgroundSetColor *) ptr;
            CoreWindowStackBackgroundSetColorReturn *return_args = (CoreWindowStackBackgroundSetColorReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_BackgroundSetColor\n" );

            return_args->result = real->BackgroundSetColor( &args->color );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackBackgroundSetColorReturn);

            return DFB_OK;
        }

        case CoreWindowStack_BackgroundSetColorIndex: {
            D_UNUSED
            CoreWindowStackBackgroundSetColorIndex       *args        = (CoreWindowStackBackgroundSetColorIndex *) ptr;
            CoreWindowStackBackgroundSetColorIndexReturn *return_args = (CoreWindowStackBackgroundSetColorIndexReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_BackgroundSetColorIndex\n" );

            return_args->result = real->BackgroundSetColorIndex( args->index );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackBackgroundSetColorIndexReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
