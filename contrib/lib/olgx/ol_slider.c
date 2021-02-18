/* 
 * @(#) ol_slider.c 1.8 89/11/14 
 */

/*
 * OPEN LOOK object drawing package
 *
 * ol_slider.c
 * Slider Module
 *
 * Sun Microsystems, Inc.,
 */

#include <stdio.h>
#include <X11/Xlib.h>

#include <olgx_private/olgx_impl.h>

/*
 * Private function declarations
 */

void olgx_draw_slider_control();

/*
 * olgx_draw_slider
 *
 * Presents a uniform interface to the other slider routines:
 *	olgx_draw_vertical_slider
 *	olgx_draw_horizontal_slider
 *	olgx_update_vertical_slider
 *	olgx_update_horizontal_slider
 */

void
  olgx_draw_slider(info, win, x, y, width, oldval, newval, state)
Graphics_info *info;
Window win;
int x, y, width, oldval, newval,  state;
   {
     
 
     if (state & OLGX_VERTICAL) {
       if (state & OLGX_UPDATE) {
	 olgx_update_vertical_slider(info, win, 
				     x, y, width, oldval, newval);
       }
       else {
	 olgx_draw_vertical_slider(info, win, 
				   x, y, width, newval, state);
       }
     }
     else {
       
       if (state & OLGX_UPDATE) {
	 olgx_update_horizontal_slider(info, win, 
				       x, y, width, oldval, newval);
       }
       else {
	 olgx_draw_horizontal_slider(info, win, 
				     x, y, width, newval, state);
       }
     }
   }


/*
 * olgx_draw_horizontal_slider
 */

void
  olgx_draw_horizontal_slider(info, win, x, y, width, value )
Graphics_info *info;
Window win;
int x,y, width, value ;
   {
     XTextItem item;
     int inside_width;
     char string[STRING_SIZE];
     short add_ins[STRING_SIZE];
     register y_slider_offset;
     register int i;
     int num_add ;
     
     /* inside_width is the width excluding the endcaps */
     inside_width = width - (info->se_width << 1);

     y_slider_offset = y + info->slider_offset;
     
     item.font = None;
     item.chars = string;
     item.delta = 0;
     if (!info->three_d){
       
       /* 2d - Unlike 3d, the slider is not split into the */
       /* left and right side, it is drawn in one piece*/
       
       num_add = calc_add_ins(inside_width, add_ins);
       item.nchars = 2 + num_add;
       string[0] = HORIZ_SLIDER_LEFT_ENDCAP_OUTLINE;
       VARIABLE_LENGTH_MACRO(1,HORIZ_SLIDER_OUTLINE_1);
       string[1+num_add] = HORIZ_SLIDER_RIGHT_ENDCAP_OUTLINE;
       XDrawText(info->dpy,win,info->gc[OLGX_BLACK],x,y_slider_offset,&item,1);
       /* The left filled part of the slider  */
       num_add = calc_add_ins(value, add_ins);
       item.nchars = 1 + num_add;
       string[0] = HORIZ_SLIDER_LEFT_ENDCAP_FILL;
       VARIABLE_LENGTH_MACRO(1, HORIZ_SLIDER_FILL_1);
       XDrawText(info->dpy,win,info->gc[OLGX_BLACK],x,y_slider_offset,&item,1);
       
     }
     else { /* 3d*/
       /*
	* draw the left part of the slider
	*/
       num_add = calc_add_ins(value, add_ins);
       item.nchars = 1 + num_add;
       string[0] = HORIZ_SLIDER_UL;	/* upper portion of left side */
       VARIABLE_LENGTH_MACRO(1, BUTTON_TOP_1);
       XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y_slider_offset, &item, 1);
       string[0] = HORIZ_SLIDER_LL; 	/* lower left */
       VARIABLE_LENGTH_MACRO(1, HORIZ_SLIDER_BOTTOM_1);
       XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y_slider_offset, &item, 1);
       string[0] = HORIZ_SLIDER_LEFT_ENDCAP_FILL;
       VARIABLE_LENGTH_MACRO(1, HORIZ_SLIDER_FILL_1);
       XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y_slider_offset, &item, 1);
       
       /* now draw the shading */
       item.nchars= num_add;
       VARIABLE_LENGTH_MACRO(0, BUTTON_TOP_1);
       /* following should be OLGX_BG3 according to spec, but OLGX_BG2 looks better */
       XDrawText(info->dpy, win, info->gc[OLGX_BG2], x+info->se_width, y_slider_offset+1, &item, 1);
       
       /*
	* draw the right part of the slider
	*/
       num_add = calc_add_ins(inside_width-value, add_ins);
       item.nchars = 1 + num_add;
       
       VARIABLE_LENGTH_MACRO(0, BUTTON_TOP_1);
       string[i] = HORIZ_SLIDER_UR;	/* upper portion of right side */
       XDrawText(info->dpy, win, info->gc[OLGX_BG3], 
		 x+value+info->se_width, y_slider_offset, &item, 1);
       
       VARIABLE_LENGTH_MACRO(0, HORIZ_SLIDER_BOTTOM_1);
       string[i] = HORIZ_SLIDER_LR; 	/* lower right */
       XDrawText(info->dpy, win, info->gc[OLGX_WHITE], 
		 x+value+info->se_width, y_slider_offset, &item, 1);
       
       VARIABLE_LENGTH_MACRO(0, HORIZ_SLIDER_FILL_1);
       string[i] = HORIZ_SLIDER_RIGHT_ENDCAP_FILL;
       XDrawText(info->dpy, win, info->gc[OLGX_BG2], 
		 x+value+info->se_width, y_slider_offset, &item, 1);
       
     }
     olgx_draw_slider_control(info, win, 
			      x+value , 
			      y, 
			      OLGX_HORIZONTAL);
     
     
   }

void
  olgx_draw_slider_control(info, win, x, y, orientation)
Graphics_info *info;
Window win;
int x, y, orientation;
   {
     XTextItem item;
     char string[2];
     
     item.nchars = 1;
     item.font = None;
     item.chars = string;
     item.delta = 0;
     
     if (orientation == OLGX_HORIZONTAL) {
       if (info->three_d){
	 string[0] = HORIZ_SLIDER_CONTROL_UL;
	 XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x, y, &item, 1);
	 string[0] = HORIZ_SLIDER_CONTROL_LR;
	 XDrawText(info->dpy, win, info->gc[OLGX_BG3], x, y, &item, 1);
	 string[0] = HORIZ_SLIDER_CONTROL_FILL;
	 XDrawText(info->dpy, win, info->gc[OLGX_BG1], x, y, &item, 1);
       }
       else {
	 string[0] = HORIZ_SLIDER_CONTROL_OUTLINE;
	 XDrawString(info->dpy,win,info->gc[OLGX_BLACK],x,y,string,1);
	 string[0] = HORIZ_SLIDER_CONTROL_FILL;
	 XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x, y, &item, 1);
       }
     }
     else {
       if (info->three_d){
	 string[0] = VERT_SLIDER_CONTROL_UL;
	 XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x, y, &item, 1);
	 string[0] = VERT_SLIDER_CONTROL_LR;
	 XDrawText(info->dpy, win, info->gc[OLGX_BG3], x, y, &item, 1);
	 string[0] = VERT_SLIDER_CONTROL_FILL;
	 XDrawText(info->dpy, win, info->gc[OLGX_BG1], x, y, &item, 1);
       }
       else {
	 string[0] = VERT_SLIDER_CONTROL_OUTLINE;
	 XDrawString(info->dpy,win,info->gc[OLGX_BLACK],x,y,string,1);
	 string[0] = VERT_SLIDER_CONTROL_FILL;
	 XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x, y, &item, 1);
       }
     }
   }



void
  olgx_update_horizontal_slider(info, win, x, y, width, 
				old_value,new_value  )
Graphics_info *info;
Window win;
int x,y, width, new_value, old_value;
   {
     XTextItem item;
     char string[STRING_SIZE];
     register int i;
     int y_slider_offset, inside_width;
     int xstart, xwidth;
     
     if (old_value == new_value) {
       return;
     }
     
     item.nchars = 1;
     item.font = None;
     item.chars = string;
     item.delta = 0;
     
     
     /* calculate width minus the slider endcaps */
     inside_width = (width - (info->se_width << 1));
     
     /* calculate the y offset of the slider control from the channel */
     y_slider_offset = y +info->slider_offset;
     
     if (old_value < new_value) {
       /* calculate start & width of area needing repaint */
       xstart = x + old_value;
       xwidth = (new_value - old_value);
       
       /* 
	* erase the old slider 
	*/
       XFillRectangle(info->dpy, win,info->three_d? info->gc[OLGX_BG1]
		      :info->gc[OLGX_WHITE], xstart, 
		      y,
		      xwidth, info->slider_height);
       
       
       /*
	* repair the damaged area
	*/
       XFillRectangle(info->dpy, win, info->gc[OLGX_BLACK], 
		      xstart, y_slider_offset, xwidth,info->se_height);
       
       /* now draw the shading */
       /* following should be OLGX_BG3 according to spec
	  but OLGX_BG2 looks better */
       XDrawLine(info->dpy, win, info->three_d ? info->gc[OLGX_BG2]:
		 info->gc[OLGX_BLACK], xstart, y_slider_offset+1, 
		 xstart + xwidth - 1, y_slider_offset+1);
       if (old_value < 4){ /* Draw the leftendcap when necessary */

	 if (!info->three_d){	/* 2d  */
	 XFillRectangle(info->dpy, win, info->gc[OLGX_WHITE], x, y_slider_offset, info->se_width,info->se_height);
	   string[0] = HORIZ_SLIDER_LEFT_ENDCAP_OUTLINE;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y_slider_offset, &item, 1);
	   string[0] = HORIZ_SLIDER_LEFT_ENDCAP_FILL;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y_slider_offset, &item, 1);
	 }
	 else{			/* 3d  */
	 XFillRectangle(info->dpy, win, info->gc[OLGX_BG1], x, y_slider_offset, info->se_width,info->se_height);
	   string[0] = HORIZ_SLIDER_UL;
	   
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y_slider_offset, &item, 1);
	   string[0] = HORIZ_SLIDER_LL; 	
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y_slider_offset, &item, 1);
	   string[0] = HORIZ_SLIDER_LEFT_ENDCAP_FILL;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y_slider_offset, &item, 1);
	   
	 }  
       }
       /* draw the new one */
       olgx_draw_slider_control(info, win, x+new_value,
				y , OLGX_HORIZONTAL);
     }
     else {
       xstart = x + new_value; 
       xwidth = (old_value - new_value)+info->slider_width;
       
       /* 
	* erase the old slider 
	*/
       XFillRectangle(info->dpy, win, info->gc[OLGX_BG1], xstart, 
		      y,
		      xwidth, info->slider_height+1);
       
       
       /* redraw the damaged area */	
       XDrawLine(info->dpy, win, info->three_d ? info->gc[OLGX_BG3]:
		 info->gc[OLGX_BLACK], xstart, y_slider_offset,
		 xstart + xwidth-1, y_slider_offset);
       XDrawLine(info->dpy, win, info->three_d ? info->gc[OLGX_WHITE] :
		 info->gc[OLGX_BLACK], xstart, 
		 y_slider_offset + (info->se_height - 1),
		 xstart + xwidth-1, y_slider_offset+(info->se_height - 1));
       XFillRectangle(info->dpy, win,info->three_d? info->gc[OLGX_BG2]:
		      info->gc[OLGX_WHITE], xstart, y_slider_offset+1,
		      xwidth, (info->se_height-2));
       if (old_value+info->slider_width > width-4){ /* Draw the endcap when necessary */
	 if (!info->three_d){	/* 2d  */
	 XFillRectangle(info->dpy, win, info->gc[OLGX_WHITE], x+inside_width+info->se_width, y_slider_offset,info->se_width,info->se_height);
	   string[0] = HORIZ_SLIDER_RIGHT_ENDCAP_OUTLINE;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x+inside_width+info->se_width, y_slider_offset, &item, 1);
	   string[0] = HORIZ_SLIDER_RIGHT_ENDCAP_FILL;
	   XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x+inside_width+info->se_width, y_slider_offset, &item, 1);
	 }
	 else{			/* 3d  */
	 XFillRectangle(info->dpy, win, info->gc[OLGX_BG1], x+inside_width+info->se_width, y_slider_offset,info->se_width,info->se_height);
	   string[0] = HORIZ_SLIDER_UR;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x+inside_width+info->se_width, y_slider_offset, &item, 1);
	   string[0] = HORIZ_SLIDER_LR; 	
	   XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x+inside_width+info->se_width, y_slider_offset, &item, 1);
	   string[0] = HORIZ_SLIDER_RIGHT_ENDCAP_FILL;
	   XDrawText(info->dpy, win, info->gc[OLGX_BG2], x+inside_width+info->se_width, y_slider_offset, &item, 1);
	   
	 }  
       }       
       /* draw the new one */
       olgx_draw_slider_control(info, win, x+new_value,
				y , OLGX_HORIZONTAL);
       
     }
     
     
   }

void
  olgx_update_vertical_slider(info, win, x, y, height, 
			      old_value,new_value)
Graphics_info *info;
Window win;
int x,y, height, new_value, old_value;
   {
     register int i;
     int ystart, yheight;
     int inside_height, x_slider_offset;
     XTextItem item;
     char string[2];
     
     item.nchars = 1;
     item.font = None;
     item.chars = string;
     item.delta = 0;
     
     if (old_value == new_value) {
       return;
     }
     
     
     /* inside_width is the width excluding the channel endcaps */
     inside_height = height - (info->se_width << 1);
     
     /* slider_offset is the x offset of the slider from the channel */
     x_slider_offset = x+info->slider_offset;
     
     if (old_value > new_value) {
       /* calculate start & width of area needing repaint */
       ystart = y + new_value + info->slider_width;
       yheight = (old_value - new_value);
       
       /* 
	* erase the old slider 
	*/
       XFillRectangle(info->dpy, win, info->three_d? info->gc[OLGX_BG1]
		      :info->gc[OLGX_WHITE],
		      x, ystart,
		      info->slider_height, yheight);
       
       
       /*
	* repair the damaged area
	*/
       XFillRectangle(info->dpy, win, info->gc[OLGX_BLACK], x_slider_offset, ystart,
		      info->se_height, yheight);
       
       /* now draw the shading */
       /* following should be OLGX_BG3 according to spec
	  but OLGX_BG2 looks better */
       XDrawLine(info->dpy, win, info->three_d? info->gc[OLGX_BG2]:
		 info->gc[OLGX_BLACK], x_slider_offset+1, ystart,
		 x_slider_offset+1, ystart + yheight - 1);
       if (old_value+info->slider_width > height-4){ /* Draw the endcap when necessary */
	 if (!info->three_d){	/* 2d  */
	 XFillRectangle(info->dpy, win, info->gc[OLGX_WHITE], x_slider_offset, y+inside_height+ info->se_width,info->se_height,info->se_width);
	   string[0] = VERT_SLIDER_BOTTOM_ENDCAP_OUTLINE;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x_slider_offset, y+inside_height+
		     info->se_width, &item, 1);
	   string[0] = VERT_SLIDER_BOTTOM_ENDCAP_FILL;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x_slider_offset, y+inside_height+
		     info->se_width, &item, 1);
	 }
	 else{			/* 3d  */
	 XFillRectangle(info->dpy, win, info->gc[OLGX_BG1], x_slider_offset, y+inside_height+
			info->se_width,info->se_height,info->se_width);
	   string[0] = VERT_SLIDER_LL;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x_slider_offset, y+inside_height+
		     info->se_width, &item, 1);
	   string[0] = VERT_SLIDER_LR; 	
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x_slider_offset, y+inside_height+
		     info->se_width, &item, 1);
	   string[0] = VERT_SLIDER_BOTTOM_ENDCAP_FILL;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x_slider_offset, y+inside_height+
		     info->se_width, &item, 1);
	   
	 }  
       }       
       
       /* draw the new one */
       olgx_draw_slider_control(info, win, x, 
				y+new_value, OLGX_VERTICAL);
       
     }
     else {
       ystart = y + old_value;
       yheight = (new_value - old_value);
       
       /* 
	* erase the old slider 
	*/
       XFillRectangle(info->dpy, win, info->gc[OLGX_BG1], 
		      x, ystart,
		      info->slider_height, yheight);
       
       
       
       /* redraw the damaged area */	
       XDrawLine(info->dpy, win, info->three_d?info->gc[OLGX_BG3]:
		 info->gc[OLGX_BLACK], x_slider_offset, ystart,
		 x_slider_offset, ystart + yheight - 1);
       XDrawLine(info->dpy, win, info->three_d?info->gc[OLGX_WHITE]:
		 info->gc[OLGX_BLACK], 
		 x_slider_offset + (info->se_height - 1), ystart, 
		 x_slider_offset + (info->se_height - 1), ystart + yheight - 1);
       XFillRectangle(info->dpy, win, info->three_d?info->gc[OLGX_BG2]:
		      info->gc[OLGX_WHITE], x_slider_offset+1, ystart, 
		      (info->se_height - 2), yheight);
       
       if (old_value < 4){ /* Draw the endcap when necessary */

	 if (!info->three_d){	/* 2d  */
	 XFillRectangle(info->dpy, win, info->gc[OLGX_WHITE], x_slider_offset, y, info->se_height,3);
	   string[0] = VERT_SLIDER_TOP_ENDCAP_OUTLINE;
	   XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x_slider_offset, y, &item, 1);
	   string[0] = VERT_SLIDER_TOP_ENDCAP_FILL;
	   XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x_slider_offset, y, &item, 1);
	 }
	 else{			/* 3d  */
	 XFillRectangle(info->dpy, win, info->gc[OLGX_BG1], x_slider_offset, y, info->se_height,3);
	   string[0] = VERT_SLIDER_UL;
	   
	   XDrawText(info->dpy, win, info->gc[OLGX_BG3], x_slider_offset, y, &item, 1);
	   string[0] = VERT_SLIDER_UR; 	
	   XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x_slider_offset, y, &item, 1);
	   string[0] = VERT_SLIDER_TOP_ENDCAP_FILL;
	   XDrawText(info->dpy, win, info->gc[OLGX_BG2], x_slider_offset, y, &item, 1);
	   
	 }  
       }
       /* draw the new one */
       olgx_draw_slider_control(info, win, 
				x , 
				y + new_value, OLGX_VERTICAL);/* draw the new one */
     }
     
   }


void
  olgx_draw_vertical_slider(info, win, x, y, height, value)
Graphics_info *info;
Window win;
int x,y, height, value;
   {
     XTextItem item;
     char string[STRING_SIZE];
     register int i;
     int inside_height, x_slider_offset;
     
     /* inside_height is the height excluding the channel endcaps */
     inside_height = height - (info->se_width << 1);
     
     /* slider_offset is the x offset of the slider from the channel */
     x_slider_offset = x+ info->slider_offset;
     
     
     item.nchars = 1;
     item.font = None;
     item.chars = string;
     item.delta = 0;
     
     /* 2d - Unlike 3d, the slider is not split into the */
     /* left and right side, it is drawn in one piece*/
     if (!info->three_d){
       string[0] = VERT_SLIDER_BOTTOM_ENDCAP_OUTLINE;
       XDrawText(info->dpy, win, info->gc[OLGX_BLACK], 
		 x_slider_offset, y+info->se_width+inside_height, &item, 1);
       string[0] = VERT_SLIDER_BOTTOM_ENDCAP_FILL;
       XDrawText(info->dpy, win, info->gc[OLGX_BLACK],
		 x_slider_offset, y+info->se_width+inside_height, &item, 1);
       XFillRectangle(info->dpy, win, info->gc[OLGX_BLACK], 
		      x_slider_offset, y+value, info->se_height, (height-value-info->se_width));
       XDrawLine(info->dpy, win, info->gc[OLGX_BLACK], 
		 x_slider_offset, y+info->se_width, x_slider_offset, y+value+info->se_width-1);
       XDrawLine(info->dpy, win, info->gc[OLGX_BLACK], 
		 x_slider_offset+ (info->se_height - 1), y + info->se_width, 
		 x_slider_offset+ (info->se_height - 1), y + value + info->se_width - 1);
       XFillRectangle(info->dpy, win, info->gc[OLGX_WHITE], 
		      x_slider_offset+1, y+info->se_width, (info->se_height - 2), value);
       string[0] = VERT_SLIDER_TOP_ENDCAP_OUTLINE;
       XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x_slider_offset, y, &item, 1);
       string[0] = VERT_SLIDER_TOP_ENDCAP_FILL;
       XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x_slider_offset, y, &item, 1);
     }
     else {
       
       /*
	* draw the bottom part of the slider
	* (NOTE: this code could be optimized to use 1 XDrawText call)
	*/
       string[0] = VERT_SLIDER_LR;
       XDrawText(info->dpy, win, info->gc[OLGX_BLACK], 
		 x_slider_offset, y+inside_height+info->se_width, &item, 1);
       
       string[0] = VERT_SLIDER_LL;
       XDrawText(info->dpy, win, info->gc[OLGX_BLACK], 
		 x_slider_offset, y+inside_height+info->se_width, &item, 1);
       
       string[0] = VERT_SLIDER_BOTTOM_ENDCAP_FILL;
       XDrawText(info->dpy, win, info->gc[OLGX_BLACK],
		 x_slider_offset, y+inside_height+info->se_width, &item, 1);
       
       XFillRectangle(info->dpy, win, info->gc[OLGX_BLACK], 
		      x_slider_offset, y+value, info->se_height, (height-value-info->se_width));
       
       
       /*
	* draw the top part of the slider
	*/
       
       XDrawLine(info->dpy, win, info->gc[OLGX_BG3], 
		 x_slider_offset, y+info->se_width, x_slider_offset, y+value+info->se_width-1);
       string[0] = VERT_SLIDER_UL;	/* upper portion of left side */
       XDrawText(info->dpy, win, info->gc[OLGX_BG3], x_slider_offset, y, &item, 1);
       
       XDrawLine(info->dpy, win, info->gc[OLGX_WHITE], 
		 x_slider_offset+ (info->se_height - 1), y + info->se_width, 
		 x_slider_offset+ (info->se_height - 1), y + value + info->se_width - 1);
       string[0] = VERT_SLIDER_UR; 	/* lower left */
       XDrawText(info->dpy, win, info->gc[OLGX_WHITE], x_slider_offset, y, &item, 1);
       
       XFillRectangle(info->dpy, win, info->gc[OLGX_BG2], 
		      x_slider_offset+1, y+info->se_width, (info->se_height - 2), value);
       string[0] = VERT_SLIDER_TOP_ENDCAP_FILL;
       XDrawText(info->dpy, win, info->gc[OLGX_BG2], x_slider_offset, y, &item, 1);
       
       /* now draw the shading on the bottom*/
       /* following should be OLGX_BG3 according to spec, but OLGX_BG2 looks better */
       XDrawLine(info->dpy, win, info->gc[OLGX_BG2], 
		 x_slider_offset+ 1, y + value + info->se_width, 
		 x_slider_offset+ 1, y + inside_height + (info->se_width - 1));
     }
     
     olgx_draw_slider_control(info, win, 
			      x, y+value, OLGX_VERTICAL);
   }








