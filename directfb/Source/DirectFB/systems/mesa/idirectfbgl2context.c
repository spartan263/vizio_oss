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

#include <stdarg.h>

#include <directfbgl2.h>

#include <direct/debug.h>

#include <core/surface.h>
#include <core/system.h>

#include <display/idirectfbsurface.h>

#include "mesa_system.h"


D_DEBUG_DOMAIN( IDFBGL_Mesa, "IDirectFBGL2Context/Mesa", "IDirectFBGL2Context Mesa Implementation" );

static DirectResult
Probe( void *ctx, ... );

static DirectResult
Construct( void *interface, ... );

#include <direct/interface_implementation.h>

DIRECT_INTERFACE_IMPLEMENTATION( IDirectFBGL2Context, Mesa )

/*
 * private data struct of IDirectFBGL2Context
 */
typedef struct {
     int                      ref;       /* reference counter */

     CoreDFB                 *core;
     MesaData                *mesa;

     EGLDisplay               eglDisplay;
     EGLContext               eglContext;

     GLuint                   fbo;
     GLuint                   depth;


     bool                     locked;

     CoreSurface             *surface;
     CoreSurfaceBufferLock    lock;
} IDirectFBGL2Context_data;


static void
IDirectFBGL2Context_Destruct( IDirectFBGL2Context *thiz )
{
     IDirectFBGL2Context_data *data = thiz->priv;

     if (data->locked)
          dfb_surface_unlock_buffer( data->surface, &data->lock );

     DIRECT_DEALLOCATE_INTERFACE( thiz );
}

static DirectResult
IDirectFBGL2Context_AddRef( IDirectFBGL2Context *thiz )
{
     DIRECT_INTERFACE_GET_DATA (IDirectFBGL2Context);

     data->ref++;

     return DFB_OK;
}

static DirectResult
IDirectFBGL2Context_Release( IDirectFBGL2Context *thiz )
{
     DIRECT_INTERFACE_GET_DATA (IDirectFBGL2Context)

     if (--data->ref == 0)
          IDirectFBGL2Context_Destruct( thiz );

     return DFB_OK;
}

static DFBResult
IDirectFBGL2Context_Bind( IDirectFBGL2Context *thiz,
                          IDirectFBSurface    *draw,
                          IDirectFBSurface    *read )
{
     DFBResult              ret;
     IDirectFBSurface_data *draw_data;
     CoreSurface           *surface;

     DIRECT_INTERFACE_GET_DATA (IDirectFBGL2Context);

     D_DEBUG_AT( IDFBGL_Mesa, "%s()\n", __FUNCTION__ );

     if (!draw || !read)
          return DFB_INVARG;

     if (draw != read)
          return DFB_UNSUPPORTED;

     if (data->locked)
          return DFB_LOCKED;

     /*
      * Get destination and source core surfaces
      */
     DIRECT_INTERFACE_GET_DATA_FROM( draw, draw_data, IDirectFBSurface );

     surface = draw_data->surface;
     if (!surface)
          return DFB_DESTROYED;

     data->surface = surface;

     eglMakeCurrent( data->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, data->eglContext );

     /* Lock destination buffer */
     ret = dfb_surface_lock_buffer( surface, CSBR_BACK, CSAID_ACCEL1, CSAF_READ | CSAF_WRITE, &data->lock );
     if (ret) {
          D_DERROR( ret, "IDirectFBGL2Context/Mesa: Could not lock destination buffer!\n" );
          return ret;
     }

     /* Update depth buffer */
     glBindRenderbuffer( GL_RENDERBUFFER, data->depth );
     glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, surface->config.size.w, surface->config.size.h );

     /* Set color and depth buffers */
     glBindFramebuffer( GL_FRAMEBUFFER, data->fbo );

     glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, data->depth );

     glFramebufferRenderbuffer( GL_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0,
                                GL_RENDERBUFFER,
                                (GLuint) (long) data->lock.handle );

     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
          D_ERROR( "DirectFB/GLES2: Framebuffer not complete\n" );
     }

     data->locked = true;

     return DFB_OK;
}

static DFBResult
IDirectFBGL2Context_Unbind( IDirectFBGL2Context *thiz )
{
     DIRECT_INTERFACE_GET_DATA (IDirectFBGL2Context);

     D_DEBUG_AT( IDFBGL_Mesa, "%s()\n", __FUNCTION__ );

     if (!data->locked)
          return DFB_BUFFEREMPTY;

     glFinish();

     dfb_surface_unlock_buffer( data->surface, &data->lock );

     eglMakeCurrent( data->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );

     data->locked = false;

     return DFB_OK;
}

static inline bool TestEGLError(const char* pszLocation)
{
     EGLint iErr = eglGetError();
     if (iErr != EGL_SUCCESS) {
          D_ERROR("DirectFB/Mesa: %s failed (%d).\n", pszLocation, iErr);
          return false;
     }

     return true;
}

/* exported symbols */

static DirectResult
Probe( void *ctx, ... )
{
     D_DEBUG_AT( IDFBGL_Mesa, "%s()\n", __FUNCTION__ );

     if (dfb_system_type() != CORE_MESA)
          return DFB_UNSUPPORTED;

     return DFB_OK;
}

static DirectResult
Construct( void *interface, ... )
{
     IDirectFBGL2Context *thiz = interface;
     CoreDFB             *core;
     MesaData            *mesa = dfb_system_data();

     D_DEBUG_AT( IDFBGL_Mesa, "%s()\n", __FUNCTION__ );

     va_list tag;
     va_start(tag, interface);
     core = va_arg(tag, CoreDFB *);
     va_end( tag );

     /* Allocate interface data. */
     DIRECT_ALLOCATE_INTERFACE_DATA( thiz, IDirectFBGL2Context );

     /* Initialize interface data. */
     data->core = core;
     data->ref  = 1;
     data->mesa = mesa;

     /*
      * Setup the OpenGL state
      */
     data->eglDisplay = mesa->dpy;
     data->eglContext = eglCreateContext( mesa->dpy, NULL, mesa->ctx, NULL );
     if (!TestEGLError("eglCreateContext"))
          return DFB_INIT;

     eglMakeCurrent( data->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, data->eglContext );
     if (!TestEGLError("eglMakeCurrent"))
          return DFB_INIT;


     glGenFramebuffers( 1, &data->fbo );

     glGenRenderbuffers( 1, &data->depth );


     /* Assign interface pointers. */
     thiz->AddRef         = IDirectFBGL2Context_AddRef;
     thiz->Release        = IDirectFBGL2Context_Release;
     thiz->Bind           = IDirectFBGL2Context_Bind;
     thiz->Unbind         = IDirectFBGL2Context_Unbind;

     return DFB_OK;
}

