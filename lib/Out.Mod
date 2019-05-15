MODULE Out;
(* see also [Oakwood Guidelines, revision 1A]
Module Out provides a set of basic routines for formatted output of characters,
numbers, and strings.  It assumes a standard output stream to which the symbols
are written.

Examples
                                output (asterisks denote blanks)
  Out.Open;
  Out.Int(-3, 5);               ***-3
  Out.Int(3, 0);                3
  Out.Real(1.5, 10);            ***1.5E+00  (original Oakwood: **1.50E+00(?))
  Out.Real(-0.005, 0)           -5.0E-03
*)

IMPORT 
  CType, Unix, Strings, ConvTypes, IntStr, RealStr;

  
CONST
  fieldWidth = RealStr.maxSigFigs+15;

  
PROCEDURE Open*;
(* Initializes the output stream. *)
  BEGIN    
  END Open;

PROCEDURE Char* (ch: CHAR);
(* Writes the character ch to the end of the output stream. *)
  VAR
    res : CType.int;
  BEGIN
    res := Unix.fputc (ORD (ch), Unix.stdout)
  END Char;

PROCEDURE String* (s: ARRAY OF CHAR);
(* Writes the null-terminated character sequence s to the end of the output
   stream (without 0X). *)
  VAR
    res : CType.int;
  BEGIN
    res := Unix.fputs (s, Unix.stdout)
  END String;

PROCEDURE Int* (i, n: LONGINT);
(* Writes the integer number i to the end of the output stream.  If the textual
   representation of i requires m characters, i is right adjusted in a field of
   Max(n, m) characters padded with blanks at the left end.  A plus sign is not
   written. *)
  VAR
    str: ARRAY 1024 OF CHAR;
  BEGIN
    IntStr.Give (str, i, SHORT (n), ConvTypes.right);
    String (str)    
  END Int;

PROCEDURE Real2Str* (x: LONGREAL; sigFigs, expCyphers: INTEGER; VAR str: ARRAY OF CHAR);
(* PRIVATE *)
(* Calls GiveFloat to generate a left justified textual representation of x 
   with sigFigs significant figures.  The result is extended to a number of 
   expCyphers cyphers in the exponent part, irrelevant zeros in the fraction 
   part are removed (but the fraction keeps at least one single digit) and 
   trailing blanks are removed.  The result is stored in str. *)
  VAR
    e, i : INTEGER;
  BEGIN
    RealStr.GiveFloat (str, x, sigFigs, SHORT (LEN (str))-1, ConvTypes.left);
    (* str has the form D.D{D}ESD{D}, where D is a digit, S is + or - *)
    (* find the position of the exponent *)
    e := 0;
    WHILE (str[e] # "E") DO
      INC (e)
    END;
    (* remove trailing blanks *)
    i := e+1;
    WHILE (str[i] # " ") & (str[i] # 0X) DO
      INC (i)
    END;
    str[i] := 0X;
    (* extend exponent to number of cyphers required *)
    WHILE (i-e-2 < expCyphers) DO
      Strings.Insert ("0", e+2, str);
      INC (i)
    END;
    (* find last non zero cypher *)
    i := e;
    WHILE (str[i-1] = "0") DO
      DEC (i)
    END;
    IF (str[i-1] = ".") THEN  (* don't strip only cypher in fraction *)
      INC (i)
    END;
    Strings.Delete (str, i, e-i)
  END Real2Str;
  
PROCEDURE Real* (x: REAL; n: INTEGER);
(* Writes the real number x to the end of the output stream using an 
   exponential form.  If the textual representation of x requires m characters
   (including a two-digit signed exponent), x is right adjusted in a field of
   Max(n, m) characters padded with blanks at the left end.  A plus sign of the
   mantissa is not written. *)
  VAR
    str : ARRAY fieldWidth+1 OF CHAR;
  BEGIN
    Real2Str (x, RealStr.maxSigFigs DIV 2-1, 2, str);
    (* pad left side with chars *)
    DEC (n, Strings.Length (str));
    WHILE (n > 0) DO
      Char (" ");
      DEC (n)
    END;
    String (str)
  END Real;

PROCEDURE LongReal* (x: LONGREAL; n: INTEGER);
(* Writes the long real number x to the end of the output stream using an 
   exponential form.  If the textual representation of x requires m characters
   (including a three-digit signed exponent), x is right adjusted in a field of
   Max(n, m) characters padded with blanks at the left end.  A plus sign of the
   mantissa is not written. *)
  VAR
    str : ARRAY fieldWidth+1 OF CHAR;
  BEGIN
    Real2Str (x, RealStr.maxSigFigs-1, 3, str);
    (* pad left side with chars *)
    DEC (n, Strings.Length (str));
    WHILE (n > 0) DO
      Char (" ");
      DEC (n)
    END;
    String (str)
  END LongReal;

PROCEDURE Ln*;
(* Writes an end-of-line symbol to the end of the output stream. *)
  BEGIN
    Char (Unix.nl)    
  END Ln;



(* Extensions that are not part of the Oakwood Guidelines: *)

PROCEDURE Flush*;    (* [no oakwood] *)
(* Writes internal buffers to the terminal or the file. *)
  VAR
    res : CType.int;
  BEGIN
    res := Unix.fflush (Unix.stdout)
  END Flush;

END Out.
