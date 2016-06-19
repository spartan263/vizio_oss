This source code was provided by Vizio based on a written source code offer on 6-June-2016.

The 'linux-3.0' folder is Linux v3.0.13 with newly-published GPL'd code with occasional (non-tracked) cherry-picks from recent kernel releases and other 3rd-party sources.

Covers MediaTek MT53xx/MT588x-series chips:

MediaTek MT5368 (Cortex-A9 single-core)  
MediaTek MT5368_FPGA (ARM11)  
MediaTek MT5396 (Cortex-A9 dual-core)  
MediaTek MT5396_FPGA (ARM11)  
MediaTek MT5369 (Cortex-A9 single-core)  
MediaTek MT5389 (Arm11)  
MediaTek MT5398 (Cobra Cortex-A9 dual-core)  
MediaTek MT5880 (Viper Cortex-A9)  
MediaTek MT5881 (Arm11)  

The provided code appears to be blob-free with the exception of Bluetooth drivers.

Additionally, Vizio is using a customized version of u-boot with complete GPLv2 copyright headers for contributed files (exceptions: 'nand_aes.c' has conflicting licensing headers and 'Makefile' is "proprietary").

On the Linux side the licensing headers are all clearly stated as GPLv2 with the exception of bcm4329 (previously published under GPL'd Android branches), DirectFB/gfx drivers ("MediaTek proprietary" but otherwise complete), and perhaps a few minor drivers.

Real-world devices known to have these chips:
 Vizio E500i-B1 LED Smart TV  
 (Unknown models) / Hisense
