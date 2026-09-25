//============================================================================
//  0x6F26E340 - CUnit's vtable slot 60 (+0xF0), QueryVisible.  See unit.h.
//
//  One instruction: a tail jump into 0x6F26DE50, the probe slots 43 and 44
//  (unit_thunks.cpp) call *directly* rather than dispatching back through
//  this slot to reach it.
//
//  Own translation unit for the usual reason - same-TU the callee would be
//  inlined and there would be no jump left to match.
//============================================================================
#include "unit.h"

int CUnit::QueryVisible(int which)
{
    return QueryVisibleImpl(which);
}
