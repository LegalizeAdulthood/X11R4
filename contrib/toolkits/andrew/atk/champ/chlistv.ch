/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
class chlistview[chlistv]:textview[textv] {
    classprocedures:
      InitializeObject(struct chlistview *self) returns boolean;
    overrides:
      Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    methods:
      ActivateItem(int pos);
};
