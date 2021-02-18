/* ************************************************************ *\
 *								*
 *    Common support routines for sockets			*
 *								*
 *       James L. Peterson	 				*
 *	(c) Copyright MCC, 1987                                 *
 * 				  				*
 * 				  				*
 \* *********************************************************** */

#include "scope.h"

/* ********************************************** */
/*						  */
/*       Debugging support routines               */
/*						  */
/* ********************************************** */

enterprocedure(s)
     char   *s;
{
  debug(2,(stderr, "-> %s\n", s));
}

warn(s)
     char   *s;
{
  fprintf(stderr, "####### %s\n", s);
}

panic(s)
     char   *s;
{
  fprintf(stderr, "%s\n", s);
  exit(1);
}

/* ********************************************** */
/*						  */
/*  Debugging forms of memory management          */
/*						  */
/* ********************************************** */

extern char *malloc();

char   *Malloc (n)
     long    n;
{
  char   *p;
  p = (char *)malloc((unsigned int)n);
  debug(64,(stderr, "%x = malloc(%d)\n", p, n));
  if (p == NULL)
    panic("no more malloc space");
  return(p);
}

Free(p)
     char   *p;
{
  debug(64,(stderr, "%x = free\n", p));
  free(p);
}



/* ************************************************************ */
/*								*/
/*    Signal Handling support					*/
/*								*/
/* ************************************************************ */

#include <signal.h>

SignalURG()
{
  debug(1,(stderr, "==> SIGURG received\n"));
}

SignalPIPE()
{
  debug(1,(stderr, "==> SIGPIPE received\n"));
}

SignalINT()
{
  debug(1,(stderr, "==> SIGINT received\n"));
  exit(1);
}

SignalQUIT()
{
  debug(1,(stderr, "==> SIGQUIT received\n"));
  exit(1);
}

SignalTERM()
{
  debug(1,(stderr, "==> SIGTERM received\n"));
  exit(1);
}

SignalTSTP()
{
  debug(1,(stderr, "==> SIGTSTP received\n"));
}

SignalCONT()
{
  debug(1,(stderr, "==> SIGCONT received\n"));
}

SetSignalHandling()
{
  enterprocedure("SetSignalHandling");
  signal(SIGURG, SignalURG);
  signal(SIGPIPE, SignalPIPE);
  signal(SIGINT, SignalINT);
  signal(SIGQUIT, SignalQUIT);
  signal(SIGTERM, SignalTERM);
  signal(SIGTSTP, SignalTSTP);
  signal(SIGCONT, SignalCONT);
}



/* ************************************************************ */
/*								*/
/*   Create a socket for a service to listen for clients        */
/*								*/
/* ************************************************************ */

#include <sys/types.h>	       /* needed by sys/socket.h and netinet/in.h */
#include <sys/uio.h>	       /* for struct iovec, used by socket.h */
#include <sys/socket.h>	       /* for AF_INET, SOCK_STREAM, ... */
#include <sys/ioctl.h>	       /* for FIONCLEX, FIONBIO, ... */
#include <netinet/in.h>	       /* struct sockaddr_in */
#include <netdb.h>	       /* struct servent * and struct hostent *  */

static int  ON = 1 /* used in ioctl */ ;
#define	BACKLOG	5

/* for use in the UsingFD call -- defined later */
extern int  NewConnection ();


SetUpConnectionSocket(iport)
     int     iport;
{
  FD ConnectionSocket;
  struct sockaddr_in  sin;
  short	port;

  enterprocedure("SetUpConnectionSocket");

  /* create the connection socket and set its parameters of use */
  ConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (ConnectionSocket < 0)
    {
      perror("socket");
      exit(-1);
    }
  (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_REUSEADDR,   (char *)NULL, 0);
  (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_USELOOPBACK, (char *)NULL, 0);
#ifdef SO_DONTLINGER
  (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_DONTLINGER,  (char *)NULL, 0);
#endif

  /* define the name and port to be used with the connection socket */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;

  /* the address of the socket is composed of two parts: the host machine and
     the port number.  We need the host machine address for the current host
   */
  {
    /* define the host part of the address */
    char    MyHostName[256];
    struct hostent *hp;

    (void) gethostname(MyHostName, sizeof(MyHostName));
    ScopeHost = (char *) Malloc((long)strlen(MyHostName));
    strcpy(ScopeHost, MyHostName);
    hp = gethostbyname(MyHostName);
    if (hp == NULL)
      panic("No address for our host");
    bcopy((char *)hp->h_addr, (char*)&sin.sin_addr, hp->h_length);
  }
    /* new code -- INADDR_ANY should be better than using the name of the
       host machine.  The host machine may have several different network
       addresses.  INADDR_ANY should work with all of them at once. */
  sin.sin_addr.s_addr = INADDR_ANY;

  port = iport;
  sin.sin_port = htons (port);
  ScopePort = port;

  /* bind the name and port number to the connection socket */
  if (bind(ConnectionSocket, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("bind");
      exit(-1);
    }

  debug(4,(stderr, "Socket is FD %d for %s,%d\n",
	   ConnectionSocket, ScopeHost, ScopePort));

  /* now activate the named connection socket to get messages */
  if (listen(ConnectionSocket, BACKLOG) < 0)
    {
      perror("listen");
      exit(-1);
    };

  /* a few more parameter settings */
  ioctl(ConnectionSocket, FIOCLEX, 0);
  ioctl(ConnectionSocket, FIONBIO, &ON);

  debug(4,(stderr, "Listening on FD %d\n", ConnectionSocket));
  UsingFD(ConnectionSocket, NewConnection);
}


