/* 
 * @(#) ol_draw.c 1.11 89/10/31 
 */

/*
 * OPEN LOOK object drawing package
 * Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <X11/Xlib.h>

#include <olgx_private/olgx_impl.h>

int
  calc_add_ins(width, add_ins)
int width;
short add_ins[20];
   {
     register int this_log2 = 4;
     register int this_bit;
     int nchars = 0;
     int i;
     
     this_bit = 1 << this_log2;
     for (this_bit = 1 << this_log2;
	  this_log2 >= 0 && width && nchars < 20;
	  this_bit = this_bit >> 1, this_log2--) {
       while (width >= this_bit) {
	 width -= this_bit;
	 add_ins[nchars++] = this_log2;
       }
     }
     return(nchars);
   }

void
  olgx_draw_box(info, win, x, y, width, height, state, fill_in)
Graphics_info *info;
Window win;
int x, y, width, height, state, fill_in;
   {
     

     width -= 1; /* This takes care of the fact , that the 
		    width passed is the including the endlines
		    of the box */
     height -= 1;


     if (state & OLGX_ERASE)
       XFillRectangle(info->dpy,win,info->three_d?info->gc[OLGX_BG1]:
		      info->gc[OLGX_WHITE],x,y,width+1,height+1);	  
     if (info->three_d){
       
       XPoint point[3];
       
       point[0].x = x;
       point[1].x = x;
       point[1].y = y;
       point[2].y = y;
       /*
	* We handle the shading slightly differently for horizontal
	* and vertical boxes. This allows small (width or height = 1)
	* boxes to be shaded properly, for tick marks and such.

	if (height == 1|| width == 1){
       if (height > width) {
       vertical boxes
	 point[0].y = y + height - 2;
	 point[2].x = x + width - 1;
       }
       else {
       horizontal boxes
	 point[0].y = y + height - 1 ;
	 point[2].x = x + width - 2;
       }
       */
	   point[0].y = y + height;
           point[2].x = x + width ;

       XDrawLines(info->dpy, win, 
		  (state & OLGX_INVOKED) ? info->gc[OLGX_BG3] : info->gc[OLGX_WHITE], point, 3, CoordModeOrigin);
       
       point[0].y = y + height;
       point[1].x = x + width ;
       point[1].y = y + height;
       point[2].x = x + width ;

       point[0].x = x+1;
       point[2].y = y;

       XDrawLines(info->dpy, win, 
		  (state & OLGX_INVOKED) ? info->gc[OLGX_WHITE] : info->gc[OLGX_BG3],
		  point, 3, CoordModeOrigin);
       
       if (fill_in && width > 1 && height > 1) {
	 XFillRectangle(info->dpy, win, 
			(state & OLGX_INVOKED) ? info->gc[OLGX_BG2] : info->gc[OLGX_BG1],
			x + 1, y + 1, width - 2, height - 2);
       }
     }
     else { /* 2d */
       if (state & OLGX_INVOKED) {
	 if (fill_in)
	   XFillRectangle(info->dpy,win,info->gc[OLGX_WHITE],x,y,width,height);	  
	 else{
	   /* Draw the special invoked state */
	   XRectangle rect[3];
           rect[0].x = x; rect[0].y = y; rect[0].width = width;rect[0].height=height;
	   rect[1].x = x+1; rect[1].y = y+1; rect[1].width = width-2;rect[1].height=height-2;
	   XDrawRectangles(info->dpy,win,info->gc[OLGX_BLACK],rect,2);	  
	   
	 }
       }
       else
	 XDrawRectangle(info->dpy,win,info->gc[OLGX_BLACK],x,y,width,height);
     }
   }

void
  olgx_draw_chiseled_box(info, win, x, y, width, height, fill_in, color)
Graphics_info *info;
Window win;
int x, y, width, height, fill_in, color;
   {
     XPoint point[3];
     
     point[0].x = x+1;
     point[0].y = y + height - 3;
     point[1].x = x+1;
     point[1].y = y+1;
     point[2].x = x + width - 3;
     point[2].y = y+1;
     XDrawLines(info->dpy, win, info->gc[OLGX_WHITE], point, 3, CoordModeOrigin);
     
     point[0].x = x;
     point[0].y = y + height - 1;
     point[1].x = x + width - 1;
     point[1].y = y + height - 1;
     point[2].x = x + width - 1;
     point[2].y = y;
     XDrawLines(info->dpy, win, info->gc[OLGX_WHITE], point, 3, CoordModeOrigin);
     
     point[0].x = x + 1;
     point[0].y = y + height - 2;
     point[1].x = x + width - 2;
     point[1].y = y + height - 2;
     point[2].x = x + width - 2;
     point[2].y = y + 1;
     XDrawLines(info->dpy, win, info->gc[OLGX_BG3], point, 3, CoordModeOrigin);
     
     point[0].x = x;
     point[0].y = y + height - 2;
     point[1].x = x;
     point[1].y = y;
     point[2].x = x + width - 2;
     point[2].y = y;
     XDrawLines(info->dpy, win, info->gc[OLGX_BG3], point, 3, CoordModeOrigin);
     
     if (fill_in) {
       XFillRectangle(info->dpy, win, info->gc[color],
		      x + 2, y + 2, width - 4, height - 4);
     }
   }

olgx_draw_choice_item(info, win, x, y, width, height, label, state)
     Graphics_info *info;
     Window win;
     long label;
     int x, y, width, height, state;
   {
     int def_decr;
     
     /* Special inactive case, so pass invoked state */
     /* to the draw box routine and swish that out later */
     
     if (!(info->three_d) && (state & OLGX_INACTIVE))
       state |= OLGX_INVOKED;

     /* draw a box (3d or 2d) outline, filling it in only if invoked */

     olgx_draw_box(info, win, x, y, width, height, 
		   state, (info->three_d)? 1:0);

     if (!(info->three_d) && (state & OLGX_INACTIVE))
       state ^= OLGX_INVOKED;

     /* the default ring rectangle looks better with width-5 for 3d */
     
     def_decr = info->three_d?6:5;

     if (state & OLGX_DEFAULT) {
       /* draw an inner box for a default setting */
       XDrawRectangle(info->dpy, win, (info->three_d)?info->gc[OLGX_BG3]:
		      info->gc[OLGX_BLACK], x + 2, y + 2, (width - def_decr), (height - def_decr));
     }
     
     /*
      * Now place the label
      */
        if (label) {
	 if (state & OLGX_LABEL_IS_PIXMAP) {
	   olgx_draw_pixmap_label(info, win,
				  (Pixmap) label,
				  x + ((state & OLGX_DEFAULT) ? 3 : 1),
				  y + ((state & OLGX_DEFAULT) ? 3 : 1),
				  width - ((state & OLGX_DEFAULT) ? 6 : 2),
				  height - ((state & OLGX_DEFAULT) ? 6 : 2));
	 }
	 else {
	   /* special case for choice invoked in drawing text 
	      where the invoked state is changed to uninvoked and
	      sent to the text drawing routine */
	   if (state & OLGX_INVOKED)
	     state ^= OLGX_INVOKED;
	   olgx_draw_text(info, win, 
			  (char *) label,
   /* a small hack to make sure , that the between the left side
      of the choice item and the text is okay under 14pt
      and 19pt size.. we are using the same info->base_off value
   */
		  x + ((info->button_height > 20)?info->base_off +2:info->base_off),
			  y + height - info->base_off,
			  width - ((info->button_height > 20)?
			  info->base_off +2 : info->base_off),
			  state);
	   /* reset to invoked state */
	   if (state & OLGX_INVOKED)
	     state = state | OLGX_INVOKED;
	 }
       }
      
     if (state & OLGX_INACTIVE){
       /* Inactive State */

       /* grey out the entire thing */
       olgx_stipple_rect(info, win, x, y, width, height);
       
       }
   }
     
     
 


