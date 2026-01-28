#ifndef __OGCC__
#define __OGCC__

/* Uncomment this define if your compiler barfs about multiple
   prototypes for malloc() and free(). */
/*#define NOPROTOTYPES*/

/* Comment this define out, if your machine doesn't require
   LONGREAL (i.e, double, 8 bytes IEEE floating point number)
   to be aligned on 8 bytes. */
#define NEED8ALIGN


/* GCC 2.5 or above demanded
   GCC extensions used:
     nested functions 
     statement expressions
     alloca (a BSD extension)
*/


/* Uncomment the following defines to deactivate the specific pseudo-traps: */
/* don't trap at end of function without a return */
/*#define DISABLE_FUNCTION_TRAP*/

/* don't trap when a case expression doesn't match any label */
/*#define DISABLE_CASE_TRAP*/

/* don't trap at dereference of NIL pointer */
/*#define DISABLE_DEREF_TRAP*/

/* don't trap when an array indices is out of range */
/*#define DISABLE_INDEX_TRAP*/

/* don't trap when a set index is out of range */
/*#define DISABLE_SET_TRAP*/

/* don't trap when an assignment's left hand side's static and dynamic type differ */
/*#define DISABLE_ASSIGN_TRAP*/

/* don't trap when a type guard fails */
/*#define DISABLE_GUARD_TRAP*/

/* don't trap when a WITH statement fails */
/*#define DISABLE_RGUARD_TRAP*/

/* missing run time checks:
     overflow checks on arithmetic operations
     overflow checks when converting between different numeric types
   I don't see much reason to implement them on C level.
*/


/* disable all run time checks */
#ifdef DISABLE_RTC
#define DISABLE_FUNCTION_TRAP
#define DISABLE_CASE_TRAP
#define DISABLE_DEREF_TRAP 
#define DISABLE_INDEX_TRAP
#define DISABLE_SET_TRAP
#define DISABLE_ASSIGN_TRAP
#define DISABLE_GUARD_TRAP
#define DISABLE_RGUARD_TRAP
#endif


#include <math.h>
#include <float.h>
#ifdef __MINGW32__
// #include <string.h>
// #include <malloc.h>
#endif

#define BITS_PER_CHAR 8  /* seems reasonable */
#ifndef NULL
#define NULL (void*)0
#endif

/* C types that are used for the basic oberon types */
typedef unsigned char BOOLEAN;
typedef unsigned char CHAR;
#if __PTRDIFF_WIDTH__ == 64
#define __TAG_SIZE__ 8
typedef signed short int SHORTINT;
typedef signed int INTEGER;  /* see LI_FORMAT */
typedef signed long int LONGINT;
/* unsigned equivalents to the integer data types */
typedef unsigned short int USHORTINT;
typedef unsigned int UINTEGER;
typedef unsigned long int ULONGINT;
#define SIZEOF_SHORTINT 2
#define SIZEOF_INTEGER  4
#define SIZEOF_LONGINT  8
#define SIZEOF_PTR      8
#else
#define __TAG_SIZE__ 4
typedef signed char SHORTINT;
typedef signed short int INTEGER;  /* see LI_FORMAT */
typedef signed int LONGINT;
/* unsigned equivalents to the integer data types */
typedef unsigned char USHORTINT;
typedef unsigned short int UINTEGER;
typedef unsigned int ULONGINT;
#define SIZEOF_SHORTINT 1
#define SIZEOF_INTEGER  2
#define SIZEOF_LONGINT  4
#define SIZEOF_PTR      4
#endif
typedef float REAL;		/* see MAX_REAL/MIN_REAL */
typedef double LONGREAL;	/* see MAX_LONGREAL/MIN_LONGREAL */
typedef unsigned long SET;	/* see MAX_SET */
typedef unsigned char BYTE;
typedef void *PTR;


/* some function prototypes */
#ifndef NOPROTOTYPES
#include <sys/types.h>
extern void* malloc (size_t SIZE);
extern void free (void *PTR);
#endif
extern double fabs (double NUMBER);
extern void* memccpy (void *TO, const void *FROM, int C, size_t SIZE);
extern int toupper (int C);
extern char* strcpy(char *TO, const char *FROM);
extern void* memcpy(void *TO, const void *FROM, size_t SIZE);
//extern int printf(const char *FMT, ...);
#include <alloca.h>

#ifdef GC
  extern void* GC_malloc (int SIZE);
  extern void* GC_malloc_atomic (int SIZE);
  extern void  GC_register_displacement (int DISPL);
  extern void GC_free (void *PTR);
  extern void GC_gcollect (void);
  typedef void (*GC_finalization_proc)(void * obj, void * client_data);
  extern void GC_register_finalizer(void * obj,
                               GC_finalization_proc fn, void * cd,
                               GC_finalization_proc *ofn, void ** ocd);
  #define GC_COLLECT GC_gcollect();
  #define MALLOC GC_malloc
  #define MALLOC_ATOMIC GC_malloc_atomic
  #define FREE GC_free
#else
  #define GC_COLLECT 
  #define MALLOC malloc
  #define MALLOC_ATOMIC malloc
  #define FREE free
#endif


/* floating point MIN/MAX values */
#define MAX_REAL FLT_MAX
#define MIN_REAL (-FLT_MAX)
#define MAX_LONGREAL DBL_MAX
#define MIN_LONGREAL (-DBL_MAX)
/* the MIN/MAX values for the other types are hardcoded into
   the compiler, see module OMachine.Mod */

/* maximum set index/set value if all BITS_PER_CHAR are set */
/* format string used by printf for LONGINT (%i if int, %li if long int) */
#if __PTRDIFF_WIDTH__ == 64
#define MAX_SET   63
#define LI_FORMAT "%li"
#else
#define MAX_SET   31
#define LI_FORMAT "%i"
#endif

/* size of Oberon-2 identifiers */
#define SIZE_IDENT 48

/* definition of internal structures */

typedef struct TypeDesc* Type;
typedef INTEGER ModuleId;
typedef CHAR Name[SIZE_IDENT];
typedef struct ModuleDesc* Module;
typedef struct ModuleDesc {
  Module next;
  Name name;
  ModuleId key;
  Type tdlist;
} ModuleDesc;
#define TDCORE CHAR name[SIZE_IDENT]; Module module; INTEGER level; LONGINT size; Type next; 
typedef struct TypeDesc { TDCORE } TypeDesc;

extern Module module_list;
extern Type td_module;
extern Type td_type;

/* boolean constants */
#define TRUE 1
#define FALSE 0



/* macros expanding monadic operations */

/* absolute functions, one for each numeric type */
#define ABSSI(expr,pos) \
  ({SHORTINT x = (expr); x>0 ? x : -x; })
#define ABSI(expr,pos) abs(expr)
#define ABSLI(expr,pos) labs(expr)
#define ABSR(expr,pos) \
  ({REAL x = (expr); x>0 ? x : -x; })
#define ABSLR(expr,pos) fabs(expr)

/* ODD functions, one for each integer type */
#define ODDSI(expr) (((BOOLEAN) ((expr) & ((SHORTINT) 1))) != (SHORTINT) 0)
#define ODDI(expr) (((BOOLEAN) ((expr) & ((INTEGER) 1))) != (INTEGER) 0)
#define ODDLI(expr) (((BOOLEAN) ((expr) & ((LONGINT) 1))) != (LONGINT) 0)

/* CAP function */
#define CAP(char) ((CHAR) toupper((int) (char)))

/* ENTIER function */
#define ENTIERR(real) ((LONGINT) floorf(real))
#define ENTIERLR(real) ((LONGINT) floor(real))



/* macros expanding dyadic operations */

/* string comparison */
#define STREQL(a,b) ((BOOLEAN) (strcmp((a),(b)) == 0))
#define STRNEQ(a,b) ((BOOLEAN) (strcmp((a),(b)) != 0))
#define STRLSS(a,b) ((BOOLEAN) (strcmp((a),(b)) < 0))
#define STRLEQ(a,b) ((BOOLEAN) (strcmp((a),(b)) <= 0))
#define STRGRT(a,b) ((BOOLEAN) (strcmp((a),(b)) > 0))
#define STRGEQ(a,b) ((BOOLEAN) (strcmp((a),(b)) >= 0))

/* division and module macros (remainder has to be not negative) */
#define DIVI(num,denom,pos) \
  ({INTEGER n = (num), d = (denom), quot, rem; \
    quot= n / d; \
    rem = n % d; \
    if (rem < 0) \
      quot--; \
    quot; \
   })
#define DIVSI(num,denom,pos) ((SHORTINT) DIVI((int) num, (int) denom, pos))

#define DIVLI(num,denom,pos) \
  ({LONGINT n = (num), d = (denom), quot, rem; \
    quot = n / d; \
    rem  = n % d; \
    if (rem < 0) \
      quot--; \
    quot; \
   })
#define MODI(num,denom,pos) \
  ({INTEGER n = (num), d = (denom), rem; \
    rem =n % d; \
    if (rem < 0) \
      rem += d; \
    rem; \
   })

#define MODSI(num,denom,pos) ((SHORTINT) MODI((int) num, (int) denom, pos))

#define MODLI(num,denom,pos) \
  ({LONGINT n = (num), d = (denom), rem; \
    rem = n % d; \
    if (rem < 0) \
      rem += d; \
    rem; \
   })
    
#define DIVR(a,b,pos) ((a)/(b))  
#define DIVLR(a,b,pos) ((a)/(b))  

/* arithmetic shift operation */
extern LONGINT ash (LONGINT value, LONGINT dir);
#define ASHSI(val,n,pos) ash(val, n)
#define ASHI(val,n,pos) ash(val, n)
#define ASHLI(val,n,pos) ash(val, n)

/* logical shift */
#define LSHSI(val,n) \
({USHORTINT uval = (val), result; \
  int dir = (n); \
  if (dir >= 0) \
    result = uval << dir; \
  else \
    result = uval >> (-dir); \
  (SHORTINT)result; \
})
#define LSHI(val,n) \
({UINTEGER uval = (val), result; \
  int dir = (n); \
  if (dir >= 0) \
    result = uval << dir; \
  else \
    result = uval >> (-dir); \
  (INTEGER)result; \
})
#define LSHLI(val,n) \
({ULONGINT uval = (val), result; \
  int dir = (n); \
  if (dir >= 0) \
    result = uval << dir; \
  else \
    result = uval >> (-dir); \
  (LONGINT)result; \
})

#define LSHB(val,n) LSHSI(val,n)
#define LSHC(val,n) LSHSI(val,n)

/* rotation */
#define ROTATION(type,val,n) \
  ({SHORTINT bits = sizeof(type)*BITS_PER_CHAR, dir = (n) % bits, i; \
    type value = (val), result = 0; \
    if (dir < 0) dir += bits; \
    for (i=0; i < bits; i++) \
      if (((type) 1 << i) & value) \
	result |= ((type) 1 << ((i+dir) % bits)); \
    result; \
   })
#define ROTSI(val,n) ROTATION(SHORTINT,val,n)
#define ROTI(val,n) ROTATION(INTEGER,val,n)
#define ROTLI(val,n) ROTATION(LONGINT,val,n)
#define ROTB(val,n) ROTATION(BYTE,val,n)
#define ROTC(val,n) ROTATION(CHAR,val,n)



/* set macros */
/* IN relation */
#define IN(bit,set,pos) (((set) & (((SET) 1) << (SET_RANGE(bit, pos)))) != (SET) 0)

/* set difference */
#define SETDIFF(a,b) ((a)&(~(b)))

/* set include/exclude */
#define INCL(set,bit,pos) (set)|=((SET) 1) << (SET_RANGE(bit, pos))
#define EXCL(set,bit,pos) (set)&=(~(((SET) 1) << (SET_RANGE(bit, pos))))

/* set range {a..b} where a or b is not a constant (empty for a>b) */
#define RANGE(a,pos0,b,pos1) \
  ({LONGINT start = (a), end = (b), i; \
    SET set = 0UL; \
    SET bit = (SET) 1 << SET_RANGE(start, pos0); \
    for (i=start; i<=end; i++) { \
      set |= bit; bit <<= 1; \
    } \
    set; \
  })

/* SYSTEM.BIT */
#define BIT(adr, n) ({  \
  long int n = (n);  \
  BOOLEAN is_set = (((*(((SET*)(adr))+(n/(MAX_SET+1)))) & (((char)1) << (n % (MAX_SET+1)))) != ((SET)0));  \
  is_set;  \
})


/* macros expanding assignment operations */

/* integer increment/decrement */
#define INCSI(var,val,pos) (var)+=(val)
#define INCI(var,val,pos) (var)+=(val)
#define INCLI(var,val,pos) (var)+=(val)
#define DECSI(var,val,pos) (var)-=(val)
#define DECI(var,val,pos) (var)-=(val)
#define DECLI(var,val,pos) (var)-=(val)

/* string copy */
#define COPY(from,to,maxsize) \
  ({int _max = (int) (maxsize); \
    if (!memccpy ((to), (from), 0, _max)) \
      { char* _ptr = ((to) + _max -1); *_ptr = '\000';} \
  })

/* memory move */
#define MOVE(from,to,size) memcpy((void *) (to), (void *) (from), (int) (size))

/* SYSTEM.GET and SYSTEM.PUT */
#define PUT(adr,val,type) (*((type*)(adr))) = (val);
#define GET(adr,var,type) (var) = (*((type*)(adr)));



/* HALT */
#define HALT(trap) exit(trap)

/* copying of array values */
#define COPYARRAY(to,from,size) memcpy((to), (from), (size))
#define COPYARRAYT(to,from,type) memcpy((to), (from), sizeof(type))
#define COPYSTRING(to,from) strcpy((char*)(to), (char*)(from))
#define VALUE_ARRAY(to,from,size) to=alloca(sizeof(*from)*(size)); memcpy(to, from, sizeof(*from)*(size))
#define VALUE_ARRAYF(to,from,size) memcpy(to, from, sizeof(to)*(size))

/* type test */
extern BOOLEAN type_test (void* var_tag, void* type_tag, INTEGER type_level);
extern BOOLEAN type_test_ptr (void* variable, void* type_tag, INTEGER type_level, ModuleId id, LONGINT pos);

/* SYSTEM.VAL(type,expr) on expression (_not_ a designator) */
#define VAL_E(type,expr) ({  \
  typeof(expr) e = (expr);  \
  type (*result) = (void*)(&e);  \
  *result; })



/* open arrays as pointer base types */
extern void* tmpNew;		/* set by NEWDYN */
#define SET_LEN(dim,len) ((* (((LONGINT*) tmpNew)-1-dim)) = (len))
#define GET_LEN(var,dim) (* (((LONGINT*) (var))-1-dim))

/* managing type descriptors */
#define TAG(type, adr, pos) ((type*) (*((void**)(CHECK_NIL (void*, adr, pos))-1)))
#define TAG_NC(type, adr) ((type*) (*((void**)(adr)-1)))
#define SETTAG(adr, tag) {void **_padr = (void**)(adr)-1; *_padr = (tag);}
#define BASETYPE(td, level) (*((void**)td-2-(level)))
extern void* create_td (const char name[], int size_obj, int intd, void* base_td, int size_basetd);
extern ModuleId add_module (const char name[]);



/* heap managment */
#ifdef NEED8ALIGN
#define ALIGN(prefix) ((prefix)+((prefix) & 1))
#else
#define ALIGN(prefix) (prefix)
#endif

#define MALLOC_TAGGED(size,prefix_len) \
  ((void**)MALLOC((size)+(prefix_len)*__TAG_SIZE__)+(prefix_len))
#define MALLOC_TAGGED_ATOMIC(size,prefix_len) \
  ((void**)MALLOC_ATOMIC((size)+(prefix_len)*__TAG_SIZE__)+(prefix_len))

#define MALLOC_OBJECT(size,prefix_len,atomic) \
({ void* ptr;  \
  if (atomic)  \
    ptr = MALLOC_TAGGED_ATOMIC(size, ALIGN(prefix_len));  \
  else  \
    ptr = MALLOC_TAGGED(size, ALIGN(prefix_len));  \
  ptr;	\
})

/* allocate memory block for fixed size type */
#define NEWFIX(var,atomic) (var) = MALLOC_OBJECT(sizeof(*var), 0, atomic)

/* SYSTEM.NEW */
#define NEWSYS(var, size) (var) = (void*)MALLOC(size)

/* NEW(p,l0,l1,..) */
#define NEWDYN(var,size,dim,atomic) tmpNew=MALLOC_OBJECT((size)*sizeof(*(var)), dim, atomic); (var)=tmpNew
#define NEWDYN_EXT(var,size,dim,atomic) (var)=MALLOC_OBJECT((size)*sizeof(*(var)), 0, atomic)

/* allocate memory for record plus tag */
/* two pointers are prepended (not just one) to avoid alignment errors
   if a double (8 byte) value appears in the record being allocated */
#define NEWREC_SIZE(var,tag,size,atomic) { \
  void** ptr0 = MALLOC_OBJECT(size, 1, atomic); \
  (*(ptr0-1)) = (tag); \
  (var) = (void*)ptr0; \
}
#define NEWREC(var,tag,atomic) NEWREC_SIZE(var, tag, sizeof(*var), atomic)

/* SYSTEM.DISPOSE */
#define DISPOSE(var) FREE(var); (var)=NULL
#define DISPOSE_DYN(var,dim) FREE (((LONGINT*)(var))-ALIGN(dim)); (var)=NULL
#define DISPOSE_REC(var) FREE(((void**)(var))-ALIGN(1)); (var)=NULL


#ifndef DISABLE_ASSERT
#define ASSERT(guard,code,pos) if (!(guard)) trap_assert(code, moduleId, pos)
extern void trap_assert (LONGINT code, ModuleId id, LONGINT pos) __attribute__ ((noreturn));
#else
#define ASSERT(guard,code,pos)
#endif

#ifndef DISABLE_FUNCTION_TRAP
#define NO_RETURN(pos) trap_no_return (moduleId, pos)
extern void trap_no_return (ModuleId id, LONGINT pos) __attribute__ ((noreturn));
#else
#define NO_RETURN(pos)
#endif

#ifndef DISABLE_CASE_TRAP
#define NO_LABEL(expr,pos) trap_no_label (expr, moduleId, pos)
extern void trap_no_label (LONGINT expr, ModuleId id, LONGINT pos) __attribute__ ((noreturn));
#else
#define NO_LABEL(expr,pos)
#endif

#ifndef DISABLE_DEREF_TRAP
#define CHECK_NIL(type,ptr,pos) ((type) test_nil(ptr, moduleId, pos))
#define DEREF(type,ptr,pos) (*CHECK_NIL(type,ptr,pos))
extern void *test_nil (void *ptr, ModuleId id, LONGINT pos);
#else
#define CHECK_NIL(type,ptr,pos) ((type) (ptr))
#define DEREF(type,ptr,pos) (*((type) (ptr)))
#endif

#ifndef DISABLE_INDEX_TRAP
#define INDEX(array,index,length,pos) array[check_invalid_index(index, length, moduleId, pos)]
#define PTR_INDEX(ptr,index,length,size,pos) ((ptr) + check_invalid_index (index, length, moduleId, pos)*(size))
extern LONGINT check_invalid_index (LONGINT index, LONGINT length, ModuleId id, LONGINT pos);
#else
#define INDEX(array,index,length,pos) (array)[index]
#define PTR_INDEX(ptr,index,length,size,pos) ((ptr) + (index)*(size))
#endif
#define PTR_INDEX_EXT(ptr,index) ((ptr) + (index))

#ifndef DISABLE_SET_TRAP
#define SET_RANGE(index,pos) check_set_index(index, moduleId, pos)
extern SHORTINT check_set_index (LONGINT index, ModuleId id, LONGINT pos);
#else
#define SET_RANGE(index,pos) ((SHORTINT) index)
#endif

#ifndef DISABLE_ASSIGN_TRAP
#define ASSERT_TYPE(tag0,tag1,pos) check_type(tag0, tag1, moduleId, pos)
extern void check_type (void* tag0, void* tag1, ModuleId id, LONGINT pos);
#else
#define ASSERT_TYPE(tag0,tag1,pos)
#endif

#ifndef DISABLE_GUARD_TRAP
#define PTR_TYPE_GUARD(type,value,type_tag,type_level,pos) (type)type_guard_ptr(value,type_tag,type_level,moduleId,pos)
#define REC_TYPE_GUARD(type,value,var_tag,type_tag,type_level,pos) (*((type *) type_guard(value,var_tag,type_tag,type_level,moduleId,pos)))
extern void* type_guard (void* value, void* var_tag, void* type_tag, INTEGER type_level, ModuleId id, LONGINT pos);
extern void* type_guard_ptr (void* value, void* type_tag, INTEGER type_level, ModuleId id, LONGINT pos);
#else
#define PTR_TYPE_GUARD(type,value,type_tag,type_level,pos) ((type)(value))
#define REC_TYPE_GUARD(type,value,var_tag,type_tag,type_level,pos) (*((type*)(value)))
#endif

#ifndef DISABLE_RGUARD_TRAP
#define NO_GUARD(pos) trap_no_guard (moduleId, pos)
extern void trap_no_guard (ModuleId id, LONGINT pos) __attribute__ ((noreturn));
#else
#define NO_GUARD(pos)
#endif


extern void _init__OGCC (void);

#endif

