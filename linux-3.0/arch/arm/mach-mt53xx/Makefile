#
# Makefile for the linux kernel.
#

# Enable warning to error.
subdir-ccflags-y += -Werror

AFLAGS_swsusp.o                 := -DTEXT_OFFSET=$(TEXT_OFFSET)

# Common support
obj-y := core.o
obj-m :=
obj-n :=
obj-  :=

obj-$(CONFIG_TV_DRV_VFY) += cache_operation.o
# mt53xx device support
obj-$(CONFIG_ARCH_MT5396) += timer.o
obj-$(CONFIG_ARCH_MT5368) += timer.o
obj-$(CONFIG_ARCH_MT5389) += timer.o
obj-$(CONFIG_ARCH_MT5398) += timer.o
obj-$(CONFIG_ARCH_MT5880) += timer.o
obj-$(CONFIG_ARCH_MT5881) += timer.o
obj-$(CONFIG_SMP)	+= platsmp.o headsmp.o
obj-$(CONFIG_MT53XX_UP_CHECK)	+= platsmp.o headsmp.o
obj-$(CONFIG_LOCAL_TIMERS)	+= localtimer.o
obj-$(CONFIG_PCI) += mtk_pci.o mtk_pciproc.o
obj-$(CONFIG_PCI_MSI) += mtk_pcimsi.o

obj-$(CONFIG_MT53XX_NATIVE_GPIO)+= gpio6896.o pdwnc_gpio6896.o
obj-$(CONFIG_HOTPLUG_CPU)       += hotplug.o

# Power Management
obj-$(CONFIG_PM)                += pm.o sleep.o suspend.o
obj-$(CONFIG_HIBERNATION)       += cpu.o swsusp.o

ifeq "${ANDROID}" "true"
    EXTRA_CFLAGS += -DANDROID
endif
