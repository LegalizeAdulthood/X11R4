;;; -*- Mode:Lisp; Package:USER; Syntax:Common-Lisp; Base:10; Lowercase:T -*-

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

;;; Change History:
;;;
;;; ----------------------------------------------------------------------------
;;;  7/13/88   SLM   Created.

(in-package 'user)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Sample data for Graph Demo

(defstruct graph-sample
  (name nil)
  (parents nil)
  (children nil))

(defvar sample (make-graph-sample :name 'sample
				  :children '(first-child second-child)))
(defvar first-child (make-graph-sample :name 'first-child
				       :parents '(sample)
				       :children '(grandchild-a grandchild-b grandchild-c)))
(defvar second-child (make-graph-sample :name 'second-child
					:parents '(sample)
					:children '(grandchild-b grandchild-d)))
(defvar grandchild-a (make-graph-sample :name 'grandchild-a
				       :parents '(first-child)))
(defvar grandchild-b (make-graph-sample :name 'grandchild-b
				       :parents '(first-child second-child)))
(defvar grandchild-c (make-graph-sample :name 'grandchild-c
				       :parents '(first-child)))
(defvar grandchild-d (make-graph-sample :name 'grandchild-d
				       :parents '(second-child)))

(defun sample-children (struct)
  (if (symbolp struct)
      (graph-sample-children (eval struct))
      (graph-sample-children struct)))

(defun sample-parents (struct)
  (if (symbolp struct)
      (graph-sample-parents (eval struct))
      (graph-sample-parents struct)))

(defun simple-graph-demo (display)
  (clue-examples::nodes-n-edges-driver
    display
    '(sample)
    :edge-info '((:isa (:children sample-children)
		       (:parents sample-parents) ))
    :vertex-info '((:border-width 3))))

