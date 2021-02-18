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
	Jserver		(Nihongo Demon)
*/
#include <stdio.h>
#include "jd_sock.h"
#include "demcom.h"
#include "jservercf.h"
#include "commonhd.h"
#include "wnn_malloc.h"

#include <ctype.h>

#ifdef BSD42
#include <sys/file.h>
#else
#ifndef UX386
#include <fcntl.h>
#endif
#endif
#include <signal.h>
#include <sys/ioctl.h>

#ifdef SYSVR2
#include <sys/param.h>
#define	SIGCHLD	SIGCLD
#endif

#include "de_header.h"
#ifdef UX386
#include <sys/termio.h>
#undef	AF_UNIX
#endif

#ifdef BSD42
#define NOFILE getdtablesize() 
#endif

#define	QUIET	1

#define	NOT_QUIET	DEBUG | !QUIET

#define	min(x,y)	( (x)<(y) ? (x) : (y) )

/*	Accept Socket	*/
#define	MAX_ACCEPTS	2
#define	UNIX_ACPT	0
#define	INET_ACPT	1

extern	void	do_command();

jmp_buf	client_dead;

#ifdef	AF_UNIX
static struct sockaddr_un saddr_un;	/**	ソケット	**/
#endif	AF_UNIX
static struct sockaddr_in saddr_in;	/**	ソケット	**/

static	int	sock_d_un;		/**	ソケットのfd	**/
static	int	sock_d_in;		/**	ソケットのfd	**/


struct	cmblk	{
		int	sd;	/** ソケットのfd	**/
	};
#define	COMS_BLOCK	struct	cmblk

static	COMS_BLOCK	*cblk;
		/** クライアントごとのソケットを管理するテーブル **/

static	COMS_BLOCK	accept_blk[MAX_ACCEPTS];	/*accept sock blocks */


/*	Communication Buffers		*/
static	char	snd_buf[R_BUF_SIZ];	/** 送信バッファ **/
static	int	sbp;			/** 送信バッファポインタ **/

static	int	rbc;			/** 受信バッファcounter **/
static	char	rcv_buf[S_BUF_SIZ];	/** 受信バッファ **/

/*	Client Table	*/
int	clientp;	/** cblkの有効なデータの最後を差している **/

int	cur_clp;	/** 現在のクライアントの番号 **/

static	int	*all_socks;	/** ビットパターン
				 which jserver to select を保持 **/
static	int	*ready_socks;	/** データのきているソケットの
					ビットパターンを保持 **/
static	int	*dummy1_socks,
		*dummy2_socks;

static	int	no_of_ready_socks;
static	int	sel_bwidth,	/** bit width of all_socks **/
		sel_width;	/** byte width of all_socks **/

static	int	nofile;		/** No. of files **/

#define	BINTSIZE	(sizeof(int)*8)
#define	sock_set(array,pos)	(array[pos/BINTSIZE] |= (1<<(pos%BINTSIZE)))
#define	sock_clr(array,pos)	(array[pos/BINTSIZE] &= ~(1<<(pos%BINTSIZE)))
#define	sock_tst(array,pos)	(array[pos/BINTSIZE] &  (1<<(pos%BINTSIZE)))


/* No arguments are used. Only options. */
main(argc, argv)
int argc;
char **argv;
{
	int signal_hand();
	extern void terminate_hand();
	extern void _exit();
	int tmpttyfd;

	setuid(geteuid());
/* check whether another jserver already exists. */
#ifdef nodef
	if (create_proof_file(PROOF_FILE_NAME) == -1){
	  fprintf(stderr,"Jserver already exists.\r\n");
	  fprintf(stderr,"If something is wrong, and you want to wake up\r\n");
	  fprintf(stderr,"jserver manually, then remove file: ");
	  fprintf(stderr,PROOF_FILE_NAME);
	  fprintf(stderr,"\r\n");
	  exit(256);
	}
#endif
	get_options(argc, argv);
	printf("\nNihongo Multi Client Server (%s)\n", SER_VERSION);
#ifndef NOTFORK
	if (fork()) {
	    signal(SIGCHLD , _exit);
	    signal(SIGHUP , SIG_IGN);
	    signal(SIGINT , SIG_IGN);
	    signal(SIGQUIT, SIG_IGN);
#ifdef	SIGTSTP
	    signal(SIGTSTP, SIG_IGN);
#endif
	    signal(SIGTERM, _exit);
	    pause();
	}
#endif /* !NOTFORK */

	signal(SIGHUP , signal_hand);
	signal(SIGINT , signal_hand);
	signal(SIGQUIT, signal_hand);
	signal(SIGTERM, terminate_hand);
#ifndef NOTFORK
#ifdef	SIGTSTP
	signal(SIGTSTP, SIG_IGN);
#endif
#endif /* !NOTFORK */
/*	alloc_area();	*/
	read_default();
	demon_init();

	init_giji_eisuu_char();

	env_init();
	file_init();
	dic_init();
	get_kaiseki_area(LENGTHCONV + 1);	/* 変換可能文字数 */
        init_work_areas();
	init_jmt(0);

	read_default_files();

#ifndef NOTFORK
	/* End of initialization, kill parent */
	kill(getppid(), SIGTERM);
	fclose(stdin);
	fclose(stdout);
	if(!noisy){
	    fclose(stderr);
	}

	if ((tmpttyfd = open("/dev/tty", O_RDWR)) >= 0) {
		ioctl(tmpttyfd, TIOCNOTTY, 0);
		close(tmpttyfd);
	}

#endif /* !NOTFORK */

	demon_main();

 demon_fin();
 delete_proof_file(PROOF_FILE_NAME);
}

static
demon_main()
{
 for(;;){
   sel_all();
   new_client();
   for(;;){
#ifdef DEBUG
	error1("Jserver: main loop: ready_socks = %02X\n", ready_socks);
#endif
	if(get_client() == -1) break;
	c_c = &client[cur_clp];
/*	error1("Jserver: cur_clp = %d\n", cur_clp);  Moved into do_command() */
	rbc = 0; sbp = 0;
/*	if(rcv_1_client(cur_clp) == 0){ del_client(); continue; }	*/
	if(setjmp(client_dead)){
		 del_client();
		 continue;
	}
	do_command(c_c);
   }
 }
}

/*
	allocs area for selecting socketts
*/
socket_disc_init()
{
 int sel_w; /* long word(==int) width of all_socks */

	nofile = NOFILE;
	sel_w = (nofile-1)/BINTSIZE + 1;
	all_socks   = (int *)calloc( sel_w,(sizeof(int)));
	ready_socks = (int *)malloc( sel_w*(sizeof(int)));
	dummy1_socks= (int *)malloc( sel_w*(sizeof(int)));
	dummy2_socks= (int *)malloc( sel_w*(sizeof(int)));
	sel_width = sel_w*sizeof(int);	/* byte width */
	sel_bwidth = sel_width*8;	/* bit width */
}

/**	全てのソケットについて待つ	**/
static
sel_all()
{
	bcopy(all_socks,ready_socks,sel_width);
	bzero(dummy1_socks,sel_width);
	bzero(dummy2_socks,sel_width);

	if((no_of_ready_socks
		=select(nofile,ready_socks,dummy1_socks,dummy2_socks,0))==-1){
		xerror("select error");
	}
#ifdef DEBUG
	error1("Jserver: select OK, ready_socks[0]=%02X, n-r-s=%x\n",
			ready_socks[0], no_of_ready_socks);
#endif
}

/**	ready_socksから今やる一人を取り出して返す(cur_clpにもセットする)
	誰も居ない時は-1
	スケジューリングはラウンドロビン	**/
static
get_client()
{
	register int i;

	if(no_of_ready_socks==0) return -1;	/* no client waits service*/

	for (i = cur_clp; ; ) {
		if(no_of_ready_socks == 0) return -1;
		i++;
		if (i >= clientp) i = 0;
		if(sock_tst(ready_socks,cblk[i].sd)){
			sock_clr(ready_socks,cblk[i].sd);
			no_of_ready_socks--;
			return cur_clp = i;
		}
	}
}

/**	新しいクライアントが居るか否かを調べる
	居た場合はcblkに登録する	**/
static
new_client()	/* NewClient */
{ register int sd;
  register int full, i;
  FILE *f[3];
  char gomi[1024];
#ifdef	AF_UNIX
	if(sock_tst(ready_socks,accept_blk[UNIX_ACPT].sd)){
		sock_clr(ready_socks,accept_blk[UNIX_ACPT].sd);
		no_of_ready_socks--;
		sd = socket_accept();
	}else
#endif
	if(sock_tst(ready_socks,accept_blk[INET_ACPT].sd)){
		sock_clr(ready_socks,accept_blk[INET_ACPT].sd);
		no_of_ready_socks--;
		sd = socket_accept_in();
	}else{
		return;
	}
	error1("Jserver: new Client: sd = %d\n", sd);
	/* reserve 2 fd */
	for(full=i=0;i<2;i++){
		if(NULL==(f[i]= fopen("/dev/null","r"))){
			full=1;
		}
	}
	for(i=0;i<2;i++){
		if(NULL != f[i]) fclose(f[i]);
	}

	if(full || sd >= nofile || clientp >= max_client) {
		fprintf(stderr,"jserver : no more client\n");
		read(sd,gomi,1024);
		shutdown(sd, 2);
		close(sd);
		return;
	}

	cblk[clientp].sd = sd;
	sock_set(all_socks,sd);
	clientp++;
}

/**	クライアントをcblkから削除する	**/
/* delete Client (please call from JD_CLOSE) */
del_client()
{
	disconnect_all_env_of_client();
	sock_clr(all_socks,cblk[cur_clp].sd);
	close(cblk[cur_clp].sd);
	cblk[cur_clp] = cblk[clientp - 1];
	client[cur_clp] = client[clientp - 1];
	clientp--;
error1(stderr,"Jserver: Delete Client: cur_clp = %d\n", cur_clp);
}


/**	サーバをイニシャライズする	**/
static
demon_init()	/* initialize Demon */
{
	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);


	if((cblk =(COMS_BLOCK *)malloc(max_client * sizeof(COMS_BLOCK)))
	   == NULL){
	    perror("Malloc for cblk");
	    delete_proof_file(PROOF_FILE_NAME);
	    exit(1);
	}
	if((client =(CLIENT *)malloc(max_client * sizeof(CLIENT)))
	   == NULL){
	    perror("Malloc for client");
	    delete_proof_file(PROOF_FILE_NAME);
	    exit(1);
	}
#if defined(SYSVR2) || defined(SUN)
	srand48(time((long *)0));
#else
	srand((int)time((long *)0));
#endif
	clientp = 0;	/* V3.0 */
	cur_clp = 0;	/* V3.0 */
	socket_disc_init();
	socket_init_in();
	socket_init();
}

/**	サーバを終わる	**/
demon_fin()
{
    int trueFlag = 1;
    int fd;
#ifdef	AF_UNIX
    struct sockaddr_un addr_un;
#endif	AF_UNIX
    struct sockaddr_in addr_in;
    int addrlen;

    /*
      accept all pending connection from new clients,
      avoiding kernel hangup.
     */

#ifdef	AF_UNIX
    ioctl(sock_d_un, FIONBIO, &trueFlag);
    for (;;) {
	addrlen = sizeof(addr_un);
	if (accept(sock_d_un, &addr_un, &addrlen) < 0) break;
	/* EWOULDBLOCK EXPECTED, but we don't check */
    }
    shutdown(sock_d_un, 2);
    close(sock_d_un);
#endif	AF_UNIX

    ioctl(sock_d_in, FIONBIO, &trueFlag);
    for (;;) {
	addrlen = sizeof(addr_in);
	if (accept(sock_d_in, &addr_in, &addrlen) < 0) break;
	/* EWOULDBLOCK EXPECTED, but we don't check */
    }
    shutdown(sock_d_in, 2);
    close(sock_d_in);

    for (fd = nofile; fd >= 0; fd--) {
	shutdown(fd, 2);
	close(fd);
    }
}

/*------*/

/**	**/
gets_cur(cp)
register char *cp;
{
    while((*cp++ = getc_cur()) != '\0');
}

/**	**/
getws_cur(cp)
register w_char *cp;
{
    while((*cp++ = get2_cur()) != 0);
}

/**	カレント・クライアントから2バイト取る	**/
get2_cur()
{
	register int	x;
	x = getc_cur();
	return (x << 8) | getc_cur();
}

/**	カレント・クライアントから4バイト取る	**/
get4_cur()
{
	register int	x1, x2, x3;
	x1 = getc_cur();
	x2 = getc_cur();
	x3 = getc_cur();
	return (x1 << (8 * 3)) | (x2 << (8 * 2)) | (x3 << (8 * 1)) | getc_cur();
}

/**	カレント・クライアントから1バイト取る	**/
getc_cur()
{static int rbp;
 if(rbc<=0){
	rbc=rcv_1_client(cur_clp);
	rbp = 0;
 }
 rbc--;
 return rcv_buf[rbp++] & 0xFF;
}

/**	クライアントから1パケット取る	**/
static
rcv_1_client(clp)
/**	clp=クライアント番号	**/
{	register int cc;
	cc = read(cblk[clp].sd, rcv_buf, S_BUF_SIZ);
#ifdef DEBUG
	error1("Jserver: rcv: clp = %d, sd = %d, cc = %d\n", 
			clp, cblk[clp].sd, cc);
	dmp(rcv_buf,cc);
#endif
	if(cc<=0){ /* client dead */ longjmp(client_dead,666); }
	return cc;
}

/**	クライアントへ1パケット送る	**/
static
snd_1_client(clp,n)
/**	clp=クライアント番号, n= send n Bytes	**/
{	register int cc,x;
#if DEBUG
	error1("Jserver: snd: clp = %d, sd = %d\n", clp, cblk[clp].sd);
	dmp(snd_buf,n);
#endif
	for(cc=0;cc<n;){
		x = write(cblk[clp].sd, &snd_buf[cc], n-cc);
		if(x<=0){ /* client dead */ longjmp(client_dead,666); }
		cc+=x;
	}
}

/**	**/
puts_cur(p) char *p;
{register int c;
 while(c= *p++) putc_cur(c);
 putc_cur(0);
}

/**	**/
putws_cur(p) wchar *p;
{register int c;
 while(c= *p++) put2_cur(c);
 put2_cur(0);
}

/**	**/
putnws_cur(p, n)
register wchar *p;
register int n;
{
    register int c;
    for(;n > 0 ; n--){
	if((c= *p++) == 0)break;
	put2_cur(c);
    }
    put2_cur(0);
}

/**	カレント・クライアントへ2バイト送る	**/
put2_cur(c)
register int c;
{
	putc_cur( c >> (8 * 1));
	putc_cur(c);
}

/**	カレント・クライアントへ4バイト送る	**/
put4_cur(c)
register int c;
{
	putc_cur(c >> (8 * 3));
	putc_cur(c >> (8 * 2));
	putc_cur(c >> (8 * 1));
	putc_cur(c);
}

/**	カレント・クライアントへ1バイト送る	**/
putc_cur(c)
register int c;
{
	snd_buf[sbp++] = c;
	if (sbp >= R_BUF_SIZ) {
		snd_1_client(cur_clp, R_BUF_SIZ);
		sbp = 0;
	}
}

/**	カレント・クライアントの送信バッファをフラッシュする	**/
putc_purge()
{
	if (sbp != 0) {
		snd_1_client(cur_clp,sbp);
		sbp = 0;
	}
}

/*-----*/

/**	ソケットのイニシャライズ	**/
static
socket_init()
{
#ifdef	AF_UNIX
	unlink(sockname);
	saddr_un.sun_family = AF_UNIX;
	strcpy(saddr_un.sun_path, sockname);
	if ((sock_d_un = socket(AF_UNIX, SOCK_STREAM, 0)) == ERROR) {
		xerror("Can't create socket.\n");
	}
	if (bind(sock_d_un, &saddr_un, strlen(saddr_un.sun_path) + 2)
	    == ERROR) {
		shutdown(sock_d_un, 2);	/* add KURI */
		xerror("Can't bind socket.\n");
	}
	if (listen(sock_d_un, 5) == ERROR) {
		shutdown(sock_d_un, 2);	/* add KURI */
		xerror("Can't listen socket.\n");
	}
	signal(SIGPIPE, SIG_IGN);
#if DEBUG
	error1("\nJserver: sock_d_un = %d\n", sock_d_un);
#endif
	accept_blk[UNIX_ACPT].sd = sock_d_un;
	sock_set(all_socks,sock_d_un);
#endif	AF_UNIX
}

/*	Inet V3.0 */
static
socket_init_in()
{
 int on = 1;	/* add KURI */
 int serverNO,port; struct servent *sp;
#if !defined(SO_DONTLINGER) && defined(SO_LINGER)
 struct linger linger;
#endif

    serverNO = 0;
    if ((sp = getservbyname("wnn4","tcp")) == NULL) {
 	port =  htons(WNN_PORT_IN + serverNO);
    } else {
#ifdef UX386
	int	hs;

	hs = ntohs(sp->s_port) + serverNO;
	port = htons(hs);
#else
	port = htons(ntohs(sp->s_port) + serverNO); 
#endif
    }
#if DEBUG
	error1("Jserver: port=%x\n",port);
#endif
    saddr_in.sin_family = AF_INET;
    saddr_in.sin_port = port;
    saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((sock_d_in = socket(AF_INET,SOCK_STREAM, 0)) == ERROR) {
	xerror("Jserver: can't create inet-socket\n");
    }
    setsockopt(sock_d_in, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)); /* ADD KURI */
#ifdef SO_DONTLINGER
    setsockopt(sock_d_in, SOL_SOCKET, SO_DONTLINGER, (char *)0, 0);
#else
#ifdef SO_LINGER
    linger.l_onoff = 0;
    linger.l_linger = 0;
    setsockopt(sock_d_in, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof linger);
#endif
#endif

    if (bind(sock_d_in,&saddr_in,sizeof(saddr_in)) == ERROR) {
	shutdown(sock_d_in, 2);	/* add KURI */
	xerror("Jserver: can't bind inet-socket\n");
    }
    if (listen(sock_d_in,5) == ERROR) {
	shutdown(sock_d_in, 2);	/* add KURI */
	xerror("Jserver: can't listen inet-socket\n");
    }
#if DEBUG
	error1("\nJserver: sock_d_in = %d\n", sock_d_in);
#endif
	accept_blk[INET_ACPT].sd = sock_d_in;
	sock_set(all_socks,sock_d_in);
}


/**	新しいクライアントのソケットをacceptする	**/
static int
socket_accept()
{
#ifdef	AF_UNIX
 struct sockaddr_un addr;
 int addrlen;

 addrlen = sizeof(addr);
 return accept(sock_d_un, &addr, &addrlen);
#endif	AF_UNIX
}

static int
socket_accept_in()
{
 struct sockaddr_in addr;
 int addrlen;

 addrlen = sizeof(addr);
 return accept(sock_d_in, &addr, &addrlen);
}

static
xerror(s)
register char *s;
{
  fprintf(stderr,"%s\n", s);
  delete_proof_file(PROOF_FILE_NAME);
  exit(1);
}

#if DEBUG
static
dmp(p,n)
char *p;
{
	int	i, j;

	for (i = 0; i < n; i += 16) {
		for (j = 0; j < 16; j++) {
			fprintf(stderr,"%02x ", p[i + j] & 0xFF);
		}
		fprintf(stderr,"n=%d\n",n);
	}
}

niide(x)		/* おまけだよ。niide(3);とコールしてね */
{
	switch (x) {
	case 0:
		return 1;
	case 1:
		fprintf(stderr,"ゲッターロボは");
		break;
	case 2:
		fprintf(stderr,"ゴレンジャーは");
		break;
	case 3:
		fprintf(stderr,"ロードランナーシリーズは");
		break;
	default:
		return x * niide(x - 1);
	}
	fprintf(stderr,"良い番組だ by 新出。\n");
	return x * niide(x - 1);
}
#endif

get_options(argc, argv)
int argc;
char **argv;
{
    int c;
    extern char *optarg;

    strcpy(jserverrcfile, SERVER_INIT_FILE);
    while ((c = getopt(argc, argv, "f:s:h:")) != EOF) {
	switch(c){
	case 'f':
	    strcpy(jserverrcfile, optarg);
	    break;
	case 's':
	    noisy = 1;
	    if(strcmp("-", optarg) != 0){
		if(freopen(optarg, "w", stderr) == NULL){
		    printf("Error in opening scriptfile %s.\n" , optarg);
		    delete_proof_file(PROOF_FILE_NAME);
		    exit(1);
		}
	    }
	    error1("Jserver script started\n");
	    break;
	case 'h':
	    hinsi_file_name = optarg;
	    break;
	default:
	    printf("usage: jserver [-F <fuzokugo file> -f <initialize-file> -s <script-file(\"-\" for stderr) -h hinsi_file>]\n");
	  delete_proof_file(PROOF_FILE_NAME);
	  exit(1);
	}
    }
}


create_proof_file(file_name)
char *file_name;
{
  int fd;

  if ((fd = open(file_name,(O_CREAT | O_EXCL), 0770)) == -1) {
    return(-1);
  }
  close(fd);
  return(0);
}

delete_proof_file(file_name)
char *file_name;
{
  if(unlink(file_name) == -1){
    return(-1);
  }
  return(0);
}


/*
*/
js_who()
{register int i,j;

 put4_cur(clientp); 
 for(i=0;i<clientp;i++){
	put4_cur(cblk[i].sd);
	puts_cur( client[i].user_name );
	puts_cur( client[i].host_name );
	for(j=0;j<WNN_MAX_ENV_OF_A_CLIENT;j++){
		put4_cur((client[i].env)[j]);
	}

 }
 putc_purge();
}

js_kill()
{
    if(clientp == 1){
	put4_cur(0);
	putc_purge();
	terminate_hand();
    }else{
	put4_cur(clientp - 1);
	putc_purge();
    }
}
