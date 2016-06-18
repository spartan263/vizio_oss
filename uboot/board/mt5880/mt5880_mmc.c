/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/

#if defined(CC_EMMC_BOOT)

#include <common.h>
#include "x_bim.h"
#include "x_timer.h"
#include <config.h>
#include <malloc.h>
#include <asm/errno.h>
#include <linux/mtd/mtd.h>
#include "mmc.h"
#include "msdc_host_hw.h"
#include "msdc_slave_hw.h"
#include "msdc_drv.h"
#include "x_ldr_env.h"
//#include "x_hal_5381.h"
#include "x_hal_arm.h"
#include "drvcust_if.h"
#include "dmx_drm_if.h"

#define MSDC_EMMC_INIT_UNIFY_EN             (1)

typedef struct
{
    char acName[32];
    uint u4ID1;
    uint u4ID2;
    uint  u4Sample;
} EMMC_FLASH_DEV_T;

int ch = 0;
int devIndex = 0;
/* specify default data transfer mode */
int dataMode = BASIC_DMA_DATA_MODE;

/* the global variable related to descriptor dma */
volatile static uint _u4MsdcAccuVect = 0x00;
msdc_gpd_t DMA_MSDC_Header, DMA_MSDC_End;
msdc_bd_t DMA_BD[MAX_BD_PER_GPD];
uint BDNum = 0;

/* the global variable related to clcok */
uint _u4MsdcFreq[] = {MSDC_CLOCK_SELECTION_STRING};
uint _u4MsdcFreqSelVal[] = {MSDC_CLOCK_SELECTION_VALUE};
uint _u4MsdcFreqIdx = SD_DEFAULT_NORM_CLOCK_INDEX;
uint _u4MsdcCurFreq;

struct mmc *emmc_dev = NULL;

/* the global variable related to sample edge */
const EMMC_FLASH_DEV_T _arEMMC_DevInfo[] =
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
    //{ "KE44A-26BN/4GB",     0x004D4D43, 0x30344744, 0x00000100},
};
#ifdef CC_MTD_ENCRYPT_SUPPORT
#define AES_ADDR_ALIGN   0x20
#define AES_LEN_ALIGN    0x10 
#define AES_BUFF_LEN     0x10000
uchar _u1MsdcBuf_AES[AES_BUFF_LEN + AES_ADDR_ALIGN], *_pu1MsdcBuf_Aes_Align;
#endif
//-----------------------------------------------------------------------------
// Partition encypt information.
//-----------------------------------------------------------------------------
extern mtk_part_info_t rMTKPartInfo[];

void MsdcDumpRegister(void)
{
    uint i = 0;

    for(; i< 0x104; i+=4)
    {
        if(i%16 == 0)
        {
            MSDC_LOG(MSG_LVL_TITLE, "\r\n%08X |", i);
        }
        MSDC_LOG(MSG_LVL_TITLE, " %08X", MSDC_READ32(MSDC_BASE_ADDR + (ch * MSDC_CH_OFFSET) + i));
    }
}

uchar MsdcCheckSumCal(uchar *buf, uint len)
{
    uint i = 0, sum = 0;

    for(; i<len;i++)
    {
        sum += *(buf + i);
    }

    return (0xFF-(uchar)sum);
}
#ifdef CC_MTD_ENCRYPT_SUPPORT
void MsdcAesInit(u16 keylen)
{  
	  uint tmp = 0;
	  uchar key[16] = {0};
    if(DMX_NAND_AES_INIT(key, keylen))
    {  
        MSDC_LOG(MSG_LVL_ERR, " aes init success!\n");
    }
    else
    {
        MSDC_LOG(MSG_LVL_ERR, " aes init failed!\n");	
    }
    
    memset(_u1MsdcBuf_AES, 0x00, AES_BUFF_LEN + AES_ADDR_ALIGN);
    
    tmp = (uint)_u1MsdcBuf_AES & (AES_ADDR_ALIGN - 1);
    if(tmp != 0x0)
        _pu1MsdcBuf_Aes_Align = (uchar *)(((uint)_u1MsdcBuf_AES & (~(AES_ADDR_ALIGN - 1))) + AES_ADDR_ALIGN);
    else
    	  _pu1MsdcBuf_Aes_Align = (uchar *)_u1MsdcBuf_AES;
         
    return;  	
}

uint MsdcPartitionEncrypted(uint blkindx)
{
    uint partId;
    uint size = 0, offset = 0, fg_en = 0;

    for (partId = 0; partId < MAX_MTD_DEVICES; partId++)
    {
        offset = DRVCUST_InitGet((QUERY_TYPE_T)(eNANDFlashPartOffset0 + partId));
        size = DRVCUST_InitGet((QUERY_TYPE_T)(eNANDFlashPartSize0 + partId));
        fg_en = DRVCUST_InitGet((QUERY_TYPE_T)(eNANDFlashPartEncypt0 + partId));
        
        if((fg_en != 0) &&
        	 ((uint)(offset>>9) <= blkindx) &&
        	 ((uint)((offset+size)>>9) > blkindx))
        {
        	  MSDC_LOG(MSG_LVL_INFO, " This partition is encrypted!\n");	
            return 1;	
        }
    }    
    
    MSDC_LOG(MSG_LVL_INFO, " This partition is not encrypted!\n");	
    return 0;	
}

int MsdcAesEncryption(uint buff, uint len)
{
    uint len_used = 0, len_total = 0;
    
    if(len & (AES_LEN_ALIGN - 1) != 0x0)
	  {
	      MSDC_LOG(MSG_LVL_ERR, " the buffer to be encrypted is not align to %d bytes!\n", AES_LEN_ALIGN);
	      return MSDC_FAILED;		
	  }	
	  
	  if( buff & (AES_ADDR_ALIGN - 1) != 0x0)
	  {
			  len_total = (int)len;
			  do
			  {
			  	  len_used = (len_total > AES_BUFF_LEN)?AES_BUFF_LEN:len_total;
			      memcpy((void *)_pu1MsdcBuf_Aes_Align, (void *)buff, len_used);	
			      if(DMX_NAND_AES_Encryption((uint)_pu1MsdcBuf_Aes_Align, (uint)_pu1MsdcBuf_Aes_Align, len_used))
		        {
		            MSDC_LOG(MSG_LVL_INFO, "Encryption to buffer(addr:0x%08X size:0x%08X) success!\n", (uint)_pu1MsdcBuf_Aes_Align, len_used);	
		        }
		        else
		        {
		            MSDC_LOG(MSG_LVL_ERR, "Encryption to buffer(addr:0x%08X size:0x%08X) failed!\n", (uint)_pu1MsdcBuf_Aes_Align, len_used);	
		            return MSDC_FAILED;    	
		        }	
		        memcpy((void *)buff, (void *)_pu1MsdcBuf_Aes_Align, len_used);
		        
		        len_total -= len_used;
		        buff += len_used;
		        
			  }while(len_total > 0);
	  }
	  else
	  {
	      if(DMX_NAND_AES_Encryption(buff, buff, len))
		    {
		        MSDC_LOG(MSG_LVL_INFO, "Encryption to buffer(addr:0x%08X size:0x%08X) success!\n", buff, len);	
		    }
		    else
		    {
		        MSDC_LOG(MSG_LVL_ERR, "Encryption to buffer(addr:0x%08X size:0x%08X) failed!\n", buff, len);	
		        return MSDC_FAILED;    	
		    }		
	  }
	  
	  return MSDC_SUCCESS;  
}

int MsdcAesDecryption(uint buff, uint len)
{
    uint len_used = 0, len_total = 0;
    
    if(len & (AES_LEN_ALIGN - 1) != 0x0)
	  {
	      MSDC_LOG(MSG_LVL_ERR, " the buffer to be encrypted is not align to %d bytes!\n", AES_LEN_ALIGN);
	      return MSDC_FAILED;		
	  }	
	  
	  if( buff & (AES_ADDR_ALIGN - 1) != 0x0)
	  {
			  len_total = len;
			  do
			  {
			  	  len_used = (len_total > AES_BUFF_LEN)?AES_BUFF_LEN:len_total;
			      memcpy((void *)_pu1MsdcBuf_Aes_Align, (void *)buff, len_used);	
			      if(DMX_NAND_AES_Decryption((uint)_pu1MsdcBuf_Aes_Align, (uint)_pu1MsdcBuf_Aes_Align, len_used))
		        {
		            MSDC_LOG(MSG_LVL_INFO, "Decryption to buffer(addr:0x%08X size:0x%08X) success!\n", (uint)_pu1MsdcBuf_Aes_Align, len_used);	
		            //return MSDC_SUCCESS;	
		        }
		        else
		        {
		            MSDC_LOG(MSG_LVL_ERR, "Decryption to buffer(addr:0x%08X size:0x%08X) failed!\n", (uint)_pu1MsdcBuf_Aes_Align, len_used);	
		            return MSDC_FAILED;    	
		        }	
		        memcpy((void *)buff, (void *)_pu1MsdcBuf_Aes_Align, len_used);
		        
		        len_total -= len_used;
		        buff += len_used;
		        
			  }while(len_total > 0);
    }
    else
	  {
	      if(DMX_NAND_AES_Decryption(buff, buff, len))
		    {
		        MSDC_LOG(MSG_LVL_INFO, "Decryption to buffer(addr:0x%08X size:0x%08X) success!\n", buff, len);	
		    }
		    else
		    {
		        MSDC_LOG(MSG_LVL_ERR, "Decryption to buffer(addr:0x%08X size:0x%08X) failed!\n", buff, len);	
		        return MSDC_FAILED;    	
		    }		
	  }		
	  
	  return MSDC_SUCCESS;
}

#endif

void MsdcDescriptorConfig(void *pBuff, uint u4BufLen)
{
    uint i, tmpBDNum, tmpBuflen/*, fgEOL*/;
    uchar *tmppBuff = (uchar *)pBuff;

    if(u4BufLen == 0)
        return;

    BDNum = u4BufLen / BD_MAX_LEN + ((u4BufLen % BD_MAX_LEN == 0)?0:1);
    tmpBDNum = BDNum;
    tmpBuflen = u4BufLen;

    MSDC_LOG(MSG_LVL_INFO, "\n----BD NUM: %d----\n", BDNum);

    if(BDNum > MAX_BD_NUM)
    {
        MSDC_LOG(MSG_LVL_WARN, "BD Number exceeds MAX value(%d)!\n", MAX_BD_NUM);
        return;
    }

    // Initial the structures
    memset(&DMA_MSDC_Header, 0x0, sizeof(msdc_gpd_t));
    memset(&DMA_MSDC_End, 0x0, sizeof(msdc_gpd_t));
    memset(DMA_BD, 0x0, sizeof(msdc_bd_t)*MAX_BD_NUM);

    // Config the BD structure array
    for(i = 0;i<BDNum;i++)
    {
        if(i != BDNum - 1)
        {
            DMA_BD[i].pNext = (void *)(&DMA_BD[i+1]);
        }
        else
            DMA_BD[i].eol = 1;
	
        DMA_BD[i].pBuff = (void *)(tmppBuff);
        DMA_BD[i].buffLen = (tmpBuflen>BD_MAX_LEN)?(BD_MAX_LEN):(tmpBuflen);
        tmpBuflen -= DMA_BD[i].buffLen;
        tmppBuff += DMA_BD[i].buffLen;

        DMA_BD[i].chksum = 0;
        DMA_BD[i].chksum = MsdcCheckSumCal((uchar *)(DMA_BD+i), 16);
    }

    // Config the GPD structure
    DMA_MSDC_Header.hwo = 1;  /* hw will clear it */
    DMA_MSDC_Header.bdp = 1;	 
    DMA_MSDC_Header.pNext = (void *)(&DMA_MSDC_End);
    DMA_MSDC_Header.pBuff = (void *)(DMA_BD);
    DMA_MSDC_Header.chksum = 0;  /* need to clear first. */  
    DMA_MSDC_Header.chksum = MsdcCheckSumCal((uchar *)(&DMA_MSDC_Header), 16);

    HalFlushInvalidateDCacheMultipleLine((uint)(&DMA_MSDC_Header), sizeof(msdc_gpd_t));
    HalFlushInvalidateDCacheMultipleLine((UINT32)(&DMA_MSDC_End), sizeof(msdc_gpd_t));
    HalFlushInvalidateDCacheMultipleLine((UINT32)(DMA_BD), sizeof(msdc_bd_t)*BDNum);

    // Config the DMA HW registers
    MSDC_SETBIT(DMA_CFG, DMA_CFG_CHKSUM);
    MSDC_WRITE32(DMA_SA, 0x0);
    MSDC_SETBIT(DMA_CTRL, DMA_CTRL_BST_64);
    MSDC_SETBIT(DMA_CTRL, DMA_CTRL_DESC_MODE);

    MSDC_WRITE32(DMA_SA, (uint)(&DMA_MSDC_Header));

}

void MsdcFindDev(uint *pCID)
{
    uint i, devNum;
    
    /*
      * why we need to define the id mask of emmc
      * Some vendors' emmc has the same brand & type but different product revision.
      * That means the firmware in eMMC has different revision
      * We should treat these emmcs as same type and timing
      * So id mask can ignore this case
    */
    uint idMask = 0xFFFFFF00;

    devNum = sizeof(_arEMMC_DevInfo)/sizeof(EMMC_FLASH_DEV_T);
    MSDC_LOG(MSG_LVL_ERR, "%08X:%08X:%08X:%08X\n", pCID[0], pCID[1], pCID[2], pCID[3]);
    MSDC_LOG(MSG_LVL_ERR, "id1:%08X id2:%08X\n", ID1(pCID), ID2(pCID));

    for(i = 0; i<devNum; i++)
    {
        if((_arEMMC_DevInfo[i].u4ID1 == ID1(pCID)) &&
           ((_arEMMC_DevInfo[i].u4ID2 & idMask) == (ID2(pCID) & idMask)))
        {
            break;
        }
    }

    devIndex = (i == devNum)?0:i;

    MSDC_LOG(MSG_LVL_TITLE, "eMMC Name: %s\n", _arEMMC_DevInfo[devIndex].acName);	
}

void MsdcSetSample(uchar fgFalling)
{
    // Set command response sample selection
    if(fgFalling & 0x0F)
    {
        MSDC_SETBIT(MSDC_IOCON, ((0x1) << 1)); 
    } 
    else
    {
        MSDC_CLRBIT(MSDC_IOCON, ((0x1) << 1));
    }
	    
    // Set read data sample selection
    if((fgFalling>>4) & 0x0F)
    {
        MSDC_SETBIT(MSDC_IOCON, ((0x1) << 2));  
    }
    else
    {
        MSDC_CLRBIT(MSDC_IOCON, ((0x1) << 2));
    }		
}

void MsdcClrTiming(void)
{
    // Clear Sample Edge
    MSDC_CLRBIT(MSDC_IOCON, (((UINT32)0x3) << 1));

    // Clear Pad Tune
    MSDC_WRITE32(PAD_TUNE, 0x00000000);
    
    // Clear 
    MSDC_WRITE32(DAT_RD_DLY0, 0x00000000);
    MSDC_WRITE32(DAT_RD_DLY1, 0x00000000);

#if defined(CONFIG_ARCH_MT5396) || \
    defined(CONFIG_ARCH_MT5368) || \
    defined(CONFIG_ARCH_MT5389)
    MSDC_LOG(MSG_LVL_INFO, "PAD_CLK_SMT Setting\n");
    MSDC_SETBIT(SD20_PAD_CTL0, (0x1<<18));
#endif

}

void MsdcSetSampleEdge(uint fgWhere)
{
    uchar u1Sample;

    u1Sample = (_arEMMC_DevInfo[devIndex].u4Sample>>(8*fgWhere)) & 0xFF;

    MSDC_LOG(MSG_LVL_ERR, "Dev Num: %d, Timing Position: %d, Sample Edge: 0x%02X\n", devIndex, fgWhere, u1Sample);

    // Set Sample Edge
    MsdcSetSample(u1Sample);

}

int MsdcReset(void)
{
    uint i;

    // Reset MSDC
    MSDC_SETBIT(MSDC_CFG, MSDC_CFG_RST);

    for(i = 0; i<MSDC_RST_TIMEOUT_LIMIT_COUNT; i++)
    {
        if (0 == (MSDC_READ32(MSDC_CFG) & MSDC_CFG_RST))
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

    return MSDC_SUCCESS;
}

int MsdcClrFifo(void)
{
    uint i;
    // Reset FIFO
    MSDC_SETBIT(MSDC_FIFOCS, MSDC_FIFOCS_FIFOCLR);

    for(i = 0; i<MSDC_RST_TIMEOUT_LIMIT_COUNT; i++)
    {
        if (0 == (MSDC_READ32(MSDC_FIFOCS) & (MSDC_FIFOCS_FIFOCLR | MSDC_FIFOCS_TXFIFOCNT_MASK | MSDC_FIFOCS_RXFIFOCNT_MASK)))
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

    return MSDC_SUCCESS;
}

void MsdcChkFifo(void)
{
    // Check if rx/tx fifo is zero
    if ((MSDC_READ32(MSDC_FIFOCS) & (MSDC_FIFOCS_TXFIFOCNT_MASK | MSDC_FIFOCS_RXFIFOCNT_MASK)) != 0)
    {
        MSDC_LOG(MSG_LVL_WARN, "FiFo not 0, FIFOCS:0x%08X !!\r\n", MSDC_READ32(MSDC_FIFOCS));
        MsdcClrFifo();
    }
}

void MsdcClrIntr(void)
{
    // Check MSDC Interrupt vector register
    if  (0x00  != MSDC_READ32(MSDC_INT))
    {
        MSDC_LOG(MSG_LVL_WARN, "MSDC INT(0x%08X) not 0:0x%08X !!\r\n", MSDC_INT, MSDC_READ32(MSDC_INT));

        // Clear MSDC Interrupt vector register
        MSDC_WRITE32(MSDC_INT, MSDC_READ32(MSDC_INT));
    }
}

void MsdcStopDMA(void)
{
    MSDC_LOG(MSG_LVL_INFO, "DMA status: 0x%.8x\n",MSDC_READ32(DMA_CFG));
	
    MSDC_SETBIT(DMA_CTRL, DMA_CTRL_STOP);
    while(MSDC_READ32(DMA_CFG) & DMA_CFG_DMA_STATUS);
	
    MSDC_LOG(MSG_LVL_INFO, "DMA Stopped!\n");
}

int MsdcWaitClkStable (void)
{
    uint i;

    for(i = 0; i<MSDC_CLK_TIMEOUT_LIMIT_COUNT; i++)
    {
        if (0 != (MSDC_READ32(MSDC_CFG) & MSDC_CFG_CARD_CK_STABLE))
        {
            break;
        }

        HAL_Delay_us(1000);
        //udelay(1000);
    }
    if(i == MSDC_CLK_TIMEOUT_LIMIT_COUNT)
    {
        MSDC_LOG(MSG_LVL_ERR, "WaitClkStable Failed !\r\n");
        return MSDC_FAILED;
    }

    return MSDC_SUCCESS;
}

int MsdcWaitHostIdle(uint timeout)
{
	  uint i;
	  
    for(i = 0;i<timeout;i++)
    {
        if ((0 == (MSDC_READ32(SDC_STS) & (SDC_STS_SDCBUSY | SDC_STS_CMDBUSY))) && (0x00  == MSDC_READ32(MSDC_INT)))
        {
        	  return MSDC_SUCCESS;
        }
        //HAL_Delay_us(1000);
        udelay(1000);
    }
    
    return MSDC_FAILED;
}

void MsdSetDataMode(uint datamode)
{
	  //Assert((NULL_DATA_MODE < datamode) && (datamode <= ENHANCED_DMA_DATA_MODE));
	  
	  dataMode = datamode;
	
    if(datamode < BASIC_DMA_DATA_MODE)
    {
        MSDC_SETBIT(MSDC_CFG, MSDC_CFG_PIO_MODE);
        MSDC_LOG(MSG_LVL_INFO, "Set PIO Mode!\r\n");
    }
    else
    {
        MSDC_CLRBIT(MSDC_CFG, MSDC_CFG_PIO_MODE);
        MSDC_LOG(MSG_LVL_INFO, "Set DMA Mode!\r\n");
    }
}

int MsdcInit (struct mmc *mmc)
{
    // Reset MSDC
    MsdcReset();

    // SD/MMC Mode
    MSDC_SETBIT(MSDC_CFG, MSDC_CFG_PIO_MODE | MSDC_CFG_SD);
	// Reset MSDC
    MsdcReset();
    MsdSetDataMode(dataMode);

    // Disable sdio & Set bus to 1 bit mode
    MSDC_CLRBIT(SDC_CFG, SDC_CFG_SDIO | SDC_CFG_BW_MASK);

    // set clock mode (DIV mode)
    MSDC_CLRBIT(MSDC_CFG, (((uint)0x03) << 16));

    // Wait until clock is stable
    if (MSDC_FAILED == MsdcWaitClkStable())
    {
        return MSDC_FAILED;
    }

    // Set default RISC_SIZE for DWRD pio mode
    MSDC_WRITE32(MSDC_IOCON, (MSDC_READ32(MSDC_IOCON) & ~MSDC_IOCON_RISC_SIZE_MASK) | MSDC_IOCON_RISC_SIZE_DWRD);

    // Set Data timeout setting => Maximum setting
    MSDC_WRITE32(SDC_CFG, (MSDC_READ32(SDC_CFG) & ~(((uint)0xFF) << SDC_CFG_DTOC_SHIFT)) | (((uint)0xFF) << SDC_CFG_DTOC_SHIFT));

    // Clear Timing to default value
    MsdcClrTiming();

    return MSDC_SUCCESS;
}

void MsdcContinueClock (int i4ContinueClock)
{
    if (i4ContinueClock)
    {
       // Set clock continuous even if no command
       MSDC_SETBIT(MSDC_CFG, (((UINT32)0x01) << 1));
    }
    else
    {
       // Set clock power down if no command
       MSDC_CLRBIT(MSDC_CFG, (((UINT32)0x01) << 1));
    }
}

// Return when any interrupt is matched or timeout
int MsdcWaitIntr (uint vector, uint timeoutCnt, uint fgMode)
{
    uint i, retVector;

    // Clear Vector variable
    _u4MsdcAccuVect = 0;
    retVector = INT_SD_DATA_CRCERR;

    for(i = 0; i<(100*timeoutCnt); i++)
    {
        // Status match any bit
        if((fgMode == 0) && ((MSDC_READ32(MSDC_INT) & vector) != 0))
        {
            _u4MsdcAccuVect |= MSDC_READ32(MSDC_INT);
            MSDC_WRITE32(MSDC_INT, _u4MsdcAccuVect);
            return MSDC_SUCCESS;
        }
        else if((fgMode == 1) && ((MSDC_READ32(MSDC_INT) & vector) == vector))
        {
            _u4MsdcAccuVect |= MSDC_READ32(MSDC_INT);
            MSDC_WRITE32(MSDC_INT, _u4MsdcAccuVect);
            return MSDC_SUCCESS;
        }

        if((MSDC_READ32(MSDC_INT) & retVector) != 0)
        {
            // return directly
            return MSDC_FAILED; 
        }

        //HAL_Delay_us(1000);
        udelay(10);
    }

    // Timeout case
    return MSDC_FAILED;
}

void MsdcSetupCmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data, uint *pu4respType, uint *pu4sdcCmd)
{
    /* Figure out the response type */
    switch(cmd->cmdidx)
    {
    case MMC_CMD_GO_IDLE_STATE:
        *pu4respType = MMC_RSP_NONE;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_NO;
        break;
    case MMC_CMD_SEND_OP_COND:
        *pu4respType = MMC_RSP_R3;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R3;
        break;
    case MMC_CMD_ALL_SEND_CID:
		case MMC_CMD_SEND_CSD:
        *pu4respType = MMC_RSP_R2;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R2;
        break;
    case MMC_CMD_SET_RELATIVE_ADDR:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R1;
        break;
    case MMC_CMD_SWITCH:
        *pu4respType = MMC_RSP_R1b;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R1B;
        break;
    case MMC_CMD_SELECT_CARD:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R1;
        break;
    case MMC_CMD_SEND_EXT_CSD:
        if(mmc->version & MMC_VERSION_MMC) 
        {
            *pu4respType = MMC_RSP_R1;
            (*pu4sdcCmd) |= (SDC_CMD_RSPTYPE_R1 | DTYPE_SINGLE_BLK | SDC_CMD_READ);
            MSDC_WRITE32(SDC_BLK_NUM, 1);
        }
        else
        { // for SD or unkown card
            *pu4respType = MMC_RSP_R7;
            (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R7;
        }
        break;
    case MMC_CMD_STOP_TRANSMISSION:
        *pu4respType = MMC_RSP_R1b;
        (*pu4sdcCmd) |= (SDC_CMD_STOP | SDC_CMD_RSPTYPE_R1B);
        break;
    case MMC_CMD_SEND_STATUS:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R1;
        break;
    case MMC_CMD_SET_BLOCKLEN:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R1;
        break;
    case MMC_CMD_READ_SINGLE_BLOCK:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= (SDC_CMD_RSPTYPE_R1 | DTYPE_SINGLE_BLK | SDC_CMD_READ);
        MSDC_WRITE32(SDC_BLK_NUM, 1);
        break;
    case MMC_CMD_READ_MULTIPLE_BLOCK:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= (SDC_CMD_RSPTYPE_R1 | DTYPE_MULTI_BLK | SDC_CMD_READ);
        if(data)
        {
            MSDC_WRITE32(SDC_BLK_NUM, data->blocks);
        }
        break;
    case MMC_CMD_WRITE_SINGLE_BLOCK:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= (SDC_CMD_RSPTYPE_R1 | DTYPE_SINGLE_BLK | SDC_CMD_WRITE);
        MSDC_WRITE32(SDC_BLK_NUM, 1);
        break;
    case MMC_CMD_WRITE_MULTIPLE_BLOCK:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= (SDC_CMD_RSPTYPE_R1 | DTYPE_MULTI_BLK | SDC_CMD_WRITE);
        if(data)
        {
            MSDC_WRITE32(SDC_BLK_NUM, data->blocks);
        }
        break;
    case MMC_CMD_PROGRAM_CSD:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= (SDC_CMD_RSPTYPE_R1 | DTYPE_SINGLE_BLK | SDC_CMD_WRITE);
        if(data)
        {
            MSDC_WRITE32(SDC_BLK_NUM, data->blocks);
        }
        break;
    case MMC_CMD_SET_WRITE_PROT:
        *pu4respType = MMC_RSP_R1b;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R1B;
        break;
    case MMC_CMD_CLR_WRITE_PROT:
        *pu4respType = MMC_RSP_R1b;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R1B;
        break;
    case MMC_CMD_ERASE_START:
    case MMC_CMD_ERASE_END:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= (SDC_CMD_RSPTYPE_R1);
        break;
    case MMC_CMD_ERASE:
        *pu4respType = MMC_RSP_R1b;
        (*pu4sdcCmd) |= (SDC_CMD_RSPTYPE_R1B);
        break;
    case SD_CMD_APP_SEND_OP_COND:
        *pu4respType = MMC_RSP_R3;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R3;
        break;
    case MMC_CMD_APP_CMD:
        *pu4respType = MMC_RSP_R1;
        (*pu4sdcCmd) |= SDC_CMD_RSPTYPE_R1;
        break;
    }

    // Set Blk Length
    if(data)
    {
        (*pu4sdcCmd) |= ((data->blocksize) << SDC_CMD_LEN_SHIFT);
        MSDC_LOG(MSG_LVL_INFO, "block size: %08x\n", data->blocksize);
    }

    // Set SDC_CMD.CMD
    (*pu4sdcCmd) |= (cmd->cmdidx & 0x3F);

    // Set SDC_CMD.AUTO
    if((cmd->flags)>>4)
        (*pu4sdcCmd) |= (SDC_CMD_AUTO_CMD12);

    // Set SDC Argument
    MSDC_WRITE32(SDC_ARG, cmd->cmdarg);

    /* Send the commands to the device */
    MSDC_WRITE32(SDC_CMD, (*pu4sdcCmd));

}


void MsdcHandleResp(struct mmc_cmd *cmd, uint *pu4respType, uint *pu4sdcCmd)
{
    // Handle the response
    switch (*pu4respType)
    {
    case MMC_RSP_NONE:
        MSDC_LOG(MSG_LVL_INFO, "%s: CMD%d CMD 0x%08X ARG 0x%08X RESPONSE_NO\r\n", __FUNCTION__, cmd->cmdidx, *pu4sdcCmd, cmd->cmdarg);
        break;
    case MMC_RSP_R1:
    case MMC_RSP_R1b:
        cmd->response[0] = MSDC_READ32(SDC_RESP0);
        MSDC_LOG(MSG_LVL_INFO, "%s: CMD%d CMD 0x%08X ARG 0x%08X RESPONSE_R1/R1B/R5/R6/R6 0x%08X\r\n", __FUNCTION__, cmd->cmdidx, *pu4sdcCmd, cmd->cmdarg, cmd->response[0]);
        break;
    case MMC_RSP_R2:
        cmd->response[0] = MSDC_READ32(SDC_RESP3);
        cmd->response[1] = MSDC_READ32(SDC_RESP2);
        cmd->response[2] = MSDC_READ32(SDC_RESP1);
        cmd->response[3] = MSDC_READ32(SDC_RESP0);
        MSDC_LOG(MSG_LVL_INFO, "%s: CMD%d CMD 0x%08X ARG 0x%08X RESPONSE_R2 0x%08X 0x%08X 0x%08X 0x%08X\r\n", __FUNCTION__, cmd->cmdidx, *pu4sdcCmd, cmd->cmdarg, 
                                                                                     cmd->response[0], cmd->response[1], cmd->response[2], cmd->response[3]);
        break;
    case MMC_RSP_R3:
        cmd->response[0] = MSDC_READ32(SDC_RESP0);
        MSDC_LOG(MSG_LVL_INFO, "%s: CMD%d CMD 0x%08X ARG 0x%08X RESPONSE_R3/R4 0x%08X\r\n", __FUNCTION__, cmd->cmdidx, *pu4sdcCmd, cmd->cmdarg, cmd->response[0]);
        break;
    }
}

int MsdcReqCmdStart(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    uint respType = MMC_RSP_NONE;
    uint sdcCmd = 0;
    int i4Ret = MSDC_SUCCESS;
    uint u4CmdDoneVect;

    // Check if rx/tx fifo is zero
    MsdcChkFifo();

    // Clear interrupt Vector
    MsdcClrIntr();

    MSDC_LOG(MSG_LVL_INFO, "MsdcSendCmd : CMD%d, ARGU!%08X!\r\n", cmd->cmdidx, cmd->cmdarg);
    
    if(MsdcWaitHostIdle(MSDC_WAIT_SDC_BUS_TIMEOUT_LIMIT_COUNT))
    {
        i4Ret = ERR_CMD_FAILED;
        MSDC_LOG(MSG_LVL_ERR, "Wait HOST idle failed: SDC_STS(%08X), MSDC_INT(%08X)!\n", MSDC_READ32(SDC_STS), MSDC_READ32(MSDC_INT));
        goto ErrorEnd;
    }

    MsdcSetupCmd(mmc, cmd, data, &respType, &sdcCmd);

    // Wait for command and response if existed
    u4CmdDoneVect = INT_SD_CMDRDY | INT_SD_CMDTO | INT_SD_RESP_CRCERR;

    if (MSDC_SUCCESS != MsdcWaitIntr(u4CmdDoneVect, MSDC_WAIT_CMD_TIMEOUT_LIMIT_COUNT, 0))
    {
        MSDC_LOG(MSG_LVL_ERR, "Failed to send CMD/RESP, DoneVect = 0x%x.\r\n", u4CmdDoneVect);
        i4Ret = ERR_CMD_FAILED;
        goto ErrorEnd;
    }

    MSDC_LOG(MSG_LVL_INFO, "Interrupt = %08X\n", _u4MsdcAccuVect);

    if (_u4MsdcAccuVect & INT_SD_CMDTO)
    {
        MSDC_LOG(MSG_LVL_ERR, "CMD%d ARG 0x%08X - CMD Timeout (AccuVect 0x%08X INTR 0x%08X).\r\n", cmd->cmdidx, cmd->cmdarg, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT));
        i4Ret = ERR_CMD_FAILED;
        goto ErrorEnd;
    }
    else if (_u4MsdcAccuVect & INT_SD_RESP_CRCERR)
    {
        MSDC_LOG(MSG_LVL_ERR, "CMD%d ARG 0x%08X - CMD CRC Error (AccuVect 0x%08X INTR 0x%08X).\r\n", cmd->cmdidx, cmd->cmdarg, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT));
        i4Ret = ERR_CMD_FAILED;
        goto ErrorEnd;
    }
    else if ((_u4MsdcAccuVect & (~(INT_SD_CMDRDY))) || (0 != MSDC_READ32(MSDC_INT)))
    {
        MSDC_LOG(MSG_LVL_ERR, "CMD%d ARG 0x%08X - UnExpect status (AccuVect 0x%08X INTR 0x%08X).\r\n", cmd->cmdidx, cmd->cmdarg, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT));
        i4Ret = ERR_CMD_FAILED;
        goto ErrorEnd;
    }

    // Handle the response
    MsdcHandleResp(cmd, &respType, &sdcCmd);

ErrorEnd:
    return i4Ret;

}

int MsdcReqDataStop(struct mmc *mmc)
{
    struct mmc_cmd stop;
    int i4Ret;
	
    memset(&stop, 0, sizeof(struct mmc_cmd));
    stop.cmdidx = MMC_CMD_STOP_TRANSMISSION;
    stop.cmdarg = 0;
    stop.resp_type = MMC_RSP_R1b;
    stop.flags = 0;
	
    i4Ret = MsdcReqCmdStart(mmc, &stop, NULL);
    MSDC_LOG(MSG_LVL_INFO, "Stop Sending-Data State(%d)!\n", i4Ret);

    return i4Ret;
}

int MsdcErrorHandling(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS;
    MSDC_LOG(MSG_LVL_INFO, "Start Error Handling...!\n");

    // Reset MSDC
    MsdcReset();

    // Stop DMA
    if((cmd->flags & 0x0F) > 1)
    {
        MsdcStopDMA();
    }

    // Clear FIFO and wait it becomes 0
    i4Ret = MsdcClrFifo();
    if(MSDC_SUCCESS != i4Ret)
    {
        goto ErrorEnd;	 
    }

    // Clear MSDC interrupts and make sure all are cleared
    MsdcClrIntr();
    if  (0x00  != MSDC_READ32(MSDC_INT))
    {
        i4Ret = MSDC_FAILED;
        goto ErrorEnd;	
    }

    // Send Stop Command for Multi-Write/Read
    if(data && (data->blocks > 1))
    {
        if (MsdcReqDataStop(mmc)) 
        {
            MSDC_LOG(MSG_LVL_WARN, "mmc fail to send stop cmd\n");
        }
    }

    MSDC_LOG(MSG_LVL_INFO, "End Error Handling...!\n");

ErrorEnd:
    return i4Ret;
}

void MsdcSetBuffBits(uint *pBuff, uint bits)
{
    (*pBuff) |= bits;
}

void MsdcClrBuffBits(uint *pBuff, uint bits)
{
    (*pBuff) &= (~bits);
}

int MsdcCalCheckSum(uchar *pBuf, uint len)
{
    uint i, checksum = 0;
    for(i = 0;i<len;i++)
    {
        checksum += pBuf[i];    
    }

    return checksum;
}

int MsdcHandleDataTransfer(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS; 
    uint u4CmdDoneVect, fgAutoCmd12, IntrWaitVect = 0, IntrCheckVect = 0;
	  
    fgAutoCmd12 = ((cmd->flags>>4)?1:0);

    if (PIO_DATA_MODE == (cmd->flags & 0x0F))
    {
        uint u4RxFifoCnt, u4TxFifoCnt;
        uint *pBufData = (uint*)((data->flags & MMC_DATA_READ)?(data->dest):(data->src));
        uint u4BufLen = (uint)((data->blocks)*(data->blocksize));
        uint u4BufEnd = (uint)pBufData + u4BufLen;
        uint u4RxCnt = 0;

        u4CmdDoneVect = INT_SD_DATTO | INT_SD_DATA_CRCERR;
        
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
                u4RxFifoCnt = (MSDC_READ32(MSDC_FIFOCS) & MSDC_FIFOCS_RXFIFOCNT_MASK);

                while ((u4BufLen) && (sizeof(int) <= u4RxFifoCnt))
                {
                    // Read Data
                    *pBufData = MSDC_READ32(MSDC_RXDATA);
                    pBufData++;

                    u4RxFifoCnt -= sizeof(int);
                    u4RxCnt += sizeof(int);

                    if(u4RxCnt == SDHC_BLK_SIZE)
                    {
                        // Check CRC error happens by every 512 Byte
                        // Check if done vector occurs
                        if (u4CmdDoneVect & MSDC_READ32(MSDC_INT))
                        {
                            MSDC_LOG(MSG_LVL_ERR, "Read Error Break !!\r\n");
                            break;
                        }

                        u4RxCnt = 0;
                        u4BufLen -= SDHC_BLK_SIZE;
                    }
                }
            }
        }
        else
		    {
            while (u4BufEnd > (UINT32)pBufData)
            {
                // Check if error done vector occurs
                if (u4CmdDoneVect & (_u4MsdcAccuVect | MSDC_READ32(MSDC_INT)))
                {
                    MSDC_LOG(MSG_LVL_ERR, "DoneVect:0x%08X, accuVect:0x%08X,  INTR:0x%08X\r\n", u4CmdDoneVect, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT));
                    break;
                }

                // wait until fifo has enough space
                while(1)
                {
                    if((MSDC_READ32(MSDC_FIFOCS) & MSDC_FIFOCS_TXFIFOCNT_MASK) == 0)
                    {
                        break;
                    }
                }

                u4TxFifoCnt = MSDC_FIFO_LEN;

                if (sizeof(int) <= u4TxFifoCnt)
                {
                    while ((u4BufEnd > (UINT32)pBufData) && (sizeof(int) <= u4TxFifoCnt))
                    {
                        // Write Data
                        MSDC_WRITE32(MSDC_TXDATA, *pBufData);
                        pBufData++;
                        u4TxFifoCnt -= sizeof(int);
                    }
                }
            }

        }

        // Wait for data complete
        if (MSDC_SUCCESS != MsdcWaitIntr((IntrWaitVect), MSDC_WAIT_DATA_COMPLETE_TIMEOUT_LIMIT_COUNT, 1))
        {
            MSDC_LOG(MSG_LVL_ERR, "Wait Intr timeout (AccuVect 0x%08X INTR 0x%08X).\r\n", _u4MsdcAccuVect, MSDC_READ32(MSDC_INT));
            i4Ret = ERR_DAT_FAILED;
            goto ErrorEnd;
        }

        if (_u4MsdcAccuVect & INT_SD_DATTO)
        {
            MSDC_LOG(MSG_LVL_ERR, "CMD%d ARG 0x%08X - Data Timeout (AccuVect 0x%08X INTR 0x%08X).\r\n", cmd->cmdidx, cmd->cmdarg, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT));
            i4Ret = ERR_DAT_FAILED;
            goto ErrorEnd;
        }
        else if (_u4MsdcAccuVect & INT_SD_DATA_CRCERR)
        {
            MSDC_LOG(MSG_LVL_ERR, "CMD%d ARG 0x%08X - Data CRC Error (AccuVect 0x%08X INTR 0x%08X).\r\n", cmd->cmdidx, cmd->cmdarg, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT));
            i4Ret = ERR_DAT_FAILED;
            goto ErrorEnd;
        }
        else if ((_u4MsdcAccuVect & ~(IntrCheckVect)) || (0 != MSDC_READ32(MSDC_INT)))
        {
            MSDC_LOG(MSG_LVL_ERR, "UnExpect status (AccuVect 0x%08X INTR 0x%08X).\r\n", _u4MsdcAccuVect, MSDC_READ32(MSDC_INT));
            i4Ret = ERR_DAT_FAILED;
            goto ErrorEnd;
        }
    }
    else if(BASIC_DMA_DATA_MODE == (cmd->flags & 0x0F))
    {
        uint *pBufData = (uint*)((data->flags & MMC_DATA_READ)?(data->dest):(data->src));
        uint u4BufLen = (uint)((data->blocks)*(data->blocksize));
        unsigned int u4AccLen = 0;
        
	      if(fgAutoCmd12)
	      {
	  	     IntrWaitVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_SD_AUTOCMD_RDY;
	  	     IntrCheckVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_SD_AUTOCMD_RDY;
	  	     MSDC_LOG(MSG_LVL_INFO, "1IntrWaitVect:%08X, Check vector: %08X\n", IntrWaitVect, IntrCheckVect);
	      }
	      else
	      {
	  	     IntrWaitVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE;
	  	     IntrCheckVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE;
	  	     MSDC_LOG(MSG_LVL_INFO, "2IntrWaitVect:%08X, Check vector: %08X\n", IntrWaitVect, IntrCheckVect);
	      }

        MSDC_LOG(MSG_LVL_INFO, "DMA Mode: pBufData = %08X, Length = %08X\r\n", (UINT32)pBufData, u4BufLen);

        while (u4AccLen < u4BufLen) 
        {
            MSDC_WRITE32(DMA_SA, (unsigned int) pBufData + u4AccLen);
            //TODO: Check why the buffer length of the last one can't be 512??
            if (u4BufLen - u4AccLen <= BASIC_DMA_MAX_LEN) 
            {
                MSDC_LOG(MSG_LVL_INFO, "->Last: AccLen = %08X, waitvector: %08X, checkvector: %08X\r\n", u4AccLen, IntrWaitVect, IntrCheckVect);
                MSDC_WRITE32(DMA_CTRL, ((u4BufLen - u4AccLen) << DMA_CTRL_XFER_SIZE_SHIFT) | (DMA_BST_64 << DMA_CTRL_BST_SHIFT) | DMA_CTRL_LAST_BUF | DMA_CTRL_START);

                // Wait for sd xfer complete
                if (MSDC_SUCCESS != MsdcWaitIntr(IntrWaitVect, MSDC_WAIT_DATA_COMPLETE_TIMEOUT_LIMIT_COUNT, 1)) 
                {
                    MSDC_LOG(MSG_LVL_ERR, "(L)%s: Failed to send/receive data (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
                    return ERR_DAT_FAILED;
                }
                if ((_u4MsdcAccuVect & ~(IntrCheckVect)) || (0 != MSDC_READ32(MSDC_INT))) 
                {
                    MSDC_LOG(MSG_LVL_ERR, "%s: Unexpected status (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
                    return ERR_DAT_FAILED;
                }
                
                // Check DMA status
                if (0 != (MSDC_READ32(DMA_CFG) & (DMA_CFG_DMA_STATUS))) 
                {
                    MSDC_LOG(MSG_LVL_ERR, "%s: Incorrect DMA status. DMA_CFG: 0x%08X\r\n", __FUNCTION__, MSDC_READ32(DMA_CFG));
                    return ERR_DAT_FAILED;
                }

                u4AccLen += u4BufLen - u4AccLen;
            }
            else 
            {
                MSDC_LOG(MSG_LVL_INFO, "->AccLen = %08X, waitvector: %08X, checkvector: %08X\r\n", u4AccLen, IntrWaitVect, IntrCheckVect);
                MSDC_WRITE32(DMA_CTRL, (BASIC_DMA_MAX_LEN << DMA_CTRL_XFER_SIZE_SHIFT) | (DMA_BST_64 << DMA_CTRL_BST_SHIFT) | DMA_CTRL_START);

                if (MSDC_SUCCESS != MsdcWaitIntr(INT_DMA_XFER_DONE, MSDC_WAIT_DATA_COMPLETE_TIMEOUT_LIMIT_COUNT, 1)) 
                {
                    MSDC_LOG(MSG_LVL_ERR, "(N)%s: Failed to send/receive data (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
                    return ERR_DAT_FAILED;
                }

                if ((_u4MsdcAccuVect & ~(INT_DMA_XFER_DONE)) || (0 != MSDC_READ32(MSDC_INT))) 
                {
                    MSDC_LOG(MSG_LVL_ERR, "%s: Unexpected status (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
                    return ERR_DAT_FAILED;
                }
                u4AccLen += BASIC_DMA_MAX_LEN;
            }
        }
    }
    else if(DESC_DMA_DATA_MODE == (cmd->flags & 0x0F))
    {
	      if(fgAutoCmd12)
	      {
	  	     IntrWaitVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_SD_AUTOCMD_RDY;
	  	     IntrCheckVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_SD_AUTOCMD_RDY | INT_DMA_Q_EMPTY;
	  	     MSDC_LOG(MSG_LVL_INFO, "1IntrWaitVect:%08X, Check vector: %08X\n", IntrWaitVect, IntrCheckVect);
	      }
	      else
	      {
	  	     IntrWaitVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE;
	  	     IntrCheckVect = INT_SD_XFER_COMPLETE | INT_DMA_XFER_DONE | INT_DMA_Q_EMPTY;
	  	     MSDC_LOG(MSG_LVL_INFO, "2IntrWaitVect:%08X, Check vector: %08X\n", IntrWaitVect, IntrCheckVect);
	      }
	      
        MSDC_SETBIT(DMA_CTRL, DMA_CTRL_START);

        // Wait for sd xfer complete
        if (MSDC_SUCCESS != MsdcWaitIntr(IntrWaitVect, 10*MSDC_WAIT_DATA_COMPLETE_TIMEOUT_LIMIT_COUNT, 1)) 
        {
            MSDC_LOG(MSG_LVL_ERR, "%s: Failed to send/receive data (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
            i4Ret = CMD_ERR_DATA_FAILED;
            goto ErrorEnd;
        }

        if ((_u4MsdcAccuVect & ~(IntrCheckVect)) || (0 != MSDC_READ32(MSDC_INT))) 
        {
            MSDC_LOG(MSG_LVL_ERR, "%s: Unexpected status (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
            i4Ret = CMD_ERR_DATA_FAILED;
            goto ErrorEnd;
        }

        if(MSDC_READ32(DMA_CFG) & (DMA_CFG_GPD_CS_ERR))
        {
            MSDC_LOG(MSG_LVL_ERR, "Descriptor DMA GPD checksum error");
            i4Ret = CMD_ERR_DATA_FAILED;
            goto ErrorEnd;
        }

        if(MSDC_READ32(DMA_CFG) & (DMA_CFG_BD_CS_ERR))
        {
            printf("Descriptor DMA BD checksum error");
            i4Ret = CMD_ERR_DATA_FAILED;
            goto ErrorEnd;
        }

        // Check DMA status
        if (0 != (MSDC_READ32(DMA_CFG) & (DMA_CFG_DMA_STATUS))) 
        {
            printf("%s: Incorrect DMA status. DMA_CFG: 0x%08X\r\n", __FUNCTION__, MSDC_READ32(DMA_CFG));
            i4Ret = CMD_ERR_DATA_FAILED;
            goto ErrorEnd;
        }
    }


ErrorEnd:
    return i4Ret;

}

int MsdcReqCmdTune(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    int result;
    uint rsmpl, cur_rsmpl, orig_rsmpl;
    uint rrdly, cur_rrdly, orig_rrdly;
    uint skip = 1;

    MSDC_LOG(MSG_LVL_WARN, "----->Go into Command Tune!\n");
	
    MsdcReset();
    MsdcClrFifo();
    MsdcClrIntr();

    orig_rsmpl = ((MSDC_READ32(MSDC_IOCON) & MSDC_IOCON_R_SMPL) >> MSDC_IOCON_R_SMPL_SHIFT);
    orig_rrdly = ((MSDC_READ32(PAD_TUNE) & PAD_CMD_RESP_RXDLY) >> PAD_CMD_RESP_RXDLY_SHIFT);

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
        }

        MSDC_CLRBIT(MSDC_IOCON, MSDC_IOCON_R_SMPL);
        MSDC_SETBIT(MSDC_IOCON, (cur_rsmpl << MSDC_IOCON_R_SMPL_SHIFT));

        result = MsdcReqCmdStart(mmc, cmd, data);
        if(result == MSDC_SUCCESS)
        {
            MSDC_LOG(MSG_LVL_WARN, "Command Tune Success\n");
            return MSDC_SUCCESS;
        }

        /* Lv2: PAD_CMD_RESP_RXDLY[26:22] */              	
        cur_rrdly = (orig_rrdly + rrdly + 1) % 32;
        MSDC_CLRBIT(PAD_TUNE, PAD_CMD_RESP_RXDLY);
        MSDC_SETBIT(PAD_TUNE, (cur_rrdly << PAD_CMD_RESP_RXDLY_SHIFT));
    }while (++rrdly < 32);

    return result;
}

int MsdcRequest(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS;

    i4Ret = MsdcReqCmdStart(mmc, cmd, data);
    if(MSDC_SUCCESS != i4Ret)
    {
        if(data)
        {
            if(MSDC_SUCCESS != MsdcErrorHandling(mmc, cmd, data))
            {
                goto ErrorEnd;
            }
        }
		
        i4Ret = MsdcReqCmdTune(mmc, cmd, data);
        if(MSDC_SUCCESS != i4Ret)
        {
            goto ErrorEnd;
        }
    }

    if(data)
    {
        i4Ret = MsdcHandleDataTransfer(mmc, cmd, data);
        if(MSDC_SUCCESS != i4Ret)
        {
            if(MSDC_SUCCESS != MsdcErrorHandling(mmc, cmd, data))
            {
                goto ErrorEnd;
            }
        }
    }

ErrorEnd:
	return i4Ret;
	
}

int MsdcReqDataReadTune(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    uint ddr=0;	
    uint dcrc = 0;
    uint rxdly, cur_rxdly0, cur_rxdly1;
    //uint rxdly, cur_rxdly;
    uint dsmpl, cur_dsmpl,  orig_dsmpl;
    uint cur_dat0,  cur_dat1,  cur_dat2,  cur_dat3;
    uint cur_dat4,  cur_dat5,  cur_dat6,  cur_dat7;
    uint orig_dat0, orig_dat1, orig_dat2, orig_dat3;
    uint orig_dat4, orig_dat5, orig_dat6, orig_dat7;
    //uint cur_dat, orig_dat;
    int result = -1;
    uint skip = 1;

    MSDC_LOG(MSG_LVL_WARN, "----->Go into Data Read Tune!\n");

    orig_dsmpl = ((MSDC_READ32(MSDC_IOCON) & MSDC_IOCON_D_SMPL) >> MSDC_IOCON_D_SMPL_SHIFT);

    /* Tune Method 2. */
    MSDC_CLRBIT(MSDC_IOCON, MSDC_IOCON_D_DLYLINE_SEL);
    MSDC_SETBIT(MSDC_IOCON, (1 << MSDC_IOCON_D_DLYLINE_SEL_SHIFT));

    MSDC_LOG(MSG_LVL_WARN, "CRC(R) Error Register: %08X!\n", MSDC_READ32(SDC_DATCRC_STS));

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

            MSDC_CLRBIT(MSDC_IOCON, MSDC_IOCON_D_SMPL);
            MSDC_SETBIT(MSDC_IOCON, (cur_dsmpl << MSDC_IOCON_D_SMPL_SHIFT));

            result = MsdcRequest(mmc, cmd, data);

            dcrc = MSDC_READ32(SDC_DATCRC_STS);
            if(!ddr)
                dcrc &= (~SDC_DATCRC_STS_NEG);
			
            MSDC_LOG(MSG_LVL_WARN, "TUNE_READ<%s> dcrc<0x%x> DATRDDLY0/1<0x%x><0x%x> dsmpl<0x%x>",
                        (result == MSDC_SUCCESS && dcrc == 0) ? "PASS" : "FAIL", dcrc,
                        MSDC_READ32(DAT_RD_DLY0), MSDC_READ32(DAT_RD_DLY1), cur_dsmpl);
			
            if((result == MSDC_SUCCESS) && dcrc == 0)
            {
                goto done;
            }
            else
            {
                // Tuning Data error but Command error happens, directly return
                if((result != MSDC_SUCCESS) && (result != ERR_DAT_FAILED))
                {
                    MSDC_LOG(MSG_LVL_WARN, "TUNE_READ(1): result<0x%x> ", result);	
                    goto done;  
                }
                else if((result != MSDC_SUCCESS) && (result == ERR_DAT_FAILED))
                {
                	  // Going On
                    MSDC_LOG(MSG_LVL_WARN, "TUNE_READ(2): result<0x%x>", result);	
                }
            }
        }

        cur_rxdly0 = MSDC_READ32(DAT_RD_DLY0);
        cur_rxdly1 = MSDC_READ32(DAT_RD_DLY1);

        /* E1 ECO. YD: Reverse */
        if (MSDC_READ32(ECO_VER) >= 4) 
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
        if (MSDC_READ32(ECO_VER) >= 4) 
        {
            cur_rxdly0 = (cur_dat0 << 24) | (cur_dat1 << 16) | (cur_dat2 << 8) | (cur_dat3 << 0);
            cur_rxdly1 = (cur_dat4 << 24) | (cur_dat5 << 16) | (cur_dat6 << 8) | (cur_dat7 << 0);
        }
        else
        {
            cur_rxdly0 = (cur_dat3 << 24) | (cur_dat2 << 16) | (cur_dat1 << 8) | (cur_dat0 << 0);
            cur_rxdly1 = (cur_dat7 << 24) | (cur_dat6 << 16) | (cur_dat5 << 8) | (cur_dat4 << 0);   
        }
	
        MSDC_WRITE32(DAT_RD_DLY0, cur_rxdly0);
        MSDC_WRITE32(DAT_RD_DLY1, cur_rxdly1);   
    }while(++rxdly < 32);


done:		
    return result;
}

int MsdcReqDataWriteTune(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    unsigned int wrrdly, cur_wrrdly = 0, orig_wrrdly;
    unsigned int dsmpl,  cur_dsmpl,  orig_dsmpl;
    unsigned int rxdly,  cur_rxdly0;
    unsigned int orig_dat0, orig_dat1, orig_dat2, orig_dat3;
    unsigned int cur_dat0,  cur_dat1,  cur_dat2,  cur_dat3;
    int result = -1;
    unsigned int skip = 1;

    MSDC_LOG(MSG_LVL_WARN, "----->Go into Data Write Tune!\n");

    orig_wrrdly = ((MSDC_READ32(PAD_TUNE) & PAD_DAT_WR_RXDLY) >> PAD_DAT_WR_RXDLY_SHIFT);
    orig_dsmpl = ((MSDC_READ32(MSDC_IOCON) & MSDC_IOCON_D_SMPL) >> MSDC_IOCON_D_SMPL_SHIFT);

    MSDC_LOG(MSG_LVL_WARN, "CRC(W) Error Register: %08X!\n", MSDC_READ32(SDC_DATCRC_STS));

    /* Tune Method 2. just DAT0 */  
    MSDC_CLRBIT(MSDC_IOCON, MSDC_IOCON_D_DLYLINE_SEL);
    MSDC_SETBIT(MSDC_IOCON, (1 << MSDC_IOCON_D_DLYLINE_SEL_SHIFT));
    cur_rxdly0 = MSDC_READ32(DAT_RD_DLY0);

    /* E1 ECO. YD: Reverse */
    if (MSDC_READ32(ECO_VER) >= 4) 
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

                MSDC_CLRBIT(MSDC_IOCON, MSDC_IOCON_D_SMPL);
                MSDC_SETBIT(MSDC_IOCON, (cur_dsmpl << MSDC_IOCON_D_SMPL_SHIFT));

                result = MsdcRequest(mmc, cmd, data);

                MSDC_LOG(MSG_LVL_WARN,  "TUNE_WRITE<%s> DSPL<%d> DATWRDLY<%d> MSDC_DAT_RDDLY0<0x%x>", 
                                  (result == MSDC_SUCCESS ? "PASS" : "FAIL"), cur_dsmpl, cur_wrrdly, cur_rxdly0);

                if(result == MSDC_SUCCESS)
                {
                    goto done;
                }
                else
                {
                	  // Tuning Data error but Command error happens, directly return
                    if((result != MSDC_SUCCESS) && (result != ERR_DAT_FAILED))
                    {
                        MSDC_LOG(MSG_LVL_WARN, "TUNE_WRITE(1): result<0x%x>", result);
						
                        goto done; 
                    }
                    else if((result != MSDC_SUCCESS) && (result == ERR_DAT_FAILED))
                    {
                    	  // Going On
                        MSDC_LOG(MSG_LVL_WARN, "TUNE_WRITE(2): result<0x%x>", result);
                    }
                }
            }

            cur_wrrdly = (orig_wrrdly + wrrdly + 1) % 32;
            MSDC_CLRBIT(PAD_TUNE, PAD_DAT_WR_RXDLY);
            MSDC_SETBIT(PAD_TUNE, (cur_wrrdly << PAD_DAT_WR_RXDLY_SHIFT));
        }while(++wrrdly < 32);
		
        cur_dat0 = (orig_dat0 + rxdly) % 32; /* only adjust bit-1 for crc */
        cur_dat1 = orig_dat1;
        cur_dat2 = orig_dat2;
        cur_dat3 = orig_dat3;                    

        /* E1 ECO. YD: Reverse */
        if (MSDC_READ32(ECO_VER) >= 4) 
        {
            cur_rxdly0 = (cur_dat0 << 24) | (cur_dat1 << 16) | (cur_dat2 << 8) | (cur_dat3 << 0);  
        }
        else
        {
            cur_rxdly0 = (cur_dat3 << 24) | (cur_dat2 << 16) | (cur_dat1 << 8) | (cur_dat0 << 0); 
        }
        MSDC_WRITE32(DAT_RD_DLY0, cur_rxdly0); 
    }while(++rxdly < 32);


done:
    return result;

}

int MsdcReqDataTune(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS;
	
    if (data->flags & MMC_DATA_READ)
    {
        i4Ret = MsdcReqDataReadTune(mmc, cmd, data);
    }
    else
    {
        i4Ret = MsdcReqDataWriteTune(mmc, cmd, data);
    }

    return i4Ret;
}

int MsdcSendCmd (struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    int i4Ret = MSDC_SUCCESS;

    i4Ret = MsdcRequest(mmc, cmd, data);
    if((i4Ret != MSDC_SUCCESS) && data && (i4Ret == ERR_DAT_FAILED))
    {
        i4Ret = MsdcReqDataTune(mmc, cmd, data);	
        if(MSDC_SUCCESS != i4Ret)
        {
            goto ErrorEnd;
        }
    } 

ErrorEnd:
    return i4Ret;
}

int MsdcDMAWaitIntr(struct mmc_cmd *cmd, uint cur_off, uint bytes, uint total_sz)
{
    uint fgAutoCmd12, IntrWaitVect = 0, IntrCheckVect = 0;
	  
    fgAutoCmd12 = ((cmd->flags>>4)?1:0);
	  	
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
	  	
	  	
    if(total_sz <= (cur_off + bytes))
    {
        // Wait for sd xfer complete
        if (MSDC_SUCCESS != MsdcWaitIntr(IntrWaitVect, MSDC_WAIT_DATA_COMPLETE_TIMEOUT_LIMIT_COUNT, 1)) 
        {
            MSDC_LOG(MSG_LVL_ERR, "(L)%s: Failed to send/receive data (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
            return MSDC_FAILED;
        }
        if ((_u4MsdcAccuVect & ~(IntrCheckVect)) || (0 != MSDC_READ32(MSDC_INT))) 
        {
            MSDC_LOG(MSG_LVL_ERR, "%s: Unexpected status (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
            return MSDC_FAILED;
        }
	    
        // Check DMA status
        if (0 != (MSDC_READ32(DMA_CFG) & (DMA_CFG_DMA_STATUS))) 
        {
            MSDC_LOG(MSG_LVL_ERR, "%s: Incorrect DMA status. DMA_CFG: 0x%08X\r\n", __FUNCTION__, MSDC_READ32(DMA_CFG));
            return MSDC_FAILED;
        }
	
        return MSDC_SUCCESS;
    }
    else 
    {
        if (MSDC_SUCCESS != MsdcWaitIntr(INT_DMA_XFER_DONE, MSDC_WAIT_DATA_COMPLETE_TIMEOUT_LIMIT_COUNT, 1)) 
        {
            MSDC_LOG(MSG_LVL_ERR, "(N)%s: Failed to send/receive data (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
            return MSDC_FAILED;
        }

        if ((_u4MsdcAccuVect & ~(INT_DMA_XFER_DONE)) || (0 != MSDC_READ32(MSDC_INT))) 
        {
            MSDC_LOG(MSG_LVL_ERR, "%s: Unexpected status (AccuVect 0x%08X INTR 0x%08X). %s line %d\r\n", __FUNCTION__, _u4MsdcAccuVect, MSDC_READ32(MSDC_INT), __FILE__, __LINE__);
            return MSDC_FAILED;
        }
		
        return MSDC_SUCCESS;
    } 
}

int MsdcDMAStart(uint cur_pos, uint cur_off, uint bytes, uint total_sz)
{  	
    MSDC_WRITE32(DMA_SA, cur_pos);

    if (total_sz <= cur_off + bytes) 
    {
        MSDC_LOG(MSG_LVL_INFO, "->Last: AccLen = %08X\r\n", (total_sz - cur_off));
        MSDC_WRITE32(DMA_CTRL, ((total_sz - cur_off) << DMA_CTRL_XFER_SIZE_SHIFT) | (DMA_BST_64 << DMA_CTRL_BST_SHIFT) | DMA_CTRL_LAST_BUF | DMA_CTRL_START);
    }
    else 
    {
	      MSDC_LOG(MSG_LVL_INFO, "->AccLen = %08X\r\n", bytes);
        MSDC_WRITE32(DMA_CTRL, (bytes << DMA_CTRL_XFER_SIZE_SHIFT) | (DMA_BST_64 << DMA_CTRL_BST_SHIFT) | DMA_CTRL_START);
    } 

    return MSDC_SUCCESS;
}

int MsdcDMATernmial(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{  
    if(MsdcErrorHandling(mmc, cmd, data))
    {
        return MSDC_FAILED;	
    }	
    
    return MSDC_SUCCESS;
}

int MsdcSetClkfreq(uint clkFreq)
{
    uint sdClkSel = 0, expFreq = 0, index = 0;
    clkFreq /= (1000*1000);
    
    index = sizeof(_u4MsdcFreq)/sizeof(uint) - 1;
    if(clkFreq >= _u4MsdcFreq[0])
    {
        sdClkSel = _u4MsdcFreqSelVal[0];
        expFreq = _u4MsdcFreq[0];    	
    }
    else if(clkFreq <= _u4MsdcFreq[index])
    {
        sdClkSel = _u4MsdcFreqSelVal[index];
        expFreq = _u4MsdcFreq[index]; 	
    }
    else
    {
        for(index -= 1;index > 0;index--)
        {
            if((clkFreq >= _u4MsdcFreq[index]) && (clkFreq < _u4MsdcFreq[index-1]))
            {
                sdClkSel = _u4MsdcFreqSelVal[index];
                expFreq = _u4MsdcFreq[index]; 
                break;	
            }	
        }
    }
    MSDC_LOG(MSG_LVL_INFO, "Request Clk:%dMHz Bus Clk:%dMHz, SelVal:%d\n", clkFreq, expFreq, sdClkSel);
    if (_u4MsdcCurFreq == expFreq)
    {
        return MSDC_SUCCESS;
    }

    // Gate msdc_src_clk
    if (IS_IC_5860_A_ES2())
    {
        MSDC_SETBIT(MSDC_CLOCK_SELECTION_REG1, MSDC_CLOCK_CKGEN_GATE_BIT);
    }
    else
    {
        MSDC_SETBIT(MSDC_CLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_GATE_BIT);
    }

    // set clock mode (DIV mode)
    MSDC_CLRBIT(MSDC_CFG, (((UINT32)0x03) << 16));
    if (expFreq >= 1)
    {
        MSDC_SETBIT(MSDC_CFG, (((UINT32)0x01) << 16));
		
    }
    else
    {
        MSDC_CLRBIT(MSDC_CFG, (((UINT32)0xFF) << 8));
        MSDC_SETBIT(MSDC_CFG, (((UINT32)0x11) << 8));
    }
    
    if (IS_IC_5860_A_ES2())
    {
        // Clr msdc_src_clk selection
        MSDC_CLRBIT(MSDC_CLOCK_SELECTION_REG1, MSDC_CLOCK_CKGEN_SLECTION_MASK);
        MSDC_SETBIT(MSDC_CLOCK_SELECTION_REG1, sdClkSel<<0);
    //MSDC_LOG(MSG_LVL_INFO,"11,The register 0x%08X  value is: 0x%08X\n", MSDC_CLOCK_SELECTION_REG1,(*(volatile unsigned int *)(MSDC_CLOCK_SELECTION_REG1)));

        // Disable gating msdc_src_clk
        MSDC_CLRBIT(MSDC_CLOCK_SELECTION_REG1, MSDC_CLOCK_CKGEN_GATE_BIT);
	}
    else
    {
        // Clr msdc_src_clk selection
        MSDC_CLRBIT(MSDC_CLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_SLECTION_MASK);
        MSDC_SETBIT(MSDC_CLOCK_SELECTION_REG0, sdClkSel<<0);
    //MSDC_LOG(MSG_LVL_INFO,"11,The register 0x%08X  value is: 0x%08X\n", MSDC_CLOCK_SELECTION_REG0,(*(volatile unsigned int *)(MSDC_CLOCK_SELECTION_REG0)));

        // Disable gating msdc_src_clk
        MSDC_CLRBIT(MSDC_CLOCK_SELECTION_REG0, MSDC_CLOCK_CKGEN_GATE_BIT);    
    }

    // Wait until clock is stable
    if (MSDC_FAILED == MsdcWaitClkStable())
    {
        MSDC_LOG(MSG_LVL_ERR, "11,Set Bus Clock as %d(MHz) Failed!\n", expFreq);
        return MSDC_FAILED;
    }

    MSDC_LOG(MSG_LVL_INFO, "Set Bus Clock as %d(MHz) Success!\n", expFreq);	
    _u4MsdcCurFreq = expFreq;

    return MSDC_SUCCESS;
}

int MsdcSetBusWidth (int busWidth)
{
    /* Modify MSDC Register Settings */
    if (1 == busWidth)
    {
        MSDC_WRITE32(SDC_CFG, (MSDC_READ32(SDC_CFG) & ~SDC_CFG_BW_MASK) | (0x00 <<  SDC_CFG_BW_SHIFT));
    }
    else if (4 == busWidth)
    {
        MSDC_WRITE32(SDC_CFG, (MSDC_READ32(SDC_CFG) & ~SDC_CFG_BW_MASK) | (0x01 <<  SDC_CFG_BW_SHIFT));
    }
    else if (8 == busWidth)
    {
        MSDC_WRITE32(SDC_CFG, (MSDC_READ32(SDC_CFG) & ~SDC_CFG_BW_MASK) | (0x02 <<  SDC_CFG_BW_SHIFT));
    }

    return MSDC_SUCCESS;
}


void MsdcSetIos(struct mmc *mmc)
{
    uint clock = mmc->clock;
    uint busWidth = mmc->bus_width;

    MsdcSetClkfreq(clock);

    MsdcSetBusWidth(busWidth);

}

int MsdcEmmcBootStop (uint fgResetMsdc)
{
    uint i;

    // Issue Reset Command
    MSDC_WRITE32(SDC_ARG, 0x00000000);

    // Set SDC Argument
    MSDC_WRITE32(SDC_CMD, 0x00001000);

    /* Send the commands to the device */
    // Boot Wait Delay
    MSDC_SETBIT(EMMC_CFG0, BTWDLY_2x32);

    // Boot Stop
    MSDC_SETBIT(EMMC_CFG0, BTSTOP);

    for(i = 0;i<MSDC_WAIT_BTSTS_0_TIMEOUT_LIMIT_COUNT;i++)
    {
        if (!(MSDC_READ32(EMMC_STS) & (BTSTS)))
        {
            break;
        }

        HAL_Delay_us(1000);
    }
    if(i==MSDC_WAIT_BTSTS_0_TIMEOUT_LIMIT_COUNT)
    {
        // Fetal Error
        return MSDC_FAILED;
    }

    // Step 6 :
    // Program Boot Support = 0
    MSDC_CLRBIT(EMMC_CFG0, (BTSUP));
    // Clear FIFO
    MsdcClrFifo();
    // Clear MSC interrupt
    MsdcClrIntr();

    // Msdc Error Handling include Msdc Reset
    if (fgResetMsdc)
    {
        // Emmc Status W1C, so write 1 to clear old status
        MSDC_WRITE32(EMMC_STS, MSDC_READ32(EMMC_STS));
        //MsdcErrorHandling();
    }

    return MSDC_SUCCESS;
}


void MSDC_PinMux(uint u4Ch)
{	
    ch = u4Ch;
    if (IS_IC_5860_A_ES2())
    {
        if(ch == 0)
        {
            MSDC_CLRBIT(0xF000D620, (0x01<<7));
            MSDC_SETBIT(0xF000D620, (0x01<<7));
            MSDC_CLRBIT(0xF000D610, 0x3FFFF<<12);
            MSDC_SETBIT(0xF000D610, 0x12492<<12);
            //Local Arbitor open
            MSDC_CLRBIT(0xF0012200, (0x03<<16) | (0x1F<<0));	 
            MSDC_SETBIT(0xF0012200, (0x01<<16) | (0x1F<<0));
        }
        else
        {
            //pinmux register d604[5:2], function 2 - CMD/CLK/DAT0/DAT1/DAT2/DAT3
            //TODO: the pinmux setting after MT5860 ECO 
            MSDC_CLRBIT(0xF000D604, (0xf<<2));
            MSDC_SETBIT(0xF000D604, 0xa<<2);
            //Local Arbitor open
            MSDC_CLRBIT(0xF0012200, (0x03<<16) | (0x1F<<5));	
            MSDC_SETBIT(0xF0012200, (0x01<<16) | (0x1F<<5));
        }
    }
    else
    {
        MSDC_CLRBIT(0xF000D604, (0x03<<4));
        MSDC_SETBIT(0xF000D604, (0x02<<4));
        //Local Arbitor open
        //MSDC_WRITE32(0xF0012200, 0x0001001F); 
    }
}

void mtk_mmc_init(void)
{
    emmc_dev = (struct mmc*)malloc(sizeof(struct mmc));

    //Initial some structure element
    //sprintf(emmc_dev->name, "%s", "emmc0");
    emmc_dev->voltages = (MMC_VDD_30_31|MMC_VDD_31_32|MMC_VDD_32_33|MMC_VDD_33_34);
    emmc_dev->f_min = 397000;
    emmc_dev->f_max = 48000000;
    emmc_dev->host_caps = (MMC_MODE_HS|MMC_MODE_HS_52MHz|SD_DATA_4BIT);
    emmc_dev->send_cmd = MsdcSendCmd;
    emmc_dev->set_ios = MsdcSetIos;
    emmc_dev->init = MsdcInit;
    mmc_register(emmc_dev);
#ifdef CC_MTD_ENCRYPT_SUPPORT   
    // aes init
    MsdcAesInit(128);
#endif
#if (MSDC_EMMC_INIT_UNIFY_EN == 1)
    LDR_ENV_T *prLdrEnv = (LDR_ENV_T *)CC_LDR_ENV_OFFSET;
    msdc_env_t msdcenv;
    
    if(sizeof(prLdrEnv->szMSDCenv) != sizeof(msdc_env_t))
    {
        printf("%d != %d\n", sizeof(prLdrEnv->szMSDCenv), sizeof(msdc_env_t));
        BUG();   	
    }  
    
    memcpy((void *)(&msdcenv), (void *)(prLdrEnv->szMSDCenv), sizeof(msdc_env_t));
    //memset(EXT_CSD, 0x00, 512);
    memcpy((void *)(&emmc_dev->ocr), (void *)(msdcenv.ocr), sizeof(uint));
    memcpy((void *)(&emmc_dev->rca), (void *)(msdcenv.rca), sizeof(ushort));
    memcpy((void *)(emmc_dev->cid), (void *)(msdcenv.cid), 4*sizeof(uint));
    memcpy((void *)(emmc_dev->csd), (void *)(msdcenv.csd), 4*sizeof(uint));
    //EXT_CSD[185] = msdcenv.ext_csd_185;
    //EXT_CSD[192] = msdcenv.ext_csd_192;
    //EXT_CSD[196] = msdcenv.ext_csd_196;
    //EXT_CSD[212] = msdcenv.ext_csd_212;
    //EXT_CSD[213] = msdcenv.ext_csd_213;
    //EXT_CSD[214] = msdcenv.ext_csd_214;
    //EXT_CSD[215] = msdcenv.ext_csd_215;
    _u4MsdcCurFreq = _u4MsdcFreq[SD_DEFAULT_NORM_CLOCK_INDEX];
    emmc_dev->clock = _u4MsdcCurFreq;
    

    if(msdcenv.fgHost != 0)
    {
    	printf("HOST 1\n");
        if (IS_IC_5860_A_ES2())
        {
#ifdef CC_EMMC_4BIT
            emmc_dev->host_caps |= MMC_MODE_4BIT;
            emmc_dev->bus_width = 4;
#else
            emmc_dev->host_caps |= MMC_MODE_8BIT;
            emmc_dev->bus_width = 8;
#endif
        }
        else
        {
            emmc_dev->host_caps |= MMC_MODE_4BIT;
            emmc_dev->bus_width = 4;
        }
    }
    else
    {
        printf("HOST 0\n");
        emmc_dev->host_caps |= MMC_MODE_4BIT;
        emmc_dev->bus_width = 4;
    }
     
    MSDC_PinMux(msdcenv.fgHost);
    MsdSetDataMode(dataMode);
    if (mmc_init_fast(emmc_dev)) 
    {
        MSDC_LOG(MSG_LVL_ERR, "MMC init failed\n");
        //BUG();
    }
#else
    _u4MsdcCurFreq = 48;
    
    emmc_dev->host_caps |= MMC_MODE_8BIT;
    //emmc_dev->host_caps |= MMC_MODE_4BIT;
    
    MSDC_PinMux(1);
    //MSDC_PinMux(0);
    if (mmc_init(emmc_dev)) 
    {
        MSDC_LOG(MSG_LVL_ERR, "MMC init failed\n");
       // BUG();
    }
#endif	

}

int board_mmc_init(bd_t *bis)
{
    mtk_mmc_init();
    return 0;
}

int cpu_mmc_init(bd_t *bis)
{
    return 0;
}

#endif // CC_EMMC_BOOT

