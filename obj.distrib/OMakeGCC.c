#include "OMakeGCC.h"
#include "CharInfo.h"
#include "Dos.h"
#include "FOut.h"
#include "Filenames.h"
#include "OGenGCC.h"
#include "OMachine.h"
#include "Out.h"
#include "Redir.h"
#include "Rts.h"
#include "OScan.h"
#include "Strings.h"
#include "Strings2.h"
#include "OTable.h"
#include "Time.h"

static ModuleId moduleId;



static BOOLEAN OMakeGCC_makeOberon;
static CHAR OMakeGCC_cc[64];
static CHAR OMakeGCC_cflags[256];
static CHAR OMakeGCC_ldflags[256];
static CHAR (* OMakeGCC_inclPaths);
static CHAR OMakeGCC_redirFile[256];
CHAR OMakeGCC_makefile[256];



static BOOLEAN OMakeGCC_GetMarkerTime (LONGINT __modName_0, const CHAR (* __modName_p), struct Unix_timeval (* _time)) {
  CHAR (* _modName);
  CHAR _marker[256];
  BOOLEAN _err;
  VALUE_ARRAY(_modName, __modName_p, 1*__modName_0);
  if (Redir_FindPathExt(OMachine_redir, __modName_0, _modName, 2, (CHAR *) "m", 256, (CHAR *) _marker))  {
    Dos_GetDate((CHAR *) _marker, _time, &(_err));
    return !_err;
  } else {
    return FALSE;
  }
  NO_RETURN (478);
}

void OMakeGCC_CompilationHook (ODepend_Module _mod) {
  if (IN(ODepend_flExternal, DEREF(ODepend_Module, _mod, 901)._flags, 895))  {
    if (Strings2_Match(4, (CHAR *) "*.c", 256, (CHAR *) DEREF(ODepend_Module, _mod, 950)._extName))  {
      DEREF(ODepend_Module, _mod, 1059)._extClass = OGenGCC_extCSource;
      if (ODepend_FindFilename(_mod, OGenGCC_cFile, 256, (CHAR *) DEREF(ODepend_Module, _mod, 1137)._extName))  {
        INCL(DEREF(ODepend_Module, _mod, 1172)._flags, OGenGCC_cFile, 1163);
      }
    } else if (Strings2_Match(4, (CHAR *) "*.o", 256, (CHAR *) DEREF(ODepend_Module, _mod, 1242)._extName))  {
      DEREF(ODepend_Module, _mod, 1351)._extClass = OGenGCC_extObject;
      if (ODepend_FindFilename(_mod, OGenGCC_objFile, 256, (CHAR *) DEREF(ODepend_Module, _mod, 1430)._extName))  {
        INCL(DEREF(ODepend_Module, _mod, 1465)._flags, OGenGCC_objFile, 1456);
      }
    } else if (Strings2_Match(4, (CHAR *) "*.a", 256, (CHAR *) DEREF(ODepend_Module, _mod, 1537)._extName))  {
      DEREF(ODepend_Module, _mod, 1646)._extClass = OGenGCC_extArchive;
      if (ODepend_FindFilename(_mod, OGenGCC_objFile, 256, (CHAR *) DEREF(ODepend_Module, _mod, 1726)._extName))  {
        INCL(DEREF(ODepend_Module, _mod, 1761)._flags, OGenGCC_objFile, 1752);
      }
    } else {
      DEREF(ODepend_Module, _mod, 1858)._extClass = OGenGCC_extLibrary;
    }
  }
}

BOOLEAN OMakeGCC_NeedCompile (ODepend_Module _mod) {
  ODepend_Import _import;
  BOOLEAN _existsH, _existsC;
  struct Unix_timeval _mtime;
  _existsH = ODepend_FindFile(_mod, OGenGCC_header, 2, (CHAR *) "h");
  if (IN(ODepend_flExternal, DEREF(ODepend_Module, _mod, 2579)._flags, 2573))  {
    _existsC = OMakeGCC_GetMarkerTime(98, (CHAR *) DEREF(ODepend_Module, _mod, 2628)._name, &(_mtime));
  } else {
    _existsC = ODepend_FindFile(_mod, OGenGCC_cFile, 2, (CHAR *) "c");
    _mtime = INDEX(DEREF(ODepend_Module, _mod, 2728)._time, OGenGCC_cFile, 5, 2734);
  }
  if (!(_existsH&&_existsC))  {
    return TRUE;
  }
  _import = DEREF(ODepend_Module, _mod, 3022)._import;
  while ((void*)_import!=(void*)NULL)  {
    if (Time_Cmp(INDEX(DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 3086)._module, 3094)._time, ODepend_flSymExists, 5, 3100), _mtime)<0)  {
      return TRUE;
    }
    _import = DEREF(ODepend_Import, _import, 3189)._next;
  }
  return Time_Cmp(INDEX(DEREF(ODepend_Module, _mod, 3304)._time, ODepend_flModExists, 5, 3310), _mtime)<0;
  NO_RETURN (1944);
}

ODepend_Module OMakeGCC_ModuleInfo (LONGINT __fileName_0, const CHAR (* __fileName_p)) {
  CHAR (* _fileName);
  ODepend_Module _mod;
  BOOLEAN _found;
  VALUE_ARRAY(_fileName, __fileName_p, 1*__fileName_0);
  NEWREC(_mod, td_ODepend_ModuleDesc, 0);
  DEREF(ODepend_Module, _mod, 3552)._next = (ODepend_Module)NULL;
  DEREF(ODepend_Module, _mod, 3574)._flags = 0x00000001U;
  DEREF(ODepend_Module, _mod, 3611)._import = (ODepend_Import)NULL;
  COPY(_fileName, INDEX(DEREF(ODepend_Module, _mod, 3651)._file, ODepend_flModExists, 5, 3657), 256);
  COPY(DEREF(OTable_Object, OTable_compiledModule, 3703)._name, DEREF(ODepend_Module, _mod, 3714)._name, 98);
  _found = ODepend_FindFile(_mod, OGenGCC_header, 2, (CHAR *) "h");
  return _mod;
  NO_RETURN (3374);
}

static void OMakeGCC_WriteName (LONGINT __modName_0, const CHAR (* __modName_p), LONGINT __comName_0, const CHAR (* __comName_p), LONGINT __suffix_0, const CHAR (* __suffix_p)) {
  CHAR (* _modName);
  CHAR (* _comName);
  CHAR (* _suffix);
  VALUE_ARRAY(_modName, __modName_p, 1*__modName_0);
  VALUE_ARRAY(_comName, __comName_p, 1*__comName_0);
  VALUE_ARRAY(_suffix, __suffix_p, 1*__suffix_0);
  FOut_String(__modName_0, _modName);
  if (STRNEQ(_comName, ""))  {
    FOut_Char('_');
    FOut_String(__comName_0, _comName);
  }
  FOut_String(__suffix_0, _suffix);
}

static void OMakeGCC_GenMainModule (ODepend_Module _module, LONGINT __modName_0, const CHAR (* __modName_p), LONGINT __comName_0, const CHAR (* __comName_p), ODepend_Module _modules) {
  CHAR (* _modName);
  CHAR (* _comName);
  ODepend_Module _mod;
  BOOLEAN _rts;
  VALUE_ARRAY(_modName, __modName_p, 1*__modName_0);
  VALUE_ARRAY(_comName, __comName_p, 1*__comName_0);
  COPY("_", DEREF(ODepend_Module, _module, 4931)._name, 98);
  Strings_Append(__modName_0, _modName, 98, (CHAR *) DEREF(ODepend_Module, _module, 4971)._name);
  if (STRNEQ(_comName, ""))  {
    Strings2_AppendChar('_', 98, (CHAR *) DEREF(ODepend_Module, _module, 5045)._name);
    Strings_Append(__comName_0, _comName, 98, (CHAR *) DEREF(ODepend_Module, _module, 5087)._name);
  }
  ODepend_NewFile(_module, OGenGCC_cFile, 2, (CHAR *) "c");
  DEREF(ODepend_Module, _module, 5161)._flags = 0x00000008U;
  FOut_Open(256, (CHAR *) INDEX(DEREF(ODepend_Module, _module, 5206)._file, OGenGCC_cFile, 5, 5212));
  _rts = FALSE;
  _mod = _modules;
  while ((void*)_mod!=(void*)NULL)  {
    OGenGCC_WriteInclude(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 5384)._file, OGenGCC_header, 5, 5390));
    if (STREQL(DEREF(ODepend_Module, _mod, 5418)._name, "Rts"))  {
      _rts = TRUE;
    }
    _mod = DEREF(ODepend_Module, _mod, 5486)._next;
  }
  FOut_Ln();
  if (_rts)  {
    FOut_String(34, (CHAR *) "int main(int argc, char **argv) {");
    FOut_Ln();
    FOut_String(19, (CHAR *) "  Rts_argc = argc;");
    FOut_Ln();
    FOut_String(26, (CHAR *) "  Rts_argv = (void*)argv;");
    FOut_Ln();
  } else {
    FOut_String(17, (CHAR *) "int main(void) {");
    FOut_Ln();
  }
  FOut_String(17, (CHAR *) "  _init__OGCC();");
  FOut_Ln();
  _mod = _modules;
  while ((void*)_mod!=(void*)NULL)  {
    if (!IN(ODepend_flExternal, DEREF(ODepend_Module, _mod, 6041)._flags, 6035)||DEREF(ODepend_Module, _mod, 6066)._extClass==OGenGCC_extCSource)  {
      FOut_String(9, (CHAR *) "  _init_");
      FOut_String(98, (CHAR *) DEREF(ODepend_Module, _mod, 6158)._name);
      FOut_String(4, (CHAR *) "();");
      FOut_Ln();
    }
    _mod = DEREF(ODepend_Module, _mod, 6239)._next;
  }
  if (STRNEQ(_comName, ""))  {
    FOut_String(3, (CHAR *) "  ");
    OMakeGCC_WriteName(__modName_0, _modName, __comName_0, _comName, 4, (CHAR *) "();");
    FOut_Ln();
  }
  if (_rts)  {
    FOut_String(23, (CHAR *) "  return Rts_exitCode;");
  } else {
    FOut_String(12, (CHAR *) "  return 0;");
  }
  FOut_Ln();
  FOut_String(2, (CHAR *) "}");
  FOut_Close();
}

static INTEGER OMakeGCC_ContainedIn (LONGINT __str_0, const CHAR (* __str_p), LONGINT __prefix_0, const CHAR (* __prefix_p), LONGINT __pattern_0, const CHAR (* __pattern_p)) {
  CHAR (* _str);
  CHAR (* _prefix);
  CHAR (* _pattern);
  CHAR _p[264];
  INTEGER _i, _l;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  VALUE_ARRAY(_prefix, __prefix_p, 1*__prefix_0);
  VALUE_ARRAY(_pattern, __pattern_p, 1*__pattern_0);
  COPY(_prefix, _p, 264);
  Strings_Append(__pattern_0, _pattern, 264, (CHAR *) _p);
  _l = Strings_Length(264, (CHAR *) _p);
  _i = Strings_Pos(264, (CHAR *) _p, __str_0, _str, 0);
  while (_i!=-1&&!((_i==0||(* PTR_INDEX(_str, _i- 1, __str_0, 1, 7088))==' ')&&((* PTR_INDEX(_str, _i+_l, __str_0, 1, 7121))=='\000'||(* PTR_INDEX(_str, _i+_l, __str_0, 1, 7140))==' ')))  {
    _i = Strings_Pos(264, (CHAR *) _p, __str_0, _str, _i+1);
  }
  return _i;
  NO_RETURN (6665);
}

static void OMakeGCC_GenAddObjs (ODepend_Module _depList, LONGINT __str_0, CHAR (* _str)) {
  ODepend_Module _mod;
  COPY("", _str, __str_0);
  _mod = _depList;
  while ((void*)_mod!=(void*)NULL)  {
    if (IN(DEREF(ODepend_Module, _mod, 7631)._extClass, 0x0000000CU, 7642)&&OMakeGCC_ContainedIn(__str_0, _str, 1, (CHAR *) "", 256, (CHAR *) DEREF(ODepend_Module, _mod, 7715)._extName)<0)  {
      Strings2_AppendChar(' ', __str_0, _str);
      Strings_Append(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 7799)._file, OGenGCC_objFile, 5, 7805), __str_0, _str);
    }
    _mod = DEREF(ODepend_Module, _mod, 7852)._next;
  }
}

static void OMakeGCC_GetLinkerFlags (ODepend_Module _depList, LONGINT __str_0, CHAR (* _str), BOOLEAN (* _err)) {
  ODepend_Module _mod, _libList, _lib;
  ODepend_Import _import;
  CHAR _errLib[256];
  INTEGER _pos;

  ODepend_Module _FindModule (LONGINT __libName_0, const CHAR (* __libName_p)) {
    CHAR (* _libName);
    ODepend_Module _mod;
    VALUE_ARRAY(_libName, __libName_p, 1*__libName_0);
    _mod = _libList;
    while ((void*)_mod!=(void*)NULL&&STRNEQ(DEREF(ODepend_Module, _mod, 8609)._name, _libName))  {
      _mod = DEREF(ODepend_Module, _mod, 8648)._next;
    }
    return _mod;
    NO_RETURN (8387);
  }

  void _AddImport (LONGINT __libName_0, const CHAR (* __libName_p), LONGINT __importName_0, const CHAR (* __importName_p)) {
    CHAR (* _libName);
    CHAR (* _importName);
    ODepend_Module _lib, _ilib;
    ODepend_Import _import;
    VALUE_ARRAY(_libName, __libName_p, 1*__libName_0);
    VALUE_ARRAY(_importName, __importName_p, 1*__importName_0);
    if (((* PTR_INDEX(_libName, 0, __libName_0, 1, 8987))!='\000'&&(* PTR_INDEX(_importName, 0, __importName_0, 1, 9010))!='\000')&&STRNEQ(_libName, _importName))  {
      _lib = _FindModule(__libName_0, _libName);
      _ilib = _FindModule(__importName_0, _importName);
      _import = DEREF(ODepend_Module, _lib, 9193)._import;
      while ((void*)_import!=(void*)NULL)  {
        if (DEREF(ODepend_Import, _import, 9255)._module==_ilib)  {
          return;
        }
        _import = DEREF(ODepend_Import, _import, 9366)._next;
      }
      NEWREC(_import, td_ODepend_ImportDesc, 0);
      DEREF(ODepend_Import, _import, 9459)._next = DEREF(ODepend_Module, _lib, 9472)._import;
      DEREF(ODepend_Import, _import, 9496)._module = _ilib;
      DEREF(ODepend_Module, _lib, 9525)._import = _import;
    }
  }
  COPY(OMakeGCC_ldflags, _str, __str_0);
  _mod = _depList;
  _libList = (ODepend_Module)NULL;
  while ((void*)_mod!=(void*)NULL)  {
    if ((DEREF(ODepend_Module, _mod, 9738)._extClass==OGenGCC_extLibrary&&INDEX(DEREF(ODepend_Module, _mod, 9782)._extName, 0, 256, 9791)!='\000')&&(void*)_FindModule(256, (CHAR *) DEREF(ODepend_Module, _mod, 9828)._extName)==(void*)NULL)  {
      NEWREC(_lib, td_ODepend_ModuleDesc, 0);
      ASSERT_TYPE(TAG(TD_ODepend_ModuleDesc, _lib, 9941), td_ODepend_ModuleDesc, 9941);
      DEREF(ODepend_Module, _lib, 9941) = DEREF(ODepend_Module, _mod, 9949);
      COPY(DEREF(ODepend_Module, _lib, 9969)._extName, DEREF(ODepend_Module, _lib, 9983)._name, 98);
      DEREF(ODepend_Module, _lib, 10003)._import = (ODepend_Import)NULL;
      DEREF(ODepend_Module, _lib, 10031)._next = _libList;
      _libList = _lib;
    }
    _mod = DEREF(ODepend_Module, _mod, 10100)._next;
  }
  if ((void*)_libList!=(void*)NULL)  {
    _mod = _depList;
    while ((void*)_mod!=(void*)NULL)  {
      if (DEREF(ODepend_Module, _mod, 10257)._extClass==OGenGCC_extLibrary)  {
        _import = DEREF(ODepend_Module, _mod, 10336)._import;
        while ((void*)_import!=(void*)NULL)  {
          if (DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 10402)._module, 10410)._extClass==OGenGCC_extLibrary)  {
            _AddImport(256, (CHAR *) DEREF(ODepend_Module, _mod, 10499)._extName, 256, (CHAR *) DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 10516)._module, 10524)._extName);
          }
          _import = DEREF(ODepend_Import, _import, 10580)._next;
        }
      }
      _mod = DEREF(ODepend_Module, _mod, 10632)._next;
    }
    _libList = ODepend_TopSort(_libList, TRUE, 256, (CHAR *) _errLib);
    if ((void*)_libList==(void*)NULL)  {
      Out_String(33, (CHAR *) "Error: Cyclic import of library ");
      Out_String(256, (CHAR *) _errLib);
      Out_String(31, (CHAR *) " -- can't create linker flags.");
      Out_Ln();
      (* _err) = TRUE;
    } else {
      _mod = _libList;
      while ((void*)_mod!=(void*)NULL)  {
        if (OMakeGCC_ContainedIn(__str_0, _str, 3, (CHAR *) "-l", 98, (CHAR *) DEREF(ODepend_Module, _mod, 11275)._name)<0)  {
          Strings_Append(4, (CHAR *) " -l", __str_0, _str);
          Strings_Append(98, (CHAR *) DEREF(ODepend_Module, _mod, 11357)._name, __str_0, _str);
        }
        _mod = DEREF(ODepend_Module, _mod, 11405)._next;
      }
    }
  }
  if (OMakeGCC_ContainedIn(__str_0, _str, 3, (CHAR *) "-l", 2, (CHAR *) "m")<0)  {
    Strings_Append(5, (CHAR *) " -lm", __str_0, _str);
  }
  _pos = OMakeGCC_ContainedIn(__str_0, _str, 3, (CHAR *) "-l", 2, (CHAR *) "c");
  if (_pos>=0)  {
    Strings_Delete(__str_0, _str, _pos- 1, 4);
  }
}

static void OMakeGCC_LinkWithGC (void) {
  CHAR _full[256];
  CHAR _file[256];
  CHAR _path[256];
  if (OMachine_generateCodeForGC)  {
    Strings_Append(6, (CHAR *) " -DGC", 256, (CHAR *) OMakeGCC_cflags);
    Filenames_ExpandPath(256, (CHAR *) _full, 18, (CHAR *) OMachine_gcLibPath);
    Filenames_GetPath(256, (CHAR *) _full, 256, (CHAR *) _path, 256, (CHAR *) _file);
    if (STREQL(_file, "gc.a"))  {
      Strings2_AppendChar(' ', 256, (CHAR *) OMakeGCC_ldflags);
      Strings_Append(256, (CHAR *) _full, 256, (CHAR *) OMakeGCC_ldflags);
    } else {
      if (STRNEQ(_full, ""))  {
        Strings_Append(4, (CHAR *) " -L", 256, (CHAR *) OMakeGCC_ldflags);
        Strings_Append(256, (CHAR *) _full, 256, (CHAR *) OMakeGCC_ldflags);
      }
      Strings_Append(6, (CHAR *) " -lgc", 256, (CHAR *) OMakeGCC_ldflags);
    }
  }
}

void OMakeGCC_Build (LONGINT __modName_0, const CHAR (* __modName_p), LONGINT __comName_0, const CHAR (* __comName_p), ODepend_Module _depList, BOOLEAN _makeAll, BOOLEAN (* _err)) {
  CHAR (* _modName);
  CHAR (* _comName);
  ODepend_Module _mod;
  CHAR _com[8192];
  CHAR _appStr[8192];
  CHAR _str[8192];
  CHAR _exec[256];
  ODepend_Module _sup, _main;
  BOOLEAN _found, _compile;
  ODepend_Import _import;

  void _RunCC (ODepend_Module _mod) {
    if ((void*)_mod!=(void*)NULL&&!IN(OGenGCC_cFile, DEREF(ODepend_Module, _mod, 14005)._flags, 13999))  {
      Out_String(23, (CHAR *) "Error: Can't generate ");
      Out_String(98, (CHAR *) DEREF(ODepend_Module, _mod, 14089)._name);
      Out_String(29, (CHAR *) ".o - no C sources available.");
      Out_Ln();
      (* _err) = TRUE;
    } else {
      ODepend_NewFile(_mod, OGenGCC_objFile, 2, (CHAR *) "o");
      COPY(OMakeGCC_cc, _com, 8192);
      Strings_Append(256, (CHAR *) OMakeGCC_cflags, 8192, (CHAR *) _com);
      Strings_Append(GET_LEN(OMakeGCC_inclPaths, 0), CHECK_NIL(CHAR (* ), OMakeGCC_inclPaths, 14330), 8192, (CHAR *) _com);
      Strings_Append(5, (CHAR *) " -o ", 8192, (CHAR *) _com);
      Strings_Append(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 14396)._file, OGenGCC_objFile, 5, 14402), 8192, (CHAR *) _com);
      Strings_Append(5, (CHAR *) " -c ", 8192, (CHAR *) _com);
      Strings_Append(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 14480)._file, OGenGCC_cFile, 5, 14486), 8192, (CHAR *) _com);
      OScan_VerboseMsg(8192, (CHAR *) _com);
      (* _err) = Rts_System((CHAR *) _com)!=0;
      INCL(DEREF(ODepend_Module, _mod, 14589)._flags, OGenGCC_objFile, 14580);
    }
  }
  VALUE_ARRAY(_modName, __modName_p, 1*__modName_0);
  VALUE_ARRAY(_comName, __comName_p, 1*__comName_0);
  OMakeGCC_LinkWithGC();
  NEWREC(_sup, td_ODepend_ModuleDesc, 0);
  COPYSTRING(&(DEREF(ODepend_Module, _sup, 14724)._name), "_OGCC");
  DEREF(ODepend_Module, _sup, 14750)._flags = 0x00000000U;
  if (!ODepend_FindFile(_sup, OGenGCC_header, 2, (CHAR *) "h"))  {
    Out_String(32, (CHAR *) "Error: Can't find file _OGCC.h.");
    Out_Ln();
    (* _err) = TRUE;
    return;
  }
  _found = ODepend_FindFile(_sup, OGenGCC_cFile, 2, (CHAR *) "c");
  _found = ODepend_FindFile(_sup, OGenGCC_objFile, 2, (CHAR *) "o");
  if (((_makeAll||!IN(OGenGCC_objFile, DEREF(ODepend_Module, _sup, 15127)._flags, 15121))||Time_Cmp(INDEX(DEREF(ODepend_Module, _sup, 15160)._time, OGenGCC_header, 5, 15166), INDEX(DEREF(ODepend_Module, _sup, 15183)._time, OGenGCC_objFile, 5, 15189))<0)||Time_Cmp(INDEX(DEREF(ODepend_Module, _sup, 15233)._time, OGenGCC_cFile, 5, 15239), INDEX(DEREF(ODepend_Module, _sup, 15255)._time, OGenGCC_objFile, 5, 15261))<0)  {
    _RunCC(_sup);
    if ((* _err))  {
      return;
    }
  }
  _mod = _depList;
  while ((void*)_mod!=(void*)NULL&&!(* _err))  {
    if (IN(ODepend_flExternal, DEREF(ODepend_Module, _mod, 15627)._flags, 15621)&&DEREF(ODepend_Module, _mod, 15642)._extClass!=OGenGCC_extCSource)  {
      _compile = FALSE;
    } else {
      _compile = ((!ODepend_FindFile(_mod, OGenGCC_objFile, 2, (CHAR *) "o")||Time_Cmp(INDEX(DEREF(ODepend_Module, _mod, 16089)._time, OGenGCC_header, 5, 16095), INDEX(DEREF(ODepend_Module, _mod, 16112)._time, OGenGCC_objFile, 5, 16118))<0)||Time_Cmp(INDEX(DEREF(ODepend_Module, _mod, 16165)._time, OGenGCC_cFile, 5, 16171), INDEX(DEREF(ODepend_Module, _mod, 16187)._time, OGenGCC_objFile, 5, 16193))<0)||Time_Cmp(INDEX(DEREF(ODepend_Module, _sup, 16240)._time, OGenGCC_header, 5, 16246), INDEX(DEREF(ODepend_Module, _mod, 16263)._time, OGenGCC_objFile, 5, 16269))<0;
      if (!_compile)  {
        _import = DEREF(ODepend_Module, _mod, 16441)._import;
        while ((void*)_import!=(void*)NULL)  {
          if (Time_Cmp(INDEX(DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 16517)._module, 16525)._time, OGenGCC_header, 5, 16531), INDEX(DEREF(ODepend_Module, _mod, 16575)._time, OGenGCC_objFile, 5, 16581))<0)  {
            _compile = TRUE;
          }
          _import = DEREF(ODepend_Import, _import, 16681)._next;
        }
      }
    }
    if (_compile)  {
      _RunCC(_mod);
    }
    _mod = DEREF(ODepend_Module, _mod, 16884)._next;
  }
  _main = (ODepend_Module)NULL;
  if (!(* _err))  {
    NEWREC(_main, td_ODepend_ModuleDesc, 0);
    OMakeGCC_GenMainModule(_main, __modName_0, _modName, __comName_0, _comName, _depList);
    _RunCC(_main);
  }
  if (!(* _err))  {
    Strings_Delete(98, (CHAR *) DEREF(ODepend_Module, _main, 17356)._name, 0, 1);
    Redir_GeneratePath(OMachine_redir, 98, (CHAR *) DEREF(ODepend_Module, _main, 17410)._name, 256, (CHAR *) _exec);
    COPY(OMakeGCC_cc, _com, 8192);
    Strings_Append(256, (CHAR *) OMakeGCC_cflags, 8192, (CHAR *) _com);
    Strings_Append(5, (CHAR *) " -o ", 8192, (CHAR *) _com);
    Strings_Append(256, (CHAR *) _exec, 8192, (CHAR *) _com);
    _mod = _depList;
    while ((void*)_mod!=(void*)NULL)  {
      if (DEREF(ODepend_Module, _mod, 17609)._extClass!=OGenGCC_extLibrary)  {
        Strings_Append(2, (CHAR *) " ", 8192, (CHAR *) _com);
        Strings_Append(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 17701)._file, OGenGCC_objFile, 5, 17707), 8192, (CHAR *) _com);
      }
      _mod = DEREF(ODepend_Module, _mod, 17758)._next;
    }
    OMakeGCC_GenAddObjs(_depList, 8192, (CHAR *) _str);
    Strings_Append(8192, (CHAR *) _str, 8192, (CHAR *) _com);
    Strings2_AppendChar(' ', 8192, (CHAR *) _com);
    Strings_Append(256, (CHAR *) INDEX(DEREF(ODepend_Module, _sup, 17897)._file, OGenGCC_objFile, 5, 17903), 8192, (CHAR *) _com);
    Strings2_AppendChar(' ', 8192, (CHAR *) _com);
    Strings_Append(256, (CHAR *) INDEX(DEREF(ODepend_Module, _main, 17984)._file, OGenGCC_objFile, 5, 17990), 8192, (CHAR *) _com);
    OMakeGCC_GetLinkerFlags(_depList, 8192, (CHAR *) _appStr, _err);
    if (!(* _err))  {
      Strings_Append(8192, (CHAR *) _appStr, 8192, (CHAR *) _com);
      OScan_VerboseMsg(8192, (CHAR *) _com);
      (* _err) = Rts_System((CHAR *) _com)!=0;
    }
  }
}

void OMakeGCC_Help (void) {
  Out_String(75, (CHAR *) "--redir <file>     Use <file> as redirection table (default is ~/.o2c.red)");
  Out_Ln();
  Out_String(46, (CHAR *) "--makefile <file>  Write makefile into <file>");
  Out_Ln();
  Out_String(38, (CHAR *) "-a                 Disable assertions");
  Out_Ln();
  Out_String(42, (CHAR *) "-R                 Disable runtime checks");
  Out_Ln();
  Out_String(60, (CHAR *) "-O                 Optimize code (set -O2 when calling gcc)");
  Out_Ln();
  Out_String(63, (CHAR *) "-g                 Include debug information (on C level, -g3)");
  Out_Ln();
  Out_String(36, (CHAR *) "-s                 Strip executable");
  Out_Ln();
  Out_String(60, (CHAR *) "-p                 Include profiler information (for gprof)");
  Out_Ln();
  Out_String(64, (CHAR *) "--cflags <string>  Pass <string> as parameter to the C compiler");
  Out_Ln();
  Out_String(60, (CHAR *) "--ldflags <string> Pass <string> as parameter to the linker");
  Out_Ln();
}

BOOLEAN OMakeGCC_Option (LONGINT __arg_0, const CHAR (* __arg_p)) {
  CHAR (* _arg);
  VALUE_ARRAY(_arg, __arg_p, 1*__arg_0);
  if (STREQL(_arg, "-h")||STREQL(_arg, "--help"))  {
    OMakeGCC_Help();
  } else if (STREQL(_arg, "-O"))  {
    Strings_Append(5, (CHAR *) " -O2", 256, (CHAR *) OMakeGCC_cflags);
  } else if (STREQL(_arg, "-g"))  {
    Strings_Append(5, (CHAR *) " -g3", 256, (CHAR *) OMakeGCC_cflags);
  } else if (STREQL(_arg, "-s"))  {
    Strings_Append(8, (CHAR *) " -Wl,-s", 256, (CHAR *) OMakeGCC_ldflags);
  } else if (STREQL(_arg, "-p"))  {
    Strings_Append(5, (CHAR *) " -pg", 256, (CHAR *) OMakeGCC_cflags);
  } else if (STREQL(_arg, "-R"))  {
    Strings_Append(15, (CHAR *) " -DDISABLE_RTC", 256, (CHAR *) OMakeGCC_cflags);
  } else if (STREQL(_arg, "-a"))  {
    Strings_Append(18, (CHAR *) " -DDISABLE_ASSERT", 256, (CHAR *) OMakeGCC_cflags);
  } else if (STREQL(_arg, "-C"))  {
    OMakeGCC_makeOberon = FALSE;
  } else {
    return FALSE;
  }
  return TRUE;
  NO_RETURN (19153);
}

BOOLEAN OMakeGCC_OptionExt (LONGINT __arg0_0, const CHAR (* __arg0_p), LONGINT __arg1_0, const CHAR (* __arg1_p)) {
  CHAR (* _arg0);
  CHAR (* _arg1);
  VALUE_ARRAY(_arg0, __arg0_p, 1*__arg0_0);
  VALUE_ARRAY(_arg1, __arg1_p, 1*__arg1_0);
  if (STREQL(_arg0, "-cc"))  {
    COPY(_arg1, OMakeGCC_cc, 48);
  } else if (STREQL(_arg0, "--cflags"))  {
    Strings2_AppendChar(' ', 256, (CHAR *) OMakeGCC_cflags);
    Strings_Append(__arg1_0, _arg1, 256, (CHAR *) OMakeGCC_cflags);
  } else if (STREQL(_arg0, "--ldflags"))  {
    Strings2_AppendChar(' ', 256, (CHAR *) OMakeGCC_ldflags);
    Strings_Append(__arg1_0, _arg1, 256, (CHAR *) OMakeGCC_ldflags);
  } else if (STREQL(_arg0, "--redir"))  {
    COPY(_arg1, OMakeGCC_redirFile, 256);
  } else if (STREQL(_arg0, "--makefile"))  {
    COPY(_arg1, OMakeGCC_makefile, 256);
  } else {
    return FALSE;
  }
  return TRUE;
  NO_RETURN (20020);
}

void OMakeGCC_Makefile (LONGINT __modName_0, const CHAR (* __modName_p), LONGINT __comName_0, const CHAR (* __comName_p), LONGINT __o2opt_0, const CHAR (* __o2opt_p), BOOLEAN (* _err)) {
  CHAR (* _modName);
  CHAR (* _comName);
  CHAR (* _o2opt);
  ODepend_Module _depList, _mod, _sup, _main;
  ODepend_Import _inode;
  CHAR _path[256];
  CHAR _str[8192];
  CHAR _appStr[8192];

  void _WriteODepend (ODepend_Module _mod, ODepend_Import _import, BOOLEAN _depH) {
    FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 21143)._file, OGenGCC_objFile, 5, 21149));
    FOut_String(3, (CHAR *) ": ");
    FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _sup, 21213)._file, OGenGCC_header, 5, 21219));
    if (_depH)  {
      FOut_Char(' ');
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 21302)._file, OGenGCC_header, 5, 21308));
    }
    if ((void*)_import==(void*)NULL&&!_depH)  {
      FOut_String(11, (CHAR *) " $(HFILES)");
    } else {
      _inode = _import;
      while ((void*)_inode!=(void*)NULL)  {
        FOut_Char(' ');
        FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, DEREF(ODepend_Import, _inode, 21527)._module, 21535)._file, OGenGCC_header, 5, 21541));
        _inode = DEREF(ODepend_Import, _inode, 21580)._next;
      }
    }
    FOut_Char(' ');
    FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 21655)._file, OGenGCC_cFile, 5, 21661));
    FOut_Ln();
    FOut_Char(CharInfo_ht);
    FOut_String(32, (CHAR *) "$(CC) $(CPPFLAGS) $(CFLAGS) -o ");
    FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 21798)._file, OGenGCC_objFile, 5, 21804));
    FOut_String(5, (CHAR *) " -c ");
    FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 21870)._file, OGenGCC_cFile, 5, 21876));
    FOut_Ln();
  }
  VALUE_ARRAY(_modName, __modName_p, 1*__modName_0);
  VALUE_ARRAY(_comName, __comName_p, 1*__comName_0);
  VALUE_ARRAY(_o2opt, __o2opt_p, 1*__o2opt_0);
  _depList = ODepend_Dependencies(__modName_0, _modName, _err);
  if ((* _err))  {
    return;
  }
  OMakeGCC_LinkWithGC();
  NEWREC(_sup, td_ODepend_ModuleDesc, 0);
  DEREF(ODepend_Module, _sup, 22111)._flags = 0x00000000U;
  DEREF(ODepend_Module, _sup, 22133)._import = (ODepend_Import)NULL;
  COPYSTRING(&(DEREF(ODepend_Module, _sup, 22157)._name), "_OGCC");
  if (!ODepend_FindFile(_sup, OGenGCC_header, 2, (CHAR *) "h")||!ODepend_FindFile(_sup, OGenGCC_cFile, 2, (CHAR *) "c"))  {
    Out_String(50, (CHAR *) "Error: Can't locate _OGCC.h or _OGCC.c, aborting.");
    Out_Ln();
    (* _err) = TRUE;
    return;
  }
  ODepend_NewFile(_sup, OGenGCC_objFile, 2, (CHAR *) "o");
  _mod = _depList;
  while ((void*)_mod!=(void*)NULL)  {
    OMakeGCC_CompilationHook(_mod);
    if (!IN(ODepend_flModExists, DEREF(ODepend_Module, _mod, 22612)._flags, 22606))  {
      ODepend_NewFile(_mod, ODepend_flModExists, 4, (CHAR *) OMachine_moduleExtension);
    }
    ODepend_NewFile(_mod, ODepend_flSymExists, 5, (CHAR *) OMachine_symbolExtension);
    ODepend_NewFile(_mod, OGenGCC_header, 2, (CHAR *) "h");
    if (DEREF(ODepend_Module, _mod, 22820)._extClass!=OGenGCC_extCSource||!IN(OGenGCC_cFile, DEREF(ODepend_Module, _mod, 22870)._flags, 22864))  {
      ODepend_NewFile(_mod, OGenGCC_cFile, 2, (CHAR *) "c");
    }
    ODepend_NewFile(_mod, OGenGCC_objFile, 2, (CHAR *) "o");
    _mod = DEREF(ODepend_Module, _mod, 23078)._next;
  }
  NEWREC(_main, td_ODepend_ModuleDesc, 0);
  OMakeGCC_GenMainModule(_main, __modName_0, _modName, __comName_0, _comName, _depList);
  ODepend_NewFile(_main, OGenGCC_objFile, 2, (CHAR *) "o");
  OScan_VerboseMsg(18, (CHAR *) "Writing Makefile.");
  Redir_GeneratePath(OMachine_redir, 256, (CHAR *) OMakeGCC_makefile, 256, (CHAR *) _path);
  FOut_Open(256, (CHAR *) _path);
  if (OMakeGCC_makeOberon)  {
    FOut_String(10, (CHAR *) "O2C = o2c");
    FOut_Ln();
    FOut_String(9, (CHAR *) "O2OPT = ");
    FOut_String(__o2opt_0, _o2opt);
    FOut_Ln();
  }
  FOut_String(14, (CHAR *) "CC = gcc -m32");
  FOut_Ln();
  FOut_String(9, (CHAR *) "CFLAGS =");
  FOut_String(256, (CHAR *) OMakeGCC_cflags);
  FOut_String(GET_LEN(OMakeGCC_inclPaths, 0), CHECK_NIL(CHAR (* ), OMakeGCC_inclPaths, 23691));
  FOut_Ln();
  FOut_String(10, (CHAR *) "LDFLAGS =");
  OMakeGCC_GetLinkerFlags(_depList, 8192, (CHAR *) _appStr, _err);
  FOut_String(8192, (CHAR *) _appStr);
  FOut_Ln();
  if (OMakeGCC_makeOberon)  {
    FOut_String(11, (CHAR *) "SYMFILES =");
    _mod = _depList;
    while ((void*)_mod!=(void*)NULL)  {
      FOut_Char(' ');
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 24030)._file, ODepend_flSymExists, 5, 24036));
      _mod = DEREF(ODepend_Module, _mod, 24074)._next;
    }
    FOut_Ln();
  }
  FOut_String(9, (CHAR *) "HFILES =");
  _mod = _depList;
  while ((void*)_mod!=(void*)NULL)  {
    FOut_Char(' ');
    FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 24309)._file, OGenGCC_header, 5, 24315));
    _mod = DEREF(ODepend_Module, _mod, 24346)._next;
  }
  FOut_Ln();
  FOut_String(10, (CHAR *) "CFILES0 =");
  _mod = _depList;
  while ((void*)_mod!=(void*)NULL)  {
    if (!IN(ODepend_flExternal, DEREF(ODepend_Module, _mod, 24616)._flags, 24610))  {
      FOut_Char(' ');
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 24680)._file, OGenGCC_cFile, 5, 24686));
    }
    _mod = DEREF(ODepend_Module, _mod, 24726)._next;
  }
  FOut_Ln();
  FOut_String(21, (CHAR *) "CFILES1 = $(CFILES0)");
  _mod = _depList;
  while ((void*)_mod!=(void*)NULL)  {
    if (DEREF(ODepend_Module, _mod, 24896)._extClass==OGenGCC_extCSource)  {
      FOut_Char(' ');
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 24980)._file, OGenGCC_cFile, 5, 24986));
    }
    _mod = DEREF(ODepend_Module, _mod, 25026)._next;
  }
  FOut_Ln();
  FOut_String(9, (CHAR *) "OBJS0 = ");
  FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _sup, 25154)._file, OGenGCC_objFile, 5, 25160));
  FOut_Char(' ');
  FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _main, 25218)._file, OGenGCC_objFile, 5, 25224));
  _mod = _depList;
  while ((void*)_mod!=(void*)NULL)  {
    if (!IN(ODepend_flExternal, DEREF(ODepend_Module, _mod, 25317)._flags, 25311)||DEREF(ODepend_Module, _mod, 25342)._extClass==OGenGCC_extCSource)  {
      FOut_Char(' ');
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 25425)._file, OGenGCC_objFile, 5, 25431));
    }
    _mod = DEREF(ODepend_Module, _mod, 25473)._next;
  }
  FOut_Ln();
  FOut_String(8, (CHAR *) "OBJS1 =");
  OMakeGCC_GenAddObjs(_depList, 8192, (CHAR *) _str);
  FOut_String(8192, (CHAR *) _str);
  FOut_Ln();
  FOut_Ln();
  FOut_String(16, (CHAR *) "# final targets");
  FOut_Ln();
  if (OMakeGCC_makeOberon)  {
    FOut_String(18, (CHAR *) "all: oberon build");
  } else {
    FOut_String(11, (CHAR *) "all: build");
  }
  FOut_Ln();
  FOut_String(25, (CHAR *) "build: $(OBJS0) $(OBJS1)");
  FOut_Ln();
  FOut_Char(CharInfo_ht);
  FOut_String(20, (CHAR *) "$(CC) $(CFLAGS) -o ");
  OMakeGCC_WriteName(__modName_0, _modName, __comName_0, _comName, 1, (CHAR *) "");
  FOut_String(30, (CHAR *) " $(OBJS0) $(OBJS1) $(LDFLAGS)");
  FOut_Ln();
  if (OMakeGCC_makeOberon)  {
    FOut_String(41, (CHAR *) "oberon: $(SYMFILES) $(HFILES) $(CFILES1)");
    FOut_Ln();
  }
  FOut_Ln();
  FOut_String(40, (CHAR *) "# clean will remove all generated files");
  FOut_Ln();
  FOut_String(8, (CHAR *) "clean: ");
  FOut_Ln();
  FOut_Char(CharInfo_ht);
  if (OMakeGCC_makeOberon)  {
    FOut_String(48, (CHAR *) "-rm $(SYMFILES) $(HFILES) $(CFILES0) $(OBJS0) _");
  } else {
    FOut_String(36, (CHAR *) "-rm $(HFILES) $(CFILES0) $(OBJS0) _");
  }
  OMakeGCC_WriteName(__modName_0, _modName, __comName_0, _comName, 1, (CHAR *) "");
  FOut_Ln();
  FOut_Ln();
  if (OMakeGCC_makeOberon)  {
    _mod = _depList;
    while ((void*)_mod!=(void*)NULL)  {
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 26758)._file, ODepend_flSymExists, 5, 26764));
      FOut_Char(' ');
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 26833)._file, OGenGCC_header, 5, 26839));
      FOut_Char(' ');
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 26903)._file, OGenGCC_cFile, 5, 26909));
      FOut_String(3, (CHAR *) ": ");
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 26975)._file, ODepend_flModExists, 5, 26981));
      _inode = DEREF(ODepend_Module, _mod, 27021)._import;
      while ((void*)_inode!=(void*)NULL)  {
        FOut_Char(' ');
        FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, DEREF(ODepend_Import, _inode, 27117)._module, 27125)._file, ODepend_flSymExists, 5, 27131));
        _inode = DEREF(ODepend_Import, _inode, 27175)._next;
      }
      FOut_Ln();
      FOut_Char(CharInfo_ht);
      FOut_String(17, (CHAR *) "$(O2C) $(O2OPT) ");
      FOut_String(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 27311)._file, ODepend_flModExists, 5, 27317));
      FOut_Ln();
      _mod = DEREF(ODepend_Module, _mod, 27372)._next;
    }
    FOut_Ln();
  }
  _WriteODepend(_sup, (ODepend_Import)NULL, TRUE);
  _mod = _depList;
  while ((void*)_mod!=(void*)NULL)  {
    if (!IN(ODepend_flExternal, DEREF(ODepend_Module, _mod, 27575)._flags, 27569)||DEREF(ODepend_Module, _mod, 27591)._extClass==OGenGCC_extCSource)  {
      _WriteODepend(_mod, DEREF(ODepend_Module, _mod, 27655)._import, TRUE);
    }
    _mod = DEREF(ODepend_Module, _mod, 27698)._next;
  }
  _WriteODepend(_main, (ODepend_Import)NULL, FALSE);
  FOut_Ln();
  FOut_Close();
  (* _err) = FALSE;
}

void OMakeGCC_Init (void) {
  Redir_Pattern _pattern;
  Redir_Path _path;
  CHAR _incl[8192];

  BOOLEAN _MatchesHeader (LONGINT __pattern_0, const CHAR (* __pattern_p)) {
    CHAR (* _pattern);
    INTEGER _i, _len;
    CHAR _suffix[256];
    VALUE_ARRAY(_pattern, __pattern_p, 1*__pattern_0);
    _len = Strings_Length(__pattern_0, _pattern);
    _i = _len- 1;
    while (_i>=0&&(* PTR_INDEX(_pattern, _i, __pattern_0, 1, 28203))!='.')  {
      DECI(_i, 1, 28225);
    }
    if (_i>=0)  {
      Strings_Extract(__pattern_0, _pattern, _i, 32767, 256, (CHAR *) _suffix);
      if (Strings2_Match(256, (CHAR *) _suffix, 3, (CHAR *) ".h"))  {
        return TRUE;
      }
    }
    _i = _len- 1;
    while (_i>=0)  {
      if ((* PTR_INDEX(_pattern, _i, __pattern_0, 1, 28602))=='*'||(* PTR_INDEX(_pattern, _i, __pattern_0, 1, 28624))=='?')  {
        Strings_Extract(__pattern_0, _pattern, _i, 32767, 256, (CHAR *) _suffix);
        if (Strings2_Match(256, (CHAR *) _suffix, 3, (CHAR *) ".h"))  {
          return TRUE;
        }
      }
      DECI(_i, 1, 28805);
    }
    return FALSE;
    NO_RETURN (27934);
  }
  OMachine_redir = Redir_Read(256, (CHAR *) OMakeGCC_redirFile);
  OScan_ReadErrorList();
  COPYSTRING(&(_incl), "");
  _pattern = OMachine_redir;
  while ((void*)_pattern!=(void*)NULL)  {
    if (_MatchesHeader(32, (CHAR *) DEREF(Redir_Pattern, _pattern, 29090)._wildcard))  {
      _path = DEREF(Redir_Pattern, _pattern, 29130)._paths;
      while ((void*)_path!=(void*)NULL)  {
        Strings_Append(10, (CHAR *) " -iquote ", 8192, (CHAR *) _incl);
        Strings_Append(256, (CHAR *) DEREF(Redir_Path, _path, 29231)._path, 8192, (CHAR *) _incl);
        _path = DEREF(Redir_Path, _path, 29268)._next;
      }
    }
    _pattern = DEREF(Redir_Pattern, _pattern, 29322)._next;
  }
  /*
  if (INDEX(_incl, 0, 8192, 29350)!='\000')  {
    Strings_Append(5, (CHAR *) " -I-", 8192, (CHAR *) _incl);
  }*/
  NEWDYN(OMakeGCC_inclPaths, 1*Strings_Length(8192, (CHAR *) _incl)+1, 1, 1);
  SET_LEN(0, Strings_Length(8192, (CHAR *) _incl)+1);
  COPY(_incl, CHECK_NIL(CHAR (* ), OMakeGCC_inclPaths, 29690), GET_LEN(OMakeGCC_inclPaths, 0));
}

void _init_OMakeGCC (void) {
  moduleId = add_module ("OMakeGCC");
  COPYSTRING(&(OMakeGCC_redirFile), "~/.o2c.red");
  OMakeGCC_makeOberon = TRUE;
  COPYSTRING(&(OMakeGCC_cc), "gcc -m32");
  COPYSTRING(&(OMakeGCC_cflags), "");
  COPYSTRING(&(OMakeGCC_ldflags), "");
  OMakeGCC_inclPaths = (CHAR (* ))NULL;
  COPYSTRING(&(OMakeGCC_makefile), "");
}
