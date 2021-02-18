/*
 *  File:   convert.c
 *  Author: Andrew Werth
 *
 *  Convert a raster from one file format to another.
 *
 *  This is a program which will read a raster stored in any one of
 *  the Raster Graphics Tool Kit formats and convert it to any other
 *  format.  The file I/O is done with UNIX "open()"-type calls.
 *  There are two methods of operation, interactive and
 *  by parameter.  If convert is called without any arguments, then
 *  interactive mode occurs.
 *
 */

#include <rgtk.h>
#include <stdio.h>
#include <fcntl.h>

#define READ	0
#define WRITE	1

#ifndef USE_LONG_NAMES

#define ReadFile    rdfile
#define WriteFile   wrfile
#define GetFormat   getfor

#define Interactive inactv

#endif

static RASTER_type *ReadFile();
static INT_type    WriteFile();
static INT_type GetFormat(/*format, ReadWrite*/);

main(argc,argv)
    int argc;
    char **argv;
{
    INIT_RGPARMS_type	Initialization;
    RASTER_type		*raster;
    FORMAT_type		FormatIn;
    FORMAT_type		FormatOut;
    FORMAT_type		*format;
    INT_type		arg;
    INT_type		fd;
    INT_type		filenum;
    CHAR_type		*fname,*fname2;
    CHAR_type		**file;

    if (LCL_ERROR_INT == LOCAL_Init()) {
	fprintf(stderr,"Can't Initialize Local\n");
	exit(-1);
    }

    Initialization.default_format = FORMAT_ANDREW;
    Initialization.memsize = -1;	/* Max memory usage */
    if (InitRGToolKit(&Initialization) == RG_ERROR_INT) {
	fprintf(stderr,"Can't Initialize Tool Kit\n");
	exit(-1);
    }

    if (argc < 2) {
	Interactive();
	goto CLEANUP;
    }

    fname = NULL;
    fname2 = NULL;
    filenum = 0;
    file = &fname;
    format = &FormatIn;
    format->format_tag = FORMAT_DETERMINE;

    for (arg = 1; arg < argc; arg++) {
	if (argv[arg][0] == '-') {
	    switch(argv[arg][1]) {
		case 'a':   format->format_tag = FORMAT_ANDREW;
			    break;
		case 'b':   format->format_tag = FORMAT_PBM;
			    break;
		case '3':   format->format_tag = FORMAT_GROUP3;
			    switch(argv[arg][2]) {
				case 'l': 
				 format->format_body.g3.msb=LEFTBIT;
				 break;
				case 'r':
				 format->format_body.g3.msb = 
							   RIGHTBIT;
				 break;
				default:  
				 fprintf(stderr,"Gr3 Error.\n");
				 goto CLEANUP;
			    }
			    format->format_body.g3.k =
					     argv[arg][3] - '0';
			    break;
		case '4':   format->format_tag = FORMAT_GROUP4;
			    switch(argv[arg][2]) {
				case 'l': 
				 format->format_body.g4.msb=LEFTBIT;
				 break;
				case 'r':
				 format->format_body.g4.msb =
							   RIGHTBIT;
				 break;
				default:  
				 fprintf(stderr,"Gr4 Error.\n");
				 goto CLEANUP;
			    }
			    sscanf(&(argv[arg][3]),"%d",
				&format->format_body.g4.width);
			    break;
		case 'g':   format->format_tag = FORMAT_GIF;
			    break;
		case 'i':   format->format_tag = FORMAT_IFF;
			    break;
		case 'c':   format->format_tag = FORMAT_PCX;
			    break;
		case 's':   format->format_tag = FORMAT_SUN;
			    break;
		case 'o':   format->format_tag = FORMAT_ODABM;
			    sscanf(&(argv[arg][2]),"%d",
			     &format->format_body.oda_bitmap.width);
			    break;
		case 'p':   format->format_tag = FORMAT_POSTSCRIPT;
			    sscanf(&(argv[arg][2]),"%f",
				&format->format_body.ps.scale);
			    break;
		case 'x':   format->format_tag = FORMAT_XBM;
			    format->format_body.xbm.name = "xbm";
			    break;
		case 'm':   format->format_tag = FORMAT_MACPAINT;
			    if (argv[arg][2] == '1' || 
				argv[arg][2] == '2') {
				format->format_body.macpaint.macbin
				    = argv[arg][2] - '0';
			    }
			    else {
				format->format_body.macpaint.macbin
				    = 0;
			    }
			    break;
		case 'h':   goto HELP;
		default:    fprintf(stderr,"Bad parameters.\n");
			    goto CLEANUP;
	    }
	    if (++filenum == 2) {
		file = &fname2;
	    }
	    format = &FormatOut;
	}
	else {
	    *file = argv[arg];
	    file = &fname2;
	    format = &FormatOut;
	}
    }

    if (fname == NULL) {
	fd = 0;		    /* Use stdin */
    }
    else {
	fd = open(fname,O_RDONLY);
	if (fd < 0) {
	    fprintf(stderr,"Could not open file %s.\n",fname);
	    goto CLEANUP;
	}
	fprintf(stderr,"Reading file %s...\n",fname);
    }

    raster = ReadRaster(fd, &FormatIn);
    if (raster == RG_ERROR_RASTER) {
	fprintf(stderr,"The file format is incorrect!\n");
	goto CLEANUP;
    }

    close(fd);

    if (fname2 == NULL) {
	fd = 1;		    /* Use stdout */
    }
    else {
        fd = open(fname2,O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
	    fprintf(stderr,"Could not create file %s.\n",fname2);
	    goto CLEANUP;
	}
	fprintf(stderr,"Writing file %s...\n",fname2);
    }

    if (WriteRaster(fd, raster, &FormatOut) == RG_ERROR_INT) {
	fprintf(stderr,"Error in writing raster.\n");
    }
    LOCAL_WriteBytes(fd, 0, -1);
    close(fd);

CLEANUP:

    FinalRGToolKit();
    LOCAL_Final();

    goto LEAVE;

HELP:

    printf("Usage:\n\n");
    printf("  convert [-FormatIn] [<inputfile>] [-FormatOut] [<outputfile>]\n\n");

    printf(
	"The values FormatIn and FormatOut determine which\n");
    printf(
	"formats will be used for reading and writing the\n");
    printf(
	"raster.  Legal values are:\n\n");
    printf(
	"    b	    Portable Bitmap Format (PBM)\n");
    printf(
	"    g	    CompuServe's GIF\n");
    printf(
	"    i	    Amiga IFF\n");
    printf(
	"    s	    Sun Raster Format\n");
    printf(
	"    a	    Andrew .raster Format\n");
    printf(
	"    o?	    ODA Bitmap Encoding Scheme\n");
    printf(
	"    x	    X11 Bitmap\n");
    printf(
	"    m?	    MacPaint Document\n");
    printf(
	"    3??    Group 3 Facsimilie Encoding (CCITT T.4)\n");
    printf(
	"    4??    Group 4 Facsimilie Encoding (CCITT T.6)\n");
    printf(
	"    p?	    PostScript (output only)\n\n");
    printf(
	"Some of these formats require additional specifiers,\n");
    printf(
	"which must be entered immediately after the format\n");
    printf(
	"character (no spaces or dashes).  These specifiers\n");
    printf(
	"are designated by '?' in the above table.  They are:\n\n");
    printf(
	"    o?	    The ? must be replaced by the width of the\n");
    printf(
	"	    raster (on reads only).\n");
    printf(
	"    3??    The first ? must be an 'l' or an 'r', to\n");
    printf(
	"	    indicate left or right bit being the MSB in\n");
    printf(
	"	    bytes.  The second ? must be either a '1',\n");
    printf(
	"	    a '2', or a '4', depending on what value\n");
    printf(
	"	    for k the raster is/should be stored in.\n");
    printf(
	"    4??    The first ? must be an 'l' or an 'r', to\n");
    printf(
	"	    indicate left or right bit being the MSB in\n");
    printf(
	"	    bytes.  The second ? must be replaced by\n");
    printf(
	"	    width of the raster (on reads only).\n");
    printf(
	"    m?     The ? may be replaced by a '1' or a '2'\n");
    printf(
	"           to indicate MacBinary formatting was used\n");
    printf(
	"	    (for reads only).\n");
    printf(
	"    p?	    The ? may be replaced by a floating point\n");
    printf(
	"           scaling factor to change the size of the\n");
    printf(
	"	    produced image\n\n");
    printf(
	"If no filenames are specified, then standard in and\n");
    printf(
	"standard out are used to read or write the raster.\n");
    printf(
	"If no parameters are specified, then the user will be\n");
    printf(
	"placed into interactive mode and will be prompted for\n");
    printf(
	"conversion information.\n\n");

LEAVE:
    exit(0);
}


static Interactive()
{
    CHAR_type	command[1024];
    RASTER_type	*raster;
    RASTER_type	*raster1;
    BOOL_type	Quit;

    printf(
	"\n\nInteractive Mode\n----------------\n");
    printf(
	"At the \"convert:\" prompt, type in either\n");
    printf(
	"\"read <filename>\" or \"write <filename>\".  You will\n");
    printf(
	"be asked what format you want to use.  Type the letter\n");
    printf(
	"of the format to use, or just press ENTER to have the\n");
    printf(
	"computer try to determine the format.\n");
    printf(
	"Read operations will read a raster into memory, and\n");
    printf(
	"write operations will write whatever raster is in\n");
    printf(
	"memory out to a file.  You may write the same raster\n");
    printf(
	"out several times, in different formats.\n\n");
    printf(
	"Type \"quit\" to exit the program.\n\n");

    Quit = BOOL_false;
    while (Quit == BOOL_false) {
	printf("\nconvert: ");
	fflush(stdout);
	gets(command);
	if (command[0] == 'q') {
	    Quit = BOOL_true;
	}
	else if (command[0] == 'r') {
	    raster1 = ReadFile(command);
	    if (raster1 != RG_ERROR_RASTER) {
		raster = raster1;
		printf("  Completed.\n");
	    }
	    else {
		printf("  Error reading file.\n");
	    }
	}
	else if (command[0] == 'w') {
	    if (WriteFile(command, raster) == RG_ERROR_INT) {
		printf("  Error writing file.\n");
	    }
	    else {
		printf("  Completed.\n");
	    }
	}
    }
    printf("\nLeaving interactive mode.\n\n");
}

static RASTER_type *ReadFile(command)
    CHAR_type	*command;
{
    RASTER_type	*rval;
    CHAR_type	*fname;
    INT_type	fd;
    FORMAT_type	format;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    GetFormat(&format,READ);
    fname = command;

    while (*fname != ' ') {
	fname++;
    }

    fname++;
    if ((fd = open(fname,O_RDONLY)) < 0) {
	fprintf(stderr,"Cannot open file %s\n",fname);
	rval = RG_ERROR_RASTER;
	goto LEAVE;
    }

    printf("Reading...");
    fflush(stdout);
    rval = ReadRaster(fd, &format);
    LOCAL_ReadBytes(fd, 0, -1);
    close(fd);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


static INT_type WriteFile(command, raster)
    CHAR_type	*command;
    RASTER_type	*raster;
{
    INT_type	rval;
    CHAR_type	*fname;
    INT_type	fd;
    FORMAT_type	Format;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    GetFormat(&Format,WRITE);
    fname = command;

    while (*fname != ' ') {
	fname++;
    }

    fname++;
    if ((fd = open(fname,O_WRONLY | O_CREAT | O_TRUNC,0666)) < 0) {
	fprintf(stderr,"Cannot open file %s\n",fname);
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    printf("Writing file...");
    fflush(stdout);
    rval = WriteRaster(fd, raster, &Format);
    Format.format_tag++;
    LOCAL_WriteBytes(fd, 0, -1);
    close(fd);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


static INT_type GetFormat(format, ReadWrite)
    FORMAT_type	*format;
    INT_type	ReadWrite;
{
    INT_type	rval;
    CHAR_type	inform[10];

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    printf("Enter format code (default = guess): ");
    fflush(stdout);
    gets(inform);
    if (inform[0] == '\0' || inform[0] == '\n') {
	format->format_tag = FORMAT_DETERMINE;
    }
    else {
      switch (inform[0]) {
	case 'a':   
	    format->format_tag = FORMAT_ANDREW;
	    if (ReadWrite == WRITE) {
		printf("Enter object id (default = 0): ");
		fflush(stdout);
		gets(inform);
		if (inform[0] < '0' || inform[0] > '9') {
		    format->format_body.andrew.object_id = 0;
		}
		else {
		    sscanf(inform,"%d",
			&format->format_body.andrew.object_id);
		}	    
	    }
	    break;
	case 'b':
	    format->format_tag = FORMAT_PBM;
	    break;
	case '3':   
	    format->format_tag = FORMAT_GROUP3;
	    printf(
	     "Enter 'l' or 'r' for LEFT or RIGHT bit allignment: ");
	    fflush(stdout);
	    gets(inform);
	    switch(inform[0]) {
		case 'l': 
		    format->format_body.g3.msb = LEFTBIT;
		    break;
		case 'r':
		    format->format_body.g3.msb = RIGHTBIT;
		    break;
		default:  
		    fprintf(stderr,"Invalid parameters.\n");
		    rval = RG_ERROR_INT;
		    goto LEAVE;
	    }
	    printf("Enter value for k (1, 2, or 4): ");
	    fflush(stdout);
	    gets(inform);
	    format->format_body.g3.k = inform[0] - '0';
	    break;	    
	case '4':
	    format->format_tag = FORMAT_GROUP4;
	    printf(
	     "Enter 'l' or 'r' for LEFT or RIGHT bit allignment: ");
	    fflush(stdout);
	    gets(inform);
	    switch(inform[0]) {
		case 'l': 
		    format->format_body.g4.msb = LEFTBIT;
		    break;
		case 'r':
		    format->format_body.g4.msb = RIGHTBIT;
		    break;
		default:  
		    fprintf(stderr,"Invalid parameters.\n");
		    rval = RG_ERROR_INT;
		    goto LEAVE;
	    }
	    if (ReadWrite == READ) {
		printf("Enter the width of the raster: ");
		fflush(stdout);
		gets(inform);
		sscanf(inform,"%d",&format->format_body.g4.width);
	    }
	    break;
	case 'g':
	    format->format_tag = FORMAT_GIF;
	    break;
	case 'i':
	    format->format_tag = FORMAT_IFF;
	    break;
	case 'c':   
	    format->format_tag = FORMAT_PCX;
	    break;
	case 's':
	    format->format_tag = FORMAT_SUN;
	    break;
	case 'o':
	    format->format_tag = FORMAT_ODABM;
	    if (ReadWrite == READ) {
		printf("Enter the width of the raster: ");
		fflush(stdout);
		gets(inform);
		sscanf(inform,"%d",
		    &format->format_body.oda_bitmap.width);
	    }
	    break;
	case 'p':
	    format->format_tag = FORMAT_POSTSCRIPT;
	    printf("Enter the scale of the raster: ");
	    fflush(stdout);
	    gets(inform);
	    sscanf(inform, "%f", &format->format_body.ps.scale);
	    break;
	case 'x':
	    format->format_tag = FORMAT_XBM;
	    format->format_body.xbm.name = "xbm";
	    break;
	case 'm':
	    format->format_tag = FORMAT_MACPAINT;
	    break;
	default:
	    fprintf(stderr,"Bad parameters.\n");
	    rval = RG_ERROR_INT;
	    goto LEAVE;
      }
    }
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}




/*  End of file .c  */
