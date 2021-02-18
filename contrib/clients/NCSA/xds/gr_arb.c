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
 *	File:		gr_arb.c
 *	Contents:	Arb level routines for graphics module
 */

#include "gr_com.h"


/*
 *	Return view scale
 */
int
gr_ArbgetCurScale(arbWin)
A_ArbWind_t	*arbWin;
{
	return(atoi(gr_DialogGetValue(arbWin->scaleDialog)));
}


/*
 *	Return number of frames to generate
 */
int
gr_ArbgetCurNumFrames(arbWin)
A_ArbWind_t	*arbWin;
{
	return(atoi(gr_DialogGetValue(arbWin->numFramesDialog)));
}


/*
 *	Return percent increment
 */
double
gr_ArbgetCurZPercent(arbWin)
A_ArbWind_t	*arbWin;
{
	float	value;
	char	*strng;

	strng = gr_DialogGetValue(arbWin->zpercentDialog);
	sscanf(strng,"%f",&value);

	return((double)value);
}


/*
 *	Unset interpolation flag
 */
void
gr_ArbSetNonInterp(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

    arbWin->interp = FALSE;

    return;
}


/*
 *	Set interpolation flag
 */
void
gr_ArbSetInterp(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

    arbWin->interp = TRUE;

    return;
}


/*
 *	Set to use Disk
 */
void
gr_ArbUseDisk(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

    arbWin->useDisk = TRUE;

    return;
}


/*
 *	Set to use memory
 */
void
gr_ArbUseMemory(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

    arbWin->useDisk = FALSE;

    return;
}


/*
 *	Set to use pixmap
 */
void
gr_ArbUsePixmap(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

    arbWin->usePixmap = TRUE;

    return;
}


/*
 *	Set to use ximage
 */
void
gr_ArbUseXImage(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

    arbWin->usePixmap = FALSE;

    return;
}


/*
 *	Open an animation window for arbitrary slicing
 */
void
gr_ArbAniOpen(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;
	A_AniWind_t	*tmp=arbWin->aniWin;
	int	scale,numframes;
	double	zincr;

	scale = gr_ArbgetCurScale(arbWin);
	numframes = gr_ArbgetCurNumFrames(arbWin);
	zincr = gr_ArbgetCurZPercent(arbWin);

    if ((arbWin->numAniWins < MAX_ANILEV_WINDS) &&
		(arbWin->xybox->cutVList != NULL))
    {
		if (arbWin->useDisk == TRUE)
			gr_SaveDiskAAni(arbWin->shell,(caddr_t)arbWin,NULL);
		else
		{
        	tmp = gr_InitAniLevel3(tmp,"XTDwindow",gr_topLevel,
				numframes,zincr,scale,arbWin,NULL);

			if (tmp != NULL)
			{
				arbWin->aniWin = tmp;
        		arbWin->numAniWins++;
			}
		}
    }
    else
	{
		if (arbWin->numAniWins > MAX_ANILEV_WINDS)
		{
			sprintf(msg,"Only %d Animation windows are allowed!\n",
				MAX_ANILEV_WINDS); 
        	gr_TextMsgOut(msg);
		}
		else
			gr_TextMsgOut("It works better if you specify a cutting plane!\n");
	}
}


/*
 *	Open a view window for arbitrary slicing
 */
void
gr_ArbViewOpen(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;
	A_ViewWind_t *tmp=arbWin->viewWin;
	int	scale;

	scale = gr_ArbgetCurScale(arbWin);

    if ((arbWin->numViewWins < MAX_VIEWLEV_WINDS) &&
		(arbWin->xybox->cutVList != NULL))
    {
        tmp = gr_InitViewLevel(tmp,"XTDwindow",gr_topLevel,
				scale,arbWin);

		if (tmp != NULL)
		{
			arbWin->viewWin = tmp;
        	arbWin->numViewWins++;
		}
    }
    else
	{
		if (arbWin->numViewWins > MAX_VIEWLEV_WINDS)
		{
			sprintf(msg,"Only %d View windows are allowed!\n",
				MAX_VIEWLEV_WINDS); 
        	gr_TextMsgOut(msg);
		}
		else
			gr_TextMsgOut("Please select a valid cutting plane first!\n");
	}
}


/*
 *	Rotate cube about the X-Z axis when X-Slider is selected
 */
void
gr_ArbXSliderSel(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

	gr_SliderSetValue(w,(int)call_data);

	gr_BoxRotY(arbWin->xybox,(int)call_data);
	gr_BoxRotate(arbWin->xybox);
	gr_BoxSlice(arbWin->xybox,arbWin->xybox->indepth);
    gr_BoxDrawXY(arbWin->drawCubeWin,arbWin->xybox);
	gr_BoxDrawZY(arbWin->drawDepthWin,arbWin->xybox);

	return;
}


/*
 *	Rotate cube about the X-Z axis when X-Slider is moved
 */
void
gr_ArbXSliderMov(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

	gr_BoxRotY(arbWin->xybox,(int)call_data);
	gr_BoxRotate(arbWin->xybox);
	gr_BoxSlice(arbWin->xybox, arbWin->xybox->indepth);
    gr_BoxDrawXY(arbWin->drawCubeWin,arbWin->xybox);
	gr_BoxDrawZY(arbWin->drawDepthWin,arbWin->xybox);
	return;
}


/*
 *	Rotate cube about the X-Z axis when X-Slider is released
 */
void
gr_ArbXSliderRel(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	return;
}


/*
 *	Rotate cube about the Y-Z axis when Y-Slider is selected
 */
void
gr_ArbYSliderSel(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

	gr_SliderSetValue(w,(int)call_data);

	gr_BoxRotX(arbWin->xybox,(int)call_data);
	gr_BoxRotate(arbWin->xybox);
	gr_BoxSlice(arbWin->xybox,arbWin->xybox->indepth);
    gr_BoxDrawXY(arbWin->drawCubeWin,arbWin->xybox);
	gr_BoxDrawZY(arbWin->drawDepthWin,arbWin->xybox);
	return;
}


/*
 *	Rotate cube about the Y-Z axis when Y-Slider is moved
 */
void
gr_ArbYSliderMov(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

	gr_BoxRotX(arbWin->xybox,(int)call_data);
	gr_BoxRotate(arbWin->xybox);
	gr_BoxSlice(arbWin->xybox,arbWin->xybox->indepth);
    gr_BoxDrawXY(arbWin->drawCubeWin,arbWin->xybox);
	gr_BoxDrawZY(arbWin->drawDepthWin,arbWin->xybox);
	return;
}


/*
 *	Rotate cube about the Y-Z axis when Y-Slider is released
 */
void
gr_ArbYSliderRel(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	return;
}


/*
 *	Move selected slice when Z-Slider is selected
 */
void
gr_ArbZSliderSel(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;
	int		sliceAt;

	gr_SliderSetValue(w,(int)call_data);
	sliceAt = 100-(int)call_data;
	gr_BoxSlice(arbWin->xybox,sliceAt);
    gr_BoxDrawXY(arbWin->drawCubeWin,arbWin->xybox);
	gr_BoxDrawZY(arbWin->drawDepthWin,arbWin->xybox);
	return;
}


/*
 *	Move selected slice when Z-Slider is moved
 */
void
gr_ArbZSliderMov(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;
	int	sliceAt;

	sliceAt = 100-(int)call_data;
	gr_BoxSlice(arbWin->xybox,sliceAt);
    gr_BoxDrawXY(arbWin->drawCubeWin,arbWin->xybox);
	gr_BoxDrawZY(arbWin->drawDepthWin,arbWin->xybox);
	return;
}


/*
 *	Move selected slice when Z-Slider is released
 */
void
gr_ArbZSliderRel(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	return;
}


/*
 *	Redraw Front cube on Expose event
 */
void
gr_ArbCubeExpose(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

	gr_BoxDrawXY(arbWin->drawCubeWin,arbWin->xybox);

	return;
}


/*
 *	Redraw Left cube  on Expose event
 */
void
gr_ArbDepthExpose(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

	gr_BoxDrawZY(arbWin->drawDepthWin,arbWin->xybox);

	return;
}
