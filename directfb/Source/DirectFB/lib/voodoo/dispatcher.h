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

#ifndef __VOODOO__DISPATCHER_H__
#define __VOODOO__DISPATCHER_H__

extern "C" {
#include <voodoo/types.h>
}


class VoodooDispatcher {
private:
     int                         magic;

     VoodooManager              *manager;

     DirectMutex                 lock;
     DirectWaitQueue             queue;

     DirectThread               *dispatch_loop;

     DirectLink                 *packets;


public:
     VoodooDispatcher( VoodooManager *manager );

     ~VoodooDispatcher();

     bool Ready    ();
     void PutPacket( VoodooPacket *packet );


private:
     void        *DispatchLoop();

     static void *DispatchLoopMain( DirectThread        *thread,
                                    void                *arg );

     void         ProcessMessages ( VoodooMessageHeader *first,
                                    size_t               total_length );
};


#endif
