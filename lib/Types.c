#include "Types.h"
/* low level interface to the type descriptor mechanism. */

static ModuleId moduleId;

Types_Module Types_modules;


Types_Type Types_BaseOf (Types_Type t, INTEGER level) {
  return BASETYPE (t, level);
}

INTEGER Types_LevelOf (Types_Type t) {
  return t->level;
}

void Types_NewObj (PTR (* o), Types_Type t) {
  NEWREC_SIZE(*o, t, t->size, 0);
}

Types_Type Types_This (Types_Module mod, const CHAR *name) {
  Types_Type td = mod->tdlist;  
  while (td) {
    if (!strcmp (name, td->name)) {
      return td;
    }
    td = td->next;
  };
  return NULL;
}

Types_Type Types_TypeOf (PTR o) {
  return TAG_NC(Types_TypeDesc, o);
}

void _init_Types (void) {
  moduleId = add_module ("Types");
  Types_modules = (Types_Module)module_list;
}


