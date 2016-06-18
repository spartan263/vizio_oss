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
#ifdef CC_DFB_INPUT_MTNET_SUPPORT

#include <config.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <linux/version.h>

#include <dfb_types.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include <directfb.h>
#include <directfb_keyboard.h>

#include <core/coredefs.h>
#include <core/coretypes.h>
#include <core/input.h>
#include <core/system.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/messages.h>
#include <direct/thread.h>
#include <direct/util.h>

#include <misc/conf.h>
#include <misc/util.h>

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
 
 
#include <core/input_driver.h>
 

DFB_INPUT_DRIVER( netevent )

#define MAX_INPUT_DEV_INUSE 1  /* create number of thread */
#define MAX_netevent_DEVICES 1


#define TAGNAME  "DFBNetEvt-R1"
struct netevnet_packet
{
	char tag[16]; /* should be TAGNAME*/
	DFBInputEvent evt;
};

/*
 * declaration of private data
 */
typedef struct {
     CoreInputDevice         *device;
     DirectThread            *thread;
     int                      fd;
     int                      number;
     struct netevnet_packet	pkt;
} NetEventData;


static int sock=-1;

static struct sockaddr_in mtnet_addr;  /* Internet socket name */

static int driver_open=1;

#ifndef CC_MTNET_ACCEPT_ALL
#define CC_MTNET_ACCEPT_ALL  0
#endif

/*
 * Input thread reading from device.
 * Generates events on incoming data.
 */
static void*
netevent_EventThread( DirectThread *thread, void *driver_data )
{
     NetEventData    *data = (NetEventData*) driver_data;

	int cnt;
	struct netevnet_packet *pkt=&data->pkt;
     DFBInputEvent devt={0};

     D_INFO("[NETEVT]thread-%d\n", data->number);
     
     if(data->number==0)
     {
	    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	       D_INFO("[NETEVT]network socket fail=%d\n", errno);
	    }
	    else
	    {
		mtnet_addr.sin_family = AF_INET;  /* Internet address */
		mtnet_addr.sin_port = htons(3579)/*0xdfb*/;  /* System will assign port #  */
#if CC_MTNET_ACCEPT_ALL
		mtnet_addr.sin_addr.s_addr = INADDR_ANY;  /* "Wildcard" */
#else
		mtnet_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  /* "loopback" 0x0100007f */
#endif
		if ( bind(sock, &mtnet_addr, sizeof(mtnet_addr) ) < 0 ) {
			D_INFO("[NETEVT]network socket bind socket fail=%d\n", errno);
			close(sock);
			sock=-1;
		}
   		if ( listen ( sock, 5 ) < 0 ) {
			D_INFO("[NETEVT]network socket listen socket fail=%d\n", errno);
			close(sock);
			sock=-1;
   		}
	    }	 
     }
     
     while (driver_open) { 
     	if(sock<0)
	{
		usleep(1000);
		continue;
	}

      if ( ( data->fd = accept ( sock, 0, 0 ) ) < 0 ) {
		D_INFO("[NETEVT]network socket accept socket fail=%d\n", errno);
		close(sock);
		sock=-1;
		continue;
      }
         do {      
            memset (pkt,0,sizeof(struct netevnet_packet));  
            if (( cnt = read (data->fd, pkt, sizeof(struct netevnet_packet))) < 0 ) {
	       D_INFO("[NETEVT]read socket fail=%d\n", errno);
               close(data->fd);
            }
            else if (cnt == 0) {
               D_INFO("[NETEVT]msg len=0,closing...\n");
               close (data->fd);
            }
            else {
		if(strncmp(pkt->tag,TAGNAME,16)==0)
		{		
			//D_INFO("[NETEVT]dispatch event\n");
			dfb_input_dispatch( data->device, &pkt->evt );
		}
		else	D_INFO("[NETEVT]got wrong event tag\n");

            } 
 
         }  while (cnt > 0); 
     }
     D_INFO("[EVT_TH]thread %d leaving\n",data->number); 
     return NULL;
}

/*
 * Fill device information.
 * Queries the input device and tries to classify it.
 */
static void
get_device_info(InputDeviceInfo *info)
{
     #define  num_buttons  3
     #define  num_rels     3
     #define  num_abs      0

     /* get device name */
     snprintf( info->desc.name,
               DFB_INPUT_DEVICE_DESC_NAME_LENGTH, "DFB NetEvent Device" );
     /* set device vendor */
     snprintf( info->desc.vendor,
               DFB_INPUT_DEVICE_DESC_VENDOR_LENGTH, "Mediatek" );

     info->desc.type |= DIDTF_MOUSE|DIDTF_JOYSTICK|DIDTF_KEYBOARD|DIDTF_REMOTE;

     /* A Keyboard, do we have at least some letters? */
     info->desc.caps |= DICAPS_KEYS|DICAPS_BUTTONS|DICAPS_AXES;

     info->desc.min_keycode =-1;
     info->desc.max_keycode =-1;//KEY_OK+ D_ARRAY_SIZE( ext_keycodes ) -1;;

     /* Buttons */
     info->desc.max_button  = DIBI_FIRST + num_buttons - 1;

     /* Axes */
     info->desc.max_axis    = DIAI_FIRST + MAX(num_rels, num_abs) - 1;

     /* Decide which primary input device to be. */
     if (info->desc.type & DIDTF_KEYBOARD)
          info->prefered_id = DIDID_KEYBOARD;
     else if (info->desc.type & DIDTF_REMOTE)
          info->prefered_id = DIDID_REMOTE;
     else if (info->desc.type & DIDTF_JOYSTICK)
          info->prefered_id = DIDID_JOYSTICK;
     else if (info->desc.type & DIDTF_MOUSE)
          info->prefered_id = DIDID_MOUSE;
     else
          info->prefered_id = DIDID_ANY;
}

/* exported symbols */

/*
 * Return the number of available devices.
 * Called once during initialization of DirectFB.
 */
static int
driver_get_available()
{
     return MAX_INPUT_DEV_INUSE;
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
                DFB_INPUT_DRIVER_INFO_NAME_LENGTH, "DFB NetEvent Driver" );
     snprintf ( info->vendor,
                DFB_INPUT_DRIVER_INFO_VENDOR_LENGTH, "Mediatek" );
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
     int              fd, ret;
     NetEventData  *data;

     D_INFO("[NETEVT]open net evnet %d\n",number);
     /* open device */
     driver_open=1;

     /* fill device info structure */
     get_device_info(info);

     /* allocate and fill private data */
     data = D_CALLOC( 1, sizeof(NetEventData) );

     data->fd     = -1;
     data->device = device;
     data->number=number;
     D_INFO("[NETEVT]create thread %d\n",number);
     /* start input thread */
     data->thread = direct_thread_create( DTT_INPUT, netevent_EventThread, data, "net event" );

     /* set private data pointer */
     *driver_data = data;

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
     NetEventData *data = (NetEventData*) driver_data;
     D_INFO("[NETEVT]closing net event dev\n");
     driver_open=0;
	if(data->fd>=0)
	{
     /* stop input thread */
     direct_thread_cancel( data->thread );
     direct_thread_join( data->thread );
     direct_thread_destroy( data->thread );

     /* release device */
     close( data->fd );
	}
     /* free private data */
     D_FREE( data );
}

#endif
