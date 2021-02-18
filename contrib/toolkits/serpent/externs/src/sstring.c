	static char rcsid [] = "$Header: /u/04c/mnt/new_integ/externs/src/RCS/sstring.c,v 1.1 89/10/08 23:19:07 serpent Exp $";

/*--------------------------------------------------------------------------*\
|   
|  Name:  sstring.c
|
|  Description:  
|     This file defines string functions for use with Slang programs.
|
|  Component of:
|     Function libraries.
|
|  Related files:
|     sstring.ext	- Slang declarations of the functions.
|     sstring.h 	- shared macros
|
|  Project:
|     Serpent
|     User Interface Project
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Brian M. Clapper
|     bmc@sei.cmu.edu  
|
$Log:	sstring.c,v $
 * Revision 1.1  89/10/08  23:19:07  serpent
 * Initial revision
 * 
 * Revision 1.2  89/09/11  10:50:57  bmc
 * Now returns null string ("") in an allocated, rather than static, area.
 * 
 * Revision 1.1  89/09/07  10:02:32  bmc
 * Initial revision
 * 
|
\*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|
\*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <values.h>
#include "u22.h"
#include "global.h"
#include "sstring.h"

#define memoryPack
#include "memoryPack.h"

/*
** Notes:
** 
** 1. The "string" and "boolean" types are defined in "global.h".
*/

/*------------------------- Macros and Constants ---------------------------*/

#define Fprintf		(void) fprintf

/*--------------------------------------------------------------------------*\
|  Macros:
|     Slang_to_C - Convert a Slang string subscript into a C string subscript.
|     C_to_Slang - Convert a C string subscript into a Slang string subscript.
|
|     While this functionality can be accomplished easily without a macro,
|     using the macros isolates the conversion in one place.
\*                                                                          */

#define Slang_to_C(subscript) 		((subscript) - 1)
#define C_to_Slang(subscript) 		((subscript) + 1)

/*
|  Parameters:
|     subscript - The Slang subscript value.
|
|  Result:
|     The converted value.  No range checking is performed.
|
\*--------------------------------------------------------------------------*/

#define STRING_NULL	((string) NULL)


/*------------------------- Local Functions --------------------------------*/


/*--------------------------------------------------------------------------*\
|  Local Routine: digits
|
|  Description:
|     Determine the number of digits in a number.
|
|  Parameters:
|     number - The number to check.
|     base   - The base (i.e., 2 through 16)
|
|  Result:
|     non-zero - The number of digits required to represent the number in
|                the specified base.
|     zero     - An illegal base was specified.
\*                                                                          */

static int digits (number, base)
long number;
short base;

{
#   define FIRST_BASE 	(2)
#   define LAST_BASE  	(16)

    short result = 0;


    if ( (base >= FIRST_BASE) && (base <= LAST_BASE) )
	{
	if (number == 0)   /* Special case -- one digit required. */
	    result = 1;
	else               /* Count the number of digits required. */
	    {
	    result = 0;
	    while (number != 0)
		{
		number = number / base;
		result++;
		}
	    }
	}

    return (result);
}

/*--------------------------------------------------------------------------*\
|  Local Routine: make_null_string
|
|  Description:
|     Returns a null string in an allocated buffer.  Necessary since most
|     string routines return allocated (rather than static) memory.
|
|  Parameters:
|     None.
|
|  Result:
|     non-NULL - Null string allocated using make_node().
|     NULL     - Memory allocation error.
\*                                                                          */

static string make_null_string ()
{
    string result;


    if ( (result = make_node (1)) == STRING_NULL )
	Fprintf (stderr,
	    "string package: can't allocate memory for null string.\n");

    else
	*result = '\0';

    return (result);
}

/*------------------------- Exported Functions -----------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: string_insert
|
|  Description:
|     Inserts one string inside another.
|
|  Parameters:
|     base_string     - The string in which to insert.
|     insert_string   - The string to be inserted.
|     insert_position - The subscript in base_string where insert_string
|                       should begin.  Constraints:
|
|		        1. If insert_position is less than 1, it is in error.
|		        2. If insert_position is 1 past the last character
|			   in base_string (i.e., string_length(base_string)+1),
|			   insert_string is concatenated to base_string.
|		        3. If insert_position is more than 1 past the last
|			   character in base_string, it is an error.
|
|  Result:
|     A new string is returned containing the result.  If an error occurs,
|     "" is returned.  Space for the new string is allocated via a call
|     to make_node().
\*                                                                          */

string	string_insert (base_string, insert_string, insert_position)
string	base_string;
string	insert_string;
int	insert_position;
{
    int base_length;
    int insert_length;
    int C_insert_position;
    string result = STRING_NULL;


    base_length = string_length (base_string);
    if ( (insert_position <= 0) ||
	 (insert_position > (base_length + 1)) )
			    /*
			    ** Parameter error.  Return a null string
			    ** rather than a NULL pointer.
			    */
	result = make_null_string ();

    else
	{
			    /*
			    ** Allocate space for the return string.  Note
			    ** that if the insert_string is NULL, we still
			    ** have to allocate space for a copy of the
			    ** base string.
			    */
	insert_length = string_length (insert_string);
	if ( (result = (string) make_node (base_length
					 + insert_length
					 + 1)) == STRING_NULL )
	    Fprintf (stderr,
		"string_insert: cannot allocate memory for function result.\n");

	else
	    {
	    *result = '\0';
			    /*
			    ** There are three parts to the final string,
			    ** not all of which need to be there:
			    **   1. The part of base_string preceding the
			    **      insertion position (which may be the
			    **      entire base_string).
			    **   2. The insertion string.
			    **   3. The part of base_string following the
			    **      insertion position (which may be the
			    **      entire base_string, if we insert at 1).
			    */
	    C_insert_position = Slang_to_C (insert_position);

			    /*
			    ** The following logic must also work when the
			    ** insertion position is 1 past the end of the
			    ** string.
			    */
	    if (C_insert_position > 0)
		(void) strncpy (result, base_string, C_insert_position);

	    if (insert_length > 0)
		(void) strcat (result, insert_string);

	    if (C_insert_position < base_length)
		(void) strcat (result, base_string + C_insert_position);

	    } /* end else */
	} /* end else */

    return (result);
}

/*--------------------------------------------------------------------------*\
|  Routine: string_delete
|
|  Description:
|     Deletes a substring from a string.
|
|  Parameters:
|     target_string     - The string from which to delete.
|     starting_position - The position within target_string where the substring
|			  to be deleted begins.  This value must be between 1
|			  and string_length(target_string); otherwise, an error
|			  is flagged.
|     substring_length  - The length of the substring to be deleted.  The value
|			  of (substring_length + starting_position) cannot
|			  exceed string_length(target_string); otherwise, an
|			  error is flagged.
|
|                         This length may alternatively be specified using
|                         the TO_STRING_END constant (defined in
|                         "slang_string_macros.h"), signifying that all
|                         characters from starting_position to the end of
|                         the string should be deleted.
|
|  Result:
|     A new string is returned containing the result.  If an error occurs,
|     "" is returned.  Space for the new string is allocated via a call
|     to make_node().
\*                                                                          */

string	string_delete (target_string, starting_position, substring_length)
string	target_string;
int	starting_position;
int	substring_length;
{
    int target_length;
    int new_length;
    int C_starting_position;
    int C_ending_position;
    string result = STRING_NULL;


    if (substring_length == TO_STRING_END)
	substring_length = string_length(target_string) - starting_position + 1;

    target_length = string_length (target_string);

    if ( (target_length <= 0) ||
	 (starting_position <= 0) ||
	 (substring_length < 0) ||
	 ((starting_position + substring_length - 1) > target_length) )

			    /*
			    ** Parameter error.  Return a null string
			    ** rather than a NULL pointer.
			    */
	result = make_null_string ();
    else
	{
			    /* Allocate space for the return string. */

	new_length = target_length - substring_length;
	if ( (result = (string) make_node (new_length + 1)) == STRING_NULL )
	    Fprintf (stderr,
		"string_delete: cannot allocate memory for function result.\n");

	else
	    {
	    *result = '\0';
			    /*
			    ** Construct the final string by concatenating the
			    ** part of target_string preceding the substring
			    ** with the part following the substring.
			    */
	    C_starting_position = Slang_to_C (starting_position);
	    C_ending_position = C_starting_position + substring_length;

	    if (C_starting_position > 0)
		(void) strncpy (result, target_string, C_starting_position);

	    if ( C_ending_position < target_length )
		(void) strcat (result, target_string + C_ending_position);

	    } /* end else */
	} /* end if */

    return (result);
}

/*--------------------------------------------------------------------------*\
|  Routine: string_length
|
|  Description:
|     Returns the length of the specified string.
|
|  Parameters:
|     target_string - The string for which a length is to be returned.
|                     This parameter must be a valid string; a value of NULL
|                     is interpreted as an error.
|
|  Result:
|     The length of the string, in characters, is returned.  A value of 0
|     indicates the string is empty.  A value less than 0 indicates that
|     an error occurred.
\*                                                                          */

int	string_length (target_string)
string	target_string;
{			    /* What a pity strlen() doesn't check for NULL. */

    return (target_string == STRING_NULL ? 0 : strlen (target_string));
}

/*--------------------------------------------------------------------------*\
|  Routine: substring
|
|  Description:
|     The substring function returns the specified portion from a string.
|
|  Parameters:
|     source_string     - The string from which to extract the substring.
|     starting_position - The subscript in source_string from which to take
|                         the substring.  This parameter must be between 1
|                         and string_length(source_string); otherwise, an
|                         error is flagged.
|     substring_length  - The length of the substring.  This parameter must
|                         be greater than starting_position and must not
|                         exceed string_length(source_string); otherwise, an
|                         error is flagged.
|
|                         This length may alternatively be specified using
|                         the TO_STRING_END constant (defined in
|                         "slang_string_macros.h"), signifying that all
|                         characters from starting_position to the end of
|                         the string should be deleted.
|
|  Result:
|     A new string is returned containing the result.  If an error occurs,
|     "" is returned.  Space for the new string is allocated via a call
|     to make_node().
\*                                                                          */

string	substring (source_string, starting_position, substring_length)
string	source_string;
int	starting_position;
int	substring_length;
{
    int source_length;
    string result = STRING_NULL;


    if (substring_length == TO_STRING_END)
	substring_length = string_length(source_string) - starting_position + 1;

    source_length = string_length (source_string);

    if ( (source_length <= 0) ||
	 (starting_position <= 0) ||
	 (substring_length < 0) ||
	 ((starting_position + substring_length - 1) > source_length) )
			    /*
			    ** Parameter error.  Return a null string
			    ** rather than a NULL pointer.
			    */
	result = make_null_string ();

    else
	{
			    /* Allocate space for the return string. */

	if ( (result = (string) make_node (substring_length
					   + 1)) == STRING_NULL )
	    Fprintf (stderr,
		"substring: cannot allocate memory for function result.\n");

	else
	    {
	    (void) strncpy (result,
			    source_string + Slang_to_C (starting_position),
			    substring_length);
	    *(result + substring_length) = '\0';
	    }
	} /* end if */

    return (result);
}

/*--------------------------------------------------------------------------*\
|  Routine: string_index
|
|  Description:
|     The index function finds the first occurence of a substring within
|     another string, returning the postion of the substring.
|
|  Parameters:
|     source_string - The string in which to search.
|     substring     - The string for which to search.
|
|  Result:
|     If the substring exists within source_string, its location (i.e.,
|     substring) is returned.  Otherwise, a 0 is returned.  (N.B.:  Specifying
|     a substring that is longer than the source_string is not an error.
|     Since the substring can never be found in this case a 0 will be
|     returned.)
\*                                                                          */

int	string_index (source_string, substring)
string	source_string;
string	substring;
{
    int result = 0;
    int substring_length;
    int source_length;
    string s;


    if ( ((source_length = string_length (source_string)) > 0) &&
	 ((substring_length = string_length (substring)) > 0) )
	{
	s = source_string;
	while ( (*s != '\0') &&
		(result == 0) &&
		(source_length >= substring_length) )
	    {
	    if (strncmp (s, substring, substring_length) == 0)
		result = C_to_Slang (s - source_string);

	    else 
		{
		s++;
		source_length--;
		}
	    } /* end while */
	}

    return (result);
}

/*--------------------------------------------------------------------------*\
|  Routine:  string_upper
|
|  Description:
|     Converts every lower-case character in a string to upper-case.
|
|  Parameters:
|     the_string - The string to be converted.
|
|  Result:
|     A new string is returned containing the result.  If an error occurs,
|     "" is returned.  Space for the new string is allocated via a call
|     to make_node().
\*                                                                          */

string	string_upper (the_string)
string	the_string;
{
    int length;
    string result = STRING_NULL;
    string s;


    if ( (length = string_length (the_string)) > 0 )
	{
	if ( (result = (string) make_node (length + 1)) == STRING_NULL )
	    Fprintf (stderr,
		"string_upper: cannot allocate memory for function result.\n");

	else
	    {
	    for ( s = result; *the_string != '\0'; s++, the_string++ )
		if ( islower (*the_string) )
		    *s = toupper (*the_string);
		else
		    *s = *the_string;

	    *s = '\0';
	    } /* end else */
	} /* end if */

    return (result);
}

/*--------------------------------------------------------------------------*\
|  Routine:  string_lower
|
|  Description:
|     Converts every upper-case character in a string to lower-case.
|
|  Parameters:
|     the_string - The string to be converted.
|
|  Result:
|     A new string is returned containing the result.  If an error occurs,
|     "" is returned.  Space for the new string is allocated via a call
|     to make_node().
\*                                                                          */

string	string_lower (the_string)
string	the_string;
{
    int length;
    string result = STRING_NULL;
    string s;


    if ( (length = string_length (the_string)) > 0 )
	{
	if ( (result = (string) make_node (length + 1)) == STRING_NULL )
	    Fprintf (stderr,
		"string_lower: cannot allocate memory for function result.\n");

	else
	    {
	    for ( s = result; *the_string != '\0'; s++, the_string++ )
		if ( isupper (*the_string) )
		    *s = tolower (*the_string);
		else
		    *s = *the_string;

	    *s = '\0';
	    } /* end else */
	} /* end if */

    return (result);
}

/*--------------------------------------------------------------------------*\
|  Routine:  string_is_integer
|
|  Description:
|     Determines whether a string contains only digits.
|
|  Parameters:
|     the_string - The string to be checked.
|
|  Result:
|     false - The string does not represent a valid integer.
|     true  - The string represents a valid integer.
\*                                                                          */

boolean	string_is_integer (the_string)
string	the_string;
{
    boolean is_integer = true;
    int length;

    extern long strtol ();	    /* convert string to long (see strtol(3)) */

			    /*
			    ** Make sure the string is not longer than
			    ** the maximum long integer on this machine.
			    ** (MAXLONG is defined in <values.h>)
			    */
    if ( ((length = string_length (the_string)) == 0) ||
	 (length > digits (MAXLONG, 10)) )
	is_integer = false;

    else
	{
	while ( (*the_string != '\0') && (is_integer) )
	    if (! isdigit (*the_string) )
		is_integer = false;
	    else
		the_string++;
	}

    return (is_integer);
}

/*--------------------------------------------------------------------------*\
|  Routine:  string_count_chars
|
|  Description:
|     Counts the number of occurrences of a single character or a set of
|     characters in a string.
|
|  Parameters:
|     the_string - The string to be checked.
|     char_set   - A string containing characters to be counted.  The function
|                  counts the number of times any character from this string
|                  appears in the_string.  (I.e., to count only occurrences
|                  of a single character, this string should only contain one
|                  character.)
|
|  Result:
|     A count of the number of occurrences of all characters in char_set is
|     returned.  0 means none were found.
\*                                                                          */

int	string_count_chars (the_string, char_set)
string	the_string;
string	char_set;
{
    int count = 0;


    if ( the_string != STRING_NULL )
	while ( ((the_string = strpbrk (the_string, char_set)) != STRING_NULL)
	   && (*the_string != '\0') )
	    {
	    the_string++;
	    count++;
	    }

    return (count);
}
