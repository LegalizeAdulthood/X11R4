/***********************************************************

$Header: WinInfo.h,v 3.0 89/11/20 09:25:47 mikew Exp $

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

typedef struct _winInfo {
    XClassHint      wmClass;
    unsigned char  *wmClientMachine;
    int             argc;
    unsigned char **argv;
    int		    x;
    int             y;
    int		    width;
    int             height;
    int		    win_gravity;
    WM_STATE        wmState;
    Window          rid;
    Window          parentRid;
    Atom           *wmProtocols;
    unsigned long   numProtocols;
    int             saveYourselfState;
    struct _winInfo *next;
    struct _winInfo *prev;
}               WinInfo, *WinInfoPtr;

#define DontSave	1
#define Saving		2
#define Saved		3

extern WinInfo *wiAlloc();
extern Status   wiFree();
extern WinInfo *wiGetByRid();
