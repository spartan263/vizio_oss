/*
 * linux/arch/arm/mach-mt53xx/localtimer.c
 *
 * Copyright (c) 2010-2012 MediaTek Inc.
 * LH Hsiao <lh.hsiao@mediatek.com>
 *
 * This file is heavily based on relaview platform, almost a copy.
 *
 * Copyright (C) 2002 ARM Ltd.
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

#include <linux/init.h>
#include <linux/smp.h>
#include <linux/clockchips.h>

#include <asm/irq.h>
#include <asm/smp_twd.h>
#include <asm/localtimer.h>
#include <mach/platform.h>
/*
 * Setup the local clock events for a CPU.
 */
int __cpuinit local_timer_setup(struct clock_event_device *evt)
{
	/* Setup the local timer base */
	twd_base = (void __iomem *)MPCORE_GIT_VIRT; 
	
	evt->irq = IRQ_LOCALTIMER;
	twd_timer_setup(evt);
	return 0;
}
