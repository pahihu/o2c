#include "CharInfo.h"

static ModuleId moduleId;



const CHAR CharInfo_nul = '\000';
const CHAR CharInfo_soh = '\001';
const CHAR CharInfo_stx = '\002';
const CHAR CharInfo_etx = '\003';
const CHAR CharInfo_eot = '\004';
const CHAR CharInfo_enq = '\005';
const CHAR CharInfo_ack = '\006';
const CHAR CharInfo_bel = '\007';
const CHAR CharInfo_bs = '\010';
const CHAR CharInfo_ht = '\011';
const CHAR CharInfo_lf = '\012';
const CHAR CharInfo_vt = '\013';
const CHAR CharInfo_ff = '\014';
const CHAR CharInfo_cr = '\015';
const CHAR CharInfo_so = '\016';
const CHAR CharInfo_si = '\017';
const CHAR CharInfo_dle = '\020';
const CHAR CharInfo_dc1 = '\021';
const CHAR CharInfo_dc2 = '\022';
const CHAR CharInfo_dc3 = '\023';
const CHAR CharInfo_dc4 = '\024';
const CHAR CharInfo_nak = '\025';
const CHAR CharInfo_syn = '\026';
const CHAR CharInfo_etb = '\027';
const CHAR CharInfo_can = '\030';
const CHAR CharInfo_em = '\031';
const CHAR CharInfo_sub = '\032';
const CHAR CharInfo_esc = '\033';
const CHAR CharInfo_fs = '\034';
const CHAR CharInfo_gs = '\035';
const CHAR CharInfo_rs = '\036';
const CHAR CharInfo_us = '\037';
const CHAR CharInfo_del = (CHAR)'\177';
const CHAR CharInfo_xon = '\021';
const CHAR CharInfo_xoff = '\023';
const CHAR CharInfo_eol = '\012';



BOOLEAN CharInfo_IsEOL (CHAR _ch) {
  return _ch==CharInfo_eol;
  NO_RETURN (768);
}

BOOLEAN CharInfo_IsDigit (CHAR _ch) {
  return _ch>='0'&&_ch<='9';
  NO_RETURN (852);
}

BOOLEAN CharInfo_IsSpace (CHAR _ch) {
  return _ch<=' '&&_ch!='\000';
  NO_RETURN (955);
}

BOOLEAN CharInfo_IsSign (CHAR _ch) {
  return _ch=='+'||_ch=='-';
  NO_RETURN (1056);
}

BOOLEAN CharInfo_IsLetter (CHAR _ch) {
  return 'A'<=CAP(_ch)&&CAP(_ch)<='Z';
  NO_RETURN (1156);
}

BOOLEAN CharInfo_IsUpper (CHAR _ch) {
  return 'A'<=_ch&&_ch<='Z';
  NO_RETURN (1273);
}

BOOLEAN CharInfo_IsLower (CHAR _ch) {
  return 'a'<=_ch&&_ch<='z';
  NO_RETURN (1376);
}

BOOLEAN CharInfo_IsControl (CHAR _ch) {
  return _ch<' ';
  NO_RETURN (1479);
}

CHAR CharInfo_EOL (void) {
  return CharInfo_eol;
  NO_RETURN (1571);
}

void _init_CharInfo (void) {
  moduleId = add_module ("CharInfo");
}
