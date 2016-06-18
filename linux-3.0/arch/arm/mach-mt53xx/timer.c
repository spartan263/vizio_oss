/*
 * linux/arch/arm/mach-mt53xx/timer.c
 *
 * system timer init.
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

/* system header files */
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>

#include <asm/mach/time.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>

extern spinlock_t mt53xx_bim_lock;

static inline u32 __bim_readl(u32 regaddr32)
{
    return __raw_readl(BIM_VIRT + regaddr32);
}

static inline void __bim_writel(u32 regval32, u32 regaddr32)
{
    __raw_writel(regval32, BIM_VIRT + regaddr32);
}


#define TICKS_PER_SECOND HZ
#define BIM_BASE					(IO_VIRT + 0x08000)
#define CKGEN_BASE					(IO_VIRT + 0x0d000)
#define HAL_READ32(_reg_)           (*((volatile unsigned long*)(_reg_)))
#define IO_READ32(base, offset)     HAL_READ32((base) + (offset))
#define BIM_REG32(offset)           IO_REG32(BIM_BASE, offset)
#define BIM_READ32(off)             IO_READ32(BIM_BASE, (off))
#define CKGEN_READ32(offset)        IO_READ32(CKGEN_BASE, (offset))
#define BIM_WRITE32(off, val)       IO_WRITE32(BIM_BASE, (off), (val))

#define REG_IRQCL                   0x0038      // RISC IRQ Clear Register
#define TIMER_ID                    2
#define TIMER_VECTOR                (VECTOR_T0 + TIMER_ID)
#define TIMER_IRQ                   (1 << TIMER_VECTOR)
#define TIMER_LOW_REG               (REG_RW_TIMER0_LOW + (TIMER_ID * 8))
#define TIMER_HIGH_REG              (REG_RW_TIMER0_HIGH + (TIMER_ID * 8))
#define TIMER_LIMIT_LOW_REG         (REG_RW_TIMER0_LLMT + (TIMER_ID * 8))
#define TIMER_LIMIT_HIGH_REG        (REG_RW_TIMER0_HLMT + (TIMER_ID * 8))
#define TCTL_TIMER_EN               (TMR_CNTDWN_EN(TIMER_ID))
#define TCTL_TIMER_AL               (TMR_AUTOLD_EN(TIMER_ID))
#define MAX_TIMER_INTERVAL          (0xffffffff)

// XTAL config
#define CLK_24MHZ               24000000    // 24 MHz
#define CLK_27MHZ               27000000    // 27 MHz
#define CLK_48MHZ               48000000    // 48 MHz
#define CLK_54MHZ               54000000    // 54 MHz
#define CLK_60MHZ               60000000    // 60 MHz

#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368) || defined(CONFIG_ARCH_MT5389)
#define XTAL_STRAP_MODE         (CKGEN_READ32(0) & 0x6000)
#define XTAL_STRAP_MODE_SHIFT   13
#define SET_XTAL_27MHZ          (0U << XTAL_STRAP_MODE_SHIFT)
#define SET_XTAL_27MHZ_A        (3U << XTAL_STRAP_MODE_SHIFT)     // 27Mhz + MT8225 mode

#define IS_XTAL_54MHZ()         0
#define IS_XTAL_60MHZ()         0
#define IS_XTAL_27MHZ()         ((XTAL_STRAP_MODE == SET_XTAL_27MHZ) ||(XTAL_STRAP_MODE == SET_XTAL_27MHZ_A))
#define IS_XTAL_24MHZ()         0
#endif /* 96 gen */

#if defined(CONFIG_ARCH_MT5398)
#define XTAL_STRAP_MODE         (CKGEN_READ32(0) & 0x2000)
#define XTAL_STRAP_MODE_SHIFT   10
#define SET_XTAL_24MHZ          (0U << XTAL_STRAP_MODE_SHIFT)
#define SET_XTAL_27MHZ          (8U << XTAL_STRAP_MODE_SHIFT)

#define IS_XTAL_54MHZ()         0
#define IS_XTAL_60MHZ()         0
#define IS_XTAL_27MHZ()         (XTAL_STRAP_MODE == SET_XTAL_27MHZ)
#define IS_XTAL_24MHZ()         (XTAL_STRAP_MODE == SET_XTAL_24MHZ)
#endif /* Cobra */

#if defined(CONFIG_ARCH_MT5880) || defined(CONFIG_ARCH_MT5881)
#define XTAL_STRAP_MODE         (CKGEN_READ32(0) & 0x1000)
#define XTAL_STRAP_MODE_SHIFT   10
#define SET_XTAL_24MHZ          (0U << XTAL_STRAP_MODE_SHIFT)
#define SET_XTAL_27MHZ          (4U << XTAL_STRAP_MODE_SHIFT)

#define IS_XTAL_54MHZ()         0
#define IS_XTAL_60MHZ()         0
#define IS_XTAL_27MHZ()         (XTAL_STRAP_MODE == SET_XTAL_27MHZ)
#define IS_XTAL_24MHZ()         (XTAL_STRAP_MODE == SET_XTAL_24MHZ)
#endif /* Python/Viper */

#define GET_XTAL_CLK()          (IS_XTAL_54MHZ() ? (CLK_54MHZ) :    \
                                (IS_XTAL_60MHZ() ? (CLK_60MHZ) :    \
                                (IS_XTAL_24MHZ() ? (CLK_24MHZ) :    \
                                (CLK_27MHZ) )))


/*======================================================================
 * timer interrupt
 */
static unsigned long _timer_tick_count;

u32 _CKGEN_GetXtalClock(void)
{
#ifdef CONFIG_MT53_FPGA
    return CLK_27MHZ;
#else
    return GET_XTAL_CLK();
#endif /* CONFIG_FPGA */
}

static irqreturn_t
mt53xx_timer_interrupt(int irq, void *dev_id)
{
#ifdef CONFIG_GENERIC_CLOCKEVENTS
	struct clock_event_device *evt = dev_id;
	evt->event_handler(evt);
#else
    timer_tick();
#endif
    return IRQ_HANDLED;
}


/* Valid TIMER_ID: 0, 1, 2 */
#define SYS_TIMER_ID        0
#define HP_TIMER_ID         2

#define SYS_TOFST           (SYS_TIMER_ID * 8)
#define HP_TOFST            (HP_TIMER_ID * 8)

#define SYS_TIMER_IRQ       (IRQ_TIMER0 +  SYS_TIMER_ID)
#define SYS_TIMER_REG_L     (REG_RW_TIMER0_LOW + SYS_TOFST)
#define SYS_TIMER_REG_H     (REG_RW_TIMER0_HIGH + SYS_TOFST)
#define SYS_TIMER_LIMIT_REG_L (REG_RW_TIMER0_LLMT + SYS_TOFST)
#define SYS_TIMER_LIMIT_REG_H (REG_RW_TIMER0_HLMT + SYS_TOFST)
#define SYS_TIMER_EN        (TCTL_T0EN << SYS_TOFST)
#define SYS_TIMER_AL        (TCTL_T0AL << SYS_TOFST)

#define HP_TIMER_IRQ       (IRQ_TIMER0 +  HP_TIMER_ID)
#define HP_TIMER_REG_L     (REG_RW_TIMER0_LOW + HP_TOFST)
#define HP_TIMER_REG_H     (REG_RW_TIMER0_HIGH + HP_TOFST)
#define HP_TIMER_LIMIT_REG_L (REG_RW_TIMER0_LLMT + HP_TOFST)
#define HP_TIMER_LIMIT_REG_H (REG_RW_TIMER0_HLMT + HP_TOFST)
#define HP_TIMER_EN        (TCTL_T0EN << HP_TOFST)
#define HP_TIMER_AL        (TCTL_T0AL << HP_TOFST)

/* bus ticks at 135MHz */
#define SYSTEM_BUS_CLOCK            135000000UL

/* we tick kernel at HZ */
#define SYS_TIMER_INTERVAL          (SYSTEM_BUS_CLOCK / HZ)

#ifdef CONFIG_GENERIC_CLOCKEVENTS
/*static inline s64 clocksource_cyc2ns(cycle_t cycles, u32 mult, u32 shift)
{
	return ((u64) cycles * mult) >> shift;
}*/

static cycle_t clksrc_read(struct clocksource *cs)
{
    unsigned long time_lo1;
    unsigned long time_hi1, time_hi2;

    do
    {
        time_hi1 = __bim_readl(HP_TIMER_REG_H);
        time_lo1 = __bim_readl(HP_TIMER_REG_L);
        time_hi2 = __bim_readl(HP_TIMER_REG_H);
    } while (time_hi1 != time_hi2);

    return ~(((unsigned long long)time_hi1 << 32) + (unsigned long long)time_lo1);
}


static struct clocksource clksrc = {
	.name		= "timer2",
	.rating		= 200,
	.read		= clksrc_read,
	.mask		= CLOCKSOURCE_MASK(32),
	.shift		= 20,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};


static void tmr_set_mode(enum clock_event_mode mode,
	struct clock_event_device *evt)
{
	unsigned long regval32, u4State;
	spin_lock_irqsave(&mt53xx_bim_lock, u4State);
	regval32=__bim_readl(REG_RW_TIMER_CTRL);
	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		    regval32 |= (SYS_TIMER_EN | SYS_TIMER_AL);
		break;
	case CLOCK_EVT_MODE_ONESHOT:
		    regval32 &= ~SYS_TIMER_AL;
		    regval32 |= SYS_TIMER_EN;
		break;

	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:		
	default:
		regval32 &= ~(SYS_TIMER_AL|SYS_TIMER_AL);	
		break;
	}
	__bim_writel(regval32, REG_RW_TIMER_CTRL);
	spin_unlock_irqrestore(&mt53xx_bim_lock, u4State);
}

static int tmr_set_next_event(unsigned long next,
	struct clock_event_device *evt)
{
	unsigned long regval32, u4State;
	spin_lock_irqsave(&mt53xx_bim_lock, u4State);
	regval32=__bim_readl(REG_RW_TIMER_CTRL) |SYS_TIMER_EN;
	__bim_writel(next, SYS_TIMER_REG_L);
	__bim_writel(regval32, REG_RW_TIMER_CTRL);
	spin_unlock_irqrestore(&mt53xx_bim_lock, u4State);
	return 0;
}


static struct clock_event_device clkevent = {
	.name		= "timer0",
	.irq		= SYS_TIMER_IRQ,
 	.shift		= 20,
	.features       = CLOCK_EVT_FEAT_PERIODIC|CLOCK_EVT_FEAT_ONESHOT,
	.set_mode	= tmr_set_mode,
	.set_next_event	= tmr_set_next_event,
	.rating		= 300,
	.cpumask	= cpu_all_mask,
};
#endif

static struct irqaction mt53xx_timer_irq =
{
    .name       = "Mt53xx Timer Tick",
    .flags      = IRQF_DISABLED | IRQF_TIMER,
    .handler    = mt53xx_timer_interrupt,
#ifdef CONFIG_GENERIC_CLOCKEVENTS
    .dev_id	= &clkevent
#endif
};


static void __init mt53xx_timer_init(void)
{
    u32 regval32;
    u32 _u4SysClock;

    /* disable system clock */
    regval32 = __bim_readl(REG_RW_TIMER_CTRL);
    regval32 &= ~(SYS_TIMER_EN | SYS_TIMER_AL);
    __bim_writel(regval32, REG_RW_TIMER_CTRL);

    /* setup timer interval */
    _u4SysClock = _CKGEN_GetXtalClock();
    __bim_writel(_u4SysClock / HZ, SYS_TIMER_LIMIT_REG_L);
    __bim_writel(0, SYS_TIMER_LIMIT_REG_H);

    /* count down for 10 thousand years */
    // already done in preloader
    // __bim_writel(0xffffffff, HP_TIMER_LIMIT_REG_L);
    // __bim_writel(0xffffffff, HP_TIMER_LIMIT_REG_H);

    /* reset timer */
    __bim_writel(_u4SysClock / HZ, SYS_TIMER_REG_L);
    __bim_writel(0, SYS_TIMER_REG_H);
    // already done in preloader
    // __bim_writel(0xffffffff, HP_TIMER_REG_L);
    // __bim_writel(0xffffffff, HP_TIMER_REG_H);

    //mt53xx_mask_irq(SYS_TIMER_IRQ);
    _timer_tick_count = 0;

#ifdef CONFIG_GENERIC_CLOCKEVENTS
	printk("ckgen Xtal:%d Hz\n",_CKGEN_GetXtalClock());
	clksrc.mult = clocksource_hz2mult(_CKGEN_GetXtalClock(), clksrc.shift);
	clocksource_register(&clksrc);

	clkevent.mult = div_sc(_CKGEN_GetXtalClock(), NSEC_PER_SEC, clkevent.shift);
	clkevent.max_delta_ns = clockevent_delta2ns(0xffffffff, &clkevent);
	clkevent.min_delta_ns = clockevent_delta2ns(0xf, &clkevent);
	clockevents_register_device(&clkevent);
#endif
    /* set system irq */
    setup_irq(SYS_TIMER_IRQ, &mt53xx_timer_irq);

    /* enable timer with auto-load */
    regval32 |= (SYS_TIMER_EN | SYS_TIMER_AL);
    __bim_writel(regval32, REG_RW_TIMER_CTRL);
}

//static unsigned long mt53xx_gettimeoffset(void)
//{
//    return 0;
//}

struct sys_timer mt53xx_timer =
{
    .init       = mt53xx_timer_init,
//    .offset     = mt53xx_gettimeoffset,
};

/* used to for scheduling */
unsigned long long notrace sched_clock(void)
{
    cycle_t cyc = clksrc_read(NULL);
    struct clocksource *cs = &clksrc;
    u64 th, tl;

    tl = (((u64)cyc&0xffffffff) * cs->mult) >> cs->shift;
    th = ((((u64)cyc>>32)&0xffffffff) * cs->mult);

    return ((th<<(32-cs->shift))+tl);
}

unsigned long mt53xx_timer_read(int nr)
{
    return __bim_readl((REG_RW_TIMER0_LOW + (nr * 8)));
}
EXPORT_SYMBOL(mt53xx_timer_read);

unsigned long long mt53xx_timer_read_ll(int nr)
{
    unsigned long long utime;
    unsigned long time_lo1;
    unsigned long time_hi1, time_hi2;

    do
    {
        time_hi1 = __bim_readl(REG_RW_TIMER0_HIGH + (nr * 8));
        time_lo1 = __bim_readl(REG_RW_TIMER0_LOW + (nr * 8));
        time_hi2 = __bim_readl(REG_RW_TIMER0_HIGH + (nr * 8));
    } while (time_hi1 != time_hi2);

    utime = ((unsigned long long)time_hi1 << 32) + (unsigned long long)time_lo1;

    return utime;
}
EXPORT_SYMBOL(mt53xx_timer_read_ll);


/* /proc/htimer */

static ssize_t _htimer_ticks_per_sec_read(struct file *file, char __user *buf,
                                          size_t count, loff_t *ppos)
{
    u32 ticks_per_sec;
    u32 tbuf[2];

    if (count != 8)
    {
        return 0;
    }

    ticks_per_sec = _CKGEN_GetXtalClock();
    tbuf[0] = ticks_per_sec;
    tbuf[1] = 0;

    if (copy_to_user(buf, (void*)(tbuf), 8))
    {
        return -EFAULT;
    }

    return 8;
}

static ssize_t _htimer_current_tick_read(struct file *file, char __user *buf,
                                         size_t count, loff_t *ppos)
{
    u32 u4TickH, u4TickL;
    u32 tbuf[2];

    if (count != 8)
    {
        return 0;
    }

    u4TickH = *((volatile u32 *) (0xF0008194));
    u4TickL = *((volatile u32 *) (0xF0008074));

    tbuf[0] = u4TickL;
    tbuf[1] = u4TickH;

    if (copy_to_user(buf, (void*)(tbuf), 8))
    {
        return -EFAULT;
    }

    return 8;
}

static struct file_operations _htimer_ticks_per_sec_operations =
{
    .read = _htimer_ticks_per_sec_read,
};

static struct file_operations _htimer_current_tick_operations =
{
    .read = _htimer_current_tick_read,
};

/* hack from ./fs/proc/internal.h:14*/
extern struct proc_dir_entry proc_root;

void mt53xx_proc_htimer_init(void)
{
    struct proc_dir_entry * proc_file;

    proc_file = create_proc_entry("htimer_ticks_per_sec", S_IFREG | S_IWUSR, &proc_root);
    if (proc_file)
    {
        proc_file->proc_fops = &_htimer_ticks_per_sec_operations;
    }

    proc_file = create_proc_entry("htimer_current_tick", S_IFREG | S_IWUSR, &proc_root);
    if (proc_file)
    {
        proc_file->proc_fops = &_htimer_current_tick_operations;
    }
}
