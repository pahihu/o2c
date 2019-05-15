MODULE FIn;
(*
This module contains procedures similar to the ones in 'In'. The Modul reads
from a file. It must be opened first with the function 'Open' and has
to be closed again with 'Close'.

Autor : Bernd Bohnet <bohnet@ind.uni-stuttgart.de>
        0195
        o2c-1.10
Example

VAR Name   : ARRAY 20 OF CHAR;
    i      : LONGINT;
BEGIN
  ...
  FOut.Open ("score"); (* Open a File with filename "score". *) 
  FOut.String(Name);   (* Writes a String *)
  FOut.Char(CHR(0));   (* Adds to the String 0X (End of String) *)
  FOut.Int(punkte,3);  (* Writes Integer of length three. *)
                       (* FOut writes integes as ASCII text. *)
  FOut.Char(CHR(0));   (* Adds to the String 0X (End of String) *)
...
  FOut.Close;          (* Writes buffer out. *)
  
END  
 
  WHILE ~ FIn.eof DO
    FIn.Open("score")    (* Opens file "score" for read. *)
    FIn.String(Name);    (* Reads String with end 0X *)
    FIn.Int(i);          (* Reads String and interpret it as LONGINT *)
  END;                   (* Last read before eof is dirty read. *)
*)


IMPORT
  F := Files, IntStr, Strings, Rts, RealStr;


CONST
  fieldWidth = RealStr.maxSigFigs+15;
  

VAR
  file : F.File;                         (* this is the file we read from *)
  writeErrorMsg: ARRAY 256+32 OF CHAR;
  rider : F.Rider;


PROCEDURE Open* (name: ARRAY OF CHAR);
(* Opens the file with 'name' for input.  Allocates buffer, if none is present
   yet.  Resets the fileposition to 0.  The file is not registered.  If the 
   file cannot be opened, an error message is printed and the program 
   terminated. *)
  BEGIN
    file := F.Old (name);
    IF file # NIL THEN
      F.Set (rider, file, 0);
      (* prepare error message in case a write fails *)
      writeErrorMsg := "[FIn] Failed by read from file ";
      Strings.Append (name, writeErrorMsg)      
    ELSE
      writeErrorMsg := "[FIn] Couldn't open input file ";
      Strings.Append (name, writeErrorMsg);
      Rts.Error (writeErrorMsg)
    END
  END Open;

PROCEDURE Char* (VAR ch : CHAR);
(* Reads 'ch' from file. *)
BEGIN
  IF ~ rider.eof THEN
    F.Read (rider, ch);
  END;
END Char;

PROCEDURE String* (VAR str : ARRAY OF CHAR);
(* Reads 's' from file. *)

VAR    i : INTEGER;

BEGIN
  i := 0;
  F.Read (rider, str[i]);
  WHILE ~ rider.eof & ~ (str[i] = CHR(10))  DO
  (* Read until chr(10) or eof *)
    INC (i);
    F.Read (rider, str[i]);
  END;
  str[i]:=CHR(0); (* Set last char to end of String CHR(0). *)
END String;
      
PROCEDURE eof*() : BOOLEAN;

BEGIN
  RETURN rider.eof;
END eof;

PROCEDURE Int* (VAR i : LONGINT);
(* Read a string and convert it in an integer. *)

VAR str: ARRAY 32 OF CHAR;
      
BEGIN
  String(str);
  i := IntStr.Value (str);
END Int;

PROCEDURE LongReal*() : LONGREAL;
(* Reads the long real number x at the current position of the input stream as
   string. The string has to end with newline (CHR(10)). And is converted to
   a long real nummber. *)
   
VAR
  str : ARRAY fieldWidth+1 OF CHAR;
BEGIN
  String(str);
  RETURN RealStr.Value(str);
END LongReal;

PROCEDURE Real*() : REAL;
(* Reads the long real number x at the current position of the input stream as
   string. The string has to end with newline (CHR(10)). And is converted to
   a real nummber. *)
BEGIN
  RETURN SHORT(LongReal());
END Real;

END FIn.
