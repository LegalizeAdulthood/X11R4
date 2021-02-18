/* Copyright International Business Machines, 1989
 *
 *
 *
 *  IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 *  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR ANY
 *  PARTICULAR USE.  IN NO EVENT SHALL
 *  IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 *  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 *  SOFTWARE
 *
 *
 *
*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

#define DEFAULTX	 20
#define DEFAULTY	 20
#define DEFAULTWIDTH	 600
#define DEFAULTHEIGHT	 400
#define DEFAULTBW	 0
#define DEFAULTCURSX     10
#define DEFAULTCURSY     10
#define DEFAULTBORDER	 "white"
#define DEFAULTBG	 "black"
#define DEFAULTFG	 "white"
#define DEFAULTSERVER	 "unix:0"
#define DEFAULTFONT	 "6x10"
#define DEFAULTSTRING	 "unknown"
#define DEFAULTGCFUNC    GXcopy
#define DEFAULTIMEOUT	 180
#define DEFAULTMINTIME	 6
#define DEFAULTCOUNT	 1

#define GETCOLOR(PIX,TEXT,DEFAULT)		\
	if (XAllocNamedColor(ops->display, DefaultColormap(ops->display,ops->dScreen),	\
			 TEXT,installedcolor,databasecolor))	\
		PIX = installedcolor->pixel;	\
	   else PIX = DEFAULT(ops->display,ops->dScreen);
 
#define GETGC(ALU,FG,BG,GCD)				\
	gcv.function = ALU;			\
	gcv.foreground = FG;			\
	gcv.background = BG;			\
	GCD = XCreateGC(display, window,  	\
	GCFunction | GCFont | GCForeground | GCBackground, &gcv);

/* local to this directory */
void settimer(); 
float seconds();
void HandleEvents();
void readoptions();
char *date();

/* system calls */
char *malloc();
char *index();
long time();
char *ctime();
void exit();
unsigned int sleep();
void perror();
unsigned int alarm();

extern char *prog;
extern int firstExposeExpected;
extern int stonescount;
extern int stonesperturbed;
extern int stonesrunning;
extern char *units;

typedef struct ops_struct {
	char	stonetype[32];
	char	casename[32];
	char 	server[32];
	char 	build[32];
	char 	machine[32];
	char	monitor[32];
	char 	font[32];
	char 	bgtext[32];
	char 	fgtext[32];
	GC	bggc;
	GC 	fggc;
	Display *display;
	Window	window;
	Visual  *dVisual;
	int     dScreen;
	int		alu;
	int 	count;
	int 	mintime;
	int		timeout;
	int     cursX;
	int     cursY;
	int     winX;
	int     winY;
	unsigned int winW;
	unsigned int winH;
	unsigned int winBW; 
	} optionsRec, *optionsPtr;

