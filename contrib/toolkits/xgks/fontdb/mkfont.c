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
 * $Header: mkfont.c,v 4.0 89/08/31 16:31:39 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/fontdb/RCS/mkfont.c,v $
 *
 * $Log:	mkfont.c,v $
 * Revision 4.0  89/08/31  16:31:39  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.6  89/06/12  14:50:59  todd
 * ptr# c1179 added include for fcntl.h this is needed for AIX.
 * 
 * Revision 3.5  88/12/13  11:51:31  amy
 * PTR c1086	MakeSpaceChar:  Changed font construction so that undefined
 * 		characters appear as the BLANK character.
 * 
 * Revision 3.4  88/09/26  15:18:03  amy
 * MIT	Deleted argc, argv from formal parameter list of WriteFont.
 * 
 * Revision 3.3  88/09/26  15:13:50  amy
 * MIT	WriteFont:  define with no parameters.
 * 
 * Revision 3.2  88/08/19  13:02:27  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:52:35  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: mkfont.c,v 4.0 89/08/31 16:31:39 amy Exp $";

#include <stdio.h>
/* file io control flags are in fcntl.h under AIX */
#include <fcntl.h>       /* c1179 */
#include <sys/file.h>
#include "font.h"

#define MAXVC	6000

struct {
	char	fn[30];
	bits16	fnominalx, fnominaly;
	bits16	ftop, fcap, fhalf, fbase, fbottom;
	int	fc[256];
	struct vcharst fvc[MAXVC];
} Font = {
	"FontName",
	0, 0,
	0, 0, 0, 0, 0,
	{ -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	},
	{ {'e', 0, 0}, }
};

int	Vc = 0;			/* next available vcharst slot */
struct vcharst *Vcp = Font.fvc;	/* pointer to same */

main(argc, argv)
	int argc;
	char *argv[];
{
/*	FONT *fp;	*/

	ReadVFont(argc, argv);

	SetCapHalf();
	MakeSpaceChar();

	WriteFont();		/*MIT*/

/*
	ReadFont(&fp, Font.fn);
	PrintFont(fp);
*/
}

SetCapHalf()
{
	struct vcharst *vcp;

	if (Font.fc['A'] != -1) {
		vcp = &(Font.fvc[Font.fc['A']]);
		while(vcp->vc_type != 'S')
			vcp++;
		Font.fcap = vcp->vc_y;
	}
	else
		Font.fcap = Font.fnominaly;

	if (Font.fc['a'] != -1) {
		vcp = &(Font.fvc[Font.fc['a']]);
		while(vcp->vc_type != 'S')
			vcp++;
		Font.fhalf = vcp->vc_y;
	}
	else
		Font.fhalf = Font.fnominaly/2;

	Font.fbase = 0;
	
}

MakeSpaceChar()
{
	struct vcharst *vcpN;
	int    code;						/*c1086*/

	if (Font.fc['N'] != -1) {
		vcpN = &(Font.fvc[Font.fc['N']]);
		while (vcpN->vc_type != 'S')
			vcpN++;
	}
	else
		vcpN = NULL;

	Font.fc[' '] = Vc;		/* Begin a SPACE charater */

	for (code = 1; code < 255; code++) /* Make all invalid chars point to SPACE*/
		if (Font.fc[code] == -1) Font.fc[code] = Vc;   /* PTR# c1086 */ 

	Vcp->vc_type = 's';		/* extent minimum */
	Vcp->vc_x = 0;
	Vcp++->vc_y = 0;
	Vc++;

	Vcp->vc_type = 'S';		/* extent maximum */
	Vcp->vc_x = (vcpN != NULL) ? vcpN->vc_x : Font.fnominalx;
	Vcp++->vc_y = (vcpN != NULL) ? vcpN->vc_y : Font.fnominaly;
	Vc++;

	Vcp->vc_type = 'm';		/* commands to make a space */
	Vcp->vc_x = 0;
	Vcp++->vc_y = 0;
	Vc++;
	Vcp->vc_type = 'm';
	Vcp->vc_x = (vcpN != NULL) ? vcpN->vc_x : Font.fnominalx;
	Vcp++->vc_y = (vcpN != NULL) ? vcpN->vc_y : Font.fnominaly;
	Vc++;

	Vcp->vc_type = 'e';
	Vcp->vc_x = 0;
	Vcp++->vc_y = 0;
	Vc++;
}

WriteFont()		/*MIT*/
{
	int fd;
	int size;

	size = (char *)Vcp - (char *)&Font;

	if ( (fd = open(Font.fn, O_CREAT|O_RDWR, 0644)) < 0) {
		perror("open");
		exit(0);
	}
	if (write(fd, &size, sizeof(int)) != sizeof(int)) {
		perror("write size");
		exit(0);
	}
	if (write(fd, &Font, size) != size) {
		perror("write Font");
		exit(0);
	}
	close(fd);
}

ReadFont(Fpp, Fn)
	FONT **Fpp;
	char *Fn;
{
	int fd;
	int size;

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

PrintFont(F)
	FONT *F;
{
	int c, co;
	struct vcharst *cp;

	printf("Font name = %s\n",F->fname);
	fflush(stdout);
	for(c=0; c<256; c++)
		if ((co = F->fcharoffset[c]) != -1) {
			printf("character %c [%d] : ",c,c);
			for (cp= &(F->fchars[co]); (cp->vc_type != 'e'); cp++)
				printf("{ '%c', %d, %d} ", cp->vc_type, cp->vc_x, cp->vc_y);
			printf("\n");
		}
}

FILE *fontfile;

ReadVFont(argc, argv)
	int argc;
	char *argv[];
{
	int x,y, spacing, width, i;
	char s[80], *sp;

	if (argc < 3) {
		fprintf(stderr,"usage:mkfont vfontin gksfontout\n");
		exit(0);
	}
	if ((fontfile = fopen(argv[1],"r")) == NULL) {
		fprintf(stderr,"can't fopen(%s,\"r\")\n",argv[1]);
		exit(0);
	}

	strncpy(Font.fn, argv[2], 30);	/* font name */
	for(i=0; i<256; i++)		/* make sure every char is undefined */
		Font.fc[i] = -1;

	Font.ftop = -9999;
	Font.fbottom = 9999;
	Font.fnominalx = 0;
	Font.fnominaly = 0;

	while (fgets(s,80,fontfile) != NULL) {
		sp = s;
		switch(*sp++) {
		case 'S':/*	fprintf(stdout,"Special font\n"); */
				break;
		case 'U':	sscanf(sp,"%d",&spacing);
			/*	fprintf(stdout,"Default intercaracter spacing = %d",spacing); */
				break;
		case 'C':	sp++; sp[strlen(sp)-1] = NULL; /* past the space & null */
			/*	fprintf(stdout,"Character name = %s\n",sp); */
				BeginChar(sp);
				break;
		case 'u':	sscanf(sp,"%d",&spacing);
			/*	fprintf(stdout,"intercharacter space for this character = %d\n",spacing);*/
				break;
		case 'w':	sscanf(sp,"%d",&width);
			/*	fprintf(stdout,"total width = %d\n",width);*/
				break;
		case 'm':	sscanf(sp,"%d%d",&x,&y);
				cklimits(x,y);
				Vcp->vc_type = 'm';
				Vcp->vc_x = x;
				Vcp++->vc_y = y;
				Vc++;

				break;
		case 'n':	sscanf(sp,"%d%d",&x,&y);
				cklimits(x,y);
				Vcp->vc_type = 'd';
				Vcp->vc_x = x;
				Vcp++->vc_y = y;
				Vc++;

				break;
		case 'E':/*	fprintf(stdout,"End of that one....");*/
				FinishChar();
				break;
		default:/*	fprintf(stdout,"%s\n",s);	*/
				break;
		}
	}
}

int xmin, ymin, xmax, ymax;
int cname;

cklimits(x,y)
int x,y;
{
	if (x<xmin)
		xmin = x;
	if (x>xmax)
		xmax = x;
	if (y<ymin)
		ymin = y;
	if (y>ymax)
		ymax = y;

	if (y>Font.ftop)
		Font.ftop = y;
	if (y<Font.fbottom)
		Font.fbottom = y;
}

BeginChar(s)
char *s;
{

	xmin = ymin = 5000;
	xmax = ymax = -5000;

	if (strlen(s) == 1)
		cname = *s;
	else
		cname = 128 + *(++s);

	Font.fc[cname] = Vc;
	Vcp++; Vc++;			/* save room for min */
	Vcp++; Vc++;			/* save room for max */
}

FinishChar()
{
	struct vcharst *vp;

	vp = &(Font.fvc[Font.fc[cname]]);
	vp->vc_type = 's';
	vp->vc_x = xmin;
	vp++->vc_y = ymin;

	vp->vc_type = 'S';
	vp->vc_x = xmax;
	vp->vc_y = ymax;

	Vcp->vc_type = 'e';
	Vcp->vc_x = 0;
	Vcp++->vc_y = 0;
	Vc++;

	if ( (xmax-xmin) > Font.fnominalx)
		Font.fnominalx = (xmax-xmin);
	if ( (ymax-ymin) > Font.fnominaly)
		Font.fnominaly = (ymax-ymin);
}
