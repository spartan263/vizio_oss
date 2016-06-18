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

#include <config.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sawman.h>

#include <direct/conf.h>
#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>
#include <direct/util.h>

#include <misc/conf.h>

#include "sawman_config.h"


SaWManConfig *sawman_config = NULL;

static const char *config_usage =
     "SaWMan Configuration\n"
     "\n"
     " --sawman-help                       Output SaWMan usage information and exit\n"
     " --sawman:<option>[,<option>]...     Pass options to SaWMan (see below)\n"
     "\n"
     "SaWMan options:\n"
     "\n"
     "  init-border=<num>                  Set border values for tier (0-2)\n"
     "  border-thickness=<num>             border thickness\n"
     "  border-resolution=<width>x<height> Set border's width and height\n"
     "  border-format=<pixelformat>        Set border's pixelformat\n"
     "  border-focused-color-index<0|1|2|3>=<color>   Set focused border's color\n"
     "  border-unfocused-color-index<0|1|2|3>=<color> Set unfocused border's color\n"
     "  border-focused-color<0|1|2|3>=AARRGGBB   Set focused border's color\n"
     "  border-unfocused-color<0|1|2|3>=AARRGGBB Set unfocused border's color\n"
     "  [no-]show-empty                    Show layer even if no window is visible\n"
     "  flip-once-timeout=<num>            Flip once timeout\n"
     "  hw-cursor=<layer-id>               Set HW Cursor mode\n"
     "  resolution=<width>x<height>        Set virtual SaWMan resolution\n"
     "\n";


static DFBResult
parse_args( const char *args )
{
     char *buf = alloca( strlen(args) + 1 );

     strcpy( buf, args );

     while (buf && buf[0]) {
          DFBResult  ret;
          char      *value;
          char      *next;

          if ((next = strchr( buf, ',' )) != NULL)
               *next++ = '\0';

          if (strcmp (buf, "help") == 0) {
               fprintf( stderr, config_usage );
               exit(1);
          }

          if ((value = strchr( buf, '=' )) != NULL)
               *value++ = '\0';

          ret = sawman_config_set( buf, value );
          switch (ret) {
               case DFB_OK:
                    break;
               case DFB_UNSUPPORTED:
                    D_ERROR( "SaWMan/Config: Unknown option '%s'!\n", buf );
                    break;
               default:
                    return ret;
          }

          buf = next;
     }

     return DFB_OK;
}

static DFBResult
config_allocate()
{
     int i;

     if (sawman_config)
          return DFB_OK;

     sawman_config = D_CALLOC( 1, sizeof(SaWManConfig) );
     if (!sawman_config)
          return D_OOM();

     sawman_config->border = &sawman_config->borders[0];

     sawman_config->borders[0].thickness    = 4;
     sawman_config->borders[0].focused[0]   = (DFBColor){ 0xff, 0xc0, 0x00, 0x00 };
     sawman_config->borders[0].focused[1]   = (DFBColor){ 0xff, 0xb0, 0x00, 0x00 };
     sawman_config->borders[0].focused[2]   = (DFBColor){ 0xff, 0xa0, 0x00, 0x00 };
     sawman_config->borders[0].focused[3]   = (DFBColor){ 0xff, 0x90, 0x00, 0x00 };
     sawman_config->borders[0].unfocused[0] = (DFBColor){ 0xff, 0x80, 0x80, 0x80 };
     sawman_config->borders[0].unfocused[1] = (DFBColor){ 0xff, 0x70, 0x70, 0x70 };
     sawman_config->borders[0].unfocused[2] = (DFBColor){ 0xff, 0x60, 0x60, 0x60 };
     sawman_config->borders[0].unfocused[3] = (DFBColor){ 0xff, 0x50, 0x50, 0x50 };

     for(i=0;i<4;i++) {
          sawman_config->borders[0].focused_index[i]   = -1;
          sawman_config->borders[0].unfocused_index[i] = -1;
     }

#ifdef CC_HW_CURSOR_SUPPORT

	 sawman_config->cursor.hw       = true;

     #ifdef CC_DFB_SUPPORT_VDP_LAYER
     sawman_config->cursor.layer_id = 3;
	 #else
     sawman_config->cursor.layer_id = 2;
	 #endif
	 
#endif

    sawman_config->max_resolution.w = 1920;
	sawman_config->max_resolution.h = 1080;
     return DFB_OK;
}

const char*
sawman_config_usage( void )
{
     return config_usage;
}

DirectResult
sawman_config_set( const char *name, const char *value )
{
     if (strcmp (name, "init-border" ) == 0) {
          if (value) {
               int index;

               if (sscanf( value, "%d", &index ) < 1) {
                    D_ERROR("SaWMan/Config '%s': Could not parse value!\n", name);
                    return DFB_INVARG;
               }

               if (index < 0 || index > D_ARRAY_SIZE(sawman_config->borders)) {
                    D_ERROR("SaWMan/Config '%s': Value %d out of bounds!\n", name, index);
                    return DFB_INVARG;
               }

               sawman_config->border = &sawman_config->borders[index];
          }
          else {
               D_ERROR("SaWMan/Config '%s': No value specified!\n", name);
               return DFB_INVARG;
          }
     } else
     if (strcmp (name, "border-thickness" ) == 0) {
          SaWManBorderInit *border = sawman_config->border;

          if (value) {
               if (sscanf( value, "%d", &border->thickness ) < 1) {
                    D_ERROR("SaWMan/Config '%s': Could not parse value!\n", name);
                    return DFB_INVARG;
               }
          }
          else {
               D_ERROR("SaWMan/Config '%s': No value specified!\n", name);
               return DFB_INVARG;
          }
     } else
     if (strcmp (name, "border-resolution" ) == 0) {
          SaWManBorderInit *border = sawman_config->border;

          if (value) {
               int width, height;

               if (sscanf( value, "%dx%d", &width, &height ) < 2) {
                    D_ERROR("SaWMan/Config '%s': Could not parse dimension!\n", name);
                    return DFB_INVARG;
               }

               border->resolution.w = width;
               border->resolution.h = height;
          }
          else {
               D_ERROR("SaWMan/Config '%s': No width and height specified!\n", name);
               return DFB_INVARG;
          }
     } else
     if (strcmp (name, "border-format" ) == 0) {
          SaWManBorderInit *border = sawman_config->border;

          if (value) {
               DFBSurfacePixelFormat format;

               format = dfb_config_parse_pixelformat( value );
               if (format == DSPF_UNKNOWN) {
                    D_ERROR("SaWMan/Config '%s': Could not parse format!\n", name);
                    return DFB_INVARG;
               }

               border->format = format;
          }
          else {
               D_ERROR("SaWMan/Config '%s': No format specified!\n", name);
               return DFB_INVARG;
          }
     } else
     if (strncmp (name, "border-focused-color-index", 26 ) == 0 || strncmp (name, "border-unfocused-color-index", 28 ) == 0) {
          SaWManBorderInit *border = sawman_config->border;
          int               cindex = (name[7] == 'f') ? (name[26] - '0') : (name[28] - '0');

          if (cindex < 0 || cindex > D_ARRAY_SIZE(border->focused)) {
               D_ERROR("SaWMan/Config '%s': Value %d out of bounds!\n", name, cindex);
               return DFB_INVARG;
          }

          if (value) {
               char *error;
               u32   index;

               index = strtoul( value, &error, 10 );

               if (*error) {
                    D_ERROR( "SaWMan/Config '%s': Error in index '%s'!\n", name, error );
                    return DFB_INVARG;
               }

               if (strncmp (name, "border-focused-color-index", 26 ) == 0)
                    border->focused_index[cindex] = index;
               else
                    border->unfocused_index[cindex] = index;
          }
          else {
               D_ERROR( "SaWMan/Config '%s': No index specified!\n", name );
               return DFB_INVARG;
          }
     } else
     if (strncmp (name, "border-focused-color", 20 ) == 0 || strncmp (name, "border-unfocused-color", 22 ) == 0) {
          SaWManBorderInit *border = sawman_config->border;
          int               cindex = (name[7] == 'f') ? (name[20] - '0') : (name[22] - '0');

          if (cindex < 0 || cindex > D_ARRAY_SIZE(border->focused)) {
               D_ERROR("SaWMan/Config '%s': Value %d out of bounds!\n", name, cindex);
               return DFB_INVARG;
          }

          if (value) {
               char *error;
               u32   argb;

               argb = strtoul( value, &error, 16 );

               if (*error) {
                    D_ERROR( "SaWMan/Config '%s': Error in color '%s'!\n", name, error );
                    return DFB_INVARG;
               }

               if (strncmp (name, "border-focused-color", 20 ) == 0) {
                    border->focused[cindex].a = (argb & 0xFF000000) >> 24;
                    border->focused[cindex].r = (argb & 0xFF0000) >> 16;
                    border->focused[cindex].g = (argb & 0xFF00) >> 8;
                    border->focused[cindex].b = (argb & 0xFF);
                    border->focused_index[cindex] = -1;
               }
               else {
                    border->unfocused[cindex].a = (argb & 0xFF000000) >> 24;
                    border->unfocused[cindex].r = (argb & 0xFF0000) >> 16;
                    border->unfocused[cindex].g = (argb & 0xFF00) >> 8;
                    border->unfocused[cindex].b = (argb & 0xFF);
                    border->unfocused_index[cindex] = -1;
               }
          }
          else {
               D_ERROR( "SaWMan/Config '%s': No color specified!\n", name );
               return DFB_INVARG;
          }
     } else
     if (strcmp (name, "show-empty") == 0) {
          sawman_config->show_empty = true;
     } else
     if (strcmp (name, "no-show-empty") == 0) {
          sawman_config->show_empty = false;
     } else
     if (strcmp (name, "flip-once-timeout") == 0) {
          if (value) {
               char *error;
               u32   timeout;

               timeout = strtoul( value, &error, 10 );

               if (*error) {
                    D_ERROR( "SaWMan/Config '%s': Error in timeout '%s'!\n", name, error );
                    return DFB_INVARG;
               }

               sawman_config->flip_once_timeout = timeout;
          }
          else {
               D_ERROR( "SaWMan/Config '%s': No timeout specified!\n", name );
               return DFB_INVARG;
          }
     } else
     if (strcmp (name, "hw-cursor" ) == 0) {
          if (value) {
               int id;

               if (sscanf( value, "%d", &id ) < 1) {
                    D_ERROR("SaWMan/Config '%s': Could not parse value!\n", name);
                    return DFB_INVARG;
               }

               if (id < 0 || id >= MAX_LAYERS) {
                    D_ERROR("SaWMan/Config '%s': Value %d out of bounds!\n", name, id);
                    return DFB_INVARG;
               }

               sawman_config->cursor.hw       = true;
               sawman_config->cursor.layer_id = id;
          }
          else {
               D_ERROR("SaWMan/Config '%s': No value specified!\n", name);
               return DFB_INVARG;
          }
     } else
     if (strcmp (name, "resolution" ) == 0) {
          if (value) {
               int width, height;

               if (sscanf( value, "%dx%d", &width, &height ) < 2) {
                    D_ERROR("SaWMan/Config '%s': Could not parse dimension!\n", name);
                    return DFB_INVARG;
               }

               sawman_config->resolution.w = width;
               sawman_config->resolution.h = height;
          }
          else {
               D_ERROR("SaWMan/Config '%s': No width and height specified!\n", name);
               return DFB_INVARG;
          }
     } else
     if (strcmp (name, "max_resolution" ) == 0) {
          if (value) {
               int width, height;

               if (sscanf( value, "%dx%d", &width, &height ) < 2) {
                    D_ERROR("SaWMan/Config '%s': Could not parse dimension!\n", name);
                    return DFB_INVARG;
               }

               sawman_config->max_resolution.w = width;
               sawman_config->max_resolution.h = height;
          }
          else {
               D_ERROR("SaWMan/Config '%s': No width and height specified!\n", name);
               return DFB_INVARG;
          }
     } else
          return DFB_UNSUPPORTED;	 

     return DFB_OK;
}

static DFBResult 
sawman_config_read( const char *filename )
{
     DFBResult  ret = DFB_OK;
     char       line[400];
     FILE      *f;

     f = fopen( filename, "r" );
     if (!f) {
          D_DEBUG( "SaWMan/Config: "
                   "Unable to open config file `%s'!\n", filename );
          return DFB_IO;
     } else {
          D_INFO( "SaWMan/Config: "
                  "Parsing config file '%s'.\n", filename );
     }

     while (fgets( line, 400, f )) {
          char *name  = line;
          char *value = strchr( line, '=' );

          if (value) {
               *value++ = 0;
               direct_trim( &value );
          }

          direct_trim( &name );

          if (!*name || *name == '#')
               continue;

          ret = sawman_config_set( name, value );
          if (ret) {
               if (ret == DFB_UNSUPPORTED)
                    D_ERROR( "SaWMan/Config: In config file `%s': "
                             "Invalid option `%s'!\n", filename, name );
               break;
          }
     }

     fclose( f );

     return ret;
}

DirectResult
sawman_config_init( int *argc, char **argv[] )
{
     DFBResult ret;
     
     if (!sawman_config) {
          char *home = getenv( "HOME" );
          char *prog = NULL;
          char *swargs;

          ret = config_allocate();
          if (ret)
               return ret;
     
          /* Read system settings. */
          ret = sawman_config_read( SYSCONFDIR"/sawmanrc" );
          if (ret  &&  ret != DFB_IO)
               return ret;
               
          /* Read user settings. */
          if (home) {
               int  len = strlen(home) + sizeof("/.sawmanrc");
               char buf[len];
     
               snprintf( buf, len, "%s/.sawmanrc", home );
     
               ret = sawman_config_read( buf );
               if (ret  &&  ret != DFB_IO)
                    return ret;
          }
          
          /* Get application name. */
          if (argc && *argc && argv && *argv) {
               prog = strrchr( (*argv)[0], '/' );
     
               if (prog)
                    prog++;
               else
                    prog = (*argv)[0];
          }
     
          /* Read global application settings. */
          if (prog && prog[0]) {
               int  len = sizeof(SYSCONFDIR"/sawmanrc.") + strlen(prog);
               char buf[len];
     
               snprintf( buf, len, SYSCONFDIR"/sawmanrc.%s", prog );
     
               ret = sawman_config_read( buf );
               if (ret  &&  ret != DFB_IO)
                    return ret;
          }
          
          /* Read user application settings. */
          if (home && prog && prog[0]) {
               int  len = strlen(home) + sizeof("/.sawmanrc.") + strlen(prog);
               char buf[len];
     
               snprintf( buf, len, "%s/.sawmanrc.%s", home, prog );
     
               ret = sawman_config_read( buf );
               if (ret  &&  ret != DFB_IO)
                    return ret;
          }
          
          /* Read settings from environment variable. */
          swargs = getenv( "SAWMANARGS" );
          if (swargs) {
               ret = parse_args( swargs );
               if (ret)
                    return ret;
          }
     }
     
     /* Read settings from command line. */
     if (argc && argv) {
          int i;
          
          for (i = 1; i < *argc; i++) {

               if (!strcmp( (*argv)[i], "--sawman-help" )) {
                    fprintf( stderr, config_usage );
                    exit(1);
               }

               if (!strncmp( (*argv)[i], "--sawman:", 9 )) {
                    ret = parse_args( (*argv)[i] + 9 );
                    if (ret)
                         return ret;

                    (*argv)[i] = NULL;
               }
          }

          for (i = 1; i < *argc; i++) {
               int k;

               for (k = i; k < *argc; k++)
                    if ((*argv)[k] != NULL)
                         break;

               if (k > i) {
                    int j;

                    k -= i;

                    for (j = i + k; j < *argc; j++)
                         (*argv)[j-k] = (*argv)[j];

                    *argc -= k;
               }
          }
     }

     return DFB_OK;
}

DirectResult sawman_config_shutdown( void )
{
     D_FREE( sawman_config );
     return DFB_OK;
}
