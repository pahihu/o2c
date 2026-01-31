#include "OMachine.h"

static ModuleId moduleId;



const BOOLEAN OMachine_defCodeForGC = FALSE;
const CHAR OMachine_gcLibPath[] = "";
const INTEGER OMachine_maxSizeString = 256;
const SHORTINT OMachine_maxSizeIdent = 48;
const SHORTINT OMachine_maxLIntHexDigits = 16;
const SHORTINT OMachine_minBool = 0;
const SHORTINT OMachine_maxBool = 1;
const SHORTINT OMachine_minChar = 0;
const INTEGER OMachine_maxChar = 255;
const SHORTINT OMachine_minSInt = -32768;
const SHORTINT OMachine_maxSInt = 32767;
const INTEGER OMachine_minInt = (-2147483647-1);
const INTEGER OMachine_maxInt = 2147483647;
const LONGINT OMachine_minLInt = (-9223372036854775807LL-1);
const LONGINT OMachine_maxLInt = 9223372036854775807LL;
const REAL OMachine_minReal = MIN_REAL;
const REAL OMachine_maxReal = MAX_REAL;
const LONGREAL OMachine_minLReal = MIN_LONGREAL;
const LONGREAL OMachine_maxLReal = MAX_LONGREAL;
const SHORTINT OMachine_minSet = 0;
const SHORTINT OMachine_maxSet = 63;
const SHORTINT OMachine_minRegNum = 0;
const SHORTINT OMachine_maxRegNum = -1;
const SHORTINT OMachine_minCC = 0;
const SHORTINT OMachine_maxCC = -1;
const SHORTINT OMachine_minTrapNum = 0;
const INTEGER OMachine_maxTrapNum = 255;
const SHORTINT OMachine_defAssertTrap = 1;
const CHAR OMachine_moduleExtension[] = "Mod";
const CHAR OMachine_symbolExtension[] = "OSym";
const BOOLEAN OMachine_allowExternal = TRUE;
const BOOLEAN OMachine_allowUnderscore = TRUE;
const BOOLEAN OMachine_allowRestParam = TRUE;
const BOOLEAN OMachine_allowUnion = TRUE;
Redir_Pattern OMachine_redir;
BOOLEAN OMachine_generateCodeForGC;



void _init_OMachine (void) {
  moduleId = add_module ("OMachine");
  OMachine_generateCodeForGC = OMachine_defCodeForGC;
}
