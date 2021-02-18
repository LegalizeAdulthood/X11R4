/*
 * Copyright 1987 by Tektronix Inc, Beaverton, Oregon.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Tektronix not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* $Header: startup.c,v 6.2 88/10/21 11:50:10 jim Exp $ */

#include <sys/signal.h>
#include <stdio.h>
#include "resource.h"

/*
 * Some X10 servers only allowed a minimum of 16 pixels
 * in tiles... force the value queried from the server
 * up to at least that.
 */
int	MinimumTileSize = 16;	/* Minimum tile size for QueryShape */

main(argc, argv)
	int	argc;
	char	**argv;
{
	int	i, whichbyte, Catch();

	whichbyte = 1;
	if (*(char *) &whichbyte)
		Bigendian = False;

	program = argv[0];

	for(i = 1; i < argc; i++)
	{
		/*
		 * if this is a display argument, we just ignore it
		 * because this is handled in X10main().
		 */
		if (strcmp (argv[i], "-display") == 0 || 
		    strcmp (argv[i], "-d") == 0) {
		    if (++i >= argc)
			Usage ();
		    continue;
		}
		if (index(argv[i], ':') != NULL)
		    continue;

		if (strncmp (argv[i], "MinimumTileSize=", 16) == 0) {
		    MinimumTileSize = atoi(argv[i]+16);
		    if (MinimumTileSize <= 0) {
			fprintf(stderr, "MinimumTileSize is %d: ",
				MinimumTileSize);
			fprintf(stderr, "it must be >= 0\n");
			Usage();
		    }
		    continue;
		}
				    
		if (strcmp (argv[i], "help") == 0)
		    Usage();

		if (!SetFlags(argv[i])) {
		    fprintf(stderr, "unknown option=\"%s\"\n", argv[i]);
		    Usage();
		}
	}

	InitKeycodeMap();

	Catch(0);

	X10main(argc,argv);
}

Catch(sig)
	int	sig;
{
#ifdef DEBUG
	if (sig == SIGINT)
		CommandInterp();
	signal (SIGINT, Catch);
#endif DEBUG
}

SetFlags(string)
	char	*string;
{
	int	result = True;

	if (*string == '+' || *string == '-')
		result = SetDebug(string);
	else if (strcmp(string, "NoOverrideRedirect") == 0)
		OverrideRedirect = False;
#ifdef DEBUG
	else if (strcmp(string, "OverrideRedirect") == 0)
		OverrideRedirect = True;
	else if (strcmp(string, "AllowGrab") == 0)
		FocusKeyboardAllowed = GrabServerAllowed = GrabMouseAllowed
			= True;
	else if (strcmp(string, "NoGrab") == 0)
		FocusKeyboardAllowed = GrabServerAllowed = GrabMouseAllowed
			= False;
	else if (strcmp(string, "AllowGrabServer") == 0)
		GrabServerAllowed = True;
	else if (strcmp(string, "NoGrabServer") == 0)
		GrabServerAllowed = False;
	else if (strcmp(string, "AllowGrabMouse") == 0)
		GrabMouseAllowed = True;
	else if (strcmp(string, "NoGrabMouse") == 0)
		GrabMouseAllowed = False;
	else if (strcmp(string, "AllowFocusKeyboard") == 0)
		FocusKeyboardAllowed = True;
	else if (strcmp(string, "NoFocusKeyboard") == 0)
		FocusKeyboardAllowed = False;
	else if (strcmp(string, "Sync") == 0) {
		DoSync = True;
		ChangeSync(True);
	}
	else if (strcmp(string, "NoSync") == 0) {
		DoSync = False;
		ChangeSync(False);
	}
#endif DEBUG
	else
		result = False;
	return(result);
}

Usage ()
{
	printf("usage: %s -display <display>\n", program);
	printf("\t[help]\n");
	printf("\t[MinimumTileSize=n]\n");
#ifndef DEBUG
	printf("\t[OverrideRedirect]\n");
#else DEBUG
	printf("\t[Sync | NoSync]\n");
	printf("\t[AllowGrabServer | NoGrabServer]\n");
	printf("\t[AllowGrabMouse | NoGrabMouse]\n");
	printf("\t[AllowFocusKeyboard | NoFocusKeyboard]\n");
	printf("\t[AllowGrab | NoGrab]\n");
	printf("\t[OverrideRedirect | NoOverrideRedirect]\n");
	printf("\t[+/-all] [+/-Requests] [+/-Events]\n");
	printf("\t[+/-x10protorequest] [+/-x11eventname]\n");
	printf("\t[+/-EventMasks]\n");
	printf("\t[+/-ButtonMasks]\n");
	printf("\t[+/-Bitmaps]\n");
	printf("\t[+/-Unimplemented]\n");
	printf("\t[+/-WinTree]\n");
	printf("\t[+/-WinTreeMappedOnly]\n");
	printf("\t[+/-PromptForEachRequest]\n");
	printf("\t[+/-PromptForEachEvent]\n");
	printf("\t[+/-LineCoordinates]\n");
	printf("\t[+/-IO]\n");
	printf("\t[+/-NameOnly]\n");
	printf("\t[+/-Initialize]\n");
	printf("\t[+/-Sequence]\n");

	printf("\nDEBUGGING FACILITIES AVAILABLE.\n");
	printf("Hitting ^C puts you into command mode where you can\n");
	printf("set/clear (+/-)  any of the above flags.\n");
#endif DEBUG
	exit(1);
}
