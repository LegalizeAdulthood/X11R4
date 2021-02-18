/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        SourceDsk.c
 **
 **   Project:     X Widgets
 **
 **   Description: Code for TextEdit widget disk source
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1987, 1988 by Digital Equipment Corporation, Maynard,
 **             Massachusetts, and the Massachusetts Institute of Technology,
 **             Cambridge, Massachusetts
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard, Digital or  M.I.T.  not be used in advertising or 
 **   publicity pertaining to distribution of the software without 
 **   written prior permission.
 **   
 **   DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 **   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 **   DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 **   ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 **   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 **   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 **   SOFTWARE.
 **   
 *****************************************************************************
 *************************************<+>*************************************/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/TextEdit.h>
#include <Xw/TextEditP.h>

extern char *tmpnam();
extern void bcopy();

/** private DiskSource definitions **/

typedef struct _DiskSourceData {
    unsigned char       *fileName;
    FILE *file;		
    XwTextPosition position, 	/* file position of first char in buffer */
 		  length; 	/* length of file */
    unsigned char *buffer;	/* piece of file in memory */
    int charsInBuffer;		/* number of bytes used in memory */
    XwEditType editMode;	/* append, read */
} DiskSourceData, *DiskSourcePtr;

#define bufSize 1000

#define Increment(data, position, direction)\
{\
    if (direction == XwsdLeft) {\
	if (position > 0) \
	    position -= 1;\
    }\
    else {\
	if (position < data->length)\
	    position += 1;\
    }\
}

static XtResource diskResources[] = {
    {XtNfile, XtCFile, XtRString, sizeof (char *),
        XtOffset(DiskSourcePtr, fileName), XtRString, ""},
    {XtNeditType, XtCEditType, XtREditMode, sizeof(int), 
        XtOffset(DiskSourcePtr, editMode), XtRString, "read"},
};

/*--------------------------------------------------------------------------+*/
static unsigned char Look(data, position, direction)
/*--------------------------------------------------------------------------+*/
  DiskSourcePtr   data;
  XwTextPosition  position;
  XwScanDirection direction;
{

    if (direction == XwsdLeft) {
	if (position == 0)
	    return('\n');
	else {
	    FillBuffer(data, position - 1);
	    return(data->buffer[position - data->position - 1]);
	}
    }
    else {
	if (position == data->length)
	    return('\n');
	else {
	    FillBuffer(data, position);
	    return(data->buffer[position - data->position]);
	}
    }
}



/*--------------------------------------------------------------------------+*/
int DiskReadText (src, pos, text, maxRead)
/*--------------------------------------------------------------------------+*/
  XwTextSource *src;
  XwTextPosition pos;	/** starting position */
  XwTextBlock *text;	/** RETURNED: text read in */
  int maxRead;		/** max number of bytes to read **/
{
    XwTextPosition count;
    DiskSourcePtr data;

    data = (DiskSourcePtr) src->data;
    FillBuffer(data, pos);
    text->firstPos = pos;
    text->ptr = data->buffer + (pos - data->position);
    count = data->charsInBuffer - (pos - data->position);
    text->length = (maxRead > count) ? count : maxRead;
    return pos + text->length;
}

/*
 * this routine reads text starting at "pos" into memory.
 * Contains heuristic for keeping the read position centered in the buffer.
 */
/*--------------------------------------------------------------------------+*/
static int FillBuffer (data, pos)
/*--------------------------------------------------------------------------+*/
  DiskSourcePtr data;
  XwTextPosition pos;
{
    long readPos;
    if ((pos < data->position ||
	    pos >= data->position + data->charsInBuffer - 100) &&
	    data->charsInBuffer != data->length) {
	if (pos < (bufSize / 2))
	    readPos = 0;
	else
	    if (pos >= data->length - bufSize)
		readPos = data->length - bufSize;
	    else
		if (pos >= data->position + data->charsInBuffer - 100)
		    readPos = pos - (bufSize / 2);
		else
		    readPos = pos;
	(void) fseek(data->file, readPos, 0);
	data->charsInBuffer = fread(data->buffer, sizeof(char), bufSize,
				data->file);
	data->position = readPos;
    }
}

/*
 * This is a dummy routine for read only disk sources.
 */
/*ARGSUSED*/  /* keep lint happy */
/*--------------------------------------------------------------------------+*/
static XwEditResult DummyReplaceText (src, startPos, endPos, text, delta)
/*--------------------------------------------------------------------------+*/
  XwTextSource *src;
  XwTextPosition startPos, endPos;
  XwTextBlock *text;
  int *delta;
{
    return(XweditError);
}


/*
 * This routine will only append to the end of a source.  If incorrect
 * starting and ending positions are given, an error will be returned.
 */
/*--------------------------------------------------------------------------+*/
static XwEditResult DiskAppendText (src, startPos, endPos, text, delta)
/*--------------------------------------------------------------------------+*/
  XwTextSource *src;
  XwTextPosition startPos, endPos;
  XwTextBlock *text;
  int *delta;
{
    long topPosition = 0;
    unsigned char *tmpPtr;
    DiskSourcePtr data;
    data = (DiskSourcePtr) src->data;
    if (startPos != endPos || endPos != data->length)
        return (XweditPosError);
    /* write the new text to the end of the file */
    if (text->length > 0) {
	(void) fseek(data->file, data->length, 0);
	(void) fwrite(text->ptr, sizeof(char), text->length, data->file);
    } else
	/* if the delete key was hit, blank out last char in the file */
	if (text->length < 0) {
		(void) fseek(data->file, data->length-1, 0);
		(void) fwrite(" ", sizeof(char), 1, data->file);
	}
    /* need this in case the application trys to seek to end of file. */
     (void) fseek(data->file, topPosition, 2);	
     
    /* put the new text into the buffer in memory */
    data->length += text->length;
    if (data->charsInBuffer + text->length <= bufSize) {
/**** NOTE: need to check if text won't fit in the buffer ***/
	if (text->length > 0) {
		tmpPtr = data->buffer + data->charsInBuffer;
		bcopy(text->ptr, tmpPtr, text->length);
	}
	data->charsInBuffer += text->length;
    } else
	FillBuffer(data, data->length - text->length);

    *delta = text->length;
    return (XweditDone);
}


/*--------------------------------------------------------------------------+*/
static int DiskSetLastPos (src, lastPos)
/*--------------------------------------------------------------------------+*/
  XwTextSource *src;
  XwTextPosition lastPos;
{
    ((DiskSourceData *)(src->data))->length = lastPos;
}

/*--------------------------------------------------------------------------+*/
static XwTextPosition DiskGetLastPos (src)
/*--------------------------------------------------------------------------+*/
  XwTextSource *src;
{
   return( ((DiskSourceData *)(src->data))->length );
}

/*
 * This routine will start at
 * the "pos" position of the source and scan in the appropriate
 * direction until it finds something of the right sType.  It returns 
 * the new position.  If upon reading it hits the end of the buffer
 * in memory, it will refill the buffer.
 */
/*--------------------------------------------------------------------------+*/
static XwTextPosition DiskScan (src, pos, sType, dir, count, include)
/*--------------------------------------------------------------------------+*/
  XwTextSource 	   *src;
  XwTextPosition   pos;
  XwScanType 	   sType;
  XwScanDirection  dir;
  int     	   count;
  Boolean	   include;
{
    DiskSourcePtr data;
    XwTextPosition position;
    int     i, whiteSpace;
    unsigned char    c;

    data = (DiskSourcePtr) src->data;
    position = pos;
    switch (sType) {
	case XwstPositions: 
	    if (!include && count > 0)
		count -= 1;
	    for (i = 0; i < count; i++) {
		Increment(data, position, dir);
	    }
	    break;
	case XwstWhiteSpace: 
	    for (i = 0; i < count; i++) {
		whiteSpace = 0;
		while (position >= 0 && position <= data->length) {
		    FillBuffer(data, position);
		    c = Look(data, position, dir);
		    whiteSpace = (c == ' ') || (c == '\t') || (c == '\n');
		    if (whiteSpace)
			break;
		    Increment(data, position, dir);
		}
		if (i + 1 != count)
		    Increment(data, position, dir);
	    }
	    if (include)
		Increment(data, position, dir);
	    break;
	case XwstEOL: 
	    for (i = 0; i < count; i++) {
		while (position >= 0 && position <= data->length) {
		    if (Look(data, position, dir) == '\n')
			break;
		    Increment(data, position, dir);
		}
		if (i + 1 != count)
		    Increment(data, position, dir);
	    }
	    if (include) {
	    /* later!!!check for last char in file # eol */
		Increment(data, position, dir);
	    }
	    break;
	case XwstLast: 
	    if (dir == XwsdLeft)
		position = 0;
	    else
		position = data->length;
    }
    return(position);
}


/*--------------------------------------------------------------------------+*/
static XwEditType DiskGetEditType(src)
/*--------------------------------------------------------------------------+*/
  XwTextSource *src;
{
    DiskSourcePtr data;
    data = (DiskSourcePtr) src->data;
    return(data->editMode);
}

/*--------------------------------------------------------------------------+*/
static Boolean XwDiskSourceCheckData(self)
/*--------------------------------------------------------------------------+*/
  Widget self;
{

}

/******* Public routines **********/

/*--------------------------------------------------------------------------+*/
void XwDiskSourceDestroy (src)
/*--------------------------------------------------------------------------+*/
  XwTextSource *src;
{
    DiskSourcePtr data;
    data = (DiskSourcePtr) src->data;
    XtFree((char *) data->buffer);
    XtFree((char *) src->data);
    XtFree((char *) src);
}

/*--------------------------------------------------------------------------+*/
XwTextSource *XwDiskSourceCreate(w, args, num_args)
/*--------------------------------------------------------------------------+*/
    Widget	w;
    ArgList	args;
    Cardinal	num_args;
{
    XwTextSource *src;
    DiskSourcePtr data;
    long topPosition = 0;

    src = XtNew(XwTextSource);
    src->read = DiskReadText;
    src->setLastPos = DiskSetLastPos;
    src->getLastPos = DiskGetLastPos;    
    src->scan = DiskScan;
    src->editType = DiskGetEditType;
    src->resources = diskResources;
    src->resource_num = XtNumber(diskResources);
    src->check_data = XwDiskSourceCheckData;
    src->destroy = XwDiskSourceDestroy;
    data = XtNew(DiskSourceData);
    src->data = (int *)data;

    /* Use the name given to the TextEdit widget this source will go with.
       This could be a problem if we allow multiple views on one source */

    XtGetSubresources (w, (caddr_t)data, XtNdiskSrc , "DiskSrc",
        diskResources, XtNumber(diskResources), args, num_args);

    /* NOTE:  Do not want to leave temp file around.  Must be unlinked
    somewhere */

    if (strcmp(data->fileName,NULL) == NULL)
	data->fileName = (unsigned char*)tempnam(NULL,"XwDSRC");

    switch (data->editMode) {
        case XwtextRead:
           if ((data->file = fopen(data->fileName, "r")) == 0)
                XtError("Cannot open source file in XwDiskSourceCreate");
            src->replace = DummyReplaceText;
            break;
        case XwtextAppend:
            if ((data->file = fopen(data->fileName, "a+")) == 0)
                XtError("Cannot open source file in XwDiskSourceCreate");
            src->replace = DiskAppendText;
            break;
        default:
            if ((data->file = fopen(data->fileName, "r")) == 0)
                XtError("Cannot open source file in XwDiskSourceCreate");
            src->replace = DummyReplaceText;
    }

    (void) fseek(data->file, topPosition, 2);  
    data->length = ftell (data->file);  
    data->buffer = (unsigned char *) XtMalloc((unsigned)bufSize);
    data->position = 0;
    data->charsInBuffer = 0;
    src->data = (int *) (data);
    return src;
}

