;;; -*- Mode:Lisp; Package:CLUEI; Syntax:COMMON-LISP; Base:10; Lowercase:T -*-

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


(in-package "CLUEI")

(export '(display-root ;; Setf'able
	  display-root-list
	  display-multipress-delay-limit
	  display-multipress-verify-p
	  *default-host*
	  *default-display*
	  *default-multipress-delay-limit*
	  *default-multipress-verify-p* 	  

	  *parent*				; Bound during contact initialization
	  open-contact-display
	  
	  basic-contact
	  contact
	  make-contact
	  ;; Contact slots:
	  display
	  parent
	  complete-name
	  complete-class
	  callbacks
	  event-translations
	  state
	  sensitive
	  compress-motion
	  compress-exposures
	  x y width height
	  border-width
	  background
	  depth
	  event-mask
	  id
	  plist
	  
	  ;; Contact slot accessors:
	  contact-background
	  contact-border-width
	  contact-callbacks
	  contact-compress-exposures
	  contact-compress-motion
	  contact-depth
	  contact-display
	  contact-event-mask
	  contact-height
	  contact-parent
	  contact-sensitive
	  contact-state
	  contact-width
	  contact-x
	  contact-y
	  
	  composite
	  children focus shells
	  composite-children
	  composite-focus
	  composite-shells
	  
	  destroy
	  
	  contact-complete-name
	  contact-complete-class
	  contact-name
	  display-name
	  display-class
;;	  find-contact
	  ancestor-p
	  realized-p
	  destroyed-p
	  mapped-p
	  top-level-p
	  managed-p
	  sensitive-p
	  event-mask  ;; Setf'able
	  resource
	  update-state
	  initialize-geometry 
	  present
	  dismiss
	  realize
	  display

	  add-callback
	  apply-callback
	  callback-p
	  delete-callback
	  *contact*
	  
	  root
	  contact-root
	  contact-screen

	  read-character
	  unread-character
	  listen-character
	  append-characters
	  clear-characters
	  
	  add-child
	  delete-child
	  previous-sibling
	  next-sibling
	  change-priority
	  manage-priority
	  accept-focus-p
	  move-focus
	  change-layout
	  change-geometry
	  preferred-size
	  move
	  resize
	  manage-geometry

	  
	  spring-loaded
	  shadow-width

	  contact-constraints
	  contact-constraint
	  class-constraints))

(pushnew :CLUE *features*)

;;;-----------------------------------------------------------------------------
;;; Extend the xlib:display object for CLUE slots

(defmacro display-root-list (display)
  "Returns a list of root contacts in the order given by xlib:open-display."
  `(the list (getf (display-plist ,display) 'root-list)))

(defun display-root (display &optional number)
  "Returns the root of the display specified by the screen NUMBER."
  (if number
      (nth number (display-root-list display))
    (getf (display-plist display) 'default-root)))

(defsetf display-root (display) (screen)
  `(setf (getf (display-plist ,display) 'default-root) ,screen))

(defmacro before-actions (display)
  "Returns the alist of functions to call before event processing with arguments."
  `(the list (getf (display-plist ,display) 'event-before-handlers)))

(defmacro timer-queue (display)
  "Returns the list of display timer structures."
  `(the list (getf (display-plist ,display) 'timer-queue)))

(defmacro display-keyboard-buffer (display)
  "Returns the buffer used for keyboard input by all stream contacts on DISPLAY."
  `(getf (display-plist ,display) 'keyboard-buffer))

(defmacro display-modifier-translate (display)
  "Returns the translations used for keyboard input by all stream contacts in DISPLAY."
  `(getf (display-plist ,display) 'modifier-translate))

(defmacro display-update-flag (display)
  "Returns the flag used to indicate when update-state has work to do."
  `(getf (display-plist ,display) 'update-flag))

(defun display-mode-stack (display)
  "Returns the mode-stack of the DISPLAY. The current input mode of a contact-display 
is given by its mode-stack. The mode-stack is an alist containing entries of the form 
(contact mode-type restrict-action . args)."
  (getf (display-plist display) 'mode-stack))

(defsetf display-mode-stack (display) (stack)
  `(setf (getf (display-plist ,display) 'mode-stack) ,stack))

(defun display-multipress-delay-limit (display)
  "Reject a multipress that occurs more than this many milliseconds after initial press event."
  (getf (display-plist display) 'multipress-delay-limit))

(defsetf display-multipress-delay-limit (display) (msec)
  `(setf (getf (display-plist ,display) 'multipress-delay-limit) ,msec))

(defun display-multipress-verify-p (display)
  "When true, verify timeout of multipress events by requesting a timestamp."
  (getf (display-plist display) 'multipress-verify-p))

(defsetf display-multipress-verify-p (display) (flag)
  `(setf (getf (display-plist ,display) 'multipress-verify-p) ,flag))

(defun display-name (display)
  "Returns the application resource name associated with the display."
  (getf (display-plist display) 'resource-name))

(defsetf display-name (display) (name)
  `(setf (getf (display-plist ,display) 'resource-name) ,name))

(defun display-class (display)
  "Returns the application resource class associated with the display."
  (getf (display-plist display) 'resource-class))

(defsetf display-class (display) (class)
  `(setf (getf (display-plist ,display) 'resource-class) ,class))


;;;-----------------------------------------------------------------------------
;;; CLUE applications call OPEN-CONTACT-DISPLAY to connect to an X server.
;;; The object returned by OPEN-CONTACT-DISPLAY is a CLX DISPLAY object that also contains
;;; the before and after event-handler lists, and the application keyboard buffer
;;;-----------------------------------------------------------------------------

(defvar *default-host* nil)
(defvar *default-display* 0)

(defvar *default-multipress-delay-limit* 250
  "Default value for display-multipress-delay-limit.")

(defvar *default-multipress-verify-p* t
  "Default value for display-multipress-verify-p.")

(defun open-contact-display (application-name
			     &key authorization-data authorization-name
			          before-actions class (default-screen 0)
				  display host protocol (root-class 'root))
  "Create and open a new contact-display."
  (declare (type stringable application-name host)
	   (type (or null integer) display)
	   (type (or null (integer 0)) default-screen)
	   (values contact-display))
  (declare (ignore protocol));; not included because of CLX bugs
  
  (unless *default-host* ;; Set default if none defined
    (setq *default-host* host))
  (let ((disp (open-display (or host *default-host*)
		 :display (or display *default-display*)
;;		 :protocol protocol ;; not included because of CLX bugs
		 :authorization-name authorization-name
		 :authorization-data authorization-data))
	(display-class (or class application-name)))

    ;; Initialize resource name and class
    (setf (display-name disp)  application-name
	  (display-class disp) display-class)
    
    ;; Create a root contact for each screen of the display
    (let ((i 0)
	  roots)
      (dolist (screen (display-roots disp))
	(let ((name (intern (format nil "SCREEN-~d" i) 'keyword)))	  
	  (push (make-contact
		  root-class
		  :display disp
		  :screen screen
		  :parent nil
		  :name name
		  :complete-name  (list application-name name)
		  :complete-class (list display-class root-class))
		roots))
	(incf i))

      ;; Initialize root list and default root
      (setf (display-root-list disp) (nreverse roots)
	    (display-root disp)      (nth default-screen (display-root-list disp))))        
    
    ;; Function to call BEFORE event handling
    (setf (before-actions disp) before-actions)
    
    ;; List of characters from the keyboard
    (setf (display-keyboard-buffer disp) nil)

    ;; Initialize multipress controls
    (setf (display-multipress-delay-limit disp) *default-multipress-delay-limit*
	  (display-multipress-verify-p disp)    *default-multipress-verify-p*)
    
    disp))


;;;-----------------------------------------------------------------------------
;;; Basic CONTACT class

(defcontact basic-contact (window)
  ((display            :initarg :display
		       :reader contact-display)
   
   (parent             :initarg :parent
		       :reader contact-parent)
   
   (name               :type symbol
		       :initarg :name
		       :initform :unnamed
		       :reader contact-name)
   
   (callbacks          :type list
		       :reader contact-callbacks
		       :initform nil)
   
   (event-translations :type list
		       :initform nil)
   
   (event-mask         :type (or null event-mask) ;; Converted to mask32 after realization. 
		       :initform #.(make-event-mask :exposure)
		       :accessor contact-event-mask)
   
   (state              :initform :mapped
		       :type (member :withdrawn :managed :mapped)
		       :accessor contact-state)
   
   (sensitive          :initform :on
		       :type (member :off :on)
		       :accessor contact-sensitive)
   
   (x                  :type int16
		       :initform 0
		       :reader contact-x)
   
   (y                  :type int16
		       :initform 0
		       :reader contact-y)
   
   (width              :type card16
		       :initform 0
		       :reader contact-width)
   
   (height             :type card16
		       :initform 0
		       :reader contact-height)
   
   (border-width       :type card16
		       :initform 1
		       :reader contact-border-width)
   
   ;; Class allocated slots
   (compress-motion    :initform :on :type (member :off :on)
		       :reader contact-compress-motion
		       :allocation :class)
   
   (compress-exposures :initform :off :type (member :off :on)
		       :reader contact-compress-exposures
		       :allocation :class
		       ))
  (:documentation "Basic contact using parent's window")
  (:resources
    (screen :type (or null card8))			;Selects screen when parent is a display
    ;; Slots
    name
    callbacks
    event-translations
    event-mask
    state
    sensitive
    x y width height border-width
    ))

(defcontact contact (basic-contact)
  ((background :type (or (member :none :parent-relative) pixel pixmap)
	       :initform :parent-relative :accessor contact-background)
   (depth :type card16 :initform 0 :reader contact-depth)
   (initialization :type (or (member :destroy) list))	; Internal slot for window initialization and destruction
   )
  (:documentation "Basic contact")
  (:resources
    (documentation :type (or list string))

    ;; Slots
    (background :type (or (member :none :parent-relative) pixel pixmap))
    (depth :type card16)
    
    ;; Window attributes for create-window
    (backing-store :type (or null (member :not-useful :when-mapped :always)))
    (border :type (or null (member :copy) pixel pixmap))
    (cursor :type (or null (member :none) cursor))
    (override-redirect :type (or null (member :on :off)))
    (save-under :type (or null (member :on :off)))

    ;; These window attributes are NOT defined as resources, because it's not worth
    ;; the cost in initialization time.
    ;;    (backing-pixel :type (or null pixel))
    ;;    (backing-planes :type (or null pixel))
    ;;    (bit-gravity :type (or null bit-gravity))
    ;;    (class :type (member :copy :input-output :input-only) :initform :copy)
    ;;    (colormap :type (or null (member :copy) colormap))
    ;;    (do-not-propagate-mask :type (or null device-event-mask))
    ;;    (gravity :type (or null win-gravity))
    ;;    (visual :type (or (member :copy) card29) :initform :copy)
    )
  (:documentation "The base class for all interactive objects in CLUE."))

(defcontact composite (contact)
  ((children :initform nil
	     :type list
	     :reader composite-children)
   (focus    :initform nil
	     :type (or null contact)
	     :reader composite-focus)
   (shells   :type list
	     :initform nil
	     :reader composite-shells))
  (:resources
    (event-mask :type (or null event-mask) :initform #.(make-event-mask))
    (focus-name :type symbol))
  (:documentation "A basic CLUE contact with children"))




;;;-----------------------------------------------------------------------------
;;; UTILITY FUNCTIONS

(defmethod print-object ((instance contact) stream)
  (let ((name (if (slot-boundp instance 'name)
		  (contact-name instance)
		  :uninitialized)))
    #+lispm
    (si:printing-random-object (instance stream)
      (princ (class-name-of instance) stream)
      (write-char #\space stream)
      (princ name stream))
    #-lispm
    (progn
      (write-string "#<" stream)
      (princ (class-name-of instance) stream)
      (write-char #\space stream)
      (princ name stream)
      (write-char #\> stream))))

(defun contact-complete-name (contact &optional nconc-name)
  ;; Return the complete name for contact
  ;; when present, nconc-name is put at the END of the name list.
  ;; This speeds getting the complete name of a contact given its parent and name.
  (let ((result (if nconc-name
		    (list (contact-name contact) nconc-name)
		    (list (contact-name contact)))))
    
    ;; Prepend names up to contact root
    (do ((parent (contact-resource-parent contact) (contact-resource-parent parent)))
	((null parent))
      (push (contact-name parent) result))
    
    ;; Prepend application name
    (push (display-name (contact-display contact)) result)
    result))

(defun contact-complete-class (contact &optional nconc-class)
  ;; Return the complete class for contact
  ;; when present, nconc-class is put at the END of the class list.
  ;; This speeds getting the complete class of a contact given its parent and class.
  (let ((result (if nconc-class
		    (list (class-name-of contact) nconc-class)
		    (list (class-name-of contact)))))
    
    ;; Prepend classes up to contact root
    (do ((parent (contact-resource-parent contact) (contact-resource-parent parent)))
	((null parent))
      (push (class-name-of parent) result))
    
    ;; Prepend application class
    (push (display-class (contact-display contact)) result)
    result))


(defmethod contact-resource-parent ((contact contact))
  (slot-value contact 'parent))

(defun find-contact (parent &key name class)
  "Return the contact in PARENT with NAME and CLASS.
 If name or class is NIL, it is ignored."
  (declare (type (or composite display) parent)
	   (type symbol name class)
	   (values (or null contact)))
  (labels ((search (list name class)
	     (dolist (contact list)
	       (when (and (or (null name) (eq name (contact-name contact)))
			  (or (null class) (eq class (class-name-of contact))))
		 (return-from search contact)))
	     (dolist (contact list)
	       (let* ((children (composite-children contact))
		      (result (and children (search children name class))))
		 (when result (return-from search result))))))
    (etypecase parent
      (display   (search (display-root-list parent) name class))
      (composite (search (composite-children parent) name class)))))

(defun ancestor-p (child parent)
  "Returns T when CHILD is a descendant of PARENT"
  (do ((p (contact-parent child) (contact-parent p)))
      ((null p))
    (when (eq p parent) (return t))))

(defun realized-p (contact)
  "Returns T when contact's window is created and not destroyed"
  (plusp (window-id contact)))

(defun destroyed-p (contact)
  "Returns true when contact's window is (being) destroyed."
  (getf (window-plist contact) :destroyed-p))

(defsetf destroyed-p (contact) (boolean)
  `(setf (getf (window-plist ,contact) :destroyed-p) ,boolean))

(proclaim '(inline managed-p))
(defun managed-p (contact)
  "Returns non-nil when contact is geometry managed by its parent"
  (NOT (EQ (contact-state contact) :withdrawn)))

(defun mapped-p (contact)
  "Returns non-nil when contact is mapped"
  (eq (contact-state contact) :mapped))

(defun visible-p (contact)
  "Returns T when contact is visible (fully or partially)"
  (and (realized-p contact)
       (mapped-p contact)
       t) ;; Put in fancy visibilty testing if deemed necessary
  )

(defun top-level-p (contact)
  "Returns T when CONTACT is a top-level window
 (i.e. under control of a window manager)"
  (and (contact-parent contact) ;; Not a root
       (null (contact-parent (contact-parent contact)))))

(defmethod (setf contact-sensitive) (value (self contact))
  ;; Redisplay when changing sensitive
  (declare (type (member :off :on) value))
  (check-type value (member :off :on))
  (with-slots ((contact-sensitive sensitive)) self
    (let ((old contact-sensitive))
      (setf contact-sensitive value)
      (when (and (not (eq old value))
		 (visible-p self))
	(with-slots (x y width height) self
	  (display self x y width height))))))

(defun sensitive-p (contact)
  "Returns T when a contact and all its ancestors are sensitive
   If there's a mode-stack, the contact, or one of its ancestors,
   must be in the current mode."
  (declare (inline sensitive-p))
  (do ((p contact (contact-parent p)))
      ((null p) t)
    (when (eq (slot-value (the contact p) 'sensitive) :off) (return nil))))

(defmethod (setf contact-event-mask) (mask (contact contact))
  (when (realized-p contact)
    (setf (window-event-mask contact) mask))
  (setf (slot-value (the contact contact) 'event-mask)
	(xlib::encode-event-mask mask)))

(defmethod (setf contact-background) (background (contact contact))
  (declare (type contact contact)
	   (type (or (member :none :parent-relative) pixel pixmap) background))
  (when (realized-p contact)
    (setf (window-background contact) background))
  (setf (slot-value (the contact contact) 'background) background))


;;;-----------------------------------------------------------------------------
;;; CONSTRAINT RESOURCES


(defmacro contact-constraints (contact)
  "Return the list of constraint resource values for the CONTACT."
  `(getf (window-plist ,contact) 'constraints))


(defmacro contact-constraint (contact name)
  "Return the value of the constraint resource NAME for the CONTACT."
  `(getf (contact-constraints ,contact) (intern (symbol-name ,name) 'keyword)))


(defun class-constraints (class &optional full-p)
  "Return the constraint resource specification list for the given CLASS.
If FULL-P is true, then the full list is returned; otherwise, a list of names is returned."
  (let ((full-list (clue-constraints class)))
    (if full-p
	full-list
	(mapcar #'first full-list))))


;;;-----------------------------------------------------------------------------
;;; Contact creation


(defun make-contact (class-name &rest options)
  "Make a contact of type CLASS-NAME, initializing with OPTIONS or from the resource database.
   Every contact must have a :PARENT."
  (apply #'make-instance class-name
	 :allow-other-keys t	;; temporary until we find a better fix
	 (default-options class-name options)))

(defmethod default-options ((class-name t) options)
  (declare (ignore options))
  ;; An (eql class-name) method should be defined by defcontact.
  (error "~s isn't the name of a contact subclass" class-name))


(defun get-contact-resource-table (class-name parent initargs)
  ;; Get the resource database table
  ;;
  ;; Note: This is called with a null parent when class-name is ROOT, in which case
  ;;       this will lose unless INITARGS contains :complete-name and :complete-class.
  (declare (special *database*))
  (get-search-table
      *database*
      (or (getf initargs :complete-name)
	  (contact-complete-name parent (or (getf initargs :name)
					    class-name)))
      (or (getf initargs :complete-class)
	  (contact-complete-class parent class-name))))

(defmethod initialize-instance :after ((self basic-contact)
				       &rest initargs
				       &key resource-table defaults
				       &allow-other-keys)  
  (with-slots ((contact-name name)	       
	       (contact-disp display)
	       (contact-parent parent)
	       (contact-event-translations event-translations)
	       (contact-event-mask event-mask)) self

    ;; Complete slot resource initialization
    (initialize-resource-slots self resource-table defaults)

    ;; Initialize constraint resources
    (when contact-parent
      (setf (contact-constraints self)
	    (initialize-constraints contact-parent initargs resource-table)))
      
    ;; Initialize name to class name by default
    (when (eq contact-name :unnamed)
      (setf contact-name (class-name-of self)))
    
    ;; Parse event-translations
    (setf contact-event-translations
	  (mapcar #'parse-event-spec contact-event-translations)
	  contact-event-mask
	  (xlib::encode-event-mask contact-event-mask))

    ;; Add to composition hierarchy
    (when contact-parent ; root contact's don't have a parent
      (setf contact-disp (contact-display contact-parent))
      (add-to-parent self))))



(defmethod initialize-instance :after ((self contact) &rest initargs)
  (declare (type list initargs))
  
  (setf (display-update-flag (contact-display self)) t)

  (with-slots (initialization border-width) self
    ;; Validate initargs for window class
    (assert
      (or (not (eq :input-only (getf initargs :class)))
	  (zerop border-width))
      () "An :input-only contact must have border-width 0.")

    ;; Save initargs
    (let ((options (copy-list initargs)))
      ;; Allow resource-table to be GC'd
      (remf options :resource-table)     
      (setf initialization options))))


;;;-----------------------------------------------------------------------------
;;; CALLBACKS

(defvar *contact* nil "Bound to the contact invoking an action.")

(proclaim '(inline callback-p))
(defun callback-p (contact callback-name)
  (cdr (assoc callback-name (slot-value contact 'callbacks) :test #'eq)))

(defun function-equal-p (f g)
  (eq (if (symbolp f) (symbol-function f) f)
      (if (symbolp g) (symbol-function g) g)))

(defun add-callback (contact name function &rest args)
  "Associate CONTACT callback NAME with the given FUNCTION and ARGS."
  (with-slots (callbacks) contact
    (let ((functions    (assoc name callbacks :test #'eq))
	  (new-function (list* function (copy-list args))))
      (if functions
	  ;; Append behind any previous functions for this callback
	  (rplacd functions (nconc (delete function (rest functions)
				     :test #'function-equal-p
				     :key #'first
				     :count 1)
				   (list new-function)))

	  ;; Else add first callback function
	  (push (list name new-function) callbacks))
      name)))

(defun delete-callback (contact name &optional function)
  "Disassociate the given FUNCTION and its args from the CONTACT callback NAME.
   If no FUNCTION is given, then all callback functions are deleted."
  (with-slots (callbacks) contact
    (let ((functions (assoc name callbacks :test #'eq)))
      (when functions
	(let ((new-functions (when function
			       (delete function (rest functions)
				       :test #'function-equal-p
				       :key #'first
				       :count 1))))
	  (if new-functions
	      (rplacd functions new-functions)
	      (setf callbacks (delete name callbacks
				      :test #'eq
				      :key #'first)))))))
  name)


(defmacro apply-callback (contact name &rest args)
  "Invoke callback functions associated with NAME for CONTACT,
   using ARGS followed by the callback arguments."
  (let ((functions (gensym))
	(instance  (gensym)))
    `(let* ((,instance  ,contact)
	    (,functions (callback-p ,instance ,name)))
       
	(when ,functions
	  (catch :abort-callback
	    (do* ((functions ,functions         (rest functions))
		  (function  (first ,functions) (first functions)))
		 
		 ((null (rest functions))
		  ;; Return value(s) of last callback function
		  (apply (first function) ,@args (rest function)))
	      
	      (apply (first function) ,@args (rest function))))))))



;;;-----------------------------------------------------------------------------
;;; Basic contact methods

(defmethod add-to-parent ((self basic-contact))
  (add-child (contact-parent self) self))

(defmethod (setf contact-parent) (new-parent (contact contact) &key x y)
  (let ((c (or (when (destroyed-p contact) contact)
	       (when (destroyed-p new-parent) new-parent))))
    (when c
      (error "~s is being destroyed." c)))
  
  (with-slots (parent) contact
    
    ;; Forestall any MATCH errors from reparent-window
    (unless (eq (contact-screen new-parent) (contact-screen parent)) 
      (error "New parent screen (~s) must be the same as old parent screen (~s)."
	     (contact-screen new-parent) (contact-screen parent)))
    (when (eq new-parent contact) 
      (error "Cannot reparent ~s to itself." contact))
    (when (ancestor-p new-parent contact) 
      (error "New parent ~s is already a descendant of ~s." new-parent contact))
    (when (and (eq (contact-background contact) :parent-relative)
	       (/= (contact-depth contact) (contact-depth new-parent))) 
      (error "New parent depth (~s) must be the same as contact depth (~s)."
	     (contact-depth new-parent) (contact-depth contact)))
    
    (let ((actual-state (contact-state contact))
	  (new-x        (or x (contact-x contact)))
	  (new-y        (or y (contact-y contact))))
      
      ;; Unmap and unmanage until reparented
      (setf (contact-state contact) :withdrawn)
      
      ;; Tell server to reparent window
      (reparent-window contact new-parent new-x new-y)
      
      ;; Update contact hierarchy
      (delete-child parent contact)
      (setf parent new-parent)
      (add-child new-parent contact)
      
      ;;Restore state
      (setf (contact-state contact) actual-state))))


(defmethod (setf contact-state) (state (contact contact))
  (declare (special *all-children-mapped-p*))
  
  (check-type state (member :withdrawn :managed :mapped))
  
  (let ((old-state (slot-value (the contact contact) 'state)))
    (unless (eq old-state state)
      (setf (slot-value (the contact contact) 'state) state)
      (if (realized-p contact)
	  ;; When realized, change state immediately
	  (progn 
	    (when (or (eq old-state :withdrawn)
		      (eq state     :withdrawn))
	      ;; Let parent react to transition to/from g.mgmt.
	      (change-layout (contact-parent contact) contact))
	    
	    (if (eq state :mapped)
		
		;; Was unmapped, now mapped
		(unless (boundp '*all-children-mapped-p*)
		  (map-window contact))
		
		(when (eq old-state :mapped)
		  ;; Was mapped, now unmapped
		  (unmap-window contact))))
	  
	  ;; Not realized, let UPDATE-STATE do the work
	  (setf (display-update-flag (contact-display contact)) t))))
  state)



;; Compatibility hack - remove soon
(defun present (contact) (setf (contact-state contact) :mapped))

;; Compatibility hack - remove soon
(defun dismiss (contact &optional (unmanage-p t))
  (if unmanage-p 
      (setf (contact-state contact) :withdrawn)
    (setf (contact-state contact) :managed)))
   
(defun update-state (display)
  (when (display-update-flag display)
    (dolist (root (display-root-list display))
      (update-tree root))
    (setf (display-update-flag display) nil)))

(defmethod update-tree ((composite composite))
  ;; Search for a composite with an unrealized child and update it.
  (let ((children (composite-children composite)))
    (if (dolist (child children)
	  (when (and (not (realized-p child)) (managed-p child))
	    (return t)))
	
	(progn
	  (initialize-geometry composite)
	  (dolist (child children)
	    (when (and (not (realized-p child)) (managed-p child))
	      (realize child)
	      (realize-state child))))
	
	;; No unrealized children here, continue the search lower down
	(dolist (child children)
	  (when (realized-p child)
	    (update-tree child))))))

(defmethod update-tree ((contact contact))
  ;; Do nothing
  )


(defmethod display ((contact basic-contact) &optional x y width height &key)
  "Display self on server"
  ;; This function needs to be over-ridden by the subclasses
  (declare (ignore x y width height))
  contact ;; not used
  )


;;;-----------------------------------------------------------------------------
;;; REALIZE - create the X window associated with a contact

(defmethod realize ((contact contact))
  "Create the window for CONTACT. This function should not be called
   by application programs."
  
  (with-slots
    (    
     background border-width depth event-mask
     height initialization parent width x y
     )
    contact

    ;; Ensure the parent is realized
    (assert
      (realized-p parent) ()
      "Parent of ~s is not realized." contact)

    ;; Ensure width/height initialized
    (assert
      (and (plusp width) (plusp height)) ()
      "Width and height have not been initialized for ~s" contact)

    ;; Calculate event-mask
    (setf event-mask (logior event-mask (compute-contact-event-mask contact)))

    (let ((input-only-p (eq :input-only (getf initialization :class))))
      ;; Create the contact window
      (apply
	#'xlib:create-window
	:window           contact
	:parent           parent
	:x                x
	:y                y
	:width            width
	:height           height
	:border-width     border-width
	:event-mask       event-mask
	:background       (unless input-only-p background)
	:depth            depth
	:allow-other-keys t
	initialization)
      
      
      ;; Record depth, if inherited from parent
      (unless (or (plusp depth) input-only-p)
	(setf depth (contact-depth parent))))

    (let* ((documentation (getf initialization :documentation)))
      (when documentation
	(setf (window-documentation contact) documentation)))

;; Keep initialiation around for awhile, it's useful for debugging
;;    (setf initialization nil) ;; Give initialization list to the garbage collector
    ))

(defmethod realize :after ((contact composite))
  ;; Default focus from the :focus-name initialization
  (with-slots (initialization focus) contact 
    (let ((focus-name (getf initialization :focus-name)))
      (when (and focus-name (not focus))
	(setf focus (find-contact contact :name focus-name)))))
  
  ;; Map children here, to ensure the composite is mapped AFTER its children
  ;; This eliminates the screen flash that would happen if children were
  ;; mapped on top of a visible parent.
  (let* ((children (composite-children contact))
	 (*all-children-mapped-p*
	   (dolist (child children t)
	     (unless (mapped-p child)
	       (return nil)))))
    (declare (special *all-children-mapped-p*))
    
    ;; Recursively realize all managed children of COMPOSITE
    ;; Note: by definition, all children are unrealized
    (dolist (child children)
      (when (managed-p child)
	(realize child)
	(realize-state child)))
    
    ;; Map all children at once, if possible
    (when *all-children-mapped-p*
      (map-subwindows contact))))


(defmethod initialize-geometry ((composite composite))
  ;; Negotiate initial managed geometry from the bottom up
  (declare (type composite composite))
  (let (unrealized-child-exists-p)

    ;; Recursively descend to initialize-geometry for all unrealized managed children
    (dolist (child (composite-children composite))            
      (when (and (not (realized-p child)) (managed-p child))
	(setq unrealized-child-exists-p t)
	(initialize-geometry child)))

    ;; Optimization: don't bother to change layout unless necessary
    (when unrealized-child-exists-p
      (change-layout composite))))

(defmethod initialize-geometry ((contact contact))
  ;; Do nuthin'
  )

(defun realize-state (contact)
  "Make the initial contact-state of a newly-realized CONTACT effective."
  (multiple-value-bind (old-state new-state) (initial-state-transition contact)
    (when old-state
      ;; Problem:  This is a special case because the value of state slot after
      ;;           initialization is not yet in effect and doesn't reflect reality.
      ;; Solution: Temporarily set initial value of state slot to reality (i.e. old-state)     
      ;;           so that (setf contact-state) will take effect correctly.
      (setf (slot-value (the contact contact) 'state) old-state)
      (setf (contact-state contact) new-state))))

(defmethod initial-state-transition ((contact basic-contact))
  "Return the old-state/new-state for the initial (setf contact-state) after CONTACT
   is realized. Return nil if (setf contact-state) need not be called, i.e. no
   initial state transition is necessary."
  (with-slots (state) contact
    (when (eq :mapped state)
      (values :managed :mapped))))

;;;-----------------------------------------------------------------------------
;;; Contact DESTRUCTION

;; Helper function
(defun map-over-children (contact function &rest args)
  ;; Apply FUNCTION first to contact's children, then to contact.
  (when (typep contact 'composite)
    (dolist (child (composite-children contact))
      (apply #'map-over-children child function args)))
  (apply function contact args))

(defmethod destroy ((contact contact))
  "Destroy a contact and all its resources"
  (when (and (not (destroyed-p contact))	; only destroy windows once
	     (contact-parent contact))		; Don't destroy root
    (when (realized-p contact)
      ;; Turn ON structure-notify to receive destroy-notify events
      (setf (window-event-mask contact) #.(make-event-mask :structure-notify))
      ;; Unmap and Destroy the contact's and children's windows.
      (destroy-window contact))
    ;; Destroy resources
    (map-over-children contact #'destroy-cleanup)
    ;; unmanage the contact
    (setf (contact-state contact) :withdrawn)
    ;; Delete contact from its parent's child list
    (delete-child (contact-parent contact) contact)
    ))

(defun destroy-cleanup (contact)
  "Deallocate contact resources (gcontexts, fonts, pixmaps etc.)."
  
  ;; Mark contact destroyed
  (setf (destroyed-p contact) t)
  
  ;; Remove contact's timers
  (delete-timer contact)
  
  ;; Destroy the contact's Gcontext when necessary
  (dolist (gcontext (getf (window-plist contact) 'gcontext-cache))
    (free-gcontext gcontext)
     ;; Debug hack to catch errors
    #+(and ti (not clos)) (setf (si:array-leader gcontext 1) 'destroyed-gcontext))
  
  ;; Ensure modes are popped
  (delete-mode contact)
  
  ;; Destroy a composite's shells
  (when (typep contact 'composite)
    (dolist (shell (slot-value (the composite contact) 'shells))
      (destroy shell)))

  ;; Invoke any :destroy callback
  (apply-callback contact :destroy))

(defun destroy-finish (contact)
  ;; Called from destroy-notify event processing to remove
  ;; contact and its descendents from the resource-id hash-table.
  (map-over-children
    contact
    #'(lambda (contact)
	(xlib::deallocate-resource-id (window-display contact) (window-id contact) 'window)
	#+(and ti (not clos))
	(setf (si:array-leader contact 1) 'destroyed-contact) ;; Debug hack to catch errors
	)))

;;;-----------------------------------------------------------------------------
;;; ROOT CONTACT
;;;
;;; For each screen of the display there's a root contact.
;;; The root contact is used as the root parent contact for all the contacts
;;; on a screen

(defcontact root (composite)
  ((screen       :type     screen
		 :initarg  :screen)
   
   (pixmap-cache :type     list
		 :initform nil)
   
   (cursor-cache :type     list
		 :initform nil)
   
   (x            :initform 0)
   (y            :initform 0)
   (width        :initform 0)			; actual value filled in by initialize-instance
   (height       :initform 0)			; actual value filled in by initialize-instance
   (border-width :initform 0)
   (depth        :initform 0)
   (background   :initform :none))
  
  (:resources
    ;; Remove all inherited resources that cannot actually be changed by user
    (background            :remove t)
    (backing-store         :remove t)
    (border                :remove t)
    (border-width          :remove t)
    (depth                 :remove t)
    (documentation         :remove t)
    (focus-name            :remove t)
    (height                :remove t)
    (name                  :remove t)
    (override-redirect     :remove t)
    (save-under            :remove t)
    (sensitive             :remove t)
    (screen                :remove t)
    (state                 :remove t)
    (width                 :remove t)
    (x                     :remove t)
    (y                     :remove t)))


(defmethod initialize-instance :after ((self root) &rest options)
  (declare (ignore options))
  (with-slots
    (display screen (id xlib::id) x y width height border-width depth initialization)
    self
    
    ;; A root contact represents a root window
    (setf
      id             (window-id (screen-root screen))
      initialization nil			;; Root window is already realized
      x              0
      y              0
      width          (screen-width screen)
      height         (screen-height screen)
      border-width   0
      depth          (screen-root-depth screen))
    
    ;; Update CLX resource id lookup to associate root id with root contact
    (xlib::save-id display id self)))

(defun contact-root (contact)
  ;; Return the root contact associated with CONTACT
  (declare (type contact contact)
	   (values root))
  (do* ((parent (contact-parent contact) (contact-parent contact)))
      ((null parent) contact)
    (setq contact parent)))

(defun contact-screen (contact)
  ;; Return the xlib:screen associated with CONTACT
  (declare (type contact contact)
	   (values screen))
  (slot-value (the root (contact-root contact)) 'screen))

(defun get-pixmap (contact image)
  "Converts an image into a pixmap resource."
  (declare (type image image)
	   (type contact contact)
	   (values (or null pixmap)))
  (let ((root (contact-root contact)))
    (with-slots ((root-pixmap-cache pixmap-cache) depth) (the root root)
      (let* ((cache root-pixmap-cache)
	     (pixmap (assoc image cache)))
	
	(if pixmap
	    (second pixmap)
	    
	    (if (> depth 1)
		
		(progn
		  (using-gcontext (gcontext :drawable root :foreground 1 :background 0) ;; **** WARNING ****
		    (setq pixmap (image-pixmap root image :depth depth :gcontext gcontext)))
		  (push (list image pixmap) root-pixmap-cache)
		  pixmap)
		
		(progn
		  (setq pixmap (image-pixmap root image :depth depth))
		  (push (list image pixmap) root-pixmap-cache)
		  pixmap)))))))

(defun get-cursor (contact number)
  (declare (type card8 number)
	   (type contact contact)
	   (values (or null pixmap)))
  (let* ((root (contact-root contact))
	 (cache (slot-value (the root root) 'cursor-cache))
	 (cursor (getf cache number)))
    (unless cursor
      (let ((font (open-font (contact-display contact) "cursor")))
	(setq cursor
	      (create-glyph-cursor
		:source-font font
		:source-char number
		:mask-font font
		:mask-char (1+ number)
		:foreground (make-color :red 0.0 :green 0.0 :blue 0.0)
		:background (make-color :red 1.0 :green 1.0 :blue 1.0))))
      (setf (slot-value (the root root) 'cursor-cache)
	    (list* number cursor cache)))
    cursor))




;;;-----------------------------------------------------------------------------
;;; STREAM SUPPORT

;;; PHILOSOPHY
;;;
;;; CLUE keeps a single character buffer for all windows, instead of a
;;; separate buffer for every window.  The reason its done this way is
;;; to prevent focus management problems within an application.  We
;;; reason that a single application will use a single display (or one
;;; display per process), and that when users type on the keyboard,
;;; they're typing to the APPLICATION, not to a (sub)widow of an
;;; application.  In particular, users shouldn't have to care about
;;; keyboard focus within an application.
;;;
;;; If there are several stream contacts for a particular display
;;; (server connection) then the contact getting keystrokes is the
;;; contact that's doing the read.  With a single buffer there's no need
;;; to worry about where the mouse is within the application, or which
;;; window has the keyboard focus.  The user is never left typing
;;; into a dead window, only to have the buffered key events appear
;;; later when the keyboard focus changes.

(defun read-character (display &optional timeout)
  "Enters an input loop which can be exited whenever a character is
   available in the display keyboard buffer. The function's return value
    is the next char from this buffer."
  (or (pop (display-keyboard-buffer display))
      (loop
	(process-next-event display timeout)
	(let ((char (pop (display-keyboard-buffer display))))
	  (when (or char timeout)
	    (return char))))))

(defun unread-character (display character)
  "Make CHARACTER be the next character returned from GET-CHARACTER"
  (push character (display-keyboard-buffer display)))

(defun listen-character (display &optional (timeout 0))
  "If a character is available within TIMEOUT seconds, return it without
    removing it from the display keyboard buffer. Otherwise return NIL."
  (let ((char (read-character display timeout)))
    (when char
      (unread-character display char)
      char)))

(defun append-characters (display character &optional (start 0) end)
  "Put a character or string in the display keyboard buffer"
  (declare (type display display)
	   (type (or string-char string) character))
  ;; When event-handlers return a character or string, stuff it into the keyboard buffer
  (etypecase character
    (character (setf (display-keyboard-buffer display)
		     (nconc (display-keyboard-buffer display) (cons character nil))))
    (string
     (do ((i start (1+ i))
	  (end (or end (length character))))
	 ((>= i end))
       (setf (display-keyboard-buffer display)
	     (nconc (display-keyboard-buffer display) (cons (char character i) nil)))))))

(defun clear-characters (display)
  "Clear the display keyboard buffer"
  (declare (type display display))
  (setf (display-keyboard-buffer display) nil))


;;;-----------------------------------------------------------------------------
;;; GEOMETRY MANAGEMENT

(defmethod add-child ((self composite) contact &key)
  "Put CONTACT on its parent's list of managed contacts"
  ;; Default is to put at end of list
  (with-slots ((manager-children children)) self
    (let ((children manager-children))
      (unless (member contact children :test #'eq)
	(setf manager-children (nconc children (cons contact nil)))))))

(defmethod delete-child ((self composite) contact &key)
  "Remove CONTACT from the list of contacts"
  (with-slots ((manager-children children)) self
    (setf manager-children (delete contact manager-children))))

;; Utility functions for geometry management
(defun previous-sibling (contact)
  "Return the first managed contact BEFORE CONTACT"
  (let ((previous nil))
    (dolist (sibling (composite-children (contact-parent contact)))
      (when (eq sibling contact) (return previous))
      (when (managed-p sibling) (setq previous sibling)))))

(defun next-sibling (contact)
  "Return the first managed contact AFTER CONTACT"
  (dolist (sibling (cdr (member contact (composite-children (contact-parent contact)) :test #'eq)))
    (when (managed-p sibling) (return sibling))))

(defmethod (setf contact-priority) (new-priority (contact contact) &optional new-sibling)
  (setf (window-priority contact new-sibling) new-priority))

(defun change-priority (contact priority &key sibling accept-p)
  "Change CONTACT's stacking order"
  (declare (type contact contact)
	   (type (member :above :below :top-if :bottom-if :opposite) priority)
	   (type (or null contact) sibling)
	   (values success-p priority sibling))
  
  ;; Refuse request, with no compromise, if unrealized
  (when (realized-p contact)
    (multiple-value-bind (success-p new-priority new-sibling)
	(if (managed-p contact)
	    (manage-priority (contact-parent contact) contact priority sibling)
	    (values t priority sibling))
      
      (when (and (or success-p accept-p)
		 (not (top-level-p contact)))	       ; Already done, if top-level
	(setf (contact-priority contact new-sibling) new-priority))
      
      (values success-p new-priority new-sibling))))

(defmethod manage-priority ((self composite) contact priority sibling &key)
  "Change the stacking order of CONTACT relative to SIBLING.
   PRIORITY is one of :above :below :top-if :bottom-if :opposite."
  (declare (type (member :above :below :top-if :bottom-if :opposite) priority)
	   (type (or null contact) sibling)
	   (values success-p priority sibling))
  self contact ;; not used
  (values t priority sibling))

(defmethod accept-focus-p ((contact contact))
  "Returns non-nil when CONTACT is willing to become the keyboard input focus"
  (declare (values boolean))
  (and (visible-p contact)
       (plusp (logand (contact-event-mask contact)
		      #.(make-event-mask :key-press :key-release)))))

(defmethod move-focus ((composite composite) &optional (direction :next) &key start revert-to)
  "Move the input focus to the :next :previous or :set contact from START.
 START defaults to the current focus if there is one, or the first child.
 Returns the new focus contact or NIL if no contacts will accept the
 focus (see accept-focus-p)."
  (declare (type (member :next :previous :set) direction)
	   (type (or null contact) start)
	   (values (or null focus-contact)))

  (let* ((start (or start (composite-focus composite)))
	 (focus (or start (first (composite-children composite)))))
    
    (when focus ;; focus nil when composite has no children
      (assert (member focus (composite-children composite) :test #'eq) ()
	      "~s isn't a child of ~s" focus composite)
      (when
	(setf focus
	      (if (eq :set direction)
		  ;; Ensure requested focus is ready to accept
		  (when (accept-focus-p focus) focus)		  

		  ;; Else look for next focus ready to accept
		  (do* ((get-sibling (ecase direction (:next 'next-sibling) (:previous 'previous-sibling)))
			(focus       (funcall get-sibling focus) (funcall get-sibling focus)))		       
		       ((or (not focus) (eq focus start)))
		    (when (accept-focus-p focus) (return focus)))))
	
	;; Tell server to change input focus
	(set-input-focus (contact-display focus) focus (or revert-to :parent)))

      ;; Record focus child found
      (setf (slot-value (the composite composite) 'focus) focus))))


(defmethod preferred-size ((contact contact) &key width height border-width)
  "Return preferred size, based on given changes to current values."
  (declare (values width height border-width))
  ;; Primary method is compliant
  (with-slots ((current-width width)
	       (current-height height)
	       (current-border-width border-width)) contact
    (values (or width current-width)
	    (or height current-height)
	    (or border-width current-border-width))))


(defun change-geometry (contact &key x y width height border-width accept-p)
  "Geometry management.  Nil values indicate parameters the geometry manager can change at will.
   Returns T when the request is granted, NIL when refused or NIL x y width height border-width
   when a compromise is offered. When ACCEPT-P, always take the geometry manager's compromise."
  (declare (type contact contact)
	   (type (or null int16) x y)
	   (type (or null card16) width height border-width)
	   (type boolean accept-p)
	   (values success-p x y width height border-width))
  (unless (destroyed-p contact)
    (let ((accept-p (or accept-p (top-level-p contact))))
      (with-slots ((contact-x x)
		   (contact-y y)
		   (contact-width width)
		   (contact-height height)
		   (contact-border-width border-width)
		   (contact-parent parent)) (the basic-contact contact)
	(multiple-value-bind (success-p x1 y1 width1 height1 border-width1)
	    (if (managed-p contact)
		(manage-geometry contact-parent contact x y width height border-width)
		(values t
			(or x contact-x) (or y contact-y)
			(or width contact-width) (or height contact-height)
			(or border-width contact-border-width)))
	  (if (or success-p (and accept-p x1))
	      (with-state (contact)
		(unless success-p ;; Accept a compromise geometry
		  (unless (manage-geometry contact-parent contact x1 y1 width1 height1 border-width1)
		    (error "manage-geometry failed to accept its own compromise geometry")))
		(when (or (not (= contact-x x1))
			  (not (= contact-y y1)))
		  (move contact x1 y1))
		(when (or (not (= contact-width width1))
			  (not (= contact-height height1))
			  (not (= contact-border-width border-width1)))
		  (resize contact width1 height1 border-width1))))

	  (values success-p x1 y1 width1 height1 border-width1))))))

(defparameter *contact-notified* nil) ;; NIL outside without-requests

(defmacro without-requests (contact &body body)
  "Any server requests on CONTACT ordinarily sent within BODY should be skipped.
This wrapper is used when CONTACT needs to update its state to reflect window changes
already performed by the user/wm."
  `(let ((*contact-notified* ,contact)) ,@body))

(defmethod move ((contact contact) x y)
  "Move CONTACT to coordinates X/Y relative to its parent."
  (with-slots ((contact-x x) (contact-y y)) contact
    (unless (eq contact *contact-notified*)
      (when (realized-p contact)
	(unless (= contact-x x) (setf (drawable-x contact) x))
	(unless (= contact-y y) (setf (drawable-y contact) y))))
    (setf contact-x x)
    (setf contact-y y)))
  
(defmethod resize ((contact contact) width height border-width)
  "Change the size of CONTACT."
  (with-slots ((contact-width width)
	       (contact-height height)
	       (contact-border-width border-width)) contact
    (unless (eq contact *contact-notified*)
      (when (realized-p contact)
	(unless (= contact-width width)
	  (setf (drawable-width contact) width))
	(unless (= contact-height height)
	  (setf (drawable-height contact) height))
	(unless (= contact-border-width border-width)
	  (setf (drawable-border-width contact) border-width))))
    (setf contact-width width)
    (setf contact-height height)
    (setf contact-border-width border-width)))

(defconstant *default-contact-height* 16)
(defconstant *default-contact-width* 16)

(defmethod manage-geometry ((parent composite) contact x y width height border-width &key)  
  (declare (type contact contact)
	   (type (or null int16) x y)
	   (type (or null card16) width height border-width)
	   (values success-p x y width height border-width))
  (with-slots ((contact-x x)
	       (contact-y y)
	       (contact-width width)
	       (contact-height height)
	       (contact-border-width border-width)) (the contact contact)

    ;; Just ensure positive size
    (let* ((requested-width   (or width contact-width))
	   (acceptable-width  (if (zerop requested-width)
				  *default-contact-width*
				  requested-width))
	   (requested-height  (or height contact-height))
	   (acceptable-height (if (zerop requested-height)
				  *default-contact-height*
				  requested-height)))
      
      (values (and (= requested-width acceptable-width)
		   (= requested-height acceptable-height))
	      (or x contact-x)
	      (or y contact-y)
	      acceptable-width
	      acceptable-height	    
	      (or border-width contact-border-width)))))


;;; change-layout should be called whenever the set of managed children for the
;;; composite is changed.  Its purpose is to update children geometries for the new
;;; managed set.  The newly-managed argument, if given, would the child which is now
;;; being added to the managed set.  (The change-layout algorithm might use this in
;;; enforcing constraints, perhaps to put the squeeze on the new guy rather than on
;;; the previously-managed set).  This method, like manage-geometry, is never called
;;; by application programmers and rarely called by contact programmers, but must be
;;; provided by the composite programmer in order to implement the composite's gmgmt
;;; policy.

;;; Most composites will probably want to over-ride this
(defmethod change-layout ((composite composite) &optional newly-managed)
  "Called whenever the set of managed children changes."
  (declare (type (or null contact) newly-managed))
  (if newly-managed
      (change-geometry newly-managed :accept-p t)
      (dolist (child (composite-children composite))
	(change-geometry child :accept-p t))))



