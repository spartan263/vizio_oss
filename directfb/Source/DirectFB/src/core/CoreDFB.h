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

#ifndef ___CoreDFB__H___
#define ___CoreDFB__H___

#include "CoreDFB_includes.h"

/**********************************************************************************************************************
 * CoreDFB
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreDFB_CreateSurface(
                    CoreDFB                                   *obj,
                    const CoreSurfaceConfig                   *config,
                    CoreSurfaceTypeFlags                       type,
                    u64                                        resource_id,
                    CorePalette                               *palette,
                    CoreSurface                              **ret_surface);

DFBResult CoreDFB_CreatePalette(
                    CoreDFB                                   *obj,
                    u32                                        size,
                    CorePalette                              **ret_palette);

DFBResult CoreDFB_CreateState(
                    CoreDFB                                   *obj,
                    CoreGraphicsState                        **ret_state);

DFBResult CoreDFB_WaitIdle(
                    CoreDFB                                   *obj
);


void *CoreDFB_Init_Dispatch(
                    CoreDFB              *core,
                    CoreDFB              *obj,
                    FusionCall           *call
);

void  CoreDFB_Deinit_Dispatch(FusionCall *call);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreDFB Calls
 */
typedef enum {
    CoreDFB_CreateSurface = 1,
    CoreDFB_CreatePalette = 2,
    CoreDFB_CreateState = 3,
    CoreDFB_WaitIdle = 4,
} CoreDFBCall;

/*
 * CoreDFB_CreateSurface
 */
typedef struct {
    CoreSurfaceConfig                          config;
    CoreSurfaceTypeFlags                       type;
    u64                                        resource_id;
    bool                                       palette_set;
    u32                                        palette_id;
} CoreDFBCreateSurface;

typedef struct {
    DFBResult                                  result;
    u32                                        surface_id;
} CoreDFBCreateSurfaceReturn;


/*
 * CoreDFB_CreatePalette
 */
typedef struct {
    u32                                        size;
} CoreDFBCreatePalette;

typedef struct {
    DFBResult                                  result;
    u32                                        palette_id;
} CoreDFBCreatePaletteReturn;


/*
 * CoreDFB_CreateState
 */
typedef struct {
} CoreDFBCreateState;

typedef struct {
    DFBResult                                  result;
    u32                                        state_id;
} CoreDFBCreateStateReturn;


/*
 * CoreDFB_WaitIdle
 */
typedef struct {
} CoreDFBWaitIdle;

typedef struct {
    DFBResult                                  result;
} CoreDFBWaitIdleReturn;





class ICore : public Interface
{
public:
    ICore( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult CreateSurface(
                    const CoreSurfaceConfig                   *config,
                    CoreSurfaceTypeFlags                       type,
                    u64                                        resource_id,
                    CorePalette                               *palette,
                    CoreSurface                              **ret_surface
    ) = 0;

    virtual DFBResult CreatePalette(
                    u32                                        size,
                    CorePalette                              **ret_palette
    ) = 0;

    virtual DFBResult CreateState(
                    CoreGraphicsState                        **ret_state
    ) = 0;

    virtual DFBResult WaitIdle(

    ) = 0;

};



class ICore_Real : public ICore
{
private:
    CoreDFB *obj;

public:
    ICore_Real( CoreDFB *core, CoreDFB *obj )
        :
        ICore( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult CreateSurface(
                    const CoreSurfaceConfig                   *config,
                    CoreSurfaceTypeFlags                       type,
                    u64                                        resource_id,
                    CorePalette                               *palette,
                    CoreSurface                              **ret_surface
    );

    virtual DFBResult CreatePalette(
                    u32                                        size,
                    CorePalette                              **ret_palette
    );

    virtual DFBResult CreateState(
                    CoreGraphicsState                        **ret_state
    );

    virtual DFBResult WaitIdle(

    );

};



class ICore_Requestor : public ICore
{
private:
    CoreDFB *obj;

public:
    ICore_Requestor( CoreDFB *core, CoreDFB *obj )
        :
        ICore( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult CreateSurface(
                    const CoreSurfaceConfig                   *config,
                    CoreSurfaceTypeFlags                       type,
                    u64                                        resource_id,
                    CorePalette                               *palette,
                    CoreSurface                              **ret_surface
    );

    virtual DFBResult CreatePalette(
                    u32                                        size,
                    CorePalette                              **ret_palette
    );

    virtual DFBResult CreateState(
                    CoreGraphicsState                        **ret_state
    );

    virtual DFBResult WaitIdle(

    );

};



class CoreDFBDispatch
{

public:
    CoreDFBDispatch( CoreDFB *core, ICore *real )
        :
        core( core ),
        real( real )
    {
    }


    virtual DFBResult Dispatch( FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length );

private:
    CoreDFB              *core;
    ICore                *real;
};


}

#endif

#endif
