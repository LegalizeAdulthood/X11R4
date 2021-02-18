; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         grep-br.lsp
; RCS:          $Header: grep-br.lsp,v 1.1 89/11/25 04:00:19 mayer Exp $
; Description:  A transformation of mail-br.lsp into a search browser using the
;               unix grep command. Evaluate the form '(grep-browser)' to bring
;               up the application.
; Author:       Niels Mayer, HPLabs
; Created:      Mon Nov 20 18:13:23 1989
; Modified:     Sat Nov 25 01:04:49 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;; Make a subclass of XM_LIST_WIDGET_CLASS which holds an additional
;; instance variable 'items'. 'items' is an array of arbitrary objects
;; (BROWSER_OBJECT) to be displayed in a browser made from the list widget.
;;
;; BROWSER-OBJECT can be any arbitrary xlisp object that responds to
;; the message :display_string.
;;
;; Message :display_string must return a string which is used as the
;; textual representation of the object in the browser display.
;;
(setq List_Browser 
      (send Class :new
	    '(items)			;new instance vars
	    '()				;no class vars
	    XM_LIST_WIDGET_CLASS))	;superclass

;;
;; We add a method to set the items browsed by the list browser
;; and set the 'items' instance variable.
;;
;; (send List_Browser :set_browser_items <items_list>)
;; <items_list> is a list of BROWSER_OBJECTs as described above.
;;
(send List_Browser :answer :SET_BROWSER_ITEMS '(items_list)
      '(
	(let* (
	       (items_end_idx (length items_list))
	       (display_items (make-array items_end_idx)))

	  ;; initialize the 'items' instance variable so that it
	  ;; holds all the BROWSER_OBJECTs passed in <items_list>
	  (setq items (make-array items_end_idx)) ;create the array
	  (do (				;copy elts from list to array
	       (i    0          (1+ i))
	       (elts items_list (cdr elts)))
	      ;; loop till no more elts
	      ((null elts))
	      ;; loop body
	      (setf (aref items i) (car elts))
	      (setf (aref display_items i) (send (car elts) :display_string))
	      )
	  ;; initialize the widget, passing in the browser items.
	  (send self :set_values
		:xmn_items display_items
		:xmn_item_count items_end_idx
		)
	  )
	)
      )


;;
;; Given a List Widget position, this returns the object associated
;; with that position. Note that the first item is at position 1.
;;
(send List_Browser :answer :GET_ITEM_AT_POSITION '(position)
      '(
	(aref items (1- position))
	))

;;
;; override methods on XM_LIST_WIDGET_CLASS so that they work properly
;; with the list browser. Note that all other list methods work fine
;; on the list browser
;;
(send List_Browser :answer :ADD_ITEM '(item position)
      '(
	(setq items (array-insert-pos items (1- position) item))
	(send-super :add_item (send item :display_string) position)
	)
      )

(send List_Browser :answer :ADD_ITEM_UNSELECTED '(item position)
      '(
	(setq items (array-insert-pos items (1- position) item))
	(send-super :add_item_unselected (send item :display_string) position)
	)
      )

(send List_Browser :answer :DELETE_ITEM '(item)
      '(
	;; this is too lame to implement... requires that we compare
	;; item with the result of :display_string done on every element
	;; of ivar 'items'
	(error "Message :DELETE_ITEM not supported in List_Browser")
	)
      )

(send List_Browser :answer :DELETE_POS '(position)
      '(
	(setq items (array-delete-pos items (1- position)))
	(send-super :delete_pos position)
	)
      )


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Define a BROWSER_OBJECT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Each BROWSER_OBJECT holds the information summarizing one mail message.
;; the information is split up into individual fields because we may want
;; to be able to sort on one field, or search for mathes on one field.
;;
(setq Grep_Item_Class
      (send Class :new
	    '(file-name line-num match-line)
	    ))

;; this method will read a single line of grep output.
;; and sets the instance variables in the 
;; BROWSER_OBJECT to the individual fields of the grep output
(send Grep_Item_Class :answer :read-grep-info '(pipe)
      '(
	(if (and
	     (setq file-name (fscanf-string pipe "%[^:]:"))
	     (setq line-num  (fscanf-fixnum pipe "%d:"))
	     (setq match-line (fscanf-string pipe "%[^\n]\n"))
	     )
	    self			;return self if succesful
	  NIL				;return NIL if hit EOF
	  )
	)
      )

(send Grep_Item_Class :answer :display_string '()
      '(
	(format nil "~A: ~A"
		file-name match-line)
	))


(send Grep_Item_Class :answer :file-name '()
      '(
	file-name
	))

(send Grep_Item_Class :answer :line-num '()
      '(
	line-num
	))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; This returns a list of Grep_Item_Class instances corresponding
;; to the items matching the search pattern and file list given
;; in argument <grep-arg-string>
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun grep (grep-arg-string)
  (do* 
   (;; loop variables, initializers, and increments.
    (fp (popen (strcat "grep -n "
		       grep-arg-string
		       " /dev/null")
	       :direction :input))
    (line (send (send Grep_Item_Class :new) :read-grep-info fp)
	  (send (send Grep_Item_Class :new) :read-grep-info fp))
    (result '())			;init to an empty list
    )
   ;; loop test and return
   ((null line)				; :read-grep-info returns NIL on EOF
    (pclose fp)				
    result				;return list of grep objects.
    )
   ;; loop body
   (setq result (nconc result (cons line NIL)))	;append grep-obj to list
   )
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; The Main program -- note that this doesn't use any global variables, so
;; you can have many grep browsers up all at once without having them
;; interact.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun grep-browser()

  (let (
	top_w paned_w controlpanel_w doit_button_w search_label_w
	search_editor_w files_label_w files_editor_w list_w filename_label_w
	viewtext_w
	)

    (setq top_w
	  (send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
		:XMN_TITLE "Grep Browser"
		:XMN_ICON_NAME "Grep Browser"
		))

    (setq paned_w
	  (send XM_PANED_WINDOW_WIDGET_CLASS :new :managed top_w
		))

    (setq controlpanel_w
	  (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed paned_w
		:XMN_ADJUST_LAST t
		:XMN_ORIENTATION :HORIZONTAL
		:XMN_PACKING :PACK_TIGHT
		:XMN_NUM_COLUMNS 1
		))

    (setq doit_button_w
	  (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed controlpanel_w
		:XMN_LABEL_STRING "DO SEARCH"
		))

    (setq search_label_w
	  (send XM_LABEL_WIDGET_CLASS :new :managed controlpanel_w
		:XMN_LABEL_STRING "Search for string:"
		))

    (setq search_editor_w
	  (send XM_TEXT_WIDGET_CLASS :new :managed controlpanel_w
		:XMN_EDIT_MODE :SINGLE_LINE_EDIT))

    (setq files_label_w
	  (send XM_LABEL_WIDGET_CLASS :new :managed controlpanel_w
		:XMN_LABEL_STRING "From Files:"
		))
      
    (setq files_editor_w
	  (send XM_TEXT_WIDGET_CLASS :new :managed controlpanel_w
		:XMN_EDIT_MODE :SINGLE_LINE_EDIT))

    (setq list_w
	  (send List_Browser :new :managed :scrolled "browser" paned_w
		:xmn_visible_item_count 20
		))

    (setq filename_label_w
	  (send XM_LABEL_WIDGET_CLASS :new :managed "label" paned_w
		:xmn_label_string "None"
		))

    (setq viewtext_w
	  (send XM_TEXT_WIDGET_CLASS :new :managed :scrolled "view" paned_w
		:XMN_EDIT_MODE :MULTI_LINE_EDIT
		:XMN_HEIGHT 400
		:XMN_EDITABLE nil	;don't allow user to change text.
		))

    (send top_w :realize)

    ;;
    ;; set contraint resources on controlpanel so that paned window
    ;; doesn't give it resize sashes.
    ;;
    (let (height)
      (send controlpanel_w :get_values :xmn_height 'height)
      (send controlpanel_w :set_values
	    :xmn_maximum height
	    :xmn_minimum height
	    )
      )

    ;;
    ;; set contraint resources on label widget so that paned window
    ;; doesn't give it resize sashes.
    ;;
    (let (height)
      (send filename_label_w :get_values :xmn_height 'height)
      (send filename_label_w :set_values
	    :xmn_maximum height
	    :xmn_minimum height
	    )
      )

    ;;
    ;; The doit_button initiates a grep search.
    ;;
    (send doit_button_w :add_callback :XMN_ARM_CALLBACK '()
	  `(
	    (send list_w :set_browser_items
		  (grep (strcat
			 "'"		;quotify string to protect from shell
			 (send ,search_editor_w :get_string) ;string to search for
			 "' "
			 (send ,files_editor_w :get_string)) ;wildcarded files
			))
	    ))

    ;;
    ;; set up a callback on the list widget initialized above such that
    ;; a double click on the browser-item will browse the object.
    ;;
    (send list_w :add_callback :xmn_default_action_callback
	  '(callback_item_position)
	  `(
	    (let* 
		((browsed-object
		  (send ,list_w :get_item_at_position callback_item_position))
		 (filename (send browsed-object :file-name))
		 (linenum (send browsed-object :line-num))
		 )
	      (send ,filename_label_w :set_values :xmn_label_string filename)
	      (send ,filename_label_w :update_display)	;incase reading file takes long time
	      (send ,viewtext_w :find_file filename linenum)
	      ))
	  )
    ))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Add a :FIND_FILE method to the Motif Text widget.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(send XM_TEXT_WIDGET_CLASS :answer :FIND_FILE '(filename linenum)
      '(
	(let*
	    (;; loc vars
	     (fp
	      (open filename :direction :input)
	      )
	     inspos
	     text_line
	     )

	  (if (null fp)
	      (error "Can't open file." filename))

	  (send self :set_string "")	;clear out old text
	  (send self :disable_redisplay NIL) ;don't show changes till done
	  (loop
	   (if (null (setq text_line (read-line fp)))
	       (return))
	   (setq inspos (send self :get_insertion_position))
	   (send self :replace inspos inspos (strcat text_line "\n"))
	   )

	  (send self :scroll linenum)	;make <linenum> be the top of screen

	  (send self :enable_redisplay) ;now show changes...

	  (close fp)
	  )
	)
      )
