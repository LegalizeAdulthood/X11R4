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
/*********************
 * kaiseki work area
**********************/

#include "commonhd.h"
#include "ddefine.h"

wchar_t	*bun;	/* D */

wchar_t giji_eisuu[MAX_GIJI_CHAR];	/* 擬似「英数」の定義 */

int	maxchg;
int	initjmt;	/* I think initjmt is the length of jmt_ */

int	*maxj;	/* maxj is counts to entries in jmt_ */

/* jmt_ptr is used in jmt0.c only.
   but it must be changed by clients.
   jishobiki does not use it. this is sent to jishobiki by arguments
   */

/* j_e_p is used to hold the current point to which jmtw_ is used */
struct	jdata	*j_e_p;
struct	jdata	**jmt_;
struct	jdata	*jmtw_;
struct	jdata	**jmt_end;
struct	jdata	*jmtw_end;
struct	jdata	**jmt_ptr;
struct	jdata	***jmtp;

struct	FT	*ft;

/********************************
 *	疑似自立語の品詞	*
 ********************************/
int	sentou_no;	/* 「先頭」文節先頭に成れるもの */
int	suuji_no;	/* 「数字」数詞相当 */
int	katakanago_no;	/* 「カナ」外来語など 名詞相当 */
int	eisuu_no;	/* 「英数」*/
int	kigou_no;	/* 「記号」 */
int	toji_kakko_no;	/* 「閉括弧」 */
int	fuzokugo_no;	/* 付属語だけ */
int	kai_kakko_no;	/* 「開括弧」*/
int	giji_no;	/* 疑似 */
