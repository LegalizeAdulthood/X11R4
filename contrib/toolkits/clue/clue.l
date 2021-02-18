;;; -*- Mode:Lisp; Package:USER; Syntax:COMMON-LISP; Base:10; Lowercase:T -*-

;;; Make the CLUE and CLUEI packages

;;;
;;;			 TEXAS INSTRUMENTS INCORPORATED
;;;				  P.O. BOX 149149
;;;			       AUSTIN, TEXAS 78714-9149
;;;
;;; Copyright (C)1988-1990 Texas Instruments Incorporated.
;;;
;;; Permission is granted to any individual or institution to use, copy, modify,
;;; and distribute this software, provided that this complete copyright and
;;; permission notice is maintained, intact, in all copies and supporting
;;; documentation.
;;;
;;; Texas Instruments Incorporated provides this software "as is" without
;;; express or implied warranty.
;;;

(in-package "USER")

(unless (find-package 'xlib)
  #-explorer
  (error "CLX must be loaded before making CLUE.")
  #+explorer
  (make-system 'clx :noconfirm))

;;
;; Make packages
;;
;;    The intrinsics are in a seperate package
(unless (find-package 'cluei)
  (make-package 'cluei
		:use '(lisp xlib)))

;;    Contacts go in clue:
(unless (find-package 'clue)
  (make-package 'clue
		:use '(lisp xlib cluei)))

;;
;; Ensure CLUE and CLUEI use a CLOS
;;
(cond ((find-symbol "DEFCLASS" 'cluei)) ;; clos-kludge must be around      

      ((find-package 'clos)
       (USE-PACKAGE 'clos 'cluei)
       (USE-PACKAGE 'clos 'clue))

      ((find-package 'pcl)
       (USE-PACKAGE 'pcl 'cluei)
       (USE-PACKAGE 'pcl 'clue))

      (t (error "CLOS must be loaded before making CLUE.")))
