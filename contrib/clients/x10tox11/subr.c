/*
 * Copyright 1987 by Tektronix Inc, Beaverton, Oregon.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Tektronix not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* 
 * $XConsortium: subr.c,v 6.3 88/10/11 18:10:23 jim Exp $ 
 * $Locker:  $ 
 */
#include "resource.h"
#include <ctype.h>

char *StaticAlloc(size)
	int	size;
{
	static char	*storage;
	static int	storage_size;

	if (storage == NULL) {
	    storage = (char *) Xalloc(size);
	    storage_size = size;
	} else if (size > storage_size) {
	    storage = (char *) Xrealloc(storage, size);
	    storage_size = size;
	}
	return(storage);
}

StaticFunction GC MakePixmapGC(dpy, pid)
	Display	*dpy;
	Pixmap	pid;
{
	XGCValues	gcv;
	GC	gc;

	gcv.subwindow_mode = IncludeInferiors;
	gcv.graphics_exposures = False;
	gc = XCreateGC(dpy,
		pid,
		(u_long)(GCGraphicsExposures|GCSubwindowMode),
		&gcv);
	return (gc);
}

X11connect(display, client)
	char	*display;
	int	client;
{
	Display		*dpy;
	Clientptr	cp;
	int		MarkEndOfRequest();
	Pixmap		tmp;
	XGCValues	gcv;


	/*
	 * Allocate the storage and initialize it.
	 */
	cp = (Clientptr)Xalloc(sizeof(Client));
	if (cp == NULL)
		return(False);
	bzero((char *)cp, sizeof(Client));
	clist[ client ] = cp;

	/*
	 * Open the display.
	 */
	if ((dpy = XOpenDisplay(display)) == NULL) {
		Notice("Could not open display \"%s\"\n", display);
		free ((char *)cp);
		return(False);
	}
	cp->dpy = dpy;

	/*
	 * XXX allocate some additional space for request buffering,
	 * because BUFSIZ is not enough.
	 */
	{
#define BufferSize	(1024 * 64)
		char	*oldbuffer = dpy->buffer;
		XFlush(dpy);
		dpy->buffer = Xrealloc(dpy->buffer, BufferSize);
		dpy->bufptr = dpy->buffer + (dpy->bufptr - oldbuffer);
		dpy->bufmax = dpy->buffer + BufferSize;
#undef BufferSize
	}

	/*
	 * Create a depth one pixmap, so we can create a GC to go
	 * with it.
	 */
	tmp = XCreatePixmap(dpy,
		DefaultRootWindow(dpy),	/* drawable */
		32, 1,			/* width, height */
		1);			/* depth */
	cp->bitmapGC = MakePixmapGC(dpy, tmp);
	XFreePixmap(dpy, tmp);

	cp->pixmapGC = MakePixmapGC(dpy, DefaultRootWindow(dpy));

	gcv.subwindow_mode = ClipByChildren;
	cp->opaqueClippedGC = XCreateGC(dpy, DefaultRootWindow(dpy),
					  GCSubwindowMode, &gcv);
	cp->clearClippedGC = XCreateGC(dpy, DefaultRootWindow(dpy),
					  GCSubwindowMode, &gcv);

	gcv.subwindow_mode = IncludeInferiors;
	cp->opaqueUnclippedGC = XCreateGC(dpy, DefaultRootWindow(dpy),
					  GCSubwindowMode, &gcv);
	cp->clearUnclippedGC = XCreateGC(dpy, DefaultRootWindow(dpy),
					  GCSubwindowMode, &gcv);

	/*
	 * Effectively a backward link.  I.e. we can figure out
	 * the x10 client number from the x11 file descriptor.
	 */
	clist[ ConnectionNumber(dpy) ] = (Clientptr)client;

	if (DoSync)
		(void) XSetAfterFunction(dpy, AfterFunction);

	return(True);
}

ChangeSync(on)
	int	on;
{
	int	i;
	Clientptr	cp;

	for (i=0; i<FileDescriptors; i++) {
		cp = clist[ i ];
		if (cp > (Clientptr)FileDescriptors && cp->dpy) {
			(void) XSetAfterFunction(cp->dpy, AfterFunction);
		}
	}
}

X11serverfd(client)
	int	client;
{
	Clientptr	cp = clist[ client ];
	if (cp)
		return(ConnectionNumber(cp->dpy));
	Error("no client for %d", client);
	/*NOTREACHED*/
}

void FreePixmapInfo(client, pinfo)
	int	client;
	PixmapInfoptr	pinfo;
{
	Clientptr cp = clist[ client ];
	PixmapInfoptr pp, parentpp;

	parentpp = NULL;
	pp = cp->pixmaps;
	while (pp) {
		if (pp == pinfo) {
			/*
			 * If this was created with PixmapSave, then the
			 * pixmap will probably be freed by FreeWinInfo()
			 */
			if (! pinfo->isBackingStore)
				XFreePixmap(cp->dpy, pinfo->id);
			if (parentpp == NULL)
				cp->pixmaps = pinfo->next;
			else
				parentpp->next = pinfo->next;
			free((char *)pinfo);
			return;
		}
		parentpp = pp;
		pp = pp->next;
	}
	Error("Pixmap ptr missing");
	/*NOTREACHED*/
}

PixmapInfoptr GetTextPixmap(client, width, height)
	int	client;
	unsigned int	width,
			height;
{
	PixmapInfoptr	pp;

	if (pp = clist[ client ]->textBackground) {
		if (width <= pp->width && height <= pp->height)
			return(pp);
		FreePixmapInfo(client, pp);
	}
	pp = CreatePixmap(client, DefaultRootWindow(clist[ client ]->dpy),
		width, height, 1, (PixmapInfoptr)NULL);
	if (pp == NULL)
		Error("can't create text pixmap (%dx%d)\n", width, height);
	clist[ client ]->textBackground = pp;
	return(pp);
}

PixmapInfoptr CreatePixmap(client, d, width, height, depth, ppbitmap)
	int		client;
	Drawable	d;
	unsigned int	width,
			height,
			depth;
	PixmapInfoptr	ppbitmap;
{
	Clientptr cp = clist[ client ];
	Display	*dpy = cp->dpy;
	Pixmap	p;
	PixmapInfoptr pp, ppnew;

	p = XCreatePixmap(dpy, d, width, height, depth);
	if (p == 0)
		return(0);

	ppnew = (PixmapInfoptr)Xalloc(sizeof(PixmapInfo));
	if (ppnew == NULL) {
		XFreePixmap(dpy, p);
		return(0);
	}
	bzero((char *)ppnew, sizeof(PixmapInfo));

	pp = cp->pixmaps;
	if (pp) {
		while (pp->next)
			pp = pp->next;
		pp->next = ppnew;
	} else
		cp->pixmaps = ppnew;

	ppnew->id = p;
	ppnew->width = width;
	ppnew->height = height;
	ppnew->depth = depth;
	if (depth == 1)
		ppnew->gc = cp->bitmapGC;
	else
		ppnew->gc = cp->pixmapGC;
	if (ppbitmap)
		ppnew->bitmapid = ppbitmap->id;
	return(ppnew);
}

PixmapInfoptr PixmapLookup(client, p)
	int	client;
	Pixmap	p;
{
	Clientptr cp = clist[ client ];
	PixmapInfoptr pp;

	for (pp = cp->pixmaps; pp; pp = pp->next)
		if (pp->id == p)
			return(pp);
	return(NULL);
}

PixmapInfoptr BitmapLookup(client, b)
	int	client;
	XID	b;
{
	return( PixmapLookup(client, b) );
}

X11Flush(state, maxsock)
	int	state;	/* 0: flush what's needed; 1: flush graphics; 2: all */
	int	maxsock; /* highest socket number in use */
{
	int	i, bytecnt;
	Clientptr	cp;
	Display		*dpy;

	for (i=0; i<maxsock; i++) {
	    cp = clist[ i ];
	    if (cp > (Clientptr)FileDescriptors && (dpy = cp->dpy)) {
		/*
		 * XXX This assignment may be illegal, because Xlib is opaque...
		 * if so, just change it to 'bytecnt = 1'.
		 */
		bytecnt = dpy->bufptr - dpy->buffer;

		if (bytecnt == 0)
		    continue;
		switch (state) {
		case 0:
		    if ((cp->pendingFlags
		     & (DeviceEvents | ChangesPending)) == 0)
			continue;
		    cp->pendingFlags = 0;
		    break;
		case 1:
		    if ((cp->pendingFlags & GraphicsPending) == 0)
			continue;
		    cp->pendingFlags = 0;
		    break;
		}
		debug(XIODebug, ("(%s)flush %d bytes for #%d...",
		    (state == 0 ? "changes" :
			(state == 1 ? "graphics" : "mandatory")),
		    bytecnt, i));
		XFlush(dpy);
		/*
		 * We may have read some data... if so, then
		 * process input events.
		 */
		CheckX11events(i, 0);
	    }
	}
}

/*
 * Free all resources owned by a client
 */
Free_client_resources (client)
	register int client;
{
	Clientptr	cp = clist[ client ];
	PixmapInfoptr pp, pptmp;
	FontInfoptr fp, fptmp;
	
	if (GrabUnviewable == client)
		GrabUnviewable = 0;
	if (cp == NULL)
		Error("no resources for client %d to free", client);

	pp = cp->pixmaps;
	while (pp) {
		pptmp = pp->next;
		free((char *)pp);
		pp = pptmp;
	}

	fp = cp->fontinfo;
	while (fp) {
		fptmp = fp->next;
		XFreeFont(cp->dpy, fp->info);
		free((char *)fp);
		fp = fptmp;
	}

	if (cp->unmapTransReq.count)
		free(cp->unmapTransReq.serial);

	FreeClientWindows(client);
	InvalidateClips(cp->opaqueClippedGC);
	InvalidateClips(cp->opaqueUnclippedGC);
	InvalidateClips(cp->clearClippedGC);
	InvalidateClips(cp->clearUnclippedGC);
	InvalidateClips(cp->bitmapGC);
	InvalidateClips(cp->pixmapGC);
	XFreeGC(cp->dpy, cp->opaqueClippedGC);
	XFreeGC(cp->dpy, cp->opaqueUnclippedGC);
	XFreeGC(cp->dpy, cp->clearClippedGC);
	XFreeGC(cp->dpy, cp->clearUnclippedGC);
	XFreeGC(cp->dpy, cp->bitmapGC);
	XFreeGC(cp->dpy, cp->pixmapGC);

	X11CloseDisplay(cp->dpy);	/* to catch broken connections */

	if (cp->seq.last) {
		free((char *)cp->seq.req);
		free((char *)cp->seq.x11seq);
	}
	free((char *)cp);
	clist[ client ] = NULL;
}

StringCompare(string1, string2)
	char	**string1, **string2;
{
	return( strcmp(*string1, *string2) );
}

SortStrings(strings, count)
	char	**strings;
	int	count;
{
	qsort((char *)strings, count, sizeof(char *), StringCompare);
}

char **X11FontNames(dpy, a_nfonts)
	Display	*dpy;
	int	*a_nfonts;
{
	char	**fontlist;
	extern FontMap	x10fontMap[];
	extern FontMap	x11fontMap[];
	int	i, nfonts, idx, missing;

	fontlist = XListFonts(dpy, "*", 0x7fff, &nfonts);
	if (nfonts <= 0)
		Error("got %d font names", nfonts);

	/*
	 * Peruse the known list, seting the index to point to
	 * something in the list returned by the server.
	 */
	SortStrings(fontlist, nfonts);
	missing = 0;
	for (i = 0; x11fontMap[i].fontname; i++) {
		idx = searchlist(fontlist, nfonts, x11fontMap[i].fontname);
		if (idx < 0) {
			Notice("font %s missing", x11fontMap[i].fontname);
			missing++;
		}
		x11fontMap[i].fontindex = idx;
	}
	if (missing)
		Error("%d fonts missing", missing);

	/*
	 * Now simply run through the list of X10 known fonts and
	 * change the index from one that points into x11fontMap[] to
	 * one that points into the list returned by the server.
	 */
	for (i=0; x10fontMap[i].fontname; i++) {
		x10fontMap[i].fontindex =
			x11fontMap[ x10fontMap[i].fontindex ].fontindex;
	}

	*a_nfonts = nfonts;
	return(fontlist);
}

char *ToLower(name)
	char	*name;
{
	static char	buf[ BUFSIZ ];
	char	*p;

	for (p=buf; *name; p++, name++) {
		if (isupper(*name))
			*p = tolower(*name);
		else
			*p = *name;
	}
	*p = '\0';

	return(buf);
}

/*
 * Do a binary search on a list of char pointers.
 */
searchlist(list, length, name)
	char	**list,
		*name;
	int	length;
{
	int	i, cmp, top, bottom, change;

	top = length - 1;
	bottom = 0;
	i = length>>1;	/* start in the middle */
	while((cmp = strcmp(name, list[i])) != 0) {
		if (top <= bottom)
			return(-1);
		if (cmp > 0) {
			change = (top - i) >> 1;
			bottom = i+1;
		} else {
			change = (bottom - i) >> 1;
			top = i-1;
		}
		if (change == 0)
			change = cmp < 0 ? -1 : 1;
		i += change;
	}
	return(i);
}

/*
 * Do a binary search on the x10 FontMap list.
 */
searchFontMap(name)
	char	*name;
{
	extern FontMap	x10fontMap[];
	extern int	x10fontMapSize;
	int	i, cmp, top, bottom, change;

	
	top = x10fontMapSize - 1;
	bottom = 0;
	i = x10fontMapSize>>1;	/* start in the middle */
	while((cmp = strcmp(name, x10fontMap[i].fontname)) != 0) {
		if (top <= bottom)
			return(-1);
		if (cmp > 0) {
			change = (top - i) >> 1;
			bottom = i+1;
		} else {
			change = (bottom - i) >> 1;
			top = i-1;
		}
		if (change == 0)
			change = cmp < 0 ? -1 : 1;
		i += change;
	}
	return(i);
}

AddFont(client, font)
	int	client;
	Font	font;
{
	XFontStruct	*fstruct;
	Clientptr cp = clist[ client ];
	FontInfoptr fp, fpnew;

	fpnew = (FontInfoptr)Xalloc(sizeof(FontInfo));
	if (fpnew == NULL)
		return(False);

	if ((fstruct = XQueryFont(cp->dpy, font)) == NULL) {
		free((char *)fpnew);
		return(False);
	}

	fpnew->next = NULL;
	fp = cp->fontinfo;
	if (fp) {
		while (fp->next)
			fp = fp->next;
		fp->next = fpnew;
	} else
		cp->fontinfo = fpnew;

	fpnew->id = font;
	fpnew->info = fstruct;
	fpnew->ascent = max(fstruct->max_bounds.ascent, fstruct->ascent);
	fpnew->descent = max(fstruct->max_bounds.descent, fstruct->descent);
	return(True);
}

FontInfoptr FontLookup(client, f)
	int	client;
	Font	f;
{
	Clientptr cp = clist[ client ];
	FontInfoptr fp;

	for (fp = cp->fontinfo; fp; fp = fp->next)
		if (fp->id == f)
			return(fp);
	return(NULL);
}

void FreeFontInfo(client, pinfo)
	int	client;
	FontInfoptr	pinfo;
{
	Clientptr cp = clist[ client ];
	FontInfoptr fp, parentfp;

	parentfp = NULL;
	fp = cp->fontinfo;
	while (fp) {
		if (fp == pinfo) {
			XFreeFont(cp->dpy, pinfo->info);
			if (parentfp == NULL)
				cp->fontinfo = pinfo->next;
			else
				parentfp->next = pinfo->next;
			free ((char *)pinfo);
			return;
		}
		parentfp = fp;
		fp = fp->next;
	}
	Error("Font ptr missing");
	/*NOTREACHED*/
}

#ifdef notdef
min(a, b)
	register a, b;
{
	if (a > b)
		return (b);
	return(a);
}
#endif

max(a, b)
	register a, b;
{
	if (a > b)
		return (a);
	return(b);
}

GrabServer(client)
	int	client;
{
	if (GrabServerAllowed && !ServerGrabbed) {
	    XGrabServer(clist[ client ]->dpy);
	    X10GrabServer(client);
	    ServerGrabbed = True;
	}
}

UngrabServer(client)
	int	client;
{
	XUngrabServer(clist[ client ]->dpy);
	X10UngrabServer();
	ServerGrabbed = False;
}

X10toX11Client(client)
	int	client;
{
	Display *dpy = clist[ client ]->dpy;

	return (ConnectionNumber(dpy));
}

unsigned long X10toX11pixel(pix)
	unsigned long	pix;
{
	/*
	 * Translate X10 white and black pixel into the right
	 * thing... anything else is assumed to have been obtained from
	 * the X11 server.
	 */
	if (pix == 1)
		pix = ScreenWhitePixel;
	else if (pix == 0)
		pix = ScreenBlackPixel;
	return(pix);
}

/*
 * X10 guarenteed that white was 1 and black was zero.
 * if this isn't so, then we must change the function to get the
 * expected results.  For example,
 *	if white = 1 and black = 0
 *		then white or black = white
 *	but if white = 0 and black = 1
 *		then white or black = black
 * For this algorythm to work, white and black pixel values must be
 * complimentary; i.e. white rop black must always yield either a
 * white or a black.
 */

StaticFunction void CopyDefaultColormap(dpy)
	Display	*dpy;
{
	XColor	*allColors = NULL;
	unsigned long *pixels = NULL;
	int	i, pixelCnt;

	pixelCnt = DisplayCells(dpy, DefaultScreen(dpy)) - 2;
	allColors = (XColor *)Xalloc(sizeof(XColor) * pixelCnt);
	if (allColors == NULL)
	    goto fail;
	pixels = (unsigned long *)Xalloc(sizeof(unsigned long) * pixelCnt);
	if (pixels == NULL)
	    goto fail;
	    
	/*
	 * This may not work if the default DisplayCells is more than
	 * the DisplayCells for this visual... but there's no way to
	 * discover it without mucking in the depths structure.
	 */
	if (! XAllocColorCells(dpy,
		   ScreenColormap, False, NULL, 0, pixels, pixelCnt)) {
	    Notice("can't allocate %d pixels in new colormap\n", pixelCnt);
	    goto fail;
	}

	for (i = 0; i < pixelCnt; i++)
	    allColors[ i ].pixel = pixels[i];
	XQueryColors(dpy, DefaultColormap(dpy, DefaultScreen(dpy)),
	    allColors, pixelCnt);

	XStoreColors(dpy, ScreenColormap, allColors, pixelCnt);

	debug(XInitializeDebug, ("store %d cells:\n0x", pixelCnt));
	for (i = 0; i < pixelCnt; i++) {
	    debug(XInitializeDebug, ("%x(%x,%x,%x)\n",
		allColors[i].pixel,
		allColors[i].red,
		allColors[i].green,
		allColors[i].blue));
	}
	XFreeColors(dpy, ScreenColormap, pixels, pixelCnt, 0);
	XFree(allColors);
	XFree(pixels);
	return;
fail:
	XFree(allColors);
	XFree(pixels);
	Notice("Default colormap not copied\n");
}

StaticFunction Bool InitBlackAndWhite(dpy, needBlackAndWhite)
	Display	*dpy;
	Bool needBlackAndWhite;
{
	Status	status;
	Bool	weOwnThem;
	XColor	colors[2];
	XColor	*whiteColor = colors+0,
		*blackColor = colors+1;
	unsigned long blackAndWhite[2];
	unsigned long *pixels = blackAndWhite;
	unsigned long foundWhite, foundBlack;
	int	i, pixelCnt;

	blackColor->pixel = BlackPixel(dpy, DefaultScreen(dpy));
	whiteColor->pixel = WhitePixel(dpy, DefaultScreen(dpy));
	XQueryColors(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), colors, 2);

	if (needBlackAndWhite) {
	    weOwnThem = True;
	    
	    if (! XAllocColorCells(dpy,
		       ScreenColormap, False, NULL, 0, pixels, 2))
		Error("Can't allocate two color cells\n");

	    blackColor->pixel = pixels[0];
	    blackColor->flags = DoRed | DoGreen | DoBlue;
	    whiteColor->pixel = pixels[1];
	    whiteColor->flags = DoRed | DoGreen | DoBlue;
	    if (whiteColor->pixel == 0 && blackColor->pixel == 1) {
		whiteColor->pixel = 1;
		blackColor->pixel = 0;
	    }
	} else
	    weOwnThem = False;

	debug(XInitializeDebug,
	    ("%s white=0x%x(%x,%x,%x),black=0x%x(%x,%x,%x)\n",
	    needBlackAndWhite ? "allocated" : "given",
	    whiteColor->pixel,
	    whiteColor->red, whiteColor->green, whiteColor->blue,
	    blackColor->pixel,
	    blackColor->red, blackColor->green, blackColor->blue));

	if (whiteColor->pixel == 1 && blackColor->pixel == 0)
	    goto success; /* life is easy (but this is also unlikely) */

	if (whiteColor->pixel == 0 && blackColor->pixel == 1) {
	    NoTransform = False;
	    goto success; /* This is close enough */
	}

	if (weOwnThem)
	    XFreeColors(dpy, ScreenColormap, pixels, 2, 0);
	else
	    return (False);

	if (ScreenDepth > 16)
	    return (False); /* it would be absurd to try. */

	/*
	 * Try an alternate allocation method.
	 */
	pixels = (unsigned long *)
		Xalloc((1<<ScreenDepth) * sizeof(unsigned long));
	if (pixels == NULL) {
	    Notice("Cannot allocate mem for %d pixels\n", 1<<ScreenDepth);
	    goto fail;
	}

	/*
	 * First, grab a very large chunk of cells.
	 */
	if (GrabServerAllowed)
	    XGrabServer(dpy);
	for (i = ScreenDepth; i > 0; i--) {
	    status = XAllocColorCells(dpy,
		ScreenColormap, True, NULL, 0, pixels, 1<<i);
	    if (status)
		break;
	}
	if (!status)
	    goto fail;

	/*
	 * Check for appropriate black and white pixels.
	 */
	pixelCnt = 1<<i;
	debug(XInitializeDebug, ("Allocated %d pixels: ", pixelCnt));
	foundBlack = 0;
	foundWhite = 0;
	for (i=0; i<pixelCnt; i++) {
	    debug(XInitializeDebug,
		("%s%x", i==0 ? "0x" : ",", pixels[i]));
	    if (pixels[i] == 0 || pixels[i] == 1) {
		if (pixels[i] == 0)
		    foundBlack++;
		else
		    foundWhite++;
		bcopy(pixels+i+1, pixels+i,
		    sizeof(pixels[0]) * (pixelCnt - i - 1));
		pixelCnt--;
		i--; /* we've moved the array: we must repeat this cell */
	    }
	}

	/*
	 * If we still haven't found the pixels we are after, then continue
	 * allocating.
	 */
	while(!foundBlack || !foundWhite) {
	    status = XAllocColorCells(dpy,
		ScreenColormap, True, NULL, 0, pixels + pixelCnt, 1);
	    if (!status)
		break;
	    debug(XInitializeDebug, (",%x", pixels[ pixelCnt ]));
	    if (pixels[ pixelCnt ] == 0 || pixels[ pixelCnt ] == 1) {
		if (pixels[ pixelCnt ] == 0)
		    foundBlack++;
		else
		    foundWhite++;
	    } else
		pixelCnt++;
	}
	debug(XInitializeDebug, ("\n"));

	/*
	 * Free the colors we don't need.
	 */
	XFreeColors(dpy, ScreenColormap, pixels, pixelCnt, 0);
	if (!foundBlack || !foundWhite) {
	    if (foundBlack)
		pixels[0] = 0;
	    if (foundWhite)
		pixels[0] = 1;
	    if (foundBlack || foundWhite)
		XFreeColors(dpy, ScreenColormap, pixels, 1, 0);
	    debug(XInitializeDebug,
		("%s not found\n", foundBlack ? "White" :
			    foundWhite ? "Black" : "Black and White"));
	    goto fail;
	}
	XFree(pixels);

	/*
	 * Success!
	 */
	if (foundBlack > 1 || foundWhite > 1)
	    Notice("Allocated %d blacks and %d whites!!\n",
		foundBlack, foundWhite);
	blackColor->pixel = 0;
	whiteColor->pixel = 1;

success:
	ScreenBlackPixel = blackColor->pixel;
	ScreenWhitePixel = whiteColor->pixel;
	if (needBlackAndWhite)
	    XStoreColors(dpy, ScreenColormap, colors, 2);
	XUngrabServer(dpy);
	return (True);	/* life is easy */

fail:
	if (pixels)
	    XFree(pixels);
	XUngrabServer(dpy);
	return (False);
}

InitVisualAndColorMap(dpy)
	Display	*dpy;
{
	int		nd, nv, selectDepth;
	Depth		*depths;
	Visual		*visual, *selectVisual;
	Screen		*scr = DefaultScreenOfDisplay(dpy);
	int		preferredVisual[6];
	Bool		needColormap = False;

	preferredVisual[ TrueColor ] = 1;
	preferredVisual[ StaticGray ] = 2;
	preferredVisual[ StaticColor ] = 3;
	preferredVisual[ DirectColor ] = 4;
	preferredVisual[ GrayScale ] = 5;
	preferredVisual[ PseudoColor ] = 6;
	/*
	 * This gives the preferred order of visual class, from lowest to
	 * highest.  The static visuals are ok (StaticGray, StaticColor,
	 * TrueColor), but the dynamic ones are better (DirectColor,
	 * GrayScale, PseudoColor).  And of these, DirectColor is
	 * not as close a match to the X10 model as are GrayScale
	 * and PseudoColor.
	 */

	depths = scr->depths;

	debug(XInitializeDebug, ("screen has %d depth%s:\n",
	    scr->ndepths, scr->ndepths == 1 ? "" : "s"));

	/*
	 * This loop effectively picks the deepest visual of the preffered
	 * class (up to 16 bits deep).
	 */
	selectVisual = DefaultVisual(dpy, DefaultScreen(dpy));
	selectDepth = DefaultDepth(dpy, DefaultScreen(dpy));
	for (nd=0; nd < scr->ndepths; nd++) {
	    depths = &scr->depths[ nd ];
	    debug(XInitializeDebug, ("   depth of %d has %d visual%s:\n",
		depths->depth, depths->nvisuals,
		depths->nvisuals == 1 ? "" : "s"));
	    for (nv = 0; nv < depths->nvisuals; nv++) {
		visual = &depths->visuals[ nv ];
		debug(XInitializeDebug, ("      vis id=0x%x, class=%d(%s),",
		    visual->visualid,
		    visual->class,
		    VisualClassNames[ visual->class ]));
		debug(XInitializeDebug, (" rgb mask=(0x%x,0x%x,0x%x)\n",
		    visual->red_mask,
		    visual->green_mask,
		    visual->blue_mask));
		debug(XInitializeDebug,
		    ("       %d bits per rgb, %d map entries\n",
		    visual->bits_per_rgb,
		    visual->map_entries));
		if (depths->depth <= 16
		 && preferredVisual[ visual->class ]
		     > preferredVisual[ selectVisual->class ]) {
			selectDepth = depths->depth;
			selectVisual = visual;
		}
	    }
	}

	ScreenDepth = selectDepth;
	ScreenVisual = selectVisual;
	ScreenColorCells = ScreenVisual->map_entries;

	if (selectVisual == DefaultVisual(dpy, DefaultScreen(dpy))) {
	    ScreenColormap = DefaultColormap(dpy, DefaultScreen(dpy));
	    ScreenBlackPixel = BlackPixel(dpy, DefaultScreen(dpy));
	    ScreenWhitePixel = WhitePixel(dpy, DefaultScreen(dpy));
	    debug(XInitializeDebug, ("using default visual id 0x%x\n",
		selectVisual->visualid));
	    if (! InitBlackAndWhite(dpy, False)
		&& (selectVisual->class == DirectColor
		 || selectVisual->class == GrayScale
		 || selectVisual->class == PseudoColor)) {
		    if (! InitBlackAndWhite(dpy, True))
			needColormap = True;
	    }
	} else {
	    needColormap = True;
	    debug(XInitializeDebug,
		("visual id 0x%x selected, default was 0x%x\n",
		selectVisual->visualid,
		DefaultVisual(dpy, DefaultScreen(dpy))->visualid));
	}

	if (needColormap) {
	    ScreenColormap = XCreateColormap(dpy,
					     DefaultRootWindow(dpy),
					     selectVisual,
					     AllocNone); /* writable */
	    if (!InitBlackAndWhite(dpy, True))
		Error("Can't set up black and white\n");
	    CopyDefaultColormap(dpy);
	    XInstallColormap(dpy, ScreenColormap);
	    Notice("I hope you have a window manager");
	    Notice("that handles multiple colormaps!!");
	    debug(XInitializeDebug,
		  ("cmap=0x%x,white=0x%x,black=0x%x,min-installed-cmaps=%d\n",
		  ScreenColormap,
		  ScreenWhitePixel, ScreenBlackPixel,
	    	  MinCmapsOfScreen(scr)));
	}
}

AdjustNonRelativeX10Points(vlist, npoints, xdelta, ydelta)
	X10Vertex *vlist;
	int	npoints,
		xdelta,
		ydelta;
{
	int	i;

	for (i = 0; i < npoints; i++) {
		if (! (vlist[i].flags & X10VertexRelative)) {
			vlist[i].x += xdelta;
			vlist[i].y += ydelta;
		}
	}
}

PixmapInfoptr
StorePixmapOrBitmap(client, width, height, depth, format, data)
	int	client;
	unsigned int	width, height, depth;
	int	format;
	char	*data;
{
	Display 	*dpy = clist[ client ]->dpy;
	PixmapInfoptr	pp;
	unsigned long	gcmask;
	XGCValues	gcv;
	XImage		*image;
	GC		gc;

	if (depth == 1 && debugged(X10_StoreBitmap))
		ShowX10Bitmap((unsigned short *)data, width, height);

	pp = CreatePixmap(client,
			DefaultRootWindow(dpy),
			width,
			height,
			depth,
			(PixmapInfoptr) NULL);
	if (pp == 0)
		return(NULL);
	gc = pp->gc;

	gcmask = GCFunction;
	gcv.function = GXcopy;

	if (depth == 1) {
		gcmask |= GCForeground|GCBackground|GCPlaneMask;
		gcv.foreground = 1; /* X10 forground */
		gcv.background = 0; /* X10 background */
		gcv.plane_mask = 0xffffffff;
	}
	XChangeGC(dpy, gc, gcmask, &gcv);

	image = CreateX10Image (client, depth, format, data, width, height);
	XPutImage(dpy,
		pp->id,	/* drawable */
		gc,	/* gc */
		image,
		0, 0,	/* source x, y */
		0, 0,	/* destination x, y offset */
		width, height); /* width, height */

	/*
	 * We free the image, because image->data is static data handed to
	 * us from above.
	 */
	XFree(image);

	return(pp);
}

/*ARGSUSED*/
ClientByteOrder(client)
	int	client;
{
	/* The RIGHT thing is to get rid of the short swapping
	 * in the X10 dispatch code, and change this to depend
	 * on the real endian of the client (passed as an arg
	 * to X11connect and stored in the Client structure).
	 * But life is short.
	 */
	return(Bigendian ? MSBFirst : LSBFirst);
}

SyncWithServer(dpy, discard)
	Display	*dpy;
	int	discard;
{
	debug(XIODebug, ("Sync client %d: have %d events...",
		(int)clist[ ConnectionNumber(dpy) ], QLength(dpy)));
	XSync(dpy, discard);
	debug(XIODebug, ("Now have %d events.\n", QLength(dpy)));
}

/* Determine the number of bytes in a bitmap (including padding) */

X10CheckBitmapSize (height, width)
	register int height, width;
{
	if (height > 0 && width > 0)
	    return (X10WordPad(X10BitmapSize(width, height)));
	Xstatus = X10BadValue;
	return (0);
}

/* Determine the number of bytes in a pixmap (including padding) */

X10CheckPixmapSize (format, height, width)
        int	format, height, width;
{
	if (height > 0 && width > 0) {
	    if (format == X10_XYFormat)
		return ( X10WordPad(
			  X10XYPixmapSize(width, height, ScreenDepth)));
	    else if (format == X10_ZFormat) {
		if (ScreenDepth > 8)
		    return (X10WordPad(X10WZPixmapSize(width, height)));
		else if (ScreenDepth > 1)
		    return (X10BytePad(X10BZPixmapSize(width, height)));
	    }
	}
	Xstatus = X10BadValue;
	return (0);
}

/* malloc wrap-around, to take care of the "no memory" case, since
 * it would be difficult in many places to "back out" on failure.
 */

char *Xalloc (amount)
	int amount;
{
	register char *ptr;
	extern char *malloc();

	if (ptr = malloc ((unsigned) amount))
	    return (ptr);
	Error ("Allocating");
	/*NOTREACHED*/
}

/* realloc wrap-around, to take care of the "no memory" case, since
 * it would be difficult in many places to "back out" on failure.
 */

char *Xrealloc (ptr, amount)
	register char *ptr;
	int amount;
{
	extern char *malloc(), *realloc();

	if (ptr)
	    ptr = realloc (ptr, (unsigned) amount);
	else
	    ptr = malloc ((unsigned) amount);
	if (ptr)
	    return (ptr);
	Error ("Allocating");
	/*NOTREACHED*/
}
