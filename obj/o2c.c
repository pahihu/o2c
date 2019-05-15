#include "o2c.h"
#include "OGenGCC.h"
#include "OMakeGCC.h"
#include "Dos.h"
#include "Files.h"
#include "ODepend.h"
#include "OEParse.h"
#include "OMachine.h"
#include "OParse.h"
#include "OScan.h"
#include "OSym.h"
#include "OTable.h"
#include "Out.h"
#include "Redir.h"
#include "Rts.h"
#include "Strings.h"
#include "Strings2.h"

static ModuleId moduleId;



static const SHORTINT o2c_make = 0;
static const SHORTINT o2c_makeall = 1;
static const CHAR o2c_version[] = "1.16";
static const CHAR o2c_date[] = "Fri Mar 28 13:15:30 MET 1997";
static BOOLEAN o2c_nocom;
static SET o2c_makeval;
static CHAR o2c_prgname[256];
static CHAR o2c_options[1024];



static void o2c_Compile (LONGINT __fileName_0, const CHAR (* __fileName_p), BOOLEAN (* _err)) {
  CHAR (* _fileName);
  OEParse_Node _root;
  BOOLEAN _found;
  ODepend_Module _mod;
  VALUE_ARRAY(_fileName, __fileName_p, 1*__fileName_0);
  if (!Dos_Exists(_fileName))  {
    (* _err) = !Redir_FindPath(OMachine_redir, __fileName_0, _fileName, __fileName_0, _fileName);
  }
  OScan_Init(__fileName_0, _fileName, OScan_maxBufferSize, _err);
  if (!(* _err))  {
    OParse_Module(&(_root));
    OScan_Close();
    if (OScan_noerr)  {
      _mod = OMakeGCC_ModuleInfo(__fileName_0, _fileName);
      _found = ODepend_FindFile(_mod, ODepend_flSymExists, 5, (CHAR *) OMachine_symbolExtension);
      OSym_Export(_mod, FALSE);
      OGenGCC_Module(_mod, _root);
    }
    OTable_RecycleMem(&(OTable_compiledModule));
    OEParse_RecycleMem(&(_root));
    (* _err) = !OScan_noerr;
  }
}

static void o2c_Make (LONGINT __modName_0, const CHAR (* __modName_p), LONGINT __comName_0, const CHAR (* __comName_p), BOOLEAN _makeAll, BOOLEAN (* _err)) {
  CHAR (* _modName);
  CHAR (* _comName);
  OTable_Object _object, _module;
  ODepend_Module _depList, _dep;
  ODepend_Import _import;
  BOOLEAN _found;

  void _Compile (ODepend_Module _mod, BOOLEAN _forceNewSym, BOOLEAN (* _err)) {
    OEParse_Node _root;
    CHAR _msg[512];
    if (!IN(ODepend_flModExists, DEREF(ODepend_Module, _mod, 1873)._flags, 1867))  {
      Out_String(19, (CHAR *) "Error: Can't find ");
      Out_String(98, (CHAR *) DEREF(ODepend_Module, _mod, 1954)._name);
      Out_Char('.');
      Out_String(4, (CHAR *) OMachine_moduleExtension);
      Out_Ln();
      (* _err) = TRUE;
      return;
    }
    Rts_GetArg(0, (CHAR *) _msg);
    Strings2_AppendChar(' ', 512, (CHAR *) _msg);
    Strings_Append(1024, (CHAR *) o2c_options, 512, (CHAR *) _msg);
    Strings_Append(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 2262)._file, ODepend_flModExists, 5, 2267), 512, (CHAR *) _msg);
    OScan_VerboseMsg(512, (CHAR *) _msg);
    OScan_Init(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 2408)._file, ODepend_flModExists, 5, 2413), OScan_maxBufferSize, _err);
    if (!(* _err))  {
      OParse_Module(&(_root));
      OScan_Close();
      if (OScan_noerr)  {
        OSym_Export(_mod, _forceNewSym);
        OGenGCC_Module(_mod, _root);
      }
      OTable_RecycleMem(&(OTable_compiledModule));
      OEParse_RecycleMem(&(_root));
      (* _err) = !OScan_noerr;
    }
  }
  VALUE_ARRAY(_modName, __modName_p, 1*__modName_0);
  VALUE_ARRAY(_comName, __comName_p, 1*__comName_0);
  _depList = ODepend_Dependencies(__modName_0, _modName, _err);
  if (!(* _err))  {
    _dep = _depList;
    while ((void*)_dep!=(void*)NULL&&!(* _err))  {
      OMakeGCC_CompilationHook(_dep);
      if (_makeAll)  {
        INCL(DEREF(ODepend_Module, _dep, 3189)._flags, ODepend_flCompile, 3180);
      } else {
        _found = ODepend_FindFile(_dep, ODepend_flSymExists, 5, (CHAR *) OMachine_symbolExtension);
        if (!_found)  {
          INCL(DEREF(ODepend_Module, _dep, 3415)._flags, ODepend_flCompile, 3406);
        }
        if (IN(ODepend_flNoSources, DEREF(ODepend_Module, _dep, 3498)._flags, 3492))  {
          Out_String(31, (CHAR *) "Error: Can't find source file ");
          Out_String(98, (CHAR *) DEREF(ODepend_Module, _dep, 3629)._name);
          Out_String(5, (CHAR *) ".Mod");
          Out_Ln();
          (* _err) = TRUE;
        }
        if (IN(ODepend_flModExists, DEREF(ODepend_Module, _dep, 3770)._flags, 3764))  {
          if ((!(* _err)&&!IN(ODepend_flCompile, DEREF(ODepend_Module, _dep, 3969)._flags, 3963))&&OMakeGCC_NeedCompile(_dep))  {
            INCL(DEREF(ODepend_Module, _dep, 4035)._flags, ODepend_flCompile, 4026);
          }
          if (!(* _err)&&!IN(ODepend_flCompile, DEREF(ODepend_Module, _dep, 4202)._flags, 4196))  {
            _import = DEREF(ODepend_Module, _dep, 4243)._import;
            while ((void*)_import!=(void*)NULL)  {
              if (IN(ODepend_flSymChanged, DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 4341)._module, 4349)._flags, 4332))  {
                INCL(DEREF(ODepend_Module, _dep, 4390)._flags, ODepend_flCompile, 4381);
              }
              _import = DEREF(ODepend_Import, _import, 4471)._next;
            }
          }
        }
      }
      if (!(* _err)&&IN(ODepend_flCompile, DEREF(ODepend_Module, _dep, 4667)._flags, 4661))  {
        _Compile(_dep, _makeAll, _err);
      }
      _dep = DEREF(ODepend_Module, _dep, 4750)._next;
    }
    if (!(* _err)&&STRNEQ(_comName, ""))  {
      _module = OSym_GetImport(__modName_0, _modName, 1, (CHAR *) "");
      if ((void*)_module!=(void*)NULL)  {
        _object = OTable_FindImport(__comName_0, _comName, _module);
      }
      if (((void*)_module==(void*)NULL||(void*)_object==(void*)NULL)||DEREF(OTable_Struct, DEREF(OTable_Object, _object, 5007)._type, 5012)._len!=0)  {
        Out_String(35, (CHAR *) "Error: No parameterless procedure ");
        Out_String(__comName_0, _comName);
        Out_String(14, (CHAR *) " exported by ");
        Out_String(__modName_0, _modName);
        Out_Char('.');
        Out_Ln();
        (* _err) = TRUE;
      }
    }
    if (!(* _err))  {
      OMakeGCC_Build(__modName_0, _modName, __comName_0, _comName, _depList, _makeAll, _err);
    }
  }
}

static void o2c_Help (void) {
  Out_String(256, (CHAR *) o2c_prgname);
  Out_String(45, (CHAR *) " [options] --make [options] module [command]");
  Out_Ln();
  Out_String(256, (CHAR *) o2c_prgname);
  Out_String(18, (CHAR *) " [options] {file}");
  Out_Ln();
  Out_String(8, (CHAR *) "where: ");
  Out_Ln();
  Out_String(46, (CHAR *) "  module : Name of main module to be compiled");
  Out_Ln();
  Out_String(42, (CHAR *) "  command: Name of command to be executed");
  Out_Ln();
  Out_String(39, (CHAR *) "  file   : Name of file to be compiled");
  Out_Ln();
  Out_String(9, (CHAR *) "Options:");
  Out_Ln();
  Out_String(35, (CHAR *) "--help, -h         Print this text");
  Out_Ln();
  Out_String(79, (CHAR *) "--verbose, -v      Print additional information (e.g. steps when running make)");
  Out_Ln();
  Out_String(77, (CHAR *) "--make, -M         Compile modules which have changed, produce an executable");
  Out_Ln();
  Out_String(66, (CHAR *) "--all, -A          Force compilation of all modules (with --make)");
  Out_Ln();
  Out_String(34, (CHAR *) "--warn, -W         Print warnings");
  Out_Ln();
  Out_String(33, (CHAR *) "--version, -V      Print version");
  Out_Ln();
  Out_String(52, (CHAR *) "--usegc / --nogc   Enable/disable use of gc package");
  Out_Ln();
  OMakeGCC_Help();
}

static BOOLEAN o2c_Option (LONGINT __arg_0, const CHAR (* __arg_p)) {
  CHAR (* _arg);
  VALUE_ARRAY(_arg, __arg_p, 1*__arg_0);
  if (STREQL(_arg, "-h")||STREQL(_arg, "--help"))  {
    o2c_Help();
    o2c_nocom = TRUE;
  } else if (STREQL(_arg, "-v")||STREQL(_arg, "--verbose"))  {
    OScan_verbose = TRUE;
  } else if (STREQL(_arg, "-V")||STREQL(_arg, "--version"))  {
    o2c_nocom = TRUE;
    if (OMachine_generateCodeForGC)  {
      Out_String(8, (CHAR *) "o2c/gc ");
    } else {
      Out_String(5, (CHAR *) "o2c ");
    }
    Out_String(5, (CHAR *) o2c_version);
    Out_String(3, (CHAR *) ", ");
    Out_String(29, (CHAR *) o2c_date);
    Out_Ln();
  } else if (STREQL(_arg, "-W")||STREQL(_arg, "--warn"))  {
    OScan_warnings = TRUE;
  } else if (STREQL(_arg, "-M")||STREQL(_arg, "--make"))  {
    INCL(o2c_makeval, o2c_make, 7355);
    return TRUE;
  } else if (STREQL(_arg, "-A")||STREQL(_arg, "--all"))  {
    INCL(o2c_makeval, o2c_makeall, 7448);
    return TRUE;
  } else if (STREQL(_arg, "--nogc"))  {
    OMachine_generateCodeForGC = FALSE;
  } else if (STREQL(_arg, "--usegc"))  {
    OMachine_generateCodeForGC = TRUE;
  } else {
    return FALSE;
  }
  Strings_Append(__arg_0, _arg, 1024, (CHAR *) o2c_options);
  Strings_Append(2, (CHAR *) " ", 1024, (CHAR *) o2c_options);
  return TRUE;
  NO_RETURN (6506);
}

static BOOLEAN o2c_OptionExt (LONGINT __arg0_0, const CHAR (* __arg0_p), LONGINT __arg1_0, const CHAR (* __arg1_p)) {
  CHAR (* _arg0);
  CHAR (* _arg1);
  VALUE_ARRAY(_arg0, __arg0_p, 1*__arg0_0);
  VALUE_ARRAY(_arg1, __arg1_p, 1*__arg1_0);
  return FALSE;
  NO_RETURN (7796);
}

static void o2c_EvalOptions (INTEGER (* _arg)) {
  INTEGER _pos, _len;
  CHAR _opt[3];
  CHAR _argument[512];
  CHAR _extension[512];

  void _GetNextExt (LONGINT __extension_0, CHAR (* _extension)) {
    if ((* _arg)<Rts_ArgNumber())  {
      Rts_GetArg((* _arg)+1, _extension);
    } else {
      COPY("", _extension, __extension_0);
    }
  }

  BOOLEAN _Accept (LONGINT __arg0_0, const CHAR (* __arg0_p), LONGINT __arg1_0, const CHAR (* __arg1_p)) {
    CHAR (* _arg0);
    CHAR (* _arg1);
    VALUE_ARRAY(_arg0, __arg0_p, 1*__arg0_0);
    VALUE_ARRAY(_arg1, __arg1_p, 1*__arg1_0);
    if (!o2c_Option(__arg0_0, _arg0)&&!OMakeGCC_Option(__arg0_0, _arg0))  {
      if (o2c_OptionExt(__arg0_0, _arg0, __arg1_0, _arg1)||OMakeGCC_OptionExt(__arg0_0, _arg0, __arg1_0, _arg1))  {
        INCI((* _arg), 1, 8696);
      } else {
        Out_String(17, (CHAR *) "illegal option: ");
        Out_String(__arg0_0, _arg0);
        Out_Ln();
        INCI((* _arg), 1, 8800);
        return FALSE;
      }
    }
    return TRUE;
    NO_RETURN (8502);
  }
  COPY("- ", _opt, 3);
  (* _arg) = 1;
  while(1) {
    if ((* _arg)>Rts_ArgNumber())  {
      goto _exit8965;
    }
    Rts_GetArg((* _arg), (CHAR *) _argument);
    _GetNextExt(512, (CHAR *) _extension);
    _len = Strings_Length(512, (CHAR *) _argument);
    if (INDEX(_argument, 0, 512, 9141)=='-')  {
      if (INDEX(_argument, 1, 512, 9202)=='-')  {
        if (_Accept(512, (CHAR *) _argument, 512, (CHAR *) _extension))  {
          INCI((* _arg), 1, 9310);
        }
      } else {
        {
          INTEGER _temp_ = _len- 1;
          for(_pos = 1 ; _pos <= _temp_ ; _pos += 1) {
            INDEX(_opt, 1, 3, 9399) = INDEX(_argument, _pos, 512, 9414);
            if (_Accept(3, (CHAR *) _opt, 512, (CHAR *) _extension))  {
            }
          }
        }
        INCI((* _arg), 1, 9504);
      }
    } else {
      goto _exit8965;
    }
  }
  _exit8965: ;
}

void o2c_Run (void) {
  BOOLEAN _error;
  INTEGER _arg;
  CHAR _file[256];
  CHAR _command[256];
  Rts_GetArg(0, (CHAR *) o2c_prgname);
  _arg = 1;
  o2c_nocom = FALSE;
  o2c_makeval = 0x00000000U;
  _error = FALSE;
  if (Rts_ArgNumber()>0)  {
    o2c_EvalOptions(&(_arg));
    OTable_InitTable();
    OMakeGCC_Init();
    if (_arg>Rts_ArgNumber())  {
      if (!o2c_nocom)  {
        Out_String(27, (CHAR *) "Error: Nothing to compile.");
        Out_Ln();
        _error = TRUE;
      }
    } else {
      if (IN(o2c_make, o2c_makeval, 10237)||STRNEQ(OMakeGCC_makefile, ""))  {
        Rts_GetArg(_arg, (CHAR *) _file);
        INCI(_arg, 1, 10342);
        if (_arg<=Rts_ArgNumber())  {
          Rts_GetArg(_arg, (CHAR *) _command);
        } else {
          COPYSTRING(&(_command), "");
        }
        if (STRNEQ(OMakeGCC_makefile, ""))  {
          OMakeGCC_Makefile(256, (CHAR *) _file, 256, (CHAR *) _command, 1024, (CHAR *) o2c_options, &(_error));
        } else {
          o2c_Make(256, (CHAR *) _file, 256, (CHAR *) _command, IN(o2c_makeall, o2c_makeval, 10764), &(_error));
        }
      } else {
        while (_arg<=Rts_ArgNumber()&&!_error)  {
          Rts_GetArg(_arg, (CHAR *) _file);
          o2c_Compile(256, (CHAR *) _file, &(_error));
          INCI(_arg, 1, 10989);
        }
      }
    }
    if (_error)  {
      Rts_exitCode = 1;
    }
  } else {
    o2c_Help();
  }
}

void _init_o2c (void) {
  moduleId = add_module ("o2c");
  Files_permanentClose = TRUE;
  o2c_Run();
}
