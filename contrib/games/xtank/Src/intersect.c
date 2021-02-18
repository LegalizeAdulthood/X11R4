/*
** Xtank
**
** Copyright 1988 by Terry Donahue
**
** intersect.c
*/

#include "xtank.h"

Boolean seg_intersect();

/*
** Returns true if the path from start to finish intersects a wall.
*/
Boolean intersect_wall(start,finish)
     Loc *start,*finish;
{
  int dx,dy,lattice_dx,lattice_dy;
  int tgrid_x,tgrid_y,fgrid_x,fgrid_y;

  dx = (int) finish->x - (int) start->x;
  dy = (int) finish->y - (int) start->y;

  tgrid_x = start->grid_x;
  tgrid_y = start->grid_y;
  fgrid_x = finish->grid_x;
  fgrid_y = finish->grid_y;

  if(fgrid_x > tgrid_x)
    if(fgrid_y > tgrid_y) {
      lattice_dx = (tgrid_x + 1)*BOX_WIDTH - (int) start->x;
      lattice_dy = (tgrid_y + 1)*BOX_HEIGHT - (int) start->y;
      while(tgrid_x != fgrid_x || tgrid_y != fgrid_y) {
	if(lattice_dy * dx < dy * lattice_dx) {
	  if(box[tgrid_x][tgrid_y+1].flags & NORTH_WALL) return TRUE;
	  tgrid_y++;
	  lattice_dy += BOX_HEIGHT;
	}
	else {
	  if(box[tgrid_x+1][tgrid_y].flags & WEST_WALL) return TRUE;
	  tgrid_x++;
	  lattice_dx += BOX_WIDTH;
	}
      }
    }
    else if(fgrid_y < tgrid_y) {
      lattice_dx = (tgrid_x + 1)*BOX_WIDTH - (int) start->x;
      lattice_dy = tgrid_y*BOX_HEIGHT - (int) start->y;
      while(tgrid_x != fgrid_x || tgrid_y != fgrid_y) {
	if(lattice_dy * dx > dy * lattice_dx) {
	  if(box[tgrid_x][tgrid_y].flags & NORTH_WALL) return TRUE;
	  tgrid_y--;
	  lattice_dy -= BOX_HEIGHT;
	}
	else {
	  if(box[tgrid_x+1][tgrid_y].flags & WEST_WALL) return TRUE;
	  tgrid_x++;
	  lattice_dx += BOX_WIDTH;
	}
      }
    }
    else {
      for(; tgrid_x < fgrid_x ; tgrid_x++)
	if(box[tgrid_x+1][tgrid_y].flags & WEST_WALL) return TRUE;
    }

  else if(fgrid_x < tgrid_x)
    if(fgrid_y > tgrid_y) {
      lattice_dx = tgrid_x*BOX_WIDTH - (int) start->x;
      lattice_dy = (tgrid_y + 1)*BOX_HEIGHT - (int) start->y;
      while(tgrid_x != fgrid_x || tgrid_y != fgrid_y) {
	if(lattice_dy * dx >= dy * lattice_dx) {
	  if(box[tgrid_x][tgrid_y+1].flags & NORTH_WALL) return TRUE;
	  tgrid_y++;
	  lattice_dy += BOX_HEIGHT;
	}
	else {
	  if(box[tgrid_x][tgrid_y].flags & WEST_WALL) return TRUE;
	  tgrid_x--;
	  lattice_dx -= BOX_WIDTH;
	}
      }
    }
    else if(fgrid_y < tgrid_y) {
      lattice_dx = tgrid_x*BOX_WIDTH - (int) start->x;
      lattice_dy = tgrid_y*BOX_HEIGHT - (int) start->y;
      while(tgrid_x != fgrid_x || tgrid_y != fgrid_y) {
	if(lattice_dy * dx < dy * lattice_dx) {
	  if(box[tgrid_x][tgrid_y].flags & NORTH_WALL) return TRUE;
	  tgrid_y--;
	  lattice_dy -= BOX_HEIGHT;
	}
	else {
	  if(box[tgrid_x][tgrid_y].flags & WEST_WALL) return TRUE;
	  tgrid_x--;
	  lattice_dx -= BOX_WIDTH;
	  }
      }
    }
    else {
      for(; tgrid_x > fgrid_x ; tgrid_x--)
	if(box[tgrid_x][tgrid_y].flags & WEST_WALL) return TRUE;
    }

  else
    if(fgrid_y > tgrid_y) {
      for(; tgrid_y < fgrid_y ; tgrid_y++)
	if(box[tgrid_x][tgrid_y+1].flags & NORTH_WALL) return TRUE;
    }
    else if(fgrid_y < tgrid_y) {
      for(; tgrid_y > fgrid_y ; tgrid_y--)
	if(box[tgrid_x][tgrid_y].flags & NORTH_WALL) return TRUE;
    }
  return FALSE;
}

/*
** Determines whether or not and where a segment intersects an object.
*/
Boolean seg_intersect_obj(obj,seg2,xoffs,yoffs,ipt)
     Object *obj;
     Segment *seg2;
     int xoffs,yoffs;
     Coord *ipt;
{
  Segment *seg1;
  int i;
  
  for(i = 0 ; i < obj->num_segs ; i++) {
    seg1 = &obj->picinfo->segment[i];
    if(seg_intersect(seg1,seg2,xoffs,yoffs,ipt))
       return TRUE;
  }
  return FALSE;
}

/*
** Determines whether or not two polygons overlap when separated by
** the given x and y offsets.
*/
Boolean obj_overlap(obj1,obj2,xoffs,yoffs)
     Object *obj1, *obj2;
     int xoffs, yoffs;
{
  Segment *seg1, *seg2;
  int i,j;

  for(i = 0 ; i < obj1->num_segs ; i++) {
    seg1 = &obj1->picinfo->segment[i];
    for(j = 0 ; j < obj2->num_segs ; j++) {
      seg2 = &obj2->picinfo->segment[i];
      if(seg_intersect(seg1,seg2,xoffs,yoffs,(Coord *) NULL))
	return TRUE;
    }
  }
  return FALSE;
}

/*
** Assumes a coordinate frame from segment 1.  Segment 2 is
** offset from segment 1 by the offset values.
*/
Boolean seg_intersect(seg1,seg2,xoffs,yoffs,ipt)
     Segment *seg1,*seg2;
     int xoffs,yoffs;
     Coord *ipt;
{
  int x1,y1;
  int ix,iy;

  /* Check bounding boxes around the segments for quick ignore */
  if(seg1->maxx < seg2->minx + xoffs || seg1->minx > seg2->maxx + xoffs ||
     seg1->maxy < seg2->miny + yoffs || seg1->miny > seg2->maxy + yoffs)
    return FALSE;

  /* If the slopes are equal, they don't intersect */
  if(seg1->slope == seg2->slope)
    return FALSE;

  /* Compute the first coordinate of the line segment in the new frame */
  x1 = seg2->x1 + xoffs;
  y1 = seg2->y1 + yoffs;

  /* Compute x coordinate of intersection of the two lines */
  ix = (int) ((y1 - x1 * seg2->slope - seg1->intercept) /
	     (seg1->slope - seg2->slope) + .5);

  /* See if that value lies within the bounding box of segment 1 */
  if((ix < seg1->minx) || (ix > seg1->maxx))
    return FALSE;

  /* If we just need to return a boolean, we're done */
  if(ipt == (Coord *) NULL) return TRUE;

  if(seg1->dx == 0)
    iy = (int) (seg2->slope * (ix - x1) + y1 + .5);
  else
    iy = (int) (seg1->slope * (ix - seg1->x1) + seg1->y1 + .5);

  ipt->x = ix;
  ipt->y = iy;
  return TRUE;
}

/*
** Makes a segment structure from two coordinates.
*/
make_segment(seg,x1,y1,x2,y2)
     Segment *seg;
     int x1,y1,x2,y2;
{
  seg->x1 = x1;
  seg->x2 = x2;
  seg->y1 = y1;
  seg->y2 = y2;

  seg->dx = x2 - x1;
  seg->dy = y2 - y1;

  if(seg->dx != 0)
    seg->slope = (float) seg->dy / (float) seg->dx;
  else {
    if(seg->dy > 0)
      seg->slope = 9999999.0;
    else if(seg->dy < 0)
      seg->slope = -9999999.0;
    else
      seg->slope = 0;
  }

  seg->intercept = y1 - seg->slope * x1;

  seg->minx = min(x1,x2);
  seg->miny = min(y1,y2);
  seg->maxx = max(x1,x2);
  seg->maxy = max(y1,y2);
}

/*
** Returns true if the point located at absolute coords (x,y) is
** within the polygon of the specified vehicle
*/
point_in_vehicle(v,x,y)
     Vehicle *v;
     float x,y;
{
  Boolean flag;
  Segment *s, *smax;

  x -= v->loc->x;
  y -= v->loc->y;
  s = v->obj->picinfo[v->vector.rot].segment;
  smax = s + v->obj->num_segs;
  flag = FALSE;

  do {
    if(((x < s->x1) ^ (x < s->x2)) && y < s->intercept + x * s->slope)
      flag ^= TRUE;
  } while(++s < smax);

  return flag;
}
