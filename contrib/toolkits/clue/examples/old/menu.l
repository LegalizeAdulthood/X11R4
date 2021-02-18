;;; -*- Mode:Lisp; Package:CLUE-EXAMPLES; Base:10; Lowercase:T; Syntax:Common-Lisp -*-

;;;
;;;			 TEXAS INSTRUMENTS INCORPORATED
;;;				  P.O. BOX 2909
;;;			       AUSTIN, TEXAS 78769
;;;
;;; Copyright (C) 1987 Texas Instruments Incorporated.
;;;
;;; Permission is granted to any individual or institution to use, copy, modify,
;;; and distribute this software, provided that this complete copyright and
;;; permission notice is maintained, intact, in all copies and supporting
;;; documentation.
;;;
;;; Texas Instruments Incorporated provides this software "as is" without
;;; express or implied warranty.
;;;

(in-package 'clue-examples :use '(lisp xlib clos cluei))


(export '( menu					; Contact class
	  multiple-menu				; Contact class
	  spring-loaded-menu			; Contact class
	  menu-choose				; Menu building utility
	  popup-choose				; Popup menu building utility
	  cascade-choose			; Spring-loaded menu building utility
	  position-over-mouse			; Position a contact over the mouse
	  position-right-of			; Position a contact for popup
	  contact-root-position			; Calculate contact position relative to root
	  ))

;; THROW is a special form, wrap it in a function
(defun do-throw (tag value) (throw tag value))


(defcontact menu (composite)
  ((ordering :type (or (member :first :last) function)
	     :initform :last
	     :accessor menu-insert-ordering)
   (border-width :type card16 :initform 0)
   )
  (:resources
    (cursor :initform "arrow"))
  (:documentation "An array of buttons or labels")
  )

(defmethod add-child ((self menu) contact &key)
  "Put CONTACT on its parent's list of managed contacts"
  (with-slots ((manager-children children)
	       (manager-ordering ordering)) self
    (let ((children manager-children)
	  (ordering manager-ordering))
      (unless (member contact children)
	(case ordering
	  (:first (push contact children))
	  (:last (setf children (nconc children (list contact))))
	  (otherwise
	   (if (or (null children)
		   (null (funcall ordering contact (car children))))
	       (push contact children)
	     (do ((c children (cdr c)))
		 ((null (cdr c))
		  (setf (cdr c) (list contact)))
	       ;; Insert contact when ordering "lessp" predicate returns nil
	       (unless (funcall ordering contact (cadr c))
		 (setf (cdr c) (list* contact (cdr c)))
		 (return nil))))))
	(setf manager-children children)))))

(defmethod change-layout ((menu menu) &optional newly-managed)
  (declare (ignore newly-managed))
  (let* ((children (composite-children menu))
	 (width 0)
	 (height (contact-border-width (first children))))
    (dolist (item children)
      (when (managed-p item)
	(let ((border-width (contact-border-width item)))
	  (move item border-width height)
	  (setq width (max width (+ (contact-width item) border-width border-width))
		height (+ height (contact-height item) border-width)))))
    (change-geometry menu :width width :height height :accept-p t)
    (setq width (contact-width menu)) ;; in case we didn't get what we wanted
    (dolist (item (composite-children menu))
      (when (managed-p item)
	(resize item width (contact-height item) (contact-border-width item))))))

(defmethod manage-geometry ((parent menu) contact x1 y1 width1 height1 border-width1 &key)
  ;; Geometry Management for menus.
  ;; This version only manages a single column.
  ;; It needs to be extended to do general row/column arrays.
  (declare (values success-p x y width height border-width))
;  (declare (type contact contact)
;	   (type (or null int16) x1 y1)
;	   (type (or null card16) width1 height1 border-width1)
;	   (values success-p x y width height border-width))
  (let* ((previous (previous-sibling contact));; Find the contact BEFORE this one
	 (x 0)
	 (y 0)
	 (parent-width (contact-width parent))
	 (parent-height (contact-height parent))
	 (width (or width1 (contact-width contact)))
	 (height (or height1 (contact-height contact)))
	 (border-width 0)
	 (success t))
    (declare (type (or null contact) previous)
	     (type (or null int16) x y)
	     (type (or null card16) width height border-width)
	     (type boolean success))
    (when previous
      (setq y (+ (contact-y previous) (contact-height previous))))
    (setq width (max width parent-width))
    (when (or (> width parent-width)
	      (> (+ y height) parent-height))
      (change-geometry parent :width width :height (+ y height) :accept-p t)
      (when (> width parent-width)
	(setq parent-width (contact-width parent)
	      width parent-width)
	(dolist (child (composite-children parent))
	  (when (managed-p child)
	    (resize child width (contact-height child) (contact-border-width child))))))
    (setq success (and (or (null x1) (= x x1))
		       (or (null y1) (= y y1))
		       (or (null width1) (= width width1))
		       (or (null height1) (= height height1))
		       (or (null border-width1) (= border-width border-width1))))
;;    (PV success x y width height border-width)
    (values success x y width height border-width)))


;;;-----------------------------------------------------------------------------
;;; MULTIPLE-MENU

(defcontact multiple-menu (menu)
  ()
  (:documentation "A menu that can have more then one item selected at once."))

(defmethod add-child :after ((self multiple-menu) contact &key)
  ;; Alter the event-translations of button children.
  (when (typep contact 'button)
    (add-event contact :button-release 'notify)))

;;;-----------------------------------------------------------------------------
;;; SPRING-LOADED-MENU

(defcontact spring-loaded-menu (menu)
  ()
  (:documentation "A menu where a mouse button is always pressed and
 an item is selected when the button is released."))

(defmethod add-child :after ((self spring-loaded-menu) contact &key)
  ;; Alter the event-translations of button children.
  (when (typep contact 'button)
    (add-event contact :button-press '(action-display :select t)) ;; Should never get to this
    (add-event contact :button-release 'notify)
    (add-event contact :enter-notify '(action-display :select t))
    (add-event contact :leave-notify 'slide-right '(action-display :select nil))))

(defmethod leave-cascade ((self spring-loaded-menu))
  (apply-callback self :leave))

(defevent spring-loaded-menu :leave-notify leave-cascade) 

;;;-----------------------------------------------------------------------------
;;; easy to use interface

;;; Helper function (we gota get rid of this crock - its slow...)
#+comment
(defun filter-options (allowable options)
  ;; Return those options in allowable
  ;; allowable is a list of keywords, options is a list of keyword/value pairs
  (do ((option options (cddr option))
       (result nil))
      ((endp option) result)
    (when (member (car option) allowable :test #'eq)
      (setq result (list* (car option) (cadr option) result)))))

(defun menu-exit (&rest values)
  "Exit from the current popup-menu returning VALUES"
  (when (car values) ;; when not aborting
    (do-throw (contact-mode *contact*) (values-list values))))

(defmethod popup-abort ((contact menu))
  ;; Throw out of a popup menu.
  ;; Don't throw when there's no popup above our level.
  (let ((mode (contact-super-mode contact)))
    (when mode
      (apply-callback contact :abort)
      (do-throw mode nil))))

(defun cascade-exit (contact)
  (let ((mode (contact-mode contact)))
    (when mode
      (apply-callback contact :abort)
      (do-throw mode nil))))

(defevent menu (:enter-notify :ancestor :virtual :nonlinear :nonlinear-virtual)
;; Abort on any entry-notify EXCEPT :ancestor, which
;;   may happen when the mouse slides between a menu-item and
;;   the gap between a menu item and the edge of the menu.
  popup-abort) ;; Throw to the NEXT level catch


(defun menu-choose (parent alist &rest options &key label
		    (mode :exclusive)
		    (handler #'menu-exit)
		    (menu-type 'menu) (item-type 'button)
		    (justify :center) (font "fg-18") &allow-other-keys)
  "Display a menu on parent from alist.
 Alist entries are (stringable . options) where options are keyword-value pairs:
 :title string
 :font font
 :justify (member :left :center :right)
 :select (or function list)"
  
  
  (let ((menu (apply #'make-contact menu-type :parent parent :allow-other-keys t options)))
    (when label
      (let ((args (and (consp label) (cdr label)))
	    (title (if (consp label) (car label) label)))
	(apply #'make-contact 'label :parent menu :name 'label :title title args)))
    (add-mode menu mode 'ignore-action)
    (dolist (entry alist)
      (let ((name entry)
	    (args nil)
	    (item-title entry))
	(when (consp entry)
	  (setq name (car entry)
		item-title name
		args (cdr entry)))
	(when (stringp name)
	  (setq name (make-symbol (string-upcase name))))
	(do* ((arg args (cddr arg))
	      (value (second arg) (second arg))
	      (item-justify justify)
	      (item-font font)
	      (item-doc nil)
	      (events nil)
	      (callback :select)
	      (callbacks nil)
	      (options nil))
	     ((endp arg)
	      ;; Set default event handler
	      (unless (assoc :select callbacks :test #'eq)
		(push (if (consp handler)
			  `(:select ,(cons (first handler)
					   (append (rest handler)
						   (list (if (consp entry)
                                                             (first entry)
                                                             entry)))))
			  `(:select ,(list handler (if (consp entry)
                                                       (first entry)
                                                       entry))))

;			  `(:select ,@(append handler (list entry)))
;			  `(:select ,handler ,entry))
		      callbacks))
	      ;; Add item to menu
	      (let ((item (apply #'make-contact
				 item-type
				 :parent menu
				 :name name
				 :title item-title
				 :callbacks callbacks
				 :justify item-justify
				 :font item-font
				 :documentation item-doc
				 options
				 )))
		;; Add events (if any) to item
		(dolist (event events)
		  (add-event item `(:button-release ,@(first event))
			     `(notify ,(second event))
			     '(action-display :select nil)))))
	  ;; Gather arguments
	  (ecase (car arg)
	    (:cascade (push `(:cascade ,value) callbacks)
		      (setq options (append '(:cursor "sb_right_arrow") options)))
	    (:justify (setq item-justify value))
	    (:title (setq item-title value))
	    (:font (setq item-font value))
	    (:documentation (setq item-doc value))
	    (:select
	     (if (consp value)
		 (push `(,callback ,(list* (first value) (rest value))) callbacks)
		 (push `(,callback ,(list value)) callbacks)))

;		 (push `(,callback ,@value) callbacks)
;		 (push `(,callback ,value) callbacks)))
	    (:event (setq callback (gensym))
		    (unless (consp value) (setq value (list value)))
		    (push `(,value ,callback) events))))))
    menu))

(defun popup-choose (display alist &rest options &key  x y  spring-loaded &allow-other-keys)
  "Popup a menu on parent from alist.
 Returns NIL if aborted (pointer moved outside the menu),
 else the value returned by the :SELECT callback of the selected button.
 Alist entries are (stringable . options) where options are keyword-value pairs:
 :font font
 :justify (member :left :center :right)
 :select (or function list)"
  ;; Get the parent
						;(when (and (null parent) (null *contact*))
						;  (xlib::required-arg parent))
  ;; Create the menu and popup-shell
  (let* ((popup (make-contact
		  'top-level-shell :parent display
		  :x x :y y 
		  :spring-loaded spring-loaded
		  :name (gensym) ;; debug
		  
		  ))
	 (menu-type (if spring-loaded 'spring-loaded-menu 'menu)))
    ;; Create MENU, which is a child of POPUP.
    ;; The default action for MENU's item's is a throws 
    ;; which returns the item to the top-level popup.
    (apply #'menu-choose popup alist
	   :menu-type menu-type
	   :mode (if spring-loaded :spring-loaded :exclusive)
	   options)
    ;; Default position is centered over the mouse
    (add-callback popup :map #'(lambda () (unless (and x y)
					    (position-over-mouse popup))))
    ;;
    ;; The event-loop
    (unwind-protect (catch popup
                      (setf (contact-state popup) :mapped)
		      (loop (process-next-event (contact-display popup)))
		      ) 
      (destroy popup)
      )))


(defun cascade-choose (alist &rest options)
  "Display a menu on parent from alist.
 Alist entries are (stringable . options) where options are keyword-value pairs:
 :font font
 :justify (member :left :center :right)
 :select (or function list)"
  (apply 'popup-choose alist :spring-loaded t options))

(defun position-over-mouse (contact)
  "Position CONTACT centered over the mouse.
 Ensures CONTACT remains inside its parent."
  (declare (type contact contact))
  (let ((parent (contact-parent contact))
	(width (contact-width contact))
	(height (contact-height contact)))
    ;; Ensure parent is realized
    (unless (realized-p parent)
      (error "Parent of ~s not realized." contact))
    ;; Find mouse position relative to parent
    (multiple-value-bind (x y)
	(query-pointer parent)
      ;; Center contact over mouse
      (decf x (floor width 2))
      (decf y (floor height 2))
      (when (minusp x) (setq x 0))
      (when (minusp y) (setq y 0))
      ;; Ensure contact is within its parent
      (let ((dx (- (contact-width parent) (+ x width)))
	    (dy (- (contact-height parent) (+ y height))))
	(when (minusp dx) (setq x (max (+ x dx) 0)))
	(when (minusp dy) (setq y (max (+ y dy) 0))))
      ;; Move the contact
      (change-geometry contact :x x :y y))))

(defun contact-root-position (contact)
  "Return the position of CONTACT relative to the root."
  (declare (type contact contact)
	   (values x y))
  (do ((p (contact-parent contact) (contact-parent p))
       (x (contact-x contact))
       (y (contact-y contact)))
      ((null p) (values x y))
    (incf x (contact-x p))
    (incf y (contact-y p))))

(defun position-right-of (contact relative-to)
  "Position CONTACT (usually a cascading menu) to the right of
  RELATIVE-TO (usually CONTACT's parent).
 CONTACT is constrained to be within the root window."
  (declare (type contact contact relative-to))
  (let ((width (contact-width contact))
	(height (contact-height contact))
	x y)
    (multiple-value-setq (x y)
      (contact-root-position relative-to))
    (setq x (+ x (contact-width relative-to) -20)
	  y y)
    (let* ((root (contact-root contact))
	   (dx (- (contact-width root) (+ x width 5)))
	   (dy (- (contact-height root) (+ y height))))
      (when (minusp dx) (setq x (max (+ x dx) 0)))
      (when (minusp dy) (setq y (max (+ y dy) 0))))
      ;; Move the contact
      (change-geometry contact :x x :y y)))