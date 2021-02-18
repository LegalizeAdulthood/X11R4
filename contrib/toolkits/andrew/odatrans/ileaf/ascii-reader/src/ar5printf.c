
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/*************************************************/
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  02/88    BT	Original coding, revised from public domain sources.
 */
/* 
 * arprintf.c 
 */
/*	Modified from the public-domain original by Bill Torcaso
 *	2/88
 *	Added Interleaf extensions '%C' and '%S' in support of shar data.
 *	Added '%#c', '%#s', '%#C', '%#S' for alternate-form printing.
 *	3/88
 *	Added '%#q', '%#Q' to support output of quoted strings in I.A.M. format.
 *	5/17/88
 *	Fix bug in %C: the order of bytes in storage matters; the fix is in
 *	a non-portable pair of macros, SHORTLOC and CHARLOC.
 *	6/29/89 deen
 *	  Added some portability fixes for VMS.  I may well have not done the
 *	  whole job of porting this file for VMS -- just enuf for calsout.
 *	6/29/89 Eric Sosman
 *	  Eliminated SHORTLOC, CHARLOC, and other dependencies on REVERSEBYTE
 *	  (a symbol available only when this file is compiled within a TPS
 *	  context, but missing when compiled for export as part of the
 *	  ASCII Reader Developer's Toolkit).
 *      7/6/89 kint
 *        Changed parameter passing scheme to a true varargs implementation.
 *        Removed '%@' format option (this was not portable and never used).
 */

/*
 *
 * This is an implementation of a subset of the standard C sprintf and fprintf
 * library functions.
 *
 * conversion notes follow:
     Sprintf returns the number of characters transmitted
     (not including the \0).

     After the %, the   following appear in sequence:

          Zero or more flags, which modify the meaning of the conversion
          specification.

          An optional decimal digit string specifying a minimum field width.
          If the converted value has fewer characters than the field width, it
          will be padded on the left (or right, if the left-adjustment flag
          (see below) has been given) to the field width;

          A precision that gives the minimum number of digits to appear
          for the d, o, u, x, or X conversions or the maximum number of
          characters to be printed from a string in s or S conversion.  The
          precision takes the form of a period (.) followed by a
          decimal digit string: a null digit string is treated as
          zero.

          An optional l specifying that a following d, o, u, x, or X
          conversion character applies to a long integer arg.

          A character that indicates the type of conversion to be applied.

     A field width or precision may be indicated by an asterisk (*) instead of
     a digit string.  In this case, an integer arg supplies the field width or
     precision.  The arg that is actually converted is not fetched until the
     conversion letter is seen, so the args specifying field width or preci-
     sion must appear before the arg (if any) to be converted.

     The flag characters and their meanings are:
     -         The result of the conversion will be left-justified within the
               field.
     +         The result of a signed conversion will always begin with a sign
               (+ or -).
     blank     If the first character of a signed conversion is not a sign, a
               blank will be prepended to the result.  This implies that if
               the blank and + flags both appear, the blank flag will be
               ignored.
     #         This flag specifies that the value is to be converted to an
               ``alternate form.''  For d and u conversions, the flag
               has no effect.  For o conversion, it increases the precision to
               force the first digit of the result to be a zero.  For x (X)
               conversion, a non-zero result will have 0x (0X) prepended to
               it.

	       Interleaf extension:  For c,C,s,S,q, and Q conversions, the flag
	       causes values outside of the printable ASCII range to be emitted
	       as <#xx> or <#xxxx>, the form used by Interleaf ASCII Markup.

     The conversion characters and their meanings are:

     d,o,u,x,X The integer arg is converted to signed decimal, unsigned octal,
               decimal, or hexadecimal notation (x and X), respectively; the
               letters abcdef are used for x conversion and the letters ABCDEF
               for X conversion.  The precision specifies the minimum number
               of digits to appear; if the value being converted can be
               represented in fewer digits, it will be expanded with leading
               zeroes.  The default precision is 1.  The result of converting
               a zero value with a precision of zero is a null string (unless
               the conversion is o, x, or X and the # flag is present).

     c         The character arg is printed.

	       Interleaf extension:
	       If the # flag is present, values outside the range of printable
	       ASCII characters are emitted as <#xx> or <#xxxx>.

     C         Interleaf extension:
	       The shar arg is printed as a character.  If the # flag is not
	       present, the value is truncated to one byte before printing.
	       If the # flag is present, values outside the range of printable
	       ASCII characters are emitted as <#xx> or <#xxxx>.

     s         The arg is taken to be a string (character pointer) and charac-
               ters from the string are printed until a null character (\0) is
               encountered or the number of characters indicated by the preci-
               sion specification is reached.  If the precision is missing, it
               is taken to be infinite, so all characters up to the first null
               character are printed.

	       Interleaf extension:
	       If the # flag is present, values outside the range of printable
	       ASCII characters are emitted as <#xx> or <#xxxx>.

     S         Interleaf extension:
	       The arg is taken to be a shar pointer and shar characters
               from the string are printed until a null shar character (0) is
               encountered or the number of characters indicated by the preci-
               sion specification is reached.  If the precision is missing, it
               is taken to be infinite, so all characters up to the first null
               shar character are printed.

	       If the # flag is present, values outside the range of printable
	       ASCII characters are emitted as <#xx> or <#xxxx>.

     q	       Interleaf extension
	       The arg is taken to be a string pointer as in %s, and characters 
	       from it are printed until a nul character is reached or the
	       precision is exhausted.  HOWEVER, characters within the string are
	       printed according to the rules of Interleaf Ascii markup: 

			'<' is doubled, becoming  '<<'
			characters outside the printable range are printed
			  as <#xxxx>
			the quote character is doubled.

	      In the normal case, the entire string is surrounded by double-
	      quotes (") and that character is doubled if it occurs within the 
	      string.  If the # flag is used (as in %#q), then the string is
	      surrounded by single quotes (') and that character is doubled 
	      within the string.

    Q	      Interleaf extension
	      Like q, except that the input is a shar string.  See %q.

     %        Print a %; no argument is converted.

     In no case does a non-existent or small field width cause truncation of a
     field; if the result of a conversion is wider than the field width, the
     field is simply expanded to contain the conversion result.

     This routine differs from the spec by having no floating point.

     Obselete features, supported

        a leading zero on an implicit length spec causes left zero fill
        to field width.

        a 'h' length modifier is cheerfully ignored (contrast to l)

        a D display spec is treated as ld.


 Without further ado:
 */
/**--------------------------------------------------------------------------**/
#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#include "sysctype.h"
#include "sysvarargs.h"
#else
#include <stdio.h>
#include <ctype.h>
#include <varargs.h>
#endif

#include "ar5defs.h"

/* ALIGN: align the said number to the architectures "natural" word alignment
 * NON-PORTABLE to i386, 68K, PC-RT, VAX, etc.
 */

#define ALIGN(p)  { int r; r = (int)p; if(r&1) ++r;  p = (unsigned *)r; }

#ifndef ERROR
#define ERROR   (-1)
#endif

       long ArFprintf();
       long ArSprintf();
static long chprintf();
static long chEmitSpecial();
static char Xradix[] = "0123456789ABCDEF";
static char xradix[] = "0123456789abcdef";

/**--------------------------------------------------------------------------**/
#define PRINT (0)
#define STORE (1)
#   define PUT(ch, printstore, outFILE, strP) \
	(printstore==PRINT? (putc((ch), outFILE)) : (*strP++ = (ch)))

/**--------------------------------------------------------------------------**/
long ArFprintf(va_alist)
va_dcl     
{
  va_list ap;
  FILE *ofile;
  char *fmtP;
  long result;

  va_start(ap);
  ofile = va_arg(ap, FILE *);
  fmtP = va_arg(ap, char *);
  if(ofile == NULL || fmtP == NULL)
    return(0);
  result = chprintf(fmtP, PRINT, ofile, NULL, &ap);
  va_end(ap);
  return(result);
}

/**--------------------------------------------------------------------------**/
long ArSprintf(va_alist)
va_dcl
{
  va_list ap;
  char *strP, *fmtP;
  long result;

  va_start(ap);
  strP = va_arg(ap, char *);
  fmtP = va_arg(ap, char *);
  if(strP == NULL || fmtP == NULL)
    return(0);
  result = chprintf(fmtP, STORE, NULL, strP, &ap);
  va_end(ap);
  return(result);
}

/**--------------------------------------------------------------------------**/
/* VARARGS */
static long
chprintf(fmt, printstore, outFILE, outSTR, apP)
 register char *fmt;        /* format control string */
 int            printstore; /* is this fprintf or sprintf? */
 FILE          *outFILE;    /* for fprintf */
 unsigned char *outSTR;     /* for sprintf */
 va_list *apP;              /* pointer to variable parameter list */
{
    register unsigned char *s = outSTR;  /* Indexes sprintf output */
    static char nullCP[] = "(null)";
    static shar nullSP[] = {'(','n','u','l','l',')',0};
    /*
     * Input parameters parsed from the format construct
     */
    int     /* flags */
	    sharp,          /* Use alt form for %X %x and %o */
	    minus,          /* left justify result */
	    blank,          /* sign as leading blank or '-' */
	    plus,           /* explicit sign '-' or '+' */
	    width,          /* min field width to use */
	    precision,      /* min number of digits to display */
	    fetchlong;      /* if true grab arg as long, else int */
    int     char_special,   /* special handling of %C %S, %Q, %#c, %#s, %#q */
	    shar_data,	    /* Read shars for %C %S %Q, but write chars */
	    quote;	    /* One of Q, q. */
    /*
     * Intermediate values describing formatting
     */
    int     lfill,          /* first print this many spaces */
	    lzero;          /* then all these zeros */
    int     prefixl;        /* length o' de prefix */
    char    *prefix,        /* then print this */
	    *bufp, *bufend; /* print chars between these */
    int     rfill;          /* finish with these spaces */
    char    buff[20],       /* format buffer for oxXudD */
	    conv,           /* conversion code (cxXuodD */
	    *rada;          /* radix string (0123456789abcdef) */
    unsigned long
	    val;            /* holds value undergoing arith conversion */

    long	c;		    /* every functions got one */
    unsigned *str_ptr = 0;  /* if in structure mode, points to */
			    /* next item in structurem else zero */

    union {		    /* for machine-independent conversion of ... */
	unsigned short shar_d;	/* ... ints to shars and ... */
	unsigned char char_d;	/* ... ints to chars	     */
    } cvt_temp;

    if ((printstore == PRINT && outFILE == NULL) 
	||
	(printstore == STORE && outSTR  == NULL)) {
	    return( 0 );	/** no target for output. **/
    }

    for(;;) {
	/* 
	 * Just pass chars through until '%' escape, or terminal null.
	 */
	while((c = *fmt++) != '%') {
	     /* NOTE: This is chsprintf's ONLY return */
	    if (c == '\0') {
		if (printstore==STORE) {
		    *s = c;		/** nul-terminate the string. **/
		}
		return (s-outSTR);    /* return # chars */
	    }
	    PUT(c,printstore,outFILE,s);
	}

	if(*fmt=='%') {         /* handle %% */
	    PUT(*fmt++,printstore,outFILE,s);
	    continue;
	}
		
	/*
	 * Looks like a real % sequence:
	 * reset state some of this initialization is
	 * clearly redundant.
	 */
	quote   = shar_data = char_special = fetchlong 
		= sharp = minus = blank 
		= plus = width = lfill
		= lzero = rfill = prefixl = 0;
	prefix  = bufp = bufend = "";
	precision = -1;         /* default to infinite precision */

	/* First check for flags: "-+# " */
	for(;;++fmt) {
	    switch(*fmt) {
	    case '#':
		sharp = 1;
		continue;
	    case '-':
		minus = 1;
		continue;
	    case ' ':
		blank = 1;
		continue;
	    case '+':
		plus = 1;
		continue;
	    default:
		break;
	    }
	    break;
	}

	/* Now get width */
	if(*fmt=='*') { /* indirect width */
	    if((width = va_arg(*apP, int)) < 0) {
		width = -width;
		minus = 1;
	    }
	    ++fmt;
	} else {
	    int lz = *fmt=='0';     /* obsolete */
	    for(; isdigit(*fmt); ++fmt)
		width = width*10 + *fmt-'0';
	    if(lz)  /* obsolete */
		precision=width;
	}
	/* now look for precision */
	if(*fmt=='.') {
	    ++fmt;
	    if(*fmt=='*') { /* indirect width */
		precision = va_arg(*apP, int);
		++fmt;
	    } else {
		precision = 0;
		for(; isdigit(*fmt); ++fmt)
		    precision = precision*10 + *fmt-'0';
	    }
	}
	/* length modifier */
	if(*fmt=='l') {
	    fetchlong = 1;
	    ++fmt;
	}
	if(*fmt == 'h')         /* Obsolete */
	    ++fmt;

	/* This switch sets the variables lfill, lzero and rfill.
	 * Also the conversions are done and pointed to by prefix
	 * and chars between bufp and bufend
	 * note bufp==bufend implies no printing 
	 * loops follow to move the stuff into the output area.
	 */
	switch(conv = *fmt++) {
	/** case 'C' is variant on 'c' and falls into it. **/
	case 'C':
	    shar_data = 1;
	case 'c':
	/* considerations: 1 char either left or right just */
	    if(!str_ptr) {
		lzero = 0;
		if (shar_data) {
		    cvt_temp.shar_d = (short)va_arg(*apP, int);
		    bufp   = (char *)(&cvt_temp.shar_d);
		    bufend = bufp + sizeof(short);
		} else {
		    cvt_temp.char_d = (char)va_arg(*apP, int);
		    bufp   = (char *)(&cvt_temp.char_d);
		    bufend = bufp+sizeof(char);
		}
	    } else {
		bufp = (char*)str_ptr; lzero = 0;
		bufend = (shar_data? bufp+sizeof(short) : bufp+1);
		str_ptr = (unsigned*)bufend;
	    }
	    if(width)
		if(minus) {
		    rfill = width-1; lfill = 0;
		} else {
		    lfill = width-1; rfill = 0;
		}
	    char_special = sharp | shar_data;
	    break;

	/** cases 'Q', 'q', and 'S' are variants on 's' and fall into it. **/
	case 'Q':
	case 'q':
	case 'S':
	    shar_data = (conv != 'q');
	    quote     = (conv != 'S');
	case 's':
	    ALIGN(str_ptr);
	    bufp = str_ptr? (char*)str_ptr: va_arg(*apP, char *);
	    if (bufp == NULL) {
		if (shar_data) {
		    bufp = (char *)nullSP;
		}
		else {
		    bufp = nullCP;
		}
	    }
	    lzero = 0;
	    if(precision < 0) {     /* infinite precision */
		bufend = bufp + (shar_data? 
			    ArShrlen((short *)bufp)*sizeof(short) : strlen(bufp));
	    } else {
		/* Caution, *apP maybe not null terminated! */
		int j = precision;
		if (shar_data) {
		    for(bufend = bufp; 
			j-- && *(short *)bufend; 
			bufend+=sizeof(short))
			    ;
		} else {
		    for(bufend = bufp; j-- && *bufend; ++bufend)
			    ;
		}
	    }
	    if(&bufp[width] > bufend)
		if(minus) {
		    rfill = width-(bufend-bufp);
		    lfill = 0;
		} else {
		    lfill = width-(bufend-bufp);
		    rfill = 0;
		}
	    if(str_ptr)
		str_ptr = (unsigned*)bufend;
	    char_special = (sharp | shar_data | quote);
	    break;

	/** the numeric cases. **/
	case 'D':
		fetchlong = 1;
	case 'd':
		ALIGN(str_ptr);
		if(!fetchlong) {
		    /* sign extend this one */
		    if(str_ptr)
			    val = *(int*)str_ptr++;
		    else
			    val = va_arg(*apP, int);
		    goto doprefix;
		}
	case 'u':
	case 'x':
	case 'X':
	case 'o':
	    ALIGN(str_ptr);
	    if(fetchlong) {
		if(str_ptr) {
		    val = *(long*)str_ptr;
		    str_ptr += sizeof(long) /
			    sizeof(unsigned);
		} else {
		    val = va_arg(*apP, long);
		}
	    } else {
		 /* no sign extend */
		if(str_ptr)
		    val = *str_ptr++;
		else
		    val = va_arg(*apP, unsigned);
	    }
	    doprefix:       /* set prefix for dDxX
			 * val is now a long with the right `sense'
			 */
	    if(conv=='d' || conv=='D') {    /* do up the sign */
		if((long)val<0) {
		    /* Watch out for overflow */
		    if(val != (1L<<(8*sizeof(long)))) {
			val = -val;
		    }
		    prefix = "-"; 
		    prefixl = 1;
		} 
		else if(blank) {
		    prefix = " "; 
		    prefixl = 1;
		} 
		else if(plus) {
		    prefix = "+"; 
		    prefixl = 1;
		}
	    } else if(sharp) {
		switch(conv) {
		case 'x': prefix = "0x"; prefixl = 2; break;
		case 'X': prefix = "0X"; prefixl = 2; break;
		case 'o': prefix = "0";  prefixl = 1; break;
		default:        break;
		}
	    }
	    /* Select radix and array, same for all except X */
	    rada = xradix;
	    bufp = bufend = &buff[sizeof buff];
	    switch(conv) {  /* drat: already switched this twice */
	    case 'X':
		rada = Xradix;
	    case 'x':
		do {
		    c = val&0x0f;
		    *--bufp = rada[c];
		    val >>= 4;
		} while (val); 
		break;
	    case 'd': case 'D': case 'u':
		do {
		    c = val%10;
		    *--bufp = rada[c];
		    val /= 10;
		} while (val);
		break;
	    case 'o':
		do {
		    c = val&0x7;
		    *--bufp = rada[c];
		    val >>= 3;
		} while (val); 
		break;
	    }
	    /* Now, bufp points to most
	     * significant digit, bufend PAST the
	     * least (both beyond buff[] if nil)
	     */

	    /* first do the left zero fill */
	    if(&bufp[precision] > bufend)
		    lzero = precision-(bufend-bufp);
	    if(&bufp[width] > bufend)
		if(minus) {
		    rfill=width-(prefixl+lzero+bufend-bufp);
		    lfill = 0;
		} else {
		    lfill=width-(prefixl+lzero+bufend-bufp);
		    rfill = 0;
		}
	    break;
	default:
#if 0
		trace("format == %c\n",c);
#else
		return ERROR;   /* error: incompete format construct */
#endif
	}
	/* now that we have are counters and buffers set up,
	 * we can finally move the fruits of our labors to
	 * the output area.
	 */
#define PUTwhile(looper, ch, printstore, outFILE, strP) \
	if (printstore==PRINT) { while (looper) { putc((ch), outFILE);}\
	} else                 { while (looper) { *strP++ = (ch); } }

	/* leading spaces */
	PUTwhile(lfill--, ' ',printstore,outFILE,s);

	/* leading radix or sign */
	PUTwhile(*prefix, *prefix++,printstore,outFILE,s);

	/* leading zeros */
	PUTwhile(lzero--, '0',printstore,outFILE,s);

	if (!char_special) {
	    /* the data itself */
	    PUTwhile((bufp < bufend), *bufp++,printstore,outFILE,s);
	} else {
	    s += chEmitSpecial(printstore, 
			       outFILE, 
			       s, 
			       bufp, 
			       bufend, 
			       sharp, 
			       shar_data,
			       quote);
	}
	/* trailing spaces */
	PUTwhile(rfill--, ' ',printstore,outFILE,s);
    }
    /* NOTREACHED */
}
/**--------------------------------------------------------------------------**/
static long
chEmitSpecial(printstore, outFILE, targetP, startP, endP, sharp, shar_data, quote)
  int  printstore;
  FILE *outFILE;
  unsigned char *targetP, *startP, *endP;
  register int sharp, shar_data, quote;
{
  long count = 0;
  register unsigned short it;

    if (quote) {
	quote = (sharp? '\'' : '"');
	sharp = 1;/** quote  implies sharp. **/
    }

    if (quote) {
	PUT(quote,printstore,outFILE,targetP);
    }
    while (startP < endP) {
	if (shar_data) {
	    it = * ((unsigned short *)startP);
	    startP += sizeof(unsigned short);
	} else {
	    it = *startP++;
	}
	if (sharp && ((it > 0x7e || it < 0x20) && it != '\n' && it != '\t')) {
	    /** put out <#xxxx> form. **/
	    PUT('<',printstore,outFILE,targetP);
	    PUT('#',printstore,outFILE,targetP);
	    PUT(Xradix[(it & 0xF000) >> 12],printstore,outFILE,targetP); 
	    PUT(Xradix[(it & 0x0F00) >>  8],printstore,outFILE,targetP);
	    PUT(Xradix[(it & 0x00F0) >>  4],printstore,outFILE,targetP);
	    PUT(Xradix[ it & 0x000F       ],printstore,outFILE,targetP);
	    PUT('>',printstore,outFILE,targetP);
	    count += 7;
	}
	else if (quote && (it == quote || it == '<')) {
	    /** double it in the output. **/
	    PUT((char)it,printstore,outFILE,targetP);
	    PUT((char)it,printstore,outFILE,targetP);
	    count += 2;
	} else {
	    /** it's a plain character. **/
	    PUT((char)it,printstore,outFILE,targetP);
	    count++;
	}
    }
    if (quote) {
	PUT(quote,printstore,outFILE,targetP);
    }
    return( count );
}
/**--------------------------------------------------------------------------**/
#ifdef MAINTEST
#include "stdio.h"
main()
{
  static char buf[512];
  static char sharHello[] = 
      { 0,'H',0,'e',0,'l',0,'l',0,'o',0,',',
	0,' ',0,'W',0,'o',0,'r',0,'l',0,'d',0,'!',0, '\n', 0,0 };
  static char charHello[] = 
      { 0x80+'H','e','l','l','o',',',' ',0x80+'W','o','r','l','d','!', '\n', 0 };
  static char squoteHello[] = 
      { '<', 0x80+'H', 'e', 'l', 'l', 'o', 's', 'q', '[', '\'', ']', '>', '\n', 0};
  static char dquoteHello[] = 
      { '<', 0x80+'H', 'e', 'l', 'l', 'o', 'd', 'q', '[', '"',  ']', '>', '\n', 0};
  static shar squoteHelloShar[] = 
      { '<', 0x80+'H', 'e', 'l', 'l', 'o', 's', 'q', '[', '\'', ']', '>', '\n', 0};
  static shar dquoteHelloShar[] = 
      { '<', 0x80+'H', 'e', 'l', 'l', 'o', 'd', 'q', '[', '"',  ']', '>', '\n', 0};

    fprintf(stdout, "ArSprintf() test.\n");
    fprintf(stdout, "Test 0:  plain `Hello, World!'\n");
  ArSprintf(buf,    "`Hello, World!'\n");
    fprintf(stdout, "[%s]", buf);
    fprintf(stdout, "\nTest 1:  shar `Hello, World!'\n");
  ArSprintf(buf,    "%S", sharHello);
    fprintf(stdout, "[%s]", buf);
    fprintf(stdout, "\nTest 2:  shar `Hello, World!' w/o '#', w/ up-range H, W\n");
  sharHello[ 1]     += 0x80;
  sharHello[15]     += 0x80;
  ArSprintf(buf,    "%S", sharHello);
  sharHello[ 1]     -= 0x80;
  sharHello[15]     -= 0x80;
    fprintf(stdout, "[%s]", buf);
    fprintf(stdout, "\nTest 3:  shar `Hello, World!' with '#' &  up-range H, W\n");
  ArSprintf(buf,    "%#S", sharHello);
    fprintf(stdout, "[%s]\n", buf);
    fprintf(stdout, "\nTest 4:  char `Hello, World!' with 's' &  up-range H, W\n");
  ArSprintf(buf,    "%s", charHello);
    fprintf(stdout, "[%s]\n", buf);
    fprintf(stdout, "\nTest 5:  char `Hello, World!' with '#s'&  up-range H, W\n");
  ArSprintf(buf,    "%#s", charHello);
    fprintf(stdout, "[%s]\n", buf);

    fprintf(stdout, "ArFprintf() test.\n");
    fprintf(stdout, "Test 0:  plain `Hello, World!'\n");
  ArFprintf(stdout, "`Hello, World!'\n");
    fprintf(stdout, "\nTest 1:  shar `Hello, World!'\n");
  ArFprintf(stdout, "%S", sharHello);
    fprintf(stdout, "\nTest 2:  shar `Hello, World!' w/o '#', w/ up-range H, W\n");
  sharHello[ 1]     += 0x80;
  sharHello[15]     += 0x80;
  ArFprintf(stdout, "%S", sharHello);
    fprintf(stdout, "\nTest 3:  shar `Hello, World!' with '#' &  up-range H, W\n");
  ArFprintf(stdout, "%#S", sharHello);
    fprintf(stdout, "\nTest 4:  char `Hello, World!' with 's' &  up-range H, W\n");
  ArFprintf(stdout, "%s", charHello);
    fprintf(stdout, "\nTest 5:  char `Hello, World!' with '#s'&  up-range H, W\n");
  ArFprintf(stdout, "%#s", charHello);

    fprintf(stdout, "ArFprintf(%q and %Q) test.\n");
    fprintf(stdout, "\nTest 0: %%q on squoteHello[].\n");
  ArFprintf(stdout, "%q", squoteHello);
    fprintf(stdout, "\nTest 1: %%#q on squoteHello[].\n");
  ArFprintf(stdout, "%#q", squoteHello);
    fprintf(stdout, "\nTest 2: %%Q on squoteHelloShar[].\n");
  ArFprintf(stdout, "%Q", squoteHelloShar);
    fprintf(stdout, "\nTest 3: %%#Q on squoteHelloShar[].\n");
  ArFprintf(stdout, "%#Q", squoteHelloShar);
    fprintf(stdout, "\nTest 4: %%q on dquoteHello[].\n");
  ArFprintf(stdout, "%q", dquoteHello);
    fprintf(stdout, "\nTest 5: %%#q on dquoteHello[].\n");
  ArFprintf(stdout, "%#q", dquoteHello);
    fprintf(stdout, "\nTest 6: %%Q on dquoteHelloShar[].\n");
  ArFprintf(stdout, "%Q", dquoteHelloShar);
    fprintf(stdout, "\nTest 7: %%#Q on dquoteHelloShar[].\n");
  ArFprintf(stdout, "%#Q", dquoteHelloShar);

    fprintf(stdout, "\nMultiple arg test of ArFprinf()\n");
  ArFprintf(stdout,
	    " %d:%S %d:%S %d:%S %d:%S",
	    1, sharHello, 2, sharHello, 3, sharHello, 4, sharHello);

    fprintf(stdout, "\nThat's all, folks!\n");
}
#endif
/**--------------------------------------------------------------------------**/
