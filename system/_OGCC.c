#include "_OGCC.h"

#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>


static ModuleId module_count = 0; /* number of linked modules, incremented by add_module */
Module module_list = NULL;	/* list of module descriptors */
Type td_module;			/* type descriptor for type 'ModuleDesc' */
Type td_type;			/* type descriptor for type 'TypeDesc' */
void* tmpNew;

ModuleId add_module (const char name[]) {
  Module mod, mptr;

  NEWREC (mod, td_module, 0);
  /* initialize the module descriptor */
  mod->next = NULL;
  mod->key = module_count;
  mod->tdlist = NULL;
  strcpy (mod->name, name);
  module_count++;		/* increment module count */
  /* add module descriptor to end of module list */
  if (module_list) {
    mptr = module_list;
    while (mptr->next) {
      mptr = mptr->next;
    }
    mptr->next = mod;
  } else {
    module_list = mod;
  }
  return mod->key;		/* return the module id */
}

Module get_module (ModuleId id) {
  Module mod = module_list;
  while (mod->key != id) {
    mod = mod->next;
  }
  return mod;
}

void* create_td (const char name[], int size_obj, int size_td, void* base_td, int size_basetd) {
  Type td;
  INTEGER level;
  Module module;

  /* get current module */
  module = module_list;
  while (module->next) {
    module = module->next;
  }
			 
  /* determine level of td */
  if (base_td) {
    level = ((Type) base_td)->level+1;
  } else {
    level = 0;
  }
  
  td = (void*) ((void**)malloc(size_td+(level+2)*sizeof(void*)) + 2 + level);

  /* initialize type descriptor */
  if (base_td) {
    memcpy (&BASETYPE(td, level-1), &BASETYPE(base_td, level-1), (level+1)*sizeof(void*)+size_basetd);
  }
  strcpy (td->name, name);
  td->module = module;
  td->level = level;
  td->size = size_obj;
  SETTAG (td, td_type);
  BASETYPE(td, level) = td;

  /* add td to list of type descriptors of the current module */
  td->next = module->tdlist;
  module->tdlist = td;

  return td;
}


void _init__OGCC (void) {
#ifdef GC
/* if you have installed gc as a SunOS 5 dynamic lib, include gc.h 
   and uncomment GC_INIT()  */
/*  GC_INIT(); */  
#ifdef NEED8ALIGN
  GC_register_displacement(8);
  GC_register_displacement(16);
#else
  GC_register_displacement(4);
  GC_register_displacement(8);
  GC_register_displacement(12);
  GC_register_displacement(16);
#endif
#endif
}





/* arithemtic shift */
LONGINT ash (LONGINT value, LONGINT dir) {
  if (dir > 0) {
    return value << dir;
  } else if (value < 0) {
    while (dir < 0) { 
      value = DIVLI (value, 2, 0); dir++; 
    }
    return value;
  } else {
    return value >> (-dir);
  }
}




static void generate_trap_header (ModuleId id, LONGINT pos) {
  fprintf (stderr, "\nError in %s.Mod at pos " LI_FORMAT ":\n", get_module(id)->name, pos);
}

static void generate_trap (ModuleId id, LONGINT pos, const char message[]) __attribute__ ((noreturn));

static void generate_trap (ModuleId id, LONGINT pos, const char message[]) {
  generate_trap_header (id, pos);
  fprintf (stderr, "%s.\n", message);
  exit(1);
}

extern void trap_assert (LONGINT code, ModuleId id, LONGINT pos) {
  generate_trap_header (id, pos);
  fprintf (stderr, "Assertion failed, code " LI_FORMAT ".\n", code);
  exit(code);
}

void trap_no_return (ModuleId id, LONGINT pos) {
  generate_trap (id, pos, "Control reaches end of function without return statement");
}

void trap_no_label (LONGINT expr, ModuleId id, LONGINT pos) {
  generate_trap_header (id, pos);
  fprintf (stderr, "Invalid CASE index.\nValue '" LI_FORMAT "' doesn't match any label.\n", expr);
  exit(1);
}

void *test_nil (void *ptr, ModuleId id, LONGINT pos) {
  if (ptr == NULL)
    generate_trap (id, pos, "Dereference of NIL pointer");
  return ptr;
}

LONGINT check_invalid_index (LONGINT index, LONGINT length, ModuleId id, LONGINT pos) {
  if ((index < 0) || (index >= length)) {
    generate_trap_header (id, pos);
    fprintf (stderr, "Invalid array index.\n" LI_FORMAT " is not in 0 <= x < " LI_FORMAT ".\n", index, length);
    exit(1);
  }
  return index;
}

SHORTINT check_set_index (LONGINT index, ModuleId id, LONGINT pos) {
  if ((index < 0) || (index > MAX_SET)) {
    generate_trap_header (id, pos);
    fprintf (stderr, "Invalid set index.\n" LI_FORMAT " is not in 0..%i.\n", index, MAX_SET);
    exit(1);
  }
  return (SHORTINT) index;
}

void check_type (void* tag0, void* tag1, ModuleId id, LONGINT pos) {
  if (tag0 != tag1) {
    generate_trap_header (id, pos);
    fprintf (stderr, "Dynamic type differs from static type.\n");
    exit(1);
  }
}

BOOLEAN type_test (void* var_tag, void* type_tag, INTEGER type_level) {
  return 
    (((Type)var_tag)->level >= type_level) &&
    (BASETYPE(var_tag, type_level) == type_tag);
}

BOOLEAN type_test_ptr (void* variable, void* type_tag, INTEGER type_level, ModuleId id, LONGINT pos) {
  void *var_tag;
  
  if (variable) {
    var_tag = TAG_NC(void, variable);
    return
      (((Type)var_tag)->level >= type_level) &&
	(BASETYPE(var_tag, type_level) == type_tag);
  } else {
    generate_trap_header (id, pos);
    fprintf (stderr, "Type test applied to NIL.\n");
    exit(1);
  }
}

void* type_guard (void* value, void* var_tag, void* type_tag, INTEGER type_level, ModuleId id, LONGINT pos) {
  if (!type_test (var_tag, type_tag, type_level)) {
    CHAR *tname = ((Type)var_tag)->name;
    CHAR *mname = ((Type)var_tag)->module->name;
    if (!tname) tname = (CHAR*)"<unknown>";

    generate_trap_header (id, pos);
    fprintf (stderr, "Type guard failed.\n");
    fprintf (stderr, "The variable's dynamic type is %s.%s\n", mname, tname);
    exit(1);
  }
  return value;
}
  
void* type_guard_ptr (void* value, void* type_tag, INTEGER type_level, ModuleId id, LONGINT pos) {
  if (value) {
    if (!type_test (TAG_NC(void, value), type_tag, type_level)) {
      CHAR *tname = TAG_NC(TypeDesc, value)->name;
      CHAR *mname = TAG_NC(TypeDesc, value)->module->name;
      if (!tname) tname = (CHAR*)"<unknown>";

      generate_trap_header (id, pos);
      fprintf (stderr, "Type guard failed.\n");
      fprintf (stderr, "The variable's dynamic type is %s.%s\n", mname, tname);
      exit(1);
    } else {
      return value;
    }
  } else {
    generate_trap_header (id, pos);
    fprintf (stderr, "Type guard applied to NIL.\n");
    exit(1);
  }
}
extern void trap_no_guard (ModuleId id, LONGINT pos) {
  generate_trap_header (id, pos);
  fprintf (stderr, "All guards failed in WITH statement.\n");
  exit(1);
}
