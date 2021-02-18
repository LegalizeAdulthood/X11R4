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

#include <stdio.h>
#include <X11/Xos.h>
/*#include <sys/types.h>*/

#ifdef SYSV
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

#define MAXENTRY	1000
#define	MALLOC		(char *) malloc


static void BubbleSort(list,num)
char *list[];
int num;
{
char *tmp;
int x,y;
	for(x=0; x< num; x++)
		for(y=0; y < num-x-1; y++)
			if (strcmp(list[y],list[y+1]) > 0) {
				tmp = list[y+1];
				list[y+1] = list[y];
				list[y] = tmp;
				}
	return;
} /* BubbleSort() */



int Glob(dir,key,nargc,rargv)
/* expand a wild card.  Can only handle an '*' at the end of filename*/
/* didn't have time to write a decent globbing routine  :-) */
/* RETURNS 0 if can't open directory */

char	*dir;	  /*string containing directory name */
char	*key;	/*search key('*' at end of filename is only wildcard excepted*/
int	*nargc;   /* RETURNED: number of matches found */
char	***rargv; /* RETURNED: expanded file name strings */
{
char	**tmpargv;
char	**nargv;
int	numFiles=0;
int	astPos;
int	space = 0;
int	x;
char	*buff;
#ifdef SYSV
struct dirent *files[MAXENTRY];
struct dirent **tfiles;
#else
struct direct *files[MAXENTRY];
struct direct **tfiles;
#endif

	if (!(numFiles = GetDir(dir,files)))
		return(0); /* couldn't open directory */

	if (!(astPos=FindGlobChar(key))) {
		/* no wild cards, so return key */
		*nargc=1;
		*rargv = (char **) MALLOC( sizeof(char *) );
		**rargv = key;
		return(1);
		}

	tfiles = files;
	*nargc = 0;
	for (x=0 ;x < numFiles; x++) {
		if (!strncmp(files[x]->d_name,key,astPos)) {
			(*nargc)++;
			space = space + strlen(files[x]->d_name) + 1;
			}
		else {
			tfiles++;
			}
		}

	tmpargv = nargv = (char **) MALLOC(*nargc * sizeof(char *));
	buff = MALLOC(space);
	for (x=0 ;x < numFiles; x++) {
		if (!strncmp(files[x]->d_name,key,astPos)) {
			strcpy(buff,files[x]->d_name);
			*tmpargv = buff;
			tmpargv++;
			buff = buff + strlen(buff) + 1;
			}
		}
	BubbleSort(nargv,*nargc);

	*rargv = nargv;
	return(1);

} /* Glob() */


static int FindGlobChar(key)
/* What character position is the '*' in the string? */
/* return 0 if no '*' */
char *key;
{
register int	count = 0;

/*	while(*key) {
		if (*key == '*')
			return(count);
		key++;
		count++;
		}
*/
	for (count = 0;(*key)&&(*key != '*');key++,count++);

	if (! *key)
		return(0); /* no '*' found */

	return(count);

} /* FindGlobChar() */


static int GetDir(theDir,files)
/* do an `ls` */
char *theDir; 			/* a string containing the directory name */
#ifdef SYSV
struct dirent *files[];		/* RETURNED: list of files in directory */
#else
struct direct *files[];
#endif
{
int count;
DIR *newdirFD;
#ifdef SYSV
struct dirent *dirInfo;
#else
struct direct *dirInfo;
#endif

        if (!(newdirFD = opendir(theDir))) {
                printf("Can't open directory %s\n",theDir);
                return(NULL);
                }
        count = 0;
        while ((files[count] = readdir(newdirFD))&& (count < MAXENTRY)) {
			/* no files that start with a '.' */
			if (files[count]->d_name[0] != '.')
                        	count++;
                        }
        closedir(newdirFD);
        return(count);
} /* GetDir() */
