#include "_OGCC.h"
#include <math.h>


static ModuleId moduleId;


double MathL_round (double x) {
  double int_part, frac_part;

  frac_part = modf (x, &int_part);
  if (frac_part > 0.5) 
    return int_part+1;
  else if (frac_part >= 0.0)
    return int_part;
  else if (frac_part > -0.5)
    return int_part;
  else
    return int_part-1;
}

double MathL_log (double x, double base) {
  return log(x)/log(base);
}


void _init_MathL (void) {
  moduleId = add_module ("MathL");
}

