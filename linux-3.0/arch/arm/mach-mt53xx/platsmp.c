/*
 * linux/arch/arm/mach-mt53xx/platsmp.c
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

#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/smp.h>
#include <linux/io.h>

#include <asm/cacheflush.h>
#include <asm/localtimer.h>
#include <asm/smp_scu.h>
#include <asm/unified.h>
#include <asm/hardware/gic.h>

#include <mach/hardware.h>


extern void mt53xx_secondary_startup(void);
extern void mt53xx_secondary_sleep(void);

static inline void __iomem *scu_base_addr(void)
{
	return (void __iomem *)MPCORE_SCU_VIRT;
}

#ifdef CONFIG_SMP
/*
 * control for which core is the next to come out of the secondary
 * boot "holding pen"
 */
volatile int __cpuinitdata pen_release = -1;

static DEFINE_SPINLOCK(boot_lock);

void __cpuinit platform_secondary_init(unsigned int cpu)
{
	trace_hardirqs_off();

	/*
	 * if any interrupts are already enabled for the primary
	 * core (e.g. timer irq), then they will not have been enabled
	 * for us: do so
	 */
	gic_secondary_init(0);

	/*
	 * let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	pen_release = -1;
	smp_wmb();

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

int __cpuinit boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	/*
	 * Set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	/*
	 * This is really belt and braces; we hold unintended secondary
	 * CPUs in the holding pen until we're ready for them.  However,
	 * since we haven't sent them a soft interrupt, they shouldn't
	 * be there.
	 */
	pen_release = cpu;
	__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
	outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));

	/*
	 * Send the secondary CPU a soft interrupt, thereby causing
	 * the boot monitor to read the system wide flags register,
	 * and branch to the address found there.
	 */
	arm_send_ping_ipi(cpu);

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (pen_release == -1)
			break;

		udelay(10);
	}

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	return pen_release != -1 ? -ENOSYS : 0;
}

/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
void __init smp_init_cpus(void)
{
	void __iomem *scu_base = scu_base_addr();
	unsigned int i, ncores;

	ncores = scu_base ? scu_get_core_count(scu_base) : 1;

	/* sanity check */
	if (ncores == 0) {
		printk(KERN_ERR
		       "mt53xx: strange CM count of 0? Default to 1\n");

		ncores = 1;
	}

	if (ncores > NR_CPUS) {
		printk(KERN_WARNING
		       "mt53xx: no. of cores (%d) greater than configured "
		       "maximum of %d - clipping\n",
		       ncores, NR_CPUS);
		ncores = NR_CPUS;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);

	set_smp_cross_call(gic_raise_softirq);
}

static void __cpuinit platform_smp_prepare_cpus_wakeup(void)
{
	scu_enable(scu_base_addr());

	/*
	 * Write the address of secondary startup into the
	 * system-wide flags register. The boot monitor waits
	 * until it receives a soft interrupt, and then the
	 * secondary CPU branches to this address.
	 */

	/* use timer1 as global reg
		HLMT: physical addr of entry
		LLMT: magic (0xffffb007)
	 */

	__raw_writel(virt_to_phys(mt53xx_secondary_startup),
		 BIM_VIRT+REG_RW_TIMER1_HLMT);
	__raw_writel(0xffffb007,
		 BIM_VIRT+REG_RW_TIMER1_LLMT);

	/* make sure write buffer is drained */
	mb();

	/* Wakeup sleepers */
	asm volatile ("sev");
}

void __init platform_smp_prepare_cpus(unsigned int max_cpus)
{
	int i;

	/*
	 * Initialise the present map, which describes the set of CPUs
	 * actually populated at the present time.
	 */
	for (i = 0; i < max_cpus; i++)
		set_cpu_present(i, true);

	if (max_cpus == 1 && num_possible_cpus() > 1)
	{
		// All others, go to sleep.
		__raw_writel(virt_to_phys(mt53xx_secondary_sleep),
			 BIM_VIRT+REG_RW_TIMER1_HLMT);
		__raw_writel(0xffffb007,
			 BIM_VIRT+REG_RW_TIMER1_LLMT);

		/* make sure write buffer is drained */
		mb();

		/* Wakeup sleepers */
		asm volatile ("sev");
		return;
	}

	platform_smp_prepare_cpus_wakeup();
}

#ifdef CONFIG_HIBERNATION
void arch_enable_nonboot_cpus_begin(void)
{
	platform_smp_prepare_cpus_wakeup();
}
#endif /* CONFIG_HIBERNATION */
#else
#define SCU_CONFIG		0x04

static int __init mt53xx_up_check(void)
{
	unsigned int ncores = __raw_readl(scu_base_addr() + SCU_CONFIG);
	ncores = (ncores & 0x03) + 1;

	if (ncores > 1)
	{
		// If there's more then 1 cores, ask all others to go to sleep.
		__raw_writel(virt_to_phys(mt53xx_secondary_sleep),
			 BIM_VIRT+REG_RW_TIMER1_HLMT);
		__raw_writel(0xffffb007,
			 BIM_VIRT+REG_RW_TIMER1_LLMT);

		/* make sure write buffer is drained */
		mb();

		/* Wakeup sleepers */
		asm volatile ("sev");
	}

	return 0;
}
early_initcall(mt53xx_up_check);
#endif /* CONFIG_SMP */
