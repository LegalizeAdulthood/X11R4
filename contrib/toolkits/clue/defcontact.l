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

(export '(
	  defcontact	  
	  )
	'cluei)

(proclaim '(inline class-name-of))
(defun class-name-of (instance)
  (class-name (class-of instance)))

(defmacro clue-resources (class-name)
  `(get ,class-name 'resources))

(defmacro clue-constraints (class-name)
  `(get ,class-name 'constraint-resources))

(proclaim '(inline class-all-superclasses))
(defun class-all-superclasses (class)
  "Return the list of all of CLASS's superclasses"
  (get class 'parent-classes))

(defsetf class-all-superclasses (class) (list)
  `(setf (get ,class 'parent-classes) ,list))


;;; Contacts are CLOS classes whose slots and non-slot initialization
;;; options may be initialized from the resource databse.  This is done
;;; as follows:
;;; slots:    A slot that is also a resource has its :initform 
;;;           altered so that a resource value can be looked up in
;;;           :after initialize-instance.  If the resource isn't found, 
;;;           the result of evaluating, then the original slot :initform is used.
;;;	      :slot for a slot resource is the slot name symbol
;;;
;;; non-slot: A resource with no associated slot has :slot NIL.  If a sub-class
;;;           defines a slot with the same name as a super-class's
;;;           non-slot resource, the resources continues to be supported
;;;           as a non-slot resource.  The slot will be initialized
;;;           through the init-plist.


(defmacro defcontact (class superclass-names variables &rest options)
  "Defines a contact CLASS."

  (let (resources documentation constraints)
    ;; Normalize slot variables
    (setq variables
	  (mapcar #'(lambda (r) (if (consp r) r (list r))) variables))
    
    ;; Collect options
    (dolist (option options)
      (ecase (car option)
	(:constraints
	 (setf constraints (mapcar #'(lambda (r) (if (consp r) r (list r))) (cdr option))))	
	(:resources
	 (setf resources   (mapcar #'(lambda (r) (if (consp r) r (list r))) (cdr option))))	 
	(:documentation
	 (setf documentation (second option)))))

    ;; Fill in options
    (setf constraints
	  (define-constraints class superclass-names constraints))
    (setf resources
	  (define-contact-resources class superclass-names variables resources))
    (setf variables 
	  (mapcar
	    #'(lambda (var)
		;; Fill in :initform and :initarg for slot resources
		(let* ((name     (intern (symbol-name (car var)) 'keyword))		       
		       (initarg  (getf (cdr var) :initarg))
		       (type     (getf (cdr var) :type))		       
		       (resource (assoc name resources :test #'eq)))
		  
		  (when resource
		    (unless initarg		      
		      ;; Variables that show up in the resources list need :initarg's
		      (setf var `(,@var :initarg ,(or initarg name))))
		    
		    ;; Save slot :initform in resource spec, if necessary.
		    (unless (getf (cdr resource) :initform)
		      (setf (getf (cdr resource) :initform)
			    (getf (cdr var) :initform)))
		    
		    ;; Set the slot :initform to nil so that resource values from the
		    ;; database will supersede (see define-initialize-resource-slots)		    
		    (setf (getf (cdr var) :initform) nil)
		    
		    (when type
		      ;; Adjust type so that nil :initform is valid
		      (setf (getf (cdr var) :type) `(or null ,type)))))
		var)
	    variables))
    `(progn
       (eval-when (compile load eval)
	 (setf (class-all-superclasses ',class)
	       ',(delete-duplicates
		   (apply #'append
			  superclass-names
			  (mapcar #'class-all-superclasses superclass-names))
		   :test #'eq))
	 (setf (clue-resources ',class)   ',resources
	       (clue-constraints ',class) ',constraints))
       (defclass ,class ,superclass-names
	 ,variables	 
	 ,@(when documentation `((:documentation ,documentation))))
       ,(define-clue-default-options class resources)
       ,(define-initialize-resource-slots class resources)
       ,(define-initialize-constraints class constraints)
       ',class)))

(proclaim '(special *resources* *parent*))

(defmacro lookup-resource (class resource-name)
  ;; Lookup and type-check/convert resource-name for class
  (let* ((value (gensym)))
    `(let ((,value ,(find-resource class resource-name (clue-resources class) '*resources*)))
       ,(convert-resource class resource-name value (clue-resources class) '*parent*))))

(defun find-resource (class resource-name resources resource-table)
  "Generate code to lookup resource-name for class, and return it or its default value."
  (let* ((name (intern (string resource-name) :keyword))
	 (resource (assoc name (or resources (clue-resources class)))))
    (unless (pop resource) (error "~a isn't a resource of the ~s class" resource-name class))
    (let ((class (getf resource :class))
	  (init (getf resource :initform)))
      `(or (get-search-resource ,resource-table ',name ',class) ,init))))

(defun convert-resource (class resource-name value resources parent)
  ;; Generate code to do type checking and conversion on value for resource-name of class
  (let* ((name (intern (string resource-name) :keyword))
	 (resource (assoc name (or resources (clue-resources class)))))
    (unless (pop resource) (error "~a isn't a resource of the ~s class" resource-name class))
    (let ((type (getf resource :type)))
      (if type
	  `(if (typep ,value ',type)
	       ,value
	       (do-convert ,parent ,value ',type))
	  value))))

(defun do-convert (parent value type)
  (let ((converted-value (convert parent value type)))
    ;; Boolean conversion may convert a non-nil value to nil, and that's OK.
    (when (and (null converted-value)
	       (not (typep converted-value type)))
      (xlib::x-type-error value type))
    converted-value))

(defun define-contact-resources (class superclass-names variables resource-list)
  "Construct and validate the resource list for CLASS.  A :slot option is added to
each resource specification containing the resource slot name, if any."
  (do* (name
	(resources resource-list   (cdr resources))
	(resource  (car resources) (car resources))
	
	;; Initialize result with all inherited resources
	(result    (delete-duplicates
		     (mapcan #'(lambda (parent) (copy-list (clue-resources parent)))
			   superclass-names)
		     :from-end t
		     :test     #'eq
		     :key      #'first)))
       ((endp resources)
	(reconcile-contact-resource-with-slots variables result))
    
    ;; Make name and class keywords
    (let ((class (getf (cdr resource) :class)))
      (setf name (intern (string (car resource)) 'keyword)
	    (car resource) name)
      (when class
	(setf (getf (cdr resource) :class) (intern (string class) 'keyword))))
    
    ;; Error checking
    (do ((option (cdr resource) (cddr option)))
	((endp option))      
      (let ((key (car option)))
	
	(assert (member key '(:initform :type :class :documentation :slot :remove)) ()
		"~s is an unknown option for resource ~s in ~s." key (car resource) class)
	
	(when (eq key :initform)
	  (assert
	    (let ((slot-initform (getf (cdr (assoc (symbol-name name) variables
						   :key #'symbol-name
						   :test #'equal))
				       :initform)))
	      (or (not slot-initform) (equal slot-initform (second option))))
	    ()
	    "Different slot and resource :initform's specified for resource ~s."
	    (car resource)))))   
    
    ;; Merge resource with parent's resource
    (do* ((inherited-resource (cdr (assoc name result :test #'eq)))
	  (old                inherited-resource   (cddr old)))
	 ((endp old))
      (unless (getf (cdr resource) (car old))
	(setq resource (append resource `(,(car old) ,(cadr old))))))
    
    ;; Add (updated) resource spec to final list (unless the :remove option was given)
    (setq result (delete name result :key #'first :test #'eq :count 1))
    (unless (getf (cdr resource) :remove) (push resource result))))


(defun define-constraints (class superclass-names resource-list)
  "Construct and validate the constraint resource list for CLASS."
  (do* (name
	(resources resource-list (cdr resources))
	(resource (car resources) (car resources))
	
	;; Initialize result with all inherited resources
	(result (mapcan #'(lambda (parent)
			    (copy-list
			      (clue-constraints parent)))
			superclass-names)))
       ((endp resources) result)
    ;;
    ;; Make name and class keywords
    (let ((class (getf (cdr resource) :class)))
      (setf name (intern (string (car resource)) 'keyword)
	    (car resource) name)
      (when class
	(setf (getf (cdr resource) :class) (intern (string class) 'keyword))))
    ;;
    ;; Merge resource with parent's resource
    (do* ((inherited-resource (cdr (assoc name result :test #'eq)))
	  (old inherited-resource (cddr old)))
	 ((endp old))
      (unless (getf (cdr resource) (car old))
	(setq resource (append resource `(,(car old) ,(cadr old))))))
    ;;
    ;; Error checking
    (do ((key (cdr resource) (cddr key)))
	((endp key))
      (unless (member (car key) '(:initform :type :class :documentation))
	(error "~s is an unknown option for constraint ~s in ~s" (car key) (car resource) class)))
    (setq result (delete name result :key #'car :test #'eq))
    (push resource result)))


(defun reconcile-contact-resource-with-slots (variables resource-list)
  "Reconcile resource and slot types and mark slot/non-slot resources."
  
  (do* ((resources resource-list   (cdr resources))
	(resource  (car resources) (car resources))
	(name      (car resource)  (car resource)))
       
       ((endp resources) resource-list)
    
    (let ((entry (assoc (symbol-name name) variables :key #'symbol-name :test #'equal)))
      (if entry
	  ;; Slot resource 
	  (let ((slot-type     (getf (cdr entry) :type))
		(slot-initform (getf (cdr entry) :initform)))

	    ;; Reconcile slot/resource type
	    (when slot-type
	      (let ((resource-type (getf (cdr resource) :type)))
		(if resource-type
		    (unless (or (equal resource-type slot-type)
				(subtypep resource-type slot-type))
		      (error "~s is incompatibly typed as ~s in slot and ~s in the resources"
			     name slot-type resource-type))
		    (setq resource (nconc resource `(:type ,slot-type))))))

	    ;; Initialize :slot option
	    (when (eq (getf (cdr resource) :slot :undefined) :undefined)
	      (setq resource (nconc resource (list* :slot (car entry) nil))))

	    ;; Initialize :initform option
	    (when slot-initform
	      (setf (getf (cdr resource) :initform) slot-initform)))
	  
	  ;; Mark non-slot resources with a NIL :slot value
	  (when (eq (getf (cdr resource) :slot :undefined) :undefined)
	    (setq resource (nconc resource '(:slot nil))))))))

(defun define-initialize-resource-slots (contact-class resources)
  "Define the initialize-resource-slots method for CONTACT-CLASS."
  (let ((code nil) slot)
    `(defmethod initialize-resource-slots ((instance ,contact-class) resource-table app-defaults)
       
       ;; Check resource types and fill in defaults.
       ;; Assumes the :initform for all resource slots NIL (the true initform is evaluated here).       
       (let ((parent (slot-value (the ,contact-class instance) 'parent)))
	 
	 ;; NOTE: PARENT is null when contact-class is ROOT.
	 ;; This may lose for some root resources requiring conversion.
	 ,@(dolist (resource resources code)
	     (when (setq slot (getf (cdr resource) :slot))
	       (push
		 (set-resource-slot (car resource) resources slot)
		 code)))))))

(defun define-initialize-constraints (composite-class constraints)
  "Define the initialize-constraints method for COMPOSITE-CLASS."
  (when constraints
    (let (code)
      `(defmethod initialize-constraints ((parent ,composite-class) initargs resource-table)
	 (let (options (app-defaults (getf initargs :defaults)))
	   
	   ;; Generate code to find constraint values, convert to representation type,
	   ;; and add to option list
	   ,@(dolist (constraint constraints code)
	       (push
		 (push-constraint (car constraint) constraints)
		 code))
	   options)))))

(defmethod initialize-constraints (parent initargs resource-table)
  (declare (ignore parent initargs resource-table))
  ;; Default primary method for class with no constraints -- nothing happens!
  nil)

(defun define-clue-default-options (contact-class resources)
  "Defines the default-options method that defaults non-slot resources for CONTACT-CLASS."
  (let (code) 
    `(defmethod default-options ((class (eql ',contact-class)) initargs)       
       (let ((options      (copy-list initargs))
	     (app-defaults (getf initargs :defaults))
	     (parent       (getf initargs :parent)))

	 ;; "Use" app-defaults to avoid compiler warning when no non-slot resources exist.
	 app-defaults
	 
	 ;; Convert parent arg, if necessary
	 (when (display-p parent)	  
	   (setf options (list* :parent
				(setf parent (display-root parent (getf initargs :screen)))
				options)))

	 ;; Build a resource table for fast resource lookup
	 (let ((resource-table (get-contact-resource-table class parent initargs)))
	   
	   ;; Generate code to find resource values, convert to representation type,
	   ;; and add to option list
	   ,@(dolist (resource resources code)
	       (unless (getf (cdr resource) :slot)
		 (push
		   (push-resource (car resource) resources)
		   code)))
	   (list* :resource-table resource-table options))))))


(defun push-resource (name resources)
  "Generate code to find resource value, convert to representation type, and add to option list."
  (let ((resource (rest (assoc name resources))))    
    (let ((type (getf resource :type)))
      `(let* ((initarg-p (getf initargs ,name))
	      (value     (or  initarg-p
			      (get-search-resource resource-table ,name ,(getf resource :class))
			      (getf app-defaults ,name)
			      ,(getf resource :initform)))
	      (no-convert-p ,(if type `(and value (typep value ',type)) t)))
	 (and value
		(not (and initarg-p no-convert-p))
		(setf options
		      (list* ',name
			     (if no-convert-p
				 value
				 ,(when type `(do-convert parent value ',type)))
			     options)))))))


(defun push-constraint (name constraints)
  "Generate code to find constraint value, convert to representation type, and add to option list."
  (let ((constraint (rest (assoc name constraints))))    
    (let ((type (getf constraint :type)))
      `(let* ((value     (or  (getf initargs ,name)
			      (get-search-resource resource-table ,name ,(getf constraint :class))
			      (getf app-defaults ,name)
			      ,(getf constraint :initform))))
	 (when value	   
	   (setf options
		 (list* ',name
			,(if type
			     `(do-convert parent value ',type)
			     'value)			     
			options)))))))


(defun set-resource-slot (name resources slot)
  "Generate code to find resource value, convert to representation type, and set corresponding slot."  
  (let ((resource (rest (assoc name resources))))    
    (let ((type (getf resource :type)))
      `(let* ((value     (or  (slot-value instance ',slot)
			      (get-search-resource resource-table ,name ,(getf resource :class))
			      (getf app-defaults ,name)
			      ,(getf resource :initform))))
	 (when value	   
	   (setf (slot-value instance ',slot)
		 ,(if type
		      `(do-convert parent value ',type)
		      'value)))))))