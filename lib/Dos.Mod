MODULE Dos ["C"] EXTERNAL ["Dos.c"];

IMPORT 
  Time;

CONST
  pathSeperator* = "/";


PROCEDURE Delete* ["Dos_Delete"](file: ARRAY OF CHAR; VAR err : BOOLEAN);

PROCEDURE Rename* ["Dos_Rename"](old, new : ARRAY OF CHAR; VAR err : BOOLEAN);

PROCEDURE Copy* ["Dos_Copy"](src,dest: ARRAY OF CHAR; VAR err : BOOLEAN);

PROCEDURE Exists* ["Dos_Exists"](file : ARRAY OF CHAR) : BOOLEAN;
  (* TRUE if file exists, otherwise FALSE *)

PROCEDURE GetDate* ["Dos_GetDate"](file : ARRAY OF CHAR; VAR date : Time.Time; VAR err : BOOLEAN);

PROCEDURE GetUserHome* ["Dos_GetUserHome"](VAR home: ARRAY OF CHAR; user: ARRAY OF CHAR);
(* Get the user's home directory path (stored in /etc/passwd)
   or the current user's home directory if user="". *)

PROCEDURE GetCWD* ["Dos_GetCWD"](VAR path: ARRAY OF CHAR);
(* Get current working directory. *)

PROCEDURE TmpFileName* ["Dos_TmpFileName"](VAR tmpnam : ARRAY OF CHAR);
(* Generate unique temporary file name.  If no file name can be generated, 
   an empty string is returned in 'tmpnam'. *)
   
END Dos.


