/*
 *	jilib -- $B$+$J4A;zJQ49MQ%i%$%V%i%j(J (Wnn Version4.0 $BBP1~HG(J)
 *		version 4.1
 *		ishisone@sra.co.jp
 */

/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 *		ishisone@sra.co.jp
 */

/*
 * Portability issue:
 *	jilib assumes that bcopy() can handle overlapping data blocks
 *	as xterm does.
 *	If your bcopy can't, you should define OVERLAP_BCOPY,
 *	which cause to use my own bcopy() instead of the
 *	one in /lib/libc.a. (but it's slow...)
 *
 *	If you use SysVR2, you should define SYSV to use /bin/mkdir
 *	to make directories rather than mkdir() system call.
 */

/*
 * $B35MW(J
 *
 * jilib $B$O(J jslib, jclib $B$N=i4|2=$r9T$&$?$a$N%i%$%V%i%j$G$"$k!#(J
 * $B<g$JL\E*$O4D6-=i4|2=%U%!%$%k(J (wnnevnrc) $B$rFI$s$G4D6-$N@_Dj$r(J
 * $B9T$J$&$3$H$G$"$k!#(J
 * jllib $B$r;H$&$D$b$j$J$i!"F1MM$N(J ($B$H$$$&$+!"$b$C$H9bEY$J(J) $B5!G=$O(J
 * jllib $B$KF~$C$F$$$k$N$G!"$3$N%i%$%V%i%j$r;H$&I,MW$O$J$$$@$m$&!#(J
 */

/*
 * Wnn Version 4 $BBP1~$K$"$?$C$F(J
 *
 * jilib $B$O$b$H$b$H(J Wnn Version 3 $B$N(J jclib $B$H6&$K;H$&$h$&$K:n$i$l$?(J
 * $B%i%$%V%i%j$G$"$k!#(Jjclib $B$r(J Wnn ver4 $BBP1~$K$9$k$K$"$?$C$F!"(Jjilib $B$b(J
 * $BBgI}$K=q$-49$($?!#(J
 * $B=>Mh$N$b$N$H$N8_49@-$O9M$($i$l$F$$$J$$!#(J
 */

/*
 * $B%a%b(J
 *
 * ver 0.0	89/08/10
 *	$B$H$j$"$($::n$j$O$8$a$k(J
 * ver 0.1	89/08/14
 *	$B0l1~$G$-$?(J
 * ver 0.2	89/08/22
 *	SYSV $B$@$C$?$i(J index/rindex $B$r(J strchr/strrchr $B$K$9$k(J
 * ver 0.3	89/08/26
 *	$B%5!<%P!"$*$h$S4D6-$,6&M-$5$l$k$h$&$K$9$k(J
 *	jiCreateEnv() $B$G(J js_dead_env_flg $B$NCM$,%j%9%H%"$5$l$J$$>l9g$,$"$C$?(J
 *	$B$N$G$=$l$r=$@5(J
 * ver 0.4	89/08/29
 *	$B%(%i!<$H%3%s%U%!!<%`$N%3!<%k%P%C%/$K(J client_data $B$N0z?t$rDI2C(J
 * ver 0.5	89/09/01
 *	$B%5!<%P$H4D6-$r(J linked list $B$G;}$D$h$&$KJQ99(J
 * ver 0.6	89/09/08
 *	Wnn-V4 $B$N?7$7$$%P!<%8%g%s$,Mh$?$i%i%$%V%i%j$N%$%s%?!<%U%'%$%9$,(J
 *	$B0c$C$F$$$?$N$G$=$l$K9g$o$;$F=$@5(J
 *	$B!&(Jjs_env_exist() $B$,DI2C$5$l$F$$$?$N$G(J envExist() $B$r$=$l$r;H$&$h$&$K(J
 *	  $B=$@5(J
 *	$B!&(Jjs_dic_add() $B$K(J rev $B0z?t!"(Jjs_dic_file_create(_client)() $B$K(J
 *	  type $B0z?t$,DI2C$5$l!"$D$$$G$K(J wnnenvrc $B$N(J setdic $B$K(J
 *	  $B$+$J4A;zJQ49(J / $B4A;z$+$JJQ49(J $B$N%U%i%0$,$D$$$?$N$G$=$l$K9g$o$;$F(J
 *	  $B=$@5(J
 * ver 0.7	89/10/16
 *	@WNN_DIC_DIR $B$H(J @ENV $B$NE83+$,DI2C$5$l$F$$$k$N$K5$$E$+$:!"(J
 *	$B%$%s%W%j%a%s%H$7$F$$$J$+$C$?$N$r=$@5(J
 *	$B$5$i$K!"(J@$B$NE83+$K$b$H$b$H%P%0$,$"$k$3$H$,H=L@$7$?$N$G$=$l$r=$@5(J
 * ver 0.8	89/10/16
 *	wnn-4.0.1 $B$G(J commonheader.h -> commonhd.h $B$K$J$C$?$N$G(J
 *	$B$=$l$N=$@5(J
 * ver 4.0	89/10/27
 *	$B%P!<%8%g%sHV9f$N=$@5(J
 * ver 4.1	89/11/02
 *	SysV $B$G$&$^$/%3%s%Q%$%k$G$-$J$$$N$r=$@5(J
 */

/*
 * $B%U%!%s%/%7%g%s(J
 *
 * jilib $B$,Ds6!$9$k%U%!%s%/%7%g%s$O<!$N(J4$B$D$G$"$k!#(J
 *
 * WNN_JSERVER_ID *jiOpenServer(char *servername, int timeout)
 *	servername $B$G;XDj$5$l$?%5!<%P$H@\B3$9$k!#(Jtimeout $B$G(J
 *	$BICC10L$G@\B3$N%?%$%`%"%&%H$r;XDj$9$k$3$H$,$G$-$k!#(J
 *	$B@\B3$G$-$?>l9g$O%5!<%P$N(J ID $B$,!"$G$-$J$+$C$?;~$K$O(J NULL $B$,JV$5$l$k!#(J
 *
 *	$B$[$H$s$I(J js_open() $B$HF1$8$@$,!"(Jservername $B$,(J NULL $B$"$k$$$O(J
 *	$B6uJ8;zNs(J ("") $B$G$"$C$?>l9g$K$O4D6-JQ?t(J JSERVER $B$NCM$r;HMQ$9$k(J
 *	$B$H$3$m!"$*$h$S$9$G$KF1$8%5!<%P$K@\B3$5$l$F$$$l$P$=$l$r6&M-(J
 *	$B$9$k$H$3$m$,(J js_open() $B$H0[$J$k!#$3$N$?$a!"(JjiOpenServer() $B$r(J
 *	$B;HMQ$7$F:n$C$?@\B3$r@Z$k$?$a$K$O!"(JjiCloseServer() $B$rMQ$$$J$/$F$O(J
 *	$B$J$i$J$$!#(J
 *
 * int jiCloseServer(WNN_JSERVER_ID *server)
 *	$B;XDj$5$l$?%5!<%P$H$N@\B3$r@Z$k!#$H$O$$$C$F$b%5!<%P;q8;$O(J
 *	$B6&M-$5$l$F$$$k$N$G!"%j%U%!%l%s%9%+%&%s%H$,(J 0 $B$K$J$i$J$$8B$j(J
 *	$B<B:]$K(J js_close() $B$,8F$P$l$k$3$H$O$J$$!#(J
 *
 *	$B;XDj$5$l$?%5!<%P$,(J jiOpenServer() $B$K$h$C$F@\B3$5$l$?$b$N$G$O(J
 *	$B$J$$>l9g!"$*$h$S(J js_close() $B$,<:GT$7$?;~$K$O(J -1 $B$,!"$=$&$G(J
 *	$B$J$$;~$K$O(J 0 $B$,JV$5$l$k!#(J
 *
 * WNN_ENV *jiCreateEnv(WNN_JSERVER_ID *server, char *envname,
 *			int override, char *wnnrcfile,
 *			void (*errmsgfunc)(), int (*confirmfunc)(),
 *			caddr_t client_data)
 *	server $B$G;XDj$5$l$?%5!<%P$K!"(Jenvname $B$H$$$&L>A0$N4D6-$r:n$k!#(J
 *	envname $B$,6uJ8;zNs$N;~$O!"%G%U%)%k%H$N4D6-L>$H$7$F%f!<%6L>$,(J
 *	$B;H$o$l$k!#(J(envname $B$,(J NULL $B$N;~$O!"4D6-L>$H$7$F$=$N$^$^(J NULL $B$,(J
 *	$B%5!<%P$KEO$5$l!"(Jjs_connect() $B$N%^%K%e%"%k$K$"$k$h$&$KB>$H6&M-(J
 *	$B$5$l$k$3$H$NL5$$4D6-$,:n$i$l$k(J)
 *
 *	jiOpenServer() $B$HF1$8$h$&$K!"F1$8%5!<%P$GF1$84D6-L>$N4D6-$O(J
 *	$B6&M-$5$l$k!#$=$N$?$a!"$3$N%U%!%s%/%7%g%s$r;H$C$F:n$C$?4D6-$r(J
 *	$B>C$9$K$O!"(JjiDeleteEnv() $B$r;HMQ$7$J$/$F$O$J$i$J$$!#(J
 *
 *	wnnrcfile $B$K$O4D6-=i4|2=%U%!%$%kL>$r;XDj$9$k!#(JjiCreateEnv() $B$O(J
 *	$B$=$NFbMF$K$7$?$,$C$F<-=q$dIUB08l%U%!%$%k$J$I$N@_Dj$r$9$k!#(J
 *	$B$b$7(J wnnrcfile $B$,(J NULL $B$J$i$P=i4|2=$N=hM}$O9T$J$o$J$$!#$^$?!"(J
 *	wnnrcfile $B$,6uJ8;zNs$G$"$C$?>l9g$K$O!"$^$:4D6-JQ?t(J WNNENVRC $B$N(J
 *	$BCM$,;HMQ$5$l$k!#$=$l$b$J$1$l$P%G%U%)%k%H$N(J /usr/local/lib/wnn/wnnenvrc
 *	$B$,;HMQ$5$l$k!#(J
 *
 *	$B%U%i%0(J override $B$,(J 0 $B$N;~$K$O!";XDj$5$l$?4D6-$,$9$G$KB8:_$7$F(J
 *	$B$$$l$P=i4|2=$N=hM}$r9T$J$o$J$$!#(J
 *
 *	errmsgfunc $B$*$h$S(J confirmfunc $B$O$H$b$K%3!<%k%P%C%/4X?t$G$"$k!#(J
 *	errmsgfunc $B$O%(%i!<$d7Y9p$N%a%C%;!<%8$rI=<($9$k;~$K<!$N7A<0$G8F$P$l$k!#(J
 *		(*errmsgfunc)(int type, char *message, caddr_t client_data);
 *	type $B$O%a%C%;!<%8$N%?%$%W$G!"(JTYPE_WARNING $B$H(J TYPE_ERROR $B$N$I$A$i$+(J
 *	$B$G$"$k!#(JTYPE_WARNING $B$OC1$J$k7Y9p$G!"=hM}$OB3$1$i$l$k!#$=$l$KBP$7(J
 *	TYPE_ERROR $B$N>l9g!"$=$N%(%i!<$,5/$-$?;~E@$G=hM}$OCf;_$5$l$k!#(J
 *	message $B$OI=<($5$l$k$Y$-%a%C%;!<%8$G$"$k!#$3$NJ8;zNs$N:G8e$K(J
 *	$B2~9T%3!<%I$OF~$C$F$$$J$$!#(J
 *	client_data $B$K$O(J jiCreateEnv() $B$N0z?t$G;XDj$5$l$?CM$,$=$N$^$^(J
 *	$BEO$5$l$k!#(J
 *	errmsgfunc $B$,(J NULL $B$N;~$O!"%a%C%;!<%8$OI8=`%(%i!<=PNO$K=q$+$l$k!#(J
 *
 *	confirmfunc $B$O!";XDj$5$l$?<-=q$,$J$+$C$?;~$K!"%f!<%6$KBP$7$F(J
 *	$B?7$?$K:n@.$9$k$+$I$&$+$r$?$:$M$k$?$a$K<!$N7A<0$G8F$S=P$5$l$k!#(J
 *		(*confirmfunc)(int type, char *file, caddr_t client_data);
 *	type $B$O:n$i$l$k%U%!%$%k$N7A<0$G!"(JTYPE_DIC $B$J$i<-=q!"(JTYPE_HINDO $B$J$i(J
 *	$BIQEY%U%!%$%k$G$"$k!#(Jfile $B$O%Q%9L>$G$"$k!#(J
 *	confirmfunc $B$O(J 0 $B$^$?$O(J 1 $B$NCM$rJV$5$J$/$F$O$J$i$J$$!#JV$5$l$?(J
 *	$BCM$,(J 0 $B$J$i%U%!%$%k$O:n$i$l$:!"$=$N<-=q$O;H$o$l$J$$!#(J1 $B$J$i$P(J
 *	$B%U%!%$%k$O?7$?$K:n$i$l$k!#(J
 *	client_data $B$K$O(J jiCreateEnv() $B$N0z?t$G;XDj$5$l$?CM$,$=$N$^$^(J
 *	$BEO$5$l$k!#(J
 *	confirmfunc $B$,(J NULL $B$N;~$O!">o$K?7$?$K%U%!%$%k$,:n$i$l$k!#(J
 *
 *	jiCreateEnv() $B$O%(%i!<$N;~$K$O(J NULL $B$r!"$=$&$G$J$1$l$P(J $B4D6-$N(J ID $B$r(J
 *	$BJV$9!#(J
 *
 * int jiDeleteEnv(WNN_ENV *env)
 *	$B;XDj$5$l$?4D6-$r>C$9!#4D6-$b%5!<%P$HF1$8$/6&M-$5$l$k$N$G(J
 *	$B%j%U%!%l%s%9%+%&%s%H$,(J 0 $B$K$J$i$J$$8B$j<B:]$K(J js_disconnect()
 *	$B$G%5!<%PFb$N4D6-$,>C5n$5$l$k$3$H$O$J$$!#(J
 *
 *	$B;XDj$5$l$?4D6-$,(J jiCreateEnv() $B$K$h$C$F:n$i$l$?$b$N$G$O(J
 *	$B$J$$>l9g!"$*$h$S(J js_disconnect() $B$,<:GT$7$?;~$K$O(J -1 $B$,!"$=$&$G(J
 *	$B$J$$;~$K$O(J 0 $B$,JV$5$l$k!#(J
 */

#ifndef lint
static char	*rcsid = "$Header: jilib.c,v 1.1 89/11/02 08:28:30 jim Exp $";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include "commonhd.h"
#include "config.h"
#include "jslib.h"
#include "jilib.h"

#ifdef CHECK_PROTOTYPE
#include "js.c.p"
#endif

#ifdef DEBUG
#define PRINT(a)	printf a
#else
#define PRINT(a)
#endif

#if defined(SYSV) || defined(USG)
#define index	strchr
#define rindex	strrchr
#endif

/* used by access(2) */
#ifndef R_OK
#define R_OK	4
#endif

#if !defined(SYSV) || defined(USG)
#define HAVE_MKDIRSYS
#endif

#define ERROR_LOAD	-2
#define NO_LOAD		-1

#define MAXARG	20

#define CONFIRM			1
#define CONFIRM1		2
#define CREATE_WITHOUT_CONFIRM	3
#define NO_CREATE		4

typedef struct _servrec {
	struct _servrec	*next;
	WNN_JSERVER_ID	*server;
	int		refcnt;
} ServerRec;

static ServerRec	*servers;

typedef struct _envrec {
	struct _envrec	*next;
	WNN_ENV		*env;
	char		envname[WNN_ENVNAME_LEN + 1];
	int		refcnt;
} EnvRec;

static EnvRec		*environs;

typedef struct {
	void	(*errorcall)();
	int	(*confirmcall)();
	caddr_t	client_data;
} CallbackRec;

#ifdef __STDC__
static WNN_JSERVER_ID *useServer(char *, int);
static void addServer(WNN_JSERVER_ID *);
static int unuseServer(WNN_JSERVER_ID *);
static WNN_ENV *findEnv(WNN_JSERVER_ID *, char *);
static char *getEnvName(WNN_ENV *);
static void addEnv(char *, WNN_ENV *);
static int deleteEnv(WNN_ENV *);
static int envExist(WNN_JSERVER_ID *, char *);
static int fileExist(WNN_ENV *, char *);
static int makeDir(WNN_ENV *, char *);
static int mkdirproc(WNN_ENV *, int, char *);
static int createDicFile(WNN_ENV *, char *, char *, char *);
static int createHindoFile(WNN_ENV *, char *, int, char *);
static char *getdicpasswd(char *);
static int fileLoad(WNN_ENV *, CallbackRec *, int,
		    char *, char *, char *, int, int *);
static char *expandDicPath(char *, WNN_ENV *);
static void expandTop(char *);
static void expandUSR(char *, WNN_ENV *);
static char *strinc(char *, char *);
static void error();
static int doconfirm(CallbackRec *, int, char *);
static int readEnvFile(WNN_ENV *, char *, CallbackRec *, int *);
static int procInclude(WNN_ENV *, CallbackRec *, int *, int, char **);
static int procSetDic(WNN_ENV *, CallbackRec *, int *, int, char **);
static int procSetFuzokugo(WNN_ENV *, CallbackRec *, int, char **);
static int procSetParam(WNN_ENV *, CallbackRec *, int, char **);
#else
static WNN_JSERVER_ID *useServer();
static void addServer();
static int unuseServer();
static WNN_ENV *findEnv();
static char *getEnvName();
static void addEnv();
static int deleteEnv();
static int envExist();
static int fileExist();
static int makeDir();
static int mkdirproc();
static int createDicFile();
static int createHindoFile();
static char *getdicpasswd();
static int fileLoad();
static char *expandDicPath();
static void expandTop();
static void expandUSR();
static char *strinc();
static void error();
static int doconfirm();
static int readEnvFile();
static int procInclude();
static int procSetDic();
static int procSetFuzokugo();
static int procSetParam();
#endif

extern char	*index();
extern char	*rindex();
extern char	*getenv();
extern char	*malloc();
extern char	*realloc();
extern void	free();
extern char	*strcpy();
extern char	*strncpy();
extern char	*strcat();
extern int	strcmp();
extern int	strncmp();

#ifdef OVERLAP_BCOPY
#undef bcopy
static
bcopy(from, to, n)
register char *from;
register char *to;
register int n;
{
	if (n <= 0 || from == to) return;

	if (from < to) {
		from += n;
		to += n;
		while (n-- > 0)
			*--to = *--from;
	} else {
		while (n-- > 0)
			*to++ = *from++;
	}
}
#endif

/* useServer -- $B;XDj$5$l$?L>A0$N%5!<%P$r;HMQ$G$-$k$h$&$K$9$k(J */
static WNN_JSERVER_ID *
useServer(servername, timeout)
char *servername;
int timeout;
{
	ServerRec	*srp;
	WNN_JSERVER_ID	*server;

	for (srp = servers; srp; srp = srp->next) {
		if (!strcmp(servername, srp->server->js_name)) {
			srp->refcnt++;
			return srp->server;
		}
	}

	PRINT(("useServer(): do js_open()\n"));
	if ((server = js_open(servername, timeout)) != NULL) {
		addServer(server);
	}

	return server;
}

/* addServer -- $B;XDj$5$l$?%5!<%P$r%j%9%H$KEPO?$9$k(J */
static void
addServer(server)
WNN_JSERVER_ID *server;
{
	ServerRec	*srp;

	if ((srp = (ServerRec *)malloc(sizeof(ServerRec))) != NULL) {
		srp->next = servers;
		servers = srp;
		srp->server = server;
		srp->refcnt = 1;
	}
}

/* unuseServer -- $B;H$o$l$J$/$J$C$?%5!<%P$N=hM}$r$9$k(J */
static int
unuseServer(server)
WNN_JSERVER_ID *server;
{
	ServerRec	*srp, *srp0;
	int		ret;

	for (srp = servers, srp0 = NULL; srp; srp0 = srp, srp = srp->next) {
		if (server == srp->server) {
			if (--srp->refcnt <= 0) {
				PRINT(("unuseServer(): do js_close()\n"));
				ret = js_close(server);
				if (srp0 == NULL) {
					servers = srp->next;
				} else {
					srp0->next = srp->next;
				}
				(void)free((char *)srp);
			}
			return ret;
		}
	}
	return -1;
}

/* findEnv -- $B;XDj$5$l$?L>A0$N4D6-$,$"$k$+!"%j%9%H$rD4$Y$k(J */
static WNN_ENV *
findEnv(server, envname)
WNN_JSERVER_ID *server;
char *envname;
{
	EnvRec	*envp;

	if (envname == NULL) return NULL;

	for (envp = environs; envp; envp = envp->next) {
		if (server == envp->env->js_id &&
		    !strcmp(envname, envp->envname)) {
			envp->refcnt++;
			return envp->env;
		}
	}

	return NULL;
}

/* getEnvName -- $B4D6-L>$rJV$9(J */
static char *
getEnvName(env)
WNN_ENV *env;
{
	EnvRec	*envp;

	if (env == NULL) return NULL;

	for (envp = environs; envp; envp = envp->next) {
		if (env == envp->env)
			return envp->envname;
	}

	return NULL;
}

/* addEnv -- $B;XDj$5$l$?4D6-$r%j%9%H$KEPO?$9$k(J */
static void
addEnv(envname, env)
char *envname;
WNN_ENV *env;
{
	EnvRec	*envp;

	if ((envp = (EnvRec *)malloc(sizeof(EnvRec))) != NULL) {
		envp->next = environs;
		environs = envp;
		envp->env = env;
		if (envname != NULL) {
			(void)strncpy(envp->envname, envname, WNN_ENVNAME_LEN);
		} else {
			envp->envname[0] = '\0';
		}
		envp->refcnt = 1;
	}
}

/* deleteEnv -- $B;H$o$l$J$/$J$C$?4D6-$N=hM}$r$9$k(J */
static int
deleteEnv(env)
WNN_ENV *env;
{
	EnvRec	*envp, *envp0;
	int	ret;

	for (envp0 = NULL, envp = environs; envp; envp0 = envp, envp = envp->next) {
		if (env == envp->env) {
			if (--envp->refcnt <= 0) {
				PRINT(("deleteEnv(): do js_disconnect()\n"));
				ret = js_disconnect(env);
				if (envp0 == NULL) {
					environs = envp->next;
				} else {
					envp0->next = envp->next;
				}
				(void)free((char *)envp);
			}
			return ret;
		}
	}
	return -1;
}

/* envExist -- $B;XDj$5$l$?L>A0$N4D6-$,$"$k$+$I$&$+D4$Y$k(J */
static int
envExist(server, envname)
WNN_JSERVER_ID *server;
char *envname;
{
	return js_env_exist(server, envname);
}

/* fileExist -- $B;XDj$5$l$?L>A0$N%U%!%$%k$,$"$k$+$I$&$+D4$Y$k(J */
static int
fileExist(env, file)
WNN_ENV *env;
char *file;
{
	int	ret;

	if (*file == C_LOCAL) {
		ret = access(file + 1, R_OK);
	} else {
		ret = js_access(env, file, R_OK);
	}

	return ret >= 0;
}

/* makeDir -- $B;XDj$5$l$?%G%#%l%/%H%j$r:n$k(J */
static int
makeDir(env, dir)
WNN_ENV *env;
char *dir;
{
	char	c;
	char	*p, *q;
	int	local = 0;

	if (*dir == C_LOCAL) {
		local++;
		dir++;
	}

	/* $B$9$G$K$"$k$+$I$&$+D4$Y$k(J */
	if (local) {
		if (access(dir, R_OK) >= 0)
			return 0;
	} else {
		if (js_access(env, dir, R_OK) >= 0)
			return 0;
		else if (wnn_errorno == WNN_JSERVER_DEAD)
			return -1;
	}

	/* $B>e$+$i:n$C$F$$$/(J */
	/* $B$5$9$,$K(J / $B$O:n$kI,MW$,$J$$$H;W$o$l$k(J */
	q = (*dir == '/') ? dir + 1 : dir;

	while (p = index(q, '/')) {
		c = *p; *p = '\0';
		if (mkdirproc(env, local, dir) < 0)
			return -1;
		*p = c;
		q = p + 1;
	}
	if (*q != '\0' && mkdirproc(env, local, dir) < 0)
		return -1;

	return 0;
}

static int
mkdirproc(env, local, dir)
WNN_ENV *env;
int local;
char *dir;
{
	if (local) {
		/* $B$^$:$O$"$k$+$I$&$+D4$Y$k(J */
		if (access(dir, R_OK) >= 0)
			return 0;
#ifdef HAVE_MKDIRSYS
		return mkdir(dir, 0777);
#else
		{
		char	buf[1024];	/*enough?*/
		(void)sprintf(buf, "/bin/mkdir %s", dir);
		if (system(buf) != 0)
			return -1;
		else
			return 0;
		}
#endif
	} else {
		if (js_access(env, dir, R_OK) >= 0)
			return 0;
		return js_mkdir(env, dir);
	}
}

static int
createDicFile(env, file, dpasswd, hpasswd)
WNN_ENV *env;
char *file;
char *dpasswd;
char *hpasswd;
{
	PRINT(("createDicFile(env, %s, %s, %s)\n", file,
	       dpasswd ? dpasswd : "-", hpasswd ? hpasswd : "-"));

	if (*file == C_LOCAL) {
		return js_dic_file_create_client(env, file + 1, WNN_REV_DICT,
						 NULL, dpasswd, hpasswd);
	} else {
		return js_dic_file_create(env, file, WNN_REV_DICT,
					  NULL, dpasswd, hpasswd);
	}
}

static int
createHindoFile(env, file, dicfid, hpasswd)
WNN_ENV *env;
char *file;
int dicfid;
char *hpasswd;
{
	PRINT(("createHindoFile(env, %s, %d, %s)\n", file, dicfid,
	       hpasswd ? hpasswd : "-"));

	if (*file == C_LOCAL) {
		return js_hindo_file_create_client(env, dicfid, file + 1,
						   NULL, hpasswd);
	} else {
		return js_hindo_file_create(env, dicfid, file, NULL, hpasswd);
	}
}

/* getdicpasswd -- $B;XDj$5$l$?%U%!%$%k$+$i%Q%9%o!<%I$r<h$j=P$9(J */
static char *
getdicpasswd(file)
char *file;	/* $B%U%!%$%kL>(J $B$?$@$7%Q%9%o!<%I$,>e=q$-$5$l$k(J */
{
	FILE	*fp = fopen(file, "r");
	int	len;

	PRINT(("getdicpasswd(%s)\n", file));
	/* file$BL>$N0z?t$K%Q%9%o!<%I$r>e=q$-$9$k(J
	 * $B1x$$$1$I!"$^$"$$$$$+(J
	 */
	if (fp == NULL) return NULL;
	if (fgets(file, 256, fp) == NULL) {
		(void)fclose(fp);
		return NULL;
	}

	len = strlen(file);
	if (file[len - 1] == '\n') file[len - 1] = '\0';

	(void)fclose(fp);
	PRINT(("getdicpasswd(): password=%s\n", file));
	return file;
}

/* fileLoad -- $B%U%!%$%k$r%m!<%I$9$k(J $B$b$7I,MW$,$"$l$P?7$?$K:n@.$9$k(J */
static int
fileLoad(env, cbrec, type, file, dpasswd, hpasswd,
	 dicfid, confirmp)
WNN_ENV *env;
CallbackRec *cbrec;
int type;
char *file;
char *dpasswd;
char *hpasswd;
int dicfid;
int *confirmp;
{
	int	fid;
	char	*filep;

	if (!fileExist(env, file)) {
		switch (*confirmp) {
		case NO_CREATE:
			error(cbrec, TYPE_WARNING, "setdic: %s doesn't exist",
			      file);
			return NO_LOAD;
		case CONFIRM:
			if (!doconfirm(cbrec, type, file)) {
				return NO_LOAD;
			}
			break;
		case CONFIRM1:
			if (!doconfirm(cbrec, type, file)) {
				*confirmp = NO_CREATE;
				return NO_LOAD;
			}
			*confirmp = CREATE_WITHOUT_CONFIRM;
			break;
		}
		/* $B%U%!%$%k$r:n$k(J */
		if (filep = rindex(file, '/')) {
			char	c = *filep;
			/* $B%G%#%l%/%H%j$r:n$k(J */
			*filep = '\0';
			if (makeDir(env, file) < 0) {
				error(cbrec, TYPE_ERROR,
				      "setdic: can't make directory %s",
				      file);
				return ERROR_LOAD;
			}
			*filep++ = c;
		}

		if (type == TYPE_DIC) {
			if (createDicFile(env, file, dpasswd, hpasswd) < 0) {
				error(cbrec, TYPE_ERROR,
				      "setdic: can't create dic (%s)",
				      file);
				return ERROR_LOAD;
			}
		} else {
			if (createHindoFile(env, file, dicfid, hpasswd) < 0) {
				error(cbrec, TYPE_ERROR,
				      "setdic: can't create hindo (%s)",
				      file);
				return ERROR_LOAD;
			}
		}
	}

	if (*file == C_LOCAL) {
		fid = js_file_send(env, file + 1);
	} else {
		fid = js_file_read(env, file);
	}

	if (fid < 0) {
		error(cbrec, TYPE_ERROR, "setdic: can't load %s", file);
		return ERROR_LOAD;
	}

	return fid;
}

/* expandDicPath -- $B<-=q$N%Q%9L>$NE83+$r$9$k(J */
static char *
expandDicPath(path, env)
char *path;
WNN_ENV *env;
{
	/* $B;O$a$K@hF,$rE83+$7$F(J */
	expandTop(path);
	/* $B$"$H$O(J @USR $B$rE83+$9$k(J */
	expandUSR(path, env);
	return path;
}

static void
expandTop(path)
char *path;
{
	char	namebuf[256];
	char	tmp[256];
	char	*name;
	char	*p;
	char	*svpath = path;
	struct passwd	*pwp;
	int	local = 0;

	if (*path == C_LOCAL) {
		local = 1;
		path++;
	}

	p = NULL;

	switch (*path) {
	case '@':
		path++;
		if (!strncmp(path, "HOME", 4)) {
			if ((p = getenv("HOME")) == NULL)
				p = "";
			path += 4;
		} else if (!strncmp(path, "LIBDIR", 6)) {
			p = LIBDIR;
			path += 6;
		} else if (!strncmp(path, "WNN_DIC_DIR", 11)) {
			if ((p = getenv("HOME")) == NULL) {
				p = "";
			} else {
				(void)strcpy(tmp, p);
				(void)strcat(tmp, "/");
				if ((p = getenv("WNN_DIC_DIR")) == NULL) {
					(void)strcat(tmp, "Wnn");
				} else {
					(void)strcat(tmp, p);
				}
				p = tmp;
			}
			path += 11;
		}
		break;
	case '~':
		if (*++path == '/') {
			pwp = getpwuid(getuid());
		} else {
			char	*cp;
			char	c;

			cp = path;
			while (*cp && *cp != '/')
				cp++;
			c = *cp;
			*cp = '\0';
			pwp = getpwnam(path);
			*cp = c;
			path = cp;
		}
		if (pwp == NULL)
			p = "";
		else
			p = pwp->pw_dir;
		break;
	}

	if (p != NULL) {
		if (strlen(path) + strlen(p) + local > 255) {
			return;
		}
		name = namebuf;
		if (local) *name++ = C_LOCAL;
		(void)strcpy(name, p);
		(void)strcat(name, path);
		(void)strcpy(svpath, namebuf);
	}
}

static void
expandUSR(path, env)
char *path;
WNN_ENV *env;
{
	static char	myname[10];
	register char	*t;
	int		mynamelen;
	char		*envname;
	int		envnamelen;

	/* @USR $B$NE83+$r$9$k(J */
	if (myname[0] == '\0') {
		(void)strcpy(myname, getpwuid(getuid())->pw_name);
	}
	mynamelen = strlen(myname);
	t = path;
	while (t = strinc(t, "@USR")) {
		if (strlen(path) + mynamelen - 4 > 255) return;

		(void)bcopy(t + 4, t + mynamelen, strlen(t + 4) + 1);
		(void)bcopy(myname, t, mynamelen);
	}

	/* @ENV $B$NE83+$r$9$k(J */
	envname = NULL;
	t = path;
	while (t = strinc(t, "@ENV")) {
		if (envname == NULL) {
			envname = getEnvName(env);
			if (envname == NULL) envname = "";
			envnamelen = strlen(envname);
		}
		if (strlen(path) + envnamelen - 4 > 255) return;

		(void)bcopy(t + 4, t + envnamelen, strlen(t + 4) + 1);
		(void)bcopy(envname, t, envnamelen);
	}
}

static char *
strinc(s1, s2)
char *s1;
char *s2;
{
	int	c = *s2;
	int	len = strlen(s2);

	while (s1 = index(s1, c)) {
		if (!strncmp(s1, s2, len))
			return s1;
		s1++;
	}
	return NULL;
}

/* parseLine -- $B%3%^%s%I9T$N%Q!<%8%s%0(J */
static int
parseLine(line, argv, argsize)
char *line;
char **argv;
int argsize;
{
	int	argc = 0;

	while (argc < argsize) {
		/* $B6uGr$N%9%-%C%W(J */
		while (*line <= ' ') {
			if (*line == '\0' || *line == '\n')
				return argc;
			line++;
		}
		argv[argc++] = line++;
		while (*line++ > ' ')
			;
		line--;
		if (*line == '\0' || *line == '\n') {
			*line = '\0';
			return argc;
		}
		*line++ = '\0';
	}
	return argc;
}


/* VARARGS3 */
static void
error(cbrec, level, s, a, b, c, d, e)
CallbackRec *cbrec;
int level;
char *s;
{
	char	tmp[1024];
	void (*callback)() = cbrec->errorcall;

	(void)sprintf(tmp, s, a, b, c, d, e);

	if (callback) {
		(*callback)(level, tmp, cbrec->client_data);
	} else {
		fprintf(stderr, "%s: %s\n",
			level == TYPE_ERROR ? "Error" : "Warning", tmp);
	}
}

static int
doconfirm(cbrec, type, file)
CallbackRec *cbrec;
int type;
char *file;
{
	int	(*callback)() = cbrec->confirmcall;

	return (callback ? (*callback)(type, file, cbrec->client_data) : 1);
}

/*
 *	$B%Q%V%j%C%/$J%U%!%s%/%7%g%s(J
 */

/* jiOpenServer -- $B%5!<%P$r%*!<%W%s$9$k(J */
WNN_JSERVER_ID *
jiOpenServer(servername, timeout)
char *servername;
int timeout;
{
	extern char	*getenv();

	/*
	 * servername $B$,(J NULL $B$^$?$O6uJ8;zNs$@$C$?>l9g$O!"(J
	 * $B4D6-JQ?t(J 'JSERVER' $B$r;HMQ$9$k(J
	 */
	if (servername == NULL || *servername == '\0') {
		servername = getenv(WNN_JSERVER_ENV);
	}
	PRINT(("jiOpenServer(): servername=%s\n",
	       servername ? servername : "<NULL>"));
	return useServer(servername, timeout);
}

/* jiCloseServer -- $B%5!<%P$r%/%m!<%:$9$k(J */
int
jiCloseServer(server)
WNN_JSERVER_ID *server;
{
	PRINT(("jiCloseServer(): servername=%s\n",
	       server->js_name));
	return unuseServer(server);
}

/* jiCreateEnv -- $B;XDj$5$l$?4D6-$r:n$k(J */
WNN_ENV *
jiCreateEnv(server, envname, override, wnnrcfile, errmsgfunc, confirmfunc, client_data)
WNN_JSERVER_ID *server;
char *envname;
int override;
char *wnnrcfile;
void (*errmsgfunc)();
int (*confirmfunc)();
caddr_t client_data;
{
	int		noInitialize = 0;
	WNN_ENV		*env;
	int		confirm;
	int		saveflag;
	CallbackRec	callbackrec;

	callbackrec.errorcall = errmsgfunc;
	callbackrec.confirmcall = confirmfunc;
	callbackrec.client_data = client_data;

	/*
	 * envname $B$,6uJ8;zNs(J (i.e. *envname == '\0') $B$@$C$?>l9g$K$O(J
	 * $B%f!<%6L>$r;H$&(J ($B$3$NJU$O(J uum $B$K9g$o$;$F$"$k(J)
	 * envname $B$,(J NULL $B$N>l9g$K$O(J NULL $B$N$^$^(J
	 */
	if (envname != NULL && *envname == '\0') {
		envname = getpwuid(getuid())->pw_name;
	}
	PRINT(("jiCreateEnv(): envname=%s\n", envname ? envname : "<NULL>"));

	/* $B$9$G$K:n$i$l$F$$$k$+$I$&$+D4$Y$k(J */
	env = findEnv(server, envname);
	if (env != NULL && !override) {
		return env;
	}

	/* $B$H$j$"$($:(J jserver $B$,;`$s$G$$$k$+D4$Y$k(J */
	if (server->js_dead) {
		error(&callbackrec, TYPE_ERROR, "JSERVER is DEAD");
		wnn_errorno = WNN_JSERVER_DEAD;
		return NULL;
	}

	/* jserver $B$,;`$s$@;~$N=hM}(J
	 * longjmp $B$GLa$C$F$/$k$H$3$m$r@_Dj$9$k(J
	 * $B$3$&$7$F$*$/$3$H$G!"0J9_(J jserver $B$,;`$s$@$+$I$&$+$N%A%'%C%/$,(J
	 * $BITMW$K$J$k(J
	 * $B$?$@$7!">e0L$N4X?t$G$9$G$K@_Dj$5$l$F$$$k$H$-$K$O$=$l$KG$$;$k(J
	 */
	if ((saveflag = server->js_dead_env_flg) == 0) {
		server->js_dead_env_flg = 1;
		if (setjmp(server->js_dead_env)) {
			error(&callbackrec, TYPE_ERROR, "JSERVER is DEAD");
			server->js_dead_env_flg = 0;
			wnn_errorno = WNN_JSERVER_DEAD;
			return NULL;
		}
	}

	if (!override && envExist(server, envname) == 1) {
		/* $B$9$G$K;XDj$5$l$?L>A0$N4D6-$,B8:_$9$k(J */
		PRINT(("jcCreateEnv(): %s already exists\n", envname));
		noInitialize = 1;
	}

	/* $B4D6-$r:n$k(J */
	if ((env = js_connect(server, envname)) == NULL) {
		error(&callbackrec, TYPE_ERROR, "can't conenct to environment");
		server->js_dead_env_flg = saveflag;
		return NULL;
	}

	/*
	 * wnnrcfile $B$,6uJ8;zNs(J (i.e. *wnnrcfile == '\0') $B$@$C$?>l9g$K$O(J
	 * $B$^$:4D6-JQ?t(J 'WNNENVRC' $B$r;H$&(J
	 * $B$=$l$b$J$1$l$P%G%U%)%k%H$N(J /usr/local/lib/wnn/wnnenvrc $B$r(J
	 * $B;H$&(J
	 * wnnrcfile $B$,(J NULL $B$N>l9g$K$O=i4|2=$O9T$J$o$J$$(J
	 */

	if (noInitialize || wnnrcfile == NULL) {
		/* $B4D6-%j%9%H$KF~$l$F$*$/(J */
		addEnv(envname, env);
		server->js_dead_env_flg = saveflag;
		return env;
	}

	if (*wnnrcfile == '\0') {
		if ((wnnrcfile = getenv("WNNENVRC")) == NULL ||
		    access(wnnrcfile, R_OK) < 0) {
			wnnrcfile = ENVRCFILE;	/* defined in 'config.h' */
		}
	}
	PRINT(("jiCreateEnv(): wnnrcfile=%s\n", wnnrcfile));

	/* $B=i4|2=$9$k(J */
	confirm = CONFIRM;
	if (readEnvFile(env, wnnrcfile, &callbackrec, &confirm) < 0) {
		(void)js_disconnect(env);
		server->js_dead_env_flg = saveflag;
		return NULL;
	}

	/* $B4D6-%j%9%H$KF~$l$F$*$/(J */
	addEnv(envname, env);

	/* $B85$KLa$7$F$*$/(J */
	server->js_dead_env_flg = saveflag;

	return env;
}

/* jiDeleteEnv -- $B4D6-$r>C$9(J */
int
jiDeleteEnv(env)
WNN_ENV *env;
{
	PRINT(("jiDeleteEnv()\n"));
	return deleteEnv(env);
}

/*
 *	$B4D6-@_Dj$N%U%!%s%/%7%g%s(J
 */

static int
readEnvFile(env, file, cbrec, confirmp)
WNN_ENV *env;
char *file;
CallbackRec *cbrec;
int *confirmp;
{
	char	line[512];
	FILE	*fp;
	FILE	*fopen();
	int	argc;
	char	*argv[MAXARG];
	int	abort;

	if ((fp = fopen(file, "r")) == NULL) {
		error(cbrec, TYPE_ERROR, "can't open %s", file);
		return -1;
	}

	while (fgets(line, sizeof(line), fp)) {
		if (*line == '\0' || *line == '\n' || *line == ';')
			continue;
		PRINT(("readEnvFile(): doing line %s", line));

		/* parsing */
		argc = parseLine(line, argv, MAXARG);
		if (argc < 1)
			continue;

		abort = 0;
		if (!strcmp(argv[0], "include")) {
			abort = procInclude(env, cbrec, confirmp, argc, argv);
		} else if (!strcmp(argv[0], "setdic")) {
			abort = procSetDic(env, cbrec, confirmp, argc, argv);
		} else if (!strcmp(argv[0], "setfuzokugo")) {
			abort = procSetFuzokugo(env, cbrec, argc, argv);
		} else if (!strcmp(argv[0], "setparam")) {
			abort = procSetParam(env, cbrec, argc, argv);
		} else if (!strcmp(argv[0], "confirm")) {
			*confirmp = CONFIRM;
		} else if (!strcmp(argv[0], "confirm1")) {
			*confirmp = CONFIRM1;
		} else if (!strcmp(argv[0], "create_without_confirm")) {
			*confirmp = CREATE_WITHOUT_CONFIRM;
		} else if (!strcmp(argv[0], "no_create")) {
			*confirmp = NO_CREATE;
		}
		if (abort) {
			(void)fclose(fp);
			return -1;
		}
	}
	(void)fclose(fp);
	return 0;
}

/*
 * $B%3%^%s%I=hM}%k!<%A%s(J
 *
 *	procInclude()		include $B$N=hM}(J
 *	procSetDic()		setdic $B$N=hM}(J
 *	procSetFuzokugo()	setfuzokugo $B$N=hM}(J
 *	procSetParam()		setparam $B$N=hM}(J
 */

static int
procInclude(env, cbrec, confirmp, ac, av)
WNN_ENV *env;
CallbackRec *cbrec;
int *confirmp;
int ac;
char **av;
{
	char	tmp[256];

	if (ac < 2) {
		error(cbrec, TYPE_ERROR, "include: filename expected");
		return -1;
	}

	(void)strcpy(tmp, av[1]);
	expandTop(tmp);
	return readEnvFile(env, tmp, cbrec, confirmp);
}

static int
procSetDic(env, cbrec, confirmp, ac, av)
WNN_ENV *env;
CallbackRec *cbrec;
int *confirmp;
int ac;
char **av;
{
	char	dicfilebuf[256], hindofilebuf[256];
	char	dicpasswdbuf[256], hindopasswdbuf[256];
	char	*dicfilename, *hindofilename;
	int	prio, dicro, hindoro, rev;
	int	dicid, hindoid;
	char	*dicpasswd, *hindopasswd;

	if (ac < 2) {
		error(cbrec, TYPE_ERROR, "setdic: dic filename expected");
		return -1;
	}
	
	(void)strcpy(dicfilebuf, av[1]);
	dicfilename = expandDicPath(dicfilebuf, env);

	/* $B%G%U%)%k%H$NCM(J */
	hindofilename = NULL;
	prio = 5;
	dicro = hindoro = 0;
	rev = 0;
	dicpasswd = hindopasswd = NULL;

	if (ac > 3 && strcmp(av[2], "-")) {
		(void)strcpy(hindofilebuf, av[2]);
		hindofilename = expandDicPath(hindofilebuf, env);
	}
	if (ac > 4) {
		prio = atoi(av[3]);
	}
	if (ac > 5) {
		dicro = atoi(av[4]);
	}
	if (ac > 6) {
		hindoro = atoi(av[5]);
	}
	if (ac > 7 && strcmp(av[6], "-")) {
		(void)strcpy(dicpasswdbuf, av[6]);
		dicpasswd = getdicpasswd(expandDicPath(dicpasswdbuf, env));
		/* $B%Q%9%o!<%I$,FI$a$J$/$F$b!"%(%i!<$K$O$7$J$$(J */
		if (dicpasswd == NULL) {
			error(cbrec, TYPE_WARNING,
			      "can't read password for %s", dicfilename);
		}
	}
	if (hindofilename && ac > 8 && strcmp(av[7], "-")) {
		(void)strcpy(dicpasswdbuf, av[7]);
		hindopasswd = getdicpasswd(expandDicPath(hindopasswdbuf, env));
		/* $B%Q%9%o!<%I$,FI$a$J$/$F$b!"%(%i!<$K$O$7$J$$(J */
		if (hindopasswd == NULL) {
			error(cbrec, TYPE_WARNING,
			      "can't read password for %s", hindofilename);
		}
	}
	if (ac > 9) {
		rev = atoi(av[8]);
	}

	dicid = hindoid = -1;

	if ((dicid = fileLoad(env, cbrec, TYPE_DIC, dicfilename,
			      dicpasswd, hindopasswd, -1, confirmp)) == ERROR_LOAD) {
		return -1;
	} else if (dicid == NO_LOAD) {
		return 0;
	}

	if (hindofilename) {
		/* $BIQEY%U%!%$%k$,;XDj$5$l$F$$$k;~!"(J
		 * confirm $B$5$l$J$$$?$a$KIQEY%U%!%$%k$,:n$i$l$J$+$C$?$J$i$P(J
		 * $BC1$K<-=q$r;H$o$J$$$@$1(J
		 * confirm $B$5$l$?$K$b$+$+$o$i$::n$i$l$J$+$C$?$J$i$P(J
		 * $B%(%i!<$G%"%\!<%H$9$k(J
		 */
		hindoid = fileLoad(env, cbrec, TYPE_HINDO,
				   hindofilename, NULL, hindopasswd,
				   dicid, confirmp);
		if (hindoid == ERROR_LOAD) {
			(void)js_file_discard(env, dicid);
			return -1;
		} else if (hindoid == NO_LOAD) {
			return 0;
		}
	}

	if (js_dic_add(env, dicid, hindoid, rev, prio, dicro, hindoro,
		       dicpasswd, hindopasswd) < 0) {
		error(cbrec, TYPE_ERROR, "setdic: can't add dic (%s)",
		      dicfilename);
		return -1;
	}

	return 0;
}

static int
procSetFuzokugo(env, cbrec, ac, av)
WNN_ENV *env;
CallbackRec *cbrec;
int ac;
char **av;
{
	char	filebuf[256];
	int	fid;

	if (ac < 2) {
		error(cbrec, TYPE_ERROR, "setfuzokugo: filename expected");
		return -1;
	}
	
	(void)strcpy(filebuf, av[1]);
	(void)expandDicPath(filebuf, env);

	if (*filebuf == C_LOCAL) {
		fid = js_file_send(env, filebuf + 1);
	} else {
		fid = js_file_read(env, filebuf);
	}
	if (fid < 0) {
		error(cbrec, TYPE_ERROR, "setfuzokugo: can't load %s", filebuf);
		return -1;
	}

	if (js_fuzokugo_set(env, fid) < 0) {
		error(cbrec, TYPE_ERROR, "setfuzokugo: can't set fuzokugo-file");
		return -1;
	}

	return 0;
}

static int
procSetParam(env, cbrec, ac, av)
WNN_ENV *env;
CallbackRec *cbrec;
int ac;
char **av;
{
	struct wnn_param	param;

	if (ac < 2) {
		error(cbrec, TYPE_ERROR, "setparam: no parameter");
		return -1;
	}

	/* $B;XDj$5$l$?%Q%i%a!<%?$N?t$,>/$J$$$H$-$O!";XDj$5$l$F$J$$(J
	 * $B%Q%i%a!<%?$NCM$O8=:_$N%Q%i%a!<%?$HF1$8$K$9$k(J
	 */
	if (ac < 18 && js_param_get(env, &param) < 0) {
		error(cbrec, TYPE_ERROR,
		      "setparam: can't get default value");
		return -1;
	}

	/* $B%Q%i%a!<%?$NCM$NBeF~(J */
	switch (ac) {
	case 18: param.p15 = atoi(av[17]);
	case 17: param.p14 = atoi(av[16]);
	case 16: param.p13 = atoi(av[15]);
	case 15: param.p12 = atoi(av[14]);
	case 14: param.p11 = atoi(av[13]);
	case 13: param.p10 = atoi(av[12]);
	case 12: param.p9 = atoi(av[11]);
	case 11: param.p8 = atoi(av[10]);
	case 10: param.p7 = atoi(av[9]);
	case  9: param.p6 = atoi(av[8]);
	case  8: param.p5 = atoi(av[7]);
	case  7: param.p4 = atoi(av[6]);
	case  6: param.p3 = atoi(av[5]);
	case  5: param.p2 = atoi(av[4]);
	case  4: param.p1 = atoi(av[3]);
	case  3: param.nsho = atoi(av[2]);
	case  2: param.n = atoi(av[1]);
	}

	if (js_param_set(env, &param) < 0) {
		error(cbrec, TYPE_ERROR, "setparam: can't set parameters");
		return -1;
	}

	return 0;
}
