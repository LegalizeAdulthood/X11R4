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
	void            (*_ChangeLineAttributes) ();
	void            (*_ChangeFillAttributes) ();
	void            (*_ChangeTextAttributes) ();
	void            (*_InitializeFigureAttr) ();
	void            (*_ChangeMenuAttr) ();

	void            (*_CurrentTextStatus) ();
	void            (*_ExitTextEdit) ();
	void            (*_TextEditing) ();
	char           *(*_SaveString) ();
	void            (*_GetTextRegion) ();
	void            (*_DrawTextObject) ();
	int             (*_SetupBitmap) ();
	void            (*_StDrawBitmapPrimitive) ();
	void            (*_DrawImageFile) ();
} StAttrFunctionDescRec;

extern StAttrFunctionDescRec attrfunctiondesc;

#define ChangeLineAttributes		(*attrfunctiondesc._ChangeLineAttributes)
#define ChangeFillAttributes		(*attrfunctiondesc._ChangeFillAttributes)
#define ChangeTextAttributes		(*attrfunctiondesc._ChangeTextAttributes)
#define InitializeFigureAttr		(*attrfunctiondesc._InitializeFigureAttr)
#define ChangeMenuAttr			(*attrfunctiondesc._ChangeMenuAttr)

/* Sat Oct 21 18:06:31 JST 1989 */
#define CurrentTextStatus		(*attrfunctiondesc._CurrentTextStatus)
#define ExitTextEdit			(*attrfunctiondesc._ExitTextEdit)
#define TextEditing			(*attrfunctiondesc._TextEditing)
#define SaveString			(*attrfunctiondesc._SaveString)
#define GetTextRegion			(*attrfunctiondesc._GetTextRegion)
#define DrawTextObject			(*attrfunctiondesc._DrawTextObject)
#define SetupBitmap			(*attrfunctiondesc._SetupBitmap)
#define StDrawBitmapPrimitive		(*attrfunctiondesc._StDrawBitmapPrimitive)
#define DrawImageFile			(*attrfunctiondesc._DrawImageFile)
