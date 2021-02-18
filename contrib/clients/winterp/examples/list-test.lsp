; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         list-test.lsp
; RCS:          $Header: list-test.lsp,v 1.1 89/11/25 04:00:23 mayer Exp $
; Description:  Various random experiments with the motif list widget. You
;               probably don't want to load this entire file. These forms
;               are best evaluated individually via winterp's gnuemacs interface
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:05:28 1989
; Modified:     Sat Nov 25 01:07:40 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(send XM_LIST_WIDGET_CLASS :new :managed :scrolled rc_w
      :xmn_selection_policy :browse_select
      :xmn_items #("foo" "bar" "baz" "bof" "boof" "frob" "snob" "blog")
      :xmn_item_count 8
      :xmn_visible_item_count 20
      )


(defun mh-scan (foldername msgs)
  (do* 
   ((fp (popen (strcat "scan -reverse -width 132 +" foldername " " msgs) 
	       :direction :input))
    (line (read-line fp) (read-line fp))
    (result (list line) (cons line result))
    )
   ((null line)
    (pclose fp)
    (cdr result)
    )
   )
  )

(setq length
      (length
       (setq list (mh-scan "inbox" "last:30"))
       ))


(setq list_w (send XM_LIST_WIDGET_CLASS :new :managed :scrolled rc_w
		   :xmn_width 500
		   :xmn_selection_policy :extended_select
		   :xmn_list_size_policy :constant
		   :xmn_scroll_bar_display_policy :static
		   :xmn_traversal_on T
		   :XMN_ITEMS list
		   :XMN_ITEM_COUNT length
		   :XMN_VISIBLE_ITEM_COUNT 20
		   ))

(send list_w :set_values 
      :XMN_ITEMS list
      :XMN_ITEM_COUNT length
      )

(send list_w :set_values 
      :XMN_SELECTED_ITEMS list
      :XMN_SELECTED_ITEM_COUNT 10
      )
(send list_w :set_values 
      :xmn_width 400
      )

(send list_w :set_values 

      )


(setq item (send list_w :ADD_ITEM (symbol-name (gensym)) 12))
(send list_w :ADD_ITEM_UNSELECTED (symbol-name (gensym)) 12)
(send list_w :DELETE_ITEM item)
(send list_w :DELETE_POS 9)
(send list_w :SELECT_ITEM item)
(send list_w :DESELECT_ITEM item)
(send list_w :SELECT_POS 2)
(send list_w :DESELECT_POS 2)
(send list_w :DESELECT_ALL_ITEMS)
(send list_w :SET_POS 3)
(send list_w :SET_BOTTOM_POS 10)
(send list_w :SET_ITEM item)
(send list_w :SET_BOTTOM_ITEM item)
(send list_w :ITEM_EXISTS "foo")
(send list_w :ITEM_EXISTS item)
(send list_w :SET_HORIZ_POS 0)

(setq cb (send list_w :set_callback :xmn_extended_selection_callback
	       '(
		 callback_widget
		 callback_reason 
		 callback_xevent
		 callback_item
		 callback_item_length
		 callback_item_position
		 callback_selected_items
		 callback_selected_item_count
		 callback_selection_type
		 )
	       '(
		 (format T "-------------------------------------------\n")
		 (format T "widget=~A, reason=~A, xevent=~A\n" 
			 callback_widget callback_reason callback_xevent)
		 (format T "LIST_ITEM=~A\n"
			 callback_item)
		 (format T "LIST_ITEM_LENGTH=~A\n"
			 callback_item_length)
		 (format T "LIST_ITEM_POSITION=~A\n"
			 callback_item_position)
		 (format T "LIST_SELECTED_ITEMS=~A\n"
			 callback_selected_items)
		 (format T "LIST_SELECTED_ITEM_COUNT=~A\n"
			 callback_selected_item_count)
		 (format T "LIST_SELECTION_TYPE=~A\n"
			 callback_selection_type)
		 )
	       ))
	       
(send list_w :remove_all_callbacks :xmn_extended_selection_callback)

(setq cb (send list_w :set_callback :xmn_default_action_callback
	       '(
		 callback_widget
		 callback_reason 
		 callback_xevent
		 callback_item
		 callback_item_length
		 callback_item_position
		 callback_selected_items
		 callback_selected_item_count
		 callback_selection_type
		 )
	       '(
		 (format T "-------------------------------------------\n")
		 (format T "widget=~A, reason=~A, xevent=~A\n" 
			 callback_widget callback_reason callback_xevent)
		 (format T "LIST_ITEM=~A\n"
			 callback_item)
		 (format T "LIST_ITEM_LENGTH=~A\n"
			 callback_item_length)
		 (format T "LIST_ITEM_POSITION=~A\n"
			 callback_item_position)
		 (format T "LIST_SELECTED_ITEMS=~A\n"
			 callback_selected_items)
		 (format T "LIST_SELECTED_ITEM_COUNT=~A\n"
			 callback_selected_item_count)
		 (format T "LIST_SELECTION_TYPE=~A\n"
			 callback_selection_type)
		 )
	       ))
	       


(send (get_moused_widget) :destroy)
(xt_remove_callback cb)

(gc)
