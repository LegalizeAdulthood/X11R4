/* ************************************************** *
 *						      *
 *  Code to decode and print X11 protocol	      *
 *						      *
 *	James Peterson, 1988			      *
 *	(c) Copyright MCC, 1988 		      *
 *						      *
 * ************************************************** */

#include "scope.h"
#include "x11.h"

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

ReportFromClient(fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  PrintTime();
  fprintf(stdout, "Client%s --> %4d %s\n",
	  ClientName(fd), n, (n == 1 ? "byte" : "bytes"));
  ProcessBuffer(fd, buf, n);
}

ReportFromServer(fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  PrintTime();
  fprintf(stdout, "\t\t\t\t\t%4d %s <-- X11 Server%s\n",
	  n, (n == 1 ? "byte" : "bytes"), ClientName(fd));
  ProcessBuffer(fd, buf, n);
}


/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

#include <sys/time.h>	       /* for struct timeval * */
static long ZeroTime1 = -1;
static long ZeroTime2 = -1;
static struct timeval   tp;

/* print the time since we started in hundredths (1/100) of seconds */

PrintTime()
{
  static long lastsec = 0;
  long    sec /* seconds */ ;
  long    hsec /* hundredths of a second */ ;

  gettimeofday(&tp, (struct timezone *)NULL);
  if (ZeroTime1 == -1 || (tp.tv_sec - lastsec) >= 1000)
    {
      ZeroTime1 = tp.tv_sec;
      ZeroTime2 = tp.tv_usec / 10000;
    }

  lastsec = tp.tv_sec;
  sec = tp.tv_sec - ZeroTime1;
  hsec = tp.tv_usec / 10000 - ZeroTime2;
  if (hsec < 0)
    {
      hsec += 100;
      sec -= 1;
    }
  fprintf(stdout, "%2d.%02ld: ", sec, hsec);
}

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* we will need to be able to interpret the values stored in the
   requests as various built-in types.  The following routines
   support the types built into X11 */

long    pad (n)
     long    n;
{
  /* round up to next multiple of 4 */
  return((n + 3) & ~0x3);
}

extern int  littleEndian;

unsigned long    ILong (buf)
     unsigned char   buf[];
{
  if (littleEndian)
    return((((((buf[3] << 8) | buf[2]) << 8) | buf[1]) << 8) | buf[0]);
  return((((((buf[0] << 8) | buf[1]) << 8) | buf[2]) << 8) | buf[3]);
}

unsigned short   IShort (buf)
unsigned char   buf[];
{
  if (littleEndian)
    return (buf[1] << 8) | buf[0];
  return((buf[0] << 8) | buf[1]);
}

unsigned short   IByte (buf)
unsigned char   buf[];
{
  return(buf[0]);
}

Boolean IBool(buf)
     unsigned char   buf[];
{
  if (buf[0] != 0)
    return(true);
  else
    return(false);
}


/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* we will need to save bytes until we get a complete request to
   interpret.  The following procedures provide this ability */

SaveBytes(fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  /* check if there is enough space to hold the bytes we want */
  if (CS[fd].NumberofSavedBytes + n > CS[fd].SizeofSavedBytes)
    {
      /* not enough room so far; malloc more space and copy */
      long    SizeofNewBytes = (CS[fd].NumberofSavedBytes + n + 1);
      unsigned char   *NewBytes = (unsigned char *)Malloc (SizeofNewBytes);
      bcopy(/* from  */(char *)CS[fd].SavedBytes,
	    /* to    */(char *)NewBytes,
	    /* count */(int)CS[fd].SizeofSavedBytes);
      Free((char *)CS[fd].SavedBytes);
      CS[fd].SavedBytes = NewBytes;
      CS[fd].SizeofSavedBytes = SizeofNewBytes;
    }

  /* now copy the new bytes onto the end of the old bytes */
  bcopy(/* from  */(char *)buf,
	/* to    */(char *)(CS[fd].SavedBytes + CS[fd].NumberofSavedBytes),
	/* count */(int)n);
  CS[fd].NumberofSavedBytes += n;
}

RemoveSavedBytes(fd, n)
     FD fd;
     long    n;
{
  /* check if all bytes are being removed -- easiest case */
  if (CS[fd].NumberofSavedBytes <= n)
    CS[fd].NumberofSavedBytes = 0;
  else if (n == 0)
    return;
  else
    {
      /* not all bytes are being removed -- shift the remaining ones down  */
      register unsigned char  *p = CS[fd].SavedBytes;
      register unsigned char  *q = CS[fd].SavedBytes + n;
      register long   i = CS[fd].NumberofSavedBytes - n;
      while (i-- > 0)
	*p++ = *q++;
      CS[fd].NumberofSavedBytes -= n;
    }
}


/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */


/* following are the possible values for ByteProcessing */
/* forward declarations */
long    StartSetUpMessage ();
long    FinishSetUpMessage ();
long    StartRequest ();
long    FinishRequest ();

long    StartSetUpReply ();
long    FinishSetUpReply ();
long    ServerPacket ();
long    FinishReply ();


/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

int littleEndian;

ProcessBuffer(fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  unsigned char   *BytesToProcess;
  long    NumberofUsedBytes;

  /* as long as we have enough bytes to do anything -- do it */

  littleEndian = CS[fd].littleEndian;
  while (CS[fd].NumberofSavedBytes + n >= CS[fd].NumberofBytesNeeded)
    {
      /*
	we have enough bytes to do something.  We want the bytes to be
	grouped together into one contiguous block of bytes. We have three
	cases:

	(1) NumberofSavedBytes = 0; so all needed bytes are in the
	read buffer, buf.

	(2) NumberofSavedBytes >= NumberofBytesNeeded;	in this case we
	will not need to copy any extra bytes into the save buffer.

	(3) 0 < NumberofSavedBytes < NumberofBytesNeeded; so
	some bytes are in the save buffer and others are in the read
	buffer.  In this case we need to copy some of the bytes from the
	read buffer to the save buffer to get as many bytes as we need,
	then use these bytes.
      */

      if (CS[fd].NumberofSavedBytes == 0)
	{
	  /* no saved bytes, so just process the first bytes in the
	     read buffer */
	  BytesToProcess = buf /* address of request bytes */;
	}
      else
	{
	  if (CS[fd].NumberofSavedBytes < CS[fd].NumberofBytesNeeded)
	    {
	      /* first determine the number of bytes we need to
		 transfer; then transfer them and remove them from
		 the read buffer. (there may be additional requests
		 in the read buffer) */
	      long    m;
	      m = CS[fd].NumberofBytesNeeded - CS[fd].NumberofSavedBytes;
	      SaveBytes(fd, buf, m);
	      buf += m;
	      n -= m;
	    }
	  BytesToProcess = CS[fd].SavedBytes /* address of request bytes */;
	}

      /*
	BytesToProcess points to a contiguous block of NumberofBytesNeeded
	bytes that we should process.  The type of processing depends upon
	the state we are in. The processing routine should return the
	number of bytes that it actually used.
      */
      NumberofUsedBytes = (*CS[fd].ByteProcessing)
                             (fd, BytesToProcess, CS[fd].NumberofBytesNeeded);

      /* the number of bytes that were actually used is normally (but not
	 always) the number of bytes needed.  Discard the bytes that were
	 actually used, not the bytes that were needed. The number of used
	 bytes must be less than or equal to the number of needed bytes. */

      if (NumberofUsedBytes > 0)
	{
	  if (CS[fd].NumberofSavedBytes > 0)
	    RemoveSavedBytes(fd, NumberofUsedBytes);
	  else
	    {
	      /* there are no saved bytes, so the bytes that were
		 used must have been in the read buffer */
	      buf += NumberofUsedBytes;
	      n -= NumberofUsedBytes;
	    }
	}
    } /* end of while (NumberofSavedBytes + n >= NumberofBytesNeeded) */

  /* not enough bytes -- just save the new bytes for more later */
  if (n > 0)
    SaveBytes(fd, buf, n);
  return;
}



/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */
/*
  Byte Processing Routines.  Each routine MUST set NumberofBytesNeeded
  and ByteProcessing.  It probably needs to do some computation first.
*/


StartClientConnection(fd)
     FD fd;
{
  enterprocedure("StartClientConnection");
  /* when a new connection is started, we have no saved bytes */
  CS[fd].SavedBytes = NULL;
  CS[fd].SizeofSavedBytes = 0;
  CS[fd].NumberofSavedBytes = 0;

  /* when a new connection is started, we have no reply Queue */
  FlushReplyQ(fd);

  /* each new connection gets a request sequence number */
  CS[fd].SequenceNumber = 0;

  /* we need 12 bytes to start a SetUp message */
  CS[fd].ByteProcessing = StartSetUpMessage;
  CS[fd].NumberofBytesNeeded = 12;
}

StopClientConnection(fd)
     FD fd;
{
  enterprocedure("StopClientConnection");
  /* when a new connection is stopped, discard the old buffer */

  if (CS[fd].SizeofSavedBytes > 0)
    Free((char*)CS[fd].SavedBytes);
}

long    StartSetUpMessage (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  short   namelength;
  short   datalength;

  enterprocedure("StartSetUpMessage");
  /*
    we need the first 12 bytes to be able to determine if, and how many,
    additional bytes we need for name and data authorization.  However, we
    can't process the first 12 bytes until we get all of them, so
    return zero bytes used, and increase the number of bytes needed
  */

  namelength = IShort(&buf[6]);
  datalength = IShort(&buf[8]);
  CS[fd].ByteProcessing = FinishSetUpMessage;
  CS[fd].NumberofBytesNeeded = n
                               + pad((long)namelength) + pad((long)datalength);
  debug(8,(stderr, "need %d bytes to finish startup\n",
	   CS[fd].NumberofBytesNeeded - n));
  return(0);
}

long    FinishSetUpMessage (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  enterprocedure("FinishSetUpMessage");
  CS[fd].littleEndian = (buf[0] == 'l');
  CS[ServerHalf(fd)].littleEndian = CS[fd].littleEndian;
  littleEndian = CS[fd].littleEndian;
  PrintSetUpMessage(buf);

  /* after a set-up message, we expect a string of requests */
  CS[fd].ByteProcessing = StartRequest;
  CS[fd].NumberofBytesNeeded = 4;
  return(n);
}


long    StartRequest (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  short   requestlength;
  enterprocedure("StartRequest");

  /* bytes 0,1 are ignored now; bytes 2,3 tell us the request length */
  requestlength = IShort(&buf[2]);
  CS[fd].ByteProcessing = FinishRequest;
  CS[fd].NumberofBytesNeeded = 4 * requestlength;
  debug(8,(stderr, "need %d more bytes to finish request\n",
	   CS[fd].NumberofBytesNeeded - n));
  return(0);
}


long    FinishRequest (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  enterprocedure("FinishRequest");
  DecodeRequest(fd, buf, n);
  CS[fd].ByteProcessing = StartRequest;
  CS[fd].NumberofBytesNeeded = 4;
  return(n);
}

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

StartServerConnection(fd)
     FD fd;
{
  enterprocedure("StartServerConnection");
  /* when a new connection is started, we have no saved bytes */
  CS[fd].SavedBytes = NULL;
  CS[fd].SizeofSavedBytes = 0;
  CS[fd].NumberofSavedBytes = 0;

  /* when a new connection is started, we have no reply Queue */
  FlushReplyQ(fd);

  /* we need 8 bytes to start a SetUp reply */
  CS[fd].ByteProcessing = StartSetUpReply;
  CS[fd].NumberofBytesNeeded = 8;
}

StopServerConnection(fd)
     FD fd;
{
  enterprocedure("StopServerConnection");
  /* when a new connection is stopped, discard the old buffer */

  if (CS[fd].SizeofSavedBytes > 0)
    Free((char *)CS[fd].SavedBytes);
}

long    StartSetUpReply (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  short   replylength;

  enterprocedure("StartSetUpReply");
  replylength = IShort(&buf[6]);
  CS[fd].ByteProcessing = FinishSetUpReply;
  CS[fd].NumberofBytesNeeded = n + 4 * replylength;
  debug(8,(stderr, "need %d bytes to finish startup reply\n",
	   CS[fd].NumberofBytesNeeded - n));
  return(0);
}

long    FinishSetUpReply (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  enterprocedure("FinishSetUpReply");
  PrintSetUpReply(buf);
  CS[fd].ByteProcessing = ServerPacket;
  CS[fd].NumberofBytesNeeded = 32;
  return(n);
}

/* ************************************************************ */

long    ErrorPacket (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  fprintf(stdout, "Error: ");
  DecodeError(fd, buf, n);
  CS[fd].ByteProcessing = ServerPacket;
  CS[fd].NumberofBytesNeeded = 32;
  return(n);
}


long    EventPacket (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  DecodeEvent(fd, buf, n);
  CS[fd].ByteProcessing = ServerPacket;
  CS[fd].NumberofBytesNeeded = 32;
  return(n);
}


long    ReplyPacket (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  short   replylength;

  replylength = ILong(&buf[4]);

  /*
    Replies may need more bytes, so we compute how many more
    bytes are needed and ask for them, not using any of the bytes
    we were given (return(0) to say that no bytes were used).
    If the replylength is zero (we don't need any more bytes), the
    number of bytes needed will be the same as what we have, and
    so the top-level loop will call the next routine immediately
    with the same buffer of bytes that we were given.
  */

  CS[fd].ByteProcessing = FinishReply;
  CS[fd].NumberofBytesNeeded = n + 4 * replylength;
  debug(8,(stderr, "need %d bytes to finish reply\n", (4 * replylength)));
  return(0);
}

long    ServerPacket (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  short   PacketType;
  enterprocedure("ServerPacket");

  PacketType = IByte(&buf[0]);
  if (PacketType == 0)
    return(ErrorPacket(fd, buf, n));
  if (PacketType == 1)
    return(ReplyPacket(fd, buf, n));
  return(EventPacket(fd, buf, n));
}

long    FinishReply (fd, buf, n)
     FD fd;
     unsigned char *buf;
     long    n;
{
  enterprocedure("FinishReply");
  DecodeReply(fd, buf, n);
  CS[fd].ByteProcessing = ServerPacket;
  CS[fd].NumberofBytesNeeded = 32;
  return(n);
}
