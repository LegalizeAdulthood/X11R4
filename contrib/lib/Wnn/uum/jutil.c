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
/* jisho utility routine for otasuke (user interface) process */
/*	�輭��Ȥϥ����ȥ桼������Τ��Ȥ���	*/

#include <stdio.h>
#include "jslib.h"
#include "jllib.h"
#include "commonhd.h"
#ifdef SYSVR2
#   include <fcntl.h>
#endif
#ifdef BSD42
#   include <sys/file.h>
#endif
#include "sdefine.h"
#include "sheader.h"
#include <stdio.h>

#define MESSAGE_REC 3
#define MESSAGE_NUM 5


char *sh_fname();

static void ret() ; /* remove sonota() , nullfun() */
 void touroku() , kensaku() ;
 void jishoadd() , dicsv(),fuzoku_set();
 void  paramchg() , dicinfoout(), select_one_dict9();
 /*
 void jishodel();
 void sakujyo();
 void dicstatuschg();
 void unable_entry();
 */

static struct msg_fun {
  char *msg;
  void (*fun)();
};

static struct msg_fun message1[] = { 
  {"�����ɲ�", jishoadd},
  {"�������",select_one_dict9},
  {"��Ͽ", touroku},
  {"����", kensaku},  
  {"�ѥ�᡼���ѹ�",paramchg},
  {"���٥�����",dicsv},
  {"�������",dicinfoout},
  {"��°���ѹ�", fuzoku_set},
		/* ����4�ĤϤ⤦�פ�ʤ���Ǥ����ɤ����ޤ��礦�� */
/*
  {"���", sakujyo},
  {"̵��/ͭ��", unable_entry},
  {"������",jishodel},
  {"�������/�Ի���", dicstatuschg},
*/
  {"��λ",ret}
};

jutil()
{
  static int last = 3;
  int not_redrawtmp = not_redraw;
  int c;
  int k;
  char *buf[sizeof(message1) / sizeof(struct msg_fun)];

  not_redraw = 1;
  for(k = 0 ; k <( sizeof(message1) / sizeof(struct msg_fun));k++){
    buf[k]=message1[k].msg;
  }

  c = select_one_element(buf,sizeof(message1)/sizeof(struct msg_fun),last,"",0,1,main_table[4]);
  if(c == -1) {
	not_redraw = not_redrawtmp;
	return(0);
  }
  last = c;
  (*message1[c].fun)();
  not_redraw = not_redrawtmp;
  return(0);
}

static void ret()
{
}


void
paramchg()
{
    int c,k;
    int newval;
    char *ch;
    char st[80];
    char message[80];
    w_char kana_buf[1024];

    static char *ccc[] = {"����ʸ���" , "��ʸ��κ����",
"���ٽŤ�" , "��ʸ��Ĺ�Ť�" , "��Ω��Ĺ�Ť�" , "�Ƕ���Ѿ����Ť�" , "����ͥ���ٽŤ�", "��ʸ���ɾ���ͽŤ�", "��ʸ��Ĺ�Ť�", "��ʸ����Ť�", /* "����Ĺ�Ť�", */
"����������", "���ʤ�����", "�ѿ�������", "���������", "�ĳ�̤�����", "��°�������", "����̤�����"};
    struct wnn_param para;
    int *paraary;
    char *cccmsg[(sizeof(ccc) / sizeof(char *))];
    char buf[1024];
    
    paraary = (int *)&para;
    if(jl_param_get(bun_data_, &para) < 0) {
    	errorkeyin();
    }
    for(k = 0,ch = buf ; k < (sizeof(ccc) / sizeof(char *)) ; k++){
      cccmsg[k] = ch;
      sprintf(ch,"%s[%d]",ccc[k],paraary[k]);
      ch += strlen(ch) + 1;
    }
  repeat:
    if((c = select_one_element(cccmsg,(sizeof(ccc) / sizeof(char *)),
			       0,"",0,1,main_table[4])) == -1){
      return;
    }
    
    throw_c(0);
    clr_line();

    if(c == 0){
      sprintf(message , "���� %dʸ�����ˡ���Ѥ��Ƥ��ޤ�����ʸ�����ˡ���Ѥ��ޤ��� ?" , paraary[0]);
    }else{
      sprintf(message , "���ߤ�%s�� %d�Ǥ������Ĥˤ��ޤ���  ? ", ccc[c] , paraary[c] );
    }
    kana_buf[0] = 0;
    if(kana_in(message , UNDER_LINE_MODE ,kana_buf , 1024) == -1)return;
    if(wchartochar(kana_buf , st) ||
       sscanf(st , "%d" , &newval) <= 0 ){
      print_msg_getc("���������Ϥ��Ƥ���������(ǡ��?)");
      goto repeat;
    }
    if(c == 0 && newval <= 0){
      print_msg_getc("���ο��������Ϥ��Ƥ���������(ǡ��?)");
      goto repeat;
    }
    paraary[c] = newval;
    if(jl_param_set(bun_data_, &para) < 0) {
      errorkeyin();
    }
}


void
dicsv()
{
    print_msg("��������������٤ȥ桼������򥻡��֤��Ƥ��ޤ���");
    flush();
    if(jl_dic_save_all(bun_data_) == -1){
    	errorkeyin();
    }
}

update_dic_list(buf)
struct wnn_buf *buf;
{
    if((dic_list_size = jl_dic_list(bun_data_, &dicinfo)) == -1){
	errorkeyin();
	return(-1);
    }
    return(0);
}

dic_nickname(dic_no, buf)
int dic_no;
char *buf;
{
    int j;

    if((j = find_dic_by_no(dic_no)) == -1)return(-1);
    if(*dicinfo[j].comment) sStrcpy(buf, dicinfo[j].comment);
    else strcpy(buf, dicinfo[j].fname);
    return(0);
}

find_dic_by_no(dic_no)
int dic_no;
{
    int j;

    for(j = 0 ; j < dic_list_size; j++){
	if(dicinfo[j].dic_no == dic_no)
	    return(j);
    }
    return(-1);
}



void
dicinfoout()
{
    int k;
    int cnt;
    char buf[4096];
    char *info[JISHOKOSUU];
    int size;

    if(update_dic_list(bun_data_) == -1){
	errorkeyin();
	return;
    }
    cnt = dic_list_size;

    cnt += 1; /* for the comment line */
    size = 4096 / cnt;
    if(make_info_out(buf, size, dicinfo, cnt) == -1){
	print_msg("��ǰ������¿�����ޤ���");
	romkan_next();
      }
    for(k = 0 ; k < cnt ; k++){
      info[k] = buf + k * size;
    }
/*    select_line_element(info,cnt,0,"",0,0,main_table[7]); */
    set_lc_offset(1);	/* ADD KURI */
    select_line_element(info,cnt,0,"",0,0,main_table[9]); 
    set_lc_offset(0);	/* ADD KURI */
}


#ifdef	not_use
void jishodel()
{
    char buf[4096];
    int l;

    if((l = select_one_dict()) == -1)return;

    sprintf(buf, "���� %s �������ޤ���������Ǥ���?(Y/N)", dicinfo[l].fname);
    if(yes_or_no(buf) == 1){
	if(jl_dic_delete(bun_data_, dicinfo[l].dic_no) == -1){
	    errorkeyin();
	    return;
	}
    }
    return;
}

void dicstatuschg()
{
    char buf[4096];
    int l;

    if((l = select_one_dict()) == -1)return;

    jl_dic_use(bun_data_, dicinfo[l].dic_no,!dicinfo[l].enablef);
}

int
select_one_dict()
{
    return(select_one_dict1(7));
}
#endif not_use

void
select_one_dict9()
{
    set_lc_offset(0);	/* ADD KURI */
    select_one_dict1(9);
}

select_one_dict1(tbl)
int tbl;
{
    int l;
    char *c;
    char buf[4096];
    char *buf_ptr[JISHOKOSUU];
    char baka[1024];

    if(update_dic_list(bun_data_) == -1){
	errorkeyin();
	return;
    }
    if(dic_list_size == 0){
	print_msg_getc("���񤬰�Ĥ�ͭ��ޤ���");
	return;
    }

    for(c = buf,l = 0 ; l < dic_list_size ; l++){
	buf_ptr[l] = c;
	dic_nickname(dicinfo[l].dic_no, baka);
	sprintf(c,"%s(%s) %s %d/%d",baka, dicinfo[l].hfname,
		dicinfo[l].enablef?  "������":"������",
		l + 1,dic_list_size);
	c += strlen(c) + 1;
    }
    return(select_line_element(buf_ptr, dic_list_size, 0, "", 0, 0, main_table[tbl]));
}

print_msg1(X)
char *X;
{push_cursor();throw_c(0); clr_line();printf(X);flush();pop_cursor();
}

void
fuzoku_set()
{
    char *message = "��°��ե�����̾ >";
    char fname[128];
    w_char kana_buf[1024];

    Strcpy(kana_buf , fuzokugopath);
    if(kana_in(message , UNDER_LINE_MODE , kana_buf , 1024) == -1) return;
    wchartochar(kana_buf , fname);
    if(strlen(fname) == 0)return;
    Strcpy(fuzokugopath , kana_buf);
    if(jl_fuzokugo_set(bun_data_,fname) == -1){
	errorkeyin();  
    }
}

void
jishoadd()
{
    char *message = "����������� > ";
    char *hmessage = "���٥ե�����̾ > ";
    char *priomessage = "�����ͥ���٤ο�������ꤷ�Ʋ������� > ";
    char fname[128];
    char tmp[128];
    char hfname[128];
    int prio;
    int rdonly;
    int hrdonly;
    int id, l;
    w_char kana_buf[1024];

    Strcpy(kana_buf , jishopath);
    if(kana_in(message , UNDER_LINE_MODE , kana_buf , 1024) == -1) return;
    wchartochar(kana_buf , fname);
    if(strlen(fname) == 0)return;
    Strcpy(jishopath , kana_buf);

    Strcpy(kana_buf,hindopath);
    getfname(hfname,fname);
    if(strcmp(hfname + strlen(hfname) - 4, ".usr") == 0 ||
       strcmp(hfname + strlen(hfname) - 4, ".sys") == 0 ||
       strcmp(hfname + strlen(hfname) - 4, ".dic") == 0 ||
       strcmp(hfname + strlen(hfname) - 4, ".rev") == 0) {
	hfname[strlen(hfname) - 4] = 0;
	strcat(hfname , ".h");
    } else if(strcmp(hfname + strlen(hfname) - 5, ".usrR") == 0 ||
       strcmp(hfname + strlen(hfname) - 5, ".sysR") == 0) {
	hfname[strlen(hfname) - 5] = 0;
	strcat(hfname , ".hR");
    } else {
	strcat(hfname , ".h");
    }
    chartowchar(hfname, kana_buf + Strlen(kana_buf));
    if(kana_in(hmessage , UNDER_LINE_MODE , kana_buf , 1024) == -1){
	return;
    }
    wchartochar(kana_buf , hfname);
    sprintf(tmp,"%d",JISHO_PRIO_DEFAULT);
    chartowchar(tmp,kana_buf);
    if(kana_in(priomessage , UNDER_LINE_MODE , kana_buf , 1024) == -1) return;
    wchartochar(kana_buf , tmp);
    prio = atoi(tmp);
    if(yes_or_no_or_newline("����򹹿�����⡼�ɤǻȤ��ޤ���?(Y or Newline / N)") == 1){
	rdonly = 0;
    }else{
	rdonly = 1;
    }
    if(yes_or_no_or_newline("���٤򹹿�����⡼�ɤǻȤ��ޤ���?(Y or Newline / N)") == 1){
	hrdonly = 0;
    }else{
	hrdonly = 1;
    }

    throw_c(0);
    clr_line();
    printf("������������ɤ߹���Ǥ��ޤ���");
    flush();

/*
  print_msg("������������ɤ߹���Ǥ��ޤ���");
*/

    if((id = jl_dic_add(bun_data_,fname,hfname,0,prio,rdonly, hrdonly, NULL, NULL, yes_or_no_or_newline, print_msg1)) == -1 && wnn_errorno != -1){
	errorkeyin();  
    }
    update_dic_list(bun_data_);
    for(l = 0 ; l < dic_list_size ; l++){
	if(dicinfo[l].dic_no == id){
	    break;
	}
    }
    if(l != dic_list_size && env_kankana && dicinfo[l].type == WNN_REV_DICT){
	if(jl_dic_add_e(env_kankana, fname,hfname, 1,prio,rdonly, hrdonly, NULL, NULL, yes_or_no_or_newline, print_msg1) == -1 && wnn_errorno != -1){
	    errorkeyin();  
	}
    }
}

char *
sh_fname(s , n , d)  /* file̾ s��Ĺ�����ˡ����κǸ�������ä� nʸ���ˤޤȤ�� d�������   */ 
char *s , *d;
int n;
{
  int k;
  char *c , *c1;

  k = strlen(s);
  if(k < n){
    strcpy(d , s);
    return(d);
  }else{
    c = s + k - n + 2;
    c1 = d;
    *c1++ = '-';
    *c1++ = '-';
    for(;*c && *c != '/';c++){
      *c1++ = '-';
    }
    strcpy(c1 , c);
    return(d);
  }
}

yes_or_no(string)
char *string;
{
  int x;
    throw_c(0);
    clr_line();
    printf(string);
    flush();
  for(;;){
    x=romkan_next();
    if(x=='n' || x=='N') return(0);
    if(x == 'y' || x == 'Y')return(1);
  }
}


yes_or_no_or_newline(string)
char *string;
{
  int x;
    throw_c(0);
    clr_line();
    printf(string);
    flush();
  for(;;){
    x=romkan_next();
    if(x=='n' || x=='N') return(0);
    if(x == 'y' || x == 'Y' || x == '\n' || x == CR)return(1);
  }
}




static char *info_out_data[] ={
  "No.", 
  "Type",
  "������",
  "�ե�����̾",
  "���٥ե�����̾",
  "���ÿ�",
  "ͥ����",
  "��Ͽ��ǽ",
  "���ٹ�����ǽ",
  "�����椫",
  "��Ͽ�ѥ����",
  "���ٹ����ѥ����",
  ""
};

make_info_out(buf,size,infobuf,cnt)
char buf[];
WNN_DIC_INFO infobuf[];
int cnt;
int size;
{
  int max1 = 0;
  int max0 = 0;
  int i,k,j;
  
  for(k = 0 ; k < cnt ; k++){
    for(j = 0 ; j < size ; j++){
      buf[k * size + j] = 0;
    }
  }

  for(i = 0 ; i < 14;i++){
    if(i == 13){
      sprintf(buf + max0,"%3d/%-3d",0,cnt - 1);
    }else{
      sprintf(buf + max0,"%s",info_out_data[i]);
    }
    max1 = max(max1, strlen(buf));
    if(max1 >= size)return(-1);
    for(k = 1 ; k < cnt;k++){
      switch(i){
      case 0:
	sprintf(buf + size * k + max0,"%3d ",infobuf[k - 1].dic_no);
	break;
      case 1:
	sprintf(buf + size * k + max0,"%8s",
		wnn_dic_types[infobuf[k - 1].type]);
	break;
      case 2:
	sStrcpy(buf + size * k + max0,infobuf[k - 1].comment);
	break;
      case 3:
	strcpy(buf + size * k + max0,infobuf[k - 1].fname);
	break;
      case 4:
	strcpy(buf + size * k + max0,infobuf[k - 1].hfname);
	break;
      case 5:
	sprintf(buf + size * k + max0,"%6d",infobuf[k - 1].gosuu);
	break;
      case 6:
	sprintf(buf + size * k + max0,"%4d",infobuf[k - 1].nice);
	break;
      case 7:
	strcpy(buf + size * k + max0,(!infobuf[k - 1].rw)? "Yes":"No");
	break;
      case 8:
	strcpy(buf + size * k + max0,(!infobuf[k - 1].hindo_rw)? "Yes":"No");
	break;
      case 9:
	strcpy(buf + size * k + max0,(infobuf[k - 1].enablef)? "������":"������");
	break;
      case 10:
	strcpy(buf + size * k + max0,infobuf[k - 1].passwd);
	break;
      case 11:
	strcpy(buf + size * k + max0,infobuf[k - 1].hpasswd);
	break;
      case 12:
	sprintf(buf + size * k + max0,"%3d/%-3d",k,cnt - 1);
	break;
      }
      max1 = max(max1, strlen(buf + size * k));
      if(max1 >= size)return(-1);
    }
    max0 = max1 + 1;
    for(k = 0 ; k < cnt;k++){
	fill(buf + size * k,max0);
    }
  }
  return(1);
}


fill(c,x)
char *c;
int x;
{
  for(;*c;c++,x--);
  for(;x;x--,c++){
    *c = ' ';
  }
}

