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
 *	File:		td_file.c
 *	Contents:	Unix file routines
 *
 */

#include "td_com.h"
#include <sys/stat.h>

#ifdef SYSV
#include <dirent.h>
#else
#ifdef UNICOS
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#endif

#define MAXDIRENTRIES	300


static struct	stat  buf;

/*
 *	Set Current working directory
 */
void
td_setDirName(dirName)
char *dirName;
{
	strcpy(td_dirName,dirName);
}


/*
 * Return nonzero if pathname is a directory or a link
 */
int
td_FileIsDir(pathname)
char *pathname;
{
	if (stat(pathname,&buf) == 0)
	{
		if (buf.st_mode & S_IFDIR)
			return(1);
	}
	return(0);
}


/* 
	Returns a pointer to a list of character strings of the filenames in
	the directory given by the pathname dirName.
*/
char
**td_FileDirStrings(dirName)
char *dirName;
{
	DIR 		  *dirp;
#ifdef SYSV
	struct dirent *dp;
#else
#ifdef UNICOS
	struct dirent *dp;
#else
	struct direct *dp;
#endif
#endif
	short		  i =0,j,k,changed=1;
	char   		  **strngs,*strng;
	extern	int	  strcmp();

	dirp = opendir(dirName);
	
	if (dirp == NULL)
		return(NULL);
	else
	{
		if ((strngs = 
			td_Malloc2D((int)MAXDIRENTRIES,(int)1,(long)sizeof(char *),
			"td_Malloc2D")) == NULL)
			return(NULL);

		for (dp=readdir(dirp); (dp!=NULL) && (i<MAXDIRENTRIES);
			 dp=readdir(dirp))
			if (dp->d_name[0] != '.')
			{

				if ((strngs[i] = td_Malloc1D((unsigned)(strlen(dp->d_name)+2),
					1,(unsigned)sizeof(char),"Mallocing Strings")) != NULL)
				{
					strcpy(strngs[i],dp->d_name);
					sprintf(msg,"%s/%s",dirName,strngs[i]);
					if (td_FileIsDir(msg))
						sprintf(strngs[i],"%s/",strngs[i]);
					else
					if (td_FileIsHdf(msg))
						sprintf(strngs[i],"%s*",strngs[i]);
					else
						sprintf(strngs[i],"%s ",strngs[i]);
					i++;
				}
			}
		close(dirp);

		if (dp != NULL)
		{
			sprintf(msg,
			"WARNING: Only %d filenames are displayed due to memory limits.\n",
			MAXDIRENTRIES);
			gr_TextMsgOut(msg);
		}

		strngs[i] = NULL;

		/* When no items in directory */
		if (i == 0)
		{
			if ((strngs[0] = td_Malloc1D((unsigned)10,
				1,(unsigned)sizeof(char),"Mallocing Strings")) != NULL)
			sprintf(strngs[0],"     ");
			strngs[1] = NULL;
			return(strngs);
		}
		/* Sort strings */
		j=i-1;
		while ((j>0) && (changed == 1))
		{
			changed = 0;
			for (k=0;k<j;k++)
			{
				if (strcmp(strngs[k],strngs[k+1]) > 0)
				{
					strng = strngs[k];
					strngs[k] = strngs[k+1];
					strngs[k+1] = strng;
					changed = 1;
				}
			}
			j--;
		}

		return(strngs);
	}

}


/*
 *	Opens the td_logFile and initialize it with the current time.
 *	Set the mode to all read/write.
 */
void
td_FileInitMsg()
{
	FILE *fp;

	if ((fp = fopen(td_logFile,"w")) != NULL)
	{
#ifdef UNICOS
		sprintf(msg,
"Welcome to %s <V%3.1f.%d %s>!\nCray Unicos client compiled on %s\nStart at %s",
		td_toolName,td_version,td_release,td_relType,COMPILEDATE,td_CurrentDate());
#else
#ifdef IRIS
		sprintf(msg,
"Welcome to %s <V%3.1f.%d %s>!\nIRIS IRIX client compiled on %s\nStart at %s",
		td_toolName,td_version,td_release,td_relType,COMPILEDATE,td_CurrentDate());
#else
#ifdef AIX
		sprintf(msg,
"Welcome to %s <V%3.1f.%d %s>!\nIBM AIX client compiled on %s\nStart at %s",
		td_toolName,td_version,td_release,td_relType,COMPILEDATE,td_CurrentDate());
#else
#ifdef SUN3
		sprintf(msg,
"Welcome to %s <V%3.1f.%d %s>!\nSUN3 OS client compiled on %s\nStart at %s",
		td_toolName,td_version,td_release,td_relType,COMPILEDATE,td_CurrentDate());
#else
#ifdef SUN386
		sprintf(msg,
"Welcome to %s <V%3.1f.%d %s>!\nSUN386i OS client compiled on %s\nStart at %s",
		td_toolName,td_version,td_release,td_relType,COMPILEDATE,td_CurrentDate());
#else
#ifdef SUNSPARC
		sprintf(msg,
"Welcome to %s <V%3.1f.%d %s>!\nSUN SparcStation I client compiled on %s\nStart at %s",
		td_toolName,td_version,td_release,td_relType,COMPILEDATE,td_CurrentDate());
#else
#ifdef TEKXD88
		sprintf(msg,
"Welcome to %s <V%3.1f.%d %s>!\nTektronics XD88 client compiled on %s\nStart at %s",
		td_toolName,td_version,td_release,td_relType,COMPILEDATE,td_CurrentDate());
#else
		sprintf(msg,
"Welcome to %s <V%3.1f.%d %s>!\nClient compiled on %s\nStart at %s",
		td_toolName,td_version,td_release,td_relType,COMPILEDATE,td_CurrentDate());
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#ifdef OPEN_WINDOWS
		sprintf(msg,"%sThis executable is OpenWindows compliant.\n",msg);
#else
		sprintf(msg,"%s\n",msg);
#endif

		gr_TextInitBuff((long)strlen(msg));
		fprintf(fp,"%s",msg);
		fclose(fp);
		chmod(td_logFile,0000700|0000070|0000007);
	}
	else
	{
		fprintf(stderr,"Error\t: Cannot open log file %s\n",td_logFile);
		exit(1);
	}
}


/*
 *	Return 1 if file exists, else return 0
 */
int
td_FileExist(pathname)
char *pathname;
{
	FILE	*fp;

	if ((fp=fopen(pathname,"r"))!=NULL)
	{
		fclose(fp);
		return(1);
	}
	else
		return(0);
}
