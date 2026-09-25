//============================================================================
//  0x6F4A6070 - reset this relation's event binding: (re-)register its
//  handle-table entry (SHandleTableEntry::RegisterOrReuse, handletable.h),
//  then reset m_field44/m_eventId/m_flags to construction defaults - the
//  exact same three values CRlAgent's own inlined base constructor stamps
//  (crlagent.h's m_baseDefaultFloat44/m_baseSentinel48/m_baseReserved4C -
//  `dword_6FAAE470`/-1/0).
//
//  `this` is passed straight through as RegisterOrReuse's own receiver:
//  this call tree never establishes CAgentRelation as inheriting from
//  SHandleTableEntry, only that the two share the same address here - the
//  same "reinterpret an existing object as a raw interface" idiom
//  agentbaseabscomputechecksum.cpp's own CAgentBaseAbsRaw already uses.
//
//  Own translation unit: SHandleTableEntry::RegisterOrReuse is a real call
//  (handletable.h) - declared locally rather than `#include`d: handletable.h
//  also defines its own `SHandleTable` (g_pHandleTable's own generic-table
//  shape), which collides with game.h's independently-modelled
//  `SHandleTable` (the handle-lookup table's own slots/altSlots shape) the
//  moment both headers land in one translation unit - a pre-existing
//  conflict between two independently-reconstructed headers, not something
//  this file needs any part of beyond SHandleTableEntry's own two methods.
//============================================================================
#include "agentrelation.h"
#include "CFloat.h"

struct SHandleTableEntry
{
    void RegisterOrReuse(SHandleRegistrationRequest* request);
    void UnregisterAndNotify(int arg);
};

void CAgentRelation::ResetEventBinding(SHandleRegistrationRequest* request)
{
    ((SHandleTableEntry*)this)->RegisterOrReuse(request);

    *(unsigned int*)&m_field44 = *(const unsigned int*)&g_CFloatZero;
    m_eventId = 0xFFFFFFFF;
    m_flags   = 0;
}
