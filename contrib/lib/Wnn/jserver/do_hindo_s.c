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
#include "commonhd.h"
#include "jslib.h"
#include "kaiseki.h"
#include "ddefine.h"
#include "jdata.h"
#include "de_header.h"
    
js_hindo_set()
{
    int env_id,dic,entry,ima,hindo,x;
    int err = 0;
    if((env_id=envhandle()) == -1) err = -1;
    dic=get4_cur();
    entry=get4_cur();
    ima=get4_cur();
    hindo=get4_cur();
    if(err == -1){ error_ret(); return;}
    if(dic == -1){
	giji_hindoup(entry);
	put4_cur(0);
	putc_purge();
	return;
    }
    if(find_dic_in_env(env_id,dic) == -1){ /* no such dic */
	wnn_errorno=WNN_DICT_NOT_USED; error_ret();
	return;
    }
    if(dic_table[dic].hindo_rw){
	wnn_errorno = WNN_RDONLY_HINDO;error_ret();
	return;
    }
    x=hindo_set(dic,entry,ima,hindo);
    if(x== -1){ error_ret();}
    else{ put4_cur(x); }
    putc_purge();
}

hindo_set(dic_no,entry,imaop,hinop)
int dic_no;
int entry;
int imaop;
int hinop;
{
#if defined(SYSVR2) || defined(SUN)
    double drand48();
#define RAND()  drand48()
#else
    double drand();
#define RAND()  ((double)rand() / (double)((1<<31) - 1))
#endif
    UCHAR *hst;
    int ima, hindo;

    if(dic_table[dic_no].hindo == -1){
	hst = ((struct JT *)(files[dic_table[dic_no].body].area))->hindo;
	((struct JT *)(files[dic_table[dic_no].body].area))->hdirty = 1;
	((struct JT *)(files[dic_table[dic_no].body].area))->dirty = 1; /* ADD KURI */
    }else{
	hst = ((struct HJT *)(files[dic_table[dic_no].hindo].area))->hindo;
	((struct HJT *)(files[dic_table[dic_no].hindo].area))->hdirty = 1;
    }
    ima = hst[entry] & 0x80;
    hindo = hst[entry] & 0x7f;
    switch(imaop){
    case WNN_HINDO_NOP:
	break;
    case WNN_IMA_ON:
	ima = 0x80;
	break;
    case WNN_IMA_OFF:
	ima = 0x0;
	break;
    }
    switch(hinop){
    case WNN_HINDO_NOP:
	break;
    case WNN_HINDO_INC:
      if(hindo != 0x7f && ( RAND() < (double)1 / ((hindo >> 2) + 1)))
	  hindo++;
	break;
    case WNN_HINDO_DECL:
      if(hindo != 0 && ( RAND() < (double)1 / ((hindo >> 2) + 1)))
	  hindo--;
	hindo = 0x0;
	break;
    case WNN_ENTRY_NO_USE:
	hindo = 0x7f;
	break;
    default:
	hindo = asshuku(hinop);
    }
    hst[entry] = hindo | ima;
    return(0);
}

    
