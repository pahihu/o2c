MODULE o2ef;  (* error filter for o2c; Author: Michael van Acken *)
(* 	$Id: o2ef.Mod,v 1.7 1995/08/10 11:39:22 oberon1 Exp $	 *)

IMPORT
  Rts, In, Out, Dos, Str := Strings, Strings2, IntStr, ConvTypes, Files, 
  Char := CharInfo;

CONST
  tab = 8;                               (* tab stop position *)
  context = 0;                           (* display error in context *)
  lineOnly = 1;                          (* display as file:line:message *)
  lineColumn = 2;                        (* display as file:line,col:message *)
  
CONST
  sizePath = 256;
  sizeErrText = 100;

TYPE 
  Filename = ARRAY sizePath OF CHAR;
  Error = POINTER TO ErrorDesc;
  FileInfo = POINTER TO FileInfoDesc;
  ErrorDesc = RECORD 
    next: Error;
    pos: LONGINT;
    text: ARRAY sizeErrText OF CHAR
  END;
  FileInfoDesc = RECORD 
    next: FileInfo;
    name: Filename;
    errList: Error
  END;

VAR
  files: FileInfo;
  fileBuffer: POINTER TO ARRAY OF CHAR;
  bufferSize: LONGINT;
  
  echoErrors: BOOLEAN;                   (* echo errors while scanning them *)
  echoText: BOOLEAN;                     (* echo all non-error messages *)
  mode: INTEGER;                         (* contect, lineOnly, lineColumn *)

  leadContext: LONGINT;                  (* context lines before error *)
  trailContext: LONGINT;                 (* context lines behind error *)
  connContext: LONGINT;                  (* connect context less/equal that many lines apart *)
  
PROCEDURE ActiveFile (name: ARRAY OF CHAR);
(* Move information block on file `name' to the start of the list `files'. *)
  VAR
    f, g: FileInfo;
  BEGIN
    (* search for `name' in list of known files *)
    f := files;
    WHILE (f # NIL) & (f. name # name) DO
      f := f. next
    END;
    IF (f = NIL) THEN  (* no `name' in list: add new entry as first element *)
      NEW (f);
      f. next := files;
      COPY (name, f. name);
      f. errList := NIL;
      files := f
    ELSIF (f # files) THEN  (* `name' found: move entry to start of list *)
      g := files;
      WHILE (g. next # f) DO
        g := files. next
      END;
      g. next := f. next;
      f. next := files;
      files := f
    END
  END ActiveFile;

PROCEDURE AddError (VAR list: Error; pos: LONGINT; text: ARRAY OF CHAR);
(* Append error message (pos, text) to `list'. *)
  BEGIN
    IF (list # NIL) THEN
      AddError (list. next, pos, text)
    ELSE
      NEW (list);
      list. next := NIL;
      list. pos := pos;
      COPY (text, list. text)
    END
  END AddError;

PROCEDURE ScanInput;
(* Scan input for error messages and add them to the data structure in `files'
   respectively `files.errList'. *)
  VAR
    i: INTEGER;
    ch: CHAR;
    str, text: ARRAY 1024 OF CHAR;
    number: ARRAY 16 OF CHAR;
    filename: Filename;
    
  PROCEDURE Ln;
    BEGIN
      (* only do a newline if the input stream contained one *)
      IF (ch = Char.eol) THEN
        Out.Ln
      END
    END Ln;
    
  BEGIN
    WHILE In.Done DO
      (* read line of text *)
      i := 0;
      LOOP
        In.Char (ch);
        IF ~In.Done OR (ch = Char.eol) THEN
          EXIT
        ELSIF (i < LEN(str)-1) THEN
          str[i] := ch;
          INC (i)
        END
      END;
      str[i] := 0X;
      (* interpret line str *)
      IF Strings2.Match ("In file *: ", str) THEN
        (* file that error messages are refering to *)
        IF echoErrors THEN
          Out.String (str);
          Ln
        END;
        Str.Extract (str, 8, Str.Length (str)-10, filename);
        ActiveFile (filename)
      ELSIF Strings2.Match ("*:* *", str) THEN
        (* error message, refering to the file in `files' *)
        IF echoErrors THEN
          Out.String (str);
          Ln
        END;
        (* locate position of first `:' *)
        i := 0;
        WHILE (str[i] # ":") DO
          INC (i)
        END;
        (* extract and convert char number *)
        Str.Extract (str, 0, i, number);
        IF (IntStr.Format (number) = ConvTypes.allRight) & (files # NIL) THEN
          (* add (number, text) to error list of file `files' *)
          Str.Extract (str, i+1, Str.Length (str)-i-1, text);
          AddError (files. errList, IntStr.Value (number), text)
        ELSIF echoText THEN
          (* no leading number, text ignored *)
          Out.String (str);
          Ln
        END
      ELSIF echoText THEN
        (* additional text, ignored *)
        Out.String (str);
        Ln
      END
    END
  END ScanInput;

PROCEDURE SortErrors;
(* Sort error messages with increasing file position. *)
  VAR
    f: FileInfo;
    newList, e: Error;
    
  PROCEDURE RemLargestPos (VAR list: Error; max: LONGINT): Error;
  (* If `list' contains an error message whose position is greater/equal to
     `max', remove the largest position from `list' and return it.  Otherwise 
     return NIL. *)
    VAR
      e: Error;
    BEGIN
      IF (list = NIL) THEN
        RETURN NIL
      ELSE
        IF (list. pos >= max) THEN
          e := RemLargestPos (list. next, list. pos);
          IF (e = NIL) THEN
            e := list;
            list := list. next
          END;
          RETURN e
        ELSE
          RETURN RemLargestPos (list. next, max)
        END
      END
    END RemLargestPos;
    
  BEGIN
    f := files;
    WHILE (f # NIL) DO    
      newList := NIL;
      WHILE (f. errList # NIL) DO
        e := RemLargestPos (f. errList, -MAX (LONGINT)-1);
        e. next := newList;
        newList := e
      END;
      f. errList := newList;
      f := f. next
    END
  END SortErrors;
  
PROCEDURE PrintErrors;
(* Write error messages to stdout.  Format is determined by `mode'. *)
  VAR
    f: FileInfo;
    e: Error;
    fileSize, scanPos, lineStart, lastPos, col, i: LONGINT;
    scanLine: LONGINT;
    file: Files.File;
    rider: Files.Rider;

  PROCEDURE CountLines (start, end: LONGINT): LONGINT;
  (* Count newline characters in the intervall [start..end[. *)
    VAR
      count: LONGINT;
    BEGIN
      count := 0;
      WHILE (start < end) DO
        IF (fileBuffer[start] = Char.eol) THEN
          INC (count)
        END;
        INC (start)
      END;
      RETURN count
    END CountLines;
  
  PROCEDURE SkipLines (pos, n: LONGINT): LONGINT;
  (* Return position of n-th previous (n<0) or n-th following line.  For
     n=0 result is the beginning of the current line.  Lines skipped may be
     less than demanded if the end (or beginning) of the file is reached 
     first. *)
    BEGIN
      IF (n <= 0) THEN
        WHILE (pos > 0) DO
          IF (fileBuffer[pos-1] = Char.eol) THEN
            IF (n = 0) THEN
              RETURN pos
            ELSE
              INC (n)
            END
          END;
          DEC (pos)
        END;
        RETURN 0
      ELSE
        WHILE (pos < fileSize) DO
          IF (fileBuffer[pos] = Char.eol) THEN
            IF (n = 1) THEN
              RETURN pos+1
            ELSE
              DEC (n)
            END
          END;
          INC (pos)
        END;
        RETURN fileSize
      END
    END SkipLines;
  
  PROCEDURE PrintText (start, end: LONGINT);
  (* Print text in intervall [start..end[.  Add a newline if end=fileSize, 
     end>start, fileBuffer[end-1]#eol. *)
    BEGIN
      WHILE (start < end) DO
        Out.Char (fileBuffer[start]);
        INC (start)
      END;
      IF (end = fileSize) & (start < end) & (fileBuffer[end-1] # Char.eol) THEN
        Out.Char (Char.eol)
      END
    END PrintText;

  PROCEDURE Column (pos: LONGINT): LONGINT;
  (* Calculate column of file position `pos'.  `lineStart' is the starting
     position of the current line, no newline characters allow in 
     [lineStart..pos[.  Tab stops are `tab' apart.  1 is the rightmost 
     column. *)
    VAR
      p, c: LONGINT;
    BEGIN
      p := lineStart;
      c := 0;
      WHILE (p < pos) DO
        IF (fileBuffer[p] = Char.ht) THEN
          c := c+tab-c MOD tab
        ELSE
          INC (c)
        END;
        INC (p)
      END;
      RETURN c+1
    END Column;
    
  BEGIN
    f := files;
    WHILE (f # NIL) DO
      (* write header for context output *)
      IF (mode = context) THEN
        Out.String ("In file ");
        Out.String (f. name);
        Out.String (":");
        Out.Ln
      END;
      
      (* read file `f.name' into `fileBuffer' *)
      IF Dos.Exists(f.name) THEN
        file := Files.Old (f. name);
        IF file = NIL THEN
          Out.String ("Error (fatal): Can't open file ");
          Out.String (f. name);
          Out.Ln;
          RETURN
        END;
        Files.Set (rider, file, 0);
        fileSize := Files.Length (file);
        IF (fileSize > bufferSize) THEN
          NEW (fileBuffer, fileSize);
          bufferSize := fileSize
        END;
        Files.ReadBytes (rider, fileBuffer^, fileSize);
        IF (rider. res # 0) THEN
          Out.String ("Error (fatal): Error while reading from ");
          Out.String (f. name);
          Out.Ln;
          RETURN
        END;
        Files.Close (file);
      
        (* print error messages *)
        lastPos := -1;
        lineStart := 0;
        scanPos := 0;
        scanLine := 1;
        e := f. errList;
        WHILE (e # NIL) DO
          (* find line number for error position `e.pos' *)
          WHILE (scanPos < e. pos) DO
            IF (fileBuffer[scanPos] = Char.eol) THEN
              INC (scanLine);
              lineStart := scanPos+1
            END;
            INC (scanPos)
          END;
          IF (mode = context) THEN  
            (* display lines of source code around error *)
            IF (lastPos >= 0) & (CountLines (lastPos, scanPos)-1 <= 
                                   leadContext+trailContext+connContext) THEN
              (* print all lines between last and current error plus the 
                 current line *)
              PrintText (SkipLines (lastPos, 1), SkipLines (scanPos, 1))
            ELSE
              IF (lastPos >= 0) THEN  (* print context behind last error *)
                PrintText (SkipLines (lastPos, 1), 
                           SkipLines (lastPos, trailContext+1));
                Out.Ln;
                Out.String ("...");
                Out.Ln;
                Out.Ln
              END;
              (* print context before current error plus the current line *)
              PrintText (SkipLines (scanPos, -leadContext), 
                         SkipLines (scanPos, 1))
            END;
            (* write error marker and error message *)
            IF (lastPos # scanPos) THEN
              col := Column (e. pos);
              i := 1;
              WHILE (i < col) DO
                IF (i <= 1) THEN
                  Out.Char ("#")
                ELSE
                  Out.Char ("-")
                END;
                INC (i)
              END;
              Out.Char ("^");
              Out.Ln
            END;
            Out.String ("# ");
            Out.Int (scanLine, 0);
            Out.String (": ");
            Out.String (e. text);
            Out.Ln;
            lastPos := scanPos
          ELSE
            Out.String (f. name);
            Out.Char (":");
            Out.Int (scanLine, 0);
            IF (mode = lineColumn) THEN
              Out.Char (",");
              Out.Int (Column (e. pos), 0)
            END;
            Out.Char (":");
            Out.String (e. text);
            Out.Ln
          END;
          e := e. next
        END;
        IF (mode = context) THEN
          (* add trailing context lines of last error message *)
          PrintText (SkipLines (lastPos, 1), 
                     SkipLines (lastPos, 1+trailContext))
        END
      END;        
      f := f. next
    END
  END PrintErrors;



PROCEDURE EvalOptions(): BOOLEAN;  
  VAR
    arg: ARRAY 256 OF CHAR;

  PROCEDURE PrintHelp;
    BEGIN
      Out.String ("Usage: o2c ... | o2ef [-c | -l | -L]"); Out.Ln;
      Out.String ("Filters error messages generated by o2c and transforms them into"); Out.Ln;
      Out.String ("another representation."); Out.Ln;
      Out.String ("Options: "); Out.Ln;
      Out.String ("-c  Print errors in source text context (default)."); Out.Ln;
      Out.String ("-l  Print errors in the format `file:line:error'."); Out.Ln;
      Out.String ("-L  Print errors in the format `file:line,column:error'."); Out.Ln
    END PrintHelp;
    
  BEGIN  
    IF (Rts.ArgNumber() > 1) THEN
      PrintHelp;
      RETURN FALSE
    ELSIF (Rts.ArgNumber() = 1) THEN
      Rts.GetArg (1, arg);
      IF (arg = "-c") THEN
        mode := context
      ELSIF (arg = "-l") THEN
        mode := lineOnly
      ELSIF (arg = "-L") THEN
        mode := lineColumn
      ELSE  
        IF (arg # "-h") & (arg # "--help") THEN
          Out.String ("o2ef: Unknown option ");
          Out.String (arg);
          Out.Char (".");
          Out.Ln
        END;
        PrintHelp;
        RETURN FALSE
      END      
    END;
    RETURN TRUE
  END EvalOptions;


BEGIN
  files := NIL;
  fileBuffer := NIL;
  bufferSize := -1;
  echoErrors := FALSE;
  echoText := TRUE;
  mode := context;
  leadContext := 3;
  trailContext := 3;
  connContext := 3;
  
  In.Open;
  IF EvalOptions() THEN
    ScanInput;
    SortErrors;
    PrintErrors
  END
END o2ef.

