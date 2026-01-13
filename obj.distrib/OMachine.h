#ifndef __OMachine_
#define __OMachine_

#include "_OGCC.h"
#include "Redir.h"



extern const BOOLEAN OMachine_defCodeForGC;
extern const CHAR OMachine_gcLibPath[];
extern const INTEGER OMachine_maxSizeString;
extern const SHORTINT OMachine_maxSizeIdent;
extern const SHORTINT OMachine_minBool;
extern const SHORTINT OMachine_maxBool;
extern const SHORTINT OMachine_minChar;
extern const INTEGER OMachine_maxChar;
extern const SHORTINT OMachine_minSInt;
extern const SHORTINT OMachine_maxSInt;
extern const INTEGER OMachine_minInt;
extern const INTEGER OMachine_maxInt;
extern const LONGINT OMachine_minLInt;
extern const LONGINT OMachine_maxLInt;
extern const REAL OMachine_minReal;
extern const REAL OMachine_maxReal;
extern const LONGREAL OMachine_minLReal;
extern const LONGREAL OMachine_maxLReal;
extern const SHORTINT OMachine_minSet;
extern const SHORTINT OMachine_maxSet;
extern const SHORTINT OMachine_minRegNum;
extern const SHORTINT OMachine_maxRegNum;
extern const SHORTINT OMachine_minCC;
extern const SHORTINT OMachine_maxCC;
extern const SHORTINT OMachine_minTrapNum;
extern const INTEGER OMachine_maxTrapNum;
extern const SHORTINT OMachine_defAssertTrap;
extern const CHAR OMachine_moduleExtension[];
extern const CHAR OMachine_symbolExtension[];
extern const BOOLEAN OMachine_allowExternal;
extern const BOOLEAN OMachine_allowUnderscore;
extern const BOOLEAN OMachine_allowRestParam;
extern const BOOLEAN OMachine_allowUnion;
extern Redir_Pattern OMachine_redir;
extern BOOLEAN OMachine_generateCodeForGC;



extern void _init_OMachine (void);

#endif
