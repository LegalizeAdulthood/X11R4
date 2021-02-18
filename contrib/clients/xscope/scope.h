/* **********************************************
 *						*
 * header file for the Server spy scope           *
 *						*
 *	James Peterson, 1987			*
 *	(c) Copyright MCC, 1987 		*
 *						*
 ********************************************** */

#include <stdio.h>

#define Boolean short
#define true 1
#define false 0

/* ********************************************** */
/*                                                */
/* ********************************************** */

#define Assert(b) (b)
#define debug(n,f) (void)((debuglevel & n) ? (fprintf f,fflush(stderr)) : 0)
short   debuglevel;

/* ********************************************** */
/*                                                */
/* ********************************************** */

short   Verbose		  /* quiet (0) or increasingly verbose  ( > 0) */ ;


int     ScopePort;
char   *ScopeHost;

/* external function type declarations */

extern char   *Malloc ();
extern char *strcpy();
extern char *sprintf();
char   *ClientName ();

/* ********************************************** */
/*                                                */
/* ********************************************** */

/* need to change the MaxFD to allow larger number of fd's */
#define StaticMaxFD 32


#include "fd.h"
