/*\
 *
 *  (C) Copyright 1989 by Carnegie Mellon University
 *
 *  Permission to use, copy, modify, and distribute these programs
 *  and their documentation for any purpose and without fee is
 *  hereby granted, provided that this copyright and permission
 *  notice appear on all copies and supporting documentation, and
 *  that the name of Carnegie Mellon University not be used in
 *  advertising or publicity pertaining to distribution of the
 *  programs without specific prior permission and notice be given
 *  in supporting documentation that copying and distribution is
 *  by permission of Carnegie Mellon University.
 *
 *  Carnegie Mellon University makes no representations about the
 *  suitability of this software for any purpose.  It is provided
 *  as is, without express or implied warranty.
 *
 *  Software by Andrew Werth, Information Technology Center,
 *  Carnegie Mellon University, except where noted.
\*/

/*\
 *  This code has been taken from the Fuzzy Bitmap library, as
 *  noted below.  It has been modified in several ways:  Input/
 *  Output has been changed to match Raster Graphics Tool Kit
 *  specifications; debugging output has been removed; error
 *  handling has been modified for RGTK specs; procedures
 *  have been made static where appropriate; and minor changes
 *  to more closely resemble RGTK coding conventions.
 *
 *  Modifications by Andrew Werth.
 *
\*/

/*****************************************************************
 * flalfb.c: FBM Library 0.9 (Beta test) 07-Mar-89  Michael Mauldin
 *
 * Copyright (C) 1989 by Michael Mauldin.  Permission is granted to
 * use this file in whole or in part provided that you do not sell it
 * for profit and that this copyright notice is retained unchanged.
 *
 * flalfb.c: Fuzzy bitmap allocation
 *
 * CONTENTS
 *	alloc_fbm (image)
 *	free_fbm (image)
 *
 * EDITLOG
 *	LastEditDate = Tue Mar  7 19:56:45 1989 - Michael Mauldin
 *	LastFileName = /usr2/mlm/src/misc/fbm/flalfb.c
 *
 * HISTORY
 * 07-Mar-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Beta release (version 0.9) mlm@cs.cmu.edu
 *
 * 12-Nov-88  Michael Mauldin (mlm) at Carnegie-Mellon University
 *	Created.
 *****************************************************************/

#define TK_TKI_fbm
#define TK_IMPL_fbm
#define TK_GVARS_flalfb

#include <fbm.h>
#include <rgsite.h>
#include <rgerr.h>
#include <rgreal.h>

#undef TK_GVARS_flalfb
#undef TK_IMPL_fbm
#undef TK_TKI_fbm


/****************************************************************
 * alloc_fbm: Allocate enough bytes for the bitmap and colormap
 *	of an image where the header has already been filled in.
 ****************************************************************/

alloc_fbm (image)
FBM *image;
{ unsigned bmsize, cmsize;

/*  if (! free_fbm (image)) return (0);*/

  /* Calculate bytes needed */
  bmsize = (image->hdr.planes * image->hdr.plnlen);
  cmsize = image->hdr.clrlen;
  
  image->bm = (unsigned char *) MEM_Malloc(bmsize);
  if ( ((POINTER_type) image->bm) == RG_ERROR_POINTER){
      RGError = RG_INTERNAL_err;
      return(-1);
  }
  if (cmsize != 0) {
      image->cm = (unsigned char *) MEM_Malloc(cmsize);
      if ( ((POINTER_type) image->cm) == RG_ERROR_POINTER){
	  RGError = RG_INTERNAL_err;
	  return(-1);
      }
  }

  return (1);
}

/****************************************************************
 * free_fbm: Free the storage allocate by alloc_fbm
 ****************************************************************/

free_fbm (image)
FBM *image;
{
  if (image->bm)
  { free ((char *) image->bm); image->bm = (unsigned char *) RG_NULL; }

  if (image->cm)
  { free ((char *) image->cm); image->cm = (unsigned char *) RG_NULL; }

  return (1);
}
