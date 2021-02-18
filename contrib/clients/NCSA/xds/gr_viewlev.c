/*
 * National Center for SuperComputing Applications, University of Illinois
 *
 * This NCSA software product is public domain software.  Permission
 * is hereby granted to do whatever you like with it. Should you wish
 * to make a contribution towards the production of this software, please
 * send us your comments about your experience with the software,  why
 * you liked or disliked it, how you use it, and most importantly, how it
 * helps your work. We will receive your comments at softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Eng-Whatt Toh, National Center for Supercomputing Applications
 *         ewtoh@ncsa.uiuc.edu
 */

/*
 *	File:		gr_viewlev.c
 *	Contents:	View level window functions for graphics module
 */

#include "gr_com.h"

#define MINVIEWXSIZE	120
#define MINVIEWYSIZE	120
#define	MAXVIEWXSIZE	900
#define	MAXVIEWYSIZE	900
#define	LEEWAY			2


/*
 *	Return a View window for an arbitrary slice of data
 */
A_ViewWind_t
*gr_InitViewLevel(header,shellName,parent,scale,tparent)
A_ViewWind_t *header;
char   	 *shellName;
int		 scale;
Widget	 parent;
A_ArbWind_t	*tparent;
{
	A_ViewWind_t	*tmp;
	A_Box_t			*box=tparent->xybox;
	Widget			boxWind;
	int				xsize,ysize,winxsize,winysize;
	char			label[STRNG80];

	gr_WidgetCursor(tparent->shell,XC_watch);

	sprintf(label,"%s: Arbitrary Slice",tparent->parent->filename);

	if ((tmp = (A_ViewWind_t *)td_Malloc(sizeof(A_ViewWind_t),
			   "A_ViewWind_t")) == NULL)
		return(NULL);

	if ((tmp->data = gr_ViewgetData(box,scale,1,tparent->parent)) == NULL)
		return(NULL);

	xsize = box->xpicsize;
	ysize = box->ypicsize;

    if (xsize > MINVIEWXSIZE)
        if (xsize > MAXVIEWXSIZE)
            winxsize = MAXVIEWXSIZE;
        else
            winxsize = xsize+LEEWAY;
    else
        winxsize = MINVIEWXSIZE+LEEWAY;

    if (ysize > MINVIEWYSIZE)
        if (ysize > MAXVIEWYSIZE)
            winysize = MAXVIEWYSIZE;
        else
            winysize = ysize+LEEWAY;
    else
        winysize = MINVIEWYSIZE+LEEWAY;

	tmp->shell = gr_MakeWindow("XDS View",parent,&(tmp->win),
					(XtCallbackProc)gr_CloseViewLevel,
					VIEWWIND,label,"Close",(caddr_t)tmp,
					110,10,winxsize+8,winysize+60);

	gr_ImageSetCMap(tmp->shell);

	tmp->imageVPort = gr_MakeVPort("XDSvport",tmp->win, NOSCROLL, HP,
			NULL, NULL, 5,(caddr_t)tmp,
			0,0,winxsize,winysize);

	tmp->image		= gr_ImageCreate(tmp->imageVPort,xsize,ysize,tmp->data);
	tmp->imageWin	= gr_MakeImageStatic("XDSstaticimage",
						tmp->imageVPort, tmp->image,
						NULL,(caddr_t)tmp,
						0,0,xsize,ysize);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,0,winysize+2,winxsize,24);
		gr_MakeButton("XDSbutton",boxWind,"Palette",
					  (XtCallbackProc)gr_LoadPAL,(caddr_t)tmp->shell,
					  1,1,50,20);
		gr_MakeButton("XDSbutton",boxWind,"Save",
					  (XtCallbackProc)gr_SaveView,(caddr_t)tmp,
					  51,1,50,20);

	tmp->parent	= tparent;
	tmp->prev	= NULL;
	tmp->next	= header;
	if (header != NULL)
		header->prev = tmp;

	gr_WidgetCursor(tmp->shell,XC_draped_box);
	gr_WidgetCursor(tparent->shell,XC_draped_box);

	return(tmp);
}


/*
 *	Close a View Window
 */
void
gr_CloseViewLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ViewWind_t	*viewWin=(A_ViewWind_t *)client_data;
	short i;

	if (viewWin != NULL)
	{
		if (viewWin->prev != NULL)
			viewWin->prev->next = viewWin->next;
		else
			viewWin->parent->viewWin = viewWin->next;

		if (viewWin->next != NULL)
			viewWin->next->prev = viewWin->prev;

		viewWin->parent->numViewWins--;

		XDestroyImage(viewWin->image);
		td_Free((char *)viewWin->data);
		XtDestroyWidget(viewWin->shell);
		td_Free((char *)viewWin);
	}
}
