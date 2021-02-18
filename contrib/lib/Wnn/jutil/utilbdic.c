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
  Binary (Updatable, Stable) dictionary common routine.
*/

/* extern functions are
  output_header(ofpter)
  output_hindo(ofpter)
  output_hinsi(ofpter)
  output_kanji(ofpter)
*/

#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

int wnnerror;

struct JT jt;
struct wnn_file_head file_head;

extern char *passwd;

output_comment(fp)
register FILE *fp;
{
    return(put_n_EU_str(fp, file_comment, jt.maxcomment));
}

output_hinsi_list(fp)
register FILE *fp;
{
    return(put_n_EU_str(fp, hinsi_list, jt.maxhinsi_list));
}

input_comment(fp)
register FILE *fp;
{
    return(get_n_EU_str(fp, jt.maxcomment, file_comment));
}

input_hinsi_list(fp)
register FILE *fp;
{
    return(get_n_EU_str(fp, jt.maxhinsi_list, hinsi_list));
}

output_hindo(ofpter)
register FILE *ofpter;
{
   register int i;
    for(i = 0 ; i < jt.maxserial ; i++){
	putc(asshuku(jeary[i]->hindo),ofpter);
    }
}

input_hindo(ifpter)
register FILE *ifpter;
{
    register int i;
    register int k;
    for(i = 0 ; i < jt.maxserial ; i++){
	jeary[i]->hindo = motoni2((k = getc(ifpter)) & 0x7f);
	if(k == -1)return(-1);
    }
    return(0);
}


output_hinsi(ofpter)
register FILE *ofpter;
{
    register int i;

    for(i = 0 ; i < jt.maxserial ; i++){
	put_short(ofpter,jeary[i]->hinsi);
    }
/*
  if(i & 0x1){
    put_short(ofpter,0);
  }
*/
}

input_hinsi(ifpter)
register FILE *ifpter;
{
    register int i;
    unsigned short s;

    for(i = 0 ; i < jt.maxserial ; i++){
	if(get_short(&s, ifpter) == -1)return(-1);
	jeary[i]->hinsi = s;
    }
/*
  if(i & 0x1){
    get_short(ifpter);
  }
*/
  return(0);
}

#ifdef nodef
revdic_jt(jtp, match, hostart, tary)
struct JT *jtp;
int match;
char *hostart;
struct uind1 *tary;
{
/* Must reverse hinsi?????? */

    if(jtp->syurui == WNN_UD_DICT){
	rev_ud_hontai(hostart,jtp->maxhontai, match);
	rev_ud_table(tary,jtp->maxtable, match);
    }else{
	rev_sd_hontai(hostart, match);
    }
    return(0);
}
#endif
