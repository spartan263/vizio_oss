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

#include "SaWManManager.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_SaWManManager, "DirectFB/SaWManManager", "DirectFB SaWManManager" );

/*********************************************************************************************************************/

DFBResult
SaWManManager_QueueUpdate(
                    SaWManManager                             *obj,
                    DFBWindowStackingClass                     stacking,
                    const DFBRegion                           *update
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManManager_Real real( core_dfb, obj );

        return real.QueueUpdate( stacking, update );
    }

    DirectFB::ISaWManManager_Requestor requestor( core_dfb, obj );

    return requestor.QueueUpdate( stacking, update );
}

DFBResult
SaWManManager_ProcessUpdates(
                    SaWManManager                             *obj,
                    DFBSurfaceFlipFlags                        flags
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManManager_Real real( core_dfb, obj );

        return real.ProcessUpdates( flags );
    }

    DirectFB::ISaWManManager_Requestor requestor( core_dfb, obj );

    return requestor.ProcessUpdates( flags );
}

DFBResult
SaWManManager_CloseWindow(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManManager_Real real( core_dfb, obj );

        return real.CloseWindow( window );
    }

    DirectFB::ISaWManManager_Requestor requestor( core_dfb, obj );

    return requestor.CloseWindow( window );
}

DFBResult
SaWManManager_InsertWindow(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window,
                    SaWManWindow                              *relative,
                    SaWManWindowRelation                       relation
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManManager_Real real( core_dfb, obj );

        return real.InsertWindow( window, relative, relation );
    }

    DirectFB::ISaWManManager_Requestor requestor( core_dfb, obj );

    return requestor.InsertWindow( window, relative, relation );
}

DFBResult
SaWManManager_RemoveWindow(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManManager_Real real( core_dfb, obj );

        return real.RemoveWindow( window );
    }

    DirectFB::ISaWManManager_Requestor requestor( core_dfb, obj );

    return requestor.RemoveWindow( window );
}

DFBResult
SaWManManager_SwitchFocus(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManManager_Real real( core_dfb, obj );

        return real.SwitchFocus( window );
    }

    DirectFB::ISaWManManager_Requestor requestor( core_dfb, obj );

    return requestor.SwitchFocus( window );
}

DFBResult
SaWManManager_SetScalingMode(
                    SaWManManager                             *obj,
                    SaWManScalingMode                          mode
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManManager_Real real( core_dfb, obj );

        return real.SetScalingMode( mode );
    }

    DirectFB::ISaWManManager_Requestor requestor( core_dfb, obj );

    return requestor.SetScalingMode( mode );
}

DFBResult
SaWManManager_SetWindowConfig(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window,
                    const SaWManWindowConfig                  *config,
                    SaWManWindowConfigFlags                    flags
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManManager_Real real( core_dfb, obj );

        return real.SetWindowConfig( window, config, flags );
    }

    DirectFB::ISaWManManager_Requestor requestor( core_dfb, obj );

    return requestor.SetWindowConfig( window, config, flags );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
SaWManManager_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::SaWManManagerDispatch *dispatch = (DirectFB::SaWManManagerDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *SaWManManager_Init_Dispatch(
                    CoreDFB              *core,
                    SaWManManager        *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, SaWManManager_Dispatch, obj, core->world );

    return NULL;
}

void  SaWManManager_Deinit_Dispatch(FusionCall *call)
{
    fusion_call_destroy( call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ISaWManManager_Requestor::QueueUpdate(
                    DFBWindowStackingClass                     stacking,
                    const DFBRegion                           *update
)
{
    DFBResult           ret;
    SaWManManagerQueueUpdate       *block = (SaWManManagerQueueUpdate*) alloca( sizeof(SaWManManagerQueueUpdate) );

    D_DEBUG_AT( DirectFB_SaWManManager, "ISaWManManager_Requestor::%s()\n", __FUNCTION__ );


    block->stacking = stacking;
  if (update) {
    block->update = *update;
    block->update_set = true;
  }
  else
    block->update_set = false;

    ret = (DFBResult) SaWManManager_Call( obj, FCEF_ONEWAY, SaWManManager_QueueUpdate, block, sizeof(SaWManManagerQueueUpdate), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManManager_Call( SaWManManager_QueueUpdate ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
ISaWManManager_Requestor::ProcessUpdates(
                    DFBSurfaceFlipFlags                        flags
)
{
    DFBResult           ret;
    SaWManManagerProcessUpdates       *block = (SaWManManagerProcessUpdates*) alloca( sizeof(SaWManManagerProcessUpdates) );

    D_DEBUG_AT( DirectFB_SaWManManager, "ISaWManManager_Requestor::%s()\n", __FUNCTION__ );


    block->flags = flags;

    ret = (DFBResult) SaWManManager_Call( obj, FCEF_ONEWAY, SaWManManager_ProcessUpdates, block, sizeof(SaWManManagerProcessUpdates), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManManager_Call( SaWManManager_ProcessUpdates ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
ISaWManManager_Requestor::CloseWindow(
                    SaWManWindow                              *window
)
{
    DFBResult           ret;
    SaWManManagerCloseWindow       *block = (SaWManManagerCloseWindow*) alloca( sizeof(SaWManManagerCloseWindow) );

    D_DEBUG_AT( DirectFB_SaWManManager, "ISaWManManager_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( window != NULL );

    block->window_id = SaWManWindow_GetID( window );

    ret = (DFBResult) SaWManManager_Call( obj, FCEF_ONEWAY, SaWManManager_CloseWindow, block, sizeof(SaWManManagerCloseWindow), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManManager_Call( SaWManManager_CloseWindow ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
ISaWManManager_Requestor::InsertWindow(
                    SaWManWindow                              *window,
                    SaWManWindow                              *relative,
                    SaWManWindowRelation                       relation
)
{
    DFBResult           ret;
    SaWManManagerInsertWindow       *block = (SaWManManagerInsertWindow*) alloca( sizeof(SaWManManagerInsertWindow) );

    D_DEBUG_AT( DirectFB_SaWManManager, "ISaWManManager_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( window != NULL );

    block->window_id = SaWManWindow_GetID( window );
  if (relative) {
    block->relative_id = SaWManWindow_GetID( relative );
    block->relative_set = true;
  }
  else
    block->relative_set = false;
    block->relation = relation;

    ret = (DFBResult) SaWManManager_Call( obj, FCEF_ONEWAY, SaWManManager_InsertWindow, block, sizeof(SaWManManagerInsertWindow), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManManager_Call( SaWManManager_InsertWindow ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
ISaWManManager_Requestor::RemoveWindow(
                    SaWManWindow                              *window
)
{
    DFBResult           ret;
    SaWManManagerRemoveWindow       *block = (SaWManManagerRemoveWindow*) alloca( sizeof(SaWManManagerRemoveWindow) );

    D_DEBUG_AT( DirectFB_SaWManManager, "ISaWManManager_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( window != NULL );

    block->window_id = SaWManWindow_GetID( window );

    ret = (DFBResult) SaWManManager_Call( obj, FCEF_ONEWAY, SaWManManager_RemoveWindow, block, sizeof(SaWManManagerRemoveWindow), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManManager_Call( SaWManManager_RemoveWindow ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
ISaWManManager_Requestor::SwitchFocus(
                    SaWManWindow                              *window
)
{
    DFBResult           ret;
    SaWManManagerSwitchFocus       *block = (SaWManManagerSwitchFocus*) alloca( sizeof(SaWManManagerSwitchFocus) );

    D_DEBUG_AT( DirectFB_SaWManManager, "ISaWManManager_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( window != NULL );

    block->window_id = SaWManWindow_GetID( window );

    ret = (DFBResult) SaWManManager_Call( obj, FCEF_ONEWAY, SaWManManager_SwitchFocus, block, sizeof(SaWManManagerSwitchFocus), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManManager_Call( SaWManManager_SwitchFocus ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
ISaWManManager_Requestor::SetScalingMode(
                    SaWManScalingMode                          mode
)
{
    DFBResult           ret;
    SaWManManagerSetScalingMode       *block = (SaWManManagerSetScalingMode*) alloca( sizeof(SaWManManagerSetScalingMode) );

    D_DEBUG_AT( DirectFB_SaWManManager, "ISaWManManager_Requestor::%s()\n", __FUNCTION__ );


    block->mode = mode;

    ret = (DFBResult) SaWManManager_Call( obj, FCEF_ONEWAY, SaWManManager_SetScalingMode, block, sizeof(SaWManManagerSetScalingMode), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManManager_Call( SaWManManager_SetScalingMode ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
ISaWManManager_Requestor::SetWindowConfig(
                    SaWManWindow                              *window,
                    const SaWManWindowConfig                  *config,
                    SaWManWindowConfigFlags                    flags
)
{
    DFBResult           ret;
    SaWManManagerSetWindowConfig       *block = (SaWManManagerSetWindowConfig*) alloca( sizeof(SaWManManagerSetWindowConfig) );

    D_DEBUG_AT( DirectFB_SaWManManager, "ISaWManManager_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( window != NULL );
    D_ASSERT( config != NULL );

    block->window_id = SaWManWindow_GetID( window );
    block->config = *config;
    block->flags = flags;

    ret = (DFBResult) SaWManManager_Call( obj, FCEF_ONEWAY, SaWManManager_SetWindowConfig, block, sizeof(SaWManManagerSetWindowConfig), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManManager_Call( SaWManManager_SetWindowConfig ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
SaWManManagerDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_SaWManManager, "SaWManManagerDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case SaWManManager_QueueUpdate: {
            D_UNUSED
            SaWManManagerQueueUpdate       *args        = (SaWManManagerQueueUpdate *) ptr;

            D_DEBUG_AT( DirectFB_SaWManManager, "=-> SaWManManager_QueueUpdate\n" );

            real->QueueUpdate( args->stacking, args->update_set ? &args->update : NULL );

            return DFB_OK;
        }

        case SaWManManager_ProcessUpdates: {
            D_UNUSED
            SaWManManagerProcessUpdates       *args        = (SaWManManagerProcessUpdates *) ptr;

            D_DEBUG_AT( DirectFB_SaWManManager, "=-> SaWManManager_ProcessUpdates\n" );

            real->ProcessUpdates( args->flags );

            return DFB_OK;
        }

        case SaWManManager_CloseWindow: {
    SaWManWindow *window = NULL;
            D_UNUSED
            SaWManManagerCloseWindow       *args        = (SaWManManagerCloseWindow *) ptr;

            D_DEBUG_AT( DirectFB_SaWManManager, "=-> SaWManManager_CloseWindow\n" );

            ret = (DFBResult) SaWManWindow_Lookup( core, args->window_id, caller, &window );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up window by ID %u failed!\n", __FUNCTION__, args->window_id );
                 return DFB_OK;
            }

            real->CloseWindow( window );

            if (window)
                SaWManWindow_Unref( window );

            return DFB_OK;
        }

        case SaWManManager_InsertWindow: {
    SaWManWindow *window = NULL;
    SaWManWindow *relative = NULL;
            D_UNUSED
            SaWManManagerInsertWindow       *args        = (SaWManManagerInsertWindow *) ptr;

            D_DEBUG_AT( DirectFB_SaWManManager, "=-> SaWManManager_InsertWindow\n" );

            ret = (DFBResult) SaWManWindow_Lookup( core, args->window_id, caller, &window );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up window by ID %u failed!\n", __FUNCTION__, args->window_id );
                 return DFB_OK;
            }

            if (args->relative_set) {
                ret = (DFBResult) SaWManWindow_Lookup( core, args->relative_id, caller, &relative );
                if (ret) {
                     D_DERROR( ret, "%s: Looking up relative by ID %u failed!\n", __FUNCTION__, args->relative_id );
                     return DFB_OK;
                }
            }

            real->InsertWindow( window, args->relative_set ? relative : NULL, args->relation );

            if (window)
                SaWManWindow_Unref( window );

            if (relative)
                SaWManWindow_Unref( relative );

            return DFB_OK;
        }

        case SaWManManager_RemoveWindow: {
    SaWManWindow *window = NULL;
            D_UNUSED
            SaWManManagerRemoveWindow       *args        = (SaWManManagerRemoveWindow *) ptr;

            D_DEBUG_AT( DirectFB_SaWManManager, "=-> SaWManManager_RemoveWindow\n" );

            ret = (DFBResult) SaWManWindow_Lookup( core, args->window_id, caller, &window );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up window by ID %u failed!\n", __FUNCTION__, args->window_id );
                 return DFB_OK;
            }

            real->RemoveWindow( window );

            if (window)
                SaWManWindow_Unref( window );

            return DFB_OK;
        }

        case SaWManManager_SwitchFocus: {
    SaWManWindow *window = NULL;
            D_UNUSED
            SaWManManagerSwitchFocus       *args        = (SaWManManagerSwitchFocus *) ptr;

            D_DEBUG_AT( DirectFB_SaWManManager, "=-> SaWManManager_SwitchFocus\n" );

            ret = (DFBResult) SaWManWindow_Lookup( core, args->window_id, caller, &window );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up window by ID %u failed!\n", __FUNCTION__, args->window_id );
                 return DFB_OK;
            }

            real->SwitchFocus( window );

            if (window)
                SaWManWindow_Unref( window );

            return DFB_OK;
        }

        case SaWManManager_SetScalingMode: {
            D_UNUSED
            SaWManManagerSetScalingMode       *args        = (SaWManManagerSetScalingMode *) ptr;

            D_DEBUG_AT( DirectFB_SaWManManager, "=-> SaWManManager_SetScalingMode\n" );

            real->SetScalingMode( args->mode );

            return DFB_OK;
        }

        case SaWManManager_SetWindowConfig: {
    SaWManWindow *window = NULL;
            D_UNUSED
            SaWManManagerSetWindowConfig       *args        = (SaWManManagerSetWindowConfig *) ptr;

            D_DEBUG_AT( DirectFB_SaWManManager, "=-> SaWManManager_SetWindowConfig\n" );

            ret = (DFBResult) SaWManWindow_Lookup( core, args->window_id, caller, &window );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up window by ID %u failed!\n", __FUNCTION__, args->window_id );
                 return DFB_OK;
            }

            real->SetWindowConfig( window, &args->config, args->flags );

            if (window)
                SaWManWindow_Unref( window );

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
