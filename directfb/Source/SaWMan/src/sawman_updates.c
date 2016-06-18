/*
   (c) Copyright 2006-2007  directfb.org

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>.

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

//#define DIRECT_ENABLE_DEBUG

#include <config.h>

#include <unistd.h>

#include <direct/debug.h>
#include <direct/list.h>

#include <fusion/conf.h>
#include <fusion/fusion.h>
#include <fusion/shmalloc.h>

#include <core/layer_context.h>
#include <core/layer_control.h>
#include <core/layer_region.h>
#include <core/palette.h>
#include <core/screen.h>
#include <core/windows_internal.h>

#include <gfx/clip.h>
#include <gfx/convert.h>
#include <gfx/util.h>

#include <misc/conf.h>

#include <sawman.h>

#include "sawman_config.h"
#include "sawman_draw.h"
#include "sawman_window.h"

#include "isawman.h"

#include "region.h"


D_DEBUG_DOMAIN( SaWMan_Auto,     "SaWMan/Auto",     "SaWMan auto configuration" );
D_DEBUG_DOMAIN( SaWMan_Update,   "SaWMan/Update",   "SaWMan window manager updates" );
D_DEBUG_DOMAIN( SaWMan_FlipOnce, "SaWMan/FlipOnce", "SaWMan window manager flip once" );
D_DEBUG_DOMAIN( SaWMan_Surface,  "SaWMan/Surface",  "SaWMan window manager surface" );

extern DFBResult mdfb_check_wait_vsync(CoreLayer* pt_this);

/**********************************************************************************************************************/

static void
update_region3( SaWMan          *sawman,
                SaWManTier      *tier,
                CardState       *state,
                int              start,
                int              x1,
                int              y1,
                int              x2,
                int              y2,
                bool             right_eye )
{
     SaWManWindow    *sawwin = 0;
     CoreWindowStack *stack;

     int winNum;
     int u;

     DFBRegion updateRegion = {x1,y1,x2,y2};

     DFBLinkRegionPool  regionpool;
     DFBLinkRegion     *regionstorage;

     DirectLink *backgroundNotNeeded = 0;
     DirectLink *backgroundNeeded    = 0;

     int blackBackground = false;

     DFBLinkRegion *lr;
     DFBRegion *r;

     D_DEBUG_AT( SaWMan_Update, "%s( %p, %d, %d,%d - %d,%d )\n", __FUNCTION__, tier, start, x1, y1, x2, y2 );

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     D_MAGIC_ASSERT( state, CardState );
     D_ASSERT( start < fusion_vector_size( &sawman->layout ) );
     D_ASSUME( x1 <= x2 );
     D_ASSUME( y1 <= y2 );

     if (x1 > x2 || y1 > y2)
          return;

     stack = tier->stack;
     D_ASSERT( stack != NULL );

     /* we need some intermediate storage */
     /* 50 = arbitrary */
     regionstorage = alloca(sizeof(DFBLinkRegion) * 50);
     dfb_linkregionpool_init( &regionpool, regionstorage, 50 );

     const int numberOfWindows  = fusion_vector_size( &sawman->layout );
     DirectLink *updatesBlend[numberOfWindows];
     DirectLink *updatesNoBlend[numberOfWindows];

     /* TODO: if we have a background picture,
      * there is a very strong case for optimizing using double blit. */

     blackBackground = (stack->bg.mode == DLBM_COLOR) && !stack->bg.color.a && !stack->bg.color.r &&  !stack->bg.color.g && !stack->bg.color.b;

     /* z-order: bottom to top */
     fusion_vector_foreach (sawwin, winNum, sawman->layout) {
          CoreWindow *window;
          DFBUpdates *visible;

          D_MAGIC_ASSERT( sawwin, SaWManWindow );

          window = sawwin->window;
          D_MAGIC_COREWINDOW_ASSERT( window );

          visible = right_eye ? &sawwin->right.visible : &sawwin->left.visible;

          D_DEBUG_AT( SaWMan_Update, "  -> visible  %4d,%4d-%4dx%4d\n", DFB_RECTANGLE_VALS_FROM_REGION( &visible->bounding ) );

          updatesBlend[winNum]   = 0;
          updatesNoBlend[winNum] = 0;

          if (SAWMAN_VISIBLE_WINDOW( window ) && (tier->classes & (1 << window->config.stacking))) {
               DFBRectangle bounds = window->config.bounds;

               bounds.x += right_eye ? -window->config.z : window->config.z;

               D_DEBUG_AT( SaWMan_Update, " -=> [%d] <=- %d\n", winNum, SAWMAN_TRANSLUCENT_WINDOW(window) );

               /* check against bounding boxes to quickly discard non-overlapping windows */
               if (!dfb_rectangle_region_intersects(&bounds, &updateRegion) )
                    continue;
                                                    
               /* Optimizing. Only possible when the opacity is max and
                  there is no color keying because this requires additional blending */
               if (      (window->config.opacity == 0xff)
                     &&   window->surface
                     &&  (window->surface->config.caps & DSCAPS_PREMULTIPLIED)
                     &&  (window->config.options & DWOP_ALPHACHANNEL)
                     && !(window->config.options & DWOP_COLORKEYING)
                     &&  (window->config.dst_geometry.mode == DWGM_DEFAULT)
                     &&   blackBackground )
               {
                    DirectLink *updates    = 0;

                    D_DEBUG_AT( SaWMan_Update, " ---> window optimized\n" );

                    /* copy all applicable updates in a separate structure */
                    for (u=0; u < visible->num_regions; u++) {

                         /* clip the visible regions to the update region */
                         if (!dfb_region_intersects( &visible->regions[u], x1, y1, x2, y2 ))
                              continue;

                         lr = dfb_linkregionpool_get( &regionpool, &visible->regions[u] );
                         dfb_region_clip( &lr->region, x1, y1, x2, y2 );
                         direct_list_append( &updates, &lr->link );
                    }

                    DFBLinkRegion *linkRegion = 0;
                    direct_list_foreach(linkRegion, updates) {

                         /* optimizing!
                            check for intersections between to-be-drawn lower windows
                            and this window.
                              If intersection with blend: -> blend.
                              If intersection with noBlend: -> blend, double source
                              Else: draw opaque (add to noBlend).
                         */
                         
                         for (u=winNum-1; u>=0; u--) {
                              direct_list_foreach(lr, updatesBlend[u]) {
                                   DFBRegion *R = &linkRegion->region;
                                   r = &lr->region;
                                   if (dfb_region_region_intersects( R, r )) {
                                        /* overlap with other window! */

                                        /* re-add remaing sections to reconsider */
                                        dfb_linkregionpool_add_allowedpartofregion( &regionpool, &updates, R, r );

                                        /* add intersection to updatesBlend[winNum] */
                                        dfb_region_clip( R, r->x1, r->y1, r->x2, r->y2 );
                                        DFBLinkRegion *lnk = dfb_linkregionpool_get( &regionpool, R );
                                        direct_list_append( &updatesBlend[winNum], &lnk->link );
                                        
                                        goto continueupdates;
                                   }
                              }
                              direct_list_foreach(lr, updatesNoBlend[u]) {
                                   DFBRegion *R = &linkRegion->region;
                                   r = &lr->region;
                                   if (dfb_region_region_intersects( R, r )) {
                                        /* overlap with other window! */
                                        /* intersection, blend double source */

                                        /* reorganise overlapped window;
                                         * we need to cut out the intersection,
                                         * and change the original entry to new ones */
                                        dfb_linkregionpool_add_allowedpartofregion( &regionpool, &updatesNoBlend[u], r, R );
                                        direct_list_remove( &updatesNoBlend[u], &lr->link );

                                        /* re-add remaing sections to reconsider */
                                        dfb_linkregionpool_add_allowedpartofregion( &regionpool, &updates, R, r );

                                        /* proceed to draw immediately 
                                         * we can store the window in another list, but this is more efficient */
                                        dfb_region_clip( R, r->x1, r->y1, r->x2, r->y2 );
                                        SaWManWindow *sw = fusion_vector_at( &sawman->layout, u );
                                        D_DEBUG_AT( SaWMan_Update, "     > window %d and %d\n", u, winNum );
                                        D_DEBUG_AT( SaWMan_Update, "     > nb %4d,%4d-%4dx%4d\n", DFB_RECTANGLE_VALS_FROM_REGION(R) );
                                        sawman_draw_two_windows( tier, sw, sawwin, state, R, right_eye );

                                        goto continueupdates;
                                   }
                              }
                         }

                         /* if we came here, it is a non-overlapping dull window */
                         DFBLinkRegion *lnk;
                         
                         lnk = dfb_linkregionpool_get( &regionpool, &linkRegion->region );
                         direct_list_append( &updatesNoBlend[winNum], &lnk->link );
                         lnk = dfb_linkregionpool_get( &regionpool, &linkRegion->region );
                         direct_list_append( &backgroundNotNeeded, &lnk->link );

                         continueupdates:
                              ;
                    }
               }
               else {
                    int translucent = SAWMAN_TRANSLUCENT_WINDOW(window);

                    D_DEBUG_AT( SaWMan_Update, " ---> default %s window\n", 
                                   translucent ? "blended" : "opaque" );

                    /* store the updates of this window inside the update region */
                    for (u=0; u < visible->num_regions; u++) {
                         if (dfb_region_intersects( &visible->regions[u], x1, y1, x2, y2 )) {
                              /* make a new region */
                              lr = dfb_linkregionpool_get( &regionpool, &visible->regions[u] );
                              dfb_region_clip( &lr->region, x1, y1, x2, y2 );
                              if (translucent)
                                   direct_list_append( &updatesBlend[winNum], &lr->link );
                              else {
                                   /* ignore background */
                                   direct_list_append( &updatesNoBlend[winNum], &lr->link );
                                   DFBLinkRegion *lrbg = dfb_linkregionpool_get( &regionpool, &lr->region );
                                   direct_list_append( &backgroundNotNeeded, &lrbg->link );
                              }
                         }
                    }
               }
          }
     }

     /* draw background */

    {
     /* inversion, but only inside updatable area */
     /* we follow a simple algorithm:
      * start top-left, determine the smallest band height
      * walk from left to right and add non-occupied regions,
      * while checking if we have regions above to fold into
      */
     int x,y,w,h;
     x=x1; y=y1; w=0;
     while (y<=y2) {
          h=y2-y+1; /* maximum */

          /* determine the height of the current band */
          direct_list_foreach(lr, backgroundNotNeeded) {
               r = &lr->region;
               if ( (r->y1 > y) && (r->y1 - y < h) )
                    h = r->y1 - y;
               if ( (r->y1 <= y) && (r->y2 >= y) && (r->y2 - y + 1 < h) )
                    h = r->y2 - y + 1; /* if a band is ended because of this, we could optimize and "hide" the region afterwards */
          }

          /* just "walk the band", looking at the x coordinates, and add updates */
          while (x<=x2) {
               w=x2-x+1; /* maximum */

               walk_the_band:

               direct_list_foreach(lr, backgroundNotNeeded) {
                    r = &lr->region;
                    if ( (r->y1 <= y) && (r->y2 >= y) ) {
                         /* valid window; this window is part of the band */
                         if ( (r->x1 <= x) && (r->x2 >= x) ) {
                              /* IN a band; adjust x and w and go back to start */
                              x = r->x2+1;
                              w = x2-x+1;
                              if (w<0) w=0;

                              goto walk_the_band;
                         }
                         if (r->x2 < x) /* out of reach */
                              continue;
                         if ( (r->x1 - x) < w) { /* this window is blocking the band */
                              w = r->x1 - x;
                         }
                    }
               }

               if (w && h) {
                    DFBRegion u         = { x, y, x+w-1, y+h-1 };
                    bool      collapsed = false;

                    /* we can optimize by checking if above us is a band with the correct width */
                    direct_list_foreach(lr, backgroundNeeded) {
                         r = &lr->region;
                         if ( (r->x1 == u.x1) && (r->x2 == u.x2) && (r->y2 + 1 == u.y1) ) {
                              r->y2 = u.y2;
                              collapsed = true;
                              break;
                         }
                    }

                    if (!collapsed) {
                         lr = dfb_linkregionpool_get( &regionpool, &u );
                         direct_list_append( &backgroundNeeded, &lr->link );
                    }
               }

               x += w;
          }

          y += h;
          x  = x1;
     }
    }

     D_DEBUG_AT( SaWMan_Update, "     > background\n" );

     direct_list_foreach(lr, backgroundNeeded) {
          r = &lr->region;
          D_DEBUG_AT( SaWMan_Update, "     > %4d,%4d-%4dx%4d\n", DFB_RECTANGLE_VALS_FROM_REGION(r) );
          sawman_draw_background( tier, state, r );
     }

     /* draw all the windows */
     fusion_vector_foreach (sawwin, winNum, sawman->layout) {
          D_DEBUG_AT( SaWMan_Update, "     > window %d\n", winNum );
          
          /* collate the updates to reduce number of draw calls */
          /* (is this needed?) */
          /* dfb_collate( &updatesNoBlend[winNum] ); */
          /* dfb_collate( &updatesBlend[winNum] ); */

          direct_list_foreach(lr, updatesNoBlend[winNum]) {
               r = &lr->region;
               D_DEBUG_AT( SaWMan_Update, "     > nb %4d,%4d-%4dx%4d\n", DFB_RECTANGLE_VALS_FROM_REGION(r) );
               sawman_draw_window( tier, sawwin, state, r, false, right_eye );
          }

          direct_list_foreach(lr, updatesBlend[winNum]) {
               r = &lr->region;
               D_DEBUG_AT( SaWMan_Update, "     > b %4d,%4d-%4dx%4d\n", DFB_RECTANGLE_VALS_FROM_REGION(r) );
               sawman_draw_window( tier, sawwin, state, r, (winNum > 0), right_eye );
          }
     }

     dfb_linkregionpool_delete( &regionpool );

     D_DEBUG_AT( SaWMan_Update, " -=> done <=-\n" );
}

void
sawman_flush_updating( SaWMan     *sawman,
                       SaWManTier *tier )
{
     int                 i;
     DFBSurfaceStereoEye old_eye;
     int                 left_num_regions  = 0;
     int                 right_num_regions = 0;

     D_DEBUG_AT( SaWMan_Surface, "%s( %p, %p )\n", __FUNCTION__, sawman, tier );

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );

     old_eye = dfb_surface_get_stereo_eye( tier->region->surface );



     D_ASSUME( tier->left.updating.num_regions > 0 || tier->right.updating.num_regions > 0 );
     D_ASSUME( tier->left.updated.num_regions == 0 && tier->right.updated.num_regions == 0 );

     if (tier->left.updating.num_regions) {
          /* 
           * save here as it might get reset in case surface reaction
           * is called synchronously during dfb_layer_region_flip_update()
           */
          left_num_regions = tier->left.updating.num_regions;

          D_DEBUG_AT( SaWMan_Surface, "  -> making updated = updating\n" );

          /* Make updated = updating */
          direct_memcpy( &tier->left.updated, &tier->left.updating, sizeof(DFBUpdates) );
          direct_memcpy( &tier->left.updated_regions[0], &tier->left.updating_regions[0], sizeof(DFBRegion) * tier->left.updating.num_regions );
          tier->left.updated.regions = &tier->left.updated_regions[0];

          D_DEBUG_AT( SaWMan_Surface, "  -> clearing updating\n" );

          /* Clear updating */
          dfb_updates_reset( &tier->left.updating );
     }

     if (tier->right.updating.num_regions) {
          /* 
           * save here as it might get reset in case surface reaction
           * is called synchronously during dfb_layer_region_flip_update()
           */
          right_num_regions = tier->right.updating.num_regions;

          D_DEBUG_AT( SaWMan_Surface, "  -> making updated = updating\n" );

          /* Make updated = updating */
          direct_memcpy( &tier->right.updated, &tier->right.updating, sizeof(DFBUpdates) );
          direct_memcpy( &tier->right.updated_regions[0], &tier->right.updating_regions[0], sizeof(DFBRegion) * tier->right.updating.num_regions );
          tier->right.updated.regions = &tier->right.updated_regions[0];

          D_DEBUG_AT( SaWMan_Surface, "  -> clearing updating\n" );

          /* Clear updating */
          dfb_updates_reset( &tier->right.updating );
     }


     D_DEBUG_AT( SaWMan_Surface, "  -> flipping the region\n" );

     /* Flip the whole layer. */
     if (tier->region->config.options & DLOP_STEREO)
          dfb_layer_region_flip_update_stereo( tier->region, NULL, NULL, DSFLIP_ONSYNC );
     else
          dfb_layer_region_flip_update( tier->region, NULL, DSFLIP_ONSYNC );


     if (left_num_regions) {
          D_DEBUG_AT( SaWMan_Surface, "  -> copying %d updated regions (F->B)\n", left_num_regions );

          for (i=0; i<left_num_regions; i++) {
               D_DEBUG_AT( SaWMan_Surface, "    -> %4d,%4d - %4dx%4d  [%d]\n",
                           DFB_RECTANGLE_VALS_FROM_REGION( &tier->left.updated.regions[i] ), i );
          }

          /* Copy back the updated region .*/
          dfb_surface_set_stereo_eye( tier->region->surface, DSSE_LEFT );

          if(0) //(mt_get_dbg_info(2)&0x1)
          {
                direct_log_printf(NULL,"%s[pid=%d][%d] \n",__FUNCTION__,__LINE__,getpid());
          }
          

          dfb_gfx_copy_regions( tier->surface, CSBR_FRONT, tier->surface, CSBR_BACK,
                                tier->left.updated.regions, left_num_regions, 0, 0 );
     }

     if (right_num_regions) {
          D_DEBUG_AT( SaWMan_Surface, "  -> copying %d updated regions (F->B)\n", right_num_regions );

          for (i=0; i<right_num_regions; i++) {
               D_DEBUG_AT( SaWMan_Surface, "    -> %4d,%4d - %4dx%4d  [%d]\n",
                           DFB_RECTANGLE_VALS_FROM_REGION( &tier->right.updated.regions[i] ), i );
          }

          /* Copy back the updated region .*/
          dfb_surface_set_stereo_eye( tier->region->surface, DSSE_RIGHT );
   
          dfb_gfx_copy_regions( tier->surface, CSBR_FRONT, tier->surface, CSBR_BACK,
                                tier->right.updated.regions, right_num_regions, 0, 0 );
     }


     dfb_surface_set_stereo_eye( tier->region->surface, old_eye );
}

static void
repaint_tier( SaWMan              *sawman,
              SaWManTier          *tier,
              const DFBRegion     *updates,
              int                  num_updates,
              DFBSurfaceFlipFlags  flags,
              bool                 right_eye )
{
     int                  i;
     CoreLayer           *layer;
     CoreLayerRegion     *region;
     CardState           *state;
     CoreSurface         *surface;
     DFBRegion            cursor_inter;
     CoreWindowStack     *stack;
     DFBSurfaceStereoEye  old_eye, eye;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     D_ASSERT( updates != NULL );
//     D_ASSERT( num_updates > 0 );
     FUSION_SKIRMISH_ASSERT( sawman->lock );

     if (num_updates == 0) 
          return;

     stack = tier->stack;
     D_ASSERT( stack != NULL );

     region = tier->region;
     D_ASSERT( region != NULL );

     layer   = dfb_layer_at( tier->layer_id );
     state   = &layer->state;
     surface = region->surface;

     if (/*!data->active ||*/ !surface)
          return;

     D_DEBUG_AT( SaWMan_Update, "%s( %p, %p )\n", __FUNCTION__, sawman, tier );

     /* Set stereo eye */
     old_eye = dfb_surface_get_stereo_eye( surface );
     eye = right_eye ? DSSE_RIGHT : DSSE_LEFT;
     dfb_surface_set_stereo_eye( surface, eye );

     for (i=0; i<num_updates; i++) {
          const DFBRegion *update = &updates[i];

          DFB_REGION_ASSERT( update );

          D_DEBUG_AT( SaWMan_Update, "  -> %d, %d - %dx%d  (%d)\n",
                      DFB_RECTANGLE_VALS_FROM_REGION( update ), i );

          /* Set destination. */
          state->destination  = surface;
          state->modified    |= SMF_DESTINATION;

          if (!DFB_PLANAR_PIXELFORMAT(region->config.format))
               dfb_state_set_dst_colorkey( state, dfb_color_to_pixel( region->config.format,
                                                                      region->config.src_key.r,
                                                                      region->config.src_key.g,
                                                                      region->config.src_key.b ) );
          else
               dfb_state_set_dst_colorkey( state, 0 );

          /* Set clipping region. */
          dfb_state_set_clip( state, update );

          /* Compose updated region. */
          update_region3( sawman, tier, state,
                          fusion_vector_size( &sawman->layout ) - 1,
                          update->x1, update->y1, update->x2, update->y2, 
                          right_eye );

          /* Update cursor? */
          cursor_inter = tier->cursor_region;
          if (tier->cursor_drawn && dfb_region_region_intersect( &cursor_inter, update )) {
               int          x, y;
               DFBRectangle rect = DFB_RECTANGLE_INIT_FROM_REGION( &cursor_inter );

               D_ASSUME( tier->cursor_bs_valid );

               dfb_gfx_copy_to( surface, tier->cursor_bs, &rect,
                                rect.x - tier->cursor_region.x1,
                                rect.y - tier->cursor_region.y1, true );
#if 0
               x = (s64) stack->cursor.x * (s64) tier->size.w / (s64) sawman->resolution.w;
               y = (s64) stack->cursor.y * (s64) tier->size.h / (s64) sawman->resolution.h;
#else
               x = (s64) stack->cursor.x;
               y = (s64) stack->cursor.y;

#endif
               sawman_draw_cursor( stack, state, surface, &cursor_inter, x, y );
          }
     }

     /* Reset destination. */
     state->destination  = NULL;
     state->modified    |= SMF_DESTINATION;

     /* Restore stereo eye */
     dfb_surface_set_stereo_eye( surface, old_eye );
}

#ifdef CC_HW_WINDOW_SUPPORT

static SaWManWindow *
get_top_window( SaWMan     *sawman,
                   SaWManTier *tier)
{
     int           n;
     SaWManWindow *sawwin;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     FUSION_SKIRMISH_ASSERT( sawman->lock );

     fusion_vector_foreach_reverse (sawwin, n, sawman->layout) {
          CoreWindow *window;

          D_MAGIC_ASSERT( sawwin, SaWManWindow );

          window = sawwin->window;
          D_MAGIC_COREWINDOW_ASSERT( window );

          if (SAWMAN_VISIBLE_WINDOW(window) && (tier->classes & (1 << window->config.stacking))) {
               if ( ( window->caps & (DWCAPS_INPUTONLY | DWCAPS_COLOR) ) 
                    || ( window->config.options & DWOP_INPUTONLY )
                    || (sawwin->window == NULL))
                    continue;

               return sawwin;
          }
     }

     return NULL;
}

#endif
static SaWManWindow *
get_single_window( SaWMan     *sawman,
                   SaWManTier *tier,
                   bool       *ret_none )
{
     int           n;
     SaWManWindow *sawwin;
     SaWManWindow *single = NULL;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     FUSION_SKIRMISH_ASSERT( sawman->lock );

     fusion_vector_foreach_reverse (sawwin, n, sawman->layout) {
          CoreWindow *window;

          D_MAGIC_ASSERT( sawwin, SaWManWindow );

          window = sawwin->window;
          D_MAGIC_COREWINDOW_ASSERT( window );

          if (SAWMAN_VISIBLE_WINDOW(window) && (tier->classes & (1 << window->config.stacking))) {
               if (      single 
                    || ( window->caps & (DWCAPS_INPUTONLY | DWCAPS_COLOR) ) 
                    || ( window->config.options & DWOP_INPUTONLY ) )
                    return NULL;

               single = sawwin;

               if (single->dst.x == 0 &&
                   single->dst.y == 0 &&
                   single->dst.w == tier->size.w &&
                   single->dst.h == tier->size.h &&
                   single->dst.w == single->src.w &&
                   single->dst.h == single->src.h &&
                   !SAWMAN_TRANSLUCENT_WINDOW(window))
                    break;
          }
     }

     if (ret_none && !single)
          *ret_none = true;

     return single;
}

static bool
get_border_only( SaWMan     *sawman,
                 SaWManTier *tier )
{
     int           n;
     SaWManWindow *sawwin;
     bool          none = true;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     FUSION_SKIRMISH_ASSERT( sawman->lock );

     fusion_vector_foreach_reverse (sawwin, n, sawman->layout) {
          CoreWindow *window;

          D_MAGIC_ASSERT( sawwin, SaWManWindow );

          window = sawwin->window;
          D_MAGIC_COREWINDOW_ASSERT( window );

          none = false;

          if (     SAWMAN_VISIBLE_WINDOW(window) 
              && !(window->caps & DWCAPS_INPUTONLY)
              && !(window->config.options & DWOP_INPUTONLY) )
               return false;
     }

     return !none;
}

static bool
windows_updating( SaWMan     *sawman,
                  SaWManTier *tier )
{
     int           i;
     SaWManWindow *window;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );

     fusion_vector_foreach (window, i, sawman->layout) {
          D_MAGIC_ASSERT( window, SaWManWindow );
          
          if (window->flags & SWMWF_UPDATING) {
               long long diff = direct_clock_get_millis() - window->update_ms;

               if (!sawman_config->flip_once_timeout || diff < sawman_config->flip_once_timeout) {
                    D_DEBUG_AT( SaWMan_FlipOnce, "  -> update blocking on window id %u (%lld ms, flags 0x%08x)\n",
                                window->id, diff, window->flags );

                    return true;
               }

               D_DEBUG_AT( SaWMan_FlipOnce, "  -> ignoring blocking of window id %u (%lld ms, flags 0x%08x)\n",
                           window->id, diff, window->flags );
          }
     }

     D_DEBUG_AT( SaWMan_FlipOnce, "  -> update not blocked by any window\n" );

     return false;
}


DirectResult
sawman_sync_buffer(SaWMan        *sawman,SaWManTier    *tier,DFBRegion*       regions)
{
     CoreLayerRegion  *primary;
     CoreSurface      *surface;

     
     D_MAGIC_ASSERT( sawman, SaWMan );
     // FUSION_SKIRMISH_ASSERT( sawman->lock );

     /* Get the primary region. */
     primary = tier->region;
     D_ASSERT( primary != NULL );

     switch (primary->config.buffermode) {
        case DLBM_BACKVIDEO:
             surface = primary->surface;
             D_ASSERT( surface != NULL );

             DFBRegion       region = {0, 0, surface->config.size.w - 1, surface->config.size.h - 1};

             if(0)//(mt_get_dbg_info(2)&0x10)
             {
                 direct_log_printf(NULL,"%s[pid=%d][%d] \n",__FUNCTION__,__LINE__,getpid());
             }
             
             if(regions)
             { 
                dfb_gfx_copy_regions( surface, CSBR_FRONT, surface, CSBR_BACK, regions, 1, 0, 0 );
             }
             else
             {
                dfb_gfx_copy_regions( surface, CSBR_FRONT, surface, CSBR_BACK, &region, 1, 0, 0 );
             }
             
             break;
        default:
             break;
     }

     return DR_OK;
}



/* FIXME: Split up in smaller functions and clean up things like forcing reconfiguration. */
DirectResult
sawman_process_updates( SaWMan              *sawman,
                        DFBSurfaceFlipFlags  flags )
{
     DirectResult  ret;
     int           i;
     int           idx = -1;
     SaWManTier   *tier;

     D_MAGIC_ASSERT( sawman, SaWMan );
     FUSION_SKIRMISH_ASSERT( sawman->lock );

     D_DEBUG_AT( SaWMan_Update, "%s( %p, 0x%08x )\n", __FUNCTION__, sawman, flags );

     direct_list_foreach (tier, sawman->tiers) {
          int              n, d;
          int              total;
          int              bounding;
          bool             none = false;
          bool             border_only;
          SaWManWindow    *single;
          CoreLayer       *layer;
          CoreLayerShared *shared;
          int              screen_width;
          int              screen_height;
          DFBColorKey      single_key;
          const DFBRegion *left_updates  = NULL;
          const DFBRegion *right_updates = NULL;
          unsigned int     left_num  = 0;
          unsigned int     right_num = 0;
          DFBRegion        full_tier_region = { 0, 0, tier->size.w - 1, tier->size.h - 1 };

          idx++;

          layer = dfb_layer_at( tier->layer_id );
          D_ASSERT( layer != NULL );

          shared = layer->shared;
          D_ASSERT( shared != NULL );

          D_MAGIC_ASSERT( tier, SaWManTier );

          if (!tier->left.updates.num_regions && !tier->right.updates.num_regions)
                continue;

          if (tier->update_once) {
               if (windows_updating( sawman, tier ))
                    continue;

               tier->update_once = false;
          }

          D_DEBUG_AT( SaWMan_Update, "  -> %d left_updates, %d right_updates (tier %d, layer %d)\n",
                      tier->left.updates.num_regions, tier->right.updates.num_regions,
                      idx, tier->layer_id );

          D_ASSERT( tier->region != NULL );

          D_DEBUG_AT( SaWMan_Update, "  -> [%d] %d left_updates, bounding %dx%d\n",
                      tier->layer_id, tier->left.updates.num_regions,
                      tier->left.updates.bounding.x2 - tier->left.updates.bounding.x1 + 1,
                      tier->left.updates.bounding.y2 - tier->left.updates.bounding.y1 + 1 );
          D_DEBUG_AT( SaWMan_Update, "  -> [%d] %d right_updates, bounding %dx%d\n",
                      tier->layer_id, tier->right.updates.num_regions,
                      tier->right.updates.bounding.x2 - tier->right.updates.bounding.x1 + 1,
                      tier->right.updates.bounding.y2 - tier->right.updates.bounding.y1 + 1 );

          if (!tier->config.width || !tier->config.height)
               continue;

          dfb_screen_get_screen_size( layer->screen, &screen_width, &screen_height );

          single = get_single_window( sawman, tier, &none );

#ifdef CC_HW_WINDOW_SUPPORT
          {
               CoreWindowStack  *stack = tier->stack;
               CoreLayerContext *context = stack->context;
               CoreLayerRegion *region = NULL;
               if(context->config.buffermode == DLBM_WINDOWS)
               {
                    if(none)  /*no window*/
                    {
                         region = context->primary.region;
                         tier->single_mode   = false;
                         tier->single_window = NULL;

                         if(region)
                         {
                              dfb_layer_region_lock( region );
                              if(region->surface)
                              {
                                   dfb_clear_surface(region->surface);
                              }
                              dfb_layer_region_unlock( region );
                         }                       
                    }
                    else if(single)
                    {
			CoreSurface *surface = single->window->surface;
			D_ASSERT((surface != NULL));
			//D_INFO("[mtk70854][%s: %d][surface->type = 0x%x] \n", __FUNCTION__, __LINE__, surface->type);
			if(surface->type & CSTF_EXTERNAL)
			{
				region = single->window->region;
				if(region == NULL)
				{
					 dfb_window_create_region(single->window);
					 region = single->window->region;
				}
			}
			else
			{
				region = context->primary.region;
				if(region)
				{
					 dfb_layer_region_lock( region );
					 if(region->surface)
					 {
						  dfb_clear_surface(region->surface);
					 }
					 dfb_layer_region_unlock( region );
				} 
			}
       			tier->single_mode	= true;
			tier->single_window = single;
                    }
                    else if(!single)
                    {
                     SaWManWindow  *top_window = get_top_window(sawman, tier);
                         if(top_window)
                         {
			CoreSurface *surface = top_window->window->surface;
			D_ASSERT((surface != NULL));
			//D_INFO("[mtk70854][%s: %d][surface->type = 0x%x] \n", __FUNCTION__, __LINE__, surface->type); 							
			if(surface->type & CSTF_EXTERNAL)
			{
				region = top_window->window->region;
				if(region == NULL)
				{
					 dfb_window_create_region(top_window->window);
					 region = top_window->window->region;
				}
			}
			else
			{
				region = context->primary.region;
				if(region)
				{
					 dfb_layer_region_lock( region );
					 if(region->surface)
					 {
						  dfb_clear_surface(region->surface);
					 }
					 dfb_layer_region_unlock( region );
				}
			}
                         }	
                         tier->single_mode   = false;
                         tier->single_window = NULL;
                    }
                    if(region)
                    {
                         dfb_layer_region_update(region);
                    }

                    dfb_updates_reset( &tier->left.updates );
                    dfb_updates_reset( &tier->right.updates );

                    tier->hw_mode = true;
                    
                    continue;
               }
               else
               {
                    if(tier->hw_mode == true)
                    {
                         tier->single_mode   = false;
                         tier->single_window = NULL;               
                    }
               }
          }
#endif        
          if (none && !sawman_config->show_empty) {
               if (tier->active) {
                    D_DEBUG_AT( SaWMan_Auto, "  -> Disabling region...\n" );

                    tier->active        = false;
                    tier->single_window = NULL;  /* enforce configuration to reallocate buffers */
#ifdef CC_HW_WINDOW_SUPPORT
                    if(tier->region)
                    {
                        dfb_layer_region_lock( tier->region );
                        if(tier->region->surface)
                        {
                             dfb_clear_surface(tier->region->surface);
                        }
                        dfb_layer_region_unlock( tier->region );
                    }

#endif                  

                    dfb_layer_region_disable( tier->region );

                    if (sawman->cursor.context && tier->context->layer_id == sawman->cursor.context->layer_id) {
                         dfb_layer_activate_context( dfb_layer_at(sawman->cursor.context->layer_id), sawman->cursor.context );

                         dfb_layer_region_flip_update( sawman->cursor.region, NULL, DSFLIP_NONE );
                    }
               }
               dfb_updates_reset( &tier->left.updates );
               dfb_updates_reset( &tier->right.updates );
               continue;
          }
          else if(none)
          {
#ifdef CC_HW_WINDOW_SUPPORT
                tier->hw_mode = false;
#endif            
                tier->single_mode   = false;
                tier->single_window = NULL;           
          }

          border_only = get_border_only( sawman, tier );

          /* Remember color key before single mode is activated. */
          if (!tier->single_mode)
               tier->key = tier->context->primary.config.src_key;


          /* If the first mode after turning off the layer is not single, then we need
             this to force a reconfiguration to reallocate the buffers. */
          if (!tier->active) {
               tier->single_mode = true;               /* avoid endless loop */
               tier->border_only = !border_only;       /* enforce configuration to reallocate buffers */
          }

          if (single && !border_only) {
               CoreWindow             *window;
               CoreSurface            *surface;
               DFBDisplayLayerOptions  options = DLOP_NONE;
               DFBRectangle            dst  = single->dst;
               DFBRectangle            src  = single->src;
               DFBRegion               clip = DFB_REGION_INIT_FROM_DIMENSION( &tier->size );

               if (shared->description.caps & DLCAPS_SCREEN_LOCATION) {
                    dst.x = dst.x * screen_width  / tier->size.w;
                    dst.y = dst.y * screen_height / tier->size.h;
                    dst.w = dst.w * screen_width  / tier->size.w;
                    dst.h = dst.h * screen_height / tier->size.h;
               }
               else {
                    if (dst.w != src.w || dst.h != src.h)
                         goto no_single;

                    if (shared->description.caps & DLCAPS_SCREEN_POSITION) {
                         dfb_rectangle_intersect_by_region( &dst, &clip );

                         src.x += dst.x - single->dst.x;
                         src.y += dst.y - single->dst.y;
                         src.w  = dst.w;
                         src.h  = dst.h;

                         dst.x += (screen_width  - tier->size.w) / 2;
                         dst.y += (screen_height - tier->size.h) / 2;
                    }
               }

#ifdef SAWMAN_NO_LAYER_DOWNSCALE
               if (rect.w < src.w)
                    goto no_single;
#endif

#ifdef SAWMAN_NO_LAYER_DST_WINDOW
               if (dst.x != 0 || dst.y != 0 || dst.w != screen_width || dst.h != screen_height)
                    goto no_single;
#endif


               window = single->window;
               D_MAGIC_COREWINDOW_ASSERT( window );

               surface = window->surface;
               D_ASSERT( surface != NULL );

               if (window->config.options & DWOP_ALPHACHANNEL)
                    options |= DLOP_ALPHACHANNEL;

               if (window->caps & DWCAPS_LR_MONO ||
                   window->caps & DWCAPS_STEREO) 
                    options |= DLOP_STEREO;

               if (window->config.options & DWOP_COLORKEYING)
                    options |= DLOP_SRC_COLORKEY;

               single_key = tier->single_key;

               if (DFB_PIXELFORMAT_IS_INDEXED( surface->config.format )) {
                    CorePalette *palette = surface->palette;

                    D_ASSERT( palette != NULL );
                    D_ASSERT( palette->num_entries > 0 );

                    dfb_surface_set_palette( tier->region->surface, surface->palette );

                    if (options & DLOP_SRC_COLORKEY) {
                         int index = window->config.color_key % palette->num_entries;

                         single_key.r     = palette->entries[index].r;
                         single_key.g     = palette->entries[index].g;
                         single_key.b     = palette->entries[index].b;
                         single_key.index = index;
                    }
               }
               else {
                    DFBColor color;

                    dfb_pixel_to_color( surface->config.format, window->config.color_key, &color );

                    single_key.r     = color.r;
                    single_key.g     = color.g;
                    single_key.b     = color.b;
                    single_key.index = window->config.color_key;
               }

               /* Complete reconfig? */
               if (tier->single_window  != single ||
                   !DFB_RECTANGLE_EQUAL( tier->single_src, src ) ||
                   tier->single_format  != surface->config.format ||
                   tier->single_options != options)
               {
                    DFBDisplayLayerConfig  config;
                    DFBSurfaceStereoEye    window_eye, region_eye;

                    D_DEBUG_AT( SaWMan_Auto, "  -> Switching to %dx%d [%dx%d] %s single mode for %p on %p...\n",
                                single->src.w, single->src.h, src.w, src.h,
                                dfb_pixelformat_name( surface->config.format ), single, tier );

                    config.flags        = DLCONF_WIDTH | DLCONF_HEIGHT | DLCONF_PIXELFORMAT | DLCONF_OPTIONS | 
                                          DLCONF_BUFFERMODE | DLCONF_SURFACE_CAPS;
                    config.width        = src.w;
                    config.height       = src.h;
                    config.pixelformat  = surface->config.format;
                    config.options      = options;
                    config.buffermode   = DLBM_BACKVIDEO;//DLBM_FRONTONLY;
                    config.surface_caps = tier->context->config.surface_caps | (options & DLOP_STEREO ? DSCAPS_STEREO : 0);

                    sawman->callback.layer_reconfig.layer_id = tier->layer_id;
                    sawman->callback.layer_reconfig.single   = (SaWManWindowHandle) single;
                    sawman->callback.layer_reconfig.config   = config;

                    switch (sawman_call( sawman, SWMCID_LAYER_RECONFIG,
                                         &sawman->callback.layer_reconfig, sizeof(sawman->callback.layer_reconfig), true )) {
                         case DFB_OK:
                              config = sawman->callback.layer_reconfig.config;
                         case DFB_NOIMPL: 
                              /* continue, no change demanded */
                              break;

                         default:
                              goto no_single;
                    }

                    if (dfb_layer_context_test_configuration( tier->context, &config, NULL ) != DFB_OK)
                         goto no_single;

                    tier->single_mode     = true;
                    tier->single_window   = single;
                    tier->single_width    = src.w;
                    tier->single_height   = src.h;
                    tier->single_src      = src;
                    tier->single_dst      = dst;
                    tier->single_format   = surface->config.format;
                    tier->single_options  = options;
                    tier->single_key      = single_key;

                    tier->active          = false;
                    tier->region->state  |= CLRSF_FROZEN;

                    dfb_updates_reset( &tier->left.updates );
                    dfb_updates_reset( &tier->right.updates );

                    dfb_layer_context_set_configuration( tier->context, &config );

                    if (shared->description.caps & DLCAPS_SCREEN_LOCATION)
                         dfb_layer_context_set_screenrectangle( tier->context, &dst );
                    else if (shared->description.caps & DLCAPS_SCREEN_POSITION)
                         dfb_layer_context_set_screenposition( tier->context, dst.x, dst.y );

                    dfb_layer_context_set_src_colorkey( tier->context,
                                                        tier->single_key.r, tier->single_key.g,
                                                        tier->single_key.b, tier->single_key.index );

                    mdfb_check_wait_vsync(layer);

                    // FIXME: put in function, same as below
                    if (tier->single_options & DLOP_STEREO) {
                         window_eye = dfb_surface_get_stereo_eye(surface);
                         region_eye = dfb_surface_get_stereo_eye(tier->region->surface);
                         dfb_surface_set_stereo_eye(surface, DSSE_LEFT);
                         dfb_surface_set_stereo_eye(tier->region->surface, DSSE_LEFT);
                         dfb_gfx_copy_to( surface, tier->region->surface, &src, window->config.z, 0, false );

                         if (window->caps & DWCAPS_STEREO) {
                              dfb_surface_set_stereo_eye(surface, DSSE_RIGHT);
                         }
                         dfb_surface_set_stereo_eye(tier->region->surface, DSSE_RIGHT);
                         dfb_gfx_copy_to( surface, tier->region->surface, &src, -window->config.z, 0, false );

                         dfb_surface_set_stereo_eye(surface, window_eye);
                         dfb_surface_set_stereo_eye(tier->region->surface, region_eye);
                    }
                    else {
                         dfb_gfx_copy_to( surface, tier->region->surface, &src, 0, 0, false );
                    }

                    tier->active = true;

                    if (sawman->cursor.context && tier->context->layer_id == sawman->cursor.context->layer_id)
                         dfb_layer_activate_context( dfb_layer_at(tier->context->layer_id), tier->context );

                    if (tier->single_options & DLOP_STEREO) {
                         dfb_layer_region_flip_update_stereo( tier->region, NULL, NULL, flags );
                    }
                    else {
                         dfb_layer_region_flip_update( tier->region, NULL, flags );
                    }

                    dfb_updates_reset( &tier->left.updates );
                    dfb_updates_reset( &tier->right.updates );

                    if (1) {
                         SaWManTierUpdate update;

                         update.regions[0].x1 = 0;
                         update.regions[0].y1 = 0;
                         update.regions[0].x2 = tier->single_width  - 1;
                         update.regions[0].y2 = tier->single_height - 1;

                         update.num_regions   = 1;
                         update.classes       = tier->classes;

                         fusion_reactor_dispatch_channel( tier->reactor, SAWMAN_TIER_UPDATE, &update, sizeof(update), true, NULL );
                    }
#ifdef CC_HW_WINDOW_SUPPORT
                    tier->hw_mode = false;
#endif
                    continue;
               }

               mdfb_check_wait_vsync(layer);

               /* Update destination window */
               if (!DFB_RECTANGLE_EQUAL( tier->single_dst, dst )) {
                    tier->single_dst = dst;

                    D_DEBUG_AT( SaWMan_Auto, "  -> Changing single destination to %d,%d-%dx%d.\n",
                                DFB_RECTANGLE_VALS(&dst) );

                    dfb_layer_context_set_screenrectangle( tier->context, &dst );
               }
               {
                    if (tier->single_options & DLOP_STEREO) {
                         DFBSurfaceStereoEye    window_eye, region_eye;

                         window_eye = dfb_surface_get_stereo_eye(surface);
                         region_eye = dfb_surface_get_stereo_eye(tier->region->surface);
                         dfb_surface_set_stereo_eye(surface, DSSE_LEFT);
                         dfb_surface_set_stereo_eye(tier->region->surface, DSSE_LEFT);
                         if (window->config.opacity == 0xff || window->config.opacity == 0x0)
                         {
                             dfb_gfx_copy_to( surface, tier->region->surface, &src, window->config.z, 0, false );
                         }
                         else
                         {
                             dfb_gfx_copy_to_ex( surface, tier->region->surface, &src, window->config.z, 0, false, window->config.opacity);
                         }

                         if (window->caps & DWCAPS_STEREO) {
                              dfb_surface_set_stereo_eye(surface, DSSE_RIGHT);
                         }
                         dfb_surface_set_stereo_eye(tier->region->surface, DSSE_RIGHT);
                         if (window->config.opacity == 0xff || window->config.opacity == 0x0)
                         {
                             dfb_gfx_copy_to( surface, tier->region->surface, &src, -window->config.z, 0, false );
                         }
                         else
                         {
                             dfb_gfx_copy_to_ex( surface, tier->region->surface, &src, -window->config.z, 0, false, window->config.opacity);
                         }

                         dfb_surface_set_stereo_eye(surface, window_eye);
                         dfb_surface_set_stereo_eye(tier->region->surface, region_eye);
                    }
                    else {
                        if (window->config.opacity == 0xff || window->config.opacity == 0x0)
                        {
                            dfb_gfx_copy_to( surface, tier->region->surface, &src, 0, 0, false );
                        }
                        else
                        {
                            dfb_gfx_copy_to_ex( surface, tier->region->surface, &src, 0, 0, false, window->config.opacity);
                        }
                    }
               }

               /* Update color key */
               if (!DFB_COLORKEY_EQUAL( single_key, tier->single_key )) {
                    D_DEBUG_AT( SaWMan_Auto, "  -> Changing single color key.\n" );

                    tier->single_key = single_key;

                    dfb_layer_context_set_src_colorkey( tier->context,
                                                        tier->single_key.r, tier->single_key.g,
                                                        tier->single_key.b, tier->single_key.index );
               }

               tier->active = true;

               if (tier->single_options & DLOP_STEREO) {
                    dfb_layer_region_flip_update_stereo( tier->region, NULL, NULL, flags );
               }
               else {
                    dfb_layer_region_flip_update( tier->region, NULL, flags );
               }

               dfb_updates_reset( &tier->left.updates );
               dfb_updates_reset( &tier->right.updates );

#ifdef CC_HW_WINDOW_SUPPORT
               tier->hw_mode = false;
#endif
               fusion_skirmish_notify( sawman->lock );
               continue;
          }

no_single:

#ifdef CC_HW_WINDOW_SUPPORT
          if (tier->single_mode || tier->hw_mode)
#else
          if (tier->single_mode)
#endif          
          {
               D_DEBUG_AT( SaWMan_Auto, "  -> Switching back from single mode...\n" );

               tier->border_only = !border_only;       /* enforce switch */
#ifdef CC_HW_WINDOW_SUPPORT
               tier->hw_mode = false;
#endif             
          }

          /* Switch border/default config? */
          if (tier->border_only != border_only) {
               DFBDisplayLayerConfig *config;

               tier->border_only = border_only;

               if (border_only)
                    config = &tier->border_config;
               else
               {
                    printf("-> using layer context config \n");
                    fflush(stdout);
                    DFBDisplayLayerConfig layer_config;
                    dfb_layer_context_get_configuration(tier->context, &layer_config);
                    D_INFO("\n\n layer config size [%d x %d] \n\n", layer_config.width, layer_config.height);
                    tier->config.width = layer_config.width;
                    tier->config.height = layer_config.height;
                    config = &tier->config;
                    D_INFO("\n\n new config size [%d x %d] \n\n", config->width, config->height);                     
               }

               D_DEBUG_AT( SaWMan_Auto, "  -> Switching to %dx%d %s %s mode.\n", config->width, config->height,
                           dfb_pixelformat_name( config->pixelformat ), border_only ? "border" : "standard" );

               sawman->callback.layer_reconfig.layer_id = tier->layer_id;
               sawman->callback.layer_reconfig.single   = SAWMAN_WINDOW_NONE;
               sawman->callback.layer_reconfig.config   = *config;
               ret = sawman_call( sawman, SWMCID_LAYER_RECONFIG,
                                  &sawman->callback.layer_reconfig, sizeof(sawman->callback.layer_reconfig), true );

               /* on DFB_OK we try to overrule the default configuration */
               if ( !ret && !dfb_layer_context_test_configuration( tier->context, &(sawman->callback.layer_reconfig.config), NULL ) ) {
                    *config = sawman->callback.layer_reconfig.config;
                    D_DEBUG_AT( SaWMan_Auto, "  -> Overruled to %dx%d %s %s mode.\n", config->width, config->height,
                         dfb_pixelformat_name( config->pixelformat ), border_only ? "border" : "standard" );
               }

               tier->active         = false;
               tier->region->state |= CLRSF_FROZEN;

               dfb_updates_reset( &tier->left.updates );
               dfb_updates_reset( &tier->right.updates );

               /* Temporarily to avoid configuration errors. */
               dfb_layer_context_set_screenposition( tier->context, 0, 0 );

               ret = dfb_layer_context_set_configuration( tier->context, config );
               if (ret) {
                    D_DERROR( ret, "SaWMan/Auto: Switching to standard mode failed!\n" );
                    /* fixme */
               }

               tier->size.w = config->width;
               tier->size.h = config->height;

               /* Notify application manager about new tier size if previous mode was single. */
               if (tier->single_mode)
                    sawman_call( sawman, SWMCID_STACK_RESIZED, &tier->size, sizeof(tier->size), false );

               if (shared->description.caps & DLCAPS_SCREEN_LOCATION) {
                    DFBRectangle full = { 0, 0, screen_width, screen_height };

                    dfb_layer_context_set_screenrectangle( tier->context, &full );
               }
               else if (shared->description.caps & DLCAPS_SCREEN_POSITION) {
                    dfb_layer_context_set_screenposition( tier->context,
                                                          (screen_width  - config->width)  / 2,
                                                          (screen_height - config->height) / 2 );
               }

               if (config->options & DLOP_SRC_COLORKEY) {
                    if (DFB_PIXELFORMAT_IS_INDEXED( config->pixelformat )) {
                         int          index;
                         CoreSurface *surface;
                         CorePalette *palette;

                         surface = tier->region->surface;
                         D_MAGIC_ASSERT( surface, CoreSurface );

                         palette = surface->palette;
                         D_ASSERT( palette != NULL );
                         D_ASSERT( palette->num_entries > 0 );

                         index = tier->key.index % palette->num_entries;

                         dfb_layer_context_set_src_colorkey( tier->context,
                                                             palette->entries[index].r,
                                                             palette->entries[index].g,
                                                             palette->entries[index].b,
                                                             index );
                    }
                    else
                         dfb_layer_context_set_src_colorkey( tier->context,
                                                             tier->key.r, tier->key.g, tier->key.b, tier->key.index );
               }
          }

          if (!tier->active) {
               D_DEBUG_AT( SaWMan_Auto, "  -> Activating tier...\n" );

               tier->active = true;

               DFBRegion region = { 0, 0, tier->size.w - 1, tier->size.h - 1 };
               dfb_updates_add( &tier->left.updates, &region );
               if (tier->region->config.options & DLOP_STEREO) 
                    dfb_updates_add( &tier->right.updates, &region );

               if (sawman->cursor.context && tier->context->layer_id == sawman->cursor.context->layer_id)
                    dfb_layer_activate_context( dfb_layer_at(tier->context->layer_id), tier->context );

               sawman_update_visible(sawman);
          }
          else
          {
               if (!(tier->region->config.options & DLOP_STEREO))
               {
                   mdfb_check_wait_vsync(layer);
                   sawman_sync_buffer(sawman, tier,NULL);
               }
          }  

          tier->single_mode   = false;
          tier->single_window = NULL;

          if (!tier->left.updates.num_regions && !tier->right.updates.num_regions)
               continue;


          dfb_updates_stat( &tier->left.updates, &total, &bounding );
          dfb_updates_stat( &tier->right.updates, &n, &d );
          total += n;
          bounding += d;

          n = tier->left.updates.max_regions - tier->left.updates.num_regions + 1;
          n += tier->right.updates.max_regions - tier->right.updates.num_regions + 1;
          d = n + 1;

          /* Try to optimize updates. In buffer swapping modes we can save the copy by updating everything. */
          if ((total > tier->size.w * tier->size.h) ||
              (total > tier->size.w * tier->size.h * 3 / 5 && (tier->context->config.buffermode == DLBM_BACKVIDEO ||
                                                               tier->context->config.buffermode == DLBM_TRIPLE)))
          {
               left_updates = &full_tier_region;
               left_num     = 1;

               if (tier->region->config.options & DLOP_STEREO) {
                    right_updates = &full_tier_region;
                    right_num     = 1;
               }
          }
          else if (tier->left.updates.num_regions + tier->right.updates.num_regions < 2 || total < bounding * n / d) {
               left_updates = tier->left.updates.regions;
               left_num     = tier->left.updates.num_regions;

               if (tier->region->config.options & DLOP_STEREO) {
                    right_updates = tier->right.updates.regions;
                    right_num     = tier->right.updates.num_regions;
               }
          }
          else 
          {
               left_updates = &tier->left.updates.bounding;
               left_num     = 1;

               if (tier->region->config.options & DLOP_STEREO) {
                    right_updates = &tier->right.updates.bounding;
                    right_num     = 1;
               }
          }
          
          if(left_num)
          {
               repaint_tier( sawman, tier, left_updates, left_num, flags, false );
          }

          if(right_num)
          {
               repaint_tier( sawman, tier, right_updates, right_num, flags, true );
          }

#if 0
#ifdef CC_HW_WINDOW_SUPPORT
         bool need_flip = false;
         if(total > tier->size.w * tier->size.h * 1 / 10)
         {
              need_flip = true;
         }
#endif
#endif

          switch (tier->region->config.buffermode) {
               case DLBM_TRIPLE:
                    /* Add the updated region. */
                    for (i=0; i<left_num; i++) {
                         const DFBRegion *update = &left_updates[i];

                         DFB_REGION_ASSERT( update );

                         D_DEBUG_AT( SaWMan_Surface, "  -> adding %d, %d - %dx%d  (%d) to updating (left)\n",
                                     DFB_RECTANGLE_VALS_FROM_REGION( update ), i );

                         dfb_updates_add( &tier->left.updating, update );
                    }

                    for (i=0; i<right_num; i++) {
                         const DFBRegion *update = &right_updates[i];

                         DFB_REGION_ASSERT( update );

                         D_DEBUG_AT( SaWMan_Surface, "  -> adding %d, %d - %dx%d  (%d) to updating (right)\n",
                                     DFB_RECTANGLE_VALS_FROM_REGION( update ), i );

                         dfb_updates_add( &tier->right.updating, update );
                    }

                    if (!tier->left.updated.num_regions && !tier->right.updated.num_regions)
                         sawman_flush_updating( sawman, tier );
                    break;

               case DLBM_BACKVIDEO:
                    if (tier->region->config.options & DLOP_STEREO) {
                         DFBSurfaceStereoEye old_eye = dfb_surface_get_stereo_eye( tier->region->surface );

                         /* Flip the whole region. */
                         dfb_layer_region_flip_update_stereo( tier->region, NULL, NULL, flags );

                         //if(mt_get_dbg_info(2)&0x10000)
                          {
                              mdfb_check_wait_vsync(layer);
                          }
                         
                         /* Copy back the updated region. */
                         if (left_num) {
                              dfb_surface_set_stereo_eye( tier->region->surface, DSSE_LEFT );
                              
                              if(0) //(mt_get_dbg_info(2)&0x1)
                              {
                                  direct_log_printf(NULL,"%s[pid=%d][%d] \n",__FUNCTION__,__LINE__,getpid());
                              }
                              
                              dfb_gfx_copy_regions( tier->region->surface, CSBR_FRONT, tier->region->surface, CSBR_BACK, left_updates, left_num, 0, 0 );
                         }

                         if (right_num) {
                              dfb_surface_set_stereo_eye( tier->region->surface, DSSE_RIGHT );

                              dfb_gfx_copy_regions( tier->region->surface, CSBR_FRONT, tier->region->surface, CSBR_BACK, right_updates, right_num, 0, 0 );
                         }

                         dfb_surface_set_stereo_eye( tier->region->surface, old_eye );
                    }
                    else {
                         /* Flip the whole region. */
#if 0                        
#ifdef CC_HW_WINDOW_SUPPORT
                         if(true == need_flip)
                         {
                              flags &= ~DSFLIP_BLIT;
                              tier->last_flip = true;
                              dfb_layer_region_flip_update( tier->region, NULL, flags );    
                         }
                         else
                         {
                              if(flags & DSFLIP_BLIT)
                              {
                                   tier->last_flip = false;
          
                                   for (i = 0; i < left_num; i++) {
                                        const DFBRegion *update = &left_updates[i];

                                        DFB_REGION_ASSERT( update );

                                        /* Flip the updated region .*/
                                        dfb_layer_region_flip_update( tier->region, update, flags );
                                   }          
                              }
                              else
                              {
                                   tier->last_flip = true;
                                   dfb_layer_region_flip_update(  tier->region, NULL, flags );   
                              }
                         }

#else
                         flags &= ~DSFLIP_BLIT;
                         dfb_layer_region_flip_update( tier->region, NULL, flags );
#endif
#else
                         /* Flip the whole region. */
                         dfb_layer_region_flip_update( tier->region, NULL, flags );

                         if(!(flags & DSFLIP_BLIT))
                         {
                              mdfb_wait_for_sync();
                              mdfb_check_wait_vsync(layer);

                              /* Copy back the updated region. */

                              if(0) //(mt_get_dbg_info(2)&0x1)
                              {
                                  direct_log_printf(NULL,"%s[pid=%d][%d] \n",__FUNCTION__,__LINE__,getpid());
                              }
                                           
                              dfb_gfx_copy_regions( tier->region->surface, CSBR_FRONT, tier->region->surface, CSBR_BACK, left_updates, left_num, 0, 0 );
                         }
#endif
                         /* Copy back the updated region. */
                         //dfb_gfx_copy_regions( tier->region->surface, CSBR_FRONT, tier->region->surface, CSBR_BACK, left_updates, left_num, 0, 0 );
                    }
                    break;

               default:
                    /* Flip the updated region .*/
                    for (i=0; i<left_num; i++) {
                         const DFBRegion *update = &left_updates[i];

                         DFB_REGION_ASSERT( update );

                         dfb_layer_region_flip_update( tier->region, update, flags );
                    }

                    for (i=0; i<right_num; i++) {
                         const DFBRegion *update = &right_updates[i];

                         DFB_REGION_ASSERT( update );

                         dfb_layer_region_flip_update( tier->region, update, flags );
                    }
                    break;
          }

#ifdef SAWMAN_DUMP_TIER_FRAMES
          {
               DFBResult          ret;
               CoreSurfaceBuffer *buffer;

               D_MAGIC_ASSERT( tier->region->surface, CoreSurface );

               if (fusion_skirmish_prevail( &tier->region->surface->lock ) == DFB_OK) {
                    if (tier->region->config.options & DLOP_STEREO) {
                         DFBSurfaceStereoEye eye;
     
                         eye = dfb_surface_get_stereo_eye( tier->region->surface );
     
                         dfb_surface_set_stereo_eye( tier->region->surface, DSSE_LEFT );
                         buffer = dfb_surface_get_buffer( tier->region->surface, CSBR_FRONT );
                         D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
                         ret = dfb_surface_buffer_dump( buffer, ".", "tier_left" );
     
                         dfb_surface_set_stereo_eye( tier->region->surface, DSSE_RIGHT );
                         buffer = dfb_surface_get_buffer( tier->region->surface, CSBR_FRONT );
                         D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
                         ret = dfb_surface_buffer_dump( buffer, ".", "tier_right" );
     
                         dfb_surface_set_stereo_eye( tier->region->surface, eye );
                    }
                    else {
                         buffer = dfb_surface_get_buffer( tier->region->surface, CSBR_FRONT );
                         D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

                         ret = dfb_surface_buffer_dump( buffer, ".", "tier" );
                    }

                    fusion_skirmish_dismiss( &tier->region->surface->lock );
               }
          }
#endif

          if (1) {
               SaWManTierUpdate update;

               direct_memcpy( &update.regions[0], left_updates, sizeof(DFBRegion) * left_num );

               update.num_regions = left_num;
               update.classes     = tier->classes;

               fusion_reactor_dispatch_channel( tier->reactor, SAWMAN_TIER_UPDATE, &update, sizeof(update), true, NULL );
          }








          dfb_updates_reset( &tier->left.updates );
          dfb_updates_reset( &tier->right.updates );

          fusion_skirmish_notify( sawman->lock );
     }

     return DFB_OK;
}

