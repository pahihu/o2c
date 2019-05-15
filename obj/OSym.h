#ifndef __OSym_
#define __OSym_

#include "_OGCC.h"
#include "ODepend.h"
#include "OTable.h"





extern void OSym_Export (ODepend_Module _module, BOOLEAN _force);
extern OTable_Object OSym_GetImport (LONGINT __module_0, const CHAR (* __module_p), LONGINT __importing_0, const CHAR (* __importing_p));
extern void OSym_Import (LONGINT __module_0, const CHAR (* __module_p), LONGINT __alias_0, const CHAR (* __alias_p), LONGINT _pos);

extern void _init_OSym (void);

#endif
