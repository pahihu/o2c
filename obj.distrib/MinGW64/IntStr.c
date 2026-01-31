#include "IntStr.h"
#include "CharInfo.h"
#include "ConvTypes.h"
#include "Strings.h"

static ModuleId moduleId;






void IntStr_Take (LONGINT __str_0, const CHAR (* __str_p), LONGINT (* _int), SHORTINT (* _format)) {
  CHAR (* _str);
  INTEGER _s, _e;
  LONGINT _digit;
  BOOLEAN _positive;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  _s = 0;
  while (CharInfo_IsSpace((* PTR_INDEX(_str, _s, __str_0, 1, 1036))))  {
    INCI(_s, 1, 1052);
  }
  if ((* PTR_INDEX(_str, _s, __str_0, 1, 1081))=='\000')  {
    (* _format) = ConvTypes_noData;
  } else {
    if (CharInfo_IsSign((* PTR_INDEX(_str, _s, __str_0, 1, 1263))))  {
      _positive = (* PTR_INDEX(_str, _s, __str_0, 1, 1319))!='-';
      INCI(_s, 1, 1339);
    } else {
      _positive = TRUE;
    }
    _e = _s;
    while (CharInfo_IsDigit((* PTR_INDEX(_str, _e, __str_0, 1, 1499))))  {
      INCI(_e, 1, 1516);
    }
    if ((* PTR_INDEX(_str, _e, __str_0, 1, 1549))!='\000')  {
      (* _format) = ConvTypes_wrongFormat;
    } else {
      (* _int) = 0;
      while (_s!=_e)  {
        _digit = (LONGINT) ((INTEGER) (* PTR_INDEX(_str, _s, __str_0, 1, 1913))- 48);
        if ((* _int)>-922337203685477580LL||(* _int)>=DIVLI((-9223372036854775807LL-1)+_digit, 10, 1994))  {
          (* _int) = (* _int)*10- _digit;
          INCI(_s, 1, 2052);
        } else {
          (* _format) = ConvTypes_outOfRange;
          if (_positive)  {
            (* _int) = 9223372036854775807LL;
          } else {
            (* _int) = (-9223372036854775807LL-1);
          }
          return;
        }
      }
      if (_positive)  {
        if ((* _int)==(-9223372036854775807LL-1))  {
          (* _format) = ConvTypes_outOfRange;
          (* _int) = 9223372036854775807LL;
          return;
        } else {
          (* _int) = -(* _int);
        }
      }
      (* _format) = ConvTypes_allRight;
    }
  }
}

SHORTINT IntStr_Format (LONGINT __str_0, const CHAR (* __str_p)) {
  CHAR (* _str);
  LONGINT _int;
  SHORTINT _format;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  IntStr_Take(__str_0, _str, &(_int), &(_format));
  return _format;
  NO_RETURN (2672);
}

LONGINT IntStr_Value (LONGINT __str_0, const CHAR (* __str_p)) {
  CHAR (* _str);
  LONGINT _int;
  SHORTINT _format;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  IntStr_Take(__str_0, _str, &(_int), &(_format));
  return _int;
  NO_RETURN (3062);
}

static void IntStr_Reverse (LONGINT __str_0, CHAR (* _str), INTEGER _start, INTEGER _end) {
  CHAR _h;
  while (_start<_end)  {
    _h = (* PTR_INDEX(_str, _start, __str_0, 1, 3631));
    (* PTR_INDEX(_str, _start, __str_0, 1, 3644)) = (* PTR_INDEX(_str, _end, __str_0, 1, 3659));
    (* PTR_INDEX(_str, _end, __str_0, 1, 3670)) = _h;
    INCI(_start, 1, 3688);
    DECI(_end, 1, 3701);
  }
}

void IntStr_Give (LONGINT __str_0, CHAR (* _str), LONGINT _int, INTEGER _width, SHORTINT _where) {
  const SHORTINT _maxLength = 20;
  CHAR _b[21];
  INTEGER _s, _e, _i, _pos;
  if (_int==(-9223372036854775807LL-1))  {
    COPYSTRING(&(_b), "-9223372036854775808LL");
    _e = _maxLength;
  } else {
    if (_int<0)  {
      INDEX(_b, 0, 21, 4575) = '-';
      _int = -_int;
      _s = 1;
    } else {
      _s = 0;
    }
    _e = _s;
    do  {
      INDEX(_b, _e, 21, 4766) = (CHAR) (MODLI(_int, 10, 4782)+48);
      _int = DIVLI(_int, 10, 4820);
      INCI(_e, 1, 4836);
    } while (!(_int==0));
    INDEX(_b, _e, 21, 4874) = '\000';
    IntStr_Reverse(21, (CHAR *) _b, _s, _e- 1);
  }
  _s = _width- _e;
  if (_s>0)  {
    {
      INTEGER _temp_ = _s- 1;
      for(_i = 0 ; _i <= _temp_ ; _i += 1) {
        (* PTR_INDEX(_str, _i, __str_0, 1, 5098)) = ' ';
      }
    }
    (* PTR_INDEX(_str, _s, __str_0, 1, 5124)) = '\000';
    {
      SHORTINT _temp_ = _where;
      switch (_temp_) {
        case 0: {
          _pos = 0;
          break;
        }
        case 2: {
          _pos = _s;
          break;
        }
        case 1: {
          _pos = _s- DIVI(_s, 2, 5312);
          break;
        }
        default: NO_LABEL (_temp_, 5202);
      }
    }
    Strings_Insert(21, (CHAR *) _b, _pos, __str_0, _str);
  } else {
    COPY(_b, _str, __str_0);
  }
}

void IntStr_Append (LONGINT _int, INTEGER _width, SHORTINT _where, LONGINT __dest_0, CHAR (* _dest)) {
  CHAR _str[32];
  IntStr_Give(32, (CHAR *) _str, _int, _width, _where);
  Strings_Append(32, (CHAR *) _str, __dest_0, _dest);
}

void _init_IntStr (void) {
  moduleId = add_module ("IntStr");
}
