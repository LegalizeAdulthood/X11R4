/*************************************************************************

	C Programmers' Toolbox

	Module Name:	mathPack
	Purpose:	routines for math functions

	Author:		Robert A Zimmermann, Carnegie -Mellon University
			Department of Electrical and Computer Engineering

	History:	Notes:

	23 Feb 84	Created module out of routines from the layout
			system module lUtils.

 *************************************************************************

 routines

 int	abs( a )	return the absolute value of a

 int	min( a,b )	returns the lesser of a, b

 int	max( a,b )	returns the greate of a, b

 int	inRange( a, r0, r1 )	returns 'is a in the range r0..r1 or r1..r0'?


/*
 * Constants and external routines
 */

#define DEBUG	0



/*
 * General arithmetic utilities
 */

int inRange( a, a1, a2 )
int a, a1, a2;
    { return (a < a1) ? (a >= a2) : (a <= a2 || a == a1); }


int min( a,b )
int a,b;
    { return (a < b) ? a : b; }
	

int max( a,b )
int a,b;
    { return (a > b) ? a : b; }

int abs( a )
int a;
    { return (a > 0) ? a : -a; }
