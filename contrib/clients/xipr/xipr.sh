#! /bin/csh -f
set flags=() xflags=() noglob
unset display
unsetenv display
set display=$DISPLAY

top:
if ($#argv > 0) then
    switch ($argv[1])
	case "*:*":
	    set display=$argv[1]
	    shift argv
	    goto top

	case -nobdrs:
	    set xflags = ($xflags $argv[1])
	    shift argv
	    goto top

	case -*:
	    set flags = ($flags $argv[1])
	    shift argv
	    goto top

    endsw
endif

xwd -xy -display $display | bitimp $flags

