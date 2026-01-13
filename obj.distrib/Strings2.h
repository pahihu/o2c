#ifndef __Strings2_
#define __Strings2_

#include "_OGCC.h"





extern void Strings2_AppendChar (CHAR _ch, LONGINT __dst_0, CHAR (* _dst));
extern void Strings2_InsertChar (CHAR _ch, INTEGER _pos, LONGINT __dst_0, CHAR (* _dst));
extern INTEGER Strings2_PosChar (CHAR _ch, LONGINT __str_0, const CHAR (* __str_p));
extern BOOLEAN Strings2_Match (LONGINT __pat_0, const CHAR (* __pat_p), LONGINT __s_0, const CHAR (* __s_p));

extern void _init_Strings2 (void);

#endif
