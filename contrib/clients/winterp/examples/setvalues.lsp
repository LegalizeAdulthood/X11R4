; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         setvalues.lsp
; RCS:          $Header: setvalues.lsp,v 1.1 89/11/25 04:00:40 mayer Exp $
; Description:  Play around with interactively changing :SET_VALUES
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:57:21 1989
; Modified:     Sat Nov 25 01:58:42 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq top_w (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "top"))
(send top_w :set_values 
      :XMN_WIDTH 500
      :XMN_HEIGHT 500
      :XMN_X 512
      :XMN_Y 300)

(send top_w :set_values
      :XMN_GEOMETRY "=500x500+500+200"
      :XMN_TITLE "poipoiwefpoiwe"
      :XMN_ICON_NAME "bar")

(send top_w :realize)

(setq rc_w (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed top_w))
(send rc_w :set_values :XmN_num_Columns 3)

(send rc_w :set_values :XmN_entry_Border 4)

(setq sc_w (send XM_SCROLLED_WINDOW_WIDGET_CLASS :new :managed "scrl" rc_w))
(setq te_w (send XM_TEXT_WIDGET_CLASS :new :managed :scrolled "textedit" sc_w ))
(send te_w :set_values 
      :XmN_blink_Rate 200
      :XMN_EDIT_MODE :MULTI_LINE_EDIT)

(send te_w :manage)
(send te_w :set_values
      :XMN_HEIGHT 100
      :XMN_WIDTH  100
      :XMN_STRING "quackquackquack"
      :XMN_SHADOW_THICKNESS 5
      :XmN_margin_Width 10
      :XMN_MARGIN_HEIGHT 10)

(setq w (send XM_LABEL_WIDGET_CLASS :new :managed "sel" rc_w 
	      :XMN_BORDER_WIDTH 20
	      ))

(setq w (send XM_SCALE_WIDGET_CLASS :new :managed "sel 1" rc_w 
	      :XMN_BORDER_WIDTH 10
	      :XmN_scale_Height 100
	      :Xmn_scale_width 100
	      :XmN_height 100
	      :XmN_width 100))

(send w :set_values
      :XMN_BORDER_WIDTH 0
      :xmn_height 100
      :xmn_width 30
      :XmN_scale_Height 100
      :Xmn_scale_width 50)


(setq ar_w (send XM_ARROW_BUTTON_WIDGET_CLASS :new :managed rc_w))
(send ar_w :set_values
      :XMN_ARROW_DIRECTION :ARROW_RIGHT)


(setq fsb_w (send XM_FILE_SELECTION_BOX_WIDGET_CLASS :new :dialog 
		  :managed rc_w "fsb"))
(send fsb_w :set_values :XMN_OK_LABEL_STRING "quack")
(send fsb_w :set_values 
      :XmN_label_String  "quack"
      :XmN_dir_Spec  "foo")

(send fsb_w :unmanage)
(send fsb_w :manage)
