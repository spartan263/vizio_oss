/*-----------------------------------------------------------------------------
 *\drivers\usb\mtk_usb.c
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


/** @file mtk_usb.c
 *  This C file implements the mtk usb host controller driver functions.  
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include <common.h>
#ifdef CONFIG_USB_MTKHCD

#include <usb.h>
#include "x_typedef.h"

#include "mgc_hdrc.h"
#include "mgc_dma.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

//#define MTK_DEBUG

#ifdef MTK_DEBUG
	#define LOG(level, fmt, args...) \
		printf("[%s:%d] " fmt, __PRETTY_FUNCTION__, __LINE__ , ## args)
#else
	#define LOG(level, fmt, args...)
#endif

static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
		"subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0" (loops));
}


#define mdelay(n) ({unsigned long msec=(n); while (msec--) delay(1000000);})

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
typedef struct 
{
    UINT32 *pBase;
    UINT32 *pPhyBase;
    UINT32 u4Insert;
    UINT32 u4HostSpeed;
    UINT32 u4DeviceSpeed;
}MTKUSBPORT;
//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------
/*
 *	PIPE attributes
 */
#define MENTOR_PIPE_CONTROL                      (0)
#define MENTOR_PIPE_ISOCHRONOUS             (1)
#define MENTOR_PIPE_BULK                              (2)
#define MENTOR_PIPE_INTERRUPT                   (3)

/* USB directions */
#define USB_OUT           (TRUE)
#define USB_IN              (FALSE)

/* DMA control */
#define DMA_ON           (TRUE)
#define DMA_OFF           (FALSE)

/* SPEED control */
#define HS_ON           (TRUE)
#define HS_OFF           (FALSE)

/** Low-speed USB */
#define MUSB_CONNECTION_SPEED_LOW (1)
/** Full-speed USB */
#define MUSB_CONNECTION_SPEED_FULL (2)
/** High-speed USB */
#define MUSB_CONNECTION_SPEED_HIGH (3)

#define USB_MAX_EP_NUM                   (5)

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
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
static MTKUSBPORT mtkusbport[] = 
{
    {  /* Port 0 information. */
        .pBase = (void*)(MUSB_BASE + MUSB_COREBASE), 
        .pPhyBase = (void*)(MUSB_BASE + MUSB_PHYBASE+ MUSB_PORT0_PHYBASE), 
        .u4HostSpeed = HS_ON, 
    },
    {  /* Port 1 information. */
        .pBase = (void*)(MUSB_BASE1 + MUSB_COREBASE), 
        .pPhyBase = (void*)(MUSB_BASE + MUSB_PHYBASE+ MUSB_PORT1_PHYBASE), 
        .u4HostSpeed = HS_ON, 
    },
    {  /* Port 2 information. */
        .pBase = (void*)(MUSB_BASE2 + MUSB_COREBASE), 
        .pPhyBase = (void*)(MUSB_BASE + MUSB_PHYBASE+ MUSB_PORT2_PHYBASE),             
        .u4HostSpeed = HS_ON, 
    },
    {  /* Port 3 information. */
        .pBase = (void*)(MUSB_BASE3 + MUSB_COREBASE), 
        .pPhyBase = (void*)(MUSB_BASE + MUSB_PHYBASE+ MUSB_PORT3_PHYBASE),             
        .u4HostSpeed = HS_ON, 
    }
};
#else
static MTKUSBPORT mtkusbport[] = 
{
    {  /* Port 0 information. */
        .pBase = (void*)(MUSB_BASE + MUSB_COREBASE), 
        .pPhyBase = (void*)(MUSB_BASE + MUSB_PHYBASE+ MUSB_PORT0_PHYBASE), 
        .u4HostSpeed = HS_ON, 
    },
    {  /* Port 1 information. */
        .pBase = (void*)(MUSB_BASE1 + MUSB_COREBASE), 
        .pPhyBase = (void*)(MUSB_BASE + MUSB_PHYBASE+ MUSB_PORT1_PHYBASE), 
        .u4HostSpeed = HS_ON, 
    }
};
#endif

static const UINT8 _aTestPacket [] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfc, 0x7e, 0xbf, 0xdf,
    0xef, 0xf7, 0xfb, 0xfd, 0x7e
};
//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** MUC_WaitEpIrq
 *  wait ep interrupt.
 *  @param  prDev: point to struct usb_device. 
 *  @param  u4EpNum          selected ep number
 *  @param  fgTX                  tx = 1, rx = 0.
 *  @param  fgDMA               check dma.
 *  @retval 0	Success
 *  @retval Others	Error
 */
//-------------------------------------------------------------------------
static INT32 MUC_WaitEpIrq(struct usb_device *prDev, UINT32 u4EpNum, BOOL fgTX, BOOL fgDMA)
{
    volatile UINT32 u4Reg = 0; /* read USB registers into this */
    volatile UINT32 u4Intreg[2] = { 0, 0 };
    volatile UINT16 u2Reg;
    UINT32 u4Port = (UINT32)prDev->portnum;

    //while (u4TimeOut < 0x100000)
    while(1)
    {           
        /*
         *  Start by seeing if INTRUSB active with anything
         */
        u4Reg = MOTG_REG_READ8(M_REG_INTRUSB);
        u4Reg &= (M_INTR_VBUSERROR | M_INTR_DISCONNECT | M_INTR_CONNECT | M_INTR_BABBLE);
        if (u4Reg)
        {
            // must reset this device, no more action.
            mtkusbport[u4Port].u4Insert = 0;
            MOTG_REG_WRITE8(M_REG_INTRUSB, u4Reg);
            prDev->status = USB_ST_BIT_ERR;
            printf("INTRUSB=%X.\n", (unsigned int)u4Reg);
            return -1;
        }

        /*
         *  EP0 active if INTRTX1, bit 0=1.  TX or RX on EP0 causes this bit to
         *  be set.
         */
        u4Intreg[0] = MOTG_REG_READ16(M_REG_INTRTX);
        if (u4Intreg[0])
        {
            MOTG_REG_WRITE16(M_REG_INTRTX, u4Intreg[0]);
            // EP0 case.
            if (u4EpNum == 0)
            {
                u2Reg = MOTG_REG_READ16(M_REG_CSR0);
                if (u2Reg & M_CSR0_H_RXSTALL)
                {                
                    // must reset this device, no more action.
                    prDev->status = USB_ST_BIT_ERR;
                    return -1;
                }

                if ((u2Reg & M_CSR0_H_ERROR) ||
                     (u2Reg & M_CSR0_H_NAKTIMEOUT))
                {                
                    // must reset this device, no more action.
                    mtkusbport[u4Port].u4Insert = 0;                            
                    prDev->status = USB_ST_BIT_ERR;
                    return -1;
                }
            
                //_MAX_u4TimeOut = (_MAX_u4TimeOut < u4TimeOut) ? u4TimeOut: _MAX_u4TimeOut;    
                if (u4Intreg[0] & 1)
                {
                    return 0;
                }
                else
                {
                    // must reset this device, no more action.
                    mtkusbport[u4Port].u4Insert = 0;                                            
                    prDev->status = USB_ST_BIT_ERR;            
                    return -1;
                }                                
            }
            else
            {
                u2Reg = MOTG_REG_READ16(M_REG_TXCSR);
                if ((u2Reg & M_TXCSR_H_RXSTALL) ||
                     (u2Reg & M_TXCSR_H_ERROR) ||
                     (u2Reg & M_TXCSR_H_NAKTIMEOUT))
                {                
                    // must reset this device, no more action.
                    mtkusbport[u4Port].u4Insert = 0;                            
                    prDev->status = USB_ST_BIT_ERR;
                    return -1;
                }
                
                if ((fgTX) && (u4Intreg[0] == (1<<u4EpNum)))
                {
                    return 0;
                }
                else
                {
                    // must reset this device, no more action.
                    mtkusbport[u4Port].u4Insert = 0;                                                            
                    prDev->status = USB_ST_BIT_ERR;            
                    return -1;
                }                
            }
        }

        u4Intreg[1] = MOTG_REG_READ16(M_REG_INTRRX);
        if (u4Intreg[1])
        {
            MOTG_REG_WRITE16(M_REG_INTRRX, u4Intreg[1]);

            u2Reg = MOTG_REG_READ16(M_REG_RXCSR);
            // device nak time out.
            if (u2Reg & M_RXCSR_DATAERROR)
            {                
                MOTG_REG_WRITE16(M_REG_RXCSR, 0);
                prDev->status = USB_ST_NAK_REC;
                return -1;
            }

            if ((u2Reg & M_RXCSR_H_RXSTALL) ||
                 (u2Reg & M_RXCSR_H_ERROR))
            {                
                // must reset this device, no more action.
                mtkusbport[u4Port].u4Insert = 0;                            
                prDev->status = USB_ST_BIT_ERR;
                return -1;
            }
            
            //_MAX_u4TimeOut = (_MAX_u4TimeOut < u4TimeOut) ? u4TimeOut: _MAX_u4TimeOut;            
            if ((!fgTX) && (u4Intreg[1] == (1<<u4EpNum)))
            {
                return 0;
            }
            else
            {
                // must reset this device, no more action.
                mtkusbport[u4Port].u4Insert = 0;                                        
                prDev->status = USB_ST_BIT_ERR;            
                return -1;
            }
        }
    }

    //prDev->status = USB_ST_BIT_ERR;
    //return -1;
}
//-------------------------------------------------------------------------
/** MUC_ReadFifo
 *  read data from selected ep fifo.
 *  @param  *pu1Buf           point to data buffer.
 *  @param  u4EpNum         selected ep number.
 *  @param  u4ReadCount           read data length. 
 *  @return  void
 */
//-------------------------------------------------------------------------
static void MUC_ReadFifo(UINT32 u4Port, UINT8 *pu1Buf, UINT32 u4EpNum, UINT32 u4ReadCount)
{
    UINT32 u4FifoAddr;
    UINT32 u4Val;
    UINT32 i;
    UINT32 u4Count;

    if (u4ReadCount == 0)
    {
        return;
    }

    u4FifoAddr = FIFO_ADDRESS(u4EpNum); /* blds absolute fifo addrs */

    LOG(7, "fifo read = %d.\n", u4ReadCount);
    // set FIFO byte count = 4 bytes.
    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
    MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 2);
    #else
    MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 4);
    #endif
    if ((u4ReadCount > 0) && ((UINT32)pu1Buf & 3))
    {
        u4Count = 4;
        /* byte access for unaligned */
        while (u4ReadCount > 0)
        {
	    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
	    if(3 == u4ReadCount || 2 == u4ReadCount)
	    {
	       MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 1);
	       u4Val = *((volatile UINT32 *)u4FifoAddr);
	       for (i = 0; i < 2; i++)
	       {
	           *pu1Buf++ = ((u4Val >> (i *8)) & 0xFF);
	       }				   
	       u4ReadCount -=2;
	    }
	    if(1 == u4ReadCount) 
	    {
	        MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 0);
	        u4Val = *((volatile UINT32 *)u4FifoAddr);
	        *pu1Buf++ = (u4Val  & 0xFF);
	        u4ReadCount -= 1;
	    }	
				
	    // set FIFO byte count = 4 bytes.
	    MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 2);
	    u4ReadCount = 0;
            #else
            if (u4ReadCount < 4)
            {
                // set FIFO byte count.
                MGC_FIFO_CNT(M_REG_FIFOBYTECNT, u4ReadCount);
                u4Count = u4ReadCount;
            }

            u4Val = *((volatile UINT32 *)u4FifoAddr);

            for (i = 0; i < u4Count; i++)
            {
                *pu1Buf++ = ((u4Val >> (i *8)) & 0xFF);
            }
            u4ReadCount -= u4Count;
	#endif
        }
    }
    else
    {
        /* word access if aligned */
        while ((u4ReadCount > 3) && !((UINT32)pu1Buf & 3))
        {
            *((volatile UINT32 *)pu1Buf) = *((volatile UINT32 *)u4FifoAddr);

            pu1Buf = pu1Buf + 4;
            u4ReadCount = u4ReadCount - 4;
        }
        if (u4ReadCount > 0)
        {
            #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
	    if(3 == u4ReadCount ||2 == u4ReadCount )
	    {
		MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 1);
	        u4Val = *((volatile UINT32 *)u4FifoAddr);
		for (i = 0; i < 2; i++)
		{
		    *pu1Buf++ = ((u4Val >> (i *8)) & 0xFF);
 		}
                    u4ReadCount -= 2;
	    }

	    if(1 == u4ReadCount)
	    {
	        MGC_FIFO_CNT(M_REG_FIFOBYTECNT,0);
	        u4Val = *((volatile UINT32 *)u4FifoAddr);
				   
		*pu1Buf++ = (u4Val & 0xFF);
		u4ReadCount -= 1;			   
	     }
					
	     // set FIFO byte count = 4 bytes.
	     MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 2);
	    #else
            // set FIFO byte count.
            MGC_FIFO_CNT(M_REG_FIFOBYTECNT, u4ReadCount);

            u4Val = *((volatile UINT32 *)u4FifoAddr);
            for (i = 0; i < u4ReadCount; i++)
            {
                *pu1Buf++ = ((u4Val >> (i *8)) & 0xFF);
            }
	    #endif
        }
    }

    // set FIFO byte count = 4 bytes.
    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
    MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 2);
    #else
    MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 4);
    #endif
}

//-------------------------------------------------------------------------
/** MUC_WriteFifo
 *  write data to selected ep fifo.
 *  @param  *pu1Buf           point to data buffer.
 *  @param  u4EpNum         selected ep number.
 *  @param  u4WriteCount    write data length. 
 *  @return  void
 */
//-------------------------------------------------------------------------
static void MUC_WriteFifo(UINT32 u4Port, UINT8 *pu1Buf, UINT32 u4EpNum, UINT32 u4WriteCount)
{
    UINT32 u4FifoAddr;
    UINT32 u4Buf;
    UINT32 u4BufSize = 4;

    u4FifoAddr = FIFO_ADDRESS(u4EpNum);

    LOG(7, "fifo write = %d.\n", u4WriteCount);
    #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
    #else
    // set FIFO byte count = 4 bytes.
    MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 4);
    #endif
    /* byte access for unaligned */
    if ((u4WriteCount > 0) && ((UINT32)pu1Buf & 3))
    {
        while (u4WriteCount)
        {
	   #if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
           if (3 == u4WriteCount || 2 == u4WriteCount)
	   {
	       u4BufSize = 2;     		
	       // set FIFO byte count.
	       MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 1);
	   }
	   else if(1 == u4WriteCount)
	   {
                u4BufSize = 1;	 		
	        // set FIFO byte count.
	        MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 0);			   
	    }

	    memcpy((void *)&u4Buf, (const void *)pu1Buf, u4BufSize);

	    *((volatile UINT32 *)u4FifoAddr) = u4Buf;
				
	    u4WriteCount -= u4BufSize;
	    pu1Buf += u4BufSize;
	    #else
            if (u4WriteCount < 4)
            {
                u4BufSize = u4WriteCount;

                // set FIFO byte count.
                MGC_FIFO_CNT(M_REG_FIFOBYTECNT, u4WriteCount);
            }

            memcpy((void *)&u4Buf, (const void *)pu1Buf, u4BufSize);

            *((volatile UINT32 *)u4FifoAddr) = u4Buf;

            u4WriteCount -= u4BufSize;
            pu1Buf += u4BufSize;
		#endif
        }
    }
    else /* word access if aligned */
    {
        while ((u4WriteCount > 3) && !((UINT32)pu1Buf & 3))
        {
            *((volatile UINT32 *)u4FifoAddr) = *((volatile UINT32 *)pu1Buf);

            pu1Buf = pu1Buf + 4;
            u4WriteCount = u4WriteCount - 4;
        }
		#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
			if (3 == u4WriteCount || 2 == u4WriteCount)
			{
				// set FIFO byte count.
				MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 1);
		
				*((volatile UINT32 *)u4FifoAddr) = *((volatile UINT32 *)pu1Buf);
				u4WriteCount -= 2;
				pu1Buf += 2;
			}
			
			if(1 == u4WriteCount)
			{
				MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 0);
				if((uint32_t)pu1Buf & 3)
				{					
					*((volatile UINT32 *)u4FifoAddr) = *((volatile UINT32 *)pu1Buf);		
				}
				else
				{
					*((volatile UINT32 *)u4FifoAddr) = *((volatile UINT32 *)pu1Buf);			
				}
			}
		#else
        if (u4WriteCount > 0)
        {
            // set FIFO byte count.
            MGC_FIFO_CNT(M_REG_FIFOBYTECNT, u4WriteCount);

            *((volatile UINT32 *)u4FifoAddr) = *((volatile UINT32 *)pu1Buf);
        }
		#endif
    }
    // set FIFO byte count = 4 bytes.
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
	MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 2);
#else
    MGC_FIFO_CNT(M_REG_FIFOBYTECNT, 4);
#endif
}

//-------------------------------------------------------------------------
/** MUC_DevInsert
 *  check device insert or not. If device insert, try to reset device.
 *  @param   void.
 *  @retval 0	device insert
 *  @retval Others	device not insert
 */
//-------------------------------------------------------------------------
static INT32 MUC_DevInsert(UINT32 u4Port)
{
    volatile UINT32 reg = 0; /* read USB registers into this */
    volatile UINT32 linestate;
	
    /*
     *  Start by seeing if INTRUSB active with anything
     */

     mdelay(30);
    #if defined(CONFIG_ARCH_MT5391)
    reg = MOTG_REG_READ8(M_REG_INTRUSB);
    reg &= MOTG_REG_READ8(M_REG_INTRUSBE);
   printf("CONFIG_ARCH_MT5391 !\n");
    if (reg)
    {
        MOTG_REG_WRITE8(M_REG_INTRUSB, reg);
    }    
    #elif defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
    reg = MOTG_REG_READ8(M_REG_INTRUSB);   
	
    if (reg & 0x10)
    {
        MOTG_REG_WRITE8(M_REG_INTRUSB, reg);
        reg = M_INTR_CONNECT;
    }  
    else
    {
        reg = M_INTR_DISCONNECT;
    }    
    #else
    reg = MOTG_PHY_REG_READ32(M_REG_PHYC5);
    reg &= M_REG_LINESTATE_MASK ;
    if ( reg != LINESTATE_DISCONNECT && reg != LINESTATE_HWERROR )
    {
        reg = M_INTR_CONNECT;
    }
    else
    {
        reg = M_INTR_DISCONNECT;
    }
    #endif
    
    if (reg & M_INTR_CONNECT) // connect event.            
    {
        mdelay(50);

        #if defined(CONFIG_ARCH_MT5391)
        linestate = MOTG_PHY_REG_READ32(M_REG_PHYC6);
        #elif defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
        linestate = MOTG_PHY_REG_READ32(M_REG_PHYC5);
        linestate = linestate >> 6;
        #else
        linestate = MOTG_PHY_REG_READ32(M_REG_PHYC5);
        #endif
        linestate &= M_REG_LINESTATE_MASK;

       if ((linestate != LINESTATE_DISCONNECT) && (linestate != LINESTATE_HWERROR))
        {            
            printf("\nUSB: Detect device !\n");

            // try to reset device.
            reg = MOTG_REG_READ8(M_REG_POWER);
            reg |= M_POWER_RESET;
            MOTG_REG_WRITE8(M_REG_POWER, reg);

            mdelay(30);

            // clear reset.
            reg = MOTG_REG_READ8(M_REG_POWER);
            reg &= ~M_POWER_RESET;
            MOTG_REG_WRITE8(M_REG_POWER, reg);


            // check device speed: LS, FS, HS.
            reg = MOTG_REG_READ8(M_REG_DEVCTL);

            if (reg & M_DEVCTL_LSDEV)
            {
                printf("USB: LS device.\n");
                mtkusbport[u4Port].u4DeviceSpeed = MUSB_CONNECTION_SPEED_LOW;
            }
            else
            {
                reg = MOTG_REG_READ8(M_REG_POWER);
				
                if (reg & M_POWER_HSMODE)
                {
                    printf("USB: HS device.\n");
                    mtkusbport[u4Port].u4DeviceSpeed = MUSB_CONNECTION_SPEED_HIGH;
                }
                else
                {
                    printf("USB: FS device.\n");
                    mtkusbport[u4Port].u4DeviceSpeed = MUSB_CONNECTION_SPEED_FULL;
                }
            }

            mtkusbport[u4Port].u4Insert= TRUE;
            
            return 0;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------
/** MUC_Initial
 *  host controller register reset and initial.
 *  @param  void 
 *  @retval 0	Success
 *  @retval Others	Error
 */
//-------------------------------------------------------------------------
#ifdef CONFIG_ARCH_MT5391
#ifdef MTK_MHDRC /* 539x */
static INT32 MUC_Initial(UINT32 u4Port)
{
    volatile UINT32 u4Reg;

    // set REL_SUSP, USBRST
    MOTG_PHY_REG_WRITE32(0x10, 0x01010000);
    MOTG_PHY_REG_WRITE32(0x10, 0x00010000);
    // set PLL_EN
    MOTG_PHY_REG_WRITE32(0x04, 0x48000406);

    MOTG_MISC_REG_WRITE32(0xE0, 0x1818);    

    // make sure DRAM clock is on. 
    u4Reg = MOTG_MISC_REG_READ32(M_REG_AUTOPOWER);
    MOTG_MISC_REG_WRITE32(M_REG_AUTOPOWER, (u4Reg | M_REG_AUTOPOWER_DRAMCLK));

    // soft reset h/w.
    MOTG_MISC_REG_WRITE32(M_REG_SOFTRESET, M_REG_SOFT_RESET_ACTIVE);

    // wait PHY clock to be valid before deactive reset.
    do
    {
        // check if this board have usb module.
        u4Reg = MOTG_PHY_REG_READ32(M_REG_PHYC6);
        if (u4Reg != 0x00001F0F)
        {
            printf("USB: init fail.\n");
            
            return -1;
        }
        u4Reg = MOTG_MISC_REG_READ32(M_REG_AUTOPOWER);
    } while (!(u4Reg & M_REG_AUTOPOWER_PHYCLK));

    MOTG_MISC_REG_WRITE32(M_REG_SOFTRESET, M_REG_SOFT_RESET_DEACTIVE);

    // set CLKMODE = USB_INTA2_CK
    MOTG_PHY_REG_WRITE32(0x04, 0x48010406);

    MOTG_PHY_REG_WRITE32(M_REG_PHYC6, 0x1F13);

    // open session.
    u4Reg = MOTG_REG_READ8(M_REG_DEVCTL); /* and refresh */
    u4Reg |= M_DEVCTL_SESSION;
    MOTG_REG_WRITE8(M_REG_DEVCTL, u4Reg);    

    // wait session valid.
    mdelay(10);

    MOTG_PHY_REG_WRITE32(M_REG_PHYC6, 0x0F03);


    // set high speed disconnect threshold = 655 mV.
    u4Reg = MOTG_PHY_REG_READ32(0x08);
    u4Reg &= ~0x07000000;
    MOTG_PHY_REG_WRITE32(0x08, u4Reg);

    // set high speed disconnect deglitch = 2.83 n.
    MOTG_PHY_REG_WRITE32(0x18, 0x00000800);

    MOTG_MISC_REG_WRITE32(0xE0, 0x1818);    

    /*  flush the recv fifo, reset the data tog to 0 
        clear AutoSet, ISO, Mode, DMAEnab, FrcDataTog for recv */
    MOTG_REG_WRITE16(M_REG_RXCSR, 
        (M_RXCSR_FLUSHFIFO | M_RXCSR_CLRDATATOG));

    /*  flush the xmit fifo, reset the data tog to 0 
        clear AutoSet, ISO, Mode, DMAEnab, FrcDataTog for xmit */
    MOTG_REG_WRITE16(M_REG_TXCSR, 
        (M_TXCSR_FLUSHFIFO | M_TXCSR_CLRDATATOG));

    /* reset all power modes */
    if (mtkusbport[u4Port].u4HostSpeed== HS_ON)
    {
        MOTG_REG_WRITE8(M_REG_POWER, 
            M_POWER_SOFTCONN | M_POWER_HSENAB | M_POWER_ENSUSPEND);
    }
    else
    {
        MOTG_REG_WRITE8(M_REG_POWER, 
            M_POWER_SOFTCONN | M_POWER_ENSUSPEND);
    }

    printf("USB: init ok.\n");
    
    return 0;
}

#else /* 538x */

static INT32 MUC_Initial(UINT32 u4Port)
{
    volatile UINT32 u4Reg;

    // set REL_SUSP, USBRST
    MOTG_PHY_REG_WRITE32(0x10, 0x11010000);
    MOTG_PHY_REG_WRITE32(0x10, 0x10010000);

    MOTG_PHY_REG_WRITE32(0x04, 0x48000408);

    MOTG_MISC_REG_WRITE32(0xE0, 0x18);    

    // make sure DRAM clock is on. 
    u4Reg = MOTG_MISC_REG_READ32(M_REG_AUTOPOWER);
    MOTG_MISC_REG_WRITE32(M_REG_AUTOPOWER, (u4Reg | M_REG_AUTOPOWER_DRAMCLK));

    // soft reset h/w.
    MOTG_MISC_REG_WRITE32(M_REG_SOFTRESET, M_REG_SOFT_RESET_ACTIVE);

    // wait PHY clock to be valid before deactive reset.
    do
    {
        // check if this board have usb module.
        u4Reg = MOTG_PHY_REG_READ32(M_REG_PHYC6);
        if (u4Reg != 0x00001F0F)
        {
            printf("USB: pBase = 0x%08X init fail.\n", (UINT32)USB_ADDR);
            
            return -1;
        }
        u4Reg = MOTG_MISC_REG_READ32(M_REG_AUTOPOWER);
    } while (!(u4Reg & M_REG_AUTOPOWER_PHYCLK));
        
    MOTG_MISC_REG_WRITE32(M_REG_SOFTRESET, M_REG_SOFT_RESET_DEACTIVE);

    MOTG_PHY_REG_WRITE32(M_REG_PHYC6, 0x1F13);

    // set PHY register setting before softreset.
    u4Reg = MOTG_PHY_REG_READ32(0x10);
    u4Reg |= 0x00010000;
    MOTG_PHY_REG_WRITE32(0x10, u4Reg);

    MOTG_PHY_REG_WRITE32(0x00, 0x08201800);

    MOTG_PHY_REG_WRITE32(0x04, 0x48000408);

    MOTG_PHY_REG_WRITE32(0x10, 0x11010000);
    MOTG_PHY_REG_WRITE32(0x10, 0x10010000);
#if 0
    u4Reg = MOTG_PHY_REG_READ32(0x04);
    u4Reg &= ~0x1F000000;
    /* set high speed termination impedance control code = 420 mV */
    u4Reg |= 0x06000000;
    MOTG_PHY_REG_WRITE32(0x04, u4Reg);
#endif
    u4Reg = MOTG_PHY_REG_READ32(0x08);
    /* set high speed disconnect threshold = 680 mV */
    u4Reg &= ~0x07000000;
    /* set high speed Tx bias current = 840 mV/(Rext +300) */
    u4Reg &= ~0x70000000;
    /* set high speed SQ threshold high selection = 165 mV */
    u4Reg &= ~0x00007000;    
    MOTG_PHY_REG_WRITE32(0x08, u4Reg);

    MOTG_MISC_REG_WRITE32(0xE0, 0x18);    

    /*  flush the recv fifo, reset the data tog to 0 
        clear AutoSet, ISO, Mode, DMAEnab, FrcDataTog for recv */
    MOTG_REG_WRITE16(M_REG_RXCSR, 
        (M_RXCSR_FLUSHFIFO | M_RXCSR_CLRDATATOG));

    /*  flush the xmit fifo, reset the data tog to 0 
        clear AutoSet, ISO, Mode, DMAEnab, FrcDataTog for xmit */
    MOTG_REG_WRITE16(M_REG_TXCSR, 
        (M_TXCSR_FLUSHFIFO | M_TXCSR_CLRDATATOG));

    /* reset all power modes */
    if (mtkusbport[u4Port].u4HostSpeed == HS_ON)
    {
        MOTG_REG_WRITE8(M_REG_POWER, 
            M_POWER_SOFTCONN | M_POWER_HSENAB | M_POWER_ENSUSPEND);
    }
    else
    {
        MOTG_REG_WRITE8(M_REG_POWER, 
            M_POWER_SOFTCONN | M_POWER_ENSUSPEND);
    }

    // open session.
    u4Reg = MOTG_REG_READ8(M_REG_DEVCTL); /* and refresh */
    u4Reg |= M_DEVCTL_SESSION;
    MOTG_REG_WRITE8(M_REG_DEVCTL, u4Reg);    

    // wait session valid.
    mdelay(10);

    u4Reg = MOTG_PHY_REG_READ32(M_REG_PHYC6);
    u4Reg &= 0xFFFF0000;
    u4Reg |= 0x0F03;
    MOTG_PHY_REG_WRITE32(M_REG_PHYC6, u4Reg);

    printf("USB: pBase = 0x%08X init ok.\n", (UINT32)USB_ADDR);
    
    return 0;    
}

#endif /* 539x */
#elif defined(CONFIG_ARCH_MT5363)
static INT32 MUC_Initial(UINT32 u4Port)
{
    uint32_t u4Reg;
    uint8_t nDevCtl;
    uint32_t u4Tmp = USB_ADDR;

    // make sure DRAM clock is on. 
    u4Reg = MOTG_MISC_REG_READ32(M_REG_AUTOPOWER);
    MOTG_MISC_REG_WRITE32(M_REG_AUTOPOWER, (u4Reg | M_REG_AUTOPOWER_DRAMCLK));

    // reset all h/w register.
    MOTG_MISC_REG_WRITE32(M_REG_SOFTRESET, M_REG_SOFT_RESET_ACTIVE);

    mdelay(1);
    
    MOTG_MISC_REG_WRITE32(M_REG_SOFTRESET, M_REG_SOFT_RESET_DEACTIVE);

    u4Reg = MOTG_PHY_REG_READ32(0x0C);
    u4Reg &= (uint32_t)(~0x000020FF);    
    u4Reg |= (uint32_t)0x00000026;
    MOTG_PHY_REG_WRITE32(0x0C, u4Reg);

    // set REL_SUSP, USBRST
    u4Reg = MOTG_PHY_REG_READ32(0x00);
    u4Reg &= (uint32_t)(~0x00030000);    
    u4Reg |= (uint32_t)0x00030000;
    MOTG_PHY_REG_WRITE32(0x00, u4Reg);

    mdelay(10);

    u4Reg = MOTG_PHY_REG_READ32(0x00);
    u4Reg &= (uint32_t)(~0x00030000);    
    u4Reg |= (uint32_t)0x00010000;
    MOTG_PHY_REG_WRITE32(0x00, u4Reg);

    mdelay(10);

    // Set RG_HSDISC_DEGL.
    // Reset RG_EARLY_HSTX_I
    // Set RG_PLL_STABLE    
    u4Reg = MOTG_PHY_REG_READ32(0x04);
    u4Reg &= (uint32_t)(~0x00004000);    
    u4Reg |= (uint32_t)0x01000002;
    MOTG_PHY_REG_WRITE32(0x04, u4Reg);

    // deglich 2n.
    // enable serializer when EN_HS_TX_1=1.
    // enable TX current when EN_HS_TX_I=1 or EN_HS_TERM=1.
    u4Reg = MOTG_PHY_REG_READ32(0x10);
    u4Reg &= (uint32_t)(~0x03030F00);    
    u4Reg |= (uint32_t)0x02020400;
    MOTG_PHY_REG_WRITE32(0x10, u4Reg);

    // wait PHY clock to be valid before deactive reset.
    do
    {
        // check if this board have usb module.
        u4Reg = MOTG_PHY_REG_READ32(M_REG_PHYC5);
        if ((u4Reg & 0xFFFF) != 0x1F0F)
        {
            printf("MUSB pBase = 0x%08X init fail.\n", (uint32_t)USB_ADDR);        
            return -1;
        }
        u4Reg = MOTG_MISC_REG_READ32(M_REG_AUTOPOWER);
    } while (!(u4Reg&M_REG_AUTOPOWER_PHYCLK));

    // RG_HS_RCV_EN_MODE1
    u4Reg = MOTG_PHY_REG_READ32(0x14);
    u4Reg &= (uint32_t)(~0x0000000C);
    u4Reg |= (uint32_t)0x0000000C;
    MOTG_PHY_REG_WRITE32(0x14, u4Reg);

    // DISC_TH = 615 mV.
    u4Reg = MOTG_PHY_REG_READ32(0x24);
    u4Reg &= (uint32_t)(~0x01C00000);
    u4Reg |= (uint32_t)0x01C00000;
    MOTG_PHY_REG_WRITE32(0x24, u4Reg);

    MOTG_PHY_REG_WRITE32(M_REG_PHYC5, 0x1F13);  

    // open session.
    nDevCtl = MOTG_REG_READ8(MGC_O_HDRC_DEVCTL);   
    nDevCtl |= MGC_M_DEVCTL_SESSION;        
    MOTG_REG_WRITE8(MGC_O_HDRC_DEVCTL, nDevCtl);

    mdelay(10);
    
    MOTG_PHY_REG_WRITE32(M_REG_PHYC5, 0xF03);  

#ifndef MTK_MHDRC   
    // turn on device address control at host mode.        
    MOTG_MISC_REG_WRITE32(M_REG_ACCESSUNIT, 0x12);
#endif

    MOTG_MISC_REG_WRITE32(0xE0, 0x1818);    

    // set user's customization HS_TERMC parameter.
/*    if (MGC_bHSTERMC <= 0x7)
    {
        u4Reg = MOTG_PHY_REG_READ32(0x10);
        u4Reg &= ~0x00000070;
        u4Reg |= MGC_bHSTERMC<<4;
        MOTG_PHY_REG_WRITE32(0x10, u4Reg);                        
    }
*/
    //-------------------------------------------------------------
    // Platform dependent
    //-------------------------------------------------------------
    // ACD Setting 0x20029410[4:6] RG_CALIB_SELE1 440mv
//    #if defined(CC_MT5363) || defined(CC_MT8225)
    u4Reg = MOTG_PHY_REG_READ32(0x10);
    u4Reg &= ~0x00000070;
    u4Reg |= 0x00000020;
    MOTG_PHY_REG_WRITE32(0x10, u4Reg);
//    #endif

    // Don't stop USB pll when entering suspend mode
//    #if defined(CC_MT5363) || defined(CC_MT8225)
    u4Reg = MOTG_PHY_REG_READ32(0x04);
    u4Reg |= 0x00000001;
    MOTG_PHY_REG_WRITE32(0x04, u4Reg);
//    #endif

    {
        // Enable USB 5V power.
        uint32_t* pu4MemTmp;
        pu4MemTmp = (uint32_t*)0x20028080;
        *pu4MemTmp = *pu4MemTmp | 0x2;
        pu4MemTmp = (uint32_t*)0x20028084;
        *pu4MemTmp = *pu4MemTmp | 0x2;
    }

    printf("MUSB pBase = 0x%08X init ok.\n", (uint32_t)USB_ADDR);

    return 0;
}

#elif defined(CONFIG_ARCH_MT5365) || defined(CONFIG_ARCH_MT5395)
static INT32 MUC_Initial(UINT32 u4Port)
{
    uint32_t u4Reg;
    uint8_t nDevCtl;
    uint32_t* pu4MemTmp;

#if 0
    // don't use it, if using it, update code system will die in uboot.
    u4Reg = MOTG_PHY_REG_READ32(0x24);
    printf("MUSB MUC_Initial u4Reg= 0x%x.\n", u4Reg);  

    u4Reg &= 0x01c00000;
    if( u4Reg == 0x01c00000 )
    {
        // On MT5365/5395, we only do the soft reset for one time.
         return TRUE;
    }
#endif
    // make sure DRAM clock is on. 
    u4Reg = MOTG_MISC_REG_READ32(M_REG_AUTOPOWER);
    MOTG_MISC_REG_WRITE32(M_REG_AUTOPOWER, (u4Reg | M_REG_AUTOPOWER_DRAMCLK));

    // reset all h/w register.
    MOTG_MISC_REG_WRITE32(M_REG_SOFTRESET, M_REG_SOFT_RESET_ACTIVE);

    mdelay(10);
    
    MOTG_MISC_REG_WRITE32(M_REG_SOFTRESET, M_REG_SOFT_RESET_DEACTIVE);

    pu4MemTmp = (uint32_t*)0xf0029408;
    *pu4MemTmp = 0x83283047;

    // set REL_SUSP, USBRST
    u4Reg = MOTG_PHY_REG_READ32(0x00);
    u4Reg &= (uint32_t)(~0x00030000);    
    u4Reg |= (uint32_t)0x00030000;
    MOTG_PHY_REG_WRITE32(0x00, u4Reg);

    mdelay(10);

    u4Reg = MOTG_PHY_REG_READ32(0x00);
    u4Reg &= (uint32_t)(~0x00030000);    
    u4Reg |= (uint32_t)0x00010000;
    MOTG_PHY_REG_WRITE32(0x00, u4Reg);

    mdelay(10);

    // Set RG_HSDISC_DEGL.  --> cancelled on mt5365/mt5366
    // Reset RG_EARLY_HSTX_I
    // Set RG_PLL_STABLE    
    u4Reg = MOTG_PHY_REG_READ32(0x04);
    u4Reg &= (uint32_t)(~0x01004000);    
    u4Reg |= (uint32_t)0x00000002;
    MOTG_PHY_REG_WRITE32(0x04, u4Reg);

    // wait PHY clock to be valid before deactive reset.
    do
    {
        // check if this board have usb module.
        u4Reg = MOTG_PHY_REG_READ32(M_REG_PHYC5);
        if ((u4Reg & 0xFFFF) != 0x1F0F)
        {
            printf("MUSB pBase = 0x%08X init fail.\n", (uint32_t)USB_ADDR);        
            return -1;
        }
        u4Reg = MOTG_MISC_REG_READ32(M_REG_AUTOPOWER);
    } while (!(u4Reg&M_REG_AUTOPOWER_PHYCLK));

    // DISC_TH = 615 mV.
    u4Reg = MOTG_PHY_REG_READ32(0x24);
    u4Reg &= (uint32_t)(~0x01C00000);
    u4Reg |= (uint32_t)0x01C00000;
    MOTG_PHY_REG_WRITE32(0x24, u4Reg);

    MOTG_PHY_REG_WRITE32(M_REG_PHYC5, 0x1F13);  

    // open session.
    nDevCtl = MOTG_REG_READ8(MGC_O_HDRC_DEVCTL);   
    nDevCtl |= MGC_M_DEVCTL_SESSION;        
    MOTG_REG_WRITE8(MGC_O_HDRC_DEVCTL, nDevCtl);

    mdelay(10);
    
    MOTG_PHY_REG_WRITE32(M_REG_PHYC5, 0xF03);  

#ifndef MTK_MHDRC   
    // turn on device address control at host mode.        
    MOTG_MISC_REG_WRITE32(M_REG_ACCESSUNIT, 0x12);
#endif

    MOTG_MISC_REG_WRITE32(0xE0, 0x1818);    

#if 0   
    // HS Slew Rate
    u4Reg = MOTG_PHY_REG_READ32(0x10);
    u4Reg &= (uint32_t)(~0x0000000e);
    u4Reg |= (uint32_t)0x00000008;
    MOTG_PHY_REG_WRITE32(0x10, u4Reg);
#else
    // HS Slew Rate AutoCalibration
    uint32_t u4SlewRate;
    uint32_t u4Clk;
    uint32_t u4FmOrg;
    u4Reg = MOTG_PHY_REG_READ32(0x1c);
    u4Reg &=(uint32_t)(~0x00500000);
    u4Reg |= (uint32_t)0x00500000;
    MOTG_PHY_REG_WRITE32(0x1c, u4Reg);

    mdelay(1);


    #ifdef CONFIG_ARCH_MT5365
    // Frequency Meter
    pu4MemTmp = (uint32_t*)0xf000d1c0;
    u4Reg = *pu4MemTmp;
    u4FmOrg = u4Reg;
    u4Reg &= (uint32_t)(~0x3f000000);
    if ( (uint32_t)USB_ADDR == 0xf0029000 )
    {
        u4Reg |= (uint32_t)0x14008000;
    }
    else
    {
        u4Reg |= (uint32_t)0x1B008000;
    }
    *pu4MemTmp = u4Reg;
    #elif defined(CONFIG_ARCH_MT5395)
    // Frequency Meter
    pu4MemTmp = (uint32_t*)0xf000d1c0;
    u4Reg = *pu4MemTmp;
    u4FmOrg = u4Reg;
    u4Reg &= (uint32_t)(~0x00003f00);
    if ( (uint32_t)USB_ADDR == 0xf0029000 )
    {
        u4Reg |= (uint32_t)0x0000a900;
    }
    else
    {
        u4Reg |= (uint32_t)0x0000aa00;
    }
    *pu4MemTmp = u4Reg;
    #endif
    
    mdelay(1);
        
    pu4MemTmp = (uint32_t*)0xf000d1cc;
    u4Reg = *pu4MemTmp;
    u4Clk = u4Reg*27/2048;

    // x/u4Clk = 5/130 --> x = 5*u4Clk/130
    u4SlewRate = ((((5*u4Clk/130)*2)+1)/2);

    u4Reg = MOTG_PHY_REG_READ32(0x10);
    u4Reg &= (uint32_t)(~0x0000000e);
    u4Reg |= (u4SlewRate<<1);
    MOTG_PHY_REG_WRITE32(0x10, u4Reg);

    pu4MemTmp = (uint32_t*)0xf00d1c0;
    *pu4MemTmp = u4FmOrg;
    printf(" MUSB Auto Calibration : 0x%x (%dMhz)\n", u4SlewRate, u4Clk);

    u4Reg = MOTG_PHY_REG_READ32(0x08);
    u4Reg &= (uint32_t)(~0x00000070);
    u4Reg |= (uint32_t)0x00000050;
    MOTG_PHY_REG_WRITE32(0x08, u4Reg);

    u4Reg = MOTG_PHY_REG_READ32(0x10);
    u4Reg &= (uint32_t)(~0x0000001c0);
    u4Reg |= (uint32_t)0x000000c0;
    MOTG_PHY_REG_WRITE32(0x10, u4Reg);
     
    // FS/LS change
    u4Reg = MOTG_PHY_REG_READ32(0x18);
    u4Reg &= (uint32_t)(~0x00043300);
    u4Reg |= (uint32_t)0x00042200;
    MOTG_PHY_REG_WRITE32(0x18, u4Reg);
#endif

    //-------------------------------------------------------------
    // Platform dependent
    //-------------------------------------------------------------


    printf("MUSB uboot pBase = 0x%08X init ok.\n", (uint32_t)USB_ADDR);

    return 0;
}
#elif defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5881) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
static INT32 MUC_Initial(UINT32 u4Port)
{
    uint32_t u4Reg = 0;
    uint8_t nDevCtl = 0;
    uint32_t *pu4MemTmp;

    //Soft Reset, RG_RESET for Soft RESET
    u4Reg = MOTG_PHY_REG_READ32(0x68);
    u4Reg |=   0x00004000; 
    MOTG_PHY_REG_WRITE32(0x68, u4Reg);
    u4Reg &=  ~0x00004000; 
    MOTG_PHY_REG_WRITE32(0x68, u4Reg);	

    #ifdef CONFIG_ARCH_MT5880
    pu4MemTmp = (uint32_t *)0xf0061300;
    *pu4MemTmp = 0x00cfff00;
    #else
    pu4MemTmp = (uint32_t *)0xf0060200;
    *pu4MemTmp = 0x00cfff00;
    #endif



    // VRT insert R enable 
    u4Reg = MOTG_PHY_REG_READ32(0x00);
    u4Reg |= 0x4000;
    MOTG_PHY_REG_WRITE32(0x00, u4Reg);



    //otg bit setting
    u4Reg = MOTG_PHY_REG_READ32(0x6C);
    u4Reg &= ~0x00003f3f;
    u4Reg |=  0x00003f2c;
    MOTG_PHY_REG_WRITE32(0x6C, u4Reg);

    //suspendom control
    u4Reg = MOTG_PHY_REG_READ32(0x68);
    u4Reg &=  ~0x00040000; 
    MOTG_PHY_REG_WRITE32(0x68, u4Reg);

    u4Reg = MOTG_REG_READ8(M_REG_PERFORMANCE3);
    u4Reg |=  0x80;
    u4Reg &= ~0x40;
    MOTG_REG_WRITE8(M_REG_PERFORMANCE3, (uint8_t)u4Reg);

    // MT5368/MT5396/MT5389 
    //0xf0050070[10]= 1 for Fs Hub + Ls Device 
    u4Reg = MOTG_REG_READ8(0x71);
    u4Reg |= 0x04;
    MOTG_REG_WRITE8(0x71, (uint8_t)u4Reg);

    // FS/LS Slew Rate change
    u4Reg = MOTG_PHY_REG_READ32(0x10);
    u4Reg &= (uint32_t)(~0x000000ff);
    u4Reg |= (uint32_t)0x00000011;
    MOTG_PHY_REG_WRITE32(0x10, u4Reg);

    // HS Slew Rate
    u4Reg = MOTG_PHY_REG_READ32(0x10);
    u4Reg &= (uint32_t)(~0x00070000);
    u4Reg |= (uint32_t)0x00040000;
    MOTG_PHY_REG_WRITE32(0x10, u4Reg);
#if 1
    // This is important: TM1_EN
    // speed up OTG setting for checking device connect event after MUC_Initial().
    u4Reg = MOTG_REG_READ32(0x604);
    u4Reg |= 0x01;
    MOTG_REG_WRITE32(0x604, u4Reg);
#endif
    // open session.
    nDevCtl = MOTG_REG_READ8(MGC_O_HDRC_DEVCTL);	 
    nDevCtl |= MGC_M_DEVCTL_SESSION;		
    MOTG_REG_WRITE8(MGC_O_HDRC_DEVCTL, nDevCtl);

    mdelay(10);
#if 1
    // This is important: TM1_EN 
    // speed up OTG setting for checking device connect event after MUC_Initial().
    u4Reg = MOTG_REG_READ32(0x604);
    u4Reg &= ~0x01;
    MOTG_REG_WRITE32(0x604, u4Reg);

#if defined(CONFIG_ARCH_MT5389) 
 //MT5389 U40 corner eye diagram improve   
    u4Reg = MOTG_PHY_REG_READ32( 0x1C);	
    u4Reg |= (uint32_t)0x03000000;	  
    MOTG_PHY_REG_WRITE32( 0x1C, u4Reg);
#endif
	
#endif

    return 0;
}
#endif

//-------------------------------------------------------------------------
/** MUC_PrepareRx
 *  host controller init register for 539x.
 *  @param  void 
 *  @retval void
 */
//-------------------------------------------------------------------------
static void MUC_PrepareRx(struct usb_device *prDev, UINT32 u4Pipe)
{
    UINT8 u1Reg = 0;
    UINT32 u4DevNum = usb_pipedevice(u4Pipe);
    UINT32 u4EpNum = usb_pipeendpoint(u4Pipe);
    UINT32 u4Port = (UINT32)prDev->portnum;

#ifdef MTK_MHDRC	

    UINT8 bHubAddr = 0;
    UINT8 bHubPort = 0;    
    UINT8 bIsMulti = FALSE;

    if (usb_pipecontrol(u4Pipe))
    {
        u1Reg = MENTOR_PIPE_CONTROL << MGC_S_TYPE_PROTO;
    }
    else if (usb_pipebulk(u4Pipe))
    {
        u1Reg = MENTOR_PIPE_BULK << MGC_S_TYPE_PROTO;
    }
    
    u1Reg |= u4EpNum & MGC_M_TYPE_REMOTE_END;

    /* speed field in proto reg */
    switch(mtkusbport[u4Port].u4DeviceSpeed)
    {
    case MUSB_CONNECTION_SPEED_LOW:
        u1Reg |= 0xc0;
        break;
    case MUSB_CONNECTION_SPEED_FULL:
        u1Reg |= 0x80;
        break;
    default:
        u1Reg |= 0x40;
    }

    MOTG_REG_WRITE8(M_REG_RXTYPE, u1Reg);

    /* target addr & hub addr/port */
    MOTG_REG_WRITE8(MGC_BUSCTL_OFFSET(u4EpNum, MGC_O_MHDRC_RXFUNCADDR), 
        u4DevNum);
    MOTG_REG_WRITE8(MGC_BUSCTL_OFFSET(u4EpNum, MGC_O_MHDRC_RXHUBADDR), 
        (bIsMulti ? (0x80 | bHubAddr) : bHubAddr));
    MOTG_REG_WRITE8(MGC_BUSCTL_OFFSET(u4EpNum, MGC_O_MHDRC_RXHUBPORT), 
        bHubPort);

#else

    if (usb_pipecontrol(u4Pipe))
    {
        u1Reg = MENTOR_PIPE_CONTROL << MGC_S_TYPE_PROTO;
    }
    else if (usb_pipebulk(u4Pipe))
    {
        u1Reg = MENTOR_PIPE_BULK << MGC_S_TYPE_PROTO;
    }
    
    u1Reg |= u4EpNum & MGC_M_TYPE_REMOTE_END;

    MOTG_REG_WRITE8(M_REG_RXTYPE, u1Reg);
           
#endif

    return;
}

//-------------------------------------------------------------------------
/** MUC_PrepareTx
 *  host controller init register for 539x.
 *  @param  void 
 *  @retval void
 */
//-------------------------------------------------------------------------
static void MUC_PrepareTx(struct usb_device *prDev, UINT32 u4Pipe)
{
    UINT8 u1Reg = 0;
    UINT32 u4DevNum = usb_pipedevice(u4Pipe);
    UINT32 u4EpNum = usb_pipeendpoint(u4Pipe);
    UINT32 u4Port = (UINT32)prDev->portnum;

#ifdef MTK_MHDRC	

    UINT8 bHubAddr = 0;
    UINT8 bHubPort = 0;    
    UINT8 bIsMulti = FALSE;

    if (usb_pipecontrol(u4Pipe))
    {
        u1Reg = MENTOR_PIPE_CONTROL << MGC_S_TYPE_PROTO;
    }
    else if (usb_pipebulk(u4Pipe))
    {
        u1Reg = MENTOR_PIPE_BULK << MGC_S_TYPE_PROTO;
    }
    
    u1Reg |= u4EpNum & MGC_M_TYPE_REMOTE_END;

    /* speed field in proto reg */
    switch(mtkusbport[u4Port].u4DeviceSpeed)
    {
    case MUSB_CONNECTION_SPEED_LOW:
        u1Reg |= 0xc0;
        break;
    case MUSB_CONNECTION_SPEED_FULL:
        u1Reg |= 0x80;
        break;
    default:
        u1Reg |= 0x40;
    }

    MOTG_REG_WRITE8(M_REG_TXTYPE, u1Reg);

    /* target addr & hub addr/port */
    MOTG_REG_WRITE8(MGC_BUSCTL_OFFSET(u4EpNum, MGC_O_MHDRC_TXFUNCADDR), 
        u4DevNum);
    MOTG_REG_WRITE8(MGC_BUSCTL_OFFSET(u4EpNum, MGC_O_MHDRC_TXHUBADDR), 
        (bIsMulti ? (0x80 | bHubAddr) : bHubAddr));
    MOTG_REG_WRITE8(MGC_BUSCTL_OFFSET(u4EpNum, MGC_O_MHDRC_TXHUBPORT), 
        bHubPort);
	
    if(!u4EpNum)
    {
        MOTG_REG_WRITE8(MGC_BUSCTL_OFFSET(u4EpNum, MGC_O_MHDRC_RXFUNCADDR), 
            u4DevNum);
        MOTG_REG_WRITE8(MGC_BUSCTL_OFFSET(u4EpNum, MGC_O_MHDRC_RXHUBADDR), 
            (bIsMulti ? (0x80 | bHubAddr) : bHubAddr));
        MOTG_REG_WRITE8(MGC_BUSCTL_OFFSET(u4EpNum, MGC_O_MHDRC_RXHUBPORT), 
            bHubPort);    
    }

#else

    if (usb_pipecontrol(u4Pipe))
    {
        u1Reg = MENTOR_PIPE_CONTROL << MGC_S_TYPE_PROTO;
    }
    else if (usb_pipebulk(u4Pipe))
    {
        u1Reg = MENTOR_PIPE_BULK << MGC_S_TYPE_PROTO;
    }
    
    u1Reg |= u4EpNum & MGC_M_TYPE_REMOTE_END;

    MOTG_REG_WRITE8(M_REG_TXTYPE, u1Reg);
    
#endif

    return;
}
//-------------------------------------------------------------------------
/** MUC_SetupPkt
 *  send setup packet for ep0.
 * SETUP starts a new control request.  Devices are not allowed to
 * STALL or NAK these; they must cancel any pending control requests. 
 *  @param  prDev: point to struct usb_device.
 *  @param  u4Pipe               selected pipe.
 *  @param   *prSetup         point to setup packet.  
 *  @retval 0	Success
 *  @retval Others	Error
 */
//-------------------------------------------------------------------------
static INT32 MUC_SetupPkt(struct usb_device *prDev, UINT32 u4Pipe, struct devrequest *prSetup)
{
    UINT32 u4Len;
    UINT32 u4EpNum = usb_pipeendpoint(u4Pipe);
    UINT32 u4Port = (UINT32)prDev->portnum;

    MOTG_REG_WRITE8(M_REG_INDEX, u4EpNum);

    //  turn on nak limit.
    MOTG_REG_WRITE8(M_REG_NAKLIMIT0, 
    	(((mtkusbport[u4Port].u4DeviceSpeed) == MUSB_CONNECTION_SPEED_HIGH) ? 16 : 13));

    u4Len = sizeof(struct devrequest);

    MUC_PrepareTx(prDev, u4Pipe);

    MUC_WriteFifo((UINT32)prDev->portnum, (UINT8 *)prSetup, u4EpNum, u4Len);

    MOTG_REG_WRITE16(M_REG_CSR0, M_CSR0_H_NO_PING | M_CSR0_TXPKTRDY | M_CSR0_H_SETUPPKT);

    return MUC_WaitEpIrq(prDev, u4EpNum, USB_OUT, DMA_OFF);
}

//-------------------------------------------------------------------------
/** MUC_StatusPkt
 *  send setup packet for ep0.
 * SETUP starts a new control request.  Devices are not allowed to
 * STALL or NAK these; they must cancel any pending control requests. 
 *  @param  u4Pipe               selected pipe.
 *  @retval 0	Success
 *  @retval Others	Error
 */
//-------------------------------------------------------------------------
static INT32 MUC_StatusPkt(struct usb_device *prDev, UINT32 u4Pipe)
{
    UINT32 u4DevNum = usb_pipedevice(u4Pipe);
    UINT32 u4EpNum = usb_pipeendpoint(u4Pipe);
    INT32 i4Ret = 0;
    UINT32 u4Port = (UINT32)prDev->portnum;

    MOTG_REG_WRITE32(M_REG_EPXADDR(u4EpNum), u4DevNum);

    MOTG_REG_WRITE8(M_REG_INDEX, u4EpNum);

    if (usb_pipein(u4Pipe))
    {   
        MUC_PrepareTx(prDev, u4Pipe);
    
        MOTG_REG_WRITE8(M_REG_CSR0, M_CSR0_TXPKTRDY | M_CSR0_H_STATUSPKT);
    }
    else
    {
        MUC_PrepareRx(prDev, u4Pipe);
    
        MOTG_REG_WRITE8(M_REG_CSR0, M_CSR0_H_REQPKT | M_CSR0_H_STATUSPKT);
    }

    i4Ret = MUC_WaitEpIrq(prDev, u4EpNum, usb_pipeout(u4Pipe), DMA_OFF);

    //  Must clear RxPktRdy in CSR0 when packet has read from FIFO.
    MOTG_REG_WRITE8(M_REG_CSR0, 0);

    return i4Ret;
}

//-------------------------------------------------------------------------
/** MUC_log2
 *  Calculate log2 arithmetic.
 *  @param  x   input value.
 *  @return  log2(x).
 */
//-------------------------------------------------------------------------
static inline s32 MUC_log2(s32 x)
{
    s32 i;

    for (i = 0; x > 1; i++)
    {
        x = x / 2;
    }

    return i;
}

//-------------------------------------------------------------------------
/** MUC_InPkt
 *  send in request to device.
 *  @param   *prDev         usb_device data structure.
 *  @param  u4Pipe               selected pipe.
 *  @param   *pvBuf             user's request data buffer.  
 *  @param   u4Len              user's request data length.   
 *  @retval 0	Success
 *  @retval Others	Error
 */
//------------------------------------------------------------------------- 
static INT32 MUC_InPkt(struct usb_device *prDev, UINT32 u4Pipe, void *pvBuf, UINT32 u4Len)
{
    UINT32 u4DevNum = usb_pipedevice(u4Pipe);
    UINT32 u4EpNum = usb_pipeendpoint(u4Pipe);
    UINT32 u4MaxPktSize = usb_maxpacket(prDev, u4Pipe);
    UINT32 u4RxLen = 0;
    UINT32 u4PktLen = 0;
    INT32 i4Ret = 0;
    UINT32 u4Port = (UINT32)prDev->portnum;

    if (u4EpNum > USB_MAX_EP_NUM)
    {
        u4EpNum = USB_MAX_EP_NUM;
    }
    
    // set device address.
    MOTG_REG_WRITE32(M_REG_EPXADDR(u4EpNum), u4DevNum);

    // set physical ep.
    MOTG_REG_WRITE8(M_REG_INDEX, u4EpNum);

    MUC_PrepareRx(prDev, u4Pipe);

    // set fifo attribute.
    if (usb_pipebulk(u4Pipe))
    {   
        //  turn on nak limit.
        MOTG_REG_WRITE8(M_REG_RXINTERVAL, 
        (((mtkusbport[u4Port].u4DeviceSpeed) == MUSB_CONNECTION_SPEED_HIGH) ? 16 : 13));

#ifdef CONFIG_USB_PL2303
        do{
            extern struct usb_device *pPL2303Dev;
            
            if (pPL2303Dev == prDev)
            {
                //  turn on nak limt for PL2303.
                MOTG_REG_WRITE8(M_REG_RXINTERVAL, 2);
            }
        }while(0);
#endif
		            	    	
        if (MOTG_REG_READ16(M_REG_RXMAXP) < u4MaxPktSize)
        {
            // set rx epx fifo size,                 
            MOTG_REG_WRITE8(M_REG_RXFIFOSZ, MUC_log2(u4MaxPktSize /8));

            // set start address of tx epx fifo.
            MOTG_REG_WRITE16(M_REG_RXFIFOADD, 8);

            //  Establish max packet size.
            MOTG_REG_WRITE16(M_REG_RXMAXP, u4MaxPktSize);
        }
    }

    // use FIFO only.
    u4RxLen = 0;
    while (u4RxLen < u4Len)
    {
        // request an IN transaction.
        if (u4EpNum > 0)
        {
            MOTG_REG_WRITE8(M_REG_RXCSR1, M_RXCSR1_H_REQPKT);
        }
        else
        {
            MOTG_REG_WRITE8(M_REG_CSR0, M_CSR0_H_REQPKT);
        }
        
        i4Ret = MUC_WaitEpIrq(prDev, u4EpNum, USB_IN, DMA_OFF);
        if (0 > i4Ret)
        {
            return i4Ret;
        }

        u4PktLen = MOTG_REG_READ16(M_REG_RXCOUNT);

        // read data from fifo.		
        MUC_ReadFifo((UINT32)prDev->portnum, pvBuf, u4EpNum, u4PktLen);

        u4RxLen += u4PktLen;
        prDev->act_len += u4PktLen;;
        pvBuf += u4PktLen;        

        // check if short packet.
        if (u4PktLen < u4MaxPktSize)
        {
            return 0;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------
/** MUC_OutPkt
 *  send out data to device.
 *  @param   *prDev         usb_device data structure.
 *  @param  u4Pipe               selected pipe.
 *  @param   *pvBuf             user's request data buffer.  
 *  @param   u4Len              user's request data length.   
 *  @retval 0	Success
 *  @retval Others	Error
 */
//------------------------------------------------------------------------- 
static INT32 MUC_OutPkt(struct usb_device *prDev, UINT32 u4Pipe, void *pvBuf, UINT32 u4Len)
{
    UINT32 u4DevNum = usb_pipedevice(u4Pipe);
    UINT32 u4EpNum = usb_pipeendpoint(u4Pipe);
    UINT32 u4MaxPktSize = usb_maxpacket(prDev, u4Pipe);
    UINT32 u4TxLen = 0;
    UINT32 u4PktLen = 0;
    INT32 i4Ret = 0;
    UINT32 u4Port = (UINT32)prDev->portnum;

    if (u4EpNum > USB_MAX_EP_NUM)
    {
        u4EpNum = USB_MAX_EP_NUM;
    }
    
    // set device address.
    MOTG_REG_WRITE32(M_REG_EPXADDR(u4EpNum), u4DevNum);

    // set physical ep.
    MOTG_REG_WRITE8(M_REG_INDEX, u4EpNum);

    MUC_PrepareTx(prDev, u4Pipe);

    // set fifo attribute.
    if (usb_pipebulk(u4Pipe))
    {
        //  turn on nak limit.
        MOTG_REG_WRITE8(M_REG_TXINTERVAL, 
        	(((mtkusbport[u4Port].u4DeviceSpeed) == MUSB_CONNECTION_SPEED_HIGH) ? 16 : 13));
    
        if (MOTG_REG_READ16(M_REG_TXMAXP) < u4MaxPktSize)
        {
            // set tx epx fifo size,                 
            MOTG_REG_WRITE8(M_REG_TXFIFOSZ, MUC_log2(u4MaxPktSize /8));

            // set start address of tx epx fifo.
            MOTG_REG_WRITE16(M_REG_TXFIFOADD, 8);

            //  Establish max packet size.
            MOTG_REG_WRITE16(M_REG_TXMAXP, u4MaxPktSize);
        }
    }

    // use FIFO only.
    u4TxLen = 0;
    while (u4TxLen < u4Len)
    {
        // max packet size as tx packet size limit.
        u4PktLen = min(u4MaxPktSize, (u4Len - u4TxLen));

        // write data to fifo.		
        MUC_WriteFifo((UINT32)prDev->portnum, pvBuf, u4EpNum, u4PktLen);
        
        if (u4EpNum > 0)
        {
            MOTG_REG_WRITE8(M_REG_TXCSR, M_TXCSR_TXPKTRDY);
        }
        else
        {
            MOTG_REG_WRITE8(M_REG_CSR0, M_CSR0_TXPKTRDY);
        }

        i4Ret = MUC_WaitEpIrq(prDev, u4EpNum, USB_OUT, DMA_OFF);
        if (0 > i4Ret)
        {
            return i4Ret;
        }

        u4TxLen += u4PktLen;
        pvBuf += u4PktLen;            
    }

    return i4Ret;
}

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------
//-------------------------------------------------------------------------
/** usb_sendse0nak
 *  set se0 nak test mode.
 *  @void.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int usb_sendse0nak(void)
{
        UINT32 u4Port;
        for(u4Port=0; u4Port<(sizeof(mtkusbport) / sizeof(MTKUSBPORT)); u4Port++)
        {
        	  if(u4Port > 3)
        	  {
        	      return -1;	
        	  }
            if (mtkusbport[u4Port].u4Insert)
            {
                break;
            }
        }

        //set se0 nak.
        MOTG_REG_WRITE8(0xF, 0x01);
        printf("USB: Set SE0 NAK...OK !\n");
        return 0;            
}

//-------------------------------------------------------------------------
/** usb_sendtestj
 *  set test j test mode.
 *  @void.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int usb_sendtestj(void)
{
        UINT32 u4Port;
        for(u4Port=0; u4Port<(sizeof(mtkusbport) / sizeof(MTKUSBPORT)); u4Port++)
        {
        	  if(u4Port > 3)
        	  {
        	      return -1;	
        	  }
            if (mtkusbport[u4Port].u4Insert)
            {
                break;
            }
        }

        //  USB-IF: 539x D+/D- will over spec.
        MOTG_PHY_REG_WRITE32(0x04, 0x4A010406);
        MOTG_PHY_REG_WRITE32(0x08, 0x78004302);

        //set test j.
        MOTG_REG_WRITE8(0xF, 0x02);
        printf("USB: Set Test J...OK !\n");
        return 0;            
}
//-------------------------------------------------------------------------
/** usb_sendtestk
 *  set test k test mode.
 *  @void.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int usb_sendtestk(void)
{
        UINT32 u4Port;
        for(u4Port=0; u4Port<(sizeof(mtkusbport) / sizeof(MTKUSBPORT)); u4Port++)
        {   
        	  if(u4Port > 3)
        	  {
        	      return -1;	
        	  }
            if (mtkusbport[u4Port].u4Insert)
            {
                break;
            }
        }

        //  USB-IF: 539x D+/D- will over spec.
        MOTG_PHY_REG_WRITE32(0x04, 0x4A010406);
        MOTG_PHY_REG_WRITE32(0x08, 0x78004302);

        //set test k.
        MOTG_REG_WRITE8(0xF, 0x04);
        printf("USB: Set Test K...OK !\n");
        return 0;            
}

//-------------------------------------------------------------------------
/** usb_sendtestpacket
 *  send test packet.
 *  @void.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int usb_sendtestpacket(void)
{
        UINT32 u4Port;
        for(u4Port=0; u4Port<(sizeof(mtkusbport) / sizeof(MTKUSBPORT)); u4Port++)
        {
            if (mtkusbport[u4Port].u4Insert)
            {
                break;
            }
        }

        if ( u4Port >= (sizeof(mtkusbport) / sizeof(MTKUSBPORT)) )
        {
            return -1;    
        }
        MOTG_REG_WRITE8(M_REG_INDEX, 0);
        // 53 bytes.
        MUC_WriteFifo(u4Port, (UINT8 *)_aTestPacket, 0, 53);
        //set test packet.
        MOTG_REG_WRITE8(0xF, 0x08);
        MOTG_REG_WRITE8(M_REG_CSR0, M_CSR0_TXPKTRDY);
        printf("USB: Sending test packet...OK !\n");
        return 0;            
}

//-------------------------------------------------------------------------
/** usb_speed
 *  usb module init speed.
 *  @param  speed = TRUE: high speed enable, FALSE: only Full speed enable.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int usb_speed(int speed)
{
        UINT32 u4Port;
        for(u4Port=0; u4Port<(sizeof(mtkusbport) / sizeof(MTKUSBPORT)); u4Port++)
        {
            mtkusbport[u4Port].u4HostSpeed = speed;
        }

        printf("Set speed = %s.\n", ((speed == HS_ON) ? "High speed" : "Full speed"));

        return 0;            
}
//-------------------------------------------------------------------------
/** usb_lowlevel_init
 *  usb module init.
 *  @param  void.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int usb_lowlevel_init(void)
{
    UINT32 u4Port;

    for(u4Port=0; u4Port<(sizeof(mtkusbport) / sizeof(MTKUSBPORT)); u4Port++)
    {
        if (!mtkusbport[u4Port].u4Insert)
        {
            if ((MUC_Initial(u4Port) == 0) && (MUC_DevInsert(u4Port) == 0))
            {
                printf("USB: Device on port %d.\n", (int)u4Port);
                mtkusbport[u4Port].u4Insert = 1;
                return 0;            
            }
            else
            {
                printf("USB: No Device on prot %d.\n", (int)u4Port);
            }
        }
	else
	{
		printf("USB: device on port %d.\n", (int)u4Port);
		return 0;
	}
    }
    return -1;
}

int usb_lowlevel_init_with_port_num(int iPortNum)
{
    UINT32 u4Port = (UINT32)iPortNum;

    if (!mtkusbport[u4Port].u4Insert)
    {
        if ((MUC_Initial(u4Port) == 0) && (MUC_DevInsert(u4Port) == 0))
        {
            printf("USB: Device on port %d.\n", (int)u4Port);
            mtkusbport[u4Port].u4Insert = 1;
            return 0;            
        }
        else
        {
            printf("USB: No Device on prot %d.\n", (int)u4Port);
        }
    }
    else
    {
	printf("USB: device on port %d.\n", (int)u4Port);
	return -1;
    }
    return -1;
}
//-------------------------------------------------------------------------
/** usb_silent_reset
 *  usb silent reset device.
 *  @param  void.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int usb_silent_reset(void)
{
#if 0
    volatile UINT32 reg = 0; /* read USB registers into this */

    // try to reset device.
    reg = MOTG_REG_READ8(M_REG_POWER);
    reg |= M_POWER_RESET;
    MOTG_REG_WRITE8(M_REG_POWER, reg);

    // reset device address.
    MOTG_REG_WRITE8(M_REG_FADDR, 0);
    MOTG_REG_WRITE32(M_REG_EP0ADDR, 0);
    MOTG_REG_WRITE32(M_REG_EP1ADDR, 0);
    MOTG_REG_WRITE32(M_REG_EP2ADDR, 0);
    MOTG_REG_WRITE32(M_REG_EP3ADDR, 0);
    MOTG_REG_WRITE32(M_REG_EP4ADDR, 0);

    mdelay(100);

    // clear reset.
    reg = MOTG_REG_READ8(M_REG_POWER);
    reg &= ~M_POWER_RESET;
    MOTG_REG_WRITE8(M_REG_POWER, reg);

    // check device speed: LS, FS, HS.
    reg = MOTG_REG_READ8(M_REG_DEVCTL);
    if (reg & M_DEVCTL_LSDEV)
    {
        printf("USB Silent Reset: LS device.\n");
        _u4DeviceSpeed = MUSB_CONNECTION_SPEED_LOW;
    }
    else
    {
        reg = MOTG_REG_READ8(M_REG_POWER);
        if (reg & M_POWER_HSMODE)
        {
            printf("USB Silent Reset: HS device.\n");
            _u4DeviceSpeed = MUSB_CONNECTION_SPEED_HIGH;
        }
        else
        {
            printf("USB Silent Reset: FS device.\n");
            _u4DeviceSpeed = MUSB_CONNECTION_SPEED_FULL;
        }
    }

    _u4Insert = TRUE;
#endif

    return 0;
}

//-------------------------------------------------------------------------
/** usb_lowlevel_stop
 *  usb module stop.
 *  @param  void.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int usb_lowlevel_stop(void)
{
    UINT32 u4Port;

    for(u4Port=0; u4Port<(sizeof(mtkusbport) / sizeof(MTKUSBPORT)); u4Port++)
    {
        if (mtkusbport[u4Port].u4Insert)
        {
            printf("USB: Stop device on port %d.\n", (int)u4Port);
            mtkusbport[u4Port].u4Insert = 0;
        }
    }

    return 0;
}

//-------------------------------------------------------------------------
/** submit_bulk_msg
 *  submit bulk transfer.
 *  @param  prDev: point to struct usb_device.
 *  @param  u4Pipe: usb u4Pipe.
 *  @param  puBuf: point to data buffer.
 *  @param  len: data buffer length.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int submit_bulk_msg(struct usb_device *prDev, unsigned long u4Pipe, void *pvBuf,
		    int i4Len)
{
        INT32 i4Ret = 0;
        UINT32 u4Port = (UINT32)prDev->portnum;

        if (!mtkusbport[u4Port].u4Insert)
        {
            prDev->act_len = 0;
            prDev->status = USB_ST_BIT_ERR;
            return -1;
        }       

	LOG(7, "prDev = %ld u4Pipe = %ld buf = %p size = %d dir_out = %d\n",
	       usb_pipedevice(u4Pipe), usb_pipeendpoint(u4Pipe), pvBuf, i4Len, usb_pipeout(u4Pipe));

	prDev->status = 0;
        prDev->act_len = 0;
        if ((i4Len > 0) && (pvBuf))
        {
            if (usb_pipein(u4Pipe))
            {
                i4Ret = MUC_InPkt(prDev, u4Pipe, pvBuf, i4Len);
            }
            else
            {
                i4Ret = MUC_OutPkt(prDev, u4Pipe, pvBuf, i4Len);
                prDev->act_len = i4Len;                
            }
            
            if (i4Ret < 0)
            {
                prDev->act_len = 0;
                return i4Ret;    
            }
        }

        return i4Ret;
}

//-------------------------------------------------------------------------
/** submit_control_msg
 *  submit control transfer.
 *  @param  prDev: point to struct usb_device.
 *  @param  u4Pipe: usb u4Pipe.
 *  @param  puBuf: point to data buffer.
 *  @param  len: data buffer length.
 *  @param  setup: point to struct devrequest.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int submit_control_msg(struct usb_device *prDev, unsigned long u4Pipe, void *pvBuf,
		       int i4Len, struct devrequest *prSetup)
{
        INT32 i4Ret = 0;
        UINT32 u4Port = (UINT32)prDev->portnum;

        if (!mtkusbport[u4Port].u4Insert)
        {
            prDev->act_len = 0;
            prDev->status = USB_ST_BIT_ERR;
            return -1;
        }       

	prDev->status = 0;

	LOG(7, "prDev = %d u4Pipe = %ld buf = %p size = %d rt = %#x req = %#x.\n",
	       usb_pipedevice(u4Pipe), usb_pipeendpoint(u4Pipe), 
	       pvBuf, i4Len, (int)prSetup->requesttype, (int)prSetup->request);

        /* setup phase */
	if (MUC_SetupPkt(prDev, u4Pipe, prSetup) == 0)
        {
	    /* data phase */
            if ((i4Len > 0) && (pvBuf))
            {
                // wait device prepare ok.
                mdelay(10);
            
                if (usb_pipein(u4Pipe))
                {
                    i4Ret = MUC_InPkt(prDev, u4Pipe, pvBuf, i4Len);
                }
                else
                {
                    i4Ret = MUC_OutPkt(prDev, u4Pipe, pvBuf, i4Len);
                }
                
                if (i4Ret < 0)
                {
                    printf("data phase failed!\n");
                    return i4Ret;    
                }
            }

            // wait device prepare ok.
            mdelay(10);

            /* status phase */
            i4Ret = MUC_StatusPkt(prDev, u4Pipe);
            if (i4Ret < 0)
            {
                printf("status phase failed!\n");
                return i4Ret;    
            }
	} 
	else 
	{
            printf("setup phase failed!\n");
	}

        // wait device prepare ok.
        mdelay(10);

	prDev->act_len = i4Len;

	return i4Len;
}

//-------------------------------------------------------------------------
/** submit_int_msg
 *  submit interrupt transfer.
 *  @param  prDev: point to struct usb_device.
 *  @param  u4Pipe: usb u4Pipe.
 *  @param  puBuf: point to data buffer.
 *  @param  len: data buffer length.
 *  @param  interval: interrupt interval.
 *  @retval 0	Success
 *  @retval Others	Fail
 */
//-------------------------------------------------------------------------
int submit_int_msg(struct usb_device *prDev, unsigned long u4Pipe, void *puBuf,
		   int len, int interval)
{
	printf("prDev = %p u4Pipe = %#lx buf = %p size = %d int = %d\n", prDev, u4Pipe,
	       puBuf, len, interval);
	return -1;
}

#endif	/* CONFIG_USB_MTKHCD */
