/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/mail/cmd/RCS/decodeid.c,v 1.8 89/09/19 14:15:51 cfe Exp $

decode_id -- decode the IDs that ams_genid() produces.

*/

#include <stdio.h>
#include <mailconf.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef hpux
extern char *inet_ntoa();
#else /* hpux */
#include <arpa/inet.h>
#endif /* hpux */
#include <netdb.h>
#include <andrewos.h> /* sys/time.h */

main(argc, argv)
int argc;
unsigned char **argv;
{
    int i, j;
    unsigned long foo, bar, baz;
    struct in_addr A;
    unsigned char *cp;
    unsigned char Addr[5];
    unsigned char ArgCopy[20];
    struct hostent *Host;
    
    if (argc > 1) {
	for (i=1; i<argc; ++i) {
	    bzero(ArgCopy, sizeof(ArgCopy));
	    strncpy(ArgCopy, argv[i], sizeof(ArgCopy));
	    ArgCopy[sizeof(ArgCopy)-1] = '\0';
	    cp=ArgCopy;
	    if (*cp == '+') ++cp;
	    foo = conv64tolong(cp);
	    bar = conv64tolong(cp+6);
	    bar = htonl(bar);
	    baz = conv64tolong(cp+12);
	    printf("``%s'': generated %s from ", cp, NiceTime(foo));
	    strncpy(Addr, (unsigned char *) &bar, 4);
	    Addr[4] = '\0';
	    Host = NULL;
	    if (Addr[0] != '\0') Host = gethostbyaddr(Addr, 4, AF_INET);
	    if (Host != NULL) {
		fputs(Host->h_name, stdout);
		fputs(" (", stdout);
	    };
	    fputc('[', stdout);
	    A.s_addr = bar;
	    fputs(inet_ntoa(A), stdout);
	    fputc(']', stdout);
	    if (Host != NULL) fputc(')', stdout);
	    j=strlen(cp);
	    if (j <= 13) {
		printf(", ctr (mod 64) of %d.\n", baz);
	    } else {
		printf(", pid %d, ctr (mod 256) of %d.\n",
		       (baz >> 16) & 0xFFFF, (baz >> 8) & 0xFF);
	    }
	}
	exit(0);
    }
    else {
	printf("usage: %s id [id] [id]...\n", argv[0]);
	exit(2);
    }
}
