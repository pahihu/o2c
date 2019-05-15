#ifndef __Redir_
#define __Redir_

#include "_OGCC.h"


struct TD_Redir_PatternDesc;
struct TD_Redir_PathDesc;

extern const SHORTINT Redir_maxPatternLen;
extern const INTEGER Redir_maxPathLen;
typedef struct Redir_PatternDesc (* Redir_Pattern);
typedef struct Redir_PathDesc (* Redir_Path);
typedef struct Redir_PatternDesc {
  Redir_Pattern _next;
  CHAR _wildcard[32];
  BOOLEAN _rcs;
  Redir_Path _paths;
} Redir_PatternDesc;
typedef struct Redir_PathDesc {
  Redir_Path _next;
  CHAR _path[256];
} Redir_PathDesc;

typedef struct TD_Redir_PatternDesc {
  TDCORE
} TD_Redir_PatternDesc;
TD_Redir_PatternDesc* td_Redir_PatternDesc;
typedef struct TD_Redir_PathDesc {
  TDCORE
} TD_Redir_PathDesc;
TD_Redir_PathDesc* td_Redir_PathDesc;

extern Redir_Pattern Redir_Read (LONGINT __fileName_0, const CHAR (* __fileName_p));
extern void Redir_Free (Redir_Pattern (* _p));
extern BOOLEAN Redir_FindPath (Redir_Pattern _redir, LONGINT __fileName_0, const CHAR (* __fileName_p), LONGINT __path_0, CHAR (* _path));
extern void Redir_GeneratePath (Redir_Pattern _redir, LONGINT __fileName_0, const CHAR (* __fileName_p), LONGINT __path_0, CHAR (* _path));
extern BOOLEAN Redir_FindPathExt (Redir_Pattern _redir, LONGINT __name_0, const CHAR (* __name_p), LONGINT __ext_0, const CHAR (* __ext_p), LONGINT __path_0, CHAR (* _path));
extern void Redir_GeneratePathExt (Redir_Pattern _redir, LONGINT __name_0, const CHAR (* __name_p), LONGINT __ext_0, const CHAR (* __ext_p), LONGINT __path_0, CHAR (* _path));
extern void Redir_RCS2File (LONGINT __rcsFile_0, const CHAR (* __rcsFile_p), LONGINT __workFile_0, CHAR (* _workFile));

extern void _init_Redir (void);

#endif
