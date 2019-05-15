#include "Strings.h"

static ModuleId moduleId;






INTEGER Strings_Length (LONGINT __s_0, const CHAR (* __s_p)) {
  CHAR (* _s);
  INTEGER _i;
  VALUE_ARRAY(_s, __s_p, 1*__s_0);
  _i = 0;
  while ((* PTR_INDEX(_s, _i, __s_0, 1, 543))!='\000')  {
    INCI(_i, 1, 562);
  }
  return _i;
  NO_RETURN (374);
}

void Strings_Insert (LONGINT __src_0, const CHAR (* __src_p), INTEGER _pos, LONGINT __dst_0, CHAR (* _dst)) {
  CHAR (* _src);
  INTEGER _lenSrc, _lenDst, _maxDst, _i;
  VALUE_ARRAY(_src, __src_p, 1*__src_0);
  _lenDst = Strings_Length(__dst_0, _dst);
  _lenSrc = Strings_Length(__src_0, _src);
  _maxDst = (INTEGER) __dst_0- 1;
  if (_pos+_lenSrc<_maxDst)  {
    if (_lenDst+_lenSrc>_maxDst)  {
      _lenDst = _maxDst- _lenSrc;
      (* PTR_INDEX(_dst, _lenDst, __dst_0, 1, 1278)) = '\000';
    }
    {
      INTEGER _temp_ = _pos;
      for(_i = _lenDst ; _i >= _temp_ ; _i += -1) {
        (* PTR_INDEX(_dst, _i+_lenSrc, __dst_0, 1, 1418)) = (* PTR_INDEX(_dst, _i, __dst_0, 1, 1435));
      }
    }
  } else {
    (* PTR_INDEX(_dst, _maxDst, __dst_0, 1, 1467)) = '\000';
    _lenSrc = _maxDst- _pos;
  }
  {
    INTEGER _temp_ = _lenSrc- 1;
    for(_i = 0 ; _i <= _temp_ ; _i += 1) {
      (* PTR_INDEX(_dst, _pos+_i, __dst_0, 1, 1604)) = (* PTR_INDEX(_src, _i, __src_0, 1, 1618));
    }
  }
}

void Strings_Append (LONGINT __s_0, const CHAR (* __s_p), LONGINT __dst_0, CHAR (* _dst)) {
  CHAR (* _s);
  INTEGER _sp, _dp, _m;
  VALUE_ARRAY(_s, __s_p, 1*__s_0);
  _m = (INTEGER) __dst_0- 1;
  _dp = Strings_Length(__dst_0, _dst);
  _sp = 0;
  while (_dp<_m&&(* PTR_INDEX(_s, _sp, __s_0, 1, 1979))!='\000')  {
    (* PTR_INDEX(_dst, _dp, __dst_0, 1, 2037)) = (* PTR_INDEX(_s, _sp, __s_0, 1, 2046));
    INCI(_dp, 1, 2058);
    INCI(_sp, 1, 2074);
  }
  (* PTR_INDEX(_dst, _dp, __dst_0, 1, 2099)) = '\000';
}

void Strings_Delete (LONGINT __s_0, CHAR (* _s), INTEGER _pos, INTEGER _n) {
  INTEGER _lenStr, _i;
  _lenStr = Strings_Length(__s_0, _s);
  if (_pos+_n<_lenStr)  {
    {
      INTEGER _temp_ = _lenStr- _n;
      for(_i = _pos ; _i <= _temp_ ; _i += 1) {
        (* PTR_INDEX(_s, _i, __s_0, 1, 2494)) = (* PTR_INDEX(_s, _i+_n, __s_0, 1, 2502));
      }
    }
  } else {
    (* PTR_INDEX(_s, _pos, __s_0, 1, 2534)) = '\000';
  }
}

void Strings_Replace (LONGINT __src_0, const CHAR (* __src_p), INTEGER _pos, LONGINT __dst_0, CHAR (* _dst)) {
  CHAR (* _src);
  INTEGER _sp, _maxDst;
  BOOLEAN _addNull;
  VALUE_ARRAY(_src, __src_p, 1*__src_0);
  _maxDst = (INTEGER) __dst_0- 1;
  _addNull = FALSE;
  _sp = 0;
  while ((* PTR_INDEX(_src, _sp, __src_0, 1, 2923))!='\000'&&_pos<_maxDst)  {
    _addNull = _addNull||(* PTR_INDEX(_dst, _pos, __dst_0, 1, 3080))=='\000';
    (* PTR_INDEX(_dst, _pos, __dst_0, 1, 3102)) = (* PTR_INDEX(_src, _sp, __src_0, 1, 3114));
    INCI(_pos, 1, 3126);
    INCI(_sp, 1, 3143);
  }
  if (_addNull)  {
    (* PTR_INDEX(_dst, _pos, __dst_0, 1, 3190)) = '\000';
  }
}

void Strings_Extract (LONGINT __src_0, const CHAR (* __src_p), INTEGER _pos, INTEGER _n, LONGINT __dst_0, CHAR (* _dst)) {
  CHAR (* _src);
  INTEGER _i;
  VALUE_ARRAY(_src, __src_p, 1*__src_0);
  if ((LONGINT) _n>__dst_0)  {
    _n = (INTEGER) __dst_0- 1;
  }
  _i = 0;
  while (_i<_n&&(* PTR_INDEX(_src, _pos+_i, __src_0, 1, 3914))!='\000')  {
    (* PTR_INDEX(_dst, _i, __dst_0, 1, 3940)) = (* PTR_INDEX(_src, _pos+_i, __src_0, 1, 3950));
    INCI(_i, 1, 3965);
  }
  (* PTR_INDEX(_dst, _i, __dst_0, 1, 3989)) = '\000';
}

INTEGER Strings_Pos (LONGINT __pat_0, const CHAR (* __pat_p), LONGINT __s_0, const CHAR (* __s_p), INTEGER _pos) {
  CHAR (* _pat);
  CHAR (* _s);
  INTEGER _posPat;
  VALUE_ARRAY(_pat, __pat_p, 1*__pat_0);
  VALUE_ARRAY(_s, __s_p, 1*__s_0);
  _posPat = 0;
  while(1) {
    if ((* PTR_INDEX(_pat, _posPat, __pat_0, 1, 4292))=='\000')  {
      return _pos- _posPat;
    } else if ((* PTR_INDEX(_s, _pos, __s_0, 1, 4389))=='\000')  {
      return -1;
    } else if ((* PTR_INDEX(_s, _pos, __s_0, 1, 4489))==(* PTR_INDEX(_pat, _posPat, __pat_0, 1, 4500)))  {
      INCI(_pos, 1, 4567);
      INCI(_posPat, 1, 4578);
    } else {
      _pos = (_pos- _posPat)+1;
      _posPat = 0;
    }
  }
  NO_RETURN (4025);
}

void Strings_Cap (LONGINT __s_0, CHAR (* _s)) {
  INTEGER _i;
  _i = 0;
  while ((* PTR_INDEX(_s, _i, __s_0, 1, 4921))!='\000')  {
    (* PTR_INDEX(_s, _i, __s_0, 1, 4941)) = CAP((* PTR_INDEX(_s, _i, __s_0, 1, 4954)));
    INCI(_i, 1, 4966);
  }
}

void _init_Strings (void) {
  moduleId = add_module ("Strings");
}
