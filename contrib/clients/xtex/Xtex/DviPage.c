/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

#include <X11/Xos.h>  
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#ifdef X11R4
#  include <X11/Xmu/Converters.h>
#else
#  include <X11/Xmu.h>
#endif

#include <stdio.h>
#include "dvi-simple.h"
#include "libtex/conv.h"
#include "mio.h"
#include <assert.h>

#ifdef	DEBUG
#  include "xtex.h"
#endif

#include "DviPageP.h"
  
  static XtResource resources[] = {

#define offset(field) XtOffset(DviPageWidget, field)

    /* {name, class, type, size, offset, default_type, default_addr}, */

    {XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
       offset(dviPage.background), XtRString, "XtDefaultBackground"},

    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
       offset(dviPage.foreground), XtRString, "XtDefaultForeground"},

    {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
       offset(dviPage.font), XtRString, "XtDefaultFont"},

    {XtNpage,  XtCPage, XtRPointer, sizeof(char *),
       offset(dviPage.page), XtRPointer, 0},

    {XtNpai,  XtCPai, XtRPointer, sizeof(struct PostAmbleInfo *),
       offset(dviPage.pai), XtRPointer, 0 },

    {XtNdviFonts,  XtCDviFonts, XtRPointer, sizeof(DviFontInfo *),
       offset(dviPage.dviFonts), XtRPointer, 0 },

    {XtNxFonts,  XtCXFonts, XtRPointer, sizeof(XFontStruct **),
       offset(dviPage.xFonts), XtRPointer, 0 },

    {XtNuserMag,  XtCUserMag, XtRInt, sizeof(int),
       offset(dviPage.userMag), XtRString, "500"},

    {XtNdpiHoriz, XtCDpiHoriz, XtRString, sizeof(char *),
       offset(dviPage.dpiHorizString), XtRString, "300"},

    {XtNdpiVert,  XtCDpiVert, XtRString, sizeof(char *),
       offset(dviPage.dpiVertString), XtRString, "-1"},

    {XtNreverseVideo,  XtCReverseVideo, XtRBool, sizeof(Bool),
       offset(dviPage.reverseVideo), XtRString, "False"},

    {XtNbackingStore, XtCBackingStore, XtRBackingStore, sizeof (int),
    	offset (dviPage.useBackingStore), XtRString, "default"},

    {XtNpaperWidth, XtCPaperWidth, XtRString, sizeof (char *),
    	offset (dviPage.paperWidthString), XtRString, "8.5"},

    {XtNpaperHeight, XtCPaperHeight, XtRString, sizeof (char *),
    	offset (dviPage.paperHeightString), XtRString, "11"},

    {XtNtopOffset, XtCTopOffset, XtRString, sizeof (char *),
    	offset (dviPage.topOffsetString), XtRString, "1"},

    {XtNleftOffset, XtCLeftOffset, XtRString, sizeof (char *),
    	offset (dviPage.leftOffsetString), XtRString, "1"},

    {XtNmaxDrift, XtCMaxDrift, XtRString, sizeof (char *),
    	offset (dviPage.maxDriftString), XtRString, "1"}

#undef offset
  };

#ifdef __STDC__
  static void ClassInitialize();
  static void Initialize(), Destroy(), Redisplay();
  static void Realize();
  static Boolean SetValues( );
  static void SetPageSize( DviPageWidget, Dimension *width, Dimension *heigth);
  static XtGeometryResult QueryGeometry();

  extern double atof(char *);
#else
  static void ClassInitialize();
  static void Initialize(), Destroy(), Redisplay();
  static void Realize();
  static Boolean SetValues();
  static XtGeometryResult QueryGeometry();
  static XtGeometryResult QueryGeometry();

  extern double atof();
#endif

DviPageClassRec dviPageClassRec = {
  { /* core fields */
    /* superclass		*/
    (WidgetClass) &widgetClassRec,
    /* class_name		*/	"DviPage",
    /* widget_size		*/	sizeof(DviPageRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	NULL,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	XtInheritTranslations,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
    },
      { /* dviPage fields */
	/* empty			*/	0
	}
};

WidgetClass dviPageWidgetClass = (WidgetClass)&dviPageClassRec;

/****************************************************************
 *
 * Class Procedures
 *
 ****************************************************************/

/* Sun cc complains about the following line, but I'll be damned if
   I know why. It's decld a static void above */

static void ClassInitialize()
{
  XtAddConverter( XtRString, XtRBackingStore, XmuCvtStringToBackingStore,
		 NULL, 0 );
}

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void
SetPageSize(w, wd, ht)
DviPageWidget w;
Dimension *wd;
Dimension *ht;
{
  DviPagePart *pi = & (w -> dviPage);
  struct PostAmbleInfo *pai = pi -> pai;

  long ppi;	/* pixels per inch */

  double dNumerator = 25400000.0;
  double dDenominator = 473628272.0;
  double dMag;
  extern double	DMagFactor();

  /* compute horizontial and vertical scaling factors - this code is
     largely taken from ../lib/conv.c, but we need to duplicate it
     because we've got two dpi values to deal with */

  if ( pi -> dpiHorizString ) pi -> dpiHoriz = atof( pi -> dpiHorizString );
  if ( pi -> dpiVertString ) pi -> dpiVert = atof( pi -> dpiVertString );

  if ( pi -> dpiVert < 0 ) {
    pi -> dpiVert = pi -> dpiHoriz;
  }

  dMag = DMagFactor( 1000) * DMagFactor(pi -> userMag);

  w -> dviPage.fastFromSpValueHoriz =
    (254000.0 / dNumerator) * (dDenominator / pi -> dpiHoriz)
      * (1.0 / dMag);

  w -> dviPage.fastFromSpValueVert =
    (254000.0 / dNumerator) * (dDenominator / pi -> dpiVert)
      * (1.0 / dMag);
    
  /* compute the screen pixels per inch at this magnification */

  w -> dviPage.pixelsPerInchHoriz = ( (double) pi -> dpiHoriz * dMag );

  w -> dviPage.pixelsPerInchVert = ( (double) pi -> dpiVert * dMag );

  w -> dviPage.pixelsWide
    = w -> dviPage.pixelsPerInchHoriz * w -> dviPage.paperWidth;

  w -> dviPage.pixelsHigh
    = w -> dviPage.pixelsPerInchVert * w -> dviPage.paperHeight;

  (*wd) = w -> dviPage.pixelsWide;
  (*ht) = w -> dviPage.pixelsHigh;
}

static void Initialize (request, new)
    Widget request, new;
{
  DviPageWidget want = (DviPageWidget) request;
  DviPageWidget w = (DviPageWidget) new;

  XFontStruct *f;
  int dir, as, ds;
  XCharStruct os;

  Dimension wd, ht;

  w -> dviPage.page = want -> dviPage.page;
  w -> dviPage.pai  = want -> dviPage.pai ;
  w -> dviPage.xFonts  = want -> dviPage.xFonts ;
  w -> dviPage.dviFonts  = want -> dviPage.dviFonts ;

  w -> dviPage.paperWidth = atof(want -> dviPage.paperWidthString);
  w -> dviPage.paperHeight = atof(want -> dviPage.paperHeightString);

  w -> dviPage.topOffset = atof(want -> dviPage.topOffsetString);
  w -> dviPage.leftOffset = atof(want -> dviPage.leftOffsetString);

  w -> dviPage.globalGC  = 0;

  w -> dviPage.dviStack  = 0;
  w -> dviPage.dviStackSize  = 0;
  w -> dviPage.pathLen  = 0;
  w -> dviPage.penSize = 1;
  w -> dviPage.whiten = False;
  w -> dviPage.shade = False;
  w -> dviPage.blacken = False;

  SetPageSize(w, &wd, &ht);

#define MaX(x,y) ((x) > (y) ) ? (x) : (y)

  w -> core.width = MaX(wd, want->core.width);
  w -> core.height = MaX(ht, want->core.height);

#undef MaX

}

static void Realize (gw, valueMask, attrs)
     Widget gw;
     XtValueMask *valueMask;
     XSetWindowAttributes *attrs;
{

  XWindowAttributes wAttributes;

  DviPageWidget	w = (DviPageWidget) gw;
  
  switch (w->dviPage.useBackingStore) {
  case Always:
  case NotUseful:
  case WhenMapped:
    *valueMask |=CWBackingStore;
    attrs->backing_store = w->dviPage.useBackingStore;
    break;
  }

  XtCreateWindow( w, InputOutput, (Visual *)CopyFromParent,
		 *valueMask, attrs);

  XGetWindowAttributes( XtDisplay(w), XtWindow(w), &wAttributes );

  
  w -> dviPage.haveBackingStore
    = ( wAttributes.save_under == Always)
      || (wAttributes.save_under == WhenMapped);

  w->dviPage.globalGC
    = XCreateGC( XtDisplay(w), XtWindow(w), 0, NULL);
}

static void Destroy (gw)
     Widget gw;
{
  DviPageWidget w = (DviPageWidget) gw;
  XtFree( w -> dviPage.dviStack );
  w -> dviPage.dviStack = 0;

  if ( w -> dviPage.globalGC ) {
    XFreeGC( XtDisplay(w), w -> dviPage.globalGC );
    w -> dviPage.globalGC = 0;
  }
}

static Boolean SetValues(current, request, new)
    DviPageWidget current, request, new;
{

  DviPagePart *pi = & (request -> dviPage);
  struct PostAmbleInfo *p = pi -> pai;

  new -> dviPage.page =
    (request -> dviPage.page)
      ? (request -> dviPage.page) : current -> dviPage.page ;

  new -> dviPage.xFonts = 
    (request -> dviPage.xFonts)
      ? (request -> dviPage.xFonts) : current -> dviPage.xFonts;;

  new -> dviPage.pai =
    (request -> dviPage.pai)
      ? (request -> dviPage.pai) : current -> dviPage.pai;

  if ( new -> dviPage.pai ) {

    int stackSize = new -> dviPage.pai -> pai_DVIStackSize;

    if (stackSize > new -> dviPage.dviStackSize ) {

      stackSize += 2;
      
      XtFree( new -> dviPage.dviStack );
      
      new -> dviPage.dviStack  = 0;
      new -> dviPage.dviStackSize = stackSize;
      
      if (stackSize > 0 ) {
	new -> dviPage.dviStack  = 
	  (LocalDviStack *) (malloc( (stackSize+1) * sizeof(LocalDviStack) ));
      }
    }
  }

  new -> dviPage.dpiHorizString =
    (request -> dviPage.dpiHorizString)
      ? (request -> dviPage.dpiHorizString) :  current -> dviPage.dpiHorizString;

  new -> dviPage.dpiVertString =
    (request -> dviPage.dpiVertString)
      ? (request -> dviPage.dpiVertString) :  current -> dviPage.dpiVertString;

  new -> dviPage.dviFonts =
    (request -> dviPage.dviFonts)
      ? (request -> dviPage.dviFonts) :  current -> dviPage.dviFonts;

  new -> dviPage.paperWidth = 
    (request -> dviPage.paperWidthString)
      ? atof(request -> dviPage.paperWidthString)
	: current -> dviPage.paperWidth;

  new -> dviPage.paperHeight = 
    (request -> dviPage.paperHeightString)
      ? atof(request -> dviPage.paperHeightString)
	: current -> dviPage.paperHeight;

  new -> dviPage.topOffset = 
    (request -> dviPage.topOffsetString)
      ? atof(request -> dviPage.topOffsetString)
	: current -> dviPage.topOffset;

  new -> dviPage.leftOffset = 
    (request -> dviPage.leftOffsetString)
      ? atof(request -> dviPage.leftOffsetString)
	: current -> dviPage.leftOffset;

  SetPageSize(new, &(new -> core.width), &(new -> core.height));

  return( True );	 /* resize */
}

static XtGeometryResult QueryGeometry(w, intended, preferred)
    Widget w;
    XtWidgetGeometry *intended, *preferred;
{
    register DviPageWidget lw = (DviPageWidget) w;
    Dimension wd, ht;

    SetPageSize((DviPageWidget) w, &wd, &ht);

    preferred -> request_mode = CWWidth | CWHeight;

    preferred -> width = wd ;
    preferred -> height =  ht ;

    if (  ((intended->request_mode & (CWWidth | CWHeight))
	   	== (CWWidth | CWHeight)) &&
	  intended->width == preferred->width &&
	  intended->height == preferred->height)
	return XtGeometryYes;
    else if (preferred->width == w->core.width &&
	     preferred->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
}

/*
 *	Some very DVI related things
 */

#define	ABS(X)	( (X) < 0 ? -(X) : (X) )

static int selectFont(w, ptr, index, font)
DviPageWidget w;
DviFontInfo **ptr;
int *index;
int font;
{
  if ( *ptr != 0 && (*ptr) -> paf.paf_DVIFontIndex == font ) {
    return 0;
  }
  else {
    int offset;
    DviFontInfo *fonts = w -> dviPage.dviFonts;

    *ptr = 0;
    for ( offset = 0; offset < RegisteredFonts; offset++ ) {
      if ( fonts[offset].paf.paf_DVIFontIndex == font ) {
	*ptr = &(fonts[offset]);
	*index = offset;
	break;
      }
    }
    
    if ( *ptr == 0 ) {
      error(0,0,"Can't find font %d", font);
      return 1;
    }
    else {
      return(0);
    }
  }
}

typedef struct {
  int spDrift;
  long spPerDot;
  DviPagePart *page; 
  DviFontInfo *dviCurrentFont;
  int dviCurrentFontIndex;
  XFontStruct *currentFontStruct;

  /* following are used to reduce the number of XDrawStrings we do */

#define MAX_DVI_STRING 64
  char dviString[ MAX_DVI_STRING ];
  int dviStringLength;

#ifdef STATS
  int longestString;
#endif

  int startHH, startVV;
  int endHH, endVV;

  LocalDviStack *dviStackP;	/* current stack item */

} DviDisplayInfo;

static void DviDisplayFlush(widget, info)
DviPageWidget widget;
DviDisplayInfo *info;
{
#ifdef DEBUG
  if (Debug) {
    fprintf(stderr,"Flush string of %d bytes at %d,%d\n",
	    info -> dviStringLength ,  info -> startHH ,  info -> startVV );
  }
#endif
  
  if (  info -> dviStringLength  > 0 ) {

    i32 atHH = fastFromSpHoriz(widget,  info -> startHH );
    i32 atVV = fastFromSpVert(widget,  info -> startVV );
    Bool doit = True;

#ifdef STATS
    if (info -> dviStringLength > info -> longestString) {
      info -> longestString = info -> dviStringLength;
    }
#endif
    
    if ( widget -> dviPage.haveBackingStore ) {
      i32 width = fastFromSpHoriz(widget,  info -> endHH );
      i32 height = fastFromSpVert(widget,  info -> endVV );
      
      doit =
	XRectInRegion( widget -> dviPage.updateRegion, atHH, atVV,
		      width, height) != RectangleOut;
    }
    
    if (doit) {
      XDrawString( XtDisplay( widget ), XtWindow( widget ),
		  info -> page  -> globalGC, atHH, atVV,
		  info -> dviString ,  info -> dviStringLength  );
    }
  }

  info -> dviStringLength = 0;
  info -> startHH = info -> dviStackP -> h;
  info -> startVV = info -> dviStackP -> v ;
  info -> endHH = info -> startHH ;
}

static void DviDisplay(widget)
     DviPageWidget widget;
{
  int c;
  i32 p;
  int advance;
  int wuz;
  DviDisplayInfo info;
  double spDriftTmp;
  
  struct PostAmbleInfo *pai;
  char *thisPage;
  LocalDviStack *dviStack;
  
  /* initialize info structure */
  
  info.page = &( widget -> dviPage); 
  info.dviCurrentFont = 0;
  info.dviCurrentFontIndex = 0;
  info.currentFontStruct = widget -> dviPage.font;
  info.dviStringLength = 0;
  info.startHH = info.startVV = -1;
  info.endHH = info.endVV = -1;
#ifdef STATS
  info.longestString = 0;
#endif
  
  /* compute  info.spDrift  -- the max. *points* we'll drift from the
     true location */
  
  spDriftTmp = atof( widget -> dviPage.maxDriftString );
  spDriftTmp  /= POINTS_PER_INCH;	/* now in inches */
  info.spDrift = INCH_TO_SP(  spDriftTmp  );
  
  /* compute  spPerDot  -- the number of Sp per actual *dot* on the display */

  info.spPerDot = INCH_TO_SP(1.0);
  info.spPerDot  /* now per dot */
    /=  info.page  -> dpiHoriz;

#ifdef STATS
  fprintf(stderr,"maxDriftString	= %s\n",
	  widget->dviPage.maxDriftString);
  fprintf(stderr,"spPerDot		= %f\n", info.spPerDot);
  fprintf(stderr,"maxDrift		= %d\n", info.spDrift);
#endif
  
  /* initialize things dependent on info structure */
  
  pai = info.page  -> pai;
  thisPage = info.page->page ;
  dviStack = info.page  -> dviStack;
  
  if ( pai == 0 || thisPage == 0 || dviStack == 0) return;
  
  assert( dviStack != 0 );
  info.dviStackP = dviStack;

  /* insure that first byte is a BOP mark */
  
  c = (*thisPage) & 0xff;
  if ( DVI_DT(c) != DT_BOP ) {
    error(0,0,"No BOP found");
    goto cleanup;
  }
  
  for(;;) {
    
    c = mGetByte(&thisPage);

    if (DVI_IsChar(c)) {
      
#ifdef DEBUG
      if (Debug) {
	fprintf(stderr,"IS_CHAR SET character %d (advance)\n", c);
      }
#endif
      
      advance = 1;
      
    doDviChar:
#ifdef DEBUG
      if (Debug) {
	fprintf(stderr,"Flush decision:\n");
	fprintf(stderr,"%-20s=%d\n", "advance", advance);
	fprintf(stderr,"%-20s=%d\n", "ABS( info.endHH -etc)",
		ABS( info.endHH  - ( info.dviStackP  -> h)));
	fprintf(stderr,"%-20s=%d\n", " info.spDrift ",
		info.spDrift );
	fprintf(stderr,"%-20s=%d\n", " info.endVV ",
		info.endVV );
	fprintf(stderr,"%-20s=%d\n", " info.dviStackP ->v",
		info.dviStackP  -> v);
	fprintf(stderr,"%-20s=%d\n", " info.dviStackP ->h",
		info.dviStackP  -> h);
      }
#endif
      if ( !advance
	  || ABS( info.endHH  - ( info.dviStackP  -> h)) >  info.spDrift 
	  ||  info.endVV  != ( info.dviStackP  -> v)
	  ||  info.dviStringLength  >= MAX_DVI_STRING )
	{
	  DviDisplayFlush(widget, &info);
	}
      
      if (info.dviStringLength == 0 ) {
	info.startHH = info.dviStackP -> h;
	info.startVV = info.dviStackP -> v ;
	info.endHH = info.startHH ;
      }
      info.dviString [  info.dviStringLength ++ ] = c; 
      
      if ( advance ) {
#ifdef DEBUG
	if (Debug) {
	  fprintf(stderr,"do advance\n");
	}
#endif
	
	if ( info.dviCurrentFont  &&  info.dviCurrentFont  -> widths) { 
	  info.dviStackP  -> h += ( info.dviCurrentFont  -> widths[ c ]) ;
	}
	
	/*
	 *	The following code makes use of the FontStruct information;
	 *	each time we cache a glyph, we update the stack to point to
	 *	the location where we should be. endHH, on the other hand,
	 *	is the *actual* location, based on the width of the displayed
	 *	glyph.
	 */
	{
	  int offset = c - ( info.currentFontStruct  -> min_char_or_byte2);
	  int thisWidth;
	  if (  info.currentFontStruct  -> per_char ) {
	    XCharStruct *cs = &( info.currentFontStruct  -> per_char[c]);
	    thisWidth = cs -> width;
	  }
	  else {
	    thisWidth = info.currentFontStruct  -> max_bounds.width;
	  }
	  info.endHH  += (thisWidth *  info.spPerDot );
	}
      }
      else {
	info.endHH = info.dviStackP  -> h;
      }
      info.endVV = info.dviStackP  -> v;
      
    } else {
      
      switch (DVI_OpLen(c)) {
	
      case DPL_NONE:
	break;
	
      case DPL_SGN1:
	p = mGetByte(&thisPage);
	p = Sign8(p);
	break;
	
      case DPL_SGN2:
	mGetWord(&thisPage, &p);
	p = Sign16(p);
	break;
	
      case DPL_SGN3:
	mGet3Byte(&thisPage, &p);
	p = Sign24(p);
	break;
	
      case DPL_SGN4:
	mGetLong(&thisPage, &p);
	break;
	
      case DPL_UNS1:
	p = UnSign8(mGetByte(&thisPage));
	break;
	
      case DPL_UNS2:
	mGetWord(&thisPage, &p);
	p = UnSign16(p);
	break;
	
      case DPL_UNS3:
	mGet3Byte(&thisPage, &p);
	p = UnSign24(p);
	break;
	
      default:
	panic("DVI_OpLen(%d) = %d", c, DVI_OpLen(c));
	goto cleanup;
	/* NOTREACHED */
      }
      
      switch (DVI_DT(c)) {
	
      case DT_SET:
	c = p;
	advance = 1;
#ifdef DEBUG
	if (Debug) {
	  fprintf(stderr,"SET character %d (advance)\n", c);
	}
#endif
	goto doDviChar;
	break;
	
      case DT_PUT:
	c = p;
	advance = 0;
#ifdef DEBUG
	if (Debug) {
	  fprintf(stderr,"PUT character %d (no advance)\n", c);
	}
#endif
	goto doDviChar;
	break;;
	
      case DT_SETRULE:
      case DT_PUTRULE:
	DviDisplayFlush(widget, &info);
	{
	  i32 cHH, cVV;
	  double dhh, dvv;
	  i32 rhh, rvv;
	  
	  cHH = fastFromSpHoriz(widget,  info.dviStackP  -> h);
	  cVV = fastFromSpVert(widget,  info.dviStackP  -> v);
	  
	  mGetLong(&thisPage, &rvv);
	  mGetLong(&thisPage, &rhh);
	  
	  if (DVI_DT(c) == DT_SETRULE) {
	    info.dviStackP  -> h += rhh;
	  }
	  
	  dhh = rhh;
	  dvv = rvv;
	  
	  rhh = CEIL( fastFromSpHoriz(widget, dhh) );
	  rvv = CEIL( fastFromSpVert(widget, dvv) );
	  
	  XFillRectangle(XtDisplay(widget), XtWindow(widget),
			 info.page  -> globalGC, cHH, cVV - rvv, rhh, rvv);
	  
	}
	break;
	
      case DT_NOP:
	break;
	
      case DT_BOP:
	/*
	 *	Each beginning of  info.page  has 11 4-byte words telling us things
	 *	about the  info.page. We ignore them.
	 */
	thisPage += (11 * 4);
	
	info.dviStackP = dviStack;
	info.dviStackP  -> h = INCH_TO_SP( (double)  info.page  -> leftOffset);
	info.dviStackP  -> v = INCH_TO_SP( (double)  info.page  -> topOffset);
	info.startHH = info.dviStackP  -> h;
	info.startVV = info.dviStackP  -> v ;
	info.endHH = info.startHH ;
	info.endVV = info.startVV ;
	info.dviStackP  -> w = 0;
	info.dviStackP  -> x = 0;
	info.dviStackP  -> y = 0;
	info.dviStackP  -> z = 0;
	
	break;
	
      case DT_EOP:
	goto cleanup;
	/* NOTREACHED */
	
      case DT_PUSH:
	DviDisplayFlush(widget, &info);
	{
	  /* copy the old stack to the new one */
	  
	  LocalDviStack *old = info.dviStackP ;
	  info.dviStackP ++;
	  * info.dviStackP = *old;
	}
	break;
	
      case DT_POP:
	/* just use the old stack */
	DviDisplayFlush(widget, &info);
	info.dviStackP --;
	break;
	
      case DT_W0:	/* there should be a way to make these pretty */
	p = info.dviStackP  -> w;
	goto move_right;
	
      case DT_W:
	info.dviStackP  -> w = p;
	goto move_right;
	
      case DT_X0:
	p = info.dviStackP  -> x;
	goto move_right;
	
      case DT_X:
	info.dviStackP  -> x = p;
	goto move_right;
	
      case DT_RIGHT:
      move_right:
	DviDisplayFlush(widget, &info);
#ifdef DEBUG
	if (Debug) {
	  fprintf(stderr,"RIGHT %d\n", p);
	}
#endif
	info.dviStackP  -> h += p;
	break;
	
      case DT_Y0:
	p = info.dviStackP  -> y;
	goto move_down;
	
      case DT_Y:
	info.dviStackP  -> y = p;
	goto move_down;
	
      case DT_Z0:
	p = info.dviStackP  -> z;
	goto move_down;
	
      case DT_Z:
	info.dviStackP  -> z = p;
	goto move_down;
	
      case DT_DOWN:
      move_down:
	DviDisplayFlush(widget, &info);
#ifdef DEBUG
	if (Debug) {
	  fprintf(stderr,"DOWN %d\n", p);
	}
#endif
	info.dviStackP  -> v += p;
	break;
	
      case DT_FNTNUM:
      case DT_FNT:
	{
	  int font;
	  
	  /* flush existing text */
	  
	  DviDisplayFlush(widget, &info);
	  
	  /* switch to new font */
	  
	  font = ( DVI_DT(c) == DT_FNTNUM )
	    ? font = (c - DVI_FNTNUM0) : p;
	  
	  if ( selectFont(widget,
			  & info.dviCurrentFont , & info.dviCurrentFontIndex , font) ) {
	    goto cleanup;
	  }
	  else {
	    /* load the font */
	    int useFont =
	      (widget -> dviPage.xFonts
	       && widget -> dviPage.xFonts[  info.dviCurrentFontIndex  ]);
	    
	    Font font ;
	    
	    info.currentFontStruct = 
	      (useFont)
		? (widget->dviPage.xFonts[  info.dviCurrentFontIndex  ])
		  : widget -> dviPage.font ;
	    
	    font = info.currentFontStruct  -> fid;
	    
	    XSetFont( XtDisplay(widget), widget -> dviPage.globalGC, font );
	  }
	}
	break;
	
      case DT_XXX:
	{
	  char specialBuffer [2048];
	  register char *cp;
	  int sweetp = 0;
	  
	  DviDisplayFlush(widget, &info);
	  if (p > 2047) {
	    sweetp = p - 2047;
	    p = 2047;
	  }
	  
	  for (cp = specialBuffer ; p > 0; p--) {
	    *cp = mGetByte(&thisPage);
	    cp++;
	  }
	  *(cp) = 0;
	  
	  while(sweetp > 0) {
	    mGetByte(&thisPage);
	  }
	  
	  widget -> dviPage.dviStackPointer = info.dviStackP ;
	  doSpecial(widget, specialBuffer);
	}
	break;
	
      case DT_FNTDEF:
	{
	  int i;
	  long j;
	  mGetLong(&thisPage, &j);
	  mGetLong(&thisPage, &j);
	  mGetLong(&thisPage, &j);
	  i = UnSign8(mGetByte(&thisPage));
	  i += UnSign8(mGetByte(&thisPage));
	  while (--i >= 0) {
	    (void) mGetByte(&thisPage);
	  }
	}
	break;
	
      case DT_PRE:
	GripeUnexpectedOp("PRE");
	goto cleanup;
	/* NOTREACHED */
	
      case DT_POST:
	GripeUnexpectedOp("POST");
	goto cleanup;
	/* NOTREACHED */
	
      case DT_POSTPOST:
	GripeUnexpectedOp("POSTPOST");
	goto cleanup;
	/* NOTREACHED */
	
      case DT_UNDEF:
	GripeUndefinedOp(c);
	goto cleanup;
	/* NOTREACHED */
	
      default:
	panic("DVI_DT(%d) = %d", c, DVI_DT(c));
	goto cleanup;
	/* NOTREACHED */
      }
    }
  }
  
 cleanup:
  /* clean up laststring if there was one */
  DviDisplayFlush(widget, &info);

#ifdef STATS
  fprintf(stderr,"Longest string was %d\n", info.longestString);
#endif

  return;
}


/* ARGSUSED */
static void Redisplay (gw, event, region)
    Widget gw;
    XEvent *event;		/* unused */
    Region region;		/* unused */
{
  DviPageWidget w = (DviPageWidget) gw;

  if (XtIsRealized(w)) {

    XGCValues	values;
    unsigned long fore = w->dviPage.foreground;
    unsigned long aft = w->dviPage.background;
    
    if ( w -> dviPage.reverseVideo ) {
      unsigned long tmp = aft;
      aft = fore;
      fore = tmp;
    }
    
    /* Prepare to clear the main screen */
    
    values.function	= GXcopy;
    values.foreground	= aft;
    values.background	= fore;
    
    XChangeGC( XtDisplay(gw), w -> dviPage.globalGC,
	      (unsigned) GCFunction | GCForeground | GCBackground,
	      &values);

    XSetRegion( XtDisplay(w), w -> dviPage.globalGC, region );

    XFillRectangle(XtDisplay(w), XtWindow(w),
		   w -> dviPage.globalGC,
		   0, 0,
		   w -> core.width, w -> core.height);

    /* Prepare to paint */
    
    values.foreground	= fore;
    values.background	= aft;
    values.plane_mask	= AllPlanes;
    values.font		= w->dviPage.font->fid;
    
    if ( aft ) {
      values.function = GXand;
    }
    else {
      values.function = GXor;
    }
    
    XChangeGC( XtDisplay(gw), w -> dviPage.globalGC,
	      (unsigned) GCFunction | GCPlaneMask |
	      GCForeground | GCBackground | GCFont,
	      &values);

    XSetRegion( XtDisplay(w), w -> dviPage.globalGC, region );

    if (w -> dviPage.pai && w -> dviPage.page  && w -> dviPage.dviFonts ) {

      w -> dviPage.updateRegion = region;

      DviDisplay(w);

    }
  }
}
