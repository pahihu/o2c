#define MODULE_OTable

#include "OTable.h"
#include "OMachine.h"
#include "OScan.h"
#include "Out.h"
#include "Rts.h"

static ModuleId moduleId;


struct TD_OTable_ScopeStackDesc;
struct TD_OTable_SymbolTableCacheDesc;
struct TD_OTable_SymbolTableImportedDesc;

static const SHORTINT OTable_objCollected = 31;
const SHORTINT OTable_objUndef = 0;
const SHORTINT OTable_objScope = 1;
const SHORTINT OTable_objConst = 2;
const SHORTINT OTable_objType = 3;
const SHORTINT OTable_objVar = 4;
const SHORTINT OTable_objVarPar = 5;
const SHORTINT OTable_objField = 6;
const SHORTINT OTable_objExtProc = 7;
const SHORTINT OTable_objIntProc = 8;
const SHORTINT OTable_objLocalProc = 9;
const SHORTINT OTable_objModule = 10;
const SHORTINT OTable_objTBProc = 11;
const SHORTINT OTable_objForwardType = 12;
const SHORTINT OTable_objForwardProc = 13;
const SHORTINT OTable_objForwardTBProc = 14;
static const SHORTINT OTable_objProtoTBProc = 15;
static const SET OTable_procedureObjects = 0x00000300U;
static const SET OTable_forwardObjects = 0x00007000U;
static const SET OTable_scopeObjects = 0x00000F00U;
static const SET OTable_methodObjects = 0x0000C800U;
static const SET OTable_usableObjects = 0x00000FFCU;
const SHORTINT OTable_exportNot = 0;
const SHORTINT OTable_exportWrite = 2;
const SHORTINT OTable_exportRead = 1;
const SHORTINT OTable_strUndef = 0;
const SHORTINT OTable_strBool = 1;
const SHORTINT OTable_strChar = 2;
const SHORTINT OTable_strShortInt = 3;
const SHORTINT OTable_strInteger = 4;
const SHORTINT OTable_strLongInt = 5;
const SHORTINT OTable_strReal = 6;
const SHORTINT OTable_strLongReal = 7;
const SHORTINT OTable_strSet = 8;
const SHORTINT OTable_strNone = 9;
const SHORTINT OTable_strString = 10;
const SHORTINT OTable_strNil = 11;
const SHORTINT OTable_strPredeclMax = 11;
const SHORTINT OTable_strSysByte = 12;
const SHORTINT OTable_strSysPtr = 13;
const SHORTINT OTable_strFixedMax = 13;
const SHORTINT OTable_strPointer = 14;
const SHORTINT OTable_strProc = 15;
const SHORTINT OTable_strTBProc = 16;
const SHORTINT OTable_strArray = 17;
const SHORTINT OTable_strDynArray = 18;
const SHORTINT OTable_strRecord = 19;
const SHORTINT OTable_flagUsed = 0;
const SHORTINT OTable_flagForward = 1;
const SHORTINT OTable_flagHasLocalProc = 2;
const SHORTINT OTable_flagExport = 5;
const SHORTINT OTable_flagParam = 6;
const SHORTINT OTable_flagReceiver = 7;
const SHORTINT OTable_flagEmptyBody = 8;
const SHORTINT OTable_flagUnion = 9;
const SHORTINT OTable_flagExternal = 10;
const SHORTINT OTable_flagAddressed = 11;
const SHORTINT OTable_flagMinFree = 16;
const SHORTINT OTable_predCHR = 13;
const SHORTINT OTable_predENTIER = 14;
const SHORTINT OTable_predLONG = 15;
const SHORTINT OTable_predORD = 16;
const SHORTINT OTable_predSHORT = 17;
const SHORTINT OTable_predMAX = 18;
const SHORTINT OTable_predMIN = 19;
const SHORTINT OTable_predSIZE = 20;
const SHORTINT OTable_predABS = 21;
const SHORTINT OTable_predCAP = 22;
const SHORTINT OTable_predODD = 23;
const SHORTINT OTable_sysADR = 24;
const SHORTINT OTable_sysCC = 25;
const SHORTINT OTable_predASH = 26;
const SHORTINT OTable_sysBIT = 27;
const SHORTINT OTable_sysLSH = 28;
const SHORTINT OTable_sysROT = 29;
const SHORTINT OTable_predLEN = 30;
const SHORTINT OTable_sysVAL = 31;
const SHORTINT OTable_predINCL = 32;
const SHORTINT OTable_predEXCL = 33;
const SHORTINT OTable_predINC = 34;
const SHORTINT OTable_predDEC = 35;
const SHORTINT OTable_predCOPY = 36;
const SHORTINT OTable_sysMOVE = 37;
const SHORTINT OTable_sysGET = 38;
const SHORTINT OTable_sysPUT = 39;
const SHORTINT OTable_sysGETREG = 40;
const SHORTINT OTable_sysPUTREG = 41;
const SHORTINT OTable_sysNEW = 42;
const SHORTINT OTable_predNEW = 43;
const SHORTINT OTable_predHALT = 45;
const SHORTINT OTable_predASSERT = 46;
const SHORTINT OTable_sysDISPOSE = 47;
const SHORTINT OTable_sysCOLLECT = 48;
const SHORTINT OTable_noPos = -1;
const CHAR OTable_noIdent = '@';
const CHAR OTable_genericParam[] = "##";
LONGINT (* OTable_structSize) (OTable_Struct OTable_t);
typedef struct OTable_ScopeStackDesc (* OTable_ScopeStack);
typedef struct OTable_ScopeStackDesc {
  OTable_ScopeStack _last;
  OTable_Object _topScope;
} OTable_ScopeStackDesc;
static OTable_ScopeStack OTable_scopeStack;
typedef struct OTable_SymbolTableImportedDesc (* OTable_SymbolTableImported);
typedef struct OTable_SymbolTableCacheDesc (* OTable_SymbolTableCache);
typedef struct OTable_SymbolTableCacheDesc {
  OTable_SymbolTableCache _pred;
  OTable_SymbolTableCache _succ;
  CHAR _name[48];
  OTable_Object _symtable;
  OTable_SymbolTableImported _imported;
} OTable_SymbolTableCacheDesc;
typedef struct OTable_SymbolTableImportedDesc {
  OTable_SymbolTableImported _next;
  OTable_SymbolTableCache _this;
} OTable_SymbolTableImportedDesc;
static OTable_SymbolTableCache OTable_symbolCache;
static LONGINT OTable_cacheMnolev;
static const CHAR OTable_systemIdent[] = "SYSTEM";
static const LONGINT OTable_systemKey = 1283764;
const SHORTINT OTable_compileMnolev = 0;
const SHORTINT OTable_predeclMnolev = -1;
const SHORTINT OTable_systemMnolev = -2;
const SHORTINT OTable_importMnolev = -3;
static OTable_Object OTable_predefined, OTable_system;
OTable_Object OTable_compiledModule;
OTable_Struct OTable_predeclType[14];
BOOLEAN OTable_external;
static OTable_Object OTable_collectedObjs;


typedef struct TD_OTable_ScopeStackDesc {
  TDCORE
} TD_OTable_ScopeStackDesc;
TD_OTable_ScopeStackDesc* td_OTable_ScopeStackDesc;
typedef struct TD_OTable_SymbolTableCacheDesc {
  TDCORE
} TD_OTable_SymbolTableCacheDesc;
TD_OTable_SymbolTableCacheDesc* td_OTable_SymbolTableCacheDesc;
typedef struct TD_OTable_SymbolTableImportedDesc {
  TDCORE
} TD_OTable_SymbolTableImportedDesc;
TD_OTable_SymbolTableImportedDesc* td_OTable_SymbolTableImportedDesc;

OTable_Object OTable_NewObject (LONGINT __name_0, const CHAR (* __name_p), SHORTINT _mode, LONGINT _pos);

OTable_Object OTable_FindField (LONGINT __ident_0, const CHAR (* __ident_p), OTable_Struct _struct);

BOOLEAN OTable_ParamsMatch (OTable_Struct _str1, OTable_Struct _str2);

void OTable_InsertField (OTable_Object _obj, OTable_Struct _struct);

static BOOLEAN OTable_ResidentModule (LONGINT __module_0, const CHAR (* __module_p)) {
  CHAR (* _module);
  VALUE_ARRAY(_module, __module_p, 1*__module_0);
  return STREQL(_module, OTable_systemIdent);
  NO_RETURN (7947);
}

static BOOLEAN OTable_IsLocalObject (OTable_Object _obj) {
  return DEREF(OTable_Object, _obj, 8130)._mnolev>=0;
  NO_RETURN (8071);
}

static void OTable_NEWObject (OTable_Object (* _obj)) {
  if ((void*)OTable_collectedObjs!=(void*)NULL)  {
    (* _obj) = OTable_collectedObjs;
    OTable_collectedObjs = DEREF(OTable_Object, OTable_collectedObjs, 8536)._next;
  } else {
    NEWREC((* _obj), td_OTable_ObjectDesc, 0);
  }
}

static void OTable_CollectObject (OTable_Object _obj) {
  DEREF(OTable_Object, _obj, 8648)._mode = OTable_objCollected;
  DEREF(OTable_Object, _obj, 8676)._next = OTable_collectedObjs;
  OTable_collectedObjs = _obj;
}

void OTable_RecycleMem (OTable_Object (* _module)) {
  LONGINT _mnolev;

  auto void _RecycleObject (OTable_Object _obj);

  BOOLEAN _MnolevOk (LONGINT _this, LONGINT _wanted) {
    return _this==_wanted||(_this>=_wanted&&_wanted>=0);
    NO_RETURN (8865);
  }

  void _RecycleStruct (OTable_Struct _str) {
    if ((void*)_str!=(void*)NULL)  {
      if (DEREF(OTable_Struct, _str, 9105)._form==OTable_strRecord&&((void*)DEREF(OTable_Struct, _str, 9144)._obj==(void*)NULL||_MnolevOk(DEREF(OTable_Object, DEREF(OTable_Struct, _str, 9171)._obj, 9175)._mnolev, _mnolev)))  {
        _RecycleObject(DEREF(OTable_Struct, _str, 9224)._link);
      }
    }
  }

  void _RecycleObject (OTable_Object _obj) {
    if ((void*)_obj!=(void*)NULL)  {
      if (IN(DEREF(OTable_Object, _obj, 9373)._mode, 0x00000FFCU, 9379))  {
        _RecycleObject(DEREF(OTable_Object, _obj, 9439)._left);
        _RecycleObject(DEREF(OTable_Object, _obj, 9474)._right);
      }
      if (IN(DEREF(OTable_Object, _obj, 9511)._mode, 0x00000B82U, 9517))  {
        _RecycleObject(DEREF(OTable_Object, _obj, 9601)._link);
      } else if (DEREF(OTable_Object, _obj, 9627)._mode==OTable_objType)  {
        _RecycleStruct(DEREF(OTable_Object, _obj, 9676)._type);
      }
      if (!IN(DEREF(OTable_Object, _obj, 9713)._mode, 0x80000000U, 9719)&&_MnolevOk(DEREF(OTable_Object, _obj, 9752)._mnolev, _mnolev))  {
        OTable_CollectObject(_obj);
      }
    }
  }
  if ((void*)(* _module)!=(void*)NULL&&!OTable_ResidentModule(48, (CHAR *) DEREF(OTable_Object, (* _module), 9905)._name))  {
    _mnolev = DEREF(OTable_Object, (* _module), 9957)._mnolev;
    _RecycleObject(DEREF(OTable_Object, (* _module), 9992)._link);
    OTable_CollectObject((* _module));
  }
}

static void OTable_CheckUnresolvedForward (OTable_Object _scope, OTable_Object _obj);

static void OTable_WarnUnused (OTable_Object _scope);

static void OTable_PutModulesNextAndSort (OTable_Object _scope);

void OTable_OpenScope (OTable_Object _scope) {
  OTable_ScopeStack _new;
  NEWREC(_new, td_OTable_ScopeStackDesc, 0);
  DEREF(OTable_ScopeStack, _new, 10486)._last = OTable_scopeStack;
  DEREF(OTable_ScopeStack, _new, 10512)._topScope = _scope;
  OTable_scopeStack = _new;
  if ((void*)_scope==(void*)NULL)  {
    Out_String(53, (CHAR *) "internal compiler error in front end: OpenScope(NIL)");
    Out_Ln();
  }
}

void OTable_CloseScope (BOOLEAN _check) {
  OTable_ScopeStack _dealloc;
  if ((void*)OTable_scopeStack!=(void*)NULL)  {
    if (_check)  {
      OTable_CheckUnresolvedForward(DEREF(OTable_ScopeStack, OTable_scopeStack, 10919)._topScope, DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 10939)._topScope, 10948)._link);
      OTable_WarnUnused(DEREF(OTable_ScopeStack, OTable_scopeStack, 10985)._topScope);
      OTable_PutModulesNextAndSort(DEREF(OTable_ScopeStack, OTable_scopeStack, 11037)._topScope);
    }
    _dealloc = OTable_scopeStack;
    OTable_scopeStack = DEREF(OTable_ScopeStack, OTable_scopeStack, 11125)._last;
    DISPOSE_REC(_dealloc);
  } else {
    Rts_Error((CHAR *) "internal compiler error in front end: more CloseScope than OpenScope");
  }
}

LONGINT OTable_LevelOfTopScope (void) {
  return DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 11358)._topScope, 11367)._mnolev;
  NO_RETURN (11299);
}

static void OTable_InitScopeStack (void) {
  OTable_scopeStack = (OTable_ScopeStack)NULL;
}

static void OTable_CheckUnresolvedForward (OTable_Object _scope, OTable_Object _obj) {
  if ((void*)_obj!=(void*)NULL)  {
    OTable_CheckUnresolvedForward(_scope, DEREF(OTable_Object, _obj, 11778)._left);
    if (IN(DEREF(OTable_Object, _obj, 11799)._mode, OTable_forwardObjects, 11805))  {
      OScan_Err(DEREF(OTable_Object, _obj, 11847)._pos, 65);
    } else if (DEREF(OTable_Object, _obj, 11873)._mode==OTable_objType&&DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 11896)._type, 11901)._form==OTable_strRecord)  {
      OTable_CheckUnresolvedForward(_scope, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 11965)._type, 11970)._link);
    } else if (IN(DEREF(OTable_Object, _obj, 11994)._mode, OTable_procedureObjects, 12000))  {
      INCL(DEREF(OTable_Object, _scope, 12122)._flags, OTable_flagHasLocalProc, 12112);
    }
    OTable_CheckUnresolvedForward(_scope, DEREF(OTable_Object, _obj, 12198)._right);
  }
}

static void OTable_WarnUnused (OTable_Object _scope) {

  BOOLEAN _IsRedefinition (OTable_Object _tbproc) {
    OTable_Struct _str;
    _str = DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OTable_Object, _tbproc, 12553)._type, 12558)._link, 12563)._type;
    if ((void*)_str!=(void*)NULL&&DEREF(OTable_Struct, _str, 12631)._form==OTable_strPointer)  {
      _str = DEREF(OTable_Struct, _str, 12733)._base;
    }
    return (void*)OTable_FindField(48, (CHAR *) DEREF(OTable_Object, _tbproc, 12789)._name, DEREF(OTable_Struct, _str, 12798)._base)!=(void*)NULL;
    NO_RETURN (12458);
  }

  void _WarnUnusedRec (OTable_Object _obj) {
    if ((void*)_obj!=(void*)NULL)  {
      _WarnUnusedRec(DEREF(OTable_Object, _obj, 13000)._left);
      if (!IN(OTable_flagUsed, DEREF(OTable_Object, _obj, 13036)._flags, 13030))  {
        if (IN(OTable_flagParam, DEREF(OTable_Object, _obj, 13079)._flags, 13073)&&IN(OTable_flagEmptyBody, DEREF(OTable_Object, _scope, 13112)._flags, 13104))  {
        } else if ((DEREF(OTable_Object, _obj, 13201)._mode==OTable_objTBProc&&DEREF(OTable_Object, _obj, 13226)._mark==OTable_exportNot)&&_IsRedefinition(_obj))  {
        } else if (IN(DEREF(OTable_Object, _obj, 13345)._mode, OTable_usableObjects, 13351)&&DEREF(OTable_Object, _obj, 13375)._mark==OTable_exportNot)  {
          OScan_Warn(DEREF(OTable_Object, _obj, 13465)._pos, 90);
        }
      }
      if (DEREF(OTable_Object, _obj, 13527)._mode==OTable_objType&&DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 13550)._type, 13555)._form==OTable_strRecord)  {
        _WarnUnusedRec(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 13606)._type, 13611)._link);
      }
      _WarnUnusedRec(DEREF(OTable_Object, _obj, 13667)._right);
    }
  }
  if (OTable_IsLocalObject(_scope))  {
    _WarnUnusedRec(DEREF(OTable_Object, _scope, 13781)._link);
  }
}

static void OTable_PutModulesNextAndSort (OTable_Object _scope) {
  OTable_Object _module, _sort, _first;

  void _RecCollectAndSort (OTable_Object _obj) {
    if ((void*)_obj!=(void*)NULL)  {
      _RecCollectAndSort(DEREF(OTable_Object, _obj, 14273)._left);
      if (DEREF(OTable_Object, _obj, 14296)._mode==OTable_objModule)  {
        DEREF(OTable_Object, _module, 14336)._next = _obj;
        DEREF(OTable_Object, _obj, 14351)._next = (OTable_Object)NULL;
        _module = _obj;
      } else if (!IN(OTable_flagParam, DEREF(OTable_Object, _obj, 14408)._flags, 14402))  {
        DEREF(OTable_Object, _sort, 14435)._sort = _obj;
        DEREF(OTable_Object, _obj, 14450)._sort = (OTable_Object)NULL;
        _sort = _obj;
      }
      _RecCollectAndSort(DEREF(OTable_Object, _obj, 14524)._right);
    }
  }
  DEREF(OTable_Object, _scope, 14598)._next = (OTable_Object)NULL;
  _first = OTable_NewObject(2, (CHAR *) "@", OTable_objCollected, -1);
  _module = _first;
  _sort = _first;
  _RecCollectAndSort(DEREF(OTable_Object, _scope, 14719)._link);
  DEREF(OTable_Object, _scope, 14736)._next = DEREF(OTable_Object, _first, 14748)._next;
  DEREF(OTable_Object, _scope, 14760)._sort = DEREF(OTable_Object, _first, 14772)._sort;
  OTable_CollectObject(_first);
}

static OTable_SymbolTableCache OTable_SearchSymbolTable (LONGINT __module_0, const CHAR (* __module_p)) {
  CHAR (* _module);
  OTable_SymbolTableCache _walk;
  VALUE_ARRAY(_module, __module_p, 1*__module_0);
  _walk = OTable_symbolCache;
  while ((void*)_walk!=(void*)NULL&&STRLSS(DEREF(OTable_SymbolTableCache, _walk, 15229)._name, _module))  {
    _walk = DEREF(OTable_SymbolTableCache, _walk, 15264)._succ;
  }
  if ((void*)_walk!=(void*)NULL&&STREQL(DEREF(OTable_SymbolTableCache, _walk, 15307)._name, _module))  {
    return _walk;
  } else {
    return NULL;
  }
  NO_RETURN (15074);
}

static OTable_SymbolTableCache OTable_RemoveSymbolTable (LONGINT __module_0, const CHAR (* __module_p)) {
  CHAR (* _module);
  OTable_SymbolTableCache _walk;
  VALUE_ARRAY(_module, __module_p, 1*__module_0);
  _walk = OTable_SearchSymbolTable(__module_0, _module);
  if ((void*)_walk!=(void*)NULL)  {
    if ((void*)DEREF(OTable_SymbolTableCache, _walk, 15638)._pred!=(void*)NULL)  {
      DEREF(OTable_SymbolTableCache, DEREF(OTable_SymbolTableCache, _walk, 15668)._pred, 15673)._succ = DEREF(OTable_SymbolTableCache, _walk, 15684)._succ;
    } else {
      OTable_symbolCache = DEREF(OTable_SymbolTableCache, _walk, 15727)._succ;
    }
    if ((void*)DEREF(OTable_SymbolTableCache, _walk, 15759)._succ!=(void*)NULL)  {
      DEREF(OTable_SymbolTableCache, DEREF(OTable_SymbolTableCache, _walk, 15789)._succ, 15794)._pred = DEREF(OTable_SymbolTableCache, _walk, 15805)._pred;
    }
    DEREF(OTable_SymbolTableCache, _walk, 15833)._succ = (OTable_SymbolTableCache)NULL;
    DEREF(OTable_SymbolTableCache, _walk, 15855)._pred = (OTable_SymbolTableCache)NULL;
    OTable_RecycleMem(&(DEREF(OTable_SymbolTableCache, _walk, 15888)._symtable));
    return _walk;
  } else {
    Out_String(52, (CHAR *) "compiler error\? Symboltable to remove was not found");
    Out_Ln();
    return NULL;
  }
  NO_RETURN (15421);
}

void OTable_InsertSymbolTable (LONGINT __module_0, const CHAR (* __module_p), OTable_Object _table) {
  CHAR (* _module);
  OTable_SymbolTableCache _pre, _walk, _new;
  VALUE_ARRAY(_module, __module_p, 1*__module_0);
  _walk = OTable_SearchSymbolTable(__module_0, _module);
  if ((void*)_walk!=(void*)NULL)  {
    Rts_Error((CHAR *) "internal compiler error in front end: Module more than once in cache");
  }
  _pre = (OTable_SymbolTableCache)NULL;
  _walk = OTable_symbolCache;
  while ((void*)_walk!=(void*)NULL&&STRLSS(DEREF(OTable_SymbolTableCache, _walk, 16412)._name, _module))  {
    _pre = _walk;
    _walk = DEREF(OTable_SymbolTableCache, _walk, 16458)._succ;
  }
  NEWREC(_new, td_OTable_SymbolTableCacheDesc, 0);
  COPY(_module, DEREF(OTable_SymbolTableCache, _new, 16508)._name, 48);
  DEREF(OTable_SymbolTableCache, _new, 16524)._symtable = _table;
  DEREF(OTable_SymbolTableCache, _new, 16545)._imported = (OTable_SymbolTableImported)NULL;
  DEREF(OTable_SymbolTableCache, _new, 16568)._pred = _pre;
  DEREF(OTable_SymbolTableCache, _new, 16583)._succ = _walk;
  if ((void*)_pre==(void*)NULL)  {
    OTable_symbolCache = _new;
  } else {
    DEREF(OTable_SymbolTableCache, _pre, 16663)._succ = _new;
  }
  if ((void*)_walk!=(void*)NULL)  {
    DEREF(OTable_SymbolTableCache, _walk, 16719)._pred = _new;
  }
}

void OTable_InsertSymbolTableImport (LONGINT __module_0, const CHAR (* __module_p), LONGINT __imports_0, const CHAR (* __imports_p)) {
  CHAR (* _module);
  CHAR (* _imports);
  OTable_SymbolTableCache _mod, _imp;
  OTable_SymbolTableImported _new;
  VALUE_ARRAY(_module, __module_p, 1*__module_0);
  VALUE_ARRAY(_imports, __imports_p, 1*__imports_0);
  _mod = OTable_SearchSymbolTable(__module_0, _module);
  _imp = OTable_SearchSymbolTable(__imports_0, _imports);
  if ((void*)_mod!=(void*)NULL&&(void*)_imp!=(void*)NULL)  {
    NEWREC(_new, td_OTable_SymbolTableImportedDesc, 0);
    DEREF(OTable_SymbolTableImported, _new, 17076)._this = _mod;
    DEREF(OTable_SymbolTableImported, _new, 17091)._next = DEREF(OTable_SymbolTableCache, _imp, 17101)._imported;
    DEREF(OTable_SymbolTableCache, _imp, 17115)._imported = _new;
  } else {
    Out_String(72, (CHAR *) "internal compiler error in front end: problems with the cache hierachie");
    Out_Ln();
  }
}

OTable_Object OTable_GetSymbolTable (LONGINT __module_0, const CHAR (* __module_p)) {
  CHAR (* _module);
  OTable_SymbolTableCache _walk;
  VALUE_ARRAY(_module, __module_p, 1*__module_0);
  _walk = OTable_SearchSymbolTable(__module_0, _module);
  if ((void*)_walk!=(void*)NULL)  {
    return DEREF(OTable_SymbolTableCache, _walk, 17468)._symtable;
  } else {
    return NULL;
  }
  NO_RETURN (17299);
}

static void OTable_FlushTable (OTable_SymbolTableCache _flush) {
  OTable_SymbolTableImported _thisflush;
  if ((void*)_flush!=(void*)NULL)  {
    while ((void*)DEREF(OTable_SymbolTableCache, _flush, 17681)._imported!=(void*)NULL)  {
      _thisflush = DEREF(OTable_SymbolTableCache, _flush, 17725)._imported;
      DEREF(OTable_SymbolTableCache, _flush, 17749)._imported = DEREF(OTable_SymbolTableImported, DEREF(OTable_SymbolTableCache, _flush, 17765)._imported, 17774)._next;
      OTable_FlushTable(DEREF(OTable_SymbolTableImported, _thisflush, 17809)._this);
    }
  }
}

void OTable_FlushSymbolTable (LONGINT __module_0, const CHAR (* __module_p)) {
  CHAR (* _module);
  OTable_SymbolTableCache _flush;
  VALUE_ARRAY(_module, __module_p, 1*__module_0);
  _flush = OTable_SearchSymbolTable(__module_0, _module);
  OTable_FlushTable(_flush);
}

static void OTable_InitSymbolCache (void) {
  OTable_symbolCache = (OTable_SymbolTableCache)NULL;
  OTable_cacheMnolev = -3;
  OTable_InsertSymbolTable(7, (CHAR *) OTable_systemIdent, OTable_system);
  OTable_cacheMnolev = -3;
}

static void OTable_FlushSymbolCache (void) {
  OTable_SymbolTableCache _flush;
  Out_String(28, (CHAR *) "FlushSymbolCache was called");
  Out_Ln();
  while ((void*)OTable_symbolCache!=(void*)NULL)  {
    _flush = OTable_RemoveSymbolTable(48, (CHAR *) DEREF(OTable_SymbolTableCache, OTable_symbolCache, 18424)._name);
    OTable_FlushTable(_flush);
  }
}

LONGINT OTable_GetImportMnolev (void) {
  if (OTable_cacheMnolev>OMachine_minLInt)  {
    DECLI(OTable_cacheMnolev, 1, 18583);
  } else {
    Out_String(50, (CHAR *) "Cache-Overflow: flushing all caches, don't panic.");
    Out_Ln();
    OTable_FlushSymbolCache();
    OTable_InitSymbolCache();
  }
  return OTable_cacheMnolev;
  NO_RETURN (18501);
}

OTable_Const OTable_NewConst (void) {
  OTable_Const _const;
  NEWREC(_const, td_OTable_ConstDesc, 0);
  DEREF(OTable_Const, _const, 19157)._intval = 0;
  DEREF(OTable_Const, _const, 19174)._intval2 = 0;
  DEREF(OTable_Const, _const, 19196)._real = 0.0000000000000000E+00;
  DEREF(OTable_Const, _const, 19213)._set = 0x00000000U;
  DEREF(OTable_Const, _const, 19228)._string = (OTable_String)NULL;
  return _const;
  NO_RETURN (19080);
}

OTable_Struct OTable_NewStruct (SHORTINT _form) {
  OTable_Struct _str;
  NEWREC(_str, td_OTable_StructDesc, 0);
  DEREF(OTable_Struct, _str, 19376)._form = _form;
  DEREF(OTable_Struct, _str, 19392)._flags = 0x00000000U;
  DEREF(OTable_Struct, _str, 19407)._len = 0;
  DEREF(OTable_Struct, _str, 19423)._base = (OTable_Struct)NULL;
  DEREF(OTable_Struct, _str, 19438)._obj = (OTable_Object)NULL;
  DEREF(OTable_Struct, _str, 19452)._link = (OTable_Object)NULL;
  DEREF(OTable_Struct, _str, 19471)._size = (-9223372036854775807L-1);
  return _str;
  NO_RETURN (19288);
}

OTable_Object OTable_NewObject (LONGINT __name_0, const CHAR (* __name_p), SHORTINT _mode, LONGINT _pos) {
  CHAR (* _name);
  OTable_Object _new;
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  OTable_NEWObject(&(_new));
  DEREF(OTable_Object, _new, 19666)._mode = _mode;
  DEREF(OTable_Object, _new, 19686)._mark = OTable_exportNot;
  COPY(_name, DEREF(OTable_Object, _new, 19721)._name, 48);
  DEREF(OTable_Object, _new, 19736)._extName = (OTable_String)NULL;
  DEREF(OTable_Object, _new, 19759)._scope = (OTable_Object)NULL;
  DEREF(OTable_Object, _new, 19775)._left = (OTable_Object)NULL;
  DEREF(OTable_Object, _new, 19790)._right = (OTable_Object)NULL;
  DEREF(OTable_Object, _new, 19810)._link = (OTable_Object)NULL;
  DEREF(OTable_Object, _new, 19825)._next = (OTable_Object)NULL;
  DEREF(OTable_Object, _new, 19840)._sort = (OTable_Object)NULL;
  DEREF(OTable_Object, _new, 19859)._const = (OTable_Const)NULL;
  DEREF(OTable_Object, _new, 19875)._adr = 0;
  DEREF(OTable_Object, _new, 19891)._pos = _pos;
  DEREF(OTable_Object, _new, 19905)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 19923);
  DEREF(OTable_Object, _new, 19942)._flags = 0x00000000U;
  DEREF(OTable_Object, _new, 19957)._mnolev = 0;
  if (IN(_mode, OTable_forwardObjects, 19995))  {
    INCL(DEREF(OTable_Object, _new, 20033)._flags, OTable_flagForward, 20025);
  }
  if (IN(_mode, OTable_scopeObjects, 20076))  {
    DEREF(OTable_Object, _new, 20107)._link = OTable_NewObject(4, (CHAR *) OScan_undefStr, OTable_objScope, _pos);
    DEREF(OTable_Object, DEREF(OTable_Object, _new, 20160)._link, 20165)._left = _new;
  }
  if ((void*)OTable_scopeStack!=(void*)NULL&&!IN(_mode, 0x00000040U, 20222))  {
    DEREF(OTable_Object, _new, 20252)._scope = DEREF(OTable_ScopeStack, OTable_scopeStack, 20270)._topScope;
  } else {
    DEREF(OTable_Object, _new, 20299)._scope = (OTable_Object)NULL;
  }
  if (_mode==OTable_objModule)  {
    DEREF(OTable_Object, _new, 20366)._const = OTable_NewConst();
  }
  return _new;
  NO_RETURN (19537);
}

static void OTable_CopyStruct (OTable_Struct _source, OTable_Struct _dest) {
  DEREF(OTable_Struct, _dest, 20534)._flags = DEREF(OTable_Struct, _source, 20548)._flags;
  DEREF(OTable_Struct, _dest, 20564)._form = DEREF(OTable_Struct, _source, 20578)._form;
  DEREF(OTable_Struct, _dest, 20593)._len = DEREF(OTable_Struct, _source, 20607)._len;
  DEREF(OTable_Struct, _dest, 20621)._base = DEREF(OTable_Struct, _source, 20635)._base;
  DEREF(OTable_Struct, _dest, 20650)._link = DEREF(OTable_Struct, _source, 20664)._link;
  DEREF(OTable_Struct, _dest, 20679)._size = DEREF(OTable_Struct, _source, 20693)._size;
}

static void OTable_CopyObject (OTable_Object _source, OTable_Object _dest) {
  DEREF(OTable_Object, _dest, 20820)._mark = DEREF(OTable_Object, _source, 20834)._mark;
  DEREF(OTable_Object, _dest, 20849)._mode = DEREF(OTable_Object, _source, 20863)._mode;
  DEREF(OTable_Object, _dest, 20878)._pos = DEREF(OTable_Object, _source, 20892)._pos;
  DEREF(OTable_Object, _dest, 20906)._link = DEREF(OTable_Object, _source, 20920)._link;
  OTable_CopyStruct(DEREF(OTable_Object, _source, 20948)._type, DEREF(OTable_Object, _dest, 20958)._type);
}

static void OTable_InsertInTree (OTable_Object (* _root), OTable_Object _obj) {
  if ((void*)(* _root)==(void*)NULL)  {
    (* _root) = _obj;
  } else {
    if (STRLSS(DEREF(OTable_Object, _obj, 21409)._name, DEREF(OTable_Object, (* _root), 21421)._name))  {
      OTable_InsertInTree(&(DEREF(OTable_Object, (* _root), 21458)._left), _obj);
    } else if (STRGRT(DEREF(OTable_Object, _obj, 21486)._name, DEREF(OTable_Object, (* _root), 21498)._name))  {
      OTable_InsertInTree(&(DEREF(OTable_Object, (* _root), 21535)._right), _obj);
    }
  }
}

OTable_Object OTable_SearchInTree (OTable_Object _root, LONGINT __ident_0, const CHAR (* __ident_p)) {
  CHAR (* _ident);
  VALUE_ARRAY(_ident, __ident_p, 1*__ident_0);
  if ((void*)_root==(void*)NULL)  {
    return NULL;
  } else if (STRLSS(_ident, DEREF(OTable_Object, _root, 21914)._name))  {
    return OTable_SearchInTree(DEREF(OTable_Object, _root, 21957)._left, __ident_0, _ident);
  } else if (STRGRT(_ident, DEREF(OTable_Object, _root, 21994)._name))  {
    return OTable_SearchInTree(DEREF(OTable_Object, _root, 22037)._right, __ident_0, _ident);
  } else {
    return _root;
  }
  NO_RETURN (21599);
}

void OTable_OptimizeTree (OTable_Object (* _root)) {

  OTable_Object _LinearizeTree (OTable_Object _root) {
    OTable_Object _newroot, _tmproot;
    _newroot = _root;
    if ((void*)_root!=(void*)NULL)  {
      DEREF(OTable_Object, _root, 23038)._right = _LinearizeTree(DEREF(OTable_Object, _root, 23064)._right);
      DEREF(OTable_Object, _root, 23118)._left = _LinearizeTree(DEREF(OTable_Object, _root, 23144)._left);
      if ((void*)DEREF(OTable_Object, _root, 23201)._left!=(void*)NULL)  {
        _newroot = DEREF(OTable_Object, _root, 23282)._left;
        DEREF(OTable_Object, _root, 23339)._left = (OTable_Object)NULL;
        _tmproot = _newroot;
        while ((void*)DEREF(OTable_Object, _tmproot, 23562)._right!=(void*)NULL)  {
          _tmproot = DEREF(OTable_Object, _tmproot, 23607)._right;
        }
        DEREF(OTable_Object, _tmproot, 23647)._right = _root;
      }
    }
    return _newroot;
    NO_RETURN (22665);
  }

  OTable_Object _RebuildTree (OTable_Object _root) {
    OTable_Object _newroot, _left, _right;

    void _SplitList (OTable_Object _list, OTable_Object (* _left), OTable_Object (* _root), OTable_Object (* _right)) {
      OTable_Object _walk;
      (* _left) = _list;
      (* _root) = _list;
      (* _right) = _list;
      _walk = _list;
      while ((void*)_walk!=(void*)NULL)  {
        _walk = DEREF(OTable_Object, _walk, 24481)._right;
        if ((void*)_walk!=(void*)NULL)  {
          _walk = DEREF(OTable_Object, _walk, 24544)._right;
          (* _root) = (* _right);
          (* _right) = DEREF(OTable_Object, (* _right), 24577)._right;
        }
      }
      if ((void*)(* _right)!=(void*)NULL)  {
        DEREF(OTable_Object, (* _root), 25038)._right = (OTable_Object)NULL;
        (* _root) = (* _right);
        (* _right) = DEREF(OTable_Object, (* _right), 25087)._right;
        DEREF(OTable_Object, (* _root), 25099)._right = (OTable_Object)NULL;
      }
    }
    _newroot = (OTable_Object)NULL;
    if ((void*)_root!=(void*)NULL&&(void*)DEREF(OTable_Object, _root, 25208)._right!=(void*)NULL)  {
      _SplitList(_root, &(_left), &(_newroot), &(_right));
      if ((void*)_newroot!=(void*)NULL)  {
        DEREF(OTable_Object, _newroot, 25320)._left = _RebuildTree(_left);
        DEREF(OTable_Object, _newroot, 25364)._right = _RebuildTree(_right);
      } else {
        _newroot = _root;
      }
    } else {
      _newroot = _root;
    }
    return _newroot;
    NO_RETURN (23744);
  }
  if ((void*)(* _root)!=(void*)NULL)  {
    (* _root) = _LinearizeTree((* _root));
    (* _root) = _RebuildTree((* _root));
  }
}

BOOLEAN OTable_SameType (OTable_Struct _a, OTable_Struct _b) {
  return _a==_b&&DEREF(OTable_Struct, _a, 26049)._form!=OTable_strDynArray;
  NO_RETURN (25947);
}

BOOLEAN OTable_EqualType (OTable_Struct _a, OTable_Struct _b) {
  return (OTable_SameType(_a, _b)||((DEREF(OTable_Struct, _a, 26217)._form==OTable_strDynArray&&DEREF(OTable_Struct, _b, 26242)._form==OTable_strDynArray)&&OTable_EqualType(DEREF(OTable_Struct, _a, 26285)._base, DEREF(OTable_Struct, _b, 26292)._base)))||((DEREF(OTable_Struct, _a, 26312)._form==OTable_strProc&&DEREF(OTable_Struct, _b, 26333)._form==OTable_strProc)&&OTable_ParamsMatch(_a, _b));
  NO_RETURN (26099);
}

BOOLEAN OTable_ExtOf (OTable_Struct _a, OTable_Struct _b) {
  if (DEREF(OTable_Struct, _a, 26480)._form==OTable_strPointer&&DEREF(OTable_Struct, _b, 26504)._form==OTable_strPointer)  {
    _a = DEREF(OTable_Struct, _a, 26539)._base;
    _b = DEREF(OTable_Struct, _b, 26550)._base;
  } else if ((DEREF(OTable_Struct, _a, 26569)._form==OTable_strPointer)!=(DEREF(OTable_Struct, _b, 26593)._form==OTable_strPointer))  {
    return FALSE;
  }
  if (DEREF(OTable_Struct, _a, 26718)._form==OTable_strRecord&&DEREF(OTable_Struct, _b, 26741)._form==OTable_strRecord)  {
    while ((void*)_a!=(void*)NULL&&_a!=INDEX(OTable_predeclType, OTable_strUndef, 14, 26805))  {
      if (OTable_SameType(_a, _b))  {
        return TRUE;
      } else {
        _a = DEREF(OTable_Struct, _a, 27023)._base;
      }
    }
  }
  return FALSE;
  NO_RETURN (26399);
}

BOOLEAN OTable_ParamsMatch (OTable_Struct _str1, OTable_Struct _str2) {
  OTable_Object _par1, _par2;
  BOOLEAN _specType;
  if (OTable_SameType(DEREF(OTable_Struct, _str1, 27333)._base, DEREF(OTable_Struct, _str2, 27343)._base))  {
    _specType = FALSE;
    _par1 = DEREF(OTable_Struct, _str1, 27516)._link;
    _par2 = DEREF(OTable_Struct, _str2, 27533)._link;
    while(1) {
      if ((void*)_par1!=(void*)NULL&&(void*)_par2!=(void*)NULL)  {
        _specType = (_specType||((DEREF(OTable_Object, _par1, 27644)._mode==OTable_objVarPar&&DEREF(OTable_Struct, DEREF(OTable_Object, _par1, 27670)._type, 27675)._form==OTable_strRecord)&&!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _par1, 27733)._type, 27738)._flags, 27726)))||DEREF(OTable_Struct, DEREF(OTable_Object, _par1, 27766)._type, 27771)._form==OTable_strDynArray;
        if (DEREF(OTable_Object, _par1, 27811)._mode==DEREF(OTable_Object, _par2, 27823)._mode&&OTable_EqualType(DEREF(OTable_Object, _par1, 27846)._type, DEREF(OTable_Object, _par2, 27856)._type))  {
          _par1 = DEREF(OTable_Object, _par1, 27890)._link;
          _par2 = DEREF(OTable_Object, _par2, 27907)._link;
        } else {
          return FALSE;
        }
      } else {
        return ((void*)_par1==(void*)NULL&&(void*)_par2==(void*)NULL)&&(!_specType||IN(OTable_flagExternal, DEREF(OTable_Struct, _str1, 28196)._flags, 28189)==IN(OTable_flagExternal, DEREF(OTable_Struct, _str2, 28227)._flags, 28220));
      }
    }
  }
  return FALSE;
  NO_RETURN (27106);
}

static BOOLEAN OTable_CheckParameter (OTable_Object _par1, OTable_Object _par2, BOOLEAN _quiet) {
  if (DEREF(OTable_Object, _par1, 28400)._mode!=DEREF(OTable_Object, _par2, 28412)._mode)  {
    if (!_quiet)  {
      OScan_Err(DEREF(OTable_Object, _par2, 28487)._pos, 69);
    }
    return FALSE;
  } else if (!OTable_EqualType(DEREF(OTable_Object, _par1, 28548)._type, DEREF(OTable_Object, _par2, 28558)._type))  {
    if (!_quiet)  {
      OScan_Err(DEREF(OTable_Object, _par2, 28628)._pos, 67);
    }
    return FALSE;
  } else {
    return TRUE;
  }
  NO_RETURN (28320);
}

static BOOLEAN OTable_SameResult (OTable_Object _func1, OTable_Object _func2, BOOLEAN _quiet) {
  if (!OTable_SameType(DEREF(OTable_Struct, DEREF(OTable_Object, _func1, 28823)._type, 28828)._base, DEREF(OTable_Struct, DEREF(OTable_Object, _func2, 28839)._type, 28844)._base))  {
    if (!_quiet)  {
      OScan_Err(DEREF(OTable_Object, _func2, 28925)._pos, 70);
    }
    return FALSE;
  } else {
    return TRUE;
  }
  NO_RETURN (28735);
}

static BOOLEAN OTable_FormalParMatch (OTable_Object _proc1, OTable_Object _proc2) {
  BOOLEAN _res;
  OTable_Object _par1, _par2;
  _res = OTable_SameResult(_proc1, _proc2, FALSE);
  _par1 = DEREF(OTable_Struct, DEREF(OTable_Object, _proc1, 29185)._type, 29190)._link;
  _par2 = DEREF(OTable_Struct, DEREF(OTable_Object, _proc2, 29208)._type, 29213)._link;
  while(1) {
    if ((void*)_par1==(void*)NULL&&(void*)_par2==(void*)NULL)  {
      goto _exit29261;
    } else if (((void*)_par1==(void*)NULL)!=((void*)_par2==(void*)NULL))  {
      OScan_Err(DEREF(OTable_Object, _proc2, 29512)._pos, 71);
      return FALSE;
    }
    _res = OTable_CheckParameter(_par1, _par2, FALSE)&&_res;
    _par1 = DEREF(OTable_Object, _par1, 29622)._link;
    _par2 = DEREF(OTable_Object, _par2, 29639)._link;
  }
  _exit29261:
  return _res;
  NO_RETURN (29028);
}

static BOOLEAN OTable_FormalParMatchTB (OTable_Object _proc1, OTable_Object _proc2) {
  BOOLEAN _res;
  OTable_Object _par1, _par2;
  BOOLEAN _quiet;

  BOOLEAN _CheckReceiver (OTable_Object _par1, OTable_Object _par2, BOOLEAN _quiet) {
    if (DEREF(OTable_Object, _par1, 29916)._mode!=DEREF(OTable_Object, _par2, 29928)._mode)  {
      return FALSE;
    } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _par1, 29980)._type, 29985)._form!=DEREF(OTable_Struct, DEREF(OTable_Object, _par2, 29997)._type, 30002)._form)  {
      return FALSE;
    } else {
      if (OTable_ExtOf(DEREF(OTable_Object, _par2, 30069)._type, DEREF(OTable_Object, _par1, 30079)._type)||OTable_EqualType(DEREF(OTable_Object, _par2, 30103)._type, DEREF(OTable_Object, _par1, 30113)._type))  {
        return TRUE;
      } else {
        if (!_quiet)  {
          OScan_Err(DEREF(OTable_Object, _par2, 30198)._pos, 84);
        }
        return FALSE;
      }
    }
    NO_RETURN (29833);
  }
  _quiet = FALSE;
  _res = OTable_SameResult(_proc1, _proc2, _quiet);
  _par1 = DEREF(OTable_Struct, DEREF(OTable_Object, _proc1, 30387)._type, 30392)._link;
  _par2 = DEREF(OTable_Struct, DEREF(OTable_Object, _proc2, 30410)._type, 30415)._link;
  _res = _CheckReceiver(_par1, _par2, _quiet)&&_res;
  _par1 = DEREF(OTable_Object, _par1, 30483)._link;
  _par2 = DEREF(OTable_Object, _par2, 30500)._link;
  while(1) {
    if ((void*)_par1==(void*)NULL&&(void*)_par2==(void*)NULL)  {
      goto _exit30549;
    } else if (((void*)_par1==(void*)NULL)!=((void*)_par2==(void*)NULL))  {
      if (!_quiet)  {
        OScan_Err(DEREF(OTable_Object, _proc2, 30802)._pos, 71);
      }
      return FALSE;
    }
    _res = OTable_CheckParameter(_par1, _par2, _quiet)&&_res;
    _par1 = DEREF(OTable_Object, _par1, 30917)._link;
    _par2 = DEREF(OTable_Object, _par2, 30934)._link;
  }
  _exit30549:
  return _res;
  NO_RETURN (29705);
}

BOOLEAN OTable_ContainsPointer (OTable_Struct _str) {

  BOOLEAN _RecordContainsPointer (OTable_Object _obj) {
    if ((void*)_obj!=(void*)NULL)  {
      return (OTable_ContainsPointer(DEREF(OTable_Object, _obj, 31258)._type)||_RecordContainsPointer(DEREF(OTable_Object, _obj, 31309)._left))||_RecordContainsPointer(DEREF(OTable_Object, _obj, 31360)._right);
    } else {
      return FALSE;
    }
    NO_RETURN (31143);
  }
  if ((void*)_str!=(void*)NULL)  {
    {
      SHORTINT _temp_ = DEREF(OTable_Struct, _str, 31492)._form;
      switch (_temp_) {
        case 13:
        case 14: {
          return TRUE;
          break;
        }
        case 17:
        case 18: {
          return OTable_ContainsPointer(DEREF(OTable_Struct, _str, 31616)._base);
          break;
        }
        case 19: {
          return OTable_ContainsPointer(DEREF(OTable_Struct, _str, 31676)._base)||_RecordContainsPointer(DEREF(OTable_Struct, _str, 31726)._link);
          break;
        }
        default:  {
        return FALSE;
      }
      }
    }
  } else {
    return FALSE;
  }
  NO_RETURN (31003);
}

static void OTable_InsertNextObject (OTable_Object _obj) {
  if (!IN(OTable_flagParam, DEREF(OTable_Object, _obj, 32241)._flags, 32235))  {
    if ((void*)DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32329)._topScope, 32338)._next!=(void*)NULL)  {
      DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32488)._topScope, 32497)._next, 32502)._next = _obj;
    } else if ((void*)DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32538)._topScope, 32547)._right==(void*)NULL)  {
      DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32636)._topScope, 32645)._right = _obj;
    } else {
      DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32782)._topScope, 32791)._next = DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32808)._topScope, 32817)._next;
      while ((void*)DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32849)._topScope, 32858)._next, 32863)._next!=(void*)NULL)  {
        DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32899)._topScope, 32908)._next = DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32925)._topScope, 32934)._next, 32939)._next;
      }
      DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 32989)._topScope, 32998)._next, 33003)._next = _obj;
    }
    DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 33051)._topScope, 33060)._next = _obj;
  }
}

static void OTable_UpdateForNewInsert (OTable_Object (* _obj)) {
  DEREF(OTable_Object, (* _obj), 33215)._scope = DEREF(OTable_ScopeStack, OTable_scopeStack, 33234)._topScope;
  DEREF(OTable_Object, (* _obj), 33252)._mnolev = DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 33271)._topScope, 33280)._mnolev;
  if (!IN(DEREF(OTable_Object, (* _obj), 33301)._mode, 0x00003400U, 33307)&&OTable_IsLocalObject((* _obj)))  {
    OTable_InsertNextObject((* _obj));
  }
  if (IN(DEREF(OTable_Object, (* _obj), 33437)._mode, 0x00000380U, 33443)&&OTable_IsLocalObject((* _obj)))  {
    DEREF(OTable_Object, DEREF(OTable_Object, (* _obj), 33581)._link, 33586)._mnolev = DEREF(OTable_Object, (* _obj), 33598)._mnolev+1;
  } else if (IN(DEREF(OTable_Object, (* _obj), 33625)._mode, 0x00000008U, 33631)&&(void*)DEREF(OTable_Struct, DEREF(OTable_Object, (* _obj), 33651)._type, 33656)._obj==(void*)NULL)  {
    DEREF(OTable_Struct, DEREF(OTable_Object, (* _obj), 33682)._type, 33687)._obj = (* _obj);
  }
}

void OTable_Insert (OTable_Object (* _obj)) {
  OTable_Object _old;
  _old = OTable_SearchInTree(DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 34068)._topScope, 34077)._link, 48, (CHAR *) DEREF(OTable_Object, (* _obj), 34086)._name);
  if ((void*)_old!=(void*)NULL)  {
    if (DEREF(OTable_Object, _old, 34188)._mode==OTable_objForwardType)  {
      if (DEREF(OTable_Object, (* _obj), 34232)._mode==OTable_objType)  {
        if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, (* _obj), 34271)._type, 34276)._form, 0x000E0000U, 34282))  {
          OTable_CopyObject((* _obj), _old);
          (* _obj) = _old;
          DEREF(OTable_Struct, DEREF(OTable_Object, (* _obj), 34394)._type, 34399)._obj = (* _obj);
          OTable_InsertNextObject((* _obj));
        } else {
          OScan_Err(DEREF(OTable_Object, (* _obj), 34592)._pos, 66);
        }
      } else {
        OScan_Err(DEREF(OTable_Object, (* _obj), 34699)._pos, 61);
      }
    } else if (DEREF(OTable_Object, _old, 34737)._mode==OTable_objForwardProc)  {
      if (IN(DEREF(OTable_Object, (* _obj), 34781)._mode, 0x00000380U, 34787))  {
        if (DEREF(OTable_Object, (* _obj), 34893)._mark!=DEREF(OTable_Object, _old, 34904)._mark)  {
          OScan_Err(DEREF(OTable_Object, (* _obj), 34938)._pos, 68);
        }
        if (OTable_FormalParMatch(_old, (* _obj)))  {
          OTable_CopyObject((* _obj), _old);
          (* _obj) = _old;
          if (OTable_IsLocalObject((* _obj)))  {
            DEREF(OTable_Object, DEREF(OTable_Object, (* _obj), 35279)._link, 35284)._mnolev = DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 35303)._topScope, 35312)._mnolev+1;
          } else {
            DEREF(OTable_Object, DEREF(OTable_Object, (* _obj), 35475)._link, 35480)._mnolev = DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 35499)._topScope, 35508)._mnolev;
          }
          OTable_InsertNextObject((* _obj));
        } else {
          OScan_Err(DEREF(OTable_Object, (* _obj), 35641)._pos, 62);
        }
      } else if (DEREF(OTable_Object, (* _obj), 35684)._mode==OTable_objForwardProc)  {
        if (OTable_FormalParMatch(_old, (* _obj)))  {
          OScan_Warn(DEREF(OTable_Object, (* _obj), 35883)._pos, 91);
          OScan_Warn(DEREF(OTable_Object, _old, 35916)._pos, 91);
          (* _obj) = _old;
        } else {
          OScan_Err(DEREF(OTable_Object, (* _obj), 36020)._pos, 62);
        }
      } else {
        OScan_Err(DEREF(OTable_Object, (* _obj), 36132)._pos, 72);
      }
    } else {
      OScan_ErrIns(DEREF(OTable_Object, (* _obj), 36214)._pos, 60, 48, (CHAR *) DEREF(OTable_Object, (* _obj), 36225)._name);
    }
  } else {
    OTable_UpdateForNewInsert(_obj);
    OTable_InsertInTree(&(DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 36313)._topScope, 36322)._link), (* _obj));
  }
}

void OTable_InsertForOSym (OTable_Object (* _obj), OTable_Object (* _last)) {
  OTable_Object _ptr, _lastLeft;
  OTable_UpdateForNewInsert(_obj);
  if ((void*)(* _last)==(void*)NULL)  {
    DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 36857)._topScope, 36866)._link = (* _obj);
    DEREF(OTable_Object, (* _obj), 36881)._sort = (OTable_Object)NULL;
  } else if (STRLSS(DEREF(OTable_Object, (* _last), 36907)._name, DEREF(OTable_Object, (* _obj), 36918)._name)&&((void*)DEREF(OTable_Object, (* _last), 36933)._sort==(void*)NULL||STRLSS(DEREF(OTable_Object, (* _obj), 36953)._name, DEREF(OTable_Object, DEREF(OTable_Object, (* _last), 36965)._sort, 36970)._name)))  {
    DEREF(OTable_Object, (* _last), 37138)._right = (* _obj);
    DEREF(OTable_Object, (* _obj), 37154)._sort = DEREF(OTable_Object, (* _last), 37165)._sort;
  } else {
    OTable_InsertInTree(&(DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 37265)._topScope, 37274)._link), (* _obj));
    _lastLeft = (OTable_Object)NULL;
    _ptr = DEREF(OTable_Object, DEREF(OTable_ScopeStack, OTable_scopeStack, 37393)._topScope, 37402)._link;
    while(1) {
      if (STRLEQ(DEREF(OTable_Object, _ptr, 37435)._name, DEREF(OTable_Object, (* _obj), 37448)._name))  {
        _ptr = DEREF(OTable_Object, _ptr, 37479)._right;
        if ((void*)_ptr==(void*)NULL)  {
          _ptr = _lastLeft;
          goto _exit37415;
        }
      } else {
        if ((void*)DEREF(OTable_Object, _ptr, 37573)._left==(void*)NULL||DEREF(OTable_Object, _ptr, 37593)._left==(* _obj))  {
          goto _exit37415;
        } else {
          _lastLeft = _ptr;
          _ptr = DEREF(OTable_Object, _ptr, 37655)._left;
        }
      }
    }
    _exit37415:
    DEREF(OTable_Object, (* _obj), 37710)._sort = _ptr;
  }
  (* _last) = (* _obj);
}

void OTable_InsertParams (OTable_Object _proc) {
  OTable_Object _param;
  OTable_OpenScope(DEREF(OTable_Object, _proc, 37970)._link);
  _param = DEREF(OTable_Struct, DEREF(OTable_Object, _proc, 37993)._type, 37998)._link;
  if (DEREF(OTable_Struct, DEREF(OTable_Object, _proc, 38017)._type, 38022)._form==OTable_strTBProc)  {
    INCL(DEREF(OTable_Object, _param, 38056)._flags, OTable_flagReceiver, 38046);
  }
  while ((void*)_param!=(void*)NULL)  {
    INCL(DEREF(OTable_Object, _param, 38126)._flags, OTable_flagParam, 38116);
    OTable_Insert(&(_param));
    _param = DEREF(OTable_Object, _param, 38178)._link;
  }
  OTable_CloseScope(FALSE);
}

OTable_Object OTable_Find (LONGINT __ident_0, const CHAR (* __ident_p)) {
  CHAR (* _ident);
  OTable_Object _walk, _scope;
  VALUE_ARRAY(_ident, __ident_p, 1*__ident_0);
  _scope = DEREF(OTable_ScopeStack, OTable_scopeStack, 38344)._topScope;
  while ((void*)_scope!=(void*)NULL)  {
    _walk = OTable_SearchInTree(DEREF(OTable_Object, _scope, 38412)._link, __ident_0, _ident);
    if ((void*)_walk!=(void*)NULL)  {
      INCL(DEREF(OTable_Object, _walk, 38470)._flags, OTable_flagUsed, 38461);
      return _walk;
    }
    _scope = DEREF(OTable_Object, _scope, 38548)._scope;
  }
  return NULL;
  NO_RETURN (38248);
}

static OTable_Object OTable_InternalFindField (LONGINT __ident_0, const CHAR (* __ident_p), OTable_Struct _struct) {
  CHAR (* _ident);
  OTable_Object _walk;
  VALUE_ARRAY(_ident, __ident_p, 1*__ident_0);
  while(1) {
    if ((void*)_struct==(void*)NULL)  {
      return NULL;
    } else {
      _walk = OTable_SearchInTree(DEREF(OTable_Struct, _struct, 39037)._link, __ident_0, _ident);
      if ((void*)_walk!=(void*)NULL)  {
        if (DEREF(OTable_Object, _walk, 39098)._mark!=OTable_exportNot||OTable_IsLocalObject(_walk))  {
          return _walk;
        } else {
          return NULL;
        }
      } else {
        _struct = DEREF(OTable_Struct, _struct, 39335)._base;
      }
    }
  }
  NO_RETURN (38812);
}

OTable_Object OTable_FindField (LONGINT __ident_0, const CHAR (* __ident_p), OTable_Struct _struct) {
  CHAR (* _ident);
  OTable_Object _field;
  VALUE_ARRAY(_ident, __ident_p, 1*__ident_0);
  _field = OTable_InternalFindField(__ident_0, _ident, _struct);
  if ((void*)_field==(void*)NULL||DEREF(OTable_Object, _field, 39577)._mode==OTable_objProtoTBProc)  {
    return NULL;
  } else {
    INCL(DEREF(OTable_Object, _field, 39650)._flags, OTable_flagUsed, 39640);
    return _field;
  }
  NO_RETURN (39412);
}

static void OTable_InsertProtoTBs (OTable_Struct _dest, OTable_Object _orig) {
  OTable_Object _virt;
  if ((void*)_dest!=(void*)NULL)  {
    _virt = OTable_SearchInTree(DEREF(OTable_Struct, _dest, 39856)._link, 48, (CHAR *) DEREF(OTable_Object, _orig, 39866)._name);
    if ((void*)_virt==(void*)NULL)  {
      _virt = OTable_NewObject(48, (CHAR *) DEREF(OTable_Object, _orig, 39960)._name, OTable_objProtoTBProc, -1);
      OTable_InsertField(_virt, _dest);
    }
    OTable_InsertProtoTBs(DEREF(OTable_Struct, _dest, 40066)._base, _orig);
  }
}

static BOOLEAN OTable_CheckTBExportSemantics (OTable_Object _sub, OTable_Object _super) {
  OTable_Object _base;
  _base = DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OTable_Object, _sub, 40318)._type, 40323)._link, 40328)._type, 40333)._obj;
  if ((void*)_super!=(void*)NULL)  {
    if (DEREF(OTable_Object, _super, 40467)._mark==OTable_exportWrite)  {
      if (DEREF(OTable_Object, _base, 40510)._mark==OTable_exportWrite)  {
        if (DEREF(OTable_Object, _sub, 40619)._mark!=OTable_exportWrite)  {
          OScan_Err(DEREF(OTable_Object, _sub, 40667)._pos, 86);
        }
        return DEREF(OTable_Object, _sub, 40806)._mark==OTable_exportWrite;
      }
    }
  }
  return TRUE;
  NO_RETURN (40121);
}

void OTable_InsertField (OTable_Object _obj, OTable_Struct _struct) {
  OTable_Object _old, _local, _super;

  void _InsertFieldNext (void) {
    OTable_Object _walk;
    _walk = DEREF(OTable_Struct, _struct, 41115)._link;
    if (_walk!=_obj)  {
      if ((void*)_walk!=(void*)NULL)  {
        while ((void*)DEREF(OTable_Object, _walk, 41270)._next!=(void*)NULL)  {
          _walk = DEREF(OTable_Object, _walk, 41308)._next;
        }
        DEREF(OTable_Object, _walk, 41344)._next = _obj;
      } else {
        DEREF(OTable_Struct, _struct, 41385)._link = _walk;
      }
    }
  }
  if (IN(DEREF(OTable_Object, _obj, 41471)._mode, 0x00000800U, 41477))  {
    if (OTable_IsLocalObject(_obj))  {
      DEREF(OTable_Object, DEREF(OTable_Object, _obj, 41542)._link, 41547)._mnolev = DEREF(OTable_Object, _obj, 41559)._mnolev+1;
    } else {
      DEREF(OTable_Object, DEREF(OTable_Object, _obj, 41594)._link, 41599)._mnolev = DEREF(OTable_Object, _obj, 41611)._mnolev;
    }
  }
  _old = OTable_InternalFindField(48, (CHAR *) DEREF(OTable_Object, _obj, 41670)._name, _struct);
  if ((void*)_old!=(void*)NULL)  {
    if (IN(DEREF(OTable_Object, _old, 41722)._mode, OTable_methodObjects, 41728))  {
      _local = OTable_SearchInTree(DEREF(OTable_Struct, _struct, 41785)._link, 48, (CHAR *) DEREF(OTable_Object, _obj, 41794)._name);
      if ((void*)_local!=(void*)NULL)  {
        if (DEREF(OTable_Object, _local, 41851)._mode==OTable_objForwardTBProc)  {
          if (OTable_FormalParMatchTB(_local, _obj)&&OTable_CheckTBExportSemantics(_local, _obj))  {
            OTable_CopyObject(_obj, _local);
          } else {
            OScan_Err(DEREF(OTable_Object, _obj, 42044)._pos, 88);
          }
        } else if (DEREF(OTable_Object, _local, 42093)._mode==OTable_objProtoTBProc)  {
          OScan_Err(DEREF(OTable_Object, _obj, 42144)._pos, 87);
        } else {
          OScan_Err(DEREF(OTable_Object, _obj, 42191)._pos, 64);
        }
      } else {
        _super = _old;
        if (DEREF(OTable_Object, _super, 42270)._mode==OTable_objProtoTBProc)  {
          OTable_InsertInTree(&(DEREF(OTable_Struct, _struct, 42330)._link), _obj);
          OTable_InsertProtoTBs(DEREF(OTable_Struct, _struct, 42375)._base, _obj);
        } else if (OTable_FormalParMatchTB(_super, _obj))  {
          if (OTable_CheckTBExportSemantics(_obj, _super))  {
            OTable_InsertInTree(&(DEREF(OTable_Struct, _struct, 42523)._link), _obj);
          }
        } else {
          OScan_Err(DEREF(OTable_Object, _obj, 42589)._pos, 89);
        }
      }
    } else {
      OScan_Err(DEREF(OTable_Object, _obj, 42656)._pos, 64);
    }
  } else {
    OTable_InsertInTree(&(DEREF(OTable_Struct, _struct, 42711)._link), _obj);
    if (IN(DEREF(OTable_Object, _obj, 42736)._mode, OTable_methodObjects, 42742))  {
      OTable_InsertProtoTBs(DEREF(OTable_Struct, _struct, 42794)._base, _obj);
    }
  }
  if (DEREF(OTable_Object, _obj, 42837)._mode==OTable_objField)  {
    _InsertFieldNext();
  }
}

void OTable_ImportInsertField (OTable_Object _obj, OTable_Struct _struct) {
  if (STRNEQ(DEREF(OTable_Object, _obj, 42990)._name, "@"))  {
    OTable_InsertInTree(&(DEREF(OTable_Struct, _struct, 43037)._link), _obj);
  }
}

OTable_Object OTable_InternalFindImport (LONGINT __ident_0, const CHAR (* __ident_p), OTable_Object _mod) {
  CHAR (* _ident);
  VALUE_ARRAY(_ident, __ident_p, 1*__ident_0);
  return OTable_SearchInTree(DEREF(OTable_Object, DEREF(OTable_Object, _mod, 43537)._link, 43542)._link, __ident_0, _ident);
  NO_RETURN (43301);
}

OTable_Object OTable_FindImport (LONGINT __ident_0, const CHAR (* __ident_p), OTable_Object _mod) {
  CHAR (* _ident);
  OTable_Object _obj;
  VALUE_ARRAY(_ident, __ident_p, 1*__ident_0);
  INCL(DEREF(OTable_Object, _mod, 43693)._flags, OTable_flagUsed, 43685);
  _obj = OTable_InternalFindImport(__ident_0, _ident, _mod);
  if ((void*)_obj!=(void*)NULL&&DEREF(OTable_Object, _obj, 43776)._mark==OTable_exportNot)  {
    return NULL;
  } else {
    return _obj;
  }
  NO_RETURN (43594);
}

OTable_Object OTable_GetModuleForObject (OTable_Object _obj) {
  OTable_Object _walk;
  _walk = _obj;
  while ((void*)_walk!=(void*)NULL&&DEREF(OTable_Object, _walk, 44006)._mode!=OTable_objModule)  {
    if (DEREF(OTable_Object, _walk, 44042)._mode==OTable_objField)  {
      return NULL;
    } else {
      _walk = DEREF(OTable_Object, DEREF(OTable_Object, _walk, 44115)._scope, 44121)._left;
    }
  }
  if ((void*)_walk!=(void*)NULL)  {
    return DEREF(OTable_Object, DEREF(OTable_Object, _walk, 44191)._link, 44196)._left;
  } else {
    return OTable_compiledModule;
  }
  NO_RETURN (43886);
}

void OTable_GetModuleName (OTable_Object _obj, LONGINT __name_0, CHAR (* _name)) {
  OTable_Object _mod;
  _mod = OTable_GetModuleForObject(_obj);
  COPY(DEREF(OTable_Object, _mod, 44510)._name, _name, __name_0);
}

static void OTable_InsertPredeclStruct (LONGINT __name_0, const CHAR (* __name_p), SHORTINT _str) {
  CHAR (* _name);
  OTable_Object _obj;
  OTable_Struct _type;
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  _obj = OTable_NewObject(__name_0, _name, OTable_objType, -1);
  _type = OTable_NewStruct(_str);
  DEREF(OTable_Object, _obj, 44960)._mark = OTable_exportWrite;
  DEREF(OTable_Object, _obj, 44983)._type = _type;
  DEREF(OTable_Struct, _type, 45000)._obj = _obj;
  OTable_Insert(&(_obj));
  INCL(DEREF(OTable_Object, _obj, 45036)._flags, OTable_flagUsed, 45028);
  INDEX(OTable_predeclType, _str, 14, 45069) = _type;
  if (((OTable_strBool<=_str&&_str<=OTable_strSet)||_str==OTable_strSysByte)||_str==OTable_strSysPtr)  {
    DEREF(OTable_Struct, _type, 45182)._size = OTable_structSize(_type);
  }
}

static void OTable_InsertProc (LONGINT __name_0, const CHAR (* __name_p), LONGINT _num) {
  CHAR (* _name);
  OTable_Object _obj;
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  _obj = OTable_NewObject(__name_0, _name, OTable_objExtProc, -1);
  DEREF(OTable_Object, _obj, 45378)._const = OTable_NewConst();
  DEREF(OTable_Const, DEREF(OTable_Object, _obj, 45406)._const, 45412)._intval = _num;
  DEREF(OTable_Object, _obj, 45429)._mark = OTable_exportWrite;
  DEREF(OTable_Object, _obj, 45452)._type = OTable_NewStruct(OTable_strProc);
  OTable_Insert(&(_obj));
  INCL(DEREF(OTable_Object, _obj, 45503)._flags, OTable_flagUsed, 45495);
}

static void OTable_InitPredefined (void) {
  OTable_Object _new;
  OTable_predefined = OTable_NewObject(4, (CHAR *) OScan_undefStr, OTable_objScope, -1);
  DEREF(OTable_Object, OTable_predefined, 45706)._mnolev = -1;
  OTable_OpenScope(OTable_predefined);
  OTable_InsertPredeclStruct(8, (CHAR *) "BOOLEAN", OTable_strBool);
  OTable_InsertPredeclStruct(5, (CHAR *) "CHAR", OTable_strChar);
  OTable_InsertPredeclStruct(8, (CHAR *) "INTEGER", OTable_strInteger);
  OTable_InsertPredeclStruct(8, (CHAR *) "LONGINT", OTable_strLongInt);
  OTable_InsertPredeclStruct(9, (CHAR *) "LONGREAL", OTable_strLongReal);
  OTable_InsertPredeclStruct(4, (CHAR *) "NIL", OTable_strNil);
  OTable_InsertPredeclStruct(5, (CHAR *) "REAL", OTable_strReal);
  OTable_InsertPredeclStruct(4, (CHAR *) "SET", OTable_strSet);
  OTable_InsertPredeclStruct(9, (CHAR *) "SHORTINT", OTable_strShortInt);
  OTable_InsertProc(4, (CHAR *) "ABS", 21);
  OTable_InsertProc(4, (CHAR *) "ASH", 26);
  OTable_InsertProc(7, (CHAR *) "ASSERT", 46);
  OTable_InsertProc(4, (CHAR *) "CAP", 22);
  OTable_InsertProc(4, (CHAR *) "CHR", 13);
  OTable_InsertProc(5, (CHAR *) "COPY", 36);
  OTable_InsertProc(4, (CHAR *) "DEC", 35);
  OTable_InsertProc(7, (CHAR *) "ENTIER", 14);
  OTable_InsertProc(5, (CHAR *) "EXCL", 33);
  OTable_InsertProc(5, (CHAR *) "HALT", 45);
  OTable_InsertProc(4, (CHAR *) "INC", 34);
  OTable_InsertProc(5, (CHAR *) "INCL", 32);
  OTable_InsertProc(4, (CHAR *) "LEN", 30);
  OTable_InsertProc(5, (CHAR *) "LONG", 15);
  OTable_InsertProc(4, (CHAR *) "MAX", 18);
  OTable_InsertProc(4, (CHAR *) "MIN", 19);
  OTable_InsertProc(4, (CHAR *) "NEW", 43);
  OTable_InsertProc(4, (CHAR *) "ODD", 23);
  OTable_InsertProc(4, (CHAR *) "ORD", 16);
  OTable_InsertProc(6, (CHAR *) "SHORT", 17);
  OTable_InsertProc(5, (CHAR *) "SIZE", 20);
  _new = OTable_NewObject(6, (CHAR *) "FALSE", OTable_objConst, -1);
  DEREF(OTable_Object, _new, 47013)._const = OTable_NewConst();
  DEREF(OTable_Const, DEREF(OTable_Object, _new, 47036)._const, 47042)._intval = 0;
  DEREF(OTable_Object, _new, 47057)._type = INDEX(OTable_predeclType, OTable_strBool, 14, 47075);
  OTable_Insert(&(_new));
  _new = OTable_NewObject(5, (CHAR *) "TRUE", OTable_objConst, -1);
  DEREF(OTable_Object, _new, 47155)._const = OTable_NewConst();
  DEREF(OTable_Const, DEREF(OTable_Object, _new, 47178)._const, 47184)._intval = 1;
  DEREF(OTable_Object, _new, 47199)._type = INDEX(OTable_predeclType, OTable_strBool, 14, 47217);
  OTable_Insert(&(_new));
  INDEX(OTable_predeclType, OTable_strUndef, 14, 47338) = OTable_NewStruct(OTable_strUndef);
  DEREF(OTable_Struct, INDEX(OTable_predeclType, OTable_strUndef, 14, 47386), 47396)._base = INDEX(OTable_predeclType, OTable_strUndef, 14, 47414);
  DEREF(OTable_Struct, INDEX(OTable_predeclType, OTable_strUndef, 14, 47441), 47451)._size = 1;
  INDEX(OTable_predeclType, OTable_strString, 14, 47476) = OTable_NewStruct(OTable_strString);
  INDEX(OTable_predeclType, OTable_strNone, 14, 47527) = OTable_NewStruct(OTable_strNone);
  OTable_OptimizeTree(&(DEREF(OTable_Object, OTable_predefined, 47590)._link));
}

static void OTable_InitSYSTEM (void) {
  OTable_Object _new;
  OTable_system = OTable_NewObject(7, (CHAR *) OTable_systemIdent, OTable_objModule, -1);
  DEREF(OTable_Const, DEREF(OTable_Object, OTable_system, 47826)._const, 47832)._intval = OTable_systemKey;
  DEREF(OTable_Object, OTable_system, 47858)._mnolev = -2;
  DEREF(OTable_Object, DEREF(OTable_Object, OTable_system, 47892)._link, 47897)._mnolev = -2;
  OTable_OpenScope(DEREF(OTable_Object, OTable_system, 47940)._link);
  OTable_InsertPredeclStruct(5, (CHAR *) "BYTE", OTable_strSysByte);
  OTable_InsertPredeclStruct(4, (CHAR *) "PTR", OTable_strSysPtr);
  OTable_InsertProc(4, (CHAR *) "ADR", 24);
  OTable_InsertProc(4, (CHAR *) "BIT", 27);
  OTable_InsertProc(3, (CHAR *) "CC", 25);
  OTable_InsertProc(8, (CHAR *) "DISPOSE", 47);
  OTable_InsertProc(4, (CHAR *) "GET", 38);
  OTable_InsertProc(7, (CHAR *) "GETREG", 40);
  OTable_InsertProc(4, (CHAR *) "LSH", 28);
  OTable_InsertProc(5, (CHAR *) "MOVE", 37);
  OTable_InsertProc(4, (CHAR *) "NEW", 42);
  OTable_InsertProc(4, (CHAR *) "PUT", 39);
  OTable_InsertProc(7, (CHAR *) "PUTREG", 41);
  OTable_InsertProc(4, (CHAR *) "ROT", 29);
  OTable_InsertProc(4, (CHAR *) "VAL", 31);
  OTable_InsertProc(8, (CHAR *) "COLLECT", 48);
  _new = OTable_NewObject(3, (CHAR *) "GC", OTable_objConst, -1);
  DEREF(OTable_Object, _new, 48511)._const = OTable_NewConst();
  DEREF(OTable_Object, _new, 48534)._type = INDEX(OTable_predeclType, OTable_strBool, 14, 48552);
  DEREF(OTable_Object, _new, 48570)._mark = OTable_exportWrite;
  if (OMachine_generateCodeForGC)  {
    DEREF(OTable_Const, DEREF(OTable_Object, _new, 48632)._const, 48638)._intval = 1;
  } else {
    DEREF(OTable_Const, DEREF(OTable_Object, _new, 48669)._const, 48675)._intval = 0;
  }
  OTable_Insert(&(_new));
  OTable_OptimizeTree(&(DEREF(OTable_Object, DEREF(OTable_Object, OTable_system, 48738)._link, 48743)._link));
  OTable_CloseScope(TRUE);
}

void OTable_InitTable (void) {
  OTable_compiledModule = (OTable_Object)NULL;
  OTable_external = FALSE;
  OTable_InitPredefined();
  OTable_InitSYSTEM();
  OTable_InitSymbolCache();
}

static LONGINT OTable_DummyStructSize (OTable_Struct _t) {
  return (-9223372036854775807L-1);
  NO_RETURN (48953);
}

void _init_OTable (void) {
  moduleId = add_module ("OTable");
  td_OTable_ObjectDesc = create_td("ObjectDesc", sizeof(OTable_ObjectDesc), sizeof(TD_OTable_ObjectDesc), NULL, 0);
  td_OTable_StructDesc = create_td("StructDesc", sizeof(OTable_StructDesc), sizeof(TD_OTable_StructDesc), NULL, 0);
  td_OTable_ConstDesc = create_td("ConstDesc", sizeof(OTable_ConstDesc), sizeof(TD_OTable_ConstDesc), NULL, 0);
  td_OTable_ScopeStackDesc = create_td("ScopeStackDesc", sizeof(OTable_ScopeStackDesc), sizeof(TD_OTable_ScopeStackDesc), NULL, 0);
  td_OTable_SymbolTableCacheDesc = create_td("SymbolTableCacheDesc", sizeof(OTable_SymbolTableCacheDesc), sizeof(TD_OTable_SymbolTableCacheDesc), NULL, 0);
  td_OTable_SymbolTableImportedDesc = create_td("SymbolTableImportedDesc", sizeof(OTable_SymbolTableImportedDesc), sizeof(TD_OTable_SymbolTableImportedDesc), NULL, 0);
  OTable_collectedObjs = (OTable_Object)NULL;
  OTable_structSize = OTable_DummyStructSize;
  OTable_InitScopeStack();
}
