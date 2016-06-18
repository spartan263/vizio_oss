/*
   (c) Copyright 2000-2002  convergence integrated media GmbH.
   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de> and
              Sven Neumann <neo@directfb.org>.

   This file is subject to the terms and conditions of the MIT License:

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge,
   publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <directfb.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define FBIO_MW_OP  0x5000

#define MW_GL_OP_SET_PLANE              ((unsigned long)0x2)

typedef struct mw_gl_op_t_
{
    unsigned long u4Op;
    union 
    {
        struct 
        {
            unsigned    e_mode;
            void*       pv_bits;
            unsigned    ui4_pitch;
            int         i4_x;
            int         i4_y;
            unsigned    ui4_width;
            unsigned    ui4_height;
            unsigned    ui4_color;
        }fill_rect;
        struct
        {
            int         i4_idx;
            int         i4_enable;
        }set_plane;
        struct
        {
            unsigned int         ui4_size;
            void*         pv_bits;
        }get_fbm_bitmap_info;           
        struct
        {
            unsigned int    ui4_id;
            unsigned int    ui4_enable;
            unsigned int    ui4_width;
            unsigned int    ui4_height;
        } enable_scaler;
    }u;
}mw_gl_op_t;

/******************************************************************************/

static IDirectFB             *dfb     = NULL;
static IDirectFBDisplayLayer *layer   = NULL;
static IDirectFBWindow       *window  = NULL;
static IDirectFBSurface      *surface = NULL;
static IDirectFBEventBuffer  *events  = NULL;

static IDirectFBSurface      *layer_surface = NULL;

static IDirectFBDisplayLayer *vdp_layer   = NULL;
static IDirectFBSurface      *vdp_surface = NULL;
static IDirectFBSurface      *jpg_surface0 = NULL;
static IDirectFBSurface      *jpg_surface1 = NULL;

static int _width = 960;
static int _height = 540;
static int _window_double_buffer = 0;
static int _layer_double_buffer = 0;
static int _mode = 0;
static int _sleep = 1000;
static int _event_enable = 0;
static int _alpha_enable = 0;
static int _surface_resize_enable = 0;
static int _layer = 0;
	
DFBDimension _win_dim[7] =
{
    {320, 240},
    {480, 320},
    {640, 480},
    {960, 540},
    {1280, 720},
    {1440, 900},    
    {1920, 1080}
};

static int _win_dim_idx = 3;

DFBPoint _win_loc = {0, 0};

/******************************************************************************/

static void init_application( int *argc, char **argv[] );
static void exit_application( int status );

static void update();

/******************************************************************************/

int
main( int argc, char *argv[] )
{
    DFBResult ret;
    int i = 0;
  
     /* Initialize application. */
     init_application( &argc, &argv );

     /* Main loop. */
     while (1) {
          DFBWindowEvent event;
          update();

          sleep(1);

         /* Release the layer. */
         if(i++ > 10)
         {
             printf("Disable VDP layer for 10 sec!\n");                                   
             if (vdp_layer)
             {
                  vdp_layer->Release( vdp_layer);
             }             
             if (vdp_surface)
             {
                  vdp_surface->ReleaseSource( vdp_surface);         
                  vdp_surface->Release( vdp_surface);
             }
             sleep(10);
             /* Get the vdp display layer. */
             ret = dfb->GetDisplayLayer( dfb, 2, &vdp_layer );
             if (ret) {
                  DirectFBError( "IDirectFB::GetDisplayLayer() vdp failed", ret );
                  exit_application( 3 );
             }
             ret = vdp_layer->SetCooperativeLevel( vdp_layer, DLSCL_ADMINISTRATIVE );
             if (ret)
                  DirectFBError( "Failed to get exclusive access vdp", ret );
                          /* Get the layer's surface. */
             ret = vdp_layer->GetSurface( vdp_layer, &vdp_surface );
             if (ret)
                  DirectFBError( "Failed to get vdp layer surface", ret );
             
             i = 0;             
             
         }
                              
     }
     
     /* Shouldn't reach this. */
     return 0;
}

/******************************************************************************/

static void
init_application( int *argc, char **argv[] )
{
     DFBResult             ret;
     DFBWindowDescription  desc;
     DFBDisplayLayerConfig config;
     DFBWindowOptions opt;
     
     /* Initialize DirectFB including command line parsing. */
     ret = DirectFBInit( argc, argv );
     if (ret) {
          DirectFBError( "DirectFBInit() failed", ret );
          exit_application( 1 );
     }

     /* Create the super interface. */
     ret = DirectFBCreate( &dfb );
     if (ret) {
          DirectFBError( "DirectFBCreate() failed", ret );
          exit_application( 2 );
     }

     /* Get the vdp display layer. */
     ret = dfb->GetDisplayLayer( dfb, 2, &vdp_layer );
     if (ret) {
          DirectFBError( "IDirectFB::GetDisplayLayer() vdp failed", ret );
          exit_application( 3 );
     }
     ret = vdp_layer->SetCooperativeLevel( vdp_layer, DLSCL_ADMINISTRATIVE );
     if (ret)
          DirectFBError( "Failed to get exclusive access vdp", ret );
          /* Get the layer's surface. */
     ret = vdp_layer->GetSurface( vdp_layer, &vdp_surface );
     if (ret)
          DirectFBError( "Failed to get vdp layer surface", ret );

     ret = vdp_surface->Flip( vdp_surface, NULL, 0 );
     if (ret)
          DirectFBError( "Failed to flip vdp layer surface", ret );         


    {
         FILE *fp;
         void* ptr;
         int pitch;
         DFBSurfaceDescription  surf_desc;
         surf_desc.width = 1920;
         surf_desc.height = 1080;
         surf_desc.pixelformat = DSPF_NV16;
         surf_desc.caps = DSCAPS_VIDEOONLY | DSCAPS_FROM_JPEG_VDP;
         surf_desc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS;
         ret = dfb->CreateSurface(dfb,
                        &surf_desc,
                         &jpg_surface0);
         if (ret)
              DirectFBError( "Failed to create jpg surface 0", ret );         
         
         fp = fopen("JPG422_Y_1920x1088_0.raw" , "rb");
         if(fp == NULL)
             DirectFBError( "Failed to open Y data!", ret );                         	
         jpg_surface0->Lock(jpg_surface0,
                         DSLF_WRITE,
                       &ptr, &pitch );
         if (ret)
             DirectFBError( "Failed to lock jpg surface 0", ret );                
         if(fread(ptr, 1, 1920 * 1080, fp) != 1920 * 1080)
         {
             DirectFBError( "Failed to read Y data!", ret );                         	         
         }
         fclose(fp);
         
         fp = fopen("JPG422_C_1920x1088_0.raw" , "rb");
         if(fp == NULL)
             DirectFBError( "Failed to open C data!", ret );                         	
         
         if(fread((void*)((int)ptr + 1920 * 1080), 1, 1920 * 1080, fp) != 1920 * 1080)
         {
             DirectFBError( "Failed to read C data!", ret );                         	                  
         }
         fclose(fp);         
         jpg_surface0->Unlock(jpg_surface0);    


         // Load 2nd .jpg file
         ret = dfb->CreateSurface(dfb,
                        &surf_desc,
                         &jpg_surface1);
         if (ret)
              DirectFBError( "Failed to create jpg surface 1", ret );         


         fp = fopen("JPG422_Y_1920x1088_1.raw" , "rb");
         if(fp == NULL)
             DirectFBError( "Failed to open Y data!", ret );                         	
         jpg_surface1->Lock(jpg_surface1,
                         DSLF_WRITE,
                       &ptr, &pitch );
         if (ret)
             DirectFBError( "Failed to lock jpg surface 1", ret );                
         if(fread(ptr, 1, 1920 * 1080, fp) != 1920 * 1080)
         {
             DirectFBError( "Failed to read Y data!", ret );                         	         
         }
         fclose(fp);
         
         fp = fopen("JPG422_C_1920x1088_1.raw" , "rb");
         if(fp == NULL)
             DirectFBError( "Failed to open C data!", ret );                         	            
         if(fread((void*)((int)ptr + 1920 * 1080), 1, 1920 * 1080, fp) != 1920 * 1080)
         {
             DirectFBError( "Failed to read C data!", ret );                         	                  
         }
         fclose(fp);            
         jpg_surface1->Unlock(jpg_surface1); 
         
    }
     
}

static void
exit_application( int status )
{
     /* Release the window's surface. */
     if (jpg_surface0)
          jpg_surface0->Release( jpg_surface0 );
     if (jpg_surface1)
          jpg_surface1->Release( jpg_surface1 );
     
     /* Release the layer. */
     if (vdp_layer)
          vdp_layer->Release( vdp_layer );

     /* Release the super interface. */
     if (dfb)
          dfb->Release( dfb );

     /* Terminate application. */
     exit( status );
}

/******************************************************************************/
static void
update()
{
     static int flip = 0;
     static int argb = 0;
     DFBResult             ret;
     
     if(flip)
     {
         DFBRectangle rect = {100, 200, 800, 640};
         vdp_surface->SetBlittingFlags(vdp_surface,
                                   DSBLIT_ROTATE90);         
         vdp_surface->Blit(vdp_surface,
                       jpg_surface1,
                       &rect,
                       0, 0);    
     }
     else
     {
         DFBRectangle rect = {200, 100, 640, 800};
         vdp_surface->SetBlittingFlags(vdp_surface,
                                   DSBLIT_ROTATE270);         
         vdp_surface->Blit(vdp_surface,
                       jpg_surface1,
                       &rect,
                       0, 0);                
     }
     vdp_surface->Flip( vdp_surface, 0, 0);

     flip = !flip;
     
}

