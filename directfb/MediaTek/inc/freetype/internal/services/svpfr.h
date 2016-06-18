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
/*  svpfr.h                                                                */
/*                                                                         */
/*    Internal PFR service functions (specification).                      */
/*                                                                         */
/*  Copyright 2003 by                                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __SVPFR_H__
#define __SVPFR_H__

#include FT_INTERNAL_SERVICE_H


FT_BEGIN_HEADER


#define FT_SERVICE_ID_PFR_METRICS  "pfr-metrics"


  typedef FT_Error
  (*FT_PFR_GetMetricsFunc)( FT_Face    face,
                            FT_UInt   *aoutline,
                            FT_UInt   *ametrics,
                            FT_Fixed  *ax_scale,
                            FT_Fixed  *ay_scale );

  typedef FT_Error
  (*FT_PFR_GetKerningFunc)( FT_Face     face,
                            FT_UInt     left,
                            FT_UInt     right,
                            FT_Vector  *avector );

  typedef FT_Error
  (*FT_PFR_GetAdvanceFunc)( FT_Face   face,
                            FT_UInt   gindex,
                            FT_Pos   *aadvance );


  FT_DEFINE_SERVICE( PfrMetrics )
  {
    FT_PFR_GetMetricsFunc  get_metrics;
    FT_PFR_GetKerningFunc  get_kerning;
    FT_PFR_GetAdvanceFunc  get_advance;

  };

 /* */

FT_END_HEADER

#endif /* __SVPFR_H__ */


/* END */
