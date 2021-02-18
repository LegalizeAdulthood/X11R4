; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         popen.lsp
; RCS:          $Header: popen.lsp,v 1.1 89/11/25 04:00:31 mayer Exp $
; Description:  Play around with  POPEN to collect unix data.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:51:55 1989
; Modified:     Sat Nov 25 01:59:06 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun ls (dirname)
  (do* 
   ((fp (popen (strcat "/bin/ls -1 -r " dirname) :direction :input))
    (line (read-line fp) (read-line fp))
    (result (list line) (cons line result))
    )
   ((null line)
    (pclose fp)
    (cdr result)
    )
   )
  )

(defun vls (dirname)
  (do* 
   ((fp (popen (strcat "/bin/ls -1 -r " dirname) :direction :input))
    (line (read-line fp) (read-line fp))
    )
   ((null line)
    (pclose fp)
    )
   (send (send XM_PUSH_BUTTON_GADGET_CLASS :new :managed rc_w
	       :XMN_LABEL_STRING line)
	 :add_callback :XMN_ARM_CALLBACK '()
	 `((system (strcat "ge " ,dirname "/" ,line))))
   )
  )

(defun mh-scan (foldername msgs)
  (do* 
   ((fp (popen (strcat "scan -reverse +" foldername " " msgs) 
	       :direction :input))
    (line (read-line fp) (read-line fp))
    (result (list line) (cons line result))
    )
   ((null line)
    (pclose fp)
    (cdr result)
    )
   )
  )

(mh-scan "inbox" "all")
(vls "/users/mayer")
