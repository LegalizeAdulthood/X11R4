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
/*
	demcom.h
	entry functions	definitions
*/

/*
 *	Demon Commands
 */
#define	JS_VERSION	0x00
#define	JS_OPEN		0x01
/*	#define	JS_OPEN_IN	0x02	*/
#define	JS_CLOSE	0x03
#define	JS_CONNECT	0x05
#define	JS_DISCONNECT	0x06
#define	JS_ENV_EXIST	0x07	/* ADD KURI */
#define	JS_ENV_STICKY	0x08	/* ADD KURI */
#define	JS_ENV_UN_STICKY	0x09	/* ADD KURI */


#define	JS_KANREN	0x11
#define	JS_KANTAN_SHO	0x12
#define	JS_KANZEN_SHO	0x13
#define	JS_KANTAN_DAI	0x14
#define	JS_KANZEN_DAI	0x15
#define	JS_HINDO_SET	0x18


#define	JS_DIC_ADD	0x21
#define	JS_DIC_DELETE	0x22
#define	JS_DIC_USE	0x23
#define	JS_DIC_LIST	0x24
#define	JS_DIC_INFO	0x25

#define	JS_FUZOKUGO_SET	0x29
#define	JS_FUZOKUGO_GET	0x30


#define	JS_WORD_ADD	0x31
#define	JS_WORD_DELETE	0x32
#define	JS_WORD_SEARCH	0x33
#define	JS_WORD_SEARCH_BY_ENV	0x34
#define	JS_WORD_INFO	0x35
#define JS_WORD_COMMENT_SET 0x36

#define	JS_PARAM_SET	0x41
#define	JS_PARAM_GET	0x42

#define	JS_MKDIR	0x51
#define	JS_ACCESS	0x52
#define	JS_WHO		0x53
#define	JS_ENV_LIST	0x55
#define	JS_FILE_LIST_ALL	0x56
#define	JS_DIC_LIST_ALL	0x57

#define	JS_FILE_READ	0x61
#define	JS_FILE_WRITE	0x62
#define	JS_FILE_SEND	0x63
#define	JS_FILE_RECEIVE	0x64

#define	JS_HINDO_FILE_CREATE	0x65
#define	JS_DIC_FILE_CREATE	0x66
#define JS_FILE_REMOVE	0x67	/* H.T. */

#define	JS_FILE_LIST	0x68
#define	JS_FILE_INFO	0x69
#define	JS_FILE_LOADED	0x6A
#define	JS_FILE_LOADED_LOCAL	0x6B
#define	JS_FILE_DISCARD	0x6C
#define JS_FILE_COMMENT_SET 0x6D
#define JS_FILE_PASSWORD_SET 0x6E /* 89/9/8 */

#define	JS_FILE_STAT	0x6F
#define JS_KILL		0x70	/* H.T. */

#define	JS_HINDO_FILE_CREATE_CLIENT	0x71 /* H.T. */
#define	JS_HINSI_LIST			0x72 /* H.T. */
#define JS_HINSI_NAME	0x73
#define JS_HINSI_NUMBER	0x74
#define JS_HINSI_DICTS  0x75
#define JS_HINSI_TABLE_SET 0x76	/* 89/9/8 */

/*

#define	JD_BEGIN	0x03
#define	JD_END		0x04

#define	JD_NEXT		0x11
#define	JD_RECONV	0x12
#define	JD_TANCONV	0x13

#define	JD_UDP		0x21
#define	JD_UDCHG	0x22
#define	JD_FREQSV	0x23
#define	JD_DICADD	0x24
#define	JD_DICDEL	0x25
#define	JD_DICINFO	0x26
#define	JD_DICSTAT	0x27	/* V3.0

#define	JD_WDEL		0x31
#define	JD_WSCH		0x32
#define	JD_WREG		0x33
#define JD_WHDEL	0x34

*/
