#define MODULE_OGenGCC

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
static const SET OGenGCC_arraySet = 0x0000000000060000ULL;
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
  if (IN(OTable_flagExternal, DEREF(OTable_Object, _obj, 2558)._flags, 2552)&&IN(DEREF(OTable_Object, _obj, 2581)._mode, 0x0000000000000290ULL, 2588))  {
    if ((void*)DEREF(OTable_Object, _obj, 2651)._extName!=(void*)NULL)  {
      FOut_String(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Object, _obj, 2694)._extName, 2703));
    } else {
      FOut_String(48, (CHAR *) DEREF(OTable_Object, _obj, 2739)._name);
    }
  } else if (((DEREF(OTable_Object, _obj, 2771)._mode==OTable_objType&&DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 2797)._type, 2803)._form<=OTable_strFixedMax)&&DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 2845)._type, 2851)._obj==_obj)||((DEREF(OTable_Object, _obj, 2881)._mode==OTable_objConst&&DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 2908)._type, 2914)._form==OTable_strBool)&&DEREF(OTable_Object, _obj, 2950)._mnolev==-1LL))  {
    FOut_String(48, (CHAR *) DEREF(OTable_Object, _obj, 3100)._name);
  } else if (DEREF(OTable_Object, _obj, 3122)._mode==OTable_objTBProc)  {
    FOut_String(4, (CHAR *) "TB_");
    OGenGCC_TBIdent(_obj, 104, (CHAR *) _name);
    FOut_String(104, (CHAR *) _name);
  } else {
    if (DEREF(OTable_Object, _obj, 3299)._mode==OTable_objField||DEREF(OTable_Object, _obj, 3328)._mnolev>0LL)  {
    } else if (DEREF(OTable_Object, _obj, 3377)._mode!=OTable_objModule)  {
      if (DEREF(OTable_Object, _obj, 3419)._mnolev<=-3LL)  {
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
      INCLI(OGenGCC_atCount, 1LL, 4607);
    }
    if (!IN(OGenGCC_sfChecked, DEREF(OTable_Struct, _t, 4683)._flags, 4679)&&(_checkStruct||(void*)DEREF(OTable_Struct, _t, 4712)._obj==(void*)NULL))  {
      INCL(DEREF(OTable_Struct, _t, 4746)._flags, OGenGCC_sfChecked, 4739);
      if (DEREF(OTable_Struct, _t, 4780)._form==OTable_strRecord&&(void*)DEREF(OTable_Struct, _t, 4806)._obj==(void*)NULL)  {
        _AssignName(16, (CHAR *) _name);
        _new = OTable_NewObject(16, (CHAR *) _name, OTable_objType, -1LL);
        DEREF(OTable_Object, _new, 4983)._type = _t;
        DEREF(OTable_Object, _new, 5019)._next = (* _obj);
        if (_export)  {
          INCL(DEREF(OTable_Object, _new, 5080)._flags, OTable_flagExport, 5071);
        }
        DEREF(OTable_Struct, _t, 5129)._obj = _new;
        (* _obj) = _new;
      } else if (DEREF(OTable_Struct, _t, 5180)._form==OTable_strRecord&&DEREF(OTable_Object, DEREF(OTable_Struct, _t, 5206)._obj, 5211)._mnolev>0LL)  {
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
        FOut_String(7, (CHAR *) "EXTERN");
        OGenGCC_Ident(OTable_compiledModule);
        FOut_Char(' ');
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 9619)._type, FALSE);
        FOut_String(3, (CHAR *) "* ");
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 9690)._type, TRUE);
        FOut_Char(';');
      }
      _td = DEREF(OEParse_Node, _td, 9757)._link;
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
          _obj = OTable_SearchInTree(DEREF(OTable_Struct, _t, 10642)._link, 48, (CHAR *) DEREF(OTable_Object, _tbProc, 10656)._name);
          if (((void*)_obj!=(void*)NULL&&DEREF(OTable_Object, _obj, 10698)._mode==OTable_objTBProc)&&(DEREF(OTable_Object, _obj, 10742)._mnolev==DEREF(OTable_Object, _tbProc, 10759)._mnolev||DEREF(OTable_Object, _obj, 10776)._mark!=OTable_exportNot))  {
            return TRUE;
          } else {
            return _Redefinition(DEREF(OTable_Struct, _t, 11068)._base, _tbProc);
          }
        }
        NO_RETURN (10293);
      }
      if ((void*)_o!=(void*)NULL)  {
        _res = _ScanFields(DEREF(OTable_Object, _o, 11215)._left);
        if ((_res<0&&DEREF(OTable_Object, _o, 11251)._mode==OTable_objTBProc)&&!_Redefinition(DEREF(OTable_Struct, _t, 11291)._base, _o))  {
          if (STREQL(DEREF(OTable_Object, _o, 11324)._name, DEREF(OTable_Object, _obj, 11336)._name))  {
            return _tbCount;
          } else {
            INCI(_tbCount, 1, 11409);
          }
        }
        if (_res<0)  {
          _res = _ScanFields(DEREF(OTable_Object, _o, 11514)._right);
        }
        return _res;
      } else {
        return -1;
      }
      NO_RETURN (10206);
    }
    if ((void*)_t!=(void*)NULL)  {
      _res = _ScanStruct(DEREF(OTable_Struct, _t, 11696)._base);
      if (_res<0)  {
        _res = _ScanFields(DEREF(OTable_Struct, _t, 11815)._link);
      }
      return _res;
    } else {
      return -1;
    }
    NO_RETURN (10125);
  }
  _tbCount = 0;
  return _ScanStruct(OGenGCC_ReceiverRecord(_obj));
  NO_RETURN (9884);
}

static void OGenGCC_GenTypeDescrInit (OEParse_Node _tdList) {
  OEParse_Node _td;
  CHAR _name[104];
  BOOLEAN _size;

  void _GenTBInit (OTable_Object _o) {
    if ((void*)_o!=(void*)NULL)  {
      _GenTBInit(DEREF(OTable_Object, _o, 12240)._left);
      if (DEREF(OTable_Object, _o, 12262)._mode==OTable_objTBProc)  {
        OGenGCC_Off(1);
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 12336)._type, TRUE);
        FOut_String(6, (CHAR *) "->tb_");
        FOut_Int((LONGINT) OGenGCC_TypeDescId(_o), 0LL);
        FOut_String(7, (CHAR *) " = TB_");
        OGenGCC_TBIdent(_o, 104, (CHAR *) _name);
        FOut_String(104, (CHAR *) _name);
        FOut_Char(';');
      }
      _GenTBInit(DEREF(OTable_Object, _o, 12561)._right);
    }
  }
  _td = _tdList;
  while ((void*)_td!=(void*)NULL)  {
    OGenGCC_Off(1);
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 12691)._type, TRUE);
    FOut_String(15, (CHAR *) " = create_td(\"");
    COPY(DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 12755)._type, 12761)._obj, 12766)._name, _name, 104);
    if (INDEX(_name, 2, 104, 12795)=='_'||DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 12812)._type, 12818)._obj, 12823)._scope, 12830)._left, 12836)._mode!=OTable_objModule)  {
      COPYSTRING(&(_name), "");
      _size = FALSE;
    } else {
      _size = TRUE;
    }
    FOut_String(104, (CHAR *) _name);
    FOut_String(4, (CHAR *) "\", ");
    if (_size)  {
      FOut_String(8, (CHAR *) "sizeof(");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 13061)._type, 13067)._obj);
    } else {
      FOut_String(3, (CHAR *) "(0");
    }
    FOut_String(11, (CHAR *) "), sizeof(");
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _td, 13175)._type, FALSE);
    FOut_String(4, (CHAR *) "), ");
    if ((void*)DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 13227)._type, 13233)._base==(void*)NULL)  {
      FOut_String(10, (CHAR *) "NULL, 0);");
    } else {
      OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 13320)._type, 13326)._base, TRUE);
      FOut_String(10, (CHAR *) ", sizeof(");
      OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 13399)._type, 13405)._base, FALSE);
      FOut_String(4, (CHAR *) "));");
    }
    _GenTBInit(DEREF(OTable_Struct, DEREF(OEParse_Node, _td, 13527)._type, 13533)._link);
    _td = DEREF(OEParse_Node, _td, 13556)._link;
  }
}

static INTEGER OGenGCC_DynDimensions (OTable_Struct _t) {
  if (DEREF(OTable_Struct, _t, 13689)._form==OTable_strDynArray)  {
    return OGenGCC_DynDimensions(DEREF(OTable_Struct, _t, 13747)._base)+1;
  } else {
    return 0;
  }
  NO_RETURN (13615);
}

static void OGenGCC_OpenArrayDecl (OTable_Object _o, BOOLEAN _modified) {
  OTable_Struct _base;
  _base = DEREF(OTable_Object, _o, 14520)._type;
  while (DEREF(OTable_Struct, _base, 14543)._form==OTable_strDynArray)  {
    _base = DEREF(OTable_Struct, _base, 14588)._base;
  }
  OGenGCC_Ident(DEREF(OTable_Struct, _base, 14619)._obj);
  FOut_Char(' ');
  if (DEREF(OTable_Struct, DEREF(OTable_Object, _o, 14654)._type, 14660)._form!=OTable_strArray)  {
    FOut_String(4, (CHAR *) "(* ");
  }
  if (_modified)  {
    FOut_Char('_');
    OGenGCC_Ident(_o);
    FOut_String(3, (CHAR *) "_p");
  } else {
    OGenGCC_Ident(_o);
  }
  if (DEREF(OTable_Struct, DEREF(OTable_Object, _o, 14843)._type, 14849)._form!=OTable_strArray)  {
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
    _first = DEREF(OTable_Struct, _pars, 15975)._link;
    FOut_String(3, (CHAR *) " (");
    if ((void*)_first==(void*)NULL)  {
      FOut_String(5, (CHAR *) "void");
    } else {
      while ((void*)_first!=(void*)NULL)  {
        _next = DEREF(OTable_Object, _first, 16183)._link;
        if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _first, 16210)._type, 16216)._form, OGenGCC_arraySet, 16223))  {
          if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _first, 16418)._type, 16424)._flags|DEREF(OTable_Struct, _pars, 16436)._flags, 16409))  {
            {
              INTEGER _temp_ = OGenGCC_DynDimensions(DEREF(OTable_Object, _first, 16543)._type)- 1;
              for(_i = 0 ; _i <= _temp_ ; _i += 1) {
                FOut_String(9, (CHAR *) "LONGINT ");
                FOut_String(2, (CHAR *) "_");
                OGenGCC_Ident(_first);
                FOut_Char('_');
                FOut_Int((LONGINT) _i, 0LL);
                FOut_String(3, (CHAR *) ", ");
              }
            }
          }
          if (DEREF(OTable_Object, _first, 16806)._mode==OTable_objVar)  {
            FOut_String(7, (CHAR *) "const ");
          }
          OGenGCC_OpenArrayDecl(_first, DEREF(OTable_Object, _first, 16950)._mode==OTable_objVar&&!OTable_external);
        } else if ((!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _first, 17026)._type, 17032)._flags|DEREF(OTable_Struct, _pars, 17044)._flags, 17017)&&DEREF(OTable_Object, _first, 17079)._mode==OTable_objVarPar)&&DEREF(OTable_Struct, DEREF(OTable_Object, _first, 17126)._type, 17132)._form==OTable_strRecord)  {
          FOut_String(8, (CHAR *) "struct ");
          OGenGCC_TypeDescrIdent(DEREF(OTable_Object, _first, 17295)._type, FALSE);
          FOut_String(3, (CHAR *) "* ");
          OGenGCC_GenTagName(_first);
          FOut_String(3, (CHAR *) ", ");
          _first = OGenGCC_NamedDecl(_first, 0x0000000000000000ULL, _off);
        } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _first, 17471)._type, 17477)._form==OTable_strNone)  {
          FOut_String(4, (CHAR *) "...");
        } else {
          _first = OGenGCC_NamedDecl(_first, 0x0000000000000000ULL, _off);
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
    return ((void*)DEREF(OTable_Struct, _t, 18137)._obj==(void*)NULL||((DEREF(OTable_Object, DEREF(OTable_Struct, _t, 18295)._obj, 18300)._mnolev>=0LL&&DEREF(OTable_Struct, _t, 18333)._form!=OTable_strRecord)&&!(IN(OGenGCC_ofDefined, DEREF(OTable_Object, DEREF(OTable_Struct, _t, 18390)._obj, 18395)._flags, 18386)||(IN(OTable_flagExport, DEREF(OTable_Object, DEREF(OTable_Struct, _t, 18443)._obj, 18448)._flags, 18439)&&!IN(OGenGCC_flagHeader, _flags, 18472)))))||(IN(OGenGCC_flagTypeDef, _flags, 18584)&&DEREF(OTable_Struct, _t, 18598)._obj==_o);
    NO_RETURN (17919);
  }

  void _TypeLeft (OTable_Struct _t) {
    OTable_Object _first;
    SET _declFlags;
    if ((void*)_t!=(void*)NULL&&!_Structure(_t))  {
      if (DEREF(OTable_Struct, _t, 18943)._form==OTable_strRecord)  {
        if (IN(OTable_flagUnion, DEREF(OTable_Struct, _t, 19072)._flags, 19068))  {
          FOut_String(7, (CHAR *) "union ");
        } else {
          FOut_String(8, (CHAR *) "struct ");
        }
      }
      OGenGCC_Ident(DEREF(OTable_Struct, _t, 19209)._obj);
      FOut_Char(' ');
    } else if ((void*)_t!=(void*)NULL)  {
      {
        SHORTINT _temp_ = DEREF(OTable_Struct, _t, 19367)._form;
        switch (_temp_) {
          case 9: {
            FOut_String(6, (CHAR *) "void ");
            break;
          }
          case 14: {
            _TypeLeft(DEREF(OTable_Struct, _t, 19470)._base);
            if (DEREF(OTable_Struct, DEREF(OTable_Struct, _t, 19494)._base, 19500)._form!=OTable_strDynArray)  {
              FOut_String(4, (CHAR *) "(* ");
            }
            break;
          }
          case 15:
          case 16: {
            _TypeLeft(DEREF(OTable_Struct, _t, 19627)._base);
            if (!IN(OGenGCC_flagFunctHeader, _flags, 19667))  {
              FOut_String(4, (CHAR *) "(* ");
            }
            break;
          }
          case 17: {
            _TypeLeft(DEREF(OTable_Struct, _t, 19768)._base);
            break;
          }
          case 10: {
            FOut_String(6, (CHAR *) "CHAR ");
            break;
          }
          case 18: {
            _base = _t;
            while (DEREF(OTable_Struct, _base, 19895)._form==OTable_strDynArray)  {
              _base = DEREF(OTable_Struct, _base, 19946)._base;
            }
            _TypeLeft(_base);
            FOut_String(4, (CHAR *) "(* ");
            break;
          }
          case 19: {
            if (IN(OTable_flagUnion, DEREF(OTable_Struct, _t, 20075)._flags, 20071))  {
              FOut_String(7, (CHAR *) "union ");
            } else {
              FOut_String(8, (CHAR *) "struct ");
            }
            if ((void*)DEREF(OTable_Struct, _t, 20199)._obj!=(void*)NULL)  {
              OGenGCC_Ident(DEREF(OTable_Struct, _t, 20237)._obj);
            }
            FOut_String(3, (CHAR *) " {");
            if ((void*)DEREF(OTable_Struct, _t, 20302)._base!=(void*)NULL)  {
              OGenGCC_Off(_off+1);
              OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OTable_Struct, _t, 20366)._base, 20372)._obj);
              FOut_String(7, (CHAR *) " base;");
            }
            if (IN(OTable_flagExternal, DEREF(OTable_Struct, _t, 20526)._flags, 20522))  {
              _declFlags = 0x0000000000000200ULL;
            } else {
              _declFlags = 0x0000000000000000ULL;
            }
            _first = DEREF(OTable_Struct, _t, 20660)._link;
            while ((void*)_first!=(void*)NULL)  {
              if (DEREF(OTable_Object, _first, 20722)._mode==OTable_objField)  {
                OGenGCC_Off(_off+1);
                _first = OGenGCC_NamedDecl(_first, _declFlags, _off+1);
                FOut_Char(';');
              } else {
                _first = DEREF(OTable_Object, _first, 20954)._next;
              }
            }
            OGenGCC_Off(_off);
            FOut_String(3, (CHAR *) "} ");
            break;
          }
          default: NO_LABEL (_temp_, 19361);
        }
      }
    }
  }

  void _TypeRight (OTable_Struct _t) {
    if ((void*)_t!=(void*)NULL&&_Structure(_t))  {
      {
        SHORTINT _temp_ = DEREF(OTable_Struct, _t, 21476)._form;
        switch (_temp_) {
          case 14: {
            _right = TRUE;
            if (DEREF(OTable_Struct, DEREF(OTable_Struct, _t, 21548)._base, 21554)._form!=OTable_strDynArray)  {
              FOut_Char(')');
            }
            _TypeRight(DEREF(OTable_Struct, _t, 21645)._base);
            break;
          }
          case 15:
          case 16: {
            _right = TRUE;
            if (!IN(OGenGCC_flagFunctHeader, _flags, 21743))  {
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
            FOut_Int(DEREF(OTable_Struct, _t, 21986)._len, 0LL);
            FOut_Char(']');
            _TypeRight(DEREF(OTable_Struct, _t, 22043)._base);
            break;
          }
          case 18: {
            _base = _t;
            while (DEREF(OTable_Struct, _base, 22118)._form==OTable_strDynArray)  {
              _base = DEREF(OTable_Struct, _base, 22169)._base;
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
  _TypeLeft(DEREF(OTable_Object, _o, 22391)._type);
  if (DEREF(OTable_Object, _o, 22409)._mode==OTable_objVarPar&&!IN(DEREF(OTable_Struct, DEREF(OTable_Object, _o, 22436)._type, 22442)._form, OGenGCC_arraySet, 22449))  {
    FOut_String(4, (CHAR *) "(* ");
  }
  if (!IN(OGenGCC_flagNoName, _flags, 22571))  {
    if (IN(OGenGCC_flagExtName, _flags, 22608))  {
      FOut_String(104, (CHAR *) OGenGCC_extName);
    } else if (IN(OGenGCC_flagOberonName, _flags, 22678))  {
      FOut_String(48, (CHAR *) DEREF(OTable_Object, _o, 22712)._name);
    } else {
      OGenGCC_Ident(_o);
    }
  }
  if (DEREF(OTable_Object, _o, 22825)._mode==OTable_objVarPar&&!IN(DEREF(OTable_Struct, DEREF(OTable_Object, _o, 22852)._type, 22858)._form, OGenGCC_arraySet, 22865))  {
    FOut_Char(')');
  }
  _TypeRight(DEREF(OTable_Object, _o, 22927)._type);
  _obj = _o;
  if (!_right&&(DEREF(OTable_Object, _o, 22969)._mode==OTable_objVar||DEREF(OTable_Object, _o, 22993)._mode==OTable_objVarPar))  {
    while (((((void*)DEREF(OTable_Object, _obj, 23118)._next!=(void*)NULL&&DEREF(OTable_Object, DEREF(OTable_Object, _obj, 23138)._next, 23144)._mode==DEREF(OTable_Object, _o, 23154)._mode)&&DEREF(OTable_Object, DEREF(OTable_Object, _obj, 23168)._next, 23174)._type==DEREF(OTable_Object, _o, 23184)._type)&&DEREF(OTable_Object, _obj, 23210)._mark==DEREF(OTable_Object, DEREF(OTable_Object, _obj, 23222)._next, 23228)._mark)&&!IN(OTable_flagParam, DEREF(OTable_Object, _obj, 23258)._flags, 23252))  {
      _obj = DEREF(OTable_Object, _obj, 23380)._next;
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Ident(_obj);
    }
  }
  return DEREF(OTable_Object, _obj, 23467)._next;
  NO_RETURN (14934);
}

static void OGenGCC_Const (OTable_Const _c, SHORTINT _form, OTable_Object _obj) {
  INTEGER _i;

  void _CharConst (LONGINT _ch, BOOLEAN _quote) {
    if (_quote)  {
      if (_ch>=127LL)  {
        FOut_String(7, (CHAR *) "(CHAR)");
      }
      FOut_Char('\047');
    }
    if ((((_ch<32LL||_ch>=127LL)||_ch==34LL)||_ch==39LL)||_ch==92LL)  {
      FOut_Char('\134');
      FOut_Char((CHAR) (DIVLI(_ch, 64LL, 24457)+48LL));
      FOut_Char((CHAR) (MODLI(DIVLI(_ch, 8LL, 24502), 8LL, 24509)+48LL));
      FOut_Char((CHAR) (MODLI(_ch, 8LL, 24552)+48LL));
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
      while ((INDEX(_str, _i, 64, 25395)!='\000'&&INDEX(_str, _i, 64, 25411)!='E')&&INDEX(_str, _i, 64, 25428)!='.')  {
        INCI(_i, 1, 25452);
      }
      if (INDEX(_str, _i, 64, 25488)!='.')  {
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
      for(_i = 60 ; _i >= _temp_ ; _i += -4) {
        _nibble = 0;
        _bit = 1;
        _j = 0;
        while (_j<4)  {
          if (IN(_i+_j, _set, 25986))  {
            INCI(_nibble, _bit, 26011);
          }
          _bit = _bit*2;
          INCI(_j, 1, 26078);
        }
        if (_nibble<10)  {
          FOut_Char((CHAR) (48+_nibble));
        } else {
          FOut_Char((CHAR) (55+_nibble));
        }
      }
    }
    FOut_String(4, (CHAR *) "ULL");
  }
  if (((void*)_obj!=(void*)NULL&&!IN(OTable_flagExternal, DEREF(OTable_Object, _obj, 26350)._flags, 26344))&&!(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 26374)._type, 26380)._form==OTable_strString&&Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, _obj, 26430)._const, 26437)._string, 26445))==1))  {
    OGenGCC_Ident(_obj);
  } else {
    {
      SHORTINT _temp_ = _form;
      switch (_temp_) {
        case 1: {
          if (DEREF(OTable_Const, _c, 26557)._intval==1LL)  {
            FOut_String(5, (CHAR *) "TRUE");
          } else {
            FOut_String(6, (CHAR *) "FALSE");
          }
          break;
        }
        case 2: {
          _CharConst(DEREF(OTable_Const, _c, 26695)._intval, TRUE);
          break;
        }
        case 3: {
          FOut_Int(DEREF(OTable_Const, _c, 26750)._intval, 0LL);
          break;
        }
        case 4: {
          if (DEREF(OTable_Const, _c, 26798)._intval==-2147483648LL)  {
            FOut_String(3, (CHAR *) "(-");
            FOut_Int(2147483647LL, 0LL);
            FOut_String(4, (CHAR *) "-1)");
          } else {
            FOut_Int(DEREF(OTable_Const, _c, 27077)._intval, 0LL);
          }
          break;
        }
        case 5: {
          if (DEREF(OTable_Const, _c, 27138)._intval==(-9223372036854775807LL-1))  {
            FOut_String(3, (CHAR *) "(-");
            FOut_Int(9223372036854775807LL, 0LL);
            FOut_String(6, (CHAR *) "LL-1)");
          } else {
            FOut_Int(DEREF(OTable_Const, _c, 27419)._intval, 0LL);
            FOut_String(3, (CHAR *) "LL");
          }
          break;
        }
        case 6:
        case 7: {
          _RealConst(DEREF(OTable_Const, _c, 27525)._real, _form==OTable_strLongReal);
          break;
        }
        case 8: {
          _SetConst(DEREF(OTable_Const, _c, 27590)._set);
          break;
        }
        case 10: {
          FOut_Char('\042');
          if (Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, _c, 27665)._string, 27673))==1)  {
            _CharConst((LONGINT) ((INTEGER) INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27713)._string, 27721), 0, 256, 27721)), FALSE);
          } else {
            _i = 0;
            while (INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27783)._string, 27791), _i, 256, 27791)!='\000')  {
              if ((INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27821)._string, 27829), _i, 256, 27829)=='\042'||INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27897)._string, 27905), _i, 256, 27905)=='\134')||INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27995)._string, 28003), _i, 256, 28003)=='?')  {
                FOut_Char('\134');
              }
              FOut_Char(INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 28147)._string, 28155), _i, 256, 28155));
              INCI(_i, 1, 28173);
            }
          }
          FOut_Char('\042');
          break;
        }
        case 11: {
          FOut_String(5, (CHAR *) "NULL");
          break;
        }
        default: NO_LABEL (_temp_, 26514);
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
  if (OTable_external&&DEREF(OTable_Object, _obj, 29190)._mode==OTable_objConst)  {
    _modifier = _dontPrint;
  } else if (IN(OGenGCC_flagAutoPrefix, _flags, 29270))  {
    _modifier = _auto;
  } else if (DEREF(OTable_Object, _obj, 29326)._mnolev>0LL)  {
    _modifier = _noModifier;
  } else {
    if (IN(DEREF(OTable_Object, _obj, 29410)._mode, 0x0000000000000B94ULL, 29417))  {
      if (!IN(OTable_flagExport, DEREF(OTable_Object, _obj, 29591)._flags, 29585))  {
        if (IN(OGenGCC_flagHeader, _flags, 29630))  {
          _modifier = _dontPrint;
        } else {
          _modifier = _static;
        }
      } else {
        if (IN(OGenGCC_flagHeader, _flags, 29777))  {
          _modifier = _extern;
        } else {
          _modifier = _noModifier;
        }
      }
    } else {
      if (IN(OTable_flagExport, DEREF(OTable_Object, _obj, 29966)._flags, 29960)!=IN(OGenGCC_flagHeader, _flags, 29989))  {
        _modifier = _dontPrint;
      } else {
        _modifier = _noModifier;
      }
    }
  }
  if (_modifier!=_dontPrint)  {
    if (IN(DEREF(OTable_Object, _obj, 30162)._mode, 0x0000000000000B80ULL, 30169)&&!OTable_external)  {
      OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 30368)._link, IN(OGenGCC_flagHeader, _flags, 30387));
    }
    OGenGCC_Off(_off);
    if (_modifier==_extern)  {
      FOut_String(8, (CHAR *) "extern ");
    } else if (_modifier==_static)  {
      FOut_String(8, (CHAR *) "static ");
    } else if (_modifier==_auto)  {
      FOut_String(6, (CHAR *) "auto ");
    }
    if (DEREF(OTable_Object, _obj, 30641)._mode==OTable_objConst)  {
      FOut_String(7, (CHAR *) "const ");
      _next = OGenGCC_NamedDecl(_obj, 0x0000000000000000ULL, _off);
      if (!IN(OGenGCC_flagHeader, _flags, 30791))  {
        FOut_String(4, (CHAR *) " = ");
        OGenGCC_Const(DEREF(OTable_Object, _obj, 30899)._const, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 30911)._type, 30917)._form, (OTable_Object)NULL);
      }
    } else if (DEREF(OTable_Object, _obj, 30958)._mode==OTable_objType)  {
      FOut_String(9, (CHAR *) "typedef ");
      _next = OGenGCC_NamedDecl(_obj, _flags|0x0000000000000004ULL, _off);
      INCL(DEREF(OTable_Object, _obj, 31113)._flags, OGenGCC_ofDefined, 31104);
    } else if (DEREF(OTable_Object, _obj, 31149)._mode==OTable_objVar)  {
      _next = OGenGCC_NamedDecl(_obj, _flags|0x0000000000000008ULL, _off);
    } else {
      _next = OGenGCC_NamedDecl(_obj, _flags|0x0000000000000040ULL, _off);
    }
    if (IN(OGenGCC_flagSemicolon, _flags, 31424))  {
      FOut_Char(';');
    }
    return _next;
  } else {
    return DEREF(OTable_Object, _obj, 31514)._next;
  }
  NO_RETURN (28315);
}

static void OGenGCC_VarDecl (LONGINT __name_0, const CHAR (* __name_p), OTable_Struct _type) {
  CHAR (* _name);
  OTable_Object _dummy;
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  if ((void*)DEREF(OTable_Struct, _type, 31649)._obj!=(void*)NULL)  {
    OGenGCC_Ident(DEREF(OTable_Struct, _type, 31684)._obj);
    FOut_Char(' ');
    FOut_String(__name_0, _name);
  } else {
    DEREF(OTable_Object, OGenGCC_emptyObj, 31757)._type = _type;
    COPY(_name, OGenGCC_extName, 104);
    _dummy = OGenGCC_NamedDecl(OGenGCC_emptyObj, 0x0000000000000100ULL, (-2147483647-1));
  }
}

static void OGenGCC_Expr (OEParse_Node _expr, BOOLEAN _paren);

static void OGenGCC_Designator (OEParse_Node _d, BOOLEAN _adr);

static void OGenGCC_ArrayLength (OEParse_Node _desig, LONGINT _dim) {
  INTEGER _ddim;
  OTable_Struct _type;
  _ddim = OGenGCC_DynDimensions(DEREF(OEParse_Node, _desig, 32199)._type);
  if ((LONGINT) _ddim<=_dim)  {
    _type = DEREF(OEParse_Node, _desig, 32306)._type;
    while (_dim!=0LL)  {
      _type = DEREF(OTable_Struct, _type, 32359)._base;
      DECLI(_dim, 1LL, 32375);
    }
    FOut_Int(DEREF(OTable_Struct, _type, 32413)._len, 0LL);
  } else if (DEREF(OEParse_Node, _desig, 32439)._class==OEParse_ndVarPar||DEREF(OEParse_Node, _desig, 32470)._class==OEParse_ndVar)  {
    FOut_String(2, (CHAR *) "_");
    OGenGCC_Ident(DEREF(OEParse_Node, _desig, 32582)._obj);
    FOut_Char('_');
    FOut_Int(_dim, 0LL);
  } else {
    FOut_String(9, (CHAR *) "GET_LEN(");
    while (DEREF(OTable_Struct, DEREF(OEParse_Node, _desig, 32732)._type, 32738)._form==OTable_strDynArray)  {
      _desig = DEREF(OEParse_Node, _desig, 32787)._left;
    }
    OGenGCC_Designator(_desig, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(_dim, 0LL);
    FOut_Char(')');
  }
}

static void OGenGCC_GetArrayPrefix (OEParse_Node _d, OEParse_Node (* _prefix), INTEGER (* _dim)) {
  (* _dim) = -1;
  (* _prefix) = _d;
  while (DEREF(OEParse_Node, (* _prefix), 33385)._class==OEParse_ndIndex)  {
    INCI((* _dim), 1, 33415);
    (* _prefix) = DEREF(OEParse_Node, (* _prefix), 33448)._left;
  }
}

static void OGenGCC_GenTypeTag (OEParse_Node _n) {
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 33583)._type, 33589)._form==OTable_strPointer)  {
    FOut_String(5, (CHAR *) "TAG(");
    OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 33680)._type, 33686)._base, FALSE);
    FOut_String(3, (CHAR *) ", ");
    OGenGCC_Designator(_n, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, _n, 33791)._pos, 0LL);
    FOut_Char(')');
  } else if (DEREF(OEParse_Node, _n, 33833)._class==OEParse_ndVarPar)  {
    OGenGCC_GenTagName(DEREF(OEParse_Node, _n, 33912)._obj);
  } else if (DEREF(OEParse_Node, _n, 33931)._class==OEParse_ndDeref)  {
    FOut_String(5, (CHAR *) "TAG(");
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _n, 34048)._type, FALSE);
    FOut_String(3, (CHAR *) ", ");
    OGenGCC_Designator(DEREF(OEParse_Node, _n, 34106)._left, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, _n, 34159)._pos, 0LL);
    FOut_Char(')');
  } else {
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _n, 34242)._type, TRUE);
  }
}

static void OGenGCC_GenTypeLevel (OTable_Struct _type) {
  INTEGER _level;
  FOut_String(3, (CHAR *) ", ");
  if (DEREF(OTable_Struct, _type, 34439)._form==OTable_strPointer)  {
    _type = DEREF(OTable_Struct, _type, 34485)._base;
  }
  _level = -1;
  do  {
    INCI(_level, 1, 34535);
    _type = DEREF(OTable_Struct, _type, 34566)._base;
  } while (!((void*)_type==(void*)NULL));
  FOut_Int((LONGINT) _level, 0LL);
}

static void OGenGCC_GenTypeTest (OEParse_Node _expr, OTable_Struct _type) {
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 34720)._type, 34726)._form==OTable_strPointer)  {
    FOut_String(15, (CHAR *) "type_test_ptr(");
    OGenGCC_Expr(_expr, FALSE);
  } else {
    FOut_String(11, (CHAR *) "type_test(");
    OGenGCC_GenTypeTag(_expr);
  }
  FOut_String(3, (CHAR *) ", ");
  OGenGCC_TypeDescrIdent(_type, TRUE);
  OGenGCC_GenTypeLevel(_type);
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 34978)._type, 34984)._form==OTable_strPointer)  {
    FOut_String(13, (CHAR *) ", moduleId, ");
    FOut_Int(DEREF(OEParse_Node, _expr, 35062)._pos, 0LL);
  }
  FOut_Char(')');
}

static void OGenGCC_TypeDesign (OTable_Struct _type, BOOLEAN _arrayToPtr) {
  OTable_Object _dummy;
  BOOLEAN _toPointer;
  _toPointer = _arrayToPtr&&IN(DEREF(OTable_Struct, _type, 35277)._form, OGenGCC_arraySet, 35284);
  while (IN(DEREF(OTable_Struct, _type, 35313)._form, OGenGCC_arraySet, 35320))  {
    _type = DEREF(OTable_Struct, _type, 35354)._base;
  }
  if (_toPointer)  {
    FOut_Char('(');
  }
  if ((void*)DEREF(OTable_Struct, _type, 35432)._obj!=(void*)NULL)  {
    OGenGCC_Ident(DEREF(OTable_Struct, _type, 35467)._obj);
  } else {
    DEREF(OTable_Object, OGenGCC_emptyObj, 35497)._type = _type;
    _dummy = OGenGCC_NamedDecl(OGenGCC_emptyObj, 0x0000000000000080ULL, (-2147483647-1));
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
    return (((_adr&&DEREF(OEParse_Node, _d, 35843)._class!=OEParse_ndDeref)&&DEREF(OEParse_Node, _d, 35868)._class!=OEParse_ndVarPar)&&DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 35908)._type, 35914)._form!=OTable_strDynArray)&&!(DEREF(OEParse_Node, _d, 35958)._class==OEParse_ndIndex&&DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 35983)._left, 35989)._type, 35995)._form==OTable_strDynArray);
    NO_RETURN (35789);
  }

  BOOLEAN _FindField (OTable_Struct _t, OTable_Object _obj) {
    OTable_Object _o;
    _o = DEREF(OTable_Struct, _t, 36154)._link;
    while ((void*)_o!=(void*)NULL)  {
      if (_o==_obj)  {
        return TRUE;
      }
      _o = DEREF(OTable_Object, _o, 36262)._next;
    }
    return FALSE;
    NO_RETURN (36052);
  }

  void _DesignRG (void) {
    if ((DEREF(OEParse_Node, _d, 36363)._class==OEParse_ndVar||DEREF(OEParse_Node, _d, 36387)._class==OEParse_ndVarPar)&&DEREF(OTable_Object, DEREF(OEParse_Node, _d, 36414)._obj, 36419)._type!=DEREF(OEParse_Node, _d, 36429)._type)  {
      FOut_String(3, (CHAR *) "((");
      OGenGCC_TypeDesign(DEREF(OEParse_Node, _d, 36611)._type, FALSE);
      if (DEREF(OEParse_Node, _d, 36640)._class==OEParse_ndVarPar)  {
        FOut_Char('*');
      }
      FOut_Char(')');
      OGenGCC_Ident(DEREF(OEParse_Node, _d, 36741)._obj);
      FOut_Char(')');
    } else {
      OGenGCC_Ident(DEREF(OEParse_Node, _d, 36797)._obj);
    }
  }
  if ((void*)_d!=(void*)NULL)  {
    if (_AdrPrefix())  {
      FOut_String(3, (CHAR *) "&(");
    }
    if (DEREF(OEParse_Node, _d, 36939)._class==OEParse_ndDeref)  {
      if (_adr||DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 36985)._type, 36991)._form==OTable_strDynArray)  {
        FOut_String(11, (CHAR *) "CHECK_NIL(");
      } else {
        FOut_String(7, (CHAR *) "DEREF(");
      }
      OGenGCC_TypeDesign(DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 37131)._left, 37137)._type, TRUE);
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Designator(DEREF(OEParse_Node, _d, 37198)._left, FALSE);
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _d, 37255)._pos, 0LL);
      FOut_Char(')');
    } else if (DEREF(OEParse_Node, _d, 37301)._class==OEParse_ndIndex)  {
      OGenGCC_GetArrayPrefix(_d, &(_prefix), &(_dim));
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 37419)._left, 37425)._type, 37431)._form==OTable_strArray)  {
        FOut_String(7, (CHAR *) "INDEX(");
        OGenGCC_Designator(DEREF(OEParse_Node, _d, 37536)._left, FALSE);
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_Expr(DEREF(OEParse_Node, _d, 37596)._right, FALSE);
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
        FOut_String(3, (CHAR *) ", ");
        FOut_Int(DEREF(OEParse_Node, _d, 37722)._pos, 0LL);
        FOut_Char(')');
      } else {
        if (!_adr)  {
          FOut_String(4, (CHAR *) "(* ");
        }
        if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 37920)._left, 37926)._type, 37932)._flags, 37916))  {
          FOut_String(15, (CHAR *) "PTR_INDEX_EXT(");
          OGenGCC_Designator(DEREF(OEParse_Node, _d, 38083)._left, TRUE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_Expr(DEREF(OEParse_Node, _d, 38146)._right, FALSE);
        } else {
          FOut_String(11, (CHAR *) "PTR_INDEX(");
          OGenGCC_Designator(DEREF(OEParse_Node, _d, 38293)._left, TRUE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_Expr(DEREF(OEParse_Node, _d, 38356)._right, FALSE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
          FOut_String(3, (CHAR *) ", ");
          FOut_Char('1');
          _type = DEREF(OEParse_Node, _d, 38517)._type;
          while (DEREF(OTable_Struct, _type, 38548)._form==OTable_strDynArray)  {
            INCI(_dim, 1, 38589);
            FOut_Char('*');
            OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
            _type = DEREF(OTable_Struct, _type, 38695)._base;
          }
          FOut_String(3, (CHAR *) ", ");
          FOut_Int(DEREF(OEParse_Node, _d, 38768)._pos, 0LL);
        }
        FOut_Char(')');
        if (!_adr)  {
          FOut_Char(')');
        }
      }
    } else if (DEREF(OEParse_Node, _d, 38905)._class==OEParse_ndGuard)  {
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 38944)._type, 38950)._form==OTable_strRecord)  {
        FOut_String(4, (CHAR *) "REC");
      } else {
        FOut_String(4, (CHAR *) "PTR");
      }
      FOut_String(13, (CHAR *) "_TYPE_GUARD(");
      OGenGCC_TypeDesign(DEREF(OEParse_Node, _d, 39113)._type, FALSE);
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Designator(DEREF(OEParse_Node, _d, 39175)._left, DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 39184)._type, 39190)._form==OTable_strRecord);
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 39226)._type, 39232)._form==OTable_strRecord)  {
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_GenTypeTag(DEREF(OEParse_Node, _d, 39309)._left);
      }
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _d, 39380)._type, TRUE);
      OGenGCC_GenTypeLevel(DEREF(OEParse_Node, _d, 39418)._type);
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _d, 39468)._pos, 0LL);
      FOut_Char(')');
    } else if (DEREF(OEParse_Node, _d, 39514)._class==OEParse_ndTBSuper)  {
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 39596)._left, 39602)._type, 39608)._form==OTable_strPointer)  {
        OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 39663)._left, 39669)._type, 39675)._base, 39681)._base, TRUE);
      } else {
        OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 39735)._left, 39741)._type, 39747)._base, TRUE);
      }
      FOut_String(6, (CHAR *) "->tb_");
      FOut_Int((LONGINT) OGenGCC_TypeDescId(DEREF(OEParse_Node, _d, 39830)._obj), 0LL);
    } else if (DEREF(OEParse_Node, _d, 39855)._class==OEParse_ndTBProc)  {
      OGenGCC_GenTypeTag(DEREF(OEParse_Node, _d, 39944)._left);
      FOut_String(6, (CHAR *) "->tb_");
      FOut_Int((LONGINT) OGenGCC_TypeDescId(DEREF(OEParse_Node, _d, 40009)._obj), 0LL);
    } else {
      OGenGCC_Designator(DEREF(OEParse_Node, _d, 40052)._left, FALSE);
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _d, 40082)._class;
        switch (_temp_) {
          case 1: {
            _DesignRG();
            break;
          }
          case 11:
          case 7:
          case 8: {
            OGenGCC_Ident(DEREF(OEParse_Node, _d, 40188)._obj);
            break;
          }
          case 2: {
            if (_adr||IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 40239)._type, 40245)._form, OGenGCC_arraySet, 40252))  {
              _DesignRG();
            } else {
              FOut_String(4, (CHAR *) "(* ");
              _DesignRG();
              FOut_Char(')');
            }
            break;
          }
          case 3: {
            _type = DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 40601)._left, 40607)._type;
            while (!_FindField(_type, DEREF(OEParse_Node, _d, 40650)._obj))  {
              FOut_String(6, (CHAR *) ".base");
              _type = DEREF(OTable_Struct, _type, 40716)._base;
            }
            FOut_String(2, (CHAR *) ".");
            if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 40797)._left, 40803)._type, 40809)._flags, 40793))  {
              FOut_String(48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, _d, 40915)._obj, 40920)._name);
            } else {
              OGenGCC_Ident(DEREF(OEParse_Node, _d, 40963)._obj);
            }
            break;
          }
          default: NO_LABEL (_temp_, 40076);
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
  if (DEREF(OEParse_Node, _expr, 41258)._type==_fType)  {
    OGenGCC_Expr(_expr, FALSE);
  } else if (DEREF(OTable_Struct, _fType, 41320)._form==OTable_strPointer)  {
    FOut_Char('(');
    OGenGCC_TypeDesign(_fType, FALSE);
    FOut_Char(')');
    OGenGCC_Expr(_expr, FALSE);
  } else {
    OGenGCC_Expr(_expr, FALSE);
    _type = DEREF(OEParse_Node, _expr, 41532)._type;
    do  {
      FOut_String(6, (CHAR *) ".base");
      _type = DEREF(OTable_Struct, _type, 41601)._base;
    } while (!(_type==_fType));
  }
}

static void OGenGCC_ProcCall (OEParse_Node _call) {
  OEParse_Node _apar, _prefix;
  OTable_Object _fpar;
  OTable_Struct _base, _t;
  INTEGER _i, _dim;
  BOOLEAN _tbCall;
  _apar = DEREF(OEParse_Node, _call, 41850)._right;
  _fpar = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41914)._left, 41920)._type, 41926)._link;
  _tbCall = DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41981)._left, 41987)._class==OEParse_ndTBSuper||DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42018)._left, 42024)._class==OEParse_ndTBProc;
  if (_tbCall)  {
    _t = DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42233)._left, 42239)._left, 42245)._type;
    if (DEREF(OTable_Struct, _t, 42264)._form==OTable_strPointer)  {
      _t = DEREF(OTable_Struct, _t, 42306)._base;
    }
    DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42334)._left, 42340)._obj = OTable_FindField(48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42366)._left, 42372)._obj, 42377)._name, _t);
    DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42399)._left, 42405)._type = DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42419)._left, 42425)._obj, 42430)._type;
    _fpar = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42456)._left, 42462)._type, 42468)._link;
    DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42566)._left, 42572)._left, 42578)._link = _apar;
    _apar = DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42612)._left, 42618)._left;
    if (DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42640)._left, 42646)._class==OEParse_ndTBSuper)  {
      _fpar = OTable_FindField(48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42801)._left, 42807)._obj, 42812)._name, DEREF(OTable_Struct, _t, 42821)._base);
      _fpar = DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 42850)._type, 42856)._link;
    }
  }
  OGenGCC_Designator(DEREF(OEParse_Node, _call, 42961)._left, FALSE);
  FOut_Char('(');
  while ((void*)_apar!=(void*)NULL)  {
    if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 43035)._type, 43041)._form==OTable_strDynArray&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 43072)._type, 43078)._base, 43084)._form==OTable_strSysByte)  {
      if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 43250)._type, 43256)._flags|DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 43268)._left, 43274)._type, 43280)._flags, 43242))  {
        FOut_String(8, (CHAR *) "(BYTE*)");
      } else {
        if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43495)._type, 43501)._form==OTable_strString)  {
          FOut_Int((LONGINT) (Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _apar, 43563)._conval, 43571)._string, 43579))+1), 0LL);
        } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43610)._type, 43616)._form==OTable_strDynArray&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43681)._type, 43687)._flags, 43674))  {
          FOut_Int(OMachine_maxLInt, 0LL);
        } else {
          FOut_String(8, (CHAR *) "sizeof(");
          OGenGCC_TypeDesign(DEREF(OEParse_Node, _apar, 43861)._type, FALSE);
          FOut_Char(')');
          if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43923)._type, 43929)._form, OGenGCC_arraySet, 43936))  {
            _t = DEREF(OEParse_Node, _apar, 43977)._type;
            OGenGCC_GetArrayPrefix(_apar, &(_prefix), &(_dim));
            while (IN(DEREF(OTable_Struct, _t, 44057)._form, OGenGCC_arraySet, 44064))  {
              INCI(_dim, 1, 44096);
              FOut_Char('*');
              OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
              _t = DEREF(OTable_Struct, _t, 44202)._base;
            }
          }
        }
        FOut_String(10, (CHAR *) ", (BYTE*)");
      }
      OGenGCC_Designator(_apar, TRUE);
    } else if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 44353)._type, 44359)._form, OGenGCC_arraySet, 44366))  {
      if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 44442)._type, 44448)._flags|DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 44460)._left, 44466)._type, 44472)._flags, 44434)&&DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 44501)._type, 44507)._form==OTable_strDynArray)  {
        if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44554)._type, 44560)._form==OTable_strString)  {
          FOut_Int((LONGINT) (Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _apar, 44622)._conval, 44630)._string, 44638))+1), 0LL);
          FOut_String(3, (CHAR *) ", ");
        } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44697)._type, 44703)._form==OTable_strDynArray&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44768)._type, 44774)._flags, 44761))  {
          FOut_Int(OMachine_maxLInt, 0LL);
          FOut_String(3, (CHAR *) ", ");
        } else {
          OGenGCC_GetArrayPrefix(_apar, &(_prefix), &(_dim));
          {
            INTEGER _temp_ = OGenGCC_DynDimensions(DEREF(OTable_Object, _fpar, 45008)._type);
            for(_i = 1 ; _i <= _temp_ ; _i += 1) {
              OGenGCC_ArrayLength(_prefix, (LONGINT) (_i+_dim));
              FOut_String(3, (CHAR *) ", ");
            }
          }
        }
      }
      if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 45152)._type, 45158)._form==OTable_strDynArray&&DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 45189)._type, 45195)._form==OTable_strDynArray)  {
        OGenGCC_Designator(_apar, TRUE);
      } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 45277)._type, 45283)._form==OTable_strDynArray)  {
        _base = DEREF(OTable_Object, _fpar, 45417)._type;
        while (DEREF(OTable_Struct, _base, 45446)._form==OTable_strDynArray)  {
          _base = DEREF(OTable_Struct, _base, 45497)._base;
        }
        FOut_Char('(');
        OGenGCC_Ident(DEREF(OTable_Struct, _base, 45621)._obj);
        FOut_String(5, (CHAR *) " *) ");
        OGenGCC_Expr(_apar, FALSE);
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 45706)._type, 45712)._form==OTable_strString&&(void*)DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 45741)._type, 45747)._obj!=(void*)NULL)  {
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_Expr(_apar, FALSE);
      } else {
        OGenGCC_Expr(_apar, FALSE);
      }
    } else if (DEREF(OTable_Object, _fpar, 45953)._mode==OTable_objVarPar)  {
      if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 46041)._type, 46047)._flags|DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 46059)._left, 46065)._type, 46071)._flags, 46033)&&DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 46100)._type, 46106)._form==OTable_strRecord)  {
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_GenTypeTag(_apar);
        FOut_String(3, (CHAR *) ", ");
        if (DEREF(OEParse_Node, _apar, 46270)._type!=DEREF(OTable_Object, _fpar, 46283)._type)  {
          FOut_String(2, (CHAR *) "(");
          OGenGCC_TypeDesign(DEREF(OTable_Object, _fpar, 46352)._type, FALSE);
          FOut_String(3, (CHAR *) "*)");
        }
        OGenGCC_Designator(_apar, TRUE);
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 46464)._type, 46470)._form==OTable_strPointer&&DEREF(OEParse_Node, _apar, 46515)._class==OEParse_ndGuard)  {
        FOut_String(2, (CHAR *) "(");
        OGenGCC_Designator(_apar, FALSE);
        FOut_String(13, (CHAR *) ", (void*)(&(");
        OGenGCC_Designator(DEREF(OEParse_Node, _apar, 46737)._left, FALSE);
        FOut_String(4, (CHAR *) ")))");
      } else if (DEREF(OTable_Object, _fpar, 46799)._type!=DEREF(OEParse_Node, _apar, 46812)._type&&DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 46827)._type, 46833)._form==OTable_strSysPtr)  {
        FOut_String(7, (CHAR *) "(PTR*)");
        OGenGCC_Designator(_apar, TRUE);
      } else {
        OGenGCC_Designator(_apar, TRUE);
      }
    } else {
      if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 47137)._type, 47143)._form, 0x0000000000084000ULL, 47150)&&DEREF(OEParse_Node, _apar, 47189)._type!=DEREF(OTable_Object, _fpar, 47202)._type)  {
        OGenGCC_Projection(_apar, DEREF(OTable_Object, _fpar, 47247)._type);
      } else {
        OGenGCC_Expr(_apar, FALSE);
      }
    }
    _apar = DEREF(OEParse_Node, _apar, 47338)._link;
    if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 47360)._type, 47366)._form!=OTable_strNone)  {
      _fpar = DEREF(OTable_Object, _fpar, 47411)._link;
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
  if ('A'<=CAP((* PTR_INDEX(_oc, 0, __oc_0, 1, 48763)))&&CAP((* PTR_INDEX(_oc, 0, __oc_0, 1, 48779)))<='Z')  {
    FOut_String(__oc_0, _oc);
    if (IN(OGenGCC_fAddType, _flags, 48854))  {
      {
        SHORTINT _temp_ = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 48883)._left, 48889)._type, 48895)._form;
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
          default: NO_LABEL (_temp_, 48877);
        }
      }
      FOut_String(4, (CHAR *) _app);
    }
    FOut_Char('(');
    if (IN(OGenGCC_fLeftAdr, _flags, 49235))  {
      OGenGCC_Designator(DEREF(OEParse_Node, _n, 49271)._left, TRUE);
    } else {
      OGenGCC_Expr(DEREF(OEParse_Node, _n, 49311)._left, FALSE);
    }
    if (!IN(OGenGCC_fMonadic, _flags, 49357))  {
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Expr(DEREF(OEParse_Node, _n, 49412)._right, FALSE);
    }
    if (IN(OGenGCC_fAddPosition, _flags, 49462))  {
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _n, 49518)._pos, 0LL);
    }
    if (IN(OGenGCC_fAddTypename, _flags, 49562))  {
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49618)._left, 49624)._type, 49630)._obj);
    }
    if (IN(OGenGCC_fAddSize, _flags, 49667))  {
      FOut_String(10, (CHAR *) ", sizeof(");
      OGenGCC_Designator(DEREF(OEParse_Node, _n, 49735)._left, FALSE);
      FOut_Char(')');
    }
    FOut_Char(')');
  } else {
    if (IN(OGenGCC_fMonadic, _flags, 49882))  {
      FOut_String(__oc_0, _oc);
      OGenGCC_Expr(DEREF(OEParse_Node, _n, 49935)._left, _pe<=_pl);
    } else {
      if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49977)._left, 49983)._type, 49989)._form, 0x0000000000006000ULL, 49996)&&DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 50043)._left, 50049)._type!=DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 50059)._right, 50066)._type)  {
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 50128)._left, _pe<=_pl);
        FOut_String(__oc_0, _oc);
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 50221)._right, _pe<=_pr);
      } else {
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 50270)._left, _pe<=_pl);
        FOut_String(__oc_0, _oc);
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 50331)._right, _pe<=_pr);
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
      return -32768;
    } else if (DEREF(OEParse_Node, _expr, 50655)._class==OEParse_ndConst&&((IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50694)._type, 50700)._form, OEParse_intSet, 50707)&&DEREF(OTable_Const, DEREF(OEParse_Node, _expr, 50727)._conval, 50735)._intval<0LL)||(IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50768)._type, 50774)._form, OEParse_realSet, 50781)&&DEREF(OTable_Const, DEREF(OEParse_Node, _expr, 50802)._conval, 50810)._real<0.0000000000000000E+00)))  {
      return 2;
    } else if (DEREF(OEParse_Node, _expr, 50860)._class<OEParse_ndMOp)  {
      return 0;
    } else {
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 50965)._subcl;
        switch (_temp_) {
          case 17:
          case 45:
          case 24: {
            return 2;
            break;
          }
          case 1 ... 4: {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 51079)._type, 51085)._form==OTable_strSet)  {
              {
                SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 51130)._subcl;
                switch (_temp_) {
                  case 1: {
                    return 8;
                    break;
                  }
                  case 2: {
                    return 9;
                    break;
                  }
                  default: NO_LABEL (_temp_, 51121);
                }
              }
            } else {
              return 3;
            }
            break;
          }
          case 6: {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 51314)._type, 51320)._form==OTable_strSet)  {
              return 10;
            } else {
              return 4;
            }
            break;
          }
          case 7: {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 51455)._type, 51461)._form==OTable_strSet)  {
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
    NO_RETURN (50542);
  }

  BOOLEAN _String (OEParse_Node _n) {
    return DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 52149)._type, 52155)._form==OTable_strString||(IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 52196)._type, 52202)._form, OGenGCC_arraySet, 52209)&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 52226)._type, 52232)._base, 52238)._form==OTable_strChar);
    NO_RETURN (52016);
  }
  if (_paren)  {
    FOut_Char('(');
  }
  _pe = _Pri(_expr);
  _pl = _Pri(DEREF(OEParse_Node, _expr, 52368)._left);
  _pr = _Pri(DEREF(OEParse_Node, _expr, 52396)._right);
  if (DEREF(OEParse_Node, _expr, 52418)._class<OEParse_ndConst)  {
    OGenGCC_Designator(_expr, FALSE);
  } else if (DEREF(OEParse_Node, _expr, 52490)._class==OEParse_ndConst)  {
    OGenGCC_Const(DEREF(OEParse_Node, _expr, 52533)._conval, DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 52547)._type, 52553)._form, DEREF(OEParse_Node, _expr, 52565)._obj);
  } else if (DEREF(OEParse_Node, _expr, 52587)._class==OEParse_ndUpto)  {
    FOut_String(7, (CHAR *) "RANGE(");
    OGenGCC_Expr(DEREF(OEParse_Node, _expr, 52655)._left, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 52711)._left, 52717)._pos, 0LL);
    FOut_String(3, (CHAR *) ", ");
    OGenGCC_Expr(DEREF(OEParse_Node, _expr, 52767)._right, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 52824)._right, 52831)._pos, 0LL);
    FOut_Char(')');
  } else if (DEREF(OEParse_Node, _expr, 52876)._class==OEParse_ndMOp&&DEREF(OEParse_Node, _expr, 52902)._subcl==OEParse_scConv)  {
    FOut_String(2, (CHAR *) "(");
    OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 53015)._type, 53021)._obj);
    FOut_String(3, (CHAR *) ") ");
    OGenGCC_Expr(DEREF(OEParse_Node, _expr, 53068)._left, _pl>=2);
  } else if (DEREF(OEParse_Node, _expr, 53098)._class==OEParse_ndMOp&&DEREF(OEParse_Node, _expr, 53124)._subcl==OEParse_scVal)  {
    if (DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 53313)._left, 53319)._class<OEParse_ndConst)  {
      FOut_String(5, (CHAR *) "(*((");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 53408)._type, 53414)._obj);
      FOut_String(4, (CHAR *) "*)&");
      OGenGCC_Expr(DEREF(OEParse_Node, _expr, 53466)._left, TRUE);
      FOut_String(3, (CHAR *) "))");
    } else {
      FOut_String(7, (CHAR *) "VAL_E(");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 53611)._type, 53617)._obj);
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Expr(DEREF(OEParse_Node, _expr, 53668)._left, TRUE);
      FOut_Char(')');
    }
  } else if (DEREF(OEParse_Node, _expr, 53729)._class==OEParse_ndMOp&&DEREF(OEParse_Node, _expr, 53755)._subcl==OEParse_scAdr)  {
    FOut_String(10, (CHAR *) "(LONGINT)");
    OGenGCC_Designator(DEREF(OEParse_Node, _expr, 53831)._left, TRUE);
  } else if (DEREF(OEParse_Node, _expr, 53860)._class==OEParse_ndMOp)  {
    _flags = 0x0000000000000000ULL;
    {
      SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 53918)._subcl;
      switch (_temp_) {
        case 7: {
          if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 53964)._type, 53970)._form!=OTable_strSet)  {
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
          _flags = 0x0000000000000003ULL;
          break;
        }
        case 22: {
          COPYSTRING(&(_str), "CAP");
          break;
        }
        case 14: {
          COPYSTRING(&(_str), "ENTIER");
          _flags = 0x0000000000000001ULL;
          break;
        }
        case 23: {
          COPYSTRING(&(_str), "ODD");
          _flags = 0x0000000000000001ULL;
          break;
        }
        case 20: {
          COPYSTRING(&(_str), "sizeof");
          break;
        }
        default: NO_LABEL (_temp_, 53909);
      }
    }
    OGenGCC_WriteOperation(_expr, 8, (CHAR *) _str, _flags|0x0000000000000004ULL, _pe, _pl, _pr);
  } else if (DEREF(OEParse_Node, _expr, 54455)._class==OEParse_ndDOp&&DEREF(OEParse_Node, _expr, 54481)._subcl==OEParse_scLen)  {
    OGenGCC_ArrayLength(DEREF(OEParse_Node, _expr, 54528)._left, DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 54540)._right, 54547)._conval, 54555)._intval);
  } else if (DEREF(OEParse_Node, _expr, 54580)._class==OEParse_ndDOp&&DEREF(OEParse_Node, _expr, 54606)._subcl==OEParse_scIs)  {
    OGenGCC_GenTypeTest(DEREF(OEParse_Node, _expr, 54670)._left, DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 54682)._right, 54689)._type);
  } else if (DEREF(OEParse_Node, _expr, 54712)._class==OEParse_ndDOp)  {
    _flags = 0x0000000000000000ULL;
    if (_String(DEREF(OEParse_Node, _expr, 54776)._left))  {
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 54831)._subcl;
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
          default: NO_LABEL (_temp_, 54822);
        }
      }
      Strings_Insert(4, (CHAR *) "STR", 0, 8, (CHAR *) _str);
    } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 55099)._type, 55105)._form==OTable_strSet)  {
      if (DEREF(OEParse_Node, _expr, 55168)._subcl==OEParse_scMinus)  {
        COPYSTRING(&(_str), "SETDIFF");
      } else {
        {
          SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 55253)._subcl;
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
            default: NO_LABEL (_temp_, 55244);
          }
        }
      }
    } else {
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 55418)._subcl;
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
            _flags = 0x0000000000000003ULL;
            break;
          }
          case 4: {
            COPYSTRING(&(_str), "MOD");
            _flags = 0x0000000000000003ULL;
            break;
          }
          case 2: {
            COPYSTRING(&(_str), "DIV");
            _flags = 0x0000000000000003ULL;
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
            _flags = 0x0000000000000002ULL;
            break;
          }
          case 26: {
            COPYSTRING(&(_str), "ASH");
            _flags = 0x0000000000000003ULL;
            break;
          }
          case 27: {
            COPYSTRING(&(_str), "BIT");
            _flags = 0x0000000000000000ULL;
            break;
          }
          case 28: {
            COPYSTRING(&(_str), "LSH");
            _flags = 0x0000000000000001ULL;
            break;
          }
          case 29: {
            COPYSTRING(&(_str), "ROT");
            _flags = 0x0000000000000001ULL;
            break;
          }
          default: NO_LABEL (_temp_, 55409);
        }
      }
    }
    OGenGCC_WriteOperation(_expr, 8, (CHAR *) _str, _flags, _pe, _pl, _pr);
  } else if (DEREF(OEParse_Node, _expr, 56337)._class==OEParse_ndCall)  {
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
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, _l, 57120)._type, 57126)._form==OTable_strRecord&&(DEREF(OEParse_Node, _l, 57153)._class==OEParse_ndVarPar||DEREF(OEParse_Node, _l, 57180)._class==OEParse_ndDeref))  {
      FOut_String(13, (CHAR *) "ASSERT_TYPE(");
      if (DEREF(OEParse_Node, _l, 57333)._class==OEParse_ndVarPar)  {
        OGenGCC_GenTagName(DEREF(OEParse_Node, _l, 57383)._obj);
      } else {
        FOut_String(5, (CHAR *) "TAG(");
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _l, 57488)._type, FALSE);
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_Designator(DEREF(OEParse_Node, _l, 57554)._left, FALSE);
        FOut_String(3, (CHAR *) ", ");
        FOut_Int(DEREF(OEParse_Node, _l, 57615)._pos, 0LL);
        FOut_Char(')');
      }
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _l, 57712)._type, TRUE);
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _l, 57768)._pos, 0LL);
      FOut_String(3, (CHAR *) ");");
      OGenGCC_Off(_off);
    }
    OGenGCC_Designator(_l, FALSE);
    FOut_String(4, (CHAR *) " = ");
    OGenGCC_Projection(_r, DEREF(OEParse_Node, _l, 57908)._type);
  }

  void _CaseLabel (OEParse_Node _l, SHORTINT _form) {
    LONGINT _b;
    OGenGCC_Const(DEREF(OEParse_Node, _l, 58053)._conval, _form, (OTable_Object)NULL);
    if (DEREF(OTable_Const, DEREF(OEParse_Node, _l, 58086)._conval, 58094)._intval!=DEREF(OTable_Const, DEREF(OEParse_Node, _l, 58104)._conval, 58112)._intval2)  {
      FOut_String(6, (CHAR *) " ... ");
      _b = DEREF(OTable_Const, DEREF(OEParse_Node, _l, 58170)._conval, 58178)._intval;
      DEREF(OTable_Const, DEREF(OEParse_Node, _l, 58197)._conval, 58205)._intval = DEREF(OTable_Const, DEREF(OEParse_Node, _l, 58218)._conval, 58226)._intval2;
      OGenGCC_Const(DEREF(OEParse_Node, _l, 58253)._conval, _form, (OTable_Object)NULL);
      DEREF(OTable_Const, DEREF(OEParse_Node, _l, 58284)._conval, 58292)._intval = _b;
    }
  }
  if (_paren)  {
    FOut_String(3, (CHAR *) " {");
  }
  while ((void*)_s!=(void*)NULL)  {
    OGenGCC_Off(_off);
    {
      SHORTINT _temp_ = DEREF(OEParse_Node, _s, 58443)._class;
      switch (_temp_) {
        case 16: {
          if (DEREF(OEParse_Node, _s, 58485)._subcl==OEParse_scMove)  {
            FOut_String(6, (CHAR *) "MOVE(");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 58555)._right, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58616)._right, 58623)._link, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58683)._right, 58690)._link, 58696)._link, FALSE);
            FOut_Char(')');
          } else if (DEREF(OEParse_Node, _s, 58751)._subcl==OEParse_scNewFix)  {
            if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58791)._left, 58797)._type, 58803)._base, 58809)._form==OTable_strRecord&&!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58867)._left, 58873)._type, 58879)._base, 58885)._flags, 58863))  {
              FOut_String(8, (CHAR *) "NEWREC(");
              OGenGCC_Expr(DEREF(OEParse_Node, _s, 58986)._left, FALSE);
              FOut_String(3, (CHAR *) ", ");
              OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59060)._left, 59066)._type, 59072)._base, TRUE);
            } else {
              FOut_String(8, (CHAR *) "NEWFIX(");
              OGenGCC_Expr(DEREF(OEParse_Node, _s, 59215)._left, FALSE);
            }
            OGenGCC_AppendAtomicFlag(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59274)._left, 59280)._type, 59286)._base);
          } else if (DEREF(OEParse_Node, _s, 59310)._subcl==OEParse_scNewDyn)  {
            FOut_String(7, (CHAR *) "NEWDYN");
            if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59399)._left, 59405)._type, 59411)._base, 59417)._flags, 59395))  {
              FOut_String(5, (CHAR *) "_EXT");
            }
            FOut_Char('(');
            OGenGCC_Designator(DEREF(OEParse_Node, _s, 59561)._left, FALSE);
            FOut_String(4, (CHAR *) ", 1");
            _dim = 0;
            _n = DEREF(OEParse_Node, _s, 59641)._right;
            while ((void*)_n!=(void*)NULL)  {
              FOut_String(2, (CHAR *) "*");
              OGenGCC_Expr(_n, FALSE);
              INCI(_dim, 1, 59748);
              _n = DEREF(OEParse_Node, _n, 59777)._link;
            }
            FOut_String(3, (CHAR *) ", ");
            FOut_Int((LONGINT) _dim, 0LL);
            OGenGCC_AppendAtomicFlag(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59881)._left, 59887)._type, 59893)._base);
            if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59936)._left, 59942)._type, 59948)._base, 59954)._flags, 59932))  {
              _dim = 0;
              _n = DEREF(OEParse_Node, _s, 60062)._right;
              while ((void*)_n!=(void*)NULL)  {
                FOut_Char(';');
                OGenGCC_Off(_off);
                FOut_String(9, (CHAR *) "SET_LEN(");
                FOut_Int((LONGINT) _dim, 0LL);
                FOut_String(3, (CHAR *) ", ");
                OGenGCC_Expr(_n, FALSE);
                FOut_Char(')');
                INCI(_dim, 1, 60326);
                _n = DEREF(OEParse_Node, _n, 60357)._link;
              }
            }
          } else if (DEREF(OEParse_Node, _s, 60410)._subcl==OEParse_scDispose)  {
            FOut_String(8, (CHAR *) "DISPOSE");
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60483)._left, 60489)._type, 60495)._form==OTable_strSysPtr||IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60553)._left, 60559)._type, 60565)._base, 60571)._flags, 60549))  {
            } else if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60671)._left, 60677)._type, 60683)._base, 60689)._form==OTable_strRecord)  {
              FOut_String(5, (CHAR *) "_REC");
            } else if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60764)._left, 60770)._type, 60776)._base, 60782)._form==OTable_strDynArray)  {
              FOut_String(5, (CHAR *) "_DYN");
            }
            FOut_Char('(');
            OGenGCC_Designator(DEREF(OEParse_Node, _s, 60903)._left, FALSE);
            if ((DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60934)._left, 60940)._type, 60946)._form==OTable_strPointer&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60987)._left, 60993)._type, 60999)._base, 61005)._form==OTable_strDynArray)&&!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61066)._left, 61072)._type, 61078)._base, 61084)._flags, 61062))  {
              FOut_String(3, (CHAR *) ", ");
              _dim = 0;
              _t = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61167)._left, 61173)._type, 61179)._base;
              while (DEREF(OTable_Struct, _t, 61207)._form==OTable_strDynArray)  {
                INCI(_dim, 1, 61248);
                _t = DEREF(OTable_Struct, _t, 61279)._base;
              }
              FOut_Int((LONGINT) _dim, 0LL);
            }
            FOut_Char(')');
          } else if (DEREF(OEParse_Node, _s, 61384)._subcl==OEParse_scCollect)  {
            FOut_String(11, (CHAR *) "GC_COLLECT");
          } else if (DEREF(OEParse_Node, _s, 61460)._subcl==OEParse_scCopy)  {
            FOut_String(6, (CHAR *) "COPY(");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 61544)._left, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 61625)._right, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_ArrayLength(DEREF(OEParse_Node, _s, 61713)._right, 0LL);
            FOut_Char(')');
          } else if (DEREF(OEParse_Node, _s, 61789)._subcl==OEParse_scAssign&&IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61815)._left, 61821)._type, 61827)._form, 0x0000000000084000ULL, 61834))  {
            _ExtendedAssignment(DEREF(OEParse_Node, _s, 61902)._left, DEREF(OEParse_Node, _s, 61911)._right);
          } else if (DEREF(OEParse_Node, _s, 61936)._subcl==OEParse_scGet||DEREF(OEParse_Node, _s, 61960)._subcl==OEParse_scPut)  {
            if (DEREF(OEParse_Node, _s, 61999)._subcl==OEParse_scGet)  {
              FOut_String(5, (CHAR *) "GET(");
            } else {
              FOut_String(5, (CHAR *) "PUT(");
            }
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 62130)._left, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 62190)._right, FALSE);
            FOut_String(3, (CHAR *) ", ");
            if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62249)._right, 62256)._type, 62262)._form, 0x000000000000C000ULL, 62269))  {
              FOut_String(6, (CHAR *) "void*");
            } else {
              OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62370)._right, 62377)._type, 62383)._obj);
            }
            FOut_Char(')');
          } else {
            _flags = 0x0000000000000000ULL;
            {
              SHORTINT _temp_ = DEREF(OEParse_Node, _s, 62480)._subcl;
              switch (_temp_) {
                case 0: {
                  if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62534)._right, 62541)._type, 62547)._form==OTable_strString)  {
                    COPYSTRING(&(_str), "COPYSTRING");
                    _flags = 0x0000000000000010ULL;
                  } else if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62653)._left, 62659)._type, 62665)._form, OGenGCC_arraySet, 62672))  {
                    if ((void*)DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62711)._left, 62717)._type, 62723)._obj!=(void*)NULL)  {
                      COPYSTRING(&(_str), "COPYARRAYT");
                      _flags = 0x0000000000000018ULL;
                    } else {
                      COPYSTRING(&(_str), "COPYARRAY");
                      _flags = 0x0000000000000030ULL;
                    }
                  } else {
                    COPYSTRING(&(_str), " = ");
                  }
                  break;
                }
                case 34: {
                  COPYSTRING(&(_str), "INC");
                  _flags = 0x0000000000000003ULL;
                  break;
                }
                case 35: {
                  COPYSTRING(&(_str), "DEC");
                  _flags = 0x0000000000000003ULL;
                  break;
                }
                case 32: {
                  COPYSTRING(&(_str), "INCL");
                  _flags = 0x0000000000000002ULL;
                  break;
                }
                case 33: {
                  COPYSTRING(&(_str), "EXCL");
                  _flags = 0x0000000000000002ULL;
                  break;
                }
                case 42: {
                  COPYSTRING(&(_str), "NEWSYS");
                  _flags = 0x0000000000000000ULL;
                  break;
                }
                default: NO_LABEL (_temp_, 62474);
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
          _n = DEREF(OEParse_Node, _s, 63506)._left;
          do  {
            if (_n==DEREF(OEParse_Node, _s, 63546)._left)  {
              FOut_String(5, (CHAR *) "if (");
            } else {
              FOut_String(11, (CHAR *) " else if (");
            }
            OGenGCC_Expr(DEREF(OEParse_Node, _n, 63672)._left, FALSE);
            FOut_String(3, (CHAR *) ") ");
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _n, 63740)._right, _off+1, TRUE);
            _n = DEREF(OEParse_Node, _n, 63779)._link;
          } while (!((void*)_n==(void*)NULL));
          if ((void*)DEREF(OEParse_Node, _s, 63822)._right!=(void*)NULL)  {
            FOut_String(6, (CHAR *) " else");
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 63895)._right, _off+1, TRUE);
          }
          break;
        }
        case 20: {
          FOut_Char('{');
          OGenGCC_Off(_off+1);
          OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64006)._left, 64012)._type, 64018)._obj);
          FOut_String(11, (CHAR *) " _temp_ = ");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 64074)._left, FALSE);
          FOut_Char(';');
          OGenGCC_Off(_off+1);
          FOut_String(18, (CHAR *) "switch (_temp_) {");
          _n = DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64187)._right, 64194)._left;
          while ((void*)_n!=(void*)NULL)  {
            _m = DEREF(OEParse_Node, _n, 64245)._link;
            do  {
              OGenGCC_Off(_off+2);
              FOut_String(6, (CHAR *) "case ");
              _CaseLabel(_m, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64354)._left, 64360)._type, 64366)._form);
              _m = DEREF(OEParse_Node, _m, 64393)._link;
              FOut_Char(':');
            } while (!((void*)_m==(void*)NULL));
            FOut_String(3, (CHAR *) " {");
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _n, 64505)._right, _off+3, FALSE);
            OGenGCC_Off(_off+3);
            FOut_String(7, (CHAR *) "break;");
            OGenGCC_Off(_off+2);
            FOut_Char('}');
            _n = DEREF(OEParse_Node, _n, 64646)._left;
          }
          OGenGCC_Off(_off+2);
          FOut_String(10, (CHAR *) "default: ");
          if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64732)._right, 64739)._conval, 64747)._set!=0x0000000000000000ULL)  {
            OGenGCC_StatementSeq(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64787)._right, 64794)._right, _off+2, TRUE);
          } else {
            FOut_String(19, (CHAR *) "NO_LABEL (_temp_, ");
            FOut_Int(DEREF(OEParse_Node, _s, 64890)._pos, 0LL);
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
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 65087)._left, FALSE);
          FOut_String(3, (CHAR *) ") ");
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 65151)._right, _off+1, TRUE);
          break;
        }
        case 24: {
          FOut_String(4, (CHAR *) "do ");
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 65242)._left, _off+1, TRUE);
          FOut_String(11, (CHAR *) " while (!(");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 65312)._right, FALSE);
          FOut_String(4, (CHAR *) "));");
          break;
        }
        case 25: {
          FOut_Char('{');
          OGenGCC_Off(_off+1);
          OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65430)._left, 65436)._link, 65442)._type, 65448)._obj);
          FOut_String(8, (CHAR *) " _temp_");
          FOut_String(4, (CHAR *) " = ");
          OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65527)._left, 65533)._right, FALSE);
          FOut_Char(';');
          OGenGCC_Off(_off+1);
          FOut_String(5, (CHAR *) "for(");
          OGenGCC_Designator(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65641)._left, 65647)._link, FALSE);
          FOut_String(4, (CHAR *) " = ");
          OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65704)._left, 65710)._left, FALSE);
          FOut_String(4, (CHAR *) " ; ");
          OGenGCC_Designator(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65773)._left, 65779)._link, FALSE);
          if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65808)._left, 65814)._conval, 65822)._intval>0LL)  {
            FOut_String(5, (CHAR *) " <= ");
          } else {
            FOut_String(5, (CHAR *) " >= ");
          }
          FOut_String(10, (CHAR *) "_temp_ ; ");
          OGenGCC_Designator(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65976)._left, 65982)._link, FALSE);
          FOut_String(5, (CHAR *) " += ");
          FOut_Int(DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66041)._left, 66047)._conval, 66055)._intval, 0LL);
          FOut_Char(')');
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 66114)._right, _off+2, TRUE);
          OGenGCC_Off(_off);
          FOut_Char('}');
          break;
        }
        case 28: {
          FOut_String(9, (CHAR *) "while(1)");
          DEREF(OEParse_Node, _s, 66234)._pos = -DEREF(OEParse_Node, _s, 66245)._pos;
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 66275)._left, _off+1, TRUE);
          if (DEREF(OEParse_Node, _s, 66310)._pos>=0LL)  {
            OGenGCC_Off(_off);
            FOut_String(6, (CHAR *) "_exit");
            FOut_Int(DEREF(OEParse_Node, _s, 66444)._pos, 0LL);
            FOut_String(4, (CHAR *) ": ;");
          }
          break;
        }
        case 29: {
          _n = DEREF(OEParse_Node, _s, 66530)._left;
          while ((void*)_n!=(void*)NULL)  {
            if (_n!=DEREF(OEParse_Node, _s, 66584)._left)  {
              FOut_String(6, (CHAR *) "else ");
            }
            FOut_String(5, (CHAR *) "if (");
            OGenGCC_GenTypeTest(DEREF(OEParse_Node, _n, 66696)._left, DEREF(OTable_Object, DEREF(OEParse_Node, _n, 66705)._obj, 66710)._type);
            FOut_Char(')');
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _n, 66768)._right, _off+1, TRUE);
            _n = DEREF(OEParse_Node, _n, 66807)._link;
          }
          OGenGCC_Off(_off);
          FOut_String(5, (CHAR *) "else");
          if (DEREF(OTable_Const, DEREF(OEParse_Node, _s, 66886)._conval, 66894)._set==0x0000000000000000ULL)  {
            FOut_String(13, (CHAR *) " { NO_GUARD(");
            FOut_Int(DEREF(OEParse_Node, _s, 66966)._pos, 0LL);
            FOut_String(5, (CHAR *) "); }");
          } else {
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 67043)._right, _off+1, TRUE);
          }
          break;
        }
        case 31: {
          DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 67104)._left, 67110)._pos = ABSLI(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 67125)._left, 67131)._pos, 67119);
          FOut_String(11, (CHAR *) "goto _exit");
          FOut_Int(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 67222)._left, 67228)._pos, 0LL);
          FOut_Char(';');
          break;
        }
        case 32: {
          FOut_String(7, (CHAR *) "return");
          if ((void*)DEREF(OEParse_Node, _s, 67322)._left!=(void*)NULL)  {
            if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 67356)._left, 67362)._type, 67368)._form, 0x0000000000006000ULL, 67375)&&DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 67424)._left, 67430)._type!=DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _s, 67440)._obj, 67445)._type, 67451)._base)  {
              FOut_String(8, (CHAR *) "(void*)");
            }
            FOut_Char(' ');
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 67553)._left, FALSE);
          }
          FOut_Char(';');
          break;
        }
        case 33: {
          FOut_String(5, (CHAR *) "HALT");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 67662)._left, TRUE);
          FOut_Char(';');
          break;
        }
        case 36: {
          FOut_String(8, (CHAR *) "ASSERT(");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 67763)._right, FALSE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 67820)._left, FALSE);
          FOut_String(3, (CHAR *) ", ");
          FOut_Int(DEREF(OEParse_Node, _s, 67877)._pos, 0LL);
          FOut_String(3, (CHAR *) ");");
          break;
        }
        default:  {
        HALT(102);
      }
      }
    }
    _s = DEREF(OEParse_Node, _s, 67965)._link;
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
    if ((void*)DEREF(OEParse_Node, _enter, 68319)._obj!=(void*)NULL)  {
      _obj = DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 68422)._obj, 68427)._type, 68433)._link;
      while ((void*)_obj!=(void*)NULL)  {
        if (DEREF(OTable_Object, _obj, 68487)._mode==OTable_objVar&&IN(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 68512)._type, 68518)._form, OGenGCC_arraySet, 68525))  {
          OGenGCC_Off(_off+1);
          OGenGCC_OpenArrayDecl(_obj, FALSE);
          FOut_Char(';');
        }
        _obj = DEREF(OTable_Object, _obj, 68668)._link;
      }
    }
    _obj = DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 68753)._obj, 68758)._link, 68764)._right;
    OGenGCC_LocalDecl(_obj, _enter, _off+1);
    if ((void*)DEREF(OEParse_Node, _enter, 68876)._obj!=(void*)NULL)  {
      _obj = DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 68988)._obj, 68993)._type, 68999)._link;
      while ((void*)_obj!=(void*)NULL)  {
        if (DEREF(OTable_Object, _obj, 69053)._mode==OTable_objVar&&IN(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 69078)._type, 69084)._form, OGenGCC_arraySet, 69091))  {
          OGenGCC_Off(_off+1);
          if (DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 69153)._type, 69159)._form==OTable_strArray)  {
            FOut_String(14, (CHAR *) "VALUE_ARRAYF(");
          } else {
            FOut_String(13, (CHAR *) "VALUE_ARRAY(");
          }
          OGenGCC_Ident(_obj);
          FOut_String(4, (CHAR *) ", _");
          OGenGCC_Ident(_obj);
          FOut_String(6, (CHAR *) "_p, 1");
          if (DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 69431)._type, 69437)._form==OTable_strDynArray)  {
            {
              INTEGER _temp_ = OGenGCC_DynDimensions(DEREF(OTable_Object, _obj, 69512)._type)- 1;
              for(_i = 0 ; _i <= _temp_ ; _i += 1) {
                FOut_Char('*');
                FOut_Char('_');
                OGenGCC_Ident(_obj);
                FOut_Char('_');
                FOut_Int((LONGINT) _i, 0LL);
              }
            }
          }
          FOut_String(3, (CHAR *) ");");
        }
        _obj = DEREF(OTable_Object, _obj, 69771)._link;
      }
    }
    OGenGCC_StatementSeq(DEREF(OEParse_Node, _enter, 69854)._right, _off+1, FALSE);
    if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 69893)._obj, 69898)._type, 69904)._base, 69910)._form!=OTable_strNone)  {
      OGenGCC_Off(_off+1);
      FOut_String(12, (CHAR *) "NO_RETURN (");
      FOut_Int(DEREF(OEParse_Node, _enter, 70010)._pos, 0LL);
      FOut_String(3, (CHAR *) ");");
    }
  }
  while ((void*)_obj!=(void*)NULL&&DEREF(OTable_Object, _obj, 70154)._mode<OTable_objExtProc)  {
    _obj = OGenGCC_Decl(_obj, 0x0000000000000002ULL, _off);
  }
  _local = DEREF(OEParse_Node, _enter, 70351)._left;
  while ((void*)_local!=(void*)NULL)  {
    FOut_Ln();
    if (DEREF(OEParse_Node, _local, 70413)._class==OEParse_ndForward&&(void*)DEREF(OEParse_Node, _enter, 70444)._obj!=(void*)NULL)  {
      _next = OGenGCC_Decl(DEREF(OEParse_Node, _local, 70543)._obj, 0x0000000000000020ULL, _off);
    } else {
      _next = OGenGCC_Decl(DEREF(OEParse_Node, _local, 70611)._obj, 0x0000000000000000ULL, _off);
    }
    if (DEREF(OEParse_Node, _local, 70653)._class==OEParse_ndForward)  {
      FOut_String(2, (CHAR *) ";");
    } else {
      FOut_String(3, (CHAR *) " {");
      _ProcedureBlock(_local, _off);
      OGenGCC_Off(_off);
      FOut_Char('}');
    }
    _local = DEREF(OEParse_Node, _local, 70880)._link;
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
    OGenGCC_WriteInclude(256, (CHAR *) INDEX(DEREF(ODepend_Module, _module, 72085)._file, OGenGCC_header, 5, 72091));
  }
  _obj = DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 72172)._link, 72178)._next;
  while ((void*)_obj!=(void*)NULL)  {
    if (DEREF(OTable_Object, DEREF(OTable_Object, _obj, 72225)._link, 72231)._mnolev<=-3LL)  {
      if (OTable_external||IN(OTable_flagExport, DEREF(OTable_Object, _obj, 72310)._flags, 72304)==_writeHeader)  {
        OTable_GetModuleName(_obj, 256, (CHAR *) _name);
        _import = DEREF(ODepend_Module, _module, 72404)._import;
        while ((void*)_import!=(void*)NULL&&STRNEQ(DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 72454)._module, 72462)._name, _name))  {
          _import = DEREF(ODepend_Import, _import, 72508)._next;
        }
        if ((void*)_import!=(void*)NULL)  {
          OGenGCC_WriteInclude(256, (CHAR *) INDEX(DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 72595)._module, 72603)._file, OGenGCC_header, 5, 72609));
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
    _obj = DEREF(OTable_Object, _obj, 72947)._next;
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
  FOut_String(14, (CHAR *) "#ifdef MODULE");
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
  OGenGCC_GenTypeDescrForward(DEREF(OEParse_Node, OGenGCC_root, 73953)._link, TRUE);
  _obj = DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 74051)._link, 74057)._right;
  while ((void*)_obj!=(void*)NULL)  {
    _obj = OGenGCC_Decl(_obj, 0x0000000000000003ULL, 0);
  }
  FOut_Ln();
  OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 74188)._link, TRUE);
  FOut_Ln();
  OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 74274)._link, TRUE);
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
  FOut_String(15, (CHAR *) "#define MODULE");
  OGenGCC_Ident(OTable_compiledModule);
  FOut_Ln();
  FOut_Ln();
  OGenGCC_Include(_mod, FALSE);
  FOut_Ln();
  FOut_String(26, (CHAR *) "static ModuleId moduleId;");
  FOut_Ln();
  OGenGCC_GenTypeDescrForward(DEREF(OEParse_Node, OGenGCC_root, 74999)._link, FALSE);
  OGenGCC_LocalDecl(DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 75046)._link, 75052)._right, OGenGCC_root, 0);
  OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 75097)._link, FALSE);
  FOut_Ln();
  FOut_Ln();
  FOut_String(11, (CHAR *) "void _init");
  OGenGCC_Ident(OTable_compiledModule);
  FOut_String(10, (CHAR *) " (void) {");
  FOut_Ln();
  FOut_String(27, (CHAR *) "  moduleId = add_module (\"");
  FOut_String(48, (CHAR *) DEREF(OTable_Object, OTable_compiledModule, 75309)._name);
  FOut_String(4, (CHAR *) "\");");
  if (!OTable_external)  {
    OGenGCC_GenTypeDescrInit(DEREF(OEParse_Node, OGenGCC_root, 75448)._link);
  }
  OGenGCC_StatementSeq(DEREF(OEParse_Node, OGenGCC_root, 75487)._right, 1, FALSE);
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
  if (OTable_external&&STRNEQ(DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, OTable_compiledModule, 76307)._const, 76314)._string, 76322), "C"))  {
    OScan_WarnIns(DEREF(OTable_Object, OTable_compiledModule, 76369)._pos, 297, 256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, OTable_compiledModule, 76397)._const, 76404)._string, 76412));
  }
  OGenGCC_CheckName(OTable_compiledModule, &(DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 76519)._link, 76525)._right));
  if (IN(ODepend_flSymChanged, DEREF(ODepend_Module, _mod, 76566)._flags, 76560)||!IN(OGenGCC_header, DEREF(ODepend_Module, _mod, 76593)._flags, 76587))  {
    ODepend_NewFile(_mod, OGenGCC_header, 2, (CHAR *) "h");
    FOut_Open(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 76726)._file, OGenGCC_header, 5, 76732));
    OGenGCC_GenHeader(_mod);
    FOut_Close();
    INCL(DEREF(ODepend_Module, _mod, 76796)._flags, OGenGCC_header, 76787);
  }
  if (OTable_external)  {
    _UpdateMarker(98, (CHAR *) DEREF(ODepend_Module, _mod, 76868)._name);
  } else {
    ODepend_NewFile(_mod, OGenGCC_cFile, 2, (CHAR *) "c");
    INCL(DEREF(ODepend_Module, _mod, 76938)._flags, OGenGCC_cFile, 76929);
    FOut_Open(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 76999)._file, OGenGCC_cFile, 5, 77005));
    OGenGCC_GenModule(_mod);
    FOut_Ln();
    FOut_Close();
  }
}

static LONGINT OGenGCC_StructSize (OTable_Struct _t) {
  {
    SHORTINT _temp_ = DEREF(OTable_Struct, _t, 77321)._form;
    switch (_temp_) {
      case 1:
      case 2:
      case 12: {
        return 1LL;
        break;
      }
      case 3: {
        return 2LL;
        break;
      }
      case 4:
      case 6:
      case 8: {
        return 4LL;
        break;
      }
      case 5:
      case 7:
      case 13:
      case 15:
      case 14: {
        return 8LL;
        break;
      }
      case 17:
      case 19: {
        return -1LL;
        break;
      }
      default:  {
      return -2LL;
    }
    }
  }
  NO_RETURN (77099);
}

void _init_OGenGCC (void) {
  moduleId = add_module ("OGenGCC");
  OGenGCC_atCount = 0LL;
  OGenGCC_emptyObj = OTable_NewObject(2, (CHAR *) "@", OTable_objType, -1LL);
  OTable_structSize = OGenGCC_StructSize;
}
