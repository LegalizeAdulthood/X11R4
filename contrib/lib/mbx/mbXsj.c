
#ifndef lint
static char sccs[] = "@(#)mbXsj.c	1.2";
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
 * Name: mbXsj - make table of SJIS mapping's 
 *
 * Description:  These functions are used to build the mbXpc932.h translation
 *			index table used to map code points to font indexes.
 *			THis file is very specific to SJIS.  To support another code set
 *			another file would need to be built.
 *
 */

#include <stdio.h>
#include "mbX.h"

index = 256 ; 				/* starting index */
char byte1[256];
int mods = 0 ;			/* number of modifiers */

main()
{


unsigned i,j;

printf("{\n\n");

/*
 * Generate the 1st 256 code points - 
 *	some are modifiers others are glyph indexes
 */

for ( i = 0 ; i < 0x81 ; ++i ) PrintBaseIndex(i);
for ( i = 0x81 ; i < 0xa0 ; ++i ) PrintModifier(i) ; 
for ( i = 0xa0 ; i < 0xe0 ; ++i ) PrintBaseIndex(i) ;
for ( i = 0xe0 ; i < 0xfd ; ++i ) PrintModifier(i) ; 
for ( i = 0xfd ; i < 0x100 ; ++i ) PrintBaseIndex(i) ;

/* Generate a page for each modifier */

index = 256 ; 

for ( i = 0 ; i < mods ; ++i ) {

	printf("\n/* BEGIN PAGE %d OFFSET 0x%x CODE 0x%x%x INDEX 0x%x */\n\n",
				i+1,index - 0x40,byte1[i],0x40,index);

 	for ( j = 0x40 ; j < 0x7f ; ++j ) PrintIndex( byte1[i], j ) ;
	printf("0x0\t/* 0x7f */,\n"); ++index;
	for ( j = 0x80 ; j < 0xfd ; ++j ) PrintIndex( byte1[i], j ) ;

}

printf("\n/* LAST_INDEX = %d */\n",index);

printf("}\n\n");

exit(0);

}

unsigned int	JISCII[54] = {  0xe9cb, 0xe9f2, 0xe957, 0x9d98, 0xe27d,
			        0x9ff3, 0xe67c, 0xe8f2, 0xe1e6, 0xe541,
			        0xe8d5, 0xe6cb, 0x9ae2, 0xe1e8, 0x9e8d,
			        0x9fb7, 0xe78e, 0xe5a2, 0x9e77, 0x98d4,
			        0xe54d, 0xe244,
			        0x88b1, 0x89a7, 0x8a61, 0x8a68, 0x8a96,
				0x8ac1, 0x8ad0, 0x8c7a, 0x8d7b, 0x8ec7,
				0x9078, 0x9174, 0x92d9, 0x9376, 0x938e,
				0x9393, 0x93f4, 0x9488, 0x954f, 0x9699,
				0x96f7, 0x9855,
				0x8bc4, 0x968a, 0x9779, 0xe0f4, 0xea9f,
				0xeaa0, 0xeaa1, 0xeaa2, 0xfa54, 0xfa5b } ;

unsigned int	SJIS83[54] = {	0x88b1, 0x89a7, 0x8a61, 0x8a68, 0x8a96,
				0x8ac1, 0x8ad0, 0x8c7a, 0x8d8b, 0x8ec7,
				0x9078, 0x9174, 0x92d9, 0x9376, 0x938e,
				0x9393, 0x93f4, 0x9488, 0x954f, 0x9699,
				0x96f7, 0x9855,
				0xe9cb, 0xe9f2, 0xe579, 0x9d98, 0xe27d,
				0x9ff3, 0xe67c, 0xe8f2, 0xe1e6, 0xe541,
				0xe85d, 0xe6cb, 0x9ae2, 0xe1e8, 0x9e8d,
				0x9fb7, 0xe78e, 0xe5a2, 0x9e77, 0x98d4,
				0xe54d, 0xe244,
				0xea9f, 0xeaa0, 0xeaa1, 0xeaa2, 0x8bc4,
				0x968a, 0x9779, 0xe0f4, 0x81ca, 0x81e9 } ;

unsigned int	pc2sjis( pccode )
unsigned int	pccode;
{
	register int	i;
	unsigned int	sjis;

	sjis = pccode;
	for ( i=0; i < 54 ; i ++ ) {
	    if ( pccode == JISCII[i] ) {
		sjis = SJIS83[i];
		break;
	    }
	}
	return ( sjis );
}


/*
 *      sjtojis()
 *      shift jis to jis conversion routine.
 *      01/11/88
 */

char    *sjtojis(s1, s2)
char    *s1, *s2;
{
        int     c1, c2, p;
        char    *s0 = s1;

        while (c1 = *s2++) {
                c2 = *s2++;
                p = 0;
                if (c2 < 0x7f)
                        c2 -= 0x1f;
                else if (c2 < 0x9f)
                        c2 -= 0x20;
                else {
                        c2 -= 0x7e;
                        p++;
                }
                if (c1 < 0xa0)
                        *s1++ = ((c1 - 0x81) << 1) + 0x21 + p;
                else
                        *s1++ = ((c1 - 0xe0) << 1) + 0x5f + p;
                *s1++ = c2;
        }
        return s0;
}

PrintIndex(byte1,byte2)
char byte1, byte2;
{

char sj[4], jis[4];

#ifdef AIX

int sjis;

/*
 * For IBM code points 1st remap to standard Shifted-JIS code points.
 */
sjis = pc2sjis( (int)((byte1<<8) + byte2) );
byte1 = (sjis>>8) & 0xff; byte2 = sjis & 0xff;
#endif

/*
 * Do standard conversion Shifted-JIS to JIS 
 */
sj[0] = byte1;
sj[1] = byte2;
sj[2] = 0;
sj[3] = 0;

sjtojis(jis, sj);

#ifdef AIX
/*
 * Now, for IBM Selected Characters and IBM User Definable
 *    characters - make sure MSB = 0. 
 */
if ( jis[0] >= 0x80 ) 
	jis[0] -= 26;
#endif
printf("0x%2x%2x,\n",jis[0],jis[1]);
++index ; 

}

PrintModifier( i ) 
int i;
{

int offset;

byte1[mods++] = i ; 

offset = (index-0x40); 

printf("0x%x, /* OFFSET 0x%x CODE 0x%x40 */\n",offset|(INDMOD<<24), offset,i);

index += 0xfc - 0x40 + 1 ; 

}

PrintBaseIndex(i)
int i ;
{
#ifdef KANA_GL
if ( i > 0x7f ) 
	printf("0x%x,\n",i & 0x7f);
else
#endif
	printf("0x%x,\n",i);
}

