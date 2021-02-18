;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; TITLE:    DEFSTRCT.LSP
; VERSION:  1.0 (beta-test)
; DATE:     May 1988
; AUTHOR:   Roman M. Jansen-Winkeln
; LANGUAGE: XLISP 2.0
;
; DESCRIPTION:
; This is an implementation of STRUCTUREs as described in L. Steele's
; book on COMMON LISP. It is very simular to the structures, which are
; known from ZETA LISP or the MIT-Version for FRANZ LISP.
;
; I tried to realize most of the features of structures in this XLISP
; implementation. Two features are missing: user defined PRINT-functions
; and user customizable parameter lists for the structure constructor-
; function. I left out the first, because there is no easy (pure lisp)
; way to change the behavior of the lisp function PRINT. User defined
; parameter lists will be part of a future version (not because this
; is so difficult; I've forgotten to realize them).
;
; Although this program covers most of the standard features of structures,
; there are a lot of possiblities to extend it. One of these is the under-
; lying data structure. Only LISTs and VECTORs are supported now. An
; interesting point would be to use XLISP OBJECTS as data structures.
; Another interesting thing would be an integration of structures into
; lisp's data structures. In COMMON LISP most build-in functions like
; EQUAL or COPY works only very insufficiently on structures. The integration
; into PRINT with a user defined PRINT-FUNCTION leads the way to generic
; functions, which works on standard data structres as well as on user
; defined structures.
;
; A few things were not realized or implemented not correctly following
; the COMMON LISP standard:
; - <slot-desription> consists only of (<slot-name> <default-init>)
;   the <slot-options> :TYPE and :READ-ONLY were not implemented.
; - <options> within the <name-and-options>-part of DEFSTRUCT are NOT
;   in parentheses; (<name> :<opt1> <val1> :<opt2> <val2> ...)
; If you dislike these points, let me know. Perhaps I will do better
; next time.
;
; This file contains the following global functions:
; - global function DEFSTRUCT as described in most COMMON LISP books
; - global function DEFSTRUCT-NAME, for internal use only. You can hide
;   it with a FLET inside of DEFSTRUCT (sometimes I miss packages in XLISP)
; Every structure defined with DEFSTRUCT puts some information on a property
; list:
; - a property DEFSTRUCT-SLOTS for every symbol with the name of a
;   structure
; At the end of the file you can find a few examples for the definition
; and the usage of structures.
;
; I make no warranty with respect to this program and disclaim any
; suggestions of usefulness for any particular purpose. Use this program
; only if you are willing to assume all risks, and damages, if any.
;
; Roman M. Jansen-Winkeln          roman%sbsvax.uucp@unido.uucp
; Universitaet des Saarlandes      roman@sbsvax.informatik.uni-saarland.dbp.de
; D-6600 Saarbruecken
; West Germany
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defmacro defstruct
  (name &rest slots
	&aux slot-position-alist slotnames slotlength)

  (when (atom name) (setq name (list name)))

  (progv '(conc-name constructor copier predicate type named
		     initial-offset include)
	 (apply 'defstruct-name name)
	 ;;; the first argument of DEFSTRUCT - normalized to a list -
	 ;;; looks exactly like a list of actual parameters for a
	 ;;; support function. DEFSTRUCT-NAME is this function which
	 ;;; will be APPLIed on this list.
	 ;;; The result of DEFSTRUCT-NAME is a list of those values, which
	 ;;; are set by the structure descricption list. PROGV is used
	 ;;; as a replacement for MULTIPLE-VALUE-BIND. It should cause no
	 ;;; problem, that it binds the introduced variables dynamically
	 ;;; (Does it really do that ???).

	 (flet
	  ;;; There were several good reasons to use FLET here.
	  ;;; - These little functions make the following code more readable,
	  ;;; - The following functions are really private to DEFSTRUCT,
	  ;;; - FLET in combination with statical binding faciliates the
	  ;;;   use of global variables und minimizes the number of parameters
	  ;;;   without the risc of unwanted side effects
	  ;;; - DEFSTRUCT seems to be no time-critical function. FLET only
	  ;;;   slows down the definition of structures, not their access.

	  ((defstruct-constructor ()
		;;; generates the "make-..."-function
		`(defun ,constructor (&key ,@slots)
			,(case type
			       (vector `(vector ,@slotnames))
			       (list   `(list ,@slotnames)))))

	  (defstruct-copier ()
		;;; generates the "copy-..."-function
		`(defun ,copier (struct1)
			,(case type
			       (vector
				`(let ((struct2
					(make-array ,(length slotnames))))
				      (dotimes (n ,(length slotnames) struct2)
					       (setf (aref struct2 n)
						     (aref struct1 n)))))
			       (list
				`(mapcar #'(lambda (x) x) struct1)))))

	  (defstruct-predicate (name)
		;;; generates the "...-p"-function
		`(defun ,predicate (struct)
			,(case type
			       (vector `(and (arrayp struct)
					     (eq ',name (aref struct 0))))
			       (list   `(and (listp struct)
					     (eq ',name (car struct)))))))

	  (defstruct-accessor (x)
	    ;;; the slot access and manipulation functions "<str>-<slot>"
	    (let ((slot (car x)) (pos (cdr x)))
		 `(defmacro ,(intern (strcat conc-name (symbol-name slot)))
			    (struct)
			    ,(case type
				   (vector `(list 'aref struct ,pos))
				   (list `(list 'nth ,pos struct))))))
	)

	 ;; add all INCLUDEd slots to SLOTS and compute the SLOTNAMES
	 ;; finally store the SLOTS under propety DEFSTRUCT-SLOTS of NAME
	 ;; for future INCLUDEs of structure NAME
	 (if include
	     (setq slots (append (get include 'defstruct-slots) slots)))
	 (putprop (car name) slots 'defstruct-slots)

	 ;; now it's time to compute some local variables, like
	 ;; SLOT-POSITION-ALIST: alist with pairs of SLOT-NAME . POSITION,
	 ;; SLOTNAMES and SLOTLENGTH
	 (setq slot-position-alist
	       (do ((i 1 (1+ i))
		    (l1 slots (cdr l1))
		    (l2 nil) (slot nil))
		   ((null l1) l2)
		   (setq slot (car l1))
		   (setq l2 (cons (cons (if (atom slot) slot (first slot)) i)
				  l2)))
	       slotnames (reverse (mapcar #'car slot-position-alist))
	       slotlength (length slotnames))

	 ;; handle the INITIAL-OFFSET
	 (dotimes (n initial-offset)
		  (setq slotnames (cons nil slotnames)))

	 ;; if the structure is NAMED, add the NAME to SLOTNAMES
	 (if named (setq slotnames (cons `',(car name) slotnames)))

	 ;; generate the DEFUNs and DEFMACROs which will be evaluated
	 ;; as a side effect of DEFSTRUCT. Finally return the NAME of
	 ;; the structure.
	 `(progn
	   ,(when constructor (defstruct-constructor))
	   ,(when copier (defstruct-copier))
	   ,(when predicate (defstruct-predicate (car name)))
	   ,@(mapcar #'defstruct-accessor slot-position-alist)
	   ',(car name)))))

(defun defstruct-name
  ;; DEFSTRUCT-NAME interprets the global strructure options, which are
  ;; given together with the name of the structure
  (name &key (conc-name   (strcat (symbol-name name) "-"))
	     (constructor (intern (strcat "MAKE-" (symbol-name name))))
	     (copier	  (intern (strcat "COPY-" (symbol-name name))))
	     (predicate   (intern (strcat (symbol-name name) "-P")))
	     (type	  'vector)
	     (named	  t)
	     (initial-offset 0)
	     include print-function)

  ;;; test the CONC-NAME; set to "" if NIL, to the print-name if symbol
  (setq conc-name
	(cond ((null conc-name) "")
	      ((stringp conc-name) conc-name)
	      ((symbolp conc-name) (symbol-name conc-name))
	      (t (error "Illegal CONC-NAME:" conc-name))))

  ;;; COPIER, PREDICATE, INCLUDE and CONSTRUCTOR must be symbols
  (if (symbolp copier) copier (error "Illegal COPIER:" copier))
  (if (symbolp include) include (error "Illegal INCLUDE:" include))
  (if (symbolp predicate) predicate (error "Illegal PREDICATE:" predicate))
  (if (symbolp constructor) constructor
      (error "Illegal CONSTRUCTOR:" constructor))

  ;;; COPIER, PREDICATE, INCLUDE and CONSTRUCTOR must be symbols
  (if (integerp initial-offset) initial-offset
      (error "Illegal INITIAL-OFFSET:" initial-offset))

  ;;; test for PREDICATE and NAMED conflict
  (when (and predicate (not named))
	(error "Structures with PREDICATEs must be NAMED"))

  ;;; Only two TYPEs are valid (at the moment):
  (unless (member type '(vector list))
	  (error "Illegal TYPE of structure " (strcat name ": " type)))

  ;;; the print-function feature is not yet implemented. There is no
  ;;; hook for structures in XLISP's PRINT function.
  (when print-function
	(print "Warning: the PRINT-FUNCTION feature is not implemented."))

  ;;; result of defstruct-name is a list of all values
  (list conc-name constructor copier predicate
	type named initial-offset include))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; E X A M P L E S for S T R U C T U R E S
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defstruct
  (human :constructor new-human)
  (sex 'woman)
  age
  home)

(defstruct
  (student :include human)
  university science)

(defstruct adress
  (country 'germany)
  (city 'saarbruecken)
  street)

(setq roman (new-human :sex 'man
		       :age 28
		       :home (make-adress :street 'richard-wagner-str-68)))
(pprint roman)

(setq uni-sb (make-adress :city 'saarbruecken-11
			  :street 'im-stadtwald))
(setq anja (make-student :university uni-sb
			 :age 28))
(pprint anja)

(setf (student-science anja) 'biology)
(setf (human-home anja) (human-home roman))
;or (setf (student-home anja) (human-home roman))
;or (setf (student-home anja) (student-home roman))
(pprint anja)

(format t "(eq (student-home anja) (student-home roman))? ~A~%"
 	   (eq (student-home anja) (student-home roman)))

