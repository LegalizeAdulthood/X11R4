#! /bin/sh
#
# xon: start an X11 client on another host
#
# Runs the given command on another host with $DISPLAY set in the environment.
#    Does an xhost to allow access to the other host.
#    Xon is very careful not to leave any extra processes waiting
#       around on either machine for the client to exit.  In order
#       to achieve this, csh is used on the remote host and all inputs
#       and outputs are redirected to /dev/null.
#    The -debug switch allows you to see errors, but doesn't worry about
#       leaving extra processes around.
#    If the given command starts with "xterm", it adds the arguments
#        "-name xterm-$hostname"
#        where $hostname is the name of the remote host.  This allows
#        you to customize your server's xrdb database to set attributes,
#        such as the window color, based on the name of remote host.
#
# Written by Jim Dempsey <jjd@bbn.com> with help from Stephen Gildea
#   <gildea@bbn.com> based on an idea by Dave Mankins <dm@think.com>.
#   Originally written around 1987.
#   Last modification Nov 22 1989 by jjd@bbn.com.

if [ "$1" = "-debug" ]; then
    debug=t; shift
fi

case $# in
    0|1) echo "usage: $0 [-debug] host cmd [args]" 1>&2; exit 1;;
esac

host="$1"; shift
thishost=`hostname`

command=$1; shift
if [ "$command" = "xterm" ]; then
    command="$command -name xterm-$host"
fi
if [ $# -ge 1 ]; then
    command="$command $@"
fi

# If run on the same host as server, modify $DISPLAY for the remote host.
newdisplay="`echo $DISPLAY | sed \"s/^unix:/${thishost}:/\"`"

# If run on the same host as server, allow remote host access X server.
if ( echo $DISPLAY | grep -s "^unix:" ); then
    if [ "$debug" ]; then
        xhost +$host
    else
        xhost +$host > /dev/null 2>&1
    fi
fi

# Do the real work using rsh.
if [ "$debug" ]; then
    echo "$0: Going to execute \"$command\" on $host."
    rsh $host -n /bin/csh -cf "\"setenv DISPLAY $newdisplay; setenv TERM xterm; $command\""
else
    exec rsh $host -n exec /bin/csh -cf "\"setenv DISPLAY $newdisplay; setenv TERM xterm; exec $command\" < /dev/null >& /dev/null"
fi

exit 0
