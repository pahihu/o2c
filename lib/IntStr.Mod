MODULE IntStr;

IMPORT
  Conv := ConvTypes, Str := Strings, Char := CharInfo;


PROCEDURE Take* (str : ARRAY OF CHAR; VAR int : LONGINT;
                 VAR format : Conv.ConvResults);
  (* pre:  "str" has a string value                              *)
  (* post: either value of "format" is "allRight",               *)
  (*         value of "int" is the corresponding LONGINT         *)
  (*       or value of "format" is "outOfRange",                 *)
  (*         value of "int" is MAX(LONGINT) or MIN(LONGINT)      *)
  (*       or value of "format" is "wrongFormat"                 *)
  (*         value of "int" is undefined                         *)
  (*       or value of format is "noData"                        *)
  (*         value of "int" is unchanged                         *)
  (* The accepted syntax is: {whitespace} ["+" | "-"] digit {digit} *)
  VAR
    s, e : INTEGER;
    digit : LONGINT;
    positive : BOOLEAN;
  BEGIN
    (* skip any amount of leading whitespace *)
    s := 0;
    WHILE Char.IsSpace (str [s]) DO  
      INC (s)
    END;
    IF (str [s] = 0X) THEN  (* no chars left behind position 's' *)
      format := Conv.noData
    ELSE  (* 's' holds the position of the first non-space character *)
      IF Char.IsSign (str [s]) THEN  (* optional sign *)
        positive := (str [s] # "-");
        INC (s)
      ELSE  (* no sign, positive number *)
        positive := TRUE
      END;
      e := s;
      (* skip over digits *)
      WHILE Char.IsDigit (str [e]) DO 
        INC (e)
      END;
      IF (str [e] # 0X) THEN  (* not at end of string: wrong format *)
        format := Conv.wrongFormat
      ELSE  (* string ends after last digit *)
        (* here holds: str[s..e-1] is a string of digits without leading sign *)
        (* now the cyphers are read and their negative sum calculated *)
        int := 0;
        WHILE (s # e) DO
          digit := ORD (str [s])-ORD ("0");
          IF (int > -214748364) OR (int >= (MIN (LONGINT)+digit) DIV 10) THEN
            int := int*10-digit;
            INC (s)
          ELSE
            format := Conv.outOfRange;
            IF positive THEN
              int := MAX (LONGINT)
            ELSE
              int := MIN (LONGINT)
            END;
            RETURN
          END
        END;
        IF positive THEN  (* int holds a positive number: switch sign *)
          IF (int = MIN (LONGINT)) THEN  (* overflow, not representable *)
            format := Conv.outOfRange;
            int := MAX (LONGINT);
            RETURN
          ELSE
            int := -int
          END
        END;
        format := Conv.allRight
      END
    END
  END Take;

PROCEDURE Format* (str : ARRAY OF CHAR) : Conv.ConvResults;
  (* pre:  "str" has a string value                              *)
  (* post: returned value corresponds to format of string        *)
  (*       value with respect to the type LONGINT.               *)
  VAR
    int : LONGINT;
    format : Conv.ConvResults;
  BEGIN
    Take (str, int, format);
    RETURN format
  END Format;

PROCEDURE Value* (str : ARRAY OF CHAR) : LONGINT;
  (* pre:  "str" has a string value                              *)
  (*       and format is "allRight" with respect to LONGINT      *)
  (* post: returned value is the corresponding LONGINT           *)
  VAR
    int : LONGINT;
    format : Conv.ConvResults;
  BEGIN
    Take (str, int, format);
    RETURN int
  END Value;


PROCEDURE Reverse (VAR str : ARRAY OF CHAR; start, end : INTEGER);
(* Reverts order of characerts in the intervall [start..end]. *)
  VAR
    h : CHAR;
  BEGIN
    WHILE (start < end) DO
      h := str [start]; str [start] := str [end]; str [end] := h;
      INC (start); DEC (end)
    END
  END Reverse;

PROCEDURE Give* (VAR str : ARRAY OF CHAR; int : LONGINT;
                 width : INTEGER; where : Conv.Alignment);
  (* post: as far as capacity of "str" allows,                             *)
  (*       the character representation of "int" is contained in "str"     *)
  (*       in a field of at least "width" characters                       *)
  (*	   left, centre (numeric), or right justified;                     *)
  CONST
    maxLength = 11;  (* maximum number of digits representing a LONGINT value *)
  VAR
    b : ARRAY maxLength+1 OF CHAR;
    s, e, i, pos : INTEGER;
  BEGIN
    (* build representation in string 'b' *)
    IF (int = MIN (LONGINT)) THEN        (* smallest LONGINT, -int is an overflow *)
      b := "-2147483648";
      e := 11
    ELSE
      IF (int < 0) THEN                  (* negative sign *)
        b[0] := "-"; int := -int; s := 1
      ELSE  (* no sign *)
        s := 0
      END;
      e := s;                            (* 's' holds starting position of string *)
      REPEAT
        b[e] := CHR (int MOD 10+ORD ("0"));
        int := int DIV 10;
        INC (e)
      UNTIL (int = 0);
      b[e] := 0X;
      Reverse (b, s, e-1)
    END;
    (* add whitespace to  'str' *)
    s := width-e;  (* difference field length - string length *)
    IF (s > 0) THEN  (* space left in string *)
      FOR i := 0 TO s-1 DO str[i] := " " END;
      str [s] := 0X;
      (* calculate position of string 's' in string 'str' *)
      CASE where OF
        Conv.left  : pos := 0 |
        Conv.right : pos := s |
        Conv.centre: pos := s-s DIV 2
      END;
      (* and finally insert 'b' into 'str' *)
      Str.Insert (b, pos, str)
    ELSE  (* no enough space, just copy 'b' to 'str' *)
      COPY (b, str)
    END
  END Give;

PROCEDURE Append* (int : LONGINT; width : INTEGER; where : Conv.Alignment; 
                   VAR dest : ARRAY OF CHAR);
  (* Append output of Give to string 'dest'. *)
  VAR
    str : ARRAY 32 OF CHAR;
  BEGIN
    Give (str, int, width, where);
    Str.Append (str, dest)
  END Append;

END IntStr.
