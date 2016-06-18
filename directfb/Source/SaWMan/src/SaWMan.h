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

#ifndef ___SaWMan__H___
#define ___SaWMan__H___

#include "SaWMan_includes.h"

/**********************************************************************************************************************
 * SaWMan
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult SaWMan_RegisterProcess(
                    SaWMan                                    *obj,
                    SaWManProcessFlags                         flags,
                    s32                                        pid,
                    u32                                        fusion_id,
                    SaWManProcess                            **ret_process);


void *SaWMan_Init_Dispatch(
                    CoreDFB              *core,
                    SaWMan               *obj,
                    FusionCall           *call
);

void  SaWMan_Deinit_Dispatch(FusionCall *call);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * SaWMan Calls
 */
typedef enum {
    SaWMan_RegisterProcess = 1,
} SaWManCall;

/*
 * SaWMan_RegisterProcess
 */
typedef struct {
    SaWManProcessFlags                         flags;
    s32                                        pid;
    u32                                        fusion_id;
} SaWManRegisterProcess;

typedef struct {
    DFBResult                                  result;
    u32                                        process_id;
} SaWManRegisterProcessReturn;





class ISaWManWM : public Interface
{
public:
    ISaWManWM( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult RegisterProcess(
                    SaWManProcessFlags                         flags,
                    s32                                        pid,
                    u32                                        fusion_id,
                    SaWManProcess                            **ret_process
    ) = 0;

};



class ISaWManWM_Real : public ISaWManWM
{
private:
    SaWMan *obj;

public:
    ISaWManWM_Real( CoreDFB *core, SaWMan *obj )
        :
        ISaWManWM( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult RegisterProcess(
                    SaWManProcessFlags                         flags,
                    s32                                        pid,
                    u32                                        fusion_id,
                    SaWManProcess                            **ret_process
    );

};



class ISaWManWM_Requestor : public ISaWManWM
{
private:
    SaWMan *obj;

public:
    ISaWManWM_Requestor( CoreDFB *core, SaWMan *obj )
        :
        ISaWManWM( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult RegisterProcess(
                    SaWManProcessFlags                         flags,
                    s32                                        pid,
                    u32                                        fusion_id,
                    SaWManProcess                            **ret_process
    );

};



class SaWManDispatch
{

public:
    SaWManDispatch( CoreDFB *core, ISaWManWM *real )
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
    ISaWManWM            *real;
};


}

#endif

#endif
