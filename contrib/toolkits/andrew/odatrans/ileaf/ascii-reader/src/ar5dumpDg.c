
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
/*	ar5dumpDg.c                              */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    CK	Original coding
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

/*************************************************************************
  This is the top level diagram output routine. It takes a pointer to a
  top level diagram structure, an ArDgHead_t *, and a pointer to the file
  you wish the output written to. It returns 0 for no error or nonzero for
  some type of error. It emits the diagram starting with the opening '='and
  ends with the master-group's closing ')'.
*************************************************************************/
int ArEmitDg(ofp, dgP)
FILE *ofp;
ArDgHead_t *dgP;
{
  int level, pos;
  
  level = pos = 0;
  ArFprintf(ofp, " =\n");
  ArFprintf(ofp, "V%d,\n", dgP->version);
  return(ArEmitObject(ofp, dgP->contents, level, &pos));
}

/**************************************************************************
  The top object-level diagram output routine. It emits the common portion
  of the diagram and calls the object-specific routines to dump the
  remainder. Note that some of the object-specific routines call
  ArEmitObject() recursively to dump their contents.
***************************************************************************/
int ArEmitObject(ofp, obj, level, pos)
FILE *ofp;  
ArDgObject_t *obj;
int level;
int *pos;
{
  char s[MAX_S_LEN], indent[MAX_NESTING+1];
  int i, result = OK;

  /* compute indent */
  for(i=0; i<level && i<MAX_NESTING; i++)
    indent[i] = ' ';
  indent[i] = '\0';

  /* write the common object data */
  sprintf(s,
	  "%s(%c%d,%d,%ld",
	  indent,
	  obj->type,
	  obj->version,
	  obj->z_coord,
	  obj->flags);
  result = ArDgputs(ofp, s, level, pos);
  if(obj->type == 'g') {
    if(obj->info != NULL) {
      fprintf(ofp, "\n");
      *pos = 0;
    }
  }
  else {
    ArDgputs(ofp, ",", level, pos);
  }

  /* write the object specific data */
  switch(obj->type) {
  case dg_GROUP:
    result = ArEmitGroupInfo(ofp, (ArDgObject_t *)obj->info, level, pos);
    break;
  case dg_LINE:
    result = ArEmitLineInfo(ofp, (LineTSD_t *)obj->info, level, pos);
    break;
  case dg_EDIT_STATE:
    result = ArEmitEditInfo(ofp, (EditStateTSD_t *)obj->info, level, pos);
    break;
  case dg_POLY:
    result = ArEmitPolyInfo(ofp, (PolyTSD_t *)obj->info, level, pos);
    break;
  case dg_ARC:
    result = ArEmitArcInfo(ofp, (ArcTSD_t *)obj->info, level, pos);
    break;
  case dg_SPLINE:
    result = ArEmitSplineInfo(ofp, (SplineTSD_t *)obj->info, level, pos);
    break;
  case dg_RELL:
    result = ArEmitEllipseInfo(ofp, (EllipseTSD_t *)obj->info, level, pos);
    break;
  case dg_IMAGE:
    result = ArEmitImageInfo(ofp, (ImageTSD_t *)obj->info, level, pos);
    break;
  case dg_TEXTSTR:
    result = ArEmitTextstrInfo(ofp, (TextstrTSD_t *)obj->info, level, pos);
    break;
  case dg_BOX:
    result = ArEmitBoxInfo(ofp, (BoxTSD_t *)obj->info, level, pos);
    break;
  case dg_EQN:
    result = ArEmitEqnInfo(ofp, (EqnTSD_t *)obj->info, level, pos);
    break;
  case dg_MICRODOC:
    result = ArEmitMicroInfo(ofp, (MicroTSD_t *)obj->info, level, pos);
    break;
  case dg_SPLINE_SEG:
    result = ArEmitSplineSegInfo(ofp, (SplineSegTSD_t *)obj->info, level, pos);
    break;
  case dg_VECTORS:
    result = ArEmitVectorListInfo(ofp, (VectorListTSD_t *)obj->info, level, pos);
    break;
  case dg_OUTLINE:
    result = ArEmitOutlineInfo(ofp, (OutlineTSD_t *)obj->info, level, pos);
    break;
  case dg_CHART:
    result = ArEmitChartInfo(ofp, (ChartTSD_t *)obj->info, level, pos);
    break;
  case dg_EPS:
    result = ArEmitEpsInfo(ofp, (EpsTSD_t *)obj->info, level, pos);
    break;
  /*
   * etc, etc ...
   */
  default:
    return(ERROR);
  }

  ArDgputs(ofp, ")", level, pos);
  if(obj->next) {
    fprintf(ofp, "\n");
    *pos = 0;
  }
  return(result);
}

/*************************************************************************
   The following routines dump the individual diagramming objects.
   Some, like those for the group and poly, are recursive, most are
   not.
**************************************************************************/
   
int ArEmitGroupInfo(ofp, info, level, pos)
FILE *ofp;
ArDgObject_t *info;
int level;
int *pos;
{
  ArDgObject_t *start = NULL;
  
  for(start=info; start; start=start->next) {
    if(ArEmitObject(ofp, start, level+1, pos) != OK)
      return(ERROR);
  }
  return(OK);
}

/*------------------------------------------------------------------------*/
int ArEmitPolyInfo(ofp, info, level, pos)
FILE *ofp;
PolyTSD_t *info;
int level;
int *pos;
{
  int result = OK;
  char s[MAX_S_LEN];
  
  sprintf(s,
	  "%d,%d,%d\n",
	  info->fpat,
	  info->fcolor,
	  info->ftrans);
  result = ArDgputs(ofp, s, level, pos);
  if(ArEmitGroupInfo(ofp, info->contents, level+1, pos) != OK)
    return(ERROR);
  return(result);
}
  
/*------------------------------------------------------------------------*/
int ArEmitLineInfo(ofp, info, level, pos)
FILE *ofp;
LineTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];

  sprintf(s,
	  "%f,%f,%f,%f,%d,%d,%d,%d",
	  info->x1,
	  info->y1,
	  info->x2,
	  info->y2,
	  info->color,
	  info->trans,
	  info->width,
	  info->pattern);
  return(ArDgputs(ofp, s, level, pos));
}

/*------------------------------------------------------------------------*/
int ArEmitEditInfo(ofp, info, level, pos)
FILE *ofp;
EditStateTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];
  int result = 0;

  /* the EDITSTATE portion */
  sprintf(s,
	  "%d,%d,%d,%f,%d,%f,%d,%d,%f,%f,%f,%d,%f,%d,%d,%d,%d,%f,%d,",
	  info->fill,
	  info->width,
	  info->gravity,
	  info->gravrad,
	  info->detent,
	  info->angle,
	  info->pattern,
	  info->zoom,
	  info->zoomlev,
	  info->xshift,
	  info->yshift,
	  info->scale,
	  info->scalefact,
	  info->fcolor,
	  info->ftrans,
	  info->ecolor,
	  info->etrans,
	  info->textangle,
	  info->tcolor);
  result = ArDgputs(ofp, s, level, pos);
  
  /* the GRID portion */
  sprintf(s,
	  "%d,%d,%d,%f,%f,%d,%d,%d,%f,%d",
	  info->display,
	  info->align,
	  info->ontop,
	  info->minw,
	  info->minh,
	  info->majw,
	  info->majh,
	  info->type,
	  info->mini,
	  info->maji);
  result += ArDgputs(ofp, s, level, pos);
  return(result);
}

/*------------------------------------------------------------------------*/
int ArEmitArcInfo(ofp, info, level, pos)
FILE *ofp;
ArcTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];

  sprintf(s,
	  "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d",
	  info->format,
	  info->btx,
	  info->bty,
	  info->bx,
	  info->by,
	  info->mx,
	  info->my,
	  info->ex,
	  info->ey,
	  info->tex,
	  info->tey,
	  info->x,
	  info->y,
	  info->fpattern,
	  info->fcolor,
	  info->ftrans,
	  info->bcolor,
	  info->btrans,
	  info->bwidth,
	  info->bpattern);
  return(ArDgputs(ofp, s, level, pos));
}

/*------------------------------------------------------------------------*/
int ArEmitEllipseInfo(ofp, info, level, pos)
FILE *ofp;
EllipseTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];

  sprintf(s,
	  "%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d",
	  info->ox,
	  info->oy,
	  info->t1x,
	  info->t1y,
	  info->t2x,
	  info->t2y,
	  info->fcolor,
	  info->ftrans,
	  info->fpattern,
	  info->bcolor,
	  info->btrans,
	  info->bwidth,
	  info->bpattern);
  return(ArDgputs(ofp, s, level, pos));
}

/*------------------------------------------------------------------------*/
int ArEmitSplineInfo(ofp, info, level, pos)
FILE *ofp;
SplineTSD_t *info;
int level;
int *pos;
{
  int result = OK;
  KNOT_t *start;
  char s[MAX_S_LEN];
  
  sprintf(s,
	  "%f,%f,%d,%d,%d,%d,",
	  info->x,
	  info->y,
	  info->fpattern,
	  info->fcolor,
	  info->ftrans,
	  info->number_of_knots);
  result = ArDgputs(ofp, s, level, pos);

  for(start=info->knot_list; start; start=start->next) {
    sprintf(s,
	    "%f,%f,%d,%d,%d,%d,",
	    start->x,
	    start->y,
	    start->color,
	    start->trans,
	    start->width,
	    start->pattern);
    result += ArDgputs(ofp, s, level, pos);
  }

  sprintf(s,
	  "%c",
	  info->type);
  result += ArDgputs(ofp, s, level, pos);
  return(result);
}

/*------------------------------------------------------------------------*/
int  ArEmitImageInfo(ofp, info, level, pos)
FILE *ofp;
ImageTSD_t *info;
int level;
int *pos;
{
  int result = OK;
  BREAK_t *start;
  char s[MAX_S_LEN];
  
  sprintf(s,
	  "%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,",
	  info->xo,
	  info->yo,
	  info->xh,
	  info->yh,
	  info->xv,
	  info->yv,
	  info->cx1,
	  info->cy1,
	  info->cx2,
	  info->cy2,
	  info->num_breaks);
  result = ArDgputs(ofp, s, level, pos);

  for(start=info->break_list; start; start=start->next) {
    sprintf(s,
	    "%f,%f,",
	    start->x,
	    start->y);
    result += ArDgputs(ofp, s, level, pos);
  }

  sprintf(s,
	  "%d,%d,%d,%f,%f,",
	  info->color,
	  info->trans,
	  info->submenu,
	  info->origw,
	  info->origh);
  result += ArDgputs(ofp, s, level, pos);

  result += ArEmitRasterInfo(ofp, info->orig, level, pos);
  result += ArDgputs(ofp, ",", level, pos);
  result += ArEmitRasterInfo(ofp, info->disp, level, pos);
  return(result);
}

/*------------------------------------------------------------------------*/
int ArEmitRasterInfo(ofp, raster, level, pos)
FILE *ofp;
RASTER_t *raster;
int level;
int *pos;
{
  char s[MAX_S_LEN];
  int result = OK;

  /* if reference only, just dump id */
  if(raster->reference == YES) {
    sprintf(s,
	    "%ld",
	    raster->id);
    return(ArDgputs(ofp, s, level, pos));
  }

  /* otherwise, dump entire raster */
  sprintf(s,
	  "%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d",
	  raster->id,
	  raster->xh,
	  raster->yh,
	  raster->xv,
	  raster->yv,
	  raster->x1,
	  raster->y1,
	  raster->x2,
	  raster->y2,
	  raster->map_size);
  result = ArDgputs(ofp, s, level, pos);

  /* emit map data as hex set, if any */
  if(raster->map_size)
    result += ArEmitHexInfo(ofp, raster->map, level+1, pos);
  
  /* continue with raster info... */
  sprintf(s,
	  ",%d,%d",
	  raster->screenx,
	  raster->screeny);
  result += ArDgputs(ofp, s, level, pos);

  /* emit screen data as hec set, if any */
  if(raster->screenx*raster->screeny)
    result += ArEmitHexInfo(ofp, raster->screen, level+1, pos);
  
  /* continue with raster info... */
  sprintf(s,
	  ",%d,%d,%d,%d,%ld",
	  raster->w,
	  raster->h,
	  raster->d,
	  raster->format,
	  raster->size);
  result += ArDgputs(ofp, s, level, pos);

  /* finally, emit main raster data as hex set */
  result += ArEmitHexInfo(ofp, raster->raster, level+1, pos);
  return(result);
}

/*------------------------------------------------------------------------*/
int ArEmitHexInfo(ofp, hex, level, pos)
FILE *ofp;
HEX_t *hex;
int level;
int *pos;
{
  char s[MAX_S_LEN], indent[MAX_NESTING+1];
  int result = OK;
  unsigned char *ip;
  register i;

  /* compute indent */
  for(i=0; i<level && i<MAX_NESTING; i++)
    indent[i] = ' ';
  indent[i] = '\0';

  /* start hex set object */
  *pos = 0;
  sprintf(s,
	  "\n%s(X0,%ld,",
	  indent,
	  hex->size);
  result = ArDgputs(ofp, s, level, pos);

  /* print the hex data */
  ip = hex->data;
  for(i=0; i<hex->size; i++) {
    sprintf(s,
	    "%02X",
	    *ip);
    ip++;
    result += ArDgputs(ofp, s, level, pos);
  }

  /* close the hex set */
  result += ArDgputs(ofp, ")", level, pos);
  return(result);
}

/*---------------------------------------------------------------------------*/
int ArEmitTextstrInfo(ofp, info, level, pos)
FILE *ofp;
TextstrTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN], temp[10];
  int result = OK;
  ushar *ip;
  
  /* write non-text portion */
  sprintf(s,
	  "%f,%f,%d,%d,%d,%f,%s,%s,",
	  info->x,
	  info->y,
	  info->anchor,
	  info->color,
	  info->trans,
	  info->angle,
	  info->attrib,
	  info->font);
  result = ArDgputs(ofp, s, level, pos);

  /* write text portion, do hex representation if necessary */
  s[0] = '\0';
  for(ip = (ushar *)info->text; *ip; ip++) {
    if(*ip > 0x7e || *ip < 0x20) {
      if(*ip > 0xff)
	sprintf(temp, "X%04x", *ip);
      else
	sprintf(temp, "x%x", *ip);
    }
    else if(*ip == '\\') {
      strcpy(temp, "\\");
    }
    else {
      sprintf(temp, "%c", *ip);
    }
    strcat(s, temp);
  }
  
  result += ArDgputs(ofp, s, level, pos);
  return(result);
}

/*-------------------------------------------------------------------------------*/
int ArEmitBoxInfo(ofp, info, level, pos)
FILE *ofp;
BoxTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];

  sprintf(s,
	  "%f,%f,%f,%f,%d,%d,%d,%d",
	  info->x1,
	  info->y1,
	  info->x2,
	  info->y2,
	  info->texture,
	  info->border_texture,
	  info->border_width,
	  info->border_pattern);
  return(ArDgputs(ofp, s, level, pos));
}

/*------------------------------------------------------------------------------*/
int ArEmitEqnInfo(ofp, info, level, pos)
FILE *ofp;
EqnTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN], temp[10];
  int result = OK;
  ushar *ip;
  

  sprintf(s,
	  "%f,%f,",
	  info->x,
	  info->y);
  result = ArDgputs(ofp, s, level, pos);
  
  sprintf(s,
	  "%s,",
	  info->font);
  result += ArDgputs(ofp, s, level, pos);

  /* write text portion, do hex representation if necessary */
  s[0] = '\0';
  for(ip = (ushar *)info->text; *ip; ip++) {
    if(*ip > 0x7e || *ip < 0x20) {
      if(*ip > 0xff)
	sprintf(temp, "X%04x", *ip);
      else
	sprintf(temp, "x%x", *ip);
    }
    else if(*ip == '\\') {
      strcpy(temp, "\\");
    }
    else {
      sprintf(temp, "%c", *ip);
    }
    strcat(s, temp);
  }
  result += ArDgputs(ofp, s, level, pos);
  
  return(result);
}

/*----------------------------------------------------------------------------*/
int ArEmitMicroInfo(ofp, info, level, pos)
FILE *ofp;
MicroTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];
  int result = OK;

  sprintf(s,
	  "%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,\n",
	  info->x,
	  info->y,
	  info->bcolor,
	  info->btrans,
	  info->bpat,
	  info->lcolor,
	  info->ltrans,
	  info->width,
	  info->pattern,
	  info->flags);
  result = ArDgputs(ofp, s, level, pos);
  *pos = 0;  /* adjust "pos" after a NL */

  ArMicrodocWrite(info->text, ofp);

  return(result);
}

/*---------------------------------------------------------------------------*/
int ArEmitSplineSegInfo(ofp, info, level, pos)
FILE *ofp;
SplineSegTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];

  sprintf(s,
	  "%f,%f,%f,%f,%d,%d,%d,%d",
	  info->x1,
	  info->y1,
	  info->x2,
	  info->y2,
	  info->color,
	  info->trans,
	  info->width,
	  info->pattern);
  return(ArDgputs(ofp, s, level, pos));
}

/*---------------------------------------------------------------------------*/
int ArEmitVectorListInfo(ofp, info, level, pos)
FILE *ofp;
VectorListTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];
  int result = OK;
  HexList_t *start;

  sprintf(s,
	  "%d,%d,%f,%f,%f,%f,%f,%f,",
	  info->color,
	  info->trans,
	  info->xo,
	  info->yo,
	  info->xh,
	  info->yh,
	  info->xv,
	  info->yv);
  result = ArDgputs(ofp, s, level, pos);

  for(start = info->list; start; start = start->next) {
    sprintf(s,
	    "(X0,2,%04lX)",
	    start->no_pairs);
    *pos = OUTPUT_LINE_LENGTH;  /* this forces a NL in output */
    result = ArDgputs(ofp, s, level+1, pos);
    *pos = 0;  /* reset position after a NL */
    result += ArEmitHexInfo(ofp, start->data, level+2, pos);
  }

  sprintf(s,
	  "(X0,2,0000)");
  *pos = OUTPUT_LINE_LENGTH;  /* this forces a NL in output */
  result += ArDgputs(ofp, s, level+1, pos);
  *pos = 0;    /* reset position after NL */
  return(result);
}

/*---------------------------------------------------------------------------*/
int ArEmitEpsInfo(ofp, info, level, pos)

   FILE *ofp;
   EpsTSD_t *info;
   int level;
   int *pos;

{
  char s[MAX_S_LEN];
  int result = OK;
  HexList_t *start;
  short		i;
  unsigned char	*dptr;
  char indent[MAX_NESTING+1];

  /* compute indent */
  for ( i = 0; i <= level && i < MAX_NESTING; i++ ){
    indent[i] = ' ';
  }
  indent[i] = '\0';

  sprintf(s,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
	  info->xo,
	  info->yo,
	  info->xh,
	  info->yh,
	  info->xv,
	  info->yv,
	  info -> llx,
	  info -> lly,
	  info -> urx,
	  info -> ury);
  result = ArDgputs(ofp, s, level, pos);

  *pos = OUTPUT_LINE_LENGTH;  /* this forces a NL in output */

  if ( ArEmitObject(ofp, info -> display_obj, level+1, pos) != OK ){
     return(ERROR);
  }

  for(start = info->list; start != NULL; start = start->next) {
    sprintf(s,"(X0,2,%04lX)",start->no_pairs);
    *pos = OUTPUT_LINE_LENGTH;  /* this forces a NL in output */
    result = ArDgputs(ofp, s, level+1, pos);
    (*pos)++;
    for ( i = 0,dptr = start -> data -> data; i < start -> no_pairs; i++ ){
       (*pos)++;
       if ( *pos >= OUTPUT_LINE_LENGTH ){
         *pos = level + 1;
         fprintf(ofp,"\n%s",indent);
       }
       if ( *dptr < 0x20 || *dptr >= 0x7f ){
	  fputc('\\',ofp);
	  fputc(((*dptr >> 6) & 3) + '0',ofp);
	  fputc(((*dptr >> 3) & 7) + '0',ofp);
	  fputc((*dptr++ & 7) + '0',ofp);
	  *pos += 3;
       }else{
	  if ( *dptr == ' ' || *dptr == '(' || *dptr == ')' ||
	       *dptr == '\\' || *dptr == ',' || *dptr == '<' || *dptr == '>' ){
             (*pos)++;
	     fputc('\\',ofp);
	  }
          if ( fputc(*dptr++,ofp) == EOF ){
            return(ERROR);
          }
       }
    }
    fputc(',',ofp);
  }
  sprintf(s,"(X0,2,0000)");
  *pos = OUTPUT_LINE_LENGTH;  /* this forces a NL in output */
  result += ArDgputs(ofp, s, level+1, pos);
  return(result);
}

/*--------------------------------------------------------------------------*/
int ArEmitOutlineInfo(ofp, info, level, pos)
FILE *ofp;
OutlineTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];
  int result = OK;

  sprintf(s,
	  "%ld,%s,%d",
	  info->key,
	  info->font,
	  info->character);
  result = ArDgputs(ofp, s, level, pos);

  *pos = OUTPUT_LINE_LENGTH;  /* force a NL before encrypted data */
  result += ArDgputs(ofp, info->encrypted_data, level+1, pos);

  return(result);
}

/*--------------------------------------------------------------------------*/
int ArEmitChartInfo(ofp, info, level, pos)
FILE *ofp;
ChartTSD_t *info;
int level;
int *pos;
{
  char s[MAX_S_LEN];
  char id_s[ID_LEN+1];
  int result = OK;
  CtRecord_t *start_rec;
  CtField_t *start_field;
  char *get_id_str();
  int field_no;
  shar *ip;
  char temp[128];
  
  sprintf(s,
	  "%f,%f,%f,%f,%s,%02d",
	  info->x1,
	  info->y1,
	  info->x2,
	  info->y2,
	  info->font,
	  info->version);
  result = ArDgputs(ofp, s, level, pos);

  for(start_rec=info->record; start_rec; start_rec=start_rec->next) {
    *pos = OUTPUT_LINE_LENGTH; /* force a NL */
    sprintf(s, "(c%s", get_id_str(start_rec->id, id_s));
    result += ArDgputs(ofp, s, level+1,pos);
    field_no = 0;
    for(start_field=start_rec->field; start_field; start_field=start_field->next) {
      field_no++;
      if(start_field->value == NULL) {  /* an empty field */
	result += ArDgputs(ofp, ",", level+1, pos);
      }
      else {
	switch(start_rec->id) {
	case D:
	  if(field_no == 1) {  	    /* write text, do hex representation if needed */
	    s[0] = ',';
	    s[1] = '\0';
	    for(ip = start_field->value->val.pval; *ip; ip++) {
	      if(*ip > 0x7e || *ip < 0x20) {
		if(*ip > 0xff)
		  sprintf(temp, "X%04x", *ip);
		else
		  sprintf(temp, "x%x", *ip);
	      }
	      else if(*ip == '\\') {
		strcpy(temp, "\\");
	      }
	      else {
		sprintf(temp, "%c", *ip);
	      }
	      strcat(s, temp);
	    }
	    break;
	  }
	  else {	          /* write data fields */
	    /* fall through to 'Dx' */
	  }
	case r:
	case Dx: {
	    char cbuf[32], fmtbuf[64]; int dig;
	    sprintf(s,",");
	    if ( (dig=start_field->value->val.cval.dig) >= 0  ) {
		sprintf(fmtbuf,"%%.%df", dig);
		sprintf(cbuf, fmtbuf, start_field->value->val.cval.val); 
		strcat(s,cbuf);
	    }
	}
	  break;
	case h:
	case lo:
	    sprintf(s, ",%f", start_field->value->val.fval);
	  break;
	default:
	  sprintf(s, ",%d", start_field->value->val.ival);
	  break;
	}
	result += ArDgputs(ofp, s, level+1, pos);
      }
    }
    result += ArDgputs(ofp, ")", level+1, pos);
  }
  return(result);
}
  
char *get_id_str(id, s)  /* used to convert the "id" enum to a zstring */
CtRecordId_e	id;		/* M.Saunders 2/9/89 */
char *s;
{
  switch(id) {
  case v:
    strcpy(s, "v"); break;
  case t:
    strcpy(s, "t"); break;
  case f:
    strcpy(s, "f"); break;
  case g:
    strcpy(s, "g"); break;
  case h:
    strcpy(s, "h"); break;
  case j:
    strcpy(s, "j"); break;
  case m:
    strcpy(s, "m"); break;
  case b:
    strcpy(s, "b"); break;
  case w:
    strcpy(s, "w"); break;
  case l:
    strcpy(s, "l"); break;
  case lo:
    strcpy(s, "lo"); break;
  case x:
    strcpy(s, "x"); break;
  case y:
    strcpy(s, "y"); break;
  case p:
    strcpy(s, "p"); break;
  case r:
    strcpy(s, "r"); break;
  case D:
    strcpy(s, "D"); break;
  case Dx:
    strcpy(s, "Dx"); break;
  case F:
    strcpy(s, "F"); break;
  case G:
    strcpy(s, "G"); break;
  case C:
    strcpy(s, "C"); break;
  case P:
    strcpy(s, "P"); break;
  case T:
    strcpy(s, "T"); break;
  case L:
    strcpy(s, "L"); break;
  case O:
    strcpy(s, "O"); break;
  case E:
    strcpy(s, "E"); break;
  case W:
    strcpy(s, "W"); break;
  }
  return(s);
}
     
/****************************************************************************
  Output formatter: gives Interleaf-style indentation to outgoing Dg info.
*****************************************************************************/
int ArDgputs(ofp, s, level, pos)
FILE *ofp;
char *s;
int level;
int *pos;
{
  char indent[MAX_NESTING+1];
  register i;

  /* compute indent */
  for(i=0; i<level && i<MAX_NESTING; i++)
    indent[i] = ' ';
  indent[i] = '\0';

  /* format output */
  for(i=0; s[i] != '\0'; i++) {
    (*pos)++;
    if(*pos >= OUTPUT_LINE_LENGTH) {
      *pos = 0;
      *pos += level;
      fprintf(ofp, "\n%s ", indent);
    }
    if(fputc(s[i], ofp) == EOF)
      return(ERROR);
  }
  return(OK);
}
