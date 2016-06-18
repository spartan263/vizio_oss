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
/*  ftcimage.h                                                             */
/*                                                                         */
/*    FreeType Generic Image cache (specification)                         */
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


 /*
  *  FTC_ICache is an _abstract_ cache used to store a single FT_Glyph
  *  image per cache node.
  *
  *  FTC_ICache extends FTC_GCache.  For an implementation example,
  *  see FTC_ImageCache in `src/cache/ftbasic.c'.
  */
  

  /*************************************************************************/
  /*                                                                       */
  /* Each image cache really manages FT_Glyph objects.                     */
  /*                                                                       */
  /*************************************************************************/


#ifndef __FTCIMAGE_H__
#define __FTCIMAGE_H__


#include <ft2build.h>
#include FT_CACHE_H
#include FT_CACHE_INTERNAL_GLYPH_H

FT_BEGIN_HEADER


  /* the FT_Glyph image node type - we store only 1 glyph per node */
  typedef struct  FTC_INodeRec_
  {
    FTC_GNodeRec  gnode;
    FT_Glyph      glyph;

  } FTC_INodeRec, *FTC_INode;

#define FTC_INODE( x )         ( (FTC_INode)( x ) )
#define FTC_INODE_GINDEX( x )  FTC_GNODE(x)->gindex
#define FTC_INODE_FAMILY( x )  FTC_GNODE(x)->family

  typedef FT_Error
  (*FTC_IFamily_LoadGlyphFunc)( FTC_Family  family,
                                FT_UInt     gindex,
                                FTC_Cache   cache,
                                FT_Glyph   *aglyph );

  typedef struct  FTC_IFamilyClassRec_
  {
    FTC_MruListClassRec        clazz;
    FTC_IFamily_LoadGlyphFunc  family_load_glyph;

  } FTC_IFamilyClassRec;

  typedef const FTC_IFamilyClassRec*  FTC_IFamilyClass;

#define FTC_IFAMILY_CLASS( x )  ((FTC_IFamilyClass)(x))

#define FTC_CACHE__IFAMILY_CLASS( x ) \
          FTC_IFAMILY_CLASS( FTC_CACHE__GCACHE_CLASS(x)->family_class )


  /* can be used as a @FTC_Node_FreeFunc */
  FT_EXPORT( void )
  FTC_INode_Free( FTC_INode  inode,
                  FTC_Cache  cache );

  /* Can be used as @FTC_Node_NewFunc.  `gquery.index' and `gquery.family'
   * must be set correctly.  This function will call the `family_load_glyph'
   * method to load the FT_Glyph into the cache node.
   */
  FT_EXPORT( FT_Error )
  FTC_INode_New( FTC_INode   *pinode,
                 FTC_GQuery   gquery,
                 FTC_Cache    cache );

  /* can be used as @FTC_Node_WeightFunc */
  FT_EXPORT( FT_ULong )
  FTC_INode_Weight( FTC_INode  inode );

 /* */

FT_END_HEADER

#endif /* __FTCIMAGE_H__ */


/* END */
