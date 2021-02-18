#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_error.c 1.25 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <xview/pkg.h>
#include <xview/xv_error.h>
#include <X11/Xlib.h>

/* Unix system error variables */
extern int      sys_nerr;
extern char    *sys_errlist[];
extern int      errno;

Xv_private      Xv_error_action(*xv_error_proc) ();

/*
 * Error handling -- The function xv_error() will return an indication of
 * what the caller should do next.  This is one of XV_ERROR_CONTINUE,
 * XV_ERROR_RETRY, or XV_ERROR_ABORT.  The meanings are as follows:
 * 
 * XV_ERROR_CONTINUE The client has acknowledged the error and would like the
 * caller to proceed.
 * 
 * XV_ERROR_RETRY The client has performed some corrective action and would like
 * the caller to retry the operation
 * 
 * XV_ERROR_ABORT The client cannot correct the error and agrees that the caller
 * should not proceed.  The caller may return an error status to its caller,
 * or may actually call exit() if there is no error status defined in its
 * interface.
 * 
 * By default, xv_error() calls xv_default_error().  xv_default_error() prints
 * out the error on stderr.
 */

/*
 * xv_error_attr_sprint - convert attribute to a reasonably readable string.
 */
static char    *
xv_error_attr_sprint(attrs, s)
    Attr_avlist	   attrs;
    char          *s;
{
    Attr_union	    a;
    Attr_avlist	    attrs2;
    char           t[256], u[256];

    (void) attr_sprint(s, attrs[0]);
    a.code = attrs[0];
    switch (a.info.base_type) {
      case ATTR_BASE_NO_VALUE:
	(void) sprintf(s, "(no value)");
	break;
	/* Fundamental C types. */
      case ATTR_BASE_INT:
	(void) sprintf(t, " %d (int)", (int) attrs[1]);
	break;
      case ATTR_BASE_LONG:
	(void) sprintf(t, " %ld (long)", (long) attrs[1]);
	break;
      case ATTR_BASE_SHORT:
	(void) sprintf(t, " %d (short)", (short) attrs[1]);
	break;
      case ATTR_BASE_ENUM:
	(void) sprintf(t, " %d (enum)", (int) attrs[1]);
	break;
      case ATTR_BASE_CHAR:
	(void) sprintf(t, " '%c' (char)", (char) attrs[1]);
	break;
      case ATTR_BASE_STRING:
	(void) sprintf(t, " \"%s\" (string)", (char *) attrs[1]);
	break;
      case ATTR_BASE_FUNCTION_PTR:
	(void) sprintf(t, " 0x%08x (func *)", (unsigned) attrs[1]);
	break;
	/* Derivative C types. */
      case ATTR_BASE_BOOLEAN:
	(void) sprintf(t, " %d (bool)", (int) attrs[1]);
	break;
      case ATTR_BASE_OPAQUE:
	(void) sprintf(t, " 0x%08x (opaque)", (unsigned) attrs[1]);
	break;
	/* Special coordinate types; look in attr_cu.c for the details. */
      case ATTR_BASE_X:
      case ATTR_BASE_Y:
	(void) sprintf(t, " %d (coord)", (int) attrs[1]);
	break;
      case ATTR_BASE_INDEX_X:
      case ATTR_BASE_INDEX_Y:
	(void) sprintf(t,
		       " %d/%d (index/coord)", (int) attrs[1], (int) attrs[2]);
	break;
      case ATTR_BASE_XY:
	(void) sprintf(t,
		       " %d/%d (coord/coord)", (int) attrs[1], (int) attrs[2]);
	break;
      case ATTR_BASE_INDEX_XY:
	(void) sprintf(t, " %d/%d/%d (index/coord/coord)", (int) attrs[1],
		(int) attrs[2], (int) attrs[3]);
	break;
	/* Pointer types. */
      case ATTR_BASE_PIXRECT_PTR:
      case ATTR_BASE_PIXFONT_PTR:
      case ATTR_BASE_RECT_PTR:
      case ATTR_BASE_ICON_PTR:
      case ATTR_BASE_SINGLE_COLOR_PTR:
      case ATTR_BASE_CURSOR_PTR:
	(void) sprintf(t, " 0x%08x (ptr type)", (unsigned) attrs[1]);
	break;
      case ATTR_BASE_AV:
	(void) strcpy(t, "[");
	for (attrs2 = &attrs[1]; *attrs2;
	     attrs2 = (Attr_avlist) attr_skip_value(*attrs2, &attrs2[1])) {
	    if (attrs2 != &attrs[1])
		(void) strcat(t, "; ");
	    (void) xv_error_attr_sprint(attrs2, u);
	    (void) strcat(t, u);
	}
	(void) strcat(t, "]");
	break;
    }
    if (a.info.base_type != ATTR_BASE_AV)
	(void) strcat(s, t);
    else
	(void) strcpy(s, t);
    return s;
}

/*
 * xv_default_error - default error routine called by xv_error() (below). The
 * major action is to come up with a message for stderr.
 */
static          Xv_error_action
xv_default_error(object, severity, avlist)
    Xv_object       object;
    Xv_error_severity severity;
    Attr_attribute     avlist[ATTR_STANDARD_SIZE];
{
    Attr_avlist     attrs;
#define MAXMSGSIZE 256
    char            s[MAXMSGSIZE];
    XErrorEvent    *xerror;
    char           *pkg_name = 0;
    char           *severity_string;
    char	    print_comma = FALSE;
    register Display *dpy;

    /* Make a brief statement about the error and return abort */

    severity_string = severity == XV_ERROR_NON_RECOVERABLE ? "error" : "warning";
    if (!object)
	(void) fprintf(stderr, "XView %s:", severity_string);
    else
	(void) fprintf(stderr, "XView %s: obj 0x%x", severity_string, object);

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case XV_ERROR_SYSTEM:
	    if ((int) errno < sys_nerr) {
		(void) fprintf(stderr, " %s", sys_errlist[(int) errno]);
	    } else {
		(void) fprintf(stderr, " unix error %d", (int) errno);
	    }
	    print_comma = TRUE;
	    break;
	  case XV_ERROR_BAD_VALUE:
	    (void) fprintf(stderr, " bad value %s",
			   xv_error_attr_sprint(&attrs[0], s));
	    break;
	  case XV_ERROR_CREATE_ONLY:
	    (void) fprintf(stderr, " create only %s",
			   xv_error_attr_sprint(&attrs[1], s));
	    break;
	  case XV_ERROR_CANNOT_SET:
	    (void) fprintf(stderr, " cannot set %s",
			   xv_error_attr_sprint(&attrs[1], s));
	    break;
	  case XV_ERROR_CANNOT_GET:
	    (void) fprintf(stderr, " cannot get %s",
			   xv_error_attr_sprint(&attrs[1], s));
	    break;
	  case XV_ERROR_SERVER:
	    xerror = (XErrorEvent *) attrs[1];
	    dpy = xerror->display;
	    XGetErrorText(dpy, (int) xerror->error_code, s, MAXMSGSIZE);
	    (void) fprintf(stderr, " %s", s);
	    break;
	  case XV_ERROR_STRING:
	    if (print_comma) {
		(void) fprintf(stderr, ",");
		print_comma = FALSE;
	    }
	    (void) fprintf(stderr, " %s", (char *) attrs[1]);
	    break;
	  case XV_ERROR_INVALID_OBJ:
	    (void) fprintf(stderr, " invalid object (%s)", (char *) attrs[1]);
	    break;
	  case XV_ERROR_INTERNAL:
	    break;
	  case XV_ERROR_PKG:
	    pkg_name = (char *) attr_pkg_name((int) attrs[1]);
	    break;
	  case XV_ERROR_PKG_NAME:
	    pkg_name = (char *) attrs[1];
	    break;
	}
    }
    if (pkg_name)
	(void) fprintf(stderr, " (%s package)", pkg_name);
    (void) fprintf(stderr, "\n");
    switch (severity) {
      case XV_ERROR_RECOVERABLE:
	return XV_ERROR_CONTINUE;
      case XV_ERROR_NON_RECOVERABLE:
	return XV_ERROR_ABORT;
      default:
	return XV_ERROR_CONTINUE;
    }
}

/*VARARGS2*/
Xv_private void
xv_error(object, severity, va_alist)
    Xv_object       object;
    Xv_error_severity severity;
va_dcl
{
    va_list         valist;
    Attr_attribute  avarray[ATTR_STANDARD_SIZE];
#ifdef ATTR_MAKE
    Attr_avlist     avlist = avarray;
#else
    Attr_avlist     avlist = (Attr_avlist) &va_alist;
#endif				/* ATTR_MAKE */
    Xv_error_action(*user_error_proc) ();

#ifdef ATTR_MAKE
    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
#else
    if (*avlist == (Attr_attribute) ATTR_LIST) {
	avlist = avarray;
	va_start(valist);
	(void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
	va_end(valist);
    }
#endif				/* ATTR_MAKE */

    if ((user_error_proc = xv_error_proc)) {
	if ((*user_error_proc) (object, severity, avlist) == XV_ERROR_ABORT)
	    exit(1);
    } else {
	if (xv_default_error(object, severity, avlist) == XV_ERROR_ABORT)
	    exit(1);
    }
}
