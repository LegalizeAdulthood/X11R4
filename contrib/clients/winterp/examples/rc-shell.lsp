; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         rc-shell.lsp
; RCS:          $Header: rc-shell.lsp,v 1.1 89/11/25 04:00:38 mayer Exp $
; Description:  Load this file to put up a default rowcolumn shell for
;               experimentation purposes. 
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:27:22 1989
; Modified:     Sat Nov 25 01:29:00 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun make-rc-shell (s_geom)
  (let*
      ((toplevel_w (send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
			 :XMN_GEOMETRY s_geom
			 :XMN_TITLE "Winterp:RC-SHELL"
			 :XMN_ICON_NAME "W:RC-SHELL"))
       (scrl_w (send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed
		     "sc" toplevel_w
		     :XMN_SCROLLING_POLICY :AUTOMATIC))
       (rowcol_w (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
		       "rc" scrl_w
		       :XMN_ORIENTATION :VERTICAL
		       :XMN_PACKING :PACK_TIGHT
		       :XMN_ENTRY_ALIGNMENT :ALIGNMENT_CENTER
		       :XMN_FOREGROUND "Black"
		       :XMN_BACKGROUND "LightGray")))

    (send toplevel_w :realize)
    (setq rc_w rowcol_w)
    (setq top_w toplevel_w)
    )
  )

(make-rc-shell "=360x720+0+0")
(format T "RC-SHELL Loaded:\n")
(format T "	TopLevelShell widget instance is in variable TOP_W\n")
(format T "	RowColumn widget instance is in variable RC_W\n")
