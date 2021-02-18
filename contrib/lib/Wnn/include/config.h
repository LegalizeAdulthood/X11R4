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

#define WNN_USERNAME_ENV	"WNNUSER"
#define WNN_JSERVER_ENV		"JSERVER"
#define WNN_UUM_ENV  		"UUMRC"
#define WNN_KEYBOARD_ENV	"KEYBOARD"
#define WNN_COUNTDOWN_ENV	"UUM_COUNTDOWN"

#define PATHNAMELEN	256					/* ADD KURI */

/* for uum */
#define RCFILE			"/usr/local/lib/wnn/uumrc"
#define USR_UUMRC		"/.uumrc"
#define RKFILE			"/usr/local/lib/wnn/rk/mode"
#define CPFILE			"/usr/local/lib/wnn/uumkey"
#define CONVERT_FILENAME	"/usr/local/lib/wnn/cvt_key_tbl"
#define MESSAGEFILE		"/usr/local/lib/wnn/message_file"

#define USR_DIC_DIR_VAR "@USR"

#define LIBDIR			"/usr/local/lib/wnn"
#define ENVRCFILE		"/usr/local/lib/wnn/wnnenvrc"

#define HINSIDATA_FILE "/usr/local/lib/wnn/hinsi.data"

/* OS dependent staff */
#ifdef	SYSVR2
#define	SIGCHLD	SIGCLD
#define	getdtablesize()	(NOFILE)	/* sys/param.h must be included */
#define	re_signal(x, y)	signal((x), (y))
#else
#define re_signal(x, y)
#endif

#ifdef UX386
#define SIGCHLD SIGCLD
#endif

/*
  if your system has wait3() system call define HAVE_WAIT3.
  wait3() doesn't have to be fully supported.
  uum uses only NULL for the 3rd parameter rusage.
 */

#ifndef	UX386
#define HAVE_WAIT3
#endif

/*
  if you wish to do flow control active for your tty,
  define FLOW_CONTROL to 1.
  note that this 'tty' means the tty from which wnn is invoked.
 */

#define FLOW_CONTROL 0

#define C_LOCAL '!'		
/* For Local File Name.
   Local File Name is send as "Hostname!Filename" when C_LOCAL is '!'.
   It is also used in jl_library to specify local file-name, that is,
   file-names which start with this character are considered to be local.
   */


/*
  define default kanji code system for your 'tty' side and 'pty' side.
  'tty' side (TTY_KCODE) means 'your terminal's code'.
  'pty' side (PTY_KCODE) means 'application's code'.
 */

#ifdef	SX8870
#define TTY_KCODE J_EUJIS
#define PTY_KCODE J_EUJIS
#else
# ifdef SX9100BSD
#define TTY_KCODE J_EUJIS
#define PTY_KCODE J_EUJIS
# else
#define TTY_KCODE J_JIS
#define PTY_KCODE J_JIS
# endif
#endif

/*
  OPTIONS defines what options are available.
  define it by modifying ALL_OPTIONS string.
  if you wish to make some option abailable, leave that character unchanged.
  else turn that character some non-option character, ex. '*'.
  TAKE CARE NOT TO MOVE CHARACTER POSITION, ORDER, ETC!!

  see sdefine.h for precise definition of ALL_OPTIONS. defines below
  may be incorrect.

#define GETOPTSTR   "hHujsUJSPxXk:c:r:l:D:n:v"
#define ALL_OPTIONS "hHujsUJSPxXkcrlDnv"
 */

#define OPTIONS     "hHujsUJSPxXkcrlDnv"

#ifndef OPTIONS
#define OPTIONS ALL_OPTIONS
#endif

#define	WNN_TIMEOUT	5	/* connect の際に５秒待つんだよ */
/* どん臭い、ｔｃｐを使っている奴は、もっと長くせんとあかんよ */
/*
  Local Variables:
  kanji-flag: t
  End:
*/
