;;; -*- Mode:Lisp; Package:XLIB; Syntax:COMMON-LISP; Base:10; Lowercase:T -*-

;;; CLX Using-Gcontext macro

;;;
;;;			 TEXAS INSTRUMENTS INCORPORATED
;;;				  P.O. BOX 149149
;;;			       AUSTIN, TEXAS 78714-9149
;;;
;;; Copyright (C)1987-1990 Texas Instruments Incorporated.
;;;
;;; Permission is granted to any individual or institution to use, copy, modify,
;;; and distribute this software, provided that this complete copyright and
;;; permission notice is maintained, intact, in all copies and supporting
;;; documentation.
;;;
;;; Texas Instruments Incorporated provides this software "as is" without
;;; express or implied warranty.
;;;

;;; Created 03/16/88 by LaMott G. OREN

(in-package "XLIB")

(export 'using-gcontext)

#| TO DO:

Date: Wed, 22 Jun 88 14:51:22 -0700
From: joel@wsl.dec.com
Subject: gc cache 
To:   lagoze@cu-arpa.cs.cornell.edu
Cc:   joel@wsl.dec.com, xpert@athena.mit.edu, dec-toolkit@wsl.dec.com
In-Reply-To: Your message of Wed, 22 Jun 88 14:26:27 -0700.
             <8806222126.AA09675@gilroy.dec.com> 


I improved the XtGetGC substantially recently, these changes will go into the
R3 release.

The two biggest changes I made were to make the GC list self-organized: when
you match against an existing GC, move it to the head of the list.  On the DEC
mail program this reduced average search length from about 6.8 Matches per
search down to 1.7 Matches per search.

Secondly, I vastly increased the speed of Match by checking against the most
commonly specified fields first, then short-circuiting at two places if
nothing else needed to be checked.  For the programs I was profiling, this got
the two routines down into the noise.

There may still be a need to break up the linear list by hashing on, say,
foreground, background, and font, maybe one or two others.  I'm always leery
of linear lists.  But this is one place where locality of reference really
minimized search length on the programs I profiled.

- Joel McCormack

|#

(defvar *gcontext-cache-length* 6 "No more than this many gcontexts in a cache")

;; The default gcontext state defined by the X protocol.
(defconstant *default-gcontext-state*
	     '#(3 #xffffffff 0 1 0 0 1 0 0 0 nil nil 0 0 nil 0 1 0 0 0 0 nil 1 nil (4) nil 1))

;; Dummy gcontext containing the default gcontext state.
;; It may be (carefully) modified with the standard gcontext setf'ers
;; Avoid putting anything here containing a resource-id!
(defparameter *default-gcontext*
	      (let ((default-state (copy-seq (the gcontext-state *default-gcontext-state*))))
		(make-gcontext :local-state default-state :server-state default-state)))

;; Default to NOT getting graphics exposure events
(setf (gcontext-exposures *default-gcontext*) :off)

;; Temp-Gcontext Resource
(defvar *gcontext-cache* nil) ;; List of unused gcontexts

(defun allocate-gcontext ()
  (or (pop *gcontext-cache*)
      (make-gcontext)))

(defun deallocate-gcontext (gc)
  (push gc *gcontext-cache*))

;;;Note: Find-Gcontext puts the display in the gcontext structure.
;;;      However, when specifying a font, the gcontext-font setf'er
;;;      needs the display in the gcontext structure BEFORE
;;;      Find-Gcontext is called.  Because of this, when there's a font
;;;      parameter using-gcontext generates extra code to initialize the
;;;      display slot of the gcontext structure before the gcontext-font
;;;      setf'er is called.

(defmacro using-gcontext ((gcontext &rest options &key drawable default clip-ordering &allow-other-keys)
			  &body body)
  ;; Equivalent to `(let ((gcontext (create-gcontext ,@options))) ,@body (free-gcontext ,gcontext))
  ;; Except more efficient because gcontexts are cached.

  (declare (arglist
	     (gcontext &rest options &key default drawable
		       function plane-mask foreground background
		       line-width line-style cap-style join-style fill-style fill-rule
		       arc-mode tile stipple ts-x ts-y font subwindow-mode
		       exposures clip-x clip-y clip-mask clip-ordering
		       dash-offset dashes &allow-other-keys)
	     &body body))
  (setq options (copy-list options))
  (remf options :clip-ordering)
  (remf options :default)
  (remf options :drawable)

  (unless drawable (required-arg drawable))

  (let ((setfs nil)
	(value (gensym))
	(drawable-arg (and (getf options :font) (gensym))))
    (do* ((option options (cddr option))
	  (name (car option) (car option)))
	 ((endp option) (setq setfs (nreverse setfs)))
      ;; The CLX GContext stuff was engineered to make NIL values mean
      ;; "unknown value".  Wrap SETF's with checks to ignore keyword
      ;; options with NIL values.
      (let ((val (cadr option))
	    (accessor `(,(xintern 'gcontext- name) ,gcontext ,@(when (eq name :clip-mask)
							   `(,clip-ordering)))))
	(cond ((constantp val)			; Optimize for constant value
	       (when (eval val)
		 (push `(setf ,accessor ,val) setfs)))
	      ((atom val)			; Optimize for variable value
	       (push `(when ,val (setf ,accessor ,val)) setfs))
	      (t (push `(let ((,value ,val))	; Don't evaluate value expression twice
			  (when ,value (setf ,accessor ,value)))
		       setfs)))))
    (when drawable-arg
      (push `(setf (gcontext-display ,gcontext) (drawable-display ,drawable-arg))
	    setfs))

    (if (and default (null setfs))
	`(let ((,gcontext ,default)) ,@body)
      `(let ((,gcontext (allocate-gcontext))
	     ,@(when drawable-arg
		 (prog1 `((,drawable-arg ,drawable))
			(setq drawable drawable-arg))))
	 (declare (type gcontext ,gcontext))
	 (replace (gcontext-local-state ,gcontext)
		  (gcontext-local-state ,(or default '(the gcontext *default-gcontext*))))
	 ,@setfs
	 (setq ,gcontext (find-gcontext ,gcontext ,drawable))
	 (unwind-protect
	     (progn ,@body)
	   (return-gcontext ,gcontext))))))

;;; Note: When find-gcontext can't find a match and must modify an
;;;       existing cache entry it ALWAYS modifies the oldest cache
;;;	  entry.  I tried an algorithm that modified the "closest"
;;;	  matching entry in the cache, and the cache thrashed 
;;;	  terribly.  Perhaps modifying the closest matching entry 
;;;       in the oldest half of the cache?

(defun find-gcontext (gcontext drawable)
  (let ((display     (drawable-display drawable))
	(local-state (gcontext-local-state gcontext))
	(cache (getf (drawable-plist drawable) 'gcontext-cache)))
    (declare (type list cache))
    (declare (type gcontext-state local-state))
    
    (with-display (display)			; Get a lock, just in case
      ;; The cache starts with a null cons.  This enables us to
      ;; update the cache without searching the plist again.
      (unless cache
	(setf (getf (drawable-plist drawable) 'gcontext-cache)
	      (setq cache (list nil))))
      
      ;; Ensure the timestamp matches
      (setf (gcontext-internal-timestamp local-state) 0)
      
      ;; Search for a matching gcontext
      (do* ((previous cache entry)
	    (entry (cdr cache) (cdr entry))
	    (n 0 (index+ n 1)))
	   
	   ((endp entry)
	    (if (< n *gcontext-cache-length*)
		;; Create a new gcontext if our limit isn't up
		(create-temp-gcontext gcontext drawable)
		
		;; Else modify the last cache entry
		(let* ((gc                   (first previous))
		       (updated-local-state  (gcontext-local-state gc))
		       (updated-server-state (gcontext-server-state gc)))
		  (declare (type gcontext gc))
		  (declare (type gcontext-state updated-local-state updated-server-state))
		  
		  ;; Modify cache gcontext with requested state
		  (replace updated-local-state local-state)

		  ;; Ensure that force-gcontext-changes will update server state
		  (fill updated-server-state nil)
		  (setf (gcontext-internal-timestamp updated-server-state) -1)

		  ;; Remove last entry from cache
		  (rplacd (nthcdr (index- n 1) cache) nil)
		  (deallocate-gcontext gcontext)
		  gc)))
	
	(declare (type list previous entry))
	
	(let* ((gc         (car entry))
	       (test-state (gcontext-local-state gc)))
	  (declare (type gcontext gc))
	  (declare (type gcontext-state test-state))
	  
	  (when (equalp test-state local-state)	; Found a match!
	    (setf (cdr previous) (cdr entry))
	    (deallocate-gcontext gcontext)
	    (return gc)))))))

(defun return-gcontext (gcontext)
  (declare (type gcontext gcontext))
  (let* ((drawable (gcontext-drawable gcontext))
	 (local-state (gcontext-local-state gcontext)))
    (setf (gcontext-internal-timestamp local-state) 0)
    ;; most recently used gcontexts are at the front of the cache
    (let ((cache (getf (drawable-plist drawable) 'gcontext-cache)))
      (setf (cdr cache) (cons gcontext (cdr cache))))))


(defun create-temp-gcontext (gcontext drawable)  
  (declare (type gcontext gcontext))
  (let* ((display      (drawable-display drawable))
	 (server-state (gcontext-server-state gcontext))
	 (local-state  (gcontext-local-state gcontext))
	 (gcontextid   (allocate-resource-id display gcontext 'gcontext)))
    (declare (type gcontext-state server-state local-state))
    
    (setf (gcontext-display  gcontext) display
	  (gcontext-id       gcontext) gcontextid
	  (gcontext-drawable gcontext) drawable)
    (setf (gcontext-internal-timestamp local-state) 1)
    (replace server-state (the gcontext-state *default-gcontext-state*))
    
    (with-display (display)
      (setf (gcontext-internal-timestamp local-state)
	    (incf-internal-timestamp server-state))
      
      (with-buffer-request (display *x-creategc*)
	(resource-id gcontextid)
	(drawable    drawable)
	(progn
	  (do ((i     0 (index+ i 1))
	       (bit   1 (the gcmask (ash bit 1)))
	       (nbyte 16)
	       (mask  0)
	       (local 0))
	      
	      ((index>= i *gcontext-fast-change-length*)
	       (card32-put 12 mask)
	       (card16-put 2 (index-ash nbyte -2))
	       (index-incf (buffer-boffset display) nbyte))
	    
	    (declare (type array-index      i nbyte)
		     (type gcmask bit       mask)
		     (type (or null card32) local))
	    
	    (unless (eql (the (or null card32) (svref server-state i))
			 (setq local (the (or null card32) (svref local-state i))))
	      (setf (svref server-state i) local)
	      (card32-put nbyte local)
	      (setq mask (the gcmask (logior mask bit)))
	      (index-incf nbyte 4)))))
      
      ;; Update GContext extensions
      (do ((extension *gcontext-extensions* (cdr extension))
	   (i *gcontext-data-length* (index+ i 1))
	   (local))
	  ((endp extension))
	(unless (eql (svref server-state i)
		     (setq local (svref local-state i)))
	  (setf (svref server-state i) local)
	  (funcall (gcontext-extension-set-function (car extension)) gcontext local)))
      
      ;; Update clipping rectangles
      (multiple-value-bind (local-clip server-clip)
	  (without-interrupts 
	    (values (gcontext-internal-clip local-state)
		    (gcontext-internal-clip server-state)))
	(unless (equalp local-clip server-clip)
	  (setf (gcontext-internal-clip server-state) nil)
	  (unless (null local-clip)
	    (with-buffer-request (display *x-setcliprectangles*)
	      (data (first local-clip))
	      (gcontext gcontext)
	      ;; XXX treat nil correctly
	      (card16 (or (gcontext-internal-clip-x local-state) 0)
		      (or (gcontext-internal-clip-y local-state) 0))
	      ;; XXX this has both int16 and card16 values
	      ((sequence :format int16) (second local-clip)))
	    (setf (gcontext-internal-clip server-state) local-clip))))
      
      ;; Update dashes
      (multiple-value-bind (local-dash server-dash)
	  (without-interrupts 
	    (values (gcontext-internal-dash local-state)
		    (gcontext-internal-dash server-state)))
	(unless (equalp local-dash server-dash)
	  (setf (gcontext-internal-dash server-state) nil)
	  (unless (null local-dash)
	    (with-buffer-request (display *x-setdashes*)
	      (gcontext gcontext)
	      ;; XXX treat nil correctly
	      (card16 (or (gcontext-internal-dash-offset local-state) 0)
		      (length local-dash))
	      ((sequence :format card8) local-dash))
	    (setf (gcontext-internal-dash server-state) local-dash))))))
  gcontext)
