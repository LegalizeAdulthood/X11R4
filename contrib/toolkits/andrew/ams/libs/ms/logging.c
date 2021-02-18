/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/logging.c,v 2.5 89/02/10 09:13:32 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/logging.c,v $
*/
#include <stdio.h>
#include <ms.h>
#include <sys/stat.h>
#include <andrewos.h>

extern FILE *fopen();
extern int IsLoggingMailStats;
extern char MAILLOGSTATFILE[];
extern char home[], Me[], *rindex();

ConsiderLoggingRead(FileName)
char *FileName;
{
    char LineBuf[1000], *s;
    int size = 0;
    FILE *fp, *logfp;
    struct stat statbuf;

    if (!IsLoggingMailStats) return;
    logfp = fopen(MAILLOGSTATFILE, "a");
    if (!logfp) return;
    fchmod(fileno(logfp), 0600);

    fp = fopen(FileName, "r");
    if (!fp) {
	fclose(logfp);
	return;
    }
    if (fstat(fileno(fp), &statbuf)) {
	fclose(logfp);
	return;
    }
    fputc('\n', logfp);
    while (fgets(LineBuf, sizeof(LineBuf), fp) && LineBuf[0] != '\n') {
	fputs(LineBuf, logfp);
	size += strlen(LineBuf);
    }
    fclose(fp);
    s = rindex(FileName, '/');
    if (s) *s = '\0';
    fprintf(logfp, "X-StatTrace: %s READ %d bytes %s %s ; %s", Me, statbuf.st_size - size, strncmp(FileName, home, strlen(home)) ? "BBOARD" : "MAIL", FileName, arpadate());
    if (s) *s = '/';
    fclose(logfp);
}
