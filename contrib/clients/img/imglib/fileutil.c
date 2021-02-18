/*

    Copyright (c) 1989 University of Southern California

	Paul Raveling
	USC Information Sciences Institute
	4676 Admiralty Way
	Marina del Rey, California 90292


	The authors grant without fee permission to use, copy, modify,
	distribute, and sell this software for any purpose, provided:

	--  The above copyright notice appears in all copies.

	--  This disclaimer appears in all source code copies.

	--  The names of ISI and USC are not used in advertising or
	    publicity  pertaining  to distribution  of the software
	    without prior specific written permission from ISI.

	ISI provides this software freely as a public service.  It is
	NOT a commercial product,  and therefore is not subject to an
	an implied  warranty  of  merchantability  or  fitness  for a
	particular purpose.  ISI provides it as is, without warranty.

	This software  is furnished  only on the basis that any party
	who receives it indemnifies  and holds harmless  the original
	authors and their sponsors  against any claims,  demands,  or
	liabilities  connected  with  using  it,   furnishing  it  to
	others, or providing it to a third party.


	The author  of this software is  Paul Raveling.   Development
	used the author's after-work hours and ISI's facilities.

	Thanks are due to ISI for allowing  such extracurricular work
	and  to Hewlett Packard,   whose  University  Grants  program
	provided  the 9000/370  workstation  and  supporting software
	which was the platform for this work.
*/

/*0
===============================================================================
=======                                                                 =======
=======		Module Fileutil  --  File-Related Utilities		=======
=======			     File fileutil.c				=======
=======                                                                 =======
===============================================================================


  Functions:

	Module fileutil implements some basic functions
	to help with finding and opening files on UNIX systems.



  Public routines:

	expand_path	Expand file pathname
	open_filter	Open filter
	close_filter	Close filter
	open_yafile	Open file
	close_yafile	Close file


  Local routines:

	none


  Module history:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-22-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes not documented in this header have been made
	within the last 3 weeks.

	3-16-89		Paul Raveling
	Created fileutil from a fragment of module utilmisc,
	which dates back to 3-3-87.
*/



#include <stdio.h>
#include <pwd.h>
#include <fcntl.h>
#include <imglib/imglib.h>


extern	struct passwd	*getpwuid ();
extern	struct passwd	*getpwnam ();	/*  System functions		*/
extern	char		*malloc ();




/*--------------------------------------------------------------*/
/*	Data shared by expand_path and internal routine gsb	*/
/*--------------------------------------------------------------*/

static	unsigned char	*sp;		/*  Pointer into csh_path	*/
static	unsigned char	*ep;		/*  Pointer into inserted text	*/
static	unsigned char	*dp;		/*  Pointer into full_path	*/
static	unsigned char	*sp0;		/*  Pointer to start of csh_path*/
static	unsigned char	*possenv;	/*  Pointer to start of possible
						environment variable	*/
static	unsigned char	 c;		/*  Current character		*/


/*1
===============================================================================
=======                                                                 =======
=======		     expand_path  --  Expand file pathname		=======
=======                                                                 =======
===============================================================================

  Invocation:

	full_path = expand_path ( csh_path )


  Functions:

	Expand a file's pathname from csh-acceptable form to
	the fully qualified pathname.  Although this involves
	the most common csh conventions, it does NOT include
	globbing (substition for *, ?, [...], etc.).

	Transformations applied to the pathname are:

	    --	Environment variables ($name) are expanded before
		all other substitutions.

	    --	"~/" at the beginning of the pathname translates
		to the current user's home directory.  This is
		expanded from the $home environment variable.

	    --	"~user/" at the beginning of the pathname translates
		to the named user's home directory.

	    --	"./" at the beginning of the pathname translates
		to the current working directory.

	    --  If the pathname does not begin with "/", "~", "~user",
		or "./", the current working directory name is
		prepended to it.

	    --	".." translates to the parent directory of the
		current point in the path.  If embedded within
		the path, it deletes the preceding directory name.


  Input:

	csh_path	unsigned char	*
			Pointer to pathname using csh conventions
			noted above.
			Format is a 0-terminated char string.


  Output:

	full_path	Byte  *
	<return value>	Pointer to full pathname expanded from csh_path.
			Format is a 0-terminated char string.
			"\0" => An environment variable was undefined
			or a user name in "~user" was invalid.

			****  This path string is statically allocated
			      within expand_path.  Therefore, any
			      full_path value becomes invalid at the
			      next call to this routine.
*/

/*======================================================*/
/*		Functions used by expand_path		*/
/*======================================================*/

/*------------------------------*/
/*	Get next source byte	*/
/*------------------------------*/

static	Byte gsb ()
{
#if 0		/*  Defined in module header, just mentioned here  */

/*--------------------------------------------------------------*/
/*	Data shared by expand_path and internal routine gsb	*/
/*--------------------------------------------------------------*/

static	Byte	*sp;		/*  Pointer into csh_path	*/
static	Byte	*ep;		/*  Pointer into inserted text	*/
static	Byte	*dp;		/*  Pointer into full_path	*/
static	Byte	*sp0;		/*  Pointer to start of csh_path*/
static	Byte	*possenv;	/*  Pointer to start of possible
						environment variable	*/
static	Byte	 c;		/*  Current character		*/

#endif



if ( ep != NULL )		/*  Fetching from inserted text?	*/
   {				/*	Yes -- ...			*/
   c = *ep++;				/*  Get next byte.		*/
   if ( c == '\\' )			/*  Is it '\'?			*/
      {					/*	Yes - 'quote' next byte	*/
      c = *ep++;
      return c;
      }
   if ( c != 0 )			/*  Is it end of insertion?	*/
      return c;				/*	No -- Return it.	*/
   else					/*	Yes - Switch back to	*/
      ep = NULL;			/*		standard source	*/
   }

c = *sp++;			/*  Get next byte from standard source.	*/

if ( c == '$' )			/*  Is it environment variable reference?  */
   {				/*	Yes -- Collect env var name	*/
   possenv = dp;		/*  Point possenv to start of name.	*/
   c = *sp++;
   while ( c == '\\' )
	 {
	 *dp++ = *sp++;
	 c     = *sp++;
	 }


   /*	Borrow space in full_path to collect environment variable name	*/

   while ((c != 0) && (c != '/') && (c != '$'))
	 {
	 *dp++ = c;
	 c = *sp++;
	 while ( c == '\\' )
	       {
	       *dp++ = *sp++;
	       c     = *sp++;
	       }
	 }

   --sp;			/*  Set to reread delimiter		*/
   *dp = 0;			/*  Terminate e.v. name			*/
   dp  =  possenv;		/*  "Release" borrowed space		*/
   ep = (Byte *) getenv ( possenv );	/*  Look up e.v.*/
   if ( ep == NULL )		/*  If there's no such env variable,	*/
      return 0xff;		/*     Use 0xff to signal failure	*/
   else				/*  Else ...				*/
      return gsb ();		/*     Begin returning env var value	*/
   }

if ( c == '\\' )		/*  Is it '\'?				*/
   c = *sp++;			/*	Yes -- Take next byte verbatim	*/

return c;
}


Byte	*expand_path ( csh_path )

Byte	*csh_path;

{
static	Byte	full_path [1000];	/*  Full path		*/
	int	 i;			/*  Utility integer	*/
	Byte	 lc;			/*  Local current char	*/
	Byte	 tild;			/*  Tilde-ref delimiter	*/
	Byte	*sep;			/*  Saved value of ep	*/
	Byte	*unptr;			/*  User name pointer	*/
	struct	passwd	*user_ref;	/*  User ref'd by ~xxx	*/


/*	Macro to get next byte, return "\0" if invalid	*/

#define	gpc  c=gsb();if (c==0xff) return (Byte *) "\0";



#if 0		/*  Defined in module header, just mentioned here  */

/*--------------------------------------------------------------*/
/*	Data shared by expand_path and internal routine gsb	*/
/*--------------------------------------------------------------*/

static	Byte	*sp;		/*  Pointer into csh_path	*/
static	Byte	*ep;		/*  Pointer into inserted text	*/
static	Byte	*dp;		/*  Pointer into full_path	*/
static	Byte	*sp0;		/*  Pointer to start of csh_path*/
static	Byte	*possenv;	/*  Pointer to start of possible
					environment variable	*/
static	Byte	 c;		/*  Current character		*/

#endif



/*------------------------------------------------------*/
/*	Initialize pointers, check 1st source byte	*/
/*------------------------------------------------------*/

sp0 = csh_path;			/*  Save start ptr to source string	*/
sp  = csh_path;			/*  Init source string pointer		*/
dp  = full_path;		/*  Init destination string pointer	*/
ep  = NULL;			/*  Init external text pointer		*/
possenv = NULL;			/*  Init possible env var pointer	*/


/*  Initialize full path string with path for current working directory	*/

#ifdef	SYSV
ep = (Byte *) getcwd ( dp, sizeof(full_path) - 2 );
#else
ep = (Byte *) getwd ( dp );
#endif

if ( ep != NULL )
   {
   while ( *dp++ != 0 ) ;	/*  Find end of string			*/
   -- dp;
   ep = NULL;

   if ( *(dp-1) != '/' )	/*  Be sure it ends with '/'		*/
      *dp++ = '/';
   }

gpc;				/*  Get first byte.			*/

switch ( c )			/*  Check special cases...		*/
	{
				/*------------------------------*/
	case '~':		/*  ~/ or ~name, probably	*/
				/*------------------------------*/

		sep = ep;	/*  Save ep to allow ~ via env vars	*/
		*dp++ = '~';	/*  ... In case this isn't user ref	*/

		gpc;		/*  Check byte after ~			*/

		if ((c == '/') || (c == 0))
		   {		/*  '~/' or just '~'	*/
		   tild = c;		/*  Save delimiter	*/
		   ep = (Byte *) getenv ( "home" );
		   if ( ep == NULL )
		      {
		      ep = (Byte *) getenv ( "HOME" );
		      if ( ep == NULL )
			 {
			 user_ref = getpwuid ( getuid () );
			 if ( user_ref != NULL )
			    ep = (Byte *) user_ref -> pw_dir;
			 if ( ep == NULL )
			    return (Byte *) "\0";
			 }
		      }
		   }
		else
		   {		/*  '~something' or '~someone'	*/
		   unptr = dp;
		   while ((c != 0) && (c != '/'))
			 {
			 *dp++ = c;
			 gpc;
			 }
		   tild = c;		/*  Save delimiter	*/
		   *dp = 0;

		   user_ref = getpwnam ( unptr );
		   if ( user_ref != NULL )
		      ep = (Byte *) user_ref -> pw_dir;
		   else
		      ep = NULL;
		   }

		if ( ep != NULL )	/*  If a ~someone dir was located  */
		   {			/*	replace current working	   */
		   dp = full_path;	/*	directory with this path   */
		   while ( 1 )
			 {
			 gpc;
			 if ( ep == NULL )
			    break;
			 *dp++ = c;
			 }
		   *dp++ = tild;	/*  Insert ~[user] delimiter	*/
		   }
		ep = sep;		/*  Restore saved input pointer	   */
		break;

				/*------------------------------*/
	case '.':		/*  './' or '..', probably	*/
				/*------------------------------*/

		gpc;		/*  Check 2nd byte	*/

		if ( c == '/')
		   {				/*  ./	*/
		   gpc;
		   }
		else
		   *dp++ = '.';

		break;

				/*------------------------------*/
	case '/':		/*  '/':  Fully qualified path	*/
				/*------------------------------*/

		dp = full_path;	/*  Discard current working dir path	*/
		break;

				/*------------------------------*/
	default:		/*  Other... Relative to cwd	*/
				/*------------------------------*/

		break;		/*  Already set up	*/
	}

while ( 1 )		/*  Copy remaining (caller-supplied) text  */
      {
      *dp++ = c;
      if ( c == 0 )
         break;

      gpc;

      if ((c == '/') || (c == 0))	/*  Check for <something>/../	*/
	 if (   ((dp-full_path) >= 3)
	     && (*(dp-1) == '.')
	     && (*(dp-2) == '.')
	     && (*(dp-3) == '/'))
	     {				/*  Delete preceding dir ref	*/
	     dp -= 3;			/*  Backtrack past '/..'	*/
	     *dp = 0;			/*  Make cur char non-'/'	*/
	     while ((dp != full_path) && (*dp != '/'))
		   --dp;		/*  Delete to preceding segment	*/
	     }
      }

*dp = 0;
return full_path;
}


/*1
===============================================================================
=======                                                                 =======
=======			open_filter --  Open a Filter			=======
=======                                                                 =======
===============================================================================

  Invocation:

	pid = open_filter (progargs, inpath, oupath, [&]instream, [&]oustream)


  Functions:

	Open an i/o filter.  The filter runs as a separate
	process whose stdin and stdout streams can be redirected
	at will.

	This is something like popen, but its calling sequence
	is slightly more versatile and it avoids the overhead
	of invoking a shell to run the program.  Its main liability
	is that the program's filename should be fully qualified:
	open_filter does NOT search the directories name in
	$path (for csh) or $PATH (for sh).

	**  It's advisable to use a fully qualified pathname
	**  in progargs[0] to identify the filter program.

	****  Warning:  This hasn't been tested completely.


  Input:

	progargs	char  *[]
			Pointer to array of command arguments.
			1st argument is pathname of program to execute;
			last argument is NULL.

	inpath		char  *
			String defining pathname of file to use as
			input to the filter program;
			NULL => open pipe to filter

	oupath		char  *
			String defining pathname of file to use as
			output from the filter program;
			NULL => open pipe from filter

  Output:

	pid		int
	<return value>	Process id of filter process;
			0 => Failed, couldn't load program or open pipe(s).

	*instream	FILE  *
			If inpath is given as NULL, open_filter
			returns a FILE pointer to a pipe (open for output)
			that connects to the filter's input.

	*iostream	FILE  *
			If oupath is given as NULL, open_filter
			returns a FILE pointer to a pipe (open for input)
			that connects to the filter's output.

*/


int	 open_filter (arguments, inpath, oupath, instream, oustream)

	char	*arguments[];
	char	*inpath;
	char	*oupath;
	FILE   **instream;
	FILE   **oustream;

{
	int	 filtpid;		/*  Filter process id		*/
	int	 filtinpipe[2];		/*  Filter input pipe		*/
	int	 filtinfile;		/*  Filter input file (fd)	*/
	int	 filtoupipe[2];		/*  Filter output pipe		*/
	int	 filtoufile;		/*  Filter output file (fd)	*/
	int	 status;


if ( inpath == NULL )
   {					/*  Open filter input pipe	*/
   status = pipe (filtinpipe);
   if ( status == -1 )
      return 0;
   filtinfile = filtinpipe[0];
   *instream = fdopen (filtinpipe[1], "w");
   if ( *instream == NULL )
      return 0;
   }
else
   {					/*  Open filter input file	*/
   filtinfile = open ( inpath, O_RDONLY );
   if ( filtinfile == -1 )
      return 0;
   }

if ( oupath == NULL )
   {					/*  Open filter output pipe	*/
   status = pipe (filtoupipe);
   if ( status == -1 )
      return 0;
   filtoufile = filtoupipe[1];
   *oustream = fdopen (filtoupipe[0], "r");
   if ( *oustream == NULL )
      return 0;
   }
else
   {					/*  Open filter output file	*/
   filtoufile = open ( inpath, O_WRONLY | O_CREAT );
   if ( filtoufile == -1 )
      return 0;
   }


filtpid = vfork ();			/*  Create filter process	*/
if ( filtpid == -1 )
   return 0;

if ( filtpid == 0 )
   {				/*  Filter process:			*/
   close (0);
   dup2  (filtinfile, 0);		/*	Set stdin		*/
   close (filtinfile);

   close (1);
   dup2  (filtoufile, 1);		/*	Set stdout		*/
   close (filtoufile);

   execv (arguments[0], arguments);	/*	Invoke filter program	*/
   }
else
   return filtpid;		/*  Main process:  Return filter pid	*/
}


/*1
===============================================================================
=======                                                                 =======
=======			open_yafile --  Open a File			=======
=======                                                                 =======
===============================================================================

  Invocation:

	yafile = open_yafile ( filename, options, suffix )


  Functions:

	Open a file.  If the given filename can't be opened,
	try it with default suffixes appended.  If any suffix
	ending with ".Z" works, open the file by piping it
	through either compress or uncompress.

	Suffix defaulting attempts to open the following names
	in this order until an open succeeds:

		1.  filename
		2.  filename.Z
		3.  filename.default_suffix
		4.  filename.default_suffix.Z

	If the original filename already ends with ".Z",
	only the caller's original filename is used.  Also, if
	the options specify "w" to write a file, the given filename
	will be the one used unless a file of that name exists and
	its protection does not allow write access to the caller.

	Information returned includes the actual filename opened
	and a flag indicating whether the file is compressed.
	In the case of compressed files, the file the caller sees
	is actually a pipe to compress or from uncompress.

	A file opened in this manner can be read or written as
	if the "yafile" is a FILE, but it must be closed with
	close_yafile.


  Input:

	filename	char *
			Pointer to filename, or at least the root
			of the filename.  Name can include csh'ish
			things, such as ~[user] and $environment_var.

	options		char *
			Options for openf.  We expect "r" or "w";
			anything else may or may not work.

	suffix		char *
			Default suffix to try if filename can't be opened.
			NULL => no default suffix


  Output:

	yafile		Yafile *
	<return value>	Pointer to Yafile info.  Caller can treat this
			as a FILE * if he doesn't want to check the file's
			actual name or its compressed/uncompressed status.
*/



Yafile *open_yafile ( filename, options, suffix )

	char	*filename;	/*  Pointer to filename	[sub]string	*/
	char	*options;	/*  Pointer to fopen options		*/
	char	*suffix;	/*  Pointer to optional suffix		*/

{
	Yafile	*yafile;	/*  Pseudo-FILE to return		*/
	FILE	*realfile;	/*  Real file info			*/
	char	*p;		/*  Pointer into filename string	*/
	char	*s;		/*  Source ptr for copying strings	*/
	char	*fullname;	/*  Full (expanded) filename		*/
	char	*command;	/*  Buffer for compress/uncompress cmd	*/
	int	 baslen;	/*  Length of basic filename, expanded	*/
	int	 suflen;	/*  Length of caller's suffix		*/
	int	 fullsize;	/*  Size of space for full filename	*/
	int	 i;		/*  Counter				*/

static	char	*uncompargs[2]	= {"/usr/bin/uncompress",NULL};
static	char	*compargs[2]	= {"/usr/bin/compress",NULL};



/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to open compress or uncompress pipe	*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#if 1		/*  Use old version for now	*/
#define	opencomp { \
 yafile -> compressed = 1; \
 if (*options == 'r') \
	{ \
	sprintf (command, "uncompress <%s", fullname); \
        realfile = popen (command,options); \
	} \
 else \
	{ \
	sprintf (command, "compress >%s", fullname); \
	realfile = popen (command,options); \
	} }
#else
#define	opencomp { \
 yafile -> compressed = 1; \
 if (*options == 'r') \
	open_filter (uncompargs, fullname, NULL, NULL, &realfile); \
 else \
	open_filter (compargs, NULL, fullname, &realfile, NULL); \
 }
#endif


/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to return the file or pipe just opened	*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#define	retreal { \
 yafile -> real_stream = *realfile; \
 free (command); \
 return yafile; }



/*-------------------------------------------*/
/*	Allocate Yafile, set up strings	     */
/*-------------------------------------------*/

s = (char *) expand_path (filename);	/*  Expand filename's path	*/
baslen = strlen (s);			/*  Find its size		*/

if ( suffix == NULL )			/*  Identify size of suffix	*/
   suflen = 0;
else
   suflen = strlen (suffix);

fullsize = baslen + suflen + 4;		/*  Size for name.suffix.Z\0	*/

yafile = (Yafile *) malloc (sizeof(Yafile));  /*  Allocate memory for	*/
if ( yafile == NULL )			/*	  a Yafile		*/
   {
   fprintf ( stderr, "open_yafile:  Insufficient memory to open file\n" );
   return NULL;
   }

p = malloc (fullsize);			/*  Allocate memory for		*/
if ( p == NULL )			/*  max length filename		*/
   {
   fprintf ( stderr, "open_yafile:  Insufficient memory to open file\n" );
   free (yafile);
   return NULL;
   }

fullname = p;				/*  Set ptr to full name string	*/
yafile -> actual_name = p;		/*  Save in yafile info		*/
i = baslen;
while ( i-- > 0 )			/*  Copy "basic" filename	*/
      *p++ = *s++;

*p = 0;					/*  Terminate it as asciz	*/



/*------------------------------------------------------------------*/
/*	Open alternative filenames until one works or all fail	    */
/*------------------------------------------------------------------*/

command = malloc ( fullsize + 15 );	/*  Get space for compress/
						uncompress command	*/
if ( command == NULL )
   {
   fprintf ( stderr, "open_yafile:  Insufficient memory to open file\n" );
   free (fullname);
   free (yafile);
   return NULL;
   }

if ( (*(p-1) == 'Z') && (*(p-2) == '.'))	/*  If original name	*/
   {						/*  ends with ".Z"	*/
   opencomp					/*     open ONLY it	*/
   if ( realfile != NULL )			/*     as compressed	*/
      retreal
   }

/*	Try 1st normal alterative:  filename	    (not compressed)	*/

realfile = fopen (fullname, options);
yafile -> compressed = 0;
if ( realfile != NULL )
   retreal

/*	Try 2nd normal alterative:  filename.Z		(compressed)	*/

*p++ = '.';
*p++ = 'Z';				/*  Append '.Z'	*/
*p = 0;
realfile = fopen (fullname, options);
if ( realfile != NULL )			/*  If file can be opened	*/
   {					/*	close it & open pipe	*/
   fclose (realfile);
   opencomp
   retreal
   }


/*	Try 3rd normal alterative:  filename.suffix  (not compressed)	*/

p -= 2;
s = suffix;
while ( *s != 0 )			/*  Replace '.Z' with suffix	*/
      *p++ = *s++;
*p = 0;
realfile = fopen (fullname, options);
yafile -> compressed = 0;
if ( realfile != NULL )
   retreal


/*	Try 4th normal alterative:  filename.suffix.Z    (compressed)	*/

*p++ = '.';
*p++ = 'Z';				/*  Append '.Z'	*/
*p = 0;
realfile = fopen (fullname, options);
if ( realfile != NULL )			/*  If file can be opened	*/
   {					/*	close it & open pipe	*/
   fclose (realfile);
   opencomp
   retreal
   }

free (command);			/*  No go -- Free stuff, return NULL	*/
free (fullname);
free (yafile);
return NULL;
}


/*1
===============================================================================
=======                                                                 =======
=======			close_yafile --  Close a File			=======
=======                                                                 =======
===============================================================================

  Invocation:

	close_yafile ( yafile )


  Functions:

	Close a file that was opened with open_yafile.

	This closes either a file or a pipe, each of which of course
	involves a different interface to UNIX, and releases memory
	associated with the Yafile structure.


  Input:

	yafile		Yafile *
			Pointer to Yafile info.  As with other stuff,
			caller can treat this as a FILE *.


  Output:	None
*/



void	 close_yafile ( yafile )

	Yafile	*yafile;	/*  Pointer to "enhanced" stream info	*/

{
/*-------------------------------------------------*/
/*	Close file or pipe and release memory	   */
/*-------------------------------------------------*/

if ( yafile -> compressed )		/*  If file is compressed	*/
   pclose ( yafile );			/*     close pipe		*/
else					/*  else			*/
   fclose ( yafile );			/*     close file		*/

free ( yafile -> actual_name );		/*  Release actual filename	*/
free ( yafile );			/*  Release file into		*/
}
