#ifndef	lint
#ifdef sccs
static char     sccsid[] = "@(#)sys_read.c 20.8 89/08/17 Copyr 1985 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Sys_read.c - Real system call to read.
 */

#include <syscall.h>
#include <xview_private/ntfy.h>

pkg_private int
notify_read(fd, buf, nbytes)
    int             fd;
    char           *buf;
    int             nbytes;
{
    return (syscall(SYS_read, fd, buf, nbytes));
}
