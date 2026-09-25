//============================================================================
//  0x6F26FA40 - resets four module-scope globals to their "nothing pending"
//  values.  Reached from CUnit::EnsureSelectionCircle's closure (slot 101,
//  0x6F28DCF0); zero arguments, no calls, no `this` - a plain reset routine.
//
//  g_unitOrderNestDepth/g_unitOrderNestFlag are zeroed and
//  g_pendingSelectionHandleA/g_pendingSelectionHandleB are set to the
//  all-ones "no handle" sentinel this dump's neighbourhood
//  (sub_6F27A780, still a `TODO` in this session) uses the same shape for -
//  a nested-call depth counter plus flag reset back to "not inside a
//  dispatch", and two handle-pair slots reset to empty.  Names are this
//  session's best reading of the shape, not a confirmed cross-reference;
//  nothing else in the tree reads these four yet, so there was nothing
//  further to confirm against.
//============================================================================
#include "game.h"

int  g_unitOrderNestDepth;
int  g_unitOrderNestFlag;
unsigned int g_pendingSelectionHandleA;
unsigned int g_pendingSelectionHandleB;

void ResetUnitOrderDispatchState()
{
    g_unitOrderNestDepth = 0;
    g_unitOrderNestFlag = 0;
    g_pendingSelectionHandleA = 0xFFFFFFFF;
    g_pendingSelectionHandleB = 0xFFFFFFFF;
}
