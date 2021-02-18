/***********************************************************

$Header: State.h,v 3.0 89/11/20 09:25:35 mikew Exp $

Copyright 1989 by Mike Wexler, Santa Clara, Ca.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Mike Wexler or not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

MIKE WEXLER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

typedef struct _hostInfoStruct {
    unsigned char		*hostName; /* name of host */
    int				numWinInfos;	/* number of windows on host */
    WinInfoPtr			*winInfos; /* array of pointers to windows */
    FILE			*outputFile; /* host's output file */
    struct _hostInfoStruct	*next; /* next host in host list */
} 		HostInfo, *HostInfoPtr;

typedef struct _stateStruct {
    WinInfo	*topList;
    HostInfo	*hostInfo;
    int      	waitForCount;
}		State, *StatePtr;

#define HOSTNAME_LEN    100
#define TEMP_LEN	500

extern void     	StateInit();
extern void     	StateCleanup();
extern Status      	StateSave();
extern Status      	StatePrint();
extern Status      	StateRestore();
extern WinInfoPtr	StateGetWinInfoByRid();
