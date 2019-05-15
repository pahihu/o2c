#ifndef __Filenames_
#define __Filenames_

#include "_OGCC.h"





extern void Filenames_GetPath (LONGINT __full_0, const CHAR (* __full_p), LONGINT __path_0, CHAR (* _path), LONGINT __file_0, CHAR (* _file));
extern void Filenames_GetExt (LONGINT __full_0, const CHAR (* __full_p), LONGINT __file_0, CHAR (* _file), LONGINT __ext_0, CHAR (* _ext));
extern void Filenames_GetFile (LONGINT __full_0, const CHAR (* __full_p), LONGINT __file_0, CHAR (* _file));
extern void Filenames_AddExt (LONGINT __full_0, CHAR (* _full), LONGINT __file_0, const CHAR (* __file_p), LONGINT __ext_0, const CHAR (* __ext_p));
extern void Filenames_AddPath (LONGINT __full_0, CHAR (* _full), LONGINT __path_0, const CHAR (* __path_p), LONGINT __file_0, const CHAR (* __file_p));
extern void Filenames_BuildFilename (LONGINT __full_0, CHAR (* _full), LONGINT __path_0, const CHAR (* __path_p), LONGINT __file_0, const CHAR (* __file_p), LONGINT __ext_0, const CHAR (* __ext_p));
extern void Filenames_ExpandPath (LONGINT __full_0, CHAR (* _full), LONGINT __path_0, const CHAR (* __path_p));

extern void _init_Filenames (void);

#endif
