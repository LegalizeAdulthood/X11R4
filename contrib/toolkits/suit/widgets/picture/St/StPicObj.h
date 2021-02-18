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
	Boolean		(*_AllocObject) ();
	void            (*_FreeObject) ();
	void            (*_LinkObject) ();
	void            (*_BreakObject) ();
	void            (*_UnlinkObject) ();
	Boolean         (*_NewGroup) ();
	Boolean         (*_NewFigure) ();
	Boolean         (*_CopyObjectTree) ();
	void            (*_TransformObject) ();
}StObjFunctionDescRec;

extern StObjFunctionDescRec objfunctiondesc;

#define AllocObject		(*objfunctiondesc._AllocObject)
#define FreeObject		(*objfunctiondesc._FreeObject)
#define LinkObject		(*objfunctiondesc._LinkObject)
#define BreakObject		(*objfunctiondesc._BreakObject)
#define UnlinkObject		(*objfunctiondesc._UnlinkObject)
#define NewGroup		(*objfunctiondesc._NewGroup)
#define NewFigure		(*objfunctiondesc._NewFigure)
#define CopyObjectTree		(*objfunctiondesc._CopyObjectTree)
#define TransformObject		(*objfunctiondesc._TransformObject)
