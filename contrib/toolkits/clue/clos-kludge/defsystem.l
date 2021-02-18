;;; -*- Mode:Lisp; Package:USER; Syntax:COMMON-LISP; Base:10; Lowercase:T -*-

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

(in-package 'user)

(unless (find-package 'cluei)
  (make-package 'cluei :use '(lisp xlib)))

(unless (find-package 'xlib)
  #+explorer (IF (w:mouse-y-or-n-p "Package XLIB does not exist.  You must load CLX before continuing.
Should I load CLX for you?")
		 (MAKE-SYSTEM 'clx :noconfirm)
		 (ERROR "Load CLX first"))
  #-explorer (warn "WARNING: CLX must be loaded before CLOS-KLUDGE"))

(defun compile-clos-kludge (&optional (option :compile) directory)
  ;; Load CLUE, optionally compiling changed files.
  ;; If OPTION is :RECOMPILE, recompile all files
  ;; If OPTION is :LOAD, don't compile anything, just load.
  (declare (type (or string pathname) directory)
	   (type (or null (member :load :compile :recompile)) option)
	   (special *CLOS-KLUDGE-directory*))
  (unless directory
    (setq directory (if (boundp '*CLOS-KLUDGE-directory*)
			*CLOS-KLUDGE-directory*
		      *default-pathname-defaults*)))
  (setq *CLOS-KLUDGE-directory* directory)	; Set defaults for the next time
  (flet ((module (file &optional opt dir)
	   (compile-load (merge-pathnames file (or dir directory)) (or opt option))))

    (module "clos")
    (module "standard")
    (module "clos-patch")
    ))

(defun load-clos-kludge (&optional directory)
  (compile-clos-kludge :load directory))

#+explorer
(defsystem clos-kludge
  (:pathname-default "clue:clos-kludge;")

  (:module clos "clos")
  (:module standard-class "standard")
  (:module clos-patch "clos-patch")

  (:compile-load clos)
  (:compile-load standard-class (:fasload clos))
  (:compile-load clos-patch (:fasload clos standard-class))
  )

#+symbolics
(defsystem clos-kludge
  (:default-pathname "clue:clos-kludge;"
   :pretty-name "Clos Kludge"
   :bug-reports ("clue-bugs@dsg.csc.ti.com" "Report problems with CLUE.")
   )
   (:serial "clos" "standard" "clos-patch"))

