/* $Header: obj_text.c,v 1.5 89/07/02 15:59:03 moraes Exp $ */
/*
 *  The text object routines. It calls the minibuf to input the text
 */
#include "xpic.h"
#include "windows.h"
#include "gels.h"
#include "input.h"
#include "newfonts.h"
#include "draw.h"
#include "assert.h"

static int textWidth, textHeight;
static char *textString;
static int textLen;
static int deltaX, deltaY;
static int textX, textY;

text_event(evtype, mx, my)
{
	switch (evtype) {
	case MOTION | START_MODE:
	case RIGHT  | START_MODE:
	case MIDDLE | START_MODE:
	case REDRAW | START_MODE:
	case RIGHT  | END_MODE:
		break;
	case MOTION | END_MODE:
		drawtext(picWin, textX, textY, textString, textLen, gcInvert, spacePad);
		textX = mx + deltaX;
		textY = my + deltaY;
		drawtext(picWin, textX, textY, textString, textLen, gcInvert, spacePad);
		break;
	case LEFT | START_MODE:
		if ((textString = get_input("? ", "", FALSE)) == NULL)
			break;
		if (fontChanged) {
			changefont();
		}
		textLen = strlen(textString);
		drawingMode = END_MODE;
		/* Put the text on the mouse */
		ComputeTextPosition(textString, textLen, textFont, &deltaX, &deltaY,
		 &textWidth, &textHeight, textHorizAlign, textVertAlign, spacePad);
		textX = mx + deltaX;
		textY = my + deltaY;
		drawtext(picWin, textX, textY, textString, textLen, gcInvert, spacePad);
		break;
	case LEFT | END_MODE:
		drawtext(picWin, textX, textY, textString, textLen, gcInvert, spacePad);
		textX = mx + deltaX;
		textY = my + deltaY;
		drawtext(picWin, textX, textY, textString, textLen, gcNormal, spacePad);
		/* !! The calculation of the bounding box is sloppy */
		AddTextGel(&(CurrentCell->gelList), textString, textLen, 
		 fontType, fontType->cursize, fontType->name, textSize, 
		 textVertAlign | textHorizAlign, 
		 mx, my, textX, textY - textHeight, textX + textWidth, textY);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = NULL;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case MIDDLE | END_MODE:
		drawtext(picWin, textX, textY, textString, textLen, gcInvert, spacePad);
		drawingMode = START_MODE;
		break;
	case REDRAW | END_MODE:
		drawtext(picWin, textX, textY, textString, textLen, gcInvert, spacePad);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown TEXT mode %d", drawingMode);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "text_event");
}


text_abort()
{
	text_event((MIDDLE  | drawingMode), 0, 0);
}
	
/*
 *  Computes the position of a given string, in a given font, relative to
 *  the current mouse position. This is computed from 'vertalign'
 *  'horizalign', which are global variables, set by the user from
 *  buttons.
 */
ComputeTextPosition(s, len, font, dx, dy, width, height, halign, valign, pad)
char *s;
int len;
XFontStruct *font;
int *dx, *dy;
int *width, *height;
int halign, valign, pad;
{
	*dx = *dy = 0;
	*width = XTextWidth(font, s, len);
	while (*s != '\0')
		if (*s++ == ' ')
			*width += pad;
	*height = font->ascent + font->descent;
	if (halign != LJUST) {
		if (halign == RJUST)
			*dx -= *width;
		else
			*dx -= *width / 2;
	}
	if (valign == TOPLINE)
		*dy += font->ascent;
	else if (valign == BOTLINE)
		*dy -= font->descent;
	else
		*dy += (font->ascent - font->descent) / 2 ;

}


/*
 *  Adjusting text simply puts the text in the minibuffer - if the user
 *  aborts the minibuffer, the text is left alone, otherwise, the new
 *  text is put in its place. I suppose the user could also be allowed
 *  to move the text as well, but I'd rather keep the actions orthogonal
 *  for now. All the action takes place in START_MODE, therefore this
 *  should never be in END_MODE.
 */
/*ARGSUSED*/ /* We don't use the mouse positions for this one */
text_adj(evtype, gel, mx, my)
int evtype;
Gel *gel;
int mx, my;
{
	static Gel *textgel;
	TextString *text;
	char *s;
	XFontStruct *font;
	int pad;
	/*
	 *  Will not need to process MOTION|START_MODE, RIGHT|START_MODE,
	 *  REDRAW|START_MODE - these are taken care of in
	 *  the adj_element routine.
	 */
	switch(evtype) {
	case MOTION | END_MODE:
		break;
	case LEFT | START_MODE:
		textgel = gel;
		GelHilite(textgel);
		message("Click Left Button to confirm selection");
		drawingMode = END_MODE;
		break;
	case LEFT | END_MODE:
		GelUnHilite(textgel);
		GelDraw(textgel, ERASE);
		text = (TextString *) textgel->data;
		textString = text->str;
		if ((s = get_input("? ", textString, FALSE)) == NULL) {
			GelDraw(textgel, DRAW);
			(void) PushUnderUndo(&(CurrentCell->gelList), textgel,
			 CurrentCell->undo);
			textgel = NULL;
			drawingMode = START_MODE;
			break;
		}
		textLen = strlen(s);
		textString = s;
		font = ChangeFont(&text->font->sizes[text->sizeindex], &pad);
		setfont(tmpGcNormal, font->fid);
		setfont(tmpGcInvert, font->fid);
		ComputeTextPosition(textString, textLen, font, &deltaX, &deltaY,
		 &textWidth, &textHeight, textgel->attributes & HALIGN, 
		 textgel->attributes & VALIGN, pad);
		textX = text->x + deltaX;
		textY = text->y + deltaY;
		AddTextGel(&(CurrentCell->gelList), textString, textLen, 
		 text->font, text->sizeindex, text->fontname, text->fontsize, 
		 textgel->attributes, text->x, text->y, 
		 textX, textY - textHeight, textX + textWidth, textY);
		drawtext(picWin, textX, textY, textString, textLen, tmpGcNormal, pad);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = textgel;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		textString = NULL;
		textgel = NULL;
		break;
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
		GelUnHilite(textgel);
		GelDraw(textgel, DRAW);
		(void) PushUnderUndo(&(CurrentCell->gelList), textgel,
		 CurrentCell->undo);
		textgel = NULL;
		drawingMode = START_MODE;
		if (evtype == (MIDDLE | END_MODE))
			ClearGelFlags(CurrentCell->gelList);
		break;
	case MIDDLE | START_MODE:
		break;
	case REDRAW | END_MODE:
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown mode %d in text_adj", 
		 evtype);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "text_adj");
}

/*
 * Finds distance of point from a text object. This is the distance of the
 * point from the text bounding box - we just call box_distance()
 */
int
text_distance(gel, xp, yp)
Gel *gel;
int xp, yp;
{
	extern int box_distance();
	
	return(box_distance(gel, xp, yp));
}
