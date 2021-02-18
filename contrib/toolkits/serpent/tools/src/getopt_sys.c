	static char rcsid [] = "$Header: /u/04c/mnt/new_integ/tools/src/RCS/getopt_sys.c,v 1.2 89/10/10 12:50:59 serpent Exp $";

/*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
$Log:	getopt_sys.c,v $
 * Revision 1.2  89/10/10  12:50:59  serpent
 * no change: wanted to see if it worked
 * 
 * Revision 1.1  89/10/10  12:49:05  serpent
 * Initial revision
 * 
\*--------------------------------------------------------------------------*/

/* This is similar to system getopt, but allows multiple params to 
   the flag. This is specified in the opts string by the number
   following the ':' (defaults to 1).

   i.e.   getopt cldgL:r:2p:3  args 
*/


#define NULL	0
#define EOF	(-1)

#define ERR(s, c)	if(opterr){\
	extern int strlen(), write();\
	char errbuf[2];\
	errbuf[0] = c; errbuf[1] = '\n';\
	(void) write(2, argv[0], (unsigned)strlen(argv[0]));\
	(void) write(2, s, (unsigned)strlen(s));\
	(void) write(2, errbuf, 2);}

extern int strcmp();
extern char *strchr();

int	opterr = 1;
int	optind = 1;
int	optopt;
char	*optarg;

int
getopt_sys(argc, argv, opts)
int	argc;
char	**argv, *opts;
{
	static int sp = 1;
	register int c;
	register char *cp;
	register int num_of_args;
	char  num_str[2];
	char  opt_arg_str[80];
	int i;

   	opt_arg_str[0] = num_str[1] = num_str[0] = '\0';  num_of_args = 0;

	if(sp == 1)
		if(optind >= argc ||
		   *(*(argv+optind)) != '-' || *(*(argv+optind)+1) == '\0')
			return(EOF);
		else if(strcmp( *(argv+optind), "--") == NULL) {
			optind++;
			return(EOF);
		}
	optopt = c = *(*(argv+optind)+sp);
	if(c == ':' || (cp=strchr(opts, c)) == NULL) {
		ERR(": illegal option -- ", c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {

		/* find the number of args to this flag */
	        if( (*(cp+1) >= '0') && (*(cp+1) <= '9') ) {
			strncat( num_str,++cp,1 );
			num_of_args = atoi(num_str);
		}
		else
			num_of_args = 1;



		/* this checks for the arg with no space between it and flag */
		if( *(*(argv+optind)+(sp+1)) != '\0') {
			optarg = &argv[optind++][sp+1];
			if ( num_of_args > 1 ) {
		          for( i=0; i < num_of_args; i++,optind++ ) {
			    if( (optind > argc) || ( *optarg == '-')) {
				ERR(": option requires 1 or more an arguments -- ", c);
				sp = 1;
				return('?');
			    } 
			    strcat( opt_arg_str,optarg );
			    strcat( opt_arg_str," " );
			    optarg = argv[optind];
			  } 
			  optarg = opt_arg_str;
			  --optind;
			}
			else {
			    if( (optind > argc) || ( *optarg == '-')) {
				ERR(": option requires 1 or more an arguments -- ", c);
				sp = 1;
				return('?');
			    } 
			}
 		}
		else if(++optind >= argc) 
		{
			ERR(": option requires an argument -- ", c);
			sp = 1;
			return('?');
		} 
		else {
		/* this checks for the arg with space between it and flag */
			optarg = argv[optind++];
			if ( num_of_args > 1 ) {
		          for( i=0; i < num_of_args; i++,optind++ ) {
			    if( (optind > argc) || ( *optarg == '-')) {
				ERR(": option requires 1 or more arguments -- ", c);
				sp = 1;
				return('?');
			    } 
			    strcat( opt_arg_str,optarg );
			    strcat( opt_arg_str," " );
			    optarg = argv[optind];
			  } 
			  optarg = opt_arg_str;
			  --optind;
			}
			else {
			    if( (optind > argc) || ( *optarg == '-')) {
				ERR(": option requires 1 or more an arguments -- ", c);
				sp = 1;
				return('?');
			    } 
			}
		}
		sp = 1;
	} 
	else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}
