/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/drawtxtv.c,v 2.20 89/11/02 10:38:42 zs01 Exp $ */
/* $ACIS:drawtxtv.c 1.6$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/drawtxtv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/drawtxtv.c,v 2.20 89/11/02 10:38:42 zs01 Exp $";
#endif /* lint */

#include <class.h>
#define AUXMODULE 1
#include <textv.eh>

#include <text.ih>
#include <mark.ih>
#include <envrment.ih>
#include <style.ih>
#include <fontdesc.ih>
#include <graphic.ih>
#include <view.ih>
#include <dict.ih>
#include <viewref.ih>
#include <txtvinfo.h>
#include <matte.ih>

static struct graphic *pat;

#define Text(self) \
    ((struct text *) ((self)->header.view.dataobject))

#define FastJustify(INFO) \
    ((INFO->endOfPara && INFO->just == \
    style_LeftAndRightJustified) || INFO->just == \
    style_LeftJustified)

#define TABBASE ((int) 'n')
/* the max number of pixels a char can overhang its bounding box */
#define MAXOVERHANG 4
#define MIN(a, b) \
    (((a) < (b)) ? (a) : (b))

static long StringWidth(widths, s)
register short *widths;
register char *s;
{
    register short w = 0;
    while (*s)
        w += widths[*s++];
    return (long) w;
}

/* Puts a 5-char sequence in string */

static void CharToOctal(s, c)
register char *s;
register char c;
{
    *s++ = '\\';
    *s++ = '0' + ((c >> 6) & 3);
    *s++ = '0' + ((c >> 3) & 7);
    *s++ = '0' + (c & 7);
    *s = '\0';
}

static long ParagraphIndent(self, text, pos, info)
struct textview *self;
struct text *text;
long pos;
register struct formattinginfo *info;
{
    struct environment *myEnv;
    struct text_statevector mysv;
    int tabWidth, spaceWidth;
    register int width = 0;
    register int c;
    struct fontdesc *font;
    short *widthTable;

    while (--pos >= 0 && text_GetChar(text, pos) != '\n')
        ;
    pos++;

    text_InitStateVector(&mysv);
    myEnv = environment_GetInnerMost(text->rootEnvironment, pos);
    text_ApplyEnvironment(&mysv, self->defaultStyle, myEnv);

    font = fontdesc_Create(mysv.CurFontFamily,
      mysv.CurFontAttributes, mysv.CurFontSize);

    widthTable = fontdesc_WidthTable(font, textview_GetDrawable(self));

    spaceWidth = widthTable[(int) ' '];
    tabWidth = 8 * self->tabWidth;

    while ((c = text_GetChar(text, pos)) == ' ' || c == '\t') {
	pos++;
	if (c == ' ')
	    width += spaceWidth;
	else
	    width = ((width + tabWidth) / tabWidth) * tabWidth;
    }
    return width;
}

static long MovePast(self, width, widths, bump, string)
struct textview *self;
register long width;
register short *widths;
register long bump;
register char *string;
{
    int tabWidth;
    register char tc;

    tabWidth = 8 * self->tabWidth;

    while (1) {
	if ((tc = *string++) == '\0')
	    return width;
	if (tc == ' ')
            width += bump;
	if (tc == '\t')
	    width = ((width + tabWidth) / tabWidth) * tabWidth;
        else
	    width += widths[0x0ff & (int) tc];
    }
}

/*
 * Finds the screen X coordinate for a given document pos
 */

static void LocateCursor(self, startX, spaceShim, startPos, widths, linePos, searchPos, info)
struct textview *self;
long startX;
long spaceShim;
long startPos;
register short *widths;
long linePos;
long searchPos;
register struct formattinginfo *info;
{
    long bx = (self->hasApplicationLayer) ? self->bx : self->ebx;

    if (startPos > searchPos)
        return;

    info->locateX = startX;   /* Default x */

    while (1) {
        register char tc = info->lineBuffer[linePos++];
	if (startPos == searchPos) {
	    info->locateX = startX;
	    return;
	}
        if (tc == '\0') {   /* At last char and still not found */
	    info->locateX = -1;
	    return;
	}
	if (tc == ' ')
	    startX += spaceShim;
	if (tc == '\t') {
            int tabWidth = 8 * self->tabWidth;
	    startX = (((startX - bx) + tabWidth) / tabWidth)
              * tabWidth + bx;
	} else
	    startX += widths[tc];
	startPos++;
    }
}

/*
 * Finds document pos for a given screen X coordinate
 */

static void LocateHit(self, startX, spaceShim, startPos, widths, linePos, searchX, info)
struct textview *self;
long startX;
long spaceShim;
long startPos;
register short *widths;
long linePos;
long searchX;
register struct formattinginfo *info;
{
    long bx = (self->hasApplicationLayer) ? self->bx : self->ebx;

    if (startX > searchX)
        return;

    info->locateX = startPos;	/* Default pos */

    while (1) {
        register char tc;
        register int endX;

	endX = startX;
	tc = info->lineBuffer[linePos++];
	if (tc == ' ')
	    endX += spaceShim;
	if (tc == '\t') {
            int tabWidth = 8 * self->tabWidth;
	    endX = (((endX - bx) + tabWidth) / tabWidth) *
              tabWidth + bx;
        } else
	    endX += widths[tc];

	/* Now we have the start (startX) and end (endX) of char */
        /* A position more than half way across a character goes to */
        /* the next character (most noticeable with tabs) */

	if ((searchX >= startX && searchX - startX <= endX - searchX) ||
          startX >= searchX || tc == '\0') {
	    info->locateX = startPos;
	    return;
	}

	startX = endX;
	startPos++;
    }
}

static void AllocateLineItem(self, text, pos, info)
struct textview *self;
struct text *text;
long pos;
register struct formattinginfo *info;
{
    register struct fontdesc *tf;
    register struct lineitem *tlp;
    struct text_statevector *tsv;
    register int th;
    struct FontSummary *fontInfo;

    /* Now allocate a lineitem for this call */

    info->clp = tlp = &(info->lineItems[info->lineIP]);
    tsv = &info->sv;

    tf = tsv->CurCachedFont;
    fontInfo = fontdesc_FontSummary(tf, textview_GetDrawable(self));

    th = fontInfo->maxHeight - fontInfo->maxBelow;

    if (tsv->CurScriptMovement < 0)
        th -= tsv->CurScriptMovement;

    if (th > info->lineAbove)
        info->lineAbove = th;

    th = fontInfo->maxHeight - th;

    if (tsv->CurScriptMovement > 0)
        th += tsv->CurScriptMovement;

    if (th > info->textBelow) {
        info->textBelow = th;
        if (th > info->lineBelow)
            info->lineBelow = th;
    }

    tlp->docPos = pos;
    tlp->scripting = tsv->CurScriptMovement;
    tlp->xPos = info->totalWidth;	/* where we are so far */

    if (tsv->CurView == NULL) {
        tlp->type = li_Plain;
        tlp->ti_lineBufPos = info->lineBP;
        tlp->ti_rSpaces = 0;		/* really set by adder */
        tlp->ti_font = tf;		/* current font structure */
        tlp->ti_fontWidths = fontdesc_WidthTable(tf,
          textview_GetDrawable(self));
        tlp->ti_styleFlags = tsv->SpecialFlags;
        tlp->ti_hasTab = 0;
    } else {
        register struct view *view = tsv->CurView;
        long desw, desh;

        tlp->type = li_View;
	tlp->vi_view = view;

	/* Offer it the whole screen.  We really shouldn't offer it
         * more than we can deliver, based on clp->height */

	view_DesiredSize(view, info->xDim - info->totalWidth,
          16384, view_NoSet, &desw, &desh);
	tlp->vi_width = (info->xDim - info->totalWidth < desw) ?
          info->xDim - info->totalWidth : desw;
	tlp->vi_height = desh;
	if (tlp->vi_height > info->lineBelow) {

            int xoffset, yoffset;

            view_GetOrigin(view, tlp->vi_width, tlp->vi_height, &xoffset, &yoffset);
            if (((tlp->vi_height) - yoffset) > info->lineBelow)
                info->lineBelow = ((tlp->vi_height) - yoffset);
            if (yoffset > info->lineAbove)
                info->lineAbove = yoffset;
        }
    }

    info->lineIP++;
}

/*
 * Todo: move showstyle data into textview?
 *
 * Important rule: since we do not have a position model that
 * indicates where within the style tree we are when we stop,
 * we have no way of differentiating between
 *  @bold{word
 *    }junk
 *
 *  and
 *
 *   @bold{word}
 *  junk
 *
 * In the case where there is *no* newline between these lines,  a
 * normal line wrapping operation has occurred.  In both cases, line
 * 2 is drawn from the same character position in the text buffer,
 * yet in one case, we are drawing some of the environment info on
 * that line, while in the other we are not.
 *
 * Since we do not know which we should do, given a simple buffer
 * position, we make the *convention* that lines can start with
 * @environmentname or normal text, but can not end with
 * @environmentname.  Similarly, they can end with } chars,
 * but can not begin with these close brackets.
 *
 * This gives us an unambiguous rule for how to draw a line, given
 * only a buffer position.
 *
 * A better "fix" is to rewrite this code to have another parameter,
 * indicating where we stopped drawing on the previous line.  This
 * might even work, although it should require some pervasive changes
 * to the paragraph analyzer code as well as the main redisplay.  It
 * also restricts how LineRedraw can be used. This whole file should
 * be rewritten.
 *
 * Yet another alternative is to clean this up and use the model that
 * there are certain states that one can not leave the line in, and that
 * one must backup to a previously-seen safe state.  E.g. of a safe
 * state: has no style lineitem at its end.
 */

struct fontdesc *exposeStylesFont = NULL;
short *exposeStylesWidths = NULL;
/* This definitely should be in the view */
#define NCXS 50

static void ComputeStyleItem(self, startPos, endPos, pixelAddr, charAddr, info)
long startPos, endPos;
struct textview *self;
long *pixelAddr, *charAddr;
register struct formattinginfo *info;
{
    struct environment *startEnv, *endEnv;
    register struct environment *cparent, *te;
    struct environment *envStack[NCXS];
    struct text *text;
    register char *tp;
    long newChars, newPixels, i, nextSlot;

    if (exposeStylesFont == NULL) {
	/* Try to fill it in */
	exposeStylesFont =
          fontdesc_Create("andysans", fontdesc_Plain, 10);
	exposeStylesWidths =
          fontdesc_WidthTable(exposeStylesFont,
	    textview_GetDrawable(self));
    }

    text = Text(self);

    /* If environment starts the line, don't do the trailing curly's, */
    /* as they're on the preceding line. */

    if (startPos == -1) {
	/* Hack: try to guess when to emit a '>' char */
	if (endPos == 0)
	    startEnv = text->rootEnvironment;
	else if (text_GetChar(text, endPos-1) == '\n')
	    startEnv =
              environment_GetInnerMost(text->rootEnvironment,
              endPos - 1);
	else {
	    startEnv =
              environment_GetInnerMost(text->rootEnvironment,
              endPos);
            i = endPos;     /* Where we should not move from */
	    while (1) {
		/* Keep going up tree until find root or new left edge */
		if (startEnv == text->rootEnvironment)
                    break;
		if (environment_Eval(startEnv) < i)
                    break;  /* Gone too far */
		startEnv = (struct environment *)
                  startEnv->header.nestedmark.parent;
	    }
	}
    } else
        startEnv =
          environment_GetInnerMost(text->rootEnvironment,
          startPos);
    endEnv =
      environment_GetInnerMost(text->rootEnvironment, endPos);
    tp = &info->lineBuffer[info->lineBP];
    cparent = environment_GetCommonParent(startEnv, endEnv);
    newChars = 0;
    newPixels = 0;
    /* start out by putting out required close curlys */
    for (te = startEnv; te && te != cparent;
	te = (struct environment *) te->header.nestedmark.parent) {
	*tp++ = '>';
	newChars++;
	newPixels += exposeStylesWidths['>'];
    }
    te = endEnv;
    for (nextSlot = 0; nextSlot < NCXS; nextSlot++) {
	if (te == cparent)
            break;
	envStack[nextSlot] = te;
	te = (struct environment *) te->header.nestedmark.parent;
    }
    for (i = nextSlot - 1; i >= 0; i--) {
	/* Put out the word into the buffer */
        /* Use @view<object-name,OBJECT> for view environments */
	te = envStack[i];
        switch (te->type) {
            case environment_Style:
                strcpy(tp, "@");
                if (te->data.style->name == NULL)
                    strcat(tp, "?");
                else
                    strcat(tp, te->data.style->name);
                strcat(tp, "<");
                break;
            case environment_View:
                strcpy(tp, "@view<");
                if (te->data.viewref->dataObject == NULL)
                    strcat(tp, "?");
                else
                    strcat(tp, class_GetTypeName(te->data.viewref->dataObject));
                strcat(tp, ",");
                break;
        }
        newPixels += StringWidth(exposeStylesWidths, tp);
        while (*tp) {
            newChars++;
            tp++;
        }
    }
    /* Finally, null terminate */
    *tp++ = 0;
    newChars++;
    /* Now handle returning stuff */
    *pixelAddr = newPixels;
    *charAddr = newChars;
    info->lineBP += newChars;
}

/* Note: When trying to speed up redisplay, GenerateLineItems is a */
/* very important place to begin. */

/* This routine breaks a line up into line items.  A line item is a */
/* section of text that has the same look, or a view, or an expose */
/* styles string, etc.  Also sets a number of global variables from */
/* the top of this file so that FullLineRedraw can know about this */
/* line. Have temporarily taken out caching of the state vector. */

static void GenerateLineItems(self, text, currentLine, info)
struct textview *self;
struct text *text;
struct mark *currentLine;
register struct formattinginfo *info;
{
    struct environment *myEnv;
    long lastEnvPos = 0;
    register char *tp, tc;
    long nChars, pos, bufEnd, tabWidth;
    long pi;            /* Value from ParagraphIndent */
    long i, localWidth, initPos, localRubber;
    int lastWidth = 0;      /* Width just before the last rubber space */
    char *lastBP;       /* &linebuffer[linebp] just before last rubber space */
    long lastIP;        /* Line item index containing last space */
    long lastPos;       /* Text pos just before the last space */
    short passes;
    long dotPos = -1, markPos = -1;
    boolean FoundmarkPos = FALSE, FounddotPos = FALSE, oncePerLine;
    long bufLen;
    char *buf = NULL;

    if (self->predrawn && mark_GetLength(self->predrawn) > 0 &&
      mark_GetPos(self->predrawn) == mark_GetPos(currentLine))
	markPos = mark_GetPos(self->predrawn) + mark_GetLength(self->predrawn);

    bufEnd = text_GetLength(text);		/* The ending point */
    initPos = pos = mark_GetPos(currentLine);	/* Initialize it */
    if (bufEnd > pos)
        dotPos = mark_GetPos(self->dot);
    oncePerLine = TRUE;
    passes = 0;
    info->lineBP = 0;
    lastBP = 0;
    lastPos = -1;
    lastIP = -1;

    /* First compute the current state vector: */

    while (1) {
	int envStart = pos, envLength; /* Remember beginning pos */
	struct text_statevector *tsv = &info->sv;

        if (info->lineIP == NTITEMS) {
            /* Trouble:  out of line items.  Just wrap before EOL */
            info->endOfPara = FALSE;
            return;
        }

	text_InitStateVector(tsv);	/* Initialize an empty state vector */

	myEnv = environment_GetInnerMost(text->rootEnvironment, pos);
	envLength = environment_GetLength(myEnv);
	text_ApplyEnvironment(tsv, self->defaultStyle, myEnv);

        /* The hidden flag handling probably does not work. */

	if ((tsv->SpecialFlags & style_Hidden) && envLength > 0) {
	    pos += envLength;
	    info->lineLength += envLength;
	    continue;
	}

	/* Now we have to generate the proper font structure in sv */

	tsv->CurCachedFont = fontdesc_Create(tsv->CurFontFamily,
          tsv->CurFontAttributes, tsv->CurFontSize);

	if (myEnv->type == environment_View) {
	    struct viewref *vr = myEnv->data.viewref;
	    if ((tsv->CurView = (struct view *)
              dictionary_LookUp(self, (char *) vr)) == NULL) {
		if ((tsv->CurView = (struct view *) matte_Create(vr,
                  (struct view *)self)) != NULL) {
		    dictionary_Insert(self, (char *) vr,
                      (char *) tsv->CurView);
		    viewref_AddObserver(vr, self);
		    if (vr == self->currentViewreference)
                      view_WantInputFocus(tsv->CurView, tsv->CurView);
		} else
		    dictionary_Insert(self, (char *) vr,
                      (char *) textview_UNKNOWNVIEW);
	    } else
                if (tsv->CurView == (struct view *) textview_UNKNOWNVIEW)
                    tsv->CurView = NULL;
        }

	nChars = environment_GetNextChange(text->rootEnvironment, pos);

	/* Break up line items at dot so we can skimp on line redrawing */

	if (pos + 1 < markPos && pos + nChars > markPos ) {
	    FoundmarkPos = TRUE;
	    nChars = markPos - pos;
	}

	if (pos <= dotPos && pos + nChars >= dotPos)
		FounddotPos = TRUE;

	/* Find end of this */

	if (oncePerLine) {
	    /* This can't be done before calling GenerateLineItems since */
            /* env must be computed before we can do these calculations. */

	    info->inContinueMode =
              (tsv->SpecialFlags & style_ContinueIndent);
	    if (! info->pBreak &&
              (tsv->SpecialFlags & style_ContinueIndent))
		pi = ParagraphIndent(self, text, pos, info);
	    else
		pi = 0;
	    info->bIndent=tsv->CurIndentation;
	    info->bLM=tsv->CurLeftMargin + pi;
	    info->just = tsv->CurJustification;
	    info->xDim = info->xDim - tsv->CurLeftMargin -
              tsv->CurRightMargin - pi;
	    if (info->pBreak)
		info->xDim = info->xDim - tsv->CurIndentation;
	    lastEnvPos = -1;
	    oncePerLine = 0;
	}

	info->myWidths = fontdesc_WidthTable(tsv->CurCachedFont,
	    textview_GetDrawable(self));

	if (nChars > bufEnd - pos)
	    nChars = bufEnd - pos;

	/* If we're in expose style mode, and haven't done styles */
        /* for this position yet, do so. */

	if (self->exposeStyles && pos != lastEnvPos) {
	    long width, chars, oldLineBP;
	    struct lineitem *tlp;
	    oldLineBP = info->lineBP;
	    ComputeStyleItem(self, lastEnvPos, pos, &width, &chars, info);
	    if (width > 0) {
		if (info->totalWidth + width > info->xDim) {
		    info->endOfPara = FALSE;	/* Wrapped the line */
		    info->lineBP = oldLineBP;
		    return;	/* That's it for now */
		}

		tlp = &info->lineItems[info->lineIP++];

		tlp->docPos = pos;
		tlp->scripting = 0;
		tlp->xPos = info->totalWidth;
                tlp->type = li_Expose;

		tlp->ti_lineBufPos = oldLineBP;
		tlp->ti_rSpaces = 0;
		tlp->ti_font = tsv->CurCachedFont;
		tlp->ti_fontWidths = info->myWidths;
                tlp->ti_styleFlags = tsv->SpecialFlags;
                tlp->ti_hasTab = 0;

		/* lineBP advanced in ComputeStyleItem */

		info->totalWidth += width;
	    }
	    lastEnvPos = pos;
	}

	/* Check to see if we're really done. */
        /* Always go once around. */

	if (passes > 0) {
	    if (pos >= bufEnd)
                return;
	} else
            passes = 1;

	AllocateLineItem(self, text, pos, info);

	if (FounddotPos) {
	    int ipos; 
	    struct fontdesc *cfont = info->clp->ti_font;
	    info->itemAtDot = info->clp;
	    /* Find how far the same font preceeds the dot */
	    info->lastFontPos = info->clp->docPos;
	    for (ipos = info->lineIP - 2; ipos >= 0; ipos--) {
		if (info->lineItems[ipos].ti_font != cfont)
                    break;
		info->lastFontPos = info->lineItems[ipos].docPos;
	    }
	    FounddotPos = FALSE;
	}

	if (info->clp->type == li_View) {
	    info->totalWidth += info->clp->vi_width;
	    if (info->xDim < info->totalWidth) {
		info->totalWidth -= info->clp->vi_width;
		info->lineIP--;	/* We'll start again on the next line */
		info->endOfPara = FALSE;    /* Wrapped the line */
		return;
	    }
	    info->lineLength += nChars;
	    pos += nChars;
	} else {
	    /* Process all of the characters into line items */

	    localWidth = info->totalWidth;
	    localRubber = 0;
	    tp = &info->lineBuffer[info->lineBP];

            bufLen = 0;  /* Refresh GetBuf since pos may have changed */

            for (i = 0; ; i++) {
		if (info->xDim < localWidth) {
		    int noWrap = tsv->SpecialFlags & style_NoWrap;
		    /* Wrap line */

		    info->endOfPara = FALSE;

		    if (noWrap || (localRubber <= 0 &&
                      info->rubberSpaces <= 0)) {
			/* Here on word too long to fit on line; */
                        /* should really push last char to next line */

			if (noWrap) {
			    /* No wrapping, skip to end of line (so it */
                            /* never gets displayed), or the end of the env. */
			    int end = envStart + envLength; 

			    info->endOfPara = TRUE;
			    do {
				if (pos >= bufEnd)
				    break;
				if (pos >= end) {
				    info->endOfPara = FALSE;
				    break;
				}
                                if (bufLen == 0)    /* filbuf */
                                    buf = text_GetBuf(Text(self),
                                      pos, 1024, &bufLen);
                                tc = *buf++, bufLen--, pos++;
			    } while (tc != '\n');
			}

			*tp++ = 0;
			info->lineBP = tp - info->lineBuffer;
			info->totalWidth = localWidth;
			info->lineLength = pos - initPos;
                        /* No rubber spaces, otherwise we'd wrap the line there */
			info->clp->ti_rSpaces = 0;
			return;
		    }

		    if (lastBP == 0) {	/* Is this necessary? */
			info->lineIP--;
			info->endOfPara = FALSE;
			info->lineLength = info->clp->docPos - initPos;
			return;
		    }

		    info->totalWidth = lastWidth;

		    /* How far we've gone, less the last word */

		    info->markLength = pos - initPos;
                    info->lineLength = lastPos - initPos;
		    info->lineIP = lastIP;
		    *lastBP++ = NULL;
		    info->lineBP = lastBP - info->lineBuffer;
		    localRubber--;  /* Include deleted one */
		    info->rubberSpaces += localRubber;
		    info->clp->ti_rSpaces = localRubber;
		    return;
		}

		if (i >= nChars)
                    break;	/* are we done yet? */

                if (bufLen == 0)    /* filbuf */
                    buf = text_GetBuf(Text(self), pos, 1024, &bufLen);
                tc = *buf++, bufLen--, pos++;

		if (tc == ' ') {
		    localRubber++;
		    lastWidth = localWidth;
		    lastBP = tp;	/* Points at the space! */
		    lastPos = pos - 1;
		    lastIP = info->lineIP;
                    *tp++ = tc;
                    localWidth += info->myWidths[(int) tc];
		} else if (tc == '\n') {
		    *tp++ = '\0';
		    info->lineBP = tp - info->lineBuffer;
		    info->totalWidth = localWidth;
		    info->lineLength = pos - initPos;
		    info->markLength = info->lineLength;
		    info->rubberSpaces += localRubber;
		    info->clp->ti_rSpaces = localRubber;
		    if (FoundmarkPos)
			info->predrawnEnd = info->totalWidth;
		    return;
		} else if (tc == '\t') {
                    *tp++ = tc;
		    tabWidth = 8 * self->tabWidth;
		    localWidth += info->bIndent + info->bLM;
		    localWidth = ((localWidth + tabWidth) /
                      tabWidth) * tabWidth;
		    localWidth -= info->bIndent + info->bLM;
		    info->sawTab = TRUE;
                    info->clp->ti_hasTab = 1;
		    /* Tabs end processing of a line item */
		    break;
                } else if (info->myWidths[(int) tc] == 0 ||
                  ! fontdesc_CharValid(tsv->CurCachedFont, tc)) {
                    if (i == 0) {
                        long width;
                        /* Gets its own lineitem.  It's not very */
                        /* efficient when there is a whole screenful */
                        /* of octal codes in a row. */
                        CharToOctal(tp, tc);
                        width = StringWidth(info->myWidths, tp);
                        if (info->totalWidth + width > info->xDim) {
                            info->endOfPara = FALSE;    /* Wrapped line */
                            info->lineIP--;     /* Retract this item */
                            return;
                        }
                        /* AllocateLineItem has set most item params */
                        info->lineItems[info->lineIP - 1].type = li_Octal;
                        tp += 5;
                        localWidth += width;
                        break;
                    }
                    /* Unprintable char ends processing of line item; */
                    /* Back up one.  Octal will be placed in its own line */
                    /* item next time around outer loop. */
                    pos--;
                    break;
                } else {    /* Plain char */
                    *tp++ = tc;
		    localWidth += info->myWidths[tc];
                }

		/* End of handling of the character */
            }

	    info->rubberSpaces += localRubber;
	    info->clp->ti_rSpaces = localRubber;
	    info->lineLength = pos - initPos;
	    info->totalWidth = localWidth;
	    *tp++ = 0;
	    info->lineBP = tp - info->lineBuffer;
        }

	if (FoundmarkPos) {
	    info->predrawnEnd = info->totalWidth;
	    FoundmarkPos = FALSE;
	}
    }
}

static void DrawBar(self, tt, bx, by, width)
struct textview *self;
struct lineitem *tt;
long bx, by;
long width;
{
    struct FontSummary *fontInfo;
    long above, below;

    if (width == 0)
        return;

    fontInfo = fontdesc_FontSummary(tt->ti_font,
      textview_GetDrawable(self));

    below = fontInfo->maxBelow;
    above = fontInfo->maxHeight - below;

    if (below > 2)
        below = 2;

    if (tt->ti_styleFlags & style_Underline) {
        textview_MoveTo(self, bx, by + below);
        textview_DrawLineTo(self, bx + width, by + below);
    }

    if (tt->ti_styleFlags & style_ChangeBar) {
        textview_MoveTo(self, bx, by - above / 3);
        textview_DrawLineTo(self, bx + width, by - above / 3);
    }

    if (tt->ti_styleFlags & style_OverBar) {
        textview_MoveTo(self, bx, by - above);
        textview_DrawLineTo(self, bx + width, by - above);
    }
}

/* Elimination of tabs from displayed strings
 * This function should be either eliminated or fixed. As far as I can tell,
 * the tab can only occur at the beginning or end of the string passed to this
 * function. The 1024 character array is a quick fix (it used to be 90) as it
 * would to painful to do it right at this time.
 */

static void DrawStringNoTabs(self, s, ctrl)
struct textview *self;
char *s;
int ctrl;
{
    char *st, *dt, xbuf[1024];
    for (st = s, dt = xbuf; *st; st++)
        if (*st != '\t')
            *dt++ = *st;
    *dt = '\0';
    textview_DrawString(self, xbuf, graphic_ATBASELINE);
}

long textview__LineRedraw(self, type, currentLine, x, y, xSize, ySize, search, cont, txheight, info)
struct textview *self;
enum textview_LineRedrawType type;
struct mark *currentLine;
long x, y;
long xSize, ySize;
long search;
boolean *cont;
long *txheight;
register struct formattinginfo *info;
{
    struct text *text = Text(self);
    long zapMe;
    int k, lli, delta, zapPos = 0, foundSpaces = 0, pos, by, bx = 0;
    char *sPtr;
    boolean isBlack = FALSE;
    struct fontdesc *fontID = textview_GetFont(self);
    register int i, j = 0;
    register struct lineitem *tt, *tlp;

    int SkipPredrawn;
    int initialIndent;

    info->xDim = xSize;
    info->totalWidth = 0;
    info->rubberSpaces = 0;
    info->lineLength = 0;
    info->markLength = -1;
    info->lineBelow = 0;
    info->textBelow = 0;
    info->lineAbove = 0;
    info->foundView = NULL;
    zapMe = -1;
    info->cursorY = y;
    info->endOfPara = TRUE;
    info->lineIP = 0;
    pos = mark_GetPos(currentLine);     /* The starting position */
    info->sawTab = FALSE;
    info->predrawnEnd = 0;
    info->itemAtDot = NULL;
    info->clp = NULL;

    /* First decide if we're a paragraph break (just after newline) */

    info->pBreak =
     (pos == 0 || text_GetChar(text, pos - 1) == '\n');

    if (type == textview_FullLineRedraw &&
      mark_GetModified(self->predrawn) &&
      ! mark_GetModified(self->prepredrawn)) {
	mark_GetLength(self->predrawn) = mark_GetLength(self->prepredrawn);
	self->predrawn->modified = 0;
    }

    GenerateLineItems(self, text, currentLine, info);

    if (type == textview_FullLineRedraw && info->predrawnEnd &&
	self->predrawnY == y + info->lineAbove && self->predrawnX == x && 
	FastJustify(info) && ! mark_GetModified(self->predrawn) && 
	self->csxPos > 0 /* Indicates if we are in a full redraw */ )
	SkipPredrawn = info->predrawnEnd;
    else
        SkipPredrawn = 0;

    /* Advance the marker over any blanks absorbed by a line-wrap */

    if (! info->endOfPara) {
	long endPos;

	for (endPos = info->lineLength + pos;
          endPos < text_GetLength(text) &&
          text_GetChar(text, endPos) == ' '; endPos++)
            ;

        info->lineLength = endPos - pos;
	if (info->markLength < info->lineLength)
	    info->markLength = info->lineLength;
    }

    /* Before drawing the lineitems, if this is justified text, */
    /* delete rubber spaces at the start and end of each line. */
   
    if (info->just == style_LeftAndRightJustified) {
	/* Do the end */

	if (! info->endOfPara) {
	    lli = info->lineIP-1;
	    tlp =  &(info->lineItems[lli]);
            j = tlp->ti_lineBufPos;    /* The start pos for the last item */
            i = info->lineBP - 2;   /* Before nul at end of last item */
	    while (i >= j) {
		if (info->lineBuffer[i] != ' ')
                    break;              /* No more spaces */
                info->lineBuffer[i]= '\0';  /* Zap the space */
                tlp->ti_rSpaces--;      /* One less rubber space */
		info->totalWidth -= tlp->ti_fontWidths[' '];
		info->rubberSpaces--;
                i--;                /* Back up one more char */
	    }
	}
    }

    /* Now all of the rubber spaces at the start and end of */
    /* the line are gone.  Next compute the amount of space */
    /* per rubber space that has to be inserted.  Delta is the left */
    /* over, which adds up to be a significant amount of text if */
    /* one tries to insert a fixed amount per rubber space. */

    if (info->just == style_LeftAndRightJustified &&
      info->rubberSpaces > 0) {
	if (info->endOfPara) {
	    info->spaceBump = 0;
	    delta = 0;
	} else {
	    info->spaceBump = (info->xDim -
              info->totalWidth) / info->rubberSpaces;
            delta = (info->xDim - info->totalWidth) -
              info->spaceBump * info->rubberSpaces;
	}
    } else {
	/* Don't even have one rubber space */
	info->spaceBump = 0;
	delta = 0;
    }

    if (info->sawTab) {
	info->spaceBump = 0;
	delta = 0;
    }

    /* If delta is non-zero, it is how much space to jam in */
    /* after seeing (rubberspaces - delta) rubberspaces.  Find */
    /* the lineitem containing this space, and set zapPos to point */
    /* at it.  Also set foundspaces to be the number of shorter */
    /* spaces to be printed in this lineitem.  Also set zapMe to */
    /* point at this item. */

    if (delta != 0) {
	foundSpaces = info->rubberSpaces - delta;
	for (i = 0; i < info->lineIP; i++) {
            tlp = &info->lineItems[i];
            if (tlp->type == li_Plain) {
                if (tlp->ti_rSpaces > foundSpaces) {
                    /* Found the guy to hit */
                    zapMe = i;
                    j = tlp->ti_lineBufPos;
                    k = foundSpaces;    /* Skip k spaces */
                    while (1) {
                        if (info->lineBuffer[j] == ' ') {
                            if (k == 0)
                                break;
                            k--;
                        }
                        j++;
                    }
                    zapPos = j;

                    /* By breaking here, foundspaces is left at the # */
                    /* of spaces printed at the original space bump */
                    /* in this lineitem */

                    break;
                }

                foundSpaces -= tlp->ti_rSpaces;
            }
        }
    }

    /* Next draw all of the lineitems.  Set initial space shim. */

    textview_SetSpaceShim(self, info->spaceBump);

    i = 0;

    /* Most of the work for justification follows.  In addition, */
    /* there is some code in GenerateLineItems which computes */
    /* xDim (amount of space available on line) when the first env */
    /* is entered.  So, if you're adding new Modes, don't forget to */
    /* consider changing GenerateLineItems, too.   j is extra x bump */
    /* due to rubber space expansion.  We initialize it to the place */
    /* to start drawing the line. */

    if (info->just == style_LeftAndRightJustified ||
      info->just == style_LeftJustified) {
	if (info->pBreak)
	    j = info->bIndent + info->bLM;
	else
	    j = info->bLM;
    } else if (info->just == style_Centered)
	j = info->bLM + (info->xDim-info->totalWidth) / 2;
    else if (info->just == style_RightJustified)
	j = info->bLM + info->xDim - info->totalWidth;
    else if (info->just == style_LeftThenRightJustified) {
	if (info->pBreak) 
	    j = info->bLM + info->bIndent;
	else
	    j = info->bLM + info->xDim - info->totalWidth;
    }

    /* If we're looking, do initializations */

    j += x;
    initialIndent = j;
    if (SkipPredrawn)
        SkipPredrawn += j;
    if (type == textview_GetCoordinate || type == textview_GetHeight)
	info->locateX = j + info->lineItems[0].xPos;
    else if (type == textview_GetPosition) {
	info->locateX = info->lineItems[0].docPos;
	info->foundView = NULL;	/* Flag tells us if mouse hit an inset */
    }

    /* We add three below to take care of overhang from the last */
    /* char, rather than compute at the proper value.  We really should */
    /* compute it properly.  This sort of counts on a reasonable clip */
    /* rectangle being set on the view. */

    /* Unfortunately, the proper thing to do is to, in the text justifier, */
    /* add in the origin to east edge to see if a char fits.  Then, if we */
    /* put a new character in, we subtract this value out and insert its */
    /* spacing value.  This way, the last character's width also includes */
    /* the distance to the edge of its bounding box.  Computationally it */
    /* is cheaper to just add a little fudge here, however. */
   
    if (type == textview_FullLineRedraw && search == 0) {
	struct rectangle clearRect;
	textview_SetTransferMode(self, graphic_COPY);
	rectangle_SetRectSize(&clearRect,
          SkipPredrawn, info->cursorY,
          textview_GetLogicalWidth(self)- SkipPredrawn,
          MIN(ySize, info->lineAbove + info->lineBelow));
	pat = textview_WhitePattern(self);
	textview_FillRect(self, &clearRect, pat);
	isBlack = FALSE;
    }

    if (! isBlack) {
	textview_SetTransferMode(self, graphic_BLACK);
	isBlack = TRUE;
    }

    while (i < info->lineIP) {		/* Print each line item */
	tt = &info->lineItems[i];
	bx = j + tt->xPos;
	sPtr = &info->lineBuffer[tt->ti_lineBufPos];
        /* by = Superscript + y coord + line "above" */
	by = tt->scripting + info->cursorY + info->lineAbove;
	if (tt->type == li_View) {
	    if (type == textview_FullLineRedraw) {
		if (bx >= SkipPredrawn) {

		    struct rectangle enclosingRect;
                    long xoffset, yoffset;

		    /* The code to display an inset; size computed in */
                    /* AllocateLineItem during line layout */

		    if (tt->vi_width > xSize)
                        tt->vi_width = xSize;
		    view_GetOrigin(tt->vi_view, tt->vi_width, tt->vi_height, &xoffset, &yoffset);
		    rectangle_SetRectSize(&enclosingRect,
                        bx - xoffset, by - yoffset, tt->vi_width, tt->vi_height);
		    view_InsertView(tt->vi_view, self, &enclosingRect);
		    textview_RetractViewCursors(self, tt->vi_view);
		    view_FullUpdate(tt->vi_view,
                      view_FullRedraw, 0, 0, 0, 0);
		}		
		info->foundView = tt->vi_view;
	    } else if (type == textview_GetPosition) {
		if (search >= bx && search <= bx + tt->vi_width)
		    info->foundView = tt->vi_view;
	    } else if (type == textview_GetCoordinate ||
                       type == textview_GetHeight) {
		/* Check if in bounds; 1 should be length of fake env */
		if (search >= tt->docPos && search < tt->docPos + 1)
		    info->locateX = bx;
                else if (i + 1 == info->lineIP && tt->docPos + 1 == search)
                    info->locateX = bx + tt->vi_width;
	    }
        } else {    /* Textual item */
	    if (tt->ti_font != fontID) {
		fontID = tt->ti_font;
		textview_SetFont(self, fontID);
	    }
	    if (i != zapMe) {
		if (type == textview_FullLineRedraw) {
		    /* Drawing a line */
		    if (bx >= SkipPredrawn) {
			if (tt->type == li_Expose) {
			    fontID = exposeStylesFont;
			    textview_SetFont(self, fontID);
			    textview_MoveTo(self, bx, by);
			    textview_DrawString(self,
                              sPtr, graphic_ATBASELINE);
			} else if (info->lineAbove +
                          info->textBelow <= ySize) {
                            textview_MoveTo(self, bx, by);
                            if (tt->ti_hasTab)
                                DrawStringNoTabs(self, sPtr, graphic_ATBASELINE);
                            else
                                textview_DrawString(self,
                                        sPtr, graphic_ATBASELINE);
                            if (tt->ti_styleFlags &
                              (style_Underline | style_ChangeBar |
                               style_OverBar)) {
                                DrawBar(self, tt, bx, by,
                                  StringWidth(tt->ti_fontWidths, sPtr) +
                                  tt->ti_rSpaces * info->spaceBump);
                            }
                        }
		    }
		} else if (type == textview_GetPosition) {
		    /* Looking for a cursor hit */
		    if (tt->type == li_Plain)
			LocateHit(self, bx, info->spaceBump,
                          tt->docPos, tt->ti_fontWidths,
                          tt->ti_lineBufPos, search, info);
                    else if (tt->type == li_Octal) {
                        long ex = bx +
                          StringWidth(tt->ti_fontWidths, sPtr);
                        if (search - bx >= ex - search)
                            info->locateX = tt->docPos + 1;
                    }
                    /* li_Expose are ignored */
		} else {  /* textview_GetCoordinate */
                    if (tt->type == li_Plain)
                        LocateCursor(self, bx, info->spaceBump,
                          tt->docPos, tt->ti_fontWidths,
                          tt->ti_lineBufPos, search, info);
                    else if (tt->type == li_Expose ||
                      tt->type == li_Octal) {
                        if (tt->docPos == search)
                            info->locateX = bx;
                        else if (tt->docPos + 1 == search)
                            /* Special case at end of line. */
                            info->locateX = bx +
                              StringWidth(tt->ti_fontWidths, sPtr);
                    }
                }
                j += tt->ti_rSpaces * info->spaceBump;
            } else {   /* i == zapMe */
		if (type == textview_GetPosition) {
		    /* Looking for document pos */
		    info->lineBuffer[zapPos] = '\0';
		    LocateHit(self, bx, info->spaceBump,
                      tt->docPos, tt->ti_fontWidths,
                      tt->ti_lineBufPos, search, info);
		    bx = MovePast(self, bx, tt->ti_fontWidths,
                      info->spaceBump, sPtr);
		    info->spaceBump++;
		    info->lineBuffer[zapPos] = ' ';
		    LocateHit(self, bx, info->spaceBump,
                      tt->docPos + zapPos - tt->ti_lineBufPos,
                      tt->ti_fontWidths, zapPos, search, info);
		} else if (type == textview_GetCoordinate ||
                  type == textview_GetHeight) {
		    /* Looking for a hit */
		    info->lineBuffer[zapPos] = '\0';
		    LocateCursor(self, bx, info->spaceBump,
                      tt->docPos, tt->ti_fontWidths,
                      tt->ti_lineBufPos, search, info);
		    bx = MovePast(self, bx, tt->ti_fontWidths,
                      info->spaceBump, sPtr);
		    info->spaceBump++;
		    info->lineBuffer[zapPos] = ' ';
		    LocateCursor(self, bx, info->spaceBump,
                      tt->docPos + zapPos - tt->ti_lineBufPos,
                      tt->ti_fontWidths, zapPos, search, info);
		} else {
		    /* Drawing a line: First take care of bar for entire */
                    /* line item (this is done before we mess with the */
                    /* buffer, bx, spaceBump, etc. */
                    if (tt->ti_styleFlags &
                      (style_Underline | style_ChangeBar | style_OverBar))
                        DrawBar(self, tt, bx, by,
                          StringWidth(tt->ti_fontWidths, sPtr) +
                          tt->ti_rSpaces * (info->spaceBump + 1) -
                          foundSpaces);
		    info->lineBuffer[zapPos] = '\0';
		    if (info->lineAbove + info->textBelow <= ySize) {
			textview_MoveTo(self, bx, by);
                        if (tt->ti_hasTab)
                            DrawStringNoTabs(self, sPtr, graphic_ATBASELINE);
                        else
                            textview_DrawString(self,
                                    sPtr, graphic_ATBASELINE);
		    }
		    bx = MovePast(self, bx, tt->ti_fontWidths,
                      info->spaceBump, sPtr);
		    textview_SetSpaceShim(self, ++info->spaceBump);
                    info->lineBuffer[zapPos] = ' ';   /* Put ' ' back */
		    if (info->lineAbove + info->textBelow <= ySize) {
			textview_MoveTo(self, bx, by);
                        if (tt->ti_hasTab)
                            DrawStringNoTabs(self,
                                 &info->lineBuffer[zapPos], graphic_ATBASELINE);
                        else
                            textview_DrawString(self,
                                    &info->lineBuffer[zapPos],
                                    graphic_ATBASELINE);
		    }
		}
		/* spaceBump has already been incremented */

		j += (tt->ti_rSpaces * info->spaceBump - foundSpaces);
	    }
	}
	i++;
    }

    if (type == textview_FullLineRedraw &&
      search == 0 && bx < SkipPredrawn) {
	/* Clear out garbage that may be at end of line after wrap */
	struct rectangle clearRect;
	textview_SetTransferMode(self, graphic_COPY);
	rectangle_SetRectSize(&clearRect,
          info->totalWidth + initialIndent, info->cursorY,
          SkipPredrawn - info->totalWidth - initialIndent,
          MIN(ySize, info->lineAbove + info->lineBelow));
	pat = textview_WhitePattern(self);
	textview_FillRect(self, &clearRect, pat);
	textview_SetTransferMode(self, graphic_BLACK);
    }

    /* Finally, set the mark length */

    if (info->markLength == -1) {
	mark_SetLength(currentLine, info->lineLength);
	currentLine->includeEnding = TRUE;
    } else {
 	mark_SetLength(currentLine, info->markLength);
	currentLine->includeEnding = FALSE;
    }

    if (type == textview_FullLineRedraw && info->itemAtDot != NULL) {
	if (FastJustify(info) && info->itemAtDot->type != li_View) {
	    /* Set marks to indicate what text prior to the dot */
            /* we  can avoid redrawing next time around. */
	    int cp, ep, WillClear;
	    char chr;
	    struct fontdesc_charInfo cinfo;
	    cp = mark_GetPos(currentLine);
	    ep = mark_GetPos(self->dot);
	    chr = text_GetChar(text, ep);
	    fontdesc_CharSummary(info->itemAtDot->ti_font,
              textview_GetDrawable(self), chr, &cinfo);
	    WillClear = (cinfo.xOriginOffset <= 0);
	    for (ep--; ep >= cp; ep--) {
		chr = text_GetChar(text, ep);
		if (chr == ' ' || chr == '\t') {
		    if (WillClear) {
			cinfo.xOriginOffset = 0;
			break;
		    }
		    WillClear = TRUE;
		    continue;
		}
		if (ep >= info->lastFontPos) {
		    fontdesc_CharSummary(info->itemAtDot->ti_font,
                      textview_GetDrawable(self), chr, &cinfo);
		    if (WillClear && cinfo.width <=
                      cinfo.xSpacing + cinfo.xOriginOffset)
			break;
		    WillClear = (cinfo.xOriginOffset <= 0);
		}
	    }
	    ep++;
	    if (ep < cp)
                ep = cp;
	    mark_SetPos(self->predrawn, cp);
	    mark_SetLength(self->predrawn, ep - cp);
	    self->predrawn->modified = 0;
	    /* Set a second mark to prevent redrawing when a */
            /* character is deleted. */
	    ep--;
	    WillClear = (cinfo.xOriginOffset <= 0);
	    for (ep--;ep >= cp;ep--) {
		chr = text_GetChar(text, ep);
		if (chr == ' ' || chr == '\t') {
		    if (WillClear) break;
		    WillClear = TRUE;
		    continue;
		}
		if (ep >= info->lastFontPos) {
		    fontdesc_CharSummary(info->itemAtDot->ti_font,
                      textview_GetDrawable(self), chr, &cinfo);
		    if (WillClear && cinfo.width <=
                      cinfo.xSpacing + cinfo.xOriginOffset)
			break;
		    WillClear = (cinfo.xOriginOffset <= 0);
		}
	    }
	    ep++;
	    if (ep < cp)
                ep = cp;
	    mark_SetPos(self->prepredrawn, cp);
	    mark_SetLength(self->prepredrawn, ep - cp);
	    self->prepredrawn->modified = 0;
	    self->predrawnY = y + info->lineAbove;
	    self->predrawnX = x;
	} else {
	    /* The mark has been moved, thus this info is useless. */
	    self->prepredrawn->modified = 1;
	    self->predrawn->modified = 1;
	}
    }

    if (txheight)
        *txheight = info->lineAbove + info->textBelow;

    if ((type == textview_FullLineRedraw ||
      type == textview_PartialLineRedraw) && cont)
        *cont = (info->inContinueMode && ! info->endOfPara);

    if (type == textview_GetPosition ||
      type == textview_GetCoordinate)
        return info->locateX;	/* We were just looking */

    return info->lineAbove + info->lineBelow;
}

void textview__ViewMove(self, lineStructure, movement)
struct textview *self;
struct linedesc *lineStructure;
long movement;
{
    struct text *text = Text(self);
    struct environment *curenv;
    struct view *CurView;
    long end, i, elen;
    struct rectangle enclosingRect;
    struct mark *currentLine;

    currentLine = lineStructure->data;
    i =  mark_GetPos(currentLine);
    end = i + lineStructure->nChars;

    while (i < end) {
	curenv = environment_GetInnerMost(text->rootEnvironment, i);
	elen = environment_GetNextChange(text->rootEnvironment, i);
	if (elen + i > end)
	    elen = end - i;
        if (curenv->type == environment_View &&
          ((CurView = (struct view *) dictionary_LookUp(self,
          (char *) (curenv->data.viewref))) != NULL)) {
              textview_RetractViewCursors(self, CurView);
              if (movement == textview_REMOVEVIEW) {
                  rectangle_SetRectSize(&enclosingRect, 0, 0, 0, 0);
                  view_InsertView(CurView, self, &enclosingRect);
                  view_FullUpdate(CurView, view_Remove, 0, 0, 0, 0);
              } else {
                  view_GetEnclosedBounds(CurView, &enclosingRect);
                  rectangle_Top(&enclosingRect) += movement;
                  view_InsertView(CurView, self, &enclosingRect);
                  view_FullUpdate(CurView,
                    view_MoveNoRedraw, 0, 0, 0, 0);
              }
          }
        elen += i;
        i = elen;
    }
}
