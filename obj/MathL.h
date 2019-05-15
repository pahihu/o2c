#ifndef __MathL_
#define __MathL_

#include "_OGCC.h"

extern LONGREAL sqrt (LONGREAL x);
extern LONGREAL pow (LONGREAL x, LONGREAL base);
extern LONGREAL exp (LONGREAL x);
extern LONGREAL log (LONGREAL x);
extern LONGREAL MathL_log (LONGREAL x, LONGREAL base);
extern LONGREAL MathL_round (LONGREAL x);
extern LONGREAL sin (LONGREAL x);
extern LONGREAL cos (LONGREAL x);
extern LONGREAL tan (LONGREAL x);
extern LONGREAL asin (LONGREAL x);
extern LONGREAL acos (LONGREAL x);
extern LONGREAL atan (LONGREAL x);
extern LONGREAL atan2 (LONGREAL xn, LONGREAL xd);
extern LONGREAL sinh (LONGREAL x);
extern LONGREAL cosh (LONGREAL x);
extern LONGREAL tanh (LONGREAL x);
extern LONGREAL modf (LONGREAL value, LONGREAL (* MathL_integerPart));

extern void _init_MathL (void);

#endif
