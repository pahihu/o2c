MODULE Redir;
(*
Syntax 'Redirection Table':
table = {pattern}.
pattern = wildcard {"," wildcard} "=" path {";" path} ["+" "RCS"].
wildcard = string.
path = string.
string = nonempty sequence of characters (without " ", "=", ";", "#", "+", 
         ",", or control codes) or
         '"' nonempty seq. of characters (without control codes) '"'.
whitespace are all characters with an ASCII code below 32/space.
comments are started with '#' and reach to the end of the line.

NOTE: `~name/' or `~/' is allowed as prefix of a path, it can also contain 
  `..' or `.'.  '+RCS' behind a path list will extend the search to RCS 
  subdirectories of the given paths.  I.e., whenever a file X isn't found at a 
  particular place xyz/, it is checked if a file xyz/RCS/X,v (resp. xyz/X,v) 
  exists.  If so, the search is aborted and xyz/RCS/X,v (resp. xyz/X,v) is 
  returned as the file's name.  This flag has only effect on the operation of 
  FindFile and FindFileExt.
*)


IMPORT 
  Dos, Files, Filenames, Rts, Str := Strings, Strings2, Out, Char := CharInfo,
  SYSTEM;

CONST 
  maxPatternLen* = 32;                   (* maximum size of a file name pattern *)
  maxPathLen* = 256;                     (* maximum size of a path string *)

TYPE
  Pattern* = POINTER TO PatternDesc;
  Path* = POINTER TO PathDesc;
  PatternDesc* = RECORD
    next-: Pattern;                      (* next file pattern description *)
    wildcard-: ARRAY maxPatternLen OF CHAR; (* wildcard expression *)
    rcs-: BOOLEAN;
    paths-: Path;                        (* list of paths associated with the pattern *)
  END; (* RECORD *)
  PathDesc* = RECORD
    next-: Path;                         (* next path *)
    path-: ARRAY maxPathLen OF CHAR;     (* path string *)
  END; (* RECORD *)



PROCEDURE Read* (fileName: ARRAY OF CHAR): Pattern;
(* Reads the redirection table in file 'fileName' and returns it's
   internal representation.
   Tilde escapes are expanded in 'fileName'.
   If no file 'fileName' exists an empty table is returned (but a warning 
   is written to stdout).  If an error occurs during the parsing of the file 
   the program will terminate with an (hopefully helpful) error message. *)
  VAR
    f: Files.File;                       (* redirection file *)
    r: Files.Rider;
    ch: CHAR;                            (* look ahead character in 'f' *)
    p: Pattern;
    fileFull: ARRAY maxPathLen OF CHAR;
    
  PROCEDURE ReadPattern(): Pattern;
  (* Reads a list a patterns from file 'f'; results to NIL if no
     further pattern description is found. *)
    VAR
      pattern: Pattern;
    
    PROCEDURE Error (msg: ARRAY OF CHAR);
    (* Writes error message, terminates program. *)
      BEGIN
        Out.String ("Error in ");
        Out.String (fileName);
        Out.String (" at/before position ");
        Out.Int (Files.Pos (r), 0);
        Out.Char (":");
        Out.Ln;
        Out.String (msg);
        Out.Ln;
        Rts.Terminate
      END Error;
      
    PROCEDURE SkipBlanks;
    (* Skip characters in 'f' until one with a code greater than 32/space
       or the end of the file is reached. 
       If a '#' is encountered, all characters til the next eof are skipped. *)
      BEGIN
        LOOP
          WHILE (ch <= " ") & ~r. eof DO
            Files.Read (r, ch)
          END;
          IF r. eof OR (ch # '#') THEN
            EXIT
          END;
          WHILE (ch # Char.eol) & ~r. eof DO
            Files.Read (r, ch)
          END
        END
      END SkipBlanks;
    
    PROCEDURE Match (test: CHAR);
    (* Tests, whether the current character matches 'test'.  If they match
       it skips the current character, otherwise it signals an error. *)
      VAR
        msg: ARRAY 16 OF CHAR;
      BEGIN
        SkipBlanks;
        IF (test # ch) THEN
          msg := "'X' expected.";  (* patch error message to contain 'test' *)
          msg[1] := test;
          Error (msg)
        END;
        Files.Read (r, ch)
      END Match;
      
    PROCEDURE ReadString (VAR str: ARRAY OF CHAR);
    (* Reads a string from file 'f'.  An error is signalled if the string 
       doesn't fit into 'str' or the string is not terminated. *)
      VAR
        len: INTEGER;
      BEGIN
        SkipBlanks;
        len := 0;
        IF ~r. eof THEN
          IF (ch = '"') THEN             (* parse string enclosed by '"' *)
            Files.Read (r, ch);          (* skip leading '"' *)
            WHILE (ch # '"') DO
              IF r. eof OR (ch < " ") THEN
                Error ("Unterminated string.")
              ELSE
                str[len] := ch;          (* copy character *)
                INC (len);
                IF (len = LEN (str)) THEN
                  Error ("String too long.")
                END
              END;
              Files.Read (r, ch)         (* read next character *)
            END;
            Files.Read (r, ch)           (* skip terminating delimiter *)
          ELSE  (* parse string until next forbidden character *)
            REPEAT
              str[len] := ch;            (* copy character *)
              INC (len);
              IF (len = LEN (str)) THEN
                Error ("String to long.")
              END;
              Files.Read (r, ch)         (* read next character *)
            UNTIL r. eof OR (ch <= " ") OR (ch = "=") OR (ch = ";") OR 
                            (ch = "#") OR (ch = "+") OR (ch = ",")
          END
        END;
        str[len] := 0X
      END ReadString;
      
    PROCEDURE ReadPath (): Path;
    (* Reads (non-empty) list of path strings from file 'f'. *)
      VAR
        path: Path;
        str: ARRAY 16 OF CHAR;
      BEGIN
        NEW (path);
        SkipBlanks;
        ReadString (path. path);
        Filenames.ExpandPath (path. path, path. path);
        SkipBlanks;
        IF (ch = ";") THEN
          Files.Read (r, ch);
          path. next := ReadPath()       (* recursively read next path *)
        ELSE
          path. next := NIL;
          IF (ch = "+") THEN             (* looks like '+RCS' is following *)
            Files.Read (r, ch);
            ReadString (str);
            IF (str = "RCS") THEN
              pattern. rcs := TRUE       (* set flag for extended RCS search *)
            ELSE                         (* no RCS or "RCS", we got an error *)
              Error ("'RCS' expected.")
            END
          END
        END;
        RETURN path
      END ReadPath;
    
    BEGIN
      SkipBlanks;
      IF (r. eof) THEN
        RETURN NIL
      ELSE
        NEW (pattern);
        ReadString (pattern. wildcard);
        IF (ch = ",") THEN               (* list of patterns *)
          Files.Read (r, ch);
          pattern. next := ReadPattern();
          pattern. rcs := pattern. next. rcs;
          pattern. paths := pattern. next. paths
        ELSE
          Match ("=");
          pattern. rcs := FALSE;
          pattern. paths := ReadPath();  (* read list of paths (recursively) *)
          pattern. next := ReadPattern() (* recursively read next pattern *)
        END;
        RETURN pattern
      END
    END ReadPattern;
    
  BEGIN
    Filenames.ExpandPath (fileFull, fileName);
    IF Dos.Exists(fileFull) THEN
      f := Files.Old (fileFull);
      IF f # NIL THEN
        Files.Set (r, f, 0);
        Files.Read (r, ch);
        p := ReadPattern();  (* read list of patterns (recursively) *)
        Files.Close (f);
        RETURN p
      ELSE
        Out.String ("Error: Can't open ");
        Out.String (fileFull);
        Out.String (", aborting.");
        Out.Ln;
        Rts.Terminate
      END
    ELSE
      Out.String ("Warning: Redirection table ");
      Out.String (fileFull);
      Out.String (" does not exist.");
      Out.Ln;
      RETURN NIL
    END
  END Read;

PROCEDURE Free* (VAR p: Pattern);
(* Deallocate memory of redirection table `p' and set `p' to NIL. *)

  PROCEDURE FreePaths (VAR path: Path);
  (* Free list of paths starting at `path'. *)
    BEGIN
      IF (path # NIL) THEN
        FreePaths (path. next);
        SYSTEM.DISPOSE (path)
      END
    END FreePaths;
  
  BEGIN
    IF (p # NIL) THEN
      IF (p. next # NIL) & (p. paths = p. next. paths) THEN
        (* multiple patterns with same path list *)
        p. paths := NIL
      ELSE
        FreePaths (p. paths)
      END;
      Free (p. next);
      SYSTEM.DISPOSE (p)
    END
  END Free;



PROCEDURE FindPath* (redir: Pattern; fileName: ARRAY OF CHAR;
                     VAR path: ARRAY OF CHAR): BOOLEAN;
(* Tries to locate a file 'fileName' in one of the paths stored in 'pattern'.
   pre: 'redir' is a redirection table initialized with 'Read' (or it is NIL).
     'fileName' (guess what) the name of a file.
   post: The redirection table is searched (from the beginning) for a pattern
     matching 'fileName'.  If one is found it tries to locate 'fileName' in one
     of the directories associated with that pattern, searching the path list 
     from left to right.
     Files are tested for existence in the following order
     1) dir/fileName
     2) dir/RCS/fileName,v (if +RCS set)
     3) dir/fileName,v (if +RCS set)
     On success the complete name of the file found is stored in 'path', 
     result is TRUE.
     If no file is found the search for another matching pattern continues.  
     Only if no more patterns are available the current directory is 
     considered (without looking for a RCS files). 
     If you want to store RCS files in the current directory, add '.' to the 
     path list. *)
  VAR
    pattern: Pattern;
    dir: Path;
   
  PROCEDURE MakeRCSName (rcsSubdir: BOOLEAN; VAR rcsFile: ARRAY OF CHAR);
    VAR
      path, name: ARRAY maxPathLen OF CHAR;
    BEGIN
      COPY (dir. path, path);
      IF rcsSubdir THEN
        Filenames.AddPath (path, path, "RCS")
      END;
      COPY (fileName, name);
      Str.Append (",v", name);
      Filenames.BuildFilename (rcsFile, path, name, "")
    END MakeRCSName;
    
  BEGIN
    pattern := redir;
    WHILE (pattern # NIL) DO             (* scan all pattern descriptions *)
      IF Strings2.Match (pattern. wildcard, fileName) THEN
        dir := pattern. paths;
        WHILE (dir # NIL) DO  (* scan paths associated with matching pattern *)
          Filenames.AddPath (path, dir. path, fileName);
          IF Dos.Exists (path) THEN      (* file exists *)
            RETURN TRUE
          ELSIF pattern. rcs THEN        (* extend search to RCS *)
            MakeRCSName (TRUE, path);
            IF Dos.Exists (path) THEN (* check whether dir/RCS/file,v exists *)
              RETURN TRUE
            ELSE
              MakeRCSName (FALSE, path);
              IF Dos.Exists (path) THEN  (* check whether dir/file,v exists *)
                RETURN TRUE
              END
            END
          END;
          dir := dir. next
        END
      END;
      pattern := pattern. next  (* no luck til now, try next pattern *)
    END;
    (* nothing found, last try is the current directory *)
    COPY (fileName, path);
    RETURN Dos.Exists (path) 
  END FindPath;
  
PROCEDURE GeneratePath* (redir: Pattern; fileName: ARRAY OF CHAR; VAR path: ARRAY OF CHAR);
(* Produces a file name (including the path information) for the file 
   'fileName'.
   pre: 'redir' is a redirection table initialized with 'Read' (or it is NIL).
     'fileName' (guess what) the name of a file.
   post: 'redir' is searched for the first pattern matching 'fileName'.  The 
     first directory associated with that pattern will be used as the file's 
     path. The complete name is then stored in 'path'.  If no pattern is found
     'path' will be set to the value in 'fileName'. *)
  VAR
    pattern: Pattern;
  BEGIN
    pattern := redir;
    WHILE (pattern # NIL) DO             (* scan all pattern descriptions *)
      IF Strings2.Match (pattern. wildcard, fileName) THEN
        Filenames.AddPath (path, pattern. paths. path, fileName);
        RETURN
      END;
      pattern := pattern. next
    END;
    COPY (fileName, path)
  END GeneratePath;



PROCEDURE FindPathExt* (redir: Pattern; name, ext: ARRAY OF CHAR;
                     VAR path: ARRAY OF CHAR): BOOLEAN;
(* Like FindPath, but 'fileName' is build like "<name>.<ext>". *)
  VAR
    fileName: ARRAY maxPathLen OF CHAR;
  BEGIN
    COPY (name, fileName);
    Strings2.AppendChar (".", name);
    Str.Append (ext, name);
    RETURN FindPath (redir, name, path)
  END FindPathExt;

PROCEDURE GeneratePathExt* (redir: Pattern; name, ext: ARRAY OF CHAR; VAR path: ARRAY OF CHAR);
(* Like GeneratePath, but 'fileName' is build like "<name>.<ext>". *)
  VAR
    fileName: ARRAY maxPathLen OF CHAR;
  BEGIN
    COPY (name, fileName);
    Strings2.AppendChar (".", name);
    Str.Append (ext, name);
    GeneratePath (redir, name, path)
  END GeneratePathExt;


PROCEDURE RCS2File* (rcsFile: ARRAY OF CHAR; VAR workFile: ARRAY OF CHAR);
(* pre: 'rcsFile' ends in 'RCS/xyz,v' or 'xyz,v' (xyz is a file name).
   post: 'workFile' contains 'rcsFile', stripped of 'RCS/' and ',v'.
   example: 'RCS/abc,v' -> 'abc'
            'dir/abc,v' -> 'dir/abc' *)            
  VAR
    path, name: ARRAY maxPathLen OF CHAR;
  BEGIN
    Filenames.GetPath (rcsFile, path, name);
    IF Strings2.Match ("*RCS/", path) THEN
      path[Str.Length (path)-4] := 0X
    END;
    IF Strings2.Match ("*,v", name) THEN
      name[Str.Length (name)-2] := 0X
    END;
    Filenames.BuildFilename (workFile, path, name, "")
  END RCS2File;
  
(*
PROCEDURE Write* (redir: Pattern);
(* Prints redirection table.  Mostly for testing. *)
  VAR
    path: Path;
  BEGIN
    IF (redir # NIL) THEN
      Out.String (redir. wildcard);
      Out.String (" = ");
      Out.Ln;
      path := redir. paths;
      REPEAT
        Out.String ("    ");
        Out.String (path. path);
        IF (path. next # NIL) THEN
          Out.Char (";")
        END;
        Out.Ln;
        path := path. next
      UNTIL (path = NIL);
      Write (redir. next)
    END
  END Write;
*)
END Redir.
