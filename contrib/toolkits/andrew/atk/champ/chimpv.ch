/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
class chimpview[chimpv]:lpair {
    classprocedures:
      InitializeObject(struct chimpview *self) returns boolean;
    overrides:
      SetDataObject(struct view *v);
    data:
      struct chlistview *lv;
      struct enodeview *env;
};
