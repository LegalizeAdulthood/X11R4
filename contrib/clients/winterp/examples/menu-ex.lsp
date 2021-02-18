; -*-Lisp-*- 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         menu-ex.lsp
; RCS:          $Header: menu-ex.lsp,v 1.1 89/11/25 04:00:26 mayer Exp $
; Description:  A demonstration of Motif pulldown menus.
; Author:       Allan Shepherd, HPLabs
; Created:      Fri Oct 20 17:19:28 1989
; Modified:     Sat Nov 25 01:14:08 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Function:         musicians
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun musicians ()
  (let*
      ((toplevel_w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
	      :XMN_GEOMETRY "=300x200+200+8"
	      :XMN_TITLE "Musicians"
	      :XMN_ICON_NAME "Music"
	      ))
       (main_w
	(send XM_MAIN_WINDOW_WIDGET_CLASS :new :managed "mw" toplevel_w
	      :xmn_main_window_margin_height 5
	      :xmn_main_window_margin_width 5
	      ))
       (menubar_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :menu_bar "rc" main_w
	      :xmn_num_columns 1
	      :xmn_packing :pack_column
	      :xmn_entry_border 0
	      ))
       (frame_w
	(send XM_FRAME_WIDGET_CLASS :new :managed "frame" main_w
	      ))
       (te_w
	(send XM_TEXT_WIDGET_CLASS  :new :managed "textedit" frame_w
	      :xmn_traversal_on :true
	      ))
       composers_pd_w
       pieces_pd_w
       )

    (send main_w :set_values
	  :xmn_menu_bar menubar_w
	  :xmn_work_window frame_w
	  )
    
    ;; LISTEN
    (setq composers_pd_w
	  (create-pulldown-menu menubar_w "Composers")
	  )
    (create-select-button composers_pd_w "Bach"
			  `(() ((play ,te_w "Bach")))
			  )
    (create-select-button composers_pd_w "Webern"
			  `(() ((play ,te_w "Webern")))
			  )
    ;; COMPOSE
    (setq pieces_pd_w
	  (create-pulldown-menu menubar_w "Pieces")
	  )
    (create-type-submenu pieces_pd_w "Start New Piece"
			 (preferred-keys)
			 `(compose-new-piece-in-key ,te_w))

    (create-select-button pieces_pd_w "Edit Piece" '(() (nil)))

    ;; etc
    (send toplevel_w :realize)
    "Musicians Example"
    )
  )



(defun preferred-keys ()
  (setq a  "A")
  (setq b-flat "B Flat")
  (setq c  "C Minor")
  '(a b-flat c)
  )

(defun compose-new-piece-in-key (text-edit-w sym)
  (send text-edit-w :set_string (type-title sym)))

(defun play (text-edit-w sound)
  (send text-edit-w :set_string
	(strcat "Imagine you are now listening to " sound)))

(defun type-title (type)  (eval type))

(defun create-pulldown-menu (pane-mgr title)
    (let*
	((pull (send XM_ROW_COLUMN_WIDGET_CLASS :new :pulldown_menu
		     "pull-it" pane-mgr
		     ))
	 (item-button
	  (send XM_CASCADE_BUTTON_WIDGET_CLASS :new :managed
		title pane-mgr
		:xmn_sub_menu_id pull))
	 )
      pull
      ))

(defun create-type-submenu (mgr title type-list select-cmd)
  (let ((submenu (create-pulldown-menu mgr title)))
    (create-type-buttons submenu type-list  select-cmd))
  )
(defun create-type-buttons (pane-mgr itype-list select-cmd)
  ;; itype-list is a list of symbols for which
  ;; the function type-title returns a print str for each symbol
  (do* ((type-list itype-list (cdr type-list))
	(ctype (car type-list) (car type-list))
	)
       ((null type-list) nil)
       (create-select-button pane-mgr
			     (type-title ctype)
			     `(() ((,(car select-cmd) ,(cadr select-cmd)
				    (quote ,ctype)))))
       ))

(defun create-select-button (pane-mgr title callback)
  (let ((push-gadg1 (send XM_PUSH_BUTTON_GADGET_CLASS :new :managed
			  title pane-mgr))
	)
    (if callback
	(send push-gadg1 :add_callback :XMN_ACTIVATE_CALLBACK
	      (car callback) (cadr callback))))

  )
    
;; -- end of menu-ex.lsp --
