/* Copyright International Business Machines, 1989
 *
 *
 *
 *  IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 *  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR ANY
 *  PARTICULAR USE.  IN NO EVENT SHALL
 *  IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 *  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 *  SOFTWARE
 *
 *
 *
*/
#include "stones.h"
#include "bitmap.h"
#include <math.h>

#define MAXPTS		100	
XPoint xpt[MAXPTS];

char *units = "poly_verts";
int growing = 0;
int poly_verts = 4;
int poly_shape = Complex;

/****************************************************************************/

int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
	XGCValues		xgcv[1];
	XWindowAttributes xwatt;
	XColor installedcolor[1];
	XColor databasecolor[1];
	Pixmap pmap;
	unsigned int osbg;
	int winW;
	int winH;
	int default_poly = 1;
	int evenodd = 1;
	int tile = 0;
	int stip = 0;
	int opaqstip = 0;
	int indx = 0;
	int i;
	int poly_tilt = 0;
	FILE *fp;

	for(i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-help")==0)  return(1);
		else if (strcmp(argv[i], "-h")==0)
		{
			printf("\n");
			printf("\tPolystones uses XFillPolygon \n");
			printf("\tPolystones' options are: \n");
			printf("\n");
  
  			printf("\t-nverts  (int)        number of vertices for default polygon (default=%d) \n",poly_verts);
			printf("\t-tilt    (int)        degrees to rotate default polygon (default=%d) \n",poly_tilt);
			printf("\t-grow    (int)        growing polygons (default=%d) \n",growing);
			printf("\t-convex               Convex mode for XFillPolygon() \n");
			printf("\t-nonconvex            Nonconvex mode for XFillPolygon() \n");
			printf("\t-complex              Complex mode for XFillPolygon() (default) \n");
			printf("\t-evenodd              EvenOdd fill rule, see XWindows(p.66) (default) \n");
			printf("\t-winding              Winding fill rule, see XWindows(p.66) \n");
			printf("\t-tile                 tile fill \n");
			printf("\t-stip                 stipple fill \n");
			printf("\t-opaqstip             opaque stipple fill \n");
			printf("\t-opaqstipbg (string)  background color for opaque stipple fill \n");
			printf("\t-file   (string)      name of polygon file to use \n");
			printf("\t-help                 stones' help page \n");
			printf("\t-h                    this listing \n");
		
			printf("\n");
			return(1);
		}
		else if (strcmp(argv[i], "-nverts")==0) poly_verts = atoi(argv[++i]); 
		else if (strcmp(argv[i], "-tilt")==0) poly_tilt = atoi(argv[++i]);
		else if (strcmp(argv[i], "-grow")==0) growing = 1;
		else if (strcmp(argv[i], "-convex")==0) poly_shape = Convex;
		else if (strcmp(argv[i], "-nonconvex")==0) poly_shape = Nonconvex;
		else if (strcmp(argv[i], "-complex")==0) poly_shape = Complex;
		else if (strcmp(argv[i], "-evenodd")==0) evenodd = 1;
		else if (strcmp(argv[i], "-winding")==0) evenodd = 0;
		else if (strcmp(argv[i], "-tile")==0) tile = 1;
		else if (strcmp(argv[i], "-stip")==0) stip = 1;
		else if (strcmp(argv[i], "-opaqstip")==0) opaqstip = 1;
		else if (strcmp(argv[i],"-opaqstipbg") ==0 )   
		{
				GETCOLOR(osbg, argv[++i], WhitePixel);
		}
		else if (strcmp(argv[i], "-file")==0) 
		{
			int tempx, tempy;

			default_poly = 0;
			if ((fp = fopen(argv[++i], "r")) == NULL)
				{
					fprintf(stderr,"Error in opening %s .",argv[i]);
					fprintf(stderr,"Using default polygon instead. \n");
					default_poly = 1;
				}
			else
			{
			while (fscanf(fp,"%d %d\n",&tempx,&tempy) != EOF)  
				{
					xpt[indx].x = tempx;
					xpt[indx].y = tempy;
					indx++;
					if (indx > MAXPTS)         /* break; */
						fprintf(stderr,"Error, array indx overflow! Please abort. \n");
				}
			poly_verts = indx;
			fclose(fp);
			}
		}
	}

	XSync( ops->display, 0);
	XGetWindowAttributes(ops->display,ops->window,&xwatt);
	winW = xwatt.width;
	winH = xwatt.height;

	if (default_poly)
	{
		if (growing)
		{
			if (make_growing_poly(poly_tilt,winW,winH))
			return(1);
		}
		else 
		{
			if (make_default_poly(poly_tilt,winW,winH))
			return(1);
		}

	}

	if (!evenodd)
		XSetFillRule(ops->display,ops->fggc,WindingRule);
	else
		XSetFillRule(ops->display,ops->fggc,EvenOddRule);
 
	if (tile)
	{
		pmap = XCreatePixmapFromBitmapData(ops->display, ops->window, 
			bitmap_bits, bitmap_width, bitmap_height, ops->fggc, ops->bggc,
			DefaultDepth(ops->display, ops->dScreen));
		xgcv->fill_style = FillTiled;
		xgcv->tile = pmap;
		XChangeGC(ops->display, ops->fggc, GCTile | GCFillStyle, xgcv);
	}
	
	if (stip)
	{
		pmap = XCreateBitmapFromData(ops->display, ops->window, 
			bitmap_bits, bitmap_width, bitmap_height);
		xgcv->stipple = pmap;
		xgcv->fill_style = FillStippled;
		XChangeGC(ops->display, ops->fggc, GCStipple | GCFillStyle, xgcv);
	}
	
	if (opaqstip)
	{
		pmap = XCreateBitmapFromData(ops->display, ops->window, 
			bitmap_bits, bitmap_width, bitmap_height);
		xgcv->stipple = pmap;
		xgcv->fill_style = FillOpaqueStippled;
		xgcv->background = osbg;
		XChangeGC(ops->display, ops->fggc, 
				GCStipple | GCFillStyle | GCBackground , xgcv);
	}

	return(0);
}


/***********************************************************************/

int
make_default_poly(poly_tilt,winWd,winHt)
int poly_tilt;
int winWd;
int winHt;
{
	int indx;
	int trans_x;
	int trans_y;
	double radius;
	double theta;
	double delta;

	trans_x = winWd / 2; 
	trans_y = winHt / 2; 
	if (trans_x < trans_y) radius = (double)( trans_x - 5 );
		else radius = (double)( trans_y - 5 );
	theta =  (double)(poly_tilt) * 360.0 / M_2PI;
	delta = M_2PI / (double)(poly_verts);
	switch(poly_shape) 
	{
		case Convex:	
		{
			indx = 0;
			break;
		}

		case Nonconvex:	
		{
			delta = M_2PI / (double)(poly_verts - 1);
			xpt[0].x = (short)(radius * cos(theta) + trans_x);
			xpt[0].y = (short)(radius * sin(theta) + trans_y);
			xpt[1].x = trans_x;
			xpt[1].y = trans_y;
			theta += delta;
			indx = 2;
			break;
		}

 		case Complex:	
		{
			xpt[0].x = (short)(radius * cos(theta) + trans_x);
			xpt[0].y = (short)(radius * sin(theta) + trans_y);
			theta += delta;
 			xpt[2].x = (short)(radius * cos(theta) + trans_x);
			xpt[2].y = (short)(radius * sin(theta) + trans_y);
			theta += delta;
 			xpt[1].x = (short)(radius * cos(theta) + trans_x); 
			xpt[1].y = (short)(radius * sin(theta) + trans_y); 
			theta += delta;
			indx = 3;
			break;
		}

		default:
		{
			fprintf(stderr,"Error, Unknown case of poly_shape\n");
			return(1);
		}

	} /* end of switch */

	while (indx < poly_verts)
	{
		xpt[indx].x = (short)(radius * cos(theta) + trans_x);
		xpt[indx].y = (short)(radius * sin(theta) + trans_y);
		theta += delta;
		indx++;
	}
	return(0);

} /* end of make_default_polys */


/***********************************************************************/

int
make_growing_poly(poly_tilt,winWd,winHt)
int poly_tilt;
int winWd;
int winHt;
{
	int indx;
	int trans_x;
	int trans_y;
	double radius;
	double theta;
	double delta;

	if (poly_verts < 3) return(1);
	trans_x = winWd / 2; 
	trans_y = winHt / 2; 
	if (trans_x < trans_y) radius = (double)( trans_x - 5 );
		else radius = (double)( trans_y - 5 );
	theta =  (double)(poly_tilt) * 360.0 / M_2PI;
	delta = M_PI / (double)(poly_verts);
	switch(poly_shape) 
	{
		case Convex:	
		{
			xpt[0].x = (short)(radius * cos(theta) + trans_x);
			xpt[0].y = (short)(radius * sin(theta) + trans_y);
			theta += M_PI;                      /* 180 deg */
			xpt[1].x = (short)(radius * cos(theta) + trans_x);
			xpt[1].y = (short)(radius * sin(theta) + trans_y);
			theta += delta;
			indx = 2;
			break;
		}

		case Nonconvex:	
		{
			if (poly_verts < 4) return(1);
			xpt[0].x = (short)(radius * cos(theta) + trans_x);
			xpt[0].y = (short)(radius * sin(theta) + trans_y);
			xpt[1].x = trans_x;
			xpt[1].y = trans_y;
			theta += 5.0 * M_2PI / 360.0;       /* 20 deg */
			xpt[2].x = (short)(radius * cos(theta) + trans_x);
			xpt[2].y = (short)(radius * sin(theta) + trans_y);
			theta += 178.0 * M_2PI / 360.0;      /* 181 deg */
			xpt[3].x = (short)(radius * cos(theta) + trans_x);
			xpt[3].y = (short)(radius * sin(theta) + trans_y);
			theta += delta;
			indx = 4;
			break;
		}

 		case Complex:	
		{ 
			if (poly_verts < 4) return(1); 
			xpt[0].x = (short)(radius * cos(theta) + trans_x);
			xpt[0].y = (short)(radius * sin(theta) + trans_y);
			theta += 60.0 * M_2PI / 360.0;       /* 60 deg */
 			xpt[2].x = (short)(radius * cos(theta) + trans_x);
			xpt[2].y = (short)(radius * sin(theta) + trans_y);
			theta += 60.0 * M_2PI / 360.0;       /* 60 deg */
 			xpt[1].x = (short)(radius * cos(theta) + trans_x); 
			xpt[1].y = (short)(radius * sin(theta) + trans_y); 
			theta += 60.0 * M_2PI / 360.0;       /* 60 deg */
 			xpt[3].x = (short)(radius * cos(theta) + trans_x); 
			xpt[3].y = (short)(radius * sin(theta) + trans_y); 
			theta += delta;
			indx = 4;
			break;
		}

		default:
		{
			fprintf(stderr,"Error, Unknown case of poly_shape\n");
			return(1);
		}

	} /* end of switch */

	while (indx < poly_verts)
	{
		xpt[indx].x = (short)(radius * cos(theta) + trans_x);
		xpt[indx].y = (short)(radius * sin(theta) + trans_y);
		theta += delta;
		indx++;
	}
	return(0);

} /* end of make_default_polys */

/***************************************************************************/

