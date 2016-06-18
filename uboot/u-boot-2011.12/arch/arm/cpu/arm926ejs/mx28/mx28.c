/*
 * Freescale i.MX28 common code
 *
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
 *
 * Based on code from LTIB:
 * Copyright (C) 2010 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/iomux.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/sys_proto.h>

DECLARE_GLOBAL_DATA_PTR;

/* 1 second delay should be plenty of time for block reset. */
#define	RESET_MAX_TIMEOUT	1000000

#define	MX28_BLOCK_SFTRST	(1 << 31)
#define	MX28_BLOCK_CLKGATE	(1 << 30)

/* Lowlevel init isn't used on i.MX28, so just have a dummy here */
inline void lowlevel_init(void) {}

void reset_cpu(ulong ignored) __attribute__((noreturn));

void reset_cpu(ulong ignored)
{

	struct mx28_rtc_regs *rtc_regs =
		(struct mx28_rtc_regs *)MXS_RTC_BASE;

	/* Wait 1 uS before doing the actual watchdog reset */
	writel(1, &rtc_regs->hw_rtc_watchdog);
	writel(RTC_CTRL_WATCHDOGEN, &rtc_regs->hw_rtc_ctrl_set);

	/* Endless loop, reset will exit from here */
	for (;;)
		;
}

int mx28_wait_mask_set(struct mx28_register *reg, uint32_t mask, int timeout)
{
	while (--timeout) {
		if ((readl(&reg->reg) & mask) == mask)
			break;
		udelay(1);
	}

	return !timeout;
}

int mx28_wait_mask_clr(struct mx28_register *reg, uint32_t mask, int timeout)
{
	while (--timeout) {
		if ((readl(&reg->reg) & mask) == 0)
			break;
		udelay(1);
	}

	return !timeout;
}

int mx28_reset_block(struct mx28_register *reg)
{
	/* Clear SFTRST */
	writel(MX28_BLOCK_SFTRST, &reg->reg_clr);

	if (mx28_wait_mask_clr(reg, MX28_BLOCK_SFTRST, RESET_MAX_TIMEOUT))
		return 1;

	/* Clear CLKGATE */
	writel(MX28_BLOCK_CLKGATE, &reg->reg_clr);

	/* Set SFTRST */
	writel(MX28_BLOCK_SFTRST, &reg->reg_set);

	/* Wait for CLKGATE being set */
	if (mx28_wait_mask_set(reg, MX28_BLOCK_CLKGATE, RESET_MAX_TIMEOUT))
		return 1;

	/* Clear SFTRST */
	writel(MX28_BLOCK_SFTRST, &reg->reg_clr);

	if (mx28_wait_mask_clr(reg, MX28_BLOCK_SFTRST, RESET_MAX_TIMEOUT))
		return 1;

	/* Clear CLKGATE */
	writel(MX28_BLOCK_CLKGATE, &reg->reg_clr);

	if (mx28_wait_mask_clr(reg, MX28_BLOCK_CLKGATE, RESET_MAX_TIMEOUT))
		return 1;

	return 0;
}

void mx28_fixup_vt(uint32_t start_addr)
{
	uint32_t *vt = (uint32_t *)0x20;
	int i;

	for (i = 0; i < 8; i++)
		vt[i] = start_addr + (4 * i);
}

#ifdef	CONFIG_ARCH_MISC_INIT
int arch_misc_init(void)
{
	mx28_fixup_vt(gd->relocaddr);
	return 0;
}
#endif

#ifdef	CONFIG_ARCH_CPU_INIT
int arch_cpu_init(void)
{
	struct mx28_clkctrl_regs *clkctrl_regs =
		(struct mx28_clkctrl_regs *)MXS_CLKCTRL_BASE;
	extern uint32_t _start;

	mx28_fixup_vt((uint32_t)&_start);

	/*
	 * Enable NAND clock
	 */
	/* Clear bypass bit */
	writel(CLKCTRL_CLKSEQ_BYPASS_GPMI,
		&clkctrl_regs->hw_clkctrl_clkseq_set);

	/* Set GPMI clock to ref_gpmi / 12 */
	clrsetbits_le32(&clkctrl_regs->hw_clkctrl_gpmi,
		CLKCTRL_GPMI_CLKGATE | CLKCTRL_GPMI_DIV_MASK, 1);

	udelay(1000);

	/*
	 * Configure GPIO unit
	 */
	mxs_gpio_init();

	return 0;
}
#endif

#if defined(CONFIG_DISPLAY_CPUINFO)
int print_cpuinfo(void)
{
	printf("Freescale i.MX28 family\n");
	return 0;
}
#endif

int do_mx28_showclocks(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("CPU:   %3d MHz\n", mxc_get_clock(MXC_ARM_CLK) / 1000000);
	printf("BUS:   %3d MHz\n", mxc_get_clock(MXC_AHB_CLK) / 1000000);
	printf("EMI:   %3d MHz\n", mxc_get_clock(MXC_EMI_CLK));
	printf("GPMI:  %3d MHz\n", mxc_get_clock(MXC_GPMI_CLK) / 1000000);
	return 0;
}

/*
 * Initializes on-chip ethernet controllers.
 */
#ifdef	CONFIG_CMD_NET
int cpu_eth_init(bd_t *bis)
{
	struct mx28_clkctrl_regs *clkctrl_regs =
		(struct mx28_clkctrl_regs *)MXS_CLKCTRL_BASE;

	/* Turn on ENET clocks */
	clrbits_le32(&clkctrl_regs->hw_clkctrl_enet,
		CLKCTRL_ENET_SLEEP | CLKCTRL_ENET_DISABLE);

	/* Set up ENET PLL for 50 MHz */
	/* Power on ENET PLL */
	writel(CLKCTRL_PLL2CTRL0_POWER,
		&clkctrl_regs->hw_clkctrl_pll2ctrl0_set);

	udelay(10);

	/* Gate on ENET PLL */
	writel(CLKCTRL_PLL2CTRL0_CLKGATE,
		&clkctrl_regs->hw_clkctrl_pll2ctrl0_clr);

	/* Enable pad output */
	setbits_le32(&clkctrl_regs->hw_clkctrl_enet, CLKCTRL_ENET_CLK_OUT_EN);

	return 0;
}
#endif

U_BOOT_CMD(
	clocks,	CONFIG_SYS_MAXARGS, 1, do_mx28_showclocks,
	"display clocks",
	""
);
