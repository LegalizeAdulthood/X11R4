#define	DSTRINGSIZE	20

struct PALEDIT {
	Widget	topLevel;
	Widget	composite;
	Widget	box1;
	Widget	box2;
	Widget	slide1;
	Widget	slide2;
	Widget	slide3;
	Widget	label1;
	Widget	label2;
	Widget	label3;
	Widget	val1;
	Widget	val2;
	Widget	val3;
	Widget	palImage;
	
	XColor	*ccells;
	Widget	dEntry;
	char	dEntryString[DSTRINGSIZE];
	int	entry;
	Widget	vce;	/*View color entry */
};
