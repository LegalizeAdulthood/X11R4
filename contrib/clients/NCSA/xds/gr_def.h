/*
 * National Center for SuperComputing Applications, University of Illinois
 *
 * This NCSA software product is public domain software.  Permission
 * is hereby granted to do whatever you like with it. Should you wish
 * to make a contribution towards the production of this software, please
 * send us your comments about your experience with the software,  why
 * you liked or disliked it, how you use it, and most importantly, how it
 * helps your work. We will receive your comments at softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Eng-Whatt Toh, National Center for Supercomputing Applications
 *         ewtoh@ncsa.uiuc.edu
 */

/*
 *	File:		gr_def.h
 *	Contents:	Header file containing all the typedefs and defs used
 *				by the graphics module.
 */

#define MAX_ANILEV_WINDS	100
#define MAX_ARBLEV_WINDS	1
#define MAX_BOSLEV_WINDS	100
#define MAX_CUBLEV_WINDS	1
#define MAX_DICELEV_WINDS	100
#define MAX_DSPLEV_WINDS	100
#define MAX_HISTLEV_WINDS	100
#define MAX_ISOLEV_WINDS	100
#define MAX_FILLEV_WINDS	1
#define MAX_SUBSLEV_WINDS	100
#define MAX_TILLEV_WINDS	100
#define MAX_VIEWLEV_WINDS	100

#define MAX_TILLEV_TILES	15

/* typedef long XtTextPosition; */

typedef enum {
	AXES, ARBITRARY, DICER, ISO
} A_DisplayMode_t;

typedef enum {
	LOADRAS, LOADSDS, LOADPAL, LOADPALSPLIT, 
	SAVEANI, SAVEDICER, SAVEDISKA, SAVEDISKP, SAVEDSPL,
	SAVEISO, SAVEPROC, SAVETILE, SAVEVIEW, SAVEVBUFF
} A_FileAction_t;

typedef enum {
	AUTOOFF, AUTOREVERSE, AUTOREPEAT
} A_PlayMode_t;

typedef enum {
	ONEOFMANY, ANYOFMANY
} A_RCmode_t;

typedef enum {
	NOSCROLL, VERTONLY, HORIZONLY, VERTHORIZ
} A_Scroll_t;

typedef enum {
	SELECTABLE, NONSELECTABLE
} A_TextSelect_t;

typedef enum {
	FILESOURCE, STRINGSOURCE
} A_TextSource_t;

typedef enum {
	AUTO, UNFOLDED
} A_Tile_t;

typedef enum {
	DIAMOND, SQUARE
} A_ToggleShape_t;

typedef enum {
	ANIWIND, ARBWIND, BOSSWIND, CNTRLWIND, CUBEWIND, DICEWIND,
	DSPLWIND, HISTWIND, ISOWIND, FILEWIND, TILEWIND, SUBSWIND, VIEWWIND
} A_Wind_t;

typedef enum {
	ATHENA, HP
} A_Widget_t;

typedef struct A_SubsType_t {
	double	lower, upper, opacity;
} A_SubsType_t;

typedef struct A_BoxVert_t {
	int					id;
	double				x,y,z;
	struct A_BoxEdge_t 	*enode;
	struct A_BoxVert_t 	*next;
} A_BoxVert_t;

typedef struct A_BoxEdge_t {
	int					id;
	int					adj[7];
	struct A_BoxVert_t	*v1,*v2;
	struct A_BoxEdge_t	*next;
} A_BoxEdge_t;

typedef	struct A_Box_t	{
	int					numCuts;
	int					xdim,ydim,zdim;
	int					indepth;
	int					xpicsize,ypicsize;
	double				xorig,yorig,zorig;
	double				xlen,ylen,zlen;
	double				boundxmin,boundxmax;
	double				boundymin,boundymax;
	double				xmin,xmax;
	double				ymin,ymax;
	double				zmin,zmax;
	double				dimFactor;
	double				matrix[5][5];
	double				invmatrix[5][5];
	double				xangle,yangle;
	double				depth;
	struct A_BoxVert_t	*origVList;
	struct A_BoxVert_t	*vertList;
	struct A_BoxEdge_t	*edgeList;
	struct A_BoxVert_t	*cutVList;
	struct A_BoxEdge_t	*cutEList;
	struct A_Box_t		*next;
} A_Box_t;

typedef struct {
	/*	1:X 2:Y 3:Z */
	int row;
	int col;
	int	axis;
	int	plane;
} A_Axes_t;

typedef struct {
	unsigned long	white, black;
	int				nColors, maxColors;
	int				depth,shiftbits;
	Boolean			canSet;
	XColor			cmapCells[256];
	unsigned char	palette[768];
} A_Color_t;

typedef	struct A_AniWind_t {
	Widget	shell,win;
	Widget	imageVPort,imageWin;
	Widget	skipDialog;
	Widget	frameDialog;
	Widget	frameSlider;
	Widget	speedSlider;
	GC		imageWinGC;
	XImage	**ximage;
	Pixmap	*image;
	char	*pathname;
	char	**data;
	long	speed;
	int		skip;
	int		playStop;
	int		aniType;
	int		curplane,numplanes;
	int		*imagexsize,*imageysize;
	Boolean	usePixmap;
	Boolean useDisk;
	A_Axes_t 		axesOrient;
	A_PlayMode_t	autoMode;
	struct A_AniWind_t	*next,*prev;
	struct A_CubeWind_t	*parent;
	struct A_ArbWind_t	*arbparent;
}	A_AniWind_t;

typedef	struct A_FileWind_t {
	Widget	shell,win;
	Widget	fileVPort, fileWin;
	Widget	dirDialog;
	Widget	fileDialog;
	Widget	sdsDialog,rasStartDialog,rasEndDialog;
	Widget	scaleDialog;
	Widget	callShell;
	A_FileAction_t	mode;
	A_FileFormat_t	format;
	short	clicks;
	char	**fileData;
	Boolean	separateFiles;
	Boolean globalPalette;
	Boolean loadAllSDS;
	Boolean usePixmap;
	Boolean useDisk;
	struct A_AniWind_t	*aniWin;
	struct A_ArbWind_t	*arbWin;
	struct A_CubeWind_t	*cubeWin;
	struct A_DiceWind_t	*diceWin;
	struct A_DsplWind_t	*dsplWin;
	struct A_IsoWind_t	*isoWin;
	struct A_SubsWind_t	*subsWin;
	struct A_TileWind_t	*tileWin;
	struct A_ViewWind_t	*viewWin;
	struct A_FileWind_t	*next,*prev;
	struct A_TopWind_t	*parent;
}	A_FileWind_t;

typedef struct A_TileWind_t{
	Widget	shell,win;
	Widget	imageVPort[MAX_TILLEV_TILES],imageWin[MAX_TILLEV_TILES];
	XImage	*image[MAX_TILLEV_TILES];
	int		imagexsize[MAX_TILLEV_TILES];
	int		imageysize[MAX_TILLEV_TILES];
	char	*data[MAX_TILLEV_TILES];
	int		numTiles;
	int		oldX, oldY;
	int		oldInd;
	Pixel	oldPix[MAX_TILLEV_TILES];
	A_Axes_t axesOrient;
	A_Tile_t tileType;
	struct	A_TileWind_t	*next,*prev;
	struct	A_CubeWind_t	*parent;
}	A_TileWind_t;

typedef	struct A_DsplWind_t{
	Widget	shell,win;
	Widget	listVPort,listWin;
	Widget	hScaleVPort,hScaleWin;
	Widget	vScaleVPort,vScaleWin;
	Widget	imageVPort,imageWin;
	Boolean synchronize;
	int		width;
	int		xdim,ydim,zdim;
	int		selX,selY;
	int		oldX, oldY;
	int		oldInd;
	int		imagexsize,imageysize;
	Pixel	oldPix;
	XImage	*image;
	char	*data;
	char	**listData,**vScaleData,**hScaleData;
	int		numHistWins;
	A_Axes_t axesOrient;
	struct	A_HistWind_t	*histWin;
	struct	A_DsplWind_t	*next,*prev;
	struct	A_CubeWind_t	*parent;
}	A_DsplWind_t;

typedef	struct A_SubsWind_t{
	Widget	shell,win;
	Widget	IDLabel, IDSlider;
	Widget	runButton;
	Widget	lowerDialog, upperDialog,opacityDialog;
	Widget	maxDialog,gammaDialog,ambDialog,attDialog;
	Widget	nearDialog,farDialog,incrDialog;
	Widget	imageVPort,imageWin;
	XImage	*image;
	char	*data;
	Boolean	curCursor;
	int		runMode;	/* 0=reset, 1=running, 2=paused */
	int		xsize,ysize;
	int		numSubs;
	int		curSubsID;
	int		xpts,ypts,zpts;
	int		nearP,farP,incrP,curP;
	int		numdone,totalpoints;
	double	gamma,amb,att;
	double	outMax,incr;
	double	percentdone;
	double	depthcue,depthcueSquared;
	A_Axes_t axesOrient;
	A_Data_t *hdf;
	A_SubsType_t	*subs;
	A_VRect_t	**vbuff;
	struct	A_SubsWind_t	*next,*prev;
	struct	A_CubeWind_t	*parent;
}	A_SubsWind_t;

typedef	struct A_ViewWind_t{
	Widget	shell,win;
	Widget	imageVPort,imageWin;
	XImage	*image;
	char	*data;
	char	*palette;
	struct	A_ViewWind_t	*next,*prev;
	struct	A_ArbWind_t		*parent;
}	A_ViewWind_t;

typedef	struct A_HistWind_t{
	Widget	shell,win;
	Widget	imageVPort,imageWin;
	XImage	*image;
	char	*data;
	int		xsize,ysize,xpos,ypos;
	struct	A_HistWind_t	*next,*prev;
	struct	A_DsplWind_t	*parent;
}	A_HistWind_t;

typedef	struct A_DiceWind_t{
	Widget	shell,win;
	Widget	imageVPort,imageWin;
	Widget	shadeSlider;
	Widget	xSlider,ySlider,zSlider;
	Widget	xToggle, yToggle, zToggle;
	Widget	splaneDialog,fplaneDialog;
	XImage	*image;
	GC		gc1,gc2,gc3;
	int		scale;
	int		org,xdim,ydim,zdim;
	int		xsize,ysize,dimxsize,dimysize;
	int		xdimS,ydimS, zdimS;
	int		maxoff;
	int		planeType;	/* 1:X, 2:Y, 3:Z */
	int		xsplane,xfplane,ysplane,yfplane,zsplane,zfplane;
	char	**data;
	int		**fb;
	Boolean sliced,first,dicer;
	A_Coord_t	vert[8],slice[8];
	struct	A_DiceWind_t	*next,*prev;
	struct	A_BossWind_t	*parent;
}	A_DiceWind_t;

typedef	struct A_IsoWind_t{
	Widget	shell,win;
	Widget	imageVPort,imageWin;
	Widget	isoDialog;
	XImage	*image;
	GC		gc;
	char	**data;
	int		**fb;
	int		scale;
	int		xdim,ydim,zdim;
	int		xsize,ysize;
	int		xangle,yangle;
	Boolean	curCursor;
	double	matrix[5][5];
	A_Axes_t	axesOrient;
	struct	A_IsoWind_t	*next,*prev;
	struct	A_BossWind_t	*parent;
}	A_IsoWind_t;

typedef	struct A_ArbWind_t{
	Widget	shell,win;
	Widget	drawCubeWin;
	Widget	drawDepthWin;
	Widget	xSlider,ySlider,zSlider;
	Widget	scaleDialog;
	Widget	zpercentDialog;
	Widget	numFramesDialog;
	Boolean interp,usePixmap,useDisk;
	struct	A_Box_t	*xybox;
	struct	A_AniWind_t		*aniWin;
	struct	A_ViewWind_t	*viewWin;
	struct	A_ArbWind_t		*next,*prev;
	struct	A_BossWind_t	*parent;
	short	numViewWins;
	short	numAniWins;
}	A_ArbWind_t;

typedef	struct A_CubeWind_t{
	Widget	shell,win;
	Widget	drawWin;
	Widget	box2D,box3D;
	Widget	planeDialog;
	Widget	incrDialog;
	Widget	scaleDialog;
	Widget	widthDialog;
	Widget	numPlanesDialog;
	Widget	xptDialog,yptDialog,zptDialog,numSubsDialog;
	Boolean interp,view2D,isStep,usePixmap,useDisk;
	A_AniWind_t			*aniWin;
	A_DsplWind_t		*dsplWin;
	A_SubsWind_t		*subsWin;
	A_TileWind_t		*tileWin;
	A_Axes_t			axesOrient;
	short	numAniWins;
	short	numDsplWins;
	short	numTileWins;
	short	numSubsWins;
	struct	A_CubeWind_t	*next,*prev;
	struct	A_BossWind_t	*parent;
}	A_CubeWind_t;

typedef	struct A_BossWind_t{
	char	filename[STRNG40];
	Widget	shell,win;
	Widget	msgVPort,msgWin;
	Widget	minDialog,maxDialog,diceScaleDialog;
	A_DisplayMode_t	dsplMode;
	A_CubeWind_t	*cubeWin;
	A_ArbWind_t		*arbWin;
	A_DiceWind_t	*diceWin;
	A_IsoWind_t		*isoWin;
	A_Data_t		*data;
	char			*fileAttrib;
	short	numCubeWins;
	short	numArbWins;
	short	numDiceWins;
	short	numIsoWins;
	struct	A_BossWind_t	*next,*prev;
	struct	A_TopWind_t		*parent;
}	A_BossWind_t;

typedef	struct A_TopWind_t {
	Widget	shell,win;
	Widget	msgVPort, msgWin;
	short	numAniWins;
	short	numBossWins;
	short	numFileWins;
	struct	A_BossWind_t	*bossWin;
	struct	A_FileWind_t	*fileWin;
	struct	A_AniWind_t	*aniWin;
}	A_TopWind_t;
