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
	void            (*_CreatePopup) ();
	void            (*_CreatePopupDraw) ();
	void            (*_CreatePopupWrite) ();
	StLineAtr      *_StMenuLineAtr;
	StPaintAtr     *_StMenuPaintAtr;
	StTextAtr      *_StMenuTextAtr;
}StPopupFunctionDescRec;

extern StPopupFunctionDescRec popupfunctiondesc;

#define CreatePopup		(*popupfunctiondesc._CreatePopup)
#define CreatePopupDraw		(*popupfunctiondesc._CreatePopupDraw)
#define CreatePopupWrite	(*popupfunctiondesc._CreatePopupWrite)
#define StMenuLineAtr		(*popupfunctiondesc._StMenuLineAtr)
#define StMenuPaintAtr		(*popupfunctiondesc._StMenuPaintAtr)
#define StMenuTextAtr		(*popupfunctiondesc._StMenuTextAtr)
