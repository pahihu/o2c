#include "OParse.h"
#include "Filenames.h"
#include "OMachine.h"
#include "OSym.h"
#include "OScan.h"
#include "Strings.h"
#include "Strings.h"
#include "OTable.h"

static ModuleId moduleId;



static OEParse_Node OParse_root;
static CHAR OParse_undeclIdents[512];



static void OParse_CheckSym (SHORTINT _s) {
  if (OScan_sym==_s)  {
    OScan_GetSym();
  } else {
    OEParse_Err(100+(INTEGER) _s);
    if (((_s==OScan_becomes&&(OScan_sym==OScan_eql||OScan_sym==OScan_colon))||(_s==OScan_eql&&OScan_sym==OScan_becomes))||(((OScan_of<=_s&&_s<=OScan_by)||(OScan_end<=_s&&_s<=OScan_module))&&OScan_sym==OScan_ident))  {
      OScan_GetSym();
    }
  }
}

static OTable_Object OParse_NewObject (LONGINT __name_0, const CHAR (* __name_p), SHORTINT _mode) {
  CHAR (* _name);
  OTable_Object _obj;
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  _obj = OTable_NewObject(__name_0, _name, _mode, OScan_lastSym);
  if (OTable_external)  {
    INCL(DEREF(OTable_Object, _obj, 1109)._flags, OTable_flagExternal, 1100);
  }
  return _obj;
  NO_RETURN (928);
}

static OTable_Struct OParse_NewStruct (SHORTINT _form) {
  OTable_Struct _s;
  _s = OTable_NewStruct(_form);
  if (OTable_external)  {
    INCL(DEREF(OTable_Struct, _s, 1322)._flags, OTable_flagExternal, 1315);
  }
  return _s;
  NO_RETURN (1186);
}

static void OParse_Qualident (OTable_Object (* _obj), LONGINT (* _pos)) {
  CHAR _identName[98];
  BOOLEAN _saveUnderscoreState;
  (* _pos) = OScan_lastSym;
  COPYSTRING(&(_identName), "|");
  _saveUnderscoreState = OScan_underscore;
  if (OScan_sym==OScan_ident)  {
    (* _obj) = OTable_Find(256, (CHAR *) OScan_ref);
    Strings_Append(256, (CHAR *) OScan_ref, 98, (CHAR *) _identName);
    OScan_GetSym();
    if ((void*)(* _obj)==(void*)NULL&&OScan_sym==OScan_period)  {
      OScan_GetSym();
      OScan_GetSym();
    }
  } else {
    (* _obj) = (OTable_Object)NULL;
  }
  if ((void*)(* _obj)!=(void*)NULL)  {
    if (DEREF(OTable_Object, (* _obj), 2516)._mode==OTable_objForwardType)  {
      (* _obj) = (OTable_Object)NULL;
    } else if (DEREF(OTable_Object, (* _obj), 2659)._mode==OTable_objModule)  {
      OScan_underscore = OScan_underscore||IN(OTable_flagExternal, DEREF(OTable_Object, (* _obj), 2860)._flags, 2854);
      OParse_CheckSym(OScan_period);
      Strings_Append(2, (CHAR *) ".", 98, (CHAR *) _identName);
      Strings_Append(256, (CHAR *) OScan_ref, 98, (CHAR *) _identName);
      if (OScan_sym==OScan_ident)  {
        (* _pos) = OScan_lastSym;
        (* _obj) = OTable_FindImport(256, (CHAR *) OScan_ref, (* _obj));
        OScan_GetSym();
      } else {
        COPYSTRING(&(_identName), "|||");
        (* _obj) = (OTable_Object)NULL;
      }
      OScan_underscore = _saveUnderscoreState;
    }
  }
  if ((void*)(* _obj)==(void*)NULL)  {
    Strings_Append(2, (CHAR *) "|", 98, (CHAR *) _identName);
    if (Strings_Pos(98, (CHAR *) _identName, 512, (CHAR *) OParse_undeclIdents, 0)>=0)  {
      OScan_lastErr = (* _pos);
    } else {
      OScan_Err((* _pos), 204);
      Strings_Append(98, (CHAR *) _identName, 512, (CHAR *) OParse_undeclIdents);
    }
    (* _obj) = OParse_NewObject(4, (CHAR *) OScan_undefStr, OTable_objVar);
  }
}

static void OParse_Type (OTable_Struct (* _t), LONGINT (* _pos), SHORTINT _exp);

static void OParse_FormalPars (OTable_Struct (* _fpars)) {
  OTable_Object _first, _last, _obj;
  SHORTINT _mode;
  LONGINT _pos;
  BOOLEAN _resynch;
  (* _fpars) = OParse_NewStruct(OTable_strProc);
  if (OScan_sym==OScan_lParen)  {
    OScan_GetSym();
    if ((OScan_sym==OScan_var||OScan_sym==OScan_ident)||(OTable_external&&OScan_sym==OScan_upto))  {
      _last = (OTable_Object)NULL;
      _resynch = FALSE;
      while(1) {
        if (OTable_external&&OScan_sym==OScan_upto)  {
          _obj = OParse_NewObject(3, (CHAR *) OTable_genericParam, OTable_objVar);
          DEREF(OTable_Object, _obj, 5103)._type = INDEX(OTable_predeclType, OTable_strNone, 14, 5126);
          if ((void*)_last==(void*)NULL)  {
            DEREF(OTable_Struct, (* _fpars), 5189)._link = _obj;
          } else {
            DEREF(OTable_Object, _last, 5238)._link = _obj;
          }
          OScan_GetSym();
          INCLI(DEREF(OTable_Struct, (* _fpars), 5325)._len, 1, 5315);
          goto _exit4930;
        } else {
          if (OScan_sym==OScan_var)  {
            _mode = OTable_objVarPar;
            OScan_GetSym();
          } else {
            _mode = OTable_objVar;
          }
          _first = _last;
          while(1) {
            if (OScan_sym==OScan_ident)  {
              if ((void*)_last==(void*)NULL)  {
                DEREF(OTable_Struct, (* _fpars), 6238)._link = OParse_NewObject(256, (CHAR *) OScan_ref, _mode);
                _last = DEREF(OTable_Struct, (* _fpars), 6304)._link;
              } else {
                DEREF(OTable_Object, _last, 6354)._link = OParse_NewObject(256, (CHAR *) OScan_ref, _mode);
                _last = DEREF(OTable_Object, _last, 6419)._link;
              }
              OScan_GetSym();
              INCLI(DEREF(OTable_Struct, (* _fpars), 6499)._len, 1, 6489);
            } else {
              OEParse_Err(100);
              OScan_GetSym();
              while ((OScan_sym!=OScan_rParen&&OScan_sym!=OScan_end)&&OScan_sym<OScan_begin)  {
                OScan_GetSym();
              }
              _resynch = TRUE;
            }
            if ((OScan_sym!=OScan_comma&&OScan_sym!=OScan_ident)||_resynch)  {
              goto _exit5816;
            }
            OParse_CheckSym(OScan_comma);
          }
          _exit5816:
          if (_resynch)  {
            goto _exit4930;
          }
          OParse_CheckSym(OScan_colon);
          OParse_Type(&(DEREF(OTable_Object, _last, 7130)._type), &(_pos), OTable_exportNot);
          if ((void*)DEREF(OTable_Struct, DEREF(OTable_Object, _last, 7177)._type, 7183)._obj==(void*)NULL&&!IN(DEREF(OTable_Struct, DEREF(OTable_Object, _last, 7204)._type, 7210)._form, 0x00048001U, 7217))  {
            OScan_Err(_pos, 212);
          }
          if ((void*)_first==(void*)NULL)  {
            _first = DEREF(OTable_Struct, (* _fpars), 7489)._link;
          } else {
            _first = DEREF(OTable_Object, _first, 7541)._link;
          }
          while (_first!=_last)  {
            DEREF(OTable_Object, _first, 7620)._type = DEREF(OTable_Object, _last, 7634)._type;
            _first = DEREF(OTable_Object, _first, 7670)._link;
          }
          if ((OScan_sym!=OScan_semicolon&&OScan_sym!=OScan_ident)&&OScan_sym!=OScan_var)  {
            goto _exit4930;
          }
          OParse_CheckSym(OScan_semicolon);
        }
      }
      _exit4930: ;
    }
    OParse_CheckSym(OScan_rParen);
    if (OScan_sym==OScan_colon)  {
      OScan_GetSym();
      OParse_Type(&(DEREF(OTable_Struct, (* _fpars), 8032)._base), &(_pos), OTable_exportNot);
      if (!IN(DEREF(OTable_Struct, DEREF(OTable_Struct, (* _fpars), 8101)._base, 8107)._form, 0x0000F1FEU, 8114))  {
        OScan_Err(_pos, 221);
      } else if ((void*)DEREF(OTable_Struct, DEREF(OTable_Struct, (* _fpars), 8318)._base, 8324)._obj==(void*)NULL)  {
        OScan_Err(_pos, 212);
      }
      DEREF(OTable_Struct, (* _fpars), 8437)._size = OTable_structSize((* _fpars));
      return;
    }
  }
  DEREF(OTable_Struct, (* _fpars), 8512)._base = INDEX(OTable_predeclType, OTable_strNone, 14, 8535);
  DEREF(OTable_Struct, (* _fpars), 8557)._size = OTable_structSize((* _fpars));
}

static void OParse_Expr (OEParse_Node (* _r));

static void OParse_ConstExpr (OTable_Const (* _const), OTable_Struct (* _type)) {
  OEParse_Node _node;
  OParse_Expr(&(_node));
  if (DEREF(OEParse_Node, _node, 9113)._class!=OEParse_ndConst)  {
    OScan_Err(DEREF(OEParse_Node, _node, 9175)._pos, 203);
    (* _const) = OTable_NewConst();
    (* _type) = INDEX(OTable_predeclType, OTable_strUndef, 14, 9311);
  } else {
    OEParse_SetInt(_node);
    (* _const) = DEREF(OEParse_Node, _node, 9433)._conval;
    (* _type) = DEREF(OEParse_Node, _node, 9461)._type;
  }
}

static void OParse_Designator (OEParse_Node (* _n)) {
  OEParse_Node _n0;
  OTable_Object _obj;
  LONGINT _pos;
  BOOLEAN _saveUnderscoreState;

  void _Deref (BOOLEAN _chkPtr) {
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _n), 10038)._type, 10044)._form==OTable_strPointer)  {
      (* _n) = OEParse_NewNode(OEParse_ndDeref);
      DEREF(OEParse_Node, (* _n), 10140)._left = _n0;
      DEREF(OEParse_Node, (* _n), 10163)._type = DEREF(OTable_Struct, DEREF(OEParse_Node, _n0, 10175)._type, 10181)._base;
      _n0 = (* _n);
    } else if (_chkPtr)  {
      OScan_Err(DEREF(OEParse_Node, (* _n), 10303)._pos, 213);
    }
  }
  (* _n) = OEParse_NewNode(OEParse_ndVar);
  OParse_Qualident(&(_obj), &(DEREF(OEParse_Node, (* _n), 10505)._pos));
  DEREF(OEParse_Node, (* _n), 10518)._obj = _obj;
  DEREF(OEParse_Node, (* _n), 10537)._type = DEREF(OTable_Object, _obj, 10550)._type;
  if (DEREF(OTable_Object, _obj, 10569)._mode==OTable_objConst)  {
    DEREF(OEParse_Node, (* _n), 10600)._class = OEParse_ndConst;
    DEREF(OEParse_Node, (* _n), 10629)._conval = OTable_NewConst();
    ASSERT_TYPE(TAG(TD_OTable_ConstDesc, DEREF(OEParse_Node, (* _n), 10717)._conval, 10725), td_OTable_ConstDesc, 10725);
    DEREF(OTable_Const, DEREF(OEParse_Node, (* _n), 10717)._conval, 10725) = DEREF(OTable_Const, DEREF(OTable_Object, _obj, 10733)._const, 10740);
    if (-3<DEREF(OTable_Object, _obj, 10773)._mnolev&&DEREF(OTable_Object, _obj, 10789)._mnolev<0)  {
      DEREF(OEParse_Node, (* _n), 10831)._obj = (OTable_Object)NULL;
    }
  } else if (DEREF(OTable_Object, _obj, 10928)._mode==OTable_objVarPar)  {
    DEREF(OEParse_Node, (* _n), 10960)._class = OEParse_ndVarPar;
  } else if (DEREF(OTable_Object, _obj, 10996)._mode==OTable_objType)  {
    DEREF(OEParse_Node, (* _n), 11026)._class = OEParse_ndType;
    return;
  } else if (IN(DEREF(OTable_Object, _obj, 11074)._mode, 0x00000380U, 11081))  {
    DEREF(OEParse_Node, (* _n), 11142)._class = OEParse_ndProc;
  }
  while(1) {
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _n), 11230)._type, 11236)._form==OTable_strProc)  {
      goto _exit11175;
    }
    _n0 = (* _n);
    if (OScan_sym==OScan_period)  {
      _Deref(FALSE);
      _saveUnderscoreState = OScan_underscore;
      OScan_underscore = OTable_external||IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, (* _n), 11704)._type, 11710)._flags, 11700);
      OScan_GetSym();
      OScan_underscore = _saveUnderscoreState;
      if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _n), 11796)._type, 11802)._form!=OTable_strRecord)  {
        OEParse_ErrNT1(150, (* _n));
      } else if (OScan_sym!=OScan_ident)  {
        OEParse_Err(100);
      } else {
        _obj = OTable_FindField(256, (CHAR *) OScan_ref, DEREF(OEParse_Node, (* _n), 12159)._type);
        if ((void*)_obj!=(void*)NULL)  {
          (* _n) = OEParse_NewNode(OEParse_ndField);
          DEREF(OEParse_Node, (* _n), 12349)._left = _n0;
          DEREF(OEParse_Node, (* _n), 12376)._type = DEREF(OTable_Object, _obj, 12389)._type;
          DEREF(OEParse_Node, (* _n), 12410)._obj = _obj;
          if (DEREF(OTable_Object, _obj, 12443)._mode!=OTable_objField)  {
            DEREF(OEParse_Node, (* _n), 12555)._class = OEParse_ndTBProc;
            if (DEREF(OEParse_Node, _n0, 12598)._class==OEParse_ndDeref&&DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 12625)._type, 12631)._link, 12637)._type, 12643)._form==OTable_strPointer)  {
              DEREF(OEParse_Node, (* _n), 12688)._left = DEREF(OEParse_Node, DEREF(OEParse_Node, (* _n), 12699)._left, 12705)._left;
            } else if (DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 12794)._type, 12800)._link, 12806)._type, 12812)._form!=DEREF(OTable_Struct, DEREF(OEParse_Node, _n0, 12823)._type, 12829)._form)  {
              OEParse_Err(245);
            }
            OScan_GetSym();
            if (OScan_sym==OScan_arrow)  {
              if ((((void*)DEREF(OEParse_Node, DEREF(OEParse_Node, (* _n), 13088)._left, 13094)._obj==(void*)NULL||!IN(OTable_flagReceiver, DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _n), 13131)._left, 13137)._obj, 13142)._flags, 13127))||(void*)DEREF(OTable_Struct, DEREF(OEParse_Node, _n0, 13177)._type, 13183)._base==(void*)NULL)||(void*)OTable_FindField(256, (CHAR *) OScan_ref, DEREF(OTable_Struct, DEREF(OEParse_Node, _n0, 13244)._type, 13250)._base)==(void*)NULL)  {
                OEParse_Err(248);
              } else {
                DEREF(OEParse_Node, (* _n), 13378)._class = OEParse_ndTBSuper;
              }
              OScan_GetSym();
            }
            return;
          }
        } else {
          OEParse_Err(204);
        }
      }
      OScan_GetSym();
    } else if (OScan_sym==OScan_lBrak)  {
      _Deref(FALSE);
      do  {
        if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _n), 13768)._type, 13774)._form!=OTable_strArray&&DEREF(OTable_Struct, DEREF(OEParse_Node, (* _n), 13799)._type, 13805)._form!=OTable_strDynArray)  {
          OEParse_ErrNT1(151, (* _n));
        }
        _pos = OScan_lastSym;
        OScan_GetSym();
        (* _n) = OEParse_NewNode(OEParse_ndIndex);
        DEREF(OEParse_Node, (* _n), 14103)._left = _n0;
        DEREF(OEParse_Node, (* _n), 14128)._type = DEREF(OTable_Struct, DEREF(OEParse_Node, _n0, 14140)._type, 14146)._base;
        DEREF(OEParse_Node, (* _n), 14165)._pos = _pos;
        _n0 = (* _n);
        OParse_Expr(&(DEREF(OEParse_Node, (* _n), 14215)._right));
        OEParse_ChkType(DEREF(OEParse_Node, (* _n), 14276)._right, OEParse_grpInteger);
      } while (!(OScan_sym!=OScan_comma));
      OParse_CheckSym(OScan_rBrak);
    } else if (OScan_sym==OScan_arrow)  {
      _Deref(TRUE);
      OScan_GetSym();
    } else if (OScan_sym==OScan_lParen&&DEREF(OTable_Struct, DEREF(OEParse_Node, (* _n), 14537)._type, 14543)._form!=OTable_strUndef)  {
      (* _n) = OEParse_NewNode(OEParse_ndGuard);
      DEREF(OEParse_Node, (* _n), 14614)._left = _n0;
      OScan_GetSym();
      OParse_Qualident(&(DEREF(OEParse_Node, (* _n), 14666)._obj), &(DEREF(OEParse_Node, (* _n), 14674)._pos));
      if (OEParse_ChkGuard(_n0, DEREF(OEParse_Node, (* _n), 14710)._obj, FALSE, DEREF(OEParse_Node, (* _n), 14725)._pos))  {
        DEREF(OEParse_Node, (* _n), 14748)._type = DEREF(OTable_Object, DEREF(OEParse_Node, (* _n), 14759)._obj, 14764)._type;
      } else {
        DEREF(OEParse_Node, (* _n), 14795)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 14818);
      }
      OParse_CheckSym(OScan_rParen);
    } else {
      goto _exit11175;
    }
  }
  _exit11175: ;
}

static void OParse_ProcCall (OEParse_Node (* _call), OEParse_Node _proc) {
  OEParse_Node _apar;
  OTable_Object _fpar;
  OTable_Struct _ftype;
  SHORTINT _fform;
  INTEGER _numPar;
  BOOLEAN _comp;
  LONGINT _endOfParams;

  void _ExprList (OEParse_Node (* _node)) {
    INCI(_numPar, 1, 15671);
    OParse_Expr(_node);
    while (OScan_sym==OScan_comma||(OScan_lParen<=OScan_sym&&OScan_sym<=OScan_ident))  {
      OParse_CheckSym(OScan_comma);
      _ExprList(&(DEREF(OEParse_Node, (* _node), 15825)._link));
    }
  }
  if (DEREF(OEParse_Node, _proc, 15882)._class!=OEParse_ndProc&&!IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _proc, 15910)._type, 15916)._form, 0x00018000U, 15923))  {
    OEParse_ErrNT1(174, _proc);
  }
  _apar = (OEParse_Node)NULL;
  _numPar = 0;
  if (OScan_sym<OScan_semicolon)  {
    OParse_CheckSym(OScan_lParen);
    if (OScan_sym!=OScan_rParen)  {
      _ExprList(&(_apar));
    }
    _endOfParams = OScan_lastSym;
    OParse_CheckSym(OScan_rParen);
  } else {
    _endOfParams = OScan_lastSym;
  }
  (* _call) = OEParse_NewNode(OEParse_ndCall);
  DEREF(OEParse_Node, (* _call), 16373)._left = _proc;
  DEREF(OEParse_Node, (* _call), 16397)._right = _apar;
  if (DEREF(OEParse_Node, _proc, 16426)._class==OEParse_ndProc&&(void*)DEREF(OTable_Object, DEREF(OEParse_Node, _proc, 16453)._obj, 16458)._const!=(void*)NULL)  {
    OEParse_PredefProc(_call, (SHORTINT) DEREF(OTable_Const, DEREF(OTable_Object, DEREF(OEParse_Node, _proc, 16557)._obj, 16562)._const, 16569)._intval, _apar, _numPar, _endOfParams);
  } else if (IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _proc, 16644)._type, 16650)._form, 0x00018000U, 16657))  {
    _fpar = DEREF(OTable_Struct, DEREF(OEParse_Node, _proc, 16801)._type, 16807)._link;
    if (DEREF(OTable_Struct, DEREF(OEParse_Node, _proc, 16829)._type, 16835)._form==OTable_strTBProc)  {
      _fpar = DEREF(OTable_Object, _fpar, 16882)._link;
    }
    while ((void*)_apar!=(void*)NULL)  {
      if ((void*)_fpar==(void*)NULL)  {
        OScan_Err(DEREF(OEParse_Node, _apar, 17019)._pos, 250);
      } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 17093)._type, 17099)._form!=OTable_strNone)  {
        _ftype = DEREF(OTable_Object, _fpar, 17147)._type;
        _fform = DEREF(OTable_Struct, _ftype, 17179)._form;
        if ((_fform==OTable_strDynArray&&DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 17377)._type, 17383)._form==OTable_strDynArray)&&IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 17446)._type, 17452)._flags, 17439))  {
          OScan_Warn(DEREF(OEParse_Node, _apar, 17490)._pos, 296);
        }
        if (DEREF(OTable_Object, _fpar, 17679)._mode==OTable_objVar)  {
          _comp = (_fform==OTable_strDynArray&&OEParse_ArrayComp(_apar, _ftype))||OEParse_AssignComp(_ftype, _apar);
        } else {
          OEParse_ChkVar(_apar);
          if (!((((_ftype==DEREF(OEParse_Node, _apar, 18072)._type||(_fform==OTable_strRecord&&OTable_ExtOf(DEREF(OEParse_Node, _apar, 18132)._type, _ftype)))||(_fform==OTable_strDynArray&&(DEREF(OTable_Struct, DEREF(OTable_Struct, _ftype, 18195)._base, 18201)._form==OTable_strSysByte||OEParse_ArrayComp(_apar, _ftype))))||(_fform==OTable_strSysByte&&(DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 18298)._type, 18304)._form==OTable_strShortInt||DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 18334)._type, 18340)._form==OTable_strChar)))||(_fform==OTable_strSysPtr&&DEREF(OTable_Struct, DEREF(OEParse_Node, _apar, 18403)._type, 18409)._form==OTable_strPointer)))  {
            OEParse_ErrT1(DEREF(OEParse_Node, _apar, 18463)._pos, 168, &(_ftype));
          }
          if (IN(DEREF(OEParse_Node, _apar, 18580)._class, 0x0000030EU, 18588))  {
            INCL(DEREF(OTable_Object, DEREF(OEParse_Node, _apar, 18676)._obj, 18681)._flags, OTable_flagAddressed, 18666);
          }
        }
        _fpar = DEREF(OTable_Object, _fpar, 18760)._link;
      }
      _apar = DEREF(OEParse_Node, _apar, 18800)._link;
    }
    if ((void*)_fpar!=(void*)NULL&&DEREF(OTable_Struct, DEREF(OTable_Object, _fpar, 18847)._type, 18853)._form!=OTable_strNone)  {
      OScan_Err(_endOfParams, 251);
    }
    DEREF(OEParse_Node, (* _call), 18974)._type = DEREF(OTable_Struct, DEREF(OEParse_Node, _proc, 18988)._type, 18994)._base;
  } else {
    DEREF(OEParse_Node, (* _call), 19020)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 19043);
  }
  DEREF(OEParse_Node, (* _call), 19073)._pos = DEREF(OEParse_Node, _proc, 19086)._pos;
}

static void OParse_Expr (OEParse_Node (* _r)) {
  BOOLEAN _ok;

  void _DOp (OEParse_Node (* _r), SHORTINT _sc) {
    OEParse_Node _n;
    _n = (* _r);
    (* _r) = OEParse_NewNode(OEParse_ndDOp);
    DEREF(OEParse_Node, (* _r), 19734)._left = _n;
    DEREF(OEParse_Node, (* _r), 19753)._subcl = _sc;
    OScan_GetSym();
  }

  void _SimpleExpr (OEParse_Node (* _r)) {

    void _Term (OEParse_Node (* _r)) {

      void _Factor (OEParse_Node (* _r)) {

        void _Element (OEParse_Node (* _range)) {
          LONGINT _i;

          void _Limit (OEParse_Node (* _limit)) {
            OParse_Expr(_limit);
            OEParse_ChkType((* _limit), OEParse_grpInteger);
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _limit), 20780)._type, 20786)._form!=OTable_strUndef&&DEREF(OEParse_Node, (* _limit), 20815)._class==OEParse_ndConst)  {
              OEParse_ChkRange((* _limit), 0, 31, 229);
            }
          }
          (* _range) = OEParse_NewNode(OEParse_ndUpto);
          _Limit(&(DEREF(OEParse_Node, (* _range), 21026)._left));
          if (OScan_sym==OScan_upto)  {
            OScan_GetSym();
            _Limit(&(DEREF(OEParse_Node, (* _range), 21187)._right));
          } else {
            DEREF(OEParse_Node, (* _range), 21284)._right = DEREF(OEParse_Node, (* _range), 21300)._left;
          }
          if (DEREF(OEParse_Node, DEREF(OEParse_Node, (* _range), 21345)._left, 21351)._class==OEParse_ndConst&&DEREF(OEParse_Node, DEREF(OEParse_Node, (* _range), 21378)._right, 21385)._class==OEParse_ndConst)  {
            if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _range), 21459)._left, 21465)._conval, 21473)._intval>DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _range), 21489)._right, 21496)._conval, 21504)._intval)  {
              OScan_Err(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _range), 21579)._left, 21585)._pos, 230);
            }
            {
              LONGINT _temp_ = DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _range), 21780)._right, 21787)._conval, 21795)._intval;
              for(_i = DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _range), 21749)._left, 21755)._conval, 21763)._intval ; _i <= _temp_ ; _i += 1) {
                INCL(DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _range), 21834)._left, 21840)._conval, 21848)._set, _i, 21823);
              }
            }
            (* _range) = DEREF(OEParse_Node, (* _range), 21905)._left;
            DEREF(OEParse_Node, (* _range), 21932)._obj = (OTable_Object)NULL;
          }
          DEREF(OEParse_Node, (* _range), 21979)._type = INDEX(OTable_predeclType, OTable_strSet, 14, 22002);
        }
        if (OScan_sym<OScan_lParen)  {
          OEParse_Err(147);
          do  {
            OScan_GetSym();
          } while (!(OScan_sym>=OScan_lParen));
        }
        if (OScan_sym==OScan_ident)  {
          OParse_Designator(_r);
          if (OScan_sym==OScan_lParen)  {
            OParse_ProcCall(_r, (* _r));
            if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 22640)._type, 22646)._form==OTable_strNone&&DEREF(OTable_Struct, DEREF(OEParse_Node, (* _r), 22687)._type, 22693)._form!=OTable_strUndef)  {
              OScan_Err(DEREF(OEParse_Node, (* _r), 22811)._pos, 265);
            }
          }
        } else if (OScan_sym==OScan_number)  {
          (* _r) = OEParse_NewNode(OEParse_ndConst);
          DEREF(OEParse_Node, (* _r), 23036)._conval = OTable_NewConst();
          {
            SHORTINT _temp_ = OScan_numType;
            switch (_temp_) {
              case 1: {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 23131)._conval, 23139)._intval = OScan_intVal;
                DEREF(OEParse_Node, (* _r), 23176)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 23199);
                break;
              }
              case 2: {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 23254)._conval, 23262)._real = OScan_realVal;
                DEREF(OEParse_Node, (* _r), 23298)._type = INDEX(OTable_predeclType, OTable_strReal, 14, 23321);
                break;
              }
              case 3: {
                DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 23374)._conval, 23382)._real = OScan_realVal;
                DEREF(OEParse_Node, (* _r), 23418)._type = INDEX(OTable_predeclType, OTable_strLongReal, 14, 23441);
                break;
              }
              default: NO_LABEL (_temp_, 23074);
            }
          }
          OEParse_SetInt((* _r));
          OScan_GetSym();
        } else if (OScan_sym==OScan_string)  {
          (* _r) = OEParse_NewNode(OEParse_ndConst);
          DEREF(OEParse_Node, (* _r), 23695)._conval = OTable_NewConst();
          if (Strings_Length(256, (CHAR *) OScan_ref)<=1)  {
            DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 23782)._conval, 23790)._intval = OScan_intVal;
            DEREF(OEParse_Node, (* _r), 23845)._type = INDEX(OTable_predeclType, OTable_strChar, 14, 23868);
          } else {
            NEWFIX(DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 24022)._conval, 24030)._string, 1);
            COPY(OScan_ref, DEREF(OTable_String, DEREF(OTable_Const, DEREF(OEParse_Node, (* _r), 24069)._conval, 24077)._string, 24085), 256);
            DEREF(OEParse_Node, (* _r), 24124)._type = INDEX(OTable_predeclType, OTable_strString, 14, 24147);
          }
          OScan_GetSym();
        } else if (OScan_sym==OScan_nil)  {
          (* _r) = OEParse_NewNode(OEParse_ndConst);
          DEREF(OEParse_Node, (* _r), 24335)._conval = OTable_NewConst();
          DEREF(OEParse_Node, (* _r), 24374)._type = INDEX(OTable_predeclType, OTable_strNil, 14, 24397);
          OScan_GetSym();
        } else if (OScan_sym==OScan_lBrace)  {
          OScan_GetSym();
          if (OScan_sym!=OScan_rBrace)  {
            _Element(_r);
            while (OScan_sym==OScan_comma)  {
              _DOp(_r, OEParse_scPlus);
              _Element(&(DEREF(OEParse_Node, (* _r), 24931)._right));
              OEParse_Check(_r);
            }
          } else {
            (* _r) = OEParse_NewNode(OEParse_ndConst);
            DEREF(OEParse_Node, (* _r), 25116)._type = INDEX(OTable_predeclType, OTable_strSet, 14, 25139);
            DEREF(OEParse_Node, (* _r), 25166)._conval = OTable_NewConst();
          }
          OParse_CheckSym(OScan_rBrace);
        } else if (OScan_sym==OScan_lParen)  {
          OScan_GetSym();
          OParse_Expr(_r);
          OParse_CheckSym(OScan_rParen);
        } else if (OScan_sym==OScan_not)  {
          (* _r) = OEParse_NewNode(OEParse_ndMOp);
          DEREF(OEParse_Node, (* _r), 25498)._subcl = OEParse_scNot;
          OScan_GetSym();
          _Factor(&(DEREF(OEParse_Node, (* _r), 25561)._left));
          OEParse_Check(_r);
        } else {
          OEParse_Err(147);
          (* _r) = OEParse_NewNode(OEParse_ndVar);
          DEREF(OEParse_Node, (* _r), 25845)._type = INDEX(OTable_predeclType, OTable_strUndef, 14, 25868);
          OScan_GetSym();
        }
      }
      _Factor(_r);
      while(1) {
        if (OScan_sym<OScan_times||OScan_sym>OScan_and)  {
          goto _exit26036;
        }
        _DOp(_r, OScan_sym);
        _Factor(&(DEREF(OEParse_Node, (* _r), 26228)._right));
        OEParse_Check(_r);
      }
      _exit26036: ;
    }
    if (OScan_sym==OScan_minus)  {
      (* _r) = OEParse_NewNode(OEParse_ndMOp);
      DEREF(OEParse_Node, (* _r), 26393)._subcl = OEParse_scMinus;
      OScan_GetSym();
      _Term(&(DEREF(OEParse_Node, (* _r), 26448)._left));
      OEParse_Check(_r);
    } else {
      if (OScan_sym==OScan_plus)  {
        OScan_GetSym();
      }
      _Term(_r);
    }
    while(1) {
      if (OScan_sym<OScan_plus||OScan_sym>OScan_or)  {
        goto _exit26626;
      }
      _DOp(_r, OScan_sym);
      _Term(&(DEREF(OEParse_Node, (* _r), 26802)._right));
      OEParse_Check(_r);
    }
    _exit26626: ;
  }
  _SimpleExpr(_r);
  if (OScan_sym<OScan_eql||OScan_sym>OScan_is)  {
    return;
  }
  _DOp(_r, OScan_sym);
  _SimpleExpr(&(DEREF(OEParse_Node, (* _r), 27023)._right));
  if (DEREF(OEParse_Node, (* _r), 27042)._subcl==OEParse_scIs)  {
    DEREF(OEParse_Node, (* _r), 27120)._type = INDEX(OTable_predeclType, OTable_strBool, 14, 27143);
    _ok = OEParse_ChkGuard(DEREF(OEParse_Node, (* _r), 27181)._left, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 27190)._right, 27197)._obj, FALSE, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _r), 27212)._right, 27219)._pos);
  } else {
    OEParse_Check(_r);
  }
}

static void OParse_StatementSeq (OEParse_Node (* _r), OEParse_Node _proc, OEParse_Node _loop) {
  OEParse_Node _last;

  void _Statement (OEParse_Node (* _node)) {
    OEParse_Node _n;
    LONGINT _pos;
    BOOLEAN _comp;
    OTable_Struct _prevType;

    void _CaseLabels (OEParse_Node (* _list), OTable_Const _range) {

      void _Overlapping (LONGINT _lower, LONGINT _upper) {
        OEParse_Node _branch, _label;
        _branch = DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 28263)._right, 28270)._left;
        while ((void*)_branch!=(void*)NULL)  {
          _label = DEREF(OEParse_Node, _branch, 28369)._link;
          while ((void*)_label!=(void*)NULL)  {
            if (!((_upper<DEREF(OTable_Const, DEREF(OEParse_Node, _label, 28489)._conval, 28497)._intval||_lower>DEREF(OTable_Const, DEREF(OEParse_Node, _label, 28524)._conval, 28532)._intval2)||_label==(* _list)))  {
              OScan_Err(DEREF(OEParse_Node, (* _list), 28592)._pos, 252);
              return;
            }
            _label = DEREF(OEParse_Node, _label, 28706)._link;
          }
          _branch = DEREF(OEParse_Node, _branch, 28758)._left;
        }
      }

      void _Limit (LONGINT (* _limit)) {
        OTable_Const _const;
        OTable_Struct _type;
        OParse_ConstExpr(&(_const), &(_type));
        (* _limit) = DEREF(OTable_Const, _const, 29063)._intval;
        if ((IN(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 29155)._left, 29161)._type, 29167)._form, OEParse_intSet, 29174)&&(!IN(DEREF(OTable_Struct, _type, 29209)._form, OEParse_intSet, 29216)||DEREF(OTable_Struct, _type, 29237)._form>DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 29250)._left, 29256)._type, 29262)._form))||(DEREF(OTable_Struct, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 29293)._left, 29299)._type, 29305)._form==OTable_strChar&&DEREF(OTable_Struct, _type, 29332)._form!=OTable_strChar))  {
          OEParse_Err(253);
        }
        if ((* _limit)<DEREF(OTable_Const, _range, 29525)._intval)  {
          DEREF(OTable_Const, _range, 29557)._intval = (* _limit);
        }
        if ((* _limit)>DEREF(OTable_Const, _range, 29617)._intval2)  {
          DEREF(OTable_Const, _range, 29650)._intval2 = (* _limit);
        }
      }
      (* _list) = OEParse_NewNode(OEParse_ndConst);
      DEREF(OEParse_Node, (* _list), 29766)._conval = OTable_NewConst();
      DEREF(OEParse_Node, (* _list), 29804)._type = DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 29818)._left, 29824)._type;
      _Limit(&(DEREF(OTable_Const, DEREF(OEParse_Node, (* _list), 29851)._conval, 29859)._intval));
      if (OScan_sym==OScan_upto)  {
        OScan_GetSym();
        _Limit(&(DEREF(OTable_Const, DEREF(OEParse_Node, (* _list), 29989)._conval, 29997)._intval2));
      } else {
        DEREF(OTable_Const, DEREF(OEParse_Node, (* _list), 30092)._conval, 30100)._intval2 = DEREF(OTable_Const, DEREF(OEParse_Node, (* _list), 30117)._conval, 30125)._intval;
      }
      _Overlapping(DEREF(OTable_Const, DEREF(OEParse_Node, (* _list), 30172)._conval, 30180)._intval, DEREF(OTable_Const, DEREF(OEParse_Node, (* _list), 30194)._conval, 30202)._intval2);
      if (OScan_sym==OScan_comma)  {
        OScan_GetSym();
        _CaseLabels(&(DEREF(OEParse_Node, (* _list), 30319)._link), _range);
      }
    }

    void _BoolExpr (OEParse_Node (* _n)) {
      OParse_Expr(_n);
      OEParse_ChkType((* _n), OTable_strBool);
    }

    void _RemoveBranch (OEParse_Node (* _n)) {
      if ((void*)(* _n)!=(void*)NULL)  {
        if (DEREF(OEParse_Node, DEREF(OEParse_Node, (* _n), 31003)._left, 31009)._class==OEParse_ndConst)  {
          if (DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _n), 31052)._left, 31058)._conval, 31066)._intval==0)  {
            (* _n) = DEREF(OEParse_Node, (* _n), 31127)._link;
            _RemoveBranch(_n);
          } else {
            DEREF(OEParse_Node, (* _n), 31219)._link = (OEParse_Node)NULL;
            DEREF(OEParse_Node, (* _node), 31299)._right = DEREF(OEParse_Node, (* _n), 31311)._right;
            (* _n) = (OEParse_Node)NULL;
          }
        } else {
          _RemoveBranch(&(DEREF(OEParse_Node, (* _n), 31505)._link));
        }
      }
    }

    void _UnreachableWith (OEParse_Node (* _n)) {
      OEParse_Node _ptr;
      if ((void*)(* _n)!=(void*)NULL)  {
        _UnreachableWith(&(DEREF(OEParse_Node, (* _n), 32006)._link));
        _ptr = DEREF(OEParse_Node, (* _node), 32036)._left;
        while (_ptr!=(* _n))  {
          if (DEREF(OEParse_Node, DEREF(OEParse_Node, _ptr, 32092)._left, 32098)._obj==DEREF(OEParse_Node, DEREF(OEParse_Node, (* _n), 32107)._left, 32113)._obj&&OTable_ExtOf(DEREF(OTable_Object, DEREF(OEParse_Node, (* _n), 32186)._obj, 32191)._type, DEREF(OTable_Object, DEREF(OEParse_Node, _ptr, 32202)._obj, 32207)._type))  {
            OScan_Warn(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _n), 32442)._left, 32448)._pos, 299);
            (* _n) = DEREF(OEParse_Node, (* _n), 32481)._link;
            return;
          }
          _ptr = DEREF(OEParse_Node, _ptr, 32549)._link;
        }
      }
    }
    if (OScan_sym<OScan_ident)  {
      OEParse_Err(149);
      do  {
        OScan_GetSym();
      } while (!(OScan_sym>=OScan_ident));
    }
    if (OScan_sym==OScan_ident)  {
      OParse_Designator(&(_n));
      if ((OScan_sym==OScan_eql||OScan_sym==OScan_colon)||OScan_sym==OScan_becomes)  {
        (* _node) = OEParse_NewNode(OEParse_ndAssign);
        DEREF(OEParse_Node, (* _node), 33000)._left = _n;
        OParse_CheckSym(OScan_becomes);
        OEParse_ChkVar(_n);
        OParse_Expr(&(DEREF(OEParse_Node, (* _node), 33089)._right));
        _comp = OEParse_AssignComp(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 33135)._left, 33141)._type, DEREF(OEParse_Node, (* _node), 33153)._right);
      } else {
        OParse_ProcCall(_node, _n);
        if (DEREF(OTable_Struct, DEREF(OEParse_Node, (* _node), 33284)._type, 33290)._form!=OTable_strNone&&DEREF(OTable_Struct, DEREF(OEParse_Node, (* _node), 33330)._type, 33336)._form!=OTable_strUndef)  {
          OScan_Err(DEREF(OEParse_Node, _n, 33382)._pos, 264);
        }
      }
    } else if (OScan_sym==OScan_if)  {
      (* _node) = OEParse_NewNode(OEParse_ndIfElse);
      DEREF(OEParse_Node, (* _node), 33549)._left = OEParse_NewNode(OEParse_ndIf);
      OScan_GetSym();
      _BoolExpr(&(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 33619)._left, 33625)._left));
      OParse_CheckSym(OScan_then);
      OParse_StatementSeq(&(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 33687)._left, 33693)._right), _proc, _loop);
      _n = DEREF(OEParse_Node, (* _node), 33732)._left;
      while (OScan_sym==OScan_elsif)  {
        DEREF(OEParse_Node, _n, 33834)._link = OEParse_NewNode(OEParse_ndIf);
        _n = DEREF(OEParse_Node, _n, 33880)._link;
        OScan_GetSym();
        _BoolExpr(&(DEREF(OEParse_Node, _n, 33929)._left));
        OParse_CheckSym(OScan_then);
        OParse_StatementSeq(&(DEREF(OEParse_Node, _n, 33992)._right), _proc, _loop);
      }
      if (OScan_sym==OScan_else)  {
        OScan_GetSym();
        OParse_StatementSeq(&(DEREF(OEParse_Node, (* _node), 34105)._right), _proc, _loop);
      }
      OParse_CheckSym(OScan_end);
      _RemoveBranch(&(DEREF(OEParse_Node, (* _node), 34263)._left));
      if ((void*)DEREF(OEParse_Node, (* _node), 34330)._left==(void*)NULL)  {
        (* _node) = DEREF(OEParse_Node, (* _node), 34418)._right;
      }
    } else if (OScan_sym==OScan_case)  {
      (* _node) = OEParse_NewNode(OEParse_ndCase);
      OScan_GetSym();
      OParse_Expr(&(DEREF(OEParse_Node, (* _node), 34566)._left));
      DEREF(OEParse_Node, (* _node), 34587)._right = OEParse_NewNode(OEParse_ndCaseElse);
      _n = DEREF(OEParse_Node, (* _node), 34641)._right;
      DEREF(OEParse_Node, _n, 34659)._conval = OTable_NewConst();
      DEREF(OTable_Const, DEREF(OEParse_Node, _n, 34725)._conval, 34733)._intval = 2147483647;
      DEREF(OTable_Const, DEREF(OEParse_Node, _n, 34769)._conval, 34777)._intval2 = (-2147483647-1);
      OEParse_ChkType(DEREF(OEParse_Node, (* _node), 34828)._left, OEParse_grpIntOrChar);
      OParse_CheckSym(OScan_of);
      while(1) {
        if (OScan_sym<OScan_bar)  {
          DEREF(OEParse_Node, _n, 34936)._left = OEParse_NewNode(OEParse_ndCaseDo);
          _n = DEREF(OEParse_Node, _n, 34988)._left;
          _CaseLabels(&(DEREF(OEParse_Node, _n, 35021)._link), DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 35033)._right, 35040)._conval);
          OParse_CheckSym(OScan_colon);
          OParse_StatementSeq(&(DEREF(OEParse_Node, _n, 35110)._right), _proc, _loop);
        }
        if (OScan_sym!=OScan_bar)  {
          goto _exit34886;
        }
        OScan_GetSym();
      }
      _exit34886:
      if (OScan_sym==OScan_else)  {
        OScan_GetSym();
        DEREF(OTable_Const, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 35309)._right, 35316)._conval, 35324)._set = 0x00000002U;
        OParse_StatementSeq(&(DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 35407)._right, 35414)._right), _proc, _loop);
      }
      OParse_CheckSym(OScan_end);
    } else if (OScan_sym==OScan_while)  {
      (* _node) = OEParse_NewNode(OEParse_ndWhile);
      OScan_GetSym();
      _BoolExpr(&(DEREF(OEParse_Node, (* _node), 35608)._left));
      OParse_CheckSym(OScan_do);
      OParse_StatementSeq(&(DEREF(OEParse_Node, (* _node), 35668)._right), _proc, _loop);
      OParse_CheckSym(OScan_end);
    } else if (OScan_sym==OScan_repeat)  {
      (* _node) = OEParse_NewNode(OEParse_ndRepeat);
      OScan_GetSym();
      OParse_StatementSeq(&(DEREF(OEParse_Node, (* _node), 35857)._left), _proc, _loop);
      OParse_CheckSym(OScan_until);
      _BoolExpr(&(DEREF(OEParse_Node, (* _node), 35928)._right));
    } else if (OScan_sym==OScan_for)  {
      (* _node) = OEParse_NewNode(OEParse_ndFor);
      DEREF(OEParse_Node, (* _node), 36070)._left = OEParse_NewNode(OEParse_ndForRange);
      OScan_GetSym();
      OParse_Designator(&(_n));
      DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 36196)._left, 36202)._link = _n;
      OEParse_ChkVar(_n);
      OEParse_ChkType(_n, OEParse_grpInteger);
      if ((DEREF(OEParse_Node, _n, 36287)._class!=OEParse_ndVar&&DEREF(OEParse_Node, _n, 36310)._class!=OEParse_ndVarPar)||DEREF(OTable_Object, DEREF(OEParse_Node, _n, 36337)._obj, 36342)._mnolev<0)  {
        OScan_Err(DEREF(OEParse_Node, _n, 36379)._pos, 231);
      }
      OParse_CheckSym(OScan_becomes);
      _n = DEREF(OEParse_Node, (* _node), 36517)._left;
      OParse_Expr(&(DEREF(OEParse_Node, _n, 36540)._left));
      _comp = OEParse_AssignComp(DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 36583)._left, 36589)._link, 36595)._type, DEREF(OEParse_Node, _n, 36604)._left);
      OParse_CheckSym(OScan_to);
      OParse_Expr(&(DEREF(OEParse_Node, _n, 36726)._right));
      _comp = OEParse_AssignComp(DEREF(OEParse_Node, DEREF(OEParse_Node, DEREF(OEParse_Node, (* _node), 36770)._left, 36776)._link, 36782)._type, DEREF(OEParse_Node, _n, 36791)._right);
      if (OScan_sym==OScan_by)  {
        OScan_GetSym();
        _pos = OScan_lastSym;
        OParse_ConstExpr(&(DEREF(OEParse_Node, _n, 36995)._conval), &(DEREF(OEParse_Node, _n, 37006)._type));
        if (!IN(DEREF(OTable_Struct, DEREF(OEParse_Node, _n, 37031)._type, 37037)._form, OEParse_intSet, 37044)||DEREF(OTable_Const, DEREF(OEParse_Node, _n, 37062)._conval, 37070)._intval==0)  {
          OScan_Err(_pos, 234);
        }
      } else {
        DEREF(OEParse_Node, _n, 37246)._conval = OTable_NewConst();
        DEREF(OTable_Const, DEREF(OEParse_Node, _n, 37283)._conval, 37291)._intval = 1;
        DEREF(OEParse_Node, _n, 37317)._type = INDEX(OTable_predeclType, OTable_strLongInt, 14, 37340);
        OEParse_SetInt(_n);
      }
      OParse_CheckSym(OScan_do);
      OParse_StatementSeq(&(DEREF(OEParse_Node, (* _node), 37485)._right), _proc, _loop);
      OParse_CheckSym(OScan_end);
    } else if (OScan_sym==OScan_loop)  {
      (* _node) = OEParse_NewNode(OEParse_ndLoop);
      OScan_GetSym();
      OParse_StatementSeq(&(DEREF(OEParse_Node, (* _node), 37668)._left), _proc, (* _node));
      OParse_CheckSym(OScan_end);
    } else if (OScan_sym==OScan_with)  {
      (* _node) = OEParse_NewNode(OEParse_ndWithElse);
      DEREF(OEParse_Node, (* _node), 37822)._conval = OTable_NewConst();
      _n = (* _node);
      DEREF(OEParse_Node, (* _node), 37879)._left = OEParse_NewNode(OEParse_ndWithGuard);
      _n = DEREF(OEParse_Node, (* _node), 37933)._left;
      while(1) {
        OScan_GetSym();
        OParse_Designator(&(DEREF(OEParse_Node, _n, 37997)._left));
        OParse_CheckSym(OScan_colon);
        OParse_Qualident(&(DEREF(OEParse_Node, _n, 38058)._obj), &(_pos));
        _comp = OEParse_ChkGuard(DEREF(OEParse_Node, _n, 38102)._left, DEREF(OEParse_Node, _n, 38111)._obj, TRUE, _pos);
        OParse_CheckSym(OScan_do);
        if (_comp)  {
          _prevType = DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 38206)._left, 38212)._obj, 38217)._type;
          DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 38238)._left, 38244)._obj, 38249)._type = DEREF(OTable_Object, DEREF(OEParse_Node, _n, 38260)._obj, 38265)._type;
          OParse_StatementSeq(&(DEREF(OEParse_Node, _n, 38300)._right), _proc, _loop);
          DEREF(OTable_Object, DEREF(OEParse_Node, DEREF(OEParse_Node, _n, 38335)._left, 38341)._obj, 38346)._type = _prevType;
        } else {
          OParse_StatementSeq(&(DEREF(OEParse_Node, _n, 38407)._right), _proc, _loop);
        }
        if (OScan_sym!=OScan_bar)  {
          goto _exit37949;
        }
        DEREF(OEParse_Node, _n, 38518)._link = OEParse_NewNode(OEParse_ndWithGuard);
        _n = DEREF(OEParse_Node, _n, 38571)._link;
      }
      _exit37949:
      if (OScan_sym==OScan_else)  {
        OScan_GetSym();
        DEREF(OTable_Const, DEREF(OEParse_Node, (* _node), 38656)._conval, 38664)._set = 0x00000002U;
        OParse_StatementSeq(&(DEREF(OEParse_Node, (* _node), 38706)._right), _proc, _loop);
      }
      OParse_CheckSym(OScan_end);
      _UnreachableWith(&(DEREF(OEParse_Node, (* _node), 38795)._left));
    } else if (OScan_sym==OScan_exit)  {
      if ((void*)_loop==(void*)NULL)  {
        OEParse_Err(239);
      }
      (* _node) = OEParse_NewNode(OEParse_ndExit);
      DEREF(OEParse_Node, (* _node), 39042)._left = _loop;
      OScan_GetSym();
    } else if (OScan_sym==OScan_return)  {
      (* _node) = OEParse_NewNode(OEParse_ndReturn);
      DEREF(OEParse_Node, (* _node), 39185)._obj = DEREF(OEParse_Node, _proc, 39198)._obj;
      OScan_GetSym();
      if (OScan_sym<OScan_semicolon)  {
        if ((void*)DEREF(OEParse_Node, _proc, 39279)._obj==(void*)NULL||DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _proc, 39298)._obj, 39303)._type, 39309)._base, 39315)._form==OTable_strNone)  {
          OEParse_Err(240);
        }
        OParse_Expr(&(DEREF(OEParse_Node, (* _node), 39440)._left));
      }
      if ((void*)DEREF(OEParse_Node, _proc, 39477)._obj!=(void*)NULL&&DEREF(OTable_Struct, DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _proc, 39495)._obj, 39500)._type, 39506)._base, 39512)._form!=OTable_strNone)  {
        if ((void*)DEREF(OEParse_Node, (* _node), 39586)._left!=(void*)NULL)  {
          _comp = OEParse_AssignComp(DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, _proc, 39682)._obj, 39687)._type, 39693)._base, DEREF(OEParse_Node, (* _node), 39705)._left);
        } else {
          OEParse_Err(241);
        }
      }
    }
  }
  _last = (OEParse_Node)NULL;
  while(1) {
    if ((void*)_last!=(void*)NULL)  {
      _Statement(&(DEREF(OEParse_Node, _last, 39997)._link));
    } else {
      _Statement(_r);
      _last = (* _r);
    }
    if ((void*)_last!=(void*)NULL)  {
      while ((void*)DEREF(OEParse_Node, _last, 40189)._link!=(void*)NULL)  {
        _last = DEREF(OEParse_Node, _last, 40228)._link;
      }
    }
    if ((OScan_sym!=OScan_semicolon&&OScan_sym!=OScan_ident)&&(OScan_if>OScan_sym||OScan_sym>OScan_return))  {
      goto _exit39869;
    }
    OParse_CheckSym(OScan_semicolon);
  }
  _exit39869: ;
}

static void OParse_IdentDef (OTable_Object (* _obj), SHORTINT _mode, SHORTINT _exp) {
  (* _obj) = OParse_NewObject(256, (CHAR *) OScan_ref, _mode);
  OScan_GetSym();
  if (OScan_sym==OScan_times||OScan_sym==OScan_minus)  {
    if (_exp<=OTable_exportNot)  {
      OEParse_Err(201);
    } else if (_exp==OTable_exportRead&&OScan_sym==OScan_minus)  {
      OEParse_Err(202);
    }
    if (OScan_sym==OScan_minus)  {
      DEREF(OTable_Object, (* _obj), 41511)._mark = OTable_exportRead;
    } else {
      DEREF(OTable_Object, (* _obj), 41556)._mark = OTable_exportWrite;
    }
    OScan_GetSym();
    if ((OTable_external&&IN(_mode, 0x00000210U, 41689))&&OScan_sym==OScan_lBrak)  {
      OScan_GetSym();
      NEWFIX(DEREF(OTable_Object, (* _obj), 41788)._extName, 1);
      COPY(OScan_ref, DEREF(OTable_String, DEREF(OTable_Object, (* _obj), 41824)._extName, 41833), 256);
      OParse_CheckSym(OScan_string);
      OParse_CheckSym(OScan_rBrak);
    }
  }
}

static void OParse_IdentList (OTable_Object (* _first), OTable_Object (* _last), SHORTINT _mode, SHORTINT _exp) {
  OParse_IdentDef(_first, _mode, _exp);
  (* _last) = (* _first);
  while(1) {
    if (OScan_sym==OScan_comma)  {
      OScan_GetSym();
    } else if (OScan_sym==OScan_ident)  {
      OEParse_Err(119);
    } else {
      goto _exit42256;
    }
    if (OScan_sym==OScan_ident)  {
      OParse_IdentDef(&(DEREF(OTable_Object, (* _last), 42469)._link), _mode, _exp);
      (* _last) = DEREF(OTable_Object, (* _last), 42509)._link;
    } else {
      OEParse_Err(100);
      OScan_GetSym();
    }
  }
  _exit42256: ;
}

static void OParse_Type (OTable_Struct (* _t), LONGINT (* _pos), SHORTINT _exp) {
  OTable_Object _obj, _first, _last;
  OEParse_Node _init;
  LONGINT _p;

  void _ArrayType (OTable_Struct (* _t)) {
    OTable_Const _const;
    OTable_Struct _t0;
    LONGINT _pos;
    _pos = OScan_lastSym;
    (* _t) = OParse_NewStruct(OTable_strArray);
    OParse_ConstExpr(&(_const), &(_t0));
    if (!IN(DEREF(OTable_Struct, _t0, 43527)._form, OEParse_intSet, 43534))  {
      OScan_Err(_pos, 207);
    } else if (DEREF(OTable_Const, _const, 43632)._intval<=0)  {
      OScan_Err(_pos, 206);
    }
    DEREF(OTable_Struct, (* _t), 43723)._len = DEREF(OTable_Const, _const, 43737)._intval;
    if (OScan_sym==OScan_comma)  {
      OScan_GetSym();
      _ArrayType(&(DEREF(OTable_Struct, (* _t), 43845)._base));
    } else {
      OParse_CheckSym(OScan_of);
      OParse_Type(&(DEREF(OTable_Struct, (* _t), 43938)._base), &(_pos), _exp);
      if (DEREF(OTable_Struct, DEREF(OTable_Struct, (* _t), 43970)._base, 43976)._form==OTable_strDynArray)  {
        OScan_Err(_pos, 217);
      }
    }
    DEREF(OTable_Struct, (* _t), 44094)._size = OTable_structSize((* _t));
  }
  (* _pos) = OScan_lastSym;
  if (OScan_sym==OScan_ident)  {
    OParse_Qualident(&(_obj), _pos);
    if (DEREF(OTable_Object, _obj, 44263)._mode!=OTable_objType)  {
      OScan_Err((* _pos), 205);
      (* _t) = INDEX(OTable_predeclType, OTable_strUndef, 14, 44360);
    } else {
      (* _t) = DEREF(OTable_Object, _obj, 44400)._type;
    }
  } else if (OScan_sym==OScan_array)  {
    OScan_GetSym();
    if (OScan_sym==OScan_of)  {
      OScan_GetSym();
      (* _t) = OParse_NewStruct(OTable_strDynArray);
      OParse_Type(&(DEREF(OTable_Struct, (* _t), 44607)._base), &(_p), _exp);
      if (OTable_external&&DEREF(OTable_Struct, DEREF(OTable_Struct, (* _t), 44650)._base, 44656)._form==OTable_strDynArray)  {
        OScan_Err(_p, 217);
      }
      DEREF(OTable_Struct, (* _t), 44766)._size = OTable_structSize((* _t));
    } else {
      _ArrayType(_t);
    }
  } else if (OScan_sym==OScan_record)  {
    (* _t) = OParse_NewStruct(OTable_strRecord);
    OScan_GetSym();
    if (OTable_external&&OScan_sym==OScan_lBrak)  {
      OScan_GetSym();
      if (OScan_sym==OScan_ident&&STREQL(OScan_ref, "UNION"))  {
        INCL(DEREF(OTable_Struct, (* _t), 45183)._flags, OTable_flagUnion, 45176);
      } else {
        OScan_Err(-1, 103);
      }
      OScan_GetSym();
      OParse_CheckSym(OScan_rBrak);
    } else if (!OTable_external&&OScan_sym==OScan_lParen)  {
      OScan_GetSym();
      OParse_Qualident(&(_obj), &(_p));
      if ((DEREF(OTable_Object, _obj, 45482)._mode==OTable_objType&&DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 45508)._type, 45514)._form==OTable_strRecord)&&!IN(OTable_flagUnion, DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 45570)._type, 45576)._flags, 45564))  {
        DEREF(OTable_Struct, (* _t), 45601)._base = DEREF(OTable_Object, _obj, 45614)._type;
        DEREF(OTable_Struct, (* _t), 45633)._len = DEREF(OTable_Struct, DEREF(OTable_Struct, (* _t), 45643)._base, 45649)._len+1;
        if (IN(OTable_flagExternal, DEREF(OTable_Struct, DEREF(OTable_Struct, (* _t), 45691)._base, 45697)._flags, 45687))  {
          OScan_Err(_p, 152);
        }
      } else {
        OScan_Err(_p, 209);
      }
      OParse_CheckSym(OScan_rParen);
    }
    while(1) {
      if (OScan_sym==OScan_ident)  {
        OParse_IdentList(&(_first), &(_last), OTable_objField, _exp);
        OParse_CheckSym(OScan_colon);
        OParse_Type(&(DEREF(OTable_Object, _last, 46113)._type), &(_p), _exp);
        if (DEREF(OTable_Struct, DEREF(OTable_Object, _last, 46184)._type, 46190)._form==OTable_strDynArray)  {
          OScan_Err(_p, 217);
        }
        _obj = _first;
        while ((void*)_obj!=(void*)NULL)  {
          _first = DEREF(OTable_Object, _obj, 46470)._link;
          DEREF(OTable_Object, _obj, 46493)._type = DEREF(OTable_Object, _last, 46507)._type;
          DEREF(OTable_Object, _obj, 46530)._link = (OTable_Object)NULL;
          OTable_InsertField(_obj, (* _t));
          _obj = _first;
        }
      }
      if (OScan_sym!=OScan_semicolon)  {
        goto _exit45918;
      }
      OScan_GetSym();
    }
    _exit45918:
    OParse_CheckSym(OScan_end);
    DEREF(OTable_Struct, (* _t), 46756)._size = OTable_structSize((* _t));
    _init = OEParse_NewNode(OEParse_ndInitTd);
    DEREF(OEParse_Node, _init, 46887)._type = (* _t);
    DEREF(OEParse_Node, _init, 46910)._link = DEREF(OEParse_Node, OParse_root, 46924)._link;
    DEREF(OEParse_Node, OParse_root, 46984)._link = _init;
  } else if (OScan_sym==OScan_pointer)  {
    (* _t) = OParse_NewStruct(OTable_strPointer);
    OScan_GetSym();
    OParse_CheckSym(OScan_to);
    if (OScan_sym==OScan_ident)  {
      _obj = OTable_Find(256, (CHAR *) OScan_ref);
      if ((void*)_obj==(void*)NULL)  {
        _obj = OParse_NewObject(256, (CHAR *) OScan_ref, OTable_objForwardType);
        DEREF(OTable_Object, _obj, 47523)._type = OParse_NewStruct(OTable_strUndef);
        DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 47570)._type, 47576)._obj = _obj;
        INCL(DEREF(OTable_Object, _obj, 47609)._flags, OTable_flagUsed, 47600);
        OTable_Insert(&(_obj));
      }
      if (DEREF(OTable_Object, _obj, 47684)._mode==OTable_objForwardType)  {
        DEREF(OTable_Struct, (* _t), 47768)._base = DEREF(OTable_Object, _obj, 47781)._type;
        OScan_GetSym();
      }
    }
    if ((void*)DEREF(OTable_Struct, (* _t), 47842)._base==(void*)NULL)  {
      OParse_Type(&(DEREF(OTable_Struct, (* _t), 47942)._base), &(_p), _exp);
      if (!IN(DEREF(OTable_Struct, DEREF(OTable_Struct, (* _t), 48007)._base, 48013)._form, 0x000E0001U, 48020))  {
        OScan_Err(_p, 208);
      }
    }
    DEREF(OTable_Struct, (* _t), 48170)._size = OTable_structSize((* _t));
  } else if (OScan_sym==OScan_procedure)  {
    OScan_GetSym();
    OParse_FormalPars(_t);
  } else {
    OEParse_Err(148);
    (* _t) = INDEX(OTable_predeclType, OTable_strUndef, 14, 48369);
    OScan_GetSym();
  }
  if ((OScan_sym<OScan_semicolon||OScan_sym>OScan_else)&&OScan_sym!=OScan_rParen)  {
    OEParse_Err(146);
    while (((OScan_sym<OScan_ident||OScan_sym>OScan_else)&&OScan_sym<OScan_begin)&&OScan_sym!=OScan_rParen)  {
      OScan_GetSym();
    }
  }
}

static void OParse_DeclSeq (SHORTINT _exp, OEParse_Node (* _declProcs)) {
  OTable_Object _obj, _first, _last, _org;
  LONGINT _p;
  OEParse_Node _n;

  void _ProcDecl (OEParse_Node (* _proc)) {
    SHORTINT _mode, _rmode;
    OTable_Object _rec;
    OTable_Struct _record;
    if (OScan_sym==OScan_procedure)  {
      OScan_GetSym();
      (* _proc) = OEParse_NewNode(OEParse_ndEnter);
      _mode = OTable_objLocalProc;
      if (!OTable_external)  {
        if (OScan_sym==OScan_arrow)  {
          _mode = OTable_objForwardProc;
          OScan_GetSym();
        }
        if (OScan_sym==OScan_lParen)  {
          if (_mode==OTable_objForwardProc)  {
            _mode = OTable_objForwardTBProc;
          } else {
            _mode = OTable_objTBProc;
          }
          OScan_GetSym();
          if (OScan_sym==OScan_var)  {
            _rmode = OTable_objVarPar;
            OScan_GetSym();
          } else {
            _rmode = OTable_objVar;
          }
          if (OScan_sym==OScan_ident)  {
            _rec = OParse_NewObject(256, (CHAR *) OScan_ref, _rmode);
            OScan_GetSym();
          } else {
            _rec = OParse_NewObject(4, (CHAR *) OScan_undefStr, _rmode);
            OEParse_Err(100);
          }
          OParse_CheckSym(OScan_colon);
          OParse_Type(&(DEREF(OTable_Object, _rec, 50281)._type), &(_p), OTable_exportNot);
          _record = DEREF(OTable_Object, _rec, 50331)._type;
          if (DEREF(OTable_Struct, _record, 50361)._form==OTable_strPointer)  {
            _record = DEREF(OTable_Struct, _record, 50419)._base;
            if (_rmode!=OTable_objVar)  {
              OScan_Err(_p, 225);
            }
          } else if (DEREF(OTable_Struct, _record, 50592)._form==OTable_strRecord&&_rmode!=OTable_objVarPar)  {
            OScan_Err(_p, 226);
          }
          if (DEREF(OTable_Struct, _record, 50748)._form!=OTable_strRecord||(void*)DEREF(OTable_Struct, _record, 50780)._obj==(void*)NULL)  {
            OScan_Err(_p, 223);
            _record = INDEX(OTable_predeclType, OTable_strUndef, 14, 50894);
          } else if (DEREF(OTable_Object, DEREF(OTable_Struct, _record, 50932)._obj, 50937)._mnolev!=0)  {
            OScan_Err(_p, 224);
            _record = INDEX(OTable_predeclType, OTable_strUndef, 14, 51048);
          }
          OParse_CheckSym(OScan_rParen);
          if (OTable_LevelOfTopScope()!=0)  {
            OScan_Err(-1, 222);
          }
        }
      }
      if (OScan_sym==OScan_ident)  {
        OParse_IdentDef(&(DEREF(OEParse_Node, (* _proc), 51361)._obj), _mode, _exp- 1);
      } else {
        DEREF(OEParse_Node, (* _proc), 51408)._obj = OParse_NewObject(4, (CHAR *) OScan_undefStr, _mode);
        OScan_Err(-1, 100);
        OScan_GetSym();
      }
      if (_mode==OTable_objLocalProc&&DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 51548)._obj, 51553)._mark!=OTable_exportNot)  {
        _mode = OTable_objExtProc;
        DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 51626)._obj, 51631)._mode = OTable_objExtProc;
      }
      OParse_FormalPars(&(DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 51691)._obj, 51696)._type));
      if (_mode==OTable_objTBProc||_mode==OTable_objForwardTBProc)  {
        DEREF(OTable_Object, _rec, 51856)._link = DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 51870)._obj, 51875)._type, 51881)._link;
        DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 51903)._obj, 51908)._type, 51914)._link = _rec;
        INCLI(DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 51948)._obj, 51953)._type, 51959)._len, 1, 51939);
        DEREF(OTable_Struct, DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 51981)._obj, 51986)._type, 51992)._form = OTable_strTBProc;
        if (DEREF(OTable_Struct, _record, 52092)._form==OTable_strRecord)  {
          OTable_InsertField(DEREF(OEParse_Node, (* _proc), 52148)._obj, _record);
        }
      } else {
        OTable_Insert(&(DEREF(OEParse_Node, (* _proc), 52245)._obj));
      }
      if (_mode==OTable_objForwardProc||_mode==OTable_objForwardTBProc)  {
        DEREF(OEParse_Node, (* _proc), 52352)._class = OEParse_ndForward;
      } else if (!OTable_external)  {
        OParse_CheckSym(OScan_semicolon);
        OTable_InsertParams(DEREF(OEParse_Node, (* _proc), 52520)._obj);
        OTable_OpenScope(DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 52610)._obj, 52615)._link);
        COPYSTRING(&(OParse_undeclIdents), "");
        OParse_DeclSeq(OTable_exportNot, &(DEREF(OEParse_Node, (* _proc), 52739)._left));
        if (OScan_sym==OScan_begin)  {
          OScan_GetSym();
          OParse_StatementSeq(&(DEREF(OEParse_Node, (* _proc), 52836)._right), (* _proc), (OEParse_Node)NULL);
        }
        OParse_CheckSym(OScan_end);
        if (OScan_sym!=OScan_ident||STRNEQ(OScan_ref, DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 52946)._obj, 52951)._name))  {
          OScan_ErrIns(-1, 200, 48, (CHAR *) DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 52999)._obj, 53004)._name);
        }
        OScan_GetSym();
        if ((void*)DEREF(OEParse_Node, (* _proc), 53155)._right==(void*)NULL)  {
          INCL(DEREF(OTable_Object, DEREF(OTable_Object, DEREF(OEParse_Node, (* _proc), 53197)._obj, 53202)._link, 53208)._flags, OTable_flagEmptyBody, 53187);
        }
        OTable_CloseScope(TRUE);
      }
      OParse_CheckSym(OScan_semicolon);
      _ProcDecl(&(DEREF(OEParse_Node, (* _proc), 53346)._link));
    }
  }
  while(1) {
    if (OScan_sym==OScan_const)  {
      OScan_GetSym();
      while (OScan_sym==OScan_ident)  {
        OParse_IdentDef(&(_obj), OTable_objConst, _exp- 1);
        OParse_CheckSym(OScan_eql);
        OParse_ConstExpr(&(DEREF(OTable_Object, _obj, 53684)._const), &(DEREF(OTable_Object, _obj, 53696)._type));
        OTable_Insert(&(_obj));
        OParse_CheckSym(OScan_semicolon);
      }
    } else if (OScan_sym==OScan_type)  {
      OScan_GetSym();
      while (OScan_sym==OScan_ident)  {
        OParse_IdentDef(&(_obj), OTable_objType, _exp);
        OParse_CheckSym(OScan_eql);
        OParse_Type(&(DEREF(OTable_Object, _obj, 53972)._type), &(_p), _exp);
        if ((void*)DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 54006)._type, 54012)._obj==(void*)NULL)  {
          DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 54084)._type, 54090)._obj = _obj;
        }
        _org = _obj;
        OTable_Insert(&(_obj));
        if (_org!=_obj)  {
          _n = DEREF(OEParse_Node, OParse_root, 54356)._link;
          while ((void*)_n!=(void*)NULL)  {
            if (DEREF(OEParse_Node, _n, 54414)._type==DEREF(OTable_Object, _org, 54426)._type)  {
              DEREF(OEParse_Node, _n, 54482)._type = DEREF(OTable_Object, _obj, 54495)._type;
            }
            _n = DEREF(OEParse_Node, _n, 54562)._link;
          }
        }
        OParse_CheckSym(OScan_semicolon);
      }
    } else if (OScan_sym==OScan_var)  {
      OScan_GetSym();
      while (OScan_sym==OScan_ident)  {
        OParse_IdentList(&(_first), &(_last), OTable_objVar, _exp);
        OParse_CheckSym(OScan_colon);
        OParse_Type(&(DEREF(OTable_Object, _last, 54878)._type), &(_p), _exp);
        if (DEREF(OTable_Struct, DEREF(OTable_Object, _last, 54944)._type, 54950)._form==OTable_strDynArray)  {
          OScan_Err(_p, 217);
        }
        _obj = _first;
        while ((void*)_obj!=(void*)NULL)  {
          _first = DEREF(OTable_Object, _obj, 55216)._link;
          DEREF(OTable_Object, _obj, 55239)._type = DEREF(OTable_Object, _last, 55253)._type;
          OTable_Insert(&(_obj));
          _obj = _first;
        }
        OParse_CheckSym(OScan_semicolon);
      }
    } else {
      goto _exit53444;
    }
  }
  _exit53444:
  _ProcDecl(_declProcs);
}

void OParse_Module (OEParse_Node (* _mod)) {
  OEParse_Node _init, _n;
  CHAR _sourceName[49];

  void _ImportList (void) {
    LONGINT _pos;
    CHAR _modId[48];
    CHAR _alias[48];
    OScan_GetSym();
    while(1) {
      if (OScan_sym==OScan_ident)  {
        COPY(OScan_ref, _modId, 48);
        COPY(OScan_ref, _alias, 48);
        _pos = OScan_lastSym;
        OScan_GetSym();
        if (OScan_sym==OScan_becomes)  {
          OScan_GetSym();
          if (OScan_sym==OScan_ident)  {
            COPY(OScan_ref, _modId, 48);
            _pos = OScan_lastSym;
            OScan_GetSym();
          } else {
            OEParse_Err(100);
          }
        }
        OSym_Import(48, (CHAR *) _modId, 48, (CHAR *) _alias, _pos);
      } else {
        OEParse_Err(100);
      }
      if (OScan_sym!=OScan_comma)  {
        goto _exit55977;
      }
      OScan_GetSym();
    }
    _exit55977:
    OParse_CheckSym(OScan_semicolon);
  }
  (* _mod) = (OEParse_Node)NULL;
  OParse_root = (OEParse_Node)NULL;
  OTable_external = FALSE;
  OScan_GetSym();
  if (OScan_sym==OScan_module)  {
    OScan_GetSym();
    if (OScan_sym==OScan_ident)  {
      Filenames_GetFile(256, (CHAR *) OScan_sourceName, 49, (CHAR *) _sourceName);
      if (STRNEQ(_sourceName, OScan_ref))  {
        OScan_WarnIns(-1, 298, 49, (CHAR *) _sourceName);
      }
      OTable_compiledModule = OParse_NewObject(256, (CHAR *) OScan_ref, OTable_objModule);
      OTable_OpenScope(DEREF(OTable_Object, OTable_compiledModule, 57112)._link);
      OScan_GetSym();
      if (OScan_sym==OScan_lBrak)  {
        OTable_external = TRUE;
        OScan_underscore = OMachine_allowUnderscore;
        INCL(DEREF(OTable_Object, OTable_compiledModule, 57425)._flags, OTable_flagExternal, 57403);
        OScan_GetSym();
        NEWFIX(DEREF(OTable_Const, DEREF(OTable_Object, OTable_compiledModule, 57637)._const, 57644)._string, 1);
        COPY(OScan_ref, DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, OTable_compiledModule, 57694)._const, 57701)._string, 57709), 256);
        OParse_CheckSym(OScan_string);
        OParse_CheckSym(OScan_rBrak);
        if (OScan_sym!=OScan_ident||STRNEQ(OScan_ref, "EXTERNAL"))  {
          OScan_Err(-1, 102);
        }
        OParse_CheckSym(OScan_ident);
        OParse_CheckSym(OScan_lBrak);
        NEWFIX(DEREF(OTable_Object, OTable_compiledModule, 58086)._extName, 1);
        COPY(OScan_ref, DEREF(OTable_String, DEREF(OTable_Object, OTable_compiledModule, 58137)._extName, 58146), 256);
        OParse_CheckSym(OScan_string);
        OParse_CheckSym(OScan_rBrak);
      }
      OParse_CheckSym(OScan_semicolon);
      if (OScan_sym==OScan_import)  {
        _ImportList();
      }
      (* _mod) = OEParse_NewNode(OEParse_ndEnter);
      OParse_root = (* _mod);
      if (OScan_noerr)  {
        COPYSTRING(&(OParse_undeclIdents), "");
        OParse_DeclSeq(OTable_exportWrite, &(DEREF(OEParse_Node, (* _mod), 58699)._left));
        _init = (OEParse_Node)NULL;
        while ((void*)DEREF(OEParse_Node, (* _mod), 58794)._link!=(void*)NULL)  {
          _n = DEREF(OEParse_Node, (* _mod), 58828)._link;
          DEREF(OEParse_Node, (* _mod), 58848)._link = DEREF(OEParse_Node, DEREF(OEParse_Node, (* _mod), 58861)._link, 58867)._link;
          DEREF(OEParse_Node, _n, 58885)._link = _init;
          _init = _n;
        }
        DEREF(OEParse_Node, (* _mod), 58942)._link = _init;
        if (!OTable_external&&OScan_sym==OScan_begin)  {
          OScan_GetSym();
          OParse_StatementSeq(&(DEREF(OEParse_Node, (* _mod), 59126)._right), (* _mod), (OEParse_Node)NULL);
        }
        OParse_CheckSym(OScan_end);
        if (OScan_sym!=OScan_ident||STRNEQ(OScan_ref, DEREF(OTable_Object, OTable_compiledModule, 59248)._name))  {
          OScan_ErrIns(-1, 200, 48, (CHAR *) DEREF(OTable_Object, OTable_compiledModule, 59308)._name);
        }
        OScan_GetSym();
        OParse_CheckSym(OScan_period);
      }
      OTable_CloseScope(TRUE);
      OParse_root = (OEParse_Node)NULL;
    } else {
      OEParse_Err(100);
    }
  } else {
    OEParse_Err(101);
  }
}

void _init_OParse (void) {
  moduleId = add_module ("OParse");
}
