/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)
  
                        All Rights Reserved
  
Permission to use, copy, modify and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  
  
IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
  
********************************************************/

/* functions */
typedef struct {
	Boolean		(*_IsWidget) ();
	Boolean		(*_IsFigureType) ();
	int             (*_SetErrorStatus) ();
	int             (*_GetErrorStatus) ();
	int             (*_SetObjAtr) ();
	int             (*_GetObjAtr) ();
	void            (*_DrawObject) ();
	Boolean         (*_IsRegion) ();
	void            (*_RedrawRegion) ();
	void            (*_ConvertMirrorObject) ();
	Boolean         (*_GetFiguresRegion) ();
	void            (*_BuildFigureGroup) ();
	Boolean         (*_DestoryFigureGroup) ();
	void            (*_ConvertRotateObject) ();
	void            (*_RotatePoint) ();
}StMiscFunctionDescRec;

extern StMiscFunctionDescRec miscfunctiondesc;

#define IsWidget		(*miscfunctiondesc._IsWidget)
#define IsFigureType		(*miscfunctiondesc._IsFigureType)
#define SetErrorStatus		(*miscfunctiondesc._SetErrorStatus)
#define GetErrorStatus		(*miscfunctiondesc._GetErrorStatus)
#define SetObjAtr		(*miscfunctiondesc._SetObjAtr)
#define GetObjAtr		(*miscfunctiondesc._GetObjAtr)
#define DrawObject		(*miscfunctiondesc._DrawObject)
#define IsRegion		(*miscfunctiondesc._IsRegion)
#define RedrawRegion		(*miscfunctiondesc._RedrawRegion)
#define ConvertMirrorObject	(*miscfunctiondesc._ConvertMirrorObject)
#define GetFiguresRegion	(*miscfunctiondesc._GetFiguresRegion)
#define BuildFigureGroup	(*miscfunctiondesc._BuildFigureGroup)
#define DestoryFigureGroup	(*miscfunctiondesc._DestoryFigureGroup)
#define ConvertRotateObject	(*miscfunctiondesc._ConvertRotateObject)
#define RotatePoint		(*miscfunctiondesc._RotatePoint)

#define LINE_ATR        0
#define PAINT_ATR       1
#define TEXT_ATR        2

