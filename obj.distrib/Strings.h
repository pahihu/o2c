#ifndef __Strings_
#define __Strings_

#include "_OGCC.h"





extern INTEGER Strings_Length (LONGINT __s_0, const CHAR (* __s_p));
extern void Strings_Insert (LONGINT __src_0, const CHAR (* __src_p), INTEGER _pos, LONGINT __dst_0, CHAR (* _dst));
extern void Strings_Append (LONGINT __s_0, const CHAR (* __s_p), LONGINT __dst_0, CHAR (* _dst));
extern void Strings_Delete (LONGINT __s_0, CHAR (* _s), INTEGER _pos, INTEGER _n);
extern void Strings_Replace (LONGINT __src_0, const CHAR (* __src_p), INTEGER _pos, LONGINT __dst_0, CHAR (* _dst));
extern void Strings_Extract (LONGINT __src_0, const CHAR (* __src_p), INTEGER _pos, INTEGER _n, LONGINT __dst_0, CHAR (* _dst));
extern INTEGER Strings_Pos (LONGINT __pat_0, const CHAR (* __pat_p), LONGINT __s_0, const CHAR (* __s_p), INTEGER _pos);
extern void Strings_Cap (LONGINT __s_0, CHAR (* _s));

extern void _init_Strings (void);

#endif
