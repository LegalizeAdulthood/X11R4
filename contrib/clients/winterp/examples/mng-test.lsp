; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         mng-test.lsp
; RCS:          $Header: mng-test.lsp,v 1.1 89/11/25 04:00:28 mayer Exp $
; Description:  Fooling around with managing and unmanaging widget arrays/lists
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:15:31 1989
; Modified:     Sat Nov 25 01:16:55 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(setq buts				;this gets set to an array of 100 pushbuttons
      (do* 
       (;; local vars
	(top_w (send TOP_LEVEL_SHELL_WIDGET_CLASS :new
		     :XMN_GEOMETRY "=600x900+0+0"))
	(rc_w (send XM_ROW_COLUMN_WIDGET_CLASS :new :unmanaged top_w
		    :XMN_ADJUST_LAST nil))
	(i 0 (1+ i))
	(num-buttons 100)
	(buttons (make-array num-buttons))
	)
       (;; test and return
	(eql i num-buttons)
	(xt_manage_children buttons)
	(send rc_w :manage)
	(send top_w :realize)
	buttons				;return array of buttons
	)
       ;; body
       (setf (aref buttons i) 
	     (send XM_PUSH_BUTTON_GADGET_CLASS :new :managed "name" rc_w
		   :XMN_LABEL_STRING (format nil "Button ~A" i)
		   ))
       (send (aref buttons i) :add_callback :XMN_ARM_CALLBACK
	     '()
	     `((format T "Hit Button ~A\n" ,i)
	       ))
       ))

(setq buts2 (make-array 50))
(copy-array buts buts2)
(length buts)
(length buts2)

(xt_unmanage_children buts2)
(xt_manage_children buts2)

(xt_unmanage_children buts)
(xt_manage_children buts)


(setq butlist nil)
(setq butlist 
      (cons (send XM_PUSH_BUTTON_GADGET_CLASS :new :unmanaged "name" rc_w
	     :XMN_LABEL_STRING (format nil "Button ~A" (gensym))
	     )
	    butlist))

(xt_manage_children butlist)
(xt_unmanage_children butlist)

(xt_manage_children (cdr butlist))
(xt_unmanage_children (cdr butlist))
