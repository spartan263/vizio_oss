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

#define BTN_NONE                ((u32)0xffffffff)
#define DFB_RAW_KEY             DIKS_CUSTOM31

DFB_INPUT_DRIVER( cli_input )

//D_DEBUG_DOMAIN( CLIInput, "CLI/Input", "CLI System Input" );

/*
 * RC6 command to DirectFB symbol mapping
 */
static const DFBInputDeviceKeySymbol _Rc6Command[256] =
{
/****************************************************************************************************************************************************************************/
    /*      *   0x00 / 0x08         0x01 / 0x09         0x02 / 0x0A         0x03 / 0x0B         0x04 / 0x0C         0x05 / 0x0D         0x06 / 0x0E         0x07 / 0x0F         */
    /****************************************************************************************************************************************************************************/
    /* 0x00 */  DIKS_0,             DIKS_1,             DIKS_2,             DIKS_3,             DIKS_4,             DIKS_5,             DIKS_6,             DIKS_7,
    /*      */  DIKS_8,             DIKS_9,             DIKS_PREVIOUS,      DFB_RAW_KEY,        DIKS_POWER,         DIKS_MUTE,          DFB_RAW_KEY,        DIKS_INFO,
    /* 0x10 */  DIKS_VOLUME_UP,     DIKS_VOLUME_DOWN,   DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_CHANNEL_UP,    DIKS_CHANNEL_DOWN,
    /* 0x20 */  DIKS_CHANNEL_UP,    DIKS_CHANNEL_DOWN,  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DIKS_FASTFORWARD,   DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_REWIND,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0x30 */  DFB_RAW_KEY,        DIKS_STOP,          DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_STOP,          DIKS_RECORD,
    /*      */  DIKS_NEXT,          DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_TEXT,          DIKS_POWER2,        DFB_RAW_KEY,        DIKS_TUNER,
    /* 0x40 */  DIKS_OPTION,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_SUBTITLE,      DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0x50 */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_MENU,          DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DIKS_CURSOR_UP,     DIKS_CURSOR_DOWN,   DIKS_CURSOR_LEFT,   DIKS_CURSOR_RIGHT,  DIKS_OK,            DIKS_MODE,          DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0x60 */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_RED,           DIKS_GREEN,         DIKS_YELLOW,
    /* 0x70 */  DIKS_BLUE,          DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_SUBTITLE,      DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0x80 */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0x90 */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0xA0 */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0xB0 */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_MENU,          DFB_RAW_KEY,
    /* 0xC0 */  DFB_RAW_KEY,        DIKS_PLAYPAUSE,     DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_EPG,           DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0xD0 */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DIKS_PERIOD,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0xE0 */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,
    /* 0xF0 */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DIKS_ZOOM,          DFB_RAW_KEY,        DFB_RAW_KEY,
    /*      */  DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY,        DFB_RAW_KEY
};    

#ifdef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
/*
 * NEC command to DirectFB symbol mapping
 */
#define DFB_BTN_NONE                    ((u32)0x00000000)
static const DFBInputDeviceKeySymbol _NECCommand[96] =
{
    DFB_BTN_RED, DFB_BTN_GREEN, DFB_BTN_POWER, DFB_BTN_YELLOW, DFB_BTN_ZOOM, DFB_BTN_BLUE, DFB_BTN_TIMER, DFB_BTN_MEM_CARD,
    DFB_BTN_DIGIT_3, DFB_BTN_DIGIT_2, DFB_BTN_DIGIT_1, DFB_BTN_ENTER, DFB_BTN_DIGIT_9, DFB_BTN_DIGIT_8, DFB_BTN_DIGIT_7, DFB_BTN_FAV_CH,
    DFB_BTN_DIGIT_6, DFB_BTN_DIGIT_5, DFB_BTN_DIGIT_4, DFB_BTN_CURSOR_DOWN, DFB_BTN_MENU, DFB_BTN_DIGIT_0, DFB_BTN_FREEZE, DFB_BTN_RECORD,
    DFB_BTN_PRG_INFO, DFB_BTN_EPG, DFB_BTN_EXIT, DFB_BTN_PREV, DFB_BTN_CURSOR_RIGHT, DFB_BTN_CURSOR_UP, DFB_BTN_CURSOR_LEFT, DFB_BTN_P_EFFECT,
    DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE,
    DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE,
    DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE,
    DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE,
    DFB_BTN_S_EFFECT, DFB_BTN_NONE, DFB_BTN_PIP_SIZE, DFB_BTN_MTS, DFB_BTN_REPEAT, DFB_BTN_SUB_TITLE, DFB_BTN_TITLE_PBC, DFB_BTN_SWAP,
    DFB_BTN_FAVORITE, DFB_BTN_ADD_ERASE, DFB_BTN_NEXT, DFB_BTN_NONE, DFB_BTN_VOL_UP, DFB_BTN_FR, DFB_BTN_PRG_DOWN, DFB_BTN_NONE,
    DFB_BTN_FF, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_NONE, DFB_BTN_VOL_DOWN, DFB_BTN_PRG_UP, DFB_BTN_MUTE, DFB_BTN_PREV_PRG,
    DFB_BTN_PIP_POP, DFB_BTN_INPUT_SRC, DFB_BTN_DIGIT_DOT, DFB_BTN_ROOT_MENU, DFB_BTN_PIP_POS, DFB_BTN_ASPECT, DFB_BTN_CC, DFB_BTN_SLEEP
};
#endif

/*
 * declaration of private data
 */
typedef struct {
     CoreInputDevice         *device;

     DirectThread            *thread;
     bool                     quit_thread;

     DFBInputDeviceKeySymbol  last_symbol;
     long long                last_time;
} CLIInputData;

static int CLI_Parser(const char *szCmdBuf)
{
	int	fd, ret;

    if (strlen(szCmdBuf) >= 256)
    {
        return -1;
    }

	if ((fd = open("/dev/cli", O_RDWR)) < 0)
	{
		return -2;
	}

    ret = ioctl(fd, 1, szCmdBuf);

	close(fd);

	return ret;
}

static void
send_symbol( CLIInputData            *data,
             DFBInputDeviceKeySymbol  symbol,
             int key_code,
             bool                     down )
{
     DFBInputEvent event;

     if (symbol == DIKS_NULL)
          return;

     event.type       = down ? DIET_KEYPRESS : DIET_KEYRELEASE;
     event.flags      = DIEF_KEYSYMBOL | DIEF_KEYCODE;
     event.key_symbol = symbol;

     // OR with 0x03000000 to indicate this is RC6     
     event.key_code = key_code | 0x03000000;
     
     printf("BTN_%s: 0x%x\n", (down?"DOWN":"UP"), event.key_code);

     dfb_input_dispatch( data->device, &event );
}

static void *
input_thread( DirectThread *thread, void *arg )
{
    CLIInputData *data = arg;
        
    u32 raw;
    u32 system;    
    u32 command = 0;
    u32 toggle = 0;
    u32 last_toggle = 0;
#ifndef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
    u32 prev_command = 0;
#else
    u32 prev_command = 0xffffffff;
    bool nec_ir;
#endif
    
    DFBInputDeviceKeySymbol symbol;

          
#ifdef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
    CLI_Parser( "ir.rx.mtk.init nec" );
#else
    CLI_Parser( "ir.rx.mtk.init rc5" );
#endif
    
    while (!data->quit_thread) 
    {

    
        raw = (u32) CLI_Parser( "ir.rx.mtk.raw" );

#ifdef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
        // NEC
        nec_ir = false;
        if ((raw & 0x0000FFFF) == 0xE31C)
        {
            command = ((raw >> 16) & 0x00FF);
            symbol = _NECCommand[command];
            nec_ir = true;
        }
        else if (raw == DFB_BTN_NONE)
        {
            symbol = DIKS_NULL;
            nec_ir = true;
        }
        else
#endif
        // Only accept RC6, discard RC5 if received
        if(raw == BTN_NONE || ((raw >> 28) & 0xf) != 0x2)
        {
            symbol = DIKS_NULL;
        }
        else
        {
            system = (raw >> 7) & 0xff;
            command = ((raw >> 15) & 0xff);

            // TELETEXT
            if(command == 0xD3)
            {
                command = 0x3C;
                raw = ((raw & ~(0xff << 15)) || (command << 15));
            }
            
            symbol = _Rc6Command[command];
            toggle = raw & 0x0060;
printf( "<<mtcli key raw %x>>\n", raw );
        }
       
        if (symbol == DIKS_NULL) 
        { /* polling timeout. */
            if (data->last_symbol != DIKS_NULL) 
            { /* Here is a key up. */
            int delta = direct_clock_get_abs_millis() - data->last_time;
printf( "<<mtcli key timeout -- up (%i ms)>>\n", delta );
#ifdef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
                send_symbol(data, (nec_ir?_NECCommand:_Rc6Command)[command], command, false); 
#else
                send_symbol(data, _Rc6Command[command], command, false); 
#endif
                data->last_symbol = DIKS_NULL;
                
            }
        } 
        else if (data->last_symbol != DIKS_NULL && prev_command == command) 
        { /* the same key with previous key. */
        
        
            if ( toggle != last_toggle  && direct_clock_get_abs_millis() - data->last_time > 250 ) 
            {                                                  
            int delta2 = direct_clock_get_abs_millis() - data->last_time;
printf( "<<mtcli key timeout -- resend (%i ms)>>\n", delta2 );
#ifdef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
                send_symbol(data, (nec_ir?_NECCommand:_Rc6Command)[command], command, false);
                send_symbol(data, (nec_ir?_NECCommand:_Rc6Command)[command], command, true);
#else
                send_symbol(data,_Rc6Command[command], command, false);
                send_symbol(data, _Rc6Command[command], command, true);
#endif
                last_toggle = toggle;
#ifdef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
                if (nec_ir)
                {
                    last_toggle ++;
                }
#endif
            } // otherwise, do nothing.
            data->last_time   = direct_clock_get_abs_millis(); 
            
        } 
        else
        {
            if (data->last_symbol != DIKS_NULL)
            {
#ifdef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
                send_symbol(data, (nec_ir?_NECCommand:_Rc6Command)[prev_command], prev_command, false);
#else
                send_symbol(data, _Rc6Command[prev_command], prev_command, false);
#endif
            }
            if ( prev_command != command || toggle != last_toggle )
            {
#ifdef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
                send_symbol(data, (nec_ir?_NECCommand:_Rc6Command)[command], command, true);           
#else
                send_symbol(data, _Rc6Command[command], command, true);           
#endif
                last_toggle = toggle;
#ifdef KEY_FROM_DFB /* MHF Linux - Bin Zhang */
                if (nec_ir)
                {                    
                    last_toggle ++;                
                }
#endif
                prev_command = command;
                data->last_symbol = symbol;
                data->last_time   = direct_clock_get_abs_millis();                     
            }
    
        }
        
        usleep( 10000 );
          
     }

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
                DFB_INPUT_DRIVER_INFO_NAME_LENGTH, "CLI Input Driver" );
     snprintf ( info->vendor,
                DFB_INPUT_DRIVER_INFO_VENDOR_LENGTH, "Denis Oliver Kropp" );

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
     CLIInputData *data;

     /* fill info */
     snprintf( info->desc.name, DFB_INPUT_DEVICE_DESC_NAME_LENGTH, "MediaTek CLI Input" );
     snprintf( info->desc.vendor, DFB_INPUT_DEVICE_DESC_VENDOR_LENGTH, "MediaTek" );

     info->desc.caps = DICAPS_KEYS;
     info->desc.type = DIDTF_REMOTE;

     /* allocate and fill private data */
     data = D_CALLOC( 1, sizeof(CLIInputData) );
     if (!data)
          return D_OOM();

     data->device = device;

     data->thread = direct_thread_create( DTT_INPUT, input_thread, data, "CLI Input" );     

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
     CLIInputData *data = driver_data;

     data->quit_thread = true;

     direct_thread_join( data->thread );
     direct_thread_destroy( data->thread );

     /* free private data */
     D_FREE( data );
}

