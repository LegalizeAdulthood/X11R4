;;/*
;; * Copyright 1989 Dirk Grunwald
;; * 
;; * Permission to use, copy, modify, distribute, and sell this software
;; * and its documentation for any purpose is hereby granted without fee,
;; * provided that the above copyright notice appear in all copies and that
;; * both that copyright notice and this permission notice appear in
;; * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
;; * not be used in advertising or publicity pertaining to distribution of
;; * the software without specific, written prior permission.  Dirk
;; * Grunwald and M.I.T. makes no representations about the suitability of
;; * this software for any purpose.  It is provided "as is" without express
;; * or implied warranty.
;; * 
;; * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
;; * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
;; * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
;; * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
;; * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
;; * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
;; * OR PERFORMANCE OF THIS SOFTWARE.
;; * 
;; * Author:
;; * 	Dr. Dirk Grunwald
;; * 	Dept. of Computer Science
;; * 	Campus Box 430
;; * 	Univ. of Colorado, Boulder
;; * 	Boulder, CO 80309
;; * 
;; * 	grunwald@colorado.edu
;; * 	
;; */ 
;;
;;	An emacs program to walk through your TeX files, extracting lines
;;	like \\font cmr10=cmr10 @ptscale500 and converting them to a
;;	cannonical form, e.g. cmr10 scaled 500
;;
;;	To use this, grep for \\font in your TeX macros directory
;;	& put it in a file. Then, run extract-fonts-names in that\
;;	buffer.
;;
;;	to generate the makefile, run build-font-makefile. You can't
;;	build this easily within Emacs because it uses 24 bit ints.
;;
;;
(setq extract-fonts-dpi 85)
(setq extract-fonts-mag-normal 1000)
(setq extract-fonts-mag-detail 1440)
(setq extract-fonts-mode "sun_lores")
(setq extract-fonts-bdftosnf "bdftosnf")

(defvar extract-fonts-makefile-name "FMakefile")
(defvar extract-fonts-makefile nil)

(defvar extract-fonts-buffer nil)

(defvar extract-fonts-save-gf nil)
(defvar extract-fonts-save-pk t)
(defvar extract-fonts-save-bdf t)

(defvar extract-pk-font-list nil)
(defvar extract-snf-font-list nil)

(defun member-equal (l a)
  (while (and (not (null l)) (not (equal (car l) a)))
    (setq l (cdr l)))
  (and (not (null l)) (equal (car l) a)))

(defun extract-fonts-names (font-name-buffer)
  (interactive "bProcess buffer? ")
  ;;
  ;;	Create the makefile buffer
  ;;
  (setq extract-fonts-makefile (find-file-noselect extract-fonts-makefile-name))
					;
  (setq extract-fonts-buffer font-name-buffer)
  (set-buffer font-name-buffer)
  (goto-char (point-min))
  (delete-non-matching-lines "\\\\font.*=.*$")
  ;;
  ;;	Get rid of some macro definitions & names from lfonts.tex
  ;;
  (goto-char (point-min))
  (delete-matching-lines "fontdimen")
  (goto-char (point-min))
  (delete-matching-lines "#")
  (goto-char (point-min))
  (delete-matching-lines "\\\\CMD")
  (goto-char (point-min))
  (delete-matching-lines "\\\\FONTNAME")
  (goto-char (point-min))
  ;;
  ;;	Clean any leading junk, because we may have commented out
  ;;	the load-on-demand-fonts
  ;;
  (replace-regexp "^.*\\\\font" "\\\\font")
  ;;
  ;;	strip any trailing comments and or whitespace
  ;;
  (goto-char (point-min))
  (replace-regexp "[ 	]*%.*$" "");
  (goto-char (point-min))
  (replace-regexp "[ 	]*$" "");
  ;;
  ;;	strip the leading stuff up to the actual font name
  ;;
  (goto-char (point-min))
  (replace-regexp "^.*=[ 	]*" "")
  ;;
  ;;	Convert \@ptscale names to scale values
  ;;
  (goto-char (point-min))
  (replace-regexp "\\\\@ptscale[ 	]*\\([0-9]+\\)" "scaled \\100")
  (goto-char (point-min))
  (replace-regexp "\\\\@halfmag" "scaled 1095")
  (goto-char (point-min))
  (replace-regexp "\\\\magstephalf" "scaled 1095")
  (goto-char (point-min))
  (replace-regexp "\\\\@magscale[	 ]*" "\\\\magstep")
  (goto-char (point-min))
  (replace-regexp "\\\\magstep1" "scaled 1200")
  (goto-char (point-min))
  (replace-regexp "\\\\magstep2" "scaled 1440")
  (goto-char (point-min))
  (replace-regexp "\\\\magstep3" "scaled 1728")
  (goto-char (point-min))
  (replace-regexp "\\\\magstep4" "scaled 2074")
  (goto-char (point-min))
  (replace-regexp "\\\\magstep5" "scaled 2488")
  ;;
  ;;	make certain everything has a scale
  ;;
  (goto-char (point-min))
  (replace-regexp "^[a-zA-Z0-9]+$" "\\& scaled 1000")
  ;;
  ;;	normalize all spacings
  ;;
  (goto-char (point-min))
  (replace-regexp "scaled" " scaled ")
  (goto-char (point-min))
  (replace-regexp "[ 	]+" " ")
  )

