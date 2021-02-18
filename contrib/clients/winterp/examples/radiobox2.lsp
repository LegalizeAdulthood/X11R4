; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         radiobox2.lsp
; RCS:          $Header: radiobox2.lsp,v 1.1 89/11/25 04:00:36 mayer Exp $
; Description:  A better (?) way of creating a radio box, using subclassing of
;               togglebutton. Note that this version doesn't waste as much
;               memory as radiobox1.lsp because it defines a single
;               entry-callback on the rowcolumn widget instead of forcing each
;               toggle-button to have separate copies of very similar
;               callback-closures. Just load this file to see the example.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:24:00 1989
; Modified:     Sat Nov 25 01:26:42 1989 (Niels Mayer) mayer@hplnpm
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
	    :XMN_TITLE "Radio-Box-Test #2"
	    :XMN_ICON_NAME "Radio-Box-Test #2"
	    ))

(setq rowcol_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :radio_box "rc" toplevel_w
	    ))

(send toplevel_w :realize)

(send rowcol_w :set_callback :xmn_entry_callback
       '(CALLBACK_ENTRY_WIDGET
	 CALLBACK_ENTRY_SET)
       '(
	 (if CALLBACK_ENTRY_SET
	     (send CALLBACK_ENTRY_WIDGET :print-which-button)
	   )
	 ))

;; make a subclass of XM_TOGGLE_BUTTON_GADGET_CLASS
(setq My_Toggle_Button			
      (send Class :new
	    '(button_name)		;a new ivar for this subclass
	    '()				;no class variables for subclass
	    XM_TOGGLE_BUTTON_GADGET_CLASS)) 

;; override XM_TOGGLE_BUTTON_GADGET_CLASS's instance initializer
(send My_Toggle_Button :answer :isnew '(name &rest args)
      '(
	(setq button_name name)
	(apply 'send-super `(:isnew ,@args
				    :xmn_label_string ,name))
	))

;; add a method that prints which button
(send My_Toggle_Button :answer :print-which-button '()
      '(
	(format T "option ~A selected\n" button_name)
	))


(do* 
 (;; local vars
  (i 0 (1+ i))
  )
 (;; test and return
  (eql i 20)
  )
 ;; body
 (send My_Toggle_Button :new (format nil "Button ~A" i) :managed rowcol_w)
 )
