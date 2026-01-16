MODULE Unix ["C"] EXTERNAL ["Unix.c"];  (* C land ahead.  Enter at your own risc! *)

IMPORT
  C := CType;
  
CONST
  nl* = 0AX; 
  NULL* = 0;
  EOF* = -1;
  
  ENOENT* = 2;
  EINTR* = 4;
  ENFILE* = 23;
  EMFILE* = 24;
  
  SEEK_SET* = 0;
  SEEK_CUR* = 1;
  SEEK_END* = 2;
  
  F_OK* = 0;
  
TYPE
  fd* = C.int;
  
TYPE 
  timeval* = RECORD
    tv_sec* : C.longint;                    (* seconds *)
    tv_usec* : C.int;                       (* microseconds *)
  END;
  Proc* = PROCEDURE;
  
VAR
  stdin- ["Unix_stdin"],              
  stdout- ["Unix_stdout"],             
  stderr- ["Unix_stderr"] : C.FILE;      

PROCEDURE Errno* ["Unix_Errno"] (): C.int;  (* read variable 'errno' *)
PROCEDURE is_hugeval* ["Unix_is_hugeval"](x: C.double): BOOLEAN;
     
PROCEDURE printf* (template : ARRAY OF C.char; ..) : C.int;
PROCEDURE sprintf* (VAR s : ARRAY OF C.char; template : ARRAY OF C.char; ..) : C.int;
PROCEDURE exit* (status : C.int);
PROCEDURE system* (command : ARRAY OF C.char) : C.int;
PROCEDURE gettimeofday* (VAR tp : timeval; tzp : C.address) : C.int;
PROCEDURE settimeofday* (VAR tp : timeval; tzp : C.address) : C.int;
PROCEDURE fputc* (c : C.int; stream : C.FILE) : C.int;
PROCEDURE fputs* (s : ARRAY OF C.char; stream : C.FILE) : C.int;
PROCEDURE fgetc* (stream : C.FILE) : C.int;
PROCEDURE fgets* (VAR s : ARRAY OF C.char; count : C.int; stream : C.FILE) : C.address;
PROCEDURE strtol* (string : ARRAY OF C.char;  VAR tailptr : C.address; base : C.int) : C.longint;
PROCEDURE strtod* (string : ARRAY OF C.char; VAR tailptr : C.address) : C.double;
PROCEDURE fopen* (filename : ARRAY OF C.char; opentype : ARRAY OF C.char) : C.FILE;
PROCEDURE fclose* (stream : C.FILE) : C.int;
PROCEDURE unlink* (filename : ARRAY OF C.char) : C.int;
PROCEDURE rename* (oldname : ARRAY OF C.char; newname : ARRAY OF C.char) : C.int;
PROCEDURE ftell* (stream : C.FILE) : C.longint;
PROCEDURE fseek* (stream : C.FILE; offset : C.longint; whence : C.int) : C.int;
PROCEDURE fflush* (stream : C.FILE) : C.int;
PROCEDURE fread* (data : C.address; size : C.size_t; count : C.size_t; stream : C.FILE) : C.size_t;
PROCEDURE fwrite* (data : C.address; size : C.size_t; count : C.size_t; stream : C.FILE) : C.size_t;
PROCEDURE feof* (stream : C.address) : C.int;
PROCEDURE access* (filename : ARRAY OF C.char; how : C.int) : C.int;
PROCEDURE getuid*() : C.uid_t;
PROCEDURE getgid*() : C.gid_t;
PROCEDURE tmpnam* (VAR result : ARRAY OF C.char) : C.address;
PROCEDURE getcwd* (VAR buffer : ARRAY OF C.char; sizeBuffer : C.int) : C.address;
PROCEDURE time* (result : C.address) : C.longint;

PROCEDURE mktemp* (VAR template : ARRAY OF C.char) : C.address; (* [JnZ] *)
PROCEDURE tmpfile* () : C.FILE; (* [JnZ] *)


PROCEDURE open* (path : ARRAY OF C.char; pflag : C.int; ..) : fd;
PROCEDURE close* (fildes : fd) : C.int;
PROCEDURE lseek* (fildes : fd ; offset : C.longint (* off_t *); whence : C.int) : C.longint (* off_t *);
PROCEDURE ftruncate* (fildes : fd; length : C.size_t) : C.int; 

PROCEDURE read* (fildes: fd; buf: C.address; nbyte: C.size_t): C.size_t;
PROCEDURE write* (fildes: fd; buf: C.address; nbyte: C.size_t): C.size_t;

END Unix.

(* some words of warning concerning EXTERNAL:

Don't confuse C macros with actual functions or variables.  E.g. 
'stdin' from stdio.h is on most systems a macro expanding into 
something like __IO[0] or, even worse, &__IO[0].  Since EXTERNAL
doesn't know about these #defines and #macros they can't be properly
put into the generated header file.  If you are programming for 
portability you should keep this in mind.  

Large, system dependent types are inherently not portable since this
mechanism doesn't use the standard system include files.  But you 
can always write a .c file that encapsulates these types and makes 
them accessible in a portable way (e.g., via procedure calls) and 
define an EXTERNAL to this .c file.

*)
