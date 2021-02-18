/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <class.h>
#include <andrewos.h>
#include <chlist.eh>

boolean chlist__InitializeObject(c, self)
struct classheader *c;
struct chlist *self;
{
    self->ItemList = NULL;
    self->numitems = 0;
    self->numitemsallocated = 0;
    chlist_SetReadOnly(self, TRUE);
    return(TRUE);
}

boolean chlist__AddItemToEnd(self, str, proc, data)
struct chlist *self;
char *str;
int (*proc)();
long data;
{
    char *mycopy;
    struct listitem *newbuf;
    int pos, len;

    if (self->numitems >= self->numitemsallocated) {
	self->numitemsallocated += 25;
	newbuf = (struct listitem *) malloc(self->numitemsallocated * sizeof(struct listitem));
	if (newbuf == NULL) {
	    return(FALSE);
	}
	if (self->ItemList != NULL) {
	    bcopy(self->ItemList, newbuf, (self->numitemsallocated - 25) * sizeof(struct listitem));
	    free(self->ItemList);
	}
	self->ItemList = newbuf;
    }
    mycopy = malloc(1+strlen(str));
    if (!mycopy) {
	--self->numitems;
	return(FALSE);
    }
    strcpy(mycopy, str);
    pos = chlist_GetLength(self);
    len = strlen(str);
    chlist_AlwaysInsertCharacters(self, pos, str, len);
    chlist_AlwaysInsertCharacters(self, pos+len, "\n", 1);
    self->ItemList[self->numitems].str = mycopy;
    self->ItemList[self->numitems].proc = proc;
    self->ItemList[self->numitems].rock = data;
    self->ItemList[self->numitems].loc = pos;
    ++self->numitems;
    return(TRUE);
}

struct listitem * chlist__FindItem(self, str)
struct chlist *self;
char *str;
{
    int i;
    i = FindItemIndex(self,str);
    if (i>= 0) return(&self->ItemList[i]);
    return(NULL);
}

static int FindItemIndex(self, str)
struct chlist *self;
char *str;
{
    int i;
    for (i=0; i<self->numitems; ++i) {
	if (!strcmp(self->ItemList[i].str, str)) {
	    return(i);
	}
    }
    return(-1);
}

boolean chlist__DeleteItem(self, str)
struct chlist *self;
char *str;
{
    int i = FindItemIndex(self, str);
    int len;

    if (i<0) return(FALSE);
    if ((i+1) < self->numitems) {
	len = self->ItemList[i+1].loc - self->ItemList[i].loc;
    } else {
	len = chlist_GetLength(self) - self->ItemList[i].loc;
    }
    chlist_AlwaysDeleteCharacters(self, self->ItemList[i].loc, len);
    free(self->ItemList[i].str);
    self->numitems --;
    while (i<self->numitems) {
	self->ItemList[i].str = self->ItemList[i+1].str;
	self->ItemList[i].loc = self->ItemList[i+1].loc - len;
	self->ItemList[i].proc = self->ItemList[i+1].proc;
	self->ItemList[i].rock = self->ItemList[i+1].rock;
	++i;
    }
    chlist_NotifyObservers(self, 0);
    return(TRUE);
}

boolean chlist__ChangeItem(self, oldstr, newstr)
struct chlist *self;
char *oldstr, *newstr;
{
    int delta_len;
    int i = FindItemIndex(self, oldstr);
    int newlen, oldlen, pos;

    if (i<0) return(FALSE);
    newlen = strlen(newstr);
    oldlen = strlen(oldstr);
    pos = self->ItemList[i].loc;

    chlist_AlwaysReplaceCharacters(self, pos, oldlen, newstr, newlen);

    self->ItemList[i].str = realloc(self->ItemList[i].str, 1+strlen(newstr));
    if (!self->ItemList[i].str) return(FALSE);
    strcpy(self->ItemList[i].str, newstr);

    delta_len = newlen - oldlen;
    while ((++i)<self->numitems) {	/* adjust other entries' position */
	self->ItemList[i].loc += delta_len;
    }
    chlist_NotifyObservers(self, 0);

    return(TRUE);
}
