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

(unless (find-package 'clue-examples)
  (make-package 'clue-examples
		:use '(lisp xlib clos cluei)))

(defun compile-clue-examples (&optional (option :compile) directory)
  ;; Load CLUE-examples, optionally compiling changed files.
  ;; If OPTION is :RECOMPILE, recompile all files
  ;; If OPTION is :LOAD, don't compile anything, just load.
  ;; WARNING: CLX and CLUE MUST BE LOADED FIRST!!!
  (declare (type (or null string pathname) directory)
	   (type (or null (member :load :compile :recompile)) option))
  (unless directory
    (setq directory (or *CLUE-EXAMPLES-directory* *default-pathname-defaults*)))
  (setq *CLUE-EXAMPLES-directory* directory)	; Set defaults for the next time
  (unless (find-package 'clue)
    (error "CLUE must be loaded before building EXAMPLES."))

  (flet ((module (file &optional opt dir)
	   (compile-load (merge-pathnames file (or dir directory)) (or opt option))))

    (module "button")		;; label and button contacts
    (module "menu")		;; menu contacts
    (module "valuators")	;; sliders
    (module "alt-menu")		;; Menus whose items aren't windows
    (module "rmenu")		;; Menu whose item list is a resource
    (module "scroll")     ;; wraps scroll bars onto a window
    ))

(defun load-clue-examples (&rest options)
  ;; Load CLUE
  ;; WARNING: CLX and CLUE MUST BE LOADED FIRST!!!
  (apply #'compile-clue-examples :load options))

;;;-----------------------------------------------------------------------------
;;; DEFSYSTEM to make lispm users more comfortable

#+explorer
(defsystem clue-examples
  (:pathname-default "sys:clue.examples.old;")
  #+pcl (:default-output-directory "sys:clue.examples.old;")

  (:module button "button")
  (:module menu "menu")
  (:module valuators "valuators")
  (:module virtual "alt-menu")
  (:module resource "rmenu")
  (:module scroll "scroll")

  (:compile-load button)
  (:compile-load menu	   (:fasload button))
  (:compile-load valuators (:fasload menu))
  (:compile-load virtual   (:fasload valuators))
  (:compile-load resource  (:fasload virtual))
  (:compile-load scroll    (:fasload resource))
  )

#+symbolics
(defsystem clue-examples
  (:default-pathname "clue:examples;"
   :bug-reports ("clue-bugs@dsg.csc.ti.com" "Report problems with CLUE.")
   ) 
  (:serial "button" "menu" "valuators" "alt-menu" "rmenu" "scroll")
  )







