;;; -*- Mode:Lisp; Package:CLUE-EXAMPLES; Base:10; Syntax:Common-Lisp -*-
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

;;; Description:  Scroll frame composite contact
;;; This will automagically add horizontal and/or vertical scroll bars and the
;;; accompanying scroll functionality to a user-supplied contact.

;;; Change History:
;;; ----------------------------------------------------------------------------
;;;  6/14/88	KK	Created.
;;;  8/24/88    SLM     Change Copyright notice from restricted to "free".
;;;  8/24/88    SLM     Rewrote the initilize-instance :around method
;;;                     (clos-kludge doesnt' handle it right) to be an
;;;                     after method.
;;;  8/25/88    SLM     Rewrote manage-geometry and scroll-frame-configure.
;;;  8/26/88    SLM     Turn on compress-exposures for the scroll-frame.
;;;  8/26/88    SLM     Fix it so a VERTICAL scroll bar does NOT have to be present!


(in-package 'clue-examples :use '(lisp xlib clos cluei))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  SCROLL-FRAME Composite definition


(defcontact scroll-frame (virtual-composite)
  ((horizontal-layout :type     (member :top :bottom nil)
		      :reader   scroll-frame-horizontal-layout  ;;setf defined elsewhere
		      :initarg  :horizontal
		      :initform nil)
   (vertical-layout   :type     (member :left :right nil)
		      :reader   scroll-frame-vertical-layout    ;;setf defined elsewhere
		      :initarg  :vertical
		      :initform nil)
   (default-scroll-bar-width  :type     card32
		      :accessor scroll-bar-width
		      :initarg  :default-scroll-bar-width
		      :initform 20)
   (documentation     :initform "Press the Q key to quit"))
  (:resources
    documentation
    (compress-exposures :initform :on)
    documentation)
  (:documentation "A contact that wraps scroll bars around a contact"))

(defevent scroll-frame (:key-press #\Q) (quit-scroll-frame quit-scroll "key-exit"))
(defevent scroll-frame (:key-press #\q) (quit-scroll-frame quit-scroll "key-exit"))
(defmethod quit-scroll-frame ((self scroll-frame) &optional (tag 'quit-scroll) value)
  (format t "~%~a ~a ~a" self tag value)
  (throw tag value))


(defparameter *default-scroll-bar-width* 20
  "Default pixel size of the valuators in a SCROLL-FRAME.")


(defmethod initialize-instance :after ((self scroll-frame)
					&rest initargs
					&key (scroll-class '(vscroller hscroller))
			                     scroll-initargs
					     (vertical :right)
					     horizontal 
					     inside-width
					     inside-height
					     width
					     height
					     (border-width 1)
					     default-scroll-bar-width
					&allow-other-keys)
  (assert (or width inside-width)
	  nil "Must specify either :WIDTH or :INSIDE-WIDTH.")
  (assert (or height inside-height)
	  nil "Must specify either :HEIGHT or :INSIDE-HEIGHT.")  
  
  (let* ((scroll-width  (or (getf scroll-initargs :width)
			    default-scroll-bar-width
			    *default-scroll-bar-width*))
	 (min-frame-size (+ scroll-width scroll-width border-width border-width))
	 (width         (max min-frame-size
			     (or width
				 (if vertical
				     (+ inside-width scroll-width border-width)
				     inside-width))))
	 (height        (max min-frame-size
			     (or height
				 (if horizontal
				     (+ inside-height scroll-width border-width)
				     inside-height)))))    

    (with-slots ((horizon horizontal-layout) (vert vertical-layout) (w width) (h height)) self
      (setf horizon horizontal
	    vert vertical
	    w width
	    h height))
    
	    

    (let ((default-initargs (nconc (copy-list scroll-initargs) initargs)))
      (when (and vertical horizontal)
	(setf height (max 1 (- height scroll-width))
	      width (max 1 (- width scroll-width))))
      ;; Make scroller children.           
      (when vertical	
	(apply #'make-contact (first scroll-class)
	       :name   :vertical
	       :parent self
	       :width  scroll-width
	       :height height
	       :border-width border-width
	       :allow-other-keys t
	       default-initargs))
      
      (when horizontal  
	(apply #'make-contact (second scroll-class)
	       :name   :horizontal
	       :parent self
	       :width width
	       :height scroll-width
	       :border-width border-width
	       :allow-other-keys t
	       default-initargs)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  Accessors

(defmethod (setf scroll-frame-horizontal-layout) (new-value (self scroll-frame))
  (assert (member new-value '(:top :bottom))
	  nil "Horizontal scroller layout must be either :TOP or :BOTTOM.")
  (setf (slot-value (the scroll-frame self) 'horizontal-layout) new-value)
  (change-layout self))

(defmethod (setf scroll-frame-vertical-layout) (new-value (self scroll-frame))
  (assert (member new-value '(:left :right))
	  nil "Vertical scroller layout must be either :LEFT or :RIGHT.")
  (setf (slot-value (the scroll-frame self) 'vertical-layout) new-value)
  (change-layout self))


(defmethod scroll-frame-body ((self scroll-frame))
  (with-slots (children) self
    (find t children
	  :key #'(lambda (contact)
		   (let ((name (contact-name contact)))
		     (not (or (eq name :horizontal) (eq name :vertical)))))
	  :test #'eq)))


(defmethod scroll-frame-horizontal ((self scroll-frame))
  (with-slots (children) self
    (find :horizontal children :key #'contact-name :test #'eq)))


(defmethod scroll-frame-vertical ((self scroll-frame))
  (with-slots (children) self
    (find :vertical children :key #'contact-name :test #'eq)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  Management/control methods

(defmethod add-child ((self scroll-frame) contact &key)
  ;;need to add some way for a user to specify what
  ;;function is to be the callback for :changed-by-user.
  (flet ((connect-horizontal (frame scroller body)
	   ;; Initialize callbacks and initial values for horizontal scroller
	   (add-callback body :horizontal-reset 'scroll-frame-horizontal-reset frame)
	   (add-callback scroller :changed-by-user 'scroll-horizontal-position body)
	   (scroll-frame-horizontal-reset frame scroller body))
	 
	 (connect-vertical (frame scroller body)
	   ;; Initialize callbacks and initial values for vertical scroller
	   (add-callback body :vertical-reset 'scroll-frame-vertical-reset frame)
	   (add-callback scroller :changed-by-user 'scroll-vertical-position body)
	   (scroll-frame-vertical-reset frame scroller body)))
    
  (with-slots (children) self
    (let ((name (contact-name contact))
	  (body (scroll-frame-body self)))
      (when 
	(case name
	  (:horizontal
	   (assert (not (scroll-frame-horizontal self))
		   nil "Horizontal scroller for ~s already exists." self)
	   (when body (connect-horizontal self contact body))
	   t)
	  
	  (:vertical
	   (assert (not (scroll-frame-vertical self))
		   nil "Vertical scroller for ~s already exists." self)
	   (when body (connect-vertical self contact body))
	   t)
	  
	  (otherwise
	   (assert (not body)
		   nil "A body for ~s already exists." self)
	   (let ((h (scroll-frame-horizontal self))
		 (v (scroll-frame-vertical self)))
	     (when h (connect-horizontal self h contact))
	     (when v (connect-vertical self v contact)))
	   t))

	(setf children (nconc children (cons contact nil)))
	)))
  (setf (contact-state contact) :mapped)
  ))


(defmethod delete-child ((self scroll-frame) contact &key)
  (with-slots (children) self
    (setf children (delete contact children))
    
    (let ((body (scroll-frame-body self)))
      (when body
	(case (contact-name contact)
	  (:horizontal
	   (delete-callback body :horizontal-reset))
	  
	  (:vertical
	   (delete-callback body :vertical-reset)))))))

(defmethod display :after ((self scroll-frame) &optional x y width height &key)
  (declare (ignore x y width height))
  (dolist (child (slot-value (the scroll-frame self) 'children))
    (display child)))

(defmethod manage-geometry ((self scroll-frame) contact
			    cx cy cwidth cheight cborder-width
			    &key)
  (declare (ignore x y width height border-width))
  (with-slots (x y width height border-width) (the contact contact)
    (if (and (eq x cx)
	     (eq y cy)
	     (eq width cwidth)
	     (eq height cheight)
	     (eq border-width cborder-width))
	(values t)
	(values nil x y width height border-width))))

(defmethod resize :after ((self scroll-frame) width height border-width)
  (declare (ignore width height border-width))
  (change-layout self)
  (display self))

(defmethod change-layout ((self scroll-frame) &optional newly-managed)
  "Update child geometry according to current SELF geometry and contents."
  (declare (ignore newly-managed))
  (with-slots (x y width height horizontal-layout vertical-layout border-width) self
    (let* ((v (scroll-frame-vertical self))
	   (h (scroll-frame-horizontal self))
	   (body (scroll-frame-body self))
	   (total-body-border (* 2 (contact-border-width body)))
	   (total-hscroll-border (if h (* 2 (contact-border-width h) 0)))
	   (total-vscroll-border (if v (* 2 (contact-border-width v) 0)))
	   (total-body-width nil)
	   (total-body-height nil)
	   (hscroll-total-height (if h (+ (contact-height h) total-hscroll-border) 0))
	   (vscroll-total-width (if v (+ (contact-width v) total-vscroll-border) 0))
	   (new-body-width (- width vscroll-total-width))
	   (new-body-height (- height hscroll-total-height))
	   )
      (resize body
	      new-body-width
	      new-body-height
	      (contact-border-width body))
      (setf total-body-width (+ (contact-width body) total-body-border)
	    total-body-height (+ (contact-height body) total-body-border))
      (when (and vertical-layout v)
	(ecase vertical-layout
	  (:left (move v 0 0)
		 (move body (+ border-width vscroll-total-width) (contact-y body)))
	  (:right (move v (+ border-width total-body-width) 0)
		  (move body 0 (contact-y body))))
	(resize v vscroll-total-width new-body-height (contact-border-width v))
	)
      (when (and horizontal-layout h)
	(ecase horizontal-layout
	  (:top (move h 0 0)
		(move body (contact-x body) hscroll-total-height))
	  (:bottom (move h 0 (+ border-width total-body-height))
		   (move body (contact-x body) 0)))
	(resize h new-body-width hscroll-total-height (contact-border-width h))
	)
      )))


(defmethod scroll-frame-horizontal-reset ((self scroll-frame) &optional scroller body)
  "Inquire new scrolling parameters and calibrate horizontal scroller."
  (let ((body     (or body (scroll-frame-body self)))
	(scroller (or scroller (scroll-frame-horizontal self))))
    (when (and body scroller)
      (multiple-value-bind (value minimum maximum indicator-size)
	  (scroll-horizontal-initialize body)
	(valuator-calibrate scroller
			    :value value
			    :minimum minimum
			    :maximum maximum
			    :indicator-size indicator-size)))))


(defmethod scroll-frame-vertical-reset ((self scroll-frame) &optional scroller body)
  "Inquire new scrolling parameters and calibrate vertical scroller."
  (let ((body     (or body (scroll-frame-body self)))
	(scroller (or scroller (scroll-frame-vertical self))))
    (when (and body scroller)
      (multiple-value-bind (value minimum maximum indicator-size)
	  (scroll-vertical-initialize body)
	(valuator-calibrate scroller
			    :value value
			    :minimum minimum
			    :maximum maximum
			    :indicator-size indicator-size)))))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  "Generic" functions that the user will need to redefine for 
;;;  whatever contact is to be scrollable
;;;
;;;  SCROLL-HORIZONTAL-INITIALIZE
;;;  SCROLL-HORIZONTAL-POSITION
;;;  SCROLL-VERTICAL-INITIALIZE
;;;  SCROLL-VERTICAL-POSITION

(defmethod scroll-horizontal-initialize ((self contact))
  ;;return four values: the initial position, the min and max
  ;;values to be represented by the scroll bar, and how fine each
  ;;movement of the indicator should be
  (values 0 0 (contact-width self) 1)
  )

(defmethod scroll-vertical-initialize ((self contact))
  ;;return four values: the initial position, the min and max
  ;;values to be represented by the scroll bar, and how fine each
  ;;movement of the indicator should be
  (values 0 0 (contact-height self) 1)
  )

(defmethod scroll-horizontal-position (value (self contact))
  (change-geometry self :x value :accept-p t))

(defmethod scroll-vertical-position (value (self contact))
  (change-geometry self :y value :accept-p t))
