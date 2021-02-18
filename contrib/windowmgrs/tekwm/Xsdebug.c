#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Xsdebug.c,v 1.3 89/11/22 15:15:22 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1989 by Tektronix, Inc. Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of Tektronix not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written prior
 * permission.  Tektronix makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

#include <stdio.h>
#include "X11/Xlib.h"

int XsSyncFunction(dpy)
Display		*dpy;
{
	XSync(dpy,0);
	fflush(stdout); 	/* so user debug messages will appear syncronously */
}

#ifndef BUFSIZ
#define BUFSIZ 160
#endif

char *get_error_code_text();
char *get_request_code_text();

int XsError(dpy, event)
Display		*dpy;
XErrorEvent	*event;
{
	char buffer[BUFSIZ];
    char mesg[BUFSIZ];
    char number[32];
    char *mtype = "XlibMessage";

    XGetErrorDatabaseText(dpy, mtype, "XError", "Error in client operation found by X server:", mesg, BUFSIZ);
    sprintf(number, "%d", event->error_code);
    XGetErrorDatabaseText(dpy, "XErrorCode", number, 
					get_error_code_text(event->error_code), buffer, BUFSIZ);
    (void) fprintf(stderr, "%s: %s ", mesg, buffer);
    XGetErrorText(dpy, event->error_code, buffer, BUFSIZ);
    (void) fprintf(stderr, "(%s)\n  ", buffer);
    XGetErrorDatabaseText(dpy, mtype, "MajorCode", "Request Major code: %d", 
	mesg, BUFSIZ);
    (void) fprintf(stderr, mesg, event->request_code);
    sprintf(number, "%d", event->request_code);
    XGetErrorDatabaseText(dpy, "XRequest", number, 
					get_request_code_text(event->request_code), buffer, BUFSIZ);
    (void) fprintf(stderr, ": %s\n  ", buffer);
    XGetErrorDatabaseText(dpy, mtype, "MinorCode", "Request Minor code: %d", 
	mesg, BUFSIZ);
    (void) fprintf(stderr, mesg, event->minor_code);
    fputs("\n  ", stderr);
    XGetErrorDatabaseText(dpy, mtype, "ResourceID", "ResourceID: 0x%x",
	mesg, BUFSIZ);
    (void) fprintf(stderr, mesg, event->resourceid);
    fputs("\n  ", stderr);
    XGetErrorDatabaseText(dpy, mtype, "ErrorSerial", "Error Serial #%d", 
	mesg, BUFSIZ);
    (void) fprintf(stderr, mesg, event->serial);
    fputs("\n  ", stderr);
    XGetErrorDatabaseText(dpy, mtype, "CurrentSerial", "Current Serial #%d",
	mesg, BUFSIZ);
    (void) fprintf(stderr, mesg, dpy->request);
    fputs("\n", stderr);
    return (0);
}

char *get_error_code_text(error_code)
int error_code;
{
	static char *errcodes[] =
	{
		 "Success"
		,"BadRequest"
		,"BadValue"
		,"BadWindow"
		,"BadPixmap"
		,"BadAtom"
		,"BadCursor"
		,"BadFont"
		,"BadMatch"
		,"BadDrawable"
		,"BadAccess"
		,"BadAlloc"
 		,"BadColor"
 		,"BadGC"
		,"BadIDChoice"
		,"BadName"
		,"BadLength"
		,"BadImplementation"
		,"Non-standard error code - possibly from an extension."
	};
	return( ((error_code >= 0) && 
			 (error_code < (sizeof(errcodes)/sizeof(char *) - 1))) ?
			errcodes[error_code] : 
			errcodes[sizeof(errcodes)/sizeof(char *) - 1] );
}

char *get_request_code_text(request_code)
int request_code;
{
	static char *requestcodes[] =
	{
		 "Undefined X request code"
		,"X_CreateWindow"
		,"X_ChangeWindowAttributes"
		,"X_GetWindowAttributes"
		,"X_DestroyWindow"
		,"X_DestroySubwindows"
		,"X_ChangeSaveSet"
		,"X_ReparentWindow"
		,"X_MapWindow"
		,"X_MapSubwindows"
		,"X_UnmapWindow"
		,"X_UnmapSubwindows"
		,"X_ConfigureWindow"
		,"X_CirculateWindow"
		,"X_GetGeometry"
		,"X_QueryTree"
		,"X_InternAtom"
		,"X_GetAtomName"
		,"X_ChangeProperty"
		,"X_DeleteProperty"
		,"X_GetProperty"
		,"X_ListProperties"
		,"X_SetSelectionOwner"
		,"X_GetSelectionOwner"
		,"X_ConvertSelection"
		,"X_SendEvent"
		,"X_GrabPointer"
		,"X_UngraPointer"
		,"X_GrabButton"
		,"X_UngrabButton"
		,"X_ChangeActivePointerGrab"
		,"X_GrabKeyboard"
		,"X_UngrabKeyboard"
		,"X_GrabKey"
		,"X_UngrabKey"
		,"X_AllowEvents"
		,"X_GrabServer"
		,"X_UngrabServer"
		,"X_QueryPointer"
		,"X_GetMotionEvents"
		,"X_TranslateCoords"
		,"X_WarpPointer"
		,"X_SetInputFocus"
		,"X_GetInputFocus"
		,"X_QueryKeymap"
		,"X_OpenFont"
		,"X_CloseFont"
		,"X_QueryFont"
		,"X_QueryTextExtents"
		,"X_ListFonts"
		,"X_ListFontsWithInfo"
		,"X_SetFontPath"
		,"X_GetFontPath"
		,"X_CreatePixmap"
		,"X_FreePixmap"
		,"X_CreateGC"
		,"X_ChangeGC"
		,"X_CopyGC"
		,"X_SetDashes"
		,"X_SetClipRectangles"
		,"X_FreeGC"
		,"X_ClearArea"
		,"X_CopyArea"
		,"X_CopyPlane"
		,"X_PolyPoint"
		,"X_PolyLine"
		,"X_PolySegment"
		,"X_PolyRectangle"
		,"X_PolyArc"
		,"X_FillPoly"
		,"X_PolyFillRectangle"
		,"X_PolyFillArc"
		,"X_PutImage"
		,"X_GetImage"
		,"X_PolyText8"
		,"X_PolyText16"
		,"X_ImageText8"
		,"X_ImageText16"
		,"X_CreateColormap"
		,"X_FreeColormap"
		,"X_CopyColormapAndFree"
		,"X_InstallColormap"
		,"X_UninstallColormap"
		,"X_ListInstalledColormaps"
		,"X_AllocColor"
		,"X_AllocNamedColor"
		,"X_AllocColorCells"
		,"X_AllocColorPlanes"
		,"X_FreeColors"
		,"X_StoreColors"
		,"X_StoreNamedColor"
		,"X_QueryColors"
		,"X_LookupColor"
		,"X_CreateCursor"
		,"X_CreateGlyphCursor"
		,"X_FreeCursor"
		,"X_RecolorCursor"
		,"X_QueryBestSize"
		,"X_QueryExtension"
		,"X_ListExtensions"
		,"X_ChangeKeyboardMapping"
		,"X_GetKeyboardMapping"
		,"X_ChangeKeyboardControl"
		,"X_GetKeyboardControl"
		,"X_Bell"
		,"X_ChangePointerControl"
		,"X_GetPointerControl"
		,"X_SetScreenSaver"
		,"X_GetScreenSaver"
		,"X_ChangeHosts"
		,"X_ListHosts"
		,"X_SetAccessControl"
		,"X_SetCloseDownMode"
		,"X_KillClient"
		,"X_RotateProperties"
		,"X_ForceScreenSaver"
		,"X_SetPointerMapping"
		,"X_GetPointerMapping"
		,"X_SetModifierMapping"
		,"X_GetModifierMapping"
		,"Undefined X request code"
		,"Undefined X request code"
		,"Undefined X request code"
		,"Undefined X request code"
		,"Undefined X request code"
		,"Undefined X request code"
		,"Undefined X request code"
		,"X_NoOperation"
		,"Non-standard request code - possibly for an extension."
	};
	return( ((request_code >= 0) && 
			 (request_code < (sizeof(requestcodes)/sizeof(char *) - 1))) ?
			requestcodes[request_code] : 
			requestcodes[sizeof(requestcodes)/sizeof(char *) - 1] );
}
