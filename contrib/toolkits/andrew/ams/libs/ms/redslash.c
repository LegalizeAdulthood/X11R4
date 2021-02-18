/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/redslash.c,v 2.3 89/02/08 15:44:55 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/redslash.c,v $
*/
ReduceSlashes(filename)
char *filename;
{
    char *old = filename, *new = filename;
    int AtSlash = 0;

    while (*old) {
	switch (*old) {
	    case '/':
		if (AtSlash) {
		    break;
		}
		AtSlash = 1;
		*new++ = *old;
		break;
	    default:
		AtSlash = 0;
		*new++ = *old;
		break;
	}
	++old;
    }
    *new = '\0';
}    

