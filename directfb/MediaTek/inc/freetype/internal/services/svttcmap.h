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
/*  svsttcmap.h                                                            */
/*                                                                         */
/*    The FreeType TrueType/sfnt cmap extra information service.           */
/*                                                                         */
/*  Copyright 2003 by                                                      */
/*  Masatake YAMATO, Redhat K.K.                                           */
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

/* Development of this service is support of 
   Information-technology Promotion Agency, Japan. */

#ifndef __SVTTCMAP_H__
#define __SVTTCMAP_H__ 

#include FT_INTERNAL_SERVICE_H
#include FT_TRUETYPE_TABLES_H


FT_BEGIN_HEADER


#define FT_SERVICE_ID_TT_CMAP "tt-cmaps"


  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    TT_CMapInfo                                                        */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A structure used to store TrueType/sfnt specific cmap information  */
  /*    which is not covered by the generic @FT_CharMap structure.  This   */
  /*    structure can be accessed with the @FT_Get_TT_CMap_Info function.  */
  /*                                                                       */
  /* <Fields>                                                              */
  /*    language ::                                                        */
  /*      The language ID used in Mac fonts.  Definitions of values are in */
  /*      freetype/ttnameid.h.                                             */
  /*                                                                       */
  typedef struct  TT_CMapInfo_
  {
    FT_ULong language;

  } TT_CMapInfo;


  typedef FT_Error
  (*TT_CMap_Info_GetFunc)( FT_CharMap    charmap,
                           TT_CMapInfo  *cmap_info );


  FT_DEFINE_SERVICE( TTCMaps )
  {
    TT_CMap_Info_GetFunc  get_cmap_info;
  }; 
  
  /* */


FT_END_HEADER

#endif /* __SVTTCMAP_H__ */


/* END */
