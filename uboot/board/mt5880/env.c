/*
 * vm_linux/chiling/uboot/board/mt5880/env.c
 *
 * Copyright (c) 2010-2012 MediaTek Inc.
 *	This file is based  ARM Realview platform
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
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

#include <common.h>
#include <exports.h>

#include "x_bim.h"
#include "drvcust_if.h"

UINT32 global_magic = 0x53965368;
UINT32 global_kernel_dram_size = PHYS_SDRAM_1_SIZE;

#if CONFIG_NR_DRAM_BANKS>1
UINT32 global_kernel_dram2_start = PHYS_SDRAM_2;
UINT32 global_kernel_dram2_size = PHYS_SDRAM_2_SIZE;
#endif
