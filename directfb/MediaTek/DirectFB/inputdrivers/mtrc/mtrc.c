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

#include <sys/types.h>
#include <linux/unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/resource.h>

#include <drv_common.h>
#include <mtal.h>
#include <mtal_ioctl.h>
#include <mtperipheral.h>
//#include "cb_if.h"

#ifdef FACTORYMODE
#include <sys/types.h>
#include <sys/stat.h>
#endif

#define __sys2(x) #x
#define __sys1(x) __sys2(x)

#ifndef __syscall
#if defined(__thumb__) || defined(__ARM_EABI__)
#define __SYS_REG(name) register long __sysreg __asm__("r7") = __NR_##name;
#define __SYS_REG_LIST(regs...) "r" (__sysreg) , ##regs
#define __syscall(name) "swi\t0"
#else
#define __SYS_REG(name)
#define __SYS_REG_LIST(regs...) regs
#define __syscall(name) "swi\t" __sys1(__NR_##name) ""
#endif
#endif

#define __syscall_return(type, res)                 \
do {                                    \
    if ((unsigned long)(res) >= (unsigned long)(-129)) {        \
        res = -1;                       \
    }                               \
    return (type) (res);                        \
} while (0)

#define _syscall0(type,name)                        \
type name(void) {                           \
  __SYS_REG(name)                           \
  register long __res_r0 __asm__("r0");                 \
  long __res;                               \
  __asm__ __volatile__ (                        \
  __syscall(name)                           \
    : "=r" (__res_r0)                       \
    : __SYS_REG_LIST()                      \
    : "memory" );                           \
  __res = __res_r0;                         \
  __syscall_return(type,__res);                     \
}

_syscall0(pid_t,gettid)

#define printf_display(x,format,arg...) { if(x) printf(format,arg);}
// #define CC_ENABLE_DFB_BTN_REPEAT
#define DFB_BTN_REPEAT_FREQUENCY (500)

#define BTN_NONE                ((u32)0xffffffff)
#define DFB_RAW_KEY             DIKS_CUSTOM31

#define DEV_MTAL                        "mtal"
#ifdef FACTORYMODE
#define FIFOSNAMEL                      "MtrcToIom"
#endif
/*
typedef struct
{
    int ai4Arg[2];
} MTAL_IOCTL_2ARG_T;
#define MTAL_IO_MTPERIPHERAL_IR_INIT                       _IOW(11, 35, int)
#define MTAL_IO_MTPERIPHERAL_IR_POLL                     _IOW(11, 36, MTAL_IOCTL_2ARG_T)
#define MTIR_IRRX_MAX_RAW_DATA_SIZE                  ((u8) 11) 
typedef struct 
{
    u8     u1Protocol;
    u32             u4Len; 
    u8              au1Data[MTIR_IRRX_MAX_RAW_DATA_SIZE];
}   MTIR_IRRX_RAW_DATA_T;
int mtal_ioctl_result(const char * funcname,int x);
#define mtal_ioctl(x...)    mtal_ioctl_result(__FUNCTION__,ioctl(x));
*/
DFB_INPUT_DRIVER( rc_input )

//D_DEBUG_DOMAIN( RCInput, "RC/Input", "RC System Input" );

static    struct sched_param rParam;

/*
 * declaration of private data
 */
typedef struct {
     CoreInputDevice         *device;

     DirectThread            *thread;
     bool                     quit_thread;

     DFBInputDeviceKeySymbol  last_symbol;
     long long                last_time;
} RCInputData;


int mtal_ioctl_result(const char * funcname,int x)
{
    if(x<0)
    {
        x=-errno;
              #if MTAL_IOCTL_DEBUG
        printf("%s() fail, ret=%d\n",funcname,x);
              #else
              (void)funcname;
              #endif
    }
    return (int)x;
}

static void
send_symbol( RCInputData            *data,
             DFBInputDeviceKeySymbol  symbol,
             int key_code,
             bool                     down )
{
     DFBInputEvent event;

     if (symbol == BTN_NONE)
          return;

     event.type       = down ? DIET_KEYPRESS : DIET_KEYRELEASE;
     event.flags      = DIEF_KEYSYMBOL | DIEF_KEYCODE;
     event.key_symbol = symbol;

     // OR with 0x03000000 to indicate this is RC6     
     //event.key_code = key_code | 0x03000000;
     event.key_code = key_code;
     printf("BTN_%s: 0x%x policy: %d prio %d\n", (down?"DOWN":"UP"), event.key_code, 
        sched_getscheduler(gettid()), (int)rParam.sched_priority);

     dfb_input_dispatch( data->device, &event );
}

static u32
translate_KeySymbol( u32 code )
{
    if((code >= BTN_KB_NULL) && (code <= BTN_KB_DELETE))
        return (code - BTN_KB_NULL) + DIKS_NULL;
    else if((code >= BTN_KB_CURSOR_LEFT) && (code <= BTN_KB_PAUSE))
        return (code - BTN_KB_CURSOR_LEFT) + DIKS_CURSOR_LEFT;
    else if((code >= BTN_KB_F1) && (code <= BTN_KB_F12))
        return (code - BTN_KB_F1) + DIKS_F1;
    else if((code >= BTN_KB_CAPS_LOCK) && (code <= BTN_KB_SCROLL_LOCK))
        return BTN_KB_NULL;//return (code - BTN_KB_CAPS_LOCK) + DIKS_CAPS_LOCK;
    else if((code >= BTN_KB_SHIFT) && (code <= BTN_KB_HYPER))
        return BTN_KB_NULL;//return (code - BTN_KB_SHIFT) + DIKS_SHIFT;
    else    
        return BTN_KB_NULL;
}
/*
static u32 u4DownCnt = 0;
static u32 u4UpCnt = 0;
*/
static void *
input_thread( DirectThread *thread, void *arg )
{
    RCInputData *data = arg;
        
    u32 raw;
    u32 system;    
    u32 command = 0;
    u32 toggle = 0;
    u32 last_toggle = 0;
    u32 prev_command = 0xffffffff;
    bool nec_ir;

#ifdef CC_ENABLE_DFB_BTN_REPEAT
    long long prev_event_time = 0;
#endif

    //DFBInputDeviceKeySymbol symbol;

    char szDevice[10];
    static int _i4DeviceHandle;
    static bool  IsDebugOpen = FALSE;
    int i4Ret;
    #ifdef FACTORYMODE
    int fd;
    if((mkfifo(FIFOSNAMEL,O_CREAT|O_EXCL)<0) &&(errno!=EEXIST));
        { 
            printf("[mtrc]cannot creat fifos\n");
        }
    fd = open(FIFOSNAMEL,O_RDWR | O_NONBLOCK);
    if(fd==-1)
        {
            printf("[mtrc]open error\n");
        }
    #endif

    printf("[DirectFB]: Wait to Init IR...!\n");
    usleep( 2000000 );    
    sprintf(szDevice, "/dev/%s", DEV_MTAL);
    
    if ((_i4DeviceHandle = open(szDevice, O_RDWR)) < 0)
    {
        printf("[DirectFB]: Cannot open device %s!\n", szDevice);
        return NULL;
    }   

    i4Ret = mtal_ioctl(_i4DeviceHandle, MTAL_IO_MTPERIPHERAL_IR_INIT, MTIR_TYPE_DEFAULT);
    if(i4Ret)
    {
        printf("[DirectFB]: Fail to init MTK IR!\n");
        return NULL;    
    }    

    fprintf (stderr,"input thread pid is %d tid is %d\n", (int) getpid(), (int) gettid ());
    fprintf(stderr, "original scheduler policy %d\n", sched_getscheduler(gettid()));
    fprintf(stderr, "scheduler nice %d\n", getpriority(PRIO_PROCESS, gettid()));
    
    i4Ret = sched_getparam(gettid(), &rParam);
    if(i4Ret)
    {
        fprintf(stderr, "[DirectFB]: Fail to getschedule param!\n");
        return NULL;
    }    
    fprintf(stderr, "original child scheduler policy %d prio %d\n", sched_getscheduler(gettid()), (int)rParam.sched_priority);

    rParam.sched_priority = sched_get_priority_max(SCHED_RR);
    i4Ret = sched_setscheduler(gettid(), SCHED_RR, &rParam);
    if(i4Ret)
    {
        fprintf(stderr, "[DirectFB]: Fail to set scheduler!\n");
        return NULL;
    }      
    fprintf(stderr, "new child scheduler policy %d prio %d\n", sched_getscheduler(gettid()), (int)rParam.sched_priority);

    printf("Start to wait IR event...\n");
    while (!data->quit_thread) 
    {
        DFBInputEvent event;
        u32 u4Key, u4BtnState;
        MTIR_IRRX_RAW_DATA_T rRaw;
        MTAL_IOCTL_3ARG_T rArg;
        rArg.ai4Arg[0] = (int)&u4Key;
        rArg.ai4Arg[1] = (int)&u4BtnState;        
        rArg.ai4Arg[2] = (int)&rRaw;
        i4Ret = mtal_ioctl(_i4DeviceHandle, MTAL_IO_MTPERIPHERAL_IR_WAIT, &rArg);    
        
        if(i4Ret)
        {
            printf("[DirectFB]: Fail to poll MTK IR!\n");
            return NULL;
        }
         if (u4Key == BTN_DEBUG_ENABLE)
        {
            if(rRaw.au1Data[0]&0x01)
                {
                    IsDebugOpen=TRUE;
                }
            else                        
                {
                    IsDebugOpen=FALSE;
                }
          continue;//return;
        }

         if (u4Key == BTN_NONE)
        {
            //printf("[DirectFB]: BTN_NONE..discard\n");
            usleep( 10000 );                  
          continue;//return;
        }
#ifdef CC_JOYSTICK_IR
         if (u4Key == BTN_IR_MOUSE_MOVE)
             {
             if (u4BtnState)
                {
                     if(((u32)rRaw.au1Data[1]))//au1Data[1] should be placed x offset at IRRX layer
                     {
                      event.type = DIET_AXISMOTION;
                      /* which additional (optional) event fields are valid. */
                      event.flags = DIEF_AXISREL;
          
                       /* Pedro: Why do we have to  send it  twice ?  */
                       event.axis    = DIAI_X;
                       if(((u32)rRaw.au1Data[2]))
                         {event.flags   |=  DIEF_FOLLOW; }                     
                       event.axisrel = ((int)((INT8)(rRaw.au1Data[1])));
                       printf_display(IsDebugOpen,"[mtrc]IR_MOUSE_Info : pos mouse dx: 0x%02x\n",((int)((INT8)(rRaw.au1Data[1])))); 
                       dfb_input_dispatch( data->device, &event );
                     }
                     if(((u32)rRaw.au1Data[2]))//au1Data[2] should be placed y offset at IRRX layer
                     {                            
                       event.type    = DIET_AXISMOTION;
                       event.flags   = DIEF_AXISREL;
                       event.axis    = DIAI_Y;
                       event.axisrel = ((int)((INT8)(rRaw.au1Data[2])));
                       printf_display(IsDebugOpen,"[mtrc]IR_MOUSE_Info : pos mouse dy: 0x%02x\n",((int)((INT8)(rRaw.au1Data[2]))));
                       dfb_input_dispatch( data->device, &event ); 
                     }
                     if(((u32)rRaw.au1Data[3]))//au1Data[3] should be placed z offset at IRRX layer
                     {
                       event.type    = DIET_AXISMOTION;
                       event.flags   = DIEF_AXISREL;
                       event.axis    = DIAI_Z;
                       event.axisrel = ((int)((INT8)(rRaw.au1Data[3])));
                       printf_display(IsDebugOpen,"IR_MOUSE_Info:pos mouse dz: 0x%02x\n",((int)((INT8)(rRaw.au1Data[3])))); 
                       dfb_input_dispatch( data->device, &event ); 
                     }
                }
               }
         else if ((u4Key & 0xffff0000)==KEY_GROUP_IR_MOUSE)
             {
                 event.type = (u4BtnState ? DIET_BUTTONPRESS : DIET_BUTTONRELEASE);     
                 /* which additional (optional) event fields are valid. */
                 event.flags = DIEF_BUTTONS | DIEF_FOLLOW ;
                 if(u4Key == BTN_IR_MOUSE_LEFT)//left    
                    {event.button = DIBI_LEFT;}
                     else if(u4Key == BTN_IR_MOUSE_RIGHT)//right
                        {event.button = DIBI_RIGHT;}                         
                event.buttons = event.button;
                 printf_display(IsDebugOpen,"IR_MOUSE_Info: event.button: 0x%02x\n",event.button);
                dfb_input_dispatch( data->device, &event );
             }
        else if ((u4Key & 0xffff0000) == KEY_GROUP_KEYBOARD)
        {
            event.type = u4BtnState ? DIET_KEYPRESS : DIET_KEYRELEASE; 
            event.flags = DIEF_KEYSYMBOL | DIEF_KEYCODE;
            event.key_symbol = translate_KeySymbol(u4Key);
            event.key_code = ((u32)rRaw.au1Data[0]) | ((u32)rRaw.au1Data[1])<< 8 | ((u32)rRaw.au1Data[2])<< 16 | ((u32)rRaw.au1Data[3])<< 24;
#if 0//may not need to implement...
            event.flags |= DIEF_KEYID;
            event.key_id = translate_key(u4Key);
#endif
            printf_display(IsDebugOpen,"[mtrc]KB_%s: code 0x%8x, symbol 0x%8x\n", ((event.type == DIET_KEYPRESS)? "DOWN":"UP"), event.key_code, event.key_symbol);
            dfb_input_dispatch( data->device, &event );      
        }  
        else
        {
#endif 

     //event.type       = u4BtnState;
     event.type       = u4BtnState ? DIET_KEYPRESS : DIET_KEYRELEASE;     
     event.key_symbol = u4Key;
     
 if (event.key_symbol & KEY_SRC_FRONT_PANEL)
     { 
     event.flags       = DIEF_KEYSYMBOL;
     }
  else
     {
        if((rRaw.u1Protocol == MTIRRX_RAW_DATA_RC5)||(rRaw.u1Protocol == MTIRRX_RAW_DATA_RC6)||(rRaw.u1Protocol == MTIRRX_RAW_DATA_NEC))
            {
                event.flags = DIEF_KEYSYMBOL | DIEF_KEYCODE;
            }
            else
            {
                event.flags = DIEF_KEYSYMBOL;
            }

     // OR with 0x03000000 to indicate this is RC6     
     //event.key_code = key_code | 0x03000000;
     raw = ((u32)rRaw.au1Data[0]) | ((u32)rRaw.au1Data[1])<<8 | ((u32)rRaw.au1Data[2])<<16 | ((u32)rRaw.au1Data[3])<<24;     
     if((rRaw.u1Protocol == MTIRRX_RAW_DATA_RC5) || (rRaw.u1Protocol == MTIRRX_RAW_DATA_RC6))//if(rRaw.u1Protocol == MTIR_TYPE_RC56)
     {
        event.key_code = ((raw >> 15) & 0xff);
        event.key_code = (event.key_code) | 0x03000000;
     }
     #ifdef FACTORYMODE
     else if(rRaw.u1Protocol == MTIRRX_RAW_DATA_FACTORY)
        {
            write(fd,&rRaw,(sizeof(u32)*3));
            if(fd==-1)
                {
                    printf("[mtrc]write to fifo error\n");

                }
        }
     #endif
     else
     {
        event.key_code = ((raw >> 16) & 0x00FF);//temporality...//key_code;        
     }
    }
     printf_display(IsDebugOpen,"[mtrc]BTN_%s: code 0x%8x, symbol 0x%8x\n", ((event.type == DIET_KEYPRESS)? "DOWN":"UP"), event.key_code, event.key_symbol);
/*
        if(event.type == DIET_KEYPRESS)
        {
            u4DownCnt++;
        }
        if(event.type == DIET_KEYRELEASE)
        {
            u4UpCnt++;
            if(u4UpCnt != u4DownCnt)
            {
                printf("mtrc error: down is not equal! Up %d, Down %d\n",u4UpCnt, u4DownCnt);
#if 0
                u4UpCnt--;
                usleep( 10000 );
                continue;
#else                
                printf("InputThread hang for ICE debug!!\n");
                while(u4UpCnt);
#endif                
            }
        }
*/        
     dfb_input_dispatch( data->device, &event ); 
     usleep( 10000 );
    }
#ifdef CC_JOYSTICK_IR
    }
#endif
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
                DFB_INPUT_DRIVER_INFO_NAME_LENGTH, "RC Input Driver" );
     snprintf ( info->vendor,
                DFB_INPUT_DRIVER_INFO_VENDOR_LENGTH, "Denis Oliver Kropp" );

     info->version.major = 0;
     info->version.minor = 1;
}

static
INT32 _IrDispatch(void* pvArg)
{
    MTGPIO_CB_T* prGpioCbArg;    
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
     RCInputData *data;

     /* fill info */
     snprintf( info->desc.name, DFB_INPUT_DEVICE_DESC_NAME_LENGTH, "MediaTek RC Input" );
     snprintf( info->desc.vendor, DFB_INPUT_DEVICE_DESC_VENDOR_LENGTH, "MediaTek" );

     info->desc.caps = DICAPS_KEYS;
     info->desc.type = DIDTF_REMOTE;

     /* allocate and fill private data */
     data = D_CALLOC( 1, sizeof(RCInputData) );
     if (!data)
          return D_OOM();

     data->device = device;

     fprintf (stderr,"input thread's parent pid is %d tid is %d\n", (int) getpid(), (int) gettid());
//    printf("[DirectFB]: Wait sometime to create IR...!\n");
//    usleep( 2000000 );    

     data->thread = direct_thread_create( DTT_INPUT, input_thread, data, "RC Input" );     
   
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
     RCInputData *data = driver_data;

     data->quit_thread = true;

     direct_thread_join( data->thread );
     direct_thread_destroy( data->thread );

     /* free private data */
     D_FREE( data );
}
