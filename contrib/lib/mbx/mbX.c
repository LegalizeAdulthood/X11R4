
#ifndef lint
static char sccs[] = "@(#)mbX.c	1.7";
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
 * Name: mbX - contains the locale specific functions
 *
 * Description:
 *
 *   mbXsetlocale - user to set / reset the locale being used.
 *   mbXFindIndex - parses a code point returning a glyph index.
 *		    NOTE: the CSid is needed from the CSid function.
 *   CSid	  - Manages the mapping of CS identifers & code point
 *		    mapping.
 *	
 */


#include <locale.h>
#include "mbX.h"
#include <X11/Xlib.h>
#include <assert.h> 


/*
 * Supported locales as of this release
 */


mbXLocale *_mbXlocale;
static char *current_locale ;	/* LC_CTYPE setting */ 

static char *InitializeLocale(locale)
char *locale;
{
  /*
   * This should really read a table from a file but this is 
   * good enough for now.  |||
   * Validation is also needed. Currently all locales default to pc850.
   */

extern mbXLocale _mbXpc850; /* PC Code, IBM code page 850, 1 byte code */
extern mbXLocale _mbXpc932; /* Japanese IBM PC Code, 
					 IBM code page 932, Shifted JIS */

  if ( !strcmp(locale,"Jp_JP.pc932") ) {
	_mbXlocale = &_mbXpc932;
  } else {
	_mbXlocale = &_mbXpc850;
  }
  return(locale);
}

char *mbXsetlocale(locale)
char *locale;
{
  char *setlocale();

  /* Handle special case if query is wanted and we are not initialized */
  if ( _mbXlocale == NULL && locale == NULL ) {
	current_locale = setlocale( LC_CTYPE, NULL);
	current_locale = InitializeLocale(current_locale);
  }

  /* Return on query */
  if ( locale == NULL ) { 
	return( current_locale ) ;
  }

  /* Caller wants system defined locale, NOTE THIS IS NOT A 
   * SET OF THE LC_CTYPE */
  if ( *locale == '\0' ) {
	char *save_locale;
	save_locale = setlocale( LC_CTYPE, NULL );
	locale = setlocale( LC_CTYPE, locale ); /* Transform name */
	setlocale( LC_CTYPE, save_locale ) ;	/* reset to callers locale */
  }

  /* Now set the mbX locale */
  return (current_locale = InitializeLocale(locale));
}


/*
 * Refer to mbX.h for an explaination of this algorithm used to 
 * map a character string code point to an glyph index.
 */
char *_mbXFindIndex( str, len ) 
char *str;
int len;
{
  int index_modifier, base_modifier;
  int n;
  char *value;
  char *indexes;

  indexes = (char *)_mbXlocale->indexes;
  for ( index_modifier = base_modifier = 0 ; len ; ++str, --len) { 
	if ((n = *str + index_modifier + base_modifier ) 
		<= _mbXlocale->hdr.last_index) {
	     index_modifier = 0; 
	     value = indexes + (n * sizeof(mbXindex));
	     if ( *value & INDBASE ) {
	       base_modifier = *((mbX4byte *)(value)) & ~INDMASK;
	     } else if ( *value & INDMOD ) {
	       index_modifier = *((mbX4byte *)(value)) & ~INDMASK;
	     } else if ((*value & INDMASK) == 0) {
	       return( value ) ;
	     } else {
		/* character index array data error - ignore */
		assert(0);
	     }
	} else {
	     /* offset into CHAR_INDEX is in error */
	     assert(0);
	}
   }
}

int CSid( str, len, fontindex16)
char *str;
int len;
XChar2b **fontindex16;
{
  if ( fontindex16 ) {
     *fontindex16 = (XChar2b *) (_mbXFindIndex( str, len ) + 2 ) ;
  }

  return( _mbXlocale->hdr.CSid[*str] ) ; 
}
