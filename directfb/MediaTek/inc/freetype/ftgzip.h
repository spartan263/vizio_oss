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
/*  ftgzip.h                                                               */
/*                                                                         */
/*    Gzip-compressed stream support.                                      */
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


#ifndef __FTGZIP_H__
#define __FTGZIP_H__

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
  /*    gzip                                                               */
  /*                                                                       */
  /* <Title>                                                               */
  /*    GZIP Streams                                                       */
  /*                                                                       */
  /* <Abstract>                                                            */
  /*    Using gzip-compressed font files                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This section contains the declaration of Gzip-specific functions.  */
  /*                                                                       */
  /*************************************************************************/


 /************************************************************************
  *
  * @function:
  *   FT_Stream_OpenGzip
  *
  * @description:
  *   Open a new stream to parse gzip-compressed font files.  This is
  *   mainly used to support the compressed *.pcf.gz fonts that come
  *   with XFree86.
  *
  * @input:
  *   stream :: The target embedding stream.
  *
  *   source :: The source stream.
  *
  * @return:
  *   FreeType error code.  0 means success.
  *
  * @note:
  *   The source stream must be opened _before_ calling this function.
  *
  *   Calling the internal function FT_Stream_Close on the new stream will
  *   *not* call FT_Stream_Close on the source stream.  None of the stream
  *   objects will be released to the heap.
  *
  *   The stream implementation is very basic and resets the decompression
  *   process each time seeking backwards is needed within the stream.
  *
  *   In certain builds of the library, gzip compression recognition is
  *   automatically handled when calling @FT_New_Face or @FT_Open_Face.
  *   This means that if no font driver is capable of handling the raw
  *   compressed file, the library will try to open a gzipped stream from
  *   it and re-open the face with it.
  *
  *   This function may return "FT_Err_Unimplemented" if your build of
  *   FreeType was not compiled with zlib support.
  */
  FT_EXPORT( FT_Error )
  FT_Stream_OpenGzip( FT_Stream  stream,
                      FT_Stream  source );

 /* */


FT_END_HEADER

#endif /* __FTGZIP_H__ */


/* END */
