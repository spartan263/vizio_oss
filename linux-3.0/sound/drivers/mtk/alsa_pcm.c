/*
* linux/sound/drivers/mtk/alsa_pcm.c
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
 
/*
 *  mt85xx sound card based on streaming button sound
 */

//#define MT85XX_DEFAULT_CODE

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

#ifdef MT85XX_DEFAULT_CODE
#include "x_typedef.h"
#include "x_os.h"
#include "x_printf.h"
#include "x_assert.h"

#include "x_aud_dec.h"
#include "DspInc.h"
#include "drv_aud.h"
#else
//COMMON
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef signed int INT32;
typedef bool BOOL;
//PLAYBACK
extern void AUD_InitALSAPlayback_MixSnd(UINT8 u1StreamId);
extern void AUD_DeInitALSAPlayback_MixSnd(UINT8 u1StreamId);
extern UINT32 AUD_GetMixSndFIFOStart(UINT8 u1StreamId);
extern UINT32 AUD_GetMixSndFIFOEnd(UINT8 u1StreamId);
extern UINT32 AUD_GetMixSndReadPtr(UINT8 u1StreamId);
extern void AUD_SetMixSndWritePtr(UINT8 u1StreamId, UINT32 u4WritePtr);
extern void AUD_PlayMixSndRingFifo(UINT8 u1StreamId, UINT32 u4SampleRate, UINT8 u1StereoOnOff, UINT8 u1BitDepth, UINT32 u4BufferSize);
#endif

#include "alsa_pcm.h"

#ifdef MT85XX_DEFAULT_CODE
#define MAX_BUFFER_SIZE     (128 * 1024)  // streaming button sound buffer size = 128KB
#define MAX_PERIOD_SIZE     MAX_BUFFER_SIZE
#else
#define MAX_BUFFER_SIZE     (16 * 1024)
#define MAX_PERIOD_SIZE     (MAX_BUFFER_SIZE/4)
#define MIN_PERIOD_SIZE     (MAX_BUFFER_SIZE/4)
#endif

#ifdef MT85XX_DEFAULT_CODE
#define USE_FORMATS         (SNDRV_PCM_FMTBIT_S16_BE)  // button sound only support 16bit, big-endian
#define USE_RATE            SNDRV_PCM_RATE_48000
#define USE_RATE_MIN        48000
#else
#define USE_FORMATS         (SNDRV_PCM_FMTBIT_S16_LE)  // DTV mixsnd engine supports 16bit, little-endian
#define USE_RATE            (SNDRV_PCM_RATE_CONTINUOUS | SNDRV_PCM_RATE_8000_48000) //SNDRV_PCM_RATE_48000
#define USE_RATE_MIN		8000
#endif
#define USE_RATE_MAX        48000

#ifdef MT85XX_DEFAULT_CODE
#define USE_CHANNELS_MIN    1
#else
#define USE_CHANNELS_MIN    2
#endif
#define USE_CHANNELS_MAX    2

#ifdef MT85XX_DEFAULT_CODE
#define USE_PERIODS_MIN     1
#define USE_PERIODS_MAX     1024
#else
#define USE_PERIODS_MIN     4
#define USE_PERIODS_MAX     4
#endif

static int snd_card_mt85xx_pcm_playback_prepare(struct snd_pcm_substream *substream)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct snd_mt85xx_pcm *pcm = runtime->private_data;

#ifdef MT85XX_DEFAULT_CODE
    AUD_DEC_STRM_BUF_INFO_T rStrmBufInfo;
    AUD_DEC_CHANNEL_TYPE_T eChType = AUD_DEC_CHANNEL_TYPE_MONO;
#endif    

    printk("[ALSA] operator: prepare\n");

    pcm->pcm_buffer_size = snd_pcm_lib_buffer_bytes(substream);
    pcm->pcm_period_size = snd_pcm_lib_period_bytes(substream);
    pcm->pcm_rptr = 0;
    pcm->bytes_elapsed = 0;

    // Step 1: get button sound buffer parameters
    // the parameters are determined by audio driver
#ifdef MT85XX_DEFAULT_CODE
    i4AudGetBtnSndStrmBufInfo(pcm->instance, &rStrmBufInfo, FALSE);
#endif

#ifdef MT85XX_DEFAULT_CODE
    runtime->dma_area  = (unsigned char *)rStrmBufInfo.u4EffsndStrmBufSA;
#else
    runtime->dma_area  = (unsigned char *)AUD_GetMixSndFIFOStart(substream->number);
#endif
    runtime->dma_addr  = 0;
    runtime->dma_bytes = pcm->pcm_buffer_size;

    printk("[ALSA] pcm->pcm_buffer_size = %d (bytes)\n", pcm->pcm_buffer_size);
    printk("[ALSA] pcm->pcm_period_size = %d (bytes)\n", pcm->pcm_period_size);

    printk("[ALSA] runtime->dma_area  = 0x%X\n", (unsigned int)runtime->dma_area);
    printk("[ALSA] runtime->dma_bytes = 0x%X\n", runtime->dma_bytes);
    printk("[ALSA] runtime->rate      = %d\n", runtime->rate);
    printk("[ALSA] runtime->format    = %d (bitwidth = %d)\n", runtime->format, snd_pcm_format_width(runtime->format));
    printk("[ALSA] runtime->channels  = %d\n", runtime->channels);
    printk("[ALSA] runtime->delay     = %d (frames)\n", (int)runtime->delay);
    printk("[ALSA] runtime->start_threshold     = %d (frames)\n", (int)runtime->start_threshold);

#ifdef MT85XX_DEFAULT_CODE
    if (pcm->pcm_buffer_size > rStrmBufInfo.u4EffsndStrmBufSize)
#else
    if (pcm->pcm_buffer_size > (AUD_GetMixSndFIFOEnd(substream->number) - AUD_GetMixSndFIFOStart(substream->number)))
#endif        
    {
        // buffer size must match
#ifdef MT85XX_DEFAULT_CODE        
        ASSERT(0);
#endif
        return -EINVAL;
    }

    // init to silence
    snd_pcm_format_set_silence(runtime->format,
                               runtime->dma_area,
                               bytes_to_samples(runtime, runtime->dma_bytes));

    // setup button sound parameters

#ifdef MT85XX_DEFAULT_CODE    
    // Step 2: setup button sound buffer DRAM
    i4AudSetBtnSndAFifo(pcm->instance,
                        (UINT32*)runtime->dma_area,
                        runtime->dma_bytes,
                        FALSE);

    // Step 3: setup sample info DRAM
    switch (runtime->channels)
    {
    case 1:
        eChType = AUD_DEC_CHANNEL_TYPE_MONO;
        break;
    case 2:
        eChType = AUD_DEC_CHANNEL_TYPE_STEREO;
        break;
    default:
        ASSERT(0);
        break;
    }

    i4AudSetBtnSndDecInfo(pcm->instance,
                          runtime->dma_bytes,
                          (UINT8)eChType,
                          runtime->rate,
                          snd_pcm_format_width(runtime->format) | 0x80); // must raise bit[7] for variable length button sound

    i4AudSetBtnSndMixGain(pcm->instance);
#else
    AUD_PlayMixSndRingFifo(substream->number, runtime->rate, 1, 16, pcm->pcm_buffer_size);
#endif

    return 0;
}

static void snd_card_mt85xx_pcm_playback_timer_start(struct snd_mt85xx_pcm *pcm)
{
#ifndef MT85XX_DEFAULT_CODE //2012/12/7 added by daniel
    if (pcm->timer_started)
    {
        del_timer_sync(&pcm->timer);
        pcm->timer_started = 0;
    }
#endif

#ifdef MT85XX_DEFAULT_CODE
    pcm->timer.expires = 1 + jiffies; // 1ms to start playback asap
#else
    pcm->timer.expires = jiffies + pcm->pcm_period_size * HZ / (pcm->substream->runtime->rate * pcm->substream->runtime->channels * 2);
#endif
    add_timer(&pcm->timer);

#ifndef MT85XX_DEFAULT_CODE //2012/12/7 added by daniel
    pcm->timer_started = 1;
#endif
}

static void snd_card_mt85xx_pcm_playback_timer_stop(struct snd_mt85xx_pcm *pcm)
{
#ifdef MT85XX_DEFAULT_CODE //2012/12/7 added by daniel
    del_timer(&pcm->timer);
#else
    if (del_timer(&pcm->timer)==1)
    {
        pcm->timer_started = 0;
    }
#endif
}

static void snd_card_mt85xx_pcm_playback_update_write_pointer(struct snd_mt85xx_pcm *pcm)
{
    struct snd_pcm_runtime *runtime = pcm->substream->runtime;
    unsigned int pcm_wptr = frames_to_bytes(runtime, runtime->control->appl_ptr % runtime->buffer_size);

    if (pcm_wptr == pcm->pcm_rptr)
    {
        // check if buffer full
        if (0 == snd_pcm_playback_avail(runtime)) {
#ifdef MT85XX_DEFAULT_CODE
            i4AudSetBtnSndWp(pcm->instance, (pcm->pcm_rptr + pcm->pcm_buffer_size - 1) % pcm->pcm_buffer_size);
#else
            AUD_SetMixSndWritePtr(pcm->substream->number, AUD_GetMixSndFIFOStart(pcm->substream->number) + ((pcm->pcm_rptr + pcm->pcm_buffer_size - 1) % pcm->pcm_buffer_size));
#endif
        } else {
          // no need process now, will be updated at next timer
          // printk("[ALSA] timer: buffer empty\n");
        }
    }
    else
    {
#ifdef MT85XX_DEFAULT_CODE    
        i4AudSetBtnSndWp(pcm->instance, pcm_wptr);
#else
        AUD_SetMixSndWritePtr(pcm->substream->number, AUD_GetMixSndFIFOStart(pcm->substream->number) + pcm_wptr);
#endif
    }
}

static int snd_card_mt85xx_pcm_playback_trigger(struct snd_pcm_substream *substream, int cmd)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct snd_mt85xx_pcm *pcm = runtime->private_data;
    int err = 0;

    printk("[ALSA] operator: trigger, cmd = %d\n", cmd);

    spin_lock(&pcm->lock);
    switch (cmd) {
    case SNDRV_PCM_TRIGGER_START:
        snd_card_mt85xx_pcm_playback_update_write_pointer(pcm);
#ifdef MT85XX_DEFAULT_CODE            
        AUD_DSPCmdEffSndPlay(pcm->instance);
#endif
        snd_card_mt85xx_pcm_playback_timer_start(pcm);
        break;
    case SNDRV_PCM_TRIGGER_STOP:
        snd_card_mt85xx_pcm_playback_timer_stop(pcm);
#ifdef MT85XX_DEFAULT_CODE
        AUD_DSPCmdEffSndStop(pcm->instance);
#endif
        break;
    case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
        break;
    case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
        break;
    default:
        err = -EINVAL;
        break;
    }
    spin_unlock(&pcm->lock);
    return 0;
}

static void snd_card_mt85xx_pcm_playback_timer_function(unsigned long data)
{
    struct snd_mt85xx_pcm *pcm = (struct snd_mt85xx_pcm *)data;
    unsigned long flags;
    unsigned int pcm_old_rptr = pcm->pcm_rptr;
    unsigned int bytes_elapsed;

    spin_lock_irqsave(&pcm->lock, flags);

    // setup next timer
#ifdef MT85XX_DEFAULT_CODE    
    pcm->timer.expires = 10 + jiffies; // 10ms later
#else
    pcm->timer.expires = jiffies + pcm->pcm_period_size * HZ / (pcm->substream->runtime->rate * pcm->substream->runtime->channels * 2);        
#endif
    add_timer(&pcm->timer);

    // STEP 1: refresh read pointer
#ifdef MT85XX_DEFAULT_CODE    
    pcm->pcm_rptr = i4AudGetBtnSndReadOffset(pcm->instance);
#else
    pcm->pcm_rptr = AUD_GetMixSndReadPtr(pcm->substream->number) - AUD_GetMixSndFIFOStart(pcm->substream->number);
#endif

    // STEP 2: update write pointer
    snd_card_mt85xx_pcm_playback_update_write_pointer(pcm);

    // STEP 3: check period
    bytes_elapsed = pcm->pcm_rptr - pcm_old_rptr + pcm->pcm_buffer_size;
    bytes_elapsed %= pcm->pcm_buffer_size;
    pcm->bytes_elapsed += bytes_elapsed;

    // printk("[ALSA] timer: pcm->bytes_elapsed = %d\n", pcm->bytes_elapsed);

    if (pcm->bytes_elapsed >= pcm->pcm_period_size)
    {
        pcm->bytes_elapsed %= pcm->pcm_period_size;
        spin_unlock_irqrestore(&pcm->lock, flags);

        snd_pcm_period_elapsed(pcm->substream);
    }
    else
    {
        spin_unlock_irqrestore(&pcm->lock, flags);
    }
}

static snd_pcm_uframes_t snd_card_mt85xx_pcm_playback_pointer(struct snd_pcm_substream *substream)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct snd_mt85xx_pcm *pcm = runtime->private_data;

    // printk("[ALSA] operator: pointer\n");

    return bytes_to_frames(runtime, pcm->pcm_rptr);
}


static struct snd_pcm_hardware snd_card_mt85xx_pcm_playback_hw =
{
    .info =            (SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED |
                        SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_MMAP_VALID),
    .formats =          USE_FORMATS,
    .rates =            USE_RATE,
    .rate_min =         USE_RATE_MIN,
    .rate_max =         USE_RATE_MAX,
    .channels_min =     USE_CHANNELS_MIN,
    .channels_max =     USE_CHANNELS_MAX,
    .buffer_bytes_max = MAX_BUFFER_SIZE,
#ifdef MT85XX_DEFAULT_CODE    
    .period_bytes_min = 64,
#else
    .period_bytes_min = MIN_PERIOD_SIZE,
#endif
    .period_bytes_max = MAX_PERIOD_SIZE,
    .periods_min =      USE_PERIODS_MIN,
    .periods_max =      USE_PERIODS_MAX,
    .fifo_size =        0,
};

static void snd_card_mt85xx_runtime_free(struct snd_pcm_runtime *runtime)
{
    kfree(runtime->private_data);
}

static int snd_card_mt85xx_pcm_playback_hw_params(struct snd_pcm_substream *substream,
                    struct snd_pcm_hw_params *hw_params)
{
    printk("[ALSA] operator: hw_params\n");

    return 0;
}

static int snd_card_mt85xx_pcm_playback_buffer_bytes_rule(struct snd_pcm_hw_params *params,
					  struct snd_pcm_hw_rule *rule)
{
#if 1
    struct snd_interval *buffer_bytes;
    //printk("[ALSA] buffer bytes rule\n");
    // fix buffer size to 128KB
    buffer_bytes = hw_param_interval(params, SNDRV_PCM_HW_PARAM_BUFFER_BYTES);
    // patch for android audio
    // allow alsa/oss client to shrink buffer size
    buffer_bytes->min = 512;
    buffer_bytes->max = MAX_BUFFER_SIZE;
#endif
    return 0;
}

static int snd_card_mt85xx_pcm_playback_hw_free(struct snd_pcm_substream *substream)
{
    printk("[ALSA] operator: hw_free\n");

    return 0;
}

static struct snd_mt85xx_pcm *new_pcm_playback_stream(struct snd_pcm_substream *substream)
{
    struct snd_mt85xx_pcm *pcm;

    pcm = kzalloc(sizeof(*pcm), GFP_KERNEL);
    if (! pcm)
        return pcm;

    init_timer(&pcm->timer);
    pcm->timer.data = (unsigned long) pcm;
    pcm->timer.function = snd_card_mt85xx_pcm_playback_timer_function;

    spin_lock_init(&pcm->lock);

    pcm->substream = substream;
#ifdef MT85XX_DEFAULT_CODE
    pcm->instance = MIXSND0 + substream->number;
#else
    pcm->instance = substream->number;
#endif

    return pcm;
}

static int snd_card_mt85xx_pcm_playback_open(struct snd_pcm_substream *substream)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct snd_mt85xx_pcm *pcm;
    int err;

    if ((pcm = new_pcm_playback_stream(substream)) == NULL)
        return -ENOMEM;

    runtime->private_data = pcm;
    runtime->private_free = snd_card_mt85xx_runtime_free;
    runtime->hw = snd_card_mt85xx_pcm_playback_hw;

    printk("[ALSA] operator: open, substream = 0x%X\n", (unsigned int) substream);
    printk("[ALSA] substream->pcm->device = 0x%X\n", substream->pcm->device);
    printk("[ALSA] substream->number = %d\n", substream->number);

    if (substream->pcm->device & 1) {
        runtime->hw.info &= ~SNDRV_PCM_INFO_INTERLEAVED;
        runtime->hw.info |= SNDRV_PCM_INFO_NONINTERLEAVED;
    }
    if (substream->pcm->device & 2)
        runtime->hw.info &= ~(SNDRV_PCM_INFO_MMAP|SNDRV_PCM_INFO_MMAP_VALID);

    // add constraint rules
    err = snd_pcm_hw_rule_add(runtime, 0, SNDRV_PCM_HW_PARAM_BUFFER_BYTES,
                              snd_card_mt85xx_pcm_playback_buffer_bytes_rule, NULL,
                              SNDRV_PCM_HW_PARAM_CHANNELS, -1);

#ifndef MT85XX_DEFAULT_CODE
    AUD_InitALSAPlayback_MixSnd(substream->number);
#endif
    return 0;
}

static int snd_card_mt85xx_pcm_playback_close(struct snd_pcm_substream *substream)
{
#ifndef MT85XX_DEFAULT_CODE //2012/12/7 added by daniel
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct snd_mt85xx_pcm *pcm = runtime->private_data;
    if (pcm->timer_started)
    {
        del_timer_sync(&pcm->timer);
        pcm->timer_started = 0;
    }
#endif

    printk("[ALSA] operator: close, substream = 0x%X\n", (unsigned int) substream);

#ifndef MT85XX_DEFAULT_CODE
    AUD_DeInitALSAPlayback_MixSnd(substream->number);
#endif
    return 0;
}

static struct snd_pcm_ops snd_card_mt85xx_playback_ops = {
    .open      = snd_card_mt85xx_pcm_playback_open,
    .close     = snd_card_mt85xx_pcm_playback_close,
    .ioctl     = snd_pcm_lib_ioctl,
    .hw_params = snd_card_mt85xx_pcm_playback_hw_params,
    .hw_free   = snd_card_mt85xx_pcm_playback_hw_free,
    .prepare   = snd_card_mt85xx_pcm_playback_prepare,
    .trigger   = snd_card_mt85xx_pcm_playback_trigger,
    .pointer   = snd_card_mt85xx_pcm_playback_pointer,
};

//Dan Zhou add on 20111125
static int snd_card_mt85xx_pcm_capture_open(struct snd_pcm_substream *substream)
{
    printk("[ALSA]snd_card_mt85xx_pcm_capture_open----> operator: open, substream = 0x%X\n", (unsigned int) substream);
	
    return EPERM;
}

static int snd_card_mt85xx_pcm_capture_close(struct snd_pcm_substream *substream)
{
    printk("[ALSA]snd_card_mt85xx_pcm_capture_close----> operator: close, substream = 0x%X\n", (unsigned int) substream);

    return 0;
}

static struct snd_pcm_ops snd_card_mt85xx_capture_ops = {
    .open      = snd_card_mt85xx_pcm_capture_open,
    .close     = snd_card_mt85xx_pcm_capture_close,
    .ioctl     = snd_pcm_lib_ioctl,
};

int __devinit snd_card_mt85xx_pcm(struct snd_mt85xx *mt85xx, int device, int substreams)
{
    struct snd_pcm *pcm;
    int err;

    if ((err = snd_pcm_new(mt85xx->card, "mt85xx PCM", device,
                   substreams, substreams, &pcm)) < 0)
        return err;

    mt85xx->pcm = pcm;

    snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_card_mt85xx_playback_ops);
    //Dan Zhou add on 20111125
    snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_card_mt85xx_capture_ops);

    pcm->private_data = mt85xx;
    pcm->info_flags = 0;
#ifdef MT85XX_DEFAULT_CODE    
    strcpy(pcm->name, "mt85xx PCM");
#else
    strcpy(pcm->name, "mtk PCM");
#endif

    return 0;
}

