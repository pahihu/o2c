#ifndef __CType_
#define __CType_

#include "_OGCC.h"

typedef CHAR CType_char;
typedef CHAR CType_signedchar;
typedef SHORTINT CType_shortint;
typedef INTEGER CType_int;
typedef LONGINT CType_longint;
typedef LONGINT CType_address;
typedef REAL CType_float;
typedef LONGREAL CType_double;
typedef INTEGER CType_enum1;
typedef INTEGER CType_enum2;
typedef INTEGER CType_enum4;
typedef LONGINT CType_FILE;
typedef LONGINT CType_size_t;
typedef INTEGER CType_uid_t;
typedef INTEGER CType_gid_t;
typedef CHAR (* CType_charPtr1d);
typedef CType_charPtr1d (* CType_charPtr2d);
typedef LONGINT (* CType_intPtr1d);
typedef void (* CType_Proc) (void);

extern void _init_CType (void);

#endif
