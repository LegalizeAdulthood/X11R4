/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xcursor.c,v 1.5 89/05/21 23:04:45 wjh Exp $ */
/* $ACIS:cursor.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xcursor.c,v $ */

#ifndef lint
	char *xcursor_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xcursor.c,v 1.5 89/05/21 23:04:45 wjh Exp $";
#endif /* lint */


#include <class.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <fontdesc.ih>
#include <xfontd.ih>
#include <xgraphic.ih>
#include <cursor.ih>
#include <view.ih>
#include <im.ih>


static char CursorTable[] = {  'a',
    'a','g','x','H','h','l','u','v','w','|',
    'J',':','C','R','M',';','/','^','s','U',
    'D','@','X','~','}' };
#define lookup(A) ((A >= 0 && A < Cursor_TABLELEN)?  CursorTable[A] :CursorTable[0])

#include <xcursor.eh>

void xcursor__ChangeShape(self)
struct cursor *self;
{
    if(cursor_IsPosted(self)){
	self->changed = TRUE;
	im_PostCursor(self->posted,NULL,self);
    }

    if(cursor_IsWindowCursor(self)){
	self->changed = TRUE;
	im_SetWindowCursor(self->windowim,self);
	}
    if(cursor_IsProcessCursor(self)){
	self->changed = TRUE;
	im_SetProcessCursor(self);
	}
}


#define DEFAULTFONTNAME "icon"
#define DEFAULTFONTSIZE 12
#define DEFAULTMASKNAME "icon"
void xcursor__SetStandard(self,ch)
struct xcursor *self;
short ch;
{
	self->header.cursor.fillFont = fontdesc_Create(DEFAULTFONTNAME,0,DEFAULTFONTSIZE);
	self->header.cursor.fillChar = lookup(ch);
	xcursor_ChangeShape(self);
}

boolean xcursor__InitializeObject(classID,self)
struct classheader *classID;
struct xcursor *self;
{
    self->Xw = 0;
    self->Xc = 0;
    self->Xd = NULL;
    self->forecolor = (XColor *) malloc(sizeof(XColor));
    self->forecolor->pixel = 0 /* or so FontCursor does */;
    self->forecolor->red = self->forecolor->green = self->forecolor->blue = 65535 /* ~0 */ ;
    self->forecolor->flags = 0;
    self->backcolor = (XColor *) malloc(sizeof(XColor));
    self->backcolor->pixel = 0;
    self->backcolor->red = self->backcolor->green = self->backcolor->blue = 0;
    self->backcolor->flags = 0;

    return TRUE;
}


void xcursor__FinalizeObject(classID,self)
struct classheader *classID;
struct xcursor *self;
{
	if(self->Xc) XFreeCursor(self->Xd, self->Xc); /* bogus */
	self->Xc = 0;
	if(self->Xw) XDestroyWindow(self->Xd, self->Xw);
        self->Xw = 0;
	if (self->forecolor)
	    free(self->forecolor);
	self->forecolor = NULL;
	if (self->backcolor)
	    free(self->backcolor);
	self->backcolor = NULL;
}
