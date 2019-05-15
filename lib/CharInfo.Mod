MODULE CharInfo;

CONST
      (* standard short character names for control chars *)
  nul* = 00X;    soh* = 01X;      stx* = 02X;      etx* = 03X;
  eot* = 04X;    enq* = 05X;      ack* = 06X;      bel* = 07X;
  bs * = 08X;    ht * = 09X;      lf * = 0AX;      vt * = 0BX;
  ff * = 0CX;    cr * = 0DX;      so * = 0EX;      si * = 0FX;
  dle* = 10X;    dc1* = 11X;      dc2* = 12X;      dc3* = 13X;
  dc4* = 14X;    nak* = 15X;      syn* = 16X;      etb* = 17X;
  can* = 18X;    em * = 19X;      sub* = 1AX;      esc* = 1BX;
  fs * = 1CX;    gs * = 1DX;      rs * = 1EX;      us * = 1FX;
  del* = 7FX;
      (* standard synonyms for certain control characters *)
  xon* = dc1;    xoff* = dc3;
      (* system dependent control characters *)
  eol* = 0AX;


PROCEDURE IsEOL* (ch : CHAR) : BOOLEAN;
  BEGIN
    RETURN (ch = eol)
  END IsEOL;

PROCEDURE IsDigit* (ch : CHAR) : BOOLEAN;
  BEGIN
    RETURN (ch >= "0") & (ch <= "9")
  END IsDigit;

PROCEDURE IsSpace* (ch : CHAR) : BOOLEAN;
  BEGIN
    RETURN (ch <= " ") & (ch # 0X)
  END IsSpace;

PROCEDURE IsSign* (ch : CHAR) : BOOLEAN;
  BEGIN
    RETURN (ch = "+") OR (ch = "-")
  END IsSign;

PROCEDURE IsLetter* (ch : CHAR) : BOOLEAN;
  BEGIN
    RETURN ("A" <= CAP (ch)) & (CAP (ch) <= "Z")
  END IsLetter;

PROCEDURE IsUpper* (ch : CHAR) : BOOLEAN;
  BEGIN
    RETURN ("A" <= ch) & (ch <= "Z")
  END IsUpper;

PROCEDURE IsLower* (ch : CHAR) : BOOLEAN;
  BEGIN
    RETURN ("a" <= ch) & (ch <= "z")
  END IsLower;

PROCEDURE IsControl* (ch : CHAR) : BOOLEAN;
  BEGIN
    RETURN (ch < " ")
  END IsControl;

PROCEDURE EOL*() : CHAR;
  BEGIN
    RETURN eol
  END EOL;

END CharInfo.
