/*
 *	File:		xds.h
 *	Contents:	Global include file for all modules of xds
 *
 */

#define	STRNG40			40
#define STRNG80			80
#define	STRNG160		160
#define STRNG256		256
#define	RESERVED_COLORS	2

#define XDS_GREATER(A,B)	((A) > (B) ? (A) : (B))
#define XDS_SMALLER(A,B)	((A) < (B) ? (A) : (B))

/* Data to pixel conversion macros */
#define	XDS_CNV2PIX(A,B,C,D,E)	\
	( (A <= B) ? (unsigned char) RESERVED_COLORS : \
	 ((A >= C) ? (unsigned char) RESERVED_COLORS+E : \
				 (unsigned char)(RESERVED_COLORS+(A-B)*D)) )

#define	XDS_CNV2SPLIT(A,B,C,D,E,F)	\
	( (A <= B) ? (unsigned char) RESERVED_COLORS+D : \
	 ((A >= C) ? (unsigned char) RESERVED_COLORS+E+F : \
				 (unsigned char)(RESERVED_COLORS+F+(A-B)*D)) )

/* Data Structure */
typedef struct {
	int		rank;
	int		scale;
	int		numSDs;
	int32	dims[3];
	float32	min,max,range,rangeFrac,rangeFracSplit;
	float32	***data;
	char	*pixData;
	char	label[3][STRNG40];
	char	units[3][STRNG40];
	char	fmt[3][10];
	char	pathName[STRNG80];
} A_Data_t;

typedef struct A_Coord_t {
    int x,y,z;
} A_Coord_t;

typedef struct A_VRect_t {
    double  intensity;
    double  opacity;
} A_VRect_t;

typedef enum {
	HDF, MIRIAD
} A_FileFormat_t;

static	char	*axesLabels[3] = {"X", "Y", "Z"};
char	msg[256];
