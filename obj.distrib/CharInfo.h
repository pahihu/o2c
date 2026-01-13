#ifndef __CharInfo_
#define __CharInfo_

#include "_OGCC.h"



extern const CHAR CharInfo_nul;
extern const CHAR CharInfo_soh;
extern const CHAR CharInfo_stx;
extern const CHAR CharInfo_etx;
extern const CHAR CharInfo_eot;
extern const CHAR CharInfo_enq;
extern const CHAR CharInfo_ack;
extern const CHAR CharInfo_bel;
extern const CHAR CharInfo_bs;
extern const CHAR CharInfo_ht;
extern const CHAR CharInfo_lf;
extern const CHAR CharInfo_vt;
extern const CHAR CharInfo_ff;
extern const CHAR CharInfo_cr;
extern const CHAR CharInfo_so;
extern const CHAR CharInfo_si;
extern const CHAR CharInfo_dle;
extern const CHAR CharInfo_dc1;
extern const CHAR CharInfo_dc2;
extern const CHAR CharInfo_dc3;
extern const CHAR CharInfo_dc4;
extern const CHAR CharInfo_nak;
extern const CHAR CharInfo_syn;
extern const CHAR CharInfo_etb;
extern const CHAR CharInfo_can;
extern const CHAR CharInfo_em;
extern const CHAR CharInfo_sub;
extern const CHAR CharInfo_esc;
extern const CHAR CharInfo_fs;
extern const CHAR CharInfo_gs;
extern const CHAR CharInfo_rs;
extern const CHAR CharInfo_us;
extern const CHAR CharInfo_del;
extern const CHAR CharInfo_xon;
extern const CHAR CharInfo_xoff;
extern const CHAR CharInfo_eol;


extern BOOLEAN CharInfo_IsEOL (CHAR _ch);
extern BOOLEAN CharInfo_IsDigit (CHAR _ch);
extern BOOLEAN CharInfo_IsSpace (CHAR _ch);
extern BOOLEAN CharInfo_IsSign (CHAR _ch);
extern BOOLEAN CharInfo_IsLetter (CHAR _ch);
extern BOOLEAN CharInfo_IsUpper (CHAR _ch);
extern BOOLEAN CharInfo_IsLower (CHAR _ch);
extern BOOLEAN CharInfo_IsControl (CHAR _ch);
extern CHAR CharInfo_EOL (void);

extern void _init_CharInfo (void);

#endif
