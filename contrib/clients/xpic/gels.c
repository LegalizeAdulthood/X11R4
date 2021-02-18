/* $Header: gels.c,v 1.4 89/07/02 15:58:13 moraes Exp $ */
#include "xpic.h"
#include "gels.h"

/*
 *  These routines create new gels of the appropriate types, and push them
 *  on a gelList.
 */

/*
 *  Circles, and Ellipses
 */
void AddConicGel(g, type, xc, yc, xr, yr, attr, xmin, ymin, xmax, ymax, thickness)
Gel **g;
int type;
int xc, yc, xr, yr, attr, thickness;
int xmin, ymin, xmax, ymax;
{
	Conic *data;
	Gel *gel;
	
	if ((gel = NewGel()) != NULL) {
		if ((data = (Conic *) malloc(sizeof(Conic))) == NULL) {
		 	message("Not enough memory for Ellipse element");
			FreeGel(gel);
			return;
		}
		data->centre.x = xc;
		data->centre.y = yc;
		data->xrad = xr;
		data->yrad = yr;
		gel->data = (caddr_t) data;
		gel->type = type;;
		gel->b_box.ll.x = xmin;
		gel->b_box.ll.y = ymin;
		gel->b_box.ur.x = xmax;
		gel->b_box.ur.y = ymax;
		gel->attributes = attr;
		gel->linewidth = thickness;
		gel->next = NULL;
		(void) PushGel(g, gel);
	}
}


/*
 *  Text
 */
void AddTextGel(g, s, len, font, size, fontname, fontsize, attr, x, y, 
 x1, y1, x2, y2)
Gel **g;
char *s;
FontFamily *font;
char *fontname;
int len, size, attr, x, y, x1, y1, x2, y2;
{
	TextString *data;
	Gel *gel;
	
	if ((gel = NewGel()) != NULL) {
		if ((data = (TextString *) malloc(sizeof(TextString))) == NULL) {
		 	message("Not enough memory for Text element");
			FreeGel(gel);
			return;
		}
		data->str = s;
		data->length = len;
		data->font = font;
		data->sizeindex = size;
		data->fontname = fontname;
		data->fontsize = fontsize;
		data->x = x;
		data->y = y;
		gel->data = (caddr_t) data;
		gel->type = TEXT;
		gel->b_box.ll.x = x1;
		gel->b_box.ll.y = y1;
		gel->b_box.ur.x = x2;
		gel->b_box.ur.y = y2;
		gel->attributes = attr;
		gel->next = NULL;
		(void) PushGel(g, gel);
	}
}


/*
 *  A box is represented by its bounding box
 */
void AddBoxGel(g, x1, y1, x2, y2, attr, thickness)
Gel **g;
int x1, y1, x2, y2, attr, thickness;
{
	Gel *gel;
	
	if ((gel = NewGel()) != NULL) {
		gel->data = (caddr_t) NULL;
		gel->type = BOX;
		gel->b_box.ll.x = x1;
		gel->b_box.ll.y = y1;
		gel->b_box.ur.x = x2;
		gel->b_box.ur.y = y2;
		gel->attributes = attr;
		gel->linewidth = thickness;
		gel->next = NULL;
		(void) PushGel(g, gel);
	}
}

/*
 *  For lines, and splines
 */
void AddLineGel(g, type, pt, attr, xmin, ymin, xmax, ymax, thickness)
Gel **g;
PointList *pt;
int type;
int attr, xmin, ymin, xmax, ymax, thickness;
{
	Gel *gel;
	
	if ((gel = NewGel()) != NULL) {
		gel->data = (caddr_t) pt;
		gel->type = type;
		gel->b_box.ll.x = xmin;
		gel->b_box.ll.y = ymin;
		gel->b_box.ur.x = xmax;
		gel->b_box.ur.y = ymax;
		gel->attributes = attr;
		gel->linewidth = thickness;
		gel->next = NULL;
		(void) PushGel(g, gel);
	}
}
