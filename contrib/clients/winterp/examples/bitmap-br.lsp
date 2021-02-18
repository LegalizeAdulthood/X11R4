; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         bitmap-br.lsp
; RCS:          $Header: bitmap-br.lsp,v 1.1 89/11/25 04:00:07 mayer Exp $
; Description:  Given a directory of X11 bitmaps at location
;               <bitmap_directory_path>, the function 
;                      (BROWSE-BITMAP-DIRECTORY <bitmap_directory_path>)
;               will put up a browser that will allow you to change your root
;               pixmap pattern by clicking on a bitmap image in the browser.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 00:53:06 1989
; Modified:     Sat Nov 25 01:00:31 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun browse-bitmap-directory (dir)
  (let* (
	 (top_w (send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
		      :XMN_GEOMETRY "=360x720+0+0"
		      :XMN_TITLE (strcat "Bitmap browser: " dir)
		      :XMN_ICON_NAME (strcat "Bitmaps[" dir "]")))
	 (sc_w (send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed
		     "sc" top_w
		     :XMN_SCROLLING_POLICY :AUTOMATIC))
	 (rc_w (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
		     "rc" sc_w
		     :xmn_orientation :vertical
		     :xmn_packing :pack_tight
		     :xmn_entry_alignment :alignment_center
		     :xmn_foreground "Black"
		     :xmn_background "LightGray"))
	 )
    (do* 
     ((fp (popen (strcat "/bin/ls " dir) :direction :input))
      (name (read-line fp) (read-line fp))
      bitmap-file
      )
     ((null name)
      (pclose fp)
      (send top_w :realize)
      )
     (setq bitmap-file (strcat dir "/" name))
     (format T "name=~A\n" name)
     (send XM_LABEL_GADGET_CLASS :new :managed
	   "filename" rc_w
	   :XMN_LABEL_TYPE :STRING
	   :XMN_LABEL_STRING name)
     (send (send XM_PUSH_BUTTON_GADGET_CLASS :new :managed
		 "image" rc_w
		 :XMN_LABEL_TYPE :PIXMAP
		 :XMN_LABEL_PIXMAP bitmap-file)
	   :add_callback :XMN_ARM_CALLBACK '()
	   `((xsetroot ,bitmap-file))
	   )
     (send XM_SEPARATOR_GADGET_CLASS :new :managed
	   "sep" rc_w
	   :XMN_SEPARATOR_TYPE :DOUBLE_LINE)

     )
    )
  )

(defun xsetroot (filename)
  (system (format nil "xsetroot -bitmap ~A -fg Black -bg DimGrey" filename)))
