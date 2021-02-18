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

;; Most of this stuff is a direct copy from label and button
;; with virtual tacked on the front of the names.

;;; Change History:
;;; ----------------------------------------------------------------------------
;;; ??/??/88   LGO   Created.


(in-package 'clue-examples :use '(lisp xlib clos cluei))


(export '( virtual-label
	  virtual-button
	  ))

;; When CLUE's CLOS gets multiple inheritance, replace all this with
;; (defcontact virtual-button (virtual button)
;;   ()
;;   )


(defcontact virtual-label (virtual)
  ((title :initform nil :type stringable) ;; Defaults to name string
   (style :initform :normal :type (member :normal :box :reverse :box-reverse))
   (justify :initform :center :type (member :left :center :right)
	    :accessor button-justify)
   (font :type font)
   (foreground :type pixel)
   (background :type pixel)
   (inside-border-width :initform 3 :type integer
			:accessor inside-border-width)
   )
  (:resources
    title
    justify
    (font :initform "fg-18")
    foreground
    background
    inside-border-width)
  (:documentation "One line string display in a single font with different styles and justification")
  )

(define-resources
  (* virtual-label foreground) 1   ;white
  (* virtual-label background) 0   ;black
  (* virtual-label border)     1   ;white
  (* virtual-label border-width) 1
  )

(defmethod initialize-instance :after ((self virtual-label) &rest init-plist)
  (declare (ignore init-plist))
  (with-slots (title font height width inside-border-width) self
    (when (symbolp title) ;; NIL is a symbol
      (setf title (string-capitalize (string (or title (contact-name self))))))
    (let ((label-font font))
      (setf height (+ (max-char-ascent label-font)
			   (max-char-descent label-font)
			   (* 2 inside-border-width)
			   2))
      (setf width (+ 2 (text-width label-font title))))))

(defmethod display ((self virtual-label) &optional x y width height &key)
  (declare (ignore x y width height))
  (let ((win self))
    (with-slots (font title justify inside-border-width style
			   (contact-height height) (contact-width width)
			   (label-foreground foreground) (label-background background)) self
      (let* ((label-font font)
	     (descent (max-char-descent label-font))
	     (string title)
	     (x 0)
	     (y (+ descent (floor contact-height 2))))
	(case justify
	  (:left nil)
	  (:center (setq x (floor (- contact-width (text-width label-font string)) 2)))
	  (:right (setq x (- contact-width (text-width label-font string)))))
	(let ((fore label-foreground)
	      (back label-background)
	      (inside-border nil))
	  (when (member style '(:reverse :box-reverse))
	    (rotatef fore back))
	  (when (member style '(:box :box-reverse))
	    (setq inside-border inside-border-width))
	  (using-gcontext (gc :drawable (contact-root self) :foreground back)
	    (rectangle win gc 0 0 contact-width contact-height :fill))
	  (using-gcontext (gc :drawable (contact-root self)
			      :font label-font
			      :foreground fore
			      :background back
			      :line-width inside-border)
	    (when inside-border
	      (let ((half (floor inside-border 2)))
		(rectangle win gc half half (- contact-width inside-border) (- contact-height inside-border))))
	    (glyphs win gc x y string)
	    ))))))


;;;-----------------------------------------------------------------------------
;;; VIRTUAL-BUTTON

(defcontact virtual-button (virtual-label)
  ((command-key :initform nil :type (or null character))
   (selected :initform nil :type boolean :accessor selected)
   (highlighted :initform nil :type boolean :accessor highlighted)
   (event-mask :initform '(:exposure :owner-grab-button))
   )
  (:resources
    (action  :initform nil :type (or null symbol function list))
    command-key)
  )

(defmethod initialize-instance :after ((self virtual-button) &key action &allow-other-keys)
  (with-slots (callbacks) self
    (when action
      (push (if (functionp action)
		(list ':select (list action))
	      (cons ':select action))
	    callbacks))))

(define-resources
  (* virtual-button foreground) 1    ;white
  (* virtual-button background) 0    ;black
  (* virtual-button border)     1    ;white
  (* virtual-button border-width) 1
  )

(defevent virtual-button :button-press (display :select :toggle))
(defevent virtual-button :button-release notify (display :select nil))
(defevent virtual-button :enter-notify (display :highlight t))
(defevent virtual-button :leave-notify (display :highlight nil))

(defmethod action-display ((button virtual-button)
                           &key (select :unspecified) (highlight :unspecified))
  (with-slots (style selected highlighted) button
    (case select				;Set SELECTED
      (:unspecified)
      (:toggle (setf selected (not selected)))
      (otherwise (setf selected select)))
    (unless (eq highlight :unspecified)		;Set HIGHLIGHTED
      (setf highlighted highlight))
    (let ((old-style style))
      (setf style				;Set STYLE
	    (if highlighted
		(if selected
		    :box-reverse
		  :box)
	      (if selected
		  :reverse
		:normal)))
      (unless (eq style old-style)		;Redisplay when changed
	(display button)))))

(defmethod notify ((button virtual-button) &optional (callback :select))
  (with-slots (selected) button
    (with-event (x y)
      (when (and selected
		 callback
		 (inside-contact-p button x y))
	(apply-callback button callback)))))

(defmethod slide-right ((button virtual-button) &optional (callback :cascade))
  (with-slots (width height selected) button
    (with-event (x y)
      (when (and selected
		 callback
		 (and (< 0 x)
		      (< 0 y height)))
	(apply-callback button callback)))))

;;-----------------------------------------------------------------------------

(defcontact virtual-menu (virtual-composite)
  ((ordering :type (or (member :first :last) function)
	     :initform :last
	     :accessor menu-insert-ordering)
   )
  (:resources function args)
  )

(define-resources
  (* virtual-menu height) 10
  (* virtual-menu width) 10
  )

(defun virtual-menu-choose (parent alist &rest options)
  "Display a menu on parent from alist.
 Alist entries are (stringable . options) where options are keyword-value pairs:
 :font font
 :justify (member :left :center :right)
 :select (or function list)"
  (apply #'menu-choose parent alist :menu-type 'virtual-menu :item-type 'virtual-button options))

(defmethod add-child ((self virtual-menu) contact &key)
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
	   (if (null (funcall ordering contact (car ordering)))
	       (push contact children)
	     (do ((c children (cdr c)))
		 ((null (cdr c))
		  (setf (cdr c) (list contact)))
	       ;; Insert contact when ordering "lessp" predicate returns nil
	       (unless (funcall ordering contact (cadr c))
		 (setf (cdr c) (list* contact (cdr c)))
		 (return nil))))))
	(setf manager-children children)))))

(defmethod manage-geometry ((parent virtual-menu) contact x1 y1 width1 height1 border-width1 &key)
  ;; The default geometry manager just does whatever it's asked
  ;; Returns the new x y width height border-width
  (declare (values success-p x y width height border-width))
;  (declare (type contact contact)
;	   (type (or null int16) x1 y1)
;	   (type (or null card16) width1 height1 border-width1)
;	   (values success-p x y width height border-width))
  (let* ((previous (previous-sibling contact));; Find the contact BEFORE this one
	 (x 0)
	 (y 0)
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
    (when (> width (contact-width parent))
      (change-geometry parent :width width :accept-p t)
      (setq width (contact-width parent))
      (dolist (child (composite-children parent))
	(when (managed-p child)
	  (resize child width (contact-height child) (contact-border-width child)))))
    (setq width (max width (contact-width parent)))
    (when (> (+ y height) (contact-height parent))
      (change-geometry parent :height (+ y height) :accept-p t))
    (setq success (and (or (null x1) (= x x1))
		       (or (null y1) (= y y1))
		       (or (null width1) (= width width1))
		       (or (null height1) (= height height1))
		       (or (null border-width1) (= border-width border-width1))))
;;    (PV success x y width height border-width)
    (values success x y width height border-width)))
