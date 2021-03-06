#if !defined(lint) && !defined(SABER)
static char rcs_id[] =
    "$XConsortium: tsource.c,v 2.19 89/12/16 03:35:39 converse Exp $";
#endif
/*
 *			  COPYRIGHT 1987
 *		   DIGITAL EQUIPMENT CORPORATION
 *		       MAYNARD, MASSACHUSETTS
 *			ALL RIGHTS RESERVED.
 *
 * THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT NOTICE AND
 * SHOULD NOT BE CONSTRUED AS A COMMITMENT BY DIGITAL EQUIPMENT CORPORATION.
 * DIGITAL MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR
 * ANY PURPOSE.  IT IS SUPPLIED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 *
 * IF THE SOFTWARE IS MODIFIED IN A MANNER CREATING DERIVATIVE COPYRIGHT RIGHTS,
 * APPROPRIATE LEGENDS MAY BE PLACED ON THE DERIVATIVE WORK IN ADDITION TO THAT
 * SET FORTH ABOVE.
 *
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting documentation,
 * and that the name of Digital Equipment Corporation not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 */

/* File: tsource.c -- the code for a toc source */

#include "xmh.h"
#include "tocintrnl.h"
#include <X11/Xatom.h>
#include "tsourceP.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(TocSourceWidget, toc_source.field)
static XtResource resources[] = {
    {XtNtoc, XtCToc, XtRPointer, sizeof(caddr_t), 
       offset(toc), XtRPointer, NULL},
};

static void Initialize();
static XawTextPosition Read(), Scan(), Search();
static int Replace();

#define SuperClass		(&textSrcClassRec)
TocSourceClassRec tocSourceClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) SuperClass,
    /* class_name	  	*/	"TocSrc",
    /* widget_size	  	*/	sizeof(TocSourceRec),
    /* class_initialize   	*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	NULL,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	FALSE,
    /* compress_exposure  	*/	FALSE,
    /* compress_enterleave	*/	FALSE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	NULL,
    /* resize		  	*/	NULL,
    /* expose		  	*/	NULL,
    /* set_values	  	*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	NULL,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	NULL,
    /* extension		*/	NULL
  },
/* textSrc_class fields */
  {
    /* Read                     */      Read,
    /* Replace                  */      Replace,
    /* Scan                     */      Scan,
    /* Search                   */      Search,
    /* SetSelection             */      XtInheritSetSelection,
    /* ConvertSelection         */      XtInheritConvertSelection
  },
/* toc_source_class fields */
  {
    /* keeping the compiler happy. */   NULL
  }
};

WidgetClass tocSourceWidgetClass = (WidgetClass)&tocSourceClassRec;

/************************************************************
 *
 * Class specific methods.
 *
 ************************************************************/

Msg MsgFromPosition(toc, position, dir)
  Toc toc;
  XawTextPosition position;
  XawTextScanDirection dir;
{
    Msg msg;
    int     h, l, m;
    if (position > toc->lastPos) position = toc->lastPos;
    if (dir == XawsdLeft) position--;
    l = 0;
    h = toc->nummsgs - 1;
    while (l < h - 1) {
	m = (l + h) / 2;
	if (toc->msgs[m]->position > position)
	    h = m;
	else
	    l = m;
    }
    msg = toc->msgs[h];
    if (msg->position > position)
	msg = toc->msgs[h = l];
    while (!msg->visible)
	msg = toc->msgs[h++];
    if (position < msg->position || position > msg->position + msg->length)
	Punt("Error in MsgFromPosition!");
    return msg;
}


static XawTextPosition CoerceToLegalPosition(toc, position)
  Toc toc;
  XawTextPosition position;
{
    return (position < 0) ? 0 :
		 ((position > toc->lastPos) ? toc->lastPos : position);
}

static XawTextPosition
Read(w, position, block, length)
Widget w;
XawTextPosition position;
XawTextBlock *block;
int length;
{
    TocSourceWidget source = (TocSourceWidget) w;
    Toc toc = source->toc_source.toc;
    Msg msg;
    int count;

    if (position < toc->lastPos) {
        block->firstPos = position;
	msg = MsgFromPosition(toc, position, XawsdRight);
	block->ptr = msg->buf + (position - msg->position);
	count = msg->length - (position - msg->position);
	block->length = (count < length) ? count : length;
	position += block->length;
    }
    else {
        block->firstPos = 0;
	block->length = 0;
	block->ptr = "";
    }
    block->format = FMT8BIT;
    return position;
}

/* Right now, we can only replace a piece with another piece of the same size,
   and it can't cross between lines. */

static int 
Replace(w, startPos, endPos, block)
Widget w;
XawTextPosition startPos, endPos;
XawTextBlock *block;
{
    TocSourceWidget source = (TocSourceWidget) w;
    Toc toc = source->toc_source.toc;
    Msg msg;
    int i;

    if (block->length != endPos - startPos)
	return XawEditError;
    msg = MsgFromPosition(toc, startPos, XawsdRight);
    for (i = 0; i < block->length; i++)
	msg->buf[startPos - msg->position + i] = block->ptr[i];
/*    for (i=0 ; i<toc->numwidgets ; i++)
	XawTextInvalidate(toc->widgets[i], startPos, endPos);
*
* CDP 9/1/89 
*/
    return XawEditDone;
}


#define Look(index, c)\
{									\
    if ((dir == XawsdLeft && index <= 0) ||				\
	    (dir == XawsdRight && index >= toc->lastPos))		\
	c = 0;								\
    else {								\
	if (index + doff < msg->position ||				\
		index + doff >= msg->position + msg->length)		\
	    msg = MsgFromPosition(toc, index, dir);			\
	c = msg->buf[index + doff - msg->position];			\
    }									\
}



static XawTextPosition 
Scan(w, position, sType, dir, count, include)
Widget w;
XawTextPosition position;
XawTextScanType sType;
XawTextScanDirection dir;
int count;
Boolean include;
{
    TocSourceWidget source = (TocSourceWidget) w;
    Toc toc = source->toc_source.toc;
    XawTextPosition index;
    Msg msg;
    char    c;
    int     ddir, doff, i, whiteSpace;
    ddir = (dir == XawsdRight) ? 1 : -1;
    doff = (dir == XawsdRight) ? 0 : -1;

    if (toc->lastPos == 0) return 0;
    index = position;
    if (index + doff < 0) return 0;
    if (dir == XawsdRight && index >= toc->lastPos) return toc->lastPos;
    msg = MsgFromPosition(toc, index, dir);
    switch (sType) {
	case XawstPositions:
	    if (!include && count > 0)
		count--;
	    index = CoerceToLegalPosition(toc, index + count * ddir);
	    break;
	case XawstWhiteSpace:
	    for (i = 0; i < count; i++) {
		whiteSpace = -1;
		while (index >= 0 && index <= toc->lastPos) {
		    Look(index, c);
		    if ((c == ' ') || (c == '\t') || (c == '\n')) {
			if (whiteSpace < 0) whiteSpace = index;
		    } else if (whiteSpace >= 0)
			break;
		    index += ddir;
		}
	    }
	    if (!include) {
		if (whiteSpace < 0 && dir == XawsdRight)
		    whiteSpace = toc->lastPos;
		index = whiteSpace;
	    }
	    index = CoerceToLegalPosition(toc, index);
	    break;
	case XawstEOL:
	case XawstParagraph:
	    for (i = 0; i < count; i++) {
		while (index >= 0 && index <= toc->lastPos) {
		    Look(index, c);
		    if (c == '\n')
			break;
		    index += ddir;
		}
		if (i < count - 1)
		    index += ddir;
	    }
	    if (include)
		index += ddir;
	    index = CoerceToLegalPosition(toc, index);
	    break;
	case XawstAll:
	    if (dir == XawsdLeft)
		index = 0;
	    else
		index = toc->lastPos;
	    break;
    }
    return index;
}
/*ARGSUSED*/
static XawTextPosition Search(w, position, direction, block)
Widget			w;
XawTextPosition		position;
XawTextScanDirection	direction;
XawTextBlock		*block;
{
    /* TocSourceWidget source = (TocSourceWidget) w;
     * Toc toc = source->toc_source.toc;
     * not implemented
     */
    return XawTextSearchError;
}

/* Public definitions. */

/* ARGSUSED*/
static void Initialize(request, new)
Widget request, new;
{
    Toc toc;
    TocSourceWidget source = (TocSourceWidget) new;

    source->text_source.edit_mode = XawtextRead; /* force read only. */

    toc = source->toc_source.toc;

    toc->hasselection = FALSE;
    toc->left = toc->right = 0;
}

void TSourceInvalid(toc, position, length)
    Toc toc;	
    XawTextPosition position;
    int length;
{
  XawTextInvalidate(XtParent(toc->source), position, 
		    (XawTextPosition) position+length-1);
}
