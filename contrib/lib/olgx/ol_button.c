/* 
 * @(#) ol_button.c 1.11 89/11/02 
 */

/*
 * OPEN LOOK object drawing package
 * Sun Microsystems, Inc.
 *
 * OLGX_button.c
 * Menu button module
 */

#include <stdio.h>
#include <X11/Xlib.h>

#include <olgx_private/olgx_impl.h>
#include <olgx_private/busy.h>			/* busy stipple bitmap */
#include <olgx_private/grey.h>			/* grey stipple bitmap */

/*
 * Private function declarations
 */
void olgx_set_busy_stipple();
void olgx_draw_pixmap_label();

void
  olgx_draw_button(info, win, x, y, width, label, state)
Graphics_info *info;
Window win;
int x,y, width;
long label;
int state;
   {
     XTextItem item;
     char string[STRING_SIZE];
     short add_ins[STRING_SIZE];
     register int i;
     int num_add;
     int inside_width;		/* width minus endcaps */
     int top_color, bottom_color, fill_color;
     
     
     inside_width = width - (2 * info->endcap_width);
     
     num_add = calc_add_ins(inside_width-1, add_ins);
     item.nchars = 2 + num_add;
     item.font = None;
     item.chars = string;
     item.delta = 0;
     if (info->three_d){
       /* 3d 
	* determine what colors we should draw in 
	*/
       if (state & OLGX_INVOKED) {		/* invoked button */
	 top_color = OLGX_BG3;
	 bottom_color = OLGX_WHITE;
	 fill_color = OLGX_BG2;
       }
       else if ((state & OLGX_DEFAULT) && (state & OLGX_MENU_ITEM)) { 
	 /* default menu item */
	 top_color = bottom_color = OLGX_BLACK;
	 fill_color = OLGX_BG1;
       }
       else if (state & OLGX_MENU_ITEM && state & OLGX_BUSY) {
	 /* busy menu item */
	 fill_color = top_color = bottom_color = OLGX_BG1;
       }
       else if (state & OLGX_MENU_ITEM) {
	 /* normal menu item */
	 fill_color = top_color = bottom_color = NONE;
       }
       else {					/* normal panel button */
	 top_color = OLGX_WHITE;
	 bottom_color = OLGX_BG3;
	 fill_color = OLGX_BG1;
	 
       }
       
       if (state & OLGX_BUSY) {
	 /*
	  * This routine changes GC information on-the-fly, but 
	  * it is assumed that OLGX_BUSY won't be called often, so it 
	  * makes sense to use the same GC rather than one for `
	  * each color.
	  */
	 olgx_set_busy_stipple(info, fill_color);	
	 fill_color = OLGX_BUSYGC;
       }
       
       /* only check erase on transparent items */
       if (fill_color == NONE) {
	 if (state & OLGX_ERASE) {
	   /* 
	    * to improve performance, we erase a rectangle the 
	    * size of a button rather than drawing a real button.
	    */
	   XFillRectangle(info->dpy, win, info->gc[OLGX_BG1], x, y,
			  width, info->button_height);
	 }
       }
       else {
	 /* if not transparent, actually draw the button */
	 
	 if (top_color != NONE) {
	   /* draw the top part of the button */
	   string[0] = BUTTON_UL; 
	   VARIABLE_LENGTH_MACRO(1, BUTTON_TOP_1);
	   string[i+1] = BUTTON_UR;
	   XDrawText(info->dpy, win, 
		     info->gc[top_color], x, y, &item, 1);
	 }
	 
	 if (bottom_color != NONE) {
	   /* draw the bottom part of the button */
	   string[0] = BUTTON_LL; 
	   VARIABLE_LENGTH_MACRO(1,BUTTON_BOTTOM_1);
	   string[i+1] = BUTTON_LR; 
	   XDrawText(info->dpy, win, 
		     info->gc[bottom_color], x, y, &item, 1);
	 }
	 
	 /* Fill in the button */
	 string[0] = BUTTON_LEFT_ENDCAP_FILL;
	 VARIABLE_LENGTH_MACRO(1,BUTTON_FILL_1);
	 string[i+1] = BUTTON_RIGHT_ENDCAP_FILL; 
	 XDrawText(info->dpy, win, 
		   info->gc[fill_color], x, y, &item, 1);
	 
	 /* draw the inner border of a default button (not menu item) */
	 if (!(state & OLGX_MENU_ITEM) && (state & OLGX_DEFAULT)) {
	   string[0] = DFLT_BUTTON_LEFT_ENDCAP;
	   VARIABLE_LENGTH_MACRO(1,DFLT_BUTTON_MIDDLE_1);
	   string[i+1] = DFLT_BUTTON_RIGHT_ENDCAP;
	   XDrawText(info->dpy, win, 
		     info->gc[OLGX_BLACK], x, y, &item, 1);
	 }
       }
     }
     else { /* draw 2d button */
       
       if (state & OLGX_ERASE)
	 XFillRectangle(info->dpy, win,info->gc[OLGX_WHITE],x,y,
			width+1, info->button_height+3);
       
       if ((state & OLGX_INVOKED)){
	 string[0] = BUTTON_FILL_2D_LEFTENDCAP;
	 VARIABLE_LENGTH_MACRO(1,BUTTON_FILL_2D_MIDDLE_1);
	 string[i+1] = BUTTON_FILL_2D_RIGHTENDCAP; 
	 XDrawText(info->dpy, win, 
		   info->gc[OLGX_BLACK], x, y, &item, 1);
       }
       
       else if (state & OLGX_BUSY) {
	 olgx_set_busy_stipple(info,OLGX_WHITE);
	 string[0] = BUTTON_FILL_2D_LEFTENDCAP;
	 VARIABLE_LENGTH_MACRO(1,BUTTON_FILL_2D_MIDDLE_1);
	 string[i+1] = BUTTON_FILL_2D_RIGHTENDCAP; 
	 XDrawText(info->dpy, win, 
		   info->gc[OLGX_BUSYGC], x, y, &item, 1);
	 
       }
       else if (!(state & OLGX_MENU_ITEM) && (state & OLGX_DEFAULT)) {
	 /* draw the 2d default ring if not menu-item */
	 string[0] = DFLT_BUTTON_LEFT_ENDCAP;
	 VARIABLE_LENGTH_MACRO(1,DFLT_BUTTON_MIDDLE_1);
	 string[i+1] = DFLT_BUTTON_RIGHT_ENDCAP;
	 XDrawText(info->dpy, win, 
		   info->gc[OLGX_BLACK], x, y, &item, 1);
       }
       
       else if (state & OLGX_DEFAULT) {
	 /* draw the 2d default ring for menu item */
	 string[0] = MENU_DFLT_OUTLINE_LEFT_ENDCAP;
	 VARIABLE_LENGTH_MACRO(1,MENU_DFLT_OUTLINE_MIDDLE_1);
	 string[i+1] = MENU_DFLT_OUTLINE_RIGHT_ENDCAP;
	 XDrawText(info->dpy, win, 
		   info->gc[OLGX_BLACK], x, y, &item, 1);
       }
       /* draw the button if it is not a menu item */
       if (!(state & OLGX_MENU_ITEM)){
	 string[0] = BUTTON_OUTLINE_LEFT_ENDCAP; 
	 VARIABLE_LENGTH_MACRO(1,BUTTON_OUTLINE_MIDDLE_1);
	 string[i+1] = BUTTON_OUTLINE_RIGHT_ENDCAP;
	 XDrawText(info->dpy, win, 
		   info->gc[OLGX_BLACK], x, y, &item, 1);
       }
     }
     
     /*
      * Place the label, if specified.
      */
     if (label) {
       if (state & OLGX_LABEL_IS_PIXMAP) {
	 olgx_draw_pixmap_label(info, win,
				(Pixmap) label,
				x + info->endcap_width,
				y + 1, 
				inside_width,
				info->button_height - 2);
       }
       else {
	 olgx_draw_text(info, win, (char *) label,
			x+info->endcap_width,
			y+(info->button_height - info->base_off), 
			inside_width - 
			((state & OLGX_MENU_MARK) ? 
			 info->mm_width : 0),
			state);
       }
     }
     
     /*
      * Place the menu mark, if desired.
      */
     if (state & OLGX_MENU_MARK) {
       /* draw the menu mark. (fill_color != OLGX_BG2) causes
	  the menu mark to be filled in only when necessary */
       if (info->three_d)
	 olgx_draw_menu_mark(info,  win, 
			     x + (width - info->endcap_width - info->mm_width),
			     y + (info->button_height - info->mm_height - info->base_off),
			     state, (fill_color != OLGX_BG2));
       else
	 olgx_draw_menu_mark(info,  win, 
			     x + (width - info->endcap_width - info->mm_width),
			     y + (info->button_height - info->mm_height - info->base_off ),
			     state, 0);
     }
     
     /*
      * Mark the item as inactive, if specified
      */
     if (state & OLGX_INACTIVE) {
       olgx_stipple_rect(info, win, x, y, width, info->button_height);
     }
   }

void
  olgx_draw_menu_mark(info, win, x, y, state, fill_in)
Graphics_info *info;
Window win;
int state, fill_in;
   {
     char string[3];
     
       if ( state & OLGX_HORIZ_MENU_MARK)
	string[0] = HORIZ_MENU_MARK_UL;
	else if ( state & OLGX_HORIZ_BACK_MENU_MARK)
	string[0] = HORIZ_BACK_MENU_MARK_UL;
	else if ( state & OLGX_VERT_BACK_MENU_MARK)
	string[0] = VERT_BACK_MENU_MARK_UL;
	else
	  string[0] = VERT_MENU_MARK_UL;
       string[1] = string[0] + 1;
       if ((state & OLGX_INVOKED)&& (!info->three_d))
       XDrawString(info->dpy, win,info->gc[OLGX_WHITE], x, y, &string[0],info->three_d?1:2);
       else
       XDrawString(info->dpy, win,info->gc[OLGX_BLACK], x, y, &string[0],info->three_d?1:2);
      if (info->three_d) 
       XDrawString(info->dpy, win, info->gc[OLGX_WHITE], x, y, &string[1], 1);
     /* fill in the menu mark, if requested */
     if (fill_in) {
       string[0] = string[0] + 2; 
       XDrawString(info->dpy, win, info->gc[OLGX_BG2], x, y, &string[0], 1);
     }
   }

void
  olgx_draw_abbrev_button(info, win, x, y, state)
Graphics_info *info;
Window win;
int x,y;
int state;
   {
     XTextItem item;
     char string[3];
     int top_color, bottom_color, fill_color;
     
     item.nchars = 1;
     item.font = None;
     item.chars = string;
     item.delta = 0;
     if (!info->three_d){ /* 2d */
       if (state & OLGX_ERASE)
	 XFillRectangle(info->dpy,win,info->gc[OLGX_WHITE],x,y,
			Abbrev_MenuButton_Width(info),Abbrev_MenuButton_Width(info));
       if (state & OLGX_BUSY){
	 olgx_set_busy_stipple(info,OLGX_WHITE);
	 string[0] = ABBREV_MENU_FILL;
	 XDrawText(info->dpy, win, info->gc[OLGX_BUSYGC], x, y, &item, 1);
       }
       if (state & OLGX_INVOKED){ 
	 string[0] = OLG_ABBREV_MENU_BUTTON_INVERTED; 
	 XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y, &item, 1);
       }
       else{
	 string[0] = OLG_ABBREV_MENU_BUTTON; 
	 XDrawText(info->dpy, win, info->gc[OLGX_BLACK], x, y, &item, 1);
       }
       
     }
     else{ /* 3d */

       if (state & OLGX_INVOKED) {
	 top_color = OLGX_BG3;
	 bottom_color = OLGX_WHITE;
	 fill_color = OLGX_BG2;
       }
       else {
	 top_color = OLGX_WHITE;
	 bottom_color = OLGX_BG3;
	 fill_color = OLGX_BG1;
       }
       if (state & OLGX_BUSY){
	 olgx_set_busy_stipple(info, fill_color);	
	 fill_color = OLGX_BUSYGC;
       }
       
       string[0] = ABBREV_MENU_UL; 
       XDrawText(info->dpy, win, info->gc[top_color], x, y, &item, 1);
       
       string[0] = ABBREV_MENU_LR; 
       XDrawText(info->dpy, win, info->gc[bottom_color], x, y, &item, 1);
       
       string[0] = ABBREV_MENU_FILL;
       XDrawText(info->dpy, win, info->gc[fill_color], x, y, &item, 1);
     }
       olgx_draw_menu_mark(info, win, 
			   x + ((info->abbrev_width - info->mm_width) >> 1), 
			   y + ((1 + info->abbrev_height - info->mm_height) >> 1),
			   state,info->three_d?1:0);
     /* If it is inactive fill the rectangle with inactive pixmap */
     if (state & OLGX_INACTIVE)
	  {
	    olgx_stipple_rect(info, win, x, y,info->abbrev_width, info->abbrev_height);
	  }	       
   }

void
  olgx_set_busy_stipple(info, color)
Graphics_info *info;
int color;
   {	
     XGCValues gcval;
     
     if (!info->gc[OLGX_BUSYGC]) {
       /* stipple doesn't exist; create it */
       info->busy_stipple = XCreatePixmapFromBitmapData(
							info->dpy, RootWindow(info->dpy, info->scrn),
							busy_bits, busy_width, busy_height, 1, 0, 1);
       
       /* create the busy GC */
       gcval.fill_style = FillOpaqueStippled;
       gcval.stipple = info->busy_stipple;
       gcval.background = info->pixvals[OLGX_BLACK];
       gcval.font = info->glyphfont->fid;
       info->gc[OLGX_BUSYGC] = XCreateGC(
					 info->dpy, RootWindow(info->dpy, info->scrn),
					 /* don't set stipple unless pixmap create succeeded */
					 ((info->busy_stipple) ? GCStipple | GCFillStyle : 0)
					 | GCBackground | GCFont,
					 &gcval);
     }
     
     /* set the foreground color */
     gcval.foreground = info->pixvals[color];
     XChangeGC(info->dpy, info->gc[OLGX_BUSYGC], GCForeground, &gcval);
   }

void
  olgx_stipple_rect(info, win, x, y, width, height)
Graphics_info *info;
Window win;
int x, y, width, height;
   {	
     XGCValues gcval;
     
     if (!info->grey_stipple) {
       /* stipple doesn't exist; create it */
       info->grey_stipple = XCreatePixmapFromBitmapData(
							info->dpy, RootWindow(info->dpy, info->scrn),
							grey_bits, grey_width, grey_height, 1, 0, 1);
       if (!info->grey_stipple) {
	 /* error handling? */
	 return;
       }
       
       /* create the grey GC */
       /* Take care of 2d and 3d cases */
       if (info->three_d)
	 gcval.foreground = info->pixvals[OLGX_BG1];
       else{
	 gcval.foreground = info->pixvals[OLGX_WHITE];
	 height = height + 3 ;
       }
       
       gcval.fill_style = FillStippled;
       gcval.stipple = info->grey_stipple;
       info->gc[OLGX_GREY_OUT] = XCreateGC(
					   info->dpy, RootWindow(info->dpy, info->scrn),
					   GCStipple | GCFillStyle | GCForeground,
					   &gcval);
     }
     
     XFillRectangle(info->dpy, win, info->gc[OLGX_GREY_OUT],
		    x, y, width,(info->three_d)? height:height+3);
   }

void
  olgx_draw_text(info, win, string, x, y, max_width,state)
Graphics_info *info;
Window win;
char *string;
int x, y, max_width;
int state;
   {
     int len = strlen(string);
     register int i;
     
     /* 
      * if the string is too long, we'll have to truncate it 
      * max_width == 0 implies don't truncate.
      */
     if (max_width && XTextWidth(info->textfont, string, len) > max_width) {
       int current_width = 0;
       
       
       for (i = 0; (i < len && current_width <= max_width); i++) {
	 current_width += 
	   XTextWidth(info->textfont, &string[i], 1);
       }
       
       /* 
	* at this point, i-1 represents the number of chars of string
	* that will fit into max_width.
	*/
       len = i-1;
     }
     
     if ((state & OLGX_INVOKED) && !(info->three_d))
       XDrawString(info->dpy, win, info->gc[OLGX_TEXTGC_REV], x, y, string, len);
     else
       XDrawString(info->dpy, win, info->gc[OLGX_TEXTGC], x, y, string, len);
     
   }

void
  olgx_draw_pixmap_label(info, win, pix, x, y, width, height)
Graphics_info *info;
Window win;
Pixmap pix;
int x, y, width, height;
   {
     Window root;
     int x_ret, y_ret;		/* x and y return */
     unsigned int w_ret, h_ret, bw_ret, pix_depth;
     int actual_x, actual_y, actual_w, actual_h;
     
     /*
      * Get the geometry of the pixmap
      */
     XGetGeometry(info->dpy, pix, &root, &x_ret, &y_ret,
		  &w_ret, &h_ret, &bw_ret, &pix_depth);
     
     /* determine the x coordinate and width of object to be drawn */
     if (w_ret >= width) {
       /* pixmap width is larger than rect; we must truncate it */
       actual_x = x;
       actual_w = width;
     }
     else {
       /* pixmap width is smaller than rect; we must center it */
       actual_x = x + ((width - w_ret) >> 1);
       actual_w = w_ret;
     }
     
     /* determine the y coordinate and height of object to be drawn */
     if (h_ret >= height) {
       /* pixmap height is larger than rect; we must truncate it */
       actual_y = y;
       actual_h = height;
     }
     else {
       /* pixmap height is smaller than rect; we must center it */
       actual_y = y + ((height - h_ret) >> 1);
       actual_h = h_ret;
     }
     
     /* draw the pixmap */
     if (pix_depth == 1) {			/* pixmap has depth 1 */
       XCopyPlane(info->dpy, 
		  pix, 			/* src */
		  win,			/* dest */
		  info->gc[OLGX_TEXTGC],
		  0, 0,			/* src x,y */
		  actual_w, actual_h,
		  actual_x, actual_y,
		  (unsigned long) 1);	/* bit plane */
     }
     else if (pix_depth == info->depth) {
       XCopyArea(info->dpy, 
		 pix, 			/* src */
		 win,			/* dest */
		 info->gc[OLGX_TEXTGC],
		 0, 0,			/* src x,y */
		 actual_w, actual_h,
		 actual_x, actual_y);
     }
     else {
       /* can't handle arbitrary depth relationship */
     }
   }
