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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netdb.h>
#include <directfb.h>
#include <directfb_keyboard.h>

#define TAGNAME  "DFBNetEvt-R1"
struct netevnet_packet
{
	char tag[16]; /* should be TAGNAME*/
	DFBInputEvent evt;
};


int main (int argc, char *argv[])
{
   int sock,          /* socket descriptor */
       val,           /* scratch variable */
       cnt;           /* number of bytes I/O */

   char key;
   int mtk_sym=1; //mtk tv control key symbols
 
   struct sockaddr_in mtnet_addr;  /* Internet socket name (addr) */
   struct sockaddr_in *nptr;   /* pointer to get port number */
 
   struct netevnet_packet pkt;   /* I/O buffer, kind of small  */
 
   struct hostent *hp, *gethostbyaddr();

   printf("usage: mtnetkb [host|ip(127.0.0.1)] [port(3579)]\n");
 
   if (( sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
      printf("socket fail=%d\n", errno);
      perror("mtnetkb");
      exit(2);
   }
  
   if(argc<3)
	mtnet_addr.sin_port = htons(3579); //0xdfb
   else
	mtnet_addr.sin_port = htons( atoi(argv[2]) ); 
   mtnet_addr.sin_family = AF_INET;
   if(argc<2)
	hp = gethostbyname ("127.0.0.1");
   else
   	hp = gethostbyname ( argv[1] );
 
   if ( hp == NULL ) {
      printf("mtnetkb gethostbyname fail=%d\n", errno);
      perror("mtnetkb");
      close ( sock );
      exit(3);
   }
 
   bcopy ( hp -> h_addr_list[0], &mtnet_addr.sin_addr.s_addr,
           hp -> h_length );

   /* Establish socket connection with (remote) server.  */
 
   if ( ( connect ( sock, &mtnet_addr, sizeof(mtnet_addr) ) ) < 0 ) {
         printf("mtnetkb %s connect fail=%d\n", argv[0], errno);
         perror("mtnetkb");
         close (sock);
         exit(4);
      }
   
   while(1)
   {
another_key:
	bzero  ( &pkt,    sizeof( pkt) );
	strcpy ( pkt.tag, TAGNAME );
	pkt.evt.type       = DIET_KEYPRESS ;
	pkt.evt.flags      = DIEF_KEYSYMBOL; /*|DIEF_KEYCODE */;
     
   	printf("Enter key:\n");
	key=(char)getchar();
	switch(key)
	{
	case '?': mtk_sym^=1; 
	default: goto another_key;
	case 'Q': goto leave;
	
	case '0': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_0:DIKS_0;break;
	case '1': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_1:DIKS_1;break;
	case '2': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_2:DIKS_2;break;
	case '3': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_3:DIKS_3;break;
	case '4': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_4:DIKS_4;break;
	case '5': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_5:DIKS_5;break;
	case '6': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_6:DIKS_6;break;
	case '7': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_7:DIKS_7;break;
	case '8': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_8:DIKS_8;break;
	case '9': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_9:DIKS_9;break;	
	case '.': pkt.evt.key_symbol=mtk_sym?DFB_BTN_DIGIT_DOT:DIKS_PERIOD;break;
	case '>': pkt.evt.key_symbol=mtk_sym?DFB_BTN_VOL_UP:DIKS_GREATER_THAN_SIGN;break;
	case '<': pkt.evt.key_symbol=mtk_sym?DFB_BTN_VOL_DOWN:DIKS_LESS_THAN_SIGN;break;
	case '+': pkt.evt.key_symbol=mtk_sym?DFB_BTN_PRG_UP:DIKS_PLUS_SIGN;break;
	case '-': pkt.evt.key_symbol=mtk_sym?DFB_BTN_PRG_DOWN:DIKS_MINUS_SIGN;break;
	case '!':pkt.evt.key_symbol=mtk_sym?DFB_BTN_PREV_PRG:DIKS_PREVIOUS;break;		
/*	case '\r':
	case '\n': */
	case ' ': /* use space resent enter */
		pkt.evt.key_symbol=mtk_sym?DFB_BTN_ENTER:DIKS_RETURN;break;	
	case 'I': pkt.evt.key_symbol=mtk_sym?DFB_BTN_INPUT_SRC:DIKS_MODE;break;
	case 'P':pkt.evt.key_symbol=mtk_sym?DFB_BTN_POWER:DIKS_POWER;break;
	case 'C':pkt.evt.key_symbol=mtk_sym?DFB_BTN_CLEAR:DIKS_CLEAR;break;	
	case 'S': pkt.evt.key_symbol=mtk_sym?DFB_BTN_SELECT:DIKS_SELECT;break;
	case 'E': pkt.evt.key_symbol=mtk_sym?DFB_BTN_EXIT:DIKS_ESCAPE;break;	
	case 'N': pkt.evt.key_symbol=mtk_sym?DFB_BTN_RETURN:DIKS_BACKSPACE;break;
	case 'i': pkt.evt.key_symbol=mtk_sym?DFB_BTN_PRG_INFO:DIKS_INFO;break;
	case 'M': pkt.evt.key_symbol=mtk_sym?DFB_BTN_MENU:DIKS_MENU;break;
	case 'L': pkt.evt.key_symbol=mtk_sym?DFB_BTN_CURSOR_LEFT:DIKS_CURSOR_LEFT;break;
	case 'R': pkt.evt.key_symbol=mtk_sym?DFB_BTN_CURSOR_RIGHT:DIKS_CURSOR_RIGHT;break;
	case 'U': pkt.evt.key_symbol=mtk_sym?DFB_BTN_CURSOR_UP:DIKS_CURSOR_UP;break;
	case 'D': pkt.evt.key_symbol=mtk_sym?DFB_BTN_CURSOR_DOWN:DIKS_CURSOR_DOWN;break;
	}

	//sending 2 event, press + release	
   	write ( sock, &pkt, sizeof(pkt) ); //press
	pkt.evt.type       = DIET_KEYRELEASE ;
   	write ( sock, &pkt, sizeof(pkt) ); //release
   }
   
leave:
   write ( sock, pkt, 0 );
   close (sock);
   exit(0);
 
}
