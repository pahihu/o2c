#include "ConvTypes.h"

static ModuleId moduleId;



const SHORTINT ConvTypes_allRight = 0;
const SHORTINT ConvTypes_outOfRange = 1;
const SHORTINT ConvTypes_wrongFormat = 2;
const SHORTINT ConvTypes_noData = 3;
const SHORTINT ConvTypes_noRoom = 4;
const SHORTINT ConvTypes_left = 0;
const SHORTINT ConvTypes_centre = 1;
const SHORTINT ConvTypes_right = 2;



void _init_ConvTypes (void) {
  moduleId = add_module ("ConvTypes");
}
