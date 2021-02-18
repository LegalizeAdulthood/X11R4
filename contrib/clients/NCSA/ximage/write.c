/*
 * This NCSA software product is public domain software.  Permission
 * is granted to do whatever you like with it. Should you feel compelled 
 * to make a contribution for the use of this software, please send 
 * comments as to what you like or don't like about the software, 
 * how you use it, how it helps your work, etc. to softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Dave Thompson, National Center for Supercomputing Applications
 *         davet@ncsa.uiuc.edu
 */

#include <stdio.h>
#include <X11/Xlib.h>

int WriteSEQPalette(name,ccells)
char	*name;
XColor	ccells[256];
{
FILE	*fp;
int	x;
char	buff[256];
	if (!(fp = fopen(name,"w"))) {
		sprintf(buff,"Can't open for writing file %s\n",name);
		PrintMesg(buff);
		return(NULL);
		}

	for (x=0; x < 256; x++)
		putc(((char )(ccells[x].red >> 8)),fp);
	for (x=0; x < 256; x++)
		putc(((char )(ccells[x].green >> 8)),fp);
	for (x=0; x < 256; x++)
		putc(((char )(ccells[x].blue >> 8)),fp);

	fclose(fp);

	sprintf(buff,"Palette saved in SEQ format as %s\n",name);
	PrintMesg(buff);

	return(768);
}
