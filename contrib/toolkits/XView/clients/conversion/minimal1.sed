  cat ${1} | \
    sed -e 's%\<alert\.h%notice\.h%g' \
	-e 's%\<ALERT_POSITION\>%/* XView CONVERSION - Alert-Positioning is now defunct, use NOTICE_FOCUS_XY instead\. See API guide, Appendix C */NOTICE_FOCUS_XY%g' \
        -e 's%\<ALERT_%NOTICE_%g' \
        -e 's%\<alert_prompt%notice_prompt%g' \
	-e 's%\<ATTR_COL%/* XView CONVERSION - Use xv_col/xv_cols instead\. See API Guide Sect 2\.2 */ATTR_COL%g' \
        -e 's%\<ATTR_ROW%/* XView CONVERSION - Use xv_row/xv_rows instead\. See API Guide Sect 2\.2 */ATTR_ROW%g' \
        -e 's%\<cms_rainbow\.h%cmsrainbow\.h%g' \
	-e 's%\<Cursor\>%/* XView CONVERSION - Due to a name space clash with Xlib, the SunView data-type Cursor is now Xv_Cursor in XView */Cursor%g' \
	-e 's%\<DEFINE_CURSOR_FROM_IMAGE\>%DEFINE_CURSOR_FROM_IMAGE/* XView CONVERSION - Cursors are now XView objects that must be created, read Sect 4\.2 on how to convert to the new API */%g' \
        -e 's%\<DEFINE_ICON_FROM_IMAGE\>%DEFINE_ICON_FROM_IMAGE/* XView CONVERSION - Icons are now XView objects that must be created, read Sect 4\.3 on how to convert to the new API */%g' \
        -e 's%\<CANVAS_MARGIN\>%/* XView CONVERSION - Read Sect 3\.2 on canvases, this attr now applies to the canvas view margin */CANVAS_MARGIN%g' \
        -e 's%\<CANVAS_PIXWIN\>%/* XView CONVERSION - Pixwins are now logically replaced with paint windows\. Read Sect 3\.2 */CANVAS_PIXWIN%g' \
        -e 's%\<CANVAS_RETAINED\>%/* XView CONVERSION - Now only a hint, must be prepared to repaint, read Sect 2\.5 */CANVAS_RETAINED%g' \
	-e 's%\<CMDSW\>%TERMSW%g' \
        -e 's%\<CURSOR_%/* XView CONVERSION - Many cursor attributes are no longer supported, read Sect 2\.5 & 3\.2 to check if this one is */CURSOR_%g' \
        -e 's%\<emptysubwindow%/* XView CONVERSION - Defunct, read Sect 4\.2 */emptysubwindow%g' \
        -e 's%\<esw_%/* XView CONVERSION - No longer supported in XView, read Sect 4\.2 */esw_%g' \
        -e 's%\<FRAME_ARGC_PTR_ARGV\>%/* XView CONVERSION - Make sure to use xv_init to process the attrs first, Sect 3\.2 */FRAME_ARGC_PTR_ARGV%g' \
        -e 's%\<FRAME_ARGS\>%/* XView CONVERSION - Make sure to use xv_ini tto process the attrs first, Sect 3\.2 */FRAME_ARGS%g' \
        -e 's%\<FRAME_EMBOLDEN_LABEL\>%/* XView CONVERSION - Defunct, simply remove and/or read Sect 3\.2 */FRAME_EMBOLDEN_LABEL%g' \
        -e 's%\<FRAME_NTH_WINDOW\>%/* XView CONVERSION - Iterate using exitsing attrs FRAME_NTH_SUBFRAME and FRAME_NTH_SUBWINDOW instead, Read Sect 3\.2 */FRAME_NTH_WINDOW%g' \
        -e 's%\<FRAME_PROPS_ACTIVE\>%/* XView CONVERSION - Defunct, cannot grey-out property item any longer in OPEN LOOK */FRAME_PROPS_ACTIVE%g' \
        -e 's%\<FRAME_PROPS_PROC\>%/* XView CONVERSION - Properties are now the full responsibility of the application program, no notification from window menu, will want to add props button to app */FRAME_PROPERTIES_PROC%g' \
        -e 's%\<FRAME_SUBWINDOWS_ADJUSTABLE\>%/* XView CONVERSION - Defunct, remove and/or see Sect 3\.2 */FRAME_SUBWINDOWS_ADJUSTABLE%g' \
        -e 's%\<fs_%/* XView CONVERSION - There is now a new FULLSCREEN package, see Sect 3\.2 and 4\.3 for replacement */fs_%g' \
        -e 's%\<fullscreen_%/* XView CONVERSION - Use new FULLSCREEN pkg instead, see Sect 3\.2 and 4\.3 for replacement */fullscreen_%g' \
        -e 's%\<gfxsw\.h%/* XView CONVERSION - Defunct, simply remove  Sect 2\.3 */gfxsw\.h%g' \
        -e 's%\<gfx_%/* XView CONVERSION - Defunct\. GFX subwindows no longer supported in XView  Sect 2\.3 */gfx_%g' \
        -e 's%\<HELP_DATA\>%XV_HELP_DATA%g' \
	-e 's%\<ic_mpr%/* XView CONVERSION - Icon structs no longer valid, use the ICON_IMAGE attr instead */ic_mpr%g' \
	-e 's%\<im_%/* XView CONVERSION - Handling of input masks have changed, see Sect 2\.4, 3\.2 and 4\.3 for any necessary changes */im_%g' \
        -e 's%\<input_read_event\>%/* XView CONVERSION - Should be using xv_input_readevent instead, see Sect 2\.5 or API change change */input_read_event%g' \
	-e 's%\<KBD_REQUEST\>%/* XView CONVERSION - No longer valid in X, simply remove it  Sect 2\.5 */KBD_REQUEST%g' \
        -e 's%\<LINT_CAST\>%/* XView CONVERSION - Now defunct, simply remove it  Sect 2\.5 */LINT_CAST%g' \
        -e 's%\<LOC_RGN\>%/* XView CONVERSION - No longer valid in X, simply remove it  Sect 2\.5 */LOC_RGN%g' \
        -e 's%\<LOC_STILL\>%/* XView CONVERSION - No longer valid in X, simply remove it  Sect 2\.5 */LOC_STILL%g' \
        -e 's%\<LOC_TRAJECTORY\>%/* XView CONVERSION - No longer valid in X, simply remove it  Sect 2\.5 */LOC_TRAJECTORY%g'
