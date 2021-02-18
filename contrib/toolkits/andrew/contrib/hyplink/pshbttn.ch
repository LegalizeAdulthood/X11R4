/* ********************************************************************** *\
 *         Copyright IBM Corporation 1989 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
class pushbutton[pshbttn]: dataobject[dataobj] {
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject(struct pushbutton *self) returns boolean;
      FinalizeObject(struct pushbutton *self);
    overrides:
      Read (FILE *fp, long id) returns long;
      Write (FILE *fp, long id, int level) returns long;
    methods:
      SetText(char *txt);
      SetStyle(int stylecode);
      SetButtonFont(struct fontdesc *f);
    macromethods:
      GetText() (self->text)
      GetStyle() (self->style)
      GetButtonFont() (self->myfontdesc)
    data:
      char *text;
      int style;
      struct fontdesc *myfontdesc;
};

