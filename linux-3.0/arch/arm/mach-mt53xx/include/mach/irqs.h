/*
 * linux/arch/arm/mach-mt53xx/include/mach/irqs.h
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

#if !defined(IRQS_H)
#define IRQS_H

#ifdef CONFIG_ARM_GIC
#define IRQ_LOCALTIMER                  29
#define IRQ_LOCALWDOG                   30

#define SPI_OFFSET	32
#define NR_IRQS         256
#else /* not CONFIG_ARM_GIC part*/
#define SPI_OFFSET	0
#define NR_IRQS         128
#endif //CONFIG_ARM_GIC

#ifndef __USE_XBIM_IRQS         // UGLY HACK! make sure people using x_bim.h get desired define.
#define IRQ_TIMER0              (3+SPI_OFFSET)
#define ARCH_TIMER_IRQ          (5+SPI_OFFSET)    // system tick timer irq

// Interrupt vectors
#define VECTOR_PDWNC    (0+SPI_OFFSET)           // Power Down module Interrupt
#define VECTOR_SERIAL   (1+SPI_OFFSET)           // Serial Interface Interrupt
#define VECTOR_NAND     (2+SPI_OFFSET)           // NAND-Flash interface interrupt
#define VECTOR_T0       (3+SPI_OFFSET)           // Timer 0
#define VECTOR_T1       (4+SPI_OFFSET)           // Timer 1
#define VECTOR_T2       (5+SPI_OFFSET)           // Timer 2
#define VECTOR_SMARTCARD (6+SPI_OFFSET)         // Smart Card Interrupt
#define VECTOR_WDT      (7+SPI_OFFSET)           // watchdog
#define VECTOR_ENET     (8+SPI_OFFSET)           // Ethernet interrupt
#define VECTOR_DTCP     (9+SPI_OFFSET)           // DCTP interrupt enable bit
#define VECTOR_MJC      (9+SPI_OFFSET)           // MJC interrupt
#define VECTOR_PSCAN    (10+SPI_OFFSET)          // PSCAN interrupt
#define VECTOR_USB1     (11+SPI_OFFSET)          // USB1 Interrupt
#define VECTOR_IMGRZ    (12+SPI_OFFSET)          // IMGRZ interrupt
#define VECTOR_GFXSC    (12+SPI_OFFSET)          // IMGRZ interrupt
#define VECTOR_VENC     (13+SPI_OFFSET)          // VENC interrupt
#define VECTOR_SPDIF    (14+SPI_OFFSET)          // Audio PSR/SPDIF interrupt
#define VECTOR_PSR      (14+SPI_OFFSET)          // Audio PSR/SPDIF interrupt
#define VECTOR_USB      (15+SPI_OFFSET)          // USB0 Interrupt
#define VECTOR_USB0     (15+SPI_OFFSET)          // USB0 Interrupt
#define VECTOR_AUDIO    (16+SPI_OFFSET)          // Audio DSP interrupt
#define VECTOR_DSP      (16+SPI_OFFSET)          // Audio DSP Interrupt
#define VECTOR_RS232    (17+SPI_OFFSET)          // RS232 Interrupt 1
#define VECTOR_LED      (18+SPI_OFFSET)          // LED interrupt
#define VECTOR_OSD      (19+SPI_OFFSET)          // OSD interrupt
#define VECTOR_VDOIN    (20+SPI_OFFSET)          // Video In interrupt (8202 side)
#define VECTOR_BLK2RS   (21+SPI_OFFSET)          // Block to Raster Interrupt
#define VECTOR_DISPLAY  (21+SPI_OFFSET)          // Block to Raster Interrupt
#define VECTOR_FLASH    (22+SPI_OFFSET)          // Serial Flash interrupt
#define VECTOR_POST_PROC (23+SPI_OFFSET)         // POST process interrupt
#define VECTOR_VDEC     (24+SPI_OFFSET)          // VDEC interrupt
#define VECTOR_GFX      (25+SPI_OFFSET)          // Graphic Interrupt
#define VECTOR_DEMUX    (26+SPI_OFFSET)          // Transport demuxer interrupt
#define VECTOR_DEMOD    (27+SPI_OFFSET)          // Demod interrupt enable bit
#define VECTOR_FCI      (28+SPI_OFFSET)          // FCI interrupt
#define VECTOR_MSDC     (28+SPI_OFFSET)          // MSDC interrupt
#define VECTOR_APWM     (29+SPI_OFFSET)          // Audio PWM Interrupt
#define VECTOR_PCMCIA   (29+SPI_OFFSET)          // PCMCIA interrupt
#define VECTOR_MISC2    (30+SPI_OFFSET)          // MISC2 interrupt
#define VECTOR_MISC     (31+SPI_OFFSET)          // MISC interrupt

#define VECTOR_MISC_BASE  (32+SPI_OFFSET)
#define VECTOR_MISC2_BASE (64+SPI_OFFSET)

//misc
#define VECTOR_DRAMC        (0+VECTOR_MISC_BASE)          // DRAM Controller interrupt
#define VECTOR_EXT1         (1+VECTOR_MISC_BASE)          // Dedicated GPIO edge interrupt
#define VECTOR_POD          (2+VECTOR_MISC_BASE)
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5389)
#define VECTOR_PMU1         (3+VECTOR_MISC_BASE)
#define VECTOR_PMU0         (4+VECTOR_MISC_BASE)
#elif defined(CONFIG_ARCH_MT5398) || defined(CONFIG_ARCH_MT5880) || defined(CONFIG_ARCH_MT5881)
#define VECTOR_PMU1         (4+VECTOR_MISC_BASE)
#define VECTOR_PMU0         (3+VECTOR_MISC_BASE)
#endif
#define VECTOR_PMU          VECTOR_PMU0                   // ARM performance monitor interrupt
#define VECTOR_DRAMC_CHB    (5+VECTOR_MISC_BASE)          // 5. DRAM Controller 1 interrupt
#define VECTOR_TVE          (6+VECTOR_MISC_BASE)          // 6. TVE interrupt
#define VECTOR_PCIE         (7+VECTOR_MISC_BASE)
#define VECTOR_APWM2        (8+VECTOR_MISC_BASE)          // 8. APWM2 interrupt
#define VECTOR_SPI          (9+VECTOR_MISC_BASE)          // 9. SPI interrupt
#define VECTOR_JPGDEC       (10+VECTOR_MISC_BASE)          // 10. Jpeg Decoder interrupt
#define VECTOR_P            (11+VECTOR_MISC_BASE)          // 11. P interrupt   //MT5395 Parser DMA is 43
#define VECTOR_AES          (12+VECTOR_MISC_BASE)          // 12. AES interrupt
#define VECTOR_UP0          (12+VECTOR_MISC_BASE)          // 12. AES 8032 interrupt 0
#define VECTOR_TCON         (13+VECTOR_MISC_BASE)          // 13. TCON
#define VECTOR_TCON_TCH     (13+VECTOR_MISC_BASE)          // 13. TCON timing control
#define VECTOR_TCON_EH      (14+VECTOR_MISC_BASE)          // 14. TCON error
#define VECTOR_OD           (15+VECTOR_MISC_BASE)          // 15. OD
#define VECTOR_USB2         (16+VECTOR_MISC_BASE)          // 16. USB2 Interrupt
#define VECTOR_USB3         (17+VECTOR_MISC_BASE)          // 17. USB3 Interrupt
#define VECTOR_GDMA         (18+VECTOR_MISC_BASE)          // 18. GDMA
#define VECTOR_EPHY         (19+VECTOR_MISC_BASE)          // 19. ethernet PHY
#define VECTOR_TCPIP        (20+VECTOR_MISC_BASE)          // 20. TCPIP checksum
#define VECTOR_GFX3D_GP     (21+VECTOR_MISC_BASE)
#define VECTOR_GFX3D_PP     (22+VECTOR_MISC_BASE)
#define VECTOR_GFX3D_PMU    (23+VECTOR_MISC_BASE)
#define VECTOR_GFX3D_GPMMU  (24+VECTOR_MISC_BASE)
#define VECTOR_GFX3D_PPMMU  (25+VECTOR_MISC_BASE)
#define VECTOR_MMU_GFX      (26+VECTOR_MISC_BASE)
#define VECTOR_MMU_GCPU     (27+VECTOR_MISC_BASE)
#define VECTOR_MMU_IMGRZ    (28+VECTOR_MISC_BASE)
#define VECTOR_MMU_JPG      (29+VECTOR_MISC_BASE)
#define VECTOR_MMU_GDMA     (30+VECTOR_MISC_BASE)
#define VECTOR_DDI          (31+VECTOR_MISC_BASE)
//misc2
#define VECTOR_LZHS         (0+VECTOR_MISC2_BASE)
#define VECTOR_L2C          (1+VECTOR_MISC2_BASE)
#define VECTOR_IMGRZ2       (2+VECTOR_MISC2_BASE)
#define VECTOR_MMU_IMGRZ2   (3+VECTOR_MISC2_BASE)
#define VECTOR_MSDC2        (4+VECTOR_MISC2_BASE)
#define VECTOR_PNG1         (5+VECTOR_MISC2_BASE)
#define VECTOR_PNG2         (6+VECTOR_MISC2_BASE)
#define VECTOR_PNG3         (7+VECTOR_MISC2_BASE)
#define VECTOR_MMU_PNG1     (8+VECTOR_MISC2_BASE)
#define VECTOR_UART_DMA     (16+VECTOR_MISC2_BASE)

#ifdef CONFIG_PCI
/* Put the all logical pci-express interrupts behind the normal interrupt. */
#define IRQ_PCIE_INTA	(50+SPI_OFFSET)
#define IRQ_PCIE_INTB	(51+SPI_OFFSET)
#define IRQ_PCIE_INTC	(52+SPI_OFFSET)
#define IRQ_PCIE_INTD	(53+SPI_OFFSET)
#define IRQ_PCIE_AER	(54+SPI_OFFSET)
#ifdef CONFIG_PCI_MSI
#define IRQ_PCIE_MSI	        (IRQ_PCIE_AER + 1)
#define IRQ_PCIE_MSI_END	(IRQ_PCIE_AER + 32)
#endif
#endif

#define _IRQ(v)         (1U << (v-SPI_OFFSET)) // IRQ bit by vector
#define _MISCIRQ(v)     (1U << (v-VECTOR_MISC_BASE))    // MISC IRQ bit by vector
#define _MISC2IRQ(v)     (1U << (v-VECTOR_MISC2_BASE))    // MISC2 IRQ bit by vector
#endif /* __USE_XBIM_IRQS */

#endif /*IRQS_H*/
