/*
 * $XConsortium: Perfmon.h,v 1.2 89/09/19 14:44:09 jim Exp $
 * 
 * Perfmon Performance Monitor Widget
 *
 * Copyright 1989, PCS Computer Systeme GmbH, West Germany
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of PCS not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  PCS makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL PCS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Author:  Emanuel Jay Berkenbilt, MIT Project Athena
 * Author:           Thomas A. Baghli, PCS Computer Systeme GmbH, West Germany
 *                   tom@meepmeep.pcs.com
 */

#ifndef _XtPerfmon_h
#define _XtPerfmon_h

/***********************************************************************
 *
 * Perfmon Widget
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		White
 border		     BorderColor	Pixel		Black
 borderWidth	     BorderWidth	Dimension	1
 collisionPackets    CollisionPackets   Boolean		True
 diskTransfers	     DiskTransfers	Boolean		True
 font		     Font		XFontStruct*	XtDefaultFont
 foreground	     Foreground		Pixel		Black
 freeMem	     FreeMem		Boolean		True
 height		     Height		Dimension	200
 idleCpu	     IdleCpu		Boolean		True
 inputPackets	     InputPackets	Boolean		True
 interrupts	     Interrupts		Boolean		True
 outputPackets	     OutputPackets	Boolean		True
 reverseVideo	     ReverseVideo	Boolean		False
 stepSize	     StepSize		int		1
 systemCpu	     SystemCpu		Boolean		True
 update		     Interval		int		5 (seconds)
 userCpu	     UserCpu		Boolean		True
 width		     Width		Dimension	200
 x		     Position		Position	0
 y		     Position		Position	0
*/

#define XtNcollisionPackets "collisionPackets"
#define XtNdiskTransfers "diskTransfers"
#define XtNfreeMem "freeMem"
#define XtNidleCpu "idleCpu"
#define XtNinputPackets "inputPackets"
#define XtNinterrupts "interrupts"
#define XtNoutputPackets "outputPackets"
#define XtNstepSize "stepSize"
#define XtNsystemCpu "systemCpu"
#define XtNupdate "update"
#define XtNuserCpu "userCpu"

#define XtCStepSize "StepSize"

typedef struct _PerfmonRec *PerfmonWidget;
typedef struct _PerfmonClassRec *PerfmonWidgetClass;

extern WidgetClass perfmonWidgetClass;

#endif /* _XtPerfmon_h */
