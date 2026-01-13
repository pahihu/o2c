#include "Strings2.h"
#include "Strings.h"

static ModuleId moduleId;






void Strings2_AppendChar (CHAR _ch, LONGINT __dst_0, CHAR (* _dst)) {
  INTEGER _len;
  _len = Strings_Length(__dst_0, _dst);
  if (_len<(INTEGER) __dst_0- 1)  {
    (* PTR_INDEX(_dst, _len, __dst_0, 1, 278)) = _ch;
    (* PTR_INDEX(_dst, _len+1, __dst_0, 1, 300)) = '\000';
  }
}

void Strings2_InsertChar (CHAR _ch, INTEGER _pos, LONGINT __dst_0, CHAR (* _dst)) {
  CHAR _src[2];
  INDEX(_src, 0, 2, 751) = _ch;
  INDEX(_src, 1, 2, 765) = '\000';
  Strings_Insert(2, (CHAR *) _src, _pos, __dst_0, _dst);
}

INTEGER Strings2_PosChar (CHAR _ch, LONGINT __str_0, const CHAR (* __str_p)) {
  CHAR (* _str);
  INTEGER _i;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  _i = 0;
  while(1) {
    if ((* PTR_INDEX(_str, _i, __str_0, 1, 1135))==_ch)  {
      return _i;
    } else if ((* PTR_INDEX(_str, _i, __str_0, 1, 1183))=='\000')  {
      return -1;
    } else {
      INCI(_i, 1, 1235);
    }
  }
  NO_RETURN (842);
}

BOOLEAN Strings2_Match (LONGINT __pat_0, const CHAR (* __pat_p), LONGINT __s_0, const CHAR (* __s_p)) {
  CHAR (* _pat);
  CHAR (* _s);
  INTEGER _lenSource, _lenPattern;

  BOOLEAN _RecMatch (LONGINT __src_0, CHAR (* _src), INTEGER _posSrc, LONGINT __pat_0, CHAR (* _pat), INTEGER _posPat) {
    INTEGER _i;
    while(1) {
      if (_posSrc==_lenSource&&_posPat==_lenPattern)  {
        return TRUE;
      } else if (_posPat==_lenPattern)  {
        return FALSE;
      } else if ((* PTR_INDEX(_pat, _posPat, __pat_0, 1, 2208))=='*')  {
        if (_posPat==_lenPattern- 1)  {
          return TRUE;
        } else {
          {
            INTEGER _temp_ = _lenSource;
            for(_i = _posSrc ; _i <= _temp_ ; _i += 1) {
              if (_RecMatch(__src_0, _src, _i, __pat_0, _pat, _posPat+1))  {
                return TRUE;
              }
            }
          }
          return FALSE;
        }
      } else if ((* PTR_INDEX(_pat, _posPat, __pat_0, 1, 2540))!='?'&&(* PTR_INDEX(_pat, _posPat, __pat_0, 1, 2562))!=(* PTR_INDEX(_src, _posSrc, __src_0, 1, 2576)))  {
        return FALSE;
      } else {
        INCI(_posSrc, 1, 2640);
        INCI(_posPat, 1, 2653);
      }
    }
    NO_RETURN (1744);
  }
  VALUE_ARRAY(_pat, __pat_p, 1*__pat_0);
  VALUE_ARRAY(_s, __s_p, 1*__s_0);
  _lenPattern = Strings_Length(__pat_0, _pat);
  _lenSource = Strings_Length(__s_0, _s);
  return _RecMatch(__s_0, _s, 0, __pat_0, _pat, 0);
  NO_RETURN (1287);
}

void _init_Strings2 (void) {
  moduleId = add_module ("Strings2");
}
