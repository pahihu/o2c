#ifndef __OTable_
#define __OTable_

#ifdef MODULE_OTable
#define EXTERN_OTable
#else
#define EXTERN_OTable extern
#endif

#include "_OGCC.h"


struct TD_OTable_ObjectDesc;
struct TD_OTable_StructDesc;
struct TD_OTable_ConstDesc;

extern const SHORTINT OTable_objUndef;
extern const SHORTINT OTable_objScope;
extern const SHORTINT OTable_objConst;
extern const SHORTINT OTable_objType;
extern const SHORTINT OTable_objVar;
extern const SHORTINT OTable_objVarPar;
extern const SHORTINT OTable_objField;
extern const SHORTINT OTable_objExtProc;
extern const SHORTINT OTable_objIntProc;
extern const SHORTINT OTable_objLocalProc;
extern const SHORTINT OTable_objModule;
extern const SHORTINT OTable_objTBProc;
extern const SHORTINT OTable_objForwardType;
extern const SHORTINT OTable_objForwardProc;
extern const SHORTINT OTable_objForwardTBProc;
extern const SHORTINT OTable_exportNot;
extern const SHORTINT OTable_exportWrite;
extern const SHORTINT OTable_exportRead;
extern const SHORTINT OTable_strUndef;
extern const SHORTINT OTable_strBool;
extern const SHORTINT OTable_strChar;
extern const SHORTINT OTable_strShortInt;
extern const SHORTINT OTable_strInteger;
extern const SHORTINT OTable_strLongInt;
extern const SHORTINT OTable_strReal;
extern const SHORTINT OTable_strLongReal;
extern const SHORTINT OTable_strSet;
extern const SHORTINT OTable_strNone;
extern const SHORTINT OTable_strString;
extern const SHORTINT OTable_strNil;
extern const SHORTINT OTable_strPredeclMax;
extern const SHORTINT OTable_strSysByte;
extern const SHORTINT OTable_strSysPtr;
extern const SHORTINT OTable_strFixedMax;
extern const SHORTINT OTable_strPointer;
extern const SHORTINT OTable_strProc;
extern const SHORTINT OTable_strTBProc;
extern const SHORTINT OTable_strArray;
extern const SHORTINT OTable_strDynArray;
extern const SHORTINT OTable_strRecord;
extern const SHORTINT OTable_flagUsed;
extern const SHORTINT OTable_flagForward;
extern const SHORTINT OTable_flagHasLocalProc;
extern const SHORTINT OTable_flagExport;
extern const SHORTINT OTable_flagParam;
extern const SHORTINT OTable_flagReceiver;
extern const SHORTINT OTable_flagEmptyBody;
extern const SHORTINT OTable_flagUnion;
extern const SHORTINT OTable_flagExternal;
extern const SHORTINT OTable_flagAddressed;
extern const SHORTINT OTable_flagMinFree;
extern const SHORTINT OTable_predCHR;
extern const SHORTINT OTable_predENTIER;
extern const SHORTINT OTable_predLONG;
extern const SHORTINT OTable_predORD;
extern const SHORTINT OTable_predSHORT;
extern const SHORTINT OTable_predMAX;
extern const SHORTINT OTable_predMIN;
extern const SHORTINT OTable_predSIZE;
extern const SHORTINT OTable_predABS;
extern const SHORTINT OTable_predCAP;
extern const SHORTINT OTable_predODD;
extern const SHORTINT OTable_sysADR;
extern const SHORTINT OTable_sysCC;
extern const SHORTINT OTable_predASH;
extern const SHORTINT OTable_sysBIT;
extern const SHORTINT OTable_sysLSH;
extern const SHORTINT OTable_sysROT;
extern const SHORTINT OTable_predLEN;
extern const SHORTINT OTable_sysVAL;
extern const SHORTINT OTable_predINCL;
extern const SHORTINT OTable_predEXCL;
extern const SHORTINT OTable_predINC;
extern const SHORTINT OTable_predDEC;
extern const SHORTINT OTable_predCOPY;
extern const SHORTINT OTable_sysMOVE;
extern const SHORTINT OTable_sysGET;
extern const SHORTINT OTable_sysPUT;
extern const SHORTINT OTable_sysGETREG;
extern const SHORTINT OTable_sysPUTREG;
extern const SHORTINT OTable_sysNEW;
extern const SHORTINT OTable_predNEW;
extern const SHORTINT OTable_predHALT;
extern const SHORTINT OTable_predASSERT;
extern const SHORTINT OTable_sysDISPOSE;
extern const SHORTINT OTable_sysCOLLECT;
extern const SHORTINT OTable_noPos;
extern const CHAR OTable_noIdent;
extern const CHAR OTable_genericParam[];
typedef struct OTable_ObjectDesc (* OTable_Object);
typedef struct OTable_StructDesc (* OTable_Struct);
typedef struct OTable_ConstDesc (* OTable_Const);
typedef CHAR (* OTable_String)[256];
typedef struct OTable_ObjectDesc {
  SHORTINT _mode;
  OTable_Object _scope;
  OTable_Object _left;
  OTable_Object _right;
  OTable_Object _link;
  OTable_Object _next;
  OTable_Object _sort;
  CHAR _name[48];
  OTable_String _extName;
  SHORTINT _mark;
  OTable_Struct _type;
  OTable_Const _const;
  SET _flags;
  LONGINT _mnolev;
  LONGINT _pos;
  LONGINT _adr;
} OTable_ObjectDesc;
typedef struct OTable_StructDesc {
  SHORTINT _form;
  OTable_Struct _base;
  OTable_Object _obj;
  OTable_Object _link;
  SET _flags;
  LONGINT _len;
  LONGINT _size;
} OTable_StructDesc;
typedef struct OTable_ConstDesc {
  OTable_String _string;
  LONGINT _intval;
  LONGINT _intval2;
  SET _set;
  LONGREAL _real;
} OTable_ConstDesc;
extern LONGINT (* OTable_structSize) (OTable_Struct OTable_t);
extern const SHORTINT OTable_compileMnolev;
extern const SHORTINT OTable_predeclMnolev;
extern const SHORTINT OTable_systemMnolev;
extern const SHORTINT OTable_importMnolev;
extern OTable_Object OTable_compiledModule;
extern OTable_Struct OTable_predeclType[14];
extern BOOLEAN OTable_external;

typedef struct TD_OTable_ObjectDesc {
  TDCORE
} TD_OTable_ObjectDesc;
EXTERN_OTable TD_OTable_ObjectDesc* td_OTable_ObjectDesc;
typedef struct TD_OTable_StructDesc {
  TDCORE
} TD_OTable_StructDesc;
EXTERN_OTable TD_OTable_StructDesc* td_OTable_StructDesc;
typedef struct TD_OTable_ConstDesc {
  TDCORE
} TD_OTable_ConstDesc;
EXTERN_OTable TD_OTable_ConstDesc* td_OTable_ConstDesc;

extern void OTable_RecycleMem (OTable_Object (* _module));
extern void OTable_OpenScope (OTable_Object _scope);
extern void OTable_CloseScope (BOOLEAN _check);
extern LONGINT OTable_LevelOfTopScope (void);
extern void OTable_InsertSymbolTable (LONGINT __module_0, const CHAR (* __module_p), OTable_Object _table);
extern void OTable_InsertSymbolTableImport (LONGINT __module_0, const CHAR (* __module_p), LONGINT __imports_0, const CHAR (* __imports_p));
extern OTable_Object OTable_GetSymbolTable (LONGINT __module_0, const CHAR (* __module_p));
extern void OTable_FlushSymbolTable (LONGINT __module_0, const CHAR (* __module_p));
extern LONGINT OTable_GetImportMnolev (void);
extern OTable_Const OTable_NewConst (void);
extern OTable_Struct OTable_NewStruct (SHORTINT _form);
extern OTable_Object OTable_NewObject (LONGINT __name_0, const CHAR (* __name_p), SHORTINT _mode, LONGINT _pos);
extern OTable_Object OTable_SearchInTree (OTable_Object _root, LONGINT __ident_0, const CHAR (* __ident_p));
extern void OTable_OptimizeTree (OTable_Object (* _root));
extern BOOLEAN OTable_SameType (OTable_Struct _a, OTable_Struct _b);
extern BOOLEAN OTable_EqualType (OTable_Struct _a, OTable_Struct _b);
extern BOOLEAN OTable_ExtOf (OTable_Struct _a, OTable_Struct _b);
extern BOOLEAN OTable_ParamsMatch (OTable_Struct _str1, OTable_Struct _str2);
extern BOOLEAN OTable_ContainsPointer (OTable_Struct _str);
extern void OTable_Insert (OTable_Object (* _obj));
extern void OTable_InsertForOSym (OTable_Object (* _obj), OTable_Object (* _last));
extern void OTable_InsertParams (OTable_Object _proc);
extern OTable_Object OTable_Find (LONGINT __ident_0, const CHAR (* __ident_p));
extern OTable_Object OTable_FindField (LONGINT __ident_0, const CHAR (* __ident_p), OTable_Struct _struct);
extern void OTable_InsertField (OTable_Object _obj, OTable_Struct _struct);
extern void OTable_ImportInsertField (OTable_Object _obj, OTable_Struct _struct);
extern OTable_Object OTable_InternalFindImport (LONGINT __ident_0, const CHAR (* __ident_p), OTable_Object _mod);
extern OTable_Object OTable_FindImport (LONGINT __ident_0, const CHAR (* __ident_p), OTable_Object _mod);
extern OTable_Object OTable_GetModuleForObject (OTable_Object _obj);
extern void OTable_GetModuleName (OTable_Object _obj, LONGINT __name_0, CHAR (* _name));
extern void OTable_InitTable (void);

extern void _init_OTable (void);

#endif
