//============================================================================
//  CAgentRelation::Teardown (0x6F4A6920) and OnRegistrationRetired
//  (0x6F4A69B0, CAgentBaseAbs's own vtable slot 18 / +0x48) - see
//  agentrelation.h's own comments for what each does and why the second
//  reads as an inherited CPrRelation virtual rather than a CAgentBaseAbs
//  override.
//
//  OnRegistrationRetired's self-redirect through vtable slot 4 is a
//  hand-written indirect dispatch (CLAUDE.md's own vtable_dispatch_audit.py
//  section) rather than a real C++ virtual call: this repo has never
//  declared a real `class CAgentBaseAbs : ...` with virtuals (see
//  agentbaseabscomputechecksum.cpp's own header comment for why - only 5 of
//  19 slots are reconstructed), so the call has to go through the object's
//  own vtable pointer by hand, exactly as CLAUDE.md's own worked example
//  does.  Slot 4 (`+0x10`) is CAgentBaseAbs's own confirmed
//  `docs/targets/vtables/CAgentBaseAbs.txt` entry, `sub_6F4A5A70`.
//
//  Own translation unit: DetachEndpointA/B, PostToSelf, PostToEndpointA/B,
//  ReleaseRegistration and SHandleTableEntry::UnregisterAndNotify are all
//  real calls (agentrelation.h / handletable.h) - SHandleTableEntry is
//  declared locally rather than pulled in via `#include "handletable.h"`,
//  the same way agentbaseabseventbinding.cpp does, to avoid that header's
//  own `SHandleTable` colliding with game.h's independently-modelled one.
//============================================================================
#include "agentrelation.h"

struct SHandleTableEntry
{
    void UnregisterAndNotify(int arg);
};

const unsigned int kRelTornDown       = 0x01000000;
const unsigned int kRelRetiring       = 0x00800000;
const unsigned int kRelNotifySelf     = 0x00000400;
const unsigned int kRelNotifyA        = 0x00000100;
const unsigned int kRelNotifyB        = 0x00000200;
const unsigned int kRegRetired        = 0x00010000;

//  Slot 4 (+0x10) on this object's own vtable - CAgentBaseAbs::sub_6F4A5A70,
//  confirmed by docs/targets/vtables/CAgentBaseAbs.txt.  `retn 4` there
//  matches this one stack argument.
typedef void (__thiscall *CAgentBaseAbsSlot4Fn)(void*, int);

//----------------------------------------------------------------------------
//  0x6F4A6920 - detach both endpoints, retire both registrations and
//  unregister this relation's handle-table entry.
//----------------------------------------------------------------------------
void CAgentRelation::Teardown(int arg)
{
    m_flags |= kRelTornDown;

    DetachEndpointB();
    DetachEndpointA();

    if (m_registration != 0)
    {
        m_registration->m_flags |= kRegRetired;
        m_registration = 0;
    }
    if (m_channelReg != 0)
    {
        m_channelReg->m_flags |= kRegRetired;
        m_channelReg = 0;
    }

    ((SHandleTableEntry*)this)->UnregisterAndNotify(arg);
}

//----------------------------------------------------------------------------
//  0x6F4A69B0 - CAgentBaseAbs's own vtable slot 18 (+0x48).  See
//  agentrelation.h's own comment for the full shape.
//----------------------------------------------------------------------------
void CAgentRelation::OnRegistrationRetired(void* registration)
{
    if (registration == m_channelReg)
    {
        CAgentBaseAbsSlot4Fn slot4 = (CAgentBaseAbsSlot4Fn)(*(void***)this)[4];
        slot4(this, 0);
        return;
    }

    m_flags |= kRelRetiring;

    //  `detail` is the retiring registration's own m_time, reused as the
    //  event's `void*` detail argument - the shipped code passes the same
    //  raw dword through, not a real pointer.
    void* detail = (void*)(unsigned int)((SSyncRegistration*)registration)->m_time;

    if (m_flags & kRelNotifySelf)
        PostToSelf(0x5E736967, detail);

    if (m_flags & kRelNotifyA)
        PostToEndpointA(0x5E736967, detail, 0);

    if (m_flags & kRelNotifyB)
        PostToEndpointB(0x5E736967, detail, 0);

    if (registration == m_registration && !(m_registration->m_flags & 1))
        ReleaseRegistration();

    m_flags &= ~kRelRetiring;
}
