/* ********************************************************************** *\
 *         Copyright IBM Corporation 1989 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <stdio.h>
#include <sys/param.h>
#include <pshbttn.eh>
#include <andrewos.h>
#include <atom.ih>
#include <cursor.ih>
#include <environ.ih>
#include <fontdesc.ih>
#include <graphic.ih>
#include <observe.ih>

/* Defined constants and macros */
#define MAX_LINE_LENGTH 70  /* can't be less than 6 */
#define DS_VERSION 1 /* datastream version */

/* External declarations */

/* Forward Declarations */
static void WriteLine();
static char *GlomStrings(), *ReadLine(), *EncodeFont();

/* Global variables */


boolean
pushbutton__InitializeClass(c)
struct classheader *c;
{
/* 
  Initialize all the class data.
*/
  return(TRUE);
}


boolean
pushbutton__InitializeObject(c, self)
struct classheader *c;
struct pushbutton *self;
{
/*
  Inititialize the object instance data.
*/

    self->text = NULL;
    self->style = environ_GetProfileInt("pushbuttonstyle", 2);
    self->myfontdesc = NULL;
    return(TRUE);
}


void
pushbutton__FinalizeObject(c, self)
struct classheader *c;
struct pushbutton *self;
{
/*
  Finalize the object instance data.
*/
  if (self->text) {
    free(self->text);
    self->text = NULL;
  }
  return;
}


static void
pushbutton__WriteDataPart(self, fp)
struct pushbutton *self;
FILE *fp;
{
/*
  Write the object data out onto the datastream.
*/
  char buf[100], *encfont;

  WriteLine(fp, self->text ? self->text : "");
  sprintf(buf,"%d", self->style); /* *BUG* how do we tell a defaulted 
				     style from a set style? */
  WriteLine(fp, buf);
  encfont = self->myfontdesc ? EncodeFont(self) : NULL;
  WriteLine(fp, encfont ? encfont : "");
  if (encfont) {
    free(encfont);
    encfont = NULL;
  }
}



long
pushbutton__Write(self, fp, id, level)
struct pushbutton *self;
FILE *fp;
long id;
int level;
{
/*
  Write the object data out onto the datastream.

  Sample output from datastream version 1:
    \begindata{pushbutton, 1234567}
    Datastream version: 1
    This is my button label
    2
    fontname
    \enddata{pushbutton, 1234567}

*/

  long uniqueid = pushbutton_UniqueID(self);

  if (id != pushbutton_GetWriteID(self)) {
    /* New Write Operation */
    pushbutton_SetWriteID(self, id);
    fprintf(fp, "\\begindata{%s,%d}\nDatastream version: %d\n",
	    class_GetTypeName(self), uniqueid, DS_VERSION);

    pushbutton__WriteDataPart(self, fp);

    fprintf(fp, "\\enddata{%s,%d}\n", class_GetTypeName(self), uniqueid);
  }
  return(uniqueid);
}


static long
pushbutton__ReadDataPart(self, fp)
struct pushbutton *self;
FILE *fp;
{
/*
  Read in the object from the file.
*/
  char *buf;
  
  if ((buf = ReadLine(fp)) == NULL)
    return(dataobject_PREMATUREEOF);
  if (strcmp(buf,"")!= 0 )
    pushbutton_SetText(self, buf);
  free(buf);

  if ((buf = ReadLine(fp)) == NULL)
    return(dataobject_PREMATUREEOF);
  if (strcmp(buf,"")!= 0 )
    pushbutton_SetStyle(self, atoi(buf));
  free(buf);

  if ((buf = ReadLine(fp)) == NULL)
    return(dataobject_PREMATUREEOF);
  if (strcmp(buf,"")!= 0) {
    char name[MAXPATHLEN];
    long style, size;
    if (fontdesc_ExplodeFontName(buf,name,sizeof(name), &style, &size)) {
      pushbutton_SetButtonFont(self,fontdesc_Create(name,style,size));
    }
  }
  free(buf);

  return(dataobject_NOREADERROR);
}



long
pushbutton__Read(self, fp, id)
struct pushbutton *self;
FILE *fp;
long id;
{
/*
  Read in the object from the file.
  (BUG) Doesn't set the font.
*/
  char *buf, buf2[255];
  long result;
  
  pushbutton_SetID(self, pushbutton_UniqueID(self));

  if ((buf = ReadLine(fp)) == NULL)
    return(dataobject_PREMATUREEOF);
  if (strncmp(buf,"Datastream version:",19))
    return(dataobject_BADFORMAT);
  if (atoi(buf+19) != DS_VERSION)	/* datastream version */
    return(dataobject_BADFORMAT);
  free(buf);

  if ((result = pushbutton__ReadDataPart(self, fp)) != dataobject_NOREADERROR)
    return(result);

  if ((buf = ReadLine(fp)) == NULL)
    return(dataobject_PREMATUREEOF);
  sprintf(buf2, "\\enddata{%s,%ld}", class_GetTypeName(self), id);
  if (strcmp(buf, buf2)) {
    free(buf);
    return(dataobject_MISSINGENDDATAMARKER);
  }
  free(buf);

  return(dataobject_NOREADERROR);
}



void
pushbutton__SetText(self, txt)
struct pushbutton *self;
char *txt;
{
/*
  Set the text label for this object.
*/

    if (self->text) {
      free(self->text);
      self->text = NULL;
    }
    if (txt)
      if (self->text = (char *)malloc(1+strlen(txt)))
	strcpy(self->text, txt);
    pushbutton_NotifyObservers(self, observable_OBJECTCHANGED);
}


void
pushbutton__SetStyle(self, stylecode)
struct pushbutton *self;
int stylecode;
{
/*
  Set the style code for this object.
*/

    self->style = stylecode;
    pushbutton_NotifyObservers(self,observable_OBJECTCHANGED);
}


void
pushbutton__SetButtonFont(self, f)
struct pushbutton *self;
struct fontdesc *f;
{
/*
  Set the font descriptor for this object.
*/

  if (self->myfontdesc) free(self->myfontdesc);
  self->myfontdesc = f;
  pushbutton_NotifyObservers(self,observable_OBJECTCHANGED);
}



static void
WriteLine(f, l)
FILE *f;
char *l;
{
/* 
  Output a single line onto the data stream, quoting
  back slashes and staying within line length limits.
  Warning:  this routine wasn't meant to handle embedded
  newlines.
*/

  char buf[MAX_LINE_LENGTH];
  int i = 0;

  for (;*l != '\0'; ++l) {
    if (i > (MAX_LINE_LENGTH - 5)) {
      buf[i++] = '\\';  /* signal for line continuation */
      buf[i++] = '\n';
      buf[i++] = '\0';
      fputs(buf,f);
      i = 0;
    } /* if (i > ...) */
    switch (*l) {
    case '\\': 
      /* if a backslash, quote it. */
      buf[i++] = '\\';
      buf[i++] = *l;
      break;
    default:
      buf[i++] = *l;
    } /* switch (*l) */
  } /* for (; *l != ... ) */

  /* Need to empty buffer */
  if ((i > 0) && (buf[i-1]==' ')) {
    /* don't allow trailing whitespace */
    buf[i++] = '\\';
    buf[i++] = '\n';
    buf[i++] = '\0';
    fputs(buf,f);
    fputs("\n",f);
  } else {
    buf[i++] = '\n';
    buf[i++] = '\0';
    fputs(buf,f);
  }
}


static char *
GlomStrings(s, t)
char *s, *t;
{
/* 
  Safely (allocs more memory) concatenates the two strings, 
  freeing the first.  Meant to build a new string of unknown length.
*/

  char *r;

  if (r = (char *)malloc(strlen(s)+strlen(t)+1)) {
    *r = '\0';
    strcpy(r,s);
    free(s);
    strcat(r,t);
    return(r);
  } else {
    free(s);
    return(NULL);
  }
}


static char *
ReadLine(f)
FILE *f;
{
/* 
  Reads from the datastream, attempting to return a single string.
  Undoes quoting and broken lines.
  Warning:  this routine wasn't meant to handle embedded
  newlines.
  Warning:  possible source of memory leaks;  remember to 
  free the returned string when finished with it!
*/

  char buf[MAX_LINE_LENGTH], /* (BUG) What if the datastream is broken? */
       buf2[MAX_LINE_LENGTH],
      *result;
  int i,j;

  
  if (result = (char *)malloc(1)) {
    *result = '\0';

    while (fgets(buf,sizeof(buf),f)) {
      for (i = 0, j = 0; buf[i] != '\0'; ++i) {
	switch (buf[i]) {
	case '\\':
	  /* Unquote backslash or splice line */
	  switch (buf[++i]) {
	  case '\\':
	    /* Unquote the backslash */
	    buf2[j++] = buf[i];
	    break;
	  case '\n':
	    /* broke long line */
	    break;
	  default:
	    /* things like \enddata come through here */
	    buf2[j++] = '\\';
	    buf2[j++] = buf[i];
	    break;
	  } /* switch (buf[++i]) */
	  break;
	case '\n':
	  /* An unquoted newline means end of string */
	  buf2[j++] = '\0';
	  result = GlomStrings(result, buf2);
	  return(result);
	default:
	  buf2[j++] = buf[i];
	  break;
	} /* switch (buf[i]) */
      } /* for (i = 0, ...) */
      buf2[j++] = '\0';
      result = GlomStrings(result, buf2);
    } /* while (fgets...) */
    /* Should not get here... it means we went off the end
       of the data stream.  Ooops. */
  } /* if (result = ... ) */
  return(NULL);
}


static char *
EncodeFont(self)
struct pushbutton *self;
{
/*
  Returns a string representing the name of the font for this object.
  (BUG) I shouldn't have to do this function, it should be a method
  of the fontdesc object.  In any case, I handle only Bold, Italic,
  and fixed styles.
*/

  char *buf, type[15];
  long myfonttype, myfontsize;
  char *myfontname;

  *type = '\0';
  myfontname = fontdesc_GetFontFamily(self->myfontdesc);
  myfontsize = fontdesc_GetFontSize(self->myfontdesc);
  myfonttype = fontdesc_GetFontStyle(self->myfontdesc);
  if (myfonttype & fontdesc_Bold) strcpy(type,"b");
  if (myfonttype & fontdesc_Italic) strcpy(type,"i");
  if (myfonttype & fontdesc_Fixed) strcpy(type,"f");
  if (buf = (char *)malloc(strlen(myfontname)+25)) {
    sprintf(buf,"%s%d%s", myfontname, myfontsize, type);
    return (buf);
  } else {
    return(NULL);
  }
}

