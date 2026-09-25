//============================================================================
//  0x6F476470 - SmartPositionTrack's vtable slot 6, SetMaxSpeed override.
//
//  Identical clamp-and-convert PositionTrack::SetMaxSpeed itself does
//  (0x6F4747B0, positiontrackspeed.cpp): resolve the handle, pick whichever
//  of `speed` and the global cap (g_pGameData->m_globalMaxSpeed) is not the
//  larger one (`!(cap < *speed)`, same ordered-compare mask this compiler
//  picks for that spelling - see positiontrackspeed.cpp's own comment and
//  docs/msvc-vc8-idioms.md), convert to grid units (WorldToGrid) and
//  install it via CPathTrace::SetMaxSpeed (0x6F4A73F0,
//  Pathfinding/pathtracesetmaxspeed.cpp) - then, SmartPosition's own
//  addition, hand the identical grid-unit value to a second call,
//  CPathTrace::SetFieldB4 (0x6F4937B0, Pathfinding/pathtracesetfieldb4.cpp),
//  a new +0xB4 field on CPathTrace with no reader anywhere in this call
//  tree.  The resolved CPathTrace* is kept in one register (edi) and reused
//  as `this` for both calls rather than re-resolved.
//
//  DIFFERS, not EXACT: the same shape as positiontrackspeed.cpp's own
//  writeup - every instruction and operand matches, but this build spends
//  one extra callee-saved register (edi) holding the resolved trace across
//  both calls where the shipped code does the same thing with esi/ecx
//  reuse patterns of its own; a compiler register-allocation choice, not a
//  behavioural gap - pending angr IDENTICAL verification.
//
//  Own translation unit: 0x6F476470 is a different module from
//  0x6F4A7xxx/0x6F493xxx (its own two callees) and from 0x6F474xxx
//  (positiontrackspeed.cpp/positiontrackfield8c.cpp, PositionTrack's own
//  slot 6/7).
//============================================================================
#include "positiontrack.h"

void SmartPositionTrack::SetMaxSpeed(const CFloat* speed)
{
    CPathTrace* trace = (CPathTrace*)LookupHandle(m_handle, m_typeTag);

    const CFloat* chosen =
        !(*(const float*)&g_pGameData->m_globalMaxSpeed.m_bits <
          *(const float*)&speed->m_bits)
            ? speed
            : &g_pGameData->m_globalMaxSpeed;

    CFloat gridSpeed = WorldToGrid(*chosen);
    trace->SetMaxSpeed(&gridSpeed);
    trace->SetFieldB4(&gridSpeed);
}
