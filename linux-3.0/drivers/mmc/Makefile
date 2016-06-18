#
# Makefile for the kernel mmc device drivers.
#

# Enable warning to error.
subdir-ccflags-y += -Werror

subdir-ccflags-$(CONFIG_MMC_DEBUG) := -DDEBUG

obj-$(CONFIG_MMC)		+= core/
obj-$(CONFIG_MMC)		+= card/
obj-$(CONFIG_MMC)		+= host/

