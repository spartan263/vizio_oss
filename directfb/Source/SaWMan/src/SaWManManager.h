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

#ifndef ___SaWManManager__H___
#define ___SaWManManager__H___

#include "SaWManManager_includes.h"

/**********************************************************************************************************************
 * SaWManManager
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult SaWManManager_QueueUpdate(
                    SaWManManager                             *obj,
                    DFBWindowStackingClass                     stacking,
                    const DFBRegion                           *update);

DFBResult SaWManManager_ProcessUpdates(
                    SaWManManager                             *obj,
                    DFBSurfaceFlipFlags                        flags);

DFBResult SaWManManager_CloseWindow(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window);

DFBResult SaWManManager_InsertWindow(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window,
                    SaWManWindow                              *relative,
                    SaWManWindowRelation                       relation);

DFBResult SaWManManager_RemoveWindow(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window);

DFBResult SaWManManager_SwitchFocus(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window);

DFBResult SaWManManager_SetScalingMode(
                    SaWManManager                             *obj,
                    SaWManScalingMode                          mode);

DFBResult SaWManManager_SetWindowConfig(
                    SaWManManager                             *obj,
                    SaWManWindow                              *window,
                    const SaWManWindowConfig                  *config,
                    SaWManWindowConfigFlags                    flags);


void *SaWManManager_Init_Dispatch(
                    CoreDFB              *core,
                    SaWManManager        *obj,
                    FusionCall           *call
);

void  SaWManManager_Deinit_Dispatch(FusionCall *call);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * SaWManManager Calls
 */
typedef enum {
    SaWManManager_QueueUpdate = 1,
    SaWManManager_ProcessUpdates = 2,
    SaWManManager_CloseWindow = 3,
    SaWManManager_InsertWindow = 4,
    SaWManManager_RemoveWindow = 5,
    SaWManManager_SwitchFocus = 6,
    SaWManManager_SetScalingMode = 7,
    SaWManManager_SetWindowConfig = 8,
} SaWManManagerCall;

/*
 * SaWManManager_QueueUpdate
 */
typedef struct {
    DFBWindowStackingClass                     stacking;
    bool                                       update_set;
    DFBRegion                                  update;
} SaWManManagerQueueUpdate;

typedef struct {
    DFBResult                                  result;
} SaWManManagerQueueUpdateReturn;


/*
 * SaWManManager_ProcessUpdates
 */
typedef struct {
    DFBSurfaceFlipFlags                        flags;
} SaWManManagerProcessUpdates;

typedef struct {
    DFBResult                                  result;
} SaWManManagerProcessUpdatesReturn;


/*
 * SaWManManager_CloseWindow
 */
typedef struct {
    u32                                        window_id;
} SaWManManagerCloseWindow;

typedef struct {
    DFBResult                                  result;
} SaWManManagerCloseWindowReturn;


/*
 * SaWManManager_InsertWindow
 */
typedef struct {
    u32                                        window_id;
    bool                                       relative_set;
    u32                                        relative_id;
    SaWManWindowRelation                       relation;
} SaWManManagerInsertWindow;

typedef struct {
    DFBResult                                  result;
} SaWManManagerInsertWindowReturn;


/*
 * SaWManManager_RemoveWindow
 */
typedef struct {
    u32                                        window_id;
} SaWManManagerRemoveWindow;

typedef struct {
    DFBResult                                  result;
} SaWManManagerRemoveWindowReturn;


/*
 * SaWManManager_SwitchFocus
 */
typedef struct {
    u32                                        window_id;
} SaWManManagerSwitchFocus;

typedef struct {
    DFBResult                                  result;
} SaWManManagerSwitchFocusReturn;


/*
 * SaWManManager_SetScalingMode
 */
typedef struct {
    SaWManScalingMode                          mode;
} SaWManManagerSetScalingMode;

typedef struct {
    DFBResult                                  result;
} SaWManManagerSetScalingModeReturn;


/*
 * SaWManManager_SetWindowConfig
 */
typedef struct {
    u32                                        window_id;
    SaWManWindowConfig                         config;
    SaWManWindowConfigFlags                    flags;
} SaWManManagerSetWindowConfig;

typedef struct {
    DFBResult                                  result;
} SaWManManagerSetWindowConfigReturn;





class ISaWManManager : public Interface
{
public:
    ISaWManManager( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult QueueUpdate(
                    DFBWindowStackingClass                     stacking,
                    const DFBRegion                           *update
    ) = 0;

    virtual DFBResult ProcessUpdates(
                    DFBSurfaceFlipFlags                        flags
    ) = 0;

    virtual DFBResult CloseWindow(
                    SaWManWindow                              *window
    ) = 0;

    virtual DFBResult InsertWindow(
                    SaWManWindow                              *window,
                    SaWManWindow                              *relative,
                    SaWManWindowRelation                       relation
    ) = 0;

    virtual DFBResult RemoveWindow(
                    SaWManWindow                              *window
    ) = 0;

    virtual DFBResult SwitchFocus(
                    SaWManWindow                              *window
    ) = 0;

    virtual DFBResult SetScalingMode(
                    SaWManScalingMode                          mode
    ) = 0;

    virtual DFBResult SetWindowConfig(
                    SaWManWindow                              *window,
                    const SaWManWindowConfig                  *config,
                    SaWManWindowConfigFlags                    flags
    ) = 0;

};



class ISaWManManager_Real : public ISaWManManager
{
private:
    SaWManManager *obj;

public:
    ISaWManManager_Real( CoreDFB *core, SaWManManager *obj )
        :
        ISaWManManager( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult QueueUpdate(
                    DFBWindowStackingClass                     stacking,
                    const DFBRegion                           *update
    );

    virtual DFBResult ProcessUpdates(
                    DFBSurfaceFlipFlags                        flags
    );

    virtual DFBResult CloseWindow(
                    SaWManWindow                              *window
    );

    virtual DFBResult InsertWindow(
                    SaWManWindow                              *window,
                    SaWManWindow                              *relative,
                    SaWManWindowRelation                       relation
    );

    virtual DFBResult RemoveWindow(
                    SaWManWindow                              *window
    );

    virtual DFBResult SwitchFocus(
                    SaWManWindow                              *window
    );

    virtual DFBResult SetScalingMode(
                    SaWManScalingMode                          mode
    );

    virtual DFBResult SetWindowConfig(
                    SaWManWindow                              *window,
                    const SaWManWindowConfig                  *config,
                    SaWManWindowConfigFlags                    flags
    );

};



class ISaWManManager_Requestor : public ISaWManManager
{
private:
    SaWManManager *obj;

public:
    ISaWManManager_Requestor( CoreDFB *core, SaWManManager *obj )
        :
        ISaWManManager( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult QueueUpdate(
                    DFBWindowStackingClass                     stacking,
                    const DFBRegion                           *update
    );

    virtual DFBResult ProcessUpdates(
                    DFBSurfaceFlipFlags                        flags
    );

    virtual DFBResult CloseWindow(
                    SaWManWindow                              *window
    );

    virtual DFBResult InsertWindow(
                    SaWManWindow                              *window,
                    SaWManWindow                              *relative,
                    SaWManWindowRelation                       relation
    );

    virtual DFBResult RemoveWindow(
                    SaWManWindow                              *window
    );

    virtual DFBResult SwitchFocus(
                    SaWManWindow                              *window
    );

    virtual DFBResult SetScalingMode(
                    SaWManScalingMode                          mode
    );

    virtual DFBResult SetWindowConfig(
                    SaWManWindow                              *window,
                    const SaWManWindowConfig                  *config,
                    SaWManWindowConfigFlags                    flags
    );

};



class SaWManManagerDispatch
{

public:
    SaWManManagerDispatch( CoreDFB *core, ISaWManManager *real )
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
    ISaWManManager       *real;
};


}

#endif

#endif
