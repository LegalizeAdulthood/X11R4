/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
 /* graphic.c
  */

#ifndef LINT
	char graphic_rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/graphic.c,v 2.5 89/03/10 16:09:52 cm26 Exp $";
#endif /* LINT */

#include <class.h>

#include <fontdesc.ih>
#include <region.ih>
#include <graphic.eh>
#include <physical.h>
#include <im.ih>

static char *foregroundColorName = NULL; /* Name of the default foreground color. */
static char *backgroundColorName = NULL; /* Name of the default background color. */

void graphic__MoveToPt(self,NewPosition)
struct graphic * self;
struct point * NewPosition; 
{
    graphic_MoveTo(self,point_X(NewPosition),point_Y(NewPosition));
}


void graphic__MoveTo(self, NewX, NewY)
struct graphic * self;
long NewY;
long NewX;
{
    point_SetPt(&self->currentPoint,NewX,NewY);
}

void graphic__Move(self, DeltaX, DeltaY)
struct graphic * self;
long DeltaY; 
long DeltaX;
{
    point_OffsetPoint(&self->currentPoint,DeltaX,DeltaY);
}

void graphic__GetCurrentPt(self,Pt)
struct graphic * self;
struct point * Pt;
{
    *Pt = self->currentPoint;
}

void graphic__DrawLineToPt(self,LineEnd)
struct graphic * self;
struct point * LineEnd;
{
    graphic_DrawLineTo(self,point_X(LineEnd),point_Y(LineEnd));
}

void graphic__DrawLineTo(self, XEnd, YEnd)
struct graphic * self;
long YEnd; 
long XEnd;
{
    point_SetPt(&self->currentPoint,XEnd,YEnd);
}

void graphic__DrawLine(self,DeltaX, DeltaY)
struct graphic * self;
long DeltaY; 
long DeltaX;
{
    point_OffsetPoint(&self->currentPoint,DeltaX,DeltaY);
}

void graphic__DrawString(self, Text, Operation)
struct xgraphic * self;
char * Text;
short Operation; 
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: DrawString not implemented for this wm\n");
    }
}

void graphic__DrawText(self, Text, TextLength, Operation)
struct graphic * self;
char * Text;
long TextLength;
short Operation;
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: DrawText not implemented for this wm\n");
    }
}

void graphic__SetSpaceShim(self,Amount)
struct graphic * self;
short Amount;
{
    self->spaceShim = Amount;
}

short graphic__GetSpaceShim(self)
struct graphic * self;
{

    return self->spaceShim;
}


void graphic__SetFont(self, ChosenFont)
struct graphic * self;
struct fontdesc * ChosenFont;
{

    if (ChosenFont) {
	self->currentFont = ChosenFont;
    }
    /* Damn, nothing there, so switch back to default */
    else {
	self->currentFont = fontdesc_Create("andysans", fontdesc_Plain, 12);
    }
}

struct fontdesc * graphic__GetFont(self)
struct graphic * self;
{
    if (self->internalFont) return NULL;
    return self->currentFont;
}

void graphic__DrawRectSize(self,x, y,width,height)
struct graphic * self;
long x,y,width,height;
{
    long left = x;
    long right = x+width;
    long top = y;
    long bottom = y+height;

    if (left > right || top > bottom) return;

    graphic_MoveTo(self,left,top);
    graphic_DrawLineTo(self,right,top);
    graphic_DrawLineTo(self,right,bottom);
    graphic_DrawLineTo(self,left,bottom);
    graphic_DrawLineTo(self,left,top);
}

void graphic__DrawRect(self, Rect)
struct graphic * self;
struct rectangle * Rect;
{
    graphic_DrawRectSize(self,rectangle_Left(Rect),rectangle_Top(Rect), rectangle_Width(Rect), rectangle_Height(Rect));
}

void graphic__DrawPolygon(self, PointArray, PointCount)
struct graphic * self;
struct point * PointArray;
short PointCount;
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: DrawPolygon not implemented for this wm\n");
    }
}

void graphic__DrawPath(self, PointArray, PointCount)
struct graphic *self;
struct point * PointArray;
short PointCount; 
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: DrawPath not implemented for this wm\n");
    }
}

void graphic__DrawOvalSize(self, x,y,width,height)
struct graphic * self;
long x,y,width,height;
{
    /* An approximation for now */
    graphic_DrawRectSize(self,x,y,width,height);
}

void graphic__DrawOval(self, Rect)
struct graphic * self;
struct rectangle * Rect;
{
    graphic_DrawOvalSize(self,rectangle_Left(Rect),
			  rectangle_Top(Rect),rectangle_Width(Rect),
			  rectangle_Height(Rect));
}

void graphic__DrawArcSize(self,x,y,width,height, StartAngle, OffsetAngle)
struct graphic * self;
long x,y,width,height;
short StartAngle;
short OffsetAngle;
{
    /* Cheap imitation by a diagonal line */
    graphic_MoveTo(self,x,y);
    graphic_DrawLineTo(self,x+width,y+height);
}

void graphic__DrawArc(self, EnclRect, StartAngle, OffsetAngle)
struct graphic * self;
struct rectangle * EnclRect;
short StartAngle;
short OffsetAngle;
{

    graphic_DrawArcSize(self,rectangle_Left(EnclRect),
			 rectangle_Top(EnclRect), rectangle_Width(EnclRect),
			 rectangle_Height(EnclRect), StartAngle, OffsetAngle);
}

void graphic__DrawRRectSize(self,x,y,width,height,cornerWidth,cornerHeight)
struct graphic * self;
long x,y,width,height;
long cornerHeight, cornerWidth;
{
    /* Handle pathologic cases in system indepedent manner
      (luser desires to bite bullet in efficiency) */

    if ( (2*cornerHeight >= height) || (2*cornerWidth >= width)) {
	/* Bizarre -- corners are bigger than rectangle, so 
	 make an appropriate looking oval */
	if ( (2*cornerHeight >= height) && (2*cornerWidth >= width))
	    graphic_DrawOvalSize(self,x,y,width,height);
	else if (2*cornerHeight >= height) {
	    /* Draw left semi-oval */
	    graphic_DrawArcSize(self,x,y,2*cornerWidth,height,0,-180);
	    /* Draw Top line */
	    graphic_MoveTo(self,x+cornerWidth,y);
	    graphic_DrawLine(self,width-2*cornerWidth,0);
	    /* Draw right semi-oval */
	    graphic_DrawArcSize(self,x+width-2*cornerWidth,y,2*cornerWidth,height,0,180);
	    /* Draw bottom line */
	    graphic_MoveTo(self,x+cornerWidth,y+height);
	    graphic_DrawLine(self,width-2*cornerWidth,0);
	}
	else { /* assuming (2*cornerWidth >= width) */
	    /* Draw top semi-oval */
	    graphic_DrawArcSize(self,x,y,width,2*cornerHeight,-90,180);
	    /* Draw right line */
	    graphic_MoveTo(self,x+width,y+cornerHeight);
	    graphic_DrawLine(self,0,height-2*cornerHeight);
	    /* Draw bottom semi-oval */
	    graphic_DrawArcSize(self,x,y+height-2*cornerHeight,width,2*cornerHeight,90,180);
	    /* Draw left line */
	    graphic_MoveTo(self,x,y+cornerHeight);
	    graphic_DrawLine(self,0,height-2*cornerHeight);
	}
	return;
    }


    /* Generic rr rect -- go around the loop */
    /* Upper left corner */
    graphic_DrawArcSize(self,x,y,cornerWidth*2, cornerHeight*2,-90,90);
    /* Top line */
    graphic_MoveTo(self,x+cornerWidth,y);
    graphic_DrawLine(self,width-2*cornerWidth,0);
    /* upper right corner */
    graphic_DrawArcSize(self,x+width-2*cornerWidth,y,cornerWidth*2,cornerHeight*2,0,90);
    /* right side */
    graphic_MoveTo(self,x+width,y+cornerHeight);
    graphic_DrawLine(self,0,height-2*cornerHeight);
    /* lower right corner */
    graphic_DrawArcSize(self,x+width-2*cornerWidth,y+height-2*cornerHeight,cornerWidth*2,cornerHeight*2,90,90);
    /* bottom line */
    graphic_MoveTo(self,x+width-cornerWidth,y+height);
    graphic_DrawLine(self,-(width-2*cornerWidth),0);
    /* lower left corner */
    graphic_DrawArcSize(self,x,y+height-2*cornerHeight,2*cornerWidth,2*cornerHeight,180,90);
    /* right side */
    graphic_MoveTo(self,x,y+height-cornerHeight);
    graphic_DrawLine(self,0,-(height-2*cornerHeight));
}

void graphic__DrawRRect(self,OuterBox, InnerBox)
struct graphic * self;
struct rectangle * OuterBox;
struct rectangle * InnerBox;
{
    graphic_DrawRRectSize(self,rectangle_Left(OuterBox), rectangle_Top(OuterBox), rectangle_Width(OuterBox), rectangle_Height(OuterBox), rectangle_Width(InnerBox), rectangle_Height(InnerBox));
}

void graphic__DrawRgn(self,Rgn)
struct graphic * self;
struct region * Rgn;
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: DrawRgn not impletemented for this wm\n");
    }
}

void graphic__DrawTrapezoid(self,topX, topY, topWidth, bottomX, bottomY, bottomWidth)
struct graphic *self;
long topX, topY, topWidth, bottomX, bottomY, bottomWidth;
{
    graphic_MoveTo(self,topX,topY);
    graphic_DrawLine(self,topWidth,0);
    graphic_DrawLineTo(self,bottomX+bottomWidth,bottomY);
    graphic_DrawLine(self,-bottomWidth,0);
    graphic_DrawLineTo(self,topX,topY);
}


void graphic__FillRectSize(self,x,y,width,height,Tile)
struct graphic * self;
long x,y,width,height;
struct graphic * Tile; 
{
    static boolean printed = FALSE;

    if (width <= 0 || height <= 0) return;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: FillRectSize not implemented for this wm\n");
    }
}

void graphic__FillRect(self, Rect, Tile)
struct graphic * self;
struct rectangle * Rect;
struct graphic * Tile;
{
    graphic_FillRectSize(self,rectangle_Left(Rect),
			  rectangle_Top(Rect), rectangle_Width(Rect),
			  rectangle_Height(Rect),Tile);
}

void graphic__FillPolygon(self,PointArray, PointCount, Tile)
struct graphic * self;
struct point * PointArray;
short PointCount;
struct graphic * Tile;
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: FillPolygon not implemented for this wm\n");
    }
}

void graphic__FillOvalSize(self,x,y,width,height,Tile)
struct graphic * self;
long x,y,width,height;
struct graphic * Tile;
{
    /* ************************
      **** test dummy code ****
      ********************** */
    graphic_FillRectSize(self,x,y,width,height,Tile);
}

void graphic__FillOval(self, Rect, Tile)
struct graphic * self;
struct rectangle * Rect;
struct graphic * Tile;
{
    graphic_FillOvalSize(self,rectangle_Left(Rect), rectangle_Top(Rect), rectangle_Width(Rect), rectangle_Height(Rect), Tile);
}

void graphic__FillArcSize(self,x,y,width,height,StartAngle, OffsetAngle,Tile)
struct graphic * self;
long x,y,width,height;
short StartAngle;
short OffsetAngle;
struct graphic * Tile;
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: FillArcSize not implemented for this wm\n");
    }
    graphic_FillRectSize(self,x,y,width,height,Tile);
}

void graphic__FillArc(self, EnclRect,StartAngle, OffsetAngle,Tile)
struct graphic * self;
struct rectangle * EnclRect;
short StartAngle;
short OffsetAngle;
struct graphic * Tile;
{
    graphic_FillArcSize(self,rectangle_Left(EnclRect), rectangle_Top(EnclRect),
			 rectangle_Width(EnclRect), rectangle_Height(EnclRect),
			 StartAngle, OffsetAngle, Tile);
}

void graphic__FillRRectSize(self,x,y,width,height,cornerWidth,cornerHeight,Tile)
struct graphic * self;
long x,y,width,height;
long cornerWidth, cornerHeight;
struct graphic * Tile;
{
    static boolean printed = FALSE;

    /* Handle pathologic cases in system indepedent manner
      (luser desires to bite bullet in efficiency) */

    if ( (2*cornerHeight >= height) || (2*cornerWidth >= width)) {
	/* Bizarre -- corners are bigger than rectangle, so 
	 make an appropriate looking oval */
	if ( (2*cornerHeight >= height) && (2*cornerWidth >= width))
	    graphic_FillOvalSize(self,x,y,width,height,Tile);
	else if (2*cornerHeight >= height) {
	    /* Fill left semi-oval */
	    graphic_FillArcSize(self,x,y,2*cornerWidth,height,0,-180,Tile);
	    /* Fill vertical rectangle */
	    graphic_FillRectSize(self,x+cornerWidth,y,width-2*cornerWidth,height,Tile);
	    /* Fill right semi-oval */
	    graphic_FillArcSize(self,x+width-2*cornerWidth,y,2*cornerWidth,height,0,180,Tile);
	}
	else { /* assuming (2*cornerWidth >= width) */
	    /* Fill top semi-oval */
	    graphic_FillArcSize(self,x,y,width,2*cornerHeight,-90,180,Tile);
	    /* Fill horizontal rectangle */
	    graphic_FillRectSize(self,x,y+cornerHeight,width,height-2*cornerHeight,Tile);
	    /* Fill bottom semi-oval */
	    graphic_FillArcSize(self,x,y+height-2*cornerHeight,width,2*cornerHeight,90,180,Tile);
	}
	return;
    }

    /* ************************
      **** test dummy code ****
      ********************** */
    graphic_FillRectSize(self,x,y,width,height,Tile);
    if (!printed){
	printed = TRUE;
	fprintf(stderr, "graphic: FillRRectSize not implemented in this wm\n");
    }
}

void graphic__FillRRect(self,OuterBox,InnerBox,Tile)
struct graphic * self;
struct rectangle * OuterBox;
struct rectangle * InnerBox;
struct graphic * Tile;
{
    graphic_FillRRectSize(self,rectangle_Left(OuterBox),rectangle_Top(OuterBox),
			   rectangle_Width(OuterBox), rectangle_Height(OuterBox),
			   rectangle_Width(InnerBox), rectangle_Height(InnerBox), Tile);
}

void graphic__FillRgn(self,Rgn,Tile)
struct graphic * self;
struct region * Rgn;
struct graphic * Tile;
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: FillRgn not implemented for this wm\n");
    }
}

void graphic__FillTrapezoid(self,topX, topY, topWidth, bottomX, bottomY, bottomWidth, Tile)
struct graphic * self, *Tile;
long topX, topY, topWidth, bottomX, bottomY, bottomWidth; 
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: FillTrapezoid not implemented for this wm\n");
    }
}


void graphic__EraseRect(self,Rect)
struct graphic * self;
struct rectangle * Rect;
{
    graphic_FillRect(self,Rect,graphic_WhitePattern(self));
}

void graphic__EraseRectSize(self, x, y, width, height)
struct graphic * self;
long x,y,width,height;
{
    graphic_FillRectSize(self,x,y,width,height,graphic_WhitePattern(self));
}

void graphic__EraseVisualRect(self)
struct graphic * self;
{
    graphic_FillRectSize(self,
			  rectangle_Left(&self->visualBounds),
			  rectangle_Top(&self->visualBounds),
			  rectangle_Width(&self->visualBounds),
			  rectangle_Height(&self->visualBounds),
			  graphic_WhitePattern(self));
}


void graphic__BitBltSize(self, srcX, srcY, dstX, dstY, Width, Height, DstGraphic, clipX, clipY, clipWidth, clipHeight)
struct graphic * self;
long srcX, srcY, dstX, dstY, Width, Height;
struct graphic * DstGraphic;
long clipX, clipY, clipWidth, clipHeight;
{
    struct rectangle tmpSrcRect, tmpClipRect;
    struct point tmpDestOrigin;
    struct rectangle * passedClipRect;

    if ((clipWidth <= 0) || (clipHeight <= 0)) passedClipRect = NULL;
    else {
	passedClipRect = &tmpClipRect;
	rectangle_SetRectSize(&tmpClipRect,clipX,clipY,clipWidth,clipHeight);
    }
    rectangle_SetRectSize(&tmpSrcRect,srcX, srcY, Width, Height);
    point_SetPt(&tmpDestOrigin,dstX,dstY);
    graphic_BitBlt(self,&tmpSrcRect,DstGraphic,&tmpDestOrigin,passedClipRect);
}

void graphic__BitBlt(self, SrcRect, DstGraphic, DstOrigin, ClipRect)
struct graphic * self;
struct rectangle * SrcRect;
struct graphic *DstGraphic;
struct point * DstOrigin;
struct rectangle * ClipRect;
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: BitBlt not implemented for this wm\n");
    }
}


void graphic__WritePixImage(self, DestX, DestY, SrcPixels, SrcX, SrcY, width, height)
struct graphic *self;
long DestX, DestY, SrcX, SrcY, width, height;
struct pixelimage *SrcPixels;
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: WritePixImage not implemented for this wm\n");
    }
}

void graphic__ReadPixImage(self, SrcX, SrcY, DestPixels, DestX, DestY, width, height)
struct graphic *self;
struct pixelimage *DestPixels;
long SrcX, SrcY, DestX, DestY, width, height;
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: ReadPixImage not implemented for this wm\n");
    }
}


void graphic__SetBitAtLoc(self,XPos,YPos, NewValue)
struct graphic * self;
long YPos;
long XPos;
boolean NewValue;
{
    struct point * tempPt;
    short tempLineWidth, tempTMode;

    /* Slow, but correct -- instead of filling a rectangle, we should
      probably draw a one pixel long, one pixel wide line. */
    graphic_GetCurrentPt(self,&tempPt);
    tempLineWidth = graphic_GetLineWidth(self);
    tempTMode = graphic_GetTransferMode(self);

    if (NewValue==TRUE)
	graphic_SetTransferMode(self,graphic_BLACK);
    else if (NewValue==FALSE)
	graphic_SetTransferMode(self,graphic_WHITE);
    else fprintf(stderr, "graphic_SetBitAtLoc: Bad bit value: %d\n",NewValue);

    graphic_MoveTo(self,XPos,YPos);
    graphic_SetLineWidth(self,1);
    graphic_DrawLineTo(self,XPos,YPos); /* yep, this draws one dot in wm */

    graphic_MoveToPt(self,&tempPt);
    graphic_SetLineWidth(self,tempLineWidth);
    graphic_SetTransferMode(self,tempTMode);

}

void graphic__MoveLogicalOrigin(self,DeltaX, DeltaY)
struct graphic * self;
long DeltaY;
long DeltaX;
{
    point_OffsetPoint(&self->savedOrigin,DeltaX,DeltaY);
    rectangle_OffsetRect(&self->localBounds,DeltaX,DeltaY);
    rectangle_OffsetRect(&self->visualBounds,DeltaX,DeltaY);
    /* *********BUG ************* */
    /*   Must change clipping rectangle  */
    /* specification uncertainity: should currentpoint be mnoved? */
    /* view bug : all children should have enclosed origins reset */
    /* *************** */
}

void graphic__SetLogicalOrigin(self,NewX,NewY)
struct graphic * self;
long NewX, NewY;
{
    graphic_MoveLogicalOrigin(self,NewX-graphic_GetLogicalLeft(self),
			       NewY-graphic_GetLogicalTop(self));
}

void graphic__SetLogicalOriginPt(self,Pt)
struct graphic * self;
struct point * Pt;
{
    graphic_SetLogicalOrigin(self,point_X(Pt),point_Y(Pt));
}


void graphic__GetLogicalBounds(self,Rect)
struct graphic * self;
struct rectangle * Rect;
{
    *Rect = self->localBounds;
}

long graphic__GetLogicalRight(self)
struct graphic * self;
{
    return graphic_GetLogicalLeft(self) + graphic_GetLogicalWidth(self);
}

long graphic__GetLogicalBottom(self)
struct graphic * self;
{
    return graphic_GetLogicalTop(self) + graphic_GetLogicalHeight(self);
}

void graphic__GetEnclosedBounds(self,Rect)
struct graphic * self;
struct rectangle * Rect;
{
    rectangle_SetRectSize(Rect,
			   point_X(&self->enclosedOrigin),
			   point_Y(&self->enclosedOrigin),
			   rectangle_Width(&self->localBounds),
			   rectangle_Height(&self->localBounds));
}


long graphic__GetEnclosedRight(self)
struct graphic * self;
{
    return graphic_GetEnclosedLeft(self) + graphic_GetEnclosedWidth(self);
}

long graphic__GetEnclosedBottom(self)
struct graphic * self;
{
    return graphic_GetEnclosedTop(self) + graphic_GetEnclosedHeight(self);
}


void graphic__SetClippingRect(self, AdditionalRect)
struct graphic * self;
struct rectangle * AdditionalRect;
{
    /* Note: we need some way to remove the clipping rectangle so that
      resizing does the "right" thing */

    if (self->clippingRect)
	*self->clippingRect = *AdditionalRect;
    else self->clippingRect = rectangle_Duplicate(AdditionalRect);
}

void graphic__SetClippingRectSize(self, x, y, w, h)
struct graphic * self;
long x, y, w, h;
{
    struct rectangle r;
    rectangle_SetRectSize(&r, x, y, w, h);
    graphic_SetClippingRect(self, &r);
}

void graphic__ClearClippingRect(self)
struct graphic * self;
{

    if (self->clippingRect) free(self->clippingRect);
    self->clippingRect = NULL;
}

void graphic__GetClippingRect(self,Rect)
struct graphic * self;
struct rectangle * Rect;
{
    if (self->clippingRect) *Rect = *self->clippingRect;
    else graphic_GetVisualBounds(self,Rect);
}

void graphic__SetLineWidth(self,NewLineWidth)
struct graphic * self;
short NewLineWidth;
{

    self->lineWidth = NewLineWidth;

}

short graphic__GetLineWidth(self)
struct graphic * self;
{
    return self->lineWidth;
}

void graphic__SetTransferMode(self,NewTransferMode)
struct graphic * self;
short NewTransferMode;
{
    self->transferMode = 0xFF & NewTransferMode;
}

short graphic__GetTransferMode(self)
struct graphic * self;
{
    return self->transferMode;
}

void graphic__GetVisualBounds(self,Rect)
struct graphic * self;
struct rectangle *Rect;
{
    *Rect = self->visualBounds;
}


long graphic__GetVisualRight(self)
struct graphic * self;
{
    return graphic_GetVisualLeft(self) + graphic_GetVisualWidth(self);
}

long graphic__GetVisualBottom(self)
struct graphic * self;
{
    return graphic_GetVisualTop(self) + graphic_GetVisualHeight(self);
}


void graphic__InsertGraphicSize(self, EnclosingGraphic, xOriginInParent,
				 yOriginInParent, width, height)
struct graphic * self;
struct graphic * EnclosingGraphic;
long xOriginInParent, yOriginInParent, width, height;
{
    struct rectangle r;

    rectangle_SetRectSize(&r,xOriginInParent, yOriginInParent,
			   width,height);
    graphic_InsertGraphic(self,EnclosingGraphic,&r);
}

void graphic__InsertGraphic(self, EnclosingGraphic, EnclosedRectangle)
struct graphic * self;
struct graphic * EnclosingGraphic;
struct rectangle * EnclosedRectangle;
{

    /* Fill in the local bounds for the rectangle (always 0,0 based
						    upon creation) */

    rectangle_SetRectSize(&self->localBounds,point_X(&self->savedOrigin),
			   point_Y(&self->savedOrigin),
			   rectangle_Width(EnclosedRectangle),
			   rectangle_Height(EnclosedRectangle));

    /* Calculate the visual bounds for the newly enclosed rectangle
	in terms of the parent. Start with entire requested area,
	and then clip (intersect) to enclosing graphic */

    self->visualBounds = *EnclosedRectangle;
    rectangle_IntersectRect(&self->visualBounds,&self->visualBounds,
			     &EnclosingGraphic->visualBounds);
    /* Now reorient the rect to local (savedOrigin based) coordinates. */
    rectangle_OffsetRect(&self->visualBounds,
			  -rectangle_Left(EnclosedRectangle),
			  -rectangle_Top(EnclosedRectangle));
    rectangle_OffsetRect(&self->visualBounds,point_X(&self->savedOrigin),
			  point_Y(&self->savedOrigin));

    /* Install the origin relative to the enclosing grphic (just
							     copy over the rectangle that it specified! */
    point_SetPt(&self->enclosedOrigin,
		 rectangle_Left(EnclosedRectangle),
		 rectangle_Top(EnclosedRectangle));

    /* Since the clippingRect is relative to the visual rect, we
      define the clipping rectangle as disappearing with an
      insertion operation */
    if (self->clippingRect) free(self->clippingRect);
    self->clippingRect = NULL;


    /* Now figure out how the local window manager offsets should be
      reset. Note: the origin that we are calculating should be the
      upper left hand corner of the local bounds (0,0 point) of the
      newly created graphic. This way we know that any references to
      a point in the local graphic are exactly offset by the
      the origin we are about to calculate. */

    self->physicalOrigin =
      EnclosingGraphic->physicalOrigin;
    point_OffsetPoint(&self->physicalOrigin,
		       rectangle_Left(EnclosedRectangle),
		       rectangle_Top(EnclosedRectangle));

}


void graphic__RestoreGraphicsState(self)
struct graphic * self;
{

}

void graphic__FlushGraphics(self)
struct graphic * self;
{
}

struct graphic * graphic__WhitePattern(self)
struct graphic * self;
{
    return graphic_GrayPattern(self,0,100);
}

struct graphic * graphic__BlackPattern(self)
struct graphic * self;
{
    return graphic_GrayPattern(self,100,100);
}

struct graphic * graphic__GrayPattern(self, IntensityNum, IntensityDenom)
struct graphic * self;
short IntensityNum, IntensityDenom; 
{
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: GrayPattern not implemented for this wm\n");
    }
    return (struct graphic *) 0;
}

struct graphic * graphic__ColorPattern(self, red, blue, green)
struct graphic * self;
long red, blue, green;
{
    /* Just a place holder */
    static boolean printed = FALSE;
    if (! printed) {
	printed = TRUE;
	fprintf(stderr, "graphic: ColorPattern not implemented for this wm \n");
    }
    return NULL;
}

void graphic__SetDefaultColors(classID, foreground, background)
struct classinfo *classID;
char *foreground;
char *background;
{

    char *tempString;

    if (foregroundColorName != NULL)
	free(foregroundColorName);
    if (foreground != NULL && *foreground && ((tempString = (char *) malloc(strlen(foreground) + 1)) != NULL)) {
	strcpy(tempString, foreground);
	foregroundColorName = tempString;
    }
    else
	foregroundColorName = NULL;

    if (backgroundColorName != NULL)
	free(backgroundColorName);
    if (background != NULL && *background && ((tempString = (char *) malloc(strlen(background) + 1)) != NULL)) {
	strcpy(tempString, background);
	backgroundColorName = tempString;
    }
    else
	backgroundColorName = NULL;
}

void graphic__GetDefaultColors(classID, foreground, background)
char **foreground;
char **background;
{

    if (foreground != NULL)
	*foreground = foregroundColorName;
    if (background != NULL)
	*background = backgroundColorName;
}

/* This stuff is a little messed up now in that the colorName can be set but not
  * gotten. This is a bug that should be fixed. Also note that the sematics of
  * these routines are that the specific routines call the generic routines after
  * they have calculated the correct rgb values for a color name. So the color
      * name and the rgb values should correspond to what is really on the screen...
      * -Z-
      */
void graphic__SetForegroundColor(self, colorName, red, blue, green)
struct graphic * self;
char *colorName;
long red, blue, green;
{
    self->foreRed = red;
    self->foreGreen = green;
    self->foreBlue = blue;
}

void graphic__GetForegroundColor(self, red, blue, green)
struct graphic * self;
long *red, *blue, *green;
{
    if (red) *red = self->foreRed;
    if (blue) *blue = self->foreBlue;
    if (green) *green = self->foreGreen;
}

void graphic__SetBackgroundColor(self, colorName, red, blue, green)
struct graphic * self;
char *colorName;
long red, blue, green;
{
    self->backRed = red;
    self->backGreen = green;
    self->backBlue = blue;
}

void graphic__GetBackgroundColor(self, red, blue, green)
struct graphic * self;
long *red, *blue, *green;
{
    if (red) *red = self->backRed;
    if (blue) *blue = self->backBlue;
    if (green) *green = self->backGreen;
}

long graphic__GetHorizontalResolution(self)
struct graphic * self;
{
    return 80L;
}

long graphic__GetVerticalResolution(self)
struct graphic * self;
{
    return 80L;
}

char * graphic__GetWindowManagerType(self)
struct graphic * self;
{
    return "";
}

long graphic__GetDevice(self)
struct graphic * self;
{
    return 0;
}

/* -------------------------------------------------- */
/*   Predefined procedures */
/* -------------------------------------------------- */

boolean graphic__InitializeObject(classID,self)
struct classheader *classID;
struct graphic *self;
{
    static struct fontdesc *defaultFont = NULL;

    rectangle_SetRectSize(&self->localBounds,0,0,0,0);
    rectangle_SetRectSize(&self->visualBounds,0,0,0,0);
    point_SetPt(&self->savedOrigin,0,0);
    point_SetPt(&self->enclosedOrigin,0,0);
    if (defaultFont == NULL)
	defaultFont = fontdesc_Create("andysans", 0, 12);
    self->currentFont = defaultFont;
    self->internalFont = FALSE;
    self->spaceShim = 0;
    self->transferMode = graphic_COPY;
    self->lineWidth = 1;
    self->clippingRect = (struct rectangle * ) NULL;
    point_SetPt(&self->currentPoint,0,0);

    point_SetPt(&self->physicalOrigin,0,0);

    self->backRed = self->backBlue = self->backGreen = 65535; /* white */
    self->foreRed = self->foreBlue = self->foreGreen = 0; /* black */
    return TRUE;
}

/* ***************************************** */
/*       Class procedures                  */
/* ***************************************** */

struct graphic * graphic__CreateGraphic(classID)
struct classheader *classID;
{
    return im_GetGraphic();
}

void graphic__FinalizeObject(classID,self)
struct classheader *classID;
struct graphic *self;
{
    if (self->internalFont) fontdesc_Destroy(self->currentFont);
    if (self->clippingRect) free(self->clippingRect);
}

