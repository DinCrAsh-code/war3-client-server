//============================================================================
//  0x6F0024E0 - CMiscCustom::~CMiscCustom(), the real, non-virtual
//  destructor. Re-stamps the real vtable address (see CMiscCustom.h's own
//  note on why this class cannot get that for free from `virtual`) then
//  tail-calls TeardownSources() (miscustom_teardownsources.cpp) - the
//  dump's own `jmp sub_6F002130` rather than a `call`, which is why there
//  is nothing after it.
//
//  Own translation unit, and that is the whole point of the file: both
//  DeleteSelf overrides (miscustom_deleteself.cpp, CSkinCustom.cpp) must
//  see only this destructor's *declaration* (CMiscCustom.h), never its
//  definition, or this compiler inlines it and the shipped body's real
//  `call sub_6F0024E0` (with the `this != 0` test that survives *because*
//  it is a real call - the same "a deleting destructor's `this != 0` test
//  tracks whether the base destructor inlined" shape
//  Item/abilitycustomdata_dtor.cpp already documents) turns into a bigger,
//  inlined stream instead. Same reasoning keeps TeardownSources() only
//  declared here, not defined.
//============================================================================
#include "CMiscCustom.h"

CMiscCustom::~CMiscCustom()
{
    m_vtable = (void*)0x6F8750A4;
    TeardownSources();
}
