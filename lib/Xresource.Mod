MODULE Xresource ["C"] EXTERNAL [""];

IMPORT
  C := CType, X := X11;


(****************************************************************
 ****************************************************************
 ***                                                          ***
 ***                                                          ***
 ***          X Resource Manager Intrinsics                   ***
 ***                                                          ***
 ***                                                          ***
 ****************************************************************
 ****************************************************************)


TYPE
  XrmQuark* = C.int;
  XrmQuarkList* = POINTER TO ARRAY OF XrmQuark;
  XrmString* = C.charPtr1d;

CONST
  NULLQUARK* = 0;
  NULLSTRING* = NIL;
  
CONST  (* enum XrmBinding *)
  XrmBindTightly* = 0;
  XrmBindLoosely* = 1;

TYPE
  XrmBinding* = C.enum1;
  XrmBindingList* = POINTER TO ARRAY OF XrmBinding;

TYPE
  XrmName* = XrmQuark;
  XrmNameList* = XrmQuarkList;
  XrmClass* = XrmQuark;
  XrmClassList* = XrmQuarkList;

TYPE
  XrmRepresentation* = XrmQuark;
  XrmValuePtr* = POINTER TO XrmValue;
  XrmValue* = RECORD
    size*: C.int;
    addr*: X.XPointer;
  END;

TYPE
  XrmHashBucketDesc = RECORD END;  
  XrmHashBucket* = POINTER TO XrmHashBucketDesc;
  XrmHashTable* = POINTER TO ARRAY OF XrmHashBucket;
  XrmSearchList* = POINTER TO ARRAY OF XrmHashTable;
  XrmDatabase* = POINTER TO XrmHashBucketDesc;

CONST
  XrmEnumAllLevels* = 0;
  XrmEnumOneLevel* = 1;

TYPE
  EnumProc* = PROCEDURE (VAR db: XrmDatabase; bindings: XrmBindingList; 
                         quarks: XrmQuarkList; VAR type: XrmRepresentation;
                         VAR value: XrmValue; closure: X.XPointer): X.Bool;

CONST  (* enum XrmOptionKind *)
  XrmoptionNoArg* = 0;
  XrmoptionIsArg* = 1;
  XrmoptionStickyArg* = 2;
  XrmoptionSepArg* = 3;
  XrmoptionResArg* = 4;
  XrmoptionSkipArg* = 5;
  XrmoptionSkipLine* = 6;
  XrmoptionSkipNArgs* = 7;

TYPE
  XrmOptionKind* = C.enum1;
  XrmOptionDescRecPtr* = POINTER TO XrmOptionDescRec;
  XrmOptionDescRec* = RECORD
    option*: C.charPtr1d;       (* Option abbreviation in argv	    *)
    specifier*: C.charPtr1d;    (* Resource specifier		    *)
    argKind*: XrmOptionKind;    (* Which style of option it is	    *)
    value*: X.XPointer;         (* Value to provide if XrmoptionNoArg   *)
  END;
  XrmOptionDescList* = POINTER TO ARRAY OF XrmOptionDescRec;
  
                         
(****************************************************************
 *
 * Memory Management
 *
 ****************************************************************)

PROCEDURE Xpermalloc* (
    size: C.int): C.address;

(****************************************************************
 *
 * Quark Management
 *
 ****************************************************************)

(* find quark for string, create new quark if none already exists *)
PROCEDURE XrmStringToQuark* (
    string: ARRAY OF C.char): XrmQuark;
PROCEDURE XrmPermStringToQuark* (
    string: ARRAY OF C.char): XrmQuark;

(* find string for quark *)
PROCEDURE XrmQuarkToString* (
    quark: XrmQuark): XrmString;
PROCEDURE XrmUniqueQuark* (): XrmQuark;

(****************************************************************
 *
 * Conversion of Strings to Lists
 *
 ****************************************************************)
 
PROCEDURE XrmStringToQuarkList* (
    string: ARRAY OF C.char;
    VAR quarks_return: ARRAY OF XrmQuark);
PROCEDURE XrmStringToBindingQuarkList* (
    string: ARRAY OF C.char;
    VAR bindings_return: ARRAY OF XrmBinding;
    VAR quarks_return: ARRAY OF XrmQuark): C.int;

(****************************************************************
 *
 * Name and Class lists.
 *
 ****************************************************************)

PROCEDURE XrmNameToString* ["XrmQuarkToString"] (
    quark: XrmQuark): XrmString;
PROCEDURE XrmStringToName* ["XrmStringToQuark"] (
    string: ARRAY OF C.char): XrmQuark;
PROCEDURE XrmStringToNameList* ["XrmStringToQuarkList"] (
    string: ARRAY OF C.char;
    VAR quarks_return: ARRAY OF XrmQuark);

PROCEDURE XrmClassToString* ["XrmQuarkToString"] (
    quark: XrmQuark): XrmString;
PROCEDURE XrmStringToClass* ["XrmStringToQuark"] (
    string: ARRAY OF C.char): XrmQuark;
PROCEDURE XrmStringToClassList* ["XrmStringToQuarkList"] (
    string: ARRAY OF C.char;
    VAR quarks_return: ARRAY OF XrmQuark);

(****************************************************************
 *
 * Resource Representation Types and Values
 *
 ****************************************************************)

PROCEDURE XrmStringToRepresentation* ["XrmStringToQuark"] (
    string: ARRAY OF C.char): XrmQuark;
PROCEDURE XrmRepresentationToString* ["XrmQuarkToString"] (
    quark: XrmQuark): XrmString;

(****************************************************************
 *
 * Resource Manager Functions
 *
 ****************************************************************)

PROCEDURE XrmDestroyDatabase* (
    database: XrmDatabase);
PROCEDURE XrmQPutResource* (
    VAR database: XrmDatabase;
    bindings: XrmBindingList;
    quarks: XrmQuarkList;
    type: XrmRepresentation;
    value: XrmValuePtr);
PROCEDURE XrmPutResource* (
    VAR database: XrmDatabase;
    specifier: ARRAY OF C.char;
    type: ARRAY OF C.char;
    value: XrmValuePtr);
PROCEDURE XrmQPutStringResource* (
    VAR database: XrmDatabase;
    bindings: XrmBindingList;
    quarks: XrmQuarkList;
    value: ARRAY OF C.char);
PROCEDURE XrmPutStringResource* (
    VAR database: XrmDatabase;
    specifier: ARRAY OF C.char;
    value: ARRAY OF C.char);
PROCEDURE XrmPutLineResource* (
    VAR database: XrmDatabase;
    line: ARRAY OF C.char);
PROCEDURE XrmQGetResource* (
    database: XrmDatabase;
    quark_name: XrmNameList;
    quark_class: XrmClassList;
    VAR quark_type_return: XrmRepresentation;
    VAR value_return: XrmValue);
PROCEDURE XrmGetResource* (
    database: XrmDatabase;
    str_name: ARRAY OF C.char;
    str_class: ARRAY OF C.char;
    VAR str_type_return: C.char;
    VAR value_return: XrmValue): X.Bool;
PROCEDURE XrmQGetSearchList* (
    database: XrmDatabase;
    names: XrmNameList;
    classes: XrmClassList;
    list_return: XrmSearchList;
    list_length: C.int): X.Bool;
PROCEDURE XrmQGetSearchResource* (
    list: XrmSearchList;
    name: XrmName;
    class: XrmClass;
    VAR type_return: XrmRepresentation;
    VAR value_return: XrmValue): X.Bool;
    
(****************************************************************
 *
 * Resource Database Management
 *
 ****************************************************************)
 
PROCEDURE XrmSetDatabase* (
    display: X.DisplayPtr;
    database: XrmDatabase);
PROCEDURE XrmGetDatabase* (
    display: X.DisplayPtr): XrmDatabase;
PROCEDURE XrmGetFileDatabase* (
    filename: ARRAY OF C.char): XrmDatabase;
PROCEDURE XrmCombineFileDatabase* (
    filename: ARRAY OF C.char;
    VAR target: XrmDatabase;
    override: X.Bool): X.Status;
PROCEDURE XrmGetStringDatabase* (
    data: ARRAY OF C.char): XrmDatabase;
PROCEDURE XrmPutFileDatabase* (
    database: XrmDatabase;
    filename: ARRAY OF C.char);
PROCEDURE XrmMergeDatabases* (
    source_db: XrmDatabase;
    VAR target_db: XrmDatabase);
PROCEDURE XrmCombineDatabase* (
    source_db: XrmDatabase;
    VAR target_db: XrmDatabase;
    override: X.Bool);
PROCEDURE XrmEnumerateDatabase* (
    db: XrmDatabase;
    name_prefix: XrmNameList;
    class_prefix: XrmClassList;
    mode: C.int;
    proc: EnumProc;
    closure: X.XPointer): X.Bool;
PROCEDURE XrmLocaleOfDatabase* (
    database: XrmDatabase): C.charPtr1d;

(****************************************************************
 *
 * Command line option mapping to resource entries
 *
 ****************************************************************)
PROCEDURE XrmParseCommand* (
    VAR database: XrmDatabase;
    table: XrmOptionDescList;
    table_count: C.int;
    name: ARRAY OF C.char;
    VAR argc_in_out: C.int;
    VAR argv_in_out: ARRAY OF C.charPtr1d);

END Xresource.
