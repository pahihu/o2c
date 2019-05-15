;;; oberon2.el --- major mode for Oberon-2 editing and compilation

;; Author: Michael van Acken, oberon1@informatik.uni-kl.de

;; This file is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.


(defvar o2-indent 2 
  "*The indentation per syntax level in Oberon-2-Mode")

(defvar o2-cwd (substitute-in-file-name "$PWD/")
  "*The working directory for programs (compiler, browser, make etc) run in a 
subshell.  If the substitution fails, please set an absolute path for o2-cwd in
your .emacs file, e.g. '(setq o2-cwd \"/users/home/my_home/o2c/\")'")


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
  (let ((map (make-sparse-keymap)))
    (define-key map "\^i" 'o2-tab)
    (define-key map "\^m" 'o2-newline)
    (define-key map "\177" 'backward-delete-char-untabify)
    (define-key map "\C-c\C-i" 'o2-indent-region)
    (define-key map "\C-cm" 'o2-insert-module)
    (define-key map "\C-cp" 'o2-insert-procedure)
    (define-key map "\C-ct" 'o2-insert-typebound-procedure)
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
    (define-key map "\C-c4f" 'o2-find-module-other-window)
    (define-key map "\C-c\C-d" 'o2-showdef-module)
    (define-key map "\C-c4\C-d" 'o2-showdef-module-other-window)
    (define-key map "\C-c4d" 'o2-showdef-module-other-window)
    (define-key map "\C-c." 'o2-find-definition)
    (define-key map "\C-c4." 'o2-find-definition-other-window)
    (define-key map "\C-c," 'o2-find-typebound-procedure)
    (define-key map "\C-c4," 'o2-find-typebound-procedure-other-window)
    (define-key map "\M-c" 'o2-compile)
    (define-key map "\M-\C-c" 'o2-make)
    (define-key map "\C-c'" 'o2-next-error)
    (define-key map "\C-c`" 'o2-next-error)
    (define-key map "\C-cg" 'o2-goto-error)
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
\\[o2-showdef-module]		generate and display the definition of a module
\\[o2-showdef-module-other-window]		like \\[o2-showdef-module], but display in another window
\\[o2-find-definition]		display (same window) the definition of an identifier
\\[o2-find-definition-other-window]		display (other window) the definition of an identifier
\\[o2-find-typebound-procedure]		display (same window) a procedure bound to a given type
\\[o2-find-typebound-procedure-other-window]		display (other window) a procedure bound to a given type
All the above functions accept a module's alias name (as declared in the 
current buffers IMPORT list) instead of the real module name. For further
information on '\\[o2-find-definition-other-window]' or '\\[o2-find-typebound-procedure-other-window]' use \\[describe-key].

Compiling:
\\[o2-compile]		compile current buffer
C-u \\[o2-compile]		prompt for compile command and compile current buffer
\\[o2-make]		run a make on a module
\\[o2-next-error]		display the next error
\\[o2-toggle-warnings]	tells \\[o2-next-error] to ignore or to display warnings
\\[o2-goto-error]		prompt for module and error position, goto position

Searches will distinct cases. Delete converts tabs to spaces as it moves back. 
Emacs won't use tab characters. 
\\[indent-for-comment] will indent or create a comment on the current line at column 41 
(default)."
  (interactive)
  (kill-all-local-variables)
  (setq selective-display t)
  (setq indent-tabs-mode nil)
  (use-local-map o2-mode-map)
  (setq major-mode 'oberon-2-mode)
  (setq mode-name "Oberon-2")
  (make-local-variable 'comment-column)
  (setq comment-column 41)
  (make-local-variable 'end-comment-column)
  (setq end-comment-column 75)
  (set-syntax-table o2-mode-syntax-table)
  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat "^$\\|" page-delimiter))
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)
  (make-local-variable 'paragraph-ignore-fill-prefix)
  (setq paragraph-ignore-fill-prefix t)
  (make-local-variable 'require-final-newline)
  (setq require-final-newline t)
  (make-local-variable 'comment-start)
  (setq comment-start "(* ")
  (make-local-variable 'comment-end)
  (setq comment-end " *)")
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "(\\*+ *")
  (make-local-variable 'comment-indent-hook)
  (setq comment-indent-hook 'c-comment-indent)
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments t)
  (setq case-fold-search nil)
  (run-hooks 'oberon-2-mode-hook))



(defun o2-newline ()
  "Insert a newline and indent following line like previous line."
  (interactive)
  (let ((prev-indent (current-indentation)))
    (newline) 
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
  (interactive "r\nnIndent: ")
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

(defun o2-insert-case (expr)
  "Build skeleton CASE statment, prompting for the <expression>."
  (interactive "sCase expression: ")
  (insert "CASE " expr " OF")
  (o2-newline)
  (o2-newline)
  (insert "END (* case *);")
  (end-of-line 0)
  (insert "| "))

(defun o2-insert-else ()
  "Insert ELSE keyword and indent for next line."
  (interactive)
  (o2-newline)
  (backward-delete-char-untabify o2-indent nil)
  (insert "ELSE")
  (o2-newline)
  (o2-tab))

(defun o2-insert-branch ()
  "Indent for next line and insert '|'."
  (interactive)
  (o2-newline)
  (if (not 
       (save-excursion
	 (forward-line -1)
	 (looking-at "[ \t]*|")))
      (backward-delete-char-untabify o2-indent nil))
  (insert "| "))

(defun o2-insert-for (init end by)
  "Build skeleton FOR loop statment, prompting for the loop parameters."
  (interactive "sInit: \nsend: \nsby: ")
  (insert "FOR " init " TO " end)
  (if (not (string-equal by ""))
	(insert " BY " by))
  (insert " DO")
  (o2-newline)
  (o2-newline)
  (insert "END (* for *);")
  (end-of-line 0)
  (o2-tab))

(defun o2-insert-header (title)
  "Insert a comment block containing the module title, author, etc."
  (interactive "sModule title: ")
  (insert "(*\n    Title: \t" title)
  (insert "\n    Created:\t")
  (insert (current-time-string))
  (insert "\n    Author: \t")
  (insert (user-full-name))
  (insert (concat "\n\t\t<" (user-login-name) "@" (system-name) ">\n"))
  (insert "*)\n\n"))

(defun o2-insert-elsif (guard)
  "Insert ELSIF part, prompting for the guard."
  (interactive "sGuard: ")
  (o2-newline)
  (backward-delete-char-untabify o2-indent nil)
  (insert "ELSIF " guard " THEN")
  (o2-newline)
  (o2-tab))

(defun o2-insert-if (guard)
  "Insert skeleton IF statment, prompting for the guard."
  (interactive "sGuard: ")
  (insert "IF " guard " THEN")
  (o2-newline)
  (o2-newline)
  (insert "END (* if *);")
  (end-of-line 0)
  (o2-tab))

(defun o2-insert-loop ()
  "Build skeleton LOOP."
  (interactive)
  (insert "LOOP")
  (o2-newline)
  (o2-newline)
  (insert "END (* loop *);")
  (end-of-line 0)
  (o2-tab))

(defun o2-insert-module (name)
  "Build skeleton MODULE, prompting for <module-name>."
  (interactive "sModule name: ")
  (insert "MODULE " name ";\n\n\nBEGIN\n")
  (o2-tab)
  (insert "\nEND " name ".\n")
  (previous-line 4))

(defun o2-insert-typebound-procedure (name receiver args result)
  "Insert skeleton PROCEDURE, prompting for <name>, <receiver>, <parameters> 
and the <result-type>." 
  (interactive "sProcedure name: \nsReceiver: \nsFormal parameters: \nsResult type: ")
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

(defun o2-insert-procedure (name args result)
  "Insert skeleton PROCEDURE, prompting for <name>, <parameters> and the
<result-type>." 
  (interactive "sProcedure name: \nsFormal parameters: \nsResult type: ")
  (o2-insert-typebound-procedure name nil args result))

(defun o2-insert-with (guard)
  "Build skeleton WITH statement, prompting for the type guard."
  (interactive "sType guard: ")
  (insert "WITH " guard " DO")
  (o2-newline)
  (o2-newline)
  (insert "END (* with *);")
  (end-of-line 0)
  (o2-tab))

(defun o2-insert-record ()
  "Insert a RECORD outline."
  (interactive)
  (insert "RECORD")
  (o2-newline)
  (o2-newline)
  (insert "END (* record *);")
  (end-of-line 0)
  (o2-tab))

(defun o2-insert-repeat (expr)
  "Build a REPEAT-UNTIL statement, prompting for the <expression>."
  (interactive "sUntil expression: ")
  (insert "REPEAT")
  (o2-newline)
  (o2-newline)
  (insert "UNTIL " expr ";")
  (end-of-line 0)
  (o2-tab))

(defun o2-insert-while (guard)
  "Insert a WHILE statement, prompting for the <guard>."
  (interactive "sWhile guard: ")
  (insert "WHILE " guard " DO")
  (o2-newline)
  (o2-newline)
  (insert "END (* while *);")
  (end-of-line 0)
  (o2-tab))





;;; Here starts the code for the modified outline functions:

(defconst o2-proc-regexp "\\([ \t]*\\)PROCEDURE[ \t]*\\(\\([-*+]\\|([a-zA-Z0-9 \t:]+)\\)[ \t]*\\)?"
;; Ugly. This should match any procedure header (including type bound procedures) until just
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
;; pre: point is inside a procedure (i.e. somewhere between the line of procedure heading
;;      (inclusive) and the ';' following the procedure end delimiter (inclusive).
;;      an error is signaled if point is outside any procedure.
;; post: result is a n-tuple with the elements
;;   name of the procedure
;;   end formal pars (pos directly following the final ;)
;;   end of CONST/TYPE/VAR declaration sequence (only if local procs exist, nil otherwise)
;;   end of local procedure declarations (only if local procs exist, nil otherwise)
;;   end of procedure (char following the final ;)
  (save-excursion
    (let ((curr-point (point)))
      (if (o2-back-to-heading 'consider-nesting)
	  (let (proc-name end-of-heading end-of-proc end-of-decl end-of-local)
	    (looking-at (concat o2-proc-regexp "\\(\\w+\\)[ \t\n]*\\(\\*[ \t\n]*\\)?"))
	    (setq proc-name (buffer-substring (match-beginning 4) (match-end 4))) ; the name of the procedure
	    (goto-char (match-end 0))
	    (if (looking-at "(")
		(o2-search ")\\([ \t\n]*:[ \t\n]*[a-zA-Z0-9.]+\\)?[ \t\n]*;"
			   "no end of formal parameters") ; skip formal parameters and result
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
	    (list proc-name end-of-heading end-of-decl end-of-local end-of-proc))
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
;; returned with the starting point of the procedure declaration appended to it.  
;; Otherwise result is nil.
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
		(let ((proc-name (buffer-substring (match-beginning 6) (match-end 6))))		
		  (re-search-backward (concat "^" o2-proc-regexp proc-name "[ \t* ;(]") nil 'move)))))
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
      (goto-char (nth 5 proc-info))
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
;; Accepts a (possibly qualified) identifier (see Oberon Report), and seperates it into
;; module and ident part. If the module name is given (qualident is something 'M.ident')
;; the module name is extracted and possibly converted if it is an alias from the
;; IMPORT list. The result is (module-name . ident).
;; If no module is given (qualident is "ident") the function returns (nil . ident).
;; If the qualident contains illegal characters it will result to nil.
  (if (string-match "^\\(\\(\\w+\\) *\\. *\\)?\\(\\w+\\)$" qualident)
      (cons
       (if (match-end 1) (o2-module-name (substring qualident 
						    (match-beginning 2) 
						    (match-end 2))))
       (substring qualident (match-beginning 3) (match-end 3)))
    (error "Not legal qualified identifier.")))



(defun o2-switch-to (location &optional other-window)
  (if other-window
      (switch-to-buffer-other-window (car location))
    (switch-to-buffer (car location)))
  (if (cdr location)
      (goto-char (cdr location))))

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
    (o2-execute-command (concat "o2whereis " file-name))
    (goto-char (point-min))
    (if (re-search-forward (concat "^.*" file-name "$") nil t)
	(expand-file-name (buffer-substring (match-beginning 0) (match-end 0))
			  o2-cwd)
      (if no-error
	  nil
	(error "Cannot locate file %s." file-name)))))
  
(defun o2-find-module-noselect (module-name)
;; Calls the external command o2whereis with the parameter modulename.Mod and reads
;; it (if necessary) into a buffer. The buffer will be returned (but not displayed).
;; If no such file exists, a error message will be printed.
  (find-file-noselect (o2-locate-file (o2-module-name module-name) "Mod")))

(defun o2-find-module (module-name)
"Call the external command o2whereis with the parameter 'MODULE-NAME.Mod' to 
find the associated file. If no such file exists, a error message will be
printed."
  (interactive "sModule name: ")
  (o2-switch-to (cons (o2-find-module-noselect module-name) nil)))

(defun o2-find-module-other-window (module-name)
"Like o2-find-module, but display the file in another window."
  (interactive "sModule name: ")
  (o2-switch-to (cons (o2-find-module-noselect module-name) nil) t))

(defun o2-showdef-module-noselect (module-name)
;; Call the external command o2showdef which extracts the interface of a module out of a
;; given symbol file. The buffer gets the name *modulename.Def* and is not selected.
  (save-excursion
    (setq module-name (o2-module-name module-name))
    (let* ((defname (concat "*" module-name ".Def*"))
	   (buffer (get-buffer-create defname)))
      (set-buffer buffer)
      (oberon-2-mode)
      (let ((buffer-read-only nil))
	(erase-buffer)
	(o2-execute-command (concat "o2b -x " module-name)))
      (setq buffer-read-only t)
      (list buffer))))

(defun o2-showdef-module (module-name)
"Call the external command o2showdef which extracts the interface of the module
MODULE-NAME out of a given symbol file. The buffer gets the name
*MODULE-NAME.Def*."
  (interactive "sModule name: ")
  (o2-switch-to (o2-showdef-module-noselect module-name))
  (oberon-2-mode))

(defun o2-showdef-module-other-window (module-name)
"Like o2-showdef-module, but display the file in another window."
  (interactive "sModule name: ")
  (o2-switch-to (o2-showdef-module-noselect module-name) t))



(defun o2-declarationp ()
;; Is t iff point is inside the declaration part of a module or procedure.
  (save-excursion
    (if (save-excursion (re-search-backward "^DEFINITION " nil t))
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
;; Searches the current buffer for the position of identifier's ident declaration
;; valid at position pos. The result is the starting point of the declaration or
;; nil if it cannot be found.
;; ident cannot be a qualified identifier.
;; This function will not merely do a backward search, but will scan only the 
;; relevant declaration scopes that may contain the declaration it is searching
;; for. It cannot find record fields or type bound procedures. 
  (save-excursion
    (goto-char pos)
    (let* (terminate
	   (procpattern (concat "PROCEDURE[ \t-*+^]+" ident "[ \t* ;(]"))
	   (endpattern (o2-proc-end-delimiter "\\(\\w+\\)"))
	   (pattern (concat "\\W" 
			    "\\(\\(" procpattern "\\)"
			    "\\|\\(" ident "[ \t*-]*:[^=]\\)"
			    "\\|\\(" ident "[ \t*-]*=\\)"
			    "\\|\\(" endpattern "\\)\\)")))
      (while (not (or terminate (bobp)))
	(if (re-search-backward pattern nil 'move)
	    (cond
	     ((or (match-beginning 2)
		  (and (match-beginning 3) (o2-declarationp) (o2-no-recordp))
		  (and (match-beginning 4) (o2-declarationp)))
	      (setq terminate (1+ (point))))
	     ((match-beginning 5)
	      (goto-char (match-beginning 5))
	      (looking-at endpattern)	; include identifier parts beyond current point
	      (let ((proc-name (buffer-substring (match-beginning 1) (match-end 1))))		
		(if (and (re-search-backward (concat o2-proc-regexp proc-name "\\W") nil 'move)  
			 (string= proc-name ident)
			 (> 1 (- (match-end 1) (match-beginning 1))))
		    (setq terminate (match-end 1))))))))
      terminate)))

(defun o2-default-qualident ()
;; Scan the buffer at (and before) the current point for a qualified identifier.
;; Return is this qualified identifier or nil if none is found.
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
	    (if (looking-at "\\(\\w+\\) *\\. *\\w+")
		(if (assoc (buffer-substring (match-beginning 1) (match-end 1)) 
			   (o2-import-list))
		    (buffer-substring (match-beginning 0) (match-end 0))
		  (buffer-substring (match-beginning 1) (match-end 1)))
	      (looking-at "\\w+")
	      (buffer-substring (match-beginning 0) (match-end 0))))))))
	      

(defun o2-read-qualident (string)
;; Prompt for a qualident, offer the ident under point (or before) as default.
  (let* ((default (o2-default-qualident))
	 (spec (read-string
		(if default
		    (format "%s(default %s) " string default)
		  string))))
    (list (if (equal spec "")
	      default
	    spec))))

(defun o2-limit-to-decl (pos limit-to-decl)
;; If limit-to-decl is non nil, return minimum of position of first procedure and pos, 
;; otherwise return pos.
  (if limit-to-decl
      (min
       (save-excursion
	 (goto-char (point-min))
	 (re-search-forward (concat o2-proc-regexp "\\w+") nil 'move)
	 (point))
       pos)
    pos))
	 
(defun o2-find-definition-noselect (qualident &optional limit-to-decl)
;; Find the definition of a given qualified identifier: result is (buffer . point).
;; If a buffer different from the current one has to be searched, it will not get selected.
;; There are three flavours:
;; 1. No module name is given. The function tries to locate the definition of
;;    the identifier in the current module.
;;    The search may fail on forward declared pointer base types. If the search
;;    at the current position fails, it is repeated with the position set to the
;;     end of the local declaration block (if possible).
;; 2. A module name is given and the sources of the module are available.
;;    The module is loaded and the cursor jumps to the position of the declaration.
;; 3. If nor 1. nor 2. holds, then the definition of the module is extracted from
;;    it's symbol file and used instead.
;; 4. If you entered a module name, 2. and 3. failed, then you will get the error message 
;;    'cannot find file xxx.OSym'.
;; limit-to-decl will bound the search for the declaration part (everything before the
;;   first normal procedure) of the module
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
	    (error "Can't find declaration of %s." ident)))))))

(defun o2-find-definition-other-window (qualident &optional limit-to-decl)
  "Like o2-find-definition, but file will be displayed in another window."
;; limit-to-decl will bound the search for the declaration part (everything
;; before the first normal procedure) of the module.
  (interactive (o2-read-qualident "(qualified) Identifier: "))
  (o2-switch-to (o2-find-definition-noselect qualident limit-to-decl) t))
	
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
  (interactive (o2-read-qualident "(qualified) Identifier: "))
  (o2-switch-to (o2-find-definition-noselect qualident limit-to-decl)))
	


  
(defun o2-get-pointer-to (record-name)
;; Finds the name of the first declared data type 'POINTER TO record-name' in the 
;; modules declaration block, or nil if the search fails.
  (save-excursion
    (goto-char (point-min))
    (if (re-search-forward (concat "\\W\\(\\w+\\)[ \t\n*-]*=\\s *POINTER\\s +TO\\s +" record-name "\\W") 
			   (o2-limit-to-decl (point-max) t) t)
	(buffer-substring (match-beginning 1) (match-end 1))
      (error "Can't find any declaration of type 'POINTER TO %s'." record-name))))

(defun o2-find-typebound-procedure-noselect (receiver proc-name &optional prev-path conv-to-pointer)
;; Takes a receiver data type (possibly qualified) and the name of a type bound
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
        ; third we try to locate the type bound procedure to the current receiver type
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
  (interactive "sReceiver type: \nsProcedure name: ")
  (o2-switch-to (o2-find-typebound-procedure-noselect receiver proc-name) t))

(defun o2-find-typebound-procedure (receiver proc-name)
"Find to a data type RECEIVER the typebound procedure PROC-NAME bound to it. If
the procedure is inherited from the receiver's base type, the search is
automatically extended to this base type and so on. RECEIVER has to be a 
(possibly qualified) identifier.
The search may take a while."
  (interactive "sReceiver type: \nsProcedure name: ")
  (o2-switch-to (o2-find-typebound-procedure-noselect receiver proc-name)))





;; Adaption of the 'compile.el' package:

(defvar o2-compile-save-buffers 'save-modules
  "*Controls if modified file are saved when invoking a compile command.
nil means no saving at all, t saves all files without asking, 'save-modules
will save all buffers whose file name ends in .Mod and any other
value will ask for every file if it should be saved.")

(defvar o2-compilation-process nil
  "Process created by compile command, or nil if none exists at the moment.
Note that the process may have been \"deleted\" and still be the value of this 
variable.")

(defvar o2-compilation-error-list nil
  "List of error message descriptors for visiting erring functions.
Each error descriptor is a list of length two: '(error-mark error-msg).
error-mark holds the buffer and the position the error is referring to,
error-msg the error text.")

(defvar o2-error-point nil
  "Points to the end of the parsed errors in *oberon-2 compilation*.")

(defvar o2-error-filename nil
  "The currently parsed error messages are refering to this file.")


(defvar o2-compile-command "o2c -v -W "
  "*String passed to the shell to start the compilation of the current buffer.
Used by command \\[o2-compile].")

(defvar o2-make-command "o2c --make -v "
  "*String passed to the shell to start the make programm.
Used by command \\[o2-make].")

(defvar o2-skip-warnings nil
  "*\\[o2-next-error] will ignore warnings if this variable is not nil.")


(defun o2-compile (module &optional compile-command)
  "Compile MODULE (default: the current buffer's file name).  
Runs the shell command 'o2-compile-command MODULE', in a separate process 
asynchronously with output going to the buffer *oberon-2 compilation*.
Use the command \\[o2-next-error] to get the next error message
and to move to the source code position that caused it."
  (interactive 
   (list (file-name-nondirectory (buffer-file-name))
	 (if current-prefix-arg
	     (read-from-minibuffer "Compile command: " o2-compile-command))))
  (if compile-command
      (setq o2-compile-command compile-command))
  (o2-compile1 (concat o2-compile-command module)))

(defun o2-make (command)
  "Make a given module respectively command.
Prompts for a command to be run in a separate process asynchronously with 
output going to the buffer *oberon-2 compilation*.
You can then use the command \\[o2-next-error] to get the next error message
and move to the source code position that caused it."
  (interactive (list (read-from-minibuffer "Make command: " o2-make-command)))
  (setq o2-make-command command)
  (o2-compile1 o2-make-command))





(defun o2-compile1 (command &optional name-of-mode)
  ; save modified buffers
  (if o2-compile-save-buffers
      (if (not (eq o2-compile-save-buffers 'save-modules))
	  (save-some-buffers (eq o2-compile-save-buffers t))
	; save all buffers whose file name ends in .Mod 
	(mapcar 
	 (lambda (buffer)
	   (if (and (buffer-file-name buffer)
		    (string-match "\\.Mod$" (buffer-file-name buffer)))
	       (save-excursion
		 (set-buffer buffer)
		 (save-buffer))))
	 (buffer-list))))
  ; check whether a compilation process is running
  (if o2-compilation-process
      (if (or (not (eq (process-status o2-compilation-process) 'run))
	      (yes-or-no-p "A compilation process is running; kill it? "))
	  (condition-case ()
	      (if o2-compilation-process
		  (let ((comp-proc o2-compilation-process))
		    (interrupt-process comp-proc)
		    (sit-for 1)
		    (delete-process comp-proc)))
	    (error nil))
	(error "Cannot handle two compilation processes.")))
  (o2-compilation-forget-errors)
  ; start new compilation process with 'command' in the directory o2-cwd
  (let ((default-directory o2-cwd))
    (setq o2-compilation-process
	  (start-process "o2-compilation" "*oberon-2 compilation*"
			 shell-file-name
			 "-c" (concat "exec " command)))
    (set-process-sentinel o2-compilation-process 'o2-compilation-sentinel)
    ; write something to the output buffer and init error parsing
    (with-output-to-temp-buffer "*oberon-2 compilation*"
      (princ "cd ") (princ (file-name-directory default-directory)) (terpri)
      (princ command) (terpri) (terpri))
    (let* ((thisdir default-directory)
	   (outbuf (process-buffer o2-compilation-process))
	   (outwin (get-buffer-window outbuf)))
      (if (eq outbuf (current-buffer))
	  (goto-char (point-max)))
      (save-excursion
	(set-buffer outbuf)
	(setq o2-error-point (point-min-marker))
	(setq o2-error-filename "")
	(buffer-flush-undo outbuf)
	(let ((start (save-excursion (set-buffer outbuf) (point-min))))
	  (set-window-start outwin start)
	  (or (eq outwin (selected-window))
	      (set-window-point outwin start)))
	(setq default-directory thisdir)
	(oberon-2-mode)
	(setq mode-name (or name-of-mode "oberon-2 compilation"))
	;; Make log buffer's mode line show process state
	(setq mode-line-process '(": %s"))))))

;; Called when compilation process changes state.
(defun o2-compilation-sentinel (proc msg)
  (cond ((null (buffer-name (process-buffer proc)))
	 ;; buffer killed
	 (set-process-buffer proc nil))
	((memq (process-status proc) '(signal exit))
	 (let* ((obuf (current-buffer))
		omax opoint)
	   ;; save-excursion isn't the right thing if
	   ;;  process-buffer is current-buffer
	   (unwind-protect
	       (progn
		 ;; Write something in *oberon-2 compilation* and hack its mode line
		 (set-buffer (process-buffer proc))
		 (setq omax (point-max) opoint (point))
		 (goto-char (point-max))
		 (insert ?\n mode-name " " msg)
		 (forward-char -1)
		 (insert " at "
			 (substring (current-time-string) 0 -5))
		 (forward-char 1)
		 (setq mode-line-process
		       (concat ": "
			       (symbol-name (process-status proc))))
		 ;; If buffer and mode line will show that the process
		 ;; is dead, we can delete it now.  Otherwise it
		 ;; will stay around until M-x list-processes.
		 (delete-process proc))
	     (setq o2-compilation-process nil)
	     ;; Force mode line redisplay soon
	     (set-buffer-modified-p (buffer-modified-p)))	   
	   (o2-fit-to-window)		; scroll to end of buffer
	   (set-buffer obuf)))))

(defun o2-kill-compilation ()
  "Kill the process created by \\[o2-compile] or \\[o2-make]."
  (interactive)
  (if o2-compilation-process
      (interrupt-process o2-compilation-process)))


(defun o2-compilation-forget-errors ()
;; Set o2-compilation-error-list to nil, and
;; unchain the markers that point to the error messages and their text.
;; This way they will no longer slow down gap motion.
;; This would happen anyway at the next garbage collection,
;; but it is better to do it right away.
  (while o2-compilation-error-list
    (let ((next-error (car o2-compilation-error-list)))
      (set-marker (car next-error) nil))
    (setq o2-compilation-error-list (cdr o2-compilation-error-list))))

(defun o2-set-compilation-buffer () 
; Set buffer *oberon-2 compilation* if one exists, otherwise signal error.
  (if (get-buffer "*oberon-2 compilation*")
      (set-buffer "*oberon-2 compilation*")
    (error "No compilation in progress.")))

(defun o2-parse-errors ()
  (save-excursion
    (message "Parsing error messages...")
    (o2-set-compilation-buffer)		; set buffer *oberon-2 compilation*
    (goto-char o2-error-point)		; move to end of alreay parsed errors
    (while (not (eobp))
      (if (re-search-forward "^\\(In file \\([a-zA-Z0-9./]+\\):\\)\\|^\\([0-9]+\\):\\(.*\\)$" nil 'move)
	  (cond 
	   ((match-beginning 1)		; matches the header containing the file name
	    (setq o2-error-filename (expand-file-name (buffer-substring (match-beginning 2) (match-end 2)))))
	   ((match-beginning 3)		; matches an error message
					; extract the error position from the match
					; extract the error message
					; get the file's buffer the error is referring to (read it if necessary)
					; create marker
	    (let* ((error-pos (1+ (string-to-int (buffer-substring (match-beginning 3) (match-end 3)))))
		   (error-msg (buffer-substring (match-beginning 4) (match-end 4)))
		   (error-buffer (if (file-exists-p o2-error-filename)
				     (find-file-noselect o2-error-filename)
				   (error "Can't find file %s." o2-error-filename)))
		   (marker (make-marker)))
	      (set-marker marker error-pos error-buffer) ; set marker to the error buffer+position
	      (setq o2-compilation-error-list (append o2-compilation-error-list 
						      (list (list marker error-msg)))))))))
    (set-marker o2-error-point (point)))) ; remember position behind last parsed error
       
(defun o2-next-error (&optional rescan)
  "Visit next compilation error message and corresponding source code position."
  (interactive "P")
  ;; close compilation windows
  (let (window)
    (while (setq window (get-buffer-window "*oberon-2 compilation*"))
      (delete-window window)))    
  (if rescan
      ; reset o2-error-point and the error list to rescan errors
      (save-excursion
	(o2-set-compilation-buffer)
	(setq o2-error-point (point-min-marker))
	(o2-compilation-forget-errors)))
  ; parse next error if list is exhausted
  (if (not o2-compilation-error-list)
      (o2-parse-errors))
  ; skip following warnings is o2-skip-warnings is set
  (while (and o2-skip-warnings
	      o2-compilation-error-list 
	      (string-match "[ 0-9]+ +[wW]arn"
			    (nth 1 (car o2-compilation-error-list))))
    (set-marker (nth 0 (car o2-compilation-error-list)) nil)
    (setq o2-compilation-error-list (cdr o2-compilation-error-list)))
  ; display error 
  (if o2-compilation-error-list
      ; go to next error, print it's text
      (let* ((next-error (car o2-compilation-error-list))
	     (error-mark (nth 0 next-error))
	     (error-msg (nth 1 next-error)))
	; remove current error from list
	(setq o2-compilation-error-list (cdr o2-compilation-error-list))
	(switch-to-buffer (marker-buffer error-mark))
	(goto-char (marker-position error-mark))
	; show procedure if error is inside hidden text
	(save-excursion
	  (if (and (re-search-backward "[\n]" nil t)
		   (eq (following-char) ?))
	      (o2-show-proc)))
	(message error-msg)
	; twist marker to point to void
	(set-marker error-mark nil))
    (message "No more errors.")))

(defun o2-toggle-warnings ()
  "Determines wheter \\[o2-next-error] will ignore warnings (default: display
warnings)."
  (interactive)
  (setq o2-skip-warnings (not o2-skip-warnings))
  (message
   (if o2-skip-warnings
       "Warnings will be ignored."
     "Warnings will be displayed.")))


(defun o2-fit-to-window ()
; Reset the window-start of the current buffer so that (point-max) is 
; displayed in the last line of window.
  (let* ((current (selected-window))
	 (window (get-buffer-window (current-buffer))))
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
and error position."
  (interactive "sModule name (default: current buffer): \nnError position: ")
  (if (not (string= module ""))
      (o2-find-module module))
  (goto-char (1+ err-pos)))
