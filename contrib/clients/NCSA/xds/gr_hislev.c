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
 *	File:		gr_hislev.c
 *	Contents:	Histogram level window functions for graphics module
 */

#include "gr_com.h"

#define MINHISTXSIZE	50
#define MINHISTYSIZE	100
#define	MAXHISTXSIZE	900
#define	MAXHISTYSIZE	900
#define	LEEWAY			2
#define XORIGIN			50
#define XOFFSET			30
#define YOFFSET			30


/*
 *	Return a Hist window for an arbitrary slice of data
 */
A_HistWind_t
*gr_InitHistLevel(header,shellName,parent,tparent)
A_HistWind_t *header;
char   	 *shellName;
Widget	 parent;
A_DsplWind_t	*tparent;
{
	A_Axes_t		orient;
	A_Data_t		*hdf=tparent->parent->parent->data;
	A_HistWind_t	*tmp;
	Widget			boxWind;
	int				xsize,ysize,winxsize,winysize;
	int				xaxis,yaxis;
	char			label[STRNG80];

	gr_WidgetCursor(tparent->shell,XC_watch);

	orient = tparent->axesOrient;
    if (orient.col < 0)
		xaxis = -orient.col;
	else 
		xaxis = orient.col;
	if (orient.row < 0)
		yaxis = -orient.row;
	else
		yaxis = orient.row;

	sprintf(label,"%s: (%s%d,%s%d)",
		tparent->parent->parent->filename,
		axesLabels[xaxis-1],tparent->selX,
		axesLabels[yaxis-1],tparent->selY);

	if ((tmp = (A_HistWind_t *)td_Malloc(sizeof(A_HistWind_t),
			   "A_HistWind_t")) == NULL)
		return(NULL);

	tmp->xpos = tparent->selX;
	tmp->ypos = tparent->selY;
	tmp->xsize = xsize = (tparent->zdim-orient.plane)*tparent->width+2*XOFFSET;
	tmp->ysize = ysize = gr_color.nColors+2*YOFFSET;

    if (xsize > MINHISTXSIZE)
        if (xsize > MAXHISTXSIZE)
            winxsize = MAXHISTXSIZE;
        else
            winxsize = xsize+LEEWAY;
    else
        winxsize = MINHISTXSIZE+LEEWAY;

    if (ysize > MINHISTYSIZE)
        if (ysize > MAXHISTYSIZE)
            winysize = MAXHISTYSIZE;
        else
            winysize = ysize+LEEWAY;
    else
        winysize = MINHISTYSIZE+LEEWAY;

	tmp->shell = gr_MakeWindow("XDS Hist",parent,&(tmp->win),
					(XtCallbackProc)gr_CloseHistLevel,
					HISTWIND,label,"Close",(caddr_t)tmp,
					110,10,winxsize+8,winysize+37);

	gr_ImageSetCMap(tmp->shell);

	tmp->imageVPort = gr_MakeVPort("XDSvport",tmp->win, NOSCROLL, HP,
			NULL, NULL, 5,(caddr_t)tmp,
			0,0,winxsize,winysize);

	tmp->imageWin	= gr_MakeWorkSpace("XDSworkspace",
						tmp->imageVPort, (XtCallbackProc)gr_HistExpose,
						NULL, NULL,(caddr_t)tmp,
						0,0,xsize,ysize);

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
 *	Close a Hist Window
 */
void
gr_CloseHistLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_HistWind_t	*histWin=(A_HistWind_t *)client_data;
	short i;

	if (histWin != NULL)
	{
		if (histWin->prev != NULL)
			histWin->prev->next = histWin->next;
		else
			histWin->parent->histWin = histWin->next;

		if (histWin->next != NULL)
			histWin->next->prev = histWin->prev;

		histWin->parent->numHistWins--;

		XtDestroyWidget(histWin->shell);
		td_Free((char *)histWin);
	}
}
