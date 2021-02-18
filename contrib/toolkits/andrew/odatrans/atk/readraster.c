/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readraster.c,v 1.2 89/09/08 17:31:53 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readraster.c,v $ */

#ifndef lint 
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readraster.c,v 1.2 89/09/08 17:31:53 mss Exp $";
#endif

/*
  $Log:	readraster.c,v $
 * Revision 1.2  89/09/08  17:31:53  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:14:46  mss
 * Initial revision
 * 
 * Revision 1.10  89/04/30  12:11:56  mss
 * Changed names to reflect new atk naming scheme
 * 
 * Revision 1.9  89/03/23  07:09:13  mss
 * Hacks added to deal with problems of getting some raster
 * attributes
 * 
 * Revision 1.8  88/11/17  11:45:53  mss
 * Changed default setting of raster scaling switch to be
 * "no scaling" in both directions; also changed scaling
 * suppression messages to appear only when debugging
 * is turned on.
 * 
 * Revision 1.7  88/11/09  23:49:22  pgc
 * fixed up some rounding errors in the scaling of rasters
 * 
 * Revision 1.6  88/11/04  17:06:45  mss
 * 
 * 
 * Revision 1.5  88/10/24  00:10:44  pgc
 * everything but at_TYPE_OF_CODING_bitmap now seems to work.
 * well...rotating is also not yet done and T.4 and T.6 don't
 * work either but....
 * 
 * Revision 1.4  88/10/21  02:44:21  pgc
 * starting to work but has some things commented out because of toolkit problems.
 * 
 * Revision 1.3  88/10/01  17:11:21  mss
 * Updated to new atk
 * 
 * Revision 1.2  88/09/22  11:59:55  mss
 * Fixed bogus use of identifier
 * 
 * Revision 1.1  88/09/22  11:15:13  mss
 * Initial revision
 * 

*/

#include <class.h>

#include <view.ih>
#include <dataobj.ih>
#include <rect.h>
#include <pixelimg.ih>
#include <rastimg.ih>
#include <raster.ih>
#include <txtstvec.h>
#include <text.ih>
#include <viewref.ih>

#include <odatk.h>
#include <odalib.ih>
#include <odaatk.h>
#include <odadef.h>
#include <readraster.h>
#include <errormsg.h>


/*
 * this is the current state of implementation.  To add new capabilities start by looking for comments with "%%%" in them.
 *
 *
 * rg_CLIPPING_type
 *
 *  clipping is fully implemented in this routine.
 * 
 * 
 * rg_LINE_PROGRESSION_type
 *
 *  line progression is fully implemented in these routine.
 *
 *
 * rg_PEL_PATH_type
 *
 *  at this time pel path must be 0.  %%%
 *
 *
 * rg_IMAGE_DIMENSIONS_type
 *
 *  image dimensions is fully implemented in this routine.
 * 
 * 
 * rg_PEL_SPACING_type
 *
 *  pel spacing is fully implemented in this routine.
 *
 *
 * rg_SPACING_RATIO_type
 *
 *  spacing ratio is fully implemented in this routine.
 *
 *
 * rg_INITIAL_OFFSET_type
 *
 *  initial offset is ignored as these must be formatted-processable rasters.
 *
 *
 * rg_PEL_TRANS_DENSITY_type
 *
 *  pel transmition density is ignored as these must be formatted-processable rasters.
 *
 *
 *
 * rg_COMPRESSION_type
 * 
 *  compression is not implemented at this time.  %%%
 * 
 *
 * rg_NUMBER_OF_LINES_type
 * 
 *  number of lines is fully implemented in this routine.
 *
 *
 * rg_NUMBER_OF_PELS_PER_LINE_type
 *
 *  number of pels per line is fully implemented in this routine.
 *
 *
 * rg_NUMBER_DISCARDED_PELS_type
 *
 *  number discarded pels is ignored as these must be formatted-processable rasters.
 *
 *
 * at_TYPE_OF_CODING_type
 * 
 *  only bitmaps are supported at this time.  %%%
 */





void ProcessRasterCP(gD, curBasic, contents, contentType, contentMode, startPosition, curSV)
p_odainfo_type gD;
CONSTITUENT curBasic;
SEQUENCE_CONSTITUENT_type	contents;	/* content portions of basic objects */
INT_type contentType, contentMode;
long * startPosition;
struct text_statevector * curSV;
{
    CONSTITUENT	thisCP;			    /* this raster's content portion */

    struct raster * newRasterObj;
    struct viewref * dummyVR;


    struct rasterimage *theRealDataObject;  /* pointer to the original */
    struct rasterimage *rasterToUse;	    /* scaled version for ODA */
    struct rectangle subrectangle;	    /* for scaling bits */
    long int realWidth;			    /* the real width of the final ATK raster */
    long int realHeight;		    /* the real height of the final ATK raster */

    unsigned char *rowBuffer;		    /* used to hold data for move to CP */
    int	bytesPerRow;			    /* width of a row in bytes */
    int	i;				    /* general loop variable */


    rg_CLIPPING_type			*Clipping;
    rg_LINE_PROGRESSION_type		LineProgression;
    rg_PEL_PATH_type			PelPath;
    rg_IMAGE_DIMENSIONS_type		*ImageDimensions;
    rg_PEL_SPACING_type			*PelSpacing;
    rg_SPACING_RATIO_type		*SpacingRatio;

    rg_COMPRESSION_type			Compression;
    rg_NUMBER_OF_LINES_type		NumberOfLines;
    rg_NUMBER_OF_PELS_PER_LINE_type	NumberOfPelsPerLine;
    at_TYPE_OF_CODING_type		TypeOfCoding;

    SEQUENCE_BYTE_type			RasterContents;


/*
 * top of procedure
 */

    if (gD->doDebugging) {
        printf("ProcessRasterCP:  Entering\n");
    }


/* %%%
 * change this to be a for loop that loops through the contents creating a new ATK raster for each.
 */
    if (contents->length != 1) {
	printf("ProcessRasterCP:  don't know how to deal with %d CPs\n", contents->length);
    }
    thisCP = contents->sequence_value.constituents[0];	/* get the first one */


/*
 * get all the attributes from the raster portion of the ODA document 
 */

    if (odalib_GetAttr(curBasic, rg_CLIPPING, (POINTER_type) &Clipping, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting rg_CLIPPING attribute (%d)\n", odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }
    if (odalib_GetAttr(curBasic, rg_LINE_PROGRESSION, (POINTER_type) &LineProgression, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting rg_LINE_PROGRESSION attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }
#if 0
    if (odalib_GetAttr(curBasic, rg_PEL_PATH, (POINTER_type) &PelPath, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting rg_PEL_PATH attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }
#else
    PelPath = rg_PEL_PATH_0;
#endif
    if (odalib_GetAttr(curBasic, rg_IMAGE_DIMENSIONS, (POINTER_type) &ImageDimensions, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting rg_IMAGE_DIMENSIONS attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }
    if (odalib_GetAttr(curBasic, rg_PEL_SPACING, (POINTER_type) &PelSpacing, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting rg_PEL_SPACING attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }
    if (odalib_GetAttr(curBasic, rg_SPACING_RATIO, (POINTER_type) &SpacingRatio, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting rg_SPACING_RATIO attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }




#if 0
    if (odalib_GetAttr(thisCP, rg_COMPRESSION, (POINTER_type) &Compression, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting rg_COMPRESSION attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }
#else
    Compression = rg_COMPRESSION_uncompressed;
#endif
    if (odalib_GetAttr(thisCP, rg_NUMBER_OF_LINES, (POINTER_type) &NumberOfLines, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting rg_NUMBER_OF_LINES attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }
    if (odalib_GetAttr(thisCP, rg_NUMBER_OF_PELS_PER_LINE, (POINTER_type) &NumberOfPelsPerLine, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting rg_NUMBER_OF_PELS_PER_LINE attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }
    if (odalib_GetAttr(thisCP, at_CONTENT_INFORMATION, (POINTER_type) &RasterContents, BOOL_false, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting at_CONTENT_INFORMATION attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }
    if (odalib_GetAttr(thisCP, at_TYPE_OF_CODING, (POINTER_type) &TypeOfCoding, BOOL_true, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessRasterCP:  error getting at_TYPE_OF_CODING attribute (%d)\n",odalib_TKError());
	printf("ProcessRasterCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }



    if (gD->doDebugging) {
	printf("ProcessRasterCP:  the following values were obtained for the raster:\n");
	printf("ProcessRasterCP:  \n");
	printf("ProcessRasterCP:  Clipping = ((%d, %d), (%d, %d))\n", Clipping->coord1.first, Clipping->coord1.second, Clipping->coord2.first, Clipping->coord2.second);
	printf("ProcessRasterCP:  LineProgression = %s\n", odalib_GetValueName(LineProgression));
	printf("ProcessRasterCP:  PelPath = %s\n", odalib_GetValueName(PelPath));
	printf("ProcessRasterCP:  ImageDimensions = %s\n", odalib_GetValueName(ImageDimensions->image_dimensions_tag));
 	printf("ProcessRasterCP:  PelSpacing = %s (%d, %d)\n", odalib_GetValueName(PelSpacing->null), PelSpacing->length, PelSpacing->pel_spaces);
 	printf("ProcessRasterCP:  SpacingRatio = (%d, %d)\n", SpacingRatio->first, SpacingRatio->second);
	printf("ProcessRasterCP:  \n");
	printf("ProcessRasterCP:  Compression = %s\n", odalib_GetValueName(Compression));
	printf("ProcessRasterCP:  NumberOfLines = %d\n", NumberOfLines);
	printf("ProcessRasterCP:  NumberOfPelsPerLine = %d\n", NumberOfPelsPerLine);
	printf("ProcessRasterCP:  TypeOfCoding = %s\n", odalib_GetValueName(TypeOfCoding));
	printf("ProcessRasterCP:  \n");
	printf("ProcessRasterCP:  Sequence Length = %d\n", RasterContents->length);
	printf("ProcessRasterCP:  Sequence Contents = ");
	for (i = 0; i < RasterContents->length; i++) {
	    printf(" 0x%2x", RasterContents->sequence_value.bytes[i]);
	}
	printf("\n");
    }


/*
 * compute some needed values
 */

    bytesPerRow = (NumberOfPelsPerLine + 7) / 8;


/* 
 * if not of type bitmap decompress the image
 */

/* no op for now %%% */


/*
 * check the results of getting the raster's attributes
 */

/*     if (TypeOfCoding != at_TYPE_OF_CODING_bitmap) { */
    if (0) {
	printf("ProcessRasterCP:  this routine only supports bitmaps, not %s\n", odalib_GetValueName(TypeOfCoding));
    } else {	    /* this is all to check for bitmaps */
	if ((NumberOfLines * bytesPerRow) != RasterContents->length) {
	    printf("ProcessRasterCP:  expecting %d bytes of Content Information and got %d bytes\n", (NumberOfLines * bytesPerRow), RasterContents->length);
	}
    }

    if ((Clipping->coord1.first < 0) || (Clipping->coord1.first > NumberOfPelsPerLine) ||
	 (Clipping->coord2.first < 0) || (Clipping->coord2.first > NumberOfPelsPerLine) ||
	 (Clipping->coord1.second < 0) || (Clipping->coord1.second > NumberOfLines) ||
	 (Clipping->coord2.second < 0) || (Clipping->coord2.second > NumberOfLines)) {
	printf("ProcessRasterCP:  problem with clipping attribute, setting to default value\n");
	printf("ProcessRasterCP:  bad values are ((%d, %d), (%d, %d)) Lines=%d, Rows=%d\n", Clipping->coord1.first, Clipping->coord1.second, Clipping->coord2.first, Clipping->coord2.second, NumberOfLines, NumberOfPelsPerLine);
	Clipping->coord1.first = 0;
	Clipping->coord1.second = 0;
	Clipping->coord2.first = NumberOfPelsPerLine - 1;
	Clipping->coord2.second = NumberOfLines - 1;
	printf("ProcessRasterCP:  values are now ((%d, %d), (%d, %d))\n", Clipping->coord1.first, Clipping->coord1.second, Clipping->coord2.first, Clipping->coord2.second);
    }


/*  
 * make a rasterimage to hold the bits and scale it to ATK resolution 
 */

    if (gD->doDebugging) {
	printf("ProcessRasterCP:  about to create raster image to hold raw bits\n");
    }

    rasterToUse = rasterimage_Create((long) NumberOfPelsPerLine, (long) NumberOfLines);

    if (gD->doDebugging) {
	printf("ProcessRasterCP:  copying data...\n");
    }

    rowBuffer =	(unsigned char *) malloc(bytesPerRow + 1);	/* make sure it's able to deal with words */

    for (i=0; i<rasterimage_GetHeight(rasterToUse); i++) {
/*	if (TypeOfCoding == at_TYPE_OF_CODING_bitmap) {	*/	/* %%% eventually this can be removed */
	if (1) {		/* %%% eventually this can be removed */
	    bcopy(&RasterContents->sequence_value.bytes[i * bytesPerRow], &rowBuffer[0], bytesPerRow);
	} else {
	    bzero(&rowBuffer[0], bytesPerRow);
	}
	(void) rasterimage_SetRow(rasterToUse, (long) 0, (long) i, rasterimage_GetWidth(rasterToUse), (short *) &rowBuffer[0]);
    }

    free(rowBuffer);


/*
 * use the subrectangle to clip while scaling
 */

    subrectangle.left = (long) Clipping->coord1.first;
    subrectangle.top = (long) Clipping->coord1.second;
    subrectangle.width = (long) Clipping->coord2.first - Clipping->coord1.first + 1;
    subrectangle.height = (long) Clipping->coord2.second - Clipping->coord1.second + 1;


#define convertSMUtoATK(x) ((((((long) 72 * x * gD->SMU_Denominator) + 600) / 1200) + (gD->SMU_Numerator / 2))/ gD->SMU_Numerator)

    if (PelSpacing->null == BOOL_false) {
	if (gD->doDebugging) {
	    printf("ProcessRasterCP:  doing regular scaling as Pel Spacing != NULL\n");
	}
	realWidth = convertSMUtoATK(subrectangle.width * PelSpacing->length / PelSpacing->pel_spaces);
	realHeight = convertSMUtoATK(subrectangle.height * PelSpacing->length * SpacingRatio->first / PelSpacing->pel_spaces / SpacingRatio->second);

	/* if rotating is supported adjust dimensions here %%% */

    } else {
	switch (ImageDimensions->image_dimensions_tag) {
		case WIDTH_CONTROLLED_tag:
		    realWidth = convertSMUtoATK(ImageDimensions->image_dimensions_value.width_controlled.second);
		    realHeight = realWidth * NumberOfLines * SpacingRatio->first / (NumberOfPelsPerLine * SpacingRatio->second);
		    break;

		case HEIGHT_CONTROLLED_tag:
		    realHeight = convertSMUtoATK(ImageDimensions->image_dimensions_value.height_controlled.second);
		    realWidth = realHeight * NumberOfPelsPerLine * SpacingRatio->second / (NumberOfLines * SpacingRatio->first);
		    break;

		case AREA_CONTROLLED_tag:
		    realWidth = convertSMUtoATK(ImageDimensions->image_dimensions_value.area_controlled.preferred_width);
		    realHeight = convertSMUtoATK(ImageDimensions->image_dimensions_value.area_controlled.preferred_height);
		    break;

		case AUTOMATIC_tag:
		    realWidth =	(72 * 8);   /* 8 inches */
		    realHeight = realWidth * NumberOfLines * SpacingRatio->first / (NumberOfPelsPerLine * SpacingRatio->second);
		    break;

		default:
		    realWidth =	72;	/* arbitrary 1 inch square */
		    realHeight = 72;
		    printf("ProcessRasterCP:  Bad value for ImageDimensions->image_dimensions_tag.\n");
		    printf("ProcessRasterCP:  Setting width=%d and height=%d\n", realWidth, realHeight);
		    break;
	}
    }

    /* used mainly for debugging...override the above computation and force the use of all the data */
    if (!(gD->ScaleRasters)) {
	if (gD->doDebugging) {
	    printf("ProcessRasterCP:  scaling of raster has been disabled.\n");
	}
	realWidth = subrectangle.width;
	realHeight = subrectangle.height;
    }

    if (gD->doDebugging) {
	printf("ProcessRasterCP:  about to scale rasterimage, sizes are:\n");
	printf("ProcessRasterCP:  \n");
	printf("ProcessRasterCP:  subrectangle.left = %ld\n", subrectangle.left);
	printf("ProcessRasterCP:  subrectangle.top = %ld\n", subrectangle.top);
	printf("ProcessRasterCP:  subrectangle.width = %ld\n", subrectangle.width);
	printf("ProcessRasterCP:  subrectangle.height = %ld\n", subrectangle.height);
	printf("ProcessRasterCP:  \n");
	printf("ProcessRasterCP:  final width = %ld\n", realWidth);
	printf("ProcessRasterCP:  final height = %ld\n", realHeight);
    }

    theRealDataObject = rasterimage_Create((long) 1, (long) 1);
    (void) rasterimage_GetScaledSubraster(rasterToUse, &subrectangle, realWidth, realHeight, theRealDataObject);


/*
 * flip and rotate if needed %%%
 */

    if (LineProgression	== rg_LINE_PROGRESSION_90) {	/* flip */
	    subrectangle.left = (long) 0;
	    subrectangle.top = (long) 0;
	    subrectangle.width = (long) rasterimage_GetWidth(theRealDataObject);
	    subrectangle.height = (long) rasterimage_GetHeight(theRealDataObject);
	    rasterimage_MirrorUDSubraster(theRealDataObject, &subrectangle);
    }

    if (PelPath	!= rg_PEL_PATH_0) {	/* fix this sometime to rotate also. %%% */
	printf("ProcessRasterCP:  PEL PATH must be 0 degrees, attribute ingored.\n");
    }


/*
 * now make a raster and rasterview and insert the bits.
 */

    if (gD->doDebugging) {
	printf("ProcessRasterCP:  about to make raster object and insert data\n");
    }

    *startPosition = text_GetLength(gD->topLevelDocument);

    dummyVR = text_InsertObject(gD->topLevelDocument, text_GetLength(gD->topLevelDocument), "raster", (char *) NULL);
    newRasterObj = (struct raster *) dummyVR->dataObject;
    if(gD->doDebugging) {
        printf("ProcessRasterCP:  Have new raster object %X\n", newRasterObj);
    }

    rasterimage_SetWriteID(theRealDataObject, (long) theRealDataObject); /* hack to make it write */
    raster_SetPix(newRasterObj,	theRealDataObject); /* attach data and update sub area */

    if (gD->doDebugging){
        printf("ProcessRasterCP:  Leaving\n");
    }

}
