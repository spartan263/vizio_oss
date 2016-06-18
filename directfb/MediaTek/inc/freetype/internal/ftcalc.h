/*----------------------------------------------------------------------------
 * No Warranty :  Except  as  may  be  otherwise  agreed  to  in  writing, no *
 * warranties  of  any  kind,  whether  express or  implied, are given by MTK *
 * with  respect  to  any  Confidential  Information  or any use thereof, and *
 * the  Confidential  Information  is  provided  on  an  "AS IS"  basis.  MTK *
 * hereby  expressly  disclaims  all  such  warranties, including any implied *
 * warranties   of  merchantability ,  non-infringement  and  fitness  for  a *
 * particular purpose and any warranties arising out of course of performance *
 * course  of dealing or usage of trade.  Parties further acknowledge that in *
 * connection  with  the Purpose, Company may, either presently and/or in the *
 * future,   instruct   MTK   to   assist  it  in  the  development  and  the *
 * implementation,  in accordance with Company's designs, of certain software *
 * relating  to  Company's  product(s)  (the  "Services").   Except as may be *
 * otherwise agreed to in writing, no warranties of any kind, whether express *
 * or  implied,  are  given by MTK with respect to the Services provided, and *
 * the  Services  are  provided  on  an  "AS  IS"  basis.   Company   further *
 * acknowledges  that  the  Services  may  contain  errors,  which testing is *
 * important  and  it  is  solely  responsible for fully testing the Services *
 * and/or   derivatives   thereof   before  they  are  used,  sublicensed  or *
 * distributed.   Should  there  be any third party action brought again MTK, *
 * arising  out  of  or  relating  to  the  Services,  Company agree to fully *
 * indemnify and hold MTK harmless.                                           *
 *---------------------------------------------------------------------------*/
/***************************************************************************/
/*                                                                         */
/*  ftcalc.h                                                               */
/*                                                                         */
/*    Arithmetic computations (specification).                             */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __FTCALC_H__
#define __FTCALC_H__


#include <ft2build.h>
#include FT_FREETYPE_H


FT_BEGIN_HEADER


  FT_EXPORT( FT_Int32 )
  FT_SqrtFixed( FT_Int32  x );


#define SQRT_32( x )  FT_Sqrt32( x )


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    FT_Sqrt32                                                          */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Computes the square root of an Int32 integer (which will be        */
  /*    handled as an unsigned long value).                                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    x :: The value to compute the root for.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The result of `sqrt(x)'.                                           */
  /*                                                                       */
  FT_EXPORT( FT_Int32 )
  FT_Sqrt32( FT_Int32  x );


  /*************************************************************************/
  /*                                                                       */
  /* FT_MulDiv() and FT_MulFix() are declared in freetype.h.               */
  /*                                                                       */
  /*************************************************************************/


#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER

  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    FT_MulDiv_No_Round                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A very simple function used to perform the computation `(a*b)/c'   */
  /*    (without rounding) with maximal accuracy (it uses a 64-bit         */
  /*    intermediate integer whenever necessary).                          */
  /*                                                                       */
  /*    This function isn't necessarily as fast as some processor specific */
  /*    operations, but is at least completely portable.                   */
  /*                                                                       */
  /* <Input>                                                               */
  /*    a :: The first multiplier.                                         */
  /*    b :: The second multiplier.                                        */
  /*    c :: The divisor.                                                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    The result of `(a*b)/c'.  This function never traps when trying to */
  /*    divide by zero; it simply returns `MaxInt' or `MinInt' depending   */
  /*    on the signs of `a' and `b'.                                       */
  /*                                                                       */
  FT_BASE( FT_Long )
  FT_MulDiv_No_Round( FT_Long  a,
                      FT_Long  b,
                      FT_Long  c );

#endif /* TT_CONFIG_OPTION_BYTECODE_INTERPRETER */


#define INT_TO_F26DOT6( x )    ( (FT_Long)(x) << 6  )
#define INT_TO_F2DOT14( x )    ( (FT_Long)(x) << 14 )
#define INT_TO_FIXED( x )      ( (FT_Long)(x) << 16 )
#define F2DOT14_TO_FIXED( x )  ( (FT_Long)(x) << 2  )
#define FLOAT_TO_FIXED( x )    ( (FT_Long)( x * 65536.0 ) )

#define ROUND_F26DOT6( x )     ( x >= 0 ? (    ( (x) + 32 ) & -64 )     \
                                        : ( -( ( 32 - (x) ) & -64 ) ) )


FT_END_HEADER

#endif /* __FTCALC_H__ */


/* END */
