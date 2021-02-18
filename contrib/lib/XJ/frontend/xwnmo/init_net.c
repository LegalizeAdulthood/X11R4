/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OMRON not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  OMRON makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*	Version 4.0
 */

#ifdef	DIRECT

#ifdef UX386
#include <X11/Xos.h>
#else
#if !defined(EWS) || !defined(FD_SETSIZE) 
#include <sys/types.h>
#endif
#endif

#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#ifdef UX386
#include <termio.h>
#include <net/in.h>
#include <net/netdb.h>
#include "ForSysV.h"
#else
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <sys/errno.h>
#include "xwnmo.h" 
#include "commonhd.h" 
#if defined(SX8870) || defined(FACOM)
#include "ForSysV.h"
#endif

#define	XWNMO_PORT_IN	(0x9494)
#define UNIX_PATH	"/tmp/xwnmo.V2"
#define	UNIX_ACPT	0
#define INET_ACPT	1

struct	cmblk	{
		int	sd;
		int	sdbit;
	};

static	struct  cmblk	accept_blk[2];

int maxsock = 0;
int	errno;
int  so, sa[FD_SETSIZE], sockused[FD_SETSIZE], af, h, port, addrlen;
static	int	so_un, so_in;
struct sockaddr  *addr;
static struct sockaddr_in  saddr_in;
#ifndef UX386
static struct sockaddr_un  saddr_un;
#endif
struct hostent  *host;
char  name[128];
char sockbuf[BUFSIZ];

init_net_un()
{
#ifndef UX386
  int i;
  int on = 1;
  char buf[100];

     saddr_un.sun_family = af = AF_UNIX;
     strcpy(saddr_un.sun_path, UNIX_PATH);
  
  if((so_un = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    Error(0, "socket");
  if(bind(so_un, &saddr_un, strlen(saddr_un.sun_path) + 2) < 0)
    Error(0, "bind");
  if(listen(so_un, 5) < 0)
    Error(0, "listen");

  accept_blk[UNIX_ACPT].sd = so_un;
  accept_blk[UNIX_ACPT].sdbit = 1 << so_un;
#endif
}

init_net_in()
{
  int i;
  int on = 1;
  char buf[100];
  struct servent	*sp;

	if ((sp = getservbyname("xwnmo","tcp")) == NULL) {
		port = XWNMO_PORT_IN;
	} else {
		port = ntohs(sp->s_port);
	}
/*
  gethostname(name, MAXLEN);
  host = gethostbyname(name);
*/
  saddr_in.sin_family = AF_INET;
  saddr_in.sin_port = htons(port);
/*
  saddr_in.sin_addr.s_addr = *((int *) host->h_addr);
*/
  saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if((so_in = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    Error(0, "socket");
  setsockopt(so_in, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
  if(bind(so_in, &saddr_in, sizeof(struct sockaddr_in)) < 0)
    Error(0, "bind");
  if(listen(so_in, 5) < 0)
    Error(0, "listen");

    accept_blk[INET_ACPT].sd = so_in;
    accept_blk[INET_ACPT].sdbit = 1 << so_in;
}

init_net()
{
	int	i;

	init_net_un();
	init_net_in();
	for (i = 0 ; i < FD_SETSIZE ; i++) {
		sockused[i] = 0;
	}
	maxsock = 0;
}

close_net()
{
    int i;
	int	trueFlag = 1;
#ifndef UX386
	struct sockaddr_un addr_un;
#endif
	struct sockaddr_in addr_in;
	int	addrlen;

#ifndef UX386
	ioctl(so_un, FIONBIO, &trueFlag);
	for (;;) {
		addrlen = sizeof(addr_un);
		if (accept(so_un, &addr_un, &addrlen) < 0) break;
	}
	shutdown(so_un, 2);
	close(so_un);
#endif
	ioctl(so_in, FIONBIO, &trueFlag);
	for (;;) {
		addrlen = sizeof(addr_in);
		if (accept(so_in, &addr_in, &addrlen) < 0) break;
	}
	shutdown(so_in, 2);
	close(so_in);

    for (i = 0 ; i < maxsock ; i++) {
      if (sockused[i]) {
	shutdown(sa[i], 2);
	close(sa[i]);
      }
    }
}


int              i, nbytes;
char             buf[STRBUFSIZE];
int              col = 0, rcol = 0, rlen = 0, cursorcol = 0;
int              charwi;
XEvent           event;
KeySym           keysym;

LXNextEvent(display, ret_ev)
Display	*display;
XEvent	*ret_ev;
{
  int len, i, n;
  Window	window;
  int		cnt;
#ifndef UX386
  struct sockaddr_un uaddr;
#endif
  struct sockaddr_in iaddr;
  int	uaddrlen, iaddrlen;
#if defined(BSD42) || defined(EWS)
  fd_set select_mask;
#else	
  int select_mask;
#endif
  int first;

  while(1){ 
/*
  acceptと、Xのイベントと、socketのreadのいずれかが
  発生するのをselectにて待つ。

  */
    if (XPending(dpy) > 0) {
	XNextEvent(dpy, ret_ev);
	return;
    }
    FD_ZERO(&select_mask);
    FD_SET(so_un, &select_mask);
    FD_SET(so_in, &select_mask);
    FD_SET(dpy->fd, &select_mask);
    for (i = 0 ; i < maxsock ; i++) {
      if (sockused[i]) {
	FD_SET(sa[i], &select_mask);
      }
    }
    n = select(FD_SETSIZE, &select_mask, 
	       (int *)NULL, (int *)NULL, (struct timeval *)NULL);
    if (FD_ISSET(so_un, &select_mask)) {
#ifndef UX386
      FD_CLR(so_un, &select_mask);
      for (i = 0 ; i < FD_SETSIZE ; i++) {
	if (sockused[i] == 0) {
	  uaddrlen = sizeof(uaddr);
	  if((sa[i] = accept(so_un, &uaddr, &uaddrlen)) < 0)
	    Error(0, "accept");
	  sockused[i] = 1;
	  new_cl_sock(sa[i]);
	  if (i + 1 > maxsock) {
	    maxsock = i + 1;
	  }
	  first = 1;
	  goto FIRST;
	  /*
	  break;
	  */
	}
      }
#endif
    }
    else if (FD_ISSET(so_in, &select_mask)) {
      FD_CLR(so_in, &select_mask);
      for (i = 0 ; i < FD_SETSIZE ; i++) {
	if (sockused[i] == 0) {
	  iaddrlen = sizeof(iaddr);
	  if((sa[i] = accept(so_in, &iaddr, &iaddrlen)) < 0)
	    Error(0, "accept");
	  sockused[i] = 1;
	  new_cl_sock(sa[i]);
	  if (i + 1 > maxsock) {
	    maxsock = i + 1;
	  }
	  first = 1;
	  goto FIRST;
	  /*
	  break;
	  */
	}
      }
    }

    else if (FD_ISSET(dpy->fd, &select_mask)) {
      /* Xのイベント */
      	XNextEvent (dpy, ret_ev);
	  return;
    }
    else {
      /* socketのread */
      for (i = 0 ; i < maxsock ; i++) {
	if (sockused[i] && FD_ISSET(sa[i], &select_mask)) {
	  FD_CLR(sa[i], &select_mask);
	  break;
	}
      }
FIRST:
      if ( (len = read(sa[i], sockbuf, sizeof(sockbuf))) == -1) {
	/* readがうまくいかない場合 */
#if defined(BSD42) || defined(EWS)
	if (errno == EBADF || ECONNRESET) { /* ファイルディスクリプタがおかしい */
#else
	if (errno == EBADF) { /* ファイルディスクリプタがおかしい */
#endif
	  /* その場合はコネクションが外れたものとみなして、
	     そのディスクリプタの記述を削除する。
	  sockused[i] = 0;
	  reset_maxsock();
	  close (sa[i]);
	  */
	  close_socket(sa[i]);
	}
	else { /* その他の場合でおかしい場合 */
#ifndef NOMESG
	  fprintf(stderr, "read: fid=%d, len=%d, errno=%d.\n",
		  sa[i], len, errno);
#endif
	  exit(0);
	}
      }
      else if (len == 0) {
	/* EOFの場合
	sockused[i] = 0;
	reset_maxsock();
	close (sa[i]);
	*/
	close_socket(sa[i]);
      }
      else { /* readがうまくいった場合 */
	if(first) {
		buffer_to_event(sockbuf, ret_ev);
	} else {
		buffer_to_init();
	}
	for (cnt = 1; cnt < MAX_CLIENTS; cnt++) {
		if (client_mem[cnt] != NULL && client_mem[cnt]->fd == sa[i]) {
			if (client_mem[cnt]->w == 0) {
				client_mem[cnt]->w = ret_ev->xkey.window;
				XSelectInput(dpy, client_mem[cnt]->w, StructureNotifyMask);
				if((input_mem[input_num] = (XJInput *)malloc(sizeof(XJInput))) == NULL){
					fprintf(stderr, "error malloc\n");
					return(-1);
				}
				input_mem[input_num]->w = ret_ev->xkey.window;
				input_mem[input_num++]->pclient = client_mem[cnt];
			}
			client_mem[cnt]->n_write = 1;
			return;
		}
	}
      }
    }
  }
}







buffer_to_event(buf, event)
     /* socketからreadしたバッファ内のデータを
	event構造対に再格納する */
char *buf;
XEvent *event;
{
  event->xkey.display = dpy;
  event->xkey.type = (int)buf[0];
  event->xkey.state = (unsigned int)buf[1];
  event->xkey.serial = (unsigned long)ntohs(*(short *)(buf + 2));
  event->xkey.time = (Time)ntohl(*(Time *)(buf + 4));
  event->xkey.root = (Window)ntohl(*(Window *)(buf + 8));
  event->xkey.window = (Window)ntohl(*(Window *)(buf + 12));
  event->xkey.subwindow = (Window)ntohl(*(Window *)(buf + 16));
  event->xkey.x_root = (int)ntohs(*(short *)(buf + 20));
  event->xkey.y_root = (int)ntohs(*(short *)(buf + 22));
  event->xkey.x = (int)ntohs(*(short *)(buf + 24));
  event->xkey.y = (int)ntohs(*(short *)(buf + 26));
  event->xkey.keycode = (unsigned int)ntohs(*(short *)(buf + 28));
  event->xkey.same_screen = (Bool)buf[30];
}

symlen_to_buffer(buf, keysym, len)
char *buf;
KeySym keysym;
int len;
{
  short foo;

  foo = htons((short)keysym);
  bcopy(&foo, buf, 2);
  foo = htons((short)len);
  bcopy(&foo, buf + 2, 2);
}

reset_maxsock()
     /* オープンされているソケットで最大の
	ファイルディスクリプタを持つものを
	確認する。 */
{
  int i;
  
  maxsock = 0;
  for (i = 0 ; i < FD_SETSIZE ; i++) {
    if (sockused[i]) {
      maxsock = i + 1;
    }
  }
}

Error()
{
	return;
}

new_cl_sock(fd)
int	fd;
{
	register XJClient	*xjc;
	char buf[16];

	if((xjc = (XJClient *)malloc(sizeof(XJClient))) == NULL){
		fprintf(stderr, "error in malloc(cl_touroku)\n");
		return(-1);
	}
	xjc->dispmode = 2;
	xjc->w = 0;
	xjc->fd = fd;

	xjc->wp1 = xjc->wp2 = xjc->wp3 = xjc->w1 = xjc->w2 = xjc->w3 
	= xjc->wnf = xjc->wnb = xjc->wnn = 0L;
	if (new_client(0, xjc) < 0) {
		fprintf(stderr,"clients table over flow\n");
		free(xjc);
		return_socket_error(fd, 7);
		return(-1);
	}
	read(fd, buf, 16);
	return_socket_ok(fd);
	return(0);
}

close_socket(fd)
int	fd;
{
	int	i;
  	for (i = 0 ; i < maxsock ; i++) {
		if (fd == i && sockused[i] == 1) {
			sockused[i] = 0;
			reset_maxsock();
			close (sa[i]);
		}
	}
}

buffer_to_init()
{
	return; /* xwnmo don't support multi display */
}

save_sockbuf(ret_buf, len, keysym, buf)
unsigned char *ret_buf;
int len, keysym;
char *buf;
{
    int tmp_keysym;
    unsigned short tmp_len;

    bzero(ret_buf, 2);
    tmp_len = (unsigned short)htons(len);
    bcopy(&tmp_len, ret_buf + 2, 2);
    tmp_keysym = htonl(keysym);
    bcopy(&tmp_keysym, ret_buf + 4, 4);
    bcopy(buf, ret_buf + 8, len);
}

make_err_socket(ret_buf, no)
unsigned char *ret_buf;
int no;
{
    short foo;

    foo = htons((short)-1);
    bcopy(&foo, ret_buf, 2);
    foo = htons((short)no);
    bcopy(&foo, ret_buf + 2, 2);
}
#endif	/* DIRECT */
