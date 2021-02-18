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
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "commonhd.h"
#include "de_header.h"
#include "jservercf.h"
#include <pwd.h>

error_exit1(x , y1 , y2 , y3 , y4 , y5)
char *x;
int y1 , y2 , y3 , y4 , y5;
{
  char buf[512];

  strcpy(buf , "Fatal error. Exiting...\n");
  strcat(buf , x);
  error1(buf , y1, y2 , y3 , y4 , y5);
  exit_hand();
}
  
error_exit(x)
char *x;
{
  char buf[512];

  strcpy(buf , "Fatal error. Exiting...\n");
  strcat(buf , x);
  my_error(buf);
  exit_hand();
}


my_error(x)
register char *x;
{
  long obakenoQ;
/*  struct passed *getpwiuid();  */

  if(!noisy)return;
  obakenoQ = time((long *)0);
  if(c_c){
#ifdef SYSVR2
      fprintf(stderr , "client %s :	%s	%s	 errno =%d\n\n" ,
/*	      getpwuid(c_c->uid)->pw_name, */
	      c_c->user_name,  /* V3.0 */
	      asctime(localtime(&obakenoQ)),
	      x , errno);
#endif
#ifdef BSD42
      fprintf(stderr , "client %s :	%s	%s	 \n\n" ,
/*	      getpwuid(c_c->uid)->pw_name,  */
	      c_c->user_name,  /* V3.0 */
	      asctime(localtime(&obakenoQ)),
	      x );
#endif
  }else{
#ifdef SYSVR2
      fprintf(stderr , "	%s	%s	 errno =%d\n\n" ,
	      asctime(localtime(&obakenoQ)),
	      x , errno);

#endif
#ifdef BSD42
      fprintf(stderr , "	%s	%s	 \n\n" ,
	      asctime(localtime(&obakenoQ)),
	      x );
#endif
  }
  fflush(stderr);
}

error1(x , y1 , y2 , y3 , y4 , y5)
register char *x;
int y1 , y2 , y3 , y4 , y5;
{
  char buf[512];

  strcpy(buf , x);
  sprintf(buf , x , y1 , y2 , y3 , y4 , y5);
  my_error(buf);
}
  

signal_hand(x)
int x;
{
  error1("signal catched signal_no = %d" , x);
#ifdef SYSVR2
  signal(x , signal_hand);
#endif
}

terminate_hand()
{
  demon_fin();
  delete_proof_file(PROOF_FILE_NAME);
  exit(0);
}

exit_hand()
{
  demon_fin();
  delete_proof_file(PROOF_FILE_NAME);
  exit(250);
}

out(x , y1 , y2 , y3 , y4 , y5, y6, y7, y8, y9, y10, y11, y12)
char *x;
int y1 , y2 , y3 , y4 , y5,  y6, y7, y8, y9, y10, y11, y12;
{
  if(!noisy)return;
  fprintf(stderr, x, y1 , y2 , y3 , y4 , y5,  y6, y7, y8, y9, y10, y11, y12);
  fflush(stderr);
}



#ifdef	DEBUG
/*
	debug print
*/

#ifdef	putwchar
#undef	putwchar
#endif
putwchar(x)
unsigned short x;
{
    if(!noisy)return;
    putc( x >> 8, stderr);
    putc( x, stderr);
    /*
    putchar( x >> 8);
    putchar( x );
    */
	fflush(stdout);
}

wsputs(buf)
short *buf;
{
    if(!noisy)return;

    for(; ; ) {
	if (*buf == 0)
	    return;
	putwchar(*buf++);
    }
}
#endif	DEBUG
