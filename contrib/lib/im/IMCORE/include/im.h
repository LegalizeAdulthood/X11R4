/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


/******************** START OF MODULE SPECIFICATIONS ********************
 *
 * MODULE NAME:		im.h
 *
 * DESCRIPTIVE NAME:	AIX Input Method header file
 *
 * MODULE TYPE:		C
 *
 * COMPILER:		AIX C
 *
 * CHANGE ACTIVITY:	0%G% - Modified
 *
 * STATUS:		Input Method Version %Q%
 *
 * CATEGORY:		Header
 *
 ******************** END OF SPECIFICATIONS *****************************/

#ifndef	_h_IM
#define _h_IM
 
/*-----------------------------------------------------------------------*
*	Include
*-----------------------------------------------------------------------*/
#include <sys/types.h>
#include <X11/X.h>			/* for Keysym definition */
#include "imerrno.h"

#ifndef	TRUE
#define TRUE	1
#endif	TRUE
#ifndef	FALSE
#define FALSE	0
#endif	FALSE
#ifndef	NULL
#define NULL	0
#endif	NULL

/*-----------------------------------------------------------------------*
*	IMDIRECTORY
*-----------------------------------------------------------------------*/
#define	IMDIRECTORY	"/usr/lib/nls/IM"
#define	IMMODULENAME	"inputmethod"
 
/*-----------------------------------------------------------------------*
*	IMLanguage
*-----------------------------------------------------------------------*/
typedef	caddr_t	IMLanguage;
 
/*-----------------------------------------------------------------------*
*	IMSTR and IMSTRATT
*-----------------------------------------------------------------------*/
typedef struct	{
	uint	len;			/* length in byte */
	caddr_t	str;			/* string, not null terminated */
}	IMSTR;

typedef	struct	{
	uint	len;			/* length in byte */
	caddr_t	str;			/* string, not null terminated */
	caddr_t	att;			/* attributes of string */
}	IMSTRATT;
 
/* attribute value is one of... */
#define	IMAttNone	0
#define	IMAttHighlight	(1 << 0)
#define	IMAttAttention	(1 << 1)
 
/*-----------------------------------------------------------------------*
*	IMFunc
*-----------------------------------------------------------------------*/
typedef int	(*IMFunc)();
 
/*-----------------------------------------------------------------------*
*	IMCallback
*-----------------------------------------------------------------------*/
typedef struct  {
	uint	textmaxwidth;		/* max width of Text area */
	IMFunc	textdraw;		/* IMTextDraw() */
	IMFunc	texthide;		/* IMTextHide() */
	IMFunc	textstart;		/* IMTextStart() */
	IMFunc	textcursor;		/* IMTextCursor() */
	IMFunc	auxcreate;		/* IMAuxCreate() */
	IMFunc	auxdraw;		/* IMAuxDraw() */
	IMFunc	auxhide;		/* IMAuxHide() */
	IMFunc	auxdestroy;		/* IMAuxDestroy() */
	IMFunc	indicatordraw;		/* IMIndicatorDraw() */
	IMFunc	indicatorhide;		/* IMIndicatorHide() */
	IMFunc	beep;			/* IMBeep() */
}	IMCallback;

/*-----------------------------------------------------------------------*
*	IMFEP
*-----------------------------------------------------------------------*/
typedef struct	_IMFepCommon	{
	uint			version;		/* version # */
	IMLanguage		language;		/* language str */
	int			imerrno;		/* error # */
	struct	_IMFepCommon	*(*iminitialize)();	/* IMInitialize() */
	void			(*imclose)();		/* IMClose() */
	struct	_IMObjectCommon	*(*imcreate)();		/* IMCreate() */
	void			(*imdestroy)();		/* IMDestroy() */
	int			(*improcess)();		/* IMProcess() */
	int			(*improcessaux)();	/* IMProcessAux() */
	int			(*imioctl)();		/* IMIoctl() */
}	IMFepRec, IMFepCommon, *IMFep;
 
/* current version is 1.1 */
#define	IMVersionNumber	((1 << 16) | 1)

/*-----------------------------------------------------------------------*
*	IMOBJ
*-----------------------------------------------------------------------*/
typedef struct  _IMObjectCommon  {
	IMFep		imfep;		/* pointer to IMFepRec */
	IMCallback	*cb;		/* pointer to CALLBACKs */
	uint		mode;		/* Processing Mode */
	caddr_t		udata;		/* caller's data */
}	IMObjectRec, IMObjectCommon, *IMObject;
 
/* mode member is one of */
#define IMSuppressedMode	0
#define IMNormalMode		(IMSuppressedMode + 1)

/*-----------------------------------------------------------------------*
*	IMTextInfo
*-----------------------------------------------------------------------*/
typedef	struct	{
	uint		maxwidth;	/* maximum width on display */
	uint		cur;		/* cursor position on display */
	uint		chgtop;		/* top of change in byte offset */
	uint		chglen;		/* length of change in byte */
	IMSTRATT	text;		/* text string & attribute */
}	IMTextInfo;

/*-----------------------------------------------------------------------*
*	IMAuxInfo
*-----------------------------------------------------------------------*/
typedef	IMSTR	IMTitle;

typedef	struct	{
	uint		maxwidth;	/* maximum width of text */
	uint		nline;		/* nubmer of msg text line */
	uint		cursor;		/* TRUE if it has cursor */
	uint		cur_row;	/* cursor position in row */
	uint		cur_col;	/* cursor position in column */
	IMSTRATT	*text;		/* ptr to array of IMSTRATT struct */
}	IMMessage;

typedef	uint	IMButton;

typedef	struct	{
	uint	selectable;		/* TRUE if selectable */
	IMSTR	text;			/* text of item */
}	IMItem;

typedef	struct	{
	uint	maxwidth;		/* maximum width of item */
	uint	item_row;		/* # of items in row */
	uint	item_col;		/* # of items in column */
	IMItem	*item;			/* array of items */
}	IMPanel;

typedef	struct	{
	uint		panel_row;	/* # of panels in row */
	uint		panel_col;	/* # of panels in column */
	IMPanel		*panel;		/* ptr to panel array */
}	IMSelection;

typedef struct	{
	IMTitle		title;		/* title string */
	IMMessage	message;	/* message text */
	IMButton	button;		/* type of button */
	IMSelection	selection;	/* selections */
	uint		hint;		/* hint of location */
	uint		status;		/* shown or hidden or ...ing */
}	IMAuxInfo;
 
/* buttons member is one of */
#define IM_NONE			0
#define IM_OK			(1 << 0)
#define	IM_YES			(1 << 1)
#define IM_NO			(1 << 2)
#define IM_ENTER		(1 << 3)
#define IM_ABORT		(1 << 4)
#define IM_RETRY		(1 << 5)
#define IM_IGNORE		(1 << 6)
#define	IM_CANCEL		(1 << 7)
#define IM_HELP			(1 << 8)
#define	IM_SELECTED		(1 << 9)
#define	IM_ERROR		(1 << 10)
#define IM_OKCANCEL		(IM_OK | IM_CANCEL)
#define IM_ENTERCANCEL		(IM_ENTER | IM_CANCEL)
#define IM_RETRYCANCEL		(IM_RETRY | IM_CANCEL)
#define IM_ABORTRETRYIGNORE	(IM_ABORT | IM_RETRY | IM_IGNORE)
#define IM_YESNO		(IM_YES | IM_NO)
#define IM_YESNOCANCEL		(IM_YES | IM_NO | IM_CANCEL)

/* hint member is one of */
#define	IM_AtTheEvent	0
#define	IM_UpperLeft	(IM_AtTheEvent + 1)
#define	IM_UpperRight	(IM_UpperLeft + 1)
#define	IM_LowerLeft	(IM_UpperRight + 1)
#define	IM_LowerRight	(IM_LowerLeft + 1)

/* status member is one of */
#define IMAuxShowing	0
#define IMAuxUpdated	(IMAuxShowing + 1)
#define IMAuxShown	(IMAuxUpdated + 1)
#define IMAuxHiding	(IMAuxShown + 1)
#define IMAuxHidden	(IMAuxHiding + 1)

/*-----------------------------------------------------------------------*
*	IMIndicatorInfo
*-----------------------------------------------------------------------*/
typedef struct  {
	uint	size	:  4;		/* size of character */
	uint	insert	:  4;		/* insert/replace */
	uint	unique	: 24;
}	IMIndicatorInfo;

/* size member is one of */
#define IMHalfWidth		0
#define IMFullWidth		(IMHalfWidth + 1)

/* insert member is one of */
#define IMInsertMode		TRUE
#define IMReplaceMode		FALSE
 
/*-----------------------------------------------------------------------*
*	IM Query State
*-----------------------------------------------------------------------*/
typedef struct	{
	uint	mode;		/* IMNormalMode/IMSuppressedMode */
	uint	text;		/* TRUE/FALSE */
	uint	aux;		/* TRUE/FALSE */
	uint	indicator;	/* TRUE/FALSE */
	int	beep;		/* persent */
}	IMQueryState;
 
/*-----------------------------------------------------------------------*
*	IMQueryText
*-----------------------------------------------------------------------*/
typedef struct	{
	IMTextInfo	*textinfo;	/* RETURN */
}	IMQueryText;
 
/*-----------------------------------------------------------------------*
*	IMQueryAuxiliary
*-----------------------------------------------------------------------*/
typedef struct	{
	caddr_t		auxid;		/* id of the Auxiliary area */
	IMAuxInfo	*auxinfo;	/* RETURN */
}	IMQueryAuxiliary;
 
/*-----------------------------------------------------------------------*
*	IMQueryIndicator
*-----------------------------------------------------------------------*/
typedef struct	{
	IMIndicatorInfo		*indicatorinfo;		/* RETURN */
}	IMQueryIndicator;
 
/*-----------------------------------------------------------------------*
*	IM Query Indicator String
*-----------------------------------------------------------------------*/
typedef struct	{
	uint		format;			/* format of indicator */
	IMSTR		indicator;		/* RETURN */
}	IMQueryIndicatorString;

/* format member is one of */
#define IMShortForm	0
#define IMLongForm	(IMShortForm + 1)

/*-----------------------------------------------------------------------*
*	Return Value of IMProcess() and IMIoctl()
*-----------------------------------------------------------------------*/
#define IMNoError	0
#define	IMError		-1

/*-----------------------------------------------------------------------*
*	Return Value of IMProcess()
*-----------------------------------------------------------------------*/
#define	IMTextAndAuxiliaryOff	0
#define	IMTextOn		(IMTextAndAuxiliaryOff + 1)
#define	IMAuxiliaryOn		(IMTextOn + 1)
#define	IMTextAndAuxiliaryOn	(IMAuxiliaryOn + 1)

/*-----------------------------------------------------------------------*
*	Command of IMIoctl()
*-----------------------------------------------------------------------*/
#define	IM_Refresh		0
#define IM_GetString		(IM_Refresh + 1)
#define IM_Clear		(IM_GetString + 1)
#define IM_Reset		(IM_Clear + 1)
#define IM_ChangeLength		(IM_Reset + 1)
#define IM_QueryState		(IM_ChangeLength + 1)
#define IM_QueryText		(IM_QueryState + 1)
#define IM_QueryAuxiliary	(IM_QueryText + 1)
#define IM_QueryIndicator	(IM_QueryAuxiliary + 1)
#define IM_QueryIndicatorString	(IM_QueryIndicator + 1)
#define IM_ChangeMode		(IM_QueryIndicatorString + 1)

/* To add Language Dependent Commands */

/* SBCS Input Method */
#define SIMIOC	('S' << 16)

/* Japanese Input Method */
#define JIMIOC	('J' << 16)

/* Chinese (Hanji) Input Method */
#define HIMIOC	('H' << 16)

/* Korean (Hangeul) Input Method */
#define KIMIOC	('K' << 16)

/*-----------------------------------------------------------------------*
*	Constant Definitions for IMTextCursor() Callback
*-----------------------------------------------------------------------*/
#define	IM_CursorUp	0
#define	IM_CursorDown	1

/*-----------------------------------------------------------------------*
*	IM Keymap Handling Functions
*-----------------------------------------------------------------------*/

typedef	caddr_t	IMMap;		/* Internal State Structure */
IMMap	IMInitializeKeymap();	/* Do IM search for Keymap file */
void	IMFreeKeymap();		/* free resources allocated for IM Keymap */
char	*IMSimpleMapping();	/* map Kbd Element - see AIXKeymap.h*/

#endif	_h_IM
