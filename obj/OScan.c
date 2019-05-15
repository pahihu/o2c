#include "OScan.h"
#include "ConvTypes.h"
#include "Dos.h"
#include "Files.h"
#include "OMachine.h"
#include "Out.h"
#include "RealStr.h"
#include "Redir.h"
#include "Rts.h"
#include "Strings.h"
#include "Strings2.h"

static ModuleId moduleId;


struct TD_OScan_ErrMsgDesc;

static const SHORTINT OScan_maxSizeNumber = 64;
const CHAR OScan_undefStr[] = "\?\?\?";
static const CHAR OScan_nul = '\000';
static const CHAR OScan_eof = '\000';
static const CHAR OScan_eol = '\012';
static const SHORTINT OScan_sizeKWTable = 85;
static const SHORTINT OScan_offKWTable = -29;
const SHORTINT OScan_undefPos = -1;
const SHORTINT OScan_times = 1;
const SHORTINT OScan_slash = 2;
const SHORTINT OScan_div = 3;
const SHORTINT OScan_mod = 4;
const SHORTINT OScan_and = 5;
const SHORTINT OScan_plus = 6;
const SHORTINT OScan_minus = 7;
const SHORTINT OScan_or = 8;
const SHORTINT OScan_eql = 9;
const SHORTINT OScan_neq = 10;
const SHORTINT OScan_lss = 11;
const SHORTINT OScan_leq = 12;
const SHORTINT OScan_gtr = 13;
const SHORTINT OScan_geq = 14;
const SHORTINT OScan_in = 15;
const SHORTINT OScan_is = 16;
const SHORTINT OScan_arrow = 17;
const SHORTINT OScan_period = 18;
const SHORTINT OScan_comma = 19;
const SHORTINT OScan_colon = 20;
const SHORTINT OScan_upto = 21;
const SHORTINT OScan_rParen = 22;
const SHORTINT OScan_rBrak = 23;
const SHORTINT OScan_rBrace = 24;
const SHORTINT OScan_of = 25;
const SHORTINT OScan_then = 26;
const SHORTINT OScan_do = 27;
const SHORTINT OScan_to = 28;
const SHORTINT OScan_by = 29;
const SHORTINT OScan_lParen = 30;
const SHORTINT OScan_lBrak = 31;
const SHORTINT OScan_lBrace = 32;
const SHORTINT OScan_not = 33;
const SHORTINT OScan_becomes = 34;
const SHORTINT OScan_number = 35;
const SHORTINT OScan_nil = 36;
const SHORTINT OScan_string = 37;
const SHORTINT OScan_ident = 38;
const SHORTINT OScan_semicolon = 39;
const SHORTINT OScan_bar = 40;
const SHORTINT OScan_end = 41;
const SHORTINT OScan_else = 42;
const SHORTINT OScan_elsif = 43;
const SHORTINT OScan_until = 44;
const SHORTINT OScan_if = 45;
const SHORTINT OScan_case = 46;
const SHORTINT OScan_while = 47;
const SHORTINT OScan_repeat = 48;
const SHORTINT OScan_loop = 49;
const SHORTINT OScan_for = 50;
const SHORTINT OScan_with = 51;
const SHORTINT OScan_exit = 52;
const SHORTINT OScan_return = 53;
const SHORTINT OScan_array = 54;
const SHORTINT OScan_record = 55;
const SHORTINT OScan_pointer = 56;
const SHORTINT OScan_begin = 57;
const SHORTINT OScan_const = 58;
const SHORTINT OScan_type = 59;
const SHORTINT OScan_var = 60;
const SHORTINT OScan_procedure = 61;
const SHORTINT OScan_import = 62;
const SHORTINT OScan_module = 63;
const SHORTINT OScan_endOfFile = 64;
const SHORTINT OScan_numInt = 1;
const SHORTINT OScan_numReal = 2;
const SHORTINT OScan_numLReal = 3;
static const INTEGER OScan_maxBufferRest = 260;
const INTEGER OScan_minBufferSize = 520;
const INTEGER OScan_maxBufferSize = 32767;
static const INTEGER OScan_sizeErrStr = 128;
static const CHAR OScan_errorMsgsFile[] = "ErrorList.Txt";
typedef struct OScan_ErrMsgDesc (* OScan_ErrMsg);
typedef CHAR OScan_ErrStr[128];
typedef struct OScan_ErrMsgDesc {
  OScan_ErrMsg _next;
  INTEGER _num;
  OScan_ErrStr _msg;
} OScan_ErrMsgDesc;
typedef LONGINT (* OScan_PosArray);
BOOLEAN OScan_warnings, OScan_verbose, OScan_underscore;
OScan_FileName OScan_sourceName;
static Files_File OScan_inFile;
static struct Files_Rider OScan_inRider;
static LONGINT OScan_len;
static INTEGER OScan_sizeBuffer;
static CHAR (* OScan_buf)[32767];
static INTEGER OScan_pos;
static LONGINT OScan_bufOffset;
static INTEGER OScan_bufRefreshPos;
static CHAR OScan_kwStr[85][10];
static SHORTINT OScan_kwSym[85];
BOOLEAN OScan_noerr;
LONGINT OScan_lastErr;
static BOOLEAN OScan_errHeader;
static OScan_ErrMsg OScan_errs;
SHORTINT OScan_sym;
LONGINT OScan_lastSym;
CHAR OScan_ref[256];
SHORTINT OScan_numType;
LONGINT OScan_intVal;
LONGREAL OScan_realVal;
static OScan_PosArray OScan_eolArray;
static LONGINT OScan_eolSize;


typedef struct TD_OScan_ErrMsgDesc {
  TDCORE
} TD_OScan_ErrMsgDesc;
TD_OScan_ErrMsgDesc* td_OScan_ErrMsgDesc;

void OScan_VerboseMsg (LONGINT __msg_0, const CHAR (* __msg_p)) {
  CHAR (* _msg);
  VALUE_ARRAY(_msg, __msg_p, 1*__msg_0);
  if (OScan_verbose)  {
    Out_String(__msg_0, _msg);
    Out_Ln();
  }
}

void OScan_ReadErrorList (void) {
  Files_File _f;
  struct Files_Rider _r;
  CHAR _ch;
  INTEGER _num;
  OScan_ErrStr _str;
  INTEGER _i;
  OScan_ErrMsg _new;
  BOOLEAN _found, _open;
  CHAR _fileName[256];
  _found = Redir_FindPath(OMachine_redir, 14, (CHAR *) OScan_errorMsgsFile, 256, (CHAR *) _fileName);
  if (_found)  {
    _f = Files_Old((CHAR *) _fileName);
    _open = (void*)_f!=(void*)NULL;
    if (_open)  {
      Files_Set(&(_r), _f, 0);
    }
  }
  if (_found&&_open)  {
    Files_Read(&(_r), &(_ch));
    while (!_r.eof)  {
      if ('0'<=_ch&&_ch<='9')  {
        _num = 0;
        while ((!_r.eof&&'0'<=_ch)&&_ch<='9')  {
          _num = (_num*10+(INTEGER) _ch)- 48;
          Files_Read(&(_r), &(_ch));
        }
        if (_ch==':')  {
          Files_Read(&(_r), &(_ch));
          while (!_r.eof&&_ch==' ')  {
            Files_Read(&(_r), &(_ch));
          }
          _i = 0;
          while (!_r.eof&&_ch>=' ')  {
            if (_i<127)  {
              INDEX(_str, _i, 128, 5568) = _ch;
              INCI(_i, 1, 5579);
            }
            Files_Read(&(_r), &(_ch));
          }
          INDEX(_str, _i, 128, 5667) = '\000';
          NEWREC(_new, td_OScan_ErrMsgDesc, 0);
          DEREF(OScan_ErrMsg, _new, 5716)._next = OScan_errs;
          DEREF(OScan_ErrMsg, _new, 5747)._num = _num;
          COPYARRAYT(&(DEREF(OScan_ErrMsg, _new, 5776)._msg), _str, OScan_ErrStr);
          OScan_errs = _new;
        }
      } else {
        Files_Read(&(_r), &(_ch));
      }
    }
    Files_Close(&(_f));
  }
}

void OScan_GetErrMsg (INTEGER _num, LONGINT __str_0, CHAR (* _str)) {
  OScan_ErrMsg _m;
  _m = OScan_errs;
  while ((void*)_m!=(void*)NULL)  {
    if (DEREF(OScan_ErrMsg, _m, 6287)._num==_num)  {
      COPY(DEREF(OScan_ErrMsg, _m, 6320)._msg, _str, __str_0);
      return;
    }
    _m = DEREF(OScan_ErrMsg, _m, 6371)._next;
  }
  COPY("(no error message available)", _str, __str_0);
}

void OScan_Replace (LONGINT __string_0, CHAR (* _string), LONGINT __insert_0, CHAR (* _insert)) {
  INTEGER _i, _j, _l, _ls;
  _i = 0;
  while ((* PTR_INDEX(_string, _i, __string_0, 1, 6666))!='%'&&(* PTR_INDEX(_string, _i, __string_0, 1, 6686))!=OScan_nul)  {
    INCI(_i, 1, 6706);
  }
  _ls = _i;
  while ((* PTR_INDEX(_string, _ls, __string_0, 1, 6753))!=OScan_nul)  {
    INCI(_ls, 1, 6774);
  }
  if ((* PTR_INDEX(_string, _i, __string_0, 1, 6806))=='%')  {
    _l = 0;
    while ((* PTR_INDEX(_insert, _l, __insert_0, 1, 6853))!=OScan_nul)  {
      INCI(_l, 1, 6875);
    }
    _j = _ls;
    while (_j>_i)  {
      (* PTR_INDEX(_string, (_j+_l)- 1, __string_0, 1, 6946)) = (* PTR_INDEX(_string, _j, __string_0, 1, 6963));
      DECI(_j, 1, 6976);
    }
    _j = 0;
    while (_j<_l)  {
      (* PTR_INDEX(_string, _i+_j, __string_0, 1, 7046)) = (* PTR_INDEX(_insert, _j, __insert_0, 1, 7061));
      INCI(_j, 1, 7074);
    }
  }
}

static void OScan_Mark (LONGINT _pos, INTEGER _num, LONGINT __msg_0, const CHAR (* __msg_p)) {
  CHAR (* _msg);
  const SHORTINT _suppress = 12;
  VALUE_ARRAY(_msg, __msg_p, 1*__msg_0);
  if (_pos<0)  {
    _pos = OScan_lastSym;
  }
  if (ABSLI(OScan_lastErr- _pos, 7517)>=12)  {
    if (!OScan_errHeader)  {
      Out_String(9, (CHAR *) "In file ");
      Out_String(256, (CHAR *) OScan_sourceName);
      Out_String(3, (CHAR *) ": ");
      Out_Ln();
      OScan_errHeader = TRUE;
    }
    Out_Int(_pos, 0);
    Out_String(2, (CHAR *) ":");
    Out_Int((LONGINT) _num, 3);
    Out_Char(' ');
    Out_String(__msg_0, _msg);
    Out_Ln();
  }
  OScan_lastErr = _pos;
}

static void OScan_MarkIns (LONGINT _pos, INTEGER _num, LONGINT __msg_0, const CHAR (* __msg_p), LONGINT __ins_0, const CHAR (* __ins_p)) {
  CHAR (* _msg);
  CHAR (* _ins);
  CHAR _str[128];
  VALUE_ARRAY(_msg, __msg_p, 1*__msg_0);
  VALUE_ARRAY(_ins, __ins_p, 1*__ins_0);
  COPY(_msg, _str, 128);
  OScan_Replace(128, (CHAR *) _str, __ins_0, _ins);
  OScan_Mark(_pos, _num, 128, (CHAR *) _str);
}

void OScan_Err (LONGINT _pos, INTEGER _num) {
  CHAR _str[128];
  OScan_noerr = FALSE;
  OScan_GetErrMsg(_num, 128, (CHAR *) _str);
  OScan_Mark(_pos, _num, 128, (CHAR *) _str);
}

void OScan_ErrIns (LONGINT _pos, INTEGER _num, LONGINT __ins_0, const CHAR (* __ins_p)) {
  CHAR (* _ins);
  CHAR _str[128];
  VALUE_ARRAY(_ins, __ins_p, 1*__ins_0);
  OScan_noerr = FALSE;
  OScan_GetErrMsg(_num, 128, (CHAR *) _str);
  OScan_MarkIns(_pos, _num, 128, (CHAR *) _str, __ins_0, _ins);
}

void OScan_ErrIns2 (LONGINT _pos, INTEGER _num, LONGINT __ins1_0, const CHAR (* __ins1_p), LONGINT __ins2_0, const CHAR (* __ins2_p)) {
  CHAR (* _ins1);
  CHAR (* _ins2);
  CHAR _str[128];
  VALUE_ARRAY(_ins1, __ins1_p, 1*__ins1_0);
  VALUE_ARRAY(_ins2, __ins2_p, 1*__ins2_0);
  OScan_noerr = FALSE;
  OScan_GetErrMsg(_num, 128, (CHAR *) _str);
  OScan_Replace(128, (CHAR *) _str, __ins1_0, _ins1);
  OScan_MarkIns(_pos, _num, 128, (CHAR *) _str, __ins2_0, _ins2);
}

static void OScan_ErrRel (INTEGER _relPos, INTEGER _num) {
  OScan_Err(OScan_bufOffset+(LONGINT) _relPos, _num);
}

void OScan_Warn (LONGINT _pos, INTEGER _num) {
  CHAR _str[128];
  if (OScan_warnings&&OScan_noerr)  {
    OScan_lastErr = -9999;
    OScan_GetErrMsg(_num, 128, (CHAR *) _str);
    OScan_Mark(_pos, _num, 128, (CHAR *) _str);
    OScan_lastErr = -9999;
  }
}

void OScan_WarnIns (LONGINT _pos, INTEGER _num, LONGINT __ins_0, const CHAR (* __ins_p)) {
  CHAR (* _ins);
  CHAR _str[128];
  VALUE_ARRAY(_ins, __ins_p, 1*__ins_0);
  if (OScan_warnings&&OScan_noerr)  {
    OScan_lastErr = -9999;
    OScan_GetErrMsg(_num, 128, (CHAR *) _str);
    OScan_MarkIns(_pos, _num, 128, (CHAR *) _str, __ins_0, _ins);
    OScan_lastErr = -9999;
  }
}

static void OScan_ReadNextBlock (void) {
  INTEGER _left, _i;
  LONGINT _next;
  CHAR _clip[260];
  _left = OScan_sizeBuffer- OScan_pos;
  _i = 0;
  while (_i!=_left)  {
    INDEX(_clip, _i, 260, 11280) = INDEX(DEREF(CHAR (* )[32767], OScan_buf, 11290), OScan_pos+_i, 32767, 11290);
    INCI(_i, 1, 11305);
  }
  INCLI(OScan_bufOffset, (LONGINT) OScan_pos, 11326);
  OScan_pos = 0;
  _next = (OScan_len- OScan_bufOffset)- (LONGINT) _left;
  if (_next<0)  {
    {
      INTEGER _temp_ = _left- 1;
      for(_i = 0 ; _i <= _temp_ ; _i += 1) {
        INDEX(DEREF(CHAR (* )[32767], OScan_buf, 11672), _i, 32767, 11672) = INDEX(_clip, _i, 260, 11683);
      }
    }
  } else {
    if (_next>(LONGINT) (OScan_sizeBuffer- _left))  {
      _next = (LONGINT) (OScan_sizeBuffer- _left);
    }
    Files_ReadBytes(&(OScan_inRider), (BYTE*)CHECK_NIL(CHAR (* )[32767], OScan_buf, 11928), _next);
    if (_next<(LONGINT) (OScan_sizeBuffer- _left))  {
      INDEX(DEREF(CHAR (* )[32767], OScan_buf, 12068), _next, 32767, 12068) = OScan_eof;
      INCLI(_next, 1, 12091);
    }
    if (_left!=0)  {
      _i = (INTEGER) _next+_left;
      while (_i!=_left)  {
        DECI(_i, 1, 12277);
        INDEX(DEREF(CHAR (* )[32767], OScan_buf, 12299), _i, 32767, 12299) = INDEX(DEREF(CHAR (* )[32767], OScan_buf, 12309), _i- _left, 32767, 12309);
      }
      while (_i!=0)  {
        DECI(_i, 1, 12427);
        INDEX(DEREF(CHAR (* )[32767], OScan_buf, 12449), _i, 32767, 12449) = INDEX(_clip, _i, 260, 12460);
      }
    }
  }
}

static void OScan_EndOfLine (void) {
  LONGINT _i;
  OScan_PosArray _new;
  if (OScan_eolSize==GET_LEN(OScan_eolArray, 0))  {
    NEWDYN(_new, 1*(OScan_eolSize+4)*2- 4, 1, 1);
    SET_LEN(0, (OScan_eolSize+4)*2- 4);
    {
      LONGINT _temp_ = OScan_eolSize- 1;
      for(_i = 0 ; _i <= _temp_ ; _i += 1) {
        (* PTR_INDEX(CHECK_NIL(OScan_PosArray, _new, 12875), _i, GET_LEN(_new, 0), 1, 12875)) = (* PTR_INDEX(CHECK_NIL(OScan_PosArray, OScan_eolArray, 12890), _i, GET_LEN(OScan_eolArray, 0), 1, 12890));
      }
    }
    OScan_eolArray = _new;
  }
  (* PTR_INDEX(CHECK_NIL(OScan_PosArray, OScan_eolArray, 12948), OScan_eolSize, GET_LEN(OScan_eolArray, 0), 1, 12948)) = OScan_bufOffset+(LONGINT) OScan_pos;
  INCLI(OScan_eolSize, 1, 12980);
}

static LONGINT OScan_FindLine (LONGINT _pos) {
  LONGINT _line, _l, _r, _m;
  if (_pos<0)  {
    return -1;
  } else {
    _l = 0;
    _r = OScan_eolSize;
    while (_l!=_r)  {
      _m = DIVLI(_l+_r, 2, 13314);
      if (_pos>(* PTR_INDEX(CHECK_NIL(OScan_PosArray, OScan_eolArray, 13347), _m, GET_LEN(OScan_eolArray, 0), 1, 13347)))  {
        _l = _m+1;
      } else {
        _r = _m;
      }
    }
    return _l;
  }
  NO_RETURN (13022);
}

LONGINT OScan_Line (LONGINT _pos) {
  return OScan_FindLine(_pos)+1;
  NO_RETURN (13479);
}

LONGINT OScan_Col (LONGINT _pos) {
  LONGINT _line;
  _line = OScan_FindLine(_pos);
  if (_line<=0)  {
    if (_pos<0)  {
      return 0;
    } else {
      return _pos+1;
    }
  } else {
    return ((_pos- 1)- (* PTR_INDEX(CHECK_NIL(OScan_PosArray, OScan_eolArray, 14135), _line- 1, GET_LEN(OScan_eolArray, 0), 1, 14135)))+1;
  }
  NO_RETURN (13788);
}

static void OScan_SkipBlanks (void) {
  while(1) {
    if (OScan_pos>=OScan_bufRefreshPos)  {
      OScan_ReadNextBlock();
    }
    if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 14428), OScan_pos, 32767, 14428)==OScan_eof||INDEX(DEREF(CHAR (* )[32767], OScan_buf, 14448), OScan_pos, 32767, 14448)>='!')  {
      return;
    } else {
      if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 14597), OScan_pos, 32767, 14597)==OScan_eol)  {
        OScan_EndOfLine();
      }
      INCI(OScan_pos, 1, 14638);
    }
  }
}

static void OScan_Comment (void) {
  LONGINT _start;
  _start = (OScan_bufOffset+(LONGINT) OScan_pos)- 1;
  INCI(OScan_pos, 1, 15061);
  while(1) {
    if (OScan_pos>=OScan_sizeBuffer- 1)  {
      OScan_ReadNextBlock();
    }
    {
      CHAR _temp_ = INDEX(DEREF(CHAR (* )[32767], OScan_buf, 15241), OScan_pos, 32767, 15241);
      switch (_temp_) {
        case '\000': {
          OScan_Err(_start, 1);
          goto _exit15076;
          break;
        }
        case '\012': {
          OScan_EndOfLine();
          INCI(OScan_pos, 1, 15464);
          break;
        }
        case '*': {
          INCI(OScan_pos, 1, 15495);
          if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 15521), OScan_pos, 32767, 15521)==')')  {
            INCI(OScan_pos, 1, 15578);
            goto _exit15076;
          }
          break;
        }
        case '(': {
          INCI(OScan_pos, 1, 15637);
          if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 15663), OScan_pos, 32767, 15663)=='*')  {
            OScan_Comment();
          }
          break;
        }
        default:  {
        INCI(OScan_pos, 1, 15824);
      }
      }
    }
  }
  _exit15076: ;
}

static void OScan_GetString (CHAR _end) {
  INTEGER _len;
  LONGINT _start;
  OScan_sym = OScan_string;
  _len = 0;
  _start = OScan_bufOffset+(LONGINT) OScan_pos;
  while(1) {
    INCI(OScan_pos, 1, 16515);
    if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 16539), OScan_pos, 32767, 16539)<' ')  {
      if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 16624), OScan_pos, 32767, 16624)==OScan_eof)  {
        OScan_Err(_start, 3);
        goto _exit16436;
      } else {
        if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 16713), OScan_pos, 32767, 16713)==OScan_eol)  {
          OScan_EndOfLine();
        }
        OScan_ErrRel(OScan_pos, 2);
      }
    } else if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 16800), OScan_pos, 32767, 16800)==_end)  {
      INCI(OScan_pos, 1, 16853);
      goto _exit16436;
    }
    if (_len<OMachine_maxSizeString)  {
      INDEX(OScan_ref, _len, 256, 16937) = INDEX(DEREF(CHAR (* )[32767], OScan_buf, 16949), OScan_pos, 32767, 16949);
    } else if (OScan_pos>=OScan_bufRefreshPos)  {
      OScan_ReadNextBlock();
    }
    INCI(_len, 1, 17034);
  }
  _exit16436:
  if (_len<OMachine_maxSizeString)  {
    INDEX(OScan_ref, _len, 256, 17145) = OScan_nul;
  } else {
    OScan_Err(_start, 12);
    INDEX(OScan_ref, 255, 256, 17199) = OScan_nul;
  }
  OScan_intVal = (LONGINT) ((INTEGER) INDEX(OScan_ref, 0, 256, 17258));
}

static void OScan_Ident (void) {
  INTEGER _sum, _len;
  LONGINT _start;
  OScan_sym = OScan_ident;
  _len = 0;
  _sum = 0;
  _start = OScan_bufOffset+(LONGINT) OScan_pos;
  do  {
    if (_len<48)  {
      INDEX(OScan_ref, _len, 256, 17830) = INDEX(DEREF(CHAR (* )[32767], OScan_buf, 17842), OScan_pos, 32767, 17842);
    } else if (OScan_pos>=OScan_bufRefreshPos)  {
      OScan_ReadNextBlock();
    }
    INCI(_len, 1, 17927);
    INCI(_sum, (INTEGER) INDEX(DEREF(CHAR (* )[32767], OScan_buf, 17962), OScan_pos, 32767, 17962), 17944);
    INCI(OScan_pos, 1, 17977);
  } while (!(!((('A'<=CAP(INDEX(DEREF(CHAR (* )[32767], OScan_buf, 18016), OScan_pos, 32767, 18016))&&CAP(INDEX(DEREF(CHAR (* )[32767], OScan_buf, 18035), OScan_pos, 32767, 18035))<='Z')||('0'<=INDEX(DEREF(CHAR (* )[32767], OScan_buf, 18077), OScan_pos, 32767, 18077)&&INDEX(DEREF(CHAR (* )[32767], OScan_buf, 18090), OScan_pos, 32767, 18090)<='9'))||(OScan_underscore&&INDEX(DEREF(CHAR (* )[32767], OScan_buf, 18137), OScan_pos, 32767, 18137)=='_'))));
  if (_len<48)  {
    INDEX(OScan_ref, _len, 256, 18196) = OScan_nul;
    _sum = MODI(_sum, 85, 18307);
    if (INDEX(INDEX(OScan_kwStr, _sum, 85, 18339), 0, 10, 18343)!='\000')  {
      if (STREQL(INDEX(OScan_kwStr, _sum, 85, 18376), OScan_ref))  {
        OScan_sym = INDEX(OScan_kwSym, _sum, 85, 18416);
      } else {
        INCI(_sum, -29, 18445);
        if (_sum>=0&&STREQL(INDEX(OScan_kwStr, _sum, 85, 18500), OScan_ref))  {
          OScan_sym = INDEX(OScan_kwSym, _sum, 85, 18542);
        }
      }
    }
  } else {
    OScan_Err(_start, 13);
    INDEX(OScan_ref, 47, 256, 18625) = OScan_nul;
  }
}

static void OScan_Number (void) {
  SHORTINT _format;
  INTEGER _i, _d, _len;
  LONGINT _start;
  const LONGREAL _eps = 2.0282409603651670E+31;

  INTEGER _GetCypher (CHAR _c, INTEGER _pos, BOOLEAN _hex) {
    INTEGER _d;
    _d = (INTEGER) _c;
    if (48<=_d&&_d<=57)  {
      DECI(_d, 48, 19521);
    } else if ((_hex&&65<=_d)&&_d<=70)  {
      DECI(_d, 55, 19607);
    } else {
      OScan_ErrRel(_pos, 5);
      _d = 0;
    }
    return _d;
    NO_RETURN (19353);
  }

  LONGINT _ConvertHex (LONGINT _spos, LONGINT _epos) {
    LONGINT _result;
    _result = 0;
    while (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 19912), _spos, 32767, 19912)=='0')  {
      INCLI(_spos, 1, 19938);
    }
    if (_epos- _spos>7)  {
      OScan_Err(_spos, 6);
    } else if (_spos<=_epos)  {
      _result = (LONGINT) _GetCypher(INDEX(DEREF(CHAR (* )[32767], OScan_buf, 20163), _spos, 32767, 20163), (INTEGER) _spos, TRUE);
      INCLI(_spos, 1, 20200);
      if (_epos- _spos==7&&_result>=8)  {
        DECLI(_result, 16, 20326);
      }
      while (_spos<=_epos)  {
        _result = _result*16+(LONGINT) _GetCypher(INDEX(DEREF(CHAR (* )[32767], OScan_buf, 20436), _spos, 32767, 20436), (INTEGER) _spos, TRUE);
        INCLI(_spos, 1, 20475);
      }
    }
    return _result;
    NO_RETURN (19762);
  }
  OScan_sym = OScan_number;
  _start = OScan_bufOffset+(LONGINT) OScan_pos;
  do  {
    INCI(OScan_pos, 1, 20705);
    if (OScan_pos==OScan_sizeBuffer- 1)  {
      OScan_ReadNextBlock();
    }
  } while (!(!(('0'<=INDEX(DEREF(CHAR (* )[32767], OScan_buf, 20807), OScan_pos, 32767, 20807)&&INDEX(DEREF(CHAR (* )[32767], OScan_buf, 20820), OScan_pos, 32767, 20820)<='9')||('A'<=INDEX(DEREF(CHAR (* )[32767], OScan_buf, 20848), OScan_pos, 32767, 20848)&&INDEX(DEREF(CHAR (* )[32767], OScan_buf, 20861), OScan_pos, 32767, 20861)<='F'))));
  if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 20888), OScan_pos, 32767, 20888)=='.'&&INDEX(DEREF(CHAR (* )[32767], OScan_buf, 20907), OScan_pos+1, 32767, 20907)!='.')  {
    INCI(OScan_pos, 1, 20968);
    while ('0'<=INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21036), OScan_pos, 32767, 21036)&&INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21049), OScan_pos, 32767, 21049)<='9')  {
      INCI(OScan_pos, 1, 21074);
      if (OScan_pos==OScan_sizeBuffer- 2)  {
        OScan_ReadNextBlock();
      }
    }
    if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21245), OScan_pos, 32767, 21245)=='D')  {
      OScan_numType = OScan_numLReal;
      INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21295), OScan_pos, 32767, 21295) = 'E';
    } else {
      OScan_numType = OScan_numReal;
    }
    if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21370), OScan_pos, 32767, 21370)=='E')  {
      INCI(OScan_pos, 1, 21421);
      if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21447), OScan_pos, 32767, 21447)=='-'||INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21467), OScan_pos, 32767, 21467)=='+')  {
        INCI(OScan_pos, 1, 21495);
      }
      if ('0'<=INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21540), OScan_pos, 32767, 21540)&&INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21553), OScan_pos, 32767, 21553)<='9')  {
        do  {
          INCI(OScan_pos, 1, 21601);
          if (OScan_pos==OScan_sizeBuffer)  {
            OScan_ReadNextBlock();
          }
        } while (!(INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21715), OScan_pos, 32767, 21715)<'0'||'9'<INDEX(DEREF(CHAR (* )[32767], OScan_buf, 21741), OScan_pos, 32767, 21741)));
      } else {
        OScan_ErrRel(OScan_pos, 9);
      }
    }
    _len = (INTEGER) ((OScan_bufOffset+(LONGINT) OScan_pos)- _start);
    if (_len<64)  {
      _d = (INTEGER) (_start- OScan_bufOffset);
      {
        INTEGER _temp_ = _len- 1;
        for(_i = 0 ; _i <= _temp_ ; _i += 1) {
          INDEX(OScan_ref, _i, 256, 22113) = INDEX(DEREF(CHAR (* )[32767], OScan_buf, 22123), _d+_i, 32767, 22123);
        }
      }
      INDEX(OScan_ref, _len, 256, 22153) = OScan_nul;
      RealStr_Take(256, (CHAR *) OScan_ref, &(OScan_realVal), &(_format));
      if (_format==ConvTypes_outOfRange||(OScan_numType==OScan_numReal&&(OScan_realVal<-3.4028236692093846E+38||OScan_realVal>=3.4028236692093846E+38)))  {
        OScan_Err(_start, 6);
      }
    } else {
      OScan_realVal = 1.0000000000000000E+00;
      OScan_Err(_start, 4);
    }
  } else {
    OScan_intVal = 0;
    if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 22659), OScan_pos, 32767, 22659)=='H'||INDEX(DEREF(CHAR (* )[32767], OScan_buf, 22679), OScan_pos, 32767, 22679)=='X')  {
      OScan_intVal = _ConvertHex((LONGINT) ((INTEGER) (_start- OScan_bufOffset)), (LONGINT) (OScan_pos- 1));
    } else {
      _len = (INTEGER) ((OScan_bufOffset+(LONGINT) OScan_pos)- _start);
      if (_len<64)  {
        _i = (INTEGER) (_start- OScan_bufOffset);
        while (_i<OScan_pos)  {
          _d = _GetCypher(INDEX(DEREF(CHAR (* )[32767], OScan_buf, 22953), _i, 32767, 22953), _i, FALSE);
          if (DIVLI(OMachine_maxLInt- (LONGINT) _d, 10, 22999)>=OScan_intVal)  {
            OScan_intVal = OScan_intVal*10+(LONGINT) _d;
          } else {
            OScan_Err(_start, 6);
          }
          INCI(_i, 1, 23149);
        }
      } else {
        OScan_intVal = 1;
        OScan_Err(_start, 4);
      }
    }
    if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 23358), OScan_pos, 32767, 23358)=='X')  {
      OScan_sym = OScan_string;
      INCI(OScan_pos, 1, 23407);
      if (OScan_intVal>255||OScan_intVal<0)  {
        OScan_Err(_start, 7);
      } else {
        INDEX(OScan_ref, 0, 256, 23568) = (CHAR) OScan_intVal;
        INDEX(OScan_ref, 1, 256, 23602) = OScan_nul;
      }
    } else {
      if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 23651), OScan_pos, 32767, 23651)=='H')  {
        INCI(OScan_pos, 1, 23679);
      }
      OScan_numType = OScan_numInt;
    }
  }
}

void OScan_GetSym (void) {
  LONGINT _c, _l, _p;
  OScan_SkipBlanks();
  if (OScan_pos>=OScan_bufRefreshPos)  {
    OScan_ReadNextBlock();
  }
  OScan_lastSym = OScan_bufOffset+(LONGINT) OScan_pos;
  {
    CHAR _temp_ = INDEX(DEREF(CHAR (* )[32767], OScan_buf, 24160), OScan_pos, 32767, 24160);
    switch (_temp_) {
      case 'a' ... 'z':
      case 'A' ... 'Z': {
        OScan_Ident();
        break;
      }
      case '_': {
        if (OScan_underscore)  {
          OScan_Ident();
        } else {
          OScan_Err(OScan_lastSym, 8);
          INCI(OScan_pos, 1, 24262);
          OScan_GetSym();
        }
        break;
      }
      case '0' ... '9': {
        OScan_Number();
        break;
      }
      case '\000': {
        OScan_sym = OScan_endOfFile;
        break;
      }
      case '\042': {
        OScan_GetString('\042');
        break;
      }
      case '\047': {
        OScan_GetString('\047');
        break;
      }
      case '~': {
        OScan_sym = OScan_not;
        INCI(OScan_pos, 1, 24422);
        break;
      }
      case '{': {
        OScan_sym = OScan_lBrace;
        INCI(OScan_pos, 1, 24460);
        break;
      }
      case '.': {
        INCI(OScan_pos, 1, 24483);
        if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 24512), OScan_pos, 32767, 24512)=='.')  {
          OScan_sym = OScan_upto;
          INCI(OScan_pos, 1, 24541);
        } else {
          OScan_sym = OScan_period;
        }
        break;
      }
      case '^': {
        OScan_sym = OScan_arrow;
        INCI(OScan_pos, 1, 24612);
        break;
      }
      case '[': {
        OScan_sym = OScan_lBrak;
        INCI(OScan_pos, 1, 24649);
        break;
      }
      case ':': {
        INCI(OScan_pos, 1, 24672);
        if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 24701), OScan_pos, 32767, 24701)=='=')  {
          OScan_sym = OScan_becomes;
          INCI(OScan_pos, 1, 24733);
        } else {
          OScan_sym = OScan_colon;
        }
        break;
      }
      case '(': {
        INCI(OScan_pos, 1, 24789);
        if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 24818), OScan_pos, 32767, 24818)=='*')  {
          OScan_Comment();
          OScan_GetSym();
        } else {
          OScan_sym = OScan_lParen;
        }
        break;
      }
      case '*': {
        OScan_sym = OScan_times;
        INCI(OScan_pos, 1, 24912);
        if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 24941), OScan_pos, 32767, 24941)==')')  {
          OScan_Err(OScan_lastSym, 14);
          INCI(OScan_pos, 1, 25002);
        }
        break;
      }
      case '/': {
        OScan_sym = OScan_slash;
        INCI(OScan_pos, 1, 25054);
        break;
      }
      case '&': {
        OScan_sym = OScan_and;
        INCI(OScan_pos, 1, 25089);
        break;
      }
      case '+': {
        OScan_sym = OScan_plus;
        INCI(OScan_pos, 1, 25125);
        break;
      }
      case '-': {
        OScan_sym = OScan_minus;
        INCI(OScan_pos, 1, 25162);
        break;
      }
      case '=': {
        OScan_sym = OScan_eql;
        INCI(OScan_pos, 1, 25197);
        break;
      }
      case '#': {
        OScan_sym = OScan_neq;
        INCI(OScan_pos, 1, 25232);
        break;
      }
      case '<': {
        INCI(OScan_pos, 1, 25255);
        if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 25284), OScan_pos, 32767, 25284)=='=')  {
          OScan_sym = OScan_leq;
          INCI(OScan_pos, 1, 25312);
        } else {
          OScan_sym = OScan_lss;
        }
        break;
      }
      case '>': {
        INCI(OScan_pos, 1, 25366);
        if (INDEX(DEREF(CHAR (* )[32767], OScan_buf, 25395), OScan_pos, 32767, 25395)=='=')  {
          OScan_sym = OScan_geq;
          INCI(OScan_pos, 1, 25423);
        } else {
          OScan_sym = OScan_gtr;
        }
        break;
      }
      case '}': {
        OScan_sym = OScan_rBrace;
        INCI(OScan_pos, 1, 25492);
        break;
      }
      case ')': {
        OScan_sym = OScan_rParen;
        INCI(OScan_pos, 1, 25530);
        break;
      }
      case ']': {
        OScan_sym = OScan_rBrak;
        INCI(OScan_pos, 1, 25567);
        break;
      }
      case '|': {
        OScan_sym = OScan_bar;
        INCI(OScan_pos, 1, 25602);
        break;
      }
      case ';': {
        OScan_sym = OScan_semicolon;
        INCI(OScan_pos, 1, 25643);
        break;
      }
      case ',': {
        OScan_sym = OScan_comma;
        INCI(OScan_pos, 1, 25680);
        break;
      }
      default:  {
      OScan_Err(OScan_lastSym, 8);
      INCI(OScan_pos, 1, 25723);
      OScan_GetSym();
    }
    }
  }
}

void OScan_LastPos (void) {
  Out_String(30, (CHAR *) "scanning pos on termination: ");
  Out_Int(OScan_bufOffset+(LONGINT) OScan_pos, 5);
  Out_Ln();
}

void OScan_Init (LONGINT __fileName_0, const CHAR (* __fileName_p), INTEGER _bufferSize, BOOLEAN (* _err)) {
  CHAR (* _fileName);

  void _CheckOut (void) {
    CHAR _com[1024];
    if (Dos_Exists(_fileName))  {
      Redir_RCS2File(__fileName_0, _fileName, __fileName_0, _fileName);
      COPYSTRING(&(_com), "co -q ");
      Strings_Append(__fileName_0, _fileName, 1024, (CHAR *) _com);
      OScan_VerboseMsg(1024, (CHAR *) _com);
      if (Rts_System((CHAR *) _com)!=0)  {
        Out_String(43, (CHAR *) " --- failed to extract latest revision of ");
        Out_String(__fileName_0, _fileName);
        Out_Ln();
        return;
      }
    } else {
      Out_String(22, (CHAR *) " --- can't open file ");
      Out_String(__fileName_0, _fileName);
      Out_Ln();
      return;
    }
  }
  VALUE_ARRAY(_fileName, __fileName_p, 1*__fileName_0);
  (* _err) = TRUE;
  if (Strings2_Match(4, (CHAR *) "*,v", __fileName_0, _fileName))  {
    _CheckOut();
  }
  COPY(_fileName, OScan_sourceName, 256);
  OScan_inFile = Files_Old(_fileName);
  if ((void*)OScan_inFile==(void*)NULL)  {
    Out_String(11, (CHAR *) " --- file ");
    Out_String(__fileName_0, _fileName);
    Out_String(11, (CHAR *) " not found");
    Out_Ln();
  } else {
    Files_Set(&(OScan_inRider), OScan_inFile, 0);
    OScan_len = Files_Length(OScan_inFile);
    OScan_sizeBuffer = _bufferSize;
    OScan_bufRefreshPos = OScan_sizeBuffer- OScan_maxBufferRest;
    OScan_pos = _bufferSize;
    OScan_bufOffset = (LONGINT) (-_bufferSize);
    (* _err) = FALSE;
    OScan_lastErr = -9999;
    OScan_errHeader = FALSE;
    OScan_lastSym = -1;
    OScan_eolSize = 0;
    OScan_ReadNextBlock();
  }
  OScan_noerr = !(* _err);
}

void OScan_Close (void) {
  Files_Close(&(OScan_inFile));
}

static void OScan_InitKeywords (void) {
  INTEGER _i;

  void _KW (LONGINT __ident_0, const CHAR (* __ident_p), SHORTINT _sym) {
    CHAR (* _ident);
    INTEGER _i, _sum;
    VALUE_ARRAY(_ident, __ident_p, 1*__ident_0);
    _sum = 0;
    _i = 0;
    while ((* PTR_INDEX(_ident, _i, __ident_0, 1, 27655))!='\000')  {
      INCI(_sum, (INTEGER) (* PTR_INDEX(_ident, _i, __ident_0, 1, 27696)), 27676);
      INCI(_i, 1, 27711);
    }
    _i = MODI(_sum, 85, 27745);
    if (INDEX(OScan_kwSym, _i, 85, 27778)>=0)  {
      INCI(_i, -29, 27801);
    }
    COPY(_ident, INDEX(OScan_kwStr, _i, 85, 27856), 10);
    INDEX(OScan_kwSym, _i, 85, 27873) = _sym;
  }
  {
    INTEGER _temp_ = 84;
    for(_i = 0 ; _i <= _temp_ ; _i += 1) {
      INDEX(OScan_kwSym, _i, 85, 27951) = -1;
      INDEX(INDEX(OScan_kwStr, _i, 85, 27973), 0, 10, 27975) = '\000';
    }
  }
  _KW(6, (CHAR *) "ARRAY", OScan_array);
  _KW(6, (CHAR *) "BEGIN", OScan_begin);
  _KW(3, (CHAR *) "BY", OScan_by);
  _KW(5, (CHAR *) "CASE", OScan_case);
  _KW(6, (CHAR *) "CONST", OScan_const);
  _KW(4, (CHAR *) "DIV", OScan_div);
  _KW(3, (CHAR *) "DO", OScan_do);
  _KW(5, (CHAR *) "ELSE", OScan_else);
  _KW(6, (CHAR *) "ELSIF", OScan_elsif);
  _KW(4, (CHAR *) "END", OScan_end);
  _KW(5, (CHAR *) "EXIT", OScan_exit);
  _KW(4, (CHAR *) "FOR", OScan_for);
  _KW(3, (CHAR *) "IF", OScan_if);
  _KW(7, (CHAR *) "IMPORT", OScan_import);
  _KW(3, (CHAR *) "IN", OScan_in);
  _KW(3, (CHAR *) "IS", OScan_is);
  _KW(5, (CHAR *) "LOOP", OScan_loop);
  _KW(4, (CHAR *) "MOD", OScan_mod);
  _KW(7, (CHAR *) "MODULE", OScan_module);
  _KW(4, (CHAR *) "NIL", OScan_nil);
  _KW(3, (CHAR *) "OF", OScan_of);
  _KW(3, (CHAR *) "OR", OScan_or);
  _KW(8, (CHAR *) "POINTER", OScan_pointer);
  _KW(10, (CHAR *) "PROCEDURE", OScan_procedure);
  _KW(7, (CHAR *) "RECORD", OScan_record);
  _KW(7, (CHAR *) "REPEAT", OScan_repeat);
  _KW(7, (CHAR *) "RETURN", OScan_return);
  _KW(5, (CHAR *) "THEN", OScan_then);
  _KW(3, (CHAR *) "TO", OScan_to);
  _KW(5, (CHAR *) "TYPE", OScan_type);
  _KW(6, (CHAR *) "UNTIL", OScan_until);
  _KW(4, (CHAR *) "VAR", OScan_var);
  _KW(6, (CHAR *) "WHILE", OScan_while);
  _KW(5, (CHAR *) "WITH", OScan_with);
}

void _init_OScan (void) {
  moduleId = add_module ("OScan");
  td_OScan_ErrMsgDesc = create_td("ErrMsgDesc", sizeof(OScan_ErrMsgDesc), sizeof(TD_OScan_ErrMsgDesc), NULL, 0);
  NEWFIX(OScan_buf, 1);
  Rts_Assert((void*)OScan_buf!=(void*)NULL, (CHAR *) "Fatal: Cannot allocate memory for file buffer.");
  OScan_warnings = FALSE;
  OScan_verbose = FALSE;
  OScan_underscore = FALSE;
  NEWDYN(OScan_eolArray, 1*1020, 1, 1);
  SET_LEN(0, 1020);
  Rts_Assert((void*)OScan_eolArray!=(void*)NULL, (CHAR *) "Fatal: Cannot allocate memory for line buffer.");
  OScan_InitKeywords();
}
