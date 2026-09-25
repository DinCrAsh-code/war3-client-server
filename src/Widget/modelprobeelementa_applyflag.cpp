//============================================================================
//  0x6F4F4A80 - SModelProbeElementA::ApplyFlag_6F4F4A80.  See
//  modelprobeelement.h.  Own translation unit so that
//  SModelInstance_PropagateFlagRecursive_6F507CE0's own call to it
//  (modelinstancenotify.cpp) survives rather than getting inlined away.
//============================================================================
#include "modelprobeelement.h"

void __thiscall SModelProbeElementA::ApplyFlag_6F4F4A80(int flag)
{
    m_flag0C = flag;
}
