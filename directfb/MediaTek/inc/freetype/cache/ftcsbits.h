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
/*  ftcsbits.h                                                             */
/*                                                                         */
/*    A small-bitmap cache (specification).                                */
/*                                                                         */
/*  Copyright 2000-2001, 2002, 2003 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __FTCSBITS_H__
#define __FTCSBITS_H__


#include <ft2build.h>
#include FT_CACHE_H
#include FT_CACHE_INTERNAL_GLYPH_H


FT_BEGIN_HEADER

#define FTC_SBIT_ITEMS_PER_NODE  16

  typedef struct  FTC_SNodeRec_
  {
    FTC_GNodeRec  gnode;
    FT_UInt       count;
    FTC_SBitRec   sbits[FTC_SBIT_ITEMS_PER_NODE];

  } FTC_SNodeRec, *FTC_SNode;


#define FTC_SNODE( x )         ( (FTC_SNode)( x ) )
#define FTC_SNODE_GINDEX( x )  FTC_GNODE( x )->gindex
#define FTC_SNODE_FAMILY( x )  FTC_GNODE( x )->family

  typedef FT_UInt
  (*FTC_SFamily_GetCountFunc)( FTC_Family   family,
                               FTC_Manager  manager );

  typedef FT_Error
  (*FTC_SFamily_LoadGlyphFunc)( FTC_Family   family,
                                FT_UInt      gindex,
                                FTC_Manager  manager,
                                FT_Face     *aface );

  typedef struct  FTC_SFamilyClassRec_
  {
    FTC_MruListClassRec        clazz;
    FTC_SFamily_GetCountFunc   family_get_count;
    FTC_SFamily_LoadGlyphFunc  family_load_glyph;

  } FTC_SFamilyClassRec;

  typedef const FTC_SFamilyClassRec*  FTC_SFamilyClass;

#define FTC_SFAMILY_CLASS( x )  ((FTC_SFamilyClass)(x))

#define FTC_CACHE__SFAMILY_CLASS( x )  \
          FTC_SFAMILY_CLASS( FTC_CACHE__GCACHE_CLASS( x )->family_class )


  FT_EXPORT( void )
  FTC_SNode_Free( FTC_SNode  snode,
                  FTC_Cache  cache );

  FT_EXPORT( FT_Error )
  FTC_SNode_New( FTC_SNode   *psnode,
                 FTC_GQuery   gquery,
                 FTC_Cache    cache );

  FT_EXPORT( FT_ULong )
  FTC_SNode_Weight( FTC_SNode  inode );


  FT_EXPORT( FT_Bool )
  FTC_SNode_Compare( FTC_SNode   snode,
                     FTC_GQuery  gquery,
                     FTC_Cache   cache );

  /* */

FT_END_HEADER

#endif /* __FTCSBITS_H__ */


/* END */
