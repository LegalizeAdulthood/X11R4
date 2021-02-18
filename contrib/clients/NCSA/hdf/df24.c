/*****************************************************************************
* 
*			  NCSA HDF version 3.00
*				December, 1989
*
* NCSA HDF Version 3.00 source code and documentation are in the public
* domain.  Specifically, we give to the public domain all rights for future
* licensing of the source code, all resale rights, and all publishing rights.
* 
* We ask, but do not require, that the following message be included in all
* derived works:
* 
* Portions developed at the National Center for Supercomputing Applications at
* the University of Illinois at Urbana-Champaign.
* 
* THE UNIVERSITY OF ILLINOIS GIVES NO WARRANTY, EXPRESSED OR IMPLIED, FOR THE
* SOFTWARE AND/OR DOCUMENTATION PROVIDED, INCLUDING, WITHOUT LIMITATION,
* WARRANTY OF MERCHANTABILITY AND WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE
* 
* NCSA HDF Group:
*  Mike Folk, Swami Natarajan, Chin_Chau Low, Likkai Ng, Peter Webb,
*  Brian Calvert and Drew Hess.
*****************************************************************************/

/*-----------------------------------------------------------------------------
 * File:    df24.c
 * Purpose: read and write 24-bit raster images
 * Invokes: dfgr.c
 * Contents: 
 *  DF24getdims: get dimensions of image
 *  DF24reqil: use this interlace when returning image
 *  DF24getimage: read in image
 *  DF24setdims: set dimensions of image
 *  DF24addimage: write out image
 *
 * Remarks: A RIG specifies attributes associated with an image- lookup table, 
 *          dimension, compression, color compensation etc.
 *---------------------------------------------------------------------------*/


#include "dfgr.h"

static int Newdata = 0;		/* does Readrig contain fresh data? */
static int dimsset = 0;		/* have dimensions been set? */

#define LUT     0
#define IMAGE   1


/*-----------------------------------------------------------------------------
 * Name:    DF24getdims
 * Purpose: get dimensions of next image RIG
 * Inputs:  filename: name of HDF file
 *          pxdim, pydim: pointer to locations for returning x,y dimensions
 *          pil: location for returning interlace of image in file
 * Returns: 0 on success, -1 on failure with DFerror set
 *          *pxdim, *pydim, *pil set on success
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIgetdims
 * Remarks: none
 *---------------------------------------------------------------------------*/

int DF24getdims(filename, pxdim, pydim, pil)
char *filename;
int32 *pxdim, *pydim;
int *pil;
{
    int ncomps;

    do {
        if (DFGRIgetdims(filename, pxdim, pydim, &ncomps, pil, IMAGE)<0)
            return(-1);
    } while (ncomps!=3);

    Newdata = 1;
    return(0);
}

/*-----------------------------------------------------------------------------
 * Name:    DF24reqil
 * Purpose: get next image with specified interlace
 * Inputs:  il: interlace to get next image with
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIreqil
 * Remarks: none
 *---------------------------------------------------------------------------*/

int DF24reqil(il)
int il;
{
    return(DFGRIreqil(il, IMAGE));
}

/*-----------------------------------------------------------------------------
 * Name:    DF24getimage
 * Purpose: get image from next RIG
 * Inputs:  filename: name of HDF file
 *          image: pointer to space to return image
 *          xdim, ydim: dimensions of space to return lut
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIgetimlut
 * Remarks: space is assumed to be xdim * ydim * 3 bytes
 *---------------------------------------------------------------------------*/

int DF24getimage(filename, image, xdim, ydim)
char *filename;
char *image;
int32 xdim, ydim;
{
    int ret, *pil;
    int32 *pxdim, *pydim;
#ifdef lint
    /* shut lint up */
    pxdim = pydim = (int32*)NULL;
    pil = (int *)NULL;
#endif /*lint*/

    if (!Newdata)
        if (DF24getdims(filename, pxdim, pydim, pil)<0) return(-1);
    ret = DFGRIgetimlut(filename, image, xdim, ydim, IMAGE, 0);
    Newdata = 0;
    return(ret);
}


/*-----------------------------------------------------------------------------
 * Name:    DF24setdims
 * Purpose: set dimensions of image to write next
 * Inputs:  xdim, ydim: dimensions of image
 *          il: interlace of image
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIsetdims
 * Remarks: none
 *---------------------------------------------------------------------------*/

int DF24setdims(xdim, ydim, il)
int32 xdim, ydim;
int il;
{
    dimsset = 1;
    return(DFGRIsetdims(xdim, ydim, 3, il, IMAGE));
}


/*-----------------------------------------------------------------------------
 * Name:    DF24addimage
 * Purpose: Write out image
 * Inputs:  filename: name of HDF file
 *          image: image to write
 *          xdim, ydim: dimensions of array image
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIaddimlut
 * Remarks: array image is assumed to be xdim * ydim * 3 bytes
 *---------------------------------------------------------------------------*/

int DF24addimage(filename, image, xdim, ydim)
char *filename;
char *image;
int32 xdim, ydim;
{
    /* 0 == C */
    if (!dimsset)
        if (DFGRIsetdims(xdim, ydim, 3, 0, IMAGE)<0) return(-1);
    dimsset = 1;

    return(DFGRIaddimlut(filename, image, xdim, ydim, IMAGE, 0));
}
