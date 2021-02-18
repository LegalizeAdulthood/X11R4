/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*  $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/runadewa.ch,v 1.1 89/10/03 14:10:46 tpn Exp $ */
/* $ACIS:eza.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/runadewa.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidrunadewa_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/runadewa.ch,v 1.1 89/10/03 14:10:46 tpn Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


struct runadewapp_fileList {
    struct runadewapp_fileList *next;
    char *filename;
    boolean newWindow;
    boolean readOnly;
};

class runadewapp[runadewa]: application[app] {
    overrides:
	ParseArgs(int argc, char **argv) returns boolean;
	Start() returns boolean;
    classprocedures:
        InitializeClass() returns boolean;
        GetArguments(int *argc) returns char **;
    data:
        struct runadewapp_fileList *files,**fileLink;
        boolean initFile;
        boolean haveBufferInWindow;
        struct im *im;
        struct frame *frame;
        struct buffer *buffer;
	char *title, *cls,*func;
	};
