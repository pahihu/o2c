MODULE o2b;

IMPORT
  Rts,Out,OSym,OTable,OBrowse,Filenames,OMachine,Redir;

CONST
  version = "1.16";
  date = "Fri Mar 28 13:15:30 MET 1997";
  
PROCEDURE PrintHelp;
  BEGIN
    Out.String("o2b [options] modulename"); Out.Ln;
    Out.String("Options:"); Out.Ln;
    Out.String("--help, -h      Print this text."); Out.Ln;
    Out.String("--version, -V   Print version."); Out.Ln;
    Out.String("--external, -e  Print external names."); Out.Ln;
    Out.String("--expand, -x    Expand records, show inherited fields."); Out.Ln;
    Out.String ("--redir <file>     Use <file> as redirection table (default is ~/.o2c.red)"); Out.Ln;
  END PrintHelp;

PROCEDURE HandleArguments;
  VAR
    mod: OTable.Object;
    expand,external: BOOLEAN;
    argno: INTEGER;
    arginfo: ARRAY 256 OF CHAR;
    redirFile: ARRAY Redir.maxPathLen OF CHAR;
  BEGIN
    IF (Rts.ArgNumber() < 1) THEN
      PrintHelp
    ELSE
      expand:=FALSE; external:=FALSE;
      COPY ("~/.o2c.red", redirFile);
      argno:=1;
      REPEAT
        Rts.GetArg(argno,arginfo);
        IF (arginfo[0] = "-") THEN
          IF (arginfo = "--version") OR (arginfo = "-V") THEN
            Out.String("o2b "); 
            Out.String (version);
            Out.String (", ");
            Out.String (date);
            Out.Ln;
            RETURN;
          ELSIF (arginfo = "--help") OR (arginfo = "-h") THEN
            PrintHelp;
            RETURN;
          ELSIF (arginfo = "--expand") OR (arginfo = "-x") THEN
            expand:=TRUE;
          ELSIF (arginfo = "--external") OR (arginfo = "-e") THEN
            external:=TRUE;
          ELSIF (arginfo = "--redir") THEN
            INC (argno);
            IF (argno > Rts.ArgNumber()) THEN
              Out.String ("Missing argument for `--redir' option");
              Out.Ln;
              RETURN
            ELSE
              Rts.GetArg (argno, redirFile)
            END
          ELSE
            Out.String("Illegal option: "); Out.String(arginfo); Out.Ln;
          END; (* IF *)
          IF (Rts.ArgNumber() > argno) THEN
            INC(argno);
            Rts.GetArg(argno,arginfo);
          ELSE
            Out.String("Module name missing"); Out.Ln;
            RETURN;
          END; (* IF *)
        END; (* IF *)
      UNTIL (arginfo[0] # "-");
      OMachine.redir := Redir.Read (redirFile);
      Filenames.GetFile(arginfo,arginfo); (* Remove all additional stuff from modulename *)
      mod:=OSym.GetImport(arginfo,"");
      IF (mod # NIL) THEN
        OBrowse.Browse(mod,expand,external);
      ELSE
        Out.String("Symbolfile for '"); Out.String(arginfo); Out.String("' not found or not valid."); Out.Ln;
      END; (* IF *)
    END; (* IF *)
  END HandleArguments;

PROCEDURE Run*;
  BEGIN
    OTable.InitTable;
    HandleArguments;
  END Run;

BEGIN
  Run
END o2b.
