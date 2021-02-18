;;; -*- Mode:Lisp; Package:CLUEI; Base:10; Lowercase:T; Syntax:Common-Lisp -*-


;;;----------------------------------------------------------------------------------+
;;;                                                                                  |
;;;                          TEXAS INSTRUMENTS INCORPORATED                          |
;;;                                   P.O. BOX 149149                                |
;;;                                AUSTIN, TEXAS 78714-9149                          |
;;;                                                                                  |
;;;                Copyright (C)1989-1990 Texas Instruments Incorporated.            |
;;;                                                                                  |
;;; Permission is granted to any individual or institution to use, copy, modify, and |
;;; distribute this software, provided that  this complete copyright and  permission |
;;; notice is maintained, intact, in all copies and supporting documentation.        |
;;;                                                                                  |
;;; Texas Instruments Incorporated provides this software "as is" without express or |
;;; implied warranty.                                                                |
;;;                                                                                  |
;;;----------------------------------------------------------------------------------+

(in-package "CLUEI")


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;;                  Geometry management methods for ROOT contacts             |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defmethod manage-geometry ((parent root) (shell wm-shell) x y width height border-width &key) 
  (declare (type contact shell)
	   (type (or null int16) x y)
	   (type (or null card16) width height border-width)
	   (values success-p x y width height border-width))

  (with-slots ((contact-x x)
	       (contact-y y)
	       (contact-width width)
	       (contact-height height)
	       (contact-border-width border-width)) (the contact shell)
    
    (if (realized-p shell)

	(progn 
	  ;; Wait for any previous :configure-notify events to be handled
	  (process-all-events (contact-display parent))
	  
	  ;; Negotiate top-level geometry with window mgr
	  (with-event-mode (shell '(:configure-notify (throw-action :configure-notify)))      
	    ;; Reconfigure top-level window, if necessary
	    (let (changed-p)
	      (with-state (shell)
		(when (and x (/= x contact-x)
			   (setf (drawable-x shell) x))
		  (setf changed-p t))
		(when (and y (/= y contact-y)
			   (setf (drawable-y shell) y))
		  (setf changed-p t))
		(when (and width (/= width contact-width)
			   (setf (drawable-width shell) width))
		  (setf changed-p t))
		(when (and height (/= height contact-height)
			   (setf (drawable-height shell) height))
		  (setf changed-p t))
		(when (and border-width (/= border-width contact-border-width)
			   (setf (drawable-border-width shell) border-width))
		  (setf changed-p t)))
	      
	      ;; Return approved geometry
	      (values
		(or
		  ;; Null changed approved immediately
		  (not changed-p)
		  
		  ;; Actual change approved if it is not modified by window mgr.
		  (progn
		    ;; Wait for :configure-notify to report actual new window geometry.
		    ;; Top-level shell's handle-event will update geometry slots in
		    ;; response to :configure-notify.	      
		    (catch :configure-notify
		      (loop (process-next-event (contact-display parent))))
		    
		    ;; Assert: shell slots now contain actual (wm-approved) geometry
		    ;; Return approval of original geometry request
		    (and
		      (not (and x (/= x contact-x)))
		      (not (and y (/= y contact-y)))
		      (not (and width (/= width contact-width)))
		      (not (and height (/= height contact-height)))
		      (not (and border-width (/= border-width contact-border-width))))))
		
		contact-x contact-y contact-width contact-height contact-border-width))))

	;; Else approve change to unrealized shell immediately
	(values t
		(or x contact-x) (or y contact-y)
		(or width contact-width) (or height contact-height)
		(or border-width contact-border-width)))))


(defmethod manage-priority ((parent root) (shell wm-shell) priority sibling &key)  
  (declare (type (member :above :below :top-if :bottom-if :opposite) priority)
	   (type (or null contact) sibling)
	   (values success-p priority sibling))

  ;; Wait for any previous :configure-notify events to be handled
  (process-all-events (contact-display parent))
  
  (with-event-mode (shell '(:configure-notify return-above-sibling))
    
    ;; Reconfigure top-level window
    (setf (window-priority shell sibling) priority)
    
    ;; Wait for :configure-notify to report actual new window priority.
    (let ((above-sibling
	    (catch :configure-notify
	      (loop (process-next-event (contact-display parent))))))
      
      ;; Return approval for original priority request
      (values
	(and (eq sibling above-sibling) (eq priority :above))
	:above))))


(defun return-above-sibling (shell)
  (declare (ignore shell))
  (with-event (above-sibling)
    (throw :configure-notify above-sibling)))


(defmethod change-layout ((parent root) &optional newly-managed)
  (declare (ignore newly-managed)) 
  ;; Adding/deleting root children has no effect on sibling layout
  )
