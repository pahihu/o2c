#ifndef __IntStr_
#define __IntStr_

#include "_OGCC.h"





extern void IntStr_Take (LONGINT __str_0, const CHAR (* __str_p), LONGINT (* _int), SHORTINT (* _format));
extern SHORTINT IntStr_Format (LONGINT __str_0, const CHAR (* __str_p));
extern LONGINT IntStr_Value (LONGINT __str_0, const CHAR (* __str_p));
extern void IntStr_Give (LONGINT __str_0, CHAR (* _str), LONGINT _int, INTEGER _width, SHORTINT _where);
extern void IntStr_Append (LONGINT _int, INTEGER _width, SHORTINT _where, LONGINT __dest_0, CHAR (* _dest));

extern void _init_IntStr (void);

#endif
