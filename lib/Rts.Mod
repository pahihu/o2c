MODULE Rts ["C"] EXTERNAL ["Rts.c"];

IMPORT
  CType;
  
  
VAR 
  (* code returned with normal program termination, i.e. when leaving _main *)
  exitCode* ["Rts_exitCode"]: INTEGER;  
  (* arguments to the C function main() *)
  argc- ["Rts_argc"]: CType.int;
  argv- ["Rts_argv"]: CType.charPtr2d;

PROCEDURE ArgNumber* ["Rts_ArgNumber"]() : INTEGER;
(* result: number of arguments, 0 if no args given *)

PROCEDURE GetArg* ["Rts_GetArg"](num : INTEGER; VAR arg : ARRAY OF CHAR);
(* pre : 0 <= `num' <= ArgNumber(), `arg' is long enough to hold the parameter.
   post: `arg' is a nul terminated string, corresponding to the num-th 
     parameter. `num=0' gives the name the program was called by. *)

PROCEDURE Terminate* ["Rts_Terminate"];
(* post: Terminates program, similar to HALT.  The value in `exitCode' is
     is used as the exit status. *)

PROCEDURE Error* ["Rts_Error"](msg : ARRAY OF CHAR);
(* post: Writes the error message to stderr and terminates the program with
     exit code 1. *)

PROCEDURE Assert* ["Rts_Assert"](expr : BOOLEAN; msg : ARRAY OF CHAR);
(* post: If `expr' is FALSE, the message is written to stderr and the program
     terminates with exit code 1. *)



PROCEDURE TerminationProc* ["Rts_TerminationProc"](proc: CType.Proc);
(* Adds `proc' to the list of procedures executed at program termination.
   Procedures are called in reverse order of registration.  
   If the registration is not successful due a limit on the number of 
   registered functions (normally 32, can be increased in Rts.c), the program 
   is aborted with an error message. 
   Note: The registered procedure will be called on normal program termination,
   if an error signal is raised, or if an terminatoin signal is raised. *)
  
PROCEDURE System* ["Rts_System"](command : ARRAY OF CHAR) : INTEGER;
(* Executes `command' as a shell command.  Result is the value returned by 
   the libc `system' function. *)

END Rts.
