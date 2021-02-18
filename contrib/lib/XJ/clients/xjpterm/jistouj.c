/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*	@(#)jistouj.c	1.0	86/12/31	*/
/*LINTLIBRARY*/
/*
 * Convert jis-jcode strings s2 to ujis-jcode strings s1
 * Return s1.
 */
#include "jcode.h"

#define SS2	0x8e

char *
jistouj(s1, s2)
register char *s1, *s2;
{
	register char *os1 = s1;
	register int nki, nko, k = 0;

	nki = strlen(KI);
	nko = strlen(KO);
	while (*s2) {
		if (k) {
			if (!strncmp(s2, KO, nko)) {
				k = 0;
				s2 += nko;
			} else {
				*s1++ = (*s2++ | 0x80);
				*s1++ = (*s2++ | 0x80);
			}
		} else if (!strncmp(s2, KI, nki)) {
			k = 1;
			s2 += nki;
		} else {
			if ((unsigned char)*s2 >= 0xa1 && (unsigned char)*s2 <= 0xdf) 
				*s1++ = SS2;
			*s1++ = *s2++;
		}
	}
	*s1 = '\0';
	return(os1);
}
