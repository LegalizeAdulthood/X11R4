/*
 * This NCSA software product is public domain software.  Permission
 * is granted to do whatever you like with it. Should you feel compelled 
 * to make a contribution for the use of this software, please send 
 * comments as to what you like or don't like about the software, 
 * how you use it, how it helps your work, etc. to softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Dave Thompson, National Center for Supercomputing Applications
 *         davet@ncsa.uiuc.edu
 */

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include "i.h"

#include <hdf/df.h>
#include <hdf/dfi.h>
#include <hdf/dfrig.h>
#include <hdf/dfsd.h>

int ReadHDFPalette();

int ReadSEQImage(i)
/* Read in an SEQ image, return size of image read in */
/* return NULL if couldn't find it */
struct IMAGE *i;
{
int     fd;
unsigned char    *imagebuff;
int	length;
int	lengthToRead;
char	msgbuff[256];


	sprintf(msgbuff,"Reading in SEQ image %s\n",i->name);
	PrintMesg(msgbuff);

	if ((fd = open(i->name,O_RDONLY)) < 0 ) {
		sprintf(msgbuff,"Can't open %s\n",i->name);
		PrintMesg(msgbuff);
		return(NULL);
		}

	lengthToRead = i->xdim * i->ydim;
	if ( ! (imagebuff = (unsigned char *) XIMalloc(lengthToRead))) {
		PrintMesg("Couldn't XIMalloc memory for image\n");
		close(fd);
		return(NULL);   /* couldn't get memory */
		}

	if ((length = read(fd,imagebuff,lengthToRead)) != lengthToRead) {
		PrintMesg("Image file is too small for given dimensions\n");
		XIFree(imagebuff);
		return(NULL);
		}

	close(fd);

	i->ri = imagebuff;
	i->isFloat = FALSE;
	i->fdataset = NULL;
	i->format = "%3d";
	i->fwidth = 3;
	i->maxvalue = 255;
	i->minvalue = 0;
	i->hasPalette = FALSE;
	i->palette = (XColor *) NULL;
	
	return(length);

} /* ReadSEQImage() */

int ReadSEQAnimation(fname,imagebuff,xdim,ydim)
/* return NULL on failure */
char	*fname;
char	*imagebuff;
int	xdim,ydim;
{
int	fd;
int	length;
int	lengthToRead;
char	msgbuff[256];

        if ((fd = open(fname,O_RDONLY)) < 0 ) {
		sprintf(msgbuff,"Can't open %s\n",fname);
		PrintMesg(msgbuff);
		return(NULL);
		}

	lengthToRead = xdim * ydim;
	if ((length = read(fd,imagebuff,lengthToRead)) != lengthToRead) {
		sprintf(msgbuff,"%s is too small for given dimensions\n",fname);
		PrintMesg(msgbuff);
		return(NULL);
		}

	close(fd);
	return(length);
}


int ReadHDFImage(i)
struct IMAGE *i;
{
DF *hdf_df;
int     lengthToRead;
int	xdim,ydim;
char	msgbuff[256];

	sprintf(msgbuff,"Reading in HDF image %s\n",i->name);
	PrintMesg(msgbuff);

        lengthToRead = i->xdim * i->ydim;
        if ( ! (i->ri = (unsigned char *) XIMalloc(lengthToRead))) {
                PrintMesg("Couldn't XIMalloc memory for image\n");
                return(NULL);   /* couldn't get memory */
                }
	DFR8restart();
	if (DFR8getimage(i->name,i->ri,i->xdim,i->ydim,NULL) == -1) {
		PrintMesg("Couldn't read in HDF image\n");
		return(NULL);   /* failed */
		}


	i->palette = (XColor  *) NULL;
	i->hasPalette = FALSE;
	if(DFPnpals(i->name) > 0) {
        	if ( ! (i->palette=(XColor *) XICalloc(256,sizeof(XColor)))) {
			PrintMesg("Couldn't XIMalloc memory for palette\n");
			return(NULL);   /* couldn't get memory */
			}
		if (ReadHDFPalette(i->name,i->palette))
			i->hasPalette = TRUE;
		else
			XIFree(i->palette);
		}


	i->isFloat = FALSE;
	i->fdataset = NULL;
	i->format = "%3d";
	i->fwidth = 3;
	i->maxvalue = 255;
	i->minvalue = 0;

	sprintf(msgbuff,"Dimension of this HDF image: %dx%d\n",i->xdim,i->ydim);
	PrintMesg(msgbuff);

	return(i->xdim * i->ydim);	/* return length if ok */
} /* ReadHDFImage() */


MakeHDFImageIndex(file,index,size)
/* make an index table for accessing the images out of HDF*/
char *file;
int index[];
int size;
{
int x;
int32	xdim,ydim;
int	ispalette;

	DFR8restart();
	for(x = 0; x < size; x++)  {
		if ( -1 == DFR8getdims(file,&xdim,&ydim,&ispalette))
			return(0);
		index[x] = DFR8lastref();
		}
	
}


int ReadHDFAnimationImage(file,raster,frameNum)
char *file;
unsigned char *raster;
int	frameNum; /* must be an HDF ref number (see MakeHDFImageIndex()) */
{
/* too slow
int x;
int32	xdim,ydim;
int	ispalette;

	DFR8restart();
	for (x=0;x<frameNum;x++) 
		if ( -1 == DFR8getdims(file,&xdim,&ydim,&ispalette))
			return(0);

	DFR8getimage(file,raster,xdim,ydim,NULL);
	return(xdim*ydim);
*/
/* doesn't do uncompression 
DF *dfile;

	if (!(dfile = DFopen(file,DFACC_READ,1000)))
		return(0);
	if (-1 == DFgetelement(dfile,DFTAG_RI8,frameNum,raster)) {
	    if (-1 == DFgetelement(dfile,DFTAG_CI8,frameNum,raster)) {
		if (-1 == DFgetelement(dfile,DFTAG_II8,frameNum,raster)) {
			DFclose(dfile);
			return(0);
			}
		}
	   }
	DFclose(dfile);
	return(1);
*/
/* just right */
int32	xdim,ydim;
int	ispalette;

	if ( -1 == DFR8readref(file,frameNum))
		return(0);
	if ( -1 == DFR8getdims(file,&xdim,&ydim,&ispalette))
		return(0);
	if (-1 == DFR8getimage(file,raster,xdim,ydim,NULL))
		return(0);

	return(xdim*ydim);
} /* ReadHDFAnimationImage() */



int ReadHDFSDS(i,x,y,z)
struct IMAGE *i;
int	x,y,z;		/* contains dimension of image */
			/* x,y,z: one of these must be set to 1 */
			/* (2D plane along [xyz] must be set to 1) */
/* return 1 if ok */
/* return 0 on failure */
{
DF *hdf_df;
int rank;
int32 sizes[100];
int	maxrank;
char	msgbuff[256];
static 	int32	s1[] = {1,1,1};
int32	s2[3];
int32	dims[3];

	sprintf(msgbuff,"Reading in HDF data set %s\n",i->name);
	PrintMesg(msgbuff);

	DFSDrestart();
/*	DFSDgetdims(i->name,&rank,sizes,maxrank);*/
/*******if (DFSDgetdata(i->name,1,sizes,i->fdataset))*****fill me in **/
	s2[0]=x;
	s2[1]=y;
	s2[2]=z;
	if (x==1) {
		dims[0]=y;
		dims[1]=z;
		}
	else {
		if (y==1) {
			dims[0]=x;
			dims[1]=z;
			}
		else {
			dims[0]=x;
			dims[1]=y;
			}
		}

	if (!(i->fdataset=(float32 *)XIMalloc(dims[0]*dims[1]*sizeof(float32)))){
	    sprintf(msgbuff,"Could not allocate memory for HDF data set%s\n",
			i->name);
	    PrintMesg(msgbuff);
	    return(NULL);
	    }
	
	i->isFloat = TRUE;
	i->xdim = dims[0];
	i->ydim = dims[1];

	if (DFSDgetslice(i->name,s1,s2,i->fdataset,2,dims) == -1 ) {
		PrintMesg("Can't read in HDF data set\n");
		return(NULL);
		}
	
	if (DFSDgetmaxmin(&(i->maxvalue),&(i->minvalue)) == -1) {
		/*max and min not set, scan dataset and set by hand*/
		float max,min;
		register int x,last;
		register float *pf;

		pf = i->fdataset;
		min = max = *(i->fdataset);
		last = i->xdim*i->ydim;

		for (x = 0; x < last ; x++,pf++) {
			min = (*pf < min)? *pf : min;
			max = (*pf > max)? *pf : max;
			}
		i->maxvalue = max;
		i->minvalue = min;
		}
/*	printf("Max = %f, Min= %f",i->maxvalue,i->minvalue);*/
	
	/*if (DFSDgetdatastrs())*/
	/* set format */
	i->format = "% 6.3f";
	i->fwidth = 7;

	i->palette = (XColor *) NULL;
	i->hasPalette = FALSE;
	if(DFPnpals(i->name) > 0) {
        	if ( ! (i->palette=(XColor *) 
				XICalloc(256,sizeof(XColor)))) {
			PrintMesg("Couldn't XIMalloc memory for palette\n");
			return(NULL);   /* couldn't get memory */
			}
		if (ReadHDFPalette(i->name,i->palette))
			i->hasPalette = TRUE;
		else
			XIFree(i->palette);
		}


	/* Create raster image */
	{
	int length;
	int x;
	float range;
	unsigned char *pc;
	float *pf;
	float min;

	length = i->xdim * i->ydim;
	if (! (i->ri = (unsigned char *) XIMalloc(length))) {
	     PrintMesg("Can't allocate memory for image of HDF data slice\n");
	     return(NULL);
	     }
	range = i->maxvalue - i->minvalue;
	min = i->minvalue;
	for (x = 0, pc=i->ri, pf=i->fdataset; x < length; x++,pc++,pf++ ){
		*pc =(unsigned char) ((( *pf - min ) / range) *255.0);
		}
	}
	sprintf(msgbuff,"Dimension of this slice: %dx%d\n",i->xdim,i->ydim);
	PrintMesg(msgbuff);

	return(1); /* return non-NULL if ok */
}




IsItHDF(filename,dimx,dimy,isfloat)
char *filename;
int *dimx,*dimy;
char *isfloat;
/* return null if not HDF */
{
int r;
int32	sizes[3];
int ret;
int ispal;

	ret = DFR8getdims(filename,dimx,dimy,&ispal);
	*isfloat = FALSE;
	if (DFerror == DFE_NOTDFFILE) {
		/* Not HDF file */
		*dimx = *dimy = 0;
		return(0);
		}
	else {
		if (ret == -1) {
			/* Not a DFR8 image, is it a DFSD? */
			ret = DFSDgetdims(filename,&r,sizes,3);
			*dimx=sizes[0];
			*dimy=sizes[1];
			/* dimz=sizes[2]; */
			*isfloat = TRUE;
			}
		}
		
	if (ret == -1) /* if not a DFSD image */
		*dimx = *dimy = 0;

	return(!ret);

} /* IsItHDF() */


int ReadHDFHasPalette(name)
char *name;
/* returns number of pals hdf file has */
/* returns -1 on error */
{
	return(DFPnpals(name));
}

int ReadHDFPalette(name,ccells)
/* return 1 if ok */
/* return 0 on failure */
char	*name;
XColor	ccells[256];
{
DF *hdf_df;
int     lengthToRead;
int	xdim,ydim;
int	isPalette;
unsigned char pal[256*3];
int	x;
char	buff[512];


/*	if (DFR8getimage(name,"garbage",1,1,pal) == -1) {*/
	DFPrestart();
	if (DFPgetpal(name,pal) == -1) {
                sprintf(buff,"Couldn't read in HDF palette %s\n",name);
		PrintMesg(buff);
                return(NULL);   /* failed */
		}

	for (x=0; x < 256;x++) {
		ccells[x].pixel = x;
		ccells[x].red   = (unsigned short) pal[x*3] << 8;
		ccells[x].green = (unsigned short) pal[x*3+1] << 8;
		ccells[x].blue  = (unsigned short) pal[x*3+2] << 8;
		ccells[x].flags = DoRed|DoGreen|DoBlue;
                }

	sprintf(buff,"Just read in HDF palette %s\n",name);
	PrintMesg(buff);
	return(1);
} /* ReadHDFPalette() */






int ReadSEQPalette(name,ccells)
/* Return NULL if could not read SEQ colormap */
char *name;
XColor ccells[256];
{
char	rbuff[256];
char	gbuff[256];
char	bbuff[256];
int	fd;		/* file descriptor */
int	x;
char 	errbuff[260];

	if ((fd = open(name,O_RDONLY,NULL)) < 0) {
		(void) sprintf(errbuff,"Can't open colormap file %s\n",name);
		PrintMesg(errbuff);
		return(NULL);
		}

	if (read(fd,rbuff,256) != 256) {
		sprintf(errbuff,"%s is not a seq colormap\n",name);
		PrintMesg(errbuff);
		return(NULL);
		}
	if (read(fd,gbuff,256) != 256) {
		sprintf(errbuff,"%s is not a seq colormap\n",name);
		PrintMesg(errbuff);
		return(NULL);
		}
	if (read(fd,bbuff,256) != 256) {
		sprintf(errbuff,"%s is not a seq colormap\n",name);
		PrintMesg(errbuff);
		return(NULL);
		}

	(void) close(fd);
	

	for (x = 0; x < 256; x++) {
		ccells[x].pixel = x;
		ccells[x].red   = (unsigned short) rbuff[x] << 8;
		ccells[x].green = (unsigned short) gbuff[x] << 8;
		ccells[x].blue  = (unsigned short) bbuff[x] << 8;
		ccells[x].flags = DoRed|DoGreen|DoBlue;
		}

	return(1);

} /* ReadSEQColormap() */


int HowManyImagesInHDF(file)
char *file;
{
DF *df;
int num;
	if(!(df = DFopen(file,DFACC_READ,0)))
		return(0); /* Can't open file */
	if (!(num = DFnumber(df,DFTAG_RI8)))
		if(!(num = DFnumber(df,DFTAG_CI8)))
			num = DFnumber(df,DFTAG_II8);
/**/	if (num == -1){
/**/	    printf("HowManyImagesInHDF(): Error reading HDF file %s\n",file);
/**/	    printf("HDF error nummber %d\n",DFerrno());
/**/	    }

	DFclose(df);
	return(num);
}

int HowManySDSInHDF(file)
char *file;
{
DF *df;
int num;

	df = DFopen(file,DFACC_READ,0);
	num = DFnumber(file,DFTAG_SD);
	DFclose(df);
	return(num);
}
