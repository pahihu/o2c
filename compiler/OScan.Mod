MODULE OScan;  (* Author: Michael van Acken *)
(* 	$Id: OScan.Mod,v 1.41 1996/07/17 16:54:56 oberon1 Exp $	 *)


IMPORT
  M := OMachine, F := Files, Dos, Out, RealStr,  Rts, Conv := ConvTypes, 
  Redir, Str := Strings, Strings2;

CONST
  maxSizeNumber = 64;                    (* maximum of digits allowed in number *)
  undefStr* = "???";

  nul = 00X;                             (* marker for end of string *)
  eof = 00X;                             (* end of file marker, always added to the buffer *)
  eol = 0AX;                             (* end of line marker, used to compute line/col of a given file position *)
  sizeKWTable = 85;                      (* size of hashtable for the keywords *)
  offKWTable = -29;                      (* offset used when collisions in hashtable occur *)
  undefPos* = -1;                        (* undefined file position *)

  (* kinds of tokens returned by 'GetSym' in 'sym' *)
  times*=1; slash*=2; div*=3; mod*=4;
  and*=5; plus*=6; minus*=7; or*=8; eql*=9;
  neq*=10; lss*=11; leq*=12; gtr*=13; geq*=14;
  in*=15; is*=16; arrow*=17; period*=18; comma*=19;
  colon*=20; upto*=21; rParen*=22; rBrak*=23; rBrace*=24;
  of*=25; then*=26; do*=27; to*=28; by*=29; lParen*=30;
  lBrak*=31; lBrace*=32; not*=33; becomes*=34; number*=35;
  nil*=36; string*=37; ident*=38; semicolon*=39;
  bar*=40; end*=41; else*=42; elsif*=43; until*=44;
  if*=45; case*=46; while*=47; repeat*=48; loop*=49;
  for*=50; with*=51; exit*=52; return*=53;
  array*=54; record*=55; pointer*=56; begin*=57; const*=58;
  type*=59; var*=60; procedure*=61;
  import*=62; module*=63; endOfFile*=64;

  (* values for 'numType' *)
  numInt*=1; numReal*=2; numLReal*=3;

  (* file buffer managment *)
  maxBufferRest = M.maxSizeString+4;
  minBufferSize* = 2*maxBufferRest;
  maxBufferSize* = 32*1024-1;  (* has to be INTEGER *)

  sizeErrStr = 128;
  errorMsgsFile = "ErrorList.Txt";         (* file containing error messages *)


TYPE
  (* data structure to hold list of error messages *)
  FileName = ARRAY Redir.maxPathLen OF CHAR;
  ErrMsg = POINTER TO ErrMsgDesc;
  ErrStr = ARRAY sizeErrStr OF CHAR;
  ErrMsgDesc =
    RECORD
      next : ErrMsg;
      num : INTEGER;
      msg : ErrStr
    END;
  PosArray = POINTER TO ARRAY OF LONGINT;

VAR
  warnings* : BOOLEAN;                   (* TRUE: emit warnings *)
  verbose* : BOOLEAN;                    (* TRUE: print extra information *)
  underscore* : BOOLEAN;                 (* TRUE: underscore _ is considered as character (in identifiers) *)
  
  sourceName- : FileName;                (* the name of the work file used as scanner input *)
  inFile : F.File;                      (* source file *)
  inRider: F.Rider;                     (* source rider *)
  len : LONGINT;                         (* length of source file in bytes *)
  sizeBuffer : INTEGER;                  (* size of the currently used buffer, set via 'Init' *)
  buf : POINTER TO ARRAY maxBufferSize OF CHAR; (* buffer area *)
  pos : INTEGER;                         (* current scanning position in 'buf' *)
  bufOffset : LONGINT;                   (* file position of the first character in 'buf' *)
  bufRefreshPos : INTEGER;               (* pos>=bufRefreshPos will read the next buffer block *)

  kwStr : ARRAY sizeKWTable, 10 OF CHAR; (* hash table for keywords *)
  kwSym : ARRAY sizeKWTable OF SHORTINT; (* token mark for the keywords (values for 'sym') *)

  noerr* : BOOLEAN;                      (* TRUE iff no error has occured til now *)
  lastErr* : LONGINT;                     (* position of last error *)
  errHeader : BOOLEAN;                   (* TRUE iff the header preceding the error messages has been written *)
  errs : ErrMsg;                         (* list of plain text error messages *)
  
  sym* : SHORTINT;                       (* current token *)
  lastSym* : LONGINT;                    (* file position the current token *)
  ref* : ARRAY M.maxSizeString OF CHAR;  (* space to return string and ident values *)
  numType* : SHORTINT;                   (* numInt, numReal or numLReal *)
  intVal* : LONGINT;
  realVal* : LONGREAL;

  (* this array holds the file positions of all end of line characters; used
     by `Line' and `Col' to translate file position into line/column pair *)
  eolArray: PosArray;
  eolSize: LONGINT;
  

PROCEDURE VerboseMsg* (msg : ARRAY OF CHAR);
(* Writes 'msg' plus newline to stdout iff verbose is set. *)
  BEGIN
    IF verbose THEN
      Out.String (msg);
      Out.Ln
    END
  END VerboseMsg;
  


PROCEDURE ReadErrorList*;
(* Reads error messages.  Makes use of Redir.FindPath to find
  the location of the file 'errorMsgsFile'. *)
  VAR
    f : F.File;
    r : F.Rider;
    ch : CHAR;
    num : INTEGER;
    str : ErrStr;
    i : INTEGER;
    new : ErrMsg;
    found, open : BOOLEAN;
    fileName : ARRAY Redir.maxPathLen OF CHAR;
  BEGIN
    found := Redir.FindPath (M.redir, errorMsgsFile, fileName);
    IF found THEN
      f := F.Old (fileName);
      open := f # NIL;
      IF open THEN
        F.Set (r, f, 0)
      END;
    END;
    IF found & open THEN
      F.Read (r, ch);
      WHILE ~r. eof DO
        IF ("0"<=ch) & (ch<="9") THEN
          num := 0;
          WHILE ~r. eof & ("0"<=ch) & (ch<="9") DO
            num := num*10+ORD(ch)-ORD("0"); F.Read (r, ch)
          END;
          IF (ch=":") THEN
            F.Read (r, ch);
            WHILE ~r. eof & (ch=" ") DO
              F.Read (r, ch)
            END;
            i := 0;
            WHILE ~r. eof & (ch>=" ") DO
              IF (i<sizeErrStr-1) THEN
                str[i] := ch; INC (i)
              END;
              F.Read (r, ch)
            END;
            str[i] := 0X;
            NEW (new);
            new. next := errs;
            new. num := num;
            new. msg := str;
            errs := new
          END
        ELSE
          F.Read (r, ch)
        END
      END;
      F.Close(f)
    END
  END ReadErrorList;

PROCEDURE GetErrMsg* (num : INTEGER; VAR str : ARRAY OF CHAR);
(* Returns plain text associated with error #num.  If no such 
  entry can be found (no error table loaded or the number does
  not occur in the file) 'str' is filled with a standard 'not
  found' message. *)
  VAR
    m : ErrMsg;
  BEGIN
    m := errs;
    WHILE (m # NIL) DO
      IF (m. num = num) THEN
        COPY (m. msg, str);
        RETURN
      END;
      m := m. next
    END;
    COPY ("(no error message available)", str)
  END GetErrMsg;



PROCEDURE Replace* (VAR string, insert : ARRAY OF CHAR);
(* post: First occurence of the character '%' in 'string' replaced with 'insert'. *)
  VAR
    i, j, l, ls : INTEGER;
  BEGIN
    i := 0;
    WHILE (string[i] # "%") & (string[i] # nul) DO
      INC (i)
    END;
    ls := i;
    WHILE (string[ls] # nul) DO
      INC (ls)
    END;
    IF (string[i]="%") THEN
      l := 0;
      WHILE (insert[l] # nul) DO
        INC (l)
      END;
      j := ls;
      WHILE (j > i) DO
        string[j+l-1] := string[j];
        DEC (j)
      END;
      j := 0;
      WHILE (j < l) DO
        string[i+j] := insert[j];
        INC (j)
      END
    END
  END Replace;

PROCEDURE Mark (pos : LONGINT; num : INTEGER; msg : ARRAY OF CHAR);
(* pre: 'pos' is a valid file position in the current source file or negative
     to denote the position of the current token.
   post: The error/warning text 'msg' is written to the error file, referring to
     file position 'pos'. *)
  CONST
    suppress = 12;
  BEGIN
    IF (pos < 0) THEN
      pos := lastSym
    END;
    IF (ABS (lastErr-pos) >= suppress) THEN
      IF ~errHeader THEN
        Out.String ("In file ");
        Out.String (sourceName);
        Out.String (": ");
        Out.Ln;
        errHeader := TRUE
      END;
      Out.Int (pos, 0);
      Out.String (":");
      Out.Int (num, 3);
      Out.Char (" ");
      Out.String (msg);
      Out.Ln
    END;
    lastErr := pos;
  END Mark;

PROCEDURE MarkIns (pos : LONGINT; num : INTEGER; msg, ins : ARRAY OF CHAR);
(* Like Mark, but Replace(msg,ins) is called first. *)
  VAR
    str : ARRAY sizeErrStr OF CHAR;
  BEGIN
    COPY (msg, str);
    Replace (str, ins);
    Mark (pos, num, str);
  END MarkIns;

PROCEDURE Err* (pos : LONGINT; num : INTEGER);
(* pre: 'pos' is valid file position in the current source file or negative
     to denote the position of the current token, 'num' a valid
     error message defined in ErrorList.Txt.
   post: Error message written at position 'pos', 'noerr' set to FALSE. *)
  VAR
    str : ARRAY sizeErrStr OF CHAR;
  BEGIN
    noerr := FALSE;
    GetErrMsg (num, str);
    Mark (pos, num, str)
  END Err;

PROCEDURE ErrIns*(pos : LONGINT; num : INTEGER; ins : ARRAY OF CHAR);
(* Like Err, but Replacs(msg,ins) is called first. *)
  VAR
    str : ARRAY sizeErrStr OF CHAR;
  BEGIN
    noerr := FALSE;
    GetErrMsg (num, str);
    MarkIns (pos, num, str, ins)
  END ErrIns;

PROCEDURE ErrIns2*(pos : LONGINT; num : INTEGER; ins1, ins2 : ARRAY OF CHAR);
(* Like Err, but Replace(msg,ins1) and Replace(msg,ins2) is called first. *)
  VAR
    str : ARRAY sizeErrStr OF CHAR;
  BEGIN
    noerr := FALSE;
    GetErrMsg (num, str);
    Replace (str, ins1);
    MarkIns (pos, num, str, ins2)
  END ErrIns2;

PROCEDURE ErrRel (relPos : INTEGER; num : INTEGER);
(* Prints error relative to current position of the read buffer. *)
  BEGIN
    Err (bufOffset+relPos, num)
  END ErrRel;

PROCEDURE Warn* (pos : LONGINT; num : INTEGER);
(* pre: 'pos' is valid file position in the current source file or negative
     to denote the position of the current token, 'num' a valid
     error message defined in ErrorList.Txt.
   post: Error message written at position 'pos'. *)
  VAR
    str : ARRAY sizeErrStr OF CHAR;
  BEGIN
    IF warnings & noerr THEN
      lastErr := -9999;                  (* force warning to be displayed *)
      GetErrMsg (num, str);
      Mark (pos, num, str);
      lastErr := -9999                   (* force following error to be displayed *)
    END
  END Warn;

PROCEDURE WarnIns* (pos : LONGINT; num : INTEGER; ins : ARRAY OF CHAR);
(* pre: 'pos' is valid file position in the current source file or negative
     to denote the position of the current token, 'num' a valid
     error message defined in ErrorList.Txt.
   post: Error message written at position 'pos'.  *)
  VAR
    str : ARRAY sizeErrStr OF CHAR;
  BEGIN
    IF warnings & noerr THEN
      lastErr := -9999;                  (* force warning to be displayed *)
      GetErrMsg (num, str);
      MarkIns (pos, num, str, ins);
      lastErr := -9999                   (* force following error to be displayed *)
    END
  END WarnIns;



PROCEDURE ReadNextBlock;
(* Reads the next part of the file from disk.
  pre: pos>=sizeBuffer-maxBufferRest.
  post: The bytes in buf[0..pos-1] are discarded, the remaining ones buf[pos..pos+left-1] are moved
    to buf[0..left-1] and a block of new bytes is read into buf[left..]. If all bytes are read, a
    final 'eof' character is added to 'buf' to mark the end of the file. *)
  VAR
    left, i : INTEGER;
    next : LONGINT;
    clip : ARRAY maxBufferRest OF CHAR;
  BEGIN
    (* save remaining (not read) bytes from the current buffer *)
    left := sizeBuffer-pos;              (* number of bytes left *)
    i := 0;
    WHILE (i # left) DO
      clip[i] := buf[pos+i];
      INC (i)
    END;
    INC (bufOffset, LONG (pos));         (* adapt buffer offset to new position *)
    pos := 0;
    next := len-bufOffset-left;          (* number of bytes left in the file *)
    IF (next < 0) THEN  (* we have read everything and already placed the eof *)
      (* copy clip[0..left-1] to buf[0..left-1] *)
      FOR i := 0 TO left-1 DO
        buf[i] := clip[i]
      END
    ELSE  (* there is something to read (even if it is only a 0 byte block) *)
      IF (next > sizeBuffer-left) THEN   (* too much bytes for the buffer? *)
        next := sizeBuffer-left
      END;
      F.ReadBytes (inRider, buf^, next);  (* read next block *)
      IF (next < sizeBuffer-left) THEN   (* place eof at the end of the buffer (if possible) *)
        buf[next] := eof;
        INC (next)
      END;
      IF (left # 0) THEN
        i := SHORT (next)+left;                  (* move buf[0..next-1] to buf[left..left+next-1] *)
        WHILE (i # left) DO
          DEC (i);
          buf[i] := buf[i-left]
        END;
        WHILE (i # 0) DO                 (* move clip[0..left-1] to buf[0..left-1] *)
          DEC (i);
          buf[i] := clip[i]
        END
      END
    END
  END ReadNextBlock;

PROCEDURE EndOfLine;
(* Adds end of line position to `eolArray'.  This information is used by
   `FindLine' to reconstruct the line number of a given file position. 
   pre: buf[pos] = eol *)
  VAR
    i: LONGINT;
    new: PosArray;
  BEGIN
    IF (eolSize = LEN (eolArray^)) THEN
      NEW (new, (eolSize+4)*2-4);
      FOR i := 0 TO eolSize-1 DO
        new[i] := eolArray[i]
      END;
      eolArray := new
    END;
    eolArray[eolSize] := bufOffset+pos;
    INC (eolSize)
  END EndOfLine;

PROCEDURE FindLine (pos: LONGINT): LONGINT;
(* Calculates the line number of source code position `pos'.  Result is -1 for
   `pos < 0'.  *)
  VAR
    line, l, r, m: LONGINT;
  BEGIN
    IF (pos < 0) THEN
      RETURN -1
    ELSE
      l := 0; r := eolSize;
      WHILE (l # r) DO
        m := (l + r) DIV 2;
        IF (pos > eolArray[m]) THEN
          l := m+1
        ELSE
          r := m
        END
      END;
      RETURN l
    END
  END FindLine;

PROCEDURE Line* (pos: LONGINT): LONGINT;
(* Returns the number of the source line that contains `pos'.  The first line 
   is 1.  The position of the `eol' character that terminates a line is
   considered as part of this line.  Result is 0 for `pos < 0'.  *)
  BEGIN
    RETURN FindLine (pos)+1
  END Line;

PROCEDURE Col* (pos: LONGINT): LONGINT;
(* Returns column number of `pos' in `Line(pos)'.  The left-most column is 1.
   Result is 0 for `pos < 0' .  *)
  VAR
    line: LONGINT;
  BEGIN
    line := FindLine (pos);
    IF (line <= 0) THEN
      IF (pos < 0) THEN
        RETURN 0
      ELSE
        RETURN pos+1
      END
    ELSE
      RETURN pos-1-eolArray[line-1]+1
    END
  END Col;


PROCEDURE SkipBlanks;
(* post: pos has moved to the first printable character buf[pos], or
     buf[pos]=eof, signalling that the end of the file is reached. *)
  BEGIN
    LOOP
      IF (pos >= bufRefreshPos) THEN
        ReadNextBlock
      END;
      IF (buf[pos] = eof) OR (buf[pos] >= 21X) THEN (* printable characters *)
        RETURN
      ELSE                               (* skip all other characters *)
        IF (buf[pos] = eol) THEN EndOfLine END;
        INC (pos)
      END
    END
  END SkipBlanks;

PROCEDURE Comment;
(* Skips comments including (of course) nested ones.
   pre: scan[pos]="*", scan[pos-1]="("
   post: pos is the index of the first character behind the comment, or buf[pos]=eof
     if the end of the file has been reached.
   side: A not terminated string will cause an error message. *)
  VAR
    start : LONGINT;
  BEGIN
    start := bufOffset+pos-1;
    INC (pos);
    LOOP                                 (* loop until end of comment/file reached *)
      IF (pos >= sizeBuffer-1) THEN
        ReadNextBlock
      END;
      CASE buf[pos] OF
      | eof:                             (* end of file: write an error *)
        Err (start, 1);
        EXIT
      | eol:                             (* end of line: store position *)
        EndOfLine;
        INC (pos)
      | "*":
        INC (pos);
        IF (buf[pos] = ")") THEN         (* end of comment *)
          INC (pos);
          EXIT
        END
      | "(":
        INC (pos);
        IF (buf[pos] = "*") THEN         (* nested comments? *)
          Comment
        END
      ELSE                               (* skip characters in comment *)
        INC (pos)
      END
    END
  END Comment;

PROCEDURE GetString (end : CHAR);
(* Read string with " or ' as delimiter.
   pre: buf[pos]=end, pos<sizeBuffer-maxSizeString
   post: buf[pos-1]=end (i.e. 'pos' is placed behind the string's ending delimiter
     or buf[pos]=eof if the end of the file was reached. 'sym' is set to string,
     a copy of the string is placed in 'ref'.
   side: Control characters in the string or a not terminated string will be
     marked with error messages. *)
  VAR
    len : INTEGER;
    start : LONGINT;
  BEGIN
    sym := string;
    len := 0;
    start := bufOffset+pos;
    LOOP                                 (* loop until end or eof reached *)
      INC (pos);
      IF (buf[pos] < " ") THEN           (* illegal control character in string *)
        IF (buf[pos] = eof) THEN
          Err (start, 3);
          EXIT
        ELSE
          IF (buf[pos] = eol) THEN EndOfLine END;
          ErrRel (pos, 2)
        END
      ELSIF (buf[pos] = end) THEN        (* end of string *)
        INC (pos);
        EXIT
      END;
      IF (len < M.maxSizeString) THEN
        ref[len] := buf[pos]
      ELSIF (pos >= bufRefreshPos) THEN
        ReadNextBlock
      END;
      INC (len)
    END;
    (* place the terminating 'nul' in 'ref' *)
    IF (len < M.maxSizeString) THEN
      ref[len] := nul
    ELSE
      Err (start, 12);
      ref[M.maxSizeString-1] := nul
    END;
    intVal := ORD (ref [0])
  END GetString;

PROCEDURE Ident;
(* Reads identifiers and identifies the keywords.
   pre: buf[pos] is a character, pos<=sizeBuffer-maxSizeIdent
   post: buf[pos] is not a character or a cipher, 'sym' is set to ident or
     to the corresponding keyword, a copy of the identifier is stored in 'ref'. *)
  VAR
    sum, len : INTEGER;
    start : LONGINT;
  BEGIN
    sym := ident;
    len := 0;
    sum := 0;
    start := bufOffset+pos;
    REPEAT                               (* loop to the first non char/cipher *)
      IF (len < M.maxSizeIdent) THEN
        ref[len] := buf[pos]
      ELSIF (pos >= bufRefreshPos) THEN
        ReadNextBlock
      END;
      INC (len);
      INC (sum, ORD (buf[pos]));
      INC (pos)
    UNTIL ~ (("A" <= CAP (buf[pos])) & (CAP (buf[pos]) <= "Z") OR
             ("0" <= buf[pos]) & (buf[pos] <= "9") OR
             underscore & (buf[pos] = "_"));
    IF (len < M.maxSizeIdent) THEN
      ref[len] := nul;
      (* Test is 'ref' denotes a keyword. If so, then set 'sym' accordingly. *)
      sum := sum MOD sizeKWTable;
      IF (kwStr[sum, 0] # 0X) THEN
        IF (kwStr[sum] = ref) THEN
          sym := kwSym[sum]
        ELSE
          INC (sum, offKWTable);
          IF (sum >= 0) & (kwStr[sum] = ref) THEN
            sym := kwSym[sum]
          END
        END
      END
    ELSE
      Err (start, 13);
      ref[M.maxSizeIdent-1] := nul
    END
  END Ident;

PROCEDURE Number;
(* Scan numbers (this includes character, dezimal, real and long real 
   constants).
   pre: buf[pos] is a cypher, pos < bufRefreshPos, maxSizeNumber<maxSizeString.
   post: The numbers internal representation is computed. If it is a real, its
     value is stored in realVal (numType=numReal or numLReal, sym=number),
     otherwise its integer value is placed in intVal (numType=intVal, 
     sym=number). If it was a character constant (suffix X) it's converted into
     a string (in 'ref', sym=string). *)
  VAR
    format : SHORTINT;
    i, d, len : INTEGER;
    start : LONGINT;
  CONST
    eps = 2.0282409603651670D+31;  (* equals 2^104 *)

  PROCEDURE GetCypher(c: CHAR; pos: INTEGER; hex: BOOLEAN): INTEGER;
    VAR
      d: INTEGER;
    BEGIN
      d:=ORD(c);
      IF (ORD ("0") <= d) & (d <= ORD ("9")) THEN
        DEC (d, ORD ("0"))
      ELSIF hex & (ORD ("A") <= d) & (d <= ORD ("F")) THEN
        DEC (d, ORD ("A")-10)
      ELSE  (* illegal cypher *)
        ErrRel (pos, 5);
        d := 0
      END;
      RETURN d
    END GetCypher;
  
  PROCEDURE ConvertHex(spos, epos: LONGINT): LONGINT;
    VAR
      result : LONGINT;
    BEGIN
      result := 0;
      (* skip leading zeros *)
      WHILE (buf[spos] = "0") DO 
        INC (spos)
      END;
      IF (epos-spos > M.maxLIntHexDigits-1) THEN  (* value has more than LONGINT hex digits significant cyphers *)
        Err (spos, 6)
      ELSIF (spos <= epos) THEN  (* if any non-zero cyphers follow *)
        result := GetCypher (buf[spos], SHORT (spos), TRUE);
        INC (spos);
        IF (epos-spos = M.maxLIntHexDigits-1) & (result >= 8) THEN
          (* value is beyond MAX(LONGINT) *)
          DEC (result, 10H)
        END;
        WHILE (spos <= epos) DO
          result := result*10H + GetCypher (buf[spos], SHORT (spos), TRUE);
          INC (spos)
        END
      END;
      RETURN result
    END ConvertHex;

  BEGIN
    sym := number;
    start := bufOffset+pos;
    (* scan characters (and copy them to ref) until the first non (hex-) cypher *)
    REPEAT
      INC (pos);
      IF (pos = sizeBuffer-1) THEN
        ReadNextBlock
      END
    UNTIL ~ (("0" <= buf[pos]) & (buf[pos] <= "9") OR ("A" <= buf[pos]) & (buf[pos] <= "F"));
    IF (buf[pos] = ".") & (buf[pos+1] # ".") THEN  (* real (but not a '..' token) *)
      INC (pos);
      (* read decimal fraction *)
      WHILE ("0" <= buf[pos]) & (buf[pos] <= "9") DO
        INC (pos);
        IF (pos = sizeBuffer-2) THEN
          ReadNextBlock
        END
      END;
      (* determine constant type (long real, or just real?) *)
      IF (buf[pos] = "D") THEN
        numType := numLReal; buf[pos] := "E"
      ELSE
        numType := numReal
      END;
      IF (buf[pos] = "E") THEN  (* read scale factor *)
        INC (pos);
        IF (buf[pos] = "-") OR (buf[pos] = "+") THEN
          INC (pos)
        END;
        IF ("0" <= buf[pos]) & (buf[pos] <= "9") THEN
          REPEAT
            INC (pos);
            IF (pos = sizeBuffer) THEN
              ReadNextBlock
            END
          UNTIL (buf[pos] < "0") OR ("9" < buf[pos])
        ELSE
          ErrRel (pos, 9)
        END
      END;
      len := SHORT (bufOffset+pos-start); (* number of cyphers in constant *)
      IF (len < maxSizeNumber) THEN      (* precondition ensures that no ReadNextBlock has been called *)
        (* copy constant into ref *)
        d := SHORT (start-bufOffset);
        FOR i := 0 TO len-1 DO
          ref[i] := buf[d+i]
        END;
        ref[len] := nul;
        RealStr.Take (ref, realVal, format); (* convert constant *)
        IF (format = Conv.outOfRange) OR
           (numType = numReal) & 
             ((realVal < LONG(M.minReal)-eps) OR 
              (realVal >= LONG(M.maxReal)+eps)) THEN
          Err (start, 6)
        END
      ELSE
        realVal := 1.0;
        Err (start, 4)                   (* number too long *)
      END
    ELSE  (* integer *)
      intVal := 0;
      (* determine base of representation *)
      IF (buf[pos] = "H") OR (buf[pos] = "X") THEN
        intVal := ConvertHex (SHORT (start-bufOffset), pos-1);
      ELSE
        len := SHORT (bufOffset+pos-start);
        IF (len < maxSizeNumber) THEN
          i := SHORT (start-bufOffset);
          WHILE (i < pos) DO
            d := GetCypher (buf[i], i, FALSE);
            IF ((M.maxLInt-d) DIV 10 >= intVal) THEN
              intVal := intVal*10+d
            ELSE
              Err (start, 6)  (* overflow *)
            END;
            INC (i)
          END
        ELSE
          intVal := 1;
          Err (start, 4)                   (* number too long *)
        END
      END;
      (* set constant type according to suffix *)
      IF (buf[pos] = "X") THEN
        sym := string;
        INC (pos);
        IF (intVal > M.maxChar) OR (intVal < M.minChar) THEN
          Err (start, 7)  (* not a legal character constant *)
        ELSE
          ref[0] := CHR (intVal);
          ref[1] := nul
        END
      ELSE
        IF (buf[pos] = "H") THEN
          INC (pos)
        END;
        numType := numInt
      END
    END
  END Number;

PROCEDURE GetSym*;
(* Reads nexts token.
   pre: Init has been executed without any errors.
   post: 'sym' denotes the class of the token, its attributes are stored in
     'ref', 'numType', 'intVal' and 'realVal' (depending on the class). *)
  VAR
    c,l,p: LONGINT;
  BEGIN
    SkipBlanks;
    IF (pos >= bufRefreshPos) THEN
      ReadNextBlock
    END;
    lastSym := bufOffset+pos;
    CASE buf[pos] OF
      "a".."z", "A".."Z": Ident |
      "_": IF underscore THEN Ident ELSE Err (lastSym, 8); INC (pos); GetSym END |
      "0".."9": Number |
      eof: sym := endOfFile |
      22X: GetString (22X) |
      "'": GetString ("'") |
      "~": sym := not; INC (pos) |
      "{": sym := lBrace; INC (pos) |
      ".": INC (pos);
           IF (buf[pos]=".") THEN sym := upto; INC (pos)
           ELSE sym := period END |
      "^": sym := arrow; INC (pos) |
      "[": sym := lBrak; INC (pos) |
      ":": INC (pos);
           IF (buf[pos]="=") THEN sym := becomes; INC (pos)
           ELSE sym := colon END |
      "(": INC (pos);
           IF (buf[pos]="*") THEN Comment; GetSym;
           ELSE sym := lParen END |
      "*": sym := times; INC (pos);
           IF (buf[pos]=")") THEN
             Err (lastSym, 14);
             INC (pos)
           END |
      "/": sym := slash; INC (pos) |
      "&": sym := and; INC (pos) |
      "+": sym := plus; INC (pos) |
      "-": sym := minus; INC (pos) |
      "=": sym := eql; INC (pos) |
      "#": sym := neq; INC (pos) |
      "<": INC (pos);
           IF (buf[pos]="=") THEN sym := leq; INC (pos)
           ELSE sym := lss END |
      ">": INC (pos);
           IF (buf[pos]="=") THEN sym := geq; INC (pos)
           ELSE sym := gtr END |
      "}": sym := rBrace; INC (pos) |
      ")": sym := rParen; INC (pos) |
      "]": sym := rBrak; INC (pos) |
      "|": sym := bar; INC (pos) |
      ";": sym := semicolon; INC (pos) |
      ",": sym := comma; INC (pos)
    ELSE
      Err (lastSym, 8); INC (pos); GetSym
    END
  END GetSym;

PROCEDURE LastPos*;
  BEGIN
    Out.String ("scanning pos on termination: ");
    Out.Int (bufOffset+pos, 5);
    Out.Ln
  END LastPos;



PROCEDURE Init* (fileName : ARRAY OF CHAR; bufferSize : INTEGER; VAR err : BOOLEAN);
  PROCEDURE CheckOut;
    VAR
      com : ARRAY 1024 OF CHAR;
    BEGIN
      IF Dos.Exists (fileName) THEN
        Redir.RCS2File (fileName, fileName); (* extract name of the work file *)
        (* assemble command to check out the latest revision *)
        com := "co -q ";      
        Str.Append (fileName, com);
        VerboseMsg (com);
        IF (Rts.System (com) # 0) THEN
          Out.String (" --- failed to extract latest revision of ");
          Out.String (fileName);
          Out.Ln;
          RETURN
        END
      ELSE
        Out.String (" --- can't open file ");
        Out.String (fileName);
        Out.Ln;
        RETURN
      END
    END CheckOut;
  
  BEGIN
    err := TRUE;
    IF Strings2.Match ("*,v", fileName) THEN  
      (* RCS file, check out latest revision *)
      CheckOut
    END;
    COPY (fileName, sourceName);
    inFile := F.Old (fileName);
    IF inFile = NIL THEN
      Out.String (" --- file ");
      Out.String (fileName);
      Out.String (" not found");
      Out.Ln
    ELSE
      F.Set (inRider, inFile, 0);
      len := F.Length (inFile);
      sizeBuffer := bufferSize;
      bufRefreshPos := sizeBuffer-maxBufferRest;
      pos := bufferSize;
      bufOffset := -bufferSize;
      err := FALSE;
      lastErr := -9999;
      errHeader := FALSE;
      lastSym := -1;
      eolSize := 0;
      ReadNextBlock
    END;
    noerr := ~err
  END Init;

PROCEDURE Close*;
  BEGIN
    F.Close (inFile)
  END Close;



PROCEDURE InitKeywords;
  VAR
    i : INTEGER;

  PROCEDURE KW (ident : ARRAY OF CHAR; sym : SHORTINT);
    VAR
      i, sum : INTEGER;
    BEGIN
      sum := 0;
      i := 0;
      WHILE (ident[i] # 0X) DO
        INC (sum, ORD (ident[i]));
        INC (i)
      END;
      i := sum MOD sizeKWTable;
      IF (kwSym [i] >= 0) THEN
        INC (i, offKWTable)
      END;
      COPY (ident, kwStr[i]);
      kwSym[i] := sym
    END KW;

  BEGIN
    FOR i := 0 TO sizeKWTable-1 DO
      kwSym[i] := -1;
      kwStr[i,0] := 0X
    END;
    KW ("ARRAY", array); KW ("BEGIN", begin); KW ("BY", by); KW ("CASE", case);
    KW ("CONST", const); KW ("DIV", div); KW ("DO", do); KW ("ELSE", else);
    KW ("ELSIF", elsif); KW ("END", end); KW ("EXIT", exit); KW ("FOR", for);
    KW ("IF", if); KW ("IMPORT", import); KW ("IN", in); KW ("IS", is);
    KW ("LOOP", loop); KW ("MOD", mod); KW ("MODULE", module); KW ("NIL", nil);
    KW ("OF", of); KW ("OR", or); KW ("POINTER", pointer); 
    KW ("PROCEDURE", procedure); KW ("RECORD", record); KW ("REPEAT", repeat);
    KW ("RETURN", return); KW ("THEN", then); KW ("TO", to); KW ("TYPE", type);
    KW ("UNTIL", until); KW ("VAR", var); KW ("WHILE", while); 
    KW ("WITH", with)
  END InitKeywords;


BEGIN
  NEW (buf);
  Rts.Assert (buf # NIL, "Fatal: Cannot allocate memory for file buffer.");
  warnings := FALSE;
  verbose := FALSE;
  underscore := FALSE;
  NEW (eolArray, 1024-4);
  Rts.Assert (eolArray # NIL, "Fatal: Cannot allocate memory for line buffer.");
  InitKeywords
END OScan.

