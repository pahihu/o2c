#include "_OGCC.h"

#define num_types 7
#define get_size(t, n, i) strcpy(sizes[i]. name, n); sizes[i]. size = sizeof(t)
#define check_size(t, n, expected) \
  if (sizeof(t) != expected) { \
    printf("error: sizeof(%s) is %i, not %i.\n", n, sizeof(t), expected);  \
    failed = TRUE;  \
  }

struct {
  char name[16];
  int size;
} sizes[num_types];

typedef enum {
  enum0, enum1
} SimpleEnum;



void get_sizes (void) {
  get_size(char, "char", 0);
  get_size(short int, "short int", 1);
  get_size(int, "int", 2);
  get_size(long int, "long int", 3);
  get_size(float, "float", 4);
  get_size(double, "double", 5);
  get_size(void*, "void*", 6);
}

void print_sizes (void) {
  int i;
  
  for(i = 0;  i != num_types; i++) {
    printf("%12s   %i bytes\n", sizes[i]. name, sizes[i]. size);
  }
}

int check_sizes (void) {
  int failed = FALSE;

  check_size (BOOLEAN, "BOOLEAN", 1);
  check_size (CHAR, "CHAR", 1);
  check_size (BYTE, "BYTE", 1);
  check_size (SHORTINT, "SHORTINT", SIZEOF_SHORTINT);
  check_size (INTEGER, "INTEGER", SIZEOF_INTEGER);
  check_size (LONGINT, "LONGINT", SIZEOF_LONGINT);
  check_size (REAL, "REAL", 4);
  check_size (LONGREAL, "LONGREAL", 8);
  check_size (SET, "SET", 4);
  check_size (PTR, "PTR", SIZEOF_PTR);
  return failed;
}

int main (int argc) {
 if (check_sizes()) {
   printf("\nThe sizes of the standard data types differ from the ones' that are hardcoded\n"
	  "into the compiler.  Sorry, your system is not supported by o2c.\n");
   get_sizes();
   print_sizes();
   check_sizes();
   return 1;
 } else {
   if (sizeof(SimpleEnum) != sizeof(int)) {
     printf ("WARNING: \n"
	     "\tYour gcc uses short enum types.  Please change the type\n"
	     "\tdeclarations in lib/CType.Mod to:\n"
	     "\tTYPE enum1*=SHORTINT; enum2*=INTEGER; enum4*=LONGINT;\n");
   }
   printf("Type sizes are ok.\n");
   if (argc != 1) {
     printf ("\nLocal type sizes:\n");
     get_sizes();
     print_sizes();
   }
   return 0;
 }
}
