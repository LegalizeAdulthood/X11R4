/*
 * Warning, this library is a crock and will only work for single display
 * applications.
 */

/*
 *   Copyright (c) 1987, 88 by
 *   PARALLAX GRAPHICS, INCORPORATED, Santa Clara, California.
 *   All rights reserved
 *
 *   This software is furnished on an as-is basis, and may be used and copied
 *   only with the inclusion of the above copyright notice.
 *
 *   The information in this software is subject to change without notice.
 *   No committment is made as to the usability or reliability of this
 *   software.
 *
 *   Parallax Graphics, Inc.
 *   2500 Condensa Street
 *   Santa Clara, California  95051
 */

#ifndef lint
static char *sid_video_c = "@(#)XPlxVideo.c	1.6 9/2/88 Parallax Graphics Inc";
#endif

/*
 * XPlxVideo.c
 *
 */

#include	"Xlibint.h"
#include	"plxvideo.h"

static int VideoReqCode = 0;
static int first_event, first_error;

/*
 * set a drawable to be of type video
 */

#define	PREAMBLE	\
		LockDisplay(dpy); \
		FlushGC(dpy, gc); \
		if (!VideoReqCode) { \
			XQueryExtension(dpy, VIDEONAME, &VideoReqCode, \
						&first_event, &first_error); \
		}

#define POSTAMBLE	\
		UnlockDisplay(dpy); \
		SyncHandle();

XPlxVideoSet(dpy, d, gc)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoSet;

	POSTAMBLE
}

XPlxVideoLive(dpy, d, gc, vx, vy, x, y, w, h)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;
	xVideoArgs xva;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoLive;

	xva.vx = vx;
	xva.vy = vy;
	xva.x = x;
	xva.y = y;
	xva.w = w;
	xva.h = h;

	req->length += sizeof(xVideoArgs) / 4;
	PackData(dpy, (char *)&xva, sizeof(xVideoArgs));

	POSTAMBLE
}

XPlxVideoStill(dpy, d, gc, vx, vy, x, y, w, h)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;
	xVideoArgs xva;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoStill;

	xva.vx = vx;
	xva.vy = vy;
	xva.x = x;
	xva.y = y;
	xva.w = w;
	xva.h = h;

	req->length += sizeof(xVideoArgs) / 4;
	PackData(dpy, (char *)&xva, sizeof(xVideoArgs));

	POSTAMBLE
}

XPlxVideoScale(dpy, d, gc, vx, vy, vw, vh, x, y, w, h)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;
	xVideoArgs xva;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoScale;

	xva.vx = vx;
	xva.vy = vy;
	xva.vw = vw;
	xva.vh = vh;
	xva.x = x;
	xva.y = y;
	xva.w = w;
	xva.h = h;

	req->length += sizeof(xVideoArgs) / 4;
	PackData(dpy, (char *)&xva, sizeof(xVideoArgs));

	POSTAMBLE
}

XPlxVideoStop(dpy, d, gc)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoStop;

	POSTAMBLE
}

XPlxVideoPriority(dpy, d, gc, pri)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;
	xVideoArgs xva;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoPriority;

	xva.xva_arg1 = pri;

	req->length += sizeof(xVideoArgs) / 4;
	PackData(dpy, (char *)&xva, sizeof(xVideoArgs));

	POSTAMBLE
}

XPlxVideoFrameMode(dpy, d, gc, onoff)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;
	xVideoArgs xva;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoFrameMode;

	xva.xva_arg1 = onoff;

	req->length += sizeof(xVideoArgs) / 4;
	PackData(dpy, (char *)&xva, sizeof(xVideoArgs));

	POSTAMBLE
}

XPlxVideoFrameStart(dpy, d, gc, type)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;
	xVideoArgs xva;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoFrameStart;

	xva.xva_arg1 = type;

	req->length += sizeof(xVideoArgs) / 4;
	PackData(dpy, (char *)&xva, sizeof(xVideoArgs));

	POSTAMBLE
}

XPlxVideoDitherControl(dpy, d, gc, luma, chroma)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;
	xVideoArgs xva;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoDitherControl;

	xva.xva_arg1 = luma;
	xva.xva_arg2 = chroma;

	req->length += sizeof(xVideoArgs) / 4;
	PackData(dpy, (char *)&xva, sizeof(xVideoArgs));

	POSTAMBLE
}

XPlxVideoInput(dpy, d, gc, channel)
register Display *dpy;
Drawable d;
GC gc;
{
	register xVideoReq *req;
	xVideoArgs xva;

	PREAMBLE

	GetReq(Video, req);
	req->drawable = d;
	req->gc = gc->gid;
	req->reqType = VideoReqCode;
	req->videoReqType = X_VideoInput;

	xva.xva_arg1 = channel;

	req->length += sizeof(xVideoArgs) / 4;
	PackData(dpy, (char *)&xva, sizeof(xVideoArgs));

	POSTAMBLE
}
