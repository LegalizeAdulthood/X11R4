
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/*************************************************/
/*	ar5freeDg.c                              */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  12/88    BT	Adapted from ar5dumpDg.c
 */

#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#else
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#endif
#include "ar5defs.h"
#include "ar5private.h"

/**--------------------------------------------------------------------------**/
static bool ArDgObjectFree();
static bool ArDgGroupFree();
static bool ArDgPolyFree();
static bool ArDgLineFree();
static bool ArDgEditFree();
static bool ArDgArcFree();
static bool ArDgEllipseFree();
static bool ArDgSplineFree();
static bool ArDgRasterFree();
static bool ArDgHexFree();
static bool ArDgTextstrFree();
static bool ArDgBoxFree();
static bool ArDgEqnFree();
static bool ArDgMicroFree();
static bool ArDgSplineSegFree();
static bool ArDgVectorListFree();
static bool ArDgOutlineFree();
static bool ArDgChartFree();
/**--------------------------------------------------------------------------**/
/*************************************************************************
  This is the top level diagram free-ing routine. It takes a pointer to a
  top level diagram structure, an ArDgHead_t *,
  It does not free the argument; presumably, the ArDgHead_t lives in a 
  Frame_t or a HeaderFooter_t and cannot be free'ed in isolation.
  The return value is YES or NO.
*************************************************************************/
bool ArDiagramFree(dgP)
ArDgHead_t *dgP;
{
  return(ArDgObjectFree(dgP->contents));
}

/**************************************************************************
  The top object-level diagram output routine. It emits the common portion
  of the diagram and calls the object-specific routines to dump the
  remainder. Note that some of the object-specific routines call
  ArDgObjectFree() recursively to dump their contents.
***************************************************************************/
static bool ArDgObjectFree(obj)
ArDgObject_t *obj;
{
 int result;

  if (obj == NULL) {
    return( YES );
  }
  /* free the object specific data */
  switch(obj->type) {
  case dg_GROUP:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgGroupFree( (ArDgObject_t *)obj->info);
    break;
  case dg_LINE:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgLineFree( (LineTSD_t *)obj->info);
    break;
  case dg_EDIT_STATE:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgEditFree( (EditStateTSD_t *)obj->info);
    break;
  case dg_POLY:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgPolyFree( (PolyTSD_t *)obj->info);
    break;
  case dg_ARC:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgArcFree( (ArcTSD_t *)obj->info);
    break;
  case dg_SPLINE:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgSplineFree( (SplineTSD_t *)obj->info);
    break;
  case dg_RELL:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgEllipseFree( (EllipseTSD_t *)obj->info);
    break;
  case dg_IMAGE:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgImageFree( (ImageTSD_t *)obj->info);
    break;
  case dg_TEXTSTR:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgTextstrFree( (TextstrTSD_t *)obj->info);
    break;
  case dg_BOX:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgBoxFree( (BoxTSD_t *)obj->info);
    break;
  case dg_EQN:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgEqnFree( (EqnTSD_t *)obj->info);
    break;
  case dg_MICRODOC:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgMicroFree( (MicroTSD_t *)obj->info);
    break;
  case dg_SPLINE_SEG:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgSplineSegFree( (SplineSegTSD_t *)obj->info);
    break;
  case dg_VECTORS:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgVectorListFree( (VectorListTSD_t *)obj->info);
    break;
  case dg_OUTLINE:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgOutlineFree( (OutlineTSD_t *)obj->info);
    break;
  case dg_CHART:
    result = /* ArDgObjectFree( obj->next ) && */
	     ArDgChartFree( (ChartTSD_t *)obj->info);
    break;
  /*
   * etc, etc ...
   */
  default:
    ArFprintf(stderr, "ArDgObjectFree(%#x) unknown type %d. Get help.\n",
		obj, obj->type);
    result = NO;
  }

  FREE((char *)obj);
  return(YES);
}

/*************************************************************************
   The following routines free the individual diagramming objects.
   Some, like those for the group and poly, are recursive, most are
   not.
**************************************************************************/
   
static bool ArDgGroupFree( info)
ArDgObject_t *info;
{
  ArDgObject_t *start;
  ArDgObject_t *next;
  
   if ( info == NULL ){
      return( YES );
   }
   for ( start = info; start; ){
      next = start -> next;
      if ( ArDgObjectFree(start) != YES ){
         return(NO);
      }
      start = next;
   }
   return(YES);
}

/*------------------------------------------------------------------------*/
static bool ArDgPolyFree( info)
PolyTSD_t *info;
{
    if (info == NULL) {
      return( YES );
    }
    if(ArDgGroupFree( info->contents) != YES)
	return(NO);
    FREE((char *)info);
    return(YES);
}
  
/*------------------------------------------------------------------------*/
static bool ArDgLineFree( info)
LineTSD_t *info;
{
    if (info == NULL) {
      return( YES );
    }
    FREE((char *)info);
    return(YES);
}

/*------------------------------------------------------------------------*/
static bool ArDgEditFree( info)
EditStateTSD_t *info;
{
    if (info == NULL) {
      return( YES );
    }
    FREE((char *)info);
    return(YES);
}

/*------------------------------------------------------------------------*/
static bool ArDgArcFree( info)
ArcTSD_t *info;
{
    if (info == NULL) {
      return( YES );
    }
    FREE((char *)info);
    return(YES);
}

/*------------------------------------------------------------------------*/
static bool ArDgEllipseFree( info)
EllipseTSD_t *info;
{
    if (info == NULL) {
      return( YES );
    }
    FREE((char *)info);
    return(YES);
}

/*------------------------------------------------------------------------*/
static bool ArDgSplineFree( info)
SplineTSD_t *info;
{
  KNOT_t *start, *nextP;

    if (info == NULL) {
      return( YES );
    }
    for(start=info->knot_list; start; ) {
      nextP = start->next;
      FREE((char *)start);
      start = nextP;
    }
    FREE((char *)info);
    return(YES);
}

/*------------------------------------------------------------------------*/
int  ArDgImageFree( info)
ImageTSD_t *info;
{
  BREAK_t *start, *nextP;
  
    if (info == NULL) {
      return( YES );
    }
    for(start=info->break_list; start; ) {
    nextP = start->next;
    FREE(start);
    start = nextP;
    }

    ArDgRasterFree( info->orig);
    ArDgRasterFree( info->disp);
    FREE((char *)info);
    return(YES);
}

/*------------------------------------------------------------------------*/
static bool ArDgRasterFree( raster)
RASTER_t *raster;
{

    if (raster == NULL) {
      return( YES );
    }
    /* free map data, if any */
    if(raster->map_size)
    ArDgHexFree( raster->map);

    /* free screen data, if any */
    if(raster->screenx*raster->screeny)
    ArDgHexFree( raster->screen);

    /* finally, free main raster data */
    ArDgHexFree( raster->raster);
    FREE((char *)raster);
    return(YES);
}

/*------------------------------------------------------------------------*/
static bool ArDgHexFree( hex)
HEX_t *hex;
{
    if (hex == NULL) {
      return( YES );
    }
    FREE(hex->data);
    FREE(hex);
    return(YES);
}

/*---------------------------------------------------------------------------*/
static bool ArDgTextstrFree( info)
TextstrTSD_t *info;
{
    if (info == NULL) {
      return( YES );
    }
    FREE(info->attrib);
    FREE(info->font);
    FREE((char *)info->text);
    FREE((char *)info);
    return(YES);
}

/*----------------------------------------------------------------------------*/
static bool ArDgBoxFree( info)
BoxTSD_t *info;
{
    if (info == NULL) {
      return( YES );
    }
    FREE((char *)info);
    return(YES);
}

/*----------------------------------------------------------------------------*/
static bool ArDgEqnFree( info)
EqnTSD_t *info;
{
    if (info == NULL) {
      return( YES );
    }
    FREE(info->font);
    FREE(info->text);
    FREE((char *)info);
    return(YES);
}

/*----------------------------------------------------------------------------*/
static bool ArDgMicroFree( info)
MicroTSD_t *info;
{
  if (info == NULL) {
      return( YES );
  }
  if (ArObjectFree(info->text->contentsP) == YES ){/* && */
      /* ArObjectFree(&info->text->micro_page) == YES) { */
	  FREE(info->text);
	  FREE((char *)info);
	  return( YES );
  }
  return( NO );
}

/*---------------------------------------------------------------------------*/
static bool ArDgSplineSegFree( info)
SplineSegTSD_t *info;
{
    if (info == NULL) {
      return( YES );
    }
    FREE((char *)info);
    return(YES);
}

/*---------------------------------------------------------------------------*/
static bool ArDgVectorListFree( info)
VectorListTSD_t *info;
{
  HexList_t *start, *nextP;

  if (info == NULL) {
      return( YES );
  }
  for(start = info->list; start; ) {
    nextP = start->next;
    ArDgHexFree( start->data);
    FREE(start);
    start = nextP;
  }
  FREE((char *)info);
  return(YES);
}

/*--------------------------------------------------------------------------*/
static bool ArDgOutlineFree( info)
OutlineTSD_t *info;
{
  if (info == NULL) {
      return( YES );
  }
  FREE(info->font);
  FREE(info->encrypted_data);
  FREE((char *)info);
  return(YES);
}

/*--------------------------------------------------------------------------*/
static bool ArDgChartFree( info)
ChartTSD_t *info;
{
 CtRecord_t *start_rec, *nextRecP;
 CtField_t *start_field, *nextFieldP;
 int first_D_rec = YES;

  if (info == NULL) {
      return( YES );
  }
  FREE(info->font);
  for(start_rec=info->record; start_rec; ) {
    nextRecP = start_rec->next;
    for(start_field=start_rec->field; start_field; ) {
       nextFieldP = start_field->next;
	if (start_rec->id == D && first_D_rec == YES) {
	    first_D_rec = NO;
	    FREE((char *)(start_field->value->val.pval));
	}
	FREE((char *)start_field->value);
	start_field = nextFieldP;
    }
    start_rec = nextRecP;
  }
  FREE((char *)info);
  return(YES);
}
  
/**--------------------------------------------------------------------------**/
