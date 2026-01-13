#include "FOut.h"
#include "CharInfo.h"
#include "ConvTypes.h"
#include "Files.h"
#include "IntStr.h"
#include "Out.h"
#include "RealStr.h"
#include "Rts.h"
#include "Strings.h"

static ModuleId moduleId;



static const INTEGER FOut_sizeBuffer = 8192;
static const SHORTINT FOut_fieldWidth = 32;
typedef CHAR (* FOut_Buffer)[8192];
typedef CHAR FOut_BufferDesc[8192];
static Files_File FOut_file;
static CHAR FOut_writeErrorMsg[288];
static struct Files_Rider FOut_rider;
static FOut_Buffer FOut_buf;
static INTEGER FOut_pos;



void FOut_Open (LONGINT __name_0, const CHAR (* __name_p)) {
  CHAR (* _name);
  VALUE_ARRAY(_name, __name_p, 1*__name_0);
  FOut_file = Files_New(_name);
  if ((void*)FOut_file!=(void*)NULL)  {
    Files_Set(&(FOut_rider), FOut_file, 0);
    if ((void*)FOut_buf==(void*)NULL)  {
      NEWFIX(FOut_buf, 1);
    }
    FOut_pos = 0;
    COPYSTRING(&(FOut_writeErrorMsg), "[FOut] Failed to write to file ");
    Strings_Append(__name_0, _name, 288, (CHAR *) FOut_writeErrorMsg);
  } else {
    COPYSTRING(&(FOut_writeErrorMsg), "[FOut] Couldn't create output file ");
    Strings_Append(__name_0, _name, 288, (CHAR *) FOut_writeErrorMsg);
    Rts_Error((CHAR *) FOut_writeErrorMsg);
  }
}

void FOut_Flush (void) {
  Files_WriteBytes(&(FOut_rider), (BYTE*)CHECK_NIL(FOut_Buffer, FOut_buf, 1734), (LONGINT) FOut_pos);
  if (FOut_rider.res!=0)  {
    Rts_Error((CHAR *) FOut_writeErrorMsg);
  } else {
    FOut_pos = 0;
  }
}

void FOut_Close (void) {
  FOut_Flush();
  Files_Register(FOut_file);
  Files_Close(&(FOut_file));
}

void FOut_Char (CHAR _ch) {
  if (FOut_pos==FOut_sizeBuffer)  {
    FOut_Flush();
  }
  INDEX(DEREF(FOut_Buffer, FOut_buf, 2112), FOut_pos, 8192, 2112) = _ch;
  INCI(FOut_pos, 1, 2129);
}

void FOut_String (LONGINT __str_0, const CHAR (* __str_p)) {
  CHAR (* _str);
  INTEGER _i;
  VALUE_ARRAY(_str, __str_p, 1*__str_0);
  _i = 0;
  while ((* PTR_INDEX(_str, _i, __str_0, 1, 2302))!='\000')  {
    if (FOut_pos==FOut_sizeBuffer)  {
      FOut_Flush();
    }
    INDEX(DEREF(FOut_Buffer, FOut_buf, 2382), FOut_pos, 8192, 2382) = (* PTR_INDEX(_str, _i, __str_0, 1, 2394));
    INCI(FOut_pos, 1, 2405);
    INCI(_i, 1, 2422);
  }
}

void FOut_Ln (void) {
  FOut_Char(CharInfo_lf);
}

void FOut_Int (LONGINT _i, LONGINT _n) {
  CHAR _str[32];
  IntStr_Give(32, (CHAR *) _str, _i, (INTEGER) _n, ConvTypes_right);
  FOut_String(32, (CHAR *) _str);
}

void FOut_Real (REAL _x, INTEGER _n) {
  CHAR _str[33];
  Out_Real2Str((LONGREAL) _x, 7, 2, 33, (CHAR *) _str);
  DECI(_n, Strings_Length(33, (CHAR *) _str), 3339);
  while (_n>0)  {
    FOut_Char(' ');
    DECI(_n, 1, 3415);
  }
  FOut_String(33, (CHAR *) _str);
}

void FOut_LongReal (LONGREAL _x, INTEGER _n) {
  CHAR _str[33];
  Out_Real2Str(_x, 16, 3, 33, (CHAR *) _str);
  DECI(_n, Strings_Length(33, (CHAR *) _str), 3997);
  while (_n>0)  {
    FOut_Char(' ');
    DECI(_n, 1, 4073);
  }
  FOut_String(33, (CHAR *) _str);
}

void _init_FOut (void) {
  moduleId = add_module ("FOut");
  FOut_buf = (FOut_Buffer)NULL;
}
