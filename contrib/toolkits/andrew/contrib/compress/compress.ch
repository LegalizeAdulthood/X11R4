/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/compress/RCS/compress.ch,v 1.1 89/09/10 14:58:21 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/compress/RCS/compress.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_celview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/compress/RCS/compress.ch,v 1.1 89/09/10 14:58:21 ghoti Exp $ ";
#endif

/*RSKmod butchered from 'fnote.ch'*/

class compress: text {
classprocedures:
/*	UpdateAll(struct text *text,long count) returns int;*/
/*	CopyAll(struct text *text,struct text *desttext,long count,boolean number) returns int;*//*RSKmod remove*/
	InitializeObject(struct compress *self)returns boolean;
	FinalizeObject(struct compress *self);
	InitializeClass() returns boolean; 
	CurrentIndentation(struct text *txt,long pos) returns int;
methods:
/*	CopyNote(struct text *text,struct text * desttext,long count,boolean number) returns long;*//*RSKmod remove*/
/*	addenv(struct text *txt,long pos);*/
        GetStyle(struct text *txt);
/*	GetLocLength() returns long;*/
	Compress(struct text *txt,long pos,long len);
	NextCompressLoc(struct text *txt,long startpos,int ind) returns long;
	Decompress(struct text *txt);
	IsHidden(struct text *txt,long pos) returns boolean;
overrides:
	 Read (FILE *file, long id) returns long;
macromethods:
    GetLocatn() ((self)->locatn)
    SetLocatn(long newValue) ( ((self)->locatn) = (newValue) )
    GetCprsLength() ((self)->length)
    SetCprsLength(long newValue) ( ((self)->length) = (newValue) )
    GetLines() ((self)->lines)
    SetLines(long nlines) ( ((self)->lines) = (nlines) )
    GetParenttxt() ((self)->parenttext)
/*    GetEnv() ((self)->env)
    SetEnv(long newValue) ( ((self)->env) = (newValue) )*/
data:
    struct mark *cpmark;
    long lines;
    struct text *parenttext;
/**/struct textview *parentview;
    struct envlist *envl;
    struct style *compressed_style;
};

