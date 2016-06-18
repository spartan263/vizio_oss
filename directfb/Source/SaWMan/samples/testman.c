/*
   (c) Copyright 2006-2007  directfb.org

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>.

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

#include <config.h>


#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <directfb.h>
#include <directfb_util.h>

#include <direct/clock.h>
#include <direct/messages.h>

#include <sawman.h>

#define MAX_WINDOWS 4
#define MAX_LAYOUTS 4

#define CHECK(x)                                  \
     do {                                         \
          DFBResult ret = (x);                    \
          if (ret != DFB_OK) {                    \
               DirectFBError(#x,ret);             \
               goto out;                          \
          }                                       \
     } while (0)


typedef struct __TestMan_TestManager    TestManager;
typedef struct __TestMan_Layout         Layout;
typedef struct __TestMan_Application    Application;

/**********************************************************************************************************************/

struct __TestMan_TestManager {
     int                magic;

     IDirectFB         *dfb;
     ISaWMan           *saw;
     ISaWManManager    *manager;

     SaWManScalingMode  scaling_mode;

     SaWManWindowHandle   windows[MAX_WINDOWS];
     int                  num_windows;
     SaWManWindowHandle   focus_window;

     const Layout      *layouts[MAX_LAYOUTS];
     int                num_layouts;
     int                current_layout;

     DirectLink        *applications;
};

struct __TestMan_Layout {
     void *data;

     void (*Relayout)    ( TestManager  *tm,
                           void         *layout_data );

     void (*AddWindow)   ( TestManager        *tm,
                           void               *layout_data,
                           SaWManWindowHandle  window );

     void (*RemoveWindow)( TestManager        *tm,
                           void               *layout_data,
                           SaWManWindowHandle  window,
                           int                 index );
};

struct __TestMan_Application {
     DirectLink         link;

     int                magic;

     const char        *name;
     const char        *program;
     const char        *args;

     bool               started;

     long long          start_time;
     pid_t              pid;
     SaWManProcess     *process;
};

/**********************************************************************************************************************/

static void
AddApplication( TestManager *tm,
                const char  *name,
                const char  *program,
                const char  *args )
{
     Application *app;

     D_MAGIC_ASSERT( tm, TestManager );
     D_ASSERT( name != NULL );
     D_ASSERT( program != NULL );

     app = D_CALLOC( 1, sizeof(Application) );
     if (!app) {
          D_OOM();
          return;
     }

     app->name    = name;
     app->program = program;
     app->args    = args;

     direct_list_append( &tm->applications, &app->link );

     D_MAGIC_SET( app, Application );
}

static Application *
LookupApplication( TestManager *tm,
                   const char  *name )
{
     Application *app;

     D_MAGIC_ASSERT( tm, TestManager );
     D_ASSERT( name != NULL );

     direct_list_foreach (app, tm->applications) {
          D_MAGIC_ASSERT( app, Application );

          if (!strcmp( app->name, name ))
               return app;
     }

     return NULL;
}

static Application *
LookupApplicationByPID( TestManager *tm,
                        pid_t        pid )
{
     Application *app;

     D_MAGIC_ASSERT( tm, TestManager );

     direct_list_foreach (app, tm->applications) {
          D_MAGIC_ASSERT( app, Application );

          if (app->pid == pid)
               return app;
     }

     return NULL;
}

/**********************************************************************************************************************/

static void
MosaicRelayout( TestManager *tm,
                void        *layout_data )
{
     int             i;
     int             hcenter;
     int             vcenter;
     ISaWManManager *manager;
     DFBRectangle    bounds[4];
     DFBDimension    size;

     D_MAGIC_ASSERT( tm, TestManager );

     if (!tm->num_windows)
          return;

     manager = tm->manager;
     D_ASSERT( manager != NULL );

     manager->Lock( manager );

     manager->GetSize( manager, DWSC_MIDDLE, &size );

     hcenter = (size.w / 2) & ~1;
     vcenter = size.h / 2;

     switch (tm->num_windows) {
          case 0:
          case 1:
               bounds[0].x = 0;
               bounds[0].y = 0;
               bounds[0].w = size.w;
               bounds[0].h = size.h;

               break;

          case 2:
               bounds[0].x = 0;
               bounds[0].y = 0;
               bounds[0].w = hcenter;
               bounds[0].h = size.h;

               bounds[1].x = hcenter;
               bounds[1].y = 0;
               bounds[1].w = size.w - hcenter;
               bounds[1].h = size.h;

               break;

          case 3:
               bounds[0].x = 0;
               bounds[0].y = 0;
               bounds[0].w = hcenter;
               bounds[0].h = vcenter;

               bounds[1].x = 0;
               bounds[1].y = vcenter;
               bounds[1].w = hcenter;
               bounds[1].h = size.h - vcenter;

               bounds[2].x = hcenter;
               bounds[2].y = 0;
               bounds[2].w = size.w - hcenter;
               bounds[2].h = size.h;

               break;

          case 4:
               bounds[0].x = 0;
               bounds[0].y = 0;
               bounds[0].w = hcenter;
               bounds[0].h = vcenter;

               bounds[1].x = 0;
               bounds[1].y = vcenter;
               bounds[1].w = hcenter;
               bounds[1].h = size.h - vcenter;

               bounds[2].x = hcenter;
               bounds[2].y = 0;
               bounds[2].w = size.w - hcenter;
               bounds[2].h = vcenter;

               bounds[3].x = hcenter;
               bounds[3].y = vcenter;
               bounds[3].w = size.w - hcenter;
               bounds[3].h = size.h - vcenter;

               break;

          default:
               D_BUG( "invalid number of windows (%d)", tm->num_windows );
               break;
     }

     for (i=0; i<tm->num_windows; i++) {
          SaWManWindowHandle        window = tm->windows[i];
          SaWManWindowConfig        config;
          SaWManWindowConfigFlags   flags;
          
          flags = SWMCF_POSITION | SWMCF_SIZE;
          config.bounds = bounds[i];
          manager->SetWindowConfig( manager, window, flags, &config );
     }

     manager->QueueUpdate( manager, DWSC_MIDDLE, NULL );

     manager->Unlock( manager );
}

static void
MosaicAddWindow( TestManager        *tm,
                 void               *layout_data,
                 SaWManWindowHandle  window )
{
     ISaWManManager *manager;

     D_MAGIC_ASSERT( tm, TestManager );

     manager = tm->manager;
     D_ASSERT( manager != NULL );

     manager->Lock( manager );

     tm->windows[tm->num_windows++] = window;

     manager->InsertWindow( manager, window, SAWMAN_WINDOW_NONE, SWMWR_TOP );

     MosaicRelayout( tm, layout_data );

     manager->Unlock( manager );
}

static void
MosaicRemoveWindow( TestManager        *tm,
                    void               *layout_data,
                    SaWManWindowHandle  window,
                    int                 index )
{
     ISaWManManager *manager;

     D_MAGIC_ASSERT( tm, TestManager );

     manager = tm->manager;
     D_ASSERT( manager != NULL );

     manager->Lock( manager );

     /* Remove window from layout. */
     manager->RemoveWindow( manager, window );

     MosaicRelayout( tm, layout_data );

     manager->Unlock( manager );
}

static const Layout mosaic_layout = {
     data:          NULL,
     Relayout:      MosaicRelayout,
     AddWindow:     MosaicAddWindow,
     RemoveWindow:  MosaicRemoveWindow
};

/**********************************************************************************************************************/

static DFBResult
LayoutWindowAdd( TestManager        *tm,
                 SaWManWindowHandle  window )
{
     const Layout *layout;

     D_MAGIC_ASSERT( tm, TestManager );

     D_ASSERT( tm->current_layout >= 0 );
     D_ASSERT( tm->current_layout < tm->num_layouts );

     layout = tm->layouts[tm->current_layout];

     D_ASSERT( layout != NULL );
     D_ASSERT( layout->AddWindow != NULL );

     if (tm->num_windows == MAX_WINDOWS) {
          D_WARN( "maximum number (%d) of managed windows exceeded", MAX_WINDOWS );
          return DFB_LIMITEXCEEDED;
     }

     /* Set some default borders. */
     //window->border_normal     = 2;
     //window->border_fullscreen = 4;

     /* Call the layout implementation. */
     layout->AddWindow( tm, layout->data, window );

     return DFB_OK;
}

static DFBResult
LayoutWindowRemove( TestManager        *tm,
                    SaWManWindowHandle  window )
{
     int           i;
     const Layout *layout;

     D_MAGIC_ASSERT( tm, TestManager );

     D_ASSERT( tm->current_layout >= 0 );
     D_ASSERT( tm->current_layout < tm->num_layouts );

     layout = tm->layouts[tm->current_layout];

     D_ASSERT( layout != NULL );
     D_ASSERT( layout->RemoveWindow != NULL );

     for (i=0; i<tm->num_windows; i++) {
          if (tm->windows[i] == window)
               break;
     }

     if (i == MAX_WINDOWS) {
          D_BUG( "could not find window %lx", window );
          return DFB_BUG;
     }

     /* Remove window from our own list of managed windows. */
     for (; i<tm->num_windows-1; i++)
          tm->windows[i] = tm->windows[i+1];

     tm->windows[i] = SAWMAN_WINDOW_NONE;

     tm->num_windows--;

     /* Call the layout implementation. */
     layout->RemoveWindow( tm, layout->data, window, i );

     return DFB_OK;
}

/**********************************************************************************************************************/

static DirectResult
start_request( void       *context,
               const char *name,
               pid_t      *ret_pid )
{
     TestManager *tm = context;
     Application *app;
     pid_t        pid;
     const char  *args[3];

     D_INFO( "SaWMan/TestMan: Start request for '%s'!\n", name );

     D_MAGIC_ASSERT( tm, TestManager );
     D_ASSERT( name != NULL );
     D_ASSERT( ret_pid != NULL );

     app = LookupApplication( tm, name );
     if (!app)
          return DFB_ITEMNOTFOUND;

     if (app->started && !waitpid( app->pid, NULL, WNOHANG )) {
          D_DEBUG( "Already running '%s' (%d)!", name, app->pid );
          return DFB_BUSY;
     }

     app->started = true;

     app->start_time = direct_clock_get_millis();

     pid = vfork();

     switch (pid) {
          case -1:
               perror("vfork");
               return DFB_FAILURE;

          case 0:
               setsid();

               args[0] = app->program;
               args[1] = app->args;
               args[2] = NULL;

               execvp( app->program, (char**) args );
               perror("execvp");
               _exit(0);

          default:
               app->pid = pid;
               break;
     }

     *ret_pid = pid;

     return DFB_OK;
}

static DirectResult
stop_request( void     *context,
              pid_t     pid,
              FusionID  caller )
{
     TestManager *tm = context;
     Application *app;

     D_INFO( "SaWMan/TestMan: Stop request from Fusion ID 0x%lx for pid %d!\n", caller, pid );

     D_MAGIC_ASSERT( tm, TestManager );

     app = LookupApplicationByPID( tm, pid );
     if (!app)
          return DFB_ITEMNOTFOUND;

     /* Already died before attaching? */
     if (waitpid( app->pid, NULL, WNOHANG )) {
          app->started = false;
          app->pid     = 0;

          return DFB_OK;
     }

     /* Not attached yet? */
     if (!app->process) {
          D_ERROR( "Application with pid %d did not attach yet!\n", app->pid );
          return DFB_NOCONTEXT;
     }

     /* FIXME: avoid signals */
     kill( app->pid, 9 );

     return DFB_OK;
}

static DirectResult
process_added( void          *context,
               SaWManProcess *process )
{
     TestManager *tm = context;
     Application *app;

     D_INFO( "SaWMan/TestMan: Process added (%d) [%lu]!\n", process->pid, process->fusion_id );

     D_MAGIC_ASSERT( tm, TestManager );

     app = LookupApplicationByPID( tm, process->pid );
     if (!app)
          return DFB_ITEMNOTFOUND;

     if (app->process) {
          D_BUG( "Already attached '%s' (%d)!", app->name, app->pid );
          return DFB_BUG;
     }

     app->process = process;

     return DFB_OK;
}

static DirectResult
process_removed( void          *context,
                 SaWManProcess *process )
{
     TestManager *tm = context;
     Application *app;

     D_INFO( "SaWMan/TestMan: Process removed (%d) [%lu]!\n", process->pid, process->fusion_id );

     D_MAGIC_ASSERT( tm, TestManager );

     app = LookupApplicationByPID( tm, process->pid );
     if (!app)
          return DFB_ITEMNOTFOUND;

     if (app->process != process) {
          D_BUG( "Process mismatch %p != %p of '%s' (%d)!", app->process, process, app->name, app->pid );
          return DFB_BUG;
     }

     if (waitpid( app->pid, NULL, 0 ) < 0)
          perror("waitpid");

     app->process = NULL;
     app->started = false;
     app->pid     = 0;

     return DFB_OK;
}

static DirectResult
input_filter( void          *context,
              DFBInputEvent *event )
{
     int             i;
     TestManager    *tm = context;
     ISaWManManager *manager;

//     D_INFO( "SaWMan/TestMan: Input filter (%x)!\n", event->type );

     D_MAGIC_ASSERT( tm, TestManager );

     manager = tm->manager;

     D_ASSERT( manager != NULL );

     manager->Lock( manager );

     switch (event->type) {
          case DIET_KEYPRESS:
               switch (event->key_symbol) {
                    case DIKS_F9:
                         if (tm->num_windows > 1) {
                              for (i=0; i<tm->num_windows; i++) {
                                   SaWManWindowHandle window = tm->windows[i];

                                   if (window == tm->focus_window) {
                                        window = tm->windows[(i+1) % tm->num_windows];

                                        manager->SwitchFocus( manager, window );

                                        break;
                                   }
                              }
                         }
                         manager->Unlock( manager );
                         return DFB_BUSY;

                    case DIKS_F10:
                         if (tm->num_layouts > 1) {
                              const Layout *layout;

                              if (++tm->current_layout == tm->num_layouts)
                                   tm->current_layout = 0;

                              layout = tm->layouts[tm->current_layout];

                              D_ASSERT( layout != NULL );
                              D_ASSERT( layout->Relayout != NULL );

                              layout->Relayout( tm, layout->data );
                         }
                         manager->Unlock( manager );
                         return DFB_BUSY;

                    case DIKS_F11:
                         tm->scaling_mode = (tm->scaling_mode == SWMSM_SMOOTH_SW) ? SWMSM_STANDARD : SWMSM_SMOOTH_SW;
                         manager->SetScalingMode( manager, tm->scaling_mode );
                         manager->Unlock( manager );
                         return DFB_BUSY;

                    default:
                         break;
               }

          case DIET_KEYRELEASE:
               switch (event->key_symbol) {
                    case DIKS_F9:
                    case DIKS_F10:
                    case DIKS_F11:
                         manager->Unlock( manager );
                         return DFB_BUSY;

                    default:
                         break;
               }

          default:
               break;
     }

     manager->Unlock( manager );
     return DFB_OK;
}


static DirectResult
window_preconfig( void               *context,
                  SaWManWindowConfig *config )
{
     D_INFO( "SaWMan/TestMan: Window preconfig (%d,%d-%dx%d)!\n",
             DFB_RECTANGLE_VALS( &config->bounds ) );

     return DFB_OK;
}

static DirectResult
window_added( void               *context,
              SaWManWindowInfo   *info )
{
     DFBResult    ret;
     TestManager *tm = context;

     D_MAGIC_ASSERT( tm, TestManager );

     D_INFO( "SaWMan/TestMan: Window added (%d,%d-%dx%d)!\n",
             DFB_RECTANGLE_VALS( &info->config.bounds ) );

     if (info->caps & DWCAPS_NODECORATION)
          return DFB_NOIMPL;  /* to let sawman insert the window */

     /* Already showing window? (reattaching) */
     if (info->config.opacity) {
          /* Activate scaling. */
          info->config.options |= DWOP_SCALE;

          ret = LayoutWindowAdd( tm, info->handle );
          if (ret)
               return ret;
     }

     return DFB_OK;
}

static DirectResult
window_removed( void               *context,
                SaWManWindowInfo   *info )
{
     D_INFO( "SaWMan/TestMan: Window removed (%d,%d-%dx%d)!\n",
             DFB_RECTANGLE_VALS( &info->config.bounds ) );

     return DFB_OK;
}

static DirectResult
window_reconfig( void                 *context,
                 SaWManWindowReconfig *reconfig )
{
     DFBResult           ret;
     TestManager        *tm = context;
     SaWManWindowConfig *current;
     SaWManWindowConfig *request;

     D_MAGIC_ASSERT( tm, TestManager );

     if (reconfig->caps & DWCAPS_NODECORATION)
          return DFB_OK;

     current = &reconfig->current;
     request = &reconfig->request;

     if (reconfig->flags & SWMCF_POSITION) {
          D_INFO( "SaWMan/TestMan: Window config - ignoring position (%d,%d)!\n", request->bounds.x, request->bounds.y );
          reconfig->flags &= ~SWMCF_POSITION;
     }

     if (reconfig->flags & SWMCF_SIZE) {
          D_INFO( "SaWMan/TestMan: Window config - ignoring size (%dx%d)!\n", request->bounds.w, request->bounds.h );
          reconfig->flags &= ~SWMCF_SIZE;
     }

     if (reconfig->flags & SWMCF_STACKING) {
          D_INFO( "SaWMan/TestMan: Window config - ignoring stacking (%d)!\n", request->stacking );
          reconfig->flags &= ~SWMCF_STACKING;
     }

     if (reconfig->flags & SWMCF_OPACITY) {
          D_INFO( "SaWMan/TestMan: Window config - opacity change %d\n", request->opacity );
          /* Show? */
          if (request->opacity && !current->opacity) {
               /* Activate scaling. */
               if ( !(reconfig->flags & SWMCF_OPTIONS) )
                    request->options = 0; /* if a flag is not set, corresponding field may not be initialised */

               reconfig->flags   |= SWMCF_OPTIONS;
               request->options  |= DWOP_SCALE;

               ret = LayoutWindowAdd( tm, reconfig->handle );
               if (ret)
                    return ret;
          }
          /* Hide? */
          else if (!request->opacity && current->opacity) {
               LayoutWindowRemove( tm, reconfig->handle );
          }
     }

     return DFB_OK;
}

static DirectResult
window_restack( void                 *context,
                SaWManWindowHandle    handle,
                SaWManWindowHandle    relative,
                SaWManWindowRelation  relation )
{
     //~ if (window->caps & DWCAPS_NODECORATION)
          //~ return DFB_OK;

     D_INFO( "SaWMan/TestMan: Window restack - refusing!\n" );

     return DFB_ACCESSDENIED;
}

static DirectResult
switch_focus( void                 *context,
              SaWManWindowHandle    window )
{
     TestManager        *tm = context;

     D_MAGIC_ASSERT( tm, TestManager );

     D_INFO( "SaWMan/TestMan: Switching focus to %lx\n", window );

     tm->focus_window = window;

     return DFB_OK;
}

static DirectResult layer_reconfig( void                *context,
                                    SaWManLayerReconfig *reconfig )
{
     TestManager           *tm      = context;
     DFBDisplayLayerConfig *config;
     DFBResult              res;

     D_MAGIC_ASSERT( tm, TestManager );
     D_ASSERT( reconfig );

     config = &reconfig->config;
     D_INFO( "SaWMan/TestMan: Layer Reconfig signalled on layer %d:\n", reconfig->layer_id );
     if (reconfig->single) {
          SaWManWindowInfo info;

          D_INFO( "   going to single mode, window: %p\n", (void*)(reconfig->single) );
          res = tm->manager->GetWindowInfo( tm->manager, reconfig->layer_id, &info );
          if (res)
              D_INFO( "   size: %dx%d\n", info.config.bounds.w, info.config.bounds.h );
     }

     config->width  = 300;
     config->height = 300;

     D_INFO( "   requested layer size: %dx%d\n", config->width, config->height );
     
     return DFB_OK;
}

static DirectResult application_id_changed( void             *context,
                                            SaWManWindowInfo *info )
{
     TestManager        *tm = context;

     D_MAGIC_ASSERT( tm, TestManager );

     D_INFO( "SaWMan/TestMan: Application ID switched to %ld\n", info->application_id );

     return DFB_OK;
}


static const SaWManCallbacks callbacks = {
     Start:              start_request,
     Stop:               stop_request,
     ProcessAdded:       process_added,
     ProcessRemoved:     process_removed,
     InputFilter:        input_filter,
     WindowPreConfig:    window_preconfig,
     WindowAdded:        window_added,
     WindowRemoved:      window_removed,
     WindowReconfig:     window_reconfig,
     WindowRestack:      window_restack,
     //~ StackResized
     SwitchFocus:        switch_focus,
     LayerReconfig:      layer_reconfig,
     ApplicationIDChanged: application_id_changed
};


int
main( int argc, char** argv )
{
     TestManager tm;

     D_INFO( "SaWMan/TestMan: Initializing...\n" );

     memset( &tm, 0, sizeof(tm) );

     tm.layouts[tm.num_layouts++] = &mosaic_layout;

     D_MAGIC_SET( &tm, TestManager );

     AddApplication( &tm, "Penguins", "df_andi", "--dfb:mode=640x480,force-windowed" );
     AddApplication( &tm, "Windows", "df_window", NULL );


     CHECK( DirectFBInit( &argc, &argv ) );

     CHECK( DirectFBCreate( &tm.dfb ) );

     CHECK( SaWManInit( &argc, &argv ) );
     CHECK( SaWManCreate( &tm.saw ) );

     CHECK( tm.saw->CreateManager( tm.saw, &callbacks, &tm, &tm.manager ) );

     pause();


out:
     D_INFO( "SaWMan/TestMan: Shutting down...\n" );

     if (tm.manager)
          tm.manager->Release( tm.manager );

     if (tm.saw)
          tm.saw->Release( tm.saw );

     if (tm.dfb)
          tm.dfb->Release( tm.dfb );

     D_MAGIC_CLEAR( &tm );

     return 0;
}

