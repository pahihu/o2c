#include "Filenames.h"
#include "Dos.h"
#include "Strings.h"
#include "Strings2.h"

static ModuleId moduleId;






static INTEGER Filenames_LocateCharLast (LONGINT __str_0, const CHAR (* __str_p), CHAR _ch) {
  CHAR (* _str);
  INTEGER _pos;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  _pos = Strings_Length(__str_0, _str);
  while (_pos>=0)  {
    if ((* PTR_INDEX(_str, _pos, __str_0, 1, 467))==_ch)  {
      return _pos;
    } else {
      DECI(_pos, 1, 524);
    }
  }
  return -1;
  NO_RETURN (176);
}

static void Filenames_SplitRChar (LONGINT __str_0, const CHAR (* __str_p), LONGINT __str1_0, CHAR (* _str1), LONGINT __str2_0, CHAR (* _str2), CHAR _ch) {
  CHAR (* _str);
  INTEGER _len, _pos;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  _len = Strings_Length(__str_0, _str);
  _pos = Filenames_LocateCharLast(__str_0, _str, _ch);
  COPY(_str, _str2, __str2_0);
  if (_pos>=0)  {
    COPY(_str, _str1, __str1_0);
    INCI(_pos, 1, 1483);
    Strings_Delete(__str2_0, _str2, 0, _pos);
    Strings_Delete(__str1_0, _str1, _pos, _len- _pos);
  } else {
    COPY("", _str1, __str1_0);
  }
}

void Filenames_GetPath (LONGINT __full_0, const CHAR (* __full_p), LONGINT __path_0, CHAR (* _path), LONGINT __file_0, CHAR (* _file)) {
  CHAR (* _full);
  VALUE_ARRAY(_full, __full_p, 1*__full_0);
  Filenames_SplitRChar(__full_0, _full, __path_0, _path, __file_0, _file, '/');
}

void Filenames_GetExt (LONGINT __full_0, const CHAR (* __full_p), LONGINT __file_0, CHAR (* _file), LONGINT __ext_0, CHAR (* _ext)) {
  CHAR (* _full);
  VALUE_ARRAY(_full, __full_p, 1*__full_0);
  if (Filenames_LocateCharLast(__full_0, _full, '/')<Filenames_LocateCharLast(__full_0, _full, '.'))  {
    Filenames_SplitRChar(__full_0, _full, __file_0, _file, __ext_0, _ext, '.');
    Strings_Delete(__file_0, _file, Strings_Length(__file_0, _file)- 1, 1);
  } else {
    COPY(_full, _file, __file_0);
    COPY("", _ext, __ext_0);
  }
}

void Filenames_GetFile (LONGINT __full_0, const CHAR (* __full_p), LONGINT __file_0, CHAR (* _file)) {
  CHAR (* _full);
  CHAR _dummy[256];
  VALUE_ARRAY(_full, __full_p, 1*__full_0);
  Filenames_GetPath(__full_0, _full, 256, (CHAR *) _dummy, __file_0, _file);
  Filenames_GetExt(__file_0, _file, __file_0, _file, 256, (CHAR *) _dummy);
}

void Filenames_AddExt (LONGINT __full_0, CHAR (* _full), LONGINT __file_0, const CHAR (* __file_p), LONGINT __ext_0, const CHAR (* __ext_p)) {
  CHAR (* _file);
  CHAR (* _ext);
  VALUE_ARRAY(_file, __file_p, 1*__file_0);
  VALUE_ARRAY(_ext, __ext_p, 1*__ext_0);
  COPY(_file, _full, __full_0);
  if ((* PTR_INDEX(_ext, 0, __ext_0, 1, 3514))!='\000')  {
    Strings2_AppendChar('.', __full_0, _full);
    Strings_Append(__ext_0, _ext, __full_0, _full);
  }
}

void Filenames_AddPath (LONGINT __full_0, CHAR (* _full), LONGINT __path_0, const CHAR (* __path_p), LONGINT __file_0, const CHAR (* __file_p)) {
  CHAR (* _path);
  CHAR (* _file);
  VALUE_ARRAY(_path, __path_p, 1*__path_0);
  VALUE_ARRAY(_file, __file_p, 1*__file_0);
  COPY(_file, _full, __full_0);
  if ((* PTR_INDEX(_path, 0, __path_0, 1, 4004))!='\000')  {
    if ((* PTR_INDEX(_path, Strings_Length(__path_0, _path)- 1, __path_0, 1, 4092))!='/')  {
      Strings_Insert(2, (CHAR *) "/", 0, __full_0, _full);
    }
    Strings_Insert(__path_0, _path, 0, __full_0, _full);
  }
}

void Filenames_BuildFilename (LONGINT __full_0, CHAR (* _full), LONGINT __path_0, const CHAR (* __path_p), LONGINT __file_0, const CHAR (* __file_p), LONGINT __ext_0, const CHAR (* __ext_p)) {
  CHAR (* _path);
  CHAR (* _file);
  CHAR (* _ext);
  VALUE_ARRAY(_path, __path_p, 1*__path_0);
  VALUE_ARRAY(_file, __file_p, 1*__file_0);
  VALUE_ARRAY(_ext, __ext_p, 1*__ext_0);
  Filenames_AddExt(__full_0, _full, __file_0, _file, __ext_0, _ext);
  Filenames_AddPath(__full_0, _full, __path_0, _path, __full_0, _full);
}

void Filenames_ExpandPath (LONGINT __full_0, CHAR (* _full), LONGINT __path_0, const CHAR (* __path_p)) {
  CHAR (* _path);
  INTEGER _len, _posSep, _posSuffix;
  CHAR _suffix[256];
  CHAR _userpath[256];
  CHAR _username[32];
  VALUE_ARRAY(_path, __path_p, 1*__path_0);
  COPY(_path, _full, __full_0);
  if ((* PTR_INDEX(_path, 0, __path_0, 1, 5143))=='~')  {
    _posSep = Strings2_PosChar('/', __path_0, _path);
    _len = Strings_Length(__path_0, _path);
    if (_posSep<0)  {
      _posSep = _len;
      _posSuffix = _len;
    } else {
      _posSuffix = _posSep+1;
    }
    Strings_Extract(__path_0, _path, _posSuffix, _len- _posSuffix, 256, (CHAR *) _suffix);
    Strings_Extract(__path_0, _path, 1, _posSep- 1, 32, (CHAR *) _username);
    Dos_GetUserHome((CHAR *) _userpath, (CHAR *) _username);
    if (INDEX(_userpath, 0, 256, 5638)!='\000')  {
      Filenames_AddPath(__full_0, _full, 256, (CHAR *) _userpath, 256, (CHAR *) _suffix);
    }
  }
}

void _init_Filenames (void) {
  moduleId = add_module ("Filenames");
}
