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

#include "CorePalette.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CorePalette, "DirectFB/CorePalette", "DirectFB CorePalette" );

/*********************************************************************************************************************/

DFBResult
CorePalette_SetEntries(
                    CorePalette                               *obj,
                    const DFBColor                            *colors,
                    u32                                        num,
                    u32                                        offset
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IPalette_Real real( core_dfb, obj );

        return real.SetEntries( colors, num, offset );
    }

    DirectFB::IPalette_Requestor requestor( core_dfb, obj );

    return requestor.SetEntries( colors, num, offset );
}

DFBResult
CorePalette_SetEntriesYUV(
                    CorePalette                               *obj,
                    const DFBColorYUV                         *colors,
                    u32                                        num,
                    u32                                        offset
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IPalette_Real real( core_dfb, obj );

        return real.SetEntriesYUV( colors, num, offset );
    }

    DirectFB::IPalette_Requestor requestor( core_dfb, obj );

    return requestor.SetEntriesYUV( colors, num, offset );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
CorePalette_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::CorePaletteDispatch *dispatch = (DirectFB::CorePaletteDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *CorePalette_Init_Dispatch(
                    CoreDFB              *core,
                    CorePalette          *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CorePalette_Dispatch, obj, core->world );

    return NULL;
}

void  CorePalette_Deinit_Dispatch(FusionCall *call)
{
	fusion_call_destroy( call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
IPalette_Requestor::SetEntries(
                    const DFBColor                            *colors,
                    u32                                        num,
                    u32                                        offset
)
{
    DFBResult           ret;
    CorePaletteSetEntries       *block = (CorePaletteSetEntries*) alloca( sizeof(CorePaletteSetEntries) + sizeof(DFBColor) * num );
    CorePaletteSetEntriesReturn  return_block;

    D_DEBUG_AT( DirectFB_CorePalette, "IPalette_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( colors != NULL );

    block->num = num;
    block->offset = offset;
    direct_memcpy( (char*) (block + 1), colors, sizeof(DFBColor) * num );

    ret = (DFBResult) CorePalette_Call( obj, FCEF_NONE, CorePalette_SetEntries, block, sizeof(CorePaletteSetEntries) + sizeof(DFBColor) * num, &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CorePalette_Call( CorePalette_SetEntries ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CorePalette_SetEntries failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}


DFBResult
IPalette_Requestor::SetEntriesYUV(
                    const DFBColorYUV                         *colors,
                    u32                                        num,
                    u32                                        offset
)
{
    DFBResult           ret;
    CorePaletteSetEntriesYUV       *block = (CorePaletteSetEntriesYUV*) alloca( sizeof(CorePaletteSetEntriesYUV) + sizeof(DFBColorYUV) * num );
    CorePaletteSetEntriesYUVReturn  return_block;

    D_DEBUG_AT( DirectFB_CorePalette, "IPalette_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( colors != NULL );

    block->num = num;
    block->offset = offset;
    direct_memcpy( (char*) (block + 1), colors, sizeof(DFBColorYUV) * num );

    ret = (DFBResult) CorePalette_Call( obj, FCEF_NONE, CorePalette_SetEntriesYUV, block, sizeof(CorePaletteSetEntriesYUV) + sizeof(DFBColorYUV) * num, &return_block, sizeof(return_block), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CorePalette_Call( CorePalette_SetEntriesYUV ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_block.result) {
         D_DERROR( return_block.result, "%s: CorePalette_SetEntriesYUV failed!\n", __FUNCTION__ );
         return return_block.result;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
CorePaletteDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CorePalette, "CorePaletteDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case CorePalette_SetEntries: {
            D_UNUSED
            CorePaletteSetEntries       *args        = (CorePaletteSetEntries *) ptr;
            CorePaletteSetEntriesReturn *return_args = (CorePaletteSetEntriesReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CorePalette, "=-> CorePalette_SetEntries\n" );

            return_args->result = real->SetEntries( (DFBColor*) ((char*)(args + 1)), args->num, args->offset );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CorePaletteSetEntriesReturn);

            return DFB_OK;
        }

        case CorePalette_SetEntriesYUV: {
            D_UNUSED
            CorePaletteSetEntriesYUV       *args        = (CorePaletteSetEntriesYUV *) ptr;
            CorePaletteSetEntriesYUVReturn *return_args = (CorePaletteSetEntriesYUVReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CorePalette, "=-> CorePalette_SetEntriesYUV\n" );

            return_args->result = real->SetEntriesYUV( (DFBColorYUV*) ((char*)(args + 1)), args->num, args->offset );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CorePaletteSetEntriesYUVReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
