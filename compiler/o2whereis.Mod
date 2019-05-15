MODULE o2whereis;  (* Author: Michael van Acken *)

IMPORT
  Rts, Out, Redir, Strings2, OMachine;


PROCEDURE WriteHelp;
  VAR
    arg : ARRAY Redir.maxPathLen OF CHAR;
  BEGIN
    Out.String ("Usage: ");
    Rts.GetArg (0, arg);
    Out.String (arg);
    Out.String (" [options] <file>"); Out.Ln;
    Out.String ("Options:"); Out.Ln;

    Out.String ("-r                 Print the name of the RCS file instead of the work file."); Out.Ln;
    Out.String ("--redir <file>     Use <file> as redirection table (default is ~/.o2c.red)"); Out.Ln;
  END WriteHelp;


PROCEDURE Query*;
  VAR
    i : INTEGER;
    printRCS : BOOLEAN;
    arg, file, redirFile : ARRAY Redir.maxPathLen OF CHAR;
  BEGIN
    printRCS := FALSE;
    IF (Rts.ArgNumber() < 1) THEN
      WriteHelp
    ELSE
      COPY ("~/.o2c.red", redirFile);
      i := 1;
      WHILE (i <= Rts.ArgNumber()) DO
        Rts.GetArg (i, arg);
        IF (arg = "-r") THEN
          printRCS := TRUE
        ELSIF (arg = "--help") THEN
          WriteHelp        
        ELSIF (arg = "--redir") THEN
          INC (i);
          IF (i > Rts.ArgNumber()) THEN
            Out.String ("missing argument for `--redir' option");
            Out.Ln
          ELSE
            Rts.GetArg (i, redirFile)
          END
        ELSIF (i # Rts.ArgNumber()) OR (arg[0] = "-") THEN
          Out.String ("Unknown option ");
          Out.String (arg);
          Out.Char (".");
          Out.Ln;
          RETURN
        ELSE  (* last option has to be the file name *)
          OMachine.redir := Redir.Read (redirFile);
          IF Redir.FindPath (OMachine.redir, arg, file) THEN
            IF ~printRCS & Strings2.Match ("*,v", file) THEN
              Redir.RCS2File (file, file)
            END;
            Out.String (file)
          ELSE
            Out.String ("Can't find file ");
            Out.String (arg);
            Out.String (".")
          END;
          Out.Ln
        END;
        INC (i)
      END
    END
  END Query;

BEGIN
  Query
END o2whereis.
