/*
 *	Original Source: ./lib/Xaw/CommandI.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

/*
* $XConsortium: CommandI.h,v 1.15 88/10/18 12:36:05 swick Exp $
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

/*
 *
 *  A few definitions to make Comch.c easier to read.
 *   
 */
#define RateofCom                            0.7

  /* Yes, this is gross, but the code will be easier to read.
     Trust me.  */
#define ComWx                            cw->core.x
#define ComWy                            cw->core.y
#define ComWdepth                        cw->core.depth
#define ComWwidth                        cw->core.width
#define ComWheight                       cw->core.height
#define ComWborder_width                  cw->core.border_width
#define ComWbackground                   cw->core.background_pixel
#define ComWborderpixel                  cw->core.border_pixel
#define ComWborderpixmap                 cw->core.border_pixmap
#define ComWbordercolor                 cw->core.border_color
#define ComWforeground                 cw->label.foreground
#define ComWnormalGC                     cw->com.normal_GC
#define ComWgrayGC                       cw->com.gray_GC
#define ComWnormalShape                  cw->com.normal_shape
#define ComWwnormalShape                  cw->com.wnormal_shape
#define ComWcallbackList                 cw->com.callback_list
#define ComWcallback                     cw->com.callback
#define ComWclosure                      cw->com.closure
#define ComWswitchon                     cw->com.switchon
#define ComWset                          cw->com.set
#define ComWline_width                    cw->com.line_width
#define ComWlinepixmap                    cw->com.line_pixmap
#define ComWlinepixel                    cw->com.line_pixel
#define ComWfirst                        cw->com.first

#define XtSField(cw,field)   cw->com.field
#define XtLField(cw,field)   cw->label.field
#define XtCField(cw,field)   cw->core.field

static void Initialize();
static void Redisplay();
static Boolean SetValues();
static void Set();
static void Notify();
static void Unset();
static void Setmotion();
static void Destroy();
static void CreatecomPixmap();
