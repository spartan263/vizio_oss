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
/*  ftgloadr.h                                                             */
/*                                                                         */
/*    The FreeType glyph loader (specification).                           */
/*                                                                         */
/*  Copyright 2002, 2003 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg                       */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __FTGLOADR_H__
#define __FTGLOADR_H__


#include <ft2build.h>
#include FT_FREETYPE_H


FT_BEGIN_HEADER


  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    FT_GlyphLoader                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The glyph loader is an internal object used to load several glyphs */
  /*    together (for example, in the case of composites).                 */
  /*                                                                       */
  /* <Note>                                                                */
  /*    The glyph loader implementation is not part of the high-level API, */
  /*    hence the forward structure declaration.                           */
  /*                                                                       */
  typedef struct FT_GlyphLoaderRec_*  FT_GlyphLoader ;


#define FT_SUBGLYPH_FLAG_ARGS_ARE_WORDS          1
#define FT_SUBGLYPH_FLAG_ARGS_ARE_XY_VALUES      2
#define FT_SUBGLYPH_FLAG_ROUND_XY_TO_GRID        4
#define FT_SUBGLYPH_FLAG_SCALE                   8
#define FT_SUBGLYPH_FLAG_XY_SCALE             0x40
#define FT_SUBGLYPH_FLAG_2X2                  0x80
#define FT_SUBGLYPH_FLAG_USE_MY_METRICS      0x200


  typedef struct  FT_SubGlyphRec_
  {
    FT_Int     index;
    FT_UShort  flags;
    FT_Int     arg1;
    FT_Int     arg2;
    FT_Matrix  transform;
    
  } FT_SubGlyphRec;


  typedef struct  FT_GlyphLoadRec_
  {
    FT_Outline   outline;       /* outline             */
    FT_Vector*   extra_points;  /* extra points table  */
    FT_UInt      num_subglyphs; /* number of subglyphs */
    FT_SubGlyph  subglyphs;     /* subglyphs           */

  } FT_GlyphLoadRec, *FT_GlyphLoad;


  typedef struct  FT_GlyphLoaderRec_
  {
    FT_Memory        memory;
    FT_UInt          max_points;
    FT_UInt          max_contours;
    FT_UInt          max_subglyphs;
    FT_Bool          use_extra;

    FT_GlyphLoadRec  base;
    FT_GlyphLoadRec  current;

    void*            other;            /* for possible future extension? */

  } FT_GlyphLoaderRec;


  /* create new empty glyph loader */
  FT_BASE( FT_Error )
  FT_GlyphLoader_New( FT_Memory        memory,
                      FT_GlyphLoader  *aloader );

  /* add an extra points table to a glyph loader */
  FT_BASE( FT_Error )
  FT_GlyphLoader_CreateExtra( FT_GlyphLoader  loader );

  /* destroy a glyph loader */
  FT_BASE( void )
  FT_GlyphLoader_Done( FT_GlyphLoader  loader );

  /* reset a glyph loader (frees everything int it) */
  FT_BASE( void )
  FT_GlyphLoader_Reset( FT_GlyphLoader  loader );

  /* rewind a glyph loader */
  FT_BASE( void )
  FT_GlyphLoader_Rewind( FT_GlyphLoader  loader );

  /* check that there is enough space to add `n_points' and `n_contours' */
  /* to the glyph loader                                                 */
  FT_BASE( FT_Error )
  FT_GlyphLoader_CheckPoints( FT_GlyphLoader  loader,
                              FT_UInt         n_points,
                              FT_UInt         n_contours );

  /* check that there is enough space to add `n_subs' sub-glyphs to */
  /* a glyph loader                                                 */
  FT_BASE( FT_Error )
  FT_GlyphLoader_CheckSubGlyphs( FT_GlyphLoader  loader,
                                 FT_UInt         n_subs );

  /* prepare a glyph loader, i.e. empty the current glyph */
  FT_BASE( void )
  FT_GlyphLoader_Prepare( FT_GlyphLoader  loader );

  /* add the current glyph to the base glyph */
  FT_BASE( void )
  FT_GlyphLoader_Add( FT_GlyphLoader  loader );

  /* copy points from one glyph loader to another */
  FT_BASE( FT_Error )
  FT_GlyphLoader_CopyPoints( FT_GlyphLoader  target,
                             FT_GlyphLoader  source );

 /* */


FT_END_HEADER

#endif /* __FTGLOADR_H__ */


/* END */
