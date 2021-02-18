;;; -*- Mode:Lisp; Package:CLUE-EXAMPLES; Lowercase:T; Base:10; Syntax:Common-Lisp -*-
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
;;; ----------------------------------------------------------------------------
;;;  7/01/88	SLM	Created.
;;;  8/19/88	LGO	Replaced reference to managed-p with contact-state
;;;  8/19/88	LGO	Cleaned up implementation of graph-menu
;;;  8/19/88	LGO	Use :background keyword to set contact background
;;;  8/19/88	LGO	make moveoutline re-entrant, parent-relative, eliminate
;;;			use of a "state" slot, and initialize correctly.
;;;  8/23/88    SLM     Convert Zetalisp LOOP into Common Lisp DO and DOLIST.
;;;  8/23/88    SLM     Get rid of a bunch of debug statements, variables and routines.
;;;  8/23/88    SLM     Make VERTEX, EDGE, and GRAPH-COMPOSITE use resources for 
;;;                     various slot values.


(in-package 'clue-examples :use '(lisp xlib clos cluei))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Global variables


(defvar *graph-menu*)
(defvar *graph-command-menu-alist* '((change-name :title "Change Name")
				     (other-menu :title "other menu")))
(defvar *moveoutline* nil)



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Utility routines

;;; This function builds a closure; the closure variables record the size/location
;;; of how the outline box was last drawn.  The arguments to the closure are:
;;;  root - a window to draw on
;;;  X,Y  - the upper left corner of the outline box
;;;  width, heigth - the size of the outline box
;;; When the closure is called with a width or height of 0, then the previously drawn
;;; box is erased, and no additional box is drawn
(defun make-moveoutline (window)
  "Return a closure of four keyword arguments, x y width and height.  It draws two rectangles
one to erase the last rectangle drawn and one new rectangle.  The first time called it draws only one.
To only erase the last outline call it with width or height of 0."
  (let ((lastx 0)
	(lasty 0)
	(lastwidth 0)
	(lastheight 0)
	(outline (make-array 8 :initial-element 0 :fill-pointer 0))
	indx)
    #'(lambda (&key (x lastx) (y lasty) (width lastwidth) (height lastheight))
	(setq indx 0)
	(unless (and (= x lastx) (= y lasty) (= width lastwidth) (= height lastheight))
	  ;;when everything is zero, there's no rectangle to erase
	  (unless (or (zerop lastwidth) (zerop lastheight))
	    (setf (aref outline indx) lastx
		  (aref outline (1+ indx)) lasty
		  (aref outline (+ 2 indx)) lastwidth
		  (aref outline (+ 3 indx)) lastheight
		  indx (+ indx 4)))
	  ;;draw-rectangles has problems when the width or height is negative,
	  ;;so we'll "shift" the rectangle's origin so width and height are positive.
	  (if (minusp width)
	      (setf lastx (+ x width)
		    lastwidth (- width))
	      (setf lastx x
		    lastwidth width))
	  (if (minusp height)
	      (setf lasty (+ y height)
		    lastheight (- height))
	      (setf lasty y
		    lastheight height))
	  ;;when everything is zero, there's no new rectangle to draw
	  (unless (or (zerop lastwidth) (zerop lastheight))
	    (setf (aref outline indx) lastx
		  (aref outline (1+ indx)) lasty
		  (aref outline (+ 2 indx)) lastwidth
		  (aref outline (+ 3 indx)) lastheight
		  indx (+ indx 4)))
	  ;;don't make a request unless there's something to draw...
	  (when (> indx 0)
	    ;;...and only draw as many rectangles as you need
	    (setf (fill-pointer outline) indx)
	    (using-gcontext (xorgc :drawable window
				   :foreground 1 :background 0 :function boole-xor
				   :subwindow-mode :include-inferiors)
	      (draw-rectangles window xorgc outline)))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; VERTICES

(defcontact vertex (button)
  ((object :type t :accessor object :initform nil :initarg :object)
   (edges-in :type (or null list) :accessor edges-in :initform nil :initarg :edges-in)
   (edges-out :type (or null list) :accessor edges-out :initform nil :initarg :edges-out)
   (visited-p :type boolean :accessor visited-p :initform nil :initarg :visited-p)
   ;(undo-info :type list :accessor undo-info :initform nil)
   (name-function :type (or null function) :accessor name-function :initform nil :initarg :name-function)
   (ptr-rel-x :type integer :accessor :ptr-rel-x :initform 0)
   (ptr-rel-y :type integer :accessor :ptr-rel-y :initform 0)
   (foreground :type pixel :accessor :foreground
	       :initform 0 :initarg :foreground)
   ;(highlight-function :type keyword :accessor highlight-function :initform nil :initarg)
   (documentation :type t :accessor :documentation :initarg :documentation)
   (compress-exposures :initform t)
   )
  (:resources
    (border-width :initform 1)
    foreground
    (background :initform 1)
    (documentation :initform "Click left and drag to move this vertex")
    (font :initform "fg-18"))
  )


;;; When the button goes down start drawing the moving outline for the WM-FRAME.
(defmethod start-vertex ((mover vertex))
  (with-event ((pointer-x x) (pointer-y y))
    (with-slots (display parent x y ptr-rel-x ptr-rel-y width height) mover
      (setf ptr-rel-x (- pointer-x x)
	    ptr-rel-y (- pointer-y y))
      ;; draw the outline the first time
      (let ((*moveoutline* (make-moveoutline parent)))
	(funcall *moveoutline* :x x :y y :width width :height height)
	(with-mode (mover)
	  (catch 'done
	    (do ()
		(())
	      (process-next-event display))))))))

;;;--------------------------------------------
;;; This is the move action for the VERTEX
;;; Move the outline with the pointer
(defmethod move-vertex ((mover vertex))
  (declare (special *moveoutline*))
  (with-event ((pointer-x x) (pointer-y y))
    (with-slots (ptr-rel-x ptr-rel-y) mover
      (when *moveoutline*
	;; draw the outline
	(funcall *moveoutline* :x (- pointer-x ptr-rel-x) :y (- pointer-y ptr-rel-y))))))

;;;--------------------------------------------
;;; This is the button release action for VERTEX
;;; Erase the moving outline and move the vertex
(defmethod finish-vertex ((mover vertex))
  (declare (special *moveoutline*))
  (with-event ((pointer-x x) (pointer-y y))
    (with-slots (ptr-rel-x ptr-rel-y)
		mover
      ;; if moving, change the position of the frame,
      ;; otherwise make priority opposite what it is now
      (when *moveoutline*
	(funcall *moveoutline* :width 0)	;turn off the moving outline
	(move mover (- pointer-x ptr-rel-x) (- pointer-y ptr-rel-y))
	(dolist (edge (edges-in mover))
          (display edge 0 0 0 0 :calculate-p t))
	(dolist (edge (edges-out mover))
	  (display edge 0 0 0 0 :calculate-p t))
	(throw 'done nil)))))

;;;---------------------------------------------
;;; This is the right-click action for VERTEX.
;;; Put up a command menu of things that can be done with the vertex.

(defun graph-command-menu (x y parent )
  (if *graph-menu*  ;; First time, create the menu
    (add-mode *graph-menu* :exclusive 'ignore-action)
    (setf *graph-menu*
	  (menu-choose parent
		       *graph-command-menu-alist*
		       :handler 'graph-command-menu-handler
		       :multiple-p nil)))
  (change-priority *graph-menu* :above)
  (change-geometry *graph-menu* :x x :y y)
  (setf (contact-state *graph-menu*) :mapped))


(defun graph-command-menu-handler (selection)
  (delete-mode *graph-menu*)
  (setf (contact-state *graph-menu*) :withdrawn)
  #+explorer
  (case (car selection)
    (change-name (ticl:beep :doorbell))
    (other-menu (ticl:beep :zowie)))
  (case (car selection)
    (change-name (print "Change Name"))
    (other-menu (print "Other Menu"))))

(defmethod action-graph-command-menu ((mover vertex))
  (with-event (root-x root-y)
    (with-slots (parent) mover
      (graph-command-menu root-x root-y parent ))))

;;;;--------------------------------------------
;;;; This is the double-click action for VERTEX
;;;; Put up a command menu of things that can be done with the frame
;(defmethod xwm-command-menu ((mover vertex))
;  (with-event (root-x root-y)
;    (with-slots (parent) mover
;      (xwm-command-menu root-x root-y parent))))

;;;----------------------------------------------------------------------
;;; here are the event definitions for the VERTEX
(defevent vertex (:button-press   :button-1) start-vertex)
(defevent vertex (:motion-notify  :button-1) move-vertex)
(defevent vertex (:button-release :button-1) finish-vertex)
(defevent vertex (:button-press   :button-3) action-graph-command-menu   (action-display :highlight nil))
;;(defevent vertex (:button-press   :button-1 :double-click) xwm-command-menu)

(defmethod add-outgoing-edge ((self vertex) edge)
  (with-slots (edges-out) self
    (setf edges-out (pushnew edge edges-out))))


(defmethod add-incoming-edge ((self vertex) edge)
  (with-slots (edges-in) self
    (setf edges-in (pushnew edge edges-in))))



(defmethod midpoint ((self vertex) &key (side nil))
  (with-slots (x y width height) self
    (case side
      (:top (values (+ x (round (/ width 2.))) y))
      (:right (values (+ x width) (+ y (round (/ height 2.)))))
      (:bottom (values (+ x (round (/ width 2.))) (+ y height)))
      (:left (values x (+ y (round (/ height 2.)))))
      (otherwise (values (+ x (round (/ width 2.))) (+ y (round (/ height 2.))))))
    ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  EDGES

(defcontact edge (virtual)
  ((to-node :type t :accessor to-node :initform nil :initarg :to-node)
   (from-node :type t :accessor from-node :initform nil :initarg :from-node)
   (x1 :type integer :accessor x1 :initform 0 :initarg :x1)
   (y1 :type integer :accessor y1 :initform 0 :initarg :y1)
   (x2 :type integer :accessor x2 :initform 0 :initarg :x2)
   (y2 :type integer :accessor y2 :initform 0 :initarg :y2)
   (px :type integer :accessor px :initform 0 :initarg :px)
   (py :type integer :accessor py :initform 0 :initarg :py)
   (pname :type (or null string) :accessor pname :initform nil :initarg :pname)
   (visited-p :type boolean :accessor visited-p :initform nil :initarg :visited-p)
   (edge-type :type (or null keyword) :accessor edge-type :initform nil :initarg :edge-type)
   (text-font :type (or null font) :accessor text-font :initarg :text-font)
   (line-style :type (member :solid :dash :double-dash) :initarg :line-style)
   (line-width :type card16 :initarg :line-width)
   (foreground :type pixel :initform 0 :initarg :foreground)
   (background :type (or (member :none :parent-relative) pixel pixmap) :initform 1 :initarg :background)
;   (compress-exposures :type (member :on :off) :accessor compress-exposures :initform :on :initarg :compress-exposures)
   ;(undo-info :type list :accessor undo-info :initform nil)
   )
  (:resources
    x1 y1 x2 y2 px py
    background foreground
    (text-font :initform "fg-13")
    (line-style :initform :solid)
    (line-width :initform 3))
  )

(defmethod cluei:initialize-geometry ((edge edge))
  ;nuthin
  )
(defmethod cluei:initial-state-transition ((edge edge))
    "Return the old-state/new-state for the initial (setf contact-state) after edge
   is realized. Return nil if (setf contact-state) need not be called, i.e. no
   initial state transition is necessary."
  (with-slots (state) edge
    (when (eq :mapped state)
      (values :managed :mapped))))

(defmethod midpoint ((self edge) &key)
  (values (+ (x1 self) (round (/ (x2 self) 2.))) (+ (y1 self) (round (/ (y2 self) 2.)))))


(defmethod display ((self edge) &optional x y width height &key (calculate-p t))
  (declare (ignore x y width height))
  (with-slots (to-node from-node pname x1 y1 x2 y2 px py background foreground line-style line-width text-font parent) self
    (using-gcontext (gcontext :drawable parent :background background :foreground foreground
			      :line-style line-style :line-width line-width :font text-font)
      (when calculate-p
	(using-gcontext (gc :drawable parent :default gcontext
			    :foreground background :background foreground
			    :line-style line-style :line-width line-width
			    :font text-font)
	  (draw-line parent gc x1 y1 x2 y2)
	  (when pname
	    (draw-glyphs parent gc px py pname)))
	(multiple-value-bind (x y)
	    (midpoint from-node :side :right)
	  (setf x1 x y1 y))
	(multiple-value-bind (x y)
	    (midpoint to-node :side :left)
	  (setf x2 x y2 y))
	(when pname
	  (multiple-value-bind (x y)
	      (midpoint self)
	    (setf px x py y))
	  (setf px (- px (round (/ (text-width (gcontext-font gcontext) pname) 2.))))))
      (draw-line parent gcontext x1 y1 x2 y2)
      (when pname
	(draw-glyphs parent gcontext px py pname)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; GRAPH COMPOSITE

;;Has to be a VIRTUAL-COMPOSITE so virtual-children get the exposure events
(defcontact graph-composite (virtual-composite)
  ((x-spacing :type integer :accessor x-spacing :initform 20 :initarg :x-spacing)
   (y-spacing :type integer :accessor y-spacing :initform 8 :initarg :y-spacing)
   (x-margin :type integer :accessor x-margin :initform 5 :initarg :x-margin)
   (y-margin :type integer :accessor y-margin :initform 5 :initarg :y-margin)
   ;(undo-info :type list :accessor undo-info :initform nil :initarg :undo-info)
   (orientations :type list :accessor orientations :initform '(:horizontal :vertical) :initarg :orientations)
   (root-list :type list :accessor root-list :initform nil :initarg :root-list)
   (vertex-info-alist :type list :accessor vertex-info-alist :initform nil :initarg :vertex-info-alist)
   (edge-info-alist :type list :accessor edge-info-alist :initform nil :initarg :edge-info-alist)
   (foreground :type pixel :accessor foreground
	       :initform 0 :initarg :foreground)
   (documentation :type t :accessor :documentation :initarg :documentation)
   (compress-exposures :initform :on)
  )
  (:resources
    x-spacing y-spacing
    x-margin y-margin
    orientations
    foreground
    (background :initform 1)
    (documentation :initform "Press the Q key to exit the grapher"))
  )


(defmethod quit-graph ((graph graph-composite) &optional (tag 'quit-graph) value)
  (format t "~%~a ~a ~a" graph tag value)
  (throw tag value))


(defevent graph-composite (:key-press #\q) (quit-graph quit-graph "key-exit"))
(defevent graph-composite (:key-press #\Q) (quit-graph quit-graph "key-exit"))


(defmethod current-orientation ((self graph-composite))
  (car (orientations self)))


(defmethod toggle-orientation ((self graph-composite))
  (with-slots (x-spacing y-spacing orientations) self
    (psetf x-spacing y-spacing
	   y-spacing x-spacing
	   orientations (append (cdr orientations) (list (car orientations))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Access routines

(defmethod vertex-children ((self vertex) &optional (edge-type :all))
  (let (vertices)
    (dolist (edge (edges-out self) vertices)
      (if (atom edge-type)
	  (or (equal (edge-type edge) edge-type)
	      (eq edge-type :all))
	  (or (find (edge-type edge) edge-type)
	      (find :all edge-type)))
      (pushnew (to-node edge) vertices))
    ))

(defmethod vertex-parents ((self vertex) &optional (edge-type '(:all)))
  (let (vertices)
  (dolist (edge (edges-in self) vertices)
    (if (atom edge-type)
	(or (equal (edge-type edge) edge-type)
	    (eq edge-type :all))
	(or (find (edge-type edge) edge-type)
	    (find :all edge-type)))
    (pushnew (from-node edge) vertices))
  ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  Graph layout

(defmethod compute-in-x-direction ((self graph-composite) &optional
			       &key (node-list nil) (x-start nil) (edge-type t) (forget-p nil))
  "NODE-LIST the are nodes to place; X-START is the x pixel position to place the left-most edge;
EDGE-TYPE is a list of the edges to traverse when deciding which children to adjust as well, or T to mean all edges;
when FORGET-P is non-nil it means recompute the x co-ordinate even if the user has explicitly positioned
the vertex at that place."
  (declare (type list node-list)
	   (type (or null integer) x-start)
	   (type (or keyword string) edge-type)) 
  (when node-list
    (with-slots (x-spacing x-margin root-list) self
      (unless x-start (setq x-start x-margin))
      (unless node-list (setq node-list root-list))
      (let* ((max-x-pos 0) 
	     (children nil))
	(dolist (node node-list)
	  ;;Move the node if it has not already been visited, 
	  ;;and if the user did not explicitly place it somewhere
	  (when forget-p (setf (contact-state (the contact node)) nil))
	  (unless (or (visited-p node)
		      (not (managed-p node)))
	    (move node x-start (contact-y node))
	    (setf max-x-pos (max max-x-pos (+ x-start (contact-width node))))
	    (setf (visited-p node) t)))
	;;Find the next x starting position
	(setq max-x-pos (+ max-x-pos x-spacing))
	(dolist (node node-list)
	  (dolist (edge (edges-out node))
	    (when (and (not (visited-p edge))
		       (if (atom edge-type)
			   (or (equal (edge-type edge) edge-type) edge-type)
			   (member (edge-type edge) edge-type #'equal)))
	      (pushnew (to-node edge) children))
	    (setf (visited-p edge) t))
	  )
	(when children
	  (compute-in-x-direction self :node-list children :x-start max-x-pos :edge-type edge-type :forget-p forget-p))
	))))



(defmethod compute-in-y-direction ((self graph-composite) &optional
			       &key (node-list nil) (edge-type t) (y-start nil))
  "NODE-LIST are the nodes to place; EDGE-TYPE is a list of the edges to 
traverse when deciding which children to adjust as well, or T to mean all edges."
  (declare (type list node-list)
	   (type (or null integer) y-start)
	   (type (or keyword string) edge-type)) 
  (with-slots (y-spacing y-margin root-list) self
    (unless y-start (setq y-start y-margin))
    (unless node-list (setq node-list root-list))
    (let* ((accumulated-height 0)
	   (y-height 0)
	   (new-y-start y-start)
	   (vertex-children nil))
      ;;Edges and nodes that were visited by the X layout have been marked as visited.
      ;;As we touch nodes and edges THIS time, we UNmark them!!!
      ;;THEREFORE, move the node IFF it HAS already been visited, 
      ;;and if the user did not explicitly place it somewhere
      (dolist (node node-list) 
	(if (and (visited-p node) (managed-p node))
	    (progn 
	      (setf (visited-p node) nil)  ;;unset the node's flag!!!
	      (dolist (edge (edges-out node))
		(when (and (visited-p edge)
		       (if (atom edge-type)
			   (or (equal (edge-type edge) edge-type) edge-type)
			   (find (edge-type edge) (the list edge-type) #'equal)))
		    (pushnew (to-node edge) vertex-children))
		(setf (visited-p edge) nil))  ;;unset the edge's flag!!!
	      (if vertex-children
		  (setq y-height (compute-in-y-direction self :node-list vertex-children
							 :edge-type edge-type :y-start new-y-start))
		  (setq y-height (+ (* 2 (contact-border-width node)) (contact-height node)))) 
	      (move node (contact-x node) (max new-y-start
					       (- (+ new-y-start (round (/ y-height 2.)))
						  (round (/ (+ (* 2 (contact-border-width node)) (contact-height node)) 2.))))))
	    (setf (visited-p node) nil)) 
	(setf new-y-start (+ y-spacing y-height new-y-start)
	      accumulated-height (+ (+ y-spacing y-height) accumulated-height)))
      accumulated-height)))


(defmethod compute-graph-layout ((self graph-composite) &optional (node-list nil)) 
  (with-slots (root-list width height) self
    (compute-in-x-direction self :node-list (or node-list root-list))
    (compute-in-y-direction self :node-list (or node-list root-list))
    (display self 0 0 width height)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Initializations

(defmethod preprocess-edge-info ((self graph-composite) edge-info &aux (edge-names nil) association)
  "Hang the infromation in EDGE_INFOR on SELF.  Return the edge-names that were processed."
  (with-slots (edge-info-alist) self
    (dolist (stuff edge-info)
      (pushnew (car stuff) edge-names)
      (if (setq association (assoc (car stuff) edge-info-alist))
	  (setf edge-info-alist (cons stuff (delete association edge-info-alist)))
	  (setf edge-info-alist (cons stuff edge-info-alist)))
      ))
  edge-names)


(defmethod preprocess-vertex-info ((self graph-composite) vertex-info &aux association)
  "Hang the information in VERTEX-INFO on SELF.  No meaningful return"
  (with-slots (vertex-info-alist) self
    (dolist (stuff vertex-info)
      (if (setq association (assoc (car stuff) vertex-info-alist))
	  (setf vertex-info-alist (cons stuff (delete association vertex-info-alist)))
	  (setf vertex-info-alist (cons stuff vertex-info-alist)))
      )))


(defmethod vertex-font-function-info ((self graph-composite))
  "Return the font-function for vertices"
  (cadr (assoc :font-function (vertex-info-alist self))))


(defmethod vertex-background-info ((self graph-composite))
  "Return the background for vertices"
  (cadr (assoc :background (vertex-info-alist self))))


(defmethod vertex-foreground-info ((self graph-composite))
  "Return the foreground for vertices"
  (cadr (assoc :foreground (vertex-info-alist self))))


(defmethod edge-line-style-info ((self graph-composite) edge)
  "Return the line-style specification for an edge of type EDGE"
  (when (typep edge 'edge) (setq edge (edge-type edge)))
  (cadr (assoc :line-style (cdr (assoc edge (edge-info-alist self))))))


(defmethod edge-line-width-info ((self graph-composite) edge)
  "Return the line-width specification for an edge of type EDGE"
  (when (typep edge 'edge) (setq edge (edge-type edge)))
  (cadr (assoc :line-width (cdr (assoc edge (edge-info-alist self))))))


(defmethod edge-foreground-info ((self graph-composite) edge)
  "Return the line-foreground specification for an edge of type EDGE"
  (when (typep edge 'edge) (setq edge (edge-type edge)))
  (cadr (assoc :line-foreground (cdr (assoc edge (edge-info-alist self))))))


(defmethod edge-background-info ((self graph-composite) edge)
  "Return the line-background specification for an edge of type EDGE"
  (when (typep edge 'edge) (setq edge (edge-type edge)))
  (cadr (assoc :line-background (cdr (assoc edge (edge-info-alist self))))))


(defmethod edge-text-info ((self graph-composite) edge)
  "Return the font specification for an edge of type EDGE"
  (when (typep edge 'edge) (setq edge (edge-type edge)))
  (cadr (assoc :edge-font (cdr (assoc edge (edge-info-alist self))))))


(defmethod edge-parents-function ((self graph-composite) edge)
  "Return the function that should be used on a vertex's object to get the list of parent objects."
  (when (typep edge 'edge) (setq edge (edge-type edge)))
  (cadr (assoc :parents (cdr (assoc edge (edge-info-alist self))))))


(defmethod edge-children-function ((self graph-composite) edge)
  "Return the function that should be used on a vertex's object to get the list of child objects."
  (when (typep edge 'edge) (setq edge (edge-type edge)))
  (cadr (assoc :children (cdr (assoc edge (edge-info-alist self))))))


(defun find-all-objects (wind node-list edge-type &optional (return-list nil) (duplicate-exists-p nil))
  "Find all objects to be included in the graph, starting with NODE-LIST.
Use the edge-info stored in WIND for each edge-type in EDGE-TYPE to obtain the children to be included."
  (let* ()
    (dolist (node node-list)
      (if (find node return-list :test #'equal)
	  (push node duplicate-exists-p)
	  (push node return-list))
      (multiple-value-setq (return-list duplicate-exists-p)
	(find-all-objects wind (funcall (edge-children-function wind edge-type) node)
		       edge-type return-list duplicate-exists-p)))
    (values return-list duplicate-exists-p)))


(defun add-edges (parent-window node-list all-nodes edge-type
		  &optional
		  (edge-children-function nil)
		  (edge-line-style nil)
		  (edge-line-width nil)
		  (edge-foreground nil)
		  (edge-background nil)
		  (edge-text-font nil))
  "Add the edges for the graph to be displayed in PARENT-WINDOW.  NODE-LIST is a starting 
list of sibling vertex objects/names;  ALL-NODES is the list of all known vertex contacts 
in this graph; EDGE-TYPE is the name (usually a keyword) of edge type we want to add. The 
optional arg EDGE-CHILDREN-FUNCTION is the function to call on each node in NODE-LIST to 
obtain the children of this edge relationship (default function stored on parent-window)."
  (let (vertex-children
	edge
	(width (contact-width parent-window))
	(height (contact-height parent-window)))
    (unless edge-children-function (setq edge-children-function
					 (edge-children-function parent-window edge-type)))
    (dolist (node node-list)
      (setq vertex-children (funcall edge-children-function node)
	    node (cadr (assoc node all-nodes :test #'equal)))
      (dolist (child vertex-children)
	(setq child (cadr (assoc child all-nodes :test #'equal))
	      edge (make-contact 'edge :parent parent-window :edge-type edge-type
				 :width width :height height
				 :from-node node :to-node child
				 :background edge-background
				 :foreground edge-foreground
				 :line-style edge-line-style
				 :line-width edge-line-width
				 :text-font edge-text-font))
	(add-outgoing-edge node edge)
	(add-incoming-edge child edge))
      (when vertex-children (add-edges parent-window vertex-children all-nodes edge-type
				       edge-children-function edge-line-style edge-line-width
				       edge-foreground edge-background edge-text-font)))))


(defun create-nodes-n-edges (root-list wind &optional &key (node-name-fun 'lisp:string) (edge-types '(:isa)))
  (let* ((complete-node-list nil)
	 tnode
	 vnode
	 edge-line-style
	 edge-line-width
	 edge-foreground
	 edge-background
	 edge-text-font
	 edge-children-function
	 vertex-foreground
	 vertex-background
	 vertex-font-function)
    (dolist (edge-type edge-types)
      (setq complete-node-list (append (find-all-objects wind root-list edge-type) complete-node-list)))
    (setq complete-node-list (nreverse (remove-duplicates complete-node-list :from-end t))
	  vertex-foreground (vertex-foreground-info wind)
	  vertex-background (vertex-background-info wind)
	  vertex-font-function (vertex-font-function-info wind))
    (dotimes (n (length complete-node-list))
      (setf tnode (nth n complete-node-list)
	    vnode (make-contact 'vertex :parent wind :object tnode
				:title (or (and (stringp tnode) tnode)
					   (and (symbolp tnode) (string tnode))
					   (funcall node-name-fun tnode))
				:label-font (when vertex-font-function
						(funcall vertex-font-function tnode)) ;;else, use default
				:background vertex-background :foreground vertex-foreground
				:border vertex-foreground
				:x 0 :y 0)
	    complete-node-list (nsubstitute (list tnode vnode) tnode complete-node-list)))
    (dolist (edge-type edge-types) 
      (setq edge-line-style (edge-line-style-info wind edge-type) 
	    edge-line-width (edge-line-width-info wind edge-type)
	    edge-background (edge-background-info wind edge-type)
	    edge-foreground (edge-foreground-info wind edge-type)
	    edge-text-font (edge-text-info wind edge-type)
	    edge-children-function (edge-children-function wind edge-type))
      (add-edges wind root-list complete-node-list edge-type edge-children-function
		 edge-line-style edge-line-width edge-foreground edge-background edge-text-font))
    complete-node-list
    ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DRIVER

(defun nodes-n-edges-driver (display node-list &optional
			     &key (edge-info '((:isa (:children lisp:ignore)
						     (:parents lisp:ignore)
						     ;(:line-style :solid)
						     ;(:line-width 1)
						     ;(:foreground 0)
						     ;(:background 1)
						     )))
			     (vertex-info '((:border-width 1)
					    ;(:font-function nil)
					    ;(:foreground 0)
					    ;(:background 1)
					    ;(:edge-font "fg-13")
					    ))
			     graph-foreground 
			     graph-background)

  (let (wind all-nodes
	(xlib::*recursive-event-queue* nil)
	(*graph-menu* nil))
	 
    (unwind-protect
	(catch 'quit-graph
	(setf	wind (make-contact 'graph-composite :parent display  :x 0 :y 0 :width 600 :height 350
				   :background graph-background
				   :foreground graph-foreground
                                   :compress-exposures :on))
        (add-mode wind :exclusive 'ignore-action)
	  ;;:BACKGROUND 0 needs the logical name for WHITE
	  ;;(add-before-action display 'contact 'DESCRIBE)
	  (preprocess-vertex-info wind vertex-info)
	  (setq all-nodes (create-nodes-n-edges node-list wind
						:edge-types (preprocess-edge-info wind edge-info)))
	  (setf (root-list wind) (mapcar #'(lambda (x) (cadr (assoc x all-nodes))) node-list)) 
	  (setf (contact-state wind) :mapped)
	  (compute-graph-layout wind)
	  (DO ()
	      (())
	    (process-next-event display)))
      (and wind (destroy wind)))

    ))


