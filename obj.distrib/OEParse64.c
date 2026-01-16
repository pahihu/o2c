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
const SET OEParse_intSet = 0x00000038U;
const SET OEParse_realSet = 0x000000C0U;
const SET OEParse_numSet = 0x000000F8U;
static const SET OEParse_nilCompSet = 0x0000E000U;
static const SET OEParse_arraySet = 0x00060000U;
static OEParse_Node OEParse_nodeHeap;



OEParse_Node OEParse_NewNode (SHORTINT _class) {
  OEParse_Node _n;
  if ((void*)OEParse_nodeHeap!=(void*)NULL)  {
    _n = OEParse_nodeHeap;
    OEParse_nodeHeap = DEREF(OEParse_Node, OEParse_nodeHeap, 3398)._link;
  } else {
    NEWREC(_n, td_OEParse_NodeDesc, 0);
  }
  DEREF(OEParse_Node, _n, 3442)._left = (OEParse_Node)NULL;
  DEREF(OEParse_Node, _n, 3458)._right = (OEParse_Node)NULL;
  DEREF(OEParse_Node, _n, 3475)._link = (OEParse_Node)NULL;
  DEREF(OEParse_Node, _n, 3495)._class = _class;
  DEREF(OEParse_Node, _n, 3514)._subcl = 0;
  DEREF(OEParse_Node, _n, 3529)._pos = OScan_lastSym;
  DEREF(OEParse_Node, _n, 3554)._obj = (OTable_Object)NULL;
  DEREF(OEParse_Node, _n, 3569)._conval = (OTable_Const)NULL;
  if (_class>=OEParse_ndInitTd)  {
    DEREF(OEParse_Node, _n, 3625)._type = INDEX(OTable_predeclType, OTable_strNone, 14, 3648);
  } else {
    DEREF(OEParse_Node, _n, 3676)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 3699);
  }
  return _n;
  NO_RETURN (3106);
}

static void OEParse_TypeName (OTable_Struct _str, LONGINT __descr_0, CHAR (* _descr)) {
  CHAR _elemDescr[256];
  if ((void*)DEREF(OTable_Struct, _str, 3946)._obj!=(void*)NULL)  {
    if (DEREF(OTable_Object, DEREF(OTable_Struct, _str, 3994)._obj, 3999)._mnolev<-1)  {
      OTable_GetModuleName(DEREF(OTable_Struct, _str, 4060)._obj, __descr_0, _descr);
      Strings2_AppendChar('.', __descr_0, _descr);
    } else {
      COPY("", _descr, __descr_0);
    }
    Strings_Append(48, (CHAR *) DEREF(OTable_Object, DEREF(OTable_Struct, _str, 4184)._obj, 4189)._name, __descr_0, _descr);
  } else {
    {
      SHORTINT _temp_ = DEREF(OTable_Struct, _str, 4227)._form;
      switch (_temp_) {
        case 17:
        case 18: {
          if (DEREF(OTable_Struct, _str, 4292)._form==OTable_strArray)  {
            COPY("ARRAY ", _descr, __descr_0);
            IntStr_Append(DEREF(OTable_Struct, _str, 4384)._len, 0, ConvTypes_left, __descr_0, _descr);
            Strings_Append(5, (CHAR *) " OF ", __descr_0, _descr);
          } else {
            COPY("ARRAY OF ", _descr, __descr_0);
          }
          OEParse_TypeName(DEREF(OTable_Struct, _str, 4543)._base, 256, (CHAR *) _elemDescr);
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
        default: NO_LABEL (_temp_, 4219);
      }
    }
  }
}

void OEParse_ErrT1 (LONGINT _pos, INTEGER _num, OTable_Struct (* _t1)) {
  CHAR _ins[128];
  if (DEREF(OTable_Struct, (* _t1), 5301)._form!=OTable_strUndef)  {
    OEParse_TypeName((* _t1), 128, (CHAR *) _ins);
    OScan_ErrIns(_pos, _num, 128, (CHAR *) _ins);
    (* _t1) = INDEX(OTable_predeclType, OTable_strUndef, 14, 5410);
  }
}

void OEParse_ErrNT1 (INTEGER _num, OEParse_Node _n1) {
  OEParse_ErrT1(DEREF(OEParse_Node, _n1, 5597)._pos, _num, &(DEREF(OEParse_Node, _n1, 5611)._type));
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
  NO_RETURN (5772);
}

static void OEParse_Convert (OEParse_Node (* _r), SHORTINT _to) {
  OEParse_Node _n;
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 6561)._type, 6567)._form!=_to&&IN(_to, 0x000000FCU, 6586))  {
    if (DEREF(OEParse_Node, (* _r), 6625)._class==OEParse_ndConst)  {
      if ((IN(DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 6687)._type, 6693)._form, OEParse_intSet, 6700)&&_to!=OTable_strChar)&&OEParse_IntConstType(DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 6760)._conval, 6768)._intval)>_to)  {
        OEParse_ErrNT1(256, (* _r));
      } else if (_to==OTable_strChar&&(DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 6905)._conval, 6913)._intval<0||DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 6938)._conval, 6946)._intval>255))  {
        OEParse_Err(256);
      } else if ((_to==OTable_strShortInt&&DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 7070)._type, 7076)._form==OTable_strChar)&&DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7100)._conval, 7108)._intval>127)  {
        OEParse_Err(256);
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 7222)._type, 7228)._form>=OTable_strReal&&_to<OTable_strReal)  {
        DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7284)._conval, 7292)._intval = (LONGINT) DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7313)._conval, 7321)._real;
      } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 7345)._type, 7351)._form<OTable_strReal&&_to>=OTable_strReal)  {
        DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7407)._conval, 7415)._real = (LONGREAL) DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 7426)._conval, 7434)._intval;
      }
      DEREF(OEParse_Node, (* _r), 7465)._obj = (OTable_Object)NULL;
    } else if (DEREF(OEParse_Node, (* _r), 7492)._subcl!=OEParse_scConv||DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 7515)._type, 7521)._form<_to)  {
      _n = OEParse_NewNode(OEParse_ndMOp);
      ASSERT_TYPE(TAG(TD_OEParse_NodeDesc, _n, 7764), td_OEParse_NodeDesc, 7764);
      DEREF(OEParse_Node, _n, 7764) = DEREF(OEParse_Node, (* _r), 7770);
      DEREF(OEParse_Node, (* _r), 7782)._class = OEParse_ndMOp;
      DEREF(OEParse_Node, (* _r), 7809)._subcl = OEParse_scConv;
      DEREF(OEParse_Node, (* _r), 7837)._left = _n;
      DEREF(OEParse_Node, (* _r), 7859)._right = (OEParse_Node)NULL;
      DEREF(OEParse_Node, (* _r), 7884)._obj = (OTable_Object)NULL;
      DEREF(OEParse_Node, (* _r), 7907)._conval = (OTable_Const)NULL;
    }
    DEREF(OEParse_Node, (* _r), 7961)._type = INDEX(OTable_predeclType, _to, 14, 7984);
  }
}

static BOOLEAN OEParse_StringConv (OEParse_Node _node) {
  if (DEREF(OTable_Struct, DEREF(OEParse_Node, _node, 8255)._type, 8261)._form==OTable_strString)  {
    return TRUE;
  } else if (DEREF(OEParse_Node, _node, 8319)._class==OEParse_ndConst&&DEREF(OTable_Struct, DEREF(OEParse_Node, _node, 8343)._type, 8349)._form==OTable_strChar)  {
    DEREF(OEParse_Node, _node, 8382)._type = INDEX(OTable_predeclType, OTable_strString, 14, 8405);
    DEREF(OEParse_Node, _node, 8430)._obj = (OTable_Object)NULL;
    NEWFIX(DEREF(OTable_Const, DEREF(OEParse_Node, _node, 8459)._conval, 8467)._string, 1);
    INDEX(DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _node, 8488)._conval, 8496)._string, 8504), 0, 256, 8504) = (CHAR) DEREF(OTable_Const, DEREF(OEParse_Node, _node, 8520)._conval, 8528)._intval;
    INDEX(DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _node, 8549)._conval, 8557)._string, 8565), 1, 256, 8565) = '\000';
    return TRUE;
  } else {
    return FALSE;
  }
  NO_RETURN (8023);
}

BOOLEAN OEParse_ArrayComp (OEParse_Node _a, OTable_Struct _f) {

  BOOLEAN _ArrayCompRec (OTable_Struct _a, OTable_Struct _f) {
    return OTable_SameType(_a, _f)||((DEREF(OTable_Struct, _f, 8888)._form==OTable_strDynArray&&IN(DEREF(OTable_Struct, _a, 8914)._form, OEParse_arraySet, 8921))&&_ArrayCompRec(DEREF(OTable_Struct, _a, 8951)._base, DEREF(OTable_Struct, _f, 8960)._base));
    NO_RETURN (8792);
  }
  return ((DEREF(OTable_Struct, _f, 9012)._form==OTable_strDynArray&&DEREF(OTable_Struct, DEREF(OTable_Struct, _f, 9038)._base, 9044)._form==OTable_strChar)&&OEParse_StringConv(_a))||_ArrayCompRec(DEREF(OEParse_Node, _a, 9097)._type, _f);
  NO_RETURN (8659);
}

BOOLEAN OEParse_AssignComp (OTable_Struct _var, OEParse_Node _expr) {
  SHORTINT _tv, _te;
  _tv = DEREF(OTable_Struct, _var, 9593)._form;
  _te = DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 9611)._type, 9617)._form;
  if (((((((OTable_SameType(_var, DEREF(OEParse_Node, _expr, 9653)._type)||OTable_ExtOf(DEREF(OEParse_Node, _expr, 9685)._type, _var))||(IN(_tv, OEParse_nilCompSet, 9713)&&_te==OTable_strNil))||(((_tv==OTable_strArray&&DEREF(OTable_Struct, DEREF(OTable_Struct, _var, 9777)._base, 9783)._form==OTable_strChar)&&_te==OTable_strString)&&(LONGINT) Strings_Length(256, (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, _expr, 9849)._conval, 9857)._string, 9865))<DEREF(OTable_Struct, _var, 9871)._len))||((((_tv==OTable_strArray&&DEREF(OTable_Struct, DEREF(OTable_Struct, _var, 9911)._base, 9917)._form==OTable_strChar)&&DEREF(OTable_Struct, _var, 9951)._len>1)&&_te==OTable_strChar)&&OEParse_StringConv(_expr)))||((((_tv==OTable_strProc&&DEREF(OEParse_Node, _expr, 10032)._class==OEParse_ndProc)&&DEREF(OTable_Object, DEREF(OEParse_Node, _expr, 10065)._obj, 10070)._mnolev<=0)&&OTable_ParamsMatch(_var, DEREF(OEParse_Node, _expr, 10123)._type))&&(DEREF(OTable_Object, DEREF(OEParse_Node, _expr, 10149)._obj, 10154)._mode==OTable_objExtProc||DEREF(OTable_Object, DEREF(OEParse_Node, _expr, 10183)._obj, 10188)._mode==OTable_objLocalProc)))||(_tv==OTable_strSysByte&&(_te==OTable_strChar||_te==OTable_strShortInt)))||(_tv==OTable_strSysPtr&&_te==OTable_strPointer))  {
    return TRUE;
  } else if ((IN(_te, OEParse_numSet, 10367)&&IN(_tv, OEParse_numSet, 10384))&&_tv>_te)  {
    OEParse_Convert(&(_expr), _tv);
    return TRUE;
  } else if ((IN(_te, OEParse_intSet, 10555)&&_tv==OTable_strPointer)&&IN(OTable_flagExternal, DEREF(OTable_Struct, _var, 10612)._flags, 10606))  {
    OEParse_Convert(&(_expr), OTable_strLongInt);
    return TRUE;
  } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _expr, 10760)._type, 10766)._form==OTable_strUndef)  {
    return TRUE;
  } else {
    OEParse_ErrT1(DEREF(OEParse_Node, _expr, 10912)._pos, 166, &(_var));
    return FALSE;
  }
  NO_RETURN (9136);
}

static BOOLEAN OEParse_String (OEParse_Node _n) {
  return DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 11111)._type, 11117)._form==OTable_strString||(IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 11144)._type, 11150)._form, OEParse_arraySet, 11157)&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 11174)._type, 11180)._base, 11186)._form==OTable_strChar);
  NO_RETURN (10986);
}

BOOLEAN OEParse_ChkGuard (OEParse_Node _var, OTable_Object _typeObj, BOOLEAN _chkQual, LONGINT _tpos) {
  LONGINT _pos;
  _pos = OScan_lastErr;
  if (DEREF(OTable_Object, _typeObj, 11852)._mode!=OTable_objType)  {
    OScan_Err(_tpos, 205);
  } else {
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 11943)._type, 11949)._form==OTable_strPointer)  {
      if ((_chkQual&&DEREF(OEParse_Node, _var, 12002)._class!=OEParse_ndVar)&&DEREF(OEParse_Node, _var, 12025)._class!=OEParse_ndVarPar)  {
        OScan_Err(DEREF(OEParse_Node, _var, 12070)._pos, 244);
      } else if (DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 12147)._type, 12153)._base, 12159)._form!=OTable_strRecord)  {
        OScan_Err(DEREF(OEParse_Node, _var, 12206)._pos, 237);
      } else if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 12289)._type, 12295)._base, 12301)._flags, 12283))  {
        OScan_Err(DEREF(OEParse_Node, _var, 12335)._pos, 164);
      }
    } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 12441)._type, 12447)._form==OTable_strRecord)  {
      if (DEREF(OEParse_Node, _var, 12489)._class!=OEParse_ndVarPar)  {
        OScan_Err(DEREF(OEParse_Node, _var, 12534)._pos, 236);
      } else if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _var, 12620)._type, 12626)._flags, 12614))  {
        OScan_Err(DEREF(OEParse_Node, _var, 12660)._pos, 164);
      }
    } else {
      OEParse_ErrT1(DEREF(OEParse_Node, _var, 12779)._pos, 164, &(DEREF(OEParse_Node, _var, 12794)._type));
    }
    if (!OTable_ExtOf(DEREF(OTable_Object, _typeObj, 12866)._type, DEREF(OEParse_Node, _var, 12877)._type))  {
      OEParse_ErrT1(_tpos, 163, &(DEREF(OEParse_Node, _var, 12919)._type));
    }
  }
  return _pos==OScan_lastErr;
  NO_RETURN (11231);
}

void OEParse_ChkType (OEParse_Node _n, SHORTINT _form) {
  SET _set;
  CHAR _expected[32];

  void _ErrTx (void) {
    CHAR _ins[128];
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 13529)._type, 13535)._form!=OTable_strUndef)  {
      OEParse_TypeName(DEREF(OEParse_Node, _n, 13580)._type, 128, (CHAR *) _ins);
      OScan_ErrIns2(DEREF(OEParse_Node, _n, 13614)._pos, 199, 32, (CHAR *) _expected, 128, (CHAR *) _ins);
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
          _set = 0x0000103CU;
          COPYSTRING(&(_expected), "Integer, Char, or Byte");
          break;
        }
        case -5: {
          _set = 0x0000E1FEU;
          COPYSTRING(&(_expected), "Unstructured");
          break;
        }
        case -6: {
          _set = 0x00006000U;
          COPYSTRING(&(_expected), "Pointer");
          break;
        }
        case -7: {
          _set = 0x000001F8U;
          COPYSTRING(&(_expected), "Numeric or SET");
          break;
        }
        case -8: {
          _set = 0x000000FCU;
          COPYSTRING(&(_expected), "Integer or CHAR");
          break;
        }
        default: NO_LABEL (_temp_, 13704);
      }
    }
    if (!IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 14365)._type, 14371)._form, _set, 14378))  {
      _ErrTx();
    }
  } else if (_form==OTable_strString)  {
    if (!OEParse_String(_n)&&!OEParse_StringConv(_n))  {
      COPYSTRING(&(_expected), "String");
      _ErrTx();
    }
  } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 14559)._type, 14565)._form!=_form)  {
    {
      SHORTINT _temp_ = _form;
      switch (_temp_) {
        case 1 ... 8: {
          COPY(DEREF(OTable_Object, DEREF(OTable_Struct, INDEX(OTable_predeclType, _form, 14, 14652), 14658)._obj, 14663)._name, _expected, 32);
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
        default: NO_LABEL (_temp_, 14591);
      }
    }
    _ErrTx();
  }
}

void OEParse_ChkRange (OEParse_Node _node, LONGINT _lower, LONGINT _upper, INTEGER _num) {
  CHAR _l[16];
  CHAR _u[16];
  if (DEREF(OEParse_Node, _node, 15168)._class==OEParse_ndConst&&(DEREF(OTable_Const, DEREF(OEParse_Node, _node, 15195)._conval, 15203)._intval<_lower||DEREF(OTable_Const, DEREF(OEParse_Node, _node, 15229)._conval, 15237)._intval>_upper))  {
    IntStr_Give(16, (CHAR *) _l, _lower, 1, 0);
    IntStr_Give(16, (CHAR *) _u, _upper, 1, 0);
    OScan_ErrIns2(DEREF(OEParse_Node, _node, 15354)._pos, _num, 16, (CHAR *) _l, 16, (CHAR *) _u);
  }
}

void OEParse_SetInt (OEParse_Node _r) {
  if (DEREF(OEParse_Node, _r, 15539)._class==OEParse_ndConst&&IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _r, 15562)._type, 15568)._form, OEParse_intSet, 15575))  {
    DEREF(OEParse_Node, _r, 15598)._type = INDEX(OTable_predeclType, OEParse_IntConstType(DEREF(OTable_Const, DEREF(OEParse_Node, _r, 15637)._conval, 15645)._intval), 14, 15621);
  }
}

void OEParse_Check (OEParse_Node (* _r)) {
  const SET _illegalNode = 0x00000680U;

  BOOLEAN _FoldMOp (SHORTINT _form, SHORTINT _subcl, OTable_Const _c) {
    {
      SHORTINT _temp_ = _subcl;
      switch (_temp_) {
        case 7: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _c, 16784)._set = ~DEREF(OTable_Const, _c, 16795)._set;
          } else if (IN(_form, OEParse_intSet, 16821))  {
            DEREF(OTable_Const, _c, 16848)._intval = -DEREF(OTable_Const, _c, 16862)._intval;
          } else {
            DEREF(OTable_Const, _c, 16929)._real = -DEREF(OTable_Const, _c, 16941)._real;
          }
          break;
        }
        case 17: {
          DEREF(OTable_Const, _c, 16984)._intval = 1- DEREF(OTable_Const, _c, 16999)._intval;
          break;
        }
        case 21: {
          if (IN(_form, OEParse_intSet, 17040))  {
            DEREF(OTable_Const, _c, 17067)._intval = ABSLI(DEREF(OTable_Const, _c, 17085)._intval, 17079);
          } else {
            DEREF(OTable_Const, _c, 17142)._real = ABSLR(DEREF(OTable_Const, _c, 17158)._real, 17152);
          }
          break;
        }
        case 22: {
          DEREF(OTable_Const, _c, 17202)._intval = (LONGINT) ((INTEGER) CAP((CHAR) DEREF(OTable_Const, _c, 17230)._intval));
          break;
        }
        case 23: {
          DEREF(OTable_Const, _c, 17266)._intval = MODLI(DEREF(OTable_Const, _c, 17279)._intval, 2, 17288);
          DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 17304)._left, 17310)._type = INDEX(OTable_predeclType, OTable_strBool, 14, 17333);
          break;
        }
        default:  {
        return FALSE;
      }
      }
    }
    return TRUE;
    NO_RETURN (16428);
  }

  BOOLEAN _FoldDOp (SHORTINT _form, SHORTINT _subcl, OTable_Const _cl, OTable_Const _cr) {
    {
      SHORTINT _temp_ = _subcl;
      switch (_temp_) {
        case 6: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _cl, 17809)._set = DEREF(OTable_Const, _cl, 17820)._set|DEREF(OTable_Const, _cr, 17828)._set;
          } else if (IN(_form, OEParse_intSet, 17854))  {
            INCLI(DEREF(OTable_Const, _cl, 17887)._intval, DEREF(OTable_Const, _cr, 17899)._intval, 17880);
          } else {
            DEREF(OTable_Const, _cl, 17959)._real = DEREF(OTable_Const, _cl, 17971)._real+DEREF(OTable_Const, _cr, 17980)._real;
          }
          break;
        }
        case 7: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _cl, 18060)._set = SETDIFF(DEREF(OTable_Const, _cl, 18071)._set, DEREF(OTable_Const, _cr, 18079)._set);
          } else if (IN(_form, OEParse_intSet, 18105))  {
            DECLI(DEREF(OTable_Const, _cl, 18138)._intval, DEREF(OTable_Const, _cr, 18150)._intval, 18131);
          } else {
            DEREF(OTable_Const, _cl, 18210)._real = DEREF(OTable_Const, _cl, 18222)._real- DEREF(OTable_Const, _cr, 18231)._real;
          }
          break;
        }
        case 1: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _cl, 18311)._set = DEREF(OTable_Const, _cl, 18322)._set&DEREF(OTable_Const, _cr, 18330)._set;
          } else if (IN(_form, OEParse_intSet, 18356))  {
            DEREF(OTable_Const, _cl, 18384)._intval = DEREF(OTable_Const, _cl, 18398)._intval*DEREF(OTable_Const, _cr, 18409)._intval;
          } else {
            DEREF(OTable_Const, _cl, 18468)._real = DEREF(OTable_Const, _cl, 18480)._real*DEREF(OTable_Const, _cr, 18489)._real;
          }
          break;
        }
        case 3: {
          if (DEREF(OTable_Const, _cr, 18538)._intval!=0)  {
            DEREF(OTable_Const, _cl, 18569)._intval = DIVLI(DEREF(OTable_Const, _cl, 18583)._intval, DEREF(OTable_Const, _cr, 18598)._intval, 18592);
          } else {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 18638)._right, 18645)._pos, 227);
          }
          break;
        }
        case 4: {
          if (DEREF(OTable_Const, _cr, 18722)._intval!=0)  {
            DEREF(OTable_Const, _cl, 18753)._intval = MODLI(DEREF(OTable_Const, _cl, 18767)._intval, DEREF(OTable_Const, _cr, 18782)._intval, 18776);
          } else {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 18822)._right, 18829)._pos, 227);
          }
          break;
        }
        case 2: {
          if (_form==OTable_strSet)  {
            DEREF(OTable_Const, _cl, 18937)._set = DEREF(OTable_Const, _cl, 18948)._set^DEREF(OTable_Const, _cr, 18958)._set;
          } else if (DEREF(OTable_Const, _cr, 18981)._real!=0.0000000000000000E+00)  {
            DEREF(OTable_Const, _cl, 19012)._real = DIVLR(DEREF(OTable_Const, _cl, 19024)._real, DEREF(OTable_Const, _cr, 19035)._real, 19031);
          } else {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 19073)._right, 19080)._pos, 227);
          }
          break;
        }
        case 5: {
          DEREF(OTable_Const, _cl, 19153)._intval = DEREF(OTable_Const, _cl, 19167)._intval*DEREF(OTable_Const, _cr, 19178)._intval;
          break;
        }
        case 8: {
          if (DEREF(OTable_Const, _cr, 19215)._intval>DEREF(OTable_Const, _cl, 19228)._intval)  {
            DEREF(OTable_Const, _cl, 19245)._intval = DEREF(OTable_Const, _cr, 19259)._intval;
          }
          break;
        }
        case 15: {
          if ((0<=DEREF(OTable_Const, _cl, 19310)._intval&&DEREF(OTable_Const, _cl, 19325)._intval<=31)&&IN(DEREF(OTable_Const, _cl, 19350)._intval, DEREF(OTable_Const, _cr, 19364)._set, 19359))  {
            DEREF(OTable_Const, _cl, 19388)._intval = 1;
          } else {
            DEREF(OTable_Const, _cl, 19427)._intval = 0;
          }
          DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 19463)._left, 19469)._type = INDEX(OTable_predeclType, OTable_strBool, 14, 19492);
          break;
        }
        case 26: {
          DEREF(OTable_Const, _cl, 19529)._intval = ASHLI(DEREF(OTable_Const, _cl, 19548)._intval, DEREF(OTable_Const, _cr, 19560)._intval, 19541);
          DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 19580)._left, 19586)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 19609);
          break;
        }
        default:  {
        return FALSE;
      }
      }
    }
    return TRUE;
    NO_RETURN (17436);
  }

  OEParse_Node _FoldBool (SHORTINT _subcl, OEParse_Node _left, OEParse_Node _right) {
    if (DEREF(OEParse_Node, _left, 20068)._class==OEParse_ndConst)  {
      if ((_subcl==OEParse_scOr&&DEREF(OTable_Const, DEREF(OEParse_Node, _left, 20125)._conval, 20133)._intval==0)||(_subcl==OEParse_scAnd&&DEREF(OTable_Const, DEREF(OEParse_Node, _left, 20184)._conval, 20192)._intval==1))  {
        return _right;
      } else {
        return _left;
      }
    } else if (DEREF(OEParse_Node, _right, 20299)._class==OEParse_ndConst&&((_subcl==OEParse_scOr&&DEREF(OTable_Const, DEREF(OEParse_Node, _right, 20356)._conval, 20364)._intval==0)||(_subcl==OEParse_scAnd&&DEREF(OTable_Const, DEREF(OEParse_Node, _right, 20418)._conval, 20426)._intval==1)))  {
      return _left;
    } else {
      return (* _r);
    }
    NO_RETURN (19715);
  }

  OTable_Struct _CheckMOp (SHORTINT _subcl, OEParse_Node _left) {
    if (_subcl==OEParse_scMinus)  {
      OEParse_ChkType(_left, OEParse_grpNumOrSet);
      return DEREF(OEParse_Node, _left, 20977)._type;
    } else if (_subcl==OEParse_scNot)  {
      OEParse_ChkType(_left, OTable_strBool);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 21111);
    } else if (_subcl==OEParse_scAbs)  {
      OEParse_ChkType(_left, OEParse_grpNumeric);
      return DEREF(OEParse_Node, _left, 21242)._type;
    } else if (_subcl==OEParse_scCap)  {
      OEParse_ChkType(_left, OTable_strChar);
      return INDEX(OTable_predeclType, OTable_strChar, 14, 21373);
    } else if (_subcl==OEParse_scOdd)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 21513);
    } else if (_subcl==OEParse_scAdr)  {
      return INDEX(OTable_predeclType, OTable_strLongInt, 14, 21630);
    } else if (_subcl==OEParse_scCc)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      OEParse_ChkRange(_left, 0, -1, 266);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 21809);
    }
    NO_RETURN (20535);
  }

  OTable_Struct _CheckDOp (SHORTINT _subcl, OEParse_Node _left, OEParse_Node _right) {
    SHORTINT _res, _lform, _rform;
    _lform = DEREF(OTable_Struct, DEREF(OEParse_Node, _left, 22247)._type, 22253)._form;
    _rform = DEREF(OTable_Struct, DEREF(OEParse_Node, _right, 22281)._type, 22287)._form;
    _res = _lform;
    if (_rform>_res)  {
      _res = _rform;
    }
    if (!IN(_res, OEParse_numSet, 22456))  {
      _res = OTable_strShortInt;
    }
    if (IN(_subcl, 0x000000C6U, 22571))  {
      if (IN(_lform, OEParse_numSet, 22674))  {
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
      return DEREF(OEParse_Node, _left, 23120)._type;
    } else if (_subcl==OEParse_scIDiv||_subcl==OEParse_scMod)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      OEParse_ChkType(_right, OEParse_grpInteger);
      OEParse_Convert(&(_left), _res);
      OEParse_Convert(&(_right), _res);
      return DEREF(OEParse_Node, _left, 23361)._type;
    } else if (_subcl==OEParse_scAnd||_subcl==OEParse_scOr)  {
      OEParse_ChkType(_left, OTable_strBool);
      OEParse_ChkType(_right, OTable_strBool);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 23547);
    } else if (_subcl==OEParse_scIn)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      OEParse_ChkType(_right, OTable_strSet);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 23713);
    } else if (OEParse_scEql<=_subcl&&_subcl<=OEParse_scGeq)  {
      if (IN(_lform, OEParse_numSet, 23810))  {
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
          if (IN(_lform, 0x00001102U, 24372))  {
            OEParse_ChkType(_right, _lform);
          } else if (_lform==OTable_strSysPtr)  {
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _right, 24520)._type, 24526)._form!=OTable_strNil)  {
              OEParse_ChkType(_right, _lform);
            }
          } else if (_lform==OTable_strPointer)  {
            if (!(((OTable_SameType(DEREF(OEParse_Node, _left, 24686)._type, DEREF(OEParse_Node, _right, 24699)._type)||_rform==OTable_strNil)||OTable_ExtOf(DEREF(OEParse_Node, _left, 24782)._type, DEREF(OEParse_Node, _right, 24795)._type))||OTable_ExtOf(DEREF(OEParse_Node, _right, 24840)._type, DEREF(OEParse_Node, _left, 24852)._type)))  {
              OEParse_ErrT1(DEREF(OEParse_Node, _left, 24893)._pos, 165, &(DEREF(OEParse_Node, _right, 24910)._type));
            }
          } else if (_lform==OTable_strProc)  {
            if (_rform==OTable_strProc)  {
              if (!OTable_ParamsMatch(DEREF(OEParse_Node, _left, 25078)._type, DEREF(OEParse_Node, _right, 25091)._type))  {
                OScan_Err(DEREF(OEParse_Node, _right, 25134)._pos, 247);
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
      return INDEX(OTable_predeclType, OTable_strBool, 14, 25632);
    } else if (_subcl==OEParse_scAsh)  {
      OEParse_ChkType(_left, OEParse_grpInteger);
      OEParse_ChkType(_right, OEParse_grpInteger);
      return INDEX(OTable_predeclType, OTable_strLongInt, 14, 25794);
    } else if (_subcl==OEParse_scBit)  {
      OEParse_ChkType(_left, OTable_strLongInt);
      OEParse_ChkType(_right, OEParse_grpInteger);
      return INDEX(OTable_predeclType, OTable_strBool, 14, 25961);
    } else if (_subcl==OEParse_scLsh||_subcl==OEParse_scRot)  {
      OEParse_ChkType(_left, OEParse_grpShift);
      OEParse_ChkType(_right, OEParse_grpInteger);
      return DEREF(OEParse_Node, _left, 26144)._type;
    }
    NO_RETURN (21862);
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
      if (DEREF(OEParse_Node, _right, 26889)._class==OEParse_ndConst)  {
        OEParse_ChkType(_right, OEParse_grpInteger);
      } else {
        OEParse_ChkType(_right, DEREF(OTable_Struct, DEREF(OEParse_Node, _left, 26992)._type, 26998)._form);
      }
      OEParse_Convert(&(_right), DEREF(OTable_Struct, DEREF(OEParse_Node, _left, 27047)._type, 27053)._form);
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
    return INDEX(OTable_predeclType, OTable_strNone, 14, 27668);
    NO_RETURN (26192);
  }
  if (IN(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 27719)._left, 27725)._class, _illegalNode, 27733))  {
    OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 27768)._left, 27774)._pos, 246);
  } else if (DEREF(OEParse_Node, (* _r), 27820)._class==OEParse_ndDOp&&IN(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 27839)._right, 27846)._class, _illegalNode, 27854))  {
    OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 27889)._right, 27896)._pos, 246);
  } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 27942)._left, 27948)._type, 27954)._form==OTable_strUndef||(DEREF(OEParse_Node, (* _r), 27979)._class==OEParse_ndDOp&&DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 27998)._right, 28005)._type, 28011)._form==OTable_strUndef))  {
    DEREF(OEParse_Node, (* _r), 28043)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 28066);
  } else if (DEREF(OEParse_Node, (* _r), 28091)._class==OEParse_ndMOp)  {
    DEREF(OEParse_Node, (* _r), 28118)._type = _CheckMOp(DEREF(OEParse_Node, (* _r), 28139)._subcl, DEREF(OEParse_Node, (* _r), 28149)._left);
  } else if (DEREF(OEParse_Node, (* _r), 28170)._class==OEParse_ndDOp)  {
    DEREF(OEParse_Node, (* _r), 28197)._type = _CheckDOp(DEREF(OEParse_Node, (* _r), 28218)._subcl, DEREF(OEParse_Node, (* _r), 28228)._left, DEREF(OEParse_Node, (* _r), 28237)._right);
  } else if (DEREF(OEParse_Node, (* _r), 28258)._class==OEParse_ndAssign)  {
    DEREF(OEParse_Node, (* _r), 28288)._type = _CheckAssign(DEREF(OEParse_Node, (* _r), 28312)._subcl, DEREF(OEParse_Node, (* _r), 28322)._left, DEREF(OEParse_Node, (* _r), 28331)._right);
  }
  if ((DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 28385)._type, 28391)._form!=OTable_strUndef&&DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28414)._left, 28420)._class==OEParse_ndConst)&&((DEREF(OEParse_Node, (* _r), 28449)._class==OEParse_ndMOp&&_FoldMOp(DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 28476)._type, 28482)._form, DEREF(OEParse_Node, (* _r), 28491)._subcl, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28501)._left, 28507)._conval))||((DEREF(OEParse_Node, (* _r), 28530)._class==OEParse_ndDOp&&DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28549)._right, 28556)._class==OEParse_ndConst)&&_FoldDOp(DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 28585)._type, 28591)._form, DEREF(OEParse_Node, (* _r), 28600)._subcl, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28610)._left, 28616)._conval, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 28627)._right, 28634)._conval))))  {
    (* _r) = DEREF(OEParse_Node, (* _r), 28662)._left;
    DEREF(OEParse_Node, (* _r), 28677)._obj = (OTable_Object)NULL;
    if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 28702)._type, 28708)._form, OEParse_intSet, 28715))  {
      OEParse_SetInt((* _r));
    }
  } else if ((DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 28772)._type, 28778)._form==OTable_strBool&&DEREF(OEParse_Node, (* _r), 28802)._class==OEParse_ndDOp)&&IN(DEREF(OEParse_Node, (* _r), 28833)._subcl, 0x00000120U, 28841))  {
    (* _r) = _FoldBool(DEREF(OEParse_Node, (* _r), 28886)._subcl, DEREF(OEParse_Node, (* _r), 28896)._left, DEREF(OEParse_Node, (* _r), 28905)._right);
  }
}

void OEParse_ChkVar (OEParse_Node _n) {
  {
    SHORTINT _temp_ = DEREF(OEParse_Node, _n, 29232)._class;
    switch (_temp_) {
      case 2:
      case 4: {
        break;
      }
      case 1:
      case 3: {
        if (DEREF(OEParse_Node, _n, 29339)._class==OEParse_ndField)  {
          OEParse_ChkVar(DEREF(OEParse_Node, _n, 29380)._left);
        }
        if (DEREF(OTable_Object, DEREF(OEParse_Node, _n, 29410)._obj, 29415)._mnolev<0&&DEREF(OTable_Object, DEREF(OEParse_Node, _n, 29447)._obj, 29452)._mark!=OTable_exportWrite)  {
          OScan_Err(DEREF(OEParse_Node, _n, 29497)._pos, 249);
        }
        break;
      }
      case 5:
      case 6: {
        OEParse_ChkVar(DEREF(OEParse_Node, _n, 29599)._left);
        break;
      }
      default:  {
      OScan_Err(DEREF(OEParse_Node, _n, 29630)._pos, 249);
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
    while (DEREF(OTable_Struct, _t, 30756)._form==OTable_strDynArray||(DEREF(OTable_Struct, _t, 30783)._form==OTable_strArray&&!_dyn))  {
      INCLI(_i, 1, 30820);
      _t = DEREF(OTable_Struct, _t, 30843)._base;
    }
    return _i;
    NO_RETURN (30333);
  }

  LONGINT _ArrayLen (OTable_Struct _t, LONGINT _dim) {
    while (_dim!=0)  {
      _t = DEREF(OTable_Struct, _t, 31267)._base;
      DECLI(_dim, 1, 31283);
    }
    if (DEREF(OTable_Struct, _t, 31315)._form==OTable_strDynArray)  {
      return -1;
    } else {
      return DEREF(OTable_Struct, _t, 31387)._len;
    }
    NO_RETURN (30907);
  }

  OEParse_Node _Find (INTEGER _num) {
    OEParse_Node _n;
    _n = _apar;
    while (_num>1)  {
      _n = DEREF(OEParse_Node, _n, 31645)._link;
      DECI(_num, 1, 31661);
    }
    return _n;
    NO_RETURN (31434);
  }

  BOOLEAN _ChkParNum (INTEGER _num) {
    OEParse_Node _n;
    if (_num<_numPar)  {
      _n = _Find(_num);
      OScan_Err(DEREF(OEParse_Node, _n, 32000)._pos, 250);
    } else if (_num>_numPar)  {
      OScan_Err(_endOfParams, 251);
    }
    return _num==_numPar;
    NO_RETURN (31724);
  }

  void _ChkConst (INTEGER _num) {
    OEParse_Node _n;
    _n = _Find(_num);
    if (DEREF(OEParse_Node, _n, 32398)._class!=OEParse_ndConst)  {
      OScan_Err(DEREF(OEParse_Node, _n, 32438)._pos, 203);
    }
  }

  void _ChkTypeVal (INTEGER _num) {
    OEParse_Node _n;
    _n = _Find(_num);
    if (DEREF(OEParse_Node, _n, 32699)._class!=OEParse_ndType)  {
      OScan_Err(DEREF(OEParse_Node, _n, 32736)._pos, 205);
    }
  }

  void _CreateConst (OEParse_Node (* _n), LONGINT _def) {
    (* _n) = OEParse_NewNode(OEParse_ndConst);
    DEREF(OEParse_Node, (* _n), 32964)._conval = OTable_NewConst();
    DEREF(OEParse_Node, (* _n), 32997)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 33020);
    DEREF(OTable_Const, DEREF(OEParse_Node, (* _n), 33043)._conval, 33051)._intval = _def;
    OEParse_SetInt((* _n));
  }
  {
    SHORTINT _temp_ = _id;
    switch (_temp_) {
      case 18:
      case 19: {
        if (_ChkParNum(1))  {
          if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 33217)._type, 33223)._form, OEParse_realSet, 33230))  {
            _CreateConst(_call, 0);
            DEREF(OEParse_Node, (* _call), 33294)._type = DEREF(OEParse_Node, _apar, 33308)._type;
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 33333)._type, 33339)._form==OTable_strReal)  {
              if (_id==OTable_predMAX)  {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 33412)._conval, 33420)._real = MAX_REAL;
              } else {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 33475)._conval, 33483)._real = MIN_REAL;
              }
            } else {
              if (_id==OTable_predMAX)  {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 33585)._conval, 33593)._real = OMachine_maxLReal;
              } else {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 33649)._conval, 33657)._real = OMachine_minLReal;
              }
            }
          } else {
            {
              SHORTINT _temp_ = DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 33740)._type, 33746)._form;
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
                  _min = -128;
                  _max = 127;
                  break;
                }
                case 4: {
                  _min = -32768;
                  _max = 32767;
                  break;
                }
                case 5: {
                  _min = OMachine_minLInt;
                  _max = OMachine_maxLInt;
                  break;
                }
                case 8: {
                  _min = 0;
                  _max = 31;
                  break;
                }
                default:  {
                OScan_Err(DEREF(OEParse_Node, _apar, 34159)._pos, 254);
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
      case 14:
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
              case 14: {
                _set = OEParse_realSet;
                break;
              }
              case 15: {
                _set = 0x00000058U;
                break;
              }
              case 16: {
                _set = 0x00000004U;
                break;
              }
              case 17: {
                _set = 0x000000B0U;
                break;
              }
              default: NO_LABEL (_temp_, 34565);
            }
          }
          if (!IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 34874)._type, 34880)._form, _set, 34887))  {
            OEParse_ErrNT1(171, _apar);
            DEREF(OEParse_Node, _apar, 34986)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 35009);
          } else {
            {
              SHORTINT _temp_ = _id;
              switch (_temp_) {
                case 13: {
                  OEParse_Convert(&(_apar), OTable_strChar);
                  break;
                }
                case 14: {
                  OEParse_Convert(&(_apar), OTable_strLongInt);
                  break;
                }
                case 15: {
                  OEParse_Convert(&(_apar), DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 35207)._type, 35213)._form+1);
                  break;
                }
                case 16: {
                  OEParse_Convert(&(_apar), OTable_strInteger);
                  break;
                }
                case 17: {
                  OEParse_Convert(&(_apar), DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 35324)._type, 35330)._form- 1);
                  break;
                }
                default: NO_LABEL (_temp_, 35045);
              }
            }
          }
        } else {
          DEREF(OEParse_Node, _apar, 35391)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 35414);
        }
        (* _call) = _apar;
        break;
      }
      case 21:
      case 22:
      case 23:
      case 24:
      case 25: {
        (* _call) = OEParse_NewNode(OEParse_ndMOp);
        if (_ChkParNum(1))  {
          DEREF(OEParse_Node, (* _call), 35601)._left = _apar;
          DEREF(OEParse_Node, (* _call), 35629)._subcl = _id;
          OEParse_Check(_call);
          if (_id==OTable_sysADR&&!IN(DEREF(OEParse_Node, _apar, 35699)._class, 0x0000037EU, 35707))  {
            OScan_Err(DEREF(OEParse_Node, _apar, 35809)._pos, 255);
          } else if (_id==OTable_sysCC&&DEREF(OEParse_Node, _apar, 35880)._class!=OEParse_ndConst)  {
            OScan_Err(DEREF(OEParse_Node, _apar, 35923)._pos, 203);
          }
          if (_id==OTable_sysADR&&IN(DEREF(OEParse_Node, _apar, 36008)._class, 0x0000030EU, 36016))  {
            INCL(DEREF(OTable_Object, DEREF(OEParse_Node, _apar, 36090)._obj, 36095)._flags, OTable_flagAddressed, 36080);
          }
        }
        break;
      }
      case 26:
      case 27:
      case 28:
      case 29: {
        (* _call) = OEParse_NewNode(OEParse_ndDOp);
        DEREF(OEParse_Node, (* _call), 36246)._subcl = _id;
        if (_ChkParNum(2))  {
          DEREF(OEParse_Node, (* _call), 36301)._left = _apar;
          DEREF(OEParse_Node, (* _call), 36329)._right = DEREF(OEParse_Node, _apar, 36344)._link;
          OEParse_Check(_call);
        }
        break;
      }
      case 20: {
        (* _call) = OEParse_NewNode(OEParse_ndConst);
        DEREF(OEParse_Node, (* _call), 36444)._conval = OTable_NewConst();
        DEREF(OEParse_Node, (* _call), 36480)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 36503);
        if (_ChkParNum(1))  {
          _ChkTypeVal(1);
          _min = DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 36590)._type, 36596)._size;
          if (_min>=0)  {
            DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 36677)._conval, 36685)._intval = _min;
            OEParse_SetInt((* _call));
          } else if (_min==-1)  {
            DEREF(OEParse_Node, (* _call), 36814)._class = OEParse_ndMOp;
            DEREF(OEParse_Node, (* _call), 36846)._subcl = OEParse_scSize;
            DEREF(OEParse_Node, (* _call), 36879)._left = _apar;
          } else {
            OScan_Err(DEREF(OEParse_Node, _apar, 36968)._pos, 258);
          }
        }
        break;
      }
      case 30: {
        (* _call) = OEParse_NewNode(OEParse_ndConst);
        DEREF(OEParse_Node, (* _call), 37103)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 37126);
        DEREF(OEParse_Node, (* _call), 37152)._conval = OTable_NewConst();
        DEREF(OEParse_Node, (* _call), 37188)._left = _apar;
        if (_numPar==1)  {
          _CreateConst(&(DEREF(OEParse_Node, _apar, 37305)._link), 0);
          INCI(_numPar, 1, 37325);
        }
        if (_ChkParNum(2))  {
          _min = _ArrayDim(DEREF(OEParse_Node, _apar, 37406)._type, FALSE);
          if (_min<0)  {
            OEParse_ErrNT1(151, _apar);
          }
          DEREF(OEParse_Node, (* _call), 37525)._right = DEREF(OEParse_Node, _apar, 37540)._link;
          OEParse_ChkType(DEREF(OEParse_Node, (* _call), 37569)._right, OEParse_grpInteger);
          _ChkConst(2);
          if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 37629)._right, 37636)._conval, 37644)._intval<0||DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 37664)._right, 37671)._conval, 37679)._intval>_ArrayDim(DEREF(OEParse_Node, _apar, 37702)._type, FALSE))  {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 37744)._right, 37751)._pos, 259);
          } else if (DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 37807)._left, 37813)._type, 37819)._form==OTable_strDynArray&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 37882)._left, 37888)._type, 37894)._flags, 37875))  {
            OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 37929)._left, 37935)._pos, 153);
          } else if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38010)._right, 38017)._conval, 38025)._intval<=_ArrayDim(DEREF(OEParse_Node, _apar, 38049)._type, TRUE))  {
            DEREF(OEParse_Node, (* _call), 38083)._class = OEParse_ndDOp;
            DEREF(OEParse_Node, (* _call), 38115)._subcl = OEParse_scLen;
          } else {
            DEREF(OTable_Const, DEREF(OEParse_Node, (* _call), 38159)._conval, 38167)._intval = _ArrayLen(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38193)._left, 38199)._type, DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _call), 38211)._right, 38218)._conval, 38226)._intval);
          }
        }
        break;
      }
      case 31: {
        (* _call) = OEParse_NewNode(OEParse_ndMOp);
        DEREF(OEParse_Node, (* _call), 38315)._subcl = OEParse_scVal;
        if (_ChkParNum(2))  {
          _ChkTypeVal(1);
          DEREF(OEParse_Node, (* _call), 38397)._left = DEREF(OEParse_Node, _apar, 38411)._link;
          DEREF(OEParse_Node, (* _call), 38431)._type = DEREF(OEParse_Node, _apar, 38445)._type;
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
        DEREF(OEParse_Node, (* _call), 38632)._subcl = _id;
        if ((_id==OTable_predINC||_id==OTable_predDEC)&&_numPar==1)  {
          _CreateConst(&(DEREF(OEParse_Node, _apar, 38792)._link), 1);
          INCI(_numPar, 1, 38812);
        }
        if (_ChkParNum(2))  {
          DEREF(OEParse_Node, (* _call), 38876)._left = _apar;
          DEREF(OEParse_Node, (* _call), 38904)._right = DEREF(OEParse_Node, _apar, 38919)._link;
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
        DEREF(OEParse_Node, (* _call), 39309)._subcl = OEParse_scNewFix;
        DEREF(OEParse_Node, (* _call), 39340)._left = _apar;
        if ((void*)_apar!=(void*)NULL)  {
          OEParse_ChkType(_apar, OTable_strPointer);
          if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 39455)._type, 39461)._flags, 39448)||((void*)DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 39489)._type, 39495)._base!=(void*)NULL&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 39534)._type, 39540)._base, 39546)._flags, 39527)))  {
            OScan_Err(DEREF(OEParse_Node, _apar, 39581)._pos, 154);
          }
        }
        if (_ChkParNum((INTEGER) (_ArrayDim(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 39697)._type, 39703)._base, TRUE)+2)))  {
          OEParse_ChkType(_apar, OTable_strPointer);
          OEParse_ChkVar(_Find(1));
          if (_numPar>1)  {
            DEREF(OEParse_Node, (* _call), 39834)._subcl = OEParse_scNewDyn;
            DEREF(OEParse_Node, (* _call), 39869)._right = DEREF(OEParse_Node, _apar, 39884)._link;
            _n = DEREF(OEParse_Node, (* _call), 39911)._right;
            while ((void*)_n!=(void*)NULL)  {
              OEParse_ChkType(_n, OEParse_grpInteger);
              _n = DEREF(OEParse_Node, _n, 40004)._link;
            }
          }
        }
        break;
      }
      case 47: {
        (* _call) = OEParse_NewNode(OEParse_ndAssign);
        DEREF(OEParse_Node, (* _call), 40111)._subcl = OEParse_scDispose;
        DEREF(OEParse_Node, (* _call), 40143)._left = _apar;
        if (_ChkParNum(1))  {
          OEParse_ChkType(_apar, OEParse_grpPointer);
          if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 40285)._type, 40291)._flags, 40278)||((void*)DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 40319)._type, 40325)._base!=(void*)NULL&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 40364)._type, 40370)._base, 40376)._flags, 40357)))  {
            OScan_Err(DEREF(OEParse_Node, _apar, 40411)._pos, 155);
          }
        }
        break;
      }
      case 48: {
        (* _call) = OEParse_NewNode(OEParse_ndAssign);
        DEREF(OEParse_Node, (* _call), 40553)._subcl = OEParse_scCollect;
        if (_ChkParNum(0))  {
        }
        break;
      }
      case 45: {
        (* _call) = OEParse_NewNode(OEParse_ndTrap);
        if (_ChkParNum(1))  {
          DEREF(OEParse_Node, (* _call), 40703)._left = _apar;
          _ChkConst(1);
          OEParse_ChkType(_apar, OEParse_grpInteger);
          OEParse_ChkRange(_apar, 0, 255, 267);
        }
        break;
      }
      case 46: {
        (* _call) = OEParse_NewNode(OEParse_ndAssert);
        if (_numPar==1)  {
          _CreateConst(&(DEREF(OEParse_Node, _apar, 40977)._link), 1);
          INCI(_numPar, 1, 41011);
        }
        if (_ChkParNum(2))  {
          DEREF(OEParse_Node, (* _call), 41075)._right = _apar;
          OEParse_ChkType(_apar, OTable_strBool);
          DEREF(OEParse_Node, (* _call), 41139)._left = DEREF(OEParse_Node, _apar, 41153)._link;
          _ChkConst(2);
          OEParse_ChkType(DEREF(OEParse_Node, (* _call), 41204)._left, OEParse_grpInteger);
          OEParse_ChkRange(DEREF(OEParse_Node, (* _call), 41247)._left, 0, 255, 267);
        }
        break;
      }
      case 37: {
        (* _call) = OEParse_NewNode(OEParse_ndAssign);
        DEREF(OEParse_Node, (* _call), 41386)._subcl = OEParse_scMove;
        DEREF(OEParse_Node, (* _call), 41415)._right = _apar;
        if (_ChkParNum(3))  {
          OEParse_ChkType(_apar, OTable_strLongInt);
          OEParse_ChkType(DEREF(OEParse_Node, _apar, 41519)._link, OTable_strLongInt);
          OEParse_ChkType(DEREF(OEParse_Node, DEREF(OEParse_Node, _apar, 41563)._link, 41569)._link, OEParse_grpInteger);
        }
        break;
      }
      default: NO_LABEL (_temp_, 33119);
    }
  }
}

void OEParse_RecycleMem (OEParse_Node (* _node)) {
  if ((void*)(* _node)!=(void*)NULL&&DEREF(OEParse_Node, (* _node), 41703)._class!=OEParse_ndCollected)  {
    DEREF(OEParse_Node, (* _node), 41741)._class = OEParse_ndCollected;
    OEParse_RecycleMem(&(DEREF(OEParse_Node, (* _node), 41787)._left));
    OEParse_RecycleMem(&(DEREF(OEParse_Node, (* _node), 41818)._right));
    OEParse_RecycleMem(&(DEREF(OEParse_Node, (* _node), 41850)._link));
    DEREF(OEParse_Node, (* _node), 41869)._link = OEParse_nodeHeap;
    OEParse_nodeHeap = (* _node);
    (* _node) = (OEParse_Node)NULL;
  }
}

void _init_OEParse (void) {
  moduleId = add_module ("OEParse");
  td_OEParse_NodeDesc = create_td("NodeDesc", sizeof(OEParse_NodeDesc), sizeof(TD_OEParse_NodeDesc), NULL, 0);
  OEParse_nodeHeap = (OEParse_Node)NULL;
}
