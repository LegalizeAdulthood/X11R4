; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         interact.lsp
; RCS:          $Header: interact.lsp,v 1.1 89/11/25 04:00:21 mayer Exp $
; Description:  Play around with interactive features of winterp. 
;               GET_MOUSED_WIDGET allows you to send a message to any widget
;               that you can see. Thus you can interactively change your
;               interfaces' appearance or functionality without having to
;               remember the name  of the desired widget. Note that this'll
;               even work on big composite widgets that create other widgets
;               internally....
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:36:28 1989
; Modified:     Sat Nov 25 01:42:49 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun invert-moused-widget ()
  (let
      ((widget (get_moused_widget))
       foreground
       background
       )
    (send widget :get_values
	  :XMN_FOREGROUND 'foreground
	  :XMN_BACKGROUND 'background
	  )
    (send widget :set_values
	  :XMN_FOREGROUND background
	  :XMN_BACKGROUND foreground
	  )
    ))

(invert-moused-widget)

(send (get_moused_widget) :get_values
      :XMN_SPACING nil
      :XMN_MARGIN_WIDTH nil
      :XMN_INPUT nil
      :XMN_X nil
      :XMN_Y nil
      :XMN_HEIGHT nil
      :XMN_WIDTH nil
      )

(send (get_moused_widget) :set_values 
      :xmn_font_list "6x10")

;;
;; Note bug: this won't work on top-level-shells for some unknown reason.
;;
(send  (GET_MOUSED_WIDGET) :destroy)

(get_moused_widget)

(send (GET_MOUSED_WIDGET) :set_values :xmn_foreground "red")
(send (GET_MOUSED_WIDGET) :set_values :xmn_foreground "white")
(send (GET_MOUSED_WIDGET) :set_values :xmn_foreground "blue")
(send (GET_MOUSED_WIDGET) :set_values :xmn_foreground "green")
(send (GET_MOUSED_WIDGET) :set_values :xmn_background "lightgrey")
(send (GET_MOUSED_WIDGET) :set_values :xmn_background "grey")
(send (GET_MOUSED_WIDGET) :set_values :xmn_background "dimgrey")

(send (get_moused_widget) :set_values
      :xmn_label_string (symbol-name (gensym))
      )

(send (get_moused_widget) :set_callback :xmn_activate_callback
      '()
      '((send xm_push_button_gadget_class :new :managed "quackquack" rc_w)))


(send (get_moused_widget) :set_callback :xmn_activate_callback
      '(callback_reason callback_xevent)
      '(
	(format T "reason = ~A; event = ~A\n" callback_reason callback_xevent)
	)
      )
