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
	void            (*_TgleGroup) ();
	void            (*_TgleHandle) ();
	void            (*_TgleSelect) ();
	void            (*_SelectAll) ();
	Boolean         (*_IsSelectedFigure) ();
	int             (*_FindHandle) ();
}StSelFunctionDescRec;

extern StSelFunctionDescRec selfunctiondesc;

#define TgleGroup		(*selfunctiondesc._TgleGroup)
#define TgleHandle		(*selfunctiondesc._TgleHandle)
#define TgleSelect		(*selfunctiondesc._TgleSelect)
#define SelectAll		(*selfunctiondesc._SelectAll)
#define IsSelectedFigure	(*selfunctiondesc._IsSelectedFigure)
#define FindHandle		(*selfunctiondesc._FindHandle)

/* handle name definition which set in StDrawControl.select */
#define SLNONE		0
#define SLUPPER_LEFT	1
#define SLUPPER_MIDDLE	2
#define SLUPPER_RIGHT	3
#define SLLEFT		4
#define SLRIGHT		5
#define SLLOWER_LEFT	6
#define SLLOWER_MIDDLE	7
#define SLLOWER_RIGHT	8
