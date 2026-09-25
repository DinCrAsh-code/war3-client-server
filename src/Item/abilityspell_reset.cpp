//============================================================================
//  CAbilitySpell::ResetField0xAC / CAbilitySpell::Method_ResolveAndCommit -
//  see abilityspell.h.  Two more of the "generic base-class hooks" batch
//  (see ability.h's own note on the four trivial constant-return ones) -
//  unlike those, these two touch CAbilitySpell's own already-established
//  fields (m_cooldown/m_cooldownMax/m_field0xAC/m_field0xA0), so they are
//  declared here rather than on CAbility itself.
//
//  Own translation unit: real calls out of both (SOptionalHandleRefResolver
//  ::ResolveChained, two unnamed vtable slots, FloatMini's own slot-0
//  notify), matching the one-TU-per-module rule.
//============================================================================
#include "abilityspell.h"
#include "itemhandleresolve.h"

//  0x6F0377F0 / 0x6F069020
void CAbilitySpell::SetField0xAC(unsigned int value) { m_field0xAC = value; }
unsigned int CAbilitySpell::GetField0xAC() const { return m_field0xAC; }

//  0x6F050390
//----------------------------------------------------------------------------
//  this+0x20 is CAgentWar3::m_flags (agentwar3.h), inherited down the same
//  way CAbility::CAbility's own m_flags write already is (ability.cpp) -
//  bit 0x20000000 set, and m_field0xAC reset to its "cleared" sentinel
//  (0xFFFFFFFF), distinct from the constructor's own initial 0
//  (abilityspell.h/.cpp).
void CAbilitySpell::ResetField0xAC()
{
    m_flags |= 0x20000000;
    m_field0xAC = 0xFFFFFFFF;
}

//  A real member of SOptionalHandleRef (itemhandleresolve.h), redeclared
//  here rather than exposed from handlereref_resolve.cpp - same struct
//  name, same base, same method signature, so it links to the identical
//  mangled symbol (sub_6F0419C0) that TU already defines. Reached at
//  this+0xA0 by raw offset rather than by retyping CAbilitySpell's own
//  m_field0xA0/m_field0xA4 pair (abilityspell.h) into an SOptionalHandleRef,
//  since nothing else in this call tree needs that field renamed and the
//  cast alone is enough to reproduce this call site.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//  FloatMini's own vtable slot 0, reinterpreted as a two-argument notify
//  rather than the destructor this reconstruction declares there - same
//  idiom, same reasoning, as FloatMiniB::SetPair's own NotifyFn
//  (floatmini.h).
typedef void (__thiscall *NotifyFn)(void* self, const CFloat* value, int flag);

//  Slots 0x358 and 0x35C (byte offsets - 0xD6/0xD7) on `this` itself: two
//  boolean-shaped queries with no name of their own yet. Not attributable
//  to a specific class in the chain (CPower/CAbilityInterfaced are not
//  reconstructed - see ability.h's own note), so reached the same raw-
//  offset way agent_slot21.cpp/widget_slot65.cpp reach an unnamed slot on
//  a known object.
typedef int (__thiscall *Slot0x358Fn)(void* self);
typedef int (__thiscall *Slot0x35CFn)(void* self);

//  0x6F050320
//----------------------------------------------------------------------------
//  Sets a flag, forwards `ref` into the +0xA0 handle-ref pair, then - for
//  each of m_cooldown and m_cooldownMax in turn - asks a query slot on
//  `this` and, only if it answers false, resets that FloatMini to zero via
//  its own vtable slot 0. Reads like "commit a cast": mark in-progress
//  (0x2000), bind whatever target/ability handle the caller resolved, and
//  only clamp cooldown/cooldownMax back to zero for whichever of the two
//  the query slot says has not already been set for real - the most
//  damage/cooldown-plausible of this batch, though the two query slots'
//  own real meaning ("is this value already authoritative") is not
//  confirmed beyond that shape.
//
//  `ref` is not a bare SOptionalHandleRef* - see sub_6F0419C0's own
//  header comment (handlereref_resolve.cpp) - so it stays a raw
//  const void*, forwarded unexamined.
void CAbilitySpell::Method_ResolveAndCommit(const void* ref)
{
    m_flags |= 0x2000;

    ((SOptionalHandleRefResolver*)((char*)this + 0xA0))->ResolveChained(ref);

    if (!((Slot0x358Fn)(*(void***)this)[0x358 / 4])(this))
        ((NotifyFn)(*(void***)&m_cooldown)[0])(&m_cooldown, &g_CFloatZero, 1);

    if (!((Slot0x35CFn)(*(void***)this)[0x35C / 4])(this))
        ((NotifyFn)(*(void***)&m_cooldownMax)[0])(&m_cooldownMax, &g_CFloatZero, 1);
}
