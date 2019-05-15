#ifndef __Rts_
#define __Rts_

#include "_OGCC.h"
#include "CType.h"

extern INTEGER Rts_exitCode;
extern LONGINT Rts_argc;
extern CType_charPtr2d Rts_argv;
extern INTEGER Rts_ArgNumber (void);
extern void Rts_GetArg (INTEGER num, CHAR (* Rts_arg));
extern void Rts_Terminate (void);
extern void Rts_Error (const CHAR (* msg));
extern void Rts_Assert (BOOLEAN expr, const CHAR (* msg));
extern void Rts_TerminationProc (CType_Proc proc);
extern INTEGER Rts_System (const CHAR (* command));

extern void _init_Rts (void);

#endif
