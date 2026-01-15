#ifndef __OEParse_
#define __OEParse_

#ifdef MODULE_OEParse
#define EXTERN_OEParse
#else
#define EXTERN_OEParse extern
#endif

#include "_OGCC.h"
#include "OTable.h"


struct TD_OEParse_NodeDesc;

extern const SHORTINT OEParse_ndVar;
extern const SHORTINT OEParse_ndVarPar;
extern const SHORTINT OEParse_ndField;
extern const SHORTINT OEParse_ndDeref;
extern const SHORTINT OEParse_ndIndex;
extern const SHORTINT OEParse_ndGuard;
extern const SHORTINT OEParse_ndType;
extern const SHORTINT OEParse_ndProc;
extern const SHORTINT OEParse_ndTBProc;
extern const SHORTINT OEParse_ndTBSuper;
extern const SHORTINT OEParse_ndConst;
extern const SHORTINT OEParse_ndUpto;
extern const SHORTINT OEParse_ndMOp;
extern const SHORTINT OEParse_ndDOp;
extern const SHORTINT OEParse_ndInitTd;
extern const SHORTINT OEParse_ndAssign;
extern const SHORTINT OEParse_ndCall;
extern const SHORTINT OEParse_ndIfElse;
extern const SHORTINT OEParse_ndIf;
extern const SHORTINT OEParse_ndCase;
extern const SHORTINT OEParse_ndCaseElse;
extern const SHORTINT OEParse_ndCaseDo;
extern const SHORTINT OEParse_ndWhile;
extern const SHORTINT OEParse_ndRepeat;
extern const SHORTINT OEParse_ndFor;
extern const SHORTINT OEParse_ndForHeader;
extern const SHORTINT OEParse_ndForRange;
extern const SHORTINT OEParse_ndLoop;
extern const SHORTINT OEParse_ndWithElse;
extern const SHORTINT OEParse_ndWithGuard;
extern const SHORTINT OEParse_ndExit;
extern const SHORTINT OEParse_ndReturn;
extern const SHORTINT OEParse_ndTrap;
extern const SHORTINT OEParse_ndEnter;
extern const SHORTINT OEParse_ndForward;
extern const SHORTINT OEParse_ndAssert;
extern const SHORTINT OEParse_scTimes;
extern const SHORTINT OEParse_scRDiv;
extern const SHORTINT OEParse_scIDiv;
extern const SHORTINT OEParse_scMod;
extern const SHORTINT OEParse_scAnd;
extern const SHORTINT OEParse_scPlus;
extern const SHORTINT OEParse_scMinus;
extern const SHORTINT OEParse_scOr;
extern const SHORTINT OEParse_scEql;
extern const SHORTINT OEParse_scNeq;
extern const SHORTINT OEParse_scLss;
extern const SHORTINT OEParse_scLeq;
extern const SHORTINT OEParse_scGrt;
extern const SHORTINT OEParse_scGeq;
extern const SHORTINT OEParse_scIn;
extern const SHORTINT OEParse_scIs;
extern const SHORTINT OEParse_scNot;
extern const SHORTINT OEParse_scSize;
extern const SHORTINT OEParse_scAbs;
extern const SHORTINT OEParse_scCap;
extern const SHORTINT OEParse_scOdd;
extern const SHORTINT OEParse_scAdr;
extern const SHORTINT OEParse_scCc;
extern const SHORTINT OEParse_scAsh;
extern const SHORTINT OEParse_scBit;
extern const SHORTINT OEParse_scLsh;
extern const SHORTINT OEParse_scRot;
extern const SHORTINT OEParse_scLen;
extern const SHORTINT OEParse_scVal;
extern const SHORTINT OEParse_scAssign;
extern const SHORTINT OEParse_scIncl;
extern const SHORTINT OEParse_scExcl;
extern const SHORTINT OEParse_scInc;
extern const SHORTINT OEParse_scDec;
extern const SHORTINT OEParse_scCopy;
extern const SHORTINT OEParse_scMove;
extern const SHORTINT OEParse_scGet;
extern const SHORTINT OEParse_scPut;
extern const SHORTINT OEParse_scGetReg;
extern const SHORTINT OEParse_scPutReg;
extern const SHORTINT OEParse_scNewSys;
extern const SHORTINT OEParse_scNewFix;
extern const SHORTINT OEParse_scNewDyn;
extern const SHORTINT OEParse_scDispose;
extern const SHORTINT OEParse_scConv;
extern const SHORTINT OEParse_scCollect;
extern const SHORTINT OEParse_grpInteger;
extern const SHORTINT OEParse_grpIntOrChar;
extern const SET OEParse_intSet;
extern const SET OEParse_realSet;
extern const SET OEParse_numSet;
typedef struct OEParse_NodeDesc (* OEParse_Node);
typedef struct OEParse_NodeDesc {
  OEParse_Node _left;
  OEParse_Node _right;
  OEParse_Node _link;
  SHORTINT _class;
  SHORTINT _subcl;
  OTable_Struct _type;
  OTable_Object _obj;
  OTable_Const _conval;
  LONGINT _pos;
} OEParse_NodeDesc;

typedef struct TD_OEParse_NodeDesc {
  TDCORE
} TD_OEParse_NodeDesc;
EXTERN_OEParse TD_OEParse_NodeDesc* td_OEParse_NodeDesc;

extern OEParse_Node OEParse_NewNode (SHORTINT _class);
extern void OEParse_ErrT1 (LONGINT _pos, INTEGER _num, OTable_Struct (* _t1));
extern void OEParse_ErrNT1 (INTEGER _num, OEParse_Node _n1);
extern void OEParse_Err (INTEGER _num);
extern BOOLEAN OEParse_ArrayComp (OEParse_Node _a, OTable_Struct _f);
extern BOOLEAN OEParse_AssignComp (OTable_Struct _var, OEParse_Node _expr);
extern BOOLEAN OEParse_ChkGuard (OEParse_Node _var, OTable_Object _typeObj, BOOLEAN _chkQual, LONGINT _tpos);
extern void OEParse_ChkType (OEParse_Node _n, SHORTINT _form);
extern void OEParse_ChkRange (OEParse_Node _node, LONGINT _lower, LONGINT _upper, INTEGER _num);
extern void OEParse_SetInt (OEParse_Node _r);
extern void OEParse_Check (OEParse_Node (* _r));
extern void OEParse_ChkVar (OEParse_Node _n);
extern void OEParse_PredefProc (OEParse_Node (* _call), SHORTINT _id, OEParse_Node _apar, INTEGER _numPar, LONGINT _endOfParams);
extern void OEParse_RecycleMem (OEParse_Node (* _node));

extern void _init_OEParse (void);

#endif
