//============================================================================
//  0x6F40F440 - CPlayerWar3::DumpState, vtable slot 17 (+0x44).  Same shape
//  as CAgentWar3::DumpState (agentwar3_slots.cpp) - a flat run of
//  ((Type*)&field)->DumpTo(sink) calls in exactly the dump's own call
//  order (not address order - +0x26C is dumped twice, and +0x2CC/+0x2E0/
//  +0x2D4 are out of order relative to each other) - plus one real loop
//  over the +0x2C0/+0x2C4 int array.  All three DumpTo helpers
//  (SIntMiniValue, FloatMini, SRefMiniValue) already exist and are EXACT;
//  this slot only calls them.
//============================================================================
#include "playerwar3.h"

void CPlayerWar3::DumpState(void* sink)
{
    //  +0x20: m_flags, inherited from CAgentWar3 (agentwar3.h) - the same
    //  "cast the plain dword through SIntMiniValue" spelling
    //  CAgentWar3::DumpState itself uses on the same field.
    ((SIntMiniValue*)&m_flags)->DumpTo(sink);

    for (int i = 0; i < 0xC; ++i)
    {
        m_dumpArrayA[i].DumpTo(sink);
        m_dumpArrayB[i].DumpTo(sink);
    }

    m_reserved248.DumpTo(sink);
    m_reserved24C.DumpTo(sink);
    m_reserved250.DumpTo(sink);
    m_reserved254.DumpTo(sink);
    m_reserved258.DumpTo(sink);
    m_reserved25C.DumpTo(sink);
    m_reserved260.DumpTo(sink);
    m_reserved264.DumpTo(sink);
    m_reserved268.DumpTo(sink);
    m_reserved26C.DumpTo(sink);
    m_reserved270.DumpTo(sink);
    m_reserved274.DumpTo(sink);
    m_reserved278.DumpTo(sink);

    m_reserved298.DumpTo(sink);
    m_reserved2A0.DumpTo(sink);
    m_reserved2A8.DumpTo(sink);

    for (unsigned int i = 0; i < m_intMiniCount; ++i)
        m_intMiniArray[i].DumpTo(sink);

    m_reserved2CC.DumpTo(sink);
    m_reserved2E0.DumpTo(sink);
    m_reserved2D4.DumpTo(sink);

    //  Reads the same field as the call above, ten lines up - the shipped
    //  body leaves `ebp` pointing at +0x26C and never reloads it, so it
    //  gets dumped twice.  See playerwar3.h's own note on m_reserved26C.
    m_reserved26C.DumpTo(sink);

    m_reserved32C.DumpTo(sink);
    m_reserved330.DumpTo(sink);
    m_reserved334.DumpTo(sink);
}
