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
 *	File:		td_port.c
 *	Contents:	Ports of variables outside td module
 *
 */

#include "td_com.h"

double
td_getVersion()
{
	return(td_version);
}

char
*td_getToolName()
{
	return(td_toolName);
}

char
*td_getDirName()
{
	return(td_dirName);
}

char
*td_getLogFileName()
{
	return(td_logFile);
}

char
*td_CurrentDate()
{
	long lntime;
	char *chtime;

	lntime = time((long *)0);
	chtime = ctime(&lntime);

	return(chtime);
}

long
td_CurrentTime()
{
	return(time((long *)0));
}
