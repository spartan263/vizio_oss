#include <alsa/asoundlib.h>

static char *device = "default";                        /* playback device */

snd_output_t *output = NULL;
unsigned char buffer[16*1024];                          /* some random data */

int main(int argc,char* argv[])
{
        int err;
        unsigned int i;
        snd_pcm_t *handle;
        snd_pcm_sframes_t frames;
        int latency = 0;
        char* filename;
        FILE* input;
        int buf_size = 8192;
        int ret;
        int ch_no;
        int sample_rate;

	if (argc != 5)
	{
		fprintf(stderr,"Usage: ut_main [filename] [ch no] [sample rate] [latency]\n");
		return -1;
	}
    
	filename = argv[1];
    ch_no = atoi(argv[2]);
    sample_rate = atoi(argv[3]);
    latency = atoi(argv[4]);
    input = fopen(filename,"rb");    
    if (!input)
    {
        fprintf(stderr,"Open input file %s failed ....\n", filename);
        return -1;
    }

        #if 0    
        for (i = 0; i < sizeof(buffer); i++)
                buffer[i] = random() & 0xff;
        #endif

        printf("main #1\n");
        if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
                printf("Playback open error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
        }
        
        printf("main #2\n");        
#if 1
        {
            int err;
            snd_pcm_hw_params_t *params;
            snd_pcm_sw_params_t *swparams;
            snd_pcm_hw_params_alloca(&params);
            snd_pcm_sw_params_alloca(&swparams);
            snd_pcm_hw_params_any(handle, params);
            snd_pcm_hw_params_set_access(handle, params,SND_PCM_ACCESS_RW_INTERLEAVED);
            snd_pcm_hw_params_set_format(handle, params,SND_PCM_FORMAT_S16_LE);
            snd_pcm_hw_params_set_channels(handle, params, ch_no);
            snd_pcm_hw_params_set_rate(handle, params,sample_rate, 0);
            snd_pcm_hw_params_set_period_size(handle, params, 0x400, 0);
            snd_pcm_hw_params_set_buffer_size(handle, params, 0x4000);
            snd_pcm_hw_params(handle, params);
        }            
#else
        if ((err = snd_pcm_set_params(handle,
                                      SND_PCM_FORMAT_S16_LE,
                                      SND_PCM_ACCESS_RW_INTERLEAVED,
                                      ch_no,                                      
                                      sample_rate,
                                      0,
                                      latency)) < 0) {   /* 0.5sec */
                printf("Playback open error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
        }
#endif
        printf("main #3\n");
        #if 1
        do
        {
            ret = fread(buffer, 1, buf_size, input);
            if (ret == 0)
            {
                break;
            }
            snd_pcm_writei(handle,buffer,ret/(ch_no*2));
        }
        while (ret);
        #else
        for (i = 0; i < 16; i++) {
                frames = snd_pcm_writei(handle, buffer, sizeof(buffer));
                if (frames < 0)
                        frames = snd_pcm_recover(handle, frames, 0);
                if (frames < 0) {
                        printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
                        break;
                }
                if (frames > 0 && frames < (long)sizeof(buffer))
                        printf("Short write (expected %li, wrote %li)\n", (long)sizeof(buffer), frames);
        }
        #endif

        printf("main #4\n");
        snd_pcm_close(handle);
        close(input);
        return 0;
}

