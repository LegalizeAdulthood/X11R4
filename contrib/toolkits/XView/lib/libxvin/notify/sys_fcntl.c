#ifndef	lint
#ifdef sccs
static char     sccsid[] = "@(#)sys_fcntl.c 20.8 89/08/17 Copyr 1985 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Sys_fcntl.c - Real system call to fcntl.
 */

#include <syscall.h>
#include <xview_private/ntfy.h>

pkg_private int
notify_fcntl(fd, cmd, arg)
    int             fd, cmd, arg;
{
    return (syscall(SYS_fcntl, fd, cmd, arg));
}
