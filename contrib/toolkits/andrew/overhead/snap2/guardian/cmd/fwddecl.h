/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/snap2/guardian/cmd/RCS/fwddecl.h,v 1.3 89/10/16 10:48:52 cfe Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/snap2/guardian/cmd/RCS/fwddecl.h,v $ */
/* Forward Declarations for guardian.c */
typedef char bool;

bool want_more_users();
char *errno_to_text();
char *guard_error_to_txt();
char bool_to_char();
int auth_use_next();
static int fuseki();
int lookup_auth_cache();
static bool AddrEql();
static bool AuthenticationRequested();
static bool CanAcceptNewClient();
static bool ForkServerHead();
static bool GetMyAddress();
static bool GetServerRequest();
static bool IgnoreCaseEql();
static bool NewUser();
static bool OkToCreateNewServer();
static bool SameHost();
static char * FindNextLine ();
static char *GetToken ();
static char *ParseReboot();
static char *SkipBlanks();
static char *SkipWord();
static char lc();
static int BindtoPort();
static int CheckServers();
static int ChildAction();
static int Connect();
static int ExecuteRequest();
static int Initialize();
static int Loop();
static int MakeKey();
static int ProcessArgs();
static int ReportError();
static int ReportNumError();
static int ReportSnapError();
static int ReportSystemError();
static int ServerHead();
static int SetAddress();
static int SetLimits();
static int SetLog();
static int SetReboot();
static int assignbool();
static int assignhex();
static int assignlimit();
static int assignlog();
static int assignreboot();
static int authenticate_if_requested();
static int build_stat_packet();
static int safeperror();
static void AServerDied();
static void CheckLog();
static void CheckPassword();
static void CheckReboot();
static void DebugSocket();
static void Demonize();
static void ExecCommand();
static void ExecuteDebugRequest();
static void HelpCommand();
static void InitServices();
static void KillCommand();
static void MailError();
static void NewClient();
static void NextRequest();
static void PrintServer();
static void ProcessDebugRequest();
static void ProcessOption();
static void SendStats();
static void ServerCommand();
static void ServerDied();
static void ServiceCommand();
static void SetServicesTable();
static void VarCommand();
static void child_sys();
static void clear_client_start_error();
static void count();
static void kill_children();
static void printbool();
static void printhex();
static void printlimit();
static void printlog();
static void printreboot();
static void printtime();
static void printversion();
static void reboot();
void init_auth_cache();
void log_death();
#ifdef AFS_ENV
static bool UserPermitted();
static bool homeCell();
static int CellPwdAuth();
static int MultiTokensAuth();
static int PwdAuth();
static int SetAuthorizedUsers();
static int SetTokens();
static int VTokensAuth();
static int really_authenticate();
static void gclearauth();
#endif /* AFS_ENV */
#ifdef RAUTH
#ifdef AFS_ENV
static char *GetCellName();
static int InitializeRPC();
static int ReportAuthError();
#endif /* AFS_ENV */
#endif /* RAUTH */

