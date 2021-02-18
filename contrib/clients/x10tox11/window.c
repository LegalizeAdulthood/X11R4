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
 * $XConsortium: window.c,v 6.1 88/10/11 18:10:50 jim Exp $ 
 * $Locker:  $ 
 */
#include "resource.h"

typedef struct _DeletedWindow DeletedWindow;
struct _DeletedWindow {
	Window	id;	/* id of known deleted window. */
	Time	time;	/* approximate server time when deleted */
};

typedef struct _WindowCache WindowCache;
struct {
	int		max;
	int		hits;
	int		misses;
	WinInfoptr	list[ 20 ];	/* previously found window */
	WinInfoptr	root;		/* tree of all known windows */
	DeletedWindow	*deleted;	/* list of deleted windows. */
	int		deleteCount;	/* count of deleted windows. */
} Cache = { 1 };			/* two cache entries */

StaticFunction void InvalidateWindowCache(wp)
	WinInfoptr	wp;
{
	int		i;

	for (i = Cache.max; i >= 0; i--) {
		if (Cache.list[i] == wp)
			Cache.list[i] = NULL;
	}
}

StaticFunction WinInfoptr FindNonCachedWinTree(wp, win)
	WinInfoptr	wp;
	Window		win;
{
	WinInfoptr		child;

	while (wp) {
		if (win == wp->id)
			break;
		if (wp->child) {
			child = FindNonCachedWinTree(wp->child, win);
			if (child) {
				wp = child;
				break;
			}
		}
		wp = wp->sibling;
	}

	return(wp);
}

StaticFunction Bool WindowHasBeenDeleted(win)
	Window	win;
{
	DeletedWindow	*dp, *base = Cache.deleted;

	if (Cache.deleteCount) {
		dp = &base[ Cache.deleteCount ];
		while(--dp >= base) {
			if (dp->id == win) {
				/* mark it newer */
				dp->time = LastKnownServerTime;
				return (True);
			}
		}
	}
	return (False);
}

StaticFunction WinInfoptr FindWinTree(win)
	Window		win;
{
	WinInfoptr	wp;
	int		i;

	for (i = Cache.max; i >= 0; i--) {
		wp = Cache.list[i];
		if (wp && wp->id == win) {
			Cache.hits++;
			goto found;
		}
	}

	Cache.misses++;
	wp = FindNonCachedWinTree(Cache.root, win);

	if (wp) {
	    switch(Cache.max) {
	    case 9: Cache.list[8] = Cache.list[9];
	    case 8: Cache.list[7] = Cache.list[8];
	    case 7: Cache.list[6] = Cache.list[7];
	    case 6: Cache.list[5] = Cache.list[6];
	    case 5: Cache.list[4] = Cache.list[5];
	    case 4: Cache.list[3] = Cache.list[4];
	    case 3: Cache.list[2] = Cache.list[3];
	    case 2: Cache.list[1] = Cache.list[2];
	    case 1: Cache.list[0] = Cache.list[1];
	    }
	    Cache.list[Cache.max] = wp;
	}

found:
	return (wp);
}

/*
 * This routine recurses UP the tree looking for the parent of the
 * current window.  When it has found a parent that will adopt all
 * the children found en route, it unwinds, updating the tree in
 * the process.
 */
StaticFunction WinInfoptr FindForiegnWinTree(client, win, winNeeded)
	int	client;
	Window	win, winNeeded;
{
	Display		*dpy;
	WinInfoptr	parentwp, wp;
	Window		root, parent, *children;
	unsigned int	nchildren;
	Status		successful;

	dpy = clist[ client ]->dpy;

	successful = XQueryTree(dpy,
				win,
				&root,
				&parent,
				&children,
				&nchildren);

	if (! successful) {
	    MarkWindowDeleted(win);
	    if (winNeeded == win)
		return(NULL); /* it really doesn't exist */
	    Error("Orphaned window=0x%x!\n", win);
	}
	if (nchildren == 0 && winNeeded != win)
	    Error("0 children for window=0x%x, ancestor of 0x%x?\n",
		win, winNeeded);

	parentwp = FindWinTree(parent);
	if (parentwp == NULL) {
	    parentwp = FindForiegnWinTree(client, parent, winNeeded);
	    wp = FindWinTree(win); /* must return non-null */
	} else {
	    wp = AddWin(PrimaryClient, win, parentwp, False);
	    wp->foriegn = True;
	    wp->client = PrimaryClient;
	    wp->mapRequested = True; /* always assume it "wants" it mapped */
	    wp->mapped = True; /* and that it is mapped to begin with */
	    wp->neverMapped = False; /* etc, etc */
	    XSelectInput(PrimaryConnection, win, StructureNotifyMask);
	}
	UpdateWinTree(client, wp, nchildren, children);
	if (nchildren)
	    free((char *)children);
	return(wp);
}

/*
 * Called by those needing window info.
 */
WinInfoptr WinLookup(client, w)
	int	client;
	Window	w;
{
	WinInfoptr	wp;

	wp = FindWinTree(w);
	if (wp == NULL) {
	    if (! WindowHasBeenDeleted(w))
		wp = FindForiegnWinTree(client, w, w);
	}
	return(wp);
}

void MarkWindowDeleted(win)
	Window	win;
{
#define	ThirtySomething	(30 * 1000)
	int		i;
	DeletedWindow	*dp;
	Time		aLittleWhileAgo = LastKnownServerTime - ThirtySomething;

	/*
	 * See if previously deleted windows have expired.
	 */
	dp = Cache.deleted;
	for (i = 0; i < Cache.deleteCount; i++, dp++) {
		if (dp->id == win)
			goto FoundFree;
		if (dp->time < aLittleWhileAgo) {
			goto FoundFree;
		}
	}
	
	i = Cache.deleteCount;
	if (Cache.deleteCount++ == 0)
		Cache.deleted = (DeletedWindow *)	
			Xalloc(sizeof(DeletedWindow));
	else
		Cache.deleted = (DeletedWindow *)
			Xrealloc(Cache.deleted,
				(i+1) * sizeof(DeletedWindow));
	dp = &Cache.deleted[ i ];

FoundFree:
	dp->time = LastKnownServerTime;
	dp->id = win;
#undef	ThirtySomething
}

StaticFunction void FreeWinTree(dpy, wp)
	Display		*dpy;
	WinInfoptr	wp;
{
	WinInfoptr	next;

	if (wp == NULL)
		return;
	while (wp) {
		MarkWindowDeleted(wp->id);
		next = wp->sibling;
		FreeWinTree(dpy, wp->child);
		if (wp->savedPixmap)
			XFreePixmap(dpy, wp->savedPixmap);
		InvalidateWindowCache(wp);
		if (wp->mapReq.count)
			free(wp->mapReq.serial);
		if (wp->configureReq.count)
			free(wp->configureReq.serial);
		free((char *)wp);
		wp = next;
	}
}

StaticFunction void DeleteWinInfo(winfo)
	WinInfoptr	winfo;
{
	WinInfoptr wp, parentwp = winfo->parent;

	if (parentwp == NULL) {
		Error("deleting the top!\n");
		return;
	}

	/*
	 * Fix up the pointers in the tree.
	 */
	if (parentwp->child == winfo)
		parentwp->child = winfo->sibling;
	else {
		for (wp = parentwp->child; wp; wp = wp->sibling)
			if (wp->sibling == winfo) {
				wp->sibling = winfo->sibling;
				break;
			}
	}
	/*
	 * Isolate its reference pointers.
	 */
	winfo->sibling = NULL;
}

StaticFunction void FreeClientTree(client, wp)
	int		client;
	WinInfoptr	wp;
{
	WinInfoptr	next;

	if (wp == NULL)
		return;
	while (wp) {
		next = wp->sibling;
		FreeClientTree(client, wp->child);
		if (wp->client == client)
			FreeWinInfo(client, wp);
		wp = next;
	}
}

void FreeClientWindows(client)
	int	client;
{
	FreeClientTree(client, Cache.root);
}

void FreeWinInfo(client, winfo)
	int	client;
	WinInfoptr	winfo;
{
	Clientptr cp = clist[ client ];

	DeleteWinInfo(winfo);
	FreeWinTree(cp->dpy, winfo);
}

StaticFunction void AttachWinInfo(parent, child)
	WinInfoptr	parent;
	WinInfoptr	child;
{
	WinInfoptr	wp;

	/*
	 * Insert this window in the tree of windows.
	 */
	if (parent == NULL)
	    wp = Cache.root;
	else
	    wp = parent;
	
	if (wp) {
	    if (wp->child == NULL)
		wp->child = child;
	    else {
		wp = wp->child;
		while (wp->sibling)
		    wp = wp->sibling;
		wp->sibling = child;
	    }
	} else
	    Cache.root = child;
	child->parent = parent;
}

WinInfoptr AddWin(client, win, parent, isTransparent)
	int		client;
	Window		win;
	WinInfoptr	parent;
	Bool		isTransparent;
{
	Clientptr cp = clist[ client ];
	WinInfoptr wpnew;
	DeletedWindow	*dp, *base = Cache.deleted;

	/*
	 * First check the deleted window list and remove this window's
	 * name from it (if present).
	 */
	if (Cache.deleteCount) {
		dp = &base[ Cache.deleteCount ];
		while(--dp >= base) {
			if (dp->id == win) {
				dp->time = 0;
				dp->id = 0;
			}
		}
	}

	/*
	 * Now Create a structure to hold the new window.
	 */
	wpnew = (WinInfoptr)Xalloc(sizeof(WinInfo));
	if (wpnew == NULL)
		return(NULL);

	bzero((char *)wpnew, sizeof(WinInfo));

	wpnew->client = client;
	wpnew->parent = parent;
	wpnew->id = win;
	wpnew->transparent = isTransparent;
	wpnew->neverMapped = True;
	if (isTransparent)
		wpnew->gc = cp->opaqueClippedGC;
	else
		wpnew->gc = cp->clearClippedGC;

	AttachWinInfo(parent, wpnew);

	return(wpnew);
}

void MarkMappedWinTree(wp, mapped, client, serial)
	WinInfoptr 	wp;
	int		mapped;
	int		client;
	unsigned long	serial;
{
	while (wp) {
		/*
		 * Only mark the first map request, in case this client
		 * is a bit redundant.
		 */
		if (mapped)
			AddRequestSerial(&wp->mapReq, client, serial);
		wp->mapRequested = mapped;
		if (wp->child)
			MarkMappedWinTree(wp->child, mapped, client, serial);
		ClearUnmappedTransparent(wp);
		wp = wp->sibling;
	}
}

/*
 * Raise a window with respect to its siblings.  We do this
 * for the sake of transparent windows so that we may know what sibling
 * windows are above others in stacking order.  From this we can discover
 * which siblings occlude the transparent window and set the clip mask in
 * the gc accordingly.
 */
void RaiseWindow(wp)
	WinInfoptr wp;
{
	WinInfoptr first, next;

	if (wp->parent == NULL)
		return;	/* doesn't matter... we don't own the root */

	first = wp->parent->child;
	if (first == wp)
		return; /* nothing more to do */

	next = first;
	while (next->sibling != NULL) {
		if (next->sibling == wp) {
			next->sibling = wp->sibling;
			wp->sibling = first;
			wp->parent->child = wp;
			return;
		}
		next = next->sibling;
	}
	Error("RaiseWindow: wp missing!\n");
}

/*
 * Lower a window with respect to its siblings.
 */
void LowerWindow(wp)
	WinInfoptr wp;
{
	WinInfoptr next;

	if (wp->parent == NULL)
		return;	/* doesn't matter... we don't own the root */

	if (wp->sibling == NULL)
		return; /* nothing more to do */

	/*
	 * find it in the list, and remove it...
	 */
	next = wp->parent->child;
	if (next == wp) {
		wp->parent->child = wp->sibling;
		next = wp->sibling;
	} else {
		while (next->sibling != NULL && next->sibling != wp)
			next = next->sibling;
		if (next->sibling == NULL)
			Error("LowerWindow: wp missing!\n");
		next->sibling = wp->sibling;
	}

	/*
	 * Find the end of the list and stick it on.
	 */
	while (next->sibling != NULL)
		next = next->sibling;
	next->sibling = wp;
	wp->sibling = NULL;
}

/*
 * Lower the highest mapped child window with respect to its siblings.
 */
void LowerHighestChild(parent)
	WinInfoptr parent;
{
	WinInfoptr next, first, prev;
	Bool	foundNextMapped;

	if (parent->child == NULL)
		return; /* nothing more to do */

	/*
	 * find the window just before the first mapped child...
	 */
	next = parent->child;
	if (next->mapped) {
		first = next;
		prev = NULL;
	} else {
		while (next->sibling != NULL && ! next->sibling->mapped)
			next = next->sibling;
		prev = next;
		first = next->sibling;
	}
	if (first->sibling == NULL)
		return;	/* already last on the list */

	/*
	 * Find the end of the list and stick it on.
	 */
	foundNextMapped = False;
	next = first;
	while (next->sibling != NULL) {
		next = next->sibling;
		if (next->mapped)
			foundNextMapped = True;
	}
	if (! foundNextMapped)
		return;	/* only one window mapped */
	if (prev == NULL)
		parent->child = first->sibling;
	else
		prev->sibling = first->sibling;
	next->sibling = first;
	first->sibling = NULL;
}

/*
 * Raise the lowest mapped child window with respect to its siblings.
 */
void RaiseLowestChild(parent)
	WinInfoptr parent;
{
	WinInfoptr next, last, prev;
	int	mappedCount;

	if (parent->child == NULL)
		return; /* nothing more to do */

	/*
	 * find the window just before the last mapped child...
	 */
	next = parent->child;
	mappedCount = 0;
	while (next != NULL) {
		if (next->mapped)
			mappedCount++;
		if (next->sibling->mapped)
			prev = next;
		next = next->sibling;
	}
	if (mappedCount <= 1)
		return;	/* only one mapped child */
	/* if mappedCount > 1, then we know prev is non-null */

	last = prev->sibling;
	prev->sibling = last->sibling;
	parent->child = last;
}

/*
 * Returns True if this window and all of its parents are mapped.
 */
Bool WindowMapped(wp)
	WinInfoptr	wp;
{
	WinInfoptr	next;

	for (next = wp; next; next = next->parent) {
		if (!next->mapped)
			return(False);
	}
	return (True);
}

Bool WindowMapRequested(wp)
	WinInfoptr	wp;
{
	WinInfoptr	next;

	for (next = wp; next; next = next->parent) {
		if (!next->mapRequested)
			return(False);
	}
	return (True);
}

/*
 * Given a transparent window W, find a drawable parent, asigning a given
 * x and y so that they can be used as offsets relative to the parent.
 */
WinInfoptr FindDrawableParent(wp, px, py)
	WinInfoptr	wp;
	int		*px, *py;
{
	WinInfoptr	next;
	int		x, y;

	x = y = 0;
	for (next = wp; next; next = next->parent) {
		if (! next->transparent)
			break;
		x += wp->x;
		y += wp->y;
	}

	*px = x;
	*py = y;

	if (next == NULL)
		Error("FindDrawableParent: missing parent drawable!\n");

	return(next);
}

/*
 * modify an X10 transparent drawable (really an InputOnly window),
 * and x and y so the drawing actually
 * happens correctly on an InputOutput window.
 */
typedef struct _GCclips GCclips;
typedef struct _ClipList ClipList, *ClipListptr;
static struct _GCclips {
	struct _ClipList {
	    GC		gc;
	    XRectangle	clip;
	} *list;
	int	length;
} clips;

void InvalidateClips(gc)
	GC	gc;
{
	int	i;

	for (i=0; i<clips.length; i++)
		if (clips.list[ i ].gc == gc)
			clips.list[ i ].gc = NULL;
}

ChangeXYAndClipAndDrawable(dpy, wp, dptr, xptr, yptr)
	Display		*dpy;
	WinInfoptr	wp;
	Drawable	*dptr;
	int		*xptr, *yptr;	/* x, y */
{
	WinInfoptr	parent;
	int		x, y;
	GC		gc = wp->gc;

	/*
	 * First, find a drawble that is suitable for graphic requests.
	 */
	parent = FindDrawableParent(wp, &x, &y);

	*dptr = parent->id;
	*xptr += x;
	*yptr += y;

	/*
	 * now set the clipping rectangle.
	 */
	if (gc != NULL) {
	    XRectangle	clip;
	    int		i, freeEntry = -1;

	    clip.x = x;
	    clip.y = y;
	    clip.width = wp->width;
	    clip.height = wp->height;

	    /*
	     * Check out cache of clips already set.
	     */
	    for (i=0; i<clips.length; i++) {
		if (clips.list[ i ].gc == gc) {
		    if (clips.list[ i ].clip.x == x
		     && clips.list[ i ].clip.y == y
		     && clips.list[ i ].clip.width == clip.width
		     && clips.list[ i ].clip.height == clip.height)
			return;
		    goto foundGC;
		} else if (clips.list[ i ].gc == NULL)
		    freeEntry = i;
	    }

	    if (freeEntry >= 0) {
		i = freeEntry;
		goto foundGC;
	    }

	    /* allocate a new clip cache entry */
	    if (clips.length++ == 0)
		clips.list = (ClipListptr) Xalloc(sizeof(ClipList));
	    else
		clips.list =
		    (ClipListptr) Xrealloc(clips.list,
					   clips.length * sizeof(ClipList));
	    i = clips.length - 1;

	foundGC:
	    clips.list[ i ].clip = clip;
	    clips.list[ i ].gc = gc;
	    XSetClipRectangles(
		    dpy,
		    gc,
		    0, 0,			/* x, y clip origin */
		    &clip,			/* clip rectangle */
		    1,			/* number of rectangles */
		    Unsorted);		/* rectangle ordering */
	}
}

void Reparent(win, parent)
	Window	win, parent;
{
	WinInfoptr	wp, oldParentwp, newParentwp;

	debug(X11_ReparentNotify, ("window 0x%x reparented... ", win));
	wp = FindWinTree(win);
	if (wp == NULL) {
	    debug(X11_ReparentNotify, ("not interested\n"));
	    return;
	}

	oldParentwp = wp->parent;
	debug(X11_ReparentNotify, ("from 0x%x ", oldParentwp->id));
	if (oldParentwp->id == parent) {
	    debug(X11_ReparentNotify, ("to the same one!\n"));
	    return;
	}

	debug(X11_ReparentNotify, ("to 0x%x.\n", parent));
	newParentwp = WinLookup(PrimaryClient, parent);
	if (newParentwp == NULL) {
	    debug(X11_ReparentNotify, ("  Can't find out about new parent!\n"));
	    return;
	}

	/*
	 * The action of looking up the parent (with FindWinTree()), may
	 * have caused us to discover that it had a child named 'win',
	 * and thus the reparenting may already have been done.
	 */
	if (wp->parent != oldParentwp) {
		debug(X11_ReparentNotify,
			("  reparenting 0x%x from 0x%x to 0x%x %s",
			win, oldParentwp->id, parent,
			"almost done twice\n"));
		return;
	}

	/* isolate it, and reattach it */
	DeleteWinInfo(wp);
	AttachWinInfo(newParentwp, wp);
}

void UpdateWinTree(client, parent, nchildren, children)
	int		client;
	WinInfoptr	parent;
	unsigned int	nchildren;
	Window		*children;
{
	WinInfoptr	wp, new;
	int		i;
	Window		child;

	/*
	 * children are returned from a QueryTree in bottom-to-top order.
	 */
	for (i = nchildren - 1; i >= 0; i--) {
	    child = children[ i ];
	    /*
	     * See if we already know about this one.
	     */
	    wp = FindWinTree(child);
	    if (wp != NULL) {
		if (wp->parent != parent) {
		    debug(X11_ReparentNotify, ("implied "));
		    Reparent(child, parent->id);
		}
		continue;
	    }
	    new = AddWin(client, child, parent, False);
	    new->foriegn = True;
	    new->client = PrimaryClient;
	    new->mapRequested = True; /* always assume it "wants" it mapped */
	    new->mapped = True; /* and that it is mapped to begin with */
	    new->neverMapped = False; /* etc, etc */
	}
}

/*
 * Change the tile mode of a window to either relative or absolute.
 */
void SetTileMode(dpy, wp, relative)
	Display		*dpy;
	WinInfoptr	wp;
	Bool		relative;
{
	XGCValues	gcv;

	if (relative) {
		gcv.ts_x_origin = - wp->x;
		gcv.ts_y_origin = - wp->y;
	} else {
		gcv.ts_x_origin = 0;
		gcv.ts_y_origin = 0;
	}
	XChangeGC(dpy,
		wp->gc,
		GCTileStipXOrigin|GCTileStipYOrigin,
		&gcv);
}

void SetClipMode(client, wp, drawThrough)
	int		client;
	WinInfoptr	wp;
	int		drawThrough;
{
	Clientptr	cp = clist[ client ];

	if (wp->transparent) {
		if (drawThrough)
			wp->gc = cp->clearUnclippedGC;
		else
			wp->gc = cp->clearClippedGC;
	} else {
		if (drawThrough)
			wp->gc = cp->opaqueUnclippedGC;
		else
			wp->gc = cp->opaqueClippedGC;
	}
	wp->drawThrough = drawThrough;
}

void ClearUnmappedTransparent(wp)
	WinInfoptr	wp;
{
	if (wp->unmapTransparent) {
		int		client = wp->client;
		Clientptr	cp = clist[ client ];
		Display		*dpy = cp->dpy;
		Window		w = wp->unmapTransparent;

		debug(X10_UnmapTransparent,
			("Clear transparent covering named 0x%x\n", w));
		AddRequestSerial(&cp->unmapTransReq, client, NextRequest(dpy));
		XDestroyWindow(dpy, w);
		MarkWindowDeleted(w);
		wp->unmapTransparent = 0;
		WindowsUnmappedTransparent--;
	}
}

StaticFunction void ClearUTWinTree(wp)
	WinInfoptr 	wp;
{
	while (wp) {
		ClearUnmappedTransparent(wp);
		if (wp->child)
			ClearUTWinTree(wp->child);
		wp = wp->sibling;
	}
}

/*
 * if any child of wp has been unmapped transparently and has a window
 * holding the screen contents, delete the window now.
 */
void ClearUnmappedTransparentTree(wp)
	WinInfoptr	wp;
{
	ClearUTWinTree(wp->child);
}

Bool CausedByRequest(rp, serial)
	RequestSerialptr	rp;
	unsigned long		serial;
{
	int		i;
	unsigned long	*serialList;

	serialList = rp->serial;
	for (i = 0; i < rp->count; i++, serialList++)
		if (*serialList == serial)
			return (True);
	return (False);
}

void AddRequestSerial(rp, client, serial)
	RequestSerialptr	rp;
	int			client;
	unsigned long		serial;
{
	Clientptr	cp = clist[ client ];
	int		i;
	unsigned long	*serialList;

	/*
	 * See if previous requests have expired.
	 */
	serialList = rp->serial;
	for (i = 0; i < rp->count; i++, serialList++)
		if (*serialList < cp->lastEventDispatched)
			goto FoundFree;
	
	i = rp->count;
	if (rp->count++ == 0)
		rp->serial = (unsigned long *) Xalloc(sizeof(unsigned long));
	else
		rp->serial = (unsigned long *)
			Xrealloc(rp->serial, (i+1) * sizeof(unsigned long));
	serialList = &rp->serial[ i ];

FoundFree:
	*serialList = serial;
}

Bool WindowHasBackingStore(client, wp)
	int		client;
	WinInfoptr	wp;
{
	Clientptr	cp = clist[ client ];
	PixmapInfoptr	pp;
	short		x = wp->x,
			y = wp->y;
	int		width = wp->width + (wp->borderWidth<<1),
			height = wp->height + (wp->borderWidth<<1);

	for (pp = cp->pixmaps; pp; pp = pp->next) {
		if (pp->isSavedPixmap
		 && x == pp->x
		 && y == pp->y
		 && width == pp->width
		 && height == pp->height) {
			pp->isBackingStore = True;
			wp->savedPixmap = pp->id;
			return(True);
		}
	}

	return(False);
}

#ifdef DEBUG
void ShowWinTree(wp, level)
	WinInfoptr	wp;
	int	level;
{
	if (! debugged(XWinTreeDebug))
		return;

	if (wp == NULL)
		wp = Cache.root;
	if (level == 0)
		bugout("Window Tree:\n");
	while (wp) {
		if (! debugged(XWinTreeMappedOnlyDebug) || wp->mapped) {
			bugout("   %*s", level * 3, "");
			bugout("0x%x (%3d,%3d) %3d X %3d mask=0x%x",
				wp->id,
				wp->x,
				wp->y,
				wp->width,
				wp->height,
				wp->mask);
			if (wp->tileModeRelative)
				bugout(",relative tile");
			if (wp->transparent)
				bugout(",transparent");
			if (wp->mapped)
				bugout(",mapped");
			if (wp->drawThrough)
				bugout(",drawThrough");
			if (wp->foriegn)
				bugout(",foriegn");
			bugout("\n");
			if (wp->child)
				ShowWinTree(wp->child, level+1);
		}
		if (level == 0)
			break;
		wp = wp->sibling;
	}
}

void ResetCache()
{
	Cache.hits = 0;
	Cache.misses = 0;
	bzero(Cache.list, sizeof(WinInfoptr) * Cache.max);
}

void SetCacheSize(size)
	int	size;
{
	Cache.max = size;
	bugout("cache size set to %d\n", size);
}

void ShowCachePerformance()
{
	float	hits = Cache.hits,
		misses = Cache.misses,
		hitPercent, missPercent;

	if (Cache.hits == 0 && Cache.misses == 0)
		hitPercent = missPercent = 0;
	else {
		hitPercent = 100 * (hits / (hits+misses));
		missPercent = 100 * (misses / (hits+misses));
	}

	bugout("%d hits (%4.1f%%), %d misses (%4.1f%%)\n",
		Cache.hits, hitPercent,
		Cache.misses, missPercent);
}
#endif DEBUG
