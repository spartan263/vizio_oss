/*-----------------------------------------------------------------------------
 *\drivers\usb\host\mtk_hcd_95.c
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


/** @file mtk_hcd.c
 *  This C file implements the mtk53xx USB host controller driver.
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
#include <linux/smp_lock.h>
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
#include "mtk_hcd_95.h"

MODULE_DESCRIPTION("MTK 53xx USB Host Controller Driver");
MODULE_LICENSE("GPL");

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------
#define DRIVER_VERSION    "18 Feb 2009"

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------
#if defined(CONFIG_ARCH_MT5391) || defined(CONFIG_ARCH_MT5365) || defined(CONFIG_ARCH_MT5395)
    #define MUC_NUM_PLATFORM_DEV (2)
#elif defined (CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
    #define MUC_NUM_PLATFORM_DEV (4)
#else 
    #define MUC_NUM_PLATFORM_DEV (1)
#endif 


#define EPRX (0)
#define EPTX (1)
#define EP0 (0)
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

static void MGC_ServiceDefaultEnd(MGC_LinuxCd *pThis);
static void MGC_ServiceTxAvail(MGC_LinuxCd *pThis, uint8_t bEnd);
static void MGC_ServiceRxReady(MGC_LinuxCd *pThis, uint8_t bEnd);

static irqreturn_t MUC_Irq(struct usb_hcd *hcd);
static int MUC_start(struct usb_hcd *hcd);
static void MUC_stop(struct usb_hcd *hcd);
static int MUC_urb_enqueue(struct usb_hcd *hcd, struct urb *pUrb, gfp_t mem_flags);
static int MUC_urb_dequeue(struct usb_hcd *hcd, struct urb *pUrb, int status);
static void MUC_endpoint_disable(struct usb_hcd *hcd, struct usb_host_endpoint *hep);
static int MUC_get_frame(struct usb_hcd *hcd);
static int MUC_hub_status_data(struct usb_hcd *hcd, char *buf);
static int MUC_hub_control(struct usb_hcd *hcd, uint16_t typeReq, uint16_t wValue, uint16_t wIndex, char *buf, uint16_t wLength);
static int MUC_bus_suspend(struct usb_hcd *hcd);
static int MUC_bus_resume(struct usb_hcd *hcd);
static void MUC_hub_descriptor(MGC_LinuxCd *pThis, struct usb_hub_descriptor *desc);

static int MUC_hcd_probe(struct platform_device *pdev);
static int MUC_hcd_remove(struct platform_device *pdev);
static int MUC_hcd_suspend(struct platform_device *pdev, pm_message_t state);
static int MUC_hcd_resume(struct platform_device *pdev);

#ifdef MUSB_DEBUG
    static int MGC_DebugLevel = 3;
    static int MGC_DebugDisable = 0;
#endif 

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
static const char MUC_HcdName[] = "MtkUsbHcd";


static MUSB_LinuxController MUC_aLinuxController[] = 
{
    #if defined(CONFIG_ARCH_MT5391) || defined(CONFIG_ARCH_MT5365) || defined(CONFIG_ARCH_MT5395)
    {
         /* Port 0 information. */
        .wType = MUSB_CONTROLLER_MHDRC, 
        .pBase = (void*)MUSB_BASE, 
        .dwIrq = VECTOR_USB0, 
        .bSupport = FALSE, 
    } ,
    {
         /* Port 1 information. */
        .wType = MUSB_CONTROLLER_MHDRC, 
        .pBase = (void*)MUSB_BASE1, 
        .dwIrq = VECTOR_USB1, 
        .bSupport = FALSE, 
    }
    #elif defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
    {
         /* Port 1 information. */
        .wType = MUSB_CONTROLLER_MHDRC, 
        .pBase = (void*)MUSB_BASE, 
        .dwIrq = VECTOR_USB0, 
        .bSupport = FALSE, 
        .bEndpoint_num = MUSB_C_NUM_EPS0,
        .bHub_support = FALSE,
    },
    {
         /* Port 1 information. */
        .wType = MUSB_CONTROLLER_MHDRC, 
        .pBase = (void*)MUSB_BASE1, 
        .dwIrq = VECTOR_USB1, 
        .bSupport = FALSE, 
        .bEndpoint_num = MUSB_C_NUM_EPS1,
        .bHub_support = FALSE,
    },
    {
         /* Port 2 information. */
        .wType = MUSB_CONTROLLER_MHDRC, 
        .pBase = (void*)MUSB_BASE2, 
        .dwIrq = VECTOR_USB2, 
        .bSupport = FALSE, 
        .bEndpoint_num = MUSB_C_NUM_EPS2,
        .bHub_support = TRUE,
    },
    {
         /* Port 3 information. */
        .wType = MUSB_CONTROLLER_MHDRC, 
        .pBase = (void*)MUSB_BASE3, 
        .dwIrq = VECTOR_USB3, 
        .bSupport = FALSE, 
        .bEndpoint_num = MUSB_C_NUM_EPS3,
        .bHub_support = TRUE,
    }
    #endif
};

static struct platform_device MUC_pdev[MUC_NUM_PLATFORM_DEV];

static struct platform_driver MUC_hcd_driver = 
{
    .probe = MUC_hcd_probe, 
    .remove = MUC_hcd_remove, 
    .suspend = MUC_hcd_suspend, 
    .resume = MUC_hcd_resume, 
    .driver = 
    {
        .name = (char*)MUC_HcdName, 
        .owner = THIS_MODULE, 
    } , 
};

static struct hc_driver MUC_hc_driver = 
{
    .description = MUC_HcdName, 
    .hcd_priv_size = sizeof(MGC_LinuxCd), 

    /*
     * generic hardware linkage
     */
    .irq = MUC_Irq, 
    .flags = HCD_USB2 | HCD_MEMORY, 

    /* Basic lifecycle operations */
    .start = MUC_start, 
    .stop = MUC_stop, 

    /*
     * managing i/o requests and associated device resources
     */
    .urb_enqueue = MUC_urb_enqueue, 
    .urb_dequeue = MUC_urb_dequeue, 
    .endpoint_disable = MUC_endpoint_disable, 

    /*
     * periodic schedule support
     */
    .get_frame_number = MUC_get_frame, 

    /*
     * root hub support
     */
    .hub_status_data = MUC_hub_status_data, 
    .hub_control = MUC_hub_control, 
    .bus_suspend = MUC_bus_suspend, 
    .bus_resume = MUC_bus_resume, 
};

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
static uint8_t MGC_Ep_Count(void *pBase)
{
    uint8_t i;
    for(i = 0; i < MUSB_PORT_NUM; i ++)
    {
        if((uint32_t)pBase == (uint32_t)MUC_aLinuxController[i].pBase)
            return MUC_aLinuxController[i].bEndpoint_num;
    }

    return MUC_aLinuxController[0].bEndpoint_num;
}
#endif

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------
void MGC_VBusTurnOn(void)
{
    #ifdef USB_PWR_SWITCH_GPIO_CUST
        printk("\n");
        printk("[USB0]: 5V On, Set GPIO none.\n");
        printk("[USB1]: 5V On, Set GPIO none.\n");
    #else 
        uint32_t *pu4MemTmp=0;
        unsigned long flags=0;
        #if defined(CONFIG_ARCH_MT5391) 
            local_irq_save(flags);
            pu4MemTmp = (uint32_t*)0xf000d520;
            *pu4MemTmp =  *pu4MemTmp | 0x5;
            pu4MemTmp = (uint32_t*)0xf000d500;
            *pu4MemTmp =  *pu4MemTmp | 0x5;
            local_irq_restore(flags);
        #elif defined(CONFIG_ARCH_MT5363)
            local_irq_save(flags);
            pu4MemTmp = (uint32_t*)0xf0028080;
            *pu4MemTmp =  *pu4MemTmp | 0x2;
            pu4MemTmp = (uint32_t*)0xf0028084;
            *pu4MemTmp =  *pu4MemTmp | 0x2;
            local_irq_restore(flags);
        #elif defined(CONFIG_ARCH_MT5365)
            printk("\n");
            printk("[USB0]: 5V On,Set GPIO23 = 1.\n");
            printk("[USB1]: 5V On,Set GPIO24 = 1.\n");
            local_irq_save(flags);
            pu4MemTmp = (uint32_t*)0xf000d500;
            *pu4MemTmp =  *pu4MemTmp | 0x01800000;
            pu4MemTmp = (uint32_t*)0xf000d510;
            *pu4MemTmp =  *pu4MemTmp | 0x01800000;
            local_irq_restore(flags);
        #elif defined(CONFIG_ARCH_MT5395)
            printk("\n");
            printk("[USB0]: 5V On,Set GPIO0 = 1.\n");
            printk("[USB1]: 5V On,Set GPIO2 = 1.\n");
            local_irq_save(flags);
            pu4MemTmp = (uint32_t*)0xf000d520;
            *pu4MemTmp =  *pu4MemTmp | 0x5;
            pu4MemTmp = (uint32_t*)0xf000d500;
            *pu4MemTmp =  *pu4MemTmp | 0x5;
            local_irq_restore(flags);
        #elif defined(CONFIG_ARCH_MT5396)
            printk("\n");
            printk("[USB0]: 5V Off,Set GPIO43 = 1.\n");
            printk("[USB1]: 5V Off,Set GPIO44 = 1.\n");
            printk("[USB2]: 5V Off,Set OPTCTRL11 = 1.\n");
            printk("[USB3]: 5V Off,Set OPTCTRL10 = 1.\n");

            local_irq_save(flags);
            // GPIO 43/44            
            pu4MemTmp = (uint32_t*)0xf000d524;
            *pu4MemTmp =  *pu4MemTmp | 0x00001800;
            pu4MemTmp = (uint32_t*)0xf000d504;
            *pu4MemTmp =  *pu4MemTmp | 0x00001800;
            // OPTCTRL 11/10
            pu4MemTmp = (uint32_t*)0xf002807c;
            *pu4MemTmp =  *pu4MemTmp | 0x00000C00;
            pu4MemTmp = (uint32_t*)0xf0028074;
            *pu4MemTmp =  *pu4MemTmp | 0x00000C00;
            
            local_irq_restore(flags);            
        #elif defined(CONFIG_ARCH_MT5368)
            (void)pu4MemTmp;
            (void)flags;
        #else 
            #error CONFIG_ARCH_MTXXXX not defined !
        #endif 
    #endif 
    return ;
}

void MGC_VBusTurnOff(void)
{
    #ifdef USB_PWR_SWITCH_GPIO_CUST
        printk("\n");
        printk("[USB0]: 5V Off,Set GPIO none.\n");
        printk("[USB1]: 5V Off,Set GPIO none.\n");
    #else 
        uint32_t *pu4MemTmp=0;
        unsigned long flags=0;
        #ifdef CONFIG_ARCH_MT5391
            local_irq_save(flags);
            pu4MemTmp = (uint32_t*)0xf000d520;
            *pu4MemTmp =  *pu4MemTmp &(~0x5);
            pu4MemTmp = (uint32_t*)0xf000d500;
            *pu4MemTmp =  *pu4MemTmp &(~0x5);
            local_irq_restore(flags);
        #elif defined(CONFIG_ARCH_MT5363)
            local_irq_save(flags);
            pu4MemTmp = (uint32_t*)0xf0028080;
            *pu4MemTmp =  *pu4MemTmp &(~0x2);
            pu4MemTmp = (uint32_t*)0xf0028084;
            *pu4MemTmp =  *pu4MemTmp &(~0x2);
            local_irq_restore(flags);
        #elif defined(CONFIG_ARCH_MT5365)
            printk("\n");
            printk("[USB0]: 5V Off,Set GPIO23 = 0.\n");
            printk("[USB1]: 5V Off,Set GPIO24 = 0.\n");
            local_irq_save(flags);
            pu4MemTmp = (uint32_t*)0xf000d500;
            *pu4MemTmp =  *pu4MemTmp &(~0x01800000);
            pu4MemTmp = (uint32_t*)0xf000d510;
            *pu4MemTmp =  *pu4MemTmp | 0x01800000;
            local_irq_restore(flags);
        #elif defined(CONFIG_ARCH_MT5395)
            printk("\n");
            printk("[USB0]: 5V Off,Set GPIO0 = 0.\n");
            printk("[USB1]: 5V Off,Set GPIO2 = 0.\n");
            local_irq_save(flags);
            pu4MemTmp = (uint32_t*)0xf000d520;
            *pu4MemTmp =  *pu4MemTmp | 0x5;
            pu4MemTmp = (uint32_t*)0xf000d500;
            *pu4MemTmp =  *pu4MemTmp &(~0x5);
            local_irq_restore(flags);
        #elif defined(CONFIG_ARCH_MT5396)
            printk("\n");
            printk("[USB0]: 5V Off,Set GPIO43 = 0.\n");
            printk("[USB1]: 5V Off,Set GPIO44 = 0.\n");
            printk("[USB2]: 5V Off,Set OPTCTRL11 = 0.\n");
            printk("[USB3]: 5V Off,Set OPTCTRL10 = 0.\n");

            local_irq_save(flags);
            // GPIO 43/44
            pu4MemTmp = (uint32_t*)0xf000d524;
            *pu4MemTmp =  *pu4MemTmp | 0x00001800;
            pu4MemTmp = (uint32_t*)0xf000d504;
            *pu4MemTmp =  *pu4MemTmp &(~0x00001800);
            // OPTCTRL 11/10
            pu4MemTmp = (uint32_t*)0xf002807c;
            *pu4MemTmp =  *pu4MemTmp | 0x00000C00;
            pu4MemTmp = (uint32_t*)0xf0028074;
            *pu4MemTmp =  *pu4MemTmp &(~0x00000C00);
            local_irq_restore(flags);
            
        #elif defined(CONFIG_ARCH_MT5368)
            (void)pu4MemTmp;
            (void)flags;
        #else 
            #error CONFIG_ARCH_MTXXXX not defined !
        #endif 
    #endif 
    return ;
}

EXPORT_SYMBOL(MGC_VBusTurnOn);
EXPORT_SYMBOL(MGC_VBusTurnOff);

int MGC_VBusThreadMain(void *pvArg)
{
    msleep(5000);
    MGC_VBusTurnOn();
    return 0;
}
static void MGC_VBusCreateTurnOnThread(void)
{
    struct task_struct *tThreadStruct;
    char ThreadName[10] = {0};
    // Thread Name : USB_VBUS
    ThreadName[0] = 'U';
    ThreadName[1] = 'S';
    ThreadName[2] = 'B';
    ThreadName[3] = '_';
    ThreadName[4] = 'V';
    ThreadName[5] = 'B';
    ThreadName[6] = 'U';
    ThreadName[7] = 'S';

    tThreadStruct = kthread_create(MGC_VBusThreadMain, NULL, ThreadName);

    if ((unsigned long)tThreadStruct ==  - ENOMEM)
    {
        printk("[USB] MGC_VBusCreateTurnOnThread Failed\n");
        return ;
    }

    wake_up_process(tThreadStruct);
    return ;
}

static void MGC_VBusInitHandler(void)
{
    MGC_VBusTurnOff();
    MGC_VBusCreateTurnOnThread();
}

static inline MGC_LinuxCd *hcd_to_musbstruct(struct usb_hcd *hcd)
{
    return (MGC_LinuxCd*)(hcd->hcd_priv);
} 

static inline struct usb_hcd *musbstruct_to_hcd(const MGC_LinuxCd *pThis)
{
    return container_of((void*)pThis, struct usb_hcd, hcd_priv);
} 

static inline int MGC_IsPeriodicUrb(struct urb *pUrb)
{
    return (usb_pipeint(pUrb->pipe) || usb_pipeisoc(pUrb->pipe));
} 

#ifdef MUSB_DEBUG
    static char *MGC_DecodeUrbProtocol(struct urb *pUrb)
    {
        static char buffer[8];

        if (!pUrb)
        {
            strcpy(&buffer[0], "NULL");

            return buffer;
        } 

        buffer[0] = usb_pipein(pUrb->pipe) ? 'I' : 'O';

        if (usb_pipeint(pUrb->pipe))
        {
            strcpy(&buffer[1], " int");
        }
        else if (usb_pipeisoc(pUrb->pipe))
        {
            strcpy(&buffer[1], " isoc");
        }
        else if (usb_pipebulk(pUrb->pipe))
        {
            strcpy(&buffer[1], " bulk");
        }
        else if (usb_pipecontrol(pUrb->pipe))
        {
            strcpy(&buffer[0], " ctl");
        }

        return buffer;
    }
#endif 

 
MGC_LinuxCd *pThisGlobal[MUSB_PORT_NUM]; 
EXPORT_SYMBOL(pThisGlobal);

static int MGC_CheckTxCsrValidForWrite(const void *pBase, uint8_t bEnd)
{
    uint32_t dwCtrl;
    uint8_t bChannel;

    // Assert when this condition happens :  Write EP TxCSR when the EP is doing DMA. 
    if (bEnd)
    {
        for (bChannel = 0; bChannel < MGC_HSDMA_CHANNELS; bChannel++)
        {
            dwCtrl = MGC_Read32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL));
            if ((dwCtrl &MGC_S_HSDMA_ENABLE) && (dwCtrl &MGC_S_HSDMA_TRANSMIT))
            {
                if (((dwCtrl &0xf0) >> 4) == bEnd)
                {
                    if (MGC_Read32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_COUNT)))
                    {
                        printk("[USB] Alert!  MGC_CheckTxCsrValidForWrite check error ! bEnd=%x, TxCSR=0x%x !\n", bEnd, dwCtrl);
                        MUSB_ASSERT(0);
                        return 0;
                    }
                }
            }
        }
    }

    return 1;
}

static int MGC_IsEndIdle(MGC_LinuxLocalEnd *pEnd)
{
    if (pEnd->pCurrentUrb)
    {
        return FALSE;
    }

    return list_empty(&pEnd->list);
}

static struct urb *MGC_GetNextUrb(MGC_LinuxLocalEnd *pEnd)
{
    MGC_LinuxUrbList *pUrbList;
    struct urb *pUrb = NULL;

    if (pEnd->pCurrentUrb)
    {
        MUSB_ASSERT(list_empty(&pEnd->list) == TRUE);
        pUrb = pEnd->pCurrentUrb;
    } 
    else
    {
        MUSB_ASSERT(list_empty(&pEnd->list) == FALSE);
        pUrbList = list_entry(pEnd->list.next, MGC_LinuxUrbList, list);
        if (pUrbList)
        {            
            pUrb = pUrbList->pUrb;
            if (pUrb)
            {                
                pEnd->pCurrentUrb = pUrb;                        
            }        
            list_del(&pUrbList->list);
            kfree (pUrbList);            
        }
    }

    /*
    if (pUrb)
    {
        printk("[USB] Next pUrb=0x%08X, size=%d.\n", (uint32_t)pUrb, pUrb->transfer_buffer_length);
    }
    else
    {
        printk("[USB] Next pUrb=NULL.\n");
    }
     */

    return pUrb;
}

static struct urb *MGC_GetCurrentUrb(MGC_LinuxLocalEnd *pEnd)
{
    return pEnd->pCurrentUrb;
} 

static void MGC_ClearEnd(MGC_LinuxLocalEnd *pEnd)
{
    pEnd->dwOffset = 0;
    pEnd->dwRequestSize = 0;
    pEnd->dwIsoPacket = 0;
    pEnd->dwWaitFrame = 0;
    pEnd->bRetries = 0;
    pEnd->bTrafficType = 0;
}

static int MGC_EnqueueEndUrb(MGC_LinuxLocalEnd *pEnd, struct urb *pUrb)
{
    MGC_LinuxUrbList *pUrbList;

    //printk("Enqueue: pUrb=0x%08X.\n", (uint32_t)pUrb);
   
    if ((pEnd->pCurrentUrb == NULL) && (list_empty(&pEnd->list) == TRUE))
    {
        // Only put pUrb to pCurrentUrb when pCurrentUrb and list are both empty.
        pEnd->pCurrentUrb = pUrb;
    } 
    else
    {
        pUrbList = kmalloc(sizeof(MGC_LinuxUrbList), pEnd->mem_flags);
        if (!pUrbList)
        {
            return  - ENOMEM;
        }

        pUrbList->pUrb = pUrb;
        list_add_tail(&pUrbList->list, &pEnd->list);
    }
    // Add pUrb to pEnd structure.
    pUrb->hcpriv = pEnd;

    return 0;
}

static int MGC_DequeueEndurb(MGC_LinuxLocalEnd *pEnd, struct urb *pUrb)
{
    MGC_LinuxUrbList *pUrbList;
    MGC_LinuxUrbList *pNextUrbList;

    //printk("Dequeue: pUrb=0x%08X.\n", (uint32_t)pUrb);

    // Remove pUrb to pEnd structure.
    pUrb->hcpriv = NULL;

    if (pEnd->pCurrentUrb == pUrb)
    {
        pEnd->pCurrentUrb = NULL;
        MGC_ClearEnd(pEnd);
    } 
    else
    {
        list_for_each_entry_safe(pUrbList, pNextUrbList, &pEnd->list, list)
        {
            if (pUrbList->pUrb == pUrb)
            {
                list_del(&pUrbList->list);
                kfree(pUrbList);
                break;
            }
        }
    }

    return 0;
}

static int MGC_CheckDequeueurb(MGC_LinuxLocalEnd *pEnd, struct urb *pUrb, int status)
{
    MGC_LinuxUrbList *pUrbList;
    MGC_LinuxUrbList *pNextUrbList;

    if (!(pEnd && pUrb))
    {
        return  - EIDRM;
    } 

    if (pEnd->pCurrentUrb == pUrb)
    {
        return 0;
    }
    else
    {
        list_for_each_entry_safe(pUrbList, pNextUrbList, &pEnd->list, list)
        {
            if (pUrbList->pUrb == pUrb)
            {
                break;
            }
        }
        if (pUrbList->pUrb != pUrb)
        {
            return  - EIDRM;
        }

        if (pUrb->unlinked)
        {
            return  - EBUSY;
        }

        pUrb->unlinked = status;
    }

    return 0;
}

static void MGC_InitEnd(MGC_LinuxCd *pThis)
{
    uint8_t bEnd;
    uint8_t bTx;

    MGC_LinuxLocalEnd *pEnd;
    void *pBase = pThis->pRegs;
    uint16_t wFifoOffset = 64;

    /* use the defined end points */
    #if defined(CONFIG_ARCH_MT5395) || defined(CONFIG_ARCH_MT5365)
    if ((uint32_t)pBase == MUSB_BASE)
    {
        pThis->bEndCount = MUSB_C_NUM_EPS;
    }
    else
    {
        pThis->bEndCount = MUSB_C_NUM_EPS1;
    }
    #elif defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
        pThis->bEndCount = MGC_Ep_Count(pBase);
    #else 
       pThis->bEndCount = MUSB_C_NUM_EPS;
    #endif 

    /* Dynamic FIFO sizing: use pre-computed values for EP0 */
    MGC_SelectEnd(pBase, 0);
    MGC_Write8(pBase, MGC_O_HDRC_TXFIFOSZ, 3);
    MGC_Write8(pBase, MGC_O_HDRC_RXFIFOSZ, 3);
    MGC_Write16(pBase, MGC_O_HDRC_TXFIFOADD, 0);
    MGC_Write16(pBase, MGC_O_HDRC_RXFIFOADD, 0);

    pEnd = &(pThis->aLocalEnd[EP0][0]);
    pEnd->bIsSharedFifo = TRUE;
    pEnd->bEnd = 0;
    pEnd->bIsTx = TRUE;
    pEnd->wMaxPacketSize = 64;
    pEnd->wPacketSize = 0;

    pThis->wEndMask = 1;
    /* reset the softstate */
    spin_lock_init(&pEnd->Lock);
    pEnd->pCurrentUrb = NULL;
    INIT_LIST_HEAD(&pEnd->list);

    pEnd->wPacketSize = 0;
    pEnd->bRemoteAddress = 0;
    pEnd->bRemoteEnd = 0;
    pEnd->bTrafficType = 0;

    #if defined(USB_WEBCAM_LOG) || defined(USB_WEBCAM_COM)
        if (pBase == MUSB_BASE)
        {
            for (bEnd = 1; bEnd < MUSB_C_NUM_EPS; bEnd++)
            {
                MGC_SelectEnd(pBase, bEnd);
                // fifo size = 512, turn off double packet buffer.
                MGC_Write8(pBase, MGC_O_HDRC_TXFIFOSZ, 6);
                MGC_Write8(pBase, MGC_O_HDRC_RXFIFOSZ, 6);
                MGC_Write16(pBase, MGC_O_HDRC_TXFIFOADD, wFifoOffset >> 3);
                DBG(3, "[USB] bEnd = %d, TX, fifo addr=%d.\n", bEnd, wFifoOffset);
                wFifoOffset += 512;
                MGC_Write16(pBase, MGC_O_HDRC_RXFIFOADD, wFifoOffset >> 3);
                DBG(3, "[USB] bEnd = %d, RX, fifo addr=%d.\n", bEnd, wFifoOffset);
                wFifoOffset += 512;
                pThis->wEndMask |= (1 << bEnd);

                for (bTx = EPRX; bTx <= EPTX; bTx++)
                {
                    pEnd = &(pThis->aLocalEnd[bTx][bEnd]);
                    pEnd->bEnd = bEnd;
                    pEnd->bIsTx = bTx;
                    pEnd->wMaxPacketSize = 512;
                    pEnd->wPacketSize = 0;
                    pEnd->bIsSharedFifo = FALSE;
                    /* reset the softstate */
                    spin_lock_init(&pEnd->Lock);
                    pEnd->pCurrentUrb = NULL;
                    INIT_LIST_HEAD(&pEnd->list);

                    pEnd->wPacketSize = 0;
                    pEnd->bRemoteAddress = 0;
                    pEnd->bRemoteEnd = 0;
                    pEnd->bTrafficType = 0;
                }
            }
        }
        else
        {
            // EP1 : 512B(IN) / 512B(OUT)
            // EP2 : 1024B(IN) / 0B(OUT)
            // EP3 : 1024B(IN) / 0B(OUT)
            for (bEnd = 1; bEnd <= 1; bEnd++)
            {
                MGC_SelectEnd(pBase, bEnd);
                // fifo size = 512, turn off double packet buffer.
                MGC_Write8(pBase, MGC_O_HDRC_TXFIFOSZ, 6);
                MGC_Write8(pBase, MGC_O_HDRC_RXFIFOSZ, 6);
                MGC_Write16(pBase, MGC_O_HDRC_TXFIFOADD, wFifoOffset >> 3);
                DBG(3, "[USB] bEnd = %d, TX, fifo addr=%d.\n", bEnd, wFifoOffset);
                wFifoOffset += 512;
                MGC_Write16(pBase, MGC_O_HDRC_RXFIFOADD, wFifoOffset >> 3);
                DBG(3, "[USB] bEnd = %d, RX, fifo addr=%d.\n", bEnd, wFifoOffset);
                wFifoOffset += 512;
                pThis->wEndMask |= (1 << bEnd);

                for (bTx = EPRX; bTx <= EPTX; bTx++)
                {
                    pEnd = &(pThis->aLocalEnd[bTx][bEnd]);
                    pEnd->bEnd = bEnd;
                    pEnd->bIsTx = bTx;
                    pEnd->wMaxPacketSize = 512;
                    pEnd->wPacketSize = 0;
                    pEnd->bIsSharedFifo = FALSE;
                    /* reset the softstate */
                    spin_lock_init(&pEnd->Lock);
                    pEnd->pCurrentUrb = NULL;
                    INIT_LIST_HEAD(&pEnd->list);

                    pEnd->wPacketSize = 0;
                    pEnd->bRemoteAddress = 0;
                    pEnd->bRemoteEnd = 0;
                    pEnd->bTrafficType = 0;
                }
            }

            for (bEnd = 2; bEnd <= 3; bEnd++)
            {
                MGC_SelectEnd(pBase, bEnd);
                MGC_Write8(pBase, MGC_O_HDRC_TXFIFOSZ, 0);
                MGC_Write8(pBase, MGC_O_HDRC_RXFIFOSZ, 0x7);
                MGC_Write16(pBase, MGC_O_HDRC_TXFIFOADD, wFifoOffset >> 3);
                DBG(3, "[USB] bEnd = %d, TX, fifo addr=%d.\n", bEnd, wFifoOffset);
                MGC_Write16(pBase, MGC_O_HDRC_RXFIFOADD, wFifoOffset >> 3);
                DBG(3, "[USB] bEnd = %d, RX, fifo addr=%d.\n", bEnd, wFifoOffset);
                wFifoOffset += 1024;
                pThis->wEndMask |= (1 << bEnd);

                for (bTx = EPRX; bTx <= EPTX; bTx++)
                {
                    if (bTx == EPRX)
                    {
                        pEnd = &(pThis->aLocalEnd[bTx][bEnd]);
                        pEnd->bEnd = bEnd;
                        pEnd->bIsTx = bTx;
                        pEnd->wMaxPacketSize = 1024;
                        pEnd->wPacketSize = 0;
                        pEnd->bIsSharedFifo = FALSE;
                        /* reset the softstate */
                        spin_lock_init(&pEnd->Lock);
                        pEnd->pCurrentUrb = NULL;
                        INIT_LIST_HEAD(&pEnd->list);

                        pEnd->wPacketSize = 0;
                        pEnd->bRemoteAddress = 0;
                        pEnd->bRemoteEnd = 0;
                        pEnd->bTrafficType = 0;
                    }
                    else
                    {
                        pEnd = &(pThis->aLocalEnd[bTx][bEnd]);
                        pEnd->bEnd = bEnd;
                        pEnd->bIsTx = bTx;
                        pEnd->wMaxPacketSize = 0;
                        pEnd->wPacketSize = 0;
                        pEnd->bIsSharedFifo = FALSE;
                        /* reset the softstate */
                        spin_lock_init(&pEnd->Lock);
                        pEnd->pCurrentUrb = NULL;
                        INIT_LIST_HEAD(&pEnd->list);

                        pEnd->wPacketSize = 0;
                        pEnd->bRemoteAddress = 0;
                        pEnd->bRemoteEnd = 0;
                        pEnd->bTrafficType = 0;
                    }
                }
            }
        }
    #else 
        /* take care of the remaining eps */
        // Original case, assign 1KB for each endopint, 512 for IN, 512 for OUT.
        for (bEnd = 1; bEnd < pThis->bEndCount; bEnd++)
        {
            MGC_SelectEnd(pBase, bEnd);
            // fifo size = 512, turn off double packet buffer.
            MGC_Write8(pBase, MGC_O_HDRC_TXFIFOSZ, 6);
            MGC_Write8(pBase, MGC_O_HDRC_RXFIFOSZ, 6);
            MGC_Write16(pBase, MGC_O_HDRC_TXFIFOADD, wFifoOffset >> 3);
            DBG(3, "[USB] bEnd = %d, TX, fifo addr=%d.\n", bEnd, wFifoOffset);
            wFifoOffset += 512;
            MGC_Write16(pBase, MGC_O_HDRC_RXFIFOADD, wFifoOffset >> 3);
            DBG(3, "[USB] bEnd = %d, RX, fifo addr=%d.\n", bEnd, wFifoOffset);
            wFifoOffset += 512;
            pThis->wEndMask |= (1 << bEnd);

            for (bTx = EPRX; bTx <= EPTX; bTx++)
            {
                pEnd = &(pThis->aLocalEnd[bTx][bEnd]);
                pEnd->bEnd = bEnd;
                pEnd->bIsTx = bTx;
                pEnd->wMaxPacketSize = 512;
                pEnd->wPacketSize = 0;
                pEnd->bIsSharedFifo = FALSE;
                /* reset the softstate */
                spin_lock_init(&pEnd->Lock);
                pEnd->pCurrentUrb = NULL;
                INIT_LIST_HEAD(&pEnd->list);

                pEnd->wPacketSize = 0;
                pEnd->bRemoteAddress = 0;
                pEnd->bRemoteEnd = 0;
                pEnd->bTrafficType = 0;
            }
        }
    #endif 

}

static void MGC_DelTimer(MGC_LinuxCd *pThis)
{
    if ( timer_pending(&pThis->Timer) )
        del_timer_sync((&pThis->Timer)); /* make sure another timer is not running */
}

static void MGC_SetTimer(MGC_LinuxCd *pThis, void(*pfFunc)(unsigned long), 
                         uint32_t pParam, uint32_t millisecs)
{
    if ( timer_pending(&pThis->Timer) )
        del_timer_sync((&pThis->Timer)); /* make sure another timer is not running */

    pThis->Timer.function = pfFunc;
    pThis->Timer.data = (unsigned long)pParam;
    pThis->Timer.expires = jiffies + (HZ *millisecs) / 1000;
    add_timer(&(pThis->Timer));
}

// lawrance.liu@mediatek.com 2010/05/25 Patched from BDP
// This is to handle invalide urb when after device disconnect.
static void MGC_UnlinkInvalidUrb(unsigned long pParam)
{
    MGC_LinuxCd *pThis = (MGC_LinuxCd*)pParam;
    struct usb_hcd *hcd = musbstruct_to_hcd(pThis);
    int i, j, k = 0;
    struct urb *pUrb = NULL;

    if (!pThis)
    {
        return ;
    } 

    if (!hcd)
    {
        return ;
    }

    #if (0)
    if (!hcd->self.root_hub->children[0] && !pThis->bInsert)
    {
        MGC_DelTimer(pThis);
        return ;
    }
    #endif 

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < pThis->bEndCount; j++)
        {
            pUrb = MGC_GetCurrentUrb(&(pThis->aLocalEnd[i][j]));
            if (!pUrb)
            {
                continue;
            }
            //if(usb_pipecontrol(pUrb->pipe))
            if (usb_pipecontrol(pUrb->pipe) || usb_pipebulk(pUrb->pipe))
            {
                if (pUrb->setup_packet)
                {
                    printk("[USB] to be unlink request is 0x%02X\n", ((struct usb_ctrlrequest*)(pUrb->setup_packet))->bRequest);
                    //[DTV00093006]leon2010-8-21:hub with 6 device plug in, then quickly plugout, the device can not disappper.
                    if (usb_pipebulk(pUrb->pipe) &(j == 1))
                    {
                        MUC_urb_dequeue(hcd, pUrb,  - EFAULT);
                        continue;
                    } 
                }
                if ((uint32_t)pUrb->dev == (uint32_t)hcd->self.root_hub->children[0] || (uint32_t)pUrb->dev->parent == (uint32_t)hcd->self.root_hub->children[0] || (uint32_t)pUrb->dev->parent == (uint32_t)hcd->self.root_hub)
                {
                    MUC_urb_dequeue(hcd, pUrb,  - EFAULT);
                    continue;
                }
                if (!hcd->self.root_hub->children[0])
                {
                    continue;
                }
                for (k = 0; k < hcd->self.root_hub->children[0]->maxchild; k++)
                {
                    if ((uint32_t)hcd->self.root_hub->children[0]->children[k] == (uint32_t)pUrb->dev)
                    {
                        MUC_urb_dequeue(hcd, pUrb,  - EFAULT);
                        continue;
                    }
                }
            }
        }
    }

    if (!pThis->bInsert)
    {
        MGC_SetTimer(pThis, MGC_UnlinkInvalidUrb, (unsigned long)pThis, 20);
    }
}

static void MGC_StartTx(MGC_LinuxCd *pThis, uint8_t bEnd)
{
    uint16_t wCsr;

    uint8_t *pBase = (uint8_t*)pThis->pRegs;

    MGC_CheckTxCsrValidForWrite(pBase, bEnd);

    if (bEnd)
    {
        wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd);

        wCsr |= MGC_M_TXCSR_TXPKTRDY;
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wCsr);
    }
    else
    {
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_H_NO_PING | MGC_M_CSR0_H_SETUPPKT | MGC_M_CSR0_TXPKTRDY);
    }
}

static MGC_LinuxLocalEnd *MGC_FindEnd(MGC_LinuxCd *pThis, struct urb *pUrb)
{
    MGC_LinuxLocalEnd *pEnd = NULL;
    int nEnd =  - 1;
    unsigned int nOut = usb_pipeout(pUrb->pipe);
    uint16_t wPacketSize = usb_maxpacket(pUrb->dev, pUrb->pipe, nOut);
    struct usb_device *dev = pUrb->dev;
    struct usb_host_endpoint *ep;
    struct usb_device_descriptor *pDescriptor = &dev->descriptor;
    struct usb_interface_descriptor *d;

    // lawrance.liu@mediatek.com (2010/05/24)
    uint8_t bAddTransaction = (wPacketSize >> 11) &0x3;
    wPacketSize &= 0x7ff;
    if (bAddTransaction == 1)
    {
        wPacketSize += wPacketSize;
    }
    else if (bAddTransaction == 2)
    {
        wPacketSize += wPacketSize + wPacketSize;
    }

    DBG(2, "[USB] <== pUrb=%p\n", pUrb);

    /* control is always EP0, and can always be queued */
    if (usb_pipecontrol(pUrb->pipe))
    {
        DBG(2, "[USB] ==> is a control pipe use ep0\n");

        pEnd = &(pThis->aLocalEnd[EP0][0]); /*0: Rx, 1: Tx*/
        return pEnd;
    } 

    /* Other EPx */
    ep = (nOut ? dev->ep_out: dev->ep_in)[usb_pipeendpoint(pUrb->pipe)];
    if (!ep)
    {
        return NULL;
    }

    if (ep->hcpriv)
    {
        pEnd = (MGC_LinuxLocalEnd*)ep->hcpriv;

        if (!pEnd->dev && dev)
        {
            pEnd->dev = dev; //this local ep should be occupied by check_free_ep
        }
        return pEnd;
    }

    #if defined(USB_WEBCAM_LOG) || defined(USB_WEBCAM_MAX) || defined(USB_WEBCAM_AT) || defined(USB_WEBCAM_COM)
        if (MUSB_BASE == (uint32_t)pThis->pRegs)
    #endif 
    {
        /* use a reserved one for bulk if any */
        if (usb_pipebulk(pUrb->pipe))
        {
            d = &dev->actconfig->interface[0]->cur_altsetting->desc;

            if ( (pDescriptor->bDeviceClass == USB_CLASS_MASS_STORAGE) || 
                 (d->bInterfaceClass == USB_CLASS_MASS_STORAGE) )
            {
                nEnd = (pThis->bEndCount) - 1;
                // use for mass storage device.
                pEnd = &(pThis->aLocalEnd[nOut][nEnd]); /*0: Rx, 1: Tx*/
                ep->hcpriv = (void*)pEnd;
                /*
                        printk("[USB] Dev=0x%X, use h/w %s ep%d, proto=%s.\n", 
                        (uint32_t)dev, (nOut ? "Out": "In"), nEnd, MGC_DecodeUrbProtocol(pUrb));
                */
                return pEnd;
            }
        }
    }


    if ((MUSB_BASE1 == (uint32_t)pThis->pRegs) && usb_pipeint(pUrb->pipe))
    {
        #if defined(USB_WEBCAM_LOG)
            if (pDescriptor->idVendor == 0x046D && pDescriptor->idProduct == 0x080C)
        #elif defined(USB_WEBCAM_AT)
            if (pDescriptor->idVendor == 0x114D && pDescriptor->idProduct == 0x8C00)
        #elif defined(USB_WEBCAM_MAX)
            if (pDescriptor->idVendor == 0x0B6A && pDescriptor->idProduct == 0x4D51)
        #elif defined(USB_WEBCAM_COM)
            if (0)
        #else 
            if (0)
        #endif 
        {
            printk("[USB] Don't support interrupt transfer on USB camera port !\n");
            return NULL;
        }
    }

    /* scan, remembering exact match and best match */
    for (nEnd = 1; nEnd <= (pThis->bEndCount - 1); nEnd++)
    {
        // The last EP is reserved for MSD
        // When camera is supported on port2, we don't reserve EP for MSD on port2.
        if (nEnd == (pThis->bEndCount - 1))
        {
            if (MUSB_BASE == (uint32_t)pThis->pRegs)
            {
                break;
            }

            if (MUSB_BASE1 == (uint32_t)pThis->pRegs)
            {
                #if defined(USB_WEBCAM_LOG) || defined(USB_WEBCAM_MAX) || defined(USB_WEBCAM_AT) || defined(USB_WEBCAM_COM)
                    // do nothing, last EP is not reserved for MSD on port2.
                #else 
                    break;
                #endif 
            }
        }

        pEnd = &(pThis->aLocalEnd[nOut][nEnd]);
        if ((pEnd->dev == NULL) && (pEnd->wMaxPacketSize >= wPacketSize))
        {
            MUSB_ASSERT(pEnd->pCurrentUrb == NULL);
            MUSB_ASSERT(list_empty(&pEnd->list) == TRUE);

            // Occupy this endpoint.
            pEnd->dev = dev;
            ep->hcpriv = (void*)pEnd;

            /*
                    printk("[USB] Dev=0x%X, use h/w %s ep%d, proto=%s.\n", 
                    (uint32_t)dev, (nOut ? "Out": "In"), nEnd, MGC_DecodeUrbProtocol(pUrb));
                 */
            return pEnd;
        }
    }

    printk("[USB] Err: Can't find HW EP to use : wMaxPacketSize=%d !\n", wPacketSize);
    return NULL;
}

static void MGC_CallbackUrb(MGC_LinuxCd *pThis, struct urb *pUrb)
{
    if (pUrb->status)
    {
        DBG(1, "[USB]Err: urb=0x%p,status=%d\n", pUrb, pUrb->status);
    } 

    usb_hcd_giveback_urb(musbstruct_to_hcd(pThis), pUrb, pUrb->status);

    if (pUrb->status)
    {
        DBG(1, "[USB]done completing pUrb=%p\n", pUrb);
    }
}

static int MGC_CompleteUrb(MGC_LinuxCd *pThis, MGC_LinuxLocalEnd *pEnd, struct urb *pUrb)
{
    int status;

    DBG(2, "[USB] Completing URB=0x%p, on pEnd->bEnd=%d status=%d, proto=%s\n", pUrb, pEnd->bEnd, pUrb->status, MGC_DecodeUrbProtocol(pUrb));

    /* prevents locking&kickstarting */
    pEnd->bBusyCompleting = 1;

    // Unlock pEnd to prevent dead lock with user's submit urb in callback.
//    spin_unlock(&pEnd->Lock);

    //printk("Complete pUrb=0x%X.\n", (uint32_t)pUrb);

    MGC_CallbackUrb(pThis, pUrb);

    // Lock pEnd to prevent race condition with other thread.
//    spin_lock(&pEnd->Lock);

    //   check if there is queued urb before kickstart next. 
    status = MGC_IsEndIdle(pEnd); /* kickstart next */

    /* allows locking&kickstarting again */
    pEnd->bBusyCompleting = 0;

    return status;
} 

static inline uint8_t MGC_GetTransferType(struct urb *pUrb)
{
    uint8_t bStdType = 0;

    const unsigned int nPipe = pUrb->pipe;

    if (usb_pipeisoc(nPipe))
    {
        bStdType = 1;
    } 
    else if (usb_pipeint(nPipe))
    {
        bStdType = 3;
    }
    else if (usb_pipebulk(nPipe))
    {
        bStdType = 2;
    }

    return bStdType;
}

static void MGC_SetProtocol(MGC_LinuxCd *pThis, struct urb *pUrb, uint8_t bEnd, unsigned int bXmt)
{
    uint8_t reg;

    uint8_t bStdType = MGC_GetTransferType(pUrb);
    unsigned int nPipe = pUrb->pipe;
    uint8_t *pBase = (uint8_t*)pThis->pRegs;

    reg = (bStdType << 4) | (((uint8_t)usb_pipeendpoint(nPipe)) &0xf);

    switch (((uint8_t)pUrb->dev->speed))
    {
        case USB_SPEED_LOW:
            reg |= 0xc0;
            break;

        case USB_SPEED_FULL:
            reg |= 0x80;
            break;

        default:
            reg |= 0x40;
    }

    if (bXmt)
    {
        if (bEnd)
        {
            MGC_WriteCsr8(pBase, MGC_O_HDRC_TXTYPE, bEnd, reg);
        }
        else
        {
            MGC_WriteCsr8(pBase, MGC_O_HDRC_TYPE0, 0, reg &0xc0);
        }
    }
    else
    {
        if (bEnd)
        {
            // Note that although we invoke WriteCsr8, but it actually write 32bits.
            // Writing to MGC_O_HDRC_RXTYPE will actually affect MGC_O_HDRC_RXINTERVAL.
            // Therefore, here we have a check to avoid iso transfer interval be affected.
            if (usb_pipeisoc(nPipe))
            {
                uint8_t regvalue;
                regvalue = MGC_ReadCsr8(pBase, MGC_O_HDRC_RXTYPE, bEnd);
                if ((regvalue | 0xC0) != (reg | 0xC0))
                {
                    MGC_WriteCsr8(pBase, MGC_O_HDRC_RXTYPE, bEnd, reg);
                }
            }
            else
            {
                MGC_WriteCsr8(pBase, MGC_O_HDRC_RXTYPE, bEnd, reg);
            }
        }
        else
        {
            MGC_WriteCsr8(pBase, MGC_O_HDRC_TYPE0, 0, reg &0xc0);
        }
    }
}

static void MGC_SetAddress(MGC_LinuxCd *pThis, struct urb *pUrb, uint8_t bEnd, unsigned int bXmt)
{
    uint8_t *pBase = (uint8_t*)pThis->pRegs;

    uint8_t bAddress = (uint8_t)usb_pipedevice(pUrb->pipe);
    uint8_t bHubAddr = 0, bHubPort = 0, bIsMulti = FALSE;

    /* NOTE: there is always a parent due to the virtual root hub */
    if (pUrb->dev->parent)
    {
        bHubAddr = (uint8_t)pUrb->dev->parent->devnum;
    } 

    if (bIsMulti)
    {
        bHubAddr |= 0x80;
    }

    /* target addr & hub addr/port */
    if (bXmt)
    {
        MGC_Write8(pBase, MGC_BUSCTL_OFFSET(bEnd, MGC_O_MHDRC_TXFUNCADDR), bAddress);
        MGC_Write8(pBase, MGC_BUSCTL_OFFSET(bEnd, MGC_O_MHDRC_TXHUBADDR), bHubAddr);
        MGC_Write8(pBase, MGC_BUSCTL_OFFSET(bEnd, MGC_O_MHDRC_TXHUBPORT), bHubPort);
    }

    /* also, try Rx (this is a bug ion the core: I always need to to do 
     * both (at least for ep0), needs to be changed when the core is
     * fixed */
    if ((bEnd == 0) || (!bXmt))
    {
        MGC_Write8(pBase, MGC_BUSCTL_OFFSET(bEnd, MGC_O_MHDRC_RXFUNCADDR), bAddress);
        MGC_Write8(pBase, MGC_BUSCTL_OFFSET(bEnd, MGC_O_MHDRC_RXHUBADDR), bHubAddr);
        MGC_Write8(pBase, MGC_BUSCTL_OFFSET(bEnd, MGC_O_MHDRC_RXHUBPORT), bHubPort);
    }

    DBG(2, "[USB] end %d, device %d, parent %d, port %d, multi-tt: %d, speed:%d\n", bEnd, pUrb->dev->devnum, bHubAddr, bHubPort, bIsMulti, pUrb->dev->speed);
}

static inline uint8_t *MGC_GetUrbBuffer(struct urb *pUrb)
{
    uint8_t *pBuffer = NULL;

    pBuffer = pUrb->transfer_buffer;

    if (!pBuffer)
    {
        pBuffer = (void*)phys_to_virt(pUrb->transfer_dma);
    } 

    return pBuffer;
}

static void MGC_LoadFifo(const uint8_t *pBase, uint8_t bEnd, uint16_t wCount, const uint8_t *pSource)
{
    uint32_t dwDatum = 0;

    uint32_t dwCount = wCount;
    uint8_t bFifoOffset = MGC_FIFO_OFFSET(bEnd);
    uint32_t dwBufSize = 4;

    DBG(2, "[USB] pBase=%p, bEnd=%d, wCount=0x%04x, pSrc=%p\n", pBase, bEnd, wCount, pSource);

    #ifdef MUSB_PARANOID
    if (IS_INVALID_ADDRESS(pSource))
    {
        ERR("[USB] loading fifo from a null buffer; why did u do that????\n");

        return ;
    }
    #endif 

    //  do not handle zero length data.
    if (dwCount == 0)
    {
        return ;
    }

    /* byte access for unaligned */
    if ((dwCount > 0) && ((uint32_t)pSource &3))
    {
        while (dwCount)
        {
            #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
             if (3 == dwCount || 2 == dwCount)
             {
                 dwBufSize = 2;             
                 // set FIFO byte count.
                 MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 1);
             }
            else if(1 == dwCount)
            {
                 dwBufSize = 1;             
                 // set FIFO byte count.
                 MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 0);               
            }
             
             memcpy((void *)&dwDatum, (const void *)pSource, dwBufSize);
             
             MGC_Write32(pBase, bFifoOffset, dwDatum);
             
             dwCount -= dwBufSize;
             pSource += dwBufSize;
            #else
            if (dwCount < 4)
            {
                dwBufSize = dwCount;

                // set FIFO byte count.
                MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, dwCount);
            }

            memcpy((void*) &dwDatum, (const void*)pSource, dwBufSize);

            MGC_Write32(pBase, bFifoOffset, dwDatum);

            dwCount -= dwBufSize;
            pSource += dwBufSize;
            #endif
        }
    }
    else /* word access if aligned */
    {
        while ((dwCount > 3) && !((uint32_t)pSource &3))
        {
            MGC_Write32(pBase, bFifoOffset, *((uint32_t*)((void*)pSource)));

            pSource += 4;
            dwCount -= 4;
        }
        #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
        if (3 == dwCount || 2 == dwCount)
        {
            MUSB_ASSERT(dwCount < 4);
    
            // set FIFO byte count.
            MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 1);
    
            MGC_Write32(pBase, bFifoOffset, *((uint32_t *)((void *)pSource)));
            dwCount -= 2;
            pSource += 2;
        }
        
        if(1 == dwCount)
        {
            MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 0);
            if((uint32_t)pSource & 3)
            {
            
                memcpy((void *)&dwDatum, (const void *)pSource, 1);
                
                MGC_Write32(pBase, bFifoOffset, dwDatum);            
            }
            else
            {
                MGC_Write32(pBase, bFifoOffset, *((uint32_t *)((void *)pSource)));            
            }
        }
        #else
        if (dwCount > 0)
        {
            // set FIFO byte count.
            MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, dwCount);

            MGC_Write32(pBase, bFifoOffset, *((uint32_t*)((void*)pSource)));
        }
        #endif
    }
    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
        MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 2);
    #else
        MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 4);
    #endif
    return ;
}

void MGC_UnloadFifo(const uint8_t *pBase, uint8_t bEnd, uint16_t wCount, uint8_t *pDest)
{
    uint32_t dwDatum = 0;

    uint32_t dwCount = wCount;
    uint8_t bFifoOffset = MGC_FIFO_OFFSET(bEnd);
    uint32_t i;

    DBG(2, "[USB] pBase=%p, bEnd=%d, wCount=0x%04x, pDest=%p\n", pBase, bEnd, wCount, pDest);

    //  do not handle zero length data.
    if (dwCount == 0)
    {
        return ;
    }

    if (((uint32_t)pDest) &3)
    {
        /* byte access for unaligned */
        while (dwCount > 0)
        {
            if (dwCount < 4)
            {            
                #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
                if(3 == dwCount || 2 == dwCount)
                {
                   MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 1);
                   dwDatum = MGC_Read32(pBase, bFifoOffset);
                   
                   for (i = 0; i < 2; i++)
                   {
                       *pDest++ = ((dwDatum >> (i *8)) & 0xFF);
                   }                   
                   dwCount -=2;
                }
                if(1 == dwCount) 
                {
                    MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 0);
                    dwDatum = MGC_Read32(pBase, bFifoOffset);
                    *pDest++ = (dwDatum  & 0xFF);
                    dwCount -= 1;
                }    
                
                // set FIFO byte count = 4 bytes.
                MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 2);
                dwCount = 0;
                #else
                // set FIFO byte count.
                MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, dwCount);

                dwDatum = MGC_Read32(pBase, bFifoOffset);

                for (i = 0; i < dwCount; i++)
                {
                    *pDest++ = ((dwDatum >> (i *8)) &0xFF);
                }

                // set FIFO byte count = 4 bytes.
                MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 4);
                dwCount = 0;
                #endif
            }
            else
            {
                dwDatum = MGC_Read32(pBase, bFifoOffset);

                for (i = 0; i < 4; i++)
                {
                    *pDest++ = ((dwDatum >> (i *8)) &0xFF);
                }

                dwCount -= 4;
            }
        }
    }
    else
    {
        /* word access if aligned */
        while (dwCount >= 4)
        {
            *((uint32_t*)((void*)pDest)) = MGC_Read32(pBase, bFifoOffset);

            pDest += 4;
            dwCount -= 4;
        }

        if (dwCount > 0)
        {    
            #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
            if(3 == dwCount ||2 == dwCount )
            {
                MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 1);
                dwDatum = MGC_Read32(pBase, bFifoOffset);
                for (i = 0; i < 2; i++)
                 {
                     *pDest++ = ((dwDatum >> (i *8)) & 0xFF);
                 }
                dwCount -= 2;
            }

            if(1 == dwCount)
            {
               MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT,0);
               dwDatum = MGC_Read32(pBase, bFifoOffset);
               
                *pDest++ = (dwDatum & 0xFF);
                dwCount -= 1;               
            }
                
            // set FIFO byte count = 4 bytes.
            MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 2);
            #else
            // set FIFO byte count.
            MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, dwCount);

            dwDatum = MGC_Read32(pBase, bFifoOffset);

            for (i = 0; i < dwCount; i++)
            {
                *pDest++ = ((dwDatum >> (i *8)) &0xFF);
            }

            // set FIFO byte count = 4 bytes.
            MGC_FIFO_CNT(pBase, M_REG_FIFOBYTECNT, 4);
            #endif
            }
    }

    return ;
}

static uint32_t MGC_Log2(uint32_t x)
{
    uint32_t i;

    for (i = 0; x > 1; i++)
    {
        x = x / 2;
    }

    return i;
}

static void MGC_SetInterval(MGC_LinuxCd *pThis, struct urb *pUrb, uint8_t bEnd)
{
    uint8_t *pBase = (uint8_t*)pThis->pRegs;

    unsigned int nPipe = pUrb->pipe;
    uint8_t bInterval = 0;

    if (usb_pipeint(nPipe))
    {
        if (pThis->bRootSpeed == 1) /* high speed */
        {
            bInterval = MGC_Log2(pUrb->interval) + 1;
        } 
        else /* full or low speed */
        {
            bInterval = pUrb->interval;
        }
    }
    else if (usb_pipeisoc(nPipe))
    {
        if (pThis->bRootSpeed == 1) /* high speed */
        {
            //            bInterval = MGC_Log2(pUrb->interval) + 1;
            bInterval = 4;
        }
        else if (pThis->bRootSpeed == 2) /* full speed */
        {
            //            bInterval = MGC_Log2(pUrb->interval) + 1;
            bInterval = 1;
        }
    }
    else if (usb_pipebulk(nPipe) && (pUrb->interval > 0))
    {
        if (pThis->bRootSpeed == 1) /* high speed */
        {
            if (pUrb->interval > 4096)
            {
                bInterval = 16;
            }
            else
            {
                bInterval = MGC_Log2(pUrb->interval) + 1;
            }
        }
        else if (pThis->bRootSpeed == 2) /* full speed */
        {
            if (pUrb->interval > 32768)
            {
                bInterval = 16;
            }
            else
            {
                bInterval = MGC_Log2(pUrb->interval) + 1;
            }
        }
    }

    if (usb_pipeout(nPipe))
    {
        MGC_WriteCsr8(pBase, MGC_O_HDRC_TXINTERVAL, bEnd, bInterval);
    }
    else
    {
        // Everytime when we write to MGC_O_HDRC_RXINTERVAL, there would be 1 frame/micro-frame delay.
        // MGC_O_HDRC_RXINTERVAL should be written only when we want to change interval.
        if (usb_pipeisoc(nPipe))
        {
            uint8_t regvalue;
            regvalue = MGC_ReadCsr8(pBase, MGC_O_HDRC_RXINTERVAL, bEnd);
            if (regvalue != bInterval)
            {
                MGC_WriteCsr8(pBase, MGC_O_HDRC_RXINTERVAL, bEnd, bInterval);
            }
        }
        else
        {
            MGC_WriteCsr8(pBase, MGC_O_HDRC_RXINTERVAL, bEnd, bInterval);
        }
    }
}

/**
 * used ONLY in host mode, I'll be moved to musb_host
 * @param pPrivateData
 * @param bLocalEnd
 * @param bTransmit
 */
static uint8_t MGC_HsDmaChannelStatusChanged(void *pPrivateData, uint8_t bLocalEnd, uint8_t bTransmit)
{
    MGC_LinuxCd *pThis = (MGC_LinuxCd*)pPrivateData;

    if (!bLocalEnd)
    {
        MGC_ServiceDefaultEnd(pThis);
    }
    else
    {
        /* endpoints 1..15 */
        if (bTransmit)
        {
            MGC_ServiceTxAvail(pThis, bLocalEnd);
        }
        else
        {
            /* receive */
            MGC_ServiceRxReady(pThis, bLocalEnd);
        }
    }

    return TRUE;
}

static MGC_DmaChannel *MGC_HsDmaAllocateChannel(void *pPrivateData, uint8_t bLocalEnd, 
                                                uint8_t bTransmit, uint8_t bProtocol, 
                                                uint16_t wMaxPacketSize)
{
    uint8_t bBit;

    MGC_DmaChannel *pChannel = NULL;
    MGC_HsDmaChannel *pImplChannel = NULL;
    MGC_HsDmaController *pController = (MGC_HsDmaController*)pPrivateData;

    for (bBit = 0; bBit < MGC_HSDMA_CHANNELS; bBit++)
    {
        if (!(pController->bmUsedChannels &(1 << bBit)))
        {
            pController->bmUsedChannels |= (1 << bBit);

            pImplChannel = &(pController->aChannel[bBit]);
            pImplChannel->pController = pController;
            pImplChannel->wMaxPacketSize = wMaxPacketSize;
            pImplChannel->bIndex = bBit;
            pImplChannel->bEnd = bLocalEnd;
            pImplChannel->bProtocol = bProtocol;
            pImplChannel->bTransmit = bTransmit;
            pChannel = &(pImplChannel->Channel);
            pChannel->pPrivateData = pImplChannel;
            pChannel->bStatus = MGC_DMA_STATUS_FREE;
            pChannel->dwMaxLength = 0x10000;
            /* Tx => mode 1; Rx => mode 0 */
            pChannel->bDesiredMode = bTransmit;
            break;
        }
    }

    return pChannel;
}

static void MGC_HsDmaReleaseChannel(MGC_DmaChannel *pChannel)
{
    MGC_HsDmaChannel *pImplChannel = (MGC_HsDmaChannel*)pChannel->pPrivateData;

    MGC_HsDmaController *pController = pImplChannel->pController;
    uint8_t *pBase = pController->pCoreBase;
    uint8_t bChannel = pImplChannel->bIndex;
    uint16_t wCsr = 0;
    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)    
    uint32_t bIntr = MGC_Read8(pBase, MGC_O_HSDMA_INTR);
    if(bIntr & (1<< bChannel))
    {
        //  write clear interrupt register value.
        MGC_Write8(pBase, MGC_O_HSDMA_INTR, (1 << bChannel));
    }
    #else
    uint32_t dwIntr = MGC_Read32(pBase, MGC_O_HSDMA_INTR);

    if (dwIntr &(1 << bChannel))
    {
        //  write clear interrupt register value.
        MGC_Write32(pBase, MGC_O_HSDMA_INTR, (1 << bChannel));
    }
    #endif

    MGC_Write32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL), 0);

    // clear TX/RX CSR register.
    if (pImplChannel->bTransmit)
    {
        wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, pImplChannel->bEnd);

        if (wCsr &(MGC_M_TXCSR_AUTOSET | MGC_M_TXCSR_DMAENAB | MGC_M_TXCSR_DMAMODE))
        {
            wCsr &= ~(MGC_M_TXCSR_AUTOSET | MGC_M_TXCSR_DMAENAB | MGC_M_TXCSR_DMAMODE);

            MGC_CheckTxCsrValidForWrite(pBase, pImplChannel->bEnd);
            MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, pImplChannel->bEnd, wCsr);
        }
    }
    else
    {
        wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, pImplChannel->bEnd);

        if (wCsr &(MGC_M_RXCSR_AUTOCLEAR | MGC_M_RXCSR_H_AUTOREQ | MGC_M_RXCSR_DMAENAB | MGC_M_RXCSR_DMAMODE))
        {
            wCsr &= ~(MGC_M_RXCSR_AUTOCLEAR | MGC_M_RXCSR_H_AUTOREQ | MGC_M_RXCSR_DMAENAB | MGC_M_RXCSR_DMAMODE);

            MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, pImplChannel->bEnd, wCsr);
        }
    }

    pImplChannel->pController->bmUsedChannels &= ~(1 << pImplChannel->bIndex);
    pImplChannel->Channel.bStatus = MGC_DMA_STATUS_FREE;
}

static uint8_t MGC_HsDmaProgramChannel(MGC_DmaChannel *pChannel, uint16_t wPacketSize, 
                                       uint8_t bMode, const uint8_t *pBuffer, uint32_t dwLength)
{
    MGC_HsDmaChannel *pImplChannel = (MGC_HsDmaChannel*)pChannel->pPrivateData;

    MGC_HsDmaController *pController = pImplChannel->pController;
    uint8_t *pBase = pController->pCoreBase;
    uint32_t dwCsr = (pImplChannel->bEnd << MGC_S_HSDMA_ENDPOINT) | (1 << MGC_S_HSDMA_ENABLE);
    uint8_t bChannel = pImplChannel->bIndex;
    uint16_t wCsr = 0;

    /* reject below threshold (packet size) */
    if (dwLength < MGC_HSDMA_MIN_DMA_LEN)
    {
        printk("[USB] MGC_HsDmaProgramChannel error !\n");
        return FALSE;
    }

    if (bMode)
    {
        dwCsr |= 1 << MGC_S_HSDMA_MODE1;
    }

    if (pImplChannel->bTransmit)
    {
        //  prevent client task and USB HISR race condition, set csr in MGC_HsDmaProgramChannel().
        wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, pImplChannel->bEnd);

        if (bMode)
        {
            wCsr |= (MGC_M_TXCSR_AUTOSET | MGC_M_TXCSR_DMAENAB | MGC_M_TXCSR_DMAMODE);
        }
        else
        {
            wCsr &= ~(MGC_M_TXCSR_AUTOSET | MGC_M_TXCSR_DMAENAB | MGC_M_TXCSR_DMAMODE);
        }

        MGC_CheckTxCsrValidForWrite(pBase, pImplChannel->bEnd);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, pImplChannel->bEnd, wCsr | MGC_M_TXCSR_MODE);
        dwCsr |= 1 << MGC_S_HSDMA_TRANSMIT;
    }
    else
    {
        if (bMode)
        {
            wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, pImplChannel->bEnd);

            wCsr |= (MGC_M_RXCSR_AUTOCLEAR | MGC_M_RXCSR_DMAENAB);
            //  Request the actual number of packet to be received.
            MGC_DMA_Write32(pBase, M_REG_REQPKT(pImplChannel->bEnd), (((dwLength + pImplChannel->wMaxPacketSize) - 1) / pImplChannel->wMaxPacketSize));

            wCsr &= ~MGC_M_RXCSR_RXPKTRDY;
            // host use MGC_M_RXCSR_DMAMODE.
            wCsr |= (MGC_M_RXCSR_H_AUTOREQ | MGC_M_RXCSR_DMAMODE | MGC_M_RXCSR_H_REQPKT);
        }
    }

    dwCsr |= 1 << MGC_S_HSDMA_IRQENABLE;

    /* address/count */
    MGC_Write32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_ADDRESS), (uint32_t)pBuffer);
    MGC_Write32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_COUNT), dwLength);

    /* control (this should start things) */
    pChannel->dwActualLength = 0L;
    pImplChannel->dwStartAddress = (uint32_t)pBuffer;
    pImplChannel->dwCount = dwLength;
    MGC_Write32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL), dwCsr);


    if (!pImplChannel->bTransmit)
    {
        /*
            Note: 
            RxCSR should be set after DMA is configured. 
            This can prevent race condition between setuping DMA and data entering fifo.
             */
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, pImplChannel->bEnd, wCsr);
    }

    return TRUE;
}

static MGC_DmaChannelStatus MGC_HsDmaGetChannelStatus(MGC_DmaChannel *pChannel)
{
    uint32_t dwAddress;

    MGC_HsDmaChannel *pImplChannel = (MGC_HsDmaChannel*)pChannel->pPrivateData;
    MGC_HsDmaController *pController = pImplChannel->pController;
    uint8_t *pBase = pController->pCoreBase;
    uint8_t bChannel = pImplChannel->bIndex;
    uint32_t dwCsr = MGC_Read32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL));
    uint32_t dwCsrMask;

    if (dwCsr &(1 << MGC_S_HSDMA_BUSERROR))
    {
        printk("[USB] MGC_HsDmaGetChannelStatus : MGC_S_HSDMA_BUSERROR !\n");
        return MGC_DMA_STATUS_BUS_ABORT;
    }

    //  handle last short packet in multiple packet DMA RX mode 1.  
    dwCsrMask = (1 << MGC_S_HSDMA_ENABLE) | (1 << MGC_S_HSDMA_MODE1) | (1 << MGC_S_HSDMA_IRQENABLE);

    if ((dwCsr &0xf) == dwCsrMask)
    {
        /* most DMA controllers would update the count register for simplicity... */
        dwAddress = MGC_Read32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_ADDRESS));

        pImplChannel->Channel.dwActualLength = dwAddress - pImplChannel->dwStartAddress;

        MGC_Write32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL), 0);
        return MGC_DMA_STATUS_MODE1_SHORTPKT;
    }

    /* most DMA controllers would update the count register for simplicity... */
    dwAddress = MGC_Read32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_ADDRESS));

    if (dwAddress < (pImplChannel->dwStartAddress + pImplChannel->dwCount))
    {
        printk("[USB] MGC_HsDmaGetChannelStatus : MGC_DMA_STATUS_BUSY !\n");
        return MGC_DMA_STATUS_BUSY;
    }

    return MGC_DMA_STATUS_FREE;
}

static uint8_t MGC_HsDmaControllerIsr(void *pPrivateData)
{
    uint8_t bChannel;

    uint32_t dwCsr;
    uint32_t dwAddress;
    MGC_HsDmaChannel *pImplChannel;
    MGC_HsDmaController *pController = (MGC_HsDmaController*)pPrivateData;
    uint8_t *pBase = pController->pCoreBase;
    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
    uint8_t bIntr = MGC_Read8(pBase, MGC_O_HSDMA_INTR);
    uint8_t bIntrMask = MGC_Read8(pBase, MGC_O_HSDMA_INTR_MASK);
    #else
    uint32_t dwIntr = MGC_Read32(pBase, MGC_O_HSDMA_INTR);
    #endif
    uint8_t bEnd;
    MGC_LinuxCd *pThis = (MGC_LinuxCd*)pController->pDmaPrivate;
    unsigned long flags = 0;
    
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
    bIntr = (bIntr & bIntrMask);
    if (!bIntr)
        return FALSE;
    MGC_Write8(pBase, MGC_O_HSDMA_INTR, bIntr);  
#else
    if (!dwIntr)
    {
        return FALSE;
    }
    //  write clear interrupt register value.
    MGC_Write32(pBase, MGC_O_HSDMA_INTR, dwIntr);
#endif

    for (bChannel = 0; bChannel < MGC_HSDMA_CHANNELS; bChannel++)
    {
    
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
    if ((bIntr &(1 << bChannel)) == 0)
        continue;
#else        
        if ((dwIntr &(1 << bChannel)) == 0)
        {
            continue;
        }
#endif
        pImplChannel = (MGC_HsDmaChannel*) &(pController->aChannel[bChannel]);
        dwCsr = MGC_Read32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL));

        if (dwCsr &(1 << MGC_S_HSDMA_BUSERROR))
        {
            printk("[USB] MGC_HsDmaControllerIsr Bus ERROR !\n");
            pImplChannel->Channel.bStatus = MGC_DMA_STATUS_BUS_ABORT;
        }
        else
        {
            /* most DMA controllers would update the count register for simplicity... */
            dwAddress = MGC_Read32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_ADDRESS));

            pImplChannel->Channel.bStatus = MGC_DMA_STATUS_FREE;
            pImplChannel->Channel.dwActualLength = dwAddress - pImplChannel->dwStartAddress;

            if (pImplChannel->bTransmit)
            {
                //  send last short packet in multiple packet transfer and tx single packet.
                if ( (pImplChannel->dwCount % pImplChannel->wMaxPacketSize) || 
                     (pImplChannel->dwCount <= pImplChannel->wMaxPacketSize) )
                {
                    bEnd = pImplChannel->bEnd;

                    spin_lock_irqsave(&pThis->SelectEndLock, flags);
                    MGC_SelectEnd(pBase, bEnd);
                    MGC_CheckTxCsrValidForWrite(pBase, bEnd);

                    if (bEnd)
                    {
                        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, MGC_M_TXCSR_TXPKTRDY);
                    }
                    else
                    {
                        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_H_NO_PING | MGC_M_CSR0_H_SETUPPKT | MGC_M_CSR0_TXPKTRDY);
                    }
                    spin_unlock_irqrestore(&pThis->SelectEndLock, flags);

                    continue;
                }
            }

            pController->pfDmaChannelStatusChanged(pController->pDmaPrivate, pImplChannel->bEnd, pImplChannel->bTransmit);
        }
    }

    return TRUE;
}

static uint8_t MGC_TxPacket(MGC_LinuxCd *pThis, MGC_LinuxLocalEnd *pEnd)
{
    uint16_t wLength = 0;
    uint8_t bDone = FALSE;
    uint8_t *pBase;
    struct urb *pUrb;
    uint8_t *pBuffer;
    int nPipe;
    uint8_t bEnd;
    int i;
    struct usb_iso_packet_descriptor *packet;
    MGC_DmaChannel *pDmaChannel;
    uint8_t *pDmaBuffer;
    uint8_t bDmaOk;
    MGC_DmaChannelStatus DmaChannelStatus = MGC_DMA_STATUS_UNKNOWN;

    if ((!pThis) || (!pEnd))
    {
        return TRUE;
    } 

    pBase = (uint8_t*)pThis->pRegs;
    pUrb = MGC_GetCurrentUrb(pEnd);
    if (!pUrb)
    {
        return TRUE;
    }
    nPipe = pUrb ? pUrb->pipe: 0;
    bEnd = pEnd->bEnd;
    pBuffer = MGC_GetUrbBuffer(pUrb);

    DBG(2, "[USB] <== bEnd=%d\n", bEnd);

    if (!pBuffer)
    {
        // abort the transfer
        ERR("[USB] ***> no buffer was given, BAD things are happening (TM)!\n");
        return TRUE;
    }

    /* see if more transactions are needed */
    if (pEnd->pDmaChannel)
    {
        DmaChannelStatus = pThis->pDmaController->pfDmaGetChannelStatus(pEnd->pDmaChannel);
        if (MGC_DMA_STATUS_FREE == DmaChannelStatus)
        {
            pEnd->dwOffset += pEnd->dwRequestSize;

            if (usb_pipeisoc(nPipe))
            {
                packet = &pUrb->iso_frame_desc[pEnd->dwIsoPacket];
                if (pEnd->dwRequestSize >= packet->length)
                {
                    pEnd->dwIsoPacket++;
                }

                if (pEnd->dwIsoPacket < pUrb->number_of_packets)
                {
                    pDmaBuffer = (uint8_t*)pUrb->transfer_dma;
                    pDmaBuffer += pUrb->iso_frame_desc[pEnd->dwIsoPacket].offset;
                    wLength = pUrb->iso_frame_desc[pEnd->dwIsoPacket].length;

                    pDmaChannel = pEnd->pDmaChannel;
                    if (pDmaChannel)
                    {
                        // Iso is always single packet to send.
                        pDmaChannel->bDesiredMode = 0;
                        pDmaChannel->dwActualLength = 0L;
                        pEnd->dwRequestSize = min((uint32_t)wLength, pDmaChannel->dwMaxLength);
                        bDmaOk = pThis->pDmaController->pfDmaProgramChannel(pDmaChannel, pEnd->wPacketSize, pDmaChannel->bDesiredMode, pDmaBuffer, pEnd->dwRequestSize);
                        if (!bDmaOk)
                        {
                            pThis->pDmaController->pfDmaReleaseChannel(pDmaChannel);
                            pEnd->pDmaChannel = NULL;
                            pEnd->dwRequestSize = 0;
                        }
                        else
                        {
                            return FALSE;
                        }
                    }
                }
                else
                {
                    for (i = 0; i < pUrb->number_of_packets; i++)
                    {
                        packet = &pUrb->iso_frame_desc[i];
                        packet->status = 0;
                        packet->actual_length = packet->length;
                    }
                    // Urb finish.
                    pUrb->status = 0;
                    return TRUE;
                }
            }
        }
        else
        {
            printk("[USB] MGC_TxPacket DMA channel status error !\n");
        }
    }
    else
    {
        pEnd->dwOffset += pEnd->dwRequestSize;
    }

    if (usb_pipeisoc(nPipe))
    {
        /* isoch case */
        if (pEnd->dwIsoPacket >= pUrb->number_of_packets)
        {
            for (i = 0; i < pUrb->number_of_packets; i++)
            {
                packet = &pUrb->iso_frame_desc[i];
                packet->status = 0;
                packet->actual_length = packet->length;
            }

            bDone = TRUE;
        }
        else
        {
            pBuffer += pUrb->iso_frame_desc[pEnd->dwIsoPacket].offset;
            wLength = pUrb->iso_frame_desc[pEnd->dwIsoPacket].length;
            pEnd->dwIsoPacket++;
        }
    }
    else
    {
        pBuffer += pEnd->dwOffset;

        wLength = min((int)pEnd->wPacketSize, (int)(pUrb->transfer_buffer_length - pEnd->dwOffset));

        if (pEnd->dwOffset >= pUrb->transfer_buffer_length)
        {
            /* sent everything; see if we need to send a null */
            if ((pUrb->transfer_flags &USB_ZERO_PACKET) && (pEnd->dwRequestSize > 0) && ((pEnd->dwRequestSize % pEnd->wPacketSize) == 0))
            {
                // send null packet.
                pEnd->dwRequestSize = 0;
                bDone = FALSE;
                printk("[USB] MGC_TxPacket: Send NULL Packet !\n");
            }
            else
            {
                bDone = TRUE;
            }
        }
    }

    if (bDone)
    {
        pUrb->status = 0;
    }
    else if (wLength)
    {
         /* @assert bDone && !wLength */
        MGC_LoadFifo(pBase, bEnd, wLength, pBuffer);
        pEnd->dwRequestSize = wLength;
    }
    else
    {
        printk("[USB] MGC_TxPacket error : 0x%x, 0x%x, 0x%x, 0x%x !\n", pEnd->wPacketSize, pUrb->transfer_buffer_length, pEnd->dwOffset, pEnd->dwRequestSize);
    }

    return bDone;
}

static uint8_t MGC_RxPacket(MGC_LinuxCd *pThis, MGC_LinuxLocalEnd *pEnd, uint16_t wRxCount)
{
    uint16_t wLength;
    uint8_t bDone = FALSE;
    uint8_t *pBase;
    uint8_t bEnd;
    struct urb *pUrb;
    uint8_t *pBuffer;
    int nPipe;
    uint16_t wPacketSize;
    uint8_t bStdType;
    uint8_t bDmaOk = FALSE;
    MGC_DmaChannel *pDmaChannel;
    MGC_DmaController *pDmaController;
    uint8_t *pDmaBuffer;

    if ((!pThis) || (!pEnd))
    {
        return TRUE;
    } 

    pBase = (uint8_t*)pThis->pRegs;
    pUrb = MGC_GetCurrentUrb(pEnd);
    if (!pUrb)
    {
        return TRUE;
    }
    nPipe = pUrb ? pUrb->pipe: 0;
    bEnd = pEnd->bEnd;
    pBuffer = MGC_GetUrbBuffer(pUrb);

    DBG(2, "[USB] <== end %d RxCount=%04x\n", bEnd, wRxCount);

    DBG(3, "[USB] bEnd=%d, pUrb->transfer_flags=0x%x pUrb->transfer_buffer=%p\n", bEnd, pUrb->transfer_flags, pUrb->transfer_buffer);
    DBG(3, "[USB] pUrb->transfer_buffer_length=%d, pEnd->dwOffset=%d, wRxCount=%d\n", pUrb->transfer_buffer_length, pEnd->dwOffset, wRxCount);

    if (!pBuffer)
    {
        // abort the transfer
        ERR("[USB] ***> pBuffer=NULL, BAD things are happening (TM)!\n");
        return TRUE;
    }

    /* unload FIFO */
    if (usb_pipeisoc(nPipe))
    {
        /* isoch case */
        pBuffer += pUrb->iso_frame_desc[pEnd->dwIsoPacket].offset;
        wLength = min((unsigned int)wRxCount, pUrb->iso_frame_desc[pEnd->dwIsoPacket].length);
        pUrb->actual_length += wLength;

        /* update actual & status */
        pUrb->iso_frame_desc[pEnd->dwIsoPacket].actual_length = wLength;
        pUrb->iso_frame_desc[pEnd->dwIsoPacket].status = USB_ST_NOERROR;

        /* see if we are done */
        bDone = (++pEnd->dwIsoPacket >= pUrb->number_of_packets);

        DBG(3, "[USB] pEnd->dwIsoPacket=%d, pUrb->number_of_packets=%d, wLength=%d\n", pEnd->dwIsoPacket, pUrb->number_of_packets, wLength);

        if (wLength)
        {
            MGC_UnloadFifo(pBase, bEnd, wLength, pBuffer);
        }

        if (bEnd && bDone)
        {
            pUrb->status = 0;
        }
    }
    else
    {
        DBG(3, "[USB] (bEnd=%d), wRxCount=%d, pUrb->transfer_buffer_length=%d, pEnd->dwOffset=%d, pEnd->wPacketSize=%d\n", bEnd, wRxCount, pUrb->transfer_buffer_length, pEnd->dwOffset, pEnd->wPacketSize);

        /* non-isoch */
        pBuffer += pEnd->dwOffset;

        wLength = min((unsigned int)wRxCount, pUrb->transfer_buffer_length - pEnd->dwOffset);

        wPacketSize = usb_maxpacket(pUrb->dev, pUrb->pipe, FALSE);

        if ((usb_pipebulk(nPipe)) && (wLength >= wPacketSize))
        {
            // receive fifo data first.
            pUrb->actual_length += wLength;
            pEnd->dwOffset += wLength;
            MGC_UnloadFifo(pBase, bEnd, wLength, pBuffer);

            // still need to receive.
            if (pUrb->transfer_buffer_length > pEnd->dwOffset)
            {
                // clear rxpkt ready before activate dma.
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, 0);

                wLength = pUrb->transfer_buffer_length - pEnd->dwOffset;
                pDmaBuffer = (uint8_t*)pUrb->transfer_dma;
                pDmaBuffer += pEnd->dwOffset;
                pDmaController = pThis->pDmaController;
                pDmaChannel = pEnd->pDmaChannel;

                /* candidate for DMA */
                if (pDmaController && !pDmaChannel)
                {
                    bStdType = MGC_GetTransferType(pUrb);
                    pEnd->pDmaChannel = pDmaController->pfDmaAllocateChannel(pDmaController->pPrivateData, bEnd, FALSE, bStdType, wPacketSize);
                    pDmaChannel = pEnd->pDmaChannel;
                }

                if (pDmaController && pDmaChannel)
                {
                    //  After receive fifo data, we need to set auto request data by mode 1.
                    pDmaChannel->bDesiredMode = 1;

                    pDmaChannel->dwActualLength = 0L;
                    pEnd->dwRequestSize = wLength;
                    bDmaOk = pDmaController->pfDmaProgramChannel(pDmaChannel, wPacketSize, pDmaChannel->bDesiredMode, pDmaBuffer, pEnd->dwRequestSize);
                    if (!bDmaOk)
                    {
                        pDmaController->pfDmaReleaseChannel(pDmaChannel);
                        pEnd->pDmaChannel = NULL;
                        pEnd->dwRequestSize = 0;
                    }
                    else
                    {
                        // process dma, not done.
                        DBG(2, "[USB] ==> bDone=%d\n", FALSE);
                        return FALSE;
                    }
                }
            }
        }
        else if (wLength > 0)
        {
            pUrb->actual_length += wLength;
            pEnd->dwOffset += wLength;

            MGC_UnloadFifo(pBase, bEnd, wLength, pBuffer);
        }

        /* see if we are done */
        bDone = (pEnd->dwOffset >= pUrb->transfer_buffer_length) || (wRxCount < pEnd->wPacketSize);
    }

    MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, 0);

    if (!bDone)
    {
         /* test for short packet */
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, MGC_M_RXCSR_H_REQPKT);
    }

    if (bEnd && bDone)
    {
        pUrb->status = 0;
    }

    DBG(2, "[USB] ==> bDone=%d\n", bDone);
    return bDone;
}

static void MGC_ProgramEnd(MGC_LinuxCd *pThis, MGC_LinuxLocalEnd *pEnd, struct urb *pUrb, uint8_t bXmt, uint8_t *pBuffer, uint32_t dwLength)
{
    uint16_t wIntrTxE;
    uint32_t dwIntrTxE;
    uint8_t *pBase = (uint8_t*)pThis->pRegs;
    unsigned int nPipe = pUrb->pipe;
    uint16_t wPacketSize = usb_maxpacket(pUrb->dev, nPipe, usb_pipeout(nPipe));
    uint8_t bInterval;
    uint8_t bEnd = pEnd->bEnd;
    uint8_t bDmaOk = FALSE;
    MGC_DmaChannel *pDmaChannel;
    MGC_DmaController *pDmaController;
    uint16_t wFrame;
    uint16_t wCount = 0;
    uint16_t wCsr = MGC_M_TXCSR_MODE;
    unsigned long flags = 0;

    spin_lock_irqsave(&pThis->SelectEndLock, flags);
    MGC_SelectEnd(pBase, bEnd);

    // setup packet in control pipe is always out.
    if (usb_pipecontrol(nPipe))
    {
        bXmt = TRUE;
    } 

    MGC_SetProtocol(pThis, pUrb, bEnd, bXmt);
    MGC_SetAddress(pThis, pUrb, bEnd, bXmt);

    if (bXmt) /* transmit */
    {
        //  This disable interrupt will clear INTRRX interrupt event. Because our 32 bits register access mode.
        /* disable interrupt in case we flush */
        wIntrTxE = MGC_Read16(pBase, MGC_O_HDRC_INTRTXE);

        //MGC_Write16(pBase, MGC_O_HDRC_INTRTXE, wIntrTxE & ~(1 << bEnd));
        dwIntrTxE = MGC_Read32(pBase, MGC_O_HDRC_INTRRX);
        dwIntrTxE &= (wIntrTxE &~(uint16_t)(1 << bEnd)) << 16;
        MGC_Write32(pBase, MGC_O_HDRC_INTRRX, dwIntrTxE);

        if (bEnd)
        {
            MGC_CheckTxCsrValidForWrite(pBase, bEnd);
            /* twice in case of double packet buffering */
            MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, MGC_M_TXCSR_CLRDATATOG);
            MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, MGC_M_TXCSR_CLRDATATOG);

            // get data toggle bit from logical ep.
            if (usb_gettoggle(pUrb->dev, pEnd->bRemoteEnd, 1))
            {
                // set data toggle bit to physical ep.
                #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
                MGC_Write32(pBase, M_REG_TXDATATOG, M_REG_SET_DATATOG(bEnd, 1));
                #else
                MGC_MISC_Write32(pBase, M_REG_TXDATATOG, M_REG_SET_DATATOG(bEnd, 1));
                #endif
            }

            /* protocol/endpoint/interval/NAKlimit */
            MGC_WriteCsr16(pBase, MGC_O_HDRC_TXMAXP, bEnd, wPacketSize);

            //  Set interrupt polling interval must depend on high, or low/full speed.
            MGC_SetInterval(pThis, pUrb, bEnd);
        }
        else
        {
            /* endpoint 0: just flush */
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, bEnd, MGC_M_CSR0_FLUSHFIFO);

            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, bEnd, MGC_M_CSR0_FLUSHFIFO);

            bInterval = MGC_Log2(pUrb->interval) + 1;
            bInterval = (bInterval > 16) ? 16 : ((bInterval <= 1) ? 0 : bInterval);

            /* protocol/endpoint/interval/NAKlimit */
            MGC_WriteCsr8(pBase, MGC_O_HDRC_NAKLIMIT0, 0, bInterval);
        }

        /* re-enable interrupt and write CSR to transmit */
        //MGC_Write16(pBase, MGC_O_HDRC_INTRTXE, wIntrTxE);             
        dwIntrTxE = (wIntrTxE) << 16;
        MGC_Write32(pBase, MGC_O_HDRC_INTRRX, dwIntrTxE);

        if (bEnd)
        {
            MGC_CheckTxCsrValidForWrite(pBase, bEnd);
            MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wCsr);
        }

        // Check DMA condition.
        if ((bEnd) && (dwLength >= MGC_HSDMA_MIN_DMA_LEN))
        {
            pDmaController = pThis->pDmaController;

            pDmaChannel = pEnd->pDmaChannel;

            /* candidate for DMA */
            if (pDmaController && !pDmaChannel)
            {
                pDmaChannel = pEnd->pDmaChannel = pDmaController->pfDmaAllocateChannel(pDmaController->pPrivateData, bEnd, TRUE, MGC_GetTransferType(pUrb), wPacketSize);
            }

            if (pDmaController && pDmaChannel)
            {
                //  set DMAReqMode by sending data size.                    
                pDmaChannel->bDesiredMode = (dwLength > wPacketSize) ? 1 : 0;

                pDmaChannel->dwActualLength = 0L;
                pEnd->dwRequestSize = min(dwLength, pDmaChannel->dwMaxLength);
                bDmaOk = pDmaController->pfDmaProgramChannel(pDmaChannel, wPacketSize, pDmaChannel->bDesiredMode, pBuffer, pEnd->dwRequestSize);

                if (!bDmaOk)
                {
                    pDmaController->pfDmaReleaseChannel(pDmaChannel);

                    pEnd->pDmaChannel = NULL;
                    pEnd->dwRequestSize = 0;
                }
            }
        }

        if (!bDmaOk)
        {
            wCount = min((uint32_t)wPacketSize, dwLength);

            if (bEnd)
            {
                MGC_TxPacket(pThis, pEnd);
            }
            else
            {
                if (wCount)
                {
                    pEnd->dwRequestSize = wCount;

                    MGC_LoadFifo(pThis->pRegs, bEnd, wCount, pBuffer);
                }
            }

            /* determine if the time is right for a periodic transfer */
            if (usb_pipeisoc(nPipe) || usb_pipeint(nPipe))
            {
                DBG(3, "[USB] check whether there's still time for periodic Tx\n");

                wFrame = MGC_Read16(pBase, MGC_O_HDRC_FRAME);
                if ((pUrb->transfer_flags &USB_ISO_ASAP) || (wFrame >= pUrb->start_frame))
                {
                    pEnd->dwWaitFrame = 0;

                    MGC_StartTx(pThis, bEnd);
                }
                else
                {
                    pEnd->dwWaitFrame = pUrb->start_frame;
                    /* enable SOF interrupt so we can count down */
                    MGC_Write8(pBase, MGC_O_HDRC_INTRUSBE, 0xff);
                }
            }
            else
            {
                MGC_StartTx(pThis, bEnd);
            }
        }
    }
    else
    {
        /* receive */
        if (pEnd->bIsSharedFifo)
        {
            /* if was programmed for Tx, be sure it is ready for re-use */
            wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd);
            #if !defined(CONFIG_ARCH_MT5396) && !defined(CONFIG_ARCH_MT5368)
            if (wCsr & MGC_M_TXCSR_MODE)
            {
                DBG(1, "[USB] reprogramming ep%d for rx\n", bEnd);

                if (wCsr &MGC_M_TXCSR_FIFONOTEMPTY)
                {
                    MGC_CheckTxCsrValidForWrite(pBase, bEnd);
                    /* this shouldn't happen */
                    MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, MGC_M_TXCSR_FRCDATATOG);

                    MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, MGC_M_TXCSR_FRCDATATOG);

                    ERR("[USB] *** switching end %d to Rx but Tx FIFO not empty\n", bEnd);
                }

                MGC_CheckTxCsrValidForWrite(pBase, bEnd);
                /* clear mode (and everything else) to enable Rx */
                MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, 0);
            }
            #endif
        }

        /* grab Rx residual if any */
        wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);

        if (wCsr &MGC_M_RXCSR_RXPKTRDY)
        {
            printk("[USB] RXPKTRDY when kicking urb !!");
            wCount = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCOUNT, bEnd);
            MGC_RxPacket(pThis, pEnd, wCount);
            spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
            return ;
        }

        /* protocol/endpoint/interval/NAKlimit */
        if (bEnd)
        {
            MGC_WriteCsr16(pBase, MGC_O_HDRC_RXMAXP, bEnd, wPacketSize);

            //  Set interrupt polling interval must depend on high, or low/full speed.
            MGC_SetInterval(pThis, pUrb, bEnd);
        }

        /* first time or re-program and shared FIFO, flush & clear toggle */
        /* twice in case of double packet buffering */
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, MGC_M_RXCSR_CLRDATATOG);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, MGC_M_RXCSR_CLRDATATOG);

        // HDRC will use this method.
        if (usb_gettoggle(pUrb->dev, pEnd->bRemoteEnd, 0))
        {
            #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
            MGC_Write32(pBase, M_REG_RXDATATOG, M_REG_SET_DATATOG(bEnd, 1));
            #else
            MGC_MISC_Write32(pBase, M_REG_RXDATATOG, M_REG_SET_DATATOG(bEnd, 1));
            #endif
        }

        // Check DMA condition.
        if ((usb_pipebulk(nPipe)) && (dwLength >= wPacketSize))
        {
            pDmaController = pThis->pDmaController;

            pDmaChannel = pEnd->pDmaChannel;

            /* candidate for DMA */
            if (pDmaController && !pDmaChannel)
            {
                pDmaChannel = pEnd->pDmaChannel = pDmaController->pfDmaAllocateChannel(pDmaController->pPrivateData, bEnd, FALSE, MGC_GetTransferType(pUrb), wPacketSize);
            }

            if (pDmaChannel && pDmaController && pDmaController->pfDmaProgramChannel)
            {
                /*
                        Note: 
                        Use "multiple packet RX, if size of data block not know" to handle rx data by dma.
                */
                pDmaChannel->bDesiredMode = 1;

                pDmaChannel->dwActualLength = 0L;
                pEnd->dwRequestSize = min(dwLength, pDmaChannel->dwMaxLength);
                bDmaOk = pDmaController->pfDmaProgramChannel(pDmaChannel, wPacketSize, pDmaChannel->bDesiredMode, pBuffer, pEnd->dwRequestSize);

                if (!bDmaOk && pDmaController->pfDmaReleaseChannel)
                {
                    pDmaController->pfDmaReleaseChannel(pDmaChannel);

                    pEnd->pDmaChannel = NULL;
                    pEnd->dwRequestSize = 0;
                }
                else
                {
                    // DMA ok.
                    spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
                    return ;
                }
            }
        }

        /* kick things off */
        if (bEnd)
        {
            MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, MGC_M_RXCSR_H_REQPKT);
        }
    }

    spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
}

static void MGC_KickUrb(MGC_LinuxCd *pThis, struct urb *pUrb, MGC_LinuxLocalEnd *pEnd)
{
    uint32_t dwLength;
    void *pBuffer;
    unsigned int nPipe;
    uint8_t bXmt;
    uint16_t wPacketSize;
    uint8_t bRemoteAddress, bRemoteEnd;

    if (!pUrb)
    {
        ERR("[USB] ***> bEnd=%d, pUrb = NULL!\n", pEnd->bEnd);
        return ;
    } 

    nPipe = pUrb->pipe;
    bXmt = usb_pipeout(nPipe);
    wPacketSize = usb_maxpacket(pUrb->dev, nPipe, usb_pipeout(nPipe));
    bRemoteAddress = (uint8_t)usb_pipedevice(nPipe);
    bRemoteEnd = (uint8_t)usb_pipeendpoint(nPipe);

    DBG(2, "[USB] <== pUrb=%p, bEnd=%d, wPacketSize=%d, bRemoteAddress=%d, bRemoteEnd=%d, bXmt=%d\n", pUrb, pEnd->bEnd, wPacketSize, bRemoteAddress, bRemoteEnd, bXmt);

    /* indicate in progress */
    pUrb->actual_length = 0;
    pUrb->error_count = 0;

    /* remember software state - find_end() will use this - */
    pEnd->bRemoteAddress = bRemoteAddress;
    pEnd->bRemoteEnd = bRemoteEnd;
    pEnd->bTrafficType = (uint8_t)usb_pipetype(nPipe);

    /* init urb */
    pEnd->dwOffset = 0;
    pEnd->dwRequestSize = 0;
    pEnd->dwIsoPacket = 0;
    pEnd->dwWaitFrame = 0;
    pEnd->bRetries = 0;
    pEnd->wPacketSize = wPacketSize;

    if (usb_pipecontrol(nPipe))
    {
        /* control transfers always start with an OUT */
        bXmt = TRUE;
        pThis->bEnd0Stage = MGC_END0_START;
    }

    /* gather right source of data */
    if (usb_pipeisoc(nPipe))
    {
        dwLength = pUrb->iso_frame_desc[0].length;
        if (dwLength >= MGC_HSDMA_MIN_DMA_LEN)
        {
            pBuffer = (void*)((uint32_t)pUrb->transfer_dma + pUrb->iso_frame_desc[0].offset);
        }
        else
        {
            pBuffer = (void*)((uint32_t)pUrb->transfer_buffer + pUrb->iso_frame_desc[0].offset);
        }
    }
    else if (usb_pipecontrol(nPipe))
    {
        dwLength = 8;
        pBuffer = (void*)pUrb->setup_packet;
    }
    else
    {
        // Only bulk transfer use DMA.
        dwLength = pUrb->transfer_buffer_length;
        if (dwLength >= MGC_HSDMA_MIN_DMA_LEN)
        {
            pBuffer = (void*)pUrb->transfer_dma;
        }
        else
        {
            pBuffer = (void*)pUrb->transfer_buffer;
        }
    }

     /* 
         Kernel 2.6.35 unmap_urb_for_dma() will invalidate cache.             
         This will cause fifo write data to cache being trashed by this invalidation.
         We just remove the flag check to prevent unmap_urb_for_dma() process invalidation.
         Kernel 2.6.27 do not have this problem.
     */
    if (pUrb->transfer_flags |(URB_SETUP_MAP_SINGLE | URB_SETUP_MAP_LOCAL |
			URB_DMA_MAP_SG | URB_DMA_MAP_PAGE |
			URB_DMA_MAP_SINGLE | URB_MAP_LOCAL))
    {
	pUrb->transfer_flags &= ~(URB_SETUP_MAP_SINGLE | URB_SETUP_MAP_LOCAL |
			URB_DMA_MAP_SG | URB_DMA_MAP_PAGE |
			URB_DMA_MAP_SINGLE | URB_MAP_LOCAL);
    }
     
    if (!pBuffer)
    {
        pBuffer = (void*)phys_to_virt(pUrb->transfer_dma);
    }

    if (!pBuffer)
    {
        // abort the transfer
        ERR("[USB] Rx requested but no buffer was given, BAD things are happening (TM)! aborting\n");
        return ;
    }

    DBG(3, "[USB] (%p): dir=%s, type=%d, wPacketSize=%d, bRemoteAddress=%d, bRemoteEnd=%d, pBuffer=%p\n", pUrb, (bXmt) ? "out" : "in", usb_pipetype(nPipe), wPacketSize, bRemoteAddress, bRemoteEnd, pBuffer);

    /* Configure endpoint */
    MGC_ProgramEnd(pThis, pEnd, pUrb, bXmt, pBuffer, dwLength);
}

static int MGC_ScheduleUrb(MGC_LinuxCd *pThis, MGC_LinuxLocalEnd *pEnd, struct urb *pUrb)
{
    int idle;

    /* async unlink?? */
    if (pUrb->status != ( - EINPROGRESS))
    {
        MGC_CompleteUrb(pThis, pEnd, pUrb);
        return 0;
    } 

    idle = MGC_IsEndIdle(pEnd);

    if (MGC_EnqueueEndUrb(pEnd, pUrb) != 0)
    {
        ERR("[USB] **>cannot queue pUrb=%p to pEnd=%p! this is bad (TM)\n", pUrb, pEnd);
        return  - EBUSY;
    }

    DBG(3, "[USB] queued URB %p (current %p) to end %d (bRemoteAddress=%d, bRemoteEnd=%d proto=%d) (idle=%d) pEnd->bBusyCompleting=%d\n", pUrb, MGC_GetCurrentUrb(pEnd), pEnd->bEnd, (uint8_t)usb_pipedevice(pUrb->pipe), (uint8_t)usb_pipeendpoint(pUrb->pipe), usb_pipetype(pUrb->pipe), idle, pEnd->bBusyCompleting);

    //  check pEnd->bBusyCompleting to prevent double kickstart the same urb.           
    if (idle && (pEnd->bBusyCompleting == 0))
    {
        MGC_KickUrb(pThis, MGC_GetCurrentUrb(pEnd), pEnd);
    }

    return 0;
}

static void MGC_CompleteEndUrb(MGC_LinuxCd *pThis, MGC_LinuxLocalEnd *pEnd, struct urb *pUrb, int toggle)
{
    if (pUrb->status == USB_ST_STALL)
    {
        toggle = 0;
    } 

    /* save data toggle */
    usb_settoggle(pUrb->dev, pEnd->bRemoteEnd, (pEnd->bIsTx) ? 1 : 0, toggle);

    if (pUrb->status)
    {
        DBG(1, "[USB] completing Tx URB=%p, status=%d, len=%x\n", pUrb, pUrb->status, pUrb->actual_length);
    }

    if (MGC_DequeueEndurb(pEnd, pUrb) == 0)
    {
        if (MGC_CompleteUrb(pThis, pEnd, pUrb) == 0)
        {
            MGC_KickUrb(pThis, MGC_GetNextUrb(pEnd), pEnd);
        }
    }
    else
    {
        ERR("[USB] *** pUrb=%p is not queued to bEnd=%d, this is BAD!\n", pUrb, pEnd->bEnd);
    }
}
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
static uint8_t MGC_IsNoiseStillSof(uint8_t *pBase)
{
    uint32_t reg;
    reg = MGC_Read8(pBase,M_REG_PERFORMANCE3);
    return (uint8_t)(reg & 0x80);/* Check if Host set SOF_FORCE bit */

}
#endif

static int MGC_ServiceUSBIntr(MGC_LinuxCd *pThis, uint8_t bIntrUSB)
{
    int handled = 0;

    uint8_t bSpeed = 1;
    uint8_t devctl;
    uint8_t *pBase = (uint8_t*)pThis->pRegs;
    MGC_LinuxLocalEnd *pEnd;
    uint32_t dwVirtualHubPortStatus = 0;
    #if !defined(CONFIG_ARCH_MT5396) && !defined(CONFIG_ARCH_MT5368)
    uint32_t dummy1;
    #endif
    uint8_t bEnd;
    uint16_t wFrame;
    uint8_t power;
    unsigned long flags = 0;

    devctl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);
    power = MGC_Read8(pBase, MGC_O_HDRC_POWER);

    if (bIntrUSB &MGC_M_INTR_RESUME)
    {
        handled++;

        DBG(2, "[USB] RESUME\n");
    }

    if (bIntrUSB &MGC_M_INTR_SESSREQ)
    {
        handled++;

        DBG(2, "[USB] SESSION_REQUEST\n");
    }

    if (bIntrUSB &MGC_M_INTR_VBUSERROR)
    {
        handled++;

        DBG(2, "[USB] V_BUS ERROR??? this is bad (TM)\n");
    }

    if (bIntrUSB &MGC_M_INTR_CONNECT)
    {
        handled++;
 
        DBG(2, "[USB] CONNECT\n");

        pThis->pRootDevice = NULL;
        pThis->bEnd0Stage = MGC_END0_START;

        if (devctl &MGC_M_DEVCTL_LSDEV)
        {
            bSpeed = MGC_SPEED_LS;

            dwVirtualHubPortStatus = (1 << USB_PORT_FEAT_C_CONNECTION) | (1 << USB_PORT_FEAT_LOWSPEED) | (1 << USB_PORT_FEAT_POWER) | (1 << USB_PORT_FEAT_CONNECTION);
        }
        else if (devctl &MGC_M_DEVCTL_FSDEV)
        {
            /* NOTE: full-speed is "speculative" until reset */
            //bSpeed = 2;
            bSpeed = (power &MGC_M_POWER_HSMODE) ? MGC_SPEED_HS : MGC_SPEED_FS;

            dwVirtualHubPortStatus = (1 << USB_PORT_FEAT_C_CONNECTION) | (1 << USB_PORT_FEAT_POWER) | (1 << USB_PORT_FEAT_CONNECTION);
        }

        pThis->bRootSpeed = bSpeed;
        pThis->dwVirtualHubPortStatus = dwVirtualHubPortStatus;
        usb_hcd_poll_rh_status(musbstruct_to_hcd(pThis));
    }

    /* saved one bit: bus reset and babble share the same bit;
     * If I am host is a babble! i must be the only one allowed
     * to reset the bus; when in otg mode it means that I have 
     * to switch to device
     */
    if (bIntrUSB &MGC_M_INTR_RESET)
    {
        handled++;

        DBG(1, "[USB] BUS babble\n");

        //  Solve the busy usb babble interrupt caused by babble device
        //  if device is attached
        if (devctl &(MGC_M_DEVCTL_FSDEV | MGC_M_DEVCTL_LSDEV))
        {
            DBG(1, "[USB] MT539x ");

            #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
            if (MGC_IsNoiseStillSof(pBase))
            {
                DBG(1,"[USB] SOF is still transmit during babble.\n", NULL);
                // Ignore this babble.
                bIntrUSB &= ~MGC_M_INTR_RESET;
            }
            #else
            dummy1 = MGC_MISC_Read32(pBase, MGC_O_HDRC_DUMMY1);
            DBG(1, "[USB] device hooked , addr %08x (dummy1) =%08x\n", (uint32_t)pBase + MUSB_MISCBASE + MGC_O_HDRC_DUMMY1, dummy1);

            if (dummy1 &MGC_M_DUMMY1_SOFFORCE)
            {
                DBG(1, "[USB] SOF is still transmit during babble\n");

                bIntrUSB &= ~MGC_M_INTR_RESET;
            }
            #endif
        }

        handled++;
    }

    if (bIntrUSB &MGC_M_INTR_SOF)
    {
        DBG(2, "[USB] START_OF_FRAME\n");

        handled++;

        /* start any periodic Tx transfers waiting for current frame */
        wFrame = MGC_Read16(pBase, MGC_O_HDRC_FRAME);

        for (bEnd = 1; (bEnd < pThis->bEndCount) && (pThis->wEndMask >= (1 << bEnd)); bEnd++)
        {
            pEnd = &(pThis->aLocalEnd[EPTX][bEnd]); /* 1: Tx */
            if ((pEnd->dwWaitFrame) && (pEnd->dwWaitFrame >= wFrame))
            {
                pEnd->dwWaitFrame = 0;

                spin_lock_irqsave(&pThis->SelectEndLock, flags);
                MGC_SelectEnd(pBase, bEnd);
                MGC_StartTx(pThis, bEnd);
                spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
            }
        }
    }

    /* p35 MUSBHDRC manual for the order of the tests */
    if (bIntrUSB &MGC_M_INTR_DISCONNECT)
    {
        DBG(2, "[USB] DISCONNECT\n");

        handled++;

        pThis->dwVirtualHubPortStatus = (1 << USB_PORT_FEAT_C_CONNECTION) | (1 << USB_PORT_FEAT_POWER);

        pThis->pRootDevice = NULL;

        // Set UTMI+PHY to low power mode and stop XCLK.
        power = MGC_Read8(pBase, MGC_O_HDRC_POWER);
        power |= (MGC_M_POWER_SUSPENDM | MGC_M_POWER_ENSUSPEND);
        MGC_Write8(pBase, MGC_O_HDRC_POWER, power);
    }

    /* I cannot get suspend while in host mode! go to error mode and ignore 
     * the other signals; need to be last (see manual p35)s  */
    if (bIntrUSB &MGC_M_INTR_SUSPEND)
    {
        DBG(2, "[USB] RECEIVED SUSPEND\n");

        handled++;
    }

    return handled;
}

static void MGC_CheckConnect(unsigned long pParam)
{
    MGC_LinuxCd *pThis = (MGC_LinuxCd*)pParam;
    #if !defined(CONFIG_ARCH_MT5396) && !defined(CONFIG_ARCH_MT5368)
    void *pBase = pThis->pRegs;
    #endif
    uint32_t dwLineState;

    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
    dwLineState = LINESTATE_FS_SPEED;
    #else
    dwLineState = MGC_PHY_Read32(pBase, M_REG_PHYC5);
    dwLineState &= M_REG_LINESTATE_MASK;
    #endif
    if (dwLineState == LINESTATE_DISCONNECT)
    {
        printk("[USB] Line State Err : 0x%x\n", dwLineState);
        return ;
    }

    if (dwLineState == LINESTATE_HWERROR)
    {
        //  Move MGC_dwCheckInsert inside MGC_LinuxCd port structure
        pThis->bCheckInsert++;

        if (pThis->bCheckInsert > 5)
        {
            DBG(2, "[USB] LINESTATE_HWERROR !!!\n");

            pThis->bCheckInsert = 0;
            return ;
        }

        MGC_SetTimer(pThis, MGC_CheckConnect, (unsigned long)pThis, MGC_CHECK_INSERT_DEBOUNCE);
        printk("[USB] Line State Err : 0x%x\n", dwLineState);
        return ;
    }

    pThis->bInsert = TRUE;

    DBG(2, "[USB] Connect interrupt !!!\n");
    MGC_UnlinkInvalidUrb((unsigned long)pThis);
    // handle device connect.
    MGC_ServiceUSBIntr(pThis, MGC_M_INTR_CONNECT);
}

static uint8_t MGC_ServiceHostDefault(MGC_LinuxCd *pThis, uint16_t wCount, struct urb *pUrb)
{
    uint8_t bMore = FALSE;
    uint8_t *pFifoDest = NULL;
    uint16_t wFifoCount = 0;
    uint8_t *pBase = (uint8_t*)pThis->pRegs;
    MGC_LinuxLocalEnd *pEnd = &(pThis->aLocalEnd[EP0][0]);
    MUSB_DeviceRequest *pRequest = (MUSB_DeviceRequest*)pUrb->setup_packet;

    DBG(2, "[USB] <== (wCount=%04x, pUrb=%lx, bStage=%02x)\n", wCount, (unsigned long)pUrb, pThis->bEnd0Stage);

    if (MGC_END0_IN == pThis->bEnd0Stage)
    {
        /* we are receiving from peripheral */
        pFifoDest = pUrb->transfer_buffer + pUrb->actual_length;
        wFifoCount = min(wCount, ((uint16_t)(pUrb->transfer_buffer_length - pUrb->actual_length)));

        DBG(3, "[USB] Receiving %d bytes in &%p[%d] (pUrb->actual_length=%u)\n", wFifoCount, pUrb->transfer_buffer, (unsigned int)pUrb->actual_length, pUrb->actual_length);

        MGC_UnloadFifo(pBase, 0, wFifoCount, pFifoDest);

        pUrb->actual_length += wFifoCount;
        if ((pUrb->actual_length < pUrb->transfer_buffer_length) && (wCount == pEnd->wPacketSize))
        {
            bMore = TRUE;
        } 
    }
    else
    {
        /* we are sending to peripheral */
        if ((MGC_END0_START == pThis->bEnd0Stage) && (pRequest->bmRequestType &USB_DIR_IN))
        {
            DBG(3, "[USB] just did setup, switching to IN\n");

            /* this means we just did setup; switch to IN */
            pThis->bEnd0Stage = MGC_END0_IN;
            bMore = TRUE;

        }
        else if (pRequest->wLength && (MGC_END0_START == pThis->bEnd0Stage))
        {
            pThis->bEnd0Stage = MGC_END0_OUT;
        }

        if (MGC_END0_OUT == pThis->bEnd0Stage)
        {
            pFifoDest = (uint8_t*)(pUrb->transfer_buffer + pUrb->actual_length);
            wFifoCount = min(pEnd->wPacketSize, ((uint16_t)(pUrb->transfer_buffer_length - pUrb->actual_length)));
            DBG(3, "[USB] Sending %d bytes to %p\n", wFifoCount, pFifoDest);
            MGC_LoadFifo(pBase, 0, wFifoCount, pFifoDest);

            pEnd->dwRequestSize = wFifoCount;
            pUrb->actual_length += wFifoCount;
            if (wFifoCount)
            {
                bMore = TRUE;
            }
        }
    }

    return bMore;
}

static void MGC_ServiceDefaultEnd(MGC_LinuxCd *pThis)
{
    struct urb *pUrb;
    uint16_t wCsrVal, wCount;
    int status = USB_ST_NOERROR;
    uint8_t *pBase = (uint8_t*)pThis->pRegs;
    MGC_LinuxLocalEnd *pEnd = &(pThis->aLocalEnd[EP0][0]);
    uint8_t bVal, bOutVal = 0, bComplete = FALSE, bError = FALSE;
    unsigned long flags = 0;

    spin_lock(&pEnd->Lock);
    pUrb = MGC_GetCurrentUrb(pEnd);
    if (!pUrb)
    {
        spin_unlock(&pEnd->Lock);
        return ;
    } 

    spin_lock_irqsave(&pThis->SelectEndLock, flags);
    MGC_SelectEnd(pBase, 0);

    wCsrVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_CSR0, 0);
    wCount = MGC_ReadCsr8(pBase, MGC_O_HDRC_COUNT0, 0);
    bVal = (uint8_t)wCsrVal;

    DBG(2, "[USB] <== CSR0=%04x, wCount=%04x\n", wCsrVal, wCount);

    /* if we just did status stage, we are done */
    if (MGC_END0_STATUS == pThis->bEnd0Stage)
    {
        bComplete = TRUE;
    }

    /* prepare status */
    if ((MGC_END0_START == pThis->bEnd0Stage) && !wCount && (wCsrVal &MGC_M_CSR0_RXPKTRDY))
    {
        printk("[USB] EP0 missed data, Flush FIFO !\n");
        /* just started and got Rx with no data, so probably missed data */
        status = USB_ST_SHORT_PACKET;
        bError = TRUE;

        //bComplete = TRUE;
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_FLUSHFIFO);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_FLUSHFIFO);
    }

    if (bVal &MGC_M_CSR0_H_RXSTALL)
    {
        printk("[USB] STALLING ENDPOINT 0\n");
        status = USB_ST_STALL;
        bError = TRUE;
    }
    else if (bVal &MGC_M_CSR0_H_ERROR)
    {
        printk("[USB] ep0 no response (error)\n");

        status = USB_ST_NORESPONSE;
        bError = TRUE;
    }
    else if (bVal &MGC_M_CSR0_H_NAKTIMEOUT)
    {
        printk("[USB] ep0 NAK timeout pEnd->bRetries=%d\n", pEnd->bRetries);

        if (++pEnd->bRetries < MUSB_MAX_RETRIES)
        {
            /* cover it up if retries not exhausted */
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, 0);
        }
        else
        {
            printk("[USB] no response (NAK timeout)\n");
            pEnd->bRetries = 0;
            status = USB_ST_NORESPONSE;
            bError = TRUE;
        }
    }

    if (USB_ST_NORESPONSE == status)
    {
        printk("[USB] ep0 aborting\n");

        /* use the proper sequence to abort the transfer */
        if (bVal &MGC_M_CSR0_H_REQPKT)
        {
            bVal &= ~MGC_M_CSR0_H_REQPKT;
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, bVal);
            bVal &= ~MGC_M_CSR0_H_NAKTIMEOUT;
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, bVal);
        }
        else
        {
            printk("[USB] end 0 no response, Flush FIFO !\n");
            bVal |= MGC_M_CSR0_FLUSHFIFO;
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, bVal);
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, bVal);
            bVal &= ~MGC_M_CSR0_H_NAKTIMEOUT;
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, bVal);
        }

        MGC_WriteCsr8(pBase, MGC_O_HDRC_NAKLIMIT0, 0, 0);
    }

    if (bError)
    {
        DBG(3, "[USB] ep0 handling error\n");

        /* clear it */
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, 0);
    }

    if (!pUrb)
    {
        /* stop endpoint since we have no place for its data, this 
         * SHOULD NEVER HAPPEN! */
        printk("[USB] no URB for end 0, Flush FIFO !\n");
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_FLUSHFIFO);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_FLUSHFIFO);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, 0);

        /* start next URB that might be queued for it */
        spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
        spin_unlock(&pEnd->Lock);
        return ;
    }

    spin_unlock_irqrestore(&pThis->SelectEndLock, flags);

    if (!bComplete && !bError)
    {
        /* call common logic and prepare response */
        if (MGC_ServiceHostDefault(pThis, wCount, pUrb))
        {
            /* more packets required */
            bOutVal = (MGC_END0_IN == pThis->bEnd0Stage) ? MGC_M_CSR0_H_REQPKT: MGC_M_CSR0_TXPKTRDY;
            DBG(3, "[USB] Need more bytes bOutVal=%04x\n", bOutVal);
        }
        else
        {
            /* data transfer complete; perform status phase */
            bOutVal = MGC_M_CSR0_H_STATUSPKT | (usb_pipeout(pUrb->pipe) ? MGC_M_CSR0_H_REQPKT : MGC_M_CSR0_TXPKTRDY);

            /* flag status stage */
            pThis->bEnd0Stage = MGC_END0_STATUS;
            DBG(3, "[USB] Data transfer complete, status phase bOutVal=%04x\n", bOutVal);
        }

        /* write CSR0 if needed */
        if (bOutVal)
        {
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, bOutVal);
        }
    }

    /* call completion handler if done */
    if (bComplete || bError)
    {
        DBG(3, "[USB] completing cntrl URB %p, status=%d, len=%x\n", pUrb, status, pUrb->actual_length);

        if (MGC_DequeueEndurb(pEnd, pUrb) == 0)
        {
            pUrb->status = status;
            if (MGC_CompleteUrb(pThis, pEnd, pUrb) == 0)
            {
                /* Endpoint is not idle. */
                MGC_KickUrb(pThis, MGC_GetNextUrb(pEnd), pEnd);
            }
        }
        else
        {
            ERR("[USB] *** pUrb=%p is not queued to bEnd=%d\n", pUrb, pEnd->bEnd);
        }
    }

    spin_unlock(&pEnd->Lock);
}

static void MGC_ServiceTxAvail(MGC_LinuxCd *pThis, uint8_t bEnd)
{
    int skip = 0;
    struct urb *pUrb;
    uint16_t wTxCsrVal;
    uint16_t wVal = 0;
    MGC_LinuxLocalEnd *pEnd = &(pThis->aLocalEnd[EPTX][bEnd]); /*1: Tx*/
    uint8_t *pBase = (uint8_t*)pThis->pRegs;
    int toggle;
    uint32_t status = 0;
    unsigned long flags = 0;

    DBG(1, "[USB] <==\n");

    spin_lock(&pEnd->Lock);
    pUrb = MGC_GetCurrentUrb(pEnd);
    if (!pUrb)
    {
        //printk("[USB] Killed Tx pUrb=0x%X.\n", (uint32_t)pUrb);
        MGC_SelectEnd(pBase, bEnd);
        MGC_CheckTxCsrValidForWrite(pBase, bEnd);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, 0);

        spin_unlock(&pEnd->Lock);
        return ;
    } 

    spin_lock_irqsave(&pThis->SelectEndLock, flags);
    MGC_SelectEnd(pBase, bEnd);

    wVal = wTxCsrVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd);

    DBG(3, "[USB] end %d wTxCsrVal=%04x\n", bEnd, wTxCsrVal);

    /* check for errors */
    if (wTxCsrVal &MGC_M_TXCSR_H_RXSTALL)
    {
        printk("[USB] TX end %d STALL, Flush FIFO (%d:%d) !\n", bEnd, pEnd->bRemoteAddress, pEnd->bEnd);
        status = USB_ST_STALL;
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, MGC_M_TXCSR_MODE | MGC_M_TXCSR_FLUSHFIFO | MGC_M_TXCSR_TXPKTRDY);
    }
    else if (wTxCsrVal &MGC_M_TXCSR_H_ERROR)
    {
        printk("[USB] TX data error on ep=%d, Flush FIFO (%d:%d) !\n", bEnd, pEnd->bRemoteAddress, pEnd->bEnd);

        status = USB_ST_NORESPONSE;

        /* do the proper sequence to abort the transfer */
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, MGC_M_TXCSR_MODE | MGC_M_TXCSR_FLUSHFIFO | MGC_M_TXCSR_TXPKTRDY);
    }
    else if (wTxCsrVal &MGC_M_TXCSR_H_NAKTIMEOUT)
    {
        /* cover it up if retries not exhausted */
        if (pUrb->status == ( - EINPROGRESS) && ++pEnd->bRetries < MUSB_MAX_RETRIES)
        {

            wVal |= MGC_M_TXCSR_MODE | MGC_M_TXCSR_TXPKTRDY;
            wVal &= ~MGC_M_TXCSR_H_NAKTIMEOUT;
            MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wVal);

            spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
            spin_unlock(&pEnd->Lock);
            printk("[USB] TX NAK Timeout (%d:%d), Retry %d !\n", pEnd->bRemoteAddress, pEnd->bEnd, pEnd->bRetries);
            return ;
        }

        if (pUrb->status == ( - EINPROGRESS))
        {
            printk("[USB] TX NAK Timeout !\n");
            status =  - USB_ST_NORESPONSE;
        }

        printk("[USB] device Tx not responding on ep=%d, Flush FIFO (%d:%d) !\n", bEnd, pEnd->bRemoteAddress, pEnd->bEnd);

        /* do the proper sequence to abort the transfer */
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, MGC_M_TXCSR_MODE | MGC_M_TXCSR_FLUSHFIFO | MGC_M_TXCSR_TXPKTRDY);
        pEnd->bRetries = 0;
    }
    else if (wTxCsrVal &MGC_M_TXCSR_FIFONOTEMPTY)
    {
        printk("[USB] FIFO not empty when Tx done on ep=%d (%d:%d) !\n", bEnd, pEnd->bRemoteAddress, pEnd->bEnd);
        /* whopps, dbould buffering better be enabled */
        skip = TRUE;
    }

    if (status)
    {
        pUrb->status = status;

        /* reset error bits */
        wVal &= ~(MGC_M_TXCSR_H_ERROR | MGC_M_TXCSR_H_RXSTALL | MGC_M_TXCSR_H_NAKTIMEOUT);
        wVal |= MGC_M_TXCSR_FRCDATATOG;
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wVal);
    }

    if (!skip && pUrb->status == ( - EINPROGRESS))
    {
        MGC_TxPacket(pThis, pEnd);
    }

    /* complete the current request or start next tx transaction */
    if (pUrb->status != ( - EINPROGRESS))
    {
        //  set data toggle in device basis for supporting Hub.
        // get data toggle bit from physical ep.
        #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
        toggle = ((MGC_Read32(pBase, M_REG_TXDATATOG)) >> bEnd) &0x01;
        #else
        toggle = ((MGC_MISC_Read32(pBase, M_REG_TXDATATOG)) >> bEnd) &0x01;
        #endif
            
        // release dma channel.
        if (pEnd->pDmaChannel)
        {
            /* release previously-allocated channel */
            pThis->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
            pEnd->pDmaChannel = NULL;
        }
        pUrb->actual_length = pEnd->dwOffset;

        spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
        MGC_CompleteEndUrb(pThis, pEnd, pUrb, toggle);
    }
    else
    {
        if (!skip)
        {
            MGC_CheckTxCsrValidForWrite(pBase, bEnd);
            MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, (MGC_M_TXCSR_MODE | MGC_M_TXCSR_TXPKTRDY));
        }
        spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
    }

    spin_unlock(&pEnd->Lock);
}

static void MGC_ServiceRxReady(MGC_LinuxCd *pThis, uint8_t bEnd)
{
    struct urb *pUrb;
    uint16_t wRxCount, wRxCsrVal, wVal = 0;
    uint8_t bIsochError = FALSE;
    MGC_LinuxLocalEnd *pEnd = &(pThis->aLocalEnd[EPRX][bEnd]); /*0: Rx*/
    uint8_t *pBase = (uint8_t*)pThis->pRegs;
    uint8_t bDone = FALSE;
    uint32_t dwLength;
    MGC_DmaChannelStatus bStatus;
    uint8_t bDmaOk = FALSE;
    uint8_t *pDmaBuffer;
    int toggle;
    uint32_t status = 0;
    unsigned long flags = 0;

    DBG(2, "[USB] <== end%d\n", bEnd);
    spin_lock(&pEnd->Lock);
    DBG(3, "[USB] locked end%d, pUrb=%p\n", bEnd, MGC_GetCurrentUrb(pEnd));

    pUrb = MGC_GetCurrentUrb(pEnd);
    if (!pUrb)
    {
        //printk("[USB] Urb has been freed before packet comeback !!");
        MGC_SelectEnd(pBase, bEnd);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, 0);

        spin_unlock(&pEnd->Lock);
        return ;
    } 

    spin_lock_irqsave(&pThis->SelectEndLock, flags);
    MGC_SelectEnd(pBase, bEnd);

    wRxCsrVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);
    wVal = wRxCsrVal;
    wRxCount = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCOUNT, bEnd);

    DBG(3, "[USB] end %d wRxCsrVal=%04x, wRxCount=%d, pUrb->actual_length=%d\n", bEnd, wRxCsrVal, wRxCount, pUrb->actual_length);

    /* check for errors, concurrent stall & unlink is not really
     * handled yet! */
    if (wRxCsrVal &MGC_M_RXCSR_H_RXSTALL)
    {
        printk("[USB] RX end %d STALL (%d:%d) !\n", bEnd, pEnd->bRemoteAddress, pEnd->bEnd);
        status = USB_ST_STALL;
    }
    else if (wRxCsrVal &MGC_M_RXCSR_H_ERROR)
    {
        printk("[USB] end %d Rx error (%d:%d) !\n", bEnd, pEnd->bRemoteAddress, pEnd->bEnd);
        status =  - USB_ST_NORESPONSE;

        /* do the proper sequence to abort the transfer */
        wVal &= ~MGC_M_RXCSR_H_REQPKT;
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
        MGC_WriteCsr8(pBase, MGC_O_HDRC_RXINTERVAL, bEnd, 0);
    }
    else if (wRxCsrVal &MGC_M_RXCSR_DATAERR)
    {
        if (PIPE_BULK == pEnd->bTrafficType)
        {
            /* cover it up if retries not exhausted, slow devices might  
             * not answer quickly enough: I was expecting a packet but the 
             * packet didn't come. The interrupt is generated after 3 failed
             * attempts, it make MUSB_MAX_RETRIESx3 attempts total..
             */
            if (pUrb->status == ( - EINPROGRESS) && ++pEnd->bRetries < MUSB_MAX_RETRIES)
            {
                /* silently ignore it */
                wRxCsrVal &= ~MGC_M_RXCSR_DATAERR;
                wRxCsrVal &= ~MGC_M_RXCSR_RXPKTRDY;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wRxCsrVal | MGC_M_RXCSR_H_REQPKT);

                spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
                spin_unlock(&pEnd->Lock);
                printk("[USB] Rx NAK Timeout (%d:%d), Retry %d !\n", pEnd->bRemoteAddress, pEnd->bEnd, pEnd->bRetries);
                return ;
            }

            if (pUrb->status == ( - EINPROGRESS))
            {
                printk("[USB] RX NAK Timeout !\n");
                status =  - USB_ST_NORESPONSE;
            }

            wVal &= ~MGC_M_RXCSR_H_REQPKT;
            MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
            MGC_WriteCsr8(pBase, MGC_O_HDRC_RXINTERVAL, bEnd, 0);
            pEnd->bRetries = 0;

            /* do the proper sequence to abort the transfer; 
             * am I dealing with a slow device maybe? */
            printk("[USB] end=%d device Rx not responding (%d:%d) !\n", bEnd, pEnd->bRemoteAddress, pEnd->bEnd);

        }
        else if (PIPE_ISOCHRONOUS == pEnd->bTrafficType)
        {
            DBG(3, "[USB] bEnd=%d Isochronous error\n", bEnd);
            bIsochError = TRUE;
        }
    }

    /* an error won't process the data */
    if (status)
    {
        pUrb->status = status;

        /* data errors are signaled */
        if (USB_ST_STALL != status)
        {
            printk("[USB] end %d Rx error, status=%d !\n", bEnd, status);
        }
        else
        {
            printk("[USB] MGC_ServiceRxReady : Flush FIFO !\n");
            /* twice in case of double packet buffering */
            MGC_WriteCsr16((uint8_t*)pThis->pRegs, MGC_O_HDRC_RXCSR, bEnd, MGC_M_RXCSR_FLUSHFIFO | MGC_M_RXCSR_CLRDATATOG);
            MGC_WriteCsr16((uint8_t*)pThis->pRegs, MGC_O_HDRC_RXCSR, bEnd, MGC_M_RXCSR_FLUSHFIFO | MGC_M_RXCSR_CLRDATATOG);
        }

        DBG(3, "[USB] clearing all error bits, right away\n");
        wVal &= ~(MGC_M_RXCSR_H_ERROR | MGC_M_RXCSR_DATAERR | MGC_M_RXCSR_H_RXSTALL);
        wVal &= ~MGC_M_RXCSR_RXPKTRDY;
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
    }

    /* no errors, unload... */
    if (pUrb->status == ( - EINPROGRESS))
    {
        /* be sure a packet is ready for unloading */
        if (!wRxCsrVal &MGC_M_RXCSR_RXPKTRDY)
        {
            pUrb->status = USB_ST_INTERNALERROR;

            /* do the proper sequence to abort the transfer */
            wVal &= ~MGC_M_RXCSR_H_REQPKT;
            MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
            DBG(3, "[USB] Rx interrupt with no errors or packet!\n");
        }
        else
        {
            /* we are expecting traffic */
            if (pEnd->pDmaChannel)
            {
                bStatus = pThis->pDmaController->pfDmaGetChannelStatus(pEnd->pDmaChannel);
                pEnd->dwOffset += pEnd->pDmaChannel->dwActualLength;
                pUrb->actual_length += pEnd->pDmaChannel->dwActualLength;

                if (MGC_DMA_STATUS_FREE == bStatus)
                {
                    /* see if we are done */
                    bDone = (pEnd->dwOffset >= pUrb->transfer_buffer_length) || (pEnd->pDmaChannel->dwActualLength < pEnd->wPacketSize);

                    if (bEnd && bDone)
                    {
                        // clear rxpktrdy.
                        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, 0);
                        pUrb->status = 0;
                    }
                    else
                    {
                        dwLength = pUrb->transfer_buffer_length - pEnd->dwOffset;
                        if (dwLength > pEnd->wPacketSize)
                        {
                            printk("[USB] MGC_ServiceRxReady CASE1 %d %d %d\n", dwLength, pUrb->transfer_buffer_length, pEnd->dwOffset);
                            pDmaBuffer = (uint8_t*)pUrb->transfer_dma;
                            pDmaBuffer += pEnd->dwOffset;
                            pEnd->pDmaChannel->bDesiredMode = 1;
                            pEnd->pDmaChannel->dwActualLength = 0L;
                            pEnd->dwRequestSize = dwLength;
                            bDmaOk = pThis->pDmaController->pfDmaProgramChannel(pEnd->pDmaChannel, pEnd->wPacketSize, pEnd->pDmaChannel->bDesiredMode, pDmaBuffer, pEnd->dwRequestSize);
                            if (!bDmaOk)
                            {
                                pThis->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
                                pEnd->pDmaChannel = NULL;
                                pEnd->dwRequestSize = 0;
                            }
                        }
                        else
                        {
                            printk("[USB] MGC_ServiceRxReady CASE2 ! %d %d %d\n", dwLength, pUrb->transfer_buffer_length, pEnd->dwOffset);
                            /* release previously-allocated channel */
                            pThis->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
                            pEnd->pDmaChannel = NULL;

                            MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, MGC_M_RXCSR_H_REQPKT);
                        }
                    }
                }
                else if (MGC_DMA_STATUS_MODE1_SHORTPKT == bStatus)
                {
                    bDone = MGC_RxPacket(pThis, pEnd, wRxCount);
                }
                else
                {
                    printk("[USB] MGC_ServiceRxReady Error DMA status %d:%d ! %d %d\n", ((MGC_HsDmaChannel*)(pEnd->pDmaChannel->pPrivateData))->bIndex, bStatus, pUrb->transfer_buffer_length, pEnd->dwOffset);
                }
            }
            else
            {
                bDone = MGC_RxPacket(pThis, pEnd, wRxCount);
            }
        }
    }

    /* complete the current request or start next one clearing RxPktRdy 
     * and setting ReqPkt */
    if (pUrb->status != ( - EINPROGRESS))
    {
        //  set data toggle in device basis for supporting Hub.
        // get data toggle bit from physical ep.
        #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
        toggle = ((MGC_Read32(pBase, M_REG_RXDATATOG)) >> bEnd) &0x01;
        #else
        toggle = ((MGC_MISC_Read32(pBase, M_REG_RXDATATOG)) >> bEnd) &0x01;
        #endif
        // release dma channel.
        if (pEnd->pDmaChannel)
        {
            /* release previously-allocated channel */
            pThis->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
            pEnd->pDmaChannel = NULL;
        }

        spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
        MGC_CompleteEndUrb(pThis, pEnd, pUrb, toggle);
    }
    else
    {
        spin_unlock_irqrestore(&pThis->SelectEndLock, flags);
    }

    spin_unlock(&pEnd->Lock);
}

static void MGC_ResetOff(unsigned long param)
{
    uint8_t power;
    MGC_LinuxCd *pThis = (MGC_LinuxCd*)param;
    void *pBase = pThis->pRegs;
    unsigned long flags;
    uint32_t u4UsbIrqEnable = 0;

    power = MGC_Read8(pBase, MGC_O_HDRC_POWER);
    MGC_Write8(pBase, MGC_O_HDRC_POWER, power &~MGC_M_POWER_RESET);

    /* check for high-speed and set in root device if so */
    power = MGC_Read8(pBase, MGC_O_HDRC_POWER);
    if (power &MGC_M_POWER_HSMODE)
    {
        DBG(3, "[USB] high-speed device connected\n");
        pThis->bRootSpeed = MGC_SPEED_HS;
    }

    // disable reset status.
    pThis->dwVirtualHubPortStatus &= ~(1 << USB_PORT_FEAT_RESET);

    // reset complete. device still connect, set port enable.
    pThis->dwVirtualHubPortStatus |= (1 << USB_PORT_FEAT_C_RESET) | (1 << USB_PORT_FEAT_ENABLE);

    if (pThis->bRootSpeed == MGC_SPEED_HS)
    {
        pThis->dwVirtualHubPortStatus &= ~(1 << USB_PORT_FEAT_LOWSPEED);
        pThis->dwVirtualHubPortStatus |= USB_PORT_STAT_HIGH_SPEED;
    }
    else if (pThis->bRootSpeed == MGC_SPEED_FS)
    {
        pThis->dwVirtualHubPortStatus &= ~(1 << USB_PORT_FEAT_LOWSPEED);
    }
    else
    {
        pThis->dwVirtualHubPortStatus |= (1 << USB_PORT_FEAT_LOWSPEED);
    }

    SPIN_UNLOCK_IRQRESTORE(&pThis->Lock, flags, pThis->nIrq);
}

static int MGC_UnlinkUrb(MGC_LinuxCd *pThis, struct urb *pUrb)
{
    unsigned long flags;
    unsigned long flags1;
    uint32_t u4UsbIrqEnable = 0;
    MGC_LinuxLocalEnd *pEnd;
    const void *pBase = pThis->pRegs;
    uint8_t bIndex;
    uint16_t wIntr;

    printk("[USB] MGC_UnlinkUrb pUrb=%p, pUrb->hcpriv=%p !\n", pUrb, pUrb->hcpriv);

    SPIN_LOCK_IRQSAVE(&pThis->Lock, flags, pThis->nIrq);

    pEnd = (MGC_LinuxLocalEnd*)pUrb->hcpriv;
    if (pEnd)
    {
        //spin_lock(&pEnd->Lock);
        if (pEnd->pCurrentUrb == pUrb)
        {
            MGC_DequeueEndurb(pEnd, pUrb);

            spin_lock_irqsave(&pThis->SelectEndLock, flags1);
            bIndex = MGC_Read8(pBase, MGC_O_HDRC_INDEX);
            MGC_SelectEnd((uint8_t*)pThis->pRegs, pEnd->bEnd);

            // release dma channel.
            if (pEnd->pDmaChannel)
            {
                /* release previously-allocated channel */
                pThis->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
                pEnd->pDmaChannel = NULL;
            } 

            if (pEnd->bEnd)
            {
                if (!pEnd->bIsTx)
                {
                    printk("[USB] MGC_UnlinkUrb: Flush Rx FIFO !\n");
                    /* twice in case of double packet buffering */
                    MGC_WriteCsr16((uint8_t*)pThis->pRegs, MGC_O_HDRC_RXCSR, pEnd->bEnd, MGC_M_RXCSR_FLUSHFIFO | MGC_M_RXCSR_CLRDATATOG);
                    MGC_WriteCsr16((uint8_t*)pThis->pRegs, MGC_O_HDRC_RXCSR, pEnd->bEnd, MGC_M_RXCSR_FLUSHFIFO | MGC_M_RXCSR_CLRDATATOG);

                    // Clear interrupt.
                    wIntr = MGC_Read16(pBase, MGC_O_HDRC_INTRRX);
                    if (wIntr &(1 << pEnd->bEnd))
                    {
                        MGC_Write16(pBase, MGC_O_HDRC_INTRRX, (1 << pEnd->bEnd));
                    }
                }
                else
                {
                    printk("[USB] MGC_UnlinkUrb: Flush Tx FIFO !\n");
                    MGC_CheckTxCsrValidForWrite(pBase, pEnd->bEnd);
                    /* twice in case of double packet buffering */
                    MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, pEnd->bEnd, MGC_M_TXCSR_FLUSHFIFO | MGC_M_TXCSR_CLRDATATOG);
                    MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, pEnd->bEnd, MGC_M_TXCSR_FLUSHFIFO | MGC_M_TXCSR_CLRDATATOG);

                    // Clear interrupt.
                    wIntr = MGC_Read16(pBase, MGC_O_HDRC_INTRTX);
                    if (wIntr &(1 << pEnd->bEnd))
                    {
                        MGC_Write16(pBase, MGC_O_HDRC_INTRTX, (1 << pEnd->bEnd));
                    }
                }
            }
            else
            {
                printk("[USB] MGC_UnlinkUrb: Flush Control FIFO  !\n");
                /* endpoint 0: just flush */
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, pEnd->bEnd, MGC_M_CSR0_FLUSHFIFO);
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, pEnd->bEnd, MGC_M_CSR0_FLUSHFIFO);

                wIntr = MGC_Read16(pBase, MGC_O_HDRC_INTRTX);
                if (wIntr &1)
                {
                    MGC_Write16(pBase, MGC_O_HDRC_INTRTX, 1);
                }
            }

            /* restore index */
            MGC_Write8(pBase, MGC_O_HDRC_INDEX, bIndex);
            spin_unlock_irqrestore(&pThis->SelectEndLock, flags1);
        }
        else
        {
            MGC_DequeueEndurb(pEnd, pUrb);
        }
        //spin_unlock(&pEnd->Lock);
    }

    MGC_CallbackUrb(pThis, pUrb);

    SPIN_UNLOCK_IRQRESTORE(&pThis->Lock, flags, pThis->nIrq);

    return 0;
}

//-------------------------------------------------------------------------
/** MUC_ResetPhy
 *  host controller register reset and initial.
 *  @param  void 
 *  @return  void
 */
//-------------------------------------------------------------------------

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
static int MUC_ResetPhy(void *pBase)
{
    uint32_t u4Reg = 0;
    uint8_t nDevCtl = 0;
#define MT5396_FBGA
#ifdef MT5396_FBGA
    uint32_t *pu4MemTmp;
    pu4MemTmp = (uint32_t *)0xf0060200;
    *pu4MemTmp = 0x00cfff00;
#endif

    //Soft Reset, RG_RESET for Soft RESET
    u4Reg = MGC_PHY_Read32(pBase,(uint32_t)0x68);
    u4Reg |=   0x00004000; 
    u4Reg &=  ~0x00004000; 
    MGC_PHY_Write32(pBase, (uint32_t)0x68, u4Reg);

    //otg bit setting
    u4Reg = MGC_PHY_Read32(pBase,(uint32_t)0x6C);
    u4Reg &= ~0x3f3f;
    u4Reg |=  0x3f2c;
    MGC_PHY_Write32(pBase, (uint32_t)0x6C, u4Reg);

    //suspendom control
    u4Reg = MGC_PHY_Read32(pBase,(uint32_t)0x68);
    u4Reg &=  ~0x00040000; 
    MGC_PHY_Write32(pBase, (uint32_t)0x68, u4Reg);

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
    return 0;
}

#elif defined(CONFIG_ARCH_MT5391)
    static int MUC_ResetPhy(void *pBase)
    {
        uint32_t u4Reg;

        uint8_t nDevCtl;

        MUSB_ASSERT(pBase);

        // set REL_SUSP, USBRST
        MGC_PHY_Write32(pBase, 0x10, 0x01010000);
        MGC_PHY_Write32(pBase, 0x10, 0x00010000);
        // set PLL_EN
        MGC_PHY_Write32(pBase, 0x04, 0x48000406);

        MGC_MISC_Write32(pBase, 0xE0, 0x1818);

        // make sure DRAM clock is on. 
        u4Reg = MGC_MISC_Read32(pBase, M_REG_AUTOPOWER);
        MGC_MISC_Write32(pBase, M_REG_AUTOPOWER, (u4Reg | M_REG_AUTOPOWER_DRAMCLK));

        // reset all h/w register.
        MGC_MISC_Write32(pBase, M_REG_SOFTRESET, M_REG_SOFT_RESET_ACTIVE);

        // wait PHY clock to be valid before deactive reset.
        do
        {
            // check if this board have usb module.
            u4Reg = MGC_PHY_Read32(pBase, M_REG_PHYC5);

            if ((u4Reg &0xFFFF) != 0x1F0F)
            {
                return  - 1;
            }

            u4Reg = MGC_MISC_Read32(pBase, M_REG_AUTOPOWER);
        }
        while (!(u4Reg &M_REG_AUTOPOWER_PHYCLK));

        MGC_MISC_Write32(pBase, M_REG_SOFTRESET, M_REG_SOFT_RESET_DEACTIVE);

        // set CLKMODE = USB_INTA2_CK
        MGC_PHY_Write32(pBase, 0x04, 0x48010406);

        MGC_PHY_Write32(pBase, M_REG_PHYC5, 0x1F13);

        // open session.
        nDevCtl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);
        nDevCtl |= MGC_M_DEVCTL_SESSION;
        MGC_Write8(pBase, MGC_O_HDRC_DEVCTL, nDevCtl);

        msleep(10);

        MGC_PHY_Write32(pBase, M_REG_PHYC5, 0xF03);

        // set high speed disconnect threshold = 655 mV.
        u4Reg = MGC_PHY_Read32(pBase, 0x08);
        u4Reg &= ~0x07000000;
        MGC_PHY_Write32(pBase, 0x08, u4Reg);

        // Set HS RCV always to be enabled while USB operating in HS mode
        u4Reg = MGC_PHY_Read32(pBase, 0x0C);
        u4Reg |= 0x10;
        MGC_PHY_Write32(pBase, 0x0C, u4Reg);

        // Set high speed disconnect mid deglitch and
        // increase the HS TX rise time.
        MGC_PHY_Write32(pBase, 0x18, 0x00000802);

        MGC_MISC_Write32(pBase, 0xE0, 0x1A18);

        /*
        0x20029404 H     PHYC1   PHY CONTROL REGISTER 1
        for JPEG module.

        bit 7:5  DIV_CK_CNT[2:0] Divide Clock Counter
        Divide Counter for 240Mhz Clock output
        0: Disable DIV_CK.
        1: DIV_CK = 240Mhz.

        bit 4        DIV_CK_ON   Divide Clock On
        Force Divide Counter for 240Mhz Clock on
        0: Normal (PLL ON when not in suspend mode)
        1: Force ON (PLL always on).
         */
        u4Reg = MGC_PHY_Read32(pBase, 0x04);
        u4Reg |= 0x30;
        MGC_PHY_Write32(pBase, 0x04, u4Reg);

        return 0;
    }

#elif defined(CONFIG_ARCH_MT5363)
    static int MUC_ResetPhy(void *pBase)
    {
        uint32_t u4Reg;
        uint8_t nDevCtl;

        MUSB_ASSERT(pBase);

        // make sure DRAM clock is on. 
        u4Reg = MGC_MISC_Read32(pBase, M_REG_AUTOPOWER);
        MGC_MISC_Write32(pBase, M_REG_AUTOPOWER, (u4Reg | M_REG_AUTOPOWER_DRAMCLK));

        // reset all h/w register.
        MGC_MISC_Write32(pBase, M_REG_SOFTRESET, M_REG_SOFT_RESET_ACTIVE);

        msleep(1);

        MGC_MISC_Write32(pBase, M_REG_SOFTRESET, M_REG_SOFT_RESET_DEACTIVE);

        u4Reg = MGC_PHY_Read32(pBase, 0x0C);
        u4Reg &= (uint32_t)(~0x000020FF);
        u4Reg |= (uint32_t)0x00000026;
        MGC_PHY_Write32(pBase, 0x0C, u4Reg);

        // set REL_SUSP, USBRST
        u4Reg = MGC_PHY_Read32(pBase, 0x00);
        u4Reg &= (uint32_t)(~0x00030000);
        u4Reg |= (uint32_t)0x00030000;
        MGC_PHY_Write32(pBase, 0x00, u4Reg);

        msleep(10);

        u4Reg = MGC_PHY_Read32(pBase, 0x00);
        u4Reg &= (uint32_t)(~0x00030000);
        u4Reg |= (uint32_t)0x00010000;
        MGC_PHY_Write32(pBase, 0x00, u4Reg);

        msleep(10);

        // Set RG_HSDISC_DEGL.
        // Reset RG_EARLY_HSTX_I
        // Set RG_PLL_STABLE    
        u4Reg = MGC_PHY_Read32(pBase, 0x04);
        u4Reg &= (uint32_t)(~0x00004000);
        u4Reg |= (uint32_t)0x01000002;
        MGC_PHY_Write32(pBase, 0x04, u4Reg);

        // deglich 2n.
        // enable serializer when EN_HS_TX_1=1.
        // enable TX current when EN_HS_TX_I=1 or EN_HS_TERM=1.
        u4Reg = MGC_PHY_Read32(pBase, 0x10);
        u4Reg &= (uint32_t)(~0x03030F00);
        u4Reg |= (uint32_t)0x02020400;
        MGC_PHY_Write32(pBase, 0x10, u4Reg);

        // wait PHY clock to be valid before deactive reset.
        do
        {
            // check if this board have usb module.
            u4Reg = MGC_PHY_Read32(pBase, M_REG_PHYC5);
            if ((u4Reg &0xFFFF) != 0x1F0F)
            {
                DBG(1, "MUSB pBase = 0x%08X init fail.\n", (uint32_t)pBase);
                return  - 1;
            }
            u4Reg = MGC_MISC_Read32(pBase, M_REG_AUTOPOWER);
        }
        while (!(u4Reg &M_REG_AUTOPOWER_PHYCLK));

        // RG_HS_RCV_EN_MODE1
        u4Reg = MGC_PHY_Read32(pBase, 0x14);
        u4Reg &= (uint32_t)(~0x0000000C);
        u4Reg |= (uint32_t)0x0000000C;
        MGC_PHY_Write32(pBase, 0x14, u4Reg);

        // DISC_TH = 615 mV.
        u4Reg = MGC_PHY_Read32(pBase, 0x24);
        u4Reg &= (uint32_t)(~0x01C00000);
        u4Reg |= (uint32_t)0x01C00000;
        MGC_PHY_Write32(pBase, 0x24, u4Reg);

        MGC_PHY_Write32(pBase, M_REG_PHYC5, 0x1F13);

        // open session.
        nDevCtl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);
        nDevCtl |= MGC_M_DEVCTL_SESSION;
        MGC_Write8(pBase, MGC_O_HDRC_DEVCTL, nDevCtl);

        msleep(10);

        MGC_PHY_Write32(pBase, M_REG_PHYC5, 0xF03);

        #ifndef MTK_MHDRC   
            // turn on device address control at host mode.        
            MGC_MISC_Write32(pBase, M_REG_ACCESSUNIT, 0x12);
        #endif 

        MGC_MISC_Write32(pBase, 0xE0, 0x1818);

        // set user's customization HS_TERMC parameter.
        /*    if (MGC_bHSTERMC <= 0x7)
        {
        u4Reg = MGC_PHY_Read32(pBase, 0x10);
        u4Reg &= ~0x00000070;
        u4Reg |= MGC_bHSTERMC<<4;
        MGC_PHY_Write32(pBase, 0x10, u4Reg);                        
        }
         */
        //-------------------------------------------------------------
        // Platform dependent
        //-------------------------------------------------------------
        // ACD Setting 0x20029410[4:6] RG_CALIB_SELE1 440mv
        //    #if defined(CC_MT5363) || defined(CC_MT8225)
        u4Reg = MGC_PHY_Read32(pBase, 0x10);
        u4Reg &= ~0x00000070;
        u4Reg |= 0x00000020;
        MGC_PHY_Write32(pBase, 0x10, u4Reg);
        //    #endif

        // Don't stop USB pll when entering suspend mode
        //    #if defined(CC_MT5363) || defined(CC_MT8225)
        u4Reg = MGC_PHY_Read32(pBase, 0x04);
        u4Reg |= 0x00000001;
        MGC_PHY_Write32(pBase, 0x04, u4Reg);
        //    #endif

        {
            // Enable USB 5V power. It should be enabled in uboot.
            uint32_t *pu4MemTmp;
            pu4MemTmp = (uint32_t*)0xF0028080;
            *pu4MemTmp =  *pu4MemTmp | 0x2;
            pu4MemTmp = (uint32_t*)0xF0028084;
            *pu4MemTmp =  *pu4MemTmp | 0x2;
        }

        DBG(5, "MUSB pBase = 0x%08X init ok.\n", (uint32_t)pBase);

        return 0;
    }
#elif defined(CONFIG_ARCH_MT5365) || defined(CONFIG_ARCH_MT5395)
    static int MUC_ResetPhy(void *pBase)
    {
        uint32_t u4Reg;
        uint8_t nDevCtl;
        #if 0//def CONFIG_ARCH_MT5395
            // HS Slew Rate AutoCalibration
            uint32_t u4SlewRate;
            uint32_t u4Clk;
            uint32_t u4FmOrg;
            uint32_t *pu4MemTmp;
        #endif 

        MUSB_ASSERT(pBase);

        // disable interrupt for ICE reload
        u4Reg = MGC_Read32(pBase, M_REG_INPKT_ENABLE);
        u4Reg &= 0x06ffffff;
        MGC_Write32(pBase, M_REG_INPKT_ENABLE, u4Reg);


        u4Reg = MGC_PHY_Read32(pBase, 0x24);
        printk("[USB]MUSB ResetPhy u4Reg 0x24= 0x%x\n", u4Reg);

        u4Reg &= 0x01c00000;
        if (u4Reg == 0x01c00000)
        {
            MGC_MISC_Write32(pBase, M_REG_SOFTRESET, 0x2);
            msleep(1);
            MGC_MISC_Write32(pBase, M_REG_SOFTRESET, M_REG_SOFT_RESET_DEACTIVE);
            #ifndef MTK_MHDRC   
                // turn on device address control at host mode.        
                MGC_MISC_Write32(pBase, M_REG_ACCESSUNIT, 0x12);
            #endif 

            MGC_MISC_Write32(pBase, 0xE0, 0x1818);
            // On MT5365/5395, we only do the soft reset for one time.
            return TRUE;
        }


        // make sure DRAM clock is on. 
        u4Reg = MGC_MISC_Read32(pBase, M_REG_AUTOPOWER);
        MGC_MISC_Write32(pBase, M_REG_AUTOPOWER, (u4Reg | M_REG_AUTOPOWER_DRAMCLK));

        // reset all h/w register.
        // MGC_MISC_Write32(pBase, M_REG_SOFTRESET, M_REG_SOFT_RESET_ACTIVE);

        // (void)MUSB_Sleep(1);

        // MGC_MISC_Write32(pBase, M_REG_SOFTRESET, M_REG_SOFT_RESET_DEACTIVE);

        // set REL_SUSP, USBRST
        u4Reg = MGC_PHY_Read32(pBase, 0x00);
        u4Reg &= (uint32_t)(~0x00030000);
        u4Reg |= (uint32_t)0x00030000;
        MGC_PHY_Write32(pBase, 0x00, u4Reg);

        msleep(10);

        u4Reg = MGC_PHY_Read32(pBase, 0x00);
        u4Reg &= (uint32_t)(~0x00030000);
        u4Reg |= (uint32_t)0x00010000;
        MGC_PHY_Write32(pBase, 0x00, u4Reg);

        msleep(10);

        // Set RG_HSDISC_DEGL. --> cancelled on mt5365/mt5366
        // Reset RG_EARLY_HSTX_I
        // Set RG_PLL_STABLE    
        u4Reg = MGC_PHY_Read32(pBase, 0x04);
        u4Reg &= (uint32_t)(~0x01004000);
        u4Reg |= (uint32_t)0x00000002;
        MGC_PHY_Write32(pBase, 0x04, u4Reg);

        // wait PHY clock to be valid before deactive reset.
        do
        {
            // check if this board have usb module.
            u4Reg = MGC_PHY_Read32(pBase, M_REG_PHYC5);
            if ((u4Reg &0xFFFF) != 0x1F0F)
            {
                DBG(1, "MUSB pBase = 0x%08X init fail.\n", (uint32_t)pBase);
                return FALSE;
            }
            u4Reg = MGC_MISC_Read32(pBase, M_REG_AUTOPOWER);
        }
        while (!(u4Reg &M_REG_AUTOPOWER_PHYCLK));

        // DISC_TH = 615 mV.
        u4Reg = MGC_PHY_Read32(pBase, 0x24);
        u4Reg &= (uint32_t)(~0x01C00000);
        u4Reg |= (uint32_t)0x01C00000;
        MGC_PHY_Write32(pBase, 0x24, u4Reg);

        MGC_PHY_Write32(pBase, M_REG_PHYC5, 0x1F13);

        // open session.
        nDevCtl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);
        nDevCtl |= MGC_M_DEVCTL_SESSION;
        MGC_Write8(pBase, MGC_O_HDRC_DEVCTL, nDevCtl);

        msleep(10);

        MGC_PHY_Write32(pBase, M_REG_PHYC5, 0xF03);

        #ifndef MTK_MHDRC   
            // turn on device address control at host mode.        
            MGC_MISC_Write32(pBase, M_REG_ACCESSUNIT, 0x12);
        #endif 

        MGC_MISC_Write32(pBase, 0xE0, 0x1818);

        #if 1
            // HS Slew Rate
            u4Reg = MGC_PHY_Read32(pBase, 0x10);
            u4Reg &= (uint32_t)(~0x0000000e);
            u4Reg |= (uint32_t)0x00000008;
            MGC_PHY_Write32(pBase, 0x10, u4Reg);
        #else 
            // HS Slew Rate AutoCalibration
            u4Reg = MGC_PHY_Read32(pBase, 0x1c);
            u4Reg &= (uint32_t)(~0x00500000);
            u4Reg |= (uint32_t)0x00500000;
            MGC_PHY_Write32(pBase, 0x1c, u4Reg);

            msleep(1);

            #ifdef CONFIG_ARCH_MT5365
                // Frequency Meter
                pu4MemTmp = (uint32_t*)0xf000d1c0;
                u4Reg =  *pu4MemTmp;
                u4FmOrg = u4Reg;
                u4Reg &= (uint32_t)(~0x3f000000);
                if ((uint32_t)pBase == 0xf0029000)
                {
                    u4Reg |= (uint32_t)0x14008000;
                }
                else
                {
                    u4Reg |= (uint32_t)0x1B008000;
                }
                *pu4MemTmp = u4Reg;
            #else //if defined(CONFIG_ARCH_MT5395)
                // Frequency Meter
                pu4MemTmp = (uint32_t*)0xf000d1c0;
                u4Reg =  *pu4MemTmp;
                u4FmOrg = u4Reg;
                u4Reg &= (uint32_t)(~0x00003f00);
                if ((uint32_t)pBase == 0xf0029000)
                {
                    u4Reg |= (uint32_t)0x0000a900;
                }
                else
                {
                    u4Reg |= (uint32_t)0x0000aa00;
                }
                *pu4MemTmp = u4Reg;
            #endif 

            msleep(1);

            pu4MemTmp = (uint32_t*)0xf000d1cc;
            u4Reg =  *pu4MemTmp;
            u4Clk = u4Reg * 27 / 2048;

            // x/u4Clk = 5/130 --> x = 5*u4Clk/130
            u4SlewRate = ((((5 *u4Clk / 130) *2) + 1) / 2);

            u4Reg = MGC_PHY_Read32(pBase, 0x10);
            u4Reg &= (uint32_t)(~0x0000000e);
            u4Reg |= (u4SlewRate << 1);
            MGC_PHY_Write32(pBase, 0x10, u4Reg);

            pu4MemTmp = (uint32_t*)0xf00d1c0;
            *pu4MemTmp = u4FmOrg;
            DBG(1, "MUSB Auto Calibration : 0x%x (%dMhz)\n", u4SlewRate, u4Clk);
        #endif 
        u4Reg = MGC_PHY_Read32(pBase, 0x08);
        u4Reg &= (uint32_t)(~0x00000070);
        u4Reg |= (uint32_t)0x00000050;
        MGC_PHY_Write32(pBase, 0x08, u4Reg);

        u4Reg = MGC_PHY_Read32(pBase, 0x10);
        u4Reg &= (uint32_t)(~0x0000001c0);
        u4Reg |= (uint32_t)0x000000c0;
        MGC_PHY_Write32(pBase, 0x10, u4Reg);

        // FS/LS change
        u4Reg = MGC_PHY_Read32(pBase, 0x18);
        u4Reg &= (uint32_t)(~0x00043300);
        u4Reg |= (uint32_t)0x00042200;
        MGC_PHY_Write32(pBase, 0x18, u4Reg);

        //-------------------------------------------------------------
        // Platform dependent
        //-------------------------------------------------------------

        DBG(5, "MUSB pBase = 0x%08X init ok.\n", (uint32_t)pBase);

        return TRUE;
    }
#endif 


//-------------------------------------------------------------------------
/** MUC_Irq
 *  system usb irq entry point.
 *  @param   irq                interrupt number.
 *  @param   *_prHcd       point to mtkhcd data structure.
 *  @param   *regs           cpu register before interrupt.
 *  @return  irqreturn_t    irq status: IRQ_HANDLED means successfully irq handled.
 */
//-------------------------------------------------------------------------
static irqreturn_t MUC_Irq(struct usb_hcd *hcd)
{
    MGC_LinuxCd *pThis = hcd_to_musbstruct(hcd);

    const void *pBase = pThis->pRegs;
    uint32_t nSource = 0;
    uint8_t bIntrUsbValue;
    uint16_t wIntrTxValue;
    uint16_t wIntrRxValue;
    uint32_t dwLineState;
    uint8_t bIndex;
    uint8_t bShift = 0;
    uint32_t reg;

    if ((pThis == NULL) || (hcd->state != HC_STATE_RUNNING))
    {
        printk("[USB] IRQ happens after hcd removed !\n");
        if (pThis->pDmaController->pfDmaControllerIsr != MGC_HsDmaControllerIsr)
        {
            printk("[USB] pThis->pDmaController->pfDmaControllerIsr != MGC_HsDmaControllerIsr\n");
        } 

        return IRQ_NONE;
    }


    /* save index */
    bIndex = MGC_Read8(pBase, MGC_O_HDRC_INDEX);

    /* ### DMA intr handler added */
    if ((pThis->pDmaController) && (pThis->pDmaController->pfDmaControllerIsr))
    {
        if (pThis->pDmaController->pfDmaControllerIsr != MGC_HsDmaControllerIsr)
        {
            printk("[USB] pThis->pDmaController->pfDmaControllerIsr != MGC_HsDmaControllerIsr\n");
            return IRQ_NONE;
        }

        if (pThis->pDmaController->pfDmaControllerIsr(pThis->pDmaController->pPrivateData))
        {
            nSource |= 1;
        }
    }

    bIntrUsbValue = MGC_Read8(pBase, MGC_O_HDRC_INTRUSB);
    wIntrTxValue = MGC_Read16(pBase, MGC_O_HDRC_INTRTX);
    wIntrRxValue = MGC_Read16(pBase, MGC_O_HDRC_INTRRX);

    if (bIntrUsbValue)
    {
        MGC_Write8(pBase, MGC_O_HDRC_INTRUSB, bIntrUsbValue);

        bIntrUsbValue &= MGC_Read8(pBase, MGC_O_HDRC_INTRUSBE);
    }

    if (wIntrTxValue)
    {
        MGC_Write16(pBase, MGC_O_HDRC_INTRTX, wIntrTxValue);

        wIntrTxValue &= MGC_Read16(pBase, MGC_O_HDRC_INTRTXE);
    }

    if (wIntrRxValue)
    {
        MGC_Write16(pBase, MGC_O_HDRC_INTRRX, wIntrRxValue);

        wIntrRxValue &= MGC_Read16(pBase, MGC_O_HDRC_INTRRXE);
    }

    if (bIntrUsbValue == (MGC_M_INTR_CONNECT | MGC_M_INTR_DISCONNECT))
    {
        DBG(2, "[USB] Connect/Disconnet interrupt = 0x%X.\n", bIntrUsbValue);

        // treat it as disconnect interrupt only.
        bIntrUsbValue &= ~MGC_M_INTR_CONNECT;
    }

    if (bIntrUsbValue &MGC_M_INTR_CONNECT)
    {
        #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
        dwLineState = LINESTATE_FS_SPEED;
        #else
        dwLineState = MGC_PHY_Read32(pBase, M_REG_PHYC5);
        dwLineState &= M_REG_LINESTATE_MASK;
        #endif
        printk("[USB] Device is connected\n");
        DBG(2, "[USB] Connect interrupt  = 0x%X.\n", bIntrUsbValue);

        //kill timer.
        MGC_DelTimer(pThis);

        if ((!pThis->bInsert) && (dwLineState != LINESTATE_DISCONNECT))
        {
            pThis->bCheckInsert = 0;
            MGC_SetTimer(pThis, MGC_CheckConnect, (unsigned long)pThis, MGC_CHECK_INSERT_DEBOUNCE);
        }

        // When connect and disconnet happen at the same time,
        // we only handle connect event. This will happen at Vbus not stable when cable is  inserted.
        bIntrUsbValue &= ~(MGC_M_INTR_CONNECT);
    }
    else if (bIntrUsbValue &MGC_M_INTR_DISCONNECT)
    // disconnect event.   
    {
        printk("[USB] Device is disconnected\n");
        DBG(2, "[USB] Disconnect interrupt  = 0x%X.\n", bIntrUsbValue);

        /* <lawrance.liu@mediatek.com> 20100516 */
        /* Patched from BDP, To avoid card reader plug/unplug problem */
        pThis->dwVirtualHubPortStatus = (1 << USB_PORT_FEAT_POWER) | (1 << USB_PORT_FEAT_C_CONNECTION);
        pThis->dwVirtualHubPortStatus &= ~(1 << USB_PORT_FEAT_CONNECTION);
        usb_hcd_poll_rh_status(hcd);

        //kill timer.
        MGC_DelTimer(pThis);

        if (!pThis->bInsert)
        {
            // no need to handle this disconnect event.
            bIntrUsbValue &= ~(MGC_M_INTR_DISCONNECT);
        }
        else
        {
            pThis->bInsert = FALSE;
        }

        MGC_SetTimer(pThis, MGC_UnlinkInvalidUrb, (unsigned long)pThis, 20);
    }

    nSource |= bIntrUsbValue | wIntrTxValue | wIntrRxValue;

    if (!nSource)
    {
        /* restore index */
        MGC_Write8(pBase, MGC_O_HDRC_INDEX, bIndex);

        return IRQ_HANDLED;
    }

    if (bIntrUsbValue)
    {
        MGC_ServiceUSBIntr(pThis, bIntrUsbValue);
    }

    /* handle tx/rx on endpoints; each bit of wIntrTxValue is an endpoint, 
     * endpoint 0 first (p35 of the manual) bc is "SPECIAL" treatment; 
     * WARNING: when operating as device you might start receving traffic 
     * to ep0 before anything else happens so be ready for it */

    reg = wIntrTxValue;
    if (reg &1)
    {
        /* EP0 */
        MGC_ServiceDefaultEnd(pThis);
    }

    /* TX on endpoints 1-15 */
    bShift = 1;
    reg >>= 1;

    while (reg)
    {
        if (reg &1)
        {
            MGC_ServiceTxAvail(pThis, bShift);
        }

        reg >>= 1;
        bShift++;
    }

    /* RX on endpoints 1-15 */
    reg = wIntrRxValue;
    bShift = 1;
    reg >>= 1;

    while (reg)
    {
        if (reg &1)
        {
            MGC_ServiceRxReady(pThis, bShift);
        }

        reg >>= 1;
        bShift++;
    }

    /* restore index */
    MGC_Write8(pBase, MGC_O_HDRC_INDEX, bIndex);

    return IRQ_HANDLED;
}

//-------------------------------------------------------------------------
/** MUC_urb_enqueue
 *  queue and process user's urb sequentially.
 *  @param   *hcd           system usb structure.
 *  @param   *urb             user's request block. 
 *  @param   mem_flags    malloc flag. 
 *  @retval   0        Success
 *  @retval  others Fail
 */
//-------------------------------------------------------------------------
static int MUC_urb_enqueue(struct usb_hcd *hcd, struct urb *pUrb, gfp_t mem_flags)
{
    MGC_LinuxCd *pThis = hcd_to_musbstruct(hcd);
    int rc;
    uint32_t u4UsbIrqEnable = 0;
    unsigned long flags = 0;
    unsigned long flags1 = 0;
    MGC_LinuxLocalEnd *pEnd = NULL;
    unsigned int pipe;
    uint8_t i = 0;
    
    for(i = 0; i < MUSB_PORT_NUM; i++)
    {
        if((uint32_t)pThis->pRegs == (uint32_t)MUC_aLinuxController[i].pBase)
            pThisGlobal[i] = pThis;
    }

    if (pThisGlobal == NULL)
    {
        return USB_ST_URB_REQUEST_ERROR;
    }

    if (pUrb == NULL)
    {
        return USB_ST_URB_REQUEST_ERROR;
    } 

    DBG(2, "[USB] pUrb=0x%p, pUrb->hcpriv=%p proto=%s\n", pUrb, pUrb->hcpriv, MGC_DecodeUrbProtocol(pUrb));

    SPIN_LOCK_IRQSAVE(&pThis->Lock, flags, pThis->nIrq);

    /* find appropriate local endpoint to do it */
    pEnd = MGC_FindEnd(pThis, pUrb);

    if (pEnd == NULL)
    {
        pipe = pUrb ? pUrb->pipe: 0;
        pUrb->status = USB_ST_URB_REQUEST_ERROR;
        ERR("[USB] ==> no resource for proto=%d, addr=%d, end=%d\n", usb_pipetype(pipe), usb_pipedevice(pipe), usb_pipeendpoint(pipe));

        SPIN_UNLOCK_IRQRESTORE(&pThis->Lock, flags, pThis->nIrq);
        return USB_ST_URB_REQUEST_ERROR;
    }

    /* if no root device, assume this must be it */
    if (!pThis->pRootDevice)
    {
        pThis->pRootDevice = pUrb->dev;
    }

    //printk("Enqueue pUrb=0x%X.\n", (uint32_t)pUrb);
    //spin_lock(&pEnd->Lock);
    spin_lock_irqsave(&pEnd->Lock, flags1);

    pEnd->mem_flags = mem_flags;

    rc = MGC_ScheduleUrb(pThis, pEnd, pUrb);

    //spin_unlock_irq(&pEnd->Lock);
    spin_unlock_irqrestore(&pEnd->Lock, flags1);

    SPIN_UNLOCK_IRQRESTORE(&pThis->Lock, flags, pThis->nIrq);

    return rc;
}

//-------------------------------------------------------------------------
/** MUC_urb_dequeue
 *  kill user's urb.
 *  @param   *hcd           system usb structure.
 *  @param   *urb             user's request block. 
 *  @retval   0        Success
 *  @retval  others Fail
 */
//-------------------------------------------------------------------------
static int MUC_urb_dequeue(struct usb_hcd *hcd, struct urb *pUrb, int status)
{
    MGC_LinuxCd *pThis = hcd_to_musbstruct(hcd);
    int result;
    MGC_LinuxLocalEnd *pEnd;
    unsigned long flags = 0;
    unsigned long flags1 = 0;

    DBG(2, "[USB] <== pUrb=%p\n", pUrb);

    if (!pUrb)
    {
        DBG(2, "[USB] ==> invalid urb%p, pUrb->hcpriv=%p\n", pUrb, (pUrb) ? pUrb->hcpriv: NULL);
        return  - EINVAL;
    } 

    if (!pUrb->dev || !pUrb->dev->bus)
    {
        DBG(2, "[USB] ==>\n");
        return  - ENODEV;
    }

    if (!pThis)
    {
        ERR("[USB] ==> pThis is null: stopping before unlink\n");
        return  - ENODEV;
    }

    pEnd = (MGC_LinuxLocalEnd*)pUrb->hcpriv;

    spin_lock_irqsave(&pThis->Lock, flags);
    result = MGC_CheckDequeueurb(pEnd, pUrb, status);
    if (result)
    {
        pUrb->status = status;
        spin_unlock_irqrestore(&pThis->Lock, flags);
        return result;
    }
    spin_unlock_irqrestore(&pThis->Lock, flags);
    pUrb->status = status;

    spin_lock_irqsave(&pEnd->Lock, flags1);
    result = MGC_UnlinkUrb(pThis, pUrb);
    spin_unlock_irqrestore(&pEnd->Lock, flags1);
    
    return result;
}

//-------------------------------------------------------------------------
/** MUC_endpoint_disable
 *  disable user's ep.
 *  @param   *hcd             system usb hcd structure.
 *  @param   *hdev           system hcd device structure.
 *  @param   epnum          ep number.
 *  @return   void
 */
//-------------------------------------------------------------------------
static void MUC_endpoint_disable(struct usb_hcd *hcd, struct usb_host_endpoint *hep)
{
    MGC_LinuxCd *pThis = hcd_to_musbstruct(hcd);
    uint32_t u4UsbIrqEnable = 0;
    unsigned long flags = 0;
    unsigned long flags1 = 0;
    MGC_LinuxLocalEnd *pEnd;
    struct urb *pUrb = NULL;
    MGC_LinuxUrbList *pUrbList;
    
    if (hep && hep->hcpriv)
    {
        SPIN_LOCK_IRQSAVE(&pThis->Lock, flags, pThis->nIrq);

        pEnd = (MGC_LinuxLocalEnd*)hep->hcpriv;
        if (pEnd)
        {
            //spin_lock(&pEnd->Lock);
            spin_lock_irqsave(&pEnd->Lock, flags1);

            //printk("\nDisable h/w %s ep%d.\n", (pEnd->bIsTx ? "Out": "In"), pEnd->bEnd);           
            if (pEnd->dev)            
            {
                // Clean all the URB on the endpoint.
                if (pEnd->pCurrentUrb)
                {
                    pUrb = pEnd->pCurrentUrb;
                    pUrb->status = -ESHUTDOWN;
                    MGC_UnlinkUrb(pThis, pUrb);
                }            
                while (!list_empty(&pEnd->list))
                {
                    pUrbList = list_entry(pEnd->list.next, MGC_LinuxUrbList, list);
                    pUrb = pUrbList->pUrb;               
                    if (pUrb)
                    {
                        pUrb->status = -ESHUTDOWN;
                        MGC_UnlinkUrb(pThis, pUrb);
                    }
                }            
                
                pEnd->dev = NULL;
            }

            //spin_unlock(&pEnd->Lock);
            spin_unlock_irqrestore(&pEnd->Lock, flags1);
            
        }

        hep->hcpriv = NULL;

        SPIN_UNLOCK_IRQRESTORE(&pThis->Lock, flags, pThis->nIrq);
    }
    return ;
}
//-------------------------------------------------------------------------
/** MUC_get_frame
 *  Get sof frame variable.
 *  @param   *hcd             system usb hcd structure.
 *  @return   frame number
 */
//-------------------------------------------------------------------------
static int MUC_get_frame(struct usb_hcd *hcd)
{
    MGC_LinuxCd *pThis = hcd_to_musbstruct(hcd);

    /* wrong except while periodic transfers are scheduled;
     * never matches the on-the-wire frame;
     * subject to overruns.
     */
    //return pThis->frame;
    (void)pThis;
    return 0;
} 

//-------------------------------------------------------------------------
/** MUC_hub_status_data
 *  Virtual root hub status api.
 *  @param   *hcd             system usb hcd structure.
 *  @param   *buf             point to status buffer.
 *  @retval   0        No change hub status.
 *  @retval   1      Change hub status.
 */
//-------------------------------------------------------------------------
static int MUC_hub_status_data(struct usb_hcd *hcd, char *buf)
{
    MGC_LinuxCd *pThis = hcd_to_musbstruct(hcd);
    unsigned long flags;
    int32_t retval;
    uint32_t u4UsbIrqEnable = 0;

    SPIN_LOCK_IRQSAVE(&pThis->Lock, flags, pThis->nIrq);

    if (!(pThis->dwVirtualHubPortStatus &MTK_PORT_C_MASK))
    {
        retval = 0;
    } 
    else
    {
        // Hub port status change. Port 1 change detected.
        *buf = (1 << 1);

        DBG(3, "[USB] port status 0x%08X has changes\n", pThis->dwVirtualHubPortStatus);
        retval = 1;
    }

    SPIN_UNLOCK_IRQRESTORE(&pThis->Lock, flags, pThis->nIrq);

    return retval;
}

//-------------------------------------------------------------------------
/** MUC_hub_control
 *  Virtual root hub control api.
 *  @param   *hcd             system usb hcd structure.
 *  @param   typeReq        request code.
 *  @param   wValue         request value.
 *  @param   wIndex         request index.
 *  @param   *buf             point to status buffer.
 *  @param   wLength       data length.
 *  @retval   0        Success.
 *  @retval   others   Fail.
 */
//-------------------------------------------------------------------------
static int MUC_hub_control(struct usb_hcd *hcd, uint16_t typeReq, uint16_t wValue, uint16_t wIndex, char *buf, uint16_t wLength)
{
    MGC_LinuxCd *pThis = hcd_to_musbstruct(hcd);
    void *pBase = pThis->pRegs;
    int32_t retval = 0;
    unsigned long flags;
    uint32_t u4UsbIrqEnable = 0;
    uint8_t power;

    DBG(3, "[USB] = 0x%X, 0x%X.\n", typeReq, wValue);

    SPIN_LOCK_IRQSAVE(&pThis->Lock, flags, pThis->nIrq);

    switch (typeReq)
    {
        case ClearHubFeature:
        case SetHubFeature:
            switch (wValue)
            {
            case C_HUB_OVER_CURRENT:
            case C_HUB_LOCAL_POWER:
                break;

            default:
                goto error;
            }

            break;

        case ClearPortFeature:
            if (wIndex != 1 || wLength != 0)
            {
                goto error;
            }

            switch (wValue)
            {
            case USB_PORT_FEAT_ENABLE:
                pThis->dwVirtualHubPortStatus &= (1 << USB_PORT_FEAT_POWER);
                break;

            case USB_PORT_FEAT_SUSPEND:
                if (!(pThis->dwVirtualHubPortStatus &(1 << USB_PORT_FEAT_SUSPEND)))
                {
                    break;
                }

                /* 20 msec of resume/K signaling, other irqs blocked */
                DBG(3, "[USB] start resume...\n");
                break;

            case USB_PORT_FEAT_POWER:
                break;

            case USB_PORT_FEAT_C_ENABLE:
            case USB_PORT_FEAT_C_SUSPEND:
            case USB_PORT_FEAT_C_CONNECTION:
            case USB_PORT_FEAT_C_OVER_CURRENT:
            case USB_PORT_FEAT_C_RESET:
                break;

            default:
                goto error;
            }

            pThis->dwVirtualHubPortStatus &= ~(1 << wValue);
            break;

        case GetHubDescriptor:
            MUC_hub_descriptor(pThis, (struct usb_hub_descriptor*)buf);
            break;

        case GetHubStatus:
            *(__le32*)buf = cpu_to_le32(0);
            break;

        case GetPortStatus:
            if (wIndex != 1)
            {
                goto error;
            }

            *(__le32*)buf = cpu_to_le32(pThis->dwVirtualHubPortStatus);

            #ifndef VERBOSE

                if (*(uint16_t*)(buf + 2))
                 /* only if wPortChange is interesting */
            #endif 

            {
                DBG(3, "[USB] GetPortStatus = 0x%08X.\n", pThis->dwVirtualHubPortStatus);
            }

            break;

        case SetPortFeature:
            if (wIndex != 1 || wLength != 0)
            {
                goto error;
            }

            switch (wValue)
            {
            case USB_PORT_FEAT_ENABLE:
                break;

            case USB_PORT_FEAT_SUSPEND:
                if (pThis->dwVirtualHubPortStatus &(1 << USB_PORT_FEAT_RESET))
                {
                    goto error;
                }

                if (!(pThis->dwVirtualHubPortStatus &(1 << USB_PORT_FEAT_ENABLE)))
                {
                    goto error;
                }

                DBG(3, "[USB] suspend...\n");
                break;

            case USB_PORT_FEAT_POWER:
                break;

            case USB_PORT_FEAT_RESET:
                if (pThis->dwVirtualHubPortStatus &(1 << USB_PORT_FEAT_SUSPEND))
                {
                    goto error;
                }

                if (!(pThis->dwVirtualHubPortStatus &(1 << USB_PORT_FEAT_POWER)))
                {
                    break;
                }

                // reset port.
                printk("[USB] Reset Port \n");
                power = MGC_Read8(pBase, MGC_O_HDRC_POWER);
                power &= ~(MGC_M_POWER_SUSPENDM | MGC_M_POWER_ENSUSPEND);
                MGC_Write8(pBase, MGC_O_HDRC_POWER, power | MGC_M_POWER_RESET);
                MGC_SetTimer(pThis, MGC_ResetOff, (unsigned long)pThis, 80);

                break;

            default:
                goto error;
            }

            pThis->dwVirtualHubPortStatus |= 1 << wValue;
            break;

        default:
            error: 
            /* "protocol stall" on error */
            retval =  - EPIPE;
    }

    SPIN_UNLOCK_IRQRESTORE(&pThis->Lock, flags, pThis->nIrq);
    return retval;
}

//-------------------------------------------------------------------------
/** MUC_bus_suspend
 *  Virtual root hub suspend.
 *  @param   *hcd             system usb hcd structure.
 *  @retval   0       Success.
 *  @retval   1      Fail.
 */
//-------------------------------------------------------------------------
static int MUC_bus_suspend(struct usb_hcd *hcd)
{
    // SOFs off
    return 0;
} 

//-------------------------------------------------------------------------
/** MUC_bus_resume
 *  Virtual root hub resume.
 *  @param   *hcd             system usb hcd structure.
 *  @retval   0       Success.
 *  @retval   1      Fail.
 */
//-------------------------------------------------------------------------
static int MUC_bus_resume(struct usb_hcd *hcd)
{
    // SOFs on
    return 0;
} 

//-------------------------------------------------------------------------
/** MUC_hub_descriptor
 *  get root hub descriptor.
 *  @param  *pHcd           mtkhcd pointer.
 *  @param   *desc             point to hub descriptor buffer.
 *  @return   void
 */
//-------------------------------------------------------------------------
static void MUC_hub_descriptor(MGC_LinuxCd *pThis, struct usb_hub_descriptor *desc)
{
    uint16_t temp = 0;

    desc->bDescriptorType = 0x29;
    desc->bHubContrCurrent = 0;

    desc->bNbrPorts = 1;
    desc->bDescLength = 9;

    /* per-port power switching (gang of one!), or none */
    desc->bPwrOn2PwrGood = 10;

    /* no overcurrent errors detection/handling */
    temp = 0x0011;

    desc->wHubCharacteristics = (__force __u16)cpu_to_le16(temp);

    /* two bitmaps:  ports removable, and legacy PortPwrCtrlMask */
    desc->bitmap[0] = 1 << 1;
    desc->bitmap[1] = ~0;
} 

/*-------------------------------------------------------------------------*/
static int MUC_start(struct usb_hcd *hcd)
{
    DBG(3, "[USB] hcd = 0x%08X.\n", (uint32_t)hcd);

    hcd->state = HC_STATE_RUNNING;

    return 0;
} 

/*-------------------------------------------------------------------------*/
static void MUC_stop(struct usb_hcd *hcd)
{
    DBG(3, "[USB] hcd = 0x%08X.\n", (uint32_t)hcd);
} 

/*-------------------------------------------------------------------------*/
static int MUC_hcd_probe(struct platform_device *pdev)
{
    uint32_t id = 0;
    int32_t retval;
    MUSB_LinuxController *pController;
    struct usb_hcd *hcd;
    MGC_LinuxCd *pThis;
    uint8_t *pBase;
    uint32_t u4Reg = 0;
    MGC_HsDmaController *pDmaController;

    DBG(3, "[USB] pdev = 0x%08X.\n", (uint32_t)pdev);

    if (!pdev)
    {
        return  - ENODEV;
    } 

    id = pdev->id;
    if (id >= (sizeof(MUC_aLinuxController) / sizeof(MUSB_LinuxController)))
    {
        return  - ENODEV;
    }
    pController = &MUC_aLinuxController[id];

    retval = MUC_ResetPhy(pController->pBase);
    if (retval < 0)
    {
        return  - ENODEV;
    }

    pController->bSupport = TRUE;

    /* allocate and initialize hcd */
    hcd = usb_create_hcd(&MUC_hc_driver, &pdev->dev, dev_name(&pdev->dev));
    if (!hcd)
    {
        return  - ENOMEM;
    }

    hcd->rsrc_start = (uint32_t)pController->pBase;
    pThis = hcd_to_musbstruct(hcd);
    pThis->pRegs = pController->pBase;
    pThis->nIrq = pController->dwIrq;
    spin_lock_init(&pThis->Lock);
    spin_lock_init(&pThis->SelectEndLock);

    init_timer(&(pThis->Timer));

    // init endpoint, fifo.
    MGC_InitEnd(pThis);

    retval = usb_add_hcd(hcd, pController->dwIrq, IRQF_SHARED | IRQF_SAMPLE_RANDOM);
    if (retval != 0)
    {
        return  - ENOMEM;
    }

    pBase = (uint8_t*)pThis->pRegs;

    MGC_Write16(pBase, MGC_O_HDRC_INTRTXE, pThis->wEndMask);

    // IntrRxE, IntrUSB, and IntrUSBE are the same 32 bits group.
    // Tricky: Set 0 in all write clear field in IntrUSB field. Prevent to clear IntrUSB.
    u4Reg = MGC_M_INTR_SUSPEND | MGC_M_INTR_RESUME | MGC_M_INTR_BABBLE | 
    /* MGC_M_INTR_SOF | */
    MGC_M_INTR_CONNECT | MGC_M_INTR_DISCONNECT | MGC_M_INTR_SESSREQ | MGC_M_INTR_VBUSERROR;
    u4Reg = (u4Reg << 24) | (pThis->wEndMask &0xfffe);
    MGC_Write32(pBase, MGC_O_HDRC_INTRRXE, u4Reg);

    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
    u4Reg = MGC_Read32(pBase, M_REG_INTRLEVEL1EN);
    #ifdef MUSB_QMU
       u4Reg |= 0x2f; 
    #else
    u4Reg |= 0x0f;
    #endif
    MGC_Write32(pBase, M_REG_INTRLEVEL1EN, u4Reg);
    DBG(1, "[USB] LEVEL1En reg:0x0x*X \n", MGC_Read8(pBase, M_REG_INTRLEVEL1EN));
    #endif
    DBG(1, "[USB] INTRUSBE reg:0x0x%X \n", MGC_Read8(pBase, MGC_O_HDRC_INTRUSBE));
    DBG(1, "[USB] INTRTXE  reg:0x0x%X \n", MGC_Read8(pBase, MGC_O_HDRC_INTRTXE));
    DBG(1, "[USB] INTRRXE  reg:0x0x%X \n", MGC_Read8(pBase, MGC_O_HDRC_INTRRXE));

    /* enable high-speed/low-power and start session */
    MGC_Write8(pBase, MGC_O_HDRC_POWER, MGC_M_POWER_SOFTCONN | MGC_M_POWER_HSENAB | MGC_M_POWER_SUSPENDM | MGC_M_POWER_ENSUSPEND);

    /* enable high-speed/low-power and start session & suspend IM host */
    MGC_Write8(pBase, MGC_O_HDRC_DEVCTL, MGC_M_DEVCTL_SESSION);

    // DMA controller init.
    pDmaController = &pController->rDma;
    memset(pDmaController, 0, sizeof(MGC_HsDmaController));
    pDmaController->bChannelCount = MGC_HSDMA_CHANNELS;
    
    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
    {
        uint8_t intrmask = 0;
        uint8_t i;
    	for(i=0; i<pDmaController->bChannelCount; i++)
    	{
    	    intrmask |= (1<<i); 
    	}	
    	MGC_Write8(pBase, MGC_O_HSDMA_INTR_SET, intrmask);
    }
    #endif
    
    pDmaController->pfDmaChannelStatusChanged = MGC_HsDmaChannelStatusChanged;
    pDmaController->pDmaPrivate = (void*)pThis;
    pDmaController->pCoreBase = (void*)pBase;
    pDmaController->Controller.pPrivateData = pDmaController;
    pDmaController->Controller.pfDmaAllocateChannel = MGC_HsDmaAllocateChannel;
    pDmaController->Controller.pfDmaReleaseChannel = MGC_HsDmaReleaseChannel;
    pDmaController->Controller.pfDmaProgramChannel = MGC_HsDmaProgramChannel;
    pDmaController->Controller.pfDmaGetChannelStatus = MGC_HsDmaGetChannelStatus;
    pDmaController->Controller.pfDmaControllerIsr = MGC_HsDmaControllerIsr;
    pThis->pDmaController = &pDmaController->Controller;


    #if defined(USB_WEBCAM_LOG) || defined(USB_WEBCAM_COM)
    // Force Port0 to be Full Speed when supporting webcam.
    if (pBase == MUSB_BASE1)
    {
        MGC_Write8(pBase, MGC_O_HDRC_POWER, MGC_Read8(pBase, MGC_O_HDRC_POWER) &(~(MGC_M_POWER_HSENAB)));
    }
    #endif 
    return 0;
}

/*-------------------------------------------------------------------------*/
static int MUC_hcd_remove(struct platform_device *pdev)
{
    struct usb_hcd *hcd;

    DBG(3, "[USB] pdev = 0x%08X.\n", (uint32_t)pdev);

    hcd = platform_get_drvdata(pdev);
    usb_remove_hcd(hcd);
    usb_put_hcd(hcd);
    return 0;
} 

/*-------------------------------------------------------------------------*/
static int MUC_hcd_suspend(struct platform_device *pdev, pm_message_t state)
{
    DBG(3, "[USB] pdev = 0x%08X, state.event=%d.\n", (uint32_t)pdev, state.event);
    return 0;
} 

/*-------------------------------------------------------------------------*/
static int MUC_hcd_resume(struct platform_device *pdev)
{
    DBG(3, "[USB] pdev = 0x%08X.\n", (uint32_t)pdev);
    return 0;
} 

/*-------------------------------------------------------------------------*/
static void MUC_hcd_release(struct device *dev)
{
    DBG(3, "[USB] dev = 0x%08X.\n", (uint32_t)dev);
} 

/*-------------------------------------------------------------------------*/
static int __init MUC_init(void)
{
    int32_t retval = 0;
    uint32_t nCount;
    uint32_t i;
    struct platform_device *pPlatformDev;
    static u64 dummy_mask = DMA_BIT_MASK(32);

    // Turn Off VBus & Turn On later.
    MGC_VBusInitHandler();

    if (usb_disabled())
    {
        return  - ENODEV;
    } 

    retval = platform_driver_register(&MUC_hcd_driver);
    if (retval < 0)
    {
        return retval;
    }

    nCount = sizeof(MUC_pdev) / sizeof(struct platform_device);
    for (i = 0; i < nCount; i++)
    {
        pPlatformDev = &MUC_pdev[i];
        pPlatformDev->name = MUC_HcdName;
        pPlatformDev->id = i;
        DBG(3, "[USB] pdev = 0x%08X, id = %d.\n", (uint32_t)pPlatformDev, i);
        //  This controller has DMA capability.
        pPlatformDev->dev.dma_mask = &dummy_mask; /* FIXME: for improper usb code */
        pPlatformDev->dev.coherent_dma_mask = 0xffffffff;
        pPlatformDev->dev.release = MUC_hcd_release;

        retval = platform_device_register(pPlatformDev);
        if (retval < 0)
        {
            platform_device_unregister(pPlatformDev);
            break;
        } 
    }

    if (retval < 0)
    {
        platform_driver_unregister(&MUC_hcd_driver);
    }

    return retval;
}

/*-------------------------------------------------------------------------*/
static void __exit MUC_cleanup(void)
{
    uint32_t nCount;
    uint32_t i;
    struct platform_device *pPlatformDev;

    nCount = sizeof(MUC_pdev) / sizeof(struct platform_device);
    for (i = 0; i < nCount; i++)
    {
        pPlatformDev = &MUC_pdev[i];
        DBG(3, "[USB] pdev = 0x%08X, id = %d.\n", (uint32_t)pPlatformDev, i);
        platform_device_unregister(pPlatformDev);
    } 

    platform_driver_unregister(&MUC_hcd_driver);

    return ;
}

/*-------------------------------------------------------------------------*/
module_init(MUC_init);
module_exit(MUC_cleanup);
