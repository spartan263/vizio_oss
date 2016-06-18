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
/*  ftpfr.h                                                                */
/*                                                                         */
/*    FreeType API for accessing PFR-specific data (specification only).   */
/*                                                                         */
/*  Copyright 2002, 2003 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __FTPFR_H__
#define __FTPFR_H__

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif


FT_BEGIN_HEADER


  /*************************************************************************/
  /*                                                                       */
  /* <Section>                                                             */
  /*    pfr_fonts                                                          */
  /*                                                                       */
  /* <Title>                                                               */
  /*    PFR Fonts                                                          */
  /*                                                                       */
  /* <Abstract>                                                            */
  /*    PFR/TrueDoc specific APIs                                          */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This section contains the declaration of PFR-specific functions.   */
  /*                                                                       */
  /*************************************************************************/


 /**********************************************************************
  *
  * @function:
  *    FT_Get_PFR_Metrics
  *
  * @description:
  *    Return the outline and metrics resolutions of a given PFR face.
  *
  * @input:
  *    face :: Handle to the input face.  It can be a non-PFR face.
  *
  * @output:
  *    aoutline_resolution ::
  *      Outline resolution.  This is equivalent to `face->units_per_EM'.
  *      Optional (parameter can be NULL).
  *
  *    ametrics_resolution ::
  *      Metrics resolution.  This is equivalent to `outline_resolution'
  *      for non-PFR fonts.  Optional (parameter can be NULL).
  *
  *    ametrics_x_scale ::
  *      A 16.16 fixed-point number used to scale distance expressed
  *      in metrics units to device sub-pixels.  This is equivalent to
  *      `face->size->x_scale', but for metrics only.  Optional (parameter
  *      can be NULL)
  *
  *    ametrics_y_scale ::
  *      Same as `ametrics_x_scale' but for the vertical direction.
  *      optional (parameter can be NULL)
  *
  * @return:
  *    FreeType error code.  0 means success.
  *
  * @note:
  *   If the input face is not a PFR, this function will return an error.
  *   However, in all cases, it will return valid values.
  */
  FT_EXPORT( FT_Error )
  FT_Get_PFR_Metrics( FT_Face    face,
                      FT_UInt   *aoutline_resolution,
                      FT_UInt   *ametrics_resolution,
                      FT_Fixed  *ametrics_x_scale,
                      FT_Fixed  *ametrics_y_scale );


 /**********************************************************************
  *
  * @function:
  *    FT_Get_PFR_Kerning
  *
  * @description:
  *    Return the kerning pair corresponding to two glyphs in a PFR face.
  *    The distance is expressed in metrics units, unlike the result of
  *    @FT_Get_Kerning.
  *
  * @input:
  *    face  :: A handle to the input face.
  *
  *    left  :: Index of the left glyph.
  *
  *    right :: Index of the right glyph.
  *
  * @output:
  *    avector :: A kerning vector.
  *
  * @return:
  *    FreeType error code.  0 means success.
  *
  * @note:
  *    This function always return distances in original PFR metrics
  *    units.  This is unlike @FT_Get_Kerning with the @FT_KERNING_UNSCALED
  *    mode, which always returns distances converted to outline units.
  *
  *    You can use the value of the `x_scale' and `y_scale' parameters
  *    returned by @FT_Get_PFR_Metrics to scale these to device sub-pixels.
  */
  FT_EXPORT( FT_Error )
  FT_Get_PFR_Kerning( FT_Face     face,
                      FT_UInt     left,
                      FT_UInt     right,
                      FT_Vector  *avector );


 /**********************************************************************
  *
  * @function:
  *    FT_Get_PFR_Advance
  *
  * @description:
  *    Return a given glyph advance, expressed in original metrics units,
  *    from a PFR font.
  *
  * @input:
  *    face   :: A handle to the input face.
  *
  *    gindex :: The glyph index.
  *
  * @output:
  *    aadvance :: The glyph advance in metrics units.
  *
  * @return:
  *    FreeType error code.  0 means success.
  *
  * @note:
  *    You can use the `x_scale' or `y_scale' results of @FT_Get_PFR_Metrics
  *    to convert the advance to device sub-pixels (i.e. 1/64th of pixels).
  */
  FT_EXPORT( FT_Error )
  FT_Get_PFR_Advance( FT_Face   face,
                      FT_UInt   gindex,
                      FT_Pos   *aadvance );

 /* */


FT_END_HEADER

#endif /* __FTBDF_H__ */


/* END */
