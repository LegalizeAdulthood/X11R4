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
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Form.h>

#include "xstuff.h"
#include "gr_def.h"
#include "gr_var.h"
#include "notebook.h"

#define	NB_DEF_XDIM	400
#define	NB_DEF_YDIM	600

extern Widget   toplevelform;

/**/	struct NOTEBOOK	n;
/**/	char 	CdevelopFlag = 0;
/**/	char 	SdevelopFlag = 0;

void Notebook();



void CBTestNotebook(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
	Notebook(&n);
}


void CBDoneNotebook(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
struct NOTEBOOK *n;
	n = (struct NOTEBOOK *) client_data;

	XtDestroyWidget(n->topLevel);
}

void CBCalculate(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
	if (!CdevelopFlag) {
		if (SdevelopFlag)
			PrintMesg("Can't calculate from notes yet either\n");
		else 
			PrintMesg("Can't calculate from notes yet\n");
		}
	else
		PrintMesg("Still can't calculate from notes yet\n");
	CdevelopFlag++;
} 

void CBSave(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
	if (!SdevelopFlag) {
		if (CdevelopFlag)
			PrintMesg("Can't save notes yet either\n");
		else 
			PrintMesg("Can't save notes yet\n");
		}
	else
		PrintMesg("Still can't save notes yet\n");

	SdevelopFlag++;
}


void Notebook(n)
struct NOTEBOOK *n;
{
int     xpos,ypos;
Widget	topForm;
Widget	topComp;
Widget	box1;
Widget	vport;
static	Arg setPlace[]={{XtNbottom,(XtArgVal) XtChainBottom},
			{XtNvertDistance,65}};

/**/	CdevelopFlag = 0;
/**/	SdevelopFlag = 0;
        PlaceWindow(&xpos,&ypos);
	n->topLevel = MakeATopLevel("X Image [Notbeook]",toplevelform,
			xpos,ypos,NB_DEF_XDIM,NB_DEF_YDIM);

        Set_Pointer(n->topLevel,XC_gumby);

	topComp= MakeForm("Notebook",n->topLevel,0,0,
				NB_DEF_XDIM,NB_DEF_YDIM,0,0);

	box1 = Make_Board("Notebook ctrls",topComp,0,0,NB_DEF_XDIM-10,30);

	(void) MakeButton("Calculate",box1,5,2,CBCalculate,n,0,0);
	(void) MakeButton("Save",box1,70,2,CBSave,n,0,0);
	(void) MakeButton("Done",box1,NB_DEF_XDIM-60,2,CBDoneNotebook,n,0,0);

	topForm = MakeForm("blabla",topComp,0,65,NB_DEF_XDIM-10,
                        NB_DEF_YDIM-70,setPlace,2);

	vport = MakeVPort("bla",topForm,VERTHORIZ,0,0,NB_DEF_XDIM-10,
			NB_DEF_YDIM-70);


	{
	static char buff[4000];
	gr_MakeText("Notebook edit",vport,
			NOSCROLL,SELECTABLE,STRINGSOURCE,
			buff,5,65,NB_DEF_XDIM-10,
			NB_DEF_YDIM-70);
	}

}



