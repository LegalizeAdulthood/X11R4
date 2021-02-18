/* $Header: null.c,v 1.5 89/07/02 15:58:41 moraes Exp $ */
#include <values.h>

#include "xpic.h"
#include "gels.h"
#include "assert.h"
#include "newfonts.h"

/*
 *  Removes all gels from 'gel' that are strictly within the limits
 *  defined by xmin, ymin and xmax, ymax.
 */
Gel *FindContainedGels(gel, xmin, ymin, xmax, ymax)
Gel **gel;
{
	register Gel *g;
	register Gel *tmp;
	Gel *contained = NULL;
	Gel *others = NULL;
	Box clip;

	if (!gel || !*gel)
		return ((Gel *) NULL);
	clip.ll.x = xmin;
	clip.ll.y = ymin;
	clip.ur.x = xmax;
	clip.ur.y = ymax;
	for (g = *gel; g != NULL; g = tmp) {
		tmp = g->next;
		if (within(&g->b_box, &clip)) {
			g->next = contained;
			contained = g;
		} else {
			g->next = others;
			others = g;
		}
	}
	*gel = others;
	return(contained);
}

/* 
 *  Removes all gels from 'gel' that intersect the box defined by xmin,
 *  ymin and xmax, ymax.
 */
Gel *FindIntersectingGels(gel, xmin, ymin, xmax, ymax)
Gel **gel;
{
	register Gel *g;
	register Gel *tmp;
	Gel *intersecting = NULL;
	Gel *others = NULL;
	Box clip;

	if (!gel || !*gel)
		return ((Gel *) NULL);
	clip.ll.x = xmin;
	clip.ll.y = ymin;
	clip.ur.x = xmax;
	clip.ur.y = ymax;
	for (g = *gel; g != NULL; g = tmp) {
		tmp = g->next;
		if (intersects(&g->b_box, &clip)) {
			g->next = intersecting;
			intersecting = g;
		} else {
			g->next = others;
			others = g;
		}
	}
	*gel = others;
	return(intersecting);
}



/************MAKE SURE THESE UPDATE THE BBOX***************/
/*
 *  Move those points in the gel list that are in the box 'clip' by (dx, dy)
 */
void AdjustGel(g, clip, dx, dy)
Gel *g;
Box *clip;
int dx, dy;
{
	register XPoint *v;
	register int i;
	Conic *conic;
	PointList *ptlist;
	TextString *text;

	for (; g != NULL; g = g->next) {
		switch (g->type) {
		case BOX:
			g->b_box.ll.x += dx;
			g->b_box.ll.y += dy;
			g->b_box.ur.x += dx;
			g->b_box.ur.y += dy;
			break;
		case CIRCLE:
		case ELLIPSE:
			conic = (Conic *)g->data;
			if (contains(&conic->centre, clip)) {
				conic->centre.x += dx;
				conic->centre.y += dy;
				g->b_box.ll.x += dx;
				g->b_box.ll.y += dy;
				g->b_box.ur.x += dx;
				g->b_box.ur.y += dy;
			}
			break;
		case LINE:
		case SPLINE:
			ptlist = (PointList *)g->data;
			for(i = ptlist->nVerts, v = ptlist->v; i > 0; i--, v++) {
				if (containsXY((int) v->x, (int) v->y, clip)) {
					v->x += dx;
					v->y += dy;
				}
			}
			CalcBBox(g, 1);
			break;
		case TEXT:
			text = (TextString *)g->data;
			if (containsXY((int) text->x, (int) text->y, clip)) {
				text->x += dx;
				text->y += dy;
				g->b_box.ll.x += dx;
				g->b_box.ll.y += dy;
				g->b_box.ur.x += dx;
				g->b_box.ur.y += dy;
			}
			break;
		default:
			break;
		}
	}
}


/* Move all the gels in the gellist g by dx, dy */
void MoveGel(g, dx, dy)
Gel *g;
int dx, dy;
{
	register XPoint *v;
	register int i;
	Conic *conic;
	PointList *ptlist;
	TextString *text;

	for(; g != NULL; g = g->next) {
		g->b_box.ll.x += dx;
		g->b_box.ll.y += dy;
		g->b_box.ur.x += dx;
		g->b_box.ur.y += dy;
		switch (g->type) {
		case BOX:
			break;
		case CIRCLE:
		case ELLIPSE:
			conic = (Conic *)g->data;
			conic->centre.x += dx;
			conic->centre.y += dy;
			break;
		case LINE:
		case SPLINE:
			ptlist = (PointList *)g->data;
			for(i = ptlist->nVerts, v = ptlist->v; i > 0; i--, v++) {
				v->x += dx;
				v->y += dy;
			}
			break;
		case TEXT:
			text = (TextString *)g->data;
			text->x += dx;
			text->y += dy;
			break;
		default:
			break;
		}
	}
}

/* Makes a copy of the first N gels in g and returns a pointer to them */
Gel *CopyGel(g, n)
Gel *g;
int n;
{
	Gel *newgel = NULL;
	char *s;
	PointList *ptlistdata;
	char *malloc();
	int x1, x2, y1, y2;
	Conic *conic;
	PointList *ptlist;
	TextString *text;

	for(;(g != NULL) && n--; g = g->next) {
		x1 = g->b_box.ll.x;
		y1 = g->b_box.ll.y;
		x2 = g->b_box.ur.x;;
		y2 = g->b_box.ur.y;
		switch (g->type) {
		case BOX:
			AddBoxGel(&newgel, x1, y1, x2, y2, g->attributes, g->linewidth);
			break;
		case CIRCLE:
		case ELLIPSE:
			conic = (Conic *)g->data;
			AddConicGel(&newgel, g->type, conic->centre.x, conic->centre.y,
			 conic->xrad, conic->yrad, g->attributes, x1, y1, x2, y2, 
			 g->linewidth);
			break;
		case LINE:
		case SPLINE:
			ptlist = (PointList *)g->data;
			if ((ptlistdata = (PointList *) 
			 NewPtList(ptlist->v, ptlist->nVerts)) == NULL) {
			 	message("Can't make new point list in copy");
				FreeGel(newgel);
				return((Gel *) NULL);
			}
			AddLineGel(&newgel, g->type, ptlistdata, g->attributes, x1, y1,
			 x2, y2, g->linewidth);
			break;
		case TEXT:
			text = (TextString *)g->data;
			if ((s = malloc((unsigned) (text->length + 1))) == NULL) {
				message("Can't make a new string in copy");
				FreeGel(newgel);
				return((Gel *) NULL);
			}
			bcopy(text->str, s, text->length);
			s[text->length] = '\0';
			AddTextGel(&newgel, s, text->length, text->font, 
			 text->sizeindex, text->fontname, text->fontsize, 
			 g->attributes, text->x, text->y, x1, y1, x2, y2);
			break;
		default:
			break;
		}
	}
	return (newgel);
}



/*
 *  Finds the closest gel to the point mx, my.  Closeness is measured
 *  from the centre of the Gel bounding box. The gels flags are marked
 *  to indicate selection
 */
Gel *SelectGel(gel, mx, my)
Gel *gel;
{
	Gel *closest = NULL;
	register Gel *g;
	int min_distance = MAXINT;
	int dist;
	
	if (!gel) {
		message("Nothing to select!");
		return(NULL);
	}
again:
	for(g = gel;g != NULL; g = g->next) {
		if (g->int_flags & SELECTED)
			continue;
		switch (g->type) {
		case TEXT:
		case BOX:
			dist = box_distance(g, mx, my);
			break;
		case ELLIPSE:
			dist = ellipse_distance(g, mx, my);
			break;
		case CIRCLE:
			dist = circle_distance(g, mx, my);
			break;
		case LINE:
		case SPLINE:
			dist = line_distance(g, mx, my);
			break;
		}
		if (dist < min_distance) {
			min_distance = dist;
			closest = g;
		}
	}
	if (closest)
		closest->int_flags |= SELECTED;
	else {
		ClearGelFlags(gel);
		message("Looping around again");
		goto again;
	}
#ifdef DEBUG
	printf("closest distance = %d\n", min_distance);
	PrintGel(closest, 1);
#endif
	return(closest);
}


/*
 *  Removes the gel g2 from the list g1 and returns. g2 must be a gel in
 *  g1
 */
Gel *RemoveGel(g1, g2)
Gel **g1;
Gel *g2;
{
	register Gel *g;
	Gel *gtmp;
	
	ASSERT (g1 && *g1 && g2, "RemoveGel null")
	if (*g1 == g2) {
		g = *g1;
		*g1 = g->next;
		g->next = NULL;
		return(g);
	}
	for(g = *g1; g->next != NULL; g = g->next) {
		if (g->next == g2) {
			gtmp = g->next;
			g->next = (g->next)->next;
			gtmp->next = NULL;;
			return(gtmp);
		}
	}
	/* Shouldn't ever reach here */
	ASSERT(0, "RemoveGel not found");
	/* NOTREACHED */
}


/* Changes the attributes of a Gel to the ones passed as arguments */
ChangeAttrib(gel, lineattr, arrowattr, linewidth, fillattr, 
	textattr, textfont, textsize)
FontFamily *textfont;
Gel *gel;
{
	TextString *text;
	
	for(;gel != NULL; gel = gel->next) {
		gel->attributes = 0;
		gel->linewidth = linewidth;
		switch(gel->type) {
		case BOX:
			gel->attributes = lineattr | fillattr;
			break;
		case ELLIPSE:
		case CIRCLE:
			gel->attributes = lineattr | fillattr;
			break;
		case TEXT:
			gel->attributes = textattr;
			text = (TextString *) gel->data;
			text->font = textfont;
			text->sizeindex = textfont->cursize;
			text->fontname = textfont->name;
			text->fontsize = textsize;
			CalcBBox(gel, 1);
			break;
		case LINE:
		case SPLINE:
			gel->attributes = lineattr | arrowattr;
			break;
		default:
#ifdef DEBUG
			(void) sprintf(errstring, "Unknown gel type in Change Attribute");
			message(errstring);
#endif
			break;
		}
	}
}


/*
 *  This recalculates the bounding boxes of the first 'n' gels in the
 *  'gel'. (or as many as there are, if there are less than n)
 */
CalcBBox(g, n)
register Gel *g;
{
	register int i;
	register XPoint *vert;
	int twidth, theight, dx, dy, pad;
	XFontStruct *font;
	Conic *conic;
	TextString *text;
	PointList *ptlist;
	
	for (; g != NULL && n--; g = g->next) {
		switch (g->type) {
		case BOX:
			break;
		case CIRCLE:
		case ELLIPSE:
			conic = (Conic *) g->data;
			g->b_box.ll.x = conic->centre.x - conic->xrad;
			g->b_box.ll.y = conic->centre.y - conic->yrad;
			g->b_box.ur.x = conic->centre.x + conic->xrad;
			g->b_box.ur.y = conic->centre.y + conic->yrad;
			break;
		case LINE:
		case SPLINE:
			ptlist = (PointList *) g->data;
			g->b_box.ll.x = g->b_box.ll.y = MAXINT;
			g->b_box.ur.x = g->b_box.ur.y = 0;
			i = ptlist->nVerts;
			vert = ptlist->v;
			if (g->type == SPLINE) {
				vert++;
				i -= 2;
			}
			for (; i != 0; i--, vert++) {
				update_box(g->b_box, vert->x, vert->y);
			}
			break;
		case TEXT:
			text = (TextString *) g->data;
			/* Only need the font for calculation - so we don't setfont() */
			font = ChangeFont(&text->font->sizes[text->sizeindex], &pad);
			ComputeTextPosition(text->str, text->length, font, 
			 &dx, &dy, &twidth, &theight, 
			 g->attributes & HALIGN, g->attributes & VALIGN, pad);
			g->b_box.ll.x = text->x + dx;
			g->b_box.ur.x = g->b_box.ll.x + twidth;
			g->b_box.ur.y = text->y + dy;
			g->b_box.ll.y = g->b_box.ur.y - theight;
			break;
		default:
			(void) sprintf(errstring, "Unknown gel type %d in CalcBBox",
			 g->type);
			message(errstring);
			break;
		}
	}
}
			
			
#ifdef DEBUG
static char *geltypes[] = {"CELL", "LINE", "SPLINE", "BOX", "CIRCLE",
	"ELLIPSE", "TEXT", "BLOCK", "ELEMENT" };
static char *linestyles[] = {"SOLID", "DOTTED", "SDASH", "LDASH", "DDASH"};
static char *arrowstyles[] = {"NO_ARROW", "ST_ARROW", "EN_ARROW", "BOTH"};
static char *texthalign[] = {"CENTRE", "LJUST", "RJUST"};
static char *textvalign[] = {"MID", "TOP", "BOT"};
static char *flags[] = {"NOTHING", "SELECTED", "HILITED", "SELECTED, HILITED"};

PrintCell(cell)
Cell *cell;
{
	printf("*********************************************************\n");
	printf("Cell \"%s\", file \"%s\"\n", cell->name? cell->name: "(null)",
	 cell->filename? cell->filename: "(null)");
	printf("Last modified filetime is %ld, cell is %smodified\n", 
	 cell->mtime, cell->saved? "" : "not ");
	printf("Cell Gel list is : \n");
	PrintGel(cell->gelList, MAXINT);
	printf("-----------------------------------------------------------\n");
	printf("\n\nNumber of elements to pop in an undo is %d\n", cell->undo);
	printf("Undo list to push:\n");
	PrintGel(cell->undoList, MAXINT);
	printf("-----------------------------------------------------------\n");
}
	
	
PrintGel(g, n)
Gel *g;
{
	printf("=========================================================");
	for (; g && n--; g = g->next) {
		printf("\n%d width=%d %8s ", g->number, g->linewidth, geltypes[g->type]);
		printf("%8s ", linestyles[getlinestyle(g->attributes)]);
		printf("%8s ", arrowstyles[getlinearrow(g->attributes)]);
		printf("%8s ", texthalign[gettext_halign(g->attributes)]);
		printf("%8s ", textvalign[gettext_valign(g->attributes)]);
		printf("%s\n", flags[g->int_flags]);
		PrintBox(&g->b_box);
		switch(g->type) {
		case BOX:
			break;
		case TEXT:
			PrintText((TextString *) g->data);
			break;
		case CIRCLE:
		case ELLIPSE:
			PrintConic((Conic *) g->data);
			break;
		case LINE:
		case SPLINE:
			PrintPtlist((PointList *) g->data);
			break;
		default:
			printf("\tI don't think I know this type\n");
		}
		printf("\tnext = 0x%x\n", g->next);
	}
}

PrintPoint(p)
Point *p;
{
	printf("%d, %d", p->x, p->y);
}

PrintBox(b)
Box *b;
{
	printf("\tll = ");
	PrintPoint(&b->ll);
	printf(", ur = ");
	PrintPoint(&b->ur);
	printf("\n");
}

PrintText(text)
TextString *text;
{
	printf("\t%d, %d \"%s\", %d, %s,(%s) %d(%d)\n", text->x, text->y, 
	 text->str, text->length, text->font? text->font->name : "No font", 
	 text->fontname, text->font->sizes[text->sizeindex].pointsize, 
	 text->fontsize);
}

PrintConic(conic)
Conic *conic;
{
	printf("\t");
	PrintPoint(&conic->centre);
	printf(" with radii %d, %d\n", conic->xrad, conic->yrad);
}

PrintXPoint(v, n)
XPoint *v;
int n;
{
	int i, j;
	
	printf("\t");
	j = 0;
	for (i = 0; i < n; i++) {
		printf("%d, %d   ",  (int) v[i].x, (int) v[i].y);
		if (++j == 5) {
			j = 0;
			printf("\n\t");
		}
	}
}


PrintPtlist(pt)
PointList *pt;
{
	printf("\tn = %d\n", pt->nVerts);
	PrintXPoint(pt->v, pt->nVerts);
}

#endif DEBUG

