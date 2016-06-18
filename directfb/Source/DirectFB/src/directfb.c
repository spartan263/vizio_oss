/*
   (c) Copyright 2001-2009  The world wide DirectFB Open Source Community (directfb.org)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include <direct/conf.h>
#include <direct/direct.h>
#include <direct/interface.h>
#include <direct/log.h>
#include <direct/mem.h>
#include <direct/messages.h>
#include <direct/util.h>

#include <directfb.h>
#include <directfb_version.h>

#include <misc/conf.h>

#if !DIRECTFB_BUILD_PURE_VOODOO
#include <unistd.h>

#include <direct/thread.h>

#include <core/core.h>
#include <core/coredefs.h>
#include <core/coretypes.h>

#include <core/input.h>
#include <core/layer_context.h>
#include <core/layer_control.h>
#include <core/layers.h>
#include <core/state.h>
#include <core/gfxcard.h>
#include <core/surface.h>
#include <core/windows.h>
#include <core/windowstack.h>
#include <core/wm.h>

#include <gfx/convert.h>

#include <display/idirectfbsurface.h>
#endif

#include <idirectfb.h>
#ifdef CC_DFB_DEBUG_SUPPORT
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <pthread.h>
#endif


#ifndef DIRECTFB_VERSION_VENDOR
#define DIRECTFB_VERSION_VENDOR
#endif


static DFBResult CreateRemote( const char *host, int session, IDirectFB **ret_interface );

/*
 * Version checking
 */
const unsigned int directfb_major_version = DIRECTFB_MAJOR_VERSION;
const unsigned int directfb_minor_version = DIRECTFB_MINOR_VERSION;
const unsigned int directfb_micro_version = DIRECTFB_MICRO_VERSION;
const unsigned int directfb_binary_age    = DIRECTFB_BINARY_AGE;
const unsigned int directfb_interface_age = DIRECTFB_INTERFACE_AGE;

const char *
DirectFBCheckVersion( unsigned int required_major,
                      unsigned int required_minor,
                      unsigned int required_micro )
{
     if (required_major > DIRECTFB_MAJOR_VERSION)
          return "DirectFB version too old (major mismatch)";
     if (required_major < DIRECTFB_MAJOR_VERSION)
          return "DirectFB version too new (major mismatch)";
     if (required_minor > DIRECTFB_MINOR_VERSION)
          return "DirectFB version too old (minor mismatch)";
     if (required_minor < DIRECTFB_MINOR_VERSION)
          return "DirectFB version too new (minor mismatch)";
     if (required_micro < DIRECTFB_MICRO_VERSION - DIRECTFB_BINARY_AGE)
          return "DirectFB version too new (micro mismatch)";
     if (required_micro > DIRECTFB_MICRO_VERSION)
          return "DirectFB version too old (micro mismatch)";

     return NULL;
}

const char *
DirectFBUsageString( void )
{
     return dfb_config_usage();
}

#ifdef CC_DFB_DEBUG_SUPPORT

static DFBResult
DirectFBAllocArgs(int argc, char **retArgv)
{
	int i = 0;
	
	for(i = 0; i < argc; i++)
	{
        retArgv[i] = (char *) malloc(60);
		memset(retArgv[i], 0, 60);
	}

	return DR_OK;
}


static DFBResult
DirectFBFreeArgs(int argc, char *argv[])
{
    int i = 0;

	for(i = 0; i < argc; i++)
	{
        free(argv[i]);                 	
	}

	return DR_OK;	
}


static DFBResult
DirectFBParseArgs(const char *strings, int n, char *retArgv[], int *retArgc)
{
    int index = 0;
    char *temp = (char *) strings;
	char *head = temp;
	
	do
	{
	     temp = strchr(head, ' ');

		 if(temp)
		 {
			  *temp = '\0';
		 }
		 
		 strcpy(retArgv[index], head);
		 D_INFO("%s: %s \n", __FUNCTION__, retArgv[index]);
		 
		 index++;
		 if(n < index)
		 {
		      break;
		 }

		 if(temp)
		 {
		      head = temp + 1;
		      while(*head == ' ')
		 	     head++;	
		 }
	}
	while(temp);

	*retArgc = index;
	
	return DR_OK;
}


static void *
DirectFBDebugThread()
{
    int ret;
	int listenfd = -1, recvfd = -1, maxfd = -1;
	int len;
	bool running = true;
	char recv_buf[1020];
	static fd_set fdset;
	struct sockaddr_un srv_addr;
    struct sockaddr_un clt_addr;

	listenfd  = socket(PF_UNIX, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		 D_INFO("[DFB DBG] cannot create communication socket \n");
		 return;
	}
	
	srv_addr.sun_family = AF_UNIX;
	sprintf(srv_addr.sun_path, "/tmp/DFB_%d.dbg", getpid());
	unlink(srv_addr.sun_path);
	
	ret = bind(listenfd, (struct sockaddr *)& srv_addr, sizeof(struct sockaddr));
	if(ret < 0)
	{
		 D_INFO("[DFB DBG] cannot bind communication socket \n");
		 close(listenfd );
		 return;
	}
	
	ret = listen(listenfd, 1);
	if(ret < 0)
	{
		 D_INFO("[DFB DBG] cannot listen communication socket");
		 close(listenfd );
		 return;
	}	

    while(running)
	{
	     FD_ZERO(&fdset);
         FD_SET(listenfd, &fdset);
         ( recvfd >= 0 )? (FD_SET(recvfd, &fdset)): 0;
		 maxfd = (recvfd > listenfd) ? recvfd: listenfd;
		 
		 switch(select(maxfd + 1, &fdset, 0, 0, 0))
		 {
		      case -1:
		      case 0:
		           D_INFO("[DFB DBG] select error \n");
	          break;

			  default:
			       if(FD_ISSET(listenfd, &fdset))
			       {
			            len = sizeof(clt_addr);
						recvfd = accept(listenfd, (struct sockaddr*)&clt_addr,(socklen_t *) &len);  
			       }
				   else if(( recvfd >= 0 ) && ( FD_ISSET(recvfd, &fdset) ))
				   {
				        int num;
						memset(recv_buf, 0, 1020);
						if((num = read(recvfd, recv_buf,sizeof(recv_buf)) > 0))
						{
						     D_INFO("[DFB DBG] recvbuffer [%s] \n", recv_buf);
							 
							 if(strstr(recv_buf, "quit"))
							 {
                                  running = false;
							 }

							 int argc = 20;
							 char *argv[20];

							 DirectFBAllocArgs(argc, argv);

							 DirectFBParseArgs(recv_buf, argc, argv, &argc);

							 dfb_config_cmdlnset(argc, argv);

							 DirectFBFreeArgs(argc, argv);

							 close(recvfd);
							 recvfd = -1;							 
						}						
				   }
			  break;
		 }
		  
	}

	close(listenfd);

	return NULL;
}

#endif

DFBResult
DirectFBInit( int *argc, char *(*argv[]) )
{
     DFBResult ret;
#ifdef CC_DFB_DEBUG_SUPPORT	 
     static bool _init = false;
     pthread_t thread;
#endif
     ret = dfb_config_init( argc, argv );
     if (ret)
          return ret;

#ifdef CC_DFB_DEBUG_SUPPORT

     if(false == _init)
     {
          pthread_create(&thread, NULL, DirectFBDebugThread, NULL);
     }
	 
     _init = true;
	 
#endif

     return DFB_OK;
}

DFBResult
DirectFBSetOption( const char *name, const char *value )
{
     DFBResult ret;

     if (dfb_config == NULL) {
          D_ERROR( "DirectFBSetOption: DirectFBInit has to be "
                   "called before DirectFBSetOption!\n" );
          return DFB_INIT;
     }

     if (!name)
          return DFB_INVARG;

     ret = dfb_config_set( name, value );
     if (ret)
          return ret;

     return DFB_OK;
}

/*
 * Programs have to call this to get the super interface
 * which is needed to access other functions
 */
DFBResult
DirectFBCreate( IDirectFB **interface_ptr )
{
#if !DIRECTFB_BUILD_PURE_VOODOO
     DFBResult  ret;
     IDirectFB *dfb;
     CoreDFB   *core_dfb;
#endif

     if (!dfb_config) {
          /*  don't use D_ERROR() here, it uses dfb_config  */
          direct_log_printf( NULL, "(!) DirectFBCreate: DirectFBInit "
                             "has to be called before DirectFBCreate!\n" );
          return DFB_INIT;
     }

     if (!interface_ptr)
          return DFB_INVARG;


     if (!dfb_config->no_singleton && idirectfb_singleton) {
          idirectfb_singleton->AddRef( idirectfb_singleton );

          *interface_ptr = idirectfb_singleton;

          return DFB_OK;
     }

     direct_initialize();

     if ( !(direct_config->quiet & DMT_BANNER) && dfb_config->banner) {
          direct_log_printf( NULL,
                             "\n"
                             "   ~~~~~~~~~~~~~~~~~~~~~~~~~~| DirectFB " DIRECTFB_VERSION DIRECTFB_VERSION_VENDOR " |~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                             "        (c) 2001-2010  The world wide DirectFB Open Source Community\n"
                             "        (c) 2000-2004  Convergence (integrated media) GmbH\n"
                             "      ----------------------------------------------------------------\n"
                             "\n" );
     }

#if !DIRECTFB_BUILD_PURE_VOODOO
     if (dfb_config->remote.host)
          return CreateRemote( dfb_config->remote.host, dfb_config->remote.port, interface_ptr );

     static DirectMutex lock = DIRECT_MUTEX_INITIALIZER(lock);

     direct_mutex_lock( &lock );


     ret = dfb_core_create( &core_dfb );
     if (ret) {
          direct_mutex_unlock( &lock );
          return ret;
     }

     DIRECT_ALLOCATE_INTERFACE( dfb, IDirectFB );

     ret = IDirectFB_Construct( dfb, core_dfb );
     if (ret) {
          dfb_core_destroy( core_dfb, false );
          direct_mutex_unlock( &lock );
          return ret;
     }

     if (dfb_core_is_master( core_dfb )) {
          if (!dfb_core_active( core_dfb )) {
               ret = IDirectFB_InitLayers( dfb );
               if (ret) {
                    dfb->Release( dfb );
                    direct_mutex_unlock( &lock );
                    return ret;
               }

               /* not fatal */
               ret = dfb_wm_post_init( core_dfb );
               if (ret)
                    D_DERROR( ret, "DirectFBCreate: Post initialization of WM failed!\n" );

               dfb_core_activate( core_dfb );
          }
     }

     direct_mutex_unlock( &lock );

     *interface_ptr = dfb;

     if (!dfb_config->no_singleton)
          idirectfb_singleton = dfb;

     return DFB_OK;
#else
     return CreateRemote( dfb_config->remote.host ? dfb_config->remote.host : "", dfb_config->remote.port, interface_ptr );
#endif
}

DFBResult
DirectFBError( const char *msg, DFBResult error )
{
     if (msg)
          direct_log_printf( NULL, "(#) DirectFBError [%s]: %s\n", msg,
                             DirectFBErrorString( error ) );
     else
          direct_log_printf( NULL, "(#) DirectFBError: %s\n",
                             DirectFBErrorString( error ) );

     return error;
}

const char *
DirectFBErrorString( DFBResult error )
{
     return DirectResultString( error );
}

DFBResult
DirectFBErrorFatal( const char *msg, DFBResult error )
{
     DirectFBError( msg, error );

     //if (idirectfb_singleton)
          //IDirectFB_Destruct( idirectfb_singleton );

     exit( error );
}

/**************************************************************************************************/

static DFBResult
CreateRemote( const char *host, int port, IDirectFB **ret_interface )
{
     DFBResult             ret;
     DirectInterfaceFuncs *funcs;
     void                 *interface_ptr;

     D_ASSERT( host != NULL );
     D_ASSERT( ret_interface != NULL );

     ret = DirectGetInterface( &funcs, "IDirectFB", "Requestor", NULL, NULL );
     if (ret)
          return ret;

     ret = funcs->Allocate( &interface_ptr );
     if (ret)
          return ret;

     ret = funcs->Construct( interface_ptr, host, port );
     if (ret)
          return ret;

     *ret_interface = interface_ptr;

     return DFB_OK;
}

int DirectFBDump(int *video, int *system, int *fbm)
{
     return -1;
}

int DirectFBDumpPrintOnly(void)
{
   return -1;
}
#if 0

/**********************************************************************************************************************/

typedef struct {
     int video;
     int system;
     int presys;
     int from_fbm;
} MemoryUsage;

/**********************************************************************************************************************/

static IDirectFB *dfb = NULL;

static MemoryUsage mem = { 0, 0 };

static bool show_shm;
static bool show_pools;
static bool show_allocs;

/**********************************************************************************************************************/

static inline int
buffer_size( CoreSurface *surface, CoreSurfaceBuffer *buffer, bool video )
{
     int                    i, mem = 0;
     CoreSurfaceAllocation *allocation;

     fusion_vector_foreach (allocation, i, buffer->allocs) {
          if (video) {
               if (allocation->access & (CSAF_GPU_READ | CSAF_GPU_WRITE))
                    mem += allocation->size;
          }
          else if (!(allocation->access & (CSAF_GPU_READ | CSAF_GPU_WRITE)))
               mem += allocation->size;
     }

     return mem;
}

static int
buffer_sizes( CoreSurface *surface, bool video )
{
     int i, mem = 0;

     for (i=0; i<surface->num_buffers; i++) {
          CoreSurfaceBuffer *buffer = surface->buffers[i];

          mem += buffer_size( surface, buffer, video );
     }

     return mem;
}


static bool
surface_callback( FusionObjectPool *pool,
                  FusionObject     *object,
                  void             *ctx )
{
     DirectResult ret;
     int          i;
     int          refs;
     CoreSurface *surface = (CoreSurface*) object;
     MemoryUsage *mem     = ctx;
     int          vmem;
     int          smem;

     if (object->state != FOS_ACTIVE)
          return true;

     ret = fusion_ref_stat( &object->ref, &refs );
     if (ret) {
          printf( "Fusion error %d!\n", ret );
          return false;
     }

     vmem = buffer_sizes( surface, true );
     smem = buffer_sizes( surface, false );

     if (surface->config.caps & DSCAPS_FROM_FBM)
     {
         mem->from_fbm += vmem;
     }
     else
     {
         mem->video += vmem;     
     }
     
      mem->system += smem;

     return true;
}

static void
dump_surfaces()
{
     dfb_core_enum_surfaces( NULL, surface_callback, &mem );
}

/**********************************************************************************************************************/

int DirectFBDump(int *video, int *system, int *fbm)
{
     DFBResult ret;
     long long millis;
     long int  seconds, minutes, hours, days;

     MemoryUsage mem_ret = { 0, 0 };

     memset((void*)&mem, 0, sizeof(MemoryUsage));

     /* Initialize DirectFB. */    
     if(dfb == NULL)
     {     
         ret = DirectFBInit( 0, NULL );
         if (ret) {
              DirectFBError( "DirectFBInit", ret );
              return -1;
         }
    
         /* Create the super interface. */
         ret = DirectFBCreate( &dfb );
         if (ret) {
              DirectFBError( "DirectFBCreate", ret );
              return -3;
         }
     }
     
     dump_surfaces();

     if(video != NULL)
     {
         *video = mem.video;
     }
     if(system != NULL)
     {
         *system = mem.system + mem.presys;
     }
     if(fbm != NULL)
     {
         *fbm = mem.from_fbm;
     }

     /* DirectFB deinitialization. */
     /*
     if (dfb)
          dfb->Release( dfb );
     */
     
     return ret;
}
#endif   

