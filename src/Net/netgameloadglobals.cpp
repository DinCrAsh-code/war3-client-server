//============================================================================
//  The two vtable symbols NetGameLoadFile's constructor and destructor
//  stamp.  Local, for the reason src/Game/savegameglobals.cpp gives: this
//  build constructs nothing as C++ here (the base's three slots are all
//  `_purecall` in the image and none of the file's three is in any dump), so
//  MSVC lays out no table, and a funcmap.DATA row whose IDA name is not
//  address-shaped would leave the .mix with an unresolved external.
//
//  On the target write-up's arm-with-caution list.
//============================================================================
#include "netgameload.h"

void* g_vftableNetGameLoadFile[1] = { 0 };
void* g_vftableNetGameLoadSource[1] = { 0 };
