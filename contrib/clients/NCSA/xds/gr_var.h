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
 *	File:		gr_var.h
 *	Contents:	Header file containing all global variables used
 *				by the graphics module.
 */

Widget		gr_topLevel;

A_TopWind_t	gr_topWin;

char		*gr_palData;
char		gr_FileName[STRNG160];

A_Color_t	gr_color;

A_Color_t	gr_colorSplit;

static  XtActionsRec gr_ActionsTable[] = {
	{"gr_FileDirReturn", gr_FileDirReturn},
	{"gr_FileSaveReturn", gr_FileSaveReturn},
};

static  char gr_ChangeDirTrans[] = "<Key>Return:    gr_FileDirReturn()";
static  char gr_SaveFileTrans[]  = "<Key>Return:    gr_FileSaveReturn()";
