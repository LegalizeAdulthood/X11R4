/*
 *      Original Source: ./contrib/windowmgrs/twm/parse.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

/*****************************************************************************/
/**       Copyright 1988 by Evans & Sutherland Computer Corporation,        **/
/**                          Salt Lake City, Utah                           **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    name  of Evans & Sutherland  not be used in advertising or publi-    **/
/**    city pertaining to distribution  of the software without  specif-    **/
/**    ic, written prior permission.                                        **/
/**                                                                         **/
/**    EVANS  & SUTHERLAND  DISCLAIMS  ALL  WARRANTIES  WITH  REGARD  TO    **/
/**    THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILI-    **/
/**    TY AND FITNESS, IN NO EVENT SHALL EVANS &  SUTHERLAND  BE  LIABLE    **/
/**    FOR  ANY  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY  DAM-    **/
/**    AGES  WHATSOEVER RESULTING FROM  LOSS OF USE,  DATA  OR  PROFITS,    **/
/**    WHETHER   IN  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS    **/
/**    ACTION, ARISING OUT OF OR IN  CONNECTION  WITH  THE  USE  OR PER-    **/
/**    FORMANCE OF THIS SOFTWARE.                                           **/
/*****************************************************************************/

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

/**********************************************************************
 *
 * $Header: parse.h,v 1.3 88/04/26 08:03:18 tlastran Exp $
 *
 * .swmrc parsing externs
 *
 *  8-Apr-88 Tom LaStrange        Initial Version.
 *
 **********************************************************************/

#ifndef _PARSE_
#define _PARSE_

extern void ParseSwmrc();
extern char SwmInput();
extern void SwmUnput();
extern void SwmOutput();

#endif _PARSE_
