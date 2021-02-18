;;; -*- Mode:Lisp; Package:CLUE-EXAMPLES; Lowercase:T; Base:10; Syntax:Common-Lisp -*-
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
;;; Description:  Valuator contacts
;;;
;;; Change History: 
;;; ----------------------------------------------------------------------------
;;; 10/27/87   KK    Created
;;;  6/09/88   KK    Updated to TICLOS v7, CLUE v13
;;;  6/14/88   KK    Removed scale, use floating pt value range
;;;  6/21/88   KK    Added precision slot
;;;  7/15/88   SLM   Add call to '(the VALUATOR self)' in VALUATOR-SET-BY-USER
;;;  8/26/88   SLM   Changed doc strings to be useful for the mouse-documentation window


;;;----------------------------------------------------------------------------------+
;;;                                                                                  |
;;;                                     valuator                                     |
;;;                                                                                  |
;;;----------------------------------------------------------------------------------+

(in-package 'clue-examples :use '(lisp xlib clos cluei))



(defcontact valuator (contact)
  ((fg-color        :type      pixel
		    :initarg   :fg-color
		    :accessor  fg-color
		    :initform  0
                    )
   (bg-color        :type      pixel
		    :initarg   :bg-color
		    :accessor  bg-color
		    :initform  1
                    )
   (minimum         :type      number
		    :initarg   :minimum
		    :accessor  valuator-minimum
		    :initform  0)
   (maximum         :type      number
		    :initarg   :maximum
		    :accessor  valuator-maximum
		    :initform  0)
   (value           :type      number
		    :initarg   :value
		    :accessor  valuator-value
		    :initform  0)
   (indicator-size  :type      number
		    :initarg   :indicator-size
		    :accessor  valuator-indicator-size
		    :initform  1)
   (increment       :type      number
		    :initarg   :increment
		    :accessor  valuator-increment
		    :initform  1)
   (precision       :type      (or null number)
		    :initarg   :precision
		    :accessor  valuator-precision
		    :initform  1)
   (documentation   :initform   "L: more  M: move to here  R: less")) 
  (:resources       increment
		    fg-color
		    bg-color
		    documentation
		    (event-mask :initform #.(make-event-mask :pointer-motion-hint)))
  (:documentation   "A basic valuator which controls a numeric value."))


(proclaim '(inline precision-round))
(defun precision-round (value precision)
  (if precision
      (* precision (round value precision))
      value))



;;;  Setting values

(defmethod initialize-instance :after ((self valuator) &key &allow-other-keys)  
  ;; Ensure that indicator is initialized
  (valuator-change-indicator self (valuator-indicator-size self)))

(defmethod (setf valuator-minimum) (min (self valuator))
  (valuator-calibrate self :minimum min))

(defmethod (setf valuator-maximum) (max (self valuator))
  (valuator-calibrate self :maximum max))

(defmethod (setf valuator-indicator-size) (new-size (self valuator))
  (valuator-calibrate self :indicator-size new-size))

(defmethod (setf valuator-value) (new-value (self valuator))
  (with-slots (maximum minimum) self
    (unless (and (<= new-value maximum) (>= new-value minimum))
	(error "Invalid value: ~a is not in the range (~a ~a)." new-value minimum maximum)))
  (valuator-calibrate self :value new-value))
    
(defmethod valuator-calibrate ((self valuator) &key minimum maximum value indicator-size)
  (with-slots ((slot-minimum minimum)
               (slot-maximum maximum)
               (slot-value value)
               (slot-indicator-size indicator-size))
              self
    ;; Set default argument values
    (setf minimum        (or minimum slot-minimum)
	  maximum        (or maximum slot-maximum)
	  indicator-size (or indicator-size slot-indicator-size)
	  value          (max (min (or value slot-value) maximum) minimum))
	  
    (let* ((range-changed     (or (/= minimum slot-minimum)
				  (/= maximum slot-maximum)
				  (/= indicator-size slot-indicator-size)))
	   (value-changed     (/= value slot-value)))
      
      ;; Erase previous indicator
      (when (realized-p self) (valuator-erase-indicator self))
      
      (when  range-changed
	;; Check for valid range
	(assert (<= minimum maximum)
		nil "Minimum (~a) exceeds maximum (~a)."
		minimum  maximum)	
	
	;; Adjust internal conversions for new range and indicator size
	(valuator-adjust-size self minimum maximum indicator-size)
	(valuator-change-indicator self indicator-size))

      ;; Update instance variables
      (setf slot-minimum        minimum
	    slot-maximum        maximum
	    slot-value          value
	    slot-indicator-size indicator-size)
      
      ;; Reposition indicator
      (valuator-move-indicator self value)

      ;; Redisplay, if necessary
      (when (realized-p self) (display self))

      ;; Report program-requested value change 
      (if value-changed (apply-callback self :changed-by-program value)))))


(defmethod valuator-adjust-size ((self valuator) new-minimum new-maximum  new-indicator-size)
  "Adjust the valuator size to new value range. The primary method for valuators is undefined."
  (declare (ignore  new-minimum new-maximum  new-indicator-size)))



;;; Indicator control

(defmethod valuator-change-indicator ((self valuator) new-indicator-size)
  "Change the size of the valuator indicator.  NEW-INDICATOR-SIZE is given in the
same units measure by the valuator."
  (declare (ignore   new-indicator-size)))

(defmethod valuator-move-indicator ((self valuator) new-value)
  "Change the indicator position to reflect the NEW-VALUE."
  (declare (ignore new-value)))

(defmethod valuator-indicator-changed-p ((self valuator) new-value)
  "Return true if setting NEW-VALUE will cause indicator display to change.
This predicate is used to avoid unnecessary display updates."
  (declare (ignore new-value))
  t)

(defmethod valuator-display-indicator ((self valuator)))
(defmethod valuator-erase-indicator ((self valuator)))

;;; Actions


(defmethod more ((self valuator))
  "Initiate an operation to increment the valuator value.
The primary valuator method does nothing. See FINISH-MORE.")

(defmethod finish-more ((self valuator))
  "Conclude an operation to increment the valuator value."
  (with-slots (value maximum increment) self
      (when (< value maximum)
        (valuator-set-by-user self (min (+ value increment) maximum)))))

(defmethod less ((self valuator))
  "Initiate an operation to decrement the valuator value.
The primary valuator method does nothing. See FINISH-LESS.")

(defmethod finish-less ((self valuator))
  "Conclude an operation to decrement the valuator value."
  (with-slots (value minimum increment) self
    (when (> value minimum)
      (valuator-set-by-user self (max (- value increment) minimum)))))

(defmethod here ((self valuator))
  "Initiate an operation to directly position the valuator value.
The primary valuator method does nothing. See FINISH-HERE.")

(defmethod finish-here ((self valuator))
  "Conclude an operation to directly position the valuator value.
The primary valuator method does nothing.")



;;; Inquiry

(defmethod valuator-per-cent ((self valuator))
  "Return the current value as a percentage of the total range (i.e. (- maximum minimum))."
  (with-slots (value minimum maximum) self
    (let ((range (- maximum minimum)))
      (if (plusp range)
	  (float (/ (- value minimum) range))
	  1.0))))

;;; Internal
(defun valuator-set-by-user (self new-value)
  ;; Echo new value
  (when (valuator-indicator-changed-p self new-value)
    (valuator-erase-indicator self)
    (valuator-move-indicator self new-value)
    (valuator-display-indicator self))
  
  (with-slots (value) (the valuator self)  ;;add '(THE valuator...) so kludged CLOS can macro expand correctly
    ;; Change value
    (setf value new-value)
    
    ;;Report new value
    (apply-callback self :changed-by-user value)))


;;; Class bindings

(defevent valuator (:button-release :button-1) finish-more)
(defevent valuator (:button-press   :button-1) more)
(defevent valuator (:button-release :button-3) finish-less)
(defevent valuator (:button-press   :button-3) less)
(defevent valuator (:button-release :button-2) finish-here)
(defevent valuator (:button-press   :button-2) move-here here)
(defevent valuator (:motion-notify  :button-2) move-here)


;;;----------------------------------------------------------------------------------+
;;;                                                                                  |
;;;                                     scroller                                     |
;;;                                                                                  |
;;;----------------------------------------------------------------------------------+

(defvar *scroller-initial-delay*
	0.35 "Seconds to delay before beginning continuous scrolling.")
(defvar *scroller-delay*
	0.10 "Seconds to delay before changing value during continuous scrolling.")

(defcontact scroller (valuator)
  (;; Internal
   (pixels-per-unit  :type number  :initform 0)
   (thumb-position   :type integer :initform 0)
   (thumb-size       :type integer :initform 0)  
   (scroll-state			   :initform :idle
     		     :type (member :idle :more :here :less :continuous-more :continuous-less))				   
   (documentation :initform '(("L: more  M: move to here  R: less")
			     ("Click-hold does the appropriate action continually"))))
  (:resources documentation
              fg-color
              bg-color
              fill-style)

  (:documentation    "A scroll bar valuator."))

(define-resources
   (* scroller  * fg-color) 0
   (* scroller  * bg-color) 1
   (* scroller * fill-style) :opaque-stippled)

;;; Indicator control

(defmethod valuator-change-indicator ((self scroller) new-indicator-size)
  "Change the size of the scroller thumb.  NEW-INDICATOR-SIZE is given in the
same units measure by the scroller."
  (with-slots (thumb-size pixels-per-unit) self
    (setf thumb-size (round (* new-indicator-size pixels-per-unit)))))

(defmethod valuator-move-indicator ((self scroller) new-value)
  "Change the thumb position to reflect the NEW-VALUE."  
  (with-slots (thumb-position pixels-per-unit minimum) self
    (setf thumb-position (round (* pixels-per-unit (- new-value minimum))))))

(defmethod valuator-indicator-changed-p ((self scroller) new-value)
  "Return true if setting NEW-VALUE will cause indicator display to change.
This predicate is used to avoid unnecessary display updates."
  (with-slots (thumb-position pixels-per-unit minimum) self
    (/= thumb-position (round (* pixels-per-unit (- new-value minimum))))))

;;; Display

(defmethod display ((self scroller) &optional x y width height &key)
  (declare (ignore x y width height))
  ;; Display thumb against background  
  (valuator-display-indicator self))


;;; Actions

(defmethod more ((self scroller)) 
  "Initiate an operation to increment the scroller value.
This method implements the continuous scrolling feature."  
  (with-slots (scroll-state value maximum increment) self
    (when (eq scroll-state :idle)
      (setf scroll-state :more)

      ;; Continuous scrolling loop
      (do ((display (drawable-display self))
	    (delay   *scroller-initial-delay*))
	  ((and (process-next-event display delay)
		(eq scroll-state :idle)))

	;; Ensure continuous controlling scroll-state
	(setf delay *scroller-delay*
	      scroll-state :continuous-more)

	(when (< value maximum)
	  (valuator-set-by-user self (min (+ value increment) maximum)))))))

(defmethod finish-more ((self scroller))
  "Conclude an operation to increment the scroller value."
  (with-event (y)
    (with-slots (scroll-state value maximum minimum pixels-per-unit
                       increment indicator-size precision) self
      (case scroll-state
        (:more
         ;; Increment relative to event position
	 (when (< value maximum)
           (valuator-set-by-user
	     self (min maximum
		       (+ value
			  (max increment
			       (precision-round
				 (* indicator-size (/ y (- maximum minimum) pixels-per-unit))
				 precision))))))
         
         ;; Return to idle scroll-state
         (setf scroll-state :idle))
        
        (:continuous-more
         ;; Simply return to idle state
         (setf scroll-state :idle))))))

(defmethod less ((self scroller))
  "Initiate an operation to decrement the scroller value.
This method implements the continuous scrolling feature."  
  (with-slots (scroll-state value minimum increment) self
    (when (eq scroll-state :idle)
      (setf scroll-state :less)

      ;; Continuous scrolling loop
      (do ((display (drawable-display self))
	    (delay   *scroller-initial-delay*))
	  ((and (process-next-event display delay)
		(eq scroll-state :idle)))

	;; Ensure continuous controlling state
	(setf delay *scroller-delay*
	      scroll-state :continuous-less)

	(when (> value minimum)
	  (valuator-set-by-user self (max (- value increment) minimum)))))))

(defmethod finish-less ((self scroller))
  "Conclude an operation to decrement the scroller value."
  (with-event (y)
    (with-slots (scroll-state value maximum minimum pixels-per-unit
                       increment indicator-size precision) self
      (case scroll-state
        (:less
         ;; Decrement relative to event position.
         (when (> value minimum)
           (valuator-set-by-user
	     self (max minimum
		       (- value
			  (max increment
			       (precision-round
				 (* indicator-size (/ y (- maximum minimum) pixels-per-unit))
				 precision))))))
         
         ;; Return to idle scroll-state
         (setf scroll-state :idle))
        
        (:continuous-less
         ;; Simply return to idle state
         (setf scroll-state :idle))))))


(defmethod here ((self scroller))
  "Initiate an operation to directly position the valuator value.
This method implements the drag scrolling feature."  
  (with-slots (scroll-state) self
    (when (eq scroll-state :idle)
      (setf scroll-state :here)

      ;; Drag scrolling loop
      (do ((display (drawable-display self))
	    (delay  *scroller-initial-delay*))
	  ((and (process-next-event display delay)
		(eq scroll-state :idle)))

	;; Ensure continuous controlling state
	(setf delay nil
	      scroll-state :continuous-here)))))

(defmethod finish-here ((self scroller))
  "Conclude an operation to directly position the valuator value."
  (with-slots (scroll-state) self
    (if (eq scroll-state :here)
	;; Position according to finish event
	(move-here self))
    
    ;; Return to idle state
    (setf scroll-state :idle)))



;;;----------------------------------------------------------------------------------+
;;;                                                                                  |
;;;                                     vscroller                                    |
;;;                                                                                  |
;;;----------------------------------------------------------------------------------+

(defcontact vscroller (scroller)
  ()
  (:documentation  "A vertically-oriented scroll bar valuator. "))


;;; Display

(defmethod valuator-display-indicator ((self vscroller))
  (with-slots (fg-color bg-color thumb-position width height thumb-size) self
    (using-gcontext (gcontext :drawable   self
			      :foreground fg-color
			      :background bg-color
			      :fill-style :stippled 66%gray)
      (draw-rectangle self gcontext
		      1 thumb-position
		      (max 1 (- width 2)) thumb-size
		      :fill-p)
      (draw-rectangle self gcontext
                      0 0 (- width 1) (- height 1)))))

(defmethod valuator-erase-indicator ((self vscroller))
  (with-slots (thumb-position width thumb-size) self
    (clear-area self
		:y      thumb-position
		:width  width
		:height thumb-size)))

;;; Size control

(defmethod valuator-adjust-size ((self vscroller) new-minimum new-maximum new-indicator-size)
  "Adjust internal data for new range and indicator size"
  (with-slots (height pixels-per-unit) self
    (let ((range (- new-maximum new-minimum)))
      (setf pixels-per-unit (/ height (if (plusp range) range new-indicator-size))))))


;;; Actions

(defmethod move-here ((self vscroller))
  "Change value according to position given by the EVENT."

  ;; Use event to trigger a request for the current pointer position
  (multiple-value-bind (x y) (query-pointer self)
    (declare (ignore x))
    (with-slots (value minimum maximum event-y pixels-per-unit precision) self
      (let ((new-value (max (min (precision-round
				   (+ minimum (/ y pixels-per-unit))
				   precision)
                                 maximum)
                            minimum)))      
        (when (/= new-value value)
          (valuator-set-by-user self new-value))))))

 



;;;----------------------------------------------------------------------------------+
;;;                                                                                  |
;;;                                     hscroller                                    |
;;;                                                                                  |
;;;----------------------------------------------------------------------------------+

(defcontact hscroller (scroller)
  ()
  (:documentation    "A horizontally-oriented scroll bar valuator."))


;;; Display

(defmethod valuator-display-indicator ((self hscroller))
  (with-slots (fg-color bg-color thumb-position thumb-size height width) self
    (using-gcontext (gcontext :drawable   self
			      :foreground fg-color
			      :background bg-color
			      :fill-style :solid)
      (draw-rectangle self gcontext
		      thumb-position 1
		      thumb-size (max 1 (- height 2))
		      :fill)
      (draw-rectangle self gcontext
                      0 0 (- width 1) (- height 1)))))

(defmethod valuator-erase-indicator ((self hscroller))
  (with-slots (thumb-position thumb-size height) self
    (clear-area self
		:x      thumb-position
		:width  thumb-size
		:height height)))

;;; Size control

(defmethod valuator-adjust-size ((self hscroller) new-minimum new-maximum new-indicator-size)
  "Adjust internal data for new range and indicator size"
  (with-slots (width pixels-per-unit) self
    (let ((range (- new-maximum new-minimum)))
      (setf pixels-per-unit (/ width (if (plusp range) range new-indicator-size))))))


;;; Actions
 
(defmethod move-here ((self hscroller))
  "Change value according to position given by the EVENT."

  ;; Use event to trigger a request for the current pointer position
  (multiple-value-bind (x y) (query-pointer self)
    (declare (ignore y))
    (with-slots (value minimum maximum event-y pixels-per-unit precision) self
      (let ((new-value (max (min (precision-round
				   (+ minimum (/ x pixels-per-unit))
				   precision)
                                 maximum)
                            minimum)))      
        (when (/= new-value value)
          (valuator-set-by-user self new-value))))))


;;;----------------------------------------------------------------------------------+
;;;                                                                                  |
;;;                                     graduated-valuator                           |
;;;                                                                                  |
;;;----------------------------------------------------------------------------------+




;;;----------------------------------------------------------------------------------+
;;;                                                                                  |
;;;                                     slider                                       |
;;;                                                                                  |
;;;----------------------------------------------------------------------------------+

(defcontact slider (scroller)
  (;; Internal
   (thumb-pixmap     :type (or null pixmap) :initform nil))
  (:documentation    "A slider valuator."))

;;; Indicator control

(defmethod valuator-change-indicator ((self slider) new-indicator-size)
  "Change the size of the slider thumb.  NEW-INDICATOR-SIZE is given in the
same units measure by the slider."
  (with-slots (thumb-pixmap thumb-size pixels-per-unit) self
    ;; Discard old image --- recreate upon display
    (when thumb-pixmap
      (free-pixmap thumb-pixmap)
      (setf thumb-pixmap nil))
    (setf thumb-size (round (* new-indicator-size pixels-per-unit)))))



;;;----------------------------------------------------------------------------------+
;;;                                                                                  |
;;;                                     vslider                                      |
;;;                                                                                  |
;;;----------------------------------------------------------------------------------+

(defcontact vslider (slider)
  ()
  (:documentation    "A vertically-oriented slider valuator."))

;;; Indicator control

(defmethod valuator-erase-indicator ((self vslider))
  (with-slots (thumb-position width thumb-size) self
    (clear-area self
		:y      thumb-position
		:width  width
		:height thumb-size)))


;;; Display (fix this!)

;(DEFMETHOD valuator-display-indicator ((self vslider))
;  (WITH-SLOTS (thumb-pixmap width thumb-size depth contact-background fg-color bg-color thumb-position) self
;    (USING-GCONTEXT (gcontext :foreground fg-color
;			      :background bg-color
;			      :fill-style :solid)
;      (UNLESS thumb-pixmap
;	;; Create slider thumb pixmap
;	(SETF thumb-pixmap (create-pixmap :drawable self
;					  :width    width
;					  :height   thumb-size
;					  :depth    depth))
;	(LET* ((offset       (ROUND (MIN width thumb-size) 8))
;	       (offset2      (ROUND offset 2))
;	       (face-left    0)
;	       (face-top     offset2)
;	       (face-right   (- width offset))
;	       (face-bottom  (- thumb-size offset2))
;	       (shadow-width (MIN 3 (ROUND offset 3))))	
	  
;	  ;; Draw background
;	  (TYPECASE contact-background
;	    (pixmap (WITH-GCONTEXT
;		      (gcontext :fill-style :tiled :tile contact-background)
;		      (DRAW-RECTANGLE thumb-pixmap gcontext 
;				      0 0
;				      width thumb-size :fill-p)))
;	    (t      (WITH-GCONTEXT
;		      (gcontext :fill-style :solid :foreground contact-background)
;		      (DRAW-RECTANGLE thumb-pixmap gcontext 
;				      0 0
;				      width thumb-size :fill-p))))
	  
;	  ;; Draw shadow
;	  (WITH-GCONTEXT
;	    (gcontext :fill-style :solid)
;	    (DRAW-RECTANGLE thumb-pixmap gcontext 
;			    shadow-width shadow-width
;			    (- width shadow-width) (- thumb-size shadow-width) :fill-p))
	  
;	  ;; Draw sides    
;	  (WITH-GCONTEXT			;top
;	    (gcontext :fill-style :opaque-stippled :stipple (cluei:get-pixmap (root self) 25%gray))
;	    (DRAW-LINES thumb-pixmap gcontext
;			(LIST face-right face-top
;			      width      0
;			      0          0
;			      face-left  face-top)
;			:fill-p t :shape :convex))
	  
;	  (WITH-GCONTEXT			;bottom
;	    (gcontext :fill-style :opaque-stippled :stipple (cluei:get-pixmap (root self) 66%gray))
;	    (DRAW-LINES thumb-pixmap gcontext
;			(LIST width      thumb-size
;			      0          thumb-size
;			      face-left  face-bottom
;			      face-right face-bottom)
;			:fill-p t :shape :convex)
;	    (DRAW-LINES thumb-pixmap gcontext	;right
;			(LIST width      thumb-size
;			      face-right face-bottom
;			      face-right face-top
;			      width 0)
;			:fill-p t :shape :convex))
	  
;	  ;; Draw top
;	  (WITH-GCONTEXT (gcontext :line-style :solid)
;	    (DRAW-RECTANGLE thumb-pixmap gcontext 0 0 width thumb-size))
;	  (WITH-GCONTEXT
;	    (gcontext :fill-style :opaque-stippled :stipple (cluei:get-pixmap (root self) 12%gray))
;	    (DRAW-RECTANGLE thumb-pixmap gcontext 
;			    face-left face-top
;			    (- face-right face-left) (- face-bottom face-top) :fill-p))
	  
;	  ;; Draw notches
;	  (LET* ((number-notches 1)
;		 (notch-height (ROUND (- face-bottom face-top) 15))
;		 (notch-y      (+ face-top
;				  (ROUND (- face-bottom face-top) (1+ number-notches))
;				  (ROUND notch-height 2)))
;		 (notch-y-nh   (- notch-y notch-height))
;		 (notch-y-nh2  (- notch-y (ROUND notch-height 2)))
;		 (notch-x      (ROUND (- face-right face-left) 15))
;		 (fl-nx        (+ face-left notch-x))
;		 (w-nx         (- width notch-x)))
;	    (DOTIMES (i number-notches)
;	      (TYPECASE contact-background
;		(pixmap (WITH-GCONTEXT
;			  (gcontext :fill-style :tiled :tile contact-background)
;			  (DRAW-LINES thumb-pixmap gcontext
;				      (LIST face-left notch-y
;					    fl-nx     notch-y-nh2
;					    face-left notch-y-nh)
;				      :fill-p t :shape :convex)))
;		(t      (WITH-GCONTEXT
;			  (gcontext :fill-style :solid :foreground contact-background)
;			  (DRAW-LINES thumb-pixmap gcontext
;				      (LIST face-left notch-y
;					    fl-nx     notch-y-nh2
;					    face-left notch-y-nh)
;				      :fill-p t :shape :convex))))	    
;	      (WITH-GCONTEXT
;		(gcontext :fill-style :opaque-stippled :stipple (cluei:get-pixmap (root self) 88%gray))
;		(DRAW-LINES thumb-pixmap gcontext
;			    (LIST face-left notch-y-nh
;				  w-nx      notch-y-nh		      
;				  width     notch-y-nh2
;				  fl-nx     notch-y-nh2)
;			    :fill-p t :shape :convex))	    	    
;	      (WITH-GCONTEXT
;		(gcontext :fill-style :opaque-stippled :stipple (cluei:get-pixmap (root self) 33%gray))
;		(DRAW-LINES thumb-pixmap gcontext
;			    (LIST w-nx      notch-y
;				  face-left notch-y		      
;				  fl-nx     notch-y-nh2
;				  width     notch-y-nh2)
;			    :fill-p t :shape :convex))	    	    
;	      (WITH-GCONTEXT
;		(gcontext :line-style :solid)
;		(DRAW-LINES thumb-pixmap gcontext
;			    (LIST face-left notch-y
;				  fl-nx     notch-y-nh2
;				  face-left notch-y-nh
;				  w-nx      notch-y-nh
;				  width     notch-y-nh2
;				  w-nx      notch-y
;				  face-left notch-y)))	    
;	      (INCF notch-y (ROUND (- face-bottom face-top) 4))
;	      )))))
;    ;; Display thumb-pixmap
;    (WITH-GCONTEXT
;      (gcontext :fill-style :tiled :tile thumb-pixmap)
;      (DRAW-RECTANGLE self gcontext
;	0      thumb-position
;	width  thumb-size
;	:fill-p))))

;;; Size control

(defmethod valuator-adjust-size ((self vslider) new-minimum new-maximum  new-indicator-size)
  "Adjust internal data for new range and indicator size."
  (with-slots (height pixels-per-unit) self
    (setf pixels-per-unit (/ height (+ new-maximum (- new-minimum)  new-indicator-size)))))





;;; Actions

(defmethod move-here ((self vslider))
  "Change value according to position given by the EVENT."
  (with-event (y)
    (with-slots (value minimum maximum event-y pixels-per-unit indicator-size precision) self
      (let ((new-value (max (min (precision-round
				   (+ minimum (/ y pixels-per-unit) (round indicator-size -2))
				   precision)
                                 maximum)
                            minimum)))      
        (when (/= new-value value)
          (valuator-set-by-user self new-value))))))
