/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>

#include <dfb_types.h>

#include <directfb.h>
#include <directfb_keyboard.h>

#include <core/coredefs.h>
#include <core/coretypes.h>
#include <core/input.h>
#include <core/system.h>

#include <direct/clock.h>
#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/messages.h>
#include <direct/thread.h>
#include <direct/util.h>

#include <misc/conf.h>
#include <misc/util.h>

#include <core/input_driver.h>

#include <linux/input.h>

DFB_INPUT_DRIVER( usbmouse ) ;

#define DBG(_xx_)  /* printf _xx_ */
#define LOG(_xx_)  /* printf _xx_ */

/* default mouse device configuration. */

#define TRUE   true
#define FALSE  false

#define MOUSE_BUF_SIZE    3

/* Note: these values are the same as DFBInputDeviceButtonMask. */
#define LEFT_MOUSE_BTN    (0x01)
#define RIGHT_MOUSE_BTN   (0x02)
#define CENTER_MOUSE_BTN  (0x04)

#define MOUSE_NO_EVENT             (0x0) 
#define MOUSE_BTN_PRESSED_EVENT   (0x01)
#define MOUSE_BTN_RELEASED_EVENT  (0x02)
#define MOUSE_MOVEMENT_EVENT      (0x04) 

#define ABSVAL(_val_)  ( (_val_ > 0) ? (_val_) : ( (_val_) * (-1) ) )
#define MOUSE_MAX(_a_,_b_)  ( ((_a_) > (_b_)) ? (_a_) : (_b_) )
#define MOUSE_MIN(_a_,_b_)  ( ((_a_) < (_b_)) ? (_a_) : (_b_) )

static char*  MOUSE_DEV="/dev/input/mouse0";
static const int     accel_limit = 5;
static const int     accel = 2;

    
/*
 * declaration of private data
 */
typedef struct _MouseObj_
{
    CoreInputDevice         *device;
    DirectThread            *thread;
    bool                     quit_thread;

    int                      mousefd ;
    unsigned char            mousebuf[MOUSE_BUF_SIZE];
    int                      mouseIdx ;
    int                      obstate  ;
    
    int                      current_mouse_x  ;
    int                      current_mouse_y  ;
    int                      screen_width     ;
    int                      screen_height    ;
} MouseObj;

typedef struct _MouseEvent_
{
    int event_type;
    int pos_x;
    int pos_y;
    int bstate;
    int but_count;
    int buts[3];
    
} MouseEvent;

    
static void Mouse_Reset(MouseObj* data)
{
    data->device      = NULL;
    data->thread      = NULL;
    data->quit_thread = true;
    data->mousefd = -1 ;
    data->mouseIdx = 0 ;
    data->obstate  = -1 ;
    data->current_mouse_x = 0 ;
    data->current_mouse_y = 0 ;
    data->screen_width    = 0 ;
    data->screen_height   = 0 ;

    return ;
}

/*----------------------------------------------------------------------
 *
 * Name: mouse_set_screen
 *
 * Description:  Set the screen width and height values. 
 *
 * Input:
 *    data:   ref to data structure for a mouse object.
 *    width:  width (pixel) of the screen.
 *    height: height (pixel) of the screen.
 *
 * Output:
 *    None
 *
 * Returns: File descriptor
 *            If ( ret value < 0 ) devic open failed
 *            if ( ret value >= 0 ) device open success.
 *
------------------------------------------------------------------------*/
static void mouse_set_screen(MouseObj* data, int width, int height)
{
    data->screen_width  = width;
    data->screen_height = height;
    data->current_mouse_x = ( (data->screen_width) / 2 );
    data->current_mouse_y = ( (data->screen_height) / 2 );
    data->obstate         = -1 ;
    data->mouseIdx        = 0 ;

    DBG(("{Mouse:USB} set screen width: %d height: %d\n",data->screen_width,data->screen_height));
    
    return;
}

/*----------------------------------------------------------------------
 *
 * Name: mouse_open_device
 *
 * Description:  Open a /dev (e.g. /dev/input/mouse0) 
 *
 * Input:
 *    data:   ref to data structure for a mouse object.
 *
 * Output:
 *    None
 *
 * Returns: File descriptor
 *            If ( ret value < 0 ) devic open failed
 *            if ( ret value >= 0 ) device open success.
 *
------------------------------------------------------------------------*/
static int mouse_open_device(MouseObj* data)
{
	if ( data->mousefd < 0 )
    {
        /* open mouse device, read-only, blocking mode. */
        if (( data->mousefd = open(MOUSE_DEV, O_RDONLY)) < 0 )
        {
            printf("{Mouse:USB} can not open mouse dev: %s, "
                   "error code: %d\n",
                   MOUSE_DEV, data->mousefd);

            printf("Please check if you have a USB mouse physically "
                   "connected to the board.\n");

            printf("To check if USB mouse is connected, "
                   "type 'cat /proc/bus/input/devices' "
                   "to see what devices are connected.\n");
            
            return data->mousefd;
        }
        else
        {
            printf("{Mouse:USB} open mouse dev: %s\n",MOUSE_DEV);
        }
	}

    data->obstate = -1 ;
    data->mouseIdx = 0 ;
    
    /* Clear pending input. */
    tcflush(data->mousefd, TCIFLUSH);

    printf("{Mouse:USB} mouse dev: %s ready (fd: %d)\n",
           MOUSE_DEV,data->mousefd);
    
	return data->mousefd;
}

/*----------------------------------------------------------------------
 *
 * Name: send_mouse_event
 *
 * Description:  Send the mouse event to DirectFB (use
 *    dfb_input_dispatch()).
 *
 *    The semantic of mouse event defined by this inputdriver:
 *
 *    1) Mouse movement (no button press or release)
 *     
 *          send mouse position x 
 *          send mouse position y 
 *
 *       A total of 2 messages dispatch to DirectFB.
 *
 *    2) Mouse button press or release
 *
 *          Loop on number of button event (press or release)
 *          {
 *             send mouse button event (Press or Release, which button,
 *             and button state)
 *          }
 *          send mouse position x
 *          send mouse position y
 *
 *       A minimum of 3 messages dispatched to DirectFB.
 *
 * Input:
 *    data:   ref to data structure for a mouse object.
 *
 * Output:
 *    None
 *
 * Returns: File descriptor
 *            If ( ret value < 0 ) devic open failed
 *            if ( ret value >= 0 ) device open success.
 *
------------------------------------------------------------------------*/
static void send_mouse_event
(
    MouseObj*           data,
    MouseEvent*         evt
)
{
     DFBInputEvent dfbevent;

     dfbevent.type = DIET_UNKNOWN;

     dfbevent.clazz      = DFEC_INPUT;

     if ( evt->event_type ==  MOUSE_BTN_PRESSED_EVENT )
     {
         dfbevent.type = DIET_BUTTONPRESS;
     }
     else if ( evt->event_type ==  MOUSE_BTN_RELEASED_EVENT )
     {
         dfbevent.type = DIET_BUTTONRELEASE;
     }
     else if ( evt->event_type == MOUSE_MOVEMENT_EVENT )
     {
         dfbevent.type = DIET_AXISMOTION;
     }

     if ( (dfbevent.type == DIET_BUTTONPRESS) ||
          (dfbevent.type == DIET_BUTTONRELEASE)
        )
     {
         int i;
         
         /* which additional (optional) event fields are valid. */
         dfbevent.flags  =  DIEF_BUTTONS | DIEF_FOLLOW ;

         for (i=0 ; i < evt->but_count; i++ )
         {
             /* The button which action has occured (pressed or released) */
             if ( evt->buts[i] == LEFT_MOUSE_BTN )
             {
                 dfbevent.button = DIBI_LEFT;
             }
             else if ( evt->buts[i] == RIGHT_MOUSE_BTN )
             {
                 dfbevent.button = DIBI_RIGHT;
             }
             else if ( evt->buts[i] == CENTER_MOUSE_BTN )
             {
                 dfbevent.button = DIBI_MIDDLE;
             }

             /* button state:  if bit is set, then button is currently in
                pressed down state. */
             dfbevent.buttons = evt->bstate;

             dfb_input_dispatch( data->device, &dfbevent );

             LOG(("{Inputdriver} Send Mouse Button: type: %d  but: %d\n",evt->event_type,evt->buts[i]));
             
         }

         /*
            After processing BUTTON event, we will also send the mouse
            position information to DirectFB.  We do this setting
            the event type to AXISMOTION.
         */
         dfbevent.type = DIET_AXISMOTION;
     }
     
     if ( (dfbevent.type == DIET_AXISMOTION) )
     {
         /* which additional (optional) event fields are valid. */
         dfbevent.flags   = DIEF_AXISABS | DIEF_FOLLOW;

         /* Pedro: Why do we have to  send it  twice ?  */
         dfbevent.axis    = DIAI_X;
         dfbevent.axisabs = evt->pos_x;
         dfb_input_dispatch( data->device, &dfbevent );

         dfbevent.flags   = DIEF_AXISABS ;
         dfbevent.axis    = DIAI_Y;
         dfbevent.axisabs = evt->pos_y;
         dfb_input_dispatch( data->device, &dfbevent );

         LOG(("{Inputdriver} Send Mouse Movement: x: %d  y: %d\n",evt->pos_x,evt->pos_y));
         
     }
     

     return;
}

/*----------------------------------------------------------------------
 *
 * Name: mouse_pos
 *
 * Description:  Get the current mouse position.
 *
 * Input:
 *    data:   ref to data structure for a mouse object.
 *
 * Output:
 *    pos_x:  absolute x position.
 *    pos_y:  absolute y position.
 *
 * Returns:
 *    None
------------------------------------------------------------------------*/
static void mouse_pos(MouseObj *data, int* pos_x, int* pos_y)
{
    *pos_x = data->current_mouse_x ;
    *pos_y = data->current_mouse_y ;

    return;
}

/*----------------------------------------------------------------------
 *
 * Name: set_mouse_pos
 *
 * Description:  Set the  mouse position.
 *
 * Input:
 *    data:   ref to data structure for a mouse object.
 *
 * Output:
 *    pos_x:  absolute x position.
 *    pos_y:  absolute y position.
 *
 * Returns:
 *    None
------------------------------------------------------------------------*/
static void set_mouse_pos(MouseObj *data, int pos_x, int pos_y)
{
    data->current_mouse_x = pos_x;
    data->current_mouse_y = pos_y;

    DBG(("mouse pos: %d %d\n",data->current_mouse_x,data->current_mouse_y));
    
    return;
}
/*----------------------------------------------------------------------
 * Name: limit_to_screen
 *
 * Description:  Clip the mouse pos to within the screen width/height
 *    value.
 *
 * Input:
 *    data:   ref to data structure for a mouse object.
 *    pos_x:  absolute x position.
 *    pos_y:  absolute y position.
 *
 * Output:
 *    pos_x:  clipped absolute x position.
 *    pos_y:  clipped absolute y position.
 *
 * Returns:
 *    void
------------------------------------------------------------------------*/
static void limit_to_screen(MouseObj *data, int* pos_x, int* pos_y)
{
    int xval, yval;

    xval=*pos_x;
    yval=*pos_y;

    xval = MOUSE_MIN(data->screen_width-1,  MOUSE_MAX(0,xval)) ;
    yval = MOUSE_MIN(data->screen_height-1, MOUSE_MAX(0,yval)) ;

    *pos_x = xval;
    *pos_y = yval;

    return;
}

/*----------------------------------------------------------------------
 * Name: mouse_btn_event
 *
 * Description: Analyze the current and old button state to
 *    determine the type of button event to send to upper layer.
 *
 * Input:
 *    bstate:   current button state.
 *    obstate:  old button state.
 *
 * Output:
 *    
 *
 * Returns:
 *    void
------------------------------------------------------------------------*/
static int mouse_btn_event(int bstate, int obstate, MouseEvent* evt )
{
    int result = MOUSE_NO_EVENT;
    
    if ( obstate == -1 )
    {
        return result ;
    }
    
    if ( bstate != obstate )
    {
        int change_bstate = bstate ^ obstate ;

        evt->but_count = 0;

        if ( change_bstate & LEFT_MOUSE_BTN )
        {
            evt->buts[evt->but_count]=LEFT_MOUSE_BTN;
            evt->but_count++;
            
            if ( bstate & LEFT_MOUSE_BTN )
            {
                result = MOUSE_BTN_PRESSED_EVENT;
                DBG(("[Event => Left Mouse Btn Pressed]\n"));
            }
            else
            {
                result = MOUSE_BTN_RELEASED_EVENT;
                DBG(("[Event => Left Mouse Btn Released]\n"));
            }
        }

        if ( change_bstate & RIGHT_MOUSE_BTN )
        {
            evt->buts[evt->but_count]=RIGHT_MOUSE_BTN;
            evt->but_count++;
            
            if ( bstate & RIGHT_MOUSE_BTN )
            {
                result = MOUSE_BTN_PRESSED_EVENT;
                DBG(("[Event => Right Mouse Btn Pressed]\n"));
            }
            else
            {
                result = MOUSE_BTN_RELEASED_EVENT;
                DBG(("[Event => Right Mouse Btn Released]\n"));
            }
        }

        if ( change_bstate & CENTER_MOUSE_BTN )
        {
            evt->buts[evt->but_count]=CENTER_MOUSE_BTN;
            evt->but_count++;
            
            if ( bstate & CENTER_MOUSE_BTN )
            {
                result = MOUSE_BTN_PRESSED_EVENT;
                DBG(("[Event => Center Mouse Btn Pressed]\n"));
            }
            else
            {
                result = MOUSE_BTN_RELEASED_EVENT;
                DBG(("[Event => Center Mouse Btn Released]\n"));
            }
        }
    }
        
    return result;
}

/*----------------------------------------------------------------------
 * Name: mouse_read_event
 *
 * Description: Read and parse the mouse event from mouse device.
 *
 * Input:
 *    data:   ref to data structure for a mouse object.
 *    
 *
 * Output:
 *    
 *
 * Returns:
 *    void
------------------------------------------------------------------------*/
static void mouse_read_event(MouseObj *data)
{
    int            n, i;
    int            idx = 0;
    int            bstate = 0;
    int            dx = 0, dy = 0;
    bool           sendEvent = TRUE;
    int            tdx = 0, tdy = 0;
    int            surplus ;

    DBG(("{mouse_read_event} enter....\n"));
    
    for (;;)
    {
        if ( ( MOUSE_BUF_SIZE - data->mouseIdx ) < 3)
        {
            DBG(("{mouse_read_event} buffer is filled.\n")); 
            break;
        }

        DBG(("{mouse_read_event} Reading mouse fd: %d\n",data->mousefd));
        
        n = read(data->mousefd, ((data->mousebuf)+(data->mouseIdx)), 3);
        if (n != 3)
        {
            DBG(("{mouse_read_event}  no mouse input data...\n")); 
            break;
        }
        data->mouseIdx += 3;
    }

    while ( (data->mouseIdx - idx) >= 3)
    {
        unsigned char *mb ;

        mb = data->mousebuf+idx;
        
        bstate = 0;
        dx = 0;
        dy = 0;
        
        if ( (mb[0] & LEFT_MOUSE_BTN) )
        {
            /* printf("[Left btn] \n"); */
            bstate |= LEFT_MOUSE_BTN;
        }
        
        if ( (mb[0] & RIGHT_MOUSE_BTN) )
        {
            /* printf("[Right btn] \n"); */
            bstate |= RIGHT_MOUSE_BTN;
        }

        if ( (mb[0] & CENTER_MOUSE_BTN) )
        {
            /* printf("[Center btn] \n"); */
            bstate |= CENTER_MOUSE_BTN;
        }
        
        dx=(signed char)mb[1];
        dy=(signed char)mb[2];

#if 1
        DBG(("dx: %d  dy: %d\n",dx,dy));
#endif
        
        sendEvent=TRUE;
        if (sendEvent)
        {
            int pos_x, pos_y;
            int opos_x, opos_y;
            
            if ( ABSVAL(dx) > accel_limit || ABSVAL(dy) > accel_limit)
            {
                dx *= accel;
                dy *= accel;
            }
            tdx += dx;
            tdy += dy;

            /* get the current mouse position. */
            mouse_pos(data, &pos_x, &pos_y);

            /* save the current mouse position. */
            opos_x=pos_x;
            opos_y=pos_y;

            /* compute the new mouse position, apply screen limit */
            pos_x  = pos_x + tdx ;
            pos_y  = pos_y - tdy ;
            limit_to_screen(data, &pos_x, &pos_y);
                
            if ( (bstate != data->obstate) ||
                 (opos_x != pos_x) ||
                 (opos_y != pos_y)
                )
            {
                MouseEvent  evt;
                int         result;

                set_mouse_pos(data, pos_x, pos_y);
                
                /* send mouse motion event notification . */
                if ( (opos_x != pos_x) || (opos_y != pos_y) )
                {
                    evt.event_type = MOUSE_MOVEMENT_EVENT;
                    evt.pos_x = pos_x;
                    evt.pos_y = pos_y;
                    evt.bstate = bstate;

                    send_mouse_event(data,&evt);
                }

                /* compare the new button state with old button state
                   to determine the button action
                */
                result = mouse_btn_event(bstate, data->obstate, &evt);
                if ( (result == MOUSE_BTN_PRESSED_EVENT) ||
                     (result == MOUSE_BTN_RELEASED_EVENT)
                   )
                {
                    evt.event_type = result;
                    evt.pos_x = pos_x;
                    evt.pos_y = pos_y;
                    evt.bstate = bstate;

                    send_mouse_event(data,&evt);
                }
                    
                sendEvent = FALSE;

                tdx = 0;
                tdy = 0;
                /* save the new button state. */
                data->obstate = bstate;
            }
        }
        
        idx += 3;
    }
    
    surplus = data->mouseIdx - idx;
    for (i = 0; i < surplus; i++)
    {
        data->mousebuf[i] = data->mousebuf[idx+i];
    }
    data->mouseIdx = surplus;

    return;
}


/*----------------------------------------------------------------------
 * Name:  get_screen_width_height_from_config_file
 *
 * Description: This function read the DirectFB config file
 *    (/etc/directfbrc) to get the screen width and height values.
 *
 * Input:
 *    fpath:  File path of 'DirectFB' configuration file.
 *
 * Output:
 *    width   contains the width of screen.
 *    height  contains the height of screen.
 *
 * Returns:
 *    void
------------------------------------------------------------------------*/
void get_screen_width_height_from_config_file
(
    char* fpath,
    int* width,
    int* height
)
{
#define  LINE_LEN   128
    
    FILE*  fp=NULL;
    char   line[LINE_LEN];
    int    w,h;
    
    /* default size. */
    *width = 1280;
    *height = 720;
    
    printf("open %s \n",fpath);
    
    fp=fopen(fpath,"r");
    if ( fp != NULL )
    {
        printf("Read config file <%s> to get screen width and height.\n",
               fpath);
    }
    else
    {
        printf("config file: <%s> not found. use default value of %d x %d\n",
               fpath, *width, *height);
        return;
    }

    while ( ! feof(fp) )
    {
        char* result;
        char  name[64];
        char  cval[64];
        
        result = fgets(line, LINE_LEN, fp);
        if ( result == NULL )
        {
            break;
        }
        
        /* skip comment line. */
        if ( line[0]=='#' )
        {
            continue;
        }
        /* printf("input: %s\n",line); */
        
        sscanf(line,"%s = %s",name,cval);
        if ( strcmp(name,"layer-size") == 0 )
        {
            /* printf("cval: %s\n",cval); */
            
            /* get width and height. */
            sscanf(cval,"%dx%d", &w, &h);
            *width=w;
            *height=h;
            break;
        }
    }

    fclose(fp);
    
    return ;
}

/*----------------------------------------------------------------------
 * Name:  mouse_event_input_thread
 *
 * Description: This function is executed in context of DirectFB
 *    event thread, it reads the mouse event from mouse device
 *    and dispatch the mouse events to DirectFB.
 *
 * Input:
 *    thread: Ref to DirectFB thread.
 *    data:   ref to data structure for a mouse object.
 *
 * Output:
 *    none.
 *
 * Returns:
 *    void
------------------------------------------------------------------------*/
static void* mouse_event_input_thread( DirectThread *thread, void *arg )
{
    int   width, height;
    
    MouseObj *data = arg;

    printf("{USB Mouse} input thread started....\n");

    /* pedro: This need to be re-visited, we need to get the screen size. */
    get_screen_width_height_from_config_file
        ("/etc/directfbrc", &width, &height);
    
    mouse_set_screen(data, width, height);
    
    while ( ! data->quit_thread ) 
    {
        /* Read mouse event: x position, y position and button data.
           and dispatch the event to DirectFB.
        */
        mouse_read_event(data);
    }

    printf("{USB Mouse} input thread terminated.\n");
    return NULL;
}

/*
 * Return the number of available devices.
 * Called once during initialization of DirectFB.
 */
static int
driver_get_available()
{
     return 1;
}

/*
 * Fill out general information about this driver.
 * Called once during initialization of DirectFB.
 */
static void
driver_get_info( InputDriverInfo *info )
{
     /* fill driver info structure */
     snprintf ( info->name,
                DFB_INPUT_DRIVER_INFO_NAME_LENGTH, "USB Mouse Driver" );
     snprintf ( info->vendor,
                DFB_INPUT_DRIVER_INFO_VENDOR_LENGTH, "MTK Pedro Tsai" );

     info->version.major = 0;
     info->version.minor = 1;
}

/*
 * Open the device, fill out information about it,
 * allocate and fill private data, start input thread.
 * Called during initialization, resuming or taking over mastership.
 */
static DFBResult
driver_open_device( CoreInputDevice  *device,
                    unsigned int      number,
                    InputDeviceInfo  *info,
                    void            **driver_data )
{
     MouseObj *data = NULL;

     /* fill info */
     snprintf( info->desc.name, DFB_INPUT_DEVICE_DESC_NAME_LENGTH, "MediaTek USB Mouse Input" );
     snprintf( info->desc.vendor, DFB_INPUT_DEVICE_DESC_VENDOR_LENGTH, "MediaTek" );

     info->prefered_id = DIDID_MOUSE;
     info->desc.caps = DICAPS_AXES | DICAPS_BUTTONS ;
     info->desc.type = DIDTF_MOUSE;

     /* allocate and fill private data */
     data = D_CALLOC( 1, sizeof(MouseObj) );
     if (!data)
     {
         return D_OOM();
     }

     Mouse_Reset(data);

     if ( mouse_open_device(data) >= 0 )
     {
         data->device = device;
         data->quit_thread = false;
         data->thread = direct_thread_create( DTT_INPUT,
                                              mouse_event_input_thread,
                                              data,"USB_Mouse" );

         /* set private data pointer */
         *driver_data = data;
     }
     else
     {
         D_FREE( data );
         return DFB_FAILURE;
     }
     return DFB_OK;
}


/*
 * Fetch one entry from the kernel keymap.
 */
static DFBResult
driver_get_keymap_entry( CoreInputDevice           *device,
                         void                      *driver_data,
                         DFBInputDeviceKeymapEntry *entry )
{
     return DFB_UNSUPPORTED;
}

/*
 * End thread, close device and free private data.
 */
static void
driver_close_device( void *driver_data )
{
     MouseObj *data = driver_data;

     data->quit_thread = true;

     direct_thread_join( data->thread );
     direct_thread_destroy( data->thread );

     close(data->mousefd);

     /* free private data */
     D_FREE( data );
}

