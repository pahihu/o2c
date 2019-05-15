MODULE RealStr;
 
IMPORT 
  SYSTEM, Unix, C := CType, Str := Strings, Strings2, IntStr, 
  Conv := ConvTypes;

  
(* 
  the text form of an real number is                               
  where w ~ whitespace, and d ~ decimal digit			
  [w] ["+"|"-"] d{d} ["." {d}] ["E" ["+"|"-"] d{d}]        
*)

CONST
  (* Maximum of significant figures of a given real number in decimal
     representation.  More figures are considered beyond the number's
     actual precision. *)
  maxSigFigs* = 17;
  (* Maximum of figures used to represent the exponent of a number. *)
  maxExpFigs = 3;
  
(* 
The strategy to convert real numbers into a textual representation is the
following:
First call the libc function sprintf to get a first textual form of the number.
This is then transformed into an representation of the form 
  S0DDDDDDDDDDDDDDDDDESXXX
                       ^ maxExpFigs digits exponent
                      ^ exponent's sign
     ^ the (implicit) decimal point is between the first and second digit 
    ^ maxSigFigs digits
  ^ sign 
Rounding is performed on this representation and finally the string is 
transformed into the desired output format.
*)

CONST
  width = 2+maxSigFigs+2+maxExpFigs;
  posExp = 2+maxSigFigs;
  
TYPE  
  NumberString = ARRAY width+1 OF CHAR;

  
  
(* convert text to real: *)
  
PROCEDURE Take* (str : ARRAY OF CHAR; VAR real : LONGREAL; VAR format : Conv.ConvResults);
  (* pre:  "str" has a string value
     post: either value of "format" is "allRight",
             value of "real" is the corresponding LONGREAL
           or value of "format" is "wrongFormat"
             value of "real" is undefined  *)
  VAR
    tail : C.address;
  BEGIN
    real := Unix.strtod (str, tail);
    IF (tail # Unix.NULL) & (tail # SYSTEM.ADR (str[Str.Length(str)])) THEN
      format := Conv.wrongFormat
    ELSIF Unix.is_hugeval (real) THEN
      format := Conv.outOfRange
    ELSE
      format := Conv.allRight
    END
  END Take;

PROCEDURE Format* (str : ARRAY OF CHAR) : Conv.ConvResults;
  (* pre:  "str" has a string value
     post: returned value corresponds to format of string
           value with respect to the type LONGREAL *)
  VAR
    f : Conv.ConvResults;
    r : LONGREAL;
  BEGIN
    Take (str, r, f);
    RETURN f
  END Format;

PROCEDURE Value* (str : ARRAY OF CHAR) : LONGREAL;
  (* pre:  "str" has a string value
           and format is "allRight" with respect to LONGREAL
     post: returned value is the corresponding LONGREAL *)
  VAR
    f : Conv.ConvResults;
    r : LONGREAL;
  BEGIN
    Take (str, r, f);
    RETURN r
  END Value;




(* convert real to text: *)

PROCEDURE ConvertNumber (x : LONGREAL; VAR s : NumberString);
  VAR
    i : INTEGER;
    result : C.int;
    template : ARRAY 20 OF CHAR;
  BEGIN
    result := Unix.sprintf (template, "%%+0#%d.%dE", width, maxSigFigs-1);
    result := Unix.sprintf (s, template, x);
    (* if number has a two digit exponent: expand exponent to
       three digits and remove leading '0' *)
    IF (s[posExp+1] = "E") THEN
      FOR i := 2 TO posExp+2 DO
        s[i-1] := s[i]
      END;
      s[posExp+2] := "0"
    END;
    (* create internal representation: remove decimal point and
       add a leading '0' *)
    s[2] := s[1];
    s[1] := "0"
  END ConvertNumber;

PROCEDURE Align (VAR str : ARRAY OF CHAR; width : INTEGER; where : Conv.Alignment);
  VAR
    len, ins, pos : INTEGER;
  BEGIN
    len := Str.Length (str); 
    ins := width-len;
    IF (ins > 0) THEN
      CASE where OF
        Conv.left  : pos := len |
        Conv.right : pos := 0 |
        Conv.centre: pos := len-ins DIV 2
      END;
      WHILE (ins > 0) DO
        Str.Insert (" ", pos, str);
        DEC (ins)
      END
    END
  END Align;

PROCEDURE Exponent (s : NumberString) : INTEGER;
  VAR
    e : ARRAY 5 OF CHAR;
    i : LONGINT;
    f : Conv.ConvResults;
  BEGIN
    Str.Extract (s, posExp+1, 4, e);
    IntStr.Take (e, i, f);
    RETURN SHORT (i)
  END Exponent;
  
PROCEDURE RoundSigFigs (sigFigs : INTEGER; VAR s : NumberString);
(* pre: 0<=sigFigs, s contains number in internal representation *)
  VAR
    i : INTEGER;
    
  PROCEDURE RoundUp () : BOOLEAN;
  (* TRUE iff number has to be rounded to higher (absolute) value. *)
    BEGIN
      IF (s[2+sigFigs] # "5") THEN
        RETURN (s[2+sigFigs] > "5")
      ELSE
        FOR i := 3+sigFigs TO 1+maxSigFigs DO
          IF (s[i] # "0") THEN
            RETURN TRUE
          END
        END;
        (* last resort: round to even lower digit number *)
        RETURN ((ORD (s[1+sigFigs])-ORD("0")) MOD 2 = 1)
      END
    END RoundUp;
    
  BEGIN
    (* Ignore less than zero significant places.  If rounding to maxSigFigs
       or more is demanded nothing has to be done since we don't have that
       much digits. *)
    IF (0 <= sigFigs) & (sigFigs < maxSigFigs) THEN      
      IF RoundUp () THEN
        i := 1+sigFigs;
        LOOP
          s[i] := CHR (ORD (s[i])+1);
          IF (s[i] <= "9") THEN
            EXIT
          END;
          s[i] := "0";
          DEC (i)
        END
      END;
      FOR i := 2+sigFigs TO 1+maxSigFigs DO
        s[i] := "0"
      END;
      (* normalize number if s[1] # "0" *)
      IF (s[1] = "1") THEN
        (* divide mantissa by 10 *)
        s[1] := "0";
        s[2] := "1";
        (* increment exponent by 1 *)
        i := Exponent (s)+1;
        IF (i >= 0) THEN
          s[posExp+1] := "+"
        END;
        s[posExp+2] := CHR (ORD ("0") + ABS (i) DIV 100);
        s[posExp+3] := CHR (ORD ("0") + (ABS (i) DIV 10) MOD 10);
        s[posExp+4] := CHR (ORD ("0") + ABS (i) MOD 10)        
      END
    END
  END RoundSigFigs;


PROCEDURE GiveFloat* (VAR str : ARRAY OF CHAR; real : LONGREAL; 
                      sigFigs, width : INTEGER; where : Conv.Alignment);
(* pre: 1 <= sigFigs < 256 
   post: as far as capacity of 'str' allows, the string representation 
     of 'real' is contained in 'str' in a field of at least 'width' characters
     using 'sigFigs' significant places *)
  VAR
    i, j, exp : INTEGER;
    n : NumberString;
    buffer : ARRAY 256+7 OF CHAR;
  BEGIN
    ConvertNumber (real, n);
    RoundSigFigs (sigFigs, n);
    (* copy significant digits into buffer *)
    j := sigFigs;
    IF (j > maxSigFigs) THEN
      j := maxSigFigs
    END;
    FOR i := 0 TO j-1 DO
      buffer[i] := n[2+i]
    END;
    FOR i := j TO sigFigs-1 DO
      buffer[i] := "0"
    END;
    buffer[sigFigs] := 0X;
    (* place decimal point *)
    IF (sigFigs > 1) THEN
      Str.Insert (".", 1, buffer)
    END;
    (* add sign *)
    IF (real < 0.0) THEN
      Str.Insert ("-", 0, buffer)
    END;
    (* add exponent with at least two digits *)
    exp := Exponent (n);
    Strings2.AppendChar ("E", buffer);    
    IF (exp < 0) THEN
      Strings2.AppendChar ("-", buffer)
    ELSE
      Strings2.AppendChar ("+", buffer)
    END;
    IF (ABS (exp) < 10) THEN
      Strings2.AppendChar ("0", buffer)
    END;
    IntStr.Append (ABS (exp), 0, Conv.left, buffer);
    (* copy buffer to 'str' and do alignment *)
    COPY (buffer, str);
    Align (str, width, where)    
  END GiveFloat;

PROCEDURE GiveEng* (VAR str : ARRAY OF CHAR; real : LONGREAL; 
                    sigFigs, width : INTEGER; where : Conv.Alignment);
(* pre: 1 <= sigFigs < 256 
   post: as far as capacity of 'str' allows, the engineering representation
     of 'real' is contained in 'str' in a field of at least 'width' characters
     using 'sigFigs' significant places *)
  VAR
    i, j, exp : INTEGER;
    n : NumberString;
    buffer : ARRAY 256+7 OF CHAR;
  BEGIN
    ConvertNumber (real, n);
    RoundSigFigs (sigFigs, n);
    (* make sure that at least 1+exp MOD 3 digits are written *)
    exp := Exponent (n);
    IF (sigFigs <= exp MOD 3) THEN
      sigFigs := exp MOD 3+1
    END;
    (* copy significant digits into buffer *)
    j := sigFigs;
    IF (j > maxSigFigs) THEN
      j := maxSigFigs
    END;
    FOR i := 0 TO j-1 DO
      buffer[i] := n[2+i]
    END;
    FOR i := j TO sigFigs-1 DO
      buffer[i] := "0"
    END;
    buffer[sigFigs] := 0X;
    (* place decimal point *)
    IF (1+exp MOD 3 < sigFigs) THEN
      Str.Insert (".", 1+exp MOD 3, buffer)
    END;
    DEC (exp, exp MOD 3);
    (* add sign *)
    IF (real < 0.0) THEN
      Str.Insert ("-", 0, buffer)
    END;
    (* add exponent with at least two digits *)
    Strings2.AppendChar ("E", buffer);
    IF (exp < 0) THEN
      Strings2.AppendChar ("-", buffer)
    END;
    IF (ABS (exp) < 10) THEN
      Strings2.AppendChar ("0", buffer)
    END;
    IntStr.Append (ABS (exp), 0, Conv.left, buffer);
    (* copy buffer to 'str' and do alignment *)
    COPY (buffer, str);
    Align (str, width, where)
  END GiveEng;

PROCEDURE GiveFixed* (VAR str : ARRAY OF CHAR; real : LONGREAL; 
                      place, width : INTEGER; where : Conv.Alignment);
(* pre: place < 256
   post: as far as capacity of 'str' allows, the fixed point representation
     of 'real' is contained in 'str', rounded at position 'place'. Positive
     values of 'place' correspond to the fraction part. *)     
  VAR
    n : NumberString;
    i, j, c, exp : INTEGER;
    buffer : ARRAY 256+1 OF CHAR;
  BEGIN
    ConvertNumber (real, n);
    RoundSigFigs (1+Exponent (n)+place, n);
    exp := Exponent (n);
    j := 2;
    (* write decimal part *)
    IF (exp < 0) OR (-place > exp) THEN
      buffer := "0";
      i := 1
    ELSE
      i := 0;
      WHILE (i <= exp) DO
        IF (i < maxSigFigs) & (i <= exp+place) THEN
          buffer[i] := n[j]
        ELSIF (i < LEN(buffer)) THEN
          buffer[i] := "0"
        END;
        INC (i); INC (j)
      END
    END;
    IF (place > 0) THEN
      (* write fractional part *)
      buffer[i] := ".";
      INC (i);
      c := 0;
      WHILE (c < place) DO
        IF (j-2 < maxSigFigs) THEN
          buffer[i] := n[j]
        ELSIF (i < LEN(buffer)) THEN
          buffer[i] := "0"
        END;
        INC (i); INC (j); INC (c)
      END
    END;
    buffer[i] := 0X;
    (* add sign *)
    IF (real < 0.0) THEN
      Str.Insert ("-", 0, buffer)
    END;
    (* copy buffer to 'str' and do alignment *)
    COPY (buffer, str);
    Align (str, width, where)    
  END GiveFixed;

END RealStr.
