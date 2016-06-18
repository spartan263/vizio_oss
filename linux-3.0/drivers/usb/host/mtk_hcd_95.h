/*-----------------------------------------------------------------------------
 *\drivers\usb\host\mtk_hcd_95.h
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

/*
 WARN() definition confict with include/asm-generic/bug.h
 */
 
 
#ifdef WARN
#undef WARN
#endif


#if defined(USB_WEBCAM_LOG) && defined(USB_WEBCAM_MAX)
#error "Only one camera could be supported at one time. !!!!!!!!!!!!!!"
#endif

/*-------------------------------------------------------------------------*/
#ifdef CONFIG_USB_DEBUG

#define DEBUG
#define MUSB_DEBUG
#else

#undef DEBUG
//#define MUSB_DEBUG
#endif

#ifdef VERBOSE

#define VDBG     DBG

#else

#define VDBG(stuff...) do{}while(0)

#endif

#ifndef TRUE

#define TRUE                (0 == 0)

#endif // TRUE

#ifndef FALSE

#define FALSE               (0 != 0)

#endif // FALSE


#define yprintk(facility, format, args...) do { printk(facility "%s %d: " format , \
    __FUNCTION__, __LINE__ , ## args); } while (0)    
#define ERR(fmt,args...) yprintk(KERN_INFO,fmt, ## args)
#define MUSB_ASSERT(x)   if (!(x)) BUG();

#ifdef MUSB_DEBUG

#define WARN(fmt, args...) yprintk(KERN_WARNING,fmt, ## args)
#define INFO(fmt,args...) yprintk(KERN_INFO,fmt, ## args)

#define STATIC
#define MGC_GetDebugLevel()    (MGC_DebugLevel)
#define MGC_EnableDebug()    do { MGC_DebugDisable=0; } while(0) 
#define MGC_DisableDebug()    do { MGC_DebugDisable=1; } while(0)

#define _dbg_level(level)  ( !MGC_DebugDisable && ((level>=-1 && MGC_GetDebugLevel()>=level) || MGC_GetDebugLevel()==level) )

#define xprintk(level, facility, format, args...) do { if ( _dbg_level(level) ) { \
    printk(facility "%s %d: " format , __FUNCTION__, __LINE__ , ## args); } } while (0)
    
#define PARANOID( x )        do {}  while (0)
#define DBG(level,fmt,args...) xprintk(level,KERN_INFO,fmt, ## args)
#define DEBUG_CODE(level, code)    do { if ( _dbg_level(level) ) { code }  } while (0)
#define TRACE(n) DEBUG_CODE(n, printk(KERN_INFO "%s:%s:%d: trace\n", \
    __FILE__, __FUNCTION__, __LINE__); )

#define ASSERT_SPINLOCK_LOCKED(_x) 
#define ASSERT_SPINLOCK_UNLOCKED(_x) 
/* debug no defined */
#define ASSERT(x)   if (!(x)) BUG();
//#define MUSB_ASSERT(x)   if (!(x)) BUG();

#else 

#define WARN(fmt, args...)
#define INFO(fmt,args...)

#define STATIC static
#define MGC_GetDebugLevel()    0
#define MGC_EnableDebug()
#define MGC_DisableDebug()

#define PARANOID( x )       do {}  while (0)
#define DBG(fmt,args...)    do {}  while (0)
#define DEBUG_CODE(x, y)    do {}  while (0)
#define TRACE(n)            do {}  while (0)

#define ASSERT_SPINLOCK_LOCKED(_x) 
#define ASSERT_SPINLOCK_UNLOCKED(_x) 

#define ASSERT(x)
//#define MUSB_ASSERT(x)
#endif

/*-------------------------------------------------------------------------*/

/** HDRC */
#define MUSB_CONTROLLER_HDRC            1

/** MHDRC */
#define MUSB_CONTROLLER_MHDRC           2

#define MBIM_VIRT                   (IO_VIRT + 0x08000)

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define MUSB_PORT_NUM (4)
#else
#define MUSB_PORT_NUM (2)
#endif

#ifndef VECTOR_USB0
    #define VECTOR_USB0 (15)
#endif 
#ifndef VECTOR_USB1
    #define VECTOR_USB1 (11)
#endif 

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
	#ifndef VECTOR_USB2
		#define VECTOR_USB2  (32 + 16)
	#endif
	#ifndef VECTOR_USB3
		#define VECTOR_USB3  (32 + 17)
	#endif
#endif


/*
	mt5396/mt5368  
	port 0: core = 0xf0050000, phy = 0xf0059800
	port 0: core = 0xf0051000, phy = 0xf0059900
	port 0: core = 0xf0052000, phy = 0xf0059a00
	port 0: core = 0xf0053000, phy = 0xf0059b00			
*/
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define MUSB_BASE                   (IO_VIRT + 0x50000)
#define MUSB_BASE1                  (IO_VIRT + 0x51000)
#define MUSB_BASE2                  (IO_VIRT + 0x52000)
#define MUSB_BASE3                  (IO_VIRT + 0x53000)
    #define MUSB_COREBASE            (0)  
    #define MUSB_DMABASE             (0x200)
    #define MUSB_PHYBASE             (0x9800)
    #define MUSB_PORT0_PHYBASE       (0)
	#define MUSB_PORT1_PHYBASE       (0x100)
	#define MUSB_PORT2_PHYBASE       (0x200)
	#define MUSB_PORT3_PHYBASE       (0x300)	
#else
#define MUSB_BASE                       (IO_VIRT + 0x29000)
#define MUSB_BASE1                      (IO_VIRT + 0x2e000)
#define MUSB_COREBASE                   (0x800)
#define MUSB_DMABASE                    (0xA00)
#define MUSB_MISCBASE                   (0x600)
#define MUSB_CBUFBASE                   (0x500)
#define MUSB_PHYBASE                    (0x400)
#endif

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

/*
Begin of MTK created register definition.
*/
//  MTK Notice: Max Liao, 2006/08/29.
//  added for non-32 bits aligned fifo read/write. Base addr = USB_BASE = 0x20029600.
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define M_REG_FIFOBYTECNT    0x690
#else
#define M_REG_FIFOBYTECNT    0xEC
#endif
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
#define M_REG_LINESTATE_MASK    0x00030000

#define LINESTATE_DISCONNECT    0x00000000
#define LINESTATE_FS_SPEED      0x00010000
#define LINESTATE_LS_SPEED      0x00020000
#define LINESTATE_HWERROR       0x00030000

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
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define M_REG_RXDATATOG		            0x80
#define M_REG_TXDATATOG		            0x84
#define M_REG_SET_DATATOG(ep, v)	    (((1 << ep) << 16) | (v << ep))
#else
#define M_REG_RXDATATOG             0x10
#define M_REG_TXDATATOG             0x14
#define M_REG_SET_DATATOG(ep, v)    (((1 << ep) << 16) | (v << ep))
#endif
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

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define M_REG_PERFORMANCE1 0x70
#define M_REG_PERFORMANCE2 0x72
#define M_REG_PERFORMANCE3 0x74
#endif


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
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define MGC_M_TXCSR_MODE          0x0000
#else
#define MGC_M_TXCSR_MODE          0x2000
#endif
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


#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)

//level 1 interrupt
#define M_REG_INTRLEVEL1 0xA0
#define M_REG_INTRLEVEL1EN 0xA4
#endif



/*
* DRC-specific definitions
* $Revision: #1 $
*/
#ifndef MUSB_C_NUM_EPS
    /* 
        5363/5392 :
            6+1
            
        5365/5395 :
            P0 : 5+1
            P1 : 3+1
        5368/5396
        	P0,P1: 5+1
        	P2,P3: 8+1
    */
    #if defined(CONFIG_ARCH_MT5395) || defined(CONFIG_ARCH_MT5365)
        #define MUSB_C_NUM_EPS   (6)
        #define MUSB_C_NUM_EPS1  (4)
    #elif defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
		#define MUSB_C_NUM_EPS	(9)
		#define MUSB_C_NUM_EPS0  (6)
		#define MUSB_C_NUM_EPS1  (6)
		#define MUSB_C_NUM_EPS2  (9)
		#define MUSB_C_NUM_EPS3  (9)
	#else
        #define MUSB_C_NUM_EPS   (7)
    #endif
#endif

#define MGC_END0_FIFOSIZE    64 /* this is non-configurable */

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

/* 
*  DRC register access macros
*/

/* Get offset for a given FIFO */
#define MGC_FIFO_OFFSET(_bEnd) (MGC_M_FIFO_EP0 + (_bEnd * 4))

#define MGC_END_OFFSET(_bEnd, _bOffset) (0x100 + (0x10*_bEnd) + _bOffset)

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define MGC_BUSCTL_OFFSET(_bEnd, _bOffset)	(0x480 + (8*_bEnd) + _bOffset)
#else
#define MGC_BUSCTL_OFFSET(_bEnd, _bOffset)  (0x80 + (8*_bEnd) + _bOffset)
#endif

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define MGC_FIFO_CNT    MGC_Write32
#else
#define MGC_FIFO_CNT    MGC_MISC_Write32
#endif

/* indexed vs. flat register model */
#define MGC_SelectEnd(_pBase, _bEnd)                    \
MGC_Write8(_pBase, MGC_O_HDRC_INDEX, _bEnd)

#define MGC_ReadCsr8(_pBase, _bOffset, _bEnd)           \
MGC_Read8(_pBase, (_bOffset + 0x10))

#define MGC_ReadCsr16(_pBase, _bOffset, _bEnd)          \
MGC_Read16(_pBase, (_bOffset + 0x10))

#define MGC_WriteCsr8(_pBase, _bOffset, _bEnd, _bData)  \
MGC_Write8(_pBase, (_bOffset + 0x10), _bData)

#define MGC_WriteCsr16(_pBase, _bOffset, _bEnd, _bData) \
MGC_Write16(_pBase, (_bOffset + 0x10), _bData)

#define MGC_VBUS_MASK            0x18                   /* DevCtl D4 - D3 */

#define MGC_END0_START  0x0
#define MGC_END0_OUT    0x2
#define MGC_END0_IN     0x4
#define MGC_END0_STATUS 0x8

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


#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define MGC_PHY_Read32(_pBase, r)      \
(\
	((uint32_t)_pBase == (MUSB_BASE))?\
	*((volatile uint32_t *)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT0_PHYBASE) + (r))):\
	(((uint32_t)_pBase == (MUSB_BASE1))?\
	*((volatile uint32_t *)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT1_PHYBASE) + (r))):\
	(((uint32_t)_pBase == (MUSB_BASE2))?\
	*((volatile uint32_t *)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT2_PHYBASE) + (r))):\
	*((volatile uint32_t *)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT3_PHYBASE) + (r))))\
))

#define MGC_PHY_Write32(_pBase, r, v)\
{ \
	if((uint32_t)_pBase == (MUSB_BASE)){ \
	*((volatile uint32_t *)((MUSB_BASE + MUSB_PHYBASE)+(MUSB_PORT0_PHYBASE) + (r))) = v;\
	}\
	else if((uint32_t)_pBase == (MUSB_BASE1)){\
	*((volatile uint32_t *)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT1_PHYBASE) + (r))) = v;\
	}\
	else if((uint32_t)_pBase == (MUSB_BASE2)){\
	*((volatile uint32_t *)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT2_PHYBASE) + (r))) = v;\
	}\
	else if((uint32_t)_pBase == (MUSB_BASE3)){\
	*((volatile uint32_t *)((MUSB_BASE + MUSB_PHYBASE)+ (MUSB_PORT3_PHYBASE) + (r))) = v;\
	}\
}
#define MGC_DMA_Read32(_pBase, r)      \
    *((volatile uint32_t *)(((uint32_t)_pBase) + (MUSB_DMABASE)+ (r)))

#define MGC_DMA_Write32(_pBase, r, v)  \
    (*((volatile uint32_t *)(((uint32_t)_pBase) + (MUSB_DMABASE)+ (r))) = v)
    
#define MGC_BIM_READ32(_offset)         *((volatile uint32_t *)(MBIM_VIRT + _offset))
	
#define MGC_BIM_WRITE32(_offset, value)  (*((volatile uint32_t *)((MBIM_VIRT)+ (_offset))) = value)

#else
#define MGC_PHY_Read32(_pBase, r)      \
    *((volatile uint32_t *)(((uint32_t)_pBase) + (MUSB_PHYBASE)+ (r)))

#define MGC_PHY_Write32(_pBase, r, v)  \
    (*((volatile uint32_t *)((((uint32_t)_pBase) + MUSB_PHYBASE)+ (r))) = v)

#define MGC_MISC_Read32(_pBase, r)     \
    *((volatile uint32_t *)(((uint32_t)_pBase) + (MUSB_MISCBASE)+ (r)))

#define MGC_MISC_Write32(_pBase, r, v) \
    (*((volatile uint32_t *)(((uint32_t)_pBase) + (MUSB_MISCBASE)+ (r))) = v)

#define MGC_DMA_Read32(_pBase, r)      \
    *((volatile uint32_t *)(((uint32_t)_pBase) + (MUSB_DMABASE)+ (r)))

#define MGC_DMA_Write32(_pBase, r, v)  \
    (*((volatile uint32_t *)(((uint32_t)_pBase) + (MUSB_DMABASE)+ (r))) = v)

#define MGC_BIM_READ32(_offset)         *((volatile uint32_t *)(MBIM_VIRT + _offset))

#define MGC_BIM_WRITE32(_offset, value)  (*((volatile uint32_t *)((MBIM_VIRT)+ (_offset))) = value)


#endif
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
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
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
#else
#define MGC_Write8(_pBase, _offset, _data)                                                   \
{                                                                                            \
volatile uint32_t u4TmpVar;                                                                  \
u4TmpVar = *((volatile uint32_t*)(((uint32_t)_pBase) + MUSB_COREBASE + ((_offset) & 0xFC))); \
u4TmpVar &= ~(((uint32_t)0xFF) << (8*((_offset) & 0x03)));                                   \
u4TmpVar |= (uint32_t)(((_data) & 0xFF) << (8*((_offset) & 0x03)));                          \
*((volatile uint32_t*)(((uint32_t)_pBase) + MUSB_COREBASE + ((_offset) & 0xFC))) = u4TmpVar; \
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
u4TmpVar = *((volatile uint32_t*)(((uint32_t)_pBase) + MUSB_COREBASE + ((_offset) & 0xFC))); \
u4TmpVar &= ~(((uint32_t)0xFFFF) << (8*((_offset) & 0x03)));                                 \
u4TmpVar |= (_data) << (8*((_offset) & 0x03));                                               \
*((volatile uint32_t*)(((uint32_t)_pBase) + MUSB_COREBASE + ((_offset) & 0xFC))) = u4TmpVar; \
}

#endif
/**
* Write a 32-bit core register
* @param _pBase core base address in memory
* @param _offset offset into the core's register space
* @param _data 32-bit datum
*/
#define MGC_Write32(_pBase, _offset, _data) \
(*((volatile uint32_t *)(((uint32_t)_pBase) + MUSB_COREBASE + _offset)) = _data)


#define SPIN_LOCK_IRQSAVE(l, f, irq) \
    u4UsbIrqEnable = MGC_BIM_READ32(REG_IRQEN) & (1 << irq); \
    if (u4UsbIrqEnable) \
    {\
    uint32_t u4TmpVar; \
    spin_lock_irqsave(l, f);\
    u4TmpVar = MGC_BIM_READ32(REG_IRQEN);\
    u4TmpVar &= ~(1 << irq);\
    MGC_BIM_WRITE32(REG_IRQEN, u4TmpVar);\
    spin_unlock_irqrestore(l, f);\
    }
    
#define SPIN_UNLOCK_IRQRESTORE(l,f, irq) \
    if (u4UsbIrqEnable) \
    {\
    uint32_t u4TmpVar; \
    spin_lock_irqsave(l, f);\
    u4TmpVar = MGC_BIM_READ32(REG_IRQEN);\
    u4TmpVar |= (1 << irq);\
    MGC_BIM_WRITE32(REG_IRQEN, u4TmpVar);\
    spin_unlock_irqrestore(l, f);\
    }

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
                                                                                
#define USB_ZERO_PACKET             0x0040          /* Finish bulk OUTs always with zero length packet */

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define MGC_HSDMA_CHANNELS  4
#else
#define MGC_HSDMA_CHANNELS  2
#endif

#define MGC_O_HSDMA_BASE    0x200
#define MGC_O_HSDMA_INTR    0x200
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
#define MGC_O_HSDMA_INTR_MASK    0x201   
#define MGC_O_HSDMA_INTR_CLEAR   0x202
#define MGC_O_HSDMA_INTR_SET     0x203
#endif

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

/***************************** TYPES ******************************/

/**
 * MGC_DmaChannel.
 * A DMA channel.
 * @field pPrivateData channel-private data; not to be interpreted by the ICD
 * @field wMaxLength the maximum number of bytes the channel can move
 * in one transaction (typically representing many USB maximum-sized packets)
 * @field dwActualLength how many bytes have been transferred
 * @field bStatus current channel status (updated e.g. on interrupt)
 * @field bDesiredMode TRUE if mode 1 is desired; FALSE if mode 0 is desired
 */
typedef struct
{
    void* pPrivateData;  /*MGC_HsDmaChannel* pImplChannel*/
    uint32_t dwMaxLength;
    uint32_t dwActualLength;
    MGC_DmaChannelStatus bStatus;
    uint8_t bDesiredMode;
} MGC_DmaChannel;

/**
 * Start a DMA controller.
 * @param pPrivateData private data pointer from MGC_DmaController
 * @return TRUE on success
 * @return FALSE on failure (e.g. no DMAC appears present)
 */
typedef uint8_t (*MGC_pfDmaStartController)(void* pPrivateData);

/**
 * Stop a DMA controller.
 * @param pPrivateData the controller's private data pointer
 * @return TRUE on success
 * @return FALSE on failure; the ICD may try again
 */
typedef uint8_t (*MGC_pfDmaStopController)(void* pPrivateData);

/**
 * Allocate a DMA channel.
 * Allocate a DMA channel suitable for the given conditions.
 * @param pPrivateData the controller's private data pointer
 * @param bLocalEnd the local endpoint index (1-15)
 * @param bTransmit TRUE for transmit; FALSE for receive
 * @param bProtocol the USB protocol, as per USB 2.0 chapter 9
 * (0 => control, 1 => isochronous, 2 => bulk, 3 => interrupt)
 * @param wMaxPacketSize maximum packet size
 * @return a non-NULL pointer on success
 * @return NULL on failure (no channel available)
 */
typedef MGC_DmaChannel* (*MGC_pfDmaAllocateChannel)(
    void* pPrivateData, uint8_t bLocalEnd, 
    uint8_t bTransmit, uint8_t bProtocol, uint16_t wMaxPacketSize);

/**
 * Release a DMA channel.
 * Release a previously-allocated DMA channel.
 * The ICD guarantess to no longer reference this channel.
 * @param pChannel pointer to a channel obtained by 
 * a successful call to pController->pfDmaAllocateChannel
 */
typedef void (*MGC_pfDmaReleaseChannel)(MGC_DmaChannel* pChannel);

/**
 * Allocate DMA buffer.
 * Allocate a buffer suitable for DMA operations with the given channel.
 * @param pChannel pointer to a channel obtained by 
 * a successful call to pController->pfDmaAllocateChannel
 * @param dwLength length, in bytes, desired for the buffer
 * @return a non-NULL pointer to a suitable region (in processor space)
 * on success
 * @return NULL on failure
 */
typedef uint8_t* (*MGC_pfDmaAllocateBuffer)(MGC_DmaChannel* pChannel,
                         uint32_t dwLength);

/**
 * Release DMA buffer.
 * Release a DMA buffer previously acquiring by a successful call
 * to pController->pfDmaAllocateBuffer.
 * @param pChannel pointer to a channel obtained by 
 * a successful call to pController->pfDmaAllocateChannel
 * @param pBuffer the buffer pointer
 * @return TRUE on success
 * @return FALSE on failure (e.g. the controller owns the buffer at present)
 */
typedef uint8_t (*MGC_pfDmaReleaseBuffer)(MGC_DmaChannel* pChannel,
                       uint8_t* pBuffer);

/**
 * Program a DMA channel.
 * Program a DMA channel to move data at the core's request.
 * The local core endpoint and direction should already be known,
 * since they are specified in the pfDmaAllocateChannel call.
 * @param pChannel pointer to a channel obtained by 
 * a successful call to pController->pfDmaAllocateChannel
 * @param wPacketSize the packet size
 * @param bMode TRUE if mode 1; FALSE if mode 0
 * @param pBuffer base address of data (in processor space)
 * @param dwLength the number of bytes to transfer;
 * guaranteed by the ICD to be no larger than the channel's reported dwMaxLength
 * @return TRUE on success
 * @return FALSE on error
 */
typedef uint8_t (*MGC_pfDmaProgramChannel)(MGC_DmaChannel* pChannel, 
                        uint16_t wPacketSize, uint8_t bMode,
                        const uint8_t* pBuffer, 
                        uint32_t dwLength);

/**
 * Get DMA channel status.
 * Get the current status of a DMA channel, if the hardware allows.
 * @param pChannel pointer to a channel obtained by 
 * a successful call to pController->DmaAllocateChannel
 * @return current status 
 * (MGC_DMA_STATUS_UNKNOWN if hardware does not have readable status)
 */
typedef MGC_DmaChannelStatus (*MGC_pfDmaGetChannelStatus)(
    MGC_DmaChannel* pChannel);

/**
 * DMA ISR.
 * If present, this function is called by the ICD on every interrupt.
 * This is necessary because with the built-in DMA controller
 * (and probably some other configurations),
 * the DMA interrupt is shared with other core interrupts.
 * Therefore, this function should return quickly 
 * when there is no DMA interrupt.
 * When there is a DMA interrupt, this function should
 * perform any implementations-specific operations,
 * and update the status of all appropriate channels.
 * If the DMA controller has its own dedicated interrupt,
 * this function should do nothing.
 * This function is called BEFORE the ICD handles other interrupts.
 * @param pPrivateData the controller's private data pointer
 * @return TRUE if an interrupt was serviced
 * @return FALSE if no interrupt required servicing
 */
typedef uint8_t (*MGC_pfDmaControllerIsr)(void* pPrivateData);

/**
 * MGC_DmaController.
 * A DMA Controller.
 * This is in a struct to allow the ICD to support
 * multiple cores of different types, 
 * since each may use a different type of DMA controller.
 * @field pPrivateData controller-private data;
 * not to be interpreted by the ICD
 * @field pfDmaStartController ICD calls this to start a DMA controller
 * @field pfDmaStopController ICD calls this to stop a DMA controller
 * @field pfDmaAllocateChannel ICD calls this to allocate a DMA channel
 * @field pfDmaReleaseChannel ICD calls this to release a DMA channel
 * @field pfDmaAllocateBuffer ICD calls this to allocate a DMA buffer
 * @field pfDmaReleaseBuffer ICD calls this to release a DMA buffer
 * @field pfDmaGetChannelStatus ICD calls this to get a DMA channel's status
 * @field pfDmaControllerIsr ICD calls this (if non-NULL) from its ISR
 */
typedef struct
{
    void* pPrivateData;
    MGC_pfDmaAllocateChannel pfDmaAllocateChannel;
    MGC_pfDmaReleaseChannel pfDmaReleaseChannel;
    MGC_pfDmaProgramChannel pfDmaProgramChannel;
    MGC_pfDmaGetChannelStatus pfDmaGetChannelStatus;
    MGC_pfDmaControllerIsr pfDmaControllerIsr;
} MGC_DmaController;

/**
 * A DMA channel has new status.
 * This may be used to notify the ICD of channel status changes asynchronously.
 * This is useful if the DMA interrupt is different from the USB controller's
 * interrupt, so on some systems there may be no control over the order of
 * USB controller and DMA controller assertion.
 * @param pPrivateData the controller's private data pointer
 * @param bLocalEnd the local endpoint index (1-15)
 * @param bTransmit TRUE for transmit; FALSE for receive
 * @return TRUE if an IRP was completed as a result of this call;
 * FALSE otherwise
 */
typedef uint8_t (*MGC_pfDmaChannelStatusChanged)(
    void* pPrivateData, uint8_t bLocalEnd, 
    uint8_t bTransmit);

/**
 * Instantiate a DMA controller.
 * Instantiate a software object representing a DMA controller.
 * @param pfDmaChannelStatusChanged channel status change notification function.
 * Normally, the ICD requests status in its interrupt handler.
 * For some DMA controllers, this may not be the correct time.
 * @param pDmaPrivate parameter for pfDmaChannelStatusChanged
 * @param pCoreBase the base address (in kernel space) of the core
 * It is assumed the DMA controller's registers' base address will be related
 * to this in some way.
 * @return non-NULL pointer on success
 * @return NULL on failure (out of memory or exhausted 
 * a fixed number of controllers)
 */
typedef MGC_DmaController* (*MGC_pfNewDmaController)(
    MGC_pfDmaChannelStatusChanged pfDmaChannelStatusChanged,
    void* pDmaPrivate,
    uint8_t* pCoreBase);

/**
 * Destroy DMA controller.
 * Destroy a previously-instantiated DMA controller.
 */
typedef void (*MGC_pfDestroyDmaController)(
    MGC_DmaController* pController);

/**
 * MGC_DmaControllerFactory.
 * A DMA controller factory.
 * To allow for multi-core implementations and different
 * types of cores and DMA controllers to co-exist,
 * it is necessary to create them from factories.
 * @field wCoreRegistersExtent the total size of the core's
 * register region with the DMA controller present,
 * for use in mapping the core into system memory.
 * For example, the MHDRC core takes 0x200 bytes of address space.
 * If your DMA controller starts at 0x200 and takes 0x100 bytes,
 * set this to 0x300.
 * @field pfNewDmaController create a DMA controller
 * @field pfDestroyDmaController destroy a DMA controller
 */
typedef struct
{
    uint16_t wCoreRegistersExtent;
    MGC_pfNewDmaController pfNewDmaController;
    MGC_pfDestroyDmaController pfDestroyDmaController;
} MGC_DmaControllerFactory;

struct _MGC_HsDmaController;

typedef struct
{
    MGC_DmaChannel Channel;
    struct _MGC_HsDmaController* pController;
    uint32_t dwStartAddress;
    uint32_t dwCount;
    uint16_t wMaxPacketSize;
    uint8_t bIndex;
    uint8_t bEnd;
    uint8_t bProtocol;
    uint8_t bTransmit;
} MGC_HsDmaChannel;

typedef struct _MGC_HsDmaController
{
    MGC_DmaController Controller;
    MGC_HsDmaChannel aChannel[MGC_HSDMA_CHANNELS];
    MGC_pfDmaChannelStatusChanged pfDmaChannelStatusChanged;
    void* pDmaPrivate;
    uint8_t* pCoreBase;
    uint8_t bChannelCount;
    uint8_t bmUsedChannels;
} MGC_HsDmaController;

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
    uint16_t wType;
    void * pBase;
    uint32_t dwIrq;
    uint8_t bSupport;
	#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368)
	uint8_t bEndpoint_num;
	uint8_t bHub_support;
	#endif
    MGC_HsDmaController rDma;
} MUSB_LinuxController;

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
    spinlock_t Lock;
    struct usb_device *dev;

    struct urb *pCurrentUrb;
    struct list_head list;
    gfp_t mem_flags;

    uint32_t dwOffset;      /* offset int the current request */
    uint32_t dwRequestSize; /* request size */
    uint32_t dwIsoPacket;
    uint32_t dwWaitFrame;

    MGC_DmaChannel * pDmaChannel;

    uint16_t wMaxPacketSize; /* h/w prepare max packet size. */
    uint16_t wPacketSize;        /* usb maxpacket */

    uint8_t bIsSharedFifo;
    uint8_t bRemoteAddress;
    uint8_t bRemoteEnd;
    uint8_t bTrafficType;
    uint8_t bRetries;
    uint8_t bBusyCompleting;    /* TRUE on Tx when the current urb is completing */
} MGC_LinuxLocalEnd;

/**
* MGC_LinuxCd.
* Driver instance data.
* @field Lock spinlock
* @field Lock to protect endpoint index (MGC_SelectEnd)
* @field Timer interval timer for various things
* @field pBus pointer to Linux USBD bus
* @field RootHub virtual root hub
* @field PortServices services provided to virtual root hub
* @field pRootDevice root device pointer, to track connection speed
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
    spinlock_t Lock;
    spinlock_t SelectEndLock;
    struct timer_list Timer;
    struct usb_device * pRootDevice;
    MGC_DmaController* pDmaController;
    void * pRegs;
    uint32_t nIrq;    
    MGC_LinuxLocalEnd aLocalEnd[2][MUSB_C_NUM_EPS]; /*0: Rx, 1: Tx*/
    uint32_t dwVirtualHubPortStatus;
    uint16_t wEndMask;
    uint8_t bEndCount;
    uint8_t bRootSpeed;
    uint8_t bInsert;
    uint8_t bCheckInsert; /* retries of checking insert event */
    uint8_t bEnd0Stage;   /* end0 stage while in host or device mode */
} MGC_LinuxCd;

#endif                    /* #ifndef MTK_HCD */
