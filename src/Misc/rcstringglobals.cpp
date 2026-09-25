//============================================================================
//  The two vtable symbols RCString's destructor stamps.  Local definitions
//  for the reason src/Misc/cstringrep.h gives at length.
//============================================================================
#include "rcstring.h"

void* g_vftableRCString[1] = { 0 };
void* g_vftableTRefCnt[1] = { 0 };
