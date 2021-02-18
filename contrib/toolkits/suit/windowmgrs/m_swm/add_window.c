/*
 *	Original Source: ./contrib/windowmgrs/twm/add_window.c
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
 * $Header: add_window.c,v 1.50 88/10/13 07:19:19 toml Exp $
 *
 * Add a new window, put the titlbar and other stuff around
 * the window
 *
 * 31-Mar-88 Tom LaStrange        Initial Version.
 *
 **********************************************************************/

#ifndef lint
static char RCSinfo[]=
"$Header: add_window.c,v 1.50 88/10/13 07:19:19 toml Exp $";
#endif lint

#include <stdio.h>
#include "swm.h"
#include <X11/Xatom.h>
#include "add_window.h"
#include "util.h"
#include "resize.h"
#include "gram.h"
#include "events.h"
#include "menus.h"

#include "iconify.bm"

int AddingX;
int AddingY;
int AddingW;
int AddingH;

static int PlaceX = 50;
static int PlaceY = 50;

char NoName[] = "No Name"; /* name if no name is specified */

extern int Oya;
extern int Desktop;
extern Window NewParent1;
extern Window NewParent2;

/***********************************************************************
 *
 *  Procedure:
 *      SuitWindow - add a new desktop or new window to the swm list
 *
 *  Returned Value:
 *      (SwmWindow *) - pointer to the SwmWindow structure
 *
 *  Inputs:
 *      w       - the window id of the window to add
 *
 ***********************************************************************
 */

SwmWindow *
SuitWindow(w)
Window w;
{
    SwmWindow *tmp_win;                 /* new swm window structure */
    int stat;
    char *prop;
    unsigned long valuemask;            /* mask for create windows */
    XSetWindowAttributes attributes;    /* attributes for create windows */
    int width, len;                     /* tmp variable */
    int junk1, junk2, junk3;
    int x;
    Pixmap pm;                  /* tmp pixmap variable */
    XWindowChanges xwc;         /* change window structure */
    unsigned int xwcm;          /* change window mask */
    int dont_know;              /* don't know where to put the window */
    XColor blob, cret;
    XEvent event;
    XGCValues       gcv;
    unsigned long   gcm, mask;

    int iconBMHeight;
    int labelwidth, labelheight;
    int namewidth;

    XWindowAttributes       info;
    Window                  win;
    int                     suitFlag = TRUE;
    int                     i;
    int                     nchildren;
    Window root,parent,*children;
    Window newParent;
    Window parentWindow;

    SwmWindow *tempDesktop;
    int rx, ry, xx, yy;
    int ii, jj;

#ifdef DEBUG
    fprintf(stderr, "SuitWindow: w = 0x%x\n", w);
#endif

    /* allocate space for the swm window */
    tmp_win = (SwmWindow *)malloc(sizeof(SwmWindow));
    tmp_win->w = w;
    tmp_win->zoomed = ZOOM_NONE;

    tempDesktop = GetDesktopId();
    tmp_win->parent = tempDesktop->windowFlag;

    XQueryTree(dpy,tmp_win->w,&root,&newParent,&children,&nchildren);

    for (ii = 0 ; ii < 20 ; ii++)
       for (jj  = 0; jj < 20 ; jj++)
          tmp_win->iconTable[ii][jj] = 0;
    
    if(Desktop == 1)
    {
        tmp_win -> windowFlag = 1;
        Desktop = 0;
    }
    else
    {
        tmp_win -> windowFlag = 2;
    }

    XFetchName(dpy, tmp_win->w, &tmp_win->window_name);

    XSelectInput(dpy, tmp_win->w, PropertyChangeMask);
    XGetWindowAttributes(dpy, tmp_win->w, &tmp_win->attr);
    XFetchName(dpy, tmp_win->w, &tmp_win->window_name);
    tmp_win->class = NoClass;
    XGetClassHint(dpy, tmp_win->w, &tmp_win->class);

#ifdef DEBUG
    fprintf(stderr, "  name = \"%s\"\n", tmp_win->window_name);
#endif
    tmp_win->wmhints = XGetWMHints(dpy, tmp_win->w);
    if (tmp_win->wmhints && (tmp_win->wmhints->flags & WindowGroupHint))
        tmp_win->group = tmp_win->wmhints->window_group;
    else
        tmp_win->group = NULL;

    if (XGetNormalHints(dpy, tmp_win->w, &tmp_win->hints) == 0)
        tmp_win->hints.flags = 0;

    if (strncmp("xterm", tmp_win->class.res_name, 5) == 0 ||
        strncmp("XTerm", tmp_win->class.res_class, 5) == 0 )
        tmp_win->xterm = TRUE;
    else
        tmp_win->xterm = FALSE;

    dont_know = TRUE;
    if (tmp_win->hints.flags & PPosition)
    {
       int addX, addY;
       int rw, rh, rb, rd;

#ifdef DEBUG
        fprintf(stderr, "  program specified hints\n");
#endif
        GetWindowOffset(TempDesktop, &rx, &ry);
        tmp_win->attr.x = tmp_win->hints.x - rx;
        tmp_win->attr.y = tmp_win->hints.y - ry;
        if (tmp_win->attr.x < 0)
           tmp_win->attr.x = 0;
        if (tmp_win->attr.y < 0)
           tmp_win->attr.y = 0;
        tmp_win->attr.width = tmp_win->hints.width ;
        tmp_win->attr.height = tmp_win->hints.height ;
    }
    if (tmp_win->hints.flags & USPosition)
    {
        int addX, addY;

#ifdef DEBUG
        fprintf(stderr, "  user specified hints\n");
#endif
        dont_know = FALSE;
        GetWindowOffset(TempDesktop, &rx, &ry);
        tmp_win->attr.x = tmp_win->hints.x - rx;
        tmp_win->attr.y = tmp_win->hints.y - ry;
        if (tmp_win->attr.x < 0)
           tmp_win->attr.x = 0;
        if (tmp_win->attr.y < 0)
           tmp_win->attr.y = 0;
        /* tmp_win->attr.width = tmp_win->hints.width-(BorderWidth+EdgeWidth)*2;
        tmp_win->attr.height = tmp_win->hints.height-(BorderWidth+EdgeWidth)*2; */
        tmp_win->attr.width = tmp_win->hints.width;
        tmp_win->attr.height = tmp_win->hints.height;
    }

    if (tmp_win->window_name == NULL)
        tmp_win->window_name = NoName;
    if (tmp_win->class.res_name == NULL)
        tmp_win->class.res_name = NoName;
    if (tmp_win->class.res_class == NULL)
        tmp_win->class.res_class = NoName;

    tmp_win->full_name = tmp_win->window_name;

    tmp_win->highlight = !(short)LookInList(NO_HILITE, tmp_win->full_name,
        &tmp_win->class);
    tmp_win->auto_raise = (short)LookInList(AUTO_RAISE, tmp_win->full_name,
        &tmp_win->class);
    tmp_win->iconify_by_unmapping = IconifyByUnmapping;
    if (IconifyByUnmapping)
    {
        tmp_win->iconify_by_unmapping =
            !(short)LookInList(DONT_ICONIFY_BY_UNMAPPING, tmp_win->full_name,
                &tmp_win->class);
    }
    if (NoTitlebar || LookInList(NO_TITLE, tmp_win->full_name, &tmp_win->class))
        tmp_win->title_height = 0;
    else
        tmp_win->title_height = TITLE_BAR_HEIGHT + BorderWidth ;

    if (HandlingEvents && dont_know && !RandomPlacement)
    {
        if ((!(tmp_win->wmhints && tmp_win->wmhints->flags & StateHint &&
            tmp_win->wmhints->initial_state == IconicState) &&
           (tmp_win->xterm || (tmp_win->attr.x == 0 && tmp_win->attr.y == 0))) ||
            suitFlag)
        {
            /* better wait until all the mouse buttons have been
             * released.
             */
            while (TRUE)
            {
                XUngrabServer(dpy);
                XSync(dpy);
                XGrabServer(dpy);
                XQueryPointer(dpy, Root, &JunkRoot, &JunkChild,
                    &JunkX, &JunkY, &AddingX, &AddingY, &JunkMask);

                /* if ((JunkMask != 257) && (suitFlag != TRUE))
        	    continue; */

                stat = XGrabPointer(dpy, Root, False,
                    ButtonPressMask | ButtonReleaseMask,
                    GrabModeAsync, GrabModeAsync,
                    Root, UpperLeftCursor, CurrentTime);

                if (stat == GrabSuccess)
                    break;
            }

            width = XTextWidth(InitialFont.font, tmp_win->window_name,
                strlen(tmp_win->window_name)) + 20;
            XResizeWindow(dpy, InitialWindow, width, InitialFont.height + 4);
            XMapRaised(dpy, InitialWindow);
            XDrawImageString(dpy, InitialWindow, InitialNormalGC,
                10, 2 + InitialFont.font->ascent,
                tmp_win->window_name, strlen(tmp_win->window_name));

            AddingW = tmp_win->attr.width;
            AddingH = tmp_win->attr.height;

            AddingW = tmp_win->attr.width + 2 * BorderWidth;
            AddingH = tmp_win->attr.height + tmp_win->title_height +
                2 * BorderWidth;

            while (TRUE)
            {
                XQueryPointer(dpy, Root, &JunkRoot, &JunkChild,
                    &JunkX, &JunkY, &AddingX, &AddingY, &JunkMask);
                MoveOutline(Root, AddingX, AddingY, AddingW+EdgeWidth*2, AddingH+EdgeWidth*2);

                if (XCheckTypedEvent(dpy, ButtonPress, &event))
                {
                    AddingX = event.xbutton.x_root;
                    AddingY = event.xbutton.y_root;
                    break;
                }
            }

            if (event.xbutton.button == Button2)
            {
                XWarpPointer(dpy, None, Root, 0, 0, 0, 0,
                    AddingX + AddingW/2, AddingY + AddingH/2);
                AddStartResize(tmp_win, AddingX, AddingY, AddingW, AddingH);

                while (TRUE)
                {
                    int lastx, lasty;

                    XQueryPointer(dpy, Root, &JunkRoot, &JunkChild,
                        &JunkX, &JunkY, &AddingX, &AddingY, &JunkMask);

                    if (lastx != AddingX || lasty != AddingY)
                    {
                        DoResize(AddingX, AddingY, tmp_win);

                        lastx = AddingX;
                        lasty = AddingY;
                    }

                    if (XCheckTypedEvent(dpy, ButtonRelease, &event))
                    {
                        AddEndResize(tmp_win);
                        break;
                    }
                }
            }

            MoveOutline(Root, 0, 0, 0, 0);
            XUnmapWindow(dpy, InitialWindow);
            XUngrabPointer(dpy, CurrentTime);

            if (tmp_win->windowFlag != 2)
               {
               GetWindowOffset(tmp_win, &rx, &ry);
               tmp_win->attr.x = AddingX - rx;
               tmp_win->attr.y = AddingY - ry + tmp_win->title_height;
               }
            else
               {
               if (TempDesktop->windowFlag == 0)
                  {
                   GetWindowOffset(tmp_win, &rx, &ry);
                   tmp_win->attr.x = AddingX - rx;
                   tmp_win->attr.y = AddingY - ry + tmp_win->title_height;
                  }
               else
                  {
                   GetWindowOffset(TempDesktop, &rx, &ry);
                   tmp_win->attr.x = AddingX - rx 
			- TempDesktop->frame_x - EdgeWidth - BorderWidth;
                   tmp_win->attr.y = AddingY - ry 
			- TempDesktop->frame_y - EdgeWidth - BorderWidth;
                  }
                }
            if (tmp_win->attr.x < 0)
              tmp_win->attr.x = 0;
            if (tmp_win->attr.y < 0)
              tmp_win->attr.y = 0;
            if (tmp_win->attr.x >= TempDesktop->attr.width)
              tmp_win->attr.x = TempDesktop->attr.width  - 64; 
            if (tmp_win->attr.y >= TempDesktop->attr.height)
              tmp_win->attr.y = TempDesktop->attr.height  - 64;
            tmp_win->attr.width = AddingW - 2 * BorderWidth;
            tmp_win->attr.height = AddingH - tmp_win->title_height -
                (2 * BorderWidth);

            XUngrabServer(dpy);
        }
    }
    else if (HandlingEvents && dont_know && RandomPlacement)
    {
        GetMousePointer(tmp_win, &rx, &ry);

        if (tmp_win->windowFlag == 1)
           {
           PlaceX = rx-BorderWidth;
           PlaceY = ry-BorderWidth;
           }
        else
           {
           PlaceX = rx-BorderWidth*2;
           PlaceY = ry-BorderWidth*2;
           }

        if ((PlaceX + tmp_win->attr.width) > MyDisplayWidth)
            PlaceX = 50;
        if ((PlaceY + tmp_win->attr.height) > MyDisplayHeight)
            PlaceY = 50;

        tmp_win->attr.x = PlaceX;
        tmp_win->attr.y = PlaceY;
        PlaceX += 30;
        PlaceY += 30;
    }

    if (tmp_win->attr.y < tmp_win->title_height)
        tmp_win->attr.y = tmp_win->title_height;

    xwcm = CWX | CWY | CWWidth | CWHeight | CWBorderWidth;

#ifdef DEBUG
        fprintf(stderr, "  position window  %d, %d  %dx%d\n",
            tmp_win->attr.x,
            tmp_win->attr.y,
            tmp_win->attr.width,
            tmp_win->attr.height);
#endif
    xwc.x = tmp_win->attr.x + tmp_win->attr.border_width;
    xwc.y = tmp_win->attr.y + tmp_win->attr.border_width;
    xwc.width = tmp_win->attr.width;
    xwc.height = tmp_win->attr.height;
    xwc.border_width = 0;

    XConfigureWindow(dpy, tmp_win->w, xwcm, &xwc);

    tmp_win->name_width = XTextWidth(TitleBarFont.font, tmp_win->window_name,
        strlen(tmp_win->window_name));

    if (XGetWindowProperty(dpy, tmp_win->w, XA_WM_ICON_NAME, 0, 200, False,
        XA_STRING, &junk1, &junk2, &junk3, &len, &tmp_win->icon_name))
        tmp_win->icon_name = tmp_win->window_name;

    if (tmp_win->icon_name == NULL)
        tmp_win->icon_name = tmp_win->window_name;

    tmp_win->iconified = FALSE;
    tmp_win->icon = FALSE;
    tmp_win->icon_on = FALSE;

    /* add the window into the swm list */
    tmp_win->next = SwmRoot.next;
    if (SwmRoot.next != NULL)
        SwmRoot.next->prev = tmp_win;
    tmp_win->prev = &SwmRoot;
    SwmRoot.next = tmp_win;

    /* create windows */

    tmp_win->frame_x = tmp_win->attr.x;
    tmp_win->frame_y = tmp_win->attr.y - tmp_win->title_height;

    XGrabServer(dpy);

    if(tmp_win -> windowFlag == 1)
	{
        parentWindow = NewParent1;
	tmp_win->parentId = parentWindow;
        tmp_win->parent = TempDesktop->windowFlag;
        }
    else
        {
        parentWindow = TempDesktop->w;
        tmp_win->parentId = parentWindow;
	tmp_win->parent = TempDesktop->windowFlag;
	}
    XQueryPointer(dpy, Root, &JunkRoot, &JunkChild,&JunkX, &JunkY,
	&AddingX, &AddingY, &JunkMask);

    tmp_win->frame = XCreateSimpleWindow(dpy, parentWindow,
        tmp_win->frame_x,
        tmp_win->frame_y,
        tmp_win->attr.width+EdgeWidth*2,
        tmp_win->attr.height + tmp_win->title_height+EdgeWidth*2,
        BorderWidth,
        BorderColor, TitleC.back);

    tmp_win->move_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth), -(BorderWidth),
        tmp_win->attr.width+EdgeWidth*2 ,
        tmp_win->attr.height + tmp_win->title_height + EdgeWidth*2,
        0,
        White, White);

    tmp_win->resize_rt_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth+ResizeEdgeWidth), -(BorderWidth),
        ResizeEdgeWidth, ResizeEdgeWidth,
        1,
        Black, White );
    tmp_win->resize_rb_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth+ResizeEdgeWidth), -(BorderWidth+ResizeEdgeWidth),
        ResizeEdgeWidth, ResizeEdgeWidth,
        1,
        Black, White );
    tmp_win->resize_lt_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth), -(BorderWidth),
        ResizeEdgeWidth, ResizeEdgeWidth,
        1,
        Black, White );
    tmp_win->resize_lb_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth), -(BorderWidth+ResizeEdgeWidth),
        ResizeEdgeWidth, ResizeEdgeWidth,
        1,
        Black, White );

    tmp_win->title_w = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth), -(BorderWidth),
        tmp_win->attr.width+EdgeWidth*2, TITLE_BAR_HEIGHT,
        BorderWidth,
        TitleC.back, TitleC.back);

    if (Highlight && tmp_win->highlight)
    {
        XSetWindowBorderPixmap(dpy, tmp_win->frame, GrayTile);
        XSetWindowBorderPixmap(dpy, tmp_win->title_w, GrayTile);
    }

    gcm = 0;
    gcm |= GCFont;          gcv.font = TitleBarFont.font->fid;
    gcm |= GCPlaneMask;     gcv.plane_mask = AllPlanes;
    gcm |= GCForeground;    gcv.foreground = TitleC.fore;
    gcm |= GCBackground;    gcv.background = TitleC.back;

    tmp_win->title_gc = XCreateGC(dpy, Root, gcm, &gcv);

    CreateTitleButtons(tmp_win);


    XDefineCursor(dpy, tmp_win->frame, ArrowCursor);
    XDefineCursor(dpy, tmp_win->title_w, ArrowCursor);
    XDefineCursor(dpy, tmp_win->move_edge, MoveCursor);
    XDefineCursor(dpy, tmp_win->resize_lt_edge, UpperLeftCursor);
    XDefineCursor(dpy, tmp_win->resize_lb_edge, LowerLeftCursor);
    XDefineCursor(dpy, tmp_win->resize_rt_edge, UpperRightCursor);
    XDefineCursor(dpy, tmp_win->resize_rb_edge, LowerRightCursor);
    XDefineCursor(dpy, tmp_win->iconify_w, ButtonCursor);

    XSelectInput(dpy, tmp_win->w, StructureNotifyMask | PropertyChangeMask |
        ColormapChangeMask);
    XSelectInput(dpy, tmp_win->frame,
        SubstructureRedirectMask | VisibilityChangeMask |
        ButtonPressMask | ButtonReleaseMask |
        EnterWindowMask | LeaveWindowMask);

    XSelectInput(dpy, tmp_win->move_edge,
        SubstructureRedirectMask | VisibilityChangeMask |
        ButtonPressMask | ButtonReleaseMask
        );

    XSelectInput(dpy, tmp_win->resize_rt_edge,
        SubstructureRedirectMask | VisibilityChangeMask |
        ButtonPressMask | ButtonReleaseMask
        );
    XSelectInput(dpy, tmp_win->resize_rb_edge,
        SubstructureRedirectMask | VisibilityChangeMask |
        ButtonPressMask | ButtonReleaseMask
        );
    XSelectInput(dpy, tmp_win->resize_lt_edge,
        SubstructureRedirectMask | VisibilityChangeMask |
        ButtonPressMask | ButtonReleaseMask
        );
    XSelectInput(dpy, tmp_win->resize_lb_edge,
        SubstructureRedirectMask | VisibilityChangeMask |
        ButtonPressMask | ButtonReleaseMask
        );

    XSelectInput(dpy, tmp_win->title_w,
        KeyPressMask |
        ButtonPressMask | ButtonReleaseMask | ExposureMask);

    XSetWindowBorderWidth(dpy, tmp_win->w, BorderWidth);
    if (Highlight && tmp_win->highlight)
        XSetWindowBorderPixmap(dpy, tmp_win->w, GrayTile);
    XAddToSaveSet(dpy, tmp_win->w);
    XReparentWindow(dpy, tmp_win->w, tmp_win->frame, 0, tmp_win->title_height);

    SetupWindow(tmp_win,
        tmp_win->frame_x,
        tmp_win->frame_y,
        tmp_win->attr.width+EdgeWidth*2+BorderWidth*2,
        tmp_win->attr.height + tmp_win->title_height+EdgeWidth*2+BorderWidth*2);

    pm = NULL;
    tmp_win->forced = FALSE;

    /* now go through the steps to get an icon window,  if ForceIcon is
     * set, then no matter what else is defined, the bitmap from the
     * .swmrc file is used
     */

    labelwidth = IconWidth-IconShadowWidth-IconBorderWidth;
    labelheight = IconHeight-IconShadowWidth-IconBorderWidth;
    iconBMHeight = labelheight - (IconHeight-IconLabelY) - IconFont.height -1;

    if (ForceIcon)
    {
        Pixmap bm;
        XImage *image;
        unsigned mask;

        if ((bm = (Pixmap)LookInNameList(ICONS, tmp_win->full_name)) == NULL)
            bm = (Pixmap)LookInList(ICONS, tmp_win->full_name, &tmp_win->class);
        if (bm != NULL)
        {
            XGetGeometry(dpy, bm, &JunkRoot, &JunkX, &JunkY,
                &tmp_win->icon_width, &tmp_win->icon_height,
                &JunkBW, &JunkDepth);

            if (tmp_win->icon_width > (labelwidth-IconBorderWidth))
                tmp_win->icon_width = labelwidth-IconBorderWidth;
            if (tmp_win->icon_height > iconBMHeight)
                tmp_win->icon_height = iconBMHeight;

            pm = XCreatePixmap(dpy, Root, tmp_win->icon_width,
                tmp_win->icon_height, d_depth);

            /* the copy plane works on color ! */
            XCopyPlane(dpy, bm, pm, IconNormalGC,
                0,0, tmp_win->icon_width, tmp_win->icon_height, 0, 0, 1 );

            tmp_win->forced = TRUE;
        }
    }

    /* if the pixmap is still NULL, we didn't get one from the above code,
     * that could mean that ForceIcon was not set, or that the window
     * was not in the Icons list, now check the WM hints for an icon
     */
    if (pm == NULL && tmp_win->wmhints &&
        tmp_win->wmhints->flags & IconPixmapHint)
    {

        XGetGeometry(dpy,   tmp_win->wmhints->icon_pixmap,
             &JunkRoot, &JunkX, &JunkY,
             &tmp_win->icon_width, &tmp_win->icon_height, &JunkBW, &JunkDepth);

        if (tmp_win->icon_width > (labelwidth-IconBorderWidth))
            tmp_win->icon_width = labelwidth-IconBorderWidth;
        if (tmp_win->icon_height > iconBMHeight)
            tmp_win->icon_height = iconBMHeight;

        pm = XCreatePixmap(dpy, Root, tmp_win->icon_width, tmp_win->icon_height,
            d_depth);

        XCopyPlane(dpy, tmp_win->wmhints->icon_pixmap, pm, IconNormalGC,
            0,0, tmp_win->icon_width, tmp_win->icon_height, 0, 0, 1 );
    }

    /* if we still haven't got an icon, let's look in the Icon list
     * if ForceIcon is not set
     */
    if (pm == NULL && !ForceIcon)
    {
        Pixmap bm;
        XImage *image;
        unsigned mask;

        if ((bm = (Pixmap)LookInNameList(ICONS, tmp_win->full_name)) == NULL)
            bm = (Pixmap)LookInList(ICONS, tmp_win->full_name, &tmp_win->class);
        if (bm != NULL)
        {
            XGetGeometry(dpy, bm, &JunkRoot, &JunkX, &JunkY,
                &tmp_win->icon_width, &tmp_win->icon_height,
                &JunkBW, &JunkDepth);

            if (tmp_win->icon_width > (labelwidth-IconBorderWidth))
                tmp_win->icon_width = labelwidth-IconBorderWidth;
            if (tmp_win->icon_height > iconBMHeight)
                tmp_win->icon_height = iconBMHeight;

            pm = XCreatePixmap(dpy, Root, tmp_win->icon_width,
                tmp_win->icon_height, d_depth);

            /* the copy plane works on color ! */
            XCopyPlane(dpy, bm, pm, IconNormalGC,
                0,0, tmp_win->icon_width, tmp_win->icon_height, 0, 0, 1 );

            /*
            this code works on a monochrome system

            XPutImage(dpy, pm, IconNormalGC,
                image, 0, 0, 0, 0, tmp_win->icon_width, tmp_win->icon_height);
            */
        }
    }

    /* if we still don't have an icon, assign the UnknownIcon */

    if (pm == NULL && UnknownPm != NULL)
    {
        tmp_win->icon_width = UnknownWidth;
        tmp_win->icon_height = UnknownHeight;

        if (tmp_win->icon_width > (labelwidth-IconBorderWidth))
            tmp_win->icon_width = labelwidth-IconBorderWidth;
        if (tmp_win->icon_height > iconBMHeight)
            tmp_win->icon_height = iconBMHeight;

        pm = XCreatePixmap(dpy, Root, tmp_win->icon_width,
            tmp_win->icon_height, d_depth);

        /* the copy plane works on color ! */
        XCopyPlane(dpy, UnknownPm, pm, IconNormalGC,
            0,0, tmp_win->icon_width, tmp_win->icon_height, 0, 0, 1 );
    }

    if (pm == NULL)
    {
        tmp_win->icon_height = 0;
        tmp_win->icon_width = 0;
        valuemask = 0;
    }
    else
    {
        valuemask = CWBackPixmap;
        attributes.background_pixmap = pm;
    }

    tmp_win->icon_w_width = IconWidth;
    namewidth = XTextWidth(IconFont.font,
        tmp_win->icon_name, strlen(tmp_win->icon_name));
    if (namewidth > IconWidth)
       tmp_win->icon_x = IconBMX;
    else
       tmp_win->icon_x = (IconWidth - namewidth) /2;
    tmp_win->icon_y = IconLabelY ;

    if (tmp_win->wmhints && tmp_win->wmhints->flags & IconWindowHint)
    {
        tmp_win->icon_w = tmp_win->wmhints->icon_window;
    }
    else
    {
        tmp_win->icon_w = XCreateSimpleWindow(dpy,parentWindow,0,0,
                                              IconWidth,IconHeight,0,
                                              IconBorderColor,IconBorderColor);
        XLowerWindow(dpy, tmp_win->icon_w);

        tmp_win->icon_label = XCreateSimpleWindow(dpy, tmp_win->icon_w,
            IconBorderWidth,IconBorderWidth,
            labelwidth, labelheight,
            0, IconC.back, IconC.back);

        tmp_win->dummyicon_w =  XCreateSimpleWindow(dpy,parentWindow,0,0,
                                IconWidth,IconHeight,0,IconBorderColor,IconBorderColor);
        XLowerWindow(dpy, tmp_win->dummyicon_w);
        tmp_win->dummyicon_label = XCreateSimpleWindow(dpy, tmp_win->dummyicon_w,
            IconShadowWidth, IconShadowWidth,
            labelwidth, labelheight,
            0, IconC.back, IconC.back);
    }

    XSelectInput(dpy, tmp_win->icon_w,
        KeyPressMask | VisibilityChangeMask |
        EnterWindowMask | LeaveWindowMask   |
        ButtonPressMask | ButtonReleaseMask | ExposureMask);
    XSelectInput(dpy, tmp_win->dummyicon_w,
        ButtonPressMask | ButtonReleaseMask | ExposureMask);

    tmp_win->icon_bm_w = NULL;
    if (pm != NULL)
    {
        if (tmp_win->icon_w_width <= (tmp_win->icon_width-IconShadowWidth))
            x = 1;
        else
            x = (tmp_win->icon_w_width - tmp_win->icon_width)/2;

        tmp_win->icon_bm_w = XCreateWindow(dpy, tmp_win->icon_w,
            x, IconBMY,
            tmp_win->icon_width, tmp_win->icon_height,
            0, d_depth, CopyFromParent,
            d_visual, valuemask, &attributes);
    }

    XDefineCursor(dpy, tmp_win->icon_w, ArrowCursor);

    GrabButtons(tmp_win);
    GrabKeys(tmp_win);

    XSaveContext(dpy, tmp_win->w, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->frame, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->move_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->resize_rt_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->resize_rb_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->resize_lt_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->resize_lb_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->title_w, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->iconify_w, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->icon_w, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->dummyicon_w, SwmContext, tmp_win);

    SetHints(tmp_win);
    XUngrabServer(dpy);

    DrawWindowName(tmp_win);

    XMapWindow(dpy,tmp_win -> w);
    XMapSubwindows(dpy,tmp_win -> w);
    XMapRaised(dpy,tmp_win -> frame);

    return (tmp_win);
}

/***********************************************************************
 *
 *  Procedure:
 *	AddWindow - add a RootDesktop Window to the swm list
 *
 *  Returned Value:
 *	(SwmWindow *) - pointer to the SwmWindow structure
 *
 *  Inputs:
 *	w	- the window id of the window to add
 *
 ***********************************************************************
 */

SwmWindow *
AddWindow(w)
Window w;
{
    SwmWindow *tmp_win;			/* new swm window structure */
    int stat;
    char *prop;
    unsigned long valuemask;		/* mask for create windows */
    XSetWindowAttributes attributes;	/* attributes for create windows */
    int width, len;			/* tmp variable */
    int junk1, junk2, junk3;
    int x;
    Pixmap pm;			/* tmp pixmap variable */
    XWindowChanges xwc;		/* change window structure */
    unsigned int xwcm;		/* change window mask */
    int dont_know;		/* don't know where to put the window */
    XColor blob, cret;
    XEvent event;
    XGCValues	    gcv;
    unsigned long   gcm, mask;

    int iconBMHeight;
    int labelwidth, labelheight;
    int namewidth;
    int ii, jj;

#ifdef DEBUG
    fprintf(stderr, "AddWindow: w = 0x%x\n", w);
#endif

    /* allocate space for the swm window */
    tmp_win = (SwmWindow *)malloc(sizeof(SwmWindow));
    tmp_win->w = w;
    tmp_win->zoomed = ZOOM_NONE;

    tmp_win->windowFlag = 0; 

    for (ii = 0 ; ii < 20 ; ii++)
       for (jj  = 0; jj < 20 ; jj++)
          tmp_win->iconTable[ii][jj] = 0;

    XSelectInput(dpy, tmp_win->w, PropertyChangeMask);
    XGetWindowAttributes(dpy, tmp_win->w, &tmp_win->attr);
    XFetchName(dpy, tmp_win->w, &tmp_win->window_name);
    tmp_win->class = NoClass;
    XGetClassHint(dpy, tmp_win->w, &tmp_win->class);

#ifdef DEBUG
    fprintf(stderr, "  name = \"%s\"\n", tmp_win->window_name);
#endif
    tmp_win->wmhints = XGetWMHints(dpy, tmp_win->w);
    if (tmp_win->wmhints && (tmp_win->wmhints->flags & WindowGroupHint))
	tmp_win->group = tmp_win->wmhints->window_group;
    else
	tmp_win->group = NULL;

    if (XGetNormalHints(dpy, tmp_win->w, &tmp_win->hints) == 0)
	tmp_win->hints.flags = 0;

    if (strncmp("xterm", tmp_win->class.res_name, 5) == 0 ||
    	strncmp("XTerm", tmp_win->class.res_class, 5) == 0 )
	tmp_win->xterm = TRUE;
    else
	tmp_win->xterm = FALSE;

    dont_know = TRUE;
    if (tmp_win->hints.flags & PPosition)
    {
#ifdef DEBUG
	fprintf(stderr, "  program specified hints\n");
#endif
	tmp_win->attr.x = tmp_win->hints.x;
	tmp_win->attr.y = tmp_win->hints.y;
	tmp_win->attr.width = tmp_win->hints.width ;
	tmp_win->attr.height = tmp_win->hints.height ;
    }
    if (tmp_win->hints.flags & USPosition)
    {
#ifdef DEBUG
	fprintf(stderr, "  user specified hints\n");
#endif
	dont_know = FALSE;
	tmp_win->attr.x = tmp_win->hints.x;
	tmp_win->attr.y = tmp_win->hints.y;
	tmp_win->attr.width = tmp_win->hints.width-(BorderWidth+EdgeWidth)*2;
	tmp_win->attr.height = tmp_win->hints.height-(BorderWidth+EdgeWidth)*2;
    }

    if (tmp_win->window_name == NULL)
	tmp_win->window_name = NoName;
    if (tmp_win->class.res_name == NULL)
    	tmp_win->class.res_name = NoName;
    if (tmp_win->class.res_class == NULL)
    	tmp_win->class.res_class = NoName;

    tmp_win->full_name = tmp_win->window_name;

    tmp_win->highlight = !(short)LookInList(NO_HILITE, tmp_win->full_name, 
	&tmp_win->class);
    tmp_win->auto_raise = (short)LookInList(AUTO_RAISE, tmp_win->full_name, 
	&tmp_win->class);
    tmp_win->iconify_by_unmapping = IconifyByUnmapping;
    if (IconifyByUnmapping)
    {
	tmp_win->iconify_by_unmapping = 
	    !(short)LookInList(DONT_ICONIFY_BY_UNMAPPING, tmp_win->full_name,
		&tmp_win->class);
    }
    if (NoTitlebar || LookInList(NO_TITLE, tmp_win->full_name, &tmp_win->class))
	tmp_win->title_height = 0;
    else
	tmp_win->title_height = TITLE_BAR_HEIGHT + BorderWidth ;

    if (HandlingEvents && dont_know && !RandomPlacement)
    {
	if (!(tmp_win->wmhints && tmp_win->wmhints->flags & StateHint &&
	    tmp_win->wmhints->initial_state == IconicState) &&
	   (tmp_win->xterm || (tmp_win->attr.x == 0 && tmp_win->attr.y == 0)))
	{
	    /* better wait until all the mouse buttons have been 
	     * released.
	     */
	    while (TRUE)
	    {
		XUngrabServer(dpy);
		XSync(dpy);
		XGrabServer(dpy);
		XQueryPointer(dpy, Root, &JunkRoot, &JunkChild,
		    &JunkX, &JunkY, &AddingX, &AddingY, &JunkMask);

		if (JunkMask != 0)
		    continue;

		stat = XGrabPointer(dpy, Root, False,
		    ButtonPressMask | ButtonReleaseMask,
		    GrabModeAsync, GrabModeAsync,
		    Root, UpperLeftCursor, CurrentTime);

		if (stat == GrabSuccess)
		    break;
	    }

	    width = XTextWidth(InitialFont.font, tmp_win->window_name,
		strlen(tmp_win->window_name)) + 20;
	    XResizeWindow(dpy, InitialWindow, width, InitialFont.height + 4);
	    XMapRaised(dpy, InitialWindow);
	    XDrawImageString(dpy, InitialWindow, InitialNormalGC,
		10, 2 + InitialFont.font->ascent,
		tmp_win->window_name, strlen(tmp_win->window_name));

	    AddingW = tmp_win->attr.width;
	    AddingH = tmp_win->attr.height;

	    AddingW = tmp_win->attr.width + 2 * BorderWidth;
	    AddingH = tmp_win->attr.height + tmp_win->title_height +
		2 * BorderWidth;

	    while (TRUE)
	    {
		XQueryPointer(dpy, Root, &JunkRoot, &JunkChild,
		    &JunkX, &JunkY, &AddingX, &AddingY, &JunkMask);

		MoveOutline(Root, AddingX, AddingY, AddingW, AddingH);

		if (XCheckTypedEvent(dpy, ButtonPress, &event))
		{
		    AddingX = event.xbutton.x_root;
		    AddingY = event.xbutton.y_root;
		    break;
		}
	    }

	    if (event.xbutton.button == Button2)
	    {
		XWarpPointer(dpy, None, Root, 0, 0, 0, 0,
		    AddingX + AddingW/2, AddingY + AddingH/2);
		AddStartResize(tmp_win, AddingX, AddingY, AddingW, AddingH);

		while (TRUE)
		{
		    int lastx, lasty;

		    XQueryPointer(dpy, Root, &JunkRoot, &JunkChild,
			&JunkX, &JunkY, &AddingX, &AddingY, &JunkMask);

		    if (lastx != AddingX || lasty != AddingY)
		    {
			DoResize(AddingX, AddingY, tmp_win);

			lastx = AddingX;
			lasty = AddingY;
		    }

		    if (XCheckTypedEvent(dpy, ButtonRelease, &event))
		    {
			AddEndResize(tmp_win);
			break;
		    }
		}
	    }

	    MoveOutline(Root, 0, 0, 0, 0);
	    XUnmapWindow(dpy, InitialWindow);
	    XUngrabPointer(dpy, CurrentTime);

	    tmp_win->attr.x = AddingX;
	    tmp_win->attr.y = AddingY + tmp_win->title_height;
	    tmp_win->attr.width = AddingW - 2 * BorderWidth;
	    tmp_win->attr.height = AddingH - tmp_win->title_height -
		(2 * BorderWidth);

	    XUngrabServer(dpy);
	}
    }
    else if (HandlingEvents && dont_know && RandomPlacement)
    {
	if ((PlaceX + tmp_win->attr.width) > MyDisplayWidth)
	    PlaceX = 50;
	if ((PlaceY + tmp_win->attr.height) > MyDisplayHeight)
	    PlaceY = 50;

	tmp_win->attr.x = PlaceX;
	tmp_win->attr.y = PlaceY;
	PlaceX += 30;
	PlaceY += 30;
    }

    if (tmp_win->attr.y < tmp_win->title_height)
	tmp_win->attr.y = tmp_win->title_height;

    xwcm = CWX | CWY | CWWidth | CWHeight | CWBorderWidth;

#ifdef DEBUG
	fprintf(stderr, "  position window  %d, %d  %dx%d\n", 
	    tmp_win->attr.x,
	    tmp_win->attr.y,
	    tmp_win->attr.width,
	    tmp_win->attr.height);
#endif
    xwc.x = tmp_win->attr.x + tmp_win->attr.border_width;
    xwc.y = tmp_win->attr.y + tmp_win->attr.border_width;
    xwc.width = tmp_win->attr.width;
    xwc.height = tmp_win->attr.height;
    xwc.border_width = 0;

    XConfigureWindow(dpy, tmp_win->w, xwcm, &xwc);

    tmp_win->name_width = XTextWidth(TitleBarFont.font, tmp_win->window_name,
	strlen(tmp_win->window_name));

    if (XGetWindowProperty(dpy, tmp_win->w, XA_WM_ICON_NAME, 0, 200, False,
	XA_STRING, &junk1, &junk2, &junk3, &len, &tmp_win->icon_name))
	tmp_win->icon_name = tmp_win->window_name;

    if (tmp_win->icon_name == NULL)
	tmp_win->icon_name = tmp_win->window_name;

    tmp_win->iconified = FALSE;
    tmp_win->icon = FALSE;
    tmp_win->icon_on = FALSE;

    /* add the window into the swm list */
    tmp_win->next = SwmRoot.next;
    if (SwmRoot.next != NULL)
	SwmRoot.next->prev = tmp_win;
    tmp_win->prev = &SwmRoot;
    SwmRoot.next = tmp_win;

    /* create windows */

    tmp_win->frame_x = tmp_win->attr.x;
    tmp_win->frame_y = tmp_win->attr.y - tmp_win->title_height;

    XGrabServer(dpy);

    tmp_win->frame = XCreateSimpleWindow(dpy, Root,
	tmp_win->frame_x,
	tmp_win->frame_y,
	tmp_win->attr.width+EdgeWidth*2,
	tmp_win->attr.height + tmp_win->title_height+EdgeWidth*2,
	BorderWidth,
	BorderColor, TitleC.back);

    tmp_win->move_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth), -(BorderWidth), 
        tmp_win->attr.width+EdgeWidth*2 ,
        tmp_win->attr.height + tmp_win->title_height + EdgeWidth*2,
        0,
        White, White);
    
    tmp_win->resize_rt_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth+ResizeEdgeWidth), -(BorderWidth),
        ResizeEdgeWidth, ResizeEdgeWidth,
        1,
        Black, White );
    tmp_win->resize_rb_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth+ResizeEdgeWidth), -(BorderWidth+ResizeEdgeWidth),
        ResizeEdgeWidth, ResizeEdgeWidth,
        1,
        Black, White );
    tmp_win->resize_lt_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth), -(BorderWidth),
        ResizeEdgeWidth, ResizeEdgeWidth,
        1,
        Black, White );
    tmp_win->resize_lb_edge = XCreateSimpleWindow(dpy, tmp_win->frame,
        -(BorderWidth), -(BorderWidth+ResizeEdgeWidth),
        ResizeEdgeWidth, ResizeEdgeWidth,
        1,
        Black, White );

    tmp_win->title_w = XCreateSimpleWindow(dpy, tmp_win->frame,
	-(BorderWidth), -(BorderWidth),
	tmp_win->attr.width+EdgeWidth*2, TITLE_BAR_HEIGHT,
	BorderWidth,
	TitleC.back, TitleC.back);
 
    if (Highlight && tmp_win->highlight)
    {
	XSetWindowBorderPixmap(dpy, tmp_win->frame, GrayTile);
	XSetWindowBorderPixmap(dpy, tmp_win->title_w, GrayTile);
    }

    gcm = 0;
    gcm |= GCFont;	    gcv.font = TitleBarFont.font->fid;
    gcm |= GCPlaneMask;	    gcv.plane_mask = AllPlanes;
    gcm |= GCForeground;    gcv.foreground = TitleC.fore;
    gcm |= GCBackground;    gcv.background = TitleC.back;

    tmp_win->title_gc = XCreateGC(dpy, Root, gcm, &gcv);

    /* CreateTitleButtons(tmp_win); */

	
    XDefineCursor(dpy, tmp_win->frame, ArrowCursor);
    XDefineCursor(dpy, tmp_win->title_w, ArrowCursor);
    XDefineCursor(dpy, tmp_win->move_edge, MoveCursor);
    XDefineCursor(dpy, tmp_win->resize_lt_edge, UpperLeftCursor);
    XDefineCursor(dpy, tmp_win->resize_lb_edge, LowerLeftCursor);
    XDefineCursor(dpy, tmp_win->resize_rt_edge, UpperRightCursor);
    XDefineCursor(dpy, tmp_win->resize_rb_edge, LowerRightCursor);
    XDefineCursor(dpy, tmp_win->iconify_w, ButtonCursor);

    XSelectInput(dpy, tmp_win->w, StructureNotifyMask | PropertyChangeMask |
	ColormapChangeMask);
     XSelectInput(dpy, tmp_win->frame,
        SubstructureRedirectMask | VisibilityChangeMask |
        ButtonPressMask | ButtonReleaseMask |
        EnterWindowMask | LeaveWindowMask);

    XSelectInput(dpy, tmp_win->move_edge,
        SubstructureRedirectMask | VisibilityChangeMask 
        );

    XSelectInput(dpy, tmp_win->resize_rt_edge,
        SubstructureRedirectMask | VisibilityChangeMask 
        );
    XSelectInput(dpy, tmp_win->resize_rb_edge,
        SubstructureRedirectMask | VisibilityChangeMask 
        );
    XSelectInput(dpy, tmp_win->resize_lt_edge,
        SubstructureRedirectMask | VisibilityChangeMask 
        );
    XSelectInput(dpy, tmp_win->resize_lb_edge,
        SubstructureRedirectMask | VisibilityChangeMask 
        );

    XSelectInput(dpy, tmp_win->title_w,
        KeyPressMask |
        ButtonPressMask | ButtonReleaseMask | ExposureMask);

    XSetWindowBorderWidth(dpy, tmp_win->w, BorderWidth);
    if (Highlight && tmp_win->highlight)
        XSetWindowBorderPixmap(dpy, tmp_win->w, GrayTile);
    XAddToSaveSet(dpy, tmp_win->w);
    XReparentWindow(dpy, tmp_win->w, tmp_win->frame, 0, tmp_win->title_height);

     SetupRootDesktop(tmp_win,
        -BorderWidth,-BorderWidth,
        MyDisplayWidth, MyDisplayHeight);

    pm = NULL;
    tmp_win->forced = FALSE;

    GrabButtons(tmp_win);
    GrabKeys(tmp_win);

    XSaveContext(dpy, tmp_win->w, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->frame, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->move_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->resize_rt_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->resize_rb_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->resize_lt_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->resize_lb_edge, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->title_w, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->iconify_w, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->icon_w, SwmContext, tmp_win);
    XSaveContext(dpy, tmp_win->dummyicon_w, SwmContext, tmp_win);

    SetHints(tmp_win);
    XUngrabServer(dpy);

    DrawWindowName(tmp_win);

    return (tmp_win);
}

/***********************************************************************
 *
 *  Procedure:
 *	MappedNotOverride - checks to see if we should really
 *		put a swm frame on the window
 *
 *  Returned Value:
 *	TRUE	- go ahead and frame the window
 *	FALSE	- don't frame the window
 *
 *  Inputs:
 *	w	- the window to check
 *
 ***********************************************************************
 */

int
MappedNotOverride(w)
    Window w;
{
    XWindowAttributes wa;

    XGetWindowAttributes(dpy, w, &wa);
    return ((wa.map_state != IsUnmapped) && (wa.override_redirect != True));
}

/***********************************************************************
 *
 *  Procedure:
 *	GrabAllButtons - grab needed buttons for all windows
 *
 ***********************************************************************
 */

void
GrabAllButtons()
{
    SwmWindow *tmp_win;

    for (tmp_win = SwmRoot.next; tmp_win != NULL; tmp_win = tmp_win->next)
    {
	GrabButtons(tmp_win);
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	GrabAllKeys - grab needed keys for all windows
 *
 ***********************************************************************
 */

void
GrabAllKeys()
{
    SwmWindow *tmp_win;

    for (tmp_win = SwmRoot.next; tmp_win != NULL; tmp_win = tmp_win->next)
    {
	GrabKeys(tmp_win);
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	UngrabAllButtons - ungrab buttons for all windows
 *
 ***********************************************************************
 */

void
UngrabAllButtons()
{
    SwmWindow *tmp_win;

    for (tmp_win = SwmRoot.next; tmp_win != NULL; tmp_win = tmp_win->next)
    {
	UngrabButtons(tmp_win);
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	UngrabAllKeys - ungrab keys for all windows
 *
 ***********************************************************************
 */

void
UngrabAllKeys()
{
    SwmWindow *tmp_win;

    for (tmp_win = SwmRoot.next; tmp_win != NULL; tmp_win = tmp_win->next)
    {
	UngrabKeys(tmp_win);
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	GrabButtons - grab needed buttons for the window
 *
 *  Inputs:
 *	tmp_win - the swm window structure to use
 *
 ***********************************************************************
 */

void
GrabButtons(tmp_win)
SwmWindow *tmp_win;
{
    int i, j;

    for (i = 0; i < MAX_BUTTONS+1; i++)
    {
	for (j = 0; j < MOD_SIZE; j++)
	{
	    if (Mouse[i][C_WINDOW][j].func != NULL)
	    {
		XGrabButton(dpy, i, j, tmp_win->w,
		    True, ButtonPressMask | ButtonReleaseMask,
		    GrabModeAsync, GrabModeAsync, None, ArrowCursor);
	    }
	}
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	GrabKeys - grab needed keys for the window
 *
 *  Inputs:
 *	tmp_win - the swm window structure to use
 *
 ***********************************************************************
 */

void
GrabKeys(tmp_win)
SwmWindow *tmp_win;
{
    FuncKey *tmp;

    for (tmp = FuncKeyRoot.next; tmp != NULL; tmp = tmp->next)
    {
	switch (tmp->cont)
	{
	case C_WINDOW:
	    XGrabKey(dpy, tmp->keycode, tmp->mods, tmp_win->w, True,
		GrabModeAsync, GrabModeAsync);
	    break;

	case C_ICON:
	    XGrabKey(dpy, tmp->keycode, tmp->mods, tmp_win->icon_w, True,
		GrabModeAsync, GrabModeAsync);

	case C_TITLE:
	    XGrabKey(dpy, tmp->keycode, tmp->mods, tmp_win->title_w, True,
		GrabModeAsync, GrabModeAsync);
	    break;

	case C_NAME:
	    XGrabKey(dpy, tmp->keycode, tmp->mods, tmp_win->w, True,
		GrabModeAsync, GrabModeAsync);
	    XGrabKey(dpy, tmp->keycode, tmp->mods, tmp_win->icon_w, True,
		GrabModeAsync, GrabModeAsync);
	    XGrabKey(dpy, tmp->keycode, tmp->mods, tmp_win->title_w, True,
		GrabModeAsync, GrabModeAsync);
	    break;
	/*
	case C_ROOT:
	    XGrabKey(dpy, tmp->keycode, tmp->mods, Root, True,
		GrabModeAsync, GrabModeAsync);
	    break;
	*/
	}
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	UngrabButtons - ungrab buttons for windows
 *
 *  Inputs:
 *	tmp_win - the swm window structure to use
 *
 ***********************************************************************
 */

void
UngrabButtons(tmp_win)
SwmWindow *tmp_win;
{
    int i;

    for (i = 0; i < MAX_BUTTONS+1; i++)
    {
	XUngrabButton(dpy, i, AnyModifier, tmp_win->w);
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	UngrabKeys - ungrab keys for windows
 *
 *  Inputs:
 *	tmp_win - the swm window structure to use
 *
 ***********************************************************************
 */

void
UngrabKeys(tmp_win)
SwmWindow *tmp_win;
{
    FuncKey *tmp;

    for (tmp = FuncKeyRoot.next; tmp != NULL; tmp = tmp->next)
    {
	switch (tmp->cont)
	{
	case C_WINDOW:
	    XUngrabKey(dpy, tmp->keycode, tmp->mods, tmp_win->w);
	    break;

	case C_ICON:
	    XUngrabKey(dpy, tmp->keycode, tmp->mods, tmp_win->icon_w);

	case C_TITLE:
	    XUngrabKey(dpy, tmp->keycode, tmp->mods, tmp_win->title_w);
	    break;

	case C_ROOT:
	    XUngrabKey(dpy, tmp->keycode, tmp->mods, Root);
	    break;
	}
    }
}

CreateTitleButtons(tmp_win)
SwmWindow *tmp_win;
{
    unsigned long valuemask;		/* mask for create windows */
    XSetWindowAttributes attributes;	/* attributes for create windows */

    /* the button have the pixmap as the background of the window,
     * that way I don't have to worry about repainting them
     * on expose events.
     */

    valuemask = CWEventMask | CWBackPixmap;
    attributes.event_mask = ButtonPressMask | ButtonReleaseMask;
    tmp_win->iconify_pm = MakePixmap(tmp_win->title_w,
	tmp_win->title_gc, iconify_bits, iconify_width, iconify_height);
    attributes.background_pixmap = tmp_win->iconify_pm;

    tmp_win->iconify_w = XCreateWindow(dpy, tmp_win->title_w,
	TITLE_BAR_SPACE, TITLE_BAR_SPACE,
	iconify_width, iconify_height,
	0, d_depth, CopyFromParent,
	d_visual, valuemask, &attributes);


    XMapSubwindows(dpy, tmp_win->move_edge);
    XMapSubwindows(dpy, tmp_win->title_w);
}
