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

(in-package 'cluei :use '(lisp xlib))

;;;
;;; Define the standard class
;;;
;;;  This is a seperate file, because the CLOS file must be loaded in order to
;;;  compile this one.
;;;

(eval-when (compile load eval)
(setf (get 'standard-class *class-property*)
      (internal-initialize-standard-class
	:name 'standard-class
	:slots '(name superclass-names slots variables prefix allocator default-initargs documentation metaclass)
	:prefix "STANDARD-CLASS-"
	:allocator 'internal-initialize-standard-class
	:documentation "The standard CLOS class"))
) ;; end eval-when

;; ****** KLUDGE to get around the lack of :around methods *******
(defmethod initialize-instance-after ((instance t) &rest args)
  ;; Dummy function
  (ignore instance args))

(defmethod initialize-instance ((class standard-class) &key name superclass-names slots variables
				prefix allocator default-initargs documentation metaclass &allow-other-keys)
  (macrolet ((init (slot &optional (value slot))
		   `(when ,slot (setf (slot-value (the standard-class class) ',slot) ,value))))
    (init name)
    (init superclass-names)
    (init slots)
    (init variables)
    (init prefix)
    (init allocator)
    (init default-initargs)
    (init documentation (car documentation))
    (init metaclass (car metaclass))))

(defmethod default-initargs ((class standard-class) initargs)
  initargs)


;;; Printing routines.

#-lispm
(defmethod print-object ((instance t) stream)
  (format stream "#<~a>" (type-of instance)))

#+lispm
(defmethod print-object ((instance t) stream)
  (si:printing-random-object (instance stream :typep)))

