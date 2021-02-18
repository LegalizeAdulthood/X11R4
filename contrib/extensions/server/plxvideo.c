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
 *
 */

#ifndef lint
static char *sid_video_c = "@(#)plxvideo.c	1.5 09/01/88 Parallax Graphics Inc";
#endif

#include <stdio.h>

#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "os.h"
#include "pixmapstr.h"
#include "gcstruct.h"
#include "extnsionst.h"
#include "dixstruct.h"
#include "resource.h"
#include "opaque.h"

#include "plxvideo.h"

int PlxVideoReqCode = 0;
short PlxVideoClass = 0;

int ProcPlxVideoDispatch(), SProcPlxVideoDispatch();
void PlxVideoResetProc();
void PlxVideoValidateGC(), PlxVideoDestroyGC(), PlxVideoDeleteState();

extern void ValidateGC();
extern ExtensionEntry *AddExtension();

/*
 * PlxVideoExtensionInit
 *
 * Called from InitExtensions in main() or from QueryExtension() if the
 * extension is dynamically loaded.
 *
 * VIDEO has no events or errors (other than the core errors)
 */

void
PlxVideoExtensionInit()
{
	register ExtensionEntry *extEntry;

	extEntry = AddExtension(VIDEONAME, 0, 0,
				ProcPlxVideoDispatch,
				SProcPlxVideoDispatch,
				PlxVideoResetProc);
	if (extEntry) {
		PlxVideoReqCode = extEntry->base;
		(void)MakeAtom(VIDEONAME, strlen(VIDEONAME), TRUE);
		PlxVideoClass = CreateNewResourceClass();
	} else {
		FatalError("PlxVideoExtensionInit: AddExtensions failed\n");
	}
}

/*
 * PlxVideoValidateGC
 *
 * Called from ValidateGC.
 *
 */

void
PlxVideoValidateGC(pGC, pGCI, mask, pDrawable)
GCPtr pGC;
GCInterestPtr pGCI;
int mask;
Drawable pDrawable;
{

/*
	fprintf(stderr, "PlxVideoValidateGC() called\n");
	fprintf(stderr, "\tplanemask = 0x%08x\n", GetGCValue(pGC, planemask));
	fprintf(stderr, "\tdepth     = %d\n", GetGCValue(pGC, depth));
	fprintf(stderr, "\tfgPixel   = %d\n", GetGCValue(pGC, fgPixel));
	fprintf(stderr, "\tbgPixel   = %d\n", GetGCValue(pGC, bgPixel));
	switch (GetGCValue(pGC, fillStyle)) {
	case FillSolid:
		fprintf(stderr, "\tfillstyle = FillSolid\n");
		break;
	case FillTiled:
		fprintf(stderr, "\tfillstyle = FillTiled\n");
		break;
	case FillStippled:
		fprintf(stderr, "\tfillstyle = FillStippled\n");
		break;
	case FillOpaqueStippled:
		fprintf(stderr, "\tfillstyle = FillOpaqueStippled\n");
		break;
	}
*/

#ifdef GCExtensionInterest
	pGCI->ValInterestMask &= ~GCExtensionInterest;
#endif
}

void
PlxVideoResetProc()
{
/*
	fprintf(stderr, "PlxVideoResetProc() called\n");
*/
}

void
PlxVideoDestroyGC()
{
/*
	fprintf(stderr, "PlxVideoDestroyGC() called\n");
*/
}

void
PlxVideoDeleteState()
{
/*
	fprintf(stderr, "PlxVideoDeleteState() called\n");
*/
}

/*
 * AddAndInitPlxVideoInterest
 *
 * Sets up at GCInterest ptr and places it on the GC's queue.
 *
 */

#define	VIDEO_INTEREST	( \
				GCFunction | GCPlaneMask |  \
				GCForeground | GCBackground |  \
				GCSubwindowMode | GCGraphicsExposures \
			)

static GCInterestPtr
AddAndInitPlxVideoInterest(pGC)
GCPtr pGC;
{
    register GCInterestPtr pGCI;

    pGCI = (GCInterestPtr)Xalloc(sizeof(GCInterestRec));
    InsertGCI(pGC,pGCI,GCI_LAST,0)

    pGCI->ValInterestMask = VIDEO_INTEREST;
    pGCI->ValidateGC = PlxVideoValidateGC;
    pGCI->DestroyGC = PlxVideoDestroyGC;
    pGCI->ChangeInterestMask = 0x0;
    pGCI->ChangeGC = NULL;
    pGCI->CopyGCSource = NULL;
    pGCI->CopyGCDest = NULL;

    pGCI->extPriv = (pointer)LookupProc(VIDEOPROCEDURES, pGC);

#ifdef GCExtensionInterest
    (* pGCI->ValidateGC)(pGC, pGCI, VIDEO_INTEREST | GCExtensionInterest, NULL);
#else
    (* pGCI->ValidateGC)(pGC, pGCI, VIDEO_INTEREST, NULL);
#endif
    return (pGCI);
}

/*
 * ProcPlxVideoDispatch
 */

int
ProcPlxVideoDispatch(client)
register ClientPtr client;
{
	REQUEST(xReq);

	switch (stuff->data) {
	case X_VideoSet:
	case X_VideoLive:
	case X_VideoStill:
	case X_VideoScale:
	case X_VideoStop:
	case X_VideoFrameMode:
	case X_VideoFrameStart:
	case X_VideoDitherControl:
	case X_VideoPriority:
	case X_VideoInput:
		return(ProcPlxVideo(client));
		/* NOT REACHED */
	default:
		SendErrorToClient(client, PlxVideoReqCode, stuff->data, 0, BadRequest);
		return(BadRequest);
	}
}

int
SProcPlxVideoDispatch(client)
register ClientPtr client;
{
	REQUEST(xReq);

	switch (stuff->data) {
	case X_VideoSet:
	case X_VideoLive:
	case X_VideoStill:
	case X_VideoScale:
	case X_VideoStop:
	case X_VideoFrameMode:
	case X_VideoFrameStart:
	case X_VideoDitherControl:
	case X_VideoPriority:
	case X_VideoInput:
		return(SProcPlxVideo(client));
		/* NOT REACHED */
	default:
		SendErrorToClient(client, PlxVideoReqCode, stuff->data, 0, BadRequest);
		return(BadRequest);
	}
}

/* 
 * Macros needed for byte-swapping, copied from swapreq.c.
 * Really should be in a header file somewhere.
 */

#define	LengthRestS(stuff)	((stuff->length << 1)  - (sizeof(*stuff) >> 1))

#define	SwapRestS(stuff)	SwapShorts(stuff + 1, LengthRestS(stuff))

int
SProcPlxVideo(client)
register ClientPtr client;
{
	register char n;

	REQUEST(xVideoReq);

	swaps(&stuff->length, n);
	swapl(&stuff->drawable, n);
	swapl(&stuff->gc, n);
	SwapRestS(stuff);
	return (ProcPlxVideo(client));
}

/*
 * ProcPlxVideo
 */

static int
ProcPlxVideo(client)
register ClientPtr client;
{
	register GCPtr pGC;
	register DrawablePtr pDraw;
	GCInterestPtr pGCI;
	int (*pPlxVideoProc)();

	REQUEST(xVideoReq);

	REQUEST_AT_LEAST_SIZE(xVideoReq);
	GetGCAndDrawableAndValidate(stuff->gc, pGC, stuff->drawable, pDraw, client);

	pGCI = (GCInterestPtr) LookupID(stuff->gc, RT_GC, PlxVideoClass);
	if (!pGCI) {
		pGCI = AddAndInitPlxVideoInterest(pGC);
		AddResource(stuff->gc, RT_GC, pGCI, PlxVideoDeleteState, PlxVideoClass);
	}
	pPlxVideoProc = (int (*)()) (pGCI->extPriv);
	if (pPlxVideoProc) {
		(* pPlxVideoProc)(pDraw, pGC, stuff->videoReqType, &stuff[1]);
		return (Success);
	}
	SendErrorToClient(client, PlxVideoReqCode, stuff->videoReqType, 0, BadImplementation);
	return (BadImplementation);
}
