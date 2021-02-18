$ PROC_VER = F$ENVIRONMENT("VERIFY_PROCEDURE")
$ IMAGE_VER = F$ENVIRONMENT("VERIFY_IMAGE")
$ SET VERIFY
$ set noon
$ define/nolog c$include [],decw$include,sys$library
$ define/nolog vaxc$include c$include
$ define/nolog c$library sys$library:vaxcdef.tbl
$ cc do_arcs.c
$ cc do_complex.c
$ cc do_copyarea.c 
$ cc do_dots.c
$ cc do_lines.c
$ cc do_movewin.c
$ cc do_rects.c
$ cc do_segs.c
$ cc do_simple.c
$ cc do_text.c
$ cc do_traps.c
$ cc do_triangles.c
$ cc do_valgc.c
$ cc do_windows.c
$ cc do_wlines.c
$ cc x11perf.c
$ cc do_tests.c
$ link /exe=x11perf /map=x11perf.map link.options/options
$ TEMP = F$VERIFY(PROC_VER, IMAGE_VER)
$ exit


