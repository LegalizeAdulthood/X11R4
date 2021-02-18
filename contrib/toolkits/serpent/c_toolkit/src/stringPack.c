/*************************************************************************

	C Programming Toolbox

	Module Name:	stringPack
	Purpose:	String input and manipulation package
	Author:		Robert A Zimmermann, Carnegie -Mellon University
			Department of Electrical and Computer Engineering

	History:	Notes:
	20 Jan 84	Began Creation of Module

	22 Feb 84	Created stringPack from layout program module
			lString
	
	23 Feb 84	Eliminated temporary sprintf in intToStr

	23 Feb 84	added skipTo and getReal
			fixed peek, though why it didn't work I don't know

	14 Mar 84	Made maximum string length longer

	 2 Apr 84	Fixed bug in intToStr ('10' was coming out '0')
	
	20 Aug 85	Added skipString

 *************************************************************************

 input functions:

	type	name			returns
	----	----			-------

	STRING	getString( fp )		address of string read from FILE fp

	int	getInt( fp )		integer read from FILE fp

	float	getReal( fp )		integer read from FILE fp

	char	peek( fp )		next character in FILE fp

	int	skipTo( fp, chars )	first character found in fp that is
					in chars

boolean functions:

	int	stringEQ( s1, s2 )	is string s1 == string s2?

	int	inString( ch, s1 )	is char ch in string s1

string functions

	STRING	newString( s1 )		an allocated copy of string s1

	STRING	stringAPP(s1, s2, max)	s1 (s1 = s1+s2 with max characters)

	STRING	intToStr( val )		address of string with ascii of val

	int	strToInt( s1 )		integer with valid part of s1

	int	stringLEN( s1 )		length of s1

special functions

	int	stringTable( s1, tab, max )	0 or string number matched

	<void>	endString( s1 )		strings read by getString may only

	<void>	skipString( s1 )	strings read by getString will begin
					after skipping these characters

/*
 * Constants and external routines
 */

#define MAXSTRING 256			/* Twiddle-able sized buffer	  */

#ifndef REPORT_ERRORS			/* default is to report errors	  */
#define REPORT_ERRORS	1		/* so unless we compile -D...	  */
#endif					/* define the report flag TRUE	  */

#define TRUE	1
#define FALSE	0

#define EOS	'\0'			/* C standard end-of-string marker*/

#include <stdio.h>			/* Unix standard IO		  */

#include <memoryPack.h>			/* string allocation routines	  */

#include <stringPack.h>			/* our header file		  */

/*
 * Our internal strings
 */


static char	buffer[ MAXSTRING ];	/* Buffer for reading in a string  */

static STRING	SkipChars  = " \t\n\f",
		EndChars   = " \t\n\f";

/*
 * Utility Routines
 */

int	peek( fp )
FILE	*fp;
    {
    register int c;
    c = fgetc( fp );			/* Yes this can be done in one	*/
    ungetc( c, fp );			/* statement, but that doesn't	*/
    return c;				/* work properly.		*/
    }


	skipString( str )
STRING	str;
    {
    SkipChars = str;
    }

	endString( str )
STRING	str;
    {
    EndChars = str;
    }

/*
 * routines for skipping over characters
 */


int	inString( c, str )
int	c;
STRING	str;
    {
    register STRING ptr;

    if ((ptr = str) == NULL || c == EOF) return FALSE;

    while (*ptr) if (*ptr++ == c) return TRUE;

    return FALSE;
    }



int	skipSkipChars( fp )
FILE	*fp;
    {
    register int c;

    while (inString( c=fgetc(fp), SkipChars ));
    return c;
    }


int	skipTo( fp, chars )
FILE	*fp;
STRING	chars;
    {
    register int c;

    while (!inString( c=fgetc(fp), chars ) && c != EOF);
    return c;
    }


/*
 * String input routines
 */

STRING	getString( fp )
FILE	*fp;
    {
    register STRING ptr;		/* Current character position	   */
    register int  c,			/* actually a char, but EOF is int */
		  len;			/* Number of characters read	   */

    if ((c = skipSkipChars( fp )) == EOF)	/* Ignore blanks, tabs, etc   */
	return NULL;				/* Return pointer at end-file */

    len = 0;
    for (   ptr = buffer;
	    ++len < MAXSTRING && !inString(*ptr = c, EndChars);
	    c = fgetc( fp ) )
		if	(c == '\b')	ptr--;	/*	BACKSPACE	*/
		else if (c == EOF)	break;	/*	End of file	*/
		else			ptr++;	/*	forward!	*/

    if (ptr == buffer)				/* one character	*/
	ptr++;					/* so, advance pointer	*/
    else {
	ungetc(c, fp);				/* save for later	*/
	if (len >= MAXSTRING) {
#if REPORT_ERRORS
	    printf("warning: getString buffer size exceeded\n");
#endif
	    ptr--;
	    }
	}

    *ptr = EOS;				/* string terminator	*/

#if DEBUG
    printf ( "---%s---",buffer );
#endif

    return buffer;				/* send back address*/
    }


/*
 * What is the length of a string?
 */
int	stringLEN( str )
STRING	str;
    {
    register int i;

    for (i=1; *str++ != EOS; i++) ;			/* find its length   */
    return i;
    }


/*
 * newString return a pointer to a string for which it allocates storage.
 * The string at 'str' has been copied into the returned string;
 */

STRING	newString( str )
STRING	str;
    {
    register STRING ptr, ptr1, ptr2;

    if ((ptr1 = str) == NULL) return NULL;		/* empty string	     */

    ptr = ptr2 = (STRING) make_node( stringLEN(ptr1) );	/* get memory	     */
    while  ((*ptr2++ = *ptr1++) != EOS);		/* copy the string   */
    
#if DEBUG
    printf ( "(new'%s', at %ld, length=%d)", ptr, ptr, i );
#endif

    return ptr;						/* return the address*/
    }

/*
 * String to integer routines (assume strings are valid integers)
 */

	strToInt( str )
STRING	str;
    {
    register int val, isNegative;
    register STRING	p;

    val = 0;
    if (str != NULL) {
	if (isNegative = (*str == '-')) str++;		/* leading '-' */
	for (p = str; *p >= '0' && *p <= '9'; val = val*10 + (*p++ - '0') );
	}
    return (isNegative) ? -val : val;
    }


getInt( fp )
FILE *fp;
    {
    return strToInt( getString( fp ) );
    }

/*
 * integer to string conversion
 */

static	STRING fillInInteger( val, str )
int	val;
STRING	str;
    {
    *str = '0' + (val%10);
    if (val >= 10) return fillInInteger( val/10, --str );
    return str;
    }


STRING	intToStr( val )
int	val;
    {
    register STRING save;
    register int  pos;

    pos = MAXSTRING - 1;
    buffer[pos] = EOS;

    if (val >= 0) return fillInInteger( val, &buffer[pos-1] );

    save = fillInInteger( -val, &buffer[pos-1] );
    *--save = '-';
    return save;
    }

/*
 * Read and return a real number from the input file.
 * NOTE: the character '.' must be in the end_string for this to work.
 */

float getReal( fp )
FILE *fp;
    {
    float intPart, fractPart;

    intPart  = (float) getInt( fp );		/* read the integer	*/

    if (peek(fp) == '.') {			/* if ther's a dec point*/
	getString( fp );			/* skip the dec point	*/
	for (fractPart = (float) getInt( fp );	/* read the fraction    */
	     fractPart >=  1.0;			/* while it is whole	*/
	     fractPart /= 10.0);		/* reduce to a fraction */
	intPart += (intPart < 0) ? -fractPart : fractPart;
	}

   return intPart;
   }

/*
 * String functions
 */

stringEQ( str1, str2 )
STRING str1, str2;
    {
    register STRING ptr1, ptr2;

    if ((ptr1 = str1) == NULL) return TRUE;	/* end-file strings always */
    if ((ptr2 = str2) == NULL) return TRUE;	/* match.  Why not??	   */

    while  (*ptr1 == *ptr2 && *ptr1 != EOS) { ptr1++; ptr2++; }
    return (*ptr1 == EOS  && *ptr2 == EOS);
    }


/*
 *	Append a string onto another.  Returns a pointer to the lead string
 */

STRING	stringAPP( dest, source, max )
STRING	dest, source;
int	max;
    {
    register STRING ptr, ptr2;
    register int    chars;

    chars = max;
    for (ptr  = dest  ; *ptr  != EOS && chars-- > 0;  ptr++		);
    for (ptr2 = source; *ptr2 != EOS && chars-- > 0; *ptr++ = *ptr2++	);

    if  (chars == 0)	*--ptr = EOS;		/* truncate to max chars */
    else		*ptr   = EOS;		/* mark end of string	 */

    return dest;
    }

/*
 * Perform a table lookup
 */

int	stringTable( str, table, max )
STRING	str, table[];
int	max;
    {
    register int    i;
    register STRING token;

    if ((token = str) == NULL) return 0;		/* read <token> */

    for (i=0; i<=max; i++)				/* search table */
	if (stringEQ( token, table[i] )) return i;	/* until found  */

#if DEBUG
    printf ("Could not identify '%s'", token );		/* DEGUG print  */
#endif

    return 0;
    }
