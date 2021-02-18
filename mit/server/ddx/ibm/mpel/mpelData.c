/***********************************************************
		Copyright IBM Corporation 1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: mpelData.c,v 1.3 90/03/05 14:17:22 swick Exp $ */
/* $Source: /xsrc/mit/server/ddx/ibm/mpel/RCS/mpelData.c,v $ */

#ifndef lint
static char *rcsid = "$Header: mpelData.c,v 1.3 90/03/05 14:17:22 swick Exp $";
#endif

#include "X.h"
#include "Xproto.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "font.h"
#include "pixmapstr.h"
#include "window.h"
#include "gcstruct.h"
#include "colormapst.h"
#include "cursorstr.h"

#include "mistruct.h"
#include "../../mfb/mfb.h"

#include "OScursor.h"
#include "ibmScreen.h"

#include "ppc.h"
#include "ppcProcs.h"

#include "mpel.h"

#include "mpelProcs.h"

extern void miRecolorCursor() ;
extern void NoopDDA() ;
extern void ibmAbort();

static PixmapPtr BogusPixmap = (PixmapPtr) 1 ;
ScreenRec mpelScreenRec ; /* Forward Declaration */

PixmapFormatRec	mpelFormats[]= { { 8, 8, 32 } } ;

VisualRec mpelVisuals[] = {
	{
	0,			/* unsigned long	vid */
	PseudoColor,		/* short	class */
	MPEL_BITS_PER_RGB,	/* short	bitsPerRGBValue */
	1 << MPELMAXPLANES,	/* short	ColormapEntries */
	MPELMAXPLANES,		/* short	nplanes */
	0,			/* unsigned long	redMask */
	0,			/* unsigned long	greenMask */
	0,			/* unsigned long	blueMask */
	0,			/* int		offsetRed */
	0,			/* int		offsetGreen */
	0			/* int		offsetBlue */
	}
} ;

static unsigned long int mpelDepthVID = 0 ;

DepthRec mpelDepths[] = {
/*	depth		numVid	vids */
    {	1,		0,	NULL	},
    {	MPELMAXPLANES,	1,	&mpelDepthVID }
} ;

#define mpelGCInterestValidateMask \
( GCLineStyle | GCLineWidth | GCJoinStyle | GCBackground | GCForeground	\
| GCFunction | GCPlaneMask | GCFillStyle | GC_CALL_VALIDATE_BIT		\
| GCClipXOrigin | GCClipYOrigin | GCClipMask | GCSubwindowMode )

ppcPrivGC mpelPrototypeGCPriv = {
	GXcopy,	/* unsigned char	rop */
	0,	/* unsigned char	ropOpStip */
	0,	/* unsigned char	ropFillArea */
	TRUE,	/* short	fExpose */
	FALSE,	/* short	freeCompClip */
	NullPixmap,	/* PixmapPtr	pRotatedPixmap */
	0,	/* RegionPtr	pCompositeClip */
	ppcAreaFill,	/* void 	(* FillArea)() */
		{
		    MPEL_ALLPLANES,	/* unsigned long	planemask */
		    0,			/* unsigned long	fgPixel */
		    1,			/* unsigned long	bgPixel */
		    GXcopy,		/* int			alu */
		    FillSolid,		/* int			fillStyle */
		}, /* ppcReducedRrop	colorRrop  */
	-1,	/* short lastDrawableType */
	-1,	/* short lastDrawableDepth */
	ibmAbort, /* void (* cacheIGBlt)() */
	ibmAbort, /* void (* cachePGBlt)() */
	0	/* pointer devPriv */
} ;

GCFuncs mpelGCFuncs = {
	ppcValidateGC,                  /* void (*ValidateGC)() */
	NoopDDA,                        /* void (*ChangeGC)() */
	NoopDDA,                        /* void (*CopyGC)() */
	ppcDestroyGC,                   /* void (*DestroyGC)() */
	ppcChangeClip,                  /* void (*ChangeClip)() */
	ppcDestroyClip,                 /* void (*DestroyClip)() */
	ppcCopyClip                     /* void (*CopyClip)() */
	};

GCOps mpelGCOps = {
	ppcSolidWindowFS,	/*  void (* FillSpans)() */
	ppcSetSpans,		/*  void (* SetSpans)()	 */
	miPutImage,		/*  void (* PutImage)()	 */
	ppcCopyArea,		/*  RegionPtr (* CopyArea)()	 */
	miCopyPlane,		/*  void (* CopyPlane)() */
	mpelPolyPoint,		/*  void (* PolyPoint)() */
	mpelZeroLine,		/*  void (* Polylines)() */
	mpelPolySegment,	/*  void (* PolySegment)() */
	miPolyRectangle,	/*  void (* PolyRectangle)() */
	mpelPolyArc,		/*  void (* PolyArc)()	 */
	mpelFillPolygon,	/*  void (* FillPolygon)() */
	miPolyFillRect,		/*  void (* PolyFillRect)() */
	miPolyFillArc,		/*  void (* PolyFillArc)() */
	mpelPolyText8,		/*  int (* PolyText8)()	 */
	mpelPolyText16,		/*  int (* PolyText16)() */
	mpelImageText8,		/*  void (* ImageText8)() */
	mpelImageText16,	/*  void (* ImageText16)() */
	(void (*)()) ppcImageGlyphBlt,	/*  void (* ImageGlyphBlt)() */
	(void (*)()) ppcPolyGlyphBlt,	/*  void (* PolyGlyphBlt)() */
	ppcPushPixels,		/*  void (* PushPixels)() */
	miMiter			/*  void (* LineHelper)() */
};
	

GC mpelPrototypeGC = {
	&mpelScreenRec,	/*  ScreenPtr	pScreen	 */
	MPELMAXPLANES,	/*  int         depth	 */
	GXcopy,			/*  int		alu	 */
	0,			/*  int		lineWidth */
	0,			/*  int		dashOffset */
	0,			/*  int		numInDashList */
	0,			/*  unsigned char *dash	 */
	LineSolid,		/*  int		lineStyle */
	CapButt,		/*  int		capStyle */
	JoinMiter,		/*  int		joinStyle */
	FillSolid,		/*  int		fillStyle */
	EvenOddRule,		/*  int		fillRule */
	ArcPieSlice,		/*  int		arcMode	 */
	ClipByChildren,		/*  int		subWindowMode */
	TRUE,			/*  Bool	graphicsExposures */
	CT_NONE,		/*  int		clientClipType */
	1,			/*  int		miTranslate:1 */
	1,			/*  unsigned int tileIsPixel:1 */
	0,			/*  unused:16 */
	MPEL_ALLPLANES,	/*  unsigned long	planemask */
	0,			/*  unsigned long	fgPixel */
	1,			/*  unsigned long	bgPixel */
	{0},			/*  PixmapPtr	tile	 */
	0,			/*  PixmapPtr	stipple	 */
	{ 0, 0 },		/*  DDXPointRec	patOrg	 */
	0,			/*  FontPtr	font	 */
	{ 0, 0 },		/*  DDXPointRec	clipOrg	 */
	{ 0, 0 },		/*  DDXPointRec	lastWinOrg */
	NULL,			/*  pointer	clientClip */
	/* (1<<(GCLastBit+1))-1, */	/*  unsigned long	stateChanges */
	0,			/*  unsigned long	stateChanges */
	0,			/*  unsigned long        serialNumber */
	&mpelGCFuncs,		/*  GCFuncs *funcs */
	&mpelGCOps,		/*  GCOps *ops */
	NULL			/*  devUnion	devPrivates	 */

} ;

ppcScrnPriv mpelScrnPriv = {
	{
		{
			DRAWABLE_PIXMAP,/* unsigned char type */
			0,		/* unsigned char class */
			MPELMAXPLANES,	/* unsigned char depth */
			1,		/* unsigned char bitsPerPixel */
			0,		/* unsigned long id */
			0,		/* short x */
			0,		/* short y */
			MPEL_WIDTH,	/* unsigned short width */
			MPEL_HEIGHT,	/* unsigned short height */
			0,		/* ScreenPtr	pScreen */
			0		/* unsigned long        serialNumber */
		},		/* DrawableRec drawable */
		1,		/* int refcnt */
		MPEL_WIDTH / 8,	/* int devKind */
		{0}		/* DevUnion devPrivate */
	},			/* PixmapRec	pixmap */
	0,			/* void	(* devHardwareInitBB)() */
	0,			/* void	(* devHardwareInitAB)() */
	&mpelPrototypeGC,	/* GCPtr	devPrototypeGC */
	&mpelPrototypeGCPriv,	/* ppcPrivGC 	*devPrototypeGCPriv */
	mpelChangeGCtype,	/* Mask		(* changeGCtype)() */
	mpelChangeWindowGC,	/* Mask		(* changeWindowGC)() */
	0,			/* Colormap	InstalledColormap */
	mpelBitBlt,		/* void	(* blit)() */
	mpelFillSolid,		/* void	(* solidFill)() */
	mpelTile,		/* void	(* tileFill)() */
	ppcStipple,		/* void	(* stipFill)() */
	ppcOpaqueStipple,	/* void	(* opstipFill)() */
	mpelDrawColorImage,	/* void	(* imageFill)() */
	mpelReadColorImage,	/* void	(* imageRead)() */
	mpelBresLine,		/* void	(* lineBres)() */
	mpelHorzLine,		/* void	(* lineHorz)() */
	mpelVertLine,		/* void	(* lineVert)() */
	(void (*)())mpelSetColor,		/* void	(* setColor)() */
	mpelRecolorCursor,	/* void	(* RecolorCursor)() */
	mpelDrawMonoImage,	/* void	(* monoFill)() */
	mpelDrawMonoByteImage,	/* void	(* glyphFill)() */
	mpelGetPlane,		/* unsigned long *((* planeRead)()) */
	ppcReplicateArea,	/* void	(* replicateArea)() */
	mpelDestroyGCPriv,	/* void	(* DestroyGCPriv)() */
/* High Level Software Cursor Support !! */
	&mpelcursorSemaphore,	/* int	* CursorSemaphore */
	mpelCheckCursor,	/* int	(* CheckCursor)() */
	mpelReplaceCursor,	/* void	(* ReplaceCursor)() */
	0			/* ppcCacheInfoPtr cacheInfo */
} ;

ScreenRec mpelScreenRec = {
	0,			/* int			myNum */
	0,			/* ATOM id */
	MPEL_WIDTH,		/* short		width */
	MPEL_HEIGHT,		/* short		height */
	(MPEL_WIDTH * 254)/880,	/* short		mmWidth */
	(MPEL_HEIGHT*254)/880,	/* short		mmHeight */
	sizeof mpelDepths/sizeof (DepthRec),	/* short	numDepths */
	MPELMAXPLANES,		/* short		rootDepth */
	mpelDepths,		/* DepthPtr       	allowedDepths */
	0,			/* unsigned long      	rootVisual */
	0,			/* unsigned long	defColormap */
	0,			/* short		minInstalledCmaps */
	1,			/* short		maxInstalledCmaps */
	Always,			/* char                backingStoreSupport */
	NotUseful,		/* char                saveUnderSupport */
	MPEL_WHITE_PIXEL,	/* unsigned long	whitePixel */
	MPEL_BLACK_PIXEL,	/* unsigned long	blackPixel */
	0,			/* unsigned long	rgf */
	{ 0 },			/* GCPtr	GCperDepth[MAXFORMATS+1] */
	{ 0 },			/* PixmapPtr		PixmapPerDepth[1] */
	(pointer) &mpelScrnPriv,	/* pointer		devPrivate */
	sizeof mpelVisuals/sizeof (VisualRec), /* short     	numVisuals */
	&mpelVisuals[0],		/* VisualPtr		visuals */
        0,                              /* int          WindowPrivateLen; */
        0,                              /* unsigned     WindowPrivateSizes; */
        0,                              /* unsigned     totalWindowSize; */
        0,                              /* int          GCPrivateLen; */
        0,                              /* unsigned     GCPrivateSizes; */
        0,                              /* unsigned     totalGCSize; */
/* Random screen procedures */
	mpelScreenClose,		/* Bool (* CloseScreen)() */
	ppcQueryBestSize,	/* void (* QueryBestSize)() */
	ibmSaveScreen,		/* Bool (* SaveScreen)() */
	ppcGetImage,		/* void (* GetImage)() */
	(void (*)()) ppcGetSpans, /* void (*GetSpans)() */
	ibmAbort,		/* void (* PointerNonInterestBox)() */
	(void (*)()) NULL,	/* void (* SourceValidate)() */
/* Window Procedures */
	ppcCreateWindow,	/* Bool (* CreateWindow)() */
	ppcDestroyWindow,	/* Bool (* DestroyWindow)() */
	ppcPositionWindow,	/* Bool (* PositionWindow)() */
	mfbChangeWindowAttributes,	/* Bool (* ChangeWindowAttributes)() */
	mfbMapWindow,		/* Bool (* RealizeWindow)() */
	mfbUnmapWindow,		/* Bool (* UnrealizeWindow)() */
	miValidateTree,		/* int  (* ValidateTree)() */
	miWindowExposures,	/* void (* WindowExposures)() */
        ppcPaintWindow,         /* void PaintWindowBackground */
        ppcPaintWindow,         /* void PaintWindowBorder */
        ppcCopyWindow,          /* void CopyWindow */
        miClearToBackground,    /* void ClearToBackground */
/* Pixmap procedures */
	ppcCreatePixmap,	/* PixmapPtr (* CreatePixmap)() */
	mfbDestroyPixmap,	/* Bool (* DestroyPixmap)() */
/* Backing store Procedures */
        NoopDDA,        /* void SaveDoomedAreas */
        (RegionPtr (*)())ppcRestoreAreas,      /* RegionPtr RestoreAreas */
        NoopDDA,        /* void ExposeCopy */
        (RegionPtr (*)())NoopDDA,        /* void TranslateBackingStore */
        (RegionPtr (*)())NoopDDA,        /* void ClearBackingStore */
        NoopDDA,        /* void DrawGuarantee */
/* Font procedures */
	mpelRealizeFont,	/* Bool (* RealizeFont)() */
	mpelUnrealizeFont,	/* Bool (* UnrealizeFont)() */
/* Cursor Procedures */
	OS_ConstrainCursor,	/* void (* ConstrainCursor)() */
	OS_CursorLimits,	/* void (* CursorLimits)() */
	mpelDisplayCursor,	/* Bool (* DisplayCursor)() */
	ppcRealizeCursor,	/* Bool (* RealizeCursor)() */
	ppcUnrealizeCursor,	/* Bool (* UnrealizeCursor)() */
	miRecolorCursor,	/* void (* RecolorCursor)() */
	OS_SetCursorPosition,	/* Bool (* SetCursorPosition)() */
/* GC procedures */
	ppcCreateGC,		/* Bool (* CreateGC)() */
/* Colormap procedures */
	ppcCreateColormapNoop,	/* Bool (* CreateColormap)() */
	NoopDDA,		/* void (* DestroyColormap)() */
	mpelInstallColormap,	/* void (* InstallColormap)() */
	ppcUninstallColormap,	/* void (* UninstallColormap)() */
	ppcListInstalledColormaps,	/* int (* ListInstalledColormaps) () */
	ppcStoreColors,		/* void (* StoreColors)() */
	ppcResolveColor,	/* void (* ResolveColor)() */
/* Region procedures */
	miRegionCreate,		/* RegionPtr (* RegionCreate)() */
	miRegionInit,		/* void (* RegionInit)() */
	miRegionCopy,		/* void (* RegionCopy)() */
	miRegionDestroy,	/* void (* RegionDestroy)() */
	miRegionUninit,		/* void (* RegionUninit)() */
	miIntersect,		/* int (* Intersect)() */
	miUnion,		/* int (* Union)() */
	miSubtract,		/* int (* Subtract)() */
	miInverse,		/* int (* Inverse)() */
	miRegionReset,		/* void (* RegionReset)() */
	miTranslateRegion,	/* void (* TranslateRegion)() */
	miRectIn,		/* int (* RectIn)() */
	miPointInRegion,	/* Bool (* PointInRegion)() */
	miRegionNotEmpty,	/* Bool (* RegionNotEmpty)() */
	miRegionEmpty,		/* void (* RegionEmpty)() */
	miRegionExtents,	/* BoxPtr (*RegionExtents)() */
        miRegionAppend,         /* BoxPtr (*RegionAppend)() */
        miRegionValidate,       /* BoxPtr (*RegionValidate)() */
        mfbPixmapToRegion,      /* BoxPtr (*BitmapToRegion)() */
        miRectsToRegion,        /* BoxPtr (*RectsToRegion)() */
	miSendGraphicsExpose,	/* void	(*SendGraphicsExpose)() */
/* os layer procedures */
	NoopDDA,		/* void (* BlockHandler)() */
	NoopDDA,		/* void (* WakeupHandler)() */
	(pointer) 0,		/* pointer blockData */
	(pointer) 0,		/* pointer wakeupData */
	0			/* DevUnion devPrivate */
} ;
