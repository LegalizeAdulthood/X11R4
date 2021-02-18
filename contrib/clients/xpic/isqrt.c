/* $Header: isqrt.c,v 1.2 89/07/02 15:58:27 moraes Exp $ */
/* 
 *  this isn't very much faster than sqrt( (double) x) on Sun-3/50s after
 *  making everything register (it was slower than sqrt before that!), but
 *  it saves having to link in the whole of the mathlib, (and will
 *  hopefully be faster on machines without floating point hardware - MM
 */

/* Faster integer square root test routine.
 * This is actually the same algorithm you learned in grade school and
 * have painfully rediscovered every five years since then.  Thanks to
 * Charles Mesztenyi for suggesting the binary analog.
 * Ben Cranston 4/23/87
 */
/*
 *                   umd5.UUCP    <= {seismo!mimsy,ihnp4!rlgvax}!cvl!umd5!zben
 * Ben Cranston zben@umd2.UMD.EDU    Kingdom of Merryland UniSys 1100/92
 *                   umd2.BITNET     "via HASP with RSCS"
 */

/* #define TEST */

int isqrt(n)
register int n;
{
    register int a,b,c,as,bs;

    a = 1;
    b = 1;
    while (a<=n) {
	a = a << 2;
	b = b << 1;
    }
    as = 0;
    bs = 0;
    while ( (b>1) && (n>0) ) {
	a = a>>2;
	b = b>>1;
	c = n - (as|a);
	if ( c>=0 ) {
	    n = c;
	    as |= (a<<1);
	    bs |= b;
	}
	as = as>>1;
    }

    return(bs);
}

#ifdef TEST
main(argc,argv)
int argc;
char **argv;
{
    int num,root,last;

    for ( num=0 ; num<65000 ; num++ ) {
		root = isqrt(num);
    }
    printf("Yow!  Are we having fun yet?\n");
}

#endif
