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

namespace DirectFB {


DFBResult
ISaWManProcess_Real::SetExiting(
)
{
     D_DEBUG_AT( DirectFB_SaWManProcess, "%s()", __FUNCTION__ );

     /* Set 'cleanly exiting' flag. */
     obj->flags = (SaWManProcessFlags)(obj->flags | SWMPF_EXITING);

     return DFB_OK;
}

DFBResult
ISaWManProcess_Real::RegisterManager(
                    const SaWManRegisterManagerData           *data,
                    SaWManManager                            **ret_manager
)
{
     D_DEBUG_AT( DirectFB_SaWManProcess, "%s()", __FUNCTION__ );

     if (m_sawman->manager.present)
          return DFB_BUSY;

     /* Initialize manager data. */
     m_sawman->manager.call      = data->call;
     m_sawman->manager.callbacks = data->callbacks;
     m_sawman->manager.context   = data->context;

     /* Set manager flag for our process. */
     obj->flags = (SaWManProcessFlags)(obj->flags | SWMPF_MANAGER);

     /* Activate it at last. */
     m_sawman->manager.present = true;

     SaWManManager_Init_Dispatch( core, &m_sawman->manager, &m_sawman->manager.call_from );

     *ret_manager = &m_sawman->manager;

     return DFB_OK;
}


}

