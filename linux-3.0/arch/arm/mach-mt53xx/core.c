/*
 * linux/arch/arm/mach-mt53xx/core.c
 *
 * CPU core init - irq, time, baseio
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
#include <linux/bootmem.h>

#include <mach/hardware.h>
#include <mach/mt53xx_linux.h>
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

#include <asm/pmu.h>
#include <linux/platform_device.h>

// NDC patch start
#include <linux/random.h>
#include <linux/net.h>
#include <asm/uaccess.h>
// NDC patch stop

// PCI related header file.
#include <linux/pci.h>
#include <asm/mach/pci.h>
#ifdef CONFIG_PCI
#include <plat/mtk_pci.h>
#endif

#ifdef CONFIG_ARM_GIC
#include <asm/hardware/gic.h>
static void (*gic_irq_mask)(struct irq_data *data) = NULL;
static void (*gic_irq_unmask)(struct irq_data *data) = NULL;
static void (*gic_irq_eoi)(struct irq_data *data) = NULL;
#endif

#include <linux/usb/android_composite.h>
#include <linux/platform_device.h>

#ifdef ANDROID
// PMEM related
#include <linux/platform_device.h>
#include <linux/bootmem.h>
#include <linux/android_pmem.h>
#define MT53XX_PMEM_SIZE 0x2F00000


int pmem_setup(struct android_pmem_platform_data *pdata,
	       long (*ioctl)(struct file *, unsigned int, unsigned long),
	       int (*release)(struct inode *, struct file *));
#endif

DEFINE_SPINLOCK(mt53xx_bim_lock);
EXPORT_SYMBOL(mt53xx_bim_lock);

unsigned int mt53xx_cha_mem_size;
static unsigned long mt53xx_reserve_start, mt53xx_reserve_size;

void mt53xx_get_reserved_area(unsigned long *start, unsigned long *size)
{
    *start = mt53xx_reserve_start;
    *size = mt53xx_reserve_size;
}
EXPORT_SYMBOL(mt53xx_get_reserved_area);

/*======================================================================
 * irq
 */

static inline u32 __bim_readl(u32 regaddr32)
{
    return __raw_readl(BIM_VIRT + regaddr32);
}

static inline void __bim_writel(u32 regval32, u32 regaddr32)
{
    __raw_writel(regval32, BIM_VIRT + regaddr32);
}

static inline u32 __pdwnc_readl(u32 regaddr32)
{
        return __raw_readl(PDWNC_VIRT + regaddr32);
}

static inline void __pdwnc_writel(u32 regval32, u32 regaddr32)
{
        __raw_writel(regval32, PDWNC_VIRT + regaddr32);
}

void __pdwnc_reboot_in_kernel(void)
{
        __pdwnc_writel(0x7fff0000, 0x104);
        __pdwnc_writel((__pdwnc_readl(0x100) & 0x00ffffff) | 0x01, 0x100);
}

static inline int _bim_is_vector_valid(u32 irq_num)
{
    return (irq_num < NR_IRQS);
}

#ifdef CONFIG_MT53XX_NATIVE_GPIO
#define MSDC_GPIO_MAX_NUMBERS 6     
int MSDC_Gpio[MSDC_GPIO_MAX_NUMBERS]= {0} ;

EXPORT_SYMBOL(MSDC_Gpio);

static int __init MSDCGPIO_CommonParamParsing(char *str, int *pi4Dist)
{
    char tmp[8]={0};
    char *p,*s;
    int len,i,j;
    if(strlen(str) != 0)
	{      
        printk(KERN_ERR "Parsing String = %s \n",str);
    }
    else
	{
        printk(KERN_ERR "Parse Error!!!!! string = NULL\n");
        return 0;
    }

    for (i=0; i<MSDC_GPIO_MAX_NUMBERS; i++)
	{
        s=str;
        if (i != (MSDC_GPIO_MAX_NUMBERS-1) )
		{
            if(!(p=strchr(str, ',')))
			{
                printk(KERN_ERR "Parse Error!!string format error 1\n");
                break;
            }
            if (!((len=p-s) >= 1))
            {
                printk(KERN_ERR "Parse Error!! string format error 2\n");
                break;
            }
        }
		else 
        {
            len = strlen(s);
        }
        
        for(j=0;j<len;j++)
        {
            tmp[j]=*s;
            s++;
        }
        tmp[j]=0;

        pi4Dist[i] = (int)simple_strtol(&tmp[0], NULL, 10);
        printk(KERN_ERR"Parse Done: msdc [%d] = %d \n", i, pi4Dist[i]);
        
        str += (len+1);
    }
    
    return 1;

}


static int __init MSDC_GpioParseSetup(char *str)
{
    return MSDCGPIO_CommonParamParsing(str,&MSDC_Gpio[0]);
}

__setup("msdcgpio=", MSDC_GpioParseSetup);

#endif

static void mt53xx_irq_mask_ack(struct irq_data *data)
{
    u32 regval32;
    unsigned long flags;
    unsigned int irq = data->irq;

    if (!_bim_is_vector_valid(irq))
    {
        while (1)
            ;
        return;
    }

#ifdef CONFIG_ARM_GIC
    gic_irq_mask(data);
    gic_irq_eoi(data);
#endif
    if (irq >= VECTOR_MISC2_BASE)
    {
        u32 misc_irq;
        misc_irq = _MISC2IRQ(irq);
        spin_lock_irqsave(&mt53xx_bim_lock, flags);
        regval32 = __bim_readl(REG_RW_M2INTEN);
        regval32 &= ~misc_irq;
        __bim_writel(regval32, REG_RW_M2INTEN);
        __bim_writel(misc_irq, REG_RW_M2INTCLR);
        spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
    }
    else if (irq >= VECTOR_MISC_BASE)
    {
        u32 misc_irq;
        misc_irq = _MISCIRQ(irq);
        spin_lock_irqsave(&mt53xx_bim_lock, flags);
        regval32 = __bim_readl(REG_RW_MINTEN);
        regval32 &= ~misc_irq;
        __bim_writel(regval32, REG_RW_MINTEN);
        if (irq != VECTOR_DRAMC)
        {
            __bim_writel(misc_irq, REG_RW_MINTCLR);
        }
        spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
    }
    else
    {
        spin_lock_irqsave(&mt53xx_bim_lock, flags);
        regval32 = __bim_readl(REG_RW_IRQEN);
        regval32 &= ~(1 << (irq-SPI_OFFSET));
        __bim_writel(regval32, REG_RW_IRQEN);
        if (irq != VECTOR_DRAMC)
        {
            __bim_writel((1 << (irq-SPI_OFFSET)), REG_RW_IRQCLR);
        }
        spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
    }
}

static void mt53xx_irq_mask(struct irq_data *data)
{
    u32 regval32;
    unsigned long flags;
    unsigned int irq = data->irq;

    if (!_bim_is_vector_valid(irq))
    {
        while (1)
            ;
        return;
    }

#ifdef CONFIG_ARM_GIC
    gic_irq_mask(data);
#endif
    if (irq >= VECTOR_MISC2_BASE)
    {
        u32 misc_irq;
        misc_irq = _MISC2IRQ(irq);
        spin_lock_irqsave(&mt53xx_bim_lock, flags);
        regval32 = __bim_readl(REG_RW_M2INTEN);
        regval32 &= ~misc_irq;
        __bim_writel(regval32, REG_RW_M2INTEN);
        spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
    }
    else if (irq >= VECTOR_MISC_BASE)
    {
        u32 misc_irq;
        misc_irq = _MISCIRQ(irq);
        spin_lock_irqsave(&mt53xx_bim_lock, flags);
        regval32 = __bim_readl(REG_RW_MINTEN);
        regval32 &= ~misc_irq;
        __bim_writel(regval32, REG_RW_MINTEN);
        spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
    }
    else
    {
        spin_lock_irqsave(&mt53xx_bim_lock, flags);
        regval32 = __bim_readl(REG_RW_IRQEN);
        regval32 &= ~(1 << (irq-SPI_OFFSET));
        __bim_writel(regval32, REG_RW_IRQEN);
        spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
    }
}

static void mt53xx_irq_unmask(struct irq_data *data)
{
    u32 regval32;
    unsigned long flags;
    unsigned int irq = data->irq;

    if (irq >= VECTOR_MISC2_BASE)
    {
        u32 misc_irq;
        misc_irq = _MISC2IRQ(irq);
        spin_lock_irqsave(&mt53xx_bim_lock, flags);
        regval32 = __bim_readl(REG_RW_M2INTEN);
        regval32 |= (misc_irq);
        __bim_writel(regval32, REG_RW_M2INTEN);
        spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
    }
    else if (irq >= VECTOR_MISC_BASE)
    {
        u32 misc_irq;
        misc_irq = _MISCIRQ(irq);
        spin_lock_irqsave(&mt53xx_bim_lock, flags);
        regval32 = __bim_readl(REG_RW_MINTEN);
        regval32 |= (misc_irq);
        __bim_writel(regval32, REG_RW_MINTEN);
        spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
    }
    else
    {
        spin_lock_irqsave(&mt53xx_bim_lock, flags);
        regval32 = __bim_readl(REG_RW_IRQEN);
        regval32 |= (1 << (irq-SPI_OFFSET));
        __bim_writel(regval32, REG_RW_IRQEN);
        spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
    }
#ifdef CONFIG_ARM_GIC
    gic_irq_unmask(data);
#endif
}

static struct irq_chip mt53xx_irqchip =
{
    .irq_mask_ack = mt53xx_irq_mask_ack,
    .irq_mask     = mt53xx_irq_mask,
    .irq_unmask   = mt53xx_irq_unmask,
};


/* Get IC Version */

static inline int BSP_IsFPGA(void)
{
    unsigned int u4Val;

    /* If there is FPGA ID, it must be FPGA, too. */
    u4Val = __bim_readl(REG_RO_FPGA_ID);
    if (u4Val != 0) { return 1; }

    /* otherwise, it's not FPGA. */
    return 0;
}

unsigned int mt53xx_get_ic_version()
{
    unsigned int u4Version;

    if (BSP_IsFPGA())
        return 0;

#if defined(CONFIG_ARCH_MT5396)
    u4Version = __bim_readl(REG_RO_SW_ID);
    u4Version = __bim_readl(REG_RO_CHIP_ID);
    u4Version = ((u4Version & 0x0fffff00U) << 4) |
                ((u4Version & 0x000000ffU) << 0) |
                ((u4Version & 0xf0000000U) >> 20);
#elif defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880) || defined(CONFIG_ARCH_MT5881)
    u4Version = __raw_readl(CKGEN_VIRT + REG_RO_SW_ID);
#else
#error Dont know how to get IC version.
#endif

    return u4Version;
}
EXPORT_SYMBOL(mt53xx_get_ic_version);



static struct resource mt53xx_pmu_resource[] = {
    [0] = {
        .start  = VECTOR_PMU0,
        .end    = VECTOR_PMU0,
        .flags  = IORESOURCE_IRQ,
    },
#if defined(CONFIG_SMP) && defined(VECTOR_PMU1)
    [1] = {
        .start  = VECTOR_PMU1,
        .end    = VECTOR_PMU1,
        .flags  = IORESOURCE_IRQ,
    },
#endif
};

static struct platform_device mt53xx_pmu_device = {
    .name       = "arm-pmu",
    .id     = ARM_PMU_DEVICE_CPU,
    .resource   = mt53xx_pmu_resource,
    .num_resources  = ARRAY_SIZE(mt53xx_pmu_resource),
};



static void __init mt53xx_init_machine_irq(void)
{
    unsigned long flags;
    unsigned int irq;
    u32 u4Reg;
    
#ifdef CONFIG_ARM_GIC
    struct irq_chip *gic;
    gic_init(0, 29, (void __iomem *)MPCORE_GIC_DIST_VIRT, (void __iomem *)MPCORE_GIC_CPU_VIRT);

    gic = irq_get_chip(29);
    gic_irq_unmask = gic->irq_unmask;
    gic_irq_mask = gic->irq_mask;
    gic_irq_eoi = gic->irq_eoi; 
    mt53xx_irqchip.irq_set_affinity=gic->irq_set_affinity;
#endif

    spin_lock_irqsave(&mt53xx_bim_lock, flags);

    /* turn all irq off */
    __bim_writel(0, REG_RW_IRQEN);
    __bim_writel(0, REG_RW_MINTEN);
    __bim_writel(0, REG_RW_M2INTEN);

    /* clear all pending irq */
    __bim_writel(0xffffffff, REG_RW_IRQCLR);
    __bim_writel(0xffffffff, REG_RW_IRQCLR);
    __bim_writel(0xffffffff, REG_RW_IRQCLR);
    __bim_writel(0xffffffff, REG_RW_IRQCLR);
    __bim_writel(0xffffffff, REG_RW_IRQCLR);
    __bim_writel(0xffffffff, REG_RW_MINTCLR);
    __bim_writel(0xffffffff, REG_RW_MINTCLR);
    __bim_writel(0xffffffff, REG_RW_MINTCLR);
    __bim_writel(0xffffffff, REG_RW_MINTCLR);
    __bim_writel(0xffffffff, REG_RW_MINTCLR);
    __bim_writel(0xffffffff, REG_RW_M2INTCLR);
    __bim_writel(0xffffffff, REG_RW_M2INTCLR);
    __bim_writel(0xffffffff, REG_RW_M2INTCLR);
    __bim_writel(0xffffffff, REG_RW_M2INTCLR);
    __bim_writel(0xffffffff, REG_RW_M2INTCLR);


    for (irq = SPI_OFFSET; irq < NR_IRQS; irq++)
    {
        irq_set_chip(irq, &mt53xx_irqchip);
        irq_set_handler(irq, handle_level_irq);
        set_irq_flags(irq, IRQF_VALID | IRQF_PROBE);
    }
        
    /* turn misc and misc2 irq on */
    __bim_writel(0xc0000000, REG_RW_IRQEN);

    /* set pmu irq to level triggered */
    u4Reg = __bim_readl(REG_RW_MISC);
    u4Reg |= 0x100;
    __bim_writel(u4Reg, REG_RW_MISC);

    /* for some reason, people believe we have to do above */
    spin_unlock_irqrestore(&mt53xx_bim_lock, flags);
}

/* io map */

static struct map_desc mt53xx_io_desc[] __initdata =
{
    {IO_VIRT, __phys_to_pfn(IO_PHYS), IO_SIZE, MT_DEVICE},
    {0xF2000000, __phys_to_pfn(0xF2000000), 0x1000, MT_DEVICE},
#ifdef CONFIG_OPM
    {LZHS_SRM_VIRT, __phys_to_pfn(0xFB004000), 0x1000, MT_MEMORY_ITCM},
#endif
    {DMX_SRAM_VIRT, __phys_to_pfn(0xFB005000), 0x1000, MT_DEVICE}, // ldr env temp copy
#ifdef CONFIG_CPU_V7
    {0xF2002000, __phys_to_pfn(0xF2002000), SZ_16K, MT_DEVICE},    // SCU
#endif
#ifdef CONFIG_PCI    
    /* use for PCI express module */
    {PCIE_MMIO_BASE, __phys_to_pfn(PCIE_MMIO_PHYSICAL_BASE), PCIE_MMIO_LENGTH, MT_DEVICE},   
#endif /* CONFIG_PCI */
};

#ifdef ANDROID
static struct android_pmem_platform_data android_pmem_pdata = {
    .name = "pmem",
    .cached = 1,
};

static struct platform_device android_pmem_device = {
    .name = "android_pmem",
    .id = 0,
    .dev = {.platform_data = &android_pmem_pdata},
};

static unsigned pmem_size = MT53XX_PMEM_SIZE;

static void __init mtk_pmem_allocate_memory_regions(void)
{
    void *addr;
    unsigned long size;
    size = pmem_size;

	printk(KERN_INFO "====> PMEM BOOTMEM ALLOC <=====\n");
    if (size) {
#if 0
        addr = alloc_bootmem(size);
        android_pmem_pdata.start = __pa(addr);
        android_pmem_pdata.size = size;
#endif
        pr_info("allocating %lu bytes at %p (%lx physical) for"
            " pmem arena\n", size, addr, __pa(addr));
    }
}

void mtk_pmem_set_memory_regions(u32 start, u32 size)
{
        int i;
        android_pmem_pdata.start = start;
        android_pmem_pdata.size = size;
        i = pmem_setup(&android_pmem_pdata, NULL, NULL);
        printk(KERN_INFO "====> PMEM callback <=====\n");
        printk(KERN_INFO "pmem: start: 0x%08X, size: 0x%08X\n", start, size);
}
EXPORT_SYMBOL(mtk_pmem_set_memory_regions);
#endif

static void __init mt53xx_map_io(void)
{
    unsigned long hole, b0end;
    struct map_desc fbm_io_desc = {0, 0, 0, MT_MEMORY};  // For FBM mapping.
    unsigned long cha_mem_size;

    // MAP iotable first, so we can access BIM register.
    iotable_init(mt53xx_io_desc, ARRAY_SIZE(mt53xx_io_desc));

    // Get ChA memory size. By looking up TCM, must do this after io map.
    mt53xx_cha_mem_size = cha_mem_size = ((TCM_DRAM_SIZE) & 0xFFFF) * 1024 * 1024;

    // Check how to map FBM.
    // We prefer to map FBM early, because it will use section mapping
    // (page size=1M). This could reduce TLB miss and save some RAM (pte space)
    // Also, it can save vmlloc space if there's a hole (2 membank)
    //
    // Here's the rule:
    // 
    // 1. If there's 2 bank, and 2nd bank start <= 512MB, then FBM is in the middle, just map it in place.
    // 2. If total memory(including FBM) <= 512MB, just map all memory after kenel.
    // 3. If 1st bank >= 512MB, don't do anything now.
    // 4. If trustzone enabled, don't mapping tz memory in mmu.

    if (meminfo.nr_banks >= 2)
    {
        b0end = meminfo.bank[0].start + meminfo.bank[0].size;
        hole = meminfo.bank[1].start - b0end;
        if (meminfo.bank[1].start <= (unsigned long)512*1024*1024 && hole)
        {
            mt53xx_reserve_start = b0end;
            mt53xx_reserve_size = hole;
        }
    }
    else
    {
        // Hueristic to guess how many memory we have and possible size of FBM.
        b0end = meminfo.bank[0].start + meminfo.bank[0].size;
        if (cha_mem_size <= 512*1024*1024 && b0end < cha_mem_size)
        {
            mt53xx_reserve_start = b0end;
            mt53xx_reserve_size = cha_mem_size - b0end;
        }
    }

    if (mt53xx_reserve_start)
    {
        struct map_desc *desc = &fbm_io_desc;

#ifdef CC_TRUSTZONE_SUPPORT
        mt53xx_reserve_size -= (16*1024*1024);
#endif

        desc->virtual = (unsigned long)phys_to_virt(mt53xx_reserve_start);
        desc->pfn = __phys_to_pfn(mt53xx_reserve_start);
        desc->length = mt53xx_reserve_size;
        iotable_init(desc, 1);
    }

#ifdef ANDROID
    mtk_pmem_allocate_memory_regions();
#endif
}

extern char _text, _etext;
int mt53xx_get_rodata_addr(unsigned int *start, unsigned int *end)
{
    if (start)
    {
        *start = (unsigned int) (&_text);;
    }

    if (end)
    {
        *end = (unsigned int) (&_etext);
    }

    return 0;
}
EXPORT_SYMBOL(mt53xx_get_rodata_addr);

extern char _data, _end;
int mt53xx_get_rwdata_addr(unsigned int *start, unsigned int *end)
{
    if (start)
    {
        *start = (unsigned int) (&_data);
    }

    if (end)
    {
        *end = (unsigned int) (&_end);
    }

    return 0;
}
EXPORT_SYMBOL(mt53xx_get_rwdata_addr);

// Print imprecise abort debug message according to BIM debug register
void mt53xx_imprecise_abort_report(void)
{
    u32 iobus_tout = __bim_readl(REG_IOBUS_TOUT_DBG);
    u32 dramf_tout = __bim_readl(REG_DRAMIF_TOUT_DBG);
    int print = 0;

    if (iobus_tout & 0x10000000)
    {
        print = 1;
        printk(KERN_ALERT "IOBUS %s addr 0xf00%05x timeout\n",
              (iobus_tout & 0x8000000) ? "write" : "read", iobus_tout & 0xfffff);
    }

    if (dramf_tout & 0x10)
    {
        print = 1;
        printk(KERN_ALERT "DRAM %s addr 0x%08x timeout\n",
              (dramf_tout & 0x8) ? "write" : "read", __bim_readl(REG_DRAMIF_TOUT_ADDR));
    }

    if (!print)
        printk(KERN_ALERT "MT53xx unknown imprecise abort\n");
}

/*****************************************************************************
 * PCI
 ****************************************************************************/
#ifdef CONFIG_PCI
static struct hw_pci mtk_pci __initdata = {
    .nr_controllers = 1,
    .swizzle    = pci_std_swizzle,
    .setup      = MTK_pci_sys_setup,
    .scan       = MTK_pci_sys_scan,
    .map_irq    = MTK_pci_map_irq
};

static int __init mtk_pci_init(void)
{
    pci_common_init(&mtk_pci);

    return 0;
}

subsys_initcall(mtk_pci_init);
#endif /* CONFIG_PCI */

/*****************************************************************************
 * RTC
 ****************************************************************************/

#ifdef CONFIG_RTC_DRV_MT53XX
static struct resource mt53xx_rtc_resource[] = {
    [0] = {
//        .start  = VECTOR_PDWNC,
//        .end    = VECTOR_PDWNC,
        .start  = -1,//TODO
        .end    = -1,
        .flags  = IORESOURCE_IRQ,
    },
};
static struct platform_device mt53xx_rtc_device = {
	.name		= "mt53xx_rtc",
	.id		= 0,
    .resource   = mt53xx_rtc_resource,
    .num_resources  = ARRAY_SIZE(mt53xx_rtc_resource),
};

static void __init mt53xx_add_device_rtc(void)
{
	platform_device_register(&mt53xx_rtc_device);
}
#else
static void __init mt53xx_add_device_rtc(void) {}
#endif

#ifdef ANDROID
static char *usb_functions_adb_ums[] = { "adb" };

static char *usb_functions_all[] = { "adb"};

struct android_usb_product usb_products[] = {
    {
    .product_id = 0x0d02, 
//    .product_id = 0x0005, 
    .num_functions = ARRAY_SIZE(usb_functions_adb_ums),
    .functions      = usb_functions_adb_ums,
    },
};

/* standard android USB platform data */
static struct android_usb_platform_data andusb_plat = {
        .vendor_id              = 0x18d1,
//        .vendor_id              = 0x0E8D,
        .product_id             = 0x0004,    
        .manufacturer_name      = "MediaTek Inc.",    
        .product_name           = "MTK_adb",    
        .serial_number          = "AndroidTV",   
        .num_products           = ARRAY_SIZE(usb_products),    
        .products               = usb_products,    
        .num_functions          = ARRAY_SIZE(usb_functions_all),    
        .functions              = usb_functions_all,    
};
        
static struct platform_device androidusb_device = {    
        .name           = "android_usb",    
        .id             = -1,    
        .dev            = {    
                .platform_data  = &andusb_plat,    
        },    
};
#endif
/*****************************************************************************
 * 53xx init
 ****************************************************************************/
#ifdef CC_SUPPORT_BL_DLYTIME_CUT
extern u32 _CKGEN_GetXtalClock(void);

int BIM_GetCurTime(void)
{
   unsigned long u4Time;
   int u4Val;
   u4Time = __bim_readl(REG_RW_TIMER2_LOW);
   u4Val = ((~u4Time)/(_CKGEN_GetXtalClock()/1000000));  // us time.
   printk("BIM_GetCurTime %d us \n", u4Val);
   return u4Val;
}
extern void mtk_gpio_set_value(unsigned gpio, int value);
extern int mtk_gpio_direction_output(unsigned gpio, int init_value);
static struct timer_list bl_timer;
static void restore_blTimer(unsigned long data);
static DEFINE_TIMER(bl_timer, restore_blTimer, 0, 0);

static void restore_blTimer(unsigned long data)
{
	u32 gpio, value;
	gpio = __bim_readl(REG_RW_GPRB0 + (5 << 2))&0xffff;
	value = __bim_readl(REG_RW_GPRB0 + (5 << 2))>>16;
	
	printk("[LVDS][backlight] Time is up!!!\n");
	mtk_gpio_direction_output(gpio, 1);
    mtk_gpio_set_value(gpio, value);
    del_timer(&bl_timer); 
}
#endif /* end of CC_SUPPORT_BL_DLYTIME_CUT */

extern void mt53xx_proc_htimer_init(void);
static void  __init mt53xx_init_machine(void)
{
#ifdef CC_SUPPORT_BL_DLYTIME_CUT
 	unsigned int u4CurTime;
	u32 gpio, value;
 	u4CurTime = BIM_GetCurTime();
	gpio = __bim_readl(REG_RW_GPRB0 + (5 << 2))&0xffff;
	value = __bim_readl(REG_RW_GPRB0 + (5 << 2))>>16;
 	printk("[LVDS][backlight] TIME=%d us\n", __bim_readl((REG_RW_GPRB0 + (4 << 2))));
	if (u4CurTime >= __bim_readl((REG_RW_GPRB0 + (4 << 2))))
	{
		printk("[LVDS][backlight] #1\n");
		mtk_gpio_direction_output(gpio, 1);
		mtk_gpio_set_value(gpio, value);
	}
	else
	{
		printk("[LVDS][backlight] #2\n");
		bl_timer.expires = jiffies + usecs_to_jiffies( abs(__bim_readl((REG_RW_GPRB0 + (4 << 2))) - u4CurTime));
    	add_timer(&bl_timer);
	}
#endif /* end of CC_SUPPORT_BL_DLYTIME_CUT */

    mt53xx_proc_htimer_init();

    platform_device_register(&mt53xx_pmu_device);
#ifdef ANDROID
    platform_device_register(&android_pmem_device);
    platform_device_register(&androidusb_device);
#endif
    mt53xx_add_device_rtc();
}

extern struct sys_timer mt53xx_timer;


#ifdef CONFIG_CACHE_L2X0
#include <asm/hardware/cache-l2x0.h>

#ifdef CONFIG_CPU_V7
static int __init mt53xx_l2_cache_init(void)
{
/*
[30] Early BRESP enable 0 = Early BRESP disabled. This is the default. 1 = Early BRESP enabled.
[29] Instruction prefetch enable 0 = Instruction prefetching disabled. This is the default. 1 = Instruction prefetching enabled.
[28] Data prefetch enable 0 = Data prefetching disabled. This is the default. 1 = Data prefetching enabled.
[24:23] Force write allocate b00 = Use AWCACHE attributes for WA. This is the default. 
b01 = Force no allocate, set WA bit always 0.
b10 = Override AWCACHE attributes, set WA bit always 1, all cacheable write misses become write allocated.
b11 = Internally mapped to 00.
[12] Exclusive cache configuration 0 = Disabled. This is the default. 1 = Enabled
[0] Full Line of Zero Enable 0 = Full line of write zero behavior disabled. This is the default. 1 = Full line of write zero behavior
normal:0xf2000104=0x73041001
benchmark:0xf2000104=0x72040001
*/
    register unsigned int reg;
    unsigned l2val = 0x71000001;
    unsigned l2mask = 0x8e7ffffe;
    void __iomem *l2x0_base = (void __iomem *)L2C_BASE;

	/*
	 * Check if l2x0 controller is already enabled.
	 * If you are booting from non-secure mode
	 * accessing the below registers will fault.
	 */
	if (!(readl_relaxed(l2x0_base + L2X0_CTRL) & 1)) 
    {
#if defined(CONFIG_ARCH_MT5398)
        /* set RAM latencies, TAG=1T, DATA=3T */
        writel(0, l2x0_base + L2X0_TAG_LATENCY_CTRL);
        writel(0x21, l2x0_base + L2X0_DATA_LATENCY_CTRL);
#elif defined(CONFIG_ARCH_MT5880)
        /* set RAM latencies, TAG=1T, DATA=2T */
        if (!IS_IC_MT5860())
        {
            writel(0, l2x0_base + L2X0_TAG_LATENCY_CTRL);
            writel(0x11, l2x0_base + L2X0_DATA_LATENCY_CTRL);
        }
        else
        {
            // For Python, Set Tag RAM DELSEL(0x1d8)
            __bim_writel(0xea3, 0x1d8);
        }
#endif

#if defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880)
        // Enable L2 double line fill.
        writel(readl(l2x0_base + 0xf60)|(1<<30), l2x0_base + 0xf60);
#endif
    }

#ifdef CONFIG_ENABLE_EXCLUSIVE_CACHE
    l2val |= 0x1000;
#endif

    l2x0_init((void __iomem *)L2C_BASE, l2val, l2mask);
    __asm__ ("MRC     p15, 0, %0, c1, c0, 1" : "=r" (reg));
    __asm__ ("ORR     %0, %1, #0x8" : "=r" (reg) : "r" (reg)); //FOZ=0x8
    __asm__ ("MCR     p15, 0, %0, c1, c0, 1" : : "r" (reg));
    return 0;
}
#else
static int __init mt53xx_l2_cache_init(void)
{
    l2x0_init((void __iomem *)L2C_BASE, 0x31000000, 0xce7fffff);
    return 0;
}
#endif /* CONFIG_CPU_V7 */
early_initcall(mt53xx_l2_cache_init);
#endif

/*****************************************************************************
 * Model Index
 ****************************************************************************/

unsigned int modelindex_id;
EXPORT_SYMBOL(modelindex_id);
static int __init modelindex_setup(char *line)
{
	sscanf(line, "%u",&modelindex_id);
	printk("kernel setup modelindex=%u\n",modelindex_id);
	return 1;
}

__setup("modelindex=", modelindex_setup);


#ifdef CONFIG_ARCH_MT5368
MACHINE_START(MT5368, "MT5368")
#elif defined(CONFIG_MACH_MT5396)
MACHINE_START(MT5396, "MT5396")
#elif defined(CONFIG_MACH_MT5369)
MACHINE_START(MT5396, "MT5369")
#elif defined(CONFIG_MACH_MT5389)
MACHINE_START(MT5389, "MT5389")
#elif defined(CONFIG_MACH_MT5398)
MACHINE_START(MT5398, "MT5398")
#elif defined(CONFIG_MACH_MT5880)
MACHINE_START(MT5880, "MT5880")
#elif defined(CONFIG_MACH_MT5881)
MACHINE_START(MT5881, "MT5881")
#else
#error Undefined mach/arch
#endif
    .boot_params    = MT53xx_SDRAM_PA + 0x100,
    .map_io         = mt53xx_map_io,
    .init_irq       = mt53xx_init_machine_irq,
    .timer          = &mt53xx_timer,
    .init_machine   = mt53xx_init_machine,
MACHINE_END
