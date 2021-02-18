/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* titles.c */

#include <stdio.h>
#include <util.h>

static char *BeginInitialArgv = NULL, *EndInitialArgv;

void SetInitialArgs(argc, argv, envp)
int argc; char **argv, **envp;
{
    int Ix;

    BeginInitialArgv = argv[0];
    Ix = 0;
    if (envp != NULL) {
	for (Ix = 0; Ix < 60; ++Ix) if (envp[Ix] == NULL) break;
    }
    if (Ix > 0) {
	EndInitialArgv = envp[Ix-1] + strlen(envp[Ix-1]);
    } else {
	EndInitialArgv = argv[argc-1] + strlen(argv[argc-1]);
    }
}

/*VARARGS1*/
void SetProcTitle(str, a1, a2, a3, a4, a5)
char *str;
{/* Set the process title. */
    char *cp;
    char Title[1500];

    if (BeginInitialArgv == NULL) return;   /* must call SetInitialArgs first */
    sprintf(Title, str, a1, a2, a3, a4, a5);
    strncpy(BeginInitialArgv, Title, EndInitialArgv - BeginInitialArgv - 1);
    EndInitialArgv[-1] = '\0';
    cp = &BeginInitialArgv[strlen(BeginInitialArgv)];
    while (cp < EndInitialArgv) *cp++ = ' ';
}

#ifdef TESTINGONLYTESTING
main(argc, argv)
int argc; char **argv;
{
    char Str[500];
    char *cp;

    SetInitialArgs(argc, argv, NULL);
    for (;;) {
	fputs("Title for process: ", stdout); fflush(stdout);
	cp = fgets(Str, sizeof(Str), stdin);
	if (cp == NULL) break;
	for (cp = &Str[strlen(Str)-1]; cp >= Str; --cp) if (*cp == '\n') *cp = '\0';
	SetProcTitle("%s", Str);
    }
    exit(0);
}
#endif /* TESTINGONLYTESTING */
