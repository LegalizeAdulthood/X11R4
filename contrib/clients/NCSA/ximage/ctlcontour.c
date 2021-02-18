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
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <Xw/Xw.h>
#include <Xw/Toggle.h>
#include <X11/Dialog.h>

#include "i.h"
#include "xstuff.h"

#define	CTRLBOX_XDIM	350		/* dimensions of contour input box*/
#define	CTRLBOX_YDIM	110

#define EVENLY_SPACED	0
#define	USER_DEF	1

#define MAX_CONTOUR_LEVELS	256	/* maximum number of contour levels */
#define	DEFAULT_NUM_LEVELS	5	/* Default number of contour levels*/

extern  Widget  toplevelform;
extern	void	CBContour();	/* <----should be moved into this module*/

static	char	AmIOpen = FALSE;	/* is the ctrlContourBox created?*/
static	Widget	ctrlContourBox;
static	Widget	ctrlCBboard;
static	Widget	InputBox;
static	char	contourMode = EVENLY_SPACED;
static	float	contourMin,contourMax,contourIsFloat;
static	unsigned char	*contourLevels;
static	struct	IMAGE	*imageToContour;		/* Image working on */

static  Widget  mes_Dialog_NL;
static	char	mes_numLevels[10];
static	Widget	mes_Dialog_Min;
static	char	mes_minLevel[10];
static	Widget	mes_Dialog_Max;
static	char	mes_maxLevel[10];

static	Widget	mud_Dialog_L;
static	char	mud_Levels[1024];
static	Widget	esToggle;
static	Widget	udToggle;
       

static void MakeEvenlySpaced(w)
Widget w;
{
char	buff[80];


	mes_Dialog_NL = Make_Dialog("NumLevels",w,"Number of Levels",
			mes_numLevels,sizeof(mes_numLevels)-1,
			2,2, CTRLBOX_XDIM/3,CTRLBOX_YDIM/2-5);

	if (contourIsFloat)
		sprintf(buff,"Min Value (%6.3f)",contourMin);
	else
		sprintf(buff,"Min Value (%d)",(int) contourMin);
		
	mes_Dialog_Min = Make_Dialog("Min",w,buff, 
			mes_minLevel,sizeof(mes_minLevel)-1,
			CTRLBOX_XDIM/3+2,2, CTRLBOX_XDIM/3-2,CTRLBOX_YDIM/2-5);

	if (contourIsFloat)
		sprintf(buff,"Max Value (%6.3f)",contourMax);
	else
		sprintf(buff,"Max Value (%d)",(int) contourMax);
	mes_Dialog_Max = Make_Dialog("Max",w,buff, 
			mes_maxLevel,sizeof(mes_maxLevel)-1,
			CTRLBOX_XDIM*2/3+2,2, CTRLBOX_XDIM/3-5,CTRLBOX_YDIM/2-5);
} /* MakeEvenlySpaced() */

static void RemoveEvenlySpaced()
{
	XtDestroyWidget(mes_Dialog_NL);
	XtDestroyWidget(mes_Dialog_Min);
	XtDestroyWidget(mes_Dialog_Max);
}


static void MakeUserDef(w)
Widget w;
{

	mud_Dialog_L = Make_Dialog("NumLevels",w,
			"Please specify levels (separate with space)",
		mud_Levels,sizeof(mud_Levels),
		2,2, CTRLBOX_XDIM-7,CTRLBOX_YDIM/2-5);
}

static void RemoveUserDef()
{
	XtDestroyWidget(mud_Dialog_L);
}

static void CBCancel(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	PrintMesg("Cancelled Contour\n");
	XtDestroyWidget((Widget) client_data);
	AmIOpen = FALSE;

	return;

} /* CBCancel */



int TranslateFtoRI(f,min,max)
float	f;
float	min,max;
/* Translate from floating point scale to int 0-255 scale */
{
	return((int)(((f - min)/(max - min)) * 255.0));
}

static void AdvanceToBlank(s)
char **s;
{
	/* skip over any white space (if any) */
	while( (**s) && ( (**s == ' ') || (**s == '\n')))
		(*s)++;
	/* skip over test till next white space */
	while( (**s) && ( (**s != ' ') && (**s != '\n')))
		(*s)++;
	
}

static void CBOk(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
char	buff[80];
	/* check everything*/

	if (contourMode == EVENLY_SPACED) {
		float max,min;
		int num,x;
		int start,end,inc,count;

		/*** This should be atof instead of atoi ***/
		/**** but atof doesn't seem to be working */
		max = (float)atoi(XtDialogGetValueString(mes_Dialog_Max));
/*		printf("Dialog max says %s\n",
			XtDialogGetValueString(mes_Dialog_Max));
		printf("max is %f\n",max);
*/
		min = (float)atoi(XtDialogGetValueString(mes_Dialog_Min));
		num = atoi(XtDialogGetValueString(mes_Dialog_NL));
		max = (max > contourMax)? contourMax: 
				(max < contourMin)? contourMax: max;
		min = (min < contourMin)? contourMin:
				(min > contourMax)? contourMin: min;

		/* check bounds */ 
		if ((max == 0.0) && (min == 0.0)) { 
			max = contourMax; 
			min = contourMin;
			}
		num = ((num <= 0) || (num >MAX_CONTOUR_LEVELS))?
				DEFAULT_NUM_LEVELS:num;

		if (contourIsFloat) {
			start = TranslateFtoRI(min,contourMin,contourMax);
			end = TranslateFtoRI(max,contourMin,contourMax);
			sprintf(buff,
			  "Plotting %d contours levels from %6.3f to %6.3f\n",
			   num,min,max);
			PrintMesg(buff);
			}
		else {
			start = (int) min;
			end = (int) max;
			sprintf(buff,
				"Plotting %d contours levels from %d to %d\n",
				num,start,end);
			PrintMesg(buff);
			}
		inc = (end-start) / num;
		for(x=start, count=1; x < end ; x+=inc, count++)
			contourLevels[count] = x;
		contourLevels[0] = --count;
		}
	else { /* User Defined */
		char	*contourString;
		float	floatLevel;
		int	intLevel;
		int	count;

		contourString = XtDialogGetValueString(mud_Dialog_L);

		if (contourIsFloat) {
			count = 1;
			while(sscanf(contourString,"%f",&floatLevel) == 1) {
				contourLevels[count++] = (unsigned char) 
						TranslateFtoRI(floatLevel,
						contourMin,contourMax);
				AdvanceToBlank(&contourString);
				}
			contourLevels[0] = --count;
			}
		else {
			count = 1;
			while(sscanf(contourString,"%d",&intLevel) == 1) {
				contourLevels[count++] = (unsigned char)
						intLevel;
				AdvanceToBlank(&contourString);
				}
			contourLevels[0] = --count;
			}/*else*/
		}/*else*/

	XtDestroyWidget((Widget) client_data);
	AmIOpen = FALSE;
	PrintMesg("Plotting Contour\n");

/*	{ int x;
	for (x=1; x <= contourLevels[0]; x++ )
		printf("doing level %d \n",(int) contourLevels[x]);
	}
*/
	CBContour(w,imageToContour,NULL);

	return;
} /* CBOk */

static void CBEvenlySpaced(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
static	Arg setTrue[]={{XtNset,True}};
static	Arg setFalse[]={{XtNset,False}};

	if (contourMode == EVENLY_SPACED) { /* if already set */
		XtSetValues(w,setTrue,1);
		return;
		}
	else {
		XtSetValues(udToggle,setFalse,1);
		RemoveUserDef();
		MakeEvenlySpaced(InputBox);
		contourMode = EVENLY_SPACED;
		}
	return;
	
}
static void CBUserDefined(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
static	Arg setTrue[]={{XtNset,True}};
static	Arg setFalse[]={{XtNset,False}};

	if (contourMode == USER_DEF) { /* if already set */
		XtSetValues(w,setTrue,1);
		return;
		}
	else {
		XtSetValues(esToggle,setFalse,1);
		RemoveEvenlySpaced();
		MakeUserDef(InputBox);
		contourMode = USER_DEF;
		}
	return;
	
}

void CBCtrlContourBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct  IMAGE *i;		/* data set info stored here */
int	xpos,ypos;		/* where to put the box */
Widget	b1,b2,b3;
static	Arg setTrue[]={{XtNset,True}};

	if (AmIOpen) {
		PrintMesg("Contour Control Box already open\n");
		return;
		}


        i = (struct IMAGE *) client_data;

	contourMin = i->minvalue;
	contourMax = i->maxvalue;
	contourIsFloat = i->isFloat;
	imageToContour = i;
	if(!(i->contourLevels = contourLevels = (unsigned char *) 
		XICalloc(MAX_CONTOUR_LEVELS,sizeof(unsigned char)))){
		PrintMesg("Can't allocate memory for contour levels\n");
		return;
		}

	PrintMesg("Please specify contour levels to plot\n");

	PlaceWindow(&xpos,&ypos);
	ctrlContourBox = MakeATopLevel("X Image [Image.Graph.Contour]",
                                toplevelform,xpos,ypos,CTRLBOX_XDIM,
				CTRLBOX_YDIM);

	ctrlCBboard = Make_Board(i->name,ctrlContourBox,0,0,
                                CTRLBOX_XDIM,CTRLBOX_YDIM);

	InputBox = Make_Board(i->name,ctrlCBboard,0,0,
				CTRLBOX_XDIM,CTRLBOX_YDIM/2);


	b2 = Make_Board(i->name,ctrlCBboard,3,CTRLBOX_YDIM/2+2,CTRLBOX_XDIM/2-3,CTRLBOX_YDIM/2-5);
	esToggle = Make_Toggle("Evenly spaced",b2,5,5,CBEvenlySpaced,NULL,
				CBEvenlySpaced,NULL,False,False);
	udToggle = Make_Toggle("User defined levels",b2,5,30,CBUserDefined,NULL,
				CBUserDefined,NULL,False,False);

	b3 = Make_Board(i->name,ctrlCBboard,CTRLBOX_XDIM/2,CTRLBOX_YDIM/2+2,
				CTRLBOX_XDIM/2-2,CTRLBOX_YDIM/2-5);
	(void) MakeButton("    Ok    ",b3,10,(CTRLBOX_YDIM/4)-12,
				CBOk,ctrlContourBox,0,0);
	(void) MakeButton("  Cancel  ",b3,(CTRLBOX_XDIM/2)-85,
				(CTRLBOX_YDIM/4)-12,CBCancel,
				ctrlContourBox,0,0);

	if (contourMode == EVENLY_SPACED) {
		MakeEvenlySpaced(InputBox);
		XtSetValues(esToggle,setTrue,1);
		}
	if (contourMode == USER_DEF) {
		MakeUserDef(InputBox);
		XtSetValues(udToggle,setTrue,1);
		}

	AmIOpen = TRUE;
	return;
} /* CBCtrlContourBox */


/*
void CBMakeEvenlySpaced(ew,client_data,call_data)
Widget ew;
caddr_t client_data;
caddr_t call_data;
{
GC	gc;
XGCValues	gcval;
char	buff[80];
Widget	w;

	w = (Widget) client_data;
	gcval.foreground = black;
	gcval.background = white;
	gc = XtGetGC(i->ghws.box3,GCForeground | GCBackground, &gcval);

	XDrawString(XtDisplay(w),XtWindow(w),gc,5,(CTRLBOX_YDIM/4)-5,
			"Number of Levels",16);
	sprintf(buff,"Min Value (%f)",contourMin);
	XDrawString(XtDisplay(w),XtWindow(w),gc,CTRLBOX_XDIM/2,5,
			buff,strlen(buff));
	sprintf(buff,"Max Value (%f)",contourMax);
	XDrawString(XtDisplay(w),XtWindow(w),gc,CTRLBOX_XDIM/2,40,
			buff,strlen(buff));
}
*/
/****	XtAddEventHandler(ctrlContourBox,Exposure,0,
		CBMakeEvenlySpaced,w); *****/
	
