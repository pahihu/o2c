#ifndef __ODepend_
#define __ODepend_

#ifdef MODULE_ODepend
#define EXTERN_ODepend
#else
#define EXTERN_ODepend extern
#endif

#include "_OGCC.h"
#include "Unix.h"


struct TD_ODepend_ModuleDesc;
struct TD_ODepend_ImportDesc;

typedef struct ODepend_ModuleDesc (* ODepend_Module);
typedef struct ODepend_ImportDesc (* ODepend_Import);
typedef struct ODepend_ModuleDesc {
  ODepend_Module _next;
  CHAR _name[98];
  CHAR _extName[256];
  SHORTINT _extClass;
  ODepend_Import _import;
  SET _flags;
  CHAR _file[5][256];
  struct Unix_timeval _time[5];
  INTEGER _count;
} ODepend_ModuleDesc;
typedef struct ODepend_ImportDesc {
  ODepend_Import _next;
  ODepend_Module _module;
} ODepend_ImportDesc;
extern const SHORTINT ODepend_flModExists;
extern const SHORTINT ODepend_flSymExists;
extern const SHORTINT ODepend_flBE0Exists;
extern const SHORTINT ODepend_flBE1Exists;
extern const SHORTINT ODepend_flBE2Exists;
extern const SHORTINT ODepend_flSymChanged;
extern const SHORTINT ODepend_flExternal;
extern const SHORTINT ODepend_flNoSources;
extern const SHORTINT ODepend_flCompile;
extern const SHORTINT ODepend_flFirstFree;

typedef struct TD_ODepend_ModuleDesc {
  TDCORE
} TD_ODepend_ModuleDesc;
EXTERN_ODepend TD_ODepend_ModuleDesc* td_ODepend_ModuleDesc;
typedef struct TD_ODepend_ImportDesc {
  TDCORE
} TD_ODepend_ImportDesc;
EXTERN_ODepend TD_ODepend_ImportDesc* td_ODepend_ImportDesc;

extern BOOLEAN ODepend_FindFilename (ODepend_Module _mod, SHORTINT _index, LONGINT __name_0, const CHAR (* __name_p));
extern BOOLEAN ODepend_FindFile (ODepend_Module _mod, SHORTINT _index, LONGINT __ext_0, const CHAR (* __ext_p));
extern void ODepend_NewFile (ODepend_Module _mod, SHORTINT _index, LONGINT __ext_0, const CHAR (* __ext_p));
extern ODepend_Module ODepend_TopSort (ODepend_Module _modules, BOOLEAN _mainFirst, LONGINT __errName_0, CHAR (* _errName));
extern ODepend_Module ODepend_Dependencies (LONGINT __modName_0, CHAR (* _modName), BOOLEAN (* _err));

extern void _init_ODepend (void);

#endif
