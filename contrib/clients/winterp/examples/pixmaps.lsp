; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         pixmaps.lsp
; RCS:          $Header: pixmaps.lsp,v 1.1 89/11/25 04:00:29 mayer Exp $
; Description:  Play around with pixmaps. These are just random individual forms
;               I eval'd to play around and test pixmaps, pixmap garbage
;               collection, image cacheing, etc.
;               Many of the pixmaps mentioned in this file do not exist on your
;               machine.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:44:36 1989
; Modified:     Sat Nov 25 01:50:53 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(progn
  (setq b1
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "background"  "white" "DarkSlateGrey")))
  (setq b2
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "25_foreground" "white" "DarkSlateGrey")))
  (setq b3
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "50_foreground"  "white" "DarkSlateGrey")))
  (setq b4
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "75_foreground"  "white" "DarkSlateGrey")))
  (setq b5
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "vertical" "white" "DarkSlateGrey")))
  (setq b6
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "horizontal"  "white" "DarkSlateGrey")))
  (setq b7
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "slant_right"  "white" "DarkSlateGrey")))
  (setq b8
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "slant_left" "white" "DarkSlateGrey")))
  (setq b9
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "menu_cascade" "white" "DarkSlateGrey")))
  (setq b10
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "menu_checkmark"  "white" "DarkSlateGrey")))
  )

(progn
  (send b1 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  (send b2 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  (send b3 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  (send b4 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  (send b5 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  (send b6 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  (send b7 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  (send b8 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  (send b9 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  (send b10 :set_values
	:XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "white" "DarkSlateGrey"))
  )

(progn
  (setq b1
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "background"))
  (setq b2
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "25_foreground" ))
  (setq b3
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "50_foreground"  ))
  (setq b4
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "75_foreground"  ))
  (setq b5
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "vertical" ))
  (setq b6
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "horizontal"  ))
  (setq b7
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "slant_right"  ))
  (setq b8
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "slant_left" ))
  (setq b9
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "menu_cascade" ))
  (setq b10
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP "menu_checkmark"  ))
  )





(setq pb_w (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm" "red" "blue")))
(send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob1.xbm" "red" "blue"))
(send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob2.xbm" "red" "blue"))
(send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob3.xbm" "red" "blue"))
(send XM_push_button_widget_CLASS :new :managed "foo" rc_w
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob4.xbm" "red" "blue"))
(send pb_w :set_values 
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP "/usr/local/mayer/src/bitmaps/bob0.xbm")






(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
  	    )
      )
(send pb_w :set_values 
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP "/usr/local/mayer/src/bitmaps/bob0.xbm"
      )
(gc)





(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP "/usr/local/mayer/src/bitmaps/bob4.xbm"
;	    :XMN_ARM_PIXMAP "/usr/local/mayer/src/bitmaps/bob2.xbm"
	    )
      )

(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP "/usr/local/mayer/src/bitmaps/bob3.xbm"
	    )
      )

(send pb_w :set_values 
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP "/usr/local/mayer/src/bitmaps/bob0.xbm"
      )

(setq bob1_pixmap (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm"
				 "white" "DarkSlateGrey"))

(setq pb_w (send xm_push_button_widget_class :new :managed rc_w
		 :XMN_LABEL_TYPE :PIXMAP
		 :XMN_LABEL_PIXMAP (xm_get_pixmap
				    "/usr/local/mayer/src/bitmaps/bob1.xbm"
				    "white" "DarkSlateGrey")
		 ))

(send pb_w :set_values 
      :XMN_LABEL_PIXMAP (xm_get_pixmap
			 "/usr/local/mayer/src/bitmaps/bob4.xbm"
			 "white" "DarkSlateGrey")
      )

(gc)

(setq bob1_pixmap nil)
(send pb_w :set_values 
      :XMN_LABEL_TYPE :PIXMAP
      :XMN_LABEL_PIXMAP bob1_pixmap
      )


(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP bob1_pixmap
	    )
      )
(setq bob1_pixmap (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob2.xbm"
				 "blue" "red"))

(gc)

(setq pb_w 
      (send XM_push_button_widget_CLASS :new :managed "foo" rc_w
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP bob1_pixmap
	    :XMN_ARM_PIXMAP   bob2_pixmap
	    )
      )


(setq rc (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
		       "rc"
		       (send (send TOP_LEVEL_SHELL_WIDGET_CLASS :new :XMN_GEOMETRY "500x500") :realize)
		       :xmn_orientation :vertical
		       :xmn_packing :pack_tight
		       :xmn_entry_alignment :alignment_center
		       :xmn_foreground "Black"
		       :xmn_background "LightGray"))

(setq to_w 
      (send XM_toggle_button_widget_CLASS :new :managed "foo" rc
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP "/usr/local/mayer/src/bitmaps/bob1.xbm"
	    :XMN_select_PIXMAP "/usr/local/mayer/src/bitmaps/bob2.xbm"
	    :XMN_select_insensitive_PIXMAP "/usr/local/mayer/src/bitmaps/bob3.xbm"
	    )
      )

(setq to_w 
      (send XM_toggle_button_widget_CLASS :new :managed "foo" rc
	    :XMN_LABEL_TYPE :PIXMAP
	    :XMN_LABEL_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob0.xbm"  "white" "DarkSlateGrey")
	    :XMN_select_PIXMAP (xm_get_pixmap "/usr/local/mayer/src/bitmaps/bob1.xbm"  "red" "blue")
	    )
      )





(send to_w :set_values 
	    :XMN_LABEL_PIXMAP "/usr/local/mayer/src/bitmaps/gnu.xbm"
	    :XMN_SELECT_PIXMAP "/usr/local/mayer/src/bitmaps/crowley.xbm"
	    )

(setq mbox_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :message_dialog top_w
	    :XMN_SYMBOL_PIXMAP "/usr/local/mayer/src/bitmaps/bob0.xbm"
	    ))



(xm_install_image
 (setq bob0_xi (xm_get_ximage_from_file "/usr/local/mayer/src/bitmaps/bob0.xbm"))
 "bob0")

(xm_install_image
 (setq bob1_xi (xm_get_ximage_from_file "/usr/local/mayer/src/bitmaps/bob1.xbm"))
 "bob1")

(xm_install_image
 (setq bob2_xi (xm_get_ximage_from_file "/usr/local/mayer/src/bitmaps/bob2.xbm"))
 "bob2")

(xm_install_image
 (setq bob3_xi (xm_get_ximage_from_file "/usr/local/mayer/src/bitmaps/bob3.xbm"))
 "bob3")

(xm_install_image
 (setq bob4_xi (xm_get_ximage_from_file "/usr/local/mayer/src/bitmaps/bob4.xbm"))
 "bob4")

(format T "~A, ~A, ~A, ~A, ~A\n" bob0_xi bob1_xi bob2_xi bob3_xi bob4_xi)

(progn
  (setq b1
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "bob0"  "white" "DarkSlateGrey")))
  (setq b2
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "bob1" "white" "DarkSlateGrey")))
  (setq b3
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "bob2"  "white" "DarkSlateGrey")))
  (setq b4
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "bob3"  "white" "DarkSlateGrey")))
  (setq b5
	(send xm_push_button_widget_class :new :managed rc_w
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap "bob4" "white" "DarkSlateGrey")))
  )

(xm_uninstall_image bob0_xi)
(xm_uninstall_image bob1_xi)
(xm_uninstall_image bob2_xi)
(xm_uninstall_image bob3_xi)
(xm_uninstall_image bob4_xi)




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; try out method :update_display in callback.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(setq pb_w (send xm_push_button_widget_class :new :managed rc_w
		 :XMN_LABEL_TYPE :PIXMAP
		 :XMN_LABEL_PIXMAP (xm_get_pixmap
				    "/usr/local/mayer/src/bitmaps/bob4.xbm"
				    "white" "green")
		 ))
(send pb_w :set_callback :XMN_ARM_CALLBACK '(callback_widget)
      '(
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-1")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-2")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-3")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-4")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-5")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-6")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values 
	      :XMN_LABEL_TYPE :STRING
	      :XMN_LABEL_STRING "WAITING-7")
	(send callback_widget :update_display)
	(system "sleep 1")
	(send callback_widget :set_values
	      :XMN_LABEL_TYPE :PIXMAP
	      :XMN_LABEL_PIXMAP (xm_get_pixmap
				 "/usr/local/mayer/src/bitmaps/bob4.xbm"
				 "white" "DarkSlateGrey"))
