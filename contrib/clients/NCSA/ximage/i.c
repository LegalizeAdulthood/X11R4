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

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "i.h"
#define	NIL	(struct IMAGE *) 0
#define	NULL	0
#define FALSE	0

static struct IMAGE *headIMAGE = NIL;
static struct IMAGE *tailIMAGE = NIL;

static char	deflabel = 'a';

struct IMAGE *NewIMAGE()
/****** this routine doesn't handle XIMalloc fail yet */
{
struct	IMAGE *i;
	if (i = (struct IMAGE *) XIMalloc(sizeof(struct IMAGE))) {

#ifdef DEBUG
/**/	printf("PrintStatus: before NewIMAGE()\n");
	fflush(stdout);
/**/	PrintStatus();
#endif

		i->name = (char *) NULL;
		i->isHDF = FALSE;
		i->xdim = 0;
		i->ydim = 0;
		i->label[0] = deflabel;
		i->label[1] = '\0';
		if ((++deflabel) > 'z') 
			deflabel = 'a';

		i->isFloat = FALSE;
		i->fdataset = (float *) NULL;
		i->maxvalue = 0;
		i->minvalue = 0;
		i->format = (char *) NULL;
		i->fwidth = 0;

		i->ri = NULL;
		i->dExpRI= NULL;
		i->isDExpRI = FALSE;
		i->polarimage = NULL;
		i->dExpPI= NULL;
		i->isDExpPI = FALSE;

		i->cimage = i->pimage = (XImage *) NULL;

		i->plotCartesian = TRUE;
		i->plotSepWindow = TRUE;
		i->ctrl_showing = FALSE;
		i->data_showing = FALSE;
		i->cart_showing = FALSE;
		i->polar_showing = FALSE;
		i->graph_showing = FALSE;

		i->cws.amIShowing = FALSE;
		i->iws.amIShowing = FALSE;
		i->pws.amIShowing = FALSE;
		i->dws.amIShowing = FALSE;
		i->gws.amIShowing = FALSE;
		i->gcws.amIShowing = FALSE;
		i->giws.amIShowing = FALSE;
		i->gsws.amIShowing = FALSE;
		i->ghws.amIShowing = FALSE;
		i->hasPalette = FALSE;
		i->palette = (XColor *) NULL;

		i->image_loaded = FALSE;
		i->image_showing = FALSE;
		i->next = NIL;
		i->prev = NIL;

		if (headIMAGE == NIL) 
			tailIMAGE = headIMAGE = i;
		else {
			tailIMAGE->next = i;
			i->prev = tailIMAGE;
			tailIMAGE = i;
			}
		}
	else { /* XIMalloc failed */
		return(NULL);
		}
#ifdef DEBUG
/**/	printf("PrintStatus: after NewIMAGE()\n");
	fflush(stdout);
/**/	PrintStatus();
#endif

	return(i);

} /* NewIMAGE() */

DestroyIMAGE(im)
struct IMAGE *im;
{
#ifdef DEBUG
/**/	printf("PrintStatus: before DestroyIMAGE()\n");
	fflush(stdout);
/**/	PrintStatus();
#endif 
	
	if (im->prev) 
		im->prev->next = im->next;
	else 
		headIMAGE = im->next;
	if(im->next)
		im->next->prev = im->prev;
	else
		tailIMAGE = im->prev;


	if (im->cimage)
		XDestroyImage(im->cimage);
	if (im->pimage)
		XDestroyImage(im->pimage);
	if (im->ri)
		XIFree(im->ri);
	if (im->polarimage)
		XIFree(im->polarimage);
	if (im->name)
		XIFree(im->name);

	if (im->isDExpRI)
		XIFree(im->dExpRI);
	if (im->isDExpPI)
		XIFree(im->dExpPI);

	XIFree(im);

#ifdef DEBUG
/**/	printf("PrintStatus: after DestroyIMAGE()\n");
	fflush(stdout);
/**/	PrintStatus();
#endif

} /* DestroyIMAGE() */


ReleaseAllImages()
{
struct IMAGE  *i;

	while(headIMAGE)
		CBCloseImageCtrlBox(0,headIMAGE,0);
}

#ifdef DEBUG
PrintStatus()
/* Print IMAGE list for debugging */
{
struct IMAGE *i;

	printf("Printing image link\n");
	fflush(stdout);

	if (headIMAGE)
		printf("headIMAGE = %s\n",headIMAGE->name);
	else
		printf("headIMAGE = NIL\n");
	fflush(stdout);

	if (tailIMAGE)
		printf("tailIMAGE = %s\n",tailIMAGE->name);
	else
		printf("tailIMAGE = NIL\n");
	fflush(stdout);

	i = headIMAGE;
	while(i != NIL){
		if (i->prev) 
			printf("%s<-",i->prev->name);
		else
			printf("NIL<- ");

		printf("%s",i->name);

		if (i->next)
			printf("-> %s \n",i->next->name);
		else
			printf("-> NIL\n");

		fflush(stdout);
		i = i->next;
		}

	printf("*****done\n");
}
#endif
