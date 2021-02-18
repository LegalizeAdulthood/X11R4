/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: help.h,v 1.2 89/12/06 21:41:16 xguest Exp $ */
/* $ACIS$ */
/* $Source: /xsrc/contrib/toolkits/andrew/atk/help/src/RCS/help.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: help.h,v 1.2 89/12/06 21:41:16 xguest Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/*---------------------------------------------------------------------------*/
/*	MODULE: help.h							     */
/*		Contains error messages and static structures used within    */
/*		the help object.					     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*				PROTOTYPES				     */
/*---------------------------------------------------------------------------*/

boolean help__InitializeClass();
boolean help__InitializeObject();
extern char *index(), *rindex();
extern int errno;
static char *AddToPanelList();
static char *AndyCopy();
static char *CopyString();
static int ScanLine();
static char *LowerCase();
static char *MapParens();
static int GetHelpOn();
static int ShowFile();
static int help__HelpappGetHelpOn();
static int mysystem ();
static int panelCompare();
static struct view *SetupLpairs();
static void AddBookmark();
static void AddHistoryItem ();
static void AddSearchDir();
static void ExitProc();
static void Expander ();
static void ExpanderAux();
static void FilterPanel();
static void HistoryHelp ();
static void NewHelp();
static void NextHelp ();
static void OverviewHelp ();
static void Print();
static void RestorePanel();
static void SearchOverviews();
static void SearchPrograms();
static void SendComments();
static void SetupMenus();
static long SetupPanel();
static void ShowChanges();
static void ShowTutorial();
static void SortAndMakePanel();
static void TextviewProc();
static void ToggleHistory();
static void ToggleOverviews();
static void TogglePanels();
static void ToggleProgramListSize();
static void TogglePrograms();
static void nono();
struct view *help__GetInstance();
struct view *help__Hit();
void help__AddSearchDir();
void help__FinalizeObject();
void help__FullUpdate();
void help__LinkTree();
void help__MoreMode();
void help__PostKeyState();
void help__PostMenus();
void help__SetAliasesFile();
void help__SetIndex();
void help__Update();
void help__WantUpdate();

/*---------------------------------------------------------------------------*/
/*			     INTERNAL CONSTANTS				     */
/*---------------------------------------------------------------------------*/

/* 
 * globals bindings for menus and keys 
 */
static struct bind_Description helpBindings[]={

/*
    {"proc-name", keybinding, keyrock,
    "menu string", menurock, menuflag, function-to-call,
    "documentation string", module-name}
*/

    /* Toggling menu items */

    /* if self->showPanels */
    {"help-toggle-panels", NULL, help_HIDE_PANEL,
    "Panels~10,Hide Panels~2", help_HIDE_PANEL, MENU_TogglePanelHide, TogglePanels,
    NULL, NULL},

    {"help-toggle-panels", NULL, help_SHOW_PANEL,
    "Panels~10,Show Panels~2", help_SHOW_PANEL, MENU_TogglePanelShow, TogglePanels,
    NULL, NULL},

    {"help-toggle-panels", HELP_KEY_TOGGLE_PANELS, help_ALWAYS_TOGGLE,
    NULL, help_ALWAYS_TOGGLE, MENU_Never, TogglePanels,
    "toggle help panels", NULL},


    /* if self->showOverview */
    {"help-toggle-overviews", NULL, help_HIDE_OVER,
    "Panels~10,Hide Overviews~20", help_HIDE_OVER, MENU_ToggleOverHide, ToggleOverviews,
    NULL, NULL},

    {"help-toggle-overviews", NULL, help_SHOW_OVER,
    "Panels~10,Show Overviews~20", help_SHOW_OVER, MENU_ToggleOverShow, ToggleOverviews,
    NULL, NULL},

    {"help-toggle-overviews", HELP_KEY_TOGGLE_OVER, help_ALWAYS_TOGGLE,
    NULL, help_ALWAYS_TOGGLE, MENU_Never, ToggleOverviews,
    "toggle overviews panel", NULL},


    /* if self->showList */
    {"help-toggle-programs", NULL, help_HIDE_LIST,
    "Panels~10,Hide Programs~22", help_HIDE_LIST, MENU_ToggleListHide, TogglePrograms,
    NULL, NULL},

    {"help-toggle-programs", NULL, help_SHOW_LIST,
    "Panels~10,Show Programs~22", help_SHOW_LIST, MENU_ToggleListShow, TogglePrograms,
    NULL, NULL},

    {"help-toggle-programs", HELP_KEY_TOGGLE_PROGRAMS, help_ALWAYS_TOGGLE,
    NULL, help_ALWAYS_TOGGLE, MENU_Never, TogglePrograms,
    "toggle programs panel", NULL},


    /* if self->showHistory */
    {"help-toggle-history", NULL, help_HIDE_HIST,
    "Panels~10,Hide History~24", help_HIDE_HIST, MENU_ToggleHistHide, ToggleHistory,
    NULL, NULL},

    {"help-toggle-history", NULL, help_SHOW_HIST,
    "Panels~10,Show History~24", help_SHOW_HIST, MENU_ToggleHistShow, ToggleHistory,
    NULL, NULL},

    {"help-toggle-history", HELP_KEY_TOGGLE_HIST, help_ALWAYS_TOGGLE,
    NULL, help_ALWAYS_TOGGLE, MENU_Never, ToggleHistory,
    "toggle history panel", NULL},


    /* if self->expandedList */
    {"help-toggle-size", NULL, help_SHRINK,
    "Panels~10,Shrink Programs List~10", help_SHRINK, MENU_ToggleSizeShrink,
    ToggleProgramListSize, NULL, NULL},

    {"help-toggle-size", NULL, help_EXPAND,
    "Panels~10,Expand Programs List~10", help_EXPAND, MENU_ToggleSizeExpand,
    ToggleProgramListSize, NULL, NULL},

    {"help-toggle-size", HELP_KEY_TOGGLE_PROGRAMS, help_ALWAYS_TOGGLE,
    NULL, help_ALWAYS_TOGGLE, MENU_Never, ToggleProgramListSize,
    "toggle program list size", NULL},


    {"help-filter-programs", NULL, help_FILTER_FILTER,
    "Other~10,Filter Programs Panel~10", help_FILTER_FILTER, MENU_ToggleFilterShow, FilterPanel,
    "filter-programs", NULL},

    {"help-refilter-programs", NULL, help_FILTER_REFILTER,
    "Other~10,ReFilter Programs Panel~12", help_FILTER_REFILTER, MENU_ToggleReFilterShow, FilterPanel,
    "filter-programs", NULL},

    {"help-restore-programs", NULL, help_FILTER_RESTORE,
    "Other~10,Restore Programs Panel~10", help_FILTER_RESTORE, MENU_ToggleRestoreShow, RestorePanel,
    "restore programs panel", NULL},


    {"help-show-more", HELP_KEY_SHOW_MORE, 0,
    "Show More Documentation~30", 0, MENU_SwitchMoreMenu, NextHelp,
    "show more documentation", NULL},

    {"help-show-tutorial", HELP_KEY_SHOW_TUTORIAL, 0,
    "Show Tutorial~32", 0, MENU_SwitchTutorialMenu, ShowTutorial,
    "show tutorial", NULL},

    {"help-changes", HELP_KEY_SHOW_CHANGES, 0,
    "Show Changes~34", 0, MENU_SwitchChangesMenu, ShowChanges,
    "show changes on topic", NULL},
    
    {"help-copy-region", HELP_KEY_COPY_REGION, help_COPY_REGION,
    "Copy~2", help_COPY_REGION, MENU_SwitchCopy, TextviewProc,
    "Copy region to kill-buffer", NULL},

    
    /* non-toggling menu items */


    {"help-delete-window", HELP_KEY_DELETE_WINDOW, 0,
    "Window~12,Delete This Window~90",0 ,MENU_SwitchDeleteMenu, ExitProc,
    "delete this window", NULL},

    {"help-add-search-dir", HELP_KEY_ADD_SEARCH_DIR, 0,
    "Other~10,Add Search Directory~20", 0, MENU_Always, AddSearchDir,
    "add a directory to the search path", NULL},

    {"help-add-bookmark", HELP_KEY_ADD_BOOKMARK, 0,
    "Other~10,Make Bookmark~30", 0, MENU_Always, AddBookmark,
    "add a history entry for this file", NULL},

    {"help-show-help-on", HELP_KEY_HELP_ON, help_ON,
    "Show Help on ...~14", help_ON, MENU_Always, NewHelp,
    "show help on a prompted keyword", NULL},

    {"help-show-help-on", HELP_KEY_HELP_ON_2, help_ON,
    NULL, help_ON, MENU_Always, NewHelp,
    NULL, NULL},

    {"help-new-show-help-on", HELP_KEY_WINDOW_SPLIT, help_ON | help_NEW_WIN,
    "Window~12,Show Help on...~14", help_ON | help_NEW_WIN, MENU_Always, NewHelp,
    "show help on a prompted keyword in a new window", NULL},

    {"help-show-help-on-selected", HELP_KEY_HELP_ON_SELECTED, help_SEL,
    "Show Help on Selected Word~16", help_SEL, MENU_Always, NewHelp,
    "show help on selected region", NULL},

    {"help-new-show-help-on-selected", NULL, help_SEL | help_NEW_WIN,
    "Window~12,Show Help on Selected Word~26", help_SEL | help_NEW_WIN, MENU_Always, NewHelp,
    "show help on selected region in a new window", NULL},

    {"help-comments", HELP_KEY_SEND_COMMENT, 0,
    "Send Comment On Help~31", 0, MENU_Always, SendComments,
    "send comments on help", NULL},

    {"help-set-printer", HELP_KEY_SET_PRINTER, help_SET_PRINTER,
    "Print~6,Set Printer~12", help_SET_PRINTER, MENU_Always, TextviewProc,
    "set the printer", NULL},

    {"help-print", HELP_KEY_PRINT, 0,
    "Print~6,Print This File~22", 0, MENU_Always, Print,
    "print current help file", NULL},

    /* imported from textview... */

    {"help-search", HELP_KEY_SEARCH, help_SEARCH,
    "Search~4,Forward~12", help_SEARCH, MENU_Always, TextviewProc,
    "forward search", NULL},

    {"help-reverse-search", HELP_KEY_RSEARCH, help_RSEARCH,
    "Search~4,Backward~14", help_RSEARCH, MENU_Always, TextviewProc,
    "reverse search", NULL},
    
    {"help-search-again", HELP_KEY_SEARCH_AGAIN, help_SEARCH_AGAIN,
    "Search~4,Search Again~16", help_SEARCH_AGAIN, MENU_Always, TextviewProc,
    "search again", NULL},

    {"help-search-overviews", HELP_KEY_SEARCH_OVERVIEWS, 0,
    "Search~4,Search Overviews~20", 0, MENU_Always, SearchOverviews,
    "search overviews", NULL},

    {"help-search-programs", HELP_KEY_SEARCH_PROGRAMS, 0,
    "Search~4,Search Programs~22", 0, MENU_Always, SearchPrograms,
    "search programs", NULL},
    
    {"help-next-screen", HELP_KEY_NEXT_SCREEN, help_PAGE_DOWN,
    NULL, help_PAGE_DOWN, 0, TextviewProc,
    "advance to next screen", NULL},

    {"help-next-screen", HELP_KEY_NEXT_SCREEN2, help_PAGE_DOWN,
    NULL, help_PAGE_DOWN, 0, TextviewProc,
    "advance to next screen", NULL},

    {"help-prev-screen", HELP_KEY_PREV_SCREEN, help_PAGE_UP,
    NULL, help_PAGE_UP, 0, TextviewProc,
    "go back to previous screen", NULL},

    NULL};

static char *overview_ext_array[] = OVERVIEW_EXTS;
static char *program_ext_array[] = PROGRAM_EXTS;

/*
 * error messages
 */

static char *err_generic = "Unspecified error.";
static char *err_unexpected = "Unexpected failure to get help on '%s'.";
static char *err_server = "Sorry; a file server is down.";
static char *err_missing = "Sorry; your help file appears to be missing.";
static char *err_read = "Sorry; could not read help file.";
static char *err_sorry = "Sorry; no help available on '%s'.";
static char *err_index = "Sorry; index %s not found.  Cannot find entries.";
static char *err_no_more = "No more help entries\n";
static char *err_file = "Help file %s not found.\n";
static char *err_no_sel = "Sorry; you must first select a help topic.";
static char *err_sel_too_long = "Selected region (topic) too long.";
static char *err_no_new_view = "Can't make new window.";
static char *err_view = "Can't create a new %s view.";
static char *err_dobj = "Can't create a new %s dataobject.";
static char *err_nontext = "Can't display non-text %s dataobject.";
static char *err_readonly = "Buffer is read-only.";
static char *err_noproc = "Can't find procedure.";
static char *err_openhelpdir = "Cannot get help files.";
static char *err_terminal =
    "The help entry you selected runs a window-manager program to show\n\
documentation.  This cannot be done on a terminal.";

/*
 * informational messages
 */
static char *msg_print_queue = "Queueing files for printing; please wait...";
static char *msg_queue_done = "Your help file has been queued for printing.";
static char *msg_expanding = "Expanding program list; please wait...";
static char *msg_comment = "Preparing sending window; it should appear soon.";
static char *msg_new_win = "Preparing new window; it should appear soon.";
static char *msg_converting = "Processing troff file; please wait...";
static char *msg_dir_prompt = "Enter a directory name to add to the searchpath: ";
static char *msg_filter_type_prompt = "What kind of filtering?";
static char *msg_filter_prompt = "Enter the filter string: ";
static char *msg_print_prompt = "Print the file %s? [yes, no]? ";
static char *msg_term_prompt =
    "See the next help file (%s) [yes, no, quit]? ";
static char *msg_ask_prompt =
    "Enter a keyword about which you want more help: ";
