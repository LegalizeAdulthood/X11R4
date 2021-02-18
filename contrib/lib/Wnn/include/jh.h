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
  header file for jisho structure program.
*/

/* The following defin and structure definition
 * are concerned with ASCII (UJIS) files.
 */

#define COMMENT "\\comment"
#define TOTAL "\\total"
#define ASC_GIJI "\\giji"
#define HINSI "\\hinsi"
#define DIC_NO "\\dic_no"

#define REV_NORMAL 2
#define REVERSE 1
#define NORMAL  0

struct je {
  w_char *yomi;
  w_char *kan;			/* Historically kanji is used so use kan. */
  w_char *comm;
  UCHAR *kanji;
  unsigned int hinsi;
  unsigned int hindo;
  int serial;			/* Only used for rev_dic */
};

extern struct je **jeary;
extern w_char file_comment[];
extern w_char hinsi_list[];

/*
 * Used in atod and others parameters.
 */

#define HEAP_PER_LINE 10	/* avelage of kanji + comment bytes */
#define YOMI_PER_LINE 3		/* avelage of yomi length(in w_char)*/
#define LINE_SIZE 1024
#define BADLMAX 3		
#define YOMI_KINDS (1 << 16)   /* Yomi characters Maximal */
#define MAX_ENTRIES 70000	/* default of max-entries for atod */
#define DEF_ENTRIES 10000

#define HEAPINC 1000
