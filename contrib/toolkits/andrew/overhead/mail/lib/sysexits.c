/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/*
		sysexits.c -- Text messages for sysexits.
*/


char *EX_Messages[] = {

/*			 	 1234567891123456789212345	*/

	/* EX_USAGE */		"Command line usage error",
	/* EX_DATAERR */	"Data format error",
	/* EX_NOINPUT */	"Cannot open input",
	/* EX_NOUSER */		"Unknown recipient",
	/* EX_NOHOST */		"Unknown host",
	/* EX_UNAVAILABLE */	"Service unavailable",
	/* EX_SOFTWARE */	"Internal software error",
	/* EX_OSERR */		"Operating system error",
	/* EX_OSFILE */		"System file missing",
	/* EX_CANTCREAT */	"Can't create file",
	/* EX_IOERR */		"I/O error",
	/* EX_TEMPFAIL */	"Temporary failure",
	/* EX_PROTOCOL */	"Remote protocol error",
	/* EX_NOPERM */		"Permission denied"

};

int EX_Nerr = (sizeof EX_Messages / sizeof EX_Messages[0]);
