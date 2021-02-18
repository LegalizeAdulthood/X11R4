/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	sys.c		
	Print name that describes current machine and operating
	system.
*/

#include <system.h>
#include <andrewos.h>

static char rcsid[] = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/sys/RCS/sys.c,v 2.5 89/05/12 07:58:57 ghoti Exp $";


extern char *index();

#define NULL (char *)0

main (argc,argv)
int argc;
char **argv;
{
  char *retval=SYS_NAME, *s;

  while (*++argv!=NULL)
    if (**argv=='-' && (*argv)[1]=='c' && (*argv)[2]=='\0') {
      if ((s=index(retval, '_')) != NULL)
	*s = '\0';
      break;
    }
  printf("%s\n", retval);
  exit(0); /* Don't return bogoid status... -zs */
}

