MODULE OGenGCC;  (* Author: Michael van Acken *)
(*      $Id: OGenGCC.Mod,v 1.59 1997/03/27 13:11:22 oberon1 Exp $     *)

IMPORT
  O := FOut, Reals := RealStr, Conv := ConvTypes, Str := Strings, Strings2,
  IntStr, Redir, Out, Rts, Filenames, 
  M := OMachine, S := OScan, T := OTable, E := OEParse, Dep := ODepend;

CONST
  flagHeader = 0;      flagSemicolon = 1;    flagTypeDef = 2;  flagVarDef = 3;
  flagAutoPrefix = 5;  flagFunctHeader = 6;  flagNoName = 7;   flagExtName = 8;
  flagOberonName = 9;
  arraySet = {T.strArray, T.strDynArray};

  ofDefined = T.flagMinFree;             (* set iff a given type definition has been written *)
  sfChecked = 31;

CONST
  fAddType = 0;
  fAddPosition = 1;
  fMonadic = 2;
  fAddTypename = 3;
  fLeftAdr = 4;
  fAddSize = 5;
  
  header* = Dep.flBE0Exists;
  cFile* = Dep.flBE1Exists;
  objFile* = Dep.flBE2Exists;

  cExt* = "c";                           (* file suffix for C source file *)
  hExt* = "h";                           (* file suffix for header file *)
  markerExt* = "m";

  (* classes for the three types of external modules *)
  extCSource* = 1;                       (* xyz.c *)
  extObject* = 2;                        (* xyz.o *)
  extArchive* = 3;                       (* xyz.a *)
  extLibrary* = 4;                       (* libxyz.a *)
  
VAR
  emptyObj : T.Object;                   (* type object, used by Typecast *)
  tdGenerated : BOOLEAN;                 (* used to enable/disable GenTypeDescr *)
  root : E.Node;
  atCount : LONGINT;                     (* counter for names of anonymous types *)
  extName : ARRAY 2*M.maxSizeIdent+8 OF CHAR;  (* used by VarDecl *)



PROCEDURE Off (off : INTEGER);
(* Iff off>=0 print 'newline' and indent for 'off' levels. *)
  BEGIN
    IF (off>=0) THEN
      O.Ln;
      WHILE (off > 0) DO
        O.String ("  ");
        DEC (off)
      END
    END
  END Off;

PROCEDURE TBIdent (obj : T.Object; VAR name : ARRAY OF CHAR);
  VAR
    t : T.Struct;
  BEGIN
    T.GetModuleName (obj, name);
    Strings2.AppendChar ("_", name);
    t := obj. type. link. type;
    IF (obj. type. link. type. form = T.strPointer) THEN
      Str.Append (t. base. obj. name, name)
    ELSE
      Str.Append (t. obj. name, name)
    END;
    Strings2.AppendChar ("_", name);
    Str.Append (obj. name, name)
  END TBIdent;
  
PROCEDURE Ident (obj : T.Object);
(* Generates the C identifier for a given object.
   The object can be local, exported, imported or predefined. *)
  VAR
    name : ARRAY 2*M.maxSizeIdent+8 OF CHAR;
  BEGIN
    IF (T.flagExternal IN obj. flags) & 
       (obj. mode IN {T.objVar, T.objLocalProc, T.objExtProc}) THEN
      IF (obj. extName # NIL) THEN
        O.String (obj. extName^)
      ELSE 
        O.String (obj. name)
      END
    ELSIF (obj. mode = T.objType) & (obj. type. form <= T.strFixedMax) & 
          (obj. type. obj = obj) OR
          (obj. mode = T.objConst) & (obj. type. form = T.strBool) &
          (obj. mnolev = T.predeclMnolev) THEN
      (* take care of the standard (predefined) data types and the boolean 
         constants *)
      O.String (obj. name)
    ELSIF (obj. mode = T.objTBProc) THEN   (* type bound procedure *)
      O.String ("TB_");
      TBIdent (obj, name);
      O.String (name)
    ELSE  (* normal identifiers *)
      IF (obj. mode = T. objField) OR (obj. mnolev > T.compileMnolev) THEN
      ELSIF (obj. mode # T.objModule) THEN
        IF (obj. mnolev <= T.importMnolev) THEN  
          (* imported from an extern module *)
          T.GetModuleName (obj, name);
          O.String (name)
        ELSE  (* declared in the current module *)
          O.String (T.compiledModule. name)
        END
      END;
      O.Char ("_");
      O.String (obj. name)
    END
  END Ident;




PROCEDURE CheckName (block: T.Object; VAR obj : T.Object);
(* Provide anonymous record types with a name and insert type declarations
   for this types. *)

  PROCEDURE CorrectType (t : T.Struct; export, checkStruct : BOOLEAN);
  (* Consider named types only if checkStruct=TRUE. *)
    VAR
      new : T.Object;
      name : ARRAY 16 OF CHAR;
      
    PROCEDURE TraverseRecord (o : T.Object);
      BEGIN
        IF (o # NIL) THEN
          TraverseRecord (o. next);
          IF (o. mode = T.objField) & (o. type. obj = NIL) THEN
            CorrectType (o. type, export, FALSE)
          END          
        END
      END TraverseRecord;
      
    PROCEDURE AssignName (VAR name : ARRAY OF CHAR);
      BEGIN
        COPY ("at_", name);  (* '_' on position marks anonymous types *)
        IntStr.Append (atCount, 0, Conv.left, name);
        INC (atCount)
      END AssignName;
    
    BEGIN
      IF ~(sfChecked IN t. flags) & (checkStruct OR (t. obj = NIL)) THEN
        INCL (t. flags, sfChecked);
        IF (t. form = T.strRecord) & (t. obj = NIL) THEN          
          (* is an unnamed record, provide name *)
          AssignName (name);
          new := T.NewObject(name, T.objType, T.noPos);
          new. type := t;          
          new. next := obj;
          IF export THEN
            INCL (new. flags, T.flagExport)
          END;
          t. obj := new;
          obj := new
        ELSIF (t. form = T.strRecord) & (t. obj. mnolev > 0) THEN
          (* 't' is a record, declared inside a procedure: provide it with
             a unique (on global scope) name, for type descriptor etc. *)
          AssignName (t. obj. name)
        END;
        CASE t. form OF
        | T.strPointer, T.strArray, T.strDynArray:
          IF (t. base. obj = NIL) THEN
            CorrectType (t. base, export, FALSE)
          END
        | T.strRecord:
          TraverseRecord (t. link)
        ELSE
          (* ignore *)
        END
      END
    END CorrectType;
    
  BEGIN
    IF (obj # NIL) THEN
      CheckName (block, obj. next);
      CASE obj. mode OF
      | T.objVar, T.objType:
        IF (obj. type. obj = NIL) OR (obj. type. obj = obj) THEN
          CorrectType (obj. type, T.flagExport IN obj. flags, TRUE)
        END
      | T.objExtProc, T.objIntProc, T.objLocalProc, T.objTBProc:
        CheckName (obj, obj. link. right)
      ELSE
        (* ignore rest *)
      END
    END
  END CheckName;

    

PROCEDURE TypeDescrIdent (t : T.Struct; variable : BOOLEAN);
(* Writes name of type desciptor variable or type. *)
  BEGIN
    IF (t. form = T.strPointer) THEN
      t := t. base
    END;
    IF variable THEN
      O.String ("td_")
    ELSE
      O.String ("TD_")
    END (* if *);
    Ident (t. obj)
  END TypeDescrIdent;

PROCEDURE GenTagName (obj : T.Object);
  BEGIN
    O.String ("tag_");
    Ident (obj)
  END GenTagName;
  
PROCEDURE ^ VarDecl (name : ARRAY OF CHAR; type : T.Struct);

PROCEDURE ReceiverRecord (obj : T.Object) : T.Struct;
(* pre: 'obj' denotes a type bound procedure.
   result: The record the procedure is bound to.  If the actual
     receiver is a record this is its type, if it is a pointer,
     it is the pointers base type. *)
  VAR
    r : T.Struct;
  BEGIN
    r := obj. type. link. type;
    IF (r. form = T.strPointer) THEN
      RETURN r. base
    END;
    RETURN r
  END ReceiverRecord;

PROCEDURE GenTypeDescrForward (tdList : E.Node; header : BOOLEAN);
  VAR
    td : E.Node;
  BEGIN
    IF ~tdGenerated THEN                 (* td havn't been written yet *)
      (* write forward declarations of type descriptor types *)
      O.Ln;      
      td := tdList;
      WHILE (td # NIL) DO
        IF ((T.flagExport IN td. type. obj. flags) = header) THEN
          Off (0);
          O.String ("struct ");
          TypeDescrIdent (td. type, FALSE);
          O.String (";")
        END;
        td := td. link
      END;
      O.Ln
    END
  END GenTypeDescrForward;
  
PROCEDURE GenTypeDescr (tdList : E.Node; header : BOOLEAN);
  VAR
    td : E.Node;
    tbCount : INTEGER;
    
  PROCEDURE GenTBEntries (type : T.Struct);    
    PROCEDURE GenEntry (o : T.Object);
      VAR
        name : ARRAY M.maxSizeIdent+4 OF CHAR;
        mr : T.Object;  (* most recent redeclaration *)
      BEGIN
        IF (o # NIL) THEN
          GenEntry (o. left);
          IF (o. mode = T.objTBProc) & 
             ((type. base = NIL) OR (T.FindField (o. name, type. base) = NIL)) THEN
            mr := T.FindField (o. name, td. type);
            IF (mr = NIL) THEN
              mr := o
            END;
            name := "tb_";
            IntStr.Append (tbCount, 0, 0, name);
            Off (1);
            VarDecl (name, mr. type);
            O.String (";  /* ");
            O.String (mr. name);
            O.String (" */");
            INC (tbCount)
          END;
          GenEntry (o. right)
        END
      END GenEntry;
      
    BEGIN
      IF (type # NIL) THEN
        GenTBEntries (type. base);
        GenEntry (type. link)
      END
    END GenTBEntries;
    
  BEGIN
    IF ~tdGenerated THEN                 (* td havn't been written yet *)
      (* write type descriptor types *)
      O.Ln;      
      td := tdList;
      WHILE (td # NIL) DO
        IF ((T.flagExport IN td. type. obj. flags) = header) THEN
          tbCount := 0;
          Off (0);
          O.String ("typedef struct ");
          TypeDescrIdent (td. type, FALSE);
          O.String (" {");
          Off (1);
          O.String ("TDCORE");
          GenTBEntries (td. type);
          Off (0);
          O.String ("} ");
          TypeDescrIdent (td. type, FALSE);
          O.Char (";");        
          Off (0);
          O.String ("EXTERN");              (* AP 19.1.2026 mark TD as module extern *)
          Ident (T.compiledModule);
          O.Char (" ");
          TypeDescrIdent (td. type, FALSE);
          O.String ("* ");
          TypeDescrIdent (td. type, TRUE);
          O.Char (";")
        END;
        td := td. link
      END;
      tdGenerated := TRUE;               (* td are done *)
      O.Ln
    END
  END GenTypeDescr;

PROCEDURE TypeDescId (obj : T.Object) : INTEGER;
(* pre: 'obj' denotes a type bound procedure.
   result: The field number associated with the tb proc in the 
           type descriptor structure. *)
  VAR
    tbCount : INTEGER;
         
  PROCEDURE ScanStruct (t : T.Struct) : INTEGER;
    VAR
      res : INTEGER;

    PROCEDURE ScanFields (o : T.Object) : INTEGER;
      VAR
        res : INTEGER;

      PROCEDURE Redefinition (t: T.Struct; tbProc: T.Object): BOOLEAN;
      (* Returns TRUE iff `tbProc' is a redefinition of another type-bound
         procedure defined in `t' or one of its base types.  *)
        VAR
          obj: T.Object;
        BEGIN
          IF (t = NIL) THEN
            RETURN FALSE
          ELSE
            obj := T.SearchInTree (t. link, tbProc. name);
            IF (obj # NIL) & (obj. mode = T.objTBProc) &
               ((obj. mnolev = tbProc. mnolev) OR (obj. mark # T.exportNot)) THEN
              (* `t' has a type-bound proc that is visible at the place of the
                 definition of `tbProc', therefore `tbProc' has to be a 
                 redefinition *)
              RETURN TRUE
            ELSE
              RETURN Redefinition (t. base, tbProc)
            END
          END
        END Redefinition;
      
      BEGIN
        IF (o # NIL) THEN
          res := ScanFields (o. left);
          IF (res < 0) & (o. mode = T.objTBProc) & ~Redefinition (t. base, o) THEN
            IF (o. name = obj. name) THEN
              RETURN tbCount
            ELSE
              INC (tbCount)
            END
          END;
          IF (res < 0) THEN
            res := ScanFields (o. right)
          END;
          RETURN res
        ELSE 
          RETURN -1
        END
      END ScanFields;
      
    BEGIN
      IF (t # NIL) THEN
        res := ScanStruct (t. base);
        IF (res < 0) THEN                (* tb proc not defined in base type *)
          res := ScanFields (t. link)
        END;
        RETURN res
      ELSE
        RETURN -1
      END
    END ScanStruct;
    
  BEGIN
    tbCount := 0;
    RETURN ScanStruct (ReceiverRecord (obj))
  END TypeDescId;
  
PROCEDURE GenTypeDescrInit (tdList : E.Node);
  VAR
    td : E.Node;
    name : ARRAY M.maxSizeIdent*2+8 OF CHAR;
    size : BOOLEAN;
  
  PROCEDURE GenTBInit (o : T.Object);
    BEGIN
      IF (o # NIL) THEN
        GenTBInit (o. left);
        IF (o. mode = T.objTBProc) THEN
          Off (1);
          TypeDescrIdent (td. type, TRUE);
          O.String ("->tb_");
          O.Int (TypeDescId (o), 0);
          O.String (" = TB_");
          TBIdent (o, name);
          O.String (name);
          O.Char (";")
        END;
        GenTBInit (o. right)
      END
    END GenTBInit;
  
  BEGIN
    td := tdList;
    WHILE (td # NIL) DO
      Off (1);
      TypeDescrIdent (td. type, TRUE);
      O.String (' = create_td("');
      COPY (td. type. obj. name, name);
      IF (name[2] = "_") OR (td. type. obj. scope. left. mode # T.objModule) THEN
        name := "";
        size := FALSE
      ELSE
        size := TRUE
      END;
      O.String (name);
      O.String ('", ');
      IF size THEN
        O.String ("sizeof(");
        Ident (td. type. obj)
      ELSE
        O.String ("(0")
      END;
      O.String ("), sizeof(");
      TypeDescrIdent (td. type, FALSE);
      O.String ("), ");
      IF (td. type. base = NIL) THEN
        O.String ("NULL, 0);")
      ELSE
        TypeDescrIdent (td. type. base, TRUE);
        O.String (", sizeof(");
        TypeDescrIdent (td. type. base, FALSE);
        O.String ("));")
      END;
      (* fill fields with type bound procedures *)
      GenTBInit (td. type. link);
      td := td. link
    END
  END GenTypeDescrInit;


  
  


PROCEDURE DynDimensions (t : T.Struct) : INTEGER;
(* pre: t#NIL *)
  BEGIN
    IF (t. form = T.strDynArray) THEN
      RETURN DynDimensions (t. base)+1
    ELSE
      RETURN 0
    END
  END DynDimensions;

PROCEDURE OpenArrayDecl (o : T.Object; modified : BOOLEAN);
(* Writes the variable declaration that represents a single formal
   dynamic array parameter resp. a pointer to an open array.
   pre: 'o' is a formal parameter of an array (dynamic or normal) type or
     a pointer variable whose base type is an open array.
     If 'modified=TRUE', then the name of the variable pointing to the
     array is transformed to '_ident_p', otherwise it stays 'Pident'.
   post: 'base ( * name)' is written, with base the array's
     base type, name equals 'ident' is modified=FALSE, '_ident_p'
     otherwise. *)
  VAR
    base : T.Struct;
  BEGIN
    (* find first non dynamic element type of array *)
    base := o. type;
    WHILE (base. form = T.strDynArray) DO
      base := base. base
    END;
    Ident (base. obj);
    O.Char (" ");
    IF (o. type. form # T.strArray) THEN
      O.String ("(* ");
    END;
    IF modified THEN
      O.Char ("_");
      Ident (o);
      O.String ("_p");
    ELSE
      Ident (o)
    END;
    IF (o. type. form # T.strArray) THEN
      O.Char (")")
    END
  END OpenArrayDecl;

PROCEDURE NamedDecl (o : T.Object; flags : SET; off : INTEGER) : T.Object;
(* Generate code for named declaration.
   pre: o.mode IN ({objType, objVar, objVarPar, objField}+objProcSet)
     flagHeader IN flags: declaration is written into the .h file (.c file otherwise)
     flagSemicolon IN flags: conclude declaration with a ';'
     flagTypeDef IN flags: declaration is a type definition
     flagAutoPrefix IN flags: a locally declared procedure prototype is generated
     flagFunctHeader IN flags: a function heading is written
     flagNoName IN flags: don't write the objects name (used for type casts)
     flagExtName IN flags: use extName instead of o.name (used by VarDecl)
     flagOberonName IN flags: use unmodified o.name as C name
   post: Result is the next unprinted declaration. *)
  VAR
    obj : T.Object;
    right : BOOLEAN;
    base : T.Struct;

  PROCEDURE FormalPars (pars : T.Struct);
  (* Generates list of parameters (ansi style). *)
    VAR
      i : INTEGER;
      first, next : T.Object;
    BEGIN
      first := pars. link;
      O.String (" (");
      IF (first = NIL) THEN  (* empty parameter list *)
        O.String ("void")
      ELSE  (* at least one parameter *)
        WHILE (first # NIL) DO
          next := first. link;
          IF (first. type. form IN arraySet) THEN  (* arrays need special treatment *)
            (* suppress implicit length parameters if a type or the parameter list are external *)
            IF ~(T.flagExternal IN (first. type. flags+pars. flags)) THEN
              (* write dimension lengths *)
              FOR i := 0 TO DynDimensions (first. type)-1 DO
                O.String ("LONGINT ");
                O.String ("_");
                Ident (first);
                O.Char ("_");
                O.Int (i, 0);
                O.String (", ")
              END
            END;
            IF (first. mode = T.objVar) THEN  (* value array parameter *)
              O.String ("const ")
            END;
            OpenArrayDecl (first, (first. mode = T.objVar) & ~T.external)
          ELSIF ~(T.flagExternal IN (first. type. flags+pars. flags)) & 
                (first. mode = T.objVarPar) & 
                (first. type. form = T.strRecord) THEN
            (* variable record parameter: pass tag with parameter *)
            O.String ("struct ");
            TypeDescrIdent (first. type, FALSE);
            O.String ("* ");
            GenTagName (first);
            O.String (", ");
            first := NamedDecl (first, {}, off)
          ELSIF (first. type. form = T.strNone) THEN  (* generic paramter '..' *)
            O.String ("...")
          ELSE  (* 'normal' parameter declaration *)
            first := NamedDecl (first, {}, off)
          END;
          first := next;  (* we need first.link here, not first.next as given by NamedDecl *)
          IF (first # NIL) THEN
            O.String (", ")
          END
        END
      END;
      O.Char (")")
    END FormalPars;

  PROCEDURE Structure (t : T.Struct) : BOOLEAN;
  (* TRUE iff the complete structure of 't' has to be written (otherwise
     the type name is used instead). *)
    BEGIN
      RETURN (* 't' is an anonymous type *)
             (t. obj = NIL) OR
             (* 't' is locally declared, not a record and not defined already 
                (here or in the header file) *)
             (t. obj. mnolev >= T.compileMnolev) & (t. form # T.strRecord) & 
               ~((ofDefined IN t. obj. flags) OR 
                 (T.flagExport IN t. obj. flags) & ~(flagHeader IN flags)) OR
             (* we a writing a type definition, 't' is being defined *)
             (flagTypeDef IN flags) & (t. obj = o)
    END Structure;

  PROCEDURE TypeLeft (t : T.Struct);
  (* Generates left hand side (i.e. left of name) of a var/const/type 
     declaration. *)
    VAR
      first : T.Object;
      declFlags : SET;
    BEGIN  (* t=NIL marks end of recursion *)
      IF (t # NIL) & ~Structure (t) THEN
        (* write type name *)
        IF (t. form = T.strRecord) THEN
          (* 't' denotes a record, use struct/union tag for the type *)
          IF (T.flagUnion IN t. flags) THEN
            O.String ("union ")
          ELSE
            O.String ("struct ")
          END
        END;
        Ident (t. obj);
        O.Char (" ")
      ELSIF (t # NIL) THEN
        (* generate the type description repectively write the first level structure *)
        CASE t. form OF
        T.strNone:
          O.String ("void ") |
        T.strPointer:
          TypeLeft (t. base);
          IF (t. base. form # T.strDynArray) THEN
            O.String ("(* ")
          END |
        T.strProc, T.strTBProc:
          TypeLeft (t. base);
          IF ~(flagFunctHeader IN flags) THEN
            O.String ("(* ")
          END |
        T.strArray:
          TypeLeft (t. base) |
        T.strString:
          O.String ("CHAR ") |
        T.strDynArray:
          base := t;
          WHILE (base. form = T.strDynArray) DO
            base := base. base
          END;
          TypeLeft (base);
          O.String ("(* ") |
        T.strRecord:
          IF (T.flagUnion IN t. flags) THEN
            O.String ("union ")
          ELSE
            O.String ("struct ")
          END;
          IF (t. obj # NIL) THEN
            Ident (t. obj);
          END;
          O.String (" {");
          IF (t. base # NIL) THEN
            Off (off+1);
            Ident (t. base. obj);
            O.String (" base;")
          END;
          (* external record fields aren't written as "_field" *)
          IF (T.flagExternal IN t. flags) THEN
            declFlags := {flagOberonName}
          ELSE
            declFlags := {}
          END;
          first := t. link;
          WHILE (first # NIL) DO
            IF (first. mode = T.objField) THEN
              (* write single record field *)
              Off (off+1);
              first := NamedDecl (first, declFlags, off+1);
              O.Char (";")
            ELSE 
              first := first. next
            END
          END;
          Off (off);
          O.String ("} ");
        END
      END
    END TypeLeft;

  PROCEDURE TypeRight (t : T.Struct);
  (* Generates right hand side (eg right of name) of a var/const/type 
     declaration.
     side: 'right' is set to TRUE iff any (functional) part of a type 
       definition is written to the right of the object's name. *)
    BEGIN
      IF (t # NIL) & Structure (t) THEN
        (* Generate the contructs matching the left hand side. *)
        CASE t. form OF
        T.strPointer:
          right := TRUE;
          IF (t. base. form # T.strDynArray) THEN
            O.Char (")")
          END;
          TypeRight (t. base) |
        T.strProc, T.strTBProc:
          right := TRUE;
          IF ~(flagFunctHeader IN flags) THEN
            O.Char (")")
          END;
          FormalPars (t) |
        T.strString:
          right := TRUE;
          O.String ("[]") |
        T.strArray:
          right := TRUE;
          O.Char ("[");
          O.Int (t. len, 0);
          O.Char ("]");
          TypeRight (t. base) |
        T.strDynArray:
          base := t;
          WHILE (base. form = T.strDynArray) DO
            base := base. base
          END;
          O.Char (")");
          TypeRight (base)
        ELSE  (* T.strNone, T.strRecord: No right hand side. *)
        END
      END
    END TypeRight;

  BEGIN
    right := FALSE;
    TypeLeft (o. type);
    IF (o. mode = T.objVarPar) & ~(o. type. form IN arraySet) THEN
      (* formal var parameter (but not an array) *)
      O.String ("(* ")
    END;
    IF ~(flagNoName IN flags) THEN
      IF (flagExtName IN flags) THEN
        O.String (extName)
      ELSIF (flagOberonName IN flags) THEN
        O.String (o. name)  (* use plain name for external record field *)
      ELSE
        Ident (o)
      END
    END;
    IF (o. mode = T.objVarPar) & ~(o. type. form IN arraySet) THEN
      O.Char (")")
    END;
    TypeRight (o. type);
    obj := o;
    IF ~right & ((o. mode = T.objVar) OR (o. mode = T.objVarPar)) THEN
      (* no right part was written, multiple declaration of variables allowed *)
      WHILE (obj. next # NIL) & (obj. next. mode = o. mode) & (obj. next. type = o. type) &
            (obj. mark = obj. next. mark) & ~(T.flagParam IN obj. flags) DO
        (* 'obj.next' is a variable of the same type like 'o' and is no formal parameter *)
        obj := obj. next;
        O.String (", ");
        Ident (obj)
      END;
    END;
    RETURN obj. next
  END NamedDecl;

PROCEDURE Const (c : T.Const; form : SHORTINT; obj : T.Object);
(* Generates ASCII representation of the constant in 'c' of type 'form'.
   If 'obj' is not NIL, the name assciated to it is used instead of the
   constant's value. *)
  VAR
    i : INTEGER;

  PROCEDURE CharConst (ch : LONGINT; quote: BOOLEAN);
  (* If 'ch' is a printable ASCII code it writes the character itself,
     otherwise it generates the escape sequence. *)
    BEGIN
      IF quote THEN
        IF (ch>=127) THEN
          (* Character constants are (in gcc 2.6.3) of the type 'signed char'.
             The type cast forces them into an 'unsigned char' type, allowing
             comparisons between character variables and (large) constants. *)
          O.String ("(CHAR)")
        END;
        O.Char ("'")
      END;
      IF (ch<32) OR (ch>=127) OR (ch=34) OR (ch=39) OR (ch=92) THEN
        (* control code, non-ASCII, ", ', or \ *)
        O.Char ("\");
        O.Char (CHR (ch DIV 64+ORD ("0")));
        O.Char (CHR ((ch DIV 8) MOD 8+ORD ("0")));
        O.Char (CHR (ch MOD 8+ORD ("0")))
      ELSE
        O.Char (CHR (ch))
      END;
      IF quote THEN
        O.Char ("'")
      END
    END CharConst;

  PROCEDURE RealConst (val : LONGREAL; long : BOOLEAN);
  (* If 'val' equals one of the floating point MIN/MAX values
     the symbolic (see _OGCC.h) constants are written, otherwise
     an ASCII representation of the constant's value is generated. *)
    VAR
      i : INTEGER;
      str : ARRAY 64 OF CHAR;
    BEGIN
      IF (val = MAX(LONGREAL)) THEN
        O.String ("MAX_LONGREAL")
      ELSIF (val = MIN(LONGREAL)) THEN
        O.String ("MIN_LONGREAL")
      ELSIF (val = MAX(REAL)) THEN
        O.String ("MAX_REAL")
      ELSIF (val = MIN(REAL)) THEN
        O.String ("MIN_REAL")
      ELSE
        Reals.GiveFloat (str, val, Reals.maxSigFigs, 0, Conv.left);
        i := 0;
        WHILE (str[i] # 0X) & (str[i] # "E") & (str[i] # ".") DO
          INC (i)
        END;
        IF (str[i] # ".") THEN
          Str.Insert (".0", i, str)
        END;
        O.String (str);
        IF ~long THEN
          O.Char ("F")
        END
      END
    END RealConst;

  PROCEDURE SetConst (set : SET);
  (* Writes the set constant as a long int, hexadecimal number. *)
    VAR
      i, j, nibble, bit : INTEGER;
    BEGIN
      O.String ("0x");
      FOR i := MAX(SET)-3 TO MIN(SET) BY -4 DO
        nibble := 0;
        bit := 1;
        j := 0;
        WHILE (j < 4) DO
          IF (i+j IN set) THEN
            INC (nibble, bit)
          END;
          bit := bit*2;
          INC (j)
        END;
        IF (nibble < 10) THEN
          O.Char (CHR (ORD("0") + nibble))
        ELSE
          O.Char (CHR (ORD("A") - 10 + nibble))
        END
      END;
      O.Char ("U")
    END SetConst;

  BEGIN
    IF (obj # NIL) & ~(T.flagExternal IN obj. flags) &
       ~((obj. type. form = T.strString) & 
         (Str.Length (obj. const. string^) = 1)) THEN  (* named constant *)
      Ident (obj)
    ELSE
      CASE form OF
      T.strBool:
        IF (c. intval=1) THEN
          O.String ("TRUE")
        ELSE
          O.String ("FALSE")
        END |
      T.strChar:
        CharConst (c. intval, TRUE) |
      T.strShortInt..T.strLongInt:
        IF (c. intval = -MAX(LONGINT)-1) THEN
          (* this way we won't draw a warning since MAX(LONGINT) < -MIN(LONGINT) *)
          O.String ("(-");
          O.Int (MAX (LONGINT), 0);  (* largest writable positive integer *)
          O.String ("L-1)")
        ELSE
          O.Int (c. intval, 0)
        END |
      T.strReal, T.strLongReal:
        RealConst (c. real, form=T.strLongReal) |
      T.strSet:
        SetConst (c. set) |
      T.strString:
        O.Char ('"');
        IF (Str.Length (c. string^) = 1) THEN
          CharConst (ORD (c. string[0]), FALSE)
        ELSE
          i := 0;
          WHILE (c. string[i] # 0X) DO
            IF (c. string[i] = '"') OR     (* avoid the string delimiter *)
               (c. string[i] = "\") OR     (* to get the escape character it has to be escaped *)
               (c. string[i] = "?") THEN   (* don't create a trigraph by accident (whatever this is) *)
              O.Char ("\")
            END;
            O.Char (c. string[i]);
            INC (i)
          END
        END;
        O.Char ('"') |
      T.strNil:
        O.String ("NULL")
      END
    END
  END Const;

PROCEDURE Decl (obj : T.Object; flags : SET; off : INTEGER) : T.Object;
(* Prints declaration, including modifiers depending on current file (.c or .h).
   Returns the next declaration in the declaration list. *)
  CONST
    dontPrint = 0;                       (* do not generate declaration (at this place) *)
    extern = 1;                          (* print declaration, put prefix 'extern' in front of it *)
    static = 2;                          (* prefix 'static' *)
    auto = 3;                            (* prefix 'static' *)
    noModifier = 4;                      (* print declaration without any special prefix *)
  VAR
    next : T.Object;
    modifier : SHORTINT;
  BEGIN
    (* Is this declaration global or local to a procedure?
       Is the declaration to be written into the current file?
       If yes, what modifier has to be used? *)
    IF T.external & (obj. mode = T.objConst) THEN
      modifier := dontPrint
    ELSIF (flagAutoPrefix IN flags) THEN    
      modifier := auto
    ELSIF (obj. mnolev > T.compileMnolev) THEN
      modifier := noModifier
    ELSE
      IF (obj. mode IN {T.objExtProc, T.objIntProc, T.objLocalProc, T.objTBProc, T.objVar, T.objConst}) THEN
        (* function/variable/constant declaration *)
        IF ~(T.flagExport IN obj. flags) THEN
          IF (flagHeader IN flags) THEN
            modifier := dontPrint
          ELSE
            modifier := static
          END
        ELSE
          IF (flagHeader IN flags) THEN
            modifier := extern
          ELSE
            modifier := noModifier
          END
        END
      ELSE  (* type declaration *)
        IF ((T.flagExport IN obj. flags) # (flagHeader IN flags)) THEN
          modifier := dontPrint
        ELSE
          modifier := noModifier
        END
      END
    END;
    IF (modifier # dontPrint) THEN
      IF (obj. mode IN {T.objExtProc, T.objIntProc, T.objLocalProc, T.objTBProc}) &
         ~T.external THEN
        (* write type descriptors after declarations, but before the procedures *)
        GenTypeDescr (root. link, flagHeader IN flags) 
      END;
      Off (off);
      IF (modifier = extern) THEN
        O.String ("extern ")
      ELSIF (modifier = static) THEN
        O.String ("static ")
      ELSIF (modifier = auto) THEN
        O.String ("auto ")
      END;
      IF (obj. mode=T.objConst) THEN     (* constant declaration *)
        O.String ("const ");
        next := NamedDecl (obj, {}, off);
        IF ~(flagHeader IN flags) THEN   (* add initializing part (.c file only) *)
          O.String (" = ");
          Const (obj. const, obj. type. form, NIL)
        END
      ELSIF (obj. mode=T.objType) THEN   (* type declaration *)
        O.String ("typedef ");
        next := NamedDecl (obj, flags+{flagTypeDef}, off);
        INCL (obj. flags, ofDefined)
      ELSIF (obj. mode=T.objVar) THEN    (* variable declaration *)
        next := NamedDecl (obj, flags+{flagVarDef}, off)
      ELSE                               (* procedure declaration *)
        next := NamedDecl (obj, flags+{flagFunctHeader}, off)
      END;
      IF (flagSemicolon IN flags) THEN
        O.Char (";")
      END;
      RETURN next
    ELSE
      RETURN obj. next
    END
  END Decl;

PROCEDURE VarDecl (name : ARRAY OF CHAR; type : T.Struct);
  VAR
    dummy : T.Object;
  BEGIN
    IF (type. obj # NIL) THEN
      Ident (type. obj);
      O.Char (" ");
      O.String (name)
    ELSE
      emptyObj. type := type;
      COPY (name, extName);
      dummy := NamedDecl (emptyObj, {flagExtName}, MIN(INTEGER))
    END
  END VarDecl;
  
  

PROCEDURE ^ Expr (expr : E.Node; paren : BOOLEAN);

PROCEDURE ^ Designator (d : E.Node; adr : BOOLEAN);

PROCEDURE ArrayLength (desig : E.Node; dim : LONGINT);
(* Write length of array 'desig' in dimension 'dim'. *)
  VAR
    ddim : INTEGER;
    type : T.Struct;
  BEGIN
    ddim := DynDimensions (desig. type);
    IF (ddim <= dim) THEN
      (* get type associated with dimension 'dim' *)
      type := desig. type;
      WHILE (dim # 0) DO
        type := type. base;
        DEC (dim)
      END;
      O.Int (type. len, 0)
    ELSIF (desig. class = E.ndVarPar) OR (desig. class = E.ndVar) THEN
      (* LEN on dynamic open array parameter *)
      O.String ("_");
      Ident (desig. obj);
      O.Char ("_");
      O.Int (dim, 0)
    ELSE
      (* LEN on dynamic pointer base type *)
      O.String ("GET_LEN(");
      WHILE (desig. type. form = T.strDynArray) DO
        desig := desig. left
      END;
      Designator (desig, FALSE);
      O.String (", ");
      O.Int (dim, 0);
      O.Char (")")
    END
  END ArrayLength;

PROCEDURE GetArrayPrefix (d : E.Node; VAR prefix : E.Node; VAR dim : INTEGER);
(* pre: 'd' is a designator.
   post: 'prefix' holds the designator 'd' stripped of all leading index 
     selectors, i.e. it holds the array designator to which the current 'd' is
     an index.  'dim' is the dimension of 'pref' which 'd' is refering to.
     If 'd' is no designator at all then d=prefix, dim=-1. *)
  BEGIN
    dim := -1;
    prefix := d;
    WHILE (prefix. class = E.ndIndex) DO
      INC (dim);
      prefix := prefix. left
    END
  END GetArrayPrefix;

PROCEDURE GenTypeTag (n : E.Node);
(* pre: 'n' denotes a record or a pointer *)
  BEGIN
    IF (n. type. form = T.strPointer) THEN  (* pointer *)
      O.String ("TAG(");
      TypeDescrIdent (n. type. base, FALSE);
      O.String (", ");
      Designator (n, FALSE);
      O.String (", ");
      O.Int (n. pos, 0);
      O.Char (")")
    ELSIF (n. class = E.ndVarPar) THEN  (* variable record parameter *)
      GenTagName (n. obj)
    ELSIF (n. class = E.ndDeref) THEN  (* record, derived by dereferenciation *)
      O.String ("TAG(");
      TypeDescrIdent (n. type, FALSE);
      O.String (", ");
      Designator (n. left, FALSE);
      O.String (", ");
      O.Int (n. pos, 0);
      O.Char (")")
    ELSE  (* static record *)
      TypeDescrIdent (n. type, TRUE)
    END
  END GenTypeTag;

PROCEDURE GenTypeLevel (type : T.Struct);
  VAR
    level : INTEGER;
  BEGIN
    O.String (", ");
    (* determine extension level of 'type' *)
    IF (type. form = T.strPointer) THEN
      type := type. base
    END;
    level := -1;
    REPEAT
      INC (level);
      type := type. base
    UNTIL (type = NIL);
    O.Int (level, 0)    
  END GenTypeLevel;


PROCEDURE GenTypeTest (expr : E.Node; type : T.Struct);
  BEGIN
    IF (expr. type. form = T.strPointer) THEN
      O.String ("type_test_ptr(");
      Expr (expr, FALSE)
    ELSE
      O.String ("type_test(");
      GenTypeTag (expr)
    END;
    O.String (", ");
    TypeDescrIdent (type, TRUE);
    GenTypeLevel (type);
    IF (expr. type. form = T.strPointer) THEN
      O.String (", moduleId, ");
      O.Int (expr. pos, 0)
    END;
    O.Char (")")
  END GenTypeTest;


PROCEDURE TypeDesign (type : T.Struct; arrayToPtr : BOOLEAN);
  VAR
    dummy : T.Object;
    toPointer : BOOLEAN;
  BEGIN
    toPointer := arrayToPtr & (type. form IN arraySet);
    WHILE (type. form IN arraySet) DO
      type := type. base
    END;
    IF toPointer THEN
      O.Char ("(")
    END;
    IF (type. obj # NIL) THEN
      Ident (type. obj)
    ELSE
      emptyObj. type := type;
      dummy := NamedDecl (emptyObj, {flagNoName}, MIN(INTEGER))
    END;
    IF toPointer THEN
      O.String ("*)")
    END
  END TypeDesign;
  
PROCEDURE Designator (d : E.Node; adr : BOOLEAN);
  VAR
    prefix : E.Node;
    type : T.Struct;
    dim : INTEGER;

  PROCEDURE AdrPrefix() : BOOLEAN;
    BEGIN
      RETURN adr & (d. class # E.ndDeref) & (d. class # E.ndVarPar) & 
             (d. type. form # T.strDynArray) & 
             ~((d. class = E.ndIndex) & (d. left. type. form = T.strDynArray))
    END AdrPrefix;

  PROCEDURE FindField (t : T.Struct; obj : T.Object) : BOOLEAN;
    VAR
      o : T.Object;
    BEGIN
      o := t. link;
      WHILE (o # NIL) DO
        IF (o = obj) THEN
          RETURN TRUE
        END;
        o := o. next
      END;
      RETURN FALSE
    END FindField;

  PROCEDURE DesignRG;
    BEGIN
      IF ((d. class = E.ndVar) OR (d. class = E.ndVarPar)) & (d. obj. type # d. type) THEN
        (* variable is subject to a regional type guard 
           (guaranteed to be a variable without any selectors) *)
        O.String ("((");
        TypeDesign (d. type, FALSE);
        IF (d. class = E.ndVarPar) THEN
          O.Char ("*")
        END;
        O.Char (")");
        Ident (d. obj);
        O.Char (")")
      ELSE
        Ident (d. obj)
      END
    END DesignRG;
    
  BEGIN
    IF (d # NIL) THEN
      IF AdrPrefix() THEN
        O.String ("&(")
      END;
      IF (d. class = E.ndDeref) THEN
        IF adr OR (d. type. form = T.strDynArray) THEN
          O.String ("CHECK_NIL(")
        ELSE
          O.String ("DEREF(")
        END;
        TypeDesign (d. left. type, TRUE);
        O.String (", ");
        Designator (d. left, FALSE);
        O.String (", ");
        O.Int (d. pos, 0);
        O.Char (")")
      ELSIF (d. class = E.ndIndex) THEN
        GetArrayPrefix (d, prefix, dim);
        (* generate index selector *)
        IF (d. left. type. form = T.strArray) THEN (* normal array index *)
          O.String ("INDEX(");
          Designator (d. left, FALSE);
          O.String (", ");
          Expr (d. right, FALSE);
          O.String (", ");
          ArrayLength (prefix, dim);
          O.String (", ");
          O.Int (d. pos, 0);
          O.Char (")")
        ELSE                             (* open array index *)
          IF ~adr THEN
            O.String ("(* ")
          END;
          IF (T.flagExternal IN d. left. type. flags) THEN
            (* external open array, no length information available *)
            O.String ("PTR_INDEX_EXT(");
            Designator (d. left, TRUE);
            O.String (", ");
            Expr (d. right, FALSE)
          ELSE  (* normal open arry, length information available *)
            O.String ("PTR_INDEX(");
            Designator (d. left, TRUE);
            O.String (", ");
            Expr (d. right, FALSE);
            O.String (", ");
            ArrayLength (prefix, dim);
            O.String (", ");
            O.Char ("1");
            type := d. type;
            WHILE (type. form = T.strDynArray) DO
              INC (dim);
              O.Char ("*");
              ArrayLength (prefix, dim);
              type := type. base
            END;
            O.String (", ");
            O.Int (d. pos, 0)
          END;
          O.Char (")");
          IF ~adr THEN
            O.Char (")")
          END
        END
      ELSIF (d. class = E.ndGuard) THEN
        IF (d. type. form = T.strRecord) THEN
          O.String ("REC")
        ELSE
          O.String ("PTR")
        END;
        O.String ("_TYPE_GUARD(");
        TypeDesign (d. type, FALSE);
        O.String (", ");
        Designator (d. left, d. type. form = T.strRecord);
        IF (d. type. form = T.strRecord) THEN
          O.String (", ");
          GenTypeTag (d. left)
        END;
        O.String (", ");
        TypeDescrIdent (d. type, TRUE);
        GenTypeLevel (d. type);
        O.String (", ");
        O.Int (d. pos, 0);
        O.Char (")")
      ELSIF (d. class = E.ndTBSuper) THEN (* super call of type bound procedure *)
        IF (d. left. type. form = T.strPointer) THEN
          TypeDescrIdent (d. left. type. base. base, TRUE)
        ELSE
          TypeDescrIdent (d. left. type. base, TRUE)
        END;
        O.String ("->tb_");
        O.Int (TypeDescId (d. obj), 0)
      ELSIF (d. class = E.ndTBProc) THEN (* super call of type bound procedure *)
        GenTypeTag (d. left);
        O.String ("->tb_");
        O.Int (TypeDescId (d. obj), 0)
      ELSE
        Designator (d. left, FALSE);
        CASE d. class OF
        E.ndVar:
          DesignRG |
        E.ndConst, E.ndType, E.ndProc:
          Ident (d. obj) |
        E.ndVarPar:
          IF adr OR (d. type. form IN arraySet) THEN
            DesignRG                     (* no dereferenciation for variable array parameters *)
          ELSE                           (* 'normal' parameter, passed as pointer: do a deref on it *)
            O.String ("(* ");
            DesignRG;
            O.Char (")")
          END |
        E.ndField:
          type := d. left. type;
          WHILE ~FindField (type, d. obj) DO
            O.String (".base");
            type := type. base
          END;
          O.String (".");
          IF (T.flagExternal IN d. left. type. flags) THEN
            (* omit leading _ when writing external record fields *)
            O.String (d. obj. name)
          ELSE
            Ident (d. obj)
          END
        END
      END;
      IF AdrPrefix() THEN
        O.Char (")")
      END;
    END
  END Designator;



PROCEDURE Projection (expr : E.Node; fType : T.Struct);
(* pre: 'expr' is a expression whose type extends 'fType' *)
  VAR
    type : T.Struct;
  BEGIN
    IF (expr. type = fType) THEN
      Expr (expr, FALSE)
    ELSIF (fType. form = T.strPointer) THEN
      O.Char ("(");
      TypeDesign (fType, FALSE);
      O.Char (")");
      Expr (expr, FALSE)
    ELSE  (* fType. form = T.strRecord *)
      Expr (expr, FALSE);
      type := expr. type;
      REPEAT
        O.String (".base");
        type := type. base
      UNTIL (type = fType)
    END
  END Projection;
  
PROCEDURE ProcCall (call : E.Node);
  VAR
    apar, prefix : E.Node;
    fpar : T.Object;
    base, t : T.Struct;    
    i, dim : INTEGER;
    tbCall : BOOLEAN;
  BEGIN
    apar := call. right;                 (* actual parameter *)
    fpar := call. left. type. link;      (* formal parameter *)
    tbCall := (call. left. class = E.ndTBSuper) OR (call. left. class = E.ndTBProc);
    IF tbCall THEN
      (* set call.left to the most recent definition of the tb proc since there
         may follow a new definition further down in the source code *)
      t := call. left. left. type;
      IF (t. form = T.strPointer) THEN
        t := t. base
      END;
      call. left. obj := T.FindField (call. left. obj. name, t);
      call. left. type := call. left. obj. type;
      fpar := call. left. type. link;    (* formal parameter *)
      (* add receiver to the actual parameter list *)
      call. left. left. link := apar;
      apar := call. left. left;
      IF (call. left. class = E.ndTBSuper) THEN
        (* super call, adjust formal parameter list to the type bound procedure being used *)
        fpar := T.FindField (call. left. obj. name, t. base);
        fpar := fpar. type. link
      END
    END;
    (* write function designator and the parameter list *)
    Designator (call. left, FALSE);
    O.Char ("(");
    WHILE (apar # NIL) DO
      IF (fpar. type. form = T.strDynArray) & (fpar. type. base. form = T.strSysByte) THEN
        (* something is passed to an open array of SYSTEM.BYTE: calculate 
           size in bytes *)
        IF (T.flagExternal IN (fpar. type. flags+call. left. type. flags)) THEN
          (* suppress length information when passing to EXTERNAL functions *)
          O.String ("(BYTE*)")
        ELSE  (* pass length(s) in front of the pointer to the array *)
          IF (apar. type. form = T.strString) THEN
            O.Int (Str.Length (apar. conval. string^)+1, 0);
          ELSIF (apar. type. form = T.strDynArray) &
                (T.flagExternal IN apar. type. flags) THEN
            (* no length information available *)
            O.Int (M.maxLInt, 0)
          ELSE
            O.String ("sizeof(");
            TypeDesign (apar. type, FALSE);
            O.Char (")");
            IF (apar. type. form IN arraySet) THEN
              t := apar. type;
              GetArrayPrefix (apar, prefix, dim);
              WHILE (t. form IN arraySet) DO
                INC (dim);
                O.Char ("*");
                ArrayLength (prefix, dim);
                t := t. base
              END
            END
          END;
          O.String (", (BYTE*)")
        END;
        Designator (apar, TRUE)
      ELSIF (fpar. type. form IN arraySet) THEN (* array parameter *)
        IF ~(T.flagExternal IN (fpar. type. flags+call. left. type. flags)) & 
           (fpar. type. form = T.strDynArray) THEN
          IF (apar. type. form = T.strString) THEN
            O.Int (Str.Length (apar. conval. string^)+1, 0);
            O.String (", ")
          ELSIF (apar. type. form = T.strDynArray) &
                (T.flagExternal IN apar. type. flags) THEN
            (* no length information available *)
            O.Int (M.maxLInt, 0);
            O.String (", ")
          ELSE
            GetArrayPrefix (apar, prefix, dim);
            FOR i := 1 TO DynDimensions (fpar. type) DO
              ArrayLength (prefix, i+dim);
              O.String (", ");
            END
          END
        END;
        IF (fpar. type. form = T.strDynArray) & (apar. type. form = T.strDynArray) THEN
          Designator (apar, TRUE)
        ELSIF (fpar. type. form = T.strDynArray) THEN
          (* formal parameter is a dynamic array translated into a pointer type *)
          base := fpar. type;
          WHILE (base. form = T.strDynArray) DO
            base := base. base
          END;
          (* write type cast to the array's base type *)
          O.Char ("(");
          Ident (base. obj);
          O.String (" *) ");
          Expr (apar, FALSE)
        ELSIF (apar. type. form = T.strString) & (fpar. type. obj # NIL) THEN
          (* string passed to a named array of char *)
          O.String ("(void*)");
          Expr (apar, FALSE) 
        ELSE
          Expr (apar, FALSE)
        END
      ELSIF (fpar. mode = T.objVarPar) THEN (* variable parameter *)
        IF ~(T.flagExternal IN (fpar. type. flags+call. left. type. flags)) & 
           (apar. type. form = T.strRecord) THEN (* VAR record, add type tag *)
          O.String ("(void*)");
          GenTypeTag (apar);
          O.String (", ");
          IF (apar. type # fpar. type) THEN
            O.String ("(");
            TypeDesign (fpar. type, FALSE);
            O.String ("*)")
          END;
          Designator (apar, TRUE)
        ELSIF (apar. type. form = T.strPointer) & 
              (apar. class = E.ndGuard) THEN
          (* type guard on pointer variable passed to VAR parameter *)
          O.String ("(");
          Designator (apar, FALSE);
          O.String (", (void*)(&(");
          Designator (apar. left, FALSE);
          O.String (")))")
        ELSIF (fpar. type # apar. type) & (fpar. type. form = T.strSysPtr) THEN
          (* passing a pointer type to a variable parameter of type PTR *)
          O.String ("(PTR*)");
          Designator (apar, TRUE)
        ELSE
          Designator (apar, TRUE)
        END;
      ELSE                             (* value parameter *)
        IF (fpar. type. form IN {T.strRecord, T.strPointer}) & (apar. type # fpar. type) THEN
          Projection (apar, fpar. type)
        ELSE
          Expr (apar, FALSE)
        END
      END;
      apar := apar. link;
      IF (fpar. type. form # T.strNone) THEN
        fpar := fpar. link
      END;
      IF (apar # NIL) THEN
        O.String (", ")
      END
    END;    
    O.Char (")")
  END ProcCall;

PROCEDURE WriteOperation (n : E.Node; oc : ARRAY OF CHAR; flags : SET; pe, pl, pr : SHORTINT);
(* Write a mondic or dyadic expression, either as an operation or a macro.
   pre: 'n' is the node that represents the expression, 'oc' the ASCII 
     representation of the node's operation code (an identifier or an 
     operator). 'flags' is a set of modifiers of the following meaning
       fAddType: add shurtcut for the type of n.left to the 'oc' (only if 'oc'
         is an ident)
       fAddPosition: add module id and file position to the macro call (only if
         'oc' is an ident)
       fMonadic: signals that 'n' is a monadic expression.
       fAddTypename: append type name of left operand (pre: n.left.type.obj # 
         NIL)
       fLeftAdr: Pass the first parameter (has to be a designator) as address.
       fAddSize: add sizeof(left_side) to the macro call
     pe, pl, pr contain the priority levels of the expressions in n, n.left, 
     n.right.
   post: The operation (or the macro) is written with n.left resp. n.right as 
     its left and right hand side.
     Additional information is written according to the value in 'flags'. *)
  VAR
    app : ARRAY 4 OF CHAR;
  BEGIN
    IF ("A" <= CAP (oc[0])) & (CAP (oc[0]) <= "Z") THEN (* macro call *)
      O.String (oc);
      IF (fAddType IN flags) THEN
        CASE n. left. type. form OF
        | T.strShortInt: app := "SI"
        | T.strInteger: app := "I"
        | T.strLongInt: app := "LI"
        | T.strReal: app := "R"
        | T.strLongReal: app := "LR"
        | T.strChar: app := "C"
        | T.strSysByte: app := "B"
        END;
        O.String (app)
      END;
      O.Char ("(");
      IF (fLeftAdr IN flags) THEN
        Designator (n. left, TRUE)
      ELSE
        Expr (n. left, FALSE)
      END;
      IF ~(fMonadic IN flags) THEN
        O.String (", ");
        Expr (n. right, FALSE)
      END;
      IF (fAddPosition IN flags) THEN
        O.String (", ");
        O.Int (n. pos, 0)
      END;
      IF (fAddTypename IN flags) THEN
        O.String (", ");
        Ident (n. left. type. obj)
      END;
      IF (fAddSize IN flags) THEN
        O.String (", sizeof(");
        Designator (n. left, FALSE);
        O.Char (")")
      END;
      O.Char (")")
    ELSE                               (* simple operator *)
      IF (fMonadic IN flags) THEN
        O.String (oc);
        Expr (n. left, pe <= pl)
      ELSE
        IF (n. left. type. form IN {T.strPointer, T.strSysPtr}) &
           (n. left. type # n. right. type) THEN
          O.String ("(void*)");
          Expr (n. left, pe <= pl);
          O.String (oc);
          O.String ("(void*)");
          Expr (n. right, pe <= pr)
        ELSE
          Expr (n. left, pe <= pl);
          O.String (oc);
          Expr (n. right, pe <= pr)
        END
      END
    END
  END WriteOperation;

PROCEDURE Expr (expr : E.Node; paren : BOOLEAN);
  VAR
    str : ARRAY 8 OF CHAR;
    flags : SET;
    pe, pr, pl : SHORTINT;

  PROCEDURE Pri (expr : E.Node) : SHORTINT;
    BEGIN
      IF (expr=NIL) THEN
        RETURN MIN(SHORTINT)
      ELSIF (expr. class=E.ndConst) &
            ((expr. type. form IN E.intSet) & (expr. conval. intval<0) OR
             (expr. type. form IN E.realSet) & (expr. conval. real<0)) THEN
        RETURN 2
      ELSIF (expr. class<E.ndMOp) THEN  (* atomic (designator and ndUpto) *)
        RETURN 0
      ELSE
        CASE expr. subcl OF
        E.scNot, E.scConv, E.scAdr:
          RETURN 2 |
        E.scTimes..E.scMod:
          IF (expr. type. form = T.strSet) THEN
            CASE expr. subcl OF
            | E.scTimes: RETURN 8 |
            | E.scRDiv: RETURN 9
            END
          ELSE
            RETURN 3
          END |
        E.scPlus:
          IF (expr. type. form = T.strSet) THEN
            RETURN 10
          ELSE
            RETURN 4
          END |
        E.scMinus:
          IF (expr. type. form = T.strSet) THEN
            (* set difference, reduced to intersection by macro expension *)
            RETURN 9
          ELSE
            RETURN 4
          END |
        E.scLss..E.scGeq:
          RETURN 6 |
        E.scEql, E.scNeq:
          RETURN 7 |
        E.scAnd:
          RETURN 11 |
        E.scOr:
          RETURN 11 |  (* actually it is 12. this way additional parantheses are enforced. *)
        ELSE  (* predefined functions, parenthised expressions etc. *)
          RETURN 0
        END
      END
    END Pri;

  PROCEDURE String (n : E.Node) : BOOLEAN;
  (* Result is TRUE iff n is a string constant or an array of characters. *)
    BEGIN
      RETURN (n. type. form = T.strString) OR 
             (n. type. form IN arraySet) & (n. type. base. form=T.strChar)
    END String;

  BEGIN
    IF paren THEN
      O.Char ("(")
    END;
    pe := Pri (expr);
    pl := Pri (expr. left);
    pr := Pri (expr. right);
    IF (expr. class < E.ndConst) THEN
      Designator (expr, FALSE)
    ELSIF (expr. class = E.ndConst) THEN
      Const (expr. conval, expr. type. form, expr. obj)
    ELSIF (expr. class = E.ndUpto) THEN
      O.String ("RANGE(");
      Expr (expr. left, FALSE);
      O.String (", ");
      O.Int (expr. left. pos, 0);
      O.String (", ");
      Expr (expr. right, FALSE);
      O.String (", ");
      O.Int (expr. right. pos, 0);
      O.Char (")")
    ELSIF (expr. class = E.ndMOp) & (expr. subcl = E.scConv) THEN
      (* type conversion: just do a type cast *)
      O.String ("(");
      Ident (expr. type. obj);
      O.String (") ");
      Expr (expr. left, pl>=2)
    ELSIF (expr. class = E.ndMOp) & (expr. subcl = E.scVal) THEN
      (* SYSTEM.VAL: a type cast wouldn't do, since this includes a type
         conversion for some C types and isn't allowed between all C types *)
      IF (expr. left. class < E.ndConst) THEN (* designator *)
        O.String ("(*((");
        Ident (expr. type. obj);
        O.String ("*)&");
        Expr (expr. left, TRUE);
        O.String ("))")
      ELSE                               (* expression *)
        O.String ("VAL_E(");
        Ident (expr. type. obj);
        O.String (", ");
        Expr (expr. left, TRUE);
        O.Char (")")
      END
    ELSIF (expr. class = E.ndMOp) & (expr. subcl = E.scAdr) THEN
      O.String ("(LONGINT)");
      Designator (expr. left, TRUE)
    ELSIF (expr. class = E.ndMOp) THEN
      flags := {};
      CASE expr. subcl OF
      | E.scMinus:
        IF (expr. type. form # T.strSet) THEN
          str := "-"  (* negation *)
        ELSE
          str := "~"  (* bit complement *)
        END |
      | E.scNot: str := "!"
      | E.scAbs: str := "ABS"; flags := {fAddType, fAddPosition}
      | E.scCap: str := "CAP"
      | E.scOdd: str := "ODD"; flags := {fAddType}
      | E.scSize: str := "sizeof"
      END;
      WriteOperation (expr, str, flags+{fMonadic}, pe, pl, pr)
    ELSIF (expr. class = E.ndDOp) & (expr. subcl = E.scLen) THEN
      ArrayLength (expr. left, expr. right. conval. intval)
    ELSIF (expr. class = E.ndDOp) & (expr. subcl = E.scIs) THEN   (* type test *)
      GenTypeTest (expr. left, expr. right. type)
    ELSIF (expr. class = E.ndDOp) THEN
      flags := {};
      IF String (expr. left) THEN  (* string comparison *)
        CASE expr. subcl OF
        | E.scEql: str := "EQL"
        | E.scNeq: str := "NEQ"
        | E.scLss: str := "LSS"
        | E.scLeq: str := "LEQ"
        | E.scGrt: str := "GRT"
        | E.scGeq: str := "GEQ"
        END;
        Str.Insert ("STR", 0, str)
      ELSIF (expr. type. form = T.strSet) THEN  (* set arithmetics *)
        IF (expr. subcl = E.scMinus) THEN
          str := "SETDIFF"
        ELSE
          CASE expr. subcl OF
          | E.scPlus: str := "|"
          | E.scTimes: str := "&"
          | E.scRDiv: str := "^"
          END
        END
      ELSE
        CASE expr. subcl OF
        | E.scPlus: str := "+"
        | E.scMinus: str := "- "
        | E.scTimes: str := "*"
        | E.scIDiv: str := "DIV"; flags := {fAddType, fAddPosition}
        | E.scMod: str := "MOD"; flags := {fAddType, fAddPosition}
        | E.scRDiv: str := "DIV"; flags := {fAddType, fAddPosition}
        | E.scEql: str := "=="
        | E.scNeq: str := "!="
        | E.scLss: str := "<"
        | E.scLeq: str := "<="
        | E.scGrt: str := ">"
        | E.scGeq: str := ">="
        | E.scAnd: str := "&&";
        | E.scOr: str := "||"
        | E.scIn: str := "IN"; flags := {fAddPosition}
        | E.scAsh: str := "ASH"; flags := {fAddType, fAddPosition}
        | E.scBit: str := "BIT"; flags := {}
        | E.scLsh: str := "LSH"; flags := {fAddType}
        | E.scRot: str := "ROT"; flags := {fAddType}
        END
      END;
      WriteOperation (expr, str, flags, pe, pl, pr)
    ELSIF (expr. class=E.ndCall) THEN
      ProcCall (expr)
    ELSE
      HALT (101)
    END;
    IF paren THEN
      O.Char (")")
    END
  END Expr;

PROCEDURE AppendAtomicFlag (t: T.Struct);
(* If type `t' contains a pointer, write ", 1)", else ", 0)".  Used when 
   emitting any NEW statements to distrinct between blocks that (may) contain
   pointers and atomic ones that are guarenteed to contain none. *)
  BEGIN
    IF T.ContainsPointer (t) THEN
      O.String (", 0)")
    ELSE 
      O.String (", 1)")
    END
  END AppendAtomicFlag;

PROCEDURE StatementSeq (s : E.Node; off : INTEGER; paren : BOOLEAN);
  VAR
    n, m : E.Node;
    str : ARRAY 16 OF CHAR;
    flags : SET;
    dim : INTEGER;
    t : T.Struct;
    
  PROCEDURE ExtendedAssignment (l, r : E.Node);
    BEGIN
      IF (l. type. form = T.strRecord) & ((l. class = E.ndVarPar) OR (l. class = E.ndDeref)) THEN
        (* assert that the lhs dynamic type is the same as the static type *)
        O.String ("ASSERT_TYPE(");
        IF (l. class = E.ndVarPar) THEN
          GenTagName (l. obj)
        ELSE (* (l. class = E.ndDeref) *)
          O.String ("TAG(");
          TypeDescrIdent (l. type, FALSE);
          O.String (", ");
          Designator (l. left, FALSE);
          O.String (", ");
          O.Int (l. pos, 0);
          O.Char (")")
        END;
        O.String (", ");
        TypeDescrIdent (l. type, TRUE);
        O.String (", ");
        O.Int (l. pos, 0);
        O.String (");");
        Off (off)
      END;
      Designator (l, FALSE);
      O.String (" = ");
      Projection (r, l. type)
    END ExtendedAssignment;
    
  PROCEDURE CaseLabel (l : E.Node; form : SHORTINT);
    VAR
      b : LONGINT;
    BEGIN
      Const (l. conval, form, NIL);
      IF (l. conval. intval#l. conval. intval2) THEN
        O.String (" ... ");
        b := l. conval. intval;
        l. conval. intval := l. conval. intval2;
        Const (l. conval, form, NIL);
        l. conval. intval := b
      END
    END CaseLabel;

  BEGIN
    IF paren THEN
      O.String (" {")
    END;
    WHILE (s#NIL) DO
      Off (off);
      CASE s. class OF
      E.ndAssign:
        IF (s. subcl=E.scMove) THEN
          O.String ("MOVE(");
          Expr (s. right, FALSE);
          O.String (", ");
          Expr (s. right. link, FALSE);
          O.String (", ");
          Expr (s. right. link. link, FALSE);
          O.Char (")")
        ELSIF (s. subcl=E.scNewFix) THEN
          IF (s. left. type. base. form = T.strRecord) &
             ~(T.flagExternal IN s. left. type. base. flags) THEN  (* pointer to Oberon-2 record *)
            O.String ("NEWREC(");
            Expr (s. left, FALSE);
            O.String (", ");
            TypeDescrIdent (s. left. type. base, TRUE)
          ELSE  (* pointer to fix size array or to external record/union *)
            O.String ("NEWFIX(");
            Expr (s. left, FALSE)
          END;
          AppendAtomicFlag (s. left. type. base)
        ELSIF (s. subcl=E.scNewDyn) THEN
          O.String ("NEWDYN");
          IF (T.flagExternal IN s. left. type. base. flags) THEN
            (* external open array *)
            O.String ("_EXT")
          END;
          O.Char ("(");
          Designator (s. left, FALSE);
          O.String (", 1");
          dim := 0;
          n := s. right;
          WHILE (n # NIL) DO
            O.String ("*");
            Expr (n, FALSE);
            INC (dim);
            n := n. link
          END;
          O.String (", ");
          O.Int (dim, 0);
          AppendAtomicFlag (s. left. type. base);
          IF ~(T.flagExternal IN s. left. type. base. flags) THEN
            (* set array length for each dimension *)
            dim := 0;
            n := s. right;
            WHILE (n # NIL) DO
              O.Char (";");
              Off (off);
              O.String ("SET_LEN(");
              O.Int (dim, 0);
              O.String (", ");
              Expr (n, FALSE);
              O.Char (")");
              INC (dim);
              n := n. link
            END
          END
        ELSIF (s. subcl=E.scDispose) THEN
          O.String ("DISPOSE");
          IF (s. left. type. form = T.strSysPtr) OR
             (T.flagExternal IN s. left. type. base. flags) THEN
            (* unstructured pointer or an external type is freed *)
          ELSIF (s. left. type. base. form = T.strRecord) THEN
            O.String ("_REC")
          ELSIF (s. left. type. base. form = T.strDynArray) THEN
            O.String ("_DYN")
          END;
          O.Char ("(");
          Designator (s. left, FALSE);
          IF (s. left. type. form = T.strPointer) & 
             (s. left. type. base. form = T.strDynArray) & 
             ~(T.flagExternal IN s. left. type. base. flags) THEN
            O.String (", ");
            dim := 0;
            t := s. left. type. base;
            WHILE (t. form = T.strDynArray) DO
              INC (dim);
              t := t. base
            END;
            O.Int (dim, 0)
          END;
          O.Char (")")
        ELSIF (s. subcl=E.scCollect) THEN
          O.String ("GC_COLLECT")
        ELSIF (s. subcl = E.scCopy) THEN
          O.String ("COPY(");            
          Expr (s. left, FALSE);         (* source *)
          O.String (", ");
          Expr (s. right, FALSE);        (* target *)
          O.String (", ");
          ArrayLength (s. right, 0);     (* target length *)
          O.Char (")")
        ELSIF (s. subcl = E.scAssign) & (s. left. type. form IN {T.strPointer, T.strRecord}) THEN
          ExtendedAssignment (s. left, s. right)
        ELSIF (s. subcl = E.scGet) OR (s. subcl = E.scPut) THEN
          IF (s. subcl = E.scGet) THEN
            O.String ("GET(")
          ELSE
            O.String ("PUT(")
          END;
          Expr (s. left, FALSE);
          O.String (", ");
          Expr (s. right, FALSE);
          O.String (", ");
          IF (s. right. type. form IN {T.strPointer, T.strProc}) THEN
            O.String ("void*")
          ELSE
            Ident (s. right. type. obj)
          END;
          O.Char (")")
        ELSE
          flags := {};
          CASE s. subcl OF
            E.scAssign:
              IF (s. right. type. form = T.strString) THEN
                str := "COPYSTRING"; flags := {fLeftAdr}
              ELSIF (s. left. type. form IN arraySet) THEN
                IF (s. left. type. obj # NIL) THEN
                  str := "COPYARRAYT"; flags := {fLeftAdr, fAddTypename}
                ELSE
                  str := "COPYARRAY"; flags := {fLeftAdr, fAddSize}
                END
              ELSE
                str := " = "
              END |
            E.scInc: str := "INC"; flags := {fAddType, fAddPosition} |
            E.scDec: str := "DEC"; flags := {fAddType, fAddPosition} |
            E.scIncl: str := "INCL"; flags := {fAddPosition} |
            E.scExcl: str := "EXCL"; flags := {fAddPosition} |
            E.scNewSys: str := "NEWSYS"; flags := {}
          END;
          WriteOperation (s, str, flags, 1, 0, 0)
        END;
        O.Char (";") |
      E.ndCall:
        ProcCall (s);
        O.Char (";") |
      E.ndIfElse:
        n := s. left;
        REPEAT
          IF (n=s. left) THEN
            O.String ("if (")
          ELSE
            O.String (" else if (")
          END;
          Expr (n. left, FALSE);
          O.String (") ");
          StatementSeq (n. right, off+1, TRUE);
          n := n. link
        UNTIL (n=NIL);
        IF (s. right#NIL) THEN
          O.String (" else");
          StatementSeq (s. right, off+1, TRUE)
        END |
      E.ndCase:
        O.Char ("{");
        Off (off+1);
        Ident (s. left. type. obj);
        O.String (" _temp_ = ");
        Expr (s. left, FALSE);
        O.Char (";");
        Off (off+1);
        O.String ("switch (_temp_) {");
        n := s. right. left;
        WHILE (n # NIL) DO
          m := n. link;
          REPEAT
            Off (off+2);
            O.String ("case ");
            CaseLabel (m, s. left. type. form);
            m := m. link;
            O.Char (":")
          UNTIL (m = NIL);
          O.String (" {");
          StatementSeq (n. right, off+3, FALSE);
          Off (off+3);
          O.String ("break;");
          Off (off+2);
          O.Char ("}");
          n := n. left
        END;
        Off (off+2);
        O.String ("default: ");
        IF (s. right. conval. set#{}) THEN
          StatementSeq (s. right. right, off+2, TRUE)
        ELSE
          O.String ("NO_LABEL (_temp_, ");
          O.Int (s. pos, 0);
          O.String (");")
        END;
        Off (off+1);
        O.Char ("}");
        Off (off);
        O.Char ("}") |
      E.ndWhile:
        O.String ("while (");
        Expr (s. left, FALSE);
        O.String (") ");
        StatementSeq (s. right, off+1, TRUE) |
      E.ndRepeat:
        O.String ("do ");
        StatementSeq (s. left, off+1, TRUE);
        O.String (" while (!(");
        Expr (s. right, FALSE);
        O.String ("));") |
      E.ndFor:
        O.Char ("{");
        Off (off+1);
        Ident (s. left. link. type. obj);
        O.String (" _temp_");
        O.String (" = ");
        Expr (s. left. right, FALSE);
        O.Char (";");
        Off (off+1);
        O.String ("for(");
        Designator (s. left. link, FALSE);
        O.String (" = ");
        Expr (s. left. left, FALSE);
        O.String (" ; ");
        Designator (s. left. link, FALSE);
        IF (s. left. conval. intval > 0) THEN
          O.String (" <= ")
        ELSE
          O.String (" >= ")
        END;
        O.String ("_temp_ ; ");
        Designator (s. left. link, FALSE);
        O.String (" += ");
        O.Int (s. left. conval. intval, 0);
        O.Char (")");
        StatementSeq (s. right, off+2,TRUE);
        Off (off);
        O.Char ("}") |
      E.ndLoop:
        O.String ("while(1)");
        s. pos := -s. pos;
        StatementSeq (s. left, off+1, TRUE);
        IF (s. pos >= 0) THEN  (* if loop has exit statements: write label *)
          Off (off);
          O.String ("_exit");
          O.Int (s. pos, 0);
          O.String (": ;")
        END |
      E.ndWithElse:
        n := s. left;
        WHILE (n # NIL) DO
          IF (n # s. left) THEN
            O.String ("else ")
          END;
          O.String ("if (");
          GenTypeTest (n. left, n. obj. type);
          O.Char (")");
          StatementSeq (n. right, off+1, TRUE);
          n := n. link
        END;
        Off (off);
        O.String ("else");
        IF (s. conval. set = {}) THEN
          O.String (" { NO_GUARD(");
          O.Int (s. pos, 0);
          O.String ("); }")
        ELSE
          StatementSeq (s. right, off+1, TRUE)
        END |
      E.ndExit:
        s. left. pos := ABS (s. left. pos);  (* set flag for 'exit exists' *)
        O.String ("goto _exit");
        O.Int (s. left. pos, 0);
        O.Char (";") |
      E.ndReturn:
        O.String ("return");
        IF (s. left # NIL) THEN
          IF (s. left. type. form IN {T.strPointer, T.strSysPtr}) &
             (s. left. type # s. obj. type. base) THEN
            O.String ("(void*)")
          END;
          O.Char (" ");
          Expr (s. left, FALSE)
        END;
        O.Char (";") |
      E.ndTrap:
        O.String ("HALT");
        Expr (s. left, TRUE);
        O.Char (";") |
      E.ndAssert:
        O.String ("ASSERT(");
        Expr (s. right, FALSE);
        O.String (", ");
        Expr (s. left, FALSE);
        O.String (", ");
        O.Int (s. pos, 0);
        O.String (");")
      ELSE
        HALT (102)
      END;
      s := s. link
    END;
    IF paren THEN
      Off (off-1);
      O.Char ("}")
    END
  END StatementSeq;



PROCEDURE LocalDecl (obj : T.Object; enter : E.Node; off : INTEGER);
  VAR
    local : E.Node;
    next : T.Object;

  PROCEDURE ProcedureBlock (enter : E.Node; off : INTEGER);
    VAR
      obj : T.Object;
      i : INTEGER;
    BEGIN
      IF (enter. obj # NIL) THEN
        (* create variables for formal array value parameters *)
        obj := enter. obj. type. link;
        WHILE (obj # NIL) DO
          IF (obj. mode = T.objVar) & (obj. type. form IN arraySet) THEN
            Off (off+1);
            OpenArrayDecl (obj, FALSE);
            O.Char (";")
          END;
          obj := obj. link
        END
      END;
      (* write local declarations *)
      obj := enter. obj. link. right;
      LocalDecl (obj, enter, off+1);
      (* additional initialization code needed? *)
      IF (enter. obj # NIL) THEN
        (* copy value into variables for formal array value parameters *)
        obj := enter. obj. type. link;
        WHILE (obj # NIL) DO
          IF (obj. mode = T.objVar) & (obj. type. form IN arraySet) THEN
            Off (off+1);
            IF (obj. type. form = T.strArray) THEN
              O.String ("VALUE_ARRAYF(")
            ELSE
              O.String ("VALUE_ARRAY(")
            END;
            Ident (obj);
            O.String (", _");
            Ident (obj);
            O.String ("_p, 1");
            IF (obj. type. form = T.strDynArray) THEN
              FOR i := 0 TO DynDimensions (obj. type)-1 DO
                O.Char ("*");
                O.Char ("_");
                Ident (obj);
                O.Char ("_");
                O.Int (i, 0)
              END
            END;
            O.String (");")
          END;
          obj := obj. link
        END
      END;
      (* emit statements *)
      StatementSeq (enter. right, off+1, FALSE);
      IF (enter. obj. type. base. form # T.strNone) THEN
        Off (off+1);
        O.String ("NO_RETURN (");
        O.Int (enter. pos, 0);
        O.String (");")
      END
    END ProcedureBlock;

  BEGIN
    (* generate local declarations *)
    WHILE (obj # NIL) & (obj. mode < T.objExtProc) DO
      (* Write const/type/var declarations, but no procedures. *)
      obj := Decl (obj, {flagSemicolon}, off)
    END;
    (* emit local procedures *)
    local := enter. left;
    WHILE (local # NIL) DO
      O.Ln;
      IF (local. class = E.ndForward) & (enter. obj # NIL) THEN
        (* forward declaration of nested procedure *)
        next := Decl (local. obj, {flagAutoPrefix}, off)
      ELSE
        next := Decl (local. obj, {}, off)
      END;
      IF (local. class=E.ndForward) THEN
        O.String (";")  (* just a forward declaration *)
      ELSE
        O.String (" {");
        ProcedureBlock (local, off);
        Off (off);
        O.Char ("}")
      END;
      local := local. link
    END
  END LocalDecl;



PROCEDURE WriteInclude* (inclFile: ARRAY OF CHAR);
  VAR
    path: ARRAY Redir.maxPathLen OF CHAR;
  BEGIN
    O.String ('#include "');
    Filenames.GetPath (inclFile, path, inclFile);
    O.String (inclFile);
    O.Char ('"');
    O.Ln
  END WriteInclude;
  
PROCEDURE Include (module : Dep.Module; writeHeader : BOOLEAN);
(* Writes include statements for imported modules.  If writeHeader=TRUE, 
   include only those modules that are part of the symbol file, otherwise 
   include the modules that are not part of the symbol file.  If the header of
   an EXTERNAL module is written, _all_ imported modules are included.
   'fileName' is the name of the file written at this moment. *)
  VAR
    obj : T.Object;
    name : ARRAY M.maxSizeString OF CHAR;
    path : ARRAY Redir.maxPathLen OF CHAR;
    import : Dep.Import;
  BEGIN
    IF writeHeader THEN
      (* include the support file *)
      IF ~Redir.FindPath (M.redir, "_OGCC.h", path) THEN
        Out.String ("Warning: Can't find file _OGCC.h.");
        Out.Ln
      END;
      WriteInclude ("_OGCC.h")
    ELSE                                 (* include the associated .h file *)
      WriteInclude (module. file[header])
    END;
    (* write include statements *)
    obj := T.compiledModule. link. next; 
    WHILE (obj # NIL) DO
      IF (obj. link. mnolev <= T.importMnolev) THEN
        IF T.external OR ((T.flagExport IN obj. flags) = writeHeader) THEN
          T.GetModuleName (obj, name);
          import := module. import;
          WHILE (import # NIL) & (import. module. name # name) DO
            import := import. next
          END;
          IF (import # NIL) THEN
            WriteInclude (import. module. file[header])
          ELSE
            IF ~Redir.FindPathExt (M.redir, name, hExt, path) THEN
              Out.String ("Warning: Can't find file ");
              Out.String (name);
              Out.String (".h.");
              Out.Ln
            END;
            WriteInclude (path)
          END
        END
      END;
      obj := obj. next
    END
  END Include;

PROCEDURE GenHeader (mod : Dep.Module);
  VAR
    obj : T.Object;
  BEGIN
    tdGenerated := T.external;           (* no type descs in external module *)
    (* build conditional to make sure that header is included only once *)
    O.String ("#ifndef _");
    Ident (T.compiledModule);
    O.Char ("_");
    O.Ln;
    O.String ("#define _");
    Ident (T.compiledModule);
    O.Char ("_");
    O.Ln;
    O.Ln;
    O.String ("#ifdef MODULE");          (* AP 19.1.2026 write conditional EXPORT for TDs *)
    Ident (T.compiledModule);
    O.Ln;
    O.String ("#define EXTERN");
    Ident (T.compiledModule);
    O.Ln;
    O.String ("#else");
    O.Ln;
    O.String ("#define EXTERN");
    Ident (T.compiledModule);
    O.String (" extern");
    O.Ln;
    O.String ("#endif");
    O.Ln;
    O.Ln;
    Include (mod, TRUE);       (* include all header that are part of the symbol file *)
    (* write forward declarations of type descriptor types *)
    GenTypeDescrForward (root. link, TRUE);
    (* generate declarations of all exported objects *)
    obj := T.compiledModule. link. right;
    WHILE (obj # NIL) DO
      obj := Decl (obj, {flagHeader, flagSemicolon}, 0)
    END;
    O.Ln;
    GenTypeDescr (root. link, TRUE);
    (* export initializing function *)
    O.Ln;
    GenTypeDescr (root. link, TRUE);
    O.String ("extern void _init");
    Ident (T.compiledModule);
    O.String (" (void);");
    O.Ln;
    (* conclude the starting '#ifndef' *)
    O.Ln;
    O.String ("#endif");
    O.Ln
  END GenHeader;

PROCEDURE GenModule (mod : Dep.Module);
  BEGIN
    tdGenerated := T.external;           (* no type descs in external module *)
    O.String ("#define MODULE");         (* AP 19.1.2026 mark MODULE compile *)
    Ident (T.compiledModule);
    O.Ln;
    O.Ln;
    Include (mod, FALSE);      (* include modules that don't appear in the symbol file *)
    O.Ln;
    O.String ("static ModuleId moduleId;");
    O.Ln;
    (* write forward declarations of type descriptor types *)
    GenTypeDescrForward (root. link, FALSE);
    LocalDecl (T.compiledModule. link. right, root, 0);    
    GenTypeDescr (root. link, FALSE);
    O.Ln;    
    O.Ln;
    O.String ("void _init");
    Ident (T.compiledModule);
    O.String (" (void) {");
    O.Ln;
    O.String ('  moduleId = add_module ("');
    O.String (T.compiledModule. name);
    O.String ('");');
    IF ~T.external THEN                  (* no type descs in external module *)
      GenTypeDescrInit (root. link)
    END;
    StatementSeq (root. right, 1, FALSE);
    O.Ln;
    O.Char ("}")
  END GenModule;

PROCEDURE Module* (mod : Dep.Module; stRoot : E.Node);
(* pre: 'stRoot' contains the root of the syntax tree, 'mod' holds information
     about this module's files.
   post: If (Dep.flSymChanged IN mod. flags) OR ~(header IN mod. flags) a new
     header file will be written.  A new .c file is always generated. *)

  PROCEDURE UpdateMarker (modName : ARRAY OF CHAR);
    VAR
      ok : BOOLEAN;
      marker : ARRAY Redir.maxPathLen+32 OF CHAR;
    BEGIN
      Redir.GeneratePathExt (M.redir, modName, markerExt, marker);
      Str.Insert ("touch ", 0, marker);
      ok := (Rts.System (marker) = 0)
    END UpdateMarker;
   
  BEGIN
    root := stRoot;
    (* emit warning if given calling convention isn't "C" *)
    IF T.external & (T.compiledModule. const. string^ # "C") THEN
      S.WarnIns (T.compiledModule. pos, 297, T.compiledModule. const. string^)
    END;
    (* provide names for anonymous records *)
    CheckName (T.compiledModule, T.compiledModule. link. right);
    IF (Dep.flSymChanged IN mod. flags) OR ~(header IN mod. flags) THEN
      (* write header file *)
      Dep.NewFile (mod, header, hExt); (* generate name for new file *)
      O.Open (mod. file[header]);
      GenHeader (mod);
      O.Close;
      INCL (mod. flags, header)
    END;
    IF T.external THEN
      UpdateMarker (mod. name)
    ELSE
      Dep.NewFile (mod, cFile, cExt);
      INCL (mod. flags, cFile);
      (* write 'module' *)
      O.Open (mod. file[cFile]);
      GenModule (mod);
      O.Ln;
      O.Close
    END
  END Module;



PROCEDURE StructSize (t : T.Struct) : LONGINT;
(* Returns size of type t. -1 means that the size is target dependend and 
   can't be computed here, -2 stands for not computable size (like for open 
   arrays). *)
  BEGIN
    CASE t. form OF
      T.strBool, T.strChar, T.strSysByte: RETURN 1 |
      T.strShortInt: RETURN 2 |
      T.strInteger, T.strReal, T.strSet: RETURN 4 |
      T.strLongInt, T.strLongReal, T.strSysPtr, T.strProc, T.strPointer: RETURN 8 |
      T.strArray, T.strRecord: RETURN -1 |
    ELSE
      RETURN -2
    END
  END StructSize;


BEGIN
  atCount := 0;
  emptyObj := T.NewObject(T.noIdent, T.objType, T.noPos);
  T.structSize := StructSize
END OGenGCC.
