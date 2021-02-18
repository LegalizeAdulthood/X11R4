/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xfontd.c,v 1.9 89/08/27 19:46:11 zs01 Exp $ */
/* $ACIS:fontdesc.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xfontd.c,v $ */

#ifndef lint
	char *xfontd_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xfontd.c,v 1.9 89/08/27 19:46:11 zs01 Exp $";
#endif /* lint */

/* fontdesc.c
 */

#include <sys/param.h> /* For MAXPATHLEN. */

#include <class.h>
#include <andrewos.h>
#include <ctype.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <cmenu.h>

#include <xgraphic.ih>
#include <fontdesc.ih>
#include <im.ih>
#include <xim.ih>
#include <xfontd.eh>
#include <environ.ih>

/* Filled in in InitializeClass from a user preference.
 * If TRUE, font substitutions result in a warning being printed on stderr.
 */
static boolean announceSubstitutions;

#define ENSUREGRAPHIC(retval)  \
    {if (graphic == NULL || ! xgraphic_Valid(graphic)) {graphic = GetAnyOldGraphic();  \
    	if (graphic == NULL || ! xgraphic_Valid(graphic)) return retval;}}

#define GETXFONT(self, graphic)  \
	((XFontStruct *)xfontdesc_GetRealFontDesc(self, graphic))
#define MDFD ((struct fcache *)(((struct fontdesc *)self)->MachineDependentFontDescriptor))

/* the MachineDependentFontDescriptor field of the fontdesc points to a list of fcache values
	GetRealFontDescriptor searches this list.  
	Unless there are connections to multiple servers, the list will have only one element.

	The host field is the file descriptor (fd in the Display) because
	    X guarantees that fonts are uniquely defined for the server (not just the Display).
	{{So why is the argument to XFreeFont a Display rater than a server?}}
*/
struct fcache {
	Display *dpy;		/* first display the font was opened on (for XFreeFont) */
	unsigned long host;		/* the host file descriptor */
	XFontStruct *font;		/* the value for wm_SelectFont() */
	struct fcache *next;
};




/* ************** auxiliary static procedures ******** */

/* This procedure creates the name of the andrew font file
given a particular set of characteristics */

static struct FontSummary *GetFontSummary(self)
    struct xfontdesc *self;
{

     register struct FontSummary *tsp;
     register XCharStruct *maxChar;
     register XFontStruct *fontInfo;
     register int i;

     tsp = &self->header.fontdesc.summary;	/* for quick reference */
     /* otherwise DescValid is set to true in LoadAndrewFont   
		-- (I think this comment is meaningless -wjh) */
     bzero(tsp, sizeof (struct FontSummary));

     if (MDFD == NULL || MDFD->font == NULL) return tsp;
     fontInfo = MDFD->font;

     maxChar = &fontInfo->max_bounds;

     /* Read the font information from the max character in the font
	information structure */
     /* Not sure about the definition of first two statements */
     tsp->maxWidth = maxChar->rbearing - maxChar->lbearing; /*LR*/
     tsp->maxSpacing = maxChar->width; /* note different terminology */

     tsp->maxHeight = maxChar->ascent + maxChar->descent;
     tsp->newlineHeight = fontInfo->ascent + fontInfo->descent;
     if (tsp->newlineHeight == 0)
         tsp->newlineHeight = tsp->maxHeight;
     tsp->maxBelow = maxChar->descent;
     tsp->maxLeft = -(maxChar->lbearing);
     for(i=0;i<fontdesc_NumIcons;i++) {
	if (fontInfo->min_byte1 || fontInfo->max_byte1) 
	    xfontdesc_SetCharValid(self, i);
	else if (i >= fontInfo->min_char_or_byte2 &&
			i <= fontInfo->max_char_or_byte2)  
	    xfontdesc_SetCharValid(self, i);
     }
     return tsp;

}

static AddStyleModifiers(string, styles)
    char *string;
    int styles;
{

    char *oldString = string;

    if (styles & fontdesc_Bold)
        *string++ = 'b';
    if (styles & fontdesc_Italic)
        *string++ = 'i';
    if (styles & fontdesc_Fixed)
        *string++ = 'f';
    if (styles & fontdesc_Shadow)
        *string++ = 's';

    if (oldString != string) /* Added something. */
        *string++ = '\000'; /* Add terminating NULL. */
}

struct bestfont {
    int index;
    int size;
    int style;
    int cost;
};

/* Number of good close fonts to try */
/* Maximum number of tries at loading fonts. This is done to either handle
 * a race condition in which the font path is reset between the XListFonts
 * call and the XLoadQueryFont call or if the server can not properly load
 * the font (a file server is down, or it has found a bad font)
 */

#define MAXBEST 3

/* This routine trys to find the closest match to the desired size and style in
 * a list of names. (It assumes that the family name of all names in the list
 * is contains the desired family name as an initial substr.) The match is
 * done as follows:
 *
 *     First the font size is matched. This takes priority over the style mask.
 *
 *     Second the closest style is chosen. Closest here is defined as the least
 *     Hamming distance between the two style masks.
 *
 *     If the above come out equal, the larger font is favored so that any
 *     bounding boxes calculated from the font size will be bigger as opposed to smaller.
 *
 * This routine depends on fontdesc_ExplodeFont name to the names in the list
 * into their size and style.
 */
static struct bestfont *ClosestFonts(possibleNames, numNames, desiredFamily, desiredSize, desiredStyle, numBest)
    char **possibleNames;
    int numNames;
    char *desiredFamily;
    int desiredSize;
    int desiredStyle;
    int *numBest;
{

    int index;
    int style;
    int size;
    static struct bestfont bestFonts[MAXBEST + 1];
    int familyLength = strlen(desiredFamily);
    char familyName[MAXPATHLEN];

/* These are two sentinels. The first is used to provide a bad font that will always get replaced. The second is used to provide a bad font so the loop searching for a bad font will always find one. After the first MAXBEST fonts are filled in, the first sentinel is gone. */
    bestFonts[0].cost = 1000000;
    bestFonts[MAXBEST].cost = 1000000;

    for (index = 0; index < numNames; ++index) {

        struct bestfont *bestFont;
        int cost;
        static char numBitsSet[16] = {0, 1, 1, 2, 1, 2, 2, 2, 1, 2, 2,
                                      2, 2, 2, 2, 2};

        fontdesc_ExplodeFontName(possibleNames[index], familyName, sizeof(familyName), &style, &size);

/* Verify that the family name is the same. We know that the initial
 * substrings are the same since the names from possible names all match the
 * regular expression <family>*
 */
        if (familyName[familyLength] != '\0')
            continue;

        cost = (abs(size - desiredSize) * 4) + numBitsSet[((desiredStyle ^ style) & 0x0f)];

/* Insert it in the array at the appropriate place if its cost within the
 * MAXBEST smallest costs we have seen.
 */

        for (bestFont = bestFonts; cost > bestFont->cost || (cost == bestFont->cost && size < bestFont->size); ++bestFont)
            ;

        if (bestFont != bestFonts + MAXBEST) {

            struct bestfont *tempFont;

            for (tempFont = bestFonts + MAXBEST - 2; tempFont >= bestFont; --tempFont) {
                tempFont[1] = tempFont[0];
            }

            bestFont->index = index;
            bestFont->size = size;
            bestFont->style = style;
            bestFont->cost = cost;
        }
    }

    *numBest = (numNames > MAXBEST) ? MAXBEST : numNames;
    return bestFonts;
}

/* Maximum number of font names to ask the server for to do a closest match
 * against.
 */

#define MAXNAMES 50

/* This routine works by constructing the Andrew fontname for the requested
 * font. If this fails, it then asks there server for all the font names that
 * begin with the family name of the requested font. It then runs a closest
 * match against the returned names. If the resulting names cannot be loaded,
 * the process is repeated. If this fails three times, the "fixed" or
 * "variable" font is requested. If this fails, the server is asked for any font
 * name and that is loaded. If that fails, the routine gives up.
 */
	static XFontStruct *
xfontdesc_LoadXFont(self, graphic)
	struct xfontdesc * self;
	struct xgraphic *graphic;
{

    XFontStruct *font = NULL;
    int desiredSize = self->header.fontdesc.FontSize;
    int desiredStyle = self->header.fontdesc.FontStyles;
    /* Need bounds checking. */
    char fontName[MAXPATHLEN];
    char *familyEnd;
    char **fontNames = NULL;
    char *substitute = NULL;
    struct fcache *oldMDFD = MDFD;

/* Try andrew naming convention. */
    strcpy(fontName, self->header.fontdesc.FontName->name);
    familyEnd = fontName + strlen(fontName);
    if (desiredSize != 0)
        sprintf(familyEnd, "%d", desiredSize);
    AddStyleModifiers(familyEnd + strlen(familyEnd), desiredStyle);
    font = XLoadQueryFont(xgraphic_XDisplay(graphic), fontName);

    if (font == NULL) {

        struct bestfont *bestFonts;
        int numNames;
        int numBest;
        char tempChars[2]; /* Used to restore the full font name... */

        tempChars[0] = familyEnd[0];
        tempChars[1] = familyEnd[1];
        familyEnd[0] = '*';
        familyEnd[1] = '\0';
        fontNames = XListFonts(xgraphic_XDisplay(graphic), fontName, MAXNAMES, &numNames);

        if (numNames > 0) {
            *familyEnd = '\0';
            bestFonts = ClosestFonts(fontNames, numNames, fontName, desiredSize, desiredStyle, &numBest);

            while (numBest > 0 && font == NULL) {
                font = XLoadQueryFont(xgraphic_XDisplay(graphic), fontNames[bestFonts->index]);
                if (bestFonts->size != desiredSize || bestFonts->style != desiredStyle)
                    substitute = fontNames[bestFonts->index];
                --numBest;
                ++bestFonts;
            }
        }
        familyEnd[0] = tempChars[0];
        familyEnd[1] = tempChars[1];
    }

/* If above failed, try the two "guaranteed" X fonts. */
    if (font == NULL)
        if (desiredStyle & fontdesc_Fixed)
            font = XLoadQueryFont(xgraphic_XDisplay(graphic), substitute = "fixed");
        else
            font = XLoadQueryFont(xgraphic_XDisplay(graphic), substitute = "variable");

/* Out of desperation, ask the server for anything. */
    if (font == NULL) {

        int numNames;

        fontNames = XListFonts(xgraphic_XDisplay(graphic), "*", 1, &numNames);
        if (numNames != 0)
            font = XLoadQueryFont(xgraphic_XDisplay(graphic), substitute = fontNames[0]);
    }

/* add to cache of defined fonts, one for each server 
	we can assume that this server is not in the list 
	because otherwise LoadFont is not called 
 */
    {
	struct fcache **fcp, *fc;
	/* find pointer to NULL */
	for (		fcp = (struct fcache **)
				(&self->header.fontdesc.MachineDependentFontDescriptor); 
			*fcp != NULL;
			fcp = &((*fcp)->next))
		{}
	/* now fcp points to a pointer to NULL 
		build new list elt in fc */
	fc = (struct fcache *)malloc(sizeof(struct fcache));
	fc->next = NULL;
	fc->dpy = xgraphic_XDisplay(graphic);
	fc->host = fc->dpy->fd;
	fc->font = font;
	*fcp = fc;	/* link the new one into the list */	
    }
    self->header.fontdesc.DescValid = (MDFD != NULL);

    if (substitute != NULL && announceSubstitutions && font != NULL)
        fprintf(stderr, "%s: Substituting font %s for %s .\n", im_GetProgramName(), substitute, fontName);

    if (fontNames != NULL)
        XFreeFontNames(fontNames);


    /* font summary info is treated here as being the same on all servers
     *	if this is invalid, the fontsummary info has to be added to fcache
     */
    if (oldMDFD == NULL)
	GetFontSummary(self);		/* compute font summary info */
    return font;
}

/* KLUDGE  called by  ENSUREGRAPHIC() */
/* GetAnyOldGraphic()
	Finds a graphic (from the im) in case a call has been made 
	from a view which is not linked.
*/
	static struct xgraphic *
GetAnyOldGraphic()
{
	struct im *im;
	im = im_GetLastUsed();
	return  ((im == NULL) ? NULL : (struct xgraphic *)im->header.view.drawable);
}



/* ************* methods ****************** */

struct graphic *xfontdesc__CvtCharToGraphic(self, graphic, SpecialChar)
    struct xfontdesc * self;
    struct xgraphic *graphic;
    char SpecialChar;
{

    struct xgraphic * RetValue;
    long width, height;
    long x, y;
    XFontStruct *info;
    XCharStruct *ci;
    Pixmap newPixmap;
    GC gc;
    char str[1];
    XGCValues gcattr;
    int depth;

    ENSUREGRAPHIC(NULL);
    depth = DefaultDepth(xgraphic_XDisplay(graphic), xgraphic_XScreen(graphic));

    RetValue = xgraphic_New();
    if (!RetValue) return NULL;
/* This is text code extracted from my test suite, not the real stuff */

    info = GETXFONT(self, graphic);
    if (SpecialChar >= info->min_char_or_byte2 && SpecialChar <= info->max_char_or_byte2) {
	ci = (info->per_char != NULL) ? &(info->per_char[SpecialChar - info->min_char_or_byte2]) : &(info->max_bounds);
	width = ci->width;
	height = info->max_bounds.ascent + info->max_bounds.descent;
/* Older code - no longer seems to work properly
	width = ci->rbearing - ci->lbearing;
	height = ci->descent + ci->ascent;
*/
	x = 0;
/* Older code - no longer seems to work properly
	x = -ci->lbearing;
*/
	y = ci->ascent;
	/* Note: we could have an empty character, in which case, we simulate it with a 1 by 1 character. Too bad X doesn't allow 0 sized pixmaps */
	if (!width) {
	    fprintf(stderr,"xfontdesc_CvtCharToGraphic: 0 width character %d in %X\n", SpecialChar, self);
	    width++;
	}
	if (!height) {
	    fprintf(stderr,"xfontdesc_CvtCharToGraphic: 0 height character %d in %X\n", SpecialChar, self);
	    height++;
	}
	newPixmap = XCreatePixmap(xgraphic_XDisplay(graphic), xgraphic_XWindow(graphic), width, height, depth);
        /* The following is bogus: it should be a in-core graphic
	   and created the same way -- insert it into a "universal pixmap"
	   such as the root of the window and use ordinary graphic operations
	   to fill in the values instead of going through the X versions.
	   This should allow 1) transportability to other systems and
           2) consistent semantics with other xgraphic objects */

	gcattr.fill_style = FillSolid;
	gcattr.foreground = graphic->backgroundpixel;
#ifndef PLANEMASK_ENV
	gc = XCreateGC(xgraphic_XDisplay(graphic), newPixmap, GCFillStyle | GCForeground, &gcattr);
#else /* PLANEMASK_ENV */
	gcattr.plane_mask = graphic->foregroundpixel ^ graphic->backgroundpixel;
	gc = XCreateGC(xgraphic_XDisplay(graphic), newPixmap, GCFillStyle | GCForeground | GCPlaneMask, &gcattr);
#endif /* PLANEMASK_ENV */
	XFillRectangle(xgraphic_XDisplay(graphic), newPixmap, gc, 0, 0, width, height);
	XSetFont(xgraphic_XDisplay(graphic), gc, info->fid);
	XSetForeground(xgraphic_XDisplay(graphic), gc, graphic->foregroundpixel);
	XSetFunction(xgraphic_XDisplay(graphic), gc, GXcopy);
	str[0] = SpecialChar;
	XDrawString(xgraphic_XDisplay(graphic), newPixmap, gc, x, y, str, 1);
	XFreeGC(xgraphic_XDisplay(graphic), gc);
    }
    else  {
	newPixmap = XCreatePixmap(xgraphic_XDisplay(graphic), xgraphic_XWindow(graphic), 8, 8, depth);
    }
    RetValue->localWindow = newPixmap;	
    RetValue->displayUsed = xgraphic_XDisplay(graphic);
    RetValue->screenUsed = xgraphic_XScreen(graphic);
    /* oops */
    RetValue->localGraphicContext = NULL;
    RetValue->localFillGraphicContext = NULL;
    return (struct graphic *) RetValue;
}

	struct font *
xfontdesc__GetRealFontDesc(self, graphic)
	struct xfontdesc * self;
	struct xgraphic *graphic;
{
	struct fcache *fc;
	ENSUREGRAPHIC(NULL);
	for (		fc = MDFD; 
			fc != NULL && fc->host != xgraphic_XDisplay(graphic)->fd; 
			fc = fc->next)
		{}
	if (fc == NULL)
		return (struct font *)xfontdesc_LoadXFont(self, graphic);
	else return (struct font *)fc->font;
}

long xfontdesc__TextSize(self, graphic, text, TextLength, XWidth, YWidth)
    struct xfontdesc * self;
    struct xgraphic *graphic;
    char * text;
    long TextLength;
    long * XWidth;
    long * YWidth;
{
    XFontStruct *font;
    register long retWidth = 0;

    ENSUREGRAPHIC(NULL);
    font = GETXFONT(self,graphic);

    if (font)
        retWidth = XTextWidth(font,text,TextLength);
    if (XWidth) *XWidth = retWidth;
    if (YWidth) *YWidth = 0;
    return retWidth;
}

/* This procedure returns the font size table for all characters in a font 
	XXX the value should depend on which display we are on */
short *xfontdesc__WidthTable(self, graphic)
	struct xfontdesc * self;
	struct xgraphic *graphic;
{
	register XFontStruct *font;
	register short * fontWidthTable;
	int i;

	if (self->header.fontdesc.widthTable)
		return self->header.fontdesc.widthTable;

	ENSUREGRAPHIC(NULL);
	fontWidthTable = (short *) malloc(fontdesc_NumIcons*sizeof(short));
	self->header.fontdesc.widthTable = fontWidthTable;
	font = GETXFONT(self, graphic);

	/* Is it a "small" font that we can handle? */
	if (font->min_byte1 || font->max_byte1) 
		/* oh well, we have a 16 bit font and we are screwed.
		   try to keep going by returning the same size for
		   everything */
		for(i=0; i < fontdesc_NumIcons; i++) 
			fontWidthTable[i] = font->max_bounds.width;
	else 
		for(i=0; i < fontdesc_NumIcons; i++) {
			if (i < font->min_char_or_byte2 ||
					i > font->max_char_or_byte2)
				 fontWidthTable[i] = 0;
			else
				fontWidthTable[i] = (font->per_char) 
					? font->per_char[i-font->min_char_or_byte2].width 
					: font->max_bounds.width;
	}
	return fontWidthTable;
}

/* This procedure returns the font size table for all characters in a font 
	XXX the value should depend on which display we are on */
short *xfontdesc__HeightTable(self, graphic)
	struct xfontdesc * self;
	struct xgraphic *graphic;
{

	register XFontStruct *font;
	register short * fontHeightTable;
	int i;

	if (self->header.fontdesc.heightTable)
		return self->header.fontdesc.heightTable;

	ENSUREGRAPHIC(NULL);
	fontHeightTable = (short *) malloc(fontdesc_NumIcons*sizeof(short));
	self->header.fontdesc.heightTable = fontHeightTable;
	font = GETXFONT(self, graphic);

	/* Is it a "small" font that we can handle? */
	if (font->min_byte1 || font->max_byte1)
		/* oh well, we have a 16 bit font and we are screwed.
		   try to keep going by returning the same size for
		   everything */
		for(i=0; i<fontdesc_NumIcons; i++)
			fontHeightTable[i] = font->max_bounds.ascent +
					font->max_bounds.descent;
	else
		for(i=0; i<fontdesc_NumIcons; i++) {
			if (i < font->min_char_or_byte2 ||
					i > font->max_char_or_byte2)
				fontHeightTable[i] = 0;
			else if (font->per_char != NULL)
				fontHeightTable[i] =
					font->per_char[i-font->min_char_or_byte2].ascent +
					font->per_char[i-font->min_char_or_byte2].descent;
			else
				fontHeightTable[i] = font->max_bounds.ascent +
					font->max_bounds.descent;
		}
	return fontHeightTable;
}

long xfontdesc__StringSize(self, graphic, string,XWidth,YWidth)
    struct xfontdesc * self;
    struct xgraphic *graphic;
    register unsigned char * string;
    register long * XWidth;
    register long * YWidth;
{

    register XFontStruct  *font;
    register long retWidth = 0;

    ENSUREGRAPHIC(NULL);
    font = GETXFONT(self, graphic);
    if (font)
        retWidth = XTextWidth(font,string,strlen(string));
    if (XWidth) *XWidth = retWidth;
    if (YWidth) *YWidth = 0;
    return retWidth;
}

void xfontdesc__CharSummary(self, graphic, LookUpChar, RetValue)
    struct xfontdesc * self;
    struct xgraphic * graphic;
    char LookUpChar;
    struct fontdesc_charInfo *RetValue;
{

    register XFontStruct *font;
    if (!RetValue) return;
    ENSUREGRAPHIC(/* no return value */);
    font = GETXFONT(self, graphic);

    /* Is it a "small" font that we can handle? */
    if (font->min_byte1 || font->max_byte1) {
	/* oh well, we have a 16 bit font and we are screwed.
	   try to keep going by returning the same size for
	   everything */
	register XCharStruct * maxChar = &font->max_bounds;
	RetValue->width = maxChar->rbearing-maxChar->lbearing;
	RetValue->height = maxChar->ascent+maxChar->descent;
	RetValue->xOriginOffset = -(maxChar->lbearing);
	RetValue->yOriginOffset = maxChar->ascent;     
	RetValue->xSpacing = maxChar->width;
	RetValue->ySpacing = 0;
    }
    else {
	if (LookUpChar<font->min_char_or_byte2 ||
	    LookUpChar>font->max_char_or_byte2) {
	    RetValue->width = 0;
	    RetValue->height = 0;
	    RetValue->xOriginOffset = 0;
	    RetValue->yOriginOffset = 0;
	    RetValue->xSpacing = 0;
	    RetValue->ySpacing = 0;
		}
	else {
	    register XCharStruct * c = (font->per_char != NULL) 
			? &font->per_char[LookUpChar-font->min_char_or_byte2] 
			: &(font->max_bounds);
	    RetValue->width = c->rbearing- c->lbearing;
	    RetValue->height = c->ascent + c->descent;
	    RetValue->xOriginOffset = -(c->lbearing);
	    RetValue->yOriginOffset = c->ascent;     
	    RetValue->xSpacing = c->width;
	    RetValue->ySpacing = 0;
	}
    }
}

/* ************* predefines ************** */

struct xfontdesc *xfontdesc__Allocate(classID)
    struct classheader *classID;
{
    return (struct xfontdesc *) malloc(sizeof(struct xfontdesc));
}

void xfontdesc__Deallocate(classID, self)
    struct classheader *classID;
    struct xfontdesc *self;
{
/* Fontdesc structures are never deallocated since they are reused. */
}

	boolean
xfontdesc__InitializeObject(classID, self)
	struct classheader *classID;
	struct xfontdesc * self;
{
	self->header.fontdesc.MachineDependentFontDescriptor = NULL;
	return TRUE;
}

	void
xfontdesc__FinalizeObject(classID, self)
	struct classheader *classID;
	struct xfontdesc * self;
{
	struct fcache *fc, *tfc;
	for (fc = MDFD; fc != NULL; fc = tfc) {
		tfc = fc->next;
		XFreeFont(fc->dpy, fc->font);
	}
}

	boolean
xfontdesc__InitializeClass(classID)
	struct classinfo classID;
{

	announceSubstitutions = 
		environ_GetProfileSwitch("AnnounceFontSubstitutions", FALSE);
	return TRUE;
}
