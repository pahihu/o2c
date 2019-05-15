#include "MathL.h"
#include "CType.h"
#include "Unix.h"
#include "Time.h"
#include "CharInfo.h"
#include "ConvTypes.h"
#include "Dos.h"
#include "Strings.h"
#include "IntStr.h"
#include "Strings2.h"
#include "Filenames.h"
#include "RealStr.h"
#include "Rts.h"
#include "Files.h"
#include "Out.h"
#include "FOut.h"
#include "Redir.h"
#include "OMachine.h"
#include "OScan.h"
#include "ODepend.h"
#include "OTable.h"
#include "OEParse.h"
#include "OGenGCC.h"
#include "OMakeGCC.h"
#include "OSym.h"
#include "OParse.h"
#include "o2c.h"

int main(int argc, char **argv) {
  Rts_argc = argc;
  Rts_argv = (void*)argv;
  _init__OGCC();
  _init_MathL();
  _init_Unix();
  _init_Time();
  _init_CharInfo();
  _init_ConvTypes();
  _init_Dos();
  _init_Strings();
  _init_IntStr();
  _init_Strings2();
  _init_Filenames();
  _init_RealStr();
  _init_Rts();
  _init_Files();
  _init_Out();
  _init_FOut();
  _init_Redir();
  _init_OMachine();
  _init_OScan();
  _init_ODepend();
  _init_OTable();
  _init_OEParse();
  _init_OGenGCC();
  _init_OMakeGCC();
  _init_OSym();
  _init_OParse();
  _init_o2c();
  return Rts_exitCode;
}