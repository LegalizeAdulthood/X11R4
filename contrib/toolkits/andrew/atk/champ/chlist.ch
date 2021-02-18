/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
struct listitem {
    char *str;
    int loc;
    int (*proc)();
    long rock;
};

class chlist:text {
    classprocedures:
      InitializeObject(struct chlist *self) returns boolean;
    methods:
      AddItemToEnd(char *str, procedure proc, long data) returns boolean;
      DeleteItem(char *str) returns boolean;
      FindItem(char *str) returns struct listitem *;
      ChangeItem(char *oldstr, char *newstr) returns boolean;
    macromethods:
      GetItemList() (self->ItemList)
      GetNumItems() (self->numitems)
    data:
      struct listitem *ItemList;
      int numitems, numitemsallocated;
};
