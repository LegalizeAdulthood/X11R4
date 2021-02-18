#include "xpic.h"
#include "windows.h"
#include "newfonts.h"
#include "draw.h"

extern void CleanUpMode();

/*
 *  If a PushButton has been pressed in a mode other than START_MODE,
 *  there's probably rubber-banded stuff on teh screen that we'll have
 *  to clean up
 */
#define RESETMODE()	if(drawingMode != START_MODE) CleanUpMode(); else

/*
 *  A new style of button - in the pure world, this would be a nifty
 *  widget - I've got a zillion and one things to do, so for now it's a
 *  quick kludge
 */


static FontFamily *curFont;
static FontFamily *availFonts;
static int nAvailFonts;
static int initialized = FALSE;
static Buf *fontBuf, *sizeBuf;
static char *sizestr;

FontFamily *defaultFontFamily;
int fontChanged = 0;

#define changelabel(w, s, buf) \
	XtSetArg(args[0], XtNlabel, UpDownLabel(s, buf)),\
	XtSetValues(w, args, 1)

void changefont()
{
	textFont = ChangeFont(&curFont->sizes[curFont->cursize], &spacePad);
	setfont(gcNormal, textFont->fid);
	setfont(gcInvert, textFont->fid);
	fontChanged = 0;
}

static char *UpDownLabel(s, buf) 
char *s;
Buf *buf;
{
	while (strlen(s) + 5>= buf->size) {
		buf->size += BUF_CHUNK;
		buf->buf = XtRealloc(buf->buf, (unsigned) buf->size);
	}
	(void) sprintf(buf->buf, "+ %s -", s);
	return(buf->buf);
}

/*ARGSUSED*/
void donothing(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
}

/*ARGSUSED*/
static int initializefonts(label)
char *label;
{
	register int i, j;
	char *default_fontname;
	int default_fontsize;
	static int status;

	if (initialized)
		return(status);
	initialized = TRUE;
	availFonts = InitFonts(&default_fontname, &default_fontsize, &nAvailFonts);
	/*
	 *  Delete font names for which we didn't find any X fonts - saves
	 *  pain later because we are guaranteed that there'll always be one
	 *  font, and that every font in the table will have at least one
	 *  size. Work backward because its faster.
	 */
#ifdef DEBUG
	printf("Fonts found are:\n");
#endif
	for(i = nAvailFonts - 1; i >= 0; i--) {
		if(availFonts[i].nsizes == 0 && (nAvailFonts - i) > 0) {
			/* 
			 *  drat! This is a bad font, and there's fonts after it, so
			 *  we're going to have to delete it. And deleting things in
			 *  arrays is such fun......
			 */
			bcopy((char *) &availFonts[i+1], (char *) &availFonts[i],
			 (nAvailFonts - i) * sizeof(FontFamily));
			nAvailFonts--;
		}
#ifdef DEBUG
		else {
			register int j;
			
			printf("\t%s:", availFonts[i].name);
			for(j = 0; j < availFonts[i].nsizes; j++) {
				printf(" %d", availFonts[i].sizes[j].pointsize);
			}
			printf("\n");
		}
#endif
	}
	if (nAvailFonts == 0) {
		/* Urk! */
#ifdef DEBUG
		printf("\tNone\n");
#endif
		status = FALSE;
		return(status);
	};
	
	if (default_fontname == NULL || 
	 (curFont = findfont(default_fontname, TRUE)) == NULL) {
		curFont = availFonts;
	}

	/*
	 *  Now we have to find the closest font size to default_fontsize
	 *  for all fonts.
	 */
	if (default_fontsize <= 0) {
		for(j = 0; j < nAvailFonts; j++)
			availFonts[j].cursize = 0;
	} else {
		for(j = 0; j < nAvailFonts; j++) {
			availFonts[j].cursize = default_fontsize;
			for (i = 0; i < availFonts[j].nsizes; i++) {
				if (availFonts[j].sizes[i].pointsize > availFonts[j].cursize) {
					break;
				}
			}
			if (i == 0 || 
			 availFonts[j].cursize - availFonts[j].sizes[i-1].pointsize >
			 availFonts[j].sizes[i].pointsize - availFonts[j].cursize) {
				/*
				 *  i == 0, in which case we have to use the i'th font, or
				 *  sizes[i].pointsize is closer to availFonts[j].cursize than
				 *  sizes[i-1].pointsize
				 */
				availFonts[j].cursize = i;
			} else {
				/*
				 *  i == availFonts[j].nsizes, in which case we have to use
				 *  i-1'th font, or sizes[i-1].pointsize is closer to
				 *  availFonts[j].cursize than sizes[i].pointsize
				 */
				availFonts[j].cursize = i-1;
			}
		}
	}
	defaultFontFamily = curFont;
	status = TRUE;
	return(status);
}

static FontSizes defaultfontsizes[] = {
	{0, DEFAULT_FONT, NULL},
};

static FontFamily defaultfontfamily[] = {
	{"default", defaultfontsizes, 1, 1, 0},
};

void textfont(label, updowninfo, posthandler)
char *label;
ButtonInfo *updowninfo;
ButtonInfo *posthandler;
{
	void textfont_updown();
	
	posthandler->name = NULL;
	fontBuf = (Buf *) XtMalloc(sizeof(Buf));
	fontBuf->buf = XtMalloc(BUF_CHUNK);
	fontBuf->size = BUF_CHUNK;
	fontChanged = 1;
	if (initializefonts(label)) {
		updowninfo->name = UpDownLabel(curFont->name, fontBuf);
		fontType = curFont;
		textSize = curFont->cursize;
		updowninfo->handler = textfont_updown;
	} else {
		updowninfo->name = "No Fonts Available";
		fontType = defaultFontFamily = curFont = defaultfontfamily;
		textSize = 0;
		updowninfo->handler = donothing;
	}
	return;
}

/*
 *  The posthandler is solely because we need the Widget type for the
 *  textsize button, since we'll need to change it when we change the
 *  textfont button - YAK (Yet another kludge)
 */
static Widget sizeWidget;

void textsize_posthandler(w)
Widget w;
{
	sizeWidget = w;
}


void textsize(label, updowninfo, posthandler)
char *label;
ButtonInfo *updowninfo;
ButtonInfo *posthandler;
{
	void textsize_updown();
	
	sizeBuf = (Buf *) XtMalloc(sizeof(Buf));
	sizeBuf->buf = XtMalloc(BUF_CHUNK);
	sizeBuf->size = BUF_CHUNK;
	sizestr = XtMalloc(5);
	fontChanged = 1;
	if (initializefonts(label)) {
		(void) sprintf(sizestr, "%4d",
		 curFont->sizes[curFont->cursize].pointsize);
		updowninfo->name = UpDownLabel(sizestr, sizeBuf);
		fontType = curFont;
		textSize = curFont->sizes[curFont->cursize].pointsize;
		updowninfo->handler = textsize_updown;
		posthandler->handler = textsize_posthandler;
		posthandler->name = "ok";	/* Anything non-null, just as a flag */
	} else {
		updowninfo->name = "No Fonts Available";
		fontType = defaultFontFamily = curFont = defaultfontfamily;
		textSize = 0;
		updowninfo->handler = donothing;
		posthandler->name = NULL;
	}
	return;
}

static int up;	/* are we incrementing or decrementing */

/*ARGSUSED*/
void textfont_updown(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	Arg args[1];
	
	RESETMODE();
	if (up) {
		if (++curFont - availFonts == nAvailFonts)
			curFont = availFonts;
	} else {
		if (curFont == availFonts)
			curFont = curFont + nAvailFonts - 1;
		else
			--curFont;
	}
	fontChanged = 1;
	changelabel(w, curFont->name, fontBuf);
	(void) sprintf(sizestr, "%4d", curFont->sizes[curFont->cursize].pointsize);
	changelabel(sizeWidget, sizestr, sizeBuf);
	fontType = curFont;
	textSize = curFont->sizes[curFont->cursize].pointsize;
}


/*ARGSUSED*/
void textsize_updown(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	Arg args[1];
	
	RESETMODE();
	if (up) {
		if (++curFont->cursize == curFont->nsizes)
			curFont->cursize = 0;
	} else {
		if (curFont->cursize == 0)
			curFont->cursize = curFont->nsizes - 1;
		else
			curFont->cursize--;
	}
	fontChanged = 1;
	(void) sprintf(sizestr, "%4d", curFont->sizes[curFont->cursize].pointsize);
	changelabel(w, sizestr, sizeBuf);
	textSize = curFont->sizes[curFont->cursize].pointsize;
}


/*ARGSUSED*/
void Increment(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
	up = TRUE;
}


/*ARGSUSED*/
void Decrement(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
	up = FALSE;
}
