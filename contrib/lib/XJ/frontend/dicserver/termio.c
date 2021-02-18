/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OMRON not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  OMRON makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *	Original Source: ./lib/Wnn/uum/termio.c
 *
 *	Modified by OMRON TATEISI ELECTRONICS CO., to handle XJp-library.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*
	Copyright
		Kyoto University Research Institute for Mathematical Sciences
		1987, 1988, 1989
	Copyright OMRON TATEISI ELECTRONICS CO. 1987, 1988, 1989
	Copyright ASTEC, Inc. 1987, 1988, 1989


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without any fee is
    hereby granted, subject to the following restrictions:

    The above copyright notice and this permission notice must appear
    in all versions of this software;

    The name of "Wnn" may not be changed; 

    All documentations of software based on "Wnn" must contain the wording
    "This software is based on the original version of Wnn developed by
    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
    OMRON TATEISI ELECTRONICS CO. and
    ASTEC, Inc.", followed by the above copyright notice;

    The name of KURIMS, OMRON and ASTEC may not be used
    for any purpose related to the marketing or advertising
    of any product based on this software.

    KURIMS, OMRON and ASTEC make no representations about
    the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    KURIMS, OMRON and ASTEC DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
    IN NO EVENT SHALL KURIMS, OMRON AND ASTEC BE LIABLE FOR ANY SPECIAL,
    INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

    Author: Hideki Tsuiki	Kyoto University
				tsuiki%kaba.or.jp@uunet.uu.net

	    Hiroshi Kuribayashi	OMRON TATEISI ELECTRONICS CO.
				kuri@frf.omron.co.jp
				uunet!nff.ncl.omron.co.jp!kuri

	    Naouki Nide		Kyoto University
				nide%kaba.or.jp@uunet.uu.net

	    Shozo Takeoka	ASTEC, Inc.
				take%astec.co.jp@uunet.uu.net

	    Takashi Suzuki	Advanced Software Technology & Mechatronics
				Research Institute of KYOTO
				suzuki%astem.or.jp@uunet.uu.net

*/
/*	Version 4.0
 */

#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"


clr_end_screen()
{
#ifndef	X11
	TTYMline_clear(TTY_currentcol) ;
#else	X11
	JWMline_clear(cur_jutil->cl->currentcol) ;
#endif	!X11
}


throw_cur_raw(col, row)
int	col, row;
{
#ifndef	X11
	TTYcursor_move(col) ;
#else	X11
	JWcursor_move(col) ;
#endif	!X11
}


h_r_on_raw()
{
#ifndef	X11
	TTYMset_reverse() ;
#else	X11
	JWMset_reverse() ;
#endif	!X11
}


h_r_off_raw()
{
#ifndef	X11
	TTYMreset_reverse() ;
#else	X11
	JWMreset_reverse() ;
#endif	!X11
}


u_s_on_raw()
{
#ifndef	X11
	TTYMset_under() ;
#else	X11
	JWMset_under() ;
#endif	!X11
}


u_s_off_raw()
{
#ifndef	X11
	TTYMreset_under() ;
#else	X11
	JWMreset_under() ;
#endif	!X11
}


b_s_on_raw()
{
#ifndef	X11
	TTYMset_bold() ;
#else	X11
	JWMset_bold() ;
#endif	!X11
}


b_s_off_raw()
{
#ifndef	X11
	TTYMreset_bold() ;
#else	X11
	JWMreset_bold() ;
#endif	!X11
}


ring_bell()
{
#ifndef	X11
	XBell(dpy, 0);
#else	X11
	XBell(dpy, 0);
#endif	!X11
}


cursor_invisible_raw()
{
#ifndef	X11
	TTYcursor_invisible() ;
#else	X11
	JWcursor_invisible() ;
#endif	!X11
}


cursor_normal_raw()
{
#ifndef	X11
	TTYMreset_under() ;
	TTYMreset_reverse() ;
	TTYcursor_visible() ;
#else	X11
	JWMreset_under() ;
	JWMreset_reverse() ;
	JWcursor_visible() ;
#endif	!X11
}
