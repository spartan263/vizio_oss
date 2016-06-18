/* $Id: //DTV/MP_BR/DTV_X_IDTV0801_002200_2_001_2_001_1210_001/chiling/kernel/linux-3.0/drivers/isdn/hisax/isdnl1.h#1 $
 *
 * Layer 1 defines
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 *
 */

#define D_RCVBUFREADY	0
#define D_XMTBUFREADY	1
#define D_L1STATECHANGE	2
#define D_CLEARBUSY	3
#define D_RX_MON0	4
#define D_RX_MON1	5
#define D_TX_MON0	6
#define D_TX_MON1	7
#define E_RCVBUFREADY	8

#define B_RCVBUFREADY	0
#define B_XMTBUFREADY	1
#define B_ACKPENDING	2

__attribute__((format(printf, 2, 3)))
void debugl1(struct IsdnCardState *cs, char *fmt, ...);
void DChannel_proc_xmt(struct IsdnCardState *cs);
void DChannel_proc_rcv(struct IsdnCardState *cs);
void l1_msg(struct IsdnCardState *cs, int pr, void *arg);
void l1_msg_b(struct PStack *st, int pr, void *arg);
void Logl2Frame(struct IsdnCardState *cs, struct sk_buff *skb, char *buf,
		int dir);
void BChannel_bh(struct work_struct *work);
