/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*******************************\
*                               *
*  Machine - dependent patches  *
*                               *
\*******************************/

#ifdef SYSV				/* System V */

#define bcopy(source, dest, count) memcpy(dest, source, count)
#define bzero(dest, count) memset(dest, 0, count)
#define bcmp(source, dest, count) memcmp(source, dest, count)

#define DECLARE_strchr extern char *strchr()
#define DECLARE_strrchr extern char *strrchr()

#else /* SYSV				/* BSD */ */

#define DECLARE_strchr extern char *index()
#define strchr(string, char) index(string, char)

#define DECLARE_strrchr extern char *rindex()
#define strrchr(string, char) rindex(string, char)

#endif /* SYSV */
