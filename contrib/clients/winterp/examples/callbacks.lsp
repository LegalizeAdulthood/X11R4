; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         callbacks.lsp
; RCS:          $Header: callbacks.lsp,v 1.1 89/11/25 04:00:12 mayer Exp $
; Description:  Demonstration of using callbacks and timeouts.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:01:08 1989
; Modified:     Sat Nov 25 01:01:54 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
TEST CALLBACKS AND TIMEOUTS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun get-date ()
  (let*
      ((pipe (popen "/bin/date" "r"))
       (str (read-line pipe))
       )
    (pclose pipe)
    str))

(defun make-rc-shell ()
  (setq top_w (send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
		    :XMN_GEOMETRY "=500x900+0+0"
		    :XMN_TITLE (get-date)
		    :XMN_ICON_NAME "win-test"))
			    
  (setq rc_w (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed top_w))

  (send top_w :realize)
)


(make-rc-shell)

(setq start_but_w 
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed 
	    "start"
	    rc_w 
	    :xmn_foreground "green"
	    :xmn_background "black"
	    ))

(send start_but_w :set_values 
      :xmn_foreground "black"
      :xmn_background "green")

(send start_but_w :set_callback :XMN_ARM_CALLBACK
      '()
      `((setq to 
	      (xt_add_timeout
	       1000 
	       '((send ,XM_PUSH_BUTTON_WIDGET_CLASS :new :managed 
		       (get-date) ,rc_w
		       :xmn_background "magenta")
		 (setq to (xt_add_timeout 1000 TIMEOUT_OBJ))
		 )
	       ))
	)
      )

(setq stop_but_w 
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed 
	    "stop"
	    rc_w 
	    :xmn_foreground "red"
	    :xmn_background "black"
	    ))

(send stop_but_w :set_callback :XMN_ARM_CALLBACK
      '()
      '((xt_remove_timeout to)
	(format t "quack\n"))
      )
