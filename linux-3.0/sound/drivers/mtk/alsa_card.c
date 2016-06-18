/*
* linux/sound/drivers/mtk/alsa_card.c
*
* MTK Sound Card Driver
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

//#define MT85XX_DEFAULT_CODE

#ifdef MT85XX_DEFAULT_CODE
#include "x_module.h"
#include "x_rm.h"
#include "x_printf.h"
#endif

#include <linux/init.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/moduleparam.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/tlv.h>
#include <sound/pcm.h>
#include <sound/rawmidi.h>
#include <sound/initval.h>

#include "alsa_pcm.h"

#ifdef MT85XX_DEFAULT_CODE
#define MAX_PCM_DEVICES     4
#define MAX_PCM_SUBSTREAMS  8
#else
#define MAX_PCM_DEVICES     1
#define MAX_PCM_SUBSTREAMS  2
#endif

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;  /* Index 0-MAX */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;   /* ID for this card */
static int enable[SNDRV_CARDS] = {1, [1 ... (SNDRV_CARDS - 1)] = 0};
static int pcm_devs[SNDRV_CARDS] = {[0 ... (SNDRV_CARDS - 1)] = 1};

static struct platform_device *devices[SNDRV_CARDS];


static int __devinit snd_mt85xx_probe(struct platform_device *devptr)
{
    struct snd_card *card;
    struct snd_mt85xx *mt85xx;
    int idx, err;
    int dev = devptr->id;

    printk("[ALSA] probe: devptr->id = %d\n", devptr->id);

    err = snd_card_create(index[dev], id[dev], THIS_MODULE,
                  sizeof(struct snd_mt85xx), &card);
    if (err < 0)
        return err;

    mt85xx = card->private_data;
    mt85xx->card = card;

    for (idx = 0; idx < MAX_PCM_DEVICES && idx < pcm_devs[dev]; idx++)
    {
        if ((err = snd_card_mt85xx_pcm(mt85xx, idx, MAX_PCM_SUBSTREAMS)) < 0)
            goto __nodev;
    }

    //if ((err = snd_card_mt85xx_new_mixer(dummy)) < 0)
    //    goto __nodev;

#ifdef MT85XX_DEFAULT_CODE
    strcpy(card->driver, "mt85xx");
    strcpy(card->shortname, "mt85xx");
    sprintf(card->longname, "mt85xx %i", dev + 1);
#else
    strcpy(card->driver, "mtk");
    strcpy(card->shortname, "mtk");
    sprintf(card->longname, "mtk %i", dev + 1);
#endif

    snd_card_set_dev(card, &devptr->dev);

    if ((err = snd_card_register(card)) == 0) {
        platform_set_drvdata(devptr, card);
        return 0;
    }

__nodev:
    snd_card_free(card);
    return err;
}

static int __devexit snd_mt85xx_remove(struct platform_device *devptr)
{
    snd_card_free(platform_get_drvdata(devptr));
    platform_set_drvdata(devptr, NULL);
    return 0;
}

#define SND_MT85XX_DRIVER    "snd_mt85xx"

static struct platform_driver snd_mt85xx_driver = {
    .probe      = snd_mt85xx_probe,
    .remove     = __devexit_p(snd_mt85xx_remove),

    .driver     = {
        .name   = SND_MT85XX_DRIVER
    },
};

static void snd_mt85xx_unregister_all(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(devices); ++i)
        platform_device_unregister(devices[i]);
    platform_driver_unregister(&snd_mt85xx_driver);
}

static int __init alsa_card_mt85xx_init(void)
{
    int i, cards, err;

    if ((err = platform_driver_register(&snd_mt85xx_driver)) < 0)
        return err;

    cards = 0;
    for (i = 0; i < SNDRV_CARDS; i++) {
        struct platform_device *device;
        if (! enable[i])
            continue;
        device = platform_device_register_simple(SND_MT85XX_DRIVER,
                             i, NULL, 0);
        if (IS_ERR(device))
            continue;
        if (!platform_get_drvdata(device)) {
            platform_device_unregister(device);
            continue;
        }
        devices[i] = device;
        cards++;
    }

    if (!cards) {
    #ifdef MODULE
        printk(KERN_ERR "mt85xx soundcard not found or device busy\n");
    #endif
        snd_mt85xx_unregister_all();
        return -ENODEV;
    }

    return 0;
}

static void __exit alsa_card_mt85xx_exit(void)
{
    snd_mt85xx_unregister_all();
}

static int __init alsa_mt85xx_init(void)
{
    printk(KERN_ERR "[ALSA] alsa_mt85xx_init().\n");

    alsa_card_mt85xx_init();
    return 0;
}

static void __exit alsa_mt85xx_exit(void)
{
    printk(KERN_ERR "[ALSA] alsa_mt85xx_exit().\n");

    alsa_card_mt85xx_exit();
}

#ifdef MT85XX_DEFAULT_CODE
DECLARE_MODULE(alsa_mt85xx);
#endif

MODULE_DESCRIPTION("mt85xx soundcard");
MODULE_LICENSE("GPL");

#ifndef MT85XX_DEFAULT_CODE
module_init(alsa_mt85xx_init)
module_exit(alsa_mt85xx_exit)
#endif

