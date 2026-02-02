#define MODULE_OEParse

#include "OEParse.h"
#include "ConvTypes.h"
#include "IntStr.h"
#include "OMachine.h"
#include "OScan.h"
#include "Strings.h"
#include "Strings2.h"

static ModuleId moduleId;



static const SHORTINT OEParse_ndCollected = -1;
const SHORTINT OEParse_ndVar = 1;
const SHORTINT OEParse_ndVarPar = 2;
const SHORTINT OEParse_ndField = 3;
const SHORTINT OEParse_ndDeref = 4;
const SHORTINT OEParse_ndIndex = 5;
const SHORTINT OEParse_ndGuard = 6;
const SHORTINT OEParse_ndType = 7;
const SHORTINT OEParse_ndProc = 8;
const SHORTINT OEParse_ndTBProc = 9;
const SHORTINT OEParse_ndTBSuper = 10;
const SHORTINT OEParse_ndConst = 11;
const SHORTINT OEParse_ndUpto = 12;
const SHORTINT OEParse_ndMOp = 13;
const SHORTINT OEParse_ndDOp = 14;
const SHORTINT OEParse_ndInitTd = 15;
const SHORTINT OEParse_ndAssign = 16;
const SHORTINT OEParse_ndCall = 17;
const SHORTINT OEParse_ndIfElse = 18;
const SHORTINT OEParse_ndIf = 19;
const SHORTINT OEParse_ndCase = 20;
const SHORTINT OEParse_ndCaseElse = 21;
const SHORTINT OEParse_ndCaseDo = 22;
const SHORTINT OEParse_ndWhile = 23;
const SHORTINT OEParse_ndRepeat = 24;
const SHORTINT OEParse_ndFor = 25;
const SHORTINT OEParse_ndForHeader = 26;
const SHORTINT OEParse_ndForRange = 27;
const SHORTINT OEParse_ndLoop = 28;
const SHORTINT OEParse_ndWithElse = 29;
const SHORTINT OEParse_ndWithGuard = 30;
const SHORTINT OEParse_ndExit = 31;
const SHORTINT OEParse_ndReturn = 32;
const SHORTINT OEParse_ndTrap = 33;
const SHORTINT OEParse_ndEnter = 34;
const SHORTINT OEParse_ndForward = 35;
const SHORTINT OEParse_ndAssert = 36;
const SHORTINT OEParse_scTimes = 1;
const SHORTINT OEParse_scRDiv = 2;
const SHORTINT OEParse_scIDiv = 3;
const SHORTINT OEParse_scMod = 4;
const SHORTINT OEParse_scAnd = 5;
const SHORTINT OEParse_scPlus = 6;
const SHORTINT OEParse_scMinus = 7;
const SHORTINT OEParse_scOr = 8;
const SHORTINT OEParse_scEql = 9;
const SHORTINT OEParse_scNeq = 10;
const SHORTINT OEParse_scLss = 11;
const SHORTINT OEParse_scLeq = 12;
const SHORTINT OEParse_scGrt = 13;
const SHORTINT OEParse_scGeq = 14;
const SHORTINT OEParse_scIn = 15;
const SHORTINT OEParse_scIs = 16;
const SHORTINT OEParse_scNot = 17;
const SHORTINT OEParse_scSize = 20;
const SHORTINT OEParse_scAbs = 21;
const SHORTINT OEParse_scCap = 22;
const SHORTINT OEParse_scOdd = 23;
const SHORTINT OEParse_scEntier = 14;
const SHORTINT OEParse_scAdr = 24;
const SHORTINT OEParse_scCc = 25;
const SHORTINT OEParse_scAsh = 26;
const SHORTINT OEParse_scBit = 27;
const SHORTINT OEParse_scLsh = 28;
const SHORTINT OEParse_scRot = 29;
const SHORTINT OEParse_scLen = 30;
const SHORTINT OEParse_scVal = 31;
const SHORTINT OEParse_scAssign = 0;
const SHORTINT OEParse_scIncl = 32;
const SHORTINT OEParse_scExcl = 33;
const SHORTINT OEParse_scInc = 34;
const SHORTINT OEParse_scDec = 35;
const SHORTINT OEParse_scCopy = 36;
const SHORTINT OEParse_scMove = 37;
const SHORTINT OEParse_scGet = 38;
const SHORTINT OEParse_scPut = 39;
const SHORTINT OEParse_scGetReg = 40;
const SHORTINT OEParse_scPutReg = 41;
const SHORTINT OEParse_scNewSys = 42;
const SHORTINT OEParse_scNewFix = 43;
const SHORTINT OEParse_scNewDyn = 44;
const SHORTINT OEParse_scDispose = 47;
const SHORTINT OEParse_scConv = 45;
const SHORTINT OEParse_scCollect = 48;
static const SHORTINT OEParse_grpNumeric = -1;
const SHORTINT OEParse_grpInteger = -2;
static const SHORTINT OEParse_grpNilComp = -3;
static const SHORTINT OEParse_grpShift = -4;
static const SHORTINT OEParse_grpExtStd = -5;
static const SHORTINT OEParse_grpPointer = -6;
static const SHORTINT OEParse_grpNumOrSet = -7;
const SHORTINT OEParse_grpIntOrChar = -8;
const SHORTINT OEParse_grpReal = -9;
const SET OEParse_intSet = 0x00000038UL;
const SET OEParse_realSet = 0x000000C0UL;
const SET OEParse_numSet = 0x000000F8UL;
static const SET OEParse_nilCompSet = 0x0000E000UL;
static const SET OEParse_arraySet = 0x00060000UL;
static OEParse_Node OEParse_nodeHeap;



OEParse_Node OEParse_NewNode (SHORTINT _class) {
  OEParse_Node _n;
  if ((void*)OEParse_nodeHeap!=(void*)NULL)  {
    _n = OEParse_nodeHeap;
    OEParse_nodeHeap = DEREF(OEParse_Node, OEParse_nodeHeap, 3491)._link;
  } else {
    NEWREC(_n, td_OEParse_NodeDesc, 0);
  }
  DEREF(OEParse_Node, _n, 3535)._left = (OEParse_Node)NULL;
  DEREF(OEParse_Node, _n, 3551)._right = (OEParse_Node)NULL;
  DEREF(OEParse_Node, _n, 3568)._link = (OEParse_Node)NULL;
  DEREF(OEParse_Node, _n, 3588)._class = _class;
  DEREF(OEParse_Node, _n, 3607)._subcl = 0;
  DEREF(OEParse_Node, _n, 3622)._pos = OScan_lastSym;
  DEREF(OEParse_Node, _n, 3647)._obj = (OTable_Object)NULL;
  DEREF(OEParse_Node, _n, 3662)._conval = (OTable_Const)NULL;
  if (_class>=OEParse_ndInitTd)  {
    DEREF(OEParse_Node, _n, 3718)._type = INDEX(OTable_predeclType, OTable_strNone, 14, 3741);
  } else {
    DEREF(OEParse_Node, _n, 3769)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 3792);
  }
  return _n;
  NO_RETURN (3199);
}

static void OEParse_TypeName (OTable_Struct _str, LONGINT __descr_0, CHAR (* _descr)) {
  CHAR _elemDescr[256];
  if ((void*)DEREF(OTable_Struct, _str, 4039)._obj!=(void*)NULL)  {
    if (DEREF(OTable_Object, DEREF(OTable_Struct, _str, 4087)._obj, 4092)._mnolev<-1)  {
      OTable_GetModuleName(DEREF(OTable_Struct, _str, 4153)._obj, __descr_0, _descr);
      Strings2_AppendChar('.', __descr_0, _descr);
    } else {
      COPY("", _descr, __descr_0);
    }
    Strings_Append(48, (CHAR *) DEREF(OTable_Object, DEREF(OTable_Struct, _str, 4277)._obj, 4282)._name, __descr_0, _descr);
  } else {
    {
      SHORTINT _temp_ = DEREF(OTable_Struct, _str, 4320)._form;
      switch (_temp_) {
        case 17:
        case 18: {
          if (DEREF(OTable_Struct, _str, 4385)._form==OTable_strArray)  {
            COPY("ARRAY ", _descr, __descr_0);
            IntStr_Append(DEREF(OTable_Struct, _str, 4477)._len, 0, ConvTypes_left, __descr_0, _descr);
            Strings_Append(5, (CHAR *) " OF ", __descr_0, _descr);
          } else {
            COPY("ARRAY OF ", _descr, __descr_0);
          }
          OEParse_TypeName(DEREF(OTable_Struct, _str, 4636)._base, 256, (CHAR *) _elemDescr);
          Strings_Append(256, (CHAR *) _elemDescr, __descr_0, _descr);
          break;
        }
        case 16: {
          COPY("type bound PROCEDURE", _descr, __descr_0);
          break;
        }
        case 11: {
          COPY("NIL", _descr, __descr_0);
          break;
        }
        case 9: {
          COPY("void", _descr, __descr_0);
          break;
        }
        case 14: {
          COPY("POINTER", _descr, __descr_0);
          break;
        }
        case 15: {
          COPY("PROCEDURE", _descr, __descr_0);
          break;
        }
        case 10: {
          COPY("string", _descr, __descr_0);
          break;
        }
        case 19: {
          COPY("RECORD", _descr, __descr_0);
          break;
        }
        case 0: {
          COPY("undef", _descr, __descr_0);
          break;
        }
        default: NO_LABEL (_temp_, 4312);
      }
    }
  }
}

void OEParse_ErrT1 (LONGINT _pos, INTEGER _num, OTable_Struct (* _t1)) {
  CHAR _ins[128];
  if (DEREF(OTable_Struct, (* _t1), 5394)._form!=OTable_strUndef)  {
    OEParse_TypeName((* _t1), 128, (CHAR *) _ins);
    OScan_ErrIns(_pos, _num, 128, (CHAR *) _ins);
    (* _t1) = INDEX(OTable_predeclType, OTable_strUndef, 14, 5503);
  }
}

void OEParse_ErrNT1 (INTEGER _num, OEParse_Node _n1) {
  OEParse_ErrT1(DEREF(OEParse_Node, _n1, 5690)._pos, _num, &(DEREF(OEParse_Node, _n1, 5704)._type));
}

void OEParse_Err (INTEGER _num) {
  OScan_Err(-1, _num);
}

static SHORTINT OEParse_IntConstType (LONGINT _val) {
  if (-32768<=_val&&_val<=32767)  {
    return OTable_strShortInt;
  } else if (-2147483648<=_val&&_val<=2147483647)  {
    return OTable_strInteger;
  } else {
    return OTable_strLongInt;
  }
  NO_RETURN (5865);
}

static void OEParse_Convert (OEParse_Node (* _r), SHORTINT _to) {
  OEParse_Node _n;
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 6654)._type, 6660)._form!=_to&&IN(_to, 0x000000FCUL, 6679))  {
    if (DEREF(OEParse_Node, (* _r), 6718)._class==OEParse_ndConst)  {
      if ((IN(DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 6780)._type, 6786)._form, OEParse_intSet, 6793)&&_to!=OTable_strChar)&&OEParse_IntConstType(DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 6853)._conval, 6861)._intval)>_to)  {
        OEParse_ErrNT1(256, (* _r));
      } else if (_to==OTable_strChar&&(DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 6998)._conval, 7006)._intval<0||DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7031)._conval, 7039)._intval>255))  {
        OEParse_Err(256);
      } else if ((_to==OTable_strShortInt&&DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 7163)._type, 7169)._form==OTable_strChar)&&DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7193)._conval, 7201)._intval>32767)  {
        OEParse_Err(256);
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 7315)._type, 7321)._form>=OTable_strReal&&_to<OTable_strReal)  {
        DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7377)._conval, 7385)._intval = ENTIERLR(DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7406)._conval, 7414)._real);
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 7438)._type, 7444)._form<OTable_strReal&&_to>=OTable_strReal)  {
        DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7500)._conval, 7508)._real = (LONGREAL) DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7519)._conval, 7527)._intval;
      }
      DEREF(OEParse_Node, (* _r), 7558)._obj = (OTable_Object)NULL;
    } else if (DEREF(OEParse_Node, (* _r), 7585)._subcl!=OEParse_scConv||DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 7608)._type, 7614)._form<_to)  {
      _n = OEParse_NewNode(OEParse_ndMOp);
      ASSERT_TYPE(TAG(TD_OEParse_NodeDesc, _n, 7857), td_OEParse_NodeDesc, 7857);
      DEREF(OEParse_Node, _n, 7857) = DEREF(OEParse_Node, (* _r), 7863);
      DEREF(OEParse_Node, (* _r), 7875)._class = OEParse_ndMOp;
      DEREF(OEParse_Node, (* _r), 7902)._subcl = OEParse_scConv;
      DEREF(OEParse_Node, (* _r), 7930)._left = _n;
      DEREF(OEParse_Node, (* _r), 7952)._right = (OEParse_Node)NULL;
      DEREF(OEParse_Node, (* _r), 7977)._obj = (OTable_Object)NULL;
      DEREF(OEParse_Node, (* _r), 8000)._conval = (OTable_Const)NULL;
    }
    DEREF(OEParse_Node, (* _r), 8054)._type = INDEX(OTable_predeclType, _to, 14, 8077);
  }
}

static BOOLEAN OEParse_StringConv (OEParse_Node _node) {
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _node, 8348)._type, 8354)._form==OTable_strString)  {
    return TRUE;
  } else if (DEREF(OEParse_Node, _node, 8412)._class==OEParse_ndConst&&DEREF(OTable_Struct, DEREF(OEParse_Node, _node, 8436)._type, 8442)._form==OTable_strChar)  {
    DEREF(OEParse_Node, _node, 8475)._type = INDEX(OTable_predeclType, OTable_strString, 14, 8498);
    DEREF(OEParse_Node, _node, 8523)._obj = (OTable_Object)NULL;
    NEWFIX(DEREF(OTable_Const, DEREF(OEParse_Node, _node, 8552)._conval, 8560)._string, 1);
    INDEX(DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _node, 8581)._conval, 8589)._string, 8597), 0, 256, 8597) = (CHAR) DEREF(OTable_Const, DEREF(OEParse_Node, _node, 8613)._conval, 8621)._intval;
    INDEX(DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _node, 8642)._conval, 8650)._string, 8658), 1, 256, 8658) = '\000';
    return TRUE;
  } else {
    return FALSE;
  }
  NO_RETURN (8116);
}

BOOLEAN OEParse_ArrayComp (OEParse_Node _a, OTable_Struct _f) {

  BOOLEAN _ArrayCompRec (OTable_Struct _a, OTable_Struct _f) {
    return OTable_SameType(_a, _f)||((DEREF(OTable_Struct, _f, 8981)._form==OTable_strDynArray&&IN(DEREF(OTable_Struct, _a, 9007)._form, OEParse_arraySet, 9014))&&_ArrayCompRec(DEREF(OTable_Struct, _a, 9044)._base, DEREF(OTable_Struct, _f, 9053)._base));
    NO_RETURN (8885);
  }
  return ((DEREF(OTable_Struct, _f, 9105)._form==OTable_strDynArray&&DEREF(OTable_Struct, DEREF(OTable_Struct, _f, 9131)._base, 9137)._form==OTable_strChar)&&OEParse_StringConv(_a))||_ArrayCompRec(DEREF(OEParse_Node, _a, 9190)._type, _f);
  NO_RETURN (8752);
}

BOOLEAN OEParse_AssignComp (OTable_Struct _var, OEParse_Node _expr) {
  SHORTINT _tv, _te;
  _tv = DEREF(OTable_Struct, _var, 9686)._form;
  _te = DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 9704)._type, 9710)._form;
  if (((((((OTable_SameType(_var, DEREF(OEParse_Node, _expr, 9746)._type)||OTable_ExtOf(DEREF(OEParse_Node, _expr, 9778)._type, _var))||(IN(_tv, OEParse_nilCompSet, 9806)&&_te==OTable_strNil))||(((_tv==OTable_strArray&&DEREF(OTable_Struct, DEREF(OTable_Struct, _var, 9870)._base, 9876)._form==OTable_strChar)&&_te==OTable_strString)&&(LONGINT) Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _expr, 9942)._conval, 9950)._string, 9958))<DEREF(OTable_Struct, _var, 9964)._len))||((((_tv==OTable_strArray&&DEREF(OTable_Struct, DEREF(OTable_Struct, _var, 10004)._base, 10010)._form==OTable_strChar)&&DEREF(OTable_Struct, _var, 10044)._len>1)&&_te==OTable_strChar)&&OEParse_StringConv(_expr)))||((((_tv==OTable_strProc&&DEREF(OEParse_Node, _expr, 10125)._class==OEParse_ndProc)&&DEREF(OTable_Object, DEREF(OEParse_Node, _expr, 10158)._obj, 10163)._mnolev<=0)&&OTable_ParamsMatch(_var, DEREF(OEParse_Node, _expr, 10216)._type))&&(DEREF(OTable_Object, DEREF(OEParse_Node, _expr, 10242)._obj, 10247)._mode==OTable_objExtProc||DEREF(OTable_Object, DEREF(OEParse_Node, _expr, 10276)._obj, 10281)._mode==OTable_objLocalProc)))||(_tv==OTable_strSysByte&&(_te==OTable_strChar||_te==OTable_strShortInt)))||(_tv==OTable_strSysPtr&&_te==OTable_strPointer))  {
    return TRUE;
  } else if ((IN(_te, OEParse_numSet, 10460)&&IN(_tv, OEParse_numSet, 10477))&&_tv>_te)  {
    OEParse_Convert(&(_expr), _tv);
    return TRUE;
  } else if ((IN(_te, OEParse_intSet, 10648)&&_tv==OTable_strPointer)&&IN(OTable_flagExternal, DEREF(OTable_Struct, _var, 10705)._flags, 10699))  {
    OEParse_Convert(&(_expr), OTable_strLongInt);
    return TRUE;
  } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 10853)._type, 10859)._form==OTable_strUndef)  {
    return TRUE;
  } else {
    OEParse_ErrT1(DEREF(OEParse_Node, _expr, 11005)._pos, 166, &(_var));
    return FALSE;
  }
  NO_RETURN (9229);
}

static BOOLEAN OEParse_String (OEParse_Node _n) {
  return DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 11204)._type, 11210)._form==OTable_strString||(IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 11237)._type, 11243)._form, OEParse_arraySet, 11250)&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 11267)._type, 11273)._base, 11279)._form==OTable_strChar);
  NO_RETURN (11079);
}

BOOLEAN OEParse_ChkGuard (OEParse_Node _var, OTable_Object _typeObj, BOOLEAN _chkQual, LONGINT _tpos) {
  LONGINT _pos;
  _pos = OScan_lastErr;
  if (DEREF(OTable_Object, _typeObj, 11945)._mode!=OTable_objType)  {
    OScan_Err(_tpos, 205);
  } else {
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 12036)._type, 12042)._form==OTable_strPointer)  {
      if ((_chkQual&&DEREF(OEParse_Node, _var, 12095)._class!=OEParse_ndVar)&&DEREF(OEParse_Node, _var, 12118)._class!=OEParse_ndVarPar)  {
        OScan_Err(DEREF(OEParse_Node, _var, 12163)._pos, 244);
      } else if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 12240)._type, 12246)._base, 12252)._form!=OTable_strRecord)  {
        OScan_Err(DEREF(OEParse_Node, _var, 12299)._pos, 237);
      } else if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 12382)._type, 12388)._base, 12394)._flags, 12376))  {
        OScan_Err(DEREF(OEParse_Node, _var, 12428)._pos, 164);
      }
    } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 12534)._type, 12540)._form==OTable_strRecord)  {
      if (DEREF(OEParse_Node, _var, 12582)._class!=OEParse_ndVarPar)  {
        OScan_Err(DEREF(OEParse_Node, _var, 12627)._pos, 236);
      } else if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 12713)._type, 12719)._flags, 12707))  {
        OScan_Err(DEREF(OEParse_Node, _var, 12753)._pos, 164);
      }
    } else {
      OEParse_ErrT1(DEREF(OEParse_Node, _var, 12872)._pos, 164, &(DEREF(OEParse_Node, _var, 12887)._type));
    }
    if (!OTable_ExtOf(DEREF(OTable_Object, _typeObj, 12959)._type, DEREF(OEParse_Node, _var, 12970)._type))  {
      OEParse_ErrT1(_tpos, 163, &(DEREF(OEParse_Node, _var, 13012)._type));
    }
  }
  return _pos==OScan_lastErr;
  NO_RETURN (11324);
}

void OEParse_ChkType (OEParse_Node _n, SHORTINT _form) {
  SET _set;
  CHAR _expected[32];

  void _ErrTx (void) {
    CHAR _ins[128];
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 13622)._type, 13628)._form!=OTable_strUndef)  {
      OEParse_TypeName(DEREF(OEParse_Node, _n, 13673)._type, 128, (CHAR *) _ins);
      OScan_ErrIns2(DEREF(OEParse_Node, _n, 13707)._pos, 199, 32, (CHAR *) _expected, 128, (CHAR *) _ins);
    }
  }
  if (_form<0)  {
    {
      SHORTINT _temp_ = _form;
      switch (_temp_) {
        case -1: {
          _set = OEParse_numSet;
          COPYSTRING(&(_expected), "Numeric");
          break;
        }
        case -2: {
          _set = OEParse_intSet;
          COPYSTRING(&(_expected), "Integer");
          break;
        }
        case -3: {
          _set = OEParse_nilCompSet;
          COPYSTRING(&(_expected), "NIL compatible");
          break;
        }
        case -4: {
          _set = 0x0000103CUL;
          COPYSTRING(&(_expected), "Integer, Char, or Byte");
          break;
        }
        case -5: {
          _set = 0x0000E1FEUL;
          COPYSTRING(&(_expected), "Unstructured");
          break;
        }
        case -6: {
          _set = 0x00006000UL;
          COPYSTRING(&(_expected), "Pointer");
          break;
        }
        case -7: {
          _set = 0x000001F8UL;
          COPYSTRING(&(_expected), "Numeric or SET");
          break;
        }
        case -8: {
          _set = 0x000000FCUL;
          COPYSTRING(&(_expected), "Integer or CHAR");
          break;
        }
        case -9: {
          _set = OEParse_realSet;
          COPYSTRING(&(_expected), "Real");
          break;
        }
        default: NO_LABEL (_temp_, 13797);
      }
    }
    if (!IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 14510)._type, 14516)._form, _set, 14523))  {
      _ErrTx();
    }
  } else if (_form==OTable_strString)  {
    if (!OEParse_String(_n)&&!OEParse_StringConv(_n))  {
      COPYSTRING(&(_expected), "String");
      _ErrTx();
    }
  } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 14704)._type, 14710)._form!=_form)  {
    {
      SHORTINT _temp_ = _form;
      switch (_temp_) {
        case 1 ... 8: {
          COPY(DEREF(OTable_Object, DEREF(OTable_Struct, INDEX(OTable_predeclType, _form, 14, 14797), 14803)._obj, 14808)._name, _expected, 32);
          break;
        }
        case 13: {
          COPYSTRING(&(_expected), "SYSTEM.PTR");
          break;
        }
        case 12: {
          COPYSTRING(&(_expected), "SYSTEM.BYTE");
          break;
        }
        case 14: {
          COPYSTRING(&(_expected), "Pointer");
          break;
        }
        case 15: {
          COPYSTRING(&(_expected), "Procedure");
          break;
        }
        default: NO_LABEL (_temp_, 14736);
      }
    }
    _ErrTx();
  }
}

void OEParse_ChkRange (OEParse_Node _node, LONGINT _lower, LONGINT _upper, INTEGER _num) {
  CHAR _l[16];
  CHAR _u[16];
  if (DEREF(OEParse_Node, _node, 15313)._class==OEParse_ndConst&&(DEREF(OTable_Const, DEREF(OEParse_Node, _node, 15340)._conval, 15348)._intval<_lower||DEREF(OTable_Const, DEREF(OEParse_Node, _node, 15374)._conval, 15382)._intval>_upper))  {
    IntStr_Give(16, (CHAR *) _l, _lower, 1, 0);
    IntStr_Give(16, (CHAR *) _u, _upper, 1, 0);
    OScan_ErrIns2(DEREF(OEParse_Node, _node, 15499)._pos, _num, 16, (CHAR *) _l, 16, (CHAR *) _u);
  }
}

void OEParse_SetInt (OEParse_Node _r) {
  if (DEREF(OEParse_Node, _r, 15684)._class==OEParse_ndConst&&IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _r, 15707)._type, 15713)._form, OEParse_intSet, 15720))  {
    DEREF(OEParse_Node, _r, 15743)._type = INDEX(OTable_predeclType, OEParse_IntConstType(DEREF(OTable_Const, DEREF(OEParse_Node, _r, 15782)._conval, 15790)._intval), 14, 15766);
  }
}

void OEParse_Check (OEParse_Node (* _r)) {
  const SET _illegalNode = 0x00000680UL;

  BOOLEAN _FoldMOp (SHORTINT _form, SHORTINT _subcl, OTable_Const _c) {
    {
      SHORTINT _temp_ = _subcl;
      switch (_temp_) {
        case 7: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _c, 16929)._set = ~DEREF(OTable_Const, _c, 16940)._set;
          } else if (IN(_form, OEParse_intSet, 16966))  {
            DEREF(OTable_Const, _c, 16993)._intval = -DEREF(OTable_Const, _c, 17007)._intval;
          } else {
            DEREF(OTable_Const, _c, 17074)._real = -DEREF(OTable_Const, _c, 17086)._real;
          }
          break;
        }
        case 17: {
          DEREF(OTable_Const, _c, 17129)._intval = 1- DEREF(OTable_Const, _c, 17144)._intval;
          break;
        }
        case 21: {
          if (IN(_form, OEParse_intSet, 17185))  {
            DEREF(OTable_Const, _c, 17212)._intval = ABSLI(DEREF(OTable_Const, _c, 17230)._intval, 17224);
          } else {
            DEREF(OTable_Const, _c, 17287)._real = ABSLR(DEREF(OTable_Const, _c, 17303)._real, 17297);
          }
          break;
        }
        case 14: {
          DEREF(OTable_Const, _c, 17350)._intval = ENTIERLR(DEREF(OTable_Const, _c, 17371)._real);
          DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 17389)._left, 17395)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 17418);
          break;
        }
        case 22: {
          DEREF(OTable_Const, _c, 17457)._intval = (LONGINT) ((INTEGER) CAP((CHAR) DEREF(OTable_Const, _c, 17485)._intval));
          break;
        }
        case 23: {
          DEREF(OTable_Const, _c, 17521)._intval = MODLI(DEREF(OTable_Const, _c, 17534)._intval, 2, 17543);
          DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 17559)._left, 17565)._type = INDEX(OTable_predeclType, OTable_strBool, 14, 17588);
          break;
        }
        default:  {
        return FALSE;
      }
      }
    }
    return TRUE;
    NO_RETURN (16573);
  }

  BOOLEAN _FoldDOp (SHORTINT _form, SHORTINT _subcl, OTable_Const _cl, OTable_Const _cr) {
    {
      SHORTINT _temp_ = _subcl;
      switch (_temp_) {
        case 6: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _cl, 18064)._set = DEREF(OTable_Const, _cl, 18075)._set|DEREF(OTable_Const, _cr, 18083)._set;
          } else if (IN(_form, OEParse_intSet, 18109))  {
            INCLI(DEREF(OTable_Const, _cl, 18142)._intval, DEREF(OTable_Const, _cr, 18154)._intval, 18135);
          } else {
            DEREF(OTable_Const, _cl, 18214)._real = DEREF(OTable_Const, _cl, 18226)._real+DEREF(OTable_Const, _cr, 18235)._real;
          }
          break;
        }
        case 7: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _cl, 18315)._set = SETDIFF(DEREF(OTable_Const, _cl, 18326)._set, DEREF(OTable_Const, _cr, 18334)._set);
          } else if (IN(_form, OEParse_intSet, 18360))  {
            DECLI(DEREF(OTable_Const, _cl, 18393)._intval, DEREF(OTable_Const, _cr, 18405)._intval, 18386);
          } else {
            DEREF(OTable_Const, _cl, 18465)._real = DEREF(OTable_Const, _cl, 18477)._real- DEREF(OTable_Const, _cr, 18486)._real;
          }
          break;
        }
        case 1: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _cl, 18566)._set = DEREF(OTable_Const, _cl, 18577)._set&DEREF(OTable_Const, _cr, 18585)._set;
          } else if (IN(_form, OEParse_intSet, 18611))  {
            DEREF(OTable_Const, _cl, 18639)._intval = DEREF(OTable_Const, _cl, 18653)._intval*DEREF(OTable_Const, _cr, 18664)._intval;
          } else {
            DEREF(OTable_Const, _cl, 18723)._real = DEREF(OTable_Const, _cl, 18735)._real*DEREF(OTable_Const, _cr, 18744)._real;
          }
          break;
        }
        case 3: {
          if (DEREF(OTable_Const, _cr, 18793)._intval!=0)  {
            DEREF(OTable_Const, _cl, 18824)._intval = DIVLI(DEREF(OTable_Const, _cl, 18838)._intval, DEREF(OTable_Const, _cr, 18853)._intval, 18847);
          } else {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 18893)._right, 18900)._pos, 227);
          }
          break;
        }
        case 4: {
          if (DEREF(OTable_Const, _cr, 18977)._intval!=0)  {
            DEREF(OTable_Const, _cl, 19008)._intval = MODLI(DEREF(OTable_Const, _cl, 19022)._intval, DEREF(OTable_Const, _cr, 19037)._intval, 19031);
          } else {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 19077)._right, 19084)._pos, 227);
          }
          break;
        }
        case 2: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _cl, 19192)._set = DEREF(OTable_Const, _cl, 19203)._set^DEREF(OTable_Const, _cr, 19213)._set;
          } else if (DEREF(OTable_Const, _cr, 19236)._real!=0.0000000000000000E+00)  {
            DEREF(OTable_Const, _cl, 19267)._real = DIVLR(DEREF(OTable_Const, _cl, 19279)._real, DEREF(OTable_Const, _cr, 19290)._real, 19286);
          } else {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 19328)._right, 19335)._pos, 227);
          }
          break;
        }
        case 5: {
          DEREF(OTable_Const, _cl, 19408)._intval = DEREF(OTable_Const, _cl, 19422)._intval*DEREF(OTable_Const, _cr, 19433)._intval;
          break;
        }
        case 8: {
          if (DEREF(OTable_Const, _cr, 19470)._intval>DEREF(OTable_Const, _cl, 19483)._intval)  {
            DEREF(OTable_Const, _cl, 19500)._intval = DEREF(OTable_Const, _cr, 19514)._intval;
          }
          break;
        }
        case 15: {
          if ((0<=DEREF(OTable_Const, _cl, 19565)._intval&&DEREF(OTable_Const, _cl, 19580)._intval<=63)&&IN(DEREF(OTable_Const, _cl, 19605)._intval, DEREF(OTable_Const, _cr, 19619)._set, 19614))  {
            DEREF(OTable_Const, _cl, 19643)._intval = 1;
          } else {
            DEREF(OTable_Const, _cl, 19682)._intval = 0;
          }
          DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 19718)._left, 19724)._type = INDEX(OTable_predeclType, OTable_strBool, 14, 19747);
          break;
        }
        case 26: {
          DEREF(OTable_Const, _cl, 19784)._intval = ASHLI(DEREF(OTable_Const, _cl, 19803)._intval, DEREF(OTable_Const, _cr, 19815)._intval, 19796);
          DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 19835)._left, 19841)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 19864);
          break;
        }
        default:  {
        return FALSE;
      }
      }
    }
    return TRUE;
    NO_RETURN (17691);
  }

  OEParse_Node _FoldBool (SHORTINT _subcl, OEParse_Node _left, OEParse_Node _right) {
    if (DEREF(OEParse_Node, _left, 20323)._class==OEParse_ndConst)  {
      if ((_subcl==OEParse_scOr&&DEREF(OTable_Const, DEREF(OEParse_Node, _left, 20380)._conval, 20388)._intval==0)||(_subcl==OEParse_scAnd&&DEREF(OTable_Const, DEREF(OEParse_Node, _left, 20439)._conval, 20447)._intval==1))  {
        return _right;
      } else {
        return _left;
      }
    } else if (DEREF(OEParse_Node, _right, 20554)._class==OEParse_ndConst&&((_subcl==OEParse_scOr&&DEREF(OTable_Const, DEREF(OEParse_Node, _right, 20611)._conval, 20619)._intval==0)||(_subcl==OEParse_scAnd&&DEREF(OTable_Const, DEREF(OEParse_Node, _right, 20673)._conval, 20681)._intval==1)))  {
      return _left;
    } else {
      return (* _r);
    }
    NO_RETURN (19970);
  }

  OTable_Struct _CheckMOp (SHORTINT _subcl, OEParse_Node _left) {
    if (_subcl==OEParse_scMinus)  {
      OEParse_ChkType(_left, OEParse_grpNumOrSet);
      return DEREF(OEParse_Node, _left, 21232)._type;
    } else if (_subcl==OEParse_scNot)  {
      OEParse_ChkType(_left, OTable_strBool);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 21366);
    } else if (_subcl==OEParse_scAbs)  {
      OEParse_ChkType(_left, OEParse_grpNumeric);
      return DEREF(OEParse_Node, _left, 21497)._type;
    } else if (_subcl==OEParse_scEntier)  {
      OEParse_ChkType(_left, OEParse_grpReal);
      return INDEX(OTable_predeclType, OTable_strLongInt, 14, 21640);
    } else if (_subcl==OEParse_scCap)  {
      OEParse_ChkType(_left, OTable_strChar);
      return INDEX(OTable_predeclType, OTable_strChar, 14, 21779);
    } else if (_subcl==OEParse_scOdd)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 21919);
    } else if (_subcl==OEParse_scAdr)  {
      return INDEX(OTable_predeclType, OTable_strLongInt, 14, 22036);
    } else if (_subcl==OEParse_scCc)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      OEParse_ChkRange(_left, 0, -1, 266);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 22215);
    }
    NO_RETURN (20790);
  }

  OTable_Struct _CheckDOp (SHORTINT _subcl, OEParse_Node _left, OEParse_Node _right) {
    SHORTINT _res, _lform, _rform;
    _lform = DEREF(OTable_Struct, DEREF(OEParse_Node, _left, 22653)._type, 22659)._form;
    _rform = DEREF(OTable_Struct, DEREF(OEParse_Node, _right, 22687)._type, 22693)._form;
    _res = _lform;
    if (_rform>_res)  {
      _res = _rform;
    }
    if (!IN(_res, OEParse_numSet, 22862))  {
      _res = OTable_strShortInt;
    }
    if (IN(_subcl, 0x000000C6UL, 22977))  {
      if (IN(_lform, OEParse_numSet, 23080))  {
        OEParse_ChkType(_right, OEParse_grpNumeric);
        if (_subcl==OEParse_scRDiv&&_res<OTable_strReal)  {
          _res = OTable_strReal;
        }
        OEParse_Convert(&(_left), _res);
        OEParse_Convert(&(_right), _res);
      } else if (_lform==OTable_strSet)  {
        OEParse_ChkType(_right, OTable_strSet);
      } else {
        OEParse_ChkType(_left, OEParse_grpNumOrSet);
      }
      return DEREF(OEParse_Node, _left, 23526)._type;
    } else if (_subcl==OEParse_scIDiv||_subcl==OEParse_scMod)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      OEParse_ChkType(_right, OEParse_grpInteger);
      OEParse_Convert(&(_left), _res);
      OEParse_Convert(&(_right), _res);
      return DEREF(OEParse_Node, _left, 23767)._type;
    } else if (_subcl==OEParse_scAnd||_subcl==OEParse_scOr)  {
      OEParse_ChkType(_left, OTable_strBool);
      OEParse_ChkType(_right, OTable_strBool);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 23953);
    } else if (_subcl==OEParse_scIn)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      OEParse_ChkType(_right, OTable_strSet);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 24119);
    } else if (OEParse_scEql<=_subcl&&_subcl<=OEParse_scGeq)  {
      if (IN(_lform, OEParse_numSet, 24216))  {
        OEParse_ChkType(_right, OEParse_grpNumeric);
        OEParse_Convert(&(_left), _res);
        OEParse_Convert(&(_right), _res);
      } else if (_lform==OTable_strChar)  {
        if (OEParse_String(_right))  {
          OEParse_ChkType(_left, OTable_strString);
        } else {
          OEParse_ChkType(_right, OTable_strChar);
        }
      } else if (OEParse_String(_left))  {
        OEParse_ChkType(_right, OTable_strString);
      } else {
        if (_subcl==OEParse_scEql||_subcl==OEParse_scNeq)  {
          if (IN(_lform, 0x00001102UL, 24778))  {
            OEParse_ChkType(_right, _lform);
          } else if (_lform==OTable_strSysPtr)  {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _right, 24926)._type, 24932)._form!=OTable_strNil)  {
              OEParse_ChkType(_right, _lform);
            }
          } else if (_lform==OTable_strPointer)  {
            if (!(((OTable_SameType(DEREF(OEParse_Node, _left, 25092)._type, DEREF(OEParse_Node, _right, 25105)._type)||_rform==OTable_strNil)||OTable_ExtOf(DEREF(OEParse_Node, _left, 25188)._type, DEREF(OEParse_Node, _right, 25201)._type))||OTable_ExtOf(DEREF(OEParse_Node, _right, 25246)._type, DEREF(OEParse_Node, _left, 25258)._type)))  {
              OEParse_ErrT1(DEREF(OEParse_Node, _left, 25299)._pos, 165, &(DEREF(OEParse_Node, _right, 25316)._type));
            }
          } else if (_lform==OTable_strProc)  {
            if (_rform==OTable_strProc)  {
              if (!OTable_ParamsMatch(DEREF(OEParse_Node, _left, 25484)._type, DEREF(OEParse_Node, _right, 25497)._type))  {
                OScan_Err(DEREF(OEParse_Node, _right, 25540)._pos, 247);
              }
            } else if (_rform!=OTable_strNil)  {
              OEParse_ChkType(_right, OTable_strProc);
            }
          } else if (_lform==OTable_strNil)  {
            OEParse_ChkType(_right, OEParse_grpNilComp);
          } else {
            OEParse_ErrNT1(161, _left);
          }
        } else {
          OEParse_ErrNT1(160, _left);
        }
      }
      return INDEX(OTable_predeclType, OTable_strBool, 14, 26038);
    } else if (_subcl==OEParse_scAsh)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      OEParse_ChkType(_right, OEParse_grpInteger);
      return INDEX(OTable_predeclType, OTable_strLongInt, 14, 26200);
    } else if (_subcl==OEParse_scBit)  {
      OEParse_ChkType(_left, OTable_strLongInt);
      OEParse_ChkType(_right, OEParse_grpInteger);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 26367);
    } else if (_subcl==OEParse_scLsh||_subcl==OEParse_scRot)  {
      OEParse_ChkType(_left, OEParse_grpShift);
      OEParse_ChkType(_right, OEParse_grpInteger);
      return DEREF(OEParse_Node, _left, 26550)._type;
    }
    NO_RETURN (22268);
  }

  OTable_Struct _CheckAssign (SHORTINT _subcl, OEParse_Node _left, OEParse_Node _right) {
    if (_subcl==OEParse_scExcl||_subcl==OEParse_scIncl)  {
      OEParse_ChkType(_left, OTable_strSet);
      OEParse_ChkType(_right, OEParse_grpInteger);
    } else if (_subcl==OEParse_scCopy)  {
      OEParse_ChkType(_left, OTable_strString);
      OEParse_ChkType(_right, OTable_strString);
    } else if (_subcl==OEParse_scInc||_subcl==OEParse_scDec)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      if (DEREF(OEParse_Node, _right, 27295)._class==OEParse_ndConst)  {
        OEParse_ChkType(_right, OEParse_grpInteger);
      } else {
        OEParse_ChkType(_right, DEREF(OTable_Struct, DEREF(OEParse_Node, _left, 27398)._type, 27404)._form);
      }
      OEParse_Convert(&(_right), DEREF(OTable_Struct, DEREF(OEParse_Node, _left, 27453)._type, 27459)._form);
    } else if (_subcl==OEParse_scGet||_subcl==OEParse_scPut)  {
      OEParse_ChkType(_left, OTable_strLongInt);
      OEParse_ChkType(_right, OEParse_grpExtStd);
    } else if (_subcl==OEParse_scGetReg||_subcl==OEParse_scPutReg)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      OEParse_ChkRange(_left, 0, -1, 268);
      OEParse_ChkType(_right, OEParse_grpExtStd);
    } else if (_subcl==OEParse_scNewSys)  {
      OEParse_ChkType(_left, OEParse_grpPointer);
      OEParse_ChkType(_right, OEParse_grpInteger);
    }
    return INDEX(OTable_predeclType, OTable_strNone, 14, 28074);
    NO_RETURN (26598);
  }
  if (IN(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28125)._left, 28131)._class, _illegalNode, 28139))  {
    OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28174)._left, 28180)._pos, 246);
  } else if (DEREF(OEParse_Node, (* _r), 28226)._class==OEParse_ndDOp&&IN(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28245)._right, 28252)._class, _illegalNode, 28260))  {
    OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28295)._right, 28302)._pos, 246);
  } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28348)._left, 28354)._type, 28360)._form==OTable_strUndef||(DEREF(OEParse_Node, (* _r), 28385)._class==OEParse_ndDOp&&DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28404)._right, 28411)._type, 28417)._form==OTable_strUndef))  {
    DEREF(OEParse_Node, (* _r), 28449)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 28472);
  } else if (DEREF(OEParse_Node, (* _r), 28497)._class==OEParse_ndMOp)  {
    DEREF(OEParse_Node, (* _r), 28524)._type = _CheckMOp(DEREF(OEParse_Node, (* _r), 28545)._subcl, DEREF(OEParse_Node, (* _r), 28555)._left);
  } else if (DEREF(OEParse_Node, (* _r), 28576)._class==OEParse_ndDOp)  {
    DEREF(OEParse_Node, (* _r), 28603)._type = _CheckDOp(DEREF(OEParse_Node, (* _r), 28624)._subcl, DEREF(OEParse_Node, (* _r), 28634)._left, DEREF(OEParse_Node, (* _r), 28643)._right);
  } else if (DEREF(OEParse_Node, (* _r), 28664)._class==OEParse_ndAssign)  {
    DEREF(OEParse_Node, (* _r), 28694)._type = _CheckAssign(DEREF(OEParse_Node, (* _r), 28718)._subcl, DEREF(OEParse_Node, (* _r), 28728)._left, DEREF(OEParse_Node, (* _r), 28737)._right);
  }
  if ((DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 28791)._type, 28797)._form!=OTable_strUndef&&DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28820)._left, 28826)._class==OEParse_ndConst)&&((DEREF(OEParse_Node, (* _r), 28855)._class==OEParse_ndMOp&&_FoldMOp(DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 28882)._type, 28888)._form, DEREF(OEParse_Node, (* _r), 28897)._subcl, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28907)._left, 28913)._conval))||((DEREF(OEParse_Node, (* _r), 28936)._class==OEParse_ndDOp&&DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28955)._right, 28962)._class==OEParse_ndConst)&&_FoldDOp(DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 28991)._type, 28997)._form, DEREF(OEParse_Node, (* _r), 29006)._subcl, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 29016)._left, 29022)._conval, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 29033)._right, 29040)._conval))))  {
    (* _r) = DEREF(OEParse_Node, (* _r), 29068)._left;
    DEREF(OEParse_Node, (* _r), 29083)._obj = (OTable_Object)NULL;
    if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 29108)._type, 29114)._form, OEParse_intSet, 29121))  {
      OEParse_SetInt((* _r));
    }
  } else if ((DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 29178)._type, 29184)._form==OTable_strBool&&DEREF(OEParse_Node, (* _r), 29208)._class==OEParse_ndDOp)&&IN(DEREF(OEParse_Node, (* _r), 29239)._subcl, 0x00000120UL, 29247))  {
    (* _r) = _FoldBool(DEREF(OEParse_Node, (* _r), 29292)._subcl, DEREF(OEParse_Node, (* _r), 29302)._left, DEREF(OEParse_Node, (* _r), 29311)._right);
  }
}

void OEParse_ChkVar (OEParse_Node _n) {
  {
    SHORTINT _temp_ = DEREF(OEParse_Node, _n, 29638)._class;
    switch (_temp_) {
      case 2:
      case 4: {
        break;
      }
      case 1:
      case 3: {
        if (DEREF(OEParse_Node, _n, 29745)._class==OEParse_ndField)  {
          OEParse_ChkVar(DEREF(OEParse_Node, _n, 29786)._left);
        }
        if (DEREF(OTable_Object, DEREF(OEParse_Node, _n, 29816)._obj, 29821)._mnolev<0&&DEREF(OTable_Object, DEREF(OEParse_Node, _n, 29853)._obj, 29858)._mark!=OTable_exportWrite)  {
          OScan_Err(DEREF(OEParse_Node, _n, 29903)._pos, 249);
        }
        break;
      }
      case 5:
      case 6: {
        OEParse_ChkVar(DEREF(OEParse_Node, _n, 30005)._left);
        break;
      }
      default:  {
      OScan_Err(DEREF(OEParse_Node, _n, 30036)._pos, 249);
    }
    }
  }
}

void OEParse_PredefProc (OEParse_Node (* _call), SHORTINT _id, OEParse_Node _apar, INTEGER _numPar, LONGINT _endOfParams) {
  OEParse_Node _n;
  SET _set;
  LONGINT _min, _max;

  LONGINT _ArrayDim (OTable_Struct _t, BOOLEAN _dyn) {
    LONGINT _i;
    _i = -1;
    while ((void*)_t!=(void*)NULL&&(DEREF(OTable_Struct, _t, 31175)._form==OTable_strDynArray||(DEREF(OTable_Struct, _t, 31202)._form==OTable_strArray&&!_dyn)))  {
      INCLI(_i, 1, 31240);
      _t = DEREF(OTable_Struct, _t, 31263)._base;
    }
    return _i;
    NO_RETURN (30739);
  }

  LONGINT _ArrayLen (OTable_Struct _t, LONGINT _dim) {
    while (_dim!=0)  {
      _t = DEREF(OTable_Struct, _t, 31687)._base;
      DECLI(_dim, 1, 31703);
    }
    if (DEREF(OTable_Struct, _t, 31735)._form==OTable_strDynArray)  {
      return -1;
    } else {
      return DEREF(OTable_Struct, _t, 31807)._len;
    }
    NO_RETURN (31327);
  }

  OEParse_Node _Find (INTEGER _num) {
    OEParse_Node _n;
    _n = _apar;
    while (_num>1)  {
      _n = DEREF(OEParse_Node, _n, 32065)._link;
      DECI(_num, 1, 32081);
    }
    return _n;
    NO_RETURN (31854);
  }

  BOOLEAN _ChkParNum (INTEGER _num) {
    OEParse_Node _n;
    if (_num<_numPar)  {
      _n = _Find(_num);
      OScan_Err(DEREF(OEParse_Node, _n, 32420)._pos, 250);
    } else if (_num>_numPar)  {
      OScan_Err(_endOfParams, 251);
    }
    return _num==_numPar;
    NO_RETURN (32144);
  }

  void _ChkConst (INTEGER _num) {
    OEParse_Node _n;
    _n = _Find(_num);
    if (DEREF(OEParse_Node, _n, 32818)._class!=OEParse_ndConst)  {
      OScan_Err(DEREF(OEParse_Node, _n, 32858)._pos, 203);
    }
  }

  void _ChkTypeVal (INTEGER _num) {
    OEParse_Node _n;
    _n = _Find(_num);
    if (DEREF(OEParse_Node, _n, 33119)._class!=OEParse_ndType)  {
      OScan_Err(DEREF(OEParse_Node, _n, 33156)._pos, 205);
    }
  }

  void _CreateConst (OEParse_Node (* _n), LONGINT _def) {
    (* _n) = OEParse_NewNode(OEParse_ndConst);
    DEREF(OEParse_Node, (* _n), 33384)._conval = OTable_NewConst();
    DEREF(OEParse_Node, (* _n), 33417)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 33440);
    DEREF(OTable_Const, DEREF(OEParse_Node, (* _n), 33463)._conval, 33471)._intval = _def;
    OEParse_SetInt((* _n));
  }
  {
    SHORTINT _temp_ = _id;
    switch (_temp_) {
      case 18:
      case 19: {
        if (_ChkParNum(1))  {
          if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 33637)._type, 33643)._form, OEParse_realSet, 33650))  {
            _CreateConst(_call, 0);
            DEREF(OEParse_Node, (* _call), 33714)._type = DEREF(OEParse_Node, _apar, 33728)._type;
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 33753)._type, 33759)._form==OTable_strReal)  {
              if (_id==OTable_predMAX)  {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 33832)._conval, 33840)._real = MAX_REAL;
              } else {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 33895)._conval, 33903)._real = MIN_REAL;
              }
            } else {
              if (_id==OTable_predMAX)  {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 34005)._conval, 34013)._real = OMachine_maxLReal;
              } else {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 34069)._conval, 34077)._real = OMachine_minLReal;
              }
            }
          } else {
            {
              SHORTINT _temp_ = DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 34160)._type, 34166)._form;
              switch (_temp_) {
                case 1: {
                  _min = 0;
                  _max = 1;
                  break;
                }
                case 2: {
                  _min = 0;
                  _max = 255;
                  break;
                }
                case 3: {
                  _min = -32768;
                  _max = 32767;
                  break;
                }
                case 4: {
                  _min = -2147483648;
                  _max = 2147483647;
                  break;
                }
                case 5: {
                  _min = OMachine_minLInt;
                  _max = OMachine_maxLInt;
                  break;
                }
                case 8: {
                  _min = 0;
                  _max = 63;
                  break;
                }
                default:  {
                OScan_Err(DEREF(OEParse_Node, _apar, 34579)._pos, 254);
                _min = 0;
                _max = 0;
              }
              }
            }
            if (_id==OTable_predMAX)  {
              _CreateConst(_call, _max);
            } else {
              _CreateConst(_call, _min);
            }
            OEParse_SetInt((* _call));
          }
        }
        break;
      }
      case 13:
      case 15:
      case 16:
      case 17: {
        if (_ChkParNum(1))  {
          {
            SHORTINT _temp_ = _id;
            switch (_temp_) {
              case 13: {
                _set = OEParse_intSet;
                break;
              }
              case 15: {
                _set = 0x00000058UL;
                break;
              }
              case 16: {
                _set = 0x00000004UL;
                break;
              }
              case 17: {
                _set = 0x000000B0UL;
                break;
              }
              default: NO_LABEL (_temp_, 34989);
            }
          }
          if (!IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 35302)._type, 35308)._form, _set, 35315))  {
            OEParse_ErrNT1(171, _apar);
            DEREF(OEParse_Node, _apar, 35414)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 35437);
          } else {
            {
              SHORTINT _temp_ = _id;
              switch (_temp_) {
                case 13: {
                  OEParse_Convert(&(_apar), OTable_strChar);
                  break;
                }
                case 15: {
                  OEParse_Convert(&(_apar), DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 35639)._type, 35645)._form+1);
                  break;
                }
                case 16: {
                  OEParse_Convert(&(_apar), OTable_strInteger);
                  break;
                }
                case 17: {
                  OEParse_Convert(&(_apar), DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 35756)._type, 35762)._form- 1);
                  break;
                }
                default: NO_LABEL (_temp_, 35473);
              }
            }
          }
        } else {
          DEREF(OEParse_Node, _apar, 35823)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 35846);
        }
        (* _call) = _apar;
        break;
      }
      case 21:
      case 22:
      case 14:
      case 23:
      case 24:
      case 25: {
        (* _call) = OEParse_NewNode(OEParse_ndMOp);
        if (_ChkParNum(1))  {
          DEREF(OEParse_Node, (* _call), 36047)._left = _apar;
          DEREF(OEParse_Node, (* _call), 36075)._subcl = _id;
          OEParse_Check(_call);
          if (_id==OTable_sysADR&&!IN(DEREF(OEParse_Node, _apar, 36145)._class, 0x0000037EUL, 36153))  {
            OScan_Err(DEREF(OEParse_Node, _apar, 36255)._pos, 255);
          } else if (_id==OTable_sysCC&&DEREF(OEParse_Node, _apar, 36326)._class!=OEParse_ndConst)  {
            OScan_Err(DEREF(OEParse_Node, _apar, 36369)._pos, 203);
          }
          if (_id==OTable_sysADR&&IN(DEREF(OEParse_Node, _apar, 36454)._class, 0x0000030EUL, 36462))  {
            INCL(DEREF(OTable_Object, DEREF(OEParse_Node, _apar, 36536)._obj, 36541)._flags, OTable_flagAddressed, 36526);
          }
        }
        break;
      }
      case 26:
      case 27:
      case 28:
      case 29: {
        (* _call) = OEParse_NewNode(OEParse_ndDOp);
        DEREF(OEParse_Node, (* _call), 36692)._subcl = _id;
        if (_ChkParNum(2))  {
          DEREF(OEParse_Node, (* _call), 36747)._left = _apar;
          DEREF(OEParse_Node, (* _call), 36775)._right = DEREF(OEParse_Node, _apar, 36790)._link;
          OEParse_Check(_call);
        }
        break;
      }
      case 20: {
        (* _call) = OEParse_NewNode(OEParse_ndConst);
        DEREF(OEParse_Node, (* _call), 36890)._conval = OTable_NewConst();
        DEREF(OEParse_Node, (* _call), 36926)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 36949);
        if (_ChkParNum(1))  {
          _ChkTypeVal(1);
          _min = DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 37036)._type, 37042)._size;
          if (_min>=0)  {
            DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 37123)._conval, 37131)._intval = _min;
            OEParse_SetInt((* _call));
          } else if (_min==-1)  {
            DEREF(OEParse_Node, (* _call), 37260)._class = OEParse_ndMOp;
            DEREF(OEParse_Node, (* _call), 37292)._subcl = OEParse_scSize;
            DEREF(OEParse_Node, (* _call), 37325)._left = _apar;
          } else {
            OScan_Err(DEREF(OEParse_Node, _apar, 37414)._pos, 258);
          }
        }
        break;
      }
      case 30: {
        (* _call) = OEParse_NewNode(OEParse_ndConst);
        DEREF(OEParse_Node, (* _call), 37549)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 37572);
        DEREF(OEParse_Node, (* _call), 37598)._conval = OTable_NewConst();
        DEREF(OEParse_Node, (* _call), 37634)._left = _apar;
        if (_numPar==1)  {
          _CreateConst(&(DEREF(OEParse_Node, _apar, 37751)._link), 0);
          INCI(_numPar, 1, 37771);
        }
        if (_ChkParNum(2))  {
          _min = _ArrayDim(DEREF(OEParse_Node, _apar, 37852)._type, FALSE);
          if (_min<0)  {
            OEParse_ErrNT1(151, _apar);
          }
          DEREF(OEParse_Node, (* _call), 37971)._right = DEREF(OEParse_Node, _apar, 37986)._link;
          OEParse_ChkType(DEREF(OEParse_Node, (* _call), 38015)._right, OEParse_grpInteger);
          _ChkConst(2);
          if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38075)._right, 38082)._conval, 38090)._intval<0||DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38110)._right, 38117)._conval, 38125)._intval>_ArrayDim(DEREF(OEParse_Node, _apar, 38148)._type, FALSE))  {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38190)._right, 38197)._pos, 259);
          } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38253)._left, 38259)._type, 38265)._form==OTable_strDynArray&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38328)._left, 38334)._type, 38340)._flags, 38321))  {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38375)._left, 38381)._pos, 153);
          } else if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38456)._right, 38463)._conval, 38471)._intval<=_ArrayDim(DEREF(OEParse_Node, _apar, 38495)._type, TRUE))  {
            DEREF(OEParse_Node, (* _call), 38529)._class = OEParse_ndDOp;
            DEREF(OEParse_Node, (* _call), 38561)._subcl = OEParse_scLen;
          } else {
            DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 38605)._conval, 38613)._intval = _ArrayLen(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38639)._left, 38645)._type, DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38657)._right, 38664)._conval, 38672)._intval);
          }
        }
        break;
      }
      case 31: {
        (* _call) = OEParse_NewNode(OEParse_ndMOp);
        DEREF(OEParse_Node, (* _call), 38761)._subcl = OEParse_scVal;
        if (_ChkParNum(2))  {
          _ChkTypeVal(1);
          DEREF(OEParse_Node, (* _call), 38843)._left = DEREF(OEParse_Node, _apar, 38857)._link;
          DEREF(OEParse_Node, (* _call), 38877)._type = DEREF(OEParse_Node, _apar, 38891)._type;
        }
        break;
      }
      case 32:
      case 33:
      case 36:
      case 34:
      case 35:
      case 38:
      case 39:
      case 40:
      case 41:
      case 42: {
        (* _call) = OEParse_NewNode(OEParse_ndAssign);
        DEREF(OEParse_Node, (* _call), 39078)._subcl = _id;
        if ((_id==OTable_predINC||_id==OTable_predDEC)&&_numPar==1)  {
          _CreateConst(&(DEREF(OEParse_Node, _apar, 39238)._link), 1);
          INCI(_numPar, 1, 39258);
        }
        if (_ChkParNum(2))  {
          DEREF(OEParse_Node, (* _call), 39322)._left = _apar;
          DEREF(OEParse_Node, (* _call), 39350)._right = DEREF(OEParse_Node, _apar, 39365)._link;
          if ((_id!=OTable_sysPUT&&_id!=OTable_sysGET)&&_id!=OTable_sysPUTREG)  {
            if (_id==OTable_predCOPY)  {
              OEParse_ChkVar(_Find(2));
            } else {
              OEParse_ChkVar(_Find(1));
            }
          }
          if (_id==OTable_sysPUTREG||_id==OTable_sysGETREG)  {
            _ChkConst(1);
          }
          OEParse_Check(_call);
        }
        break;
      }
      case 43: {
        (* _call) = OEParse_NewNode(OEParse_ndAssign);
        DEREF(OEParse_Node, (* _call), 39755)._subcl = OEParse_scNewFix;
        DEREF(OEParse_Node, (* _call), 39786)._left = _apar;
        if ((void*)_apar!=(void*)NULL)  {
          OEParse_ChkType(_apar, OTable_strPointer);
          if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 39901)._type, 39907)._flags, 39894)||((void*)DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 39935)._type, 39941)._base!=(void*)NULL&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 39980)._type, 39986)._base, 39992)._flags, 39973)))  {
            OScan_Err(DEREF(OEParse_Node, _apar, 40027)._pos, 154);
          }
        }
        if (_ChkParNum((INTEGER) (_ArrayDim(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 40143)._type, 40149)._base, TRUE)+2)))  {
          OEParse_ChkType(_apar, OTable_strPointer);
          OEParse_ChkVar(_Find(1));
          if (_numPar>1)  {
            DEREF(OEParse_Node, (* _call), 40280)._subcl = OEParse_scNewDyn;
            DEREF(OEParse_Node, (* _call), 40315)._right = DEREF(OEParse_Node, _apar, 40330)._link;
            _n = DEREF(OEParse_Node, (* _call), 40357)._right;
            while ((void*)_n!=(void*)NULL)  {
              OEParse_ChkType(_n, OEParse_grpInteger);
              _n = DEREF(OEParse_Node, _n, 40450)._link;
            }
          }
        }
        break;
      }
      case 47: {
        (* _call) = OEParse_NewNode(OEParse_ndAssign);
        DEREF(OEParse_Node, (* _call), 40557)._subcl = OEParse_scDispose;
        DEREF(OEParse_Node, (* _call), 40589)._left = _apar;
        if (_ChkParNum(1))  {
          OEParse_ChkType(_apar, OEParse_grpPointer);
          if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 40731)._type, 40737)._flags, 40724)||((void*)DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 40765)._type, 40771)._base!=(void*)NULL&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 40810)._type, 40816)._base, 40822)._flags, 40803)))  {
            OScan_Err(DEREF(OEParse_Node, _apar, 40857)._pos, 155);
          }
        }
        break;
      }
      case 48: {
        (* _call) = OEParse_NewNode(OEParse_ndAssign);
        DEREF(OEParse_Node, (* _call), 40999)._subcl = OEParse_scCollect;
        if (_ChkParNum(0))  {
        }
        break;
      }
      case 45: {
        (* _call) = OEParse_NewNode(OEParse_ndTrap);
        if (_ChkParNum(1))  {
          DEREF(OEParse_Node, (* _call), 41149)._left = _apar;
          _ChkConst(1);
          OEParse_ChkType(_apar, OEParse_grpInteger);
          OEParse_ChkRange(_apar, 0, 255, 267);
        }
        break;
      }
      case 46: {
        (* _call) = OEParse_NewNode(OEParse_ndAssert);
        if (_numPar==1)  {
          _CreateConst(&(DEREF(OEParse_Node, _apar, 41423)._link), 1);
          INCI(_numPar, 1, 41457);
        }
        if (_ChkParNum(2))  {
          DEREF(OEParse_Node, (* _call), 41521)._right = _apar;
          OEParse_ChkType(_apar, OTable_strBool);
          DEREF(OEParse_Node, (* _call), 41585)._left = DEREF(OEParse_Node, _apar, 41599)._link;
          _ChkConst(2);
          OEParse_ChkType(DEREF(OEParse_Node, (* _call), 41650)._left, OEParse_grpInteger);
          OEParse_ChkRange(DEREF(OEParse_Node, (* _call), 41693)._left, 0, 255, 267);
        }
        break;
      }
      case 37: {
        (* _call) = OEParse_NewNode(OEParse_ndAssign);
        DEREF(OEParse_Node, (* _call), 41832)._subcl = OEParse_scMove;
        DEREF(OEParse_Node, (* _call), 41861)._right = _apar;
        if (_ChkParNum(3))  {
          OEParse_ChkType(_apar, OTable_strLongInt);
          OEParse_ChkType(DEREF(OEParse_Node, _apar, 41965)._link, OTable_strLongInt);
          OEParse_ChkType(DEREF(OEParse_Node, DEREF(OEParse_Node, _apar, 42009)._link, 42015)._link, OEParse_grpInteger);
        }
        break;
      }
      default: NO_LABEL (_temp_, 33539);
    }
  }
}

void OEParse_RecycleMem (OEParse_Node (* _node)) {
  if ((void*)(* _node)!=(void*)NULL&&DEREF(OEParse_Node, (* _node), 42149)._class!=OEParse_ndCollected)  {
    DEREF(OEParse_Node, (* _node), 42187)._class = OEParse_ndCollected;
    OEParse_RecycleMem(&(DEREF(OEParse_Node, (* _node), 42233)._left));
    OEParse_RecycleMem(&(DEREF(OEParse_Node, (* _node), 42264)._right));
    OEParse_RecycleMem(&(DEREF(OEParse_Node, (* _node), 42296)._link));
    DEREF(OEParse_Node, (* _node), 42315)._link = OEParse_nodeHeap;
    OEParse_nodeHeap = (* _node);
    (* _node) = (OEParse_Node)NULL;
  }
}

void _init_OEParse (void) {
  moduleId = add_module ("OEParse");
  td_OEParse_NodeDesc = create_td("NodeDesc", sizeof(OEParse_NodeDesc), sizeof(TD_OEParse_NodeDesc), NULL, 0);
  OEParse_nodeHeap = (OEParse_Node)NULL;
}
