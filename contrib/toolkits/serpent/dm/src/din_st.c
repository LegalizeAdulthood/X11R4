static char rcsid [] = "$Header: /u/04c/mnt/integ/dm/src/RCS/din_st.c,v 1.4 89/10/23 19:47:25 ejh Exp $";

/*--------------------------------------------------------------------------*\
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
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

/*
$Log:	din_st.c,v $
 * Revision 1.4  89/10/23  19:47:25  ejh
 * declared fopen
 * 
 * Revision 1.3  89/05/25  17:36:12  little
 * rearrange include order becasue of changes to d22_sne int d22.h
 * 
 * 
 * Revision 1.2  89/02/15  18:04:53  little
 * change var named buffer to a_buffer (do not ask me why)
 * add copyright notic
 * 
*/

#include <strings.h>
#include <stdio.h>
#define memoryPack              /* use memoryPack rather than mallocPack    */
#include "listPack.h"
#include "global.h"
#include "isc.h"
#include "ipc.h"
#include "din.h"
#include "cus.h"
#include "dob.h"
#include "d22.h"


#define MAX_SYMBOL_STRING_SIZE 256


/*  -------------------------------------------------  din_get_st_data */
void din_get_st_data (file_name)
string file_name;
{  boolean cont;
   FILE *fopen (), *stream;

   stream = fopen (file_name, "r");
   if (!stream)
      FATAL_ERROR (fprintf (stderr,
	"could not open symbol table descriptor file named %s\n", file_name))
   cont = din_get_st_entry (stream);
   while (cont)
      cont = din_get_st_entry (stream);
   return;
}


/*  -------------------------------------------------  din_get_st_entry */
boolean din_get_st_entry (stream)
FILE *stream;
{  cus_sym symbol;
   char a_buffer [10];
   boolean cont;
   int len;

   len = strlen (BEGIN_SYMBOL_ENTRY_ESCAPE);
   fread (a_buffer, 1, len, stream);
   if (feof (stream))
      return (FALSE);
   a_buffer [len] = '\0';
   if (strcmp (a_buffer, BEGIN_SYMBOL_ENTRY_ESCAPE))
      FATAL_ERROR (fprintf (stderr,"bad start st entry escape = %s\n", a_buffer))
   symbol = din_get_symbol (stream);
   DEBUG3 (fprintf (stderr, "starting a symbol entry\n"))
   cont = din_get_prop (stream, symbol);
   while (cont)
      cont = din_get_prop (stream, symbol);
   return (TRUE);
}



/*  -------------------------------------------------  din_get_element */
char *din_get_element (stream)
FILE *stream;
{  char a_buffer [10];
   int len;

   len = strlen (SYMBOL_ESCAPE);
   fread (a_buffer, 1, len, stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   a_buffer [len] = '\0';
			/* ?? the case characters are hard wired here, it
			   should probably be fixed to somehow use the 
			   same mechanism as the xxx_ESCAPE
			   strings defined in the header file */
   switch (a_buffer [1])
      {case 'S':
         return ((char *) din_read_symbol (stream));
         break;
      case 'I':
         return ((char *) din_read_integer (stream));
         break;
      case '(':
         return ((char *) din_read_list (stream));
         break;
      case ')':
         DEBUG3 (fprintf (stderr, "end of a list\n"))
         return (NULL);
         break;
      case 'G':
         return ((char *) din_read_string (stream));
         break;
      default:
         FATAL_ERROR (fprintf (stderr, "bad escape switch = %s\n", a_buffer))
      }

			/* better not get here, but if do return something */
   return (NULL);
}


/*  -------------------------------------------------  din_get_symbol */
cus_sym din_get_symbol (stream)
FILE *stream;
{  char a_buffer [MAX_SYMBOL_STRING_SIZE + 1];
   int len;

   len = strlen (SYMBOL_ESCAPE);
   fread (a_buffer, 1, len, stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   a_buffer [len] = '\0';
   if (strcmp (a_buffer, SYMBOL_ESCAPE))
      FATAL_ERROR (fprintf (stderr, "bad symbol escape = %s\n", a_buffer))
   return (din_read_symbol (stream));
}


/*  -------------------------------------------------  din_read_symbol */
cus_sym din_read_symbol (stream)
FILE *stream;
{  char a_buffer [MAX_SYMBOL_STRING_SIZE + 1];
   int new, len;
   cus_sym_type type;
   cus_sym entry;

   fread (&len, 1, sizeof (int), stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   if (len > MAX_SYMBOL_STRING_SIZE)
      FATAL_ERROR (fprintf (stderr, "too long symbol string = %d\n", len))
   fread (a_buffer, 1, len, stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   a_buffer [len] = '\0';
   fread (&type, 1, sizeof (cus_sym_type), stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   DEBUG3 (fprintf (stderr, "symbol = %s, of type = %s\n", a_buffer,
	    cus_type_print_val (type)))
   entry = cus_add_sym (a_buffer, type, &new, NO_CONTEXT);
   if (new)
      cus_put_prop (entry, id_prop, dob_create_symbol_table_entry (entry));

   DEBUG2 (fprintf (stderr, "exit din_read_symbol: sym = %s, id = %d\n",
	cus_sym_representation (entry), cus_get_prop (entry, id_prop)););
   return (entry);
}


/*  -------------------------------------------------  din_get_prop */
boolean din_get_prop (stream, symbol)
FILE *stream;
cus_sym symbol;
{  cus_property_type prop;
   char a_buffer [10];
   int len;
   char *prop_value;

   len = strlen (PROPERTY_ESCAPE);
   fread (a_buffer, 1, len, stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   a_buffer [len] = '\0';
   if (!strcmp (a_buffer, END_SYMBOL_ENTRY_ESCAPE))
      {DEBUG3 (fprintf (stderr, "end of a symbol entry\n\n"))
      return (FALSE);
      }
   if (strcmp (a_buffer, PROPERTY_ESCAPE))
      FATAL_ERROR (fprintf (stderr, "bad property escape = %s\n", a_buffer))
   fread (&prop, sizeof (cus_property_type), 1, stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   DEBUG3 (fprintf (stderr, "property = %s\n", cus_prop_print_val (prop)))
   prop_value = din_get_element (stream);

			/* some prop values need modifying before saving */
   switch (prop)
      {case mailbox_name_prop:	/* get the ipc_mailbox # from the string */
         cus_put_prop (symbol, prop, ipc_init (prop_value));
         break;
      default:			/* 'standard" prop value, no modification */
         cus_put_prop (symbol, prop, prop_value);
         break;
      }

   return (TRUE);
}


/*  -------------------------------------------------  din_read_integer */
char *din_read_integer (stream)
FILE *stream;
{  int the_int;

   fread (&the_int, sizeof (int), 1, stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   DEBUG3 (fprintf (stderr, "integer = %d\n", the_int))
   return ((char *) the_int);
}


/*  -------------------------------------------------  din_read_list */
char *din_read_list (stream)
FILE *stream;
{  LIST the_list;
   char *cont;

   DEBUG3 (fprintf (stderr, "starting a list\n"))
   the_list = make_list ();
   cont = (char *) TRUE;
   while (cont)
      if (cont = din_get_element (stream))
         add_to_tail (the_list, cont);
   return ((char *) the_list);
}


/*  -------------------------------------------------  din_read_string */
char *din_read_string (stream)
FILE *stream;
{  string the_string;
   int len;

   fread (&len, 1, sizeof (int), stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   the_string = make_node (len + 1);
   fread (the_string, 1, len, stream);
   if (feof (stream))
      FATAL_ERROR (fprintf (stderr, "premature EOF reading st data\n"))
   the_string [len] = '\0';
   DEBUG3 (fprintf (stderr, "string = %s\n", the_string))
   return (the_string);
}


