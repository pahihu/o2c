#ifndef __FOut_
#define __FOut_

#include "_OGCC.h"





extern void FOut_Open (LONGINT __name_0, const CHAR (* __name_p));
extern void FOut_Flush (void);
extern void FOut_Close (void);
extern void FOut_Char (CHAR _ch);
extern void FOut_String (LONGINT __str_0, const CHAR (* __str_p));
extern void FOut_Ln (void);
extern void FOut_Int (LONGINT _i, LONGINT _n);
extern void FOut_Real (REAL _x, INTEGER _n);
extern void FOut_LongReal (LONGREAL _x, INTEGER _n);

extern void _init_FOut (void);

#endif
