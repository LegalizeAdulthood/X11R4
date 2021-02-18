;;; -*- Mode:Lisp; Package:CLUEI; Base:10; Lowercase:T; Syntax:Common-Lisp -*-

;;;
;;;			 TEXAS INSTRUMENTS INCORPORATED
;;;				  P.O. BOX 149149
;;;			       AUSTIN, TEXAS 78714-9149
;;;
;;; Copyright (C)1988-1990 Texas Instruments Incorporated.
;;;
;;; Permission is granted to any individual or institution to use, copy, modify,
;;; and distribute this software, provided that this complete copyright and
;;; permission notice is maintained, intact, in all copies and supporting
;;; documentation.
;;;
;;; Texas Instruments Incorporated provides this software "as is" without
;;; express or implied warranty.
;;;

;; To do:
;; 1. Need margins

;;; Change History:
;;; ----------------------------------------------------------------------------
;;;  11/09/87   LGO	Created.
;;;  08/17/88	LGO	Added Common-Windows rubout-handler support
;;;  08/22/88   SLM     Display the text cursor position.
;;;  08/23/88   SLM     Toggle solid and hollow text cursor when input focus 
;;;  02/24/89	DNG	When using Explorer CLOS, enable instances of 
;;;			interactive-stream to accept flavor messages from the 
;;;			system I/O functions.
;;;  02/28/89   KK      Updated for CLUE 1.16
;;;  05/06/89	DNG	For the Explorer, update to use the stream generic functions 
;;;			in the TICLOS package, and add support for the :READ-CURSORPOS and 
;;;			:INCREMENT-CURSORPOS messages.



;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;;  WARNING: Non-portable code! A portable implementation of an interactive-  |
;;;  stream will not be possible until a standard generic function protocol    |
;;;  for Common Lisp streams has been defined. This implementation works       |
;;;  for Explorers and other Lisp machines. It may serve as an example for     |
;;;  other implementations as well.                                            |
;;;                                                                            |
;;;----------------------------------------------------------------------------+



(in-package "CLUEI")

#+(and Explorer CLOS)
(import '(ticlos:stream-clear-input
	  ticlos:stream-unread-char
	  ticlos:stream-listen
	  ticlos:stream-read-char
	  ticlos:stream-clear-output
	  ticlos:stream-write-char
	  ticlos:stream-write-string
	  ticlos:stream-fresh-line
	  ))
(export '(interactive-stream
	  stream-clear-input
	  stream-unread-char
	  stream-listen
	  stream-peek-char
	  stream-read-char
	  stream-read-line
	  set-cursorpos
	  stream-clear-output
	  stream-move-cursor
	  stream-write-char
	  clear-line
	  clear-eol
	  stream-write-string
	  text-within-width
	  stream-fresh-line
	  draw-lozenged-string
	  simple-rubout-handler
	  with-input-editing
	  rubout-handler
	  get-rubout-handler-buffer
	  force-input
	  ))

(defcontact interactive-stream
	    (contact #+(and Explorer CLOS) TICLOS:FUNDAMENTAL-CHARACTER-OUTPUT-STREAM
		     #+(and Explorer CLOS) TICLOS:FUNDAMENTAL-CHARACTER-INPUT-STREAM)
  ((gcontext
     :type     (or null gcontext)
     :initform nil
     :reader   stream-gcontext)
   
   (font
     :type     font
     :reader   stream-font
     :initform 'fixed)
   
   (cursor-x
     :initform 0
     :type     integer
     :reader   stream-cursor-x)
   
   (cursor-y
     :initform 0
     :type     integer
     :reader   stream-cursor-y)
   
   (line-height
     :initform 0
     :type     integer
     :accessor stream-line-height) ; Pixels per character line.

   (tab-width
     :initform 0
     :type     integer
     :accessor stream-tab-width)   ; Number of pixels in a tab

   (lozenge-font
     :type     font
     :accessor stream-lozenge-font
     :initform 'micro)

   (unreadp
     :type     boolean
     :initform nil)		   ; True if a character was unread (already echoed)
   
   ;; more-height:
   ;; When non-nil, then every time a new line is output, this is decremented by
   ;; LINE-HEIGHT.  When this is less than zero, MORE-PROCESSING is called.
   ;; MORE-HEIGHT gets set to the value of (RESET-MORE-HEIGHT stream) in
   ;; STREAM-READ-CHAR, STREAM-CLEAR-OUTPUT and :after REALIZE.
   (more-height
     :initform t
     :type (or boolean card16)
     :accessor stream-more-height)
   
   (rubout-handler-function
     :initform 'simple-rubout-handler	
     :accessor stream-rubout-handler-function)
   
   (output-history-top
     :initform nil)		   ; Output history line at top of window

   (output-history
     :initform nil)		   ; Circular list of strings

   (output-history-size
     :initform 100))
  
  (:resources
    (background :initform :black)
    cursor-x
    cursor-y
    font
    gcontext
    line-height
    lozenge-font
    more-height
    output-history-size
    rubout-handler-function
    tab-width))

(define-resources
  (* interactive-stream width)  400
  (* interactive-stream height) 400)

(defmethod initialize-instance :after ((self interactive-stream) &rest options &aux (between-line-spacing 1))
  (declare (ignore options))
  (with-slots ( output-history output-history-top output-history-size
	       gcontext cursor-y font line-height tab-width) (the interactive-stream self)
    (when (zerop line-height)
      (setf line-height (+ (max-char-ascent font) (max-char-descent font) between-line-spacing)))
    (when (zerop cursor-y) (setf cursor-y (- line-height (max-char-descent font))))
    (when (zerop tab-width)
      (setf tab-width (* 8 (max-char-width font))))
    (setf output-history (make-list output-history-size))
    (setf (cdr (last output-history)) output-history) ;; Make circular
    (setf (car output-history) (make-array 256 :fill-pointer 0 :element-type 'string-char))
    (setf output-history-top output-history)
    ))

(defmethod realize :after ((self interactive-stream))
  ;; Ensure the gcontext is initialized
  (with-slots (gcontext font background) self
    (unless gcontext
      (setf gcontext (create-gcontext :drawable self :font font
				      :background background
				      :foreground (logxor background 1)))))
  (reset-more-height self))

(defevent interactive-stream :key-press stream-save-key)
(defmethod stream-save-key ((stream interactive-stream))
  (with-event (character display)
    (let ((char character))
      (when (characterp char)
	(append-characters display char)))
    t))

(defevent interactive-stream :focus-in (stream-display-cursor t))
(defevent interactive-stream :focus-out (stream-display-cursor nil))
(defmethod stream-display-cursor ((stream interactive-stream) fill-p)
  (with-slots (gcontext cursor-x cursor-y) stream
    (draw-cursor stream cursor-x cursor-y gcontext :erase-p t :fill-p (not fill-p))
    (draw-cursor stream cursor-x cursor-y gcontext :fill-p fill-p)))

(defun draw-cursor (window cursor-x cursor-y gcontext &optional &key (fill-p t) (erase-p nil))
  (let* ((font (gcontext-font gcontext))
	 (width (xlib:max-char-width font))
	 (height (+ (xlib:max-char-ascent font) (xlib:max-char-descent font)))
	 (rectangle-y (- cursor-y (xlib:max-char-ascent font))))
    (when erase-p
      (using-gcontext (gc :drawable window
			  :background (gcontext-foreground gcontext)
			  :foreground (gcontext-background gcontext))
	(draw-rectangle window gc cursor-x rectangle-y width height fill-p))
      (return-from draw-cursor))
    (draw-rectangle window gcontext cursor-x rectangle-y width height fill-p)))

(defmethod stream-clear-input ((self interactive-stream))
  "Clear all input that hasn't been read yet from the current io-buffer."
  (setf (slot-value (the interactive-stream self) 'unreadp) nil)
  (clear-characters (contact-display self))
  (loop  ;; Eat any characters sitting in the event buffer
    (unless (read-character (contact-display self) 0)
      (return nil))))

(defmethod stream-unread-char ((self interactive-stream) character)
  "Put CHARACTER back in the io-buffer so that it will be the next
character returned by ANY-TYI.  Note that CHARACTER must be exactly the
last character that was read, and that it is illegal to do two
unread-char's in a row."
  (setf (slot-value (the interactive-stream self) 'unreadp) t)
  (unread-character (contact-display self) character))

;; for common-windows compatibility
(defun force-input (contact string &key (begin 0) end)
  "Forces the characters from BEGIN to END in STRING into the input buffer for CONTACT."
  (append-characters (contact-display contact) string begin end))

(defmethod stream-listen ((self interactive-stream))
  "If a character is waiting in the current io-buffer, return it
leaving the character in the buffer.  If no character is available, return NIL."
  (listen-character (contact-display self)))

(defmethod stream-peek-char ((self interactive-stream) peek-type)
  (let (char)
    (loop
      (setq char (stream-read-char self))
      (cond ((null peek-type) (return nil))
	    ((eq peek-type t)
	     (unless (member char '(#\space #\tab #\newline)) (return nil)))
	    ((char= char peek-type) (return nil)))
      (stream-unread-char self char))
    char))

(defmethod stream-read-char ((self interactive-stream))
  "Read a character from the keyboard.
 Echoing is handled by the rubout handler (see rh-read-char)"
  (declare (special *rubout-handler*))
  (reset-more-height self)
  (let (char)
    (cond ((slot-value (the interactive-stream self) 'unreadp)
	   (setf (slot-value (the interactive-stream self) 'unreadp) nil)
	   (setq char (read-character (contact-display self))))
	  ((eq *rubout-handler* 'tyi)
	   (setq char (read-character (contact-display self))))
	  (*rubout-handler*
	   (setq char (rh-read-char self)))
	  (t (setq char (read-character (contact-display self)))
	     ;; Note: characters are not echoed without a rubout handler
	     ))
    #+ti
    (handle-asynchronous-characters char self)
    char))

(defmethod reset-more-height ((self interactive-stream))
  (with-slots (height more-height line-height) self
    (when more-height		   ;; Reset MORE height
      (setf more-height (- height (* 2 line-height))))))

(defmethod stream-read-line ((self interactive-stream) &rest make-array-options)
  (do ((line nil (cons (stream-read-char self) line)))
      ((eql (car line) #\newline)
       (let* ((i (1- (length line)))
	      (result (apply #'make-array i :element-type 'string-char
			     make-array-options)))
	 (dolist (c (cdr line))
	   (setf (aref result (decf i)) c))
	 result))))


(defun set-cursorpos (interactive-stream &key x y)
  (with-slots (cursor-x cursor-y gcontext)
	      (the interactive-stream interactive-stream)
    (draw-cursor interactive-stream cursor-x cursor-y gcontext :erase-p t)
    (when x
      (setf cursor-x x))
    (when y
      (setf cursor-y y))
    (draw-cursor interactive-stream cursor-x cursor-y gcontext :fill-p t))
  ;; needs to move a cursor character around
  )


;;;-----------------------------------------------------------------------------
;;; Output

(defvar *no-stream-history-p* nil) ;; Bound to T during display

(defmethod display ((self interactive-stream) &optional x y width height &key)
  (declare (ignore x y width height))
  (let ((win self))
    (clear-area win)
    (with-slots (cursor-x cursor-y line-height font gcontext
			  output-history output-history-top) self
      (setf cursor-x 0
	    cursor-y (- line-height (max-char-descent font)))
      ;;display the cursor
      (draw-cursor self cursor-x cursor-x gcontext :fill-p t)
      (do ((history output-history-top (cdr history))
	   (*no-stream-history-p* t))
	  ((eq history output-history))
	(draw-cursor self cursor-x cursor-y gcontext :erase-p t)
	(stream-write-string self (car history))
	(setf cursor-x 0
	      cursor-y (+ cursor-y line-height))
	(draw-cursor self cursor-x cursor-y gcontext)))
    ))

(defmethod stream-clear-output ((self interactive-stream))
  (with-slots ( output-history output-history-size output-history-top
	       font height cursor-x cursor-y more-height line-height gcontext) self
    (do ((i output-history-size (1- i))
	 (history output-history (cdr history)))
	((zerop i))
      (when (car history)
	(setf (fill-pointer (car history)) 0)))
    (setf cursor-x 0
	  cursor-y (- line-height (max-char-descent font)))
    (reset-more-height self)
    (setf output-history-top output-history)
    (clear-area self)
    (draw-cursor self cursor-x cursor-y gcontext))
  nil)

(defmethod stream-move-cursor ((self interactive-stream) &optional x y)
  (with-slots (cursor-x cursor-y gcontext font) self
    (let ((oldx cursor-x)
	  (oldy cursor-y))
      (when x (setf cursor-x x))
      (when y (setf cursor-x x))
      (let ((newx cursor-x)
	    (newy cursor-y))
	(draw-cursor self oldx oldy gcontext :erase-p t)
	(draw-cursor self newx newy gcontext :fill-p t)
	;; ********************* NEED TO ADD CURSOR OBJECT *****************
	newx newy oldx oldy nil
	))))

(defmethod stream-write-char ((self interactive-stream) character)
  (when (integerp character)		   ;; Kludge for old zetalisp code
    (setq character (int-char character)))
  (with-slots ( cursor-x cursor-y
	       (contact-width width) (contact-height height)
	       gcontext font more-height line-height
	       output-history  tab-width lozenge-font) self
    (draw-cursor self cursor-x cursor-y gcontext :erase-p t) 
    (if (graphic-char-p character)
	(progn
	  (when (> cursor-x contact-width) ;; Wrap on wide lines
	    (stream-write-char self #\newline))
	  (let ((width (char-width font (char-int character))))
	    (draw-glyph self gcontext cursor-x cursor-y (char-int character)
			:width width :size 8 :translate #'xlib::translate-default)
	    (incf cursor-x width)
	    (unless *no-stream-history-p*
	      (vector-push-extend character (car output-history)))))
      (progn ;; Undrawable character
	(case character
	  (#\newline (setf cursor-x 0
			   cursor-y (+ cursor-y line-height))
		     (unless *no-stream-history-p*
		       (pop output-history)
		       (unless (car output-history)
			 (setf (car output-history)
			       (make-array 256 :fill-pointer 0 :element-type 'string-char :adjustable t)))
		       (setf (fill-pointer (car output-history)) 0))
		     (when (> (+ cursor-y (max-char-descent font)) contact-height)
		       (when *no-stream-history-p*
			 (error "EOP during refresh")) ;; should never get here...
		       (end-of-page self))
		     (when (and more-height
				(minusp (decf more-height line-height)))
		       (more-processing self)))
	  (#\backspace (let ((width (char-width font (font-default-char font))))
		  (setf cursor-x (max 0 (- cursor-x width))))
		(unless *no-stream-history-p*
		  (vector-push-extend character (car output-history))))
	  (#\tab (setf cursor-x (+ cursor-x tab-width))
		 (unless *no-stream-history-p*
		   (vector-push-extend character (car output-history))))
	  (otherwise
	   (unless *no-stream-history-p*
	     (vector-push-extend character (car output-history)))
	   (incf cursor-x
		 (draw-lozenged-string self gcontext cursor-x cursor-y
				       (string (or (char-name character)
						   (format nil "~:@C" (char-int character))))
				       lozenge-font))))))
    (draw-cursor self cursor-x cursor-y gcontext))
  character)

(defmethod end-of-page ((interactive-stream interactive-stream))
  ;; Scroll up one line
  (with-slots (cursor-x cursor-y height width line-height output-history-top (gc gcontext))
	      (the interactive-stream interactive-stream)
    (let* ((bottom-line (min cursor-y height))
	   (clear-height (- height bottom-line)))
      (copy-area interactive-stream gc 0 line-height width bottom-line
		 interactive-stream 0 0)
      (when (plusp clear-height)
	(clear-area interactive-stream :x 0 :y bottom-line
		    :width width :height clear-height))
      (pop output-history-top))
    (decf cursor-y line-height)
    (draw-cursor interactive-stream cursor-x cursor-y gc)))

(defun more-processing (interactive-stream)
  (reset-more-height interactive-stream)
  (let ((*no-stream-history-p* t))
    (stream-write-string interactive-stream "*** MORE ***")
    (display-force-output (contact-display interactive-stream))
    (stream-read-char interactive-stream)
    (clear-line interactive-stream)))
  
(defun clear-line (interactive-stream)
  ;; Clear the current line
  (setf (slot-value (the interactive-stream interactive-stream) 'cursor-x) 0)
  (clear-eol interactive-stream))

(defun clear-eol (interactive-stream)
  ;; Clear the current line starting at the current cursor-x
  (with-slots (font cursor-x cursor-y line-height width line-height gcontext)
	      (the interactive-stream interactive-stream)
    (clear-area interactive-stream :x cursor-x :y (+ (font-descent font)
						     (- cursor-y line-height))
		:width width :height  line-height)
    (draw-cursor interactive-stream cursor-x cursor-y gcontext)))

(defmethod stream-write-string ((self interactive-stream) string &optional (start 0) end)
  (unless end (setq end (length string)))
  (do ((i start (+ i index 1))
       (index 0))
      ((>= i end) string)
    (declare (type integer i)
	     (type (or null integer) index))
    (with-slots ((contact-width width)
		 cursor-x cursor-y
		 font gcontext output-history) self
      (draw-cursor self cursor-x cursor-y gcontext :erase-p t)
      (when (> cursor-x contact-width) ;; Wrap on wide lines
	(stream-write-char self #\newline))
      (let ((line-width (- contact-width cursor-x))
	    (string-width 0)
	    (new-end end))
	(multiple-value-setq (string-width index)
	  (text-width font string :start i :end end))
	(when index (setq new-end index))
	(when (> string-width line-width) ;; Clip strings longer than remaing line width
	  (setq new-end (text-within-width line-width font string :start i :end new-end)
		string-width line-width))
	(setq index
	      (draw-glyphs self gcontext cursor-x cursor-y
			   string :start i :end new-end :width string-width))
	(incf cursor-x string-width))
      (draw-cursor self cursor-x cursor-y gcontext)
      ;; Save history
      (unless *no-stream-history-p*
	(let* ((history (car output-history))
	       (j (fill-pointer history))
	       (l (or index end))
	       (h (+ j (- l i))))
	  ;; Grow history if necessary
	  (when (> h (array-total-size history))
	    (setq history (adjust-array history (+ h 80)))
	    (setf (car output-history) history))
	  (setf (fill-pointer history) h)
	  (replace history string :start1 j :end1 h :start2 i :end2 l)))
      ;; do special characters not printed
      (if index
	  (stream-write-char self (aref string index))
	(return string)))))

(defun text-within-width (width font string &key (start 0) end translate)
  "Return an index within STRING such that the string width is less than WIDTH"
  ;; Estimate size
  (do* ((index (+ start (ceiling width (min-char-width font))) (1+ index))
	(stop (or end (length string))))
       ((>= index stop) index)
    (multiple-value-bind (w i)
	(text-width font string :start start :end index :translate translate)
      (when i (return i))
      (when (> w width) (return (1- index))))))

(defmethod stream-fresh-line ((self interactive-stream))
  (with-slots (output-history) self
    (unless (or (null (car output-history))
		(zerop (length (car output-history))))
      (stream-write-char self #\newline))))

;;;--------------------------------------------------------------------------------------------
;;; LOZENGED-STRINGS

(defun draw-lozenged-string (window gcontext x0 y0 string font)
  "Display string inside a lozenge at X0 Y0."
  (declare (values right-coordinate bottom-coordinate))
  (multiple-value-bind (width ascent descent)
      (text-extents font string)
    (let* (;; Put 2 pixels to the top and bottom of the string.
	   (lozenge-height (+ ascent 4))
	   (wid (+ lozenge-height width))
	   (xpos (+ x0 (ceiling lozenge-height 2)))
	   (ypos (+ y0 descent)))
      ;; Put the string then the box around it.
      (using-gcontext (gc :drawable (contact-root window) :default gcontext :font font)
	(draw-glyphs window gc xpos ypos string))
      (draw-lozenge window gcontext wid lozenge-height x0 (- y0 ascent 2))
      (values wid lozenge-height))))

(defun draw-lozenge (window gcontext width height x y)
  "Draw a hollow lozenge on WINDOW.
 (a LOZENGE is a rectangle whose left and right ends are <pointed>.
A lozenge whose width and height are equal is a diamond shape.)"
  (let* ((hh (floor (1- height) 2))
	 (cy (+ y hh    hh))
	 (cx (+ x width -1)))
    (draw-lines window gcontext (list
                ;;                        ; _
                ;;                        ;/ \  This looks like
                ;;                        ;\_/  what we are drawing
		x         (+ y hh)
		(+  x hh) y		  ;/
		(- cx hh) y		  ; _
		cx        (+ y hh)	  ;  \
		(- cx hh) cy		  ;  /
		(+  x hh) cy		  ; _
		x         (+ y hh)))	  ;\
    ))


;;;-----------------------------------------------------------------------------
;;; Alas, Common-lisp doesn't specify a portable way to make your own stream object.
;;; Here is a zetalisp implementation for lisp machines using clos-kludge.
;;; PLEASE mail an implementation for YOUR lisp to clue-review@dsg.csc.ti.com
#+(and lispm (not clos))
(defun (:property interactive-stream si:named-structure-invoke)
       (method self &rest args
	&aux (operations '( :which-operations :operation-handled-p :send-if-handles
			   :print-self :listen :clear-input :untyi :tyi :line-in
			   :clear-screen :tyo :string-out :fresh-line :rubout-handler :clear-eol)))
  (ecase method
    (:which-operations operations)
    (:operation-handled-p (member (first args) operations))
    (:send-if-handles (when (member (first args) operations)
			(apply self args)))
    (:print-self (format (first args) "#<interactive-stream ~o>" (si:%pointer self)))
    (:listen (apply #'stream-listen self args))
    (:clear-input (apply #'stream-clear-input self args))
    (:untyi (apply #'stream-unread-char self args))
    ((:tyi :any-tyi) (stream-read-char self))
    (:line-in (let ((leader (car args)))
		(stream-read-line self :leader-length (and (numberp leader) leader))))
    (:clear-screen (apply #'stream-clear-output self args))
    (:tyo (apply #'stream-write-char self args))
    (:string-out (apply #'stream-write-string self args))
    (:fresh-line (apply #'stream-fresh-line self args))
    (:clear-eol (apply #'clear-eol self args))
    (:force-output (display-force-output (contact-display self)))
    (:finish (display-finish-output (contact-display self)))
    (:rubout-handler (apply #'stream-rubout-handler self args))

;;    #+ti (:preemptable-read (apply #'stream-rubout-handler self args))
    #+ti (:read-cursorpos (values 0 0))
    #+ti (:process si:current-process)
    ))

#+(and Explorer CLOS)
(progn
(defmethod (interactive-stream :clear-eol) ()
  (clear-eol zl:self))

(defmethod (interactive-stream :line-in) (&optional leader)
  (stream-read-line zl:self :leader-length (and (numberp leader) leader)))

(defmethod ticlos:stream-read-line ((stream interactive-stream))
  (stream-read-line stream))

(defmethod ticlos:stream-read-char-no-hang ((self interactive-stream))
  (and (listen-character (contact-display self))
       (stream-read-char self)))

(defmethod ticlos:stream-force-output ((stream interactive-stream))
  (display-force-output (contact-display stream)))

(defmethod ticlos:stream-finish-output ((stream interactive-stream))
  (display-finish-output (contact-display stream)))

(defmethod ticlos:stream-line-column ((stream interactive-stream))
  (values (zl:send stream :read-cursorpos ':character)))

(defmethod ticlos:stream-start-line-p ((stream interactive-stream))
  (zerop (slot-value stream 'cursor-x)))

(defmethod (interactive-stream :read-cursorpos) (&optional units)
  (declare (notinline char-width))
  (if (eq units ':character)
      (values (round cursor-x (char-width font (char-int #\n)))
	      (round cursor-y line-height))
    ;; else assume pixels
    (values cursor-x cursor-y)))

(defmethod (interactive-stream :increment-cursorpos) (dx dy &optional units)
  (declare (notinline char-width))
  (if (eq units ':character)
      (stream-move-cursor zl:self (+ cursor-x (* dx (char-width font (char-int #\n))))
			  (+ cursor-y (* dy line-height)))
    ;; else assume pixels
    (stream-move-cursor zl:self (+ cursor-x dx) (+ cursor-y dy))))

(defmethod (interactive-stream :process) () si:current-process)

  ) ; end Explorer and CLOS


;;-----------------------------------------------------------------------------
;; A SIMPLE rubbout handler
;;		(what an understatement, but it's something to build on...)


(defmacro with-input-editing ((stream &optional rubout-options) &body body)
  "Execute BODY inside of STREAM's stream-rubout-handler method.
If BODY does input from STREAM, it will be done with rubout processing
if STREAM is an interactive-stream.
RUBOUT-OPTIONS should be the options for the stream-rubout-handler method"
  (unless stream (setq stream '*standard-input*))
  `(stream-rubout-handler ,stream ,rubout-options
			  #'(lambda () ,@body)))

(defun stream-rubout-handler (contact options function &rest args)
  ;; Rubout handling in the zetalisp tradition
  (if (typep contact 'interactive-stream)
      (let ((option-plist nil))
	(dolist (option options)
	  (setq option-plist (append option-plist option)))
	(if args
	    (funcall (stream-rubout-handler-function contact) contact option-plist
		     #'(lambda () (apply function args)))
	  (funcall (stream-rubout-handler-function contact) contact option-plist function)))
    (apply function args)))

(defmacro rubout-handler (&rest options &key (stream *terminal-io*) body
			  pass-through do-not-echo help initial-input)
  "Common-windows rubout-hander"
  (declare (ignore pass-through do-not-echo help initial-input))
  (let ((option-plist (copy-list options)))
    (remf option-plist :stream)
    (remf option-plist :body)
    `(rubout-handler-internal ,stream #'(lambda () ,body) ,@option-plist)))

(defun rubout-handler-internal (contact function &rest options)
  ;; Rubout handling in the common-windows tradition
  (if (typep contact 'interactive-stream)
      ;; Note: OPTIONS doesn't have to be copied, even though it's an &rest arg,
      ;; because it's never referenced outside this dynamic scope.
      (funcall (stream-rubout-handler-function contact) contact options function)
    (funcall function)))
;;
;; Rubout-Handler-Buffer
;;
(defstruct (rubout-handler-buffer (:conc-name rhb-))
  (fill-pointer   0)
  (scan-pointer   0)
  (buffer (make-array 128 :element-type 'string-char))
  (options nil))

;; Since it doesn't make much sense for a process to get input from more
;; than one stream at a time, This rubout handler implementation doesn't
;; allocate a rubout-handler-buffer for each stream.  Instead, a cache
;; of buffers is kept, and the rubout-handler-buffer is bound to *rhb*
;; within the scope of the rubout handler, instead of in a slot of the
;; stream.

(defvar *rhb* nil) ;; rubout-handler-buffer
(defvar *rhb-cache* nil) ;; rubout handler buffer cache

(defun allocate-rhb ()
  (or (pop *rhb-cache*)
      (make-rubout-handler-buffer)))

(defun deallocate-rhb (rhb)
  (setf (rhb-options rhb) nil) ;; Zap options for garbage collection
  (push rhb *rhb-cache*))

(defun get-rubout-handler-buffer (stream)
  "Return a string that represents the current state of
 the rubout handler associated with STREAM.
 This must be called from within the BODY passed to rubout handler."
  (declare (ignore stream))
  ;; We keep the rubout-handler-buffer in *RHB*, NOT in a slot of the stream.
  (subseq (rhb-buffer *rhb*) 0 (rhb-fill-pointer *rhb*)))

;; *RUBOUT-HANDLER* keeps track of the state of the rubout handler.  It can have
;; one of the following settings:
;;
;;    NIL	Outside the rubout handler
;;    READ	Inside the rubout-handler but not inside rubout-handler-edit
;;    TYI	Inside rubout-handler-edit
;;
;; This variable is bound back to NIL whenever entering a new listener loop
;; which establishes its own editing context.
(defvar *rubout-handler* nil "Rubout handler state. NIL when not INSIDE the rubout handler")

(defun simple-rubout-handler (contact options function)
  ;; A rubout handler in the common-windows tradition
  ;; Options include:
  ;; :full-rubout flag    If the user erases all of the characters then presses
  ;;                      the rubout character once more, control is returned
  ;;			  from the input editor immediately.  Two values are
  ;;			  returned: NIL and FLAG.  In the absence of this option,
  ;;			  the input editor simply waits for more characters.
  ;;
  ;; :prompt string	  string to display or function of one argument (the contact)
  ;; :reprompt string
  ;; :initial-input string
  ;; :initial-input-pointer card16
  (let ((*rhb* (allocate-rhb)))
    (unwind-protect
	(progn
	  (setf (rhb-options *rhb*) options)
	  (setf (rhb-fill-pointer *rhb*) 0)	   ; number of characters in the buffer
	  (setf (rhb-scan-pointer *rhb*) 0)	   ; number of characters sent to application

	  ;; PROMPT option
	  (let ((prompt-option (getf options :prompt)))
	    (cond ((null prompt-option))
		  ((stringp prompt-option)
		   (stream-write-string contact prompt-option))
		  #+explorer ;; explorer error-handler hack
		  ;; old zetalisp required 2 arguments to prompt and read.
		  ((eq prompt-option 'sys:prompt-and-read-prompt-function)
		   (funcall prompt-option contact nil))
		  (t (funcall prompt-option contact))))

	  ;; INITIAL-INPUT option
	  (let ((initial-input (getf options :initial-input)))
	    (when initial-input
	      (let* ((initial-input-pointer (getf options :initial-input-pointer 0))
		     (length (- (length initial-input) initial-input-pointer))
		     (size (array-total-size (rhb-buffer *rhb*))))
		(when (> length size)
		  (setf (rhb-buffer *rhb*) (adjust-array (rhb-buffer *rhb*) (+ length size))))
		(replace (rhb-buffer *rhb*) initial-input :start1 initial-input-pointer)
		(setf (rhb-fill-pointer *rhb*) length)
		(stream-write-string contact initial-input initial-input-pointer))))

	  (when (slot-value (the interactive-stream contact) 'unreadp) ; Make sure type ahead is processed
	    (simple-rubout-handler-edit contact)		    ; by rubout handler, not just by TYI
	    (setf (rhb-scan-pointer *rhb*) 0))
	  (do ((*rubout-handler* 'read)	   ;Establish rubout handler
	       #+ti (si:rubout-handler t)  ;Needed for explorer compatability
	       )
	      (nil)
	    (catch 'rubout-handler		; Throw here when rubbing out
	      (progn
		#+cleh				; Hopefully, someday everyone will use this
		(conditions:handler-case
		    (return (funcall function))
		  (error (condition) (princ condition)))
		#+(and lispm (not cleh))
		(si:catch-error			; If a read-error occurs, print a message and loop back
		  (return			; Exit rubbout handler when read function returns
		    (funcall function)))	; Call read function
		#+(and kcl (not cleh))
		(multiple-value-bind (tag value)
		    (si:error-set `(funcall ',function))
		  (unless tag (return value)))
		#+(and excl (not cleh))
		(multiple-value-bind (tag value)
		    (excl:error-set (funcall function) :announce)
		  (unless tag (return value)))
		#-(or cleh lispm kcl excl)
		(return (funcall function))
		;; We come here after read errors (catch-error caught)
		(fresh-line contact)		; Echo the rubout handler buffer
		(stream-write-string contact (rhb-buffer *rhb*) 0 (rhb-fill-pointer *rhb*))
		(loop (stream-read-char contact))))	; and force user to edit it
	    ;; Come here on throw to 'rubout-handler
	    ;; Maybe return when user rubs all the way back
	    (and (zerop (rhb-fill-pointer *rhb*))
		 (let ((full-rubout-option (getf options :full-rubout)))
		   (and full-rubout-option (return (values nil full-rubout-option)))))))
      (deallocate-rhb *rhb*))))

(defun rh-read-char (contact &aux idx)
  ;; Get the next character from the rubout-handler buffer, or the user
  ;; Called from stream-read-char when *rubout-handler* is 'read
  (cond ((> (rhb-fill-pointer *rhb*)	   ;Return characters from rhb until end of buffer
	    (setq idx (rhb-scan-pointer *rhb*)))
	 (setf (rhb-scan-pointer *rhb*) (1+ idx))
	 (aref (rhb-buffer *rhb*) idx))
	(t (simple-rubout-handler-edit contact)))	;Else, editing the buffer
  )

(defun simple-rubout-handler-edit (contact)
  ;; This is the "guts" of the rubout handler, where the editing occurs
  ;; This needs LOTS more editing commands!
  (do ((rubbed-out-some nil)
       (*rubout-handler* 'tyi)
       (ch))
      (nil) ;; forever
    (setq ch (stream-read-char contact))
    (case ch
      (#\control-u				;CLEAR-INPUT flushes all buffered input
       (setf (rhb-fill-pointer *rhb*) 0)
       (setq rubbed-out-some t)			;Will need to throw out
       (stream-write-char contact ch)		;Echo and advance to new line
       (stream-write-char contact #\Newline))
      (#\control-l				;Retype buffered input
       (display contact)
       (let ((prompt (or (getf (rhb-options *rhb*) :reprompt)
			 (getf (rhb-options *rhb*) :prompt))))
	 (cond ((null prompt))
	       ((stringp prompt)
		(stream-write-string contact prompt))
	       #+explorer ;; explorer error-handler hack
	       ;; old zetalisp required 2 arguments to prompt and read.
	       ((eq prompt 'sys:prompt-and-read-prompt-function)
		(funcall prompt contact nil))
	       (t
		(funcall prompt contact))))
       (stream-write-string contact (rhb-buffer *rhb*) 0 (rhb-fill-pointer *rhb*)))
      (#\Rubout
       (let ((len (rhb-fill-pointer *rhb*)))
	 (unless (zerop len)
	   (setf (rhb-fill-pointer *rhb*) (setq len (1- len)))
	   (set-cursorpos contact :x (- (stream-cursor-x contact)
					(char-width (stream-font contact)
						    (char-int (aref (rhb-buffer *rhb*) len)))))
	   (clear-eol contact)
	   (setq rubbed-out-some t)
	   (when (zerop len) ;; when all rubbed out
	     (setf (rhb-scan-pointer *rhb*) 0)
	     (throw 'rubout-handler t)))))
      (otherwise
       (if (plusp (char-bits ch))
	   (bell (contact-display contact)) ;; unknown command
	 ;; Echo character
	 (let ((fill-pointer (rhb-fill-pointer *rhb*)))
	   (stream-write-char contact ch)
	   ;; Put character in buffer, after first ensuring its big enough
	   (when (> (setf (rhb-fill-pointer *rhb*) (1+ fill-pointer))
		    (array-total-size (rhb-buffer *rhb*)))
	     (setf (rhb-buffer *rhb*)
		   (adjust-array (rhb-buffer *rhb*) (* 2 fill-pointer))))
	   (setf (aref (rhb-buffer *rhb*) fill-pointer) ch)
	   (cond (rubbed-out-some
		  ;; Make the reader closure re-read all input from the beginning
		  (setf (rhb-scan-pointer *rhb*) 0)
		  (throw 'rubout-handler t))
		 (t
		  ;; New character at the end of the buffer, just return it.
		  (setf (rhb-scan-pointer *rhb*) (rhb-fill-pointer *rhb*))
		  (return ch)))))))))
#+ti
(defun handle-asynchronous-characters (char contact)
  ;; This handles things like abort, break, system and terminal for TI Explorers
  (let ((entry (assoc char tv:kbd-intercepted-characters)))
    (cond (entry (funcall (second entry) char))
	  ((setq entry (assoc char tv:kbd-global-asynchronous-characters))
	   (funcall (second entry) char contact))
	  ((setq entry (assoc char tv:kbd-standard-asynchronous-characters))
	   (funcall (second entry) char contact)))))
