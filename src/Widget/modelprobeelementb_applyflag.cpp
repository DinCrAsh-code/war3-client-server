//============================================================================
//  0x6F4F1EC0 - SModelProbeElementB::ApplyFlag_6F4F1EC0.  See
//  modelprobeelement.h.  Own translation unit so that
//  SModelInstance_PropagateFlagRecursive_6F507CE0's own call to it
//  (modelinstancenotify.cpp) survives rather than getting inlined away.
//============================================================================
#include "modelprobeelement.h"

void __thiscall SModelProbeElementB::ApplyFlag_6F4F1EC0(int flag)
{
    m_flags194.m_isFlagged = flag;
}
