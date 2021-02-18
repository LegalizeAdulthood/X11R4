/* ********************************************************************** *\
  *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
  *        For full copyright information see:'andrew/config/COPYRITE'     *
  \* ********************************************************************** */
/*
  usysex.c -- Return a static string describing a sysexits value.
  */

#include <errno.h>
#include <andrewos.h>
#ifdef HAS_SYSEXITS
#include <sysexits.h>
#endif /* HAS_SYSEXITS */

char *UnixSysExits(exitNumber)
int	exitNumber;
{
    /* Returns a pointer to a static buffer containing English text describing the condition that exitNumber describes (interpreted as a sysexits.h value).  The text has no newlines in it. */
    static char ExitBuff[40];
    extern int EX_Nerr;
    extern char *EX_Messages[];

    if (exitNumber == EX_OK) return "OK";
    if (exitNumber >= EX__BASE && exitNumber < (EX__BASE + EX_Nerr))
	return EX_Messages[exitNumber - EX__BASE];
    sprintf(ExitBuff, "Exit number %d", exitNumber);
    return ExitBuff;
}
