/*
 * SDIO Host Controller Spec header file
 * Register map and definitions for the Standard Host Controller
 *
 * Copyright (C) 1999-2010, Broadcom Corporation
 * 
 *      Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2 (the "GPL"),
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the
 * following added to such license:
 * 
 *      As a special exception, the copyright holders of this software give you
 * permission to link this software with independent modules, and to copy and
 * distribute the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and conditions of
 * the license of that module.  An independent module is a module which is not
 * derived from this software.  The special exception does not apply to any
 * modifications of the software.
 * 
 *      Notwithstanding the above, under no circumstances may you combine this
 * software in any way with any other Broadcom software provided under a license
 * other than the GPL, without Broadcom's express prior written consent.
 *
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002200_2_001_2_001_1210_001/chiling/kernel/linux-3.0/drivers/net/wireless/bcm4329/include/sdioh.h#1 $
 */

#ifndef	_SDIOH_H
#define	_SDIOH_H

#define SD_SysAddr			0x000
#define SD_BlockSize			0x004
#define SD_BlockCount 			0x006
#define SD_Arg0				0x008
#define SD_Arg1 			0x00A
#define SD_TransferMode			0x00C
#define SD_Command 			0x00E
#define SD_Response0			0x010
#define SD_Response1 			0x012
#define SD_Response2			0x014
#define SD_Response3 			0x016
#define SD_Response4			0x018
#define SD_Response5 			0x01A
#define SD_Response6			0x01C
#define SD_Response7 			0x01E
#define SD_BufferDataPort0		0x020
#define SD_BufferDataPort1 		0x022
#define SD_PresentState			0x024
#define SD_HostCntrl			0x028
#define SD_PwrCntrl			0x029
#define SD_BlockGapCntrl 		0x02A
#define SD_WakeupCntrl 			0x02B
#define SD_ClockCntrl			0x02C
#define SD_TimeoutCntrl 		0x02E
#define SD_SoftwareReset		0x02F
#define SD_IntrStatus			0x030
#define SD_ErrorIntrStatus 		0x032
#define SD_IntrStatusEnable		0x034
#define SD_ErrorIntrStatusEnable 	0x036
#define SD_IntrSignalEnable		0x038
#define SD_ErrorIntrSignalEnable 	0x03A
#define SD_CMD12ErrorStatus		0x03C
#define SD_Capabilities			0x040
#define SD_Capabilities_Reserved	0x044
#define SD_MaxCurCap			0x048
#define SD_MaxCurCap_Reserved		0x04C
#define SD_ADMA_SysAddr			0x58
#define SD_SlotInterruptStatus		0x0FC
#define SD_HostControllerVersion 	0x0FE

/* SD specific registers in PCI config space */
#define SD_SlotInfo	0x40

/* SD_Capabilities reg (0x040) */
#define CAP_TO_CLKFREQ_M 	BITFIELD_MASK(6)
#define CAP_TO_CLKFREQ_S 	0
#define CAP_TO_CLKUNIT_M  	BITFIELD_MASK(1)
#define CAP_TO_CLKUNIT_S 	7
#define CAP_BASECLK_M 		BITFIELD_MASK(6)
#define CAP_BASECLK_S 		8
#define CAP_MAXBLOCK_M 		BITFIELD_MASK(2)
#define CAP_MAXBLOCK_S		16
#define CAP_ADMA2_M		BITFIELD_MASK(1)
#define CAP_ADMA2_S		19
#define CAP_ADMA1_M		BITFIELD_MASK(1)
#define CAP_ADMA1_S		20
#define CAP_HIGHSPEED_M		BITFIELD_MASK(1)
#define CAP_HIGHSPEED_S		21
#define CAP_DMA_M		BITFIELD_MASK(1)
#define CAP_DMA_S		22
#define CAP_SUSPEND_M		BITFIELD_MASK(1)
#define CAP_SUSPEND_S		23
#define CAP_VOLT_3_3_M		BITFIELD_MASK(1)
#define CAP_VOLT_3_3_S		24
#define CAP_VOLT_3_0_M		BITFIELD_MASK(1)
#define CAP_VOLT_3_0_S		25
#define CAP_VOLT_1_8_M		BITFIELD_MASK(1)
#define CAP_VOLT_1_8_S		26
#define CAP_64BIT_HOST_M	BITFIELD_MASK(1)
#define CAP_64BIT_HOST_S	28

/* SD_MaxCurCap reg (0x048) */
#define CAP_CURR_3_3_M		BITFIELD_MASK(8)
#define CAP_CURR_3_3_S		0
#define CAP_CURR_3_0_M		BITFIELD_MASK(8)
#define CAP_CURR_3_0_S		8
#define CAP_CURR_1_8_M		BITFIELD_MASK(8)
#define CAP_CURR_1_8_S		16

/* SD_SysAddr: Offset 0x0000, Size 4 bytes */

/* SD_BlockSize: Offset 0x004, Size 2 bytes */
#define BLKSZ_BLKSZ_M		BITFIELD_MASK(12)
#define BLKSZ_BLKSZ_S		0
#define BLKSZ_BNDRY_M		BITFIELD_MASK(3)
#define BLKSZ_BNDRY_S		12

/* SD_BlockCount: Offset 0x006, size 2 bytes */

/* SD_Arg0: Offset 0x008, size = 4 bytes  */
/* SD_TransferMode Offset 0x00C, size = 2 bytes */
#define XFER_DMA_ENABLE_M   	BITFIELD_MASK(1)
#define XFER_DMA_ENABLE_S	0
#define XFER_BLK_COUNT_EN_M 	BITFIELD_MASK(1)
#define XFER_BLK_COUNT_EN_S	1
#define XFER_CMD_12_EN_M    	BITFIELD_MASK(1)
#define XFER_CMD_12_EN_S 	2
#define XFER_DATA_DIRECTION_M	BITFIELD_MASK(1)
#define XFER_DATA_DIRECTION_S	4
#define XFER_MULTI_BLOCK_M	BITFIELD_MASK(1)
#define XFER_MULTI_BLOCK_S	5

/* SD_Command: Offset 0x00E, size = 2 bytes */
/* resp_type field */
#define RESP_TYPE_NONE 		0
#define RESP_TYPE_136  		1
#define RESP_TYPE_48   		2
#define RESP_TYPE_48_BUSY	3
/* type field */
#define CMD_TYPE_NORMAL		0
#define CMD_TYPE_SUSPEND	1
#define CMD_TYPE_RESUME		2
#define CMD_TYPE_ABORT		3

#define CMD_RESP_TYPE_M		BITFIELD_MASK(2)	/* Bits [0-1] 	- Response type */
#define CMD_RESP_TYPE_S		0
#define CMD_CRC_EN_M		BITFIELD_MASK(1)	/* Bit 3 	- CRC enable */
#define CMD_CRC_EN_S		3
#define CMD_INDEX_EN_M		BITFIELD_MASK(1)	/* Bit 4 	- Enable index checking */
#define CMD_INDEX_EN_S		4
#define CMD_DATA_EN_M		BITFIELD_MASK(1)	/* Bit 5 	- Using DAT line */
#define CMD_DATA_EN_S		5
#define CMD_TYPE_M		BITFIELD_MASK(2)	/* Bit [6-7] 	- Normal, abort, resume, etc
							 */
#define CMD_TYPE_S		6
#define CMD_INDEX_M		BITFIELD_MASK(6)	/* Bits [8-13] 	- Command number */
#define CMD_INDEX_S		8

/* SD_BufferDataPort0	: Offset 0x020, size = 2 or 4 bytes */
/* SD_BufferDataPort1 	: Offset 0x022, size = 2 bytes */
/* SD_PresentState	: Offset 0x024, size = 4 bytes */
#define PRES_CMD_INHIBIT_M	BITFIELD_MASK(1)	/* Bit 0	May use CMD */
#define PRES_CMD_INHIBIT_S	0
#define PRES_DAT_INHIBIT_M	BITFIELD_MASK(1)	/* Bit 1	May use DAT */
#define PRES_DAT_INHIBIT_S	1
#define PRES_DAT_BUSY_M		BITFIELD_MASK(1)	/* Bit 2	DAT is busy */
#define PRES_DAT_BUSY_S		2
#define PRES_PRESENT_RSVD_M	BITFIELD_MASK(5)	/* Bit [3-7]	rsvd */
#define PRES_PRESENT_RSVD_S	3
#define PRES_WRITE_ACTIVE_M	BITFIELD_MASK(1)	/* Bit 8	Write is active */
#define PRES_WRITE_ACTIVE_S	8
#define PRES_READ_ACTIVE_M	BITFIELD_MASK(1)	/* Bit 9	Read is active */
#define PRES_READ_ACTIVE_S	9
#define PRES_WRITE_DATA_RDY_M	BITFIELD_MASK(1)	/* Bit 10	Write buf is avail */
#define PRES_WRITE_DATA_RDY_S	10
#define PRES_READ_DATA_RDY_M	BITFIELD_MASK(1)	/* Bit 11	Read buf data avail */
#define PRES_READ_DATA_RDY_S	11
#define PRES_CARD_PRESENT_M	BITFIELD_MASK(1)	/* Bit 16	Card present - debounced */
#define PRES_CARD_PRESENT_S	16
#define PRES_CARD_STABLE_M	BITFIELD_MASK(1)	/* Bit 17	Debugging */
#define PRES_CARD_STABLE_S	17
#define PRES_CARD_PRESENT_RAW_M	BITFIELD_MASK(1)	/* Bit 18	Not debounced */
#define PRES_CARD_PRESENT_RAW_S	18
#define PRES_WRITE_ENABLED_M	BITFIELD_MASK(1)	/* Bit 19	Write protected? */
#define PRES_WRITE_ENABLED_S	19
#define PRES_DAT_SIGNAL_M	BITFIELD_MASK(4)	/* Bit [20-23]	Debugging */
#define PRES_DAT_SIGNAL_S	20
#define PRES_CMD_SIGNAL_M	BITFIELD_MASK(1)	/* Bit 24	Debugging */
#define PRES_CMD_SIGNAL_S	24

/* SD_HostCntrl: Offset 0x028, size = 1 bytes */
#define HOST_LED_M		BITFIELD_MASK(1)	/* Bit 0	LED On/Off */
#define HOST_LED_S		0
#define HOST_DATA_WIDTH_M	BITFIELD_MASK(1)	/* Bit 1	4 bit enable */
#define HOST_DATA_WIDTH_S	1
#define HOST_HI_SPEED_EN_M	BITFIELD_MASK(1)	/* Bit 2	High speed vs low speed */
#define HOST_DMA_SEL_S		3
#define HOST_DMA_SEL_M		BITFIELD_MASK(2)	/* Bit 4:3	DMA Select */
#define HOST_HI_SPEED_EN_S	2

/* misc defines */
#define SD1_MODE 		0x1	/* SD Host Cntrlr Spec */
#define SD4_MODE 		0x2	/* SD Host Cntrlr Spec */

/* SD_PwrCntrl: Offset 0x029, size = 1 bytes */
#define PWR_BUS_EN_M		BITFIELD_MASK(1)	/* Bit 0	Power the bus */
#define PWR_BUS_EN_S		0
#define PWR_VOLTS_M		BITFIELD_MASK(3)	/* Bit [1-3]	Voltage Select */
#define PWR_VOLTS_S		1

/* SD_SoftwareReset: Offset 0x02F, size = 1 byte */
#define SW_RESET_ALL_M		BITFIELD_MASK(1)	/* Bit 0	Reset All */
#define SW_RESET_ALL_S		0
#define SW_RESET_CMD_M		BITFIELD_MASK(1)	/* Bit 1	CMD Line Reset */
#define SW_RESET_CMD_S		1
#define SW_RESET_DAT_M		BITFIELD_MASK(1)	/* Bit 2	DAT Line Reset */
#define SW_RESET_DAT_S		2

/* SD_IntrStatus: Offset 0x030, size = 2 bytes */
/* Defs also serve SD_IntrStatusEnable and SD_IntrSignalEnable */
#define INTSTAT_CMD_COMPLETE_M		BITFIELD_MASK(1)	/* Bit 0 */
#define INTSTAT_CMD_COMPLETE_S		0
#define INTSTAT_XFER_COMPLETE_M		BITFIELD_MASK(1)
#define INTSTAT_XFER_COMPLETE_S		1
#define INTSTAT_BLOCK_GAP_EVENT_M	BITFIELD_MASK(1)
#define INTSTAT_BLOCK_GAP_EVENT_S	2
#define INTSTAT_DMA_INT_M		BITFIELD_MASK(1)
#define INTSTAT_DMA_INT_S		3
#define INTSTAT_BUF_WRITE_READY_M	BITFIELD_MASK(1)
#define INTSTAT_BUF_WRITE_READY_S	4
#define INTSTAT_BUF_READ_READY_M	BITFIELD_MASK(1)
#define INTSTAT_BUF_READ_READY_S	5
#define INTSTAT_CARD_INSERTION_M	BITFIELD_MASK(1)
#define INTSTAT_CARD_INSERTION_S	6
#define INTSTAT_CARD_REMOVAL_M		BITFIELD_MASK(1)
#define INTSTAT_CARD_REMOVAL_S		7
#define INTSTAT_CARD_INT_M		BITFIELD_MASK(1)
#define INTSTAT_CARD_INT_S		8
#define INTSTAT_ERROR_INT_M		BITFIELD_MASK(1)	/* Bit 15 */
#define INTSTAT_ERROR_INT_S		15

/* SD_ErrorIntrStatus: Offset 0x032, size = 2 bytes */
/* Defs also serve SD_ErrorIntrStatusEnable and SD_ErrorIntrSignalEnable */
#define ERRINT_CMD_TIMEOUT_M		BITFIELD_MASK(1)
#define ERRINT_CMD_TIMEOUT_S		0
#define ERRINT_CMD_CRC_M		BITFIELD_MASK(1)
#define ERRINT_CMD_CRC_S		1
#define ERRINT_CMD_ENDBIT_M		BITFIELD_MASK(1)
#define ERRINT_CMD_ENDBIT_S		2
#define ERRINT_CMD_INDEX_M		BITFIELD_MASK(1)
#define ERRINT_CMD_INDEX_S		3
#define ERRINT_DATA_TIMEOUT_M		BITFIELD_MASK(1)
#define ERRINT_DATA_TIMEOUT_S		4
#define ERRINT_DATA_CRC_M		BITFIELD_MASK(1)
#define ERRINT_DATA_CRC_S		5
#define ERRINT_DATA_ENDBIT_M		BITFIELD_MASK(1)
#define ERRINT_DATA_ENDBIT_S		6
#define ERRINT_CURRENT_LIMIT_M		BITFIELD_MASK(1)
#define ERRINT_CURRENT_LIMIT_S		7
#define ERRINT_AUTO_CMD12_M		BITFIELD_MASK(1)
#define ERRINT_AUTO_CMD12_S		8
#define ERRINT_VENDOR_M			BITFIELD_MASK(4)
#define ERRINT_VENDOR_S			12

/* Also provide definitions in "normal" form to allow combined masks */
#define ERRINT_CMD_TIMEOUT_BIT		0x0001
#define ERRINT_CMD_CRC_BIT		0x0002
#define ERRINT_CMD_ENDBIT_BIT		0x0004
#define ERRINT_CMD_INDEX_BIT		0x0008
#define ERRINT_DATA_TIMEOUT_BIT		0x0010
#define ERRINT_DATA_CRC_BIT		0x0020
#define ERRINT_DATA_ENDBIT_BIT		0x0040
#define ERRINT_CURRENT_LIMIT_BIT	0x0080
#define ERRINT_AUTO_CMD12_BIT		0x0100

/* Masks to select CMD vs. DATA errors */
#define ERRINT_CMD_ERRS		(ERRINT_CMD_TIMEOUT_BIT | ERRINT_CMD_CRC_BIT |\
				 ERRINT_CMD_ENDBIT_BIT | ERRINT_CMD_INDEX_BIT)
#define ERRINT_DATA_ERRS	(ERRINT_DATA_TIMEOUT_BIT | ERRINT_DATA_CRC_BIT |\
				 ERRINT_DATA_ENDBIT_BIT)
#define ERRINT_TRANSFER_ERRS	(ERRINT_CMD_ERRS | ERRINT_DATA_ERRS)

/* SD_WakeupCntr_BlockGapCntrl : Offset 0x02A , size = bytes */
/* SD_ClockCntrl	: Offset 0x02C , size = bytes */
/* SD_SoftwareReset_TimeoutCntrl 	: Offset 0x02E , size = bytes */
/* SD_IntrStatus	: Offset 0x030 , size = bytes */
/* SD_ErrorIntrStatus 	: Offset 0x032 , size = bytes */
/* SD_IntrStatusEnable	: Offset 0x034 , size = bytes */
/* SD_ErrorIntrStatusEnable : Offset 0x036 , size = bytes */
/* SD_IntrSignalEnable	: Offset 0x038 , size = bytes */
/* SD_ErrorIntrSignalEnable : Offset 0x03A , size = bytes */
/* SD_CMD12ErrorStatus	: Offset 0x03C , size = bytes */
/* SD_Capabilities	: Offset 0x040 , size = bytes */
/* SD_MaxCurCap		: Offset 0x048 , size = bytes */
/* SD_MaxCurCap_Reserved: Offset 0x04C , size = bytes */
/* SD_SlotInterruptStatus: Offset 0x0FC , size = bytes */
/* SD_HostControllerVersion : Offset 0x0FE , size = bytes */

#endif /* _SDIOH_H */
