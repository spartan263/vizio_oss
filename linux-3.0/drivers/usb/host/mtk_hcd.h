/*-----------------------------------------------------------------------------
 *\drivers\usb\host\mtk_hcd.h
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


/** @file mtk_hcd.h
 *  This header file implements the mtk53xx USB host controller driver.
 *
 * Status:  Passed basic stress testing, works with hubs, mice, keyboards,
 * and usb-storage.
 *
 * TODO:
 * - usb suspend/resume triggered by prHcd (with USB_SUSPEND)
 * - various issues noted in the code
 * - performance work; use both register banks; ...
 * - use urb->iso_frame_desc[] with ISO transfers
 */
/*-------------------------------------------------------------------------*/
#ifndef MTK_HCD

#define MTK_HCD

#include <mach/hardware.h>
#include "mtk_qmu.h"

/*
 WARN() definition confict with include/asm-generic/bug.h
 */
 
#ifdef WARN
#undef WARN
#endif

/*-------------------------------------------------------------------------*/
/* 
* CONFIG_USB_HOST_DEBUG is open only when use dbg_config
*/
#ifndef CONFIG_USB_HOST_DEBUG
#define CONFIG_USB_HOST_DEBUG
#endif
#ifdef CONFIG_USB_HOST_DEBUG
#undef DEBUG
#define MUSB_DEBUG
#endif

#ifndef TRUE

#define TRUE                (0 == 0)

#endif // TRUE

#ifndef FALSE

#define FALSE               (0 != 0)

#endif // FALSE

#define yprintk(facility, format, args...) do {printk(facility "%s %d: " format , \
    __FUNCTION__, __LINE__ , ## args); } while (0) 
 
#define ERR(fmt,args...) yprintk(KERN_ERR,fmt, ## args)
#define INFO(fmt,args...) yprintk(KERN_INFO,fmt, ## args)

#ifdef MUSB_DEBUG
#define WARN(fmt, args...) yprintk(KERN_WARNING,fmt, ## args)

#define MGC_GetDebugEnalbe()    ((mgc_debuglevel)? TRUE: FALSE)
#define MGC_GetDebugLevel()    (mgc_debuglevel)
#define MGC_EnableDebug()    do { mgc_debuglevel = 1; } while(0) 
#define MGC_DisableDebug()    do { mgc_debuglevel = 0; } while(0)

#define _dbg_level(level)  (MGC_GetDebugEnalbe() \
	&& ((level>0 && MGC_GetDebugLevel()>=level) || MGC_GetDebugLevel()==level) )

#define xprintk(level, facility, format, args...) do { if ( _dbg_level(level) ) { \
    printk(facility "%s %d: " format , __FUNCTION__, __LINE__ , ## args); } } while (0)
    
#define DBG(level,fmt,args...) xprintk(level,KERN_INFO,fmt, ## args)
#define MUSB_ASSERT(x)   if (!(x)) BUG();

#else 

#define WARN(fmt, args...)

#define MGC_GetDebugEnalbe()    0
#define MGC_GetDebugLevel()     0
#define MGC_EnableDebug()
#define MGC_DisableDebug()

#define DBG(fmt,args...)    do {}  while (0)

#define MUSB_ASSERT(x)   	
#endif
/*20120725 USB Mass Storage Device disconnect, deadlock in usb-storage waiting sg finish,
and cause khubd thead deadlock, and can't detect USB Device again*/
#define MGC_UNLINK_URB
#define MGC_PRE_CHECK_FREE_ENDPOINT
/*
*20121126 LGE WiFi burn-in cause Device Rx Error. Doubt the Device receives 
error data from USB host becuase the data's sequence is cause the data error in 
the dual core system. So add mb() to protect when the USB Host driver write 
register's value when send data by CPU.
*/
#define MGC_MB_ADD_SUPPORT
#ifdef MGC_MB_ADD_SUPPORT
#define MU_MB()  mb() 
#define MU_RMB() rmb()
#define MU_WMB() wmb()
#else
#define MU_MB()   
#define MU_RMB() 
#define MU_WMB() 
#endif
/*-------------------------------------------------------------------------*/

/** HDRC */
#define MUSB_CONTROLLER_HDRC            1

/** MHDRC */
#define MUSB_CONTROLLER_MHDRC           2

#define MBIM_VIRT                   (IO_VIRT + 0x08000)

#define MUSB_PORT_NUM (4)

#ifndef VECTOR_USB0
    #define VECTOR_USB0 (15)
#endif 
#ifndef VECTOR_USB1
    #define VECTOR_USB1 (11)
#endif 

#ifndef VECTOR_USB2
	#define VECTOR_USB2  (32 + 16)
#endif
#ifndef VECTOR_USB3
	#define VECTOR_USB3  (32 + 17)
#endif

/*
	mt5396/mt5368  
	port 0: core = 0xf0050000, phy = 0xf0059800
	port 0: core = 0xf0051000, phy = 0xf0059900
	port 0: core = 0xf0052000, phy = 0xf0059a00
	port 0: core = 0xf0053000, phy = 0xf0059b00			
*/
/*
	mt5396/mt5368  
	0xf0060204[4:3] = 10   Port1 <--> Por2 
		
	port 0: core = 0xf0050000, phy = 0xf0059800
	port 1: core = 0xf0052000, phy = 0xf0059a00
	port 2: core = 0xf0051000, phy = 0xf0059900
	port 3: core = 0xf0053000, phy = 0xf0059b00 		
*/

/*
1. confirm chip_boot.c  0xf0060204
2.  open the #define in uboot and loader at the same time. "mgc_hdrc.h", "x_hal_5381.h"  
*/
#define MUSB_BASE                   (IO_VIRT + 0x50000)
#define MUSB_BASE1                  (IO_VIRT + 0x51000)
#define MUSB_BASE2                  (IO_VIRT + 0x52000)
#define MUSB_BASE3                  (IO_VIRT + 0x53000)
    #define MUSB_COREBASE            (0)  
    #define MUSB_DMABASE             (0x200)
    #define MUSB_PHYBASE             (0x9800)
    #define MUSB_PORT0_PHYBASE       (0)
	#define MUSB_PORT1_PHYBASE       (0x100) //    -- MAC 1    
	#define MUSB_PORT2_PHYBASE       (0x200) //    -- MAC 2
	#define MUSB_PORT3_PHYBASE       (0x300)	

#define MUSB_DEFAULT_ADDRESS_SPACE_SIZE 0x00001000

/*
*     MUSBHDRC Register map 
*/

/* Common USB registers */

#define MGC_O_HDRC_FADDR        0x00    /* 8-bit */
#define MGC_O_HDRC_POWER        0x01    /* 8-bit */

#define MGC_O_HDRC_INTRTX       0x02    /* 16-bit */
#define MGC_O_HDRC_INTRRX       0x04
#define MGC_O_HDRC_INTRTXE      0x06
#define MGC_O_HDRC_INTRRXE      0x08
#define MGC_O_HDRC_INTRUSB      0x0A    /* 8 bit */
#define MGC_O_HDRC_INTRUSBE     0x0B    /* 8 bit */
#define MGC_O_HDRC_FRAME        0x0C
#define MGC_O_HDRC_INDEX        0x0E    /* 8 bit */
#define MGC_O_HDRC_TESTMODE     0x0F    /* 8 bit */

#define MGC_O_HDRC_DUMMY1       0xE0    /* 32 bit */

#define MGC_M_DUMMY1_SOFFORCE   (1<<12)

/* Additional Control Registers */

#define MGC_O_HDRC_DEVCTL     0x60 /* 8 bit */

/* These are actually indexed: */
#define MGC_O_HDRC_TXFIFOSZ     0x62    /* 8-bit (see masks) */
#define MGC_O_HDRC_RXFIFOSZ     0x63    /* 8-bit (see masks) */
#define MGC_O_HDRC_TXFIFOADD    0x64    /* 16-bit offset shifted right 3 */
#define MGC_O_HDRC_RXFIFOADD    0x66    /* 16-bit offset shifted right 3 */

/* Endpoint registers */
#define MGC_O_HDRC_TXMAXP       0x00
#define MGC_O_HDRC_TXCSR        0x02
#define MGC_O_HDRC_TXCSR2       0x03
#define MGC_O_HDRC_CSR0         MGC_O_HDRC_TXCSR /* re-used for EP0 */
#define MGC_O_HDRC_RXMAXP       0x04
#define MGC_O_HDRC_RXCSR        0x06
#define MGC_O_HDRC_RXCSR2       0x07
#define MGC_O_HDRC_RXCOUNT      0x08
#define MGC_O_HDRC_COUNT0       MGC_O_HDRC_RXCOUNT         /* re-used for EP0 */
#define MGC_O_HDRC_TXTYPE       0x0A
#define MGC_O_HDRC_TYPE0        MGC_O_HDRC_TXTYPE         /* re-used for EP0 */
#define MGC_O_HDRC_TXINTERVAL   0x0B
#define MGC_O_HDRC_NAKLIMIT0    MGC_O_HDRC_TXINTERVAL /* re-used for EP0 */
#define MGC_O_HDRC_RXTYPE       0x0C
#define MGC_O_HDRC_RXINTERVAL   0x0D
#define MGC_O_HDRC_FIFOSIZE     0x0F
#define MGC_O_HDRC_CONFIGDATA   MGC_O_HDRC_FIFOSIZE /* re-used for EP0 */

//level 1 interrupt
#define M_REG_INTRLEVEL1 0xA0
#define M_REG_INTRLEVEL1EN 0xA4

#define MGC_M_L1INTS_DPDM_INT     0x00000080

//queue mode
#define M_REG_QISAR   0xc00
#define M_REG_QIMR    0xc04

/*
Begin of MTK created register definition.
*/
//  MTK Notice: Max Liao, 2006/08/29.
//  added for non-32 bits aligned fifo read/write. Base addr = USB_BASE = 0x20029600.
#define M_REG_FIFOBYTECNT    0x690

//  MTK Notice: Max Liao, 2006/08/18.
//  support one to one mapping ep and device address. Base addr = USB_BASE = 0x20029800.
#define M_REG_EP0ADDR       0x90
#define M_REG_EP1ADDR       0x94
#define M_REG_EP2ADDR       0x98
#define M_REG_EP3ADDR       0x9C
#define M_REG_EP4ADDR       0xA0
#define M_REG_EPXADDR(X)    (M_REG_EP0ADDR + ((X) << 2))

//  MTK Notice: Max Liao, 2006/05/22.
//  read PHY line state. Base addr = USB_PHYBASE = 0x20029400.
#define M_REG_PHYC0             0x00
#define M_REG_PHYC1             0x04
#define M_REG_PHYC2             0x08
#define M_REG_PHYC3             0x0c
#define M_REG_PHYC4             0x10
#if defined(CONFIG_ARCH_MT5391)
#define M_REG_PHYC5             0x14
#else
#define M_REG_PHYC5             0x20
#endif
#define M_REG_PHYC6             0x18
#define M_REG_PHYC7             0x1c

#define M_REG_U2PHYDMON1                0x74
#define M_REG_LINESTATE_MASK	        0x00c00000
#define LINESTATE_DISCONNECT            0x00000000
#define LINESTATE_FS_SPEED              0x00400000
#define LINESTATE_LS_SPEED              0x00800000
#define LINESTATE_HWERROR               0x00c00000


//  MTK Notice: Max Liao, 2006/05/29.
//  MTK add: soft reset register. Base addr = USB_MISCBASE = 0x20029600.
#define M_REG_SOFTRESET             0x0
#define M_REG_SOFT_RESET_ACTIVE     0x0
#define M_REG_SOFT_RESET_DEACTIVE   0x3

//  MTK add: access unit control register. Base addr = USB_MISCBASE = 0x20029600.
/*
0x20029604
bit[1:0] reg_size  : should be always  2'b10
bit[4] Function address enable : enable function address selected by endpoint, default :1(enable)
bit[8] Force DRAM read byte enable : Force Byte enable = 0xFFFF during DRAM read, default: 0(disable)
bit[9]: Enable group2 DRAM agent, Select group2 DRAM agent, default: 0(group3)
*/
#define M_REG_ACCESSUNIT        0x4
#define M_REG_ACCESSUNIT_8BIT   0x0
#define M_REG_ACCESSUNIT_16BIT  0x1
#define M_REG_ACCESSUNIT_32BIT  0x2
#define M_REG_DEV_ADDR_MODE     0x10

//  MTK add: data toggle control register. Base addr = USB_MISCBASE = 0x20029600.
#define M_REG_RXDATATOG		            0x80
#define M_REG_TXDATATOG		            0x84
#define M_REG_SET_DATATOG(ep, v)	    (((1 << ep) << 16) | (v << ep))

//  MTK add: request packet number. Base addr = USB_DMABASE = 0x20029A00.
#define M_REG_REQPKT(ep)                      (0x100 + ((ep)<<2))

//  MTK Notice: Max Liao, 2006/09/19.
//  MTK add: IN packet interrupt. Base addr = USB_MISCBASE = 0x20029600.
/*
0x20029608[15:0] : Interrupt mask ( default : 16'hFFFF, will change to 16'h0 later)
0x2002960C [15:0]: interrupt status ( default : 0)
bit[15:0] RX IN endpoint request bit[0] : EP0, bit[1] : EP1, ...
Notes: Endpoint number is logical endpoint number, not physical.
*/
#define M_REG_INPKT_ENABLE                          0x8
#define M_REG_INPKT_STATUS                          0xC

//  MTK Notice: Max Liao, 2006/09/19.
//  MTK add: Powe down register. Base addr = USB_MISCBASE = 0x20029600.
/*
bit 0 : Enable DRAM clock, default : 1<Enable>
bit 1 : Enable Hardware Auto-PowerDown/Up, default : 0<Disable>, Auto-Clear after PowerUp
bit 2 : Read Only, 1: PHY Clock valid, 0: PHY clock is off.
After turn off DRAM clock, only 0x20029680 registers is accessable.
Write other registers makes no effect, and read other registers return constant value, 32'hDEAD_BEAF
*/
#define M_REG_AUTOPOWER                     0x80
#define M_REG_AUTOPOWER_DRAMCLK             0x01
#define M_REG_AUTOPOWER_ON                  0x02
#define M_REG_AUTOPOWER_PHYCLK              0x04
/*
End of MTK created register definition.
*/

/* Added in HDRC 1.9(?) & MHDRC 1.4 */
/* ULPI pass-through */
#define MGC_O_HDRC_ULPI_VBUSCTL     0x70
#define MGC_O_HDRC_ULPI_REGDATA     0x74
#define MGC_O_HDRC_ULPI_REGADDR     0x75
#define MGC_O_HDRC_ULPI_REGCTL      0x76

#define M_REG_PERFORMANCE1 0x70
#define M_REG_PERFORMANCE2 0x72
#define M_REG_PERFORMANCE3 0x74


/* extended config & PHY control */
#define MGC_O_HDRC_ENDCOUNT  0x78
#define MGC_O_HDRC_DMARAMCFG 0x79
#define MGC_O_HDRC_PHYWAIT   0x7A
#define MGC_O_HDRC_PHYVPLEN  0x7B   /* units of 546.1 us */
#define MGC_O_HDRC_HSEOF1    0x7C   /* units of 133.3 ns */
#define MGC_O_HDRC_FSEOF1    0x7D   /* units of 533.3 ns */
#define MGC_O_HDRC_LSEOF1    0x7E   /* units of 1.067 us */

/* "bus control" registers */
#define MGC_O_MHDRC_TXFUNCADDR  0x00
#define MGC_O_MHDRC_TXHUBADDR   0x02
#define MGC_O_MHDRC_TXHUBPORT   0x03

#define MGC_O_MHDRC_RXFUNCADDR  0x04
#define MGC_O_MHDRC_RXHUBADDR   0x06
#define MGC_O_MHDRC_RXHUBPORT   0x07

/*
*     MUSBHDRC Register bit masks
*/

/* POWER */

#define MGC_M_POWER_ISOUPDATE   0x80
#define MGC_M_POWER_SOFTCONN    0x40
#define MGC_M_POWER_HSENAB      0x20
#define MGC_M_POWER_HSMODE      0x10
#define MGC_M_POWER_RESET       0x08
#define MGC_M_POWER_RESUME      0x04
#define MGC_M_POWER_SUSPENDM    0x02
#define MGC_M_POWER_ENSUSPEND   0x01

/* TESTMODE */

#define MGC_M_TEST_FORCE_HOST   0x80
#define MGC_M_TEST_FIFO_ACCESS  0x40
#define MGC_M_TEST_FORCEFS      0x20
#define MGC_M_TEST_FORCEHS      0x10
#define MGC_M_TEST_PACKET       0x08
#define MGC_M_TEST_K            0x04
#define MGC_M_TEST_J            0x02
#define MGC_M_TEST_SE0_NAK      0x01

/* allocate for double-packet buffering (effectively doubles assigned _SIZE) */
#define MGC_M_FIFOSZ_DPB    0x10
/* allocation size (8, 16, 32, ... 4096) */
#define MGC_M_FIFOSZ_SIZE   0x0f

/* CSR0 in Peripheral and Host mode */

#define MGC_M_CSR0_FLUSHFIFO      0x0100

/* New in 15-July-2005 (MHDRC v1.4 HDRC ) */
#define MGC_M_CSR0_H_NO_PING 0x0800

/* TxType/RxType */
#define MGC_M_TYPE_PROTO        0x30
#define MGC_S_TYPE_PROTO        4
#define MGC_M_TYPE_REMOTE_END   0xf

/* CONFIGDATA */

#define MGC_M_CONFIGDATA_MPRXE      0x80 /* auto bulk pkt combining */
#define MGC_M_CONFIGDATA_MPTXE      0x40 /* auto bulk pkt splitting */
/* TODO: was this in an older HDRC?
#define MGC_M_CONFIGDATA_DMA        0x40
*/
#define MGC_M_CONFIGDATA_BIGENDIAN  0x20
#define MGC_M_CONFIGDATA_HBRXE      0x10
#define MGC_M_CONFIGDATA_HBTXE      0x08
#define MGC_M_CONFIGDATA_DYNFIFO    0x04
#define MGC_M_CONFIGDATA_SOFTCONE   0x02
#define MGC_M_CONFIGDATA_UTMIDW     0x01 /* data width 0 => 8bits, 1 => 16bits */

/* TXCSR in Peripheral and Host mode */

#define MGC_M_TXCSR_AUTOSET       0x8000
#define MGC_M_TXCSR_ISO           0x4000
#define MGC_M_TXCSR_MODE          0x0000
#define MGC_M_TXCSR_DMAENAB       0x1000
#define MGC_M_TXCSR_FRCDATATOG    0x0800
#define MGC_M_TXCSR_DMAMODE       0x0400
#define MGC_M_TXCSR_CLRDATATOG    0x0040
#define MGC_M_TXCSR_FLUSHFIFO     0x0008
#define MGC_M_TXCSR_FIFONOTEMPTY  0x0002
#define MGC_M_TXCSR_TXPKTRDY      0x0001

/* TXCSR in Peripheral mode */

#define MGC_M_TXCSR_P_INCOMPTX    0x0080
#define MGC_M_TXCSR_P_SENTSTALL   0x0020
#define MGC_M_TXCSR_P_SENDSTALL   0x0010
#define MGC_M_TXCSR_P_UNDERRUN    0x0004

/* TXCSR in Host mode */
#define MGC_M_TXCSR_H_WR_DATATOGGLE 0x0200
#define MGC_M_TXCSR_H_DATATOGGLE    0x0100
#define MGC_M_TXCSR_H_NAKTIMEOUT    0x0080
#define MGC_M_TXCSR_H_RXSTALL       0x0020
#define MGC_M_TXCSR_H_ERROR         0x0004

/* RXCSR in Peripheral and Host mode */

#define MGC_M_RXCSR_AUTOCLEAR     0x8000
#define MGC_M_RXCSR_DMAENAB       0x2000
#define MGC_M_RXCSR_DISNYET       0x1000
#define MGC_M_RXCSR_PID_ERR       0x1000
#define MGC_M_RXCSR_DMAMODE       0x0800
#define MGC_M_RXCSR_INCOMPRX      0x0100
#define MGC_M_RXCSR_CLRDATATOG    0x0080
#define MGC_M_RXCSR_FLUSHFIFO     0x0010
#define MGC_M_RXCSR_DATAERR       0x0008
#define MGC_M_RXCSR_FIFOFULL      0x0002
#define MGC_M_RXCSR_RXPKTRDY      0x0001

/* RXCSR in Peripheral mode */

#define MGC_M_RXCSR_P_ISO         0x4000
#define MGC_M_RXCSR_P_SENTSTALL   0x0040
#define MGC_M_RXCSR_P_SENDSTALL   0x0020
#define MGC_M_RXCSR_P_OVERRUN     0x0004

/* RXCSR in Host mode */

#define MGC_M_RXCSR_H_AUTOREQ       0x4000
#define MGC_M_RXCSR_H_WR_DATATOGGLE 0x0400
#define MGC_M_RXCSR_H_DATATOGGLE    0x0200
#define MGC_M_RXCSR_H_RXSTALL       0x0040
#define MGC_M_RXCSR_H_REQPKT        0x0020
#define MGC_M_RXCSR_H_ERROR         0x0004

/* MUSB Double Buffer support, only for mass storage Device */
#define MUSB_TX_DPB_SUPPORT     TRUE
#define MUSB_RX_DPB_SUPPORT     TRUE

/* 
        5368/5396
        	P0,P1: 5+1, Note: This Is Physical Endpoint Number.
        	P2,P3: 8+1, Note: This Is Physical Endpoint Number.
        	P0, P1, P2, P3 Fifosize = ((5*(512+512)) + 64) Bytes. 
        5398/5388	
        	P0,P1,P2,P3: Tx 5+1, Rx 7+1
        	P0, P1, P2, P3 Fifosize = ((5*(512+512)) + 64) Bytes.
*/
#if defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880) || defined(CONFIG_ARCH_MT5881)
#define MUSB_C_NUM_EPS  (8)
#define MUSB_C_NUM_TX_EPS  (6)
#define MUSB_C_NUM_RX_EPS  (8) 
#else
#define MUSB_C_NUM_EPS  (9)
#define MUSB_C_NUM_TX_EPS  (9)
#define MUSB_C_NUM_RX_EPS  (9) 
#endif
#define MGC_END_MSD   (pThis->bEndTxCount - 1) /* this is non-configurable */
#define MGC_END0_FIFOSIZE    (64) /* this is non-configurable */
#define MGC_ENDX_FIFOSIZE    (5*1024) /* total fifo size exclude ep0. */
/* Reserved for mass storage with the last 1024 bytes on fifo */
#define MGC_END_MSD_FIFOADDR (MGC_END0_FIFOSIZE+MGC_ENDX_FIFOSIZE-1024) 
#define MGC_TOTAL_FIFOSIZE    (MGC_ENDX_FIFOSIZE + MGC_END0_FIFOSIZE)/* this is non-configurable */

#define MGC_M_FIFO_EP0      0x20

#define MGC_O_DRC_INDEX     0x0E
#define MGC_O_DRC_FIFOSIZE  0x1F

/* Interrupt register bit masks */
#define MGC_M_INTR_SUSPEND      0x01
#define MGC_M_INTR_RESUME       0x02
#define MGC_M_INTR_RESET        0x04
#define MGC_M_INTR_BABBLE       0x04
#define MGC_M_INTR_SOF          0x08
#define MGC_M_INTR_CONNECT      0x10
#define MGC_M_INTR_DISCONNECT   0x20
#define MGC_M_INTR_SESSREQ      0x40
#define MGC_M_INTR_VBUSERROR    0x80 /* FOR SESSION END */

#define MGC_M_INTR_EP0          0x01   /* FOR EP0 INTERRUPT */

/* DEVCTL */

#define MGC_M_DEVCTL_BDEVICE    0x80
#define MGC_M_DEVCTL_FSDEV      0x40
#define MGC_M_DEVCTL_LSDEV      0x20
#define MGC_M_DEVCTL_HM         0x04
#define MGC_M_DEVCTL_HR         0x02
#define MGC_M_DEVCTL_SESSION    0x01

/* CSR0 */
#define MGC_M_CSR0_TXPKTRDY       0x0002
#define MGC_M_CSR0_RXPKTRDY       0x0001

/* CSR0 in Peripheral mode */
#define MGC_M_CSR0_P_SVDSETUPEND  0x0080
#define MGC_M_CSR0_P_SVDRXPKTRDY  0x0040
#define MGC_M_CSR0_P_SENDSTALL    0x0020
#define MGC_M_CSR0_P_SETUPEND     0x0010
#define MGC_M_CSR0_P_DATAEND      0x0008
#define MGC_M_CSR0_P_SENTSTALL    0x0004

/* CSR0 in Host mode */
#define MGC_M_CSR0_H_NAKTIMEOUT   0x0080
#define MGC_M_CSR0_H_STATUSPKT    0x0040
#define MGC_M_CSR0_H_REQPKT       0x0020
#define MGC_M_CSR0_H_ERROR        0x0010
#define MGC_M_CSR0_H_SETUPPKT     0x0008
#define MGC_M_CSR0_H_RXSTALL      0x0004

/*
*  CID input signal indicates the ID pin of mini-A/B connector
*  on FDRC, 'CID' is available in DevCtl, 
*  on HDRC, 'B-Device' in DevCtl is valid only while a session is in progress
*/
#define MGC_CID_UNKNOWN    2
#define MGC_CID_A_DEVICE   0
#define MGC_CID_B_DEVICE   1

/*
*  A_DEVICE, B_DEVICE must be qualified by CID_VALID for valid context.
*  x is a pointer to the core object.
*/

#define MGC_A_DEVICE(x)         ((x)->cid == CID_A_DEVICE)
#define MGC_B_DEVICE(x)         ((x)->cid == CID_B_DEVICE)

/* Vbus values */
#define MGC_VBUS_BELOW_SESSION_END  0
#define MGC_VBUS_ABOVE_SESSION_END  1
#define MGC_VBUS_ABOVE_AVALID       2
#define MGC_VBUS_ABOVE_VBUS_VALID   3
#define MGC_VBUS_ERROR             0xff

#define FEATURE_SOFT_CONNECT    1
#define FEATURE_DMA_PRESENT     2
#define FEATURE_HDRC_FS         4
#define FEATURE_HIGH_BW         8
#define FEATURE_DFIFO           16
#define FEATURE_MULTILAYER      32
#define FEATURE_I2C             64

#define MUSB_URB_QUEUE_SIZE   (50)

//add for queue support
#define MUSB_EP_OFFSET(_epnum, _offset)	\
	(0x100 + (0x10*(_epnum)) + (_offset))

#define MUSB_RXCSR_H_WZC_BITS	\
	(MGC_M_RXCSR_H_RXSTALL | MGC_M_RXCSR_H_ERROR \
	| MGC_M_RXCSR_DATAERR | MGC_M_RXCSR_RXPKTRDY)

//end

/* 
*  DRC register access macros
*/

/* Get offset for a given FIFO */
#define MGC_FIFO_OFFSET(_bEnd) (MGC_M_FIFO_EP0 + (_bEnd * 4))

#define MGC_END_OFFSET(_bEnd, _bOffset) (0x100 + (0x10*_bEnd) + _bOffset)

#define MGC_BUSCTL_OFFSET(_bEnd, _bOffset)	(0x480 + (8*_bEnd) + _bOffset)

/* indexed vs. flat register model */
#define MGC_SelectEnd(_pBase, _bEnd)                    \
MGC_Write8(_pBase, MGC_O_HDRC_INDEX, _bEnd)

#define MGC_ReadCsr8(_pBase, _bOffset, _bEnd)           \
MGC_Read8(_pBase, (_bOffset + (0x10 * _bEnd) + 0x100))

#define MGC_ReadCsr16(_pBase, _bOffset, _bEnd)          \
MGC_Read16(_pBase, (_bOffset + (0x10 * _bEnd) + 0x100))

#define MGC_WriteCsr8(_pBase, _bOffset, _bEnd, _bData)  \
MGC_Write8(_pBase, (_bOffset + (0x10 * _bEnd) + 0x100), _bData)

#define MGC_WriteCsr16(_pBase, _bOffset, _bEnd, _bData) \
MGC_Write16(_pBase, (_bOffset + (0x10 * _bEnd) + 0x100), _bData)

#define MGC_VBUS_MASK            0x18                   /* DevCtl D4 - D3 */

#define MGC_END0_START  0x0
#define MGC_END0_OUT    0x2
#define MGC_END0_IN     0x4
#define MGC_END0_STATUS 0x8
#define MGC_END0_DMA_IN     0x10
#define MGC_END0_DMA_OUT     0x20

#define MGC_END0_STAGE_SETUP        0x0
#define MGC_END0_STAGE_TX           0x2
#define MGC_END0_STAGE_RX           0x4
#define MGC_END0_STAGE_STATUSIN     0x8
#define MGC_END0_STAGE_STATUSOUT    0xf
#define MGC_END0_STAGE_STALL_BIT    0x10

/* obsolete */
#define MGC_END0_STAGE_DATAIN      MGC_END0_STAGE_TX
#define MGC_END0_STAGE_DATAOUT     MGC_END0_STAGE_RX

#define MGC_CHECK_INSERT_DEBOUNCE   100
#define MGC_MAX_ERR_RETRIES         200

#define USB_EVENTS_UNLINK_INVALID_URB (0x01 << 0)
#define USB_EVENTS_CHECK_CONNECT      (0x01 << 1)
#define USB_EVENTS_RESET_OFF          (0x01 << 2)
#define USB_EVENTS_VBUS_ON            (0x01 << 3)
#define USB_EVENTS_BUS_RESUME         (0x01 << 4)
#define USB_EVENTS_BUS_RESUME_OFF     (0x01 << 5)
/*-------------------------------------------------------------------------*/
#define MIN_JIFFIES  ((msecs_to_jiffies(2) > 1) ? msecs_to_jiffies(2) : 2)

/* the virtual root hub timer IRQ checks for hub status */

#define MTK_PORT_C_MASK               \
((1 << USB_PORT_FEAT_C_CONNECTION)   \
| (1 << USB_PORT_FEAT_C_ENABLE)       \
| (1 << USB_PORT_FEAT_C_SUSPEND)      \
| (1 << USB_PORT_FEAT_C_OVER_CURRENT) \
| (1 << USB_PORT_FEAT_C_RESET))

#define MGC_SPEED_HS 1
#define MGC_SPEED_FS 2
#define MGC_SPEED_LS 3

#define MGC_PHY_Read32(_pBase, r)      \
    *((volatile uint32_t *)(((uint32_t)_pBase) + (r)))

#define MGC_PHY_Write32(_pBase, r, v)  \
    (*((volatile uint32_t *)(((uint32_t)_pBase) + (r))) = v)

#define MGC_DMA_Read32(_pBase, r)      \
    *((volatile uint32_t *)(((uint32_t)_pBase) + (MUSB_DMABASE)+ (r)))

#define MGC_DMA_Write32(_pBase, r, v)  \
    (*((volatile uint32_t *)(((uint32_t)_pBase) + (MUSB_DMABASE)+ (r))) = v)
    
#define MGC_BIM_READ32(_offset)         *((volatile uint32_t *)(MBIM_VIRT + _offset))
	
#define MGC_BIM_WRITE32(_offset, value)  (*((volatile uint32_t *)((MBIM_VIRT)+ (_offset))) = value)

/**
* Read an 8-bit register from the core
* @param _pBase core base address in memory
* @param _offset offset into the core's register space
* @return 8-bit datum
*/
#define MGC_Read8(_pBase, _offset) *((volatile uint8_t *)(((uint32_t)_pBase) + MUSB_COREBASE + _offset))

/**
* Read a 16-bit register from the core
* @param _pBase core base address in memory
* @param _offset offset into the core's register space
* @return 16-bit datum
*/
#define MGC_Read16(_pBase, _offset) *((volatile uint16_t *)(((uint32_t)_pBase) + MUSB_COREBASE + _offset))

/**
* Read a 32-bit register from the core
* @param _pBase core base address in memory
* @param _offset offset into the core's register space
* @return 32-bit datum
*/
#define MGC_Read32(_pBase, _offset) *((volatile uint32_t *)(((uint32_t)_pBase) + MUSB_COREBASE + _offset))

/**
* Write an 8-bit core register
* @param _pBase core base address in memory
* @param _offset offset into the core's register space
* @param _data 8-bit datum
*/
#define MGC_Write8(_pBase, _offset, _data)                                                   \
{                                                                                            \
volatile uint32_t u4TmpVar;                                                                  \
u4TmpVar = *((volatile uint32_t*)(((uint32_t)_pBase) + MUSB_COREBASE + ((_offset) & 0xFFFC))); \
u4TmpVar &= ~(((uint32_t)0xFF) << (8*((_offset) & 0x03)));                                   \
u4TmpVar |= (uint32_t)(((_data) & 0xFF) << (8*((_offset) & 0x03)));                          \
*((volatile uint32_t*)(((uint32_t)_pBase) + MUSB_COREBASE + ((_offset) & 0xFFFC))) = u4TmpVar; \
}

/**
* Write a 16-bit core register
* @param _pBase core base address in memory
* @param _offset offset into the core's register space
* @param _data 16-bit datum
*/
#define MGC_Write16(_pBase, _offset, _data)                                                  \
{                                                                                            \
volatile uint32_t u4TmpVar;                                                                  \
u4TmpVar = *((volatile uint32_t*)(((uint32_t)_pBase) + MUSB_COREBASE + ((_offset) & 0xFFFC))); \
u4TmpVar &= ~(((uint32_t)0xFFFF) << (8*((_offset) & 0x03)));                                 \
u4TmpVar |= (_data) << (8*((_offset) & 0x03));                                               \
*((volatile uint32_t*)(((uint32_t)_pBase) + MUSB_COREBASE + ((_offset) & 0xFFFC))) = u4TmpVar; \
}

/**
* Write a 32-bit core register
* @param _pBase core base address in memory
* @param _offset offset into the core's register space
* @param _data 32-bit datum
*/
#define MGC_Write32(_pBase, _offset, _data) \
(*((volatile uint32_t *)(((uint32_t)_pBase) + MUSB_COREBASE + _offset)) = _data)


#define MGC_FIFO_CNT    MGC_Write32

#define USB_HALT_ENDPOINT(_dev, _pipe_ep, _pipe_out) ((_dev)->bus->op->disable(_dev, _pipe_ep))
#define USB_RUN_ENDPOINT(_dev, _pipe_ep, _pipe_out) usb_endpoint_running(_dev, _pipe_ep, _pipe_out)
#define USB_ENDPOINT_HALTED(_dev, _pipe_ep, _pipe_out) ( 0 )

#define COMPLETE_URB(_pUrb, _p) _pUrb->complete(_pUrb, _p)
#define WAIT_MS(_ms)            mdelay(_ms)

#define USB_ISO_ASAP            0x0002
#define USB_ASYNC_UNLINK        0x0008

#define USB_ST_NOERROR              (0)
#define USB_ST_CRC                  (-EILSEQ)
#define USB_ST_BITSTUFF             (-EPROTO)
#define USB_ST_NORESPONSE           (-ETIMEDOUT)    /* device not responding/handshaking */
#define USB_ST_DATAOVERRUN          (-EOVERFLOW)
#define USB_ST_DATAUNDERRUN         (-EREMOTEIO)
#define USB_ST_BUFFEROVERRUN        (-ECOMM)
#define USB_ST_BUFFERUNDERRUN       (-ENOSR)
#define USB_ST_INTERNALERROR        (-EPROTO)       /* unknown error */
#define USB_ST_SHORT_PACKET         (-EREMOTEIO)
#define USB_ST_PARTIAL_ERROR        (-EXDEV)        /* ISO transfer only partially completed */
#define USB_ST_URB_KILLED           (-ENOENT)       /* URB canceled by user */
#define USB_ST_URB_PENDING          (-EINPROGRESS)
#define USB_ST_REMOVED              (-ENODEV)       /* device not existing or removed */
#define USB_ST_TIMEOUT              (-ETIMEDOUT)    /* communication timed out, also in urb->status**/
#define USB_ST_NOTSUPPORTED         (-ENOSYS)
#define USB_ST_BANDWIDTH_ERROR      (-ENOSPC)       /* too much bandwidth used */
#define USB_ST_URB_INVALID_ERROR    (-EINVAL)       /* invalid value/transfer type */
#define USB_ST_URB_REQUEST_ERROR    (-ENXIO)        /* invalid endpoint */
#define USB_ST_STALL                (-EPIPE)        /* pipe stalled, also in urb->status*/
#define USB_ST_NOMEMORY             (-ENOMEM)       /* 12. Out of memory */

                                                                                
#define USB_ZERO_PACKET             0x0040          /* Finish bulk OUTs always with zero length packet */

#define SUPPORT_SHARE_USBQ_DMA   // USB IP only have 4 h/w DMA channal, the last one will be shared to common USB if not used.
#define MGC_HSDMA_CHANNELS  4  // 5396 has 4 h/w DMA channel. Reseve the last one for Cmd Queue.

#define MGC_O_HSDMA_BASE    0x200
#define MGC_O_HSDMA_INTR    0x200
#define MUSB_HSDMA_CFG      0x220

#define MGC_O_HSDMA_INTR_MASK    0x201   
#define MGC_O_HSDMA_INTR_CLEAR   0x202
#define MGC_O_HSDMA_INTR_SET     0x203


#define MGC_O_HSDMA_CONTROL 4
#define MGC_O_HSDMA_ADDRESS 8
#define MGC_O_HSDMA_COUNT   0xc

#define MGC_HSDMA_CHANNEL_OFFSET(_bChannel, _bOffset) \
    (MGC_O_HSDMA_BASE + (_bChannel << 4) + _bOffset)

/* control register (16-bit): */
#define MGC_S_HSDMA_ENABLE          0
#define MGC_S_HSDMA_TRANSMIT        1
#define MGC_S_HSDMA_MODE1           2
#define MGC_S_HSDMA_IRQENABLE       3
#define MGC_S_HSDMA_ENDPOINT        4
#define MGC_S_HSDMA_BUSERROR        8
#define MGC_S_HSDMA_BURSTMODE       9
#define MGC_M_HSDMA_BURSTMODE       (3 << MGC_S_HSDMA_BURSTMODE)
#define MGC_HSDMA_BURSTMODE_UNSPEC  0
#define MGC_HSDMA_BURSTMODE_INCR4   1
#define MGC_HSDMA_BURSTMODE_INCR8   2
#define MGC_HSDMA_BURSTMODE_INCR16  3

#define MGC_HSDMA_MIN_DMA_LEN       (64)

#define MUSB_MAX_RETRIES            8

/*-------------------------------------------------------------------------*/
/**
 * DMA channel status.
 */
typedef enum 
{
    /** A channel's status is unknown */
    MGC_DMA_STATUS_UNKNOWN,
    /** A channel is available (not busy and no errors) */
    MGC_DMA_STATUS_FREE,
//  MTK Notice: Max Liao, 2007/05/14.
//  DMA mode1, and rx short packet.
    /** A channel is available (not busy and no errors) */
    MGC_DMA_STATUS_MODE1_SHORTPKT,
    /** A channel is busy (not finished attempting its transactions) */
    MGC_DMA_STATUS_BUSY,
    /** A channel aborted its transactions due to a local bus error */
    MGC_DMA_STATUS_BUS_ABORT,
    /** A channel aborted its transactions due to a core error */
    MGC_DMA_STATUS_CORE_ABORT
} MGC_DmaChannelStatus;

/**
 * DMA channel for control transfer status.
 */
typedef enum 
{
    MGC_DMA_IN,
    MGC_DMA_OUT,
    MGC_DMA_FIFO_DATAEND,
    MGC_DMA_FIFO_DATAMORE
} MGC_DmaControlStatus;

/***************************** TYPES ******************************/
typedef struct
{
    void* pController;  /* point to MGC_HsDmaController */    
    uint32_t dwStartAddress;
    uint32_t dwCount;               /* Request size */
    uint32_t dwActualLength;   /* Actual finish size */
    uint16_t wMaxPacketSize;
    MGC_DmaChannelStatus bStatus;
    uint8_t bDesiredMode;
    uint8_t bIndex;
    uint8_t bEnd;
    uint8_t bProtocol;
    uint8_t bTransmit;
} MGC_HsDmaChannel;

typedef struct _MGC_HsDmaController
{
    MGC_HsDmaChannel aChannel[MGC_HSDMA_CHANNELS];
    void* pThis;                    /* point to MGC_LinuxCd */
    uint8_t bChannelCount;
    uint8_t bmUsedChannels;
} MGC_HsDmaController;

/**
 * The device request.
 */
typedef struct __attribute__((packed)) {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} MUSB_DeviceRequest;

typedef struct
{
    struct urb *pUrb;
    struct list_head list;
} MGC_LinuxUrbList;

/**
* MGC_LinuxLocalEnd.
* Local endpoint resource.
* @field Lock spinlock
* @field pUrb current URB
* @field urb_list list
* @field dwOffset current buffer offset
* @field dwRequestSize how many bytes were last requested to move
* @field wMaxPacketSizeTx local Tx FIFO size
* @field wMaxPacketSizeRx local Rx FIFO size
* @field wPacketSize programmed packet size
* @field bIsSharedFifo TRUE if FIFO is shared between Tx and Rx
* @field bAddress programmed bus address
* @field bEnd programmed remote endpoint address
* @field bTrafficType programmed traffic type
* @field bIsClaimed TRUE if claimed
* @field bIsTx TRUE if current direction is Tx
* @field bIsReady TRUE if ready (available for new URB)
*/
typedef struct
{
    uint8_t bEnd;               /* ep number */
    uint8_t bIsTx;
    struct urb *pCurrentUrb;
    struct list_head list;

    uint32_t dwOffset;      /* offset int the current request */
    uint32_t dwRequestSize; /* request size */
    uint32_t dwIsoPacket;
    MGC_HsDmaChannel* pDmaChannel;
    uint16_t wMaxPacketSize; /* h/w prepare max packet size. */
    uint16_t wPacketSize;        /* usb maxpacket */
    uint16_t wFifoAddress;        /* FIFO start address */
    uint8_t bDoublePacketBuffer;  /* Turn on double packet buffer support */

    uint8_t bRemoteAddress;
    uint8_t bRemoteEnd;
    uint8_t bTrafficType;
    uint8_t bRetries;
    uint8_t bBusyCompleting;    /* TRUE on Tx when the current urb is completing */
	#ifdef MGC_PRE_CHECK_FREE_ENDPOINT
    uint8_t bIsOccupy; 
	#endif

#ifdef CONFIG_USB_QMU_SUPPORT
    uint8_t bCmdQEnable;
    uint8_t bStateGated;         /* TRUE: like mass storage, CBW, DATA, CSW. Need to care state transition. 
                                                              Must not IN/OUT data phase with CBW phase and
                                                              must not IN CSW phase with DATA phase.
                                                    FALSE: like ISO transfer always submit urb to command queue.*/
#endif
} MGC_LinuxLocalEnd;

#ifdef CONFIG_PM
struct mgc_context_registers 
{
	uint32_t levelen;
	uint16_t intrtxe, intrrxe;
	uint8_t power;
	uint8_t intrusbe;
	uint8_t devctl;
};
#endif

/**
* MGC_LinuxCd.
* Driver instance data.
* @field Lock spinlock
* @field Lock to protect endpoint index (MGC_SelectEnd)
* @field Timer interval timer for various things
* @field pBus pointer to Linux USBD bus
* @field RootHub virtual root hub
* @field PortServices services provided to virtual root hub
* @field nIrq IRQ number (needed by free_irq)
* @field nIsPci TRUE if PCI
* @field bIsMultipoint TRUE if multi-point core
* @field bIsHost TRUE if host
* @field bIsDevice TRUE if peripheral
* @field nIackAddr IACK address (PCI only)
* @field nIackSize size of IACK PCI region (needed by release_region)
* @field nRegsAddr address of registers PCI region (needed by release_region)
* @field nRegsSize size of registers region (needed by release_region)
* @field pIack pointer to mapped IACK region (PCI only)
* @field pRegs pointer to mapped registers
*/
typedef struct
{
    uint8_t bPortNum;
    spinlock_t SelectEndLock;
	#ifdef CONFIG_PM
	struct mgc_context_registers context;
	#endif
	struct timer_list events_timer;
    MGC_HsDmaController rDma;    
    void * pRegs;
    void * pPhyBase;                    /* USB analog base register. */
    uint32_t nIrq;    
    MGC_LinuxLocalEnd aLocalEnd[2][MUSB_C_NUM_EPS]; /*0: Rx, 1: Tx*/
    uint32_t dwVirtualHubPortStatus;
	uint32_t events;
    uint16_t wEndMask;
    uint8_t bEndTxCount;
	uint8_t bEndRxCount;
    uint8_t bRootSpeed;
    uint8_t bInsert;
    uint8_t bCheckInsert; /* retries of checking insert event */
    uint8_t bEnd0Stage;   /* end0 stage while in host or device mode */    
    uint8_t bSupportCmdQ;


    /* Customization function. */
    void (*MGC_pfVbusControl)(uint8_t bPortNum, uint8_t bOn);
    int (*MGC_pfPhyReset)(void * pBase, void * pPhyBase);    

#ifdef CONFIG_USB_QMU_SUPPORT
    Q_Port_Info rCmdQ;
    struct device *dev;        /* originate from struct platform_device *pdev  */
#endif    

} MGC_LinuxCd;

/**
* Board-specific information about a controller
* @field wType one of the MUSB_CONTROLLER_* constants
* @field pBase base address for hard-wired controller
* @field dwIrq IRQ for hard-wired controller
* @field bSupport support or not.
* @field rDma dma controller data structure.
*/
typedef struct
{
    void * pBase;
    void * pPhyBase;                    /* USB analog base register. */
    uint32_t dwIrq;
    uint8_t bSupport;
    uint8_t bEndpoint_Tx_num;
	uint8_t bEndpoint_Rx_num;
    uint8_t bHub_support;
    uint8_t bSupportCmdQ;    

    /* Customization function. */
    void (*MGC_pfVbusControl)(uint8_t bPortNum, uint8_t bOn);    
    int (*MGC_pfPhyReset)(void * pBase, void * pPhyBase);    

    MGC_LinuxCd *pThis;
} MUSB_LinuxController;

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------
static inline MGC_LinuxCd *hcd_to_musbstruct(struct usb_hcd *hcd)
{
    return (MGC_LinuxCd*)(hcd->hcd_priv);
} 

static inline struct usb_hcd *musbstruct_to_hcd(const MGC_LinuxCd *pThis)
{
    return container_of((void*)pThis, struct usb_hcd, hcd_priv);
} 
extern uint32_t MUCUST_GetControllerNum(void);
extern MUSB_LinuxController* MUCUST_GetControllerDev(uint32_t u4Index);
extern struct platform_device* MUCUST_GetPlatformDev(uint32_t u4Index);

extern struct urb *MGC_GetCurrentUrb(MGC_LinuxLocalEnd *pEnd);
extern void MGC_KickUrb(MGC_LinuxCd *pThis, struct urb *pUrb, MGC_LinuxLocalEnd *pEnd);
extern int MGC_CheckTxCsrValidForWrite(MGC_LinuxCd *pThis, uint8_t bEnd, uint8_t bState);
extern void MGC_DequeueEndurb(MGC_LinuxCd *pThis, MGC_LinuxLocalEnd *pEnd, struct urb *pUrb);
extern void MGC_CompleteUrb(MGC_LinuxCd *pThis, MGC_LinuxLocalEnd *pEnd, struct urb *pUrb);
extern struct urb *MGC_GetNextUrb(MGC_LinuxLocalEnd *pEnd);
extern int MGC_IsEndIdle(MGC_LinuxLocalEnd *pEnd);
extern int MGC_CheckFreeEndpoint(struct usb_device *dev, int configuration);

extern int MUT_init(void);
extern void MUT_exit(void);
#endif                    /* #ifndef MTK_HCD */
