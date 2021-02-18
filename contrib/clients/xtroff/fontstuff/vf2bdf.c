#include	<stdio.h>
#include	<vfont.h>
#include	<sys/types.h>

#ifndef VFONT_MAGIC
#define	VFONT_MAGIC	0436
#endif
#ifndef NUM_DISPATCH
#define	NUM_DISPATCH	256
#endif

#define		RESOLUTION	72

/*
**	Convert vfont to bdf font, reads specified file name and prints bdf
**	on stdout.
*/

fatal(msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

main(argc, argv)
	int		argc;
	char		*argv[];
{
	register int	i, nchars;
	register u_char	*glyphs;
	register int	maxleft, maxright, maxup, maxdown;
	int		ps;
	struct header	h;
	struct dispatch	d[NUM_DISPATCH], *pd;

	if (argc < 3)
		fatal("Usage: vf2bdf vfontfile pointsize");

	if (freopen(argv[1], "r", stdin) == NULL)
		fatal("Cannot open vfontfile");

	if ((ps = atoi(argv[2])) <= 0)
		ps = 10;

	if (fread(&h, sizeof(h), 1, stdin) != 1)
		fatal("Can't read header");

	if (h.magic != VFONT_MAGIC)
		fatal("Not vfont file, wrong magic number");

	if (fread(d, sizeof(d[0]), NUM_DISPATCH, stdin) != NUM_DISPATCH)
		fatal("Can't read dispatch structures");

	if ((glyphs = (u_char *)malloc(h.size)) == NULL)
		fatal("Can't get memory for bitmaps");

	if (fread(glyphs, sizeof(u_char), h.size, stdin) != h.size)
		fatal("Short read on bitmaps");

/*
**	Count number of characters and find maximum ascent and minimun descent
*/
	maxleft = -1000;
	maxright = -1000;
	maxup = -1000;
	maxdown = -1000;
	nchars = 0;
	for (i = 0; i < NUM_DISPATCH; ++i)
	{

		if (d[i].nbytes <= 0)
			continue;
		++nchars;
		pd = &d[i];
		if (pd->left > maxleft)
			maxleft = pd->left;
		if (pd->right > maxright)
			maxright = pd->right;
		if (pd->up > maxup)
			maxup = pd->up;
		if (pd->down > maxdown)
			maxdown = pd->down;
	}

	printf("STARTFONT 2.1\n");
	printf("FONT %s\n", argv[1]);
	printf("SIZE %d %d %d\n", ps, RESOLUTION, RESOLUTION);
	/* not sure if maxx and maxy are correct here */
	/* they're not!! (DAC)*/
	printf("FONTBOUNDINGBOX %d %d %d %d\n", maxleft+maxright,
	       maxup+maxdown, -maxleft, -maxdown);
	printf("STARTPROPERTIES 4\n");
	printf("FONT_ASCENT %d\n", maxup);
	printf("FONT_DESCENT %d\n", maxdown);
	printf("DEFAULT_CHAR 32\n");
	printf("COPYRIGHT \"Derived from a vfont\"\n");
	printf("ENDPROPERTIES\n");
	printf("CHARS %d\n", nchars);

	for (i = 0; i < NUM_DISPATCH; ++i)
	{
		register int	j, k;
		register u_char	*p;
		int nb, nba;

		pd = &d[i];
		if (pd->nbytes <= 0)
			continue;
		printf("STARTCHAR H%02x\n", i);
		
		if (pd->left + pd->right == 0)	/* zero width character */
		{
			printf("ENCODING -1\n");
			printf("ENDCHAR\n");
			continue;
		}
		
		printf("ENCODING %d\n", i);

		/*
		 **	Don't think bdf to snf cares about SWIDTH actually
		 */
		j = pd->width * 72000 / (ps * RESOLUTION);
		printf("SWIDTH %d 0\n", j);
		printf("DWIDTH %d 0\n", pd->width);

		p = glyphs + pd->addr;
		nb = (pd->left + pd->right + 7) / 8;

		/*
		 *  suppress trailing zeros
		 */
		for (j = pd->up + pd->down; --j >= 0; )
		{
			for (k = 0; k < nb; k++) {
				if (p[j*nb + k] != 0)
					break;
			}
			if (k == nb)
				(pd->down)--;
			else
				break;
		}

		/*
		 *  suppress leading zeros
		 */
		while (pd->up + pd->down)
		{
			for (k = 0; k < nb; k++) {
				if (p[k] != 0)
					break;
			}
			if (k == nb) {
				(pd->up)--;
				p += nb;
			} else
				break;
		}

		/*
		 *  suppress trailing bytes
		 */
		for (k = nb; --k >= 0; ) {
			int bits = 0;

			for (j = 0; j < pd->up + pd->down; j++)
				bits |= p[j*nb + k];
			if (bits != 0) {
				nba = k + 1;
				for (j = 7; j > 0; j--) {
					if (bits & (0x80 >> j))
						break;
				}
				pd->right = 8*k + j + 1 - pd->left;
				break;
			}
		}
		printf("BBX %d %d %d %d\n",
			pd->left + pd->right,
			pd->up + pd->down,
			-pd->left,
			-pd->down);
		printf("BITMAP\n");
		for (j = 0; j < pd->up + pd->down; ++j)
		{
			for (k = 0; k < nba; ++k)
				printf("%02X", p[j*nb + k]);
			printf("\n");
		}
		printf("ENDCHAR\n");
	}

	printf("ENDFONT\n");

	exit(0);
}
