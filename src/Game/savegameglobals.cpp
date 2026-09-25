//============================================================================
//  The vtable symbol CSaveGame's constructor stamps.  Local, for the reason
//  src/Misc/cstringrep.h gives at length: this build constructs nothing as
//  C++ here, so MSVC lays out no table, and a funcmap.DATA row whose IDA
//  name is not address-shaped would leave the .mix with an unresolved
//  external.  On the target write-up's arm-with-caution list.
//============================================================================
#include "savegame.h"

void* g_vftableCSaveGame[1] = { 0 };
