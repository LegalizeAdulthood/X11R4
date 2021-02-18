/* 
 * @(#) ol_sb.c 1.10 89/11/20 
 */

/*
 * OPEN LOOK object drawing package
 *
 * ol_sb.c
 * Scrollbar Module
 *
 * Sun Microsystems, Inc.,
 */

#include <stdio.h>
#include <X11/Xlib.h>

#include <olgx_private/olgx_impl.h>

olgx_draw_elevator(info, win, x, y, oldx,oldy,state)
     Graphics_info *info;
     Window win;
     int state;
     int oldx,oldy;
   {
     char string[1];
     int initial;
     int newy = 0;
     int newx = 0;
     short sbheight;


       if (state & OLGX_ABBREV) 
	sbheight =info->sb_height-((info->sb_height-2)/3);
	else
	sbheight = info->sb_height;

     if ((state & OLGX_ERASE))
       XFillRectangle(info->dpy,win,info->three_d?info->gc[OLGX_BG1]:info->gc[OLGX_WHITE],x,y,
		  (state & OLGX_VERTICAL)?info->sb_width:sbheight,
		      (state & OLGX_VERTICAL)?sbheight:info->sb_width);
     if (state & OLGX_UPDATE)
       XFillRectangle(info->dpy,win,(info->three_d)?info->gc[OLGX_BG1]:info->gc[OLGX_WHITE],oldx,oldy,
			(state & OLGX_VERTICAL)?info->sb_width:
			sbheight,(state & OLGX_VERTICAL)?sbheight:
			info->sb_width);

     /* Determine which scrollbar glyphs to use. */
     
     if (info->three_d){ /* 3d begins*/
       
       if (state & OLGX_ABBREV) 
	 
	 initial = (state & OLGX_HORIZONTAL)?HORIZ_ABBREV_SB_UL:VERT_ABBREV_SB_UL;
       else 
	 initial = (state & OLGX_HORIZONTAL)?HORIZ_SB_UL:VERT_SB_UL;
       
       string[0] = initial;
       XDrawString(info->dpy, win, info->gc[OLGX_WHITE], x, y, string, 1);
       string[0] = initial + 1;
       XDrawString(info->dpy, win, info->gc[OLGX_BG3], x, y, string, 1);

	 if (state & OLGX_SCROLL_BACKWARD){
	 newx = x;
	 newy = y;
	 string[0] = (state & OLGX_VERTICAL)?VERT_SB_TOPBOX_FILL
			: HORIZ_SB_LEFTBOX_FILL;
	 XDrawString(info->dpy, win, info->gc[OLGX_BG2], newx, y, string, 1);
	 }
	 else if (state & OLGX_SCROLL_FORWARD){
	   newy = (state & OLGX_HORIZONTAL)?y: y+(((ScrollbarElevator_Height(info) -2)/3) << 1);
	 newx = (state & OLGX_VERTICAL)?x:x+(((ScrollbarElevator_Height(info) -2)/3) << 1);
	 if (state  & OLGX_ABBREV)
	 {
	  if (state & OLGX_VERTICAL)
	 newy = y + ((ScrollbarElevator_Height(info)-2)/3) ;
	  else if (state & OLGX_HORIZONTAL)
	 newx = x + ((ScrollbarElevator_Height(info)-2)/3) ;
	 }
	 string[0] = (state & OLGX_VERTICAL)?VERT_SB_BOTBOX_FILL:
			HORIZ_SB_RIGHTBOX_FILL;
	 XDrawString(info->dpy, win, info->gc[OLGX_BG2], newx, newy, string, 1);
	 }
	else  if (state & OLGX_SCROLL_ABSOLUTE){
	 newx = (state & OLGX_VERTICAL)?x: x+((ScrollbarElevator_Height(info) -2)/3)+1 ;
	 newy = (state & OLGX_HORIZONTAL)?y-1: y+((ScrollbarElevator_Height(info) -2)/3) ;
	     string[0] = DIMPLE_UL;
	     XDrawString(info->dpy, win, info->gc[OLGX_BLACK], newx, newy, string, 1);
	     string[0] = DIMPLE_LR;
	     XDrawString(info->dpy, win, info->gc[OLGX_WHITE], newx, newy, string, 1);
	     string[0] = DIMPLE_FILL;
	     XDrawString(info->dpy, win, info->gc[OLGX_BG2], newx, newy, string, 1);
	     newx = 0;newy = 0;
	   }
       if ((newy) || (newx)){
	 string[0] = (state & OLGX_VERTICAL)?VERT_SB_BOX_UL:HORIZ_SB_BOX_UL;
	 XDrawString(info->dpy, win, info->gc[OLGX_BG3], newx, newy, string, 1);
	 string[0] = (state & OLGX_VERTICAL)?VERT_SB_BOX_LR:HORIZ_SB_BOX_LR;
	 XDrawString(info->dpy, win, info->gc[OLGX_WHITE], newx, newy, string, 1);
       }
	} /* 3d ends */
	else{
	/* 2d begins */

	if (state & OLGX_ABBREV) { /* abbrev scroll bar */
	   if (state & OLGX_SCROLL_BACKWARD)
	      initial = (state & OLGX_HORIZONTAL)?
			 OLG_HSB_REDUCED_ELEVATOR_LINE_BACKWARD:
	                 OLG_VSB_REDUCED_ELEVATOR_LINE_BACKWARD;
	   else  if (state & OLGX_SCROLL_FORWARD)
	      initial = (state & OLGX_HORIZONTAL)?
			 OLG_HSB_REDUCED_ELEVATOR_LINE_FORWARD:
	                 OLG_VSB_REDUCED_ELEVATOR_LINE_FORWARD;
           else  /* default - normal -uninvoked scrollbar */
	      initial = (state & OLGX_HORIZONTAL)?
			 OLG_HSB_REDUCED_ELEVATOR:
	                 OLG_VSB_REDUCED_ELEVATOR;
          }
	  else { /* normal scrollbar */
	   if (state & OLGX_SCROLL_BACKWARD)
	      initial = (state & OLGX_HORIZONTAL)?
			 OLG_HSB_ELEVATOR_LINE_BACKWARD:
	                 OLG_VSB_ELEVATOR_LINE_BACKWARD;
	   else if (state & OLGX_SCROLL_NO_BACKWARD)
	     initial = (state & OLGX_HORIZONTAL)?
	                HORIZ_SB_NO_BACK_OUTLINE:
	                VERT_SB_NO_BACK_OUTLINE;
	   else if (state & OLGX_SCROLL_NO_FORWARD)
	     initial = (state & OLGX_HORIZONTAL)?
	                HORIZ_SB_NO_FWD_OUTLINE:
	                VERT_SB_NO_FWD_OUTLINE;
	   else if (state & OLGX_INACTIVE)
	     initial = (state & OLGX_HORIZONTAL)?
	                HORIZ_SB_INACTIVE_OUTLINE:
	                VERT_SB_INACTIVE_OUTLINE;
	   else  if (state & OLGX_SCROLL_FORWARD)
	      initial = (state & OLGX_HORIZONTAL)?
			 OLG_HSB_ELEVATOR_LINE_FORWARD:
	                 OLG_VSB_ELEVATOR_LINE_FORWARD;
           else if (state & OLGX_SCROLL_ABSOLUTE)
	      initial = (state & OLGX_HORIZONTAL)?
			 OLG_HSB_ELEVATOR_ABSOLUTE:
	                 OLG_VSB_ELEVATOR_ABSOLUTE;
           else  /* default - normal -uninvoked scrollbar */
	      initial = (state & OLGX_HORIZONTAL)?
			 OLG_HSB_ELEVATOR:
	                 OLG_VSB_ELEVATOR;


	  }
	string[0] = initial ;
	XDrawString(info->dpy, win, info->gc[OLGX_BLACK], x, y, string, 1);

     }	     
}
