/*
 * National Center for SuperComputing Applications, University of Illinois
 *
 * This NCSA software product is public domain software.  Permission
 * is hereby granted to do whatever you like with it. Should you wish
 * to make a contribution towards the production of this software, please
 * send us your comments about your experience with the software,  why
 * you liked or disliked it, how you use it, and most importantly, how it
 * helps your work. We will receive your comments at softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Eng-Whatt Toh, National Center for Supercomputing Applications
 *         ewtoh@ncsa.uiuc.edu
 */

/*
 *	File:		td_axes.c
 *	Contents:	Axes Planes Data calls
 *
 */

#include "td_com.h"


/*
 *	Return dimension of SDS along axis
 */
int
td_HdfgetDim(hdf,axis)
A_Data_t	*hdf;
int axis;
{
	if (axis < 0)
		axis = -axis;

	return(hdf->dims[axis-1]);
}


/*
 *	Check if plane exists along axis
 */
int32
td_HdfCheckPlane(hdf,axis,planeID)
A_Data_t	*hdf;
int	axis;
int	planeID;
{
	int32 max,plane=0;

	if (planeID < 0)
		plane = 0;
	else
	{
		max = (int32)td_HdfgetDim(hdf,axis);
		if ((int32)planeID >= max)
			plane = max-1;
		else
			plane = planeID;
	}

	return(plane);
}


/*
 *	Convert numeric data to pixel data
 */
char
*td_HdfgetPixData(hdf,ys,xs,axis,planeID,interp,nColors)
A_Data_t *hdf;
int 	ys,xs,axis,planeID;
Boolean	interp;
unsigned char nColors;
{
	char	*data;
	int32	k=0,baseline=0,linesize,singleline;
	int32	xincr,yincr;
	int32	xstart,ystart,xend,yend;
	int32	x,y,xdim,ydim;
	int32	plane;
	int32	scale=hdf->scale;
	float32	min=hdf->min,max=hdf->max,frac=hdf->rangeFrac;
	float32	***hdfdata=hdf->data;

	plane = td_HdfCheckPlane(hdf,axis,planeID);
	xdim = td_HdfgetDim(hdf,xs);
	ydim = td_HdfgetDim(hdf,ys);
	if (interp == TRUE)
	{
		xdim--; ydim--;
	}
	data = td_Malloc1D((int)(xdim*scale),(int)(ydim*scale),
			(long)(sizeof(unsigned char)),"td_Malloc1D");
	if (data == NULL)
		return((char *)NULL);

	if (xs > 0)
	{
		xincr = 1; xstart = 0; xend = xdim;
	}
	else
	{
		xincr = -1;
		if (interp == TRUE)
		{
			xstart = xdim; xend = 0;
		}
		else
		{
			xstart = xdim-1; xend = -1;
		}
	}
	if (ys > 0)
	{
		yincr = -1;
		if (interp == TRUE)
		{
			ystart = ydim; yend = 0;
		}
		else
		{
			ystart = ydim-1; yend = -1;
		}
	}
	else
	{
		yincr = 1; ystart = 0; yend = ydim;
	}

	singleline = xdim*scale;
	linesize = singleline*scale;

	switch(axis)
	{
		case 1: /* axis=X */
			y = ystart;
			if ((xs == 2) || (xs == -2))
			{
				while (y != yend)
				{
					x = xstart;
					k = 0;
					while (x != xend)
					{
						if (interp == TRUE)
							td_HdfInterpPixel(scale,min,max,nColors,frac,data,
							hdfdata[plane][x][y],
							hdfdata[plane][x+xincr][y],
							hdfdata[plane][x+xincr][y+yincr],
							hdfdata[plane][x][y+yincr],
							k,baseline,singleline);
						else
							td_HdfsetPixel(scale,data,
							XDS_CNV2PIX(hdfdata[plane][x][y],min,max,frac,nColors),
							k,baseline,singleline);
						x += xincr;
						k += scale;
					}	
					baseline += linesize;
					y += yincr;
				}
			}
			else
			{
				while (y != yend)
				{
					x = xstart;
					k = 0;
					while (x != xend)
					{
						if (interp == TRUE)
							td_HdfInterpPixel(scale,min,max,nColors,frac,data,
							hdfdata[plane][y][x],
							hdfdata[plane][y][x+xincr],
							hdfdata[plane][y+yincr][x+xincr],
							hdfdata[plane][y+yincr][x],
							k,baseline,singleline);
						else
							td_HdfsetPixel(scale,data,
							XDS_CNV2PIX(hdfdata[plane][y][x],min,max,frac,nColors),
							k,baseline,singleline);
						x += xincr;
						k += scale;
					}	
					baseline += linesize;
					y += yincr;
				}
			}
			break;
		case 2: /* axis=Y */
			y = ystart;
			if ((xs == 1) || (xs == -1))
			{
				while (y != yend)
				{
					x = xstart;
					k = 0;
					while (x != xend)
					{
						if (interp == TRUE)
							td_HdfInterpPixel(scale,min,max,nColors,frac,data,
							hdfdata[x][plane][y],
							hdfdata[x+xincr][plane][y],
							hdfdata[x+xincr][plane][y+yincr],
							hdfdata[x][plane][y+yincr],
							k,baseline,singleline);
						else
							td_HdfsetPixel(scale,data,
							XDS_CNV2PIX(hdfdata[x][plane][y],min,max,frac,nColors),
							k,baseline,singleline);
						x += xincr;
						k += scale;
					}
					baseline += linesize;
					y += yincr;
				}
			}
			else
			{
				while (y != yend)
				{
					x = xstart;
					k = 0;
					while (x != xend)
					{
						if (interp == TRUE)
							td_HdfInterpPixel(scale,min,max,nColors,frac,data,
							hdfdata[y][plane][x],
							hdfdata[y][plane][x+xincr],
							hdfdata[y+yincr][plane][x+xincr],
							hdfdata[y+yincr][plane][x],
							k,baseline,singleline);
						else
							td_HdfsetPixel(scale,data,
							XDS_CNV2PIX(hdfdata[y][plane][x],min,max,frac,nColors),
							k,baseline,singleline);
						x += xincr;
						k += scale;
					}	
					baseline += linesize;
					y += yincr;
				}
			}
			break;
		case 3: /* Axis=Z */
			y = ystart;
			if ((xs == 1) || (xs == -1))
			{
				while (y != yend)
				{
					x = xstart;
					k = 0;
					while (x != xend)
					{
						if (interp == TRUE)
							td_HdfInterpPixel(scale,min,max,nColors,frac,data,
							hdfdata[x][y][plane],
							hdfdata[x+xincr][y][plane],
							hdfdata[x+xincr][y+yincr][plane],
							hdfdata[x][y+yincr][plane],
							k,baseline,singleline);
						else
							td_HdfsetPixel(scale,data,
							XDS_CNV2PIX(hdfdata[x][y][plane],min,max,frac,nColors),
							k,baseline,singleline);
						x += xincr;
						k += scale;
					}	
					baseline += linesize;
					y += yincr;
				}
			}
			else
			{
				while (y != yend)
				{
					x = xstart;
					k=0;
					while (x != xend)
					{
						if (interp == TRUE)
							td_HdfInterpPixel(scale,min,max,nColors,frac,data,
							hdfdata[y][x][plane],
							hdfdata[y][x+xincr][plane],
							hdfdata[y+yincr][x+xincr][plane],
							hdfdata[y+yincr][x][plane],
							k,baseline,singleline);
						else
							td_HdfsetPixel(scale,data,
							XDS_CNV2PIX(hdfdata[y][x][plane],min,max,frac,nColors),
							k,baseline,singleline);
						x += xincr;
						k += scale;
					}	
					baseline += linesize;
					y +=  yincr;
				}
			}
			break;
	}

	return(data);
}


/*
 *	Get numeric data for spreadsheet display
 */
char
**td_HdfgetPlaneData(hdf,ys,xs,axis,planeID)
A_Data_t	*hdf;
int ys,xs,axis,planeID;
{
	char	**strngs;
	char	value[10];
	int		i=0;
	int32	xincr,yincr;
	int32	xstart,ystart;
	int32	xend,yend;
	int32	x,y;
	int32	xdim,ydim;
	int32	plane;
	float32	***hdfdata=hdf->data;

	plane = td_HdfCheckPlane(hdf,axis,planeID);

	xdim = td_HdfgetDim(hdf,xs);
	ydim = td_HdfgetDim(hdf,ys);
	strngs = td_Malloc2D((int)xdim,(int)ydim,(long)sizeof(char *),
				"td_Malloc2D");

	if (xs > 0)
	{ xincr = 1; xstart = 0; xend = xdim; }
	else
	{ xincr = -1; xstart = xdim-1; xend = -1; }

	if (ys > 0)
	{ yincr = -1; ystart = ydim-1; yend = -1; }
	else
	{ yincr = 1; ystart = 0; yend = ydim; }

	switch(axis)
	{
		case 1: /* axis=X */
			y = ystart;
			while (y != yend)
			{
				x = xstart;
				while (x != xend)
				{
					if (xs == 2)
						sprintf(value,"%6.2f",hdfdata[plane][x][y]);
					else
						sprintf(value,"%6.2f",hdfdata[plane][y][x]);
					strngs[i] = td_Malloc1D(strlen(value),1,(long)sizeof(char),
								"Malloc1D:string");
					strcpy(strngs[i],value);
					i++;
					x = x + xincr;
				}	
				y = y + yincr;
			}
			break;
		case 2: /* axis=Y */
			y = ystart;
			while (y != yend)
			{
				x = xstart;
				while (x != xend)
				{
					if (xs == 1)
						sprintf(value,"%6.2f",hdfdata[x][plane][y]);
					else
						sprintf(value,"%6.2f",hdfdata[y][plane][x]);
					strngs[i] = td_Malloc1D(strlen(value),1,(long)sizeof(char),
								"Malloc1D:string");
					strcpy(strngs[i],value);
					i++;
					x = x + xincr;
				}	
				y = y + yincr;
			}
			break;
		case 3: /* Axis=Z */
			y = ystart;
			while (y != yend)
			{
				x = xstart;
				while (x != xend)
				{
					if (xs == 1)
						sprintf(value,"%6.2f",hdfdata[x][y][plane]);
					else
						sprintf(value,"%6.2f",hdfdata[y][x][plane]);
					strngs[i] = td_Malloc1D(strlen(value),1,(long)sizeof(char),
								"Malloc1D:string");
					strcpy(strngs[i],value);
					i++;
					x = x + xincr;
				}	
				y = y + yincr;
			}
			break;
	}

	strngs[i] = NULL;

	return (strngs);
}


/*
 *	Get horizontal scale values for spreadsheet display
 */
char
**td_HdfgetHScale(hdf,xs)
A_Data_t	*hdf;
int xs;
{
	char	value[10];
	int		i=0;
	int		x;
	int		xdim;
	int		xincr,xstart,xend;
	char	**strngs;

	xdim = td_HdfgetDim(hdf,xs);
	strngs = td_Malloc2D((int)xdim,(int)1,(long)sizeof(char *),
				"td_Malloc2D");

	if (xs > 0)
	{ xincr = 1; xstart = 0; xend = xdim; }
	else
	{ xincr = -1; xstart = xdim-1; xend = -1; }

	x = xstart;
	while (x != xend)
	{
		sprintf(value,"%6d",x);
		strngs[i] = td_Malloc1D(strlen(value),1,(long)sizeof(char),
					"Malloc1D:string");
		strcpy(strngs[i],value);
		x = x + xincr;
		i++;
	}	
	strngs[i] = NULL;

	return(strngs);
}


/*
 *	Get vertical scale values for spreadsheet display
 */
char
**td_HdfgetVScale(hdf,ys)
A_Data_t	*hdf;
int ys;
{
	char	value[10];
	int		i=0;
	int		y;
	int		ydim;
	int		yincr,ystart,yend;
	char	**strngs;

	ydim = td_HdfgetDim(hdf,ys);
	strngs = td_Malloc2D((int)1,(int)ydim,(long)sizeof(char *),
				"td_Malloc2D");

	if (ys < 0)
	{ yincr = 1; ystart = 0; yend = ydim; }
	else
	{ yincr = -1; ystart = ydim-1; yend = -1; }

	y = ystart;
	while (y != yend)
	{
		sprintf(value,"%6d",y);
		strngs[i] = td_Malloc1D(strlen(value),1,(long)sizeof(char),
					"Malloc1D:string");
		strcpy(strngs[i],value);
		y = y + yincr;
		i++;
	}	
	strngs[i] = NULL;

	return(strngs);
}
