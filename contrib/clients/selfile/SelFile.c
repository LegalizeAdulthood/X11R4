#ifndef lint
static char rcsid[] = "$Header: SelFile.c,v 1.2 89/12/15 11:59:32 kit Exp $";
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

/*
 * Author's address:
 *
 *     erik@sra.co.jp
 *                                            OR
 *     erik%sra.co.jp@uunet.uu.net
 *                                            OR
 *     erik%sra.co.jp@mcvax.uucp
 *                                            OR
 *     try junet instead of co.jp
 *                                            OR
 *     Erik M. van der Poel
 *     Software Research Associates, Inc.
 *     1-1-1 Hirakawa-cho, Chiyoda-ku
 *     Tokyo 102 Japan. TEL +81-3-234-2692
 */

#include <stdio.h>
#include <sys/param.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Composite.h>
#include <X11/Shell.h>

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)

#include <Xw/Xw.h>
#include <Xw/Form.h>

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
#include <X11/Form.h>
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
#include <X11/Xaw/Form.h>
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)

#include <Xw/Xw.h>
#include <Xw/PButton.h>

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
#include <X11/Command.h>
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
#include <X11/Xaw/Command.h>
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

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

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT)

#include <Xw/Xw.h>
#include <Xw/SText.h>

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
#include <X11/Label.h>
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
#include <X11/Xaw/Label.h>
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT) */

#include "SFinternal.h"

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
#ifdef SEL_FILE_JAPANESE
#include <X11/XWStr.h>
#endif /* def SEL_FILE_JAPANESE */
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif /* ndef MAXPATHLEN */

#if !defined(SYSV) && !defined(USG)
extern char *getwd();
#endif /* !defined(SYSV) && !defined(USG) */

int SFstatus = SEL_FILE_NULL;

char
	SFstartDir[MAXPATHLEN],
	SFcurrentPath[MAXPATHLEN],
	SFcurrentDir[MAXPATHLEN];

Widget
	selFile,
	selFileCancel,
	selFileField,
	selFileForm,
	selFileHScroll,
	selFileHScrolls[3],
	selFileLists[3],
	selFileOK,
	selFilePrompt,
	selFileVScrolls[3];

Display *SFdisplay;

Pixel SFfore, SFback;

XSegment SFsegs[2], SFcompletionSegs[2];

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)

XwTextPosition SFtextPos;

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
XtTextPosition SFtextPos;
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
XawTextPosition SFtextPos;
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

int SFupperX, SFlowerY, SFupperY;

int SFtextX, SFtextYoffset;

int SFentryWidth, SFentryHeight;

int SFlineToTextH = 3;

int SFlineToTextV = 3;

int SFbesideText = 3;

int SFaboveAndBelowText = 2;

int SFcharsPerEntry = 15;

int SFlistSize = 10;

#ifndef SEL_FILE_R2_XT

int SFworkProcAdded = 0;

XtAppContext SFapp;

#endif /* ndef SEL_FILE_R2_XT */

int SFpathScrollWidth, SFvScrollHeight, SFhScrollWidth;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */
char SFtextBuffer[MAXPATHLEN];
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

XtIntervalId SFdirModTimerId;

int (*SFfunc)();

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
static char *oneLineTextEditTranslations = "\
	<Key>Return:	execute()\n\
	Ctrl<Key>M:	execute()\n\
";
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */
static char *oneLineTextEditTranslations = "\
	<Key>Return:	redraw-display()\n\
	Ctrl<Key>M:	redraw-display()\n\
";
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

/* ARGSUSED */
static void
SFexposeList(w, n, event)
	Widget		w;
	int		n;
	XExposeEvent	*event;
{
	if ((event->type == NoExpose) || event->count) {
		return;
	}

	SFdrawList(n, SF_DO_NOT_SCROLL);
}

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
static void
returnCallback()
{
	SFstatus = SEL_FILE_OK;
}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

/* ARGSUSED */
static void
SFmodVerifyCallback(w, client_data, event)
	Widget			w;
	caddr_t			client_data;
	XKeyPressedEvent	*event;
{

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
	SFstatus = SEL_FILE_TEXT;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */
	char	buf[2];

	if (
		(XLookupString(event, buf, 2, NULL, NULL) == 1) &&
		((*buf) == '\r')
	) {
		SFstatus = SEL_FILE_OK;
	} else {
		SFstatus = SEL_FILE_TEXT;
	}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

}

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
static XtCallbackRec SFmodVerify[] = {
	{ SFmodVerifyCallback, (caddr_t) NULL },
	{ NULL, (caddr_t) NULL },
};
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)
static
SFresetButton(w)
	Widget w;
{
	static Arg	arg[] = {
		{ XtNset, False }
	};

	XtSetValues(w, arg, 1);
}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

/* ARGSUSED */
static void
SFokCallback(w)
	Widget	w;
{
	SFstatus = SEL_FILE_OK;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)
	SFresetButton(w);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

}

static XtCallbackRec SFokSelect[] = {
	{ SFokCallback, (caddr_t) NULL },
	{ NULL, (caddr_t) NULL },
};

/* ARGSUSED */
static void
SFcancelCallback(w)
	Widget	w;
{
	SFstatus = SEL_FILE_CANCEL;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)
	SFresetButton(w);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

}

static XtCallbackRec SFcancelSelect[] = {
	{ SFcancelCallback, (caddr_t) NULL },
	{ NULL, (caddr_t) NULL },
};

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)

#ifdef SEL_FILE_R2_XT

typedef struct {
	int	height;
} SFfieldData, *sfFieldPtr;

static XtResource SFfieldResources[] = {
	{
		XtNheight, XtCHeight, XtRInt, sizeof(int),
		XtOffset(sfFieldPtr, height), XtRString, "1"
	},
};

#else /* def SEL_FILE_R2_XT */

typedef struct {
	Dimension	height;
} SFfieldData, *sfFieldPtr;

static XtResource SFfieldResources[] = {
	{
		XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
		XtOffset(sfFieldPtr, height), XtRString, "1"
	},
};

#endif /* def SEL_FILE_R2_XT */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

static int
SFcreateWidgets(prompt, ok, cancel)
	u_char	*prompt;
	u_char	*ok;
	u_char	*cancel;
{
	int		i, n;
	int		listWidth, listHeight;
	int		listSpacing = 10;
	int		scrollThickness = 15;
	int		hScrollX, hScrollY;
	int		vScrollX, vScrollY;
	Cursor
			xtermCursor,

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
			sbDownArrowCursor,
			sbLeftArrowCursor,
			sbUpArrowCursor,
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

			sbRightArrowCursor,
			dotCursor;

#ifdef SEL_FILE_JAPANESE
	CStr		csl[2];
	u_char		cstr[256];
#endif /* def SEL_FILE_JAPANESE */

	Arg		arglist[24];

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
	SFfieldData	*data;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

	i = 0;
	XtSetArg(arglist[i], XtNallowShellResize, True);		i++;
	XtSetArg(arglist[i], XtNinput, True);				i++;

#ifdef SEL_FILE_R2_XT

	selFile = XtCreateApplicationShell("selFile",
		applicationShellWidgetClass, arglist, i);

#else /* def SEL_FILE_R2_XT */

	selFile = XtAppCreateShell("selFile", "SelFile",
		applicationShellWidgetClass, SFdisplay, arglist, i);

#endif /* def SEL_FILE_R2_XT */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)
	selFileForm = XtCreateManagedWidget("selFileForm",
		XwformWidgetClass, selFile, (ArgList) NULL, 0);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */
	i = 0;
	XtSetArg(arglist[i], XtNdefaultDistance, 30);			i++;
	selFileForm = XtCreateManagedWidget("selFileForm",
		formWidgetClass, selFile, arglist, i);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

	i = 0;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT)

#ifdef SEL_FILE_JAPANESE

#ifdef SEL_FILE_SJIS
	(void) convSJIStoCS(prompt, cstr);
#else /* def SEL_FILE_SJIS */
	(void) convEUCtoCS(prompt, cstr);
#endif /* def SEL_FILE_SJIS */

	csl[0] = cstr;
	csl[1] = NULL;
	XtSetArg(arglist[i], XtNstrings, csl);				i++;
#else /* def SEL_FILE_JAPANESE */
	XtSetArg(arglist[i], XtNstring, prompt);			i++;
#endif /* def SEL_FILE_JAPANESE */

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT) */
	XtSetArg(arglist[i], XtNlabel, prompt);				i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)
	XtSetArg(arglist[i], XtNxOffset, 30);				i++;
	XtSetArg(arglist[i], XtNyOffset, 30);				i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

	XtSetArg(arglist[i], XtNborderWidth, 0);			i++;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT)
	selFilePrompt = XtCreateManagedWidget("selFilePrompt",
		XwstatictextWidgetClass, selFileForm, arglist, i);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT) */
	selFilePrompt = XtCreateManagedWidget("selFilePrompt",
		labelWidgetClass, selFileForm, arglist, i);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT) */

	i = 0;
	XtSetArg(arglist[i], XtNforeground, &SFfore);			i++;
	XtSetArg(arglist[i], XtNbackground, &SFback);			i++;
	XtGetValues(selFilePrompt, arglist, i);

	SFinitFont();

	SFentryWidth = SFbesideText + SFcharsPerEntry * SFcharWidth +
			SFbesideText;
	SFentryHeight = SFaboveAndBelowText + SFcharHeight +
			SFaboveAndBelowText;

	listWidth = SFlineToTextH + SFentryWidth + SFlineToTextH + 1 +
			scrollThickness;
	listHeight = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
			SFlineToTextV + SFlistSize * SFentryHeight +
			SFlineToTextV + 1 + scrollThickness;

	SFpathScrollWidth = 3 * listWidth + 2 * listSpacing + 4;

	hScrollX = -1;
	hScrollY = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
			SFlineToTextV + SFlistSize * SFentryHeight +
			SFlineToTextV;
	SFhScrollWidth = SFlineToTextH + SFentryWidth + SFlineToTextH;

	vScrollX = SFlineToTextH + SFentryWidth + SFlineToTextH;
	vScrollY = SFlineToTextV + SFentryHeight + SFlineToTextV;
	SFvScrollHeight = SFlineToTextV + SFlistSize * SFentryHeight +
			SFlineToTextV;

	SFupperX = SFlineToTextH + SFentryWidth + SFlineToTextH - 1;
	SFlowerY = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
			SFlineToTextV;
	SFupperY = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
			SFlineToTextV + SFlistSize * SFentryHeight - 1;

	SFtextX = SFlineToTextH + SFbesideText;
	SFtextYoffset = SFlowerY + SFaboveAndBelowText + SFcharAscent;

	SFsegs[0].x1 = 0;
	SFsegs[0].y1 = vScrollY;
	SFsegs[0].x2 = vScrollX - 1;
	SFsegs[0].y2 = vScrollY;
	SFsegs[1].x1 = vScrollX;
	SFsegs[1].y1 = 0;
	SFsegs[1].x2 = vScrollX;
	SFsegs[1].y2 = vScrollY - 1;

	SFcompletionSegs[0].x1 = SFcompletionSegs[0].x2 = SFlineToTextH;
	SFcompletionSegs[1].x1 = SFcompletionSegs[1].x2 =
		SFlineToTextH + SFentryWidth - 1;

	i = 0;
	XtSetArg(arglist[i], XtNwidth, 3 * listWidth + 2 * listSpacing + 4);
									i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)
	XtSetArg(arglist[i], XtNxRefWidget, selFilePrompt);		i++;
	XtSetArg(arglist[i], XtNyRefWidget, selFilePrompt);		i++;
	XtSetArg(arglist[i], XtNyAddHeight, True);			i++;
	XtSetArg(arglist[i], XtNyOffset, 10);				i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */
	XtSetArg(arglist[i], XtNfromVert, selFilePrompt);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
	XtSetArg(arglist[i], XtNresizable, True);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
	XtSetArg(arglist[i], XtNmodifyVerification, SFmodVerify);	i++;
	XtSetArg(arglist[i], XtNmotionVerification, SFmodVerify);	i++;
	selFileField = XtCreateManagedWidget("selFileField",
		XwtexteditWidgetClass, selFileForm, arglist, i);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */
	XtSetArg(arglist[i], XtNstring, SFtextBuffer);			i++;
	XtSetArg(arglist[i], XtNlength, MAXPATHLEN);			i++;

#ifdef SEL_FILE_PRE_R4_XAW_XMU
	XtSetArg(arglist[i], XtNeditType, XttextEdit);			i++;
	XtSetArg(arglist[i], XtNtextOptions,	0		|
						editable	|
						resizeHeight	|
						wordBreak	|
						0);			i++;
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
	XtSetArg(arglist[i], XtNeditType, XawtextEdit);			i++;
	XtSetArg(arglist[i], XtNwrap, XawtextWrapWord);			i++;
	XtSetArg(arglist[i], XtNresize, XawtextResizeHeight);		i++;
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
	selFileField = XtCreateManagedWidget("selFileField",
		asciiStringWidgetClass, selFileForm, arglist, i);
#else
	selFileField = XtCreateManagedWidget("selFileField",
		asciiTextWidgetClass, selFileForm, arglist, i);
#endif

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

	XtOverrideTranslations(selFileField,
		XtParseTranslationTable(oneLineTextEditTranslations));

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)

	data = XtNew(SFfieldData);

	XtGetApplicationResources(
		selFileField,
		(caddr_t) data,
		SFfieldResources,
		XtNumber(SFfieldResources),
		(Arg *) NULL,
		0
	);

	if (data->height == 1) {

#ifdef SEL_FILE_JAPANESE
		XFontStruct	*font, *kanjiFont;
		XGCValues	values;
		XWSGC		gcSet;
#else /* def SEL_FILE_JAPANESE */
		XFontStruct	*font;
#endif /* def SEL_FILE_JAPANESE */

		int		ascent, descent;
		Dimension	top, bottom;

		i = 0;
		XtSetArg(arglist[i], XtNtopMargin, &top);		i++;
		XtSetArg(arglist[i], XtNbottomMargin, &bottom);		i++;

#ifdef SEL_FILE_JAPANESE
		XtSetArg(arglist[i], XtNfontG0, &font);			i++;
		XtSetArg(arglist[i], XtNfontG1, &kanjiFont);		i++;
#else /* def SEL_FILE_JAPANESE */
		XtSetArg(arglist[i], XtNfont, &font);			i++;
#endif /* def SEL_FILE_JAPANESE */

		XtGetValues(selFileField, arglist, i);

#ifdef SEL_FILE_JAPANESE
		gcSet = XtWSGetGCSet(selFileField, 0, &values, font, kanjiFont,
			NULL, NULL);
		XWSFontHeight(gcSet, NULL, 0, &ascent, &descent);
#else /* def SEL_FILE_JAPANESE */
		ascent = font->ascent;
		descent = font->descent;
#endif /* def SEL_FILE_JAPANESE */

		i = 0;
		XtSetArg(arglist[i], XtNheight,
			top + bottom + ascent + descent);		i++;
		XtSetValues(selFileField, arglist, i);
	}

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

	i = 0;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	XtSetArg(arglist[i], XtNslideOrientation, XwHORIZONTAL);	i++;
	XtSetArg(arglist[i], XtNsliderMin, 0);				i++;
	XtSetArg(arglist[i], XtNsliderExtent, 3);			i++;
	XtSetArg(arglist[i], XtNrepeatRate, 1);				i++;
	XtSetArg(arglist[i], XtNgranularity, 1);			i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	XtSetArg(arglist[i], XtNorientation, XtorientHorizontal);	i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	XtSetArg(arglist[i], XtNwidth, SFpathScrollWidth);		i++;
	XtSetArg(arglist[i], XtNheight, scrollThickness);		i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)
	XtSetArg(arglist[i], XtNxRefWidget, selFileField);		i++;
	XtSetArg(arglist[i], XtNyRefWidget, selFileField);		i++;
	XtSetArg(arglist[i], XtNyAddHeight, True);			i++;
	XtSetArg(arglist[i], XtNyOffset, 30);				i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */
	XtSetArg(arglist[i], XtNfromVert, selFileField);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 30);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	selFileHScroll = XtCreateManagedWidget("selFileHScroll",
		XwscrollbarWidgetClass, selFileForm, arglist, i);

	XtAddCallback(selFileHScroll, XtNsliderMoved,
		SFpathSliderMovedCallback, (caddr_t) NULL);
	XtAddCallback(selFileHScroll, XtNareaSelected,
		SFpathAreaSelectedCallback, (caddr_t) NULL);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
	selFileHScroll = XtCreateManagedWidget("selFileHScroll",
		scrollbarWidgetClass, selFileForm, arglist, i);

	XtAddCallback(selFileHScroll, XtNjumpProc,
		SFpathSliderMovedCallback, (caddr_t) NULL);
	XtAddCallback(selFileHScroll, XtNscrollProc,
		SFpathAreaSelectedCallback, (caddr_t) NULL);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	i = 0;
	XtSetArg(arglist[i], XtNwidth, listWidth);			i++;
	XtSetArg(arglist[i], XtNheight, listHeight);			i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)
	XtSetArg(arglist[i], XtNxRefWidget, selFileHScroll);		i++;
	XtSetArg(arglist[i], XtNyRefWidget, selFileHScroll);		i++;
	XtSetArg(arglist[i], XtNyAddHeight, True);			i++;
	XtSetArg(arglist[i], XtNyOffset, 10);				i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */
	XtSetArg(arglist[i], XtNfromVert, selFileHScroll);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

	selFileLists[0] = XtCreateManagedWidget("selFileList1",
		compositeWidgetClass, selFileForm, arglist, i);

	i = 0;
	XtSetArg(arglist[i], XtNwidth, listWidth);			i++;
	XtSetArg(arglist[i], XtNheight, listHeight);			i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)
	XtSetArg(arglist[i], XtNxRefWidget, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNyRefWidget, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNxAddWidth, True);			i++;
	XtSetArg(arglist[i], XtNxOffset, listSpacing);			i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */
	XtSetArg(arglist[i], XtNfromHoriz, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNfromVert, selFileHScroll);		i++;
	XtSetArg(arglist[i], XtNhorizDistance, listSpacing);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

	selFileLists[1] = XtCreateManagedWidget("selFileList2",
		compositeWidgetClass, selFileForm, arglist, i);

	i = 0;
	XtSetArg(arglist[i], XtNwidth, listWidth);			i++;
	XtSetArg(arglist[i], XtNheight, listHeight);			i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)
	XtSetArg(arglist[i], XtNxRefWidget, selFileLists[1]);		i++;
	XtSetArg(arglist[i], XtNyRefWidget, selFileLists[1]);		i++;
	XtSetArg(arglist[i], XtNxAddWidth, True);			i++;
	XtSetArg(arglist[i], XtNxOffset, listSpacing);			i++;
	XtSetArg(arglist[i], XtNxAttachRight, True);			i++;
	XtSetArg(arglist[i], XtNxAttachOffset, 30);			i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */
	XtSetArg(arglist[i], XtNfromHoriz, selFileLists[1]);		i++;
	XtSetArg(arglist[i], XtNfromVert, selFileHScroll);		i++;
	XtSetArg(arglist[i], XtNhorizDistance, listSpacing);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

	selFileLists[2] = XtCreateManagedWidget("selFileList3",
		compositeWidgetClass, selFileForm, arglist, i);

	for (n = 0; n < 3; n++) {

		i = 0;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
		XtSetArg(arglist[i], XtNsliderMin, 0);			i++;
		XtSetArg(arglist[i], XtNrepeatRate, 1);			i++;
		XtSetArg(arglist[i], XtNgranularity, 1);		i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

		XtSetArg(arglist[i], XtNx, vScrollX);			i++;
		XtSetArg(arglist[i], XtNy, vScrollY);			i++;
		XtSetArg(arglist[i], XtNwidth, scrollThickness);	i++;
		XtSetArg(arglist[i], XtNheight, SFvScrollHeight);	i++;
		XtSetArg(arglist[i], XtNborderColor, SFfore);		i++;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
		selFileVScrolls[n] = XtCreateManagedWidget("selFileVScroll",
			XwscrollbarWidgetClass, selFileLists[n], arglist, i);

		XtAddCallback(selFileVScrolls[n], XtNsliderMoved,
			SFvSliderMovedCallback, (caddr_t) n);
		XtAddCallback(selFileVScrolls[n], XtNareaSelected,
			SFvAreaSelectedCallback, (caddr_t) n);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
		selFileVScrolls[n] = XtCreateManagedWidget("selFileVScroll",
			scrollbarWidgetClass, selFileLists[n], arglist, i);

		XtAddCallback(selFileVScrolls[n], XtNjumpProc,
			SFvFloatSliderMovedCallback, (caddr_t) n);
		XtAddCallback(selFileVScrolls[n], XtNscrollProc,
			SFvAreaSelectedCallback, (caddr_t) n);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

		i = 0;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
		XtSetArg(arglist[i], XtNslideOrientation, XwHORIZONTAL);i++;
		XtSetArg(arglist[i], XtNsliderMin, 0);			i++;
		XtSetArg(arglist[i], XtNrepeatRate, 1);			i++;
		XtSetArg(arglist[i], XtNgranularity, 1);		i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
		XtSetArg(arglist[i], XtNorientation, XtorientHorizontal);
									i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

		XtSetArg(arglist[i], XtNx, hScrollX);			i++;
		XtSetArg(arglist[i], XtNy, hScrollY);			i++;
		XtSetArg(arglist[i], XtNwidth, SFhScrollWidth);		i++;
		XtSetArg(arglist[i], XtNheight, scrollThickness);	i++;
		XtSetArg(arglist[i], XtNborderColor, SFfore);		i++;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
		selFileHScrolls[n] = XtCreateManagedWidget("selFileHScroll",
			XwscrollbarWidgetClass, selFileLists[n], arglist, i);

		XtAddCallback(selFileHScrolls[n], XtNsliderMoved,
			SFhSliderMovedCallback, (caddr_t) n);
		XtAddCallback(selFileHScrolls[n], XtNareaSelected,
			SFhAreaSelectedCallback, (caddr_t) n);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */
		selFileHScrolls[n] = XtCreateManagedWidget("selFileHScroll",
			scrollbarWidgetClass, selFileLists[n], arglist, i);

		XtAddCallback(selFileHScrolls[n], XtNjumpProc,
			SFhSliderMovedCallback, (caddr_t) n);
		XtAddCallback(selFileHScrolls[n], XtNscrollProc,
			SFhAreaSelectedCallback, (caddr_t) n);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	}

	i = 0;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)

#ifdef SEL_FILE_JAPANESE

#ifdef SEL_FILE_SJIS
	(void) convSJIStoCS(ok, cstr);
#else /* def SEL_FILE_SJIS */
	(void) convEUCtoCS(ok, cstr);
#endif /* def SEL_FILE_SJIS */

	XtSetArg(arglist[i], XtNlabel, cstr);				i++;
#else /* def SEL_FILE_JAPANESE */
	XtSetArg(arglist[i], XtNlabel, ok);				i++;
#endif /* def SEL_FILE_JAPANESE */

	XtSetArg(arglist[i], XtNhSpace, 4);				i++;
	XtSetArg(arglist[i], XtNvSpace, 4);				i++;
	XtSetArg(arglist[i], XtNselect, SFokSelect);			i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */
	XtSetArg(arglist[i], XtNlabel, ok);				i++;
	XtSetArg(arglist[i], XtNcallback, SFokSelect);			i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)
	XtSetArg(arglist[i], XtNxRefWidget, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNyRefWidget, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNyAddHeight, True);			i++;
	XtSetArg(arglist[i], XtNyOffset, 30);				i++;
	XtSetArg(arglist[i], XtNyAttachBottom, True);			i++;
	XtSetArg(arglist[i], XtNyAttachOffset, 30);			i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */
	XtSetArg(arglist[i], XtNfromVert, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 30);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)
	selFileOK = XtCreateManagedWidget("selFileOK", XwpushButtonWidgetClass,
		selFileForm, arglist, i);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */
	selFileOK = XtCreateManagedWidget("selFileOK", commandWidgetClass,
		selFileForm, arglist, i);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

	i = 0;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)

#ifdef SEL_FILE_JAPANESE

#ifdef SEL_FILE_SJIS
	(void) convSJIStoCS(cancel, cstr);
#else /* def SEL_FILE_SJIS */
	(void) convEUCtoCS(cancel, cstr);
#endif /* def SEL_FILE_SJIS */

	XtSetArg(arglist[i], XtNlabel, cstr);				i++;
#else /* def SEL_FILE_JAPANESE */
	XtSetArg(arglist[i], XtNlabel, cancel);				i++;
#endif /* def SEL_FILE_JAPANESE */

	XtSetArg(arglist[i], XtNhSpace, 4);				i++;
	XtSetArg(arglist[i], XtNvSpace, 4);				i++;
	XtSetArg(arglist[i], XtNselect, SFcancelSelect);		i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */
	XtSetArg(arglist[i], XtNlabel, cancel);				i++;
	XtSetArg(arglist[i], XtNcallback, SFcancelSelect);		i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM)
	XtSetArg(arglist[i], XtNxRefWidget, selFileOK);			i++;
	XtSetArg(arglist[i], XtNyRefWidget, selFileOK);			i++;
	XtSetArg(arglist[i], XtNxAddWidth, True);			i++;
	XtSetArg(arglist[i], XtNxOffset, 30);				i++;
	XtSetArg(arglist[i], XtNyAttachBottom, True);			i++;
	XtSetArg(arglist[i], XtNyAttachOffset, 30);			i++;
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */
	XtSetArg(arglist[i], XtNfromHoriz, selFileOK);			i++;
	XtSetArg(arglist[i], XtNfromVert, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNhorizDistance, 30);			i++;
	XtSetArg(arglist[i], XtNvertDistance, 30);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWFORM) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)
	selFileCancel = XtCreateManagedWidget("selFileCancel",
		XwpushButtonWidgetClass, selFileForm, arglist, i);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */
	selFileCancel = XtCreateManagedWidget("selFileCancel",
		commandWidgetClass, selFileForm, arglist, i);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

	XtRealizeWidget(selFile);

	SFcreateGC();

	xtermCursor = XCreateFontCursor(SFdisplay, XC_xterm);

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	sbDownArrowCursor = XCreateFontCursor(SFdisplay, XC_sb_down_arrow);
	sbLeftArrowCursor = XCreateFontCursor(SFdisplay, XC_sb_left_arrow);
	sbUpArrowCursor = XCreateFontCursor(SFdisplay, XC_sb_up_arrow);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	sbRightArrowCursor = XCreateFontCursor(SFdisplay, XC_sb_right_arrow);
	dotCursor = XCreateFontCursor(SFdisplay, XC_dot);

	XDefineCursor(SFdisplay, XtWindow(selFileForm), xtermCursor);
	XDefineCursor(SFdisplay, XtWindow(selFileField), xtermCursor);

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	XDefineCursor(SFdisplay, XtWindow(selFileHScroll), sbDownArrowCursor);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	for (n = 0; n < 3; n++) {
		XDefineCursor(SFdisplay, XtWindow(selFileLists[n]),
			sbRightArrowCursor);

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
		XDefineCursor(SFdisplay, XtWindow(selFileVScrolls[n]),
			sbLeftArrowCursor);
		XDefineCursor(SFdisplay, XtWindow(selFileHScrolls[n]),
			sbUpArrowCursor);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	}
	XDefineCursor(SFdisplay, XtWindow(selFileOK), dotCursor);
	XDefineCursor(SFdisplay, XtWindow(selFileCancel), dotCursor);

	for (n = 0; n < 3; n++) {
		XtAddEventHandler(selFileLists[n], ExposureMask, True,
			SFexposeList, (caddr_t) n);
		XtAddEventHandler(selFileLists[n], EnterWindowMask, False,
			SFenterList, (caddr_t) n);
		XtAddEventHandler(selFileLists[n], LeaveWindowMask, False,
			SFleaveList, (caddr_t) n);
		XtAddEventHandler(selFileLists[n], PointerMotionMask, False,
			SFmotionList, (caddr_t) n);
		XtAddEventHandler(selFileLists[n], ButtonPressMask, False,
			SFbuttonPressList, (caddr_t) n);
		XtAddEventHandler(selFileLists[n], ButtonReleaseMask, False,
			SFbuttonReleaseList, (caddr_t) n);
	}

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
	XtAddCallback(selFileField, XtNexecute, returnCallback, (caddr_t) NULL);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */
	XtAddEventHandler(selFileField, KeyPressMask, False,
		SFmodVerifyCallback, (caddr_t) NULL);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#ifndef SEL_FILE_R2_XT

	SFapp = XtWidgetToApplicationContext(selFile);

#endif /* ndef SEL_FILE_R2_XT */

}

SFtextChanged()
{

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)
	static char	*SFtextBuffer = NULL;

#ifdef SEL_FILE_JAPANESE
	static int	alloc = 0;
	wchar_t		*wstr;
	int		len;
#endif /* def SEL_FILE_JAPANESE */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)

#ifdef SEL_FILE_JAPANESE
	wstr = XwTextCopyBuffer(selFileField);
	len = convWStoEUC(wstr, (unsigned char *) NULL);
	while (len + 1 > alloc) {
		alloc = 2 * (alloc + 1);
		SFtextBuffer = XtRealloc(SFtextBuffer, (unsigned) alloc);
	}
	(void) convWStoEUC(wstr, (unsigned char *) SFtextBuffer);
	XtFree((char *) wstr);
#else /* def SEL_FILE_JAPANESE */
	XtFree(SFtextBuffer);
	SFtextBuffer = (char *) XwTextCopyBuffer(selFileField);
#endif /* def SEL_FILE_JAPANESE */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

	if ((SFtextBuffer[0] == '/') || (SFtextBuffer[0] == '~')) {
		(void) strcpy(SFcurrentPath, SFtextBuffer);

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)

		SFtextPos = XwTextGetInsertPos(selFileField);

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
		SFtextPos = XtTextGetInsertionPoint(selFileField);
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
		SFtextPos = XawTextGetInsertionPoint(selFileField);
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

	} else {
		(void) strcat(strcpy(SFcurrentPath, SFstartDir), SFtextBuffer);

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)

		SFtextPos = XwTextGetInsertPos(selFileField) +
			strlen(SFstartDir);

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
		SFtextPos = XtTextGetInsertionPoint(selFileField) +
			strlen(SFstartDir);
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
		SFtextPos = XawTextGetInsertionPoint(selFileField) +
			strlen(SFstartDir);
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

	}

#ifndef SEL_FILE_R2_XT

	if (!SFworkProcAdded) {
		(void) XtAppAddWorkProc(SFapp, SFworkProc, NULL);
		SFworkProcAdded = 1;
	}

#endif /* ndef SEL_FILE_R2_XT */

	SFupdatePath();
}

static char *
SFgetText()
{
	return strcpy(XtMalloc((unsigned) (strlen(SFcurrentPath) + 1)),
		SFcurrentPath);
}

static
SFprepareToReturn()
{
	SFstatus = SEL_FILE_NULL;
	XtRemoveGrab(selFile);
	XtUnmapWidget(selFile);
	XtRemoveTimeOut(SFdirModTimerId);
	if (SFchdir(SFstartDir)) {

#ifdef SEL_FILE_R2_XT

		XtError(

#else /* def SEL_FILE_R2_XT */

		XtAppError(
			SFapp,

#endif /* def SEL_FILE_R2_XT */

			"XsraSelFile: can't return to current directory"
		);
	}
}

int
XsraSelFile(display, prompt, ok, cancel, init_path, show_entry, path_return)
	Display		*display;
	unsigned char	*prompt;
	unsigned char	*ok;
	unsigned char	*cancel;
	char		*init_path;
	int		(*show_entry)();
	char		**path_return;
{
	static int	firstTime = 1;

#ifdef SEL_FILE_JAPANESE
	CStr		csl[2];
	u_char		cstr[256];
#endif /* def SEL_FILE_JAPANESE */

	int		i;
	Arg		arglist[20];
	XEvent		event;

	if ((!display) && firstTime) {
		Widget	w;
		int	argc;
		char	*argv[1];

		argc = 1;
		argv[0] = "xfoo";
		w = XtInitialize(argv[0], "XFoo", (XrmOptionDescRec *) NULL, 0,
			(Cardinal *) &argc, argv);
		display = XtDisplay(w);
	}

	if (!prompt) {
		prompt = (unsigned char *) "Pathname:";
	}

	if (!ok) {
		ok = (unsigned char *) "OK";
	}

	if (!cancel) {
		cancel = (unsigned char *) "Cancel";
	}

	if (firstTime) {
		firstTime = 0;
		SFdisplay = display;
		SFcreateWidgets(prompt, ok, cancel);
	} else {
		if (display && (SFdisplay != display)) {

#ifdef SEL_FILE_R2_XT

			XtError("XsraSelFile: display different");

#else /* def SEL_FILE_R2_XT */

			XtAppError(SFapp, "XsraSelFile: display different");

#endif /* def SEL_FILE_R2_XT */

		}

		i = 0;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT)

#ifdef SEL_FILE_JAPANESE

#ifdef SEL_FILE_SJIS
		(void) convSJIStoCS(prompt, cstr);
#else /* def SEL_FILE_SJIS */
		(void) convEUCtoCS(prompt, cstr);
#endif /* def SEL_FILE_SJIS */

		csl[0] = cstr;
		csl[1] = NULL;
		XtSetArg(arglist[i], XtNstrings, csl);			i++;
#else /* def SEL_FILE_JAPANESE */
		XtSetArg(arglist[i], XtNstring, prompt);		i++;
#endif /* def SEL_FILE_JAPANESE */

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT) */
		XtSetArg(arglist[i], XtNlabel, prompt);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSTATICTEXT) */

		XtSetValues(selFilePrompt, arglist, i);

		i = 0;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)

#ifdef SEL_FILE_JAPANESE

#ifdef SEL_FILE_SJIS
		(void) convSJIStoCS(ok, cstr);
#else /* def SEL_FILE_SJIS */
		(void) convEUCtoCS(ok, cstr);
#endif /* def SEL_FILE_SJIS */

		XtSetArg(arglist[i], XtNlabel, cstr);			i++;
#else /* def SEL_FILE_JAPANESE */
		XtSetArg(arglist[i], XtNlabel, ok);			i++;
#endif /* def SEL_FILE_JAPANESE */

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */
		XtSetArg(arglist[i], XtNlabel, ok);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

		XtSetValues(selFileOK, arglist, i);

		i = 0;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON)

#ifdef SEL_FILE_JAPANESE

#ifdef SEL_FILE_SJIS
		(void) convSJIStoCS(cancel, cstr);
#else /* def SEL_FILE_SJIS */
		(void) convEUCtoCS(cancel, cstr);
#endif /* def SEL_FILE_SJIS */

		XtSetArg(arglist[i], XtNlabel, cstr);			i++;
#else /* def SEL_FILE_JAPANESE */
		XtSetArg(arglist[i], XtNlabel, cancel);			i++;
#endif /* def SEL_FILE_JAPANESE */

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */
		XtSetArg(arglist[i], XtNlabel, cancel);			i++;
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWPUSHBUTTON) */

		XtSetValues(selFileCancel, arglist, i);

		XtMapWidget(selFile);
	}

#if defined(SYSV) || defined(USG)
	if (!getcwd(SFstartDir, MAXPATHLEN)) {
#else /* defined(SYSV) || defined(USG) */
	if (!getwd(SFstartDir)) {
#endif /* defined(SYSV) || defined(USG) */

#ifdef SEL_FILE_R2_XT

		XtError("XsraSelFile: can't get current directory");

#else /* def SEL_FILE_R2_XT */

		XtAppError(SFapp, "XsraSelFile: can't get current directory");

#endif /* def SEL_FILE_R2_XT */

	}
	(void) strcat(SFstartDir, "/");
	(void) strcpy(SFcurrentDir, SFstartDir);

	if (init_path) {
		if (init_path[0] == '/') {
			(void) strcpy(SFcurrentPath, init_path);
			if (strncmp(
				SFcurrentPath,
				SFstartDir,
				strlen(SFstartDir)
			)) {
				SFsetText(SFcurrentPath);
			} else {
				SFsetText(&(SFcurrentPath[strlen(SFstartDir)]));
			}
		} else {
			(void) strcat(strcpy(SFcurrentPath, SFstartDir),
				init_path);
			SFsetText(&(SFcurrentPath[strlen(SFstartDir)]));
		}
	} else {
		(void) strcpy(SFcurrentPath, SFstartDir);
	}

	SFfunc = show_entry;

	SFtextChanged();

	XtAddGrab(selFile, True, True);

#ifdef SEL_FILE_R2_XT

	SFdirModTimerId = XtAddTimeOut((unsigned long) 1000,

#else /* def SEL_FILE_R2_XT */

	SFdirModTimerId = XtAppAddTimeOut(SFapp, (unsigned long) 1000,

#endif /* def SEL_FILE_R2_XT */

		SFdirModTimer, (caddr_t) NULL);

	while (1) {

#ifdef SEL_FILE_R2_XT

		XtNextEvent(&event);

#else /* def SEL_FILE_R2_XT */

		XtAppNextEvent(SFapp, &event);

#endif /* def SEL_FILE_R2_XT */

		XtDispatchEvent(&event);
		switch (SFstatus) {
		case SEL_FILE_TEXT:
			SFstatus = SEL_FILE_NULL;
			SFtextChanged();
			break;
		case SEL_FILE_OK:
			SFprepareToReturn();
			if (path_return) {
				*path_return = SFgetText();
			}
			return SEL_FILE_OK;
		case SEL_FILE_CANCEL:
			SFprepareToReturn();
			return SEL_FILE_CANCEL;
		case SEL_FILE_NULL:
			break;
		}
	}
}
