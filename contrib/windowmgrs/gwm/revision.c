/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************************************************************\
* 									   *
* 			   @@@@@@  @       @ @       @			   *
* 			  @      @ @       @ @@     @@			   *
* 			 @         @   @   @ @ @   @ @			   *
* 			 @    @@@@ @  @ @  @ @  @ @  @			   *
* 			 @       @ @ @   @ @ @   @   @			   *
* 			  @     @@ @@     @@ @       @			   *
* 			   @@@@@ @ @       @ @       @			   *
* 									   *
*  _    ,								   *
* ' )  /      _/_                   /)					   *
*  /--/ o _   /  __,_  , ,     ____//     __  _ , __o  _   o ______   _	   *
* /  (_(_/_)_(__(_) (_(_/_    (_) /__    / (_(<_\/ (__/_)_(_(_) /) )_/_)_  *
*                      /         />					   *
*                     '         </					   *
* 									   *
\**************************************************************************/

char	*GWM_Banner = "GWM (gwm@mirsa.inria.fr)";

char           *RCS_Header =
"$Header: /tmp_mnt/u/columbo/0/koala/colas/GWM/src/RCS/gwm.shar,v 1.5 89/11/28 11:35:02 colas Exp Locker: colas $";
char           *RCS_Revision = "$Revision: 1.5 $";

/* Here is the log of all revisions */
/* $Log:	gwm.shar,v $
 * Revision 1.5  89/11/28  11:35:02  colas
 * X11R4 official release
 * 
 * Revision 1.5  89/11/27 18:10:00 colas
 * X11R4 RELEASE	
 * - is now n-ary (- n1 (+ n2 n3 n4))
 *
 * Revision 1.4.1.31  89/11/23  17:34:34  colas
 * ok for grabpointer in R4
 * DEBUG enables stack overflow (10000) checking
 * no more TRACE flag, implied by DEBUG
 * new trigger-error function
 * icon-name context variable for place-menu
 * 
 * Revision 1.4.1.30  89/11/15  12:12:34  colas
 * Modification on the lex correction
 * cursor-names.gwm
 * mwm-profile: -f mwm
 * 
 * Revision 1.4.1.29  89/11/10  16:22:20  colas
 * changes updated
 * 
 * Revision 1.4.1.28  89/11/10  12:13:01  colas
 * Context was not initialized
 * do not recurse if transient-for itself (new idraw)
 * window-name "." converted to "_"
 * wl_namespace ==> wl_name
 * 
 * Revision 1.4.1.27  89/10/20  15:33:10  colas
 * no xor pour grid-color
 * meter background works
 * 
 * Revision 1.4.1.26  89/10/13  17:34:55  colas
 * simple-icon.legend
 * Imakefile & Makefile.noXtree
 * ok if transient_for or group is root
 * set-prop on root ok (root -> client = root!)
 * data/ instead of gwm/
 * WinGravity supported
 * get-wm-command
 * 
 * Revision 1.4.1.25  89/10/09  14:28:16  colas
 * Terry Weissman patch for calling iconify-window on map by client
 * sort was bugged 
 * 
 * Revision 1.4.1.24  89/10/06  17:02:28  colas
 * autocolormap
 * unbind was bugged
 * 
 * Revision 1.4.1.23  89/10/03  19:14:51  colas
 * current-event-window-coords bugged when window did not have resize inc hints
 * looped on windows having itself as group leader (weissman@wsl.dec.com)
 * shape extension supported
 * window-is-shaped
 * 
 * Revision 1.4.1.22  89/09/08  14:33:07  colas
 * input_mask field in wobs
 * better handling of grabs
 * wob-parent returns nil
 * 
 * Revision 1.4.1.21  89/09/04  18:26:44  colas
 * bugs in keys
 * current-event-time
 * 
 * Revision 1.4.1.20  89/09/01  18:15:17  colas
 * bugs
 * 
 * Revision 1.4.1.19  89/09/01  15:39:33  colas
 * keysym instead of keycodes in events
 * confine-grabs
 * ungrab-server no arg always, with arg only if == (and unpop also)
 * 
 * Revision 1.4.1.18  89/08/31  15:31:31  colas
 * with-modifier-N in code instead of with-modifierN
 * ## and # works on atoms and wobs
 * ## is able to make unique and to expand lists
 * delete-nth
 * 
 * Revision 1.4.1.17  89/08/29  18:39:42  colas
 * current-event-from-grab (for enter/leave & focus)
 * wob properties must be lists
 * # and ## on atoms and wobs
 * compat.gwm
 * 
 * Revision 1.4.1.16  89/08/28  19:01:29  colas
 * colors no more cached
 * window-program-set-position, window-program-set-size
 * (match exp "string" n p q) and always returns NIL
 * No more was_in_zrt field!
 * 
 * Revision 1.4.1.14  89/08/23  17:06:38  colas
 * defname was changing targetwob
 * wob-x, wob-y
 * ungrab-server do not take args anymore
 * no more freeze-on-grabs, bug (replayable-event...)
 * terser X error messages
 * multi-line strings ok
 * should trap sudden death of clients
 */

#include "wool.h"
DECLARE_strchr;

GWM_print_banner()
{
    static char     rev_number[20];

    strncpy(rev_number, strchr(RCS_Header, ',') + 1,
	    strchr(RCS_Header + 9, ':') - strchr(RCS_Header, ',') - 12);
    printf("%s %s\n", GWM_Banner, rev_number);
}
