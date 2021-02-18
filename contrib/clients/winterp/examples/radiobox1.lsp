; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         radiobox1.lsp
; RCS:          $Header: radiobox1.lsp,v 1.1 89/11/25 04:00:35 mayer Exp $
; Description:  The straighforward way to define a radio box.
;               See radiobox2.lsp for a better way using a WINTERP-subclassed
;               toggle-button. Just load this file to see the example.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:22:37 1989
; Modified:     Sat Nov 25 01:23:40 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq toplevel_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
	    :XMN_GEOMETRY "500x500+1+1"
	    :XMN_TITLE "Radio-Box-Test #1"
	    :XMN_ICON_NAME "Radio-Box-Test #1"
	    ))

(setq rowcol_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :radio_box "rc" toplevel_w
	    ))

(send toplevel_w :realize)

(do* 
 (;; local vars
  (i 0 (1+ i))
  but_w
  (buttons nil)
  )
 (;; test and return
  (eql i 20)
  )
 ;; body
 (setq but_w
       (send XM_TOGGLE_BUTTON_GADGET_CLASS :new :managed rowcol_w
	     :XMN_LABEL_STRING (format nil "Button ~A" i)
	     ))
 (send but_w :set_callback :xmn_value_changed_callback
       '(callback_widget callback_set)
       '((if callback_set
	     (print_set_button callback_widget buttons)
	   )))
 (setq buttons (cons (cons but_w i) buttons))
 )


(defun print_set_button (widget buttons)
  (format T "Selected button ~A\n" (cdr (assoc widget buttons)))
  )
