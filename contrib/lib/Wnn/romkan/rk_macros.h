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
/***********************************************************************
			rk_macros.h
						87.11.17  �� ��

    ���Ѵ��ǻȤäƤ���ޥ���ؿ�����rk_header.h ��include����Ƥ��롣
    �����include����ȡ���ưŪ��rk_spclval.h��include����롣�桼����
    �ץ����Ǥ����include����С�to_upper�ʤɤΥޥ����Ȥ��롣 
***********************************************************************/

/*	Version 3.0
 */

#ifndef RKMCRO

#define RKMCRO
#include "rk_spclval.h" /* ctype.h��Ȥ�ʤ��ޥ���ϡ��������������Ƥ��� */

#ifndef MVUX
#  include <ctype.h>
#endif

	/* �ޥ���ؿ����ʰ�����ʣ����ɾ�������Τ�¿���Τ���ա�*/

 /* 7�ӥåȥ�����char�ѥޥ��� */
#define isoctal(c) (isdigit(c) && (c) < '8') /* 8�ʤο����� */
#define isnulsp(c) (isspace(c) || (c) == '\0') /* EOL���϶���ʸ���Ǥ��뤫 */

  /* ��ʸ��������ʸ���Ѵ���������
     SYSVR2�����  toupdown   7�ӥå�char��isascii������Ω���ϰϡ�
		   _toupdown  ��ʸ����isalpha������Ω���ϰϡ�
    ����¾�ξ��   toupdown		    ��				*/
#ifdef SYSVR2
#  define _toupdown(c) (isupper(c) ? _tolower(c) : _toupper(c))
#  define toupdown(c) (isupper(c) ? _tolower(c) : toupper(c))
#else
#  define toupdown(c) (isupper(c) ? tolower(c) : toupper(c))
#endif

 /* ctype�ޥ�����������ĥ������Ρ�letter���������ˤ�Ŭ�Ѳġ�
    is_eolsp�������ȼ��Τ�Ρ�*/
#define is_lower(l) (isascii(l) && islower(l))
#define is_upper(l) (isascii(l) && isupper(l))
#define is_alpha(l) (isascii(l) && isalpha(l))
#define is_alnum(l) (isascii(l) && isalnum(l))
#define is_digit(l) (isascii(l) && isdigit(l))
#define is_octal(l) (isascii(l) && isoctal(l))
#define is_xdigit(l) (isascii(l) && isxdigit(l))
#define is_space(l) (isascii(l) && isspace(l))
#define is_cntrl(l) (isascii(l) && iscntrl(l))
#define is_nulsp(l) (isascii(l) && isnulsp(l))
#define is_eolsp(l) (is_space(l) || (l) == EOLTTR)
#ifdef SYSVR2
#  define to_upper(l) (is_lower(l) ? _toupper(l) : (l))
#  define to_lower(l) (is_upper(l) ? _tolower(l) : (l))
#  define to_updown(l) (is_alpha(l) ? _toupdown(l) : (l))
#else
#  define to_upper(l) (is_lower(l) ? toupper(l) : (l))
#  define to_lower(l) (is_upper(l) ? tolower(l) : (l))
#  define to_updown(l) (is_alpha(l) ? toupdown(l) : (l))
#endif

#endif /* RKMCRO */
