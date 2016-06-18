/*
 * linux/arch/arm/mach-mt53xx/include/mach/mt53xx_linux.h
 *
 * Copyright (c) 2010-2012 MediaTek Inc.
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

#ifndef __MT53XX_LINUX_H__
#define __MT53XX_LINUX_H__


extern spinlock_t mt53xx_bim_lock;

/// Size of channel A DRAM size.
extern unsigned int mt53xx_cha_mem_size;

void mt53xx_get_reserved_area(unsigned long *start, unsigned long *size);


/* MT53xx family IC Check */
extern unsigned int mt53xx_get_ic_version(void);

#define IS_IC_MT5396()        ((mt53xx_get_ic_version() >> 16)==0x5396U)
#define IS_IC_MT5398()        ((mt53xx_get_ic_version() >> 16)==0x5398U)
#define IS_IC_MT5880()        ((mt53xx_get_ic_version() >> 16)==0x5880U)
#define IS_IC_MT5860()        ((mt53xx_get_ic_version() >> 16)==0x5860U)

#define IC_VER_5398_AA        0x53980000         // 5398 AA
#define IC_VER_5398_AB        0x53980001         // 5398 AB
#define IC_VER_5398_AC        0x53980002         // 5398 AC

#define IC_VER_5860_AA        0x58600000         // 5880 AA
#define IC_VER_5860_AB        0x58600001         // 5880 AB
#define IC_VER_5860_AC        0x58600002         // 5880 AC

#define IS_IC_MT5860_E2IC()   ((mt53xx_get_ic_version())==IC_VER_5860_AB)

#endif /* __MT53XX_LINUX_H__ */

