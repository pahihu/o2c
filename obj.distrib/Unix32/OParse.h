#ifndef __OParse_
#define __OParse_

#ifdef MODULE_OParse
#define EXTERN_OParse
#else
#define EXTERN_OParse extern
#endif

#include "_OGCC.h"
#include "OEParse.h"





extern void OParse_Module (OEParse_Node (* _mod));

extern void _init_OParse (void);

#endif
