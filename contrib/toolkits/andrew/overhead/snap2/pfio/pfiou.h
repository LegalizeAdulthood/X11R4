/*
 * portable packet packing/unpacking interface
 * unix io module interface
 * by Aaron Wohl
 * $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/snap2/pfio/RCS/pfiou.h,v 1.2 89/09/01 06:13:21 aw0g Exp $
 */

#include <ctype.h>
#include <andrewos.h>
#include <pfio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
extern int errno;
#include <errno.h>

#ifndef er_ucall
#define er_ucall(xx_ures,xx_err){if((xx_ures)<0){rl_error xx_err ;};}
#endif

void get_foreign_addr();
void set_no_block();
void get_foreign_addr();
void set_no_block();
int new_socket();
long get_netaddr();
