/* 
 * getload.c: get load, using rwho statistics - used in xnetload
 *
 * Copyright 1989 University of Waterloo
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of UW not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  UW makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * UW DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL UW
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Mike Berkley, jmberkley@watnext.waterloo.edu, 1989
 *
 */

/* getload includes - no need for X stuff */
#include <stdio.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/time.h>

/* Dynix doesn't have an rwhod.h file, so here's the struct for that system */
#ifdef sequent
struct  outmp {
    char  out_line[8];    /* tty name */
    char  out_name[8];    /* user id */
    long  out_time;       /* time on */
};
struct  whod {
    char  wd_vers;
    char  wd_type;
    char  wd_fill[2];
    int   wd_sendtime;
    int   wd_recvtime;
    char  wd_hostname[32];
    int   wd_loadav[3];
    int   wd_boottime;
    struct whoent {
        struct outmp    we_utmp;
        int             we_idle;
    } wd_we[1024 / sizeof (struct whoent)];
};
#else
#include <protocols/rwhod.h>
#endif

extern time_t time();

#define DOWNTIME (60 * 5)
#define ISDOWN(now,wd) (((now) - (wd).wd_recvtime) > DOWNTIME)
#define RWHODHD "/usr/spool/rwho/whod"

/*
 * actual function to get the load
 * reads rwhod stats to find the load average of a machine
 */
int
getload(hostname)
char *hostname;
{
    int nbytes;			/* number of bytes read */
    int fd;			/* file descriptor */
    int counter;		/* loop counter */
    time_t currtime;		/* current time */
    struct whod    wd;
    char buf[sizeof(struct whod)], fname[256];

    sprintf(fname,"%s.%s",RWHODHD,(char *)hostname);

    (void) time(&currtime);

    /* Loop 3 times, looking for a non-zero load. */
    /* This keeps us from getting a zero load if rwhod is in */
    /* the process of doing something to the file.  This is */
    /* important, since we'll get strange looking zero gaps */
    /* in the graph if we don't try our best to get a non-zero load. */
    for(counter=0;counter<=3;counter++) {
        /* open and read from the file */
        if((fd = open(fname, O_RDONLY, 0)) < 0)
          /* we can't open file, so it's down or something */
          return(-1);

        nbytes = read(fd, buf, sizeof(struct whod));
        close(fd);
        /* if we got enough bytes, then copy it to wd */
        if(nbytes >= (sizeof (wd) - sizeof(wd.wd_we))) {

            /* NOT everbody has memcpy, but can't use strncpy() */
            memcpy(&wd,buf,(sizeof (wd) - sizeof(wd.wd_we)));

            /* ISDOWN macro, if machine is really down or faking */
            if(ISDOWN(currtime,wd))
              return(-1);
            else if(wd.wd_loadav[0] > 0)
              return(wd.wd_loadav[0]);
        }

        /* If we got to here, then we need to loop again */
        sleep(1);
    }
    return(0);		/* couldn't read load if here */
}
