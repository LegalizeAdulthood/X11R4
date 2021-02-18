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
/*	Version 4
 */
/*
	Nihongo Henkan Library
*/
/*
	entry functions

	js_open		js_close
	js_change_current_jserver
	js_connect	js_disconnect
	js_isconnect (add kuri)

	js_param_get	js_param_set

	js_access	js_mkdir


extern	Variables
	int	wnn_errorno;
*/


extern	char	*malloc();

#include <stdio.h>
#include <ctype.h>
#ifdef UX386
#include <X11/Xos.h>
#else
#include <fcntl.h>
#endif
#include <pwd.h>
#ifndef UX386
#include <sys/types.h>
#endif
#include <sys/stat.h>
#include <signal.h>	/* ADD KURI */
/* #include <setjmp.h>	/* ADD KURI */
#include "jd_sock.h"
#include "demcom.h"
#include "config.h"

#include "wnnerror.h"
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"

#define JS			/* For include ../etc/bdic.c */
/*  #include "../etc/gethinsi.c" */
#include "../etc/bdic.c"
#include "../etc/pwd.c"

char *malloc();

#ifdef SYSVR2
#define	bzero(adr,n)	memset((adr),0,(n))
/*#define	bcopy(x,y,n)	memcpy((y),(x),(n))*/
#endif

#define	HOST_LEN	20

int	wnn_errorno	=0;

/*	j Lib.	*/

static	int		current_sd;		/** �����å�fd	**/
static	WNN_JSERVER_ID	*current_js;


/*	Packet Buffers		*/
static	unsigned char	snd_buf[S_BUF_SIZ];	/** ���� **/
/* static	unsigned char	rcv_buf[R_BUF_SIZ];	/** ���� **/
static	int	sbp=0;			/** �����Хåե����ݥ��󥿡� **/
static	int	rbc= -1;		/** �����Хåե����ݥ��󥿡� **/


/*********	V4	*****************/
/***
	jserver_dead Macro
***/

static	jmp_buf	current_jserver_dead;

#define	handler_of_jserver_dead(err_val) \
{ \
	if(current_js->js_dead){wnn_errorno=WNN_JSERVER_DEAD;return err_val;}\
	if(setjmp(current_jserver_dead)){ \
		wnn_errorno=WNN_JSERVER_DEAD; \
		return err_val; \
	} \
	wnn_errorno = 0; /* here initialize wnn_errorno; */    \
}


/**	�ǡ���󤬻������Τ���θ����	**/
static
demon_dead()
{
 current_js->js_dead= -1;
 wnn_errorno= WNN_JSERVER_DEAD;
 shutdown(current_sd, 2);
 close(current_sd);
#if DEBUG
	fprintf(stderr,"jslib:JSERVER %s is Dead\n",current_js->js_name);
#endif
 if(current_js->js_dead_env_flg){
	longjmp(current_js->js_dead_env,666);
 }
 longjmp(current_jserver_dead,666);
/* never reach */
}


/**
	�����åȤ򥪡��ץ󤷤�current_sd�˥����å�fd���֤�
			(cd�Ȥ����Τϥ��ߥ�˥��������ǥХ�����̾��)
**/
static
cd_open()
{
#ifdef AF_UNIX
    int sd;
    struct sockaddr saddr;		/** �����å� **/
	saddr.sa_family = AF_UNIX;
	strcpy(saddr.sa_data,sockname);
	if ((sd = socket(AF_UNIX,SOCK_STREAM, 0)) == ERROR) {
#if DEBUG
		xerror("jslib:Can't create socket.\n");
#endif
		return -1;
	}
	if (connect(sd,&saddr,16) == ERROR) {
#if DEBUG
		xerror("jslib:Can't connect socket.\n");
#endif
		return -1;
	}
	return sd;
#else
	return -1;
#endif
}

static
cd_open_in(server, timeout)
register char *server;
register int timeout;
{int sd;
 struct sockaddr_in saddr_in;		/** �����å� **/
 struct servent *sp;
 register struct hostent *hp;
 int serverNO;
 void connect_timeout();	/* ADD KURI */
 int ret;
	serverNO=0;
	if ((sp = getservbyname("wnn4","tcp")) != NULL) {
		serverNO += ntohs(sp->s_port);
	}else{
		serverNO += WNN_PORT_IN;
	}
	if ((hp = gethostbyname(server)) == NULL) { return  -1; }
	bzero((char *)&saddr_in,sizeof(saddr_in));
	bcopy(hp->h_addr,(char *)&saddr_in.sin_addr, hp->h_length);
	saddr_in.sin_family = AF_INET;
	saddr_in.sin_port = htons(serverNO);
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
#if DEBUG
		xerror("jslib:Can't create Inet socket.\n");
#endif
		return -1 ;
	}

	/* ����ʤ��Ȥ��Ƥ����Τ��ʤ��� */
	if (timeout != 0 && timeout > 0) {
	    signal(SIGALRM, connect_timeout); /* ADD KURI */
	    alarm(timeout); /* ADD KURI */
	}
	ret = connect(sd, &saddr_in, sizeof(saddr_in));
	if (timeout != 0 && timeout > 0) {
	    alarm(0); /* ADD KURI */
	    signal(SIGALRM, SIG_IGN); /* ADD KURI */
	}
	if (ret == ERROR) {
#if DEBUG
		xerror("jslib:Can't connect Inet socket.\n");
#endif
		return -1 ;
	}
	return sd;
}

static void
connect_timeout()
{
}


/*	Packet SND/RCV subroutines	*/

/**	�ѥ��åȤΥإå���������	**/
static
snd_head(cmd)
	/** cmd=���ޥ�� **/
{
 sbp=0;
 put4com(cmd);
 rbc= -1;
}

/**	�ѥ��åȤΥإå���������	**/
static
snd_env_head(env,cmd)
register struct wnn_env *env;
	/** cmd=���ޥ�� **/
{
 current_js = (env->js_id);
 current_sd = current_js->sd;
 snd_head(cmd);
 put4com(env->env_id);
 return 0;
}

/**	�ѥ��åȤΥإå���������	**/
static
snd_server_head(server,cmd)
register WNN_JSERVER_ID *server;
	/** cmd=���ޥ�� **/
{
 current_js = server;
 current_sd = current_js->sd;
 snd_head(cmd);
 return 0;
}

/**	�����Хåե���ե�å��夹��	**/
static
snd_flush()
{
 if(sbp==0)return(-1);
 writen(sbp);
 sbp=0;
 return(0);
}

static
writen(n)
{int cc,x;
 for(cc=0;cc<n;){
	x=write(current_sd, &snd_buf[cc],n-cc );
	if(x<=0){ demon_dead();return -1; }
	cc+=x;
 }
#if DEBUG
	fprintf(stderr,"jslib:writen=%d\n",x);
/*	dmp(snd_buf,x); */
#endif
 return 0;
}

/**	�����Ф�1�Х�������	**/
static
put1com(c)
{
 snd_buf[sbp++]=c;
 if(sbp>=S_BUF_SIZ){ 
	writen(S_BUF_SIZ);
	sbp=0;
 }
 return(0);
}

/**	�����Ф�2�Х�������	**/
static
put2com(c)
{
 put1com(c>>(8*1));
 put1com(c       );
}

/**	�����Ф�4�Х�������	**/
static
put4com(c)
{
 put1com(c>>(8*3));
 put1com(c>>(8*2));
 put1com(c>>(8*1));
 put1com(c       );
}

/**	�����Ф�ʸ���������	**/
static
putwscom(p) register wchar *p;
{
 if(p==NULL){ put2com(0); return; }
 while(*p)put2com(*p++);
 put2com(0);
}

/**	�����Ф�ʸ���������	**/
static
putscom(p) register char *p;
{
 if(p==NULL){ put1com(0); return; }
 while(*p)put1com(*p++);
 put1com(0);
}

/**	�����Ф���1�Х��ȼ�����	**/
static
get1com()
{static int rbp;
 static	unsigned char	rcv_buf[R_BUF_SIZ];	/** ���� **/
 if(rbc<=0){
	rbc = read(current_sd, rcv_buf, R_BUF_SIZ);
	if(rbc <= 0){ demon_dead(); return -1; }
	rbp=0;
#if DEBUG
	fprintf(stderr,"jslib:read:rbc=%d\n",rbc);
/*	dmp(rcv_buf,rbc); */
#endif
 }
 rbc--;
 return rcv_buf[rbp++] & 0xFF ;
}

/**	�����Ф���2�Х��ȼ�����	**/
static
get2com()
{register int h;
 h=get1com();
 return (h<<8) | get1com();
}

/**	�����Ф���4�Х��ȼ�����	**/
static
get4com()
{register int h1,h2,h3;
 h1=get1com() << 24 ;
 h2=get1com() << 16 ;
 h3=get1com() <<  8 ;
 return h1 | h2 | h3 | get1com();
}

/**	�����Ф�ʸ���������	**/
static
getscom(p) register char *p;
{
 while(*p++= get1com())
	;
}

/**	�����Ф�ʸ���������	**/
static
getwscom(p) wchar register *p;
{
 while(*p++= get2com())
	;
}

/* Moved from ../etc/string.c */
/**	**/
static
int Strlen(s)
register w_char *s;
{
    register int n;

    for (n = 0;*s++ != 0;n++);
    return n;
}

/**	**/
static 
w_char *Strcpy(s1,s2)
register w_char *s1;
register w_char *s2;
{
	register w_char *d;

	for (d = s1;(*d++ = *s2++) != 0;);
	return s1;
}



/*	Debug Subroutines	*/
#if DEBUG
xerror(s)char *s;
{
 fprintf(stderr,"%s\n",s);
}

dmp(p,c)char *p;
{
 int i,j;
 for(i=0;;i+=16){
	for(j=0;j<16;j++){
	    	if(c<=0){	fprintf(stderr,"\n"); return;}
		fprintf(stderr,"%02x ",p[i+j]&0xFF);
		c--;
	}
	fprintf(stderr,"\n");
 }
}

#endif

/*	get login name form /etc/passwd file	*/
static	char *
getlogname()
{
 struct passwd *getpwuid();
 return getpwuid(getuid())->pw_name;
}




/*
 *		Lib. Functions
 *		raw lib.
 */

/***
	js
	��global
***/

/**	  jserver ����³���롣jserver_id ���֤���	**/
WNN_JSERVER_ID *
js_open(server, timeout)
register char *server;
register int timeout;
{char *new_js;
 char host[256],*user=0;
 int x;

 sbp=0;	/* init sndBufPointer */
 if(!(new_js=(char *)malloc(sizeof(WNN_JSERVER_ID)))){
 				wnn_errorno=WNN_ALLOC_FAIL;return NULL;
 }
 current_js =(WNN_JSERVER_ID *) new_js;
 strcpy(current_js->js_name , server);
 current_js->js_dead= 0;
 current_js->js_dead_env_flg= 0;
/*
 if(user == NULL || 0==strcmp(user,""))
*/
   user= getlogname();
 if(server == NULL || 0==strcmp(server,"") || 0==strcmp(server,"unix")){
   strcpy(host,"unix");
   if((current_sd= cd_open())==-1){wnn_errorno=WNN_SOCK_OPEN_FAIL;return NULL;}
 }else{
   gethostname(host,256);
   if((current_sd= cd_open_in(server, timeout))==-1){
	wnn_errorno=WNN_SOCK_OPEN_FAIL;return NULL;
   }
 }
 current_js->sd= current_sd;
 handler_of_jserver_dead(NULL);
 snd_head(JS_OPEN);
 put4com(JLIB_VERSION);		/* H.T. */
 putscom(host);
 putscom(user);
 snd_flush();
 if(get4com()==-1){
     x = wnn_errorno=get4com();
     js_close(current_js);		/* H.T. */
     wnn_errorno = x;
     return NULL;
 }
 return current_js;
}


/**	�����åȤ򥯥�������	**/
/**	  jserver �Ȥ���³�� close ���롣	**/
int
js_close(server)
WNN_JSERVER_ID *server;
{
 register int x;
 WNN_JSERVER_ID tmp_js_id;
 tmp_js_id = *server;
 free((char *)server);
 current_js = &tmp_js_id;
 current_sd = current_js->sd;
/*	handler of jserver dead */
 handler_of_jserver_dead(-1);
 snd_head(JS_CLOSE);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno=get4com();
 close(current_sd);
 return x;
}


/*
	jserver �Ȥδ֤� connection ��ĥ�ꡢƱ���� jserver �������˴�
	�����롣env_name �˴���¸�ߤ���Ķ�����ꤷ�����ˤϤ��δĶ���
	�֤���NULL ����ꤷ�����ˤϿ������Ķ����ä��֤���
*/
struct wnn_env *
js_connect(server,env_name)
register char *env_name;
WNN_JSERVER_ID *server;
{register int e_id;
 register struct wnn_env *env;
 current_js = server;
 current_sd = current_js->sd;
 if(!(env=(struct wnn_env *)malloc(sizeof(struct wnn_env)))){
 				wnn_errorno=WNN_ALLOC_FAIL;return NULL;
 }
 handler_of_jserver_dead(NULL);
 snd_head(JS_CONNECT);
 putscom(env_name);
 snd_flush();
 e_id=get4com();
 if(e_id==-1){ wnn_errorno= get4com(); return NULL; }
 env->env_id = e_id;
 env->js_id  = server;
 return env;
}

js_env_exist(server,env_name)
register char *env_name;
register WNN_JSERVER_ID *server;
{
 current_js = server;
 current_sd = current_js->sd;
 handler_of_jserver_dead(-1);
 snd_head(JS_ENV_EXIST);
 putscom(env_name);
 snd_flush();
 return(get4com());
}

js_env_sticky(env)
register struct wnn_env *env;
{
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_ENV_STICKY);
 snd_flush();
 return(get4com());
}

js_env_un_sticky(env)
register struct wnn_env *env;
{
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_ENV_UN_STICKY);
 snd_flush();
 return(get4com());
}

/**
	  env �Ǽ������Ķ���̵������
**/
int
js_disconnect(env)
register struct wnn_env *env;
{register int x;
 struct wnn_env tmp_env;
 tmp_env= *env;
 free((char *)env);
 handler_of_jserver_dead(NULL);
 snd_env_head(&tmp_env,JS_DISCONNECT);
 snd_flush();
 x=get4com();
 if(x==-1){ wnn_errorno= get4com(); }
 return x;
}

/**	�����Фȥ��ͥ��Ȥ��Ƥ��뤫	**/
js_isconnect(env)
register struct wnn_env *env;
{
    if (env && env->js_id)
	return(env->js_id->js_dead);
    return(-1);
}

/**
	  env �� �Ķ� �Ȥ��̿��Хåե��� flush ���롣
**/
int
js_flush(env)
struct wnn_env *env;
{
}



/*	Parameter set/get	*/
/**	�Ѵ� parameter �����ꤹ�롣	**/
/**	js_param_set		**/
int
js_param_set(env,para)
struct wnn_env *env;
register struct wnn_param *para;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_PARAM_SET);
 put4com(para->n);	/* ��(��)ʸ����ϤΣ� */
 put4com(para->nsho);	/* ��ʸ����ξ�ʸ��κ���� ADD KURI */
 put4com(para->p1);	/* ��������٤Υѥ�᡼�� */
 put4com(para->p2);	/* ��ʸ��Ĺ�Υѥ�᡼�� */
 put4com(para->p3);	/* ����Ĺ�Υѥ�᡼�� */
 put4com(para->p4);	/* ���Ȥä���ӥåȤΥѥ�᡼�� */
 put4com(para->p5);	/* ����Υѥ�᡼�� */
 put4com(para->p6);	/* ��ʸ���ɾ���ͤΥѥ�᡼�� *//* Change KURI */
 put4com(para->p7);	/* ��ʸ��Ĺ�Υѥ�᡼�� */
 put4com(para->p8);	/* ��ʸ����Υѥ�᡼�� */

 put4com(para->p9);	/* �����ʻ� ���������� */
 put4com(para->p10);	/* �����ʻ� ���ʤ����� */
 put4com(para->p11);	/* �����ʻ� �ѿ������� */
 put4com(para->p12);	/* �����ʻ� ��������� */
 put4com(para->p13);	/* �����ʻ� �ĳ�̤����� */
 put4com(para->p14);	/* �����ʻ� ��°������� */
 put4com(para->p15);	/* �����ʻ� ����̤����� */

 snd_flush();
 x=get4com();
 if(x==-1){ wnn_errorno= get4com(); return -1; }
 return 0;
}

/**	js_param_get		**/
/**	env �Ǽ������Ķ����Ѵ� parameter ����Ф���	**/
int
js_param_get(env,para)
struct wnn_env *env;
register struct wnn_param *para;
{
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_PARAM_GET);
 snd_flush();
 if(get4com() == -1){
     wnn_errorno= get4com(); return -1;
 }
 para->n=get4com();	/* ��(��)ʸ����ϤΣ� */
 para->nsho=get4com();	/* ��ʸ����ξ�ʸ��κ���� ADD KURI */
 para->p1=get4com();	/* ��������٤Υѥ�᡼�� */
 para->p2=get4com();	/* ��ʸ��Ĺ�Υѥ�᡼�� */
 para->p3=get4com();	/* ����Ĺ�Υѥ�᡼�� */
 para->p4=get4com();	/* ���Ȥä���ӥåȤΥѥ�᡼�� */
 para->p5=get4com();	/* ����Υѥ�᡼�� */
 para->p6=get4com();	/* ��ʸ���ɾ���ͤΥѥ�᡼�� *//* Change KURI */
 para->p7=get4com();	/* ��ʸ��Ĺ�Υѥ�᡼�� */
 para->p8=get4com();	/* ��ʸ����Υѥ�᡼�� */
 para->p9=get4com();	/* �����ʻ� ���������� */
 para->p10=get4com();	/* �����ʻ� ���ʤ����� */
 para->p11=get4com();	/* �����ʻ� �ѿ������� */
 para->p12=get4com();	/* �����ʻ� ��������� */
 para->p13=get4com();	/* �����ʻ� �ĳ�̤����� */
 para->p14=get4com();	/* �����ʻ� ��°������� */
 para->p15=get4com();	/* �����ʻ� ����̤����� */

 return 0;
}

/*
	global File Operation
*/
/**	js_mkdir	**/
js_mkdir(env,path)
struct wnn_env *env;
char *path;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_MKDIR);
 putscom(path);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_access	**/
js_access(env,path,amode)
struct wnn_env *env;
char *path;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_ACCESS);
 put4com(amode);
 putscom(path);
 snd_flush();
 x=get4com();
 return x;
}

/**	js_file_list_all	**/
int
js_file_list_all(server,ret)
WNN_JSERVER_ID *server;
struct wnn_ret_buf *ret;
{
 handler_of_jserver_dead(-1);
 snd_server_head(server,JS_FILE_LIST_ALL);
 snd_flush();
 return rcv_file_list(ret);
}


/**	js_file_list	**/
int
js_file_list(env,ret)
struct wnn_env *env;
struct wnn_ret_buf *ret;
{
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FILE_LIST);
 snd_flush();
 return rcv_file_list(ret);
}

static
rcv_file_list(ret)
struct wnn_ret_buf *ret;
{register int i,count;
 WNN_FILE_INFO_STRUCT	*files;
 count=get4com();
 re_alloc(ret,sizeof(WNN_FILE_INFO_STRUCT)*count);
 files=(WNN_FILE_INFO_STRUCT *)ret->buf;
 for(i=0;i<count;i++){
	files->fid= get4com();
	files->localf= get4com();
	files->ref_count= get4com();
	files->type= get4com();
	getscom(files->name);
	files++;
 }
 return count;
}

/**	js_file_stat	**/
js_file_stat(env,path, s)
struct wnn_env *env;
char *path;
WNN_FILE_STAT *s;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FILE_STAT);
 putscom(path);
 snd_flush();
 x=get4com();
 s->type = x;
 return x;
}


/**	js_file_info	**/
js_file_info(env,fid,file)
struct wnn_env *env;
register WNN_FILE_INFO_STRUCT *file;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FILE_INFO);
 put4com(fid);
 snd_flush();
 file->fid= fid;
 x=get4com();
 if(x==-1){
     wnn_errorno= get4com();
     return(-1);
 }
 getscom(file->name);
 file->localf= get4com();
 file->ref_count= get4com();
 file->type= get4com();
 return 0;
}

/**	js_file_loaded	**/
js_file_loaded(server,path)
WNN_JSERVER_ID *server;
char *path;
{register int x;
 handler_of_jserver_dead(-1);
 snd_server_head(server,JS_FILE_LOADED);
 putscom(path);
 snd_flush();
 x=get4com();
 return x;
}

/**	js_file_loaded_local	**/
js_file_loaded_local(server,path)
WNN_JSERVER_ID *server;
char *path;
{int x;
 handler_of_jserver_dead(-1);

 if(check_local_file(path) == -1) return (-1);
 snd_server_head(server, JS_FILE_LOADED_LOCAL);
 x=file_loaded_local(path);
 if(x==-1){ return -1; }
 return x;
}

static
check_local_file(path)
char *path;
{
 register FILE *f;
 register int x;
 struct wnn_file_head fh;

 f=fopen(path,"r");
 if(f == NULL){
     wnn_errorno = WNN_OPENF_ERR;
     return -1;
 }
 x=input_file_header(f, &fh);
 if(x==-1){
     fclose(f);
     wnn_errorno = WNN_NOT_A_FILE;
     return -1;
 }
 if(check_inode(f, &fh) == -1){
     change_file_uniq(&fh, path);
     if(check_inode(f, &fh) == -1){
	 wnn_errorno = WNN_INODE_CHECK_ERROR;
	 return (-1);
     }
 }
 fclose(f);
 return 0;
}


static
file_loaded_local(path)
char *path;
{register int x,i;
 FILE *f;
 struct wnn_file_head fh;

 f=fopen(path,"r");
 if(f == NULL){
     wnn_errorno = WNN_OPENF_ERR;
     return -1;
 }
 x=input_file_header(f, &fh);
 if(x==-1){
     fclose(f);
     wnn_errorno = WNN_NOT_A_FILE;
     return -1;
 }
 put4com(fh.file_uniq.time);
 put4com(fh.file_uniq.dev);
 put4com(fh.file_uniq.inode);
 for(i=0;i<WNN_HOSTLEN;i++){
	put1com(fh.file_uniq.createhost[i]);
 }

 snd_flush();
 x=get4com();
 fclose(f);
 return x;
}


/**	js_hindo_file_create	**/
js_hindo_file_create(env,fid,fn,comment,hpasswd)
struct wnn_env *env;
int	fid;
char *fn;
wchar *comment;
char *hpasswd;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_HINDO_FILE_CREATE);
 put4com(fid);
 putscom(fn);
 putwscom(comment);
 putscom(hpasswd);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_dic_file_create	**/
js_dic_file_create(env,fn,type,comment,passwd, hpasswd)
struct wnn_env *env;
char *fn;
wchar *comment;
char *passwd, *hpasswd;
int type;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_DIC_FILE_CREATE);
 putscom(fn);
 putwscom(comment);
 putscom(passwd);
 putscom(hpasswd);
 put4com(type);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}


/**	js_file_discard	**/
js_file_discard(env,fid)
struct wnn_env *env;
int	fid;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FILE_DISCARD);
 put4com(fid);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_file_read	**/
int
js_file_read(env,fn)
struct wnn_env *env;
char	*fn;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FILE_READ);
 putscom(fn);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_file_write	**/
int
js_file_write(env,fid,fn)
struct wnn_env *env;
int	fid;
char	*fn;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FILE_WRITE);
 put4com(fid);
 putscom(fn);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_file_receive	**/
int
js_file_receive(env,fid,fn)
struct wnn_env *env;
int	fid;
char	*fn;
{register int mode, x;
 char file_name[1024];
 char buf[1024];
 FILE *f;
 int n;
 struct wnn_file_head fh;
 int i;

 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FILE_RECEIVE);
 put4com(fid);
 snd_flush();
/**/
 getscom(file_name);
 if(fn ==NULL || strcmp(fn,"")==0){
     gethostname(buf, 1024);
     n = strlen(buf);
     buf[n] = C_LOCAL;
     buf[n+1] = 0;
     if(strncmp(buf, file_name, n + 1) == 0){
	 fn = file_name + n + 1;
     }
 }
 if((f = fopen(fn, "r")) == NULL){ /* New File */
     fh.file_uniq.time = fh.file_uniq.dev = fh.file_uniq.inode = 0;
 }else{			/* Old File Exists */
     if(input_file_header(f, &fh) == -1){
	 wnn_errorno=WNN_NOT_A_FILE;
	 fclose(f);
	 put4com(-1);snd_flush();
	 return(-1);
	}
     fclose(f);
 }
 put4com(0);			/* Ack */
 put4com(fh.file_uniq.time);
 put4com(fh.file_uniq.dev);
 put4com(fh.file_uniq.inode);
 for(i=0;i<WNN_HOSTLEN;i++){
     put1com(fh.file_uniq.createhost[i]);
 }

 snd_flush();

 if((mode=get4com())==-1){ /* check stat */
	wnn_errorno= get4com();
	return -1;
 }else if(mode==0){
	return 0; /* need not saving */
 }else if(mode == 1 || mode == 3){ /* mode == 3 means the file is a new one. */
     if((f = fopen(fn, "w+")) == NULL){ 
	 wnn_errorno=WNN_FILE_WRITE_ERROR;
	 put4com(-1);snd_flush();
	 return(-1);
     }
 }else if(mode == 2){
     if((f = fopen(fn, "r+")) == NULL){ /* New File */
	 wnn_errorno=WNN_FILE_WRITE_ERROR;
	 put4com(-1);snd_flush();
	 return(-1);
     }
 }
 put4com(0); snd_flush(); /* ACK */
 for(;;){
	if((x=xget1com())== -1) break; /* EOF */
	fputc(x,f);
 }
 fclose(f);

 x=get4com();
 if(x==-1)wnn_errorno= get4com();

 return x;
}

static
xget1com()
{register int x;
 if((x= get1com()) != 0xFF) return x;
 if(get1com() == 0xFF) return -1; /* EOF */
 return 0xFF;
}

/**	js_file_send	**/
int
js_file_send(env,fn)
struct wnn_env *env;
char	*fn;
{register int x;
 FILE *f;
 int n;
 unsigned char buf[1024],*b;
 register int cc,i;
 handler_of_jserver_dead(-1);

 if(check_local_file(fn) == -1) return (-1);

 snd_env_head(env,JS_FILE_SEND);
 x=file_loaded_local(fn);
 if(x!= -1){ /* file is already loaded */
     if(get4com() == -1) {
	 wnn_errorno= get4com();
	 return(-1);
     }
     return x;
 }

 x=get4com();
 if(x==-1){
	wnn_errorno= get4com();
	return -1;
 }

 gethostname(buf, 1024);
 n = strlen(buf);
 buf[n] = C_LOCAL;
 strcpy(buf + n + 1,fn);
 putscom(buf);

 if((f=fopen(fn,"r"))== NULL){
	xput1com(-1); /* EOF */
	return -1;
 }

 /* send contents of file */
 for(;;){
	cc = fread(buf,1,1024,f);
	if(cc <= 0) break; /* EOF */
	for(b=buf,i=0;i<cc;i++){
		xput1com(*b++);
	}
 }
 fclose(f);
 xput1com(-1); /* EOF */
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

static
xput1com(d)
{
 if(d == -1){ put1com(0xFF); put1com(0xFF);return;/* EOF */}
 put1com(d);
 if(d == 0xFF){ put1com(0x00);}
}


/***	Dic. Operation for Env.	 ***/

/**	js_dic_add	**/
int
js_dic_add(env,fid,hfid,rev, jnice,rw,hrw, pw1, pw2)
struct wnn_env *env;
int	fid,hfid,jnice,rw, rev;
char *pw1, *pw2;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_DIC_ADD);
 put4com(fid);
 put4com(hfid);
 put4com(jnice);
 put4com(rw);
 put4com(hrw);
 putscom(pw1);
 putscom(pw2);
 put4com(rev);		/* rev is to add it as reverse dict */
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_dic_delete	**/
int
js_dic_delete(env,dicno)
struct wnn_env *env;
int	dicno;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_DIC_DELETE);
 put4com(dicno);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_dic_use	**/
int
js_dic_use(env,dic_no,flag)
struct wnn_env *env;
int	dic_no,flag;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_DIC_USE);
 put4com(dic_no);
 put4com(flag);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_fuzokugo_set	**/
int
js_fuzokugo_set(env,fid)
struct wnn_env *env;
int	fid;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FUZOKUGO_SET);
 put4com(fid);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_fuzokugo_get	**/
int
js_fuzokugo_get(env)
struct wnn_env *env;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FUZOKUGO_GET);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_dic_list_all	**/
int
js_dic_list_all(server,ret)
WNN_JSERVER_ID *server;
struct wnn_ret_buf *ret;
{
 handler_of_jserver_dead(-1);
 snd_server_head(server,JS_DIC_LIST_ALL);
 snd_flush();
 return rcv_dic_list(ret);
}


/**	js_dic_list	**/
int
js_dic_list(env,ret)
struct wnn_env *env;
struct wnn_ret_buf *ret;
{
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_DIC_LIST);
 snd_flush();
 return rcv_dic_list(ret);
}

static
rcv_dic_list(ret)
struct wnn_ret_buf *ret;
{register int i,count;
 register WNN_DIC_INFO *dic;
 count=get4com();
/* 
 re_alloc(ret, sizeof(WNN_DIC_INFO)*count);  tsuiki 88/11/30
*/
 re_alloc(ret, sizeof(WNN_DIC_INFO)*(count + 1));

 dic=(WNN_DIC_INFO *)ret->buf;
 for(i=0;i<count;i++){
     get_dic_info(dic);
     dic++;
 }
 dic->dic_no = -1;  /* tsuiki 88/11/30 */
 return count;
}

static
get_dic_info(dic)
register WNN_DIC_INFO *dic;
{
	dic->dic_no =get4com();	/* dic_No */
	dic->body =get4com();	/* body fid */
	dic->hindo =get4com();	/* hindo fid */
	dic->rw =get4com();	/* r/w */
	dic->hindo_rw =get4com();	/* hindo r/w */
	dic->enablef =get4com();	/* enable/disable */
	dic->nice =get4com();	/* nice */
	dic->rev = get4com();
/* added H.T */
	getwscom(dic->comment);
	getscom(dic->fname);
	getscom(dic->hfname);
	getscom(dic->passwd);
	getscom(dic->hpasswd);
	dic->type = get4com();
	dic->gosuu = get4com();
	dic->localf = get4com();
	dic->hlocalf = get4com();
}

/***	Dic. Operation by dic_No.	***/

/**	js_word_add		**/
int
js_word_add(env,dic_no,yomi,kanji,comment,hinshi,init_hindo)
struct wnn_env *env;
int	dic_no;
wchar	*yomi,*kanji,*comment;
int	hinshi,init_hindo;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_WORD_ADD);
 put4com(dic_no);
 putwscom(yomi);
 putwscom(kanji);
 putwscom(comment);
 put4com(hinshi);
 put4com(init_hindo);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}

/**	js_word_delete		**/
int
js_word_delete(env,dic_no,entry)
struct wnn_env *env;
int	dic_no;
int	entry;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_WORD_DELETE);
 put4com(dic_no);
 put4com(entry);
 snd_flush();
 x=get4com();
 if(x==-1)wnn_errorno= get4com();
 return x;
}


/**	js_word_search		**/
int
js_word_search(env,dic_no,yomi,ret)
struct wnn_env *env;
int	dic_no;
wchar	*yomi;
struct wnn_ret_buf *ret;
{

 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_WORD_SEARCH);
 put4com(dic_no);
 putwscom(yomi);
 snd_flush();

 return (rcv_word_data(ret, yomi));
}

/**	js_word_search_by_env	**/
int
js_word_search_by_env(env,yomi,ret)
struct wnn_env *env;
wchar	*yomi;
struct wnn_ret_buf *ret;
{
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_WORD_SEARCH_BY_ENV);
 putwscom(yomi);
 snd_flush();

 return (rcv_word_data(ret, yomi));
}

/**	js_word_info		**/
int
js_word_info(env,dic_no,entry,ret)
struct wnn_env *env;
int	dic_no,entry;
struct wnn_ret_buf *ret;
{register int x;
 wchar yomi[LENGTHYOMI];

 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_WORD_INFO);
 put4com(dic_no);
 put4com(entry);
 snd_flush();
 x=get4com();
 if(x==-1){
     wnn_errorno= get4com();
     return(-1);
 }
 getwscom(yomi);
 rcv_word_data(ret, yomi);
 return(0);
}

js_word_comment_set(env, dic_no, entry, comment)
struct wnn_env *env;
int	dic_no,entry;
wchar *comment;
{
 register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_WORD_COMMENT_SET);
 put4com(dic_no);
 put4com(entry);
 putwscom(comment);
 snd_flush();
 x=get4com();
 if(x==-1){
     wnn_errorno= get4com();
     return(-1);
 }
 return(0);	/* ADD KURI */
}    

/**	rcv for word_search		**/
static
rcv_word_data(ret, yomi)
struct wnn_ret_buf *ret;
wchar *yomi;			/* Yomi is not sent from server
				 *  (at least for the time being).
				 */
{register int x, j_c,k_c;
 wchar *k;
 register struct wnn_jdata *jd;
 register int cnt;

 j_c = get4com();
 k_c = get4com();
 re_alloc(ret, sizeof(struct wnn_jdata)*(j_c + 1) + sizeof(wchar)*(k_c + j_c * 3 + j_c * Strlen(yomi)));
 jd=(struct wnn_jdata *)ret->buf;
 for(cnt = 0;;cnt++){
	jd->dic_no = x = get4com();
	if(x==-1) break;
	jd->serial  = get4com();
	jd->hinshi = get4com();
	jd->hindo = get4com();	jd->ima = get4com();
	jd->int_hindo = get4com();	jd->int_ima = get4com();
	jd++;
 }
 jd++;
 k= (wchar *)jd;
 jd=(struct wnn_jdata *)ret->buf;
 for(;;){
	if(jd->dic_no==-1) break;

	jd->yomi = k;		/* Copy Yomi */
	Strcpy(k, yomi);
	k+= Strlen(k)+1;

	jd->kanji = k;		/* Get Kanji */
	getwscom(k);
	k+= Strlen(k)+1;

	jd->com = k;		/* Get Comment */
	getwscom(k);
	k+= Strlen(k)+1;
	jd++;
 }
 return cnt;
}


/**	js_dic_info	**/
int
js_dic_info(env,dic_no,ret)
struct wnn_env *env;
int	dic_no;
register WNN_DIC_INFO *ret;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_DIC_INFO);
 put4com(dic_no);
 snd_flush();
 x=get4com();
 if(x==-1){ return get4com();}
 get_dic_info(ret);
 return dic_no;
}


/**	js_who		**/
js_who(server,ret)
WNN_JSERVER_ID *server;
struct wnn_ret_buf *ret;
{register int i,j,c;
 WNN_JWHO *w;
 handler_of_jserver_dead(-1);
 snd_server_head(server,JS_WHO);
 snd_flush();

 c=get4com();
 if(c==-1){ wnn_errorno= get4com();return -1;}

 re_alloc(ret, sizeof(WNN_JWHO)*c);
 w=(WNN_JWHO *)ret->buf;
 for(i=0;i<c;i++){
	w->sd=get4com();
	getscom(w->user_name);
	getscom(w->host_name);
	for(j=0;j<WNN_MAX_ENV_OF_A_CLIENT;j++){
		(w->env)[j]=get4com();
	}
	w++;
 }
 return(c);
}

/**	jserver ������ƤδĶ��˴ؤ����������롣
	(���鵻)
**/
int
js_env_list(server,ret)
WNN_JSERVER_ID *server;
struct wnn_ret_buf *ret;
{register int i,j,c;
 WNN_ENV_INFO *w;
 handler_of_jserver_dead(-1);
 snd_server_head(server,JS_ENV_LIST);
 snd_flush();

 c=get4com();
 if(c==-1){ wnn_errorno= get4com();return -1;}

 re_alloc(ret, sizeof(WNN_ENV_INFO)*c);
 w=(WNN_ENV_INFO *)ret->buf;
 for(i=0;i<c;i++){
	w->env_id = get4com();     
	getscom(w->env_name);
	w->ref_count=get4com();
	w->fzk_fid=get4com();
	w->jishomax=get4com();
	for(j=0;j<WNN_MAX_JISHO_OF_AN_ENV;j++){
		(w->jisho)[j]= get4com();
	}
	for(j=0;j<WNN_MAX_FILE_OF_AN_ENV;j++){
		(w->file)[j]= get4com();
	}
	w++;
 }
 return(c);
}

/****

****/
js_hindo_set(env,dic,entry,ima,hindo)
struct wnn_env *env;
{register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_HINDO_SET);

 put4com(dic);
 put4com(entry);
 put4com(ima);
 put4com(hindo);
 snd_flush();
 if((x=get4com())==-1){
	wnn_errorno= get4com();return -1;
 }
 return x;
}


/****
	Henkan
****/

static
put_fzk_vec(hinsi,fzk,vec,vec1)
int hinsi;
wchar *fzk;
int vec;
int vec1;
{
	put4com(hinsi);
	putwscom(fzk);
	put4com(vec);
	put4com(vec1);
}

/**
	kanren
**/
js_kanren(env,yomi,hinsi,fzk,vec,vec1,vec2,rb)
struct wnn_env *env;
wchar	*yomi;
int	hinsi;	/* ADD KURI */
wchar	*fzk;
int	vec;
int	vec1;
int	vec2;
struct wnn_ret_buf *rb;
{
 handler_of_jserver_dead(-1);

 snd_env_head(env,JS_KANREN);
 putwscom(yomi);
 put_fzk_vec(hinsi,fzk,vec,vec1);
 put4com(vec2);
 snd_flush();
 return rcv_dai(rb);
}

/*	rcv dai		*/
static
rcv_dai(ret)
struct wnn_ret_buf *ret;
{int dai_cnt,sho_sum,kanji_sum,d_size,s_size,k_size,x;
 register int i;
 struct wnn_dai_bunsetsu *dai_list;
 register struct wnn_dai_bunsetsu *dp;
 struct wnn_sho_bunsetsu *sho_list;
 register struct wnn_sho_bunsetsu *sp;
 wchar *kanji,*kp;

 dai_cnt = get4com();
 if(dai_cnt == -1){	/* error dayo */
	wnn_errorno = get4com();
	return -1;
 }
 sho_sum = get4com();
 kanji_sum = get4com();

 d_size=sizeof(struct wnn_dai_bunsetsu)*dai_cnt;
 s_size=sizeof(struct wnn_sho_bunsetsu)*sho_sum;
 k_size=sizeof(wchar)*kanji_sum;

/* re_alloc(ret, d_size+s_size+k_size); Seems This cause Bug?? H.T.*/
 re_alloc(ret, d_size+s_size+k_size); 

 dai_list = ( struct wnn_dai_bunsetsu *) ret->buf;
 sho_list = ( struct wnn_sho_bunsetsu *)((char *)ret->buf + d_size);
 kanji = (wchar *)((char *)ret->buf + d_size + s_size);

 for(dp = dai_list,i=0; i<dai_cnt; i++){
	dp -> end = get4com();
	dp -> start = get4com();
	dp -> sbncnt = get4com();
	dp -> hyoka = get4com();
	dp++;
 }

 for(dp = dai_list, sp = sho_list, i=0; i<dai_cnt; i++){
	dp -> sbn = sp;
	x = dp -> sbncnt;
	rcv_sho_x(sp,x);
	sp += x;
	dp++;
 }

 for(dp=dai_list, kp=kanji, i=0; i<dai_cnt; i++){
	rcv_sho_kanji(dp -> sbn,dp -> sbncnt,&kp);
	dp++;
 }
 return dai_cnt;
}

/*	rcv sho routines	*/
static
rcv_sho_x(sho_list,sho_cnt)
register struct wnn_sho_bunsetsu *sho_list;
{register int i;
 for(i=0;i<sho_cnt;i++){
	sho_list -> end = get4com();
	sho_list -> start = get4com();
	sho_list -> jiriend = get4com();
	sho_list -> dic_no = get4com();
	sho_list -> entry = get4com();
	sho_list -> hindo = get4com();
	sho_list -> ima = get4com();
	sho_list -> hinsi = get4com();
	sho_list -> status = get4com();
	sho_list -> status_bkwd = get4com();	/* ADD KURI */
	sho_list ->kangovect = get4com();
	sho_list -> hyoka = get4com();
	sho_list++;
 }
}

static
rcv_sho_kanji(sho_list,sho_cnt,kanji)
struct wnn_sho_bunsetsu *sho_list;
int sho_cnt;
wchar **kanji;
{
 register wchar *k;
 register int i, x;
 k = *kanji;
 for(i=0;i<sho_cnt;i++){
	sho_list -> kanji = k;
	getwscom(k);
	x= Strlen(k);
	k += x+1;

	sho_list -> yomi = k;
	getwscom(k);
	x= Strlen(k);
	k += x+1;

	sho_list -> fuzoku = k;
	getwscom(k);
	x= Strlen(k);
	k += x+1;
	sho_list++;
 }
 *kanji = k;
}


static
rcv_sho(ret)
struct wnn_ret_buf *ret;
{register int sho_sum,kanji_sum,s_size,k_size;
 struct wnn_sho_bunsetsu *sho_list;
 wchar *kanji,*kp;

 sho_sum = get4com();
 if(sho_sum == -1){	/* error dayo */
	wnn_errorno = get4com();
	return -1;
 }
 kanji_sum = get4com();

 s_size=sizeof(struct wnn_sho_bunsetsu)*sho_sum;
 k_size=sizeof(wchar)*kanji_sum;

 re_alloc(ret, s_size+k_size);

 sho_list = ( struct wnn_sho_bunsetsu *)((char *)ret->buf);
 kanji = (wchar *)((char *)ret->buf + s_size);

 rcv_sho_x(sho_list,sho_sum);
 kp=kanji;
 rcv_sho_kanji(sho_list,sho_sum,&kp);
 return sho_sum;
}

/**
	kantan
**/
js_kantan_dai(env,yomi,hinsi,fzk,vec,vec1,rb)
struct wnn_env *env;
wchar	*yomi;
int	hinsi;	/* ADD KURI */
wchar	*fzk;
int	vec;
int	vec1;
struct wnn_ret_buf *rb;
{
 handler_of_jserver_dead(-1);

 snd_env_head(env,JS_KANTAN_DAI);
 putwscom(yomi);
 put_fzk_vec(hinsi,fzk,vec,vec1);
 snd_flush();
 
 return rcv_dai(rb);
}

js_kantan_sho(env,yomi,hinsi,fzk,vec,vec1,rb)
struct wnn_env *env;
wchar	*yomi;
int	hinsi;	/* ADD KURI */
wchar	*fzk;
int	vec;
int	vec1;
struct wnn_ret_buf *rb;
{int sbncnt;
 handler_of_jserver_dead(-1);

 snd_env_head(env,JS_KANTAN_SHO);
 putwscom(yomi);
 put_fzk_vec(hinsi,fzk,vec,vec1);
 snd_flush();
 
 sbncnt = rcv_sho(rb);
 return sbncnt;
}

/**
	kanzen
**/
js_kanzen_dai(env,yomi,hinsi,fzk,vec,vec1,rb)
struct wnn_env *env;
wchar	*yomi;
int	hinsi;	/* ADD KURI */
wchar	*fzk;
int	vec;
int	vec1;
struct wnn_ret_buf *rb;
{
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_KANZEN_DAI);
 putwscom(yomi);
 put_fzk_vec(hinsi,fzk,vec,vec1);
 snd_flush();

 return rcv_dai(rb);
}

js_kanzen_sho(env,yomi,hinsi,fzk,vec,vec1,rb)
struct wnn_env *env;
wchar	*yomi;
int	hinsi;	/* ADD KURI */
wchar	*fzk;
int	vec;
int	vec1;
struct wnn_ret_buf *rb;
{int sbncnt;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_KANZEN_SHO);
 putwscom(yomi);
 put_fzk_vec(hinsi,fzk,vec,vec1);
 snd_flush();

 sbncnt = rcv_sho(rb);
 return sbncnt;
}

/**	js_version		**/
js_version(server,serv,libv)
int *serv,*libv;
WNN_JSERVER_ID *server;
{
 handler_of_jserver_dead(-1);
 snd_server_head(server, JS_VERSION);
 snd_flush();
 *libv= JLIB_VERSION;
 return *serv = get4com();
}

static
re_alloc(ret,size)
register struct wnn_ret_buf *ret;
{
 if(ret->size < size){
	if(ret->buf)	/* ADD KURI NULL nara free sinai */
	    free((char *)ret->buf);
	ret->buf = malloc(size);
	ret->size = size;
 }
}


js_kill(server)
WNN_JSERVER_ID *server;
{
    int x;
    handler_of_jserver_dead(-1);
    snd_server_head(server, JS_KILL);
    snd_flush();
    x = get4com();
    return(x);
}


js_file_remove(server, n, pwd)
WNN_JSERVER_ID *server;
char *n, *pwd;
{
    register int x;
    handler_of_jserver_dead(-1);
    snd_server_head(server,JS_FILE_REMOVE);
    putscom(n);
    putscom(pwd);
    snd_flush();
    if((x=get4com())==-1){
	wnn_errorno= get4com();return -1;
    }
    return(x);
}    

js_file_remove_client(server, n, pwd)
WNN_JSERVER_ID *server;
char *n, *pwd;
{
    struct wnn_file_head fh;
    register FILE *fp;
    handler_of_jserver_dead(-1);
    if(js_file_loaded_local(server, n) != -1){
	wnn_errorno = WNN_FILE_IN_USE;
	return(-1);
    }
    if((fp=fopen(n,"r")) == NULL){
	wnn_errorno = WNN_FILE_READ_ERROR;
	return(-1);
    }
    if(input_file_header(fp, &fh) == -1){
	fclose(fp);
	return(-1);
    }
    fclose(fp);
    if(!check_pwd(pwd, fh.file_passwd)){
	wnn_errorno = WNN_INCORRECT_PASSWD;
	return(-1);
    }
    if(unlink(n) == -1){
	wnn_errorno = WNN_UNLINK;
	return(-1);
    }
    return(0);
}

/**	js_dic_file_create_client	**/
js_dic_file_create_client(env,fn,type,com,passwd, hpasswd)
struct wnn_env *env;
int type;
char *fn;
wchar *com;
char *passwd, *hpasswd;
{
    int x;
    if(type != WNN_REV_DICT && type != WNN_UD_DICT){
	wnn_errorno = WNN_NOT_A_UD;
	return(-1);
    }
    x = create_null_dic(fn,com, passwd, hpasswd, type);
    if(x == -1){
	wnn_errorno = WNN_FILE_CREATE_ERROR;
	return(-1);
    }
    return(0);
}


/**	js_hindo_file_create_client	**/
js_hindo_file_create_client(env,fid,fn,com,hpasswd)
struct wnn_env *env;
int	fid;
char *fn;
wchar *com;
char *hpasswd;
{
    register int x;
    struct wnn_file_uniq funiq;
    int serial;
    register int i;

    handler_of_jserver_dead(-1);
    snd_env_head(env,JS_HINDO_FILE_CREATE_CLIENT);
    put4com(fid);
    snd_flush();
    x = get4com();
    if(x == -1){
	wnn_errorno = get4com();
	return(-1);
    }
    serial = get4com();
    funiq.time = get4com();
    funiq.dev =  get4com();
    funiq.inode = get4com();
    for(i=0;i<WNN_HOSTLEN;i++){
	funiq.createhost[i]=get1com();
    }
/*    getscom(funiq.createhost); */
    if(create_hindo_file(&funiq,fn,com,hpasswd,serial) == -1){
	wnn_errorno = WNN_FILE_CREATE_ERROR;
	return(-1);
    }
    return(0);
}

js_file_comment_set(env, fid, comment)
struct wnn_env *env;
int fid;
wchar *comment;
{
 register int x;
 handler_of_jserver_dead(-1);
 snd_env_head(env,JS_FILE_COMMENT_SET);
 put4com(fid);
 putwscom(comment);
 snd_flush();
 x=get4com();
 if(x==-1){
     wnn_errorno= get4com();
     return(-1);
 }
 return(0);	/* ADD KURI */
}
    

/* 
 * Hinsi Primitives
 */

int
js_hinsi_name(server, no, rb)
WNN_JSERVER_ID *server;
int no;
struct wnn_ret_buf *rb;
{
    register int size;

    handler_of_jserver_dead(-1);
    snd_server_head(server,JS_HINSI_NAME);
    put4com(no);
    snd_flush();
    if((size = get4com()) == -1){
	wnn_errorno = get4com(); return(-1);
    }
    re_alloc(rb,sizeof(wchar)*(size + 1));
    getwscom((wchar *)rb->buf);
    return(0);
}


int 
js_hinsi_number(server, name)
WNN_JSERVER_ID *server;
wchar *name;
{
    register int no;

    handler_of_jserver_dead(-1);
    snd_server_head(server,JS_HINSI_NUMBER);
    putwscom(name);
    snd_flush();
    if((no = get4com()) == -1){
	wnn_errorno = get4com();
	return(-1);
    }
    return(no);
}


int
js_hinsi_list(env, dic_no, name, rb)
struct wnn_env *env;
int dic_no;
wchar *name;
struct wnn_ret_buf *rb;
{
    int size;
    int count;
    register wchar *s;
    register wchar **r;
    register int k;

    handler_of_jserver_dead(-1);
    snd_env_head(env,JS_HINSI_LIST);
    put4com(dic_no);
    putwscom(name);
    snd_flush();
    if((count = get4com()) == -1){
	wnn_errorno = get4com();
	return(-1);
    }
    size = get4com();
    re_alloc(rb,sizeof(wchar)*(size + 1) + count * sizeof(wchar *) );
    r = (wchar **)rb->buf;
    s = (wchar *)((wchar **)rb->buf + count);
    for(k = 0 ; k < count ; k++){
	*r++ = s;
	getwscom(s);
	s+= Strlen(s) + 1;
    }
    return(count);
}


int 
js_hinsi_dicts(env, no,  rb)
struct wnn_env *env;
int no;
struct wnn_ret_buf *rb;
{
    register int k, count;
    int *p;

    handler_of_jserver_dead(-1);
    snd_env_head(env,JS_HINSI_DICTS);
    put4com(no);
    snd_flush();
    if((count = get4com()) == -1){
	wnn_errorno = get4com();
	return(-1);
    }
    re_alloc(rb,sizeof(int) * (count + 1));
    p = (int *)rb->buf;

    for(k = 0 ; k < count ; k++){
	*p++ = get4com();
    }
    return(count);
}


char *wnn_dic_types[] = {"","����","��Ͽ","�հ�","����"};


#ifdef nodef       Not supported
js_hinsi_put(env, dic_no, name)
struct wnn_env *env;
int dic_no;
wchar *name
{
}
#endif

/* New primitives  9/8 */

js_file_password_set(env, fid, which, old, new)
struct wnn_env *env;
int fid;
int which;			/* 1 for dic, 2 for hindo 3(0) for both*/
char *old, *new;
{
    register int x;
    handler_of_jserver_dead(-1);
    snd_env_head(env,JS_FILE_PASSWORD_SET);
    put4com(fid);
    put4com(which);
    putscom(old);
    putscom(new);
    snd_flush();
    x=get4com();
    if(x==-1){
	wnn_errorno= get4com();
	return(-1);
    }
    return(0);
}

js_hinsi_table_set(env, dic_no, hinsi_table)
struct wnn_env *env;
int dic_no;
wchar *hinsi_table;
{
    int x;
    handler_of_jserver_dead(-1);
    snd_env_head(env,JS_HINSI_TABLE_SET);
    put4com(dic_no);
    putwscom(hinsi_table);
    snd_flush();
    x=get4com();
    if(x==-1){
	wnn_errorno= get4com();
	return(-1);
    }
    return(0);
}
