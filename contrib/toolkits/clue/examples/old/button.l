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


(export '( label
	  justify
	  inside-border-width
	  button
	  notify
	  slide-right
	  ))

(defcontact label (contact)
  ((title :initform nil :type stringable) ;; Defaults to name string
   (style :initform :normal :type (member :normal :box :reverse :box-reverse))
   (justify :initform :center :type (member :left :center :right)
	    :accessor button-justify)
   (font :type font)
   (foreground :type pixel)
   (border-width :type card16 :initform 0)
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
  (* label foreground) 1   ;white
  (* label background) 0   ;black
  (* label border) 1       ;white
  )

(defmethod initialize-instance :after ((self label) &rest init-plist)
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

(defmethod display ((self label) &optional x y width height &key)
  (declare (ignore x y width height))
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
	  (draw-rectangle self gc 0 0 contact-width contact-height :fill))
	(using-gcontext (gc :drawable (contact-root self)
			    :font label-font
			    :foreground fore :background back
			    :line-width inside-border)
	  (when inside-border
	    (let ((half (floor inside-border 2)))
	      (draw-rectangle self gc half half
			      (- contact-width inside-border)
			      (- contact-height inside-border))))
	  (draw-glyphs self gc x y string)
	  )))))


;;;-----------------------------------------------------------------------------
;;; BUTTON

(defcontact button (label)
  ((command-key :initform nil :type (or null character list))
   (selected :initform nil :type boolean :accessor selected)
   (highlighted :initform nil :type boolean :accessor highlighted)
   )
  (:resources
    (event-mask :initform '(:exposure :owner-grab-button))
    (select :initform nil :type (or null symbol function list))
    (doit   :initform nil :type (or null symbol function list))
    command-key)
  )

(defmethod initialize-instance :after ((self button) &key select doit &allow-other-keys)
  (with-slots (callbacks command-key) self
    (when (and select (not (assoc :select callbacks)))
      (push (if (functionp select)
		(list ':select (list select))
	      (cons ':select select))
	    callbacks))
    (when (and doit (not (assoc :doit callbacks)))
      (push (if (functionp doit)
		(list ':doit (list doit))
	      (cons ':doit doit))
	    callbacks))
    (when command-key
      (if (atom command-key)
	  (add-event self `(:key-press ,command-key) '(display :select :toggle))
	(add-event self `(:key-press ,@command-key) '(display :select :toggle))))))

(define-resources
  (* button foreground) 1  ;white
  (* button background) 0  ;black
  (* button border) 1      ;white
  )

(defevent button :button-press (action-display :select :toggle))
(defevent button :button-release notify (action-display :select nil :highlight nil))
(defevent button :enter-notify (action-display :highlight t))
(defevent button :leave-notify slide-right (action-display :select nil :highlight nil))

(defmethod action-display ((button button) &key (select :unspecified) (highlight :unspecified)) 
  (declare (ignore x y width height))
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

(defmethod notify ((button button) &optional (callback :select))
  (with-slots (selected) button
    (with-event (x y)
      (when (and selected
		 callback
		 (inside-contact-p button x y))
	(apply-callback button callback)))))

(defmethod slide-right ((button button) &optional (callback :cascade) )
  ;; Hook for cascading menus.  Apply CALLBACK when mouse is to the right of BUTTON.
  (let (t1 t2)
    (with-slots (width height selected) button
      (with-event (x y)
	(setq t1 (and selected
			    callback
			    (and (< 0 x)
				 (< 0 y height))))
	  ;; Select button with the results from the cascade callback
	(setq t2 (callback-p button callback))
	(if (and t1 t2)
	    (progn (delete-callback (contact-parent button) :leave)
		   (apply-callback button :select
				   (apply-callback button callback)))

	    (add-callback (contact-parent button) :leave #'cascade-exit (contact-parent button) ))))))






                                    





