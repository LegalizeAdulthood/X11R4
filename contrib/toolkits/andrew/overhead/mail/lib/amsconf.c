/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* amsconf.c -- Uses the /AndrewSetup file to see if all programs
	in the Andrew Message System should be run from an experimental
	binary directory 
*/

#include <stdio.h>
#include <sys/param.h>
#include <errprntf.h>

#define CALLMEMADAM "__AMS__EXP__"
#define LOOKFOR "AMS-Binaries"

amsconfig(argc, argv, name)
int argc;
char **argv, *name;
{
    char *s, ExpDir[1+MAXPATHLEN], NewName[100];

    CheckAMSConfiguration();
    if (!strncmp(argv[0], CALLMEMADAM, sizeof(CALLMEMADAM) -1)) {
	return(0); /* Do not recurse */
    }
    if ((s = (char *) GetConfiguration(LOOKFOR)) == NULL) {
	return(1);
    }
    if (!*s) {
	return(0);
    }
    /* There really is an entry in the configuration file */
    errprintf(name, ERR_WARNING, 0, 0, "Running experimental %s from %s", name, s);
    sprintf(ExpDir, "%s/%s", s, name);
    sprintf(NewName, "%s%s", CALLMEMADAM, name);
    argv[0] = NewName;
    execv(ExpDir, argv);
    errprintf(name, ERR_WARNING, 0, 0, "Exec failed -- running wrong version");
    return(2);
}

