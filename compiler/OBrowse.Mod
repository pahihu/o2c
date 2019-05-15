MODULE OBrowse;

IMPORT
  OT := OTable,
  OM := OMachine,
  RealStr,
  Conv := ConvTypes,
  Strings,
  O := Out;

CONST
  indent = 2; (* for each level *)
  flagBrowsed = OT.flagMinFree; (* only used in this Browser *)
  flagBrowseMark = flagBrowsed + 1; (* will be browsed in near future *)

  sflExpand = flagBrowsed + 2; (* expand Struct *)
  sflProc   = flagBrowsed + 3; (* Struct is real procedure, not proc-type *)

  doExpand = {sflExpand};
  doProc   = {sflProc};


PROCEDURE Indent (level: LONGINT);
  VAR
    i: LONGINT;
  BEGIN
    O.Ln;
    FOR i := 1 TO (level * indent) DO
      O.String (" ");
    END; (* FOR *)
  END Indent;

PROCEDURE Browse*(mod: OT.Object; expandBrowse: BOOLEAN; externalBrowse: BOOLEAN);
  VAR
    wrotetype: BOOLEAN;

  PROCEDURE IsLocal (obj: OT.Object): BOOLEAN;
    VAR
      module: OT.Object;
    BEGIN
      module := OT.GetModuleForObject (obj);
      RETURN (obj^.mnolev = OT.predeclMnolev) OR (module # NIL) & (mod^.link = module^.link);
    END IsLocal;

  PROCEDURE IsVisible (obj: OT.Object): BOOLEAN;
    BEGIN
      RETURN (obj^.mark > OT.exportNot) (* OR expandBrowse *);
    END IsVisible;

  PROCEDURE WriteString (str: ARRAY OF CHAR);

    PROCEDURE HasDoubleQuote(): BOOLEAN;
      VAR
        pos: INTEGER;
      BEGIN
        pos := 0;
        WHILE (str[pos] # 0X) & (str[pos] # '"') DO
          INC (pos)
        END;
        RETURN (str[pos] = '"')
      END HasDoubleQuote;

    BEGIN
      IF HasDoubleQuote() THEN
        O.String ("'"); O.String (str); O.String ("'");
      ELSE
        O.String ('"'); O.String (str); O.String ('"');
      END; (* IF *)
    END WriteString;

  PROCEDURE WriteIdent (obj: OT.Object);
    VAR
      module: OT.Object;
    BEGIN
      IF ~(obj^.mode IN {OT.objModule, OT.objField}) & ~IsLocal (obj) THEN (* imported type *)
        module := OT.GetModuleForObject (obj);
        O.String (module^.name); O.String (".");
      END; (* IF *)
      O.String (obj^.name);
    END WriteIdent;

  PROCEDURE IdentDef (obj: OT.Object);
    BEGIN
      WriteIdent (obj);
      IF (obj.mark = OT.exportRead) THEN
        O.String ("-")
      END; (* IF *)
      IF (obj. extName # NIL) THEN
        O.String (" [");
        WriteString (obj. extName^);
        O.String ("]")
      END
    END IdentDef;

  PROCEDURE WriteConst (constobj: OT.Object; level: LONGINT);

    PROCEDURE WriteSet (s: SET);
      VAR
        i, j  : SHORTINT;
        first: BOOLEAN;
        run  : BOOLEAN;
      BEGIN
        first := TRUE;
        run  :=FALSE;
        O.String ("{");
        i := 0;
        WHILE (i <= OM.maxSet) DO
          IF (i IN s) THEN
            IF ~first THEN
              O.String (", ");
            ELSE
              first := FALSE;
            END; (* IF *)
            O.Int (i, 0);
            INC (i);
            j := i;
            WHILE (j <= OM.maxSet) & (j IN s) DO
              INC (j);
            END; (* WHILE *)
            IF ((j - 1) > i) THEN
              O.String ("..");
              O.Int (j-1, 0);
              i := j;
            END; (* IF *)
          ELSE
            INC (i);
          END; (* IF *)
        END; (* WHILE *)
        O.String ("}");
      END WriteSet;

    PROCEDURE WriteChar (charval: LONGINT);
      VAR
        res : ARRAY 5 OF CHAR;
        conv: ARRAY 17 OF CHAR;
      BEGIN
        IF (charval < 32) OR (charval > 126) THEN (* non printable character *)
          COPY ("0123456789ABCDEF", conv);
          res[0] := conv[charval DIV 16];
          res[1] := conv[charval MOD 16];
          res[2] := "X"; res[3] := 0X; res[4] := 0X;
          IF (res[0] >= "A")
            THEN
              res[3] := res[2];
              res[2] := res[1];
              res[1] := res[0];
              res[0] := "0";
          END; (* IF *)
          O.String (res);
        ELSE
          IF (CHR (SHORT (SHORT (charval))) = '"') THEN
            O.String ("'"); O.String ('"'); O.String ("'");
          ELSE
            O.String ('"'); O.Char (CHR (SHORT (SHORT (charval)))); O.String ('"');
          END; (* IF *)
        END; (* IF *)
      END WriteChar;

    PROCEDURE WriteReal (real: REAL);
      VAR
        str : ARRAY 64 OF CHAR;
      BEGIN
        RealStr.GiveFloat (str, real, (RealStr.maxSigFigs+1) DIV 2, 0, Conv.left);
        O.String (str)
      END WriteReal;

    PROCEDURE WriteLongReal (real: LONGREAL);
      VAR
        str : ARRAY 64 OF CHAR;
      BEGIN
        RealStr.GiveFloat (str, real, RealStr.maxSigFigs, 0, Conv.left);
        str[Strings.Pos ("E", str, 0)] := "D";
        O.String (str)
      END WriteLongReal;

    BEGIN
      Indent (level); IdentDef (constobj); O.String (" = ");
      CASE constobj^.type^.form OF
        |OT.strBool:
            IF (constobj^.const^.intval = 0) THEN O.String ("FALSE");
            ELSIF (constobj^.const^.intval = 1) THEN O.String ("TRUE");
            END; (* IF *)
        |OT.strChar:
           WriteChar (constobj^.const^.intval)
        |OT.strShortInt,
         OT.strInteger,
         OT.strLongInt,
         OT.strSysByte:
            O.Int (constobj^.const^.intval, 0);
        |OT.strReal:
            WriteReal (SHORT (constobj^.const^.real));
        |OT.strLongReal:
            WriteLongReal (constobj^.const^.real);
        |OT.strSet:
            WriteSet (constobj^.const^.set);
        |OT.strString:
            WriteString (constobj^.const^.string^)
        |OT.strNil:
            O.String ("NIL");
        ELSE
      END; (* CASE *)
      O.String (";");
    END WriteConst;

  PROCEDURE ^WriteStruct (str: OT.Struct; level: LONGINT; flags: SET);

  PROCEDURE WriteRecord (str: OT.Struct; level: LONGINT);
    PROCEDURE WriteTBProcs (obj: OT.Object);
      VAR
        fp: OT.Object;
        isfunc: BOOLEAN;
      BEGIN
        IF (obj # NIL) THEN
          WriteTBProcs (obj^.left);
          IF (obj^.mode = OT.objTBProc) & IsVisible (obj) THEN
            Indent (level+1);
            fp := obj^.type^.link;
            O.String ("PROCEDURE (");
            IF fp^.mode = OT.objVarPar THEN
              O.String ("VAR ");
            END; (* IF *)
            WriteIdent (fp); O.String (": "); WriteStruct (fp^.type, level+1, doProc); fp := fp^.link; O.String (") ");

            IdentDef (obj);
            isfunc := (obj^.type^.base # OT.predeclType[OT.strNone]);
            IF (fp # NIL) OR isfunc THEN
              O.String (" (");
              WHILE (fp # NIL) DO
                IF (fp^.mode = OT.objVarPar) THEN
                  O.String ("VAR ");
                END; (* IF *)
                WriteIdent (fp);
                WHILE (fp^.link # NIL) & (fp^.link^.mode = fp^.mode) & (fp^.link^.type = fp^.type) DO
                  fp := fp^.link;
                  O.String (", "); WriteIdent (fp);
                END; (* WHILE *)
                O.String (": "); WriteStruct (fp^.type, level+1, doProc);
                fp := fp^.link;
                IF (fp # NIL) THEN
                  O.String ("; ");
                END; (* IF *)
              END; (* WHILE *)
              O.String (")");
            END; (* IF *)
            IF isfunc THEN
              O.String (": ");
              WriteStruct (obj^.type^.base, level, {});
            END; (* IF *)
            O.String (";");
          END; (* IF *)
          WriteTBProcs (obj^.right);
        END; (* IF *)
      END WriteTBProcs;

    PROCEDURE WriteFields (str: OT.Struct);
      VAR
        field, next: OT.Object;
      BEGIN
        field := str^.link;
        WHILE (field # NIL) & (field^.mode = OT.objField) DO
          next := field^.next;
          IF IsVisible (field) THEN
            Indent (level+1);
            IdentDef (field);
            WHILE (next # NIL) & (next^.type = field^.type) DO
              IF IsVisible (next) THEN
                O.String (", "); IdentDef (next);
              END; (* IF *)
              next := next^.next;
            END; (* WHILE *)
            O.String (": ");
            WriteStruct (field^.type, level+1, {}); O.String (";");
          END; (* IF *)
          field := next;
        END; (* WHILE *)
        WriteTBProcs (str^.link);
      END WriteFields;

    PROCEDURE WriteBaseFields (str: OT.Struct);
      BEGIN
        IF expandBrowse & (str^.base # NIL) THEN
          IF (str^.base^.base # NIL) THEN
            WriteBaseFields (str^.base);
          END; (* IF *)
          Indent (level+1); O.String ("(* "); WriteIdent (str^.base^.obj); O.String (" *) ");
          WriteFields (str^.base);
        END; (* IF *)
      END WriteBaseFields;

    PROCEDURE WriteBaseTypes (str: OT.Struct);
      BEGIN
        IF (str^.base # NIL) THEN
          O.String (" ("); WriteStruct (str^.base, level, {});
          IF expandBrowse THEN
            WriteBaseTypes (str^.base);
          END; (* IF *)
          O.String (")");
        END; (* IF *)
      END WriteBaseTypes;

    BEGIN
      O.String ("RECORD");
      IF (OT.flagUnion IN str^.flags) THEN
        O.String (" [UNION]");
      END; (* IF *)
      WriteBaseTypes (str);
      WriteBaseFields (str);
      IF expandBrowse & (str^.base # NIL) THEN
        Indent (level+1); O.String ("(* "); WriteIdent (str^.obj); O.String (" *)");
      END; (* IF *)
      WriteFields (str);
      Indent (level);
      O.String ("END")
    END WriteRecord;

  PROCEDURE WriteFPList (str: OT.Struct; level: LONGINT);
    VAR
      fp: OT.Object;
      isfunc: BOOLEAN;
    BEGIN
      fp := str^.link;
      isfunc := (str^.base # OT.predeclType[OT.strNone]);
      IF (fp # NIL) OR isfunc THEN
        O.String (" (");
        WHILE (fp # NIL) DO
          IF (fp.type.form = OT.strNone) THEN (* external, many more parameters... *)
            O.String ("..");
          ELSE
            IF (fp^.mode = OT.objVarPar) THEN
              O.String ("VAR ");
            END; (* IF *)
            WriteIdent (fp);
            WHILE (fp^.link # NIL) & (fp^.link^.mode = fp^.mode) & (fp^.link^.type = fp^.type) DO
              fp := fp^.link;
              O.String (", "); WriteIdent (fp);
            END; (* WHILE *)
            O.String (": "); WriteStruct (fp^.type, level+1, doProc);
          END;
          fp := fp^.link;
          IF (fp # NIL) THEN
            O.String ("; ");
          END; (* IF *)
        END; (* WHILE *)
        O.String (")");
      END; (* IF *)
      IF isfunc THEN
          O.String (": "); WriteStruct (str^.base, level, {});
      END; (* IF *)
    END WriteFPList;

  PROCEDURE ^WriteType (obj: OT.Object; level: LONGINT);

  PROCEDURE WriteAllBasetypes (str: OT.Struct; level: LONGINT);
    VAR
      walk: OT.Object;
    BEGIN
      IF (str # NIL) & ~(flagBrowseMark IN str^.flags) THEN
        INCL (str^.flags, flagBrowseMark);
        IF (str^.obj # NIL) & ~(flagBrowseMark IN str^.obj^.flags) THEN
          INCL (str^.obj^.flags, flagBrowseMark);
        END; (* IF *)
        CASE str^.form OF
          |OT.strArray,
           OT.strDynArray:
              WriteAllBasetypes (str^.base, level);
              IF (str^.obj # NIL) THEN
                WriteType (str^.obj, level);
              END; (* IF *)
          |OT.strRecord:
              WriteAllBasetypes (str^.base, level);
              walk := str^.link;
              WHILE (walk # NIL) DO
                IF IsVisible (walk) THEN
                  WriteAllBasetypes (walk^.type, level);
                END; (* IF *)
                walk := walk^.next;
              END; (* WHILE *)
              IF (str^.obj # NIL) THEN
                WriteType (str^.obj, level);
              END; (* IF *)
          |OT.strProc:
              WriteAllBasetypes (str^.base, level);
              walk := str^.link;
              WHILE (walk # NIL) DO
                WriteAllBasetypes (walk^.type, level);
                walk := walk^.link;
              END; (* WHILE *)
              IF (str^.obj # NIL) THEN
                WriteType (str^.obj, level);
              END; (* IF *)
          |OT.strTBProc:
              WriteAllBasetypes (str^.base, level);
              walk := str^.link;
              WHILE (walk # NIL) DO
                WriteAllBasetypes (walk^.type, level);
                walk := walk^.link;
              END; (* WHILE *)
          ELSE
        END; (* CASE *)
      END; (* IF *)
    END WriteAllBasetypes;

  PROCEDURE WriteType (obj: OT.Object; level: LONGINT);
    BEGIN
      WriteAllBasetypes (obj^.type, level);
      IF ~(flagBrowsed IN obj^.flags) THEN
        INCL (obj^.flags, flagBrowsed);
        IF IsLocal (obj) THEN
          Indent (level);
          IdentDef (obj);
          O.String (" = ");
          WriteStruct (obj^.type, level, doExpand); O.String (";")
        END; (* IF *)
      END; (* IF *)
    END WriteType;

  PROCEDURE WriteStruct (str: OT.Struct; level: LONGINT; flags: SET);
    VAR
      base: OT.Struct;
    BEGIN
      IF (str^.obj # NIL) & (str^.obj^.mnolev # OT.predeclMnolev) & ~(sflExpand IN flags) THEN
        (* named data type, just print name *)
        WriteIdent (str^.obj);
      ELSE
        CASE str^.form OF
          |OT.strBool..OT.strSet:
              O.String (str^.obj^.name);
          |OT.strNil:
              O.String ("NIL");
          |OT.strSysByte:
              O.String ("SYSTEM.BYTE");
          |OT.strSysPtr:
              O.String ("SYSTEM.PTR");
          |OT.strPointer:
              O.String ("POINTER TO ");
              WriteStruct (str^.base, level+1, flags-doExpand);
          |OT.strProc:
              IF ~(sflProc IN flags) THEN
                O.String ("PROCEDURE");
              END; (* IF *)
              WriteFPList (str, level);
          |OT.strTBProc:
              O.String ("strTBProc ");
          |OT.strArray:
              base := str^.base;
              O.String ("ARRAY ");
              O.Int (str^.len, 0);
              WHILE (base # NIL) & (base^.obj = NIL) & (base^.form = OT.strArray) DO
                O.String (", "); O.Int (base^.len, 0);
                base := base^.base;
              END; (* WHILE *)
              O.String (" OF ");
              WriteStruct (base, level+1, flags-doExpand);
          |OT.strDynArray:
              O.String ("ARRAY OF ");
              WriteStruct (str^.base, level+1, flags-doExpand);
          |OT.strRecord:
              WriteRecord (str, level);
          ELSE
        END; (* CASE *)
      END; (* IF *)
    END WriteStruct;

  PROCEDURE Import (mod: OT.Object);
    VAR
      walk: OT.Object;
    BEGIN
      walk := mod^.link^.next;
      IF (walk # NIL) THEN
        O.Ln; O.String ("IMPORT");
        Indent (1);
        REPEAT
          WriteIdent (walk^.link^.left); (* O.String ("(* "); O.Int (walk^.const^.intval, 0); O.String (" *)"); *)
          walk := walk^.next;
          IF (walk # NIL) THEN
            O.String (", ");
          ELSE
            O.String (";");
          END; (* IF *)
        UNTIL (walk = NIL);
        O.Ln;
      END; (* IF *)
    END Import;

  PROCEDURE Const (constobj: OT.Object; level: LONGINT);
    VAR
      wroteconst: BOOLEAN;
    BEGIN
      wroteconst := FALSE;
      WHILE (constobj # NIL) DO
        IF (constobj^.mode = OT.objConst) & IsVisible (constobj) THEN
          IF ~wroteconst THEN
             Indent (level); O.String ("CONST");
             wroteconst := TRUE;
          END; (* IF *)
          WriteConst (constobj, level+1);
          INCL (constobj^.flags, flagBrowsed);
        END; (* IF *)
        constobj := constobj^.sort;
      END; (* WHILE *)
      IF wroteconst THEN
        O.Ln;
      END; (* IF *)
    END Const;

  PROCEDURE Type (typeobj: OT.Object; level: LONGINT);
    VAR
      wrotetype: BOOLEAN;
      walk, alias: OT.Object;
      typeflags: SET;
    BEGIN
      wrotetype := FALSE;
      walk := typeobj;
      WHILE (walk # NIL) DO
        IF (walk^.mode = OT.objType) & ~(flagBrowsed IN walk^.flags) & (walk^.type^.form = OT.strPointer) & IsVisible (walk) THEN
          IF ~wrotetype THEN
            Indent (level); O.String ("TYPE");
            wrotetype := TRUE;
          END; (* IF *)
          
          INCL (walk^.flags, flagBrowsed);
          
          alias := walk^.type^.obj;
          IF (alias = walk) THEN alias := NIL; END; (* IF *)
          
          IF (alias # NIL) THEN
            (* browse alias *)
            IF IsLocal (alias) & ~(flagBrowsed IN alias.flags) THEN 
              (* 'alias' is also a local type, which is not declared yet *)
              Indent (level+1);
              IdentDef (alias);
              O.String (" = "); WriteStruct (alias^.type, level, doExpand);
              INCL (alias.flags, flagBrowsed);
            END;
            Indent (level+1);
            IdentDef (walk); O.String (" = "); IdentDef(alias);
          ELSE
            Indent (level+1);
            IdentDef (walk); O.String (" = "); WriteStruct (walk^.type, level, doExpand);
          END; (* IF *)
          O.String (";");
        END; (* IF *)
        walk := walk^.sort;
      END; (* WHILE *)

      walk := typeobj;
      WHILE (walk # NIL) DO
        IF (walk^.mode = OT.objType) & ~(flagBrowsed IN walk^.flags) & ~(walk^.type^.form = OT.strPointer) & IsVisible (walk) THEN
          IF ~wrotetype THEN
            Indent (level); O.String ("TYPE");
            wrotetype := TRUE;
          END; (* IF *)
          alias := walk^.type^.obj;
          IF (alias = walk) THEN alias := NIL; END; (* IF *)
          IF (alias # NIL) & ~(flagBrowsed IN alias^.flags)  THEN
            IF IsLocal (alias) THEN
              typeflags := doExpand;
            ELSE
              typeflags := {};
            END; (* IF *)
            Indent (level+1);
            IdentDef (walk); O.String (" = "); WriteStruct (alias^.type, level, typeflags); O.String (";");
            INCL (walk^.flags, flagBrowsed);
          END; (* IF *)
          WriteType (walk, level+1);
        END; (* IF *)
        walk := walk^.sort;
      END; (* WHILE *)
      IF wrotetype THEN O.Ln; END; (* IF *)
    END Type;

  PROCEDURE Var (varobj: OT.Object; level: LONGINT);
    VAR
      wrotevar: BOOLEAN;
      sortnext: OT.Object;
    BEGIN
      wrotevar := FALSE;
      WHILE (varobj # NIL) DO
        IF IsVisible (varobj) & (varobj^.mode = OT.objVar) & ~(flagBrowsed IN varobj^.flags) THEN
          IF ~wrotevar THEN
            Indent (level); O.String ("VAR");
            wrotevar := TRUE;
          END; (* IF *)
          Indent (level+1);
          IdentDef (varobj);
          INCL (varobj^.flags, flagBrowsed);
          IF (varobj^.type^.obj = NIL) THEN
            (* all variables declared with unnamed types have to be written
               just like their original definition *)
            sortnext := varobj^.sort;
            WHILE (sortnext # NIL) DO
              IF IsVisible (sortnext) & (sortnext^.mode = OT.objVar) & (sortnext^.type = varobj^.type) THEN
                O.String (", "); IdentDef (sortnext);
                INCL (sortnext^.flags, flagBrowsed);
              END; (* IF *)
              sortnext := sortnext^.sort;
            END; (* WHILE *)
          END; (* IF *)
          O.String (": "); WriteStruct (varobj^.type, level+1, {}); O.String (";");
        END; (* IF *)
        varobj := varobj^.sort;
      END; (* WHILE *)
      IF wrotevar THEN O.Ln; END; (* IF *)
    END Var;

  PROCEDURE Proc (procobj: OT.Object; level: LONGINT);
    VAR
      procwritten: BOOLEAN;
    BEGIN
      procwritten := FALSE;
      WHILE (procobj # NIL) DO
        IF (procobj^.mode IN {OT.objExtProc, OT.objIntProc, OT.objLocalProc}) & IsVisible (procobj) THEN
          procwritten := TRUE;
          Indent (level);
          O.String ("PROCEDURE "); IdentDef (procobj);
          IF (procobj^.mnolev # OT.systemMnolev) THEN
            WriteStruct (procobj^.type, level, doProc); O.String ("; ");
          ELSE
            O.String ("; (* SYSTEM *)");
          END;
        END; (* IF *)
        procobj := procobj^.sort;
      END; (* WHILE *)
      IF procwritten THEN O.Ln; END; (* IF *)
    END Proc;

  BEGIN
    IF expandBrowse THEN
      O.String ("EXTENDED ");
    END; (* IF *)
    O.String ("DEFINITION"); O.String (" "); WriteIdent (mod);
    IF externalBrowse & (mod^.const^.string # NIL) THEN
      O.String (' ["'); O.String (mod^.const^.string^); O.String ('"]');
    END; (* IF *)
    IF (OT.flagExternal IN mod^.flags) THEN
      O.String (" EXTERNAL");
    END; (* IF *)
    IF externalBrowse & (mod^.extName # NIL) THEN
      O.String (' ["'); O.String (mod^.extName^); O.String ('"]');
    END; (* IF *)
    O.String (";"); O.Ln;
(*    O.String ("(* "); O.Int (mod^.const^.intval, 0); O.String (" *)"); *)
    Import (mod);
    Const (mod^.link^.sort, 0);
    wrotetype := FALSE;
    Type (mod^.link^.sort, 0);
    Var (mod^.link^.sort, 0);
    Proc (mod^.link^.sort, 0);
    O.Ln; O.String ("END"); O.String (" "); WriteIdent (mod); O.String ("."); O.Ln;
  END Browse;

END OBrowse.

