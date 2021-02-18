;;; -*- Mode:Lisp; Package:CLUEI; Base:10; Lowercase:T; Syntax:Common-Lisp -*-

;;;
;;;			 TEXAS INSTRUMENTS INCORPORATED
;;;				  P.O. BOX 149149
;;;			       AUSTIN, TEXAS 78714-9149
;;;
;;; Copyright (C)1989-1990 Texas Instruments Incorporated.
;;;
;;; Permission is granted to any individual or institution to use, copy, modify,
;;; and distribute this software, provided that this complete copyright and
;;; permission notice is maintained, intact, in all copies and supporting
;;; documentation.
;;;
;;; Texas Instruments Incorporated provides this software "as is" without
;;; express or implied warranty.
;;;

(in-package "CLUEI")

(export
  '(
    override-shell
    shell
    shell-mapped
    shell-owner
    shell-unmapped
    sm-client-host
    sm-command
    top-level-session
    top-level-shell
    transient-shell
    with-wm-properties
    with-wm-properties 
    wm-base-height
    wm-base-width
    wm-colormap-owners
    wm-delta-height
    wm-delta-width
    wm-gravity
    wm-group
    wm-icon
    wm-icon-mask
    wm-icon-title
    wm-icon-x
    wm-icon-y
    wm-initial-state
    wm-keyboard-input
    wm-max-aspect
    wm-max-height
    wm-max-width
    wm-message 
    wm-message-protocol
    wm-message-timestamp
    wm-min-aspect
    wm-min-height
    wm-min-width
    wm-protocols-used
    wm-shell    
    wm-title
    wm-user-specified-position-p
    wm-user-specified-size-p
    )
  'cluei)



;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 				      Shell                                    |
;;;                                                                            |
;;;----------------------------------------------------------------------------+


(defcontact shell (composite)
  ((state        :type     (member :withdrawn :iconic :mapped)
		 :accessor contact-state)
   
   (owner        :type     composite
		 :reader   shell-owner))
  (:resources
    (state       :type     (member :withdrawn :iconic :mapped)
		 :initform (shell-default-state instance)))
  
  (:documentation
    "Base class for all shell contacts."))



;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 				 Override Shell                                |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defcontact override-shell (shell)
  ()
  (:resources
    (override-redirect :initform :on))
  (:documentation
    "Base class for shells which override the window manager."))

(defmethod (setf contact-state) :around (new-state (shell override-shell))
  ;; :iconic is equivalent to :withdrawn for override-shell
  (call-next-method
    (if (eq :iconic new-state) :withdrawn new-state)
    shell))


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;;		    Batching window manager property changes                   |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defmacro wm-properties-changed (shell &optional default)
  "Return list of changed window manager properties for the SHELL."
  `(getf (window-plist ,shell) 'wm-properties-changed ,default))

(defmacro wm-changing-properties-p (shell)
  "Return true if currently batching changes to window manager properties of the SHELL."
  `(not (eq :undefined (wm-properties-changed ,shell :undefined))))

(defsetf wm-changing-properties-p (shell) (value)
  "Turn off/on batching of changes to window manager properties of the SHELL."
  `(if ,value
       (setf (wm-properties-changed ,shell) nil)
       (remf (window-plist ,shell) 'wm-properties-changed)))

(defmacro with-wm-properties ((shell) &body body)
  "Batch all changes to window manager properties of the SHELL into one request
after the BODY." 
  `(progn
     (setf (wm-changing-properties-p ,shell) t)
     ,@body
     (when (wm-properties-changed ,shell)
       (apply #'wm-batch-change-properties (wm-properties-changed ,shell)))
     (setf (wm-changing-properties-p ,shell) nil)))


(defmacro define-wm-batch-change-properties ()
  "Generate WM-BATCH-CHANGE-PROPERTIES function definition."
  `(defun wm-batch-change-properties (shell &rest properties)
     "Change the properties which control window manager interaction."
     ,@(let (code)
	 (dolist (p
		   '(
		     ;; class, transient-for properties not included because
		     ;; they should only be changed during initialization
		     
		     client-machine
		     colormap-windows
		     command
		     hints
		     icon-name
		     name
		     normal-hints
		     protocols
		     )
		   code)
	   (let ((accessor (intern (format nil "WM-CHANGE-~a" (symbol-name p)) 'cluei)))
	     (push
	       `(when (member ',p properties :test #'eq)
		  (,accessor shell))
	       code))))))

(define-wm-batch-change-properties)


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;;			      Window Manager Shell                             |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defcontact wm-shell (shell)
  ((hints                :type     (OR null wm-hints)
			 :initform nil 
			 :initarg  :hints
			 :accessor shell-hints)
   
   (normal-hints         :type     (OR null wm-size-hints)
			 :initform nil 
			 :initarg  :normal-hints
			 :accessor shell-normal-hints)
   
   (protocols-used       :type     (OR null list) 
			 :initform nil 
			 :initarg  :protocols-used
			 :accessor wm-protocols-used)
   
   (title                :type     (OR null stringable)
			 :initform nil 
			 :initarg  :wm-title
			 :accessor wm-title))

  (:resources
    (event-mask :initform #.(make-event-mask :structure-notify)))
    
  (:documentation
    "Base class for shells which interact with the window manager."))


(defmethod realize :after ((shell wm-shell)) 
  (wm-change-class        shell)
  (wm-change-hints        shell)
  (wm-change-name         shell)
  (wm-change-normal-hints shell)
  (wm-change-protocols    shell))




;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			     WM_PROTOCOLS accessors                            |
;;;                                                                            |
;;;----------------------------------------------------------------------------+


(defmethod wm-change-protocols ((shell wm-shell))
  "Send a request to change the WM_PROTOCOLS property for the SHELL."
  (when (realized-p shell)
    (with-slots (protocols-used display) shell
      (if protocols-used	  
	  (change-property shell :wm_protocols
			   (map 'list #'(lambda (atom) (intern-atom display atom))
				protocols-used)
			   :atom 32)
	  (delete-property shell :wm_protocols)))))

(defmethod (setf wm-protocols-used) :after (new-protocols (shell wm-shell))
  (declare (ignore new-protocols))
  (if (wm-changing-properties-p shell)
      (adjoin 'protocols (wm-properties-changed shell))
      (wm-change-protocols shell)))



;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			        WM_NAME accessors                              |
;;;                                                                            |
;;;----------------------------------------------------------------------------+


(defmethod wm-change-name ((shell wm-shell))
  "Send a request to change the WM_NAME property for the SHELL."
  (when (realized-p shell)
    (with-slots (title name) shell
      (setf (wm-name shell) (or title name)))))

(defmethod (setf wm-title) :after (new-title (shell wm-shell))
  (declare (ignore new-title))
  (if (wm-changing-properties-p shell)
      (adjoin 'name (wm-properties-changed shell))
      (wm-change-name shell)))


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			       WM_CLASS accessors                              |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

;; This should only be called during initialization
(defmethod wm-change-class ((shell wm-shell))
  "Send a request to change the WM_CLASS property for the SHELL."
  (when (realized-p shell)
    (let ((d (contact-display shell)))
      (set-wm-class shell
		    (display-name d)
		    (display-class d)))))
			    

;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			    WM_NORMAL_HINTS accessors                          |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defmethod (setf shell-normal-hints) :after (new-normal-hints (shell wm-shell))
  (declare (ignore new-normal-hints))
  (wm-change-normal-hints shell))

(defmethod wm-change-normal-hints ((shell wm-shell))
  "Send a request to change the WM_NORMAL_HINTS property for the SHELL."
  (when (realized-p shell)
    (with-slots (normal-hints) shell
      (if normal-hints
	  (setf (wm-normal-hints shell) normal-hints)
	  (delete-property shell :wm_normal_hints)))))

(defun wm-update-normal-hints (shell)
  "Record an update to the WM_NORMAL_HINTS property for the SHELL."
  (if (wm-changing-properties-p shell)
      (adjoin 'normal-hints (wm-properties-changed shell))
      (wm-change-normal-hints shell)))

(defmethod wm-user-specified-size-p ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-user-specified-size-p normal-hints))))

(defmethod (setf wm-user-specified-size-p) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-user-specified-size-p normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-user-specified-position-p ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-user-specified-position-p normal-hints))))

(defmethod (setf wm-user-specified-position-p) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-user-specified-position-p normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-min-width ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-min-width normal-hints))))

(defmethod (setf wm-min-width) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-min-width normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-min-height ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-min-height normal-hints))))

(defmethod (setf wm-min-height) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-min-height normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-min-aspect ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-min-aspect normal-hints))))

(defmethod (setf wm-min-aspect) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-min-aspect normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-max-width ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-max-width normal-hints))))

(defmethod (setf wm-max-width) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-max-width normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-max-height ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-max-height normal-hints))))

(defmethod (setf wm-max-height) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-max-height normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-max-aspect ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-max-aspect normal-hints))))

(defmethod (setf wm-max-aspect) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-max-aspect normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-gravity ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-win-gravity normal-hints))))

(defmethod (setf wm-gravity) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-win-gravity normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-delta-width ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-width-inc normal-hints))))

(defmethod (setf wm-delta-width) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-width-inc normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-delta-height ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-height-inc normal-hints))))

(defmethod (setf wm-delta-height) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-height-inc normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-base-width ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-base-width normal-hints))))

(defmethod (setf wm-base-width) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-base-width normal-hints) value))
  (wm-update-normal-hints shell))

(defmethod wm-base-height ((shell wm-shell))
  (with-slots (normal-hints) shell
    (when normal-hints
      (wm-size-hints-base-height normal-hints))))

(defmethod (setf wm-base-height) (value (shell wm-shell))
  (with-slots (normal-hints) shell
    (unless normal-hints
      (setf normal-hints (make-wm-size-hints)))
    (setf (wm-size-hints-base-height normal-hints) value))
  (wm-update-normal-hints shell))




;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			 WM_HINTS accessors for wm-shell                       |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defmethod (setf shell-hints) :after (new-hints (shell wm-shell))
  (declare (ignore new-hints))
  (wm-change-hints shell))

(defmethod wm-change-hints ((shell wm-shell))
  "Send a request to change the WM_HINTS property for the SHELL."
  (when (realized-p shell)
    (with-slots (hints) shell
      (if hints
	  (setf (wm-hints shell) hints)
	  (delete-property shell :wm_hints)))))

(defun wm-update-hints (shell)
  "Record an update to the WM_HINTS property for the SHELL."
  (if (wm-changing-properties-p shell)
      (adjoin 'hints (wm-properties-changed shell))
      (wm-change-hints shell)))

(defmethod wm-group ((shell wm-shell))
  (with-slots (hints) shell
    (when hints
      (wm-hints-window-group hints))))

(defmethod (setf wm-group) (value (shell wm-shell))
  (with-slots (hints) shell
    (unless hints
      (setf hints (make-wm-hints)))
    (setf (wm-hints-window-group hints) value))
  (wm-update-hints shell))

(defmethod wm-keyboard-input ((shell wm-shell))
  (with-slots (hints) shell
    (when hints
      (wm-hints-input hints))))

(defmethod (setf wm-keyboard-input) (value (shell wm-shell))
  (with-slots (hints) shell
    (unless hints
      (setf hints (make-wm-hints)))
    (setf (wm-hints-input hints) value))
  (wm-update-hints shell))

(defmethod wm-initial-state ((shell wm-shell))
  (with-slots (hints) shell
    (when hints
      (wm-hints-initial-state hints))))

(defmethod (setf wm-initial-state) (value (shell wm-shell))
  (with-slots (hints) shell
    (unless hints
      (setf hints (make-wm-hints)))
    (setf (wm-hints-initial-state hints) value))
  (wm-update-hints shell))



;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			   :client-message translations                        |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defstruct (wm-message (:type (vector int32)))
  "Common data fields of all :client-message events from a window/session mgr." 
  protocol
  timestamp)

(defun wm-message-protocol-atom (wm-message)
  (declare (special *event-display*))
  (atom-name *event-display* (wm-message-protocol wm-message)))

(defevent wm-shell (:wm_take_focus) wm-take-focus)
(defevent wm-shell :focus-in        wm-take-focus)

(defmethod wm-take-focus ((shell wm-shell))
  (with-slots (children) shell
    (when children
      (wm-take-focus (first children)))))

(defmethod wm-take-focus ((composite composite))
  (with-slots (display) composite
    (if (accept-focus-p composite)
	(set-input-focus display composite :parent)
	(move-focus composite :set))))

(defmethod wm-take-focus ((contact contact))
  (with-slots (display) contact
    (when (accept-focus-p contact)
      (set-input-focus display contact :parent))))



;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 				Transient Shells                               |
;;;                                                                            |
;;;----------------------------------------------------------------------------+



(defcontact transient-shell (wm-shell)
  ()  
  (:documentation
    "Base class for shells which are never iconified."))

(defmethod realize :after ((shell transient-shell)) 
  (wm-change-transient-for shell))

;; This should only be called during initialization
(defmethod wm-change-transient-for ((shell transient-shell))
  "Send a request to change the TRANSIENT-FOR property for the SHELL."
  (when (realized-p shell)
    (with-slots (owner) shell
      (setf (transient-for shell) owner))))





;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 				Top-Level Shells                               |
;;;                                                                            |
;;;----------------------------------------------------------------------------+


(defcontact top-level-shell (wm-shell)
  ((colormap-owners      :type     list
			 :initform nil 
			 :initarg  :colormap-owners
			 :accessor wm-colormap-owners)
   
   (icon                 :type     (or null drawable)
			 :initform nil 
			 :initarg  :icon
			 :accessor wm-icon)
   
   (icon-mask            :type     (or null pixmap)
			 :initform nil 
			 :initarg  :icon-mask
			 :accessor wm-icon-mask)
   
   (icon-title           :type     (or null stringable)
			 :initform nil 
			 :initarg  :icon-title
			 :accessor wm-icon-title)
   
   (icon-x               :type     (or null int16)
			 :initform nil 
			 :initarg  :icon-x
			 :accessor wm-icon-x)
   
   (icon-y               :type     (or null int16)
			 :initform nil 
			 :initarg  :icon-y
			 :accessor wm-icon-y))
  
  (:documentation
    "Base class for normal top-level shells."))

(defmethod realize :after ((shell top-level-shell)) 
  (wm-change-colormap-windows shell) 
  (wm-change-icon-name        shell))


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			  WM_COLORMAP_WINDOWS accessors                        |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defmethod (setf wm-colormap-owners) :after (new-colormap-owners (shell top-level-shell))
  (declare (ignore new-colormap-owners))
  (if (wm-changing-properties-p shell)
      (adjoin 'colormap-windows (wm-properties-changed shell))
      (wm-change-colormap-windows shell)))

(defmethod wm-change-colormap-windows ((shell top-level-shell))
  "Send a request to change the COLORMAP-WINDOWS property for the SHELL."
  (when (realized-p shell)
    (with-slots (colormap-owners) shell
      (if colormap-owners
	  (change-property shell :wm_colormap_windows
			   colormap-owners
			   :window
			   32
			   :transform #'window-id)
	  (delete-property shell :wm_colormap_windows)))))


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			     WM_ICON_NAME accessors                            |
;;;                                                                            |
;;;----------------------------------------------------------------------------+


(defmethod (setf wm-icon-title) :after (new-icon-title (shell top-level-shell))
  (declare (ignore new-icon-title))
  (if (wm-changing-properties-p shell)
      (adjoin 'icon-name (wm-properties-changed shell))
      (wm-change-icon-name shell)))

(defmethod wm-change-icon-name ((shell top-level-shell))
  "Send a request to change the WM_ICON_NAME property for the SHELL."
  (when (realized-p shell)
    (with-slots (icon-title) shell
      (if icon-title
	  (setf (wm-icon-name shell) icon-title)
	  (delete-property shell :wm_icon_name)))))


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			 WM_HINTS accessors for top-level-shell                |
;;;                                                                            |
;;;----------------------------------------------------------------------------+


(defmethod wm-icon ((shell top-level-shell))
  (with-slots (hints) shell
    (when hints
      (or (wm-hints-icon-pixmap hints)
	  (wm-hints-icon-window hints)))))

(defmethod (setf wm-icon) ((value pixmap) (shell top-level-shell))
  (with-slots (hints) shell
    (unless hints
      (setf hints (make-wm-hints)))
    (setf (wm-hints-icon-pixmap hints) value
	  (wm-hints-icon-window hints) nil))
  (wm-update-hints shell))

(defmethod (setf wm-icon) ((value window) (shell top-level-shell))
  (with-slots (hints) shell
    (unless hints
      (setf hints (make-wm-hints)))
    (setf (wm-hints-icon-window hints) value
	  (wm-hints-icon-pixmap hints) nil))
  (wm-update-hints shell))

(defmethod wm-icon-mask ((shell top-level-shell))
  (with-slots (hints) shell
    (when hints
      (wm-hints-icon-mask hints))))

(defmethod (setf wm-icon-mask) (value (shell top-level-shell))
  (with-slots (hints) shell
    (unless hints
      (setf hints (make-wm-hints)))
    (setf (wm-hints-icon-mask hints) value))
  (wm-update-hints shell))
 

(defmethod wm-icon-x ((shell top-level-shell))
  (with-slots (hints) shell
    (when hints
      (wm-hints-icon-x hints))))

(defmethod (setf wm-icon-x) (value (shell top-level-shell))
  (with-slots (hints) shell
    (unless hints
      (setf hints (make-wm-hints)))
    (setf (wm-hints-icon-x hints) value))
  (wm-update-hints shell))


(defmethod wm-icon-y ((shell top-level-shell))
  (with-slots (hints) shell
    (when hints
      (wm-hints-icon-y hints))))

(defmethod (setf wm-icon-y) (value (shell top-level-shell))
  (with-slots (hints) shell
    (unless hints
      (setf hints (make-wm-hints)))
    (setf (wm-hints-icon-y hints) value))
  (wm-update-hints shell))




;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			    Top-Level Session Shells                           |
;;;                                                                            |
;;;----------------------------------------------------------------------------+


(defcontact top-level-session (top-level-shell)
  ((client-host          :type     (or null stringable)
			 :initform nil 
			 :initarg  :client-host
			 :accessor sm-client-host)

   (command              :type     (or null string)
			 :initform nil 
			 :initarg  :command
			 :accessor sm-command))  
  (:documentation
    "Base class for top-level shells that communicate with a session manager."))


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			     WM_CLIENT_MACHINE accessors                       |
;;;                                                                            |
;;;----------------------------------------------------------------------------+


(defmethod (setf sm-client-host) :after (new-client-host (shell top-level-session))
  (declare (ignore new-client-host))
  (if (wm-changing-properties-p shell)
      (adjoin 'client-machine (wm-properties-changed shell))
      (wm-change-client-machine  shell)))

(defmethod wm-change-client-machine  ((shell top-level-session))
  "Send a request to change the WM_CLIENT_MACHINE property for the SHELL."
  (when (realized-p shell)
    (with-slots (client-host) shell
      (if client-host
	  (setf (wm-client-machine shell) client-host)
	  (delete-property shell :wm_client_machine)))))


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			     WM_COMMAND accessors                              |
;;;                                                                            |
;;;----------------------------------------------------------------------------+


(defmethod (setf sm-command) :after (new-command (shell top-level-session))
  (declare (ignore new-command))
  (if (wm-changing-properties-p shell)
      (adjoin 'command (wm-properties-changed shell))
      (wm-change-command shell)))

(defmethod wm-change-command ((shell top-level-session))
  "Send a request to change the WM_COMMAND property for the SHELL."
  (when (realized-p shell)
    (with-slots (command) shell
      (if command
	  (setf (wm-command shell) command)
	  (delete-property shell :wm_command)))))



;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			     Shell: State management                           |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defun shell-default-state (shell)
  ;; WARNING: assumes that the parent slot still contains the owner and
  ;; has not yet been reset to the root!

  ;; Is this a root shell?
  (if (eq (contact-parent shell) (contact-root shell))
      :mapped
      :withdrawn))

(defmethod initial-state-transition ((shell shell))
  "Return the old-state/new-state for the initial (setf contact-state) after CONTACT
   is realized. Return nil if (setf contact-state) need not be called, i.e. no
   initial state transition is necessary."
  (with-slots (state) shell
    (unless (eq :withdrawn state)
      (values :withdrawn state))))

(defmethod (setf contact-state) (new-state (shell shell))
  (check-type new-state (member :withdrawn :iconic :mapped))    
  
  (with-slots (parent display state) shell
    (unless (eq state new-state)
      (let ((old-state state))
	
	(setf state new-state)
	
	(if (realized-p shell)
	    ;; Change state now -- but don't send side-effect requests if inside
	    ;; without-requests wrapper (i.e. eq *contact-notified*) ---
	    ;; that is, if responding to notification of state change from window mgr.
	    (case new-state
	      (:mapped    (shell-mapped shell)
			  (unless (eq *contact-notified* shell)
			    (map-window shell)))
	      
	      (:iconic    (if (eq old-state :withdrawn)			     
			      (unless (eq *contact-notified* shell)
				(unless (eq :iconic (wm-initial-state shell))
				  (setf (wm-initial-state shell) :iconic))
				(map-window shell))
			      
			      (progn
				(unless (eq *contact-notified* shell)
				  (send-event parent
					      :client-message
					      #.(make-event-mask)
					      :event-window parent
					      :type :wm_change_state
					      :format 32
					      :data '(3)	;Crock: this should be an xlib defconstant
					      ))	
				(shell-unmapped shell))))
	      
	      (:withdrawn (unless (eq *contact-notified* shell)
			    (unmap-window shell)
			    (send-event parent
					:unmap-notify
					#.(make-event-mask :substructure-redirect :substructure-notify)
					:event-window parent
					:window shell
					:configure-p nil))
			  (shell-unmapped shell)))
	    
	    ;; Not realized, let UPDATE-STATE do the work
	    (setf (display-update-flag display) t)))))
  new-state)

(defmethod shell-mapped ((shell shell))
  (apply-callback shell :map))

(defmethod shell-unmapped ((shell shell))
  (apply-callback shell :unmap))

;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			   Shell: Geometry Management                          |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defmethod add-to-parent ((self shell))
  (with-slots (parent owner) self
    (let ((root (contact-root self)))
      
      ;; Initialize shell owner
      (setf owner parent)
      (with-slots (shells) owner
	(unless (member self shells :test #'eq)
	    (setf shells (nconc shells (cons self nil)))))

      ;; A shell is always a child of its root
      (setf parent root)
      (add-child root self))))

(defmethod contact-resource-parent ((shell shell))
  (slot-value shell 'owner))

(defmethod add-child :before ((shell shell) child &key)  
  (with-slots (children) shell
    (when children
      (error "~s already has child ~s; cannot add child ~s."
	     shell
	     (first children)
	     child))))

(defmethod manage-geometry ((parent shell) child x y width height border-width &key) 
  (declare (type contact child)
	   (type (or null int16) x y)
	   (type (or null card16) width height border-width)
	   (values success-p x y width height border-width))  
  
  (let* ((child-bw     (or border-width (contact-border-width child)))
	 (required-pos (- child-bw)))
    
    (with-slots ((parent-width width) (parent-height height)) parent 
      (multiple-value-bind (size-approved-p approved-width approved-height)
	  
	  (if (and (realized-p parent)
		   (or (setf width  (unless (eq width  (contact-width child))  width))
		       (setf height (unless (eq height (contact-height child)) height))))
	      
	      ;; Request corresponding change in top-level shell size
	      ;; Since shell is top-level, changed size is effected immediately
	      (values
		(change-geometry parent :width width :height height)
		parent-width
		parent-height)
	      
	      ;; Unrealized shell approves and adopts change immediately
	      (values
		t
		(setf parent-width  (or width  (contact-width child)))
		(setf parent-height (or height (contact-height child)))))

	;; Shell child always positioned so that its borders are invisible.
	(values
	  (and size-approved-p
	       (or (null x) (= x required-pos))
	       (or (null y) (= y required-pos)))    
	  required-pos
	  required-pos
	  approved-width   
	  approved-height
	  child-bw)))))


(defmethod manage-priority ((parent shell) child priority sibling &key)  
  (declare (type (member :above :below :top-if :bottom-if :opposite) priority)
	   (type (or null contact) sibling)
	   (values success-p priority sibling))
  (declare (ignore child priority sibling))
  ;; Never approved since shell has only one child
  nil)

(defmethod change-layout ((shell shell) &optional newly-managed)
  (declare (ignore newly-managed))
  (with-slots (children width height) shell
    (when children
      ;; Shell assumes size of its content
      (let* ((content        (first children))
	     (content-width  (contact-width content))
	     (content-height (contact-height content)))
	
	(if (realized-p shell)
	    ;; Negotiate with window mgr
	    (change-geometry shell
			     :width  content-width
			     :height content-height)
	    
	    ;; Else change size of unrealized shell immediately
	    (setf width  content-width
		  height content-height))
	
	;; Position content to hide content border
	(change-geometry content :accept-p t)))))


(defmethod resize :after ((shell shell) width height border-width)
  (declare (ignore border-width)) 
  (with-slots (children) shell
    (let ((content (first children)))
      (when content
	;; Force content to have same size
	(resize content width height
		(contact-border-width content))))))


(defmethod preferred-size ((shell shell) &key width height border-width)
  (declare (ignore border-width))

  (with-slots (children (current-width width) (current-height height)) shell
    (let ((content (first children)))
      (multiple-value-bind (preferred-width preferred-height)	
	  (if content
	      ;; Return preferred size of content
	      (preferred-size content :width width :height height)

	      ;; Else accept suggested or current size
	      (values (or width current-width) (or height current-height)))
	
	(values preferred-width preferred-height 0)))))


;;;----------------------------------------------------------------------------+
;;;                                                                            |
;;; 			      Shell: Event Handling                            |
;;;                                                                            |
;;;----------------------------------------------------------------------------+

(defmethod handle-event :before ((shell shell) (event event))
  (with-slots (key) event
    (case key
      (:configure-notify
       ;; Update geometry slots to reflect reality
       (with-slots ((new-x            x)
		    (new-y            y)
		    (new-width        width)
		    (new-height       height)
		    (new-border-width border-width)) event
	 (with-slots (x y width height border-width) shell	   
	   (without-requests shell		;no configure request side-effect
	     ;; Use move/resize protocol so that any auxilliary methods will fire
	     ;; Call only if geometry actually changed 
	     (unless (and (= x new-x) (= y new-y))
	       (move shell new-x new-y))

	     ;; Note: The only effect of a new border-width would be on the layout
	     ;; of siblings, but for top-level contact, this is handled by window manager.
	     ;; Therefore, consider only width/height when propagating new size.
	     (unless (and (= width new-width) (= height new-height))
	       (resize shell new-width new-height new-border-width))))))
      
      (:map-notify
       ;; Update state to reflect reality
       (without-requests shell
	 (setf (contact-state shell) :mapped)))
      
      (:unmap-notify
       ;; Update state to reflect reality
       (without-requests shell
	 (setf (contact-state shell) :iconic))))))
