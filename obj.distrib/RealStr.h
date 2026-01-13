#ifndef __RealStr_
#define __RealStr_

#include "_OGCC.h"



extern const SHORTINT RealStr_maxSigFigs;


extern void RealStr_Take (LONGINT __str_0, const CHAR (* __str_p), LONGREAL (* _real), SHORTINT (* _format));
extern SHORTINT RealStr_Format (LONGINT __str_0, const CHAR (* __str_p));
extern LONGREAL RealStr_Value (LONGINT __str_0, const CHAR (* __str_p));
extern void RealStr_GiveFloat (LONGINT __str_0, CHAR (* _str), LONGREAL _real, INTEGER _sigFigs, INTEGER _width, SHORTINT _where);
extern void RealStr_GiveEng (LONGINT __str_0, CHAR (* _str), LONGREAL _real, INTEGER _sigFigs, INTEGER _width, SHORTINT _where);
extern void RealStr_GiveFixed (LONGINT __str_0, CHAR (* _str), LONGREAL _real, INTEGER _place, INTEGER _width, SHORTINT _where);

extern void _init_RealStr (void);

#endif
