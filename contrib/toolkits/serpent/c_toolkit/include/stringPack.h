/*************************************************************************

	C Programming Toolbox

	Module Name:	stringPack
	Purpose:	String input and manipulation package
	Author:		Robert A Zimmermann, Carnegie -Mellon University
			Department of Electrical and Computer Engineering

	History:	Notes:
	20 Jan 84	Began Creation of Module

	22 Feb 84	Created stringPack from layout program module
			lString
	
	25 Jun 84	Added type STRING

	13 Jul 84	Added recursion preventitive

 *************************************************************************/

#ifndef DID_STRINGPACK

#define DID_STRINGPACK

typedef char * STRING;

	extern STRING	getString();

	extern int	getInt();

	extern float	getReal();

	extern int	skipTo();

	extern int	peek();

	extern int	stringEQ();

	extern int	inString();

	extern STRING	newString();

	extern STRING	stringAPP();

	extern STRING	intToStr();

	extern int	strToInt();

	extern int	stringLEN();

	extern int	stringTable();

	extern int	endString();

#endif
