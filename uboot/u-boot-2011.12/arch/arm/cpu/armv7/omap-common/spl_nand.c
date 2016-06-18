/*
 * Copyright (C) 2011
 * Corscience GmbH & Co. KG - Simon Schwarz <schwarz@corscience.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/u-boot.h>
#include <asm/utils.h>
#include <asm/arch/sys_proto.h>
#include <nand.h>
#include <version.h>
#include <asm/omap_common.h>


void spl_nand_load_image(void)
{
	struct image_header *header;
	switch (omap_boot_mode()) {
	case NAND_MODE_HW_ECC:
		debug("spl: nand - using hw ecc\n");
		gpmc_init();
		nand_init();
		break;
	default:
		puts("spl: ERROR: This bootmode is not implemented - hanging");
		hang();
	}

	/*use CONFIG_SYS_TEXT_BASE as temporary storage area */
	header = (struct image_header *)(CONFIG_SYS_TEXT_BASE);

#ifdef CONFIG_NAND_ENV_DST
	nand_spl_load_image(CONFIG_ENV_OFFSET,
		CONFIG_SYS_NAND_PAGE_SIZE, (void *)header);
	spl_parse_image_header(header);
	nand_spl_load_image(CONFIG_ENV_OFFSET, spl_image.size,
		(void *)image_load_addr);
#ifdef CONFIG_ENV_OFFSET_REDUND
	nand_spl_load_image(CONFIG_ENV_OFFSET_REDUND,
		CONFIG_SYS_NAND_PAGE_SIZE, (void *)header);
	spl_parse_image_header(header);
	nand_spl_load_image(CONFIG_ENV_OFFSET_REDUND, spl_image.size,
		(void *)image_load_addr);
#endif
#endif
	/* Load u-boot */
	nand_spl_load_image(CONFIG_SYS_NAND_U_BOOT_OFFS,
		CONFIG_SYS_NAND_PAGE_SIZE, (void *)header);
	spl_parse_image_header(header);
	nand_spl_load_image(CONFIG_SYS_NAND_U_BOOT_OFFS,
		spl_image.size, (void *)spl_image.load_addr);
	nand_deselect();
}
