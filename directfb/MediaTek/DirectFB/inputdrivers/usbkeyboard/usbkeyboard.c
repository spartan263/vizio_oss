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

DFB_INPUT_DRIVER( usbkeyboard ) ;

#define DBG(_xx_)  /* printf _xx_ */
#define LOG(_xx_)  /* printf _xx_ */

/* default mouse device configuration. */

#define TRUE   true
#define FALSE  false

/* keyboard event mask */
#define KEY_NO_EVENT             (0x0) 
#define KEY_PRESSED_EVENT        (0x01)
#define KEY_RELEASED_EVENT       (0x02)
#define KEY_HOLD_EVENT           (0x04)

/*
  modifer masks.
*/
#define SHIFT_KEY                (0x01)
#define CONTROL_KEY              (0x02)
#define ALT_KEY                  (0x04)
#define ALTGR_KEY                (0x08)
#define META_KEY                 (0x10)
#define SUPER_KEY                (0x20)
#define HYPER_KEY                (0x40)

/*
  keylocks state mask
*/    
#define SCROLL_KEY               (0x01)
#define NUM_KEY               (0x02)  
#define CAPS_KEY              (0x04)

/* device file to read the key inputs. */
static char*     KYB_DEV="/dev/input/input0";

/*
 * declaration of private data
 */
typedef struct _KeyboardObj_
{
    CoreInputDevice         *device;
    DirectThread            *thread;
    int                     kybfd ;
    bool                    quit_thread;

    /* keep track the current state of keyboard. */
    int                     kyb_state;
    unsigned int            modkeys;
    unsigned int            lockkeys;
    unsigned int            o_lockkeys;
                      
} KeyboardObj;

typedef struct _KeyboardEvent_
{
    int                  event_type;
    struct input_event   inp;
} KeyboardEvent;

    
static void keyboard_reset(KeyboardObj* data)
{
    data->device        = NULL;
    data->thread        = NULL;
    data->quit_thread   = true;
    data->kybfd         = -1 ;

    data->kyb_state     = -1 ;
    data->modkeys       = 0 ;
    data->lockkeys      = 0 ;
    data->o_lockkeys    = 0 ;

    return ;
}

/*----------------------------------------------------------------------
 *
 * Name: keyboard_open_device
 *
 * Description:  Open a Linux keyboard input device (e.g. /dev/input/input0) 
 *
 * Input:
 *    data:   ref to data structure for a keyboard object.
 *
 * Output:
 *    None
 *
 * Returns: File descriptor
 *     If ( ret value < 0 ) device open failed
 *     if ( ret value >= 0 ) device open success.
 *
------------------------------------------------------------------------*/
static int keyboard_open_device(KeyboardObj* data)
{
	if ( data->kybfd < 0 )
    {
        /* open keyboard device, read-only, blocking mode. */
        if ( ( data->kybfd = open(KYB_DEV, O_RDONLY) ) < 0 )
        {
            printf("{keyboard_open_device} can not open keyboard dev: %s, "
                   "error code: %d\n",
                   KYB_DEV, data->kybfd);

            printf("Please check if you have a USB keyboard physically "
                   "connected to the board.\n");

            printf("To check if USB keyboard is connected, "
                   "type 'cat /proc/bus/input/devices' "
                   "to see what devices are connected.\n");
            
            return data->kybfd;
        }
        else
        {
            printf("{keyboard_open_device} open keyboard dev: %s\n",KYB_DEV);
        }
	}
    
    /* Clear pending input. */
    tcflush(data->kybfd, TCIFLUSH);

    printf("{keyboard_open_device} keyboard dev: %s ready (fd: %d)\n",
           KYB_DEV,data->kybfd);
    
	return data->kybfd;
}

typedef struct _INPUT_CODE_DFB_KEYID_MAP_
{
    DFBInputDeviceKeyIdentifier     dfb_id;
    int                             code;
} INPUT_CODE_DFB_KEYID_MAP;


static INPUT_CODE_DFB_KEYID_MAP  input_code_dfb_keyid_table[] =
{
    { DIKI_A,                     KEY_A },
    { DIKI_B,                     KEY_B },
    { DIKI_C,                     KEY_C },
    { DIKI_D,                     KEY_D },
    { DIKI_E,                     KEY_E },
    { DIKI_F,                     KEY_F },
    { DIKI_G,                     KEY_G },
    { DIKI_H,                     KEY_H },
    { DIKI_I,                     KEY_I },
    { DIKI_J,                     KEY_J },
    { DIKI_K,                     KEY_K },
    { DIKI_L,                     KEY_L },
    { DIKI_M,                     KEY_M },
    { DIKI_N,                     KEY_N },
    { DIKI_O,                     KEY_O },
    { DIKI_P,                     KEY_P },
    { DIKI_Q,                     KEY_Q },
    { DIKI_R,                     KEY_R },
    { DIKI_S,                     KEY_S },
    { DIKI_T,                     KEY_T },
    { DIKI_U,                     KEY_U },
    { DIKI_V,                     KEY_V },
    { DIKI_W,                     KEY_W },
    { DIKI_X,                     KEY_X },
    { DIKI_Y,                     KEY_Y },
    { DIKI_Z,                     KEY_Z },
    { DIKI_0,                     KEY_0 },
    { DIKI_1,                     KEY_1 },
    { DIKI_2,                     KEY_2 },
    { DIKI_3,                     KEY_3 },
    { DIKI_4,                     KEY_4 },
    { DIKI_5,                     KEY_5 },
    { DIKI_6,                     KEY_6 },
    { DIKI_7,                     KEY_7 },
    { DIKI_8,                     KEY_8 },
    { DIKI_9,                     KEY_9 },
    { DIKI_F1,                    KEY_F1 },
    { DIKI_F2,                    KEY_F2 },
    { DIKI_F3,                    KEY_F3 },
    { DIKI_F4,                    KEY_F4 },
    { DIKI_F5,                    KEY_F5 },
    { DIKI_F6,                    KEY_F6 },
    { DIKI_F7,                    KEY_F7 },
    { DIKI_F8,                    KEY_F8 },
    { DIKI_F9,                    KEY_F9 },
    { DIKI_F10,                   KEY_F10 },
    { DIKI_F11,                   KEY_F11 },
    { DIKI_F12,                   KEY_F12 },
    { DIKI_SHIFT_L,               KEY_LEFTSHIFT },
    { DIKI_SHIFT_R,               KEY_RIGHTSHIFT },
    { DIKI_CONTROL_L,             KEY_LEFTCTRL },
    { DIKI_CONTROL_R,             KEY_RIGHTCTRL },
    { DIKI_ALT_L,                 KEY_LEFTALT },
    { DIKI_ALT_R,                 KEY_RIGHTALT },
    { DIKI_META_L,                KEY_LEFTMETA },
    { DIKI_META_R,                KEY_RIGHTMETA },

#if 0    
    /*
      pedro:  What are super and hyper keys ????
    */
    { DIKI_SUPER_L,               KEY_SUPER_L },
    { DIKI_SUPER_R,               KEY_SUPER_R },
    { DIKI_HYPER_L,               KEY_HYPER_L },
    { DIKI_HYPER_R,               KEY_HYPER_R },
#endif    
   
    { DIKI_CAPS_LOCK,             KEY_CAPSLOCK },
    { DIKI_NUM_LOCK,              KEY_NUMLOCK },
    { DIKI_SCROLL_LOCK,           KEY_SCROLLLOCK },
    { DIKI_ESCAPE,                KEY_ESC },
    { DIKI_LEFT,                  KEY_LEFT },
    { DIKI_RIGHT,                 KEY_RIGHT },
    { DIKI_UP,                    KEY_UP },
    { DIKI_DOWN,                  KEY_DOWN },
    { DIKI_TAB,                   KEY_TAB },
    { DIKI_ENTER,                 KEY_ENTER },
    { DIKI_SPACE,                 KEY_SPACE },
    { DIKI_BACKSPACE,             KEY_BACKSPACE },
    { DIKI_INSERT,                KEY_INSERT },
    { DIKI_DELETE,                KEY_DELETE },
    { DIKI_HOME,                  KEY_HOME },
    { DIKI_END,                   KEY_END },
    { DIKI_PAGE_UP,               KEY_PAGEUP },
    { DIKI_PAGE_DOWN,             KEY_PAGEDOWN },
    { DIKI_PRINT,                 KEY_PRINT },
    { DIKI_PAUSE,                 KEY_PAUSE },
    { DIKI_QUOTE_LEFT,            KEY_GRAVE },
    { DIKI_MINUS_SIGN,            KEY_MINUS },
    { DIKI_EQUALS_SIGN,           KEY_EQUAL },
    { DIKI_BRACKET_LEFT,          KEY_LEFTBRACE }, /* if SHIFT pressed, left-brace.*/
    { DIKI_BRACKET_RIGHT,         KEY_RIGHTBRACE },/* if SHIFT pressed, right-brace.*/
    { DIKI_BACKSLASH,             KEY_BACKSLASH },
    { DIKI_SEMICOLON,             KEY_SEMICOLON },
    { DIKI_QUOTE_RIGHT,           KEY_APOSTROPHE }, /* pedro, check this !!! */
    { DIKI_COMMA,                 KEY_COMMA },
    { DIKI_PERIOD,                KEY_DOT },
    { DIKI_SLASH,                 KEY_SLASH },
    { DIKI_LESS_SIGN,             KEY_COMMA }, /* pedro, check this !!! */
    { DIKI_KP_DIV,                KEY_KPSLASH },
    { DIKI_KP_MULT,               KEY_KPASTERISK },
    { DIKI_KP_MINUS,              KEY_KPMINUS },
    { DIKI_KP_PLUS,               KEY_KPPLUS },
    { DIKI_KP_ENTER,              KEY_KPENTER },
    
#if 0
    /*
      pedro: can not find these in Linux input.h 
    */
    { DIKI_KP_SPACE,            KEY_KP_SPACE },
    { DIKI_KP_TAB,              KEY_KP_TAB },
    { DIKI_KP_F1,               KEY_KP_F1 },
    { DIKI_KP_F2,               KEY_KP_F2 },
    { DIKI_KP_F3,               KEY_KP_F3 },
    { DIKI_KP_F4,               KEY_KP_F4 },
#endif
    
    { DIKI_KP_EQUAL,              KEY_KPEQUAL },

#if 0    
    /*
      pedro: what is 'separator' ???
    */
    { DIKI_KP_SEPARATOR,          KEY_KP_SEPARATOR },
#endif
    
    { DIKI_KP_DECIMAL,            KEY_KPDOT },    
    { DIKI_KP_0,                  KEY_KP0 },
    { DIKI_KP_1,                  KEY_KP1 },
    { DIKI_KP_2,                  KEY_KP2 },
    { DIKI_KP_3,                  KEY_KP3 },
    { DIKI_KP_4,                  KEY_KP4 },
    { DIKI_KP_5,                  KEY_KP5 },
    { DIKI_KP_6,                  KEY_KP6 },
    { DIKI_KP_7,                  KEY_KP7 },
    { DIKI_KP_8,                  KEY_KP8 },
    { DIKI_KP_9,                  KEY_KP9 },
    { DIKI_KEYDEF_END,            KEY_RESERVED },
};

/*----------------------------------------------------------------------
 * Name: map_key_code_to_dfb_key_id
 *
 * Description: Map keyboard event code to directfb keyid. This is basic 
 *        mapping, independent of modifier key.
 %
 * Input:
 *    code:    raw key code 
 *
 * Output:
 *    
 *
 * Returns:
 *    DirectFB key id (basic mapping without modifier key state).
------------------------------------------------------------------------*/
static int map_key_code_to_dfb_key_id
(
    int code
)
{
    int i = 0 ;
    int result = DIKI_UNKNOWN ;
    while ( input_code_dfb_keyid_table[i].dfb_id != DIKI_KEYDEF_END )
    {
        if ( input_code_dfb_keyid_table[i].code ==
             code )
        {
            result = input_code_dfb_keyid_table[i].dfb_id ;

            /*
            printf("map key code <%d> to key id <%d>\n",
                   code, result);
            */
            
            break;
        }
        i++;
    }
    
    return result;
}


/*------------------------------------------------------------------
  Define the Linux Input symbol to DFB symbol
  mapping table.

  Depending on the state of modkeys and lockkey
  we will select the appropriate table for mapping.

  For example, if the 'shift-key' is down, then we
  will use the 'upper_case_char_map' table to
  map Linux input 'char letter' to DirectFB key symbol.

  Or if the 'num-lock' is active on keypad is active, we will
  use 'upper_kp_symbol_map' to map Linux input 'keypad code'
  to DirectFB keypad symbol.  But if 'num-lock' is not active,
  then we will use 'lower_kp_symbol_map' to map Linux input
  'keypad code' to DirectFB keypad symbol.

-------------------------------------------------------------------*/
typedef struct _INPUT_CODE_DFB_KEYSYMBOL_MAP_
{
    DFBInputDeviceKeyIdentifier     dfb_symbol;
    int                             code;
} INPUT_CODE_DFB_KEYSYMBOL_MAP;


static INPUT_CODE_DFB_KEYSYMBOL_MAP  lower_case_char_map[] =
{
    { DIKS_SMALL_A,               KEY_A },
    { DIKS_SMALL_B,               KEY_B },
    { DIKS_SMALL_C,               KEY_C },
    { DIKS_SMALL_D,               KEY_D },
    { DIKS_SMALL_E,               KEY_E },
    { DIKS_SMALL_F,               KEY_F },
    { DIKS_SMALL_G,               KEY_G },
    { DIKS_SMALL_H,               KEY_H },
    { DIKS_SMALL_I,               KEY_I },
    { DIKS_SMALL_J,               KEY_J },
    { DIKS_SMALL_K,               KEY_K },
    { DIKS_SMALL_L,               KEY_L },
    { DIKS_SMALL_M,               KEY_M },
    { DIKS_SMALL_N,               KEY_N },
    { DIKS_SMALL_O,               KEY_O },
    { DIKS_SMALL_P,               KEY_P },
    { DIKS_SMALL_Q,               KEY_Q },
    { DIKS_SMALL_R,               KEY_R },
    { DIKS_SMALL_S,               KEY_S },
    { DIKS_SMALL_T,               KEY_T },
    { DIKS_SMALL_U,               KEY_U },
    { DIKS_SMALL_V,               KEY_V },
    { DIKS_SMALL_W,               KEY_W },
    { DIKS_SMALL_X,               KEY_X },
    { DIKS_SMALL_Y,               KEY_Y },
    { DIKS_SMALL_Z,               KEY_Z },
};
static int nb_entry_lower_case_char_map = sizeof(lower_case_char_map)/sizeof(INPUT_CODE_DFB_KEYSYMBOL_MAP);


static INPUT_CODE_DFB_KEYSYMBOL_MAP upper_case_char_map[] =
{
    { DIKS_CAPITAL_A,             KEY_A },
    { DIKS_CAPITAL_B,             KEY_B },
    { DIKS_CAPITAL_C,             KEY_C },
    { DIKS_CAPITAL_D,             KEY_D },
    { DIKS_CAPITAL_E,             KEY_E },
    { DIKS_CAPITAL_F,             KEY_F },
    { DIKS_CAPITAL_G,             KEY_G },
    { DIKS_CAPITAL_H,             KEY_H },
    { DIKS_CAPITAL_I,             KEY_I },
    { DIKS_CAPITAL_J,             KEY_J },
    { DIKS_CAPITAL_K,             KEY_K },
    { DIKS_CAPITAL_L,             KEY_L },
    { DIKS_CAPITAL_M,             KEY_M },
    { DIKS_CAPITAL_N,             KEY_N },
    { DIKS_CAPITAL_O,             KEY_O },
    { DIKS_CAPITAL_P,             KEY_P },
    { DIKS_CAPITAL_Q,             KEY_Q },
    { DIKS_CAPITAL_R,             KEY_R },
    { DIKS_CAPITAL_S,             KEY_S },
    { DIKS_CAPITAL_T,             KEY_T },
    { DIKS_CAPITAL_U,             KEY_U },
    { DIKS_CAPITAL_V,             KEY_V },
    { DIKS_CAPITAL_W,             KEY_W },
    { DIKS_CAPITAL_X,             KEY_X },
    { DIKS_CAPITAL_Y,             KEY_Y },
    { DIKS_CAPITAL_Z,             KEY_Z },
};
static int nb_entry_upper_case_char_map = sizeof(upper_case_char_map)/sizeof(INPUT_CODE_DFB_KEYSYMBOL_MAP);

static INPUT_CODE_DFB_KEYSYMBOL_MAP lower_symbol_map[] =
{
    { DIKS_0,                     KEY_0 },
    { DIKS_1,                     KEY_1 },
    { DIKS_2,                     KEY_2 },
    { DIKS_3,                     KEY_3 },
    { DIKS_4,                     KEY_4 },
    { DIKS_5,                     KEY_5 },
    { DIKS_6,                     KEY_6 },
    { DIKS_7,                     KEY_7 },
    { DIKS_8,                     KEY_8 },
    { DIKS_9,                     KEY_9 },
    { DIKS_F1,                    KEY_F1 },
    { DIKS_F2,                    KEY_F2 },
    { DIKS_F3,                    KEY_F3 },
    { DIKS_F4,                    KEY_F4 },
    { DIKS_F5,                    KEY_F5 },
    { DIKS_F6,                    KEY_F6 },
    { DIKS_F7,                    KEY_F7 },
    { DIKS_F8,                    KEY_F8 },
    { DIKS_F9,                    KEY_F9 },
    { DIKS_F10,                   KEY_F10 },
    { DIKS_F11,                   KEY_F11 },
    { DIKS_F12,                   KEY_F12 },
    { DIKS_SHIFT,                 KEY_LEFTSHIFT },
    { DIKS_SHIFT,                 KEY_RIGHTSHIFT },
    { DIKS_CONTROL,               KEY_LEFTCTRL },
    { DIKS_CONTROL,               KEY_RIGHTCTRL },
    { DIKS_ALT,                   KEY_LEFTALT },
    { DIKS_ALT,                   KEY_RIGHTALT },
    { DIKS_META,                  KEY_LEFTMETA },
    { DIKS_META,                  KEY_RIGHTMETA },
    { DIKS_CAPS_LOCK,             KEY_CAPSLOCK },
    { DIKS_NUM_LOCK,              KEY_NUMLOCK },
    { DIKS_SCROLL_LOCK,           KEY_SCROLLLOCK },
    
    { DIKS_ESCAPE,                KEY_ESC },
    { DIKS_CURSOR_LEFT,           KEY_LEFT },
    { DIKS_CURSOR_RIGHT,          KEY_RIGHT },
    { DIKS_CURSOR_UP,             KEY_UP },
    { DIKS_CURSOR_DOWN,           KEY_DOWN },
    { DIKS_TAB,                   KEY_TAB },
    { DIKS_ENTER,                 KEY_ENTER },
    { DIKS_SPACE,                 KEY_SPACE },
    { DIKS_BACKSPACE,             KEY_BACKSPACE },
    { DIKS_INSERT,                KEY_INSERT },
    { DIKS_DELETE,                KEY_DELETE },
    { DIKS_HOME,                  KEY_HOME },
    { DIKS_END,                   KEY_END },
    { DIKS_PAGE_UP,               KEY_PAGEUP },
    { DIKS_PAGE_DOWN,             KEY_PAGEDOWN },
    { DIKS_PRINT,                 KEY_PRINT },
    { DIKS_PAUSE,                 KEY_PAUSE },
    { DIKS_GRAVE_ACCENT,          KEY_GRAVE },
    { DIKS_MINUS_SIGN,            KEY_MINUS },
    { DIKS_EQUALS_SIGN,           KEY_EQUAL },
    
    /* lower symbol map: 
       left-brace maps to left-bracket
       right-brace maps to righ-bracket
    */
    { DIKS_SQUARE_BRACKET_LEFT,   KEY_LEFTBRACE }, 
    { DIKS_SQUARE_BRACKET_RIGHT,  KEY_RIGHTBRACE },
    
    { DIKS_BACKSLASH,             KEY_BACKSLASH },
    { DIKS_SEMICOLON,             KEY_SEMICOLON },
    { DIKS_APOSTROPHE,            KEY_APOSTROPHE }, 
    { DIKS_COMMA,                 KEY_COMMA },
    { DIKS_PERIOD,                KEY_DOT },
    { DIKS_SLASH,                 KEY_SLASH },
    
};
static int nb_entry_lower_symbol_map = sizeof(lower_symbol_map)/sizeof(INPUT_CODE_DFB_KEYSYMBOL_MAP);

static INPUT_CODE_DFB_KEYSYMBOL_MAP upper_symbol_map[] =
{
    { DIKS_PARENTHESIS_RIGHT,     KEY_0 },
    { DIKS_EXCLAMATION_MARK,      KEY_1 },
    { DIKS_AT,                    KEY_2 },
    { DIKS_NUMBER_SIGN,           KEY_3 },
    { DIKS_DOLLAR_SIGN,           KEY_4 },
    { DIKS_PERCENT_SIGN,          KEY_5 },
    { DIKS_CIRCUMFLEX_ACCENT,     KEY_6 },
    { DIKS_AMPERSAND,             KEY_7 },
    { DIKS_ASTERISK,              KEY_8 },
    { DIKS_PARENTHESIS_LEFT,      KEY_9 },
    { DIKS_F1,                    KEY_F1 },
    { DIKS_F2,                    KEY_F2 },
    { DIKS_F3,                    KEY_F3 },
    { DIKS_F4,                    KEY_F4 },
    { DIKS_F5,                    KEY_F5 },
    { DIKS_F6,                    KEY_F6 },
    { DIKS_F7,                    KEY_F7 },
    { DIKS_F8,                    KEY_F8 },
    { DIKS_F9,                    KEY_F9 },
    { DIKS_F10,                   KEY_F10 },
    { DIKS_F11,                   KEY_F11 },
    { DIKS_F12,                   KEY_F12 },
    { DIKS_SHIFT,                 KEY_LEFTSHIFT },
    { DIKS_SHIFT,                 KEY_RIGHTSHIFT },
    { DIKS_CONTROL,               KEY_LEFTCTRL },
    { DIKS_CONTROL,               KEY_RIGHTCTRL },
    { DIKS_ALT,                   KEY_LEFTALT },
    { DIKS_ALT,                   KEY_RIGHTALT },
    { DIKS_META,                  KEY_LEFTMETA },
    { DIKS_META,                  KEY_RIGHTMETA },
    { DIKS_CAPS_LOCK,             KEY_CAPSLOCK },
    { DIKS_NUM_LOCK,              KEY_NUMLOCK },
    { DIKS_SCROLL_LOCK,           KEY_SCROLLLOCK },
    
    { DIKS_ESCAPE,                KEY_ESC },
    { DIKS_CURSOR_LEFT,           KEY_LEFT },
    { DIKS_CURSOR_RIGHT,          KEY_RIGHT },
    { DIKS_CURSOR_UP,             KEY_UP },
    { DIKS_CURSOR_DOWN,           KEY_DOWN },
    { DIKS_TAB,                   KEY_TAB },
    { DIKS_ENTER,                 KEY_ENTER },
    { DIKS_SPACE,                 KEY_SPACE },
    { DIKS_BACKSPACE,             KEY_BACKSPACE },
    { DIKS_INSERT,                KEY_INSERT },
    { DIKS_DELETE,                KEY_DELETE },
    { DIKS_HOME,                  KEY_HOME },
    { DIKS_END,                   KEY_END },
    { DIKS_PAGE_UP,               KEY_PAGEUP },
    { DIKS_PAGE_DOWN,             KEY_PAGEDOWN },
    { DIKS_PRINT,                 KEY_PRINT },
    { DIKS_PAUSE,                 KEY_PAUSE },

    
    { DIKS_TILDE,                 KEY_GRAVE },
    { DIKS_UNDERSCORE,            KEY_MINUS },
    { DIKS_PLUS_SIGN,             KEY_EQUAL },
    
    /* upper symbol map: 
       left-brace maps to left-curly-bracket
       right-brace maps to righ-curly-bracket
    */
    { DIKS_CURLY_BRACKET_LEFT,    KEY_LEFTBRACE }, 
    { DIKS_CURLY_BRACKET_RIGHT,   KEY_RIGHTBRACE },
    
    { DIKS_VERTICAL_BAR,          KEY_BACKSLASH },
    { DIKS_COLON,                 KEY_SEMICOLON },
    { DIKS_QUOTATION,             KEY_APOSTROPHE }, 
    { DIKS_LESS_THAN_SIGN,        KEY_COMMA },
    { DIKS_GREATER_THAN_SIGN,     KEY_DOT },
    { DIKS_QUESTION_MARK,         KEY_SLASH },
    
};
static int nb_entry_upper_symbol_map = sizeof(upper_symbol_map)/sizeof(INPUT_CODE_DFB_KEYSYMBOL_MAP);

static INPUT_CODE_DFB_KEYSYMBOL_MAP lower_kp_symbol_map[] =
{
    { DIKS_SLASH,                 KEY_KPSLASH },
    { DIKS_ASTERISK,              KEY_KPASTERISK },
    { DIKS_MINUS_SIGN,            KEY_KPMINUS },
    { DIKS_PLUS_SIGN,             KEY_KPPLUS },
    { DIKS_ENTER,                 KEY_KPENTER },
    { DIKS_EQUALS_SIGN,           KEY_KPEQUAL },

    { DIKS_DELETE,                KEY_KPDOT },    
    { DIKS_INSERT,                KEY_KP0 },
    { DIKS_END,                   KEY_KP1 },
    { DIKS_CURSOR_DOWN,           KEY_KP2 },
    { DIKS_PAGE_DOWN,             KEY_KP3 },
    { DIKS_CURSOR_LEFT,           KEY_KP4 },
    { DIKS_NULL,                  KEY_KP5 }, /* KP5 maps to NULL in lower symbol */
    { DIKS_CURSOR_RIGHT,          KEY_KP6 },
    { DIKS_HOME,                  KEY_KP7 },
    { DIKS_CURSOR_UP,             KEY_KP8 },
    { DIKS_PAGE_UP,               KEY_KP9 },
};
static int nb_entry_lower_kp_symbol_map = sizeof(lower_kp_symbol_map)/sizeof(INPUT_CODE_DFB_KEYSYMBOL_MAP);

static INPUT_CODE_DFB_KEYSYMBOL_MAP upper_kp_symbol_map[] =
{
    { DIKS_SLASH,                 KEY_KPSLASH },
    { DIKS_ASTERISK,              KEY_KPASTERISK },
    { DIKS_MINUS_SIGN,            KEY_KPMINUS },
    { DIKS_PLUS_SIGN,             KEY_KPPLUS },
    { DIKS_ENTER,                 KEY_KPENTER },
    { DIKS_EQUALS_SIGN,           KEY_KPEQUAL },
    
    { DIKS_DELETE,                KEY_KPDOT },    
    { DIKS_0,                     KEY_KP0 },
    { DIKS_1,                     KEY_KP1 },
    { DIKS_2,                     KEY_KP2 },
    { DIKS_3,                     KEY_KP3 },
    { DIKS_4,                     KEY_KP4 },
    { DIKS_5,                     KEY_KP5 }, 
    { DIKS_6,                     KEY_KP6 },
    { DIKS_7,                     KEY_KP7 },
    { DIKS_8,                     KEY_KP8 },
    { DIKS_9,                     KEY_KP9 },
};
static int nb_entry_upper_kp_symbol_map = sizeof(upper_kp_symbol_map)/sizeof(INPUT_CODE_DFB_KEYSYMBOL_MAP);

/*----------------------------------------------------------------------
 * Name: map_key_code_to_dfb_key_symbol
 *
 * Description: Map keyboard event code to directfb key symbol. This is 
 *     advanced mapping, it depends on the modifier keys state.
 %
 * Input:
 *    KeyboardObj:     Keyboard object 
 *    code:            raw key code.
 *
 * Output:
 *    
 *
 * Returns:
 *    void
------------------------------------------------------------------------*/
static int map_key_code_to_dfb_key_symbol
(
    KeyboardObj*       data,
    int                code
)
{
    INPUT_CODE_DFB_KEYSYMBOL_MAP*   char_map;
    int                             nb_entry_char_map = 0;
    
    INPUT_CODE_DFB_KEYSYMBOL_MAP*   symbol_map;
    int                             nb_entry_symbol_map = 0;
    
    INPUT_CODE_DFB_KEYSYMBOL_MAP*   kp_symbol_map;
    int                             nb_entry_kp_symbol_map = 0;
    
    int                             i = 0 ;
    int                             result = DIKS_NULL ;
    

    /* From the state of modkey and lockkey, detemine which
       translation table should be used to map Linux Input key
       event to DirectFB Key symbol.
    */
    if ( data->modkeys & SHIFT_KEY )
    {
        if ( data->lockkeys & CAPS_KEY )
        {
            /* User lower_case_char_map */
            char_map = lower_case_char_map ;
            nb_entry_char_map = nb_entry_lower_case_char_map;
        }
        else
        {
            /* Use upper_case_char_map */
            char_map = upper_case_char_map ;
            nb_entry_char_map = nb_entry_upper_case_char_map ;
        }
        
        /* for non-char, use upper_symobl_map */
        symbol_map = upper_symbol_map;
        nb_entry_symbol_map = nb_entry_upper_symbol_map ;

        
        if ( data->lockkeys & NUM_KEY )
        {
            /* User lower_keypad_map */
            kp_symbol_map = lower_kp_symbol_map;
            nb_entry_kp_symbol_map = nb_entry_lower_kp_symbol_map;
        }
        else
        {
            /* Use upper_keypad_map */
            kp_symbol_map = upper_kp_symbol_map;
            nb_entry_kp_symbol_map = nb_entry_upper_kp_symbol_map;
        }
    }
    else
    {
        if ( data->lockkeys & CAPS_KEY )
        {
            /* User upper_case_char_map */
            char_map = upper_case_char_map ;
            nb_entry_char_map = nb_entry_upper_case_char_map ;
        }
        else
        {
            /* Use lower_case_char_map */
            char_map = lower_case_char_map ;
            nb_entry_char_map = nb_entry_lower_case_char_map;
        }
        
        /* for non-char, use lower_symobl_map */
        symbol_map = lower_symbol_map;
        nb_entry_symbol_map = nb_entry_lower_symbol_map ;
        
        if ( data->lockkeys & NUM_KEY )
        {
            /* User upper_keypad_map */
            kp_symbol_map = upper_kp_symbol_map;
            nb_entry_kp_symbol_map = nb_entry_upper_kp_symbol_map;
        }
        else
        {
            /* Use lower_keypad_map */
            kp_symbol_map = lower_kp_symbol_map;
            nb_entry_kp_symbol_map = nb_entry_lower_kp_symbol_map;
        }
    }

    /* go through all translation map table to find the corresponding
       DFB keysymbol value. */
    for (i=0 ; i < nb_entry_char_map ; i++ )
    {
        if ( char_map[i].code == code )
        {
            result = char_map[i].dfb_symbol ;
            return result;
        }
    }
    
    for (i=0 ; i < nb_entry_symbol_map ; i++ )
    {
        if ( symbol_map[i].code == code )
        {
            result = symbol_map[i].dfb_symbol ;
            return result;
        }
    }

    for (i=0 ; i < nb_entry_kp_symbol_map ; i++ )
    {
        if ( kp_symbol_map[i].code == code )
        {
            result = kp_symbol_map[i].dfb_symbol ;
            return result;
        }
    }
    return result;
}



/*----------------------------------------------------------------------
 *
 * Name: send_keyboard_event
 *
 * Description:  Send the keyboard event to DirectFB (use
 *    dfb_input_dispatch()).
 *
 *    The semantic of keyboard event defined by this inputdriver:
 *
 *    
 *
 * Input:
 *    data:   ref to data structure for a keyboard object.
 *
 * Output:
 *    None
 *
 * Returns:
 *    void
 *
------------------------------------------------------------------------*/
static void send_keyboard_event
(
    KeyboardObj*           data,
    KeyboardEvent*         evt
)
{
     DFBInputEvent dfbevent;

     LOG(("{Inputdriver} Send keyboard event: \n"));
     
     dfbevent.type = DIET_UNKNOWN;

     dfbevent.clazz      = DFEC_INPUT;
     
     if ( evt->event_type ==  KEY_PRESSED_EVENT )
     {
         dfbevent.type = DIET_KEYPRESS;
     }
     else if ( evt->event_type ==  KEY_RELEASED_EVENT )
     {
         dfbevent.type = DIET_KEYRELEASE;
     }
     else if ( evt->event_type == KEY_HOLD_EVENT )
     {
         dfbevent.type = DIET_KEYPRESS;
     }

     /*
       specified the keyboard event data to send up to DirectFB
     */
     dfbevent.flags = DIEF_KEYCODE | DIEF_KEYID | DIEF_KEYSYMBOL ;
     
     dfbevent.key_code   = evt->inp.code;
     dfbevent.key_id     = map_key_code_to_dfb_key_id(evt->inp.code);
     dfbevent.key_symbol = map_key_code_to_dfb_key_symbol
                             (data, evt->inp.code);
     
     /*
        Additional (optional) event fields are valid
        We will also send keymodifier, keylock, and timestamp
        state information to DirectFB
     */
     dfbevent.flags = dfbevent.flags |
         DIEF_MODIFIERS | DIEF_LOCKS | DIEF_TIMESTAMP;

     dfbevent.timestamp = evt->inp.time;

     /*
       modifer mask.
     */
     dfbevent.modifiers = 0 ;
     
     if ( data->modkeys & SHIFT_KEY )
     {
         dfbevent.modifiers = dfbevent.modifiers | DIMM_SHIFT ;
     }
     if ( data->modkeys & CONTROL_KEY )
     {
         dfbevent.modifiers = dfbevent.modifiers | DIMM_CONTROL ;
     }
     if ( data->modkeys & ALT_KEY )
     {
         dfbevent.modifiers = dfbevent.modifiers | DIMM_ALT ;
     }
     if ( data->modkeys & ALTGR_KEY )
     {
         dfbevent.modifiers = dfbevent.modifiers | DIMM_ALTGR ;
     }
     if ( data->modkeys & META_KEY )
     {
         dfbevent.modifiers = dfbevent.modifiers | DIMM_META ;
     }
     if ( data->modkeys & SUPER_KEY )
     {
         dfbevent.modifiers = dfbevent.modifiers | DIMM_SUPER ;
     }
     if ( data->modkeys & HYPER_KEY )
     {
         dfbevent.modifiers = dfbevent.modifiers | DIMM_HYPER ;
     }
     
     /*
       keylocks state 
     */
     dfbevent.locks = 0 ;
     if ( data->lockkeys & SCROLL_KEY )
     {
         dfbevent.locks = dfbevent.locks | DILS_SCROLL ;
     }
     if ( data->lockkeys & NUM_KEY )
     {
         dfbevent.locks = dfbevent.locks | DILS_NUM ;
     }
     if ( data->lockkeys & CAPS_KEY )
     {
         dfbevent.locks = dfbevent.locks | DILS_CAPS ;
     }
          
     dfb_input_dispatch( data->device, &dfbevent );

     return;
}


/*----------------------------------------------------------------------
 * Name: analyz_raw_key_event
 *
 * Description: Analyze the raw key input.
 *
 * Input:
 *    data:   ref to data structure for a keyboard object.
 *    
 *
 * Output:
 *    
 *
 * Returns:
 *    void
------------------------------------------------------------------------*/
static int analyz_raw_key_event
(
    KeyboardObj         *data,
    struct input_event  *event,
    KeyboardEvent       *evt
)
{
    DBG(("{analyz_raw_key_event} enter....\n"));
    /* Interpret the raw key information. */
    if ( event->value == 0 )
    {
        evt->event_type = KEY_RELEASED_EVENT ;
    }
    else if ( event->value == 1 )
    {
        evt->event_type = KEY_PRESSED_EVENT ;
    }
    else if ( event->value == 2 )
    {
        evt->event_type = KEY_HOLD_EVENT ;
    }
    else
    {
        return -1;
    }

    /* analyze the modifier keys */
    if ( (evt->event_type == KEY_PRESSED_EVENT) ||
         (evt->event_type == KEY_HOLD_EVENT)
        )
    {
        switch ( event->code )
        {
            case KEY_LEFTSHIFT:
            case KEY_RIGHTSHIFT:
            {
                data->modkeys = data->modkeys | SHIFT_KEY ;
            }
            break;

            case KEY_LEFTCTRL:
            case KEY_RIGHTCTRL:
            {
                data->modkeys = data->modkeys | CONTROL_KEY;
            }
            break;

            case KEY_LEFTALT:
            case KEY_RIGHTALT:
            {
                data->modkeys = data->modkeys | ALT_KEY;
            }
            break;

            case KEY_LEFTMETA:
            case KEY_RIGHTMETA:
            {
                data->modkeys = data->modkeys | META_KEY;
            }
            break;

            default:
            {
            }
            break;
        }
    }
    else if ( evt->event_type == KEY_RELEASED_EVENT )
    {
        
#define UNSET_MODKEY(_mkey_,_which_mkey_)              \
        if ( (_mkey_) & (_which_mkey_) ) {            \
           (_mkey_) = ((_mkey_) ^ (_which_mkey_)) ;    \
        }
        
        switch ( event->code )
        {
            case KEY_LEFTSHIFT:
            case KEY_RIGHTSHIFT:
            {
                UNSET_MODKEY(data->modkeys,SHIFT_KEY);
            }
            break;

            case KEY_LEFTCTRL:
            case KEY_RIGHTCTRL:
            {
                UNSET_MODKEY(data->modkeys,CONTROL_KEY);
            }
            break;

            case KEY_LEFTALT:
            case KEY_RIGHTALT:
            {
                UNSET_MODKEY(data->modkeys,ALT_KEY);
            }
            break;

            case KEY_LEFTMETA:
            case KEY_RIGHTMETA:
            {
                UNSET_MODKEY(data->modkeys,META_KEY);
            }
            break;

            default:
            {
            }
            break;
        }
    }   

    /* analyze the lock keys:  lockkeys are 'sticky', so
       the logic is different than modkeys.

       The 'sticky' behaviors are:
       
       If not in active state, when press, the lockkeys immediately
       change to active state.

       If in active state, when press the lockkeys remains in
       active state until release, then it become un-active.
       
    */
    if ( (evt->event_type == KEY_PRESSED_EVENT) )
    {
        /*  If in un-active state, set active state, eles do nothing. */
#define SET_LOCKKEY(_mkey_,_which_mkey_)              \
        if ( ! ((_mkey_) & (_which_mkey_)) ) {       \
           (_mkey_) = ((_mkey_) | (_which_mkey_)) ;    \
        }

        /*  record the lock key state before the key is pressed. */
        data->o_lockkeys = data->lockkeys;
                
        switch ( event->code )
        {
            case KEY_SCROLLLOCK:
            {
                SET_LOCKKEY(data->lockkeys,SCROLL_KEY);                
 
            }
            break;

            case KEY_NUMLOCK:
            {
                SET_LOCKKEY(data->lockkeys,NUM_KEY);
            }
            break;

            case KEY_CAPSLOCK:
            {
                SET_LOCKKEY(data->lockkeys,CAPS_KEY);
            }
            break;

            default:
            {
            }
            break;
        }
    }
    else if ( evt->event_type == KEY_RELEASED_EVENT )
    {
        /* If in old lockey state (prior KEY_PRESS)
           is in an active state, then we make it un-active.
        */
#define UNSET_LOCKKEY(_mkey_,_o_mkey_,_which_mkey_)   \
        if ( ((_o_mkey_) & (_which_mkey_)) ) {       \
           (_mkey_) = ((_mkey_) ^ (_which_mkey_)) ;    \
        }        

        switch ( event->code )
        {
            case KEY_SCROLLLOCK:
            {
                UNSET_LOCKKEY(data->lockkeys,data->o_lockkeys,SCROLL_KEY); 
            }
            break;

            case KEY_NUMLOCK:
            {
                UNSET_LOCKKEY(data->lockkeys,data->o_lockkeys,NUM_KEY);
            }
            break;

            case KEY_CAPSLOCK:
            {
                UNSET_LOCKKEY(data->lockkeys,data->o_lockkeys,CAPS_KEY);
            }
            break;

            default:
            {
            }
            break;
        }
    }
    else if ( evt->event_type == KEY_HOLD_EVENT  )
    {
        /* stay in the current lock state, no changed. */
        
    }
    
    /* copy  the raw key event to internal keyboard
       event structure.
    */
    evt->inp = *event;

    DBG(("{analyz_raw_key_event} complete \n"));
    return 1 ;
}

/*----------------------------------------------------------------------
 * Name: keyboard_read_event
 *
 * Description: Read and parse the keyboard event from keyboard device.
 *
 * Input:
 *    data:   ref to data structure for a keyboard object.
 *    
 *
 * Output:
 *    
 *
 * Returns:
 *    void
------------------------------------------------------------------------*/
static void keyboard_read_event(KeyboardObj *data)
{
    int                 n;
    KeyboardEvent       evt;
    struct input_event  event;
    size_t              byte_to_read;
    
    DBG(("{keyboard_read_event} enter....\n"));

    byte_to_read = sizeof(struct input_event);

    n = read(data->kybfd, &event, byte_to_read);
    if (n != byte_to_read )
    {
        printf("{keyboard_read_event}  no keyboard input data...\n"); 
        return ;
    }
    else
    {
        if ( event.type != EV_KEY )
        {
            /* printf("event type is not Key: %d\n",event.type);  */
            return ;
        }
        /*
        printf("{keyboard_read_event}  read %d bytes from keyboard input...\n",
               n);
        */
    }

    /*
       code  :  key_a, key_b, etc.

       value :  press:   1
                release: 0
                hold:    2
                
       event.time:  second
                    micro-seconds
    */
    /*
    printf("code= %d value= %d sec: %ld  usec: %ld\n",
           event.code,
           event.value,
           (long int)event.time.tv_sec,
           (long int)event.time.tv_usec);
    */

    if ( analyz_raw_key_event(data, &event, &evt) < 0 )
    {
        return;
    }

    send_keyboard_event(data,&evt);
    
    return;
}


/*----------------------------------------------------------------------
 * Name:  keyboard_event_input_thread
 *
 * Description: This function is executed in context of DirectFB
 *    event thread, it reads the keyboard event from keyboard device
 *    and dispatch the keyboard events to DirectFB.
 *
 * Input:
 *    thread: Ref to DirectFB thread.
 *    data:   ref to data structure for a keyboard object.
 *
 * Output:
 *    none.
 *
 * Returns:
 *    void
------------------------------------------------------------------------*/
static void* keyboard_event_input_thread( DirectThread *thread, void *arg )
{
    KeyboardObj *data = arg;

    printf("{USB Keyboard} input thread started....\n");
    
    while ( ! data->quit_thread ) 
    {
        /* Read keyboard event and dispatch the event to DirectFB.
        */
        keyboard_read_event(data);
    }

    printf("{USB Keyboard} input thread terminated.\n");
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
                DFB_INPUT_DRIVER_INFO_NAME_LENGTH, "USB Keyboard Driver" );
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
     KeyboardObj *data = NULL;

     /* fill in device info */
     snprintf( info->desc.name, DFB_INPUT_DEVICE_DESC_NAME_LENGTH, "MediaTek USB Keyboard Input" );
     snprintf( info->desc.vendor, DFB_INPUT_DEVICE_DESC_VENDOR_LENGTH, "MediaTek" );

     info->prefered_id = DIDID_KEYBOARD;
     info->desc.caps   = DICAPS_KEYS ;
     info->desc.type   = DIDTF_KEYBOARD;

     /* allocate and fill private data */
     data = D_CALLOC( 1, sizeof(KeyboardObj) );
     if (!data)
     {
         return D_OOM();
     }

     keyboard_reset(data);

     if ( keyboard_open_device(data) >= 0 )
     {
         data->device = device;
         data->quit_thread = false;
         data->thread = direct_thread_create( DTT_INPUT,
                                              keyboard_event_input_thread,
                                              data,"USB_Keyboard" );

         /* get device name */
         ioctl( data->kybfd,
                EVIOCGNAME(DFB_INPUT_DEVICE_DESC_NAME_LENGTH - 1),
                info->desc.name );

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
     KeyboardObj *data = driver_data;

     data->quit_thread = true;

     direct_thread_join( data->thread );
     direct_thread_destroy( data->thread );

     close(data->kybfd);

     /* free private data */
     D_FREE( data );
}

