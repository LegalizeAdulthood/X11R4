
#define A_STOP	0
#define	A_CFOR	1
#define	A_CREV	2
#define	DSTRINGSIZE	10		/* dialog string size */

struct	ANIMATION {
	Widget	topLevel;
	Widget	composite;
	Widget	pal_win;
	Widget	slidebar;		/* valuator widget */

	char	*name;
	int	xdim,ydim;
	char	isHDF;
	char	isFloat;
	char	*directory;
	char	**files;
	int	frameNum;	/* which frame are we on? */
	int	numFrames;	/* number of frames */
	int	numFiles;
	char	inMemory;	/* images will be stored in pixmaps */
	int	*frameIndex;	/* only used with HDF single file animations*/

	char	ctrl_showing;	/* is animate control window showing? */

	unsigned char *raster;
	unsigned char *expRaster;	/* expanded raster */
	unsigned char isExpanded;	/* is raster expanded */
	XImage	*image;
	Pixmap	*pixmaps;

	char	hasPalette;
	XColor	*palette;	/* XColor[256] (colors for window)*/
	Colormap cmap;
	char	lockcmap;

	char	status;	
	int	skip;		/* what is the skip factor */
	int	delay;
	char	repeat;		/* should repeat */

	Widget	bcrev;	/*button continuous reverse*/
	Widget	bsrev;	/*button single reverse*/
	Widget	bstop;	/*button stop */
	Widget	bsfor;	/*button single forward */
	Widget	bcfor;	/*button continous forward */
	Widget	dFrame;
	char	dFrameString[DSTRINGSIZE];
	Widget	dSkip;
	char	dSkipString[DSTRINGSIZE];
	Widget	dDelay;
	char	dDelayString[DSTRINGSIZE];

	Widget	awin;	/* animation window */
	GC	awinGC;

	Widget	topQbox;	/* top widget for question box */

	struct ANIMATION *next;
	struct ANIMATION *prev;
};

