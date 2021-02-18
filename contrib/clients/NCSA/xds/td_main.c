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
 *	File:		td_main.c
 *	Contents:	Main initialization/termination routines for td module
 *
 */

#include "td_com.h"

extern void td_Init();


/*
 *	Entry point into X DataSlice
 */
main(argc,argv)
int 	argc;
char	*argv[];
{
	td_Init(argc,argv);

	gr_ProcessLoop();

}


/*
 *	Initialize X DataSlice
 */
void
td_Init(argc,argv)
int		argc;
char	*argv[];
{
	extern char *getcwd();

	td_version = 1.1;
	td_release = 0;
	if ((td_dirName = getcwd((char *)NULL,STRNG160)) == NULL)
	{
		gr_TextMsgOut("WARNING: Cannot get current directory pathname\n");
		td_dirName = td_dirDefName;
	};

	td_FileInitMsg();

	gr_Init(argc,argv);
}


/*
 *	Terminate X DataSlice
 */
void
td_Terminate()
{
	exit(0);
}
