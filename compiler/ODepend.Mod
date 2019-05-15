MODULE ODepend;  (* Author: Michael van Acken *)
(* 	$Id: ODepend.Mod,v 1.16 1995/05/19 17:23:39 oberon1 Exp $	 *)

IMPORT
  Out, Time, Dos, Redir, Str := Strings, Strings2,
  M := OMachine, S := OScan;



CONST
  numFiles = 5;                          (* maximum number of files associated with a given module, like *)
                                         (* .Mod, .OSym, .h, .c or .o *)
                                         
TYPE
  Module* = POINTER TO ModuleDesc;
  Import* = POINTER TO ImportDesc;
  ModuleDesc* = RECORD
    next* : Module;                      (* next module in list *)
    name* : ARRAY 2*M.maxSizeIdent+2 OF CHAR; (* module name (_not_ its file name) *)
                                         (* the maxSizeIdent+2 additional characters are used in OMakeGCC *)
    extName* : ARRAY Redir.maxPathLen OF CHAR; (* external library name *)
    extClass* : SHORTINT;                (* external classification, initialized to 0, back end dependend use *)
    import* : Import;                    (* list of references to imported modules *)
    flags* : SET;                        (* set of the below defined flXXX constants *)
    file* : ARRAY numFiles, Redir.maxPathLen OF CHAR; (* file names associated to the flags flXXX down below *)
    time* : ARRAY numFiles OF Time.Time; (* time stamps of the files *)
    count : INTEGER;                     (* reference count, used for topological sorting *)
  END;
  ImportDesc* = RECORD                   
    next* : Import;                      (* next import *)
    module* : Module;                    (* pointer to imported module *)
  END;
  
CONST
  (* values for ModuleDesc.flags *)
  flModExists* = 0;                      (* set iff module sources exist *)
  flSymExists* = 1;                      (* set iff symbol file exists *)
  flBE0Exists* = 2;                      (* set iff the corresponding back end file exists *)
  flBE1Exists* = 3;                      (* set iff the corresponding back end file exists *)
  flBE2Exists* = 4;                      (* set iff the corresponding back end file exists *)
  
  flSymChanged* = 5;                     (* set iff a the symbol file has changed with the last compilation *)
  flExternal* = 6;                       (* set iff module defines an EXTERNAL *)
  
  flScanError = 7;                       (* set iff an error occured during parsing of that module *)
  flNoSources* = 8;                      (* set iff no sources corresponding to the module name can be found *)
  flScanned = 9;                         (* set iff the module's import list has been scanned *)
  flCompile* = 10;                       (* set iff the module's source are/will be translated *)
  flFirstFree* = 11;                     (* marks first of unused flags *)


PROCEDURE FindFilename* (mod : Module; index : SHORTINT; name : ARRAY OF CHAR) : BOOLEAN;
(* pre: 'mod' is a module entry created by the procedure Dependencies,
     'index' a value from 0 to numFiles-1, 'name' a not empty string.
   post: If a file 'name' can be found with the help of the redirection table 
     in 'redir', the full path name is stored in mod.file[index],
     the files time stamp stored in mod.time[index] and the flags 'index' set
     in mod.flags. Result is TRUE.
     Otherwise mod.time[index] is set to zero, mod.file[index] is undefined,
     and result is FALSE. *)
  VAR
    err, found : BOOLEAN;
  BEGIN
    found := Redir.FindPath (M.redir, name, mod. file[index]);
    IF found THEN
      INCL (mod. flags, index);
      Dos.GetDate (mod. file[index], mod. time[index], err)
    ELSE
      Time.Reset (mod. time[index])
    END;
    RETURN found
  END FindFilename;

PROCEDURE FindFile* (mod : Module; index : SHORTINT; ext : ARRAY OF CHAR) : BOOLEAN;
(* pre: 'mod' is a module entry created by the procedure Dependencies,
     'index' a value from 0 to numFiles-1, 'ext' a not empty string.
   post: If a file "<module name>.<ext>" can be found with the help of
     the redirection table in 'redir', the full path name is stored in 
     mod.file[index], the files time stamp stored in mod.time[index] and the 
     flags 'index' set in mod.flags.  Result is TRUE.
     Otherwise mod.time[index] is set to zero, mod.file[index] is undefined,
     and result is FALSE. *)
  VAR
    fname : ARRAY Redir.maxPathLen OF CHAR;
  BEGIN
    COPY (mod. name, fname);
    Strings2.AppendChar (".", fname);
    Str.Append (ext, fname);
    RETURN FindFilename (mod, index, fname)
  END FindFile;

PROCEDURE NewFile* (mod : Module; index : SHORTINT; ext : ARRAY OF CHAR);
(* pre: 'mod' is a module entry created by the procedure Dependencies,
     'index' a value from 1 to numFiles-1, 'ext' a not empty string.
   post: With the help of the redirection table in 'redir' the full name
     of the file "<module name>.<ext>" is created and stored in 
     mod.file[index]. mod.time[index] is set to the current time. *)
  BEGIN
    Redir.GeneratePathExt (M.redir, mod. name, ext, mod. file[index]);
    Time.GetSysTime (mod. time[index])
  END NewFile;



PROCEDURE TopSort* (modules : Module; mainFirst : BOOLEAN; VAR errName : ARRAY OF CHAR) : Module;
(* Does a topological sort on the list of modules.
   pre: 'modules' is a list of modules, each of it contains a list of all its 
     imported modules in 'module.import'.
   post: Result is a permutation of the list 'modules' in such a way, that
     each module precedes all other modules in the list that are importing it. 
     'mainFirst' determines if the main module should be at the head of the 
     list (TRUE) or whether the list should be reversed (FALSE).
     If the sort failed, e.g. at least one cyclic import occured, the result 
     is NIL.  'errName' will contain the name of a module in this cycle. *) 
  VAR
    mod : Module;
    inode : Import;
    topList : Module;
  
  PROCEDURE RemoveBest (VAR mod : Module; prevBest : INTEGER) : Module;
  (* pre: 'mod' contains the unsearched part of the module list, 'prevBest' 
       the minimum import count encountered in the list til node 'mod'
     post: If the list starting at 'mod' contains a module whose module count 
       is below 'prevBest', this module is removed from the list and returned 
       as the result.  Otherwise NIL is returned and the list is not modified. *)
    VAR
      best : Module; 
    BEGIN
      IF (mod = NIL) THEN
        RETURN NIL
      ELSIF (mod. count < prevBest) THEN
        best := RemoveBest (modules. next, mod. count);
        IF (best = NIL) THEN
          best := mod;
          mod := mod. next
        END;
        RETURN best
      ELSE
        RETURN RemoveBest (mod. next, prevBest)
      END
    END RemoveBest;
      
  BEGIN
    (* initialize import counter for each module *)
    mod := modules;
    WHILE (mod # NIL) DO
      mod. count := 0;
      inode := mod. import;
      WHILE (inode # NIL) DO
        INC (mod. count);
        inode := inode. next
      END;
      mod := mod. next
    END;
    (* rearrange list in 'modules', store result in topList *)
    topList := NIL;
    WHILE (modules # NIL) DO
      mod := RemoveBest (modules, MAX(INTEGER));
      mod. next := topList;
      topList := mod;
      IF (topList. count # 0) THEN  (* oops, cyclic import *)
        COPY (topList. name, errName);
        RETURN NIL
      END;
      (* update import count in remaining modules *)
      mod := modules;
      WHILE (mod # NIL) DO
        inode := mod. import;
        WHILE (inode # NIL) DO
          IF (inode. module = topList) THEN
            DEC (mod. count)
          END;
          inode := inode. next
        END;
        mod := mod. next
      END
    END;
    
    IF mainFirst THEN
      RETURN topList
    ELSE
      (* revert topList to move the module with the least imports to the start *)
      WHILE (topList # NIL) DO
        mod := topList;
        topList := topList. next;
        mod. next := modules;
        modules := mod
      END;
      RETURN modules
    END
  END TopSort;
    

PROCEDURE Dependencies* (VAR modName : ARRAY OF CHAR; VAR err : BOOLEAN) : Module;
(* pre: 'modName' is the name of a "main" module (no suffix like .Mod).
   post: An error message is printed if no file "modName.Mod" can be found.
     Otherwise all imports of the module and all of their imports in
     turn are determined by scanning the import lists of the modules. 
     An information block of type 'ModuleDesc' is created for each module
     encountered in this search, the list of its imports stored in the field
     'import' (internal modules like SYSTEM are ignored).
     If a module's file can't be found, 'flNoSource' is set. If the file
     exists, 'flModExists' is set, the full path name stored in 'file[0]' and
     'time[0]' set to the file time stamp.
     An error during the scan a file is marked by 'flScanError'. 
     The result is a list of the information blocks sorted in such a way that
     all modules importing a given module succeed it in the list. 
     'err' is set to TRUE if no main module can be found, parsing of a module 
     failed or a cyclic import was detected. *)
  VAR
    modules, mod : Module;
    external : BOOLEAN;
    errFile : ARRAY 2*M.maxSizeIdent+2 OF CHAR;
    
  PROCEDURE Error (msg, file : ARRAY OF CHAR);
    BEGIN
      Out.String ("Error: ");
      Out.String (msg);
      Out.String (" in ");
      Out.String (file);
      Out.Ln;
      err := TRUE
    END Error;
  
  PROCEDURE AddModule (name : ARRAY OF CHAR) : Module;
  (* Retrieves or creates a module descriptor.
     pre: 'name' is a modules identifier.
     post: The information node associated with 'name' is the result.
     side: If no module 'name' is recorded in the list 'modules', a new node is
       created, initialized accordingly and added to 'modules'. *)
    VAR
      mod : Module;
    BEGIN 
      (* find module information block associated with 'name'. *)
      mod := modules;
      WHILE (mod # NIL) & (mod. name # name) DO
        mod := mod. next
      END;
      IF (mod = NIL) THEN  (* no module 'name' known, create new block *)
        IF (modules = NIL) THEN
          (* set main module name to the one given in the source text *)     
          COPY (name, modName)  
        END;
        NEW (mod);
        mod. next := modules;
        COPY (name, mod. name);
        mod. extName := "";
        mod. extClass := 0;
        mod. import := NIL;
        mod. flags := {};
        modules := mod
      END;
      RETURN mod
    END AddModule;
        
  PROCEDURE ScanModule (mod : Module);
  (* Parses the import list of a given module.
      pre: 'mod' es an initialized module descriptor, 'flScanned' is not set 
        in 'mod.flags'.
      post: If the sources for module 'mod.name' can be found and the import 
        list could be parsed without any errors, the list 'mod.import' 
        contains all modules (except SYSTEM) that 'mod' imports.  If the 
        sources could not be found then 'flNoSources' is set in 'mod.flags', 
        any parsing error will set 'flScanError' in 'mod.flags'. 'flScanned' 
        is set unconditional. 
        Note: Missing source files will not cause an error message.
      side: If an imported module is not contained in 'modules', its module 
        descriptor is added to the list. *)
    VAR
      importName : ARRAY M.maxSizeIdent OF CHAR; 
      inode : Import;
      found, ferr : BOOLEAN;
      
    PROCEDURE CheckSym (s : SHORTINT);
      VAR
        errNum : INTEGER;
      BEGIN
        IF (s # S.sym) THEN
          IF (s = S.ident) THEN
            errNum := 100
          ELSIF (s = S.module) THEN
            errNum := 101
          ELSE
            errNum := 100+LONG(s)
          END;
          S.Err (-1, errNum);
          INCL (mod. flags, flScanError);
          err := TRUE
        END;
        S.GetSym
      END CheckSym;
      
    BEGIN
      (* put filename together and initialize scanner on the file *)
      found := FindFile (mod, flModExists, M.moduleExtension);
      IF found THEN
        S.Init (mod. file[0], S.minBufferSize + S.minBufferSize DIV 2, ferr)
      END;
      IF ~found OR ferr THEN
        INCL (mod. flags, flNoSources)
      ELSE  (* init succeeded, now we try to parse the import list *)
        (* set name of work file (may differ with RCS) *)
        COPY (S.sourceName, mod. file[0]); 
        S.GetSym;
        external := (S.sym = S.ident) & (S.ref = "EXTERNAL");
        IF external THEN
          INCL (mod. flags, flExternal);
          S.GetSym
        END;
        CheckSym (S.module);
        IF (S.sym = S.ident) THEN
          (* replace module name with name of source file *)
          COPY (S.ref, mod. name)  
        END;
        CheckSym (S.ident);
        (* check for external declaration *)
        IF M.allowExternal & (S.sym = S.lBrak) THEN
          INCL (mod. flags, flExternal);
          S.GetSym;
          CheckSym (S.string);
          CheckSym (S.rBrak);
          IF (S.sym # S.ident) OR (S.ref # "EXTERNAL") THEN
            S.Err (-1, 102);
            INCL (mod. flags, flScanError);
            err := TRUE
          END;
          CheckSym (S.ident);
          CheckSym (S.lBrak);
          COPY (S.ref, mod. extName);
          CheckSym (S.string);
          CheckSym (S.rBrak)
        ELSE
          mod. extName := ""
        END;        
        CheckSym (S.semicolon);
        IF (S.sym = S.import) THEN  (* file actually has an import list? *)
          REPEAT
            S.GetSym;
            COPY (S.ref, importName);
            CheckSym (S.ident);
            IF (S.sym = S.becomes) THEN  (* module is declared with an alias? *)
              S.GetSym;
              COPY (S.ref, importName);
              CheckSym (S.ident)
            END;
            IF S.noerr & ~(flScanError IN mod. flags) THEN
              IF (importName # "SYSTEM") THEN  (* we don't need internal modules *)
                NEW (inode);
                inode. next := mod. import;
                inode. module :=  AddModule (importName);
                mod. import := inode
              END
            ELSE  (* error occured, we have lost *)
              INCL (mod. flags, flScanError)  (* make sure that the error is recorded *)
            END
          UNTIL (flScanError IN mod. flags) OR (S.sym # S.comma);
          CheckSym (S.semicolon)
        END;
        S.Close
      END;
      INCL (mod. flags, flScanned)
    END ScanModule;
  
  BEGIN
    err := FALSE;
    modules := NIL; 
    (* read all imported modules (and their import lists) *)
    ScanModule (AddModule (modName));
    IF (flNoSources IN modules. flags) THEN
      Out.String ("Error: Can't find main module ");
      Out.String (modName);
      Out.String (".Mod");
      Out.Ln;
      err := TRUE
    ELSE
      LOOP
        mod := modules;
        (* find first not scanned module *)
        WHILE (mod # NIL) & (flScanned IN mod. flags) DO
          mod := mod. next
        END;
        IF (mod = NIL) OR err THEN  (* all modules scanned *)
          EXIT
        ELSE  (* read mod's import list *)
          ScanModule (mod)
        END
      END;
      IF ~err THEN
        (* run a topological sort on 'modules', i.e. place modules 
           with no imports at the beginning of the list, the 'main 
           module' at the end *)
        modules := TopSort (modules, FALSE, errFile);
        IF (modules = NIL) THEN
          Error ("Cyclic import", errFile)
        END
      END
    END;
    RETURN modules
  END Dependencies;

END ODepend.
