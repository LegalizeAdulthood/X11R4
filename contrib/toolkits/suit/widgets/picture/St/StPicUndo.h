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
	void            (*_FreeUndoPacket) ();
	void            (*_NewUndoPacket) ();
	void            (*_NewUndoObjects) ();
	void            (*_RealDeleteObjects) ();
	void            (*_SetUndoCopy) ();
	void            (*_SetUndoMove) ();
	void            (*_SetUndoResize) ();
	void            (*_SetUndoDraw) ();
	void            (*_SetUndoPrio) ();
	void            (*_SetUndoMirror) ();
	void            (*_SetUndoRotate) ();
	void            (*_SetUndoDelete) ();
	void            (*_DoUndoMove) ();
	void            (*_DoUndoResize) ();
	void            (*_DoUndoPrio) ();
	void            (*_DoUndoMirror) ();
	void            (*_DoUndoRotate) ();
	void            (*_DoUndoDelete) ();
	void            (*_DoUndoCreate) ();
	void            (*_UndoObject) ();
}StUndoFunctionDescRec;

extern StUndoFunctionDescRec undofunctiondesc;

#define FreeUndoPacket		(*undofunctiondesc._FreeUndoPacket)
#define NewUndoPacket		(*undofunctiondesc._NewUndoPacket)
#define NewUndoObjects		(*undofunctiondesc._NewUndoObjects)
#define RealDeleteObjects	(*undofunctiondesc._RealDeleteObjects)
#define SetUndoCopy		(*undofunctiondesc._SetUndoCopy)
#define SetUndoMove		(*undofunctiondesc._SetUndoMove)
#define	SetUndoResize 		(*undofunctiondesc._SetUndoResize)
#define SetUndoDraw  		(*undofunctiondesc._SetUndoDraw)
#define SetUndoPrio  		(*undofunctiondesc._SetUndoPrio)
#define SetUndoMirror  		(*undofunctiondesc._SetUndoMirror)
#define SetUndoRotate  		(*undofunctiondesc._SetUndoRotate)
#define SetUndoDelete  		(*undofunctiondesc._SetUndoDelete)
#define DoUndoMove  		(*undofunctiondesc._DoUndoMove)
#define	DoUndoResize 		(*undofunctiondesc._DoUndoResize)
#define DoUndoPrio  		(*undofunctiondesc._DoUndoPrio)
#define DoUndoMirror  		(*undofunctiondesc._DoUndoMirror)
#define DoUndoRotate  		(*undofunctiondesc._DoUndoRotate)
#define DoUndoDelete  		(*undofunctiondesc._DoUndoDelete)
#define DoUndoCreate  		(*undofunctiondesc._DoUndoCreate)
#define UndoObject		(*undofunctiondesc._UndoObject)

typedef enum {			/* Undo operation code */
	UNull,
	UAttr,
	UDraw,
	UGroupSet,
	UGroupUnset,
	UPrio,
	UMirror,
	URotate,
	UDelete,
	UCopy,
	UMove,
	UResize,
	UUresize,
	UUcreate,
	UUprio,
	UUgroup
} UndoOperation;
