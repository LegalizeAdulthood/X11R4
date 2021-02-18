/*
 * This NCSA software product is public domain software.  Permission
 * is granted to do whatever you like with it. Should you feel compelled 
 * to make a contribution for the use of this software, please send 
 * comments as to what you like or don't like about the software, 
 * how you use it, how it helps your work, etc. to softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Dave Thompson, National Center for Supercomputing Applications
 *         davet@ncsa.uiuc.edu
 */

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "animate.h"

#define NIL     (struct ANIMATION *) 0

struct ANIMATION *headANIMATION = NIL;
struct ANIMATION *tailANIMATION = NIL;


struct ANIMATION *NewAnimation()
{
struct ANIMATION *a;
	

	if (!(a = (struct ANIMATION *) XIMalloc(sizeof(struct ANIMATION)))) 
		return(NULL);

	a->name = (char *) 0;
	a->xdim = 0;
	a->ydim = 0;
	a->files = (char **) NULL;
	a->numFrames = 0;
	a->frameNum = 0;
	a->skip = 0;
	a->status = A_STOP;
	a->inMemory = FALSE;
	a->ctrl_showing = FALSE;
	a->pixmaps = (Pixmap *) NULL;
	a->raster = (unsigned char *) NULL;
	a->expRaster = (unsigned char *) NULL;
	a->isExpanded = FALSE;

	a->next = NIL;
	a->prev = NIL;

	if (headANIMATION == NIL)
	        tailANIMATION = headANIMATION = a;
	else {
	        tailANIMATION->next = a;
	        tailANIMATION = a;
	        }

	return(a);
}

void DestroyAnimation(a)
struct ANIMATION *a;
{
        if (a->prev)
                a->prev->next = a->next;
        else
                headANIMATION = a->next;
        if(a->next)
                a->next->prev = a->prev;
        else
                tailANIMATION = a->prev;

	if (a->isExpanded)
		XIFree(a->expRaster);

	XIFree(a->files);
	XIFree(a->name);
	return;
}

ReleaseAllAnimations()
{
struct ANIMATION *a;
	while (headANIMATION)
		CBDoneAnimateCtrlBox(0,headANIMATION,0);
}
