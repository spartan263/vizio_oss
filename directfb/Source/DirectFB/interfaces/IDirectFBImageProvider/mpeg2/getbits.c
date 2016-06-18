/* getbits.c, bit level routines                                            */

/*
 * All modifications (mpeg2decode -> mpeg2play) are
 * Copyright (C) 1996, Stefan Eckart. All Rights Reserved.
 */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "global.h"

/* initialize buffer, call once before first getbits or showbits */

void
MPEG2_Initialize_Buffer(MPEG2_Decoder *dec)
{
     dec->Incnt = 0;
     dec->Rdptr = dec->Rdbfr + 2048;
     dec->Rdmax = dec->Rdptr;

     dec->Bfr = 0;

     MPEG2_Flush_Buffer(dec, 0); /* fills valid data into bfr */
}

void
MPEG2_Fill_Buffer(MPEG2_Decoder *dec)
{
     int Buffer_Level;

     Buffer_Level = dec->mpeg2_read (dec->Rdbfr, 2048, dec->mpeg2_read_ctx);
     dec->Rdptr = dec->Rdbfr;

     /* end of the bitstream file */
     if (Buffer_Level < 2048) {
          /* just to be safe */
          if (Buffer_Level < 0)
               Buffer_Level = 0;

          /* pad until the next to the next 32-bit word boundary */
          while (Buffer_Level & 3)
               dec->Rdbfr[Buffer_Level++] = 0;

          /* pad the buffer with sequence end codes */
          while (Buffer_Level < 2048) {
               dec->Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE>>24;
               dec->Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE>>16;
               dec->Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE>>8;
               dec->Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE&0xff;
          }
     }
}


/* return next n bits (right adjusted) without advancing */

unsigned int
MPEG2_Show_Bits(MPEG2_Decoder *dec, int N)
{
     return dec->Bfr >> (32-N);
}


/* return next bit (could be made faster than MPEG2_Get_Bits(1)) */

unsigned int
MPEG2_Get_Bits1(MPEG2_Decoder *dec)
{
     return MPEG2_Get_Bits(dec, 1);
}


/* advance by n bits */

void
MPEG2_Flush_Buffer(MPEG2_Decoder *dec, int N)
{
     int Incnt;

     dec->Bfr <<= N;

     Incnt = dec->Incnt -= N;

     if (Incnt <= 24) {
          if (dec->Rdptr < dec->Rdbfr+2044) {
               do {
                    dec->Bfr |= *dec->Rdptr++ << (24 - Incnt);
                    Incnt += 8;
               }
               while (Incnt <= 24);
          }
          else {
               do {
                    if (dec->Rdptr >= dec->Rdbfr+2048)
                         MPEG2_Fill_Buffer(dec);
                    dec->Bfr |= *dec->Rdptr++ << (24 - Incnt);
                    Incnt += 8;
               }
               while (Incnt <= 24);
          }
          dec->Incnt = Incnt;
     }
}


/* return next n bits (right adjusted) */

unsigned int
MPEG2_Get_Bits(MPEG2_Decoder *dec, int N)
{
     unsigned int Val;

     Val = MPEG2_Show_Bits(dec, N);
     MPEG2_Flush_Buffer(dec, N);

     return Val;
}

