//============================================================================
//  0x6F02E3A0 - CAbility's vtable slot 17 (+0x44), DumpState.  See
//  ability.h.
//
//  Dumps ten of CAbility's own words: m_flags (CAgentWar3's own +0x20,
//  the first byte past CAgent's embedded CObserver) as a single
//  "[IntMini]" value, the +0x24/+0x28 SOptionalHandleRef pair
//  (CUnitRefList's own m_field24/m_field28) as one "[RefMini]" value, and
//  eight of CAbility's own ten reserved dwords (m_field2C and m_field30
//  are not dumped) each as their own "[IntMini]" - the same
//  widget_dump.cpp idiom, just with a mixed run of scalar and paired
//  fields instead of two scalars.
//============================================================================
#include "ability.h"
#include "textsink.h"

void CAbility::DumpState(void* sink)
{
    ((SIntMiniValue*)&m_flags)->DumpTo(sink);
    ((SRefMiniValue*)&m_field24)->DumpTo(sink);
    ((SIntMiniValue*)&m_field3C)->DumpTo(sink);
    ((SIntMiniValue*)&m_field40)->DumpTo(sink);
    ((SIntMiniValue*)&m_field44)->DumpTo(sink);
    ((SIntMiniValue*)&m_field48)->DumpTo(sink);
    ((SIntMiniValue*)&m_field4C)->DumpTo(sink);
    ((SIntMiniValue*)&m_typeId)->DumpTo(sink);
    ((SIntMiniValue*)&m_field50)->DumpTo(sink);
    ((SIntMiniValue*)&m_field38)->DumpTo(sink);
}
