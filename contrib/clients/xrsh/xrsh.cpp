/**/#!/bin/sh
/**/#
/**/# A frontend to rsh to start an X client on another host.
/**/#
/**/# A version of this was originally posted to Usenet in
/**/# 12/88 by Chris Torek (chris@mimsy.umd.edu).
/**/# Enhanced by Marion Hakanson (hakanson@cse.ogc.edu).
/**/# $Id: xrsh,v 1.8 89/10/13 14:06:35 hakanson Rel $
/**/#
/**/# Simply exports $DISPLAY, and runs the given command.
/**/#
/**/# Uses /bin/sh to factor out the user's default remote shell.
/**/# Redirects I/O to allow rsh & rshd to exit immediately, and uses
/**/# exec, both in order to minimize the number of processes involved.
/**/#
/**/# Set "out" to /dev/null if you don't care to have access to
/**/# diagnostics from the last xrsh invocation on target machine.

case $# in
0|1) echo "usage: $0 host cmd [args]" 1>&2; exit 1;;
esac

: ${DISPLAY?"environment variable DISPLAY not set"}

case "X$DISPLAY" in
Xunix:*)
  : ${HOSTNAME=`hostname`}
  DISPLAY=`echo "$DISPLAY" | sed -e "s/^unix:/${HOSTNAME}:/"`
  ;;
X:*)
  : ${HOSTNAME=`hostname`}
  DISPLAY="$HOSTNAME$DISPLAY"
  ;;
esac

host="$1"
shift
out=".xrsh.out"

rsh "$host" -n exec /bin/sh -c \
   "'DISPLAY=\"$DISPLAY\"; export DISPLAY; exec $@ </dev/null >$out 2>&1'"
