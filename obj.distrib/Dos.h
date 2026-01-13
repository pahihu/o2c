#ifndef __Dos_
#define __Dos_

#include "_OGCC.h"
#include "Time.h"
#include "Unix.h"

extern void Dos_Delete (const CHAR (* file), BOOLEAN (* Dos_err));
extern void Dos_Rename (const CHAR (* old), const CHAR (* new), BOOLEAN (* Dos_err));
extern void Dos_Copy (const CHAR (* src), const CHAR (* dest), BOOLEAN (* Dos_err));
extern BOOLEAN Dos_Exists (const CHAR (* file));
extern void Dos_GetDate (const CHAR (* file), struct Unix_timeval (* Dos_date), BOOLEAN (* Dos_err));
extern void Dos_GetUserHome (CHAR (* Dos_home), const CHAR (* user));
extern void Dos_GetCWD (CHAR (* Dos_path));
extern void Dos_TmpFileName (CHAR (* Dos_tmpnam));

extern void _init_Dos (void);

#endif
