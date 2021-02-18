/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) usermenu.c 5.1 89/12/14 Crucible";
/*
 * This file contains all of the functions for manipulating the user menu
 *
 * Global Functions:
 * InitUserMenu -- load the user menu from .olwmmenu and initialise
 * AppMenuFunc  -- handle an "application" item being selected on olwm menu
 *
 * Global data:
 * RootMenu -- workspace menu
 */

/*
 * syntax of the user menu file:
 *
 *	There are five reserved keywords: 
 *		MENU and END, which are used to delimit a submenu.  
 *		DEFAULT tags a default button
 *		TITLE tags a menu title
 *		PIN tags whether menu is pinnable
 *
 * 	The file is line-oriented (yecch) and somewhat compatible to sunview.
 *
 *	Each line consists of three fields:  an optional tag, a 
 *	label, and a function.  The tag does not exist in sunview.
 *	The tag is used to mark special lines, including menu
 *	titles, pinnability, and defaults.  The label gives the
 *	label printed with the item, and the function names what
 *	should be done with the item.
 *
 *	each field contains a string; strings with embedded blanks
 *	must be enclosed in double quotes.  The following can be
 *	escaped with a backslash:
 *		\\ produces a backslash
 *		\" produces a double quote
 *		\n produces a newline
 *
 *	comments can be embedded in a file by starting a line with a
 *	pound sign (#)  Comments may not be preserved as the file is 
 *	used.
 *
 *	A submenu can be specified by putting the keyword "MENU" in the 
 *	function.  The submenu may contain a title and pinning line
 *	followed by a series of menu items.  The submenu ends with a
 *	line optionally repeating the label and containing the 
 *	keyword 'end'.
 *
 *	There are several functions which aren't invoked as programs;
 *	rather, they are built in to window manager.  These built-in
 *	services are each denoted by a single keyword; they are:
 *		NOP		No-operation
 *		EXIT		Exit server
 *		REFRESH		Refresh the screen
 *		CLIPBOARD	Clipboard viewer
 *		PRINTSCREEN	Print the screen
 *		PROPERTIES	Bring up properties dialogue
 *		WINDOWCTL	Window control functions 
 *
 *	example:
 *	TITLE	"Workspace Menu"
 *	PIN
 *	Programs	MENU
 *		PIN
 *		"Command Tool" cmdtool
 *		"Blue Xterm" "xterm -fg white -bg blue"
 *		END
 *	Utilities	MENU
 *		"Refresh Screen" REFRESH
 *		"Clipboard"	 CLIPBOARD
 *	Utilities	END
 */

#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "menu.h"
#include "olwm.h"

#define	MENUFILE	"/.olwmmenu"
#define TOKLEN		300

/* Externals */
extern char 		**Envp;
extern char		*StringDup();
extern void		*IntMalloc();

extern int RefreshFunc();
extern int ClipboardFunc();
extern int PrintScreenFunc();
extern int ExitFunc();
extern int PropertiesFunc();
extern MakeMenuWin();
extern int NopFunc();

/* global data */
Menu *RootMenu;
int AppMenuFunc();

/* local forward declarations */
static void	parseMenu();
static int	windowCtlFunc();
static void	initMenu();

/* local data */
typedef struct _buttondata {
	struct _buttondata *next;
	char *name;
	FuncPtr func;
	char *exec;		/* string to be executed, like "xterm" */
	void *submenu;
	} buttondata;


typedef struct {
	char *title;
	int idefault;
	int nbuttons;
	Bool pinnable;
	buttondata *bfirst, *blast;
	} menudata;

menudata userroot = { NULL, -1, 0, NULL, NULL };

typedef enum { UsrToken, MenuToken, EndToken, DefaultToken, PinToken, 
	TitleToken, ServiceToken } TokenType;

#define NSERVICES 12
struct {
	char *token;
	FuncPtr func;
	TokenType toktype;
	} svctokenlookup[NSERVICES] =
		{"REFRESH", RefreshFunc, ServiceToken, 
		 "CLIPBOARD", ClipboardFunc, ServiceToken, 
		 "PRINTSCREEN", PrintScreenFunc, ServiceToken, 
		 "EXIT", ExitFunc, ServiceToken, 
		 "PROPERTIES", PropertiesFunc, ServiceToken, 
		 "NOP", NopFunc, ServiceToken, 
		 "DEFAULT", NULL, DefaultToken, 
		 "MENU", NULL, MenuToken, 
		 "END", NULL, EndToken, 
		 "PIN", NULL, PinToken, 
		 "TITLE", NULL, TitleToken, 
		 "WINDOWCTL", windowCtlFunc, ServiceToken,
		};

/* lookupToken -- look up a token in the list of tokens
 *	given a supposed keyword or service name.  If the name doesn't
 *	match any existing token, return the user-defined token.  
 */
static TokenType
lookupToken(nm,ppf)
char *nm;
FuncPtr *ppf;
{
	int ii;

	for (ii=0; ii<NSERVICES; ii++)
	{
		if (!strcmp(nm,svctokenlookup[ii].token))
		{
			if (ppf != (FuncPtr *)0) 
				*ppf = svctokenlookup[ii].func;
			return svctokenlookup[ii].toktype;
		}
	}
	if (ppf != (FuncPtr *)0)
		*ppf = AppMenuFunc;
	return UsrToken;
}


/* insertInMenu -- insert a button at the end of a menu
 */
static void
insertInMenu(m,b,def)
menudata *m;
buttondata *b;
int def;
{
	if (m->bfirst == NULL)
	{
		m->bfirst = m->blast = b;
		m->nbuttons = 1;
		m->idefault = def?0:-1;
	} 
	else
	{
		m->blast->next = b;
		m->blast = b;
		if (def)
			m->idefault = m->nbuttons;
		m->nbuttons++;
	}
}

/* parseError -- report a parse error and bail out 
 */
static void
parseError(s1, s2)
char *s1, *s2;
{
	fprintf(stderr, "%s %s", s1, s2);
	exit(1);
}

/* extractToken -- get a token out of the string given a starting
 * 	position
 */
static char *
extractToken(pr)
char **pr;
{
	char *p = *pr;
	char *pb = NULL;

	while ((isspace(*p)) && (*p != '\0')) p++;

	if (*p == '"')
	{
		p++;
		pb = p;
		while ((*p != '"') && (*p != '\0'))
		{
			if (*p == '\\')
			{
				if (*(++p) == '\0')
					parseError("Can't have a backslash at the end of a line","");
			}
			p++;
		}
	}
	else if (*p != '\0')
	{
		pb = p;
		while ((!isspace(*p)) && (*p != '\0')) p++;
	}

	if (*p != '\0')
		*(p++) = '\0';

	*pr = p;
	return pb;
}

/* parseLine -- extract fields from a line 
 */
static void
parseLine(s, t, l, f)
char *s;
char **t, **l, **f;
{
	char *p;

	*t = *l = *f = NULL;
	p = s;
	if (*p == '#') return;
	*t = extractToken(&p);
	*l = extractToken(&p);
	*f = extractToken(&p);
}


/* parseButton -- process a simple button (one of form
 *	"title" "exec" or "title" servicename or "title" {submenu})
 *	def is True when this is a default button
 */
static void
parseButton(stream, label, func, fp, menuroot, def)
FILE *stream;
char *label, *func;
FuncPtr fp;
menudata *menuroot;
int def;
{
	buttondata *b;

	b = (buttondata *)IntMalloc((unsigned)sizeof(buttondata));
	b->next = NULL;
	b->name = StringDup(label);
	b->func = fp;

	if (fp == AppMenuFunc)
	{
		b->exec = StringDup(func);
		b->submenu = NULL;
	}
	else if (fp != NULL)
	{
		b->exec = NULL;
		b->submenu = NULL;
	}
	else
	{
		b->exec = NULL;
		initMenu( (menudata **)&(b->submenu) );
		parseMenu(stream, (menudata *)(b->submenu));
	}

	insertInMenu(menuroot, b, def);
}

/*
 * parseMenu -- read the user menu from the given stream
 *	parse the stream into the menu structures defined locally
 *	these structures (which are local to this module) are later
 *	used to build real menu structures
 */
static void
parseMenu(stream,menuroot)
FILE *stream;
menudata *menuroot;
{
	char filline[TOKLEN];
	char *tag, *label, *func;
	FuncPtr fp;
	Bool isdefault, isdone;
	TokenType toktype;
	Bool menuisdone = False;

	while ((!menuisdone) && (fgets(filline, TOKLEN, stream) != NULL))
	{
		if (filline[0] == '#')
			continue;

		parseLine(filline, &tag, &label, &func);
		if ((tag == NULL) && (label == NULL) && (func == NULL))
			continue;
		isdone = isdefault = False;

		switch (lookupToken(tag, (FuncPtr *)0))
		{
		case DefaultToken:
			isdefault = True;
			break;

		case PinToken:
			menuroot->pinnable = True;
			isdone = True;
			break;

		case TitleToken:
			if (label == NULL)
				parseError("Menu titles require a title string","");
			menuroot->title = StringDup(label);
			isdone = True;
			break;

		default:
			func = label;
			label = tag;
		}

		if (isdone) continue;

		switch (lookupToken(label, (FuncPtr *)0))
		{
		case UsrToken:
		case ServiceToken:
			toktype = lookupToken(func, &fp);
			switch (toktype)
			{
			case UsrToken:
			case ServiceToken:
			case MenuToken:
				parseButton(stream,label, func, fp, menuroot, isdefault);
				break;
			
			case EndToken:
				menuisdone = True;

			default:
				parseError("parse error in usermenu: didn't function expect",func);
			}
			break;

		case MenuToken:
		case DefaultToken:
		case PinToken:
		case TitleToken:
			parseError("Syntax error:  unexpected string", label);
			break;
			
		case EndToken:
			menuisdone = True;
			if ( func != NULL &&
			    lookupToken(func, (FuncPtr *)0) == PinToken)
			    menuroot->pinnable = True;
			break;

		default:
			parseError("Fatal internal parse error in usermenu","");
		}
	}
	if (menuroot->nbuttons == 0)
		parseError("Error in user menu:  menu with no buttons","");
}

/* buildFromSpec -- build the real menu structures, and create the
 * 	associated menus, from the specifications parsed from
 *	the menu layout.  Free up the specifications as we go
 *	along.
 */
static Menu *
buildFromSpec(dpy,menu,deftitle)
Display *dpy;
menudata *menu;
char *deftitle;
{
	Menu *m;
	Button *b;
	int ii;
	buttondata *bdata, *bsave;

	m = (Menu *)IntMalloc((unsigned)sizeof(Menu));
	if (menu->pinnable)
	{
		m->hasPushPin = True;
		m->pinAction = MakeMenuWin;
		if (menu->title == NULL)
		{
			m->title = StringDup(deftitle);
		}
		else
		{
			m->title = StringDup(menu->title);
		}
	}
	else
	{
		m->hasPushPin = False;
		m->pinAction = NULL;
		if (menu->title != NULL)
		{
			m->title = StringDup(menu->title);
		}
	}
	m->buttonCount = menu->nbuttons;
	m->buttonDefault = menu->idefault;
	b = (Button *)IntMalloc((unsigned)(menu->nbuttons * sizeof(Button)));
	m->buttons = b;
	for (ii=0,bdata=menu->bfirst; ii<menu->nbuttons; ii++)
	{
		b[ii].label = bdata->name;
		b[ii].stacked = bdata->submenu != NULL;
		b[ii].state = Enabled;
		b[ii].action.callback = bdata->func;
		if ( b[ii].stacked )
			b[ii].action.submenu = 
				(void *)buildFromSpec(dpy, 
					      (menudata *)(bdata->submenu),
					      bdata->name);
		else
			b[ii].action.submenu = (void *)bdata->exec;

		bsave = bdata;
		bdata = bdata->next;
		/* lint will complain about this cast */
		free((char *)bsave);

	}
	MenuCreate(dpy,m);
	/* lint will complain about this cast */
	free((char *)menu);
	return(m);
}


/* lint will complain about the following strings since 
 * Button.ButtonAction.submenu is declared as a void * (normally a Menu *)
 * {in ButtonServices[] and AppMenuButtons[]} 
 */

Button ButtonServices[] = {
        { "Refresh", False, Enabled, {RefreshFunc, NULL} },
	{ "Reset Input", False, Enabled, {AppMenuFunc, (void*)"kbd_mode -u"} },
	{ "Save Workspace", False, Disabled, {AppMenuFunc, (void*)"xplaces"} },
	{ "Lock Screen", False, Enabled, {AppMenuFunc, (void*)"xlock"} },
	{ "Console...", False, Enabled, {AppMenuFunc,
					     (void*)"cmdtool -Wh 8 -C"} },
#ifdef notdef
	{ "Window Controls...", False, Enabled, {windowCtlFunc, NULL} },
        { "Clipboard...", False, Enabled, {ClipboardFunc, NULL} },
        { "Print Screen", False, Enabled, {PrintScreenFunc, NULL} },
#endif /* notdef */
        };
Menu MenuServices = { NULL, ButtonServices, 5, -1, False, NULL };

Button AppMenuButtons[] = {
	{ "Command Tool...", False, Enabled, {AppMenuFunc, (void*)"cmdtool"} },
	{ "Text Editor...", False, Enabled, {AppMenuFunc, (void*)"textedit"} },
	{ "File Manager...", False, Enabled, {AppMenuFunc, (void*)"filemgr"} },
	{ "Mail Tool...", False, Enabled, {AppMenuFunc, (void*)"mailtool"} },
	{ "Clock...", False, Enabled, {AppMenuFunc, (void*)"clock"} },
	{ "Calculator...", False, Enabled, {AppMenuFunc, (void*)"xcalc"} },
	{ "Snapshot...", False, Enabled, {AppMenuFunc, (void*)"snapshot"} },
	{ "Perf Meter...", False, Enabled, {AppMenuFunc, (void*)"perfmeter"} },
	{ "Icon Editor...", False, Enabled, {AppMenuFunc, (void*)"iconedit"} },
	{ "Shell Tool...", False, Enabled, {AppMenuFunc, (void*)"shelltool"} },
	{ "SunView Dbxtool...", False, Enabled, {AppMenuFunc,
						(void*)"/usr/bin/dbxtool"} },
	};
Menu AppMenuMenu = {"Programs", AppMenuButtons, 11, 0, False, NULL};

Button RootButtons[] = {
        { "Programs", True, Enabled, {(FuncPtr) 0, (void*)&AppMenuMenu} },
        { "Utilities", True, Enabled, {(FuncPtr) 0, (void*)&MenuServices} },
        { "Properties...", False, Enabled, {PropertiesFunc, NULL} },
        { "Exit", False, Enabled, {ExitFunc, NULL} },
        };
Menu DefaultRootMenu = { "Workspace", RootButtons, 4, -1, True, MakeMenuWin};


static Menu *
useDefaultMenu(dpy)
Display *dpy;
{
	MenuCreate(dpy,&MenuServices);
	MenuCreate(dpy,&AppMenuMenu);
	MenuCreate(dpy,&DefaultRootMenu);
	return (&DefaultRootMenu);
}

/*
 * Global routines
 */

/*	
 * InitUserMenu	-- load the user menu 
 *	dpy	- display
 * readUserMenu -- read the application menu from the file .olwmmenu in
 *	either the directory specified by OLWMPATH or HOME.
 *
 * Sets up for the parse of this file, then calls parser.
 */
InitUserMenu(dpy)
Display		*dpy;
{
	char	*dir, *getenv();
	char	path[100];
	FILE	*stream;

	if ((dir = getenv("OLWMPATH")) == (char *)0)
		if ((dir = getenv("HOME")) == (char *)0)
			RootMenu = useDefaultMenu(dpy);
	
	strcpy(path, dir);
	strcat(path, MENUFILE);

	if ((stream = fopen(path, "r")) == (FILE *)0)
		RootMenu = useDefaultMenu(dpy);
	else
	{
		parseMenu(stream,&userroot);
		RootMenu = buildFromSpec(dpy,&userroot,"Workspace");
	}
}

/*
 * AppMenuFunc -- called when the "Applications" item has been selected on
 *      the olwm menu
 */
/*ARGSUSED*/	/* dpy, winInfo args will be used later */
int
AppMenuFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinInfo *winInfo;
Menu    *menu;
int     idx;
{
        char    *commArgv[4];
	int	pid;

        commArgv[0] = "/bin/sh";
        commArgv[1] = "-c";
	/* normally submenu is a Menu *, but it's a char * for commands
	 * which will be executed here (lint will complain)
	 */
        commArgv[2] = (char *)menu->buttons[idx].action.submenu;
        commArgv[3] = NULL;

	pid = fork();
	if (pid == -1)
	{
		perror("olwm: fork");
	}
	else if (pid == 0)
	{
		/* child */
		setpgrp(0, getpid());
		execve(commArgv[0], commArgv, Envp);
		exit(1);
	}
}

/*
 * NopFunc - a no-operation function, used as a placeholder for
 * 	the NOP service
 */
/*ARGSUSED*/	/* dpy, winInfo, menu, idx args included for consistency */
static int
NopFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinInfo *winInfo;
Menu    *menu;
int     idx;
{
}

/*
 * windowCtlFunc - Window Controls
 */
/*ARGSUSED*/	/* dpy, winInfo, menu, idx args used when implemented */
static int
windowCtlFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinInfo *winInfo;
Menu    *menu;
int     idx;
{
	fprintf(stderr,"Window controls not implemented\n");
}

/*
 * initMenu - 
 */
static void
initMenu( newmenu )
menudata	**newmenu;
{
	*newmenu = (menudata *)IntMalloc( (unsigned)sizeof(menudata) );
	(*newmenu)->bfirst = (buttondata *)0;
	(*newmenu)->title = NULL;
}
