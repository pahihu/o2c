#include "Redir.h"
#include "CharInfo.h"
#include "Dos.h"
#include "Filenames.h"
#include "Files.h"
#include "Out.h"
#include "Rts.h"
#include "Strings.h"
#include "Strings2.h"

static ModuleId moduleId;



const SHORTINT Redir_maxPatternLen = 32;
const INTEGER Redir_maxPathLen = 256;



Redir_Pattern Redir_Read (LONGINT __fileName_0, const CHAR (* __fileName_p)) {
  CHAR (* _fileName);
  Files_File _f;
  struct Files_Rider _r;
  CHAR _ch;
  Redir_Pattern _p;
  CHAR _fileFull[256];

  Redir_Pattern _ReadPattern (void) {
    Redir_Pattern _pattern;

    void _Error (LONGINT __msg_0, const CHAR (* __msg_p)) {
      CHAR (* _msg);
      VALUE_ARRAY(_msg, __msg_p, 1*__msg_0);
      Out_String(10, (CHAR *) "Error in ");
      Out_String(__fileName_0, _fileName);
      Out_String(21, (CHAR *) " at/before position ");
      Out_Int(Files_Pos(&(_r)), 0);
      Out_Char(':');
      Out_Ln();
      Out_String(__msg_0, _msg);
      Out_Ln();
      Rts_Terminate();
    }

    void _SkipBlanks (void) {
      while(1) {
        while (_ch<=' '&&!_r.eof)  {
          Files_Read(&(_r), &(_ch));
        }
        if (_r.eof||_ch!='#')  {
          goto _exit3223;
        }
        while (_ch!=CharInfo_eol&&!_r.eof)  {
          Files_Read(&(_r), &(_ch));
        }
      }
      _exit3223:
    }

    void _Match (CHAR _test) {
      CHAR _msg[16];
      _SkipBlanks();
      if (_test!=_ch)  {
        COPYSTRING(&(_msg), "'X' expected.");
        INDEX(_msg, 1, 16, 3890) = _test;
        _Error(16, (CHAR *) _msg);
      }
      Files_Read(&(_r), &(_ch));
    }

    void _ReadString (LONGINT __str_0, CHAR (* _str)) {
      INTEGER _len;
      _SkipBlanks();
      _len = 0;
      if (!_r.eof)  {
        if (_ch=='\042')  {
          Files_Read(&(_r), &(_ch));
          while (_ch!='\042')  {
            if (_r.eof||_ch<' ')  {
              _Error(21, (CHAR *) "Unterminated string.");
            } else {
              (* PTR_INDEX(_str, _len, __str_0, 1, 4587)) = _ch;
              INCI(_len, 1, 4646);
              if ((LONGINT) _len==__str_0)  {
                _Error(17, (CHAR *) "String too long.");
              }
            }
            Files_Read(&(_r), &(_ch));
          }
          Files_Read(&(_r), &(_ch));
        } else {
          do  {
            (* PTR_INDEX(_str, _len, __str_0, 1, 5043)) = _ch;
            INCI(_len, 1, 5102);
            if ((LONGINT) _len==__str_0)  {
              _Error(16, (CHAR *) "String to long.");
            }
            Files_Read(&(_r), &(_ch));
          } while (!((((((_r.eof||_ch<=' ')||_ch=='=')||_ch==';')||_ch=='#')||_ch=='+')||_ch==','));
        }
      }
      (* PTR_INDEX(_str, _len, __str_0, 1, 5458)) = '\000';
    }

    Redir_Path _ReadPath (void) {
      Redir_Path _path;
      CHAR _str[16];
      NEWREC(_path, td_Redir_PathDesc, 0);
      _SkipBlanks();
      _ReadString(256, (CHAR *) DEREF(Redir_Path, _path, 5733)._path);
      Filenames_ExpandPath(256, (CHAR *) DEREF(Redir_Path, _path, 5776)._path, 256, (CHAR *) DEREF(Redir_Path, _path, 5788)._path);
      _SkipBlanks();
      if (_ch==';')  {
        Files_Read(&(_r), &(_ch));
        DEREF(Redir_Path, _path, 5888)._next = _ReadPath();
      } else {
        DEREF(Redir_Path, _path, 5975)._next = (Redir_Path)NULL;
        if (_ch=='+')  {
          Files_Read(&(_r), &(_ch));
          _ReadString(16, (CHAR *) _str);
          if (STREQL(_str, "RCS"))  {
            DEREF(Redir_Pattern, _pattern, 6185)._rcs = TRUE;
          } else {
            _Error(16, (CHAR *) "'RCS' expected.");
          }
        }
      }
      return _path;
      NO_RETURN (5513);
    }
    _SkipBlanks();
    if (_r.eof)  {
      return NULL;
    } else {
      NEWREC(_pattern, td_Redir_PatternDesc, 0);
      _ReadString(32, (CHAR *) DEREF(Redir_Pattern, _pattern, 6583)._wildcard);
      if (_ch==',')  {
        Files_Read(&(_r), &(_ch));
        DEREF(Redir_Pattern, _pattern, 6707)._next = _ReadPattern();
        DEREF(Redir_Pattern, _pattern, 6749)._rcs = DEREF(Redir_Pattern, DEREF(Redir_Pattern, _pattern, 6765)._next, 6771)._rcs;
        DEREF(Redir_Pattern, _pattern, 6795)._paths = DEREF(Redir_Pattern, DEREF(Redir_Pattern, _pattern, 6813)._next, 6819)._paths;
      } else {
        _Match('=');
        DEREF(Redir_Pattern, _pattern, 6880)._rcs = FALSE;
        DEREF(Redir_Pattern, _pattern, 6913)._paths = _ReadPath();
        DEREF(Redir_Pattern, _pattern, 6993)._next = _ReadPattern();
      }
      return _pattern;
    }
    NO_RETURN (2428);
  }
  VALUE_ARRAY(_fileName, __fileName_p, 1*__fileName_0);
  Filenames_ExpandPath(256, (CHAR *) _fileFull, __fileName_0, _fileName);
  if (Dos_Exists((CHAR *) _fileFull))  {
    _f = Files_Old((CHAR *) _fileFull);
    if ((void*)_f!=(void*)NULL)  {
      Files_Set(&(_r), _f, 0);
      Files_Read(&(_r), &(_ch));
      _p = _ReadPattern();
      Files_Close(&(_f));
      return _p;
    } else {
      Out_String(19, (CHAR *) "Error: Can't open ");
      Out_String(256, (CHAR *) _fileFull);
      Out_String(12, (CHAR *) ", aborting.");
      Out_Ln();
      Rts_Terminate();
    }
  } else {
    Out_String(28, (CHAR *) "Warning: Redirection table ");
    Out_String(256, (CHAR *) _fileFull);
    Out_String(17, (CHAR *) " does not exist.");
    Out_Ln();
    return NULL;
  }
  NO_RETURN (1779);
}

void Redir_Free (Redir_Pattern (* _p)) {

  void _FreePaths (Redir_Path (* _path)) {
    if ((void*)(* _path)!=(void*)NULL)  {
      _FreePaths(&(DEREF(Redir_Path, (* _path), 8037)._next));
      DISPOSE_REC((* _path));
    }
  }
  if ((void*)(* _p)!=(void*)NULL)  {
    if ((void*)DEREF(Redir_Pattern, (* _p), 8149)._next!=(void*)NULL&&DEREF(Redir_Pattern, (* _p), 8167)._paths==DEREF(Redir_Pattern, DEREF(Redir_Pattern, (* _p), 8178)._next, 8184)._paths)  {
      DEREF(Redir_Pattern, (* _p), 8259)._paths = (Redir_Path)NULL;
    } else {
      _FreePaths(&(DEREF(Redir_Pattern, (* _p), 8305)._paths));
    }
    Redir_Free(&(DEREF(Redir_Pattern, (* _p), 8338)._next));
    DISPOSE_REC((* _p));
  }
}

BOOLEAN Redir_FindPath (Redir_Pattern _redir, LONGINT __fileName_0, const CHAR (* __fileName_p), LONGINT __path_0, CHAR (* _path)) {
  CHAR (* _fileName);
  Redir_Pattern _pattern;
  Redir_Path _dir;

  void _MakeRCSName (BOOLEAN _rcsSubdir, LONGINT __rcsFile_0, CHAR (* _rcsFile)) {
    CHAR _path[256];
    CHAR _name[256];
    COPY(DEREF(Redir_Path, _dir, 9729)._path, _path, 256);
    if (_rcsSubdir)  {
      Filenames_AddPath(256, (CHAR *) _path, 256, (CHAR *) _path, 4, (CHAR *) "RCS");
    }
    COPY(_fileName, _name, 256);
    Strings_Append(3, (CHAR *) ",v", 256, (CHAR *) _name);
    Filenames_BuildFilename(__rcsFile_0, _rcsFile, 256, (CHAR *) _path, 256, (CHAR *) _name, 1, (CHAR *) "");
  }
  VALUE_ARRAY(_fileName, __fileName_p, 1*__fileName_0);
  _pattern = _redir;
  while ((void*)_pattern!=(void*)NULL)  {
    if (Strings2_Match(32, (CHAR *) DEREF(Redir_Pattern, _pattern, 10106)._wildcard, __fileName_0, _fileName))  {
      _dir = DEREF(Redir_Pattern, _pattern, 10155)._paths;
      while ((void*)_dir!=(void*)NULL)  {
        Filenames_AddPath(__path_0, _path, 256, (CHAR *) DEREF(Redir_Path, _dir, 10282)._path, __fileName_0, _fileName);
        if (Dos_Exists(_path))  {
          return TRUE;
        } else if (DEREF(Redir_Pattern, _pattern, 10407)._rcs)  {
          _MakeRCSName(TRUE, __path_0, _path);
          if (Dos_Exists(_path))  {
            return TRUE;
          } else {
            _MakeRCSName(FALSE, __path_0, _path);
            if (Dos_Exists(_path))  {
              return TRUE;
            }
          }
        }
        _dir = DEREF(Redir_Path, _dir, 10830)._next;
      }
    }
    _pattern = DEREF(Redir_Pattern, _pattern, 10884)._next;
  }
  COPY(_fileName, _path, __path_0);
  return Dos_Exists(_path);
  NO_RETURN (8405);
}

void Redir_GeneratePath (Redir_Pattern _redir, LONGINT __fileName_0, const CHAR (* __fileName_p), LONGINT __path_0, CHAR (* _path)) {
  CHAR (* _fileName);
  Redir_Pattern _pattern;
  VALUE_ARRAY(_fileName, __fileName_p, 1*__fileName_0);
  _pattern = _redir;
  while ((void*)_pattern!=(void*)NULL)  {
    if (Strings2_Match(32, (CHAR *) DEREF(Redir_Pattern, _pattern, 11839)._wildcard, __fileName_0, _fileName))  {
      Filenames_AddPath(__path_0, _path, 256, (CHAR *) DEREF(Redir_Path, DEREF(Redir_Pattern, _pattern, 11906)._paths, 11913)._path, __fileName_0, _fileName);
      return;
    }
    _pattern = DEREF(Redir_Pattern, _pattern, 11982)._next;
  }
  COPY(_fileName, _path, __path_0);
}

BOOLEAN Redir_FindPathExt (Redir_Pattern _redir, LONGINT __name_0, const CHAR (* __name_p), LONGINT __ext_0, const CHAR (* __ext_p), LONGINT __path_0, CHAR (* _path)) {
  CHAR (* _name);
  CHAR (* _ext);
  CHAR _fileName[256];
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  VALUE_ARRAY(_ext, __ext_p, 1*__ext_0);
  COPY(_name, _fileName, 256);
  Strings2_AppendChar('.', __name_0, _name);
  Strings_Append(__ext_0, _ext, __name_0, _name);
  return Redir_FindPath(_redir, __name_0, _name, __path_0, _path);
  NO_RETURN (12057);
}

void Redir_GeneratePathExt (Redir_Pattern _redir, LONGINT __name_0, const CHAR (* __name_p), LONGINT __ext_0, const CHAR (* __ext_p), LONGINT __path_0, CHAR (* _path)) {
  CHAR (* _name);
  CHAR (* _ext);
  CHAR _fileName[256];
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  VALUE_ARRAY(_ext, __ext_p, 1*__ext_0);
  COPY(_name, _fileName, 256);
  Strings2_AppendChar('.', __name_0, _name);
  Strings_Append(__ext_0, _ext, __name_0, _name);
  Redir_GeneratePath(_redir, __name_0, _name, __path_0, _path);
}

void Redir_RCS2File (LONGINT __rcsFile_0, const CHAR (* __rcsFile_p), LONGINT __workFile_0, CHAR (* _workFile)) {
  CHAR (* _rcsFile);
  CHAR _path[256];
  CHAR _name[256];
  VALUE_ARRAY(_rcsFile, __rcsFile_p, 1*__rcsFile_0);
  Filenames_GetPath(__rcsFile_0, _rcsFile, 256, (CHAR *) _path, 256, (CHAR *) _name);
  if (Strings2_Match(6, (CHAR *) "*RCS/", 256, (CHAR *) _path))  {
    INDEX(_path, Strings_Length(256, (CHAR *) _path)- 4, 256, 13269) = '\000';
  }
  if (Strings2_Match(4, (CHAR *) "*,v", 256, (CHAR *) _name))  {
    INDEX(_name, Strings_Length(256, (CHAR *) _name)- 2, 256, 13357) = '\000';
  }
  Filenames_BuildFilename(__workFile_0, _workFile, 256, (CHAR *) _path, 256, (CHAR *) _name, 1, (CHAR *) "");
}

void _init_Redir (void) {
  moduleId = add_module ("Redir");
  td_Redir_PatternDesc = create_td("PatternDesc", sizeof(Redir_PatternDesc), sizeof(TD_Redir_PatternDesc), NULL, 0);
  td_Redir_PathDesc = create_td("PathDesc", sizeof(Redir_PathDesc), sizeof(TD_Redir_PathDesc), NULL, 0);
}
