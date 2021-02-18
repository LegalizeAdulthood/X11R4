; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         trans.lsp
; RCS:          $Header: trans.lsp,v 1.1 89/11/25 04:00:46 mayer Exp $
; Description:  Tests of Xt trnaslation, accelerator facilies. Also test
;               winterp's "Lisp()" action, which allows you to call the
;               lisp evaluator from a translation/accelerator table.
; Author:       Niels Mayer, HPLabs
; Created:      Fri Nov 24 20:16:35 1989
; Modified:     Fri Nov 24 20:18:49 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;; Test use of 'Lisp' action to call lisp evaluator through translation
;; or accelerator table entry:
;;

(setq trans
      (XT_PARSE_TRANSLATION_TABLE 
       "Ctrl<Key>L: Lisp(quack action_widget action_xevent) \
Ctrl<Key>K: Lisp(print 'ctrl-key-k-hit)"
       ))

(setq accel
      (XT_PARSE_ACCELERATOR_TABLE
       "Ctrl<Key>L: Lisp(quack action_widget action_xevent) \
Ctrl<Key>K: Lisp(print 'ctrl-key-k-hit)"
       ))

(defun quack (w e)
  (format T "widget=~A; event=~A\n" w e)
)

(setq pb_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb_w" rc_w
	    ))
(send pb_w :augment_translations trans)
(send pb_w :override_translations trans)
(send pb_w :augment_translations "Ctrl<Key>A: Lisp(print 'ctrl-key-a-hit)")
(send pb_w :uninstall_translations)	;this completely disables widget!


;;
;; Accelerators
;;
(send rc_w :set_values
      :xmn_accelerators "Ctrl<Key>A: Lisp(print 'ctrl-key-a-hit)"
      )

(progn					;install accelerator on  rc_w
					;onto each pushbuttonwe create
  (setq pb_w
	(send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb_w" rc_w
	      ))
  (send pb_w :install_all_accelerators rc_w)
  )
