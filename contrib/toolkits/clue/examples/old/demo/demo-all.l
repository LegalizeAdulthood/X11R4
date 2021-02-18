;;; -*- Mode:Lisp; Package:CLUE-EXAMPLES; Base:10; Lowercase:T; Syntax:Common-Lisp -*-
;;;
;;;			 TEXAS INSTRUMENTS INCORPORATED
;;;				  P.O. BOX 2909
;;;			       AUSTIN, TEXAS 78769
;;;
;;; Copyright (C) 1988 Texas Instruments Incorporated.
;;;
;;; Permission is granted to any individual or institution to use, copy, modify,
;;; and distribute this software, provided that this complete copyright and
;;; permission notice is maintained, intact, in all copies and supporting
;;; documentation.
;;;
;;; Texas Instruments Incorporated provides this software "as is" without
;;; express or implied warranty.
;;;

;;;
;;; Change history:
;;;
;;;  Date	Author	Description
;;; -------------------------------------------------------------------------------------
;;; 07/27/88    SLM     Defined demo menu driver.
;;;                     Each "demo" can be run standalone, or from the demo driver.
;;;                     bind XLIB:*recursive-event-queue* for each demo to avoid problems 
;;;                     resulting from the opening of a new contact-display to the same host 
;;;                     as the demo driver.

(in-package 'clue-examples :use '(lisp xlib clos cluei))



;;;----------------------------------------
;;; Take advantage of TI multiprocess support to run mouse doc demo
#+explorer
(defvar *mouse-doc-process nil)

#+explorer
(defun toggle-doc (&optional (host *default-host*))
  (if *mouse-doc-process
      (progn (ticl:send *mouse-doc-process :kill)
	     (setf *mouse-doc-process nil))
    (setf *mouse-doc-process
	  (ticl:process-run-function "mdoc" 'xlib:mouse-doc host :height 3 :border-width 5))))


;;;----------------------------------------
;;; Driver utility routines

(defun execute-the-demo (&rest args)
  (let ((xlib::*recursive-event-queue* nil))
#+explorer
    (catch () ;;keep "abort" in the demos from aborting the driver
	(apply (car args) (cdr args)))
#-explorer
    (apply (car args) (cdr args))))


(defmethod quit-demo ((contact contact) &optional (tag 'quit) value)
  (throw tag value))


(defvar *default-image* (merge-pathnames "ti-logo.xbm" user::*clue-demo-directory*))


(define-resources
  (test * border) 1 							
  (* menu * documentation) "Mouse-Left to select"
  (* demo-menu x) 100
  (* demo-menu y) 100
  )




;;;================================================================================
;;; Demo Driver 



(defun demo-all (&optional (host *default-host*))
  (let (display demo)
    (unwind-protect
	(catch 'exit
	  (setf display (open-contact-display 'test :host host)
		demo (make-contact 'top-level-shell :parent display
				    :shadow-width 8
				    :width 600 :height 600 :x 212 :y 54))
          (x11:x-select-and-enable)
	  (menu-choose demo `(("Regular Menu" :select (execute-the-demo menu-demo ,display))
			      ("Popup Cascading Menu" :select (execute-the-demo popup-demo ,display))
			      ("Another Popup Menu" :select (execute-the-demo popup-demo2 ,display))
			      ("Simple Grapher" :select (execute-the-demo user::simple-graph-demo ,display))
			      ("Lisp Listener" :select (execute-the-demo lisp-listener ,display))
			      ("Scrolling Bitmap" :select (execute-the-demo scroll-a-bitmap ,display ,*default-image*))
			      #+explorer ("Toggle Mouse Documentation Window"
					  :select (execute-the-demo toggle-doc ,host))
			      (exit :documentation "Exit the CLUE Demo"
				    :select (do-throw exit 'xxx) :font "vgi-20")
			      )
		       :label "CLUE Demo Menu")
			     
	  (add-event demo '(:key-press #\q) '(quit-demo exit))
	  (setf (contact-state demo) :mapped)
	  (loop (process-next-event display))
	  )
      (when display (close-display display))
      #+explorer
      (when *mouse-doc-process
	(ticl:send *mouse-doc-process :kill)
	(setf *mouse-doc-process nil))
      )
    ))
