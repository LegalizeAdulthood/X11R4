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

int firstExposeExpected = 0 ;     /* set in init() */
int stonescount;                  /* total number of units */
int stonesperturbed = 0 ;         /* -1 == disrupted */
int stonesrunning;                /* -1 == running */
char *prog;                       /* argv[0] */

main(argc, argv)
int argc;
char **argv;
{
optionsRec ops[1];
float total = 0.0;
float multiplier;
float t0;
float t1;
int i;

FILE *fp;


	readoptions(argc, argv, ops);

	if ((3*ops->mintime) >= ops->timeout)
	{
		printf("MINIMUM TIME MUST BE LESS THAN A THIRD OF THE TIMEOUT VALUE = %d. \n",
			ops->timeout);
		ops->timeout = ops->mintime * 3;
		ops->timeout += 1;
		printf("NEW TIMEOUT VALUE = %d \n",ops->timeout);
	}

	if (init(ops) < 0 )
		return ( -1 );

	while ( firstExposeExpected ) HandleEvents(ops->display); 
	while ( XPending(ops->display) ) HandleEvents(ops->display);

	if (setup(argc, argv, ops))
		return ( -1 );

	sleep(3);

	while( ops->mintime > total)
	{
		while ( XPending(ops->display) ) HandleEvents(ops->display);
		XSync(ops->display,0);
	
		/* SET TIME OUT TIMER */
		settimer(ops->timeout);
	
		stonesrunning = -1;
	
		/******************************************/
		/* printf("starting timing \n"); */

		t0 = seconds();
	
		stonescount = run( ops );
		XSync( ops->display, 0 );
	
		t1 = seconds();
	
		/* printf("stop timing \n"); */
		/*******************************************/
	
		total = (t1 - t0);
		if (total < 0.0001) total = 0.1; 
		if ((float)(ops->mintime) > total) 
		{
			/* printf("TOTAL TIME = %f , MINIMUM TIME = %f \n", */
				/* total, (float)(ops->mintime)); */
			/* printf("ORIGINAL REPETITIONS          = %d \n", ops->count); */
	
			multiplier = (float)(ops->mintime) / total ;
			if (multiplier < 1.01) 
				ops->count++;
			else if (multiplier < 2.0) 
				ops->count *= 2;
			else 
				ops->count *= (int)(multiplier);
		
			/* printf("TRYING AGAIN WITH REPETITIONS = %d ! \n", ops->count); */
		}
		else if ((float)(ops->timeout) < total)
		{
			stonesperturbed = -1;
			continue;
		}
	}
	
	while ( XPending(ops->display) ) HandleEvents(ops->display);

	if ((fp = fopen(".out","a")) == NULL)
	{
		printf("PROBLEM OPENING '.out', USING 'stdout' INSTEAD. \n");
		fp = stdout;
	}

	if (!stonesperturbed)
	{
		fprintf(fp,"\n");
		fprintf(fp,"stonestype: ");
		for(i=0; i<=argc; i++)
			fprintf(fp,"%s ",argv[i]);
		fprintf(fp,"\n");
		fprintf(fp,"date:       %s",date());
		fprintf(fp,"machine:    %s \n",ops->machine);
		fprintf(fp,"build:      %s \n",ops->build);
		fprintf(fp,"vender:     %s \n",XServerVendor(ops->display));
		fprintf(fp,"release:    %d \n",XVendorRelease(ops->display));
		fprintf(fp,"server:     %s \n",ops->server);
		fprintf(fp,"monitor:    %s \n",ops->monitor);
		fprintf(fp,"font:       %s \n",ops->font);
		fprintf(fp,"background: %s \n",ops->bgtext);
		fprintf(fp,"foreground: %s \n",ops->fgtext);
		fprintf(fp,"mintime:    %d \n",ops->mintime);
		fprintf(fp,"timeout:    %d \n",ops->timeout);
		fprintf(fp,"count:      %d \n",ops->count);
		fprintf(fp,"stonecount: %d \n",stonescount);
		fprintf(fp,"batchsize:  %d \n",stonescount / ops->count);
		fprintf(fp,"time:       %f \n",total);
		fprintf(fp,"casename:   %s \n",ops->casename);
		fprintf(fp,"record:     %g %s per second during %s. \n",(float)stonescount/total,units,ops->stonetype);
		fprintf(fp,"\n");

	}
	else
	{
		fprintf(fp,"\n");
		fprintf(fp,"stonestype: ");
		for(i=0; i<=argc; i++)
			fprintf(fp,"%s ",argv[i]);
		fprintf(fp,"\n");
		fprintf(fp,"date:       %s",date()); 
		fprintf(fp,"machine:    %s \n",ops->machine);
		fprintf(fp,"build:      %s \n",ops->build);
		fprintf(fp,"vender:     %s \n",XServerVendor(ops->display));
		fprintf(fp,"release:    %d \n",XVendorRelease(ops->display));
		fprintf(fp,"server:     %s \n",ops->server);
		fprintf(fp,"monitor:    %s \n",ops->monitor);
		fprintf(fp,"font:       %s \n",ops->font);
		fprintf(fp,"background: %s \n",ops->bgtext);
		fprintf(fp,"foreground: %s \n",ops->fgtext);
		fprintf(fp,"mintime:    %d \n",ops->mintime);
		fprintf(fp,"timeout:    %d \n",ops->timeout);
		fprintf(fp,"count:      %d \n",ops->count);
		fprintf(fp,"stonecount: %d \n",stonescount);
		fprintf(fp,"batchsize:  %d \n",stonescount / ops->count);
		fprintf(fp,"time:       0.0 \n");
		fprintf(fp,"casename:   %s \n",ops->casename);
		fprintf(fp,"record:     0.0 %s per second during %s. \n",units,ops->stonetype);
		fprintf(fp,"\n");

	}

	return ( stonesperturbed ) ;
}
 
/*****************************************************/
