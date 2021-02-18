/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
#ifndef MPEL_FIFO_SEEN
#define	MPEL_FIFO_SEEN 1
/***********************************************************
		Copyright IBM Corporation 1987,1988

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

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelFifo.h,v 6.4 89/04/29 21:39:37 jeff Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelFifo.h,v $ */

#ifndef lint
static char *rcsidmpelfifo = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelFifo.h,v 6.4 89/04/29 21:39:37 jeff Exp $";
#endif

   /*
    * Definitions for megapel hardware fifo 
    */

#define	MPEL_FIFO_LEN	2048
#define	MPEL_MAXRETRY	500

#define MPELWaitFifo()   {while (MPEL_PSR&PSR_FIFO_NOT_EMPTY);}

   /*
    * structure elements (instructions)
    */

#ifdef PORTABLE
#define	mfCmd0Args(cmd)		mpelWFifo(0,(cmd),0,0)
#define	mfCmd1Arg(cmd,a)	mpelWFifo(1,(cmd),(short *)(a),0)
#define	mfCmd2Args(cmd,a,a2)	mpelWFifo(2,(cmd),(short *)(a),(short *)(a2))
#define	mfCmdNArgs(cmd,n,a)	mpelWFifo((n),(cmd),(short *)(a),0)
#define	mfCmdVArgs(cmd,n,a,e)	mpelWFifo((n),(cmd),(short *)(a),(e))
#define	mfData(pD,n)		mpelWFifo(n,0,pD,0)
#else
extern	void	mfCmd0Args();
extern	void	mfCmd1Arg();
extern	void	mfCmd2Args();
extern	void	mfCmdNArgs();
extern	void	mfCmdVArgs();
extern	void	mfData();
#endif /* PORTABLE */

extern int MPELNoOpData;

#define	MPELSetLineType(t)			mfCmd1Arg(0x5040,(t))
#define	MPELSetPolylineColor(i)			mfCmd1Arg(0x5042,(i))
#define	MPELSetMarkerType(t)			mfCmd1Arg(0x5043,(t))
#define	MPELSetMarkerSizeScaleFactor(f)		mfCmd1Arg(0x5044,(f))
#define	MPELSetPolymarkerColor(i)		mfCmd1Arg(0x5045,(i))
#define	MPELSetTextFont(f)			mfCmd1Arg(0x5046,(f))
#define	MPELSetTextPrecision(p)			mfCmd1Arg(0x5047,(p))
#define	MPELSetCharExpansionFactor(x)		mfCmd1Arg(0x5048,(x))
#define	MPELSetCharSpacing(s)			mfCmd1Arg(0x5049,(s))
#define	MPELSetTextColor(i)			mfCmd1Arg(0x504a,(i))
#define	MPELSetCharHeight(h)			mfCmd1Arg(0x504b,(h))
#define	MPELSetCharUpVector(x,y)		mfCmd2Args(0x504c,(x),(y))
#define	MPELSetTextPath(p)			mfCmd1Arg(0x504d,(p))
#define	MPELSetTextAlignment(h,v)		mfCmd2Args(0x5050,(h),(v))
#define	MPELSetPgonInteriorStyle(i)		mfCmd1Arg(0x5051,(i))
#define	MPELSetPgonInteriorStyleIndex(i)	mfCmd1Arg(0x5052,(i))
#define	MPELSetPgonInteriorColor(i)		mfCmd1Arg(0x5053,(i))
#define	MPELSetPgonEdgeFlag(f)			mfCmd1Arg(0x5054,(f))
#define	MPELSetPgonEdgeLinetype(t)		mfCmd1Arg(0x5055,(t))
#define	MPELSetPgonEdgeColor(i)			mfCmd1Arg(0x5056,(i))
#define	MPELSetLogicalOp(o)			mfCmd1Arg(0x5060,(o))
#define	MPELSetPlaneMask(m)			mfCmd1Arg(0x5061,(m))
#define	MPELSetTextCodePage(p)			mfCmd1Arg(0x5062,(p))
#define	MPELSetHighlightingColor(i)		mfCmd1Arg(0x5063,(i))
#define	MPELSetBlinkControlParameters(p)	mfCmdNArgs(0x5064,10,(p))
#define	MPELSetLineOnLineBg(i)			mfCmd1Arg(0x5065,(i))
#define	MPELSetUserLineStyle(s)			mfCmdNArgs(0x5066,8,(s))
#define	MPELSetLineOnLineMask(m)		mfCmd1Arg(0x5067,(m))
#define	MPELBeginPgon()				mfCmd0Args(0x500a)
#define	MPELCircle(c)				mfCmdNArgs(0x5006,10,(c))
#define	MPELCircularArc(a)			mfCmdNArgs(0x5007,14,(a))
#define	MPELClose()				mfCmd0Args(0x500c)
#define	MPELEllipse(e)				mfCmdNArgs(0x5008,14,(e))
#define	MPELEllipticalArc(a)			mfCmdNArgs(0x5009,18,(a))
#define	MPELEndPgon()				mfCmd0Args(0x500b)
#define	MPELFillCircle(c)			mfCmdNArgs(0x5005,10,(c))
#define	MPELFillRectangle(r)			mfCmdNArgs(0x5004,12,(r))
#define	MPELCircularArcWConnectivity(a)		mfCmdNArgs(0x5026,20,(a))
#define	MPELEllipticalArcWConnectivity(a)	mfCmdNArgs(0x5027,23,(a))
#define	MPELBeginHighlight(h)			mfCmd1Arg(0x501e,(h))
#define	MPELEndHighlight()			mfCmd0Args(0x501f)
#define	MPELEnablePick()			mfCmd0Args(0x501a)
#define	MPELDisablePick()			mfCmd0Args(0x501b)
#define	MPELSynchronize(s)			mfCmdNArgs(0x5020,10,(s))
#define	MPELSetScissor(b)			mfCmdNArgs(0x5017,12,(b))
#define	MPELSetTransformAndClipModes(m)		mfCmdNArgs(0x5028,14,(m))
#define	MPELSetClipping(b)			mfCmdNArgs(0x5014,16,(b))
#define	MPELSetTransformationMatrix(m)		mfCmdNArgs(0x5015,26,(m))
#define	MPELSetViewport(v)			mfCmdNArgs(0x5016,12,(v))
#define	MPELVPMCopy(r)				mfCmdNArgs(0x500e,18,(r))
#define	MPELVPMBLTDest(r)			mfCmdNArgs(0x5011,18,(r))
#define	MPELSrcBLTVPM(r)			mfCmdNArgs(0x5012,20,(r))
#define	MPELSrcBLTVPMWColorExpansion(b)		mfCmdNArgs(0x5013,18,(b))
#define	MPELSrcBLTVPMWColorExpansionROP(b)	mfCmdNArgs(0x5013,20,(b))
#define	MPELSrcBLTVPMNoMask(s)			mfCmdNArgs(0x5021,30,(s))
#define	MPELSrcBLTVPMWMask(s)			mfCmdNArgs(0x5021,40,(s))
#define	MPELVPMBLTDestNoMask(s)			mfCmdNArgs(0x5022,30,(s))
#define	MPELVPMBLTDestWMask(s)			mfCmdNArgs(0x5022,40,(s))
#define	MPELVPMtoVPMCopyNoMask(s)		mfCmdNArgs(0x5023,30,(s))
#define	MPELVPMtoVPMCopyWMask(s)		mfCmdNArgs(0x5023,40,(s))
#define	MPELTileBLTVPMNoMask(s)			mfCmdNArgs(0x5024,22,(s))
#define	MPELTileBLTVPMWMask(s)			mfCmdNArgs(0x5024,32,(s))

#define	MPELDisjointPolyline(n,l) \
				mfCmdVArgs(0x501d,4+(n)*sizeof(mpelPoint),(l),0)
#define	MPELMultiline(n,l)	mfCmdVArgs(0x5003,4+(n)*sizeof(mpelPoint),(l),0)
#define	MPELPolyline(n,l) 	mfCmdVArgs(0x5000,4+(n)*sizeof(mpelPoint),(l),0)
#define	MPELPolymarker(n,m)	mfCmdVArgs(0x5001,4+(n)*sizeof(mpelPoint),(m),0)
#define	MPELNoOp(n,d)		mfCmdVArgs(0x501c,4+(n),(d),0)
#ifdef AIXrt
#define	MPELConvexPoly(n,v)	mfCmdVArgs(0x502b,4+(n)*sizeof(mpelPoint),(v),0)
#define	MPELOpenConvexPoly(n,v)	\
	mfCmdVArgs(0x502b,4+(n)*sizeof(mpelPoint),(v),sizeof(mpelPoint))
#define MPELCloseConvexPoly(p)	MPELSendData(sizeof(mpelPoint),(p))
#endif AIXrt

#define	MPELAnnotationText(n,t) \
	  mfCmdVArgs(0x5002,12,(t),((unsigned long int)(n)))
#define	MPELText(n,t) \
	  mfCmdVArgs(0x500d,10,(t),((unsigned long int)(n)))
#define	MPELBLTImmed(n,s) \
	  mfCmdVArgs(0x5018,16,(s),((unsigned long int)(n)))
#define	MPELBLTImmedWColorExpansion(n,s) \
	mfCmdVArgs(0x5019,16,(s),((unsigned long int)(n)))
#define	MPELBLTImmed4x4WColorExpansion(n,s) \
	mfCmdVArgs(0x5025,14,(s),((unsigned long int)(n)))

#define	MPELSendData(n,d)			mfData(d,n)

	/*
	 * arguments to structure elements
	 */

typedef struct {
	short int x; /* 16-bit object */
	short int y; /* 16-bit object */
} mpelPoint;

typedef mpelPoint	*mpelPointList;

typedef struct {
	unsigned short int rate; /* 16-bit object */
	unsigned long int color; /* 32-bit object */
} mpelBlinkControlParameters;

typedef struct {
	mpelPoint	center;
	unsigned short int radius; /* 16-bit object */
} mpelCircle;

typedef struct {
	mpelCircle		circle;
	unsigned short int	start;
	unsigned short int	end;
} mpelCircularArc;

typedef struct {
	mpelPoint	center;
	short int	majax;
	short int	minax;
	short int	angle;
} mpelEllipse;

typedef struct {
	mpelEllipse		ellipse;
	unsigned short int	start;
	unsigned short int	end;
} mpelEllipticalArc;

typedef struct {
	mpelPoint	lleft;
	mpelPoint	uright;
} mpelRectangle;

typedef struct {
	mpelPoint		point;
	unsigned short int	reserved;
	unsigned short int	length;
	char			*string;
} mpelAnnotationText;

typedef struct {
	mpelPoint		point;
	unsigned short int	length;
	char			*string;
} mpelText;

typedef struct {
	mpelCircle	circle;
	mpelPoint	startpt;
	mpelPoint	endpt;
	short int	drawdir;
} mpelCircularArcWConnectivity;

typedef struct {
	mpelEllipse	ellipse;
	mpelPoint	startpt;
	mpelPoint	endpt;
	short int	drawdir;
} mpelEllipticalArcWConnectivity;

typedef struct {
	short int		funct;
	short int		inter;
	unsigned short int	data;
} mpelSynchronize;

typedef struct {
	short int		tran;
	short int		clip;
	short int		map;
	short int		reserved;
} mpelTransformClipModes;

typedef struct {
	short int		xleft;
	short int		xright;
	short int		yleft;
	short int		yright;
} mpelClippingRectangle;

typedef struct {
	unsigned short int	ctype;
	short int		scalef;
	short int		matrix[3][3];
} mpelTransformationMatrix;

typedef struct {
	mpelRectangle		source;
	mpelPoint		dlleft;
	unsigned short int	alu;
} mpelVPMCopy;

typedef struct {
	unsigned long int	destaddr;
	mpelRectangle		source;
	short int		comp;
} mpelVPMBLTDestination;

typedef struct {
	unsigned long int	srcaddr;
	mpelRectangle		dest;
	unsigned short int	bpixel;
	unsigned short int	alu;
} mpelSrcBLTVPM;

typedef struct {
	mpelRectangle		dest;
	unsigned short int	bpixel;
	unsigned short int	alu;
	char			*data;
} mpelBLTImmed;

typedef struct {
	mpelRectangle		dest;
	unsigned short int	color;
	unsigned short int	alu;
	char			*data;
} mpelBLTImmedWColorExpansion;

typedef struct {
	mpelRectangle		dest;
	unsigned short int	color;
	char			*data;
} mpelBLTImmed4x4WColorExpansion;

typedef struct {
	unsigned long int	srcaddr;
	mpelRectangle		dest;
	unsigned short int	color;
	unsigned short int	alu;
} mpelSrcBLTVPMWColorExpansion;

typedef struct {
	unsigned long int	srcaddr;
	unsigned short int	swidth;
	mpelPoint		suleft;
	unsigned short int	pwidth;
	unsigned short int	pheight;
	unsigned long int	dstaddr;
	unsigned short int	dwidth;
	mpelPoint		duleft;
	unsigned short int	alu;
	unsigned long int	mskaddr;
	unsigned short int	mwidth;
	mpelPoint		muleft;
} mpelBLTVPMWMask;

typedef struct {
	unsigned long int	tileaddr;
	unsigned short int	twidth;
	unsigned short int	theight;
	mpelRectangle		dest;
	unsigned short int	alu;
	unsigned long int	mskaddr;
	unsigned short int	mwidth;
	unsigned short int	muleft;
} mpelTileBLTVPMWMask;

	/*
	 * Default values for some registers
	 */
extern	unsigned short int	mpelMaskXlate[];
extern	mpelRectangle		mpelDfltScissor;

	/*
	 * Macros to set to corresponding X values or reset
	 * defaults
	 */

#define	mpelSetPlaneMask(m) \
	MPELSetPlaneMask( (unsigned long int) mpelMaskXlate[ (m) & 0x3ff ] )
#define	mpelSetALU(a)		MPELSetLogicalOp((a)+1)

#define	MPELResetScissor()	MPELSetScissor(&mpelDfltScissor)
#define	MPELResetPlaneMask()	MPELSetPlaneMask(0)
#define	MPELResetALU()		MPELSetLogicalOp(4)

#endif /* ndef MPEL_FIFO_SEEN */
