
#ifndef lint
static char sccs[] = "@(#)mbXLoad.c	1.1";
#endif

/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * Name: mbXLoad - load in fonts needed for multibyte string
 *
 * Description:
 *
 */

#include <X11/Xlib.h>
#include <locale.h>
#include <ctype.h>
#include "mbX.h"

typedef XFontStruct **mbXFont;

mbXFont mbXLoadQueryFont( dpy, fontlist )
Display *dpy;
char *fontlist;
{
  char *fl, *s, *name;
  int len, i;
  XFontStruct *font;
  mbXFont fonts;
  int current_font;

  if ( fontlist == NULL || *fontlist == '\0' ) {
     return NULL;
  }

  if ( _mbXlocale == NULL ) {
     mbXsetlocale(setlocale(LC_CTYPE, NULL));
  }

  fl = s = (char *) XtMalloc( (len = strlen(fontlist)) + 1 );
  bcopy( fontlist, fl, len + 1 ) ; 

  fonts = (mbXFont) XtMalloc(sizeof(XFontStruct *) 
				* (_mbXlocale->hdr.num_CSid + 1)) ;
  current_font = 0 ;

  while (current_font < _mbXlocale->hdr.num_CSid) {

      /* Delimit a name by spaces <<< This may change>>> */
      while ( isspace(*s) && *s != '\0' ) { ++s; }
      name = s;
      while( ! isspace(*s) && *s != '\0' ) { ++s; }
      if ( (s-name) == 0 ) break;

      /* Save start of font name */ 
      fonts[current_font++] = (XFontStruct *)name ; 

      if ( *s != '\0' ) *s++ = '\0' ;
  }

  /* Verify we have at least one font */
  if ( current_font == 0 ) {
      XtFree(fl);
      return ( NULL );
  }

  /* Make last font entry null */
  fonts[current_font] = NULL;

  for ( i = 0 ; i < current_font ; ++i ) {

      name = (char *)fonts[i];

      if ( *name == '-' && name[1] == '\0' ) {
	 if ( i == 0 ) {
	    XtFree(fl);
	    XtFree( fonts );
	    return( NULL ) ; 
	 }
	 fonts[i] = fonts[0] ; 
      } else if ( (font = XLoadQueryFont( dpy, name) ) == NULL ) {
	 while ( i-- ) XFreeFont( dpy, fonts[i-1] ); 		
	 XtFree( fl ) ; 
	 XtFree( fonts ) ;
	 return ( NULL ) ;
      } else {
	 fonts[i] = font ;
      }
  }
  while ( i < _mbXlocale->hdr.num_CSid ) fonts[i++] = fonts[0];
  XtFree(fl);
  return(fonts);
}


