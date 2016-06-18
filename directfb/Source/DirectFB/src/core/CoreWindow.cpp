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

#include "CoreWindow.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreWindow, "DirectFB/CoreWindow", "DirectFB CoreWindow" );

/*********************************************************************************************************************/

DFBResult
CoreWindow_Repaint(
                    CoreWindow                                *obj,
                    const DFBRegion                           *left,
                    const DFBRegion                           *right,
                    DFBSurfaceFlipFlags                        flags
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.Repaint( left, right, flags );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.Repaint( left, right, flags );
}

DFBResult
CoreWindow_BeginUpdates(
                    CoreWindow                                *obj,
                    const DFBRegion                           *update
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.BeginUpdates( update );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.BeginUpdates( update );
}

DFBResult
CoreWindow_Restack(
                    CoreWindow                                *obj,
                    CoreWindow                                *relative,
                    int                                        relation
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.Restack( relative, relation );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.Restack( relative, relation );
}

DFBResult
CoreWindow_SetConfig(
                    CoreWindow                                *obj,
                    const CoreWindowConfig                    *config,
                    const DFBInputDeviceKeySymbol             *keys,
                    u32                                        num_keys,
                    CoreWindow                                *parent,
                    CoreWindowConfigFlags                      flags
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetConfig( config, keys, num_keys, parent, flags );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetConfig( config, keys, num_keys, parent, flags );
}

DFBResult
CoreWindow_Bind(
                    CoreWindow                                *obj,
                    CoreWindow                                *source,
                    int                                        x,
                    int                                        y
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.Bind( source, x, y );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.Bind( source, x, y );
}

DFBResult
CoreWindow_Unbind(
                    CoreWindow                                *obj,
                    CoreWindow                                *source
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.Unbind( source );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.Unbind( source );
}

DFBResult
CoreWindow_RequestFocus(
                    CoreWindow                                *obj

)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.RequestFocus(  );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.RequestFocus(  );
}

DFBResult
CoreWindow_ChangeGrab(
                    CoreWindow                                *obj,
                    CoreWMGrabTarget                           target,
                    bool                                       grab
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.ChangeGrab( target, grab );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.ChangeGrab( target, grab );
}

DFBResult
CoreWindow_GrabKey(
                    CoreWindow                                *obj,
                    DFBInputDeviceKeySymbol                    symbol,
                    DFBInputDeviceModifierMask                 modifiers
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.GrabKey( symbol, modifiers );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.GrabKey( symbol, modifiers );
}

DFBResult
CoreWindow_UngrabKey(
                    CoreWindow                                *obj,
                    DFBInputDeviceKeySymbol                    symbol,
                    DFBInputDeviceModifierMask                 modifiers
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.UngrabKey( symbol, modifiers );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.UngrabKey( symbol, modifiers );
}

DFBResult
CoreWindow_Move(
                    CoreWindow                                *obj,
                    int                                        dx,
                    int                                        dy
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.Move( dx, dy );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.Move( dx, dy );
}

DFBResult
CoreWindow_MoveTo(
                    CoreWindow                                *obj,
                    int                                        x,
                    int                                        y
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.MoveTo( x, y );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.MoveTo( x, y );
}

DFBResult
CoreWindow_Resize(
                    CoreWindow                                *obj,
                    int                                        width,
                    int                                        height
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.Resize( width, height );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.Resize( width, height );
}

DFBResult
CoreWindow_Destroy(
                    CoreWindow                                *obj

)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.Destroy(  );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.Destroy(  );
}

DFBResult
CoreWindow_SetCursorPosition(
                    CoreWindow                                *obj,
                    int                                        x,
                    int                                        y
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetCursorPosition( x, y );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetCursorPosition( x, y );
}

DFBResult
CoreWindow_ChangeEvents(
                    CoreWindow                                *obj,
                    DFBWindowEventType                         disable,
                    DFBWindowEventType                         enable
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.ChangeEvents( disable, enable );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.ChangeEvents( disable, enable );
}

DFBResult
CoreWindow_ChangeOptions(
                    CoreWindow                                *obj,
                    DFBWindowOptions                           disable,
                    DFBWindowOptions                           enable
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.ChangeOptions( disable, enable );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.ChangeOptions( disable, enable );
}

DFBResult
CoreWindow_SetColor(
                    CoreWindow                                *obj,
                    const DFBColor                            *color
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetColor( color );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetColor( color );
}

DFBResult
CoreWindow_SetColorKey(
                    CoreWindow                                *obj,
                    u32                                        key
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetColorKey( key );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetColorKey( key );
}

DFBResult
CoreWindow_SetOpaque(
                    CoreWindow                                *obj,
                    const DFBRegion                           *opaque
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetOpaque( opaque );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetOpaque( opaque );
}

DFBResult
CoreWindow_SetOpacity(
                    CoreWindow                                *obj,
                    u8                                         opacity
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetOpacity( opacity );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetOpacity( opacity );
}

DFBResult
CoreWindow_SetStacking(
                    CoreWindow                                *obj,
                    DFBWindowStackingClass                     stacking
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetStacking( stacking );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetStacking( stacking );
}

DFBResult
CoreWindow_SetBounds(
                    CoreWindow                                *obj,
                    const DFBRectangle                        *bounds
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetBounds( bounds );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetBounds( bounds );
}

DFBResult
CoreWindow_SetKeySelection(
                    CoreWindow                                *obj,
                    DFBWindowKeySelection                      selection,
                    const DFBInputDeviceKeySymbol             *keys,
                    u32                                        num_keys
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetKeySelection( selection, keys, num_keys );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetKeySelection( selection, keys, num_keys );
}

DFBResult
CoreWindow_SetRotation(
                    CoreWindow                                *obj,
                    int                                        rotation
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.SetRotation( rotation );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.SetRotation( rotation );
}

DFBResult
CoreWindow_GetSurface(
                    CoreWindow                                *obj,
                    CoreSurface                              **ret_surface
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IWindow_Real real( core_dfb, obj );

        return real.GetSurface( ret_surface );
    }

    DirectFB::IWindow_Requestor requestor( core_dfb, obj );

    return requestor.GetSurface( ret_surface );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
CoreWindow_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::CoreWindowDispatch *dispatch = (DirectFB::CoreWindowDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *CoreWindow_Init_Dispatch(
                    CoreDFB              *core,
                    CoreWindow           *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreWindow_Dispatch, obj, core->world );

    return NULL;
}

void  CoreWindow_Deinit_Dispatch(FusionCall *call)
{
	fusion_call_destroy( call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
IWindow_Requestor::Repaint(
                    const DFBRegion                           *left,
                    const DFBRegion                           *right,
                    DFBSurfaceFlipFlags                        flags
)
{
    DFBResult           ret;
    CoreWindowRepaint       *block = (CoreWindowRepaint*) alloca( sizeof(CoreWindowRepaint) );
    CoreWindowRepaintReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( left != NULL );
    D_ASSERT( right != NULL );

    block->left = *left;
    block->right = *right;
    block->flags = flags;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_Repaint, block, sizeof(CoreWindowRepaint), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_Repaint ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_Repaint failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::BeginUpdates(
                    const DFBRegion                           *update
)
{
    DFBResult           ret;
    CoreWindowBeginUpdates       *block = (CoreWindowBeginUpdates*) alloca( sizeof(CoreWindowBeginUpdates) );
    CoreWindowBeginUpdatesReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


  if (update) {
    block->update = *update;
    block->update_set = true;
  }
  else
    block->update_set = false;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_BeginUpdates, block, sizeof(CoreWindowBeginUpdates), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_BeginUpdates ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_BeginUpdates failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::Restack(
                    CoreWindow                                *relative,
                    int                                        relation
)
{
    DFBResult           ret;
    CoreWindowRestack       *block = (CoreWindowRestack*) alloca( sizeof(CoreWindowRestack) );
    CoreWindowRestackReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


  if (relative) {
    block->relative_id = CoreWindow_GetID( relative );
    block->relative_set = true;
  }
  else
    block->relative_set = false;
    block->relation = relation;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_Restack, block, sizeof(CoreWindowRestack), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_Restack ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_Restack failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetConfig(
                    const CoreWindowConfig                    *config,
                    const DFBInputDeviceKeySymbol             *keys,
                    u32                                        num_keys,
                    CoreWindow                                *parent,
                    CoreWindowConfigFlags                      flags
)
{
    DFBResult           ret;
    CoreWindowSetConfig       *block = (CoreWindowSetConfig*) alloca( sizeof(CoreWindowSetConfig) + sizeof(DFBInputDeviceKeySymbol) * num_keys );
    CoreWindowSetConfigReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    block->config = *config;
    block->num_keys = num_keys;
  if (parent) {
    block->parent_id = CoreWindow_GetID( parent );
    block->parent_set = true;
  }
  else
    block->parent_set = false;
    block->flags = flags;
  if (keys) {
    direct_memcpy( (char*) (block + 1), keys, sizeof(DFBInputDeviceKeySymbol) * num_keys );
    block->keys_set = true;
  }
  else {
    block->keys_set = false;
    keys = 0;
  }

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetConfig, block, sizeof(CoreWindowSetConfig) + sizeof(DFBInputDeviceKeySymbol) * num_keys, &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetConfig ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetConfig failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::Bind(
                    CoreWindow                                *source,
                    int                                        x,
                    int                                        y
)
{
    DFBResult           ret;
    CoreWindowBind       *block = (CoreWindowBind*) alloca( sizeof(CoreWindowBind) );
    CoreWindowBindReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( source != NULL );

    block->source_id = CoreWindow_GetID( source );
    block->x = x;
    block->y = y;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_Bind, block, sizeof(CoreWindowBind), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_Bind ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_Bind failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::Unbind(
                    CoreWindow                                *source
)
{
    DFBResult           ret;
    CoreWindowUnbind       *block = (CoreWindowUnbind*) alloca( sizeof(CoreWindowUnbind) );
    CoreWindowUnbindReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( source != NULL );

    block->source_id = CoreWindow_GetID( source );

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_Unbind, block, sizeof(CoreWindowUnbind), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_Unbind ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_Unbind failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::RequestFocus(

)
{
    DFBResult           ret;
    CoreWindowRequestFocus       *block = (CoreWindowRequestFocus*) alloca( sizeof(CoreWindowRequestFocus) );
    CoreWindowRequestFocusReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_RequestFocus, block, sizeof(CoreWindowRequestFocus), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_RequestFocus ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_RequestFocus failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::ChangeGrab(
                    CoreWMGrabTarget                           target,
                    bool                                       grab
)
{
    DFBResult           ret;
    CoreWindowChangeGrab       *block = (CoreWindowChangeGrab*) alloca( sizeof(CoreWindowChangeGrab) );
    CoreWindowChangeGrabReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->target = target;
    block->grab = grab;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_ChangeGrab, block, sizeof(CoreWindowChangeGrab), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_ChangeGrab ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_ChangeGrab failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::GrabKey(
                    DFBInputDeviceKeySymbol                    symbol,
                    DFBInputDeviceModifierMask                 modifiers
)
{
    DFBResult           ret;
    CoreWindowGrabKey       *block = (CoreWindowGrabKey*) alloca( sizeof(CoreWindowGrabKey) );
    CoreWindowGrabKeyReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->symbol = symbol;
    block->modifiers = modifiers;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_GrabKey, block, sizeof(CoreWindowGrabKey), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_GrabKey ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_GrabKey failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::UngrabKey(
                    DFBInputDeviceKeySymbol                    symbol,
                    DFBInputDeviceModifierMask                 modifiers
)
{
    DFBResult           ret;
    CoreWindowUngrabKey       *block = (CoreWindowUngrabKey*) alloca( sizeof(CoreWindowUngrabKey) );
    CoreWindowUngrabKeyReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->symbol = symbol;
    block->modifiers = modifiers;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_UngrabKey, block, sizeof(CoreWindowUngrabKey), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_UngrabKey ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_UngrabKey failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::Move(
                    int                                        dx,
                    int                                        dy
)
{
    DFBResult           ret;
    CoreWindowMove       *block = (CoreWindowMove*) alloca( sizeof(CoreWindowMove) );
    CoreWindowMoveReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->dx = dx;
    block->dy = dy;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_Move, block, sizeof(CoreWindowMove), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_Move ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_Move failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::MoveTo(
                    int                                        x,
                    int                                        y
)
{
    DFBResult           ret;
    CoreWindowMoveTo       *block = (CoreWindowMoveTo*) alloca( sizeof(CoreWindowMoveTo) );
    CoreWindowMoveToReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->x = x;
    block->y = y;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_MoveTo, block, sizeof(CoreWindowMoveTo), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_MoveTo ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_MoveTo failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::Resize(
                    int                                        width,
                    int                                        height
)
{
    DFBResult           ret;
    CoreWindowResize       *block = (CoreWindowResize*) alloca( sizeof(CoreWindowResize) );
    CoreWindowResizeReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->width = width;
    block->height = height;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_Resize, block, sizeof(CoreWindowResize), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_Resize ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_Resize failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::Destroy(

)
{
    DFBResult           ret;
    CoreWindowDestroy       *block = (CoreWindowDestroy*) alloca( sizeof(CoreWindowDestroy) );
    CoreWindowDestroyReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_Destroy, block, sizeof(CoreWindowDestroy), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_Destroy ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_Destroy failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetCursorPosition(
                    int                                        x,
                    int                                        y
)
{
    DFBResult           ret;
    CoreWindowSetCursorPosition       *block = (CoreWindowSetCursorPosition*) alloca( sizeof(CoreWindowSetCursorPosition) );
    CoreWindowSetCursorPositionReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->x = x;
    block->y = y;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetCursorPosition, block, sizeof(CoreWindowSetCursorPosition), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetCursorPosition ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetCursorPosition failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::ChangeEvents(
                    DFBWindowEventType                         disable,
                    DFBWindowEventType                         enable
)
{
    DFBResult           ret;
    CoreWindowChangeEvents       *block = (CoreWindowChangeEvents*) alloca( sizeof(CoreWindowChangeEvents) );
    CoreWindowChangeEventsReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->disable = disable;
    block->enable = enable;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_ChangeEvents, block, sizeof(CoreWindowChangeEvents), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_ChangeEvents ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_ChangeEvents failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::ChangeOptions(
                    DFBWindowOptions                           disable,
                    DFBWindowOptions                           enable
)
{
    DFBResult           ret;
    CoreWindowChangeOptions       *block = (CoreWindowChangeOptions*) alloca( sizeof(CoreWindowChangeOptions) );
    CoreWindowChangeOptionsReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->disable = disable;
    block->enable = enable;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_ChangeOptions, block, sizeof(CoreWindowChangeOptions), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_ChangeOptions ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_ChangeOptions failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetColor(
                    const DFBColor                            *color
)
{
    DFBResult           ret;
    CoreWindowSetColor       *block = (CoreWindowSetColor*) alloca( sizeof(CoreWindowSetColor) );
    CoreWindowSetColorReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( color != NULL );

    block->color = *color;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetColor, block, sizeof(CoreWindowSetColor), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetColor ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetColor failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetColorKey(
                    u32                                        key
)
{
    DFBResult           ret;
    CoreWindowSetColorKey       *block = (CoreWindowSetColorKey*) alloca( sizeof(CoreWindowSetColorKey) );
    CoreWindowSetColorKeyReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->key = key;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetColorKey, block, sizeof(CoreWindowSetColorKey), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetColorKey ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetColorKey failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetOpaque(
                    const DFBRegion                           *opaque
)
{
    DFBResult           ret;
    CoreWindowSetOpaque       *block = (CoreWindowSetOpaque*) alloca( sizeof(CoreWindowSetOpaque) );
    CoreWindowSetOpaqueReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( opaque != NULL );

    block->opaque = *opaque;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetOpaque, block, sizeof(CoreWindowSetOpaque), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetOpaque ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetOpaque failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetOpacity(
                    u8                                         opacity
)
{
    DFBResult           ret;
    CoreWindowSetOpacity       *block = (CoreWindowSetOpacity*) alloca( sizeof(CoreWindowSetOpacity) );
    CoreWindowSetOpacityReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->opacity = opacity;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetOpacity, block, sizeof(CoreWindowSetOpacity), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetOpacity ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetOpacity failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetStacking(
                    DFBWindowStackingClass                     stacking
)
{
    DFBResult           ret;
    CoreWindowSetStacking       *block = (CoreWindowSetStacking*) alloca( sizeof(CoreWindowSetStacking) );
    CoreWindowSetStackingReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->stacking = stacking;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetStacking, block, sizeof(CoreWindowSetStacking), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetStacking ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetStacking failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetBounds(
                    const DFBRectangle                        *bounds
)
{
    DFBResult           ret;
    CoreWindowSetBounds       *block = (CoreWindowSetBounds*) alloca( sizeof(CoreWindowSetBounds) );
    CoreWindowSetBoundsReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( bounds != NULL );

    block->bounds = *bounds;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetBounds, block, sizeof(CoreWindowSetBounds), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetBounds ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetBounds failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetKeySelection(
                    DFBWindowKeySelection                      selection,
                    const DFBInputDeviceKeySymbol             *keys,
                    u32                                        num_keys
)
{
    DFBResult           ret;
    CoreWindowSetKeySelection       *block = (CoreWindowSetKeySelection*) alloca( sizeof(CoreWindowSetKeySelection) + sizeof(DFBInputDeviceKeySymbol) * num_keys );
    CoreWindowSetKeySelectionReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->selection = selection;
    block->num_keys = num_keys;
  if (keys) {
    direct_memcpy( (char*) (block + 1), keys, sizeof(DFBInputDeviceKeySymbol) * num_keys );
    block->keys_set = true;
  }
  else {
    block->keys_set = false;
    keys = 0;
  }

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetKeySelection, block, sizeof(CoreWindowSetKeySelection) + sizeof(DFBInputDeviceKeySymbol) * num_keys, &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetKeySelection ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetKeySelection failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::SetRotation(
                    int                                        rotation
)
{
    DFBResult           ret;
    CoreWindowSetRotation       *block = (CoreWindowSetRotation*) alloca( sizeof(CoreWindowSetRotation) );
    CoreWindowSetRotationReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );


    block->rotation = rotation;

    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_SetRotation, block, sizeof(CoreWindowSetRotation), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_SetRotation ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_SetRotation failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IWindow_Requestor::GetSurface(
                    CoreSurface                              **ret_surface
)
{
    DFBResult           ret;
    CoreSurface *surface = NULL;
    CoreWindowGetSurface       *block = (CoreWindowGetSurface*) alloca( sizeof(CoreWindowGetSurface) );
    CoreWindowGetSurfaceReturn  return_block;

    D_DEBUG_AT( DirectFB_CoreWindow, "IWindow_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_surface != NULL );


    ret = (DFBResult) CoreWindow_Call( obj, FCEF_NONE, CoreWindow_GetSurface, block, sizeof(CoreWindowGetSurface), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindow_Call( CoreWindow_GetSurface ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CoreWindow_GetSurface failed!\n", __FUNCTION__ );
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

/*********************************************************************************************************************/

DFBResult
CoreWindowDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreWindow, "CoreWindowDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case CoreWindow_Repaint: {
            D_UNUSED
            CoreWindowRepaint       *args        = (CoreWindowRepaint *) ptr;
            CoreWindowRepaintReturn *return_args = (CoreWindowRepaintReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_Repaint\n" );

            return_args->result = real->Repaint( &args->left, &args->right, args->flags );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowRepaintReturn);

            return DFB_OK;
        }

        case CoreWindow_BeginUpdates: {
            D_UNUSED
            CoreWindowBeginUpdates       *args        = (CoreWindowBeginUpdates *) ptr;
            CoreWindowBeginUpdatesReturn *return_args = (CoreWindowBeginUpdatesReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_BeginUpdates\n" );

            return_args->result = real->BeginUpdates( args->update_set ? &args->update : NULL );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowBeginUpdatesReturn);

            return DFB_OK;
        }

        case CoreWindow_Restack: {
    CoreWindow *relative = NULL;
            D_UNUSED
            CoreWindowRestack       *args        = (CoreWindowRestack *) ptr;
            CoreWindowRestackReturn *return_args = (CoreWindowRestackReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_Restack\n" );

            if (args->relative_set) {
                ret = (DFBResult) CoreWindow_Lookup( core, args->relative_id, caller, &relative );
                if (ret) {
                     D_DERROR( ret, "%s: Looking up relative by ID %u failed!\n", __FUNCTION__, args->relative_id );
                     return_args->result = ret;
                     return DFB_OK;
                }
            }

            return_args->result = real->Restack( args->relative_set ? relative : NULL, args->relation );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowRestackReturn);

            if (relative)
                CoreWindow_Unref( relative );

            return DFB_OK;
        }

        case CoreWindow_SetConfig: {
    CoreWindow *parent = NULL;
            D_UNUSED
            CoreWindowSetConfig       *args        = (CoreWindowSetConfig *) ptr;
            CoreWindowSetConfigReturn *return_args = (CoreWindowSetConfigReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetConfig\n" );

            if (args->parent_set) {
                ret = (DFBResult) CoreWindow_Lookup( core, args->parent_id, caller, &parent );
                if (ret) {
                     D_DERROR( ret, "%s: Looking up parent by ID %u failed!\n", __FUNCTION__, args->parent_id );
                     return_args->result = ret;
                     return DFB_OK;
                }
            }

            return_args->result = real->SetConfig( &args->config, args->keys_set ? (DFBInputDeviceKeySymbol*) ((char*)(args + 1)) : NULL, args->num_keys, args->parent_set ? parent : NULL, args->flags );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetConfigReturn);

            if (parent)
                CoreWindow_Unref( parent );

            return DFB_OK;
        }

        case CoreWindow_Bind: {
    CoreWindow *source = NULL;
            D_UNUSED
            CoreWindowBind       *args        = (CoreWindowBind *) ptr;
            CoreWindowBindReturn *return_args = (CoreWindowBindReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_Bind\n" );

            ret = (DFBResult) CoreWindow_Lookup( core, args->source_id, caller, &source );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up source by ID %u failed!\n", __FUNCTION__, args->source_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real->Bind( source, args->x, args->y );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowBindReturn);

            if (source)
                CoreWindow_Unref( source );

            return DFB_OK;
        }

        case CoreWindow_Unbind: {
    CoreWindow *source = NULL;
            D_UNUSED
            CoreWindowUnbind       *args        = (CoreWindowUnbind *) ptr;
            CoreWindowUnbindReturn *return_args = (CoreWindowUnbindReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_Unbind\n" );

            ret = (DFBResult) CoreWindow_Lookup( core, args->source_id, caller, &source );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up source by ID %u failed!\n", __FUNCTION__, args->source_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real->Unbind( source );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowUnbindReturn);

            if (source)
                CoreWindow_Unref( source );

            return DFB_OK;
        }

        case CoreWindow_RequestFocus: {
            D_UNUSED
            CoreWindowRequestFocus       *args        = (CoreWindowRequestFocus *) ptr;
            CoreWindowRequestFocusReturn *return_args = (CoreWindowRequestFocusReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_RequestFocus\n" );

            return_args->result = real->RequestFocus(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowRequestFocusReturn);

            return DFB_OK;
        }

        case CoreWindow_ChangeGrab: {
            D_UNUSED
            CoreWindowChangeGrab       *args        = (CoreWindowChangeGrab *) ptr;
            CoreWindowChangeGrabReturn *return_args = (CoreWindowChangeGrabReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_ChangeGrab\n" );

            return_args->result = real->ChangeGrab( args->target, args->grab );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowChangeGrabReturn);

            return DFB_OK;
        }

        case CoreWindow_GrabKey: {
            D_UNUSED
            CoreWindowGrabKey       *args        = (CoreWindowGrabKey *) ptr;
            CoreWindowGrabKeyReturn *return_args = (CoreWindowGrabKeyReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_GrabKey\n" );

            return_args->result = real->GrabKey( args->symbol, args->modifiers );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowGrabKeyReturn);

            return DFB_OK;
        }

        case CoreWindow_UngrabKey: {
            D_UNUSED
            CoreWindowUngrabKey       *args        = (CoreWindowUngrabKey *) ptr;
            CoreWindowUngrabKeyReturn *return_args = (CoreWindowUngrabKeyReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_UngrabKey\n" );

            return_args->result = real->UngrabKey( args->symbol, args->modifiers );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowUngrabKeyReturn);

            return DFB_OK;
        }

        case CoreWindow_Move: {
            D_UNUSED
            CoreWindowMove       *args        = (CoreWindowMove *) ptr;
            CoreWindowMoveReturn *return_args = (CoreWindowMoveReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_Move\n" );

            return_args->result = real->Move( args->dx, args->dy );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowMoveReturn);

            return DFB_OK;
        }

        case CoreWindow_MoveTo: {
            D_UNUSED
            CoreWindowMoveTo       *args        = (CoreWindowMoveTo *) ptr;
            CoreWindowMoveToReturn *return_args = (CoreWindowMoveToReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_MoveTo\n" );

            return_args->result = real->MoveTo( args->x, args->y );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowMoveToReturn);

            return DFB_OK;
        }

        case CoreWindow_Resize: {
            D_UNUSED
            CoreWindowResize       *args        = (CoreWindowResize *) ptr;
            CoreWindowResizeReturn *return_args = (CoreWindowResizeReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_Resize\n" );

            return_args->result = real->Resize( args->width, args->height );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowResizeReturn);

            return DFB_OK;
        }

        case CoreWindow_Destroy: {
            D_UNUSED
            CoreWindowDestroy       *args        = (CoreWindowDestroy *) ptr;
            CoreWindowDestroyReturn *return_args = (CoreWindowDestroyReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_Destroy\n" );

            return_args->result = real->Destroy(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowDestroyReturn);

            return DFB_OK;
        }

        case CoreWindow_SetCursorPosition: {
            D_UNUSED
            CoreWindowSetCursorPosition       *args        = (CoreWindowSetCursorPosition *) ptr;
            CoreWindowSetCursorPositionReturn *return_args = (CoreWindowSetCursorPositionReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetCursorPosition\n" );

            return_args->result = real->SetCursorPosition( args->x, args->y );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetCursorPositionReturn);

            return DFB_OK;
        }

        case CoreWindow_ChangeEvents: {
            D_UNUSED
            CoreWindowChangeEvents       *args        = (CoreWindowChangeEvents *) ptr;
            CoreWindowChangeEventsReturn *return_args = (CoreWindowChangeEventsReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_ChangeEvents\n" );

            return_args->result = real->ChangeEvents( args->disable, args->enable );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowChangeEventsReturn);

            return DFB_OK;
        }

        case CoreWindow_ChangeOptions: {
            D_UNUSED
            CoreWindowChangeOptions       *args        = (CoreWindowChangeOptions *) ptr;
            CoreWindowChangeOptionsReturn *return_args = (CoreWindowChangeOptionsReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_ChangeOptions\n" );

            return_args->result = real->ChangeOptions( args->disable, args->enable );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowChangeOptionsReturn);

            return DFB_OK;
        }

        case CoreWindow_SetColor: {
            D_UNUSED
            CoreWindowSetColor       *args        = (CoreWindowSetColor *) ptr;
            CoreWindowSetColorReturn *return_args = (CoreWindowSetColorReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetColor\n" );

            return_args->result = real->SetColor( &args->color );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetColorReturn);

            return DFB_OK;
        }

        case CoreWindow_SetColorKey: {
            D_UNUSED
            CoreWindowSetColorKey       *args        = (CoreWindowSetColorKey *) ptr;
            CoreWindowSetColorKeyReturn *return_args = (CoreWindowSetColorKeyReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetColorKey\n" );

            return_args->result = real->SetColorKey( args->key );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetColorKeyReturn);

            return DFB_OK;
        }

        case CoreWindow_SetOpaque: {
            D_UNUSED
            CoreWindowSetOpaque       *args        = (CoreWindowSetOpaque *) ptr;
            CoreWindowSetOpaqueReturn *return_args = (CoreWindowSetOpaqueReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetOpaque\n" );

            return_args->result = real->SetOpaque( &args->opaque );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetOpaqueReturn);

            return DFB_OK;
        }

        case CoreWindow_SetOpacity: {
            D_UNUSED
            CoreWindowSetOpacity       *args        = (CoreWindowSetOpacity *) ptr;
            CoreWindowSetOpacityReturn *return_args = (CoreWindowSetOpacityReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetOpacity\n" );

            return_args->result = real->SetOpacity( args->opacity );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetOpacityReturn);

            return DFB_OK;
        }

        case CoreWindow_SetStacking: {
            D_UNUSED
            CoreWindowSetStacking       *args        = (CoreWindowSetStacking *) ptr;
            CoreWindowSetStackingReturn *return_args = (CoreWindowSetStackingReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetStacking\n" );

            return_args->result = real->SetStacking( args->stacking );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetStackingReturn);

            return DFB_OK;
        }

        case CoreWindow_SetBounds: {
            D_UNUSED
            CoreWindowSetBounds       *args        = (CoreWindowSetBounds *) ptr;
            CoreWindowSetBoundsReturn *return_args = (CoreWindowSetBoundsReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetBounds\n" );

            return_args->result = real->SetBounds( &args->bounds );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetBoundsReturn);

            return DFB_OK;
        }

        case CoreWindow_SetKeySelection: {
            D_UNUSED
            CoreWindowSetKeySelection       *args        = (CoreWindowSetKeySelection *) ptr;
            CoreWindowSetKeySelectionReturn *return_args = (CoreWindowSetKeySelectionReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetKeySelection\n" );

            return_args->result = real->SetKeySelection( args->selection, args->keys_set ? (DFBInputDeviceKeySymbol*) ((char*)(args + 1)) : NULL, args->num_keys );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetKeySelectionReturn);

            return DFB_OK;
        }

        case CoreWindow_SetRotation: {
            D_UNUSED
            CoreWindowSetRotation       *args        = (CoreWindowSetRotation *) ptr;
            CoreWindowSetRotationReturn *return_args = (CoreWindowSetRotationReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_SetRotation\n" );

            return_args->result = real->SetRotation( args->rotation );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowSetRotationReturn);

            return DFB_OK;
        }

        case CoreWindow_GetSurface: {
    CoreSurface *surface = NULL;
            D_UNUSED
            CoreWindowGetSurface       *args        = (CoreWindowGetSurface *) ptr;
            CoreWindowGetSurfaceReturn *return_args = (CoreWindowGetSurfaceReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindow, "=-> CoreWindow_GetSurface\n" );

            return_args->result = real->GetSurface( &surface );
            if (return_args->result == DFB_OK) {
                CoreSurface_Throw( surface, caller, &return_args->surface_id );
            }

            *ret_length = sizeof(CoreWindowGetSurfaceReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
