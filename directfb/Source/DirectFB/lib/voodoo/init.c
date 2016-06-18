/*
   (c) Copyright 2001-2008  The world wide DirectFB Open Source Community (directfb.org)
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

#include <direct/util.h>

#include <voodoo/conf.h>
#include <voodoo/init.h>
#include <voodoo/play.h>


/**********************************************************************************************************************/

typedef void (*Func)( void );


static Func init_funcs[] = {
     __Voodoo_conf_init,
     __Voodoo_play_init,
};

static Func deinit_funcs[] = {
     __Voodoo_play_deinit,
     __Voodoo_conf_deinit,
};

/**********************************************************************************************************************/

void
__Voodoo_init_all()
{
     size_t i;

     for (i=0; i<D_ARRAY_SIZE(init_funcs); i++)
          init_funcs[i]();
}

void
__Voodoo_deinit_all()
{
     size_t i;

     for (i=0; i<D_ARRAY_SIZE(deinit_funcs); i++)
          deinit_funcs[i]();
}

