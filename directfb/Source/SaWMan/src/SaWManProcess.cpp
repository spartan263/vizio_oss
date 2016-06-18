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

#include "SaWManProcess.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_SaWManProcess, "DirectFB/SaWManProcess", "DirectFB SaWManProcess" );

/*********************************************************************************************************************/

DFBResult
SaWManProcess_SetExiting(
                    SaWManProcess                             *obj

)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManProcess_Real real( core_dfb, obj );

        return real.SetExiting(  );
    }

    DirectFB::ISaWManProcess_Requestor requestor( core_dfb, obj );

    return requestor.SetExiting(  );
}

DFBResult
SaWManProcess_RegisterManager(
                    SaWManProcess                             *obj,
                    const SaWManRegisterManagerData           *data,
                    SaWManManager                            **ret_manager
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManProcess_Real real( core_dfb, obj );

        return real.RegisterManager( data, ret_manager );
    }

    DirectFB::ISaWManProcess_Requestor requestor( core_dfb, obj );

    return requestor.RegisterManager( data, ret_manager );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
SaWManProcess_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::SaWManProcessDispatch *dispatch = (DirectFB::SaWManProcessDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *SaWManProcess_Init_Dispatch(
                    CoreDFB              *core,
                    SaWManProcess        *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, SaWManProcess_Dispatch, obj, core->world );

    return NULL;
}

void  SaWManProcess_Deinit_Dispatch(FusionCall *call)
{
	fusion_call_destroy( call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ISaWManProcess_Requestor::SetExiting(

)
{
    DFBResult           ret;
    SaWManProcessSetExiting       *block = (SaWManProcessSetExiting*) alloca( sizeof(SaWManProcessSetExiting) );
    SaWManProcessSetExitingReturn  return_block;

    D_DEBUG_AT( DirectFB_SaWManProcess, "ISaWManProcess_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) SaWManProcess_Call( obj, FCEF_NONE, SaWManProcess_SetExiting, block, sizeof(SaWManProcessSetExiting), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManProcess_Call( SaWManProcess_SetExiting ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: SaWManProcess_SetExiting failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
ISaWManProcess_Requestor::RegisterManager(
                    const SaWManRegisterManagerData           *data,
                    SaWManManager                            **ret_manager
)
{
    DFBResult           ret;
    SaWManManager *manager = NULL;
    SaWManProcessRegisterManager       *block = (SaWManProcessRegisterManager*) alloca( sizeof(SaWManProcessRegisterManager) );
    SaWManProcessRegisterManagerReturn  return_block;

    D_DEBUG_AT( DirectFB_SaWManProcess, "ISaWManProcess_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( data != NULL );
    D_ASSERT( ret_manager != NULL );

    block->data = *data;

    ret = (DFBResult) SaWManProcess_Call( obj, FCEF_NONE, SaWManProcess_RegisterManager, block, sizeof(SaWManProcessRegisterManager), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWManProcess_Call( SaWManProcess_RegisterManager ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: SaWManProcess_RegisterManager failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    ret = (DFBResult) SaWManManager_Catch( core, return_block.manager_id, &manager );
    if (ret) {
         D_DERROR( ret, "%s: Catching manager by ID %u failed!\n", __FUNCTION__, return_block.manager_id );
         return ret;
    }

    *ret_manager = manager;

    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
SaWManProcessDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_SaWManProcess, "SaWManProcessDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case SaWManProcess_SetExiting: {
            D_UNUSED
            SaWManProcessSetExiting       *args        = (SaWManProcessSetExiting *) ptr;
            SaWManProcessSetExitingReturn *return_args = (SaWManProcessSetExitingReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_SaWManProcess, "=-> SaWManProcess_SetExiting\n" );

            return_args->result = real->SetExiting(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(SaWManProcessSetExitingReturn);

            return DFB_OK;
        }

        case SaWManProcess_RegisterManager: {
    SaWManManager *manager = NULL;
            D_UNUSED
            SaWManProcessRegisterManager       *args        = (SaWManProcessRegisterManager *) ptr;
            SaWManProcessRegisterManagerReturn *return_args = (SaWManProcessRegisterManagerReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_SaWManProcess, "=-> SaWManProcess_RegisterManager\n" );

            return_args->result = real->RegisterManager( &args->data, &manager );
            if (return_args->result == DFB_OK) {
                SaWManManager_Throw( manager, caller, &return_args->manager_id );
            }

            *ret_length = sizeof(SaWManProcessRegisterManagerReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
