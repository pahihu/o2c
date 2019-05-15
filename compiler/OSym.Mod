MODULE OSym;
(*
    Title: 	OSym
    Created:	Fri Feb  3 14:03:37 1995
    Author: 	Juergen Zimmermann
		<oberon2@chablis.informatik.uni-kl.de>
    Project:    Oberon-2 Compiler Front End, Oberon Implementation
    Module:     SymbolTable: Import, Export
    Contents:   Import of symbolfiles, Export of symbolfiles, Universal symbolfiles
    $Id
*)

IMPORT
  SYSTEM,
  Redir,  Files,  Out,  Rts,  Strings,
  OM:=OMachine,  OS:=OScan,  OT:=OTable,  OD:=ODepend;

CONST
  fullSym = TRUE; (* write even not exported fields with full name into the symbolfile*)
  
  SymbolFileVersion = 4;
  SymbolFileBegin = 256*256*256*ORD("B") + 256*256*ORD("S") + 256*ORD("y") + ORD("m") + SymbolFileVersion;
  SymbolFileEnd   = 256*256*256*ORD("E") + 256*256*ORD("S") + 256*ORD("y") + ORD("m") + SymbolFileVersion;

TYPE
  FileName = ARRAY 256 OF CHAR;

CONST
  notinlist   = -1;
  defaultStructSize = 128;
  maxExpStructSize = 128 * defaultStructSize;
  
  defaultModuleSize = 16;

TYPE
  StructItemList = POINTER TO ARRAY OF OT.Struct;
  StructList =
    RECORD
      lastnum: LONGINT; (* number of the last valid position *)
      items: StructItemList;
    END; (* RECORD *)

TYPE
  ModuleItemList = POINTER TO ARRAY OF OT.Object;
  ModuleList =
    RECORD
      lastnum: LONGINT; (* number of the last valid position *)
      items: ModuleItemList;
    END; (* RECORD *)


(*****************************************************************************)
(*****************************************************************************)
(*
  Common routines for "StructList"
   - Insertion
   - Lookup the number of a certain "Struct"
   - Lookup the "Struct" for a given number
*)
(*****************************************************************************)
(*****************************************************************************)

PROCEDURE InsertStruct(VAR list: StructList; item: OT.Struct);
  VAR
    newlist: StructItemList;
    index: LONGINT;
    newlen: LONGINT;
    oldlen: LONGINT;
  BEGIN
    INC(list.lastnum);
    oldlen:=LEN(list.items^);
    IF (list.lastnum >= oldlen) THEN
      (* reallocate list *)
      IF (oldlen >= maxExpStructSize) THEN (* stop the exponential growth *)
        newlen:=oldlen + maxExpStructSize;
      ELSE
        newlen:=oldlen * 2;
      END; 
      NEW(newlist,newlen);
      FOR index:=0 TO (oldlen-1) DO
        newlist^[index]:=list.items^[index];
      END;
      SYSTEM.DISPOSE(list.items);
      list.items:=newlist;
    END;
    list.items^[list.lastnum]:=item;
  END InsertStruct;

PROCEDURE InsertPredefinedStructs(VAR structlist: StructList);
  VAR
    str: LONGINT;
  BEGIN
    FOR str:=OT.strUndef TO OT.strNil DO
      InsertStruct(structlist,OT.predeclType[str]);
    END; (* FOR *)
  END InsertPredefinedStructs;

PROCEDURE InitStructList(VAR list: StructList);
(* Initialize "structlist" by inserting the predeclared 
   Structures to the list *)

  (* inserts the global predefined structures in the list of
     the local imported structures, so we can reference them
     directly by their "compiler"-number, we don't have to write
     them into the symbol-file!
     Also have a good look at "OTable.InitPredefined"! *)
  BEGIN
    NEW(list.items,defaultStructSize);
    list.lastnum:=-1;
  END InitStructList;
  
PROCEDURE FreeStructList(VAR list: StructList);
  BEGIN
    SYSTEM.DISPOSE(list.items);
    list.lastnum:=-1;
  END FreeStructList;

PROCEDURE GetStruct(list: StructList; num: LONGINT): OT.Struct;
  BEGIN
    IF (num > list.lastnum) THEN
      RETURN(NIL);
    ELSE
      RETURN(list.items^[num]);
    END;
  END GetStruct;

PROCEDURE GetStructNum(list: StructList;item: OT.Struct): LONGINT;
  VAR
    i: LONGINT;
  BEGIN
    i:=0;
    WHILE (i < list.lastnum) & (list.items^[i] # item) DO
      INC(i);
    END;
    IF (i <= list.lastnum) & (list.items^[i] = item) THEN
      RETURN(i);
    ELSE
      RETURN(notinlist);
    END; (* IF *)
  END GetStructNum;

(*****************************************************************************)
(*****************************************************************************)
(*
  Common Routines for "ModuleList"
   - Insertion 
   - Lookup the number of a given "Module"
   - Lookup the "Module" for a given number
*)
(*****************************************************************************)
(*****************************************************************************)

PROCEDURE InitModuleList(VAR list: ModuleList);
  BEGIN
    NEW(list.items,defaultModuleSize);
    list.lastnum:=-1;
  END InitModuleList;

PROCEDURE FreeModuleList(VAR list: ModuleList);
  BEGIN
    SYSTEM.DISPOSE(list.items);
    list.lastnum:=-1;
  END FreeModuleList;

PROCEDURE InsertModule(VAR list: ModuleList; item: OT.Object);
  VAR
    newlist: ModuleItemList;
    index: LONGINT;
    newlen: LONGINT;
    oldlen: LONGINT;
  BEGIN
    INC(list.lastnum);
    oldlen:=LEN(list.items^);
    IF (list.lastnum >= oldlen) THEN
      (* reallocate list *)
      newlen:=oldlen + defaultModuleSize;
      NEW(newlist,newlen);

      FOR index:=0 TO (oldlen-1) DO
        newlist^[index]:=list.items^[index];
      END;
      SYSTEM.DISPOSE(list.items);
      list.items:=newlist;
    END;
    list.items^[list.lastnum]:=item;
  END InsertModule;

PROCEDURE GetModule(list: ModuleList; num: LONGINT): OT.Object;
  BEGIN
    IF (num > list.lastnum) THEN
      RETURN(NIL);
    ELSE
      RETURN(list.items^[num]);
    END;
  END GetModule;

PROCEDURE GetModuleNum(list: ModuleList; item: OT.Object): LONGINT;
  VAR
    i: LONGINT;
  BEGIN
    i:=0;
    WHILE (i < list.lastnum) & (list.items^[i] # item) DO
      INC(i);
    END;
    IF (i <= list.lastnum) & (list.items^[i] = item) THEN
      RETURN(i);
    ELSE
      RETURN(notinlist);
    END;
  END GetModuleNum;

PROCEDURE GetLastModNum(list: ModuleList): LONGINT;
  BEGIN
    RETURN(list.lastnum);
  END GetLastModNum;


(*****************************************************************************)
(*****************************************************************************)
(*****************************************************************************)
(*****************************************************************************)


PROCEDURE Export*(module: OD.Module; force: BOOLEAN);
(* pre: Symbol file for the module named "module^.name" exists
        if "OD.flSymExists IN module^.flags".
          In this case it's name is stored in "module^.file[OD.flSymExists]".
   post: The path of the current (newly generated or old) symbol file
         for "module^.name" is stored in "module^.file[OD.flSymExists]",
         "OD.flSymExists" is set in "module^.flags". *)

  VAR
    structBuffer: StructList;
    moduleBuffer: ModuleList;
    exportFile: Files.File;      (* this file will be written *)
    exportRider: Files.Rider;
    symbolfileKey: LONGINT;
    tmpFileName: FileName;
    isExternal: BOOLEAN; 
    
  PROCEDURE GenerateFingerprint(VAR key: ARRAY OF SYSTEM.BYTE);
    CONST
      fingerprintSize = SIZE(LONGINT);
      blockSize = 1024;
    VAR
      i, last, read, size: LONGINT;
      tmpRider: Files.Rider;
      fingerprint:
        RECORD
          fp : ARRAY fingerprintSize OF SYSTEM.BYTE;
          i,j: INTEGER;
        END; (* RECORD *)
      block: ARRAY blockSize OF SYSTEM.BYTE;

    PROCEDURE NextFingerprint(ch: SYSTEM.BYTE);
  (* Structured Programming(1993) 14: 136-147 *)
      BEGIN
        (* looks really ugly with all those 'casts'... *)
        fingerprint.fp[fingerprint.i]:=CHR((ORD(SYSTEM.VAL(CHAR,fingerprint.fp[fingerprint.i])) +
            ORD(SYSTEM.VAL(CHAR,SYSTEM.ROT(ch,fingerprint.j)))) MOD 256);
        INC(fingerprint.i); 
        IF (fingerprint.i = fingerprintSize) THEN
          fingerprint.j:=(fingerprint.j+1) MOD 8;
          fingerprint.i:=0;
        END; (* IF *)
      END NextFingerprint;
  
    BEGIN
      (* initialize fingerprint *)
      fingerprint.i:=0; fingerprint.j:=0;
      FOR i:=0 TO (fingerprintSize - 1) DO
        fingerprint.fp[i]:=0;
      END; (* FOR *)

      last:=Files.Pos(exportRider);
      Files.Set(tmpRider,exportFile,0);
      size:=blockSize;
      LOOP
        IF (last < blockSize) THEN
          size:=last;
        END; 
        Files.ReadBytes(tmpRider,block,size); 
        read:=size - tmpRider.res; (* if no error occured, then 'read=size' should be true *)
        DEC(last,read);
        FOR i:=0 TO (read-1) DO
          NextFingerprint(block[i]);
        END;
        
        IF (read # size) & (last # 0) THEN (* an error occured during reading *)
          Out.String("[OSym] Failed to generate right fingerprint for symbolfile ");
          Out.String(module^.name); Out.Char("."); Out.String(OM.symbolExtension); Out.Ln;
          EXIT;
        END;
        IF last = 0 THEN EXIT; END;
      END;
      (* get the fingerprint *)
      FOR i:=0 TO (SIZE(LONGINT)-1) DO 
        key[i]:=fingerprint.fp[i];
      END; (* FOR *)
    END GenerateFingerprint;

  PROCEDURE WriteBoolFlag(set: SET; flag: LONGINT);
    BEGIN
      Files.WriteBool(exportRider,flag IN set);
    END WriteBoolFlag;

  PROCEDURE WriteIdent(ident: ARRAY OF CHAR);
  (* Identifiers will be delimited by a character >= 128,
     they MUST be at least one character long *)
    VAR
      i: INTEGER;
    BEGIN
      i:=Strings.Length(ident)-1;
      IF i > 0 THEN
        Files.WriteBytes(exportRider,ident,i);
      END;
      Files.Write(exportRider,CHR(ORD(ident[i]) + 128));
    END WriteIdent;

  PROCEDURE WriteModule(module: OT.Object);
    VAR
      num: LONGINT;
      mod,lastimport: OT.Object;
    BEGIN
      num:=GetModuleNum(moduleBuffer,module);
      IF (num = notinlist) THEN (* Module was not yet exported *)
        InsertModule(moduleBuffer,module); (* insert in the list *)
        Files.WriteNum(exportRider,OT.objModule);
        WriteIdent(module^.name);
        (* this is the symbolfile's key *)
        Files.WriteLInt(exportRider,module^.const^.intval); 
        IF (module^.link # OT.compiledModule^.link) THEN
          (* look, if the module is really in the importlist *)
          mod:=OT.compiledModule^.link^.next; lastimport:=NIL;
          (* scan importlist *)
          WHILE (mod # NIL) & (mod^.link # module^.link) DO 
            lastimport:=mod;
            mod:=mod^.next;
          END; (* WHILE *)
          IF (mod = NIL) THEN 
            (* module is not directly imported, so insert (temporary) 
               module in the importlist *)
            mod:=OT.NewObject(module^.name,OT.objModule,OT.noPos);
            mod^.name[0]:=CHR(ORD(mod^.name[0]) + 128); (* make name illegal *)
            mod^.link:=module^.link;
(*
            the old, clean version, which needs too much to be 
            done when the symboltable is closed again...

            (* open the symboltable once again *)
            OT.OpenScope(OT.compiledModule^.link);
            OT.Insert(mod); (* insert the module *)
            OT.CloseScope(TRUE); (* and close the symboltable *)
*)
            lastimport^.next:=mod;
         END; (* IF *)
          INCL(mod^.flags,OT.flagExport);
        END; (* IF *)
      ELSE
        Files.WriteNum(exportRider,-num);
      END; (* IF *)
    END WriteModule;

  PROCEDURE ^WriteType(obj: OT.Object);

  PROCEDURE WriteStruct(type: OT.Struct; decl: BOOLEAN);
    VAR
      ref: LONGINT;

    PROCEDURE WriteFP(parlist: OT.Object; nofArgs: LONGINT);
      VAR
        par: OT.Object;
      BEGIN
        par:=parlist;
        Files.WriteNum(exportRider,nofArgs);
        WHILE (nofArgs > 0) DO
          Files.WriteNum(exportRider,par^.mode);
          WriteIdent(par^.name); WriteStruct(par^.type,TRUE);
          DEC(nofArgs);
          par:=par^.link;
        END; (* WHILE *)
      END WriteFP;

    PROCEDURE WriteFields(field: OT.Object);
      VAR
        walk: OT.Object;

      PROCEDURE WriteMethods(meth: OT.Object);
        BEGIN
          IF (meth # NIL) THEN 
            WriteMethods(meth^.left);
            IF (meth^.mode = OT.objTBProc) (* & (meth^.mark > OT.exportNot) *) THEN
              INCL(meth^.flags,OT.flagExport);
              Files.WriteNum(exportRider,meth^.mode);
              WriteIdent(meth^.name);
              Files.WriteNum(exportRider,meth^.mark);
              WriteStruct(meth^.type,FALSE); (* Methods cannot be aliased *)
            END; (* IF *)
            WriteMethods(meth^.right);
          END; (* IF *)
        END WriteMethods;

      BEGIN
        walk:=field;
        WHILE (walk # NIL) & (walk^.mode = OT.objField) DO
          Files.WriteNum(exportRider,OT.objField);
          IF fullSym OR (walk^.mark > OT.exportNot) THEN
            WriteIdent(walk^.name);
            Files.WriteNum(exportRider,walk^.mark);
          ELSE
            WriteIdent(OT.noIdent);
          END; (* IF *)
          INCL(walk^.flags,OT.flagExport);
          WriteStruct(walk^.type,TRUE);
          walk:=walk^.next;
        END; (* WHILE *)
        WriteMethods(field);
        Files.WriteNum(exportRider,0);
      END WriteFields;

    BEGIN
      IF (type = NIL) THEN
        Files.WriteNum(exportRider,0); (* ref *)
      ELSE
        ref:=GetStructNum(structBuffer,type);
        IF (ref = notinlist) THEN (* first export of this struct *)
          Files.WriteNum(exportRider,1); (* ref *)
          IF decl & (type^.obj # NIL) THEN 
            (* write declaration for this struct *)
            Files.WriteNum(exportRider,1);
            WriteType(type^.obj);
            RETURN; 
            (* We wrote a type object, so we need not write anything more *)
          ELSE
            Files.WriteNum(exportRider,0);
          END; (* IF *)
          INCL(type^.flags,OT.flagExport);
          InsertStruct(structBuffer,type);

          IF (type^.obj = NIL) OR (type^.obj^.mnolev >= OT.compileMnolev) THEN
            (* only export types that we created in the currently 
               compiled module *)
            Files.WriteNum(exportRider,type^.form);
            WriteBoolFlag(type^.flags,OT.flagExternal);
            CASE type^.form OF
              |OT.strProc,
               OT.strTBProc:
               WriteFP(type^.link,type^.len);
              |OT.strArray:
               Files.WriteNum(exportRider,type^.len);
              |OT.strDynArray:
              |OT.strRecord:
                IF isExternal THEN
                  WriteBoolFlag(type^.flags,OT.flagUnion);
                END; (* IF *) 
                WriteFields(type^.link);
              ELSE (* there's nothing more to write for other structs... *)
            END; (* CASE *)
            WriteStruct(type^.base,TRUE);
          END; (* IF *)
        ELSE (* ref >= 0, already referenced type *)
          Files.WriteNum(exportRider,-ref);
        END; (* IF *)
      END; (* IF *)
    END WriteStruct;

  PROCEDURE WriteConst(const: OT.Object);
    BEGIN
      Files.WriteNum(exportRider,OT.objConst);
      WriteIdent(const^.name);
      WriteStruct(const^.type,TRUE);
      IF (const^.type^.form = OT.strString) THEN
        Files.WriteString(exportRider,const^.const^.string^);
      ELSIF (const^.type^.form IN {OT.strBool,OT.strChar,OT.strShortInt,OT.strInteger,OT.strLongInt}) THEN
        Files.WriteNum(exportRider,const^.const^.intval);
      ELSIF (const^.type^.form IN {OT.strReal,OT.strLongReal}) THEN
        Files.WriteLReal(exportRider,const^.const^.real);
      ELSIF (const^.type^.form = OT.strSet) THEN
        Files.WriteSet(exportRider,const^.const^.set);
      ELSIF (const^.type^.form = OT.strNil) THEN
        (* what should we write for NIL? *)
      ELSE
        Out.String("ERROR: Writing of unknown constant aborted. StructNo.:"); Out.Int(const^.type^.form,0); Out.Ln;
      END; (* IF *)
    END WriteConst;

  PROCEDURE WriteExternalName(obj: OT.Object);
    BEGIN
      IF isExternal THEN
        Files.WriteBool(exportRider,obj^.extName # NIL);
        IF obj^.extName # NIL THEN
          Files.WriteString(exportRider,obj^.extName^);
        END; (* IF *)
      END; (* IF *)
    END WriteExternalName;
  
  PROCEDURE WriteVar(var: OT.Object);
    BEGIN
      Files.WriteNum(exportRider,OT.objVar);
      WriteIdent(var^.name);
      Files.WriteNum(exportRider,var^.mark);
      WriteExternalName(var);
      WriteStruct(var^.type,TRUE);
    END WriteVar;

  PROCEDURE WriteProc(proc: OT.Object);
    BEGIN
      Files.WriteNum(exportRider,proc^.mode);
      WriteIdent(proc^.name);
      WriteExternalName(proc);
      WriteStruct(proc^.type,FALSE); (* Procedure-Structs cannot be aliased *)
    END WriteProc;

  PROCEDURE WriteType(obj: OT.Object);
    BEGIN
      Files.WriteNum(exportRider,OT.objType);
      WriteIdent(obj^.name);
      Files.WriteNum(exportRider,obj^.mark);
      WriteModule(OT.GetModuleForObject(obj));
      INCL(obj^.flags,OT.flagExport);
      IF (obj^.mnolev >= OT.compileMnolev) THEN
        WriteStruct(obj^.type,(obj # obj^.type^.obj));
      END; (* IF *)
    END WriteType;

  PROCEDURE WriteConstTypeVarProc(obj: OT.Object);
    BEGIN
      IF (obj # NIL) THEN
        WriteConstTypeVarProc(obj^.left);
        IF (obj^.mark # OT.exportNot) THEN
          IF (obj^.mode = OT.objConst) THEN
            WriteConst(obj);
          ELSIF (obj^.mode = OT.objVar) THEN
            WriteVar(obj);
          ELSIF (obj^.mode IN {OT.objExtProc,OT.objIntProc}) THEN
            WriteProc(obj);
          ELSIF (obj^.mode = OT.objType) & ~(OT.flagExport IN obj^.flags) THEN
            (* write type only, if not already exported *)
            WriteType(obj);
          END; (* IF *)
          INCL(obj^.flags,OT.flagExport);
        END; (* IF *)
        WriteConstTypeVarProc(obj^.right);
      END; (* IF *)
    END WriteConstTypeVarProc;

  PROCEDURE WriteHeader;
    BEGIN
      Files.WriteLInt(exportRider,SymbolFileBegin);
      WriteModule(OT.compiledModule);
      WriteBoolFlag(OT.compiledModule^.flags,OT.flagExternal);
      isExternal:=(OT.flagExternal IN OT.compiledModule^.flags);
      IF isExternal THEN
        Files.WriteString(exportRider,OT.compiledModule^.const^.string^);
        Files.WriteString(exportRider,OT.compiledModule^.extName^);
      END; (* IF *)
    END WriteHeader;

  PROCEDURE DifferentSymbolFiles(): BOOLEAN;
  (* TRUE: key for both files is different! *)
  (* pre: symbol file exists, it's path is contained in 
          "module^.file[OD.flSymExists]". *)
    VAR
      oldSymfile: Files.File;
      oldSymrider: Files.Rider;
      res: BOOLEAN;

    PROCEDURE DiffHeads(): BOOLEAN;
      VAR
        i: LONGINT;
        ch: CHAR;
      BEGIN
        Files.ReadLInt(oldSymrider,i); (* do not look on this... *)
        IF (i # SymbolFileBegin) THEN (* no symbolfile! *)
          RETURN(TRUE);
        END; (* IF *)
        Files.ReadNum(oldSymrider,i);
        IF (i # OT.objModule) THEN 
          (* buggy symbolfile, the module-info is not present *)
          RETURN(TRUE);
        END; (* IF *)
        (* do not look on the modulename *)
        REPEAT
          Files.Read(oldSymrider,ch);
        UNTIL (ORD(ch) >= 128);
        Files.ReadLInt(oldSymrider,i);
        RETURN(i # symbolfileKey); 
        (* different KeyInfo => different symbolfiles! *)
      END DiffHeads; 

    BEGIN
      oldSymfile:=Files.Old(module^.file[OD.flSymExists]);
      IF (oldSymfile # NIL) THEN
        Files.Set(oldSymrider,oldSymfile,0);
        res:=DiffHeads();
        RETURN res;
      ELSE
        RETURN(FALSE);
      END; (* IF *)
    END DifferentSymbolFiles; 

  PROCEDURE CopySymbolFile;
  (* A new symbolfile must be created, but the prototype-file contains
     no valid keyinformation. So we will copy the prototype-file to the
     final destination and insert the key at the right position *)
    VAR
      i: LONGINT;
      keyRider: Files.Rider;
      ch: CHAR;
    BEGIN
      Files.Set(keyRider,exportFile,0);
      Files.ReadNum(keyRider,i); (* symbolFileBegin *)
      Files.ReadNum(keyRider,i); (* objModule *)
      REPEAT
        Files.Read(keyRider,ch);
      UNTIL (ORD(ch) >= 128);    (* skip over module's name *)
      (* overwrite the old dummy 0-key *)
      Files.WriteLInt(keyRider,symbolfileKey);
      Files.Register(exportFile);
    END CopySymbolFile;

  BEGIN
    isExternal:=FALSE;
    InitModuleList(moduleBuffer);
    InitStructList(structBuffer);
    InsertPredefinedStructs(structBuffer);
    Redir.GeneratePathExt(OM.redir,OT.compiledModule^.name,OM.symbolExtension,tmpFileName);
    exportFile:=Files.New(tmpFileName);
    IF (exportFile # NIL) THEN
      Files.Set(exportRider,exportFile,0);
      WriteHeader;
      WriteConstTypeVarProc(OT.compiledModule^.link^.link);
      Files.WriteNum(exportRider,SymbolFileEnd);
      GenerateFingerprint(symbolfileKey);
      IF ~(OD.flSymExists IN module^.flags) OR force THEN
        CopySymbolFile;
        INCL(module^.flags,OD.flSymChanged);
      ELSIF DifferentSymbolFiles() THEN
        CopySymbolFile;
        INCL(module^.flags,OD.flSymChanged);
        OT.FlushSymbolTable(OT.compiledModule^.name);
      END; (* IF *)
      Files.Close(exportFile);
    ELSE
      Out.String("Error: Couldn't create new symbolfile!"); Out.Ln;
    END; (* IF *)
    FreeStructList(structBuffer);
    FreeModuleList(moduleBuffer);
  END Export;

PROCEDURE GetImport*(module: ARRAY OF CHAR; importing: ARRAY OF CHAR): OT.Object;
(* module: Identifier of module to be imported *)
(* result: Pointer to the module-object of corresponding module
           or NIL, if module was not found *)
  VAR
    importFile: Files.File;
    importRider: Files.Rider;
    thisModule: OT.Object;
    importedModules: OT.Object; (* list of recursively imported modules for browser *)
    structBuffer: StructList;
    moduleBuffer: ModuleList;
    filename: FileName;
    found: BOOLEAN;
    isExternal: BOOLEAN; 
    lastInserted: OT.Object;

  PROCEDURE ReadBoolFlag(VAR set: SET; flag: LONGINT);
    VAR
      x: BOOLEAN;
    BEGIN
      Files.ReadBool(importRider,x);
      IF x THEN
        INCL(set,flag);
      END; (* IF *)
    END ReadBoolFlag;
  
  PROCEDURE SetBoolFlag(VAR set: SET; flag: LONGINT; bool: BOOLEAN);
    BEGIN
      IF bool THEN
        INCL(set,flag);
      END; (* IF *)
    END SetBoolFlag;

  PROCEDURE ReadIdent(VAR ident: ARRAY OF CHAR);
  (* reads in a identifier and transfers it immediatly to the "StringBuffer"
     of "OScan.Mod".
     If name is a dummy for an not-exported record-field, it will be
     recognised as such and not be transfered.
     identifiers are delimited by a character >= CHR(128)
   *)
    VAR
      i : LONGINT;
      ch: CHAR;
    BEGIN
      i:=0;
      REPEAT
        Files.Read(importRider,ch);
        ident[i]:=ch;
        INC(i);
      UNTIL (ORD(ch) >= 128);
      ident[i-1]:=CHR(ORD(ch) - 128);
      ident[i]:=0X;
    END ReadIdent;

  PROCEDURE ReadHeader(): BOOLEAN;
    VAR
      begin,key,mnolev: LONGINT;
      name: ARRAY OM.maxSizeIdent OF CHAR;
    BEGIN
      Files.ReadLInt(importRider,begin);
      IF (begin # SymbolFileBegin) THEN 
        RETURN(FALSE);
      END; (* IF *)
      Files.ReadNum(importRider,begin);
      IF (begin # OT.objModule) THEN
        RETURN(FALSE);
      END; (* IF *)
      ReadIdent(name);
      Files.ReadLInt(importRider,key);
      thisModule:=OT.NewObject(name,OT.objModule,OT.noPos);
      mnolev:=OT.GetImportMnolev();
      thisModule^.mnolev:=mnolev;       (* this is a global number for this module *)
      thisModule^.const^.intval:=key;
      thisModule^.link^.mnolev:=mnolev; (* the scope MUST have the same number as the module! *)
      ReadBoolFlag(thisModule^.flags,OT.flagExternal);
      isExternal:=(OT.flagExternal IN thisModule^.flags);
      IF isExternal THEN
        NEW(thisModule^.const^.string);
        NEW(thisModule^.extName);
        Files.ReadString(importRider,thisModule^.const^.string^);
        Files.ReadString(importRider,thisModule^.extName^);
      END; (* IF *)
      InsertModule(moduleBuffer,thisModule);
      OT.InsertSymbolTable(name,thisModule);
      RETURN(TRUE);
    END ReadHeader;

  PROCEDURE ReadModule;
    VAR
      key   : LONGINT;
      mod,
      modloc: OT.Object;
      ident : ARRAY OM.maxSizeIdent OF CHAR;
    BEGIN
      ReadIdent(ident);
      Files.ReadLInt(importRider,key);
      mod:=GetImport(ident,importing);
      IF (mod # NIL) THEN
        IF (mod^.const^.intval = key) THEN (* module is imported & the keys are equal *)
          InsertModule(moduleBuffer,mod);
          OT.InsertSymbolTableImport(module,ident);
          IF (ident # module) THEN (* insert only really imported modules *)
            ident[0]:=CHR(ORD(ident[0]) + 128);
            modloc:=OT.NewObject(ident,OT.objModule,OT.noPos);
            modloc^.mnolev:=mod^.mnolev;
            modloc^.link:=mod^.link;
            modloc^.const:=mod^.const;
(*            OT.Insert(modloc); *)
            modloc^.next:=importedModules;
            importedModules:=modloc;
          END; (* IF *)
        ELSE (* invalid keys! *)
          OS.ErrIns2(OT.noPos,53,module,ident);
          Rts.Terminate;
        END; (* IF *)
      ELSE (* module not found! *)
        OS.ErrIns(OT.noPos,51,ident); 
        Rts.Terminate;
      END; (* IF *)
    END ReadModule;

  PROCEDURE ReadGetModule(): LONGINT;
    VAR
      mnolev: LONGINT;
    BEGIN
      Files.ReadNum(importRider,mnolev);
      IF (mnolev = OT.objModule) THEN
        ReadModule;
        mnolev:=-GetLastModNum(moduleBuffer);
      END; (* IF *)
      RETURN(mnolev);
    END ReadGetModule; 

  PROCEDURE ^ReadType(): OT.Struct;

  PROCEDURE ReadStruct(): OT.Struct;
    VAR
      type: OT.Struct;
      mode: LONGINT;
      form: LONGINT;

    PROCEDURE ReadFP(VAR parlist: OT.Object; VAR nofArgs: LONGINT);
      VAR
        last,
        par: OT.Object;
        mode,
        num  : LONGINT;
        name : ARRAY OM.maxSizeIdent OF CHAR;
      BEGIN
        last:=NIL;
        Files.ReadNum(importRider,nofArgs);
        num:=nofArgs;
        WHILE (num > 0) DO
          Files.ReadNum(importRider,mode);
          ReadIdent(name);
          par:=OT.NewObject(name,SHORT(SHORT(mode)),OT.noPos);
          SetBoolFlag(par^.flags,OT.flagExternal,isExternal);
          par^.mnolev:=OT.LevelOfTopScope();
          par^.type:=ReadStruct();
          INCL(par^.flags,OT.flagParam);
          IF (last # NIL) THEN
            last^.link:=par;
          ELSE
            parlist:=par;
          END; (* IF *)
          last:=par;
          DEC(num);
        END; (* WHILE *)
      END ReadFP;

    PROCEDURE ReadFields(scope: OT.Struct);
      VAR
        field,
        method,
        last: OT.Object;
        name: ARRAY OM.maxSizeIdent OF CHAR;
        mark,
        mode: LONGINT;

      BEGIN
        Files.ReadNum(importRider,mode);
        last:=NIL;
        WHILE (mode = OT.objField) DO
          ReadIdent(name);
          IF (name # OT.noIdent) THEN (* no anonymous field! *)
            Files.ReadNum(importRider,mark);
          ELSE
            mark:=OT.exportNot;
          END; (* IF *)
          field:=OT.NewObject(name,OT.objField,OT.noPos);
          field^.mark:=SHORT(SHORT(mark));
          SetBoolFlag(field^.flags,OT.flagExternal,isExternal);
          field^.mnolev:=OT.LevelOfTopScope();
          field^.type:=ReadStruct();
          IF (last # NIL) THEN
            last^.next:=field;
          END; (* IF *)
          last:=field;
          OT.ImportInsertField(field,scope);
          Files.ReadNum(importRider,mode);
        END; (* WHILE *)
        WHILE (mode = OT.objTBProc) DO
          ReadIdent(name);
          Files.ReadNum(importRider,mark);
          method:=OT.NewObject(name,OT.objTBProc,OT.noPos);
          method^.mnolev:=OT.LevelOfTopScope();
          method^.mark:=SHORT(SHORT(mark));
          method^.type:=ReadStruct();
          INCL(method^.type^.link^.flags,OT.flagReceiver);
          OT.ImportInsertField(method,scope);
          Files.ReadNum(importRider,mode);
        END; (* WHILE *)
       END ReadFields;

    BEGIN
      Files.ReadNum(importRider,mode);
      IF (mode = 0) THEN
        RETURN(NIL);
      ELSIF (mode < 0) THEN
        RETURN(GetStruct(structBuffer,-mode));
      ELSE (* (mode = 1) *)
        Files.ReadNum(importRider,mode);
        IF (mode = 1) THEN
          Files.ReadNum(importRider,mode);
          type:=ReadType();
          RETURN(type);
        ELSE (* mode = 0 *)
          Files.ReadNum(importRider,form);
          type:=OT.NewStruct(SHORT(SHORT(form)));
          ReadBoolFlag(type^.flags,OT.flagExternal);
          InsertStruct(structBuffer,type);
          CASE form OF
            |OT.strProc,
             OT.strTBProc:
                ReadFP(type^.link,type^.len);
            |OT.strArray:
                Files.ReadNum(importRider,type^.len);
            |OT.strDynArray:
            |OT.strRecord:
                IF isExternal THEN
                  ReadBoolFlag(type^.flags,OT.flagUnion);
                END; (* IF *)
                ReadFields(type);
            ELSE
          END; (* CASE *)
          type^.base:=ReadStruct();
          type^.size:=OT.structSize(type);
          RETURN(type);
        END; (* IF *)
      END; (* IF *)
    END ReadStruct;

  PROCEDURE ReadExternalName(obj: OT.Object);
    VAR
      test: BOOLEAN;
    BEGIN
      IF isExternal THEN
        Files.ReadBool(importRider,test);
        IF test THEN
          NEW(obj^.extName);
          Files.ReadString(importRider,obj^.extName^);
        END; (* IF *)
      END; (* IF *)
    END ReadExternalName;
    
  PROCEDURE ReadProc(mode: LONGINT);
    VAR
      proc: OT.Object;
      name: ARRAY OM.maxSizeIdent OF CHAR;
    BEGIN
      ReadIdent(name);
      proc:=OT.NewObject(name,SHORT(SHORT(mode)),OT.noPos);
      SetBoolFlag(proc^.flags,OT.flagExternal,isExternal);
      proc^.mark:=OT.exportWrite;
      ReadExternalName(proc);
      proc^.type:=ReadStruct();
      OT.InsertForOSym(proc,lastInserted);
    END ReadProc;

  PROCEDURE ReadType(): OT.Struct;
    VAR
      mark: LONGINT;
      module: LONGINT;
      obj : OT.Object;
      name: ARRAY OM.maxSizeIdent OF CHAR;
    BEGIN
      ReadIdent(name);
      Files.ReadNum(importRider,mark);
      module:=ReadGetModule();
      IF (module < 0) THEN
        obj:=OT.InternalFindImport(name,GetModule(moduleBuffer,-module));
      ELSE
        obj:=OT.NewObject(name,OT.objType,OT.noPos);
        obj^.mark:=SHORT(SHORT(mark));
        SetBoolFlag(obj^.flags,OT.flagExternal,isExternal);
        obj^.type:=ReadStruct();
        IF (obj^.type^.obj = NIL) THEN
          obj^.type^.obj:=obj;
        END; (* IF *)
        OT.InsertForOSym(obj,lastInserted);
      END; (* IF *)
      RETURN(obj^.type);
    END ReadType;

  PROCEDURE ReadVar;
    VAR
      mark: LONGINT;
      var : OT.Object;
      name: ARRAY OM.maxSizeIdent OF CHAR;
    BEGIN
      ReadIdent(name);
      Files.ReadNum(importRider,mark);
      var:=OT.NewObject(name,OT.objVar,OT.noPos);
      var^.mark:=SHORT(SHORT(mark));
      SetBoolFlag(var^.flags,OT.flagExternal,isExternal);
      ReadExternalName(var);      
      var^.type:=ReadStruct();
      OT.InsertForOSym(var,lastInserted);
    END ReadVar;

  PROCEDURE ReadConst;
    VAR
      const: OT.Object;
      type : OT.Struct;
      name: ARRAY OM.maxSizeIdent OF CHAR;

    BEGIN
      ReadIdent(name);
      const:=OT.NewObject(name,OT.objConst,OT.noPos);
      SetBoolFlag(const^.flags,OT.flagExternal,isExternal);
      const^.type :=ReadStruct();
      const^.mark :=OT.exportWrite;
      const^.const:=OT.NewConst();
      type:=const^.type;

      IF (type^.form = OT.strString) THEN
        NEW(const^.const^.string);
        Files.ReadString(importRider,const^.const^.string^);
      ELSIF (type^.form IN {OT.strBool,OT.strChar,OT.strShortInt,OT.strInteger,OT.strLongInt}) THEN
        Files.ReadNum(importRider,const^.const^.intval);
      ELSIF (type^.form IN {OT.strReal,OT.strLongReal}) THEN
        Files.ReadLReal(importRider,const^.const^.real);
      ELSIF (type^.form = OT.strSet) THEN
        Files.ReadSet(importRider,const^.const^.set);
      ELSIF (type^.form = OT.strNil) THEN
        (* Read NIL? *)
      ELSE
        Out.String("ERROR: Reading of unknown constant aborted: StructNo.:");  Out.Int(type^.form,0); Out.Ln;
      END; (* IF *)
      OT.InsertForOSym(const,lastInserted); 
    END ReadConst;

  PROCEDURE ParseSymbolFile;
    VAR
      look : LONGINT;
      dummy: OT.Struct;
    BEGIN
      Files.ReadNum(importRider,look);
      WHILE (look # SymbolFileEnd) DO
        CASE look OF
          |OT.objModule:
              ReadModule;
          |OT.objVar:
              ReadVar;
          |OT.objConst:
              ReadConst;
          |OT.objType:
              dummy:=ReadType();
          |OT.objIntProc,
           OT.objExtProc:
              ReadProc(look);
          ELSE
            Out.String("Illegal lookahead while parsing symbolfile: "); Out.Int(look,0); Out.Ln;
            Out.String("Maybe this is an old symbolfile, please remove it."); Out.Ln;
            Rts.Terminate;
        END; (* CASE *)
        Files.ReadNum(importRider,look);
      END; (* WHILE *)
    END ParseSymbolFile;

  PROCEDURE InsertImportedModulesForBrowser;
    VAR
      walk: OT.Object;
    BEGIN
      WHILE importedModules # NIL DO
        walk:=importedModules;
        importedModules:=importedModules.next;
        walk^.next:=NIL;
        OT.InsertForOSym(walk,lastInserted);
      END; (* WHILE *)
    END InsertImportedModulesForBrowser;
  
  BEGIN
    IF (module = importing) THEN
      (* module tries to import itself *)
      OS.Err(OS.lastSym,54);
      Rts.Terminate;
    ELSE
      thisModule:=OT.GetSymbolTable(module);
      IF (thisModule # NIL) THEN (* found in cache *)
        RETURN(thisModule);
      ELSE
        lastInserted:=NIL; 
        found:=Redir.FindPathExt(OM.redir,module,OM.symbolExtension,filename);
        importFile:=Files.Old(filename);
        IF (importFile = NIL) THEN
          OS.ErrIns(OT.noPos,50,filename);
          RETURN(NIL);
        END; (* IF *)
        Files.Set(importRider,importFile,0);

        (* inserts the global predefined structures in the list of
          the local imported structures, so we can reference them
          directly by their "compiler"-number, we don't have to write
          them into the symbol-file!
          Also have a good look at "OTable.InitPredefined"! *)

        InitModuleList(moduleBuffer);
        InitStructList(structBuffer);
        InsertPredefinedStructs(structBuffer);
        importedModules:=NIL;
  
        IF ReadHeader() THEN
          OT.OpenScope(thisModule^.link);
          ParseSymbolFile;
          InsertImportedModulesForBrowser;
          OT.CloseScope(TRUE); (* this must be true in order to find the modules in the right list (call "PutModulesNext" must be activated) *)
          OT.OptimizeTree(thisModule^.link^.link); (* We must supply the root for optimation! *)
          Files.Close(importFile);
          RETURN(thisModule);
        ELSE
          OS.ErrIns(OT.noPos,55,filename);
          Rts.Terminate;
        END; (* IF *)
        FreeStructList(structBuffer);
        FreeModuleList(moduleBuffer);
      END; (* IF *)
    END; (* IF *)
  END GetImport;


PROCEDURE Import*(module, alias: ARRAY OF CHAR; pos: LONGINT);
  VAR
    cacheModule,localModuleCopy: OT.Object;
  BEGIN
    IF (module = OT.compiledModule^.name) THEN
      (* Module tries to import itself: Not allowed according to Oberon-2-Report ! *)
      OS.Err(pos,54);
    ELSE
      cacheModule:=GetImport(module,OT.compiledModule^.name);
      IF (cacheModule # NIL) THEN
        localModuleCopy:=OT.NewObject(alias,OT.objModule,pos);
        OT.Insert(localModuleCopy);
        localModuleCopy^.link:=cacheModule^.link;
        localModuleCopy^.mnolev:=cacheModule^.mnolev; (* this is needed by export later *)
        localModuleCopy^.const^.intval:=cacheModule^.const^.intval; (* We have to transfer the key-information for later... *)
        localModuleCopy^.flags:=cacheModule^.flags;
      ELSE
        (* symbolfile not found -- error! *)
      END; (* IF *)
    END; (* IF *)
  END Import;

END OSym.



