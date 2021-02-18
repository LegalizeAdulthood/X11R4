$ PROC_VER = F$ENVIRONMENT("VERIFY_PROCEDURE")
$ IMAGE_VER = F$ENVIRONMENT("VERIFY_IMAGE")
$ SET VERIFY
$ set noon
$ define/nolog c$include [],decw$include,sys$library
$ define/nolog vaxc$include c$include
$ define/nolog c$library sys$library:vaxcdef.tbl
$ link/debug /exe=x11perf /map=x11perf.map link.options/options
$ TEMP = F$VERIFY(PROC_VER, IMAGE_VER)
$ exit


