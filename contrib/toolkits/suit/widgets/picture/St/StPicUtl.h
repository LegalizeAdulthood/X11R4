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
	StFigureType	(*_GetType) ();
	StDrawFunction	(*_GetMenu) ();
	int             (*_GetSize) ();
	void            (*_CheckCoordLimit) ();
	void            (*_CheckRegionLimit) ();
	void            (*_AddDisp) ();
	void            (*_ToGlobalXY) ();
	void            (*_ToWidgetXY) ();
	void            (*_ScaleObject) ();
	Boolean         (*_NewObjNumber) ();
	Boolean         (*_DelObjNumber) ();
	StHashTbl      *(*_MakeHashTbl) ();
	Boolean         (*_DeleteObjAddr) ();
	void            (*_SetObjAddr) ();
	StObject       *(*_GetObjAddr) ();
	Boolean         (*_IsInBox) ();
	Boolean         (*_IsCrossBox) ();
	Boolean         (*_IsIncludeBox) ();
	void            (*_JoinBox) ();
	char           *(*_NewInqBuf) ();
	Boolean         (*_SetFigureList) ();
	Boolean         (*_SetPointList) ();
	void            (*_SetupTergetFigures) ();
	void            (*_FreeTergetFigures) ();
	Boolean         (*_IsMoveAction) ();
	Boolean         (*_IsResizeAction) ();
	void            (*_Memcpy) ();
	void            (*_SavePoint) ();
	void            (*_StuffPoint) ();
	StDrawControl  *(*_GetDrawControl) ();
	void            (*_SetDrawStat) ();
	void            (*_SaveOldDrawStat) ();
	void            (*_RestorOldDrawStat) ();
	void            (*_MoveView) ();
}StUtlFunctionDescRec;

extern StUtlFunctionDescRec utlfunctiondesc;

#define GetType			(*utlfunctiondesc._GetType)
#define GetMenu			(*utlfunctiondesc._GetMenu)
#define GetSize			(*utlfunctiondesc._GetSize)
#define CheckCoordLimit		(*utlfunctiondesc._CheckCoordLimit)
#define CheckRegionLimit	(*utlfunctiondesc._CheckRegionLimit)
#define AddDisp			(*utlfunctiondesc._AddDisp)
#define ToGlobalXY		(*utlfunctiondesc._ToGlobalXY)
#define ToWidgetXY		(*utlfunctiondesc._ToWidgetXY)
#define ScaleObject		(*utlfunctiondesc._ScaleObject)
#define	NewObjNumber		(*utlfunctiondesc._NewObjNumber)
#define	DelObjNumber		(*utlfunctiondesc._DelObjNumber)
#define	MakeHashTbl		(*utlfunctiondesc._MakeHashTbl)
#define	DeleteObjAddr		(*utlfunctiondesc._DeleteObjAddr)
#define SetObjAddr		(*utlfunctiondesc._SetObjAddr)
#define GetObjAddr		(*utlfunctiondesc._GetObjAddr)
#define IsInBox			(*utlfunctiondesc._IsInBox)
#define IsCrossBox		(*utlfunctiondesc._IsCrossBox)
#define IsIncludeBox		(*utlfunctiondesc._IsIncludeBox)
#define JoinBox			(*utlfunctiondesc._JoinBox)
#define NewInqBuf		(*utlfunctiondesc._NewInqBuf)
#define SetFigureList		(*utlfunctiondesc._SetFigureList)
#define SetPointList		(*utlfunctiondesc._SetPointList)
#define SetupTergetFigures	(*utlfunctiondesc._SetupTergetFigures)
#define FreeTergetFigures	(*utlfunctiondesc._FreeTergetFigures)
#define IsMoveAction		(*utlfunctiondesc._IsMoveAction)
#define IsResizeAction		(*utlfunctiondesc._IsResizeAction)
#define Memcpy			(*utlfunctiondesc._Memcpy)
#define SavePoint		(*utlfunctiondesc._SavePoint)
#define StuffPoint		(*utlfunctiondesc._StuffPoint)
#define GetDrawControl		(*utlfunctiondesc._GetDrawControl)
#define SetDrawStat		(*utlfunctiondesc._SetDrawStat)
#define SaveOldDrawStat		(*utlfunctiondesc._SaveOldDrawStat)
#define RestorOldDrawStat	(*utlfunctiondesc._RestorOldDrawStat)
#define MoveView		(*utlfunctiondesc._MoveView)
