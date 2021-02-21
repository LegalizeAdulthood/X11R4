/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *  this file contains:
 *	get_char_str
 *	get_int
 *	get_real
 *	get_real3
 *	read_int
 *	read_real
 *	read_real2
 *	read_real3
 *	read_char
 *	read_char_str
 *	write_int
 *	write_real
 *	write_real2
 *	write_real3
 *	write_char_str
 *	write_char
 */

#include <stdio.h>
#include <strings.h>
#include <xgks.h>
#include "changes.h"

extern char *calloc();


/*
 *  get_char_str
 *
 *  description:	read in next char str from file
 *
 *  parameters:		fd (FILE *) - file to read from
 *
 *  returns:		str (char *) - ptr to next cahr string
 */

char *
get_char_str(fd,errno)
FILE *fd;				/* file to read from */
IDX errno;				/* error message no */
{
	char ln[80];			/* next input ln */
	char *str;			/* next char str */

	/* read in input ln and strip off <cr> */

	fgets(ln,80,fd);

	/* copy string into str */
	str = (char *) calloc((unsigned) strlen(ln),sizeof(char));
	(void) strncpy(str,ln,strlen(ln) - 1);
	str[strlen(ln) - 1] = (char) NULL;
	if (strlen(str) == 0)
		exit_error("get_char_str",errno);

	return(str);
}  /* end get_char_str */


/*
 *  get_int
 *
 *  description:	read in next int from file
 *
 *  parameters:		fd (FILE *) - file to read from
 *			i (int *) - return next integer in i
 */

get_int(fd,i,errno)
FILE *fd;				/* file to read from */
int *i;					/* next int in file */
IDX errno;				/* error message no */
{
	char ln[80];			/* next input ln */

	/* read in next input ln */

	fgets(ln,80,fd);
	if (sscanf(ln,"%d",i) != 1)
		exit_error("get_int",errno);

}  /* end get_int */


/*
 *  get_real
 *
 *  description:	read in next int from file
 *
 *  parameters:		fd (FILE *) - file to read from
 *			r (Gfloat *) - return next Gfloat
 */

get_real(fd,r,errno)
FILE *fd;				/* file to read from */
Gfloat *r;				/* next Gfloat in file */
IDX errno;				/* error message no */
{
	char ln[80];			/* next input ln */

	/* read in next input ln */

	fgets(ln,80,fd);
	if (sscanf(ln,"%f",r) != 1)
		exit_error("get_real",errno);

}  /* end get_real */


/*
 *  get_real3
 *
 *  description:	read in next int from file
 *
 *  parameters:		fd (FILE *) - file to read from
 *			r (Gfloat *) - return next Gfloat
 */

get_real3(fd,r1,r2,r3,errno)
FILE *fd;				/* file to read from */
Gfloat *r1,*r2,*r3;			/* next Gfloat in file */
IDX errno;				/* error message no */
{
	char ln[80];			/* next input ln */

	/* read in next input ln */

	fgets(ln,80,fd);
	if (sscanf(ln,"%f%f%f",r1,r2,r3) != 3)
		exit_error("get_real3",errno);
}  /* end get_real3 */


/*
 *  read_int
 *
 *  description:	reads next integer
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			i (int *) - next integer in file
 *			error (BOOLEAN *) - true if error
 *			reading int
 */

read_int(fd,i,error)
FILE *fd;
int *i;
BOOLEAN *error;
{
	char ln[80];			/* next input ln */

	/* read in next input ln */

	fgets(ln,80,fd);
	if (sscanf(ln,"%d",i) != 1)
	{
		reprompt(28);
		*error = TRUE;
	}
	else
		*error = FALSE;

}  /* end read_int */


/*
 *  read_real
 *
 *  description:	reads next real
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			r (Gfloat *) - next realeger in file
 *			error (BOOLEAN *) - true if error
 *			reading real
 */

read_real(fd,r,error)
FILE *fd;
Gfloat *r;
BOOLEAN *error;
{
	char ln[80];			/* next input ln */

	/* read in next input ln */

	fgets(ln,80,fd);
	if (sscanf(ln,"%f",r) != 1)
	{
		reprompt(28);
		*error = TRUE;
	}
	else
		*error = FALSE;

}  /* end read_real */


/*
 *  read_real2
 *
 *  description:	reads next 2 reals
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			r1,r2 (Gfloat *) - next integer in file
 *			error (BOOLEAN *) - true if error
 *			reading int
 */

read_real2(fd,r1,r2,error)
FILE *fd;
Gfloat *r1,*r2;
BOOLEAN *error;
{
	char ln[80];			/* next input ln */

	/* read in next input ln */

	fgets(ln,80,fd);
	if (sscanf(ln,"%f %f",r1,r2) != 2)
	{
		reprompt(28);
		*error = TRUE;
	}
	else
		*error = FALSE;

}  /* end read_real2 */


/*
 *  read_real3
 *
 *  description:	reads next 3 reals
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			r1,r2,r3 (Gfloat *) - next 3 reals in file
 *			error (BOOLEAN *) - true if error
 *			reading reals
 */

read_real3(fd,r1,r2,r3,error)
FILE *fd;
Gfloat *r1,*r2,*r3;
BOOLEAN *error;
{
	char ln[80];			/* next input ln */

	/* read in next input ln */

	fgets(ln,80,fd);
	if (sscanf(ln,"%f %f %f",r1,r2,r3) != 3)
	{
		reprompt(28);
		*error = TRUE;
	}
	else
		*error = FALSE;

}  /* end read_real3 */


/*
 *  read_char_str
 *
 *  description:	read in next char str from file
 *
 *  parameters:		fd (FILE *) - file to read from
 *			error (BOOLEAN *) - true if error reading string
 *
 *  returns:		str (char *) - ptr to next cahr string
 */

char *
read_char_str(fd,error)
FILE *fd;				/* file to read from */
BOOLEAN *error;
{
	char ln[80];			/* next input ln */
	char *str;			/* next char str */

	/* read in input ln and strip off <cr> */

	fgets(ln,80,fd);

	/* copy string into str */
	str = (char *) calloc((unsigned) strlen(ln),sizeof(char));
	(void) strncpy(str,ln,strlen(ln) - 1);
	str[strlen(ln) - 1] = (char) NULL;
	if (strlen(str) == 0)
	{
		reprompt(28);
		*error = TRUE;
	}
	else
	{
		*error = FALSE;
	}

	return(str);
}  /* end read_char_str */


/*
 *  read_char
 *
 *  description:	read in next char str from file
 *
 *  parameters:		fd (FILE *) - file to read from
 *			ch - (char *) - ptr to single ch
 *			error (BOOLEAN *) - true if error reading string
 */

read_char(fd,ch,error)
FILE *fd;				/* file to read from */
char *ch;
BOOLEAN *error;
{
	char ln[80];			/* next input ln */

	fgets(ln,80,fd);

	/* make sure line is not blank */

	if ((strlen(ln) - 1) <= 0)
	{
		reprompt(28);
		*error = TRUE;
		*ch = (char) NULL;
	}
	else
	{
		*error = FALSE;
		*ch = ln[0];
	}

	return;
}  /* end read_char */


/*
 *  write_int
 *
 *  description:	writes next integer
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			i (int) - next integer to write out
 */

write_int(fd,i)
FILE *fd;
int i;
{
	fprintf(fd,"%d\n",i);
}  /* end write_int */


/*
 *  write_real
 *
 *  description:	writes next real
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			r (Gfloat *) - next real to write out
 */

write_real(fd,r)
FILE *fd;
Gfloat r;
{
	fprintf(fd,"%f\n",r);
}  /* end write_real */


/*
 *  write_real2
 *
 *  description:	writes next 2 reals
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			r1,r2 (Gfloat) - next two reals to write out
 */

write_real2(fd,r1,r2)
FILE *fd;
Gfloat r1,r2;
{
	fprintf(fd,"%f %f\n",r1,r2);
}  /* end write_real2 */


/*
 *  write_real3
 *
 *  description:	writes next 3 reals
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			r1,r2,r3 (Gfloat) - next 3 reals to write
 */

write_real3(fd,r1,r2,r3)
FILE *fd;
Gfloat r1,r2,r3;
{
	fprintf(fd,"%f %f %f\n",r1,r2,r3);
}  /* end write_real3 */


/*
 *  write_char_str
 *
 *  description:	writes next char string to file
 *
 *  parameters:		fd (FILE *) - file to write to
 *			str (char *) - string to write
 */

write_char_str(fd,str)
FILE *fd;				/* file to write to */
char *str;
{
	fprintf(fd,"%s\n",str);
}  /* end write_char_str */


/*
 *  write_char
 *
 *  description:	writes next char to file
 *
 *  parameters:		fd (FILE *) - file to write to
 *			ch (char) - next char to write
 */

write_char(fd,ch)
FILE *fd;				/* file to write to */
char ch;
{
	fprintf(fd,"%c\n",ch);
}  /* end write_char */


