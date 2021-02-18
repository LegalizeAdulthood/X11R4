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
#include "stones.h"

void
readoptions(argc,argv, op)
int argc;
char **argv;
optionsPtr op;
{
int i;
Display *display;
    
prog = argv[0];

strcpy(op->stonetype , DEFAULTSTRING);
strcpy(op->casename , DEFAULTSTRING);
strcpy(op->server , DEFAULTSTRING);
strcpy(op->build , DEFAULTSTRING);
strcpy(op->machine , DEFAULTSTRING);
strcpy(op->monitor , DEFAULTSTRING);
strcpy(op->font , DEFAULTFONT);
strcpy(op->bgtext , DEFAULTBG);
strcpy(op->fgtext , DEFAULTFG);
strcpy(op->server , DEFAULTSERVER );
op->alu = DEFAULTGCFUNC;
op->count = DEFAULTCOUNT;
op->mintime = DEFAULTMINTIME;
op->timeout = DEFAULTIMEOUT;
op->cursX = DEFAULTCURSX;
op->cursY = DEFAULTCURSY;
op->winX = DEFAULTX;
op->winY = DEFAULTY;
op->winW = DEFAULTWIDTH;
op->winH = DEFAULTHEIGHT;
op->winBW = DEFAULTBW; 

for(i=1; i<argc; i++)
{
	if (index(argv[i],':') != 0)
	{
		strcpy(op->server, argv[i] );
		i = argc+1;
	}
}

if (!(display = XOpenDisplay(op->server)))
{ 
	perror("Cannot open display\n"); 
	exit(-1); 
}

for(i=1; i<argc; i++)
{
	if (strcmp(argv[i],"-winX")==0) op->winX = atoi(argv[++i]);
	else if (strcmp(argv[i],"-winY")==0) op->winY = atoi(argv[++i]);
	else if (strcmp(argv[i],"-winW")==0) op->winW = atoi(argv[++i]);
	else if (strcmp(argv[i],"-winH")==0) op->winH = atoi(argv[++i]);
	else if (strcmp(argv[i],"-winBW")==0) op->winBW = atoi(argv[++i]);
	else if (strcmp(argv[i],"-cursX")==0) op->cursX = atoi(argv[++i]);
	else if (strcmp(argv[i],"-cursY")==0) op->cursY = atoi(argv[++i]);
	else if (strcmp(argv[i],"-bg") ==0 ) strcpy(op->bgtext,argv[++i]);
	else if (strcmp(argv[i],"-fg") ==0 ) strcpy(op->fgtext,argv[++i]);
	else if (strcmp(argv[i],"-fn") ==0 ) strcpy(op->font,argv[++i]);
	else if (strcmp(argv[i],"-n") ==0 ) op->count = atoi(argv[++i]);
	else if (strcmp(argv[i],"-timeout") ==0 ) op->timeout = atoi(argv[++i]);
	else if (strcmp(argv[i],"-mintime") ==0 ) op->mintime = atoi(argv[++i]);
	else if (strcmp(argv[i],"-build") ==0 ) strcpy(op->build,argv[++i]);
	else if (strcmp(argv[i],"-machine") ==0 ) strcpy(op->machine,argv[++i]);
	else if (strcmp(argv[i],"-monitor") ==0 ) strcpy(op->monitor,argv[++i]);
	else if (strcmp(argv[i],"-casename") ==0 ) strcpy(op->casename,argv[++i]);
	else if (strcmp(argv[i],"-help") ==0 )
	{
		printf("\n");
		printf("\tStones' main options are: \n\n");
	
		printf("\t-machine  (string)     name of the machine running X11 (default=%s) \n",op->machine);
		printf("\tserver:0               where 'server' is 'unix' or 'netname' (default=%s) \n",op->server);
		printf("\t-build    (string)     build name of the X11 server (default=%s) \n",op->build);
		printf("\t-monitor  (string)     name of the video display terminal (default=%s) \n",op->monitor);
		printf("\t-casename (string)     nickname for a specific case of stones (default=%s) \n",op->casename);
		printf("\t-winX     (int)        window placement (default=%d) \n",op->winX);
		printf("\t-winY     (int)        window placement (default=%d) \n",op->winY);
		printf("\t-winW     (int)        window width (default=%d) \n",op->winW);
		printf("\t-winH     (int)        window heigth (default=%d) \n",op->winH);
		printf("\t-winBW    (int)        window border width (default=%d) \n",op->winBW);
		printf("\t-cursX    (int)        cursor placement (default=%d) \n",op->cursX);
		printf("\t-cursY    (int)        cursor placement (default=%d) \n",op->cursY);
		printf("\t-fg       (string)     foreground (default=%s) \n",op->fgtext);
		printf("\t-bg       (string)     background (default=%s) \n",op->bgtext);
		printf("\t-n        (int)        initial repetitions (default=%d) \n",op->count);
		printf("\t-mintime  (int)        minimum amount of time to run (default=%d) \n",op->mintime);
		printf("\t-timeout  (int)        seconds before timeout (default=%d) \n",op->timeout);
		printf("\t-alu      (GXstring)   GXlogical operation (default=%d) \n",op->alu);
		printf("\t-fn       (string)     name of font to use like 'Rom14.500' (default=%s) \n",op->font);
		printf("\t-help                  stones' help page \n");
		printf("\t-h                     %s' help page \n",prog);
	
		printf("\n");
	}
	else if (strcmp(argv[i],"-alu")==0)
	{
		char *at;
		int  ati;
		at = argv[++i];
		ati = atoi(at);
		if      (strcmp(at, "GXclear") == 0)
				op->alu = GXclear;
		else if (strcmp(at, "GXand") == 0)
				op->alu = GXand;
		else if (strcmp(at, "GXandReverse") == 0)
				op->alu = GXandReverse;
		else if (strcmp(at, "GXcopy") == 0)
				op->alu = GXcopy;
		else if (strcmp(at, "GXandInverted") == 0)
				op->alu = GXandInverted;
		else if (strcmp(at, "GXnoop") == 0)
				op->alu = GXnoop;
		else if (strcmp(at, "GXxor") == 0)
				op->alu = GXxor;
		else if (strcmp(at, "GXor") == 0)
				op->alu = GXor;
		else if (strcmp(at, "GXnor") == 0)
				op->alu = GXnor;
		else if (strcmp(at, "GXequiv") == 0)
				op->alu = GXequiv;
		else if (strcmp(at, "GXinvert") == 0)
				op->alu = GXinvert;
		else if (strcmp(at, "GXorReverse") == 0)
				op->alu = GXorReverse;
		else if (strcmp(at, "GXcopyInverted") == 0)
				op->alu = GXcopyInverted;
		else if (strcmp(at, "GXorInverted") == 0)
				op->alu = GXorInverted;
		else if (strcmp(at, "GXnand") == 0)
				op->alu = GXnand;
		else if (strcmp(at, "GXset") == 0)
				op->alu = GXset;
		else if ((ati < 16) && (ati> 0)) op->alu = ati;
		else if (strcmp(at, "0")==0) op->alu = 0;
		else 
		{
			printf("ALU %s not recognized\n",at);
			op->alu = GXcopy;
	     }
	}
}

op->display = display;
op->dScreen = DefaultScreen(display);
op->dVisual = DefaultVisual(display,op->dScreen);
strcpy(op->stonetype,prog);
}

/******************************************************/
