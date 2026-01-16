MODULE OMachine;

IMPORT 
  Redir;


CONST
  (* This constant controls whether the compiler generates per default code for
     the gc package.  This default value may be overidden with the options 
     --nogc resp. --usegc.  See README for details. *)
  defCodeForGC* = TRUE;
  (* Where is the gc library located?  And how is it linked in?
     a) if the non-directory part of gcLibPath is "gc.a": gcLibPath will be put
        in the linkers object list (example: gcLibPath = "~/o2gcc/Test/gc.a");
     b) if gcLibPath is empty (i.e. gcLibPath=""), the compiler assumes that 
        the library file libgc.* is somewhere in the linker's standard search 
        paths, and links the gc with -lgc
     c) if gcLibPath isn't empty, it's assumed to be the path to the library
        libgc.*, and the linker is called with the parameters -L<gcLibPath> 
        -lgc (example: gcLibPath = "/usr/lang/o2c/lib") *)
  gcLibPath* = "";
  
  
  maxSizeString* = 256;     (* Length(string constant)<maxSizeString *)
  maxSizeIdent* = 48;       (* Length(identifier)<maxSizeIdent *)
  maxLIntHexDigits* = 16;   (* max number of hex digits in LONGINT *)
  (* MIN/MAX values of the standard data types *)
  minBool* = 0;             maxBool* = 1;
  minChar* = 0;             maxChar* = 0FFH;
  minSInt* = -8000H;        maxSInt* = 07FFFH;
  minInt*  = -7FFFFFFFH-1;  maxInt*  = 07FFFFFFFH;
  minLInt* = -7FFFFFFFFFFFFFFFH-1;  maxLInt* = 07FFFFFFFFFFFFFFFH;
  minReal* = MIN(REAL);     maxReal* = MAX(REAL);      (* defined in _OGCC.h *)
  minLReal*= MIN(LONGREAL); maxLReal*= MAX(LONGREAL);  (* defined in _OGCC.h *)
  minSet*  = 0;             maxSet*  = 31;
  (* index range for PUTREG, GETREG, CC and HALT: *)
  minRegNum*=0; maxRegNum*=-1;           (* disabled *)
  minCC*=0; maxCC*=-1;                   (* disabled *)
  minTrapNum*=0; maxTrapNum*=255;
  defAssertTrap*=1;                      (* default trap for ASSERT(x) *)

CONST
  (* file extensions *)
  moduleExtension*     = "Mod";          (* module files *)
  symbolExtension*     = "OSym";         (* symbol files *)

CONST
  (* enable parsing of EXTERNAL modules *)
  allowExternal* = TRUE;
  (* allow underscore to be used in identifiers declared EXTERNAL *)
  allowUnderscore* = TRUE;
  (* allow 'rest parameter ..' in EXTERNAL modules *)
  allowRestParam* = TRUE;
  (* allow use of UNION type in EXTERNAL modules *)
  allowUnion* = TRUE;
  
VAR
  redir* : Redir.Pattern;
  generateCodeForGC*: BOOLEAN;

BEGIN
  generateCodeForGC := defCodeForGC
END OMachine.
