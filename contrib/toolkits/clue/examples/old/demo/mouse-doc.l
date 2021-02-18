;;; -*- Mode:Lisp; Package:XLIB; Base:10; Syntax:Common-Lisp; Lowercase:T -*-

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

;;; Created 2/29/88 by LaMott G. Oren

;;; Change history:
;;;
;;;  Date	Author	Description
;;; -------------------------------------------------------------------------------------
;;;  02/29/88   LGO     Created
;;;  07/14/88   SLM     Macro-expand inline definitions for function TICL:PV, which is defined
;;;                     in the PUBLIC directory.  Don't want dependencies on non-standard software.
;;;  07/15/88   SLM     Round width and height to whole numbers to prevent text display errors

(in-package 'xlib :use '(lisp))

(export 'mouse-doc)

(defun mouse-doc (host &rest args &key (font "fg-16") (width 80) (height 1) (border-width 1) debug)
  ;; X11 client for printing mouse documentation strings using FONT
  ;; IN ITS OWN WINDOW on HOST.  Width and height are in characters.
  ;;
  ;; This program looks for and displays the WM_DOCUMENTATION property
  ;; on the window the mouse is in.
  (let* ((xlib::*recursive-event-queue* nil)
	 (display (open-display host))
	 (screen (display-default-screen display))
	 (root (screen-root screen))
	 (black (screen-black-pixel screen))
	 (white (screen-white-pixel screen))
	 (font (open-font display font))
	 (border 1)				; Minimum margin around the text
	 ;; Convert width and height from characters to pixels
	 (width (round (* width (max-char-width font))))
	 (height (round (+ (* height (+ (max-char-ascent font) (max-char-descent font))) (* 2 border))))
	 (bw2 (+ border-width border-width))
	 ;; Initial placement is in lower-left hand corner
	 (x (- (screen-width screen) width bw2))
	 (y (- (screen-height screen) height bw2))
	 
	 (doc-window (create-window
		       :parent root
		       :x x :y y :width width :height height
		       :background black
		       :border white
		       :border-width border-width
		       :colormap (screen-default-colormap screen)
		       :bit-gravity :center))

	 (gcontext (create-gcontext
		     :drawable doc-window
		     :background black
		     :foreground white
		     :font font))
	 (event-state 0)
	 (string "")
	 (pointer-window nil))  ;The window we think the mouse is in.
    ;; Set window manager hints
    (set-standard-properties doc-window
			     :name 'mouse-documentation
			     :icon-name "Mouse Doc"
			     :resource-name 'mouse-doc
			     :resource-class 'mouse-doc
			     :command (list* 'mouse-doc host args)
			     :x x :y y :width width :height height
			     :min-width width :min-height height
			     :input :off :initial-state :normal)
    
    (unwind-protect
	(macrolet
	  ((window-doc (window) `(getf (window-plist ,window) :documentation))
	   (window-children (window) `(getf (window-plist ,window) :children)))
	  (labels

	    ((get-documentation (window &optional parent-doc)
	       ;; Get the documentation properties for WINDOW and all its descendants.
	       (declare (type window window))
	       (when debug 
		 (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil) 'get-documentation
			 'window window) )  ;;this format statement used to be a call to TICL:PV
	       ;; Find and save WM documentation if the window doesn't already have its own.
	       (unless (window-doc window)
		 (multiple-value-bind (doc type format)
		     (get-property window :wm_documentation :type :string
				   :result-type 'string :transform #'card8->char)
		   (unless (eq type :string)
		     (setq doc (get-property
				 window :wm_documentation :type type
				 :result-type (if (= format 8) '(array card8) '(array card16)))))
		   (when (and debug doc) (format t "~%~s ~d  ~a" window (window-id window) doc))
		   (setq parent-doc (or doc parent-doc :unknown))
		   (setf (window-doc window) parent-doc)
		   (setf (window-event-mask window)
			 (if (not (eq parent-doc :unknown))
			     '(:enter-window :leave-window)
			   '(:enter-window :leave-window)))))
	       (let ((tree (query-tree window)))
		 (setf (window-children window) tree)
		 (dolist (w tree)
		   (get-documentation w parent-doc))))

	     (pointer-window (pointer-window)
		;; Find the leaf window under the pointer, starting from pointer-window
		(loop
		  (multiple-value-bind (x y same-screen-p child)
		      (query-pointer pointer-window)
		    (declare (ignore x y same-screen-p))
		    (setq pointer-window child))
		  (when debug 
		    (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil)
			    '**********-query-pointer 'pointer-window pointer-window))  ;;another expansion of TICL:PV
		  (unless pointer-window (return nil))
		  (unless (window-children pointer-window)
		    (return pointer-window))))

	     (remove-window (window)
		;; Remove WINDOW and all its descendants from the window cache
		(deallocate-resource-id display (window-id window) 'window)
		(dolist (w (window-children window))
		  (remove-window w)))

	     (display ()
	       ;; Display the current documentation string (if any)
	       (typecase string
		 (string
		  (let ((x 0)
			(y (max-char-ascent font)))
		    ;; Draw text centered in window in Y direction
		    (draw-glyphs doc-window gcontext x y string)))
		 ((array card8)
		  (display-window-doc doc-window gcontext string *x-polytext8* event-state height))
		 ((array card16)
		  (display-window-doc doc-window gcontext string *x-polytext16* event-state height))
		 ))

	     (look-ahead (display timeout)
		;; Returns T when enter-notify events are in the event queue
		(event-case (display :peek-p t :timeout timeout)
		  ((enter-notify leave-notify) () t))))
	    
	    ;; Initialize
	    (dolist (screen (display-roots display))
	      (get-documentation (screen-root screen))
	      (setf (window-event-mask (screen-root screen))
		    '( :enter-window :leave-window	;; Watch the mouse
		      :key-press :key-release		;; Watch the modifier keys
		      :substructure-notify)))		;; Watch for destroy-notify
	    (setf (window-event-mask doc-window) '(:exposure :button-press :structure-notify))
	    (map-window doc-window)
	    
	    (setq pointer-window (pointer-window (screen-root screen)))
	    (setq string (and pointer-window (window-doc pointer-window)))

	    ;; Handle events
	    (event-case (display :discard-p t :force-output-p t)
	      (configure-notify ;; Keep width and height current
		((window event-window) (width new-width) (height new-height) (border-width new-border-width))
		(when debug 
		  (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil) 'configure-notify
			  'event-window event-window 'doc-window doc-window 'new-width new-width 'new-height
			  new-height 'new-border-width new-border-width) )
		(when debug (print (make-event-keys (window-event-mask doc-window))))
		(when (window-equal event-window doc-window)
		  (setq width new-width
			height new-height
			border-width new-border-width)
		  (display))
		nil)

	      (exposure  ;; Refresh on demand
		((window event-window) count)
		(when debug 
		  (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil) 'exposure
			  'event-window event-window 'count count) )
		(when (and (zerop count) ;; Ignore all but the last exposure event
			   (window-equal event-window doc-window))
		  (display)
		  ;; Returning non-nil causes event-case to exit
		  nil))

	      (key-press ;; If a modifier key is pressed, update the event-state
		((window event-window) code state)
		(let* ((keysym (keycode->keysym display code 0))
		       (mapping (get-display-modifier-mapping display))
		       (mask (assoc keysym mapping)))
		  (when mask
		    (setq event-state (logior state (cdr mask)))
		    (when string (clear-area doc-window))
		    (display)))
		(when debug 
		  (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil) 'key-press
			  'event-window event-window 'code code 'state state '(make-state-keys event-state)
			  (make-state-keys event-state)) )
		nil)

	      (key-release  ;; If a modifier key is released, update the event-state
		((window event-window) code state)
		(let* ((keysym (keycode->keysym display code 0))
		       (mapping (get-display-modifier-mapping display))
		       (mask (assoc keysym mapping)))
		  (when mask
		    (setq event-state (logandc2 state (cdr mask)))
		    (when string (clear-area doc-window))
		    (display)))
		(when debug 
		  (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil) 'key-release
			  'event-window event-window 'code code 'state state '(make-state-keys event-state)
			  (make-state-keys event-state)) )
		nil)

	      ;; Enter-notify is selected only on those windows known to have documentation
	      (enter-notify
		((window event-window) state kind mode)
		(when debug 
		  (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil) 'enter-notify
			  'event-window event-window 'kind kind 'mode mode) )
		(setq pointer-window event-window)
		(setq event-state state)
		(unless (look-ahead display 0)
		  (let ((doc (or (window-doc event-window)
				 (get-documentation event-window))))
		    (when (setq string (if (eq doc :unknown) nil doc))
		      (display)))
		  nil))

	      ;; Rather than select substructure-notify on the world, we select
	      ;; leave-notify, and make sure we know about all the windows the
	      ;; mouse will be in.
	      (leave-notify
		(child kind mode window)
		(when debug 
		  (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil) 'leave-notify 'child
			  child 'kind kind 'mode mode 'window window) )
		(when string
		  (clear-area doc-window)
		  (setq string nil))
		(unless (look-ahead display 0.3) ;; when no enter-notify follows
		  ;; Must have moved to a window we don't know about... Find it!
		  (setq pointer-window (pointer-window (screen-root screen)))
		  (when debug 
		    (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil)
			    '****-finding-pointer-window*** 'pointer-window pointer-window) )
		  (when (and pointer-window (not (window-doc pointer-window)))
		    (get-documentation pointer-window)
		    (setq pointer-window (pointer-window pointer-window))
		    (when (setq string (and pointer-window (window-doc pointer-window)))
		      (display))))
		nil)

	      (map-notify
		((window event-window)) (when debug 
					  (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil) 'map-notify
						  'event-window event-window) )
		(when (not (window-doc event-window))
		  (get-documentation event-window))
		nil)

	      ;; When window is destroyed, Remove window-id's from cache
	      (destroy-notify
		((window event-window)) (when debug 
					  (format *debug-io* "~%;;~@{~<~%;;~1:; ~@[~a~] ~s~>~^,~2,10t~}." (quote nil) 'destroy-notify
						  'event-window event-window) )
		(remove-window event-window)
		nil)

	      (button-press () t)))) ;; Pressing any mouse-button exits

      ;; Ensure display is closed when done
      (close-display display))))

(defun display-window-doc (window gcontext data request event-state height)
  (do* ((i 0 (+ i length))
	(start 0)
	(end (length data))
	(length 0 0)
	(state 0)
	(select 0)
	(display (window-display window))
	;;(window-height (screen-height (display-default-screen display)))
	(font (gcontext-font gcontext))
	(line-height (+ (max-char-ascent font) (max-char-descent font) 2))
	(x 0)
	(y (max-char-ascent font)))
       ((< height (+ y line-height)))		;;Remove this condition to make any and all text display 
						;;even if there's not enough height for the whole line to show.
						;;Otherwise, this is an efficiency hack.
    (when (< i end) (setq length (aref data i)))
    (case length
      (0					; New state/select, or end
       ;; Display previously scanned text
       (when (and (zerop (logand select (logxor state event-state)))
		  (< start i))
	 (setq length (- i start))
	 (with-buffer-request (display request :gc-force gcontext :length length)
	   (drawable window)
	   (gcontext gcontext)
	   (int16 x y)
	   (progn
	     (let ((boffset (index+ buffer-boffset length 16)))
	       (buffer-replace buffer-bbuf data
			       (index+ buffer-boffset 16)
			       boffset
			       start)
	       ;; Ensure terminated with zero bytes
	       (do ((end (the array-index (lround boffset))))
		   ((index>= boffset end))
		 (setf (aref buffer-bbuf boffset) 0)
		 (index-incf boffset))
	       (length-put 2 (index-ash (index- (lround boffset) buffer-boffset) -2)) ;; Set request length
	       (setf (buffer-boffset display) (lround boffset)))
	     (incf y line-height))))
       ;; Check for termination
       (when (>= i (- end 5)) (return nil))
       ;; Get new state/select
       (setq state (dpb (aref data (+ i 1)) (byte 8 8) (aref data (+ i 2)))
	     select (dpb (aref data (+ i 3)) (byte 8 8) (aref data (+ i 4)))
	     length 5
	     start (+ i length)))
      (255					; New Font
       (setq length 5))
      (otherwise				; More text
	(incf length 2)))))

