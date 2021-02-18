/*
 *	File:		gr_inc.h
 *	Contents:	Header file containing all the includes and externs
 *				by the graphics module.
 */

#include <stdio.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Box.h>
#include <X11/Command.h>
#include <X11/Dialog.h>
#include <X11/Label.h>
#include <X11/List.h>
#include <X11/Scroll.h>
#include <X11/Shell.h>
#include <X11/Form.h>
#include <X11/AsciiText.h>
#include <X11/Viewport.h>
#include <X11/Cardinals.h>
#include <X11/cursorfont.h>

/*
 * Local externs
 */

extern void		gr_CloseCubeLevel();
extern void		gr_CloseDsplLevel();
extern void		gr_DisplaySetVal();
extern void		gr_DisplayX();
extern void		gr_DisplayY();
extern void		gr_DisplayZ();
extern void		gr_FileDirAccept();
extern void		gr_FileCheck();
extern void		gr_FileLoad();
extern void		gr_Terminate();
extern Widget	gr_InitTopLevel();
extern Widget	gr_InitCubeLevel();
extern Widget	gr_InitDsplLevel();
extern Widget	gr_MakeBox();
extern Widget	gr_MakeButton();
extern Widget	gr_MakeDialog();
extern Widget	gr_MakeList();
extern Widget	gr_MakeText();
extern Widget	gr_MakeVPort();
extern Widget	gr_MakeWindow();
extern char		*gr_TextGetStr();

/*
 * Inter-module externs
 */

extern char		**td_FileDirStrings();
extern char		*td_CurrentDate();
extern char		*td_getDirName();
extern char		*td_getLogFileName();
extern char		*td_getTmpFileName();
extern char		*td_getToolName();
extern char		*td_HdfgetData();
extern char		*td_HdfgetPixMap();
extern char		**td_HdfgetPlaneData();
extern int		td_HdfgetColumns();
extern int		td_HdfgetXDim();
extern int		td_HdfgetYDim();
extern int		td_HdfCheck();
extern int		td_HdfLoad();
extern double	td_getVersion();
