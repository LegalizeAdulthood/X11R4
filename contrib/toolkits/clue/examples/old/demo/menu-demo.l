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


;;; Change history:
;;;
;;;  Date	Author	Description
;;; -------------------------------------------------------------------------------------
;;; 09/14/87	LGO	Created
;;; 07/26/88    LGO     Updated to use new implementation of menus.

(in-package 'clue-examples :use '(lisp xlib clos cluei))




(defun menu-demo (display)
  ;;	  (add-before-action display 'basic-contact 'trace :motion-notify) ;; debug
  
  
  (let* ((p (make-contact 'top-level-shell :parent display
                          :name 'demo-menu
                          :shadow-width 8)))
    (add-callback p :map #'position-over-mouse p)
    (unwind-protect
	(progn
	  (catch 'exit
	    
	    (menu-choose p '((one :documentation "Item one documentation")
			     (two :documentation (("item two normal documentation")
						  ("item two shift documentation" :state (:shift))))
			     (three :documentation (("Item three documentation")
						    ("Double-click any key to beep"))
				    :select (print hi-there)
				    :event (:any :double-click) :select (eval (print "Greetings")))
			     (four :documentation (("item Four normal documentation" :select (:shift))
						   ("Item Four shifty documentation" :state (:shift))))
			     (five :documentation "Item Five documentation")
			     (six :documentation (("item Six normal documentation" :select (:shift :control))
						  ("Item Six Shift documentation" :state (:shift))
						  ("Item Six Control documentation" :state (:control))))
			     (seven :documentation "Item Seven documentation")
			     (eight :documentation (("Item Eight documentation, line 1")
						    ("Item Eight documentation, line 2")))
			     (exit :documentation "Exit the Menu-Demo"
				   :select  (do-throw exit 'xxx)
				   :font "vgi-20"))
			 :label "Regular Menu"
			 :width 20
			 :handler 'print)
            (setf (contact-state p) :mapped)
	    (loop (process-next-event display))))
      (destroy p))))





(defun popup-demo (display)
  (let* ((p (make-contact 'top-level-shell :parent display
                          :name 'demo-menu
                          :shadow-width 8)))
    (add-callback p :map #'position-over-mouse p)
    (unwind-protect
	(progn
	  (catch 'abort
;;	  (add-before-action display 'basic-contact 'trace :motion-notify) ;; debug
	    
	    
	    (menu-choose p
			 `(one
			    two
			    three
			    four
			    (cascade :cascade
				     (popup-choose ,display
						   (aaaaaaaa
						     bbbbbbbbb
						     (ccccccccc :cascade
								(popup-choose ,display
									      (xxxxx	
										yyyyy
										zzzzz
										)
									      :spring-loaded t
									      :name cascade-2))
						     ddddddddd
						     eeeeeeeee
						     )
						   :spring-loaded t
						   :name cascade-1))
			    six
			    seven
			    eight
			    (exit :select (do-throw abort 'xxx) :font "vgi-20"))
	    
	    :label "Pop-up Cascading menu"
	    :handler #'print
	    :name 'first-entry-to-cascade)
          (setf (contact-state p) :mapped)
	  (loop (process-next-event display))))
      (destroy p))))



(defun popup-demo2 (display)
  (let* ((shell (make-contact 'top-level-shell :parent display)))

    (add-callback shell :map #'position-over-mouse shell)
    (unwind-protect
        (catch 'shell
          (menu-choose shell
                       `((one :select (menu-exit 1))
                         (two :select (menu-exit 2.0))
                         (three :select (menu-exit 3))
                         (four :select (menu-exit 4))
			 (cascade :cascade (popup-choose ,display
							 (aaaaaaaa
							    bbbbbbbbb
							    (ccccccccc :cascade
								       (popup-choose ,display
										     (xxxxx yyyyy zzzzzz)
										     :name cascade2
										     :spring-loaded t
										     :handler menu-exit))
							    ddddddddd
							    eeeeeeeee)
							 :name cascade1
							 :spring-loaded t
							 :handler menu-exit))
			 (six :select (menu-exit 6))
			 (seven :select (menu-exit 7))
			 (eight :select (menu-exit 8)))
		       :name 'popup-demo-2)
          (setf (contact-state shell) :mapped)
	  (loop (process-next-event display)))
      (destroy shell))))








