/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/procchar.c,v 1.2 89/09/08 17:49:46 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/procchar.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
CMU-ITC-071) for full text & permitted uses.

$Log:	procchar.c,v $
 * Revision 1.2  89/09/08  17:49:46  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:35:00  mss
 * Initial revision
 * 
 * Revision 1.3  89/05/17  15:13:13  jaap
 * plugging memory leaks,
 * cosmetic changes
 * 
 * Revision 1.2  89/01/12  17:56:45  jaap
 * better handling of escape sequences in character content portion
 * As a side effect we now also know about STAB although it is not done properly to the
 * standard
 * 
 * Revision 1.1  89/01/05  18:44:48  jaap
 * Initial revision
 * 
 */

#ifndef lint
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/procchar.c,v $";
#endif
/* static char sccsid[] = "%Z%	%M%	%I%	%E%"; */
/*
 * minimum solution for Processing Char contents
 */

#include <ext.h>

#define MAXTRFONTS 8

/*
 * process char content portions(basic portion, contents, contentstype, contentsmode)
 *
 */

at_CONCATENATION_type concat;
struct FontStruct trfonts;
cc_CHARACTER_FONTS_type font ;

INT_type
ProcessCharCP(basic, contents, type, mode)
CONSTITUENT basic;
SEQUENCE_CONSTITUENT_type contents;
INT_type type, mode;
{
	at_OFFSET_type *offset;
	at_SEPARATION_type *separation;
	cc_ALIGNMENT_type align;
	cc_1ST_LINE_OFFSET_type firstline;
	cc_LINE_SPACING_type linespace;
	cc_ORPHAN_SIZE_type orphan;
	cc_INDENTATION_type indent;

	INT_PAIR_type  *ipptr;
	INT_type        i, rval = SUCCESS_INT;

	/*
	 * trcomment("processCharCP"); Check for concatenation 
	 */
	if (GetValueAttr(basic, at_CONCATENATION, (VALUE_type *) & concat, INHERIT)
	    == ERROR_INT) {
		sprintf(msgsbuf, "Couldn't get concatenation, assuming non fatal: %s",
			ErrorMsg(TKError));
		fprintf(stderr, "%s\n", msgsbuf);
		trcomment(msgsbuf);
		concat = at_CONCATENATION_non_concated;
	}
	if (concat == at_CONCATENATION_non_concated) {
		/*
		 * Linespacing 
		 */
		if (GetIntAttr(basic, cc_LINE_SPACING, (POINTER_type) & linespace, INHERIT)
		    == ERROR_INT) {
			sprintf(msgsbuf, "Couldn't get linespacing: %s",
							ErrorMsg(TKError));
			fprintf(stderr, "%s\n", msgsbuf);
			trcomment(msgsbuf);
			linespace = 200;	/* default */
		}
		/*
		 * Get Offset values 
		 */
		if (GetAttr(basic, at_OFFSET, (POINTER_type) & offset, INHERIT, IGNORE)
		    == ERROR_INT) {
			sprintf(msgsbuf, "Couldn't get offset defaulting, %s",
								ErrorMsg(TKError));
			trcomment(msgsbuf);
			fprintf(stderr, "%s\n", msgsbuf);
			offset->right_hand_offset = 0;
			offset->left_hand_offset = 0;
		}
		/*
		 * indentation value
		 */
		if (GetIntAttr(basic, cc_INDENTATION, (POINTER_type) &indent, INHERIT)
		    == ERROR_INT) {
			sprintf(msgsbuf, "Couldn't get indentation, %d",
								ErrorMsg(TKError));
			trcomment(msgsbuf);
			fprintf(stderr, "%s\n", msgsbuf);
		}
		/*
		 * First Line offset 
		 */
		if (GetIntAttr(basic, cc_1ST_LINE_OFFSET, (POINTER_type)&firstline,INHERIT)
		     == ERROR_INT) {
			sprintf(msgsbuf, "Couldn't get first line offset %s",
								 ErrorMsg(TKError));
			trcomment(msgsbuf);
			fprintf(stderr, "%s\n", msgsbuf);
			firstline = 0;
		}
		/*
		 * Get orphan size 
		 */
		if (GetIntAttr(basic, cc_ORPHAN_SIZE, (POINTER_type) & orphan, INHERIT)
		     == ERROR_INT) {
			sprintf(msgsbuf, "Couldn't get orphan size %s",
								 ErrorMsg(TKError));
			trcomment(msgsbuf);
			fprintf(stderr, "%s\n", msgsbuf);
			orphan = 1;
		}
		/*
		 * Get separation values 
		 *
		 * (not supported by toolkit??), note no minimum & maximum
		 * values 
		 */
		if (GetAttr(basic, at_SEPARATION, (POINTER_type) & separation, INHERIT,
		    IGNORE) == ERROR_INT) {
			sprintf(msgsbuf, "Couldn't get separation %s", ErrorMsg(TKError));
			fprintf(stderr, "%s\n", msgsbuf);
			trcomment(msgsbuf);
			separation->trailing_edge = 200;
			separation->leading_edge = 200;
		}
		/*
		 * SHOULD do FONT stuff here ?
		 */
		if (font != NULL_SEQUENCE) {	/* delete old font sequence */
			if (DeleteSequence(font) == ERROR_INT) {
				sprintf(msgsbuf, "Couldn't delete font: %s",
							ErrorMsg(TKError));
				trcomment(msgsbuf);
				fprintf(stderr, "%s\n", msgsbuf);
			}
		}
		if (GetAttr(basic, cc_CHARACTER_FONTS, (POINTER_type) & font, INHERIT,
		    IGNORE) == ERROR_INT) {
			sprintf(msgsbuf, "Couldn't get character fonts %s",
				ErrorMsg(TKError));
			fprintf(stderr, "%s\n", msgsbuf);
		} else {
			if (font != NULL_SEQUENCE) {
				ipptr = &font->sequence_value.int_pairs[0];
				/*
				 * mount 8 max of 8 fonts in troff
				 */
				i = font->length;
				if (i > MAXTRFONTS) {
					sprintf(msgsbuf, "Warning: possible fontspace problem %d",i);
					trcomment(msgsbuf);
					fprintf(stderr, "%s: %s\n",progname, msgsbuf);
				}
				blfonts(i,(POINTER_type) font);
				/* Now set primary */
				if ((i = InvokeFont(ipptr->second, &trfonts)) != SUCCESS) {
					sprintf(msgsbuf, "Couldn't invoke font");
					errexit(msgsbuf);
				}
				trfont(&trfonts);
			}
		}
		/*
		 * alignment 
		 */
		if (GetValueAttr(basic, cc_ALIGNMENT, (POINTER_type) & align, INHERIT)
		    == ERROR_INT) {
			sprintf(msgsbuf, "Couldn't get alignment %s", ErrorMsg(TKError));
			trcomment(msgsbuf);
			fprintf(stderr, "%s\n", msgsbuf);
			align = cc_ALIGNMENT_justified;
		}
		/*
		 * All done, send the info to troff 
		 */
		trsep(separation, orphan * linespace);
		trpar(offset, indent, align, firstline, linespace);
		/*
		 * process the contents, character by character 
		 */
	} else {	/* restore primary font (convention...) */
		if (font != NULL_SEQUENCE) {
			ipptr = &font->sequence_value.int_pairs[0];	/* primary */
			if ((i = InvokeFont(ipptr->second, &trfonts)) != SUCCESS) {
				sprintf(msgsbuf, "Couldn't invoke font");
				errexit(msgsbuf);
			}
			trfont(&trfonts);
		}
#if 0
	/* This will mimc ATK semantics, but is not ODA */
		/*
		 * alignment 
		 */
		if (GetValueAttr(basic, cc_ALIGNMENT, (POINTER_type) & align, INHERIT)
		    == ERROR_INT) {
			sprintf(msgsbuf, "Couldn't get alignment %s", ErrorMsg(TKError));
			trcomment(msgsbuf);
			fprintf(stderr, "%s\n", msgsbuf);
			align = cc_ALIGNMENT_justified;
		}
		tralign(align);
#endif
	}
	for (i = 0; i < contents->length; i++) {
		rval = ProcessSingleCharCP((POINTER_type) font,
					contents->sequence_value.constituents[i], mode);
	}
LEAVE:
	free(offset);
	free(separation);
	return rval;
}

/*
 * mount all fonts necessary in this basic logical object
 */

blfonts( i, f)
int i;
cc_CHARACTER_FONTS_type f;
{
	int j = 1;
	int k;
	INT_PAIR_type *ipptr;
	int broken = 0;
	
	/*
	 * run around the ODA font cache twice, mounting the fonts
	 */
	do {
		for(k = i - 1; k >=0; k--){
			if(f != NULL_SEQUENCE){ /* sanity check */
				ipptr = &f->sequence_value.int_pairs[k];
				if(InvokeFont(ipptr->second, &trfonts) != SUCCESS) {
					sprintf(msgsbuf, "Couldn't invoke font %d", k);
					fprintf(stderr, "%s\n", msgsbuf);
				}
				if(trfonts.DoMount == YES) {
					if (!broken) {
						trbreak("break before mount");
						broken++;
					}
					trmount( &trfonts);
				}
			}
		}
	} while(j--);
}

/*
 * get the real text, ignore the fonts on the moment
 */
#define	FONTBUF 512
INT_type
ProcessSingleCharCP(font, content, mode)
cc_CHARACTER_FONTS_type font;
CONSTITUENT content;
INT_type mode ;
{
	INT_type rval = SUCCESS_INT;
	register int  i = 0;
	SEQUENCE_BYTE_type text;

/*	trcomment("Single Char CP"); */

	if (( rval = GetAttr(content, at_CONTENT_INFORMATION, (POINTER_type) & text,
	    LOCAL, IGNORE)) == ERROR_INT) {
		sprintf(msgsbuf, "No content local content info: %s",
						ErrorMsg(TKError));
		fprintf(stderr, "%s\n", msgsbuf);
		trcomment(msgsbuf);
		errexit(msgsbuf);
		goto LEAVE;
	}

	/*
	 * walk over the character contents
	 */
	while( i < text->length) {
		if (text->sequence_value.bytes[i] == ESC) {
			i += procescape(&text->sequence_value.bytes[i], font);
		} else if (text->sequence_value.bytes[i] == '\015') {
			if ( i >= text->length
			    || text->sequence_value.bytes[++i] != '\012') {
				sprintf(msgsbuf, "illegal naked <cr>");
				trcomment(msgsbuf);
				fprintf(stderr, "%s\n", msgsbuf);
			}
			/*
			 * hard newline sequence,
			 */
			trnewline();
			i++;
		} else
			trout(text->sequence_value.bytes[i++]);
	}
	/* fprintf(stderr, "done with text, deleting\n"); */
	if (DeleteSequence(text) == ERROR_INT)
		fprintf(stderr, "Couldn't delete text content info %s\n",
			ErrorMsg(TKError));
LEAVE:
	return rval;
}

/*
 * process escape sequences
 */

int
procescape( p, font)
register char *p;
cc_CHARACTER_FONTS_type font;
{
	register int i = 1;
	BOOL_type done = BOOL_false;
	char argbuf[BUFSIZ];
	register char *q = argbuf;

	/* sanity check */
	if(*p++ != ESC)
		errexit("Not an escape sequence");

	switch(*p) {
		case '[':	/* we know about this one */
			p++; i++;
			while( done == BOOL_false ) {
				*q++ = *p++;
				i++;
				switch( *p) {
					case SGR:	/* fontselect */
						*q = '\0';
						fontselect(argbuf, font);
						i++;
						done = BOOL_true;
						break;
					case ' ':
						i++;
						*q = '\0';
						if (*(p+1) != '^') {
							sprintf(msgsbuf,
								"Unknown <ESC[ %c>",
									*(p+1));
							errexit(msgsbuf);
						} else {
							i++;
							dotab(argbuf);
						}
						done = BOOL_true;
						break;
					default:
						if(i >= BUFSIZ) {
							errexit("Buf Overflow in procescape");
						}
				}
			}
			break;					
		default:
			sprintf(msgsbuf, "Unknown escape in character content %c", *p);
			trcomment(msgsbuf);
			i++;	
	}
	return i;
}

static
fontselect(p, font)
char *p;
cc_CHARACTER_FONTS_type font;
{
	int fontno;
	INT_PAIR_type *ipptr;

	fontno = atoi(p);
	/* sgr's from 10-19, sequence 0-9 */
	fontno -= 10;
	if ( font != NULL_SEQUENCE) {
		ipptr = &font->sequence_value.int_pairs[fontno];
		if (InvokeFont(ipptr->second, &trfonts)!= SUCCESS){
			sprintf(msgsbuf, "Couldn't invoke font");
			errexit(msgsbuf);
		}
		trfont(&trfonts);
	}
}

/*
 * we should actually do a STAB function, so go to the n'th tab stop
 * since the tab stops are not implemented, we just issue a tab to troff
 */
dotab(p)
char *p;
{
	trtab(atoi(p));
}
