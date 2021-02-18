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

typedef struct {
	void            (*_HandleDrag) ();
	void            (*_HandleMove) ();
	void            (*_HandleCopy) ();
	void            (*_HandleResize) ();
	void            (*_HandleSelect) ();
	void            (*_HandleSelectBox) ();
	void            (*_HandleScrool) ();
	void            (*_FixupObject) ();
	void            (*_FixupMoveObject) ();
	void            (*_FixupCopyObject) ();
	void            (*_FixupResizeObject) ();
	void            (*_FixupSelectBox) ();
	void            (*_FixupSelectObject) ();
	void            (*_FixupScrool) ();
	void            (*_SetResizeDir) ();
	void            (*_SetupArcPram) ();
	void            (*_StartScrool) ();
}StUIFunctionDescRec;

extern StUIFunctionDescRec uifunctiondesc;

#define HandleDrag		(*uifunctiondesc._HandleDrag)
#define HandleMove		(*uifunctiondesc._HandleMove)
#define HandleCopy		(*uifunctiondesc._HandleCopy)
#define HandleResize		(*uifunctiondesc._HandleResize)
#define HandleSelect		(*uifunctiondesc._HandleSelect)
#define HandleSelectBox		(*uifunctiondesc._HandleSelectBox)
#define HandleScrool		(*uifunctiondesc._HandleScrool)
#define FixupObject		(*uifunctiondesc._FixupObject)
#define FixupMoveObject		(*uifunctiondesc._FixupMoveObject)
#define FixupCopyObject		(*uifunctiondesc._FixupCopyObject)
#define FixupResizeObject	(*uifunctiondesc._FixupResizeObject)
#define FixupSelectBox		(*uifunctiondesc._FixupSelectBox)
#define FixupSelectObject	(*uifunctiondesc._FixupSelectObject)
#define FixupScrool		(*uifunctiondesc._FixupScrool)
#define SetResizeDir		(*uifunctiondesc._SetResizeDir)
#define SetupArcPram		(*uifunctiondesc._SetupArcPram)
#define StartScrool		(*uifunctiondesc._StartScrool)

#define Single	0
#define Multi	1
