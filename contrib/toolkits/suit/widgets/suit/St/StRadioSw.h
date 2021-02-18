/*
 *      Original Source: ./lib/Xaw/Command.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
/*
* $XConsortium: Command.h,v 1.17 88/10/23 13:28:49 swick Exp $
*/

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

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

#ifndef _StRadioSwitch_h
#define _StRadioSwitch_h

/***********************************************************************
 *
 * SUIT Radio Switch Widget
 *
 ***********************************************************************/

#include <St/StMaskSw.h>

/* Resources:

 Name                Class              RepType         Default Value
 ----                -----              -------         -------------
 background          Background         Pixel           XtDefaultBackground
 backgroundPixmap    Pixmap             Pixmap          None
 bitmap              Pixmap             Pixmap          None
 borderColor         BorderColor        Pixel           XtDefaultForeground
 borderPixmap        Pixmap             Pixmap          None
 borderWidth         BorderWidth        Dimension       1
 callback            Callback           Pointer         NULL
 callbackInterval    StCIntervalTime    int             0
 cursor              Cursor             Cursor          None
 contents            StCContents        Boolean         False
 destroyCallback     Callback           Pointer         NULL
 drawable            Pixmap             Pixmap          None
 font                Font               XFontStruct*    XtDefaultFont
 foreground          Foreground         Pixel           XtDefaultForeground
 height              Height             Dimension       0
 insensitivePixmap   Pixmap             Pixmap          None
 justify             Justify            XtJustify       XtJustifyCenter
 label               Label              String          NULL
 mappedWhenManaged   MappedWhenManaged  Boolean         True
 marginX             StCLabelMargin     Dimension       2
 marginY             StCLabelMargin     Dimension       0
 maxHeight           StCMaxSize         Dimension       65535
 maxWidth            StCMaxSize         Dimension       65535
 minHeight           StCMinSize         Dimension       0
 minWidth            StCMinSize         Dimension       0
 pixmapForUnset      Pixmap             Pixmap          None
 pixmapForSet        Pixmap             Pixmap          None
 sensitive           Sensitive          Boolean         True
 transparence        StCTransparence    Boolean         False
 width               Width              Dimension       0
 x                   Position           Position        0
 y                   Position           Position        0

*/

extern WidgetClass    stRadioSwitchWidgetClass;

typedef struct _StRadioSwitchClassRec    *StRadioSwitchWidgetClass;
typedef struct _StRadioSwitchRec         *StRadioSwitchWidget;

#endif _StRadioSwitch_h
/* DON'T ADD STUFF AFTER THIS */
