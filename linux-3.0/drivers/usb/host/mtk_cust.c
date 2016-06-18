/*-----------------------------------------------------------------------------
 *\drivers\usb\host\mtk_cust.c
 *
 * MT53xx USB driver
 *
 * Copyright (c) 2008-2012 MediaTek Inc.
 * $Author: dtvbm11 $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 *---------------------------------------------------------------------------*/


/** @file mtk_cust.c
 *  This C file implements the mtk53xx USB host controller customization driver.
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/usb.h>
#include <linux/kthread.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/unaligned.h>
#include <asm/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/usb/hcd.h>
#ifdef CONFIG_MT53XX_NATIVE_GPIO
#include <mach/mtk_gpio.h>
#endif
#include "mtk_hcd.h"
#include "mtk_qmu_api.h"

#ifdef MUSB_DEBUG
extern int mgc_debuglevel;
#endif

#ifndef CONFIG_USB_OC_SUPPORT
//#define CONFIG_USB_OC_SUPPORT  //add this define to enable  Over current function 
#endif

#define MTK_USB_TEST_MAJOR 244
#define DEVICE_NAME "usb_cust"


#define MUT_MAGIC 'MUC'
#define MUT_IOCTL_GET _IOR(MUT_MAGIC, 0, int)
#define MUT_IOCTL_SET _IOW(MUT_MAGIC, 1, int)
#define MUT_IOCTL_OC  0x58000000

#ifndef DEMO_BOARD
#ifdef CONFIG_USB_OC_SUPPORT
static uint8_t u1MUSBVbusEanble = FALSE;   // USB Vbus status, true = Vbus On / false = Vbus off
static uint8_t u1MUSBOCEnable = FALSE;      // USB OC function enable status, true= enable oc detect /false = not
static uint8_t u1MUSBOCStatus = FALSE;      // USB OC status, true = oc o / false=oc not cours
#endif
#endif
//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------
//WIFI Port pull down the DP for more than 200ms after USB HW reset
#ifdef SUPPORT_DP_PULL_DOWN
#define LGE_WIFI_PORT_NUM     2
#endif
//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------
#define MUC_NUM_PLATFORM_DEV (sizeof(MUC_aLinuxController) / sizeof(MUSB_LinuxController))

#define MUC_NUM_MAX_CONTROLLER      (4)
//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------
static int MGC_PhyReset(void * pBase, void * pPhyBase)
{
    uint32_t u4Reg = 0;
    uint8_t nDevCtl = 0;

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368) || defined(CONFIG_ARCH_MT5398)
	uint32_t *pu4MemTmp;
	// USB DMA enable

	pu4MemTmp = (uint32_t *)0xf0060200;
	*pu4MemTmp = 0x00cfff00; 

#elif defined(CONFIG_ARCH_MT5880) ||  defined(CONFIG_ARCH_MT5881)
	uint32_t *pu4MemTmp;

	// USB DMA enable
	pu4MemTmp = (uint32_t *)0xf0061300;
	*pu4MemTmp = 0x00cfff00;
#endif

    // VRT insert R enable 
    u4Reg = MGC_PHY_Read32(pPhyBase, 0x00);
    u4Reg |= 0x4000;
    MGC_PHY_Write32(pPhyBase, 0x00, u4Reg);

    //Soft Reset, RG_RESET for Soft RESET
    u4Reg = MGC_PHY_Read32(pPhyBase,(uint32_t)0x68);
    u4Reg |=   0x00004000; 
    MGC_PHY_Write32(pPhyBase, (uint32_t)0x68, u4Reg);
    u4Reg &=  ~0x00004000; 
    MGC_PHY_Write32(pPhyBase, (uint32_t)0x68, u4Reg);

	#ifdef SUPPORT_DP_PULL_DOWN
    if(pPhyBase != MUC_aLinuxController[LGE_WIFI_PORT_NUM].pPhyBase)
	#endif
    {
	    //otg bit setting
	    u4Reg = MGC_PHY_Read32(pPhyBase,(uint32_t)0x6C);
	    u4Reg &= ~0x3f3f;
	    u4Reg |=  0x3f2c;
	    MGC_PHY_Write32(pPhyBase, (uint32_t)0x6C, u4Reg);
    }
    //suspendom control
    u4Reg = MGC_PHY_Read32(pPhyBase,(uint32_t)0x68);
    u4Reg &=  ~0x00040000; 
    MGC_PHY_Write32(pPhyBase, (uint32_t)0x68, u4Reg);
    
    //eye diagram improve   
    //TX_Current_EN setting 01 , 25:24 = 01 //enable TX current when
    //EN_HS_TX_I=1 or EN_HS_TERM=1
    u4Reg = MGC_PHY_Read32(pPhyBase,(uint32_t)0x1C);
    u4Reg |=  0x01000000; 
    MGC_PHY_Write32(pPhyBase, (uint32_t)0x1C, u4Reg);

    //disconnect threshold,set 615mV
    u4Reg = MGC_PHY_Read32(pPhyBase,(uint32_t)0x18);
    u4Reg |=  0x00060000; 
    MGC_PHY_Write32(pPhyBase, (uint32_t)0x18, u4Reg);
	
    u4Reg = MGC_Read8(pBase,M_REG_PERFORMANCE3);
    u4Reg |=  0x80;
    u4Reg &= ~0x40;
    MGC_Write8(pBase, M_REG_PERFORMANCE3, (uint8_t)u4Reg);

    // MT5368/MT5396/MT5389 
    //0xf0050070[10]= 1 for Fs Hub + Ls Device 
    u4Reg = MGC_Read8(pBase, (uint32_t)0x71);
    u4Reg |= 0x04;
    MGC_Write8(pBase, 0x71, (uint8_t)u4Reg);

    // open session.
    nDevCtl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);     
    nDevCtl |= MGC_M_DEVCTL_SESSION;        
    MGC_Write8(pBase, MGC_O_HDRC_DEVCTL, nDevCtl);

    // This is important: TM1_EN
    // speed up OTG setting for checking device connect event after MUC_Initial().
    u4Reg = MGC_Read32(pBase, 0x604);
    u4Reg |= 0x01;
    MGC_Write32(pBase, 0x604, u4Reg);
	
    // open session.
    nDevCtl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);   
    nDevCtl |= MGC_M_DEVCTL_SESSION;        
    MGC_Write8(pBase, MGC_O_HDRC_DEVCTL, nDevCtl);

    // FS/LS Slew Rate change
    u4Reg = MGC_PHY_Read32(pPhyBase, 0x10);
    u4Reg &= (uint32_t)(~0x000000ff);
    u4Reg |= (uint32_t)0x00000011;
    MGC_PHY_Write32(pPhyBase, 0x10, u4Reg);

    // HS Slew Rate
    u4Reg = MGC_PHY_Read32(pPhyBase, 0x10);
    u4Reg &= (uint32_t)(~0x00070000);
    u4Reg |= (uint32_t)0x00040000;
    MGC_PHY_Write32(pPhyBase, 0x10, u4Reg);

    // This is important: TM1_EN
    // speed up OTG setting for checking device connect event after MUC_Initial().
    u4Reg = MGC_Read32(pBase, 0x604);
    u4Reg &= ~0x01;
    MGC_Write32(pBase,0x604, u4Reg);

	#ifdef SUPPORT_DP_PULL_DOWN
	if(pPhyBase == MUC_aLinuxController[LGE_WIFI_PORT_NUM].pPhyBase)
    {
		// delay 200ms to enable host mode. add more than 100ms to waiting USB reset finish.        
		msleep(300);
		//otg bit setting
		u4Reg = MGC_PHY_Read32(pPhyBase,(uint32_t)0x6C);
		u4Reg &= ~0x3f3f;
		u4Reg |=  0x3f2c;
		MGC_PHY_Write32(pPhyBase, (uint32_t)0x6C, u4Reg);
    }
	#endif
    return 0;
}

#ifdef CONFIG_MT53XX_NATIVE_GPIO
static int MUC_aPortUsing[MUC_NUM_MAX_CONTROLLER] = {0};
static int MUC_aPwrGpio[MUC_NUM_MAX_CONTROLLER]= {0} ;
static int MUC_aPwrPolarity[MUC_NUM_MAX_CONTROLLER] = {0};
static int MUC_aOCGpio[MUC_NUM_MAX_CONTROLLER] = {0} ;
static int MUC_aOCPolarity[MUC_NUM_MAX_CONTROLLER] = {0};
static int MUC_aCdcSupport[MUC_NUM_MAX_CONTROLLER] = {0};

static int __init MGC_CommonParamParsing(char *str, int *pi4Dist)
{
    char tmp[8]={0};
    char *p,*s;
    int len,i,j;
    if(strlen(str) != 0)  {      
        DBG(-2, "Parsing String = %s \n",str);
    }
    else{
        DBG(-2, "Parse Error!!!!! string = NULL\n");
        return 0;
    }

    for (i=0; i<MUC_NUM_MAX_CONTROLLER; i++){
        s=str;
        if (i  !=  (MUC_NUM_MAX_CONTROLLER-1) ){
            if(!(p=strchr (str, ','))){
                DBG(-2, "Parse Error!!string format error 1\n");
                break;
            }
            if (!((len=p-s) >= 1)){
                DBG(-2, "Parse Error!! string format error 2\n");
                break;
            }
        }else {
            len = strlen(s);
        }
        
        for(j=0;j<len;j++){
            tmp[j]=*s;
            s++;
        }
        tmp[j]=0;

        pi4Dist[i] = (int)simple_strtol(&tmp[0], NULL, 10);
        DBG(-2, "Parse Done[%d]=%d\n", i, pi4Dist[i]);
        
        str += (len+1);
    }
    
    return 1;
}

static int __init MGC_PortUsingParseSetup(char *str)
{
	DBG(-2, "USB port Using = .\n");
    return MGC_CommonParamParsing(str,&MUC_aPortUsing[0]);
}

static int __init MGC_PwrGpioParseSetup(char *str)
{
	DBG(-2, "USB Power GPIO Port = .\n");
    return MGC_CommonParamParsing(str,&MUC_aPwrGpio[0]);
}

static int __init MGC_PwrPolarityParseSetup(char *str)
{
	DBG(-2, "USB Power Polarity = .\n");
    return MGC_CommonParamParsing(str,&MUC_aPwrPolarity[0]);
}

static int __init MGC_OCGpioParseSetup(char *str)
{
	DBG(-2, "USB OC GPIO Port = .\n");
    return MGC_CommonParamParsing(str,&MUC_aOCGpio[0]);
}

static int __init MGC_OCPolarityParseSetup(char *str)
{
	DBG(-2, "USB OC Polarity = .\n");
    return MGC_CommonParamParsing(str,&MUC_aOCPolarity[0]);
}

static int __init MGC_CdcSupportParseSetup(char *str)
{
    DBG(-2, "CdcSupport = %s\n",str);
    return MGC_CommonParamParsing(str,&MUC_aCdcSupport[0]);
}
__setup("usbportusing=", MGC_PortUsingParseSetup);
__setup("usbpwrgpio=", MGC_PwrGpioParseSetup);
__setup("usbpwrpolarity=", MGC_PwrPolarityParseSetup);
__setup("usbocgpio=", MGC_OCGpioParseSetup);
__setup("usbocpolarity=", MGC_OCPolarityParseSetup);
__setup("usbcdcsupport=", MGC_CdcSupportParseSetup);

#endif

#ifdef CONFIG_ARCH_MT5396

#ifdef CONFIG_MT53XX_NATIVE_GPIO
static void MGC_VBusControl(uint8_t bPortNum, uint8_t bOn)
{
    unsigned long flags=0;
#ifdef USB_DEVICE_MODE_SUPPORT
    bPortNum += 0x1;
#endif
    // usb power switch pin define in \target\mt5396\5396_driver\drv_cust\mtk\config\*.h
    //INFO("[USB%d]Vbus %s gpio[%d].", bPortNum, ((bOn) ? "On": "Off"),(MUC_aPortUsing[bPortNum] == TRUE)? (uint32_t *)MUC_aPwrGpio[bPortNum]: NULL);
    local_irq_save(flags);
    if(MUC_aPortUsing[bPortNum] == 1)
    {
        mtk_gpio_set_value(MUC_aPwrGpio[bPortNum], 
                ((bOn) ? (MUC_aPwrPolarity[bPortNum]) : (1-MUC_aPwrPolarity[bPortNum])));
    }
    local_irq_restore(flags);
}

#else  //CONFIG_MT53XX_NATIVE_GPIO

static void MGC_VBusControl(uint8_t bPortNum, uint8_t bOn)
{
#ifdef USB_PWR_SWITCH_GPIO_CUST
    printk("\n");
    INFO("[USB0]: 5V On, Set GPIO none.\n");
    INFO("[USB1]: 5V On, Set GPIO none.\n");
    INFO("[USB2]: 5V On, Set GPIO none.\n");
    INFO("[USB3]: 5V On, Set GPIO none.\n");
#else 
    unsigned long flags=0;
#ifdef USB_DEVICE_MODE_SUPPORT
		bPortNum += 0x1;
#endif

    INFO("[USB%d] Vbus %s.\n", bPortNum, ((bOn) ? "On": "Off"));

    local_irq_save(flags);

    if (bOn)
    {
        switch (bPortNum)
        {
            case 0:
                // GPIO 43            
                *((volatile uint32_t *)0xF000D524) |= 0x00000800;
                *((volatile uint32_t *)0xF000D504) |= 0x00000800;                            
                break;
            case 1:
                // GPIO 44            
                *((volatile uint32_t *)0xF000D524) |= 0x00001000;
                *((volatile uint32_t *)0xF000D504) |= 0x00001000;
                break;
            case 2:
                // OPTCTRL 10  
                *((volatile uint32_t *)0xF002807C) |= 0x00000400;
                *((volatile uint32_t *)0xF0028074) |= 0x00000400;
                break;
            case 3:
                // OPTCTRL 11
                *((volatile uint32_t *)0xF002807C) |= 0x00000800;
                *((volatile uint32_t *)0xF0028074) |= 0x00000800;
                break;
            default:
                break;
        }
    }
    else
    {
        switch (bPortNum)
        {
            case 0:
                // GPIO 43            
                *((volatile uint32_t *)0xF000D524) |= 0x00000800;
                *((volatile uint32_t *)0xF000D504) &= ~0x00000800;                            
                break;
            case 1:
                // GPIO 44            
                *((volatile uint32_t *)0xF000D524) |= 0x00001000;
                *((volatile uint32_t *)0xF000D504) &= ~0x00001000;
                break;
            case 2:
                // OPTCTRL 10  
                *((volatile uint32_t *)0xF002807C) |= 0x00000400;
                *((volatile uint32_t *)0xF0028074) &= ~0x00000400;
                break;
            case 3:
                // OPTCTRL 11
                *((volatile uint32_t *)0xF002807C) |= 0x00000800;
                *((volatile uint32_t *)0xF0028074) &= ~0x00000800;
                break;
            default:
                break;
        }
    }
            
    local_irq_restore(flags);            
          
    #endif 
}
#endif

static MUSB_LinuxController MUC_aLinuxController[] = 
{
#ifndef USB_DEVICE_MODE_SUPPORT   
    {
         /* Port 0 information. */
        .pBase = (void*)MUSB_BASE, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT0_PHYBASE)), 
        .dwIrq = VECTOR_USB0, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 6,   /* Endpoint Number : 5+1*/
        .bEndpoint_Rx_num = 6,   /* Endpoint Number : 5+1*/
        .bHub_support = FALSE,
        .MGC_pfVbusControl = MGC_VBusControl, 
        .MGC_pfPhyReset = MGC_PhyReset,          
    },
#endif    
    {
         /* Port 1 information. */
        .pBase = (void*)MUSB_BASE1, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT1_PHYBASE)), 
        .dwIrq = VECTOR_USB1, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 6,   /* Endpoint Number : 5+1*/
        .bEndpoint_Rx_num = 6,   /* Endpoint Number : 5+1*/
        .bHub_support = FALSE,
        .MGC_pfVbusControl = MGC_VBusControl,
        .MGC_pfPhyReset = MGC_PhyReset,        
    },
    {
         /* Port 2 information. */
        .pBase = (void*)MUSB_BASE2,
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT2_PHYBASE)),         
        .dwIrq = VECTOR_USB2, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 9,   /* Endpoint Number : 8+1*/
        .bEndpoint_Rx_num = 9,   /* Endpoint Number : 8+1*/
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl,
        .MGC_pfPhyReset = MGC_PhyReset,        
    },
    {
         /* Port 3 information. */
        .pBase = (void*)MUSB_BASE3, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT3_PHYBASE)),         
        .dwIrq = VECTOR_USB3, 
        .bSupportCmdQ = FALSE, 
        .bEndpoint_Tx_num = 9,   /* Endpoint Number : 8+1*/
        .bEndpoint_Rx_num = 9,   /* Endpoint Number : 8+1*/
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl,
        .MGC_pfPhyReset = MGC_PhyReset,        
    }
};

#elif defined(CONFIG_ARCH_MT5368)

static void MGC_VBusControl(uint8_t bPortNum, uint8_t bOn)
{
#ifdef USB_PWR_SWITCH_GPIO_CUST
    printk("\n");
    INFO("[USB0]: 5V On, Set GPIO none.\n");
    INFO("[USB1]: 5V On, Set GPIO none.\n");
#else 
    unsigned long flags=0;

    INFO("[USB%d] Vbus %s.\n", bPortNum, ((bOn) ? "On": "Off"));

    local_irq_save(flags);

    if (bOn)
    {
        switch (bPortNum)
        {
            case 0:               
                // OPTCTRL 0        
                *((volatile uint32_t *)0xf002807c) |= 0x00000001;
                *((volatile uint32_t *)0xf0028074) |= 0x00000001;
                break;
            case 1:
                // OPTCTRL 1        
                *((volatile uint32_t *)0xf002807c) |= 0x00000002;
                *((volatile uint32_t *)0xf0028074) |= 0x00000002;
                break;
            case 2:
                // OPTCTRL 2        
                *((volatile uint32_t *)0xf002807c) |= 0x00000004;
                *((volatile uint32_t *)0xf0028074) |= 0x00000004;
                break;
            case 3:
                // OPTCTRL 3        
                *((volatile uint32_t *)0xf002807c) |= 0x00000008;
                *((volatile uint32_t *)0xf0028074) |= 0x00000008;
                break;
            default:
                break;
        }
    }
    else
    {
        switch (bPortNum)
        {
            case 0:               
                // OPTCTRL 0        
                *((volatile uint32_t *)0xf002807c) |= 0x00000001;
                *((volatile uint32_t *)0xf0028074) &= ~0x00000001;
                break;
            case 1:
                // OPTCTRL 1        
                *((volatile uint32_t *)0xf002807c) |= 0x00000002;
                *((volatile uint32_t *)0xf0028074) &= ~0x00000002;
                break;
            case 2:
                // OPTCTRL 2        
                *((volatile uint32_t *)0xf002807c) |= 0x00000004;
                *((volatile uint32_t *)0xf0028074) &= ~0x00000004;
                break;
            case 3:
                // OPTCTRL 3        
                *((volatile uint32_t *)0xf002807c) |= 0x00000008;
                *((volatile uint32_t *)0xf0028074) &= ~0x00000008;
                break;
            default:
                break;
        }
    }
            
    local_irq_restore(flags);            
          
    #endif 
}

static MUSB_LinuxController MUC_aLinuxController[] = 
{
    {
         /* Port 0 information. */
        .pBase = (void*)MUSB_BASE, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT0_PHYBASE)), 
        .dwIrq = VECTOR_USB0, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 9,   /* Endpoint Number : 8+1*/
        .bEndpoint_Rx_num = 9,   /* Endpoint Number : 8+1*/
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl, 
        .MGC_pfPhyReset = MGC_PhyReset,          
    },
    {
         /* Port 1 information. */
        .pBase = (void*)MUSB_BASE1, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT1_PHYBASE)), 
        .dwIrq = VECTOR_USB1, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 9,   /* Endpoint Number : 8+1*/
        .bEndpoint_Rx_num = 9,   /* Endpoint Number : 8+1*/
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl,
        .MGC_pfPhyReset = MGC_PhyReset,        
    }
};

#elif defined(CONFIG_ARCH_MT5389)

static void MGC_VBusControl(uint8_t bPortNum, uint8_t bOn)
{
#ifdef USB_PWR_SWITCH_GPIO_CUST
    printk("\n");
    INFO("[USB0]: 5V On, Set GPIO none.\n");
    INFO("[USB1]: 5V On, Set GPIO none.\n");
#else 
    unsigned long flags=0;

    INFO("[USB%d] Vbus %s.\n", bPortNum, ((bOn) ? "On": "Off"));

    local_irq_save(flags);

    if (bOn)
    {
        switch (bPortNum)
        {
            case 0:
                // OPTCTRL 0      
                *((volatile uint32_t *)0xf002807c) |= 0x00000001;
                *((volatile uint32_t *)0xf0028074) |= 0x00000001;
                break;
            default:
                break;
        }
    }
    else
    {
        switch (bPortNum)
        {
            case 0:
                // OPTCTRL 0      
                *((volatile uint32_t *)0xf002807c) |= 0x00000001;
                *((volatile uint32_t *)0xf0028074) &= ~0x00000001;
                break;
            default:
                break;
        }
    }
            
    local_irq_restore(flags);            
          
    #endif 
}

static MUSB_LinuxController MUC_aLinuxController[] = 
{
#ifndef USB_DEVICE_MODE_SUPPORT   
    {
         /* Port 0 information. */
        .pBase = (void*)MUSB_BASE, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT0_PHYBASE)), 
        .dwIrq = VECTOR_USB0, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 9,     /* Endpoint Number : 8+1*/
        .bEndpoint_Rx_num = 9,   /* Endpoint Number : 8+1*/
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl, 
        .MGC_pfPhyReset = MGC_PhyReset,          
    },
#endif
    {
         /* Port 1 information. */
        .pBase = (void*)MUSB_BASE1, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT1_PHYBASE)), 
        .dwIrq = VECTOR_USB1, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 9,     /* Endpoint Number : 8+1*/
        .bEndpoint_Rx_num = 9,   /* Endpoint Number : 8+1*/
        .bHub_support = TRUE,
        .MGC_pfVbusControl = NULL,  /* Only use one gpio */
        .MGC_pfPhyReset = MGC_PhyReset,
    }
};

#elif defined(CONFIG_ARCH_MT5398)
#ifdef CONFIG_MT53XX_NATIVE_GPIO 
static void MGC_VBusControl(uint8_t bPortNum, uint8_t bOn)
{
    unsigned long flags=0;
#ifdef USB_DEVICE_MODE_SUPPORT
    bPortNum += 0x1;
#endif
    // usb power switch pin define in \target\mt53980\5398_driver\drv_cust\mtk\config\*.h
    //INFO("[USB%d]Vbus %s gpio[%d].", bPortNum, ((bOn) ? "On": "Off"),(MUC_aPortUsing[bPortNum] == TRUE)? (UINT32_T *)MUC_aPwrGpio[bPortNum]: NULL);
    local_irq_save(flags);
    if(MUC_aPortUsing[bPortNum] == 1)
    {
        mtk_gpio_set_value(MUC_aPwrGpio[bPortNum], 
                ((bOn) ? (MUC_aPwrPolarity[bPortNum]) : (1-MUC_aPwrPolarity[bPortNum])));
    }
    local_irq_restore(flags); 
}

#else

static void MGC_VBusControl(uint8_t bPortNum, uint8_t bOn)
{
#ifdef USB_PWR_SWITCH_GPIO_CUST
    printk("\n");
    INFO("[USB0]: 5V On, Set GPIO none.\n");
    INFO("[USB1]: 5V On, Set GPIO none.\n");
    INFO("[USB2]: 5V On, Set GPIO none.\n");
    INFO("[USB3]: 5V On, Set GPIO none.\n");
#else 
    unsigned long flags=0;
#ifdef USB_DEVICE_MODE_SUPPORT
		bPortNum += 0x1;
#endif

    INFO("[USB%d] Vbus %s.\n", bPortNum, ((bOn) ? "On": "Off"));

    local_irq_save(flags);

    if (bOn)
    {
        switch (bPortNum)
        {
            case 0:
                // GPIO 51
                *((volatile uint32_t *)0xF000D724) |= 0x00080000;
                *((volatile uint32_t *)0xF000D704) |= 0x00080000;
                break;
            case 1:
                // GPIO 43            
                *((volatile uint32_t *)0xF000D724) |= 0x00000800;
                *((volatile uint32_t *)0xF000D704) |= 0x00000800;                            
                break;
            case 2:
                // GPIO 44            
                *((volatile uint32_t *)0xF000D724) |= 0x00001000;
                *((volatile uint32_t *)0xF000D704) |= 0x00001000;
                break;
            case 3:
                // OPTCTRL 10  
                *((volatile uint32_t *)0xF002807C) |= 0x00000400;
                *((volatile uint32_t *)0xF0028074) |= 0x00000400;
                break;

            default:
                break;
        }
    }
    else
    {
        switch (bPortNum)
        {
            case 0:
                // GPIO 51
                *((volatile uint32_t *)0xF000D724) |= 0x00080000;
                *((volatile uint32_t *)0xF000D704) &= ~0x00080000;
                break;
            case 1:
                // GPIO 43            
                *((volatile uint32_t *)0xF000D724) |= 0x00000800;
                *((volatile uint32_t *)0xF000D704) &= ~0x00000800;                            
                break;
            case 2:
                // GPIO 44            
                *((volatile uint32_t *)0xF000D724) |= 0x00001000;
                *((volatile uint32_t *)0xF000D704) &= ~0x00001000;
                break;
            case 3:
                // OPTCTRL 10  
                *((volatile uint32_t *)0xF002807C) |= 0x00000400;
                *((volatile uint32_t *)0xF0028074) &= ~0x00000400;
                break;

            default:
                break;
        }
    }
            
    local_irq_restore(flags);            
          
    #endif 
}
#endif

static MUSB_LinuxController MUC_aLinuxController[] = 
{
#ifndef USB_DEVICE_MODE_SUPPORT   
    {
         /* Port 0 information. */
        .pBase = (void*)MUSB_BASE, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT0_PHYBASE)), 
        .dwIrq = VECTOR_USB0, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 6,	  /* Endpoint Tx Number : 5+1 */
        .bEndpoint_Rx_num = 8,    /* Endpoint Rx Number : 7+1 */
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl, 
        .MGC_pfPhyReset = MGC_PhyReset,          
    },
#endif    
    {
         /* Port 1 information. */
        .pBase = (void*)MUSB_BASE1, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT1_PHYBASE)), 
        .dwIrq = VECTOR_USB1, 
        .bSupportCmdQ = TRUE, 
	 .bEndpoint_Tx_num = 6,    /* Endpoint Tx Number : 5+1 */
	 .bEndpoint_Rx_num = 8,    /* Endpoint Rx Number : 7+1 */
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl,
        .MGC_pfPhyReset = MGC_PhyReset,        
    },
    {
         /* Port 2 information. */
        .pBase = (void*)MUSB_BASE2,
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT2_PHYBASE)),         
        .dwIrq = VECTOR_USB2, 
        .bSupportCmdQ = TRUE, 
		.bEndpoint_Tx_num = 6,    /* Endpoint Tx Number : 5+1 */
		.bEndpoint_Rx_num = 8,    /* Endpoint Rx Number : 7+1 */
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl,
        .MGC_pfPhyReset = MGC_PhyReset,        
    },
    {
         /* Port 3 information. */
        .pBase = (void*)MUSB_BASE3, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT3_PHYBASE)),         
        .dwIrq = VECTOR_USB3, 
        .bSupportCmdQ = TRUE, 
		.bEndpoint_Tx_num = 6,    /* Endpoint Tx Number : 5+1 */
		.bEndpoint_Rx_num = 8,    /* Endpoint Rx Number : 7+1 */
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl,
        .MGC_pfPhyReset = MGC_PhyReset,        
    }
};

#elif defined(CONFIG_ARCH_MT5880) || defined(CONFIG_ARCH_MT5881)

#ifdef CONFIG_MT53XX_NATIVE_GPIO
static void MGC_VBusControl(uint8_t bPortNum, uint8_t bOn)
{
    unsigned long flags=0;
    #ifdef USB_DEVICE_MODE_SUPPORT
    bPortNum += 0x1;
    #endif
    // usb power switch pin define in \target\mt5880\5880_driver\drv_cust\mtk\config\*.h
    INFO("[USB%d]Vbus %s gpio[%d].\n", bPortNum, ((bOn) ? "On": "Off"),(MUC_aPortUsing[bPortNum] == TRUE)? (uint32_t *)MUC_aPwrGpio[bPortNum]: NULL);
    local_irq_save(flags);
    if(MUC_aPortUsing[bPortNum] == 1)
    {
        mtk_gpio_set_value(MUC_aPwrGpio[bPortNum], 
                ((bOn) ? (MUC_aPwrPolarity[bPortNum]) : (1-MUC_aPwrPolarity[bPortNum])));
    }
#ifndef DEMO_BOARD
#ifdef CONFIG_USB_OC_SUPPORT
	if(bOn)
	  {
		    u1MUSBVbusEanble = TRUE;
	  }
	  else
	  {
		    u1MUSBVbusEanble = FALSE;
	  }
#endif
#endif
    local_irq_restore(flags);                
}
#else    //CONFIG_MT53XX_NATIVE_GPIO
static void MGC_VBusControl(uint8_t bPortNum, uint8_t bOn)
{
#ifdef USB_PWR_SWITCH_GPIO_CUST
    printk("\n");
    INFO("[USB0]: 5V On, Set GPIO none.\n");
    INFO("[USB1]: 5V On, Set GPIO none.\n");
#else 
    unsigned long flags=0;
#ifdef USB_DEVICE_MODE_SUPPORT
	bPortNum += 0x1;
#endif
    INFO("[USB%d] Vbus %s.\n", bPortNum, ((bOn) ? "On": "Off"));

    local_irq_save(flags);

    if (bOn)
    {
        switch (bPortNum)
        {
            case 0:
                // GPIO 63
                *((volatile uint32_t *)0xF000D600) |= 0x500000;  //pimux set GPIO63 to GPIO output 0XF000D600[22:20]=5
                *((volatile uint32_t *)0xF000D724) |= 0x80000000;
                *((volatile uint32_t *)0xF000D704) |= 0x80000000;
                break;
            case 1:
                // GPIO 64       
                *((volatile uint32_t *)0xF000D600) |= 0x2800000; //pimux set GPIO63 to GPIO output 0XF000D600[25:23]=5
                *((volatile uint32_t *)0xF000D728) |= 0x00000001;
                *((volatile uint32_t *)0xF000D708) |= 0x00000001;                            
                break;
                
            case 2:
            	    //GPIO 61   
                *((volatile uint32_t *)0xF000D600) |= 0x14000;  //pimux set GPIO63 to GPIO output 0XF000D600[16:14]=5
                *((volatile uint32_t *)0xF000D724) |= 0x20000000;
                *((volatile uint32_t *)0xF000D704) |= 0x20000000; 
                
            default:
                break;
        }
    }
    else
    {
        switch (bPortNum)
        {
            case 0:
                // GPIO 63
                *((volatile uint32_t *)0xF000D600) |= 0x500000;  //pimux set GPIO63 to GPIO output 0XF000D600[22:20]=5
                *((volatile uint32_t *)0xF000D724) |= 0x80000000;
                *((volatile uint32_t *)0xF000D704) &= ~0x80000000;
                break;
            case 1:
                // GPIO 64   
                *((volatile uint32_t *)0xF000D600) |= 0x2800000; //pimux set GPIO63 to GPIO output 0XF000D600[25:23]=5
                *((volatile uint32_t *)0xF000D728) |= 0x00000001;
                *((volatile uint32_t *)0xF000D708) &= ~0x00000001;                            
                break;
            case 2:
            	    //GPIO 61   
                *((volatile uint32_t *)0xF000D600) |= 0x14000;  //pimux set GPIO63 to GPIO output 0XF000D600[16:14]=5
                *((volatile uint32_t *)0xF000D724) |= 0x20000000;
                *((volatile uint32_t *)0xF000D704) &= ~0x20000000;     
                
            default:
                break;
        }
    }
            
#ifndef DEMO_BOARD
#ifdef CONFIG_USB_OC_SUPPORT
		if(bOn)
		  {
				u1MUSBVbusEanble = TRUE;
		  }
		  else
		  {
				u1MUSBVbusEanble = FALSE;
		  }
#endif
#endif
    local_irq_restore(flags);            
          
    #endif 
}
#endif

static MUSB_LinuxController MUC_aLinuxController[] = 
{
#ifndef USB_DEVICE_MODE_SUPPORT   
    {
         /* Port 0 information. */
        .pBase = (void*)MUSB_BASE, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT0_PHYBASE)), 
        .dwIrq = VECTOR_USB0, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 6,	  /* Endpoint Tx Number : 5+1 */
        .bEndpoint_Rx_num = 8,    /* Endpoint Rx Number : 7+1 */
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl, 
        .MGC_pfPhyReset = MGC_PhyReset,          
    },
#endif    
    {
         /* Port 1 information. */
        .pBase = (void*)MUSB_BASE1, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT1_PHYBASE)), 
        .dwIrq = VECTOR_USB1, 
        .bSupportCmdQ = TRUE, 
	 .bEndpoint_Tx_num = 6,    /* Endpoint Tx Number : 5+1 */
	 .bEndpoint_Rx_num = 8,    /* Endpoint Rx Number : 7+1 */
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl,
        .MGC_pfPhyReset = MGC_PhyReset,        
    },
    {
         /* Port 2 information. */
        .pBase = (void*)MUSB_BASE2, 
        .pPhyBase = (void*)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT2_PHYBASE)), 
        .dwIrq = VECTOR_USB2, 
        .bSupportCmdQ = TRUE, 
        .bEndpoint_Tx_num = 6,    /* Endpoint Tx Number : 5+1 */
        .bEndpoint_Rx_num = 8,    /* Endpoint Rx Number : 7+1 */
        .bHub_support = TRUE,
        .MGC_pfVbusControl = MGC_VBusControl,
        .MGC_pfPhyReset = MGC_PhyReset,        
    }
};

#else
#error CONFIG_ARCH_MTXXXX not defined !
#endif 

static struct platform_device MUC_pdev[MUC_NUM_PLATFORM_DEV];

//---------------------------------------------------------------------------
// Global function
//---------------------------------------------------------------------------
uint32_t MUCUST_GetControllerNum(void)
{
    return (MUC_NUM_PLATFORM_DEV);
}

MUSB_LinuxController* MUCUST_GetControllerDev(uint32_t u4Index)
{
    return &MUC_aLinuxController[u4Index];
}

struct platform_device* MUCUST_GetPlatformDev(uint32_t u4Index)
{
    return &MUC_pdev[u4Index];
}

#ifndef DEMO_BOARD
#ifdef CONFIG_USB_OC_SUPPORT
static int proc_oc_message(void __user *arg)
{
    uint8_t *buf = NULL;
    uint8_t size;
    size = sizeof(u1MUSBOCStatus);
    if ((buf = kmalloc(size, GFP_KERNEL)) == NULL)
	return -ENOMEM;

    *buf = u1MUSBOCStatus;
   // printk("proc_oc_message>>u1MUSBOCStatus = %u\n", u1MUSBOCStatus);
    
    if (copy_to_user((uint8_t __user *)arg, buf, sizeof(u1MUSBOCStatus)))
        return -EFAULT;
    kfree(buf);
    return 0;
}

static int MGC_USBOCThreadMain(void * pvArg)
{
	unsigned long	 flags;
	uint32_t u4Val;
	uint8_t fgOCStatus;
	uint8_t fgOCNewStatus;
	uint32_t u4OCCount;
	uint8_t i = 0;
	
	local_irq_save(flags);
	
	//OC pin PIMUX SET TO GPIO as input (set pimux &function)
	*((volatile uint32_t *)0xF000D600) &= ~0x800;  
	*((volatile uint32_t *)0xF000D724) &= ~0x10000000;

	local_irq_restore(flags);
	
	u1MUSBOCEnable = FALSE;
	u1MUSBOCStatus = FALSE;

   do{
   		if((i == 0) && (u1MUSBVbusEanble))
		{
			//printk("[OC]USB OC enable.\n");
			u1MUSBOCEnable = TRUE;
			i = 1;
		}
		
		if(u1MUSBOCEnable)
		{
			// Check OverCurrent.					
			u4Val = 0;
			u4OCCount = 0;
			fgOCNewStatus = u1MUSBOCStatus;
			//printk("[OC]Start do usb overcurrent checking\n");
			
			do
			{				 
				// check over current status.
				{
					if(mtk_gpio_get_value(60))
						fgOCStatus = FALSE;
					else
						fgOCStatus = TRUE;
				}				 
				
				// debounce.
				if (fgOCNewStatus != fgOCStatus)
				{
					fgOCNewStatus = fgOCStatus;
					u4OCCount = 0;
					//printk("usb1 status changefgOCStatus = %d:\n", fgOCStatus);
				}
				else
				{
					//printk("usb1 status stable fgOCStatus = %d, u4OCCount =%d.\n", fgOCStatus, u4OCCount);
					u4OCCount ++;
				}						 
				msleep(50);
			} while(u4OCCount < 5);
			//printk("[OC]do usb upgrade fgOCStatus = %d, u1MUSBOCStatus =%d u1MUSBVbusEanble=%d.\n", fgOCStatus, u1MUSBOCStatus, u1MUSBVbusEanble);				 

			if (fgOCStatus != u1MUSBOCStatus)
			{	
				// change over current status.
				u1MUSBOCStatus = fgOCStatus; 
				if(u1MUSBOCStatus) printk("###OC OC OC ###.\n");
				// Over current is just disappear. Turn on Vbus, and check if it still happen. 
				if ((!fgOCStatus) && (u1MUSBVbusEanble == FALSE))
				{
					//printk("USB OC: Turn on Vbus GPIO on.\n");
					u1MUSBVbusEanble = TRUE;
					MGC_VBusControl(1, 1);
				}
			
				if ((fgOCStatus) && (u1MUSBVbusEanble == TRUE))
				{
					//printk("USB OC: Turn off Vbus GPIO off\n");
					u1MUSBVbusEanble = FALSE;
					MGC_VBusControl(1, 0);	   
					msleep(5000);
				}							   
			}
			else
			{
				/*
					When turn on servo gpio to get adc value, it will turn on vbus.
				*/
				if (fgOCStatus)
				{
					if ((u1MUSBVbusEanble == TRUE))
					{
						//printk("USB OC2: Turn off Vbus GPIO off\n");
						u1MUSBVbusEanble = FALSE;	
						MGC_VBusControl(1, 0);
						msleep(5000);
					}
				}
			}

		 msleep(1000);
	 }
}while(1);

	return 0;
}


static void MGC_USBOCCreateThread(void)
{
    struct task_struct *tThreadStruct;
    char ThreadName[10]={0};
    // Thread Name : USB_OC
    ThreadName[0] = 'U';
    ThreadName[1] = 'S';
    ThreadName[2] = 'B';
    ThreadName[3] = '_';
    ThreadName[4] = 'O';
    ThreadName[5] = 'C';

	printk("OCCreate thread.\n");
	
    tThreadStruct = kthread_create(MGC_USBOCThreadMain, NULL, ThreadName);

    if ( (unsigned long)tThreadStruct == -ENOMEM )
    {
        printk("[USB]MGC_USBOCCreateThread Failed\n");
        return;
    }

    wake_up_process(tThreadStruct);
    return;
}

#endif
#endif

static ssize_t MUT_read(struct file *file, char *buf, size_t count, loff_t *ptr)
{

    //printk("MUT_read\n");
    return 0;
}

static ssize_t MUT_write(struct file *file, const char *buf, size_t count, loff_t * ppos)
{

    //printk("MUT_write\n");
    return 0;
}

static int MUT_ioctl( struct file *file, unsigned int cmd, unsigned long arg)
{
    int retval=0;

	switch (cmd) {
       #ifndef DEMO_BOARD
	#ifdef CONFIG_USB_OC_SUPPORT
		case MUT_IOCTL_OC:
         		 retval = proc_oc_message(arg);
          		break;

			return retval;
	 #endif		
     	 #endif
		case MUT_IOCTL_SET:
    			 return retval;
			
		default:
			return -ENOTTY;
	}

	return 0;

}

static int MUT_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int MUT_release(struct inode *inode, struct file *file)
{
    return 0;
}

struct file_operations MUT_fops = {
    owner:   THIS_MODULE,
    read:    MUT_read,
    write:   MUT_write,
    unlocked_ioctl:   MUT_ioctl,
    open:    MUT_open,
    release: MUT_release,
};

int MUT_init(void)
{
#ifndef DEMO_BOARD
#ifdef CONFIG_USB_OC_SUPPORT

    int retval = 0;
    printk("MUT_init\n");
    retval = register_chrdev(MTK_USB_TEST_MAJOR, DEVICE_NAME, &MUT_fops);
    if(retval < 0)
	{
		printk("MUT_init register_chrdev() failed, %d\n", retval);
		return retval;
	}
    // Init USB OC support.
    MGC_USBOCCreateThread();
#endif
#endif
    return 0;
}

void MUT_exit(void)
{
#ifndef DEMO_BOARD
#ifdef CONFIG_USB_OC_SUPPORT
    unregister_chrdev(MTK_USB_TEST_MAJOR, DEVICE_NAME);
#endif
#endif
    printk("MUT_exit\n");
}

