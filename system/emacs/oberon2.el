;;; oberon2.el --- major mode for Oberon-2 editing and compilation

;; Copyright (C) 1995  Michael van Acken <acken@informatik.uni-kl.de>
;; Version: 1.6, requires Emacs 19.28
;; You can ignore the "free variable o2-source-for-errors" errors when 
;; byte-compiling this file.

;; This file is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; Hint: If you are working at a color display, you may try to put the
;; following code into your .emacs file.  It will bring some color into
;; the Oberon-2 sources.
;;	(cond (window-system
;;	       (require 'hilit19)
;;	       (hilit-set-mode-patterns
;;	        '(o2-mode oberon-2-mode)
;;	        '(("(\\*" "\\*)" comment)
;;		  ("\"[^\n\"]*\"\\|\'[^\n\']*\'" 0 string)
;;	          ("^[ \t]*PROCEDURE" nil defun)
;;	          ("\\<\\(RECORD\\|ARRAY\\|OF\\|POINTER\\|TO\\|BEGIN\\|END\\|FOR\\|BY\\|IF\\|THEN\\|ELSE\\|ELSIF\\|CASE\\|WHILE\\|DO\\|MODULE\\|FROM\\|RETURN\\|IMPORT\\|VAR\\|LOOP\\|UNTIL\\|OR\\|DIV\\|EXIT\\|IN\\|IS\\|REPEAT\\|WITH\\|CONST\\|TYPE\\)\\>" nil keyword)))))
       

(require 'compile)

(defvar o2-indent 2 
  "*The indentation per syntax level in Oberon-2-Mode")

(defvar o2-block-suffix t
  "*If not nil, then the commands that insert structured statements like IF, 
WHILE, etc. will add the initial keyword in a comment after the final END.")

(defvar o2-compilation-ask-about-save nil
  "*If not nil, \\[o2-compile] and \\[o2-make] ask which buffers to save before
compiling.  Otherwise all modified buffers are saved without asking.")

(defvar o2-cwd (expand-file-name command-line-default-directory)
  "*The working directory for programs (compiler, browser, make etc) run in a 
subshell.  If you want to set the default path regardless from the place where
emacs was started, please set an absolute path for o2-cwd in your .emacs file,
e.g. with (setq o2-cwd \"/users/home/my_home/o2c/\")")

(defvar o2-compile-command "o2c -v -W "
  "*String passed to the shell to start the compilation of the current buffer.
Used by command \\[o2-compile].")

(defvar o2-make-command "o2c --make -v "
  "*String passed to the shell to run the make programm.
Used by command \\[o2-make].")

(defvar o2-whereis-command "o2whereis "
  "*Command called with file name to retrieve the file's path.")

(defvar o2-browse-command "o2b -x "
  "*Command called with module name to generate the module's exported definition.")



;; various history lists
(defvar o2-compile-history nil)
(defvar o2-make-history nil)
(defvar o2-module-history nil)
(defvar o2-identifier-history nil)
(defvar o2-proc-history nil)
(defvar o2-fpar-history nil)
(defvar o2-result-history nil)
(defvar o2-receiver-history nil)
(defvar o2-tbproc-history nil)
(defvar o2-receiver-type-history nil)


(defvar o2-mode-syntax-table nil
  "Syntax table in use in Oberon-2-mode buffers.")

(if o2-mode-syntax-table
    ()
  (let ((table (make-syntax-table)))	
    (modify-syntax-entry ?\\ "\\" table)
    (modify-syntax-entry ?\( "()1" table)
    (modify-syntax-entry ?\) ")(4" table)
    (modify-syntax-entry ?* ". 23" table)
    (modify-syntax-entry ?+ "." table)
    (modify-syntax-entry ?- "." table)
    (modify-syntax-entry ?= "." table)
    (modify-syntax-entry ?% "." table)
    (modify-syntax-entry ?< "." table)
    (modify-syntax-entry ?> "." table)
    (modify-syntax-entry ?$ "." table)
    (modify-syntax-entry ?& "." table)
    (modify-syntax-entry ?/ "." table)
    (modify-syntax-entry ?_ "." table)    
    (modify-syntax-entry ?| "." table)    
    (modify-syntax-entry ?_ "w" table)    
    (modify-syntax-entry ?\' "\"" table)
    (setq o2-mode-syntax-table table)))

(defvar o2-mode-map nil
  "Keymap used in Oberon-2 mode.")

(if o2-mode-map ()
  (let ((map (make-sparse-keymap))
	(imap (make-sparse-keymap "Oberon-2 Insert"))
	(hmap (make-sparse-keymap "Oberon-2 Hide/Show")))
    (define-key map "\^i" 'o2-tab)
    (define-key map "\^m" 'o2-newline)
    (define-key map "\177" 'backward-delete-char-untabify)
    (define-key map "\C-c\C-i" 'o2-indent-region)
    (define-key map "\C-cm" 'o2-insert-module)
    (define-key map "\C-cp" 'o2-insert-procedure)
    (define-key map "\C-ct" 'o2-insert-typebound-procedure)
    (define-key map "\C-c\C-t" 'o2-insert-typebound-redef)
    (define-key map "\C-ci" 'o2-insert-if)
    (define-key map "\C-cc" 'o2-insert-case)
    (define-key map "\C-c\C-w" 'o2-insert-with)
    (define-key map "\C-ce" 'o2-insert-else)
    (define-key map "\C-c\C-e" 'o2-insert-elsif)
    (define-key map "\C-cb" 'o2-insert-branch)
    (define-key map "\C-cf" 'o2-insert-for)
    (define-key map "\C-cw" 'o2-insert-while)
    (define-key map "\C-cr" 'o2-insert-repeat)
    (define-key map "\C-cl" 'o2-insert-loop)
    (define-key map "\C-c\C-r" 'o2-insert-record)
    (define-key map "\C-ch" 'o2-insert-header)
    (define-key map "\C-c\C-n" 'o2-next-visible-proc)
    (define-key map "\C-c\C-p" 'o2-previous-visible-proc)
    (define-key map "\C-c\C-u" 'o2-up-heading)
    (define-key map "\C-c\C-s" 'o2-show-subtree)
    (define-key map "\C-c\C-h" 'o2-hide-subtree)
    (define-key map "\C-c\C-l" 'o2-hide-leaves)
    (define-key map "\M-\C-h" 'o2-mark-procedure)
    (define-key map "\C-c\C-f" 'o2-find-module)
    (define-key map "\C-c4\C-f" 'o2-find-module-other-window)
    (define-key map "\C-c5\C-f" 'o2-find-module-other-frame)
    (define-key map "\C-c4f" 'o2-find-module-other-window)
    (define-key map "\C-c5f" 'o2-find-module-other-frame)
    (define-key map "\C-c\C-d" 'o2-showdef-module)
    (define-key map "\C-c4\C-d" 'o2-showdef-module-other-window)
    (define-key map "\C-c5\C-d" 'o2-showdef-module-other-frame)
    (define-key map "\C-c4d" 'o2-showdef-module-other-window)
    (define-key map "\C-c5d" 'o2-showdef-module-other-frame)
    (define-key map "\C-c." 'o2-find-definition)
    (define-key map "\C-c4." 'o2-find-definition-other-window)
    (define-key map "\C-c5." 'o2-find-definition-other-frame)
    (define-key map "\C-c," 'o2-find-typebound-procedure)
    (define-key map "\C-c4," 'o2-find-typebound-procedure-other-window)
    (define-key map "\C-c5," 'o2-find-typebound-procedure-other-frame)
    (define-key map "\C-c-" 'o2-find-def-under-cursor)
    (define-key map "\C-c/" 'o2-find-def-under-cursor)
    (define-key map "\M-c" 'o2-compile)
    (define-key map "\M-\C-c" 'o2-make)
    (define-key map "\C-c`" 'o2-next-error)
    (define-key map "\C-c'" 'o2-next-error)
    (define-key map "\C-cg" 'o2-goto-error)

    (define-key imap [o2-insert-record] '("RECORD" . o2-insert-record))
    (define-key imap [o2-insert-loop] '("LOOP" . o2-insert-loop))
    (define-key imap [o2-insert-repeat] '("REPEAT..." . o2-insert-repeat))
    (define-key imap [o2-insert-while] '("WHILE..." . o2-insert-while))
    (define-key imap [o2-insert-for] '("FOR..." . o2-insert-for))
    (define-key imap [o2-insert-branch] '("|" . o2-insert-branch))
    (define-key imap [o2-insert-else] '("ELSE" . o2-insert-else))
    (define-key imap [o2-insert-with] '("WITH..." . o2-insert-with))
    (define-key imap [o2-insert-case] '("CASE..." . o2-insert-case))
    (define-key imap [o2-insert-elsif] '("ELSIF..." . o2-insert-elsif))
    (define-key imap [o2-insert-if] '("IF..." . o2-insert-if))
    (define-key imap [o2-insert-typebound-redef] 
      '("Type-bound Redef..." . o2-insert-typebound-redef))
    (define-key imap [o2-insert-typebound-procedure] 
      '("Type-bound Proc..." . o2-insert-typebound-procedure))
    (define-key imap [o2-insert-procedure]
      '("Procedure..." . o2-insert-procedure))
    (define-key imap [o2-insert-module] '("Module..." . o2-insert-module))

    (define-key hmap [o2-show-all] '("Show All" . o2-show-all))
    (define-key hmap [o2-hide-all] '("Hide All" . o2-hide-all))
    (define-key hmap [o2-hide-leaves] '("Hide Local Procs" . o2-hide-leaves))
    (define-key hmap [o2-show-subtree] '("Show Procedure" . o2-show-subtree))
    (define-key hmap [o2-hide-subtree] '("Hide Procedure" . o2-hide-subtree))

    (define-key map [menu-bar o2] 
      (cons "Oberon-2" (make-sparse-keymap "Oberon-2")))
    
    (define-key map [menu-bar o2 o2-hide-show] (cons "Hide / Show" hmap))
    (define-key map [menu-bar o2 o2-insert] (cons "Insert" imap))
    (define-key map [menu-bar o2 o2-indent-region] 
      '("Indent Region..." . o2-indent-region))
    (define-key map [menu-bar o2 separator-eval2] '("--"))
    (define-key map [menu-bar o2 o2-goto-error] 
      '("Goto Error..." . o2-goto-error))
    (define-key map [menu-bar o2 o2-next-error] 
      '("Next Error" . o2-next-error))
    (define-key map [menu-bar o2 o2-make] '("Make..." . o2-make))
    (define-key map [menu-bar o2 o2-compile] '("Compile" . o2-compile))
    (define-key map [menu-bar o2 separator-eval1] '("--"))
    (define-key map [menu-bar o2 o2-find-typebound-procedure] 
      '("Find tb Proc..." . o2-find-typebound-procedure))
    (define-key map [menu-bar o2 o2-find-definition] 
      '("Find Definition..." . o2-find-definition))
    (define-key map [menu-bar o2 o2-showdef-module] 
      '("Show Module Def..." . o2-showdef-module))
    (define-key map [menu-bar o2 o2-find-module] 
      '("Find Module..." . o2-find-module))
    
    (define-key map [S-mouse-3] 'o2-find-def-under-mouse)
      
    (setq o2-mode-map map)))

(defun oberon-2-mode ()
"Major mode for editing Oberon-2 code.

Indentation:
\\[o2-newline]		make a newline, but indent like previous line
\\[o2-tab]		indent for o2-indent spaces
\\[backward-delete-char-untabify]		convert tabs to spaces while moving backward
\\[o2-indent-region]		add/remove indentation level(s) to region
The variable o2-ident determines how much spaces are inserted per indentation
level (default 2). 

Inserting Oberon-2 constructs:
\\[o2-insert-module]		MODULE outline
\\[o2-insert-procedure]		PROCEDURE outline 
\\[o2-insert-typebound-procedure]		typebound PROCEDURE outline 
\\[o2-insert-typebound-redef]		redefinition of typebound PROCEDURE
\\[o2-insert-if]		IF THEN statement
\\[o2-insert-elsif]		insert ELSIF
\\[o2-insert-case]		CASE statement
\\[o2-insert-with]		WITH statement
\\[o2-insert-else]		insert ELSE 
\\[o2-insert-branch]		insert a '|' 
\\[o2-insert-for]		FOR statement
\\[o2-insert-while]		WHILE statement
\\[o2-insert-repeat]		REPEAT UNTIL statement 
\\[o2-insert-loop]		LOOP statement
\\[o2-insert-record]		RECORD constructor
\\[o2-insert-header]		comment with various information about the module

Move by procedure headings:
\\[o2-next-visible-proc]		move to next procedure
\\[o2-previous-visible-proc]		move to previous procedure
\\[o2-up-heading]		move to procedure, to which the present one is local
\\[o2-mark-procedure]		put mark at end of procedure, point at the beginning

Functions to hide procedure bodies and declarations:
\\[o2-hide-subtree]		hide whole procedure
\\[o2-show-subtree]		show whole procedure again
\\[o2-hide-leaves]		hide bodies of all local procedures
\\[o2-hide-proc]	hide local declarations and the body
\\[o2-show-proc]	show local declarations and body again
\\[o2-hide-all]		hide all of buffer except procedure headings
\\[o2-show-all]		make all text in the buffer visible again

Managing source code:
\\[o2-find-module]		find file to a given module name
\\[o2-find-module-other-window]		like \\[o2-find-module], but display in another window
\\[o2-find-module-other-frame]		like \\[o2-find-module], but display in another frame
\\[o2-showdef-module]		generate and display the definition of a module
\\[o2-showdef-module-other-window]		like \\[o2-showdef-module], but display in another window
\\[o2-showdef-module-other-frame]		like \\[o2-showdef-module], but display in another frame
\\[o2-find-definition]		display (same window) the definition of an identifier
\\[o2-find-definition-other-window]		display (other window) the definition of an identifier
\\[o2-find-definition-other-frame]		display (other frame) the definition of an identifier
\\[o2-find-def-under-cursor]		display & blink to declaration of ident under cursor
\\[o2-find-typebound-procedure]		display (same window) a procedure bound to a given type
\\[o2-find-typebound-procedure-other-window]		display (other window) a procedure bound to a given type
\\[o2-find-typebound-procedure-other-frame]		display (other frame) a procedure bound to a given type
All the above functions accept a module's alias name (as declared in the 
current buffers IMPORT list) instead of the real module name. For further
information on '\\[o2-find-definition]' or '\\[o2-find-typebound-procedure]' use \\[describe-key].
Any of the above functions that displays files in another window or frame won't
change the selected window if the `C-u' prefix is set.  Example: `C-u C-c 4 .'
will display the source of the definition of the given identifier in another
window, but the current window stays selected and point stays at the current
position.
`\\[o2-find-def-under-mouse]' (ie, shift with right mouse button) will, similar to `\\[o2-find-def-under-cursor]',
invoke `C-u \\[o2-find-definition-other-window]' with the qualified identifier under the mouse cursor as 
argument and blink to the found position.

Compiling:
\\[o2-compile]		compile current buffer
C-u \\[o2-compile]		prompt for compile command and compile current buffer
\\[o2-make]		run a make on a module
\\[o2-next-error]		display the next error
\\[o2-goto-error]		prompt for module and error position, goto position

Searches will distinct cases. Delete converts tabs to spaces as it moves back. 
Emacs won't use tab characters. 
\\[indent-for-comment] will indent or create a comment on the current line at column 41 
(default).
The Oberon-2 menu can also be invoked with `C-mouse-3' (ie, control with right
mouse button) under GNU Emacs 19.29."
  (interactive)
  (kill-all-local-variables)
  (setq selective-display t)
  (setq indent-tabs-mode nil)
  (use-local-map o2-mode-map)
  (setq major-mode 'oberon-2-mode)
  (setq mode-name "Oberon-2")  
  (set (make-local-variable 'comment-column) 41)
  (set-syntax-table o2-mode-syntax-table)
  (set (make-local-variable 'paragraph-start) (concat "^$\\|" page-delimiter))
  (set (make-local-variable 'paragraph-separate) paragraph-start)
  (set (make-local-variable 'paragraph-ignore-fill-prefix) t)
  (set (make-local-variable 'require-final-newline) t)
  (set (make-local-variable 'comment-start) "(* ")
  (set (make-local-variable 'comment-end) " *)")
  (set (make-local-variable 'comment-start-skip) "(\\*+ *")
  (set (make-local-variable 'comment-indent-function) 'c-comment-indent)
  (set (make-local-variable 'parse-sexp-ignore-comments) nil)
  (setq case-fold-search nil)
  ; prepare compilation package to work with o2c
  (set (make-local-variable 'compilation-parse-errors-function)
       'o2-compilation-parse-errors)
  (add-hook 'compilation-mode-hook 'o2-compilation-mode-hook)
  (run-hooks 'oberon-2-mode-hook))

(defun o2-compilation-mode-hook ()
;; This is called when compilation-mode-hook is run.  The mode-specific 
;; variables are only set if the variable o2-running-compile is not void, i.e.
;; if o2-compile or o2-make are running.
  (cond ((boundp 'o2-running-compile)
	 (set (make-local-variable 'o2-source-for-errors) "")
	 (set (make-local-variable 'compilation-finish-function)
	      (lambda (buffer exit) (o2-fit-to-window buffer)))
	 (set (make-local-variable 'compilation-parse-errors-function)
	      'o2-compilation-parse-errors))))



(defun o2-interactive-strings (strings &optional barf-if-read-only)
;; STRINGS is a list of '(prompt . history) pairs, where prompt is a string
;; and history the history list for this prompt (or nil, if the default list
;; should be used).
  (if barf-if-read-only (barf-if-buffer-read-only))
  (mapcar (lambda (element)
	    (read-from-minibuffer (car element) nil nil nil (cdr element)))
	  strings))

(defun o2-newline ()
  "Insert a newline and indent following line like previous line."
  (interactive)
  (let ((prev-indent (current-indentation)))
    (newline) 
    (if (looking-at "[ \t]+$")
	(delete-region (match-beginning 0) (match-end 0)))
    (let ((curr-indent (current-indentation)))
      (if (<= curr-indent prev-indent)
	  (indent-to-column (- prev-indent curr-indent)))
      (let* ((end (progn (end-of-line) (point)))
	     (start (progn (beginning-of-line) (point))))
	(untabify start end)
	(forward-char prev-indent)))))

(defun o2-tab ()
  "Indent to next tab stop."
  (interactive)
  (let* ((curr-indent (current-indentation))
	 (new-indent (* (1+ (/ curr-indent o2-indent)) o2-indent))
	 (diff (- new-indent curr-indent)))
    (if (> diff 0)
	(insert-char ?\040 diff))))

(defun o2-what-line ()
  (save-restriction
    (widen)
    (save-excursion
      (beginning-of-line)
      (1+ (count-lines 1 (point))))))

(defun o2-indent-region (start end arg)
  "Add/remove indentation to/from the region. Each line will be indented ARG more
levels respectively less levels if ARG negative."
  (interactive "*r\nnIndent: ")
  (let* ((start-line (progn (goto-char start) (o2-what-line)))
	 (end-line (progn (goto-char end) (backward-char 1) (o2-what-line)))
	 (spaces (* arg o2-indent))
	 (current-line start-line)
	 count)
    (untabify (progn (goto-line start-line) 
		     (point))
	      (progn (goto-line end-line)
		     (end-of-line)
		     (point)))
    (goto-line start-line)
    (while (<= current-line end-line)
      (setq count spaces)
      (cond ((> arg 0)
	     (insert-char ?\040 count))
	    ((< arg 0)
	     (while (and (< count 0)
			 (eq (following-char) ?\040))
	       (delete-char 1)
	       (setq count (1+ count)))))
      (next-line 1)
      (beginning-of-line)
      (setq current-line (1+ current-line)))
    (goto-line start-line)
    (push-mark (point) t)
    (goto-line (1+ end-line))))

(defun o2-insert-block-suffix (label)
  (o2-newline)
  (o2-newline)
  (insert "END")
  (if o2-block-suffix
      (insert " (* " label " *)"))
  (insert ";")
  (end-of-line 0))

(defun o2-insert-case (expr)
  "Build skeleton CASE statment, prompting for the <expression>."
  (interactive "*sCase expression: ")
  (insert "CASE " expr " OF")
  (o2-insert-block-suffix "case")
  (insert "| "))

(defun o2-insert-else ()
  "Insert ELSE keyword and indent for next line."
  (interactive "*")
  (o2-newline)
  (backward-delete-char-untabify o2-indent nil)
  (insert "ELSE")
  (o2-newline)
  (o2-tab))

(defun o2-insert-branch ()
  "Indent for next line and insert '|'."
  (interactive "*")
  (o2-newline)
  (if (not 
       (save-excursion
	 (forward-line -1)
	 (looking-at "[ \t]*|")))
      (backward-delete-char-untabify o2-indent nil))
  (insert "| "))

(defun o2-insert-for (init end by)
  "Build skeleton FOR loop statment, prompting for the loop parameters."
  (interactive "*sInit: \nsend: \nsby: ")
  (insert "FOR " init " TO " end)
  (if (not (string-equal by ""))
	(insert " BY " by))
  (insert " DO")
  (o2-insert-block-suffix "for")
  (o2-tab))

(defun o2-insert-header (title)
  "Insert a comment block containing the module title, author, etc."
  (interactive "*sModule title: ")
  (insert "(*\n    Title: \t" title)
  (insert "\n    Created:\t")
  (insert (current-time-string))
  (insert "\n    Author: \t")
  (insert (user-full-name))
  (insert (concat "\n\t\t<" (user-login-name) "@" (system-name) ">\n"))
  (insert "*)\n\n"))

(defun o2-insert-elsif (guard)
  "Insert ELSIF part, prompting for the guard."
  (interactive "*sGuard: ")
  (o2-newline)
  (backward-delete-char-untabify o2-indent nil)
  (insert "ELSIF " guard " THEN")
  (o2-newline)
  (o2-tab))

(defun o2-insert-if (guard)
  "Insert skeleton IF statment, prompting for the guard."
  (interactive "*sGuard: ")
  (insert "IF " guard " THEN")
  (o2-insert-block-suffix "if")
  (o2-tab))

(defun o2-insert-loop ()
  "Build skeleton LOOP."
  (interactive "*")
  (insert "LOOP")
  (o2-insert-block-suffix "loop")
  (o2-tab))

(defun o2-insert-module (name)
  "Build skeleton MODULE, prompting for <module-name>."
  (interactive 
   (list (progn
	   (barf-if-buffer-read-only)
	   (read-from-minibuffer 
	    "Module name: "
	    (if (buffer-file-name)
		(let ((file-name (file-name-nondirectory (buffer-file-name))))
		  (if (string-match "\\([a-zA-Z0-9]+\\)\\.Mod" file-name)
		      (substring file-name 0 (match-end 1)))))
	    nil nil 'o2-module-history))))
  (insert "MODULE " name ";\n\n\nBEGIN\n")
  (o2-tab)
  (insert "\nEND " name ".\n")
  (previous-line 4))

(defun o2-insert-typebound-procedure (name receiver args result)
  "Insert skeleton PROCEDURE, prompting for <name>, <receiver>, <parameters> 
and the <result-type>." 
  (interactive (o2-interactive-strings
		'(("Procedure name: " . o2-proc-history)
		  ("Receiver: " . o2-receiver-history)
		  ("Formal parameters: " . o2-fpar-history)
		  ("Result type: " . o2-result-history))
		'barf-if-read-only))
  (insert "PROCEDURE ")
  (if receiver
      (insert "(" receiver ") "))
  (insert name)
  (if (not (and (string-equal args "") (string-equal result "")))
      (insert " (" args ")"))
  (if (not (string-equal result ""))
      (insert ": " result))
  (insert ";")
  (o2-newline)
  (o2-tab)
  (insert "BEGIN")
  (o2-newline)
  (o2-newline)
  ; remove export mark from procedure name
  (if (eq (aref name (1- (length name))) ?*)
      (setq name (substring name 0 (1- (length name)))))
  (insert "END " name ";")
  (o2-newline)
  (backward-delete-char-untabify o2-indent)
  (o2-newline)
  (end-of-line -2)
  (o2-tab))

(defun o2-insert-typebound-redef (name receiver)
  "Insert skeleton PROCEDURE, prompting for <name> and <receiver>.  The remaining
parameters (the formal parameter list including the result type, and the export
flag) will be copied from the exisiting definition of the procedure <name> for the receiver's base type."
  (interactive (o2-interactive-strings
		'(("Procedure name: " . o2-proc-history)
		  ("Receiver: " . o2-receiver-history))
		'barf-if-read-only))
  (let* ((rec-type (if (string-match ":[ \t\n]*\\(\\w+\\)[ \t\n]*" receiver)
		       (substring receiver (match-beginning 1) (match-end 1))
		     (error "Can't extract data type from receiver")))
	 (simple-name (if (string-match "[ \t\n*-]" name)
			  (substring name 0 (match-beginning 0))
			name))
	 (base (save-excursion
		 (o2-find-typebound-procedure-noselect rec-type simple-name)))
	 proc-attribs formal-pars result)
    (save-excursion 
      (set-buffer (car base))
      (goto-char (cdr base))
      (setq proc-attribs (o2-get-procedure))
      (setq formal-pars (nth 6 proc-attribs))
      (if (string-match "(\\(.*\\))[ \t\n]*\\(:[ \t\n]*\\(\\w+\\)[ \t\n]*\\)?"
			formal-pars)
	  (setq result (if (match-end 3)
			   (substring formal-pars 
				      (match-beginning 3) 
				      (match-end 3))
			 "")
		formal-pars (substring formal-pars 
				       (match-beginning 1) 
				       (match-end 1)))
	(setq result ""
	      formal-pars "")))
    (o2-insert-typebound-procedure (concat simple-name (nth 5 proc-attribs))
				   receiver
				   formal-pars
				   result)))

(defun o2-insert-procedure (name args result)
  "Insert skeleton PROCEDURE, prompting for <name>, <parameters> and the
<result-type>." 
  (interactive (o2-interactive-strings
		'(("Procedure name: " . o2-proc-history)
		  ("Formal parameters: " . o2-fpar-history)
		  ("Result type: " . o2-result-history))
		'barf-if-read-only))
  (o2-insert-typebound-procedure name nil args result))

(defun o2-insert-with (guard)
  "Build skeleton WITH statement, prompting for the type guard."
  (interactive "*sType guard: ")
  (insert "WITH " guard " DO")
  (o2-insert-block-suffix "with")
  (o2-tab))

(defun o2-insert-record ()
  "Insert a RECORD outline."
  (interactive "*")
  (insert "RECORD")
  (o2-insert-block-suffix "record")
  (o2-tab))

(defun o2-insert-repeat (expr)
  "Build a REPEAT-UNTIL statement, prompting for the <expression>."
  (interactive "*sUntil expression: ")
  (insert "REPEAT")
  (o2-newline)
  (o2-newline)
  (insert "UNTIL " expr ";")
  (end-of-line 0)
  (o2-tab))

(defun o2-insert-while (guard)
  "Insert a WHILE statement, prompting for the <guard>."
  (interactive "*sWhile guard: ")
  (insert "WHILE " guard " DO")
  (o2-insert-block-suffix "while")
  (o2-tab))





;;; Here starts the code for the modified outline functions:

(defconst o2-proc-regexp "\\([ \t]*\\)PROCEDURE[ \t]*\\(\\([-*+]\\|([a-zA-Z0-9 \t:]+)\\)[ \t]*\\)?"
;; Ugly. This should match any procedure header (including type-bound procedures) until just
;; before the actual procedure name.  It won't match a procedure forward declaration!
  "Regular expression to match the beginning of a procedure heading.")

(defun o2-indentation-level ()
;; Return the depth to which a procedures is nested.
;; Point must be at the beginning of a the heading, otherwise 0 is returned. 
;; The first \(...\) of o2-proc-regexp have to be put around the indentation.
  (save-excursion
    (beginning-of-line)
    (if (looking-at o2-proc-regexp)
	(/ (- (match-end 1) (match-beginning 1)) o2-indent)
      0)))

(defun o2-next-visible-proc (arg)
  "Move to the next visible procedure heading.
With argument, repeats or move backward if negative."
  (interactive "p")
  (let ((curr-point (point)))
    (if (< arg 0)
	(beginning-of-line)
      (end-of-line))
    (if (re-search-forward (concat "^\\(" o2-proc-regexp "\\w+\\)") nil 'move arg)
	(goto-char (match-end 2))
      (goto-char curr-point)		; restore cursor position
      (error "No further procedures."))))

(defun o2-previous-visible-proc (arg)
  "Move to the previous procedure heading.
With argument, repeats or move forward if negative."
  (interactive "p")
  (o2-next-visible-proc (- arg)))

(defun o2-next-proc ()
;;  Skip forward to just before the next (possibly invisible) procedure heading.
  (if (re-search-forward (concat "[\n\^M]\\(" o2-proc-regexp "\\w+\\)")
			 nil 'move)
      (goto-char (match-beginning 1))))

(defun o2-on-heading-p ()
;; Return T if point is on a header line.
  (save-excursion
    (if (re-search-backward "[\n]" nil 'move)
	(forward-char))
    (looking-at (concat "^" o2-proc-regexp "\\w+"))))

(defun o2-back-to-heading (&optional consider-nesting)
;; Move to previous procedure heading, or to beginning of this line if it is a
;; heading.
;; If consider-nesting is not nil local procedures before point will be skipped.
  (if (re-search-backward "[\n]" nil 'move)
      (forward-char))
  (or (o2-on-heading-p)
      (if consider-nesting	  
	  (let (exit result)
	    (while (and (not exit)
			(re-search-backward (concat "\\(END[ \t]+\\(\\w+\\)[ \t]*;\\|" o2-proc-regexp "\\w+\\)") nil 'move))
	      (if (match-beginning 2)
		  (let ((name (buffer-substring (match-beginning 2) (match-end 2))))
		    (if (re-search-backward (concat o2-proc-regexp name) nil 'move)
			t
		      (error "Can't find start of procedure %s." name)))
		(setq exit t)
		(beginning-of-line)
		(setq result (point))))
	    result)
	(re-search-backward (concat "^\\(" o2-proc-regexp "\\w+\\)") nil 'move))))

(defun o2-up-heading (arg)
  "Move to the procedure heading to which the present procedure is local.
With argument, move up ARG levels."
  (interactive "p")
  (o2-back-to-heading)
  (if (eq (o2-indentation-level) 0)
      (error "Uppermost level."))
    (while (and (> (o2-indentation-level) 0)
		(> arg 0)
		(not (bobp)))
      (let ((present-level (o2-indentation-level)))
	(while (>= (o2-indentation-level) present-level)
	  (o2-previous-visible-proc 1))
	(setq arg (1- arg)))))



(defun o2-search (pattern error-msg)
; Does a re-search-forward for pattern, signals error (with error-msg)
; if a occurence is found.
  (if (not (re-search-forward pattern nil t))
      (error error-msg)))

(defun o2-proc-end-delimiter (name)
  (concat "END[ \t]+" name "[ \t]*;"))

(defun o2-get-procedure ()
;; pre: point is inside a procedure (i.e. somewhere between the line of 
;;   procedure heading (inclusive) and the ';' following the procedure end 
;;   delimiter (inclusive).
;;   An error is signaled if point is outside any procedure.
;; post: result is a n-tuple with the elements
;;   name of the procedure
;;   end formal pars (pos directly following the final ;)
;;   end of CONST/TYPE/VAR declaration sequence (only if local procs exist, 
;;     nil otherwise) 
;;   end of local procedure declarations (only if local procs exist, nil 
;;     otherwise) 
;;   end of procedure (char following the final ;)
;;   export mark ("*" or "")
;;   formal parameters (string from initial parenthesis upto trailing ;)
  (save-excursion
    (let ((curr-point (point)))
      (if (o2-back-to-heading 'consider-nesting)
	  (let ((formal-pars "") (export-mark "")
		proc-name end-of-heading end-of-proc end-of-decl end-of-local)
	    (looking-at (concat o2-proc-regexp "\\(\\w+\\)[ \t\n]*\\(\\*[ \t\n]*\\)?"))
	    ; store export mark, if one exists
	    (if (match-beginning 5)
		(setq export-mark (buffer-substring (match-beginning 5)
						    (1+ (match-beginning 5)))))
	    (setq proc-name (buffer-substring (match-beginning 4) (match-end 4))) ; the name of the procedure
	    (goto-char (match-end 0))
	    (if (looking-at "(")
		(let ((start (point)))
		  (o2-search ")\\([ \t\n]*:[ \t\n]*[a-zA-Z0-9.]+\\)?[ \t\n]*;"
			     "no end of formal parameters") ; skip formal parameters and result
		  (setq formal-pars (buffer-substring start (1- (point)))))
	      (re-search-forward ";"))
	    (setq end-of-heading (point))	; this is the pos directly following the final ';' of the procedure heading
	    (o2-search (o2-proc-end-delimiter proc-name)
		       (concat "no end of procedure " proc-name))
	    (setq end-of-proc (point))
	    (if (> curr-point end-of-proc)
		(error "Point is not inside a procedure."))		
	    (goto-char end-of-heading)
	    (if (re-search-forward (concat "^" o2-proc-regexp "\\w+") end-of-proc t)
		(progn
		  (setq end-of-decl (- (match-beginning 0) 1))
		  (goto-char end-of-proc)
		  (re-search-backward (o2-proc-end-delimiter "\\w+") end-of-decl t 2)
		  (setq end-of-local (point))))
	    (list proc-name end-of-heading end-of-decl 
		  end-of-local end-of-proc export-mark formal-pars))
	(error "Point is not inside a procedure.")))))

(defun o2-flag-region (from to flag)
;; Hides or shows lines from FROM to TO, according to FLAG. If FLAG is `\\n' 
;; (newline character) then text is shown, while if FLAG is `\\^M' (control-M) 
;; the text is hidden.
  (let (buffer-read-only		; allow modification of buffer
	(modp (buffer-modified-p)))	; but keep the 'modified' flag
    (unwind-protect
        (subst-char-in-region from to
			      (if (= flag ?\n) ?\^M ?\n)
			      flag t)
     (set-buffer-modified-p modp))))	; restore flag

(defun o2-modify-proc (flag)
; Hides/shows procedure the point is in.
; Writes an error if point is not inside a procedure.
  (let ((proc-extent (o2-get-procedure)))
    (if (not (nth 2 proc-extent))	
	(o2-flag-region (nth 1 proc-extent) (nth 4 proc-extent) flag)
      (o2-flag-region (nth 1 proc-extent) (nth 2 proc-extent) flag)
      (o2-flag-region (nth 3 proc-extent) (nth 4 proc-extent) flag))))
	    
(defun o2-hide-proc ()
  "Hide the body directly following this heading."
  (interactive)
  (o2-modify-proc ?\^M))

(defun o2-show-proc ()
  "Show the body directly following this heading."
  (interactive)
  (o2-modify-proc ?\n))


(defun o2-hide-region-body (start end)
;; Hide all body lines in the region, but not headings.
  (save-excursion
    (save-restriction
      (narrow-to-region start end)
      (goto-char (point-min))
      (o2-next-proc)
      (while (not (eobp))
	(o2-modify-proc ?\^M)
	(o2-next-proc)))))

(defun o2-hide-all ()
  "Hide all of buffer except headings."
  (interactive)
  (o2-hide-region-body (point-min) (point-max)))

(defun o2-show-all ()
  "Show all of the text in the buffer."
  (interactive)
  (o2-flag-region (point-min) (point-max) ?\n))

(defun o2-hide-subtree ()
  "Hide everything after this heading at deeper levels."
  (interactive)
  (let ((proc-extent (o2-get-procedure)))
    (o2-flag-region (nth 1 proc-extent) (nth 4 proc-extent) ?\^M)))

(defun o2-show-subtree ()
  "Show everything after this heading at deeper levels."
  (interactive)
  (let ((proc-extent (o2-get-procedure)))
    (o2-flag-region (nth 1 proc-extent) (nth 4 proc-extent) ?\n)))

(defun o2-hide-leaves ()
  "Hide all bodies after this heading at deeper levels."
  (interactive)
  (let ((proc-extent (o2-get-procedure)))
    (o2-hide-region-body (nth 1 proc-extent) (nth 4 proc-extent))))

(defun o2-get-procedure-at-point ()
;; If point is inside a procedure declaration a list as in o2-get-procedure is 
;; returned with the starting point of the procedure declaration appended to 
;; it.  Otherwise result is nil.
  (save-excursion
    (let* (result
	   (pattern (concat "\\(" o2-proc-regexp "\\w+\\)\\|\\(" (o2-proc-end-delimiter "\\([a-zA-Z0-9]+\\)") "\\)")))
      (beginning-of-line)
      (if (o2-on-heading-p)
	  (append (o2-get-procedure) (list (point)))      
	(while (and (not result) (not (bobp)))
	  (if (re-search-backward pattern nil 'move)
	      (if (match-beginning 1)
					; found procedure heading
		  (let ((start (match-beginning 1)))
		    (setq result (append (o2-get-procedure) (list start))))
					; found end of procedure
		(let ((proc-name (buffer-substring (match-beginning 6) 
						   (match-end 6))))
		  (re-search-backward (concat "^" o2-proc-regexp proc-name 
					      "[ \t* ;(]") 
				      nil 'move)))))
	result))))

(defun o2-mark-procedure ()
  "Put mark at end of procedure (behind the last following whitespace), point at
beginning (the start of the line containing the procedure header). If point is 
not inside a procedure nothing will happen."
  (interactive)
  (let ((proc-info (o2-get-procedure-at-point)))
    (if (not proc-info)
	(message "Point is not inside a procedure, no region marked.")
      (goto-char (nth 4 proc-info))
      (re-search-forward "[^ \t\n]" nil 'move)
      (beginning-of-line)
      (push-mark (point))
      (goto-char (nth 7 proc-info))
      (beginning-of-line)
      (message (concat "Marked procedure " (nth 0 proc-info) ".")))))




			 


(defun o2-read-import ()
;; If a module import is following, it's elements are returned in the form (alias-name true-name).
;; Otherwise the functions result is nil.
  (if (re-search-forward "\\(;\\)\\|\\(\\w+\\)\\([ \t\n]*:=[ \t\n]*\\(\\w+\\)[ \t\n]*\\)?[,;]" nil t)
      (prog1
	  (cond ((match-beginning 1)	; end of import list (';') reached
		 nil)
		((match-beginning 3)	; there exists an ':= xxx' branch
		 (cons
		  (buffer-substring (match-beginning 2) (match-end 2))
		  (buffer-substring (match-beginning 4) (match-end 4))))
		(t			; only the plain module name is imported
		 (cons
		  (buffer-substring (match-beginning 2) (match-end 2))
		  (buffer-substring (match-beginning 2) (match-end 2)))))
	(backward-char 1))	
    nil))

(defun o2-import-list ()
;; Scans the complete IMPORT list and gives the imported modules in a list like
;; "((AliasA . ModuleA) (AliasB . ModuleB) ...)". If the keyword IMPORT
;; can not be found, nil is returned. If a module is not imported via an alias
;; name, the alias and the module name are identic.
  (save-excursion
    (let (import-list 
	  read-names
	  (data (match-data)))
      (unwind-protect
	  (progn
	    (goto-char (point-min))
	    (if (re-search-forward "[ \t\n]+IMPORT[ \t\n]+" nil t)
		(progn
		  (setq read-names (o2-read-import))
		  (while read-names		; not nice, but should work
		    (setq import-list (cons read-names import-list))
		    (setq read-names (o2-read-import)))))
	    import-list)
	(store-match-data data)))))

(defun o2-module-name (module-name)
;; If module-name is an alias of an imported module, the true module name is
;; returned, otherwise simply module-name. 
  (let ((alias (cdr (assoc module-name (o2-import-list)))))
    (if alias alias module-name)))

(defun o2-qualified-import (qualident)
;; Accepts a (possibly qualified) identifier (see Oberon Report), and seperates
;; it into module and ident part. If the module name is given (qualident is 
;; something 'M.ident') the module name is extracted and possibly converted if
;; it is an alias from the IMPORT list. The result is (module-name . ident).
;; If no module is given (qualident is "ident") the function returns (nil . 
;; ident).  If the qualident contains illegal characters it will result to nil.
  (if (string-match "^\\(\\(\\w+\\) *\\. *\\)?\\(\\w+\\)$" qualident)
      (cons
       (if (match-end 1) (o2-module-name (substring qualident 
						    (match-beginning 2) 
						    (match-end 2))))
       (substring qualident (match-beginning 3) (match-end 3)))
    (error "Not legal qualified identifier.")))



(defun o2-switch-to (location &optional other save-mark)
; Note: This function accesses the variable current-prefix-arg to decide
; if to select another window or not.  This is not very `clean', I admit.
  (if (and save-mark
	   (not (memq other '(other-window other-frame)))
	   (eq (current-buffer) (car location)))
      (progn
	(push-mark nil t)
	(message "Mark saved where search started")))
  (let ((old-window (selected-window)))
    (if (eq other 'other-window)
	(switch-to-buffer-other-window (car location))
      (if (eq other 'other-frame)
	  (switch-to-buffer-other-frame (car location))
	(switch-to-buffer (car location))))
    (if (cdr location)
	(set-window-point (selected-window) (cdr location)))
    (if current-prefix-arg
	(select-window old-window))))

(defun o2-set-to (location)
;; location is a list (buffer position)
  (set-buffer (car location))
  (goto-char (cdr location)))


    
(defun o2-execute-command (command)
  (let ((shell shell-file-name)
	(default-directory o2-cwd))
    (call-process shell nil t t "-c" (concat "exec " command))))

(defun o2-locate-file (file-name &optional extension no-error)
;; Uses the command o2whereis to find the absolute path of files.
;; If extension is non nil and the name does not end with an extension,
;; the ending '.<extension>' is automatically appended. 
  (save-excursion
    (if (and extension
	     (not (string-match "\\.\\w+" file-name)))
	(setq file-name (concat file-name "." extension)))
    (set-buffer (get-buffer-create "*command execution*"))
    (erase-buffer)
    (o2-execute-command (concat o2-whereis-command file-name))
    (goto-char (point-min))
    (if (re-search-forward (concat "^.*" file-name "$") nil t)
	(expand-file-name (buffer-substring (match-beginning 0) (match-end 0))
			  o2-cwd)
      (if no-error
	  nil
	(error "Cannot locate file %s." file-name)))))
  
(defun o2-find-module-noselect (module-name)
;; Calls the external command o2whereis with the parameter modulename.Mod and 
;; reads it (if necessary) into a buffer.  The buffer will be returned (but not
;; displayed).  If no such file exists, a error message will be printed.
  (find-file-noselect (o2-locate-file (o2-module-name module-name) "Mod")))

(defun o2-find-module (module-name)
"Call the external command o2whereis with the parameter 'MODULE-NAME.Mod' to 
find the associated file. If no such file exists, a error message will be
printed."
  (interactive (o2-read-qualident "Module name: " 'just-module 
				  'o2-module-history))
  (o2-switch-to (cons (o2-find-module-noselect module-name) nil)))

(defun o2-find-module-other-window (module-name)
"Like o2-find-module, but display the file in another window."
  (interactive (o2-read-qualident "Module name: " 'just-module
				  'o2-module-history))
  (o2-switch-to (cons (o2-find-module-noselect module-name) nil) 'other-window))

(defun o2-find-module-other-frame (module-name)
"Like o2-find-module, but display the file in another frame."
  (interactive (o2-read-qualident "Module name: " 'just-module
				  'o2-module-history))
  (o2-switch-to (cons (o2-find-module-noselect module-name) nil) 'other-frame))

(defun o2-showdef-module-noselect (module-name)
;; Call the external command o2-browse-command which extracts the interface of
;; a module out of a given symbol file.  The buffer gets the name 
;; *modulename.Def* and is not selected.
  (save-excursion
    (setq module-name (o2-module-name module-name))
    (let* ((defname (concat "*" module-name ".Def*"))
	   (buffer (get-buffer-create defname)))
      (set-buffer buffer)
      (oberon-2-mode)
      (let ((buffer-read-only nil))
	(erase-buffer)
	(o2-execute-command (concat o2-browse-command module-name)))
      (setq buffer-read-only t)
      (list buffer))))

(defun o2-showdef-module (module-name)
"Call the external command o2showdef which extracts the interface of the module
MODULE-NAME out of a given symbol file. The buffer gets the name
*MODULE-NAME.Def*."
  (interactive (o2-read-qualident "Module name: " 'just-module
				  'o2-module-history))
  (o2-switch-to (o2-showdef-module-noselect module-name))
  (oberon-2-mode))

(defun o2-showdef-module-other-window (module-name)
"Like o2-showdef-module, but display the file in another window."
  (interactive (o2-read-qualident "Module name: " 'just-module
				  'o2-module-history))
  (o2-switch-to (o2-showdef-module-noselect module-name) 'other-window))

(defun o2-showdef-module-other-frame (module-name)
"Like o2-showdef-module, but display the file in another frame."
  (interactive (o2-read-qualident "Module name: " 'just-module
				  'o2-module-history))
  (o2-switch-to (o2-showdef-module-noselect module-name) 'other-frame))



(defun o2-declarationp ()
;; Is t iff point is inside the declaration part of a module or procedure.
  (save-excursion
    (if (save-excursion (re-search-backward "[^\n]*DEFINITION " nil t))
	(not (re-search-backward "PROCEDURE[ \t\n]+\\w+" nil t))      
      (and (re-search-backward 
	    (concat "\\(PROCEDURE\\|MODULE\\|DEFINITION\\)\\|BEGIN\\|" (o2-proc-end-delimiter "\\w+"))
	    nil 'move)
	   (match-beginning 1)))))

(defun o2-no-recordp ()
;; Is t iff point is not inside a record type declaration.
  (save-excursion
    (let (terminate result (end-count 0))
      (while (and (not terminate)
		  (not result)
		  (re-search-backward 
		   "\\W\\(MODULE\\|PROCEDURE\\|CONST\\|TYPE\\|VAR\\)\\|RECORD\\|\\(END\\)\\W"
		   nil 'move))
	(cond 
	 ((match-beginning 1) (setq result t))
	 ((match-beginning 2) (setq end-count (1+ end-count)))
	 ((= end-count 0)     (setq terminate t))
	 (t                   (setq end-count (1- end-count)))))
      result)))

(defun o2-find-local-declaration (ident pos)
;; Searches the current buffer for the position of identifier's ident 
;; declaration valid at position pos. The result is the starting point of the 
;; declaration or nil if it cannot be found.  IDENT cannot be a qualified 
;; identifier.
;; This function will not merely do a backward search, but will scan only the 
;; relevant declaration scopes that may contain the declaration it is searching
;; for. It cannot find record fields or type-bound procedures. 
  (save-excursion
    (goto-char pos)
    (let* (terminate
	   (procpattern (concat "PROCEDURE[ \t-*+^]+" ident "[ \t* ;(]"))
	   (endpattern (o2-proc-end-delimiter "\\(\\w+\\)"))
	   (pattern (concat "\\W" 
			    "\\(\\(" procpattern "\\)"
			    "\\|\\(" "\\(\\w+[ \t\n*-]*,[ \t\n]*\\)*"
                                ident "\\([ \t\n*-]*,[ \t\n]*\\w+\\)*" 
                                "[ \t*-]*:[^=]\\)"
			    "\\|\\(" ident "[ \t*-]*=\\)"
			    "\\|\\(" endpattern "\\)\\)")))
      (while (not (or terminate (bobp)))
	(if (re-search-backward pattern nil 'move)
	    (cond
	     ((or (match-beginning 2)
		  (and (match-beginning 3) (o2-declarationp) (o2-no-recordp))
		  (and (match-beginning 6) (o2-declarationp)))
	      (setq terminate (1+ (point))))
	     ((match-beginning 7)
	      (goto-char (match-beginning 7))
	      (looking-at endpattern)	; include identifier parts beyond current point
	      (let ((proc-name (buffer-substring (match-beginning 1) (match-end 1))))		
		(if (and (re-search-backward (concat o2-proc-regexp proc-name "\\W") nil 'move)  
			 (string= proc-name ident)
			 (> 1 (- (match-end 1) (match-beginning 1))))
		    (setq terminate (match-end 1))))))))
      terminate)))

(defun o2-default-qualident (&optional just-module)
;; Scan the buffer at (and before) the current point for a qualified 
;; identifier.  Result is this qualified identifier (or the module if 
;; just-module isn't nil) or nil if none is found.
  (save-excursion
    (let (start-pos
	  (curr-point (point)))
      (beginning-of-line)
      (while (and (re-search-forward "[a-zA-Z]\\w*\\( *\\. *\\w+\\)*" nil t)
		  (<= (match-beginning 0) curr-point))
	(setq start-pos (match-beginning 0)))
      (if start-pos
	  (progn
	    (goto-char start-pos)
	    (if (and (not just-module) (looking-at "\\(\\w+\\) *\\. *\\w+"))
		(if (assoc (buffer-substring (match-beginning 1) (match-end 1)) 
			   (o2-import-list))
		    (buffer-substring (match-beginning 0) (match-end 0))
		  (buffer-substring (match-beginning 1) (match-end 1)))
	      (looking-at "\\w+")
	      (buffer-substring (match-beginning 0) (match-end 0))))))))
	      

(defun o2-read-qualident (string &optional just-module history)
;; Prompt for a qualident, offer the ident under point (or before) as default.
;; If history is non-nil, it is used as history list, otherwise the default
;; minibuffer-history is used.
  (let* ((default (o2-default-qualident just-module))
	 (spec (read-from-minibuffer
		(if default
		    (format "%s(default %s) " string default)
		  string)
		nil
		nil
		nil
		history))
	 (result (if (string= spec "")
		     default
		   spec)))
    ; update history manually
    (if (and history 
	     (not (string= (car (eval history)) result)))
	(set history (cons result (eval history))))
    (list result)))

(defun o2-limit-to-decl (pos limit-to-decl)
;; If limit-to-decl is non nil, return minimum of position of first procedure 
;; and pos, otherwise return pos.
  (if limit-to-decl
      (min
       (save-excursion
	 (goto-char (point-min))
	 (re-search-forward (concat o2-proc-regexp "\\w+") nil 'move)
	 (point))
       pos)
    pos))
	 
(defun o2-find-definition-noselect (qualident &optional limit-to-decl)
;; Find the definition of a given qualified identifier: result is (buffer . 
;; point).  If a buffer different from the current one has to be searched, it 
;; will not get selected.
;; There are three flavours:
;; 1. No module name is given. The function tries to locate the definition of
;;    the identifier in the current module.
;;    The search may fail on forward declared pointer base types. If the search
;;    at the current position fails, it is repeated with the position set to 
;;    the end of the local declaration block (if possible).
;; 2. A module name is given and the sources of the module are available.
;;    The module is loaded and the cursor jumps to the position of the 
;;    declaration.
;; 3. If nor 1. nor 2. holds, then the definition of the module is extracted 
;;    from it's symbol file and used instead.
;; 4. If you entered a module name and 2. and 3. failed, then you will get the 
;;    error message 'cannot find file xxx.OSym'.
;; limit-to-decl will bound the search for the declaration part (everything 
;; before the first normal procedure) of the module
  (let* 
      ((analyze     (o2-qualified-import qualident))
       (ident       (cdr analyze))
       (module-name (car analyze))
       (module-path (if module-name (o2-locate-file module-name "Mod" 'no-error)))
       (symbol-path (if (and module-name (not module-path)) (o2-locate-file module-name "OSym")))
       (buffer
	(cond
	 (symbol-path			; module name given, no module sources, symbol file exists
	  (o2-showdef-module-noselect module-name))
	 (module-path			; module name given, module sources exist
	  (o2-find-module-noselect module-name))
	 (t
	  (current-buffer)))))
    (save-excursion
      (set-buffer buffer)
      (save-excursion
	(let ((location
	       (o2-find-local-declaration 
		ident 
		(o2-limit-to-decl (if (or symbol-path module-path) (point-max) (point)) limit-to-decl))))
	  ; second try, if nothing is found and search is local
	  (if (and (not location)
		   (not module-name)
		   (re-search-forward "^\\W*\\(BEGIN\\|END[ \t\n]+\\w+\\|PROCEDURE\\)\\W" nil 'move))
	      (setq location
		    (o2-find-local-declaration 
		     ident 
		     (o2-limit-to-decl (point) limit-to-decl))))
	  (if location 
	      (cons buffer location)
	    (error "Can't find declaration of %s" ident)))))))

(defun o2-find-definition (qualident &optional limit-to-decl)
"Find and display the definition of QUALIDENT.
If a module name is given (like 'M.ident') the module's source code (or the
definition, if the sources are not available) is loaded and searched for the
identifier in it's global scope. If just the identifier is given, the current
buffer will be searched for the definition of the identifier valid at the
current cursor position according to Oberon-2's scope rules.
If QUALIDENT is a null string, the identififier on or before the point (if 
there is any on the current line) will be used." 
;; limit-to-decl will bound the search for the declaration part (everything
;; before the first normal procedure) of the module.
  (interactive (o2-read-qualident "(qualified) Identifier: " nil
				  'o2-identifier-history))
  (o2-switch-to (o2-find-definition-noselect qualident limit-to-decl)
		nil 'save-mark))
	
(defun o2-find-definition-other-window (qualident &optional limit-to-decl)
  "Like o2-find-definition, but file will be displayed in another window."
;; limit-to-decl will bound the search for the declaration part (everything
;; before the first normal procedure) of the module.
  (interactive (o2-read-qualident "(qualified) Identifier: " nil
				  'o2-identifier-history))
  (o2-switch-to (o2-find-definition-noselect qualident limit-to-decl) 
		'other-window 'save-mark))
	
(defun o2-find-definition-other-frame (qualident &optional limit-to-decl)
  "Like o2-find-definition, but file will be displayed in another frame."
;; limit-to-decl will bound the search for the declaration part (everything
;; before the first normal procedure) of the module.
  (interactive (o2-read-qualident "(qualified) Identifier: " nil
				  'o2-identifier-history))
  (o2-switch-to (o2-find-definition-noselect qualident limit-to-decl) 
		'other-frame 'save-mark))

	
(defun o2-find-def-blink (new-window message)
  (let ((qualident (o2-default-qualident)))
    (if qualident
	(save-excursion
	  (o2-find-definition-other-window qualident)
	  (sit-for (if (boundp 'blink-matching-delay)
		       blink-matching-delay
		     1))
	  (select-window new-window))
      (error "%s not on qualified identifier." message))))

(defun o2-find-def-under-mouse (event)
  "Find definition of qualified identifier under the mouse and display it in
another window."
  (interactive "e")
  (let* ((start (event-start event))
	 (buffer (window-buffer (car start))))
    (set-buffer buffer)
    (goto-char (nth 1 start))
    (sit-for 0)
    (o2-find-def-blink (car start) "Mouse")))
	 
(defun o2-find-def-under-cursor ()
  "Find definition of qualified identifier under the cursor and display it in
another window."
  (interactive)
  (o2-find-def-blink (selected-window) "Cursor"))
	 
	 
  
(defun o2-get-pointer-to (record-name)
;; Finds the name of the first declared data type 'POINTER TO record-name' in 
;; the modules declaration block, or nil if the search fails.
  (save-excursion
    (goto-char (point-min))
    (if (re-search-forward (concat "\\W\\(\\w+\\)[ \t\n*-]*=\\s *POINTER\\s +TO\\s +" record-name "\\W") 
			   (o2-limit-to-decl (point-max) t) t)
	(buffer-substring (match-beginning 1) (match-end 1))
      (error "Can't find any declaration of type 'POINTER TO %s'." record-name))))

(defun o2-find-typebound-procedure-noselect (receiver proc-name &optional prev-path conv-to-pointer)
;; Takes a receiver data type (possibly qualified) and the name of a type-bound
;; procedure. It will find the procedure bound to the receiver. If the actual
;; procedure in use is inherited from a base type, this base type's procedure 
;; will be located. In the last line of the display the search path is printed.
;; The optional arguments mean: 
;;   prev-path is the search path until now
;;   conv-to-pointer controls if the receiver has to be modified from the passed record type 
;;                   to the corresponding pointer type.
;; The whole search process won't effect the display.
  (save-excursion
    (if (not prev-path) (setq prev-path ""))
    (goto-char (o2-limit-to-decl (point-max) t))
    (let* ((location (o2-find-definition-noselect receiver 'limit-to-decl))
	   (receiver (o2-qualified-import receiver))
	   (module (car receiver))
	   (ident (cdr receiver))
	   record 
	   base-type)
      (o2-set-to location)
      (save-excursion
        ; first we find the record type the procedure is bound to and generate the receiver type (if necessary)
	(if conv-to-pointer
	    (progn				; get the pointer type to the given record type
	      (setq record ident)
	      (setq ident (o2-get-pointer-to record)))
	  (if (looking-at (concat ident "[ \t\n*]*=\\s *POINTER\\s +TO\\s +\\(\\w+\\);"))
	      (progn (setq conv-to-pointer t) ; we have a pointer, now we extract the record type
		     (setq record (buffer-substring (match-beginning 1) (match-end 1)))
		     (let ((pos (o2-find-local-declaration record (o2-limit-to-decl (point-max) t))))
		       (if pos (goto-char pos) 
			 (error "Can't find record %s to pointer %s." record ident))))
	    (setq record ident)))
        ; second we try to extract the record's base type (if there is any)
	(if (not (looking-at (concat record "[ \t\n*\\-]*=\\s *RECORD\\W")))
	    (error "Receiver is not a (POINTER TO) RECORD data type.")
	  (setq base-type
		(if (and (re-search-forward 
			  "\\WEND\\W\\|(\\s *\\(\\w+\\s *\\(\\.\\w+\\s *\\)?\\))" nil t)
			 (match-end 1))
		    (buffer-substring (match-beginning 1) (match-end 1)))))
        ; third we try to locate the type-bound procedure to the current receiver type
	(goto-char (point-min))
	(if (re-search-forward 
	     (concat "\\WPROCEDURE\\s *(\\w+\\s *:\\s *" ident "\\s *)\\s *" proc-name "\\W") nil t)
	    (progn (message (concat prev-path ident)) ; yepp, we found it
		   (cons (current-buffer) (1+ (match-beginning 0))))
	  (if base-type			; we failed, so we try the same on the receiver's base type
	      (o2-find-typebound-procedure-noselect base-type 
						    proc-name 
						    (concat prev-path ident " ") 
						    conv-to-pointer)
	    (error "Can't find typebound procedure %s." proc-name)))))))

(defun o2-find-typebound-procedure-other-window (receiver proc-name)
  "Like o2-find-typebound-procedure, but file will be displayed in another window."
  (interactive (o2-interactive-strings
		'(("Receiver type: " . o2-receiver-type-history)
		  ("Procedure name: " . o2-proc-history))))
  (o2-switch-to (o2-find-typebound-procedure-noselect receiver proc-name) 
		'other-window 'save-mark))

(defun o2-find-typebound-procedure-other-frame (receiver proc-name)
  "Like o2-find-typebound-procedure, but file will be displayed in another frame."
  (interactive (o2-interactive-strings
		'(("Receiver type: " . o2-receiver-type-history)
		  ("Procedure name: " . o2-proc-history))))
  (o2-switch-to (o2-find-typebound-procedure-noselect receiver proc-name) 
		'other-frame 'save-mark))

(defun o2-find-typebound-procedure (receiver proc-name)
"Find to a data type RECEIVER the typebound procedure PROC-NAME bound to it. If
the procedure is inherited from the receiver's base type, the search is
automatically extended to this base type and so on. RECEIVER has to be a 
(possibly qualified) identifier.
The search may take a while."
  (interactive (o2-interactive-strings
		'(("Receiver type: " . o2-receiver-type-history)
		  ("Procedure name: " . o2-proc-history))))
  (o2-switch-to (o2-find-typebound-procedure-noselect receiver proc-name)
                nil 'save-mark))



(defun o2-fit-to-window (buffer)
; Reset the window-start of the buffer so that (point-max) is 
; displayed in the last line of window.
  (let* ((current (selected-window))
	 (window (get-buffer-window buffer t)))
    (if window
	(save-excursion
	  (select-window window)
	  (let ((old-point (window-point)))
	    (unwind-protect
		(progn
		  (forward-line (- 0 (- (window-height) 2 )))
		  (set-window-start window (point))
		  (while (not (pos-visible-in-window-p (point-max) window))
		    (forward-line 1)
		    (set-window-start window (point))))
	      (select-window current)
	      (set-window-point window old-point)))))))

(defun o2-goto-error (module err-pos)
  "Localize error as reported by a failed run time check.  Prompts for module name
and error position.  If the first string is a number, the point is placed at 
this position.  Otherwise the first string is assumed to be a module name. The
current buffer is set to this module, and point set to the second argument 
given."
  (interactive
   (let ((str (read-from-minibuffer "Module name or error position: ")))
     (if (string-match "^[0-9]" str)
	 (list nil (string-to-int str))
       (list str (string-to-int (read-from-minibuffer "Error position: "))))))
  (if (and module
	   (not (string-equal module "")))
      (o2-find-module module))
  (goto-char (1+ err-pos)))




(defun o2-compile-internal (command name)
;; Execute compile function with some specific settings.
  (let (o2-running-compile  ; set flag for compilation hook
	(default-directory o2-cwd)
	(compilation-ask-about-save o2-compilation-ask-about-save)
	(compilation-buffer-name-function
	 (lambda (name-of-mode) (concat "*" name "*"))))
    (compile command)))
  
(defun o2-compile (module &optional compile-command)
  "Compile MODULE (default: the current buffer's file name).  If C-u prefix is
set, read the compilation command from the minibuffer.
Runs the shell command 'o2-compile-command MODULE', in a separate process 
asynchronously with output going to the buffer *o2-compile:MODULE*.
Use the command \\[o2-next-error] to get the next error message
and to move to the source code position that caused it."
  (interactive 
   (list (file-name-nondirectory (buffer-file-name))
	 (if current-prefix-arg
	     (read-from-minibuffer "Compile command: " o2-compile-command
				   nil nil 'o2-compile-history))))
  (if compile-command
      (setq o2-compile-command compile-command))
  (o2-compile-internal (concat o2-compile-command module) 
		       (concat "o2-compile:" module)))

(defun o2-make (command)
  "Make a given module respectively command.
Prompts for a command to be run in a separate process asynchronously with 
output going to the buffer *o2-make:COMMAND*.
You can then use the command \\[o2-next-error] to get the next error message
and move to the source code position that caused it."
  (interactive (list (read-from-minibuffer "Make command: " o2-make-command
					   nil nil 'o2-make-history)))
  (setq o2-make-command command)
  (let ((make-command 
	 (if (string-match "^.*[ \t]\\([a-zA-Z0-9]+\\)[ \t]*$" command)
	     (substring command (match-beginning 1) (match-end 1))
	   "unknown")))
    (o2-compile-internal command (concat "o2-make:" make-command))))

;;; The functions o2-compilation-parse-errors, o2-compilation-goto-locus, and
;;; o2-next-error are closely related to the corresponding functions in 
;;; Roland McGrath's compile.el package.  They are slightly modified to deal
;;; with a different error format generated by the compiler and to display
;;; errors in the message area instead of a seperate buffer.

(defun o2-compilation-parse-errors (limit-search find-at-least)
  "Parse the current buffer as o2c error messages.
See variable `compilation-parse-errors-function' for the interface it uses.
The value of find-at-least is ignored and taken to be nil."
  (setq find-at-least nil)
  (setq compilation-error-list nil)
  (message "Parsing error messages...")
  (let (regexp error-group file-group
	(found-desired nil)
	(compilation-num-errors-found 0))

    ;; Don't reparse messages already seen at last parse.
    (goto-char compilation-parsing-end)
    
    ;; Compile all the regexps we want to search for into one.
    (setq regexp "^\\(In file \\([^:]+\\): \\| *\\([0-9]+\\):\\(.*\\)\\)$"
	  file-group 2
	  error-group 3)

    (while (and (not found-desired)
		;; We don't just pass LIMIT-SEARCH to re-search-forward
		;; because we want to find matches containing LIMIT-SEARCH
		;; but which extend past it.
		(re-search-forward regexp nil t))

      ;; Figure out which constituent regexp matched.
      (cond ((and limit-search (>= (point) limit-search)
		  ;; The user wanted a specific error, and we're past it.
		  ;; We do this check here (and in the leave-group case)
		  ;; rather than at the end of the loop because if the last
		  ;; thing seen is an error message, we must carefully
		  ;; discard the last error when it is the first in a new
		  ;; file (see below in the error-group case).
		  (setq found-desired t)))

	    ((match-beginning file-group)
	     ;; The match is the file name that the following error
	     ;; messages are referring to.
	     ;; The variable o2-source-for-errors is created by 
	     ;; o2-compilation-mode-hook as local variable to this buffer.
	     (setq o2-source-for-errors 
		   (find-file-noselect
		    (concat o2-cwd
			    (buffer-substring (match-beginning file-group)
					      (match-end file-group))))))
	    
	    ((match-beginning error-group)
	     ;; Extract the file position from the error message.
	     (let ((beginning-of-match (match-beginning 0)) ;looking-at nukes
		   (filepos (1+ (string-to-int
				 (buffer-substring
				  (match-beginning error-group)
				  (match-end error-group))))))

	       ;; Locate the erring file position.
	       ;; Cons a new elt onto compilation-error-list,
	       ;; giving a marker for the current compilation buffer
	       ;; location, and a marker for the error.
	       (save-excursion
		 (beginning-of-line 1)
		 (let ((this (cons (point-marker)
				   (set-marker (make-marker)
					       filepos 
					       o2-source-for-errors))))
		   (setq compilation-error-list
			 (cons this
			       compilation-error-list))
		   (setq compilation-num-errors-found
			 (1+ compilation-num-errors-found))))
	       (if (or (and find-at-least (> compilation-num-errors-found
					     find-at-least))
		       (and limit-search (>= (point) limit-search)))
		   (setq found-desired t)))))
      
      (message "Parsing error messages...%d (%.0f%% of buffer)"
	       compilation-num-errors-found
	       ;; Use floating-point because (* 100 (point)) frequently
	       ;; exceeds the range of Emacs Lisp integers.
	       (/ (* 100.0 (point)) (point-max)))
      
      (and limit-search (>= (point) limit-search)
	   ;; The user wanted a specific error, and we're past it.
	   (setq found-desired t)))
    (setq compilation-parsing-end (if found-desired
				      (point)
				    ;; We have searched the whole buffer.
				    (point-max))))
  (setq compilation-error-list (nreverse compilation-error-list))
  (message "Parsing error messages...done"))

(defun o2-compilation-goto-locus (next-error)
  "Jump to an error locus returned by `compilation-next-error-locus'.
Takes one argument, a cons (ERROR . SOURCE) of two markers.
Selects a window with point at SOURCE, and display a message containing the 
error text."
  (if (and (window-dedicated-p (selected-window))
	   (eq (selected-window) (frame-root-window)))
      (switch-to-buffer-other-frame (marker-buffer (cdr next-error)))
    (switch-to-buffer (marker-buffer (cdr next-error))))
  (goto-char (cdr next-error))
  ;; If narrowing got in the way of
  ;; going to the right place, widen.
  (or (= (point) (marker-position (cdr next-error)))
      (progn
	(widen)
	(goto-char (cdr next-error))))

  ;; Show error text in message area
  (save-excursion
    (set-buffer (marker-buffer (car next-error)))
    (goto-char (marker-position (car next-error)))
    (if (looking-at "^ *[0-9]+:\\(.*\\)$")
	(message (buffer-substring (match-beginning 1) (match-end 1)))
      (error "Cannot extract error message"))))

(defun o2-next-error (&optional argp)
  "See function next-error in the standard compile.el package for the general
functionality. This specific function will close all compilation windows and
display the error in the message area."
  (interactive "P")
  ; close compilation windows
  (let ((buffer (compilation-find-buffer))
	 window)
    (while (and (setq window (get-buffer-window buffer))
		(not (one-window-p 'no-mini)))
      (delete-window window))
    (setq compilation-last-buffer buffer))
  (o2-compilation-goto-locus (compilation-next-error-locus
			      ;; We want to pass a number here only if
			      ;; we got a numeric prefix arg, not just C-u.
			      (and (not (consp argp))
				   (prefix-numeric-value argp))
			      (consp argp))))
