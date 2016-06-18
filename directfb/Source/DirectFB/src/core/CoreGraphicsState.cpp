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

#include "CoreGraphicsState.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreGraphicsState, "DirectFB/CoreGraphicsState", "DirectFB CoreGraphicsState" );

/*********************************************************************************************************************/

DFBResult
CoreGraphicsState_SetDrawingFlags(
                    CoreGraphicsState                         *obj,
                    DFBSurfaceDrawingFlags                     flags
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetDrawingFlags( flags );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetDrawingFlags( flags );
}

DFBResult
CoreGraphicsState_SetBlittingFlags(
                    CoreGraphicsState                         *obj,
                    DFBSurfaceBlittingFlags                    flags
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetBlittingFlags( flags );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetBlittingFlags( flags );
}

DFBResult
CoreGraphicsState_SetClip(
                    CoreGraphicsState                         *obj,
                    const DFBRegion                           *region
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetClip( region );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetClip( region );
}

DFBResult
CoreGraphicsState_SetColor(
                    CoreGraphicsState                         *obj,
                    const DFBColor                            *color
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetColor( color );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetColor( color );
}

DFBResult
CoreGraphicsState_SetSrcBlend(
                    CoreGraphicsState                         *obj,
                    DFBSurfaceBlendFunction                    function
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetSrcBlend( function );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetSrcBlend( function );
}

DFBResult
CoreGraphicsState_SetDstBlend(
                    CoreGraphicsState                         *obj,
                    DFBSurfaceBlendFunction                    function
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetDstBlend( function );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetDstBlend( function );
}

DFBResult
CoreGraphicsState_SetSrcColorKey(
                    CoreGraphicsState                         *obj,
                    u32                                        key
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetSrcColorKey( key );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetSrcColorKey( key );
}

DFBResult
CoreGraphicsState_SetDstColorKey(
                    CoreGraphicsState                         *obj,
                    u32                                        key
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetDstColorKey( key );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetDstColorKey( key );
}

DFBResult
CoreGraphicsState_SetDestination(
                    CoreGraphicsState                         *obj,
                    CoreSurface                               *surface
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetDestination( surface );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetDestination( surface );
}

DFBResult
CoreGraphicsState_SetSource(
                    CoreGraphicsState                         *obj,
                    CoreSurface                               *surface
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetSource( surface );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetSource( surface );
}

DFBResult
CoreGraphicsState_SetSourceMask(
                    CoreGraphicsState                         *obj,
                    CoreSurface                               *surface
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetSourceMask( surface );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetSourceMask( surface );
}

DFBResult
CoreGraphicsState_SetSourceMaskVals(
                    CoreGraphicsState                         *obj,
                    const DFBPoint                            *offset,
                    DFBSurfaceMaskFlags                        flags
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetSourceMaskVals( offset, flags );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetSourceMaskVals( offset, flags );
}

DFBResult
CoreGraphicsState_SetIndexTranslation(
                    CoreGraphicsState                         *obj,
                    const s32                                 *indices,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetIndexTranslation( indices, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetIndexTranslation( indices, num );
}

DFBResult
CoreGraphicsState_SetColorKey(
                    CoreGraphicsState                         *obj,
                    const DFBColorKey                         *key
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetColorKey( key );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetColorKey( key );
}

DFBResult
CoreGraphicsState_SetRenderOptions(
                    CoreGraphicsState                         *obj,
                    DFBSurfaceRenderOptions                    options
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetRenderOptions( options );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetRenderOptions( options );
}

DFBResult
CoreGraphicsState_SetMatrix(
                    CoreGraphicsState                         *obj,
                    const s32                                 *values
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetMatrix( values );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetMatrix( values );
}

DFBResult
CoreGraphicsState_SetSource2(
                    CoreGraphicsState                         *obj,
                    CoreSurface                               *surface
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.SetSource2( surface );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.SetSource2( surface );
}

DFBResult
CoreGraphicsState_DrawRectangles(
                    CoreGraphicsState                         *obj,
                    const DFBRectangle                        *rects,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.DrawRectangles( rects, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.DrawRectangles( rects, num );
}

DFBResult
CoreGraphicsState_DrawLines(
                    CoreGraphicsState                         *obj,
                    const DFBRegion                           *lines,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.DrawLines( lines, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.DrawLines( lines, num );
}

DFBResult
CoreGraphicsState_FillRectangles(
                    CoreGraphicsState                         *obj,
                    const DFBRectangle                        *rects,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.FillRectangles( rects, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.FillRectangles( rects, num );
}

DFBResult
CoreGraphicsState_FillTriangles(
                    CoreGraphicsState                         *obj,
                    const DFBTriangle                         *triangles,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.FillTriangles( triangles, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.FillTriangles( triangles, num );
}

DFBResult
CoreGraphicsState_FillTrapezoids(
                    CoreGraphicsState                         *obj,
                    const DFBTrapezoid                        *trapezoids,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.FillTrapezoids( trapezoids, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.FillTrapezoids( trapezoids, num );
}

DFBResult
CoreGraphicsState_FillSpans(
                    CoreGraphicsState                         *obj,
                    s32                                        y,
                    const DFBSpan                             *spans,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.FillSpans( y, spans, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.FillSpans( y, spans, num );
}

DFBResult
CoreGraphicsState_Blit(
                    CoreGraphicsState                         *obj,
                    const DFBRectangle                        *rects,
                    const DFBPoint                            *points,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.Blit( rects, points, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.Blit( rects, points, num );
}

DFBResult
CoreGraphicsState_Blit2(
                    CoreGraphicsState                         *obj,
                    const DFBRectangle                        *rects,
                    const DFBPoint                            *points1,
                    const DFBPoint                            *points2,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.Blit2( rects, points1, points2, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.Blit2( rects, points1, points2, num );
}

DFBResult
CoreGraphicsState_StretchBlit(
                    CoreGraphicsState                         *obj,
                    const DFBRectangle                        *srects,
                    const DFBRectangle                        *drects,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.StretchBlit( srects, drects, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.StretchBlit( srects, drects, num );
}

DFBResult
CoreGraphicsState_TileBlit(
                    CoreGraphicsState                         *obj,
                    const DFBRectangle                        *rects,
                    const DFBPoint                            *points1,
                    const DFBPoint                            *points2,
                    u32                                        num
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.TileBlit( rects, points1, points2, num );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.TileBlit( rects, points1, points2, num );
}

DFBResult
CoreGraphicsState_TextureTriangles(
                    CoreGraphicsState                         *obj,
                    const DFBVertex                           *vertices,
                    u32                                        num,
                    DFBTriangleFormation                       formation
)
{
    if (!fusion_config->secure_fusion || dfb_core_is_master( core_dfb )) {
        DirectFB::IGraphicsState_Real real( core_dfb, obj );

        return real.TextureTriangles( vertices, num, formation );
    }

    DirectFB::IGraphicsState_Requestor requestor( core_dfb, obj );

    return requestor.TextureTriangles( vertices, num, formation );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
CoreGraphicsState_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    DirectFB::CoreGraphicsStateDispatch *dispatch = (DirectFB::CoreGraphicsStateDispatch*) ctx;

    dispatch->Dispatch( caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void *CoreGraphicsState_Init_Dispatch(
                    CoreDFB              *core,
                    CoreGraphicsState    *obj,
                    FusionCall           *call)
{
    fusion_call_init3( call, CoreGraphicsState_Dispatch, obj, core->world );

    return NULL;
}

void  CoreGraphicsState_Deinit_Dispatch(FusionCall* call)
{
    fusion_call_destroy(call);
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
IGraphicsState_Requestor::SetDrawingFlags(
                    DFBSurfaceDrawingFlags                     flags
)
{
    DFBResult           ret;
    CoreGraphicsStateSetDrawingFlags       *block = (CoreGraphicsStateSetDrawingFlags*) alloca( sizeof(CoreGraphicsStateSetDrawingFlags) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );


    block->flags = flags;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetDrawingFlags, block, sizeof(CoreGraphicsStateSetDrawingFlags), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetDrawingFlags ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetBlittingFlags(
                    DFBSurfaceBlittingFlags                    flags
)
{
    DFBResult           ret;
    CoreGraphicsStateSetBlittingFlags       *block = (CoreGraphicsStateSetBlittingFlags*) alloca( sizeof(CoreGraphicsStateSetBlittingFlags) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );


    block->flags = flags;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetBlittingFlags, block, sizeof(CoreGraphicsStateSetBlittingFlags), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetBlittingFlags ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetClip(
                    const DFBRegion                           *region
)
{
    DFBResult           ret;
    CoreGraphicsStateSetClip       *block = (CoreGraphicsStateSetClip*) alloca( sizeof(CoreGraphicsStateSetClip) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( region != NULL );

    block->region = *region;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetClip, block, sizeof(CoreGraphicsStateSetClip), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetClip ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetColor(
                    const DFBColor                            *color
)
{
    DFBResult           ret;
    CoreGraphicsStateSetColor       *block = (CoreGraphicsStateSetColor*) alloca( sizeof(CoreGraphicsStateSetColor) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( color != NULL );

    block->color = *color;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetColor, block, sizeof(CoreGraphicsStateSetColor), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetColor ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetSrcBlend(
                    DFBSurfaceBlendFunction                    function
)
{
    DFBResult           ret;
    CoreGraphicsStateSetSrcBlend       *block = (CoreGraphicsStateSetSrcBlend*) alloca( sizeof(CoreGraphicsStateSetSrcBlend) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );


    block->function = function;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetSrcBlend, block, sizeof(CoreGraphicsStateSetSrcBlend), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetSrcBlend ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetDstBlend(
                    DFBSurfaceBlendFunction                    function
)
{
    DFBResult           ret;
    CoreGraphicsStateSetDstBlend       *block = (CoreGraphicsStateSetDstBlend*) alloca( sizeof(CoreGraphicsStateSetDstBlend) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );


    block->function = function;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetDstBlend, block, sizeof(CoreGraphicsStateSetDstBlend), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetDstBlend ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetSrcColorKey(
                    u32                                        key
)
{
    DFBResult           ret;
    CoreGraphicsStateSetSrcColorKey       *block = (CoreGraphicsStateSetSrcColorKey*) alloca( sizeof(CoreGraphicsStateSetSrcColorKey) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );


    block->key = key;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetSrcColorKey, block, sizeof(CoreGraphicsStateSetSrcColorKey), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetSrcColorKey ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetDstColorKey(
                    u32                                        key
)
{
    DFBResult           ret;
    CoreGraphicsStateSetDstColorKey       *block = (CoreGraphicsStateSetDstColorKey*) alloca( sizeof(CoreGraphicsStateSetDstColorKey) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );


    block->key = key;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetDstColorKey, block, sizeof(CoreGraphicsStateSetDstColorKey), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetDstColorKey ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetDestination(
                    CoreSurface                               *surface
)
{
    DFBResult           ret;
    CoreGraphicsStateSetDestination       *block = (CoreGraphicsStateSetDestination*) alloca( sizeof(CoreGraphicsStateSetDestination) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( surface != NULL );

    block->surface_id = CoreSurface_GetID( surface );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetDestination, block, sizeof(CoreGraphicsStateSetDestination), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetDestination ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetSource(
                    CoreSurface                               *surface
)
{
    DFBResult           ret;
    CoreGraphicsStateSetSource       *block = (CoreGraphicsStateSetSource*) alloca( sizeof(CoreGraphicsStateSetSource) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( surface != NULL );

    block->surface_id = CoreSurface_GetID( surface );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetSource, block, sizeof(CoreGraphicsStateSetSource), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetSource ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetSourceMask(
                    CoreSurface                               *surface
)
{
    DFBResult           ret;
    CoreGraphicsStateSetSourceMask       *block = (CoreGraphicsStateSetSourceMask*) alloca( sizeof(CoreGraphicsStateSetSourceMask) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( surface != NULL );

    block->surface_id = CoreSurface_GetID( surface );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetSourceMask, block, sizeof(CoreGraphicsStateSetSourceMask), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetSourceMask ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetSourceMaskVals(
                    const DFBPoint                            *offset,
                    DFBSurfaceMaskFlags                        flags
)
{
    DFBResult           ret;
    CoreGraphicsStateSetSourceMaskVals       *block = (CoreGraphicsStateSetSourceMaskVals*) alloca( sizeof(CoreGraphicsStateSetSourceMaskVals) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( offset != NULL );

    block->offset = *offset;
    block->flags = flags;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetSourceMaskVals, block, sizeof(CoreGraphicsStateSetSourceMaskVals), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetSourceMaskVals ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetIndexTranslation(
                    const s32                                 *indices,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateSetIndexTranslation       *block = (CoreGraphicsStateSetIndexTranslation*) alloca( sizeof(CoreGraphicsStateSetIndexTranslation) + sizeof(s32) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( indices != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), indices, sizeof(s32) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetIndexTranslation, block, sizeof(CoreGraphicsStateSetIndexTranslation) + sizeof(s32) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetIndexTranslation ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetColorKey(
                    const DFBColorKey                         *key
)
{
    DFBResult           ret;
    CoreGraphicsStateSetColorKey       *block = (CoreGraphicsStateSetColorKey*) alloca( sizeof(CoreGraphicsStateSetColorKey) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( key != NULL );

    block->key = *key;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetColorKey, block, sizeof(CoreGraphicsStateSetColorKey), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetColorKey ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetRenderOptions(
                    DFBSurfaceRenderOptions                    options
)
{
    DFBResult           ret;
    CoreGraphicsStateSetRenderOptions       *block = (CoreGraphicsStateSetRenderOptions*) alloca( sizeof(CoreGraphicsStateSetRenderOptions) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );


    block->options = options;

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetRenderOptions, block, sizeof(CoreGraphicsStateSetRenderOptions), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetRenderOptions ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetMatrix(
                    const s32                                 *values
)
{
    DFBResult           ret;
    CoreGraphicsStateSetMatrix       *block = (CoreGraphicsStateSetMatrix*) alloca( sizeof(CoreGraphicsStateSetMatrix) + sizeof(s32) * 9 );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( values != NULL );

    direct_memcpy( (char*) (block + 1), values, sizeof(s32) * 9 );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetMatrix, block, sizeof(CoreGraphicsStateSetMatrix) + sizeof(s32) * 9, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetMatrix ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::SetSource2(
                    CoreSurface                               *surface
)
{
    DFBResult           ret;
    CoreGraphicsStateSetSource2       *block = (CoreGraphicsStateSetSource2*) alloca( sizeof(CoreGraphicsStateSetSource2) );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( surface != NULL );

    block->surface_id = CoreSurface_GetID( surface );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_SetSource2, block, sizeof(CoreGraphicsStateSetSource2), NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_SetSource2 ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::DrawRectangles(
                    const DFBRectangle                        *rects,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateDrawRectangles       *block = (CoreGraphicsStateDrawRectangles*) alloca( sizeof(CoreGraphicsStateDrawRectangles) + sizeof(DFBRectangle) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( rects != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), rects, sizeof(DFBRectangle) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_DrawRectangles, block, sizeof(CoreGraphicsStateDrawRectangles) + sizeof(DFBRectangle) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_DrawRectangles ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::DrawLines(
                    const DFBRegion                           *lines,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateDrawLines       *block = (CoreGraphicsStateDrawLines*) alloca( sizeof(CoreGraphicsStateDrawLines) + sizeof(DFBRegion) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( lines != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), lines, sizeof(DFBRegion) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_DrawLines, block, sizeof(CoreGraphicsStateDrawLines) + sizeof(DFBRegion) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_DrawLines ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::FillRectangles(
                    const DFBRectangle                        *rects,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateFillRectangles       *block = (CoreGraphicsStateFillRectangles*) alloca( sizeof(CoreGraphicsStateFillRectangles) + sizeof(DFBRectangle) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( rects != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), rects, sizeof(DFBRectangle) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_FillRectangles, block, sizeof(CoreGraphicsStateFillRectangles) + sizeof(DFBRectangle) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_FillRectangles ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::FillTriangles(
                    const DFBTriangle                         *triangles,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateFillTriangles       *block = (CoreGraphicsStateFillTriangles*) alloca( sizeof(CoreGraphicsStateFillTriangles) + sizeof(DFBTriangle) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( triangles != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), triangles, sizeof(DFBTriangle) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_FillTriangles, block, sizeof(CoreGraphicsStateFillTriangles) + sizeof(DFBTriangle) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_FillTriangles ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::FillTrapezoids(
                    const DFBTrapezoid                        *trapezoids,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateFillTrapezoids       *block = (CoreGraphicsStateFillTrapezoids*) alloca( sizeof(CoreGraphicsStateFillTrapezoids) + sizeof(DFBTrapezoid) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( trapezoids != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), trapezoids, sizeof(DFBTrapezoid) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_FillTrapezoids, block, sizeof(CoreGraphicsStateFillTrapezoids) + sizeof(DFBTrapezoid) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_FillTrapezoids ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::FillSpans(
                    s32                                        y,
                    const DFBSpan                             *spans,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateFillSpans       *block = (CoreGraphicsStateFillSpans*) alloca( sizeof(CoreGraphicsStateFillSpans) + sizeof(DFBSpan) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( spans != NULL );

    block->y = y;
    block->num = num;
    direct_memcpy( (char*) (block + 1), spans, sizeof(DFBSpan) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_FillSpans, block, sizeof(CoreGraphicsStateFillSpans) + sizeof(DFBSpan) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_FillSpans ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::Blit(
                    const DFBRectangle                        *rects,
                    const DFBPoint                            *points,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateBlit       *block = (CoreGraphicsStateBlit*) alloca( sizeof(CoreGraphicsStateBlit) + sizeof(DFBRectangle) * num + sizeof(DFBPoint) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( rects != NULL );
    D_ASSERT( points != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), rects, sizeof(DFBRectangle) * num );
    direct_memcpy( (char*) (block + 1) + sizeof(DFBRectangle) * num, points, sizeof(DFBPoint) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_Blit, block, sizeof(CoreGraphicsStateBlit) + sizeof(DFBRectangle) * num + sizeof(DFBPoint) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_Blit ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::Blit2(
                    const DFBRectangle                        *rects,
                    const DFBPoint                            *points1,
                    const DFBPoint                            *points2,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateBlit2       *block = (CoreGraphicsStateBlit2*) alloca( sizeof(CoreGraphicsStateBlit2) + sizeof(DFBRectangle) * num + sizeof(DFBPoint) * num + sizeof(DFBPoint) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( rects != NULL );
    D_ASSERT( points1 != NULL );
    D_ASSERT( points2 != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), rects, sizeof(DFBRectangle) * num );
    direct_memcpy( (char*) (block + 1) + sizeof(DFBRectangle) * num, points1, sizeof(DFBPoint) * num );
    direct_memcpy( (char*) (block + 1) + sizeof(DFBRectangle) * num + sizeof(DFBPoint) * num, points2, sizeof(DFBPoint) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_Blit2, block, sizeof(CoreGraphicsStateBlit2) + sizeof(DFBRectangle) * num + sizeof(DFBPoint) * num + sizeof(DFBPoint) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_Blit2 ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::StretchBlit(
                    const DFBRectangle                        *srects,
                    const DFBRectangle                        *drects,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateStretchBlit       *block = (CoreGraphicsStateStretchBlit*) alloca( sizeof(CoreGraphicsStateStretchBlit) + sizeof(DFBRectangle) * num + sizeof(DFBRectangle) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( srects != NULL );
    D_ASSERT( drects != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), srects, sizeof(DFBRectangle) * num );
    direct_memcpy( (char*) (block + 1) + sizeof(DFBRectangle) * num, drects, sizeof(DFBRectangle) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_StretchBlit, block, sizeof(CoreGraphicsStateStretchBlit) + sizeof(DFBRectangle) * num + sizeof(DFBRectangle) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_StretchBlit ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::TileBlit(
                    const DFBRectangle                        *rects,
                    const DFBPoint                            *points1,
                    const DFBPoint                            *points2,
                    u32                                        num
)
{
    DFBResult           ret;
    CoreGraphicsStateTileBlit       *block = (CoreGraphicsStateTileBlit*) alloca( sizeof(CoreGraphicsStateTileBlit) + sizeof(DFBRectangle) * num + sizeof(DFBPoint) * num + sizeof(DFBPoint) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( rects != NULL );
    D_ASSERT( points1 != NULL );
    D_ASSERT( points2 != NULL );

    block->num = num;
    direct_memcpy( (char*) (block + 1), rects, sizeof(DFBRectangle) * num );
    direct_memcpy( (char*) (block + 1) + sizeof(DFBRectangle) * num, points1, sizeof(DFBPoint) * num );
    direct_memcpy( (char*) (block + 1) + sizeof(DFBRectangle) * num + sizeof(DFBPoint) * num, points2, sizeof(DFBPoint) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_TileBlit, block, sizeof(CoreGraphicsStateTileBlit) + sizeof(DFBRectangle) * num + sizeof(DFBPoint) * num + sizeof(DFBPoint) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_TileBlit ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}


DFBResult
IGraphicsState_Requestor::TextureTriangles(
                    const DFBVertex                           *vertices,
                    u32                                        num,
                    DFBTriangleFormation                       formation
)
{
    DFBResult           ret;
    CoreGraphicsStateTextureTriangles       *block = (CoreGraphicsStateTextureTriangles*) alloca( sizeof(CoreGraphicsStateTextureTriangles) + sizeof(DFBVertex) * num );

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "IGraphicsState_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( vertices != NULL );

    block->num = num;
    block->formation = formation;
    direct_memcpy( (char*) (block + 1), vertices, sizeof(DFBVertex) * num );

    ret = (DFBResult) CoreGraphicsState_Call( obj, FCEF_ONEWAY, CoreGraphicsState_TextureTriangles, block, sizeof(CoreGraphicsStateTextureTriangles) + sizeof(DFBVertex) * num, NULL, 0, NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreGraphicsState_Call( CoreGraphicsState_TextureTriangles ) failed!\n", __FUNCTION__ );
        return ret;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

DFBResult
CoreGraphicsStateDispatch::Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreGraphicsState, "CoreGraphicsStateDispatch::%s()\n", __FUNCTION__ );

    switch (method) {
        case CoreGraphicsState_SetDrawingFlags: {
            D_UNUSED
            CoreGraphicsStateSetDrawingFlags       *args        = (CoreGraphicsStateSetDrawingFlags *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetDrawingFlags\n" );

            real->SetDrawingFlags( args->flags );

            return DFB_OK;
        }

        case CoreGraphicsState_SetBlittingFlags: {
            D_UNUSED
            CoreGraphicsStateSetBlittingFlags       *args        = (CoreGraphicsStateSetBlittingFlags *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetBlittingFlags\n" );

            real->SetBlittingFlags( args->flags );

            return DFB_OK;
        }

        case CoreGraphicsState_SetClip: {
            D_UNUSED
            CoreGraphicsStateSetClip       *args        = (CoreGraphicsStateSetClip *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetClip\n" );

            real->SetClip( &args->region );

            return DFB_OK;
        }

        case CoreGraphicsState_SetColor: {
            D_UNUSED
            CoreGraphicsStateSetColor       *args        = (CoreGraphicsStateSetColor *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetColor\n" );

            real->SetColor( &args->color );

            return DFB_OK;
        }

        case CoreGraphicsState_SetSrcBlend: {
            D_UNUSED
            CoreGraphicsStateSetSrcBlend       *args        = (CoreGraphicsStateSetSrcBlend *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetSrcBlend\n" );

            real->SetSrcBlend( args->function );

            return DFB_OK;
        }

        case CoreGraphicsState_SetDstBlend: {
            D_UNUSED
            CoreGraphicsStateSetDstBlend       *args        = (CoreGraphicsStateSetDstBlend *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetDstBlend\n" );

            real->SetDstBlend( args->function );

            return DFB_OK;
        }

        case CoreGraphicsState_SetSrcColorKey: {
            D_UNUSED
            CoreGraphicsStateSetSrcColorKey       *args        = (CoreGraphicsStateSetSrcColorKey *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetSrcColorKey\n" );

            real->SetSrcColorKey( args->key );

            return DFB_OK;
        }

        case CoreGraphicsState_SetDstColorKey: {
            D_UNUSED
            CoreGraphicsStateSetDstColorKey       *args        = (CoreGraphicsStateSetDstColorKey *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetDstColorKey\n" );

            real->SetDstColorKey( args->key );

            return DFB_OK;
        }

        case CoreGraphicsState_SetDestination: {
    CoreSurface *surface = NULL;
            D_UNUSED
            CoreGraphicsStateSetDestination       *args        = (CoreGraphicsStateSetDestination *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetDestination\n" );

            ret = (DFBResult) CoreSurface_Lookup( core, args->surface_id, caller, &surface );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up surface by ID %u failed!\n", __FUNCTION__, args->surface_id );
                 return DFB_OK;
            }

            real->SetDestination( surface );

            if (surface)
                CoreSurface_Unref( surface );

            return DFB_OK;
        }

        case CoreGraphicsState_SetSource: {
    CoreSurface *surface = NULL;
            D_UNUSED
            CoreGraphicsStateSetSource       *args        = (CoreGraphicsStateSetSource *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetSource\n" );

            ret = (DFBResult) CoreSurface_Lookup( core, args->surface_id, caller, &surface );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up surface by ID %u failed!\n", __FUNCTION__, args->surface_id );
                 return DFB_OK;
            }

            real->SetSource( surface );

            if (surface)
                CoreSurface_Unref( surface );

            return DFB_OK;
        }

        case CoreGraphicsState_SetSourceMask: {
    CoreSurface *surface = NULL;
            D_UNUSED
            CoreGraphicsStateSetSourceMask       *args        = (CoreGraphicsStateSetSourceMask *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetSourceMask\n" );

            ret = (DFBResult) CoreSurface_Lookup( core, args->surface_id, caller, &surface );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up surface by ID %u failed!\n", __FUNCTION__, args->surface_id );
                 return DFB_OK;
            }

            real->SetSourceMask( surface );

            if (surface)
                CoreSurface_Unref( surface );

            return DFB_OK;
        }

        case CoreGraphicsState_SetSourceMaskVals: {
            D_UNUSED
            CoreGraphicsStateSetSourceMaskVals       *args        = (CoreGraphicsStateSetSourceMaskVals *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetSourceMaskVals\n" );

            real->SetSourceMaskVals( &args->offset, args->flags );

            return DFB_OK;
        }

        case CoreGraphicsState_SetIndexTranslation: {
            D_UNUSED
            CoreGraphicsStateSetIndexTranslation       *args        = (CoreGraphicsStateSetIndexTranslation *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetIndexTranslation\n" );

            real->SetIndexTranslation( (s32*) ((char*)(args + 1)), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_SetColorKey: {
            D_UNUSED
            CoreGraphicsStateSetColorKey       *args        = (CoreGraphicsStateSetColorKey *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetColorKey\n" );

            real->SetColorKey( &args->key );

            return DFB_OK;
        }

        case CoreGraphicsState_SetRenderOptions: {
            D_UNUSED
            CoreGraphicsStateSetRenderOptions       *args        = (CoreGraphicsStateSetRenderOptions *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetRenderOptions\n" );

            real->SetRenderOptions( args->options );

            return DFB_OK;
        }

        case CoreGraphicsState_SetMatrix: {
            D_UNUSED
            CoreGraphicsStateSetMatrix       *args        = (CoreGraphicsStateSetMatrix *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetMatrix\n" );

            real->SetMatrix( (s32*) ((char*)(args + 1)) );

            return DFB_OK;
        }

        case CoreGraphicsState_SetSource2: {
    CoreSurface *surface = NULL;
            D_UNUSED
            CoreGraphicsStateSetSource2       *args        = (CoreGraphicsStateSetSource2 *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_SetSource2\n" );

            ret = (DFBResult) CoreSurface_Lookup( core, args->surface_id, caller, &surface );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up surface by ID %u failed!\n", __FUNCTION__, args->surface_id );
                 return DFB_OK;
            }

            real->SetSource2( surface );

            if (surface)
                CoreSurface_Unref( surface );

            return DFB_OK;
        }

        case CoreGraphicsState_DrawRectangles: {
            D_UNUSED
            CoreGraphicsStateDrawRectangles       *args        = (CoreGraphicsStateDrawRectangles *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_DrawRectangles\n" );

            real->DrawRectangles( (DFBRectangle*) ((char*)(args + 1)), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_DrawLines: {
            D_UNUSED
            CoreGraphicsStateDrawLines       *args        = (CoreGraphicsStateDrawLines *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_DrawLines\n" );

            real->DrawLines( (DFBRegion*) ((char*)(args + 1)), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_FillRectangles: {
            D_UNUSED
            CoreGraphicsStateFillRectangles       *args        = (CoreGraphicsStateFillRectangles *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_FillRectangles\n" );

            real->FillRectangles( (DFBRectangle*) ((char*)(args + 1)), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_FillTriangles: {
            D_UNUSED
            CoreGraphicsStateFillTriangles       *args        = (CoreGraphicsStateFillTriangles *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_FillTriangles\n" );

            real->FillTriangles( (DFBTriangle*) ((char*)(args + 1)), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_FillTrapezoids: {
            D_UNUSED
            CoreGraphicsStateFillTrapezoids       *args        = (CoreGraphicsStateFillTrapezoids *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_FillTrapezoids\n" );

            real->FillTrapezoids( (DFBTrapezoid*) ((char*)(args + 1)), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_FillSpans: {
            D_UNUSED
            CoreGraphicsStateFillSpans       *args        = (CoreGraphicsStateFillSpans *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_FillSpans\n" );

            real->FillSpans( args->y, (DFBSpan*) ((char*)(args + 1)), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_Blit: {
            D_UNUSED
            CoreGraphicsStateBlit       *args        = (CoreGraphicsStateBlit *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_Blit\n" );

            real->Blit( (DFBRectangle*) ((char*)(args + 1)), (DFBPoint*) ((char*)(args + 1) + sizeof(DFBRectangle) * args->num), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_Blit2: {
            D_UNUSED
            CoreGraphicsStateBlit2       *args        = (CoreGraphicsStateBlit2 *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_Blit2\n" );

            real->Blit2( (DFBRectangle*) ((char*)(args + 1)), (DFBPoint*) ((char*)(args + 1) + sizeof(DFBRectangle) * args->num), (DFBPoint*) ((char*)(args + 1) + sizeof(DFBRectangle) * args->num + sizeof(DFBPoint) * args->num), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_StretchBlit: {
            D_UNUSED
            CoreGraphicsStateStretchBlit       *args        = (CoreGraphicsStateStretchBlit *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_StretchBlit\n" );

            real->StretchBlit( (DFBRectangle*) ((char*)(args + 1)), (DFBRectangle*) ((char*)(args + 1) + sizeof(DFBRectangle) * args->num), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_TileBlit: {
            D_UNUSED
            CoreGraphicsStateTileBlit       *args        = (CoreGraphicsStateTileBlit *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_TileBlit\n" );

            real->TileBlit( (DFBRectangle*) ((char*)(args + 1)), (DFBPoint*) ((char*)(args + 1) + sizeof(DFBRectangle) * args->num), (DFBPoint*) ((char*)(args + 1) + sizeof(DFBRectangle) * args->num + sizeof(DFBPoint) * args->num), args->num );

            return DFB_OK;
        }

        case CoreGraphicsState_TextureTriangles: {
            D_UNUSED
            CoreGraphicsStateTextureTriangles       *args        = (CoreGraphicsStateTextureTriangles *) ptr;

            D_DEBUG_AT( DirectFB_CoreGraphicsState, "=-> CoreGraphicsState_TextureTriangles\n" );

            real->TextureTriangles( (DFBVertex*) ((char*)(args + 1)), args->num, args->formation );

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}

}
