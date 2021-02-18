/* Font translation for vfont-style fonts to character format.
 *
 *	Use:  vft2ch fontfile  [ character_list ]
 *
 *		Reads "fontfile" from current directory (or if not found,
 *	from VFONTDIR defined below) and converts it to a character font format
 *	editable by real people, and convertable BACK to vfont format by the
 *	ch2rst program.  Output goes to stdout.
 */

#include <stdio.h>
#include <ctype.h>
#include <vfont.h>


#define	DIRSIZ	256
#define	MAGICNO	0436

#ifndef	VFONTDIR
#define VFONTDIR		"/usr/lib/vfont"
#endif	VFONTDIR


struct header FontHeader;
struct dispatch disptable[DIRSIZ];

char	IName[100];
long	fbase;

unsigned char	defascii[DIRSIZ];
unsigned char	*charswanted = defascii;
char	charbits[20000];
int	height, width, bwidth;


main(argc,argv)
int argc;
char **argv;
{
	int FID,i,j;

	if (argc < 2)
		error("usage: %s filename", argv[0]);

	for (i=0; i<DIRSIZ; i++)
		defascii[i] = i;
	if (argc >= 3)
		charswanted = (unsigned char *) argv[2];

	sprintf(IName,"%s/%s", VFONTDIR, argv[1]);
	if ((FID = open(argv[1],0)) < 0)
		if ((FID = open(IName,0)) < 0)
			error("Can't find %s", argv[1]);

	if (read(FID,&FontHeader,sizeof FontHeader) != sizeof FontHeader)
		error("bad header in Font file.");
	if (FontHeader.magic != MAGICNO)
		error("magic number %o wrong", FontHeader.magic);
	if (read(FID,&disptable[0],sizeof disptable) != sizeof disptable)
		error("bad dispatch table in Font file");
	fbase = sizeof FontHeader + sizeof disptable;

	while (*argv[1] != '.' || *(argv[1]+1) < '0' || *(argv[1]+1) > '9')
		(argv[1])++;
	printf("fontheader\nsize 46\nmag 1000\ndesiz %d\n", atoi(*argv));
	printf("rot 0\ncadv 0\nladv 1\nid 0\nres 200\n");

	for (i=0; i<DIRSIZ; i++) {
		j = charswanted[i];
		if (i>0 && j==0)
			break;
		if (disptable[j].nbytes != 0) {
			register int k, l;
			register int left = disptable[j].left;
			register int right = disptable[j].right;
			register int up = disptable[j].up;
			register int down = disptable[j].down;


			printf(":%d, width = %d.01\n", j, disptable[j].width);

			lseek(FID, fbase+disptable[j].addr, 0);
			read(FID, charbits, disptable[j].nbytes);
			height = up + down;
			width = left + right;
			bwidth = (width+7)/8;
			for (k = up; k < 0; k++) {
			    for (l = left; l < 0; l++)
				pbit(l, k, l==left && k==up);
			    for (l = 0; l <= left; l++)
				pbit(l, k, l==left && k==up);
			    while (l < width)
				pbit(l++, k, 0);
			    printf("\n");
			}
			for (k = 0; k <= up; k++) {
			    for (l = left; l < 0; l++)
				pbit(l, k, l==left && k==up);
			    for (l = 0; l <= left; l++)
				pbit(l, k, l==left && k==up);
			    while (l < width)
				pbit(l++, k, 0);
			    printf("\n");
			}
			while (k < height) {
			    for (l = left; l < 0; l++)
				pbit(l, k, 0);
			    for (l = 0; l <= left; l++)
				pbit(l, k, 0);
			    while (l < width)
				pbit(l++, k, 0);
			    printf("\n");
			    k++;
			}
			printf("\n");
		}
	}
}


/*VARARGS1*/
error(string, a1, a2, a3, a4)
char *string;
{ 
	fprintf(stderr, "vft2ch: ");
	fprintf(stderr, string, a1, a2, a3, a4);
	fprintf(stderr, "\n");
	exit(8);
}


pbit(col, row, flag)
int col, row, flag;
{
	if (col<0 || row<0 || row>=height || col>=width) {
		printf(flag?"x":".");
		return;
	}
	if ((charbits[row*bwidth + (col>>3)] & 0xff) & (0x80 >> (col&7)))
		printf(flag?"X":"@");
	else
		printf(flag?"x":".");
}
