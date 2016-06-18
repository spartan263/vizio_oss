/*
 * cpu.h
 *
 * AM33xx specific header file
 *
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _AM33XX_CPU_H
#define _AM33XX_CPU_H

#if !(defined(__KERNEL_STRICT_NAMES) || defined(__ASSEMBLY__))
#include <asm/types.h>
#endif /* !(__KERNEL_STRICT_NAMES || __ASSEMBLY__) */

#include <asm/arch/hardware.h>

#define BIT(x)				(1 << x)
#define CL_BIT(x)			(0 << x)

/* Timer register bits */
#define TCLR_ST				BIT(0)	/* Start=1 Stop=0 */
#define TCLR_AR				BIT(1)	/* Auto reload */
#define TCLR_PRE			BIT(5)	/* Pre-scaler enable */
#define TCLR_PTV_SHIFT			(2)	/* Pre-scaler shift value */
#define TCLR_PRE_DISABLE		CL_BIT(5) /* Pre-scalar disable */

/* device type */
#define DEVICE_MASK			(BIT(8) | BIT(9) | BIT(10))
#define TST_DEVICE			0x0
#define EMU_DEVICE			0x1
#define HS_DEVICE			0x2
#define GP_DEVICE			0x3

/* cpu-id for AM33XX family */
#define AM335X				0xB944
#define DEVICE_ID			0x44E10600

/* This gives the status of the boot mode pins on the evm */
#define SYSBOOT_MASK			(BIT(0) | BIT(1) | BIT(2)\
					| BIT(3) | BIT(4))

/* Reset control */
#ifdef CONFIG_AM335X
#define PRM_RSTCTRL			0x44E00F00
#endif
#define PRM_RSTCTRL_RESET		0x01

#ifndef __KERNEL_STRICT_NAMES
#ifndef __ASSEMBLY__
/* Encapsulating core pll registers */
struct cm_wkuppll {
	unsigned int wkclkstctrl;	/* offset 0x00 */
	unsigned int wkctrlclkctrl;	/* offset 0x04 */
	unsigned int resv1[1];
	unsigned int wkl4wkclkctrl;	/* offset 0x0c */
	unsigned int resv2[4];
	unsigned int idlestdpllmpu;	/* offset 0x20 */
	unsigned int resv3[2];
	unsigned int clkseldpllmpu;	/* offset 0x2c */
	unsigned int resv4[1];
	unsigned int idlestdpllddr;	/* offset 0x34 */
	unsigned int resv5[2];
	unsigned int clkseldpllddr;	/* offset 0x40 */
	unsigned int resv6[4];
	unsigned int clkseldplldisp;	/* offset 0x54 */
	unsigned int resv7[1];
	unsigned int idlestdpllcore;	/* offset 0x5c */
	unsigned int resv8[2];
	unsigned int clkseldpllcore;	/* offset 0x68 */
	unsigned int resv9[1];
	unsigned int idlestdpllper;	/* offset 0x70 */
	unsigned int resv10[3];
	unsigned int divm4dpllcore;	/* offset 0x80 */
	unsigned int divm5dpllcore;	/* offset 0x84 */
	unsigned int clkmoddpllmpu;	/* offset 0x88 */
	unsigned int clkmoddpllper;	/* offset 0x8c */
	unsigned int clkmoddpllcore;	/* offset 0x90 */
	unsigned int clkmoddpllddr;	/* offset 0x94 */
	unsigned int clkmoddplldisp;	/* offset 0x98 */
	unsigned int clkseldpllper;	/* offset 0x9c */
	unsigned int divm2dpllddr;	/* offset 0xA0 */
	unsigned int divm2dplldisp;	/* offset 0xA4 */
	unsigned int divm2dpllmpu;	/* offset 0xA8 */
	unsigned int divm2dpllper;	/* offset 0xAC */
	unsigned int resv11[1];
	unsigned int wkup_uart0ctrl;	/* offset 0xB4 */
	unsigned int resv12[8];
	unsigned int divm6dpllcore;	/* offset 0xD8 */
};

/**
 * Encapsulating peripheral functional clocks
 * pll registers
 */
struct cm_perpll {
	unsigned int l4lsclkstctrl;	/* offset 0x00 */
	unsigned int l3sclkstctrl;	/* offset 0x04 */
	unsigned int l4fwclkstctrl;	/* offset 0x08 */
	unsigned int l3clkstctrl;	/* offset 0x0c */
	unsigned int resv1[6];
	unsigned int emifclkctrl;	/* offset 0x28 */
	unsigned int ocmcramclkctrl;	/* offset 0x2c */
	unsigned int resv2[12];
	unsigned int l4lsclkctrl;	/* offset 0x60 */
	unsigned int l4fwclkctrl;	/* offset 0x64 */
	unsigned int resv3[6];
	unsigned int timer2clkctrl;	/* offset 0x80 */
	unsigned int resv4[19];
	unsigned int emiffwclkctrl;	/* offset 0xD0 */
	unsigned int resv5[2];
	unsigned int l3instrclkctrl;	/* offset 0xDC */
	unsigned int l3clkctrl;		/* Offset 0xE0 */
	unsigned int resv6[14];
	unsigned int l4hsclkstctrl;	/* offset 0x11C */
	unsigned int l4hsclkctrl;	/* offset 0x120 */
};

/* Encapsulating Display pll registers */
struct cm_dpll {
	unsigned int resv1[2];
	unsigned int clktimer2clk;	/* offset 0x08 */
};

/* Watchdog timer registers */
struct wd_timer {
	unsigned int resv1[4];
	unsigned int wdtwdsc;	/* offset 0x010 */
	unsigned int wdtwdst;	/* offset 0x014 */
	unsigned int wdtwisr;	/* offset 0x018 */
	unsigned int wdtwier;	/* offset 0x01C */
	unsigned int wdtwwer;	/* offset 0x020 */
	unsigned int wdtwclr;	/* offset 0x024 */
	unsigned int wdtwcrr;	/* offset 0x028 */
	unsigned int wdtwldr;	/* offset 0x02C */
	unsigned int wdtwtgr;	/* offset 0x030 */
	unsigned int wdtwwps;	/* offset 0x034 */
	unsigned int resv2[3];
	unsigned int wdtwdly;	/* offset 0x044 */
	unsigned int wdtwspr;	/* offset 0x048 */
	unsigned int resv3[1];
	unsigned int wdtwqeoi;	/* offset 0x050 */
	unsigned int wdtwqstar;	/* offset 0x054 */
	unsigned int wdtwqsta;	/* offset 0x058 */
	unsigned int wdtwqens;	/* offset 0x05C */
	unsigned int wdtwqenc;	/* offset 0x060 */
	unsigned int resv4[39];
	unsigned int wdt_unfr;	/* offset 0x100 */
};

/* Timer Registers */
struct timer_reg {
	unsigned int resv1[4];
	unsigned int tiocpcfgreg;	/* offset 0x10 */
	unsigned int resv2[9];
	unsigned int tclrreg;		/* offset 0x38 */
	unsigned int tcrrreg;		/* offset 0x3C */
	unsigned int tldrreg;		/* offset 0x40 */
	unsigned int resv3[4];
	unsigned int tsicrreg;		/* offset 0x54 */
};

/* Timer 32 bit registers */
struct gptimer {
	unsigned int tidr;		/* offset 0x00 */
	unsigned int res1[0xc];
	unsigned int tiocp_cfg;		/* offset 0x10 */
	unsigned int res2[0xc];
	unsigned int tier;		/* offset 0x20 */
	unsigned int tistatr;		/* offset 0x24 */
	unsigned int tistat;		/* offset 0x28 */
	unsigned int tisr;		/* offset 0x2c */
	unsigned int tcicr;		/* offset 0x30 */
	unsigned int twer;		/* offset 0x34 */
	unsigned int tclr;		/* offset 0x38 */
	unsigned int tcrr;		/* offset 0x3c */
	unsigned int tldr;		/* offset 0x40 */
	unsigned int ttgr;		/* offset 0x44 */
	unsigned int twpc;		/* offset 0x48 */
	unsigned int tmar;		/* offset 0x4c */
	unsigned int tcar1;		/* offset 0x50 */
	unsigned int tscir;		/* offset 0x54 */
	unsigned int tcar2;		/* offset 0x58 */
};

/* UART Registers */
struct uart_sys {
	unsigned int resv1[21];
	unsigned int uartsyscfg;	/* offset 0x54 */
	unsigned int uartsyssts;	/* offset 0x58 */
};

/* VTP Registers */
struct vtp_reg {
	unsigned int vtp0ctrlreg;
};

/* Control Status Register */
struct ctrl_stat {
	unsigned int resv1[16];
	unsigned int statusreg;		/* ofset 0x40 */
};

void init_timer(void);
#endif /* __ASSEMBLY__ */
#endif /* __KERNEL_STRICT_NAMES */

#endif /* _AM33XX_CPU_H */
