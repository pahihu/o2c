MODULE OParse;  (* Author: Michael van Acken *)
(* 	$Id: OParse.Mod,v 1.54 1997/03/27 09:01:12 oberon1 Exp $	 *)

IMPORT
  M := OMachine, S := OScan, T := OTable, E := OEParse, OSym, Str := Strings, 
  Filenames, Strings;


VAR
  root : E.Node;  (* root of syntax tree, used to insert 'init type descriptor' nodes *)
  undeclIdents: ARRAY 512 OF CHAR;
  

PROCEDURE CheckSym (s : SHORTINT);
  (* If s=S.sym then get next symbol, otherwise write error msg and call 
     GetSym only with certain s/S.sym combinations. *)
  BEGIN
    IF (S.sym = s) THEN
      S.GetSym
    ELSE
      E.Err (100+LONG (s));  (* just numerate the error messages according to the token codes *)
      IF (s=S.becomes) & ((S.sym=S.eql) OR (S.sym=S.colon)) OR
         (s=S.eql) & (S.sym=S.becomes) OR
         ((S.of<=s) & (s<=S.by) OR (S.end<=s) & (s<=S.module)) & (S.sym = S.ident) THEN
        S.GetSym
      END
    END
  END CheckSym;

PROCEDURE NewObject (name: ARRAY OF CHAR; mode: SHORTINT) : T.Object;
  VAR
    obj : T.Object; 
  BEGIN
    obj := T.NewObject (name, mode, S.lastSym);
    IF T.external THEN
      INCL (obj. flags, T.flagExternal)
    END;
    RETURN obj
  END NewObject;

PROCEDURE NewStruct (form : SHORTINT): T.Struct;
  VAR
    s : T.Struct;
  BEGIN
    s := T.NewStruct (form);
    IF T.external THEN
      INCL (s. flags, T.flagExternal)
    END;
    RETURN s
  END NewStruct;
  
    
PROCEDURE Qualident (VAR obj : T.Object; VAR pos : LONGINT);
(* Reads a (possibly quailified) identifier at the current position in the 
   token stream.
   post: obj contains the denoted (possibly imported) object, pos the current 
   position in the source code. If the current token is not an identifieror not
   a declared object, obj is set to a variable with the type 'undef'. *)
  VAR
    identName: ARRAY M.maxSizeIdent*2+2 OF CHAR;
     saveUnderscoreState:BOOLEAN;
  BEGIN
    pos := S.lastSym;
    identName := "|";
    saveUnderscoreState := S.underscore;
    IF (S.sym = S.ident) THEN  (* identifier, search current scope for it *)
      obj := T.Find (S.ref);
      Strings.Append (S.ref, identName);
      S.GetSym;
      (* if obj can't be found and '.' follows, skip two tokens (period+ident) *)
      IF (obj = NIL) & (S.sym = S.period) THEN
        S.GetSym;  (* skip period *)
        S.GetSym   (* probably identifier, skip it *)
      END
    ELSE  (* no identifier, syntax error *)
      obj := NIL
    END;
    IF (obj # NIL) THEN  
      (* a declaration of this identifier exists *)
      IF (obj. mode = T.objForwardType) THEN  
        obj := NIL                       (* forward declared types officially don't exist *)
      ELSIF (obj. mode = T.objModule) THEN  (* we got a module here, read the imported object *)
        (* allow underscores for imports from EXTERNAL *)
        S.underscore := S.underscore OR (T.flagExternal IN obj. flags);
        CheckSym (S.period);
        Strings.Append (".", identName);
        Strings.Append (S.ref, identName);
        IF (S.sym=S.ident) THEN  (* search for the identifier in the module's scope *)
          pos := S.lastSym;
          obj := T.FindImport (S.ref, obj);
          S.GetSym
        ELSE
          identName := "|||";            (* ignore any previous error *)
          obj := NIL
        END;
        S.underscore := saveUnderscoreState (* disable '_' if necessary *)
      END
    END;
    IF (obj = NIL) THEN  
      (* no legal object found: give error message (suppress it if a message
         of same text has been printed before) and fill obj with a dummy 
         variable *)
      Strings.Append ("|", identName);
      IF (Strings.Pos (identName, undeclIdents, 0) >= 0) THEN
        (* write 'empty error': no text, but remember error position *)
        S.lastErr := pos
      ELSE
        S.Err (pos, 204);                (* identifier not declared *)
        Strings.Append (identName, undeclIdents)
      END;
      obj := NewObject (S.undefStr, T.objVar)
    END
  END Qualident;

PROCEDURE ^ Type (VAR t : T.Struct; VAR pos : LONGINT; exp : SHORTINT);

PROCEDURE FormalPars (VAR fpars : T.Struct);
(* Reads a formal parameter list and returns its value as a type structure.
   post: If current token is not '(', then the parameter list is considered to be empty (no parameters and
     no function result). Otherwise the parameters inside the enclosing parenthesis are parsed, possibly
     followed by the functions result type. *)
  VAR
    first, last, obj : T.Object;
    mode : SHORTINT;
    pos : LONGINT;
    resynch : BOOLEAN;
  BEGIN
    fpars := NewStruct (T.strProc);  (* initialize fpar with an empty parameter list *)
    IF (S.sym=S.lParen) THEN  (* list of formal parameters exists (even if it is empty) *)
      S.GetSym;
      IF (S.sym=S.var) OR (S.sym=S.ident) OR 
         M.allowRestParam & T.external & (S.sym=S.upto) THEN  (* list is not empty *)
        last := NIL;
        resynch := FALSE;
        LOOP
          IF M.allowRestParam & T.external & (S.sym=S.upto) THEN   (* generic rest parameter *)
            obj := NewObject (T.genericParam, T.objVar);
            obj. type := T.predeclType[T.strNone];
            IF (last=NIL) THEN
              fpars. link := obj
            ELSE
              last. link := obj
            END;            
            S.GetSym;
            INC (fpars. len);
            EXIT
          ELSE
            (* First check if the identifier list is preceded by a VAR modifier. Set mode accordingly. *)
            IF (S.sym=S.var) THEN
              mode := T.objVarPar;
              S.GetSym
            ELSE
              mode := T.objVar
            END;
            (* Read list of identifiers and create an object for each entry.
               first and last mark the start/end of the object list. *)
            first := last;
            LOOP
              (* Parse identifiers seperated by a comma. A missing comma marks the end of the list.
                 keeps track of last identifier in list. Concatenate objects with obj.link field. *)
              IF (S.sym=S.ident) THEN
                (* Create new object, link the very first one to fpars and keep count of the parameters in fpars.len *)
                IF (last=NIL) THEN
                  fpars. link := NewObject (S.ref, mode);
                  last := fpars. link
                ELSE
                  last. link := NewObject (S.ref, mode);
                  last := last. link
                END;
                S.GetSym;
                INC (fpars. len)
              ELSE
                E.Err (100);  (* identifier expected *)
                S.GetSym;
                WHILE (S.sym # S.rParen) & (S.sym # S.end) & (S.sym < S.begin) DO
                  S.GetSym
                END;
                resynch := TRUE
              END;
              IF (S.sym#S.comma) & (S.sym#S.ident) OR resynch THEN
                EXIT
              END;
              CheckSym (S.comma)
            END;
            IF resynch THEN
              EXIT
            END;
            (* Read type declaration following the identifiers. *)
            CheckSym (S.colon);
            Type (last. type, pos, T.exportNot);
            IF (last. type. obj = NIL) & ~(last. type. form IN {T.strUndef, T.strDynArray, T.strProc}) THEN
              S.Err (pos, 212)  (* anonymous type cannot be used here *)
            END;
            (* Set the type for each identifier in the preceding list. *)
            IF (first=NIL) THEN
              first := fpars. link
            ELSE
              first := first. link
            END;
            WHILE (first # last) DO
              first. type := last. type;
              first := first. link
            END;
            IF (S.sym#S.semicolon) & (S.sym#S.ident) & (S.sym#S.var) THEN
              EXIT
            END;
            CheckSym (S.semicolon)
          END
        END
      END;
      CheckSym (S.rParen);
      IF (S.sym=S.colon) THEN  (* formal parameters describe a real function: parse type *)
        S.GetSym;
        Type (fpars. base, pos, T.exportNot);  (* read result type *)
        IF ~(fpars. base. form IN {T.strBool..T.strSet, T.strSysByte..T.strProc}) THEN
          (* check whether type is a legal function result *)
          S.Err (pos, 221)  (* illegal type as function result *)
        ELSIF (fpars. base. obj = NIL) THEN
          S.Err (pos, 212)  (* anonymous type cannot be used here *)
        END;
        fpars. size := T.structSize (fpars);
        RETURN
      END
    END;
    fpars. base := T.predeclType[T.strNone];
    fpars. size := T.structSize (fpars);
  END FormalPars;

PROCEDURE ^ Expr (VAR r : E.Node);

PROCEDURE ConstExpr (VAR const : T.Const; VAR type : T.Struct);
(* Reads an expression and returns its constand value and its type.
   post: If expression can be evaluated during parsing (i.e. it is composed of constants and operations
     on constants), return the value in const, the constant's type in (guess what) type.
     If its not a constant, provide dummy value in const, set type to 'undef'. *)
  VAR
    node : E.Node;
  BEGIN
    Expr (node);
    IF (node. class # E.ndConst) THEN  (* no constant *)
      S.Err (node. pos, 203);  (* has to be constant *)
      const := T.NewConst();  (* set const/type to 'useful' values *)
      type := T.predeclType[T.strUndef]
    ELSE  (* node is a constant, copy the value into const/type *)
      E.SetInt (node);
      const := node. conval;
      type := node. type
    END
  END ConstExpr;

PROCEDURE Designator (VAR n : E.Node);
(* Reads a (qualified) identifier and a sequence of selectors following it.
   post: n contains the complete designator. It is a list with one or more 
     elements, concatenated with n.left. The list is stored in reverse order, 
     the initial variable standing at the end of the list, the most recent 
     selector at the head (i.e. in n), *)
  VAR
    n0 : E.Node;
    obj : T.Object;
    pos : LONGINT;
    saveUnderscoreState: BOOLEAN;

  PROCEDURE Deref (chkPtr : BOOLEAN);
    BEGIN
      IF (n. type. form=T.strPointer) THEN  (* create deref node *)
        n := E.NewNode (E.ndDeref);
        n. left := n0;
        n. type := n0. type. base;
        n0 := n
      ELSIF chkPtr THEN  (* n is no pointer: only create error msg if required *)
        S.Err (n. pos, 213)  (* no pointer *)
      END
    END Deref;

  BEGIN
    (* Read the initial (qualified) identifier, initialize the node n accordingly. *)
     n := E.NewNode (E.ndVar);
    Qualident (obj, n. pos);
    n. obj := obj;
    n. type := obj. type;
    IF (obj. mode=T.objConst) THEN
      n. class := E.ndConst;
      n. conval := T.NewConst();  (* copy the value so that it can be modified later *)
      n. conval^ := obj. const^;
      IF (T.importMnolev < obj. mnolev) & (obj. mnolev < T.compileMnolev) THEN
        n. obj := NIL   (* predef constants have no declarations in the C code *)
      END
    ELSIF (obj. mode=T.objVarPar) THEN
      n. class := E.ndVarPar
    ELSIF (obj. mode=T.objType) THEN
      n. class := E.ndType;
      RETURN
    ELSIF (obj. mode IN {T.objLocalProc, T.objIntProc, T.objExtProc}) THEN
      n. class := E.ndProc
    END;
    LOOP  (* loop over the list of selectors *)
      IF (n. type. form = T.strProc) THEN
        EXIT  (* leave, so that a type guard isn't mixed up with the procedure call's parameter list *)
      END;
      n0 := n;  (* n0 always points to the last node installed *)
      IF (S.sym=S.period) THEN  (* field (or type bound procedure) selector *)
        Deref (FALSE);(* automatic deref of pointers, n0 designates a record *)
        saveUnderscoreState := S.underscore;
        S.underscore := T.external OR (T.flagExternal IN n. type. flags);
        S.GetSym;
        S.underscore := saveUnderscoreState;
        IF (n. type. form # T.strRecord) THEN  (* due to auto deref n should now denote a record *)
          E.ErrNT1 (150, n)  (* not a record *)
        ELSIF (S.sym # S.ident) THEN  (* missing field/proc name *)
          E.Err (100)  (* identifier expected *)
        ELSE  (* n is a record and the current token is an identifier *)
          obj := T.FindField (S.ref, n. type);
          IF (obj # NIL) THEN  (* a field named S.ref exists *)
            (* add a new node of class ndField (preliminary) *)
            n := E.NewNode (E.ndField);
            n. left := n0;
            n. type := obj. type;
            n. obj := obj;
            IF (obj. mode # T.objField) THEN  (* no field, but rather a type bound procedure: correct node class *)
              n. class := E.ndTBProc;
              IF (n0. class = E.ndDeref) & (obj. type. link. type. form = T.strPointer) THEN
                n. left := n. left. left  (* receiver is a pointer type, undo hasty dereference *)
              ELSIF (obj. type. link. type. form # n0. type. form) THEN
                E.Err (245)  (* conflict in receiver types (should be record/record or pointer/pointer) *)
              END;
              S.GetSym;
              IF (S.sym = S.arrow) THEN  (* marker for super call found *)
                IF (n. left. obj = NIL) OR ~(T.flagReceiver IN n. left. obj. flags) OR 
                   (n0. type. base = NIL) OR 
                   (T.FindField (S. ref, n0. type. base) = NIL) THEN
                  E.Err (248)  (* invalid call of base procedure *)
                ELSE
                  n. class := E.ndTBSuper
                END;
                S.GetSym
              END;
              RETURN
            END
          ELSE
            E.Err (204)  (* identifier not declared *)
          END
        END;
        S.GetSym
      ELSIF (S.sym = S.lBrak) THEN  (* array index *)
        Deref (FALSE);
        REPEAT
          (* test, whether n is an array *)
          IF (n. type. form # T.strArray) & (n. type. form # T.strDynArray) THEN
            E.ErrNT1 (151, n)  (* not an array *)
          END;
          pos := S.lastSym;
          S.GetSym;  (* skip '[' respectively ', ' *)
          (* create index node, set its type to the array's base type *)
          n := E.NewNode (E.ndIndex);
          n. left := n0;
          n. type := n0. type. base;
          n. pos := pos;
          n0 := n;
          Expr (n. right);  (* read index expression *)
          E.ChkType (n. right, E.grpInteger) (* index has to be an integer expression *)
        UNTIL (S.sym # S.comma);
        CheckSym (S.rBrak)
      ELSIF (S.sym=S.arrow) THEN  (* pointer dereference *)
        Deref (TRUE);
        S.GetSym
      ELSIF (S.sym = S.lParen) & (n. type. form # T.strUndef) THEN
        n := E.NewNode (E.ndGuard);
        n. left := n0;
        S.GetSym;
        Qualident (n. obj, n. pos);
        IF E.ChkGuard (n0, n. obj, FALSE, n. pos) THEN
          n. type := n. obj. type
        ELSE
          n. type := T.predeclType[T.strUndef]
        END;
        CheckSym (S.rParen)
      ELSE
        EXIT
      END
    END
  END Designator;

PROCEDURE ProcCall (VAR call : E.Node; proc : E. Node);
(* Parses the actual parameter list and compares formal to actual parameters.
   pre: proc contains a procedure designator.
   post: The parameter call returns the representation of the procedure call.
     If proc designates a predefined procedure the call is translated into a
     ndMOp/ndDOp/ndAssign node. If it is a normal procedure then return a
     'ndCall' node. Otherwise fill call with a node of type 'ndCall' with an 
     'undef' result type. *)
  VAR
    apar : E.Node;
    fpar : T.Object;
    ftype : T.Struct;
    fform : SHORTINT;
    numPar : INTEGER;
    comp : BOOLEAN;
    endOfParams : LONGINT;
    
  PROCEDURE ExprList (VAR node : E.Node);
    BEGIN
      INC (numPar);
      Expr (node);
      WHILE (S.sym=S.comma) OR (S.lParen<=S.sym) & (S.sym<=S.ident) DO
        CheckSym (S.comma);
        ExprList (node. link)
      END
    END ExprList;

  BEGIN
    IF (proc. class # E.ndProc) & ~(proc. type. form IN {T.strProc, T.strTBProc}) THEN
      E.ErrNT1 (174, proc)  (* procedure expected *)
    END;
    apar := NIL;
    numPar := 0;
    (* Read list of actual parameters. *)
    IF (S.sym < S.semicolon) THEN
      CheckSym (S.lParen);
      IF (S.sym # S.rParen) THEN
        ExprList (apar)
      END;
      endOfParams := S.lastSym;
      CheckSym (S.rParen)
    ELSE
      endOfParams := S.lastSym
    END;
    call := E.NewNode (E.ndCall);
    call. left := proc;
    call. right := apar;
    IF (proc. class = E.ndProc) & (proc. obj. const # NIL) THEN  
      (* predefined procedure *)
      E.PredefProc (call, SHORT (SHORT (proc. obj. const. intval)), 
                    apar, numPar, endOfParams)
    ELSIF (proc. type. form IN {T.strProc, T.strTBProc}) THEN  
      (* 'normal' procedure call *)
      (* Store the first formal parameter in fpar. *)
      fpar := proc. type. link;
      IF (proc. type. form = T.strTBProc) THEN
        fpar := fpar. link
      END;
      WHILE (apar # NIL) DO  (* compare formal to actual parameters *)
        IF (fpar=NIL) THEN
          S.Err (apar. pos, 250)  (* fewer formal than actual parameters *)
        ELSIF (fpar. type. form # T.strNone) THEN
          ftype := fpar. type;
          fform := ftype. form;
          (* emit warning, if length information is lost when passing an
             external array to a formal Oberon-2 open array parameter *)
          IF (fform = T.strDynArray) & (apar. type. form = T.strDynArray) & 
             (T.flagExternal IN apar. type. flags) THEN
            S.Warn (apar. pos, 296)  
            (* Warning: Passing external array without length information *)
          END;
          (* check parameter according to 10.1 and Appendix C *)
          IF (fpar. mode=T.objVar) THEN  (* value parameter *)
            comp :=
              (fform=T.strDynArray) & E.ArrayComp (apar, ftype) OR
              E.AssignComp (ftype, apar)
            (* AssignComp will write an error msg and do a type conversion if 
               necessary *)
          ELSE  (* variable parameter *)
            E.ChkVar (apar);
            IF ~(
              (ftype=apar. type) OR
              (fform=T.strRecord) & T.ExtOf (apar. type, ftype) OR
              (fform=T.strDynArray) & ((ftype. base. form=T.strSysByte) OR E.ArrayComp (apar, ftype)) OR
              (fform=T.strSysByte) & ((apar. type. form=T.strShortInt) OR (apar. type. form=T.strChar)) OR
              (fform=T.strSysPtr) & (apar. type. form=T.strPointer)) THEN
              E.ErrT1 (apar. pos, 168, ftype)  
              (* not compatible to formal VAR parameter *)
            END;
            IF (apar. class IN {E.ndVar, E.ndVarPar, E.ndField, E.ndTBProc, E.ndProc}) THEN
              INCL (apar. obj. flags, T.flagAddressed)
            END
          END;
          fpar := fpar. link
        END;
        apar := apar. link
      END;
      IF (fpar # NIL) & (fpar. type. form # T.strNone) THEN
        S.Err (endOfParams, 251)  (* more formal than actual parameters *)
      END;
      call. type := proc. type. base
    ELSE
      call. type := T.predeclType[T.strUndef]
    END;
    call. pos := proc. pos
  END ProcCall;

PROCEDURE Expr (VAR r : E.Node);
(* Parses an expression. Even if an error occurs it returns a valid operator 
   tree in r and sets its type to 'undef'. Constant folding is automatically 
   applied to the expression. *)
  VAR
    ok : BOOLEAN;

 PROCEDURE DOp (VAR r : E.Node; sc : SHORTINT);
 (* pre: r contains initially the new left hand expression.
    post: In r the new two-sided expression (class=ndDOp) of the subclass sc.
      The initial value of r is the new left hand side.
      The current token is skipped (with GetSym). *)
   VAR
     n : E.Node;
   BEGIN
     n := r;
     r := E.NewNode (E.ndDOp);
     r. left := n;
     r.subcl := sc;
     S.GetSym
   END DOp;

 PROCEDURE SimpleExpr (VAR r : E.Node);
    PROCEDURE Term (VAR r : E.Node);
      PROCEDURE Factor (VAR r : E.Node);
      (* Reads a factor, returns the value in r.
         post: If the current token can be hte prefix of a factor then skip 
           tokens until a valid one (or a keyword) is found. If no factor is 
           encountered, return a dummy variable (type 'undef') in r. Otherwise
           parse the factor and return the operator tree in r. *)

        PROCEDURE Element (VAR range : E.Node);
        (* Read a set's index range. If the intervall's bounds are constants 
           construct the set constant directly. *)
          VAR
            i : LONGINT;

          PROCEDURE Limit (VAR limit : E.Node);
          (* Read a set index, check its type and (if it is a constant) it is 
             inside the system limits for set indices. *)
            BEGIN
              Expr (limit);
              E.ChkType (limit, E.grpInteger);
              IF (limit. type. form # T.strUndef) & (limit. class = E.ndConst) THEN
                E.ChkRange (limit, M.minSet, M.maxSet, 229)
              END
            END Limit;

          BEGIN
            range := E.NewNode (E.ndUpto);
            Limit (range. left);  (* read lower bound *)
            IF (S.sym=S.upto) THEN  (* it's an intervall, read upper bound *)
              S.GetSym;
              Limit (range. right)
            ELSE  (* no intervall: lower bound equals upper bound *)
              range. right := range. left
            END;
            IF (range. left. class=E.ndConst) & (range. right. class=E.ndConst) THEN  (* constant intervall? *)
              IF (range. left. conval. intval > range. right. conval. intval) THEN  (* invalid intervall bounds *)
                S.Err (range. left. pos, 230)  (* lower bound > upper bound *)
              END;
              (* Create the set constant described by [left..right]. *)
              FOR i := range. left. conval. intval TO range. right. conval. intval DO
                INCL (range. left. conval. set, i)
              END;
              range := range. left;
              range. obj := NIL
            END;
            range. type := T.predeclType[T.strSet]
          END Element;

        BEGIN
          IF (S.sym < S.lParen) THEN
            (* The current token is no legal prefix of a factor, so skip all 
               tokens until a legal token is reached or a synchronizing 
               keyword is found. *)
            E.Err (147); (* factor starts with illegal symbol *)
            REPEAT
              S.GetSym
            UNTIL (S.sym >= S.lParen)
          END;
          IF (S.sym=S.ident) THEN  (* designator or function call *)
            Designator (r);
            IF (S.sym=S.lParen) THEN  (* function call *)
              ProcCall (r, r);
              IF (r. type. form = T.strNone) &
                 (r. type. form # T.strUndef) THEN  
               (* test for function (i.e. a result type is set *)
                S.Err (r. pos, 265)  (* not a function *)
              END
            END
          ELSIF (S.sym=S.number) THEN  
            (* numeric constant (integer, real or long real) *)
            r := E.NewNode (E.ndConst);
            r. conval := T.NewConst();
            CASE S.numType OF
            | S.numInt:
              r. conval. intval := S.intVal;
              r. type := T.predeclType[T.strLongInt]
            | S.numReal:
              r. conval. real := S.realVal;
              r. type := T.predeclType[T.strReal]
            | S.numLReal:
              r. conval. real := S.realVal;
              r. type := T.predeclType[T.strLongReal]
            END;
            E.SetInt (r);  (* set integer type according to the value *)
            S.GetSym
          ELSIF (S.sym=S.string) THEN  (* string or character constant *)
            r := E.NewNode (E.ndConst);
            r. conval := T.NewConst();
            IF (Str.Length (S.ref) <= 1) THEN
              r. conval. intval := S.intVal;  (* char value *)
              r. type := T.predeclType[T.strChar] 
              (* consider `r' to be a character (may later be converted 
                 to string) *)
            ELSE
              NEW (r. conval. string);
              COPY (S.ref, r. conval. string^);  (* string value *)
              r. type := T.predeclType[T.strString]  (* is a string constant *)
            END;
            S.GetSym
          ELSIF (S.sym=S.nil) THEN  (* nil constant *)
            r := E.NewNode (E.ndConst);
            r. conval := T.NewConst();
            r. type := T.predeclType[T.strNil];
            S.GetSym
          ELSIF (S.sym=S.lBrace) THEN  (* set constructor *)
            S.GetSym;
            IF (S.sym # S.rBrace) THEN  (* parse list of set elements *)
              Element (r);
              (* multiple elements -seperated by comma- are transfered into 
                 sequence of set union operations. *)
              WHILE (S.sym=S.comma) DO  
                (* multiple elements, seperate by comma *)
                DOp (r, E.scPlus);  (* new union operator *)
                Element (r. right);
                E.Check (r)  (* called for constant folding *)
              END
            ELSE  (* empty set {} *)
              r := E.NewNode (E.ndConst);
              r. type := T.predeclType[T.strSet];
              r. conval := T.NewConst()
            END;
            CheckSym (S.rBrace)
          ELSIF (S.sym=S.lParen) THEN  (* expression in parenthesis *)
            S.GetSym;
            Expr (r);
            CheckSym (S.rParen)
          ELSIF (S.sym=S.not) THEN  (* negation operator *)
            r := E.NewNode (E.ndMOp);
            r. subcl := E.scNot;
            S.GetSym;
            Factor (r. left);
            E.Check (r)  (* check type, try constant folding *)
          ELSE  
            (* error: no legal factor, create a dummy var of type 'undef' *)
            E.Err (147);  (* factor starts with illegal symbol *)
            r := E.NewNode (E.ndVar);
            r. type := T.predeclType[T.strUndef];
            S.GetSym                 (* skip token to prevent infinite loop *)
          END
        END Factor;

      BEGIN
        Factor (r);
        LOOP
          IF (S.sym<S.times) OR (S.sym>S.and) THEN  
            (* no multiplication operator, leave Term *)
            EXIT
          END;
          DOp (r, S.sym);
          Factor (r. right);
          E.Check (r)
        END
      END Term;

    BEGIN
      IF (S.sym=S.minus) THEN  (* monadic minus *)
        r := E.NewNode (E.ndMOp);
        r. subcl := E.scMinus;
        S.GetSym;
        Term (r. left);
        E.Check (r)
      ELSE
        IF (S.sym = S.plus) THEN  (* meaningless '+' prefix, skip it *)
          S.GetSym
        END;
        Term (r)
      END;
      LOOP
        IF (S.sym<S.plus) OR (S.sym>S.or) THEN  
          (* no addition operator, leave SimpleExpr *)
          EXIT
        END;
        DOp (r, S.sym);
        Term (r. right);
        E.Check (r)
      END
    END SimpleExpr;

  BEGIN
    SimpleExpr (r);
    IF (S.sym<S.eql) OR (S.sym>S.is) THEN  (* no relation, leave Expr *)
      RETURN
    END;
    DOp (r, S.sym);
    SimpleExpr (r. right);
    IF (r. subcl=E.scIs) THEN (* check the special case of the 'IS' operator *)
      r. type := T.predeclType[T.strBool];
      ok := E.ChkGuard (r. left, r. right. obj, FALSE, r. right. pos)
    ELSE  (* check for all other cases *)
      E.Check (r)
    END
  END Expr;


PROCEDURE StatementSeq (VAR r : E.Node; proc, loop : E.Node);
(* Reads a sequence of statements.
   pre: proc points to the enclosing procedure (or to the module node), 
     loop to the most recent enclosing LOOP statement (NIL if there isn't any).
   post: The head of the list of statements is stored in r, the statements are 
     linked with Node.link. *)
  VAR
    last : E.Node;

  PROCEDURE Statement (VAR node : E.Node);
    VAR
      n : E.Node;
      pos : LONGINT;
      comp : BOOLEAN;
      prevType : T.Struct;

    PROCEDURE CaseLabels (VAR list : E.Node; range : T.Const);
      PROCEDURE Overlapping (lower, upper : LONGINT);
        (* pre: lower, upper are the bounds of the latest parsed intervall.
           side: If one of the value in [lower..upper] is already used by a 
             previous declared case label an error message is written. *)
        VAR
          branch, label : E.Node;
        BEGIN
          branch := node. right. left;
          WHILE (branch # NIL) DO  (* loop over all branches *)
            label := branch. link;
            WHILE (label#NIL) DO  (* loop over all labels i a single branch *)
              IF ~((upper < label. conval. intval) OR (lower > label. conval. intval2) OR (label=list)) THEN
                S.Err (list. pos, 252);  (* overlapping case labels *)
                RETURN
              END;
              label := label. link
            END;
            branch := branch. left
          END
        END Overlapping;

      PROCEDURE Limit (VAR limit : LONGINT);  (* Read a single case value. *)
        VAR
          const : T.Const;
          type : T.Struct;
        BEGIN
          (* read constant expression *)
          ConstExpr (const, type);
          limit := const. intval;
          (* check for type compability with case expression *)
          IF (node. left. type. form IN E.intSet) &
             (~(type. form IN E.intSet) OR (type. form > node. left. type. form))  OR
             (node. left. type. form = T.strChar) & (type. form # T.strChar) THEN
            E.Err (253)  (* label not compatible to case expression *)
          END;
          (* keep track of the range of case labels *)
          IF (limit < range. intval) THEN
            range. intval := limit
          END;
          IF (limit > range. intval2) THEN
            range. intval2 := limit
          END
        END Limit;

      BEGIN
        list := E.NewNode (E.ndConst);
        list. conval := T.NewConst();
        list. type := node. left. type;
        Limit (list. conval. intval);  (* lower bound *)
        IF (S.sym=S.upto) THEN  (* a intervall is given *)
          S.GetSym;
          Limit (list. conval. intval2)  (* upper bound *)
        ELSE  (* otherwise: just a single value *)
          list. conval. intval2 := list. conval. intval
        END;
        Overlapping (list. conval. intval, list. conval. intval2);
        IF (S.sym=S.comma) THEN  (* more labels in list *)
          S.GetSym;
          CaseLabels (list. link, range)
        END
      END CaseLabels;

    PROCEDURE BoolExpr (VAR n : E.Node);
      BEGIN
        Expr (n);
        E.ChkType (n, T.strBool)
      END BoolExpr;

    PROCEDURE RemoveBranch (VAR n : E.Node);
    (* pre: n denotes a node of class ndIf, or is NIL. node (declared in 
         Statement) contains the root of the current IF statement.
       post: If the guard of the [ELS]IF branch evaluates to TRUE, all 
         following branches are removed and the branch itself is moved into the
         ELSE part.  If it is a constant FALSE, remove the branch and call 
         RemoveBranch on the next one. *)
      BEGIN
        IF (n # NIL) THEN
          IF (n. left. class =E. ndConst) THEN
            IF (n. left. conval. intval = 0) THEN  (* constant FALSE *)
              n := n. link;
              RemoveBranch (n)
            ELSE  (* constant TRUE *)
              n. link := NIL;            (* remove all following branches *)
              node. right := n. right;   (* install branch as ELSE part *)
              n := NIL                   (* and finally remove it from the [ELS]IF list *)
            END
          ELSE
            RemoveBranch (n. link)
          END
        END
      END RemoveBranch;

    PROCEDURE UnreachableWith (VAR n : E.Node);
    (* pre: n is a node of class ndWithGuard or NIL, node (declared in 
         Statement) contains the root of the current WITH statement.
       post: If the guard of node n never evaluates to TRUE, remove the node. 
         This is repeated on all following type guards of the statement. *)
      VAR
        ptr : E.Node;
      BEGIN
        IF (n # NIL) THEN
          UnreachableWith (n. link);
          ptr := node. left;
          WHILE (ptr # n) DO
            IF (ptr. left. obj = n. left. obj) & (* ptr and n test the same variable *)
               T.ExtOf (n. obj. type, ptr. obj. type) THEN (* and n is an extension of the type *)
              (* since the guard ptr evaluates to TRUE every time n would evaluate to TRUE, 
                 this branch will never be executed: remove it *)
              S.Warn (n. left. pos, 299);
              n := n. link;
              RETURN
            END;
            ptr := ptr. link
          END
        END
      END UnreachableWith;


    BEGIN
      IF (S.sym < S.ident) THEN
        E.Err (149);  (* statement expected *)
        REPEAT
          S.GetSym
        UNTIL (S.sym >= S.ident)
      END;
      IF (S.sym=S.ident) THEN  (* assignment or procedure call *)
        Designator (n);
        IF (S.sym = S.eql) OR (S.sym = S.colon) OR (S.sym = S.becomes) THEN
          node := E.NewNode (E.ndAssign);
          node. left := n;
          CheckSym (S.becomes);
          E.ChkVar (n);
          Expr (node. right);
          comp := E.AssignComp (node. left. type, node. right);  (* err msg/type conv if necessary *)
        ELSE  (* procedure call *)
          ProcCall (node, n);
          IF (node. type. form # T.strNone) &
             (node. type. form # T.strUndef) THEN
            S.Err (n. pos, 264)  (* no proper procedure *)
          END
        END
      ELSIF (S.sym=S.if) THEN  (* if statement *)
        node := E.NewNode (E.ndIfElse);
        node. left := E.NewNode (E.ndIf);
        S.GetSym;
        BoolExpr (node. left. left);
        CheckSym (S.then);
        StatementSeq (node. left. right, proc, loop);
        n := node. left;  (* n points to the end of list of ELSIF parts *)
        WHILE (S.sym=S.elsif) DO
          n. link := E.NewNode (E.ndIf);
          n := n. link;
          S.GetSym;
          BoolExpr (n. left);
          CheckSym (S.then);
          StatementSeq (n. right, proc, loop)
        END;
        IF (S.sym=S.else) THEN
          S.GetSym;
          StatementSeq (node. right, proc, loop)
        END;
        CheckSym (S.end);
        (* try to optimize if statement by removing unreachable code *)
        RemoveBranch (node. left);  (* try to remove unreachable branches *)
        IF (node. left = NIL) THEN  (* all branches unreachable, use ELSE part *)
          node := node. right
        END
      ELSIF (S.sym=S.case) THEN  (* case statement *)
        node := E.NewNode (E.ndCase);
        S.GetSym;
        Expr (node. left);
        node. right := E.NewNode (E.ndCaseElse);
        n := node. right;
        n. conval := T.NewConst();  (* initialize range marker *)
        n. conval. intval := MAX (LONGINT);
        n. conval. intval2 := MIN (LONGINT);
        E.ChkType (node. left, E.grpIntOrChar);
        CheckSym (S.of);
        LOOP
          IF (S.sym<S.bar) THEN
            n. left := E.NewNode (E.ndCaseDo);
            n := n. left;
            CaseLabels (n. link, node. right. conval);
            CheckSym (S.colon);
            StatementSeq (n. right, proc, loop)
          END;
          IF (S.sym # S.bar) THEN
            EXIT
          END;
          S.GetSym
        END;
        IF (S.sym=S.else) THEN
          S.GetSym;
          node. right. conval. set := {1};  (* set flag for 'else branch exists' *)
          StatementSeq (node. right. right, proc, loop)
        END;
        CheckSym (S.end)
      ELSIF (S.sym=S.while) THEN  (* while statement *)
        node := E.NewNode (E.ndWhile);
        S.GetSym;
        BoolExpr (node. left);
        CheckSym (S.do);
        StatementSeq (node. right, proc, loop);
        CheckSym (S.end)
      ELSIF (S.sym=S.repeat) THEN  (* repeat statement *)
        node := E.NewNode (E.ndRepeat);
        S.GetSym;
        StatementSeq (node. left, proc, loop);
        CheckSym (S.until);
        BoolExpr (node. right);
      ELSIF (S.sym=S.for) THEN  (* for statement *)
        (* initialize nodes *)
        node := E.NewNode (E.ndFor);
        node. left := E.NewNode (E.ndForRange);
        S.GetSym;
        (* read control variable *)
        Designator (n);
        node. left. link := n;
        E.ChkVar (n);
        E.ChkType (n, E.grpInteger);
        IF (n. class # E.ndVar) & (n. class # E.ndVarPar) OR (n. obj. mnolev < 0) THEN
          S.Err (n. pos, 231)  (* has to be local variable *)
        END;
        CheckSym (S.becomes);
        (* read low expression *)
        n := node. left;
        Expr (n. left);
        comp := E.AssignComp (node. left. link. type, n. left);  (* err msg/type conv if necessary *)
        CheckSym (S.to);
        (* read high expression *)
        Expr (n. right);
        comp := E.AssignComp (node. left. link. type, n. right);  (* err msg/type conv if necessary *)
        (* read optional step *)
        IF (S.sym=S.by) THEN  (* step value given *)
          S.GetSym;
          pos := S.lastSym;
          ConstExpr (n. conval, n. type);
          IF ~(n. type. form IN E.intSet) OR (n. conval. intval = 0) THEN
            S.Err (pos, 234)  (* step has to be a not zero integer constant *)
          END
        ELSE  (* install the default value of '1' *)
          n. conval := T.NewConst();
          n. conval. intval := 1;
          n. type := T.predeclType[T.strLongInt];
          E.SetInt (n)
        END;
        (* finally: read the loop body *)
        CheckSym (S.do);
        StatementSeq (node. right, proc, loop);
        CheckSym (S.end)
      ELSIF (S.sym=S.loop) THEN  (* loop statement *)
        node := E.NewNode (E.ndLoop);
        S.GetSym;
        StatementSeq (node. left, proc, node);
        CheckSym (S.end)
      ELSIF (S.sym=S.with) THEN  (* with statement *)
        node := E.NewNode (E.ndWithElse);
        node. conval := T.NewConst();
        n := node;
        node. left := E.NewNode (E.ndWithGuard);
        n := node. left;
        LOOP
          S.GetSym;
          Designator (n. left);
          CheckSym (S.colon);
          Qualident (n. obj, pos);
          comp := E.ChkGuard (n. left, n. obj, TRUE, pos);
          CheckSym (S.do);
          IF comp THEN
            prevType :=  n. left. obj. type;
            n. left. obj. type := n. obj. type;
            StatementSeq (n. right, proc, loop);
            n. left. obj. type := prevType
          ELSE
            StatementSeq (n. right, proc, loop)
          END;
          IF (S.sym#S.bar) THEN
            EXIT
          END;
          n. link := E.NewNode (E.ndWithGuard);
          n := n. link
        END;
        IF (S.sym=S.else) THEN
          S.GetSym;
          node. conval. set := {1};
          StatementSeq (node. right, proc, loop)
        END;
        CheckSym (S.end);
        UnreachableWith (node. left)  (* find unreachable branches, remove them *)
      ELSIF (S.sym=S.exit) THEN  (* exit statement *)
        IF (loop = NIL) THEN
          E.Err (239)  (* no enclosing loop *)
        END;
        node := E.NewNode (E.ndExit);
        node. left := loop;
        S.GetSym
      ELSIF (S.sym=S.return) THEN  (* return statement *)
        node := E.NewNode (E.ndReturn);
        node. obj := proc. obj;
        S.GetSym;
        IF (S.sym < S.semicolon) THEN
          IF (proc. obj=NIL) OR (proc. obj. type. base. form=T.strNone) THEN
            E.Err (240)  (* statement is not part of a function *)
          END;
          Expr (node. left)
        END;
        IF (proc. obj#NIL) & (proc. obj. type. base. form#T.strNone) THEN  (* proc is a 'real' function *)
          IF (node. left # NIL) THEN  (* check result with function type *)
            comp := E.AssignComp (proc. obj. type. base, node. left)
          ELSE
            E.Err (241)  (* function result expected *)
          END
        END
      END
    END Statement;

  BEGIN
    last := NIL;
    LOOP  (* loop over sequence of statement, skip empty statements on the way *)
      IF (last # NIL) THEN
        Statement (last. link)
      ELSE
        Statement (r);
        last := r
      END;
      (* correct last to point to the end of the current statm sequence *)
      IF (last # NIL) THEN
        WHILE (last. link # NIL) DO
          last := last. link
        END
      END;
      IF (S.sym # S.semicolon) & (S.sym # S.ident) & ((S.if > S.sym) OR (S.sym > S.return)) THEN
        EXIT
      END;
      CheckSym (S.semicolon);
    END
  END StatementSeq;


PROCEDURE IdentDef (VAR obj : T.Object; mode : SHORTINT; exp : SHORTINT);
(* Reads a single identifier, possibly marked for export with '-' or '*'.  The
   name (resp the export mark) may be followed by an external name definition 
   if the current module is EXTERNAL and mode=objVar or objLocalProc.
   pre: mode contains the new object value for obj.mode, exp determines, 
     whether an export mark is allowed or not: exp<=exportNot means no export, 
     exp=exportRead allows only export with '*', exp>=exportWrite allows 
     export with '-' and '*'. The current token has to be an identifier.     
   post: A new object of the class 'mode' is created, its export mark set 
     accordingly. *)
  BEGIN
    obj := NewObject (S.ref, mode);
    S.GetSym;
    IF (S.sym=S.times) OR (S.sym=S.minus) THEN
      IF (exp <= T.exportNot) THEN
        E.Err (201)  (* export only on level 0 possible *)
      ELSIF (exp = T.exportRead) & (S.sym = S.minus) THEN
        E.Err (202)  (* only export with '*' possible *)
      END;
      IF (S.sym = S.minus) THEN
        obj. mark := T.exportRead
      ELSE
        obj. mark := T.exportWrite
      END;
      S.GetSym;
      (* check for external name *)
      IF M.allowExternal & T.external & (mode IN {T.objVar, T.objLocalProc}) &
         (S.sym = S.lBrak) THEN
        S.GetSym;
        NEW (obj. extName);
        COPY (S.ref, obj. extName^);
        CheckSym (S.string);
        CheckSym (S.rBrak)
      END
    END
  END IdentDef;

PROCEDURE IdentList (VAR first, last : T.Object; mode : SHORTINT; exp : SHORTINT);
(* Reads a list of identifiers seperated by comma.
   pre: Same conditions as for IdentDef.
   post: first points to the head of the object list, last to its last 
     element. *)
  BEGIN
    IdentDef (first, mode, exp);
    last := first;
    LOOP
      IF (S.sym=S.comma) THEN
        S.GetSym
      ELSIF (S.sym=S.ident) THEN
        E.Err (119)  (* ', ' expected *)
      ELSE
        EXIT
      END;
      IF (S.sym=S.ident) THEN
        IdentDef (last. link, mode, exp);
        last := last. link
      ELSE
        E.Err (100);  (* identifier expected *)
        S.GetSym
      END
    END
  END IdentList;

PROCEDURE Type (VAR t : T.Struct; VAR pos : LONGINT; exp : SHORTINT);
(* Parses a type description and handles the problem of forward declared types.
   pre: exp<=exportNot forbids export of record fields, otherwise it is 
     allowed.
   post: The resulting type structure is returned in t, its file position 
     stored in pos.  If no valid type could be read, an 'undef' type is 
     returned. *)
  VAR
    obj, first, last : T.Object;
    init : E.Node;
    p : LONGINT;

  PROCEDURE ArrayType (VAR t : T.Struct);
  (* Read lengths of the array's dimensions.
     pre: Current token is the start of a constant expression.
     post: The complete array type is returned in t. *)
    VAR
      const : T.Const;
      t0 : T.Struct;
      pos : LONGINT;
    BEGIN
      (* read array length *)
      pos := S.lastSym;
      t := NewStruct (T.strArray);
      ConstExpr (const, t0);
      IF ~(t0. form IN E.intSet) THEN
        S.Err (pos, 207)  (* has to be an integer constant *)
      ELSIF (const. intval <= 0) THEN
        S.Err (pos, 206)  (* illegal array length *)
      END;
      t. len := const. intval;
      IF (S.sym=S.comma) THEN  (* more dimensions follow *)
        S.GetSym;
        ArrayType (t. base)
      ELSE  (* read the array's base type *)
        CheckSym (S.of);
        Type (t. base, pos, exp);
        IF (t. base. form=T.strDynArray) THEN
          S.Err (pos, 217)  (* no open array allowed here *)
        END
      END;
      t. size := T.structSize (t)
    END ArrayType;

  BEGIN
    pos := S.lastSym;
    IF (S.sym=S.ident) THEN  (* type identifier *)
      Qualident (obj, pos);
      IF (obj. mode # T.objType) THEN
        S.Err (pos, 205);  (* no data type *)
        t := T.predeclType[T.strUndef]
      ELSE
        t := obj. type
      END
    ELSIF (S.sym=S.array) THEN  (* (open) array type *)
      S.GetSym;
      IF (S.sym=S.of) THEN  (* open array *)
        S.GetSym;
        t := NewStruct (T.strDynArray);
        Type (t. base, p, exp);
        IF T.external & (t. base. form = T.strDynArray) THEN
          S.Err (p, 217)  (* no open array allowed here *)
        END;
        t. size := T.structSize(t)
      ELSE  (* 'normal' array of fixed length *)
        ArrayType (t)
      END
    ELSIF (S.sym=S.record) THEN  (* record (or union) type *)
      t := NewStruct (T.strRecord);
      S.GetSym;
      IF M.allowUnion & T.external & (S.sym=S.lBrak) THEN  (* record modifier *)
        S.GetSym;
        IF (S.sym = S.ident) & (S.ref="UNION") THEN  (* union type: set flag *)
          INCL (t. flags, T.flagUnion)
        ELSE
          S.Err (-1, 103)  (* UNION expected *)
        END;
        S.GetSym;
        CheckSym (S.rBrak)
      ELSIF ~T.external & (S.sym=S.lParen) THEN  (* the record is an extension: read receiver *)
        S.GetSym;
        Qualident (obj, p);
        IF (obj. mode = T.objType) & (obj. type. form = T.strRecord) & 
           ~(T.flagUnion IN obj. type. flags) THEN
          t. base := obj. type;
          t. len := t. base. len+1;
          IF (T.flagExternal IN t. base. flags) THEN
            S.Err (p, 152)  (* this record has no type descriptor *)
          END
        ELSE  (* no type or no record *)
          S.Err (p, 209)
        END;
        CheckSym (S.rParen)
      END;
      LOOP  (* loop over identifier lists *)
        IF (S.sym=S.ident) THEN
          IdentList (first, last, T.objField, exp);  (* get ident list *)
          CheckSym (S.colon);
          Type (last. type, p, exp);  (* and the type belonging to it *)
          IF (last. type. form = T.strDynArray) THEN
            S.Err (p, 217)  (* no open array allowed here *)
          END;
          (* set type for all fields in the ident list and insert them into the symbol table *)
          obj := first;
          WHILE (obj # NIL) DO
            first := obj. link;
            obj. type := last. type;
            obj. link := NIL;
            T.InsertField (obj, t);
            obj := first
          END
        END;
        IF (S.sym#S.semicolon) THEN
          EXIT
        END;
        S.GetSym
      END;
      CheckSym (S.end);
      t. size := T.structSize (t);
      (* create a ndInitTd node for the record type *)
      init := E.NewNode (E.ndInitTd);
      init. type := t;
      init. link := root. link;  (* add node to the list in root. link *)
      root. link := init
    ELSIF (S.sym=S.pointer) THEN  (* pointer type (possibly with a forward declaration) *)
      t := NewStruct (T.strPointer);
      S.GetSym;
      CheckSym (S.to);
      IF (S.sym = S.ident) THEN  (* first check, if this is a forward declaration *)
        obj := T.Find (S.ref);
        IF (obj = NIL) THEN  (* no object named S.ref is declared *)
          (* initialize a new 'forward type' object and insert it into 
             the symbol table *)
          obj := NewObject (S.ref, T.objForwardType);
          obj. type := NewStruct (T.strUndef);
          obj. type. obj := obj;
          INCL (obj. flags, T.flagUsed);
          T.Insert (obj)
        END;
        IF (obj. mode = T.objForwardType) THEN  (* do we have a forward type by now? *)
          t. base := obj. type;
          S.GetSym
        END
      END;
      IF (t. base = NIL) THEN  (* no forward, but rather a 'normal' pointer type declaration *)
        Type (t. base, p, exp);  (* read the pointer base type *)
        IF ~(t. base. form IN {T.strArray, T.strDynArray, T.strRecord, T.strUndef}) THEN
          S.Err (p, 208)  (* illegal pointer base type *)
        END
      END;
      t. size := T.structSize (t)
    ELSIF (S.sym=S.procedure) THEN  (* procedure type *)
      S.GetSym;
      FormalPars (t)
    ELSE
      E.Err (148);  (* data type expected *)
      t := T.predeclType[T.strUndef];
      S.GetSym
    END;
    IF ((S.sym < S.semicolon) OR (S.sym > S.else)) & (S.sym # S.rParen) THEN
      E.Err (146);  (* unexpected symbol (has to be out of follow(Type)) *)
      WHILE ((S.sym < S.ident) OR (S.sym > S.else)) & (* skip token *)
            (S.sym < S.begin) & (S.sym # S.rParen) DO
        S.GetSym
      END
    END
  END Type;

PROCEDURE DeclSeq (exp : SHORTINT; VAR declProcs : E.Node);
(* Parse sequence of declarations.
   pre: exp=T.exportWrite allows export of identifiers, exportNot disables it.
   post: The parsed declarations are inserted into the current scope. The head 
     of the list of defined procedures (including the type bound ones) is
     returned in declProcs. *)
  VAR
    obj, first, last, org : T.Object;
    p : LONGINT;
    n : E.Node;

  PROCEDURE ProcDecl (VAR proc : E.Node);
    VAR
      mode, rmode : SHORTINT;
      rec : T.Object;
      record : T.Struct;
    BEGIN
      IF (S.sym=S.procedure) THEN
        S.GetSym;
        proc := E.NewNode (E.ndEnter);
        mode := T.objLocalProc;
        IF ~T.external THEN
          IF (S.sym = S.arrow) THEN
            mode := T.objForwardProc;
            S.GetSym
          END;
          IF (S.sym=S.lParen) THEN  (* type bound procedure: parse receiver *)
            IF (mode = T.objForwardProc) THEN
              mode := T.objForwardTBProc
            ELSE
              mode := T.objTBProc
            END;
            S.GetSym;
            IF (S.sym=S.var) THEN
              rmode := T.objVarPar;
              S.GetSym
            ELSE
              rmode := T.objVar
            END;
            IF (S.sym=S.ident) THEN
              rec := NewObject (S.ref, rmode);
              S.GetSym
            ELSE
              rec := NewObject (S.undefStr, rmode);
              E.Err (100)  (* identifier expected *)
            END;
            CheckSym (S.colon);
            Type (rec. type, p, T.exportNot);
            record := rec. type;
            IF (record. form = T.strPointer) THEN
              record := record. base;
              IF (rmode # T.objVar) THEN
                S.Err (p, 225)  (* pointer receiver has to be value parameter *)
              END
            ELSIF (record. form=T.strRecord) & (rmode # T.objVarPar) THEN
              S.Err (p, 226)  (* record receiver has to be value *)
            END;
            IF (record. form # T.strRecord) OR (record. obj = NIL) THEN
              S.Err (p, 223);  (* illegal receiver type *)
              record := T.predeclType[T.strUndef]
            ELSIF (record. obj. mnolev # 0) THEN
              S.Err (p, 224);  (* has to be level 0 *)
              record := T.predeclType[T.strUndef]
            END;
            CheckSym (S.rParen);
            IF (T.LevelOfTopScope() # T.compileMnolev) THEN
              S.Err (-1, 222)  (* type bound procedure has to be defined on top level *)
            END
          END
        END;
        IF (S.sym = S.ident) THEN
          IdentDef (proc. obj, mode, exp-1)
        ELSE
          proc. obj := NewObject (S.undefStr, mode);
          S.Err (-1, 100);
          S.GetSym
        END;
        IF (mode = T.objLocalProc) & (proc. obj. mark # T.exportNot) THEN
          mode := T.objExtProc;
          proc. obj. mode := T.objExtProc
        END;
        FormalPars (proc. obj. type);
        IF (mode=T.objTBProc) OR (mode=T.objForwardTBProc) THEN
          (* install receiver as part of the procedures parameter list *)
          rec. link := proc. obj. type. link;
          proc. obj. type. link := rec;
          INC (proc. obj. type. len);
          proc. obj. type. form := T.strTBProc;
          (* add procedure to the record's field list *)
          IF (record. form=T.strRecord) THEN
            T.InsertField (proc. obj, record)
          END
        ELSE  (* insert normal procedure *)
          T.Insert (proc. obj)
        END;
        IF (mode = T.objForwardProc) OR (mode = T.objForwardTBProc) THEN
          proc. class := E.ndForward
        ELSIF ~T.external THEN          (* no local decls in external modules *)
          CheckSym (S.semicolon);
          T.InsertParams (proc. obj); (* put parameters into the procedure's symbol table *)
          T.OpenScope (proc. obj. link);
          undeclIdents := "";            (* erase list of undeclared idents *)
          DeclSeq (T.exportNot, proc. left);
          IF (S.sym = S.begin) THEN
            S.GetSym;
            StatementSeq (proc. right, proc, NIL)
          END;
          CheckSym (S.end);
          IF (S.sym # S.ident) OR (S.ref # proc. obj. name) THEN
            S.ErrIns (-1, 200, proc. obj. name)  (* procedure identifier expected *)
          END;
          S.GetSym;
          (* set flag iff procedure body is empty *)
          IF (proc. right = NIL) THEN
            INCL (proc. obj. link. flags, T.flagEmptyBody)
          END;
          T.CloseScope (TRUE)
        END;
        CheckSym (S.semicolon);
        ProcDecl (proc. link)  (* read next procedure in the procedure list *)
      END
    END ProcDecl;

  BEGIN
    LOOP
      IF (S.sym=S.const) THEN  (* constant declaration *)
        S.GetSym;
        WHILE (S.sym=S.ident) DO
          IdentDef (obj, T.objConst, exp-1);  (* no 'read only' export *)
          CheckSym (S.eql);
          ConstExpr (obj. const, obj. type);
          T.Insert (obj);
          CheckSym (S.semicolon)
        END
      ELSIF (S.sym=S.type) THEN  (* type declaration *)
        S.GetSym;
        WHILE (S.sym=S.ident) DO
          IdentDef (obj, T.objType, exp);
          CheckSym (S.eql);
          Type (obj. type, p, exp);
          IF (obj. type. obj = NIL) THEN  (* set name if obj.type is unnamed *)
            obj. type. obj := obj
          END;
          org := obj;
          T.Insert (obj);
          IF (org # obj) THEN  (* obj has been eliminated due to solved forward reference *)
            (* some E.ndInitTd nodes may point now to the discarded type *)
            n := root. link;
            WHILE (n # NIL) DO
              IF (n. type = org. type) THEN (* uses discarded type *)
                n. type := obj. type     (* replace it *)
              END;
              n := n. link
            END
          END;
          CheckSym (S.semicolon)
        END
      ELSIF (S.sym=S.var) THEN  (* variable declaration *)
        S.GetSym;
        WHILE (S.sym=S.ident) DO
          IdentList (first, last, T.objVar, exp);  (* read ident list *)
          CheckSym (S.colon);
          Type (last. type, p, exp);  (* and the variable's type *)
          IF (last. type. form=T.strDynArray) THEN
            S.Err (p, 217)  (* no open array allowed here *)
          END;
          (* set type for all variables in ident list and insert the var objects *)
          obj := first;
          WHILE (obj # NIL) DO
            first := obj. link;
            obj. type := last. type;
            T.Insert (obj);
            obj := first
          END;
          CheckSym (S.semicolon)
        END
      ELSE
        EXIT
      END
    END;
    ProcDecl (declProcs)  (* parse list of procedure declarations *)
  END DeclSeq;

PROCEDURE Module* (VAR mod : E.Node);
(* Parses the complete file and stores the syntax tree in mod. *)
  VAR
    init, n : E.Node;
    sourceName : ARRAY M.maxSizeIdent+1 OF CHAR;

  PROCEDURE ImportList;
  (* Parses list of module imports, inserts them into the symbol table and 
     imports their symbole files (with OSym.Import). *)
    VAR
      pos : LONGINT;
      modId, alias : ARRAY M.maxSizeIdent OF CHAR;
    BEGIN  (* pre: S.sym=import *)
      S.GetSym;
      LOOP
        IF (S.sym = S.ident) THEN
          COPY (S.ref, modId);
          COPY (S.ref, alias);
          pos := S.lastSym;
          S.GetSym;
          IF (S.sym=S.becomes) THEN
            S.GetSym;
            IF (S.sym=S.ident) THEN
              COPY (S.ref, modId);
              pos := S.lastSym;
              S.GetSym
            ELSE
              E.Err (100)
            END
          END;
          OSym.Import (modId, alias, pos)
        ELSE
          E.Err (100)
        END;
        IF (S.sym # S.comma) THEN
          EXIT
        END;
        S.GetSym
      END;
      CheckSym (S.semicolon)
    END ImportList;

  BEGIN
    mod := NIL; root := NIL; T.external := FALSE;
    S.GetSym;
    IF (S.sym = S.module) THEN
      S.GetSym;
      IF (S.sym = S.ident) THEN
        (* compare module name with the source file's name *)
        Filenames.GetFile (S.sourceName, sourceName);
        IF (sourceName # S.ref) THEN
          S.WarnIns (-1, 298, sourceName)
        END;
        (* create root of symbol table *)
        T.compiledModule := NewObject (S.ref, T.objModule);
        T.OpenScope (T.compiledModule. link);
        S.GetSym;
        IF M.allowExternal & (S.sym = S.lBrak) THEN (* file is an EXTERNAL module *)
          T.external := TRUE;
          (* allow '_' for identifiers in EXTERNAL (but not for the module name) *)          
          S.underscore := M.allowUnderscore;
          INCL (T.compiledModule. flags, T.flagExternal);
          (* parse external header: [ convention ] EXTERNAL [ externalLibraryName] *)
          (* read convention string *)
          S.GetSym;          
          NEW (T.compiledModule. const. string);
          COPY (S.ref, T.compiledModule. const. string^);
          CheckSym (S.string);
          CheckSym (S.rBrak);
          (* check for pseudo keyword EXTERNAL *)
          IF (S.sym # S.ident) OR (S.ref # "EXTERNAL") THEN
            S.Err (-1, 102)  (* EXTERNAL expected *)
          END;
          CheckSym (S.ident);
          (* read external library name *)
          CheckSym (S.lBrak);
          NEW (T.compiledModule. extName);
          COPY (S.ref, T.compiledModule. extName^);
          CheckSym (S.string);
          CheckSym (S.rBrak)
        END;
        (* parse import list *)
        CheckSym (S.semicolon);
        IF (S.sym=S.import) THEN
          ImportList
        END;
        (* initialize root of syntax tree *)
        mod := E.NewNode (E.ndEnter);
        root := mod;
        IF S.noerr THEN  (* useless to proceed here if we already encountered an error *)
          undeclIdents := "";            (* erase list of undeclared idents *)
          (* parse declarations *)
          DeclSeq (T.exportWrite, mod. left);
    	  (* reverse list of type descriptors *)
          init := NIL;
    	  WHILE (mod. link # NIL) DO
    	    n := mod. link;
    	    mod. link := mod. link. link;
    	    n. link := init;
    	    init := n
    	  END;
    	  mod. link := init;
    	  (* parse the module's initialization code (if it exists) *)
          IF ~T.external & (S.sym = S.begin) THEN
            S.GetSym;
            StatementSeq (mod. right, mod, NIL)
          END;
          CheckSym (S.end);
          IF (S.sym # S.ident) OR (S. ref # T.compiledModule. name) THEN
            S.ErrIns (-1, 200, T.compiledModule. name)  (* module identifier expected *)
          END;
          S.GetSym;
          CheckSym (S.period)
        END;
        T.CloseScope (TRUE);
        root := NIL
      ELSE
        E.Err (100)  (* identifier expected *)
      END
    ELSE
      E.Err (101)  (* MODULE expected *)
    END
  END Module;

END OParse.
