/*      @(#)tty_impl.h 20.14 89/08/18 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef _xview_private_ttysw_impl_h_already_included
#define _xview_private_ttysw_impl_h_already_included

/*
 * A tty subwindow is a subwindow type that is used to provide a
 * terminal emulation for teletype based programs.
 */

#include <xview/tty.h>
#include <xview/sel_svc.h>

#define TTY_PRIVATE(_t)      XV_PRIVATE(Ttysw, Xv_tty, _t)
#define TTY_PUBLIC(_tty_folio)     XV_PUBLIC(_tty_folio)

#define TTY_VIEW_PRIVATE(_t)     	 XV_PRIVATE(Ttysw_view_object, Xv_tty_view, _t)
#define TTY_VIEW_PUBLIC(_tty_view)	 XV_PUBLIC(_tty_view)

#define IS_TTY(_t) \
	(((Xv_base *)(_t))->pkg == TTY)
	
#define IS_TTY_VIEW(_t) \
	(((Xv_base *)(_t))->pkg == TTY_VIEW)
		
/* BUG: This could be made cleaner.  See 
 * TERMSW_FOLIO_FROM_TERMSW_VIEW_HANDLE in termsw_impl.h 
 */
#define TTY_VIEW_HANDLE_FROM_TTY_FOLIO(_tty_folio_private) \
	 ((Ttysw_view_handle)(((Ttysw_folio)_tty_folio_private)->view)) 
 	     
#define TTY_FOLIO_FROM_TTY_VIEW_HANDLE(_tty_view_private) \
	 ((Ttysw_folio)(((Ttysw_view_handle)_tty_view_private)->folio))
	 
#define TTY_FOLIO_FROM_TTY_VIEW(_tty_view_public) 	\
	 ((Ttysw_folio)					\
	  (((Ttysw_view_handle) TTY_VIEW_PRIVATE(_tty_view_public))->folio))

#define TTY_FROM_TTY_VIEW(_tty_view_public) 		\
	((Tty) TTY_PUBLIC(TTY_FOLIO_FROM_TTY_VIEW(_tty_view_public)))

/*
 * These are the data structures internal to the tty subwindow
 * implementation.  They are considered private to the implementation.
 */

struct cbuf {
    char               *cb_rbp;	   /* read pointer */
    char               *cb_wbp;	   /* write pointer */
    char               *cb_ebp;	   /* end of buffer */
    char                cb_buf[8192];
};

struct keymaptab {
    int                 kmt_key;
    int                 kmt_output;
    char               *kmt_to;
};

struct textselpos {
    int                 tsp_row;
    int                 tsp_col;
};

struct ttyselection {
    int                 sel_made;  /* a selection has been made */
    int                 sel_null;  /* the selection is null */
    int                 sel_level; /* see below */
    int                 sel_anchor;/* -1 = left, 0 = none, 1 = right */
    struct textselpos   sel_begin; /* beginning of selection */
    struct textselpos   sel_end;   /* end of selection */
    struct timeval      sel_time;  /* time selection was made */
    Seln_rank		sel_rank;  /* type of selection. primary or secondary */
    int			dehilite_op;  /* Operation for taking down selection */
};

/* selection levels */
#define	SEL_CHAR	0
#define	SEL_WORD	1
#define	SEL_LINE	2
#define	SEL_PARA	3
#define	SEL_MAX		3

extern struct ttyselection	null_ttyselection;

enum ttysw_hdrstate { HS_BEGIN, HS_HEADER, HS_ICON, HS_ICONFILE, HS_FLUSH };

typedef struct ttysubwindow {
    Tty			public_self;		/* Back pointer to the object*/
    struct ttysw_view_object			/* View window */
    			*view;			/* (Pure tty has only one view) */
    Tty_view		current_view_public;	/* This keep trace of the view become ttysw */			
    unsigned		ttysw_flags;
    /* common */
    int                 ttysw_opt;		/* option mask; see ttysw.h */
    struct cbuf         ttysw_ibuf;		/* input buffer */
    struct cbuf         ttysw_obuf;		/* output buffer */
    /* pty and subprocess */
    int                 ttysw_pty;		/* pty file descriptor */
    int                 ttysw_tty;		/* tty file descriptor */
    int                 ttysw_ttyslot;		/* ttyslot in utmp for tty */
    /* page mode */
    int                 ttysw_lpp;		/* page mode: lines per page */
    /* subprocess */
    int                 ttysw_pidchild;		/* pid of the child */
    /* Caps Lock */
    int                 ttysw_capslocked;
#define TTYSW_CAPSLOCKED	0x01	/* capslocked on mask bit */
#define TTYSW_CAPSSAWESC	0x02	/* saw escape while caps locked */
    /* stuff from old ttytlsw */
    enum ttysw_hdrstate	hdrstate;		/* string trying to load */
    char		*nameptr;               /* namebuf ptr */
    char		namebuf[256];           /* accumulates esc string */
    /* selection */
    int                 ttysw_butdown;		/* which button is down */
    struct ttyselection	ttysw_caret;
    struct ttyselection	ttysw_primary;
    struct ttyselection	ttysw_secondary;
    struct ttyselection	ttysw_shelf;
    caddr_t             ttysw_seln_client;
    /* replaceable ops (return TTY_OK or TTY_DONE) */
    int                 (*ttysw_escapeop) ();	/* handle escape sequences */
    int                 (*ttysw_stringop) ();	/* handle accumulated string */
    int                 (*ttysw_eventop) ();	/* handle input event */
    /* kbd translation */
    struct keymaptab    ttysw_kmt[3 * 16 + 2];	/* Key map list */
    struct keymaptab   *ttysw_kmtp;		/* next empty ttysw_kmt slot */
    int		  	(*layout_proc)(); /* interposed window layout proc */

}   Ttysw;

typedef Ttysw		*Ttysw_folio;

typedef struct ttysw_view_object {
    Tty_view		public_self;
    Ttysw_folio		folio;
} Ttysw_view_object;

typedef Ttysw_view_object* 	Ttysw_view_handle;

/* Values for ttysw_flags */
#define TTYSW_FL_FROZEN			0x1
#define TTYSW_FL_IS_TERMSW		0x2
#define TTYSW_FL_IN_PRIORITIZER		0x4

#define TTYSW_NULL      ((Ttysw *)0)

/*
 * Possible returns codes from replaceable ops. 
 */
#define	TTY_OK		(0)	   /* args should be handled as normal */
#define	TTY_DONE	(1)	   /* args have been fully handled */

#define	ttysw_handleevent(ttysw, ie) \
	(*(ttysw)->ttysw_eventop)(TTY_PUBLIC(ttysw), (ie))
#define	ttysw_handleescape(_ttysw_view, c, ac, av) \
	(*(ttysw)->ttysw_escapeop)(TTY_VIEW_PUBLIC(_ttysw_view), (c), (ac), (av))
#define	ttysw_handlestring(ttysw, strtype, c) \
	(*(ttysw)->ttysw_stringop)(TTY_PUBLIC(ttysw), (strtype), (c))

/*	extern routines	*/

void	ttysel_init_client(),
	ttysel_destroy(),
	ttysel_acquire(),
	ttysel_make(),
	ttysel_move(),
	ttysel_deselect(),
	ttysel_hilite(),
	ttyhiliteselection(),
	ttysel_nullselection(),
	ttysel_setselection(),
	ttysel_getselection();

#ifdef	cplus
/*
 * C Library routines specifically related to private ttysw subwindow
 * functions.  ttysw_output and ttysw_input return the number of characters
 * accepted/processed (usually equal to len). 
 */
int 
ttysw_output(Tty ttysw_public, char *addr, int len);

/* Interpret string in terminal emulator. */
int 
ttysw_input(Tty ttysw_public, char *addr, int len);

/* Add string to the input queue. */
#endif	cplus

#endif _xview_private_ttysw_impl_h_already_included
