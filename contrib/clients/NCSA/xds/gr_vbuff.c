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
 *	File:		gr_vbuff.c
 *	Contents:	routines for V-buffer
 */

#include "gr_com.h"
#include <math.h>

double
gr_VbuffgetIntensity(subsWin,val)
A_SubsWind_t *subsWin;
float32	val;
{
	return(pow((double)((val-subsWin->hdf->min)/subsWin->hdf->range),
			subsWin->gamma));
}

double
gr_VbuffgetOpacity(subsWin,val)
A_SubsWind_t *subsWin;
double	  val;
{
	int i;

	for (i=0;i<subsWin->numSubs;i++)
		if ((val > subsWin->subs[i].lower) &&
			(val < subsWin->subs[i].upper))
			return(subsWin->subs[i].opacity);

	return((double)0.0);
}

double
gr_VbuffgetAtten(subsWin,val)
A_SubsWind_t *subsWin;
double val;
{
	return(subsWin->depthcueSquared/pow((val+subsWin->depthcue),2.0));
}

void
gr_VbuffIntegrate(subsWin,intensity,opacity,atten,x,y)
A_SubsWind_t *subsWin;
double	intensity,opacity,atten;
int	x,y;
{
	if ((subsWin->vbuff[x][y].opacity < (double)1.0) &&
		(opacity != (double)0.0))
	{

#ifdef XDEBUG
printf("%d,%d => intensity=%lf, opacity=%lf, atten=%lf\n",x,y,intensity,
opacity,atten);
#endif
		subsWin->vbuff[x][y].intensity +=
		(intensity+subsWin->amb)*opacity*atten*
		((double)1.0 - subsWin->vbuff[x][y].opacity);

		if (subsWin->vbuff[x][y].intensity > subsWin->outMax)
			subsWin->outMax = subsWin->vbuff[x][y].intensity;

		subsWin->vbuff[x][y].opacity +=
		(opacity - (subsWin->vbuff[x][y].opacity * opacity));

	}
}
