
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
/*	ar5loadDg.c                              */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    CK	Original coding
    02/16/89 ram fix chart loader to get doubles and display precision
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
/* #include "ar5Dg.h" this was consolidated into ar5defs.h */

/****************************************************************************
    Top level diagram load routine.
      Starts with file positioned at '=' of "Diagram =" or "Frame =".
      Ends with file positioned at first char after diagram, namely the
       closing '>' for the "<Frame".
      Returns a pointer to a valid ArDgHead struct if successful, NULL on fail.
*****************************************************************************/    
short	Arimagecount;

ArDgHead_t *ArSaveDg(fiP) 
File_info_t *fiP;
{
  ArDgHead_t *new = NULL;
  shar c;
  int number;
  
  if((new = (ArDgHead_t *)MALLOC(sizeof(ArDgHead_t))) == NULL) {
    ArSkipDg(fiP, ERR_MEMORY);
    return(NULL);
  }
  if((c = ArGetchNW(fiP)) == '=') { /* eat '=', if present */
    c = ArGetchNW(fiP);             /* otherwise, just read 'V' */
  }
  if(c != 'V') {
    ArSkipDg(fiP, ERR_FORMAT);
    return(NULL);
  }
  ArGetInt(fiP, &number, YES, YES);
  if(number != CURRENT_DIAGRAM_VERS) {
    ArSkipDg(fiP, ERR_VERSION);
    return(NULL);
  }
  new->version = number;
  new->contents = NULL;
  Arimagecount = 0;
  if((new->contents = ArSaveObject(fiP)) == NULL) {
    return(NULL);
  }
  return(new);
}

/***************************************************************************
  Object level diagram load routine.
     Starts with file positioned at opening '(' of object.
     Ends with file positioned at first character after closing ')' of object.
     Returns a pointer to a valid ArDgObject struct, or NULL on failure.
     NOTE: If the object is a group, it will contain all the objects within
     that group.
***************************************************************************/
ArDgObject_t *ArSaveObject(fiP)
File_info_t *fiP;
{
  ArDgObject_t *new = NULL;     /* the pointer to the new object we're saving */
  shar c;
  int number;

  /*** get new object structure, initialize pointer fields to NULL ***/
  if((new = (ArDgObject_t *)MALLOC(sizeof(ArDgObject_t))) == NULL) {
    ArSkipDg(fiP, ERR_MEMORY);
    return(NULL);
  }
  new->next = NULL;
  new->info = NULL;
  
  /*** first, we do this section for any object ***/
  c = ArGetchNW(fiP);                       /* skip opening '(' */
  new->type = ArGetchNW(fiP);               /* read 'type' */
  if(ArGetInt(fiP, &number, YES, YES) == NO) {   /* read version, eat ',' */
    ArSkipDg(fiP, ERR_FORMAT);
    return(NULL);
  }
  new->version = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO) {   /* read z_coord, eat ',' */
    ArSkipDg(fiP, ERR_FORMAT);
    return(NULL);
  }
  new->z_coord = number;
  if(ArGetInt(fiP, &number, NO, YES) == NO) {/*read flags, put back terminator*/
    ArSkipDg(fiP, ERR_FORMAT);
    return(NULL);
  }
  new->flags = number;
  if((c = ArGetchNW(fiP)) != ',')           /* eat ',' if there is one */
    ArUnGetch(c, fiP);
  
  /*** now, we do object-specific portion ***/
  switch(new->type) {
  case dg_GROUP:
    new->info = (opaque_ptr_t)ArSaveGroupInfo(fiP);
    break;
  case dg_LINE:
    new->info = (opaque_ptr_t)ArSaveLineInfo(fiP);
    break;
  case dg_EDIT_STATE:
    new->info = (opaque_ptr_t)ArSaveEditInfo(fiP);
    break;
  case dg_POLY:
    new->info = (opaque_ptr_t)ArSavePolyInfo(fiP);
    break;
  case dg_ARC:
    new->info = (opaque_ptr_t)ArSaveArcInfo(fiP);
    break;
  case dg_SPLINE:
    new->info = (opaque_ptr_t)ArSaveSplineInfo(fiP);
    break;
  case dg_RELL:
    new->info = (opaque_ptr_t)ArSaveEllipseInfo(fiP);
    break;
  case dg_TEXTSTR:
    new->info = (opaque_ptr_t)ArSaveTextstrInfo(fiP);
    break;
  case dg_IMAGE:
    new->info = (opaque_ptr_t)ArSaveImageInfo(fiP);
    break;
  case dg_BOX:
    new->info = (opaque_ptr_t)ArSaveBoxInfo(fiP);
    break;
  case dg_EQN:
    new->info = (opaque_ptr_t)ArSaveEqnInfo(fiP);
    break;
  case dg_MICRODOC:
    new->info = (opaque_ptr_t)ArSaveMicroInfo(fiP);
    break;
  case dg_SPLINE_SEG:
    new->info = (opaque_ptr_t)ArSaveSplineSegInfo(fiP);
    break;
  case dg_VECTORS:
    new->info = (opaque_ptr_t)ArSaveVectorListInfo(fiP);
    break;
  case dg_OUTLINE:
    new->info = (opaque_ptr_t)ArSaveOutlineInfo(fiP);
    break;
  case dg_CHART:
    new->info = (opaque_ptr_t)ArSaveChartInfo(fiP);
    break;
  case dg_POLYBOX:
    break;
  case dg_ARC_CC:
    break;
  case dg_HLINE:
    break;
  case dg_VLINE:
    break;
  case dg_TRACE:
    break;
  case dg_CONVERT:
    break;
  case dg_EPS:
    new->info = (opaque_ptr_t)ArSaveEPSInfo(fiP);
    break;
  default:
    break;
  }
  
  if(new->info == NULL) {          /* possible error in object */
    if(new->type == dg_GROUP || new->type == dg_POLY)  /* empty groups & polys OK */
      return(new);
    ArSkipObject(fiP, new->type);                      /* anything else is error */
    if(new != NULL)
      free(new);
    return(NULL);
  }
  return(new);
}

/************************************************************************
  Saves the contents of a group, returns a pointer to a LL of object
  structures that are the group's contents.
  Starts with file positioned at opening '(' of first object in the
  group. Ends at first character after closing ')' of group.
  NOTE: the group is a recursive structure. Thus, this routine works
  recursively!!
************************************************************************/
ArDgObject_t *ArSaveGroupInfo(fiP)
File_info_t *fiP;
{
  ArDgObject_t *startP, *tempP, *endP;
  shar c;

  startP = tempP = endP = NULL;
  while((c = ArGetchNW(fiP)) != ')') {
    ArUnGetch(c, fiP);
    if((tempP = ArSaveObject(fiP)) == NULL)  /* this means an error in an object */
      continue;                              /* just skip the object, and go on */
    if(startP == NULL)
      startP = tempP;
    else
      endP->next = tempP;
    endP = tempP;
  }
  return(startP);
}

/*************************************************************************
  Saves a poly structure. The poly is a special case of group. Thus, we
  simply save the poly-specific data and call ArSaveGroupInfo() to do
  the rest. It is, therefore, recursive, as is ArSaveGroupInfo().
*************************************************************************/
PolyTSD_t *ArSavePolyInfo(fiP)
File_info_t *fiP;
{
  int number;
  PolyTSD_t *new = NULL;

  if((new = (PolyTSD_t *)MALLOC(sizeof(PolyTSD_t))) == NULL)
    return(NULL);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fpat = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->ftrans = number;
  if((new->contents = ArSaveGroupInfo(fiP)) == NULL)
    return(NULL);
  return(new);
}

/***************************************************************************
  The following routines save the "non-common" contents of a diagrammming
  object. Each returns a pointer to a type specific structure (TSD)
  containing this data or null if error. Each starts at the first character
  of non-common data and ends with the file positioned after the closing
  ')' of this object. Since the objects they describe cannot contain other
  diagramming objects, these routines are not recursive. 
***************************************************************************/
LineTSD_t *ArSaveLineInfo(fiP)
File_info_t *fiP;
{
  LineTSD_t *new = NULL;
  int number;
  shar c;

  if((new = (LineTSD_t *)MALLOC(sizeof(LineTSD_t))) == NULL)
    return(NULL);
  if(ArGetFloat(fiP, &new->x1) == NO)   
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y1) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->x2) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y2) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->color = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->trans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->width = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->pattern = number;
  c = ArGetchNW(fiP);      /* eat closing ')' of group */
  return(new);
}

/*------------------------------------------------------------------------------*/
EditStateTSD_t *ArSaveEditInfo(fiP)
File_info_t *fiP;
{
  EditStateTSD_t *new = NULL;
  int number;
  shar c;

  if((new = (EditStateTSD_t *)MALLOC(sizeof(EditStateTSD_t))) == NULL)
    return(NULL);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fill = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->width = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->gravity = number;
  if(ArGetFloat(fiP, &new->gravrad) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->detent = number;
  if(ArGetFloat(fiP, &new->angle) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->pattern = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->zoom = number;
  if(ArGetFloat(fiP, &new->zoomlev) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->xshift) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->yshift) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->scale = number;
  if(ArGetFloat(fiP, &new->scalefact) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->ftrans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->ecolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->etrans = number;
  if(ArGetFloat(fiP, &new->textangle) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->tcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->display = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->align = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->ontop = number;
  if(ArGetFloat(fiP, &new->minw) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->minh) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->majw = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->majh = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->type = number;
  if(ArGetFloat(fiP, &new->mini) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->maji = number;
  c = ArGetchNW(fiP);      /* eat closing ')' of group */
  return(new);
}

/*------------------------------------------------------------------------------*/
ArcTSD_t *ArSaveArcInfo(fiP)
File_info_t *fiP;
{
  ArcTSD_t *new = NULL;
  int number;
  shar c;

  if((new = (ArcTSD_t *)MALLOC(sizeof(ArcTSD_t))) == NULL)
    return(NULL);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->format = number;
  if(ArGetFloat(fiP, &new->btx) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->bty) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->bx) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->by) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->mx) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->my) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->ex) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->ey) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->tex) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->tey) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->x) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fpattern = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->ftrans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->bcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->btrans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->bwidth = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->bpattern = number;
  c = ArGetchNW(fiP);      /* eat closing ')' of group */
  return(new);
}

/*------------------------------------------------------------------------------*/
SplineTSD_t *ArSaveSplineInfo(fiP)
File_info_t *fiP;
{
  SplineTSD_t *new = NULL;
  int number, i;
  shar c;
  KNOT_t *temp_knot, *last_knot = NULL;

  if((new = (SplineTSD_t *)MALLOC(sizeof(SplineTSD_t))) == NULL)
    return(NULL);
  if(ArGetFloat(fiP, &new->x) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fpattern = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->ftrans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->number_of_knots = number;

  /** save knots as LL of KNOT_t structures **/
  new->knot_list = NULL;
  for(i=0; i<new->number_of_knots; i++) {
    temp_knot = NULL;
    if((temp_knot = (KNOT_t *)MALLOC(sizeof(KNOT_t))) == NULL)
      return(NULL);
    temp_knot->next = NULL;
    if(ArGetFloat(fiP, &temp_knot->x) == NO)
      return(NULL);
    c = ArGetchNW(fiP);
    if(ArGetFloat(fiP, &temp_knot->y) == NO)
      return(NULL);
    c = ArGetchNW(fiP);
    if(ArGetInt(fiP, &number, YES, YES) == NO)
      return(NULL);
    temp_knot->color = number;
    if(ArGetInt(fiP, &number, YES, YES) == NO)
      return(NULL);
    temp_knot->trans = number;
    if(ArGetInt(fiP, &number, YES, YES) == NO)
      return(NULL);
    temp_knot->width = number;
    if(ArGetInt(fiP, &number, YES, YES) == NO)
      return(NULL);
    temp_knot->pattern = number;
    if ( last_knot == NULL )
      new->knot_list = temp_knot;
    else
      last_knot->next = temp_knot;
    last_knot = temp_knot;
  }

  new->type = ArGetchNW(fiP);
  c = ArGetchNW(fiP);      /* eat closing ')' of group */
  return(new);
}

/*------------------------------------------------------------------------------*/
EllipseTSD_t *ArSaveEllipseInfo(fiP)
File_info_t *fiP;
{
  EllipseTSD_t *new = NULL;
  int number;
  shar c;

  if((new = (EllipseTSD_t *)MALLOC(sizeof(EllipseTSD_t))) == NULL)
    return(NULL);
  if(ArGetFloat(fiP, &new->ox) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->oy) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->t1x) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->t1y) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->t2x) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->t2y) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->ftrans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->fpattern = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->bcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->btrans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->bwidth = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->bpattern = number;
  c = ArGetchNW(fiP);      /* eat closing ')' of group */
  return(new);
}

/*------------------------------------------------------------------------------*/
TextstrTSD_t *ArSaveTextstrInfo(fiP)
File_info_t *fiP;
{
  TextstrTSD_t *new;
  shar c, last_c;
  int number;
  long hex;
  char *ip, buf[80];
  shar shr_buf[MAX_TEXT_S_LEN+1];       /* a shar buffer  */
  shar *shr_ip;                         /* an index into it */
  shar *shr_limit;                      /* the end of shar buffer */

  if((new = (TextstrTSD_t *)MALLOC(sizeof(TextstrTSD_t))) == NULL)
    return(NULL);
  if(ArGetFloat(fiP, &new->x) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->anchor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->color = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->trans = number;
  if(ArGetFloat(fiP, &new->angle) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  
  /* save attribute info */
  ip = buf;
  while((c = ArGetchNW(fiP)) != ',')
    *ip++ = c;
  *ip = '\0';
  if((new->attrib = (char *)MALLOC(strlen(buf) + 1)) == NULL)
    return(NULL);
  strcpy(new->attrib, buf);

  /* save font info */
  ip = buf;
  while((c = ArGetchNW(fiP)) != ',')
    *ip++ = c;
  *ip = '\0';
  if((new->font = (char *)MALLOC(strlen(buf) + 1)) == NULL)
    return(NULL);
  strcpy(new->font, buf);

  /* save text (as a shar string)  */
  shr_ip = shr_buf;
  last_c = 0;
  for(shr_limit = shr_ip+MAX_TEXT_S_LEN; shr_ip < shr_limit; shr_ip++) {
    c = ArGetch(fiP);
    if(c == '\n') {                  /* skip NL and indent in IAM */
      c = ArGetchNW(fiP);
    }
    if((c == 'X' || c == 'x') && last_c == '\\') { /* process a hex char */
      if(c == 'X') 
	ArGetHex(fiP, &hex, 4);
      else 
	ArGetHex(fiP, &hex, 2);
      c = hex;
    }
    if(c == ')' && last_c != '\\') { /* normal break point */
      break;
    }
    *shr_ip = c;
    last_c = c;
  }
  *shr_ip = '\0';
  if((new->text = (shar *)MALLOC(sizeof(shar) * (ArShrlen(shr_buf) + 1))) == NULL)
    return(NULL);
  ArShrcpy(new->text, shr_buf);

  return(new);
}

/*------------------------------------------------------------------------------*/
ImageTSD_t *ArSaveImageInfo(fiP)
File_info_t *fiP;
{
  ImageTSD_t *new;
  shar c;
  int number, i;
  BREAK_t *last = NULL, *temp;

  if((new = (ImageTSD_t *)MALLOC(sizeof(ImageTSD_t))) == NULL)
    return(NULL);
  if(ArGetFloat(fiP, &new->xo) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->yo) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->xh) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->yh) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->xv) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->yv) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->cx1 = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->cy1 = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->cx2 = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->cy2 = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->num_breaks = number;
  
  /* save LL of break pairs */
  new->break_list = NULL;
  for(i=0; i<new->num_breaks; i++) {
    temp = NULL;
    if((temp = (BREAK_t *)MALLOC(sizeof(BREAK_t))) == NULL)
      return(NULL);
    temp->next = NULL;
    if(ArGetFloat(fiP, &temp->x) == NO)
      return(NULL);
    c = ArGetchNW(fiP);
    if(ArGetFloat(fiP, &temp->y) == NO)
      return(NULL);
    c = ArGetchNW(fiP);
    if(last == NULL)
      new->break_list = temp;
    else
      last->next = temp;
    last = temp;
  }

  /* continue with image structure */
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->color = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->trans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->submenu = number;
  if(ArGetFloat(fiP, &new->origw) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->origh) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  new->orig = ArSaveRaster(fiP);
  c = ArGetchNW(fiP);    /* eat ',' */
  new->disp = ArSaveRaster(fiP);
  c = ArGetchNW(fiP);      /* eat closing ')' of group */
  return(new);
}

/*------------------------------------------------------------------------------*/
RASTER_t *ArSaveRaster(fiP)
File_info_t *fiP;
{
  RASTER_t *new;
  int number;
  shar c;

  if((new = (RASTER_t *)MALLOC(sizeof(RASTER_t))) == NULL)
    return(NULL);
  if(ArGetInt(fiP, &number, NO, YES) == NO)
    return(NULL);
  new->id = number;

  if ( number <= Arimagecount ){
     new -> reference = YES;
     return(new);
  }else{
     Arimagecount = number;
     new -> reference = NO;
  }
  c = ArGetchNW(fiP);    /* eat ',' */

#ifdef NEVER

   this is wrong - MS

  /* check if this is just a reference  */
  if((c = ArGetchNW(fiP)) == ')') {
    ArUnGetch(c, fiP);	/** BT **/
    new->reference = YES;
    return(new);
  }
  new->reference = NO;
  ArUnGetch(c, fiP);

#endif

  /* continue on with raster info */
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->xh = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->yh = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->xv = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->yv = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->x1 = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->y1 = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->x2 = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->y2 = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->map_size = number;

  /* save map, if any */
  if(new->map_size > 0) {
    new->map = ArSaveHex(fiP);
    c = ArGetchNW(fiP);  /* eat ',' after hex set */
  }
  else
    new->map = NULL;

  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->screenx = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->screeny = number;

  /*save screen, if any */
  if(new->screenx * new->screeny > 0) {
    new->screen = ArSaveHex(fiP);
    c = ArGetchNW(fiP);  /* eat ',' after hex set */
  }
  else
    new->screen = NULL;

  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->w = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->h = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->d = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->format = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->size = number;
  new->raster = ArSaveHex(fiP);
  return(new);
}

/*------------------------------------------------------------------------------*/
HEX_t *ArSaveHex(fiP)
File_info_t *fiP;
{
  shar c;
  int number;
  HEX_t *new;
  unsigned char *ip;
  char s[3];
  register i;
  
  if((new = (HEX_t *)MALLOC(sizeof(HEX_t))) == 0)
    return(NULL);
  c = ArGetchNW(fiP);      /* eat opening '(' of hex set */
  c = ArGetchNW(fiP);      /* eat 'X' */
  c = ArGetchNW(fiP);      /* eat '0' */
  c = ArGetchNW(fiP);      /* eat ',' */
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->size = number;

  if((new->data = (unsigned char *)MALLOC(new->size)) == 0)
    return(NULL);
  ip = new->data;
  for(i=0; i<new->size; i++) {
    s[0] = ArGetchNW(fiP);
    s[1] = ArGetchNW(fiP);
    s[2] = '\0';
    sscanf(s, "%x", &number);
    *ip++ = (unsigned char)number;
  }

  c = ArGetchNW(fiP);      /* eat closing ')' of hex set */
  return(new);
}

/*------------------------------------------------------------------------------*/
BoxTSD_t *ArSaveBoxInfo(fiP)
File_info_t *fiP;
{
  BoxTSD_t *new = NULL;
  int number;
  shar c;

  if((new = (BoxTSD_t *)MALLOC(sizeof(BoxTSD_t))) == NULL)
    return(NULL);
  if(ArGetFloat(fiP, &new->x1) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y1) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->x2) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y2) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->texture = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->border_texture = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->border_width = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->border_pattern = number;
  c = ArGetchNW(fiP);      /* eat closing ')' of group */
  return(new);
}

/*---------------------------------------------------------------------*/
EqnTSD_t *ArSaveEqnInfo(fiP)
File_info_t *fiP;
{
  EqnTSD_t *new = NULL;
  shar c, last_c;
  char *ip, buf[30];
  shar *shr_ip, shr_buf[1024];
  long hex;

  if((new = (EqnTSD_t *)MALLOC(sizeof(EqnTSD_t))) == NULL)
    return(NULL);
  if(ArGetFloat(fiP, &new->x) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  
  /* save font info  */
  ip = buf;
  while((c = ArGetchNW(fiP)) != ',')
    *ip++ = c;
  *ip = '\0';
  if((new->font = (char *)MALLOC(strlen(buf) + 1)) == NULL)
    return(NULL);
  strcpy(new->font, buf);

  /* save equation info */
  shr_ip = shr_buf;
  for( last_c = 0;;) {
    c = ArGetch(fiP);
    if(c == '\n') {                  /* skip NL and indent in IAM */
      c = ArGetchNW(fiP);
    }
    if((c == 'X' || c == 'x') && last_c == '\\') { /* process a hex char */
      if(c == 'X') 
	ArGetHex(fiP, &hex, 4);
      else 
	ArGetHex(fiP, &hex, 2);
      c = hex;
    }
    if(c == ')' && last_c != '\\') { /* normal break point */
      break;
    }
    *shr_ip++ = c;
    last_c = c;
  }
  *shr_ip = '\0';
  if((new->text = (shar *)MALLOC(sizeof(shar) * (ArShrlen(shr_buf) + 1))) == NULL)
    return(NULL);
  ArShrcpy(new->text, shr_buf);

  return(new);
}
  
/*----------------------------------------------------------------------------*/
MicroTSD_t *ArSaveMicroInfo(fiP)
File_info_t *fiP;
{
  MicroTSD_t *new;
  int number;
  shar c;

  if((new = (MicroTSD_t *)MALLOC(sizeof(MicroTSD_t))) == NULL)
    return(NULL);
  if(ArGetFloat(fiP, &new->x) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->bcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->btrans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->bpat = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->lcolor = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->ltrans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->width = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->pattern = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->flags = number;

  new->text = ArBuildMicrodoc(fiP);

  c = ArGetchNW(fiP);  /* eat closing ')' of object */
  return(new);
}

/*---------------------------------------------------------------------------*/
SplineSegTSD_t *ArSaveSplineSegInfo(fiP)
File_info_t *fiP;
{
  SplineSegTSD_t *new;
  shar c;
  int number;

  if((new = (SplineSegTSD_t *)MALLOC(sizeof(SplineSegTSD_t))) == NULL)
    return(NULL);
  if(ArGetFloat(fiP, &new->x1) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y1) == NO)
      return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->x2) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y2) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->color = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->trans = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->width = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->pattern = number;
  c = ArGetchNW(fiP);      /* eat closing ')' of group */
  return(new);
}

HexList_t *
arbuildhexlist(fiP)

   File_info_t *fiP;

{
   char        s[4+1];
   shar        c;
   long        no_of_pairs;
   HexList_t   *last;
   HexList_t   *first;
   HexList_t   *temp;

  /*
   * for each data hex set there is a prior hex set
   * that contains the number of x,y pairs in the
   * following data hex set. First, we read this 
   * small set to get this number. While it is not
   * zero, we save the following set and add it to
   * the LL.
   */

  last = first = NULL;
  for(;;) {
    c = ArGetchNW(fiP);  /* eat '(' */
    c = ArGetchNW(fiP);  /* eat 'X' */
    c = ArGetchNW(fiP);  /* eat '0' */
    c = ArGetchNW(fiP);  /* eat ',' */
    c = ArGetchNW(fiP);  /* eat '2' */
    c = ArGetchNW(fiP);  /* eat ',' */
    s[0] = ArGetchNW(fiP);
    s[1] = ArGetchNW(fiP);
    s[2] = ArGetchNW(fiP);
    s[3] = ArGetchNW(fiP);
    s[4] = '\0';
    c = ArGetchNW(fiP);  /* eat ')' */

    sscanf(s, "%lx", &no_of_pairs);
    if(no_of_pairs == 0)
      break;

    if((temp = (HexList_t *)MALLOC(sizeof(HexList_t))) == NULL)
      return(last);
    temp->next = NULL;
    temp->no_pairs = no_of_pairs;
    temp->data = ArSaveHex(fiP);
    if ( last == NULL ){
       first = temp;
    }else{
       last->next = temp;
    }
    last = temp;
  }
  return(first);
}

/*--------------------------------------------------------------------------*/
VectorListTSD_t *ArSaveVectorListInfo(fiP)
File_info_t *fiP;
{
  VectorListTSD_t *new = NULL;
  shar c;
  int number;

  if((new = (VectorListTSD_t *)MALLOC(sizeof(VectorListTSD_t))) == NULL)
    return(NULL);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->color = number;
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->trans = number;
  if(ArGetFloat(fiP, &new->xo) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->yo) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->xh) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->yh) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->xv) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->yv) == NO)
    return(NULL);
  c = ArGetchNW(fiP);

  if ( (new -> list = arbuildhexlist(fiP)) == NULL ){
     free(new);
     return(NULL);
  }else{
     c = ArGetchNW(fiP); /* eat closing ')' of object */
     return(new);
  }
}

EpsTSD_t *ArSaveEPSInfo(fiP)

   File_info_t *fiP;

{
  EpsTSD_t	*new = NULL;
  long		no_of_bytes;
  shar		c;
  char		s[4+1];
  HexList_t	*first,*last,*temp;
  unsigned char	*dptr;
  short		i;

   last = first = NULL;
   if ( (new = (EpsTSD_t *)MALLOC(sizeof(EpsTSD_t))) == NULL ){
     return(NULL);
   }
   if ( ArGetFloat(fiP, &new -> xo) == NO ){
      goto eps_error;
   }
   c = ArGetchNW(fiP);
   if ( ArGetFloat(fiP, &new -> yo) == NO){
      goto eps_error;
   }
   c = ArGetchNW(fiP);
   if ( ArGetFloat(fiP, &new -> xh) == NO){
      goto eps_error;
   }
   c = ArGetchNW(fiP);
   if ( ArGetFloat(fiP, &new -> yh) == NO){
      goto eps_error;
   }
   c = ArGetchNW(fiP);
   if ( ArGetFloat(fiP, &new -> xv) == NO){
      goto eps_error;
   }
   c = ArGetchNW(fiP);
   if ( ArGetFloat(fiP, &new -> yv) == NO){
      goto eps_error;
   }
   c = ArGetchNW(fiP);
   if ( ArGetFloat(fiP, &new -> llx) == NO){
      goto eps_error;
   }
   c = ArGetchNW(fiP);
   if ( ArGetFloat(fiP, &new -> lly) == NO){
      goto eps_error;
   }
   c = ArGetchNW(fiP);
   if ( ArGetFloat(fiP, &new -> urx) == NO){
      goto eps_error;
   }
   c = ArGetchNW(fiP);
   if ( ArGetFloat(fiP, &new -> ury) == NO){
      goto eps_error;
   }
   if ( (new -> display_obj = ArSaveObject(fiP)) == NULL ){
      goto eps_error;
   }
   for(;;) {
      if ( ArGetchNW(fiP) == '(' &&
           ArGetchNW(fiP) == 'X'  &&
           ArGetchNW(fiP) == '0'  &&
           ArGetchNW(fiP) == ','  &&
           ArGetchNW(fiP) == '2'  &&
           ArGetchNW(fiP) == ','  ){
         s[0] = ArGetchNW(fiP);
         s[1] = ArGetchNW(fiP);
         s[2] = ArGetchNW(fiP);
         s[3] = ArGetchNW(fiP);
         s[4] = '\0';
         c = ArGetchNW(fiP);  /* eat ')' */
         if ( sscanf(s, "%lx", &no_of_bytes) == 1 ){
            if ( no_of_bytes == 0 ){
               break;
            }
            if ( (temp = (HexList_t *)MALLOC(sizeof(HexList_t))) == NULL){
               goto eps_error;
            }else{
               if ( first == NULL ){
	          first = temp;
	          last = temp;
	       }else{
	          last -> next = temp;
	          last = temp;
	       }
	       temp -> next = NULL;
	       temp -> no_pairs = no_of_bytes;
	       if ( (temp -> data = (HEX_t *)MALLOC(sizeof(HEX_t))) == NULL ){
                  goto eps_error;
	       }else{
	          temp -> data -> size = no_of_bytes;
	          if ( (temp -> data -> data =
		        (unsigned char *)MALLOC(no_of_bytes)) == NULL ){
                     goto eps_error;
	          }else{
	             for ( dptr = temp -> data -> data,i = 0;
		           i < no_of_bytes;
			   i++,dptr++ ){
		        if ( (c = ArGetchNW(fiP)) == '\\' ){
		           c = ArGetch(fiP);
		           if ( c >= '0' && c <= '3' ){
		              register int accum = (c - '0') << 6;
		              c = ArGetchNW(fiP);
		              if ( c >= '0' && c <= '7' ){
			         accum += (c - '0') << 3;
			         c = ArGetchNW(fiP);
			         if ( c >= '0' && c <= '7' ){
			            c += accum - '0';
			         }else{
			            /* error */
                                    goto eps_error;
			         }
			      }else{
			         /* error */
                                 goto eps_error;
			      }
		           }
		        }
		        *dptr = c;
	             }
	             if ( (c = ArGetchNW(fiP)) != ',' ){
		        /* error */
                        goto eps_error;
	             }
	          }
	       }
            }
         }else{  /* sscanf error */
            goto eps_error;
         }
      }else{  /* not (X0,2, ... */
         goto eps_error;
      }
   }
   new -> list = first;
   c = ArGetchNW(fiP); /* eat closing ')' of object */
   return(new);

eps_error:
   for ( last = first; last != NULL; ){
      last = last -> next;
      if ( first -> data != NULL ){
         if ( first -> data -> data != NULL ){
            free(first -> data -> data);
         }
         free(first -> data);
      }
      free(first);
      first = last;
   }
   ArSkipObject(fiP, dg_EPS);
   free(new);
   return(NULL);
}

/*-----------------------------------------------------------------------------*/
OutlineTSD_t *ArSaveOutlineInfo(fiP)
File_info_t *fiP;
{
  OutlineTSD_t *new = NULL;
  shar c;
  int number, level, len, limit;
  char s[MAX_S_LEN], *ip;

  if((new = (OutlineTSD_t *)MALLOC(sizeof(OutlineTSD_t))) == NULL)
    return(NULL);
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->key = number;

  /* save font info as string */
  ip = s;
  while((c = ArGetchNW(fiP)) != ',') 
    *ip++ = c;
  *ip = '\0';
  if((new->font = (char *)MALLOC(strlen(s) + 1)) == NULL)
    return(NULL);
  strcpy(new->font, s);

  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->character = number;

  /* save encrypted data as string (not great, but what else can we do ?) */
  len = 0;
  level = 1;
  limit = MAX_S_LEN;
  ip = (char *)ArSafeMalloc(limit+1);
  while(level) {
    c = ArGetchNW(fiP);
    if(c == ')')
      level--;
    if(c == '(')
      level++;
    if(len >= limit) {
      limit += MAX_S_LEN;
      ip = (char *)ArSafeRealloc(ip, limit+1);
    }
    ip[len++] = c;
  }
  ip[len-1] = '\0';   /* terminate string & cut off last paren. */
  new->encrypted_data = (char *)ArSafeMalloc(len);
  strcpy(new->encrypted_data, ip);
  free(ip);

  return(new);
}

/*-----------------------------------------------------------------------------*/
ChartTSD_t *ArSaveChartInfo(fiP)
File_info_t *fiP;
{
  ChartTSD_t *new = NULL;
  int number;
  shar c;
  char *ip, buf[80];
  CtRecord_t *record, *last_record = NULL;

  if((new = (ChartTSD_t *)MALLOC(sizeof(ChartTSD_t))) == NULL)
    return(NULL);
  new->record = NULL;
  if(ArGetFloat(fiP, &new->x1) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y1) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->x2) == NO)
    return(NULL);
  c = ArGetchNW(fiP);
  if(ArGetFloat(fiP, &new->y2) == NO)
    return(NULL);
  c = ArGetchNW(fiP);

  /* save font info */
  ip = buf;
  while((c = ArGetchNW(fiP)) != ',')
    *ip++ = c;
  *ip = '\0';
  if((new->font = (char *)MALLOC(strlen(buf) + 1)) == NULL)
    return(NULL);
  strcpy(new->font, buf);

  /* read chart version number */
  if(ArGetInt(fiP, &number, YES, YES) == NO)
    return(NULL);
  new->version = number;

  /* read records */
  while((record = get_ct_record(fiP)) != NULL) {
    if(last_record == NULL)
      new->record = record;
    else
      last_record->next = record;
    last_record = record;
  }

  c = ArGetchNW(fiP); /* eat closing ')' of object */
  return(new);
}

CtRecord_t *get_ct_record(fiP)
File_info_t *fiP;
{
  shar c;
  char id[ID_LEN+1];
  CtRecord_t *new;
  CtField_t *new_field, *last_field = NULL;
  int i;
  int field_no;

  /* allocate & initialize a record structure */
  if((new = (CtRecord_t *)MALLOC(sizeof(CtRecord_t))) == NULL)
    return(NULL);
  new->next = NULL;
  new->field = NULL;
  
  /* read the record id */
  if((c = ArGetchNW(fiP)) != '(') { /* eat opening '(' of record */
    ArUnGetch(c, fiP);
    free(new);
    return(NULL);  /* end of chart data, return NULL */
  }
  c = ArGetchNW(fiP);               /* eat following 'c' */
  for(i=0; i<ID_LEN; i++) {
    if((c = ArGetchNW(fiP)) == ',') 
      break;
    id[i] = c;
  }
  id[i] = '\0';
  if(strcmp(id, "v") == 0)
    new->id = v;
  else if(strcmp(id, "t") == 0)
    new->id = t;
  else if(strcmp(id, "f") == 0)
    new->id = f;
  else if(strcmp(id, "g") == 0)
    new->id = g;
  else if(strcmp(id, "h") == 0)
    new->id = h;
  else if(strcmp(id, "j") == 0)
    new->id = j;
  else if(strcmp(id, "m") == 0)
    new->id = m;
  else if(strcmp(id, "b") == 0)
    new->id = b;
  else if(strcmp(id, "w") == 0)
    new->id = w;
  else if(strcmp(id, "l") == 0)
    new->id = l;
  else if(strcmp(id, "lo") == 0)
    new->id = lo;
  else if(strcmp(id, "x") == 0)
    new->id = x;
  else if(strcmp(id, "y") == 0)
    new->id = y;
  else if(strcmp(id, "p") == 0)
    new->id = p;
  else if(strcmp(id, "r") == 0)
    new->id = r;
  else if(strcmp(id, "D") == 0)
    new->id = D;
  else if(strcmp(id, "Dx") == 0)
    new->id = Dx;
  else if(strcmp(id, "F") == 0)
    new->id = F;
  else if(strcmp(id, "G") == 0)
    new->id = G;
  else if(strcmp(id, "C") == 0)
    new->id = C;
  else if(strcmp(id, "P") == 0)
    new->id = P;
  else if(strcmp(id, "T") == 0)
    new->id = T;
  else if(strcmp(id, "L") == 0)
    new->id = L;
  else if(strcmp(id, "O") == 0)
    new->id = O;
  else if(strcmp(id, "E") == 0)
    new->id = E;
  else if(strcmp(id, "W") == 0)
    new->id = W;
  else
    return(NULL);

  /* read the fields */
  field_no = 0;
  do {
    field_no++;
    /** allocate a field structure & initialize it **/
    if((new_field = (CtField_t *)MALLOC(sizeof(CtField_t))) == NULL)
      return(NULL);
    new_field->next = NULL;

    /** fill it in with a value or null for an empty field **/
    if((c = ArGetchNW(fiP)) == ',' || c == ')') {
      new_field->value = NULL;                     /* empty field */
    }
    else {                                         /* valid field */
      ArUnGetch(c, fiP);
      new_field->value = get_ct_value(fiP, new->id, field_no);
      c = ArGetchNW(fiP);
    }

    /** add it to the list of field in the record **/
    if(last_field == NULL)
      new->field = new_field;
    else
      last_field->next = new_field;
    last_field = new_field;
  } while(c != ')');
  
  return(new);
}

CtValue_t *get_ct_value(fiP, id, field_no)
File_info_t *fiP;
CtRecordId_e id;
int field_no;
{
  float fval;
  CtData_t cval;
  int inum;
  CtValue_t *new;
  shar c = 0;
  shar shr_buf[MAX_TEXT_S_LEN+1];       /* a shar buffer for label shrings  */
  shar *shr_ip;                         /* an index into it */
  shar *shr_limit;                      /* the end of shar buffer */
  shar last_c;
  long hex;
  
  /* read and store value according to type, don't eat following ',' or ')' */
  if((new = (CtValue_t *)MALLOC(sizeof(CtValue_t))) == NULL)
    return(NULL);
  switch(id) {
  case D:                                          /* its a float OR a shrig */
    if(field_no == 1) {   /* saves the first field of 'D' which is different */
      shr_ip = shr_buf;
      last_c = 0;
      for(shr_limit = shr_ip+MAX_TEXT_S_LEN; shr_ip < shr_limit; shr_ip++) {
	c = ArGetch(fiP);
	if(c == '\n') {                  /* skip NL and indent in IAM */
	  c = ArGetchNW(fiP);
	}
	if((c == 'X' || c == 'x') && last_c == '\\') { /* process a hex char */
	  if(c == 'X') 
	    ArGetHex(fiP, &hex, 4);
	  else 
	    ArGetHex(fiP, &hex, 2);
	  c = hex;
	}
	if(c == ',' && last_c != '\\') { /* normal break point */
	  break;
	}
	*shr_ip = c;
	last_c = c;
      }
      *shr_ip = '\0';
      ArUnGetch(c, fiP);   /* put back trailing ',' */
      new->val.pval = (shar *)MALLOC(sizeof(shar) * (ArShrlen(shr_buf)+1));
      ArShrcpy(new->val.pval, shr_buf);
      break;
    }
    else {                /* this saves all other 'D' fields as float values */
      /* fall through to Dx */
    }
  case r:
  case Dx:                                         /* its a VALUE */
    if(ArGetDouble(fiP, &(cval.val), &(cval.dig)) == NO)  
      return(NULL);
    new->val.cval = cval;
    break;
  case h:
  case lo:
    if(ArGetFloat(fiP, &fval) == NO)               /* its a float */
      return(NULL);
    new->val.fval = fval;
    break;
  default:
    if(ArGetInt(fiP, &inum, NO, NO) == NO)         /* its an integer */
      return(NULL);
    new->val.ival = inum;
  }
  return(new);
}
	
/*************************************************************************
      Error routines:   These need beefing up !! ie:
                  1) don't just give up on an object, remove its stub (DONE)
                  2) check object version numbers                     (NOT DONE)
**************************************************************************/

/***************************************************************************
  Report diagram error, skip to closing '>' of current diagram.
  If no closing '>' found, report fatal error and terminate program.
***************************************************************************/  
void ArSkipDg(fiP, error)
File_info_t *fiP;
int error;
{
  shar c, last_c;
  char *msg;
 
  switch(error) {
  case ERR_MEMORY:
    msg = "memory allocation failure";
    break;
  case ERR_VERSION:
    msg = "incorrect diagramming version";
    break;
  case ERR_FORMAT:
    msg = "error in diagramming format";
    break;
  case ERR_TYPE:
    msg = "unknown diagramming object";
    break;
  default:
    msg = "unknown error";
    break;
  }
  fprintf(stderr, "ArSaveDg: %s, diagram skipped.\n", msg);

  last_c = 0;
  while((c = ArGetchNW(fiP)) != EOF) {
    if(c == '>' && last_c == ')') {
      ArUnGetch(c, fiP);  /* put back closing '>' of frame */
      return;
    }
    last_c = c;
  }
  fprintf(stderr, "ArSaveDg: fatal error reading diagram, program terminated.\n");
  exit(-25);
}

/**************************************************************************
  Report error, skip past closing ')' of curent object.
  If no closing ')' found, report fatal error and terminate program.
**************************************************************************/
void ArSkipObject(fiP, type)
File_info_t *fiP;
ArDgObj_type type;
{
  shar c, last_c = 0;
  
  ArFprintf(stderr, "ArSaveDg: error in object type %#C, object skipped\n", type);
  while((c = ArGetchNW(fiP)) != EOF) {
    if(c == ')' && last_c != '\\')
      return;
    last_c = c;
  }
  fprintf(stderr, "ArSaveDg: fatal error reading diagram, program terminated.\n");
  exit(-26);
}

