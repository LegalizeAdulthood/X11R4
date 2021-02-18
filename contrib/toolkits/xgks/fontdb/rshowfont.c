/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 * $Header: rshowfont.c,v 4.0 89/08/31 16:31:46 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/fontdb/RCS/rshowfont.c,v $
 *
 * $Log:	rshowfont.c,v $
 * Revision 4.0  89/08/31  16:31:46  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.3  88/09/26  15:15:50  amy
 * MIT	Delete unused variables:  fd, size in main; x, y in DisplayFont.
 * 
 * Revision 3.2  88/08/19  13:04:19  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:52:39  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: rshowfont.c,v 4.0 89/08/31 16:31:46 amy Exp $";

#include <stdio.h>
#include <sys/file.h>
#include "font.h"

main(argc, argv)
	int argc;
	char *argv[];
{
	FONT *fp;

	if (argc < 3) {
		fprintf(stderr, "usage:showfont font scale\n");
		exit(0);
	}
	ReadFont(&fp, argv[1]);
	DisplayFont(fp, atoi(argv[2]) );
}

ReadFont(Fpp, Fn)
	FONT **Fpp;
	char *Fn;
{
	int fd;
	int size;
	char *malloc();

	if ( (fd = open(Fn, O_RDONLY, 0644)) < 0) {
		perror("open");
		exit(0);
	}
	if (read(fd, &size, sizeof(int)) != sizeof(int)) {
		perror("read size");
		exit(0);
	}
	if ( (*Fpp = (FONT *) malloc( (unsigned) size)) == NULL) {
		perror("malloc failed");
		exit(0);
	}
	if (read(fd, *Fpp, size) != size) {
		perror("read Font");
		exit(0);
	}
	close(fd);
}

int XORG, YORG;
#define DX (160/Scale)
#define DY (170/Scale)
short Wd, Ht;
int Scale;
int xmin, ymin, xmax, ymax;

DisplayFont(Fp, scale)
	FONT *Fp;
	int scale;
{
	int c, co;
	struct vcharst *cp;

	Scale = scale;
	ropenrtk(60);

	/* ibmaed origin at top left */
	XORG = 0 - DX;	/* BIG NUM off right edge */
	YORG = 950- 125/Scale;

	for(c=0; c<256; c++)
		if ((co = Fp->fcharoffset[c]) != -1) {
			NextChar();
			for (cp= &(Fp->fchars[co]); (cp->vc_type != 'e'); cp++)
				switch (cp->vc_type) {
				case 's':	xmin = cp->vc_x; ymin = cp->vc_y;
						break;
				case 'S':	xmax = cp->vc_x; ymax = cp->vc_y;
						break;
				case 'm':	rmove(XORG +((cp->vc_x)/Scale), YORG+((cp->vc_y)/Scale));
					/*	fprintf(stderr, "M %d %d\n",(cp->vc_x)/Scale+XORG, (cp->vc_y)/Scale+YORG);
					*/
						break;
				case 'd':	rqline(XORG+((cp->vc_x)/Scale), YORG+((cp->vc_y)/Scale));
					/*	fprintf(stderr, "L %d %d\n",(cp->vc_x)/Scale+XORG, (cp->vc_y)/Scale+YORG);
					*/
						break;
				};

		}
	getchar();
	rclosertk();
}

NextChar()
{

/* show extent
 *	rmove(XORG+(xmin/Scale), YORG+(ymin/Scale));
 *	rqline(XORG+(xmax/Scale), YORG+(ymin/Scale));
 *	rqline(XORG+(xmax/Scale), YORG+(ymax/Scale));
 *	rqline(XORG+(xmin/Scale), YORG+(ymax/Scale));
 *	rqline(XORG+(xmin/Scale), YORG+(ymin/Scale));
 */
	XORG += DX;
	if ((XORG+DX) > 1274) {
		XORG = 0;
		YORG -= DY;
		if ((YORG-35/Scale) < 0) {
			fprintf(stderr,"Next page...\n");
			rflush();
			getchar();
			rberase();
			XORG = 0;
			YORG = 950 - 125/Scale;
		}
	}
}
