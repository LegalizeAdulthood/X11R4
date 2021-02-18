/*
 * $XConsortium: casts.h,v 1.3 89/05/05 12:18:40 jim Exp $
 *
 * Original source Copyright 1988 Exceptions
 * X11 portions Copyright 1989 Concurrent Computer Corporation
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Exceptions and Concurrent not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Exceptions and Concurrent make
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 * 
 * EXCEPTIONS AND CONCURRENT DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL EXCEPTIONS OR CONCURRENT BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Author:  Mark Holm, Exceptions, nosun!tessi!exc!markh@sun.com
 * 
 * Acknowledgments to Dorothy Robinson for her artistic abilities in drawing
 * the icons and to Jim Batch for technical support and graphical concepts 
 * Jim Batch for techical support and graphical concepts.
 * 
 * X11 port by:  Jay Estabrook, Concurrent Computer Corporation
 */

/* temporary casts */

typedef int Frame;
typedef int Panel;
typedef Pixmap Panel_item;
typedef int Pixwin;

struct icon { int i; };
struct pixrect { int i; };

typedef Pixmap Pixrect;
typedef XEvent Event;
#define DEFINE_ICON_FROM_IMAGE(a,b) \
    Pixmap (b);
#define mpr_static(a,b,c,d,e) \
    Pixmap (e);

#define LeftButton Button1
#define MiddleButton Button2
#define RightButton Button3

#define MS_LEFT   Button1
#define MS_MIDDLE Button2
#define MS_RIGHT  Button3

#define event_id(p) (((XEvent *)(p))->type)
#define event_x(p) (((XButtonEvent *)(p))->x)
#define event_y(p) (((XButtonEvent *)(p))->y)
#define event_is_up(p) ((boolean)((XEvent *)(p))->type == ButtonRelease)
#define event_is_down(p) ((boolean)((XEvent *)(p))->type == ButtonPress)
#define event_button(p) (((XButtonEvent *)(p))->button && 0xff)
