#include "OGenGCC.h"
#include "ConvTypes.h"
#include "Filenames.h"
#include "IntStr.h"
#include "OMachine.h"
#include "FOut.h"
#include "Out.h"
#include "RealStr.h"
#include "Redir.h"
#include "Rts.h"
#include "OScan.h"
#include "Strings.h"
#include "Strings2.h"
#include "OTable.h"

static ModuleId moduleId;



static const SHORTINT OGenGCC_flagHeader = 0;
static const SHORTINT OGenGCC_flagSemicolon = 1;
static const SHORTINT OGenGCC_flagTypeDef = 2;
static const SHORTINT OGenGCC_flagVarDef = 3;
static const SHORTINT OGenGCC_flagAutoPrefix = 5;
static const SHORTINT OGenGCC_flagFunctHeader = 6;
static const SHORTINT OGenGCC_flagNoName = 7;
static const SHORTINT OGenGCC_flagExtName = 8;
static const SHORTINT OGenGCC_flagOberonName = 9;
static const SET OGenGCC_arraySet = 0x00060000U;
static const SHORTINT OGenGCC_ofDefined = 16;
static const SHORTINT OGenGCC_sfChecked = 31;
static const SHORTINT OGenGCC_fAddType = 0;
static const SHORTINT OGenGCC_fAddPosition = 1;
static const SHORTINT OGenGCC_fMonadic = 2;
static const SHORTINT OGenGCC_fAddTypename = 3;
static const SHORTINT OGenGCC_fLeftAdr = 4;
static const SHORTINT OGenGCC_fAddSize = 5;
const SHORTINT OGenGCC_header = 2;
const SHORTINT OGenGCC_cFile = 3;
const SHORTINT OGenGCC_objFile = 4;
const CHAR OGenGCC_cExt = 'c';
const CHAR OGenGCC_hExt = 'h';
const CHAR OGenGCC_markerExt = 'm';
const SHORTINT OGenGCC_extCSource = 1;
const SHORTINT OGenGCC_extObject = 2;
const SHORTINT OGenGCC_extArchive = 3;
const SHORTINT OGenGCC_extLibrary = 4;
static OTable_Object OGenGCC_emptyObj;
static BOOLEAN OGenGCC_tdGenerated;
static OEParse_Node OGenGCC_root;
static LONGINT OGenGCC_atCount;
static CHAR OGenGCC_extName[104];



static void OGenGCC_Off (INTEGER _off) {
  if (_off>=0)  {
    FOut_Ln();
    while (_off>0)  {
      FOut_String(3, (CHAR *) "  ");
      DECI(_off, 1, 1840);
    }
  }
}

static void OGenGCC_TBIdent (OTable_Object _obj, LONGINT __name_0, CHAR (* _name)) {
  OTable_Struct _t;
  OTable_GetModuleName(_obj, __name_0, _name);
  Strings2_AppendChar('_', __name_0, _name);
  _t = DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 2056)._type, 2062)._link, 2068)._type;
  if (DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 2087)._type, 2093)._link, 2099)._type, 2105)._form==OTable_strPointer)  {
    Strings_Append(48, (CHAR *) DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OTable_Struct, _t, 2152)._base, 2158)._obj, 2163)._name, __name_0, _name);
  } else {
    Strings_Append(48, (CHAR *) DEREF(OTable_Object, DEREF(OTable_Struct, _t, 2205)._obj, 2210)._name, __name_0, _name);
  }
  Strings2_AppendChar('_', __name_0, _name);
  Strings_Append(48, (CHAR *) DEREF(OTable_Object, _obj, 2289)._name, __name_0, _name);
}

static void OGenGCC_Ident (OTable_Object _obj) {
  CHAR _name[104];
  if (IN(OTable_flagExternal, DEREF(OTable_Object, _obj, 2558)._flags, 2552)&&IN(DEREF(OTable_Object, _obj, 2581)._mode, 0x00000290U, 2588))  {
    if ((void*)DEREF(OTable_Object, _obj, 2651)._extName!=(void*)NULL)  {
      FOut_String(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Object, _obj, 2694)._extName, 2703));
    } else {
      FOut_String(48, (CHAR *) DEREF(OTable_Object, _obj, 2739)._name);
    }
  } else if (((DEREF(OTable_Object, _obj, 2771)._mode==OTable_objType&&DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 2797)._type, 2803)._form<=OTable_strFixedMax)&&DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 2845)._type, 2851)._obj==_obj)||((DEREF(OTable_Object, _obj, 2881)._mode==OTable_objConst&&DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 2908)._type, 2914)._form==OTable_strBool)&&DEREF(OTable_Object, _obj, 2950)._mnolev==-1))  {
    FOut_String(48, (CHAR *) DEREF(OTable_Object, _obj, 3100)._name);
  } else if (DEREF(OTable_Object, _obj, 3122)._mode==OTable_objTBProc)  {
    FOut_String(4, (CHAR *) "TB_");
    OGenGCC_TBIdent(_obj, 104, (CHAR *) _name);
    FOut_String(104, (CHAR *) _name);
  } else {
    if (DEREF(OTable_Object, _obj, 3299)._mode==OTable_objField||DEREF(OTable_Object, _obj, 3328)._mnolev>0)  {
    } else if (DEREF(OTable_Object, _obj, 3377)._mode!=OTable_objModule)  {
      if (DEREF(OTable_Object, _obj, 3419)._mnolev<=-3)  {
        OTable_GetModuleName(_obj, 104, (CHAR *) _name);
        FOut_String(104, (CHAR *) _name);
      } else {
        FOut_String(48, (CHAR *) DEREF(OTable_Object, OTable_compiledModule, 3653)._name);
      }
    }
    FOut_Char('_');
    FOut_String(48, (CHAR *) DEREF(OTable_Object, _obj, 3723)._name);
  }
}

static void OGenGCC_CheckName (OTable_Object _block, OTable_Object (* _obj)) {

  void _CorrectType (OTable_Struct _t, BOOLEAN _export, BOOLEAN _checkStruct) {
    OTable_Object _new;
    CHAR _name[16];

    void _TraverseRecord (OTable_Object _o) {
      if ((void*)_o!=(void*)NULL)  {
        _TraverseRecord(DEREF(OTable_Object, _o, 4217)._next);
        if (DEREF(OTable_Object, _o, 4241)._mode==OTable_objField&&(void*)DEREF(OTable_Struct, DEREF(OTable_Object, _o, 4266)._type, 4272)._obj==(void*)NULL)  {
          _CorrectType(DEREF(OTable_Object, _o, 4316)._type, _export, FALSE);
        }
      }
    }

    void _AssignName (LONGINT __name_0, CHAR (* _name)) {
      COPY("at_", _name, __name_0);
      IntStr_Append(OGenGCC_atCount, 0, ConvTypes_left, __name_0, _name);
      INCLI(OGenGCC_atCount, 1, 4607);
    }
    if (!IN(OGenGCC_sfChecked, DEREF(OTable_Struct, _t, 4683)._flags, 4679)&&(_checkStruct||(void*)DEREF(OTable_Struct, _t, 4712)._obj==(void*)NULL))  {
      INCL(DEREF(OTable_Struct, _t, 4746)._flags, OGenGCC_sfChecked, 4739);
      if (DEREF(OTable_Struct, _t, 4780)._form==OTable_strRecord&&(void*)DEREF(OTable_Struct, _t, 4806)._obj==(void*)NULL)  {
        _AssignName(16, (CHAR *) _name);
        _new = OTable_NewObject(16, (CHAR *) _name, OTable_objType, -1);
        DEREF(OTable_Object, _new, 4983)._type = _t;
        DEREF(OTable_Object, _new, 5019)._next = (* _obj);
        if (_export)  {
          INCL(DEREF(OTable_Object, _new, 5080)._flags, OTable_flagExport, 5071);
        }
        DEREF(OTable_Struct, _t, 5129)._obj = _new;
        (* _obj) = _new;
      } else if (DEREF(OTable_Struct, _t, 5180)._form==OTable_strRecord&&DEREF(OTable_Object, DEREF(OTable_Struct, _t, 5206)._obj, 5211)._mnolev>0)  {
        _AssignName(48, (CHAR *) DEREF(OTable_Object, DEREF(OTable_Struct, _t, 5402)._obj, 5407)._name);
      }
      {
        SHORTINT _temp_ = DEREF(OTable_Struct, _t, 5442)._form;
        switch (_temp_) {
          case 14:
          case 17:
          case 18: {
            if ((void*)DEREF(OTable_Struct, DEREF(OTable_Struct, _t, 5518)._base, 5524)._obj==(void*)NULL)  {
              _CorrectType(DEREF(OTable_Struct, _t, 5568)._base, _export, FALSE);
            }
            break;
          }
          case 19: {
            _TraverseRecord(DEREF(OTable_Struct, _t, 5655)._link);
            break;
          }
          default:  {
        }
        }
      }
    }
  }
  if ((void*)(* _obj)!=(void*)NULL)  {
    OGenGCC_CheckName(_block, &(DEREF(OTable_Object, (* _obj), 5806)._next));
    {
      SHORTINT _temp_ = DEREF(OTable_Object, (* _obj), 5829)._mode;
      switch (_temp_) {
        case 4:
        case 3: {
          if ((void*)DEREF(OTable_Struct, DEREF(OTable_Object, (* _obj), 5883)._type, 5889)._obj==(void*)NULL||DEREF(OTable_Struct, DEREF(OTable_Object, (* _obj), 5909)._type, 5915)._obj==(* _obj))  {
            _CorrectType(DEREF(OTable_Object, (* _obj), 5959)._type, IN(OTable_flagExport, DEREF(OTable_Object, (* _obj), 5986)._flags, 5980), TRUE);
          }
          break;
        }
        case 7:
        case 8:
        case 9:
        case 11: {
          OGenGCC_CheckName((* _obj), &(DEREF(OTable_Object, DEREF(OTable_Object, (* _obj), 6105)._link, 6111)._right));
          break;
        }
        default:  {
      }
      }
    }
  }
}

static void OGenGCC_TypeDescrIdent (OTable_Struct _t, BOOLEAN _variable) {
  if (DEREF(OTable_Struct, _t, 6331)._form==OTable_strPointer)  {
    _t = DEREF(OTable_Struct, _t, 6371)._base;
  }
  if (_variable)  {
    FOut_String(4, (CHAR *) "td_");
  } else {
    FOut_String(4, (CHAR *) "TD_");
  }
  OGenGCC_Ident(DEREF(OTable_Struct, _t, 6493)._obj);
}

static void OGenGCC_GenTagName (OTable_Object _obj) {
  FOut_String(5, (CHAR *) "tag_");
  OGenGCC_Ident(_obj);
}

static void OGenGCC_VarDecl (LONGINT __name_0, const CHAR (* __name_p), OTable_Struct _type);

static OTable_Struct OGenGCC_ReceiverRecord (OTable_Object _obj) {
  OTable_Struct _r;
  _r = DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 7002)._type, 7008)._link, 7014)._type;
  if (DEREF(OTable_Struct, _r, 7031)._form==OTable_strPointer)  {
    return DEREF(OTable_Struct, _r, 7073)._base;
  }
  return _r;
  NO_RETURN (6702);
}

static void OGenGCC_GenTypeDescrForward (OEParse_Node _tdList, BOOLEAN _header) {
  OEParse_Node _td;
  if (!OGenGCC_tdGenerated)  {
    FOut_Ln();
    _td = _tdList;
    while ((void*)_td!=(void*)NULL)  {
      if (IN(OTable_flagExport, DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 7456)._type, 7462)._obj, 7467)._flags, 7451)==_header)  {
        OGenGCC_Off(0);
        FOut_String(8, (CHAR *) "struct ");
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 7570)._type, FALSE);
        FOut_String(2, (CHAR *) ";");
      }
      _td = DEREF(OEParse_Node, _td, 7640)._link;
    }
    FOut_Ln();
  }
}

static void OGenGCC_GenTypeDescr (OEParse_Node _tdList, BOOLEAN _header) {
  OEParse_Node _td;
  INTEGER _tbCount;

  void _GenTBEntries (OTable_Struct _type) {

    void _GenEntry (OTable_Object _o) {
      CHAR _name[52];
      OTable_Object _mr;
      if ((void*)_o!=(void*)NULL)  {
        _GenEntry(DEREF(OTable_Object, _o, 8077)._left);
        if (DEREF(OTable_Object, _o, 8101)._mode==OTable_objTBProc&&((void*)DEREF(OTable_Struct, _type, 8145)._base==(void*)NULL||(void*)OTable_FindField(48, (CHAR *) DEREF(OTable_Object, _o, 8177)._name, DEREF(OTable_Struct, _type, 8189)._base)==(void*)NULL))  {
          _mr = OTable_FindField(48, (CHAR *) DEREF(OTable_Object, _o, 8242)._name, DEREF(OEParse_Node, _td, 8252)._type);
          if ((void*)_mr==(void*)NULL)  {
            _mr = _o;
          }
          COPYSTRING(&(_name), "tb_");
          IntStr_Append((LONGINT) _tbCount, 0, 0, 52, (CHAR *) _name);
          OGenGCC_Off(1);
          OGenGCC_VarDecl(52, (CHAR *) _name, DEREF(OTable_Object, _mr, 8457)._type);
          FOut_String(7, (CHAR *) ";  /* ");
          FOut_String(48, (CHAR *) DEREF(OTable_Object, _mr, 8523)._name);
          FOut_String(4, (CHAR *) " */");
          INCI(_tbCount, 1, 8574);
        }
        _GenEntry(DEREF(OTable_Object, _o, 8624)._right);
      }
    }
    if ((void*)_type!=(void*)NULL)  {
      _GenTBEntries(DEREF(OTable_Struct, _type, 8735)._base);
      _GenEntry(DEREF(OTable_Struct, _type, 8766)._link);
    }
  }
  if (!OGenGCC_tdGenerated)  {
    FOut_Ln();
    _td = _tdList;
    while ((void*)_td!=(void*)NULL)  {
      if (IN(OTable_flagExport, DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 9028)._type, 9034)._obj, 9039)._flags, 9023)==_header)  {
        _tbCount = 0;
        OGenGCC_Off(0);
        FOut_String(16, (CHAR *) "typedef struct ");
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 9174)._type, FALSE);
        FOut_String(3, (CHAR *) " {");
        OGenGCC_Off(1);
        FOut_String(7, (CHAR *) "TDCORE");
        _GenTBEntries(DEREF(OEParse_Node, _td, 9293)._type);
        OGenGCC_Off(0);
        FOut_String(3, (CHAR *) "} ");
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 9376)._type, FALSE);
        FOut_Char(';');
        OGenGCC_Off(0);
        FOut_String(7, (CHAR *) "EXTERN");      /* AP 19.1.2026 patch */
        OGenGCC_Ident(OTable_compiledModule);
        FOut_Char(' ');
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 9471)._type, FALSE);
        FOut_String(3, (CHAR *) "* ");
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 9542)._type, TRUE);
        FOut_Char(';');
      }
      _td = DEREF(OEParse_Node, _td, 9609)._link;
    }
    OGenGCC_tdGenerated = TRUE;
    FOut_Ln();
  }
}

static INTEGER OGenGCC_TypeDescId (OTable_Object _obj) {
  INTEGER _tbCount;

  INTEGER _ScanStruct (OTable_Struct _t) {
    INTEGER _res;

    INTEGER _ScanFields (OTable_Object _o) {
      INTEGER _res;

      BOOLEAN _Redefinition (OTable_Struct _t, OTable_Object _tbProc) {
        OTable_Object _obj;
        if ((void*)_t==(void*)NULL)  {
          return FALSE;
        } else {
          _obj = OTable_SearchInTree(DEREF(OTable_Struct, _t, 10494)._link, 48, (CHAR *) DEREF(OTable_Object, _tbProc, 10508)._name);
          if (((void*)_obj!=(void*)NULL&&DEREF(OTable_Object, _obj, 10550)._mode==OTable_objTBProc)&&(DEREF(OTable_Object, _obj, 10594)._mnolev==DEREF(OTable_Object, _tbProc, 10611)._mnolev||DEREF(OTable_Object, _obj, 10628)._mark!=OTable_exportNot))  {
            return TRUE;
          } else {
            return _Redefinition(DEREF(OTable_Struct, _t, 10920)._base, _tbProc);
          }
        }
        NO_RETURN (10145);
      }
      if ((void*)_o!=(void*)NULL)  {
        _res = _ScanFields(DEREF(OTable_Object, _o, 11067)._left);
        if ((_res<0&&DEREF(OTable_Object, _o, 11103)._mode==OTable_objTBProc)&&!_Redefinition(DEREF(OTable_Struct, _t, 11143)._base, _o))  {
          if (STREQL(DEREF(OTable_Object, _o, 11176)._name, DEREF(OTable_Object, _obj, 11188)._name))  {
            return _tbCount;
          } else {
            INCI(_tbCount, 1, 11261);
          }
        }
        if (_res<0)  {
          _res = _ScanFields(DEREF(OTable_Object, _o, 11366)._right);
        }
        return _res;
      } else {
        return -1;
      }
      NO_RETURN (10058);
    }
    if ((void*)_t!=(void*)NULL)  {
      _res = _ScanStruct(DEREF(OTable_Struct, _t, 11548)._base);
      if (_res<0)  {
        _res = _ScanFields(DEREF(OTable_Struct, _t, 11667)._link);
      }
      return _res;
    } else {
      return -1;
    }
    NO_RETURN (9977);
  }
  _tbCount = 0;
  return _ScanStruct(OGenGCC_ReceiverRecord(_obj));
  NO_RETURN (9736);
}

static void OGenGCC_GenTypeDescrInit (OEParse_Node _tdList) {
  OEParse_Node _td;
  CHAR _name[104];
  BOOLEAN _size;

  void _GenTBInit (OTable_Object _o) {
    if ((void*)_o!=(void*)NULL)  {
      _GenTBInit(DEREF(OTable_Object, _o, 12092)._left);
      if (DEREF(OTable_Object, _o, 12114)._mode==OTable_objTBProc)  {
        OGenGCC_Off(1);
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 12188)._type, TRUE);
        FOut_String(6, (CHAR *) "->tb_");
        FOut_Int((LONGINT) OGenGCC_TypeDescId(_o), 0);
        FOut_String(7, (CHAR *) " = TB_");
        OGenGCC_TBIdent(_o, 104, (CHAR *) _name);
        FOut_String(104, (CHAR *) _name);
        FOut_Char(';');
      }
      _GenTBInit(DEREF(OTable_Object, _o, 12413)._right);
    }
  }
  _td = _tdList;
  while ((void*)_td!=(void*)NULL)  {
    OGenGCC_Off(1);
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 12543)._type, TRUE);
    FOut_String(15, (CHAR *) " = create_td(\"");
    COPY(DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 12607)._type, 12613)._obj, 12618)._name, _name, 104);
    if (INDEX(_name, 2, 104, 12647)=='_'||DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 12664)._type, 12670)._obj, 12675)._scope, 12682)._left, 12688)._mode!=OTable_objModule)  {
      COPYSTRING(&(_name), "");
      _size = FALSE;
    } else {
      _size = TRUE;
    }
    FOut_String(104, (CHAR *) _name);
    FOut_String(4, (CHAR *) "\", ");
    if (_size)  {
      FOut_String(8, (CHAR *) "sizeof(");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 12913)._type, 12919)._obj);
    } else {
      FOut_String(3, (CHAR *) "(0");
    }
    FOut_String(11, (CHAR *) "), sizeof(");
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 13027)._type, FALSE);
    FOut_String(4, (CHAR *) "), ");
    if ((void*)DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 13079)._type, 13085)._base==(void*)NULL)  {
      FOut_String(10, (CHAR *) "NULL, 0);");
    } else {
      OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 13172)._type, 13178)._base, TRUE);
      FOut_String(10, (CHAR *) ", sizeof(");
      OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 13251)._type, 13257)._base, FALSE);
      FOut_String(4, (CHAR *) "));");
    }
    _GenTBInit(DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 13379)._type, 13385)._link);
    _td = DEREF(OEParse_Node, _td, 13408)._link;
  }
}

static INTEGER OGenGCC_DynDimensions (OTable_Struct _t) {
  if (DEREF(OTable_Struct, _t, 13541)._form==OTable_strDynArray)  {
    return OGenGCC_DynDimensions(DEREF(OTable_Struct, _t, 13599)._base)+1;
  } else {
    return 0;
  }
  NO_RETURN (13467);
}

static void OGenGCC_OpenArrayDecl (OTable_Object _o, BOOLEAN _modified) {
  OTable_Struct _base;
  _base = DEREF(OTable_Object, _o, 14372)._type;
  while (DEREF(OTable_Struct, _base, 14395)._form==OTable_strDynArray)  {
    _base = DEREF(OTable_Struct, _base, 14440)._base;
  }
  OGenGCC_Ident(DEREF(OTable_Struct, _base, 14471)._obj);
  FOut_Char(' ');
  if (DEREF(OTable_Struct, DEREF(OTable_Object, _o, 14506)._type, 14512)._form!=OTable_strArray)  {
    FOut_String(4, (CHAR *) "(* ");
  }
  if (_modified)  {
    FOut_Char('_');
    OGenGCC_Ident(_o);
    FOut_String(3, (CHAR *) "_p");
  } else {
    OGenGCC_Ident(_o);
  }
  if (DEREF(OTable_Struct, DEREF(OTable_Object, _o, 14695)._type, 14701)._form!=OTable_strArray)  {
    FOut_Char(')');
  }
}

static OTable_Object OGenGCC_NamedDecl (OTable_Object _o, SET _flags, INTEGER _off) {
  OTable_Object _obj;
  BOOLEAN _right;
  OTable_Struct _base;

  void _FormalPars (OTable_Struct _pars) {
    INTEGER _i;
    OTable_Object _first, _next;
    _first = DEREF(OTable_Struct, _pars, 15827)._link;
    FOut_String(3, (CHAR *) " (");
    if ((void*)_first==(void*)NULL)  {
      FOut_String(5, (CHAR *) "void");
    } else {
      while ((void*)_first!=(void*)NULL)  {
        _next = DEREF(OTable_Object, _first, 16035)._link;
        if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _first, 16062)._type, 16068)._form, OGenGCC_arraySet, 16075))  {
          if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _first, 16270)._type, 16276)._flags|DEREF(OTable_Struct, _pars, 16288)._flags, 16261))  {
            {
              INTEGER _temp_ = OGenGCC_DynDimensions(DEREF(OTable_Object, _first, 16395)._type)- 1;
              for(_i = 0 ; _i <= _temp_ ; _i += 1) {
                FOut_String(9, (CHAR *) "LONGINT ");
                FOut_String(2, (CHAR *) "_");
                OGenGCC_Ident(_first);
                FOut_Char('_');
                FOut_Int((LONGINT) _i, 0);
                FOut_String(3, (CHAR *) ", ");
              }
            }
          }
          if (DEREF(OTable_Object, _first, 16658)._mode==OTable_objVar)  {
            FOut_String(7, (CHAR *) "const ");
          }
          OGenGCC_OpenArrayDecl(_first, DEREF(OTable_Object, _first, 16802)._mode==OTable_objVar&&!OTable_external);
        } else if ((!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _first, 16878)._type, 16884)._flags|DEREF(OTable_Struct, _pars, 16896)._flags, 16869)&&DEREF(OTable_Object, _first, 16931)._mode==OTable_objVarPar)&&DEREF(OTable_Struct, DEREF(OTable_Object, _first, 16978)._type, 16984)._form==OTable_strRecord)  {
          FOut_String(8, (CHAR *) "struct ");
          OGenGCC_TypeDescrIdent(DEREF(OTable_Object, _first, 17147)._type, FALSE);
          FOut_String(3, (CHAR *) "* ");
          OGenGCC_GenTagName(_first);
          FOut_String(3, (CHAR *) ", ");
          _first = OGenGCC_NamedDecl(_first, 0x00000000U, _off);
        } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _first, 17323)._type, 17329)._form==OTable_strNone)  {
          FOut_String(4, (CHAR *) "...");
        } else {
          _first = OGenGCC_NamedDecl(_first, 0x00000000U, _off);
        }
        _first = _next;
        if ((void*)_first!=(void*)NULL)  {
          FOut_String(3, (CHAR *) ", ");
        }
      }
    }
    FOut_Char(')');
  }

  BOOLEAN _Structure (OTable_Struct _t) {
    return ((void*)DEREF(OTable_Struct, _t, 17989)._obj==(void*)NULL||((DEREF(OTable_Object, DEREF(OTable_Struct, _t, 18147)._obj, 18152)._mnolev>=0&&DEREF(OTable_Struct, _t, 18185)._form!=OTable_strRecord)&&!(IN(OGenGCC_ofDefined, DEREF(OTable_Object, DEREF(OTable_Struct, _t, 18242)._obj, 18247)._flags, 18238)||(IN(OTable_flagExport, DEREF(OTable_Object, DEREF(OTable_Struct, _t, 18295)._obj, 18300)._flags, 18291)&&!IN(OGenGCC_flagHeader, _flags, 18324)))))||(IN(OGenGCC_flagTypeDef, _flags, 18436)&&DEREF(OTable_Struct, _t, 18450)._obj==_o);
    NO_RETURN (17771);
  }

  void _TypeLeft (OTable_Struct _t) {
    OTable_Object _first;
    SET _declFlags;
    if ((void*)_t!=(void*)NULL&&!_Structure(_t))  {
      if (DEREF(OTable_Struct, _t, 18795)._form==OTable_strRecord)  {
        if (IN(OTable_flagUnion, DEREF(OTable_Struct, _t, 18924)._flags, 18920))  {
          FOut_String(7, (CHAR *) "union ");
        } else {
          FOut_String(8, (CHAR *) "struct ");
        }
      }
      OGenGCC_Ident(DEREF(OTable_Struct, _t, 19061)._obj);
      FOut_Char(' ');
    } else if ((void*)_t!=(void*)NULL)  {
      {
        SHORTINT _temp_ = DEREF(OTable_Struct, _t, 19219)._form;
        switch (_temp_) {
          case 9: {
            FOut_String(6, (CHAR *) "void ");
            break;
          }
          case 14: {
            _TypeLeft(DEREF(OTable_Struct, _t, 19322)._base);
            if (DEREF(OTable_Struct, DEREF(OTable_Struct, _t, 19346)._base, 19352)._form!=OTable_strDynArray)  {
              FOut_String(4, (CHAR *) "(* ");
            }
            break;
          }
          case 15:
          case 16: {
            _TypeLeft(DEREF(OTable_Struct, _t, 19479)._base);
            if (!IN(OGenGCC_flagFunctHeader, _flags, 19519))  {
              FOut_String(4, (CHAR *) "(* ");
            }
            break;
          }
          case 17: {
            _TypeLeft(DEREF(OTable_Struct, _t, 19620)._base);
            break;
          }
          case 10: {
            FOut_String(6, (CHAR *) "CHAR ");
            break;
          }
          case 18: {
            _base = _t;
            while (DEREF(OTable_Struct, _base, 19747)._form==OTable_strDynArray)  {
              _base = DEREF(OTable_Struct, _base, 19798)._base;
            }
            _TypeLeft(_base);
            FOut_String(4, (CHAR *) "(* ");
            break;
          }
          case 19: {
            if (IN(OTable_flagUnion, DEREF(OTable_Struct, _t, 19927)._flags, 19923))  {
              FOut_String(7, (CHAR *) "union ");
            } else {
              FOut_String(8, (CHAR *) "struct ");
            }
            if ((void*)DEREF(OTable_Struct, _t, 20051)._obj!=(void*)NULL)  {
              OGenGCC_Ident(DEREF(OTable_Struct, _t, 20089)._obj);
            }
            FOut_String(3, (CHAR *) " {");
            if ((void*)DEREF(OTable_Struct, _t, 20154)._base!=(void*)NULL)  {
              OGenGCC_Off(_off+1);
              OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OTable_Struct, _t, 20218)._base, 20224)._obj);
              FOut_String(7, (CHAR *) " base;");
            }
            if (IN(OTable_flagExternal, DEREF(OTable_Struct, _t, 20378)._flags, 20374))  {
              _declFlags = 0x00000200U;
            } else {
              _declFlags = 0x00000000U;
            }
            _first = DEREF(OTable_Struct, _t, 20512)._link;
            while ((void*)_first!=(void*)NULL)  {
              if (DEREF(OTable_Object, _first, 20574)._mode==OTable_objField)  {
                OGenGCC_Off(_off+1);
                _first = OGenGCC_NamedDecl(_first, _declFlags, _off+1);
                FOut_Char(';');
              } else {
                _first = DEREF(OTable_Object, _first, 20806)._next;
              }
            }
            OGenGCC_Off(_off);
            FOut_String(3, (CHAR *) "} ");
            break;
          }
          default: NO_LABEL (_temp_, 19213);
        }
      }
    }
  }

  void _TypeRight (OTable_Struct _t) {
    if ((void*)_t!=(void*)NULL&&_Structure(_t))  {
      {
        SHORTINT _temp_ = DEREF(OTable_Struct, _t, 21328)._form;
        switch (_temp_) {
          case 14: {
            _right = TRUE;
            if (DEREF(OTable_Struct, DEREF(OTable_Struct, _t, 21400)._base, 21406)._form!=OTable_strDynArray)  {
              FOut_Char(')');
            }
            _TypeRight(DEREF(OTable_Struct, _t, 21497)._base);
            break;
          }
          case 15:
          case 16: {
            _right = TRUE;
            if (!IN(OGenGCC_flagFunctHeader, _flags, 21595))  {
              FOut_Char(')');
            }
            _FormalPars(_t);
            break;
          }
          case 10: {
            _right = TRUE;
            FOut_String(3, (CHAR *) "[]");
            break;
          }
          case 17: {
            _right = TRUE;
            FOut_Char('[');
            FOut_Int(DEREF(OTable_Struct, _t, 21838)._len, 0);
            FOut_Char(']');
            _TypeRight(DEREF(OTable_Struct, _t, 21895)._base);
            break;
          }
          case 18: {
            _base = _t;
            while (DEREF(OTable_Struct, _base, 21970)._form==OTable_strDynArray)  {
              _base = DEREF(OTable_Struct, _base, 22021)._base;
            }
            FOut_Char(')');
            _TypeRight(_base);
            break;
          }
          default:  {
        }
        }
      }
    }
  }
  _right = FALSE;
  _TypeLeft(DEREF(OTable_Object, _o, 22243)._type);
  if (DEREF(OTable_Object, _o, 22261)._mode==OTable_objVarPar&&!IN(DEREF(OTable_Struct, DEREF(OTable_Object, _o, 22288)._type, 22294)._form, OGenGCC_arraySet, 22301))  {
    FOut_String(4, (CHAR *) "(* ");
  }
  if (!IN(OGenGCC_flagNoName, _flags, 22423))  {
    if (IN(OGenGCC_flagExtName, _flags, 22460))  {
      FOut_String(104, (CHAR *) OGenGCC_extName);
    } else if (IN(OGenGCC_flagOberonName, _flags, 22530))  {
      FOut_String(48, (CHAR *) DEREF(OTable_Object, _o, 22564)._name);
    } else {
      OGenGCC_Ident(_o);
    }
  }
  if (DEREF(OTable_Object, _o, 22677)._mode==OTable_objVarPar&&!IN(DEREF(OTable_Struct, DEREF(OTable_Object, _o, 22704)._type, 22710)._form, OGenGCC_arraySet, 22717))  {
    FOut_Char(')');
  }
  _TypeRight(DEREF(OTable_Object, _o, 22779)._type);
  _obj = _o;
  if (!_right&&(DEREF(OTable_Object, _o, 22821)._mode==OTable_objVar||DEREF(OTable_Object, _o, 22845)._mode==OTable_objVarPar))  {
    while (((((void*)DEREF(OTable_Object, _obj, 22970)._next!=(void*)NULL&&DEREF(OTable_Object, DEREF(OTable_Object, _obj, 22990)._next, 22996)._mode==DEREF(OTable_Object, _o, 23006)._mode)&&DEREF(OTable_Object, DEREF(OTable_Object, _obj, 23020)._next, 23026)._type==DEREF(OTable_Object, _o, 23036)._type)&&DEREF(OTable_Object, _obj, 23062)._mark==DEREF(OTable_Object, DEREF(OTable_Object, _obj, 23074)._next, 23080)._mark)&&!IN(OTable_flagParam, DEREF(OTable_Object, _obj, 23110)._flags, 23104))  {
      _obj = DEREF(OTable_Object, _obj, 23232)._next;
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Ident(_obj);
    }
  }
  return DEREF(OTable_Object, _obj, 23319)._next;
  NO_RETURN (14786);
}

static void OGenGCC_Const (OTable_Const _c, SHORTINT _form, OTable_Object _obj) {
  INTEGER _i;

  void _CharConst (LONGINT _ch, BOOLEAN _quote) {
    if (_quote)  {
      if (_ch>=127)  {
        FOut_String(7, (CHAR *) "(CHAR)");
      }
      FOut_Char('\047');
    }
    if ((((_ch<32||_ch>=127)||_ch==34)||_ch==39)||_ch==92)  {
      FOut_Char('\134');
      FOut_Char((CHAR) (DIVLI(_ch, 64, 24309)+48));
      FOut_Char((CHAR) (MODLI(DIVLI(_ch, 8, 24354), 8, 24361)+48));
      FOut_Char((CHAR) (MODLI(_ch, 8, 24404)+48));
    } else {
      FOut_Char((CHAR) _ch);
    }
    if (_quote)  {
      FOut_Char('\047');
    }
  }

  void _RealConst (LONGREAL _val, BOOLEAN _long) {
    INTEGER _i;
    CHAR _str[64];
    if (_val==MAX_LONGREAL)  {
      FOut_String(13, (CHAR *) "MAX_LONGREAL");
    } else if (_val==MIN_LONGREAL)  {
      FOut_String(13, (CHAR *) "MIN_LONGREAL");
    } else if (_val==MAX_REAL)  {
      FOut_String(9, (CHAR *) "MAX_REAL");
    } else if (_val==MIN_REAL)  {
      FOut_String(9, (CHAR *) "MIN_REAL");
    } else {
      RealStr_GiveFloat(64, (CHAR *) _str, _val, 17, 0, ConvTypes_left);
      _i = 0;
      while ((INDEX(_str, _i, 64, 25247)!='\000'&&INDEX(_str, _i, 64, 25263)!='E')&&INDEX(_str, _i, 64, 25280)!='.')  {
        INCI(_i, 1, 25304);
      }
      if (INDEX(_str, _i, 64, 25340)!='.')  {
        Strings_Insert(3, (CHAR *) ".0", _i, 64, (CHAR *) _str);
      }
      FOut_String(64, (CHAR *) _str);
      if (!_long)  {
        FOut_Char('F');
      }
    }
  }

  void _SetConst (SET _set) {
    INTEGER _i, _j, _nibble, _bit;
    FOut_String(3, (CHAR *) "0x");
    {
      INTEGER _temp_ = 0;
      for(_i = 28 ; _i >= _temp_ ; _i += -4) {
        _nibble = 0;
        _bit = 1;
        _j = 0;
        while (_j<4)  {
          if (IN(_i+_j, _set, 25838))  {
            INCI(_nibble, _bit, 25863);
          }
          _bit = _bit*2;
          INCI(_j, 1, 25930);
        }
        if (_nibble<10)  {
          FOut_Char((CHAR) (48+_nibble));
        } else {
          FOut_Char((CHAR) (55+_nibble));
        }
      }
    }
    FOut_Char('U');
  }
  if (((void*)_obj!=(void*)NULL&&!IN(OTable_flagExternal, DEREF(OTable_Object, _obj, 26198)._flags, 26192))&&!(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 26222)._type, 26228)._form==OTable_strString&&Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, _obj, 26278)._const, 26285)._string, 26293))==1))  {
    OGenGCC_Ident(_obj);
  } else {
    {
      SHORTINT _temp_ = _form;
      switch (_temp_) {
        case 1: {
          if (DEREF(OTable_Const, _c, 26405)._intval==1)  {
            FOut_String(5, (CHAR *) "TRUE");
          } else {
            FOut_String(6, (CHAR *) "FALSE");
          }
          break;
        }
        case 2: {
          _CharConst(DEREF(OTable_Const, _c, 26543)._intval, TRUE);
          break;
        }
        case 3: {
          FOut_Int(DEREF(OTable_Const, _c, 26746)._intval, 0);
          break;
        }
        case 4: {
          if (DEREF(OTable_Const, _c, 26794)._intval==(-2147483647-1))  {
            FOut_String(3, (CHAR *) "(-");
            FOut_Int(2147483647, 0);
            FOut_String(4, (CHAR *) "-1)");
          } else {
            FOut_Int(DEREF(OTable_Const, _c, 27074)._intval, 0);
          }
          break;
        }
        case 5: {
          if (DEREF(OTable_Const, _c, 27160)._intval==(-9223372036854775807LL-1))  {
            FOut_String(3, (CHAR *) "(-");
            FOut_Int(9223372036854775807LL, 0);
            FOut_String(6, (CHAR *) "LL-1)");
          } else {
            FOut_Int(DEREF(OTable_Const, _c, 27441)._intval, 0);
            FOut_String(3, (CHAR *) "LL");
          }
          break;
        }
        case 6:
        case 7: {
          _RealConst(DEREF(OTable_Const, _c, 26964)._real, _form==OTable_strLongReal);
          break;
        }
        case 8: {
          _SetConst(DEREF(OTable_Const, _c, 27029)._set);
          break;
        }
        case 10: {
          FOut_Char('\042');
          if (Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, _c, 27104)._string, 27112))==1)  {
            _CharConst((LONGINT) ((INTEGER) INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27152)._string, 27160), 0, 256, 27160)), FALSE);
          } else {
            _i = 0;
            while (INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27222)._string, 27230), _i, 256, 27230)!='\000')  {
              if ((INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27260)._string, 27268), _i, 256, 27268)=='\042'||INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27336)._string, 27344), _i, 256, 27344)=='\134')||INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27434)._string, 27442), _i, 256, 27442)=='?')  {
                FOut_Char('\134');
              }
              FOut_Char(INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27586)._string, 27594), _i, 256, 27594));
              INCI(_i, 1, 27612);
            }
          }
          FOut_Char('\042');
          break;
        }
        case 11: {
          FOut_String(5, (CHAR *) "NULL");
          break;
        }
        default: NO_LABEL (_temp_, 26362);
      }
    }
  }
}

static OTable_Object OGenGCC_Decl (OTable_Object _obj, SET _flags, INTEGER _off) {
  const SHORTINT _dontPrint = 0;
  const SHORTINT _extern = 1;
  const SHORTINT _static = 2;
  const SHORTINT _auto = 3;
  const SHORTINT _noModifier = 4;
  OTable_Object _next;
  SHORTINT _modifier;
  if (OTable_external&&DEREF(OTable_Object, _obj, 28629)._mode==OTable_objConst)  {
    _modifier = _dontPrint;
  } else if (IN(OGenGCC_flagAutoPrefix, _flags, 28709))  {
    _modifier = _auto;
  } else if (DEREF(OTable_Object, _obj, 28765)._mnolev>0)  {
    _modifier = _noModifier;
  } else {
    if (IN(DEREF(OTable_Object, _obj, 28849)._mode, 0x00000B94U, 28856))  {
      if (!IN(OTable_flagExport, DEREF(OTable_Object, _obj, 29030)._flags, 29024))  {
        if (IN(OGenGCC_flagHeader, _flags, 29069))  {
          _modifier = _dontPrint;
        } else {
          _modifier = _static;
        }
      } else {
        if (IN(OGenGCC_flagHeader, _flags, 29216))  {
          _modifier = _extern;
        } else {
          _modifier = _noModifier;
        }
      }
    } else {
      if (IN(OTable_flagExport, DEREF(OTable_Object, _obj, 29405)._flags, 29399)!=IN(OGenGCC_flagHeader, _flags, 29428))  {
        _modifier = _dontPrint;
      } else {
        _modifier = _noModifier;
      }
    }
  }
  if (_modifier!=_dontPrint)  {
    if (IN(DEREF(OTable_Object, _obj, 29601)._mode, 0x00000B80U, 29608)&&!OTable_external)  {
      OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 29807)._link, IN(OGenGCC_flagHeader, _flags, 29826));
    }
    OGenGCC_Off(_off);
    if (_modifier==_extern)  {
      FOut_String(8, (CHAR *) "extern ");
    } else if (_modifier==_static)  {
      FOut_String(8, (CHAR *) "static ");
    } else if (_modifier==_auto)  {
      FOut_String(6, (CHAR *) "auto ");
    }
    if (DEREF(OTable_Object, _obj, 30080)._mode==OTable_objConst)  {
      FOut_String(7, (CHAR *) "const ");
      _next = OGenGCC_NamedDecl(_obj, 0x00000000U, _off);
      if (!IN(OGenGCC_flagHeader, _flags, 30230))  {
        FOut_String(4, (CHAR *) " = ");
        OGenGCC_Const(DEREF(OTable_Object, _obj, 30338)._const, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 30350)._type, 30356)._form, (OTable_Object)NULL);
      }
    } else if (DEREF(OTable_Object, _obj, 30397)._mode==OTable_objType)  {
      FOut_String(9, (CHAR *) "typedef ");
      _next = OGenGCC_NamedDecl(_obj, _flags|0x00000004U, _off);
      INCL(DEREF(OTable_Object, _obj, 30552)._flags, OGenGCC_ofDefined, 30543);
    } else if (DEREF(OTable_Object, _obj, 30588)._mode==OTable_objVar)  {
      _next = OGenGCC_NamedDecl(_obj, _flags|0x00000008U, _off);
    } else {
      _next = OGenGCC_NamedDecl(_obj, _flags|0x00000040U, _off);
    }
    if (IN(OGenGCC_flagSemicolon, _flags, 30863))  {
      FOut_Char(';');
    }
    return _next;
  } else {
    return DEREF(OTable_Object, _obj, 30953)._next;
  }
  NO_RETURN (27754);
}

static void OGenGCC_VarDecl (LONGINT __name_0, const CHAR (* __name_p), OTable_Struct _type) {
  CHAR (* _name);
  OTable_Object _dummy;
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  if ((void*)DEREF(OTable_Struct, _type, 31088)._obj!=(void*)NULL)  {
    OGenGCC_Ident(DEREF(OTable_Struct, _type, 31123)._obj);
    FOut_Char(' ');
    FOut_String(__name_0, _name);
  } else {
    DEREF(OTable_Object, OGenGCC_emptyObj, 31196)._type = _type;
    COPY(_name, OGenGCC_extName, 104);
    _dummy = OGenGCC_NamedDecl(OGenGCC_emptyObj, 0x00000100U, -32768);
  }
}

static void OGenGCC_Expr (OEParse_Node _expr, BOOLEAN _paren);

static void OGenGCC_Designator (OEParse_Node _d, BOOLEAN _adr);

static void OGenGCC_ArrayLength (OEParse_Node _desig, LONGINT _dim) {
  INTEGER _ddim;
  OTable_Struct _type;
  _ddim = OGenGCC_DynDimensions(DEREF(OEParse_Node, _desig, 31638)._type);
  if ((LONGINT) _ddim<=_dim)  {
    _type = DEREF(OEParse_Node, _desig, 31745)._type;
    while (_dim!=0)  {
      _type = DEREF(OTable_Struct, _type, 31798)._base;
      DECLI(_dim, 1, 31814);
    }
    FOut_Int(DEREF(OTable_Struct, _type, 31852)._len, 0);
  } else if (DEREF(OEParse_Node, _desig, 31878)._class==OEParse_ndVarPar||DEREF(OEParse_Node, _desig, 31909)._class==OEParse_ndVar)  {
    FOut_String(2, (CHAR *) "_");
    OGenGCC_Ident(DEREF(OEParse_Node, _desig, 32021)._obj);
    FOut_Char('_');
    FOut_Int(_dim, 0);
  } else {
    FOut_String(9, (CHAR *) "GET_LEN(");
    while (DEREF(OTable_Struct, DEREF(OEParse_Node, _desig, 32171)._type, 32177)._form==OTable_strDynArray)  {
      _desig = DEREF(OEParse_Node, _desig, 32226)._left;
    }
    OGenGCC_Designator(_desig, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(_dim, 0);
    FOut_Char(')');
  }
}

static void OGenGCC_GetArrayPrefix (OEParse_Node _d, OEParse_Node (* _prefix), INTEGER (* _dim)) {
  (* _dim) = -1;
  (* _prefix) = _d;
  while (DEREF(OEParse_Node, (* _prefix), 32824)._class==OEParse_ndIndex)  {
    INCI((* _dim), 1, 32854);
    (* _prefix) = DEREF(OEParse_Node, (* _prefix), 32887)._left;
  }
}

static void OGenGCC_GenTypeTag (OEParse_Node _n) {
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 33022)._type, 33028)._form==OTable_strPointer)  {
    FOut_String(5, (CHAR *) "TAG(");
    OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 33119)._type, 33125)._base, FALSE);
    FOut_String(3, (CHAR *) ", ");
    OGenGCC_Designator(_n, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, _n, 33230)._pos, 0);
    FOut_Char(')');
  } else if (DEREF(OEParse_Node, _n, 33272)._class==OEParse_ndVarPar)  {
    OGenGCC_GenTagName(DEREF(OEParse_Node, _n, 33351)._obj);
  } else if (DEREF(OEParse_Node, _n, 33370)._class==OEParse_ndDeref)  {
    FOut_String(5, (CHAR *) "TAG(");
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _n, 33487)._type, FALSE);
    FOut_String(3, (CHAR *) ", ");
    OGenGCC_Designator(DEREF(OEParse_Node, _n, 33545)._left, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, _n, 33598)._pos, 0);
    FOut_Char(')');
  } else {
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _n, 33681)._type, TRUE);
  }
}

static void OGenGCC_GenTypeLevel (OTable_Struct _type) {
  INTEGER _level;
  FOut_String(3, (CHAR *) ", ");
  if (DEREF(OTable_Struct, _type, 33878)._form==OTable_strPointer)  {
    _type = DEREF(OTable_Struct, _type, 33924)._base;
  }
  _level = -1;
  do  {
    INCI(_level, 1, 33974);
    _type = DEREF(OTable_Struct, _type, 34005)._base;
  } while (!((void*)_type==(void*)NULL));
  FOut_Int((LONGINT) _level, 0);
}

static void OGenGCC_GenTypeTest (OEParse_Node _expr, OTable_Struct _type) {
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 34159)._type, 34165)._form==OTable_strPointer)  {
    FOut_String(15, (CHAR *) "type_test_ptr(");
    OGenGCC_Expr(_expr, FALSE);
  } else {
    FOut_String(11, (CHAR *) "type_test(");
    OGenGCC_GenTypeTag(_expr);
  }
  FOut_String(3, (CHAR *) ", ");
  OGenGCC_TypeDescrIdent(_type, TRUE);
  OGenGCC_GenTypeLevel(_type);
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 34417)._type, 34423)._form==OTable_strPointer)  {
    FOut_String(13, (CHAR *) ", moduleId, ");
    FOut_Int(DEREF(OEParse_Node, _expr, 34501)._pos, 0);
  }
  FOut_Char(')');
}

static void OGenGCC_TypeDesign (OTable_Struct _type, BOOLEAN _arrayToPtr) {
  OTable_Object _dummy;
  BOOLEAN _toPointer;
  _toPointer = _arrayToPtr&&IN(DEREF(OTable_Struct, _type, 34716)._form, OGenGCC_arraySet, 34723);
  while (IN(DEREF(OTable_Struct, _type, 34752)._form, OGenGCC_arraySet, 34759))  {
    _type = DEREF(OTable_Struct, _type, 34793)._base;
  }
  if (_toPointer)  {
    FOut_Char('(');
  }
  if ((void*)DEREF(OTable_Struct, _type, 34871)._obj!=(void*)NULL)  {
    OGenGCC_Ident(DEREF(OTable_Struct, _type, 34906)._obj);
  } else {
    DEREF(OTable_Object, OGenGCC_emptyObj, 34936)._type = _type;
    _dummy = OGenGCC_NamedDecl(OGenGCC_emptyObj, 0x00000080U, -32768);
  }
  if (_toPointer)  {
    FOut_String(3, (CHAR *) "*)");
  }
}

static void OGenGCC_Designator (OEParse_Node _d, BOOLEAN _adr) {
  OEParse_Node _prefix;
  OTable_Struct _type;
  INTEGER _dim;

  BOOLEAN _AdrPrefix (void) {
    return (((_adr&&DEREF(OEParse_Node, _d, 35282)._class!=OEParse_ndDeref)&&DEREF(OEParse_Node, _d, 35307)._class!=OEParse_ndVarPar)&&DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 35347)._type, 35353)._form!=OTable_strDynArray)&&!(DEREF(OEParse_Node, _d, 35397)._class==OEParse_ndIndex&&DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 35422)._left, 35428)._type, 35434)._form==OTable_strDynArray);
    NO_RETURN (35228);
  }

  BOOLEAN _FindField (OTable_Struct _t, OTable_Object _obj) {
    OTable_Object _o;
    _o = DEREF(OTable_Struct, _t, 35593)._link;
    while ((void*)_o!=(void*)NULL)  {
      if (_o==_obj)  {
        return TRUE;
      }
      _o = DEREF(OTable_Object, _o, 35701)._next;
    }
    return FALSE;
    NO_RETURN (35491);
  }

  void _DesignRG (void) {
    if ((DEREF(OEParse_Node, _d, 35802)._class==OEParse_ndVar||DEREF(OEParse_Node, _d, 35826)._class==OEParse_ndVarPar)&&DEREF(OTable_Object, DEREF(OEParse_Node, _d, 35853)._obj, 35858)._type!=DEREF(OEParse_Node, _d, 35868)._type)  {
      FOut_String(3, (CHAR *) "((");
      OGenGCC_TypeDesign(DEREF(OEParse_Node, _d, 36050)._type, FALSE);
      if (DEREF(OEParse_Node, _d, 36079)._class==OEParse_ndVarPar)  {
        FOut_Char('*');
      }
      FOut_Char(')');
      OGenGCC_Ident(DEREF(OEParse_Node, _d, 36180)._obj);
      FOut_Char(')');
    } else {
      OGenGCC_Ident(DEREF(OEParse_Node, _d, 36236)._obj);
    }
  }
  if ((void*)_d!=(void*)NULL)  {
    if (_AdrPrefix())  {
      FOut_String(3, (CHAR *) "&(");
    }
    if (DEREF(OEParse_Node, _d, 36378)._class==OEParse_ndDeref)  {
      if (_adr||DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 36424)._type, 36430)._form==OTable_strDynArray)  {
        FOut_String(11, (CHAR *) "CHECK_NIL(");
      } else {
        FOut_String(7, (CHAR *) "DEREF(");
      }
      OGenGCC_TypeDesign(DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 36570)._left, 36576)._type, TRUE);
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Designator(DEREF(OEParse_Node, _d, 36637)._left, FALSE);
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _d, 36694)._pos, 0);
      FOut_Char(')');
    } else if (DEREF(OEParse_Node, _d, 36740)._class==OEParse_ndIndex)  {
      OGenGCC_GetArrayPrefix(_d, &(_prefix), &(_dim));
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 36858)._left, 36864)._type, 36870)._form==OTable_strArray)  {
        FOut_String(7, (CHAR *) "INDEX(");
        OGenGCC_Designator(DEREF(OEParse_Node, _d, 36975)._left, FALSE);
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_Expr(DEREF(OEParse_Node, _d, 37035)._right, FALSE);
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
        FOut_String(3, (CHAR *) ", ");
        FOut_Int(DEREF(OEParse_Node, _d, 37161)._pos, 0);
        FOut_Char(')');
      } else {
        if (!_adr)  {
          FOut_String(4, (CHAR *) "(* ");
        }
        if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 37359)._left, 37365)._type, 37371)._flags, 37355))  {
          FOut_String(15, (CHAR *) "PTR_INDEX_EXT(");
          OGenGCC_Designator(DEREF(OEParse_Node, _d, 37522)._left, TRUE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_Expr(DEREF(OEParse_Node, _d, 37585)._right, FALSE);
        } else {
          FOut_String(11, (CHAR *) "PTR_INDEX(");
          OGenGCC_Designator(DEREF(OEParse_Node, _d, 37732)._left, TRUE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_Expr(DEREF(OEParse_Node, _d, 37795)._right, FALSE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
          FOut_String(3, (CHAR *) ", ");
          FOut_Char('1');
          _type = DEREF(OEParse_Node, _d, 37956)._type;
          while (DEREF(OTable_Struct, _type, 37987)._form==OTable_strDynArray)  {
            INCI(_dim, 1, 38028);
            FOut_Char('*');
            OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
            _type = DEREF(OTable_Struct, _type, 38134)._base;
          }
          FOut_String(3, (CHAR *) ", ");
          FOut_Int(DEREF(OEParse_Node, _d, 38207)._pos, 0);
        }
        FOut_Char(')');
        if (!_adr)  {
          FOut_Char(')');
        }
      }
    } else if (DEREF(OEParse_Node, _d, 38344)._class==OEParse_ndGuard)  {
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 38383)._type, 38389)._form==OTable_strRecord)  {
        FOut_String(4, (CHAR *) "REC");
      } else {
        FOut_String(4, (CHAR *) "PTR");
      }
      FOut_String(13, (CHAR *) "_TYPE_GUARD(");
      OGenGCC_TypeDesign(DEREF(OEParse_Node, _d, 38552)._type, FALSE);
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Designator(DEREF(OEParse_Node, _d, 38614)._left, DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 38623)._type, 38629)._form==OTable_strRecord);
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 38665)._type, 38671)._form==OTable_strRecord)  {
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_GenTypeTag(DEREF(OEParse_Node, _d, 38748)._left);
      }
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _d, 38819)._type, TRUE);
      OGenGCC_GenTypeLevel(DEREF(OEParse_Node, _d, 38857)._type);
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _d, 38907)._pos, 0);
      FOut_Char(')');
    } else if (DEREF(OEParse_Node, _d, 38953)._class==OEParse_ndTBSuper)  {
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 39035)._left, 39041)._type, 39047)._form==OTable_strPointer)  {
        OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 39102)._left, 39108)._type, 39114)._base, 39120)._base, TRUE);
      } else {
        OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 39174)._left, 39180)._type, 39186)._base, TRUE);
      }
      FOut_String(6, (CHAR *) "->tb_");
      FOut_Int((LONGINT) OGenGCC_TypeDescId(DEREF(OEParse_Node, _d, 39269)._obj), 0);
    } else if (DEREF(OEParse_Node, _d, 39294)._class==OEParse_ndTBProc)  {
      OGenGCC_GenTypeTag(DEREF(OEParse_Node, _d, 39383)._left);
      FOut_String(6, (CHAR *) "->tb_");
      FOut_Int((LONGINT) OGenGCC_TypeDescId(DEREF(OEParse_Node, _d, 39448)._obj), 0);
    } else {
      OGenGCC_Designator(DEREF(OEParse_Node, _d, 39491)._left, FALSE);
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _d, 39521)._class;
        switch (_temp_) {
          case 1: {
            _DesignRG();
            break;
          }
          case 11:
          case 7:
          case 8: {
            OGenGCC_Ident(DEREF(OEParse_Node, _d, 39627)._obj);
            break;
          }
          case 2: {
            if (_adr||IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 39678)._type, 39684)._form, OGenGCC_arraySet, 39691))  {
              _DesignRG();
            } else {
              FOut_String(4, (CHAR *) "(* ");
              _DesignRG();
              FOut_Char(')');
            }
            break;
          }
          case 3: {
            _type = DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 40040)._left, 40046)._type;
            while (!_FindField(_type, DEREF(OEParse_Node, _d, 40089)._obj))  {
              FOut_String(6, (CHAR *) ".base");
              _type = DEREF(OTable_Struct, _type, 40155)._base;
            }
            FOut_String(2, (CHAR *) ".");
            if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 40236)._left, 40242)._type, 40248)._flags, 40232))  {
              FOut_String(48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, _d, 40354)._obj, 40359)._name);
            } else {
              OGenGCC_Ident(DEREF(OEParse_Node, _d, 40402)._obj);
            }
            break;
          }
          default: NO_LABEL (_temp_, 39515);
        }
      }
    }
    if (_AdrPrefix())  {
      FOut_Char(')');
    }
  }
}

static void OGenGCC_Projection (OEParse_Node _expr, OTable_Struct _fType) {
  OTable_Struct _type;
  if (DEREF(OEParse_Node, _expr, 40697)._type==_fType)  {
    OGenGCC_Expr(_expr, FALSE);
  } else if (DEREF(OTable_Struct, _fType, 40759)._form==OTable_strPointer)  {
    FOut_Char('(');
    OGenGCC_TypeDesign(_fType, FALSE);
    FOut_Char(')');
    OGenGCC_Expr(_expr, FALSE);
  } else {
    OGenGCC_Expr(_expr, FALSE);
    _type = DEREF(OEParse_Node, _expr, 40971)._type;
    do  {
      FOut_String(6, (CHAR *) ".base");
      _type = DEREF(OTable_Struct, _type, 41040)._base;
    } while (!(_type==_fType));
  }
}

static void OGenGCC_ProcCall (OEParse_Node _call) {
  OEParse_Node _apar, _prefix;
  OTable_Object _fpar;
  OTable_Struct _base, _t;
  INTEGER _i, _dim;
  BOOLEAN _tbCall;
  _apar = DEREF(OEParse_Node, _call, 41289)._right;
  _fpar = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41353)._left, 41359)._type, 41365)._link;
  _tbCall = DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41420)._left, 41426)._class==OEParse_ndTBSuper||DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41457)._left, 41463)._class==OEParse_ndTBProc;
  if (_tbCall)  {
    _t = DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41672)._left, 41678)._left, 41684)._type;
    if (DEREF(OTable_Struct, _t, 41703)._form==OTable_strPointer)  {
      _t = DEREF(OTable_Struct, _t, 41745)._base;
    }
    DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41773)._left, 41779)._obj = OTable_FindField(48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41805)._left, 41811)._obj, 41816)._name, _t);
    DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41838)._left, 41844)._type = DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41858)._left, 41864)._obj, 41869)._type;
    _fpar = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41895)._left, 41901)._type, 41907)._link;
    DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42005)._left, 42011)._left, 42017)._link = _apar;
    _apar = DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42051)._left, 42057)._left;
    if (DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42079)._left, 42085)._class==OEParse_ndTBSuper)  {
      _fpar = OTable_FindField(48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42240)._left, 42246)._obj, 42251)._name, DEREF(OTable_Struct, _t, 42260)._base);
      _fpar = DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 42289)._type, 42295)._link;
    }
  }
  OGenGCC_Designator(DEREF(OEParse_Node, _call, 42400)._left, FALSE);
  FOut_Char('(');
  while ((void*)_apar!=(void*)NULL)  {
    if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 42474)._type, 42480)._form==OTable_strDynArray&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 42511)._type, 42517)._base, 42523)._form==OTable_strSysByte)  {
      if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 42689)._type, 42695)._flags|DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42707)._left, 42713)._type, 42719)._flags, 42681))  {
        FOut_String(8, (CHAR *) "(BYTE*)");
      } else {
        if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 42934)._type, 42940)._form==OTable_strString)  {
          FOut_Int((LONGINT) (Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _apar, 43002)._conval, 43010)._string, 43018))+1), 0);
        } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43049)._type, 43055)._form==OTable_strDynArray&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43120)._type, 43126)._flags, 43113))  {
          FOut_Int(OMachine_maxLInt, 0);
        } else {
          FOut_String(8, (CHAR *) "sizeof(");
          OGenGCC_TypeDesign(DEREF(OEParse_Node, _apar, 43300)._type, FALSE);
          FOut_Char(')');
          if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43362)._type, 43368)._form, OGenGCC_arraySet, 43375))  {
            _t = DEREF(OEParse_Node, _apar, 43416)._type;
            OGenGCC_GetArrayPrefix(_apar, &(_prefix), &(_dim));
            while (IN(DEREF(OTable_Struct, _t, 43496)._form, OGenGCC_arraySet, 43503))  {
              INCI(_dim, 1, 43535);
              FOut_Char('*');
              OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
              _t = DEREF(OTable_Struct, _t, 43641)._base;
            }
          }
        }
        FOut_String(10, (CHAR *) ", (BYTE*)");
      }
      OGenGCC_Designator(_apar, TRUE);
    } else if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 43792)._type, 43798)._form, OGenGCC_arraySet, 43805))  {
      if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 43881)._type, 43887)._flags|DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 43899)._left, 43905)._type, 43911)._flags, 43873)&&DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 43940)._type, 43946)._form==OTable_strDynArray)  {
        if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43993)._type, 43999)._form==OTable_strString)  {
          FOut_Int((LONGINT) (Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _apar, 44061)._conval, 44069)._string, 44077))+1), 0);
          FOut_String(3, (CHAR *) ", ");
        } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44136)._type, 44142)._form==OTable_strDynArray&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44207)._type, 44213)._flags, 44200))  {
          FOut_Int(OMachine_maxLInt, 0);
          FOut_String(3, (CHAR *) ", ");
        } else {
          OGenGCC_GetArrayPrefix(_apar, &(_prefix), &(_dim));
          {
            INTEGER _temp_ = OGenGCC_DynDimensions(DEREF(OTable_Object, _fpar, 44447)._type);
            for(_i = 1 ; _i <= _temp_ ; _i += 1) {
              OGenGCC_ArrayLength(_prefix, (LONGINT) (_i+_dim));
              FOut_String(3, (CHAR *) ", ");
            }
          }
        }
      }
      if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 44591)._type, 44597)._form==OTable_strDynArray&&DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44628)._type, 44634)._form==OTable_strDynArray)  {
        OGenGCC_Designator(_apar, TRUE);
      } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 44716)._type, 44722)._form==OTable_strDynArray)  {
        _base = DEREF(OTable_Object, _fpar, 44856)._type;
        while (DEREF(OTable_Struct, _base, 44885)._form==OTable_strDynArray)  {
          _base = DEREF(OTable_Struct, _base, 44936)._base;
        }
        FOut_Char('(');
        OGenGCC_Ident(DEREF(OTable_Struct, _base, 45060)._obj);
        FOut_String(5, (CHAR *) " *) ");
        OGenGCC_Expr(_apar, FALSE);
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 45145)._type, 45151)._form==OTable_strString&&(void*)DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 45180)._type, 45186)._obj!=(void*)NULL)  {
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_Expr(_apar, FALSE);
      } else {
        OGenGCC_Expr(_apar, FALSE);
      }
    } else if (DEREF(OTable_Object, _fpar, 45392)._mode==OTable_objVarPar)  {
      if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 45480)._type, 45486)._flags|DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 45498)._left, 45504)._type, 45510)._flags, 45472)&&DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 45539)._type, 45545)._form==OTable_strRecord)  {
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_GenTypeTag(_apar);
        FOut_String(3, (CHAR *) ", ");
        if (DEREF(OEParse_Node, _apar, 45709)._type!=DEREF(OTable_Object, _fpar, 45722)._type)  {
          FOut_String(2, (CHAR *) "(");
          OGenGCC_TypeDesign(DEREF(OTable_Object, _fpar, 45791)._type, FALSE);
          FOut_String(3, (CHAR *) "*)");
        }
        OGenGCC_Designator(_apar, TRUE);
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 45903)._type, 45909)._form==OTable_strPointer&&DEREF(OEParse_Node, _apar, 45954)._class==OEParse_ndGuard)  {
        FOut_String(2, (CHAR *) "(");
        OGenGCC_Designator(_apar, FALSE);
        FOut_String(13, (CHAR *) ", (void*)(&(");
        OGenGCC_Designator(DEREF(OEParse_Node, _apar, 46176)._left, FALSE);
        FOut_String(4, (CHAR *) ")))");
      } else if (DEREF(OTable_Object, _fpar, 46238)._type!=DEREF(OEParse_Node, _apar, 46251)._type&&DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 46266)._type, 46272)._form==OTable_strSysPtr)  {
        FOut_String(7, (CHAR *) "(PTR*)");
        OGenGCC_Designator(_apar, TRUE);
      } else {
        OGenGCC_Designator(_apar, TRUE);
      }
    } else {
      if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 46576)._type, 46582)._form, 0x00084000U, 46589)&&DEREF(OEParse_Node, _apar, 46628)._type!=DEREF(OTable_Object, _fpar, 46641)._type)  {
        OGenGCC_Projection(_apar, DEREF(OTable_Object, _fpar, 46686)._type);
      } else {
        OGenGCC_Expr(_apar, FALSE);
      }
    }
    _apar = DEREF(OEParse_Node, _apar, 46777)._link;
    if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 46799)._type, 46805)._form!=OTable_strNone)  {
      _fpar = DEREF(OTable_Object, _fpar, 46850)._link;
    }
    if ((void*)_apar!=(void*)NULL)  {
      FOut_String(3, (CHAR *) ", ");
    }
  }
  FOut_Char(')');
}

static void OGenGCC_WriteOperation (OEParse_Node _n, LONGINT __oc_0, const CHAR (* __oc_p), SET _flags, SHORTINT _pe, SHORTINT _pl, SHORTINT _pr) {
  CHAR (* _oc);
  CHAR _app[4];
  VALUE_ARRAY(_oc, __oc_p, 1*__oc_0);
  if ('A'<=CAP((* PTR_INDEX(_oc, 0, __oc_0, 1, 48202)))&&CAP((* PTR_INDEX(_oc, 0, __oc_0, 1, 48218)))<='Z')  {
    FOut_String(__oc_0, _oc);
    if (IN(OGenGCC_fAddType, _flags, 48293))  {
      {
        SHORTINT _temp_ = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 48322)._left, 48328)._type, 48334)._form;
        switch (_temp_) {
          case 3: {
            COPYSTRING(&(_app), "SI");
            break;
          }
          case 4: {
            COPYSTRING(&(_app), "I");
            break;
          }
          case 5: {
            COPYSTRING(&(_app), "LI");
            break;
          }
          case 6: {
            COPYSTRING(&(_app), "R");
            break;
          }
          case 7: {
            COPYSTRING(&(_app), "LR");
            break;
          }
          case 2: {
            COPYSTRING(&(_app), "C");
            break;
          }
          case 12: {
            COPYSTRING(&(_app), "B");
            break;
          }
          default: NO_LABEL (_temp_, 48316);
        }
      }
      FOut_String(4, (CHAR *) _app);
    }
    FOut_Char('(');
    if (IN(OGenGCC_fLeftAdr, _flags, 48674))  {
      OGenGCC_Designator(DEREF(OEParse_Node, _n, 48710)._left, TRUE);
    } else {
      OGenGCC_Expr(DEREF(OEParse_Node, _n, 48750)._left, FALSE);
    }
    if (!IN(OGenGCC_fMonadic, _flags, 48796))  {
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Expr(DEREF(OEParse_Node, _n, 48851)._right, FALSE);
    }
    if (IN(OGenGCC_fAddPosition, _flags, 48901))  {
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _n, 48957)._pos, 0);
    }
    if (IN(OGenGCC_fAddTypename, _flags, 49001))  {
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49057)._left, 49063)._type, 49069)._obj);
    }
    if (IN(OGenGCC_fAddSize, _flags, 49106))  {
      FOut_String(10, (CHAR *) ", sizeof(");
      OGenGCC_Designator(DEREF(OEParse_Node, _n, 49174)._left, FALSE);
      FOut_Char(')');
    }
    FOut_Char(')');
  } else {
    if (IN(OGenGCC_fMonadic, _flags, 49321))  {
      FOut_String(__oc_0, _oc);
      OGenGCC_Expr(DEREF(OEParse_Node, _n, 49374)._left, _pe<=_pl);
    } else {
      if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49416)._left, 49422)._type, 49428)._form, 0x00006000U, 49435)&&DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49482)._left, 49488)._type!=DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49498)._right, 49505)._type)  {
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 49567)._left, _pe<=_pl);
        FOut_String(__oc_0, _oc);
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 49660)._right, _pe<=_pr);
      } else {
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 49709)._left, _pe<=_pl);
        FOut_String(__oc_0, _oc);
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 49770)._right, _pe<=_pr);
      }
    }
  }
}

static void OGenGCC_Expr (OEParse_Node _expr, BOOLEAN _paren) {
  CHAR _str[8];
  SET _flags;
  SHORTINT _pe, _pr, _pl;

  SHORTINT _Pri (OEParse_Node _expr) {
    if ((void*)_expr==(void*)NULL)  {
      return -128;
    } else if (DEREF(OEParse_Node, _expr, 50094)._class==OEParse_ndConst&&((IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50133)._type, 50139)._form, OEParse_intSet, 50146)&&DEREF(OTable_Const, DEREF(OEParse_Node, _expr, 50166)._conval, 50174)._intval<0)||(IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50207)._type, 50213)._form, OEParse_realSet, 50220)&&DEREF(OTable_Const, DEREF(OEParse_Node, _expr, 50241)._conval, 50249)._real<0.0000000000000000E+00)))  {
      return 2;
    } else if (DEREF(OEParse_Node, _expr, 50299)._class<OEParse_ndMOp)  {
      return 0;
    } else {
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 50404)._subcl;
        switch (_temp_) {
          case 17:
          case 45:
          case 24: {
            return 2;
            break;
          }
          case 1 ... 4: {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50518)._type, 50524)._form==OTable_strSet)  {
              {
                SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 50569)._subcl;
                switch (_temp_) {
                  case 1: {
                    return 8;
                    break;
                  }
                  case 2: {
                    return 9;
                    break;
                  }
                  default: NO_LABEL (_temp_, 50560);
                }
              }
            } else {
              return 3;
            }
            break;
          }
          case 6: {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50753)._type, 50759)._form==OTable_strSet)  {
              return 10;
            } else {
              return 4;
            }
            break;
          }
          case 7: {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50894)._type, 50900)._form==OTable_strSet)  {
              return 9;
            } else {
              return 4;
            }
            break;
          }
          case 11 ... 14: {
            return 6;
            break;
          }
          case 9:
          case 10: {
            return 7;
            break;
          }
          case 5: {
            return 11;
            break;
          }
          case 8: {
            return 11;
            break;
          }
          default:  {
          return 0;
        }
        }
      }
    }
    NO_RETURN (49981);
  }

  BOOLEAN _String (OEParse_Node _n) {
    return DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 51588)._type, 51594)._form==OTable_strString||(IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 51635)._type, 51641)._form, OGenGCC_arraySet, 51648)&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 51665)._type, 51671)._base, 51677)._form==OTable_strChar);
    NO_RETURN (51455);
  }
  if (_paren)  {
    FOut_Char('(');
  }
  _pe = _Pri(_expr);
  _pl = _Pri(DEREF(OEParse_Node, _expr, 51807)._left);
  _pr = _Pri(DEREF(OEParse_Node, _expr, 51835)._right);
  if (DEREF(OEParse_Node, _expr, 51857)._class<OEParse_ndConst)  {
    OGenGCC_Designator(_expr, FALSE);
  } else if (DEREF(OEParse_Node, _expr, 51929)._class==OEParse_ndConst)  {
    OGenGCC_Const(DEREF(OEParse_Node, _expr, 51972)._conval, DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 51986)._type, 51992)._form, DEREF(OEParse_Node, _expr, 52004)._obj);
  } else if (DEREF(OEParse_Node, _expr, 52026)._class==OEParse_ndUpto)  {
    FOut_String(7, (CHAR *) "RANGE(");
    OGenGCC_Expr(DEREF(OEParse_Node, _expr, 52094)._left, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 52150)._left, 52156)._pos, 0);
    FOut_String(3, (CHAR *) ", ");
    OGenGCC_Expr(DEREF(OEParse_Node, _expr, 52206)._right, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 52263)._right, 52270)._pos, 0);
    FOut_Char(')');
  } else if (DEREF(OEParse_Node, _expr, 52315)._class==OEParse_ndMOp&&DEREF(OEParse_Node, _expr, 52341)._subcl==OEParse_scConv)  {
    FOut_String(2, (CHAR *) "(");
    OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 52454)._type, 52460)._obj);
    FOut_String(3, (CHAR *) ") ");
    OGenGCC_Expr(DEREF(OEParse_Node, _expr, 52507)._left, _pl>=2);
  } else if (DEREF(OEParse_Node, _expr, 52537)._class==OEParse_ndMOp&&DEREF(OEParse_Node, _expr, 52563)._subcl==OEParse_scVal)  {
    if (DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 52752)._left, 52758)._class<OEParse_ndConst)  {
      FOut_String(5, (CHAR *) "(*((");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 52847)._type, 52853)._obj);
      FOut_String(4, (CHAR *) "*)&");
      OGenGCC_Expr(DEREF(OEParse_Node, _expr, 52905)._left, TRUE);
      FOut_String(3, (CHAR *) "))");
    } else {
      FOut_String(7, (CHAR *) "VAL_E(");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 53050)._type, 53056)._obj);
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Expr(DEREF(OEParse_Node, _expr, 53107)._left, TRUE);
      FOut_Char(')');
    }
  } else if (DEREF(OEParse_Node, _expr, 53168)._class==OEParse_ndMOp&&DEREF(OEParse_Node, _expr, 53194)._subcl==OEParse_scAdr)  {
    FOut_String(10, (CHAR *) "(LONGINT)");
    OGenGCC_Designator(DEREF(OEParse_Node, _expr, 53270)._left, TRUE);
  } else if (DEREF(OEParse_Node, _expr, 53299)._class==OEParse_ndMOp)  {
    _flags = 0x00000000U;
    {
      SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 53357)._subcl;
      switch (_temp_) {
        case 7: {
          if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 53403)._type, 53409)._form!=OTable_strSet)  {
            COPYSTRING(&(_str), "-");
          } else {
            COPYSTRING(&(_str), "~");
          }
          break;
        }
        case 17: {
          COPYSTRING(&(_str), "!");
          break;
        }
        case 21: {
          COPYSTRING(&(_str), "ABS");
          _flags = 0x00000003U;
          break;
        }
        case 22: {
          COPYSTRING(&(_str), "CAP");
          break;
        }
        case 23: {
          COPYSTRING(&(_str), "ODD");
          _flags = 0x00000001U;
          break;
        }
        case 20: {
          COPYSTRING(&(_str), "sizeof");
          break;
        }
        default: NO_LABEL (_temp_, 53348);
      }
    }
    OGenGCC_WriteOperation(_expr, 8, (CHAR *) _str, _flags|0x00000004U, _pe, _pl, _pr);
  } else if (DEREF(OEParse_Node, _expr, 53837)._class==OEParse_ndDOp&&DEREF(OEParse_Node, _expr, 53863)._subcl==OEParse_scLen)  {
    OGenGCC_ArrayLength(DEREF(OEParse_Node, _expr, 53910)._left, DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 53922)._right, 53929)._conval, 53937)._intval);
  } else if (DEREF(OEParse_Node, _expr, 53962)._class==OEParse_ndDOp&&DEREF(OEParse_Node, _expr, 53988)._subcl==OEParse_scIs)  {
    OGenGCC_GenTypeTest(DEREF(OEParse_Node, _expr, 54052)._left, DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 54064)._right, 54071)._type);
  } else if (DEREF(OEParse_Node, _expr, 54094)._class==OEParse_ndDOp)  {
    _flags = 0x00000000U;
    if (_String(DEREF(OEParse_Node, _expr, 54158)._left))  {
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 54213)._subcl;
        switch (_temp_) {
          case 9: {
            COPYSTRING(&(_str), "EQL");
            break;
          }
          case 10: {
            COPYSTRING(&(_str), "NEQ");
            break;
          }
          case 11: {
            COPYSTRING(&(_str), "LSS");
            break;
          }
          case 12: {
            COPYSTRING(&(_str), "LEQ");
            break;
          }
          case 13: {
            COPYSTRING(&(_str), "GRT");
            break;
          }
          case 14: {
            COPYSTRING(&(_str), "GEQ");
            break;
          }
          default: NO_LABEL (_temp_, 54204);
        }
      }
      Strings_Insert(4, (CHAR *) "STR", 0, 8, (CHAR *) _str);
    } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 54481)._type, 54487)._form==OTable_strSet)  {
      if (DEREF(OEParse_Node, _expr, 54550)._subcl==OEParse_scMinus)  {
        COPYSTRING(&(_str), "SETDIFF");
      } else {
        {
          SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 54635)._subcl;
          switch (_temp_) {
            case 6: {
              COPYSTRING(&(_str), "|");
              break;
            }
            case 1: {
              COPYSTRING(&(_str), "&");
              break;
            }
            case 2: {
              COPYSTRING(&(_str), "^");
              break;
            }
            default: NO_LABEL (_temp_, 54626);
          }
        }
      }
    } else {
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 54800)._subcl;
        switch (_temp_) {
          case 6: {
            COPYSTRING(&(_str), "+");
            break;
          }
          case 7: {
            COPYSTRING(&(_str), "- ");
            break;
          }
          case 1: {
            COPYSTRING(&(_str), "*");
            break;
          }
          case 3: {
            COPYSTRING(&(_str), "DIV");
            _flags = 0x00000003U;
            break;
          }
          case 4: {
            COPYSTRING(&(_str), "MOD");
            _flags = 0x00000003U;
            break;
          }
          case 2: {
            COPYSTRING(&(_str), "DIV");
            _flags = 0x00000003U;
            break;
          }
          case 9: {
            COPYSTRING(&(_str), "==");
            break;
          }
          case 10: {
            COPYSTRING(&(_str), "!=");
            break;
          }
          case 11: {
            COPYSTRING(&(_str), "<");
            break;
          }
          case 12: {
            COPYSTRING(&(_str), "<=");
            break;
          }
          case 13: {
            COPYSTRING(&(_str), ">");
            break;
          }
          case 14: {
            COPYSTRING(&(_str), ">=");
            break;
          }
          case 5: {
            COPYSTRING(&(_str), "&&");
            break;
          }
          case 8: {
            COPYSTRING(&(_str), "||");
            break;
          }
          case 15: {
            COPYSTRING(&(_str), "IN");
            _flags = 0x00000002U;
            break;
          }
          case 26: {
            COPYSTRING(&(_str), "ASH");
            _flags = 0x00000003U;
            break;
          }
          case 27: {
            COPYSTRING(&(_str), "BIT");
            _flags = 0x00000000U;
            break;
          }
          case 28: {
            COPYSTRING(&(_str), "LSH");
            _flags = 0x00000001U;
            break;
          }
          case 29: {
            COPYSTRING(&(_str), "ROT");
            _flags = 0x00000001U;
            break;
          }
          default: NO_LABEL (_temp_, 54791);
        }
      }
    }
    OGenGCC_WriteOperation(_expr, 8, (CHAR *) _str, _flags, _pe, _pl, _pr);
  } else if (DEREF(OEParse_Node, _expr, 55719)._class==OEParse_ndCall)  {
    OGenGCC_ProcCall(_expr);
  } else {
    HALT(101);
  }
  if (_paren)  {
    FOut_Char(')');
  }
}

static void OGenGCC_AppendAtomicFlag (OTable_Struct _t) {
  if (OTable_ContainsPointer(_t))  {
    FOut_String(5, (CHAR *) ", 0)");
  } else {
    FOut_String(5, (CHAR *) ", 1)");
  }
}

static void OGenGCC_StatementSeq (OEParse_Node _s, INTEGER _off, BOOLEAN _paren) {
  OEParse_Node _n, _m;
  CHAR _str[16];
  SET _flags;
  INTEGER _dim;
  OTable_Struct _t;

  void _ExtendedAssignment (OEParse_Node _l, OEParse_Node _r) {
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, _l, 56502)._type, 56508)._form==OTable_strRecord&&(DEREF(OEParse_Node, _l, 56535)._class==OEParse_ndVarPar||DEREF(OEParse_Node, _l, 56562)._class==OEParse_ndDeref))  {
      FOut_String(13, (CHAR *) "ASSERT_TYPE(");
      if (DEREF(OEParse_Node, _l, 56715)._class==OEParse_ndVarPar)  {
        OGenGCC_GenTagName(DEREF(OEParse_Node, _l, 56765)._obj);
      } else {
        FOut_String(5, (CHAR *) "TAG(");
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _l, 56870)._type, FALSE);
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_Designator(DEREF(OEParse_Node, _l, 56936)._left, FALSE);
        FOut_String(3, (CHAR *) ", ");
        FOut_Int(DEREF(OEParse_Node, _l, 56997)._pos, 0);
        FOut_Char(')');
      }
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _l, 57094)._type, TRUE);
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _l, 57150)._pos, 0);
      FOut_String(3, (CHAR *) ");");
      OGenGCC_Off(_off);
    }
    OGenGCC_Designator(_l, FALSE);
    FOut_String(4, (CHAR *) " = ");
    OGenGCC_Projection(_r, DEREF(OEParse_Node, _l, 57290)._type);
  }

  void _CaseLabel (OEParse_Node _l, SHORTINT _form) {
    LONGINT _b;
    OGenGCC_Const(DEREF(OEParse_Node, _l, 57435)._conval, _form, (OTable_Object)NULL);
    if (DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57468)._conval, 57476)._intval!=DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57486)._conval, 57494)._intval2)  {
      FOut_String(6, (CHAR *) " ... ");
      _b = DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57552)._conval, 57560)._intval;
      DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57579)._conval, 57587)._intval = DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57600)._conval, 57608)._intval2;
      OGenGCC_Const(DEREF(OEParse_Node, _l, 57635)._conval, _form, (OTable_Object)NULL);
      DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57666)._conval, 57674)._intval = _b;
    }
  }
  if (_paren)  {
    FOut_String(3, (CHAR *) " {");
  }
  while ((void*)_s!=(void*)NULL)  {
    OGenGCC_Off(_off);
    {
      SHORTINT _temp_ = DEREF(OEParse_Node, _s, 57825)._class;
      switch (_temp_) {
        case 16: {
          if (DEREF(OEParse_Node, _s, 57867)._subcl==OEParse_scMove)  {
            FOut_String(6, (CHAR *) "MOVE(");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 57937)._right, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 57998)._right, 58005)._link, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58065)._right, 58072)._link, 58078)._link, FALSE);
            FOut_Char(')');
          } else if (DEREF(OEParse_Node, _s, 58133)._subcl==OEParse_scNewFix)  {
            if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58173)._left, 58179)._type, 58185)._base, 58191)._form==OTable_strRecord&&!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58249)._left, 58255)._type, 58261)._base, 58267)._flags, 58245))  {
              FOut_String(8, (CHAR *) "NEWREC(");
              OGenGCC_Expr(DEREF(OEParse_Node, _s, 58368)._left, FALSE);
              FOut_String(3, (CHAR *) ", ");
              OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58442)._left, 58448)._type, 58454)._base, TRUE);
            } else {
              FOut_String(8, (CHAR *) "NEWFIX(");
              OGenGCC_Expr(DEREF(OEParse_Node, _s, 58597)._left, FALSE);
            }
            OGenGCC_AppendAtomicFlag(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58656)._left, 58662)._type, 58668)._base);
          } else if (DEREF(OEParse_Node, _s, 58692)._subcl==OEParse_scNewDyn)  {
            FOut_String(7, (CHAR *) "NEWDYN");
            if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58781)._left, 58787)._type, 58793)._base, 58799)._flags, 58777))  {
              FOut_String(5, (CHAR *) "_EXT");
            }
            FOut_Char('(');
            OGenGCC_Designator(DEREF(OEParse_Node, _s, 58943)._left, FALSE);
            FOut_String(4, (CHAR *) ", 1");
            _dim = 0;
            _n = DEREF(OEParse_Node, _s, 59023)._right;
            while ((void*)_n!=(void*)NULL)  {
              FOut_String(2, (CHAR *) "*");
              OGenGCC_Expr(_n, FALSE);
              INCI(_dim, 1, 59130);
              _n = DEREF(OEParse_Node, _n, 59159)._link;
            }
            FOut_String(3, (CHAR *) ", ");
            FOut_Int((LONGINT) _dim, 0);
            OGenGCC_AppendAtomicFlag(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59263)._left, 59269)._type, 59275)._base);
            if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59318)._left, 59324)._type, 59330)._base, 59336)._flags, 59314))  {
              _dim = 0;
              _n = DEREF(OEParse_Node, _s, 59444)._right;
              while ((void*)_n!=(void*)NULL)  {
                FOut_Char(';');
                OGenGCC_Off(_off);
                FOut_String(9, (CHAR *) "SET_LEN(");
                FOut_Int((LONGINT) _dim, 0);
                FOut_String(3, (CHAR *) ", ");
                OGenGCC_Expr(_n, FALSE);
                FOut_Char(')');
                INCI(_dim, 1, 59708);
                _n = DEREF(OEParse_Node, _n, 59739)._link;
              }
            }
          } else if (DEREF(OEParse_Node, _s, 59792)._subcl==OEParse_scDispose)  {
            FOut_String(8, (CHAR *) "DISPOSE");
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59865)._left, 59871)._type, 59877)._form==OTable_strSysPtr||IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59935)._left, 59941)._type, 59947)._base, 59953)._flags, 59931))  {
            } else if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60053)._left, 60059)._type, 60065)._base, 60071)._form==OTable_strRecord)  {
              FOut_String(5, (CHAR *) "_REC");
            } else if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60146)._left, 60152)._type, 60158)._base, 60164)._form==OTable_strDynArray)  {
              FOut_String(5, (CHAR *) "_DYN");
            }
            FOut_Char('(');
            OGenGCC_Designator(DEREF(OEParse_Node, _s, 60285)._left, FALSE);
            if ((DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60316)._left, 60322)._type, 60328)._form==OTable_strPointer&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60369)._left, 60375)._type, 60381)._base, 60387)._form==OTable_strDynArray)&&!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60448)._left, 60454)._type, 60460)._base, 60466)._flags, 60444))  {
              FOut_String(3, (CHAR *) ", ");
              _dim = 0;
              _t = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60549)._left, 60555)._type, 60561)._base;
              while (DEREF(OTable_Struct, _t, 60589)._form==OTable_strDynArray)  {
                INCI(_dim, 1, 60630);
                _t = DEREF(OTable_Struct, _t, 60661)._base;
              }
              FOut_Int((LONGINT) _dim, 0);
            }
            FOut_Char(')');
          } else if (DEREF(OEParse_Node, _s, 60766)._subcl==OEParse_scCollect)  {
            FOut_String(11, (CHAR *) "GC_COLLECT");
          } else if (DEREF(OEParse_Node, _s, 60842)._subcl==OEParse_scCopy)  {
            FOut_String(6, (CHAR *) "COPY(");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 60926)._left, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 61007)._right, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_ArrayLength(DEREF(OEParse_Node, _s, 61095)._right, 0);
            FOut_Char(')');
          } else if (DEREF(OEParse_Node, _s, 61171)._subcl==OEParse_scAssign&&IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61197)._left, 61203)._type, 61209)._form, 0x00084000U, 61216))  {
            _ExtendedAssignment(DEREF(OEParse_Node, _s, 61284)._left, DEREF(OEParse_Node, _s, 61293)._right);
          } else if (DEREF(OEParse_Node, _s, 61318)._subcl==OEParse_scGet||DEREF(OEParse_Node, _s, 61342)._subcl==OEParse_scPut)  {
            if (DEREF(OEParse_Node, _s, 61381)._subcl==OEParse_scGet)  {
              FOut_String(5, (CHAR *) "GET(");
            } else {
              FOut_String(5, (CHAR *) "PUT(");
            }
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 61512)._left, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 61572)._right, FALSE);
            FOut_String(3, (CHAR *) ", ");
            if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61631)._right, 61638)._type, 61644)._form, 0x0000C000U, 61651))  {
              FOut_String(6, (CHAR *) "void*");
            } else {
              OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61752)._right, 61759)._type, 61765)._obj);
            }
            FOut_Char(')');
          } else {
            _flags = 0x00000000U;
            {
              SHORTINT _temp_ = DEREF(OEParse_Node, _s, 61862)._subcl;
              switch (_temp_) {
                case 0: {
                  if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61916)._right, 61923)._type, 61929)._form==OTable_strString)  {
                    COPYSTRING(&(_str), "COPYSTRING");
                    _flags = 0x00000010U;
                  } else if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62035)._left, 62041)._type, 62047)._form, OGenGCC_arraySet, 62054))  {
                    if ((void*)DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62093)._left, 62099)._type, 62105)._obj!=(void*)NULL)  {
                      COPYSTRING(&(_str), "COPYARRAYT");
                      _flags = 0x00000018U;
                    } else {
                      COPYSTRING(&(_str), "COPYARRAY");
                      _flags = 0x00000030U;
                    }
                  } else {
                    COPYSTRING(&(_str), " = ");
                  }
                  break;
                }
                case 34: {
                  COPYSTRING(&(_str), "INC");
                  _flags = 0x00000003U;
                  break;
                }
                case 35: {
                  COPYSTRING(&(_str), "DEC");
                  _flags = 0x00000003U;
                  break;
                }
                case 32: {
                  COPYSTRING(&(_str), "INCL");
                  _flags = 0x00000002U;
                  break;
                }
                case 33: {
                  COPYSTRING(&(_str), "EXCL");
                  _flags = 0x00000002U;
                  break;
                }
                case 42: {
                  COPYSTRING(&(_str), "NEWSYS");
                  _flags = 0x00000000U;
                  break;
                }
                default: NO_LABEL (_temp_, 61856);
              }
            }
            OGenGCC_WriteOperation(_s, 16, (CHAR *) _str, _flags, 1, 0, 0);
          }
          FOut_Char(';');
          break;
        }
        case 17: {
          OGenGCC_ProcCall(_s);
          FOut_Char(';');
          break;
        }
        case 18: {
          _n = DEREF(OEParse_Node, _s, 62888)._left;
          do  {
            if (_n==DEREF(OEParse_Node, _s, 62928)._left)  {
              FOut_String(5, (CHAR *) "if (");
            } else {
              FOut_String(11, (CHAR *) " else if (");
            }
            OGenGCC_Expr(DEREF(OEParse_Node, _n, 63054)._left, FALSE);
            FOut_String(3, (CHAR *) ") ");
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _n, 63122)._right, _off+1, TRUE);
            _n = DEREF(OEParse_Node, _n, 63161)._link;
          } while (!((void*)_n==(void*)NULL));
          if ((void*)DEREF(OEParse_Node, _s, 63204)._right!=(void*)NULL)  {
            FOut_String(6, (CHAR *) " else");
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 63277)._right, _off+1, TRUE);
          }
          break;
        }
        case 20: {
          FOut_Char('{');
          OGenGCC_Off(_off+1);
          OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 63388)._left, 63394)._type, 63400)._obj);
          FOut_String(11, (CHAR *) " _temp_ = ");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 63456)._left, FALSE);
          FOut_Char(';');
          OGenGCC_Off(_off+1);
          FOut_String(18, (CHAR *) "switch (_temp_) {");
          _n = DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 63569)._right, 63576)._left;
          while ((void*)_n!=(void*)NULL)  {
            _m = DEREF(OEParse_Node, _n, 63627)._link;
            do  {
              OGenGCC_Off(_off+2);
              FOut_String(6, (CHAR *) "case ");
              _CaseLabel(_m, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 63736)._left, 63742)._type, 63748)._form);
              _m = DEREF(OEParse_Node, _m, 63775)._link;
              FOut_Char(':');
            } while (!((void*)_m==(void*)NULL));
            FOut_String(3, (CHAR *) " {");
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _n, 63887)._right, _off+3, FALSE);
            OGenGCC_Off(_off+3);
            FOut_String(7, (CHAR *) "break;");
            OGenGCC_Off(_off+2);
            FOut_Char('}');
            _n = DEREF(OEParse_Node, _n, 64028)._left;
          }
          OGenGCC_Off(_off+2);
          FOut_String(10, (CHAR *) "default: ");
          if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64114)._right, 64121)._conval, 64129)._set!=0x00000000U)  {
            OGenGCC_StatementSeq(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64169)._right, 64176)._right, _off+2, TRUE);
          } else {
            FOut_String(19, (CHAR *) "NO_LABEL (_temp_, ");
            FOut_Int(DEREF(OEParse_Node, _s, 64272)._pos, 0);
            FOut_String(3, (CHAR *) ");");
          }
          OGenGCC_Off(_off+1);
          FOut_Char('}');
          OGenGCC_Off(_off);
          FOut_Char('}');
          break;
        }
        case 23: {
          FOut_String(8, (CHAR *) "while (");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 64469)._left, FALSE);
          FOut_String(3, (CHAR *) ") ");
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 64533)._right, _off+1, TRUE);
          break;
        }
        case 24: {
          FOut_String(4, (CHAR *) "do ");
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 64624)._left, _off+1, TRUE);
          FOut_String(11, (CHAR *) " while (!(");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 64694)._right, FALSE);
          FOut_String(4, (CHAR *) "));");
          break;
        }
        case 25: {
          FOut_Char('{');
          OGenGCC_Off(_off+1);
          OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64812)._left, 64818)._link, 64824)._type, 64830)._obj);
          FOut_String(8, (CHAR *) " _temp_");
          FOut_String(4, (CHAR *) " = ");
          OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64909)._left, 64915)._right, FALSE);
          FOut_Char(';');
          OGenGCC_Off(_off+1);
          FOut_String(5, (CHAR *) "for(");
          OGenGCC_Designator(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65023)._left, 65029)._link, FALSE);
          FOut_String(4, (CHAR *) " = ");
          OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65086)._left, 65092)._left, FALSE);
          FOut_String(4, (CHAR *) " ; ");
          OGenGCC_Designator(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65155)._left, 65161)._link, FALSE);
          if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65190)._left, 65196)._conval, 65204)._intval>0)  {
            FOut_String(5, (CHAR *) " <= ");
          } else {
            FOut_String(5, (CHAR *) " >= ");
          }
          FOut_String(10, (CHAR *) "_temp_ ; ");
          OGenGCC_Designator(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65358)._left, 65364)._link, FALSE);
          FOut_String(5, (CHAR *) " += ");
          FOut_Int(DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65423)._left, 65429)._conval, 65437)._intval, 0);
          FOut_Char(')');
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 65496)._right, _off+2, TRUE);
          OGenGCC_Off(_off);
          FOut_Char('}');
          break;
        }
        case 28: {
          FOut_String(9, (CHAR *) "while(1)");
          DEREF(OEParse_Node, _s, 65616)._pos = -DEREF(OEParse_Node, _s, 65627)._pos;
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 65657)._left, _off+1, TRUE);
          if (DEREF(OEParse_Node, _s, 65692)._pos>=0)  {
            OGenGCC_Off(_off);
            FOut_String(6, (CHAR *) "_exit");
            FOut_Int(DEREF(OEParse_Node, _s, 65826)._pos, 0);
            FOut_String(4, (CHAR *) ": ;");
          }
          break;
        }
        case 29: {
          _n = DEREF(OEParse_Node, _s, 65908)._left;
          while ((void*)_n!=(void*)NULL)  {
            if (_n!=DEREF(OEParse_Node, _s, 65962)._left)  {
              FOut_String(6, (CHAR *) "else ");
            }
            FOut_String(5, (CHAR *) "if (");
            OGenGCC_GenTypeTest(DEREF(OEParse_Node, _n, 66074)._left, DEREF(OTable_Object, DEREF(OEParse_Node, _n, 66083)._obj, 66088)._type);
            FOut_Char(')');
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _n, 66146)._right, _off+1, TRUE);
            _n = DEREF(OEParse_Node, _n, 66185)._link;
          }
          OGenGCC_Off(_off);
          FOut_String(5, (CHAR *) "else");
          if (DEREF(OTable_Const, DEREF(OEParse_Node, _s, 66264)._conval, 66272)._set==0x00000000U)  {
            FOut_String(13, (CHAR *) " { NO_GUARD(");
            FOut_Int(DEREF(OEParse_Node, _s, 66344)._pos, 0);
            FOut_String(5, (CHAR *) "); }");
          } else {
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 66421)._right, _off+1, TRUE);
          }
          break;
        }
        case 31: {
          DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66482)._left, 66488)._pos = ABSLI(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66503)._left, 66509)._pos, 66497);
          FOut_String(11, (CHAR *) "goto _exit");
          FOut_Int(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66600)._left, 66606)._pos, 0);
          FOut_Char(';');
          break;
        }
        case 32: {
          FOut_String(7, (CHAR *) "return");
          if ((void*)DEREF(OEParse_Node, _s, 66700)._left!=(void*)NULL)  {
            if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66734)._left, 66740)._type, 66746)._form, 0x00006000U, 66753)&&DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66802)._left, 66808)._type!=DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _s, 66818)._obj, 66823)._type, 66829)._base)  {
              FOut_String(8, (CHAR *) "(void*)");
            }
            FOut_Char(' ');
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 66931)._left, FALSE);
          }
          FOut_Char(';');
          break;
        }
        case 33: {
          FOut_String(5, (CHAR *) "HALT");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 67040)._left, TRUE);
          FOut_Char(';');
          break;
        }
        case 36: {
          FOut_String(8, (CHAR *) "ASSERT(");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 67141)._right, FALSE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 67198)._left, FALSE);
          FOut_String(3, (CHAR *) ", ");
          FOut_Int(DEREF(OEParse_Node, _s, 67255)._pos, 0);
          FOut_String(3, (CHAR *) ");");
          break;
        }
        default:  {
        HALT(102);
      }
      }
    }
    _s = DEREF(OEParse_Node, _s, 67343)._link;
  }
  if (_paren)  {
    OGenGCC_Off(_off- 1);
    FOut_Char('}');
  }
}

static void OGenGCC_LocalDecl (OTable_Object _obj, OEParse_Node _enter, INTEGER _off) {
  OEParse_Node _local;
  OTable_Object _next;

  void _ProcedureBlock (OEParse_Node _enter, INTEGER _off) {
    OTable_Object _obj;
    INTEGER _i;
    if ((void*)DEREF(OEParse_Node, _enter, 67697)._obj!=(void*)NULL)  {
      _obj = DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 67800)._obj, 67805)._type, 67811)._link;
      while ((void*)_obj!=(void*)NULL)  {
        if (DEREF(OTable_Object, _obj, 67865)._mode==OTable_objVar&&IN(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 67890)._type, 67896)._form, OGenGCC_arraySet, 67903))  {
          OGenGCC_Off(_off+1);
          OGenGCC_OpenArrayDecl(_obj, FALSE);
          FOut_Char(';');
        }
        _obj = DEREF(OTable_Object, _obj, 68046)._link;
      }
    }
    _obj = DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 68131)._obj, 68136)._link, 68142)._right;
    OGenGCC_LocalDecl(_obj, _enter, _off+1);
    if ((void*)DEREF(OEParse_Node, _enter, 68254)._obj!=(void*)NULL)  {
      _obj = DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 68366)._obj, 68371)._type, 68377)._link;
      while ((void*)_obj!=(void*)NULL)  {
        if (DEREF(OTable_Object, _obj, 68431)._mode==OTable_objVar&&IN(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 68456)._type, 68462)._form, OGenGCC_arraySet, 68469))  {
          OGenGCC_Off(_off+1);
          if (DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 68531)._type, 68537)._form==OTable_strArray)  {
            FOut_String(14, (CHAR *) "VALUE_ARRAYF(");
          } else {
            FOut_String(13, (CHAR *) "VALUE_ARRAY(");
          }
          OGenGCC_Ident(_obj);
          FOut_String(4, (CHAR *) ", _");
          OGenGCC_Ident(_obj);
          FOut_String(6, (CHAR *) "_p, 1");
          if (DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 68809)._type, 68815)._form==OTable_strDynArray)  {
            {
              INTEGER _temp_ = OGenGCC_DynDimensions(DEREF(OTable_Object, _obj, 68890)._type)- 1;
              for(_i = 0 ; _i <= _temp_ ; _i += 1) {
                FOut_Char('*');
                FOut_Char('_');
                OGenGCC_Ident(_obj);
                FOut_Char('_');
                FOut_Int((LONGINT) _i, 0);
              }
            }
          }
          FOut_String(3, (CHAR *) ");");
        }
        _obj = DEREF(OTable_Object, _obj, 69149)._link;
      }
    }
    OGenGCC_StatementSeq(DEREF(OEParse_Node, _enter, 69232)._right, _off+1, FALSE);
    if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 69271)._obj, 69276)._type, 69282)._base, 69288)._form!=OTable_strNone)  {
      OGenGCC_Off(_off+1);
      FOut_String(12, (CHAR *) "NO_RETURN (");
      FOut_Int(DEREF(OEParse_Node, _enter, 69388)._pos, 0);
      FOut_String(3, (CHAR *) ");");
    }
  }
  while ((void*)_obj!=(void*)NULL&&DEREF(OTable_Object, _obj, 69532)._mode<OTable_objExtProc)  {
    _obj = OGenGCC_Decl(_obj, 0x00000002U, _off);
  }
  _local = DEREF(OEParse_Node, _enter, 69729)._left;
  while ((void*)_local!=(void*)NULL)  {
    FOut_Ln();
    if (DEREF(OEParse_Node, _local, 69791)._class==OEParse_ndForward&&(void*)DEREF(OEParse_Node, _enter, 69822)._obj!=(void*)NULL)  {
      _next = OGenGCC_Decl(DEREF(OEParse_Node, _local, 69921)._obj, 0x00000020U, _off);
    } else {
      _next = OGenGCC_Decl(DEREF(OEParse_Node, _local, 69989)._obj, 0x00000000U, _off);
    }
    if (DEREF(OEParse_Node, _local, 70031)._class==OEParse_ndForward)  {
      FOut_String(2, (CHAR *) ";");
    } else {
      FOut_String(3, (CHAR *) " {");
      _ProcedureBlock(_local, _off);
      OGenGCC_Off(_off);
      FOut_Char('}');
    }
    _local = DEREF(OEParse_Node, _local, 70258)._link;
  }
}

void OGenGCC_WriteInclude (LONGINT __inclFile_0, const CHAR (* __inclFile_p)) {
  CHAR (* _inclFile);
  CHAR _path[256];
  VALUE_ARRAY(_inclFile, __inclFile_p, 1*__inclFile_0);
  FOut_String(11, (CHAR *) "#include \"");
  Filenames_GetPath(__inclFile_0, _inclFile, 256, (CHAR *) _path, __inclFile_0, _inclFile);
  FOut_String(__inclFile_0, _inclFile);
  FOut_Char('\042');
  FOut_Ln();
}

static void OGenGCC_Include (ODepend_Module _module, BOOLEAN _writeHeader) {
  OTable_Object _obj;
  CHAR _name[256];
  CHAR _path[256];
  ODepend_Import _import;
  if (_writeHeader)  {
    if (!Redir_FindPath(OMachine_redir, 8, (CHAR *) "_OGCC.h", 256, (CHAR *) _path))  {
      Out_String(34, (CHAR *) "Warning: Can't find file _OGCC.h.");
      Out_Ln();
    }
    OGenGCC_WriteInclude(8, (CHAR *) "_OGCC.h");
  } else {
    OGenGCC_WriteInclude(256, (CHAR *) INDEX(DEREF(ODepend_Module, _module, 71463)._file, OGenGCC_header, 5, 71469));
  }
  _obj = DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 71550)._link, 71556)._next;
  while ((void*)_obj!=(void*)NULL)  {
    if (DEREF(OTable_Object, DEREF(OTable_Object, _obj, 71603)._link, 71609)._mnolev<=-3)  {
      if (OTable_external||IN(OTable_flagExport, DEREF(OTable_Object, _obj, 71688)._flags, 71682)==_writeHeader)  {
        OTable_GetModuleName(_obj, 256, (CHAR *) _name);
        _import = DEREF(ODepend_Module, _module, 71782)._import;
        while ((void*)_import!=(void*)NULL&&STRNEQ(DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 71832)._module, 71840)._name, _name))  {
          _import = DEREF(ODepend_Import, _import, 71886)._next;
        }
        if ((void*)_import!=(void*)NULL)  {
          OGenGCC_WriteInclude(256, (CHAR *) INDEX(DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 71973)._module, 71981)._file, OGenGCC_header, 5, 71987));
        } else {
          if (!Redir_FindPathExt(OMachine_redir, 256, (CHAR *) _name, 2, (CHAR *) "h", 256, (CHAR *) _path))  {
            Out_String(26, (CHAR *) "Warning: Can't find file ");
            Out_String(256, (CHAR *) _name);
            Out_String(4, (CHAR *) ".h.");
            Out_Ln();
          }
          OGenGCC_WriteInclude(256, (CHAR *) _path);
        }
      }
    }
    _obj = DEREF(OTable_Object, _obj, 72325)._next;
  }
}

static void OGenGCC_GenHeader (ODepend_Module _mod) {
  OTable_Object _obj;
  OGenGCC_tdGenerated = OTable_external;
  FOut_String(10, (CHAR *) "#ifndef _");
  OGenGCC_Ident(OTable_compiledModule);
  FOut_Char('_');
  FOut_Ln();
  FOut_String(10, (CHAR *) "#define _");
  OGenGCC_Ident(OTable_compiledModule);
  FOut_Char('_');
  FOut_Ln();
  FOut_Ln();
  FOut_String(14, (CHAR *) "#ifdef MODULE");    /* AP 19.1.2026 patch */
  OGenGCC_Ident(OTable_compiledModule);
  FOut_Ln();
  FOut_String(15, (CHAR *) "#define EXTERN");
  OGenGCC_Ident(OTable_compiledModule);
  FOut_Ln();
  FOut_String(6, (CHAR *) "#else");
  FOut_Ln();
  FOut_String(15, (CHAR *) "#define EXTERN");
  OGenGCC_Ident(OTable_compiledModule);
  FOut_String(8, (CHAR *) " extern");
  FOut_Ln();
  FOut_String(7, (CHAR *) "#endif");
  FOut_Ln();
  FOut_Ln();
  OGenGCC_Include(_mod, TRUE);
  OGenGCC_GenTypeDescrForward(DEREF(OEParse_Node, OGenGCC_root, 72947)._link, TRUE);
  _obj = DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 73045)._link, 73051)._right;
  while ((void*)_obj!=(void*)NULL)  {
    _obj = OGenGCC_Decl(_obj, 0x00000003U, 0);
  }
  FOut_Ln();
  OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 73182)._link, TRUE);
  FOut_Ln();
  OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 73268)._link, TRUE);
  FOut_String(18, (CHAR *) "extern void _init");
  OGenGCC_Ident(OTable_compiledModule);
  FOut_String(9, (CHAR *) " (void);");
  FOut_Ln();
  FOut_Ln();
  FOut_String(7, (CHAR *) "#endif");
  FOut_Ln();
}

static void OGenGCC_GenModule (ODepend_Module _mod) {
  OGenGCC_tdGenerated = OTable_external;
  FOut_String(15, (CHAR *) "#define MODULE");   /* AP 19.1.2026 patch */
  OGenGCC_Ident(OTable_compiledModule);
  FOut_Ln();
  FOut_Ln();
  OGenGCC_Include(_mod, FALSE);
  FOut_Ln();
  FOut_String(26, (CHAR *) "static ModuleId moduleId;");
  FOut_Ln();
  OGenGCC_GenTypeDescrForward(DEREF(OEParse_Node, OGenGCC_root, 73863)._link, FALSE);
  OGenGCC_LocalDecl(DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 73910)._link, 73916)._right, OGenGCC_root, 0);
  OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 73961)._link, FALSE);
  FOut_Ln();
  FOut_Ln();
  FOut_String(11, (CHAR *) "void _init");
  OGenGCC_Ident(OTable_compiledModule);
  FOut_String(10, (CHAR *) " (void) {");
  FOut_Ln();
  FOut_String(27, (CHAR *) "  moduleId = add_module (\"");
  FOut_String(48, (CHAR *) DEREF(OTable_Object, OTable_compiledModule, 74173)._name);
  FOut_String(4, (CHAR *) "\");");
  if (!OTable_external)  {
    OGenGCC_GenTypeDescrInit(DEREF(OEParse_Node, OGenGCC_root, 74312)._link);
  }
  OGenGCC_StatementSeq(DEREF(OEParse_Node, OGenGCC_root, 74351)._right, 1, FALSE);
  FOut_Ln();
  FOut_Char('}');
}

void OGenGCC_Module (ODepend_Module _mod, OEParse_Node _stRoot) {

  void _UpdateMarker (LONGINT __modName_0, const CHAR (* __modName_p)) {
    CHAR (* _modName);
    BOOLEAN _ok;
    CHAR _marker[288];
    VALUE_ARRAY(_modName, __modName_p, 1*__modName_0);
    Redir_GeneratePathExt(OMachine_redir, __modName_0, _modName, 2, (CHAR *) "m", 288, (CHAR *) _marker);
    Strings_Insert(7, (CHAR *) "touch ", 0, 288, (CHAR *) _marker);
    _ok = Rts_System((CHAR *) _marker)==0;
  }
  OGenGCC_root = _stRoot;
  if (OTable_external&&STRNEQ(DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, OTable_compiledModule, 75171)._const, 75178)._string, 75186), "C"))  {
    OScan_WarnIns(DEREF(OTable_Object, OTable_compiledModule, 75233)._pos, 297, 256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, OTable_compiledModule, 75261)._const, 75268)._string, 75276));
  }
  OGenGCC_CheckName(OTable_compiledModule, &(DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 75383)._link, 75389)._right));
  if (IN(ODepend_flSymChanged, DEREF(ODepend_Module, _mod, 75430)._flags, 75424)||!IN(OGenGCC_header, DEREF(ODepend_Module, _mod, 75457)._flags, 75451))  {
    ODepend_NewFile(_mod, OGenGCC_header, 2, (CHAR *) "h");
    FOut_Open(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 75590)._file, OGenGCC_header, 5, 75596));
    OGenGCC_GenHeader(_mod);
    FOut_Close();
    INCL(DEREF(ODepend_Module, _mod, 75660)._flags, OGenGCC_header, 75651);
  }
  if (OTable_external)  {
    _UpdateMarker(98, (CHAR *) DEREF(ODepend_Module, _mod, 75732)._name);
  } else {
    ODepend_NewFile(_mod, OGenGCC_cFile, 2, (CHAR *) "c");
    INCL(DEREF(ODepend_Module, _mod, 75802)._flags, OGenGCC_cFile, 75793);
    FOut_Open(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 75863)._file, OGenGCC_cFile, 5, 75869));
    OGenGCC_GenModule(_mod);
    FOut_Ln();
    FOut_Close();
  }
}

static LONGINT OGenGCC_StructSize (OTable_Struct _t) {
  {
    SHORTINT _temp_ = DEREF(OTable_Struct, _t, 76185)._form;
    switch (_temp_) {
      case 1: /* strBool */
      case 2: /* strChar */
      case 12: { /* strSysByte */
        return 1;
        break;
      }
      case 3: { /* strShortInt */
        return 2;
        break;
      }
      case 4: /* strInteger */
      case 6: /* strReal */
      case 8: { /* strSet */
        return 4;
        break;
      }
      case 5: /* strLongInt */
      case 7: /* strLongReal */
      case 13: /* strSysPtr */
      case 15: /* strSysProc */
      case 14: { /* strPointer */
        return 8;
        break;
      }
      case 17:
      case 19: {
        return -1;
        break;
      }
      default:  {
      return -2;
    }
    }
  }
  NO_RETURN (75963);
}

void _init_OGenGCC (void) {
  moduleId = add_module ("OGenGCC");
  OGenGCC_atCount = 0;
  OGenGCC_emptyObj = OTable_NewObject(2, (CHAR *) "@", OTable_objType, -1);
  OTable_structSize = OGenGCC_StructSize;
}
