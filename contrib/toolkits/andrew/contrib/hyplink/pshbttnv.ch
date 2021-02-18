/* ********************************************************************** *\
 *         Copyright IBM Corporation 1989 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
class pushbuttonview[pshbttnv]: view {
 classprocedures:
  InitializeClass() returns boolean;
  InitializeObject(struct pushbuttonview *self) returns boolean;
  FinalizeObject(struct pushbuttonview *self);
 overrides:	
  DesiredSize(long width, long height, enum view_DSpass pass, long * desired_width, long * desired_height) returns enum view_DSattributes;
  GetOrigin(long width, long height, long * originX, long * originY);
  FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
  Update();
  Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
 data:
  short lit;
  struct cursor *cursor;
};

