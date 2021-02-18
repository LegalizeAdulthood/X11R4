/**/#!/bin/sh
/**/#
/**/# 
/**/#  Copyright 1989 Purdue Research Foundation
/**/# 
/**/#  Permission to use, copy, modify, distribute, and sell this software 
/**/#  and its documentation for any purpose is hereby granted without fee, 
/**/#  provided that the above copyright notice appear in all copies and that 
/**/#  both that copyright notice and this permission notice appear in 
/**/#  supporting documentation, and that the name of Purdue not be used in 
/**/#  advertising or publicity pertaining to distribution of the software 
/**/#  without specific, written prior permission.  Purdue makes no 
/**/#  representations about the suitability of this software for any purpose.  
/**/#  It is provided "as is" without express or implied warranty.
/**/# 
/**/#  PURDUE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
/**/#  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
/**/#  PURDUE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
/**/#  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
/**/#  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
/**/#  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
/**/#  SOFTWARE.
/**/# 
/**/#  Author:  George Kraft IV (gk4@staff.cc.purdue.edu)
/**/#           Purdue University Computing Center
/**/#           Mathematical Sciences Building
/**/#           West Lafayette, IN 47907
/**/# 
/**/#
/**/#	startx(1L) - Robustly start X on a workstation/displaystation,
/**/#		     employing the user configuration files provided.
/**/#			
/**/#			
/**/#	WORKSTATIONS:		startx
/**/#			
/**/#	DISPLAYSTATIONS: 	exec; kill -9 $$

PATH=$PATH:BINDIR:UNSUPBINDIR

/**/#	
/**/#	Current working directory is home.
/**/#

cd

/**/#
/**/#  Parse the arguments.
/**/#

usage="USAGE: $0 [-d | -w] [-x <startup> | [-r <resources>] [-m <wmgr>]]"

/**/# How to chop an argument off.
cut='P=$1
shift
set __ -`expr "$P" : '\''-.\(.*\)'\''` "$@"; shift'

/**/# "dwm:r:x:"
while [ $# -gt 0 ]; do
	case "$1" in
	-d)
		if [ "true" = "${WFLAG-}" ]; then 
			echo "$usage" 2>&1
			exit
		fi
		DFLAG=true
		WFLAG=false
		shift
		;;
	-d*)
		if [ "true" = "${WFLAG-}" ]; then 
			echo "$usage" 2>&1
			exit
		fi
		DFLAG=true
		WFLAG=false
		eval "$cut"
		;;
	-w)
		if [ "true" = "${DFLAG-}" ]; then 
			echo "$usage" 2>&1
			exit
		fi
		WFLAG=true
		DFLAG=false
		shift
		;;
	-w*)
		if [ "true" = "${DFLAG-}" ]; then 
			echo "$usage" 2>&1
			exit
		fi
		WFLAG=true
		DFLAG=false
		eval "$cut"
		;;
	-r)
		xdefaults=$2
		shift ; shift
		;;
	-r*)
		xdefaults=`expr "$1" : '-r\(.*\)'`
		shift
		;;
	-m)
		wmrc=$2
		shift ; shift
		;;
	-m*)
		wmrc=`expr "$1" : '-m\(.*\)'`
		shift
		;;
	-x)
		xinitrc=$2
		shift ; shift
		;;
	-x*)
		xinitrc=`expr "$1" : '-m\(.*\)'`
		shift
		;;
	--)
		shift
		break
		;;
	-*)
		echo "$usage" 1>&2 
		exit 1
		;;
	*)
		break
		;;
	esac
done

if [ $# -gt 0 ]; then
	echo "$usage" 1>&2 
	exit 1
fi

if [ "" != "${xinitrc-}" ]; then 
	if [ "" != "${wmrc-}" ]; then 
		echo "$usage" 1>&2 
		echo -n "Window manager option unexpected " 1>&2
		echo "with X startup option." 1>&2
		exit 1
	fi
	if [ "" != "${xdefaults-}" ]; then 
		echo "$usage" 1>&2 
		echo "X resources option unexpected with X startup option." 1>&2
		exit 1
	fi
fi

TTYNAME=`tty | sed -e "s/\/dev\///"`

if [ "${WFLAG-}" = "" ]; then
	case "$TTYNAME" in
	console | co)
		WFLAG="true"
		DFLAG="false"
		;;
	*)
		DFLAG="true"
		WFLAG="false"
		;;
	esac
fi

/**/#
/**/#	Find a xrdb(1L) configuration file.
/**/#

xrdbconfigs="${xdefaults-} .Xdefaults .xdefaults .Xresources .xresources"

for config in $xrdbconfigs; do
	if [ -r $config -a -s $config ]; then
		xrdbconfig="$config"
		break;
	fi
done

/**/#
/**/#	Find a X configuration file.
/**/#

xconfigs="${xinitrc-} ${XINITRC-} .Xinit .xinit .Xinitrc .xinitrc .xsession"

for config in $xconfigs; do
	if [ -r $config -a -s $config ]; then

		xconfig="$config"

/**/		#
/**/		# Determine the shell of the X config file.
/**/		#

		if [ "`head -1 $xconfig | grep "/bin/sh"`" ]; then
			shell="/bin/sh"
		else
			shell="/bin/csh"
		fi

		break;
	fi
done

/**/#
/**/#	Find a window manager configuration file.
/**/#

wmgrconfigs="${wmrc-} .twmrc .awmrc .uwmrc"

for config in $wmgrconfigs; do
	if [ -r $config -a -s $config ]; then
		wmgrconfig="$config"
		break;
	fi
done

/**/#
/**/#	Determine which error file to use.
/**/#

xerrors=${XERRORS=.xerrors}

/**/#
/**/#	Does the user know which DISPLAY to send to? 
/**/#

if [ "${DFLAG-}" = "true" -a "${DISPLAY-}" = "" ]; then
	export DISPLAY
	DISPLAY=`who am i | sed -e 's/\(.*\)(\(.*\))/\2/'`:0
	echo "Environment variable DISPLAY was set to $DISPLAY"
elif [ "${WFLAG-}" = "true" -a "${DISPLAY-}" = "" ]; then
	export DISPLAY
	DISPLAY=`hostname|sed -e 's/\.edu$//' -e 's/\.purdue$//' -e 's/\.cc$//'`:0
fi

/**/#
/**/#	Start X.
/**/#

if [ "${xconfig-}" != "" ]; then
	if [ -r $xconfig -a -s $xconfig ]; then
		if [ "${WFLAG-}" = "true" ]; then
			xinit /bin/sh -c "DISPLAY=$DISPLAY $shell $xconfig" \
			> $xerrors 2>&1; kbd_mode -a; clear
			exit 0
		else
			echo -n "Suspend your telnet(1C) session and wait for "
			echo    "the window manager."
			($shell $xconfig > $xerrors 2>&1 &)
			exit 0
		fi
	fi
fi

if [ "${WFLAG-}" = "true" ]; then
	(xinit /bin/sh -c "DISPLAY=$DISPLAY xterm -C" > $xerrors 2>&1; \
	kbd_mode -a; clear &)
	sleep 10
	if [ -r .xauth -a -s .xauth ]; then
		xhost + > $xerrors 2>&1
	fi
else
	echo "Suspend your Telnet(1C) session and wait for the window manager."
fi

case "$wmgrconfig" in
.awmrc)
	(awm > $xerrors 2>&1 &)
	;;
.uwmrc)
	(uwm > $xerrors 2>&1 &)
	;;
.twmrc | *)
	(twm > $xerrors 2>&1 &)
	;;
esac

if [ "${xrdbconfig-}" != "" ]; then
	xrdb $xrdbconfig > $xerrors 2>&1
fi

if [ "${WFLAG-}" = "true" ]; then
	wait
else
	(xterm > $xerrors 2>&1 &)
fi
