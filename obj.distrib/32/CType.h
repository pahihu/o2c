#ifndef __CType_
#define __CType_

#include "_OGCC.h"

typedef CHAR CType_char;
typedef SHORTINT CType_signedchar;
typedef INTEGER CType_shortint;
typedef LONGINT CType_int;
typedef LONGINT CType_longint;
typedef LONGINT CType_address;
typedef REAL CType_float;
typedef LONGREAL CType_double;
typedef LONGINT CType_enum1;
typedef LONGINT CType_enum2;
typedef LONGINT CType_enum4;
typedef LONGINT CType_FILE;
typedef LONGINT CType_size_t;
typedef LONGINT CType_uid_t;
typedef LONGINT CType_gid_t;
typedef CHAR (* CType_charPtr1d);
typedef CType_charPtr1d (* CType_charPtr2d);
typedef LONGINT (* CType_intPtr1d);
typedef void (* CType_Proc) (void);

extern void _init_CType (void);

#endif
