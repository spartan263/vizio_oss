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

#ifndef ___SaWManProcess__H___
#define ___SaWManProcess__H___

#include "SaWManProcess_includes.h"

/**********************************************************************************************************************
 * SaWManProcess
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult SaWManProcess_SetExiting(
                    SaWManProcess                             *obj
);

DFBResult SaWManProcess_RegisterManager(
                    SaWManProcess                             *obj,
                    const SaWManRegisterManagerData           *data,
                    SaWManManager                            **ret_manager);


void *SaWManProcess_Init_Dispatch(
                    CoreDFB              *core,
                    SaWManProcess        *obj,
                    FusionCall           *call
);

void  SaWManProcess_Deinit_Dispatch(FusionCall *call);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * SaWManProcess Calls
 */
typedef enum {
    SaWManProcess_SetExiting = 1,
    SaWManProcess_RegisterManager = 2,
} SaWManProcessCall;

/*
 * SaWManProcess_SetExiting
 */
typedef struct {
} SaWManProcessSetExiting;

typedef struct {
    DFBResult                                  result;
} SaWManProcessSetExitingReturn;


/*
 * SaWManProcess_RegisterManager
 */
typedef struct {
    SaWManRegisterManagerData                  data;
} SaWManProcessRegisterManager;

typedef struct {
    DFBResult                                  result;
    u32                                        manager_id;
} SaWManProcessRegisterManagerReturn;





class ISaWManProcess : public Interface
{
public:
    ISaWManProcess( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult SetExiting(

    ) = 0;

    virtual DFBResult RegisterManager(
                    const SaWManRegisterManagerData           *data,
                    SaWManManager                            **ret_manager
    ) = 0;

};



class ISaWManProcess_Real : public ISaWManProcess
{
private:
    SaWManProcess *obj;

public:
    ISaWManProcess_Real( CoreDFB *core, SaWManProcess *obj )
        :
        ISaWManProcess( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetExiting(

    );

    virtual DFBResult RegisterManager(
                    const SaWManRegisterManagerData           *data,
                    SaWManManager                            **ret_manager
    );

};



class ISaWManProcess_Requestor : public ISaWManProcess
{
private:
    SaWManProcess *obj;

public:
    ISaWManProcess_Requestor( CoreDFB *core, SaWManProcess *obj )
        :
        ISaWManProcess( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetExiting(

    );

    virtual DFBResult RegisterManager(
                    const SaWManRegisterManagerData           *data,
                    SaWManManager                            **ret_manager
    );

};



class SaWManProcessDispatch
{

public:
    SaWManProcessDispatch( CoreDFB *core, ISaWManProcess *real )
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
    ISaWManProcess       *real;
};


}

#endif

#endif
