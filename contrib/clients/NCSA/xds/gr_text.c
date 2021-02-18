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
 *	File:		gr_text.c
 *	Contents:	Text manipulation routines
 */

#include "gr_com.h"

long	gr_textBuffLength;


/*
 *	Initialize text buffer
 */
void
gr_TextInitBuff(len)
long len;
{
	gr_textBuffLength = len;
}


/*
 *	Write message out to text file
 */
void
gr_TextMsgOut(strng)
char *strng;
{
	gr_TextReplace(gr_topWin.msgWin,gr_textBuffLength,gr_textBuffLength,strng);
	gr_textBuffLength += (long)strlen(strng);
	gr_TextSetInsertionPoint(gr_topWin.msgWin,gr_textBuffLength);
}
