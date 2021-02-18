/* This file is part of xdir, an X-based directory browser.
 * It contains routines for dealing with actual directories.
 *
 *	Created: 8/13/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/dir.c,v $
 *	$Athena: dir.c,v 1.1 88/09/16 01:29:42 treese Locked $
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_xdir_c[] = "$Header: dir.c,v 2.1 89/03/25 02:48:06 treese Exp $";
#endif

#include <sys/types.h>
#include <sys/dir.h>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>

#include "xdir.h"

#define NENTRIES	20

/* Function:	SPComp() compares two string pointers for qsort().
 * Arguments:	s1, s2: strings to be compared.
 * Returns:	Value of strcmp().
 * Notes:
 */

static int
SPComp(s1, s2)
char **s1, **s2;
{
	return(strcmp(*s1, *s2));
}

char **
MakeFileList(dir_name)
char *dir_name;
{
	DIR *dirp;
	struct direct *dp;
	char **filelist;
	char **cur_file;
	char **last_file;
	int entry_cnt;

	entry_cnt = NENTRIES;
	filelist = (char **) calloc(entry_cnt, sizeof (char *));
	cur_file = filelist;
	last_file = filelist + entry_cnt - 1;

	dirp = opendir(dir_name);
	if (dirp == NULL) {
		return(NULL);
	}
	for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)) {
		if ( !strcmp(dp->d_name, ".") )
			continue;
		if ( (! ShowDotFiles) && (dp->d_name[0] == '.'))
			continue;
		if ( (! ShowBackupFiles) &&
		    (dp->d_name[dp->d_namlen-1] == TWIDDLE))
			continue;
		*cur_file++ = SaveString(dp->d_name);
		if (cur_file == last_file) {
			filelist = (char **) realloc(filelist,
					    2 * entry_cnt * sizeof (char *));
			cur_file = filelist + entry_cnt - 1;
			entry_cnt = 2 * entry_cnt;
			last_file = filelist + entry_cnt - 1;
		}
	}
	*cur_file = NULL;
	qsort(filelist, cur_file - filelist - 1, sizeof (char *), SPComp);
	return(filelist);
}

/* Function:	PrintFileList() prints a list of files from an array.
 * Arguments:	file_list:	List to print.
 * Returns:	Nothing.
 * Notes:
 */

void
PrintFileList(file_list)
char **file_list;
{
	if (file_list == NULL)
		return;
	while(*file_list != NULL)
		puts(*file_list++);
}
