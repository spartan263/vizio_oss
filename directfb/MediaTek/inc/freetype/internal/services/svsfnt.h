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
/*  svsfnt.h                                                               */
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


#ifndef __SVSFNT_H__
#define __SVSFNT_H__

#include FT_INTERNAL_SERVICE_H
#include FT_TRUETYPE_TABLES_H


FT_BEGIN_HEADER


  /*
   *  SFNT table loading service.
   */

#define FT_SERVICE_ID_SFNT_TABLE  "sfnt-table"


  /*
   * Used to implement FT_Load_Sfnt_Table().
   */
  typedef FT_Error
  (*FT_SFNT_TableLoadFunc)( FT_Face    face,
                            FT_ULong   tag,
                            FT_Long    offset,
                            FT_Byte*   buffer,
                            FT_ULong*  length );

  /*
   * Used to implement FT_Get_Sfnt_Table().
   */
  typedef void*
  (*FT_SFNT_TableGetFunc)( FT_Face      face,
                           FT_Sfnt_Tag  tag );

 
  FT_DEFINE_SERVICE( SFNT_Table )
  {
    FT_SFNT_TableLoadFunc  load_table;
    FT_SFNT_TableGetFunc   get_table;
  };

  /* */

 
FT_END_HEADER


#endif /* __SVSFNT_H__ */


/* END */
