/*
 * XLife Copyright 1989 Jon Bennett jb7m+@andrew.cmu.edu, jcrb@cs.cmu.edu
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

extern int sys_nerr, errno;
extern char *sys_errlist[];
#define SYSERR sys_errlist[(errno > sys_nerr? 0 : errno)]

GLOBAL Display *disp;
GLOBAL Window rootw, mainw, lifew, inputw;
GLOBAL int screen;
GLOBAL unsigned long fcolor, bcolor;
GLOBAL XEvent event;
GLOBAL XFontStruct *nfont, *bfont;
GLOBAL int getinput;
#define INPBUFLEN 255
GLOBAL char inpbuf[INPBUFLEN];
GLOBAL int minbuflen;
#define DIRBUFLEN 100
GLOBAL char loadirbuf[DIRBUFLEN];
GLOBAL char savedirbuf[DIRBUFLEN];
GLOBAL char loadir[DIRBUFLEN];
GLOBAL char savedir[DIRBUFLEN];
GLOBAL char keybuf[16];
GLOBAL KeySym ks;
GLOBAL XGCValues xgcv;
GLOBAL GC ntextgc, btextgc,inputgc,blackgc,whitegc;
GLOBAL Pixmap lifepm;
GLOBAL Cursor cursor;

GLOBAL char *itoa();

GLOBAL unsigned long xpos,ypos;

GLOBAL unsigned long hash();
GLOBAL struct cell *create();
GLOBAL struct cell *link();
GLOBAL void generate();
GLOBAL void addcell();
GLOBAL void deletecell();
GLOBAL void redisplay();
GLOBAL void Motion();
GLOBAL void kill();
GLOBAL void clear();
GLOBAL void savefile();
GLOBAL void loadfile();
GLOBAL void center();
GLOBAL void newrules();
GLOBAL void redrawscreen();
GLOBAL void help();
GLOBAL void randomize();
GLOBAL unsigned long dispcells;
GLOBAL unsigned long load;
GLOBAL unsigned long save;
GLOBAL unsigned long scale;
GLOBAL unsigned long born;
GLOBAL unsigned long live;
GLOBAL unsigned long run;
GLOBAL unsigned long maxdead;
GLOBAL unsigned long hide;
GLOBAL unsigned long generations;
GLOBAL unsigned long numboxes;
GLOBAL unsigned long numcells;
GLOBAL int width;
GLOBAL int height;
GLOBAL int inputlength;
GLOBAL int onpt,offpt,onrect,offrect;
GLOBAL int state;
#define HASHSIZE 32768

GLOBAL struct cell *head;
GLOBAL struct cell *freep;

