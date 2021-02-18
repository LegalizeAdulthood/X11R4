/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xgraphic.ch,v 1.3 89/02/17 18:22:47 ghoti Exp $ */
/* $ACIS:graphic.ch 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xgraphic.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidxgraphic_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xgraphic.ch,v 1.3 89/02/17 18:22:47 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


#include <fontdesc.ih>
#include <point.h>
#include <rect.h>
#include <region.ih>

#define xgraphic_PROGRAMMERVERSION	1

class xgraphic : graphic {
overrides:
    DrawLineTo(long XEnd, long YEnd);
    DrawLine(long DeltaX, long DeltaY);

    DrawString(char * Text, short Operation);
    DrawText(char * Text, long TextLength, short Operation);

    SetFont(struct fontdesc * ChosenFont);

    DrawRectSize(long x,long y,long width,long height);
    DrawPolygon(struct point * PointArray, short PointCount);
    DrawPath(struct point * PointArray, short PointCount);
    DrawOvalSize(long x,long y,long width,long height);
    DrawArcSize(long x,long y,long width,long height, short StartAngle, short OffsetAngle);
    DrawRRectSize(long x,long y,long width,long height, long cornerWidth,long cornerHeight);
    DrawTrapezoid(long topX, long topY, long topWidth, long bottomX, long bottomY, long bottomWidth);


    FillRectSize(long x,long y,long width,long height, struct graphic * Tile);
    FillPolygon(struct point * PointArray, short PointCount, struct graphic * Tile);
    FillOvalSize(long x,long y,long width,long height, struct graphic * Tile);
    FillArcSize(long x,long y,long width,long height, short StartAngle, short OffsetAndle, struct graphic * Tile);
    FillRRectSize(long x, long y, long width, long height, long cornerWidth, long cornerHeight, struct graphic * Tile);
    FillRgn(region * Rgn, struct graphic * Tile);
    FillTrapezoid(long topX, long topY, long topWidth, long bottomX, long bottomY, long bottomWidth, struct graphic * Tile);

    BitBlt(struct rectangle * SrcRect, 
		struct graphic *DestGraphic, struct point * DstOrigin, 
		struct rectangle * ClipRectList);
    SetBitAtLoc(long XPos, long YPos, boolean NewValue);
    WritePixImage(/* struct xgraphic *self, */ long DestX, long DestY, 
		struct pixelimage *SrcPixels, long SrcX, long SrcY, 
		long width, long height);
		/* transfers a pixelimage to the graphic */
    ReadPixImage(/* struct xgraphic *self, */ long SrcX, long SrcY, 
		struct pixelimage *DestPixels, long DestX, long DestY, 
		long width, long height);
		/* transfers a piece of the graphic to a piece of the pixelimage */

    InsertGraphic(struct graphic * EnclosingGraphic, struct rectangle * EnclosedRectangle);
    InsertGraphicSize(struct graphic * EnclosingGraphic, 
		long xOriginInParent, long yOriginInParent, long width, long height);

    SetClippingRect(struct rectangle * AdditionalRect);
    ClearClippingRect();

    SetLineWidth(short NewLineWidth);

    SetTransferMode(short NewTransferMode);

    FlushGraphics();

    WhitePattern() returns struct graphic *;
    BlackPattern() returns struct graphic *;
    GrayPattern(short IntensityNum, IntensityDenom) returns struct graphic *;
    ColorPattern(long red, long blue, long green) returns struct graphic *;

    /* If colorName is NULL, use the RGB. */
    SetForegroundColor(char *colorName, long red, long blue, long green);
    SetBackgroundColor(char *colorName, long red, long blue, long green);

    GetHorizontalResolution() returns long;
    GetVerticalResolution() returns long;
    GetWindowManagerType() returns char *;
    GetDevice() returns long;
macromethods:
    Valid() ((self)->valid)
    XWindow() ((self)->localWindow)
    XDisplay() ((self)->displayUsed)
    XScreen() ((self)->screenUsed)
    XGC()  ((self)->localGraphicContext)
    XFillGC() ((self)->localFillGraphicContext)
classprocedures:
    SetUpdateRegion(Region Rgn,Display* whichDisplay, Drawable whichWindow);
data: 
    struct xgraphic ** gray_shades;   /* Current set of gray pixmaps */
    GC localGraphicContext;	/* Context for the pixmap */
    GC localFillGraphicContext;	/* Context for filling */
    long lastFillStyle;		/* Fill style used for fill gc */
    unsigned long lastFillPixel; /* Pixel used for fill gc */
    struct xgraphic * lastFillTile;  /* Tile used for fill gc */
    Drawable localWindow;		/* X pixmap id  */
    Display * displayUsed;		/* which display is used */
    int	    screenUsed;			/* which screen is used */
    long lastUpdateRegionIDUsed;    /* set the last update region sequence number that has been processed */
    unsigned long foregroundpixel; /* Pixel value of foreground color. */
    unsigned long backgroundpixel; /* Pixel value of background color. */
    boolean valid;		/* Set to true if we have a valid Xwindow, etc */
};
