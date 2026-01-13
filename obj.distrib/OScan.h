#ifndef __OScan_
#define __OScan_

#include "_OGCC.h"



extern const CHAR OScan_undefStr[];
extern const SHORTINT OScan_undefPos;
extern const SHORTINT OScan_times;
extern const SHORTINT OScan_slash;
extern const SHORTINT OScan_div;
extern const SHORTINT OScan_mod;
extern const SHORTINT OScan_and;
extern const SHORTINT OScan_plus;
extern const SHORTINT OScan_minus;
extern const SHORTINT OScan_or;
extern const SHORTINT OScan_eql;
extern const SHORTINT OScan_neq;
extern const SHORTINT OScan_lss;
extern const SHORTINT OScan_leq;
extern const SHORTINT OScan_gtr;
extern const SHORTINT OScan_geq;
extern const SHORTINT OScan_in;
extern const SHORTINT OScan_is;
extern const SHORTINT OScan_arrow;
extern const SHORTINT OScan_period;
extern const SHORTINT OScan_comma;
extern const SHORTINT OScan_colon;
extern const SHORTINT OScan_upto;
extern const SHORTINT OScan_rParen;
extern const SHORTINT OScan_rBrak;
extern const SHORTINT OScan_rBrace;
extern const SHORTINT OScan_of;
extern const SHORTINT OScan_then;
extern const SHORTINT OScan_do;
extern const SHORTINT OScan_to;
extern const SHORTINT OScan_by;
extern const SHORTINT OScan_lParen;
extern const SHORTINT OScan_lBrak;
extern const SHORTINT OScan_lBrace;
extern const SHORTINT OScan_not;
extern const SHORTINT OScan_becomes;
extern const SHORTINT OScan_number;
extern const SHORTINT OScan_nil;
extern const SHORTINT OScan_string;
extern const SHORTINT OScan_ident;
extern const SHORTINT OScan_semicolon;
extern const SHORTINT OScan_bar;
extern const SHORTINT OScan_end;
extern const SHORTINT OScan_else;
extern const SHORTINT OScan_elsif;
extern const SHORTINT OScan_until;
extern const SHORTINT OScan_if;
extern const SHORTINT OScan_case;
extern const SHORTINT OScan_while;
extern const SHORTINT OScan_repeat;
extern const SHORTINT OScan_loop;
extern const SHORTINT OScan_for;
extern const SHORTINT OScan_with;
extern const SHORTINT OScan_exit;
extern const SHORTINT OScan_return;
extern const SHORTINT OScan_array;
extern const SHORTINT OScan_record;
extern const SHORTINT OScan_pointer;
extern const SHORTINT OScan_begin;
extern const SHORTINT OScan_const;
extern const SHORTINT OScan_type;
extern const SHORTINT OScan_var;
extern const SHORTINT OScan_procedure;
extern const SHORTINT OScan_import;
extern const SHORTINT OScan_module;
extern const SHORTINT OScan_endOfFile;
extern const SHORTINT OScan_numInt;
extern const SHORTINT OScan_numReal;
extern const SHORTINT OScan_numLReal;
extern const INTEGER OScan_minBufferSize;
extern const INTEGER OScan_maxBufferSize;
typedef CHAR OScan_FileName[256];
extern BOOLEAN OScan_warnings, OScan_verbose, OScan_underscore;
extern OScan_FileName OScan_sourceName;
extern BOOLEAN OScan_noerr;
extern LONGINT OScan_lastErr;
extern SHORTINT OScan_sym;
extern LONGINT OScan_lastSym;
extern CHAR OScan_ref[256];
extern SHORTINT OScan_numType;
extern LONGINT OScan_intVal;
extern LONGREAL OScan_realVal;


extern void OScan_VerboseMsg (LONGINT __msg_0, const CHAR (* __msg_p));
extern void OScan_ReadErrorList (void);
extern void OScan_GetErrMsg (INTEGER _num, LONGINT __str_0, CHAR (* _str));
extern void OScan_Replace (LONGINT __string_0, CHAR (* _string), LONGINT __insert_0, CHAR (* _insert));
extern void OScan_Err (LONGINT _pos, INTEGER _num);
extern void OScan_ErrIns (LONGINT _pos, INTEGER _num, LONGINT __ins_0, const CHAR (* __ins_p));
extern void OScan_ErrIns2 (LONGINT _pos, INTEGER _num, LONGINT __ins1_0, const CHAR (* __ins1_p), LONGINT __ins2_0, const CHAR (* __ins2_p));
extern void OScan_Warn (LONGINT _pos, INTEGER _num);
extern void OScan_WarnIns (LONGINT _pos, INTEGER _num, LONGINT __ins_0, const CHAR (* __ins_p));
extern LONGINT OScan_Line (LONGINT _pos);
extern LONGINT OScan_Col (LONGINT _pos);
extern void OScan_GetSym (void);
extern void OScan_LastPos (void);
extern void OScan_Init (LONGINT __fileName_0, const CHAR (* __fileName_p), INTEGER _bufferSize, BOOLEAN (* _err));
extern void OScan_Close (void);

extern void _init_OScan (void);

#endif
