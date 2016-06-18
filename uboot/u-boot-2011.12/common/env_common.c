/*
 * (C) Copyright 2000-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <search.h>
#include <errno.h>
#include <malloc.h>

#define CC_UBOOT_TRANSFER_USBGPIO_KERNEL

#define CC_UBOOT_TRANSFER_MSDCGPIO_KERNEL
#include "drvcust_if.h"

DECLARE_GLOBAL_DATA_PTR;

/************************************************************************
 * Default settings to be used when no valid environment is found
 */
#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

const uchar default_environment[] = {
#ifdef	CONFIG_BOOTARGS
	"bootargs="	CONFIG_BOOTARGS			"\0"
#endif
#ifdef	CONFIG_BOOTCOMMAND
	"bootcmd="	CONFIG_BOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_RAMBOOTCOMMAND
	"ramboot="	CONFIG_RAMBOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_NFSBOOTCOMMAND
	"nfsboot="	CONFIG_NFSBOOTCOMMAND		"\0"
#endif
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	"bootdelay="	MK_STR(CONFIG_BOOTDELAY)	"\0"
#endif
#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
	"baudrate="	MK_STR(CONFIG_BAUDRATE)		"\0"
#endif
#ifdef	CONFIG_LOADS_ECHO
	"loads_echo="	MK_STR(CONFIG_LOADS_ECHO)	"\0"
#endif
#ifdef	CONFIG_ETHADDR
	"ethaddr="	MK_STR(CONFIG_ETHADDR)		"\0"
#endif
#ifdef	CONFIG_ETH1ADDR
	"eth1addr="	MK_STR(CONFIG_ETH1ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH2ADDR
	"eth2addr="	MK_STR(CONFIG_ETH2ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH3ADDR
	"eth3addr="	MK_STR(CONFIG_ETH3ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH4ADDR
	"eth4addr="	MK_STR(CONFIG_ETH4ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH5ADDR
	"eth5addr="	MK_STR(CONFIG_ETH5ADDR)		"\0"
#endif
#ifdef	CONFIG_IPADDR
	"ipaddr="	MK_STR(CONFIG_IPADDR)		"\0"
#endif
#ifdef	CONFIG_SERVERIP
	"serverip="	MK_STR(CONFIG_SERVERIP)		"\0"
#endif
#ifdef	CONFIG_SYS_AUTOLOAD
	"autoload="	CONFIG_SYS_AUTOLOAD		"\0"
#endif
#ifdef	CONFIG_PREBOOT
	"preboot="	CONFIG_PREBOOT			"\0"
#endif
#ifdef	CONFIG_ROOTPATH
	"rootpath="	CONFIG_ROOTPATH			"\0"
#endif
#ifdef	CONFIG_GATEWAYIP
	"gatewayip="	MK_STR(CONFIG_GATEWAYIP)	"\0"
#endif
#ifdef	CONFIG_NETMASK
	"netmask="	MK_STR(CONFIG_NETMASK)		"\0"
#endif
#ifdef	CONFIG_HOSTNAME
	"hostname="	MK_STR(CONFIG_HOSTNAME)		"\0"
#endif
#ifdef	CONFIG_BOOTFILE
	"bootfile="	CONFIG_BOOTFILE			"\0"
#endif
#ifdef	CONFIG_LOADADDR
	"loadaddr="	MK_STR(CONFIG_LOADADDR)		"\0"
#endif
#ifdef	CONFIG_CLOCKS_IN_MHZ
	"clocks_in_mhz=1\0"
#endif
#if defined(CONFIG_PCI_BOOTDELAY) && (CONFIG_PCI_BOOTDELAY > 0)
	"pcidelay="	MK_STR(CONFIG_PCI_BOOTDELAY)	"\0"
#endif
#ifdef	CONFIG_EXTRA_ENV_SETTINGS
	CONFIG_EXTRA_ENV_SETTINGS
#endif
	"\0"
};
extern char mtdparts_default[];

struct hsearch_data env_htab;
int default_environment_size = sizeof(default_environment);
static unsigned int kmemsize = 0;
static unsigned int kmem2start = 0;
static unsigned int kmem2size = 0;

static uchar env_get_char_init(int index)
{
	/* if crc was bad, use the default environment */
	if (gd->env_valid)
		return env_get_char_spec(index);
	else
		return default_environment[index];
}

uchar env_get_char_memory(int index)
{
	return *env_get_addr(index);
}

uchar env_get_char(int index)
{
	/* if relocated to RAM */
	if (gd->flags & GD_FLG_RELOC)
		return env_get_char_memory(index);
	else
		return env_get_char_init(index);
}

const uchar *env_get_addr(int index)
{
	if (gd->env_valid)
		return (uchar *)(gd->env_addr + index);
	else
		return &default_environment[index];
}

#if defined(CC_FAST_INIT) || defined(CC_ENABLE_PRODUCT_ID) || defined(CC_MT53XX_SUPPORT_2G_DRAM) || defined(CC_ENABLE_MALI_CORE_NUM) || defined(CC_UBOOT_TRANSFER_USBGPIO_KERNEL) || defined(LPJ_IC_VER1) || defined(LPJ_IC_VER2)
static void env_add_bootargs(char* oldstr, char* newstr)
{
    char *p1 = NULL, *p2 = NULL;
    char *bootargs = NULL;

    // generate bootargs string
    p1 = getenv("bootargs");

    if (p1 == NULL)
    {
        return;
    }

    bootargs = (char *)malloc(default_environment_size);
    memset(bootargs, 0, default_environment_size);
    strcpy(bootargs, p1);

    // delete any existing string in bootargs string.
    p1 = strstr(bootargs, oldstr);
    if (p1)
    {
        p2 = strstr(p1, " ");
        if(p2)
        {
            while(*++p2)
            {
                *p1++ = *p2;
            }

            *p1++ = ' ';
        }

        *p1 = '\0';
    }
    else
    {
        strcat(bootargs, " ");
    }

    // append new string at the end of bootargs string.
    strcat(bootargs, newstr);

    // update bootargs environment
    setenv("bootargs", bootargs);

    free(bootargs);
}
#endif

#ifdef CC_ENABLE_MTK_MODEL_INDEX
extern unsigned int GetCurrentPanelIndex(void);
#endif
static void env_add_panel_index(void)
{
    char newstr[32] = {0};

#ifdef CC_ENABLE_MTK_MODEL_INDEX
    sprintf(newstr, "modelindex_id=%d", GetCurrentPanelIndex());
#else
    sprintf(newstr, "modelindex_id=%d", GetModelIndexFromEEPROM());
#endif // CC_ENABLE_MTK_MODEL_INDEX
    env_add_bootargs("modelindex_id=", newstr);
}


#ifdef CC_ENABLE_MALI_CORE_NUM
extern unsigned int Efuse_MaliMaliCoreNum(void);
static void env_add_mali_core_num(void)
{
    char newstr[32] = {0};

    sprintf(newstr, "mali_core_num=%d", Efuse_MaliMaliCoreNum());
    env_add_bootargs("mali_core_num=", newstr);
}
#endif

#ifdef CC_UBOOT_TRANSFER_USBGPIO_KERNEL
#define MUSB_MAX_CONTROLLERS 4

static void env_add_usb_gpio(void)
{
	int i;
	int u4UsbPortConfig;
	char pstr[32];
	int _i4usbportusing[MUSB_MAX_CONTROLLERS] ={eUSB0Config,eUSB1Config,eUSB2Config,eUSB3Config};
	int _i4usbpwrgpio[MUSB_MAX_CONTROLLERS] ={eUSBVbus0Gpio,eUSBVbus1Gpio,eUSBVbus2Gpio,eUSBVbus3Gpio};
	int _i4usbpwrpolarity[MUSB_MAX_CONTROLLERS] ={eUSBVbus0EnablePolarity,eUSBVbus1EnablePolarity,eUSBVbus2EnablePolarity,eUSBVbus3EnablePolarity};
	int _i4usbocgpio[MUSB_MAX_CONTROLLERS] ={eUSB0OCGpio,eUSB1OCGpio,eUSB2OCGpio,eUSB3OCGpio};
	int _i4usbocgpiopolarity[MUSB_MAX_CONTROLLERS] ={eUSB0OCEnablePolarity,eUSB1OCEnablePolarity,eUSB2OCEnablePolarity,eUSB3OCEnablePolarity};
	
	char usbportusing[32] = "usbportusing=";
	char usbpwrgpio[32] = "usbpwrgpio=";
	char usbpwrpolarity[32] = "usbpwrpolarity=";
	char usbocgpio[32] = "usbocgpio=";
	char usbocgpiopolarity[32] = "usbocgpiopolarity=";

	
    for(i = 0; i < MUSB_MAX_CONTROLLERS; i++)
    	{
    		if(0 == DRVCUST_InitQuery(_i4usbportusing[i], &u4UsbPortConfig))
    			{
            		sprintf(pstr,"%d",u4UsbPortConfig);
            		strcat(usbportusing,pstr);
    			}
			else
				{
					strcat(usbportusing,"-1");
				}
				
    		if( 0 ==DRVCUST_InitQuery(_i4usbpwrgpio[i], &u4UsbPortConfig))
    			{
					sprintf(pstr,"%d",u4UsbPortConfig);
					strcat(usbpwrgpio,pstr );
    			}
			else
				{
					strcat(usbpwrgpio,"-1");
				}

			if( 0==DRVCUST_InitQuery(_i4usbpwrpolarity[i], &u4UsbPortConfig))
				{
					sprintf(pstr,"%d",u4UsbPortConfig);
					strcat(usbpwrpolarity,pstr);
				}
			else
				{
					strcat(usbpwrpolarity,"-1");
				}

			if ( 0==DRVCUST_InitQuery(_i4usbocgpio[i], &u4UsbPortConfig))
				{
					sprintf(pstr,"%d",u4UsbPortConfig);
					strcat(usbocgpio,pstr);
				}
			else
				{
					strcat(usbocgpio,"-1");
				}

			if(0 ==DRVCUST_InitQuery(_i4usbocgpiopolarity[i], &u4UsbPortConfig))
				{
					sprintf(pstr,"%d",u4UsbPortConfig);
					strcat(usbocgpiopolarity,pstr);
				}
			
			else
				{
					strcat(usbocgpiopolarity,"-1");
				}
			if(i !=MUSB_MAX_CONTROLLERS-1)
				{
				
					strcat(usbportusing,",");
					strcat(usbpwrgpio,",");
					strcat(usbpwrpolarity,",");
					strcat(usbocgpio,",");
					strcat(usbocgpiopolarity,",");
					
				}
			
    	}

	

	//printf("%s,\n",	usbportusing);
	//printf("%s,\n",	usbpwrgpio);
	//printf("%s,\n",	usbpwrpolarity);
	//printf("%s,\n",	usbocgpio);
	//printf("%s,\n",	usbocgpiopolarity);

    env_add_bootargs("usbportusing=", usbportusing);
    env_add_bootargs("usbpwrgpio=", usbpwrgpio);
    env_add_bootargs("usbpwrpolarity=", usbpwrpolarity);
    env_add_bootargs("usbocgpio=", usbocgpio);
    env_add_bootargs("usbocgpiopolarity=", usbocgpiopolarity); 
    /* for cdc support, default support usb port0 */
    env_add_bootargs("usbcdcsupport=", "usbcdcsupport=1,0,0,0");

}
#endif
#ifdef CC_UBOOT_TRANSFER_MSDCGPIO_KERNEL
#define MSDC_GPIO_MAX_NUMBERS 6

static void env_add_msdc_gpio(void)
{
	int i;
	int u4MsdcGpioConfig;
	char pstr[32];
	int _i4MSDCgpio[MSDC_GPIO_MAX_NUMBERS]={eMSDC0DetectGpio,eMSDC0WriteProtectGpio,eMSDC0PoweronoffDetectGpio,eMSDC0VoltageSwitchGpio,eMSDCbackup1Gpio,eMSDCbackup2Gpio};
    
	char msdcgpio[32] = "msdcgpio=";

    for(i = 0; i < MSDC_GPIO_MAX_NUMBERS; i++)
    {
		if( 0 ==DRVCUST_InitQuery(_i4MSDCgpio[i], &u4MsdcGpioConfig))
		{
			sprintf(pstr,"%d",u4MsdcGpioConfig);
			strcat(msdcgpio,pstr );
		}
		else
		{
			strcat(msdcgpio,"-1");
		}
	
		if(i !=MSDC_GPIO_MAX_NUMBERS-1)
		{
			strcat(msdcgpio,",");	
		}			
    }
	printf("tyler.zhang test msdcgpio%s,\n",msdcgpio);
    env_add_bootargs("msdcgpio=",msdcgpio);
}
#endif


extern int DRVCUST_QueryPart(int i, int *pData, int type);
extern int DRVCUST_QueryPart64(int i, u64 *pData, int type);

static void set_cust_env(void)
{
#ifndef CC_KERNEL_ON_NOR
    int i = 0, i4PartNum = 0, encrypt = 0;
    u64 size = 0;
    char *p1 = NULL, *p2 = NULL;
    char *name = NULL;
    char buf[64] = {0};
    char *mtdparts = NULL;
    char *bootargs = NULL;

    mtdparts = (char *)malloc(default_environment_size);
    memset(mtdparts, 0, default_environment_size);

    bootargs = (char *)malloc(default_environment_size);
    memset(bootargs, 0, default_environment_size);

    DRVCUST_QueryPart(0, &i4PartNum, 0);

    // generate mtdparts string
    while (i <= i4PartNum)
    {
        DRVCUST_QueryPart64(i, &size, 1);
        DRVCUST_QueryPart(i, (int *)&name, 2);
        DRVCUST_QueryPart(i, &encrypt, 3);

        if (!size)
        {
            break;
        }

        if (i == 0)
        {
#if defined(CC_EMMC_BOOT)
            strcat(mtdparts, "mt53xx-emmc:");
#else
            strcat(mtdparts, "mt53xx-nand:");
#endif
        }
        else
        {
            strcat(mtdparts, ",");
        }

		/* CAUTION: 64 bit size limition 2TB !!! */
        if ((size % (1024 * 1024)) == 0)
        {
            sprintf(buf, "%dM(%s)", (u32)(size /1024 /1024), name);
        }
        else if ((size % 1024) == 0)
        {
            sprintf(buf, "%dk(%s)", (u32)(size /1024), name);
        }
        else
        {
            BUG();
        }

        strcat(mtdparts, buf);

        if (encrypt)
        {
            strcat(mtdparts, "enc");
        }

        i++;
    }

    strcpy(mtdparts_default, mtdparts);

    // update mtdparts string
    setenv("mtdparts", mtdparts);

    // generate bootargs string
    p1 = getenv("bootargs");
    if (p1 == NULL)
    {
        free(mtdparts);
        free(bootargs);
        return;
    }

    strcpy(bootargs, p1);

    // delete any existing mtdparts string in bootargs string.
    p1 = strstr(bootargs, "mtdparts=");
    if (p1)
    {
        p2 = strstr(p1, " ");
        if(p2)
        {
            while(*++p2)
            {
                *p1++ = *p2;
            }

            *p1++ = ' ';
        }

        *p1 = '\0';
    }
    else
    {
        strcat(bootargs, " ");
    }

    // append new mtdparts string at the end of bootargs string.
    strcat(bootargs, "mtdparts=");
    strcat(bootargs, mtdparts);

    // update bootargs environment
    setenv("bootargs", bootargs);

    // set kernel memory size
    if (kmemsize)
    {
        sprintf(buf, "0x%x", kmemsize);
        setenv("kmemsize", buf);
#if CONFIG_NR_DRAM_BANKS>1
        sprintf(buf, "0x%x", kmem2start);
        setenv("kmem2start", buf);
        sprintf(buf, "0x%x", kmem2size);
        setenv("kmem2size", buf);
#endif
    }
    if (getenv("kmemsize")==NULL)
    {
        sprintf(buf, "0x%x", gd->bd->bi_dram[0].size);
        setenv("kmemsize", buf);
    }

    free(mtdparts);
    free(bootargs);
#endif
}

void set_default_env(const char *s)
{
    unsigned int* penv;
	if (sizeof(default_environment) > ENV_SIZE) {
		puts("*** Error - default environment is too large\n\n");
		return;
	}

	if (s) {
		if (*s == '!') {
			printf("*** Warning - %s, "
				"using default environment\n\n",
				s + 1);
		} else {
			puts(s);
		}
	} else {
		puts("Using default environment\n\n");
	}
		penv = (unsigned int*)env_ptr;
		//get ATAG kernel memory from env.o
		if (penv[0]==0x464c457f)
		{
			int i=1;
			for (; i<0x40; i++)
			{
				// find magic key, the next work is global_kernel_dram_size
				if (penv[i] == 0x53965368)
				{
					kmemsize = penv[i+1];
#if CONFIG_NR_DRAM_BANKS>1
					kmem2start = penv[i+2];
					kmem2size = penv[i+3];
#endif
					printf("set_default_env, kmemsize=0x%x, kmem2start=0x%x, kmem2size=0x%x\n", kmemsize, kmem2start, kmem2size);
					break;
				}
			}
		}

	if (himport_r(&env_htab, (char *)default_environment,
			sizeof(default_environment), '\0', 0) == 0)
		error("Environment import failed: errno = %d\n", errno);

	gd->flags |= GD_FLG_ENV_READY;
	
	set_cust_env();
    saveenv();
}

/*
 * Check if CRC is valid and (if yes) import the environment.
 * Note that "buf" may or may not be aligned.
 */
int env_import(const char *buf, int check)
{
	env_t *ep = (env_t *)buf;

	if (check) {
		uint32_t crc;

		memcpy(&crc, &ep->crc, sizeof(crc));

		if (crc32(0, ep->data, ENV_SIZE) != crc) {
			set_default_env("!bad CRC");
			return 0;
		}
	}

	if (himport_r(&env_htab, (char *)ep->data, ENV_SIZE, '\0', 0)) {
		gd->flags |= GD_FLG_ENV_READY;
		return 1;
	}

	error("Cannot import environment: errno = %d\n", errno);

	set_default_env("!import failed");

	return 0;
}
#if defined(CC_SECURE_ROM_BOOT)
int IsRunOnUsb(char* uenv, int uenv_size);

static void env_add_usig(void)
{
    char *p1 = NULL;
    char uenv[125];

    if (IsRunOnUsb((char *)&uenv, sizeof(uenv)))
    {
        // generate bootargs string
        p1 = getenv("bootargs");
        if (p1 == NULL)
        {
            return;
        }
        printf("run_on_usb, uenv=%s\n", uenv);
        // append new string at the end of bootargs string.
        strcat(p1, " ");
        strcat(p1, uenv);
        // update bootargs environment
        setenv("bootargs", p1);
        setenv("usigenv", uenv);
    }

}
#endif // CC_SECURE_ROM_BOOT

void env_relocate(void)
{
#if defined(CONFIG_NEEDS_MANUAL_RELOC)
	env_reloc();
#endif
	if (gd->env_valid == 0) {
#if defined(CONFIG_ENV_IS_NOWHERE)	/* Environment not changable */
		set_default_env(NULL);
#else
		show_boot_progress(-60);
		set_default_env("!bad CRC");
#endif
	} else {
		env_relocate_spec();
#ifndef CC_KERNEL_ON_NOR
	{
		char *p1 = NULL;

		p1 = getenv("mtdparts");
		if (p1 == NULL)
		{
			return;
		}

		strcpy(mtdparts_default, p1);
	}
#endif
	}
#ifdef CC_ENABLE_MTK_MODEL_INDEX 
    env_add_panel_index();
#endif

#ifdef CC_ENABLE_MALI_CORE_NUM
    env_add_mali_core_num(); 
#endif

#ifdef CC_UBOOT_TRANSFER_USBGPIO_KERNEL
    env_add_usb_gpio(); 
#endif

#ifdef CC_UBOOT_TRANSFER_MSDCGPIO_KERNEL
    env_add_msdc_gpio(); 
#endif
#ifdef CC_FAST_INIT
    env_add_fastboot();
#endif
#ifdef CC_MT53XX_SUPPORT_2G_DRAM
    //env_add_vmalloc_cust();
#endif
#ifdef CC_SECURE_ROM_BOOT
    env_add_usig();
#endif
#ifdef CONFIG_AMIGAONEG3SE
    disable_nvram();
#endif

	#ifdef LPJ_IC_VER1
	if(BSP_GetIcVersion() == LPJ_IC_VER1)
		{
			env_add_bootargs("lpj=", LPJ_IC_VALUE1);
		}
	#endif

	#ifdef LPJ_IC_VER1
		if(BSP_GetIcVersion() == LPJ_IC_VER2)
		{
			env_add_bootargs("lpj=", LPJ_IC_VALUE2);
		}
	#endif

}

#ifdef CONFIG_AUTO_COMPLETE
int env_complete(char *var, int maxv, char *cmdv[], int bufsz, char *buf)
{
	ENTRY *match;
	int found, idx;

	idx = 0;
	found = 0;
	cmdv[0] = NULL;

	while ((idx = hmatch_r(var, idx, &match, &env_htab))) {
		int vallen = strlen(match->key) + 1;

		if (found >= maxv - 2 || bufsz < vallen)
			break;

		cmdv[found++] = buf;
		memcpy(buf, match->key, vallen);
		buf += vallen;
		bufsz -= vallen;
	}

	qsort(cmdv, found, sizeof(cmdv[0]), strcmp_compar);

	if (idx)
		cmdv[found++] = "...";

	cmdv[found] = NULL;
	return found;
}
#endif
