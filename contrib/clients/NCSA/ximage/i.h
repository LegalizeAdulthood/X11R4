#define DISPLAY_CART    1
#define DISPLAY_POLAR   2

struct	IWS {
	int	amIShowing;
	Widget	toplevel;
	Widget	composite;
	Widget	box1;
	Widget	box2;
	Widget	box3;
	Widget	box4;
	Widget	box5;
	Widget	box6;
	Widget	box7;
	Widget	label;
	Widget	title;
	Widget	pal_win;
	Colormap cmap;
	char	lockcmap;
	char	*pdata;
	char	*pdata2;
	char	*pdata3;
	char	*pdata4;
	char	*pdata5;
	char	*pdata6;
	int	data1;
	int	data2;
	int	data3;
	};

struct	IMAGE {
	char	*name;		/* file name of image */
	int	isHDF;		/* file format of image */
	int	xdim,ydim;	/* X and Y dimensions */

	char	label[2];

	char	isFloat;	/* is this IMAGE in float? */
	float	*fdataset;	/* floating point data set */
	float	maxvalue;	/* maximum value only used if float*/
	float	minvalue;	/* minimum value only used if float*/

	char	*format;	/* printf format for printing */
	int	fwidth;		/* format width (maximum # char of output) */

	unsigned char	*ri;	/* raster image data set stored here */
	unsigned char *dExpRI;	/* depth expantion ri (16,24,etc bit)*/
	char	isDExpRI;	/* is ri depth expanded */
	XImage	*cimage;	/* cartesian XImage structure here */

	char	*polarimage;	/* polar image */
	unsigned char *dExpPI;	/* depth expantion polar (16,24,etc bit)*/
	char	isDExpPI;	/* is polar Image depth expanded */
	XImage  *pimage;	/* polar XImage structure here */

	char	plotCartesian;	/* Plot cartesian image */
	char	plotSepWindow;	/* Plot graphs in seperate window */
	unsigned char	*contourLevels;

	struct	IWS	cws;	/* control window widgets */
	struct	IWS	iws;	/* image window widgets */
	struct	IWS	pws;	/* polar window widgets */
	struct	IWS	dws;	/* data window widgets */
	struct	IWS	gws;	/* Graph control box */
	struct	IWS	gcws;	/* graph contour widgets */
	struct	IWS	giws;	/* graph intensity widgets */
	struct	IWS	gsws;	/* graph shaded widgets */
	struct	IWS	ghws;	/* graph histogram widgets */
	char	ctrl_showing;
	char	data_showing;
	char	cart_showing;
	char	polar_showing;
	char	graph_showing;

	char	hasPalette;
	XColor	*palette;

	Widget	pal_win;	/* palette image */
	GC	iwgc;		/* image widget GC */
	Visual	*ivis;		/* image visual */
	XImage	*i;		/* X image struct */
	char	image_loaded;	/* is the image loaded? */
	char	image_showing;	/* is the image window showing? */

	struct IMAGE *prev;	/* next image structure */
	struct IMAGE *next;	/* next image structure */

	};
