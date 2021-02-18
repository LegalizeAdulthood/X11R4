/*

ODA Toolkit Validation Suite

FILE: vsupp.c

Copyright (c) 1989 Carnegie Mellon University

Two support functions for the various validation test routines:

PrtOD sends a null terminated string to an output descriptor.
It returns an integer (SUCCESS_INT or ERROR_INT) but this
is ignored in situations where PrtOD is used to display
error messages.  (If it fails, display another error message??)

Chars2Sequence converts a null terminated string to a
sequence of BYTE_type.

*/

#include <odatk.h>


/*

PrtOD prints a character string to a given output descriptor.
Adapted from PrintString() in valid.c.

*/

INT_type PrtOD(od, str)
    /*IN*/ INT_type od;
    /*IN*/ CHAR_type *str;
{
    INT_type rval;
    CHAR_type *pos;
    CHAR_type buffer[1];

    pos = str;
    do {
	buffer[0] = *pos++;
	rval = LOCAL_WriteChars(od, buffer, 1);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    } while (*pos != '\0');

LEAVE:
    return rval;
}


/*

Chars2Sequence takes a standard null-terminated string and
returns a value of SEQUENCE_BYTE_type which is a converted
version of the string.  
Caveat: do not modify the null-terminated string, even after
Chars2Sequence returns.

*/

SEQUENCE_type Chars2Sequence(str)
    /*IN*/  CHAR_type	*str;
{
    SEQUENCE_type   seq;
    INT_type	    length;
    INT_type	    i;
    CHAR_type	    *c;

    /* Find the length of the C string */
    for (length=0, c=str; *c!='\0'; c++) length++;

    seq = MakeSequence(SEQUENCE_BYTE_tag, (INT_type) length);
    if (seq == ERROR_SEQUENCE) {
	goto LEAVE;
    }

    /* Now, copy the bytes */
    for (i=0; i<length; i++) {
	seq->sequence_value.bytes[i] = str[i];
    }

LEAVE:
    return seq;
}
