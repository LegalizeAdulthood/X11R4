#ifndef lint
static char rcsid[] = "$Header: Draw.c,v 1.2 89/12/15 11:59:08 kit Exp $";
#endif

/*
 * Copyright 1989 Software Research Associates, Inc., Tokyo, Japan
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Software Research Associates
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * SOFTWARE RESEARCH ASSOCIATES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL SOFTWARE RESEARCH ASSOCIATES BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Erik M. van der Poel
 *         Software Research Associates, Inc., Tokyo, Japan
 *         erik@sra.co.jp
 */

#include <stdio.h>
#include "SFinternal.h"
#include <sys/stat.h>
#include <X11/StringDefs.h>

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)

#include <Xw/Xw.h>
#include <Xw/ScrollBar.h>

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
#include <X11/Scroll.h>
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
#include <X11/Xaw/Scrollbar.h>
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

#define SF_DEFAULT_FONT "9x15"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

typedef struct {
	char *fontname;
} TextData, *textPtr;

int SFcharWidth, SFcharAscent, SFcharHeight;

int SFcurrentInvert[3] = { -1, -1, -1 };

static GC SFlineGC, SFscrollGC, SFinvertGC, SFtextGC;

static XtResource textResources[] = {
	{XtNfont, XtCFont, XtRString, sizeof (char *),
		XtOffset(textPtr, fontname), XtRString, SF_DEFAULT_FONT},
};

static XFontStruct *SFfont;

static int SFcurrentListY;

static XtIntervalId SFscrollTimerId;

SFinitFont()
{
	TextData	*data;

	data = XtNew(TextData);

	XtGetApplicationResources(selFileForm, (caddr_t) data, textResources,
		XtNumber(textResources), (Arg *) NULL, 0);

	SFfont = XLoadQueryFont(SFdisplay, data->fontname);
	if (!SFfont) {
		SFfont = XLoadQueryFont(SFdisplay, SF_DEFAULT_FONT);
		if (!SFfont) {
			char	sbuf[256];

			(void) sprintf(sbuf, "XsraSelFile: can't get font %s",
				SF_DEFAULT_FONT);

#ifdef SEL_FILE_R2_XT

			XtError(sbuf);

#else /* def SEL_FILE_R2_XT */

			XtAppError(SFapp, sbuf);

#endif /* def SEL_FILE_R2_XT */

		}
	}

	SFcharWidth = (SFfont->max_bounds.width + SFfont->min_bounds.width) / 2;
	SFcharAscent = SFfont->max_bounds.ascent;
	SFcharHeight = SFcharAscent + SFfont->max_bounds.descent;
}

SFcreateGC()
{
	XGCValues	gcValues;
	XRectangle	rectangles[1];

	gcValues.foreground = SFfore;

	SFlineGC = XtGetGC(
		selFileLists[0],
		(XtGCMask)
			GCForeground		|
			0,
		&gcValues
	);

	SFscrollGC = XtGetGC(
		selFileLists[0],
		(XtGCMask)
			0,
		&gcValues
	);

	gcValues.function = GXinvert;
	gcValues.plane_mask = (SFfore ^ SFback);

	SFinvertGC = XtGetGC(
		selFileLists[0],
		(XtGCMask)
			GCFunction		|
			GCPlaneMask		|
			0,
		&gcValues
	);

	gcValues.foreground = SFfore;
	gcValues.background = SFback;
	gcValues.font = SFfont->fid;

	SFtextGC = XCreateGC(
		SFdisplay,
		XtWindow(selFileLists[0]),
		(u_long)
			GCForeground		|
			GCBackground		|
			GCFont			|
			0,
		&gcValues
	);

	rectangles[0].x = SFlineToTextH + SFbesideText;
	rectangles[0].y = 0;
	rectangles[0].width = SFcharsPerEntry * SFcharWidth;
	rectangles[0].height = SFupperY + 1;

	XSetClipRectangles(
		SFdisplay,
		SFtextGC,
		0,
		0,
		rectangles,
		1,
		Unsorted
	);
}

SFclearList(n, doScroll)
	int	n;
	int	doScroll;
{
	SFDir	*dir;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	int	i;
	Arg	arglist[20];
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	SFcurrentInvert[n] = -1;

	XClearWindow(SFdisplay, XtWindow(selFileLists[n]));

	XDrawSegments(SFdisplay, XtWindow(selFileLists[n]), SFlineGC, SFsegs,
		2);

	if (doScroll) {
		dir = &(SFdirs[SFdirPtr + n]);

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
		i = 0;
		if ((SFdirPtr + n < SFdirEnd) && dir->nEntries) {
			XtSetArg(arglist[i], XtNsliderMax, dir->nEntries);
									i++;
			XtSetArg(arglist[i], XtNsliderExtent,
				dir->nEntries < SFlistSize ? dir->nEntries :
				SFlistSize);				i++;
			XtSetArg(arglist[i], XtNsliderOrigin, dir->vOrigin);
									i++;
		} else {
			XtSetArg(arglist[i], XtNsliderMax, SFlistSize);	i++;
			XtSetArg(arglist[i], XtNsliderExtent, SFlistSize);
									i++;
			XtSetArg(arglist[i], XtNsliderOrigin, 0);	i++;
		}
		XtSetValues(selFileVScrolls[n], arglist, i);

		i = 0;
		if ((SFdirPtr + n < SFdirEnd) && dir->nChars) {
			XtSetArg(arglist[i], XtNsliderMax, dir->nChars);i++;
			XtSetArg(arglist[i], XtNsliderExtent,
				dir->nChars < SFcharsPerEntry ? dir->nChars :
				SFcharsPerEntry);			i++;
			XtSetArg(arglist[i], XtNsliderOrigin, dir->hOrigin);
									i++;
		} else {
			XtSetArg(arglist[i], XtNsliderMax, SFcharsPerEntry);
									i++;
			XtSetArg(arglist[i], XtNsliderExtent, SFcharsPerEntry);
									i++;
			XtSetArg(arglist[i], XtNsliderOrigin, 0);	i++;
		}
		XtSetValues(selFileHScrolls[n], arglist, i);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
		if ((SFdirPtr + n < SFdirEnd) && dir->nEntries && dir->nChars) {

#ifdef SEL_FILE_PRE_R4_XAW_XMU
			XtScrollBarSetThumb(
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
			XawScrollBarSetThumb(
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

				selFileVScrolls[n],
				(float) (((double) dir->vOrigin) /
					dir->nEntries),
				(float) (((double) ((dir->nEntries < SFlistSize)
					? dir->nEntries : SFlistSize)) /
					dir->nEntries)
			);

#ifdef SEL_FILE_PRE_R4_XAW_XMU
			XtScrollBarSetThumb(
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
			XawScrollBarSetThumb(
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

				selFileHScrolls[n],
				(float) (((double) dir->hOrigin) / dir->nChars),
				(float) (((double) ((dir->nChars <
					SFcharsPerEntry) ? dir->nChars :
					SFcharsPerEntry)) / dir->nChars)
			);
		} else {

#ifdef SEL_FILE_PRE_R4_XAW_XMU
			XtScrollBarSetThumb(selFileVScrolls[n], (float) 0.0,
				(float) 1.0);
			XtScrollBarSetThumb(selFileHScrolls[n], (float) 0.0,
				(float) 1.0);
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
			XawScrollBarSetThumb(selFileVScrolls[n], (float) 0.0,
				(float) 1.0);
			XawScrollBarSetThumb(selFileHScrolls[n], (float) 0.0,
				(float) 1.0);
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

		}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	}
}

static
SFdeleteEntry(dir, entry)
	SFDir	*dir;
	SFEntry	*entry;
{
	register SFEntry	*e;
	register SFEntry	*end;
	int			n;
	int			idx;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)

	int			i;
	Arg			arglist[20];

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	idx = entry - dir->entries;

	if (idx < dir->beginSelection) {
		dir->beginSelection--;
	}
	if (idx <= dir->endSelection) {
		dir->endSelection--;
	}
	if (dir->beginSelection > dir->endSelection) {
		dir->beginSelection = dir->endSelection = -1;
	}

	if (idx < dir->vOrigin) {
		dir->vOrigin--;
	}

	XtFree(entry->real);

	end = &(dir->entries[dir->nEntries - 1]);

	for (e = entry; e < end; e++) {
		*e = *(e + 1);
	}

	if (!(--dir->nEntries)) {
		return;
	}

	n = dir - &(SFdirs[SFdirPtr]);
	if ((n < 0) || (n > 2)) {
		return;
	}

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)

	i = 0;
	XtSetArg(arglist[i], XtNsliderMax, dir->nEntries);		i++;
	XtSetArg(arglist[i], XtNsliderExtent, dir->nEntries < SFlistSize ?
		dir->nEntries : SFlistSize);				i++;
	XtSetArg(arglist[i], XtNsliderOrigin, dir->vOrigin);		i++;
	XtSetValues(selFileVScrolls[n], arglist, i);

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
	XtScrollBarSetThumb(
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
	XawScrollBarSetThumb(
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

		selFileVScrolls[n],
		(float) (((double) dir->vOrigin) / dir->nEntries),
		(float) (((double) ((dir->nEntries < SFlistSize) ?
			dir->nEntries : SFlistSize)) / dir->nEntries)
	);

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

}

static
SFwriteStatChar(name, last, statBuf)
	char		*name;
	int		last;
	struct stat	*statBuf;
{
	switch (statBuf->st_mode & S_IFMT) {
	case S_IFDIR:
		name[last] = '/';
		break;
	case S_IFREG:
		if (statBuf->st_mode & 0111) {
			name[last] = '*';
		} else {
			name[last] = ' ';
		}
		break;

#ifdef S_IFSOCK
	case S_IFSOCK:
		name[last] = '=';
		break;
#endif /* def S_IFSOCK */

	default:
		name[last] = ' ';
		break;
	}
}

static int
SFstatAndCheck(dir, entry)
	SFDir	*dir;
	SFEntry	*entry;
{
	struct stat	statBuf;
	char		save;
	int		last;

	/*
	 * must be restored before returning
	 */
	save = *(dir->path);
	*(dir->path) = 0;

	if (!SFchdir(SFcurrentPath)) {
		last = strlen(entry->real) - 1;
		entry->real[last] = 0;
		entry->statDone = 1;
		if (
			(!stat(entry->real, &statBuf))

#ifdef S_IFLNK

		     || (!lstat(entry->real, &statBuf))

#endif /* ndef S_IFLNK */

		) {
			if (SFfunc) {
				char *shown;

				shown = NULL;
				if (SFfunc(entry->real, &shown, &statBuf)) {
					if (shown) {
						int len;

						len = strlen(shown);
						entry->shown = XtMalloc(
							(unsigned) (len + 2)
						);
						(void) strcpy(entry->shown,
							shown);
						SFwriteStatChar(
							entry->shown,
							len,
							&statBuf
						);
						entry->shown[len + 1] = 0;
					}
				} else {
					SFdeleteEntry(dir, entry);

					*(dir->path) = save;
					return 1;
				}
			}
			SFwriteStatChar(entry->real, last, &statBuf);
		} else {
			entry->real[last] = ' ';
		}
	}

	*(dir->path) = save;
	return 0;
}

static
SFdrawStrings(w, dir, from, to)
	register Window	w;
	register SFDir	*dir;
	register int	from;
	register int	to;
{
	register int		i;
	register SFEntry	*entry;
	int			x;

	x = SFtextX - dir->hOrigin * SFcharWidth;

	if (dir->vOrigin + to >= dir->nEntries) {
		to = dir->nEntries - dir->vOrigin - 1;
	}
	for (i = from; i <= to; i++) {
		entry = &(dir->entries[dir->vOrigin + i]);
		if (!(entry->statDone)) {
			if (SFstatAndCheck(dir, entry)) {
				if (dir->vOrigin + to >= dir->nEntries) {
					to = dir->nEntries - dir->vOrigin - 1;
				}
				i--;
				continue;
			}
		}
		XDrawImageString(
			SFdisplay,
			w,
			SFtextGC,
			x,
			SFtextYoffset + i * SFentryHeight,
			entry->shown,
			strlen(entry->shown)
		);
		if (dir->vOrigin + i == dir->beginSelection) {
			XDrawLine(
				SFdisplay,
				w,
				SFlineGC,
				SFlineToTextH + 1,
				SFlowerY + i * SFentryHeight,
				SFlineToTextH + SFentryWidth - 2,
				SFlowerY + i * SFentryHeight
			);
		}
		if (
			(dir->vOrigin + i >= dir->beginSelection) &&
			(dir->vOrigin + i <= dir->endSelection)
		) {
			SFcompletionSegs[0].y1 = SFcompletionSegs[1].y1 =
				SFlowerY + i * SFentryHeight;
			SFcompletionSegs[0].y2 = SFcompletionSegs[1].y2 =
				SFlowerY + (i + 1) * SFentryHeight - 1;
			XDrawSegments(
				SFdisplay,
				w,
				SFlineGC,
				SFcompletionSegs,
				2
			);
		}
		if (dir->vOrigin + i == dir->endSelection) {
			XDrawLine(
				SFdisplay,
				w,
				SFlineGC,
				SFlineToTextH + 1,
				SFlowerY + (i + 1) * SFentryHeight - 1,
				SFlineToTextH + SFentryWidth - 2,
				SFlowerY + (i + 1) * SFentryHeight - 1
			);
		}
	}
}

SFdrawList(n, doScroll)
	int	n;
	int	doScroll;
{
	SFDir	*dir;
	Window	w;

	SFclearList(n, doScroll);

	if (SFdirPtr + n < SFdirEnd) {
		dir = &(SFdirs[SFdirPtr + n]);
		w = XtWindow(selFileLists[n]);
		XDrawImageString(
			SFdisplay,
			w,
			SFtextGC,
			SFtextX - dir->hOrigin * SFcharWidth,
			SFlineToTextV + SFaboveAndBelowText + SFcharAscent,
			dir->dir,
			strlen(dir->dir)
		);
		SFdrawStrings(w, dir, 0, SFlistSize - 1);
	}
}

SFdrawLists(doScroll)
	int	doScroll;
{
	int	i;

	for (i = 0; i < 3; i++) {
		SFdrawList(i, doScroll);
	}
}

static
SFinvertEntry(n)
	register int	n;
{
	XFillRectangle(
		SFdisplay,
		XtWindow(selFileLists[n]),
		SFinvertGC,
		SFlineToTextH,
		SFcurrentInvert[n] * SFentryHeight + SFlowerY,
		SFentryWidth,
		SFentryHeight
	);
}

static unsigned long
SFscrollTimerInterval()
{
	static int	maxVal = 200;
	static int	varyDist = 50;
	static int	minDist = 50;
	int		t;
	int		dist;

	if (SFcurrentListY < SFlowerY) {
		dist = SFlowerY - SFcurrentListY;
	} else if (SFcurrentListY > SFupperY) {
		dist = SFcurrentListY - SFupperY;
	} else {
		return (unsigned long) 1;
	}

	t = maxVal - ((maxVal / varyDist) * (dist - minDist));

	if (t < 1) {
		t = 1;
	}

	if (t > maxVal) {
		t = maxVal;
	}

	return (unsigned long) t;
}

static
SFscrollTimer(n)
	int	n;
{
	SFDir	*dir;
	int	save;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	int	i;
	Arg	arglist[20];
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	dir = &(SFdirs[SFdirPtr + n]);
	save = dir->vOrigin;

	if (SFcurrentListY < SFlowerY) {
		if (dir->vOrigin > 0) {
			SFvSliderMovedCallback(selFileVScrolls[n], n,
				dir->vOrigin - 1);
		}
	} else if (SFcurrentListY > SFupperY) {
		if (dir->vOrigin < dir->nEntries - SFlistSize) {
			SFvSliderMovedCallback(selFileVScrolls[n], n,
				dir->vOrigin + 1);
		}
	}

	if (dir->vOrigin != save) {

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
		i = 0;
		XtSetArg(arglist[i], XtNsliderOrigin, dir->vOrigin);	i++;
		XtSetValues(selFileVScrolls[n], arglist, i);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
		if (dir->nEntries) {

#ifdef SEL_FILE_PRE_R4_XAW_XMU
		    XtScrollBarSetThumb(
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
		    XawScrollBarSetThumb(
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

			selFileVScrolls[n],
			(float) (((double) dir->vOrigin) / dir->nEntries),
			(float) (((double) ((dir->nEntries < SFlistSize) ?
				dir->nEntries : SFlistSize)) / dir->nEntries)
		    );
		}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	}

	if (SFbuttonPressed) {

#ifdef SEL_FILE_R2_XT

		SFscrollTimerId = XtAddTimeOut(

#else /* def SEL_FILE_R2_XT */

		SFscrollTimerId = XtAppAddTimeOut(SFapp,

#endif /* def SEL_FILE_R2_XT */

			SFscrollTimerInterval(), SFscrollTimer, (caddr_t) n);
	}
}

static int
SFnewInvertEntry(n, event)
	register int		n;
	register XMotionEvent	*event;
{
	register int	x, y;
	register int	new;
	static int	SFscrollTimerAdded = 0;

	x = event->x;
	y = event->y;

	if (SFdirPtr + n >= SFdirEnd) {
		return -1;
	} else if (
		(x >= 0)	&& (x <= SFupperX) &&
		(y >= SFlowerY)	&& (y <= SFupperY)
	) {
		register SFDir *dir = &(SFdirs[SFdirPtr + n]);

		if (SFscrollTimerAdded) {
			SFscrollTimerAdded = 0;
			XtRemoveTimeOut(SFscrollTimerId);
		}

		new = (y - SFlowerY) / SFentryHeight;
		if (dir->vOrigin + new >= dir->nEntries) {
			return -1;
		}
		return new;
	} else {
		if (SFbuttonPressed) {
			SFcurrentListY = y;
			if (!SFscrollTimerAdded) {
				SFscrollTimerAdded = 1;

#ifdef SEL_FILE_R2_XT

				SFscrollTimerId = XtAddTimeOut(

#else /* def SEL_FILE_R2_XT */

				SFscrollTimerId = XtAppAddTimeOut(SFapp,

#endif /* def SEL_FILE_R2_XT */

					SFscrollTimerInterval(), SFscrollTimer,
					(caddr_t) n);
			}
		}

		return -1;
	}
}

/* ARGSUSED */
void
SFenterList(w, n, event)
	Widget				w;
	register int			n;
	register XEnterWindowEvent	*event;
{
	register int	new;

	/* sanity */
	if (SFcurrentInvert[n] != -1) {
		SFinvertEntry(n);
		SFcurrentInvert[n] = -1;
	}

	new = SFnewInvertEntry(n, (XMotionEvent *) event);
	if (new != -1) {
		SFcurrentInvert[n] = new;
		SFinvertEntry(n);
	}
}

/* ARGSUSED */
void
SFleaveList(w, n, event)
	Widget		w;
	register int	n;
	XEvent		*event;
{
	if (SFcurrentInvert[n] != -1) {
		SFinvertEntry(n);
		SFcurrentInvert[n] = -1;
	}
}

/* ARGSUSED */
void
SFmotionList(w, n, event)
	Widget			w;
	register int		n;
	register XMotionEvent	*event;
{
	register int	new;

	new = SFnewInvertEntry(n, event);

	if (new != SFcurrentInvert[n]) {
		if (SFcurrentInvert[n] != -1) {
			SFinvertEntry(n);
		}
		SFcurrentInvert[n] = new;
		if (new != -1) {
			SFinvertEntry(n);
		}
	}
}

/* ARGSUSED */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
SFvFloatSliderMovedCallback(w, n, fnew)
	Widget	w;
	int	n;
	float	*fnew;
{
	int	new;

	new = (*fnew) * SFdirs[SFdirPtr + n].nEntries;

	SFvSliderMovedCallback(w, n, new);
}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

/* ARGSUSED */
SFvSliderMovedCallback(w, n, new)
	Widget	w;
	int	n;
	int	new;
{
	int		old;
	register Window	win;
	SFDir		*dir;

	dir = &(SFdirs[SFdirPtr + n]);

	old = dir->vOrigin;
	dir->vOrigin = new;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	if (old == new) {
		return;
	}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	win = XtWindow(selFileLists[n]);

	if (ABS(new - old) < SFlistSize) {
		if (new > old) {
			XCopyArea(
				SFdisplay,
				win,
				win,
				SFscrollGC,
				SFlineToTextH,
				SFlowerY + (new - old) * SFentryHeight,
				SFentryWidth + SFlineToTextH,
				(SFlistSize - (new - old)) * SFentryHeight,
				SFlineToTextH,
				SFlowerY
			);
			XClearArea(
				SFdisplay,
				win,
				SFlineToTextH,
				SFlowerY + (SFlistSize - (new - old)) *
					SFentryHeight,
				SFentryWidth + SFlineToTextH,
				(new - old) * SFentryHeight,
				False
			);
			SFdrawStrings(win, dir, SFlistSize - (new - old),
				SFlistSize - 1);
		} else {
			XCopyArea(
				SFdisplay,
				win,
				win,
				SFscrollGC,
				SFlineToTextH,
				SFlowerY,
				SFentryWidth + SFlineToTextH,
				(SFlistSize - (old - new)) * SFentryHeight,
				SFlineToTextH,
				SFlowerY + (old - new) * SFentryHeight
			);
			XClearArea(
				SFdisplay,
				win,
				SFlineToTextH,
				SFlowerY,
				SFentryWidth + SFlineToTextH,
				(old - new) * SFentryHeight,
				False
			);
			SFdrawStrings(win, dir, 0, old - new);
		}
	} else {
		XClearArea(
			SFdisplay,
			win,
			SFlineToTextH,
			SFlowerY,
			SFentryWidth + SFlineToTextH,
			SFlistSize * SFentryHeight,
			False
		);
		SFdrawStrings(win, dir, 0, SFlistSize - 1);
	}
}

/* ARGSUSED */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
SFvAreaSelectedCallback(w, n, new)
	Widget	w;
	int	n;
	int	new;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
SFvAreaSelectedCallback(w, n, pnew)
	Widget	w;
	int	n;
	int	pnew;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

{
	SFDir	*dir;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	int	i;
	Arg	arglist[20];
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	int	new;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	dir = &(SFdirs[SFdirPtr + n]);

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	new -= (SFlistSize / 2);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	new = dir->vOrigin +
		(((double) pnew) / SFvScrollHeight) * dir->nEntries;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	if (new > dir->nEntries - SFlistSize) {
		new = dir->nEntries - SFlistSize;
	}

	if (new < 0) {
		new = 0;
	}

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	i = 0;
	XtSetArg(arglist[i], XtNsliderOrigin, new);			i++;
	XtSetValues(w, arglist, i);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	if (dir->nEntries) {
		float	f;

		f = ((double) new) / dir->nEntries;

#ifdef SEL_FILE_PRE_R4_XAW_XMU
		XtScrollBarSetThumb(
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
		XawScrollBarSetThumb(
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

			w,
			f,
			(float) (((double) ((dir->nEntries < SFlistSize) ?
				dir->nEntries : SFlistSize)) / dir->nEntries)
		);
	}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	SFvSliderMovedCallback(w, n, new);
}

/* ARGSUSED */
SFhSliderMovedCallback(w, n, new)
	Widget	w;
	int	n;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	int	new;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	float	*new;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

{

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	SFdirs[SFdirPtr + n].hOrigin = new;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	{
		SFDir	*dir;
		int	save;

		dir = &(SFdirs[SFdirPtr + n]);
		save = dir->hOrigin;
		dir->hOrigin = (*new) * dir->nChars;
		if (dir->hOrigin == save) {
			return;
		}
	}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	SFdrawList(n, SF_DO_NOT_SCROLL);
}

/* ARGSUSED */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
SFhAreaSelectedCallback(w, n, new)
	Widget	w;
	int	n;
	int	new;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
SFhAreaSelectedCallback(w, n, pnew)
	Widget	w;
	int	n;
	int	pnew;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

{
	SFDir	*dir;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	int	i;
	Arg	arglist[20];
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	int	new;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	dir = &(SFdirs[SFdirPtr + n]);

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	new -= (SFcharsPerEntry / 2);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	new = dir->hOrigin +
		(((double) pnew) / SFhScrollWidth) * dir->nChars;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	if (new > dir->nChars - SFcharsPerEntry) {
		new = dir->nChars - SFcharsPerEntry;
	}

	if (new < 0) {
		new = 0;
	}

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	i = 0;
	XtSetArg(arglist[i], XtNsliderOrigin, new);			i++;
	XtSetValues(w, arglist, i);

	SFhSliderMovedCallback(w, n, new);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	if (dir->nChars) {
		float	f;

		f = ((double) new) / dir->nChars;

#ifdef SEL_FILE_PRE_R4_XAW_XMU
		XtScrollBarSetThumb(
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
		XawScrollBarSetThumb(
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

			w,
			f,
			(float) (((double) ((dir->nChars < SFcharsPerEntry) ?
				dir->nChars : SFcharsPerEntry)) / dir->nChars)
		);

		SFhSliderMovedCallback(w, n, &f);
	}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

}

/* ARGSUSED */
SFpathSliderMovedCallback(w, client_data, new)
	Widget	w;
	caddr_t	client_data;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	int	new;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	float	*new;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

{
	SFDir		*dir;
	int		n;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)

	XwTextPosition	pos;

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
	XtTextPosition	pos;
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
	XawTextPosition	pos;
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	SFdirPtr = new;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	{
		int	SFdirPtrSave;

		SFdirPtrSave = SFdirPtr;
		SFdirPtr = (*new) * SFdirEnd;
		if (SFdirPtr == SFdirPtrSave) {
			return;
		}
	}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	SFdrawLists(SF_DO_SCROLL);

	n = 2;
	while (SFdirPtr + n >= SFdirEnd) {
		n--;
	}

	dir = &(SFdirs[SFdirPtr + n]);

	pos = dir->path - SFcurrentPath;

	if (!strncmp(SFcurrentPath, SFstartDir, strlen(SFstartDir))) {
		pos -= strlen(SFstartDir);
		if (pos < 0) {
			pos = 0;
		}
	}

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)

	XwTextSetInsertPos(selFileField, pos);

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
	XtTextSetInsertionPoint(selFileField, pos);
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
	XawTextSetInsertionPoint(selFileField, pos);
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

}

/* ARGSUSED */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
SFpathAreaSelectedCallback(w, client_data, new)
	Widget	w;
	caddr_t	client_data;
	int	new;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
SFpathAreaSelectedCallback(w, client_data, pnew)
	Widget	w;
	caddr_t	client_data;
	int	pnew;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

{

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	int	i;
	Arg	arglist[20];
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	int	new;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	new -= (3 / 2);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	new = SFdirPtr + (((double) pnew) / SFpathScrollWidth) * SFdirEnd;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	if (new > SFdirEnd - 3) {
		new = SFdirEnd - 3;
	}

	if (new < 0) {
		new = 0;
	}

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	i = 0;
	XtSetArg(arglist[i], XtNsliderOrigin, new);			i++;
	XtSetValues(w, arglist, i);

	SFpathSliderMovedCallback(w, client_data, new);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	{
		float	f;

		f = ((double) new) / SFdirEnd;

#ifdef SEL_FILE_PRE_R4_XAW_XMU
		XtScrollBarSetThumb(
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
		XawScrollBarSetThumb(
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

			w,
			f,
			(float) (((double) ((SFdirEnd < 3) ? SFdirEnd : 3)) /
				SFdirEnd)
		);

		SFpathSliderMovedCallback(w, (caddr_t) NULL, &f);
	}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

}

#ifndef SEL_FILE_R2_XT

Boolean
SFworkProc()
{
	register SFDir		*dir;
	register SFEntry	*entry;

	for (dir = &(SFdirs[SFdirEnd - 1]); dir >= SFdirs; dir--) {
		if (!(dir->nEntries)) {
			continue;
		}
		for (
			entry = &(dir->entries[dir->nEntries - 1]);
			entry >= dir->entries;
			entry--
		) {
			if (!(entry->statDone)) {
				(void) SFstatAndCheck(dir, entry);
				return False;
			}
		}
	}

	SFworkProcAdded = 0;

	return True;
}

#endif /* ndef SEL_FILE_R2_XT */
