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
/*  svpostnm.h                                                             */
/*                                                                         */
/*    The FreeType PostScript name services (specification).               */
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


#ifndef __SVPOSTNM_H__
#define __SVPOSTNM_H__

#include FT_INTERNAL_SERVICE_H


FT_BEGIN_HEADER

  /*
   *  A trivial service used to retrieve the PostScript name of a given
   *  font when available.  The `get_name' field should never be NULL.
   *
   *  The correponding function can return NULL to indicate that the
   *  PostScript name is not available.
   *
   *  The name is owned by the face and will be destroyed with it.
   */

#define FT_SERVICE_ID_POSTSCRIPT_FONT_NAME  "postscript-font-name"


  typedef const char*
  (*FT_PsName_GetFunc)( FT_Face  face );


  FT_DEFINE_SERVICE( PsFontName )
  {
    FT_PsName_GetFunc  get_ps_font_name;
  };

  /* */


FT_END_HEADER


#endif /* __SVPOSTNM_H__ */


/* END */
