/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/text.ch,v 2.10 89/09/26 11:51:34 tpn Exp $ */
/* $ACIS:text.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/text.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/text.ch,v 2.10 89/09/26 11:51:34 tpn Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define text_VERSION 1

class text: simpletext[smpltext] {
overrides:
    SetAttributes(struct attributes *attributes);
    Clear();
    GetModified() returns long;
    ReplaceCharacters(long pos, long len, char *repStr, long replen) returns boolean;
    AlwaysReplaceCharacters(long pos, long len, char *repStr, long replen);
    LengthChanged(long pos, long len);
    HandleKeyWord(long pos, char *keyword, FILE *file) returns long;
    HandleCloseBrace(long pos, FILE *file) returns long;
    Read(FILE *file, long id) returns long;
    Write(FILE *file, long writeID, int level) returns long;
    ReadSubString(long pos, FILE *file, boolean quoteCharacters) returns long;
    WriteSubString(long pos, long len, FILE *file, boolean quoteCharacters);
    ObservedChanged (struct observable *changed, long value);
    AlwaysCopyText(long pos,struct thisobject *srctext,long srcpos,long len);
methods:
    InsertObject(long pos, char *name,char *viewname) returns struct viewref *;
    AddStyle(long pos, long len, struct style *style) returns struct environment *; 
    AddView(long pos, char *viewtype, struct dataobject *dataobject) returns struct environment *;
    AlwaysAddStyle(long pos, long len, struct style *style) returns struct environment *; 
    AlwaysAddView(long pos, char *viewtype, struct dataobject *dataobject) returns struct environment *;
    SetEnvironmentStyle(struct environment *envptr, struct style *styleptr);
    SetGlobalStyle(struct style *styleptr);
    GetGlobalStyle() returns struct style *;
    ReadTemplate(char *templateName, boolean inserttemplatetext) returns long;
    FindViewreference(long pos, long len) returns struct viewref *;
    AlwaysInsertFile(FILE * file,char *filename, long position) returns long;
    InsertFile(FILE * file,char *filename, long position) returns long;
    ClearCompletely();
    EnumerateEnvironments(long pos,long len,procedure callBack,long rock) returns struct environment *;
macromethods:
    SetExportEnvironments(boolean val) (self->exportEnvs = val)
    GetExportEnvironments() (self->exportEnvs)
    GetStyleSheet() (self->styleSheet)
    SetWriteAsText(TorF)  ((self)->WriteAsText = TorF)
    GetWriteAsText()  ((self)->WriteAsText)
    SetCopyAsText(TorF)  ((self)->CopyAsText = TorF)
    GetCopyAsText()  ((self)->CopyAsText)

classprocedures:
    FinalizeObject(struct text *self);
    ApplyEnvironment(struct text_statevector *sv,struct style *defaultStyle, struct environment *env);
    InitializeObject(struct text *self) returns boolean;
    InitStateVector(struct text_statevector *sv);
    FinalizeStateVector(struct text_statevector *sv);
data:
    struct environment *rootEnvironment;
    struct stylesheet *styleSheet;
    char *templateName; /* Template to read if uninterpreted document. */
    struct viewref *currentViewreference;
    boolean exportEnvs;
    boolean executingGetModified;
    boolean WriteAsText,CopyAsText;
};
