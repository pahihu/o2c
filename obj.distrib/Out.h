#ifndef __Out_
#define __Out_

#include "_OGCC.h"





extern void Out_Open (void);
extern void Out_Char (CHAR _ch);
extern void Out_String (LONGINT __s_0, const CHAR (* __s_p));
extern void Out_Int (LONGINT _i, LONGINT _n);
extern void Out_Real2Str (LONGREAL _x, INTEGER _sigFigs, INTEGER _expCyphers, LONGINT __str_0, CHAR (* _str));
extern void Out_Real (REAL _x, INTEGER _n);
extern void Out_LongReal (LONGREAL _x, INTEGER _n);
extern void Out_Ln (void);
extern void Out_Flush (void);

extern void _init_Out (void);

#endif
