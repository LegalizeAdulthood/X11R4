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

(unless (find-package 'clue)
  (warn "WARNING: CLUE must be loaded before building DEMO."))

(unless (find-package 'clue-examples)
  (warn "WARNING: EXAMPLES must be loaded before building DEMO."))

(defun compile-clue-demo (&optional (option :compile) directory)
  ;; Load CLUE-demo, optionally compiling changed files.
  ;; If OPTION is :RECOMPILE, recompile all files
  ;; If OPTION is :LOAD, don't compile anything, just load.
  ;; WARNING: CLX, CLUE and EXAMPLES MUST BE LOADED FIRST!!!
  (declare (type (or null string pathname) directory)
	   (type (or null (member :load :compile :recompile)) option))
  (unless directory
    (setq directory (or *clue-demo-directory* *default-pathname-defaults*)))
  (setq *clue-demo-directory* directory)	; Set defaults for the next time
  (unless (find-package 'clue)
    (error "CLUE must be loaded before building DEMO."))

  (flet ((module (file &optional opt dir)
	   (compile-load (merge-pathnames file (or dir directory)) (or opt option))))

    (module "mouse-doc")	;; pointer documentation window
    (module "menu-demo")	;; Simple menu demos
    (module "grapher")		;; tree displayer
    (module "graph-data")	;; Data for grapher
    (module "listener")		;; Lisp Listener
    (module "scroller")    ;; scrolling bitmap displayer
    (module "demo-all")		;; Menu-driven interface for above
    ))

(defun load-clue-demo (&optional directory)
  ;; Load CLUE
  ;; WARNING: CLX, CLUE and EXAMPLES MUST BE LOADED FIRST!!!
  (compile-clue-demo :load directory))

;;;-----------------------------------------------------------------------------
;;; DEFSYSTEM to make lispm users more comfortable

#+explorer
(defsystem clue-demo
  (:pathname-default "sys:clue.examples.old.demo;")
  #+pcl (:default-output-directory "sys:clue.examples.old.demo;")

  (:module mouse-doc "mouse-doc")
  (:module menu-demo "menu-demo")
  (:module layout "grapher")
  (:module gdata "graph-data")
  (:module listener "listener")
  (:module scroll "scroller")
  (:module driver "demo-all")
  (:module image "ti-logo.xbm")

  (:compile-load mouse-doc)
  (:compile-load menu-demo)
  (:compile-load layout)
  (:compile-load gdata (:fasload layout))
  (:compile-load listener)
  (:compile-load scroll)
  (:compile-load driver (:fasload mouse-doc menu-demo layout gdata listener))
  (:auxiliary image)
  )

#+symbolics
(defsystem clue-demo
  (:default-pathname "sys:clue.examples.old.demo;"
   :pretty-name "CLUE-DEMO"
   :bug-reports ("clue-bugs@dsg.csc.ti.com" "Report problems with CLUE.")
   )
  (:module layout ("grapher"))
  (:module mouse-doc ("mouse-doc"))
  (:module menu-demo ("menu-demo"))
  (:module gdata ("graph-data")
	   (:uses-definitions-from layout))
  (:module listener ("listener"))
  (:module scroll ("scroller"))
  (:module driver ("demo-all")
	   (:uses-definitions-from mouse-doc menu-demo gdata listener))
  (:module image ("ti-logo")
	   (:type :lisp-example))
  )


