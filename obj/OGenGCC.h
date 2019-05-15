#ifndef __OGenGCC_
#define __OGenGCC_

#include "_OGCC.h"
#include "ODepend.h"
#include "OEParse.h"



extern const SHORTINT OGenGCC_header;
extern const SHORTINT OGenGCC_cFile;
extern const SHORTINT OGenGCC_objFile;
extern const CHAR OGenGCC_cExt;
extern const CHAR OGenGCC_hExt;
extern const CHAR OGenGCC_markerExt;
extern const SHORTINT OGenGCC_extCSource;
extern const SHORTINT OGenGCC_extObject;
extern const SHORTINT OGenGCC_extArchive;
extern const SHORTINT OGenGCC_extLibrary;


extern void OGenGCC_WriteInclude (LONGINT __inclFile_0, const CHAR (* __inclFile_p));
extern void OGenGCC_Module (ODepend_Module _mod, OEParse_Node _stRoot);

extern void _init_OGenGCC (void);

#endif
