/*
   (c) Copyright 2001-2007  directfb.org
   (c) Copyright 2000-2004  convergence (integrated) media GmbH.

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org> and
              Ville Syrj�l� <syrjala@sci.fi>.

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

#ifndef __SAWMAN_DRAW_H__
#define __SAWMAN_DRAW_H__

#include <directfb.h>

#include <core/coretypes.h>

#include <sawman_types.h>


void sawman_draw_cursor    ( CoreWindowStack *stack,
                             CardState       *state,
                             CoreSurface     *surface,
                             DFBRegion       *region,
                             int              x,
                             int              y );

void sawman_draw_window    ( SaWManTier      *tier,
                             SaWManWindow    *sawwin,
                             CardState       *state,
                             DFBRegion       *pregion,
                             bool             alpha_channel,
                             bool             right_eye );

void sawman_draw_two_windows( SaWManTier      *tier,
                              SaWManWindow    *sawwin1,
                              SaWManWindow    *sawwin2,
                              CardState       *state,
                              DFBRegion       *pregion,
                              bool             right_eye );

void sawman_draw_background( SaWManTier      *tier,
                             CardState       *state,
                             DFBRegion       *region );

#endif

