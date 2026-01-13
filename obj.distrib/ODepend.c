#include "ODepend.h"
#include "Dos.h"
#include "OMachine.h"
#include "Out.h"
#include "Redir.h"
#include "OScan.h"
#include "Strings.h"
#include "Strings2.h"
#include "Time.h"

static ModuleId moduleId;



static const SHORTINT ODepend_numFiles = 5;
const SHORTINT ODepend_flModExists = 0;
const SHORTINT ODepend_flSymExists = 1;
const SHORTINT ODepend_flBE0Exists = 2;
const SHORTINT ODepend_flBE1Exists = 3;
const SHORTINT ODepend_flBE2Exists = 4;
const SHORTINT ODepend_flSymChanged = 5;
const SHORTINT ODepend_flExternal = 6;
static const SHORTINT ODepend_flScanError = 7;
const SHORTINT ODepend_flNoSources = 8;
static const SHORTINT ODepend_flScanned = 9;
const SHORTINT ODepend_flCompile = 10;
const SHORTINT ODepend_flFirstFree = 11;



BOOLEAN ODepend_FindFilename (ODepend_Module _mod, SHORTINT _index, LONGINT __name_0, const CHAR (* __name_p)) {
  CHAR (* _name);
  BOOLEAN _err, _found;
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  _found = Redir_FindPath(OMachine_redir, __name_0, _name, 256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 3467)._file, _index, 5, 3473));
  if (_found)  {
    INCL(DEREF(ODepend_Module, _mod, 3516)._flags, _index, 3507);
    Dos_GetDate((CHAR *) INDEX(DEREF(ODepend_Module, _mod, 3555)._file, _index, 5, 3561), &(INDEX(DEREF(ODepend_Module, _mod, 3573)._time, _index, 5, 3579)), &(_err));
  } else {
    Time_Reset(&(INDEX(DEREF(ODepend_Module, _mod, 3623)._time, _index, 5, 3629)));
  }
  return _found;
  NO_RETURN (2795);
}

BOOLEAN ODepend_FindFile (ODepend_Module _mod, SHORTINT _index, LONGINT __ext_0, const CHAR (* __ext_p)) {
  CHAR (* _ext);
  CHAR _fname[256];
  VALUE_ARRAY(_ext, __ext_p, 1*__ext_0);
  COPY(DEREF(ODepend_Module, _mod, 4362)._name, _fname, 256);
  Strings2_AppendChar('.', 256, (CHAR *) _fname);
  Strings_Append(__ext_0, _ext, 256, (CHAR *) _fname);
  return ODepend_FindFilename(_mod, _index, 256, (CHAR *) _fname);
  NO_RETURN (3695);
}

void ODepend_NewFile (ODepend_Module _mod, SHORTINT _index, LONGINT __ext_0, const CHAR (* __ext_p)) {
  CHAR (* _ext);
  VALUE_ARRAY(_ext, __ext_p, 1*__ext_0);
  Redir_GeneratePathExt(OMachine_redir, 98, (CHAR *) DEREF(ODepend_Module, _mod, 4974)._name, __ext_0, _ext, 256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 4990)._file, _index, 5, 4996));
  Time_GetSysTime(&(INDEX(DEREF(ODepend_Module, _mod, 5030)._time, _index, 5, 5036)));
}

ODepend_Module ODepend_TopSort (ODepend_Module _modules, BOOLEAN _mainFirst, LONGINT __errName_0, CHAR (* _errName)) {
  ODepend_Module _mod;
  ODepend_Import _inode;
  ODepend_Module _topList;

  ODepend_Module _RemoveBest (ODepend_Module (* _mod), INTEGER _prevBest) {
    ODepend_Module _best;
    if ((void*)(* _mod)==(void*)NULL)  {
      return NULL;
    } else if (DEREF(ODepend_Module, (* _mod), 6415)._count<_prevBest)  {
      _best = _RemoveBest(&(DEREF(ODepend_Module, _modules, 6475)._next), DEREF(ODepend_Module, (* _mod), 6486)._count);
      if ((void*)_best==(void*)NULL)  {
        _best = (* _mod);
        (* _mod) = DEREF(ODepend_Module, (* _mod), 6568)._next;
      }
      return _best;
    } else {
      return _RemoveBest(&(DEREF(ODepend_Module, (* _mod), 6649)._next), _prevBest);
    }
    NO_RETURN (5866);
  }
  _mod = _modules;
  while ((void*)_mod!=(void*)NULL)  {
    DEREF(ODepend_Module, _mod, 6818)._count = 0;
    _inode = DEREF(ODepend_Module, _mod, 6850)._import;
    while ((void*)_inode!=(void*)NULL)  {
      INCI(DEREF(ODepend_Module, _mod, 6905)._count, 1, 6897);
      _inode = DEREF(ODepend_Import, _inode, 6937)._next;
    }
    _mod = DEREF(ODepend_Module, _mod, 6971)._next;
  }
  _topList = (ODepend_Module)NULL;
  while ((void*)_modules!=(void*)NULL)  {
    _mod = _RemoveBest(&(_modules), 32767);
    DEREF(ODepend_Module, _mod, 7157)._next = _topList;
    _topList = _mod;
    if (DEREF(ODepend_Module, _topList, 7215)._count!=0)  {
      COPY(DEREF(ODepend_Module, _topList, 7281)._name, _errName, __errName_0);
      return NULL;
    }
    _mod = _modules;
    while ((void*)_mod!=(void*)NULL)  {
      _inode = DEREF(ODepend_Module, _mod, 7451)._import;
      while ((void*)_inode!=(void*)NULL)  {
        if (DEREF(ODepend_Import, _inode, 7511)._module==_topList)  {
          DECI(DEREF(ODepend_Module, _mod, 7556)._count, 1, 7548);
        }
        _inode = DEREF(ODepend_Import, _inode, 7604)._next;
      }
      _mod = DEREF(ODepend_Module, _mod, 7642)._next;
    }
  }
  if (_mainFirst)  {
    return _topList;
  } else {
    while ((void*)_topList!=(void*)NULL)  {
      _mod = _topList;
      _topList = DEREF(ODepend_Module, _topList, 7888)._next;
      DEREF(ODepend_Module, _mod, 7907)._next = _modules;
      _modules = _mod;
    }
    return _modules;
  }
  NO_RETURN (5073);
}

ODepend_Module ODepend_Dependencies (LONGINT __modName_0, CHAR (* _modName), BOOLEAN (* _err)) {
  ODepend_Module _modules, _mod;
  BOOLEAN _external;
  CHAR _errFile[98];

  void _Error (LONGINT __msg_0, const CHAR (* __msg_p), LONGINT __file_0, const CHAR (* __file_p)) {
    CHAR (* _msg);
    CHAR (* _file);
    VALUE_ARRAY(_msg, __msg_p, 1*__msg_0);
    VALUE_ARRAY(_file, __file_p, 1*__file_0);
    Out_String(8, (CHAR *) "Error: ");
    Out_String(__msg_0, _msg);
    Out_String(5, (CHAR *) " in ");
    Out_String(__file_0, _file);
    Out_Ln();
    (* _err) = TRUE;
  }

  ODepend_Module _AddModule (LONGINT __name_0, const CHAR (* __name_p)) {
    CHAR (* _name);
    ODepend_Module _mod;
    VALUE_ARRAY(_name, __name_p, 1*__name_0);
    _mod = _modules;
    while ((void*)_mod!=(void*)NULL&&STRNEQ(DEREF(ODepend_Module, _mod, 9963)._name, _name))  {
      _mod = DEREF(ODepend_Module, _mod, 9999)._next;
    }
    if ((void*)_mod==(void*)NULL)  {
      if ((void*)_modules==(void*)NULL)  {
        COPY(_name, _modName, __modName_0);
      }
      NEWREC(_mod, td_ODepend_ModuleDesc, 0);
      DEREF(ODepend_Module, _mod, 10277)._next = _modules;
      COPY(_name, DEREF(ODepend_Module, _mod, 10319)._name, 98);
      COPYSTRING(&(DEREF(ODepend_Module, _mod, 10339)._extName), "");
      DEREF(ODepend_Module, _mod, 10367)._extClass = 0;
      DEREF(ODepend_Module, _mod, 10395)._import = (ODepend_Import)NULL;
      DEREF(ODepend_Module, _mod, 10423)._flags = 0x00000000U;
      _modules = _mod;
    }
    return _mod;
    NO_RETURN (9457);
  }

  void _ScanModule (ODepend_Module _mod) {
    CHAR _importName[48];
    ODepend_Import _inode;
    BOOLEAN _found, _ferr;

    void _CheckSym (SHORTINT _s) {
      INTEGER _errNum;
      if (_s!=OScan_sym)  {
        if (_s==OScan_ident)  {
          _errNum = 100;
        } else if (_s==OScan_module)  {
          _errNum = 101;
        } else {
          _errNum = 100+(INTEGER) _s;
        }
        OScan_Err(-1, _errNum);
        INCL(DEREF(ODepend_Module, _mod, 11769)._flags, ODepend_flScanError, 11760);
        (* _err) = TRUE;
      }
      OScan_GetSym();
    }
    _found = ODepend_FindFile(_mod, ODepend_flModExists, 4, (CHAR *) OMachine_moduleExtension);
    if (_found)  {
      OScan_Init(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 12052)._file, 0, 5, 12058), 780, &(_ferr));
    }
    if (!_found||_ferr)  {
      INCL(DEREF(ODepend_Module, _mod, 12167)._flags, ODepend_flNoSources, 12158);
    } else {
      COPY(OScan_sourceName, INDEX(DEREF(ODepend_Module, _mod, 12348)._file, 0, 5, 12354), 256);
      OScan_GetSym();
      _external = OScan_sym==OScan_ident&&STREQL(OScan_ref, "EXTERNAL");
      if (_external)  {
        INCL(DEREF(ODepend_Module, _mod, 12485)._flags, ODepend_flExternal, 12476);
        OScan_GetSym();
      }
      _CheckSym(OScan_module);
      if (OScan_sym==OScan_ident)  {
        COPY(OScan_ref, DEREF(ODepend_Module, _mod, 12689)._name, 98);
      }
      _CheckSym(OScan_ident);
      if (OScan_sym==OScan_lBrak)  {
        INCL(DEREF(ODepend_Module, _mod, 12856)._flags, ODepend_flExternal, 12847);
        OScan_GetSym();
        _CheckSym(OScan_string);
        _CheckSym(OScan_rBrak);
        if (OScan_sym!=OScan_ident||STRNEQ(OScan_ref, "EXTERNAL"))  {
          OScan_Err(-1, 102);
          INCL(DEREF(ODepend_Module, _mod, 13069)._flags, ODepend_flScanError, 13060);
          (* _err) = TRUE;
        }
        _CheckSym(OScan_ident);
        _CheckSym(OScan_lBrak);
        COPY(OScan_ref, DEREF(ODepend_Module, _mod, 13217)._extName, 256);
        _CheckSym(OScan_string);
        _CheckSym(OScan_rBrak);
      } else {
        COPYSTRING(&(DEREF(ODepend_Module, _mod, 13315)._extName), "");
      }
      _CheckSym(OScan_semicolon);
      if (OScan_sym==OScan_import)  {
        do  {
          OScan_GetSym();
          COPY(OScan_ref, _importName, 48);
          _CheckSym(OScan_ident);
          if (OScan_sym==OScan_becomes)  {
            OScan_GetSym();
            COPY(OScan_ref, _importName, 48);
            _CheckSym(OScan_ident);
          }
          if (OScan_noerr&&!IN(ODepend_flScanError, DEREF(ODepend_Module, _mod, 13809)._flags, 13803))  {
            if (STRNEQ(_importName, "SYSTEM"))  {
              NEWREC(_inode, td_ODepend_ImportDesc, 0);
              DEREF(ODepend_Import, _inode, 13957)._next = DEREF(ODepend_Module, _mod, 13970)._import;
              DEREF(ODepend_Import, _inode, 14001)._module = _AddModule(48, (CHAR *) _importName);
              DEREF(ODepend_Module, _mod, 14057)._import = _inode;
            }
          } else {
            INCL(DEREF(ODepend_Module, _mod, 14168)._flags, ODepend_flScanError, 14159);
          }
        } while (!(IN(ODepend_flScanError, DEREF(ODepend_Module, _mod, 14285)._flags, 14279)||OScan_sym!=OScan_comma));
        _CheckSym(OScan_semicolon);
      }
      OScan_Close();
    }
    INCL(DEREF(ODepend_Module, _mod, 14404)._flags, ODepend_flScanned, 14395);
  }
  (* _err) = FALSE;
  _modules = (ODepend_Module)NULL;
  _ScanModule(_AddModule(__modName_0, _modName));
  if (IN(ODepend_flNoSources, DEREF(ODepend_Module, _modules, 14623)._flags, 14613))  {
    Out_String(31, (CHAR *) "Error: Can't find main module ");
    Out_String(__modName_0, _modName);
    Out_String(5, (CHAR *) ".Mod");
    Out_Ln();
    (* _err) = TRUE;
  } else {
    while(1) {
      _mod = _modules;
      while ((void*)_mod!=(void*)NULL&&IN(ODepend_flScanned, DEREF(ODepend_Module, _mod, 14910)._flags, 14904))  {
        _mod = DEREF(ODepend_Module, _mod, 14942)._next;
      }
      if ((void*)_mod==(void*)NULL||(* _err))  {
        goto _exit14792;
      } else {
        _ScanModule(_mod);
      }
    }
    _exit14792:
    if (!(* _err))  {
      _modules = ODepend_TopSort(_modules, FALSE, 98, (CHAR *) _errFile);
      if ((void*)_modules==(void*)NULL)  {
        _Error(14, (CHAR *) "Cyclic import", 98, (CHAR *) _errFile);
      }
    }
  }
  return _modules;
  NO_RETURN (8020);
}

void _init_ODepend (void) {
  moduleId = add_module ("ODepend");
  td_ODepend_ModuleDesc = create_td("ModuleDesc", sizeof(ODepend_ModuleDesc), sizeof(TD_ODepend_ModuleDesc), NULL, 0);
  td_ODepend_ImportDesc = create_td("ImportDesc", sizeof(ODepend_ImportDesc), sizeof(TD_ODepend_ImportDesc), NULL, 0);
}
