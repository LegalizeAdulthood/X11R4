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

;; RMENU is a menu that takes its item list from a ITEMS resource.
;; The ITEMS resource is a list whose elements are item names or
;; lists of (name . parameters).  Item parameters may be specified
;; as resources, or in the item parameter list.
;;
;; The item class comes from the :DEFAULT-CLASS resource of ITEMS.  The
;; ONLY way to override this default is by specifying it in the ITEMS
;; resource.  For example: ((title :class label) item1 ...)

(in-package 'clue-examples :use '(lisp xlib clos cluei))


(defcontact rmenu (multiple-menu)
  ()
  (:resources
    (items :type list)
    (default-class :type symbol :initform 'button)
    (event-mask :initform (make-event-mask :key-press)))
  (:documentation "A menu whose items are described by a :ITEMS resource")
  )

(defmethod initialize-instance :after ((self rmenu) &key items default-class &allow-other-keys)
  ;; Add our items
  (dolist (item items)
    (if (atom item)
	(add-child self (make-contact default-class :parent self :name item))
      (let ((class (getf (cdr item) :class)))
	(when class ;; Remove class from parameters [:class is parameter to xlib:create-window]
	  (setq item (copy-list item))
	  (remf (cdr item) :class))
	(add-child
	  self
	  (apply #'make-contact (or class default-class)
		 :parent self :name (car item) (cdr item)))))))

;;;-----------------------------------------------------------------------------
;;; Test code


(defun rmenu-test (&optional (host "sun4"))
  (let (display)
    (unwind-protect
	(catch 'exit
	  (setq display (open-contact-display 'test :host host))
	  (let* ((p (make-contact 'popup-shell :parent display
				   :shadow-width 8)))
	    ;(SETQ menu
	    (make-contact 'rmenu :name 'lunch :parent p);)
            (setf (contact-state p) :mapped)
	    (loop (process-next-event display))))
      (xlib:close-display display))))

(define-resources
  (* lunch items) '((label :class label :title "Lunch!")
		    peanut-butter
		     (jelly :doit (sandwitch strawberry-jelly) :key #\j)
		     (jam :doit (print "OH GOODY, JAM!"))
		     pickles toast bread
		     exit abort)
  (* lunch peanut-butter callbacks) '((:doit sandwitch skippy))
  (* lunch peanut-butter key) #\p
  (* lunch * doit) 'sandwitch			  ;; Default doit
  (* rmenu exit callbacks) '((:select menu-exit)) ;; CALLBACKS take precidence over DOIT
  (* rmenu exit title) "Do It"
  (* rmenu exit key) #\q
  (* rmenu abort callbacks) '((:select menu-abort)) ;; CALLBACKS take precidence over DOIT
  (* rmenu abort key) #\control-q
  )

#+comment ;; this conflicts with the menu-exit in the menu file.
(defun menu-exit (&optional value)
  ;; Call the :doit action for all siblings
  (let ((parent (contact-parent *contact*)))
    (dolist (sibling (composite-children parent))
      (when (or (not (typep sibling 'button))
		(selected sibling))
	(apply-callback sibling :doit))))
  (throw 'exit value))

(defun menu-abort (&optional value)
  (throw 'exit value))

(defun sandwitch (&optional component) (print (or component *contact*)))

