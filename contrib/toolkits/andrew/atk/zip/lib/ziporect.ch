/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/ziporect.ch,v 2.6 89/02/25 12:53:18 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/ziporect.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/ziporect.ch,v 2.6 89/02/25 12:53:18 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/*
ziporect.H

  03/31/88	Create for ATK (TCP)
*/

#define  ziporect_VERSION    1

#define  Data				(self->header.zipobject.data_object)
#define  Env				(Data->env)
#define  View				(self->header.zipobject.view_object)
#define  Edit				(self->header.zipobject.edit_object)
#define  Print				(self->header.zipobject.print_object)


class ziporect : zipobject[zipobj]
  {

overrides:

  Object_Icon()						returns char;
  Object_Icon_Cursor()					returns char;
  Object_Datastream_Code()				returns char;

  Build_Object( pane, action, x, y, clicks, X, Y )	returns long;
  Show_Object_Properties( pane, figure )		returns long;
  Draw_Object( object, pane )				returns long;
  Clear_Object( object, pane )				returns long;
  Print_Object( object, pane )				returns long;
  Proximate_Object_Points( object, pane, x, y )		returns long;
  Within_Object( object, pane, x, y )			returns long;
  Enclosed_Object( object, pane, x, y, w, h )		returns boolean;
  Object_Enclosure( object, pane, *x, *y, *w, *h )	returns long;
  Highlight_Object_Points( object, pane )		returns long;
  Normalize_Object_Points( object, pane )		returns long;
  Expose_Object_Points( object, pane )			returns long;
  Hide_Object_Points( object, pane )			returns long;
  Set_Object_Point( object, point, x, y )		returns long;
  Adjust_Object_Point_Suite( object, x_delta, y_delta )	returns long;
  Set_Object_Shade( object, shade )			returns long;

  };

