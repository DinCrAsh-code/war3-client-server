//============================================================================
//  0x6F256180 - CCustomData::~CCustomData().  Stamps this class's own
//  vtable (the compiler's own doing, entering any destructor of a
//  polymorphic class) and tail-calls ReleaseOwnedRefAndFinalize()
//  (abilitycustomdata_release.cpp).
//
//  Own translation unit, and that is the whole point of the file: both
//  DeleteSelf overrides (abilitycustomdata_deleteself.cpp) must see only
//  this destructor's declaration, never its definition, or /Ob2 inlines it
//  and the shipped body's real `call sub_6F256180` (with the `this != 0`
//  test that survives *because* it is a real call - CLAUDE.md's "A
//  deleting destructor's `this != 0` test tracks whether the base
//  destructor inlined") turns into a bigger, inlined stream instead.  The
//  same reasoning keeps ReleaseOwnedRefAndFinalize() only declared here.
//============================================================================
#include "abilitycustomdata.h"

CCustomData::~CCustomData()
{
    ReleaseOwnedRefAndFinalize();
}
