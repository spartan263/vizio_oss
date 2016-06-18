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

#ifndef ___CoreSurface__H___
#define ___CoreSurface__H___

#include "CoreSurface_includes.h"

/**********************************************************************************************************************
 * CoreSurface
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreSurface_SetConfig(
                    CoreSurface                               *obj,
                    const CoreSurfaceConfig                   *config);

DFBResult CoreSurface_Flip(
                    CoreSurface                               *obj,
                    bool                                       swap);

DFBResult CoreSurface_GetPalette(
                    CoreSurface                               *obj,
                    CorePalette                              **ret_palette);

DFBResult CoreSurface_SetPalette(
                    CoreSurface                               *obj,
                    CorePalette                               *palette);

DFBResult CoreSurface_PreLockBuffer(
                    CoreSurface                               *obj,
                    u32                                        buffer_index,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    u32                                       *ret_allocation_index);

DFBResult CoreSurface_PreReadBuffer(
                    CoreSurface                               *obj,
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index);

DFBResult CoreSurface_PreWriteBuffer(
                    CoreSurface                               *obj,
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index);


void *CoreSurface_Init_Dispatch(
                    CoreDFB              *core,
                    CoreSurface          *obj,
                    FusionCall           *call
);

void  CoreSurface_Deinit_Dispatch(FusionCall *call);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreSurface Calls
 */
typedef enum {
    CoreSurface_SetConfig = 1,
    CoreSurface_Flip = 2,
    CoreSurface_GetPalette = 3,
    CoreSurface_SetPalette = 4,
    CoreSurface_PreLockBuffer = 5,
    CoreSurface_PreReadBuffer = 6,
    CoreSurface_PreWriteBuffer = 7,
} CoreSurfaceCall;

/*
 * CoreSurface_SetConfig
 */
typedef struct {
    CoreSurfaceConfig                          config;
} CoreSurfaceSetConfig;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceSetConfigReturn;


/*
 * CoreSurface_Flip
 */
typedef struct {
    bool                                       swap;
} CoreSurfaceFlip;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceFlipReturn;


/*
 * CoreSurface_GetPalette
 */
typedef struct {
} CoreSurfaceGetPalette;

typedef struct {
    DFBResult                                  result;
    u32                                        palette_id;
} CoreSurfaceGetPaletteReturn;


/*
 * CoreSurface_SetPalette
 */
typedef struct {
    u32                                        palette_id;
} CoreSurfaceSetPalette;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceSetPaletteReturn;


/*
 * CoreSurface_PreLockBuffer
 */
typedef struct {
    u32                                        buffer_index;
    CoreSurfaceAccessorID                      accessor;
    CoreSurfaceAccessFlags                     access;
} CoreSurfacePreLockBuffer;

typedef struct {
    DFBResult                                  result;
    u32                                        allocation_index;
} CoreSurfacePreLockBufferReturn;


/*
 * CoreSurface_PreReadBuffer
 */
typedef struct {
    u32                                        buffer_index;
    DFBRectangle                               rect;
} CoreSurfacePreReadBuffer;

typedef struct {
    DFBResult                                  result;
    u32                                        allocation_index;
} CoreSurfacePreReadBufferReturn;


/*
 * CoreSurface_PreWriteBuffer
 */
typedef struct {
    u32                                        buffer_index;
    DFBRectangle                               rect;
} CoreSurfacePreWriteBuffer;

typedef struct {
    DFBResult                                  result;
    u32                                        allocation_index;
} CoreSurfacePreWriteBufferReturn;





class ISurface : public Interface
{
public:
    ISurface( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult SetConfig(
                    const CoreSurfaceConfig                   *config
    ) = 0;

    virtual DFBResult Flip(
                    bool                                       swap
    ) = 0;

    virtual DFBResult GetPalette(
                    CorePalette                              **ret_palette
    ) = 0;

    virtual DFBResult SetPalette(
                    CorePalette                               *palette
    ) = 0;

    virtual DFBResult PreLockBuffer(
                    u32                                        buffer_index,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    u32                                       *ret_allocation_index
    ) = 0;

    virtual DFBResult PreReadBuffer(
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
    ) = 0;

    virtual DFBResult PreWriteBuffer(
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
    ) = 0;

};



class ISurface_Real : public ISurface
{
private:
    CoreSurface *obj;

public:
    ISurface_Real( CoreDFB *core, CoreSurface *obj )
        :
        ISurface( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetConfig(
                    const CoreSurfaceConfig                   *config
    );

    virtual DFBResult Flip(
                    bool                                       swap
    );

    virtual DFBResult GetPalette(
                    CorePalette                              **ret_palette
    );

    virtual DFBResult SetPalette(
                    CorePalette                               *palette
    );

    virtual DFBResult PreLockBuffer(
                    u32                                        buffer_index,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    u32                                       *ret_allocation_index
    );

    virtual DFBResult PreReadBuffer(
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
    );

    virtual DFBResult PreWriteBuffer(
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
    );

};



class ISurface_Requestor : public ISurface
{
private:
    CoreSurface *obj;

public:
    ISurface_Requestor( CoreDFB *core, CoreSurface *obj )
        :
        ISurface( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetConfig(
                    const CoreSurfaceConfig                   *config
    );

    virtual DFBResult Flip(
                    bool                                       swap
    );

    virtual DFBResult GetPalette(
                    CorePalette                              **ret_palette
    );

    virtual DFBResult SetPalette(
                    CorePalette                               *palette
    );

    virtual DFBResult PreLockBuffer(
                    u32                                        buffer_index,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    u32                                       *ret_allocation_index
    );

    virtual DFBResult PreReadBuffer(
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
    );

    virtual DFBResult PreWriteBuffer(
                    u32                                        buffer_index,
                    const DFBRectangle                        *rect,
                    u32                                       *ret_allocation_index
    );

};



class CoreSurfaceDispatch
{

public:
    CoreSurfaceDispatch( CoreDFB *core, ISurface *real )
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
    ISurface             *real;
};


}

#endif

#endif
