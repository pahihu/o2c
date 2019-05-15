MODULE In;
(* see also [Oakwood Guidelines, revision 1A]
Module In provides a set of basic routines for formatted input of characters, 
character sequences, numbers, and names.  It assumes a standard input stream 
with a current position that can be reset to the beginning of the stream. 

State
 - Current position.  The character position in the input stream from where the
   next symbol is read.  Open (re)sets it to the beginning of the input stream.
   After reading a symbol the current position is set to the position 
   immediately after this symbol.  Before the first call to Open the current
   position is undefined.
 - Done.  Indicates the success of an input operation.  If Done is TRUE after 
   an input operation, the operation was successful and its result is valid. 
   An unsuccessful input operation sets Done to FALSE; it remains FALSE until
   the next call to Open.  In particular, Done is set to FALSE if an attempt
   is made to read beyond the end of the input stream.   

All operations (except Open) require Done=TRUE and guarantee (Done=TRUE and the
result is valid) or (Done=FALSE).  All operations except Char skip leading
blanks, tabs or end-of-line characters.

Example:
  VAR i: INTEGER; ch: CHAR; r: REAL; s, n: ARRAY 32 OF CHAR;
  ...
  In.Open;
  In.Int(i); In.Char(ch); In.Real(r); In.String(s); In.Name(n)
  
  Input stream:
        123*1.5 "abc" Mod.Proc
  
  Results:
    i=123, ch="*", r=1.5E0, s="abc", n="Mod.Proc"
*)


IMPORT
  CType, Unix, CharInfo, ConvTypes, RealStr;
  

VAR
  Done-: BOOLEAN;                        (* all operations succeeded *)
  noLookAhead: BOOLEAN;                  (* no lookahead character available *)
  lookAhead: CHAR;                       (* lookahead character *)
  eof: BOOLEAN;                          (* last operation started behind end of file *)

PROCEDURE GetChar;  
(* Attempts to read a character from stdin.  On error or end-of-file the flag
   eof is set to TRUE (and lookAhead to 0X), otherwise the character is stored
   in lookAhead and eof stays FALSE. *)
  VAR
    res : CType.int;
  BEGIN
    IF Done THEN
      res := Unix.fgetc (Unix.stdin);
      IF (res = Unix.EOF) THEN
        lookAhead := 0X;
        eof := TRUE
      ELSE
        lookAhead := CHR (res)
      END
    END
  END GetChar;
  
PROCEDURE SkipWhitespace;
(* Skip whitespace characters blank, tab and newline until a non-whitespace
   character is found.  If the file ends without containing such an character,
   Done is set to FALSE (and eof=TRUE, lookAhead=0X). *)
  BEGIN    
    IF noLookAhead THEN                  (* get first character *)
      GetChar;
      noLookAhead := FALSE
    END;
    WHILE (lookAhead = " ") OR (lookAhead = CharInfo.ht) OR
          (lookAhead = CharInfo.eol) DO
      GetChar
    END;
    Done := Done & ~eof
  END SkipWhitespace;


PROCEDURE Open*;
(* (Re)sets the current position to the beginning of the input stream.  Done
   indicates if the operation was successful. *)
  BEGIN
    Done := (Unix.fseek (Unix.stdin, 0, Unix.SEEK_SET) = 0) OR TRUE;
    (* fseek seems to return FALSE if stdin is the terminal or a pipe, and TRUE
       if stdin is a redirected file.  We try a fseek, but set Done=TRUE
       regardless of the outcome. *)
    eof := FALSE;
    noLookAhead := TRUE
  END Open;

PROCEDURE Char* (VAR ch: CHAR);
(* Returns the character ch at the current position. *)
  BEGIN
    IF ~Done OR eof THEN
      Done := FALSE
    ELSE
      IF noLookAhead THEN                   (* get first character *)
        GetChar
      END;
      noLookAhead := TRUE;
      ch := lookAhead
    END
  END Char;

PROCEDURE LongInt* (VAR n: LONGINT);
(* Returns the long integer constant n at the current position according to the
   format:
     IntConst = digit {digit} | digit {hexDigit} "H". 
   Note: This implementation will only accepts values less or equal to 
   MAX(LONGINT), e.g. 080000000H will be rejected. *)
  CONST
    stringSize = 12;  (* maximum of 10 digits + 2 extra *)
  VAR
    s: ARRAY stringSize OF CHAR;
    hex: BOOLEAN;
    i, j, digit: INTEGER;
  BEGIN
    SkipWhitespace;
    IF ("0" <= lookAhead) & (lookAhead <= "9") THEN
      (* read digits into string s, but skip leading zeros *)
      hex := FALSE;
      i := 0;
      REPEAT
        s[i] := lookAhead;
        hex := hex OR (lookAhead > "9");
        IF ((i > 0) OR (lookAhead # "0")) & (i < stringSize-1) THEN
          INC (i)
        END;
        GetChar
      UNTIL (("0" > lookAhead) OR (lookAhead > "9")) &
            (("A" > lookAhead) OR (lookAhead > "F"));
      (* there were more than stringSize-2 digits if i=stringSize-1 *)
      
      (* trailing "H" turns everything into a hex number *)
      IF hex & (lookAhead # "H") THEN
        Done := FALSE;                    (* hex digits, but no marker *)
        RETURN
      END;
      hex := (lookAhead = "H");
      IF hex THEN
        GetChar
      END;
      
      (* convert string into number *)
      IF (i = 0) THEN
        (* only zeros found *)
        n := 0
      ELSIF (i = stringSize-1) OR 
            hex & ((i > 8) OR (i = 8) & (s[i] > "7")) THEN
        (* too many digits or hex number is >MAX(LONGINT) *)
        Done := FALSE
      ELSIF hex THEN  
        (* hex number in range 1 <= n <= MAX(LONGINT) *)
        n := 0;
        j := 0;
        WHILE (j < i) DO
          IF (s[j] > "9") THEN
            n := n*16 + (ORD (s[j]) - (ORD ("A") - 10))
          ELSE
            n := n*16 + (ORD (s[j]) - ORD ("0"))
          END;
          INC (j)
        END
      ELSE  
        (* decimal number, take care of overflows *)
        n := 0;
        j := 0;
        WHILE (j < i) DO
          digit := ORD (s[j]) - ORD ("0");
          IF ((MAX(LONGINT)-digit) DIV 10 >= n) THEN
            n := n*10 + digit
          ELSE  (* overflow, number larger than MAX(LONGINT) *)
            Done := FALSE
          END;
          INC (j)
        END
      END
    ELSE  (* no digit found: syntax error *)
      Done := FALSE
    END
  END LongInt;

PROCEDURE Int* (VAR n: INTEGER);
(* Returns the integer constant n at the current position according to the
   format:
     IntConst = digit {digit} | digit {hexDigit} "H". *)
  VAR
    long: LONGINT;
  BEGIN
    LongInt (long);
    IF Done & (long <= MAX(INTEGER)) THEN
      n := SHORT (long)
    ELSE
      Done := FALSE
    END
  END Int;

PROCEDURE ReadReal (VAR s: ARRAY OF CHAR; allowDouble: BOOLEAN);
(* Reads real string in real or long real syntax. *)
  VAR
    i, m : INTEGER;
    
  PROCEDURE AddChar (ch: CHAR);
  (* Add ch to s (unless s is full) and get next character. *)
    BEGIN
      IF (i < m) THEN
        s[i] := ch;
        INC (i)
      ELSE  (* string overflow *)
        Done := FALSE
      END;
      GetChar
    END AddChar;
    
  BEGIN
    SkipWhitespace;
    IF ("0" <= lookAhead) & (lookAhead <= "9") THEN
      m := SHORT (LEN(s))-1;  (* maximum length of string *)
      
      (* read digits into string s *)
      i := 0;
      REPEAT
        AddChar (lookAhead)
      UNTIL (lookAhead < "0") OR (lookAhead > "9");
      
      IF (lookAhead = ".") THEN
        (* decimal point *)
        AddChar (".");
        
        (* fraction part *)
        WHILE ("0" <= lookAhead) & (lookAhead <= "9") DO
          AddChar (lookAhead)
        END;
        
        IF (lookAhead = "E") OR allowDouble & (lookAhead = "D") THEN
          (* exponent, put always "E" into s *)
          AddChar ("E");
          IF (lookAhead = "-") OR (lookAhead = "+") THEN
            AddChar (lookAhead);
            IF ("0" <= lookAhead) & (lookAhead <= "9") THEN
              (* non-empty sequence of digits *)
              REPEAT
                AddChar (lookAhead)
              UNTIL (lookAhead < "0") OR (lookAhead > "9")              
            ELSE 
              Done := FALSE
            END
          ELSE  (* missing sign in exponent *)
            Done := FALSE
          END
        END
      END;
      IF (i < m) THEN
        s[i] := 0X
      END
    ELSE  (* no digit found: syntax error *)
      Done := FALSE
    END
  END ReadReal;
  
PROCEDURE LongReal* (VAR n: LONGREAL);
(* Returns the long real constant n at the current position according to the 
   format:
     LongRealConst = digit {digit} ["." {digit} 
                                    [("D" | "E") ("+" | "-") digit {digit}]]. 
*)     
  VAR
    s: ARRAY 2*1024 OF CHAR;  (* should be enough *)
    format: ConvTypes.ConvResults;
  BEGIN
    ReadReal (s, TRUE);
    IF Done THEN
      RealStr.Take (s, n, format);
      Done := (format = ConvTypes.allRight)
    END    
  END LongReal;

PROCEDURE Real* (VAR n: REAL);
(* Returns the real constant n at the current position according to the format:
     RealConst = digit {digit} ["." {digit} ["E" ("+" | "-") digit {digit}]]. 
*)     
  VAR
    s: ARRAY 2*1024 OF CHAR;  (* should be enough *)
    long: LONGREAL;
    format: ConvTypes.ConvResults;
  BEGIN
    ReadReal (s, FALSE);
    IF Done THEN
      RealStr.Take (s, long, format);
      Done := (format = ConvTypes.allRight) & 
              (MIN(REAL) <= long) & (long <= MAX(REAL));
      IF Done THEN
        n := SHORT (long)
      END
    END    
  END Real;

PROCEDURE String* (VAR s: ARRAY OF CHAR);
(* Returns the string s at the current position according to the format:
     StringConst = '"' char {char} '"'.
   The string must not contain characters less than blank such as EOL or 
   tab. *)
  VAR
    i : INTEGER;
  BEGIN
    SkipWhitespace;
    IF (lookAhead = '"') THEN
      GetChar;
      i := 0;
      REPEAT
        IF (lookAhead < " ") THEN  (* illegal character *)
          Done := FALSE
        ELSIF (i < LEN(s)-1) THEN
          s[i] := lookAhead;
          INC (i);
          GetChar
        ELSE  (* string too long *)
          Done := FALSE
        END
      UNTIL ~Done OR (lookAhead = '"');
      IF Done THEN  (* skip " character, terminate string *)
        GetChar;
        s[i] := 0X
      END
    ELSE
      Done := FALSE
    END
  END String;

PROCEDURE Name* (VAR s: ARRAY OF CHAR);
(* Returns the name s at the current position according to the file name format
   of the underlying operating system (e.g. "lib/My.Mod" under Unix). 
   Note: This implementation defines a name as  
     Name = char {char},
   where char is any character greater than blank. *)
  VAR
    i: INTEGER;
  BEGIN
    SkipWhitespace;
    IF (lookAhead > " ") THEN
      i := 0;
      REPEAT
        IF (i < LEN(s)-1) THEN
          s[i] := lookAhead;
          INC (i);
          GetChar
        ELSE  (* string too long *)
          Done := FALSE
        END
      UNTIL ~Done OR (lookAhead < " ");
      IF Done THEN  (* terminate string *)
        s[i] := 0X
      END
    ELSE  
      Done := FALSE
    END
  END Name;

BEGIN
  Done := FALSE  (* using In without calling Open first won't work *)
END In.
