/*
**LIBS: -lsnap 
*/
/*
  guardian loadaverage reporter client
*/
char *rcsid="$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/snap2/loadserv/RCS/loadav.c,v 2.6 89/06/07 18:15:46 aw0g Exp $";

#include <andrewos.h> /* sys/types.h */
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <loadserv.h>
#include <scb.h>

#define LOAD_NAME "loadserv"

extern char *oSNAP_ernum_to_str();
extern char *oGASP_ernum_to_str();

#define VER_MAJ (1)
#define VER_MIN (0)

#ifndef FALSE
#define FALSE (0==1)
#define TRUE (0==0)
#endif /* FALSE */

#define fatal_error(xx_arg) {printf("\n");printf xx_arg;fflush(stdout);exit(1);}

extern char *malloc();

struct {
    char *hostname;
    int cid;      /*snap connection id*/
    SNAP_CPARMS bcparms;
    char *rcv_buf;
    int err_nest;
}gl;

void close_snap();
void check_snap_call();

/*
  close up a snap connection
  */
void close_snap()
{
    if(gl.cid==(-1))return;
    SNAP_EndConv(gl.cid,"",0,NULL);
    gl.cid=(-1);
}

/*
  assert that a snap call succeded
  */
void check_snap_call(arst,acal)
int arst;
char *acal;
{/*don't get stuck in error loops*/
    /*try harder and harder to die*/
    char *er_text;
    if(arst>=0)return;
    if(gl.err_nest++ > 0) {
	if(gl.err_nest == 1)return;
	if(gl.err_nest == 2)exit(1);
	if(gl.err_nest >= 3)exit(-1);
    }
    if(arst!=(-1)) {
      fprintf(stdout,"\n%s failed:",acal);
      er_text=oSNAP_ernum_to_str(arst);
      if(er_text==0)
	fprintf(stdout,"Internal error:unknown snap error %d\n",arst);
      else
	fprintf(stdout,"%s\n",er_text);
    }
    fflush(stdout);
    close_snap();
    exit(1);
}

/*
  initilize a snap connection
  */
void snap_init()
{
    extern int SNAP_login;
    gl.bcparms.maxtime=30;
    gl.bcparms.timeout=2;
    gl.bcparms.encryptlevel=SNAP_NOENCRYPT;
    check_snap_call(SNAP_ClientInit(),"client init");
    SNAP_login = TRUE;
    SNAP_Nop();
    fprintf(stdout,"loadav %d.%d Connecting to '%s'...",
	     VER_MAJ,VER_MIN,gl.hostname);
    fflush(stdout);
    gl.cid=SNAP_BeginConv("snap.guardian",gl.hostname,LOAD_NAME,
			   "ms40","message",9,0,&gl.bcparms);
    /*begin conversation is special as an extra error code
      is stashed in in guardian_rc
      */
    if(gl.cid<0) {
	char *er_text;
	fprintf(stdout,"\nconnect failed:");
	er_text=oGASP_ernum_to_str(gl.bcparms.guardian_rc);
	if(er_text==0)
	    fprintf(stdout,"Internal error:unknown gasp error %d\n",gl.bcparms.guardian_rc);
	else
	    fprintf(stdout,"%s\n",er_text);
	check_snap_call(-1,"connect");
    }
    fprintf(stdout," [ok]\n");
}

void snap_get_load(options)
char *options;
{
    char send_buf[100];
    char *ptr=send_buf;
    int length;
    fprintf(stdout,"Downloading ... ");
    fflush(stdout);
    ptr = putint(ptr,(SNAP_INTEGER)VER_MAJ);
    ptr = putint(ptr,(SNAP_INTEGER)VER_MIN);
    ptr = putint(ptr,(SNAP_INTEGER)OP_LOAD_REQ); 
    ptr = putstr(ptr,options);
    length = ptr - send_buf;
    check_snap_call(SNAP_SendWithReply(gl.cid,send_buf,length,
					gl.rcv_buf,SNAP_BUF_SIZE,NULL),"download request");
    close_snap();
    fprintf(stdout," [ok]\n");
}

void print_buf()
{
    char *ptr=gl.rcv_buf;
    char *astr;
    SNAP_INTEGER rcode;
    SNAP_INTEGER numstrs;
    int i;
    ptr = getint(ptr,&rcode);
    ptr = getint(ptr,&numstrs);
    for(i=0;i<numstrs;i++) {
	ptr = getstr(ptr,&astr);
	fprintf(stdout,"%s",astr);
    }
    fflush(stdout);
}

/*
  tell the user how to use this program
  */
void usage()
{
    fprintf(stdout,"usage:loadav hostname\n");
    fflush(stdout);
    exit(0);
}

main(argc,argv)
int argc;
char *argv[];
{
    char *options="";
    gl.cid = (-1);
    if(argc<2)
	usage();
    if(argc>2)options=argv[2];
    gl.hostname=argv[1];
    gl.cid=0;
    gl.rcv_buf=malloc(SNAP_BUF_SIZE);
    if(gl.rcv_buf==0)
	fatal_error(("can't allocate %d bytes for buffer"));
    snap_init();
    snap_get_load(options);
    print_buf();
}
