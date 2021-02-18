/*
 * graphics.c  --  xrobots 
 * 
 * Copyright 1989 Brian Warkentine
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the author's name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The author makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 * 
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE 
 * AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * The author's current employer has no connection with this software, as it
 * was written before being employed at Sun.  The following address is for 
 * contacting the author only and does not imply any responsibility on the 
 * behalf of Sun Microsystems.
 * 
 * Contact the author at:
 * 	Brian Warkentine  (brianw@Sun.COM)
 * 	Sun Microsystems
 * 	2550 Garcia Avenue
 * 	Mountain View, Ca 94043-1100
 *
 */

#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include "xrobots.h"
#include "bitmaps.h"
#include "icon.h"

static Pixmap playerP, dead_playerP, robotP, heapP, iconP;
static Cursor upC, up_rightC, rightC, down_rightC, downC, 
              down_leftC, leftC, up_leftC, stayC, cant_goC, thumbsC;

#define NUM_TMP_CURSOR_PIXMAPS 11
static Pixmap tmp_pixmap[NUM_TMP_CURSOR_PIXMAPS]; 

/*----------------------------------------------------------------------*/

void
init_pixmaps(top_shell)
  Widget top_shell;
{
/*
 * Let's make some pixmaps and some cursors.
 * And then let's set the iconpixmap.
 */
  Pixmap tmpP;
  XColor fgcolor,bgcolor;
  Arg arg;

  playerP = XCreateBitmapFromData(display,playfield, 
		Image_player_bits, Image_player_width, Image_player_height);

  dead_playerP = XCreateBitmapFromData(display,playfield, 
		Image_player_dead_bits, Image_player_dead_width, 
		Image_player_dead_height);

  iconP   = XCreateBitmapFromData(display,playfield,
		icon_bits, icon_width, icon_height);

  robotP  = XCreateBitmapFromData(display,playfield, 
		Image_robot_bits, Image_robot_width, Image_robot_height);

  heapP   = XCreateBitmapFromData(display,playfield, 
		Image_heap_bits, Image_heap_width, Image_heap_height);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fgcolor.pixel = fg;
  bgcolor.pixel = bg;
  fgcolor.flags =  DoRed | DoGreen | DoBlue;
  bgcolor.flags =  DoRed | DoGreen | DoBlue;
  XQueryColor(display,DefaultColormapOfScreen(XtScreen(playfield_widget)), &fgcolor);
  XQueryColor(display,DefaultColormapOfScreen(XtScreen(playfield_widget)), &bgcolor);


  tmp_pixmap[0] = 
  tmpP = XCreateBitmapFromData(display,playfield, up_bits, 
                                  up_width, up_height);
  upC  = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[1] = 
  tmpP = XCreateBitmapFromData(display,playfield, up_right_bits, 
                                  up_right_width, up_right_height);
  up_rightC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[2] = 
  tmpP = XCreateBitmapFromData(display,playfield, right_bits, 
                                  right_width, right_height);
  rightC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[3] = 
  tmpP = XCreateBitmapFromData(display,playfield, down_right_bits, 
                                  down_right_width, down_right_height);
  down_rightC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[4] = 
  tmpP = XCreateBitmapFromData(display,playfield, down_bits, 
                                  down_width, down_height);
  downC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[5] = 
  tmpP = XCreateBitmapFromData(display,playfield, down_left_bits, 
                                  down_left_width, down_left_height);
  down_leftC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[6] = 
  tmpP = XCreateBitmapFromData(display,playfield, left_bits, 
                                  left_width, left_height);
  leftC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[7] = 
  tmpP = XCreateBitmapFromData(display,playfield, up_left_bits, 
                                  up_left_width, up_left_height);
  up_leftC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[8] = 
  tmpP = XCreateBitmapFromData(display,playfield, stay_bits, 
                                  stay_width, stay_height);
  stayC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[9] = 
  tmpP = XCreateBitmapFromData(display,playfield, cant_go_bits, 
                                  cant_go_width, cant_go_height);
  cant_goC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,8,8);

  tmp_pixmap[10] = 
  tmpP = XCreateBitmapFromData(display,playfield, thumbs_down_bits, 
                                  thumbs_down_width, thumbs_down_height);
  thumbsC = XCreatePixmapCursor(display, tmpP, tmpP, &fgcolor,&bgcolor,
 				thumbs_down_x_hot,thumbs_down_y_hot);

  XtSetArg(arg,XtNiconPixmap,iconP);
  XtSetValues(top_shell,&arg,1);
}

/*----------------------------------------------------------------------*/

static int cant_move;  /* set as a side effect of checking possible moves */

void
auto_teleport()
{
  if(!cant_move)
	return;
  if(autoteleport && sonic_used) {
	teleport();
	return;
  }
  if(autoteleportalways)
	teleport();
}

static void
display_ok_move(x,y)
  int x,y;
{

  if(can_go(x,y)) {
    cant_move = 0;
    if(!spiffy) return;
			/* show the icon for a good move */
    XDrawPoint(display, playfield, gc, 
               pos_to_coord(x)+CELLSIZE/2,pos_to_coord(y)+CELLSIZE/2);
  } else {
    if(!spiffy) return;
			/* or erase any previous dross */
    if( INXRANGE(x) && INYRANGE(y) && (robot_array[x][y] == EMPTY))
      XClearArea(display, playfield, 
                 pos_to_coord(x), pos_to_coord(y),
                 CELLSIZE, CELLSIZE, False);
  }
}

void
display_possible_moves()
{
  cant_move = 1;

  display_ok_move( human_x-1,human_y-1 );
  display_ok_move( human_x  ,human_y-1 );
  display_ok_move( human_x+1,human_y-1 );
  display_ok_move( human_x-1,human_y   );

  display_ok_move( human_x+1,human_y   );
  display_ok_move( human_x-1,human_y+1 );
  display_ok_move( human_x  ,human_y+1 );
  display_ok_move( human_x+1,human_y+1 );

}


void
display_level()
{
/* Naive refresh algorithm.... */
  int x,y;

  XClearWindow(display,playfield);
  if(game_active)
    XCopyPlane(display,playerP,playfield,gc,0,0,
 	Image_player_width,Image_player_height,
 	pos_to_coord(human_x),pos_to_coord(human_y),1);
  else
    XCopyPlane(display,dead_playerP,playfield,gc,0,0,
 	Image_player_dead_width,Image_player_dead_height,
 	pos_to_coord(human_x),pos_to_coord(human_y),1);

  for(y=0;y<MAXY;y++)
  {
    for(x=0;x<MAXX;x++)
        switch (robot_array[x][y])
        {
          case ROBOT:
            XCopyPlane(display,robotP,playfield,gc,0,0,
                       Image_robot_width,Image_robot_height,
                       pos_to_coord(x),pos_to_coord(y),1);
            break;

          case HEAP:  
            XCopyPlane(display,heapP,playfield,gc,0,0,
                       Image_heap_width,Image_heap_height,
                       pos_to_coord(x),pos_to_coord(y),1);
            break;
    }
  }
}


void
show_movement()
{
/* 
 * draw just the changes.
 */
  int x,y;

  /* force a redraw of the area surrounding last position */
  for(x=last_human_x-1;x<last_human_x+2;x++)
    for(y=last_human_y-1;y<last_human_y+2;y++)
      if( INXRANGE(x) && INYRANGE(y) )
        robot_array_bak[x][y] = REDRAW;  

  /* 
   * This is a little hacked up because the player isn't really in the 
   * robot array.  Assigning the backup array to EMPTY avoids
   * drawing over the player.
   */
  robot_array_bak[human_x][human_y] = EMPTY;   

  XCopyPlane(display,playerP,playfield,gc,0,0,
        Image_player_width,Image_player_height,
        pos_to_coord(human_x),pos_to_coord(human_y),1);

  for(y=0;y<MAXY;y++)
  {
    for(x=0;x<MAXX;x++)
      if(robot_array[x][y] != robot_array_bak[x][y])
        switch (robot_array[x][y])
        {
          case ROBOT:
            XCopyPlane(display,robotP,playfield,gc,0,0,
                       Image_robot_width,Image_robot_height,
                       pos_to_coord(x),pos_to_coord(y),1);
            break;

          case HEAP:
            XCopyPlane(display,heapP,playfield,gc,0,0,
                       Image_heap_width,Image_heap_height,
                       pos_to_coord(x),pos_to_coord(y),1);
            break;
          default:
            XClearArea(display, playfield, 
                       pos_to_coord(x), pos_to_coord(y),
                       CELLSIZE, CELLSIZE, False);
    }
  }
}



/*ARGSUSED*/
XtEventHandler
redisplay_level(w, closure, event)
  Widget w;
  caddr_t closure;
  XExposeEvent *event;
{
/* refresh entire screen -- e.g. expose events.  Just a little naive. */
  display_level();
  display_possible_moves();
  XFlush(display);
}


void
update_pointer(direction)
  int direction;
{
/*
 * set the cursor to the appropriate one.
 */
  int tmp_human_x = human_x, tmp_human_y = human_y;

  if(direction & UP)    tmp_human_y--;
  if(direction & DOWN)  tmp_human_y++;
  if(direction & LEFT)  tmp_human_x--;
  if(direction & RIGHT) tmp_human_x++;

  if(!can_go(tmp_human_x,tmp_human_y)) {
    XDefineCursor(display,playfield,cant_goC);
    return;
    }
  if(direction&LEFT)
    if(direction&UP)
      XDefineCursor(display,playfield,up_leftC);
    else 
      if(direction&DOWN)
          XDefineCursor(display,playfield,down_leftC);
      else
        XDefineCursor(display,playfield,leftC);

   if(direction&RIGHT)
    if(direction&UP)
      XDefineCursor(display,playfield,up_rightC);
    else 
      if(direction&DOWN)
          XDefineCursor(display,playfield,down_rightC);
      else
        XDefineCursor(display,playfield,rightC);

  if(direction == UP)
        XDefineCursor(display,playfield,upC);
  if(direction == DOWN)
        XDefineCursor(display,playfield,downC);
  if(direction == STILL)
        XDefineCursor(display,playfield,stayC);
    

}


/*----------------------------------------------------------------------*/

void free_pixmaps()
{
  int i;

  XFreeCursor(display,upC);
  XFreeCursor(display,up_rightC);
  XFreeCursor(display,rightC);
  XFreeCursor(display,down_rightC);
  XFreeCursor(display,downC);
  XFreeCursor(display,down_leftC);
  XFreeCursor(display,leftC); 
  XFreeCursor(display,up_leftC); 
  XFreeCursor(display,stayC);
  XFreeCursor(display,thumbsC);
  XFreeCursor(display,cant_goC);
  for(i=0;i++;i<NUM_TMP_CURSOR_PIXMAPS) 
    XFreePixmap(display,tmp_pixmap[i]);

  XFreePixmap(display,playerP);
  XFreePixmap(display,dead_playerP);
  XFreePixmap(display,robotP);
  XFreePixmap(display,iconP);
  XFreePixmap(display,heapP);

}



/*----------------------------------------------------------------------*/

void
do_death()
{
  XDefineCursor(display,playfield,thumbsC);
  XCopyPlane(display,dead_playerP,playfield,gc,0,0,
 	Image_player_dead_width,Image_player_dead_height,
 	pos_to_coord(human_x),pos_to_coord(human_y),1);
  XFlush(display);
}

/*----------------------------------------------------------------------*/

void
show_teleport()
{
/* 
 * Cheap animation... but it needs a fast server
 */
unsigned int i;

  if(!spiffy) return;

  for(i=100;i>0;i-=10) {
    XDrawArc(display,playfield,gc,
      pos_to_coord(human_x)+(CELLSIZE/2)-(i/2)-2,
      pos_to_coord(human_y)+(CELLSIZE/2)-(i/2)-2,
      i,i,0,64*360);
    XFlush(display);
  }
}

/*----------------------------------------------------------------------*/

static void
do_sonic(agc)
GC agc;
{
/* 
 * do some effects for the sonic screwdriver...
 */
#define SPACING 3
int center_x = pos_to_coord(human_x)+(CELLSIZE/2)-2,
    center_y = pos_to_coord(human_y)+(CELLSIZE/2)-2;
int i;

  if(!spiffy) return;

  for(i=pos_to_coord(human_x-1);i<pos_to_coord(human_x+2);i+=SPACING) {
    XDrawLine(display,playfield,agc,center_x,center_y,i,pos_to_coord(human_y-1));
    XCopyPlane(display,playerP,playfield,gc,0,0,
 	Image_player_width,Image_player_height,
 	pos_to_coord(human_x),pos_to_coord(human_y),1);
  }

  for(i=pos_to_coord(human_y-1);i<pos_to_coord(human_y+2);i+=SPACING) {
    XDrawLine(display,playfield,agc,center_x,center_y,pos_to_coord(human_x+2),i);
    XCopyPlane(display,playerP,playfield,gc,0,0,
 	Image_player_width,Image_player_height,
 	pos_to_coord(human_x),pos_to_coord(human_y),1);
  }

  for(i=pos_to_coord(human_x+2);i>pos_to_coord(human_x-1);i-=SPACING) {
    XDrawLine(display,playfield,agc,center_x,center_y,i,pos_to_coord(human_y+2));
    XCopyPlane(display,playerP,playfield,gc,0,0,
 	Image_player_width,Image_player_height,
 	pos_to_coord(human_x),pos_to_coord(human_y),1);
  }

  for(i=pos_to_coord(human_y+2);i>pos_to_coord(human_y-1);i-=SPACING) {
    XDrawLine(display,playfield,agc,center_x,center_y,pos_to_coord(human_x-1),i);
    XCopyPlane(display,playerP,playfield,gc,0,0,
 	Image_player_width,Image_player_height,
 	pos_to_coord(human_x),pos_to_coord(human_y),1);
  }
  XFlush(display);
}


void
show_sonic()
{
  do_sonic(gc);		/* do it, */
  do_sonic(cleargc);	/* then erase it. */
}

