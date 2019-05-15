MODULE OTable;
(*----------------------------------------------------------------------------
  Project : Oberon-2 Compiler Front End, Oberon Implementation
  Modul   : SymbolTable.
  Datum   : $Date: 1996/10/14 20:18:37 $
  Autor   : $Author: oberon1 $
  Locking : $Locker:  $
  Revision: $Revision: 3.14 $
  Contents: Bookkeeping of Structures & Idents.
----------------------------------------------------------------------------*)

IMPORT
  SYSTEM,
  OM:=OMachine, OS:=OScan, 
  Out, Rts;

CONST
(* *)
  objCollected = MAX(SET);
  objUndef* = 0; objScope* = 1; objConst* = 2; objType* = 3;
  objVar* = 4;  objVarPar* = 5; objField* = 6; objExtProc* = 7;
  objIntProc* = 8; objLocalProc* = 9; objModule* =10; objTBProc* =11;
  objForwardType* =12; objForwardProc* =13; objForwardTBProc* =14;
  objProtoTBProc =15;

  procedureObjects = {objIntProc .. objLocalProc};
  forwardObjects = {objForwardType .. objForwardTBProc};
  scopeObjects = {objModule,objTBProc} + procedureObjects;
  methodObjects = {objTBProc,objForwardTBProc,objProtoTBProc};
  usableObjects = scopeObjects + {objConst .. objTBProc};

CONST
  exportNot* = 0; exportWrite* = exportNot + 2; exportRead* = exportWrite - 1;

CONST
(*  strCollected = MAX(SET); *)
  strUndef* = 0; strBool* = 1; strChar* = 2; strShortInt* = 3;
  strInteger* = 4; strLongInt* = 5; strReal* = 6; strLongReal* = 7;
  strSet* = 8; strNone* = 9; strString* =10; strNil* =11;

  strPredeclMax* = strNil;               (* the first free number for *)
                                         (* types in the symbolfil *)
  strSysByte* =12; strSysPtr* =13;
  strFixedMax* = strSysPtr;              (* all other structures don't *)
                                         (* have a fixed number associated. *)
  strPointer* = 14; strProc* = 15; strTBProc* = 16;
  strArray* =17; strDynArray* =18; strRecord* =19;


CONST
  flagUsed* = 0; flagForward* = 1; flagHasLocalProc*= 2; flagExport* = 5; 
  flagParam* = 6; flagReceiver* = 7; flagEmptyBody* = 8; flagUnion* = 9; 
  flagExternal* = 10; 
  flagAddressed* = 11; 
  (*  This flag is set for an object in Object.flags, if it is passed as a
      variable parameter to a normal (ie, not predefined) procedure or is
      used as argument of SYSTEM.ADR.
    Some explanations:
    - normally the above mentioned object will be a variable or a
      variable parameter; it can also be a record field or a procedure 
    - the flag is set if the object's (variable's) address is used just
      once in such a way in its complete visibility scope
    - it's not set if the variable is used as (first) parameter in INCL,
      EXCL, COPY(s,v), INC, DEC, GET, PUT, GETREG, PUTREG, SYSTEM.NEW,
      NEW, or if it is the left hand side of an assignment
    - the flag is not necessarily set for variables of structured types *)
    
  flagMinFree* = 16;                     


(* Constants for predefined- and SYSTEM-procedures *)
CONST
(* conversions *)
  predCHR* =13; predENTIER*=14; predLONG* =15;
  predORD* =16; predSHORT* =17;
(* some constants as defined in OMachine.Mod *)
  predMAX* =18; predMIN* =19; 
  predSIZE* =20; (* special case *)
(*  monadic operators *)
  predABS* =21; predCAP* =22; predODD* =23; sysADR* =24; sysCC* =25;
(* binary operators *)
  predASH* =26; sysBIT* =27; sysLSH* =28; sysROT* =29;
  predLEN* =30;  (* special case *)
  sysVAL* =31;  (* special case *)
(* assign variations *)
  predINCL* =32; predEXCL* =33; predINC* =34; predDEC* =35; predCOPY* =36;
  sysMOVE* =37;  (* special case *)
  sysGET* =38; sysPUT* =39; sysGETREG* =40; sysPUTREG* =41; sysNEW* =42;
  predNEW* =43;  (* special case, 44 must be left free *)
(* trap *)
  predHALT* =45; predASSERT*=46;
(* dispose memory *)
  sysDISPOSE* = 47;
(* optional garbage collector *)
  sysCOLLECT* = 48;


CONST
  noPos* = -1; (* no position is available *)
  noIdent* = "@"; (* ident of not exported record fields in the symbol file *)
  genericParam* = "##";

TYPE
  Object* = POINTER TO ObjectDesc;
  Struct* = POINTER TO StructDesc;
  Const* = POINTER TO ConstDesc;
  String = POINTER TO ARRAY OM.maxSizeString OF CHAR;

  ObjectDesc* =
    RECORD
      mode*: SHORTINT;
      scope-,left*,right*,link*,next*,sort-: Object;
      name*: ARRAY OM.maxSizeIdent OF CHAR;
      extName*: String;
      mark*: SHORTINT;
      type*: Struct;
      const*: Const;
      flags*: SET;
      mnolev*,pos-,adr*: LONGINT;
    END; (* RECORD *)

  StructDesc* =
    RECORD
       form* : SHORTINT;
       base* : Struct;
       obj*,link*: Object;
       flags*: SET;
       len*,size*  : LONGINT;
    END; (* RECORD *)

  ConstDesc* =
    RECORD
      string*: String;
      intval*, intval2*: LONGINT;
      set*: SET;
      real*: LONGREAL;
    END; (* RECORD *)

VAR
(* Returns size of type `t'; function is provided by the back end.
   -1 means that the size is target dependend and can't be computed here, 
   -2 stands for not computable size (like for open arrays).  
   Used by the front end to compute the value of Struct.size (and to translate
   function SIZE() into a constant value).  The function can assume that it has
   already been called on t's base type(s) (except for forward declared pointer
   base types).  Note: Compilation errors may create illegal types of 
   Struct.form=strUndef (their size is 1), but `t' is never NIL.  *)
  structSize* : PROCEDURE (t : Struct) : LONGINT;
  
(* --------------------------------------------------------------------------*)
(* type declarations for the organisation of nested scopes *)
(* --------------------------------------------------------------------------*)

TYPE
  ScopeStack = POINTER TO ScopeStackDesc;
  ScopeStackDesc = 
    RECORD
      last: ScopeStack;
      topScope: Object;
    END;

VAR
  scopeStack: ScopeStack;


(* --------------------------------------------------------------------------*)
(* type declaration for the symbol file cache *)
(* --------------------------------------------------------------------------*)

TYPE (* Symboltable-Caching *)
  SymbolTableImported = POINTER TO SymbolTableImportedDesc;
  SymbolTableCache = POINTER TO SymbolTableCacheDesc;

  SymbolTableCacheDesc =
    RECORD
      pred,succ: SymbolTableCache;
      name: ARRAY OM.maxSizeIdent OF CHAR;
      symtable : Object; (* an Object with mode "objModule" *)
      imported : SymbolTableImported;
    END; (* RECORD *)

  SymbolTableImportedDesc =
    RECORD
      next: SymbolTableImported;
      this: SymbolTableCache;
    END; (* RECORD *)

VAR
  symbolCache: SymbolTableCache;
  cacheMnolev: LONGINT; 

(* --------------------------------------------------------------------------*)
(* declarations for build in modules and identifiers *)
(* --------------------------------------------------------------------------*)

CONST
  systemIdent = "SYSTEM";
  systemKey = 1283764;

  compileMnolev* = 0;
  predeclMnolev* = -1;
  systemMnolev* = -2;
  importMnolev* = -3;

VAR
  predefined,
  system,
  compiledModule*: Object;
  predeclType-: ARRAY (strFixedMax+1) OF Struct;
  external*: BOOLEAN;

(* --------------------------------------------------------------------------*)
(* a simple recycling mechanism for Objects *)
(* --------------------------------------------------------------------------*)

VAR
  collectedObjs: Object;

(* --------------------------------------------------------------------------*)
(* Some forward declarations of procedures *)
(* --------------------------------------------------------------------------*)

PROCEDURE ^NewObject*(name: ARRAY OF CHAR; mode: SHORTINT; pos: LONGINT): Object;
PROCEDURE ^FindField*(ident: ARRAY OF CHAR; struct: Struct): Object;
PROCEDURE ^ParamsMatch*(str1,str2: Struct): BOOLEAN;
PROCEDURE ^InsertField*(obj: Object; struct: Struct);


(* --------------------------------------------------------------------------*)
(* some usefull procedures *)
(* --------------------------------------------------------------------------*)

PROCEDURE ResidentModule(module: ARRAY OF CHAR): BOOLEAN;
  BEGIN
    RETURN (module = systemIdent);
  END ResidentModule;

PROCEDURE IsLocalObject(obj: Object): BOOLEAN;
  BEGIN
    RETURN(obj.mnolev >= compileMnolev);
  END IsLocalObject;

(*****************************************************************************)
(* procedures for a simple recycling mechanism on "Object" *)
(*****************************************************************************)

PROCEDURE NEWObject(VAR obj: Object);
  BEGIN
    IF (collectedObjs # NIL) THEN
      obj:=collectedObjs; collectedObjs:=collectedObjs.next;
    ELSE
      NEW(obj);
    END;
  END NEWObject;

PROCEDURE CollectObject(obj: Object);
  BEGIN
    obj.mode:=objCollected;
    obj.next:=collectedObjs; collectedObjs:=obj;
  END CollectObject;

PROCEDURE RecycleMem*(VAR module: Object);
  VAR
    mnolev: LONGINT;

  PROCEDURE ^RecycleObject(obj: Object);

  PROCEDURE MnolevOk(this,wanted: LONGINT): BOOLEAN;
    BEGIN
      RETURN(this=wanted) OR ((this >= wanted) & (wanted >= compileMnolev));
    END MnolevOk;
  
  PROCEDURE RecycleStruct(str: Struct);
    BEGIN
      IF (str # NIL) THEN
        IF (str.form = strRecord) & 
            ((str.obj = NIL) OR MnolevOk(str.obj.mnolev,mnolev)) THEN
          RecycleObject(str.link);
        END;
      END;
    END RecycleStruct;
  
  PROCEDURE RecycleObject(obj: Object);
    BEGIN
      IF (obj # NIL) THEN
        IF (obj.mode IN {objConst .. objTBProc}) THEN
          RecycleObject(obj.left);
          RecycleObject(obj.right);
        END;
        IF (obj.mode IN {objScope,objExtProc .. objLocalProc,objTBProc}) THEN
          RecycleObject(obj.link);
        ELSIF (obj.mode = objType) THEN
          RecycleStruct(obj.type);
        END;
        IF ~(obj.mode IN {objCollected}) & MnolevOk(obj.mnolev,mnolev) THEN
          CollectObject(obj);
        END;
      END;
    END RecycleObject;

  BEGIN
    IF (module # NIL) & ~ResidentModule(module.name) THEN
      (* recycle *)
      mnolev:=module.mnolev;
      RecycleObject(module.link);
      CollectObject(module);
    END;
  END RecycleMem;

(*****************************************************************************)
(* Opening/Closing of scopes *)
(*****************************************************************************)

PROCEDURE ^CheckUnresolvedForward(scope,obj: Object);
PROCEDURE ^WarnUnused(scope: Object);
PROCEDURE ^PutModulesNextAndSort(scope: Object);


PROCEDURE OpenScope*(scope: Object);
  VAR
    new: ScopeStack;
  BEGIN
    NEW(new);
    new.last:=scopeStack;
    new.topScope:=scope;
    scopeStack:=new;
    IF (scope = NIL) THEN
      Out.String("internal compiler error in front end: OpenScope(NIL)"); 
      Out.Ln;
    END; (* IF *)
  END OpenScope;

PROCEDURE CloseScope*(check: BOOLEAN);
  VAR
    dealloc: ScopeStack;
  BEGIN
    IF (scopeStack # NIL) THEN
      IF check THEN (* check for unresolved forwarddeclarations *)
        CheckUnresolvedForward(scopeStack.topScope,scopeStack.topScope.link);
        WarnUnused(scopeStack.topScope);
        PutModulesNextAndSort(scopeStack.topScope);
      END; (* IF *)

      dealloc:=scopeStack;
      scopeStack:=scopeStack.last;
      SYSTEM.DISPOSE(dealloc);
    ELSE
      Rts.Error("internal compiler error in front end: more CloseScope than OpenScope");
    END;
  END CloseScope;

PROCEDURE LevelOfTopScope*(): LONGINT;
  BEGIN
    RETURN (scopeStack.topScope.mnolev);
  END LevelOfTopScope;

PROCEDURE InitScopeStack;
  BEGIN
    scopeStack:=NIL;
  END InitScopeStack;

(* procedures which are called by CloseScope *)

PROCEDURE CheckUnresolvedForward(scope,obj: Object);
(* recursive check-procedure for unresolved forward-declarations.
   At the same time, look for locally declared procedures *)
  BEGIN
    IF (obj # NIL) THEN
      CheckUnresolvedForward(scope,obj.left);
      IF (obj.mode IN forwardObjects) THEN
        OS.Err(obj.pos,65);
      ELSIF (obj.mode = objType) & (obj.type.form = strRecord) THEN
        CheckUnresolvedForward(scope,obj.type.link);
      ELSIF (obj.mode IN procedureObjects) THEN
        (* for effiency in back end: Check for locally declared procedures *)
        INCL(scope.flags,flagHasLocalProc);
      END; 
      CheckUnresolvedForward(scope,obj.right);
    END;
  END CheckUnresolvedForward;

PROCEDURE WarnUnused(scope: Object);
(* This procedure has the problem that it cannot recognise unused types
   of the following pattern:
   "TYPE a = POINTER TO aDesc; aDesc = RECORD b: a END; "
*)
  PROCEDURE IsRedefinition(tbproc: Object): BOOLEAN;
    VAR
      str: Struct;
    BEGIN
      str:=tbproc.type.link.type; (* this is the receiver's type *)
      IF (str # NIL) & (str.form = strPointer) THEN
        (* resolve pointer, get corresponding record type *)
        str:=str.base;
      END; (* IF *)
      RETURN(FindField(tbproc.name,str.base) # NIL); 
      (* if field is visible, then it's a redefinition *)
    END IsRedefinition;
  
  PROCEDURE WarnUnusedRec(obj: Object);
    BEGIN
      IF (obj # NIL) THEN
        WarnUnusedRec(obj.left);
        IF ~(flagUsed IN obj.flags) THEN
          IF (flagParam IN obj.flags) & (flagEmptyBody IN scope.flags) THEN
            (* suppress warning for unused parameter *)
          ELSIF (obj.mode = objTBProc) & (obj.mark = exportNot) 
            & IsRedefinition(obj) THEN (* suppress warning for redefinition *)
          ELSIF (obj.mode IN usableObjects) & (obj.mark = exportNot) THEN
            (* all other unused objects *)
            OS.Warn(obj.pos,90);
          END;
        END;
        
        IF (obj.mode = objType) & (obj.type.form = strRecord) THEN
          WarnUnusedRec(obj.type.link);
        END; (* IF *) 
        WarnUnusedRec(obj.right);
      END;
    END WarnUnusedRec;
    
  BEGIN
    IF IsLocalObject(scope) THEN
      WarnUnusedRec(scope.link);
    END;
  END WarnUnused;

PROCEDURE PutModulesNextAndSort(scope: Object);
(* Collects all module-Object "objModule" in "scope" and puts them
   into a via "obj.next" linear linked list at "scope.right".

   This procedure must only be called when nothing more is done in
   "scope" *)
  VAR
    module,sort,first: Object;

  PROCEDURE RecCollectAndSort(obj: Object);
    (* collect all modules, sort other objects *)
    BEGIN
      IF (obj # NIL) THEN
        RecCollectAndSort(obj.left);
        IF (obj.mode = objModule) THEN
          module.next:=obj; obj.next:=NIL; module:=obj;
        ELSIF ~(flagParam IN obj.flags) THEN
          sort.sort:=obj; obj.sort:=NIL; sort:=obj;
        END; (* IF *)
        RecCollectAndSort(obj.right);
      END; (* IF *)
    END RecCollectAndSort;

  BEGIN
    scope.next:=NIL;
    first:=NewObject(noIdent,objCollected,noPos);
    module:=first; sort:=first;
    RecCollectAndSort(scope.link);
    scope.next:=first.next; scope.sort:=first.sort;
    CollectObject(first);
  END PutModulesNextAndSort;

(*****************************************************************************)
(* Caching of symbol files (better symbol tables of symbol files *)
(*****************************************************************************)

PROCEDURE SearchSymbolTable(module: ARRAY OF CHAR): SymbolTableCache;
  VAR
    walk: SymbolTableCache;
  BEGIN
    walk:=symbolCache;
    WHILE (walk # NIL) & (walk.name < module) DO
      walk:=walk.succ;
    END;
    IF (walk # NIL) & (walk.name = module) THEN
      RETURN(walk);
    ELSE
      RETURN(NIL);
    END;
  END SearchSymbolTable;

PROCEDURE RemoveSymbolTable(module: ARRAY OF CHAR): SymbolTableCache;
(* Removed cache object is returned *)
  VAR
    walk: SymbolTableCache;
  BEGIN
    walk:=SearchSymbolTable(module);
    IF (walk # NIL) THEN
      IF (walk.pred # NIL) THEN
        walk.pred.succ:=walk.succ;
      ELSE
        symbolCache:=walk.succ;
      END;
      IF (walk.succ # NIL) THEN
        walk.succ.pred:=walk.pred;
      END;
      walk.succ:=NIL;
      walk.pred:=NIL;
      RecycleMem(walk.symtable);
      RETURN(walk);
    ELSE
      Out.String("compiler error? Symboltable to remove was not found"); 
      Out.Ln;
      RETURN(NIL);
    END;
  END RemoveSymbolTable;

PROCEDURE InsertSymbolTable*(module: ARRAY OF CHAR; table: Object);
  VAR
    pre,walk,new: SymbolTableCache;
  BEGIN
    walk:=SearchSymbolTable(module);
    IF (walk # NIL) THEN
      Rts.Error("internal compiler error in front end: Module more than once in cache");
    END;

    pre:=NIL;walk:=symbolCache;
    WHILE (walk # NIL) & (walk.name < module) DO
      pre:=walk; walk:=walk.succ;
    END;

    NEW(new);
    COPY(module,new.name);

    new.symtable:=table; new.imported:=NIL;
    new.pred:=pre; new.succ:=walk;

    IF (pre = NIL) THEN
      symbolCache:=new;
    ELSE
      pre.succ:=new;
    END;
    IF (walk # NIL) THEN
      walk.pred:=new;
    END;
  END InsertSymbolTable;

PROCEDURE InsertSymbolTableImport*(module,imports: ARRAY OF CHAR);
(* 'module' imports 'imports'*)
  VAR
    mod,imp: SymbolTableCache;
    new: SymbolTableImported;
  BEGIN
    mod:=SearchSymbolTable(module);
    imp:=SearchSymbolTable(imports);
    IF (mod # NIL) & (imp # NIL) THEN
      NEW(new);
      new.this:=mod; new.next:=imp.imported; imp.imported:=new;
    ELSE
      Out.String("internal compiler error in front end: problems with the cache hierachie"); 
      Out.Ln;
    END;
  END InsertSymbolTableImport;

PROCEDURE GetSymbolTable*(module: ARRAY OF CHAR): Object;
  VAR
    walk: SymbolTableCache;
  BEGIN
    walk:=SearchSymbolTable(module);
    IF (walk # NIL) THEN
      RETURN(walk.symtable);
    ELSE
      RETURN(NIL);
    END;
  END GetSymbolTable;

PROCEDURE FlushTable(flush: SymbolTableCache);
  VAR
    thisflush: SymbolTableImported;
  BEGIN
    IF (flush # NIL) THEN
      WHILE (flush.imported # NIL) DO
        thisflush:=flush.imported;
        flush.imported:=flush.imported.next;
        FlushTable(thisflush.this);
      END;
    END;
  END FlushTable;

PROCEDURE FlushSymbolTable*(module: ARRAY OF CHAR);
  VAR
    flush: SymbolTableCache;
  BEGIN
    flush:=SearchSymbolTable(module);
    FlushTable(flush);
  END FlushSymbolTable;

PROCEDURE InitSymbolCache;
  BEGIN
    symbolCache:=NIL;
    cacheMnolev:=importMnolev;
    InsertSymbolTable(systemIdent,system);
    cacheMnolev:=importMnolev;
  END InitSymbolCache;

PROCEDURE FlushSymbolCache;
  VAR
    flush: SymbolTableCache;
  BEGIN
    Out.String("FlushSymbolCache was called"); Out.Ln;
    WHILE (symbolCache # NIL) DO
      flush:=RemoveSymbolTable(symbolCache.name);
      FlushTable(flush);
    END;
  END FlushSymbolCache;

PROCEDURE GetImportMnolev*(): LONGINT;
  BEGIN
    IF (cacheMnolev > OM.minLInt) THEN
      DEC(cacheMnolev);
    ELSE (* this should never happen, but we will check this carefully *)
      Out.String("Cache-Overflow: flushing all caches, don't panic."); Out.Ln;
      FlushSymbolCache;
      InitSymbolCache;
    END; (* IF *)
    RETURN(cacheMnolev);
  END GetImportMnolev;

(*****************************************************************************)
(* Initialize the exported structures *)
(*****************************************************************************)

PROCEDURE NewConst*(): Const;
  VAR
    const: Const;
  BEGIN
    NEW(const);
    const.intval:=0; const.intval2:=0;
    const.real:=0.0; const.set:={}; const.string:=NIL;
    RETURN(const);
  END NewConst;

PROCEDURE NewStruct*(form: SHORTINT): Struct;
  VAR
    str: Struct;
  BEGIN
    NEW(str);
    str.form:=form; str.flags:={}; str.len:=0;
    str.base:=NIL; str.obj:=NIL; str.link:=NIL;
    str.size:=MIN(LONGINT);
    RETURN(str);
  END NewStruct;

PROCEDURE NewObject*(name: ARRAY OF CHAR; mode: SHORTINT; pos: LONGINT): Object;
  VAR
    new: Object;
  BEGIN
    NEWObject(new);
    new.mode:=mode;
    new.mark:=exportNot;
    COPY(name,new.name);
    new.extName:=NIL;

    new.scope:=NIL; new.left:=NIL; new.right:=NIL;
    new.link:=NIL; new.next:=NIL; new.sort:=NIL;
    new.const:=NIL; new.adr:=0;
    new.pos:=pos; new.type:=predeclType[strUndef];
    new.flags:={}; new.mnolev:=compileMnolev;

    IF (mode IN forwardObjects) THEN
      INCL(new.flags,flagForward);
    END;
    IF (mode IN scopeObjects) THEN
      new.link:=NewObject(OS.undefStr,objScope,pos);
      new.link.left:=new;
    END;

    IF (scopeStack # NIL) & ~(mode IN {objField})  THEN
      new.scope:=scopeStack.topScope;
    ELSE
      new.scope:=NIL;
    END;
    
    IF (mode = objModule) THEN
      new.const:=NewConst();
    END;

    RETURN(new);
  END NewObject;

PROCEDURE CopyStruct(source,dest: Struct);
(* copies relevant fields of a structure *)
  BEGIN
    dest.flags:=source.flags;
    dest.form :=source.form;
    dest.len  :=source.len;
    dest.base :=source.base;
    dest.link :=source.link;
    dest.size :=source.size;
  END CopyStruct;

PROCEDURE CopyObject(source,dest: Object);
(* copies relevant fields of an object *)
  BEGIN
    dest.mark :=source.mark;
    dest.mode :=source.mode;
    dest.pos  :=source.pos;
    dest.link :=source.link;
    CopyStruct(source.type,dest.type);
  END CopyObject;


(*****************************************************************************)
(* PROCEDUREs, which work on binary-trees of "Object" *)
(*****************************************************************************)

PROCEDURE InsertInTree(VAR root: Object; obj : Object);
(* insert the object "obj" in the sorted-binary-tree with the root "root" *)
  BEGIN
    IF (root = NIL) THEN
      root:=obj;
    ELSE
      IF (obj.name < root.name) THEN
        InsertInTree(root.left,obj);
      ELSIF (obj.name > root.name) THEN
        InsertInTree(root.right,obj);
      END;
    END;
  END InsertInTree;

PROCEDURE SearchInTree*(root: Object; ident: ARRAY OF CHAR): Object;
(* Searches for an object with the identifier "ident" in the sorted 
   binary-tree with root "root".
   If such an object is found, its pointer will be returned, otherwise NIL. *)
  BEGIN
    IF (root = NIL) THEN 
      RETURN NIL;
    ELSIF (ident < root.name) THEN 
      RETURN SearchInTree(root.left,ident);
    ELSIF (ident > root.name) THEN 
      RETURN SearchInTree(root.right,ident);
    ELSE 
      RETURN root(* ident = root.name, Object found *)
    END;
  END SearchInTree;

(*****************************************************************************)
(* PROCEDUREs, which are used for binary-tree-optimation! (these can be 
   replaced by dummy-procedures for smaller compilers *)
(*****************************************************************************)

PROCEDURE OptimizeTree*(VAR root: Object);
(* Optimize the binary-tree with root "root" to minimal depth.
   Only internal pointers will be modified so external
   references will stay valid for this operation. *)

  PROCEDURE LinearizeTree(root: Object): Object;
  (* rebuilds the binary-tree "root" to a linear-list, object
     are linked via "Object.right". Does not destroy order of nodes,
     nor moves the place of nodes in memory! *)
    VAR
      newroot,tmproot: Object;
    BEGIN
      newroot:=root;
      IF (root # NIL) THEN (* work only if there are nodes in the tree! *)
        root.right:=LinearizeTree(root.right);(* build lin.list on the right *)
        root.left :=LinearizeTree(root.left); (* build lin.list on the left *)
        IF (root.left # NIL) THEN (* are there some nodes on the left? *)
          newroot:=root.left; (* this will become our new root *)
          root.left:=NIL;     (* set the left-tree to NIL *)
          tmproot:=newroot;    (* and put the right side with the old "root"
                                  on the rightmost node of the new root *)
          WHILE (tmproot.right # NIL) DO
            tmproot:=tmproot.right;
          END;
          tmproot.right:=root;
        END;
      END;
      RETURN(newroot);
    END LinearizeTree;

  PROCEDURE RebuildTree(root: Object): Object;
  (* Builds a binary-tree with minimal depth out of a linear-list, which
     must be linked via "Object.right".
     Call this procedure directly after "LinearizeTree" *)
     VAR
       newroot,left,right: Object;

     PROCEDURE SplitList(list: Object; VAR left,root,right: Object);
     (* splits a via Object.right linked list into the following parts:
        left : left part, LEN(left) = LEN(list) DIV 2;
        root : middle node of the list (root of binary tree)
        right: left part, LEN(right) = LEN(list) DIV 2 [-1]
     *)
       VAR
         walk: Object;
       BEGIN
         left :=list; root :=list; right:=list; walk :=list;
         WHILE (walk # NIL) DO
           walk:=walk.right;
           IF (walk # NIL) THEN
             walk:=walk.right; root:=right; right:=right.right;
           END;
         END;
         (* walk is one behind the end of the list,
            right stands at the middle node, root one node before right.
            In order to remove the middle node from the left-side-list,
            we have to set root.right to NIL, then identify root with
            the middle node (which is stored in right now) and move
            right to the next position *)
         IF (right # NIL) THEN
           root.right:=NIL; root:=right;
           right:=right.right; root.right:=NIL;
         END;
       END SplitList;

    BEGIN
      newroot:=NIL;
      IF (root # NIL) & (root.right # NIL) THEN
        SplitList(root,left,newroot,right);
        IF (newroot # NIL) THEN
          newroot.left :=RebuildTree(left);
          newroot.right:=RebuildTree(right);
        ELSE (* there was just one node left *)
          newroot:=root;
        END;
      ELSE
        newroot:=root;
      END;
      RETURN(newroot);
    END RebuildTree;

  BEGIN
    IF (root # NIL) THEN
      root:=LinearizeTree(root);
      root:=RebuildTree(root);
    END; (* IF *)
  END OptimizeTree;

(*****************************************************************************)
(* Check-Functions of some Oberon-2-definitions and other Functions. *)
(*****************************************************************************)

PROCEDURE SameType*(a,b: Struct): BOOLEAN;
(* see Oberon-2-Report, Appendix A *)
  BEGIN
    RETURN((a = b) & (a.form # strDynArray));
  END SameType;

PROCEDURE EqualType*(a,b: Struct): BOOLEAN;
(* see Oberon-2-Report, Appendix A *)
  BEGIN
    RETURN (SameType(a,b) OR
      ((a.form = strDynArray) & (b.form = strDynArray) 
        & EqualType(a.base,b.base)) OR
      ((a.form = strProc) & (b.form = strProc) & ParamsMatch(a,b)));
 END EqualType;

PROCEDURE ExtOf*(a,b: Struct): BOOLEAN;
(* Is "a" an extension of "b"? *)
  BEGIN
    IF (a.form = strPointer) & (b.form = strPointer) THEN
      a:=a.base; b:=b.base;
    ELSIF (a.form = strPointer) # (b.form = strPointer) THEN 
      (* Pointers cannot be extensions of other types... *)
      RETURN(FALSE);
    END;
    IF (a.form = strRecord) & (b.form = strRecord) THEN
      WHILE (a # NIL) & (a # predeclType[strUndef]) DO 
        (* remember: predeclType[strUndef].base = predeclType[strUndef], 
           so we jump into an infinite loop *)
        IF SameType(a,b) THEN
          RETURN(TRUE);
        ELSE
          a:=a.base;
        END;
      END;
    END;
    RETURN(FALSE);
  END ExtOf;

PROCEDURE ParamsMatch*(str1,str2: Struct): BOOLEAN;
(* checks for the two PROCEDURE-structures "str1" and "str2", if their
   formal parametertypes match *)
  VAR
    par1,par2: Object;
    specType: BOOLEAN;
  BEGIN
    IF SameType(str1.base,str2.base) (* test the result-type *) THEN
      specType := FALSE;  (* set to TRUE iff params contain VAR 
                             record or open array *)
      par1:=str1.link; par2:=str2.link;
      LOOP
        IF (par1 # NIL) & (par2 # NIL) THEN
          specType:=specType OR
            (par1.mode = objVarPar) & (par1.type.form = strRecord) 
              & ~(flagExternal IN par1.type.flags) OR
            (par1.type.form = strDynArray);
          IF (par1.mode = par2.mode) & EqualType(par1.type,par2.type) THEN
            par1:=par1.link; par2:=par2.link;
          ELSE
            RETURN(FALSE); (* parameters differ *)
          END; (* IF *)
        ELSE
          RETURN((par1 = NIL) & (par2 = NIL)) & 
                  (* no more parameters => everything's ok *)
               (~specType OR 
                ((flagExternal IN str1.flags) = (flagExternal IN str2.flags)));
        END;
      END;
    END;
    RETURN(FALSE);
  END ParamsMatch;

PROCEDURE CheckParameter(par1,par2: Object; quiet: BOOLEAN): BOOLEAN;
  BEGIN
    IF (par1.mode # par2.mode) THEN (* objVarParam <-> objParam *)
      IF ~quiet THEN OS.Err(par2.pos,69); END;
      RETURN(FALSE);
    ELSIF ~EqualType(par1.type,par2.type) THEN (* types are not equal *)
      IF ~quiet THEN OS.Err(par2.pos,67); END;
      RETURN(FALSE);
    ELSE
      RETURN(TRUE);
    END;
  END CheckParameter;

PROCEDURE SameResult(func1,func2: Object; quiet: BOOLEAN): BOOLEAN;
  BEGIN
    IF ~SameType(func1.type.base,func2.type.base) THEN 
      (* different result-types *)
      IF ~quiet THEN OS.Err(func2.pos,70); END;
      RETURN(FALSE);
    ELSE
      RETURN(TRUE);
    END;
  END SameResult;

PROCEDURE FormalParMatch(proc1,proc2: Object): BOOLEAN;
  VAR
    res: BOOLEAN;
    par1,par2: Object;

  BEGIN
    res:=SameResult(proc1,proc2,FALSE);
    par1:=proc1.type.link; par2:=proc2.type.link;
    (* check the other parameters *)
    LOOP
      IF (par1 = NIL) & (par2 = NIL) THEN
      (* same number of parameters, exit return to caller with 'res' *)
        EXIT;
      ELSIF (par1 = NIL) # (par2 = NIL) THEN 
        (* Error: different number of parameters *)
        OS.Err(proc2.pos,71);
        RETURN(FALSE);
      END;
      res:=CheckParameter(par1,par2,FALSE) & res;
      par1:=par1.link; par2:=par2.link;
    END;
    RETURN(res);
  END FormalParMatch;

PROCEDURE FormalParMatchTB(proc1,proc2: Object): BOOLEAN;
  VAR
    res: BOOLEAN;
    par1,par2: Object;
    quiet: BOOLEAN;

  PROCEDURE CheckReceiver(par1,par2: Object; quiet: BOOLEAN): BOOLEAN;
    BEGIN
      IF (par1.mode # par2.mode) THEN
        RETURN(FALSE);
      ELSIF (par1.type.form # par2.type.form) THEN
        RETURN(FALSE);
      ELSE
        IF ExtOf(par2.type,par1.type) OR EqualType(par2.type,par1.type) THEN
          RETURN(TRUE);
        ELSE
          IF ~quiet THEN OS.Err(par2.pos,84); END;
          RETURN(FALSE);
        END; (* IF *)
      END; (* IF *)
    END CheckReceiver;

  BEGIN
    quiet:=FALSE; 
    res:=SameResult(proc1,proc2,quiet);

    par1:=proc1.type.link; par2:=proc2.type.link;
    res:=CheckReceiver(par1,par2,quiet) & res;
    par1:=par1.link; par2:=par2.link;

    (* check the other parameters *)
    LOOP
      IF (par1 = NIL) & (par2 = NIL) THEN 
        (* everything is ok, same number of parameters *)
        EXIT;
      ELSIF (par1 = NIL) # (par2 = NIL) THEN 
        (* Error: different number of parameters *)
        IF ~quiet THEN OS.Err(proc2.pos,71); END;
        RETURN(FALSE);
      END;
      res:=CheckParameter(par1,par2,quiet) & res;
      par1:=par1.link; par2:=par2.link;
    END;
    RETURN(res);
  END FormalParMatchTB;


PROCEDURE ContainsPointer*(str: Struct): BOOLEAN;
(* This function inspects the given Struct 'str', if there is 
   a pointer inside. *)

  PROCEDURE RecordContainsPointer(obj: Object): BOOLEAN;
    BEGIN
      IF (obj # NIL) THEN
        RETURN ContainsPointer(obj.type) OR 
               RecordContainsPointer(obj.left) OR 
               RecordContainsPointer(obj.right)
      ELSE
        RETURN FALSE
      END
    END RecordContainsPointer;
    
  BEGIN
    IF (str # NIL) THEN
      CASE str.form OF
      | strSysPtr, strPointer:
        RETURN TRUE
      | strArray,strDynArray:
        RETURN ContainsPointer(str.base)
      | strRecord:
        RETURN ContainsPointer(str.base) OR
               RecordContainsPointer(str.link)
      ELSE
        RETURN FALSE
      END
    ELSE
      RETURN FALSE
    END
  END ContainsPointer;
  
(*****************************************************************************)
(* Insertion and searching in scopes. *)
(*****************************************************************************)

PROCEDURE InsertNextObject(obj: Object);
(* all objects are also in a linear list which reflects their 
   declaration order, we update this list with this procedure *)
  BEGIN
    IF ~(flagParam IN obj.flags) THEN 
      (* do not insert parameters to the next-list *)
      IF (scopeStack.topScope.next # NIL) THEN 
        (* There are already some object in the scope *)
        (* put the new object at the end of the list *)
        scopeStack.topScope.next.next:=obj; 
      ELSIF (scopeStack.topScope.right = NIL) THEN
        (* we are the first object in this scope *)
        scopeStack.topScope.right:=obj; 
      ELSE 
        (* we have to search the end of the next-list 
           (the scope is reopened) *)
        scopeStack.topScope.next:=scopeStack.topScope.next;
        WHILE (scopeStack.topScope.next.next # NIL) DO
          scopeStack.topScope.next:=scopeStack.topScope.next.next;
        END; (* WHILE *)
        scopeStack.topScope.next.next:=obj;
      END; (* IF *)
      scopeStack.topScope.next:=obj; (* bookkeeping for the next object *)
    END; (* IF *)
  END InsertNextObject;

PROCEDURE UpdateForNewInsert(VAR obj: Object);
  BEGIN
    obj.scope :=scopeStack.topScope;
    obj.mnolev:=scopeStack.topScope.mnolev;
    IF ~(obj.mode IN {objForwardProc,objForwardType,objModule}) & IsLocalObject(obj) THEN
      InsertNextObject(obj);
    END; (* IF *)
    IF (obj.mode IN {objIntProc,objExtProc,objLocalProc}) & IsLocalObject(obj) THEN
      (* the scopes of these objects have a higher mnolev! *)
      obj.link.mnolev:=obj.mnolev + 1;
    ELSIF (obj.mode IN {objType}) & (obj.type.obj = NIL) THEN
      obj.type.obj:=obj;
    END; (* IF *)
  END UpdateForNewInsert;

PROCEDURE Insert*(VAR obj: Object);
(* puts the object in the currently opened scope.
   If a forward declaration is resolved, semantic checks will be made.
   If an object with the same name already exists in the current scope, an
   error will be reported. *)
  VAR
    old: Object;
  BEGIN
    old:=SearchInTree(scopeStack.topScope.link,obj.name);
    IF (old # NIL) THEN
      (* object with the same ident already exists... *)
      IF (old.mode = objForwardType) THEN
        IF (obj.mode = objType) THEN
          IF (obj.type.form IN {strRecord,strArray,strDynArray}) THEN
            CopyObject(obj,old);
            obj:=old;
            obj.type.obj:=obj;
            InsertNextObject(obj);
          ELSE 
            (* only records, arrays or dynamic arrays are 
               allowed as forward-declarations *)
            OS.Err(obj.pos,66);
          END;
        ELSE (* a forward-type MUST be resolved by a type! *)
          OS.Err(obj.pos,61)
        END;
      ELSIF (old.mode = objForwardProc) THEN
        IF (obj.mode IN {objExtProc,objLocalProc,objIntProc}) THEN 
          (* check the parameter lists *)
          IF (obj.mark # old.mark) THEN
            OS.Err(obj.pos,68);
          END;
          IF FormalParMatch(old,obj) THEN
            CopyObject(obj,old); (* copy the new data into the old location *)
            obj:=old;
            IF IsLocalObject(obj) THEN 
              (* increase level only for "local"-procedures, not 
                 for procs in imported modules *)
              obj.link.mnolev:=scopeStack.topScope.mnolev + 1;
            ELSE 
              (* imported procs have the number of the 
                 corresponding module in their "mnolev" *)
              obj.link.mnolev:=scopeStack.topScope.mnolev;
            END;
            InsertNextObject(obj);
          ELSE (* formal parameterlists differ *)
            OS.Err(obj.pos,62);
          END;
        ELSIF (obj.mode = objForwardProc) THEN 
          (* it is allowed to declare several 
             forward-references, but they must match! *)
          IF FormalParMatch(old,obj) THEN
            OS.Warn(obj.pos,91);
            OS.Warn(old.pos,91);
            obj:=old;
          ELSE (* formal parameterlists differ *)
            OS.Err(obj.pos,62);
          END;
        ELSE (* resolve forward-procedures only with procedures *)
          OS.Err(obj.pos,72);
        END;
      ELSE (* object defined twice *)
        OS.ErrIns(obj.pos,60,obj.name);
      END;
    ELSE
      UpdateForNewInsert(obj);
      InsertInTree(scopeStack.topScope.link,obj);
    END;
  END Insert;

PROCEDURE InsertForOSym*(VAR obj: Object; VAR last: Object);
(* Used to insert objects into the symbol table that are read from the symbol
   file: We don't need to test for double declarations and the objects come in
   (almost) alphabetic order.  'last' is the most recent object inserted into
   the tree, 'last.sort' the lexicographic following object to 'last'. *)
  VAR
    ptr, lastLeft: Object;
  BEGIN
    UpdateForNewInsert(obj);
    IF (last = NIL) THEN  (* empty tree *)
      scopeStack.topScope.link:=obj; obj.sort:=NIL
    ELSIF (last.name < obj.name) & ((last.sort = NIL) OR (obj.name < last.sort.name)) THEN
      (* obj is in the intervall ]last, last.sort[; since last is a leaf
         of the tree, we can safely add the new object at last.right *)
      last.right:=obj; obj.sort:=last.sort;
    ELSE  (* do normal insertion and calculate new last.sort *)
      InsertInTree(scopeStack.topScope.link,obj);
      (* find first object larger than obj and store in in obj.sort *)
      lastLeft:=NIL; ptr:=scopeStack.topScope.link;
      LOOP
        IF (ptr. name <= obj. name) THEN
          ptr:=ptr.right;
          IF (ptr = NIL) THEN ptr:=lastLeft; EXIT; END;
        ELSE
          IF (ptr.left = NIL) OR (ptr.left = obj) THEN EXIT;
          ELSE lastLeft:=ptr; ptr:=ptr.left;
          END;
        END;
      END;
      obj.sort:=ptr;
    END;
    last:=obj;
  END InsertForOSym;

PROCEDURE InsertParams*(proc: Object);
  VAR
    param: Object;
  BEGIN
(*    IF IsLocalObject(proc) THEN (* set the right mnolev *)
      proc.link.mnolev:=proc.mnolev+1;
    END;
*)
    OpenScope(proc.link);
    param:=proc.type.link;
    IF (proc.type.form = strTBProc) THEN INCL(param.flags,flagReceiver); END;
    WHILE (param # NIL) DO
      INCL(param.flags,flagParam); Insert(param);
      param:=param.link;
    END;
    CloseScope(FALSE);
  END InsertParams;

PROCEDURE Find*(ident: ARRAY OF CHAR): Object;
  VAR
    walk,scope: Object;
  BEGIN
    scope:=scopeStack.topScope;
    WHILE (scope # NIL) DO
      walk:=SearchInTree(scope.link,ident);
      IF (walk # NIL) THEN
        INCL(walk.flags,flagUsed);
        RETURN(walk);
      END; (* IF *)
      scope:=scope.scope;
    END;
    RETURN(NIL);
  END Find;

(*****************************************************************************)
(* Insertion and searching within RECORDs. *)
(*****************************************************************************)

PROCEDURE InternalFindField(ident: ARRAY OF CHAR; struct: Struct): Object;
  VAR
    walk: Object;
  BEGIN
    LOOP
      IF (struct = NIL) THEN (* no more base types *)
        RETURN(NIL);
      ELSE
        walk:=SearchInTree(struct.link,ident);
        IF (walk # NIL) THEN
          IF (walk.mark # exportNot) OR IsLocalObject(walk) THEN (* field visible *)
            RETURN(walk);
          ELSE (* field not visible *)
            RETURN(NIL);
          END;
        ELSE (* look in the base type *)
          struct:=struct.base; 
        END;
      END;
    END;
  END InternalFindField;

PROCEDURE FindField*(ident: ARRAY OF CHAR; struct: Struct): Object;
  VAR
    field: Object;
  BEGIN
    field:=InternalFindField(ident,struct);
    IF (field = NIL) OR (field.mode = objProtoTBProc) THEN
      RETURN(NIL);
    ELSE
      INCL(field.flags,flagUsed);
      RETURN(field);
    END;
  END FindField;

PROCEDURE InsertProtoTBs(dest: Struct; orig: Object);
  VAR
    virt: Object;
  BEGIN
    IF (dest # NIL) THEN
      virt:=SearchInTree(dest.link,orig.name);
      IF (virt = NIL) THEN (* just insert the Proto tb *)
        virt:=NewObject(orig.name,objProtoTBProc,noPos);
        InsertField(virt,dest);
      END; (* IF *)
      InsertProtoTBs(dest.base,orig);
    END;
  END InsertProtoTBs;

PROCEDURE CheckTBExportSemantics(sub,super: Object): BOOLEAN;
(* Checks, if (none-)export is allowed according to some rules *)
(* !!! *)
  VAR
    base: Object; (* Base-Recordobject *)
  BEGIN
    base:=sub.type.link.type.obj; (* get the type of the receiver *)
    IF (super # NIL) THEN (* check for consistency between "sub" & "super" *)
      IF (super.mark = exportWrite) THEN 
        IF (base.mark = exportWrite) THEN (* in this case "sub" must be exported according to the report *)
          IF (sub.mark # exportWrite) THEN
            OS.Err(sub.pos,86);
(*            Out.String("Error: typebound procedure must be exported!"); Out.Ln; *)
          END; (* IF *)
          RETURN(sub.mark = exportWrite);
        END; (* IF *)
      END; (* IF *)
    END; (* IF *)
    RETURN(TRUE);
  END CheckTBExportSemantics;

PROCEDURE InsertField*(obj: Object; struct: Struct);
  VAR
    old,local,super: Object;
    
  PROCEDURE InsertFieldNext;
    VAR
      walk: Object;
    BEGIN
      walk:=struct.link;
      IF (walk # obj) THEN (* if 'obj' is the first field in 'struct', this evaluates to FALSE *)
        IF (walk # NIL) THEN
          WHILE (walk.next # NIL) DO
            walk:=walk.next;
          END;
          walk.next:=obj;
        ELSE
          struct.link:=walk;
        END;
      END;
    END InsertFieldNext;
    
  BEGIN
    IF (obj.mode IN {objTBProc}) THEN
      IF IsLocalObject(obj) THEN
        obj.link.mnolev:=obj.mnolev + 1;
      ELSE
        obj.link.mnolev:=obj.mnolev;
      END;
    END;
    old:=InternalFindField(obj.name,struct);
    IF (old # NIL) THEN
      IF (old.mode IN methodObjects) THEN
        local:=SearchInTree(struct.link,obj.name);
        IF (local # NIL) THEN
          IF (local.mode = objForwardTBProc) THEN
            IF FormalParMatchTB(local,obj) & CheckTBExportSemantics(local,obj) THEN
              CopyObject(obj,local);
            ELSE
              OS.Err(obj.pos,88);
            END;
          ELSIF (local.mode = objProtoTBProc) THEN
            OS.Err(obj.pos,87);
          ELSE
            OS.Err(obj.pos,64);
          END;
        ELSE
          super:=old;
          IF (super.mode = objProtoTBProc) THEN
            InsertInTree(struct.link,obj);
            InsertProtoTBs(struct.base,obj);
          ELSIF FormalParMatchTB(super,obj) THEN
            IF CheckTBExportSemantics(obj,super) THEN
              InsertInTree(struct.link,obj);
            END;
          ELSE
            OS.Err(obj.pos,89);
          END;
        END;
      ELSE
        OS.Err(obj.pos,64);
      END;
    ELSE
      InsertInTree(struct.link,obj);
      IF (obj.mode IN methodObjects) THEN
        InsertProtoTBs(struct.base,obj);
      END;
    END;
    IF (obj.mode = objField) THEN
      InsertFieldNext;
    END;
  END InsertField;

PROCEDURE ImportInsertField*(obj: Object; struct: Struct);
  BEGIN
    IF (obj.name # noIdent) THEN
      InsertInTree(struct.link,obj);
    END;
  END ImportInsertField;

(*****************************************************************************)
(* Search identifiers in imported modules. *)
(*****************************************************************************)

PROCEDURE InternalFindImport*(ident: ARRAY OF CHAR; mod: Object): Object;
(* only for use in OSym, does not check for visibility (because symbolfiles 
   contain more information than only exported types...) *)
  BEGIN
    RETURN(SearchInTree(mod.link.link,ident));
  END InternalFindImport;

PROCEDURE FindImport*(ident: ARRAY OF CHAR; mod: Object): Object;
  VAR
    obj: Object;
  BEGIN
    INCL(mod.flags,flagUsed);
    obj:=InternalFindImport(ident,mod);
    IF (obj # NIL) & (obj.mark = exportNot) THEN 
      RETURN(NIL);
    ELSE
      RETURN(obj);
    END;
  END FindImport;

PROCEDURE GetModuleForObject*(obj : Object): Object;
  VAR
    walk: Object;
  BEGIN
    walk:=obj;
    WHILE (walk # NIL) & (walk.mode # objModule) DO
      IF (walk.mode = objField) THEN
        RETURN(NIL);
      ELSE
        walk:=walk.scope.left;
      END;
    END;

    IF (walk # NIL) THEN
      RETURN(walk.link.left);
    ELSE
      RETURN(compiledModule);
    END; (* IF *)
  END GetModuleForObject;

PROCEDURE GetModuleName*(obj : Object; VAR name: ARRAY OF CHAR);
(* Gets the "real" (not alias) name of the module in which "obj" is declared *)
  VAR
    mod: Object;
  BEGIN
    mod:=GetModuleForObject(obj);
    COPY(mod.name,name);
  END GetModuleName;

(*****************************************************************************)
(* Build the internal structures and scopes during initialisation *)
(*****************************************************************************)

PROCEDURE InsertPredeclStruct(name: ARRAY OF CHAR; str: SHORTINT);
  VAR
    obj: Object;
    type: Struct;
  BEGIN
    obj:=NewObject(name,objType,noPos); type:=NewStruct(str);
    obj.mark:=exportWrite; obj.type:=type; type.obj:=obj;
    Insert(obj); INCL(obj.flags,flagUsed);
    predeclType[str]:=type;
    IF (strBool <= str) & (str <= strSet) OR (str = strSysByte) OR (str = strSysPtr) THEN
      type.size:=structSize(type);
    END;
  END InsertPredeclStruct;

PROCEDURE InsertProc(name: ARRAY OF CHAR; num: LONGINT);
  VAR
    obj: Object;
  BEGIN
    obj:=NewObject(name,objExtProc,noPos); obj.const:=NewConst(); 
    obj.const.intval:=num; obj.mark:=exportWrite; obj.type:=NewStruct(strProc);
    Insert(obj);INCL(obj.flags,flagUsed);
  END InsertProc;

PROCEDURE InitPredefined;
  VAR
    new: Object;
  BEGIN
    predefined:=NewObject(OS.undefStr,objScope,noPos); 
    (* open the lowest scope of all *)
    predefined.mnolev:=predeclMnolev; (* in order to get this level *)
    OpenScope(predefined);

  (* insert predefined types *)
    InsertPredeclStruct("BOOLEAN",strBool);
    InsertPredeclStruct("CHAR",strChar);
    InsertPredeclStruct("INTEGER",strInteger); 
    InsertPredeclStruct("LONGINT",strLongInt);
    InsertPredeclStruct("LONGREAL",strLongReal); 
    InsertPredeclStruct("NIL",strNil);
    InsertPredeclStruct("REAL",strReal); 
    InsertPredeclStruct("SET",strSet);
    InsertPredeclStruct("SHORTINT",strShortInt);
  (* insert predefined procedures/functions *)
    InsertProc("ABS",predABS); InsertProc("ASH",predASH);
    InsertProc("ASSERT",predASSERT); InsertProc("CAP",predCAP);
    InsertProc("CHR",predCHR); InsertProc("COPY",predCOPY);
    InsertProc("DEC",predDEC); InsertProc("ENTIER",predENTIER);
    InsertProc("EXCL",predEXCL); InsertProc("HALT",predHALT);
    InsertProc("INC",predINC); InsertProc("INCL",predINCL);
    InsertProc("LEN",predLEN); InsertProc("LONG",predLONG);
    InsertProc("MAX",predMAX); InsertProc("MIN",predMIN);
    InsertProc("NEW",predNEW); InsertProc("ODD",predODD);
    InsertProc("ORD",predORD); InsertProc("SHORT",predSHORT);
    InsertProc("SIZE",predSIZE);
    
  (* the most common constants: TRUE & FALSE *)
    new:=NewObject("FALSE",objConst,noPos); 
    new.const:=NewConst(); new.const.intval:=0; new.type:=predeclType[strBool];
    Insert(new);
    new:=NewObject("TRUE", objConst,noPos); 
    new.const:=NewConst(); new.const.intval:=1; new.type:=predeclType[strBool];
    Insert(new);
    
 (* some structs to be initialized, but no special object for them... *)
    predeclType[strUndef]:=NewStruct(strUndef);
    predeclType[strUndef].base:=predeclType[strUndef];
    predeclType[strUndef].size:=1;
    predeclType[strString]:=NewStruct(strString); 
    predeclType[strNone]:=NewStruct(strNone);
    
    OptimizeTree(predefined.link); (* optimize, just for better performance *)
  END InitPredefined;

PROCEDURE InitSYSTEM;
(* Initialize the compilermodule "SYSTEM" *)
  VAR
    new: Object;
  BEGIN
    system:=NewObject(systemIdent,objModule,noPos); 
    system.const.intval:=systemKey; system.mnolev:=systemMnolev; 
    system.link.mnolev:=systemMnolev;
    OpenScope(system.link);

    InsertPredeclStruct("BYTE",strSysByte); 
    InsertPredeclStruct("PTR",strSysPtr);

    InsertProc("ADR",sysADR); InsertProc("BIT",sysBIT);
    InsertProc("CC",sysCC); InsertProc("DISPOSE",sysDISPOSE);
    InsertProc("GET",sysGET); InsertProc("GETREG",sysGETREG);
    InsertProc("LSH",sysLSH); InsertProc("MOVE",sysMOVE);
    InsertProc("NEW",sysNEW); InsertProc("PUT",sysPUT);
    InsertProc("PUTREG",sysPUTREG); InsertProc("ROT",sysROT);
    InsertProc("VAL",sysVAL);
    InsertProc("COLLECT",sysCOLLECT);

    new:=NewObject("GC",objConst,noPos); 
    new.const:=NewConst(); new.type:=predeclType[strBool];
    new.mark:=exportWrite;
    IF OM.generateCodeForGC THEN
      new.const.intval := 1
    ELSE
      new.const.intval := 0
    END;
    Insert(new);

    OptimizeTree(system.link.link);
    CloseScope(TRUE);
  END InitSYSTEM;

PROCEDURE InitTable*;
  BEGIN
    compiledModule:=NIL;
    external:=FALSE;
    InitPredefined;
    InitSYSTEM;
    InitSymbolCache;
  END InitTable;

PROCEDURE DummyStructSize(t: Struct): LONGINT;
  BEGIN
    RETURN MIN(LONGINT);
  END DummyStructSize;
  
BEGIN
  collectedObjs:=NIL;
  structSize:=DummyStructSize;
  InitScopeStack;
END OTable.
