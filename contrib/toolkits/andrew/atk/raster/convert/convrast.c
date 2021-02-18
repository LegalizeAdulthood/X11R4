/*LIBS: -lbasics -lclass -lmalloc -lerrors -lutil
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/raster/convert/RCS/convrast.c,v 2.7 89/04/20 14:01:16 pgc Exp Locker: pgc $ */
/* $ACIS:convrast.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/raster/convert/RCS/convrast.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/raster/convert/RCS/convrast.c,v 2.7 89/04/20 14:01:16 pgc Exp Locker: pgc $";
#endif /* lint */

/* convrast.c - program to convert raster formats

    convertraster [-nulr] [-c(x,y,w,h)] [-intype=type] [-outtype=type] 
		[-infile=filename] [-outfile=filename]

    Convertraster reads the infile, generates an internal representation of the raster, 
    processes it as described by the switches, and writes the outfile.  

    If infile is omitted, standard input is read.  
    If outfile is omitted, statndard output is written.

    Switch definitions:

	-c(x,y,w,h)  Crop the output to the subimage of size (w, h) at (x,y).
	-n	Negative image.
	-u	Reflect across horizontal axis. (Swap Up and down). 
	-l	Reflect across vertical axis.  (Swap Left and right).
	-r	Rotate 90 degrees clockwise.  Multiple r's rotate 180, 270, . . .

    Cropping is done before the other processing.

    The 'type's supported are: 

	MacPaint  - the format produced by MacPaint.
	RF - the old ITC RasterFile format
	raster - the BE2 .raster format
	PostScript - print data stream;  not supported for intype

    If the output type is not specified, a .raster file is generated.  
    If the input type is not specified, the program will examine 
    the first byte and use RF for 0xF1 and raster for '\'.  If the first
    byte doesn't help, the program halts.
*/


#include <stdio.h>
#include <andrewos.h> /* strings.h */
#include <ctype.h>
#include <rect.h>

#include <class.h>
#include <pixelimg.ih>
#include <heximage.ih>
#include <paint.ih>
#include <oldrf.ih>
#include <rasterio.ih>

/* include the ones utilized, but not by this .c file itself */
#define class_StaticEntriesOnly
#include <observe.ih>
#include <proctbl.ih>
#include <dataobj.ih>
#undef class_StaticEntriesOnly


char inname[1025], outname[1025];

enum rasterType {
	typeUnknown,
	typeMacPaint,
	typeRF,
	typeraster,
	typePostscript
}  inType, outType;

char opSwitches[20];	/* -runl */
struct rectangle crop;	/* -c(l,t,w,h) */


	static void
ProcessPix(pix)
	struct pixelimage *pix;
{
	struct pixelimage *tix;
	char *sx;
	struct rectangle r;

	if ( ! rectangle_IsEmptyRect(&crop)) {
		/* do the cropping by replacing the bits area of the pix  XXX */
		long buf[1000];
		register long row;

		tix = pixelimage_New();
		pixelimage_Resize(tix, crop.width, crop.height);
		/* copy sub image to 'tix' */
		for (row = crop.top; row < crop.top+crop.height; row++) {
			pixelimage_GetRow(pix, crop.left, row, crop.width, (short *)buf);
			pixelimage_SetRow(tix, 0, row-crop.top, crop.width, (short *)buf);
		}
		/* remove the bits from 'pix' */
		free(pixelimage_GetBitsPtr(pix));
		pixelimage_SetBitsPtr(pix, NULL);
		/* resize 'pix' and give it the bits from 'tix' */
		pixelimage_Resize(pix, crop.width, crop.height);
		free(pixelimage_GetBitsPtr(pix));
		pixelimage_SetBitsPtr(pix, pixelimage_GetBitsPtr(tix));
		/* get rid of 'tix' */
		pixelimage_SetBitsPtr(tix, NULL);
		pixelimage_Destroy(tix);
	}

	for (sx = opSwitches; *sx; sx++) {
		rectangle_SetRectSize(&r, 0, 0, 
				pixelimage_GetWidth(pix),
				pixelimage_GetHeight(pix));
		switch (*sx) {
		case 'n':	pixelimage_InvertSubraster(pix, &r); break;
		case 'u':	pixelimage_MirrorUDSubraster(pix, &r); break;
		case 'l':	pixelimage_MirrorLRSubraster(pix, &r); break;
		case 'r':
			tix = pixelimage_New();	
			pixelimage_GetRotatedSubraster(pix, &r, tix);
			/* remove the bits from 'pix' */
			free(pixelimage_GetBitsPtr(pix));
			pixelimage_SetBitsPtr(pix, NULL);
			/* resize 'pix' and give it the bits from 'tix' */
			pixelimage_Resize(pix, r.height, r.width);
			free(pixelimage_GetBitsPtr(pix));
			pixelimage_SetBitsPtr(pix, pixelimage_GetBitsPtr(tix));
			/* get rid of 'tix' */
			pixelimage_SetBitsPtr(tix, NULL);
			pixelimage_Destroy(tix);
			break;
		}
	}
}

	static long
ReadInputFile(InputFile, pix)
	FILE * InputFile;
	struct pixelimage *pix;
{
	switch (inType) {
	case typePostscript: 
	case typeUnknown: 
		return dataobject_BADFORMAT;
	case typeraster: 
		return rasterio_ReadImage(InputFile, pix);
	case typeRF: 
		return oldRF_ReadImage(InputFile, pix);
	case typeMacPaint: 
		return paint_ReadImage(InputFile, pix);
	}
}

	static void
WriteOutputFile(OutputFile, pix)
	FILE * OutputFile;
	struct pixelimage *pix;
{
	struct rectangle r;
	rectangle_SetRectSize(&r, 0, 0, pixelimage_GetWidth(pix), pixelimage_GetHeight(pix));

	switch (outType) {
	case typeUnknown: 
		return;
	case typeraster: 
		rasterio_WriteImage(OutputFile, pix, &r);
		return;
	case typeRF: 
		oldRF_WriteImage(OutputFile, pix, &r);
		return;
	case typeMacPaint: 
		paint_WriteImage(OutputFile, pix, &r);
		return;
	case typePostscript: 
		heximage_WritePostscript(OutputFile, pix, &r, 0.5, 0.5);
		return;
	}
}




	static void
fail(msg)
	char *msg;
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}


	static void
usage()
{
	fprintf(stderr, "usage: convertraster [-ruln] [-c(x,y,w,h)]");
		fprintf(stderr, " [intype=type] [infile=file] [outtype=type] [outfile=file]\n");
	fprintf(stderr, "usage:    Types are MacPaint, RF, raster, and Postscript\n");
			     exit(2);
}


struct symentry {	
	char symbol[12];		/* string to recognize */
	void (*f)();		/* function to call */
	char *target;		/* where the function assigns value */
	enum rasterType v;	/* value the function assigns */
};

static struct symentry *FindSym();


	static void
storename(arg, sym)
	char *arg;
	struct symentry *sym;
{
	strcpy(sym->target, arg+1);
	if (strlen(sym->target) > 1023)
		fail ("file name too long");
}
	static void
storetype(arg, sym)
	char *arg;
	struct symentry *sym;
{
	struct symentry *typesym;
	char buf[20];
	strcpy(buf, arg+1);
	if (strlen(buf) > 18)
		fail ("type name too long");
	typesym = FindSym(buf);
	if (typesym == NULL)
		fail ("unknown type");
	*((enum rasterType *)sym->target) = typesym->v;			/* ! store a type value */
	
}


	static struct symentry 
SymTable[] = {
	"infile", storename, inname, typeUnknown,
	"outfile", storename, outname, typeUnknown,
	"intype", storetype, (char *)&inType, typeUnknown,
	"outtype", storetype, (char *)&outType, typeUnknown,
	"macpaint", NULL, NULL, typeMacPaint,
	"macp", NULL, NULL, typeMacPaint,
	"mp", NULL, NULL, typeMacPaint,
	"rf", NULL, NULL, typeRF,
	"ras", NULL, NULL, typeRF,
	"rasterfile", NULL, NULL, typeRF,
	"raster", NULL, NULL, typeraster,
	"be2", NULL, NULL, typeraster,
	"ps", NULL, NULL, typePostscript,
	"postscript", NULL, NULL, typePostscript,
	"", NULL, NULL, typeUnknown
};

	static struct symentry *
FindSym(s)
	char *s;
{
	char buf[12];
	char *bx = buf;
	struct symentry *tx;
	for ( ; *s && bx - buf < 10; s++)
		if (islower(*s)) *bx++ = *s;
		else if (isupper(*s)) *bx++ = tolower(*s);
	*bx++ = '\0';
	for (tx = SymTable; *tx->symbol; tx++)
		if (strcmp(tx->symbol, buf) == 0) 
			return tx;
	return NULL;
}


	static void
ParseSwitches(argc, argv)
	int argc;
	char **argv;
{
	struct symentry *sym;
	char buf[20];
	char *bx, *sx;
	char *arg;

	while (--argc > 0)  {
		arg = *++argv;
		if (*arg == '-')  arg++;		/* '-' is optional  !!! */
		switch (*arg) {
		case 'c': {
			long left, top, width, height;
			if (sscanf(arg+1, "(%d,%d,%d,%d)", 
					&left, &top, &width, &height) != 4)
				fail("crop with  -c(left,top,width,height)");
			rectangle_SetRectSize(&crop, left, top, width, height);
			} break;
		case 'n':
		case 'r':
		case 'u':
		case 'l':
			strcat(opSwitches, arg);
			if (strlen(opSwitches) > 18) 
				fail ("Too many n,l,r,u operations");
			break;
		case 'i':
		case 'o':
			for (bx = buf, sx = arg; 
				isalpha(*sx) && bx - buf < 18; 
				*bx++ = *sx++) {}
			*bx++ = '\0';
			sym = FindSym(buf);
			if (sym != NULL && sym->f != NULL)
				(sym->f)(sx, sym);
			else usage();
			break;	

		default: 
			usage();
		}
	}
}


	static FILE *
OpenInputFile()
{
	FILE *infile;
	register c;

	if ( ! *inname)  
		infile = stdin;
	else
		infile = fopen(inname, "r");
	if (infile == NULL) 
		return NULL;

	/* check first byte and possibly change inType */
	ungetc((c=getc(infile)), infile);
	if (c == 0xF1)  inType = typeRF;
	else if (c == '\\') inType = typeraster;

	return infile;
}

	static FILE *
OpenOutputFile()
{
	unsigned char *ext = (unsigned char *)rindex(outname, '.');
	if (*outname) {
		if (rindex == NULL)
			fail("Output filename must have an extension");
		else switch (outType) {
		case typeraster:
			if (strcmp(ext, ".raster") != 0)
				fail("Output file extension for raster files must be \".raster\"");
			break;
		case typeRF:
			if (strcmp(ext, ".ras") != 0)
				fail("Output file extension for old RasterFile format must be \".ras\"");
			break;
		case typeMacPaint:
			if (strcmp(ext, ".mp") != 0)
				fail("Output file extension for MacPaint files must be \".mp\"");
			break;
		case typePostscript:
			if (strcmp(ext, ".ps") != 0)
				fail("Output file extension for Postscript files must be \".ps\"");
			break;
		}
		return fopen(outname, "w");
	}
	else return stdout;
}


main(argc, argv)
	int argc;
	char **argv;
{
	FILE *infile, *outfile;
	long ret;
	struct pixelimage *pix;

	class_Init(AndrewDir("/dlib/atk"));		/* directory for dataobject path  */

	observable_StaticEntry;
	proctable_StaticEntry;
	dataobject_StaticEntry;

	*inname = '\0';
	*outname = '\0';
	inType = typeUnknown;
	outType = typeraster;
	*opSwitches = '\0';
	rectangle_EmptyRect(&crop);

	ParseSwitches(argc, argv);

	if (inType == typePostscript)
		fail("convertraster can write Postscript, but cannot read it.");
	infile = OpenInputFile();
	if (infile == NULL)
		fail("Cannot open infile");
	if (inType == typeUnknown)
		fail("Input file is not Raster or RasterFile.  Do you want 'intype=macpaint'?");

	outfile = OpenOutputFile();
	if (outfile == NULL) 
		fail ("Cannot open outfile"); 

	fprintf(stderr, "%s->%s\n", 
		(*inname) ? inname : "stdin" , 
		(*outname) ? outname : "stdout" );
	if ( ! rectangle_IsEmptyRect(&crop))
		fprintf(stderr, "	Crop input to (%d, %d, %d,%d)\n",
				crop.left, crop.top, crop.width, crop.height);
	if (*opSwitches) 
		fprintf(stderr, "	Process with \"%s\"\n", opSwitches);

	pix = pixelimage_New();

	ret = ReadInputFile(infile, pix);
	if (ret != dataobject_NOREADERROR) {
		fprintf (stderr, "Read of %s failed with code %d\n", inname, ret);
		exit(3);
	}
	fclose(infile);

	ProcessPix(pix);

	WriteOutputFile(outfile, pix);
	fclose(outfile);
}
