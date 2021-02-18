/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
 *      Structure declarations for lotus-style menus
 */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/msclients/vui/RCS/lmenus.h,v 2.2 89/02/08 15:53:08 ghoti Exp $ */

typedef struct mkeytab {
    int scan_code;
    int (*keyhandler)();
    } MKEYTAB;

typedef struct menuopts {
        char *Option;
        PRMPT prompt;
        } MENU_OPTS;

#define MENU_TREE struct menu_tree_st

struct menu_tree_st {
        int this;
        struct menu_tree_st *submenu;
        } ;

#define MAXOPTS 10
#define OPTWIDTH 8
#define RETURN_KEY_HIT -127             /* Arbitrary Constants */
#define ESCAPE_KEY_HIT -335
#define REDRAW_KEY_HIT -99

#define NULL_MENU  -1, 0
