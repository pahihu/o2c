MODULE OMakeGCC;  (* Author: Michael van Acken *)
(* 	$Id: OMakeGCC.Mod,v 1.47 1996/06/28 04:51:21 oberon1 Exp $	 *)

IMPORT
  Rts, CharInfo, Str := Strings, Out, Time, FOut, Redir, Dos, Strings2,
  Filenames,
  M := OMachine, S := OScan, T := OTable, Dep := ODepend, Gen := OGenGCC;


VAR
  makeOberon : BOOLEAN;
  cc : ARRAY 64 OF CHAR;
  cflags, ldflags : ARRAY 256 OF CHAR;
  inclPaths : POINTER TO ARRAY OF CHAR;
  redirFile, makefile- : ARRAY 256 OF CHAR;
  
  
PROCEDURE GetMarkerTime (modName : ARRAY OF CHAR; VAR time : Time.Time) : BOOLEAN;
  VAR
    marker : ARRAY Redir.maxPathLen OF CHAR;
    err : BOOLEAN;
  BEGIN
    IF Redir.FindPathExt (M.redir, modName, Gen.markerExt, marker) THEN
      Dos.GetDate (marker, time, err);
      RETURN ~err
    ELSE
      RETURN FALSE
    END
  END GetMarkerTime;


PROCEDURE CompilationHook* (mod : Dep.Module);
  BEGIN
    IF (Dep.flExternal IN mod. flags) THEN
      IF Strings2.Match ("*.c", mod. extName) THEN
        (* external .c file: lookup file, store modification time and set flag *)
        mod. extClass := Gen.extCSource;
        IF Dep.FindFilename (mod, Gen.cFile, mod. extName) THEN
          INCL (mod. flags, Gen.cFile)
        END
      ELSIF Strings2.Match ("*.o", mod. extName) THEN
        (* external .a file: lookup file, store modification time and set flag *)
        mod. extClass := Gen.extObject;
        IF Dep.FindFilename (mod, Gen.objFile, mod. extName) THEN
          INCL (mod. flags, Gen.objFile)
        END
      ELSIF Strings2.Match ("*.a", mod. extName) THEN
        (* external .a file: lookup file, store modification time and set flag *)
        mod. extClass := Gen.extArchive;
        IF Dep.FindFilename (mod, Gen.objFile, mod. extName) THEN
          INCL (mod. flags, Gen.objFile)
        END
      ELSE
        (* external library reference *)
        mod. extClass := Gen.extLibrary     
      END
    END
  END CompilationHook;

PROCEDURE NeedCompile* (mod : Dep.Module) : BOOLEAN;
  (* pre: 'mod' is a module whose sources exist in 'mod.file[0]', 
       'mod.time[0]' contains the source file's time stamp, 
       'mod.import' the list of imported modules.
     post: Result is TRUE if one of the back end generated files is 
       missing, the sources are more recent than the output or
       an imported symbol file is more recent than the output. *)
  VAR
    import: Dep.Import;
    existsH, existsC : BOOLEAN;
    mtime : Time.Time;
  BEGIN
    (* set flags header and cFile *)
    existsH := Dep.FindFile (mod, Gen.header, Gen.hExt);
    IF (Dep.flExternal IN mod. flags) THEN
      existsC := GetMarkerTime (mod. name, mtime)
    ELSE
      existsC := Dep.FindFile (mod, Gen.cFile, Gen.cExt);
      mtime := mod. time[Gen.cFile]
    END;
    (* recompile if the .h or the .c file do not exist or, 
       if this module is external, .m is missing *)
    IF ~(existsH & existsC) THEN
      RETURN TRUE
    END;
    (* recompile if an imported symbol file is more recent than the output *)
    import := mod. import;
    WHILE (import # NIL) DO
      IF (Time.Cmp (import. module. time[Dep.flSymExists], mtime) < 0) THEN
        RETURN TRUE
      END;
      import := import. next
    END;
    (* recompile if the sources are more recent than the .c or .m file *)
    RETURN (Time.Cmp (mod. time[Dep.flModExists], mtime) < 0)
  END NeedCompile;
  

PROCEDURE ModuleInfo* (fileName : ARRAY OF CHAR) : Dep.Module;
  VAR
    mod : Dep.Module;
    found : BOOLEAN;
  BEGIN
    (* initialize module information block *)
    NEW (mod);
    mod. next := NIL;
    mod. flags := {Dep.flModExists};
    mod. import := NIL;
    COPY (fileName, mod. file[Dep.flModExists]);
    COPY (T.compiledModule. name, mod. name);
    found := Dep.FindFile (mod, Gen.header, Gen.hExt);
    RETURN mod
  END ModuleInfo;

    
    
PROCEDURE WriteName (modName, comName, suffix : ARRAY OF CHAR);
  BEGIN
    FOut.String (modName);
    IF (comName # "") THEN
      FOut.Char ("_");
      FOut.String (comName)
    END;
    FOut.String (suffix)
  END WriteName;
  
PROCEDURE GenMainModule (module : Dep.Module; modName, comName : ARRAY OF CHAR; modules : Dep.Module);
(* pre: 'module' is an empty description block, 'modName' the name of the main
     module, 'comName' is either an exported, parameterless procedure of the 
     main module or empty, 'modules' is the module list as provided by 
     'Dep.Dependencies'.
   post: A file "_modName_comName.c" (resp. "_modName.c") is generated (its 
     complete name is stored in 'module.file[cFile]').  This file consists of 
     includes for all header files in 'modules', followed by a main function 
     that will call every module's initialization function (in the order of 
     'modules').  If 'comName' is given the denoted procedure will be called 
     then. *)
  VAR
    mod : Dep.Module;
    rts : BOOLEAN;
  BEGIN
    (* put together the .c file's name *)
    COPY ("_", module. name);
    Str.Append (modName, module. name);
    IF (comName # "") THEN
      Strings2.AppendChar ("_", module. name);
      Str.Append (comName, module. name)
    END;
    Dep.NewFile (module, Gen.cFile, Gen.cExt);
    module. flags := {Gen.cFile};
    FOut.Open (module. file[Gen.cFile]);
    
    (* write an include for every used module's header file *)
    rts := FALSE;
    mod := modules;
    WHILE (mod # NIL) DO
      Gen.WriteInclude (mod. file[Gen.header]);
      IF (mod. name = "Rts") THEN
        rts := TRUE
      END;
      mod := mod. next
    END;
    FOut.Ln;
    
    (* generate main() function *)
    IF rts THEN  (* fill Rts_arg variables iff Rts is imported *)
      FOut.String ("int main(int argc, char **argv) {"); FOut.Ln;
      FOut.String ("  Rts_argc = argc;"); FOut.Ln;
      FOut.String ("  Rts_argv = (void*)argv;"); FOut.Ln
    ELSE
      FOut.String ("int main(void) {"); FOut.Ln
    END;
    FOut.String ("  _init__OGCC();"); FOut.Ln;
    
    (* call init function for every used module *)
    mod := modules;
    WHILE (mod # NIL) DO
      IF ~(Dep.flExternal IN mod. flags) OR
         (mod. extClass = Gen.extCSource) THEN
        FOut.String ("  _init_");
        FOut.String (mod. name);
        FOut.String ("();");
        FOut.Ln
      END;
      mod := mod. next
    END;
    
    IF (comName # "") THEN
      (* now call the command *)
      FOut.String ("  ");
      WriteName (modName, comName, "();");
      FOut.Ln
    END;
    IF rts THEN
      (* return the exit code stored in Rts.exitCode *)
      FOut.String ("  return Rts_exitCode;")
    ELSE
      FOut.String ("  return 0;")
    END;
    FOut.Ln;
    FOut.String ("}");
    FOut.Close
  END GenMainModule;


PROCEDURE ContainedIn (str : ARRAY OF CHAR; prefix, pattern : ARRAY OF CHAR) : INTEGER;
(* Returns TRUE iff <prefix><pattern> occurs in str with whitespace before 
   and after the found occurence. *)
  VAR
    p : ARRAY Redir.maxPathLen+8 OF CHAR;
    i, l : INTEGER;
  BEGIN
    COPY (prefix, p);
    Str.Append (pattern, p);
    l := Str.Length (p);
    i := Str.Pos (p, str, 0);
    WHILE (i # -1) & 
          ~(((i = 0) OR (str[i-1] = " ")) &
            ((str[i+l] = 0X) OR (str[i+l] = " "))) DO
      i := Str.Pos (p, str, i+1)      
    END;
    RETURN i
  END ContainedIn;
  
PROCEDURE GenAddObjs (depList : Dep.Module; VAR str : ARRAY OF CHAR);
(* Generates list of additional object files as declared in the external 
   modules.  The result 'str' is either empty or has the form " a.o b.o ..". *)
  VAR
    mod : Dep.Module;
  BEGIN
    (* collect list of all object files in 'str' *)
    COPY ("", str);
    mod := depList;
    WHILE (mod # NIL) DO
      IF (mod. extClass IN {Gen.extObject, Gen.extArchive}) &
         (ContainedIn (str, "", mod. extName) < 0) THEN
        Strings2.AppendChar (" ", str);
        Str.Append (mod. file[Gen.objFile], str)
      END;
      mod := mod. next
    END
  END GenAddObjs;
    
PROCEDURE GetLinkerFlags (depList : Dep.Module; VAR str : ARRAY OF CHAR; VAR err : BOOLEAN);
(* Append list of additional libraries as declared in the external modules
  to the string ldFlags, return result in 'str'.  The added parts have the 
  form " -la -lb ..".  'err' is set to TRUE if there are cyclic dependencies 
  between the libraries. *)
  VAR
    mod, libList, lib : Dep.Module;
    import : Dep.Import;
    errLib : ARRAY M.maxSizeString OF CHAR;
    pos : INTEGER;
    
  PROCEDURE FindModule (libName : ARRAY OF CHAR) : Dep.Module;
  (* Return library of name 'libName' in 'libList', NIL if there is none. *)
    VAR
      mod : Dep.Module;
    BEGIN
      mod := libList;
      WHILE (mod # NIL) & (mod. name # libName) DO
        mod := mod. next;
      END;
      RETURN mod
    END FindModule;
  
  PROCEDURE AddImport (libName, importName : ARRAY OF CHAR);
  (* Add import of library 'importName' to library 'libName' if no 
     such import exists already and importName#libName. *)
    VAR
      lib, ilib : Dep.Module;
      import : Dep.Import;
    BEGIN
      IF (libName[0] # 0X) & (importName[0] # 0X) & (libName # importName) THEN
        lib := FindModule (libName);
        ilib := FindModule (importName);
        (* scan list of existing imports *)
        import := lib. import;
        WHILE (import # NIL) DO
          IF (import. module = ilib) THEN  (* import already exists *)
            RETURN
          END;
          import := import. next
        END;
        (* create new import link *)
        NEW (import);
        import. next := lib. import;
        import. module := ilib;
        lib. import := import
      END
    END AddImport;
    
  BEGIN
    COPY (ldflags, str);
    
    (* create list of external libraries *)
    mod := depList;
    libList := NIL;
    WHILE (mod # NIL) DO
      IF (mod. extClass = Gen.extLibrary) &
         (mod. extName[0] # 0X) &
         (FindModule (mod. extName) = NIL) THEN
        (* external library that is not part of the list *)
        NEW (lib);
        lib^ := mod^;
        COPY (lib. extName, lib. name);
        lib. import := NIL;
        lib. next := libList;
        libList := lib
      END;
      mod := mod. next
    END;
    
    IF (libList # NIL) THEN
      (* add direct imports of libraries *)
      mod := depList;
      WHILE (mod # NIL) DO
        IF (mod. extClass = Gen.extLibrary) THEN  (* mod is library *)
          import := mod. import;
          WHILE (import # NIL) DO
            IF (import. module. extClass = Gen.extLibrary) THEN  (* mod imports library *)
              AddImport (mod. extName, import. module. extName)
            END;
            import := import. next
          END
        END;
        mod := mod. next
      END;
      (* run topological sort of library list, placing the library with 
         the most dependencies at the start of the list*)
      libList := Dep.TopSort (libList, TRUE, errLib);
      
      IF (libList = NIL) THEN      
        (* error: cyclic import of libraries *)
        Out.String ("Error: Cyclic import of library ");
        Out.String (errLib);
        Out.String (" -- can't create linker flags.");
        Out.Ln;
        err := TRUE
      ELSE
        (* append list of all additional libraries to 'ldflags' *)
        mod := libList;
        WHILE (mod # NIL) DO
          IF (ContainedIn (str, "-l", mod. name) < 0) THEN
            Str.Append (" -l", str);
            Str.Append (mod. name, str)
          END;
          mod := mod. next
        END
      END
    END;
    
    (* link math lib and remove obsolete linking of libc *)
    IF (ContainedIn (str, "-l", "m") < 0) THEN
      Str.Append (" -lm", str)
    END;
    pos := ContainedIn (str, "-l", "c");
    IF (pos >= 0) THEN
      Str.Delete (str, pos-1, 4)
    END
  END GetLinkerFlags;

PROCEDURE LinkWithGC;
(* Adapt the compiler (cflags) and the linker (ldflags) if the garbage 
   collector is used. *)
  VAR
    full, file, path : ARRAY Redir.maxPathLen OF CHAR;
  BEGIN
    IF M.generateCodeForGC THEN
      Str.Append (" -DGC", cflags);
      Filenames.ExpandPath (full, M.gcLibPath);
      Filenames.GetPath (full, path, file);
      IF (file = "gc.a") THEN
        (* put archive file in the object list *)
        Strings2.AppendChar (" ", ldflags);
        Str.Append (full, ldflags)
      ELSE
        IF (full # "") THEN
          (* add additional path for libraries *)
          Str.Append (" -L", ldflags);
          Str.Append (full, ldflags)
        END;
        Str.Append (" -lgc", ldflags)
      END
    END
  END LinkWithGC;

    
    

PROCEDURE Build* (modName, comName: ARRAY OF CHAR; depList: Dep.Module; makeAll: BOOLEAN; VAR err : BOOLEAN);
(* pre:  'modName' is the name of the main module exporting 'comName' as a 
     parmaterless procedure (or 'comName' is empty).  'depList' the list of 
     modules used by the main module as provided by 'Dep.Dependencies', 
     completed with the information of the source, symbol and c file names 
     asscociated with each module (and their time stamps). For every used 
     module the .h and .c files are up to date. 
   post: The existence of the support files is checked, their object file 
     generated if necessary.  Each module's object file is compiled from its 
     C sources if it isn't up to date.  The file containing the main function 
     is generated, compiled and linked to the other object files. *)   
  VAR
    mod : Dep.Module;
    com, appStr, str : ARRAY 8*1024 OF CHAR;
    exec : ARRAY Redir.maxPathLen OF CHAR;
    sup, main : Dep.Module;
    found, compile : BOOLEAN;
    import : Dep.Import;
    
  PROCEDURE RunCC (mod : Dep.Module);
  (* pre: 'mod' describes a module whose C sources and all included header 
       files are up to date.
     post: The C compiler is run on the module, leaving the object file's name
       in mod.file[objFile] (and setting 'mod.flags[objFile]'.
       If no sources can be found an error message is printed. 'err' is TRUE 
       iff the object file couldn't be generated. *)
    BEGIN
      IF (mod # NIL) & ~(Gen.cFile IN mod. flags) THEN
        Out.String ("Error: Can't generate ");
        Out.String (mod. name);
        Out.String (".o - no C sources available.");
        Out.Ln;
        err := TRUE
      ELSE
        Dep.NewFile (mod, Gen.objFile, "o");
        COPY (cc, com);
        Str.Append (cflags, com);
        Str.Append (inclPaths^, com);
        Str.Append (" -o ", com);
        Str.Append (mod. file[Gen.objFile], com);
        Str.Append (" -c ", com);
        Str.Append (mod. file[Gen.cFile], com);
        S.VerboseMsg (com);
        err := (Rts.System (com) # 0);
        INCL (mod. flags, Gen.objFile)
      END
    END RunCC;

  BEGIN
    LinkWithGC;
    
    (* validate/generate _OGCC.o *)
    NEW (sup);
    sup. name := "_OGCC";
    sup. flags := {};
    IF ~Dep.FindFile (sup, Gen.header, Gen.hExt) THEN 
      (* no _OGCC.h, impossible to compile anything *)
      Out.String ("Error: Can't find file _OGCC.h.");
      Out.Ln;
      err := TRUE;
      RETURN
    END;
    found := Dep.FindFile (sup, Gen.cFile, Gen.cExt);
    found := Dep.FindFile (sup, Gen.objFile, "o");
    IF makeAll OR ~(Gen.objFile IN sup. flags) OR
       (Time.Cmp (sup. time[Gen.header], sup. time[Gen.objFile]) < 0) OR
       (Time.Cmp (sup. time[Gen.cFile], sup. time[Gen.objFile]) < 0) THEN
      (* _OGCC.o is missing or _OGCC.h/_OGCC.c is more recent than the object file *)
      RunCC (sup);
      IF err THEN
        RETURN
      END
    END;
    (* here holds: _OGCC.h and _OGCC.o exist, err=FALSE *)
    
    (* validate/generate .o files *)
    mod := depList;
    WHILE (mod # NIL) & ~err DO
      IF (Dep.flExternal IN mod. flags) & (mod. extClass # Gen.extCSource) THEN
        (* we have no sources we could compile *)
        compile := FALSE
      ELSE
        (* compile, if the object file does not exist or
          the .h file is more recent the the object file or
          the .c file is more recent the the object file or
          the file _OGCC.h is more recent than the object file *)
        compile := ~Dep.FindFile (mod, Gen.objFile, "o") OR
          (Time.Cmp (mod. time[Gen.header], mod. time[Gen.objFile]) < 0) OR
          (Time.Cmp (mod. time[Gen.cFile], mod. time[Gen.objFile]) < 0) OR
          (Time.Cmp (sup. time[Gen.header], mod. time[Gen.objFile]) < 0);
        IF ~compile THEN
          (* recompile, if any if the imported foreign .h files is more recent than the object file *)
          import := mod. import;
          WHILE (import # NIL) DO
            IF (Time.Cmp (import. module. time[Gen.header], 
                          mod. time[Gen.objFile]) < 0) THEN
              compile := TRUE
            END;
            import := import. next
          END
        END
      END;
      IF compile THEN
        (* the c sources have to be compiled. what a pity. take a break, get some tea. *)
        RunCC (mod)
      END;
      mod := mod. next
    END;
    (* here holds: the object files of the modules are all upto date *)

    (* create main function, compile it. *)
    main := NIL;                         (* stays NIL, if GenMainModule is not run *)
    IF ~err THEN
      NEW (main);
      GenMainModule (main, modName, comName, depList);
      RunCC (main)
    END;
    (* here holds: all needed object files are up to date *)
    
    (* link files together *)
    IF ~err THEN
      Str.Delete (main. name, 0, 1);
      Redir.GeneratePath (M.redir, main. name, exec);    
      COPY (cc, com);
      Str.Append (cflags, com);
      Str.Append (" -o ", com);
      Str.Append (exec, com);
      mod := depList;
      WHILE (mod # NIL) DO
        IF (mod. extClass # Gen.extLibrary) THEN
          Str.Append (" ", com);
          Str.Append (mod. file[Gen.objFile], com)
        END;
        mod := mod. next
      END;
      GenAddObjs (depList, str);
      Str.Append (str, com);
      Strings2.AppendChar (" ", com);
      Str.Append (sup. file[Gen.objFile], com);
      Strings2.AppendChar (" ", com);
      Str.Append (main. file[Gen.objFile], com);      
      GetLinkerFlags (depList, appStr, err);
      IF ~err THEN
        Str.Append (appStr, com);
        S.VerboseMsg (com);
        err := (Rts.System (com) # 0)
      END
    END;
    (* here holds: executable stored under the name in 'exec'. *)
  END Build;



PROCEDURE Help*;
  BEGIN
    Out.String ("--redir <file>     Use <file> as redirection table (default is ~/.o2c.red)"); Out.Ln;
    Out.String ("--makefile <file>  Write makefile into <file>"); Out.Ln;
    Out.String ("-a                 Disable assertions"); Out.Ln;
    Out.String ("-R                 Disable runtime checks"); Out.Ln;
    Out.String ("-O                 Optimize code (set -O2 when calling gcc)"); Out.Ln;
    Out.String ("-g                 Include debug information (on C level, -g3)"); Out.Ln;
    Out.String ("-s                 Strip executable"); Out.Ln;
    Out.String ("-p                 Include profiler information (for gprof)"); Out.Ln;
    Out.String ("--cflags <string>  Pass <string> as parameter to the C compiler"); Out.Ln;
    Out.String ("--ldflags <string> Pass <string> as parameter to the linker"); Out.Ln
  END Help;

PROCEDURE Option* (arg : ARRAY OF CHAR) : BOOLEAN;
(* pre: 'arg' is a command line parameter, an option to be precise.
   post: 'result' is TRUE iff the back end can handle the passed option.
   side: Some back end internal flags are set depending on the argument
     passed. *)
  BEGIN
    IF (arg = "-h") OR (arg = "--help") THEN
      Help
    ELSIF (arg = "-O") THEN
      Str.Append (" -O2", cflags)
    ELSIF (arg = "-g") THEN 
      Str.Append (" -g3", cflags);
    ELSIF (arg = "-s") THEN
      Str.Append (" -Wl,-s", ldflags)
    ELSIF (arg = "-p") THEN
      Str.Append (" -pg", cflags)
    ELSIF (arg = "-R") THEN
      Str.Append (" -DDISABLE_RTC", cflags)
    ELSIF (arg = "-a") THEN
      Str.Append (" -DDISABLE_ASSERT", cflags)
    ELSIF (arg = "-C") THEN
      makeOberon := FALSE
    ELSE
      RETURN FALSE
    END;
    RETURN TRUE
  END Option;

PROCEDURE OptionExt* (arg0, arg1 : ARRAY OF CHAR) : BOOLEAN;
(* pre: 'arg0', 'arg1' are command line parameters, 'arg1' follows 'arg0'.
   post: 'result' is TRUE iff the back end can handle the passed option.
   side: Some back end internal flags are set depending on the argument
     passed. *)
  BEGIN
    IF (arg0 = "-cc") THEN
      COPY (arg1, cc)
    ELSIF (arg0 = "--cflags") THEN
      Strings2.AppendChar (" ", cflags);
      Str.Append (arg1, cflags)
    ELSIF (arg0 = "--ldflags") THEN
      Strings2.AppendChar (" ", ldflags);
      Str.Append (arg1, ldflags)
    ELSIF (arg0 = "--redir") THEN
      COPY (arg1, redirFile)
    ELSIF (arg0 = "--makefile") THEN
      COPY (arg1, makefile)
    ELSE
      RETURN FALSE
    END;
    RETURN TRUE
  END OptionExt;
  



PROCEDURE Makefile* (modName, comName, o2opt : ARRAY OF CHAR; VAR err : BOOLEAN);
  VAR
    depList, mod, sup, main : Dep.Module;
    inode : Dep.Import;
    path : ARRAY Redir.maxPathLen OF CHAR;
    str, appStr : ARRAY 8*1024 OF CHAR;
    
  PROCEDURE WriteODepend (mod : Dep.Module; import : Dep.Import; depH : BOOLEAN);
    BEGIN
      FOut.String (mod. file[Gen.objFile]);
      FOut.String (": ");
      FOut.String (sup. file[Gen.header]);
      IF depH THEN
        FOut.Char (" ");
        FOut.String (mod. file[Gen.header])
      END;
      IF (import = NIL) & ~depH THEN
        FOut.String (" $(HFILES)")
      ELSE
        inode := import;
        WHILE (inode # NIL) DO
          FOut.Char (" ");
          FOut.String (inode. module. file[Gen.header]);
          inode := inode. next
        END
      END;
      FOut.Char (" ");
      FOut.String (mod. file[Gen.cFile]);
      FOut.Ln;
      FOut.Char (CharInfo.ht);
      FOut.String ("$(CC) $(CPPFLAGS) $(CFLAGS) -o ");
      FOut.String (mod. file[Gen.objFile]);
      FOut.String (" -c ");
      FOut.String (mod. file[Gen.cFile]);
      FOut.Ln
    END WriteODepend;
    
  BEGIN
    depList := Dep.Dependencies (modName, err);
    IF err THEN
      RETURN
    END;
    LinkWithGC;
    
    (* find position of the _OGCC files *)
    NEW (sup);
    sup. flags := {};
    sup. import := NIL;
    sup. name := "_OGCC";
    IF ~Dep.FindFile (sup, Gen.header, Gen.hExt) OR
       ~Dep.FindFile (sup, Gen.cFile, Gen.cExt) THEN
      Out.String ("Error: Can't locate _OGCC.h or _OGCC.c, aborting.");
      Out.Ln;
      err := TRUE;
      RETURN
    END;
    Dep.NewFile (sup, Gen.objFile, "o");
    
    (* set filenames to be used in the makefile *)
    mod := depList;
    WHILE (mod # NIL) DO
      CompilationHook (mod);
      IF ~(Dep.flModExists IN mod. flags) THEN
        Dep.NewFile (mod, Dep.flModExists, M.moduleExtension)
      END;
      Dep.NewFile (mod, Dep.flSymExists, M.symbolExtension);
      Dep.NewFile (mod, Gen.header, Gen.hExt);
      IF (mod. extClass # Gen.extCSource) OR ~(Gen.cFile IN mod. flags) THEN
        (* no overide on the .c file name through an external declaration *)
        Dep.NewFile (mod, Gen.cFile, Gen.cExt)
      END;
      Dep.NewFile (mod, Gen.objFile, "o");
      mod := mod. next
    END;

    (* write main module *)
    NEW (main);
    GenMainModule (main, modName, comName, depList);
    Dep.NewFile (main, Gen.objFile, "o");

    S.VerboseMsg ("Writing Makefile.");
    Redir.GeneratePath (M.redir, makefile, path);
    FOut.Open (path);
    (* variable declarations *)
    IF makeOberon THEN
      FOut.String ("O2C = o2c");
      FOut.Ln;
      FOut.String ("O2OPT = ");
      FOut.String (o2opt);
      FOut.Ln
    END;
    FOut.String ("CC = gcc-apple-4.2 -m32 -fnested-functions");
    FOut.Ln;
    FOut.String ("CFLAGS =");
    FOut.String (cflags);
    FOut.String (inclPaths^);
    FOut.Ln;
    FOut.String ("LDFLAGS =");
    GetLinkerFlags (depList, appStr, err);
    FOut.String (appStr);
    FOut.Ln;
    (* define variable containing all symbol files *)
    IF makeOberon THEN
      FOut.String ("SYMFILES =");
      mod := depList;
      WHILE (mod # NIL) DO
        FOut.Char (" ");
        FOut.String (mod. file[Dep.flSymExists]);
        mod := mod. next
      END;
      FOut.Ln
    END;
    (* define variable containing all header files (excluding _OGCC.h) *)
    FOut.String ("HFILES =");
    mod := depList;
    WHILE (mod # NIL) DO
      FOut.Char (" ");
      FOut.String (mod. file[Gen.header]);
      mod := mod. next
    END;
    FOut.Ln;
    (* define variable containing all C source files (excluding _OGCC.c) *)
    FOut.String ("CFILES0 =");  (* var containing the generated (and erasable) files *)
    mod := depList;
    WHILE (mod # NIL) DO
      IF ~(Dep.flExternal IN mod. flags) THEN
        FOut.Char (" "); 
        FOut.String (mod. file[Gen.cFile])
      END;
      mod := mod. next
    END;
    FOut.Ln;
    FOut.String ("CFILES1 = $(CFILES0)");  (* var containing all c source files *)
    mod := depList;
    WHILE (mod # NIL) DO
      IF (mod. extClass = Gen.extCSource) THEN
        FOut.Char (" "); 
        FOut.String (mod. file[Gen.cFile])
      END;
      mod := mod. next
    END;
    FOut.Ln;
    (* define variable containing all objects *)
    FOut.String ("OBJS0 = ");
    FOut.String (sup. file[Gen.objFile]);
    FOut.Char (" ");
    FOut.String (main. file[Gen.objFile]);
    mod := depList;
    WHILE (mod # NIL) DO
      IF ~(Dep.flExternal IN mod. flags) OR
         (mod. extClass = Gen.extCSource) THEN
        FOut.Char (" ");
        FOut.String (mod. file[Gen.objFile])
      END;
      mod := mod. next
    END;
    FOut.Ln;
    FOut.String ("OBJS1 =");
    GenAddObjs (depList, str);
    FOut.String (str);
    FOut.Ln;
    FOut.Ln;
    (* write rule for executable *)
    FOut.String ("# final targets");
    FOut.Ln;
    IF makeOberon THEN
      FOut.String ("all: oberon build")
    ELSE
      FOut.String ("all: build")
    END;
    FOut.Ln;
    FOut.String ("build: $(OBJS0) $(OBJS1)");
    FOut.Ln;
    FOut.Char (CharInfo.ht);
    FOut.String ("$(CC) $(CFLAGS) -o ");
    WriteName (modName, comName, "");
    FOut.String (" $(OBJS0) $(OBJS1) $(LDFLAGS)");
    FOut.Ln;
    IF makeOberon THEN
      FOut.String ("oberon: $(SYMFILES) $(HFILES) $(CFILES1)");
      FOut.Ln
    END;
    FOut.Ln;
    (* write rule for cleaning up *)
    FOut.String ("# clean will remove all generated files");
    FOut.Ln;
    FOut.String ("clean: ");
    FOut.Ln;
    FOut.Char (CharInfo.ht);
    IF makeOberon THEN
      FOut.String ("-rm $(SYMFILES) $(HFILES) $(CFILES0) $(OBJS0) _")
    ELSE
      FOut.String ("-rm $(HFILES) $(CFILES0) $(OBJS0) _")
    END;
    WriteName (modName, comName, "");
    FOut.Ln;
    FOut.Ln;
    
    (* write rules to create the necessary .OSym/.h/.c files *)
    IF makeOberon THEN
      mod := depList;
      WHILE (mod # NIL) DO
        FOut.String (mod. file[Dep.flSymExists]);
        FOut.Char (" ");
        FOut.String (mod. file[Gen.header]);
        FOut.Char (" ");
        FOut.String (mod. file[Gen.cFile]);
        FOut.String (": ");
        FOut.String (mod. file[Dep.flModExists]);
        inode := mod. import;
        WHILE (inode # NIL) DO
          FOut.Char (" ");
          FOut.String (inode. module. file[Dep.flSymExists]);
          inode := inode. next
        END;
        FOut.Ln;
        FOut.Char (CharInfo.ht);
        FOut.String ("$(O2C) $(O2OPT) ");
        FOut.String (mod. file[Dep.flModExists]);
        FOut.Ln;
        mod := mod. next
      END;
      FOut.Ln
    END;
    
    (* write rules to create the .o files *)
    WriteODepend (sup, NIL, TRUE);
    mod := depList;
    WHILE (mod # NIL) DO
      IF ~(Dep.flExternal IN mod. flags) OR (mod. extClass = Gen.extCSource) THEN
        WriteODepend (mod, mod. import, TRUE)
      END;
      mod := mod. next
    END;
    WriteODepend (main, NIL, FALSE);
    FOut.Ln;
    FOut.Close;
    err := FALSE
  END Makefile;

PROCEDURE Init*;
  VAR
    pattern: Redir.Pattern;
    path: Redir.Path;
    incl: ARRAY 8192 OF CHAR;
    
  PROCEDURE MatchesHeader (pattern: ARRAY OF CHAR): BOOLEAN;
    VAR
      i, len: INTEGER;
      suffix: ARRAY 256 OF CHAR;
    BEGIN
      len := Str.Length (pattern);
      (* first check whether a . is present in the pattern *)
      i := len-1;
      WHILE (i >= 0) & (pattern[i] # ".") DO
        DEC (i)
      END;
      (* check if suffix pattern matches .h *)
      IF (i >= 0) THEN   (* i is position of last . in pattern *)
        Str.Extract (pattern, i, MAX(INTEGER), suffix);
        IF Strings2.Match (suffix, ".h") THEN
          RETURN TRUE
        END
      END;
      (* check all suffix matches *)
      i := len-1;
      WHILE (i >= 0) DO
        IF (pattern[i] = "*") OR (pattern[i] = "?") THEN
          Str.Extract (pattern, i, MAX(INTEGER), suffix);
          IF Strings2.Match (suffix, ".h") THEN
            RETURN TRUE
          END
        END;
        DEC (i)
      END;
      RETURN FALSE
    END MatchesHeader;
  
  BEGIN
    M.redir := Redir.Read (redirFile);
    S.ReadErrorList;
    (* build include paths that are passed to gcc *)
    incl := "";
    pattern := M.redir;
    WHILE (pattern # NIL) DO
      IF MatchesHeader (pattern. wildcard) THEN
        path := pattern. paths;
        WHILE (path # NIL) DO
          Str.Append (" -iquote ", incl);
          Str.Append (path. path, incl);
          path := path. next
        END
      END;
      pattern := pattern. next
    END;
    (* IF (incl[0] # 0X) THEN
      make sure, that the previous paths are only used for the ".." includes
      and not for the <..> ones; this should avoid confusion if names of
      system header files and o2c header files overlap
      Str.Append (" -I-", incl)
    END; *)
    NEW (inclPaths, Str.Length (incl)+1);
    COPY (incl, inclPaths^)
  END Init;


BEGIN
  redirFile := "~/.o2c.red";
  makeOberon := TRUE;
  cc := "gcc-apple-4.2 -m32 -fnested-functions";
  cflags := "";
  ldflags := "";  (* -lm will be set by GetLinkerFlags *)
  inclPaths := NIL;
  makefile := ""
END OMakeGCC.
