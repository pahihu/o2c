#include "RealStr.h"
#include "CType.h"
#include "ConvTypes.h"
#include "IntStr.h"
#include "Strings.h"
#include "Strings2.h"
#include "Unix.h"

static ModuleId moduleId;



const SHORTINT RealStr_maxSigFigs = 17;
static const SHORTINT RealStr_maxExpFigs = 3;
static const SHORTINT RealStr_width = 24;
static const SHORTINT RealStr_posExp = 19;
typedef CHAR RealStr_NumberString[25];



void RealStr_Take (LONGINT __str_0, const CHAR (* __str_p), LONGREAL (* _real), SHORTINT (* _format)) {
  CHAR (* _str);
  LONGINT _tail;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  (* _real) = strtod(_str, &(_tail));
  if (_tail!=0&&_tail!=(LONGINT)PTR_INDEX(_str, Strings_Length(__str_0, _str), __str_0, 1, 1773))  {
    (* _format) = ConvTypes_wrongFormat;
  } else if (Unix_is_hugeval((* _real)))  {
    (* _format) = ConvTypes_outOfRange;
  } else {
    (* _format) = ConvTypes_allRight;
  }
}

SHORTINT RealStr_Format (LONGINT __str_0, const CHAR (* __str_p)) {
  CHAR (* _str);
  SHORTINT _f;
  LONGREAL _r;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  RealStr_Take(__str_0, _str, &(_r), &(_f));
  return _f;
  NO_RETURN (1971);
}

LONGREAL RealStr_Value (LONGINT __str_0, const CHAR (* __str_p)) {
  CHAR (* _str);
  SHORTINT _f;
  LONGREAL _r;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  RealStr_Take(__str_0, _str, &(_r), &(_f));
  return _r;
  NO_RETURN (2287);
}

static void RealStr_ConvertNumber (LONGREAL _x, RealStr_NumberString _s) {
  INTEGER _i;
  LONGINT _result;
  CHAR _template[20];
  _result = sprintf((CHAR *) _template, (CHAR *) "%%+0#%d.%dE", RealStr_width, 16);
  _result = sprintf((CHAR *) _s, (CHAR *) _template, _x);
  if (INDEX(_s, 20, 25, 3006)=='E')  {
    {
      INTEGER _temp_ = 21;
      for(_i = 2 ; _i <= _temp_ ; _i += 1) {
        INDEX(_s, _i- 1, 25, 3070) = INDEX(_s, _i, 25, 3080);
      }
    }
    INDEX(_s, 21, 25, 3102) = '0';
  }
  INDEX(_s, 2, 25, 3226) = INDEX(_s, 1, 25, 3234);
  INDEX(_s, 1, 25, 3244) = '0';
}

static void RealStr_Align (LONGINT __str_0, CHAR (* _str), INTEGER _width, SHORTINT _where) {
  INTEGER _len, _ins, _pos;
  _len = Strings_Length(__str_0, _str);
  _ins = _width- _len;
  if (_ins>0)  {
    {
      SHORTINT _temp_ = _where;
      switch (_temp_) {
        case 0: {
          _pos = _len;
          break;
        }
        case 2: {
          _pos = 0;
          break;
        }
        case 1: {
          _pos = _len- DIVI(_ins, 2, 3600);
          break;
        }
        default: NO_LABEL (_temp_, 3484);
      }
    }
    while (_ins>0)  {
      Strings_Insert(2, (CHAR *) " ", _pos, __str_0, _str);
      DECI(_ins, 1, 3686);
    }
  }
}

static INTEGER RealStr_Exponent (const RealStr_NumberString __s_p) {
  RealStr_NumberString _s;
  CHAR _e[5];
  LONGINT _i;
  SHORTINT _f;
  VALUE_ARRAYF(_s, __s_p, 1);
  Strings_Extract(25, (CHAR *) _s, 20, 4, 5, (CHAR *) _e);
  IntStr_Take(5, (CHAR *) _e, &(_i), &(_f));
  return (INTEGER) _i;
  NO_RETURN (3738);
}

static void RealStr_RoundSigFigs (INTEGER _sigFigs, RealStr_NumberString _s) {
  INTEGER _i;

  BOOLEAN _RoundUp (void) {
    if (INDEX(_s, 2+_sigFigs, 25, 4250)!='5')  {
      return INDEX(_s, 2+_sigFigs, 25, 4291)>'5';
    } else {
      {
        INTEGER _temp_ = 18;
        for(_i = 3+_sigFigs ; _i <= _temp_ ; _i += 1) {
          if (INDEX(_s, _i, 25, 4382)!='0')  {
            return TRUE;
          }
        }
      }
      return MODI((INTEGER) INDEX(_s, 1+_sigFigs, 25, 4532)- 48, 2, 4555)==1;
    }
    NO_RETURN (4137);
  }
  if (0<=_sigFigs&&_sigFigs<17)  {
    if (_RoundUp())  {
      _i = 1+_sigFigs;
      while(1) {
        INDEX(_s, _i, 25, 4911) = (CHAR) ((INTEGER) INDEX(_s, _i, 25, 4929)+1);
        if (INDEX(_s, _i, 25, 4953)<='9')  {
          goto _exit4895;
        }
        INDEX(_s, _i, 25, 5013) = '0';
        DECI(_i, 1, 5035);
      }
      _exit4895:
    }
    {
      INTEGER _temp_ = 18;
      for(_i = 2+_sigFigs ; _i <= _temp_ ; _i += 1) {
        INDEX(_s, _i, 25, 5119) = '0';
      }
    }
    if (INDEX(_s, 1, 25, 5195)=='1')  {
      INDEX(_s, 1, 25, 5256) = '0';
      INDEX(_s, 2, 25, 5277) = '1';
      _i = RealStr_Exponent(_s)+1;
      if (_i>=0)  {
        INDEX(_s, 20, 25, 5392) = '+';
      }
      INDEX(_s, 21, 25, 5432) = (CHAR) (48+DIVI(ABSI(_i, 5463), 100, 5471));
      INDEX(_s, 22, 25, 5490) = (CHAR) (48+MODI(DIVI(ABSI(_i, 5522), 10, 5530), 10, 5538));
      INDEX(_s, 23, 25, 5556) = (CHAR) (48+MODI(ABSI(_i, 5587), 10, 5595));
    }
  }
}

void RealStr_GiveFloat (LONGINT __str_0, CHAR (* _str), LONGREAL _real, INTEGER _sigFigs, INTEGER _width, SHORTINT _where) {
  INTEGER _i, _j, _exp;
  RealStr_NumberString _n;
  CHAR _buffer[263];
  RealStr_ConvertNumber(_real, _n);
  RealStr_RoundSigFigs(_sigFigs, _n);
  _j = _sigFigs;
  if (_j>17)  {
    _j = 17;
  }
  {
    INTEGER _temp_ = _j- 1;
    for(_i = 0 ; _i <= _temp_ ; _i += 1) {
      INDEX(_buffer, _i, 263, 6327) = INDEX(_n, 2+_i, 25, 6335);
    }
  }
  {
    INTEGER _temp_ = _sigFigs- 1;
    for(_i = _j ; _i <= _temp_ ; _i += 1) {
      INDEX(_buffer, _i, 263, 6393) = '0';
    }
  }
  INDEX(_buffer, _sigFigs, 263, 6423) = '\000';
  if (_sigFigs>1)  {
    Strings_Insert(2, (CHAR *) ".", 1, 263, (CHAR *) _buffer);
  }
  if (_real<0.0000000000000000E+00)  {
    Strings_Insert(2, (CHAR *) "-", 0, 263, (CHAR *) _buffer);
  }
  _exp = RealStr_Exponent(_n);
  Strings2_AppendChar('E', 263, (CHAR *) _buffer);
  if (_exp<0)  {
    Strings2_AppendChar('-', 263, (CHAR *) _buffer);
  } else {
    Strings2_AppendChar('+', 263, (CHAR *) _buffer);
  }
  if (ABSI(_exp, 6870)<10)  {
    Strings2_AppendChar('0', 263, (CHAR *) _buffer);
  }
  IntStr_Append((LONGINT) ABSI(_exp, 6959), 0, ConvTypes_left, 263, (CHAR *) _buffer);
  COPY(_buffer, _str, __str_0);
  RealStr_Align(__str_0, _str, _width, _where);
}

void RealStr_GiveEng (LONGINT __str_0, CHAR (* _str), LONGREAL _real, INTEGER _sigFigs, INTEGER _width, SHORTINT _where) {
  INTEGER _i, _j, _exp;
  RealStr_NumberString _n;
  CHAR _buffer[263];
  RealStr_ConvertNumber(_real, _n);
  RealStr_RoundSigFigs(_sigFigs, _n);
  _exp = RealStr_Exponent(_n);
  if (_sigFigs<=MODI(_exp, 3, 7745))  {
    _sigFigs = MODI(_exp, 3, 7778)+1;
  }
  _j = _sigFigs;
  if (_j>17)  {
    _j = 17;
  }
  {
    INTEGER _temp_ = _j- 1;
    for(_i = 0 ; _i <= _temp_ ; _i += 1) {
      INDEX(_buffer, _i, 263, 7956) = INDEX(_n, 2+_i, 25, 7964);
    }
  }
  {
    INTEGER _temp_ = _sigFigs- 1;
    for(_i = _j ; _i <= _temp_ ; _i += 1) {
      INDEX(_buffer, _i, 263, 8022) = '0';
    }
  }
  INDEX(_buffer, _sigFigs, 263, 8052) = '\000';
  if (1+MODI(_exp, 3, 8113)<_sigFigs)  {
    Strings_Insert(2, (CHAR *) ".", 1+MODI(_exp, 3, 8164), 263, (CHAR *) _buffer);
  }
  DECI(_exp, MODI(_exp, 3, 8206), 8192);
  if (_real<0.0000000000000000E+00)  {
    Strings_Insert(2, (CHAR *) "-", 0, 263, (CHAR *) _buffer);
  }
  Strings2_AppendChar('E', 263, (CHAR *) _buffer);
  if (_exp<0)  {
    Strings2_AppendChar('-', 263, (CHAR *) _buffer);
  }
  if (ABSI(_exp, 8467)<10)  {
    Strings2_AppendChar('0', 263, (CHAR *) _buffer);
  }
  IntStr_Append((LONGINT) ABSI(_exp, 8556), 0, ConvTypes_left, 263, (CHAR *) _buffer);
  COPY(_buffer, _str, __str_0);
  RealStr_Align(__str_0, _str, _width, _where);
}

void RealStr_GiveFixed (LONGINT __str_0, CHAR (* _str), LONGREAL _real, INTEGER _place, INTEGER _width, SHORTINT _where) {
  RealStr_NumberString _n;
  INTEGER _i, _j, _c, _exp;
  CHAR _buffer[257];
  RealStr_ConvertNumber(_real, _n);
  RealStr_RoundSigFigs((1+RealStr_Exponent(_n))+_place, _n);
  _exp = RealStr_Exponent(_n);
  _j = 2;
  if (_exp<0||-_place>_exp)  {
    COPYSTRING(&(_buffer), "0");
    _i = 1;
  } else {
    _i = 0;
    while (_i<=_exp)  {
      if (_i<17&&_i<=_exp+_place)  {
        INDEX(_buffer, _i, 257, 9508) = INDEX(_n, _j, 25, 9516);
      } else if ((LONGINT) _i<257)  {
        INDEX(_buffer, _i, 257, 9573) = '0';
      }
      INCI(_i, 1, 9605);
      INCI(_j, 1, 9614);
    }
  }
  if (_place>0)  {
    INDEX(_buffer, _i, 257, 9711) = '.';
    INCI(_i, 1, 9729);
    _c = 0;
    while (_c<_place)  {
      if (_j- 2<17)  {
        INDEX(_buffer, _i, 257, 9830) = INDEX(_n, _j, 25, 9838);
      } else if ((LONGINT) _i<257)  {
        INDEX(_buffer, _i, 257, 9895) = '0';
      }
      INCI(_i, 1, 9927);
      INCI(_j, 1, 9936);
      INCI(_c, 1, 9945);
    }
  }
  INDEX(_buffer, _i, 257, 9982) = '\000';
  if (_real<0.0000000000000000E+00)  {
    Strings_Insert(2, (CHAR *) "-", 0, 257, (CHAR *) _buffer);
  }
  COPY(_buffer, _str, __str_0);
  RealStr_Align(__str_0, _str, _width, _where);
}

void _init_RealStr (void) {
  moduleId = add_module ("RealStr");
}
