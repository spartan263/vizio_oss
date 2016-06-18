/*
* linux/sound/drivers/mtk/alsa_pcm.h
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

#ifndef _ALSA_PCM_H_
#define _ALSA_PCM_H_

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

struct snd_mt85xx {
    struct snd_card *card;
    struct snd_pcm *pcm;
};

#define TIMER_FIRST   0
#define TIMER_NORMAL  1
#define TIMER_LAST    2

struct snd_mt85xx_pcm {
    spinlock_t                  lock;
    struct timer_list           timer;
#ifndef MT85XX_DEFAULT_CODE //2012/12/7 added by daniel
    unsigned int                timer_started;
#endif
    unsigned int                instance;
    unsigned int                pcm_buffer_size;
    unsigned int                pcm_period_size;
    unsigned int                pcm_rptr; // byte offset
    unsigned int                bytes_elapsed;   
    struct snd_pcm_substream   *substream;
};

extern int __devinit snd_card_mt85xx_pcm(struct snd_mt85xx *mt85xx, int device, int substreams);

#endif  // _ALSA_PCM_H_
