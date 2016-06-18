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
/*  svbdf.h                                                                */
/*                                                                         */
/*    The FreeType BDF services (specification).                           */
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


#ifndef __SVBDF_H__
#define __SVBDF_H__

#include FT_BDF_H
#include FT_INTERNAL_SERVICE_H


FT_BEGIN_HEADER


#define FT_SERVICE_ID_BDF  "bdf"

  typedef FT_Error
  (*FT_BDF_GetCharsetIdFunc)( FT_Face       face,
                              const char*  *acharset_encoding,
                              const char*  *acharset_registry );

  typedef FT_Error
  (*FT_BDF_GetPropertyFunc)( FT_Face           face,
                             const char*       prop_name,
                             BDF_PropertyRec  *aproperty );


  FT_DEFINE_SERVICE( BDF )
  {
    FT_BDF_GetCharsetIdFunc  get_charset_id;
    FT_BDF_GetPropertyFunc   get_property;
  };

  /* */


FT_END_HEADER


#endif /* __SVBDF_H__ */


/* END */
