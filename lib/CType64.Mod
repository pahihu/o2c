MODULE CType ["C"] EXTERNAL [""];

(*
These types are intended to be equivalent to their C counterparts.
They may vary depending on your system, but as long as you stick to a 32 Bit
Unix they should be fairly safe.
*)  

TYPE
  char* = CHAR;
  signedchar* = CHAR;                    (* signed char *)
  shortint* = SHORTINT;                  (* short int *)
  int* = INTEGER;
  longint* = LONGINT;                    (* long int *)
  address* = LONGINT;
  float* = REAL;
  double* = LONGREAL;

  enum1* = int;
  enum2* = int;
  enum4* = int;
  
  (* if your gcc uses short enumerations, you'll have to replace the
     above declarations with
  enum1* = SHORTINT;
  enum2* = INTEGER;
  enum4* = LONGINT;
  *)
  
  FILE* = address;  (* this is acually a replacement to 'FILE*', i.e. a pointer *)
  size_t* = longint;
  uid_t* = int;
  gid_t* = int;


TYPE  (* some commonly used C array types *)
  charPtr1d* = POINTER TO ARRAY OF char;
  charPtr2d* = POINTER TO ARRAY OF charPtr1d;
  intPtr1d* = POINTER TO ARRAY OF int;

TYPE
  Proc* = PROCEDURE;
  
END CType.

