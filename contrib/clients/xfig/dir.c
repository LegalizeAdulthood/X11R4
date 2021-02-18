/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	November 1985.
 *	1st revision: Mar 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"

extern char		*sys_errlist[];
extern int		sys_nerr, errno;

extern char		directory[];

int
change_directory(path)
char	*path;
{
	if (path == NULL) return(0);
	put_msg("Changing . . .");
	if (chdir(path) == -1) {
	    put_err("Can not change to directory %s, : %s",
		path, sys_errlist[errno]);
	    }
	else {
	    print_directory();
	    }
	return(1);
	}

print_directory()
{
	extern char	*getwd();

	if (*directory == '\0') {
	    if (NULL == getwd(directory)) {
		put_msg("%s", directory);       /* err msg is in directory */
		*directory = '\0';
		return;
		}
	    else
		put_msg(directory);
	    }
	else
	    put_msg(directory);
	}
