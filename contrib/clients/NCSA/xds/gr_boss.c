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
 *	File:		gr_boss.c
 *	Contents:	Boss level routines for graphics module
 */

#include "gr_com.h"


/*
 *  Recalculate SDS value ranges before opening any windows
 */
gr_BossCalRanges(bossWin)
A_BossWind_t *bossWin;
{
    A_Data_t *hdf=bossWin->data;
	float32 val;
    char *strng;

	strng = gr_DialogGetValue(bossWin->minDialog);
	sscanf(strng,"%f",&val);
	hdf->min = val;
	strng = gr_DialogGetValue(bossWin->maxDialog);
	sscanf(strng,"%f",&val);
	hdf->max = val;
    hdf->range = hdf->max - hdf->min;
	hdf->rangeFrac =
	 (float32)((float32)gr_color.nColors/hdf->range);
	hdf->rangeFracSplit = 
	 (float32)((float32)gr_colorSplit.nColors/hdf->range);
}


/*
 *	Open a cube window
 */
void
gr_BossSetAxes(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_BossWind_t	*bossWin=(A_BossWind_t *)client_data;
	A_CubeWind_t	*tmp=bossWin->cubeWin;

	bossWin->dsplMode = AXES;

	if (bossWin->numCubeWins < MAX_CUBLEV_WINDS)
	{
		gr_BossCalRanges(bossWin);
		tmp = gr_InitCubeLevel(tmp,gr_topLevel,bossWin);

		if (tmp != NULL)
		{
			bossWin->cubeWin = tmp;
			bossWin->numCubeWins++;
		}
	}
	else
	{
		sprintf(msg,"Only %d Axes Mode windows are allowed!\n",
				MAX_CUBLEV_WINDS);
		gr_TextMsgOut(msg);
	}

	return;
}


/*
 *	Open an arbitrary display mode window
 */
void
gr_BossSetArb(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_BossWind_t	*bossWin=(A_BossWind_t *)client_data;
	A_ArbWind_t		*tmp=bossWin->arbWin;

	bossWin->dsplMode = ARBITRARY;

	if (bossWin->numArbWins < MAX_ARBLEV_WINDS)
	{
		gr_BossCalRanges(bossWin);
		tmp = gr_InitArbLevel(tmp,gr_topLevel,bossWin);

		if (tmp != NULL)
		{
			bossWin->arbWin=tmp;
			bossWin->numArbWins++;
		}
	}
	else
	{
		sprintf(msg,"Only %d Arbitrary Mode windows are allowed!\n",
				MAX_ARBLEV_WINDS);
		gr_TextMsgOut(msg);
	}
	return;
}


/*
 *	Open a Dicer window
 */
void
gr_BossSetDicer(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_BossWind_t	*bossWin=(A_BossWind_t *)client_data;
	A_DiceWind_t	*tmp=bossWin->diceWin;
	int				scale;

	bossWin->dsplMode = DICER;
	scale = atoi(gr_DialogGetValue(bossWin->diceScaleDialog));

	if (bossWin->numDiceWins < MAX_DICELEV_WINDS)
	{
		gr_BossCalRanges(bossWin);
		tmp = gr_InitDiceLevel(tmp,gr_topLevel,bossWin,scale);

		if (tmp != NULL)
		{
			bossWin->diceWin=tmp;
			bossWin->numDiceWins++;
		}
	}
	else
	{
		sprintf(msg,"Only %d Dicer Mode window is allowed!\n",
				MAX_DICELEV_WINDS);
		gr_TextMsgOut(msg);
	}
	return;
}


/*
 *	Open an Iso-surface renderer window
 */
void
gr_BossSetIso(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_BossWind_t *bossWin=(A_BossWind_t *)client_data;
	A_IsoWind_t	*tmp=bossWin->isoWin;
	int		scale;

	bossWin->dsplMode = ISO;
	scale = atoi(gr_DialogGetValue(bossWin->diceScaleDialog));

	if (bossWin->numIsoWins < MAX_ISOLEV_WINDS)
	{
		gr_BossCalRanges(bossWin);
		tmp = gr_InitIsoLevel(tmp,gr_topLevel,bossWin,scale);

		if (tmp != NULL)
		{
			bossWin->isoWin=tmp;
			bossWin->numIsoWins++;
		}
	}
	else
	{
		sprintf(msg,"Only %d Iso-surface renderer window is allowed!\n",
				MAX_ISOLEV_WINDS);
		gr_TextMsgOut(msg);
	}
	return;
}
