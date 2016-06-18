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

#include "SaWMan.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_SaWMan, "DirectFB/SaWMan", "DirectFB SaWMan" );

/*********************************************************************************************************************/

DFBResult
SaWMan_RegisterProcess(
                    SaWMan                                    *obj,
                    SaWManProcessFlags                         flags,
                    s32                                        pid,
                    u32                                        fusion_id,
                    SaWManProcess                            **ret_process
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::ISaWManWM_Real real( core_dfb, obj );

        return real.RegisterProcess( flags, pid, fusion_id, ret_process );
    }

    DirectFB::ISaWManWM_Requestor requestor( core_dfb, obj );

    return requestor.RegisterProcess( flags, pid, fusion_id, ret_process );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
SaWMan_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::SaWManDispatch *dispatch = (DirectFB::SaWManDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *SaWMan_Init_Dispatch(
                    CoreDFB              *core,
                    SaWMan               *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, SaWMan_Dispatch, obj, core->world );

    return NULL;
}

void  SaWMan_Deinit_Dispatch(FusionCall *call)
{
	fusion_call_destroy( call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ISaWManWM_Requestor::RegisterProcess(
                    SaWManProcessFlags                         flags,
                    s32                                        pid,
                    u32                                        fusion_id,
                    SaWManProcess                            **ret_process
)
{
    DFBResult           ret;
    SaWManProcess *process = NULL;
    SaWManRegisterProcess       *block = (SaWManRegisterProcess*) alloca( sizeof(SaWManRegisterProcess) );
    SaWManRegisterProcessReturn  return_block;

    D_DEBUG_AT( DirectFB_SaWMan, "ISaWManWM_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_process != NULL );

    block->flags = flags;
    block->pid = pid;
    block->fusion_id = fusion_id;

    ret = (DFBResult) SaWMan_Call( obj, FCEF_NONE, SaWMan_RegisterProcess, block, sizeof(SaWManRegisterProcess), &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: SaWMan_Call( SaWMan_RegisterProcess ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: SaWMan_RegisterProcess failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    ret = (DFBResult) SaWManProcess_Catch( core, return_block.process_id, &process );
    if (ret) {
         D_DERROR( ret, "%s: Catching process by ID %u failed!\n", __FUNCTION__, return_block.process_id );
         return ret;
    }

    *ret_process = process;

    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
SaWManDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_SaWMan, "SaWManDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case SaWMan_RegisterProcess: {
    SaWManProcess *process = NULL;
            D_UNUSED
            SaWManRegisterProcess       *args        = (SaWManRegisterProcess *) ptr;
            SaWManRegisterProcessReturn *return_args = (SaWManRegisterProcessReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_SaWMan, "=-> SaWMan_RegisterProcess\n" );

            return_args->result = real->RegisterProcess( args->flags, args->pid, args->fusion_id, &process );
            if (return_args->result == DFB_OK) {
                SaWManProcess_Throw( process, caller, &return_args->process_id );
            }

            *ret_length = sizeof(SaWManRegisterProcessReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
