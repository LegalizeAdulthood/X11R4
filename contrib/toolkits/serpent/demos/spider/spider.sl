
/*--------------------------------------------------------------------------*\
|
|  Name:  Spider Chart Dialog
|
|  Description:
|     Manages the dialog and display for the spider chart demo.  This dialog
|     is associated with the Sensor Site Status application.
|
| $Log:	spider.sl,v $
% Revision 1.1  89/10/10  23:43:54  serpent
% Initial revision
% 
Revision 1.3  89/09/01  15:10:40  ljb
make work under saw

Revision 1.2  89/08/18  08:56:29  trm
no changes. just checkin' checkin.   trm.

Revision 1.1  89/08/01  14:58:00  serpent
initial_checkin

| Revision 1.5  89/03/27  16:22:12  bmc
| - VC "dialog" is now a sub-VC to "sensor".
| 
| Revision 1.4  89/03/24  13:31:11  bmc
| Most calls to get_component_value and put_component_value replaced
| by direct access to variables.  Removed all calls to get_local.
| Removed all uses of and definitions for get_grandparent_vc and
| company.
| 
| Revision 1.3  89/03/22  16:22:25  bmc
| - Correlation centers are now part of the main VC, and come into being
|   immediately.  They are no longer tied to an spplication shared data
|   element.
| - All constants are now upper case.  (Consistent with C)
| - Various cosmetic changes made.
| 
\*---------------------- Copyright 1987 CMU --------------------------------*/

#include "spider.ill"
#include "saw.ill"
|||
/* definitions shared with application */
#include "shared.h"

#include "defines.sl"
#include "nsl.sl"

  /*
  ** The top level VC holds variables and owns the main display
  ** window.  It also contains constant display items, such as the correlation
  ** centers, which never change.
  */


  VARIABLES:

    sensor_number:  0;				/* # assigned to next sensor */

  OBJECTS:

    the_window:  form_widget			/* the parent of 'em all */

    {

    ATTRIBUTES:

      width:		   XMAX;
      height:		   YMAX;

      background_color:    "black";
    }


    cmc_cc:  label_widget			/* CMC correlation center */

    {

    ATTRIBUTES:

      /* core stuff */

      parent:		    the_window;

      width:		    CC_WIDTH;
      height:		    CC_HEIGHT;

      background_color:	    "green";
      foreground_color:	    "black";

      vert_distance:	    CC_YLOC;
      horiz_distance:	    LEFT_CC_SENSOR_HOOK;

      label_text:	    "CMC";
      justify:		    CENTER;
      internal_width:	    5;
      internal_height:	    5;

    }

    oft_cc:  label_widget			/* OFT correlation center */

    {

    ATTRIBUTES:

      parent:		    the_window;

      width:		    CC_WIDTH;
      height:		    CC_HEIGHT;

      background_color:	    "green";
      foreground_color:	    "black";

      vert_distance:	    CC_YLOC;
      horiz_distance:	    SIDE_TO_MIDDLE + CC_LINE_OFFSET;

      label_text:	    "OFT";
      justify:		    CENTER;
      internal_width:	    5;
      internal_height:	    5;

    }


    non_selectable_line(cc_to_cc_line,
                        the_window,
                        CC_FROM_X,
                        TOP_TO_MID_CC,
                        CC_TO_X-CC_FROM_X,
                        "x",
                        "green")

    non_selectable_line(cc_left_line,
                        the_window,
                        LEFT_CC_SENSOR_HOOK - CC_LINE_STUB,
                        TOP_TO_MID_CC,
                        CC_LINE_STUB,
                        "x",
                        "green")

    non_selectable_line(cc_right_line,
                        the_window,
                        RIGHT_CC_SENSOR_HOOK,
                        TOP_TO_MID_CC,
                        CC_LINE_STUB,
                        "x",
                        "green")

    non_selectable_line(cc_left_vert,
                        the_window,
                        LEFT_CC_SENSOR_HOOK - CC_LINE_STUB,
                        SENSOR_TO_TOP,
                        YMAX - SENSOR_TO_TOP - SENSOR_TO_BOTTOM,
                        "y",
                        "green")

    non_selectable_line(cc_right_vert,
                        the_window,
                        RIGHT_CC_SENSOR_HOOK + CC_LINE_STUB,
                        SENSOR_TO_TOP,
                        YMAX - SENSOR_TO_TOP - SENSOR_TO_BOTTOM,
                        "y",
                        "green")

    
#include "sensor.sl"

#include "lines.sl"
