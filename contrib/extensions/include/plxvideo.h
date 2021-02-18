/*
 * Copyright (c) 1987, 1988 by
 * PARALLAX GRAPHICS, INCORPORATED, Santa Clara, California.
 * All rights reserved
 *
 * This software is furnished on an as-is basis, and may be used and copied
 * only with the inclusion of the above copyright notice.
 *
 * The information in this software is subject to change without notice.
 * No committment is made as to the usability or reliability of this
 * software.
 *
 * Parallax Graphics, Inc.
 * 2500 Condensa Street
 * Santa Clara, California  95051
 */

/* "@(#)plxvideo.h	1.4 08/31/88 Parallax Graphics Inc" */

/*
 * Video request paramaters.
 */

#define X_Video			1

#define X_VideoSet		11
#define X_VideoLive		12
#define X_VideoStill		13
#define X_VideoScale		14
#define X_VideoStop		15
#define X_VideoPriority		16
#define	X_VideoFrameMode	17
#define	X_VideoFrameStart	18
#define	X_VideoDitherControl	19
#define	X_VideoInput		20

typedef struct _Video {
	CARD8 reqType;			/* always VideoReqCode */
	CARD8 videoReqType;		/* one of X_Video_XXXXX */
	CARD16 length B16;
	CARD32 drawable B32;
	CARD32 gc B32;
} xVideoReq;				/* followed by any request paramaters */
#define sz_xVideoReq 12

typedef struct _VideoArgs {
	INT16 vx B16, vy B16, vw B16, vh B16;	/* input video area */
	INT16 x B16, y B16, w B16, h B16;	/* windowing cord's */
} xVideoArgs;
#define sz_xVideoArgs 16

#define	xva_arg1	vx
#define	xva_arg2	vy

#define	VIDEONAME	"ParallaxVideo"
#define	VIDEOPROCEDURES	"ParallaxVideoProcedures"
