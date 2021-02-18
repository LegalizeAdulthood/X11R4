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

(export '(*database*
	   convert
	   define-resources
	   undefine-resources
	   ))

(export '(default-resources ;; Debug aids
	   class-resources
	   describe-resource))

(defvar *database* (make-resource-database))

(defun describe-resource (name &optional (database *database*) (max-levels 32))
  "Describe the entries for NAME in DATA-BASE"
  (let ((name-vector (make-array max-levels))
	(tight-vector (make-array max-levels :element-type 'bit)))
    (labels ((dr-internal (name database level)
	       (when (eq name (xlib::resource-database-name database))
		 (format t "~%~10s (" (xlib::resource-database-value database))
		 (dotimes (i level)
		   (when (plusp i) (princ " "))
		   (when (zerop (aref tight-vector i)) (princ "* "))
		   (princ (aref name-vector i)))
		 (princ ")"))
	       (when (xlib::resource-database-tight database)
		 (dolist (tight (xlib::resource-database-tight database))
		   (setf (aref name-vector level) (xlib::resource-database-name tight)
			 (aref tight-vector level) 1)
		   (dr-internal name tight (1+ level))))
	       (when (xlib::resource-database-loose database)
		 (dolist (loose (xlib::resource-database-loose database))
		   (setf (aref name-vector level) (xlib::resource-database-name loose)
			 (aref tight-vector level) 0)
		   (dr-internal name loose (1+ level))))))
      (format t "~%Value      Name")
      (dr-internal (resource-key name) database 0))))

(defun default-resources (contact resource-class &optional resource-name)
  "Return the plist of default resources for RESOURCE-CLASS on CONTACT.
   If CONTACT is a string, its taken as a host name."
  (if (stringp contact)
      (let ((display (open-contact-display contact)))
	(unwind-protect
	    (default-resources (display-root display) resource-class resource-name)
	  (close-display display)))
      
      (let* ((contact-class (class-name (class-of contact)))
	     (class-key     (intern (symbol-name resource-class) 'keyword))
	     (name-key      (when resource-name (intern (symbol-name resource-name) 'keyword)))

	     ;; Find resource(s) that match given RESOURCE-CLASS and RESOURCE-NAME
	     (resources     (delete
			      nil
			      (append (class-resources   contact-class t)
				      (class-constraints contact-class t))
			      :key #'(lambda(r)
				       (let ((rclass (getf (rest r) :class))
					     (rname  (first r)))
					 (and
					   (eq (or rclass rname) class-key)
					   (or (null resource-name)
					       (eq rname name-key))))))))
	
	(when resources
	  (get-resources
	    nil
	    resources
	    contact
	    (append (contact-complete-name contact)  (list (or resource-name resource-class)))
	    (append (contact-complete-class contact) (list resource-class)))))))

(defun class-resources (class &optional full-p)
  "Return the resource list for CLASS.
  When full-p return the full alist."
  (let ((resources (clue-resources class)))
    (unless resources
      (error "~s isn't a CLUE class." class))
    (if full-p
	resources
      (mapcar #'car resources))))

(defun get-resources (arglist resources parent full-name full-class)	
  ;; Useful for making init-plists for contacts
  ;; arglist   Specifies  the  ArgList  to   override   resources
  ;;	   obtained from the resource database.
  ;; parent Specifies the  parent contact
  ;; full-name Specifies the name of this contact (may be overrid-
  ;;	   den by the arglist).
  ;; full-class Specifies the class of this contact.
  
  (declare (type list arglist)
	   (type contact parent)
	   (type list full-name full-class)
	   (values (list values) full-name full-type))
  
  (do* ((table (get-search-table *database* full-name full-class))
	(resources resources (cdr resources))
	(resource (caar resources) (caar resources))
	(value-type nil)
	(arg nil)
	(result nil))

       ((endp resources) result)
    
    (setq value-type (getf (cdar resources) :type))
    (if (setq arg (getf arglist resource))

	(when value-type
	  (let ((carg (convert parent arg value-type)))
	    (if (or carg (null arg))
		(setq arg carg)
		(error "The ~s initialization is ~s which isn't type ~s"
		       resource arg value-type))))
	
	(let ((value (get-search-resource table resource (getf (cdar resources) :class resource)))
	      (db nil))
	  
	  (if value

	      ;; Resource in the database
	      (when (and (setq arg value) value-type)
		(let ((carg (convert parent value value-type)))
		  (if (or carg (null arg))
		      (setq arg carg)
		      (error "The resource value for ~s is ~s which isn't type ~s"
			     (reverse (cons resource db)) value value-type))))
	      
	      ;; Resource NOT in the database
	      (let ((initform (getf (cdar resources) :initform)))
		(when initform			; Resource has an initform
		  (setq arg (eval initform))	;************ EVAL ALERT *********
		  (when value-type
		    (let ((carg (convert parent arg value-type)))
		      (if (or carg (null arg))
			  (setq arg carg)
			  (error "The ~s initialization has :initform ~s which evaluates to ~s which isn't type ~s"
				 resource initform arg value-type)))))))))
    (when arg
      (setq result (list* resource arg result)))))



(defun resource (contact name)
  "Lookup resource NAME for CONTACT"
  (getf (slot-value (the contact contact) 'initialization) name))

(defun get-clue-resource-internal (contact name class)
  (let ((initialization (slot-value (the contact contact) 'initialization)))
    (or (getf initialization name)
	(get-search-resource (second initialization) name (or class name)))))

;; This :argument-precedence-order makes things more efficient.
;; Remove condition when PCL fully supports defgeneric.
#+(and clos (not pcl))
(defgeneric convert (contact value type)
       (:argument-precedence-order type contact value))

;; The default method
(defmethod convert (contact value (type t))
  "Convert VALUE to TYPE"
  (cond ((and (consp type) (eq (car type) 'or))	; OR type
	 (dolist (typ (cdr type))		; Use the first conversion that works
	   (if (eq typ 'null)
	       (when (null value) (return nil))
	     (let ((result (convert contact value typ)))
	       (when result
		 (return result))))))

	((and (consp type) (eq (car type) 'member))	; MEMBER type
	 (unless (keywordp value)
	   (setq value (convert contact value 'keyword)))
	 (and (member value (cdr type) :test #'eq) value))

	((typep value type) value)		; If type works, use it!

	((or (stringp value)			; Last resort, try read-from-string
	     (symbolp value))
	 (let ((value (string value))
	       (*read-base* 10.)
	       (eof '#.(gensym)))
	   (multiple-value-bind (result index)
	       (read-from-string value nil eof)
	     (and (= index (length value))
		  (not (eq result eof))
		  (typep result type)
		  result))))
	(t nil)))

(defmethod convert (contact value (type (eql 'keyword)))
  (declare (ignore contact))
  (typecase value
    (keyword value)
    (symbol (intern (symbol-name value) 'keyword))
    (string
     (unless (position #\space (the string value))
       (intern (string-upcase value) 'keyword)))
    (otherwise nil)))

(defmethod convert (contact value (type (eql 'pixel)))
  (typecase value
    (stringable					; a color name
     (when (symbolp value) (setq value (symbol-name value)))
     (let ((screen (contact-screen contact)))
       (cond ((equalp value "WHITE") (screen-white-pixel screen))
	     ((equalp value "BLACK") (screen-black-pixel screen))
	     (t (let* ((colormap (screen-default-colormap screen))
		       (cache (getf (screen-plist screen) :color-cache))
		       (pixel (cdr (assoc value cache :test #'equalp))))
		  (or pixel
		      (progn
			(when (setf pixel (#+lispm si:ignore-errors ;; Color may not be found
					   #-lispm progn 
					    (alloc-color colormap value)))
			  (setf (getf (screen-plist screen) :color-cache)
				(cons (cons value pixel) cache)))
			pixel)))))))
    (pixel value)
    (otherwise nil)))

(defmethod convert (contact value (type (eql 'color)))
  (typecase value
    (stringable				   ; a color name
     (lookup-color (screen-default-colormap (contact-screen contact)) value)
     ;; Don't use window-colormap, because that causes a server round-trip.
     ;; (lookup-color (window-colormap contact) value)
     )
    (color value)
    (otherwise nil)))

(defmethod convert (contact value (type (eql 'font)))
  (typecase value
    (stringable				   ; a color name
     (open-font (contact-display contact) value))
    (font value)
    (otherwise nil)))
     
(defmethod convert (contact value (type (eql 'pixmap)))
  (typecase value
    (stringable
     (let ((image (convert contact value 'image)))
       (and image (get-pixmap contact image))))
    ((or (rational 0 1)
	 (float 0.0 1.0))
     (let ((gray (svref '#(0%gray 6%gray 12%gray 25%gray 37%gray 50%gray 62%gray 75%gray 88%gray 93%gray 100%gray)
			(round (* value 10)))))
       (and gray (boundp gray) (get-pixmap contact (symbol-value gray)))))
    (image (get-pixmap contact value))
    (pixmap value)
    (otherwise nil)))

(defmethod convert (contact value (type (eql 'image)))
  (declare (ignore contact))
  (declare (special cluei::*bitmap-images*))
  (typecase value
    (stringable
     (let ((symbol (find value cluei::*bitmap-images* :test #'xlib::stringable-equal)))
       (and symbol
	    (boundp symbol)
	    (typep (symbol-value symbol) 'image)
	    (symbol-value symbol))))
    (image value)
    (otherwise nil)))

(defmethod convert (contact value (type (eql 'cursor)))
  (declare (special *cursor-names*))
  (typecase value
    (card8 (get-cursor contact value))
    (stringable
     (let ((entry (assoc (string value) *cursor-names* :test #'string-equal)))
       (cond (entry
	      (get-cursor contact (second entry)))
	     ((and (stringp value)
		   (setq entry (parse-integer value :junk-allowed t)))
	      (get-cursor contact entry)))))
    (cursor value)
    (otherwise nil)))

(defmethod convert (contact value (type (eql 'boolean)))
  (declare (ignore contact))
  (typecase value
    ((member t nil) value)
    (stringable
     (member value '(:yes :on :true t) :test #'xlib::stringable-equal))
    (otherwise nil)))

(defmethod convert (contact value (type (eql 'stringable)))
  (declare (ignore contact))
  (typecase value
    (stringable value)
    (otherwise
     (princ-to-string value))))

(defmethod convert (contact value (type (eql 'string)))
  (declare (ignore contact))
  (typecase value
    (string value)
    (symbol (symbol-name value))
    (otherwise
     (princ-to-string value))))

(defmacro define-resources (&body name-value-pairs)
  "Sugar coating for xlib:add-resource"
  `(progn ,@(do* ((name-values name-value-pairs (cddr name-values))
		  (result nil))
		 ((endp name-values) (nreverse result))
	      (push `(add-resource *database* ',(first name-values) ,(second name-values)) result))))

(defmacro undefine-resources (&body name-value-pairs)
  "Sugar coating for xlib:delete-resource"
  `(progn ,@(do* ((name-values name-value-pairs (cddr name-values))
		  (result nil))
		 ((endp name-values) (nreverse result))
	      (push `(delete-resource *database* ',(first name-values)) result))))


;;;-----------------------------------------------------------------------------

#| ;; Test code
(setq *database* (make-resource-database))
(define-resources
  clue:(xmail toc button border) 3
  clue:(button font) 'helv10
  clue:(xmail background) 'red
  clue:(button background) 'blue
  clue:(button color) 'green
  clue:(xmail buttonbox color) 'gray
  clue:(xmail buttonbox font) 'fg-18
  clue:(xmail buttonbox foreground) 'red
  clue:(xmail buttonbox background) 'blue
  clue:(xmail buttonbox button color) 'gray
  clue:(application color) 'gray
  clue:(xmail toc button active-foreground) 'black1
  clue:(xmail toc buttonbox button  active-foreground) 'black2
  clue:(xmail toc button    include active-foreground) 'bblack
  clue:(* contact active-foreground) 'ablack
  )

(get-resource *database* :active-foreground 'color
	      'clue:(xmail       toc         button    include)
	      'clue:(application panelwindow buttonbox button))

(get-resource *database* :active-foreground 'color
	      'clue:(include button toc xmail)
	      'clue:(button buttonbox panelwindow application) t)

(SETQ vlist (MAKE-ARRAY 20 :fill-pointer 0 :adjustable t))
(get-search-table
  *database*
  'clue:(xmail       toc)
  'clue:(application buttonbox)
  vlist)

(get-search-resource vlist :foreground 'color)
(get-search-resource vlist :background 'color)

(DEFMACRO define-c-resources (STRING)
  `(WITH-INPUT-FROM-STRING (STREAM ,string)
     (read-resources *database* stream)))

(define-c-resources
  "xmail*toc*button*border: 3
  button*font: helv10
  xmail*background: red
  button*background: blue
  button*color: green
  (xmail*buttonbox*color: gray
  xmail*buttonbox*button*color: gray
  application*color: gray
  xmail*toc*button*active-foreground: black
  xmail*toc*buttonbox*button*active-foreground: black
  xmail*toc*button*include*active-foreground: bblack")

|#
