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
;;;  8/26//88	SLM	Created.



(in-package 'clue-examples :use '(lisp xlib clos cluei))



(defcontact scrollable-pixmap-window (contact)
  ((pixmap :type (or null pixmap) :accessor pixmap :initform nil :initarg :pixmap)
   (pixmap-width :type integer :accessor pixmap-width :initform 0 :initarg :pixmap-width)
   (pixmap-height :type integer :accessor pixmap-height :initform 0 :initarg :pixmap-height)
   (foreground :type (or pixel pixmap) :accessor foreground :initarg :foreground :initform 0)
   (scroll-increment :type integer :reader scroll-increment :initarg :scroll-increment :initform 1)
   (scrolled-to-x :type integer :accessor scrolled-to-x :initform 0)
   (scrolled-to-y :type integer :accessor scrolled-to-y :initform 0))
  (:resources
    (compress-exposures :initform :on)
    foreground)
  )

(defmethod display ((self scrollable-pixmap-window) &optional x y width height &key)
  (with-slots ((contact-x x) (contact-y y)
	       pixmap-width pixmap-height
	       foreground background pixmap
	       scrolled-to-x scrolled-to-y) self
    (let ((startx (or x scrolled-to-x))
	  (starty (or y scrolled-to-y))
	  (draw-w (or width pixmap-width))
	  (draw-h (or height pixmap-height)))
      (using-gcontext (gc :drawable self
			  :foreground foreground
			  :background background)
	;;(clear-area self)
	(copy-area pixmap gc startx starty draw-w draw-h self contact-x contact-y)))
    ))
     

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  "Generic" functions that the user will need to redefine for 
;;;  whatever contact is to be scrollable
;;;
;;;  SCROLL-HORIZONTAL-INITIALIZE
;;;  SCROLL-HORIZONTAL-POSITION
;;;  SCROLL-VERTICAL-INITIALIZE
;;;  SCROLL-VERTICAL-POSITION

(defmethod scroll-horizontal-initialize ((self scrollable-pixmap-window))
  ;;return four values: the initial position, the min and max
  ;;values to be represented by the scroll bar, and how fine each
  ;;movement of the indicator should be
  (with-slots (scroll-increment pixmap-width) self
    (values 0 0 (* pixmap-width scroll-increment) scroll-increment)
    ))

(defmethod scroll-vertical-initialize ((self scrollable-pixmap-window))
  ;;return four values: the initial position, the min and max
  ;;values to be represented by the scroll bar, and how fine each
  ;;movement of the indicator should be
  (with-slots (pixmap scroll-increment pixmap-height) self
    (values 0 0 (* pixmap-height scroll-increment) scroll-increment))
    )

(defmethod scroll-horizontal-position (value (pixmap-window scrollable-pixmap-window))
  (with-slots (scroll-increment scrolled-to-x) (the scrollable-pixmap-window pixmap-window)
    (setf scrolled-to-x (round (/ value scroll-increment))))
  (display pixmap-window)
  )

(defmethod scroll-vertical-position (value (pixmap-window scrollable-pixmap-window))
  (with-slots (scroll-increment scrolled-to-y scrolled-to-x) (the scrollable-pixmap-window pixmap-window)
    (setf scrolled-to-y (round (/ value scroll-increment))) 
  (display pixmap-window scrolled-to-x scrolled-to-y))
  )

;; Cache the last image
(defvar *bitmap-cache* nil) ;; list of (pathname image)

(defun get-bitmap (thing)
  (etypecase thing
    (image thing)
    ((or pathname string)
     (let ((pathname (parse-namestring thing)))
       (if (equal pathname (first *bitmap-cache*))
	   (second *bitmap-cache*)
	 (let ((image (read-bitmap-file pathname)))
	   (setq *bitmap-cache* (list pathname image))
	   image))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  Main routine to use all the above stuff...

(defun scroll-a-bitmap (display bitmap &optional
			&key (foreground "black") (background "white")
			(horizontal :bottom) (vertical :right))
  "Display BITMAP in a scrollable window.  
BITMAP is either a bitmap, or the pathname of a bitmap file.
HORIZONTAL can be either :TOP or :BOTTOM
VERTICAL can be either :LEFT or :RIGHT
FOREGROUND and BACKGROUND can be any string identifying a color that the server knows about."

  (setf (display-image-lsb-first-p display) nil)
  (setq bitmap (get-bitmap bitmap))
      
  (let* ((pmap nil)
	 
	 (pwidth (xlib:image-width bitmap))
	 (pheight (xlib:image-height bitmap))
	 (scroll (make-contact 'scroll-frame :parent display
			       :name 'bitmap-scroller
			       :horizontal horizontal :vertical vertical
			       :x 20 :y 20 :inside-width pwidth :inside-height pheight
			       :foreground 0 :background 1 
			       :compress-exposures :on))
	 (window (make-contact 'scrollable-pixmap-window :parent scroll
			       :x 0 :y 0
			       :width pwidth :height pheight 
			       :foreground (convert scroll foreground 'pixel)
			       :background (convert scroll background 'pixel) 
			       :scroll-increment 0.2
			       :pixmap-width pwidth
			       :pixmap-height pheight))
	 )
    (add-mode scroll :exclusive 'ignore-action)
    (setf (contact-state scroll) :mapped)
    (update-state display)
    (setf (slot-value (the scrollable-pixmap-window window) 'pixmap)
	  (setf pmap (create-pixmap :width pwidth :height pheight
				    :depth (contact-depth window) :drawable window))
	  )
    (using-gcontext (gc :drawable pmap
			:foreground (convert scroll foreground 'pixel)
			:background (convert scroll background 'pixel))
      (put-image pmap gc bitmap :x 0 :y 0 :bitmap-p t))
    (display window)
    
    ;(add-before-action display 'scroll-frame 'trace :motion-notify)
    ;(add-before-action display 'scrollable-pixmap-window 'TRACE :motion-notify)
    ;(SETF (DISPLAY-after-function display) #'xlib:display-finish-output)
    (unwind-protect
	(catch 'quit-scroll 
	  (do ()
	      (())
	    (process-next-event display)))
      (destroy window)
      (destroy scroll))))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  For those of us who just want a bitmap displayed, use this...

(defun display-a-pixmap (display bitmap)
  (setq bitmap (get-bitmap bitmap))
  (let* ((window (make-contact 'contact
			      :parent display
			      :width (xlib:image-width bitmap)
			      :height (xlib:image-height bitmap)
			      :x 0 :y 0 :background 1 :foreground 0
			      :state :mapped))
	 (pixmap nil)
	 (pwidth (xlib:image-width bitmap))
	 (pheight (xlib:image-height bitmap))
	 )
    (setf (contact-state window) :mapped)
    (update-state display)
    (setf pixmap (create-pixmap :width pwidth :height pheight :drawable window :depth (contact-depth window)))
    (using-gcontext (gc :drawable pixmap
			:foreground (convert window 0 'pixel)
			:background (convert window 1 'pixel))
      (put-image pixmap gc bitmap :x 0 :y 0 :width pwidth :height pheight :bitmap-p t))
    (using-gcontext (gc :drawable window
			:foreground (convert window 0 'pixel)
			:background (convert window 1 'pixel))
      (copy-area pixmap gc 0 0 pwidth pheight window 0 0))
    (setf (contact-state window) :mapped)
    (unwind-protect
	(do ()
	    (())
	  (process-next-event display))
      (destroy window))))

