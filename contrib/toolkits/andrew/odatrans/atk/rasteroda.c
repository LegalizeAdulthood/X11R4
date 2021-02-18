/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/rasteroda.c,v 1.2 89/09/08 17:31:07 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/rasteroda.c,v $ */

#ifndef lint
static char *rcsidrasterodac = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/rasteroda.c,v 1.2 89/09/08 17:31:07 mss Exp $";
#endif

/* $Log:	rasteroda.c,v $
 * Revision 1.2  89/09/08  17:31:07  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:18:44  mss
 * Initial revision
 * 
 * Revision 1.20  89/08/25  11:41:29  mss
 * Changed calls from attribute support to tool kit
 * 
 * Revision 1.19  89/05/20  20:39:01  mss
 * Changes how pres/lay attributes set
 * 
 * Revision 1.18  89/04/30  12:11:42  mss
 * Changed names to reflect new atk naming scheme
 * 
 * Revision 1.17  89/03/23  07:08:38  mss
 * Uses new component specifications
 * 
 * Revision 1.16  88/11/17  11:44:44  mss
 * Changed default setting of raster scaling switch to be
 * "no scaling" in both directions; also changed scaling
 * suppression messages to appear only when debugging
 * is turned on.
 *  
*
*/

#include <class.h>

#include <rect.h>
#include <pixelimg.ih>
#include <rastimg.ih>
#include <raster.ih>
#include <fontdesc.ih>
#include <environ.ih>
#include <dict.ih>
#include <viewref.ih>
#include <view.ih>
#include <rasterv.ih>
#include <envrment.ih>
#include <style.ih>

#include <odatk.h>
#include <odadef.h>
#include <attsup.h>
#include <atkoda.ih>
#include <odalib.ih>

#include <rasteroda.eh>

#define tracing 0

INT_type FillInAttributes(gD, RasterBLO, RasterCP, theView, theDataObject)
struct atkoda		*gD;		/* Document/translation global state */
CONSTITUENT		RasterBLO;	/* Basic logical object for raster */
CONSTITUENT		RasterCP;	/* Content Portion for raster */
struct rasterview	*theView;	/* View on the raster object */
struct raster		*theDataObject;	/* Raster data object */
{
    struct rasterimage *theRealDataObject;  /* pointer to the original */
    struct rasterimage *rasterToUse;	    /* scaled version for ODA */

    struct rectangle subRectangle;	    /* area of raster to use */

    unsigned char *rowBuffer;		    /* used to hold data for move to CP */
    int	bytesPerRow;			    /* width of a row in bytes */
    int	i, j;				    /* general loop variables */

    INT_type		rval;

    rg_CLIPPING_type			Clipping;
    rg_LINE_PROGRESSION_type		LineProgression;
    rg_PEL_PATH_type			PelPath;
    rg_IMAGE_DIMENSIONS_type		ImageDimensions;
    rg_PEL_SPACING_type			PelSpacing;
    rg_SPACING_RATIO_type		SpacingRatio;

    rg_COMPRESSION_type			Compression;
    rg_NUMBER_OF_LINES_type		NumberOfLines;
    rg_NUMBER_OF_PELS_PER_LINE_type	NumberOfPelsPerLine;

    SEQUENCE_BYTE_type RasterContents;


/*
 *  This is where the real work is done.  First, the ATK raster is scaled from 72 dots/inch
 *  to the 200 dots/inch we are using for the first few exchanges.  Next, the attributes are
 *  set.  Finally, the data is converted and attached to the contents portion.
 *
 *  Note that the contents must not contain any padding bytes.  Also notice how the 
 *  sequence of BYTE that holds the contents can be deleted after the information is
 *  on the Content Portion.
 *  
 *  Only a subset of the attributes are used for the formatted-processable form of rasters.
 *  These attributes include:
 *    Presentation Attributes:
 *      CLIPPING
 *      LINE PROGRESSION
 *      PEL PATH
 *      IMAGE DIMENSIONS
 *      PEL SPACING
 *      SPACING RATIO
 *
 *    Content Portion Attributes:
 *      NUMBER OF LINES
 *      NUMBER OF PELS PER LINE
 *      COMPRESSION
 *      content information
 *
 *  Each of these is used as follows:
 *
 *      CLIPPING
 *        The clipping is used to tell what sub area of the raster to use.  This will normally
 *        be set to (0,0) (NUMBER OF PELS PER LINE -1, NUMBER OF LINES -1).
 *      LINE PROGRESSION
 *        For now this will be set to 0.  (the default)
 *      PEL PATH
 *        For now this will be set to 270.  (the default)
 *      IMAGE DIMENSIONS
 *        For now set this to automatic and use PEL SPACING and SPACING RATIO
 *      PEL SPACING
 *        For now set this to 6, 1 (200 dots/inch)
 *      SPACING RATIO
 *        For now set this to 1, 1.
 *
 *    Content Portion Attributes:
 *      NUMBER OF LINES
 *        Number of lines in full, uncropped image.
 *      NUMBER OF PELS PER LINE
 *        Number of rows in full, uncropped image.
 *      COMPRESSION
 *        For now set to compressed.
 *      content information
 *        For now we only exchange BITMAP types of contents, no T4 or T6.
 */


    if (gD->doDebugging) {
	printf("Entering FillInAttributes\n");
    }

    rval = SUCCESS_INT;


    /* Convert ATK raster bits into appropriate ODA raster attributes */

    theRealDataObject = raster_GetPix(theDataObject);

    /* select entire rectangle */
    subRectangle.top = 0;
    subRectangle.left = 0;
    subRectangle.width = rasterimage_GetWidth(theRealDataObject);
    subRectangle.height = rasterimage_GetHeight(theRealDataObject);

    if (gD->doDebugging) {
	printf("passed raster is %d wide and %d high\n", rasterimage_GetWidth(theRealDataObject), rasterimage_GetHeight(theRealDataObject));
    }

    if (gD->ScaleRaster) {
	/* scale to 200 dots per inch */
	rasterToUse = rasterimage_Create(((((long) rasterimage_GetWidth(theRealDataObject) * 200) + 36) / 72), ((((long) rasterimage_GetHeight(theRealDataObject) * 200) + 36) / 72));
    } else {
	/* leave it at 72 dots per inch */
	if (gD->doDebugging) {
	    printf("ProcessRasterCP:  scaling of raster has been disabled.\n");
	}
	rasterToUse = rasterimage_Create((long) rasterimage_GetWidth(theRealDataObject), (long) rasterimage_GetHeight(theRealDataObject));
    }

    if (gD->doDebugging) {
	printf("new raster is %d wide and %d high\n", rasterimage_GetWidth(rasterToUse), rasterimage_GetHeight(rasterToUse));
    }

    rasterimage_GetScaledSubraster(theRealDataObject, &subRectangle,
			 (long) rasterimage_GetWidth(rasterToUse), (long) rasterimage_GetHeight(rasterToUse), rasterToUse);
    bytesPerRow	= (rasterimage_GetWidth(rasterToUse) + 7) / 8;	/* for use below */


    if (gD->doDebugging) {
	printf("final raster is %d wide and %d high\n", rasterimage_GetWidth(rasterToUse),rasterimage_GetHeight(rasterToUse));
    }


    /* for now we can only write bitmaps */

    odalib_SetAttr(RasterCP, at_TYPE_OF_CODING, (POINTER_type) odalib_at_TYPE_OF_CODING_bitmap(), Ignore_Parms);

    /*
    * attributes for presentation portion
    */
    /* rg_CLIPPING_type */
    Clipping.coord1.first = 0;
    Clipping.coord1.second = 0;
    Clipping.coord2.first = rasterimage_GetWidth(rasterToUse) - 1;
    Clipping.coord2.second = rasterimage_GetHeight(rasterToUse) - 1;

    /* rg_LINE_PROGRESSION_type */
    LineProgression = rg_LINE_PROGRESSION_270;	    /* for all ATK */

    /* rg_PEL_PATH_type */
    PelPath = rg_PEL_PATH_0;			    /* for all ATK */

    /* rg_IMAGE_DIMENSIONS_type */
    ImageDimensions.image_dimensions_tag = AUTOMATIC_tag;

    /* rg_PEL_SPACING_type */
    PelSpacing.null = BOOL_false;
    PelSpacing.length = atkoda_CVDotsPerInchToSMU(gD,/* density */ 200, /* num of dots */1);
    PelSpacing.pel_spaces = 1;

    /* rg_SPACING_RATIO_type */
    SpacingRatio.first = 1;				    /* always use 1 for ATK */
    SpacingRatio.second = 1;				    /* always use 1 for ATK */

    /*
    * attributes for content portion
    */
    /* rg_COMPRESSION_type */
    Compression	= rg_COMPRESSION_compressed;	    /* safe for now */

    /* rg_NUMBER_OF_LINES_type */
    NumberOfLines = rasterimage_GetHeight(rasterToUse);

    /* rg_NUMBER_OF_PELS_PER_LINE_type */
    NumberOfPelsPerLine = rasterimage_GetWidth(rasterToUse);



    /* apply all the attributes */

    odalib_SetPresAttr(RasterBLO, rg_CLIPPING, (POINTER_type) &Clipping, ALL_PARMS);
    odalib_SetPresAttr(RasterBLO, rg_LINE_PROGRESSION, (POINTER_type) &LineProgression, ALL_PARMS);
    odalib_SetPresAttr(RasterBLO, rg_PEL_PATH, (POINTER_type) &PelPath, ALL_PARMS);
    odalib_SetPresAttr(RasterBLO, rg_IMAGE_DIMENSIONS, (POINTER_type) &ImageDimensions, ALL_PARMS);
    odalib_SetPresAttr(RasterBLO, rg_PEL_SPACING, (POINTER_type) &PelSpacing, ALL_PARMS);
    odalib_SetPresAttr(RasterBLO, rg_SPACING_RATIO, (POINTER_type) &SpacingRatio, ALL_PARMS);

    odalib_SetAttr(RasterCP, rg_COMPRESSION, (POINTER_type) &Compression, ALL_PARMS); 
    odalib_SetAttr(RasterCP, rg_NUMBER_OF_LINES, (POINTER_type) &NumberOfLines, ALL_PARMS);
    odalib_SetAttr(RasterCP, rg_NUMBER_OF_PELS_PER_LINE, (POINTER_type) &NumberOfPelsPerLine, ALL_PARMS);


    rowBuffer =	(unsigned char *) malloc(bytesPerRow + 1);  /* to make sure it is big enough for words */


    if (gD->doDebugging) {
	printf("in FillInAttributes for Raster Pel/Line=%d, Lines=%d, Bytes/Row=%d\n", NumberOfPelsPerLine, NumberOfLines, bytesPerRow);
    }


    /* Attach data to CP */
    if (odalib_ERROR_SEQUENCE() ==
	 (RasterContents = odalib_MakeSequence(SEQUENCE_BYTE_tag, (bytesPerRow * NumberOfLines) ) )) {
	printf("Could not create byte sequence to hold content portion (%d)\n", odalib_TKError());
	return ERROR_INT;
    }

    for (j = 0; j < rasterimage_GetHeight(rasterToUse); j++) {
	(void) rasterimage_GetRow(rasterToUse, (long) 0, (long) j, rasterimage_GetWidth(rasterToUse), (short *) &rowBuffer[0]);
	bcopy(&rowBuffer[0], &RasterContents->sequence_value.bytes[j * bytesPerRow], bytesPerRow);
    }

    if (ERROR_INT == odalib_SetAttr(RasterCP, at_CONTENT_INFORMATION, (POINTER_type) RasterContents, ALL_PARMS)) {
	printf("Could not set attribute for content information on CP (%d)\n", odalib_TKError());
	/* return ERROR_INT; */
    }
    /* And get rid of copy of sequence */
    if (ERROR_INT == odalib_DeleteSequence(RasterContents)) {
	printf("Could not deallocate content information sequence (%d)\n", odalib_TKError());
	/* return ERROR_INT; */
    }

    free(rowBuffer);	/* give back the space */


LEAVE:

    if (tracing) {
	printf("Leaving FillInAttributes with return value %d\n", rval);
    }

    return rval;

}




static CONSTITUENT MakeRaster(gD)
struct atkoda * gD;{
    CONSTITUENT rval;

    if (tracing) {
        printf("Entering MakeRaster\n");
    }

    rval = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_bas_logical_obj, SPECIFIC_COMPONENT);

    (void) odalib_SetStringAttr(rval, at_APPLICATION_COMMENTS, "Raster", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(rval, at_USER_READABLE_COMMENTS, "Basic ATK Raster Object", DATA_CHAR_TYPE);

/* Done by reference to generic */
#ifdef RedundantArchClass
    odalib_SetAttr(rval, at_CONTENT_ARCH_CLASS, odalib_at_CONTENT_ARCH_CLASS_fp_rg(),ALL_PARMS);
#endif

    /* Connect to generic */
    odalib_SetAttr(rval, at_OBJECT_CLASS, (POINTER_type) gD->genericRaster, ALL_PARMS);

    if (tracing) {
        printf("Leaving MakeRaster\n");
    }

    return rval;

}


static CONSTITUENT MakeRasterCP(gD)
struct atkoda * gD;{
    CONSTITUENT rval;

    if (tracing) {
        printf("Entering MakeRasterCP\n");
    }

    rval = odalib_MakeContent(gD->outputDocument, RASTER_GRAPHICS_CONTENT);

    if (tracing) {
        printf("Leaving MakeRasterCP\n");
    }

    return rval;

}


enum view_DescriberErrs rasteroda__Describe(descObject,viewToDescribe,format,file,gD)
struct rasteroda * descObject;
struct rasterview * viewToDescribe;
char * format;
FILE * file;
struct atkoda * gD; {

    enum view_DescriberErrs EmbeddedDescriptionStatus;
    register struct raster *d;
    CONSTITUENT RasterBLO, RasterCP;
    SEQUENCE_CONSTITUENT_type tempSeq;

    /* Raster processing is very simple:
              1. make a raster (basic logical object)
              2. attach it to the figure
              3. make a content portion
              4. attach it to the raster
              5. Fill in the raster attributes as appropriate */

    if(gD->doDebugging) {
	printf("Entering rasterodaDescribe\n");
    }
 
    /* Make the pieces */
    RasterBLO = MakeRaster(gD);
    RasterCP = MakeRasterCP(gD);

    /* Connect them together */
    atkoda_AppendBLOToPhrase(gD, RasterBLO, RasterCP, NULL);
    tempSeq = odalib_MakeSequence(SEQUENCE_CONSTITUENT_tag, 1);
    tempSeq->sequence_value.constituents[0] = RasterCP;
    odalib_SetAttr(RasterBLO, at_CONTENT_PORTIONS, (POINTER_type) tempSeq, ALL_PARMS);
    odalib_DeleteSequence(tempSeq);

    /* Connect to appropriate generics */

    /* And set the attributes */

    d = (struct raster *) viewToDescribe->header.view.dataobject;

    FillInAttributes(gD, RasterBLO, RasterCP, viewToDescribe, d);

    if (gD->doDebugging) {
	printf("About to retrun from raster oda desciber\n");
    }


    return view_NoDescribeError;

}


