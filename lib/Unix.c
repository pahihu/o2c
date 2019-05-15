#include <stdio.h>
#include <errno.h>
#include <math.h>

FILE *Unix_stdin, *Unix_stdout, *Unix_stderr;

int Unix_Errno () {
  /* volatile variables can't be accessed from Oberon-2 without risk */
  return errno;
}

unsigned char Unix_is_hugeval(double x) {
  return ((x >= HUGE_VAL) || (x <= -HUGE_VAL));
}

void _init_Unix (void) {
  Unix_stdin = stdin;
  Unix_stdout = stdout;
  Unix_stderr = stderr;
}

