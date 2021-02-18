/* ************************************************************ *\
 *								*
 *    Support routines for file descriptors (FD) 		*
 *								*
 *	James Peterson, 1987  	 				*
 *	(c) Copyright MCC, 1987	 				*
 * 				  				*
 * 				  				*
 \* *********************************************************** */

#include "scope.h"


/*
  All of this code is to support the handling of file descriptors (FD).
  The idea is to keep a table of the FDs that are in use and why.
  For each FD that is open for input, we keep the name of a procedure
  to call if input arrives for that FD.  When an FD is created
  (by an open, pipe, socket, ...) declare that by calling UsingFD.
  When it is no longer in use (close ...), call NotUsingFD.
*/

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

InitializeFD()
{
  register short  i;

  enterprocedure("InitializeFD");
  /* get the number of file descriptors the system will let us use */
  MaxFD = getdtablesize();

  /* allocate space for a File Descriptor (FD) Table */
  FDD = (struct FDDescriptor *)
    Malloc ((long)(MaxFD * sizeof (struct FDDescriptor)));

  /* be sure all fd's are closed and marked not busy */
  for (i = 0; i < MaxFD; i++)
    {
      /* 0, 1, 2 are special (stdin, stdout, stderr) */
      if (i > 2)
	close(i);
      FDD[i].Busy = false;
    }

  /* save one FD for single file input or output like debugging */
  /* also the getservbyname call is currently using an FD */
  MaxFD -= 4;

  nFDsInUse = 0 /* stdin, stdout, stderr */ ;
  ReadDescriptors = 0;
  HighestFD = 0;

  UsingFD(fileno(stdin),  (int (*)())NULL);
  UsingFD(fileno(stdout), (int (*)())NULL);
  UsingFD(fileno(stderr), (int (*)())NULL);
}

/* ************************************************************ */

UsingFD(fd, Handler)
     FD fd;
     int     (*Handler)();
{
  if (FDD[fd].Busy)
    NotUsingFD(fd);
  nFDsInUse += 1;

  FDD[fd].Busy = true;
  FDD[fd].InputHandler = Handler;
  if (Handler == NULL)
    ReadDescriptors &= ~(1 << fd) /* clear fd bit */ ;
  else
    ReadDescriptors |= 1 << fd /* set fd bit */ ;

  if (fd > HighestFD)
    HighestFD = fd;

  if (nFDsInUse >= MaxFD)
    panic("no more FDs");

  debug(128,(stderr, "Using FD %d, %d of %d in use\n", fd, nFDsInUse, MaxFD));
}

/* ************************************************************ */

NotUsingFD(fd)
     FD fd;
{
  debug(128,(stderr, "Not Using FD %d\n", fd));

  if (FDD[fd].Busy)
    nFDsInUse -= 1;

  FDD[fd].Busy = false;
  ReadDescriptors &= ~(1 << fd) /* clear fd bit */ ;

  while (!FDD[HighestFD].Busy && HighestFD > 0)
    HighestFD -= 1;

  debug(128,(stderr, "Highest FD %d, in use %d\n", HighestFD, nFDsInUse));
}

/* ************************************************************ */

EOFonFD(fd)
     FD fd;
{
  enterprocedure("EOFonFD");
  debug(128,(stderr, "EOF on %d\n", fd));
  close(fd);
  NotUsingFD(fd);
}


/* ************************************************************ */
/*								*/
/*     Main Loop -- wait for input from any source and Process  */
/*								*/
/* ************************************************************ */

#include <errno.h>	       /* for EINTR, EADDRINUSE, ... */
extern int  errno;


MainLoop()
{
  enterprocedure("MainLoop");

  while (true)
    {
      int     rfds, wfds, xfds;
      short   nfds;
      short   fd;

      /* wait for something */
      rfds = ReadDescriptors;
      wfds = 0;
      xfds = rfds;

      debug(128,(stderr, "select %d, rfds = 0%o\n", HighestFD + 1, rfds));
      nfds = select(HighestFD + 1, &rfds, &wfds, &xfds, NULL);
      debug(128,(stderr, "select nfds = 0%o, rfds = 0%o, 0%o, xfds 0%o\n",
		 nfds, rfds, wfds, xfds));

      if (nfds < 0)
	{
	  if (errno == EINTR)
	    continue /* to end of while loop */ ;
	  debug(1,(stderr, "Bad select - errno = %d\n", errno));
	  if (errno == EBADF)
	    {
	      /* one of the bits in rfds is invalid, close down
		 files until it goes away */
	      EOFonFD(HighestFD);
	      continue;
	    }

	  panic("Select returns error");
	  continue /* to end of while loop */ ;
	}

      if (nfds == 0)
	{
	  TimerExpired();
	  continue;
	}

      /* check each fd to see if it has input */
      for (fd = 0; 0 < nfds && fd <= HighestFD; fd++)
	{
	  /*
	    check all returned fd's; this prevents
	    starvation of later clients by earlier clients
	  */

	  if ((rfds & (1 << fd)) == 0)
	    continue;

	  nfds -= 1;

	  if (FDD[fd].InputHandler == NULL)
	    {
	      panic("FD selected with no handler");
	      debug(1,(stderr, "FD %d has NULL handler\n", fd));
	    }
	  else
	    (FDD[fd].InputHandler)(fd);
	}
    }
}
