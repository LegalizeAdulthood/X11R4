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
#include <ctype.h>
#include "commonhd.h"
#include "ddefine.h"
#include "jservercf.h"
#include "de_header.h"
#include "wnn_malloc.h"

struct wnn_param default_para = {
   2, 10,
/* £Î ¾®Ê¸Àá¿ô */
   2,   34,      5,     80,        5,   1,         20,      0,
/* ÉÑÅÙ ¾®Ê¸ÀáÄ¹ ´´¸ìÄ¹ º£»È¤Ã¤¿¤è ¼­½ñ ¾®Ê¸ÀáÉ¾²Á ÂçÊ¸ÀáÄ¹ ¾®Ê¸Àá¿ô
   400, -100,400, 80,  200,   2,     200
/* ¿ô»ú ¥«¥Ê ±Ñ¿ô µ­¹æ ÊÄ³ç¸Ì ÉÕÂ°¸ì ³«³ç¸Ì */
};

/* daemon initialize routin */
read_default()
{
    FILE *fp, *fopen();
    char data[EXPAND_PATH_LENGTH];
    char code[EXPAND_PATH_LENGTH];
    char s[20][EXPAND_PATH_LENGTH];

    strcpy(jserver_dir,JSERVER_DIR);

    if((fp = fopen(jserverrcfile , "r")) == NULL){
	perror("");
	printf("Error can't open %s\n" , jserverrcfile);
	return(-1);
    }
    while (fgets(data, 1024, fp) != NULL) {
	sscanf(data,
	  "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",code,
	       s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[8],s[9],s[10],
	       s[11],s[12],s[13],s[14],s[15],s[16],s[17],s[18],s[19]) ;

/*
	if(strcmp(code, "jt_len") == 0){
	    jt_len = atoi(s[0]);
	}else if(strcmp(code, "hjt_len") == 0){
	    hjt_len = atoi(s[0]);
	}
*/
	if(strcmp(code, "max_client") == 0){
		sscanf(data,"%s %d ",code,&max_client);
		error1("max_client=%d\n",max_client);
	}else if(strcmp(code, "max_sticky_env") == 0){
		sscanf(data,"%s %d ",code,&max_sticky_env);
		error1("max_client=%d\n",max_client);
	}else if(strcmp(code , "jserver_dir") == 0){
	    strcpy(jserver_dir,s[0]);
	}else if(strcmp(code, "def_param") == 0){
	    change_ascii_to_int(s[0], &default_para.n);
	    change_ascii_to_int(s[1], &default_para.nsho);
	    change_ascii_to_int(s[2], &default_para.p1);
	    change_ascii_to_int(s[3], &default_para.p2);
	    change_ascii_to_int(s[4], &default_para.p3);
	    change_ascii_to_int(s[5], &default_para.p4);
	    change_ascii_to_int(s[6], &default_para.p5);
	    change_ascii_to_int(s[7], &default_para.p6);
	    change_ascii_to_int(s[8], &default_para.p7);
	    change_ascii_to_int(s[9], &default_para.p8);
	    change_ascii_to_int(s[10], &default_para.p9);
	
	    change_ascii_to_int(s[11], &default_para.p10);
	    change_ascii_to_int(s[12], &default_para.p11);
	    change_ascii_to_int(s[13], &default_para.p12);
	    change_ascii_to_int(s[14], &default_para.p13);
	    change_ascii_to_int(s[15], &default_para.p14);
	    change_ascii_to_int(s[16], &default_para.p15);
	}
    }
    fclose(fp);
    return(0);
}

read_default_files()
{
    FILE *fp, *fopen();
    char data[256];
    int num ;
    char code[256];
    char file[256];

    if((fp = fopen(jserverrcfile , "r")) == NULL){
	perror("");
	printf("Error can't open %s\n" , jserverrcfile);
	return(-1);
    }
    while(fgets(data,256,fp  ) != NULL){
	num = sscanf(data,"%s %s",code,file ) ;
	if (strcmp(code, "readfile") == 0 && num == 2){
	    read_default_file(file);
	}
    }
    fclose(fp);

    printf("Finished Reading Files\n");
    
    return(0);
}

/*
dummy_env()
{
    int i = 0;
    struct cnv_env *ne;

    if((ne= (struct cnv_env *)malloc(sizeof(struct cnv_env)))==0) return -1;
    env[i]=ne; new_env(i,"dummy_env");
}
*/

read_default_file(n)
char *n;
{
    int fid;

    expand_file_name(n);
    fid = file_loaded(n);
    if(fid == -1){  /* Not correct file */
	printf("Error reading %s\n", n);
	return(-1);
    }
    if(FILE_NOT_READ != fid){ /* already read */
	return(-1);
    }
    if((fid= get_new_fid()) == -1){ /* no more file */
	printf("Error reading %s\n", n);
	return(-1);
    }

    files[fid].localf= LOCAL;
    strcpy(files[fid].name, n);
    printf("Reading %s\t Fid = %d\n", n, fid);
    if(read_file(&files[fid])==-1){
	printf("Error reading %s\n", n);
	files[fid].ref_count= -1;	/* fail */
	return(-1);
    }
    files[fid].ref_count= 1;
    return(fid);
}

change_ascii_to_int(st,dp)
char *st;
int *dp;
{
    register int total,flag;

    total = 0;
    flag = 0;
    while(*st != NULL){
	if (isdigit(*st)){
	    total = total * 10 + (*st - '0');
	} else if (*st == '+') {
	    if (flag != 0) { return(-1); }
	    flag = 1;
	} else if (*st == '-') {
	    if (flag != 0) { return(-1); }
	    flag = -1;
	} else { return(-1); }
	st++;
    }
    if (flag == 0){
	flag = 1;
    }
    *dp = total * flag;
    return(1);
}
