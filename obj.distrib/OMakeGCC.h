#ifndef __OMakeGCC_
#define __OMakeGCC_

#include "_OGCC.h"
#include "ODepend.h"



extern CHAR OMakeGCC_makefile[256];


extern void OMakeGCC_CompilationHook (ODepend_Module _mod);
extern BOOLEAN OMakeGCC_NeedCompile (ODepend_Module _mod);
extern ODepend_Module OMakeGCC_ModuleInfo (LONGINT __fileName_0, const CHAR (* __fileName_p));
extern void OMakeGCC_Build (LONGINT __modName_0, const CHAR (* __modName_p), LONGINT __comName_0, const CHAR (* __comName_p), ODepend_Module _depList, BOOLEAN _makeAll, BOOLEAN (* _err));
extern void OMakeGCC_Help (void);
extern BOOLEAN OMakeGCC_Option (LONGINT __arg_0, const CHAR (* __arg_p));
extern BOOLEAN OMakeGCC_OptionExt (LONGINT __arg0_0, const CHAR (* __arg0_p), LONGINT __arg1_0, const CHAR (* __arg1_p));
extern void OMakeGCC_Makefile (LONGINT __modName_0, const CHAR (* __modName_p), LONGINT __comName_0, const CHAR (* __comName_p), LONGINT __o2opt_0, const CHAR (* __o2opt_p), BOOLEAN (* _err));
extern void OMakeGCC_Init (void);

extern void _init_OMakeGCC (void);

#endif
