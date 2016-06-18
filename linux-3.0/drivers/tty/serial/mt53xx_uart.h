/*
 * drivers/tty/serial/mt53xx_uart.h
 *
 * MT53xx UART driver
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
 */

#ifndef __DRV_SER_MT53xx_UART_H
#define __DRV_SER_MT53xx_UART_H

#include <mach/hardware.h>
#include <mach/irqs.h>

#define UART_PORT_0  0
#define UART_PORT_1  1
#define UART_PORT_2  2

/* uart general */
#define UART_FLASH_EN           0x008       // flash enable
    #define FLASH_EN_CMD            0x3c        // enable flash
    #define FLASH_DI_CMD            0x0         // disable flash
    #define FLASH_EN_BIT            0x1         // check bit

#define UART_INT_EN             0x00c       // interrupt enable
    #define U1_RERR     (1 <<  0)   // u1 rx error
                                    // parity, end, overrun and break
    #define U1_RBUF     (1 <<  1)   // u1 rx buffer full
    #define U1_TOUT     (1 <<  2)   // u1 rx time out
    #define U1_TBUF     (1 <<  3)   // u1 tx buffer empty
    #define U1_MODEM    (1 <<  4)   // u1 modem state change
    #define U1_OVRUN    (1 <<  5)   // u1 rx buffer overflow
    #define U1_WBUF_EMPTY    (1 <<  8)
    #define U1_WBUF_FULL     (1 <<  9)
    #define U1_WBUF_OVER     (1 <<  10)    
    #define U1_WBUF_TOUT     (1 <<  11)      
    #define U0_RERR     (1 << 16)   // u0 rx error
    #define U0_RBUF     (1 << 17)   // u0 rx buffer full
    #define U0_TOUT     (1 << 18)   // u0 rx time out
    #define U0_TBUF     (1 << 19)   // u0 tx buffer empty
    #define U0_MODEM    (1 << 20)   // u0 modem state change
    #define U0_OVRUN    (1 << 21)   // u0 rx buffer overflow
    #define U0_RISCD    (1 << 22)   // u0 debug port risc data in

#define UART_INT_ID             0x010       // interrupt identification
 
#define UART_RS232_MODE         0x018       // rs232 mode, 3 debug mode
    #define DBG_MODE_MSK            0x3
    #define DBG_RS232_ACTIVE        0x0         // host2flash access
    #define DBG_ADSP_ACTIVE         0x1         // host2dsp access
    #define DBG_RISC_ACTIVE         0x2         // host2risc access

/* uart debug port */
#define UART_FCONFIG            0x080       // flash configuration
    #define RFI_HANDSHAKE   (1 <<  0)
    #define RFI_N16BIT      (1 <<  1)
    #define RFI_CHKERR      (1 <<  2)
    #define RFI_ALWREQ      (1 <<  3)
    #define RFI_HOLDCMD     (1 <<  4)
    #define RFI_A19         (1 <<  6)

#define UART_FADDR              0x084       // flash address

#define UART_F4CYC              0x090       // flash 4 cycle command

#define UART_FDREG              0x094       // flash write data latch

#define UART_FERASE             0x098       // flash chip erase

#define UART_FDI                0x09c       // flash data in

#define UART_F1CYC              0x0a4       // flash 1 cycle command

#define UART_FDC                0x0ac       // flash direct control
    #define F_OEB       (1 <<  0)
    #define F_WEB       (1 <<  1)
    #define F_DE        (1 <<  2)
    #define F_CSB       (1 <<  3)
    #define F_CSMC      (1 <<  4)
    #define F_ADRLOW    (1 <<  5)
    #define F_ADRMID    (1 <<  6)
    #define F_ADRHIGH   (1 <<  7)

#define UART_FBANK              0x0b0       // flash bank
    #define BANK_0      0
    #define BANK_1      1
    #define BANK_2      2

/* uart 0 / debug port */
#define UART0_DATA              0x000           // u0 data
#define UART0_DATA_BYTE         (UART0_DATA)    // u0 byte / word data
#define UART0_DATA_WORD         (UART0_DATA)

#define UART0_STATUS_OLD        0x004           // old status
    #define U0_RD_ALLOW             (1 << 0)        // can read
    #define U0_WR_ALLOW             (1 << 1)        // can write
    #define U0_END_ERR              (1 << 4)        // rx error (end)
    #define U0_PARITY_ERR           (1 << 5)        // rx error (parity)
    #define U0_TRANSPARENT          (1 << 6)        // uart 0 transparent mode

#define UART0_BUFFER_SIZE       0x01c           // u0 buffer size selection

#define UART0_COMMCTRL          0x048           // u0 comm control
#define UART1_COMMCTRL          0x0c8           // u0 comm control
    /* databit with stopbit */
    #define UBIT_MASK       0x7
    #define UBIT_8_1        0x0     // 8 bit 1 stop
    #define UBIT_8_2        0x4     // 8 bit 2 stop
    #define UBIT_7_1        0x1     // 7 bit 1 stop
    #define UBIT_7_2        0x5     // 7 bit 2 stop
    #define UBIT_6_1        0x2     // 6 bit 1 stop
    #define UBIT_6_2        0x6     // 6 bit 2 stop
    #define UBIT_5_1        0x3     // 5 bit 1 stop
    #define UBIT_5_1_5      0x7     // 5 bit 1.5 stop
    /* data bit */
    #define DATA_BIT_MASK   0x3
    #define DATA_BIT_8      0
    #define DATA_BIT_7      1
    #define DATA_BIT_6      2
    #define DATA_BIT_5      3
    /* stop bit */
    #define STOP_BIT_MASK   (1 << 2)
    #define STOP_BIT_1      (0 << 2)
    #define STOP_BIT_2      (1 << 2)
    #define STOP_BIT_1_5    (1 << 2)
    /* baud */
    #define SETBAUD(x)      (((x) & 0xf) << 8)
    #define GETBAUD(x)      (((x) >> 8) & 0xf)
    #define BAUD_MASK       (0xf << 8)
    #define BAUD_X1         0
    #define BAUD_X2         1
    #define BAUD_X4         2
    #define BAUD_X8         3
    #define BAUD_57600      4
    #define BAUD_38400      5
    #define BAUD_19200      6
    #define BAUD_9600       7
    #define BAUD_4800       8
    #define BAUD_2400       9
    #define BAUD_1200       10
    #define BAUD_300        11
    #define BAUD_110        12
    #define BAUD_CUST       13        
    #define BAUD_115200     BAUD_X1
    #define BAUD_230400     BAUD_X2
    #define BAUD_460800     BAUD_X4
    #define BAUD_921600     BAUD_X8

    #define U2_BAUD_X1         0
    #define U2_BAUD_57600      1
    #define U2_BAUD_38400      2
    #define U2_BAUD_19200      3
    #define U2_BAUD_9600       4
    #define U2_BAUD_2400       5
    #define U2_BAUD_115200     U2_BAUD_X1
    #define SETCUSTBAUD(x)      (((x) & 0xffff) << 16)
    #define GETCUSTBAUD(x)      (((x) >> 16) & 0xffff)
    #define CUSTBAUD_MASK       (0xffff << 16)
    
    /* parity check */
    #define PARITY_MASK     (3 << 4)
    #define PARITY_NONE     (0 << 4)
    #define PARITY_ODD      (1 << 4)
    #define PARITY_EVEN     (3 << 4)
    #define IS_PARITY(REG,FLAG)     ((REG) & (FLAG) == (FLAG))
    /* break */
    #define CONTROL_BREAK   (1 << 7)

#define UART0_STATUS            0x04c           // u0 status
    #define UART_WRITE_ALLOW        0x1F00
    #define UART_READ_ALLOW         0x001F
    #define UART_FIFO_SIZE          31                               // UART real fifo size
    #define RX_BUF_SIZE             (UART_FIFO_SIZE)                 // Real use of rx fifo
    #define TX_BUF_SIZE             (UART_FIFO_SIZE)                 // Real use of tx fifo
    #define RX_BUF_MASK             (UART_FIFO_SIZE)
    #define TX_BUF_MASK             ((UART_FIFO_SIZE) << 8)
    #define RX_BUF_STATE(x)         ((x) & (UART_FIFO_SIZE))
    #define TX_BUF_STATE(x)         (((x) & (UART_FIFO_SIZE)) << 8)
    #define GET_TX_EMPTY_SIZE(x)    (((x) & TX_BUF_MASK) >> 8)
    #define GET_RX_DATA_SIZE(x)     ((x) & RX_BUF_MASK)
    #define ERR_PARITY              (1 <<  5)
    #define ERR_END                 (1 <<  6)
    #define ERR_BREAK               (1 <<  7)
    #define ERR4_PARITY             (1 << 13)
    #define ERR4_END                (1 << 14)
    #define ERR4_BREAK              (1 << 15)

#define UART0_BUFCTRL             0x050           // u0 buffer control
    #define RX_TRIG_LVL(x)          ((x) & 0x1f)
    #define RX_TOUT_CYCLE(x)        (((x) & 0xff) << 8)
    #define CLEAR_RBUF              (1 << 6)
    #define CLEAR_TBUF              (1 << 7)
    #define BUFCTRL_INIT            (CLEAR_TBUF | CLEAR_RBUF | RX_TOUT_CYCLE(0xf) | RX_TRIG_LVL(0x1a))

#define UART0_MODEM             0x054           // u0 modem status
    #define RTS_TRIG_LEVEL(x)       (((x) & 0xf) << 4)
    #define MDM_DTR         (1 <<  0)
    #define MDM_RTS         (1 <<  1)
    #define MDM_HW_RTS      (1 <<  2)
    #define MDM_HW_CTS      (1 <<  3)
    #define MDM_DSR         (1 <<  8)
    #define MDM_CTS         (1 <<  9)
    #define MDM_RI          (1 << 10)
    #define MDM_DCD         (1 << 11)
    #define MDM_VCHG_DSR    (1 << 12)
    #define MDM_VCHG_CTS    (1 << 13)
    #define MDM_VCHG_RI     (1 << 14)
    #define MDM_VCHG_DCD    (1 << 15)

/* uart 1 */
#define UART1_DATA_BYTE         0x0c0           // u1 byte data
#define UART1_DATA              (UART1_DATA_BYTE)
#define UART1_DATA_WORD         0x0c4           // u1 word data
#define UART1_STATUS            0x0cc           // u1 status
#define UART1_BUFCTRL           0x0d0           // buffer control
#define UART1_MODEM             0x0d4           // modem status
#define MODEM_DTR                   (1 << 0)
#define MODEM_RTS                   (1 << 1)
#define MODEM_HW_RTS                (1 << 2)
#define MODEM_HW_CTS                (1 << 3)
#define MODEM_RTS_TRIGGER_MASK      (0xf << 4)
#define MODEM_RTS_TRIGGER_VALUE     (0xf << 4)
#define UART1_DMAW_CTRL         0x140           // dma control
#define UART1_DMAW_EN            0x140
#define DMAW_CIR                 (0x1<<1)
#define DMAW_EN                  (0x1<<0)
#define UART1_DMAW_RST           0x144
#define UART1_DMAW_LEVEL         0x148
#define UART1_DMAW_SPTR          0x150
#define UART1_DMAW_EPTR          0x154
#define UART1_DMAW_WPTR          0x158
#define UART1_DMAW_RPTR          0x15c

#define UART1_DMAR_EN            0x160
#define DMAR_CIR                 (0x1<<1)
#define DMAR_EN                  (0x1<<0)
#define UART1_DMAR_RST           0x164
#define UART1_DMAR_LEVEL         0x168
#define UART1_DMAR_SPTR          0x170
#define UART1_DMAR_EPTR          0x174
#define UART1_DMAR_WPTR          0x178
#define UART1_DMAR_RPTR          0x17c

/* uart 2 */
#define UART2_BASE              (0x800)

#define UART2_DATA              (UART2_BASE + 0x000)
#define UART2_DATA_BYTE         (UART2_DATA + 0x000)

#define UART2_STATUS_OLD        (UART2_BASE + 0x004)
    #define U2_TRANSPARENT (1 << 6)

#define UART2_INT_EN            (UART2_BASE + 0x00C)
    #define U2_T8032D      (1 << 7)  // normal mode t8032 data
    #define U2_RISCD       (1 << 6)  // normal mode risc data
    #define U2_OVRUN       (1 << 5)  // rxd buffer overflow
    #define U2_MODEM       (1 << 4)  // modem signal change
    #define U2_TBUF        (1 << 3)  // txd buffer empty
    #define U2_TOUT        (1 << 2)  // rxd timeout
    #define U2_RBUF        (1 << 1)  // rxd data available
    #define U2_RERR        (1 << 0)  // rxd error

    #define U0_INTMSK      (0x007F0000)
    #define U1_INTMSK      (0x00000F3F)
    #define U2_INTMSK      (U2_T8032D | U2_RISCD | U2_OVRUN | U2_MODEM | U2_TBUF | U2_TOUT | U2_RBUF | U2_RERR)
    #define U0_INTALL      (U0_TOUT | U0_RBUF | U0_RERR | U0_RISCD)
    #define U1_INTALL      (U1_TOUT | U1_RBUF | U1_RERR)
    #define U2_INTALL      (U2_TOUT | U2_RBUF | U2_RERR)

#define UART2_INT_ID            (UART2_BASE + 0x010)

#define UART2_RS232_MODE        (UART2_BASE + 0x018)

#define UART2_BUFFER_SIZE       (UART2_BASE + 0x01c)

#define UART2_COMMCTRL          (UART2_BASE + 0x030)

#define UART2_STATUS            (UART2_BASE + 0x034)

#define UART2_BUFCTRL           (UART2_BASE + 0x038)


/* others */
    /* system level, not related to hardware */
    #define UST_FRAME_ERROR     (1 << 0)
    #define UST_PARITY_ERROR    (1 << 1)
    #define UST_BREAK_ERROR     (1 << 2)
    #define UST_OVRUN_ERROR     (1 << 3)
    #define UST_DUMMY_READ      (1 << 31)

#define PORT_MT53xx         5391


/* x_bim.h */
    #define VECTOR_RS232_0  VECTOR_RS232          // RS232 Interrupt 0
    #define VECTOR_RS232_1  VECTOR_RS232          // RS232 Interrupt 1
    #define VECTOR_RS232_1_AB  VECTOR_UART_DMA 
    #define VECTOR_RS232_2  VECTOR_PDWNC          // RS232 Interrupt 2

#endif /* __DRV_SER_MT53xx_UART_H */
