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

#include <stdio.h>
#include "commonhd.h"
#include "de_header.h"
#include "jdata.h"

inspect(dic_no, serial, yomi, jd)
int dic_no;
int serial;
w_char *yomi;
struct jdata *jd;
{
    struct JT *jtl;

    if(dic_no < 0)return(-1);
    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    if(jtl->syurui == WNN_UD_DICT){
	if(inspect_ud(dic_no , serial, yomi, jd) == -1) return(-1);
    }else if(jtl->syurui == WNN_REV_DICT){
	if(inspect_rd(dic_no , serial, yomi, jd) == -1) return(-1);
    }else{
	if(inspect_sd(dic_no , serial, yomi, jd) == -1) return(-1);
    }
    return(0);
}



int get_yomi_from_serial(dic_no, serial, yomi)
int dic_no;
int serial;
w_char *yomi;
{
    int k;
    struct jdata jdata;
    struct JT *jtl;

    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    for(k = 0 ; k < LENGTHYOMI; k++){
	yomi[k] = 0;
    }

    if(jtl->syurui == WNN_UD_DICT){
	if(inspect_ud(dic_no , serial, yomi, &jdata) == -1) return(-1);
    }else if(jtl->syurui == WNN_REV_DICT){
	if(inspect_rd(dic_no , serial, yomi, &jdata) == -1) return(-1);
    }else{
	if(inspect_sd(dic_no , serial, yomi, &jdata) == -1) return(-1);
    }
    return(0);
}


inspect_sd(dic_no , serial, yomi, jd)
int dic_no;
int serial;
w_char *yomi;
struct jdata *jd;
{
    int len;
    struct JT *jtl;
    struct HJT *hjtl;
    UCHAR *hontaistart;
    UCHAR *hopter;
    int key;
    int tsize;
    w_char *charst;
    w_char *sumst;
    int *ptrst;
    int state;
    int sdiff;
    int last;
    int serialst;
    int kanji1;
    

    yomi[0] = 0;
    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    if(dic_table[dic_no].hindo >= 0)
	hjtl = (struct HJT *)(files[dic_table[dic_no].hindo].area);
    else hjtl = NULL;
    hontaistart = jtl->hontai;
    hopter = hontaistart;
    if(serial >= jtl->maxserial){
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }
    if(jtl->hinsi[serial] == SAKUJO_HINSI) return(-1);

    for(;;){
	switch((state = *(w_char *)hopter)){
	case ST_NORMAL:
	case ST_NOPTER:
	    tsize = *(w_char *)(hopter + 2);
	    serialst = *(int *)(hopter + 4);
	    kanji1 = *(int *)(hopter + 8);
	    charst = (w_char *)(hopter + 12);
	    sumst = ((w_char *)charst + tsize + 2);
	    /* + 2 keeps two zero words */
	    ptrst = (int *)((w_char *)sumst + tsize);
	    sdiff = serial - *(int *)(hopter + 4);
	    
	    /* May be a little late, but enough */
	    if(sdiff <  sumst[tsize - 1]){
		for(key = 0 ; key < tsize; key++){
		    if(sdiff < sumst[key]){
			break;
		    }
		}
		len = Strlen(yomi);
		yomi[len] = charst[key];
		yomi[len + 1] = 0;

		jd->kanji1 = kanji1;
		jd->kanji2 = serial - serialst;
		jd->serial = serial;
		jd->kosuu = 1;	
		jd->jishono = dic_no;
		jd->jptr = NULL;
		jd->hinsi = jtl->hinsi + serial;
		if(hjtl){
		    jd->hindo = hjtl->hindo + serial;
		    jd->hindo_in = jtl->hindo + serial;
		}else{
		    jd->hindo = jtl->hindo + serial;
		    jd->hindo_in = NULL;
		}
		return(0);
	    }else{
		if(state == ST_NOPTER){
		    wnn_errorno = WNN_WORD_NO_EXIST;
		    return(-1);
		}
		last = 0;
		for(key = 0 ; key < tsize; key++){
		    if(ptrst[key]){
			if(serial < get_fst_serial(hontaistart + ptrst[key], hontaistart))
			    break;
			last = key;
		    }
		}
		len = Strlen(yomi);
		yomi[len] = charst[last];
		yomi[len + 1] = 0;
		hopter = hontaistart + ptrst[last];
	    }
	    break;
	case ST_NOENT:
	    tsize = *(w_char *)(hopter + 2);
	    charst = (w_char *)(hopter + 4);
	    ptrst = (int *)AL_INT((w_char *)charst + tsize);
	    for(key = 0 ; key < tsize; key++){
		if(ptrst[key]){
		    if(serial < get_fst_serial(hontaistart + ptrst[key],hontaistart))
			break;
		    last = key;
		}
	    }
	    len = Strlen(yomi);
	    yomi[len] = charst[last];
	    yomi[len + 1] = 0;
	    hopter = hontaistart + ptrst[last];
	    break;
	case ST_SMALL:
	    len = Strlen(yomi);
	    yomi[len] = *(w_char *)(hopter + 2);
	    yomi[len + 1] = 0;
	    hopter = hopter + 4;
	    break;
	default:
	    error1( "Jserver:inspect_sd:Error 2\n");
	}
    }
}

get_fst_serial(hopter, hontaistart)
UCHAR *hontaistart;
char *hopter;
{
    int state, tsize;
    int k;
    int *ptrst;
    w_char *charst;

    switch((state = *(w_char *)hopter)){
    case ST_NORMAL:
    case ST_NOPTER:
	return(*(int *)(hopter + 4));
    case ST_NOENT:
	tsize = *(w_char *)(hopter + 2);
	charst = (w_char *)(hopter + 4);
	ptrst = (int *)AL_INT((w_char *)charst + tsize);
	for(k = 0; k < tsize; k++){
	    if(ptrst[k]){
		return(get_fst_serial(hontaistart + ptrst[k], hontaistart));
	    }
	}
	break;
    case ST_SMALL:
	return(get_fst_serial(hopter + 4, hontaistart));
    default:
	error1( "Jserver:get_fst_serial:Error 1\n");
    }
    return(-1);
}
	

inspect_ud(dic_no , serial, yomi, jd)
int dic_no;
int serial;
w_char *yomi;
struct jdata *jd;
{
    struct JT *jtl;
    struct HJT *hjtl;
    struct uind1 *tary;
    struct uind2 *p;
    int k;
    int len;

    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    tary = jtl->table;
    if(dic_table[dic_no].hindo >= 0)
	hjtl = (struct HJT *)(files[dic_table[dic_no].hindo].area);
    else hjtl = NULL;

    if(serial >= jtl->maxserial){
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }
    if(jtl->hinsi[serial] == SAKUJO_HINSI) return(-1);

    for(k = 0 ; k < jtl->maxtable ; k++){
	for(p=((struct uind2 *)((tary[k].pter) + jtl->hontai));
	    ;p = ((struct uind2 *)((p->next) + jtl->hontai))){
	    if(p ->serial <=serial &&
	       p -> serial + p->kosuu > serial) goto found_it;
	    if(p->next == ENDPTR) break;
	}
    }
    wnn_errorno = WNN_WORD_NO_EXIST;
    return(-1);

 found_it:
    yomi[0] = tary[k].yomi1 >> 16;
    yomi[1] = tary[k].yomi1 & 0xffff;
    if(yomi[1]){
	yomi[2] = tary[k].yomi2 >> 16;
	if(yomi[2]){
	    yomi[3] = tary[k].yomi2 & 0xffff;
	}
    }
    len = p->yomi[0];
    Strncpy(yomi + 4, (p->yomi) + 1, len - 4);
    yomi[len] = 0;

    jd->kanji1 = p->kanjipter;
    jd->kanji2 = serial - p->serial;
    jd->serial = serial;
    jd->kosuu = 1;	
    jd->jishono = dic_no;
    jd->jptr = NULL;
    jd->hinsi = jtl->hinsi + serial;
    if(hjtl){
	jd->hindo = hjtl->hindo + serial;
	jd->hindo_in = jtl->hindo + serial;
    }else{
	jd->hindo = jtl->hindo + serial;
	jd->hindo_in = NULL;
    }

    return(0);
}


