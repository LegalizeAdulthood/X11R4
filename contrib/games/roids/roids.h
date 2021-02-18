/*
 * Copyright 1989 Digital Equipment Corporation
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  Digital Equipment Corporation makes no representations
 * about the suitability of this software for any purpose.  It is
 * provided "as is" without express or implied warranty.
 *
 * DIGITAL EQUIPMENT CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Terry Weissman
 *          weissman@decwrl.dec.com
 */

/* roids.h - definitions of roids data structures. */

#ifndef _roids_h
#define _roids_h

#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>

#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#define XtRFloat "Float"
#define rint(x) ((int) (x))
#define MIN(x, y)	((x) < (y) ? (x) : (y))
#define MAX(x, y)	((x) > (y) ? (x) : (y))
#define NPOINTS 3		/* Number of points on a ship. */

#ifndef M_PI
#define	M_PI	3.14159265358979323846
#define	M_PI_2	1.57079632679489661923
#define	M_PI_4	0.78539816339744830962
#endif

#ifdef MAIN
#define ext
#else
#define ext extern
#endif

/*
 * Definitions to make us act as a widget.
 */

/* New fields for the Roids widget class record */
typedef struct {
     int mumble;   /* No new procedures */
} RoidsClassPart;

/* Full class record declaration */
typedef struct _RoidsClassRec {
    CoreClassPart       core_class;
    RoidsClassPart    roids_class;
} RoidsClassRec;

extern RoidsClassRec roidsClassRec;
extern WidgetClass roidsWidgetClass;

/* New fields for the Roids widget record */
typedef struct _RoidsPart {
    int dummy;
} RoidsPart;



/* Full instance record declaration */

typedef struct _RoidsRec {
    CorePart core;
    RoidsPart roids;
} RoidsRec, *RoidsWidget;




/*
 * Actual roids definitions.
 */

ext Boolean debug;

ext Display *dpy;
ext Window gamewindow;
ext RoidsWidget gamewidget;
ext int gamewidth, gameheight;
ext Widget toplevel, scorewidget;
ext int score;
ext int shipsleft;

/* Ship info: */

ext int shipradius;
ext int numdir;

ext int shipwait;		/* Number of milleseconds to wait between */
				/* moving ship. */
ext int numdestroyed;



ext XtIntervalId shiptimerid;

ext Pixel shippixel;
ext float maxv;			/* Constant: what our maximum velocity is. */
ext float accper;		/* Constant: how fast we accelerate. */
ext Boolean shipdestroyed;	/* TRUE if the ship is non-existant */

/* Rock info: */

ext XtIntervalId rocktimerid;
ext Pixel rockpixel;
ext int rockwait;		/* Number of milleseconds to wait between */
				/* moving rocks. */
ext float maxrockspeed;		/* Constant: fastest velocity for a rock. */

/* Shot info. */

ext XtIntervalId shottimerid;
ext Pixel shotpixel;
ext int shotwait;
ext float shotacc;		/* How many times faster a shot goes than */
				/* the ship.*/
ext int maxshots;		/* How many shots are allowed to exist at */
				/* once. */
ext int shotduration;		/* How long (in milleseconds) a shot lives. */




ext GC backgc, shipgc, rockgc, shotgc;



/* From widget.c */

extern Boolean TakeFocus();
extern void Quit();



/* From ship.c */

extern void MoveShip();
extern void ThrustOn();
extern void ThrustOff();
extern void RotateLeft();
extern void RotateRight();
extern void RotateOff();
extern void RotateToPoint();
extern void StopRotateToPoint();
extern void RotateMouseMoved();
extern void Fire();

/* From rocks.c */

extern Boolean CheckIfShipHitRocks();
extern Boolean AreaForShipIsClear();
extern Boolean LineHitsRock();
extern void MoveRocks();
extern void InitRocks();

/* From shot.c */

extern void AddShot();
extern void MoveShots();

#endif _roids_h
