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
static const SET OGenGCC_arraySet = 0x00060000UL;
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
  if (IN(OTable_flagExternal, DEREF(OTable_Object, _obj, 2558)._flags, 2552)&&IN(DEREF(OTable_Object, _obj, 2581)._mode, 0x00000290UL, 2588))  {
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
        FOut_Int((LONGINT) OGenGCC_TypeDescId(_o), 0);
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
                FOut_Int((LONGINT) _i, 0);
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
          _first = OGenGCC_NamedDecl(_first, 0x00000000UL, _off);
        } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _first, 17471)._type, 17477)._form==OTable_strNone)  {
          FOut_String(4, (CHAR *) "...");
        } else {
          _first = OGenGCC_NamedDecl(_first, 0x00000000UL, _off);
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
    return ((void*)DEREF(OTable_Struct, _t, 18137)._obj==(void*)NULL||((DEREF(OTable_Object, DEREF(OTable_Struct, _t, 18295)._obj, 18300)._mnolev>=0&&DEREF(OTable_Struct, _t, 18333)._form!=OTable_strRecord)&&!(IN(OGenGCC_ofDefined, DEREF(OTable_Object, DEREF(OTable_Struct, _t, 18390)._obj, 18395)._flags, 18386)||(IN(OTable_flagExport, DEREF(OTable_Object, DEREF(OTable_Struct, _t, 18443)._obj, 18448)._flags, 18439)&&!IN(OGenGCC_flagHeader, _flags, 18472)))))||(IN(OGenGCC_flagTypeDef, _flags, 18584)&&DEREF(OTable_Struct, _t, 18598)._obj==_o);
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
              _declFlags = 0x00000200UL;
            } else {
              _declFlags = 0x00000000UL;
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
            FOut_Int(DEREF(OTable_Struct, _t, 21986)._len, 0);
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
      if (_ch>=127)  {
        FOut_String(7, (CHAR *) "(CHAR)");
      }
      FOut_Char('\047');
    }
    if ((((_ch<32||_ch>=127)||_ch==34)||_ch==39)||_ch==92)  {
      FOut_Char('\134');
      FOut_Char((CHAR) (DIVLI(_ch, 64, 24457)+48));
      FOut_Char((CHAR) (MODLI(DIVLI(_ch, 8, 24502), 8, 24509)+48));
      FOut_Char((CHAR) (MODLI(_ch, 8, 24552)+48));
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
      for(_i = 28 ; _i >= _temp_ ; _i += -4) {
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
    FOut_String(3, (CHAR *) "UL");
  }
  if (((void*)_obj!=(void*)NULL&&!IN(OTable_flagExternal, DEREF(OTable_Object, _obj, 26349)._flags, 26343))&&!(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 26373)._type, 26379)._form==OTable_strString&&Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, _obj, 26429)._const, 26436)._string, 26444))==1))  {
    OGenGCC_Ident(_obj);
  } else {
    {
      SHORTINT _temp_ = _form;
      switch (_temp_) {
        case 1: {
          if (DEREF(OTable_Const, _c, 26556)._intval==1)  {
            FOut_String(5, (CHAR *) "TRUE");
          } else {
            FOut_String(6, (CHAR *) "FALSE");
          }
          break;
        }
        case 2: {
          _CharConst(DEREF(OTable_Const, _c, 26694)._intval, TRUE);
          break;
        }
        case 3 ... 5: {
          if (DEREF(OTable_Const, _c, 26760)._intval==(-2147483647L-1))  {
            FOut_String(3, (CHAR *) "(-");
            FOut_Int(2147483647, 0);
            FOut_String(5, (CHAR *) "L-1)");
          } else {
            FOut_Int(DEREF(OTable_Const, _c, 27040)._intval, 0);
          }
          break;
        }
        case 6:
        case 7: {
          _RealConst(DEREF(OTable_Const, _c, 27119)._real, _form==OTable_strLongReal);
          break;
        }
        case 8: {
          _SetConst(DEREF(OTable_Const, _c, 27184)._set);
          break;
        }
        case 10: {
          FOut_Char('\042');
          if (Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, _c, 27259)._string, 27267))==1)  {
            _CharConst((LONGINT) ((INTEGER) INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27307)._string, 27315), 0, 256, 27315)), FALSE);
          } else {
            _i = 0;
            while (INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27377)._string, 27385), _i, 256, 27385)!='\000')  {
              if ((INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27415)._string, 27423), _i, 256, 27423)=='\042'||INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27491)._string, 27499), _i, 256, 27499)=='\134')||INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27589)._string, 27597), _i, 256, 27597)=='?')  {
                FOut_Char('\134');
              }
              FOut_Char(INDEX(DEREF(OTable_String, DEREF(OTable_Const, _c, 27741)._string, 27749), _i, 256, 27749));
              INCI(_i, 1, 27767);
            }
          }
          FOut_Char('\042');
          break;
        }
        case 11: {
          FOut_String(5, (CHAR *) "NULL");
          break;
        }
        default: NO_LABEL (_temp_, 26513);
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
  if (OTable_external&&DEREF(OTable_Object, _obj, 28784)._mode==OTable_objConst)  {
    _modifier = _dontPrint;
  } else if (IN(OGenGCC_flagAutoPrefix, _flags, 28864))  {
    _modifier = _auto;
  } else if (DEREF(OTable_Object, _obj, 28920)._mnolev>0)  {
    _modifier = _noModifier;
  } else {
    if (IN(DEREF(OTable_Object, _obj, 29004)._mode, 0x00000B94UL, 29011))  {
      if (!IN(OTable_flagExport, DEREF(OTable_Object, _obj, 29185)._flags, 29179))  {
        if (IN(OGenGCC_flagHeader, _flags, 29224))  {
          _modifier = _dontPrint;
        } else {
          _modifier = _static;
        }
      } else {
        if (IN(OGenGCC_flagHeader, _flags, 29371))  {
          _modifier = _extern;
        } else {
          _modifier = _noModifier;
        }
      }
    } else {
      if (IN(OTable_flagExport, DEREF(OTable_Object, _obj, 29560)._flags, 29554)!=IN(OGenGCC_flagHeader, _flags, 29583))  {
        _modifier = _dontPrint;
      } else {
        _modifier = _noModifier;
      }
    }
  }
  if (_modifier!=_dontPrint)  {
    if (IN(DEREF(OTable_Object, _obj, 29756)._mode, 0x00000B80UL, 29763)&&!OTable_external)  {
      OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 29962)._link, IN(OGenGCC_flagHeader, _flags, 29981));
    }
    OGenGCC_Off(_off);
    if (_modifier==_extern)  {
      FOut_String(8, (CHAR *) "extern ");
    } else if (_modifier==_static)  {
      FOut_String(8, (CHAR *) "static ");
    } else if (_modifier==_auto)  {
      FOut_String(6, (CHAR *) "auto ");
    }
    if (DEREF(OTable_Object, _obj, 30235)._mode==OTable_objConst)  {
      FOut_String(7, (CHAR *) "const ");
      _next = OGenGCC_NamedDecl(_obj, 0x00000000UL, _off);
      if (!IN(OGenGCC_flagHeader, _flags, 30385))  {
        FOut_String(4, (CHAR *) " = ");
        OGenGCC_Const(DEREF(OTable_Object, _obj, 30493)._const, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 30505)._type, 30511)._form, (OTable_Object)NULL);
      }
    } else if (DEREF(OTable_Object, _obj, 30552)._mode==OTable_objType)  {
      FOut_String(9, (CHAR *) "typedef ");
      _next = OGenGCC_NamedDecl(_obj, _flags|0x00000004UL, _off);
      INCL(DEREF(OTable_Object, _obj, 30707)._flags, OGenGCC_ofDefined, 30698);
    } else if (DEREF(OTable_Object, _obj, 30743)._mode==OTable_objVar)  {
      _next = OGenGCC_NamedDecl(_obj, _flags|0x00000008UL, _off);
    } else {
      _next = OGenGCC_NamedDecl(_obj, _flags|0x00000040UL, _off);
    }
    if (IN(OGenGCC_flagSemicolon, _flags, 31018))  {
      FOut_Char(';');
    }
    return _next;
  } else {
    return DEREF(OTable_Object, _obj, 31108)._next;
  }
  NO_RETURN (27909);
}

static void OGenGCC_VarDecl (LONGINT __name_0, const CHAR (* __name_p), OTable_Struct _type) {
  CHAR (* _name);
  OTable_Object _dummy;
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  if ((void*)DEREF(OTable_Struct, _type, 31243)._obj!=(void*)NULL)  {
    OGenGCC_Ident(DEREF(OTable_Struct, _type, 31278)._obj);
    FOut_Char(' ');
    FOut_String(__name_0, _name);
  } else {
    DEREF(OTable_Object, OGenGCC_emptyObj, 31351)._type = _type;
    COPY(_name, OGenGCC_extName, 104);
    _dummy = OGenGCC_NamedDecl(OGenGCC_emptyObj, 0x00000100UL, -32768);
  }
}

static void OGenGCC_Expr (OEParse_Node _expr, BOOLEAN _paren);

static void OGenGCC_Designator (OEParse_Node _d, BOOLEAN _adr);

static void OGenGCC_ArrayLength (OEParse_Node _desig, LONGINT _dim) {
  INTEGER _ddim;
  OTable_Struct _type;
  _ddim = OGenGCC_DynDimensions(DEREF(OEParse_Node, _desig, 31793)._type);
  if ((LONGINT) _ddim<=_dim)  {
    _type = DEREF(OEParse_Node, _desig, 31900)._type;
    while (_dim!=0)  {
      _type = DEREF(OTable_Struct, _type, 31953)._base;
      DECLI(_dim, 1, 31969);
    }
    FOut_Int(DEREF(OTable_Struct, _type, 32007)._len, 0);
  } else if (DEREF(OEParse_Node, _desig, 32033)._class==OEParse_ndVarPar||DEREF(OEParse_Node, _desig, 32064)._class==OEParse_ndVar)  {
    FOut_String(2, (CHAR *) "_");
    OGenGCC_Ident(DEREF(OEParse_Node, _desig, 32176)._obj);
    FOut_Char('_');
    FOut_Int(_dim, 0);
  } else {
    FOut_String(9, (CHAR *) "GET_LEN(");
    while (DEREF(OTable_Struct, DEREF(OEParse_Node, _desig, 32326)._type, 32332)._form==OTable_strDynArray)  {
      _desig = DEREF(OEParse_Node, _desig, 32381)._left;
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
  while (DEREF(OEParse_Node, (* _prefix), 32979)._class==OEParse_ndIndex)  {
    INCI((* _dim), 1, 33009);
    (* _prefix) = DEREF(OEParse_Node, (* _prefix), 33042)._left;
  }
}

static void OGenGCC_GenTypeTag (OEParse_Node _n) {
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 33177)._type, 33183)._form==OTable_strPointer)  {
    FOut_String(5, (CHAR *) "TAG(");
    OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 33274)._type, 33280)._base, FALSE);
    FOut_String(3, (CHAR *) ", ");
    OGenGCC_Designator(_n, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, _n, 33385)._pos, 0);
    FOut_Char(')');
  } else if (DEREF(OEParse_Node, _n, 33427)._class==OEParse_ndVarPar)  {
    OGenGCC_GenTagName(DEREF(OEParse_Node, _n, 33506)._obj);
  } else if (DEREF(OEParse_Node, _n, 33525)._class==OEParse_ndDeref)  {
    FOut_String(5, (CHAR *) "TAG(");
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _n, 33642)._type, FALSE);
    FOut_String(3, (CHAR *) ", ");
    OGenGCC_Designator(DEREF(OEParse_Node, _n, 33700)._left, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, _n, 33753)._pos, 0);
    FOut_Char(')');
  } else {
    OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _n, 33836)._type, TRUE);
  }
}

static void OGenGCC_GenTypeLevel (OTable_Struct _type) {
  INTEGER _level;
  FOut_String(3, (CHAR *) ", ");
  if (DEREF(OTable_Struct, _type, 34033)._form==OTable_strPointer)  {
    _type = DEREF(OTable_Struct, _type, 34079)._base;
  }
  _level = -1;
  do  {
    INCI(_level, 1, 34129);
    _type = DEREF(OTable_Struct, _type, 34160)._base;
  } while (!((void*)_type==(void*)NULL));
  FOut_Int((LONGINT) _level, 0);
}

static void OGenGCC_GenTypeTest (OEParse_Node _expr, OTable_Struct _type) {
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 34314)._type, 34320)._form==OTable_strPointer)  {
    FOut_String(15, (CHAR *) "type_test_ptr(");
    OGenGCC_Expr(_expr, FALSE);
  } else {
    FOut_String(11, (CHAR *) "type_test(");
    OGenGCC_GenTypeTag(_expr);
  }
  FOut_String(3, (CHAR *) ", ");
  OGenGCC_TypeDescrIdent(_type, TRUE);
  OGenGCC_GenTypeLevel(_type);
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 34572)._type, 34578)._form==OTable_strPointer)  {
    FOut_String(13, (CHAR *) ", moduleId, ");
    FOut_Int(DEREF(OEParse_Node, _expr, 34656)._pos, 0);
  }
  FOut_Char(')');
}

static void OGenGCC_TypeDesign (OTable_Struct _type, BOOLEAN _arrayToPtr) {
  OTable_Object _dummy;
  BOOLEAN _toPointer;
  _toPointer = _arrayToPtr&&IN(DEREF(OTable_Struct, _type, 34871)._form, OGenGCC_arraySet, 34878);
  while (IN(DEREF(OTable_Struct, _type, 34907)._form, OGenGCC_arraySet, 34914))  {
    _type = DEREF(OTable_Struct, _type, 34948)._base;
  }
  if (_toPointer)  {
    FOut_Char('(');
  }
  if ((void*)DEREF(OTable_Struct, _type, 35026)._obj!=(void*)NULL)  {
    OGenGCC_Ident(DEREF(OTable_Struct, _type, 35061)._obj);
  } else {
    DEREF(OTable_Object, OGenGCC_emptyObj, 35091)._type = _type;
    _dummy = OGenGCC_NamedDecl(OGenGCC_emptyObj, 0x00000080UL, -32768);
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
    return (((_adr&&DEREF(OEParse_Node, _d, 35437)._class!=OEParse_ndDeref)&&DEREF(OEParse_Node, _d, 35462)._class!=OEParse_ndVarPar)&&DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 35502)._type, 35508)._form!=OTable_strDynArray)&&!(DEREF(OEParse_Node, _d, 35552)._class==OEParse_ndIndex&&DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 35577)._left, 35583)._type, 35589)._form==OTable_strDynArray);
    NO_RETURN (35383);
  }

  BOOLEAN _FindField (OTable_Struct _t, OTable_Object _obj) {
    OTable_Object _o;
    _o = DEREF(OTable_Struct, _t, 35748)._link;
    while ((void*)_o!=(void*)NULL)  {
      if (_o==_obj)  {
        return TRUE;
      }
      _o = DEREF(OTable_Object, _o, 35856)._next;
    }
    return FALSE;
    NO_RETURN (35646);
  }

  void _DesignRG (void) {
    if ((DEREF(OEParse_Node, _d, 35957)._class==OEParse_ndVar||DEREF(OEParse_Node, _d, 35981)._class==OEParse_ndVarPar)&&DEREF(OTable_Object, DEREF(OEParse_Node, _d, 36008)._obj, 36013)._type!=DEREF(OEParse_Node, _d, 36023)._type)  {
      FOut_String(3, (CHAR *) "((");
      OGenGCC_TypeDesign(DEREF(OEParse_Node, _d, 36205)._type, FALSE);
      if (DEREF(OEParse_Node, _d, 36234)._class==OEParse_ndVarPar)  {
        FOut_Char('*');
      }
      FOut_Char(')');
      OGenGCC_Ident(DEREF(OEParse_Node, _d, 36335)._obj);
      FOut_Char(')');
    } else {
      OGenGCC_Ident(DEREF(OEParse_Node, _d, 36391)._obj);
    }
  }
  if ((void*)_d!=(void*)NULL)  {
    if (_AdrPrefix())  {
      FOut_String(3, (CHAR *) "&(");
    }
    if (DEREF(OEParse_Node, _d, 36533)._class==OEParse_ndDeref)  {
      if (_adr||DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 36579)._type, 36585)._form==OTable_strDynArray)  {
        FOut_String(11, (CHAR *) "CHECK_NIL(");
      } else {
        FOut_String(7, (CHAR *) "DEREF(");
      }
      OGenGCC_TypeDesign(DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 36725)._left, 36731)._type, TRUE);
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Designator(DEREF(OEParse_Node, _d, 36792)._left, FALSE);
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _d, 36849)._pos, 0);
      FOut_Char(')');
    } else if (DEREF(OEParse_Node, _d, 36895)._class==OEParse_ndIndex)  {
      OGenGCC_GetArrayPrefix(_d, &(_prefix), &(_dim));
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 37013)._left, 37019)._type, 37025)._form==OTable_strArray)  {
        FOut_String(7, (CHAR *) "INDEX(");
        OGenGCC_Designator(DEREF(OEParse_Node, _d, 37130)._left, FALSE);
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_Expr(DEREF(OEParse_Node, _d, 37190)._right, FALSE);
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
        FOut_String(3, (CHAR *) ", ");
        FOut_Int(DEREF(OEParse_Node, _d, 37316)._pos, 0);
        FOut_Char(')');
      } else {
        if (!_adr)  {
          FOut_String(4, (CHAR *) "(* ");
        }
        if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 37514)._left, 37520)._type, 37526)._flags, 37510))  {
          FOut_String(15, (CHAR *) "PTR_INDEX_EXT(");
          OGenGCC_Designator(DEREF(OEParse_Node, _d, 37677)._left, TRUE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_Expr(DEREF(OEParse_Node, _d, 37740)._right, FALSE);
        } else {
          FOut_String(11, (CHAR *) "PTR_INDEX(");
          OGenGCC_Designator(DEREF(OEParse_Node, _d, 37887)._left, TRUE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_Expr(DEREF(OEParse_Node, _d, 37950)._right, FALSE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
          FOut_String(3, (CHAR *) ", ");
          FOut_Char('1');
          _type = DEREF(OEParse_Node, _d, 38111)._type;
          while (DEREF(OTable_Struct, _type, 38142)._form==OTable_strDynArray)  {
            INCI(_dim, 1, 38183);
            FOut_Char('*');
            OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
            _type = DEREF(OTable_Struct, _type, 38289)._base;
          }
          FOut_String(3, (CHAR *) ", ");
          FOut_Int(DEREF(OEParse_Node, _d, 38362)._pos, 0);
        }
        FOut_Char(')');
        if (!_adr)  {
          FOut_Char(')');
        }
      }
    } else if (DEREF(OEParse_Node, _d, 38499)._class==OEParse_ndGuard)  {
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 38538)._type, 38544)._form==OTable_strRecord)  {
        FOut_String(4, (CHAR *) "REC");
      } else {
        FOut_String(4, (CHAR *) "PTR");
      }
      FOut_String(13, (CHAR *) "_TYPE_GUARD(");
      OGenGCC_TypeDesign(DEREF(OEParse_Node, _d, 38707)._type, FALSE);
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Designator(DEREF(OEParse_Node, _d, 38769)._left, DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 38778)._type, 38784)._form==OTable_strRecord);
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 38820)._type, 38826)._form==OTable_strRecord)  {
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_GenTypeTag(DEREF(OEParse_Node, _d, 38903)._left);
      }
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _d, 38974)._type, TRUE);
      OGenGCC_GenTypeLevel(DEREF(OEParse_Node, _d, 39012)._type);
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _d, 39062)._pos, 0);
      FOut_Char(')');
    } else if (DEREF(OEParse_Node, _d, 39108)._class==OEParse_ndTBSuper)  {
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 39190)._left, 39196)._type, 39202)._form==OTable_strPointer)  {
        OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 39257)._left, 39263)._type, 39269)._base, 39275)._base, TRUE);
      } else {
        OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 39329)._left, 39335)._type, 39341)._base, TRUE);
      }
      FOut_String(6, (CHAR *) "->tb_");
      FOut_Int((LONGINT) OGenGCC_TypeDescId(DEREF(OEParse_Node, _d, 39424)._obj), 0);
    } else if (DEREF(OEParse_Node, _d, 39449)._class==OEParse_ndTBProc)  {
      OGenGCC_GenTypeTag(DEREF(OEParse_Node, _d, 39538)._left);
      FOut_String(6, (CHAR *) "->tb_");
      FOut_Int((LONGINT) OGenGCC_TypeDescId(DEREF(OEParse_Node, _d, 39603)._obj), 0);
    } else {
      OGenGCC_Designator(DEREF(OEParse_Node, _d, 39646)._left, FALSE);
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _d, 39676)._class;
        switch (_temp_) {
          case 1: {
            _DesignRG();
            break;
          }
          case 11:
          case 7:
          case 8: {
            OGenGCC_Ident(DEREF(OEParse_Node, _d, 39782)._obj);
            break;
          }
          case 2: {
            if (_adr||IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _d, 39833)._type, 39839)._form, OGenGCC_arraySet, 39846))  {
              _DesignRG();
            } else {
              FOut_String(4, (CHAR *) "(* ");
              _DesignRG();
              FOut_Char(')');
            }
            break;
          }
          case 3: {
            _type = DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 40195)._left, 40201)._type;
            while (!_FindField(_type, DEREF(OEParse_Node, _d, 40244)._obj))  {
              FOut_String(6, (CHAR *) ".base");
              _type = DEREF(OTable_Struct, _type, 40310)._base;
            }
            FOut_String(2, (CHAR *) ".");
            if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _d, 40391)._left, 40397)._type, 40403)._flags, 40387))  {
              FOut_String(48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, _d, 40509)._obj, 40514)._name);
            } else {
              OGenGCC_Ident(DEREF(OEParse_Node, _d, 40557)._obj);
            }
            break;
          }
          default: NO_LABEL (_temp_, 39670);
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
  if (DEREF(OEParse_Node, _expr, 40852)._type==_fType)  {
    OGenGCC_Expr(_expr, FALSE);
  } else if (DEREF(OTable_Struct, _fType, 40914)._form==OTable_strPointer)  {
    FOut_Char('(');
    OGenGCC_TypeDesign(_fType, FALSE);
    FOut_Char(')');
    OGenGCC_Expr(_expr, FALSE);
  } else {
    OGenGCC_Expr(_expr, FALSE);
    _type = DEREF(OEParse_Node, _expr, 41126)._type;
    do  {
      FOut_String(6, (CHAR *) ".base");
      _type = DEREF(OTable_Struct, _type, 41195)._base;
    } while (!(_type==_fType));
  }
}

static void OGenGCC_ProcCall (OEParse_Node _call) {
  OEParse_Node _apar, _prefix;
  OTable_Object _fpar;
  OTable_Struct _base, _t;
  INTEGER _i, _dim;
  BOOLEAN _tbCall;
  _apar = DEREF(OEParse_Node, _call, 41444)._right;
  _fpar = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41508)._left, 41514)._type, 41520)._link;
  _tbCall = DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41575)._left, 41581)._class==OEParse_ndTBSuper||DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41612)._left, 41618)._class==OEParse_ndTBProc;
  if (_tbCall)  {
    _t = DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41827)._left, 41833)._left, 41839)._type;
    if (DEREF(OTable_Struct, _t, 41858)._form==OTable_strPointer)  {
      _t = DEREF(OTable_Struct, _t, 41900)._base;
    }
    DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41928)._left, 41934)._obj = OTable_FindField(48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41960)._left, 41966)._obj, 41971)._name, _t);
    DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 41993)._left, 41999)._type = DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42013)._left, 42019)._obj, 42024)._type;
    _fpar = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42050)._left, 42056)._type, 42062)._link;
    DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42160)._left, 42166)._left, 42172)._link = _apar;
    _apar = DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42206)._left, 42212)._left;
    if (DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42234)._left, 42240)._class==OEParse_ndTBSuper)  {
      _fpar = OTable_FindField(48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42395)._left, 42401)._obj, 42406)._name, DEREF(OTable_Struct, _t, 42415)._base);
      _fpar = DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 42444)._type, 42450)._link;
    }
  }
  OGenGCC_Designator(DEREF(OEParse_Node, _call, 42555)._left, FALSE);
  FOut_Char('(');
  while ((void*)_apar!=(void*)NULL)  {
    if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 42629)._type, 42635)._form==OTable_strDynArray&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 42666)._type, 42672)._base, 42678)._form==OTable_strSysByte)  {
      if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 42844)._type, 42850)._flags|DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 42862)._left, 42868)._type, 42874)._flags, 42836))  {
        FOut_String(8, (CHAR *) "(BYTE*)");
      } else {
        if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43089)._type, 43095)._form==OTable_strString)  {
          FOut_Int((LONGINT) (Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _apar, 43157)._conval, 43165)._string, 43173))+1), 0);
        } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43204)._type, 43210)._form==OTable_strDynArray&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43275)._type, 43281)._flags, 43268))  {
          FOut_Int(OMachine_maxLInt, 0);
        } else {
          FOut_String(8, (CHAR *) "sizeof(");
          OGenGCC_TypeDesign(DEREF(OEParse_Node, _apar, 43455)._type, FALSE);
          FOut_Char(')');
          if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 43517)._type, 43523)._form, OGenGCC_arraySet, 43530))  {
            _t = DEREF(OEParse_Node, _apar, 43571)._type;
            OGenGCC_GetArrayPrefix(_apar, &(_prefix), &(_dim));
            while (IN(DEREF(OTable_Struct, _t, 43651)._form, OGenGCC_arraySet, 43658))  {
              INCI(_dim, 1, 43690);
              FOut_Char('*');
              OGenGCC_ArrayLength(_prefix, (LONGINT) _dim);
              _t = DEREF(OTable_Struct, _t, 43796)._base;
            }
          }
        }
        FOut_String(10, (CHAR *) ", (BYTE*)");
      }
      OGenGCC_Designator(_apar, TRUE);
    } else if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 43947)._type, 43953)._form, OGenGCC_arraySet, 43960))  {
      if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 44036)._type, 44042)._flags|DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 44054)._left, 44060)._type, 44066)._flags, 44028)&&DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 44095)._type, 44101)._form==OTable_strDynArray)  {
        if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44148)._type, 44154)._form==OTable_strString)  {
          FOut_Int((LONGINT) (Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _apar, 44216)._conval, 44224)._string, 44232))+1), 0);
          FOut_String(3, (CHAR *) ", ");
        } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44291)._type, 44297)._form==OTable_strDynArray&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44362)._type, 44368)._flags, 44355))  {
          FOut_Int(OMachine_maxLInt, 0);
          FOut_String(3, (CHAR *) ", ");
        } else {
          OGenGCC_GetArrayPrefix(_apar, &(_prefix), &(_dim));
          {
            INTEGER _temp_ = OGenGCC_DynDimensions(DEREF(OTable_Object, _fpar, 44602)._type);
            for(_i = 1 ; _i <= _temp_ ; _i += 1) {
              OGenGCC_ArrayLength(_prefix, (LONGINT) (_i+_dim));
              FOut_String(3, (CHAR *) ", ");
            }
          }
        }
      }
      if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 44746)._type, 44752)._form==OTable_strDynArray&&DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 44783)._type, 44789)._form==OTable_strDynArray)  {
        OGenGCC_Designator(_apar, TRUE);
      } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 44871)._type, 44877)._form==OTable_strDynArray)  {
        _base = DEREF(OTable_Object, _fpar, 45011)._type;
        while (DEREF(OTable_Struct, _base, 45040)._form==OTable_strDynArray)  {
          _base = DEREF(OTable_Struct, _base, 45091)._base;
        }
        FOut_Char('(');
        OGenGCC_Ident(DEREF(OTable_Struct, _base, 45215)._obj);
        FOut_String(5, (CHAR *) " *) ");
        OGenGCC_Expr(_apar, FALSE);
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 45300)._type, 45306)._form==OTable_strString&&(void*)DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 45335)._type, 45341)._obj!=(void*)NULL)  {
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_Expr(_apar, FALSE);
      } else {
        OGenGCC_Expr(_apar, FALSE);
      }
    } else if (DEREF(OTable_Object, _fpar, 45547)._mode==OTable_objVarPar)  {
      if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 45635)._type, 45641)._flags|DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _call, 45653)._left, 45659)._type, 45665)._flags, 45627)&&DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 45694)._type, 45700)._form==OTable_strRecord)  {
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_GenTypeTag(_apar);
        FOut_String(3, (CHAR *) ", ");
        if (DEREF(OEParse_Node, _apar, 45864)._type!=DEREF(OTable_Object, _fpar, 45877)._type)  {
          FOut_String(2, (CHAR *) "(");
          OGenGCC_TypeDesign(DEREF(OTable_Object, _fpar, 45946)._type, FALSE);
          FOut_String(3, (CHAR *) "*)");
        }
        OGenGCC_Designator(_apar, TRUE);
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 46058)._type, 46064)._form==OTable_strPointer&&DEREF(OEParse_Node, _apar, 46109)._class==OEParse_ndGuard)  {
        FOut_String(2, (CHAR *) "(");
        OGenGCC_Designator(_apar, FALSE);
        FOut_String(13, (CHAR *) ", (void*)(&(");
        OGenGCC_Designator(DEREF(OEParse_Node, _apar, 46331)._left, FALSE);
        FOut_String(4, (CHAR *) ")))");
      } else if (DEREF(OTable_Object, _fpar, 46393)._type!=DEREF(OEParse_Node, _apar, 46406)._type&&DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 46421)._type, 46427)._form==OTable_strSysPtr)  {
        FOut_String(7, (CHAR *) "(PTR*)");
        OGenGCC_Designator(_apar, TRUE);
      } else {
        OGenGCC_Designator(_apar, TRUE);
      }
    } else {
      if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 46731)._type, 46737)._form, 0x00084000UL, 46744)&&DEREF(OEParse_Node, _apar, 46783)._type!=DEREF(OTable_Object, _fpar, 46796)._type)  {
        OGenGCC_Projection(_apar, DEREF(OTable_Object, _fpar, 46841)._type);
      } else {
        OGenGCC_Expr(_apar, FALSE);
      }
    }
    _apar = DEREF(OEParse_Node, _apar, 46932)._link;
    if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 46954)._type, 46960)._form!=OTable_strNone)  {
      _fpar = DEREF(OTable_Object, _fpar, 47005)._link;
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
  if ('A'<=CAP((* PTR_INDEX(_oc, 0, __oc_0, 1, 48357)))&&CAP((* PTR_INDEX(_oc, 0, __oc_0, 1, 48373)))<='Z')  {
    FOut_String(__oc_0, _oc);
    if (IN(OGenGCC_fAddType, _flags, 48448))  {
      {
        SHORTINT _temp_ = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 48477)._left, 48483)._type, 48489)._form;
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
          default: NO_LABEL (_temp_, 48471);
        }
      }
      FOut_String(4, (CHAR *) _app);
    }
    FOut_Char('(');
    if (IN(OGenGCC_fLeftAdr, _flags, 48829))  {
      OGenGCC_Designator(DEREF(OEParse_Node, _n, 48865)._left, TRUE);
    } else {
      OGenGCC_Expr(DEREF(OEParse_Node, _n, 48905)._left, FALSE);
    }
    if (!IN(OGenGCC_fMonadic, _flags, 48951))  {
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Expr(DEREF(OEParse_Node, _n, 49006)._right, FALSE);
    }
    if (IN(OGenGCC_fAddPosition, _flags, 49056))  {
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _n, 49112)._pos, 0);
    }
    if (IN(OGenGCC_fAddTypename, _flags, 49156))  {
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49212)._left, 49218)._type, 49224)._obj);
    }
    if (IN(OGenGCC_fAddSize, _flags, 49261))  {
      FOut_String(10, (CHAR *) ", sizeof(");
      OGenGCC_Designator(DEREF(OEParse_Node, _n, 49329)._left, FALSE);
      FOut_Char(')');
    }
    FOut_Char(')');
  } else {
    if (IN(OGenGCC_fMonadic, _flags, 49476))  {
      FOut_String(__oc_0, _oc);
      OGenGCC_Expr(DEREF(OEParse_Node, _n, 49529)._left, _pe<=_pl);
    } else {
      if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49571)._left, 49577)._type, 49583)._form, 0x00006000UL, 49590)&&DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49637)._left, 49643)._type!=DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 49653)._right, 49660)._type)  {
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 49722)._left, _pe<=_pl);
        FOut_String(__oc_0, _oc);
        FOut_String(8, (CHAR *) "(void*)");
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 49815)._right, _pe<=_pr);
      } else {
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 49864)._left, _pe<=_pl);
        FOut_String(__oc_0, _oc);
        OGenGCC_Expr(DEREF(OEParse_Node, _n, 49925)._right, _pe<=_pr);
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
    } else if (DEREF(OEParse_Node, _expr, 50249)._class==OEParse_ndConst&&((IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50288)._type, 50294)._form, OEParse_intSet, 50301)&&DEREF(OTable_Const, DEREF(OEParse_Node, _expr, 50321)._conval, 50329)._intval<0)||(IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50362)._type, 50368)._form, OEParse_realSet, 50375)&&DEREF(OTable_Const, DEREF(OEParse_Node, _expr, 50396)._conval, 50404)._real<0.0000000000000000E+00)))  {
      return 2;
    } else if (DEREF(OEParse_Node, _expr, 50454)._class<OEParse_ndMOp)  {
      return 0;
    } else {
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 50559)._subcl;
        switch (_temp_) {
          case 17:
          case 45:
          case 24: {
            return 2;
            break;
          }
          case 1 ... 4: {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50673)._type, 50679)._form==OTable_strSet)  {
              {
                SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 50724)._subcl;
                switch (_temp_) {
                  case 1: {
                    return 8;
                    break;
                  }
                  case 2: {
                    return 9;
                    break;
                  }
                  default: NO_LABEL (_temp_, 50715);
                }
              }
            } else {
              return 3;
            }
            break;
          }
          case 6: {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 50908)._type, 50914)._form==OTable_strSet)  {
              return 10;
            } else {
              return 4;
            }
            break;
          }
          case 7: {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 51049)._type, 51055)._form==OTable_strSet)  {
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
    NO_RETURN (50136);
  }

  BOOLEAN _String (OEParse_Node _n) {
    return DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 51743)._type, 51749)._form==OTable_strString||(IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 51790)._type, 51796)._form, OGenGCC_arraySet, 51803)&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 51820)._type, 51826)._base, 51832)._form==OTable_strChar);
    NO_RETURN (51610);
  }
  if (_paren)  {
    FOut_Char('(');
  }
  _pe = _Pri(_expr);
  _pl = _Pri(DEREF(OEParse_Node, _expr, 51962)._left);
  _pr = _Pri(DEREF(OEParse_Node, _expr, 51990)._right);
  if (DEREF(OEParse_Node, _expr, 52012)._class<OEParse_ndConst)  {
    OGenGCC_Designator(_expr, FALSE);
  } else if (DEREF(OEParse_Node, _expr, 52084)._class==OEParse_ndConst)  {
    OGenGCC_Const(DEREF(OEParse_Node, _expr, 52127)._conval, DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 52141)._type, 52147)._form, DEREF(OEParse_Node, _expr, 52159)._obj);
  } else if (DEREF(OEParse_Node, _expr, 52181)._class==OEParse_ndUpto)  {
    FOut_String(7, (CHAR *) "RANGE(");
    OGenGCC_Expr(DEREF(OEParse_Node, _expr, 52249)._left, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 52305)._left, 52311)._pos, 0);
    FOut_String(3, (CHAR *) ", ");
    OGenGCC_Expr(DEREF(OEParse_Node, _expr, 52361)._right, FALSE);
    FOut_String(3, (CHAR *) ", ");
    FOut_Int(DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 52418)._right, 52425)._pos, 0);
    FOut_Char(')');
  } else if (DEREF(OEParse_Node, _expr, 52470)._class==OEParse_ndMOp&&DEREF(OEParse_Node, _expr, 52496)._subcl==OEParse_scConv)  {
    FOut_String(2, (CHAR *) "(");
    OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 52609)._type, 52615)._obj);
    FOut_String(3, (CHAR *) ") ");
    OGenGCC_Expr(DEREF(OEParse_Node, _expr, 52662)._left, _pl>=2);
  } else if (DEREF(OEParse_Node, _expr, 52692)._class==OEParse_ndMOp&&DEREF(OEParse_Node, _expr, 52718)._subcl==OEParse_scVal)  {
    if (DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 52907)._left, 52913)._class<OEParse_ndConst)  {
      FOut_String(5, (CHAR *) "(*((");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 53002)._type, 53008)._obj);
      FOut_String(4, (CHAR *) "*)&");
      OGenGCC_Expr(DEREF(OEParse_Node, _expr, 53060)._left, TRUE);
      FOut_String(3, (CHAR *) "))");
    } else {
      FOut_String(7, (CHAR *) "VAL_E(");
      OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 53205)._type, 53211)._obj);
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_Expr(DEREF(OEParse_Node, _expr, 53262)._left, TRUE);
      FOut_Char(')');
    }
  } else if (DEREF(OEParse_Node, _expr, 53323)._class==OEParse_ndMOp&&DEREF(OEParse_Node, _expr, 53349)._subcl==OEParse_scAdr)  {
    FOut_String(10, (CHAR *) "(LONGINT)");
    OGenGCC_Designator(DEREF(OEParse_Node, _expr, 53425)._left, TRUE);
  } else if (DEREF(OEParse_Node, _expr, 53454)._class==OEParse_ndMOp)  {
    _flags = 0x00000000UL;
    {
      SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 53512)._subcl;
      switch (_temp_) {
        case 7: {
          if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 53558)._type, 53564)._form!=OTable_strSet)  {
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
          _flags = 0x00000003UL;
          break;
        }
        case 22: {
          COPYSTRING(&(_str), "CAP");
          break;
        }
        case 14: {
          COPYSTRING(&(_str), "ENTIER");
          _flags = 0x00000001UL;
          break;
        }
        case 23: {
          COPYSTRING(&(_str), "ODD");
          _flags = 0x00000001UL;
          break;
        }
        case 20: {
          COPYSTRING(&(_str), "sizeof");
          break;
        }
        default: NO_LABEL (_temp_, 53503);
      }
    }
    OGenGCC_WriteOperation(_expr, 8, (CHAR *) _str, _flags|0x00000004UL, _pe, _pl, _pr);
  } else if (DEREF(OEParse_Node, _expr, 54049)._class==OEParse_ndDOp&&DEREF(OEParse_Node, _expr, 54075)._subcl==OEParse_scLen)  {
    OGenGCC_ArrayLength(DEREF(OEParse_Node, _expr, 54122)._left, DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 54134)._right, 54141)._conval, 54149)._intval);
  } else if (DEREF(OEParse_Node, _expr, 54174)._class==OEParse_ndDOp&&DEREF(OEParse_Node, _expr, 54200)._subcl==OEParse_scIs)  {
    OGenGCC_GenTypeTest(DEREF(OEParse_Node, _expr, 54264)._left, DEREF(OEParse_Node, DEREF(OEParse_Node, _expr, 54276)._right, 54283)._type);
  } else if (DEREF(OEParse_Node, _expr, 54306)._class==OEParse_ndDOp)  {
    _flags = 0x00000000UL;
    if (_String(DEREF(OEParse_Node, _expr, 54370)._left))  {
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 54425)._subcl;
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
          default: NO_LABEL (_temp_, 54416);
        }
      }
      Strings_Insert(4, (CHAR *) "STR", 0, 8, (CHAR *) _str);
    } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 54693)._type, 54699)._form==OTable_strSet)  {
      if (DEREF(OEParse_Node, _expr, 54762)._subcl==OEParse_scMinus)  {
        COPYSTRING(&(_str), "SETDIFF");
      } else {
        {
          SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 54847)._subcl;
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
            default: NO_LABEL (_temp_, 54838);
          }
        }
      }
    } else {
      {
        SHORTINT _temp_ = DEREF(OEParse_Node, _expr, 55012)._subcl;
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
            _flags = 0x00000003UL;
            break;
          }
          case 4: {
            COPYSTRING(&(_str), "MOD");
            _flags = 0x00000003UL;
            break;
          }
          case 2: {
            COPYSTRING(&(_str), "DIV");
            _flags = 0x00000003UL;
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
            _flags = 0x00000002UL;
            break;
          }
          case 26: {
            COPYSTRING(&(_str), "ASH");
            _flags = 0x00000003UL;
            break;
          }
          case 27: {
            COPYSTRING(&(_str), "BIT");
            _flags = 0x00000000UL;
            break;
          }
          case 28: {
            COPYSTRING(&(_str), "LSH");
            _flags = 0x00000001UL;
            break;
          }
          case 29: {
            COPYSTRING(&(_str), "ROT");
            _flags = 0x00000001UL;
            break;
          }
          default: NO_LABEL (_temp_, 55003);
        }
      }
    }
    OGenGCC_WriteOperation(_expr, 8, (CHAR *) _str, _flags, _pe, _pl, _pr);
  } else if (DEREF(OEParse_Node, _expr, 55931)._class==OEParse_ndCall)  {
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
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, _l, 56714)._type, 56720)._form==OTable_strRecord&&(DEREF(OEParse_Node, _l, 56747)._class==OEParse_ndVarPar||DEREF(OEParse_Node, _l, 56774)._class==OEParse_ndDeref))  {
      FOut_String(13, (CHAR *) "ASSERT_TYPE(");
      if (DEREF(OEParse_Node, _l, 56927)._class==OEParse_ndVarPar)  {
        OGenGCC_GenTagName(DEREF(OEParse_Node, _l, 56977)._obj);
      } else {
        FOut_String(5, (CHAR *) "TAG(");
        OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _l, 57082)._type, FALSE);
        FOut_String(3, (CHAR *) ", ");
        OGenGCC_Designator(DEREF(OEParse_Node, _l, 57148)._left, FALSE);
        FOut_String(3, (CHAR *) ", ");
        FOut_Int(DEREF(OEParse_Node, _l, 57209)._pos, 0);
        FOut_Char(')');
      }
      FOut_String(3, (CHAR *) ", ");
      OGenGCC_TypeDescrIdent(DEREF(OEParse_Node, _l, 57306)._type, TRUE);
      FOut_String(3, (CHAR *) ", ");
      FOut_Int(DEREF(OEParse_Node, _l, 57362)._pos, 0);
      FOut_String(3, (CHAR *) ");");
      OGenGCC_Off(_off);
    }
    OGenGCC_Designator(_l, FALSE);
    FOut_String(4, (CHAR *) " = ");
    OGenGCC_Projection(_r, DEREF(OEParse_Node, _l, 57502)._type);
  }

  void _CaseLabel (OEParse_Node _l, SHORTINT _form) {
    LONGINT _b;
    OGenGCC_Const(DEREF(OEParse_Node, _l, 57647)._conval, _form, (OTable_Object)NULL);
    if (DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57680)._conval, 57688)._intval!=DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57698)._conval, 57706)._intval2)  {
      FOut_String(6, (CHAR *) " ... ");
      _b = DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57764)._conval, 57772)._intval;
      DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57791)._conval, 57799)._intval = DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57812)._conval, 57820)._intval2;
      OGenGCC_Const(DEREF(OEParse_Node, _l, 57847)._conval, _form, (OTable_Object)NULL);
      DEREF(OTable_Const, DEREF(OEParse_Node, _l, 57878)._conval, 57886)._intval = _b;
    }
  }
  if (_paren)  {
    FOut_String(3, (CHAR *) " {");
  }
  while ((void*)_s!=(void*)NULL)  {
    OGenGCC_Off(_off);
    {
      SHORTINT _temp_ = DEREF(OEParse_Node, _s, 58037)._class;
      switch (_temp_) {
        case 16: {
          if (DEREF(OEParse_Node, _s, 58079)._subcl==OEParse_scMove)  {
            FOut_String(6, (CHAR *) "MOVE(");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 58149)._right, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58210)._right, 58217)._link, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58277)._right, 58284)._link, 58290)._link, FALSE);
            FOut_Char(')');
          } else if (DEREF(OEParse_Node, _s, 58345)._subcl==OEParse_scNewFix)  {
            if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58385)._left, 58391)._type, 58397)._base, 58403)._form==OTable_strRecord&&!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58461)._left, 58467)._type, 58473)._base, 58479)._flags, 58457))  {
              FOut_String(8, (CHAR *) "NEWREC(");
              OGenGCC_Expr(DEREF(OEParse_Node, _s, 58580)._left, FALSE);
              FOut_String(3, (CHAR *) ", ");
              OGenGCC_TypeDescrIdent(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58654)._left, 58660)._type, 58666)._base, TRUE);
            } else {
              FOut_String(8, (CHAR *) "NEWFIX(");
              OGenGCC_Expr(DEREF(OEParse_Node, _s, 58809)._left, FALSE);
            }
            OGenGCC_AppendAtomicFlag(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58868)._left, 58874)._type, 58880)._base);
          } else if (DEREF(OEParse_Node, _s, 58904)._subcl==OEParse_scNewDyn)  {
            FOut_String(7, (CHAR *) "NEWDYN");
            if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 58993)._left, 58999)._type, 59005)._base, 59011)._flags, 58989))  {
              FOut_String(5, (CHAR *) "_EXT");
            }
            FOut_Char('(');
            OGenGCC_Designator(DEREF(OEParse_Node, _s, 59155)._left, FALSE);
            FOut_String(4, (CHAR *) ", 1");
            _dim = 0;
            _n = DEREF(OEParse_Node, _s, 59235)._right;
            while ((void*)_n!=(void*)NULL)  {
              FOut_String(2, (CHAR *) "*");
              OGenGCC_Expr(_n, FALSE);
              INCI(_dim, 1, 59342);
              _n = DEREF(OEParse_Node, _n, 59371)._link;
            }
            FOut_String(3, (CHAR *) ", ");
            FOut_Int((LONGINT) _dim, 0);
            OGenGCC_AppendAtomicFlag(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59475)._left, 59481)._type, 59487)._base);
            if (!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 59530)._left, 59536)._type, 59542)._base, 59548)._flags, 59526))  {
              _dim = 0;
              _n = DEREF(OEParse_Node, _s, 59656)._right;
              while ((void*)_n!=(void*)NULL)  {
                FOut_Char(';');
                OGenGCC_Off(_off);
                FOut_String(9, (CHAR *) "SET_LEN(");
                FOut_Int((LONGINT) _dim, 0);
                FOut_String(3, (CHAR *) ", ");
                OGenGCC_Expr(_n, FALSE);
                FOut_Char(')');
                INCI(_dim, 1, 59920);
                _n = DEREF(OEParse_Node, _n, 59951)._link;
              }
            }
          } else if (DEREF(OEParse_Node, _s, 60004)._subcl==OEParse_scDispose)  {
            FOut_String(8, (CHAR *) "DISPOSE");
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60077)._left, 60083)._type, 60089)._form==OTable_strSysPtr||IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60147)._left, 60153)._type, 60159)._base, 60165)._flags, 60143))  {
            } else if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60265)._left, 60271)._type, 60277)._base, 60283)._form==OTable_strRecord)  {
              FOut_String(5, (CHAR *) "_REC");
            } else if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60358)._left, 60364)._type, 60370)._base, 60376)._form==OTable_strDynArray)  {
              FOut_String(5, (CHAR *) "_DYN");
            }
            FOut_Char('(');
            OGenGCC_Designator(DEREF(OEParse_Node, _s, 60497)._left, FALSE);
            if ((DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60528)._left, 60534)._type, 60540)._form==OTable_strPointer&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60581)._left, 60587)._type, 60593)._base, 60599)._form==OTable_strDynArray)&&!IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60660)._left, 60666)._type, 60672)._base, 60678)._flags, 60656))  {
              FOut_String(3, (CHAR *) ", ");
              _dim = 0;
              _t = DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 60761)._left, 60767)._type, 60773)._base;
              while (DEREF(OTable_Struct, _t, 60801)._form==OTable_strDynArray)  {
                INCI(_dim, 1, 60842);
                _t = DEREF(OTable_Struct, _t, 60873)._base;
              }
              FOut_Int((LONGINT) _dim, 0);
            }
            FOut_Char(')');
          } else if (DEREF(OEParse_Node, _s, 60978)._subcl==OEParse_scCollect)  {
            FOut_String(11, (CHAR *) "GC_COLLECT");
          } else if (DEREF(OEParse_Node, _s, 61054)._subcl==OEParse_scCopy)  {
            FOut_String(6, (CHAR *) "COPY(");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 61138)._left, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 61219)._right, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_ArrayLength(DEREF(OEParse_Node, _s, 61307)._right, 0);
            FOut_Char(')');
          } else if (DEREF(OEParse_Node, _s, 61383)._subcl==OEParse_scAssign&&IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61409)._left, 61415)._type, 61421)._form, 0x00084000UL, 61428))  {
            _ExtendedAssignment(DEREF(OEParse_Node, _s, 61496)._left, DEREF(OEParse_Node, _s, 61505)._right);
          } else if (DEREF(OEParse_Node, _s, 61530)._subcl==OEParse_scGet||DEREF(OEParse_Node, _s, 61554)._subcl==OEParse_scPut)  {
            if (DEREF(OEParse_Node, _s, 61593)._subcl==OEParse_scGet)  {
              FOut_String(5, (CHAR *) "GET(");
            } else {
              FOut_String(5, (CHAR *) "PUT(");
            }
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 61724)._left, FALSE);
            FOut_String(3, (CHAR *) ", ");
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 61784)._right, FALSE);
            FOut_String(3, (CHAR *) ", ");
            if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61843)._right, 61850)._type, 61856)._form, 0x0000C000UL, 61863))  {
              FOut_String(6, (CHAR *) "void*");
            } else {
              OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 61964)._right, 61971)._type, 61977)._obj);
            }
            FOut_Char(')');
          } else {
            _flags = 0x00000000UL;
            {
              SHORTINT _temp_ = DEREF(OEParse_Node, _s, 62074)._subcl;
              switch (_temp_) {
                case 0: {
                  if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62128)._right, 62135)._type, 62141)._form==OTable_strString)  {
                    COPYSTRING(&(_str), "COPYSTRING");
                    _flags = 0x00000010UL;
                  } else if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62247)._left, 62253)._type, 62259)._form, OGenGCC_arraySet, 62266))  {
                    if ((void*)DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 62305)._left, 62311)._type, 62317)._obj!=(void*)NULL)  {
                      COPYSTRING(&(_str), "COPYARRAYT");
                      _flags = 0x00000018UL;
                    } else {
                      COPYSTRING(&(_str), "COPYARRAY");
                      _flags = 0x00000030UL;
                    }
                  } else {
                    COPYSTRING(&(_str), " = ");
                  }
                  break;
                }
                case 34: {
                  COPYSTRING(&(_str), "INC");
                  _flags = 0x00000003UL;
                  break;
                }
                case 35: {
                  COPYSTRING(&(_str), "DEC");
                  _flags = 0x00000003UL;
                  break;
                }
                case 32: {
                  COPYSTRING(&(_str), "INCL");
                  _flags = 0x00000002UL;
                  break;
                }
                case 33: {
                  COPYSTRING(&(_str), "EXCL");
                  _flags = 0x00000002UL;
                  break;
                }
                case 42: {
                  COPYSTRING(&(_str), "NEWSYS");
                  _flags = 0x00000000UL;
                  break;
                }
                default: NO_LABEL (_temp_, 62068);
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
          _n = DEREF(OEParse_Node, _s, 63100)._left;
          do  {
            if (_n==DEREF(OEParse_Node, _s, 63140)._left)  {
              FOut_String(5, (CHAR *) "if (");
            } else {
              FOut_String(11, (CHAR *) " else if (");
            }
            OGenGCC_Expr(DEREF(OEParse_Node, _n, 63266)._left, FALSE);
            FOut_String(3, (CHAR *) ") ");
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _n, 63334)._right, _off+1, TRUE);
            _n = DEREF(OEParse_Node, _n, 63373)._link;
          } while (!((void*)_n==(void*)NULL));
          if ((void*)DEREF(OEParse_Node, _s, 63416)._right!=(void*)NULL)  {
            FOut_String(6, (CHAR *) " else");
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 63489)._right, _off+1, TRUE);
          }
          break;
        }
        case 20: {
          FOut_Char('{');
          OGenGCC_Off(_off+1);
          OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 63600)._left, 63606)._type, 63612)._obj);
          FOut_String(11, (CHAR *) " _temp_ = ");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 63668)._left, FALSE);
          FOut_Char(';');
          OGenGCC_Off(_off+1);
          FOut_String(18, (CHAR *) "switch (_temp_) {");
          _n = DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 63781)._right, 63788)._left;
          while ((void*)_n!=(void*)NULL)  {
            _m = DEREF(OEParse_Node, _n, 63839)._link;
            do  {
              OGenGCC_Off(_off+2);
              FOut_String(6, (CHAR *) "case ");
              _CaseLabel(_m, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 63948)._left, 63954)._type, 63960)._form);
              _m = DEREF(OEParse_Node, _m, 63987)._link;
              FOut_Char(':');
            } while (!((void*)_m==(void*)NULL));
            FOut_String(3, (CHAR *) " {");
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _n, 64099)._right, _off+3, FALSE);
            OGenGCC_Off(_off+3);
            FOut_String(7, (CHAR *) "break;");
            OGenGCC_Off(_off+2);
            FOut_Char('}');
            _n = DEREF(OEParse_Node, _n, 64240)._left;
          }
          OGenGCC_Off(_off+2);
          FOut_String(10, (CHAR *) "default: ");
          if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64326)._right, 64333)._conval, 64341)._set!=0x00000000UL)  {
            OGenGCC_StatementSeq(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 64381)._right, 64388)._right, _off+2, TRUE);
          } else {
            FOut_String(19, (CHAR *) "NO_LABEL (_temp_, ");
            FOut_Int(DEREF(OEParse_Node, _s, 64484)._pos, 0);
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
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 64681)._left, FALSE);
          FOut_String(3, (CHAR *) ") ");
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 64745)._right, _off+1, TRUE);
          break;
        }
        case 24: {
          FOut_String(4, (CHAR *) "do ");
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 64836)._left, _off+1, TRUE);
          FOut_String(11, (CHAR *) " while (!(");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 64906)._right, FALSE);
          FOut_String(4, (CHAR *) "));");
          break;
        }
        case 25: {
          FOut_Char('{');
          OGenGCC_Off(_off+1);
          OGenGCC_Ident(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65024)._left, 65030)._link, 65036)._type, 65042)._obj);
          FOut_String(8, (CHAR *) " _temp_");
          FOut_String(4, (CHAR *) " = ");
          OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65121)._left, 65127)._right, FALSE);
          FOut_Char(';');
          OGenGCC_Off(_off+1);
          FOut_String(5, (CHAR *) "for(");
          OGenGCC_Designator(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65235)._left, 65241)._link, FALSE);
          FOut_String(4, (CHAR *) " = ");
          OGenGCC_Expr(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65298)._left, 65304)._left, FALSE);
          FOut_String(4, (CHAR *) " ; ");
          OGenGCC_Designator(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65367)._left, 65373)._link, FALSE);
          if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65402)._left, 65408)._conval, 65416)._intval>0)  {
            FOut_String(5, (CHAR *) " <= ");
          } else {
            FOut_String(5, (CHAR *) " >= ");
          }
          FOut_String(10, (CHAR *) "_temp_ ; ");
          OGenGCC_Designator(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65570)._left, 65576)._link, FALSE);
          FOut_String(5, (CHAR *) " += ");
          FOut_Int(DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 65635)._left, 65641)._conval, 65649)._intval, 0);
          FOut_Char(')');
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 65708)._right, _off+2, TRUE);
          OGenGCC_Off(_off);
          FOut_Char('}');
          break;
        }
        case 28: {
          FOut_String(9, (CHAR *) "while(1)");
          DEREF(OEParse_Node, _s, 65828)._pos = -DEREF(OEParse_Node, _s, 65839)._pos;
          OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 65869)._left, _off+1, TRUE);
          if (DEREF(OEParse_Node, _s, 65904)._pos>=0)  {
            OGenGCC_Off(_off);
            FOut_String(6, (CHAR *) "_exit");
            FOut_Int(DEREF(OEParse_Node, _s, 66038)._pos, 0);
            FOut_String(4, (CHAR *) ": ;");
          }
          break;
        }
        case 29: {
          _n = DEREF(OEParse_Node, _s, 66124)._left;
          while ((void*)_n!=(void*)NULL)  {
            if (_n!=DEREF(OEParse_Node, _s, 66178)._left)  {
              FOut_String(6, (CHAR *) "else ");
            }
            FOut_String(5, (CHAR *) "if (");
            OGenGCC_GenTypeTest(DEREF(OEParse_Node, _n, 66290)._left, DEREF(OTable_Object, DEREF(OEParse_Node, _n, 66299)._obj, 66304)._type);
            FOut_Char(')');
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _n, 66362)._right, _off+1, TRUE);
            _n = DEREF(OEParse_Node, _n, 66401)._link;
          }
          OGenGCC_Off(_off);
          FOut_String(5, (CHAR *) "else");
          if (DEREF(OTable_Const, DEREF(OEParse_Node, _s, 66480)._conval, 66488)._set==0x00000000UL)  {
            FOut_String(13, (CHAR *) " { NO_GUARD(");
            FOut_Int(DEREF(OEParse_Node, _s, 66560)._pos, 0);
            FOut_String(5, (CHAR *) "); }");
          } else {
            OGenGCC_StatementSeq(DEREF(OEParse_Node, _s, 66637)._right, _off+1, TRUE);
          }
          break;
        }
        case 31: {
          DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66698)._left, 66704)._pos = ABSLI(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66719)._left, 66725)._pos, 66713);
          FOut_String(11, (CHAR *) "goto _exit");
          FOut_Int(DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66816)._left, 66822)._pos, 0);
          FOut_Char(';');
          break;
        }
        case 32: {
          FOut_String(7, (CHAR *) "return");
          if ((void*)DEREF(OEParse_Node, _s, 66916)._left!=(void*)NULL)  {
            if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 66950)._left, 66956)._type, 66962)._form, 0x00006000UL, 66969)&&DEREF(OEParse_Node, DEREF(OEParse_Node, _s, 67018)._left, 67024)._type!=DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _s, 67034)._obj, 67039)._type, 67045)._base)  {
              FOut_String(8, (CHAR *) "(void*)");
            }
            FOut_Char(' ');
            OGenGCC_Expr(DEREF(OEParse_Node, _s, 67147)._left, FALSE);
          }
          FOut_Char(';');
          break;
        }
        case 33: {
          FOut_String(5, (CHAR *) "HALT");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 67256)._left, TRUE);
          FOut_Char(';');
          break;
        }
        case 36: {
          FOut_String(8, (CHAR *) "ASSERT(");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 67357)._right, FALSE);
          FOut_String(3, (CHAR *) ", ");
          OGenGCC_Expr(DEREF(OEParse_Node, _s, 67414)._left, FALSE);
          FOut_String(3, (CHAR *) ", ");
          FOut_Int(DEREF(OEParse_Node, _s, 67471)._pos, 0);
          FOut_String(3, (CHAR *) ");");
          break;
        }
        default:  {
        HALT(102);
      }
      }
    }
    _s = DEREF(OEParse_Node, _s, 67559)._link;
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
    if ((void*)DEREF(OEParse_Node, _enter, 67913)._obj!=(void*)NULL)  {
      _obj = DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 68016)._obj, 68021)._type, 68027)._link;
      while ((void*)_obj!=(void*)NULL)  {
        if (DEREF(OTable_Object, _obj, 68081)._mode==OTable_objVar&&IN(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 68106)._type, 68112)._form, OGenGCC_arraySet, 68119))  {
          OGenGCC_Off(_off+1);
          OGenGCC_OpenArrayDecl(_obj, FALSE);
          FOut_Char(';');
        }
        _obj = DEREF(OTable_Object, _obj, 68262)._link;
      }
    }
    _obj = DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 68347)._obj, 68352)._link, 68358)._right;
    OGenGCC_LocalDecl(_obj, _enter, _off+1);
    if ((void*)DEREF(OEParse_Node, _enter, 68470)._obj!=(void*)NULL)  {
      _obj = DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 68582)._obj, 68587)._type, 68593)._link;
      while ((void*)_obj!=(void*)NULL)  {
        if (DEREF(OTable_Object, _obj, 68647)._mode==OTable_objVar&&IN(DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 68672)._type, 68678)._form, OGenGCC_arraySet, 68685))  {
          OGenGCC_Off(_off+1);
          if (DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 68747)._type, 68753)._form==OTable_strArray)  {
            FOut_String(14, (CHAR *) "VALUE_ARRAYF(");
          } else {
            FOut_String(13, (CHAR *) "VALUE_ARRAY(");
          }
          OGenGCC_Ident(_obj);
          FOut_String(4, (CHAR *) ", _");
          OGenGCC_Ident(_obj);
          FOut_String(6, (CHAR *) "_p, 1");
          if (DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 69025)._type, 69031)._form==OTable_strDynArray)  {
            {
              INTEGER _temp_ = OGenGCC_DynDimensions(DEREF(OTable_Object, _obj, 69106)._type)- 1;
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
        _obj = DEREF(OTable_Object, _obj, 69365)._link;
      }
    }
    OGenGCC_StatementSeq(DEREF(OEParse_Node, _enter, 69448)._right, _off+1, FALSE);
    if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _enter, 69487)._obj, 69492)._type, 69498)._base, 69504)._form!=OTable_strNone)  {
      OGenGCC_Off(_off+1);
      FOut_String(12, (CHAR *) "NO_RETURN (");
      FOut_Int(DEREF(OEParse_Node, _enter, 69604)._pos, 0);
      FOut_String(3, (CHAR *) ");");
    }
  }
  while ((void*)_obj!=(void*)NULL&&DEREF(OTable_Object, _obj, 69748)._mode<OTable_objExtProc)  {
    _obj = OGenGCC_Decl(_obj, 0x00000002UL, _off);
  }
  _local = DEREF(OEParse_Node, _enter, 69945)._left;
  while ((void*)_local!=(void*)NULL)  {
    FOut_Ln();
    if (DEREF(OEParse_Node, _local, 70007)._class==OEParse_ndForward&&(void*)DEREF(OEParse_Node, _enter, 70038)._obj!=(void*)NULL)  {
      _next = OGenGCC_Decl(DEREF(OEParse_Node, _local, 70137)._obj, 0x00000020UL, _off);
    } else {
      _next = OGenGCC_Decl(DEREF(OEParse_Node, _local, 70205)._obj, 0x00000000UL, _off);
    }
    if (DEREF(OEParse_Node, _local, 70247)._class==OEParse_ndForward)  {
      FOut_String(2, (CHAR *) ";");
    } else {
      FOut_String(3, (CHAR *) " {");
      _ProcedureBlock(_local, _off);
      OGenGCC_Off(_off);
      FOut_Char('}');
    }
    _local = DEREF(OEParse_Node, _local, 70474)._link;
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
    OGenGCC_WriteInclude(256, (CHAR *) INDEX(DEREF(ODepend_Module, _module, 71679)._file, OGenGCC_header, 5, 71685));
  }
  _obj = DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 71766)._link, 71772)._next;
  while ((void*)_obj!=(void*)NULL)  {
    if (DEREF(OTable_Object, DEREF(OTable_Object, _obj, 71819)._link, 71825)._mnolev<=-3)  {
      if (OTable_external||IN(OTable_flagExport, DEREF(OTable_Object, _obj, 71904)._flags, 71898)==_writeHeader)  {
        OTable_GetModuleName(_obj, 256, (CHAR *) _name);
        _import = DEREF(ODepend_Module, _module, 71998)._import;
        while ((void*)_import!=(void*)NULL&&STRNEQ(DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 72048)._module, 72056)._name, _name))  {
          _import = DEREF(ODepend_Import, _import, 72102)._next;
        }
        if ((void*)_import!=(void*)NULL)  {
          OGenGCC_WriteInclude(256, (CHAR *) INDEX(DEREF(ODepend_Module, DEREF(ODepend_Import, _import, 72189)._module, 72197)._file, OGenGCC_header, 5, 72203));
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
    _obj = DEREF(OTable_Object, _obj, 72541)._next;
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
  OGenGCC_GenTypeDescrForward(DEREF(OEParse_Node, OGenGCC_root, 73547)._link, TRUE);
  _obj = DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 73645)._link, 73651)._right;
  while ((void*)_obj!=(void*)NULL)  {
    _obj = OGenGCC_Decl(_obj, 0x00000003UL, 0);
  }
  FOut_Ln();
  OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 73782)._link, TRUE);
  FOut_Ln();
  OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 73868)._link, TRUE);
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
  OGenGCC_GenTypeDescrForward(DEREF(OEParse_Node, OGenGCC_root, 74593)._link, FALSE);
  OGenGCC_LocalDecl(DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 74640)._link, 74646)._right, OGenGCC_root, 0);
  OGenGCC_GenTypeDescr(DEREF(OEParse_Node, OGenGCC_root, 74691)._link, FALSE);
  FOut_Ln();
  FOut_Ln();
  FOut_String(11, (CHAR *) "void _init");
  OGenGCC_Ident(OTable_compiledModule);
  FOut_String(10, (CHAR *) " (void) {");
  FOut_Ln();
  FOut_String(27, (CHAR *) "  moduleId = add_module (\"");
  FOut_String(48, (CHAR *) DEREF(OTable_Object, OTable_compiledModule, 74903)._name);
  FOut_String(4, (CHAR *) "\");");
  if (!OTable_external)  {
    OGenGCC_GenTypeDescrInit(DEREF(OEParse_Node, OGenGCC_root, 75042)._link);
  }
  OGenGCC_StatementSeq(DEREF(OEParse_Node, OGenGCC_root, 75081)._right, 1, FALSE);
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
  if (OTable_external&&STRNEQ(DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, OTable_compiledModule, 75901)._const, 75908)._string, 75916), "C"))  {
    OScan_WarnIns(DEREF(OTable_Object, OTable_compiledModule, 75963)._pos, 297, 256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, OTable_compiledModule, 75991)._const, 75998)._string, 76006));
  }
  OGenGCC_CheckName(OTable_compiledModule, &(DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 76113)._link, 76119)._right));
  if (IN(ODepend_flSymChanged, DEREF(ODepend_Module, _mod, 76160)._flags, 76154)||!IN(OGenGCC_header, DEREF(ODepend_Module, _mod, 76187)._flags, 76181))  {
    ODepend_NewFile(_mod, OGenGCC_header, 2, (CHAR *) "h");
    FOut_Open(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 76320)._file, OGenGCC_header, 5, 76326));
    OGenGCC_GenHeader(_mod);
    FOut_Close();
    INCL(DEREF(ODepend_Module, _mod, 76390)._flags, OGenGCC_header, 76381);
  }
  if (OTable_external)  {
    _UpdateMarker(98, (CHAR *) DEREF(ODepend_Module, _mod, 76462)._name);
  } else {
    ODepend_NewFile(_mod, OGenGCC_cFile, 2, (CHAR *) "c");
    INCL(DEREF(ODepend_Module, _mod, 76532)._flags, OGenGCC_cFile, 76523);
    FOut_Open(256, (CHAR *) INDEX(DEREF(ODepend_Module, _mod, 76593)._file, OGenGCC_cFile, 5, 76599));
    OGenGCC_GenModule(_mod);
    FOut_Ln();
    FOut_Close();
  }
}

static LONGINT OGenGCC_StructSize (OTable_Struct _t) {
  {
    SHORTINT _temp_ = DEREF(OTable_Struct, _t, 76915)._form;
    switch (_temp_) {
      case 1:
      case 2:
      case 3:
      case 12: {
        return 1;
        break;
      }
      case 4: {
        return 2;
        break;
      }
      case 5:
      case 6:
      case 8:
      case 13:
      case 15:
      case 14: {
        return 4;
        break;
      }
      case 7: {
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
  NO_RETURN (76693);
}

void _init_OGenGCC (void) {
  moduleId = add_module ("OGenGCC");
  OGenGCC_atCount = 0;
  OGenGCC_emptyObj = OTable_NewObject(2, (CHAR *) "@", OTable_objType, -1);
  OTable_structSize = OGenGCC_StructSize;
}
