;;; -*- Mode:Common-Lisp; Package:CLUEI; Syntax:COMMON-LISP; Base:10; Lowercase:T -*-

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

;;; Hacked subset of CLOS that implements the a minimal object programming environment based on defstructs.
;;; The following are impemented:
;;;
;;; DEFCLASS
;;;   At most one super-class is supported (no multiple inheritance)
;;;   The only slot options supported are :INITARG :INITFORM, :TYPE, :READER and :ACCESSOR
;;;   :ALLOCATION is always :instance even when specified :class or :dynamic
;;;   The only class options supported are :DOCUMENTATION
;;;
;;; DEFMETHOD
;;;  Method qualifiers are not allowed
;;;   only one parameter-specializer is supported
;;;   :around method qualifiers are NOT supported
;;;
;;; CALL-NEXT-METHOD
;;;   The only method combination supported is :most-specific-first.
;;;
;;; WITH-SLOTS
;;;   Does direct substitution, not a code walk.
;;;
;;; SLOT-VALUE
;;;   A macro that only works when the first parameter is typed
;;;   with (the CLASS value), and the slot-name must be a constant.
;;;
;;; MAKE-INSTANCE
;;; CLASS-OF
;;; CMAKUNBOUND
;;; CBOUNDP
;;; SLOT-EXISTS-P
;;;
;;; KNOWN PROBLEMS:
;;; ---------------
;;; This system introduces load-order dependencies between files 
;;; that have the same method name.  For example:
;;; The system consists of files A B and C, compiled and loaded in that order,
;;; files B and C containing defmethod XXX.  If file A is modified to contain
;;; a method XXX (for a different class) it will have a new generic method for
;;; the XXX method.  However, files B and C haven't changed and so don't get
;;; re-compiled. When reloading the files, A will load the correct generic method,
;;; but it gets clobered by the old (incorrect) generic methods in files B and C.
;;; The moral of the story is: Recompile EVERYTHING often.
;;;
;;; Change history:
;;;
;;;  Date	Author	Description
;;; -------------------------------------------------------------------------------------
;;; 08/12/87	LGO	Created
;;; 11/23/87	LGO	Added :before and :after method qualifiers
;;; 01/11/88	LGO	Changed copy-list to copy-tree in insert-method
;;;			This fixes the nasty disappearing :after method bug.
;;; 02/02/88	LGO	Added &rest arg to call-next-method
;;;			Removed :accessor-prefix option from DEFCLASS
;;;			Added SLOT-VALUE, CMAKUNBOUND and CBOUNDP.
;;;			Changed the definition of WITH-SLOTS to conform to
;;;			the new CLOS spec.
;;; 02/12/88	LGO	Added UNDEFMETHOD
;;; 02/12/88	LGO	Added PRINT-INSTANCE method
;;; 02/18/88	LGO	Added ALLOCATOR slot to standard-class
;;; 02/18/88	LGO	Added class si:type-predicate property to speed-up TYPEP
;;; 02/26/88	LGO	Added compile-time type checking of initforms to defclass.
;;; 03/22/88	LGO	Added slot-exists-p
;;; 03/23/88	LGO	Initialize-instance now initializes inherited slots
;;; 03/23/88	LGO	Don't put initializations (or types) in defstruct
;;;			[This speeds up make-instance greatly]
;;; 03/24/88	LGO	Use CASE instead of TYPECASE for initialize-instance
;;; 08/10/88	LGO	Replace symbol-class with find-class
;;; 08/24/88	LGO	Use correct arglist ordering for setf methods
;;; 08/25/88	LGO	Use subclass-p instead of subtypep in insert-method
;;;			(works around a bug in Franz CL)
;;; 10/04/88	LGO	Add fake slot-boundp

(in-package 'cluei :use '(lisp xlib))

(pushnew :CLOS-KLUDGE *features*)

(export '(defclass
	   defmethod
	   call-next-method
	   with-slots
	   slot-value
	   make-instance
	   class-of
	   find-class
	   cmakunbound
	   cboundp
	   slot-exists-p
	   slot-boundp
	   initialize-instance

	   undefmethod
	   ))

;;; if FORM is defined as the list
;;;      '((X Y Z) 
;;;	   "this is a test doc-string"
;;;	   (DECLARE (ARGLIST (X Y Z)) 
;;;                  (VALUES (+ X Y Z)) (SPECIAL icky pick wicky) (inline mumble) (income tax))
;;;	   (+ x y z)))
;;; then (PARSE-BODY (CDR FORM) nil t) would return three values:
;;;  1) ((+ X Y Z))
;;;  2) ((DECLARE (ARGLIST (X Y Z)) 
;;;                 (VALUES (+ X Y Z)) (SPECIAL ICKY PICK WICKY) (INLINE MUMBLE) (INCOME TAX)))
;;;  3) "this is a test doc-string"
;;; and (PROCESS-DEFUN-BODY 'foo form nil t) would return the list
;;;  (NAMED-LAMBDA 
;;;      (FOO (:DESCRIPTIVE-ARGLIST (X Y Z)) (:VALUES (+ X Y Z))) (X Y Z) 
;;;      "this is a test doc-string" 
;;;      (DECLARE (SPECIAL ICKY PICK WICKY) (INLINE MUMBLE) (INCOME TAX)) 
;;;      (BLOCK FOO (+ X Y Z)))

#-ti
(defun parse-body (body environment &optional (doc-string-allowed t))
  "[From SPICE]:This function is to parse the declarations and doc-string out
  of the body of a defun-like form.  Body is the list to be parsed and consists
  of everything after the formal parameter list.
  Environment is the lexical environment to expand macros in.  If
  Doc-String-Allowed is true, then a doc string will be parsed out of the body
  and returned.  If it is false then a string will terminate the search for
  declarations.  Three values are returned: the tail of Body after the
  declarations and doc strings, a list of declare forms, and the doc-string,
  or NIL if none."
  (declare (values body decs doc-string))
  (let (decls
	doc)
    (do ((tail body (cdr tail)))
	((endp tail)
	 (values tail 
		 (nreverse (the list decls)) 
		 doc))
      (let ((form (car tail)))
	(cond ((and (stringp form) (cdr tail))
	       (if doc-string-allowed
		   (setq doc form)
		   (return (values tail 
				   (nreverse (the list decls))
				   doc))))
	      ((not (and (consp form) (symbolp (car form))))
	       (return (values tail
			       (nreverse (the list decls))
			       doc)))
	      ((eq (car form) 'declare)
	       (push form decls))
	      (t
	       (multiple-value-bind (res win)
		   (macroexpand form environment)
		 (if (and win (consp res) (eq (car res) 'declare))
		     (push res decls)
		     (return (values tail
				     (nreverse (the list decls))
				     doc))))))))))

(defun macro-warn (string &rest format-args)
  #+explorer
  (apply #'sys:record-and-print-warning nil nil nil string format-args)
  #-explorer
  (apply #'warn string format-args))

(defstruct (standard-class (:constructor internal-initialize-standard-class)
			   (:copier nil))
  (name nil :type symbol)			; Class name
  (superclass-names nil :type list)		; List of parent class names
  (slots nil :type list)			; list of slot names for this class and superclasses
						;; Slots is (mapcar #'variable-name variables)
  (variables nil :type list)			; list of variable for this class and superclasses
  (prefix "" :type string)			; Accessor prefix string
  (allocator nil)				; Allocator function
  ;; class options:
  (default-initargs nil :type list)
  (documentation "" :type string)
  (metaclass 'standard-class :type symbol)
  )

(defstruct (variable (:type vector) (:copier nil))
  name initarg initform)

(proclaim '(inline class-name))
(defun class-name (class) (standard-class-name class))

(defconstant *class-property* 'meta-class)

(defun find-class (class-name &optional (errorp t) environment)
  (declare (inline find-class)
	   (ignore environment))  
  (or (get class-name *class-property*)
      (and errorp (error "~s isn't a class-name" class-name))))

(defsetf find-class (class-name &optional errorp environment) (new-name)
  (declare (ignore errorp environment))
  `(setf (get ,class-name *class-property*) ,new-name))

(defun cboundp  (class &optional environment)
  "Returns T if CLASS is the name of a class in ENVIRONMENT"
  (declare (type symbol class)
	   (ignore environment))
  (and (get class *class-property*) t))

(defun cmakunbound (class-name &optional environment)
  ;; Cause the CLASS symbol to no-longer name a class in ENVIRONMENT
  (declare (type symbol class-name)
	   (ignore environment))
  (let ((class (get class-name *class-property*)))
    (when class
      (dolist (super (standard-class-superclass-names class))
	(cmakunbound super))
      (remprop class-name *class-property*))))

#-explorer
(defun class-of (object) (get (type-of object) *class-property*))
#+explorer
(defun class-of (object) (get (si:array-leader object 1) *class-property*))
      
#+explorer
(compiler::add-optimizer class-name class-name-opt)
#+explorer
(defun class-name-opt (form)
  ;; Optimize (class-name (class-of thing)) to (si:array-leader thing 1)
  ;; This is used by CLASS-NAME-OF, which needs to be fast.
  (if (and (consp (cadr form))
	   (eq (caadr form) 'class-of))
      `(si:array-leader ,@(cdadr form) 1)
    form))

(defun slot-exists-p (object slot-name)
  "Returns T when SLOT-NAME is defined for OBJECT."
  (let* ((class (class-of object)))
    (and (member slot-name (standard-class-slots class) :test #'eq) t)))

(proclaim '(inline subclass-p))
#+comment
(defun subclass-p (subclass class)
  ;; Returns T when SUBCLASS is a subclass of CLASS
  (if (and (symbolp subclass) (symbolp class))
      (let* ((pclass (get subclass *class-property*))
	     (parents (and pclass (standard-class-superclass-names pclass))))
	(or (member class parents :test #'eq)
	    (and parents
		 (dolist (p parents)
		   (and (subclass-p p class)
			(return t))))))
    (subtypep subclass class)))

;; This version assumes only one parent per class
(defun subclass-p (subclass class)
  ;; Returns T when SUBCLASS is a subclass of CLaSS, and is twice as fast
  (if (and (symbolp subclass) (symbolp class))
      (or (eq class t) ;; everything's a subclass of T
	  (do ((pclass (get subclass *class-property*)))
	      ((null pclass) nil)
	    (let ((parent (car (standard-class-superclass-names pclass))))
	      (when (eq class parent)
		(return t))
	      (setq pclass (get parent *class-property*)))))
    (subtypep subclass class)))

(defmacro defclass (class-name superclass-names slots &rest options)
  (let* ((documentation nil)
	 (prefix (concatenate 'string "%INTERNAL-" (string class-name) "-"))
	 (internal-initializer (intern (concatenate 'string "INTERNAL-INITIALIZE-" (string class-name))))
	 (meta-class (or (second (assoc :metaclass options)) 'standard-class))
	 (readers nil)
	 (accessors nil)
	 (variables nil)
	 (class-allocated nil)
	 (slot-names (mapcar #'(lambda (slot) (if (atom slot) slot (car slot))) slots))
	 (superclasses (mapcar #'find-class superclass-names))
	 (inherited-variables (mapcan #'(lambda (v) (copy-list (standard-class-variables v))) superclasses))
	 (inherited-slots (mapcan #'(lambda (s) (copy-list (standard-class-slots s))) superclasses))
	 new-slots);
    ;; The following is equivalent to:
    ;; (set-difference slot-names inherited-slots)))
    ;; except slot ordering is preserved.
    (setq new-slots (remove-if #'(lambda (x) (member x inherited-slots :test #'eq))
			       (The List slot-names)))
    (setq slots					; Reformat slots for defstruct
	  (mapcan
	    #'(lambda (slot)
		(if (atom slot) (list slot)
		  (apply
		    #'(lambda (name &key initarg (initform nil initform-p) type
				    accessor reader (allocation :instance))
			(declare (ignore allocation)) ;; always use :instance
			(when (or initarg initform-p)
			  (setq inherited-variables
				(delete name inherited-variables :key #'variable-name))
			  (push (make-variable :name name :initarg initarg :initform initform)
				variables))
			(when (or reader accessor)
			  (push (list name (or reader accessor)) readers))
			(when accessor
			  (push (list name accessor) accessors))
			;; Check initform type
			#+comment
			(when (and initform-p type (constantp initform))
			  (si:ignore-errors ;; TYPEP may error if type isn't defined yet
			    (unless (typep (eval initform) type)
			      (macro-warn "Warning: For class ~s the initform for slot ~s, ~s isn't ~s"
					  class-name name initform type))))
			;; build defstruct slot descriptor
			`((,name ,initform ,@(and type `(:type ,type)))))
			 slot)))
		  slots))
    (setq variables (nconc (reverse variables) inherited-variables))
    (when (cdr superclass-names)
      (error "Multiple superclass-names not supported"))
    (setq documentation (second (assoc :documentation options)))
    `(progn
       (eval-when (compile load eval)
	 (setf (get ',class-name *class-property*)
	       (make-instance ',meta-class
		 :name ',class-name
		 :prefix ,prefix
		 :allocator ',internal-initializer
		 :superclass-names ',superclass-names
		 :slots ',(union slot-names inherited-slots)
		 :variables ',variables
		 ;; Quote the options
		 ,@(do ((option options (cdr option))
			(result nil))
		       ((endp option) result)
		     (setq result (nconc `(,(caar option) ',(cdar option)) result))))))
       (defstruct (,class-name
		   (:conc-name ,(intern prefix))
		   (:constructor ,internal-initializer ()) ;; Nil arglist for faster construction
		   #-symbolics ;; Symbolics REQUIRES a predicate for typep to work.
		   (:predicate nil)
		   (:copier nil)
		   (:print-function print-instance)
		   ,@(when superclass-names `((:include ,@superclass-names))))
	 ,@(when documentation (list documentation))
	 ,@new-slots)

       ,@(mapcan #'(lambda (slot)
		     (let* ((name (car slot))
			    (type (getf (cdr slot) :type))
			    (initform (getf (cdr slot) :initform))
			    (accessor (intern (concatenate 'string prefix (string (car slot))))))
		       `((defmacro ,accessor (instance)
			   (list 'progn instance
				 ',(if type
				       `(the ,type (get ',class-name ',name))
				     `(get ',class-name ',name))))
			 ,@(when initform
			     `((setf (get ',class-name ',name) ,initform))))))
		 class-allocated)
						; Define a default initialize-instance method
       (defmethod initialize-instance ((instance ,class-name) &rest options)
	 options ;; may not be used
	 ,@(mapcar
	     #'(lambda (variable)
		 (let ((slot (variable-name variable))
		       (initarg (variable-initarg variable))
		       (initform (variable-initform variable)))
		   (if initform
		       `(setf (slot-value (the ,class-name instance) ',slot)
			      ,(if initarg
				   `(or (getf options ',initarg) ,initform)
				 initform))
		     (when initarg
		       `(let ((value (getf options ',initarg)))
			  (when value
			    (setf (slot-value (the ,class-name instance) ',slot) value)))))))
	     variables)
	 instance)
       ,@(mapcar #'(lambda (reader)		;Define reader methods
		     `(defmethod ,(second reader) ((self ,class-name))
			(,(intern (concatenate 'string prefix
					       (string (first reader)))) self)))
		 readers)
       
       ,@(mapcar #'(lambda (reader)		;Define setf methods
		     `(defmethod (setf ,(second reader)) (value (self ,class-name))
			(setf (,(intern (concatenate 'string prefix
						     (string (first reader)))) self) value)))
		 accessors)
       ',class-name
       )))

(defun print-instance (instance stream depth)
  (declare (ignore depth))
  (print-object instance stream))

(defmacro inhibit-fdefine-warnings (&body body)
  #+ti
  `(let ((si:inhibit-fdefine-warnings t)) ,@body)
  #+symbolics  ;; Symbolics can't wrap a defun inside a let
  `(progn (setq *save-inhibit-fdefine-warnings* si:inhibit-fdefine-warnings
		si:inhibit-fdefine-warnings t)
	  ,@body
	  (setq si:inhibit-fdefine-warnings *save-inhibit-fdefine-warnings*))
  #-(or ti symbolics)
  `(progn ,@body)
  )
;; Allow forms within inhibit-fdefine-warnings to start in column 0
#+ti (setf (get 'inhibit-fdefine-warnings 'si:may-surround-defun) t)

(defmacro defmethod (name lambda-list &body body &environment env)
  (if (consp name)
      (progn
	(unless (eq (car name) 'setf)
	  (error "Invalid method name ~s" name))
	(let ((setfer (intern (concatenate 'string "SET-" (string (second name))))))
	  `(progn ;; The first parameter to a setf-method is the value.  Reverse that here.
	     (defmethod ,setfer (,@(last lambda-list) ,@(butlast lambda-list)) ,@body)
	     (eval-when (compile load eval)
	       (inhibit-fdefine-warnings
		 (defsetf ,(second name) ,setfer))))))
    (let (type n args code decs doc
	  (qualifier nil)
	  $method $type $lambda-list)
      (declare (special $method $type $lambda-list))
      (when (keywordp lambda-list)
	(setq qualifier lambda-list
	      lambda-list (pop body)))
      (multiple-value-setq (code decs doc)
	(parse-body body env))
      ;; Find the (first) specialized parameter in the lambda list,
      (do ((arguments lambda-list (cdr arguments))
	   (arg)
	   (i 0 (1+ i)))
	  ((or (endp arguments)
	       (member (setq arg (car arguments)) lambda-list-keywords))
	   (error "Method has no type"))
	(when (consp arg)
	  ;; Save the type and position of the specialized parameter
	  (setq type (second arg)
		n i
		args (copy-list lambda-list))
	  ;; Special case for EQL types
	  (when (and (consp type) (eq (car type) 'eql))
	    (unless (constantp (cadr type))
	      (error "Non-constant parameter specializers not supported"))
	    (setq type (list 'member (eval (cadr type)))))
	  ;; Form the method arglist
	  (setf (nth i args) (first arg))
	  (return nil)))
      ;; Generate code for method and generic method
      (let* ((code-name (intern (format nil "~@[~a-~]~a-~a-METHOD" qualifier name type)
				(symbol-package name)))
	     (methods (insert-method name type code-name qualifier)))
	`(progn
	   (eval-when (compile load eval) (setf (get ',name 'methods) ',methods))
	   ;; Define the method
	   (compiler-let (($method ',name)
			  ($type ',type)
			  ($lambda-list ',args))
	     ,(if (cdr methods) ;; If more than one (us)
		  `(defun ,code-name ,args ,@decs ,(nth n args) ,@code)
		`(defun ,name ,args ,@(and doc (list doc)) ,@decs ,(nth n args) ,@code)))
	   ;; Define the generic method
	   ,(if (cdr methods) ;; If more than one (us)
		`(inhibit-fdefine-warnings
		   ,(make-generic-function name n methods :documentation doc))
	      ;; Speed hack for when there's only one method
	      `(setf (symbol-function ',code-name) (symbol-function ',name)))
	   ',name)))))

(defun make-generic-function (name n methods &key documentation &aux no-otherwise)
  `(defun ,name (&rest args)
     ,@(and documentation (list documentation))
     (declare (type list args))
     (let ((discriminator (nth ,n args)))
       ;; It may be faster to do (case (type-of discriminator) ...)
       (,@(if (eq name 'initialize-instance)
	      ;; Speed hack for the methods that EVERY class has.
	      '(case #+explorer (si:array-leader discriminator 1)
		     #-explorer (type-of discriminator))
	    '(typecase discriminator))
	,@(do ((method methods (cdr method))
	       (result nil))
	      ((endp method) (nreverse result))
	    (let* ((class (caar method))
		   (main (getf (cdar method) nil))
		   (before (find-methods class :before method))
		   (after (find-methods class :after method)))
	      (when (or main before after)
		(unless main (setq main (first (find-methods class nil method))))
		(when (eq class 't) (setq no-otherwise t))
		(push `(,class
			,@(mapcar #'(lambda (method) `(apply (function ,method) args))
				  before)
			(multiple-value-prog1
			  ,(if main `(apply (function ,main) args)
			     `(error "Method ~s has no primary handler for ~s" ',name discriminator))
			  ,@(mapcar #'(lambda (method) `(apply (function ,method) args))
				    (nreverse after))))
		      result))))
	,@(unless no-otherwise
	    `((otherwise (error "Method ~s has no handler for ~s" ',name discriminator))))))))

(defun find-methods (class qualifier methods)
  (do ((method methods (cdr method))
       (result nil))
      ((endp method) (nreverse result))
    (when (or (eq class (caar method))
	      (subclass-p class (caar method)))
      (let ((name (getf (cdar method) qualifier)))
	(when name (push name result))))))

(defun insert-method (name class method qualifier)
  ;; The METHODS property of the method name contains an alist of the form:
  ;; (class qualifier method-name qualifier method-name ...)
  ;; The methods list is sorted by subtype, most specific first.
  (unless (member qualifier '(nil :before :after))
    (error "Method qualifier ~s not handled" qualifier))
  (let* ((list (copy-tree (get name 'methods)))
	 (old (assoc class list :test #'equal)))
    (if old
	(setf (getf (cdr old) qualifier) method)
      (do* ((new (list class qualifier method))
	    (previous nil entry)
	    (entry list (cdr entry)))
	   ((or (endp entry)
		(subclass-p class (caar entry)))
	    (if previous
		(setf (cdr previous)
		      (cons new entry))
	      (setq list (cons new list))))))
    list))

(defun delete-method (name class method qualifier)
  ;; The METHODS property of the method name contains an alist of the form:
  ;; (class qualifier method-name qualifier method-name ...)
  (unless (member qualifier '(nil :before :after))
    (error "Method qualifier ~s not handled" qualifier))
  (let* ((list (copy-tree (get name 'methods)))
	 (old (assoc class list)))
    (if old
	(progn
	  (remf (cdr old) qualifier)
	  (unless (cdr old)
	    (setq list (delete class list :key #'car))))
      (if (fboundp method)
	  (error "Method entry not found for ~s" method)
	(macro-warn "Warning: Method ~s ~:[~;~s ~]for class ~s not defined." name qualifier class)))
    list))

;; CLOS requires this to be a function
(defmacro call-next-method (&rest args)
  "Run the next method up in the type heiarchy.  Use ONLY within a DEFMETHOD.
 Returns the first value of the last method found."
  (declare (special $method $type $lambda-list))
  (unless (boundp '$method) (error "Call-Next-Method executed outside of a defmethod"))
  (let ((rest (member '&rest $lambda-list) ))
    (cond (args `(call-next-method-internal ',$type ',$method ,@args))
	  (rest					;Rest arg hair
	   `(apply #'call-next-method-internal ',$type ',$method
		   ,@(subseq $lambda-list 0 (- (length $lambda-list) (length rest))) ,(second rest)))
	  ((setq rest (member '&key $lambda-list))	;Keyword arg hair
	   `(call-next-method-internal ',$type ',$method
				       ,@(subseq $lambda-list 0 (- (length $lambda-list) (length rest)))
				       ,@(do* ((vars (cdr rest) (cdr vars))
					       (var (car vars) (car vars))
					       (result nil))
					     ((or (endp vars) (member var lambda-list-keywords))
					      (nreverse result))
					   (push (intern (string var) 'keyword) result)
					   (push var result))))
	  (t `(call-next-method-internal ',$type ',$method ,@$lambda-list)))))

(defun call-next-method-internal (type method &rest args)
  ;; Internal function to call METHOD of one of TYPE's parent types.
  ;; Returns the first value of the last method found.

  ;; The following DO is an in-line expansion of (member type (get method 'methods) :key #'car)
  (do ((methods (get method 'methods) (cdr methods)))
      ((or (endp methods) (eq (caar methods) type))
       (do ((next (cdr methods) (cdr next))
	    (function))
	   ((endp next))
	 (when (and (subclass-p type (caar next))
		    (setq function (getf (cdar next) nil)))
	   (return (apply function args)))))))

;; WARNING: Does direct substitution, not a code walk
(defmacro symbol-macrolet (varlist &body body)
  (labels ((subst-list (varlist s-exp)
	     (cond ((atom s-exp)
		    (or (second (assoc s-exp varlist :test #'eq))
			s-exp))
		   ;; Stop at quote and function
		   ((member (car s-exp) '(quote function)) s-exp)
		   (t (setq s-exp (copy-list s-exp))
		      (do ((s s-exp (cdr s))
			   (prev nil s))
			  ((endp s))
			(rplaca s (subst-list varlist (car s))))
		      s-exp))))
    `(progn ,@(subst-list varlist body))))
  
;; WARNING: Does direct substitution, not a code walk
(defmacro with-slots (slots instance &body body)
  (declare (special $type))
  (unless body
    (error "Not enough parameters to WITH-SLOTS"))
  (when (atom instance)
    (if (and (boundp '$type) $type)
	(setq instance `(the ,$type ,instance))
      (unless (boundp '$type)
	(macro-warn "Warning: WITH-SLOTS doesn't know the instance type, use (the TYPE ~s)" instance))))
  `(symbol-macrolet
     ,(mapcar #'(lambda (slot)
		  (if (atom slot)
		      `(,slot (slot-value ,instance ',slot))
		    `(,(first slot) (slot-value ,instance ',(second slot)))))
	      slots)
     ,@body))

(defmacro slot-value (object slot-name)
  "Get the SLOT-NAME value from object.
   OBJECT MUST BE OF THE FORM (the CLASS object)"
  (let (class slots)
    (if (and (consp object)
	     (eq (car object) 'the)
	     (setq class (find-class (second object)))
	     (third object)
	     (not (cdddr object))
	     (constantp slot-name))
	(progn
	  (setq slot-name (eval slot-name))
	  (setq slots (standard-class-slots class))
	  (unless (member slot-name slots)
	    (error "~s isn't a slot of ~s" slot-name (class-name class)))
	  `(,(intern (concatenate 'string (standard-class-prefix class)
				  (string slot-name))
		     (symbol-package (standard-class-allocator class)))
	    ,object))
      (progn
	(UNLESS class
	  (macro-warn "Warning: (slot-value ~a ~a) is slow - add (the TYPE ~a)"
		      object slot-name object))
	`(slow-slot-value ,object ,slot-name)))))

(defun slow-slot-value (object slot-name)
  (declare (inline find-class))
  (let* ((class (class-of object))
	 (reader (intern (concatenate 'string (standard-class-prefix class)
				      (symbol-name slot-name))
			 (symbol-package (class-name class)))))
    (funcall reader object)))

(defmacro slot-boundp (object slot-name)
  ;; Fake a slot-boundp macro.
  ;; Assume all nil slots are unbound.
  `(slot-value ,object ,slot-name))

(defun make-instance (class-name &rest initialize-keywords-and-values)
  (let* ((class (find-class class-name))
	 (allocator (standard-class-allocator class))
	 (instance (funcall allocator))
	 (initargs (default-initargs class initialize-keywords-and-values)))
    (apply 'initialize-instance instance initargs)
    instance))

;;-----------------------------------------------------------------------------

(defmacro undefmethod (name lambda-list &body body &environment env)
  (if (consp name)
      (progn
	(unless (eq (car name) 'setf)
	  (error "Invalid method name ~s" name))
	(let ((setfer (intern (concatenate 'string "SET-" (string (second name))))))
	  `(progn
	     (defsetf ,(second name) (foo) (bar) (declare (ignore foo bar))
		(error "No setf defined for ~s" ',(second name)))
	     (undefmethod ,setfer ,lambda-list ,@body))))
    (let (type n args code decs doc
	  (qualifier nil)
	  $method $type $lambda-list)
      (declare (special $method $type $lambda-list))
      (when (keywordp lambda-list)
	(setq qualifier lambda-list
	      lambda-list (pop body)))
      (multiple-value-setq (code decs doc)
	(parse-body body env))
      ;; Find the (first) specialized parameter in the lambda list,
      (do ((arguments lambda-list (cdr arguments))
	   (arg)
	   (i 0 (1+ i)))
	  ((or (endp arguments)
	       (member (setq arg (car arguments)) lambda-list-keywords))
	   (error "Method has no type"))
	(when (consp arg)
	  ;; Save the type and position of the specialized parameter
	  (setq type (second arg)
		n i
		args (copy-list lambda-list))
	  ;; Special case for EQL types
	  (when (and (consp type) (eq (car type) 'eql))
	    (unless (constantp (cadr type))
	      (error "Non-constant parameter specializers not supported"))
	    (setq type (list 'member (eval (cadr type)))))
	  ;; Form the method arglist
	  (setf (nth i args) (first arg))
	  (return nil)))
      ;; Generate code for method and generic method
      (let* ((code-name (intern (format nil "~@[~a-~]~a-~a-METHOD" qualifier name type)
				(symbol-package name)))
	     (methods (delete-method name type code-name qualifier)))
	`(progn
	   (eval-when (compile load eval) (setf (get ',name 'methods) ',methods))
	   ;; Define the method
	   (when (fboundp ',code-name)
	     (fmakunbound ',code-name))
	   ;; Define the generic method
	   ,(if methods
		`(inhibit-fdefine-warnings
		   ,(make-generic-function name n methods :documentation doc))
	      ;; Remove the generic method also
	      `(when (fboundp ',name) (fmakunbound ',name)))
	   ',name)))))
