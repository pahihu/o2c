#include "Time.h"
#include "CType.h"
#include "MathL.h"

static ModuleId moduleId;






void Time_Reset (struct Unix_timeval (* _time)) {
  (* _time).tv_sec = 0;
  (* _time).tv_usec = 0;
}

void Time_Add (struct Unix_timeval _a, struct Unix_timeval _b, struct Unix_timeval (* _result)) {
  (* _result).tv_sec = _b.tv_sec+_a.tv_sec;
  (* _result).tv_usec = _b.tv_usec+_a.tv_usec;
  if ((* _result).tv_usec>=1000000)  {
    INCLI((* _result).tv_sec, 1, 496);
    DECLI((* _result).tv_usec, 1000000, 524);
  }
}

void Time_Sub (struct Unix_timeval _a, struct Unix_timeval _b, struct Unix_timeval (* _result)) {
  (* _result).tv_sec = _b.tv_sec- _a.tv_sec;
  (* _result).tv_usec = _b.tv_usec- _a.tv_usec;
  if ((* _result).tv_usec<0)  {
    DECLI((* _result).tv_sec, 1, 831);
    INCLI((* _result).tv_usec, 1000000, 859);
  }
}

void Time_Mult (struct Unix_timeval _a, LONGREAL _m, struct Unix_timeval (* _result)) {
  LONGREAL _int, _frac;
  _frac = modf(((LONGREAL) _a.tv_sec+DIVLR((LONGREAL) _a.tv_usec, 1.0000000000000000E+06, 1148))*_m, &(_int));
  (* _result).tv_sec = (LONGINT) _int;
  (* _result).tv_usec = (LONGINT) (_frac*1.0000000000000000E+06);
  if ((* _result).tv_usec<0)  {
    DECLI((* _result).tv_sec, 1, 1288);
    INCLI((* _result).tv_usec, 1000000, 1316);
  } else if ((* _result).tv_usec>=1000000)  {
    INCLI((* _result).tv_sec, 1, 1397);
    DECLI((* _result).tv_usec, 1000000, 1425);
  }
}

INTEGER Time_Cmp (struct Unix_timeval _a, struct Unix_timeval _b) {
  if (_a.tv_sec>_b.tv_sec||(_a.tv_sec==_b.tv_sec&&_a.tv_usec>_b.tv_usec))  {
    return -1;
  } else if (_a.tv_sec==_b.tv_sec&&_a.tv_usec==_b.tv_usec)  {
    return 0;
  } else {
    return 1;
  }
  NO_RETURN (1488);
}

void Time_GetSysTime (struct Unix_timeval (* _time)) {
  LONGINT _res;
  _res = gettimeofday(_time, 0);
}

void Time_Since (struct Unix_timeval _start, struct Unix_timeval (* _elapsed)) {
  LONGINT _res;
  struct Unix_timeval _curr;
  _res = gettimeofday(&(_curr), 0);
  Time_Sub(_start, _curr, _elapsed);
}

void _init_Time (void) {
  moduleId = add_module ("Time");
}
