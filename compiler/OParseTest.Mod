MODULE OParseTest;
(* Writes syntax tree as Oberon-2 source code. 
   I apologize for every missing comment in this Module. *)

IMPORT
  O := Out, M := OMachine, T := OTable, E := OEParse,
  Reals := RealStr, Conv := ConvTypes, Str := Strings;

CONST
  noIdent*=MIN(INTEGER);
  genStatmPos = FALSE;

PROCEDURE Off (off : INTEGER);
  VAR
    i : INTEGER;
  BEGIN
    IF (off>=0) THEN
      O.Ln;
      FOR i := 1 TO off*2 DO
        O.Char (" ")
      END
    END
  END Off;

PROCEDURE OStr (ref : ARRAY OF CHAR);
  BEGIN
    O.String (ref)
  END OStr;

PROCEDURE ^ Type (t : T.Struct; structure : BOOLEAN; off : INTEGER);

PROCEDURE FormalPars (pars : T.Struct);
  VAR
    first, last : T.Object;
  BEGIN
    first := pars. link;
    IF (pars. form=T.strTBProc) THEN
      first := first. link
    END;
    IF (first#NIL) OR (pars. base. form#T.strNone) THEN
      O.String (" (");
      WHILE (first#NIL) DO
        last := first;
        IF (first. type. form=T.strNone) THEN
          O.String ("..") 
        ELSE
          IF (first. mode=T.objVarPar) THEN
            O.String ("VAR ")
          END;
          OStr (first. name);
          WHILE (last. link#NIL) & (last. link. mode=first. mode) & (last. link. type=first. type) DO
            last := last. link;
            O.String (", ");
            OStr (last. name)
          END;
          O.String (" : ");
          Type (first. type, FALSE, noIdent);
        END;
        first := last. link;
        IF (first#NIL) THEN
          O.String ("; ")
        END
      END;
      O.Char (")");
      IF (pars. base. form#T.strNone) THEN
        O.String (" : ");
        Type (pars. base, FALSE, noIdent)
      END
    END
  END FormalPars;

PROCEDURE CharConst (ch : LONGINT);
  PROCEDURE Hex (ch : LONGINT) : CHAR;
    BEGIN
      IF (ch<10) THEN
        RETURN CHR (ch+ORD ("0"))
      ELSE
        RETURN CHR (ch-10+ORD ("A"))
      END
    END Hex;

  BEGIN
    IF (ch<32) OR (ch>=127) OR (ch=34) THEN
      IF (ch>=10*16) THEN
        O.Char ("0")
      END;
      O.Char (Hex (ch DIV 16));
      O.Char (Hex (ch MOD 16));
      O.Char ("X")
    ELSE
      O.Char ('"');
      O.Char (CHR (ch));
      O.Char ('"')
    END
  END CharConst;

PROCEDURE ConstString (str : ARRAY OF CHAR);
  BEGIN
    IF (Str.Length(str) = 1) THEN
      CharConst (ORD (str[0]))
    ELSIF (Str.Pos ('"', str, 0) >= 0) THEN
      O.Char ("'");
      OStr (str);
      O.Char ("'")
    ELSE
      O.Char ('"');
      OStr (str);
      O.Char ('"')
    END
  END ConstString;
  
PROCEDURE IdentDef (obj : T.Object);
  BEGIN
    OStr (obj. name);
    IF (obj. mark=T.exportRead) THEN
      O.Char ("-")
    ELSIF (obj. mark=T.exportWrite) THEN
      O.Char ("*")
    END;    
    IF (obj. extName # NIL) THEN         (* external name present *)
      O.String (" [");
      ConstString (obj. extName^);
      O.Char ("]")
    END
  END IdentDef;

PROCEDURE Qualident (obj : T.Object);
  VAR
    mod : T.Object;
  BEGIN
    IF (obj. mnolev < T.predeclMnolev) THEN  (* this includes imports from SYSTEM *)
      mod := obj. scope. left;
      OStr (mod. name);
      O.Char (".")
    END;
    OStr (obj. name)
  END Qualident;

PROCEDURE Type (t : T.Struct; structure : BOOLEAN; off : INTEGER);
  CONST
    structuredSet = {T.strPointer, T.strProc, T.strArray, T.strDynArray, T.strRecord};
  VAR
    first, last : T.Object;
  BEGIN
    IF (t. obj=NIL) OR (structure & (t. form IN structuredSet)) THEN
      CASE t. form OF
      T.strPointer:
        O.String ("POINTER TO ");
        Type (t. base, FALSE, off) |
      T.strProc:
        O.String ("PROCEDURE");
        FormalPars (t) |
      T.strArray:
        O.String ("ARRAY ");
        LOOP
          O.Int (t. len, 0);
          IF (t. base. form#T.strArray) THEN
            EXIT
          END;
          O.String (", ");
          t := t. base
        END;
        O.String (" OF ");
        Type (t. base, FALSE, off) |
      T.strDynArray:
        O.String ("ARRAY OF ");
        Type (t. base, FALSE, off) |
      T.strRecord:
        O.String ("RECORD");
        IF (T.flagUnion IN t. flags) THEN
          O.String (" [UNION]")
        END;
        IF (t. base#NIL) THEN  (* record is an extension *)
          Off (off+1);
          O.Char ("(");
          Qualident (t. base. obj);
          O.Char (")")
        END;
        first := t. link;
        WHILE (first#NIL) & (first. mode#T.objTBProc) DO
          Off (off+1);
          (* emit list of field names of same type *)
          IdentDef (first);
          last := first;
          WHILE (last. next#NIL) & (last. next. type=first. type) DO
            O.String (", ");
            last := last. next;
            IdentDef (last)
          END;
          (* add field type *)
          O.String (" : ");
          Type (first. type, FALSE, off+1);
          first := last. next;
          IF (first#NIL) THEN
            O.Char (";")
          END
        END;
        Off (off);
        O.String ("END")
      END
    ELSE
      Qualident (t. obj)
    END
  END Type;

PROCEDURE Pri (expr : E.Node) : SHORTINT;
  BEGIN
    IF (expr=NIL) THEN
      RETURN MIN(SHORTINT)
    ELSIF (expr. class=E.ndConst) &
	  ((expr. type. form IN E.intSet) & (expr. conval. intval<0) OR
     	   (expr. type. form IN E.realSet) & (expr. conval. real<0)) THEN
      RETURN 4
    ELSIF (expr. class<E.ndMOp) THEN  (* undividable (Designator and ndUpto) *)
      RETURN 0
    ELSE
      CASE expr. subcl OF
      E.scNot:
        RETURN 1 |
      E.scTimes..E.scMod, E.scAnd:
        RETURN 2 |
      E.scPlus, E.scMinus, E.scOr:
        RETURN 3 |
      E.scEql..E.scIs:
        RETURN 5 |
      ELSE  (* predefined functions etc. *)
        RETURN 0
      END
    END
  END Pri;

PROCEDURE ^ Expr* (expr : E.Node; paren : BOOLEAN);

PROCEDURE Designator (d : E.Node);
  BEGIN
    IF (d#NIL) THEN
      Designator (d. left);
      CASE d. class OF
      E.ndVar, E.ndVarPar, E.ndType, E.ndProc:
        Qualident (d. obj) |
      E.ndField, E.ndTBProc, E.ndTBSuper:
        O.String (". ");
        OStr (d. obj. name);
        IF (d. class=E.ndTBSuper) THEN
          O.Char ("^")
        END |
      E.ndDeref:
        O.Char ("^") |
      E.ndIndex:
        O.Char ("[");
        Expr (d. right, FALSE);
        O.Char ("]") |
      E.ndGuard:
        O.Char ("(");
        Qualident (d. obj);
        O.Char (")")
      ELSE
        O.String ("OParseTest.Designator: invalid case ");
        O.Int (d. class, 0);
        HALT (100)
      END
    END
  END Designator;

PROCEDURE Const (c : T.Const; form : SHORTINT; obj : T.Object);
  VAR
    i, j : INTEGER;
    comma : BOOLEAN;

  PROCEDURE RealConst (val : LONGREAL; long : BOOLEAN);
    VAR
      i : INTEGER;
      str : ARRAY 32 OF CHAR;
    BEGIN
      Reals.GiveEng (str, val, 8, 16, Conv.left);
      i := 0;
      WHILE (str[i]#0X) & (str[i]#20X) DO
        INC (i)
      END;
      str[i] := 0X;
      IF long THEN
        i := Str.Pos ("E", str, 0);
        IF (i >= 0) THEN
          str[i] := "D"
        ELSE
          Str.Append ("D0", str)
        END
      END;
      O.String (str)
    END RealConst;

  BEGIN
    IF (obj#NIL) THEN
      Qualident (obj)
    ELSE
      CASE form OF
      T.strBool:
        IF (c. intval=1) THEN
          O.String ("TRUE")
        ELSE
          O.String ("FALSE")
        END |
      T.strChar:
        IF (c. intval=M.minChar) THEN
          O.String ("MIN(CHAR)")
        ELSIF (c. intval=M.maxChar) THEN
          O.String ("MAX(CHAR)")
        ELSE
          CharConst (c. intval)
        END |
      T.strShortInt..T.strLongInt:
        IF (c. intval=M.minSInt) & (form=T.strShortInt) THEN
          O.String ("MIN(SHORTINT)")
        ELSIF (c. intval=M.maxSInt) & (form=T.strShortInt) THEN
          O.String ("MAX(SHORTINT)")
        ELSIF (c. intval=M.minInt) & (form=T.strInteger) THEN
          O.String ("MIN(INTEGER)")
        ELSIF (c. intval=M.maxInt) & (form=T.strInteger) THEN
          O.String ("MAX(INTEGER)")
        ELSIF (c. intval=M.minLInt) & (form=T.strLongInt) THEN
          O.String ("MIN(LONGINT)")
        ELSIF (c. intval=M.maxLInt) & (form=T.strLongInt) THEN
          O.String ("MAX(LONGINT)")
        ELSE
          O.Int (c. intval, 0)
        END |
      T.strReal, T.strLongReal:
        IF (c. real=M.minReal) & (form=T.strReal) THEN
          O.String ("MIN(REAL)")
        ELSIF (c. real=M.maxReal) & (form=T.strReal) THEN
          O.String ("MAX(REAL)")
        ELSIF (c. real=M.minLReal) & (form=T.strLongReal) THEN
          O.String ("MIN(LONGREAL)")
        ELSIF (c. real=M.maxLReal) & (form=T.strLongReal) THEN
          O.String ("MAX(LONGREAL)")
        ELSE
          RealConst (c. real, form=T.strLongReal)
        END |
      T.strSet:
        O.Char ("{");
        i := M.minSet;
        comma := FALSE;
        WHILE (i<=M.maxSet) DO
          IF (i IN c. set) THEN
            j := i;
            WHILE (j<M.maxSet) & (j+1 IN c. set) DO
              INC (j)
            END;
            IF comma THEN
              O.String (", ")
            ELSE
              comma := TRUE
            END;
            O.Int (i, 0);
            IF (j#i) THEN
              O.String ("..");
              O.Int (j, 0);
              i := j
            END
          END;
          INC (i)
        END;
        O.Char ("}") |
      T.strString:
        ConstString (c. string^) |
      T.strNil:
        O.String ("NIL")
      END
    END
  END Const;

PROCEDURE ProcCall (call : E.Node; statement : BOOLEAN);
  VAR
    apar : E.Node;
  BEGIN
    Designator (call. left);
    IF ~statement OR (call. left. type. base. form#T.strNone) OR (call. right#NIL) THEN
      O.Char ("(");
      IF (call. right#NIL) THEN
        apar := call. right;
        REPEAT
          Expr (apar, FALSE);
          apar := apar. link;
          IF (apar#NIL) THEN
            O.String (", ")
          END
        UNTIL (apar=NIL)
      END;
      O.Char (")")
    END
  END ProcCall;

PROCEDURE Convert (value : E.Node; to : SHORTINT);
  VAR
    from : SHORTINT;
  BEGIN
    from := value. type. form;
    IF (to=from+1) & (from IN {T.strShortInt, T.strInteger, T.strReal}) THEN
      O.String ("LONG");
      Expr (value, TRUE)
    ELSIF (to>from) & (from IN E.intSet) & (to IN E.intSet) THEN
      O.String ("LONG(");
      Convert (value, to-1);
      O.Char (")")
    ELSIF (to=from-1) & (from IN {T.strInteger, T.strLongInt, T.strLongReal}) THEN
      O.String ("SHORT");
      Expr (value, TRUE)
    ELSIF (to<from) & (from IN E.intSet) & (to IN E.intSet) THEN
      O.String ("SHORT(");
      Convert (value, to+1);
      O.Char (")")
    ELSIF (from=T.strChar) & (to IN E.intSet) THEN
      O.String ("ORD ");
      Expr (value, TRUE)
    ELSIF (from IN E.intSet) & (to=T.strChar) THEN
      O.String ("CHR");
      Expr (value, TRUE)
    ELSIF (from IN E.realSet) & (to IN E.intSet) THEN
      O.String ("ENTIER");
      Expr (value, TRUE)
    ELSIF (from IN E.intSet) & (to IN E.realSet) THEN  (* there is no fct for integer->real conversion *)
      Expr (value, TRUE)
    ELSE
      O.String ("_conv_");
      Expr (value, TRUE)
    END
  END Convert;

PROCEDURE Expr* (expr : E.Node; paren : BOOLEAN);
  VAR
    o : ARRAY 6 OF CHAR;
    pe, pr, pl : SHORTINT;
  BEGIN
    IF paren THEN
      O.Char ("(")
    END;
    pe := Pri (expr);
    pl := Pri (expr. left);
    pr := Pri (expr. right);
    IF (expr. class<E.ndConst) THEN
      Designator (expr)
    ELSIF (expr. class=E.ndConst) THEN
      Const (expr. conval, expr. type. form, expr. obj)
    ELSIF (expr. class=E.ndUpto) THEN
      Expr (expr. left, FALSE);
      IF (expr. right#expr. left) THEN
        O.String ("..");
        Expr (expr. right, FALSE)
      END
    ELSIF (expr. class=E.ndMOp) & (expr. subcl<=E.scNot) THEN
      CASE expr. subcl OF
      E.scMinus:
        O.Char ("-") |
      E.scNot:
        O.Char ("~")
      END;
      Expr (expr. left, pe<=pl);
    ELSIF (expr. class=E.ndMOp) & (expr. subcl=E.scConv) THEN
      Convert (expr. left, expr. type. form)
    ELSIF (expr. class=E.ndMOp) & (expr. subcl>E.scGeq) THEN
      CASE expr. subcl OF
        E.scAbs: o := "ABS" |
        E.scCap: o := "CAP" |
        E.scENTIER: o := "ENTIER" |
        E.scOdd: o := "ODD" |
        E.scAdr: o := ".ADR" |
        E.scCc: o := ".CC" |
        E.scSize: o := "SIZE"
      ELSE
        o := "_mop_"
      END;
      IF (o[0]=".") THEN
        O.String ("SYSTEM")
      END;
      O.String (o);
      Expr (expr. left, TRUE)
    ELSIF (expr. class=E.ndDOp) & (expr. subcl<E.scNot) THEN
      Expr (expr. left, pe<=pl);
      CASE expr. subcl OF
        E.scPlus: o := "+ " |
        E.scMinus: o := "- " |
        E.scTimes: o := "* " |
        E.scRDiv: o := "/ " |
        E.scIDiv: o := " DIV " |
        E.scMod: o := " MOD " |
        E.scAnd: o := " & " |
        E.scOr: o := " OR " |
        E.scIn: o := " IN " |
        E.scIs: o := " IS " |
        E.scEql: o := "= " |
        E.scNeq: o := "# " |
        E.scLss: o := "< " |
        E.scLeq: o := "<=" |
        E.scGrt: o := "> " |
        E.scGeq: o := ">="
      END;
      IF (o[1]=" ") THEN o[1] := 0X END;
      O.String (o);
      Expr (expr. right, pe<=pr)
    ELSIF (expr. class=E.ndDOp) & (expr. subcl>E.scNot) THEN
      CASE expr. subcl OF
        E.scAsh: o := "ASH" |
        E.scBit: o := ".BIT" |
        E.scLsh: o := ".LSH" |
        E.scRot: o := ".ROT" |
        E.scLen: o := "LEN" |
      ELSE
        o := "_dop_"
      END;
      IF (o[0]=".") THEN
        O.String ("SYSTEM")
      END;
      O.String (o);
      O.Char ("(");
      Expr (expr. left, FALSE);
      IF (expr. subcl#E.scLen) OR (expr. right. conval. intval#0) THEN
        O.String (", ");
        Expr (expr. right, FALSE)
      END;
      O.Char (")")
    ELSIF (expr. class=E.ndCall) THEN
      ProcCall (expr, FALSE)
    ELSE
      O.String ("_expr_")
    END;
    IF paren THEN
      O.Char (")")
    END
  END Expr;

PROCEDURE StatementSeq (s : E.Node; off : INTEGER);
  VAR
    n, m : E.Node;
    o : ARRAY 8 OF CHAR;

  PROCEDURE CaseLabel (l : E.Node; form : SHORTINT);
    VAR
      b : LONGINT;
    BEGIN
      Const (l. conval, form, NIL);
      IF (l. conval. intval#l. conval. intval2) THEN
        O.String ("..");
        b := l. conval. intval;
        l. conval. intval := l. conval. intval2;
        Const (l. conval, form, NIL);
        l. conval. intval := b
      END
    END CaseLabel;

  BEGIN
    WHILE (s#NIL) DO
      Off (off);
      IF genStatmPos THEN
      	O.String ("(*");
      	O.Int (s. conval. intval, 0);
      	O.String ("*)")
      END;
      CASE s. class OF
      E.ndAssign:
        IF (s. subcl=E.scAssign) THEN
          Designator (s. left);
          O.String (" := ");
          Expr (s. right, FALSE)
        ELSIF (s. subcl=E.scMove) THEN
          O.String ("SYSTEM.MOVE(");
          Expr (s. right, FALSE);
          O.String (", ");
          Expr (s. right. link, FALSE);
          O.String (", ");
          Expr (s. right. link. link, FALSE);
          O.Char (")")
        ELSIF (s. subcl=E.scDispose) THEN
          O.String ("SYSTEM.DISPOSE");
          Expr (s. left, TRUE)
        ELSIF (s. subcl=E.scNewFix) OR (s. subcl=E.scNewDyn) THEN
          O.String ("NEW(");
          Expr (s. left, FALSE);
          n := s. right;
          WHILE (n#NIL) DO
            O.String (", ");
            Expr (n, FALSE);
            n := n. link
          END;
          O.Char (")")
        ELSE
          CASE s. subcl OF
            E.scInc: o := "INC" |
            E.scDec: o := "DEC" |
            E.scIncl: o := "INCL" |
            E.scExcl: o := "EXCL" |
            E.scCopy: o := "COPY" |
            E.scGet: o := ".GET" |
            E.scPut: o := ".PUT" |
            E.scGetReg: o := ".GETREG" |
            E.scPutReg: o := ".PUTREG" |
            E.scNewSys: o := ".NEW"
          ELSE
            O.String ("_assign proc_")
          END;
          IF (o[0]=".") THEN
            O.String ("SYSTEM")
          END;
          O.String (o);
          O.Char ("(");
          Expr (s. left, FALSE); n := s.right;
          IF ~(((s. subcl=E.scInc) OR (s. subcl=E.scDec)) &
               ((s. right. class=E.ndConst) & (s. right. conval. intval=1))) THEN
            O.String (", ");
            Expr (s. right, FALSE)
          END;
          O.Char (")")
        END |
      E.ndCall:
        ProcCall (s, TRUE) |
      E.ndIfElse:
        n := s. left;
        REPEAT
          IF (n=s. left) THEN
            O.String ("IF ")
          ELSE
            Off (off);
            O.String ("ELSIF ")
          END;
          Expr (n. left, FALSE);
          O.String (" THEN");
          StatementSeq (n. right, off+1);
          n := n. link
        UNTIL (n=NIL);
        IF (s. right#NIL) THEN
          Off (off);
          O.String ("ELSE");
          StatementSeq (s. right, off+1)
        END;
        Off (off);
        O.String ("END") |
      E.ndCase:
        O.String ("CASE ");
        Expr (s. left, FALSE);
        O.String (" OF");
        n := s. right. left;
        WHILE (n#NIL) DO
          Off (off);
          O.String ("| ");
          m := n. link;
          REPEAT
            CaseLabel (m, s. left. type. form);
            m := m. link;
            IF (m#NIL) THEN
              O.String (", ")
            END
          UNTIL (m=NIL);
          O.Char (":");
          StatementSeq (n. right, off+1);
          n := n. left
        END;
        IF (s. right. conval. set#{}) THEN
          Off (off);
          O.String ("ELSE");
          StatementSeq (s. right. right, off+1)
        END;
        Off (off);
        O.String ("END") |
      E.ndWhile:
        O.String ("WHILE ");
        Expr (s. left, FALSE);
        O.String (" DO");
        StatementSeq (s. right, off+1);
        Off (off);
        O.String ("END") |
      E.ndRepeat:
        O.String ("REPEAT");
        StatementSeq (s. left, off+1);
        Off (off);
        O.String ("UNTIL ");
        Expr (s. right, FALSE) |
      E.ndFor:
        O.String ("FOR ");
        Designator (s. left. link);
        O.String (" := ");
        Expr (s. left. left, FALSE);
        O.String (" TO ");
        Expr (s. left. right, FALSE);
        IF (s. left. conval. intval#1) THEN
          O.String (" BY ");
          Const (s. left. conval, s. left. type. form, s. left. obj)
        END;
        O.String (" DO");
        StatementSeq (s. right, off+1);
        Off (off);
        O.String ("END") |
      E.ndLoop:
        O.String ("LOOP");
        StatementSeq (s. left, off+1);
        Off (off);
        O.String ("END") |
      E.ndWithElse:
        n := s. left;
        WHILE (n#NIL) DO
          IF (n=s.left) THEN
            O.String ("WITH ")
          ELSE
            Off (off);
            O.String ("| ")
          END;
          Designator (n. left);
          O.String (" : ");
          Qualident (n. obj);
          O.String (" DO ");
          StatementSeq (n. right, off+1);
          n := n. link
        END;
        IF (s. conval. set#{}) THEN
          Off (off);
          O.String ("ELSE");
          StatementSeq (s. right, off+1)
        END;
        Off (off);
        O.String ("END") |
      E.ndExit:
        O.String ("EXIT") |
      E.ndReturn:
        O.String ("RETURN");
        IF (s. left#NIL) THEN
          O.Char (" ");
          Expr (s. left, FALSE)
        END |
      E.ndTrap:
        O.String ("HALT");
        Expr (s. left, TRUE) |
      E.ndAssert:
        O.String ("ASSERT(");
        Expr (s. right, FALSE);
        IF (s. left. conval. intval # M.defAssertTrap) THEN
          O.String (", ");
          Expr (s. left, FALSE)
        END;
        O.Char (")")
      ELSE
        O.String ("_statement_")
      END;
      s := s. link;
      IF (s#NIL) THEN
        O.Char (";")
      END
    END
  END StatementSeq;

PROCEDURE Block (enter : E.Node; off : INTEGER);
  VAR
    obj : T.Object;
    local, td : E.Node;
    lastMode : SHORTINT;
    name : ARRAY M.maxSizeIdent OF CHAR;

  PROCEDURE Receiver (rec : T.Object);
    BEGIN
      O.Char ("(");
      IF (rec. mode=T.objVarPar) THEN
        O.String ("VAR ")
      END;
      OStr (rec. name);
      O.String (" : ");
      Type (rec. type, FALSE, noIdent);
      O.String (") ")
    END Receiver;

  BEGIN
    IF (enter. obj = NIL) THEN
      obj := T.compiledModule. link. next;
      IF (obj#NIL) THEN
        O.Ln;
        Off (off);
        O.String ("IMPORT");
        Off (off+1);
        LOOP
          T.GetModuleName (obj, name);
          OStr (name);
          obj := obj. next;
          IF (obj=NIL) THEN
            EXIT
          END;
          O.String (", ")
        END;
        O.Char (";")
      END;
      obj := T.compiledModule. link. right
    ELSE
      obj := enter. obj. link. right
    END;
    lastMode := MIN(SHORTINT);
    (* write declarations *)
    WHILE (obj#NIL) & (obj. mode < T.objExtProc) DO
      IF (lastMode#obj. mode) THEN
        IF (enter. obj=NIL) THEN
          O.Ln
        END;
        Off (off);
        CASE obj. mode OF
          T.objConst: O.String ("CONST") |
          T.objType: O.String ("TYPE") |
          T.objVar: O.String ("VAR")
        ELSE
          O.String ("ILLEGAL CASE (Block): ");
          O.Int (obj. mode, 0);
          HALT (100)
        END
      END;
      IF (obj. mode=T.objConst) THEN
        Off (off+1);
        IdentDef (obj);
        O.String (" = ");
        Const (obj. const, obj. type. form, NIL);
        O.Char (";")
      ELSIF (obj. mode=T.objType) THEN
        Off (off+1);
        IdentDef (obj);
        O.String (" = ");
        Type (obj. type, TRUE, off+1);
        O.Char (";")
      ELSIF (obj. mode=T.objVar) THEN
        Off (off+1);
        IdentDef (obj);
        WHILE (obj. next#NIL) & (obj. next. mode=T.objVar) & (obj. next. type=obj. type) DO
          obj := obj. next;
          O.String (", ");
          IdentDef (obj)
        END;
        O.String (" : ");
        Type (obj. type, FALSE, off+2);
        O.Char (";")
      END;
      lastMode := obj. mode;
      obj := obj. next
    END;
    (* write procedures *)
    local := enter. left;
    WHILE (local#NIL) DO
      O.Ln;
      Off (off);
      O.String ("PROCEDURE ");
      IF (local. class=E.ndForward) THEN
        O.String ("^ ")
      END;
      IF (local. obj. mode = T.objTBProc) THEN
        Receiver (local. obj. type. link)
      END;
      IdentDef (local. obj);
      FormalPars (local. obj. type);
      IF (local. class#E.ndForward) & ~T.external THEN
        O.Char (";");
        Block (local, off+1);
        OStr (local. obj. name)
      END;
      O.Char (";");
      local := local. link
    END;
    IF (enter. left#NIL) THEN
      O.Ln
    END;
    IF (enter. obj=NIL) THEN  (* if module: generate type descriptors *)
      td := enter. link;
      WHILE (td # NIL) DO
      	Off (off);
      	O.String ("(* TD: ");
        IF (td. type. obj = NIL) THEN
          O.String ("anonymous type")
        ELSE
       	  OStr (td. type. obj. name)
        END;
      	O.String (" *)");
      	td := td. link
      END
    END;
    IF (enter. right#NIL) THEN
      Off (off); O.String ("BEGIN");
      StatementSeq (enter. right, off+1)
    END;
    Off (off); O.String ("END ");
  END Block;

PROCEDURE Module* (root : E.Node);
  BEGIN
    O.String ("MODULE ");
    OStr (T.compiledModule. name);
    IF T.external THEN
      O.String (" [");
      ConstString (T.compiledModule. const. string^);      
      O.String ("] EXTERNAL [");
      ConstString (T.compiledModule. extName^);
      O.Char ("]")
    END; 
    O.Char (";");
    Block (root, 0);
    OStr (T.compiledModule. name);
    O.Char (".")
  END Module;

END OParseTest.
