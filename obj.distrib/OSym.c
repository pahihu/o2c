#include "OSym.h"
#include "Files.h"
#include "OMachine.h"
#include "OScan.h"
#include "Out.h"
#include "Redir.h"
#include "Rts.h"
#include "Strings.h"

static ModuleId moduleId;


struct TD_OSym_StructList;
struct TD_OSym_ModuleList;
struct TD_OSym_at_0;

static const BOOLEAN OSym_fullSym = TRUE;
static const SHORTINT OSym_SymbolFileVersion = 4;
static const LONGINT OSym_SymbolFileBegin = 1112766833;
static const LONGINT OSym_SymbolFileEnd = 1163098481;
typedef CHAR OSym_FileName[256];
static const SHORTINT OSym_notinlist = -1;
static const INTEGER OSym_defaultStructSize = 128;
static const INTEGER OSym_maxExpStructSize = 16384;
static const SHORTINT OSym_defaultModuleSize = 16;
typedef OTable_Struct (* OSym_StructItemList);
typedef struct OSym_StructList {
  LONGINT _lastnum;
  OSym_StructItemList _items;
} OSym_StructList;
typedef OTable_Object (* OSym_ModuleItemList);
typedef struct OSym_ModuleList {
  LONGINT _lastnum;
  OSym_ModuleItemList _items;
} OSym_ModuleList;


typedef struct TD_OSym_StructList {
  TDCORE
} TD_OSym_StructList;
TD_OSym_StructList* td_OSym_StructList;
typedef struct TD_OSym_ModuleList {
  TDCORE
} TD_OSym_ModuleList;
TD_OSym_ModuleList* td_OSym_ModuleList;
typedef struct TD_OSym_at_0 {
  TDCORE
} TD_OSym_at_0;
TD_OSym_at_0* td_OSym_at_0;

static void OSym_InsertStruct (struct TD_OSym_StructList* tag__list, struct OSym_StructList (* _list), OTable_Struct _item) {
  OSym_StructItemList _newlist;
  LONGINT _index, _newlen, _oldlen;
  INCLI((* _list)._lastnum, 1, 2001);
  _oldlen = GET_LEN((* _list)._items, 0);
  if ((* _list)._lastnum>=_oldlen)  {
    if (_oldlen>=16384)  {
      _newlen = _oldlen+16384;
    } else {
      _newlen = _oldlen*2;
    }
    NEWDYN(_newlist, 1*_newlen, 1, 0);
    SET_LEN(0, _newlen);
    {
      LONGINT _temp_ = _oldlen- 1;
      for(_index = 0 ; _index <= _temp_ ; _index += 1) {
        (* PTR_INDEX(CHECK_NIL(OSym_StructItemList, _newlist, 2364), _index, GET_LEN(_newlist, 0), 1, 2365)) = (* PTR_INDEX(CHECK_NIL(OSym_StructItemList, (* _list)._items, 2384), _index, GET_LEN((* _list)._items, 0), 1, 2385));
      }
    }
    DISPOSE_DYN((* _list)._items, 1);
    (* _list)._items = _newlist;
  }
  (* PTR_INDEX(CHECK_NIL(OSym_StructItemList, (* _list)._items, 2489), (* _list)._lastnum, GET_LEN((* _list)._items, 0), 1, 2490)) = _item;
}

static void OSym_InsertPredefinedStructs (struct TD_OSym_StructList* tag__structlist, struct OSym_StructList (* _structlist)) {
  LONGINT _str;
  {
    LONGINT _temp_ = 11;
    for(_str = 0 ; _str <= _temp_ ; _str += 1) {
      OSym_InsertStruct((void*)tag__structlist, _structlist, INDEX(OTable_predeclType, _str, 14, 2713));
    }
  }
}

static void OSym_InitStructList (struct TD_OSym_StructList* tag__list, struct OSym_StructList (* _list)) {
  NEWDYN((* _list)._items, 1*OSym_defaultStructSize, 1, 0);
  SET_LEN(0, OSym_defaultStructSize);
  (* _list)._lastnum = -1;
}

static void OSym_FreeStructList (struct TD_OSym_StructList* tag__list, struct OSym_StructList (* _list)) {
  DISPOSE_DYN((* _list)._items, 1);
  (* _list)._lastnum = -1;
}

static OTable_Struct OSym_GetStruct (struct OSym_StructList _list, LONGINT _num) {
  if (_num>_list._lastnum)  {
    return NULL;
  } else {
    return (* PTR_INDEX(CHECK_NIL(OSym_StructItemList, _list._items, 3567), _num, GET_LEN(_list._items, 0), 1, 3568));
  }
  NO_RETURN (3421);
}

static LONGINT OSym_GetStructNum (struct OSym_StructList _list, OTable_Struct _item) {
  LONGINT _i;
  _i = 0;
  while (_i<_list._lastnum&&(* PTR_INDEX(CHECK_NIL(OSym_StructItemList, _list._items, 3752), _i, GET_LEN(_list._items, 0), 1, 3753))!=_item)  {
    INCLI(_i, 1, 3774);
  }
  if (_i<=_list._lastnum&&(* PTR_INDEX(CHECK_NIL(OSym_StructItemList, _list._items, 3831), _i, GET_LEN(_list._items, 0), 1, 3832))==_item)  {
    return _i;
  } else {
    return -1;
  }
  NO_RETURN (3613);
}

static void OSym_InitModuleList (struct TD_OSym_ModuleList* tag__list, struct OSym_ModuleList (* _list)) {
  NEWDYN((* _list)._items, 1*OSym_defaultModuleSize, 1, 0);
  SET_LEN(0, OSym_defaultModuleSize);
  (* _list)._lastnum = -1;
}

static void OSym_FreeModuleList (struct TD_OSym_ModuleList* tag__list, struct OSym_ModuleList (* _list)) {
  DISPOSE_DYN((* _list)._items, 1);
  (* _list)._lastnum = -1;
}

static void OSym_InsertModule (struct TD_OSym_ModuleList* tag__list, struct OSym_ModuleList (* _list), OTable_Object _item) {
  OSym_ModuleItemList _newlist;
  LONGINT _index, _newlen, _oldlen;
  INCLI((* _list)._lastnum, 1, 4849);
  _oldlen = GET_LEN((* _list)._items, 0);
  if ((* _list)._lastnum>=_oldlen)  {
    _newlen = _oldlen+16;
    NEWDYN(_newlist, 1*_newlen, 1, 0);
    SET_LEN(0, _newlen);
    {
      LONGINT _temp_ = _oldlen- 1;
      for(_index = 0 ; _index <= _temp_ ; _index += 1) {
        (* PTR_INDEX(CHECK_NIL(OSym_ModuleItemList, _newlist, 5084), _index, GET_LEN(_newlist, 0), 1, 5085)) = (* PTR_INDEX(CHECK_NIL(OSym_ModuleItemList, (* _list)._items, 5104), _index, GET_LEN((* _list)._items, 0), 1, 5105));
      }
    }
    DISPOSE_DYN((* _list)._items, 1);
    (* _list)._items = _newlist;
  }
  (* PTR_INDEX(CHECK_NIL(OSym_ModuleItemList, (* _list)._items, 5209), (* _list)._lastnum, GET_LEN((* _list)._items, 0), 1, 5210)) = _item;
}

static OTable_Object OSym_GetModule (struct OSym_ModuleList _list, LONGINT _num) {
  if (_num>_list._lastnum)  {
    return NULL;
  } else {
    return (* PTR_INDEX(CHECK_NIL(OSym_ModuleItemList, _list._items, 5409), _num, GET_LEN(_list._items, 0), 1, 5410));
  }
  NO_RETURN (5263);
}

static LONGINT OSym_GetModuleNum (struct OSym_ModuleList _list, OTable_Object _item) {
  LONGINT _i;
  _i = 0;
  while (_i<_list._lastnum&&(* PTR_INDEX(CHECK_NIL(OSym_ModuleItemList, _list._items, 5595), _i, GET_LEN(_list._items, 0), 1, 5596))!=_item)  {
    INCLI(_i, 1, 5617);
  }
  if (_i<=_list._lastnum&&(* PTR_INDEX(CHECK_NIL(OSym_ModuleItemList, _list._items, 5674), _i, GET_LEN(_list._items, 0), 1, 5675))==_item)  {
    return _i;
  } else {
    return -1;
  }
  NO_RETURN (5455);
}

static LONGINT OSym_GetLastModNum (struct OSym_ModuleList _list) {
  return _list._lastnum;
  NO_RETURN (5783);
}

void OSym_Export (ODepend_Module _module, BOOLEAN _force) {
  struct OSym_StructList _structBuffer;
  struct OSym_ModuleList _moduleBuffer;
  Files_File _exportFile;
  struct Files_Rider _exportRider;
  LONGINT _symbolfileKey;
  OSym_FileName _tmpFileName;
  BOOLEAN _isExternal;

  void _GenerateFingerprint (LONGINT __key_0, BYTE (* _key)) {
    const SHORTINT _fingerprintSize = 4;
    const INTEGER _blockSize = 1024;
    LONGINT _i, _last, _read, _size;
    struct Files_Rider _tmpRider;
    typedef struct OSym_at_0 {
      BYTE _fp[4];
      INTEGER _i;
      INTEGER _j;
    } OSym_at_0;
    struct OSym_at_0 _fingerprint;
    BYTE _block[1024];

    void _NextFingerprint (BYTE _ch) {
      INDEX(_fingerprint._fp, _fingerprint._i, 4, 7474) = (CHAR) (MODI((INTEGER) (*((CHAR*)&(INDEX(_fingerprint._fp, _fingerprint._i, 4, 7530))))+(INTEGER) VAL_E(CHAR, (ROTB(_ch, _fingerprint._j))), 256, 7614));
      INCI(_fingerprint._i, 1, 7632);
      if (_fingerprint._i==4)  {
        _fingerprint._j = MODI(_fingerprint._j+1, 8, 7746);
        _fingerprint._i = 0;
      }
    }
    _fingerprint._i = 0;
    _fingerprint._j = 0;
    {
      LONGINT _temp_ = 3;
      for(_i = 0 ; _i <= _temp_ ; _i += 1) {
        INDEX(_fingerprint._fp, _i, 4, 7985) = 0;
      }
    }
    _last = Files_Pos(&(_exportRider));
    Files_Set(&(_tmpRider), _exportFile, 0);
    _size = 1024;
    while(1) {
      if (_last<1024)  {
        _size = _last;
      }
      Files_ReadBytes(&(_tmpRider), (BYTE*)&(_block), _size);
      _read = _size- (LONGINT) _tmpRider.res;
      DECLI(_last, _read, 8345);
      {
        LONGINT _temp_ = _read- 1;
        for(_i = 0 ; _i <= _temp_ ; _i += 1) {
          _NextFingerprint(INDEX(_block, _i, 1024, 8424));
        }
      }
      if (_read!=_size&&_last!=0)  {
        Out_String(60, (CHAR *) "[OSym] Failed to generate right fingerprint for symbolfile ");
        Out_String(98, (CHAR *) DEREF(ODepend_Module, _module, 8645)._name);
        Out_Char('.');
        Out_String(5, (CHAR *) OMachine_symbolExtension);
        Out_Ln();
        goto _exit8120;
      }
      if (_last==0)  {
        goto _exit8120;
      }
    }
    _exit8120:
    {
      LONGINT _temp_ = 3;
      for(_i = 0 ; _i <= _temp_ ; _i += 1) {
        (* PTR_INDEX(_key, _i, __key_0, 1, 8868)) = INDEX(_fingerprint._fp, _i, 4, 8887);
      }
    }
  }

  void _WriteBoolFlag (SET _set, LONGINT _flag) {
    Files_WriteBool(&(_exportRider), IN(_flag, _set, 9044));
  }

  void _WriteIdent (LONGINT __ident_0, const CHAR (* __ident_p)) {
    CHAR (* _ident);
    INTEGER _i;
    VALUE_ARRAY(_ident, __ident_p, 1*__ident_0);
    _i = Strings_Length(__ident_0, _ident)- 1;
    if (_i>0)  {
      Files_WriteBytes(&(_exportRider), (BYTE*)_ident, (LONGINT) _i);
    }
    Files_Write(&(_exportRider), (CHAR) ((INTEGER) (* PTR_INDEX(_ident, _i, __ident_0, 1, 9421))+128));
  }

  void _WriteModule (OTable_Object _module) {
    LONGINT _num;
    OTable_Object _mod, _lastimport;
    _num = OSym_GetModuleNum(_moduleBuffer, _module);
    if (_num==-1)  {
      OSym_InsertModule((void*)td_OSym_ModuleList, &(_moduleBuffer), _module);
      Files_WriteNum(&(_exportRider), 10);
      _WriteIdent(48, (CHAR *) DEREF(OTable_Object, _module, 9826)._name);
      Files_WriteLInt(&(_exportRider), DEREF(OTable_Const, DEREF(OTable_Object, _module, 9920)._const, 9927)._intval);
      if (DEREF(OTable_Object, _module, 9957)._link!=DEREF(OTable_Object, OTable_compiledModule, 9983)._link)  {
        _mod = DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 10092)._link, 10098)._next;
        _lastimport = (OTable_Object)NULL;
        while ((void*)_mod!=(void*)NULL&&DEREF(OTable_Object, _mod, 10189)._link!=DEREF(OTable_Object, _module, 10204)._link)  {
          _lastimport = _mod;
          _mod = DEREF(OTable_Object, _mod, 10265)._next;
        }
        if ((void*)_mod==(void*)NULL)  {
          _mod = OTable_NewObject(48, (CHAR *) DEREF(OTable_Object, _module, 10481)._name, OTable_objModule, -1);
          INDEX(DEREF(OTable_Object, _mod, 10527)._name, 0, 48, 10533) = (CHAR) ((INTEGER) INDEX(DEREF(OTable_Object, _mod, 10549)._name, 0, 48, 10555)+128);
          DEREF(OTable_Object, _mod, 10607)._link = DEREF(OTable_Object, _module, 10621)._link;
          DEREF(OTable_Object, _lastimport, 10997)._next = _mod;
        }
        INCL(DEREF(OTable_Object, _mod, 11051)._flags, OTable_flagExport, 11043);
      }
    } else {
      Files_WriteNum(&(_exportRider), -_num);
    }
  }

  auto void _WriteType (OTable_Object _obj);

  void _WriteStruct (OTable_Struct _type, BOOLEAN _decl) {
    LONGINT _ref;

    void _WriteFP (OTable_Object _parlist, LONGINT _nofArgs) {
      OTable_Object _par;
      _par = _parlist;
      Files_WriteNum(&(_exportRider), _nofArgs);
      while (_nofArgs>0)  {
        Files_WriteNum(&(_exportRider), (LONGINT) DEREF(OTable_Object, _par, 11564)._mode);
        _WriteIdent(48, (CHAR *) DEREF(OTable_Object, _par, 11597)._name);
        _WriteStruct(DEREF(OTable_Object, _par, 11621)._type, TRUE);
        DECLI(_nofArgs, 1, 11645);
        _par = DEREF(OTable_Object, _par, 11677)._link;
      }
    }

    void _WriteFields (OTable_Object _field) {
      OTable_Object _walk;

      void _WriteMethods (OTable_Object _meth) {
        if ((void*)_meth!=(void*)NULL)  {
          _WriteMethods(DEREF(OTable_Object, _meth, 11933)._left);
          if (DEREF(OTable_Object, _meth, 11962)._mode==OTable_objTBProc)  {
            INCL(DEREF(OTable_Object, _meth, 12049)._flags, OTable_flagExport, 12040);
            Files_WriteNum(&(_exportRider), (LONGINT) DEREF(OTable_Object, _meth, 12118)._mode);
            _WriteIdent(48, (CHAR *) DEREF(OTable_Object, _meth, 12156)._name);
            Files_WriteNum(&(_exportRider), (LONGINT) DEREF(OTable_Object, _meth, 12210)._mark);
            _WriteStruct(DEREF(OTable_Object, _meth, 12249)._type, FALSE);
          }
          _WriteMethods(DEREF(OTable_Object, _meth, 12351)._right);
        }
      }
      _walk = _field;
      while ((void*)_walk!=(void*)NULL&&DEREF(OTable_Object, _walk, 12479)._mode==OTable_objField)  {
        Files_WriteNum(&(_exportRider), 6);
        _WriteIdent(48, (CHAR *) DEREF(OTable_Object, _walk, 12639)._name);
        Files_WriteNum(&(_exportRider), (LONGINT) DEREF(OTable_Object, _walk, 12691)._mark);
        INCL(DEREF(OTable_Object, _walk, 12794)._flags, OTable_flagExport, 12785);
        _WriteStruct(DEREF(OTable_Object, _walk, 12844)._type, TRUE);
        _walk = DEREF(OTable_Object, _walk, 12878)._next;
      }
      _WriteMethods(_field);
      Files_WriteNum(&(_exportRider), 0);
    }
    if ((void*)_type==(void*)NULL)  {
      Files_WriteNum(&(_exportRider), 0);
    } else {
      _ref = OSym_GetStructNum(_structBuffer, _type);
      if (_ref==-1)  {
        Files_WriteNum(&(_exportRider), 1);
        if (_decl&&(void*)DEREF(OTable_Struct, _type, 13290)._obj!=(void*)NULL)  {
          Files_WriteNum(&(_exportRider), 1);
          _WriteType(DEREF(OTable_Struct, _type, 13430)._obj);
          return;
        } else {
          Files_WriteNum(&(_exportRider), 0);
        }
        INCL(DEREF(OTable_Struct, _type, 13637)._flags, OTable_flagExport, 13628);
        OSym_InsertStruct((void*)td_OSym_StructList, &(_structBuffer), _type);
        if ((void*)DEREF(OTable_Struct, _type, 13723)._obj==(void*)NULL||DEREF(OTable_Object, DEREF(OTable_Struct, _type, 13744)._obj, 13749)._mnolev>=0)  {
          Files_WriteNum(&(_exportRider), (LONGINT) DEREF(OTable_Struct, _type, 13928)._form);
          _WriteBoolFlag(DEREF(OTable_Struct, _type, 13967)._flags, 10);
          {
            SHORTINT _temp_ = DEREF(OTable_Struct, _type, 14014)._form;
            switch (_temp_) {
              case 15:
              case 16: {
                _WriteFP(DEREF(OTable_Struct, _type, 14107)._link, DEREF(OTable_Struct, _type, 14118)._len);
                break;
              }
              case 17: {
                Files_WriteNum(&(_exportRider), DEREF(OTable_Struct, _type, 14200)._len);
                break;
              }
              case 18: {
                break;
              }
              case 19: {
                if (_isExternal)  {
                  _WriteBoolFlag(DEREF(OTable_Struct, _type, 14339)._flags, 9);
                }
                _WriteFields(DEREF(OTable_Struct, _type, 14425)._link);
                break;
              }
              default:  {
            }
            }
          }
          _WriteStruct(DEREF(OTable_Struct, _type, 14566)._base, TRUE);
        }
      } else {
        Files_WriteNum(&(_exportRider), -_ref);
      }
    }
  }

  void _WriteConst (OTable_Object _const) {
    Files_WriteNum(&(_exportRider), 2);
    _WriteIdent(48, (CHAR *) DEREF(OTable_Object, _const, 14886)._name);
    _WriteStruct(DEREF(OTable_Object, _const, 14918)._type, TRUE);
    if (DEREF(OTable_Struct, DEREF(OTable_Object, _const, 14947)._type, 14953)._form==OTable_strString)  {
      Files_WriteString(&(_exportRider), (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, _const, 15024)._const, 15031)._string, 15039));
    } else if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _const, 15061)._type, 15067)._form, 0x0000003EU, 15074))  {
      Files_WriteNum(&(_exportRider), DEREF(OTable_Const, DEREF(OTable_Object, _const, 15190)._const, 15197)._intval);
    } else if (IN(DEREF(OTable_Struct, DEREF(OTable_Object, _const, 15226)._type, 15232)._form, 0x000000C0U, 15239))  {
      Files_WriteLReal(&(_exportRider), DEREF(OTable_Const, DEREF(OTable_Object, _const, 15318)._const, 15325)._real);
    } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _const, 15352)._type, 15358)._form==OTable_strSet)  {
      Files_WriteSet(&(_exportRider), DEREF(OTable_Const, DEREF(OTable_Object, _const, 15423)._const, 15430)._set);
    } else if (DEREF(OTable_Struct, DEREF(OTable_Object, _const, 15456)._type, 15462)._form==OTable_strNil)  {
    } else {
      Out_String(55, (CHAR *) "ERROR: Writing of unknown constant aborted. StructNo.:");
      Out_Int((LONGINT) DEREF(OTable_Struct, DEREF(OTable_Object, _const, 15633)._type, 15639)._form, 0);
      Out_Ln();
    }
  }

  void _WriteExternalName (OTable_Object _obj) {
    if (_isExternal)  {
      Files_WriteBool(&(_exportRider), (void*)DEREF(OTable_Object, _obj, 15820)._extName!=(void*)NULL);
      if ((void*)DEREF(OTable_Object, _obj, 15852)._extName!=(void*)NULL)  {
        Files_WriteString(&(_exportRider), (CHAR *) DEREF(OTable_String, DEREF(OTable_Object, _obj, 15916)._extName, 15925));
      }
    }
  }

  void _WriteVar (OTable_Object _var) {
    Files_WriteNum(&(_exportRider), 4);
    _WriteIdent(48, (CHAR *) DEREF(OTable_Object, _var, 16114)._name);
    Files_WriteNum(&(_exportRider), (LONGINT) DEREF(OTable_Object, _var, 16159)._mark);
    _WriteExternalName(_var);
    _WriteStruct(DEREF(OTable_Object, _var, 16219)._type, TRUE);
  }

  void _WriteProc (OTable_Object _proc) {
    Files_WriteNum(&(_exportRider), (LONGINT) DEREF(OTable_Object, _proc, 16339)._mode);
    _WriteIdent(48, (CHAR *) DEREF(OTable_Object, _proc, 16369)._name);
    _WriteExternalName(_proc);
    _WriteStruct(DEREF(OTable_Object, _proc, 16431)._type, FALSE);
  }

  void _WriteType (OTable_Object _obj) {
    Files_WriteNum(&(_exportRider), 3);
    _WriteIdent(48, (CHAR *) DEREF(OTable_Object, _obj, 16623)._name);
    Files_WriteNum(&(_exportRider), (LONGINT) DEREF(OTable_Object, _obj, 16668)._mark);
    _WriteModule(OTable_GetModuleForObject(_obj));
    INCL(DEREF(OTable_Object, _obj, 16738)._flags, OTable_flagExport, 16730);
    if (DEREF(OTable_Object, _obj, 16775)._mnolev>=0)  {
      _WriteStruct(DEREF(OTable_Object, _obj, 16833)._type, _obj!=DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 16850)._type, 16856)._obj);
    }
  }

  void _WriteConstTypeVarProc (OTable_Object _obj) {
    if ((void*)_obj!=(void*)NULL)  {
      _WriteConstTypeVarProc(DEREF(OTable_Object, _obj, 17025)._left);
      if (DEREF(OTable_Object, _obj, 17049)._mark!=OTable_exportNot)  {
        if (DEREF(OTable_Object, _obj, 17094)._mode==OTable_objConst)  {
          _WriteConst(_obj);
        } else if (DEREF(OTable_Object, _obj, 17170)._mode==OTable_objVar)  {
          _WriteVar(_obj);
        } else if (IN(DEREF(OTable_Object, _obj, 17242)._mode, 0x00000180U, 17249))  {
          _WriteProc(_obj);
        } else if (DEREF(OTable_Object, _obj, 17336)._mode==OTable_objType&&!IN(OTable_flagExport, DEREF(OTable_Object, _obj, 17381)._flags, 17375))  {
          _WriteType(_obj);
        }
        INCL(DEREF(OTable_Object, _obj, 17524)._flags, OTable_flagExport, 17516);
      }
      _WriteConstTypeVarProc(DEREF(OTable_Object, _obj, 17603)._right);
    }
  }

  void _WriteHeader (void) {
    Files_WriteLInt(&(_exportRider), OSym_SymbolFileBegin);
    _WriteModule(OTable_compiledModule);
    _WriteBoolFlag(DEREF(OTable_Object, OTable_compiledModule, 17827)._flags, 10);
    _isExternal = IN(OTable_flagExternal, DEREF(OTable_Object, OTable_compiledModule, 17908)._flags, 17888);
    if (_isExternal)  {
      Files_WriteString(&(_exportRider), (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, OTable_compiledModule, 17998)._const, 18005)._string, 18013));
      Files_WriteString(&(_exportRider), (CHAR *) DEREF(OTable_String, DEREF(OTable_Object, OTable_compiledModule, 18072)._extName, 18081));
    }
  }

  BOOLEAN _DifferentSymbolFiles (void) {
    Files_File _oldSymfile;
    struct Files_Rider _oldSymrider;
    BOOLEAN _res;

    BOOLEAN _DiffHeads (void) {
      LONGINT _i;
      CHAR _ch;
      Files_ReadLInt(&(_oldSymrider), &(_i));
      if (_i!=OSym_SymbolFileBegin)  {
        return TRUE;
      }
      Files_ReadNum(&(_oldSymrider), &(_i));
      if (_i!=10)  {
        return TRUE;
      }
      do  {
        Files_Read(&(_oldSymrider), &(_ch));
      } while (!((INTEGER) _ch>=128));
      Files_ReadLInt(&(_oldSymrider), &(_i));
      return _i!=_symbolfileKey;
      NO_RETURN (18426);
    }
    _oldSymfile = Files_Old((CHAR *) INDEX(DEREF(ODepend_Module, _module, 19195)._file, ODepend_flSymExists, 5, 19201));
    if ((void*)_oldSymfile!=(void*)NULL)  {
      Files_Set(&(_oldSymrider), _oldSymfile, 0);
      _res = _DiffHeads();
      return _res;
    } else {
      return FALSE;
    }
    NO_RETURN (18139);
  }

  void _CopySymbolFile (void) {
    LONGINT _i;
    struct Files_Rider _keyRider;
    CHAR _ch;
    Files_Set(&(_keyRider), _exportFile, 0);
    Files_ReadNum(&(_keyRider), &(_i));
    Files_ReadNum(&(_keyRider), &(_i));
    do  {
      Files_Read(&(_keyRider), &(_ch));
    } while (!((INTEGER) _ch>=128));
    Files_WriteLInt(&(_keyRider), _symbolfileKey);
    Files_Register(_exportFile);
  }
  _isExternal = FALSE;
  OSym_InitModuleList((void*)td_OSym_ModuleList, &(_moduleBuffer));
  OSym_InitStructList((void*)td_OSym_StructList, &(_structBuffer));
  OSym_InsertPredefinedStructs((void*)td_OSym_StructList, &(_structBuffer));
  Redir_GeneratePathExt(OMachine_redir, 48, (CHAR *) DEREF(OTable_Object, OTable_compiledModule, 20344)._name, 5, (CHAR *) OMachine_symbolExtension, 256, (CHAR *) _tmpFileName);
  _exportFile = Files_New((CHAR *) _tmpFileName);
  if ((void*)_exportFile!=(void*)NULL)  {
    Files_Set(&(_exportRider), _exportFile, 0);
    _WriteHeader();
    _WriteConstTypeVarProc(DEREF(OTable_Object, DEREF(OTable_Object, OTable_compiledModule, 20562)._link, 20568)._link);
    Files_WriteNum(&(_exportRider), OSym_SymbolFileEnd);
    _GenerateFingerprint(sizeof(LONGINT), (BYTE*)&(_symbolfileKey));
    if (!IN(ODepend_flSymExists, DEREF(ODepend_Module, _module, 20703)._flags, 20694)||_force)  {
      _CopySymbolFile();
      INCL(DEREF(ODepend_Module, _module, 20769)._flags, ODepend_flSymChanged, 20758);
    } else if (_DifferentSymbolFiles())  {
      _CopySymbolFile();
      INCL(DEREF(ODepend_Module, _module, 20878)._flags, ODepend_flSymChanged, 20867);
      OTable_FlushSymbolTable(48, (CHAR *) DEREF(OTable_Object, OTable_compiledModule, 20949)._name);
    }
    Files_Close(&(_exportFile));
  } else {
    Out_String(39, (CHAR *) "Error: Couldn't create new symbolfile!");
    Out_Ln();
  }
  OSym_FreeStructList((void*)td_OSym_StructList, &(_structBuffer));
  OSym_FreeModuleList((void*)td_OSym_ModuleList, &(_moduleBuffer));
}

OTable_Object OSym_GetImport (LONGINT __module_0, const CHAR (* __module_p), LONGINT __importing_0, const CHAR (* __importing_p)) {
  CHAR (* _module);
  CHAR (* _importing);
  Files_File _importFile;
  struct Files_Rider _importRider;
  OTable_Object _thisModule, _importedModules;
  struct OSym_StructList _structBuffer;
  struct OSym_ModuleList _moduleBuffer;
  OSym_FileName _filename;
  BOOLEAN _found, _isExternal;
  OTable_Object _lastInserted;

  void _ReadBoolFlag (SET (* _set), LONGINT _flag) {
    BOOLEAN _x;
    Files_ReadBool(&(_importRider), &(_x));
    if (_x)  {
      INCL((* _set), _flag, 21919);
    }
  }

  void _SetBoolFlag (SET (* _set), LONGINT _flag, BOOLEAN _bool) {
    if (_bool)  {
      INCL((* _set), _flag, 22086);
    }
  }

  void _ReadIdent (LONGINT __ident_0, CHAR (* _ident)) {
    LONGINT _i;
    CHAR _ch;
    _i = 0;
    do  {
      Files_Read(&(_importRider), &(_ch));
      (* PTR_INDEX(_ident, _i, __ident_0, 1, 22598)) = _ch;
      INCLI(_i, 1, 22615);
    } while (!((INTEGER) _ch>=128));
    (* PTR_INDEX(_ident, _i- 1, __ident_0, 1, 22664)) = (CHAR) ((INTEGER) _ch- 128);
    (* PTR_INDEX(_ident, _i, __ident_0, 1, 22702)) = '\000';
  }

  BOOLEAN _ReadHeader (void) {
    LONGINT _begin, _key, _mnolev;
    CHAR _name[48];
    Files_ReadLInt(&(_importRider), &(_begin));
    if (_begin!=OSym_SymbolFileBegin)  {
      return FALSE;
    }
    Files_ReadNum(&(_importRider), &(_begin));
    if (_begin!=10)  {
      return FALSE;
    }
    _ReadIdent(48, (CHAR *) _name);
    Files_ReadLInt(&(_importRider), &(_key));
    _thisModule = OTable_NewObject(48, (CHAR *) _name, OTable_objModule, -1);
    _mnolev = OTable_GetImportMnolev();
    DEREF(OTable_Object, _thisModule, 23279)._mnolev = _mnolev;
    DEREF(OTable_Const, DEREF(OTable_Object, _thisModule, 23365)._const, 23372)._intval = _key;
    DEREF(OTable_Object, DEREF(OTable_Object, _thisModule, 23403)._link, 23409)._mnolev = _mnolev;
    _ReadBoolFlag(&(DEREF(OTable_Object, _thisModule, 23513)._flags), 10);
    _isExternal = IN(OTable_flagExternal, DEREF(OTable_Object, _thisModule, 23587)._flags, 23574);
    if (_isExternal)  {
      NEWFIX(DEREF(OTable_Const, DEREF(OTable_Object, _thisModule, 23644)._const, 23651)._string, 1);
      NEWFIX(DEREF(OTable_Object, _thisModule, 23684)._extName, 1);
      Files_ReadString(&(_importRider), (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, _thisModule, 23743)._const, 23750)._string, 23758));
      Files_ReadString(&(_importRider), (CHAR *) DEREF(OTable_String, DEREF(OTable_Object, _thisModule, 23809)._extName, 23818));
    }
    OSym_InsertModule((void*)td_OSym_ModuleList, &(_moduleBuffer), _thisModule);
    OTable_InsertSymbolTable(48, (CHAR *) _name, _thisModule);
    return TRUE;
    NO_RETURN (22743);
  }

  void _ReadModule (void) {
    LONGINT _key;
    OTable_Object _mod, _modloc;
    CHAR _ident[48];
    _ReadIdent(48, (CHAR *) _ident);
    Files_ReadLInt(&(_importRider), &(_key));
    _mod = OSym_GetImport(48, (CHAR *) _ident, __importing_0, _importing);
    if ((void*)_mod!=(void*)NULL)  {
      if (DEREF(OTable_Const, DEREF(OTable_Object, _mod, 24262)._const, 24269)._intval==_key)  {
        OSym_InsertModule((void*)td_OSym_ModuleList, &(_moduleBuffer), _mod);
        OTable_InsertSymbolTableImport(__module_0, _module, 48, (CHAR *) _ident);
        if (STRNEQ(_ident, _module))  {
          INDEX(_ident, 0, 48, 24524) = (CHAR) ((INTEGER) INDEX(_ident, 0, 48, 24542)+128);
          _modloc = OTable_NewObject(48, (CHAR *) _ident, OTable_objModule, -1);
          DEREF(OTable_Object, _modloc, 24636)._mnolev = DEREF(OTable_Object, _mod, 24649)._mnolev;
          DEREF(OTable_Object, _modloc, 24677)._link = DEREF(OTable_Object, _mod, 24688)._link;
          DEREF(OTable_Object, _modloc, 24714)._const = DEREF(OTable_Object, _mod, 24726)._const;
          DEREF(OTable_Object, _modloc, 24789)._next = _importedModules;
          _importedModules = _modloc;
        }
      } else {
        OScan_ErrIns2(-1, 53, __module_0, _module, 48, (CHAR *) _ident);
        Rts_Terminate();
      }
    } else {
      OScan_ErrIns(-1, 51, 48, (CHAR *) _ident);
      Rts_Terminate();
    }
  }

  LONGINT _ReadGetModule (void) {
    LONGINT _mnolev;
    Files_ReadNum(&(_importRider), &(_mnolev));
    if (_mnolev==10)  {
      _ReadModule();
      _mnolev = -OSym_GetLastModNum(_moduleBuffer);
    }
    return _mnolev;
    NO_RETURN (25153);
  }

  auto OTable_Struct _ReadType (void);

  OTable_Struct _ReadStruct (void) {
    OTable_Struct _type;
    LONGINT _mode, _form;

    void _ReadFP (OTable_Object (* _parlist), LONGINT (* _nofArgs)) {
      OTable_Object _last, _par;
      LONGINT _mode, _num;
      CHAR _name[48];
      _last = (OTable_Object)NULL;
      Files_ReadNum(&(_importRider), _nofArgs);
      _num = (* _nofArgs);
      while (_num>0)  {
        Files_ReadNum(&(_importRider), &(_mode));
        _ReadIdent(48, (CHAR *) _name);
        _par = OTable_NewObject(48, (CHAR *) _name, (SHORTINT) _mode, -1);
        _SetBoolFlag(&(DEREF(OTable_Object, _par, 26062)._flags), 10, _isExternal);
        DEREF(OTable_Object, _par, 26112)._mnolev = OTable_LevelOfTopScope();
        DEREF(OTable_Object, _par, 26157)._type = _ReadStruct();
        INCL(DEREF(OTable_Object, _par, 26197)._flags, OTable_flagParam, 26189);
        if ((void*)_last!=(void*)NULL)  {
          DEREF(OTable_Object, _last, 26267)._link = _par;
        } else {
          (* _parlist) = _par;
        }
        _last = _par;
        DECLI(_num, 1, 26376);
      }
    }

    void _ReadFields (OTable_Struct _scope) {
      OTable_Object _field, _method, _last;
      CHAR _name[48];
      LONGINT _mark, _mode;
      Files_ReadNum(&(_importRider), &(_mode));
      _last = (OTable_Object)NULL;
      while (_mode==6)  {
        _ReadIdent(48, (CHAR *) _name);
        if (STRNEQ(_name, "@"))  {
          Files_ReadNum(&(_importRider), &(_mark));
        } else {
          _mark = 0;
        }
        _field = OTable_NewObject(48, (CHAR *) _name, OTable_objField, -1);
        DEREF(OTable_Object, _field, 27013)._mark = (SHORTINT) _mark;
        _SetBoolFlag(&(DEREF(OTable_Object, _field, 27068)._flags), 10, _isExternal);
        DEREF(OTable_Object, _field, 27120)._mnolev = OTable_LevelOfTopScope();
        DEREF(OTable_Object, _field, 27167)._type = _ReadStruct();
        if ((void*)_last!=(void*)NULL)  {
          DEREF(OTable_Object, _last, 27236)._next = _field;
        }
        _last = _field;
        OTable_ImportInsertField(_field, _scope);
        Files_ReadNum(&(_importRider), &(_mode));
      }
      while (_mode==11)  {
        _ReadIdent(48, (CHAR *) _name);
        Files_ReadNum(&(_importRider), &(_mark));
        _method = OTable_NewObject(48, (CHAR *) _name, OTable_objTBProc, -1);
        DEREF(OTable_Object, _method, 27596)._mnolev = OTable_LevelOfTopScope();
        DEREF(OTable_Object, _method, 27644)._mark = (SHORTINT) _mark;
        DEREF(OTable_Object, _method, 27688)._type = _ReadStruct();
        INCL(DEREF(OTable_Object, DEREF(OTable_Struct, DEREF(OTable_Object, _method, 27731)._type, 27737)._link, 27743)._flags, OTable_flagReceiver, 27720);
        OTable_ImportInsertField(_method, _scope);
        Files_ReadNum(&(_importRider), &(_mode));
      }
    }
    Files_ReadNum(&(_importRider), &(_mode));
    if (_mode==0)  {
      return NULL;
    } else if (_mode<0)  {
      return OSym_GetStruct(_structBuffer, -_mode);
    } else {
      Files_ReadNum(&(_importRider), &(_mode));
      if (_mode==1)  {
        Files_ReadNum(&(_importRider), &(_mode));
        _type = _ReadType();
        return _type;
      } else {
        Files_ReadNum(&(_importRider), &(_form));
        _type = OTable_NewStruct((SHORTINT) _form);
        _ReadBoolFlag(&(DEREF(OTable_Struct, _type, 28416)._flags), 10);
        OSym_InsertStruct((void*)td_OSym_StructList, &(_structBuffer), _type);
        {
          LONGINT _temp_ = _form;
          switch (_temp_) {
            case 15:
            case 16: {
              _ReadFP(&(DEREF(OTable_Struct, _type, 28587)._link), &(DEREF(OTable_Struct, _type, 28598)._len));
              break;
            }
            case 17: {
              Files_ReadNum(&(_importRider), &(DEREF(OTable_Struct, _type, 28678)._len));
              break;
            }
            case 18: {
              break;
            }
            case 19: {
              if (_isExternal)  {
                _ReadBoolFlag(&(DEREF(OTable_Struct, _type, 28812)._flags), 9);
              }
              _ReadFields(_type);
              break;
            }
            default:  {
          }
          }
        }
        DEREF(OTable_Struct, _type, 28956)._base = _ReadStruct();
        DEREF(OTable_Struct, _type, 28992)._size = OTable_structSize(_type);
        return _type;
      }
    }
    NO_RETURN (25481);
  }

  void _ReadExternalName (OTable_Object _obj) {
    BOOLEAN _test;
    if (_isExternal)  {
      Files_ReadBool(&(_importRider), &(_test));
      if (_test)  {
        NEWFIX(DEREF(OTable_Object, _obj, 29298)._extName, 1);
        Files_ReadString(&(_importRider), (CHAR *) DEREF(OTable_String, DEREF(OTable_Object, _obj, 29352)._extName, 29361));
      }
    }
  }

  void _ReadProc (LONGINT _mode) {
    OTable_Object _proc;
    CHAR _name[48];
    _ReadIdent(48, (CHAR *) _name);
    _proc = OTable_NewObject(48, (CHAR *) _name, (SHORTINT) _mode, -1);
    _SetBoolFlag(&(DEREF(OTable_Object, _proc, 29664)._flags), 10, _isExternal);
    DEREF(OTable_Object, _proc, 29711)._mark = OTable_exportWrite;
    _ReadExternalName(_proc);
    DEREF(OTable_Object, _proc, 29775)._type = _ReadStruct();
    OTable_InsertForOSym(&(_proc), &(_lastInserted));
  }

  OTable_Struct _ReadType (void) {
    LONGINT _mark, _module;
    OTable_Object _obj;
    CHAR _name[48];
    _ReadIdent(48, (CHAR *) _name);
    Files_ReadNum(&(_importRider), &(_mark));
    _module = _ReadGetModule();
    if (_module<0)  {
      _obj = OTable_InternalFindImport(48, (CHAR *) _name, OSym_GetModule(_moduleBuffer, -_module));
    } else {
      _obj = OTable_NewObject(48, (CHAR *) _name, OTable_objType, -1);
      DEREF(OTable_Object, _obj, 30291)._mark = (SHORTINT) _mark;
      _SetBoolFlag(&(DEREF(OTable_Object, _obj, 30342)._flags), 10, _isExternal);
      DEREF(OTable_Object, _obj, 30390)._type = _ReadStruct();
      if ((void*)DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 30427)._type, 30433)._obj==(void*)NULL)  {
        DEREF(OTable_Struct, DEREF(OTable_Object, _obj, 30464)._type, 30470)._obj = _obj;
      }
      OTable_InsertForOSym(&(_obj), &(_lastInserted));
    }
    return DEREF(OTable_Object, _obj, 30584)._type;
    NO_RETURN (29871);
  }

  void _ReadVar (void) {
    LONGINT _mark;
    OTable_Object _var;
    CHAR _name[48];
    _ReadIdent(48, (CHAR *) _name);
    Files_ReadNum(&(_importRider), &(_mark));
    _var = OTable_NewObject(48, (CHAR *) _name, OTable_objVar, -1);
    DEREF(OTable_Object, _var, 30859)._mark = (SHORTINT) _mark;
    _SetBoolFlag(&(DEREF(OTable_Object, _var, 30908)._flags), 10, _isExternal);
    _ReadExternalName(_var);
    DEREF(OTable_Object, _var, 30989)._type = _ReadStruct();
    OTable_InsertForOSym(&(_var), &(_lastInserted));
  }

  void _ReadConst (void) {
    OTable_Object _const;
    OTable_Struct _type;
    CHAR _name[48];
    _ReadIdent(48, (CHAR *) _name);
    _const = OTable_NewObject(48, (CHAR *) _name, OTable_objConst, -1);
    _SetBoolFlag(&(DEREF(OTable_Object, _const, 31304)._flags), 10, _isExternal);
    DEREF(OTable_Object, _const, 31352)._type = _ReadStruct();
    DEREF(OTable_Object, _const, 31386)._mark = OTable_exportWrite;
    DEREF(OTable_Object, _const, 31422)._const = OTable_NewConst();
    _type = DEREF(OTable_Object, _const, 31463)._type;
    if (DEREF(OTable_Struct, _type, 31486)._form==OTable_strString)  {
      NEWFIX(DEREF(OTable_Const, DEREF(OTable_Object, _const, 31531)._const, 31538)._string, 1);
      Files_ReadString(&(_importRider), (CHAR *) DEREF(OTable_String, DEREF(OTable_Const, DEREF(OTable_Object, _const, 31591)._const, 31598)._string, 31606));
    } else if (IN(DEREF(OTable_Struct, _type, 31627)._form, 0x0000003EU, 31634))  {
      Files_ReadNum(&(_importRider), &(DEREF(OTable_Const, DEREF(OTable_Object, _const, 31749)._const, 31756)._intval));
    } else if (IN(DEREF(OTable_Struct, _type, 31784)._form, 0x000000C0U, 31791))  {
      Files_ReadLReal(&(_importRider), &(DEREF(OTable_Const, DEREF(OTable_Object, _const, 31869)._const, 31876)._real));
    } else if (DEREF(OTable_Struct, _type, 31902)._form==OTable_strSet)  {
      Files_ReadSet(&(_importRider), &(DEREF(OTable_Const, DEREF(OTable_Object, _const, 31966)._const, 31973)._set));
    } else if (DEREF(OTable_Struct, _type, 31998)._form==OTable_strNil)  {
    } else {
      Out_String(55, (CHAR *) "ERROR: Reading of unknown constant aborted: StructNo.:");
      Out_Int((LONGINT) DEREF(OTable_Struct, _type, 32149)._form, 0);
      Out_Ln();
    }
    OTable_InsertForOSym(&(_const), &(_lastInserted));
  }

  void _ParseSymbolFile (void) {
    LONGINT _look;
    OTable_Struct _dummy;
    Files_ReadNum(&(_importRider), &(_look));
    while (_look!=OSym_SymbolFileEnd)  {
      {
        LONGINT _temp_ = _look;
        switch (_temp_) {
          case 10: {
            _ReadModule();
            break;
          }
          case 4: {
            _ReadVar();
            break;
          }
          case 2: {
            _ReadConst();
            break;
          }
          case 3: {
            _dummy = _ReadType();
            break;
          }
          case 8:
          case 7: {
            _ReadProc(_look);
            break;
          }
          default:  {
          Out_String(45, (CHAR *) "Illegal lookahead while parsing symbolfile: ");
          Out_Int(_look, 0);
          Out_Ln();
          Out_String(51, (CHAR *) "Maybe this is an old symbolfile, please remove it.");
          Out_Ln();
          Rts_Terminate();
        }
        }
      }
      Files_ReadNum(&(_importRider), &(_look));
    }
  }

  void _InsertImportedModulesForBrowser (void) {
    OTable_Object _walk;
    while ((void*)_importedModules!=(void*)NULL)  {
      _walk = _importedModules;
      _importedModules = DEREF(OTable_Object, _importedModules, 33260)._next;
      DEREF(OTable_Object, _walk, 33279)._next = (OTable_Object)NULL;
      OTable_InsertForOSym(&(_walk), &(_lastInserted));
    }
  }
  VALUE_ARRAY(_module, __module_p, 1*__module_0);
  VALUE_ARRAY(_importing, __importing_p, 1*__importing_0);
  if (STREQL(_module, _importing))  {
    OScan_Err(OScan_lastSym, 54);
    Rts_Terminate();
  } else {
    _thisModule = OTable_GetSymbolTable(__module_0, _module);
    if ((void*)_thisModule!=(void*)NULL)  {
      return _thisModule;
    } else {
      _lastInserted = (OTable_Object)NULL;
      _found = Redir_FindPathExt(OMachine_redir, __module_0, _module, 5, (CHAR *) OMachine_symbolExtension, 256, (CHAR *) _filename);
      _importFile = Files_Old((CHAR *) _filename);
      if ((void*)_importFile==(void*)NULL)  {
        OScan_ErrIns(-1, 50, 256, (CHAR *) _filename);
        return NULL;
      }
      Files_Set(&(_importRider), _importFile, 0);
      OSym_InitModuleList((void*)td_OSym_ModuleList, &(_moduleBuffer));
      OSym_InitStructList((void*)td_OSym_StructList, &(_structBuffer));
      OSym_InsertPredefinedStructs((void*)td_OSym_StructList, &(_structBuffer));
      _importedModules = (OTable_Object)NULL;
      if (_ReadHeader())  {
        OTable_OpenScope(DEREF(OTable_Object, _thisModule, 34523)._link);
        _ParseSymbolFile();
        _InsertImportedModulesForBrowser();
        OTable_CloseScope(TRUE);
        OTable_OptimizeTree(&(DEREF(OTable_Object, DEREF(OTable_Object, _thisModule, 34782)._link, 34788)._link));
        Files_Close(&(_importFile));
        return _thisModule;
      } else {
        OScan_ErrIns(-1, 55, 256, (CHAR *) _filename);
        Rts_Terminate();
      }
      OSym_FreeStructList((void*)td_OSym_StructList, &(_structBuffer));
      OSym_FreeModuleList((void*)td_OSym_ModuleList, &(_moduleBuffer));
    }
  }
  NO_RETURN (21197);
}

void OSym_Import (LONGINT __module_0, const CHAR (* __module_p), LONGINT __alias_0, const CHAR (* __alias_p), LONGINT _pos) {
  CHAR (* _module);
  CHAR (* _alias);
  OTable_Object _cacheModule, _localModuleCopy;
  VALUE_ARRAY(_module, __module_p, 1*__module_0);
  VALUE_ARRAY(_alias, __alias_p, 1*__alias_0);
  if (STREQL(_module, DEREF(OTable_Object, OTable_compiledModule, 35299)._name))  {
    OScan_Err(_pos, 54);
  } else {
    _cacheModule = OSym_GetImport(__module_0, _module, 48, (CHAR *) DEREF(OTable_Object, OTable_compiledModule, 35482)._name);
    if ((void*)_cacheModule!=(void*)NULL)  {
      _localModuleCopy = OTable_NewObject(__alias_0, _alias, OTable_objModule, _pos);
      OTable_Insert(&(_localModuleCopy));
      DEREF(OTable_Object, _localModuleCopy, 35647)._link = DEREF(OTable_Object, _cacheModule, 35666)._link;
      DEREF(OTable_Object, _localModuleCopy, 35697)._mnolev = DEREF(OTable_Object, _cacheModule, 35718)._mnolev;
      DEREF(OTable_Const, DEREF(OTable_Object, _localModuleCopy, 35788)._const, 35795)._intval = DEREF(OTable_Const, DEREF(OTable_Object, _cacheModule, 35816)._const, 35823)._intval;
      DEREF(OTable_Object, _localModuleCopy, 35915)._flags = DEREF(OTable_Object, _cacheModule, 35935)._flags;
    }
  }
}

void _init_OSym (void) {
  moduleId = add_module ("OSym");
  td_OSym_StructList = create_td("StructList", sizeof(OSym_StructList), sizeof(TD_OSym_StructList), NULL, 0);
  td_OSym_ModuleList = create_td("ModuleList", sizeof(OSym_ModuleList), sizeof(TD_OSym_ModuleList), NULL, 0);
  td_OSym_at_0 = create_td("", (0), sizeof(TD_OSym_at_0), NULL, 0);
}
