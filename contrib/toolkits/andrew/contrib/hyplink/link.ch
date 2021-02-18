/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
class link: pushbutton[pshbttn] {
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject(struct link *self) returns boolean;
      FinalizeObject(struct link *self);
    overrides:
      Read (FILE *file, long id) returns long;
      Write (FILE *file, long writeid, int level) returns long;
      SetText(char *txt);
    methods:
      SetLink(char *link);
    macromethods:
      GetLink() (self->link)
      LabelSetP() (self->label_set)
    data:
      char *link;
      int label_set;
};

