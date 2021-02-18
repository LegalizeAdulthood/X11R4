/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/app.ch,v 2.10 89/08/30 15:43:32 tpn Exp $ */
/* $ACIS:app.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/app.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidapplication_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/app.ch,v 2.10 89/08/30 15:43:32 tpn Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/* Generic application
 */

class application[app] {
    methods:
        ParseArgs(int argc,char **argv) returns boolean;
        ReadInitFile();
        Start() returns boolean;
        Run() returns int;
        Stop();
        Fork() returns boolean;
        PrintVersionNumber();
    macromethods:
        SetFork(do) (self->fork=(do))
        GetFork() (self->fork)

        SetName(str) (self->name=(str))
        GetName() (self->name)

        SetForceLoad(f) ((self)->forceload=(f))
        GetForceLoad() ((self)->forceload)
        SetErrorProc(p) ((self)->errorProc=(p))
        GetErrorProc() ((self)->errorProc)
        SetErrorRock(r) ((self)->errorRock=(r))
        GetErrorRock() ((self)->errorRock)
        SetMajorVersion(mv) ((self)->majorversion=(mv))
        GetMajorVersion() ((self)->majorversion)
        SetMinorVersion(mv) ((self)->minorversion=(mv))
        GetMinorVersion() ((self)->minorversion)
        SetPrintVersionFlag(TorF) ((self)->printversioninfo=(TorF))
        GetPrintVersionFlag() ((self)->printversioninfo)
    classprocedures:
        DeleteArgs(char **argv, int num);
        GetATKVersion() returns char *;
        InitializeObject(struct app *self) returns boolean;
        FinalizeObject(struct app *self);
    data:
        boolean fork;
        boolean forceload,readInitFile;
        char *profile; /* class to profile, "" for self, NULL for none */
        char *name;
        void (*errorProc)();
        pointer errorRock;
        char *fgcolor, *bgcolor;
        char *geometry;
        long majorversion, minorversion;
        boolean printversioninfo;
};
