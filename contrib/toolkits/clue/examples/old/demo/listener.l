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

;;; Change history
;;; ----------------------------------------------------------------------------
;;; 09/11/87	LGO	Created

;;; Note to franz users from John Irwin, Franz Inc.:
;;;	Re: the definition of interactive-stream in examples/stream.l:
;;;   	streams are "privileged" objects in Allegro CL.  This causes death when
;;;   	trying the lisp-listener demo since *terminal-io* is bound to an
;;;   	interactive stream.

(in-package 'clue-examples :use '(lisp xlib clos cluei))



(define-resources
  (top-level listener height) 400
  (top-level listener width) 800
  (* interactive-stream * background) 0
  (* interactive-stream * font) "fg-18"
  (* interactive-stream * lozenge-font) "vgi-20"
  )

(defun lisp-listener (display)
  (let ((s (make-contact 'interactive-stream :parent display :name 'listener
                         :exposures :off
                         :cursor-y 10)))
    
    ;; Simple lisp listener
    (#+ti si:let-globally #+ti((eh:*use-old-debugger* t))
     #-ti progn
     (add-mode s :exclusive 'ignore-action)
     (let ((*debug-io* *terminal-io*)
           (*trace-output* *terminal-io*)
           (*terminal-io* s)
           (*standard-output* s)
           (*standard-input* s)
           (*query-io* s)
           - + ++ +++ * ** *** / // ///
           )
       (declare (special - + ++ +++ * ** *** / // ///))
       ;; On the Explorer, catch the ABORT key from the debugger
       (#+ti si:error-restart-loop  #+ti((si:abort error) "Return to Lisp Listener top level.")
        #-ti loop
        (fresh-line)
        (shiftf +++ ++ + - (read))
        (shiftf /// // / (multiple-value-list (eval -)))
        (shiftf *** ** * (car /))
        (dolist (value /)
          (fresh-line)
          (prin1 value)))))))

(defun set-keymap (display character)
  ;; Change a keyboard-mapping for HOST.
  (let* ((keysym (car (xlib:character->keysyms character)))
	 (width 300)
	 (height 40)
	 (screen (xlib:display-default-screen display))
	 (black (xlib:screen-black-pixel screen))
	 (white (xlib:screen-white-pixel screen))
	 (win (xlib:create-window
		:parent (xlib:screen-root screen)
		:background white
		:border black
		:border-width 3
		:colormap (xlib:screen-default-colormap screen)
		:event-mask '(:exposure :key-press)
		:x 20 :y 20
		:width width :height height))
	 (gc (xlib:create-gcontext
	       :drawable win
	       :background white
	       :foreground black
	       :font "vg-25")))
    (unwind-protect
	(progn 
	  (xlib:map-window win)
	  (xlib:event-case (display :discard-p t :force-output-p t)
	    (:exposure (count)
	       (when (zerop count) ;; Ignore all but the last exposure event
		 (xlib:clear-area win)
		 (xlib:draw-glyphs win gc 20 20
			      (if (null keysym)
				  (format nil "No keysym defined for the ~@:c key" character)
				(format nil "Press the ~@:c key" character))))
	       nil)
	    (:key-press (code state)
		(macrolet ((keystate-bit (state keyword)
			     `(the bit
				   (ldb (byte 1 ,(or (position keyword xlib::*state-mask-vector*)
						     (error "~s isn't a state-mask keyword" keyword)))
					,state))))		  
		  (let ((index (logxor (keystate-bit state :shift)
				       (keystate-bit state :control)
				       (keystate-bit state :lock))))
		    (modify-keyboard-mapping display code index keysym)))
		t)))
      (xlib:display-finish-output display))))

(defun modify-keyboard-mapping (display code index keysym)
  ;; Modify the keyboard-mapping for keycode CODE and INDEX to be KEYSYM
  (let* ((mapping (xlib:keyboard-mapping display :first-keycode code :start 0 :end 1)))
    (setf (aref mapping 0 index) keysym)
    (xlib:change-keyboard-mapping display mapping :first-keycode code)))

#+explorer
(defun set-sun-keys ()
  ;; Define keysyms mapping Sun function keys to Explorer special characters.
  (let ((f1 (keysym 255 190)) (f2 (keysym 255 191)) (f3 (keysym 255 145))
	(f4 (keysym 255 146)) (f5 (keysym 255 147)) (f6 (keysym 255 195))
	(f7 (keysym 255 196)) (f8 (keysym 255 197)) (f9 (keysym 255 198))
	(l1 (keysym 255 200)) (l2 (keysym 255 201)) (l3 (keysym 255 202))
	(l4 (keysym 255 203)) (l5 (keysym 255 204)) (l6 (keysym 255 205))
	(l7 (keysym 255 206)) (l8 (keysym 255 207)) (l9 (keysym 255 208))
	(l10 (keysym 255 209))
	(r1 (keysym 255 210)) (r2 (keysym 255 211)) (r3 (keysym 255 212))
;;	  (R4 (keysym 255 213)) (R5 (keysym 255 214)) (R6 (keysym 255 215))
;;	  (R7 (keysym 255 216)) (R8 (keysym 255 217)) (R9 (keysym 255 218))
;;	  (R10 (keysym 255 219)) (R11 (keysym 255 220)) (R12 (keysym 255 221))
;;	  (R13 (keysym 255 222)) (R14 (keysym 255 223)) (R15 (keysym 255 224))
	)
    f5 l3 l4 ;; unused
    (define-keysym #\help l1)
    (define-keysym #\help l2)
    (define-keysym #\resume l5)
    (define-keysym #\resume l6)
    (define-keysym #\break l7)
    (define-keysym #\break l8)
    (define-keysym #\abort l9)
    (define-keysym #\abort l10)
    
    (define-keysym #\system f1)
    (define-keysym #\Network f2)
    (define-keysym #\Status f3)
    (define-keysym #\terminal f4)
    
    (define-keysym #\refresh f6)
    (define-keysym #\clear-input f7)
    (define-keysym #\undo f8)
    (define-keysym #\end f9)
    
    (define-keysym #\f1 r1)
    (define-keysym #\f2 r2)
    (define-keysym #\f3 r3)
    t
    ))

#+explorer
(set-sun-keys)









