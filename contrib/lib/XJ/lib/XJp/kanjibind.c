/* Copyright 1989 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 * Author:  Akira Kon, NEC Corporation.
 *          kon@bs6.bsd.mt.nec.co.jp
 */

/*
  ERROR NUMBER for XLookupKanjiString

  ����ϡ�XJplib.h�ˤ�񤤤Ƥ���Ȼפ��롣�����������Υե�����Ǥϡ�
  XJplib.h��include�����ˡ��������������뤳�ȤȤ��롣
*/

#define XJP_L_NO_EXIST_FE 200
#define XJP_L_BAD_SOCK    401
#define XJP_L_NOT_CONN    402
#define XJP_L_ERR_WRITE   403
#define XJP_L_ERR_INTR    404
#define XJP_L_ERR_MISC    405

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <sys/socket.h>
#ifdef UX386
#include <net/in.h>
#include <net/netdb.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <sys/errno.h>

extern int XJp_errno;
extern int errno;

#define XLKSError(x) (XJp_errno = x, -1)

static char sccs_id[] = "%W%	%D% %T% NEC Corporation.";
static char rcs_id[] = "$Header$";

#define ServerNameLength 100	/* �إ����Ф�̾���κ���Ĺ */
#define XJpFEPropName	 "XJPFRONTEND"

#if !defined(SYSV) && !defined(USG)
#include <setjmp.h>
#include <signal.h>

static jmp_buf alrmjmp;

static catch()
{
  longjmp(alrmjmp);
}
#endif /* !defined(SYSV) && !defined(USG) */


XLookupKanjiString(event_struct, buffer_return, bytes_buffer,
		   keysym_return, status_return)
     XKeyEvent      *event_struct;
     unsigned char  *buffer_return;
     int            bytes_buffer;
     KeySym         *keysym_return;
     XComposeStatus *status_return;
{
  static firstTime = 1; 
  static int  so;
  int  len;
  char sockbuf[BUFSIZ];
  int state;
  
#ifdef DEBUG
  fprintf(stderr, "XLookupKanjiString called.\n");
#endif
  
/* �Ѵ���̤�bytes_buffer�����礭���Ȥ��Ϥɤ�����ΤǤ��礦��?
   ��Ϥ�ΤƤƤ��ޤ��ΤǤ��礦��
   ����Ȥ⡢�Ĥ�򲱤��Ƥ����ơ����λ������ˤĤ����֤��Ƥ�����ΤǤ��礦����
   ������ȡ����Υ������������ޤǻĤ꤬�֤��ʤ���Ǥ����
   ������ᤷ�����ȤǤ���
   ���Ƥɤ����ޤ��礦��?		�����Ǥ���
*/

  if (firstTime) {
    int port, af, addrlen;
    struct sockaddr  *addr;
    struct sockaddr_in  iaddr;
    struct hostent  *host;
    Atom prop, what_type;
    int what_format;
    long nitems, remain;
    char *gotten;
    Window w;
    Display *dpy = event_struct->display;

    /* �ץ�ѥƥ�"XJPFRONTEND"��Ĵ�٤ơ��ɤΥۥ��Ȥ�
       ���ʴ����ե��ȥ���ɤ�ư���Ƥ��뤫�����롣*/

    prop = XInternAtom(dpy, 
		       XJpFEPropName, 
		       0 /* ���ȥब¸�ߤ��ʤ��ä����� */
		       );

    /* XJPFRONTEND�ˤϰʲ��Τ褦�ʥǡ�������Ǽ����Ƥ���Ȼפ���

      (�⤷��XJPFRONTEND�Ȥ����ץ�ѥƥ���¸�ߤ��Ƥ��ʤ��Τʤ顢XJP��
       ���ȥ���ɤ�¸�ߤ��Ƥ��ʤ��Ȥ������ȤǤ��롣)

      +-------------------------------------+
      | XJP�ե��ȥ���ɤ�̾���ȥС������ |
      |             (128 Byte)              |
      +-------------------------------------+
      |XJP�ե��ȥ���ɤ�ư���Ƥ���ۥ���̾|
      |    (NULL�����ߥ͡��ȡ�128 Byte)     |
      +-------------------------------------+
      |     �����ӥ��ݡ����ֹ� (4 Byte)     |
      +-------------------------------------+

	*/

#define offset_to_version	0
#define version_size		128
#define offset_to_hname		offset_to_version + version_size
#define hname_size		128
#define offset_to_portnumber	offset_to_hname + hname_size
#define portnumber_size		4

    w = XGetSelectionOwner(dpy, prop);
    if (w == None) {
      return XLKSError(XJP_L_NO_EXIST_FE); 
      /* ���ʴ����ե��ȥ���ɤ�ư���Ƥ��ʤ� */
    }

    XGetWindowProperty(dpy, DefaultRootWindow(dpy), 
		       prop, 0, ServerNameLength,
		       0, AnyPropertyType, &what_type, &what_format, 
		       &nitems, &remain, &gotten);

    bcopy(gotten + offset_to_portnumber, (char *)&port, portnumber_size);
    port = ntohl((unsigned long)port);
    iaddr.sin_family = af = AF_INET;
    iaddr.sin_port = port & 0xffff;
    host = gethostbyname(gotten + offset_to_hname);
    if (host)
    	iaddr.sin_addr.s_addr = *((int *) host->h_addr);
    else
    	iaddr.sin_addr.s_addr = htonl((unsigned long)INADDR_ANY);
    addr = (struct sockaddr *) &iaddr;
    addrlen = sizeof(struct sockaddr_in);

/*
#undef offset_to_version
#undef version_size
#undef offset_to_hname
#undef hname_size
#undef offset_to_portnumber
#undef portnumber_size
*/
  
    if((so = socket(af, SOCK_STREAM, 0)) < 0) {
      return XLKSError(XJP_L_BAD_SOCK); /* socket����������ʤ� */
    }
    if(connect(so, addr, addrlen) < 0) {
      close (so);
      return XLKSError(XJP_L_NOT_CONN); /* connect�Ǥ��ʤ� */
    }
    firstTime = 0; /* �⤦ firstTime �ǤϤʤ��� */
    { /* �����

	 ��ʬ���ɤΣإ����ФǺ�ư���Ƥ��뤫�����Τ��롣*/

      char firstdata[ServerNameLength], *getenv();
      int state, detail;

      if (DisplayString(dpy))
	strcpy(firstdata + 2, DisplayString(dpy));
      /* XOpenDisplay��ʸ������Ϥ������ˤϤ����ǽ����� */

      else if (getenv("DISPLAY"))
	strcpy(firstdata + 2, getenv("DISPLAY"));
      /* DISPLAY�Ķ��ѿ��˵��Ҥ��Ƥ���ʸ������ɤ� */

      else {
	firstTime = 1; /* ����Ϻǽ餫����ľ�� */
	return XLKSError(XJP_L_ERR_MISC);
	/* ��������ʬ�ˤ���ʤ�ơ��������ʤ��Ȼפ��뤬... */
      }

      if (!strncmp(firstdata + 2, "unix:", 5) || 
	  !strncmp(firstdata + 2, ":", 1)) {

	char hostname[ServerNameLength];

	if (gethostname(hostname, ServerNameLength) == -1) {
	  firstTime = 1; /* ����Ϻǽ餫����ľ�� */
	  return XLKSError(XJP_L_ERR_MISC);
	  /* ����⡢��������ʬ���̤뤳�ȼ��Τ����ˤ������� */
	}
	if (strcmp(hostname, gotten + offset_to_hname)) {

	  /* XJP�ե��ȥ���ɤΥۥ���̾�ȡ����Υ饤�֥������Ѥ��Ƥ� 
	     �륢�ץꥱ�������Υۥ���̾���㤦�ʤ��unix:0�ǤϤ����ʤ��� */

	  strcat(hostname, index(firstdata + 2, ':'));
	  strcpy(firstdata + 2, hostname);
	}
      }
      firstdata[0] = (char)1;
      firstdata[1] = (char)strlen(firstdata + 2);
      if (write(so, firstdata, firstdata[1] + 2) == -1) {
	firstTime = 1; /* ����Ϻǽ餫����ľ�� */
	close (so);
	return XLKSError(XJP_L_ERR_WRITE); /* write�Ǥ��ʤ�����꤬���ʤ��� */
      }

      {
#if !defined(SYSV) && !defined(USG)
	int (*func)();

	if (setjmp(alrmjmp)) {
	  firstTime = 1;
	  close (so);
	  return XLKSError(XJP_L_ERR_INTR);
	}
	func = signal(SIGALRM, catch);
#endif /* !defined(SYSV) && !defined(USG) */

        /* �����ǡ������ޤ򥻥åȤ��ơ����ޤ�쥹�ݥ󥹤��ʤ�����
	   ���顼�Ȥ��롣 */
      
        alarm(7); /* ���ä��Ԥġ�*/
        len = read(so, firstdata, sizeof(firstdata));
        alarm(0);

#if !defined(SYSV) && !defined(USG)
	signal(SIGALRM, func);
#endif /* !defined(SYSV) && !defined(USG) */

        if (len == -1) { /* read�����顼���֤�����硣 */
	  firstTime = 1; /* ����Ϻǽ餫����ľ�� */
	  close (so);
	  if (errno == EINTR) { /* ���顼�ब���ä���硣 */
	    return XLKSError(XJP_L_ERR_INTR);
	  }
	  else { /* ����ʳ��Υ��顼�ʤ��ꤨ�ʤ��褦�˻פ��뤬...�� */
	    return XLKSError(XJP_L_ERR_MISC);
	  }
	}
      }
      buf_to_firstanswer(firstdata, &state, &detail);
      if (state == -1) { /* XJP�ե��ȥ���ɤ����顼���֤������ */
	return XLKSError(detail);
      }
    }
  } /* end of firsttime */
  event_to_buffer(event_struct, sockbuf);
  if (write(so, sockbuf, 32) == -1) {
    firstTime = 1; /* ����Ϻǽ餫����ľ�� */
    close (so);
    return XLKSError(XJP_L_ERR_WRITE); /* write�Ǥ��ʤ�����꤬�⤦���ʤ��� */
  }

#ifdef DEBUG
  fprintf(stderr, "XLKUPKS: keycode = 0x%x\n", event_struct->keycode);
  fprintf(stderr, "XLKUPKS: sockbuf.keycode = 0x%x\n",
	  ntohs(*(unsigned short *)(sockbuf + 28)));
  fprintf(stderr, "Event written\n");
#endif

  {
#if !defined(SYSV) && !defined(USG)
    int (*func)();

    if (setjmp(alrmjmp)) {
      firstTime = 1;
      close (so);
      return XLKSError(XJP_L_ERR_INTR);
    }
    func = signal(SIGALRM, catch);
#endif /* !defined(SYSV) && !defined(USG) */

    /* �����ǡ������ޤ򥻥åȤ��ơ����ޤ�쥹�ݥ󥹤��ʤ�����
       ���顼�Ȥ��롣 */

    alarm(7); /* ���ä��Ԥġ�*/
    len = read(so, sockbuf, sizeof(sockbuf));
    alarm(0);

#if !defined(SYSV) && !defined(USG)
    signal(SIGALRM, func);
#endif /* !defined(SYSV) && !defined(USG) */

    if (len == -1) { /* read�����顼���֤�����硣 */
      firstTime = 1; /* ����Ϻǽ餫����ľ�� */
      close (so);
      if (errno == EINTR) { /* ���顼�ब���ä���硣 */
        return XLKSError(XJP_L_ERR_INTR);
      }
      else { /* ����ʳ��Υ��顼�ʤ��ꤨ�ʤ��褦�˻פ��뤬...�� */
        return XLKSError(XJP_L_ERR_MISC);
      }
    }
  }
  buf_to_symlen(sockbuf, &state, &len, keysym_return);
  if (state == -1) {
    return XLKSError(len); /* ���顼�λ���len�˥��顼�����ɤ����� */
  }
/*
  strncpy(buffer_return, sockbuf + 8,
	(len < bytes_buffer)? len : bytes_buffer);
*/
  strncpy((char *)buffer_return, sockbuf + 8,
	(len < bytes_buffer)? len :(len = bytes_buffer));
  return len;
}

buf_to_symlen(buf, state, len, keysym)
char *buf;
KeySym *keysym;
int *state, *len;
{
  short foo;
  int goo;
  
  bcopy(buf, (char *)&foo, 2);
  *state = (int)ntohs((unsigned short)foo);
  bcopy(buf + 2, (char *)&foo, 2);
  *len = (int)ntohs((unsigned short)foo);
  if (keysym) { /* check if the KEYSYM is NULL */
    bcopy(buf + 4, (char *)&goo, 4);
    *keysym = (KeySym)ntohl((unsigned long)goo);
  }
}

buf_to_firstanswer(buf, state, detail)
char *buf;
int *state, *detail;
{
  short foo;
  
  bcopy(buf, (char *)&foo, 2);
  *state = (int)ntohs((unsigned short)foo);
  bcopy(buf + 2, (char *)&foo, 2);
  *detail = (int)ntohs((unsigned short)foo);
}

/*
  event_to_buffer �ϡ��������٥�Ȥ򣳣��Х��ȤΥХåե��˽񤭹���
  ��ΤǤ��롣���ΥХåե��Ϥ��ʴ����Ѵ��ե��ȥ���ɤȤ��̿���
  �Ѥ������ΤǤ��롣
  
  ����:
    event - �������٥��
    buf   - �Хåե�
*/

event_to_buffer(event, buf)
XKeyEvent *event;
char *buf;
{
  short foo;
  int bar;

  buf[0] = 2;/* code */
  buf[1] = event->state;
  foo = htons((unsigned short)(event->serial));
  bcopy((char *)&foo, buf + 2, 2);
  bar = htonl((unsigned long)event->time);
  bcopy((char *)&bar, buf + 4, 4);
  bar = htonl((unsigned long)event->root);
  bcopy((char *)&bar, buf + 8, 4);
  bar = htonl((unsigned long)event->window);
  bcopy((char *)&bar, buf + 12, 4);
  bar = htonl((unsigned long)event->subwindow);
  bcopy((char *)&bar, buf + 16, 4);
  foo = htons((unsigned short)(event->x_root));
  bcopy((char *)&foo, buf + 20, 2);
  foo = htons((unsigned short)(event->y_root));
  bcopy((char *)&foo, buf + 22, 2);
  foo = htons((unsigned short)(event->x));
  bcopy((char *)&foo, buf + 24, 2);
  foo = htons((unsigned short)(event->y));
  bcopy((char *)&foo, buf + 26, 2);
  foo = htons((unsigned short)(event->keycode));
  bcopy((char *)&foo, buf + 28, 2);
#ifdef DEBUG
  fprintf(stderr, "event2buf: keycode = 0x%x\n", foo);
  fprintf(stderr, "event2buf: buf[28:29] = 0x%x\n", *(short *)(buf + 28));
#endif
  buf[30] = event->same_screen;
  buf[31] = 0;
}
