/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/******************************************************************************
* [File]			mtk_msdc_host0.c
* [Version]			v1.0
* [Revision Date]	2011-05-04
* [Author]			Shunli Wang, shunli.wang@mediatek.inc, 82896, 2011-05-04
* [Description]
*	MSDC Driver Source File
* [Copyright]
*	Copyright (C) 2011 MediaTek Incorporation. All Rights Reserved.
******************************************************************************/
#include <linux/delay.h>
#include <asm/delay.h>
#include <linux/highmem.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>

#include <linux/platform_device.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <asm/scatterlist.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/random.h>
#include <mach/irqs.h>
#include <mach/memory.h>
#ifdef CC_MTD_ENCRYPT_SUPPORT  
#include <crypto/mtk_crypto.h>
#endif
#include <mach/mtk_gpio.h>

#include "x_typedef.h"
#include "mtk_msdc_host_hw.h"
#include "mtk_msdc_slave_hw.h"
#include "mtk_msdc_drv.h"

#define DRIVER_NAME0 "MTK_MSDC0"
#define DRIVER_VERSION0 "0.10"

//#define BUGMAIL "Mediatek. Inc"

const int ch0 = 0;
static struct msdc_host *msdc_host0 = NULL;
static struct completion comp0;

#ifdef CONFIG_MTKEMMC_BOOT
extern struct semaphore msdc_host_sema;
#endif

#ifdef CONFIG_OPM
extern int force_suspend;
#endif

extern struct msdc_host *msdc_host_boot;
extern int (*msdc_send_command)(struct msdc_host *host, struct mmc_command *cmd);

#ifdef CC_MTD_ENCRYPT_SUPPORT  
extern mtk_partition mtk_msdc_partition;
#define AES_ADDR_ALIGN   0x40
#define AES_LEN_ALIGN    0x10 
#define AES_BUFF_LEN     0x10000
static UINT8 _u1MsdcBuf_AES[AES_BUFF_LEN + AES_ADDR_ALIGN], *_pu1MsdcBuf_Aes_Align;
#endif

/* Removable Card related */
#ifdef CC_SDMMC_SUPPORT	
#ifdef CONFIG_MT53XX_NATIVE_GPIO
#define MSDC_GPIO_MAX_NUMBERS 6 
extern int MSDC_Gpio[MSDC_GPIO_MAX_NUMBERS];
#endif 
struct tasklet_struct card_tasklet;	/* Tasklet structures */
struct tasklet_struct finish_tasklet;
struct timer_list card_detect_timer;	/* card detection timer */	
UINT32 plugCnt = 0;  /* count fast plug out-in times */	
UINT32 detectFlag = 0; /* detect flag */
UINT32 _bMSDCCardExist = 0;
UINT32 _sd_gpio_isr = 0;
#endif

/* the global variable related to clcok */
const UINT32 _u4MsdcFreq0[] = {MSDC_CLOCK_SELECTION_STRING};
const UINT32 _u4MsdcFreqSelVal0[] = {MSDC_CLOCK_SELECTION_VALUE};
UINT32 _u4MsdcFreqIdx0 = SD_DEFAULT_NORM_CLOCK_INDEX;
UINT32 _u4MsdcCurFreq0 = 48;

/* the global variable related to sample edge */
const EMMC_FLASH_DEV_T _arEMMC_DevInfo0[] =
{
    // Device name              ID1         ID2      Sample
    { "UNKNOWN",            0x00000000, 0x00000000, 0x00110000},
    { "H26M21001ECR",       0x4A48594E, 0x49582056, 0x00101100},
    { "H26M31003FPR",       0x4A205849, 0x4E594812, 0x00110000},
    { "H26M31003GMR",       0x4A483447, 0x31640402, 0x00110000},
    { "SDIN5D2-4G",         0x0053454D, 0x30344790, 0x00101100},
    { "KLM2G1HE3F-B001",    0x004D3247, 0x31484602, 0x00000000},
    { "THGBM3G4D1FBAIG",    0x00303032, 0x47303000, 0x00000100},
    { "THGBM4G4D1FBAIG",    0x00303032, 0x47343900, 0x00110100},
    { "THGBM4G5D1HBAIR",    0x00303034, 0x47343900, 0x00110100},
    { "THGBM4G6D2HBAIR",    0x00303038, 0x47344200, 0x00110100},
    { "THGBMAG5A1JBAIR",    0x00303034, 0x47393051, 0x00110100},
    { "MTFC4GMCDM-1M",      0x4E503158, 0x58585813, 0x00110000},
    { "2FA18-JW812",        0x4E4D4D43, 0x30344734, 0x00110000},
};

void MsdcFindDev0(UINT32 *pCID)
{
    UINT32 i, devNum;

	/*
      * why we need to define the id mask of emmc
      * Some vendors' emmc has the same brand & type but different product revision.
      * That means the firmware in eMMC has different revision
      * We should treat these emmcs as same type and timing
      * So id mask can ignore this case
      */
    UINT32 idMask = 0xFFFFFF00;

    devNum = sizeof(_arEMMC_DevInfo0)/sizeof(EMMC_FLASH_DEV_T);
    MSDC_LOG(MSG_LVL_INFO, "[0]%08X:%08X:%08X:%08X\n", pCID[0], pCID[1], pCID[2], pCID[3]);
    MSDC_LOG(MSG_LVL_INFO, "[0]id1:%08X id2:%08X\n", ID1(pCID), ID2(pCID));

    for(i = 0; i<devNum; i++)
    {
        if((_arEMMC_DevInfo0[i].u4ID1 == ID1(pCID)) &&
           ((_arEMMC_DevInfo0[i].u4ID2 & idMask) == (ID2(pCID) & idMask)))
        {
            break;
        }
    }

    msdc_host0->devIndex = (i == devNum)?0:i;

    MSDC_LOG(MSG_LVL_TITLE, "[0]eMMC Name: %s\n", _arEMMC_DevInfo0[msdc_host0->devIndex].acName);	
}

#ifdef CC_MTD_ENCRYPT_SUPPORT  
void msdc1_aes_init(void)
{  
	  UINT32 tmp;
	  
    if(NAND_AES_INIT())
    {  
        MSDC_LOG(MSG_LVL_ERR, "[1]aes init success!\n");
    }
    else
    {
        MSDC_LOG(MSG_LVL_ERR, "[1]aes init failed!\n");	
    }
    
    memset(_u1MsdcBuf_AES, 0x00, AES_BUFF_LEN + AES_ADDR_ALIGN);
    
    tmp = (UINT32)_u1MsdcBuf_AES & (AES_ADDR_ALIGN - 1);
    if(tmp != 0x0)
        _pu1MsdcBuf_Aes_Align = (UINT8 *)(((UINT32)_u1MsdcBuf_AES & (~(AES_ADDR_ALIGN - 1))) + AES_ADDR_ALIGN);
    else
    	  _pu1MsdcBuf_Aes_Align = (UINT8 *)_u1MsdcBuf_AES;
         
    return;     	
}

INT32 msdc1_partition_encrypted(UINT32 blkindx)
{
    int i = 0;
    
    for(i = 0;i < mtk_msdc_partition.nparts; i++)
    {
        if((mtk_msdc_partition.parts[i].attr & ATTR_ENCRYPTION) &&
        	 (blkindx >= mtk_msdc_partition.parts[i].start_sect) &&
        	 (blkindx < (mtk_msdc_partition.parts[i].start_sect + mtk_msdc_partition.parts[i].nr_sects)))	
        {
        	  MSDC_LOG(MSG_LVL_INFO, " the buffer(0x%08x) is encrypted!\n", blkindx);
            return 1;	
        }
    }
    
    return 0;	
}

INT32 msdc1_aes_encryption_buf(UINT32 buff, UINT32 len)
{
	  //MSDC_LOG(MSG_LVL_ERR, "->(enscryption.1)buffer %08x len %08x val %08x\n", buff, len, *((UINT32 *)buff));
	  
		if((len & (AES_LEN_ALIGN - 1)) != 0x0)
		{
		    MSDC_LOG(MSG_LVL_ERR, " the buffer length(0x%08x) to be encrypted is not align to %d bytes!\n", len, AES_LEN_ALIGN);
			  return MSDC_FAILED;	
		}
		
		if((buff & (AES_ADDR_ALIGN - 1)) != 0x0)  
		{
		    MSDC_LOG(MSG_LVL_ERR, " the buffer address(0x%08x) to be encrypted is not align to %d bytes!\n", buff, AES_ADDR_ALIGN);
			  return MSDC_FAILED;	
		}
		
		if(NAND_AES_Encryption(virt_to_phys((void *)buff), virt_to_phys((void *)buff), len))
    {
        MSDC_LOG(MSG_LVL_INFO, "Encryption to buffer(addr:0x%08X size:0x%08X) success(cur:%08x)!\n", buff, len, 
				                                                                          *((UINT32 *)buff));
    }
    else
    {
        MSDC_LOG(MSG_LVL_ERR, "Encryption to buffer(addr:0x%08X size:0x%08X) failed!\n", buff, len);	
        return MSDC_FAILED;    	
    }
    
		//MSDC_LOG(MSG_LVL_ERR, "->(enscryption.2)buffer %08x len %08x val %08x\n", buff, len, *((UINT32 *)buff));
		
	  return MSDC_SUCCESS;	
}

INT32 msdc1_aes_decryption_buf(UINT32 buff, UINT32 len)
{
	  //MSDC_LOG(MSG_LVL_ERR, "->(descryption.1)buffer %08x len %08x val %08x\n", buff, len, *((UINT32 *)buff));
	  
		if((len & (AES_LEN_ALIGN - 1)) != 0x0)
		{
		    MSDC_LOG(MSG_LVL_ERR, " the buffer length(0x%08x) to be encrypted is not align to %d bytes!\n", len, AES_LEN_ALIGN);
			  return MSDC_FAILED;	
		}
		
		if((buff & (AES_ADDR_ALIGN - 1)) != 0x0)  
		{
		    MSDC_LOG(MSG_LVL_ERR, " the buffer address(0x%08x) to be encrypted is not align to %d bytes!\n", buff, AES_ADDR_ALIGN);
			  return MSDC_FAILED;	
		}
		
    if(NAND_AES_Decryption(virt_to_phys((void *)buff), virt_to_phys((void *)buff), len))
    {
        MSDC_LOG(MSG_LVL_INFO, "Decryption to buffer(addr:0x%08X size:0x%08X) success(cur:%08x)!\n", buff, len,
				                                                                                     *((UINT32 *)buff));	
    }
    else
    {
        MSDC_LOG(MSG_LVL_ERR, "Decryption to buffer(addr:0x%08X size:0x%08X) failed!\n", buff, len);	
       return MSDC_FAILED;    	
    }	
		
		//MSDC_LOG(MSG_LVL_ERR, "->(descryption.2)buffer %08x len %08x val %08x\n", buff, len, *((UINT32 *)buff));
		
	  return MSDC_SUCCESS;			
}

INT32 msdc1_aes_encryption_sg(struct msdc_host *host, struct mmc_data *data)
{
    UINT32 len_used = 0, len_total = 0, len = 0, i = 0, buff = 0;
    struct scatterlist *sg;
    
    sg = data->sg;
    for(i = 0;i < data->sg_len;i++, sg++)
    {
		    len = sg->length;
		    buff = (UINT32)sg_virt(sg);
		    
		    if((len & (AES_LEN_ALIGN - 1)) != 0x0)
			  {
			      MSDC_LOG(MSG_LVL_ERR, " the buffer(0x%08x) to be encrypted is not align to %d bytes!\n", buff, AES_LEN_ALIGN);
			      return MSDC_FAILED;		
			  }	
			  
			  if( (virt_to_phys((void *)buff) & (AES_ADDR_ALIGN - 1)) != 0x0)
			  {
					  len_total = (UINT32)len;
					  do
					  {
					  	  len_used = (len_total > AES_BUFF_LEN)?AES_BUFF_LEN:len_total;
					      memcpy((void *)_pu1MsdcBuf_Aes_Align, (void *)buff, len_used);	
					      if(NAND_AES_Encryption(virt_to_phys((void *)_pu1MsdcBuf_Aes_Align), virt_to_phys((void *)_pu1MsdcBuf_Aes_Align), len_used))
				        {
				            MSDC_LOG(MSG_LVL_INFO, "Encryption to buffer(addr:0x%08X size:0x%08X) success(cur:%08x)!\n", (UINT32)_pu1MsdcBuf_Aes_Align, len_used,
				                                                                                  *((UINT32 *)_pu1MsdcBuf_Aes_Align));	
				        }
				        else
				        {
				            MSDC_LOG(MSG_LVL_ERR, "Encryption to buffer(addr:0x%08X size:0x%08X) failed!\n", (UINT32)_pu1MsdcBuf_Aes_Align, len_used);	
				            return MSDC_FAILED;    	
				        }	
				        memcpy((void *)buff, (void *)_pu1MsdcBuf_Aes_Align, len_used);
				        
				        len_total -= len_used;
				        buff += len_used;
				        
					  }while(len_total > 0);
			  }
			  else
			  {
			      if(NAND_AES_Encryption(virt_to_phys((void *)buff), virt_to_phys((void *)buff), len))
				    {
				        MSDC_LOG(MSG_LVL_INFO, "Encryption to buffer(addr:0x%08X size:0x%08X) success(cur:%08x)!\n", buff, len, 
				                                                                          *((UINT32 *)buff));
				    }
				    else
				    {
				        MSDC_LOG(MSG_LVL_ERR, "Encryption to buffer(addr:0x%08X size:0x%08X) failed!\n", buff, len);	
				        return MSDC_FAILED;    	
				    }		
			  }
	  }
	  
	  return MSDC_SUCCESS;	
}

INT32 msdc1_aes_decryption_sg(struct msdc_host *host, struct mmc_data *data)
{
    UINT32 len_used = 0, len_total = 0, len = 0, i = 0, buff = 0;
    struct scatterlist *sg;
    
    sg = data->sg;
    for(i = 0;i < data->sg_len;i++, sg++)
    {
    	  len = sg->length;
		    buff = (UINT32)sg_virt(sg);
		    
		    if((len & (AES_LEN_ALIGN - 1)) != 0x0)
			  {
			      MSDC_LOG(MSG_LVL_ERR, " the buffer(0x%08x) to be encrypted is not align to %d bytes!\n", buff, AES_LEN_ALIGN);
			      return MSDC_FAILED;		
			  }	
			  
			  if( (virt_to_phys((void *)buff) & (AES_ADDR_ALIGN - 1)) != 0x0)
			  {
					  len_total = len;
					  do
					  {
					  	  len_used = (len_total > AES_BUFF_LEN)?AES_BUFF_LEN:len_total;
					      memcpy((void *)_pu1MsdcBuf_Aes_Align, (void *)buff, len_used);	
					      if(NAND_AES_Decryption(virt_to_phys((void *)_pu1MsdcBuf_Aes_Align), virt_to_phys((void *)_pu1MsdcBuf_Aes_Align), len_used))
				        {
				            MSDC_LOG(MSG_LVL_INFO, "Decryption to buffer(addr:0x%08X size:0x%08X) success(cur:%08x)!\n", (UINT32)_pu1MsdcBuf_Aes_Align, len_used,
				                                                                                         *((UINT32 *)_pu1MsdcBuf_Aes_Align));	
				            //return MSDC_SUCCESS;	
				        }
				        else
				        {
				            MSDC_LOG(MSG_LVL_ERR, "Decryption to buffer(addr:0x%08X size:0x%08X) failed!\n", (UINT32)_pu1MsdcBuf_Aes_Align, len_used);	
				            return MSDC_FAILED;    	
				        }	
				        memcpy((void *)buff, (void *)_pu1MsdcBuf_Aes_Align, len_used);
				        
				        len_total -= len_used;
				        buff += len_used;
				        
					  }while(len_total > 0);
		    }
		    else
			  {
			      if(NAND_AES_Decryption(virt_to_phys((void *)buff), virt_to_phys((void *)buff), len))
				    {
				        MSDC_LOG(MSG_LVL_INFO, "Decryption to buffer(addr:0x%08X size:0x%08X) success(cur:%08x)!\n", buff, len,
				                                                                                     *((UINT32 *)buff));	
				    }
				    else
				    {
				        MSDC_LOG(MSG_LVL_ERR, "Decryption to buffer(addr:0x%08X size:0x%08X) failed!\n", buff, len);	
				        return MSDC_FAILED;    	
				    }		
			  }	
		}	
	  
	  return MSDC_SUCCESS;
}

#endif
UINT8 MsdcCheckSumCal0(UINT8 *buf, UINT32 len)
{
    UINT32 i = 0, sum = 0;

    for(; i<len;i++)
    {
        sum += *(buf + i);
    }

    return (0xFF-(UINT8)sum);
}

void MsdcDescriptorDump0(struct msdc_host *host)
{
    msdc_gpd_t *gpd;
    UINT32 i = 0;

	
    gpd = host->gpd;
	printk("addr - %08x\n", (UINT32)(host->gpd_addr));
	for(;i<2*sizeof(msdc_gpd_t);i++)
	{
        printk("%02x ", *((u8*)(gpd)+i));
	}
	
}

void MsdcDescriptorConfig0(struct msdc_host *host, struct mmc_data *data)
{
    UINT32 bdlen, i;
    int nents;
    struct scatterlist *sg;
    INT32 dir = DMA_FROM_DEVICE;
    msdc_gpd_t *gpd;
    msdc_bd_t *bd;
    
#ifdef CC_MTD_ENCRYPT_SUPPORT  
    UINT32 addr_blk = 0;
#endif    
    bdlen = data->sg_len;
    sg = data->sg;
    gpd = host->gpd;
    bd = host->bd;
    dir = (data->flags & MMC_DATA_READ)?DMA_FROM_DEVICE:DMA_TO_DEVICE;
#ifdef CC_MTD_ENCRYPT_SUPPORT  
    if((host->cur_cmd.opcode == MMC_WRITE_BLOCK) ||
    	 (host->cur_cmd.opcode == MMC_WRITE_MULTIPLE_BLOCK))
    {
    	  addr_blk = mmc_card_blockaddr(host->mmc->card)?(host->cur_cmd.arg):(host->cur_cmd.arg>>9);
    	  	
        if(msdc1_partition_encrypted(addr_blk))
        {  
        	if (msdc1_aes_encryption_sg(host, data))
        	{
        	    MSDC_LOG(MSG_LVL_ERR, "[1]encryption before write process failed!\n");	
        	    return;
        	} 
        	MSDC_LOG(MSG_LVL_INFO, "[1]encryption before write process success!\n");	
        } 	
    }  
#endif

    nents = dma_map_sg(mmc_dev(host->mmc), data->sg, data->sg_len, dir);
    if(0 == nents)
    {
        printk(KERN_ERR "[0]bad sg map!!\n");
        return;
    }

    /* modify gpd*/
    //gpd->intr = 0; 
    gpd->hwo = 1;  /* hw will clear it */
    gpd->bdp = 1;     
    gpd->chksum = 0;  /* need to clear first. */   
    gpd->chksum = MsdcCheckSumCal0((UINT8 *)gpd, 16);

    /* modify bd*/          
    for (i = 0; i < bdlen; i++) 
    {
        //msdc_init_bd(&bd[j], blkpad, dwpad, sg_dma_address(sg), sg_dma_len(sg)); 
        bd[i].pBuff = (void *)(sg_dma_address(sg));
        bd[i].buffLen = sg_dma_len(sg);          
        if(i == bdlen - 1) 
        {
            bd[i].eol = 1;     	/* the last bd */
        } 
        else 
        {
            bd[i].eol = 0; 	
        }

        bd[i].chksum = 0; /* checksume need to clear first */
        bd[i].chksum = MsdcCheckSumCal0((UINT8 *)(&bd[i]), 16);         
        sg++;
    }

    mb();

    // Config the DMA HW registers
    MSDC_SETBIT(DMA_CFG(ch0), DMA_CFG_CHKSUM);
    MSDC_WRITE32(DMA_SA(ch0), 0x0);
    MSDC_SETBIT(DMA_CTRL(ch0), DMA_CTRL_BST_64);
    MSDC_SETBIT(DMA_CTRL(ch0), DMA_CTRL_DESC_MODE);

    MSDC_WRITE32(DMA_SA(ch0), (UINT32)host->gpd_addr);

}

void MsdcDescriptorFlush0(struct msdc_host *host, struct mmc_data *data)
{
    UINT32 tmpSgLen;
    struct scatterlist *tmpSg;
    INT32 dir = DMA_FROM_DEVICE;
#ifdef CC_MTD_ENCRYPT_SUPPORT  
    UINT32 addr_blk = 0;
#endif
    tmpSgLen = data->sg_len;
    tmpSg = data->sg;
    dir = (data->flags & MMC_DATA_READ)?DMA_FROM_DEVICE:DMA_TO_DEVICE;

    if(tmpSgLen == 0)
        return;	
        
#ifdef CC_MTD_ENCRYPT_SUPPORT  
    if((host->cur_cmd.opcode == MMC_READ_SINGLE_BLOCK) ||
    	 (host->cur_cmd.opcode == MMC_READ_MULTIPLE_BLOCK))
    {
    	  addr_blk = mmc_card_blockaddr(host->mmc->card)?(host->cur_cmd.arg):(host->cur_cmd.arg>>9);
    	  	
        if(msdc1_partition_encrypted(addr_blk))
        {  
        	if (msdc1_aes_decryption_sg(host, data))
        	{
        	    MSDC_LOG(MSG_LVL_ERR, "[1]decryption after read process failed!\n");	
        	    return;
        	}
        	
        	MSDC_LOG(MSG_LVL_INFO, "[1]decryption after read process success!\n");	 
        } 	
    }  
#endif    
    dma_unmap_sg(mmc_dev(host->mmc), data->sg, data->sg_len, dir);
    
}

void MsdcSetSample0(UINT8 fgFalling)
{
    // Set command response sample selection
    if(fgFalling & 0x0F)
    {
        MSDC_SETBIT(MSDC_IOCON(ch0), ((0x1) << 1)); 
    } 
    else
    {
        MSDC_CLRBIT(MSDC_IOCON(ch0), ((0x1) << 1));
    }
	    
    // Set read data sample selection
    if((fgFalling>>4) & 0x0F)
    {
        MSDC_SETBIT(MSDC_IOCON(ch0), ((0x1) << 2));  
    }
    else
    {
        MSDC_CLRBIT(MSDC_IOCON(ch0), ((0x1) << 2));
    }		
}

void MsdcClrTiming0(void)
{
    // Clear Sample Edge
    MSDC_CLRBIT(MSDC_IOCON(ch0), (((UINT32)0xF) << 0));

    // Clear Pad Tune
    MSDC_WRITE32(PAD_TUNE(ch0), 0x00000000);
    
    // Clear 
    MSDC_WRITE32(DAT_RD_DLY0(ch0), 0x00000000);
    MSDC_WRITE32(DAT_RD_DLY1(ch0), 0x00000000);

#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || \
    defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368) || defined(CONFIG_ARCH_MT5389)
    MSDC_LOG(MSG_LVL_ERR, "[0]PAD_CLK_SMT Setting\n");
    MSDC_SETBIT(SD20_PAD_CTL0(ch0), (0x1<<18));
#endif

}

void MsdcSetSampleEdge0(UINT32 fgWhere)
{
    UINT8 u1Sample;

    u1Sample = (_arEMMC_DevInfo0[msdc_host0->devIndex].u4Sample>>(8*fgWhere)) & 0xFF;

    MSDC_LOG(MSG_LVL_ERR, "[0]Dev Num: %d, Timing Position: %d, Sample Edge: 0x%02X\n", msdc_host0->devIndex, fgWhere, u1Sample);

    // Set Sample Edge
    MsdcSetSample0(u1Sample);

}

INT32 MsdcReset0(void)
{
#if 0
    UINT32 i;

	// Reset MSDC
    MSDC_SETBIT(MSDC_CFG(ch0), MSDC_CFG_RST);

    for(i = 0; i<MSDC_RST_TIMEOUT_LIMIT_COUNT; i++)
    {
        if (0 == (MSDC_READ32(MSDC_CFG(ch0)) & MSDC_CFG_RST))
        {
            break;
        }

        //HAL_Delay_us(1000);
        udelay(1000);
    }
    if(i == MSDC_RST_TIMEOUT_LIMIT_COUNT)
    {
        return MSDC_FAILED;
    }
#else

    unsigned long u4JiffSt = 0;
    UINT32 u4Val;

    // Reset MSDC
    MSDC_SETBIT(MSDC_CFG(ch0), MSDC_CFG_RST);

    u4JiffSt = jiffies;
    while(1)
    {
        u4Val = (MSDC_READ32(MSDC_CFG(ch0)) & MSDC_CFG_RST);
        if(u4Val == 0)
        {
            break;
        }
        else
        {
            if(time_after(jiffies, u4JiffSt + 20/(1000/HZ)))
            {
                MSDC_LOG(MSG_LVL_WARN, "[0]Wait HOST Controller Stable Timeout!\r\n");
                return MSDC_FAILED;
            }
        }
    }
#endif

    return MSDC_SUCCESS;

}

INT32 MsdcClrFifo0(void)
{
#if 0
    UINT32 i;
    // Reset FIFO
    MSDC_SETBIT(MSDC_FIFOCS(ch0), MSDC_FIFOCS_FIFOCLR);

    for(i = 0; i<MSDC_RST_TIMEOUT_LIMIT_COUNT; i++)
    {
        if (0 == (MSDC_READ32(MSDC_FIFOCS(ch0)) & (MSDC_FIFOCS_FIFOCLR | MSDC_FIFOCS_TXFIFOCNT_MASK | MSDC_FIFOCS_RXFIFOCNT_MASK)))
        {
            break;
        }

        //HAL_Delay_us(1000);
        udelay(1000);
    }
    if(i == MSDC_FIFOCLR_TIMEOUT_LIMIT_COUNT)
    {
        return MSDC_FAILED;
    }
#else

    unsigned long u4JiffSt = 0;
    UINT32 u4Val;

    // Reset FIFO
    MSDC_SETBIT(MSDC_FIFOCS(ch0), MSDC_FIFOCS_FIFOCLR);

    u4JiffSt = jiffies;
    while(1)
    {
        u4Val = (MSDC_READ32(MSDC_FIFOCS(ch0)) & (MSDC_FIFOCS_FIFOCLR | MSDC_FIFOCS_TXFIFOCNT_MASK | MSDC_FIFOCS_RXFIFOCNT_MASK));
        if(u4Val == 0)
        {
            break;
        }
        else
        {
            if(time_after(jiffies, u4JiffSt + 20/(1000/HZ)))
            {
                MSDC_LOG(MSG_LVL_WARN, "[0]Wait HOST Controller FIFO Clear Timeout!\r\n");
                return MSDC_FAILED;
            }
        }
    }
#endif

    return MSDC_SUCCESS;
}

void MsdcChkFifo0(void)
{
    // Check if rx/tx fifo is zero
    if ((MSDC_READ32(MSDC_FIFOCS(ch0)) & (MSDC_FIFOCS_TXFIFOCNT_MASK | MSDC_FIFOCS_RXFIFOCNT_MASK)) != 0)
    {
        MSDC_LOG(MSG_LVL_INFO, "[0]FiFo not 0, FIFOCS:0x%08X !!\r\n", MSDC_READ32(MSDC_FIFOCS(ch0)));
        MsdcClrFifo0();
    }
}

void MsdcClrIntr0(void)
{
    // Check MSDC Interrupt vector register
    if  (0x00  != MSDC_READ32(MSDC_INT(ch0)))
    {
        MSDC_LOG(MSG_LVL_INFO, "[0]MSDC INT(0x%08X) not 0:0x%08X !!\r\n", MSDC_INT(ch0), MSDC_READ32(MSDC_INT(ch0)));

        // Clear MSDC Interrupt vector register
        MSDC_WRITE32(MSDC_INT(ch0), MSDC_READ32(MSDC_INT(ch0)));
    }
}

void MsdcStopDMA0(void)
{
    MSDC_LOG(MSG_LVL_INFO, "[0]DMA status: 0x%.8x\n",MSDC_READ32(DMA_CFG(ch0)));
	
    MSDC_SETBIT(DMA_CTRL(ch0), DMA_CTRL_STOP);
    while(MSDC_READ32(DMA_CFG(ch0)) & DMA_CFG_DMA_STATUS);
	
    MSDC_LOG(MSG_LVL_INFO, "[0]DMA Stopped!\n");
}

INT32 MsdcWaitClkStable0(void)
{
#if 0
    UINT32 i;

    for(i = 0; i<MSDC_CLK_TIMEOUT_LIMIT_COUNT; i++)
    {
        if (0 != (MSDC_READ32(MSDC_CFG(ch0)) & MSDC_CFG_CARD_CK_STABLE))
        {
            break;
        }

        //HAL_Delay_us(1000);
        udelay(1000);
    }
	  if(i == MSDC_CLK_TIMEOUT_LIMIT_COUNT)
	  {
        MSDC_LOG(MSG_LVL_ERR, "WaitClkStable Failed !\r\n");
        return MSDC_FAILED;
	  }
#else

    unsigned long u4JiffSt = 0;
    UINT32 u4Val;

    u4JiffSt = jiffies;
    while(1)
    {
        u4Val = (MSDC_READ32(MSDC_CFG(ch0)) & MSDC_CFG_CARD_CK_STABLE);
        if(u4Val != 0)
        {
            break;
        }
        else
        {
            if(time_after(jiffies, u4JiffSt + 20/(1000/HZ)))
            {
                MSDC_LOG(MSG_LVL_WARN, "[0]Wait HOST Controller Clock Stable Timeout!\r\n");
                return MSDC_FAILED;
            }
        }
    }
#endif

    return MSDC_SUCCESS;
}

#ifdef CC_SDMMC_SUPPORT	
static irqreturn_t MsdcHandleGPIOIsr0(int irq, void *dev_id)
{
    MSDC_LOG(MSG_LVL_ERR, "[0][MsdcHandleGPIOIsr0, %d]---------------------->!\n", plugCnt);
    plugCnt++;

	//SD_CARD_DETECT_GPIO_STS_CLR;
    return IRQ_HANDLED;
}

static void MsdcSetGPIOIsr0(UINT32 u4Mask, struct msdc_host *host)
{
    SD_GPIO_INTERRUPT_EN;
}

static INT32 MsdcRegGPIOIsr0(struct msdc_host *host)
{
	MSDC_LOG(MSG_LVL_INFO, "[0]Register GPIO ISR\n");
    if (request_irq(VECTOR_EXT1, MsdcHandleGPIOIsr0, IRQF_SHARED, DRIVER_NAME0, host) != 0)
    {
        MSDC_LOG(MSG_LVL_ERR, "Request MSDC GPIO IRQ fail!\n");
    }  

    return 0;
}
#endif

static irqreturn_t MsdcHandleIsr0(int irq, void *dev_id)
{
    volatile UINT32 u4TmpAccuVect = 0x00;
    UINT32 u4TriggerComplete = 0;

    u4TmpAccuVect = MSDC_READ32(MSDC_INT(ch0));

    msdc_host0->MsdcAccuVect |= u4TmpAccuVect;

    MSDC_LOG(MSG_LVL_TRACE, "[0][MSDC TMP]%08X!\r\n", u4TmpAccuVect);

    MSDC_WRITE32(MSDC_INT(ch0), u4TmpAccuVect);

    if((msdc_host0->waitIntMode == 0) && ((msdc_host0->MsdcAccuVect & msdc_host0->desVect) != 0))
    {
        MSDC_LOG(MSG_LVL_TRACE, "[0][MSDC FINAL]%08X!\r\n", msdc_host0->MsdcAccuVect);
        u4TriggerComplete = 1;
        //complete(&comp1);
    }
    else if((msdc_host0->waitIntMode == 1) && ((msdc_host0->MsdcAccuVect & msdc_host0->desVect) == msdc_host0->desVect))
    {
        MSDC_LOG(MSG_LVL_TRACE, "[0][MSDC FINAL]%08X!\r\n", msdc_host0->MsdcAccuVect);
        u4TriggerComplete = 1;
        //complete(&comp1);
    }
    else
    {
        // do nothing
    }

    if((msdc_host0->MsdcAccuVect & INT_SD_DATA_CRCERR) != 0)
    {
        // return directly
        MSDC_LOG(MSG_LVL_TRACE, "[0][MSDC FINAL]%08X!\r\n", msdc_host0->MsdcAccuVect);
        u4TriggerComplete = 1;
        //complete(&comp1);
    }	

    if((msdc_host0->MsdcAccuVect & INT_SD_AUTOCMD_RESP_CRCERR) != 0)
    {
        // return directly
        MSDC_LOG(MSG_LVL_TRACE, "[0][MSDC FINAL]%08X!\r\n", msdc_host0->MsdcAccuVect);
        u4TriggerComplete = 1;
        //complete(&comp1);
    }
	
    if (u4TriggerComplete == 1)
    {
        complete(&comp0);
    }

    return IRQ_HANDLED;
}

static void MsdcSetIsr0(UINT32 u4Mask, struct msdc_host *host)
{
    MSDC_SETBIT(MSDC0_EN_REG, (0x1<<MSDC0_EN_SHIFT));
    MSDC_WRITE32(MSDC_INTEN(ch0), u4Mask);
    enable_irq(host->irq);
}

static INT32 MsdcRegIsr0(struct msdc_host *host)
{
    MsdcClrIntr0();  // Clear interrupt, read clear

    if (request_irq(host->irq, MsdcHandleIsr0, IRQF_SHARED, DRIVER_NAME0, host) != 0)
    {
        printk(KERN_ERR "[0]Request MSDC0 IRQ fail!\n");
        return -1;
    }

#ifdef CC_SDMMC_SUPPORT	
    MSDC_LOG(MSG_LVL_INFO, "[0]Register GPIO ISR\n");
    if (request_irq(VECTOR_EXT1, MsdcHandleGPIOIsr0, IRQF_SHARED, DRIVER_NAME0, host) != 0)
    {
        MSDC_LOG(MSG_LVL_ERR, "Request MSDC GPIO IRQ fail!\n");
    }  
#endif

    MsdcClrIntr0();  // Clear interrupt, read clear
    disable_irq(host->irq);

    return 0;

}

void MsdcSetTimeout(struct msdc_host *host, UINT32 ns, UINT32 clks)
{
    UINT32 timeout, clk_ns;

	host->timeout_ns = ns;
	host->timeout_clks = clks;

	clk_ns = 1000000000UL/host->clk;
	timeout = ns/clk_ns + clks;
	timeout = timeout >> 16;
	timeout = (timeout > 1)? (timeout-1):0;
	timeout = (timeout > 255)?255:(timeout);

	MSDC_WRITE32(SDC_CFG(ch0), (timeout<<SDC_CFG_DTOC_SHIFT));
}

void MsdcPinMux0(void)
{
    MSDC_LOG(MSG_LVL_INFO, "[0]msdc0 pinmux for mt%d\n", CONFIG_CHIP_VERSION);

#if defined(CC_MT5396) || defined(CONFIG_ARCH_MT5396) || \
	defined(CC_MT5368) || defined(CONFIG_ARCH_MT5368) || \
	defined(CC_MT5389) || defined(CONFIG_ARCH_MT5389)	  

    MSDC_CLRBIT(0xF000D408, 0x03);
    MSDC_SETBIT(0xF000D408, 0x01);
    
	//Local Arbitor open	
    MSDC_CLRBIT(0xF0012200, (0x03<<16) | (0x1F<<0));	 
    MSDC_SETBIT(0xF0012200, (0x01<<16) | (0x1F<<0));

#elif defined(CC_MT5398) ||defined(CONFIG_ARCH_MT5398) 

    //pinmux register d604[27:26], function 1 - CMD/CLK/DAT0~DAT3		 
    MSDC_CLRBIT(0xF000D604, 0x03<<26); 	   
    MSDC_SETBIT(0xF000D604, 0x01<<26);				 

    //Local Arbitor open	
    MSDC_CLRBIT(0xF0012200, (0x03<<16) | (0x1F<<0));	 
    MSDC_SETBIT(0xF0012200, (0x01<<16) | (0x1F<<0));	
      
#elif  defined(CC_MT5880) || defined(CONFIG_ARCH_MT5880)	

    if (IS_IC_MT5860_E2IC()) // sd card
    {
        MSDC_LOG(MSG_LVL_INFO, "[0]msdc0(sd) pinmux for mt5860\n");			
        MSDC_CLRBIT(0xF000D620, 0x1<<7);
        MSDC_SETBIT(0xF000D620, 0x1<<7);
        MSDC_CLRBIT(0xF000D610, 0x3FFFF<<12);
        MSDC_SETBIT(0xF000D610, 0x12492<<12);
        //Local Arbitor open
        MSDC_CLRBIT(0xF0012200, (0x03<<16) | (0x1F<<0));	 
        MSDC_SETBIT(0xF0012200, (0x01<<16) | (0x1F<<0));
    }
    else // emmc
    {
        MSDC_LOG(MSG_LVL_INFO, "[0]msdc0(emmc) pinmux for mt5860\n");			
        MSDC_CLRBIT(0xF000D604, 0x03<<4);
        MSDC_SETBIT(0xF000D604, 0x02<<4);
    }
#else

    MSDC_CLRBIT(0xF000D408, 0x03);
    MSDC_SETBIT(0xF000D408, 0x01);

    //Local Arbitor open	
    MSDC_CLRBIT(0xF0012200, (0x03<<16) | (0x1F<<0));	 
    MSDC_SETBIT(0xF0012200, (0x01<<16) | (0x1F<<0));

#endif	  
}

INT32 MsdcInit0(void)
{
    // Pinmux Switch
    MsdcPinMux0();

	MSDC_LOG(MSG_LVL_INFO, "[0]msdc1 reset for mt%d\n", CONFIG_CHIP_VERSION);

#if defined(CC_MT5396) || defined(CONFIG_ARCH_MT5396) || \
	defined(CC_MT5368) || defined(CONFIG_ARCH_MT5368) || \
	defined(CC_MT5389) || defined(CONFIG_ARCH_MT5389)

    // Reset MSDC
    MsdcReset0();
    // Set SD/MMC Mode
    MSDC_SETBIT(MSDC_CFG(ch0), MSDC_CFG_SD);
	
#elif defined(CC_MT5398) || defined(CONFIG_ARCH_MT5398) || \
      defined(CC_MT5880) || defined(CONFIG_ARCH_MT5880) || \
      defined(CC_MT5881) || defined(CONFIG_ARCH_MT5881) || \
      defined(CC_MT5860) || defined(CONFIG_ARCH_MT5860)

    // Set SD/MMC Mode
    MSDC_SETBIT(MSDC_CFG(ch0), MSDC_CFG_SD);
    // Reset MSDC
    MsdcReset0();
	
#else

    // Reset MSDC
    MsdcReset0();
    // Set SD/MMC Mode
    MSDC_SETBIT(MSDC_CFG(ch0), MSDC_CFG_SD);
	
#endif
	
    if(msdc_host0->data_mode > 1)
    {
        // Set DMA Mode
        MSDC_CLRBIT(MSDC_CFG(ch0), MSDC_CFG_PIO_MODE);
    }
    else
    {
        // Set PIO Mode
        MSDC_SETBIT(MSDC_CFG(ch0), MSDC_CFG_PIO_MODE);
    }

    // Disable sdio & Set bus to 1 bit mode
    MSDC_CLRBIT(SDC_CFG(ch0), SDC_CFG_SDIO | SDC_CFG_BW_MASK);

    // set clock mode (DIV mode)
    MSDC_CLRBIT(MSDC_CFG(ch0), (((UINT32)0x03) << 16));

    // Wait until clock is stable
    if (MSDC_FAILED == MsdcWaitClkStable0())
    {
        return MSDC_FAILED;
    }

    // Set default RISC_SIZE for DWRD pio mode
    MSDC_WRITE32(MSDC_IOCON(ch0), (MSDC_READ32(MSDC_IOCON(ch0)) & ~MSDC_IOCON_RISC_SIZE_MASK) | MSDC_IOCON_RISC_SIZE_DWRD);

    // Set Data timeout setting => Maximum setting
    MSDC_WRITE32(SDC_CFG(ch0), (MSDC_READ32(SDC_CFG(ch0)) & ~(((UINT32)0xFF) << SDC_CFG_DTOC_SHIFT)) | (((UINT32)0xFF) << SDC_CFG_DTOC_SHIFT));

    MsdcClrTiming0();

    return MSDC_SUCCESS;
}


void MsdcContinueClock0(int i4ContinueClock)
{
    if (i4ContinueClock)
    {
       // Set clock continuous even if no command
       MSDC_SETBIT(MSDC_CFG(ch0), (((UINT32)0x01) << 1));
    }
    else
    {
       // Set clock power down if no command
       MSDC_CLRBIT(MSDC_CFG(ch0), (((UINT32)0x01) << 1));
    }
}

INT32 MsdcWaitHostIdle0(void)
{
#if 0
    UINT32 i;
    for(i=0;i<MSDC_WAIT_SDC_BUS_TIMEOUT_LIMIT_COUNT;i++)
    {
        if ((0 == (MSDC_READ32(SDC_STS(ch0)) & (SDC_STS_SDCBUSY | SDC_STS_CMDBUSY))) && (0x00  == MSDC_READ32(MSDC_INT(ch0))))
        {
            break;
        }
        //HAL_Delay_us(1000);
        udelay(1000);
    }
    if(i == MSDC_WAIT_SDC_BUS_TIMEOUT_LIMIT_COUNT)
    {
        return MSDC_FAILED;
    }
#else
    unsigned long u4JiffSt = 0;

    u4JiffSt = jiffies;
    while(1)
    {
        if ((0 == (MSDC_READ32(SDC_STS(ch0)) & (SDC_STS_SDCBUSY | SDC_STS_CMDBUSY))) && (0x00  == MSDC_READ32(MSDC_INT(ch0))))
        {
            break;
        }
        else
        {
            if(time_after(jiffies, u4JiffSt + 400/(1000/HZ)))
            {
                MSDC_LOG(MSG_LVL_WARN, "[0]Wait HOST Controller Idle Timeout!\r\n");
                return MSDC_FAILED;
            }
        }
    }
#endif

    return MSDC_SUCCESS;
}

INT32 MsdcWaitIntr0(UINT32 vector, UINT32 timeoutCnt, UINT32 fgMode)
{
    //UINT32 i; 
    UINT32 u4Ret;

    // Clear Vector variable
    // modified by shunli.wang at 2012/10/27
    //msdc_host0->MsdcAccuVect = 0;
    msdc_host0->desVect = vector;
    msdc_host0->waitIntMode = fgMode;

    if(!msdc_host0->msdc_isr_en)
    {
#if 0
        for(i = 0; i<timeoutCnt; i++)
        {
            // Status match any bit
            if (0 != (MSDC_READ32(MSDC_INT(ch0)) & vector))
            {
                msdc_host0->MsdcAccuVect |= MSDC_READ32(MSDC_INT(ch0));
                printk(KERN_ERR "->%08X\r\n", MSDC_READ32(MSDC_INT(ch0)));
                printk(KERN_ERR "->%08X, %08X, %08X, %08X, %08X\r\n", MSDC_READ32(MSDC_INTEN(ch0)), MSDC_READ32(0xF0008034), MSDC_READ32(0xF0008030), MSDC_READ32(0xF0008084), MSDC_READ32(0xF0008080));
                MSDC_WRITE32(MSDC_INT(ch0), msdc_host0->MsdcAccuVect);
                return MSDC_SUCCESS;
            }

            //HAL_Delay_us(1000);
            udelay(1000);
        }
#else
        unsigned long u4JiffSt = 0;

        u4JiffSt = jiffies;
        while(1)
        {
            if((fgMode == 0) && ((MSDC_READ32(MSDC_INT(ch0)) & vector) != 0))
            {
                msdc_host0->MsdcAccuVect |= MSDC_READ32(MSDC_INT(ch0));
                MSDC_WRITE32(MSDC_INT(ch0), msdc_host0->MsdcAccuVect);
                return MSDC_SUCCESS;
            }
            else if((fgMode == 1) && ((MSDC_READ32(MSDC_INT(ch0)) & vector) == vector))
            {
                msdc_host0->MsdcAccuVect |= MSDC_READ32(MSDC_INT(ch0));
                MSDC_WRITE32(MSDC_INT(ch0), msdc_host0->MsdcAccuVect);
                return MSDC_SUCCESS;
            }
            else
            {
                if(time_after(jiffies, u4JiffSt + timeoutCnt/(1000/HZ)))
                {
                    MSDC_LOG(MSG_LVL_WARN, "[0]Wait HOST Controller Interrupt Timeout!\r\n");
                    return MSDC_FAILED;
                }
            }
        }
#endif
    }
    else
    {
        u4Ret = wait_for_completion_timeout(&comp0, timeoutCnt);

        if(u4Ret > 0)
        {
            if((msdc_host0->MsdcAccuVect & INT_SD_DATA_CRCERR) != 0)
            {
                MSDC_LOG(MSG_LVL_ERR, "[0]CRC ERROR, Interrupt Handle Function return directly!\n");
            return MSDC_FAILED;
        	}
            if((msdc_host0->MsdcAccuVect & INT_SD_AUTOCMD_RESP_CRCERR) != 0)
            {
                MSDC_LOG(MSG_LVL_ERR, "[0]Auto CMD12 Response CRC ERROR, We don't care this case!\n");
                return MSDC_SUCCESS;
            }
            return MSDC_SUCCESS;
        }    
        else
        {
            MSDC_LOG(MSG_LVL_ERR, "[1]Wait HOST Controller Interrupt Timeout-0(acc_vect%08X des_vect%08X)!\r\n", msdc_host0->MsdcAccuVect,
                                                                                                                      msdc_host0->desVect);
            MSDC_LOG(MSG_LVL_ERR, "[1]Information(cmd%d arg%08x len:%08x precmd%d prearg%08x)!\n", msdc_host0->cur_cmd.opcode, msdc_host0->cur_cmd.arg, 
                                                                          (msdc_host0->cur_cmd.data?(msdc_host0->cur_cmd.data->blocks)*(msdc_host0->cur_cmd.data->blksz):0),
                                                                           msdc_host0->pre_cmd.opcode, msdc_host0->pre_cmd.arg);
            MSDC_LOG(MSG_LVL_ERR, "[1]waitIntMode:%d fgMode:%d\n", msdc_host0->waitIntMode, fgMode); 
            goto tryEnd;
        }
  	}
tryEnd:
    {
        unsigned long u4JiffSt = 0;
        u4JiffSt = jiffies;
        while(1)
        {
            if((fgMode == 0) && ((MSDC_READ32(MSDC_INT(ch0)) & vector) != 0))
            {
                msdc_host0->MsdcAccuVect |= MSDC_READ32(MSDC_INT(ch0));
                MSDC_WRITE32(MSDC_INT(ch0), msdc_host0->MsdcAccuVect);
                return MSDC_SUCCESS;
            }
            else if((fgMode == 1) && ((MSDC_READ32(MSDC_INT(ch0)) & vector) == vector))
			{
                msdc_host0->MsdcAccuVect |= MSDC_READ32(MSDC_INT(ch0));
                MSDC_WRITE32(MSDC_INT(ch0), msdc_host0->MsdcAccuVect);
                return MSDC_SUCCESS;
            }
            else
            {
                if(time_after(jiffies, u4JiffSt + timeoutCnt))
                {
                    MSDC_LOG(MSG_LVL_ERR, "[1]Wait HOST Controller Interrupt Timeout-1!\r\n");
                    return MSDC_FAILED;
        		}
  			}
        }    	
    }

    // Timeout case
    return MSDC_FAILED;
}

INT32 MsdcSetClkfreq0(UINT32 clkFreq)
{
    UINT32 sdClkSel = 0, expFreq = 0, index = 0;
    clkFreq /= (1000*1000);
    
    index = sizeof(_u4MsdcFreq0)/sizeof(UINT32) - 1;
    if(clkFreq >= _u4MsdcFreq0[0])
    {
        sdClkSel = _u4MsdcFreqSelVal0[0];
        expFreq = _u4MsdcFreq0[0];    	
    }
    else if(clkFreq <= _u4MsdcFreq0[index])
    {
        sdClkSel = _u4MsdcFreqSelVal0[index];
        expFreq = _u4MsdcFreq0[index]; 	
    }
    else
    {
        for(index -= 1;index > 0;index--)
        {
            if((clkFreq >= _u4MsdcFreq0[index]) && (clkFreq < _u4MsdcFreq0[index-1]))
            {
                sdClkSel = _u4MsdcFreqSelVal0[index];
                expFreq = _u4MsdcFreq0[index]; 
                break;	
            }	
        }
    }
    MSDC_LOG(MSG_LVL_INFO, "[0]Request Clk:%dMHz Bus Clk:%dMHz, SelVal:%d\n", clkFreq, expFreq, sdClkSel);
    if (_u4MsdcCurFreq0 == expFreq)
    {
        return MSDC_SUCCESS;
    }

    // Gate msdc_src_clk
    MSDC_SETBIT(MSDC_SCLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_GATE_BIT);

    // set clock mode (DIV mode)
    MSDC_CLRBIT(MSDC_CFG(ch0), (((UINT32)0x03) << 16));
    if (expFreq >= 1)
    {
        MSDC_SETBIT(MSDC_CFG(ch0), (((UINT32)0x01) << 16));
		
    }
    else
    {
        MSDC_CLRBIT(MSDC_CFG(ch0), (((UINT32)0xFF) << 8));
        MSDC_SETBIT(MSDC_CFG(ch0), (((UINT32)0x11) << 8));
    }
    
    // Clr msdc_src_clk selection
    MSDC_CLRBIT(MSDC_SCLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_SLECTION_MASK);  
    MSDC_SETBIT(MSDC_SCLOCK_SELECTION_REG0, sdClkSel<<0);  

    // Disable gating msdc_src_clk
    MSDC_CLRBIT(MSDC_SCLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_GATE_BIT);    
	
    // Wait until clock is stable
    if (MSDC_FAILED == MsdcWaitClkStable0())
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]Set Bus Clock as %d(MHz) Failed!\n", expFreq);
        return MSDC_FAILED;
    }

    MSDC_LOG(MSG_LVL_INFO, "[0]Set Bus Clock as %d(MHz) Success!\n", expFreq);	
    _u4MsdcCurFreq0 = expFreq;

    return MSDC_SUCCESS;
}

INT32 MsdcSetBusWidth0(INT32 busWidth)
{
    MSDC_LOG(MSG_LVL_INFO, "[0]Set Bus Width:%d\n", (1<<busWidth));

    /* Modify MSDC Register Settings */
    if (MMC_BUS_WIDTH_1 == busWidth)
    {
        MSDC_WRITE32(SDC_CFG(ch0), (MSDC_READ32(SDC_CFG(ch0)) & ~SDC_CFG_BW_MASK) | (0x00 <<  SDC_CFG_BW_SHIFT));
    }
    else if (MMC_BUS_WIDTH_4 == busWidth)
    {
        MSDC_WRITE32(SDC_CFG(ch0), (MSDC_READ32(SDC_CFG(ch0)) & ~SDC_CFG_BW_MASK) | (0x01 <<  SDC_CFG_BW_SHIFT));
    }
    else if (MMC_BUS_WIDTH_8 == busWidth)
    {
        MSDC_WRITE32(SDC_CFG(ch0), (MSDC_READ32(SDC_CFG(ch0)) & ~SDC_CFG_BW_MASK) | (0x02 <<  SDC_CFG_BW_SHIFT));
    }

    return MSDC_SUCCESS;
}

void MsdcSetIos0(struct mmc_host *mmc, struct mmc_ios *ios)
{
    UINT32 clock = ios->clock;
    UINT32 busWidth = ios->bus_width;

#if 0//def CONFIG_OPM
    if (force_suspend != 0)
    {
        MSDC_LOG(MSG_LVL_INFO, "[0]MSDC skip set clock(%d) and buswidth(%d) if suspend ~~~~~~~~~~~~~~~~\n", clock, busWidth);
        return;
    }
#endif
    MsdcSetClkfreq0(clock);

    MsdcSetBusWidth0(busWidth);

}

void MsdcSetupCmd0(struct mmc_command *cmd, struct mmc_data *data)
{
    UINT32 u4sdcCmd = 0;
	UINT32 u4DataLen = 0;

    /* Figure out the response type */
    switch(cmd->opcode)
    {
    case MMC_GO_IDLE_STATE:
        u4sdcCmd |= SDC_CMD_RSPTYPE_NO;
        break;
    case MMC_SEND_OP_COND:
        u4sdcCmd |= SDC_CMD_RSPTYPE_R3;
        break;
    case MMC_ALL_SEND_CID:
    case MMC_SEND_CSD:
    case MMC_SEND_CID:
        u4sdcCmd |= SDC_CMD_RSPTYPE_R2;
        break;
    case MMC_SET_RELATIVE_ADDR:
        u4sdcCmd |= SDC_CMD_RSPTYPE_R1;
        break;
    case MMC_SWITCH:
        u4sdcCmd |= ((mmc_resp_type(cmd) == MMC_RSP_R1)?(SDC_CMD_RSPTYPE_R1):(SDC_CMD_RSPTYPE_R1B));
        if(data)
        {
            u4sdcCmd |= (DTYPE_SINGLE_BLK | SDC_CMD_READ);
            MSDC_WRITE32(SDC_BLK_NUM(ch0), 1);
        }
        break;
    case MMC_SELECT_CARD:
        u4sdcCmd |= SDC_CMD_RSPTYPE_R1;
        break;
    case MMC_SEND_EXT_CSD:
        u4sdcCmd |= ((mmc_resp_type(cmd) == MMC_RSP_R1)?(SDC_CMD_RSPTYPE_R1):(SDC_CMD_RSPTYPE_R7));
        if(data)
        {
            u4sdcCmd |= (DTYPE_SINGLE_BLK | SDC_CMD_READ);
            MSDC_WRITE32(SDC_BLK_NUM(ch0), 1);
        }
        break;
    case MMC_STOP_TRANSMISSION:
        u4sdcCmd |= (SDC_CMD_STOP | SDC_CMD_RSPTYPE_R1);
        break;
    case MMC_SEND_STATUS:
        u4sdcCmd |= SDC_CMD_RSPTYPE_R1;
        if(data)
        {
            u4sdcCmd |= (DTYPE_SINGLE_BLK | SDC_CMD_READ);
            MSDC_WRITE32(SDC_BLK_NUM(ch0), 1);
        }
        break;
    case MMC_SET_BLOCKLEN:
        u4sdcCmd |= SDC_CMD_RSPTYPE_R1;
        break;
    case MMC_READ_SINGLE_BLOCK:
        u4sdcCmd |= (SDC_CMD_RSPTYPE_R1 | DTYPE_SINGLE_BLK | SDC_CMD_READ);
        MSDC_WRITE32(SDC_BLK_NUM(ch0), 1);
        break;
    case MMC_READ_MULTIPLE_BLOCK:
        u4sdcCmd |= (SDC_CMD_RSPTYPE_R1 | DTYPE_MULTI_BLK | SDC_CMD_READ);
        if(data)
        {
            MSDC_WRITE32(SDC_BLK_NUM(ch0), data->blocks);
        }
        else
        {
            MSDC_WRITE32(SDC_BLK_NUM(ch0), 0x0);
        }
        break;
    case MMC_WRITE_BLOCK:
        u4sdcCmd |= (SDC_CMD_RSPTYPE_R1 | DTYPE_SINGLE_BLK | SDC_CMD_WRITE);
        MSDC_WRITE32(SDC_BLK_NUM(ch0), 1);
        break;
    case MMC_WRITE_MULTIPLE_BLOCK:
        u4sdcCmd |= (SDC_CMD_RSPTYPE_R1 | DTYPE_MULTI_BLK | SDC_CMD_WRITE);
        if(data)
        {
            MSDC_WRITE32(SDC_BLK_NUM(ch0), data->blocks);
        }
        else
        {
            MSDC_WRITE32(SDC_BLK_NUM(ch0), 0x0);
        }
        break;
    case SD_APP_OP_COND:
        u4sdcCmd |= SDC_CMD_RSPTYPE_R3;
        break;
    case SD_APP_SEND_SCR:			
        u4sdcCmd |= (DTYPE_SINGLE_BLK | SDC_CMD_READ | SDC_CMD_RSPTYPE_R1);
        MSDC_WRITE32(SDC_BLK_NUM(ch0), 1);			
        break; 
    case MMC_APP_CMD:
        u4sdcCmd |= SDC_CMD_RSPTYPE_R1;
        break;
    }

    // Set Blk Length
    if(data)
    {
        u4DataLen = (data->blksz)*(data->blocks);
		u4DataLen = (u4DataLen >= SDHC_BLK_SIZE)?SDHC_BLK_SIZE:u4DataLen;
        u4sdcCmd |= ((u4DataLen) << SDC_CMD_LEN_SHIFT);
    }

    // Set SDC_CMD.CMD
    u4sdcCmd |= (cmd->opcode & 0x3F);

    if(data && data->stop)
    {
        u4sdcCmd |= SDC_CMD_AUTO_CMD12;
    }

    MSDC_LOG(MSG_LVL_INFO, "[0]CMD%d CMD:%08X, ARGU:%08X, BLKNUM:%08X\n", cmd->opcode, u4sdcCmd, cmd->arg, 
		                                                                 MSDC_READ32(SDC_BLK_NUM(ch0)));

    // Set SDC Argument
    MSDC_WRITE32(SDC_ARG(ch0), cmd->arg);

    /* Send the commands to the device */
    MSDC_WRITE32(SDC_CMD(ch0), u4sdcCmd);

}

void MsdcHandleResp0(struct mmc_command *cmd)
{
    // Handle the response
    switch (mmc_resp_type(cmd))
    {
    case MMC_RSP_NONE:
        MSDC_LOG(MSG_LVL_INFO, "[0]CMD%d ARG 0x%08X RESPONSE_NO\r\n", cmd->opcode, cmd->arg);
        break;
    case MMC_RSP_R1:
    case MMC_RSP_R1B:
        cmd->resp[0] = MSDC_READ32(SDC_RESP0(ch0));
        MSDC_LOG(MSG_LVL_INFO, "[0]CMD%d ARG 0x%08X RESPONSE_R1/R1B/R5/R6/R6 0x%08X\r\n", cmd->opcode, cmd->arg, cmd->resp[0]);
        break;
    case MMC_RSP_R2:
        cmd->resp[0] = MSDC_READ32(SDC_RESP3(ch0));
        cmd->resp[1] = MSDC_READ32(SDC_RESP2(ch0));
        cmd->resp[2] = MSDC_READ32(SDC_RESP1(ch0));
        cmd->resp[3] = MSDC_READ32(SDC_RESP0(ch0));
        MSDC_LOG(MSG_LVL_INFO, "[0]CMD%d ARG 0x%08X RESPONSE_R2 0x%08X 0x%08X 0x%08X 0x%08X\r\n", cmd->opcode, cmd->arg, cmd->resp[0], cmd->resp[1], cmd->resp[2], cmd->resp[3]);
        break;
    case MMC_RSP_R3:
        cmd->resp[0] = MSDC_READ32(SDC_RESP0(ch0));
        MSDC_LOG(MSG_LVL_INFO, "[0]CMD%d ARG 0x%08X RESPONSE_R3/R4 0x%08X\r\n", cmd->opcode, cmd->arg, cmd->resp[0]);
        break;
    }

	if(cmd->opcode == MMC_ALL_SEND_CID)
	{
        MsdcFindDev0(cmd->resp);
	}
}

int MsdcReqCmd0(struct mmc_command *cmd, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS;
    UINT32 u4CmdDoneVect;
    cmd->error = 0;
    msdc_host0->MsdcAccuVect = 0;
    msdc_host0->desVect = INT_SD_CMDRDY| INT_SD_CMDTO | INT_SD_RESP_CRCERR;
    msdc_host0->waitIntMode = 0;
	
    // Check if rx/tx fifo is zero
    MsdcChkFifo0();
	
    // Clear interrupt Vector
    MsdcClrIntr0();
	
    MSDC_LOG(MSG_LVL_INFO, "[0]MsdcSendCmd : CMD%d ARG%08X!!\r\n", cmd->opcode, cmd->arg);
	
	  // modified by shunli.wang at 2012/10/28
	  // I think it is not necessary to check host idle status before cmd12/cmd13 is issued.
	  // But for some emmc, we need to wait host idle status before cmd13 is issued.
	  // (For toshiba emmc, it will be timeout to send cmd13 for busy state check after high speed switch)
	  // But if not pass, we still go on.
	  //if((cmd->opcode != MMC_STOP_TRANSMISSION) && (cmd->opcode != MMC_SEND_STATUS))
	if(cmd->opcode != MMC_STOP_TRANSMISSION)
	{
        if(MSDC_FAILED == MsdcWaitHostIdle0())
   	 	{
        	if(cmd->opcode != MMC_SEND_STATUS)
            {
        		i4Ret = ERR_CMD_FAILED;
        		cmd->error = ERR_HOST_BUSY;
        		goto ErrorEnd;
            }
        }
    }
	
    MsdcSetupCmd0(cmd, data);
	
    // Wait for command and response if existed
    u4CmdDoneVect = INT_SD_CMDRDY | INT_SD_CMDTO | INT_SD_RESP_CRCERR;

    if (MSDC_SUCCESS != MsdcWaitIntr0(u4CmdDoneVect, CMD_TIMEOUT, 0))
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]Failed to send CMD/RESP, DoneVect = 0x%x.\r\n", u4CmdDoneVect);
        i4Ret = ERR_CMD_FAILED;
        cmd->error = ERR_CMD_NO_INT;
        goto ErrorEnd;
    }
	
    //printf("Interrupt = %08X\n", msdc_host0->MsdcAccuVect);
	
    if (msdc_host0->MsdcAccuVect & INT_SD_CMDTO)
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]CMD%d ARG 0x%08X - CMD Timeout (AccuVect 0x%08X INTR 0x%08X).\r\n", cmd->opcode, cmd->arg, msdc_host0->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)));
        i4Ret = ERR_CMD_FAILED;
        cmd->error = ERR_CMD_RESP_TIMEOUT;
        goto ErrorEnd;
    }
    else if (msdc_host0->MsdcAccuVect & INT_SD_RESP_CRCERR)
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]CMD%d ARG 0x%08X - CMD CRC Error (AccuVect 0x%08X INTR 0x%08X).\r\n", cmd->opcode, cmd->arg, msdc_host0->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)));
        i4Ret = ERR_CMD_FAILED;
        cmd->error = ERR_CMD_RESP_CRCERR;
        goto ErrorEnd;
    }
    else if ((msdc_host0->MsdcAccuVect & (~(INT_SD_CMDRDY))) || (0 != MSDC_READ32(MSDC_INT(ch0))))
    {
        MSDC_LOG(MSG_LVL_INFO, "[0]CMD%d ARG 0x%08X - UnExpect status (AccuVect 0x%08X INTR 0x%08X).\r\n", cmd->opcode, cmd->arg, msdc_host0->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)));
        //i4Ret = ERR_CMD_FAILED;
        //cmd->error = ERR_CMD_UNEXPECT_INT;
        //goto ErrorEnd;
    }
	
    // Handle the response
    MsdcHandleResp0(cmd);

ErrorEnd:
    return i4Ret;

}

int MsdcReqCmdTune0(struct msdc_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
    int result = MSDC_SUCCESS;
    UINT32 rsmpl = 0, cur_rsmpl = 0, orig_rsmpl = 0;
    UINT32 rrdly = 0, cur_rrdly = 0, orig_rrdly = 0;
    UINT32 skip = 1;

    MSDC_LOG(MSG_LVL_ERR, "[0]----->Go into Command Tune!\n");
	
    MsdcReset0();
    MsdcClrFifo0();
    MsdcClrIntr0();

    orig_rsmpl = ((MSDC_READ32(MSDC_IOCON(ch0)) & MSDC_IOCON_R_SMPL) >> MSDC_IOCON_R_SMPL_SHIFT);
    orig_rrdly = ((MSDC_READ32(PAD_TUNE(ch0)) & PAD_CMD_RESP_RXDLY) >> PAD_CMD_RESP_RXDLY_SHIFT);

    rrdly = 0; 
    do 
    {
        for (rsmpl = 0; rsmpl < 2; rsmpl++) 
        {
            /* Lv1: R_SMPL[1] */		
            cur_rsmpl = (orig_rsmpl + rsmpl) % 2;		  
            if (skip == 1) 
            {
                skip = 0;	
                continue;	
            }

            MSDC_CLRBIT(MSDC_IOCON(ch0), MSDC_IOCON_R_SMPL);
            MSDC_SETBIT(MSDC_IOCON(ch0), (cur_rsmpl << MSDC_IOCON_R_SMPL_SHIFT));

            result = MsdcReqCmd0(cmd, data);
            if(result == MSDC_SUCCESS)
            {
                MSDC_LOG(MSG_LVL_ERR, "[1]Command Tune Success\n");
				
                return MSDC_SUCCESS;
            }
            else
            {
                MsdcReset0();
                MsdcClrFifo0();
                MsdcClrIntr0();
            }
        }

#if 1
        /* Lv2: PAD_CMD_RESP_RXDLY[26:22] */              	
        cur_rrdly = (orig_rrdly + rrdly + 1) % 32;
        MSDC_CLRBIT(PAD_TUNE(ch0), PAD_CMD_RESP_RXDLY);
        MSDC_SETBIT(PAD_TUNE(ch0), (cur_rrdly << PAD_CMD_RESP_RXDLY_SHIFT));
    }while (++rrdly < 32);
#else
    }while (0);
#endif

    return result;
}

INT32 MsdcReqDataStop0(struct msdc_host *host)
{   
    INT32 i4Ret;
    struct mmc_command stop;
	
    memset(&stop, 0, sizeof(struct mmc_command));
    stop.opcode = MMC_STOP_TRANSMISSION;
    stop.arg = 0;
    stop.flags = MMC_RSP_R1B | MMC_CMD_AC;
	
    i4Ret = MsdcReqCmd0(&stop, NULL);
    MSDC_LOG(MSG_LVL_ERR, "[0]Stop Sending-Data State(%d)!\n", i4Ret);

    return i4Ret;    
}

INT32 MsdcReqCardStatus0(struct msdc_host *host, UINT32 *status)
{
    INT32 i4Ret;
    struct mmc_command query_status;
	
    memset(&query_status, 0, sizeof(struct mmc_command));
    query_status.opcode = MMC_SEND_STATUS;
    query_status.arg = (host->mmc->card->rca << 16);
    query_status.flags = MMC_RSP_R1 | MMC_CMD_AC;
	
    i4Ret = MsdcReqCmd0(&query_status, NULL);

    if(status)
    {
	    *status = query_status.resp[0];
    }

	return i4Ret;

}

INT32 MsdcCheckCardBusy(struct msdc_host *host)
{
    UINT32 err = 0, status = 0;
    UINT32 i = 0;

	do
	{
        err = MsdcReqCardStatus0(host, &status);
		if(err)
		{
		    MSDC_LOG(MSG_LVL_ERR, "[0]get card status failed(%d)\n", err);
            return err;
		}
         
        udelay(1000);
        i++;
    }while((R1_CURRENT_STATE(status) != 4) && (i < 1000));
    if(i >= 1000)
    {
    	  MSDC_LOG(MSG_LVL_ERR, "[0]card is always in busy state(%08X)\n", status);
        err = -1;
    }
    else
    {
        err = 0;
    }
	return err;
}

INT32 MsdcErrorHandling0(struct msdc_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
    INT32 i4Ret = MSDC_SUCCESS, iTmpRet = 0;
	UINT32 status = 0;
    
    MSDC_LOG(MSG_LVL_ERR, "[0]Start Error Handling(CMD%d ARGU %08X,blksz %08X blknum %08X)...!\n", cmd->opcode, cmd->arg, 	                                                                              data->blksz, data->blocks);

    // Reset MSDC
    MsdcReset0();

    // Stop DMA
    if(host->data_mode > 1)
        MsdcStopDMA0();

    // Clear FIFO and wait it becomes 0
    i4Ret = MsdcClrFifo0();
    if(MSDC_SUCCESS != i4Ret)
    {
        goto ErrorEnd;	 
    }

    // Clear MSDC interrupts and make sure all are cleared
    MsdcClrIntr0();
    if  (0x00  != MSDC_READ32(MSDC_INT(ch0)))
    {
        i4Ret = MSDC_FAILED;
        goto ErrorEnd;	
    }

    // Send Stop Command for Multi-Write/Read
    if(data && ((cmd->opcode == MMC_READ_SINGLE_BLOCK) ||
		       (cmd->opcode == MMC_READ_MULTIPLE_BLOCK) || 
		       (cmd->opcode == MMC_WRITE_BLOCK) ||
		       (cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK)))
    {
        iTmpRet = MsdcReqCardStatus0(host, &status); 
        if(iTmpRet)
        {
            MSDC_LOG(MSG_LVL_ERR, "read card status before stop command failed!\n");
        }
        else
        {
            MSDC_LOG(MSG_LVL_ERR, "read card status before stop command(%08X)!\n", status);
        }
    
        if((data->blocks > 1) && (((status>>9)&0xF)!=0x4))
        {
            if (MsdcReqDataStop0(host)) 
            {
                MSDC_LOG(MSG_LVL_WARN, "[0]mmc fail to send stop cmd\n");
            }
        }

		// polling the card status to idle
		if(MsdcCheckCardBusy(host))
		{
            i4Ret = MSDC_FAILED;
			goto ErrorEnd;
		}		
    }

    MSDC_LOG(MSG_LVL_ERR, "[0]End Error Handling...!\n");

ErrorEnd:
    return i4Ret;

}

static inline void msdc0_init_sg(struct msdc_host *host, struct mmc_data *data);
static inline int msdc0_next_sg(struct msdc_host *host);
static inline char *msdc0_sg_to_buffer(struct msdc_host *host);
INT32 MsdcReqDataPIO0(struct msdc_host *host, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS;
    UINT32 fgAutoCmd12, IntrWaitVect = 0, IntrCheckVect = 0;
    UINT32 i, u4Tmp;
	
    UINT32 u4RxFifoCnt, u4TxFifoCnt;
    UINT32 u4BufLen = (UINT32)((data->blocks)*(data->blksz));
    UINT8 *buffer;
    UINT32 fifo = 4;
    UINT8 fifo_buf[4] = {0x0};
    UINT32 u4RxCnt = 0;
	
    buffer = msdc0_sg_to_buffer(host) + host->offset;
	
    if(buffer == NULL)
    {
        i4Ret = ERR_DAT_FAILED;
        MSDC_LOG(MSG_LVL_INFO, "[0]End Error Handling...! buffer NULL --- 1 ---\n");
        goto ErrorEnd;
    }
    fgAutoCmd12 = (data->stop)?1:0;

    if(fgAutoCmd12)
    {		 
        IntrWaitVect = INT_SD_XFER_COMPLETE | INT_SD_AUTOCMD_RDY; 	   
        IntrCheckVect = INT_SD_XFER_COMPLETE | INT_SD_AUTOCMD_RDY;	 
    }	  
    else	  
    { 	   
        IntrWaitVect = INT_SD_XFER_COMPLETE; 	   
        IntrCheckVect = INT_SD_XFER_COMPLETE;	
	}
	
    // Read
    if (data->flags & MMC_DATA_READ)
    {
        while (u4BufLen)
        {
            // wait until fifo has enough data
            u4RxFifoCnt = (MSDC_READ32(MSDC_FIFOCS(ch0)) & MSDC_FIFOCS_RXFIFOCNT_MASK);
	
            while ((u4BufLen) && (sizeof(int) <= u4RxFifoCnt))
            {
                // Read Data
                *((UINT32 *)fifo_buf) = MSDC_READ32(MSDC_RXDATA(ch0));
	
                u4RxFifoCnt -= sizeof(int);
                u4RxCnt += sizeof(int);
	
                for (i = 0; i < fifo; i++)
                {
                    *buffer = fifo_buf[i];
                    buffer++;
                    host->offset++;
                    host->remain--;
	
                    data->bytes_xfered++;
	
                    /*
                                * End of scatter list entry?
                                */
                    if (host->remain == 0)
                    {
                        /*
                                      * Get next entry. Check if last.
                                      */
                        if (!msdc0_next_sg(host))
                        {
                            break;
                        }
	
                        buffer = msdc0_sg_to_buffer(host);
                    }
                }
	
                if(u4RxCnt == SDHC_BLK_SIZE)
                {
                    // Check CRC error happens by every 512 Byte
                    // Check if done vector occurs
                    u4Tmp = MSDC_READ32(MSDC_INT(ch0));
                    if(INT_SD_DATTO & u4Tmp)
                        data->error = ERR_DAT_TIMEOUT;
                    else if(INT_SD_DATA_CRCERR & u4Tmp)
                        data->error = ERR_DAT_CRCERR;
							 
                    if(data->error)
                    {
                        i4Ret = ERR_DAT_FAILED;
                        MSDC_LOG(MSG_LVL_ERR, "[0]Read Error Break !!\r\n");
                        goto ErrorEnd;
                    }
                    else
                    {
                        u4RxCnt = 0;
                        u4BufLen -= SDHC_BLK_SIZE;
                    }
                }
	
            }
        }
    }
    else
    {
        while (u4BufLen)
        {
            // Check if error done vector occurs
            u4Tmp = MSDC_READ32(MSDC_INT(ch0));
            if(INT_SD_DATTO & u4Tmp)
                data->error = ERR_DAT_TIMEOUT;
            else if(INT_SD_DATA_CRCERR & u4Tmp)
                data->error = ERR_DAT_CRCERR;
							 
            if(data->error)
            {
                i4Ret = ERR_DAT_FAILED;
                MSDC_LOG(MSG_LVL_ERR, "[0]Write Error Break !!\r\n");
                goto ErrorEnd;
            }
	
            // wait until fifo has enough space
            while(1)
            {
                if((MSDC_READ32(MSDC_FIFOCS(ch0)) & MSDC_FIFOCS_TXFIFOCNT_MASK) == 0)
                {
                    break;
                }
            }
	
            u4TxFifoCnt = MSDC_FIFO_LEN;
	
            if (sizeof(int) <= u4TxFifoCnt)
            {
                while ((u4BufLen) && (sizeof(int) <= u4TxFifoCnt))
                {
                    // gather byte data into fifo_buf
                    for (i = 0; i < fifo; i++)
                    {
                        fifo_buf[i] = *buffer;
                        buffer++;
                        host->offset++;
                        host->remain--;
	
                        data->bytes_xfered++;
	
                        /*
                                      * End of scatter list entry?
                                      */
                        if (host->remain == 0)
                        {
                            /*
                                            * Get next entry. Check if last.						  
                                            */
                            if (!msdc0_next_sg(host))
                            {
                                break;
                            }
	
                            buffer = msdc0_sg_to_buffer(host);
                        }
                    }
	
                    // Write Data
                    MSDC_WRITE32(MSDC_TXDATA(ch0), *((UINT32 *)(fifo_buf)));
                    u4TxFifoCnt -= sizeof(int);
                    u4BufLen -= sizeof(int);
                }
            }
        }
    }
	
    // Wait for data complete
    if (MSDC_SUCCESS != MsdcWaitIntr0(IntrWaitVect, DAT_TIMEOUT, 1))
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]Wait Intr timeout (AccuVect 0x%08X INTR 0x%08X).\r\n", msdc_host0->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)));
        i4Ret = ERR_DAT_FAILED;
        data->error = ERR_DAT_NO_INT;
        goto ErrorEnd;
    }
	
    if ((msdc_host0->MsdcAccuVect & ~(IntrCheckVect)) || (0 != MSDC_READ32(MSDC_INT(ch0))))
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]UnExpect status (AccuVect 0x%08X INTR 0x%08X).\r\n", msdc_host0->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)));
        i4Ret = ERR_DAT_FAILED;
        data->error = ERR_DAT_UNEXPECT_INT;
        goto ErrorEnd;
    }

ErrorEnd:
    return i4Ret;

}

INT32 MsdcReqDataBasicDMA0(struct msdc_host *host, struct mmc_data *data)
{
    INT32 i4Ret = MSDC_SUCCESS;
    UINT32 u4BufLen = (UINT32)((data->blocks)*(data->blksz)), u4ActuralLen, u4PhyAddr;
    UINT8 *buffer;	
    UINT32 fgAutoCmd12, IntrWaitVect = 0, IntrCheckVect = 0;
#ifdef CC_MTD_ENCRYPT_SUPPORT 
    UINT32 addr_blk = 0;
#endif

    fgAutoCmd12 = (data->stop)?1:0;

    if(fgAutoCmd12)
    {		 
        IntrWaitVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_SD_AUTOCMD_RDY; 	   
        IntrCheckVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_SD_AUTOCMD_RDY;	 
    }	  
    else	  
    { 	   
        IntrWaitVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE; 	   
        IntrCheckVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE;	
    }
    //add by shunli.wang at 2012/12/27
    msdc_host0->MsdcAccuVect = 0;
    msdc_host0->desVect = IntrWaitVect;
    msdc_host0->waitIntMode = 1;
	
    while(u4BufLen)
    {
        /*
            * End of scatter list entry?
            */
        if (host->remain == 0)
        {
            /*
             * Get next entry. Check if last.
             */
            if (!msdc0_next_sg(host))
            {
                break;
            }
        }
#ifdef CC_MTD_ENCRYPT_SUPPORT     
        BUG_ON(host->cur_sg->length > BASIC_DMA_MAX_LEN); 
        #if(0)
        memcpy(_pu1MsdcBuf_Aes_Align, msdc1_sg_to_buffer(host) + host->offset, MIN_NUM(BASIC_DMA_MAX_LEN, host->remain));
        #else
        sg_copy_to_buffer(host->cur_sg,1,_pu1MsdcBuf_Aes_Align,MIN_NUM(BASIC_DMA_MAX_LEN, host->remain));
        #endif
        
        buffer = _pu1MsdcBuf_Aes_Align;
        
        if((host->cur_cmd.opcode == MMC_WRITE_BLOCK) ||
    	     (host->cur_cmd.opcode == MMC_WRITE_MULTIPLE_BLOCK))
        {
    	      addr_blk = mmc_card_blockaddr(host->mmc->card)?(host->cur_cmd.arg):(host->cur_cmd.arg>>9); 	
            if(msdc1_partition_encrypted(addr_blk))
            { 
            	  //MSDC_LOG(MSG_LVL_ERR, "[1]check the value(encrypt.1)(gbuffer val: 0x%08x, sg val: 0x%08x)\n", *((UINT32 *)buffer),
            	                                                            //*((UINT32 *)(msdc1_sg_to_buffer(host) + host->offset)));
                if(msdc1_aes_encryption_buf((UINT32)buffer, MIN_NUM(BASIC_DMA_MAX_LEN, host->remain)))
                {
                    MSDC_LOG(MSG_LVL_ERR, "[1]aes encryption failed!\r\n");
                    return MSDC_FAILED;    	
                }	
                //MSDC_LOG(MSG_LVL_ERR, "[1]check the value(encrypt.2)(gbuffer val: 0x%08x, sg val: 0x%08x)\n", *((UINT32 *)buffer),
            	                                                            //*((UINT32 *)(msdc1_sg_to_buffer(host) + host->offset)));
            }
        }
#else	
        buffer = msdc0_sg_to_buffer(host) + host->offset;
        if(buffer == NULL)
       	{
            i4Ret = ERR_DAT_FAILED;
            MSDC_LOG(MSG_LVL_INFO, "[0]End Error Handling...! buffer NULL --- 4 ---\n");
            goto ErrorEnd;
       	}
#endif	
        u4PhyAddr = dma_map_single(NULL, buffer, host->remain, (data->flags & MMC_DATA_READ)?(DMA_FROM_DEVICE):(DMA_TO_DEVICE));
	
        if(u4PhyAddr >= mt53xx_cha_mem_size)
            printk("[0]!!!!MTK MSDC: DMA to ChB %p\n", buffer);
        MSDC_WRITE32(DMA_SA(ch0), u4PhyAddr);
	
        if((host->num_sg == 1) && ((host->remain) <= BASIC_DMA_MAX_LEN))
        {
            u4ActuralLen = host->remain;
            MSDC_LOG(MSG_LVL_INFO, "[0](Last)AccLen = %08X\r\n", u4ActuralLen);
            msdc_host0->desVect = IntrWaitVect; //avoid isr/thread context race condition
            msdc_host0->waitIntMode = 1; 
            MSDC_WRITE32(DMA_CTRL(ch0), ((u4ActuralLen) << DMA_CTRL_XFER_SIZE_SHIFT) | (DMA_BST_64 << DMA_CTRL_BST_SHIFT) | DMA_CTRL_LAST_BUF | DMA_CTRL_START);
	
            // Wait for sd xfer complete
            if (MSDC_SUCCESS != MsdcWaitIntr0(IntrWaitVect, DAT_TIMEOUT, 1)) 
            {
                MSDC_LOG(MSG_LVL_ERR, "[0](L)%s: Failed to send/receive data (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, host->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)), __FILE__, __LINE__);
                i4Ret = ERR_DAT_FAILED;
                data->error = ERR_DAT_NO_INT;
                goto ErrorEnd;
            }
	
            if ((host->MsdcAccuVect & ~(IntrCheckVect)) || (0 != MSDC_READ32(MSDC_INT(ch0)))) 
            {
                MSDC_LOG(MSG_LVL_ERR, "[0]%s: Unexpected status (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, host->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)), __FILE__, __LINE__);
                i4Ret = ERR_DAT_FAILED;
                data->error = ERR_DAT_UNEXPECT_INT;
                goto ErrorEnd;
            }
	
            // Check DMA status
            if (0 != (MSDC_READ32(DMA_CFG(ch0)) & (DMA_CFG_DMA_STATUS))) 
            {
                MSDC_LOG(MSG_LVL_ERR, "[0]%s: Incorrect DMA status. DMA_CFG: 0x%08X\r\n", __FUNCTION__, MSDC_READ32(DMA_CFG(ch0)));
                i4Ret = ERR_DAT_FAILED;
                data->error = ERR_DMA_STATUS_FAILED;
                goto ErrorEnd;
            }
        }
        else
        {
            u4ActuralLen = ((host->remain) <= BASIC_DMA_MAX_LEN)?(host->remain):(BASIC_DMA_MAX_LEN);
            MSDC_LOG(MSG_LVL_INFO, "[0](NOT LAST)AccLen = %08X\r\n", u4ActuralLen);
            msdc_host0->desVect = INT_DMA_XFER_DONE; //avoid isr/thread context race condition
            msdc_host0->waitIntMode = 1; 
            MSDC_WRITE32(DMA_CTRL(ch0), (u4ActuralLen << DMA_CTRL_XFER_SIZE_SHIFT) | (DMA_BST_64 << DMA_CTRL_BST_SHIFT) | DMA_CTRL_START);
	
            if (MSDC_SUCCESS != MsdcWaitIntr0(INT_DMA_XFER_DONE, MSDC_WAIT_DATA_COMPLETE_TIMEOUT_LIMIT_COUNT, 1)) 
            {
                MSDC_LOG(MSG_LVL_ERR, "[0](N)%s: Failed to send/receive data (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, host->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)), __FILE__, __LINE__);
                i4Ret = ERR_DAT_FAILED;
                data->error = ERR_DAT_NO_INT;
                goto ErrorEnd;
            }
	
            if ((host->MsdcAccuVect & ~(INT_DMA_XFER_DONE)) || (0 != MSDC_READ32(MSDC_INT(ch0)))) 
            {
                MSDC_LOG(MSG_LVL_ERR, "[0]%s: Unexpected status (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, host->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)), __FILE__, __LINE__);
                i4Ret = ERR_DAT_FAILED;
                data->error = ERR_DAT_UNEXPECT_INT;
                goto ErrorEnd;
            }
        }
	
        dma_unmap_single(NULL, u4PhyAddr, host->remain, (data->flags & MMC_DATA_READ)?(DMA_FROM_DEVICE):(DMA_TO_DEVICE));
#ifdef CC_MTD_ENCRYPT_SUPPORT        
        if((host->cur_cmd.opcode == MMC_READ_SINGLE_BLOCK) ||
    	     (host->cur_cmd.opcode == MMC_READ_MULTIPLE_BLOCK))
        {
    	      addr_blk = mmc_card_blockaddr(host->mmc->card)?(host->cur_cmd.arg):(host->cur_cmd.arg>>9); 	
            if(msdc1_partition_encrypted(addr_blk))
            { 
            	  //MSDC_LOG(MSG_LVL_ERR, "[1]check the value(decrypt.1)(gbuffer val: 0x%08x, sg val: 0x%08x)\n", *((UINT32 *)buffer),
            	                                                            //*((UINT32 *)(msdc1_sg_to_buffer(host) + host->offset)));
                if(msdc1_aes_decryption_buf((UINT32)buffer, u4ActuralLen))
                {
                    MSDC_LOG(MSG_LVL_ERR, "[1]aes encryption failed!\r\n");
                    return MSDC_FAILED;    	
                }	
                //MSDC_LOG(MSG_LVL_ERR, "[1]check the value(decrypt.2)(gbuffer val: 0x%08x, sg val: 0x%08x)\n", *((UINT32 *)buffer),
            	                                                            //*((UINT32 *)(msdc1_sg_to_buffer(host) + host->offset)));
            }
        }
        
        if(data->flags & MMC_DATA_READ)
        {
        	#if 0
            memcpy(msdc1_sg_to_buffer(host) + host->offset, _pu1MsdcBuf_Aes_Align, u4ActuralLen);  
            #else
            sg_copy_from_buffer(host->cur_sg,1,_pu1MsdcBuf_Aes_Align,u4ActuralLen);
            #endif
            //MSDC_LOG(MSG_LVL_ERR, "[1]check the value(decrypt.3)(gbuffer val: 0x%08x, sg val: 0x%08x)\n", *((UINT32 *)buffer),
            	                                                            //*((UINT32 *)(msdc1_sg_to_buffer(host) + host->offset))); 	
        }       
#endif
	
        u4BufLen -= u4ActuralLen;
        host->offset += u4ActuralLen;
        host->remain -= u4ActuralLen;
        data->bytes_xfered += u4ActuralLen;	
    }

ErrorEnd:
    return i4Ret;

}

INT32 MsdcCheckDMAStatus0(struct msdc_host *host, struct mmc_data *data)
{
	INT32 i4Ret = MSDC_SUCCESS;
	unsigned long u4JiffSt = 0;
	  
    if(MSDC_READ32(DMA_CFG(ch0)) & (DMA_CFG_GPD_CS_ERR))
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]Descriptor DMA GPD checksum error");
        i4Ret = ERR_DAT_FAILED;
        data->error = ERR_DMA_STATUS_FAILED;
    }
	
    if(MSDC_READ32(DMA_CFG(ch0)) & (DMA_CFG_BD_CS_ERR))
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]Descriptor DMA BD checksum error");
        i4Ret = ERR_DAT_FAILED;
        data->error = ERR_DMA_STATUS_FAILED;
    }

    u4JiffSt = jiffies;
    while(1)
    {
        // Check DMA status
        if (0 == (MSDC_READ32(DMA_CFG(ch0)) & (DMA_CFG_DMA_STATUS))) 
        {
            break;
        }
        
        // 20ms for the timeout
        if(time_after(jiffies, u4JiffSt + (HZ*20)/1000))
        {
            MSDC_LOG(MSG_LVL_ERR, "[0]%s: Incorrect DMA status. DMA_CFG: 0x%08X\r\n", __FUNCTION__, MSDC_READ32(DMA_CFG(ch0)));
            i4Ret = ERR_DAT_FAILED;
            data->error = ERR_DMA_STATUS_FAILED;
            break;
        }
    } 
    
    return i4Ret;	
}

INT32 MsdcReqDataDescriptorDMA0(struct msdc_host *host, struct mmc_data *data)
{
    INT32 i4Ret = MSDC_SUCCESS;
    UINT32 u4BufLen = (UINT32)((data->blocks)*(data->blksz));
    UINT32 fgAutoCmd12, IntrWaitVect = 0, IntrCheckVect = 0;

    fgAutoCmd12 = (data->stop)?1:0;

    if(fgAutoCmd12)
    {		 
        IntrWaitVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_SD_AUTOCMD_RDY | INT_DMA_Q_EMPTY; 	   
        IntrCheckVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_SD_AUTOCMD_RDY | INT_DMA_Q_EMPTY; 
    }	  
    else	  
    { 	   
        IntrWaitVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_DMA_Q_EMPTY; 	   
        IntrCheckVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_DMA_Q_EMPTY;
    }
    
    //add by shunli.wang at 2012/12/27
    msdc_host0->MsdcAccuVect = 0;
    msdc_host0->desVect = IntrWaitVect;
    msdc_host0->waitIntMode = 1;
    MsdcDescriptorConfig0(host, data);
    MSDC_SETBIT(DMA_CTRL(ch0), DMA_CTRL_START);
	
    // Wait for sd xfer complete
    if (MSDC_SUCCESS != MsdcWaitIntr0(IntrWaitVect, DAT_TIMEOUT, 1)) 
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]%s: Failed to send/receive data (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, host->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)), __FILE__, __LINE__);
        i4Ret = ERR_DAT_FAILED;
        data->error = ERR_DAT_NO_INT;
        goto ErrorEnd;
    }
	
    if ((host->MsdcAccuVect & ~(IntrCheckVect)) || (0 != MSDC_READ32(MSDC_INT(ch0)))) 
    {
        MSDC_LOG(MSG_LVL_INFO, "[0]%s: Unexpected status (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, host->MsdcAccuVect, MSDC_READ32(MSDC_INT(ch0)), __FILE__, __LINE__);
        i4Ret = ERR_DAT_FAILED;
        data->error = ERR_DAT_UNEXPECT_INT;
        goto ErrorEnd;
    }
	
	i4Ret = MsdcCheckDMAStatus0(host, data);
    if(MSDC_SUCCESS != i4Ret)
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]Check DMA status failed!\n");
        goto ErrorEnd;
    }
            
    MsdcDescriptorFlush0(host, data);       	
    data->bytes_xfered += u4BufLen;   
    
ErrorEnd:
    return i4Ret;
    	
}

INT32 MsdcReqData0(struct msdc_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS;
    data->error = 0;

    // Initialize sg for data transfer
    msdc0_init_sg(host, data);

    /*
      * Handle excessive data.
      */
    if (host->num_sg == 0)
        return MSDC_SUCCESS;

    if(PIO_DATA_MODE == host->data_mode)
    {
        i4Ret = MsdcReqDataPIO0(host, data);
        if(MSDC_SUCCESS != i4Ret)
        {
            goto ErrorEnd;
        }			
    }
    else if(BASIC_DMA_DATA_MODE == host->data_mode)
    {
        i4Ret = MsdcReqDataBasicDMA0(host, data);
        if(MSDC_SUCCESS != i4Ret)
        {
            goto ErrorEnd;
        }
    }
    else if(DESC_DMA_DATA_MODE == host->data_mode)
    {
        i4Ret = MsdcReqDataDescriptorDMA0(host, data);
        if(MSDC_SUCCESS != i4Ret)
        {
            goto ErrorEnd;
        }    	
    }

ErrorEnd:
	return i4Ret;
	
}

int MsdcSendCmd0(struct msdc_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS;

    i4Ret = MsdcReqCmd0(cmd, data);
    if(MSDC_SUCCESS != i4Ret)
    {
        if(data)
        {
            if(MSDC_SUCCESS != MsdcErrorHandling0(host, cmd, data))
            {
                goto ErrorEnd;
            }
        }
		
        i4Ret = MsdcReqCmdTune0(host, cmd, data);
        if(MSDC_SUCCESS != i4Ret)
        {
            goto ErrorEnd;
        }
    }
	
    if(data)
    {
        i4Ret = MsdcReqData0(host, cmd, data);
        if(MSDC_SUCCESS != i4Ret)
        {
            if(MSDC_SUCCESS != MsdcErrorHandling0(host, cmd, data))
            {
                goto ErrorEnd;
            }
        }
    }

ErrorEnd:
    return i4Ret;
    
}

int MsdcReqDataReadTune0(struct msdc_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
    UINT32 ddr=0;	
    UINT32 dcrc = 0;
    UINT32 rxdly, cur_rxdly0, cur_rxdly1;
    //UINT32 rxdly, cur_rxdly;
    UINT32 dsmpl, cur_dsmpl,  orig_dsmpl;
    UINT32 cur_dat0,  cur_dat1,  cur_dat2,  cur_dat3;
    UINT32 cur_dat4,  cur_dat5,  cur_dat6,  cur_dat7;
    UINT32 orig_dat0, orig_dat1, orig_dat2, orig_dat3;
    UINT32 orig_dat4, orig_dat5, orig_dat6, orig_dat7;
    //UINT32 cur_dat, orig_dat;
    INT32 result = -1;
    UINT32 skip = 1;

    MSDC_LOG(MSG_LVL_ERR, "[0]Go into Data Read Tune(%08X, %08X)!\n", MSDC_READ32(DAT_RD_DLY0(ch0)), MSDC_READ32(DAT_RD_DLY1(ch0)));

    orig_dsmpl = ((MSDC_READ32(MSDC_IOCON(ch0)) & MSDC_IOCON_D_SMPL) >> MSDC_IOCON_D_SMPL_SHIFT);

    /* Tune Method 2. */
    MSDC_CLRBIT(MSDC_IOCON(ch0), MSDC_IOCON_D_DLYLINE_SEL);
    MSDC_SETBIT(MSDC_IOCON(ch0), (1 << MSDC_IOCON_D_DLYLINE_SEL_SHIFT));

    MSDC_LOG(MSG_LVL_ERR, "[0]CRC(R) Error Register: %08X!\n", MSDC_READ32(SDC_DATCRC_STS(ch0)));

    rxdly = 0; 
    do 
    {
        for (dsmpl = 0; dsmpl < 2; dsmpl++) 
        {
            cur_dsmpl = (orig_dsmpl + dsmpl) % 2;
            if (skip == 1) 
            {
                skip = 0; 	
                continue;	
            } 

            MSDC_CLRBIT(MSDC_IOCON(ch0), MSDC_IOCON_D_SMPL);
            MSDC_SETBIT(MSDC_IOCON(ch0), (cur_dsmpl << MSDC_IOCON_D_SMPL_SHIFT));

            result = MsdcSendCmd0(host, cmd, data);

            dcrc = MSDC_READ32(SDC_DATCRC_STS(ch0));
            if(!ddr)
                dcrc &= (~SDC_DATCRC_STS_NEG);
			
            MSDC_LOG(MSG_LVL_INFO, "[0]TUNE_READ<%s> dcrc<0x%x> DATRDDLY0/1<0x%x><0x%x> dsmpl<0x%x> cmd_error<%d> data_error<%d>",
                        (result == MSDC_SUCCESS && dcrc == 0) ? "PASS" : "FAIL", dcrc,
                        MSDC_READ32(DAT_RD_DLY0(ch0)), MSDC_READ32(DAT_RD_DLY1(ch0)), cur_dsmpl,
                        cmd->error, data->error);
			
            if((result == MSDC_SUCCESS) && dcrc == 0)
            {
                cmd->error = 0;
                data->error = 0;
				if(data->stop)
                    data->stop->error = 0;
				
                goto done;
            }
            else
            {
                // Tuning Data error but Command error happens, directly return;
                if((result != MSDC_SUCCESS) && (result != ERR_DAT_FAILED))
                {
                    MSDC_LOG(MSG_LVL_INFO, "[0]TUNE_READ(1): result<0x%x> cmd_error<%d> data_error<%d>", 
						             result, cmd->error, data->error);	
					
                    goto done;  
                }
                else if((result != MSDC_SUCCESS) && (result == ERR_DAT_FAILED))
                {
                    MSDC_LOG(MSG_LVL_INFO, "[0]TUNE_READ(2): result<0x%x> cmd_error<%d> data_error<%d>", 
						             result, cmd->error, data->error);	
                }
            }
        }

        cur_rxdly0 = MSDC_READ32(DAT_RD_DLY0(ch0));
        cur_rxdly1 = MSDC_READ32(DAT_RD_DLY1(ch0));

        /* E1 ECO. YD: Reverse */
        if (MSDC_READ32(ECO_VER(ch0)) >= 4) 
        {
            orig_dat0 = (cur_rxdly0 >> 24) & 0x1F;
            orig_dat1 = (cur_rxdly0 >> 16) & 0x1F;
            orig_dat2 = (cur_rxdly0 >>	8) & 0x1F;
            orig_dat3 = (cur_rxdly0 >>	0) & 0x1F;
            orig_dat4 = (cur_rxdly1 >> 24) & 0x1F;
            orig_dat5 = (cur_rxdly1 >> 16) & 0x1F;
            orig_dat6 = (cur_rxdly1 >>	8) & 0x1F;
            orig_dat7 = (cur_rxdly1 >>	0) & 0x1F;
        } 
        else 
        {   
            orig_dat0 = (cur_rxdly0 >>	0) & 0x1F;
            orig_dat1 = (cur_rxdly0 >>	8) & 0x1F;
            orig_dat2 = (cur_rxdly0 >> 16) & 0x1F;
            orig_dat3 = (cur_rxdly0 >> 24) & 0x1F;
            orig_dat4 = (cur_rxdly1 >>	0) & 0x1F;
            orig_dat5 = (cur_rxdly1 >>	8) & 0x1F;
            orig_dat6 = (cur_rxdly1 >> 16) & 0x1F;
            orig_dat7 = (cur_rxdly1 >> 24) & 0x1F;
        }

        if(ddr) 
        {
            cur_dat0 = (dcrc & (1 << 0) || dcrc & (1 << 8))  ? ((orig_dat0 + 1) % 32) : orig_dat0;
            cur_dat1 = (dcrc & (1 << 1) || dcrc & (1 << 9))  ? ((orig_dat1 + 1) % 32) : orig_dat1;
            cur_dat2 = (dcrc & (1 << 2) || dcrc & (1 << 10)) ? ((orig_dat2 + 1) % 32) : orig_dat2;
            cur_dat3 = (dcrc & (1 << 3) || dcrc & (1 << 11)) ? ((orig_dat3 + 1) % 32) : orig_dat3;
        } 
        else 
        {
            cur_dat0 = (dcrc & (1 << 0)) ? ((orig_dat0 + 1) % 32) : orig_dat0;
            cur_dat1 = (dcrc & (1 << 1)) ? ((orig_dat1 + 1) % 32) : orig_dat1;
            cur_dat2 = (dcrc & (1 << 2)) ? ((orig_dat2 + 1) % 32) : orig_dat2;
            cur_dat3 = (dcrc & (1 << 3)) ? ((orig_dat3 + 1) % 32) : orig_dat3;
        }
        cur_dat4 = (dcrc & (1 << 4)) ? ((orig_dat4 + 1) % 32) : orig_dat4;
        cur_dat5 = (dcrc & (1 << 5)) ? ((orig_dat5 + 1) % 32) : orig_dat5;
        cur_dat6 = (dcrc & (1 << 6)) ? ((orig_dat6 + 1) % 32) : orig_dat6;
        cur_dat7 = (dcrc & (1 << 7)) ? ((orig_dat7 + 1) % 32) : orig_dat7;

        /* E1 ECO. YD: Reverse */
        if (MSDC_READ32(ECO_VER(ch0)) >= 4) 
        {
            cur_rxdly0 = (cur_dat0 << 24) | (cur_dat1 << 16) | (cur_dat2 << 8) | (cur_dat3 << 0);
            cur_rxdly1 = (cur_dat4 << 24) | (cur_dat5 << 16) | (cur_dat6 << 8) | (cur_dat7 << 0);
        }
        else
        {
            cur_rxdly0 = (cur_dat3 << 24) | (cur_dat2 << 16) | (cur_dat1 << 8) | (cur_dat0 << 0);
            cur_rxdly1 = (cur_dat7 << 24) | (cur_dat6 << 16) | (cur_dat5 << 8) | (cur_dat4 << 0);   
        }
#if 1
        MSDC_WRITE32(DAT_RD_DLY0(ch0), cur_rxdly0);
        MSDC_WRITE32(DAT_RD_DLY1(ch0), cur_rxdly1);   
    }while(++rxdly < 32);
#else
    }while(0);
#endif


done:
		
    return result;
}

int MsdcReqDataWriteTune0(struct msdc_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
    UINT32 wrrdly, cur_wrrdly = 0, orig_wrrdly;
    UINT32 dsmpl,  cur_dsmpl,  orig_dsmpl;
    UINT32 rxdly,  cur_rxdly0;
    UINT32 orig_dat0, orig_dat1, orig_dat2, orig_dat3;
    UINT32 cur_dat0,  cur_dat1,  cur_dat2,  cur_dat3;
    int result = -1;
    UINT32 skip = 1;

    MSDC_LOG(MSG_LVL_ERR, "[0]----->Go into Data Write Tune!\n");

    orig_wrrdly = ((MSDC_READ32(PAD_TUNE(ch0)) & PAD_DAT_WR_RXDLY) >> PAD_DAT_WR_RXDLY_SHIFT);
    orig_dsmpl = ((MSDC_READ32(MSDC_IOCON(ch0)) & MSDC_IOCON_D_SMPL) >> MSDC_IOCON_D_SMPL_SHIFT);

    MSDC_LOG(MSG_LVL_ERR, "[0]CRC(W) Error Register: %08X!\n", MSDC_READ32(SDC_DATCRC_STS(ch0)));

    /* Tune Method 2. just DAT0 */  
    MSDC_CLRBIT(MSDC_IOCON(ch0), MSDC_IOCON_D_DLYLINE_SEL);
    MSDC_SETBIT(MSDC_IOCON(ch0), (1 << MSDC_IOCON_D_DLYLINE_SEL_SHIFT));
    cur_rxdly0 = MSDC_READ32(DAT_RD_DLY0(ch0));

    /* E1 ECO. YD: Reverse */
    if (MSDC_READ32(ECO_VER(ch0)) >= 4) 
    {
        orig_dat0 = (cur_rxdly0 >> 24) & 0x1F;
        orig_dat1 = (cur_rxdly0 >> 16) & 0x1F;
        orig_dat2 = (cur_rxdly0 >>	8) & 0x1F;
        orig_dat3 = (cur_rxdly0 >>	0) & 0x1F;
    } 
    else 
    {
        orig_dat0 = (cur_rxdly0 >>	0) & 0x1F;
        orig_dat1 = (cur_rxdly0 >>	8) & 0x1F;
        orig_dat2 = (cur_rxdly0 >> 16) & 0x1F;
        orig_dat3 = (cur_rxdly0 >> 24) & 0x1F;
    }

    rxdly = 0;
    do 
    {
        wrrdly = 0;
        do 
        {    
            for (dsmpl = 0; dsmpl < 2; dsmpl++) 
            {
                cur_dsmpl = (orig_dsmpl + dsmpl) % 2;
                if (skip == 1) 
                {
                    skip = 0;
                    continue; 	
                }

                MSDC_CLRBIT(MSDC_IOCON(ch0), MSDC_IOCON_D_SMPL);
                MSDC_SETBIT(MSDC_IOCON(ch0), (cur_dsmpl << MSDC_IOCON_D_SMPL_SHIFT));

                result = MsdcSendCmd0(host, cmd, data);

                MSDC_LOG(MSG_LVL_INFO, "[0]TUNE_WRITE<%s> DSPL<%d> DATWRDLY<%d> MSDC_DAT_RDDLY0<0x%x> cmd_error<%d> data_error<%d>", 
                                  result == MSDC_SUCCESS ? "PASS" : "FAIL", cur_dsmpl, cur_wrrdly, cur_rxdly0,
                                  cmd->error, data->error);

                if(result == MSDC_SUCCESS)
                {
                    cmd->error = 0;
                    data->error = 0;
                    if(data->stop)
                        data->stop->error = 0;
					
                    goto done;
                }
                else
                {
                    if((result != MSDC_SUCCESS) && (result != ERR_DAT_FAILED))
                    {
                        MSDC_LOG(MSG_LVL_INFO, "[0]TUNE_WRITE(1): result<0x%x> cmd_error<%d> data_error<%d>", 
                                result, cmd->error, data->error);
						
                        goto done; 
                    }
                    else if((result != MSDC_SUCCESS) && (result == ERR_DAT_FAILED))
                    {
                        MSDC_LOG(MSG_LVL_INFO, "[0]TUNE_WRITE(2): result<0x%x> cmd_error<%d> data_error<%d>", 
                                result, cmd->error, data->error);
                    }
                }				
            }

            cur_wrrdly = (orig_wrrdly + wrrdly + 1) % 32;
            MSDC_CLRBIT(PAD_TUNE(ch0), PAD_DAT_WR_RXDLY);
            MSDC_SETBIT(PAD_TUNE(ch0), (cur_wrrdly << PAD_DAT_WR_RXDLY_SHIFT));
        }while(++wrrdly < 32);
		
        cur_dat0 = (orig_dat0 + rxdly) % 32; /* only adjust bit-1 for crc */
        cur_dat1 = orig_dat1;
        cur_dat2 = orig_dat2;
        cur_dat3 = orig_dat3;                    

        /* E1 ECO. YD: Reverse */
        if (MSDC_READ32(ECO_VER(ch0)) >= 4) 
        {
            cur_rxdly0 = (cur_dat0 << 24) | (cur_dat1 << 16) | (cur_dat2 << 8) | (cur_dat3 << 0);  
        }
        else
        {
            cur_rxdly0 = (cur_dat3 << 24) | (cur_dat2 << 16) | (cur_dat1 << 8) | (cur_dat0 << 0); 
        }
        MSDC_WRITE32(DAT_RD_DLY0(ch0), cur_rxdly0); 
#if 1
    }while(++rxdly < 32);
#else
    }while(0);
#endif

done:
    return result;
}

int MsdcReqDataTune0(struct msdc_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS;
	
    if (data->flags & MMC_DATA_READ)
    {
        i4Ret = MsdcReqDataReadTune0(host, cmd, data);
    }
    else
    {
        i4Ret = MsdcReqDataWriteTune0(host, cmd, data);
    }

    return i4Ret;
}

/*
 * Scatter/gather functions
 */

static inline void msdc0_init_sg(struct msdc_host *host, struct mmc_data *data)
{
	/*
	 * Get info. about SG list from data structure.
	 */
	host->cur_sg = data->sg;
	host->num_sg = data->sg_len;

	host->offset = 0;
	host->remain = host->cur_sg->length;
}

static inline int msdc0_next_sg(struct msdc_host *host)
{
	/*
	 * Skip to next SG entry.
	 */
	host->cur_sg++;
	host->num_sg--;

	/*
	 * Any entries left?
	 */
	if (host->num_sg > 0)
	{
		host->offset = 0;
		host->remain = host->cur_sg->length;
	}

	return host->num_sg;
}

static inline char *msdc0_sg_to_buffer(struct msdc_host *host)
{
    return sg_virt(host->cur_sg);
}

static int msdc0_send_command(struct msdc_host *host, struct mmc_command *cmd)
{
    int i4Ret;
    struct mmc_data *data = cmd->data;

    MSDC_LOG(MSG_LVL_INFO, "[0]Start - Sending cmd (%d), flags = 0x%08x\n", cmd->opcode, cmd->flags);

    // Save current command
    memcpy(&(host->pre_cmd), &(host->cur_cmd), sizeof(struct mmc_command));
    memcpy(&(host->cur_cmd), cmd, sizeof(struct mmc_command));

    /*
      * Send the command
      */

    i4Ret = MsdcSendCmd0(host, cmd, data);
    if((i4Ret != MSDC_SUCCESS) && data && data->error)
    {
        i4Ret = MsdcReqDataTune0(host, cmd, data);   
        if(i4Ret != MSDC_SUCCESS)
        {
            goto end;
        }
    }

end:
	return i4Ret;
}

/*****************************************************************************\
 *                                                                           *
 * MMC callbacks                                                             *
 *                                                                           *
\*****************************************************************************/

static void msdc0_request_end(struct msdc_host *host, struct mmc_request *mrq)
{

    MSDC_LOG(MSG_LVL_INFO, "[0]%s: req done (CMD%u): %d/%d/%d: %08x %08x %08x %08x\n",
    	 mmc_hostname(host->mmc), mrq->cmd->opcode, mrq->cmd->error,
    	 mrq->data ? mrq->data->error : 0,
    	 mrq->stop ? mrq->stop->error : 0,
    	 mrq->cmd->resp[0], mrq->cmd->resp[1], mrq->cmd->resp[2], mrq->cmd->resp[3]);

    // Save current finished command
    memcpy(&(host->pre_cmd), mrq->cmd, sizeof(struct mmc_command));


    host->mrq = NULL;

    /*
     * MMC layer might call back into the driver so first unlock.
     */
    //spin_unlock(&host->lock);
    up(&host->msdc_sema);
    mmc_request_done(host->mmc, mrq);
    //spin_lock(&host->lock);
    //down_interruptible(&host->msdc_sema);
}

#ifdef CC_SDMMC_SUPPORT
int msdc0_card_exist(void);
#endif
static void msdc0_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
    struct msdc_host *host = mmc_priv(mmc);
    struct mmc_command *cmd;
    int i4Ret;

    //spin_lock_bh(&host->lock);
    down(&host->msdc_sema);
//#if	defined(CC_MT5880) || defined(CONFIG_ARCH_MT5880)					
//    if (IS_IC_MT5860_E2IC()) // sd card
//    {
//        MSDC_LOG(MSG_LVL_INFO, "[0]msdc0(sd) pinmux for mt5860\n");			
//        MSDC_CLRBIT(0xF000D620, 0x1<<7);
//        MSDC_SETBIT(0xF000D620, 0x1<<7);
//        MSDC_CLRBIT(0xF000D610, 0x3FFFF<<12);
//        MSDC_SETBIT(0xF000D610, 0x12492<<12);
//    }
//    else // emmc
//    {
//        MSDC_LOG(MSG_LVL_INFO, "[0]msdc0(emmc) pinmux for mt5860\n");			
//        MSDC_CLRBIT(0xF000D604, 0x03<<4);
//        MSDC_SETBIT(0xF000D604, 0x02<<4);
//    }
//#endif

    cmd = mrq->cmd;

    host->mrq = mrq;

    MSDC_LOG(MSG_LVL_INFO, "[0]Request : cmd (%d), arg (0x%x)\n", cmd->opcode, cmd->arg);

#ifdef CC_SDMMC_SUPPORT
    if(!msdc0_card_exist()) 
    {
        MSDC_LOG(MSG_LVL_INFO, "Check if there is acturally a card\n");
        cmd->error = -ENOMEDIUM;
        goto done;
    }
#endif

    i4Ret = msdc0_send_command(host, cmd);

#ifdef CC_SDMMC_SUPPORT
    if((cmd->error == -ENOMEDIUM) || ((cmd->data) && (cmd->data->error == -ENOMEDIUM)))
    {
        goto done;	
    }
#endif

    if (cmd->data && (cmd->error == 0) && (cmd->data->error == 0))
    {
        MSDC_LOG(MSG_LVL_INFO, "[0]Request with Data Success! Ending data transfer (%d bytes)\n", cmd->data->bytes_xfered);
    }
    else if((cmd->data == NULL) && (cmd->error == 0))
    {
        MSDC_LOG(MSG_LVL_INFO, "[0]Request without Data Success!\n");
    }
    else
    {
        MSDC_LOG(MSG_LVL_INFO, "Request Failed: %d!\n", cmd->error);
    }

#ifdef CC_SDMMC_SUPPORT
done:
#endif
    //spin_unlock_bh(&host->lock);
    //up(&host->msdc_sema);
    msdc0_request_end(host, mrq);
}

static void msdc0_reset_variables(struct msdc_host *host)
{
    // Reset variables
    host->hispeedcard = false;
}

/*
 * Initial GPD and BD pointer
 */
static void msdc0_init_gpd_bd(struct msdc_host *host)
{
    msdc_gpd_t *gpd = host->gpd; 
    msdc_bd_t  *bd  = host->bd; 	
    msdc_bd_t  *ptr, *prev;

    /* we just support one gpd */     
    int bdlen = MAX_BD_PER_GPD;   	

    /* init the 2 gpd */
    memset(gpd, 0, sizeof(msdc_gpd_t) * 2);
    //gpd->next = (void *)virt_to_phys(gpd + 1); /* pointer to a null gpd, bug! kmalloc <-> virt_to_phys */  
    //gpd->next = (dma->gpd_addr + 1);    /* bug */
    gpd->pNext = (void *)((u32)host->gpd_addr + sizeof(msdc_gpd_t));    

    //gpd->intr = 0;
    gpd->bdp  = 1;   /* hwo, cs, bd pointer */      
    //gpd->ptr  = (void*)virt_to_phys(bd); 
    gpd->pBuff = (void *)host->bd_addr; /* physical address */

    memset(bd, 0, sizeof(msdc_bd_t) * bdlen);
    ptr = bd + bdlen - 1;
    //ptr->eol  = 1;  /* 0 or 1 [Fix me]*/
    //ptr->next = 0;    

    while (ptr != bd) 
    {
        prev = ptr - 1;
        prev->pNext = (void *)(host->bd_addr + sizeof(msdc_bd_t) *(ptr - bd));
        ptr = prev;
    }
}

/*
 * Allocate/free DMA port and buffer
 */
static void __devinit msdc0_request_dma(struct msdc_host *host)
{
    /* using dma_alloc_coherent*/  
    host->gpd = (msdc_gpd_t *)dma_alloc_coherent(NULL, MAX_GPD_NUM * sizeof(msdc_gpd_t), 
                                                       &host->gpd_addr, GFP_KERNEL); 
    host->bd =  (msdc_bd_t *)dma_alloc_coherent(NULL, MAX_BD_NUM  * sizeof(msdc_bd_t),  
                                                       &host->bd_addr,  GFP_KERNEL); 
    BUG_ON((!host->gpd) || (!host->bd));    
    msdc0_init_gpd_bd(host);
}

static void __devinit msdc0_release_dma(struct msdc_host *host)
{
    /* using dma_alloc_coherent*/  
    dma_free_coherent(NULL, MAX_GPD_NUM * sizeof(msdc_gpd_t), 
                      host->gpd, host->gpd_addr); 
    dma_free_coherent(NULL, MAX_BD_NUM  * sizeof(msdc_bd_t),  
                      host->gpd, host->bd_addr);
}

#ifdef CC_SDMMC_SUPPORT
static void msdc0_tasklet_finish(unsigned long param)
{
    struct msdc_host *host = (struct msdc_host *)param;
    
    WARN_ON(host->mrq == NULL);
    
    if ((host->mrq->data) && (host->mrq->data->error))
    {
        host->mrq->data->bytes_xfered = 0;
    }
    else if(host->mrq->data && (host->mrq->data->error == 0))
    {
        host->mrq->data->bytes_xfered = host->mrq->data->blocks * host->mrq->data->blksz;
    }
    else
    {
    	
    }

    up(&host->msdc_sema);
    msdc0_request_end(host, host->mrq);
}

static void msdc0_init_tasklet(struct msdc_host *host)
{
    tasklet_init(&finish_tasklet, msdc0_tasklet_finish, 
		        (unsigned long)host);
}
#endif

/*
 * Allocate/free IRQ.
 */
static int __devinit msdc0_request_irq(struct msdc_host *host)
{
    if(host->msdc_isr_en == 1)
    {
        MsdcRegIsr0(host);
        MsdcSetIsr0(INT_SD_ALL, host);
    }
    
#ifdef CC_SDMMC_SUPPORT
    if(_sd_gpio_isr == 1)
    {
        MsdcRegGPIOIsr0(host);
        MsdcSetGPIOIsr0(0x0, host);    
    }
#endif

    return 0;
}

static void msdc0_release_irq(struct msdc_host *host)
{
    if (!host->irq)
        return;

    free_irq(host->irq, host);

    host->irq = 0;
}

#ifdef CC_SDMMC_SUPPORT
// wait for extend if power pin is controlled by GPIO
void msdc0_card_power_up(void)
{	
}
// wait for extend if power pin is controlled by GPIO
void msdc0_card_power_down(void)
{	
}
#ifdef CONFIG_MT53XX_NATIVE_GPIO
int msdc0_card_exist(void)
{
    unsigned int val;	
    //printk(KERN_ERR " tyler.zhang MSDC_Gpio[MSDC0DetectGpio] :%d \n",MSDC_Gpio[MSDC0DetectGpio]);
if (IS_IC_MT5860_E2IC()) // sd card
{  
    //SD CARD DETECT GPIO VALUE
    if(MSDC_Gpio[MSDC0DetectGpio] == -1)
    {
  
        //printk(KERN_ERR " tyler.zhang return 0 \n");
        return 0;	
    }
    else
    {
        mtk_gpio_direction_input(MSDC_Gpio[MSDC0DetectGpio]);
        val = mtk_gpio_get_value(MSDC_Gpio[MSDC0DetectGpio]);
        //printk(KERN_ERR " tyler.zhang return val: %d \n",val);
    }
    
    if(val)	
    {
        msdc0_card_power_down();   
        MSDC_LOG(MSG_LVL_ERR, "[0]Card not Insert(%d)!\n", val);	
        return 0;	
    }	

    msdc0_card_power_up();

    MSDC_LOG(MSG_LVL_ERR, "[0]Card Insert(%d)!\n", val);   
    return 1;
    
}
else//emmc
{
	 return 1;
}

}

#else
int msdc0_card_exist(void)
{
    unsigned int val;	

    if (IS_IC_MT5860_E2IC()) // sd card
    {
        //SD CARD DETECT GPIO VALUE
        val = SD_CARD_DETECT_GPIO;
	
        if(val)	
        {
            msdc0_card_power_down();   
            MSDC_LOG(MSG_LVL_INFO, "[0]Card not Insert(%d)!\n", val);	
            return 0;	
        }	

        msdc0_card_power_up();

        MSDC_LOG(MSG_LVL_INFO, "[0]Card Insert(%d)!\n", val);   
        return 1;
    }
    else // emmc
    {
        return 1;
    }
}
#endif

static void msdc0_tasklet_card(unsigned long param)
{
    struct msdc_host *host = (struct msdc_host *)param;
    int delay = -1;

    _bMSDCCardExist = msdc0_card_exist();
    MSDC_LOG(MSG_LVL_INFO, "[0]Card Detect %d(%d)\n", _bMSDCCardExist, plugCnt);

    if(plugCnt >= 2)
    {
        detectFlag = 1;
    }

	if (_bMSDCCardExist)    
	{        	            
	    if (!(host->flags & SDHCI_FCARD_PRESENT))        
		{            		                

		    // Prevent glitch            		                
		    mdelay(10);              
			if(msdc0_card_exist() && (plugCnt < 2))            
			{			                

			    MSDC_LOG(MSG_LVL_INFO, "[0]Card inserted\n");
				host->flags |= SDHCI_FCARD_PRESENT;
				msdc0_card_power_up();

				// Reset variables            			                
                msdc0_reset_variables(host);
				MsdcInit0();
				delay = 0;  
				
                goto ErrEnd;            
            }
            else
            {                
                // do nothing	            	            
            }
		}
		else
        {
        }
    }
    else
    {
        if (host->flags & SDHCI_FCARD_PRESENT)
        {            
            // Prevent glitch
            mdelay(10);
			if((!msdc0_card_exist()) && (plugCnt < 2))
            {
                MSDC_LOG(MSG_LVL_INFO, "[0]Card removed\n");
				host->flags &= ~SDHCI_FCARD_PRESENT;
				if (host->mrq)
                {
                    MSDC_LOG(MSG_LVL_INFO, "[0]%s: Card removed during transfer!\n", mmc_hostname(host->mmc));
					msdc0_card_power_down();

					// Reset variables
					msdc0_reset_variables(host);
					MsdcInit0();
					host->mrq->cmd->error = -ENOMEDIUM;
					tasklet_schedule(&finish_tasklet);
				}

				delay = 0;

				goto ErrEnd;
            }
            else
            {
                // do nothing
            }
        }
        else
        {
        }
    }
	
	if((detectFlag == 1) && (plugCnt < 2))
    {
        // Prevent glitch
        mdelay(10);
        if(msdc0_card_exist())
        {
            MSDC_LOG(MSG_LVL_INFO, "[0]Card inserted(*)\n");
			host->flags |= SDHCI_FCARD_PRESENT;
			msdc0_card_power_up();
			// Reset variables
			msdc0_reset_variables(host);
			MsdcInit0();
			delay = 0;
		}
		else
		{
		    MSDC_LOG(MSG_LVL_INFO, "[0]Card removed(*)\n");
			host->flags &= ~SDHCI_FCARD_PRESENT;
			if (host->mrq)
            {
                MSDC_LOG(MSG_LVL_INFO, "[0]%s: Card removed during transfer!\n", mmc_hostname(host->mmc));
				msdc0_card_power_down();
				// Reset variables
				msdc0_reset_variables(host);
				MsdcInit0();
				host->mrq->cmd->error = -ENOMEDIUM;
				tasklet_schedule(&finish_tasklet);
			}
			
			delay = 0;
		}

		goto ErrEnd;
	}

ErrEnd:
	
    plugCnt = 0;  		      	    
	if (delay != -1)
	{    	  
        detectFlag = 0;  
        mmc_detect_change(host->mmc, msecs_to_jiffies(delay));     
    }   	    

    mod_timer(&card_detect_timer, jiffies + HZ/3);
}
#endif

static void msdc0_free_mmc(struct device *dev)
{
    struct mmc_host *mmc;
    struct msdc_host *host;

    mmc = dev_get_drvdata(dev);
    if (!mmc)
        return;

    host = mmc_priv(mmc);
    BUG_ON(host == NULL);

    mmc_free_host(mmc);

    dev_set_drvdata(dev, NULL);
}

/*
 * Add a set_ios hook which is called when the SDHCI driver
 * is called to change parameters such as clock or card width.
 */
static void msdc0_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
    struct msdc_host *host = mmc_priv(mmc);

    MSDC_LOG(MSG_LVL_INFO, "[0]ios->power_mode = 0x%x, ios->clock =%d, ios->bus_width = 0x%x\n", ios->power_mode, ios->clock, ios->bus_width);

    //spin_lock_bh(&host->lock);
    down(&host->msdc_sema);

    host->clk = ios->clock;
    host->bus_width = ios->bus_width;

    MsdcSetIos0(mmc, ios);

    if (IS_IC_MT5860_E2IC()) // sd card
    {
    }
    else // emmc
    {
        if(host->clk >= 26000000)
        {
            if(mmc->caps & MMC_CAP_MMC_HIGHSPEED)
            {
                MsdcSetSampleEdge0(2);
            }
         }
    }

    //spin_unlock_bh(&host->lock);
    up(&host->msdc_sema);
}

static int msdc0_get_ro(struct mmc_host *mmc)
{
    return 0;
}

static int msdc0_get_cd(struct mmc_host *mmc)
{
#ifdef CC_SDMMC_SUPPORT 
if (IS_IC_MT5860_E2IC()) // sd card
{ 
	return msdc0_card_exist();
}
else//emmc
{
    return 1;	
}	
#else
    return 1;
#endif
}

static const struct mmc_host_ops msdc0_ops = {
   .request  = msdc0_request,
   .set_ios  = msdc0_set_ios,
   .get_ro   = msdc0_get_ro,
   .get_cd   = msdc0_get_cd,
};

/*****************************************************************************\
 *                                                                           *
 * Suspend/resume                                                            *
 *                                                                           *
\*****************************************************************************/
#ifdef CONFIG_PM
static int msdc0_platform_suspend(struct platform_device *dev, pm_message_t state)
{
    int ret = 0;
    struct mmc_host *mmc = msdc_host0->mmc;

    // mmc suspend call
    if (mmc)
        ret = mmc_suspend_host(mmc);

    down(&msdc_host0->msdc_sema);

    MsdcSetClkfreq0(msdc_host0->clk);
    MsdcSetBusWidth0(msdc_host0->bus_width);
    
    disable_irq(msdc_host0->irq);

    //gate hclk & sclk
    MSDC_SETBIT(MSDC_HCLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_GATE_BIT);	
    MSDC_SETBIT(MSDC_SCLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_GATE_BIT);	

    up(&msdc_host0->msdc_sema);

    return ret;
}

static int msdc0_platform_resume(struct platform_device *dev)
{
    int ret = 0;
    struct mmc_host *mmc = msdc_host0->mmc;

    down(&msdc_host0->msdc_sema);
    
    //gate hclk & sclk
    MSDC_CLRBIT(MSDC_HCLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_GATE_BIT);	
    MSDC_CLRBIT(MSDC_SCLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_GATE_BIT);	

    // Init Host Controller
    MsdcInit0();
    
    // Enable IRQ
    MSDC_SETBIT(MSDC0_EN_REG, (0x1<<MSDC0_EN_SHIFT));
    MSDC_WRITE32(MSDC_INTEN(ch0), INT_SD_ALL);
    enable_irq(msdc_host0->irq);

    init_completion(&comp0);
    
    // mmc resume call
    if (mmc)
        ret = mmc_resume_host(mmc);
	
    if (IS_IC_MT5860_E2IC()) // sd card
    {
        // don't need set sd card clock now
    }
    else // emmc
    {
        MsdcSetClkfreq0(mmc->f_max);
        MsdcSetBusWidth0(MMC_BUS_WIDTH_4);
    }

    up(&msdc_host0->msdc_sema);

    return ret;
}
#else
#define msdc0_platform_suspend NULL
#define msdc0_platform_resume NULL
#endif


/*****************************************************************************\
 *                                                                           *
 * Device probing/removal                                                    *
 *                                                                           *
\*****************************************************************************/

/*
 * Allocate/free MMC structure.
 */

static struct sdhci_msdc_pdata sdhci_msdc0_pdata = {
  /* This is currently here to avoid a number of if (host->pdata)
   * checks. Any zero fields to ensure reaonable defaults are picked. */
};

static int __devinit msdc0_probe(struct platform_device *pdev)
{
    struct msdc_host *host = NULL;
    struct mmc_host *mmc = NULL;
    int ret = 0;
   
    if(IS_IC_MT5860_E2IC())
    {
#ifdef CONFIG_MTKEMMC_BOOT
        down(&msdc_host_sema);
#endif
    }
    MSDC_LOG(MSG_LVL_INFO, "\n");

    /*
      * Allocate MMC structure.
      */
    mmc = mmc_alloc_host(sizeof(struct msdc_host), &pdev->dev);

    if (!mmc)
    {
      ret = -ENOMEM;
      goto probe_out;
    }

    host = mmc_priv(mmc);
    host->mmc = mmc;
    host->pdev  = pdev;
    host->pdata = pdev->dev.platform_data;
    host->chip_id = CONFIG_CHIP_VERSION;
    host->base = (MSDC_BASE_ADDR + ch0*MSDC_CH_OFFSET);
#ifdef CC_MTD_ENCRYPT_SUPPORT
    host->data_mode = BASIC_DMA_DATA_MODE;
#else    
    host->data_mode = DESC_DMA_DATA_MODE;
#endif
    host->irq = ch0?(VECTOR_MSDC2):(VECTOR_MSDC);
    host->devIndex = 0;
    host->MsdcAccuVect = 0x00;
    host->msdc_isr_en = 1;
    host->desVect = 0;
    host->waitIntMode = 0;
    host->timeout_ns = 0;    
    host->timeout_clks = DEFAULT_DTOC * 65536;
#ifdef CC_MTD_ENCRYPT_SUPPORT 
    // aes init
    msdc1_aes_init();
#endif

    // Reset variables
    msdc0_reset_variables(host);

#ifdef CC_SDMMC_SUPPORT 
    // Default card exists
    _bMSDCCardExist = msdc0_card_exist();
    if(_bMSDCCardExist)
    {	
        host->flags |= SDHCI_FCARD_PRESENT;
		msdc0_card_power_up();
    }
	else
    {
        host->flags &= (~SDHCI_FCARD_PRESENT);
		msdc0_card_power_down();
    }
#else
    // Default card exists
    host->flags |= SDHCI_FCARD_PRESENT;
#endif

    if (!host->pdata)
    {
      pdev->dev.platform_data = &sdhci_msdc0_pdata;
      host->pdata = &sdhci_msdc0_pdata;
    }

    // Save host pointer for futher useage
    msdc_host0 = host;
    if (IS_IC_MT5860_E2IC()) // sd card
    {
    }
    else // emmc
    {
	    msdc_host_boot = msdc_host0;
        msdc_send_command = msdc0_send_command;
    }

    /*
     * Set host parameters.
     */
    mmc->ops = &msdc0_ops;
    mmc->f_min = 397000;
    mmc->f_max = 48000000;

    mmc->ocr_avail = MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_34_35 | MMC_VDD_35_36;
    mmc->caps = MMC_CAP_4_BIT_DATA | MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED;
    if (IS_IC_MT5860_E2IC())  // sd card
    {
        mmc->special_arg = MSDC_TRY_SD;   // special for SD host
    }
    else // emmc
    {
        mmc->special_arg = MSDC_TRY_MMC;   // special for eMMC host
    }

    spin_lock_init(&host->lock);
    sema_init(&host->msdc_sema, 1);
    init_completion(&comp0);

    /*
     * Maximum number of segments. Worst case is one sector per segment
     * so this will be 64kB/512.
     */
    mmc->max_segs = 128;

    /*	 * Maximum request size. Also limited by 64KiB buffer.	 */
    mmc->max_req_size = 64 * SDHC_BLK_SIZE;

    /*
     * Maximum segment size. Could be one segment with the maximum number
     * of bytes.
     */

    mmc->max_seg_size = mmc->max_req_size;

    /*
     * Maximum block size. We have 12 bits (= 4095) but have to subtract
     * space for CRC. So the maximum is 4095 - 4*2 = 4087.
     */

    mmc->max_blk_size = 512;

    /*
     * Maximum block count. There is no real limit so the maximum
     * request size will be the only restriction.
     */

    mmc->max_blk_count = mmc->max_req_size;

    dev_set_drvdata(&(pdev->dev), mmc);

#ifdef CC_SDMMC_SUPPORT  
    // GPIO Setting
    //SD_CARD_DETECT_GPIO_SETTING;
#endif

    // Initialize MSDC hardware
    MsdcInit0();

    /*
     * Allocate DMA.
     */
    msdc0_request_dma(host);

    /* Setup irq function */
    if (msdc0_request_irq(host))
    {
        dev_err(&pdev->dev, "[0]failed to request sdhci interrupt.\n");
        ret = -ENOENT;
        goto probe_out;
    }

#ifdef CC_SDMMC_SUPPORT 
    /* Initial tasklet */
    msdc0_init_tasklet(host);
#endif

    /* We get spurious interrupts even when we have set the IMSK
     * register to ignore everything, so use disable_irq() to make
     * ensure we don't lock the system with un-serviceable requests. */
    // disable_irq(host->irq);

    mmc_add_host(mmc);

    mmc = dev_get_drvdata(&(pdev->dev));

    printk(KERN_ERR "%s:", mmc_hostname(mmc));
    if (host->chip_id != 0)
      printk(" %d", (int)host->chip_id);
    printk(KERN_ERR " at 0x%x irq %d data mode %d\n", (int)host->base, (int)host->irq, (int)(host->data_mode));

    /* 
     * Set up timers     
     */
#ifdef CC_SDMMC_SUPPORT  
    init_timer(&card_detect_timer);    
    card_detect_timer.data = (unsigned long)host;    
    card_detect_timer.function = msdc0_tasklet_card;    
    card_detect_timer.expires = jiffies + 2 * HZ;    
    add_timer(&card_detect_timer);
#endif

probe_out:
    if(IS_IC_MT5860_E2IC())
    {
#ifdef CONFIG_MTKEMMC_BOOT
        up(&msdc_host_sema);
#endif
    }
    return ret;
    
}


/*****************************************************************************\
 *                                                                           *
 * Driver init/exit                                                          *
 *                                                                           *
\*****************************************************************************/

/*
 * Release all resources for the host.
 */
static void __devexit msdc0_shutdown(struct device *dev)
{
    struct mmc_host *mmc = dev_get_drvdata(dev);
    struct msdc_host *host;

    if (!mmc)
        return;

    host = mmc_priv(mmc);

    mmc_remove_host(mmc);

    msdc0_release_dma(host);

    msdc0_release_irq(host);

    msdc0_free_mmc(dev);
}

static int __devexit msdc0_remove(struct platform_device *dev)
{
    MSDC_LOG(MSG_LVL_INFO, "\n");

    msdc0_shutdown(&dev->dev);

    return 0;
}

static struct platform_device *msdc0_device;
static struct platform_driver msdc0_driver = {
    .probe    = msdc0_probe,
    .remove   = __devexit_p(msdc0_remove),

    .suspend  = msdc0_platform_suspend,
    .resume   = msdc0_platform_resume,
    .driver   = {
        .name = DRIVER_NAME0,
        .owner= THIS_MODULE,
    },
};

static int __init msdc0_drv_init(void)
{
    int result;

    MSDC_LOG(MSG_LVL_INFO, "[0]initialize..##.\n");

    result = platform_driver_register(&msdc0_driver);

    if (result < 0)
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]platform_driver_register failed !!\n");
        return result;
    }

    msdc0_device = platform_device_alloc(DRIVER_NAME0, -1);

    if (!msdc0_device)
    {
        MSDC_LOG(MSG_LVL_ERR, "[0]platform_device_alloc failed !!\n");
        platform_driver_unregister(&msdc0_driver);
        return -ENOMEM;
    }

    msdc0_device->dev.coherent_dma_mask = MTK_MSDC_DMA_MASK;
    msdc0_device->dev.dma_mask = &msdc0_device->dev.coherent_dma_mask;

    result = platform_device_add(msdc0_device);

    if (result)
    {
         MSDC_LOG(MSG_LVL_ERR, "[0]platform_device_add failed !!\n");
         platform_device_put(msdc0_device);
         platform_driver_unregister(&msdc0_driver);
         return result;
    }

    MSDC_LOG(MSG_LVL_INFO, "[0]initialize finish..\n");

    return 0;
}

static void __exit msdc0_drv_exit(void)
{
    MSDC_LOG(MSG_LVL_INFO, "\n");

    platform_device_unregister(msdc0_device);

    platform_driver_unregister(&msdc0_driver);
}


module_init(msdc0_drv_init);
module_exit(msdc0_drv_exit);

MODULE_AUTHOR("Shunli Wang");
MODULE_DESCRIPTION("MTK Secure Digital Host Controller Interface driver");
MODULE_VERSION(DRIVER_VERSION0);
MODULE_LICENSE("GPL");

