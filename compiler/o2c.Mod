MODULE o2c;

IMPORT
  Out, Rts, Strings, Strings2, Redir, Dos, Files,
  OParse, OTable, OSym, OMachine, OScan, OEParse, ODepend,
  BackMake := OMakeGCC, BackGen := OGenGCC;

CONST
  make=0; makeall=make+1;
  version = "1.16";
  date = "Fri Mar 28 13:15:30 MET 1997";

VAR
  nocom: BOOLEAN; (* set to TRUE if nothing has to be compiled (with -V -h) *)
  makeval: SET;
  prgname: ARRAY 256 OF CHAR;
  options: ARRAY 1024 OF CHAR;


PROCEDURE Compile (fileName: ARRAY OF CHAR; VAR err: BOOLEAN);
  VAR
    root: OEParse.Node;
    found: BOOLEAN;
    mod : ODepend.Module;
  BEGIN
    (* filter fileName through the redirection mechanism, if
       it doesn't denote an existing file *)
    IF ~Dos.Exists (fileName) THEN
      err := ~Redir.FindPath (OMachine.redir, fileName, fileName);
    END; (* IF *)
    (* init scanner and symbol table, parse source file *)
    OScan.Init (fileName, OScan.maxBufferSize, err);
    IF ~err THEN
      OParse.Module (root);
      OScan.Close;
      IF OScan.noerr THEN
        mod := BackMake.ModuleInfo (fileName);
        found := ODepend.FindFile (mod, ODepend.flSymExists, OMachine.symbolExtension);
        OSym.Export (mod, FALSE);
        (* compile module *)
        BackGen.Module (mod, root);
      END; (* IF *)
      OTable.RecycleMem (OTable.compiledModule);
      OEParse.RecycleMem (root);
      err := ~OScan.noerr
    END (* IF *)
  END Compile;

PROCEDURE Make (modName, comName: ARRAY OF CHAR; makeAll: BOOLEAN; VAR err: BOOLEAN);
  VAR
    object, module: OTable.Object;
    depList, dep: ODepend.Module;
    import: ODepend.Import;
    found: BOOLEAN;

  PROCEDURE Compile (mod: ODepend.Module; forceNewSym: BOOLEAN; VAR err: BOOLEAN);
    VAR
      root: OEParse.Node;
      msg: ARRAY 2*Redir.maxPathLen OF CHAR;
    BEGIN
      (* get file name of sources for module 'mod' *)
      IF ~(ODepend.flModExists IN mod. flags) THEN
        Out.String ("Error: Can't find "); 
        Out.String (mod. name); Out.Char (".");
        Out.String (OMachine.moduleExtension); Out.Ln;
        err := TRUE;
        RETURN
      END; (* IF *)
      (* write equivalent compile command *)
      Rts.GetArg (0, msg);
      Strings2.AppendChar (" ", msg);
      Strings.Append (options, msg);
      Strings.Append (mod.file[ODepend.flModExists], msg);
      OScan.VerboseMsg (msg);
      (* init scanner and symbol table, parse source file *)
      OScan.Init (mod.file[ODepend.flModExists], OScan.maxBufferSize, err);
      IF ~err THEN
        OParse.Module (root);
        OScan.Close;
        IF OScan.noerr THEN
          OSym.Export (mod, forceNewSym);
          BackGen.Module (mod, root)
        END;
        OTable.RecycleMem (OTable.compiledModule);
        OEParse.RecycleMem (root);
        err := ~OScan.noerr
      END (* IF *)
    END Compile;

  BEGIN
    (* generate list of modules (including their import relationship) *)
    depList := ODepend.Dependencies (modName, err);
    IF ~err THEN
      (* first pass: generate the compiler output *)
      dep := depList;
      WHILE (dep # NIL) & ~err DO
        BackMake.CompilationHook (dep);  (* evaluate external information first *)
        IF makeAll THEN
          INCL (dep.flags, ODepend.flCompile);
        ELSE
          (* test, whether symbol file exists *)
          found := ODepend.FindFile (dep, ODepend.flSymExists, OMachine.symbolExtension);
          IF ~found THEN
            INCL (dep. flags, ODepend.flCompile)
          END;
          IF (ODepend.flNoSources IN dep. flags) THEN  (* no sources: print error *)
            Out.String ("Error: Can't find source file ");
            Out.String (dep. name);
            Out.String (".Mod");
            Out.Ln;
            err := TRUE
          END;
          IF (ODepend.flModExists IN dep. flags) THEN (* sources exists, check if they need to be recompiled *)
            (* test if some back end specific files are missing/out of date *)
            IF ~err & ~(ODepend.flCompile IN dep. flags) & BackMake.NeedCompile (dep) THEN
              INCL (dep. flags, ODepend.flCompile)
            END;
            (* test if the module is depending on changed symbol files *)
            IF ~err & ~(ODepend.flCompile IN dep. flags) THEN
              import := dep. import;
              WHILE (import # NIL) DO
                IF (ODepend.flSymChanged IN import. module. flags) THEN
                  INCL (dep. flags, ODepend.flCompile)
                END;
                import := import. next
              END
            END
          END
        END;

        (* compile module if flCompile has been set in one of the previous steps *)
        IF ~err & (ODepend.flCompile IN dep. flags) THEN
          Compile (dep, makeAll, err)
        END;
        dep := dep. next
      END;

      IF ~err & (comName # "") THEN
        module := OSym.GetImport (modName, "");
        IF (module # NIL) THEN
          object := OTable.FindImport (comName, module)
        END;
        IF (module = NIL) OR (object = NIL) OR (object.type.len # 0) THEN
          Out.String ("Error: No parameterless procedure "); Out.String (comName);
          Out.String (" exported by "); Out.String (modName); Out.Char ("."); Out.Ln;
          err := TRUE;
        END; (* IF *)
      END; (* IF *)

      (* second pass: create executable *)
      IF ~err THEN
        BackMake.Build (modName, comName, depList, makeAll, err)
      END
    END
  END Make;

PROCEDURE Help;
  BEGIN
    Out.String (prgname); Out.String (" [options] --make [options] module [command]"); Out.Ln;
    Out.String (prgname); Out.String (" [options] {file}"); Out.Ln;
    Out.String ("where: "); Out.Ln;
    Out.String ("  module : Name of main module to be compiled"); Out.Ln;
    Out.String ("  command: Name of command to be executed"); Out.Ln;
    Out.String ("  file   : Name of file to be compiled"); Out.Ln;
    Out.String ("Options:"); Out.Ln;
    Out.String ("--help, -h         Print this text"); Out.Ln;
    Out.String ("--verbose, -v      Print additional information (e.g. steps when running make)"); Out.Ln;

    Out.String ("--make, -M         Compile modules which have changed, produce an executable"); Out.Ln;
    Out.String ("--all, -A          Force compilation of all modules (with --make)"); Out.Ln;
    Out.String ("--warn, -W         Print warnings"); Out.Ln;
    Out.String ("--version, -V      Print version"); Out.Ln;
    Out.String ("--usegc / --nogc   Enable/disable use of gc package"); Out.Ln;
    BackMake.Help
  END Help;


PROCEDURE Option (arg: ARRAY OF CHAR): BOOLEAN;
(* pre: 'arg' is a command line parameter, an option to be precise.
   post: 'result' is TRUE iff the back end can handle the passed option.
   side: Some back end internal flags are set depending on the argument
     passed. *)
  BEGIN
    IF (arg = "-h") OR (arg = "--help") THEN
      Help;
      nocom := TRUE;
    ELSIF (arg = "-v") OR (arg = "--verbose") THEN
      OScan.verbose := TRUE;
    ELSIF (arg = "-V") OR (arg = "--version") THEN
      nocom := TRUE;
      IF OMachine.generateCodeForGC THEN
        Out.String ("o2c/gc ")
      ELSE
        Out.String ("o2c ")
      END;
      Out.String (version);
      Out.String (", ");
      Out.String (date);
      Out.Ln
    ELSIF (arg = "-W") OR (arg = "--warn") THEN
      OScan.warnings := TRUE
    ELSIF (arg = "-M") OR (arg = "--make") THEN
      INCL (makeval, make);
      RETURN TRUE
    ELSIF (arg = "-A") OR (arg = "--all") THEN
      INCL (makeval, makeall);
      RETURN TRUE
    ELSIF (arg = "--nogc") THEN
      OMachine.generateCodeForGC := FALSE
    ELSIF (arg = "--usegc") THEN
      OMachine.generateCodeForGC := TRUE
    ELSE
      RETURN FALSE
    END; (* IF *)
    Strings.Append (arg, options);
    Strings.Append (" ", options);
    RETURN TRUE
  END Option;

PROCEDURE OptionExt (arg0, arg1: ARRAY OF CHAR): BOOLEAN;
(* pre: 'arg0', 'arg1' are command line parameters, 'arg1' follows 'arg0'.
   post: 'result' is TRUE iff the back end can handle the passed option.
   side: Some back end internal flags are set depending on the argument
     passed. *)
  BEGIN
    RETURN FALSE
  END OptionExt;


PROCEDURE EvalOptions (VAR arg: INTEGER);
  VAR
    pos, len: INTEGER;
    opt: ARRAY 3 OF CHAR;
    argument, extension: ARRAY 512 OF CHAR;

  PROCEDURE GetNextExt (VAR extension: ARRAY OF CHAR);
    BEGIN
      IF (arg < Rts.ArgNumber()) THEN
        Rts.GetArg (arg+1, extension);
      ELSE
        COPY ("", extension);
      END; (* IF *)
    END GetNextExt;

  PROCEDURE Accept (arg0, arg1: ARRAY OF CHAR): BOOLEAN;
    BEGIN
      IF ~Option (arg0) & ~BackMake.Option (arg0) THEN
        IF OptionExt (arg0, arg1) OR BackMake.OptionExt (arg0, arg1) THEN
          INC (arg);
        ELSE
          Out.String ("illegal option: "); Out.String (arg0); Out.Ln;
          INC (arg);
          RETURN (FALSE);
        END; (* IF *)
      END; (* IF *)
      RETURN (TRUE);
    END Accept;

  BEGIN
    COPY ("- ", opt);
    arg := 1;
    LOOP
      IF (arg > Rts.ArgNumber()) THEN EXIT; END;
      Rts.GetArg (arg, argument);
      GetNextExt (extension);
      len := Strings.Length (argument);
      IF (argument[0] = "-") THEN (* really an argument *)
        IF (argument[1] = "-") THEN (* that's a full name option *)
          IF Accept (argument, extension) THEN 
            INC (arg)
          END
        ELSE
          FOR pos := 1 TO (len-1) DO
            opt[1] := argument[pos];
            IF Accept (opt, extension) THEN END
          END; (* FOR *)
          INC (arg)
        END (* IF *)
      ELSE (* no more arguments *)
        EXIT
      END (* IF *)
    END (* LOOP *)
  END EvalOptions;

PROCEDURE Run*;
  VAR
    error: BOOLEAN;
    arg: INTEGER;
    file, command: ARRAY 256 OF CHAR;
  BEGIN
    Rts.GetArg (0, prgname);
    arg := 1;
    nocom := FALSE;
    makeval := {};
    error := FALSE;

    IF (Rts.ArgNumber() > 0) THEN
      EvalOptions (arg);
      OTable.InitTable;
      BackMake.Init;
      (* all following args should be file names *)
      IF (arg > Rts.ArgNumber()) THEN  (* error: no module name given *)
        IF ~nocom THEN
          Out.String ("Error: Nothing to compile."); Out.Ln;
          error := TRUE
        END  (* IF *)
      ELSE
        IF (make IN makeval) OR (BackMake.makefile # "") THEN  (* run make *)
          Rts.GetArg (arg, file);
          INC (arg);
          IF (arg <= Rts.ArgNumber()) THEN
            Rts.GetArg (arg, command);
          ELSE
            command := "";
          END; (* IF *)
          IF (BackMake.makefile # "") THEN  
            (* create Makefile, don't build executable *)
            BackMake.Makefile (file, command, options, error)
          ELSE  (* run make to produce an executable *)
            Make (file, command, (makeall IN makeval), error)
          END (* IF *)
        ELSE (* just compile single module *)
          WHILE (arg <= Rts.ArgNumber()) & ~error DO
            Rts.GetArg (arg, file);
            Compile (file, error);
            INC (arg)
          END (* WHILE *)
        END (* IF *)
      END; (* IF *)

      IF error THEN
        Rts.exitCode := 1;
      END (* IF *)
    ELSE
      Help;
    END; (* IF *)
  END Run;

BEGIN
  Files.permanentClose := TRUE;  (* let File.Close deallocate file handle *)
  Run
END o2c.
