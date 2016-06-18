/**
 *	@file star.c
 *	@brief Star Ethernet Driver
 *	@author mtk02196
 *
 */

#include "star.h"

#ifdef FASTPATH
#include <asm/arch/fastpath/fastpath.h>
#endif
#ifdef SUPPORT_MOD_LOG                                                                                                                                                                                             
#include <asm/arch/log/mod_log.h>
#endif

static spinlock_t star_lock;

#if defined (USE_RX_TASKLET) && defined (USE_RX_NAPI)
#warning "USE_RX_NAPI and USE_RX_TASKLET can not be used at the same time"
#undef USE_RX_TASKLET
#endif

void star_if_queue_up_down(struct up_down_queue_par *par);

/* ============= Global variable ============= */
#ifdef STARMAC_SUPPORT_ETHTOOL
static struct ethtool_ops starmac_ethtool_ops;
#endif
static struct net_device *this_device = NULL;
static u32 eth_base = 0;
static u32 pinmux_base = 0;
//static u32 pdwnc_base = 0;
//static u32 eth_pdwnc_base = 0;
//static u32 eth_chksum_base = 0;
static u32 ipll_base1 = 0;
//static u32 ipll_base2 = 0;
//static u32 bsp_base = 0;
static StarDev *star_dev = NULL;
static struct sk_buff *skb_dbg; /* skb pointer used for debugging */
static int multicast_filter_limit = 32;
static u32 fgNetHWActive = 1;
static u32 fgStarOpen=FALSE;

int star_dbg_lvl = STAR_DBG_LVL_DEFAULT;
/* ============= Global Definition ============= */
//#define DEFAULT_MAC_ADDRESS         { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 }
#define DEFAULT_MAC_ADDRESS         {0x00, 0x0C, 0xE7, 0x06, 0x00, 0x00}
//#define DEFAULT_MAC_ADDRESS         {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

#define IRQ_SWITCH			    6

//#define SUPPORT_ETH_POWERDOWN 1

#ifdef SUPPORT_ETH_POWERDOWN
int Ethernet_suspend(void);
int Ethernet_resume(void);
#endif//#ifdef SUPPORT_ETH_POWERDOWN

#ifdef USE_RX_NAPI
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
static int star_poll(struct napi_struct *napi, int budget)
#else
static int star_poll(struct net_device *dev, int *budget)
#endif
#else
static void star_receive(struct net_device *dev)
#endif
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	int retval;
#ifdef USE_RX_NAPI	
	int npackets = 0;
    //u32 intrStatus = 0;
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,10)
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10)
	int quota = min(dev->quota, *budget);
#endif
	int done = 0;
#endif

#if (defined USE_RX_NAPI) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	struct net_device *dev;
	starPrv = container_of(napi, StarPrivate, napi);
	dev = starPrv->dev;
#else
	starPrv = netdev_priv(dev);
#endif
	starDev = &starPrv->stardev;

#ifdef USE_RX_NAPI
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	while(npackets < budget)
#else
	while (npackets < quota)
#endif
#else
	do
#endif
    {
		u32 extBuf;
		u32 ctrlLen;
		u32 len;
		dma_addr_t dmaBuf;
		struct sk_buff *currSkb;
		struct sk_buff *newSkb;

		retval = StarDmaRxGet(starDev, &dmaBuf, &ctrlLen, &extBuf);

		if (retval >= 0 && extBuf != 0)
		{
			currSkb = (struct sk_buff *)extBuf;
			dma_unmap_single(NULL, dmaBuf, skb_tailroom(currSkb), DMA_FROM_DEVICE);

			if (StarDmaRxValid(ctrlLen))
			{
#ifdef CHECKSUM_OFFLOAD
				int cksumOk = 0;

				if (StarDmaRxProtocolIP(ctrlLen) && !StarDmaRxIPCksumErr(ctrlLen)) /* IP packet & IP checksum correct */
				{
					cksumOk = 1;
					if (StarDmaRxProtocolTCP(ctrlLen) || StarDmaRxProtocolUDP(ctrlLen)) /* TCP/UDP packet */
					{
						if (StarDmaRxL4CksumErr(ctrlLen)) /* L4 checksum error */
						{
							cksumOk = 0;
						}
					}
				}
#endif
				len = StarDmaRxLength(ctrlLen);

				if (len < ETH_MAX_LEN_PKT_COPY) /* Allocate new skb */
				{
					newSkb = currSkb;
#ifdef FASTPATH
					currSkb = dev_alloc_skb(len + 2 + ETH_HDR_LEN_FOR_FASTPATH);
					if (currSkb) skb_reserve(currSkb, 2 + ETH_HDR_LEN_FOR_FASTPATH);
#else
					currSkb = dev_alloc_skb(len + 2);
					if (currSkb) skb_reserve(currSkb, 2);
#endif
	
					if (!currSkb) /* No skb can be allocate */
					{
						currSkb = newSkb;
						newSkb = NULL;
					} else
					{
						memcpy(currSkb->data, newSkb->data, len);
					}
				} else
				{
#ifdef FASTPATH
					newSkb = dev_alloc_skb(ETH_MAX_FRAME_SIZE + ETH_HDR_LEN_FOR_FASTPATH);
#else
					newSkb = dev_alloc_skb(ETH_MAX_FRAME_SIZE);
#endif

					/* Shift to 16 byte alignment */
                    /*
					if ((u32)(newSkb->tail) & 0xf) 
					{
						u32 shift = ((u32)(newSkb->tail) & 0xf);
						skb_reserve(newSkb, shift);
					}
					*/
					if(newSkb)
                    {               
                        if((u32)(newSkb->tail) & (ETH_SKB_ALIGNMENT-1))
                        {
                            u32 offset = ((u32)(newSkb->tail) & (ETH_SKB_ALIGNMENT-1));
                            skb_reserve(newSkb, ETH_SKB_ALIGNMENT - offset);
                        }
                    }
                    else
                    {
                        STAR_MSG(STAR_ERR,"star_receive mem alloc fail(1), packet dropped\n");
                    }

					/* for zero copy */
#ifdef FASTPATH
					if (newSkb) skb_reserve(newSkb, 2 + ETH_HDR_LEN_FOR_FASTPATH);
#else
					if (newSkb) skb_reserve(newSkb, 2);
#endif
				}

				if (!newSkb) /* No skb can be allocated -> packet drop */
				{
					if (printk_ratelimit())
					{
						STAR_MSG(STAR_ERR, "star_receive mem alloc fail, packet dropped\n");
					}
					starDev->stats.rx_dropped ++;
					newSkb = currSkb;
				} else
				{
					skb_put(currSkb, len);
#ifdef CHECKSUM_OFFLOAD
					if (cksumOk) {currSkb->ip_summed = CHECKSUM_UNNECESSARY;} else {currSkb->ip_summed = CHECKSUM_NONE;}
#else
					currSkb->ip_summed = CHECKSUM_NONE;
#endif
					currSkb->dev = dev;
					currSkb->protocol = eth_type_trans(currSkb, dev);

#ifdef FASTPATH
					skb_push(currSkb, ETH_HLEN);
					if (!fastpath_in(FASTPATH_ID, currSkb))
					{
						skb_pull(currSkb, ETH_HLEN);
#endif

#ifdef USE_RX_NAPI					
						netif_receive_skb(currSkb);     /* send the packet up protocol stack */
#else
						netif_rx(currSkb);
#endif

#ifdef FASTPATH
					}
#endif
					dev->last_rx = jiffies;   		/* set the time of the last receive */
					starDev->stats.rx_packets ++;
					starDev->stats.rx_bytes += len;
#ifdef USE_RX_NAPI
					npackets ++;
#endif
				}
			} else
			{   /* Error packet */
				newSkb = currSkb;
				starDev->stats.rx_errors ++;
				starDev->stats.rx_crc_errors += StarDmaRxCrcErr(ctrlLen);
			}

			dmaBuf = dma_map_single(NULL, 
                                    newSkb->tail - 2/*Because Star Ethernet buffer must 16 byte align*/, 
                                    skb_tailroom(newSkb), 
                                    DMA_FROM_DEVICE);
			StarDmaRxSet(starDev, dmaBuf, skb_tailroom(newSkb), (u32)newSkb);
#ifdef USE_RX_NAPI
		}else
		{
			done = 1;
			break;
#endif
		}
    }
#ifndef USE_RX_NAPI
	while (retval >= 0);
#endif

	StarDmaRxResume(starDev);

#ifdef USE_RX_NAPI
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	if (done)
	{
		local_irq_disable();
		napi_complete(napi);
		StarIntrRxEnable(starDev);   /* Enable rx interrupt */        
		local_irq_enable();
	}
	return(npackets);    
#else /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) */
	*budget -= npackets;
	dev->quota -= npackets;
	if (done) /* All packets have been processed */
	{
		local_irq_disable();
		__netif_rx_complete(dev);
		StarIntrRxEnable(starDev);   /* Enable rx interrupt */        
		local_irq_enable();                
        return 0;
	}
	/* there are another packets need process */
	return 1;
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26) */
#endif /* USE_RX_NAPI */
}

static void star_finish_xmit(struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	int retval;
	int wake = 0;

	STAR_MSG(STAR_VERB, "star_finish_xmit(%s)\n", dev->name);

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	do {
		u32 extBuf;
		u32 ctrlLen;
		u32 len;
		dma_addr_t dmaBuf;
		unsigned long flags;
		
		spin_lock_irqsave(&starPrv->lock, flags);
		retval = StarDmaTxGet(starDev, &dmaBuf, &ctrlLen, &extBuf);
		spin_unlock_irqrestore(&starPrv->lock, flags);

		if (retval >= 0 && extBuf != 0)
		{
			len = StarDmaTxLength(ctrlLen);
			dma_unmap_single(NULL, dmaBuf, len, DMA_TO_DEVICE);
			STAR_MSG(STAR_VERB, 
                     "star_finish_xmit(%s) - get tx descriptor %d for skb 0x%08x, length = %08x\n", 
                     dev->name, retval, extBuf, len);

			/* ??????????? reuse skb */

			dev_kfree_skb_irq((struct sk_buff *)extBuf);

			/* Tx statistics, use MIB? */
			starDev->stats.tx_bytes += len;
			starDev->stats.tx_packets ++;
			
			wake = 1;
		}
	} while (retval >= 0);

	if (wake)
	{
		netif_wake_queue(dev);
	}
}

#if defined (USE_TX_TASKLET) || defined (USE_RX_TASKLET)
static void star_dsr(unsigned long data)
{
	struct net_device *dev = (struct net_device *)data;
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;

	STAR_MSG(STAR_VERB, "star_dsr(%s)\n", dev->name);

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

#ifdef USE_TX_TASKLET
	if (starPrv->tsk_tx)
	{
		starPrv->tsk_tx = 0;
		star_finish_xmit(dev);
	}
#endif
#ifdef USE_RX_TASKLET
	if (starPrv->tsk_rx)
	{
		starPrv->tsk_rx = 0;
		star_receive(dev);
	}
#endif
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
static irqreturn_t star_isr(int irq, void *dev_id)
#else
static irqreturn_t star_isr(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
    struct net_device *dev = (struct net_device *)dev_id;
    StarPrivate *starPrv = NULL;
    StarDev *starDev = NULL;
    u32 intrStatus;

    if (!dev)
    {
        STAR_MSG(STAR_ERR, "star_isr - unknown device\n");
        return IRQ_NONE;
    }

	STAR_MSG(STAR_VERB, "star_isr(%s)\n", dev->name);

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	intrStatus = StarIntrStatus(starDev);
#ifdef USE_RX_NAPI
	StarIntrClear(starDev, (intrStatus & (~STAR_INT_STA_RXC)));
#else
	StarIntrClear(starDev, intrStatus);
#endif

	do 
    {
		STAR_MSG(STAR_VERB, "star_isr - interrupt status = 0x%08x\n", intrStatus);

		if (intrStatus & STAR_INT_STA_RXC) /* Rx Complete */
		{
			STAR_MSG(STAR_VERB, "rx complete\n");
#ifdef USE_RX_NAPI
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
			if (napi_schedule_prep(&starPrv->napi))
			{
				StarIntrRxDisable(starDev); /* Disable rx interrupts */
				StarIntrClear(starDev, STAR_INT_STA_RXC); /* Clear rx interrupt */
				__napi_schedule(&starPrv->napi);
			}
	#else
			if (netif_rx_schedule_prep(dev))
			{
				StarIntrRxDisable(starDev); /* Disable rx interrupts */
				StarIntrClear(starDev, STAR_INT_STA_RXC); /* Clear rx interrupt */
				__netif_rx_schedule(dev);
			}
	#endif
#else
	#ifdef USE_RX_TASKLET
			starPrv->tsk_rx = 1;
	#else
			star_receive(dev);
	#endif
#endif
		}
		
		if (intrStatus & STAR_INT_STA_RXQF) /* Rx Queue Full */
		{
			STAR_MSG(STAR_VERB, "rx queue full\n");
		}

		if (intrStatus & STAR_INT_STA_RXFIFOFULL) /* Rx FIFO Full */
		{
			STAR_MSG(STAR_WARN, "rx fifo full\n");
		}
		
		if (intrStatus & STAR_INT_STA_TXC) /* Tx Complete */
		{
			STAR_MSG(STAR_VERB, " tx complete\n");
#ifdef USE_TX_TASKLET
			starPrv->tsk_tx = 1;
#else
			star_finish_xmit(dev);
#endif
		}

		if (intrStatus & STAR_INT_STA_TXQE) /* Tx Queue Empty */
		{
			STAR_MSG(STAR_VERB, "tx queue empty\n");
		}

     #ifdef CC_ETHERNET_6896
		if (intrStatus & STAR_INT_STA_RX_PCODE) 
		{
		 	STAR_MSG(STAR_DBG, "Rx PCODE\n");
		}
     #else
		if (intrStatus & STAR_INT_STA_TXFIFOUDRUN) /* Tx FIFO underrun */
		{
			STAR_MSG(STAR_ERR, "tx fifo underrun\n");
		}
    #endif

		if (intrStatus & STAR_INT_STA_MAGICPKT) /* Receive magic packet */
		{
			STAR_MSG(STAR_WARN, "magic packet received\n");
		}

		if (intrStatus & STAR_INT_STA_MIBCNTHALF) /* MIB counter reach 2G (0x80000000) */
		{
			STAR_MSG(STAR_VERB, " mib counter reach 2G\n");
			StarMibInit(starDev);
		}

		if (intrStatus & STAR_INT_STA_PORTCHANGE) /* Port status change */
		{
			STAR_MSG(STAR_DBG, "port status change\n");
            StarLinkStatusChange(starDev);
		}

		intrStatus = StarIntrStatus(starDev);  /* read interrupt requests came during interrupt handling */

#ifdef USE_RX_NAPI
        StarIntrClear(starDev, (intrStatus & (~STAR_INT_STA_RXC)));
#else
        StarIntrClear(starDev, intrStatus);
#endif
    }
#ifdef USE_RX_NAPI
    while ((intrStatus & (~STAR_INT_STA_RXC)) != 0);
#else
    while (intrStatus != 0);
#endif 

	STAR_MSG(STAR_VERB, "star_isr return\n");
	
#if defined (USE_TX_TASKLET) && defined (USE_RX_TASKLET)
	if (starPrv->tsk_tx || starPrv->tsk_rx)
#elif defined (USE_TX_TASKLET)
	if (starPrv->tsk_tx)
#elif defined (USE_RX_TASKLET)
	if (starPrv->tsk_rx)
#endif
    {
#if defined (USE_TX_TASKLET) || defined (USE_RX_TASKLET)
		tasklet_schedule(&starPrv->dsr);
#endif	
    }
	return IRQ_HANDLED;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void star_netpoll(struct net_device *dev)
{
	StarPrivate *tp   = netdev_priv(dev);
	StarDev     *pdev = tp->mii.dev;

	disable_irq(pdev->irq);
	star_isr(pdev->irq, dev);
	enable_irq(pdev->irq);
}
#endif

static int star_open(struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	int retval;
	u32 intrStatus;

	STAR_MSG(STAR_DBG, "star_open(%s)\n", dev->name);

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;
	
    /* Start RX FIFO receive */
    StarNICPdSet(starDev, 0);
    
	StarIntrDisable(starDev);
	StarDmaTxStop(starDev);
	StarDmaRxStop(starDev);
	intrStatus = StarIntrStatus(starDev);
	StarIntrClear(starDev, intrStatus);

    /* init carrier to off */
    netif_carrier_off(dev);
    
	/* Register NIC interrupt */
	STAR_MSG(STAR_DBG, "request interrupt vector=%d\n", dev->irq);
	if (request_irq(dev->irq, star_isr, 0, dev->name, dev) != 0)	/* request non-shared interrupt */
	{
		STAR_MSG(STAR_ERR, "interrupt %d request fail\n", dev->irq);
		return -ENODEV;
	}

	/* ============== Initialization due to Pin Trap in ASIC mode ==============  */
	/* ===== There are 3 modes: Internal switch/internal phy/RvMII(IC+175C) ===== */
	if (1) //(StarIsASICMode(starDev))
	{
		/* MII Pad output enable */
		StarSetBit(STAR_ETHPHY(starDev->base), STAR_ETHPHY_MIIPAD_OE);
		/* Force SMI Enable */
		StarSetBit(STAR_ETHPHY(starDev->base), STAR_ETHPHY_FRC_SMI_EN);
		/* Disable extended retry - for 10-half mode runs normally */
		StarClearBit(STAR_TEST1(starDev->base), STAR_TEST1_EXTEND_RETRY);
		
		switch (StarPhyMode(starDev))
		{
			case INT_PHY_MODE:
			{
				STAR_MSG(STAR_DBG, "Internal PHY mode\n");
				break;
			}
			case INT_SWITCH_MODE:
			{
				break;
			}
			case EXT_MII_MODE:
			{
				// TODO: External MII device initialization
				break;
			}
			case EXT_RGMII_MODE:
			{
				// TODO: Giga-Ethernet initialization
				break;
			}
			case EXT_RVMII_MODE:
			{
				u32 phyIdentifier;

				phyIdentifier = StarMdcMdioRead(starDev, 0, 3);
				if (phyIdentifier == 0x0d80)
				{	/* IC+ 175C */
					u16 ctrlRegValue = StarMdcMdioRead(starDev, 31, 5);
					ctrlRegValue |= 0x8000; /* Enable P4EXT */
					ctrlRegValue |= 0x0800; /* Enable MII0 mac mode */
					ctrlRegValue &= 0xfbff; /* Disable MII0 RMII mode */
					StarMdcMdioWrite(starDev, 31, 5, ctrlRegValue);
				}
				break;
			}
			default:
			{
				STAR_MSG(STAR_ERR, "star_open unknown Eth mode!\n\r");
				return -1;
			}
		}
	}

	STAR_MSG(STAR_VERB, "MAC Initialization\n");
	if (StarMacInit(starDev, dev->dev_addr) != 0)     /* MAC Initialization */
	{
		STAR_MSG(STAR_ERR, "MAC init fail\n");
		return -ENODEV;		
	}

    STAR_MSG(STAR_VERB, "StarDmaInit virAddr=0x%08x, dmaAddr=0x%08x\n", 
                starPrv->desc_virAddr, starPrv->desc_dmaAddr);
	if (StarDmaInit(starDev, starPrv->desc_virAddr, starPrv->desc_dmaAddr) != 0)    /* DMA Initialization */
	{
		STAR_MSG(STAR_ERR, "DMA init fail\n");
		return -ENODEV;
	}

	STAR_MSG(STAR_VERB, "PHY Control Initialization\n");
	if (StarPhyCtrlInit(starDev, 1/*Enable PHY auto-polling*/, starPrv->phy_addr) != 0)
	{
		STAR_MSG(STAR_ERR, "PHY Control init fail\n");
		return -ENODEV;
	}

	do { /* pre-allocate Rx buffer */
		dma_addr_t dmaBuf;
#ifdef FASTPATH
		struct sk_buff *skb = dev_alloc_skb(dev->mtu + ETH_EXTRA_PKT_LEN + ETH_HDR_LEN_FOR_FASTPATH);
#else
		struct sk_buff *skb = dev_alloc_skb(dev->mtu + ETH_EXTRA_PKT_LEN);
#endif

    		if (skb == NULL)
    		{
    			STAR_MSG(STAR_ERR, "Error! No momory for rx sk_buff!\n");
    			/* TODO */
    			return -ENOMEM;
    		}

		/* Shift to 16 byte alignment */
        if((u32)(skb->tail) & (ETH_SKB_ALIGNMENT-1))
        {
            u32 offset = ((u32)(skb->tail) & (ETH_SKB_ALIGNMENT-1));
            skb_reserve(skb, ETH_SKB_ALIGNMENT - offset);
        }


		/* Reserve 2 bytes for zero copy */
#ifdef FASTPATH		
		if (skb) {skb_reserve(skb, 2 + ETH_HDR_LEN_FOR_FASTPATH);}
#else
        /* Reserving 2 bytes makes the skb->data points to
           a 16-byte aligned address after eth_type_trans is called.
           Since eth_type_trans will extracts the pointer (ETH_LEN)
           14 bytes. With this 2 bytes reserved, the skb->data
           can be 16-byte aligned before passing to upper layer. */
		if (skb) {skb_reserve(skb, 2);}
#endif
		
        /* Note:
            We pass to dma addr with skb->tail-2 (4N aligned). But
            the RX_OFFSET_2B_DIS has to be set to 0, making DMA to write
            tail (4N+2) addr. 
         */
		dmaBuf = dma_map_single(NULL, 
                                skb->tail - 2/*Because Star Ethernet buffer must 16 byte align*/, 
                                skb_tailroom(skb), 
                                DMA_FROM_DEVICE);
		retval = StarDmaRxSet(starDev, dmaBuf, skb_tailroom(skb), (u32)skb);
		STAR_MSG(STAR_VERB, "rx descriptor idx:%d for skb %p\n", retval, skb);
		
		if (retval < 0)
		{
			dma_unmap_single(NULL, dmaBuf, skb_tailroom(skb), DMA_FROM_DEVICE);
			dev_kfree_skb(skb);
		}
	} while (retval >= 0);

#ifdef USE_RX_NAPI
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	napi_enable(&starPrv->napi);
#endif
#endif

	intrStatus = StarIntrStatus(starDev);
	StarIntrClear(starDev, intrStatus);
	StarIntrEnable(starDev);
	StarDmaTxEnable(starDev);
	StarDmaRxEnable(starDev);


    /* PHY reset */
    StarPhyReset(starDev);
    /* wait for a while until PHY ready */
    msleep(100);
    {
        /* NOTE by sarick---------------------
           This is a workaround for the problem that, when powering on 
           platform and the Ethernet is connected with a 10Mbps Hub, the
           platform could fail to get IP.
           We found if disabling and then enabling the Auto-Negotiation,
           the problem does not appear.
         */
        u32 val = 0;
        
        STAR_MSG(STAR_VERB, "Re-enabling AN\n");    
        val = StarGetReg(STAR_PHY_CTRL1(starDev->base));
        val &= ~(STAR_PHY_CTRL1_ANEN);
        StarSetReg(STAR_PHY_CTRL1(starDev->base), val);
        val |= STAR_PHY_CTRL1_ANEN;
        StarSetReg(STAR_PHY_CTRL1(starDev->base), val);
    }
    
    StarLinkStatusChange(starDev);
	netif_start_queue(dev);
	fgNetHWActive = 1;
	fgStarOpen = TRUE ;
    STAR_MSG(STAR_VERB, "star_open done\n");	
	return 0;
}

static int star_stop(struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	int retval;
	u32 intrStatus;

	STAR_MSG(STAR_DBG, "star_stop(%s)\n", dev->name);

    if(fgStarOpen == FALSE) return -1 ;
	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	netif_stop_queue(dev);

#ifdef USE_RX_NAPI
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	napi_disable(&starPrv->napi);
#endif
#endif

	StarIntrDisable(starDev);
	StarDmaTxStop(starDev);
	StarDmaRxStop(starDev);
	intrStatus = StarIntrStatus(starDev);
	StarIntrClear(starDev, intrStatus);

	free_irq(dev->irq, dev);      /* free nic irq */

	do { /* Free Tx descriptor */		
		u32 extBuf;
		u32 ctrlLen;
		u32 len;
		dma_addr_t dmaBuf;
	
		retval = StarDmaTxGet(starDev, &dmaBuf, &ctrlLen, &extBuf);
		if (retval >= 0 && extBuf != 0)
		{
			len = StarDmaTxLength(ctrlLen);
			dma_unmap_single(NULL, dmaBuf, len, DMA_TO_DEVICE);
			STAR_MSG(STAR_DBG, "star_stop - get tx descriptor idx:%d for skb 0x%08x\n", retval, extBuf);
			dev_kfree_skb((struct sk_buff *)extBuf);
		}
	} while (retval >= 0);

	do { /* Free Rx descriptor */
		u32 extBuf;
		dma_addr_t dmaBuf;

		retval = StarDmaRxGet(starDev, &dmaBuf, NULL, &extBuf);
		if (retval >= 0 && extBuf != 0)
		{
			dma_unmap_single(NULL, dmaBuf, skb_tailroom((struct sk_buff *)extBuf), DMA_FROM_DEVICE);
			STAR_MSG(STAR_VERB, "star-stop - get rx descriptor idx:%d for skb 0x%08x\n", retval, extBuf);
			dev_kfree_skb((struct sk_buff *)extBuf);
		}
	} while (retval >= 0);
    
    /* Stop RX FIFO receive */
    StarNICPdSet(starDev, 1);

    fgStarOpen = FALSE ;
	return 0;
}

static int star_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	dma_addr_t dmaBuf;
	unsigned long flags;
	int retval;

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	/* If frame size > Max frame size, drop this packet */
	if (skb->len > ETH_MAX_FRAME_SIZE)
	{
		STAR_MSG(STAR_WARN, "start_xmit(%s) - Tx frame length is oversized: %d bytes\n", dev->name, skb->len);
		dev_kfree_skb(skb);
		starDev->stats.tx_dropped ++;
		return 0;
	}

#ifdef FASTPATH
	fastpath_out(FASTPATH_ID, skb);
#endif

	//STAR_MSG(STAR_VERB, "hard_start_xmit\n");

	/* ?????? force to send or return NETDEV_TX_BUSY */

	dmaBuf = dma_map_single(NULL, skb->data, skb->len, DMA_TO_DEVICE);
	
	spin_lock_irqsave(&starPrv->lock, flags);
	
	retval = StarDmaTxSet(starDev, dmaBuf, skb->len, (u32)skb);
	if (starDev->txNum == starDev->txRingSize) /* Tx descriptor ring full */
	{
	    //STAR_MSG(STAR_WARN, "Tx descriptor full\n");
		netif_stop_queue(dev);
	}

	spin_unlock_irqrestore(&starPrv->lock, flags);

	StarDmaTxResume(starDev);
	dev->trans_start = jiffies;

	return 0;
}

static struct net_device_stats *star_get_stats(struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
  
	STAR_MSG(STAR_VERB, "get_stats\n");

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	return &starDev->stats;
}

static void star_set_multicast_list(struct net_device *dev)
{
#define STAR_HTABLE_SIZE		(512)
	unsigned long flags;

	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
  
	STAR_MSG(STAR_VERB, "star_set_multicast_list\n");

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	spin_lock_irqsave(&star_lock, flags);

	if (dev->flags & IFF_PROMISC)
	{
		STAR_MSG(STAR_WARN, "%s: Promiscuous mode enabled.\n", dev->name);
		StarArlPromiscEnable(starDev);
	} 
    else if ((netdev_mc_count(dev) > multicast_filter_limit) || (dev->flags & IFF_ALLMULTI))
	{
		u32 hashIdx;
		for (hashIdx = 0; hashIdx < STAR_HTABLE_SIZE; hashIdx ++)
		{
			StarSetHashBit(starDev, hashIdx, 1);
		}
	} 
    else
	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
		struct netdev_hw_addr *ha;
		netdev_for_each_mc_addr(ha, dev) {
			u32 hashAddr;
			hashAddr = (u32)(((ha->addr[0] & 0x1) << 8) + (u32)(ha->addr[5]));
			StarSetHashBit(starDev, hashAddr, 1);
		}
#else // < linux kernel 2.6.34
		u32 mcIdx;
		struct dev_mc_list *mclist;
		for (mcIdx = 0, mclist = dev->mc_list; mclist && mcIdx < netdev_mc_count(dev); mcIdx++, mclist = mclist->next)
		{
			u32 hashAddr;
			hashAddr = (u32)(((mclist->dmi_addr[0] & 0x1) << 8) + (u32)(mclist->dmi_addr[5]));
			StarSetHashBit(starDev, hashAddr, 1);
		}
#endif
	}

	spin_unlock_irqrestore(&star_lock, flags);
}

static int star_ioctl(struct net_device *dev, struct ifreq *req, int cmd)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	unsigned long flags;
	int rc=0;
    struct ioctl_eth_mac_cmd *mac_cmd= NULL;
	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	if (!netif_running(dev))
		return -EINVAL;

	if (StarIsASICMode(starDev))
	{
		if (StarPhyMode(starDev) != INT_SWITCH_MODE)
		{
			if (starPrv->mii.phy_id == 32)
				return -EINVAL;
		}
	} 
    else
	{
		if (starPrv->mii.phy_id == 32)
			return -EINVAL;
	}

    switch (cmd)
    {
    	
    	case SIOC_WRITE_MAC_ADDR_CMD:
    	break;
    	
    	case SIOC_THROUGHPUT_GET_CMD:
    	
    	break;
    	
    	case SIOC_ETH_MAC_CMD:
    	 mac_cmd = (struct ioctl_eth_mac_cmd *)&req->ifr_data;
    	
         if(mac_cmd->eth_cmd == ETH_MAC_REG_READ)
         {
         	
         }
         else if(mac_cmd->eth_cmd == ETH_MAC_REG_READ)	
         {
         	
         }	
    	 else if(mac_cmd->eth_cmd == ETH_MAC_REG_WRITE)	
    	 {
    	 }	
    	 else if(mac_cmd->eth_cmd == ETH_MAC_TX_DESCRIPTOR_READ)	
    	 {
    	 	
    	 }	
    	 else if(mac_cmd->eth_cmd == ETH_MAC_RX_DESCRIPTOR_READ)	
    	 {
    	 	
    	 }	
    	 else if(mac_cmd->eth_cmd == ETH_MAC_UP_DOWN_QUEUE)	
    	 {
    	   star_if_queue_up_down(&mac_cmd->ifr_ifru.up_down_queue);	
    	 	
    	 }	
       	
        rc = 0; 
    	break;
    	
    	
        case SIOC_MDIO_CMD: /* for linux ethernet diag command */
        {
            struct ioctl_mdio_cmd *mdio_cmd = (struct ioctl_mdio_cmd *)&req->ifr_data;
            if (mdio_cmd->wr_cmd)
            {
                StarMdcMdioWrite(starDev, starPrv->mii.phy_id, mdio_cmd->reg, mdio_cmd->val);
            }
            else
            {
                mdio_cmd->val = StarMdcMdioRead(starDev, starPrv->mii.phy_id, mdio_cmd->reg);
            }
        }
            rc = 0;
            break;

		case SIOC_PHY_CTRL_CMD: /* for linux ethernet diag command */
        {
            struct ioctl_phy_ctrl_cmd *pc_cmd = (struct ioctl_phy_ctrl_cmd *)&req->ifr_data;
            StarDisablePortCHGInt(starDev);
            StarPhyDisableAtPoll(starDev);
			if (pc_cmd->wr_cmd)
            {
                switch(pc_cmd->Prm)
                {
                 case ETH_PHY_DACAMP_CTRL:  //100M Amp
				 	#ifdef CC_ETHERNET_6896
					STAR_MSG(STAR_ERR, "vSetDACAmp(%d) \n",pc_cmd->val);
				 	vSetDACAmp(starDev,pc_cmd->val);
				 	#endif
				 	break;
					
				 case ETH_PHY_10MAMP_CTRL:
				 	STAR_MSG(STAR_ERR, "vSet10MAmp(%d) \n",pc_cmd->val);
				 	vStarSet10MAmp(starDev,pc_cmd->val);
				 	break;

				 case ETH_PHY_IN_BIAS_CTRL:
				 	STAR_MSG(STAR_ERR, "vSetInputBias(%d) \n",pc_cmd->val);		
				   	vStarSetInputBias(starDev,pc_cmd->val);	 
					break;
				   
				 case ETH_PHY_OUT_BIAS_CTRL:
				 	STAR_MSG(STAR_ERR, "vStarSetOutputBias(%d) \n",pc_cmd->val);
					 vStarSetOutputBias(starDev,pc_cmd->val); 	 
				   break;

				 case ETH_PHY_FEDBAK_CAP_CTRL:
				 	STAR_MSG(STAR_ERR, "vSetFeedbackCap(%d) \n",pc_cmd->val);	 
					 vStarSetFeedBackCap(starDev,pc_cmd->val);
				 	break;

				 case ETH_PHY_SLEW_RATE_CTRL:
				 	STAR_MSG(STAR_ERR, "vSetSlewRate(%d) \n",pc_cmd->val);
					 vStarSetSlewRate(starDev,pc_cmd->val);
				   break;
				   
				 case ETH_PHY_EYE_OPEN_CTRL:
				 	#ifdef CC_ETHERNET_6896
					 STAR_MSG(STAR_ERR, "MT8560 do not have this setting \n");
					#endif 
				   break;

				 case ETH_PHY_BW_50P_CTRL:
				 	STAR_MSG(STAR_ERR, "vSet50percentBW(%d) \n",pc_cmd->val);
					 vStarSet50PercentBW(starDev,pc_cmd->val); 	 
				  break;
				  
                 default:
                  STAR_MSG(STAR_ERR, "set nothing \n");
					break;
					
                }

            }
            else
            {
				switch(pc_cmd->Prm)
				 {
				  case ETH_PHY_DACAMP_CTRL:  //100M Amp
					 #ifdef CC_ETHERNET_6896
					 STAR_MSG(STAR_ERR, "vGetDACAmp() \n");
					 vGetDACAmp(starDev,&(pc_cmd->val));
					 #endif
					 break;
					 
				  case ETH_PHY_10MAMP_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGet10MAmp() \n");
					 vStarGet10MAmp(starDev,&(pc_cmd->val));
					 break;
				
				  case ETH_PHY_IN_BIAS_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGetInputBias() \n");	 
					 vStarGetInputBias(starDev,&(pc_cmd->val));  
					 break;
					
				  case ETH_PHY_OUT_BIAS_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGetOutputBias() \n");
					  vStarGetOutputBias(starDev,&(pc_cmd->val));	  
					break;
				
				  case ETH_PHY_FEDBAK_CAP_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGetFeedBackCap() \n");	  
					  vStarGetFeedBackCap(starDev,&(pc_cmd->val));
					 break;
				
				  case ETH_PHY_SLEW_RATE_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGetSlewRate() \n");
					  vStarGetSlewRate(starDev,&(pc_cmd->val));
					break;
					
				  case ETH_PHY_EYE_OPEN_CTRL:
	            #ifdef CC_ETHERNET_6896
					  STAR_MSG(STAR_ERR, "MT8560 do not have this setting \n");
	            #endif 
					break;
				
				  case ETH_PHY_BW_50P_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGet50PercentBW() \n");
					  vStarGet50PercentBW(starDev,&(pc_cmd->val));   
				   break;
				   
				  default:
				   STAR_MSG(STAR_ERR, "Get nothing \n");
					 break;
					 
				 }

            }
			 StarPhyEnableAtPoll(starDev);
			 StarIntrClear(starDev, STAR_INT_STA_PORTCHANGE);
             StarEnablePortCHGInt(starDev);
            rc = 0;
		}
        break;

        default:
			spin_lock_irqsave(&star_lock, flags);
			rc = generic_mii_ioctl(&starPrv->mii, if_mii(req), cmd, NULL);
			spin_unlock_irqrestore(&star_lock, flags);
            break;
    }

	return rc;
}

static int mdcMdio_read(struct net_device *dev, int phy_id, int location)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;

	starPrv = netdev_priv(dev);  
	starDev = &starPrv->stardev;

    return (StarMdcMdioRead(starDev, phy_id, location));
}

static void mdcMdio_write(struct net_device *dev, int phy_id, int location, int val)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;

	starPrv = netdev_priv(dev);  
	starDev = &starPrv->stardev;


    StarMdcMdioWrite(starDev, phy_id, location, val);
}

const struct net_device_ops star_netdev_ops = {
	.ndo_open		= star_open,
	.ndo_stop		= star_stop,
	.ndo_start_xmit		= star_start_xmit,
	.ndo_get_stats 		= star_get_stats,
	.ndo_set_multicast_list = star_set_multicast_list,
    .ndo_do_ioctl           = star_ioctl,
#ifdef CONFIG_NET_POLL_CONTROLLER
        .ndo_poll_controller	= star_netpoll,
#endif
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_set_mac_address 	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
};

#ifdef STARMAC_SUPPORT_ETHTOOL
static int starmac_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
        StarPrivate *starPrv = NULL;
        int rc;
        unsigned long flags;

        if((dev == NULL)||(cmd == NULL))
        {
                return -1;
        }

        starPrv = netdev_priv(dev);
        spin_lock_irqsave(&starPrv->lock, flags);
        rc = mii_ethtool_gset(&starPrv->mii, cmd);
        spin_unlock_irqrestore(&starPrv->lock, flags);

        return rc;
}

static int starmac_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
        StarPrivate *starPrv = NULL;
        int rc;
        unsigned long flags;
        StarDev *stardev = NULL;

        if((dev == NULL)||(cmd == NULL))
        {
                return -1;
        }

        starPrv = netdev_priv(dev);
        stardev = &(starPrv->stardev);
        spin_lock_irqsave(&starPrv->lock, flags);
        rc = mii_ethtool_sset(&starPrv->mii, cmd);
        if (cmd->autoneg == 0)
        {
                /* set speed and duplex to force mode */
                //stardev->full_duplex = cmd->duplex;
                //stardev->speed_100 = (cmd->speed == SPEED_100) ? 1:0;
                // MmacMacReset(mmac);
        }
        spin_unlock_irqrestore(&starPrv->lock, flags);

        return rc;
}

static int starmac_nway_reset(struct net_device *dev)
{
        StarPrivate *starPrv = NULL;
        int rc;
        unsigned long flags;


        if(dev == NULL)
        {
                return -1;
        }

        starPrv = netdev_priv(dev);
        spin_lock_irqsave(&starPrv->lock, flags);
        rc = mii_nway_restart(&starPrv->mii);
        spin_unlock_irqrestore(&starPrv->lock, flags);
        return rc;
}

static u32 starmac_get_link(struct net_device *dev)
{
        StarPrivate *starPrv = NULL;
        u32 rc;
        unsigned long flags;

        if(dev == NULL)
        {
                return 1;
        }

        starPrv = netdev_priv(dev);
        spin_lock_irqsave(&starPrv->lock, flags);
        rc = mii_link_ok(&starPrv->mii);
        spin_unlock_irqrestore(&starPrv->lock, flags);
        STAR_MSG(STAR_DBG, "ETHTOOL_TEST is called\n");
        return rc;
}

static struct ethtool_ops starmac_ethtool_ops = {
        .get_settings           = starmac_get_settings,
        .set_settings           = starmac_set_settings,
        .nway_reset             = starmac_nway_reset,
        .get_link               = starmac_get_link,
};
#endif

#ifdef CONFIG_OPM
static  int star_suspend(struct device *dev, pm_message_t state)
{	
#if 0
    u32 u4Reg;
    if(fgStarOpen == FALSE) return 0 ;
	STAR_MSG(STAR_DBG, "%s entered\n", __FUNCTION__);
    if(this_device!=NULL)
    {
        #if defined (CONFIG_ARCH_MT5398) || defined (CONFIG_ARCH_MT5880)
        star_stop(this_device);
        u4Reg = StarGetReg(ipll_base1+0x164);
        u4Reg = u4Reg | (0x1 <<1);
        StarSetReg(ipll_base1+0x164, u4Reg);
        u4Reg = StarGetReg(eth_base+0x308);
        u4Reg = u4Reg & (~0x83);
        StarSetReg(eth_base+0x308, u4Reg);
        #ifdef SUPPORT_ETH_POWERDOWN
        Ethernet_suspend();
        #endif
        #endif
    }
#endif
	return 0;
}	
static  int star_resume(struct device *dev)
{
#if 0
    u32 u4Reg;
    if(fgStarOpen == TRUE) return 0;
	STAR_MSG(STAR_DBG, "%s entered\n", __FUNCTION__);
	if(this_device!=NULL)
	{
		StarPrivate *starPrv = NULL;
	    StarDev *starDev = NULL;
		starPrv = netdev_priv(this_device);
		starDev = &starPrv->stardev;
		
		#if defined (CONFIG_ARCH_MT5398) || defined (CONFIG_ARCH_MT5880)
        u4Reg = StarGetReg(ipll_base1+0x164);
        u4Reg = u4Reg & (~(0x1 <<1));
        StarSetReg(ipll_base1+0x164, u4Reg);
        u4Reg = StarGetReg(eth_base+0x308);
        u4Reg = u4Reg | 0x80;
        StarSetReg(eth_base+0x308, u4Reg);
        msleep(1);
        u4Reg = u4Reg | 0x3;
        StarSetReg(eth_base+0x308, u4Reg);
		StarHwInit(starDev);
	    star_open(this_device);
        #ifdef SUPPORT_ETH_POWERDOWN
        Ethernet_resume();
        #endif
		#endif
	}	
#endif
	return 0;
}
#endif

#ifdef SUPPORT_ETH_POWERDOWN	 
int Ethernet_suspend(void)
{	
    STAR_MSG(STAR_ERR,"star:suspend\n");
	
#if defined (CONFIG_ARCH_MT5398) || defined (CONFIG_ARCH_MT5880)

	if(this_device!=NULL )
	{
	 	StarPrivate *starPrv = NULL;
	    StarDev *starDev = NULL;
		starPrv = netdev_priv(this_device);

		//starPrv = netdev_priv(dev);  
		starDev = &starPrv->stardev;

        StarPhyDisableAtPoll(starDev);
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1f, 0x2a30);  //test page
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x00, 0x00);  
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x01, 0x0080); 
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x02, 0x0000); 
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)&(~(1<<13)));//PL_BIAS_CTRL_MODE 
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)|((1<<11)));// 
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)&(~(1<<5)));//

		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x13, StarMdcMdioRead(starDev, starPrv->phy_addr,0x13)|((1<<6)));// PLL power down
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x14, StarMdcMdioRead(starDev, starPrv->phy_addr,0x14)|((1<<4)|(1<<5)));// MDI power down
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x16, StarMdcMdioRead(starDev, starPrv->phy_addr,0x16)|((1<<14)|(1<<13)));// BG power down
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1F, 0x00);  //main page
		//StarSetBit(STAR_PHY_CTRL1(dev->base),SWH_CK_PWN | INTER_PYH_PD);
		//StarSetBit(STAR_MAC_CFG(dev->base), NIC_PD);
		StarPhyEnableAtPoll(starDev);

		//N_Mask(0x0304,(0<<19),(1<<19) ) ;  ///disable phy clock
		//N_Mask(0x0300,(0<<7),(1<<7) ) ;   ///disable mac clock
		fgNetHWActive = 0;

	}
	
#endif

	return 0;
}	

int Ethernet_resume(void)
{
	STAR_MSG(STAR_ERR,"star:resume\n");
	
#if defined (CONFIG_ARCH_MT5398) || defined (CONFIG_ARCH_MT5880)
	if( this_device!=NULL && fgNetHWActive == 0)
	{
		StarPrivate *starPrv = NULL;
		StarDev *starDev = NULL;

		//struct net_device *star_device = dev_get_drvdata(dev) ;
		starPrv = netdev_priv(this_device);
		starDev = &starPrv->stardev;

		StarPhyDisableAtPoll(starDev);
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1f, 0x2a30);  //test page
		if(StarMdcMdioRead(starDev, starPrv->phy_addr, 0x01)!= 0x0000)
		{
		   
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x00, 0x00);  
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x01, 0x0000); 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x02, 0x0000); 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)|((1<<13)));//PL_BIAS_CTRL_MODE 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)&(~(1<<11)));// 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)|((1<<5)));//
		
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x13, StarMdcMdioRead(starDev, starPrv->phy_addr,0x13)&(~(1<<6)));// PLL power down
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x14, StarMdcMdioRead(starDev, starPrv->phy_addr,0x14)&(~((1<<4)|(1<<5))));// MDI power down
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x16, StarMdcMdioRead(starDev, starPrv->phy_addr,0x16)&(~((1<<14)|(1<<13))));// BG power down
		}
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1F, 0x00);  //main page
        StarPhyEnableAtPoll(starDev);
		//StarIntPhyInit(starDev);

        fgNetHWActive = 1 ;
			//StarClearBit(STAR_PHY_CTRL1(dev->base),SWH_CK_PWN | INTER_PYH_PD);
			//StarClearBit(STAR_MAC_CFG(dev->base), NIC_PD);
       
	}
#endif
	
	return 0;
}
#endif//#ifdef SUPPORT_ETH_POWERDOWN


int star_init(void)
{
	int result = 0;
	u8 macAddr[ETH_ADDR_LEN] = DEFAULT_MAC_ADDRESS;
    StarPrivate *starPrv = NULL;
	StarDev *starDev;
#ifdef CONFIG_OPM	
    struct device *dev; 
#endif

    STAR_MSG(STAR_VERB, "%s entered\n", __FUNCTION__);
	this_device = alloc_etherdev(sizeof(StarPrivate));
	if (!this_device)
	{
		result = -ENOMEM;
		goto out;
	}

	/* Base Register of  Ethernet */
        /* DO NOT use ioremap to mapping ethernet hardware address */
	eth_base = (u32)ETH_BASE; 
	pinmux_base = (u32)PINMUX_BASE;
	ipll_base1 = (u32)IPLL1_BASE; 
	
#if 0
	/* PDWNC base Register */
	pdwnc_base = (u32)(ioremap_nocache(PDWNC_BASE+PDWNC_BASE_CFG_OFFSET, 320)); 
	if (!pdwnc_base)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "pdwnc_base=0x%08x\n", pdwnc_base);

	/* Ethernet PDWNC Register */
	eth_pdwnc_base = (u32)(ioremap_nocache(ETHERNET_PDWNC_BASE, 320)); 
	if (!eth_pdwnc_base)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "eth_pdwnc_base=0x%08x\n", eth_pdwnc_base);

	/* Ethernet checksum Register */
	eth_chksum_base = (u32)(ioremap_nocache(ETH_CHKSUM_BASE, 320)); 
	if (!eth_chksum_base)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "eth_chksum_base=0x%08x\n", eth_chksum_base);

	

	/* IPLL2 Register */
	ipll_base2 = (u32)(ioremap_nocache(IPLL2_BASE, 320)); 
	if (!ipll_base2)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "ipll_base2=0x%08x\n", ipll_base2);

    bsp_base = (u32)(ioremap_nocache(BSP_BASE, 16));
    if (!bsp_base)
    {
        result = -ENOMEM;
        goto out;
    }
	STAR_MSG(STAR_DBG, "bsp_base=0x%08x\n", bsp_base);
#endif

  	starPrv = netdev_priv(this_device);
	memset(starPrv, 0, sizeof(StarPrivate));
	starPrv->dev = this_device;

#if defined (USE_TX_TASKLET) || defined (USE_RX_TASKLET)
	tasklet_init(&starPrv->dsr, star_dsr, (unsigned long)this_device);
#endif

    /* Init system locks */
	spin_lock_init(&starPrv->lock);
    spin_lock_init(&star_lock);
    spin_lock_init(&starPrv->tsk_lock);

	starPrv->desc_virAddr = (u32)dma_alloc_coherent(
                                    NULL, 
                                    TX_DESC_TOTAL_SIZE + RX_DESC_TOTAL_SIZE, 
                                    &(starPrv->desc_dmaAddr), 
                                    GFP_KERNEL);
	if (!starPrv->desc_virAddr)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "desc addr: 0x%08x(virtual)/0x%08x(physical)\n", 
             starPrv->desc_virAddr, 
             starPrv->desc_dmaAddr);
    
  	starDev = &starPrv->stardev;
	StarDevInit(starDev, eth_base);
	starDev->pinmuxBase = pinmux_base;
	starDev->ethIPLL1Base = ipll_base1;

    StarInitPrepare(starDev, macAddr);
	#if 0
    starDev->pdwncBase = pdwnc_base;
    starDev->pinmuxBase = pinmux_base;
    starDev->ethPdwncBase = eth_pdwnc_base;
    starDev->ethChksumBase = eth_chksum_base;
    starDev->ethIPLL1Base = ipll_base1;
    starDev->ethIPLL2Base = ipll_base2;
    starDev->bspBase = bsp_base;
    #endif
    
    starDev->starPrv = starPrv;

    /* Init hw related settings (eg. pinmux, clock ...etc) */
    if (StarHwInit(starDev) != 0)
    {
        STAR_MSG(STAR_ERR, "Ethernet MAC HW init fail!\n");
        result = -ENODEV;
        goto out;
    }

    StarNICPdSet(starDev , 1);

	starPrv->mii.dev = this_device;
	starPrv->mii.mdio_read = mdcMdio_read;
	starPrv->mii.mdio_write = mdcMdio_write;
	starPrv->mii.phy_id_mask = 0x1f;
	starPrv->mii.reg_num_mask = 0x1f;
	

    this_device->addr_len = ETH_ADDR_LEN;
	memcpy(this_device->dev_addr, macAddr, this_device->addr_len);       /* Set MAC address */
	this_device->irq = ETH_IRQ;
	this_device->base_addr = eth_base;
	this_device->netdev_ops = &star_netdev_ops;
#ifdef STARMAC_SUPPORT_ETHTOOL
    STAR_MSG(STAR_DBG, "EthTool installed\n");
    this_device->ethtool_ops = &starmac_ethtool_ops;
#endif
#ifdef USE_RX_NAPI
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	netif_napi_add(this_device, &starPrv->napi, star_poll, 64);
#else
	this_device->poll = star_poll;
	this_device->weight = 64;
#endif
#endif

	/* Check Ethernet Hardware is star or not by AHB Burst Type Register */
	if (StarGetReg(STAR_AHB_BURSTTYPE(eth_base)) != 0x77)
	{
	    STAR_MSG(STAR_ERR, "Ethernet MAC HW is unknown!\n");
		result = -EFAULT;
		goto out;
	}

	if ((result = register_netdev(this_device)) != 0)
	{
		unregister_netdev(this_device);
		goto out;
	}

#ifdef CONFIG_OPM	
	dev= &(this_device->dev);
	dev->class->suspend = star_suspend;
	dev->class->resume = star_resume;
#endif
    
    STAR_MSG(STAR_WARN, "Star MAC init done\n");
    skb_dbg = NULL;
    star_dev = starDev;

//    star_open(starPrv->dev);
//    star_stop(starPrv->dev);

	return 0;
	
out:

    if (starPrv->desc_virAddr)
    {
	    dma_free_coherent(NULL, 
                        TX_DESC_TOTAL_SIZE + RX_DESC_TOTAL_SIZE, 
                        (void *)starPrv->desc_virAddr, 
                        starPrv->desc_dmaAddr);
    }
#if 0    
    if (bsp_base)
    {
        iounmap((u32 *)bsp_base);
        bsp_base = 0;
    }
    
    if (ipll_base2)
    {
        iounmap((u32 *)ipll_base2);
        ipll_base2 = 0;
    }
    
    if (eth_chksum_base)
    {
        iounmap((u32 *)eth_chksum_base);
        eth_chksum_base = 0;
    }
    if (eth_pdwnc_base)
    {
        iounmap((u32 *)eth_pdwnc_base);
        eth_pdwnc_base = 0;
    }
    if (pdwnc_base)
    {
        iounmap((u32 *)pdwnc_base);
        pdwnc_base = 0;
    }
    
#endif    
    if (this_device)
    {
		unregister_netdev(this_device);
        free_netdev(this_device);
        this_device = NULL;
    }
    return result;
	
}

void star_cleanup(void)
{
	if (this_device)
	{
		StarPrivate *starPrv;

		starPrv = netdev_priv(this_device);

		unregister_netdev(this_device);
		dma_free_coherent(NULL, 
                          TX_DESC_TOTAL_SIZE + RX_DESC_TOTAL_SIZE, 
                          (void *)starPrv->desc_virAddr, 
                          starPrv->desc_dmaAddr);
#if 0                          
        if (bsp_base)
        {
            iounmap((u32 *)bsp_base);
            bsp_base = 0;
        }
        if (ipll_base2)
        {
            iounmap((u32 *)ipll_base2);
            ipll_base2 = 0;
        }
        
        if (eth_chksum_base)
        {
            iounmap((u32 *)eth_chksum_base);
            eth_chksum_base = 0;
        }
        if (eth_pdwnc_base)
        {
            iounmap((u32 *)eth_pdwnc_base);
            eth_pdwnc_base = 0;
        }
        if (pdwnc_base)
        {
            iounmap((u32 *)pdwnc_base);
            pdwnc_base = 0;
        }
        
#endif       
        if (ipll_base1)
        {
            iounmap((u32 *)ipll_base1);
            ipll_base1 = 0;
        }
        
        if (pinmux_base)
        {
            iounmap((u32 *)pinmux_base);
            pinmux_base = 0;
        }
         
        if (eth_base)
        {
            iounmap((u32 *)eth_base);
            eth_base = 0;
        }
		free_netdev(this_device);
	} 
    star_dev = NULL;
}

void star_if_up_down(int up)
{
    struct net_device * dev = this_device;

    if (!dev)
        return;
    if (up)
        star_open(dev);
    else
        star_stop(dev);
}


void star_if_queue_up_down(struct up_down_queue_par *par)
{
  struct net_device * dev = this_device;

  if(par->up)
  {
    STAR_MSG(STAR_DBG, "star_wake_queue(%s)\n", dev->name);
    netif_wake_queue(dev);
    	
  }	
  else
  {
    STAR_MSG(STAR_DBG, "star_stop_queue(%s)\n", dev->name);
    netif_stop_queue(dev);
    
  } 	
	
}

EXPORT_SYMBOL(star_if_up_down);
EXPORT_SYMBOL(star_cleanup);

module_init(star_init)
module_exit(star_cleanup)

#ifdef MODULE
MODULE_LICENSE("GPL");
#endif
