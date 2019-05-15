MODULE OEParse;  (* Author: Michael van Acken *)
(* 	$Id: OEParse.Mod,v 1.48 1997/03/27 08:56:46 oberon1 Exp $
   Supporting procedures for the parser, including type checking, constant 
   folding and handling of the predefined procedures. *)

IMPORT
  S := OScan, T := OTable, M := OMachine, Str := Strings, Strings2, IntStr, 
  Conv := ConvTypes;

CONST
  ndCollected = -1;  (* pseudo class that marks already collected nodes (see RecycleMem) *)
  (* designator classes *)
  ndVar*=1; ndVarPar*=2; ndField*=3; ndDeref*=4;
  ndIndex*=5; ndGuard*=6; ndType*=7;
  ndProc*=8; ndTBProc*=9; ndTBSuper*=10; ndConst*=11;
  (* expression classes *)
  ndUpto*=12; ndMOp*=13;     ndDOp*=14;
  (* statement classes *)
  ndInitTd*=15; ndAssign*=16; ndCall*=17;
  ndIfElse*=18; ndIf*=19; ndCase*=20; ndCaseElse*=21;
  ndCaseDo*=22; ndWhile*=23; ndRepeat*=24; ndFor*=25;
  ndForHeader*=26; ndForRange*=27; ndLoop*=28; ndWithElse*=29;
  ndWithGuard*=30; ndExit*=31; ndReturn*=32; ndTrap*=33;
  ndEnter*=34; ndForward*=35; ndAssert*=36;
  (* subclasses of ndDOp, ndMOp *)
  scTimes*=S.times; scRDiv*=S.slash; scIDiv*=S.div; scMod*=S.mod; scAnd*=S.and;
  scPlus*=S.plus; scMinus*=S.minus; scOr*=S.or;
  scEql*=S.eql; scNeq*=S.neq; scLss*=S.lss; scLeq*=S.leq; scGrt*=S.gtr; 
  scGeq*=S.geq; scIn*=S.in; scIs*=S.is;
  scNot*=17;
  scSize*=T.predSIZE; scAbs*=T.predABS; scCap*=T.predCAP; scOdd*=T.predODD; 
  scAdr*=T.sysADR; scCc*=T.sysCC;
  scAsh*=T.predASH; scBit*=T.sysBIT; scLsh*=T.sysLSH; scRot*=T.sysROT; 
  scLen*=T.predLEN; scVal*=T.sysVAL;
  (* subclasses of ndAssign *)
  scAssign*=0;
  scIncl*=T.predINCL; scExcl*=T.predEXCL; scInc*=T.predINC; scDec*=T.predDEC;
  scCopy*=T.predCOPY; scMove*=T.sysMOVE; scGet*=T.sysGET; scPut*=T.sysPUT; 
  scGetReg*=T.sysGETREG; scPutReg*=T.sysPUTREG; scNewSys*=T.sysNEW; 
  scNewFix*=T.predNEW; scNewDyn*=scNewFix+1; scDispose* = T.sysDISPOSE;
  scConv*=45; scCollect*=T.sysCOLLECT;
  
  (* constants used in ChkType to check for groups of types *)
  grpNumeric = -1;                       (* numeric types *)
  grpInteger* = -2;                      (* integer types *)
  grpNilComp = -3;                       (* types assignment/expression compatible to NIL *)
  grpShift = -4;                         (* types applicable to LSH/ROT *)
  grpExtStd = -5;                        (* types applicable to (GET|PUT)[REG] *)
  grpPointer = -6;                       (* types of first parameter SYSTEM.NEW *)
  grpNumOrSet = -7;                      (* numeric or set type *)
  grpIntOrChar* = -8;                    (* integer or char type *)

  intSet* = {T.strShortInt, T.strInteger, T.strLongInt};
  realSet* = {T.strReal, T.strLongReal};
  numSet* = intSet + realSet;
  nilCompSet = {T.strPointer, T.strProc, T.strSysPtr};
  arraySet = {T.strArray, T.strDynArray};

TYPE
  Node* = POINTER TO NodeDesc;
  NodeDesc =
    RECORD
      left*, right*, link* : Node;
      class*, subcl* : SHORTINT;
      type* : T.Struct;
      obj* : T.Object;
      conval* : T.Const;
      pos* : LONGINT;
    END;

VAR
  nodeHeap : Node;  (* list of unused nodes (see RecycleMem) *)

PROCEDURE NewNode* (class : SHORTINT) : Node;
(* Creates and initializes a new node of the form 'class'. If 'class' denotes a
   statement node.type is set to 'noType', otherwise to 'undefType'. *)
  VAR
    n : Node;
  BEGIN
    IF (nodeHeap # NIL) THEN
      n := nodeHeap;
      nodeHeap := nodeHeap. link
    ELSE
      NEW (n)
    END;
    n. left := NIL; n. right := NIL; n. link := NIL;
    n. class := class; n. subcl := 0; n. pos := S.lastSym;
    n. obj := NIL; n. conval := NIL;
    IF (class >= ndInitTd) THEN
      n. type := T.predeclType[T.strNone]
    ELSE
      n. type := T.predeclType[T.strUndef]
    END;
    RETURN n
  END NewNode;




PROCEDURE TypeName (str  : T.Struct; VAR descr: ARRAY OF CHAR);
(* Get a plain text type description for a structure. *)
  VAR
    elemDescr : ARRAY M.maxSizeString OF CHAR;
  BEGIN
    IF (str. obj # NIL) THEN (* named type *)
      IF (str. obj. mnolev < T.predeclMnolev) THEN
        T.GetModuleName (str. obj, descr);
        Strings2.AppendChar (".", descr)
      ELSE
        COPY ("", descr)
      END;
      Str.Append (str. obj. name, descr)
    ELSE
      CASE str. form OF
        | T.strArray, T.strDynArray: 
          IF (str. form = T.strArray) THEN
            COPY ("ARRAY ", descr);
            IntStr.Append (str. len, 0, Conv.left, descr);
            Str.Append (" OF ", descr)
          ELSE
            COPY ("ARRAY OF ", descr)
          END;
          TypeName (str. base, elemDescr);
          Str.Append (elemDescr, descr)
        | T.strTBProc: COPY ("type bound PROCEDURE", descr)
        | T.strNil: COPY ("NIL", descr)
        | T.strNone: COPY ("void", descr)
        | T.strPointer: COPY ("POINTER", descr)
        | T.strProc: COPY ("PROCEDURE", descr)
        | T.strString: COPY ("string", descr)
        | T.strRecord: COPY ("RECORD", descr)
        | T.strUndef: COPY ("undef", descr)
      END
    END
  END TypeName;

PROCEDURE ErrT1* (pos : LONGINT; num : INTEGER; VAR t1 : T.Struct);
(* If 't1' is no 'undefType', then print error message 'num' with a textual 
   representation of type 't1' inserted instead of the '%'. 'noerr' is set to 
   FALSE. *)
  VAR
    ins : ARRAY 128 OF CHAR;
  BEGIN
    IF (t1. form # T.strUndef) THEN
      TypeName (t1, ins);
      S.ErrIns (pos, num, ins);
      t1 := T.predeclType[T.strUndef]
    END
  END ErrT1;

PROCEDURE ErrNT1* (num : INTEGER; n1 : Node);
(* Same as ErrT1, but use the node's position and its type for the 
   error msg. *)
  BEGIN
    ErrT1 (n1. pos, num, n1. type)
  END ErrNT1;

PROCEDURE Err* (num : INTEGER);
(* Give error msg at the current token's position. *)
  BEGIN
    S.Err (-1, num)
  END Err;



PROCEDURE IntConstType (val : LONGINT) : SHORTINT;
(* Returns the id of the smallest integer type 'val' will fit it. *)
  BEGIN
    IF (M.minSInt <= val) & (val <= M.maxSInt) THEN
      RETURN T.strShortInt
    ELSIF (M.minInt <= val) & (val <= M.maxInt) THEN
      RETURN T.strInteger
    ELSE
      RETURN T.strLongInt
    END
  END IntConstType;

PROCEDURE Convert (VAR r : Node; to : SHORTINT);
(* Inserts a conversion node before the expression in 'r'.
  pre: 'r' is an expression, 'to' the destination type (a numeric or character
    type).  If to=T.strChar, 'r' has to be an integer expression.
  post: 'r' contains the expression, converted to the type 'to'.
    If 'r' is a constant its value is converted directly and no extra node is
    created. *)
  VAR
    n : Node;
  BEGIN
    IF (r. type. form # to) & (to IN numSet+{T.strChar}) THEN
      IF (r. class=ndConst) THEN  (* constant conversion *)
        IF (r. type. form IN intSet) & (to # T.strChar) & 
           (IntConstType (r. conval. intval) > to) THEN
          ErrNT1 (256, r) (* constant too large, no down conversion possible *)
        ELSIF (to = T.strChar) & ((r. conval. intval<M.minChar) OR (r. conval. intval>M.maxChar)) THEN
          Err (256)  (* constant beyond character range *)
        ELSIF (to = T.strShortInt) & (r. type. form = T.strChar) & (r. conval. intval > M.maxSInt) THEN
           Err (256)  (* character constant too large for short int *)
        ELSIF (r. type. form >= T.strReal) & (to < T.strReal) THEN
          r. conval. intval := ENTIER (r. conval. real)
        ELSIF (r. type. form < T.strReal) & (to >= T.strReal) THEN
          r. conval. real := r. conval. intval
        END;
        r. obj := NIL
      ELSIF (r. subcl # scConv) OR (r. type. form < to) THEN  (* create new node *)
        (* since r may be part of an expression list, replace r^ with the conversion node
           and put the original expression structure in a new node n. *)
        n := NewNode (ndMOp);
        n^ := r^;
        r. class := ndMOp;
        r. subcl := scConv;
        r. left := n;
        r. right := NIL;
        r. obj := NIL;
        r. conval := NIL  (* keep r. link *)
      END;
      r. type := T.predeclType[to]
    END
  END Convert;

PROCEDURE StringConv (node : Node) : BOOLEAN;
(* TRUE iff 'node' is a string constant or it was possible to convert 'node' 
   into one.  In the later case the type of the node is changed to string as a
   side effect. *)
  BEGIN
    IF (node. type. form=T.strString) THEN
      RETURN TRUE
    ELSIF (node. class=ndConst) & (node. type. form=T.strChar) THEN
      node. type := T.predeclType[T.strString];
      node. obj := NIL;
      NEW (node. conval. string);
      node. conval. string[0] := CHR (node. conval. intval);
      node. conval. string[1] := 0X;
      RETURN TRUE
    ELSE
      RETURN FALSE
    END
  END StringConv;

PROCEDURE ArrayComp* (a : Node; f : T.Struct) : BOOLEAN;
(* TRUE iff expression 'a' is array compatible in respect to type 'f'. *)
  PROCEDURE ArrayCompRec (a, f : T.Struct) : BOOLEAN;
    BEGIN
      RETURN
        T.SameType (a, f) OR (f. form=T.strDynArray) & (a. form IN arraySet) & ArrayCompRec (a. base, f. base)
    END ArrayCompRec;

  BEGIN
    RETURN (f. form=T.strDynArray) & (f. base. form=T.strChar) & StringConv (a) OR ArrayCompRec (a. type, f)
  END ArrayComp;

PROCEDURE AssignComp* (var : T.Struct; expr : Node) : BOOLEAN;
(* Test whether 'expr' is assignment compatible to a variable of type 'var'.
  pre: 'var' denotes a type, 'expr' contains an expression
  post: Result is TRUE iff 'expr' is assignment compatible to 'var'.
  side: If 'var' and 'expr' are of a numeric type and are assignment 
        compatible, 'expr' is converted to the type 'var' as a side effect. *)
  VAR
    tv, te : SHORTINT;
  BEGIN
    tv := var. form; te := expr. type. form;
    IF T.SameType (var, expr. type) OR
        T.ExtOf (expr. type, var) OR
        (tv IN nilCompSet) & (te=T.strNil) OR
        (tv=T.strArray) & (var. base. form=T.strChar) &
          (te=T.strString) & (Str.Length (expr. conval. string^)<var. len) OR
        (tv=T.strArray) & (var. base. form=T.strChar) &
          (var. len>1) & (te = T.strChar) & StringConv (expr) OR
        (tv=T.strProc) & (expr. class=ndProc) &
          (expr. obj. mnolev<=T.compileMnolev) & T.ParamsMatch (var, expr. type) &
          ((expr. obj. mode=T.objExtProc) OR (expr. obj. mode=T.objLocalProc)) OR
        (tv=T.strSysByte) & ((te=T.strChar) OR (te=T.strShortInt)) OR
        (tv=T.strSysPtr) & (te=T.strPointer) THEN
      RETURN TRUE
    ELSIF (te IN numSet) & (tv IN numSet) & (tv > te) THEN
      (* implicit type conversion *)
      Convert (expr, tv);                (* adapt 'expr' to type 'var' *)
      RETURN TRUE
    ELSIF (te IN intSet) & (tv = T.strPointer) & (T.flagExternal IN var. flags) THEN
      (* LONGINT is assignment compatible to external pointer *)
      Convert (expr, T.strLongInt);
      RETURN TRUE
    ELSIF (expr. type. form = T.strUndef) THEN    
      (* some error in 'expr', no need to emit an error message here *)
      RETURN TRUE
    ELSE
      ErrT1 (expr. pos, 166, var);
      RETURN FALSE
    END
  END AssignComp;

PROCEDURE String (n : Node) : BOOLEAN;
(* Result is TRUE iff n is a string constant or an array of characters. *)
  BEGIN
    RETURN (n. type. form = T.strString) OR (n. type. form IN arraySet) & (n. type. base. form=T.strChar)
  END String;



PROCEDURE ChkGuard* (var : Node; typeObj : T.Object; chkQual : BOOLEAN; tpos : LONGINT) : BOOLEAN;
(* Tests if the type guard/test test is allowed.
  pre: 'var' contains a variable designator, 'typeObj' the type test applied 
    to 'var'. chkQual=TRUE demands that 'var' is not more than a qualified 
    identifier (i.e. no selectors). 'tpos' is the file position of 'typeObj'
    (used for error messages).
  post: Result is TRUE iff 'typeObj' is a legal type guard/type test to the 
    variable 'var'.  Otherwise an error is emitted and FALSE returned. *)
  VAR
    pos : LONGINT;
  BEGIN
    pos := S.lastErr;
    IF (typeObj. mode # T.objType) THEN
      S.Err (tpos, 205)  (* no data type *)
    ELSE
      IF (var. type. form = T.strPointer) THEN
        IF chkQual & (var. class # ndVar) & (var. class # ndVarPar) THEN
          S.Err (var. pos, 244)   (* only qualified identifier allowed here *)
        ELSIF (var. type. base. form # T.strRecord) THEN
          S.Err (var. pos, 237)  (* has to be pointer or record *)
        ELSIF (T.flagExternal IN var. type. base. flags) THEN
          S.Err (var. pos, 164)  (* this variable has no dynamic type; no type test applicable *)
        END
      ELSIF (var. type. form = T.strRecord) THEN
        IF (var. class # ndVarPar) THEN
          S.Err (var. pos, 236)  (* has to be a variable parameter *)
        ELSIF (T.flagExternal IN var. type. flags) THEN
          S.Err (var. pos, 164)  (* this variable has no dynamic type; no type test applicable *)
        END
      ELSE
        ErrT1 (var. pos, 164, var. type)  (* has no dynamic type *)
      END;
      IF ~T.ExtOf (typeObj. type, var. type) THEN
        ErrT1 (tpos, 163, var. type)  (* has to be an extension of type of var *)
      END
    END;
    RETURN (pos = S.lastErr)  (* TRUE iff no error occured *)
  END ChkGuard;

PROCEDURE ChkType* (n : Node; form : SHORTINT);
(* If 'n' is not of a type specified with 'form', print error message.
   pre: form is one of the OTable.strXXX or a grpXXX constant defined above.
   side: if form=T.strString and 'n' is a constant character expression, the 
     type of the node 'n' is changed to string. *)
  VAR
    set : SET;
    expected : ARRAY 32 OF CHAR;

  PROCEDURE ErrTx;
    VAR
      ins : ARRAY 128 OF CHAR;
    BEGIN
      IF (n. type. form # T.strUndef) THEN
        TypeName (n. type, ins);
        S.ErrIns2 (n. pos, 199, expected, ins)
      END
    END ErrTx;

  BEGIN
    IF (form < 0) THEN
      CASE form OF
      | grpNumeric: set := numSet; expected := "Numeric"
      | grpInteger: set := intSet; expected := "Integer"
      | grpNilComp: set := nilCompSet; expected := "NIL compatible"
      | grpShift: set := intSet+{T.strChar, T.strSysByte}; expected := "Integer, Char, or Byte"
      | grpExtStd: set := {T.strBool..T.strSet, T.strProc, T.strPointer, T.strSysPtr}; expected := "Unstructured"
      | grpPointer: set := {T.strPointer, T.strSysPtr}; expected := "Pointer"
      | grpNumOrSet: set := numSet+{T.strSet}; expected := "Numeric or SET"
      | grpIntOrChar: set := numSet+{T.strChar}; expected := "Integer or CHAR"
      END;
      IF ~(n. type. form IN set) THEN
        ErrTx
      END
    ELSIF (form=T.strString) THEN
      IF ~String (n) & ~StringConv (n) THEN
        expected := "String";
        ErrTx
      END
    ELSIF (n. type. form # form) THEN
      CASE form OF
      | T.strBool..T.strSet: COPY (T.predeclType[form]. obj. name, expected)
      | T.strSysPtr: expected := "SYSTEM.PTR"
      | T.strSysByte: expected := "SYSTEM.BYTE"
      | T.strPointer: expected := "Pointer"
      | T.strProc: expected := "Procedure"
      END;
      ErrTx
    END
  END ChkType;

PROCEDURE ChkRange* (node : Node; lower, upper : LONGINT; num : INTEGER);
(* If 'node' is a constant (integer) expression out of the interall 
   [lower..upper], print error message 'num' at node.pos. *)
  VAR
    l, u : ARRAY 16 OF CHAR;
  BEGIN
    IF (node. class = ndConst) & ((node. conval. intval < lower) OR (node. conval. intval > upper)) THEN
      IntStr.Give (l, lower, 1, 0);
      IntStr.Give (u, upper, 1, 0);
      S.ErrIns2 (node. pos, num, l, u)
    END
  END ChkRange;

PROCEDURE SetInt* (r : Node);
(* If r is an integer constant, the exact type is set according to the 
   constants value. *)
  BEGIN
    IF (r. class = ndConst) & (r. type. form IN intSet) THEN
      r. type := T.predeclType[IntConstType (r. conval. intval)]
    END
  END SetInt;

PROCEDURE Check* (VAR r : Node);
(* Checks for expression compability and does constant folding.
  pre: 'r' is a one- or two-handed expression (this includes prefdefined 
    procedures).
  post: The expression is checked for type compability as stated in the 
    report. Type conversion is applied to the left or right hand side if 
    necessary (but only if 'r' is a expression, no type conversion is used on 
    parameters of predefined procedures). If the expression is a constant
    addition or multiplication operation (including the operations on sets and
    booleans) or a predefined function, the expression is evaluated and r 
    replaced by a constant value. *)
  CONST
    illegalNode = {ndType, ndTBProc, ndTBSuper};

  PROCEDURE FoldMOp (form : SHORTINT; subcl : SHORTINT; c : T.Const) : BOOLEAN;
  (* pre: 'subcl' denotes a one-handed operation applied an the constant 'c'. 
       'form' contains the result type of the operation.
     post: 'c' is replaced by the result of the operation. *)
    BEGIN
      CASE subcl OF
      | scMinus:
        IF (form=T.strSet) THEN
          c. set := -c. set
        ELSIF (form IN intSet) THEN
          c. intval := -c. intval
        ELSE  (* (r. type. form IN realSet) *)
          c. real := -c. real
        END
      | scNot:
        c. intval := 1-c. intval
      | scAbs:
        IF (form IN intSet) THEN
          c. intval := ABS (c. intval)
        ELSE  (* form IN realSet *)
          c. real := ABS (c. real)
        END
      | scCap:
        c. intval := ORD (CAP (CHR (c. intval)))
      | scOdd:
        c. intval := c. intval MOD 2;
        r. left. type := T.predeclType[T.strBool]
      ELSE
        RETURN FALSE
      END;
      RETURN TRUE
    END FoldMOp;

  PROCEDURE FoldDOp (form : SHORTINT; subcl : SHORTINT; cl, cr : T.Const) : BOOLEAN;
  (* pre: 'subcl' denotes a two-handed operation applied an the constants 'cl'
       and 'cr'.  'form' contains the result type of the operation.
     post: 'cl' is replaced by the result of the operation. *)
    BEGIN
      CASE subcl OF
      | scPlus:
        IF (form=T.strSet) THEN
          cl. set := cl. set+cr. set
        ELSIF (form IN intSet) THEN
          INC (cl. intval, cr. intval)
        ELSE  (* (form IN realSet) *)
          cl. real := cl. real+cr. real
        END
      | scMinus:
        IF (form=T.strSet) THEN
          cl. set := cl. set-cr. set
        ELSIF (form IN intSet) THEN
          DEC (cl. intval, cr. intval)
        ELSE  (* (form IN realSet) *)
          cl. real := cl. real-cr. real
        END
      | scTimes:
        IF (form=T.strSet) THEN
          cl. set := cl. set*cr. set
        ELSIF (form IN intSet) THEN
          cl. intval := cl. intval*cr. intval
        ELSE  (* (form IN realSet) *)
          cl. real := cl. real*cr. real
        END
      | scIDiv:
        IF (cr. intval # 0) THEN
          cl. intval := cl. intval DIV cr. intval
        ELSE
          S.Err (r. right. pos, 227)  (* division by zero *)
        END
      | scMod:
        IF (cr. intval # 0) THEN
          cl. intval := cl. intval MOD cr. intval
        ELSE
          S.Err (r. right. pos, 227)  (* division by zero *)
        END
      | scRDiv:
        IF (form=T.strSet) THEN
          cl. set := cl. set / cr. set
        ELSIF (cr. real # 0) THEN  
          cl. real := cl. real / cr. real
        ELSE
          S.Err (r. right. pos, 227)  (* division by zero *)
        END
      | scAnd:
        cl. intval := cl. intval*cr. intval
      | scOr:
        IF (cr. intval > cl. intval) THEN cl. intval := cr. intval END
      | scIn:
        IF (M.minSet<=cl. intval) & (cl. intval<=M.maxSet) & (cl. intval IN cr. set) THEN
          cl. intval := 1
        ELSE
          cl. intval := 0
        END;
        r. left. type := T.predeclType[T.strBool]
      | scAsh:
        cl. intval := ASH (cl. intval, cr. intval);
        r. left. type := T.predeclType[T.strLongInt]
      ELSE
        RETURN FALSE
      END;
      RETURN TRUE
    END FoldDOp;

  PROCEDURE FoldBool (subcl : SHORTINT; left, right : Node) : Node;
    (* pre: 'left', 'right' contain a valid boolean expression, not both
         are constants. 'subcl' denotes a AND or OR operation.
       post: If the operation can be simplified, return the new expression.
         Otherwise return the original expression in 'r'. *)
    BEGIN
      IF (left. class = ndConst) THEN
        IF (subcl = scOr) & (left. conval. intval = 0) OR
           (subcl = scAnd) & (left. conval. intval = 1) THEN
          RETURN right
        ELSE
          RETURN left
        END
      ELSIF (right. class = ndConst) &
            ((subcl = scOr) & (right. conval. intval = 0) OR
             (subcl = scAnd) & (right. conval. intval = 1)) THEN
        RETURN left
      ELSE
        RETURN r
      END
    END FoldBool;

  PROCEDURE CheckMOp (subcl : SHORTINT; left : Node) : T.Struct;
  (* pre: 'left' is an expression with a "not undefined" type.
     post: The parameter's/operand's type is checked in regard to the 
       predefined function (respectively the operator). The expression's 
       result type is returned. *)
    BEGIN
      IF (subcl=scMinus) THEN  (* monadic minus on numeric or set expression *)
        ChkType (left, grpNumOrSet);
        RETURN left. type
      ELSIF (subcl=scNot) THEN  (* not on boolean expression *)
        ChkType (left, T.strBool);
        RETURN T.predeclType[T.strBool]
      ELSIF (subcl=scAbs) THEN  (* ABS on numeric expression *)
        ChkType (left, grpNumeric);
        RETURN left. type
      ELSIF (subcl=scCap) THEN  (* CAP on char expression *)
        ChkType (left, T.strChar);
        RETURN T.predeclType[T.strChar]
      ELSIF (subcl=scOdd) THEN  (* ODD on integer expression *)
        ChkType (left, grpInteger);
        RETURN T.predeclType[T.strBool]
      ELSIF (subcl=scAdr) THEN  (* ADR (no type check, result is LONGINT) *)
        RETURN T.predeclType[T.strLongInt]
      ELSIF (subcl=scCc) THEN  (* CC on integer constant *)
        ChkType (left, grpInteger); ChkRange (left, M.minCC, M.maxCC, 266);
        RETURN T.predeclType[T.strBool]
      END
    END CheckMOp;

  PROCEDURE CheckDOp (subcl : SHORTINT; left, right : Node) : T.Struct;
  (* pre: 'left' and 'right' are expressions with a "not undefined" type.
     post: The parameter's/operand's type is checked in regard to the 
       predefined function respectively the operator. The expression's result 
       type is returned. *)
    VAR
      res, lform, rform : SHORTINT;
    BEGIN
      lform := left. type. form;
      rform := right. type. form;
      (* store in res the largest type including both operands *)
      res := lform;
      IF (rform > res) THEN
        res := rform
      END;
      IF ~(res IN numSet) THEN  (* don't let res be a not numeric type *)
        res := T.strShortInt
      END;
      IF (subcl IN {scTimes, scPlus, scMinus, scRDiv}) THEN  (* + - * / numeric or set expression *)
        IF (lform IN numSet) THEN
          ChkType (right, grpNumeric);
          IF (subcl = scRDiv) & (res < T.strReal) THEN  (* result of division is smallest enclosing real type *)
            res := T.strReal
          END;
          Convert (left, res);
          Convert (right, res)
        ELSIF (lform=T.strSet) THEN
          ChkType (right, T.strSet)
        ELSE
          ChkType (left, grpNumOrSet)  (* will fail *)
        END;
        RETURN left. type
      ELSIF (subcl=scIDiv) OR (subcl=scMod) THEN  (* DIV MOD integer expression *)
        ChkType (left, grpInteger);
        ChkType (right, grpInteger);
        Convert (left, res);
        Convert (right, res);
        RETURN left. type
      ELSIF (subcl=scAnd) OR (subcl=scOr) THEN  (* AND OR boolean expression *)
        ChkType (left, T.strBool);
        ChkType (right, T.strBool);
        RETURN T.predeclType[T.strBool]
      ELSIF (subcl=scIn) THEN  (* IN set expression *)
        ChkType (left, grpInteger);
        ChkType (right, T.strSet);
        RETURN T.predeclType[T.strBool];
      ELSIF (scEql<=subcl) & (subcl<=scGeq) THEN  (* relations *)
        IF (lform IN numSet) THEN  (* integer comparison *)
          ChkType (right, grpNumeric);
          Convert (left, res);
          Convert (right, res)
        ELSIF (lform=T.strChar) THEN  (* char comparison *)
          IF String (right) THEN  (* right side is a string? *)
            ChkType (left, T.strString)
          ELSE
            ChkType (right, T.strChar)
          END
        ELSIF String (left) THEN
          ChkType (right, T.strString)
        ELSE
          IF (subcl=scEql) OR (subcl=scNeq) THEN   (* equivalence operations *)
            IF (lform IN {T.strBool, T.strSet, T.strSysByte}) THEN
              ChkType (right, lform)
            ELSIF (lform=T.strSysPtr) THEN
              IF (right. type. form # T.strNil) THEN
                ChkType (right, lform)
              END
            ELSIF (lform=T.strPointer) THEN
              IF ~(T.SameType (left. type, right. type) OR 
                   (rform=T.strNil) OR
                   T.ExtOf (left. type, right. type) OR 
                   T.ExtOf (right. type, left. type)) THEN
                ErrT1 (left. pos, 165, right. type)  (* not comparable *)
              END
            ELSIF (lform=T.strProc) THEN
              IF (rform=T.strProc) THEN
                IF ~T.ParamsMatch (left. type, right. type) THEN
                  S.Err (right. pos, 247)  (* procedure values not comparable *)
                END
              ELSIF (rform#T.strNil) THEN
                ChkType (right, T.strProc)
              END
            ELSIF (lform=T.strNil) THEN
              ChkType (right, grpNilComp)
            ELSE
              ErrNT1 (161, left)  (* no equivalence relation defined *)
            END
          ELSE
            ErrNT1 (160, left)  (* no ordering relation defined *)
          END
        END;
        RETURN T.predeclType[T.strBool]
      ELSIF (subcl=scAsh) THEN  (* SYSTEM.ASH *)
        ChkType (left, grpInteger);
        ChkType (right, grpInteger);
        RETURN T.predeclType[T.strLongInt]
      ELSIF (subcl=scBit) THEN  (* SYSTEM.BIT *)
        ChkType (left, T.strLongInt);
        ChkType (right, grpInteger);
        RETURN T.predeclType[T.strBool]
      ELSIF (subcl=scLsh) OR (subcl=scRot) THEN  (* SYSTEM.LSH and SYSTEM.ROT *)
        ChkType (left, grpShift);
        ChkType (right, grpInteger);
        RETURN left. type
      END
    END CheckDOp;

  PROCEDURE CheckAssign (subcl : SHORTINT; left, right : Node) : T.Struct;
  (* pre: 'left' and 'right' are expressions with a "not undefined" type.
     post: The parameter's/operand's type is checked in regard to the 
       predefined function respectively the operator. The returned type is 
       always 'noType'. *)
    BEGIN
      IF (subcl=scExcl) OR (subcl=scIncl) THEN  (* INCL and EXCL *)
        ChkType (left, T.strSet);
        ChkType (right, grpInteger)
      ELSIF (subcl=scCopy) THEN  (* COPY *)
        ChkType (left, T.strString);
        ChkType (right, T.strString)
      ELSIF (subcl=scInc) OR (subcl=scDec) THEN  (* INC and DEC *)
        ChkType (left, grpInteger);
        IF (right. class=ndConst) THEN
          ChkType (right, grpInteger)
        ELSE
          ChkType (right, left. type. form)
        END;
        Convert (right, left. type. form)  (* convert inc/dec value automatically to the variable's type *)
      ELSIF (subcl=scGet) OR (subcl=scPut) THEN  (* SYSTEM.GET and SYSTEM.PUT *)
        ChkType (left, T.strLongInt);
        ChkType (right, grpExtStd)
      ELSIF (subcl=scGetReg) OR (subcl=scPutReg) THEN  (* SYSTEM.GETREG and SYSTEM.PUTREG *)
        ChkType (left, grpInteger);
        ChkRange (left, M.minRegNum, M.maxRegNum, 268);
        ChkType (right, grpExtStd)
      ELSIF (subcl=scNewSys) THEN        (* SYSTEM.NEW *)
        ChkType (left, grpPointer);
        ChkType (right, grpInteger)
      END;
      RETURN T.predeclType[T.strNone]
    END CheckAssign;

  BEGIN
    IF (r. left. class IN illegalNode) THEN
      S.Err (r. left. pos, 246)  (* is not a value *)
    ELSIF (r. class=ndDOp) & (r. right. class IN illegalNode) THEN
      S.Err (r. right. pos, 246)  (* is not a value *)
    ELSIF (r. left. type. form=T.strUndef) OR ((r. class=ndDOp) & (r. right. type. form=T.strUndef)) THEN
      r. type := T.predeclType[T.strUndef]
    ELSIF (r. class=ndMOp) THEN
      r. type := CheckMOp (r. subcl, r. left);
    ELSIF (r. class=ndDOp) THEN
      r. type := CheckDOp (r. subcl, r. left, r. right)
    ELSIF (r. class=ndAssign) THEN
      r. type := CheckAssign (r. subcl, r. left, r. right)
    END;
    (* constant folding *)
    IF (r. type. form#T.strUndef) & (r. left. class=ndConst) &
       ((r. class=ndMOp) & FoldMOp (r. type. form, r. subcl, r. left. conval) OR
        (r. class=ndDOp) & (r. right. class=ndConst) & FoldDOp (r. type. form, r. subcl, r. left. conval, r. right. conval)) THEN
      r := r. left;
      r. obj := NIL;
      IF (r. type. form IN intSet) THEN
        SetInt (r)
      END
    ELSIF (r. type. form = T.strBool) & (r. class = ndDOp) &
          (r. subcl IN {scAnd, scOr}) THEN
      r := FoldBool (r. subcl, r. left, r. right)
    END
  END Check;

PROCEDURE ChkVar* (n : Node);
(* Checks whether 'n' denotes a variable that can be modified (it takes also 
   import of read-only variables/record fields into consideration).
   pre: 'n' is a valid designator.
   side: An error message is written, if 'n' can't be modified. *)
  BEGIN
    CASE n. class OF
    | ndVarPar, ndDeref:                 (* always writable *)
    | ndVar, ndField:
      IF (n. class = ndField) THEN
        ChkVar (n. left)
      END;
      IF (n. obj. mnolev < T.compileMnolev) & (n. obj. mark # T.exportWrite) THEN
        S.Err (n. pos, 249)  (* no variable respectivly read-only *)
      END
    | ndIndex, ndGuard:
      ChkVar (n. left)
    ELSE
      S.Err (n. pos, 249)  (* no variable respectivly read-only *)
    END
  END ChkVar;

PROCEDURE PredefProc* (VAR call : Node; id : SHORTINT; apar : Node; numPar : INTEGER; endOfParams : LONGINT);
(* Checks parameters of predefined procedures, transfers the call into the 
   internal ndMOp / ndDOp representation.
   pre: 'id' is the procedure id of the called predefined procedure/function, 
     'apar' contains the call's parameter list (of length 'numPar').
   post: The number of parameters, their type etc in respect to the procedure
     'id' is checked, a ndMOp/ndDOp node is created in call and its type is
     set accordingly. *)
  VAR
    n : Node;
    set : SET;
    min, max : LONGINT;

  PROCEDURE ArrayDim ( t : T.Struct; dyn : BOOLEAN) : LONGINT;
  (* Counts array dimensions.
    pre: t is a type, dyn toggles whether only 'open' dimensions should be 
      counted.
    post: Result is -1 if t is no array, and n if it is a (n+1) dimensional 
      array.  With dyn=TRUE only 'open' dimensions are counted, otherwise all 
      array constructors. *)
    VAR
      i : LONGINT;
    BEGIN
      i := -1;
      WHILE (t. form=T.strDynArray) OR (t. form=T.strArray) & ~dyn DO
        INC (i);
        t := t. base
      END;
      RETURN i
    END ArrayDim;

  PROCEDURE ArrayLen (t : T.Struct; dim : LONGINT) : LONGINT;
  (* Retrieves the length of a given array dimension.
     pre: t is an array type, 0<=dim<=ArrayDim (t, FALSE)
     post: If the dimension is an 'open' one, -1 is returned. Otherwise the 
       constant length as it was declared in the type definition of t. *)
    BEGIN
      WHILE (dim#0) DO
        t := t. base;
        DEC (dim)
      END;
      IF (t. form=T.strDynArray) THEN
        RETURN -1
      ELSE
        RETURN t. len
      END
    END ArrayLen;

  PROCEDURE Find (num : INTEGER) : Node;
  (* Finds the parameter #num in the parameter list (num=1 means first param 
     etc). *)
    VAR
      n : Node;
    BEGIN
      n := apar;
      WHILE (num > 1) DO
        n := n. link;
        DEC (num)
      END;
      RETURN n
    END Find;

  PROCEDURE ChkParNum (num : INTEGER) : BOOLEAN;
  (* Test whether the procedure is called with the right number of arguments.
     If not, write an error message and return FALSE. *)
    VAR
      n : Node;
    BEGIN
      IF (num < numPar) THEN
        n := Find (num);
        S.Err (n. pos, 250)  (* less formal than actual parameters *)
      ELSIF (num > numPar) THEN
        S.Err (endOfParams, 251)  (* more formal than actual parameters *)
      END;
      RETURN (num = numPar)
    END ChkParNum;

  PROCEDURE ChkConst (num : INTEGER);
  (* Tests, if parameter num is a constant. Writes error msg if not. *)
    VAR
      n : Node;
    BEGIN
      n := Find (num);
      IF (n. class # ndConst) THEN
        S.Err (n. pos, 203)  (* has to be a constant value *)
      END
    END ChkConst;

  PROCEDURE ChkTypeVal (num : INTEGER);
  (* Tests, if parameter num is a type designator. Write error msg if not. *)
    VAR
      n : Node;
    BEGIN
      n := Find (num);
      IF (n. class#ndType) THEN
        S.Err (n. pos, 205)  (* no data type *)
      END
    END ChkTypeVal;

  PROCEDURE CreateConst (VAR n : Node; def : LONGINT);
  (* Creates a new constant node and sets its value to def. *)
    BEGIN
      n := NewNode (ndConst);
      n. conval := T.NewConst();
      n. type := T.predeclType[T.strLongInt];
      n. conval. intval := def;
      SetInt (n)
    END CreateConst;

  BEGIN
    CASE id OF
    T.predMAX, T.predMIN:  (* E.ndConst *)
      IF ChkParNum (1) THEN
        IF (apar. type. form IN realSet) THEN
          CreateConst (call, 0);
          call. type := apar. type;
          IF apar. type. form=T.strReal THEN
            IF id=T.predMAX THEN
              call. conval. real := M.maxReal
            ELSE
              call. conval. real := M.minReal
            END
          ELSE
            IF id=T.predMAX THEN
              call. conval. real := M.maxLReal
            ELSE
              call. conval. real := M.minLReal
            END
          END
        ELSE
          CASE apar. type. form OF
            T.strBool: min := M.minBool; max := M.maxBool |
            T.strChar: min := M.minChar; max := M.maxChar |
            T.strShortInt: min := M.minSInt; max := M.maxSInt |
            T.strInteger: min := M.minInt; max := M.maxInt |
            T.strLongInt: min := M.minLInt; max := M.maxLInt |
            T.strSet: min := M.minSet; max := M.maxSet |
          ELSE
            S.Err (apar. pos, 254);  (* standard data type expected *)
            min := 0;
            max := 0
          END;
          IF (id=T.predMAX) THEN
            CreateConst (call, max)
          ELSE
            CreateConst (call, min)
          END;
          SetInt (call)
        END
      END |
    T.predCHR, T.predENTIER, T.predLONG, T.predORD, T.predSHORT:  (* E.ndConv *)
      IF ChkParNum (1) THEN
        CASE id OF
          T.predCHR: set := intSet |
          T.predENTIER: set := realSet |
          T.predLONG: set := {T.strShortInt, T.strInteger, T.strReal} |
          T.predORD: set := {T.strChar} |
          T.predSHORT: set := {T.strLongInt, T.strInteger, T.strLongReal} |
        END;
        IF ~(apar. type. form IN set) THEN
          ErrNT1 (171, apar);  (* function not defined for this type *)
          apar. type := T.predeclType[T.strUndef]
        ELSE
          CASE id OF
            T.predCHR: Convert (apar, T.strChar) |
            T.predENTIER: Convert (apar, T.strLongInt) |
            T.predLONG: Convert (apar, apar. type. form+1) |
            T.predORD: Convert (apar, T.strInteger)  |
            T.predSHORT: Convert (apar, apar. type. form-1) |
          END
        END
      ELSE
        apar. type := T.predeclType[T.strUndef]
      END;
      call := apar |
    T.predABS, T.predCAP, T.predODD, T.sysADR, T.sysCC:  (* E.ndMOp *)
      call := NewNode (ndMOp);
      IF ChkParNum (1) THEN
        call. left := apar;
        call. subcl := id;
        Check (call);
        IF (id=T.sysADR) & ~(apar. class IN {ndVar, ndVarPar, ndField, ndDeref, ndIndex, ndGuard, ndTBProc, ndProc}) THEN
          S.Err (apar. pos, 255)  (* variable expected *)
        ELSIF (id=T.sysCC) & (apar. class#ndConst) THEN
          S.Err (apar. pos, 203)  (* has to be constant *)
        END;
        IF (id = T.sysADR) & (apar. class IN {ndVar, ndVarPar, ndField, ndTBProc, ndProc}) THEN
          INCL (apar. obj. flags, T.flagAddressed)
        END
      END |
    T.predASH, T.sysBIT, T.sysLSH, T.sysROT:  (* E.ndDOp *)
      call := NewNode (ndDOp);
      call. subcl := id;
      IF ChkParNum (2) THEN
        call. left := apar;
        call. right := apar. link;
        Check (call)
      END |
    T.predSIZE:
      call := NewNode (ndConst);
      call. conval := T.NewConst();
      call. type := T.predeclType[T.strLongInt];
      IF ChkParNum (1) THEN
        ChkTypeVal (1);
        min := apar. type. size;
        IF (min>=0) THEN  (* size known at compile time *)
          call. conval. intval := min;
          SetInt (call)
        ELSIF (min=-1) THEN  (* size is a constant, but not known here *)
          call. class := ndMOp;
          call. subcl := scSize;
          call. left := apar
        ELSE  (* size not computable (open array) *)
          S.Err (apar. pos, 258)  (* SIZE not computable for this type *)
        END
      END |
    T.predLEN:
      call := NewNode (ndConst);
      call. type := T.predeclType[T.strLongInt];
      call. conval := T.NewConst();
      call. left := apar;
      IF (numPar = 1) THEN  (* provide default value of second parameter *)
        CreateConst (apar. link, 0);
        INC (numPar)
      END;
      IF ChkParNum (2) THEN
        min := ArrayDim (apar. type, FALSE);
        IF (min < 0) THEN
          ErrNT1 (151, apar)  (* has to be array *)
        END;
        call. right := apar. link;
        ChkType (call. right, grpInteger);
        ChkConst (2);
        IF (call. right. conval. intval<0) OR (call. right. conval. intval>ArrayDim (apar. type, FALSE)) THEN
          S.Err (call. right. pos, 259)  (* illegal dimension *)
        ELSIF (call. left. type. form = T.strDynArray) &
              (T.flagExternal IN call. left. type. flags) THEN
          S.Err (call. left. pos, 153)  (* this array has no length information *)
        ELSIF (call. right. conval. intval<=ArrayDim (apar. type, TRUE)) THEN
          call. class := ndDOp;
          call. subcl := scLen
        ELSE
          call. conval. intval := ArrayLen (call. left. type, call. right. conval. intval)
        END
      END |
    T.sysVAL:
      call := NewNode (ndMOp);
      call. subcl := scVal;
      IF ChkParNum (2) THEN
        ChkTypeVal (1);
        call. left := apar. link;
        call. type := apar. type;
      END |
    T.predINCL, T.predEXCL, T.predCOPY, T.predINC, T.predDEC, 
    T.sysGET, T.sysPUT, T.sysGETREG, T.sysPUTREG, T.sysNEW:
      call := NewNode (ndAssign);
      call. subcl := id;
      IF ((id=T.predINC) OR (id=T.predDEC)) & (numPar=1) THEN  
        (* provide default value of second parameter *)
        CreateConst (apar. link, 1);
        INC (numPar)
      END;
      IF ChkParNum (2) THEN
        call. left := apar;
        call. right := apar. link;
        IF (id#T.sysPUT) & (id#T.sysGET) & (id#T.sysPUTREG) THEN
          IF (id=T.predCOPY) THEN
            ChkVar (Find (2))
          ELSE
            ChkVar (Find (1))
          END
        END;
        IF (id=T.sysPUTREG) OR (id=T.sysGETREG) THEN
          ChkConst (1)
        END;
        Check (call)
      END |
    T.predNEW:
      call := NewNode (ndAssign);
      call. subcl := scNewFix;
      call. left := apar;
      IF (apar # NIL) THEN
        ChkType (apar, T.strPointer);
        IF (T.flagExternal IN apar. type. flags) OR
           (apar. type. base # NIL) & (T.flagExternal IN apar. type. base. flags) THEN
          S.Err (apar. pos, 154)         (* NEW not appl to ext types *)
        END
      END;
      IF ChkParNum (SHORT (ArrayDim (apar. type. base, TRUE)+2)) THEN
        ChkType (apar, T.strPointer);
        ChkVar (Find (1));
        IF (numPar > 1) THEN
          call. subcl := scNewDyn;
          call. right := apar. link;
          n := call. right;
          WHILE (n # NIL) DO
            ChkType (n, grpInteger);
            n := n. link
          END
        END
      END |
    T.sysDISPOSE:
      call := NewNode (ndAssign);
      call. subcl := scDispose;
      call. left := apar;
      IF ChkParNum (1) THEN  (* SYSTEM.DISPOSE (ptr) *)
        ChkType (apar, grpPointer);
        IF (T.flagExternal IN apar. type. flags) OR
           (apar. type. base # NIL) & (T.flagExternal IN apar. type. base. flags) THEN
          S.Err (apar. pos, 155)         (* DISPOSE not appl to ext types *)
        END
      END |
    T.sysCOLLECT:
      call := NewNode (ndAssign);
      call. subcl := scCollect;
      IF ChkParNum (0) THEN
      END |
    T.predHALT:
      call := NewNode (ndTrap);
      IF ChkParNum (1) THEN
        call. left := apar;
        ChkConst (1);
        ChkType (apar, grpInteger);
        ChkRange (apar, M.minTrapNum, M.maxTrapNum, 267)  (* illegal trap number *)
      END |
    T.predASSERT:
      call := NewNode (ndAssert);
      IF (numPar = 1) THEN
        CreateConst (apar. link, M.defAssertTrap);
        INC (numPar)
      END;
      IF ChkParNum (2) THEN
        call. right := apar;
        ChkType (apar, T.strBool);
        call. left := apar. link;
        ChkConst (2);
        ChkType (call. left, grpInteger);
        ChkRange (call. left, M.minTrapNum, M.maxTrapNum, 267)  (* illegal trap number *)
      END |
    T.sysMOVE:
      call := NewNode (ndAssign);
      call. subcl := scMove;
      call. right := apar;
      IF ChkParNum (3) THEN
        ChkType (apar, T.strLongInt);
        ChkType (apar. link, T.strLongInt);
        ChkType (apar. link. link, grpInteger)
      END
    END
  END PredefProc;


PROCEDURE RecycleMem* (VAR node : Node);
  BEGIN
    IF (node # NIL) & (node. class # ndCollected) THEN
      node. class := ndCollected;
      RecycleMem (node. left);
      RecycleMem (node. right);
      RecycleMem (node. link);
      node. link := nodeHeap;
      nodeHeap := node;
      node := NIL
    END
  END RecycleMem;

BEGIN
  nodeHeap := NIL
END OEParse.
