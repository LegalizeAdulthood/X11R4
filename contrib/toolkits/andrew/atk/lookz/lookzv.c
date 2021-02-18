/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lookz/RCS/lookzv.c,v 2.15 89/11/12 13:16:52 jhh Exp $ */
/* $ACIS:lookzv.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lookz/RCS/lookzv.c,v $ */

#ifndef lint
char *lookzv_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lookz/RCS/lookzv.c,v 2.15 89/11/12 13:16:52 jhh Exp $";
#endif /* lint */

/* lookzv.c	

	The view module for the lookz dataobject

	When initiated as an inset in a document, the FullUpdate routine attempts to 
	find a stylesheet associated with the parent text.  Otherwise a stylesheet can 
	be specified by a client calling SetStyleSheet.  Either way, lookzview displays 
	some of the components of the style via stringtbls.  Clicks on the tables 
	cause the associated attributes to be changed in the stylesht.


Deferred:
	Want to avoid direct modification of the style in the surrounding text.
		Instead, make a copy of the style, modify the copy, and
		call style_Copy to replace the original style.
	Menu options: Add Font, Rename Style, Cancel
	Baseline separation and minimum and maximum shim size.
*/
#include <stdio.h>
/* sys/types.h in AIX PS2 defines "struct label", causing a type name clash.
   Avoid this by temporarily redefining "label" to be something else in the preprocessor. */
#define label gezornenplatz
#include <andrewos.h> /* strings.h */
#undef label
#include <util.h>
#include <ctype.h>
#include <class.h>
#include <message.ih>
#include <rect.h>
#include <graphic.ih>
#include <dataobj.ih>
#include <view.ih>
#include <text.ih>
#include <lpair.ih>
#include <stylesht.ih>
#include <style.ih>

#include <bpair.ih>
#include <strtbl.ih>
#include <strtblv.ih>
#include <lprruler.ih>
#include <lprrulv.ih>
#include <label.ih>
#include <labelv.ih>

#define class_StaticEntriesOnly
#include <fontdesc.ih>	/* for face codes */
#undef class_StaticEntriesOnly

#include <lookz.ih>
#include <lookzv.eh>

#include <menulist.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <bind.ih>

static boolean  debug = FALSE;
#define DEBUG(s) {if (debug) {printf s ; fflush(stdout);}}
#define ENTER(r) DEBUG(("Enter %s(0x%lx)\n", "r", self))
#define LEAVE(r) DEBUG(("Leave %s(0x%lx)\n", "r", self))



static void UpdateDocument(), AddStyle();

static struct bind_Description MenuOptions[] = {
	{"lookzview-update-document", "\033U", 0, "Update Document", 0, 0,
		UpdateDocument, "Cause document to redisplay itself", NULL},
	{"lookzview-add-style", "\033A", 0, "Add Style", 0, 0,
		AddStyle, "Add a new style to stylesheet", NULL},
	NULL
};

static struct menulist  *MenuList;
static struct keymap  *Keymap;



/* indices into arrays st[] and stv[] */
enum TableNum {
	TNmenucard, TNstylename, TNfont, TNsize, 
	TNmode, TNsubscr, TNenable, TNdisable
};

/* set up initial strings for stringtbls.  
		Where a string in this table is supposed to be the 
		same as a string in one of the XxxxNames tables, 
		both must be spelled identically */
	static char *Imenucard[] = {"", NULL,};
	static char *Istylename[] = {"", NULL,};
	static char *Ifont[] = {"Andy", "AndySans", "AndyType", NULL,};
	static char *Isize[] = {
			"-8", "-6", "-4", "-3", "-2", "+2", "+3", "+4", "+6", "+8", 
			"7", "8", "9", "10", "11", "12", "14", "16", "18", 
			"20", "24", "30", "36", NULL,};
	static char *Imode[] = {"Center", "Left flush", "Right flush", 
			"Justified", "Left-right", NULL,};
	static char *Isubscr[] = {"up 15", "up 6", "up 4", "super (up 2)", 
			"sub (down 2)", "down 4", "down 6", NULL,};
	static char *Ienable[] = {"Bold", "Italic",  "Fixed width", "Pass thru", NULL,};
	static char *Idisable[] = {"Bold", "Italic",  "Fixed width", "Pass thru", NULL,};

static char **InitialStrings[]=
		{Imenucard, Istylename, Ifont, Isize, Imode, Isubscr, Ienable, Idisable, NULL,};


struct strTbl {char *str; long val;};

/* map justification mode names to and from justification codes*/
static 
struct strTbl ModeNames[] = {
		{"Center", (long)style_Centered},
		{"Left flush", (long)style_LeftJustified},
		{"Right flush", (long)style_RightJustified},
		{"Justified", (long)style_LeftAndRightJustified},
		{"Left-right", (long)style_LeftThenRightJustified},
		{NULL, 0L},
};
/* map flag names to and from flag codes*/
static 
struct strTbl FlagNames[] = {
		{"Underline", style_Underline},
		{"Hidden", style_Hidden},
		{"ReadOnly", style_ReadOnly},
		{"Pass thru", style_PassThru},
		{"Icon", style_Icon},
		{"ContinueIndent", style_ContinueIndent},
		{"Hinge", style_Hinge},
		{"NewPage", style_NewPage},
		{"ChangeBar", style_ChangeBar},
		{"OverBar", style_OverBar},
		{"NoWrap", style_NoWrap},
		{"NoFill", style_NoFill},
		{"KeepPriorNL", style_KeepPriorNL},
		{"KeepNextNL", style_KeepNextNL},
		{NULL, 0L},
};
/* map face names to and from face codes*/
static 
struct strTbl FaceNames[] = {
		{"Bold", fontdesc_Bold},
		{"Italic", fontdesc_Italic},
		{"Shadow", fontdesc_Shadow},
		{"Fixed width", fontdesc_Fixed},
		{"Outline", fontdesc_Outline},
		{"Thin", fontdesc_Thin},
		{"Black", fontdesc_Black},
		{"Medium", fontdesc_Medium},
		{"Heavy", fontdesc_Heavy},
		{"Condense", fontdesc_Condense},
		{NULL, 0L},
};
/* map baseline codes to distance */
static 
struct strTbl SubScrNames[] = {
		{"up 15", -15},
		{"super (up 6)", -6},
		{"up 4", -4},
		{"up 2", -2},
		{"sub (down 2)", 2},
		{"down 4", 4},
		{"down 6", 6},
		{NULL, 0L},
};


/* long CVDots(amt, unit)
	Convert the distance 'amt' measured in the given 'units' 
	to a distance measured in pixels (points)
	(should be a class procedure in style.c) 
*/

static
long CVDots(amt, unit)
	register long amt;
	enum style_Unit unit;
{
	switch (unit) {
            case style_RawDots:
                return amt;
            case style_Inches:
                return (amt * 72) >> 16;
            case style_CM:
                return (amt * 225 / 127) >> 12;
            case style_Points:
                return amt;
            default: 		/* style_Ems,  style_Lines */
                return 0;
        }
}

/* CVFractionalPoints(amt, unit)
	convert a value to a fixed-point floating value in points
              (long with binary point in the middle) 
*/

static long 
CVFractionalPoints(amt, unit)
	register long amt;
	enum style_Unit unit;
{
	switch (unit) {
            case style_RawDots:
                return amt << 16;
            case style_Inches:
                return amt * 72;
            case style_CM:
                return (amt<<4)*225/127;
            case style_Points:
                return amt << 16;
            default:
                return 0;		/* style_Ems,  style_Lines */
        }
}

static long
MapStringToVal(tbl, s)
	struct strTbl tbl[];
	char *s;
{
	long i;
	for (i = 0; tbl[i].str; i++)
		if (strcmp(tbl[i].str, s) == 0) 
			return (tbl[i].val);
	return -999L;
}

static char *
MapValToString(tbl, val)
	struct strTbl tbl[];
	long val;
{
	long i;
	for (i = 0; tbl[i].str; i++)
		if (tbl[i].val == val)
			return (tbl[i].str);
	return NULL;
}

/* MapDeltaToString(dots, basis)
	convert dots value dd to "dd" if ConstantSize or "+dd" or "-dd" for PreviousSize 
		result is in a static buffer 
		(NOTE: used for both FontSize and ScriptMovement: 
		ASSUMES parallel definition of style_FontSize 
			and style_ScriptMovement) */
static char *
MapDeltaToString(dots, basis)
	long dots;
	enum style_FontSize basis;
{
	static char buf[15];
	if (dots == 0) 
		*buf = '\0';
	else 
		sprintf(buf, "%s%ld", 
			(basis == style_ConstantFontSize) ? "" 
				:  ((dots >= 0) ? "+" : ""), 
			dots);
	return buf;
}

/* MapStringToDelta(str, dots, basis)
	convert string generated by MapDeltaToString back to SizeBasis and dots value 
*/
	static
MapStringToDelta(str, dots, basis)
	char *str;
	long *dots;
	enum style_FontSize *basis;
{
	*basis = (*str == '+' || *str == '-') ? style_PreviousFontSize 
				: style_ConstantFontSize;
	sscanf(str, "%ld", dots);
}

static void 
ChangeStyle(self)
	register struct lookzview *self;
{
	self->curstyle->template = 0;
	stylesheet_NotifyObservers(self->curss, self);	/*BOGUS XXX
			The protocol for stylesheet observed_changed status is
			to send the address of the view that caused the change;
			Thus its own ObservedChanged routine can reject it.  */
}

/* RulerValueChanged(rv, self, icon, newvalue)
	update the style parameter given by setProc 
		relative to basis to the given value 
	set values in centimeters  {2.54/72 ==  127/(225*16)}
	use relative zero to mean no value */

static void
RulerValueChanged(rv, self, icon, newvalue)
	struct lprrulerview *rv;
	register struct lookzview *self;
	enum iconcode icon;
	long newvalue;
{
	if (self->curstyle == NULL) return;
	switch (icon) {
		case leftIcon:
			if (newvalue <= lprrulerview_NoValue) newvalue = 0;
			style_SetNewLeftMargin(self->curstyle, style_LeftMargin,
					(newvalue>>4)*127/225, style_CM);
			break;
		case rightIcon:
			if (newvalue <= lprrulerview_NoValue) newvalue = 0;
			style_SetNewRightMargin(self->curstyle, style_RightMargin,
					(newvalue>>4)*127/225, style_CM);
			break;
		case paraIcon:
			if (newvalue <= lprrulerview_NoValue) newvalue = 0;
			style_SetNewIndentation(self->curstyle, style_LeftMargin,
					(newvalue>>4)*127/225, style_CM);
			break;
	}
	ChangeStyle(self);
}



/* SetBitsForCode(code, names, tbl, parity)
	set bits in stringtbl -tbl- for the names given in -names- for the bits in -code- 
	parity adjusts for the nonsense of the OutXxxx stuff 
*/
	static
SetBitsForCode(code, names, tbl, parity)
	unsigned long code;
	struct strTbl names[];
	struct stringtbl *tbl;
	long parity;
{
	long i;
	for (i = 1; code; 	i <<= 1, code >>= 1) 
		if ((1 & code) ^ parity)
			stringtbl_SetBitOfEntry(tbl,
				stringtbl_AddString(tbl, 
					(unsigned char *)MapValToString(names, i)), 
				1);
}

static void 
UnpackStyle(self)
	struct lookzview *self;
{
	register struct style *style = self->curstyle;
	register struct stringtbl **st = self->st;
	{	/* justification mode */
		stringtbl_ClearBits(st[(long)TNmode]);
		stringtbl_SetBit(st[(long)TNmode], 
			(unsigned char *)MapValToString(ModeNames,
					style_GetJustification(style)),
			1);
	}
	{	/* font family */
		char font[50];
		stringtbl_ClearBits(st[(long)TNfont]);
		style_GetFontFamily(style, font, sizeof (font));
		stringtbl_SetBitOfEntry(st[(long)TNfont],
				stringtbl_AddString(st[(long)TNfont], font), 1);
	}
	{	/* font size */
		enum style_FontSize basis;
		long points;
		stringtbl_ClearBits(st[(long)TNsize]);
		style_GetFontSize(style, &basis, &points);
		stringtbl_SetBitOfEntry(st[(long)TNsize],
			stringtbl_AddString(st[(long)TNsize], 
				(unsigned char *)MapDeltaToString(points, basis)), 
			1);
	}
	{	/* font script movement  (baseline adjustment) */
		enum style_ScriptMovement basis;
		long amt;
		enum style_Unit unit;
		char *str;
		stringtbl_ClearBits(st[(long)TNsubscr]);
		style_GetFontScript(style, &basis, &amt, &unit);
		str = MapValToString(SubScrNames, CVDots(amt, unit));
		stringtbl_SetBitOfEntry(st[(long)TNsubscr],
			stringtbl_AddString(st[(long)TNsubscr], 
				(unsigned char *)((str) ? str : 
				MapDeltaToString(CVDots(amt, unit), basis))), 
			1);
	}

	stringtbl_ClearBits(st[(long)TNenable]);
	stringtbl_ClearBits(st[(long)TNdisable]);
	{	/* font face codes */
		SetBitsForCode(style_GetAddedFontFaces(style), 
				FaceNames, st[(long)TNenable], 0);
		SetBitsForCode(style_GetRemovedFontFaces(style), 
				FaceNames, st[(long)TNdisable], 1);
	}
	{	/* flags */
			/* KLUDGE:  missing macro in style.ch 
				access fields of style directly */
		SetBitsForCode(style->AddMiscFlags, 
				FlagNames, st[(long)TNenable], 0);
		SetBitsForCode(style->OutMiscFlags, 
				FlagNames, st[(long)TNdisable], 1);
	}
	{	/* leftmargin, rightmargin, paraindent */
		long leftmargin, rightmargin, paraindent;
		enum style_MarginValue Basis;
		long Operand;
		enum style_Unit Unit;
		leftmargin = rightmargin = paraindent = lprrulerview_NoValue;	/* initially no icons */
		style_GetNewLeftMargin(style, &Basis, &Operand, &Unit);
		if (Basis == style_LeftMargin && Operand != 0)
			leftmargin = CVFractionalPoints(Operand, Unit);
		style_GetNewRightMargin(style, &Basis, &Operand, &Unit);
		if (Basis == style_RightMargin && Operand != 0)
			rightmargin = CVFractionalPoints(Operand, Unit);
		style_GetNewIndentation(style, &Basis, &Operand, &Unit);
		if (Basis == style_LeftMargin && Operand != 0)
			paraindent = CVFractionalPoints(Operand, Unit);
		lprrulerview_SetValues(self->rulerview, leftmargin, rightmargin, paraindent);
	}
	self->NeedsUnpack = FALSE;
}

static char *
InitialWord(s)
	char *s;
{
	static char buf[50];
	register char *bx, *sx;
	long i;
	for (bx=buf, sx=s, i=48;   i && *sx && *sx != '~' && *sx != ',';   i--)
		*bx++ = *sx++;
	*bx++ = '\0';
	return buf;
}

/* clean(s) 
	removes spaces and non-alphabetics.  It changes uppercase to lower 
	result must be less than 50 bytes 
	returns a pointer to an internal buffer
*/
	static char *
clean(s)
	char *s;
{
	static char buf[50];
	register char *bx, *sx;
	long i;
	for (bx=buf, sx=s, i=48;   i && *sx;   i--) {
		register char c = *sx++;
		if ( ! isalnum(c)) {}
		else if (isupper(c)) 
			*bx++ = tolower(c);
		else
			*bx++ = c;
	}
	*bx++ = '\0';
	return buf;
}

/* SetStyleDisplay(self, st)
	initiates editing the style 'st'
*/
	static void
SetStyleDisplay(self, st)
	register struct lookzview *self;
	register struct style *st;
{
	self->curstyle = st;
	if (st) 
		UnpackStyle(self);
}

/* ClearStyleDisplay(self)
	removes highlights from all attributes in the tableux
*/

	static void
ClearStyleDisplay(self)
	register struct lookzview *self;
{
	long i;
	for (i=(long)TNfont; i<=(long)TNdisable; i++)
		stringtbl_ClearBits(self->st[i]);

	lprrulerview_SetValues(self->rulerview, lprrulerview_NoValue,
			 lprrulerview_NoValue, lprrulerview_NoValue);
}

/* CloseStyleSheet(self)
	terminates editing the current style
*/
	static void
CloseStyleSheet(self)
	register struct lookzview *self;
{
	/* The document is updated for all changes, so there is no need
	to do anything about the style currently displayed */
	
	if ( ! self->foundstylesheet) return;
	ClearStyleDisplay(self);
	stringtbl_Clear(self->st[(long)TNmenucard]);
	stringtbl_Clear(self->st[(long)TNstylename]);
	stylesheet_RemoveObserver(self->curss, self);
}

/* called by EnumerateStyles in OpenStyleSheet */
static boolean
AddCardName(s, st)
	struct style *s;
	register struct stringtbl *st;
{
	char *mnnm = style_GetMenuName(s);
	if (mnnm != NULL)    stringtbl_AddString(st, (unsigned char *)InitialWord(mnnm));
	return FALSE;
}

static
OpenStyleSheet(self, ss)
	register struct lookzview *self;
	register struct stylesheet *ss;
{
	self->curss = ss;
	stylesheet_AddObserver(ss, self);
	/* set up the cardmenu and stylename data objects */
	stylesheet_EnumerateStyles(self->curss, AddCardName,
			 self->st[(long)TNmenucard]);
	self->foundstylesheet = TRUE;
}


/* called by EnumerateStyles in HitMenuCard */
static boolean
AddStyleName(s, lv)
	register struct style *s;
	register struct lookzview *lv;
{
	register char *mnnm = style_GetMenuName(s), *ThisStylename;
	if (mnnm != NULL && strcmp(lv->curcard, InitialWord(mnnm)) == 0) {
		ThisStylename = index(mnnm, ',');
		if (ThisStylename) 
			stringtbl_AddString(lv->st[(long)TNstylename], 
				(unsigned char *)InitialWord(ThisStylename+1));
	}
	return FALSE;
}

/* called as an ItemHitProc for stringtbl[TNmenucard] 
		set up the stylename stringtbl */
static
HitMenuCard(st0, lv, accnum)
	struct stringtbl *st0;
	register struct lookzview *lv;
	long accnum;
{
	strtblview_OneOnly(st0, lv, accnum);
	stringtbl_Clear(lv->st[(long)TNstylename]);
	ClearStyleDisplay(lv);
	lv->curstyle = NULL;
	lv->curcard = (char *)stringtbl_GetStringOfEntry(st0, accnum);
	stylesheet_EnumerateStyles(lv->curss, AddStyleName, lv);
}


static char *TempStyleName;  /* holds the stylename during 
		EnumerateStyles (...FindAndUnpack...) */
/* FindAndUnPack is called by Enumerate styles in HitStyleName
		it finds the style named by lv->curcard and TempStyleName
		then it unpacks that style */
static boolean
FindAndUnpack(s, lv)
	register struct style *s;
	register struct lookzview *lv;
{
	register char *mnnm = style_GetMenuName(s);
	if (mnnm != NULL && strcmp(lv->curcard, InitialWord(mnnm)) == 0) {
		register char *ThisStylename = index(mnnm, ',');
		if (ThisStylename
				&& strcmp(InitialWord(ThisStylename+1), 
							TempStyleName) == 0) {
			SetStyleDisplay(lv, s);
			return TRUE;  /* finished */
		}
	}
	return FALSE;
}

/* HitStyleName is called as an ItemHitProc for the stylename stringtbl */
static
HitStyleName(st1, lv, accnum)
	register struct stringtbl *st1;
	register struct lookzview *lv;
	long accnum;
{
	strtblview_OneOnly(st1, lv, accnum);
	ClearStyleDisplay(lv);
	TempStyleName = (char *) stringtbl_GetStringOfEntry(lv->st[(long)TNstylename],
			accnum);	/* side arg to FindAndUnpack KLUDGE */
	stylesheet_EnumerateStyles(lv->curss, FindAndUnpack, lv);
}


/* called as an ItemHitProc for stringtbl[TNfont] 
		modify font  in the stylesheet */
static
HitFont(st, self, accnum)
	struct stringtbl *st;
	register struct lookzview *self;
	long accnum;
{
	if (self->curstyle == NULL) return;
	strtblview_ZeroOrOne(st, self, accnum);
	if (stringtbl_GetBitOfEntry(st, accnum))
		style_SetFontFamily(self->curstyle, (char *)stringtbl_GetStringOfEntry(st, accnum));
	else {
		/* delete font reference from style.   
			KLUDGE KLUDGE: style_SetFontFamily 
			is BOGUS: we do it directly here */
		if (self->curstyle->FontFamily != NULL) {
			free(self->curstyle->FontFamily);
			self->curstyle->FontFamily = NULL;
		}
	}
	ChangeStyle(self);
}

/* WARNING:  KLUDGE KLUDGE
	style.c, style.ch, and default.tpl treat the value of FontSize as Points,
		but do not shift the value left 16 bits.
	the same modules treat the FontScriptMovement value as points, 
		but DO shift the values left 16 bits.
	The code below reflects this "convention".
*/

/* called as an ItemHitProc for stringtbl[TNsize] 
		modify size  in the stylesheet */
static
HitSize(st, self, accnum)
	struct stringtbl *st;
	register struct lookzview *self;
	long accnum;
{
	enum style_FontSize basis;
	long points;
	if (self->curstyle == NULL) return;
	strtblview_ZeroOrOne(st, self, accnum);
	if (stringtbl_GetBitOfEntry(st, accnum)) 
		MapStringToDelta(((char *)stringtbl_GetStringOfEntry(st, accnum)), &points, &basis);
	else
		points = 0, basis = style_PreviousFontSize;
	style_SetFontSize(self->curstyle, basis, points);
	ChangeStyle(self);
}

/* called as an ItemHitProc for stringtbl[TNmode] 
		modify JustificationMode  in the stylesheet */
static
HitMode(st, self, accnum)
	struct stringtbl *st;
	register struct lookzview *self;
	long accnum;
{
	if (self->curstyle == NULL) return;
	strtblview_ZeroOrOne(st, self, accnum);
	if (stringtbl_GetBitOfEntry(st, accnum)) {
		/* (The braces around this clause are NECESSARY because
			style_SetJustification is surrounded by braces) */
		style_SetJustification(self->curstyle, 
				(enum style_Justification)MapStringToVal(
						ModeNames, 
						((char *)stringtbl_GetStringOfEntry(st, accnum))));
	}
	else 
		style_SetJustification(self->curstyle, style_PreviousJustification);
	ChangeStyle(self);
}

/* called as an ItemHitProc for stringtbl[TNsubscr] 
		modify subscription  in the stylesheet */
static
HitSubscr(st, self, accnum)
	struct stringtbl *st;
	register struct lookzview *self;
	long accnum;
{
	enum style_ScriptMovement  basis = style_PreviousScriptMovement;
	long points = 0;
	if (self->curstyle == NULL) return;
	strtblview_ZeroOrOne(st, self, accnum);
	if (stringtbl_GetBitOfEntry(st, accnum)) {
		register char *str = (char *)stringtbl_GetStringOfEntry(st, accnum);
		points = MapStringToVal(SubScrNames, str);
		if (points == -999L) 
			MapStringToDelta(str, &points, (enum style_FontSize *)&basis);
	}
	/* else points=0 and basis=style_PreviousScriptMovement */
	style_SetFontScript(self->curstyle, basis, points, style_Points);
	ChangeStyle(self);
}

/* FinagleStyleBits is called by HitEnable and HitDisable to modify the
	XxxFontFaces or XxxMiscFlags fields of the style.
	KLUDGE KLUDGE  This routine does not use the
	access procedures defined in style.ch;  that module 
	DOES NOT DEFINE six of the eight functions that are required */
static
FinagleStyleBits(faces, flags, str, bit, parity)
	unsigned long *faces, *flags;
	char *str;
	unsigned long bit;
	long parity;
{
	unsigned long *which;
	unsigned long val;
	val = MapStringToVal(FaceNames, str);
	if (val == -999L) {
		val = MapStringToVal(FlagNames, str);
		if (val == -999L) return;		/* unknown str value */
		which = flags;
	}
	else which = faces;
	if (bit ^ parity)
		*which |= val;		/* OR in a bit */
	else
		*which &= ~val;	/* remove a bit */
}

/* called as an ItemHitProc for stringtbl[TNenable] 
		modify enable flags  in the stylesheet */
static
HitEnable(st, self, accnum)
	struct stringtbl *st;
	register struct lookzview *self;
	long accnum;
{
	if (self->curstyle == NULL) return;
	strtblview_ZeroOrMany(st, self, accnum);
	FinagleStyleBits(((unsigned long *)&self->curstyle->AddFontFaces), 
			((unsigned long *)&self->curstyle->AddMiscFlags),
			((char *)stringtbl_GetStringOfEntry(st, accnum)), 
			stringtbl_GetBitOfEntry(st, accnum), 0);
	ChangeStyle(self);
}
/* called as an ItemHitProc for stringtbl[TNdisable] 
		modify disable flags  in the stylesheet */
static
HitDisable(st, self, accnum)
	struct stringtbl *st;
	register struct lookzview *self;
	long accnum;
{
	if (self->curstyle == NULL) return;
	strtblview_ZeroOrMany(st, self, accnum);
	FinagleStyleBits(((unsigned long *)&self->curstyle->OutFontFaces), 
			((unsigned long *)&self->curstyle->OutMiscFlags),
			((char *)stringtbl_GetStringOfEntry(st, accnum)), 
			stringtbl_GetBitOfEntry(st, accnum), 1);
	ChangeStyle(self);
}

static
HitShrink(shrink, action, self)
	struct labelview *shrink;
	register enum view_MouseAction  action;
	register struct lookzview *self;
{
	if (action != view_LeftDown && action != view_RightDown)
		return;
	lookzview_SetVisibility(self, ! lookzview_GetVisibility(self));
	lookzview_WantNewSize(self, self);
	lookzview_WantInputFocus(self, self);
}
	

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 *	Now the routines for the view 
 *		(Above were the routines for the application
 *	
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void
ChooseShrinkIcon(self)
	register struct lookzview *self;
{
	label_SetText((struct label *)self->shrinkicon->header.view.dataobject, 
			(self->HasInputFocus) ? "-" : ",");
}


/* LL - generate a horizontal or vertical pair of views 
	first arg is H or V to choose direction, second is percent of split,
	third and fourth args are the views 
	all boundaries are movable */

enum dir {H, V};

static struct lpair *
LL(dir, pct, top, bot)
	enum dir dir;
	long pct;
	struct lpair *top, *bot;
{
	/* XXX (sigh) would really like lines to be movable, 
			but the wm has a terrible time leak in that area */
	register struct lpair *ret = lpair_New();
	if (dir == H) 
		lpair_HSplit(ret, top, bot, pct, FALSE);
	else
		lpair_VSplit(ret, top, bot, pct, FALSE);
	return ret;
}

/* LBL - generate an lpair with a fixed size top having a label */
static struct lpair *
LBL (label, view)
	char *label;
	struct view *view;
{
	register struct label *l = label_New();
	register struct labelview *lv = labelview_New();
	label_SetFont(l, "andysans", fontdesc_Bold, 8);
	label_SetText(l, label);
	labelview_SetDataObject(lv, l);
	return  (struct lpair *)(bpair_VTFixed(bpair_New(), lv, view, 12, FALSE));
}


boolean
lookzview__InitializeClass(ClassID)
	struct classhdr *ClassID;
{
	MenuList = menulist_New();
	Keymap = keymap_New();
	bind_BindList(MenuOptions, Keymap, MenuList,
				&lookzview_classinfo);
	return TRUE;
}

boolean
lookzview__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct lookzview  *self;
{
	long i, j;
	struct lpair *R[5];
	struct label *shrinklabel, *emptylabel;
	struct labelview *emptylabelview;

ENTER(lookzview__InitializeObject);

	self->MyMenus = menulist_DuplicateML(MenuList, self);

	self->Keystate = keystate_Create(self, Keymap);
/*
	self->dummyview_cursor = cursor_Create( self );
*/
	self->embedded = TRUE;
	self->foundstylesheet = FALSE;
	self->curcard = NULL;
	self->curstyle = NULL;
	self->curstyle = NULL;
	self->HasInputFocus = FALSE;
	self->NeedsUnpack = FALSE;
	self->OnScreen = FALSE;
	self->Linked = FALSE;

	shrinklabel = label_New();
	label_SetFont(shrinklabel, "icon", fontdesc_Plain, 12);
	self->shrinkicon = labelview_New();
	labelview_SetHitProc(self->shrinkicon, HitShrink, self);
	labelview_SetDataObject(self->shrinkicon, shrinklabel);
	/* defer initial ChooseShrinkIcon because it will call SetText,
		which calls NotifyObservers, 
		which calls ObservedChanged in labelview, 
		which calls WantNewSize, 
		which requires having a parent 
		(The error is in having SetText call NotifyObservers
		rather than have that done by the caller of SetText.)
	*/

	emptylabel = label_New();
	emptylabelview = labelview_New();
	label_SetFont(emptylabel, "icon", fontdesc_Plain, 12);
	label_SetText(emptylabel, "");
	labelview_SetDataObject(emptylabelview, emptylabel);

	for (i = 0; InitialStrings[i]; i++) {
		self->st[i] = stringtbl_New();
		for (j = 0; InitialStrings[i][j]; j++)
			stringtbl_AddString(self->st[i], 
				(unsigned char *)InitialStrings[i][j]);

		self->stv[i] = strtblview_New();
		strtblview_SetDataObject(self->stv[i], self->st[i]);
	}
	strtblview_SetItemHitProc(self->stv[(long)TNmenucard], HitMenuCard, self);
	strtblview_SetItemHitProc(self->stv[(long)TNstylename], HitStyleName, self);
	strtblview_SetItemHitProc(self->stv[(long)TNfont], HitFont, self);
	strtblview_SetItemHitProc(self->stv[(long)TNsize], HitSize, self);
	strtblview_SetItemHitProc(self->stv[(long)TNmode], HitMode, self);
	strtblview_SetItemHitProc(self->stv[(long)TNsubscr], HitSubscr, self);
	strtblview_SetItemHitProc(self->stv[(long)TNenable], HitEnable, self);
	strtblview_SetItemHitProc(self->stv[(long)TNdisable], HitDisable, self);

	self->ruler = lprruler_New();
	self->rulerview = lprrulerview_New();
	lprrulerview_SetDataObject(self->rulerview, self->ruler);
	lprrulerview_SetValues(self->rulerview, lprrulerview_NoValue,
			 lprrulerview_NoValue, lprrulerview_NoValue);
	lprrulerview_SetValueChangeProc(self->rulerview, RulerValueChanged, self);
	

	/* build image 
		there are ten apparent rectangles arranged like this:
	
	icon		MENUCARD		MENUNAME

	FONT	ENABLE		DISABLE		JUSTIFI-		BASE-
									CATION		LINE
		FONTSIZE - - - - - - - - - - - >

	RULER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - >

	All but the icon rectangle have a small upper rectangle for the label.
	There is a small and empty label icon between the top row and the rest.
	There are five groups of rectangles:
		R[0]	(icon, MENUCARD, MENUNAME)
		R[1]	(FONT, ENABLE, DISABLE)
		R[2]	(JUSTIFICATION, BASELINE)
		R[3]	(FONTSIZE)
		R[4]	(RULER)
	We first create each group and then arrange them as a whole.
*/
	R[0] = LL(V, 4, 
		self->shrinkparent = LL(H, 94,
			(struct lpair *)self->shrinkicon,
			LL (H, 69,
				LBL("menu card", 
					(struct view *)self->stv[(long)TNmenucard]),
				LBL("name on menu card", 
					(struct view *)self->stv[(long)TNstylename])
			)
		),
		(struct lpair *)emptylabelview  /* dummy to make thin line below menu stuff */
	);
	R[1] = LL(H, 62,
		LBL("font", (struct view *)self->stv[(long)TNfont]),
		LL(H, 50,
			LBL("enable", (struct view *)self->stv[(long)TNenable]),
			LBL("disable", (struct view *)self->stv[(long)TNdisable])
		)
	);
	R[2] = LL(H, 50,
		LBL("justify", (struct view *)self->stv[(long)TNmode]),
		LBL("baseline", (struct view *)self->stv[(long)TNsubscr])
	);
	R[3] = LBL("font size", (struct view *)self->stv[(long)TNsize]);
	R[4] = LBL("relative margins and paragraph indent", (struct view *)self->rulerview);

	/* put it all together */
	self->image = LL(V, 74,
		R[0],		/* menucard and menuname */
		LL(V, 32,
			LL(H, 35,
				LL(V, 37,
					R[1],		/* font, enable, disable */
					R[3]		/* fontsize */
				),
				R[2]		/* justification and baseline */
			),
			R[4]		/* ruler */
		)
	);

	ChooseShrinkIcon(self);

LEAVE(lookzview__InitializeObject);

	return TRUE;
}

static
ChopTree(branch, lpairInfo)
	struct lpair *branch;
	struct classinfo *lpairInfo;
{
	struct classinfo *branchInfo = class_GetType(branch);
	/* destroy both children if the branch is either an lpair or
		a sub class of lpair (such as bpair) 
		XXX use knowledge that class_GetType returns ptr to classinfo */
	if (branch == NULL) {}
	else if (branchInfo == lpairInfo || branchInfo->superclass == lpairInfo) {
		struct view* o0 = lpair_GetNth(branch, 0);
		struct view* o1 = lpair_GetNth(branch, 1);
		lpair_SetNth(branch, 0, NULL);
		lpair_SetNth(branch, 1, NULL);
		lpair_UnlinkTree(branch);
		ChopTree((struct lpair *)o0, lpairInfo);
		ChopTree((struct lpair *)o1, lpairInfo);
		lpair_Destroy(branch);
	}
	else {
		struct view *v = (struct view *)branch;
		/* destroy a childview and its data object */
		struct dataobject *dobj = ((struct view *)branch)->dataobject;
		view_UnlinkTree(v);
		view_Destroy(v);
		dataobject_Destroy(dobj);
	}
}

void 
lookzview__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct lookzview  *self;
{
ENTER(lookzview__FinalizeObject);
	if (self->foundstylesheet)
		stylesheet_RemoveObserver(self->curss, self);

	lookzview_SetVisibility(self, TRUE);
	lookzview_LinkTree(self, self->header.view.parent);

	ChopTree(self->image, class_GetType(self->image));

	menulist_Destroy(self->MyMenus);
	keystate_Destroy(self->Keystate);
LEAVE(lookzview__FinalizeObject);
}

struct lookzview *
lookzview__GetApplicationLayer(self)
	struct lookzview *self;
{
	self->embedded = FALSE;
	/* XXX ought to delete the shrink-icon from the view tree */
	lookzview_WantInputFocus(self, self);
	return self;
}

	void
lookzview__LinkTree(self, parent)
	struct lookzview *self;
	struct view *parent;
{
ENTER(lookzview__LinkTree);
	super_LinkTree(self, parent);
	if (lookzview_GetVisibility(self)) {
		labelview_LinkTree(self->shrinkicon, NULL);
		lpair_SetNth(self->shrinkparent, 0, self->shrinkicon);
		lpair_LinkTree(self->image, self);
	}
	else {
		lpair_LinkTree(self->image, NULL);
		lpair_SetNth(self->shrinkparent, 0, NULL);
		labelview_LinkTree(self->shrinkicon, self);
	}
	self->Linked = TRUE;
LEAVE(lookzview__LinkTree);
}


/* intercept all requests from children for inputfocus 
		and cause the lookz itself to request inputfocus */
void 
lookzview__WantInputFocus(self, child)
	register struct lookzview  *self;
	register struct view *child;
{
ENTER(lookzview__WantInputFocus);
	if ( ! self->HasInputFocus)
	super_WantInputFocus(self, self);
LEAVE(lookzview__WantInputFocus);
}

	void 
lookzview__ReceiveInputFocus(self)
	register struct lookzview  *self;
{
ENTER(lookzview__ReceiveInputFocus);
	lookzview_PostMenus(self, self->MyMenus);

	self->Keystate->next = NULL;
	lookzview_PostKeyState(self, self->Keystate);
	self->HasInputFocus = TRUE;
	ChooseShrinkIcon(self);
	if (self->Linked)
	        lookzview_WantUpdate( self, self );
LEAVE(lookzview__ReceiveInputFocus);
}


void 
lookzview__LoseInputFocus(self)
	register struct lookzview  *self;
{
ENTER(lookzview__LoseInputFocus);
	self->HasInputFocus = FALSE;
	ChooseShrinkIcon(self);
	lookzview_WantUpdate(self, self);
LEAVE(lookzview__LoseInputFocus);
}

void 
lookzview__FullUpdate(self, type, left, top, width, height)
	register struct lookzview  *self;
	register enum view_UpdateType  type;
	register long  left, top, width, height;
{
	struct rectangle r;
	self->OnScreen = (type != view_Remove);
	lookzview_GetLogicalBounds(self, &r);
DEBUG(("FullUpdate type %d  redraw (%d,%d,%d,%d) within (%d,%d,%d,%d)\n", 
type, left, top, width, height, r.left, r.top, r.width, r.height));
	if (type == view_FullRedraw && ! self->foundstylesheet) {
		/* BOGOSITY ALERT:  we grub around in the parent to
			find its stylesheet */
		register struct view *v;
		for (v = (struct view *)self; v != NULL; v = v->parent) 
			if (strcmp("textview", class_GetTypeName(v)) == 0) {
				register struct text *t = (struct text *)v->dataobject;
				if (t && t->styleSheet) 
					OpenStyleSheet(self, t->styleSheet);
				break;
			}
	}
	if (type == view_FullRedraw && self->embedded && self->OnScreen) {
		lookzview_SetTransferMode(self, graphic_COPY);
		lookzview_MoveTo(self, 0, 0);
		lookzview_DrawLineTo(self, r.width-1, 0);
		lookzview_DrawLineTo(self, r.width-1, r.height-1);
		lookzview_DrawLineTo(self, 0, r.height-1);
		lookzview_DrawLineTo(self, 0, 0);

		r.top++, r.left++, r.height-=2, r.width-=2;
	}

	if (! self->Linked)
		lookzview_LinkTree(self, self->header.view.parent);

DEBUG(("	Drawable at 0x%lx\n", lookzview_GetDrawable(self)));

	if (lookzview_GetVisibility(self)) {
		if (type != view_PartialRedraw 
				&& type != view_LastPartialRedraw) {
			lpair_InsertView(self->image, self, &r);
		}
		lpair_FullUpdate(self->image,  type, left, top, width, height);
	}
	else {
		if (type != view_PartialRedraw 
				&& type != view_LastPartialRedraw) 
			labelview_InsertView(self->shrinkicon, self, &r);
		labelview_FullUpdate(self->shrinkicon,  type, left, top, width, height);
	}
LEAVE(lookzview__FullUpdate);
}


void 
lookzview__Update( self )
	register struct lookzview *self;
{
ENTER(lookzview__Update);
	if (! self->OnScreen) return;
	if ( ! self->Linked)
		lookzview_FullUpdate(self, view_FullRedraw, 0, 0,
				lookzview_GetLogicalWidth(self),
				lookzview_GetLogicalHeight(self));
	else if (lookzview_GetVisibility(self)) {
		if (self->NeedsUnpack) 
			UnpackStyle(self);
		lpair_Update(self->image);
	}
	else 
		labelview_Update(self->shrinkicon);
LEAVE(lookzview__Update);
}

struct view *
lookzview__Hit(self, action, x, y, num_clicks)
	register struct lookzview  *self;
	register enum view_MouseAction  action;
	register long  x, y, num_clicks;
{
	struct view *ret;
	boolean oldvis=lookzview_GetVisibility(self);
ENTER(lookzview__Hit);
	if (action == view_NoMouseEvent)
		return (struct view *)self;
	if ( ! self->OnScreen) return NULL;
DEBUG(("	OldVis %d\n", oldvis));
	if (oldvis)
		ret = (struct view *)lpair_Hit(self->image, action, x, y, num_clicks);
	else
		ret = (struct view *)labelview_Hit(self->shrinkicon, action, x, y, num_clicks);
LEAVE(lookzview__Hit);
	if (oldvis == lookzview_GetVisibility(self))
		return ret;
	else return (struct view *)self;	/* visibility changed */
}

enum view_DSattributes
lookzview__DesiredSize( self, width, height, pass, 
				desiredWidth, desiredHeight ) 
	register struct lookzview *self;
	long width;
	long height;
	enum view_DSpass pass;
	long *desiredWidth;
	long *desiredHeight;
{
	if (lookzview_GetVisibility(self)) 
		*desiredWidth = 550,  *desiredHeight = 260;
	else
		*desiredWidth = 26,  *desiredHeight = 20;
DEBUG(("Desired Size %d x %d\n", *desiredWidth, *desiredHeight));
	return view_Fixed;
}

void
lookzview__Print( self, file, processor, format, level )
	register struct lookzview 	 *self;
	register FILE   file;
	register char  	 *processor;
	register char  	 *format;
	register boolean  	level;
{
	/* never print anything */
}

void
lookzview__ObservedChanged(self, dobj, status)
	register struct lookzview  *self;
	struct stylesheet  *dobj;
	long  status;
{
	if (status == (long)self || self->curstyle == NULL 
				|| status == observable_OBJECTDESTROYED) 
		return;	/* changed by self  or deleting it */

	self->NeedsUnpack = TRUE;
	lookzview_WantUpdate(self, self);
}


void
lookzview__SetVisibility(self, visibility)
	struct lookzview *self;
	boolean visibility;
{
	if (visibility != lookzview_GetVisibility(self)) {
DEBUG(("Change Visibility to %d\n", visibility));
		/* I am surprised I need the cast in the next line */
		lookz_SetVisibility(((struct lookz *)self->header.view.dataobject), visibility);
		self->Linked = FALSE;
	}
}

boolean
lookzview__GetVisibility(self)
	struct lookzview *self;
{
	return lookz_GetVisibility(self->header.view.dataobject);
}

void
lookzview__SetStyleSheet(self, ss)
	struct lookzview *self;
	struct stylesheet *ss;
{
	CloseStyleSheet(self);
	OpenStyleSheet(self, ss);
}

struct stylesheet *
lookzview__GetStyleSheet(self)
	struct lookzview *self;
{
	return self->curss;
}

static void UpdateDocument(self)
	register struct lookzview *self;
{
	if (self->embedded) {
		/* BOGOSITY ALERT:  we call Full_Update of parent */
		register struct view *v;
		for (v = (struct view *)self; v != NULL; v = v->parent) 
			if (strcmp("textview", class_GetTypeName(v)) == 0) {
				/* this is exceedingly dangerous XXX XXX
					We call a FullUpdate 
					without establishing any environment */
				view_FullUpdate(v, view_FullRedraw, 0, 0, 0, 0);
				break;
			}
	}
}


/* * * * * * * * * * * * * /
/*   A D D   S T Y L E   */
/* * * * * * * * * * * * */

static char CardFound[50];   /* result from FindCardName */

/* FindCardName(s, nm)
	compare nm to the menu card name in s
	when match, set CardFound and exit
*/
	static boolean
FindCardName(s, nm)
	register struct style *s;
	register char *nm;
{
	register char *mnnm = style_GetMenuName(s);
	register char *iw;
	if (mnnm != NULL  &&  (iw=InitialWord(mnnm)) != NULL
			&& FOLDEDEQ(nm, iw)) {
		/* found a match */
		long len;
		char *comma;
		comma = index(mnnm, ',');
		if (comma == NULL)
			len = strlen(mnnm);
		else
			len = comma - mnnm;
		if (len > 49)  len = 49;
		strncpy(CardFound, mnnm, len);
		CardFound[len] = '\0';
		return TRUE;
	}
	return FALSE;
}

/* NewMenuName(name)
	compute a MenuName string for name
	return pointer to static storage containing the name
*/
	static char *
NewMenuName(lv, name)
	struct lookzview *lv;
	char *name;
{
	static char buf[100];
	char *tilde, *comma;
	char *eltloc;

	tilde = index(name, '~');
	comma = index(name, ',');

	*CardFound = '\0';
	if (comma != NULL && (tilde == NULL || tilde > comma)) {
		/* There is no tilde for the card name,
			see if there is an existing menu card */
		*comma = '\0';
		stylesheet_EnumerateStyles(lv->curss, FindCardName, name);
		*comma = ',';
	}
	if (*CardFound != '\0') {
		/* use existing card name and the remainder of 'name' */
		register long len = strlen(CardFound);
		strcpy(buf, CardFound);
		eltloc = buf + len;
		strncpy(eltloc++, comma, 99 - len);
	}
	else {
		/* new card and new style.  just do capitalization */
		strncpy(buf, name, 99);
		if (islower(*buf))  *buf = toupper(*buf);
		eltloc = (comma == NULL) ? NULL 
			: buf + (comma-name) + 1;
	}
	buf[99] = '\0';
	if (eltloc != NULL && islower(*eltloc)) 
		*eltloc = toupper(*eltloc);
	return buf;
}

static void AddStyle(self)
	register struct lookzview *self;
{
	register struct style *newsty;
	char *stylename;
	long accnum;
	char name[75], temp[75];

	if ( ! self->foundstylesheet) return;	/* can't add if have no style sheeet */
	ClearStyleDisplay(self);
	stringtbl_ClearBits(self->st[(long)TNstylename]);

	sprintf(temp, "%s,", self->curcard);
	if (message_AskForString(self, 0, "Name for new style: ", temp, name, 75) < 0 
			|| *temp == '\0') {
		message_DisplayString(self, 0, "Cancelled.");
		return;
	}

	stringtbl_ClearBits(self->st[(long)TNmenucard]);
	accnum = stringtbl_AddString(self->st[(long)TNmenucard], 
			(unsigned char *)InitialWord(name));
	stringtbl_SetBitOfEntry(self->st[(long)TNmenucard], accnum, 1);
	self->curcard = (char *)stringtbl_GetStringOfEntry(self->st[(long)TNmenucard], accnum);

	stylename = index(name, ',');
	if (stylename) stylename = InitialWord(stylename+1);

	if ( stylename == NULL || *stylename == '\0') {
		message_DisplayString(self, 0, 
			"Missing comma or stylename.  Enter name as:  <MenuCard>,<StyleName>");
		return;
	}
	accnum = stringtbl_AddString(self->st[(long)TNstylename], 
				(unsigned char *)stylename);
	stringtbl_SetBitOfEntry(self->st[(long)TNstylename], accnum, 1);

	stylename = clean(stylename);
	/* by this time 'stylename' contains an all lowercase version of the 
		name after the comma without any ~00 */
	newsty=stylesheet_Find(self->curss, stylename);
	if (newsty == NULL) {
		/* create the style */
		newsty = style_New();
		style_SetName(newsty, stylename);
		stylesheet_Add(self->curss, newsty);	
		style_SetMenuName(newsty, NewMenuName(self, name));
	}
	else if (index(name, '~') != NULL) {
		/* existing style and the input contains a ~, 
				change the menu name */
		style_SetMenuName(newsty, name);
	}

	SetStyleDisplay(self, newsty);
}
