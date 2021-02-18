/*
* $Header: CheckItemI.h,v 1.8 87/12/04 10:58:49 swick Exp $
*/

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 * 
 *                         All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.  
 * 
 * 
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* 
 * CheckItemI.h - Private definitions for CheckItem widget
 *
 * Author:      Robert P. Shen
 *              Malcolm Forbes
 *              Digital Equipment Corporation
 *              ESG Systems Engineering
 *
 * Date:        February 1, 1988
 */
/*
 *
 *  A few definitions to make CheckItem.c easier to read.
 *   
 */

  /* Yes, this is gross, but the code will be easier to read.
     Trust me.  */
#define ComWx                            cbw->core.x
#define ComWy                            cbw->core.y
#define ComWdepth                        cbw->core.depth
#define ComWwidth                        cbw->core.width
#define ComWheight                       cbw->core.height
#define ComWborder_width                 cbw->core.border_width
#define ComWbackground                   cbw->core.background_pixel
#define ComWforeground                   cbw->label.foreground
#define ComWfont                         cbw->label.font
#define ComWlabel                        cbw->label.label
#define ComWjustify                      cbw->label.justify
#define ComWinternalWidth                cbw->label.internal_width
#define ComWinternalHeight               cbw->label.internal_height
#define ComWlabelX                       cbw->label.label_x
#define ComWlabelY                       cbw->label.label_y
#define ComWlabelWidth                   cbw->label.label_width
#define ComWlabelHeight                  cbw->label.label_height
#define ComWlabelLen                     cbw->label.label_len
#define ComWnormalGC                     cbw->label.normal_GC
#define ComWgrayGC                       cbw->label.gray_GC
#define ComWgrayPixmap                   cbw->label.gray_pixmap
#define ComWsensitive                    cbw->core.sensitive
#define ComWcallbackList                 cbw->checkitem.callback_list
#define ComWcallback                     cbw->checkitem.callback
#define ComWclosure                      cbw->checkitem.closure
#define ComWhighlightGC                  cbw->checkitem.highlight_GC
#define ComWeraseHighGC                  cbw->checkitem.erase_highlight_GC
#define ComWinverseGC                    cbw->checkitem.inverse_GC
#define ComWinverseTextGC                cbw->checkitem.inverse_text_GC
#define ComWhighlightThickness           cbw->checkitem.highlight_thickness
#define ComWset                          cbw->checkitem.set
#define ComWhighlighted                  cbw->checkitem.highlighted
#define ComWdisplaySet                   cbw->checkitem.display_set
#define ComWdisplayHighlighted           cbw->checkitem.display_highlighted
#define ComWcheckState			 cbw->checkitem.check_state

#define XtCBField(cbw,field)  cbw->checkitem.field
#define XtLField(cbw,field)   cbw->label.field
#define XtCField(cbw,field)   cbw->core.field


static void Initialize();
static void Redisplay();
static Boolean SetValues();
static void Set();
static void Notify();
static void Unset();
static void Highlight();
static void Unhighlight();
static void Unset();
static void Destroy();
