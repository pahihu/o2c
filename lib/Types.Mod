MODULE Types ["C"] EXTERNAL ["Types.c"];

IMPORT 
  SYSTEM;

CONST
  sizeIdent = 48;

TYPE
  Type* = POINTER TO TypeDesc;
  Name* = ARRAY sizeIdent OF CHAR;       (* ETH Oberon: Kernel.Name *)
  Module* = POINTER TO ModuleDesc;       (* ETH Oberon: Kernel.Module *)
  ModuleDesc* = RECORD
    next-: Module;                       (* list of linked together modules *)
    name-: Name;                         (* module identifier *)
    key: INTEGER;                        (* module identification *)
    tdlist: Type;
  END;
  TypeDesc* = RECORD
    (* prefix: array of base types *)
    name-: ARRAY sizeIdent OF CHAR;      (* type identifier *)
    module-: Module;                     (* module in which type is defined *)
      
    level: INTEGER;         (* extension level: 0 original, 1 once extended *)
    size: LONGINT;                       (* size of record in bytes *)
    next: Type;
    (* suffix: list of type bound procedures *)
  END;

VAR
  modules- ["Types_modules"]: Module;    (* ETH Oberon: Kernel.modules *)
  
  
PROCEDURE TypeOf* ["Types_TypeOf"] (o: SYSTEM.PTR): Type;
(* pre: 'o' is a POINTER TO R, where R is a record type whose type is declared
     in a normal module (and not in an EXTERNAL module like this one).
     'o' has to have a legal value, ie, has to be initialized with NEW.
   post: Result is a pointer to R's type descriptor. *)
   
PROCEDURE This* ["Types_This"] (mod: Module; name: ARRAY OF CHAR): Type;
(* pre: 'mod' is one of the modules in the list 'modules', 'name' the 
     identifier R associated with a record type R declared in 'module'.
   post: Result is the pointer to the type descriptor of type R if such a
     record declaration exists, NIL otherwise. *)
     
PROCEDURE LevelOf* ["Types_LevelOf"] (t: Type): INTEGER;
(* pre: 't' is a type descriptor (and is not NIL).
   post: Result is the extension level of t's record type T (0 if T is not
     an extended type, plus 1 for each level of extension). *)
     
PROCEDURE BaseOf* ["Types_BaseOf"] (t: Type; level: INTEGER): Type;
(* pre: 't' is a type descriptor (and is not NIL), 0 <= level <= LevelOf(t).
   post: Result is the type descriptor associated with t's extention level
     'level'.  level=0 will get the first (unextended) base type, level-1 the
     type of which 't' is an extension, level=LevelOf(t) with return 't'. *)

PROCEDURE NewObj* ["Types_NewObj"] (VAR o: SYSTEM.PTR; t: Type);
(* pre: 't' is a type descriptor (and is not NIL).
   post: A new object of type R is created, where R is the record type 
     associated with the type descriptor 't'.  A pointer to this object
     is returned in 'o'. *)
  
END Types.
