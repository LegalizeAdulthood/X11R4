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
 *	File:		td_inc.h
 *	Contents:	Header file containing all the includes and externs
 *				by the td module.
 */

#include <hdf/df.h>
#include "xds.h"
#include <stdio.h>
#include <X11/Intrinsic.h>

/*
 * Local calls
 */
extern	char	*td_Malloc1D();
extern	char	**td_Malloc2D();
extern	float32	***td_Malloc3Dfloat32();
extern	char	*td_CurrentDate();
extern	int32	td_HdfCheckPlane();
extern  void 	td_Init();
extern	int		td_FileIsHdf();

/*
 * Inter-module calls
 */

extern	void	gr_TextMsgOut();
