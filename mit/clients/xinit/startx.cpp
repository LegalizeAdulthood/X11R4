/**/#!/bin/sh

/**/# 
/**/# This is just a sample implementation of a slightly less primitive 
/**/# interface than xinit.  It looks for user .xinitrc and .xserverrc
/**/# files, then system xinitrc and xserverrc files, else lets xinit choose
/**/# its default.  The system xinitrc should probably do things like check
/**/# for .Xresources files and merge them in, startup up a window manager,
/**/# and pop a clock and serveral xterms.
/**/#
/**/# Site administrators are STRONGLY urged to write nicer versions.
/**/# 

userclientrc=$HOME/.xinitrc
userserverrc=$HOME/.xserverrc
sysclientrc=XINITDIR/xinitrc
sysserverrc=XINITDIR/xserverrc
clientargs=""
serverargs=""

if [ -f $userclientrc ]; then
    clientargs=$userclientrc
else if [ -f $sysclientrc ]; then
    clientargs=$sysclientrc
fi
fi

if [ -f $userserverrc ]; then
    serverargs=$userserverrc
else if [ -f $sysserverrc ]; then
    serverargs=$sysserverrc
fi
fi

whoseargs="client"
while [ "x$1" != "x" ]; do
    case "$1" in
	/''*|\.*)	if [ "$whoseargs" = "client" ]; then
		    clientargs="$1"
		else
		    serverargs="$1"
		fi ;;
	--)	whoseargs="server" ;;
	*)	if [ "$whoseargs" = "client" ]; then
		    clientargs="$clientargs $1"
		else
		    serverargs="$serverargs $1"
		fi ;;
    esac
    shift
done

xinit $clientargs -- $serverargs

/*
 * various machines need special cleaning up
 */
#ifdef macII
Xrepair
screenrestore
#endif

#ifdef sun
kbd_mode -a
#endif
