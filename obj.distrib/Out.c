#include "Out.h"
#include "CType.h"
#include "ConvTypes.h"
#include "IntStr.h"
#include "RealStr.h"
#include "Strings.h"
#include "Unix.h"

static ModuleId moduleId;



static const SHORTINT Out_fieldWidth = 32;



void Out_Open (void) {
}

void Out_Char (CHAR _ch) {
  LONGINT _res;
  _res = fputc((LONGINT) ((INTEGER) _ch), Unix_stdout);
}

void Out_String (LONGINT __s_0, const CHAR (* __s_p)) {
  CHAR (* _s);
  LONGINT _res;
  VALUE_ARRAY(_s, __s_p, 1*__s_0);
  _res = fputs(_s, Unix_stdout);
}

void Out_Int (LONGINT _i, LONGINT _n) {
  CHAR _str[1024];
  IntStr_Give(1024, (CHAR *) _str, _i, (INTEGER) _n, ConvTypes_right);
  Out_String(1024, (CHAR *) _str);
}

void Out_Real2Str (LONGREAL _x, INTEGER _sigFigs, INTEGER _expCyphers, LONGINT __str_0, CHAR (* _str)) {
  INTEGER _e, _i;
  RealStr_GiveFloat(__str_0, _str, _x, _sigFigs, (INTEGER) __str_0- 1, ConvTypes_left);
  _e = 0;
  while ((* PTR_INDEX(_str, _e, __str_0, 1, 2267))!='E')  {
    INCI(_e, 1, 2287);
  }
  _i = _e+1;
  while ((* PTR_INDEX(_str, _i, __str_0, 1, 2365))!=' '&&(* PTR_INDEX(_str, _i, __str_0, 1, 2382))!='\000')  {
    INCI(_i, 1, 2401);
  }
  (* PTR_INDEX(_str, _i, __str_0, 1, 2425)) = '\000';
  while ((_i- _e)- 2<_expCyphers)  {
    Strings_Insert(2, (CHAR *) "0", _e+2, __str_0, _str);
    INCI(_i, 1, 2570);
  }
  _i = _e;
  while ((* PTR_INDEX(_str, _i- 1, __str_0, 1, 2649))=='0')  {
    DECI(_i, 1, 2671);
  }
  if ((* PTR_INDEX(_str, _i- 1, __str_0, 1, 2699))=='.')  {
    INCI(_i, 1, 2766);
  }
  Strings_Delete(__str_0, _str, _i, _e- _i);
}

void Out_Real (REAL _x, INTEGER _n) {
  CHAR _str[33];
  Out_Real2Str((LONGREAL) _x, 7, 2, 33, (CHAR *) _str);
  DECI(_n, Strings_Length(33, (CHAR *) _str), 3357);
  while (_n>0)  {
    Out_Char(' ');
    DECI(_n, 1, 3433);
  }
  Out_String(33, (CHAR *) _str);
}

void Out_LongReal (LONGREAL _x, INTEGER _n) {
  CHAR _str[33];
  Out_Real2Str(_x, 16, 3, 33, (CHAR *) _str);
  DECI(_n, Strings_Length(33, (CHAR *) _str), 4011);
  while (_n>0)  {
    Out_Char(' ');
    DECI(_n, 1, 4087);
  }
  Out_String(33, (CHAR *) _str);
}

void Out_Ln (void) {
  Out_Char('\012');
}

void Out_Flush (void) {
  LONGINT _res;
  _res = fflush(Unix_stdout);
}

void _init_Out (void) {
  moduleId = add_module ("Out");
}
