/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/lprrulv.ch,v 2.7 89/04/04 19:38:56 wjh Exp $ */
/* $ACIS:lprrulv.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/lprrulv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidlprrulerview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/lprrulv.ch,v 2.7 89/04/04 19:38:56 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
lprrulerview.H

	Class definitions for lprruler view

	displays and allows editting of the values for three icons: 
		leftmargin, rightmargin, and paragraph indent
	the measurement units toggle among Inches, Centimeters, and Points

	values are communicated with a client in fractional pixels: 
		the value is a long word with the binary point to the right of 1<<16.

	When there is a change of a value, the client can have a procedure called.
	The client calls lprrulerview_SetValueChangeProc(table, proc, rock).
		When there is a change of a value, a call to the proc is made with
		the following parameters:
			valuechangeproc(lprrulerview, rock, iconcode, newvalue)
		where rock is the value passed to SetValueChangeProc,
		iconcode is one of leftIcon, rightIcon, or paraIcon, and
		the new value is as for SetValue and GetValue

*/

#define  lprrulerview_PROGRAMMERVERSION    1

#define lprrulerview_NoValue (-999<<16)

class lprrulerview[lprrulv] : view {
overrides:

	FullUpdate( enum view_UpdateType type, 
		long left, long top, long width, long height );
	Update();
	Hit( enum view_MouseAction action, long x, long y, long n)
			returns struct view *;
	DesiredSize( long width, long height, enum view_DSpass pass, 
				long *desiredWidth, long *desiredHeight ) 
			returns enum view_DSattributes;

methods:

	/* all values are pixel positions expressed as long int's with the binary point at 1<<16 */

	SetValues(/* struct lprrulerview *self, */ long leftmargin, rightmargin, paraindent);
		/* sets the values for the icon positions.  Values -999<<16 and lower indicate
			that no value is to be displayed */
	GetValues(/* struct lprrulerview *self, */ long *leftmargin, *rightmargin, *paraindent);
		/* sets the three parameters to the values of the icon positions */

macromethods:	

	SetValueChangeProc(proc, rock)   /* store a proc to call when a value changes */   \
				(self->ValueChangeRock = (long)rock, \
				self->ValueChangeProc = (void (*)())proc)
classprocedures:

	InitializeClass(/* struct classhdr *ClassID*/) returns boolean;
	InitializeObject(/* struct classhdr *ClassID;*/ struct lprrulerview *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct lprrulerview *self);

data:

	boolean OnScreen;			/* if not view_Removed */
	boolean rulerchanged, iconschanged, textchanged;	/* indicate redraw requirements */
	
	/* Image patterns might differ between instances  on different displays: */
	struct graphic  *BlackPattern, *Grey25Pattern, *WhitePattern;	

	short leftline, rightline, bottomline, topline, middle, textloc;
	short leftzero, rightzero;
	struct TickTbl *TickTbl;

	void (*ValueChangeProc)();		/* called when a value changes */
		/* ValueChangeProc(lprrulerview, ValueChangeRock, iconcode, newvalue) */
	long ValueChangeRock;			/* passed as second arg to ValueChangeProc */

	enum style_Unit unit;

	enum iconcode {leftIcon, rightIcon, paraIcon, noIcon} MovingIcon;
#define LEFT (self->iconloc[(short)leftIcon])
#define RIGHT (self->iconloc[(short)rightIcon])
#define PARA (self->iconloc[(short)paraIcon])

	struct icondata {
		long value;		/* the value set by client */
		char icon;		/* the icon to display */
		short x;			/* the x value corresponding to value */
		short zero;		/* the zero x for ruler segment */
		short left;			/* left edge x of ruler segment */
		short right;		/* right edge x of ruler segment */
		short parkx;		/* the x value of center of parking lot */
		short parkoffset;		/* the offset from parkx to where to plot icon */
		boolean isBlack;		/* TRUE iff there is a value, but it is off the ruler */
		boolean icondirty;		/* TRUE iff needs to be plotted */
		boolean parkdirty;		/* TRUE iff park needs painted */
	} iconloc[3];

	short icony;			/* baseline for icons */
};
