MODULE FOut;
(*
This module contains procedures similar to the ones in 'Out'.  The output is
written into a file.  It must be opened first with the function 'Open' and has
to be closed again with 'Close' in order to write the data to disk and to
register the file.
*)

IMPORT
  F := Files, CharInfo, IntStr, RealStr, ConvTypes, Strings, Rts, Out;

CONST
  sizeBuffer = 8*1024;                   (* size of write buffer *)
  fieldWidth = RealStr.maxSigFigs+15;


TYPE
  Buffer = POINTER TO BufferDesc;
  BufferDesc = ARRAY sizeBuffer OF CHAR;

VAR
  file : F.File;                         (* this is the file we write to *)
  writeErrorMsg: ARRAY 256+32 OF CHAR;
  rider : F.Rider;
  buf : Buffer;
  pos : INTEGER;

PROCEDURE Open* (name: ARRAY OF CHAR);
(* Opens the file with 'name' for output.  Allocates buffer, if none is present
   yet.  Resets the fileposition to 0.  The file is not registered.  If the 
   file cannot be opened, an error message is printed and the program 
   terminated. *)
  BEGIN
    file := F.New (name);
    IF file # NIL THEN
      F.Set (rider, file, 0);
      IF (buf = NIL) THEN
        NEW (buf)
      END;
      pos := 0;
      (* prepare error message in case a write fails *)
      writeErrorMsg := "[FOut] Failed to write to file ";
      Strings.Append (name, writeErrorMsg)      
    ELSE
      writeErrorMsg := "[FOut] Couldn't create output file ";
      Strings.Append (name, writeErrorMsg);
      Rts.Error (writeErrorMsg)
    END
  END Open;

PROCEDURE Flush*;
(* Flushes the internal buffer, i.e. writes out all data in the buffer but 
   not yet written to the file.  If the write fails an error message is printed
   and the program terminated.  *)
  BEGIN
    F.WriteBytes (rider, buf^, pos);
    IF (rider. res # 0) THEN
      Rts.Error (writeErrorMsg)
    ELSE
      pos := 0
    END
  END Flush;

PROCEDURE Close*;
(* Flush buffer, register file and close it. *)
  BEGIN
    Flush;
    F.Register (file);
    F.Close (file)
  END Close;

PROCEDURE Char* (ch : CHAR);
(* Writes 'ch' to file. *)
  BEGIN
    IF (pos=sizeBuffer) THEN
      Flush
    END;
    buf[pos] := ch;
    INC (pos)
  END Char;

PROCEDURE String* (str : ARRAY OF CHAR);
(* Writes 's' to file without adding a newline.  *)
  VAR
    i : INTEGER;
  BEGIN
    i := 0;
    WHILE (str[i] # 0X) DO
      IF (pos = sizeBuffer) THEN
        Flush
      END;
      buf[pos] := str[i];
      INC (pos);
      INC (i)
    END
  END String;

PROCEDURE Ln*;
(* Writes 'newline' to file. *)
  BEGIN
    Char (CharInfo.lf);
  END Ln;

PROCEDURE Int* (i, n : LONGINT);
(* Converts 'i' into a string, places it right oriented in a field
   of width 'n' and writes it to the file. *)
  VAR
    str: ARRAY 32 OF CHAR;
  BEGIN
    IntStr.Give (str, i, SHORT (n), ConvTypes.right);
    String (str)
  END Int;

PROCEDURE Real* (x: REAL; n: INTEGER);
(* Writes the real number x to the end of the output stream using an
   exponential form.  If the textual representation of x requires m characters
   (including a two-digit signed exponent), x is right adjusted in a field of
   Max (n, m) characters padded with blanks at the left end.  A plus sign of the
   mantissa is not written. *)
  VAR
    str : ARRAY fieldWidth+1 OF CHAR;
  BEGIN
    Out.Real2Str (x, RealStr.maxSigFigs DIV 2-1, 2, str);
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
   Max (n, m) characters padded with blanks at the left end.  A plus sign of the
   mantissa is not written. *)
  VAR
    str : ARRAY fieldWidth+1 OF CHAR;
  BEGIN
    Out.Real2Str (x, RealStr.maxSigFigs-1, 3, str);
    (* pad left side with chars *)
    DEC (n, Strings.Length (str));
    WHILE (n > 0) DO
      Char (" ");
      DEC (n)
    END;
    String (str)
  END LongReal;

BEGIN
  buf := NIL
END FOut.
