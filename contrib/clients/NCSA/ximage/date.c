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

#ifndef VERSION
#define VERSION	"NCSA X Image Version 1.0.3"
#endif

#ifndef VERSIONDATE
#define VERSIONDATE "11/29/89"
#endif

static char vinfo[80];
static char vinfo2[80];

char *GetVersionDate()
{

vinfo2[0]='(';
vinfo2[1]='\0';
#ifdef SYSV
	strcat(vinfo2,"System V");
#endif
#ifdef UNICOS
	strcat(vinfo2," UNICOS");
#endif
	strcat(vinfo2,")");

	if (vinfo2[1]==')')
		sprintf(vinfo,"%s %s\n",VERSION,VERSIONDATE);
	else
		sprintf(vinfo,"%s %s %s\n",VERSION,vinfo2,VERSIONDATE);
	return(vinfo);
}

char *GetVersion()
{
	return(VERSION);
}
