/*
** tifftopgm.c
**
**      Copyright 1988 by Paul J. Emerson
**
**      Permission to use, copy, modify, and distribute this software
**      and its documentation for any purpose and without fee is
**      hereby granted, provided that the above copyright notice
**      appear in all copies and that both that copyright notice and
**      this permission notice appear in supporting documentation, and
**      that the name of Paul J. Emerson not be used in advertising or
**      publicity pertaining to distribution of the software without
**      specific, written prior permission.  Paul J. Emerson makes no
**      representations about the suitability of this software for
**      any purpose.  It is provided "as is" without express or implied
**      warranty.
*/

#include <stdio.h>
#include <fcntl.h>
#include "tiff.h"
#include "pgm.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    int argn, dump;
    struct Tiffstruct Tiff;
    struct TiffHeader Header;
    char *usage = "[-h] [tifffile]";

    pm_progname = argv[0];

    argn = 1;
    dump = 0;

    if ( argn < argc && argv[argn][0] == '-' )
	{
	if ( argv[argn][1] == 'h' && argv[argn][2] == '\0' )
	    dump = 1;
	else
	    pm_usage( usage );
	argn++;
	}

    if ( argn < argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    if ( readTiff( &Tiff, &Header, ifd ) )
	{
	if ( dump )
	    dumpHeader( &Tiff, &Header );
	PgmOut( ifd, &Tiff );
	}
    else
	pm_error( "readTiff failure", 0,0,0,0,0 );

    pm_close( ifd );
    exit( 0 );
    }

/*
**  Send image to stdout in PGM format
*/
PgmOut( fd, t )
FILE *fd;
struct Tiffstruct *t;
    {
    register int i, row;
    char *bptr;
    LONG *cntptr, count;
    LONG *dataptr;
    gray **grays;
    int maxval;
    extern char *malloc();

    grays = pgm_allocarray( t->ImageWidth, t->ImageLength );

    dataptr = t->StripOffset;
    cntptr  = t->SBytesCntOffset;
    if ( cntptr == (LONG *) 0 )
	{ /* Fucking AppleScan writes bogus TIFF files... */
	fprintf( stderr, "Missing StripByteCounts, proceeding to guess...\n" );
	count = ( t->ImageWidth * t->ImageLength * t->BitsPerSample + 7 ) / 8 / t->StOffsetCnt;
	}
    else
	count = *cntptr;
    maxval = ( 1 << t->BitsPerSample ) - 1;
    if ( maxval > PGM_MAXMAXVAL )
	pm_error( "BitsPerSample is too large - try recompiling with a larger gray type" );

    for ( i=0, row=0; i < t->StOffsetCnt; i++, row += t->RowsStrip )
	{
	fseek( fd, (long) *dataptr, 0);
	bptr = malloc( count );
	if ( bptr == 0 )
	    pm_error( "out of memory", 0,0,0,0,0 );
	if  ( fread( bptr, 1, count, fd ) != count )
	    pm_perror( "StripOffset read" );

	PgmDoStrip(
	    bptr, count, grays, row, t->ImageWidth, t->BitsPerSample,
	    (gray) maxval );

	free( bptr );
	if ( cntptr != (LONG *) 0 )
	    count = *(++cntptr);
	dataptr++;
	}

    pgm_writepgm( stdout, grays, t->ImageWidth, t->ImageLength, (gray) maxval );
    }
/*
** Output a strip in PGM format
*/
PgmDoStrip(buffer,count,grays,row,cols,bps,maxval)
char *buffer;
int count;
gray **grays;
int maxval;
int row, cols, bps;
    {
    register int i, col;

    col = 0;
    while ( count-- )
	{
	for ( i = 7 ; i >= 0; i -= bps )
	    {
	    grays[row][col++] = maxval - ( ( *buffer >> i ) & maxval );
	    if ( col >= cols )
		{
		col = 0;
		row++;
		break;
		}
	    }
	buffer++;
	}
    }

int bigendian;

/*
** Read and decode a Tiff header.  Not all TIFF capabilities are supported.
*/
int
readTiff( T, H, fd )
struct Tiffstruct *T;
struct TiffHeader *H;
FILE *fd;
    {
    struct IDF idf;
    struct IDF_Entry  *ptr;
    LONG TempRational[2];
    int i;

#ifdef SYSV
    memset((char *)T,0,sizeof(struct Tiffstruct));
#else /*SYSV*/
    bzero((char *)T,sizeof(struct Tiffstruct));
#endif /*SYSV*/

    if (fread(H->ByteOrder,sizeof(H->ByteOrder),1,fd) != 1)
	pm_perror("reading header byte order");
    if ( H->ByteOrder[0] == 0x4d && H->ByteOrder[1] == 0x4d )
	bigendian = 1;
    else if ( H->ByteOrder[0] == 0x49 && H->ByteOrder[1] == 0x49 )
	bigendian = 0;
    else
	pm_error(
	    "unknown byte order: 0x%02x%02x", H->ByteOrder[0], H->ByteOrder[1],
	    0,0,0 );

    {
    if (readshort(fd,&H->Version) == -1)
	pm_perror("reading header version");
    if (readlong(fd,&H->IdfOffset) == -1)
	pm_perror("reading header ifd offset");
    }

    fseek(fd,(long)H->IdfOffset,0);

    if (readshort(fd,&idf.NumEntries) == -1)
	pm_perror("reading idf count ");

    if ((idf.idfptr = (struct IDF_Entry *) calloc(idf.NumEntries,sizeof(struct IDF_Entry))) == NULL)
	pm_error( "out of memory", 0,0,0,0,0 );

    for( i = 0; i < idf.NumEntries; ++i )
	{
	if (readshort(fd,&idf.idfptr[i].Tag) == -1)
	    pm_perror("reading idf entries");
	if (readshort(fd,&idf.idfptr[i].Type) == -1)
	    pm_perror("reading idf entries");
	if (readlong(fd,&idf.idfptr[i].Length) == -1)
	    pm_perror("reading idf entries");
	if (readlong(fd,&idf.idfptr[i].ValueOffset) == -1)
	    pm_perror("reading idf entries");
	}

    ptr = idf.idfptr;

    while (idf.NumEntries--)
	{
	switch (ptr->Tag)
	    {
	    case  0x00FF:
	    T->SubFileType = (ptr->ValueOffset >> 16);
	    break;

	    case  0x0100:
	    T->ImageWidth = (ptr->ValueOffset >> 16);
	    break;

	    case  0x0101:
	    T->ImageLength = (ptr->ValueOffset >> 16);
	    break;

	    case  0x0102:
	    T->BitsPerSample = (ptr->ValueOffset >> 16);
	    break;

	    case  0x0103:
	    T->Compression = (ptr->ValueOffset >> 16);
	    break;

	    case  0x0106:
	    T->PhotoInterp = (ptr->ValueOffset >> 16);
	    break;

	    case  0x0107:
	    T->Threshold = (ptr->ValueOffset >> 16);
	    break;

	    case  0x0108:
	    T->CellWidth = (ptr->ValueOffset >> 16);
	    break;

	    case  0x0109:
	    T->CellLength = (ptr->ValueOffset >> 16);
	    break;

	    case  0x010A:
	    T->FillOrder = (ptr->ValueOffset >> 16);
	    break;

	    case  0x010D:
	    fprintf(stderr,"Unsupported feature\n");
	    /*
	    fseek(fd,(long)ptr->ValueOffset,0);
	    */
	    break;

	    case  0x010E:
	    fprintf(stderr,"Unsupported feature\n");
	    /*
	    fseek(fd,(long)ptr->ValueOffset,0);
	    */
	    break;

	    case  0x010F:
	    /*
	    fseek(fd,(long)ptr->ValueOffset,0);
	    */
	    fprintf(stderr,"Make: %s\n", T->Make);
	    break;

	    case  0x0110:
	    fprintf(stderr,"Unsupported feature\n");
	    /*
	    fseek(fd,(long)ptr->ValueOffset,0);
	    */
	    break;

	    case  0x0111:
	    T->StOffsetCnt = ptr->Length;
	    if ((T->StripOffset = (LONG *) calloc(T->StOffsetCnt, sizeof (LONG)))== NULL)
		pm_error( "out of memory", 0,0,0,0,0 );
	    if(T->StOffsetCnt == 1)
		*(T->StripOffset) = ptr->ValueOffset;
	    else
		{
		fseek(fd, (long) ptr->ValueOffset, 0);
		for( i = 0; i < T->StOffsetCnt; ++i)
		    if(readlong(fd,T->StripOffset+i) == -1)
			pm_error( "reading strip offsets", 0,0,0,0,0 );
		}
	    break;

	    case  0x0112:
	    T->Orientation= (ptr->ValueOffset >> 16);
	    break;

	    case  0x0115:
	    T->SamplesPixel= (ptr->ValueOffset >> 16);
	    break;

	    case  0x0116:
	    T->RowsStrip= (ptr->ValueOffset);
	    break;

	    case  0x0117:
	    T->StripByteCnt = (ptr->Length);
	    if ((T->SBytesCntOffset = (LONG *) calloc(T->StripByteCnt, sizeof (LONG)))== NULL)
		pm_error( "out of memory", 0,0,0,0,0 );
	    if(T->StripByteCnt == 1)
		*(T->SBytesCntOffset) = ptr->ValueOffset;
	    else
		{
		fseek(fd,(long)ptr->ValueOffset,0);
		for( i = 0; i < T->StripByteCnt; ++i)
		    if(readlong(fd,T->SBytesCntOffset+i) == -1)
			pm_error( "reading strip byte count offsets", 0,0,0,0,0 );
		}
	    break;

	    case  0x0118:
	    T->MinSampleValue= (ptr->ValueOffset >> 16);
	    break;

	    case  0x0119:
	    T->MaxSampleValue= (ptr->ValueOffset >> 16);
	    break;

	    case  0x011A:
	    fseek(fd,(long)ptr->ValueOffset,0);
	    if(readlong(fd,&TempRational[0]) == -1)
		pm_error( "reading X resolution", 0,0,0,0,0 );
	    if(readlong(fd,&TempRational[1]) == -1)
		pm_error( "reading X resolution", 0,0,0,0,0 );
	    T->Xres = TempRational[0]/TempRational[1];
	    break;

	    case  0x011B:
	    fseek(fd,(long)ptr->ValueOffset,0);
	    if(readlong(fd,&TempRational[0]) == -1)
		pm_error( "reading Y resolution", 0,0,0,0,0 );
	    if(readlong(fd,&TempRational[1]) == -1)
		pm_error( "reading Y resolution", 0,0,0,0,0 );
	    T->Yres = TempRational[0]/TempRational[1];
	    break;

	    case  0x011C:
	    T->PlanarConfig= (ptr->ValueOffset >> 16);
	    break;

	    case  0x011D:
	    fprintf(stderr,"Unsupported feature: PageName\n");
	    /* T->PageName */
	    break;

	    case  0x011E:
	    fprintf(stderr,"Unsupported feature: Xpos\n");
	    /* T->XPos */
	    break;

	    case  0x011F:
	    fprintf(stderr,"Unsupported feature: Ypos\n");
	    /* T->YPos */
	    break;

	    case  0x0120:
	    fprintf(stderr,"Unsupported feature: FreeOffsets\n");
	    /* T->FreeOffsets */
	    break;

	    case  0x0121:
	    fprintf(stderr,"Unsupported feature: FreeByteCount \n");
	    /* T->FreeByteCount */
	    break;

	    case  0x0122:
	    fprintf(stderr,"Unsupported feature: GrayResUnit\n");
	    /* T->GrayResUnit */
	    break;

	    case  0x0123:
	    fprintf(stderr,"Unsupported feature: GrayResCurve\n");
	    /* T->GrayResCurve */
	    break;

	    case  0x0124:
	    fprintf(stderr,"Unsupported feature: Group3Option\n");
	    /* T->Group3Option */
	    break;

	    case  0x0125:
	    fprintf(stderr,"Unsupported feature: Group4Option\n");
	    /* T->Group4Option */
	    break;

	    case  0x0128:
	    fprintf(stderr,"Unsupported feature: ResolutionUnit\n");
	    /* T->ResolutionUnit */
	    break;

	    case  0x0129:
	    fprintf(stderr,"Unsupported feature: PageNumber\n");
	    /* T->PageNumber */
	    break;

	    case  0x012C:
	    fprintf(stderr,"Unsupported feature: ColorResUnit\n");
	    /* T->ColorResUnit */
	    break;

	    case  0x012D:
	    fprintf(stderr,"Unsupported feature: ColorResCurv\n");
	    /* T->ColorResCurv */
	    break;

	    default:
	    fprintf(stderr,"Unsupported feature: Unknown Tag\n");
	    fprintf(stderr,"Default\n");
	    break;
	    }
	ptr++;
	}

    return 1;
    }

/*
** Dump header information
*/
dumpHeader( T, H )
struct Tiffstruct *T;
struct TiffHeader *H;
    {
    int i;
    LONG *offptr;

    fprintf( stderr, "Version:        %d\n",H->Version );
    fprintf( stderr, "ByteOrder:      %c%c\n", H->ByteOrder[0], H->ByteOrder[1] );
    fprintf( stderr, "Subfile Type:   %d\n", T->SubFileType );
    fprintf( stderr, "ImageWidth:     %d\n", T->ImageWidth );
    fprintf( stderr, "ImageLength:    %d\n", T->ImageLength );
    fprintf( stderr, "BitsPerSample:  %d\n", T->BitsPerSample );
    fprintf( stderr, "Compression:    %d\n", T->Compression );
    fprintf( stderr, "PhotoInterp:    %d\n", T->PhotoInterp );
    fprintf( stderr, "Threshold:      %d\n", T->Threshold );
    fprintf( stderr, "CellWidth:      %d\n", T->CellWidth );
    fprintf( stderr, "CellLength:     %d\n", T->CellLength );
    fprintf( stderr, "FillOrder:      %d\n", T->FillOrder );
    fprintf( stderr, "DocName:        %s\n", T->DocName );
    fprintf( stderr, "ImageDescript:  %s\n", T->ImageDescript );
    fprintf( stderr, "Model:          %s\n", T->Model );
    fprintf( stderr, "StripOffsetCnt: %d\n", T->StOffsetCnt );
    offptr = T->StripOffset;
    for( i=0; i < T->StOffsetCnt; i++ )
	{
	fprintf( stderr, "Strip [%02d] starts at: %d \n", i,*offptr );
	offptr++;
	}
    fprintf( stderr, "Orientation:    %d\n", T->Orientation );
    fprintf( stderr, "SamplesPixel:   %d\n", T->SamplesPixel );
    fprintf( stderr, "RowsStrip:      %d\n", T->RowsStrip );
    fprintf( stderr, "StripByteCnt:   %d\n", T->StripByteCnt );
    offptr = T->SBytesCntOffset;
    for( i=0; i < T->StripByteCnt; i++ )
	{
	fprintf( stderr, "Strip [%02d]: %d bytes\n", i,*offptr );
	offptr++;
	}
    fprintf( stderr, "MinSampleValue: %d\n", T->MinSampleValue );
    fprintf( stderr, "MaxSampleValue: %d\n", T->MaxSampleValue );
    fprintf( stderr, "Xres:           %d\n", T->Xres );
    fprintf( stderr, "Yres:           %d\n", T->Yres );
    fprintf( stderr, "PlanarConfig:   %d\n", T->PlanarConfig );
    }

readshort( in, sP )
FILE *in;
short *sP;
    {
    if ( bigendian )
	return pm_readbigshort( in, sP );
    else
	return pm_readlittleshort( in, sP );
    }

readlong( in, lP )
FILE *in;
long *lP;
    {
    if ( bigendian )
	return pm_readbiglong( in, lP );
    else
	return pm_readlittlelong( in, lP );
    }
