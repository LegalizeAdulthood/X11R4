/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/apps/RCS/statload.c,v 2.7 89/06/22 13:25:10 ghoti Exp $ */
/* $ACIS:statload.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/apps/RCS/statload.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/apps/RCS/statload.c,v 2.7 89/06/22 13:25:10 ghoti Exp $";
#endif /* lint */


#include <class.h>
#include <andyenv.h>
#define STATICLOADTYPESCRIPT 1
#define class_StaticEntriesOnly
#ifdef WM_ENV
#include <wfontd.ih>
#include <wgraphic.ih>
#include <wim.ih>
#include <wcursor.ih>
#include <wws.ih>
#include <mrl.ih>
#endif /* WM_ENV */
#ifdef X11_ENV
#include <X11/X.h>
#include <X11/Xlib.h>
#include <cmenu.h>
#include <xfontd.ih>
#include <xgraphic.ih>
#include <xim.ih>
#include <xcursor.ih>
#include <xws.ih>
#endif
#include <updlist.ih>
#include <keyrec.ih>
#include <proctbl.ih>
#include <keystate.ih>
#include <keymap.ih>
#include <menulist.ih>
#include <fontdesc.ih>
#include <im.ih>
#include <graphic.ih>
#include <buffer.ih>
#include <mark.ih>
#include <nstdmark.ih>
#include <rectlist.ih>
#include <text.ih>
#include <envrment.ih>
#include <tree23.ih>
#include <stylesht.ih>
#include <style.ih>
#include <textv.ih>
#include <framev.ih>
#include <msghndlr.ih>
#include <message.ih>
#include <framemsg.ih>
#include <cursor.ih>
#include <filetype.ih>
#include <frame.ih>
#include <framecmd.ih>
#include <scroll.ih>
#include <event.ih>
#include <search.ih>
#include <init.ih>
#include <environ.ih>
#include <dict.ih>
#include <viewref.ih>
#include <bind.ih>
#include <complete.ih>
#include <atom.ih>
#include <atomlist.ih>
#include <namespc.ih>
#include <rm.ih>
#include <app.ih>
#include <eza.ih>
#include <helptxtv.ih>
#include <matte.ih>

#ifdef STATICLOADTYPESCRIPT
#include <tscripta.ih>
#include <tscript.ih>
#include <typetext.ih>
#endif /* STATICLOADTYPESCRIPT */

#undef class_StaticEntriesOnly

doStaticLoads()
{
#ifdef WM_ENV
    wmfontdesc_StaticEntry;
    wmgraphic_StaticEntry;
    wmim_StaticEntry;
    wmcursor_StaticEntry;
    wmws_StaticEntry;
    mrl_StaticEntry;
#endif /* WM_ENV */
#ifdef X11_ENV
    xfontdesc_StaticEntry;
    xgraphic_StaticEntry;
    xim_StaticEntry;
    xcursor_StaticEntry;
    xws_StaticEntry;
#endif /* X11_ENV */
    graphic_StaticEntry;
    updatelist_StaticEntry;
    keyrec_StaticEntry;
    proctable_StaticEntry;
    keystate_StaticEntry;
    keymap_StaticEntry;
    menulist_StaticEntry;
    fontdesc_StaticEntry;
    im_StaticEntry;
    buffer_StaticEntry;
    mark_StaticEntry;
    nestedmark_StaticEntry;
    rectlist_StaticEntry;
    text_StaticEntry;
    environment_StaticEntry;
    tree23int_StaticEntry;
    stylesheet_StaticEntry;
    style_StaticEntry;
    textview_StaticEntry;
    frameview_StaticEntry;
    msghandler_StaticEntry;
    message_StaticEntry;
    framemessage_StaticEntry;
    cursor_StaticEntry;
    filetype_StaticEntry;
    frame_StaticEntry;
    framecmds_StaticEntry;
    scroll_StaticEntry;
    event_StaticEntry;
    search_StaticEntry;
    init_StaticEntry;
    environ_StaticEntry;
    dictionary_StaticEntry;
    viewref_StaticEntry;
    bind_StaticEntry;
    completion_StaticEntry;
    atom_StaticEntry;
    atomlist_StaticEntry;
    namespace_StaticEntry;
    rm_StaticEntry;
    helptextview_StaticEntry;
    matte_StaticEntry;
#ifdef STATICLOADTYPESCRIPT
    typescriptapp_StaticEntry;
    typescript_StaticEntry;
    typetext_StaticEntry;
#endif /* STATICLOADTYPESCRIPT */

    application_StaticEntry;
    ezapp_StaticEntry;
}
