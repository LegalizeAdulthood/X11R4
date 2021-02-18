/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ez/RCS/eza.ch,v 2.6 89/02/20 12:48:18 ghoti Exp $ */
/* $ACIS:eza.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ez/RCS/eza.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidezapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ez/RCS/eza.ch,v 2.6 89/02/20 12:48:18 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


struct ezapp_fileList {
    struct ezapp_fileList *next;
    char *filename;
    char *ObjectName;
    boolean newWindow;
    boolean readOnly;
};

class ezapp[eza]: application[app] {
    overrides:
	ParseArgs(int argc, char **argv) returns boolean;
        ReadInitFile();
	Start() returns boolean;
        Run() returns int;
    classprocedures:
        InitializeClass() returns boolean;
    data:
        struct ezapp_fileList *files,**fileLink;
        boolean initFile;
        boolean haveBufferInWindow;
        struct im *im;
        struct frame *frame;
        struct buffer *buffer;
        struct buffer *errorBuffer;
	char *defaultObject;
};
