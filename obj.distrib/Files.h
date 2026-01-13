#ifndef __Files_
#define __Files_

#include "_OGCC.h"
#include "CType.h"
#include "Rts.h"

typedef struct Files_Buffer {
  BYTE data[2048];
  LONGINT start;
  LONGINT end;
  BOOLEAN read;
} Files_Buffer;
typedef struct Files_FileDesc (* Files_File);
typedef struct Files_FileDesc {
  Files_File next;
  LONGINT fd;
  CHAR name[256];
  CHAR tmpn[256];
  BOOLEAN flagMark;
  BOOLEAN flagValid;
  BOOLEAN flagRead;
  BOOLEAN flagWrite;
  BOOLEAN flagReg;
  BOOLEAN flagKeepFd;
  LONGINT pos;
  struct Files_Buffer buffer;
} Files_FileDesc;
typedef struct Files_Rider {
  Files_File file;
  BOOLEAN eof;
  INTEGER res;
  LONGINT pos;
} Files_Rider;
extern BOOLEAN Files_permanentClose;
extern Files_File Files_Old (const CHAR (* name));
extern Files_File Files_New (const CHAR (* name));
extern void Files_Delete (const CHAR (* name), INTEGER (* Files_res));
extern LONGINT Files_Length (Files_File f);
extern void Files_Close (Files_File (* Files_f));
extern void Files_Register (Files_File f);
extern void Files_Purge (Files_File f);
extern void Files_Rename (const CHAR (* old), const CHAR (* new), INTEGER (* Files_res));
extern void Files_GetDate (Files_File f, LONGINT (* Files_t), LONGINT (* Files_d));
extern void Files_Set (struct Files_Rider (* Files_r), Files_File f, LONGINT pos);
extern LONGINT Files_Pos (struct Files_Rider (* Files_r));
extern Files_File Files_Base (struct Files_Rider (* Files_r));
extern void Files_ReadBytes (struct Files_Rider (* Files_r), BYTE (* Files_x), LONGINT n);
extern void Files_WriteBytes (struct Files_Rider (* Files_r), BYTE (* Files_x), LONGINT n);
extern void Files_Read (struct Files_Rider (* Files_r), BYTE (* Files_x));
extern void Files_ReadInt (struct Files_Rider (* Files_r), INTEGER (* Files_x));
extern void Files_ReadLInt (struct Files_Rider (* Files_r), LONGINT (* Files_x));
extern void Files_ReadReal (struct Files_Rider (* Files_r), REAL (* Files_x));
extern void Files_ReadLReal (struct Files_Rider (* Files_r), LONGREAL (* Files_x));
extern void Files_ReadNum (struct Files_Rider (* Files_r), LONGINT (* Files_x));
extern void Files_ReadString (struct Files_Rider (* Files_r), CHAR (* Files_x));
extern void Files_ReadSet (struct Files_Rider (* Files_r), SET (* Files_x));
extern void Files_ReadBool (struct Files_Rider (* Files_r), BOOLEAN (* Files_x));
extern void Files_Write (struct Files_Rider (* Files_r), BYTE x);
extern void Files_WriteInt (struct Files_Rider (* Files_r), INTEGER x);
extern void Files_WriteLInt (struct Files_Rider (* Files_r), LONGINT x);
extern void Files_WriteReal (struct Files_Rider (* Files_r), REAL x);
extern void Files_WriteLReal (struct Files_Rider (* Files_r), LONGREAL x);
extern void Files_WriteNum (struct Files_Rider (* Files_r), LONGINT x);
extern void Files_WriteString (struct Files_Rider (* Files_r), const CHAR (* x));
extern void Files_WriteSet (struct Files_Rider (* Files_r), SET x);
extern void Files_WriteBool (struct Files_Rider (* Files_r), BOOLEAN x);

extern void _init_Files (void);

#endif
