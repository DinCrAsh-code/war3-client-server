//============================================================================
//  CUnitRefList: the two-slot layer between CAgentWar3 and CAbility in the
//  real RTTI chain (`.data:6FA6DBE0`'s own inheritance comment, supplied
//  directly against this build's own database - CAbilitySpellBook :
//  CAbilitySpell : CAbilityButton : CPower : CAbilityInterfaced : CAbility
//  : CUnitRefList : CAgentWar3 : CAgent : CObserver : TRefCnt).
//
//  A correction to this session's earlier CAbility/CAbilityInventory work,
//  which had CAbility deriving from CAgent directly - byte-identical in
//  every already-verified score (CAgentWar3 and CUnitRefList's own
//  constructors, both trivial besides a field write and a redundant
//  vtable stamp, fully inline into whatever derived class actually gets
//  constructed - the same "redundant vtable store eliminated" shape
//  floatmini.h's own FloatMini/FloatMiniB note documents), but wrong
//  about which class owns which offset and which vtable slot index a new
//  virtual lands at.
//
//  Fields, from CAbility::CAbility's own writes (ability.cpp) now
//  attributed correctly: +0x24/+0x28 sit here, not on CAbility - both
//  sentinel -1 ("not yet bound"), the same convention CAgent's own
//  m_handle/m_typeTag pair uses.
//
//  Vtable: adds exactly two new slots (30, 31 - CAgentWar3's own 30 slots
//  are 0-29) on top of CAgentWar3's own 30, matching this build's own
//  vtable dump for CUnitRefList directly (`??_7CUnitRefList@@6B@`,
//  0x6F87798C - 32 slots total).
//    slot 30 (0x6F052170) - point this agent's tracked-target link at `a`
//      (CAgent::SetTrackedTarget), then resolve `b` (a handle) through
//      this object's own +0x24/+0x28 field pair - the same
//      SOptionalHandleRef shape agenttimer.h's own m_target uses,
//      reached here via SOptionalHandleRefResolver::ResolveChained
//      (handlereref_resolve.cpp) - and return a pointer to that pair.
//      See unitreflist_vtable.cpp.
//    slot 31 (0x6F0383A0) - resolve this agent's own owner
//      (SAttachmentOwnerQuery::QueryOwner, attachmentownerquery.cpp,
//      reading the +0x0C/+0x10 handle/typeTag pair CAgent itself owns)
//      and tail-jump into *its* vtable slot 59 (+0xEC) with no arguments
//      of its own - the target class is not committed anywhere in this
//      build, so the dispatch is a raw offset rather than a named call.
//      See unitreflist_vtable.cpp.
//============================================================================
#ifndef UNITREFLIST_H
#define UNITREFLIST_H

#include "agentwar3.h"

class CUnitRefList : public CAgentWar3
{
public:
    CUnitRefList()
        : m_field24(-1), m_field28(-1)
    {
    }

    //  See the class comment above and unitreflist_vtable.cpp.
    virtual void* Method_0x78(int a, int b);
    virtual void Method_0x7C();

    int m_field24;  // +0x24
    int m_field28;  // +0x28
};

#endif
