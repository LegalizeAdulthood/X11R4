#include <sys/signal.h>
#include "game.h"

static int hasX;

static int gotsignal;

caught ()
{
	gotsignal++;
	signal (SIGINT, caught);
}

dispInit (argc, argv)
    unsigned int    argc;
    char	    **argv;
{
    if (getenv ("DISPLAY"))
    {
	hasX = 1;
	dispInitX (argc, argv);
    }
    else
    {
	hasX = 0;
	signal (SIGINT, caught);
	dispInitCurses (argc, argv);
    }
}

playGame ()
{
    if (hasX)
	return playGameX ();
    else
	return playGameCurses ();
}

display (board)
    boardT  board;
{
    if (hasX)
	displayX (board);
    else
	displayCurses (board);
}

dispError (s)
char	*s;
{
    if (hasX)
	dispErrorX (s);
    else
	dispErrorCurses (s);
}

dispEnd ()
{
    if (hasX)
	dispEndX();
    else
	dispEndCurses ();
}

dispTurn (player)
{
    if (hasX)
	dispTurnX(player);
    else
	dispTurnCurses (player);
}

dispMove (x, y, player, com)
{
    if (hasX)
	dispMoveX (x, y, player, com);
    else
	dispMoveCurses (x, y, player);
}

dispIllegalMove (x, y, player)
{
    if (hasX)
	dispIllegalMoveX (x, y, player);
    else
	dispIllegalMoveCurses (x, y, player);
}

dispHint (x, y, player)
{
    if (hasX)
	dispHintX (x, y, player);
    else
	dispHintCurses (x, y, player);
}

checkForInput ()
{
    if (hasX)
	checkForInputX ();
}

