; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         dialogshel.lsp
; RCS:          $Header: dialogshel.lsp,v 1.1 89/11/25 04:00:16 mayer Exp $
; Description:  test out motif dialog shells
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:54:12 1989
; Modified:     Sat Nov 25 01:55:49 1989 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r4 contrib tape release
;
; (c) Copyright 1989, Hewlett-Packard Company.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq fsb_w
      (send XM_FILE_SELECTION_BOX_WIDGET_CLASS :new :managed :dialog top_w))
(setq form_w
      (send XM_FORM_WIDGET_CLASS :new :managed :dialog top_w))
(setq mbox_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed top_w))
(setq md_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :message_dialog top_w))
(setq ed_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :error_dialog top_w))
(setq id_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :information_dialog top_w))
(setq qd_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :question_dialog top_w))
(setq wrnd_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :warning_dialog top_w))
(setq wrkd_w
      (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed :working_dialog top_w))

(send fsb_w :unmanage)
(send md_w :unmanage)
(send ed_w :unmanage)
(send id_w :unmanage)
(send qd_w :unmanage)
(send wrnd_w :unmanage)
(send wrkd_w :unmanage)

(send fsb_w :manage)
(send md_w :manage)
(send ed_w :manage)
(send id_w :manage)
(send qd_w :manage)
(send wrnd_w :manage)
(send wrkd_w :manage)
