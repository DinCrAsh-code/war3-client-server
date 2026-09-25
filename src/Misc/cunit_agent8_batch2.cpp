//============================================================================
//  More small leaves from CUnit's own vtable BFS closure
//  (cunit_agent8_worklist.md), no owning class otherwise identified.
//============================================================================
#include "missile_thunderbolt.h"
#include "unitorder.h"
#include "widget.h"
#include "pathmove.h"     // CFloatSqrt

//  0x6F2AB3E0 - reconstructed for real as
//  SEventNotifyOrForwardHost::NotifyOrForwardEvent
//  (Misc/misc_notifyorforward_event.cpp, EXACT) - declared here as that
//  same struct/method, not a locally-invented one, so UpdateRef54 below
//  binds to the one real body (tools/link_check.py's first defect class).
struct SEventNotifyOrForwardHost
{
    void NotifyOrForwardEvent(void* target, int flag);   // 0x6F2AB3E0
};

//  0x6F2E37E0 - update `this`'s own +0x54 handle-ref slot to `newTarget`,
//  first registering/unregistering `this` as an observer of `newTarget`
//  through two independent mechanisms (SetObserverRegistration and its
//  sibling above). If the +0x54 slot was already resolved to something
//  other than `newTarget`, update a second slot at +0x60 instead of the
//  +0x54 one - the one piece of this shape that does not match
//  SetTargetRef/SetFieldA0Ref's own simpler "clear then set" pattern
//  (missile_thunderbolt.h), so this is its own function rather than a
//  third instance of either. `this`'s real class is not established;
//  only the three offsets this function itself touches are named.
//  0x6F2DEEF0 - Finalize().  Each of the four +0x28/+0x30/+0x38/+0x40
//  "publish slots" is an 8-byte pair - a sink object pointer and a cached
//  CFloat - established from the single-target path below, which reads
//  its *second* publish of each pair back out of the *first* slot's own
//  cache half (`&slotA.m_sink + 4` literally, in the shipped code) rather
//  than recomputing or re-reading a fresh local: slot pairs (A,C) and
//  (B,D) share one value each in that path.  The two-target path does not
//  touch the cache half at all - every value it publishes is a fresh
//  stack temporary - so the cache's only reader is the single-target path
//  publishing to its own sink's sibling.
struct SPublishSlot
{
    void*  m_sink;    // +0x00 - vtable-having, slot 0: (this=&m_sink, CFloat*, 1)
    CFloat m_cache;   // +0x04 - read directly by the single-target path only
};

struct SDualHandleRefHost6F2E37E0
{
    void UpdateRef54(void* newTarget);
    void Finalize();   // 0x6F2DEEF0

    char                          m_reserved00[0x28];
    SPublishSlot                  m_slotA;   // +0x28
    SPublishSlot                  m_slotB;   // +0x30
    SPublishSlot                  m_slotC;   // +0x38
    SPublishSlot                  m_slotD;   // +0x40
    SOptionalHandleRefResolver    m_ref48;   // +0x48
    char                          m_reserved50[0x54 - 0x50];
    SOptionalHandleRefResolver    m_ref54;   // +0x54
    char                          m_reserved5C[0x60 - 0x5C];
    SOptionalHandleRefResolver    m_ref60;   // +0x60
};

//  Slot 0 on whatever vtable-having object an SPublishSlot::m_sink points
//  at - read straight off the call site, the same shape CLAUDE.md's own
//  `ApplyOffsetFn` example documents.  Neither sink's concrete type is
//  established.
typedef void (__thiscall *PublishFn)(void*, CFloat*, int);

static void PublishSlotValue(void* slotSinkField, CFloat* value)
{
    ((PublishFn)(*(void***)slotSinkField)[0])(slotSinkField, value, 1);
}

extern const CFloat g_CFloatHalf;        // 0x6FAAE4D0
extern const CFloat g_slopeThreshold1;   // 0x6FAAE4F4 - encoded 1.0
extern const CFloat g_unk6FAAE594;       // 0x6FAAE594 - not otherwise identified

void SDualHandleRefHost6F2E37E0::Finalize()
{
    void* a = 0;
    if ((m_ref54.m_typeTag & (int)m_ref54.m_handle) != -1)
        a = QueryHandleField0x54(&m_ref54);

    void* b = 0;
    if ((m_ref48.m_typeTag & (int)m_ref48.m_handle) != -1)
        b = QueryHandleField0x54(&m_ref48);

    if (a != 0)
    {
        if (b != 0)
        {
            CWorldVec3 posA = ((CPathRef*)((CWidget*)a)->GetHandleObject())->SmartPtrToWorldVec3();
            CWorldVec3 posB = ((CPathRef*)((CWidget*)b)->GetHandleObject())->SmartPtrToWorldVec3();

            CWorldVec3 diff = posB - posA;

            CFloat midX = posA.m_x + diff.m_x * g_CFloatHalf;
            PublishSlotValue(&m_slotC.m_sink, &midX);

            CFloat midX2 = posA.m_x + diff.m_x * g_CFloatHalf;
            PublishSlotValue(&m_slotD.m_sink, &midX2);

            CFloat lenSq;
            diff.LengthSquared(&lenSq);
            CFloat len = CFloatSqrt(lenSq);
            CFloat invLen = g_slopeThreshold1 / len;

            CFloat valA = posA.m_z + diff.m_z * g_unk6FAAE594 * invLen;
            PublishSlotValue(&m_slotA.m_sink, &valA);

            CFloat valB = posB.m_x + diff.m_x * g_unk6FAAE594 * invLen;
            PublishSlotValue(&m_slotB.m_sink, &valB);

            return;
        }

        CWorldVec3 pos = ((CPathRef*)((CWidget*)a)->GetHandleObject())->SmartPtrToWorldVec3();
        m_slotA.m_cache = pos.m_x;
        PublishSlotValue(&m_slotA.m_sink, &m_slotA.m_cache);
        PublishSlotValue(&m_slotC.m_sink, &m_slotA.m_cache);
        m_slotB.m_cache = pos.m_y;
        PublishSlotValue(&m_slotB.m_sink, &m_slotB.m_cache);
        PublishSlotValue(&m_slotD.m_sink, &m_slotB.m_cache);
        return;
    }

    if (b != 0)
    {
        CWorldVec3 pos = ((CPathRef*)((CWidget*)b)->GetHandleObject())->SmartPtrToWorldVec3();
        m_slotA.m_cache = pos.m_x;
        PublishSlotValue(&m_slotA.m_sink, &m_slotA.m_cache);
        PublishSlotValue(&m_slotC.m_sink, &m_slotA.m_cache);
        m_slotB.m_cache = pos.m_y;
        PublishSlotValue(&m_slotB.m_sink, &m_slotB.m_cache);
        PublishSlotValue(&m_slotD.m_sink, &m_slotB.m_cache);
    }
}

void SDualHandleRefHost6F2E37E0::UpdateRef54(void* newTarget)
{
    ((SEventNotifyOrForwardHost*)newTarget)->NotifyOrForwardEvent(this, 1);
    ((CMissileThunderBoltRefs*)newTarget)->SetObserverRegistration(this, 1);

    void* resolved = 0;
    if ((m_ref54.m_typeTag & (int)m_ref54.m_handle) != -1)
        resolved = QueryHandleField0x54(&m_ref54);

    if (!resolved)
    {
        m_ref54.ResolveChained(newTarget);
        Finalize();
    }
    else if (resolved != newTarget)
    {
        m_ref60.ResolveChained(newTarget);
    }
}


//  0x6F3A15F0 - a flag/ability mask test with a "recheck via a slower
//  path" escape hatch, gated by the object's own +0x5C bit 0x1000000
//  (a "flags cached / recompute" bit several other reconstructions in this
//  tree already read - see agent 39's own family for the general shape).
//  Not a member function: `ecx` is never read, all three arguments arrive
//  on the stack (`retn 0Ch`).  The vtable call is through the receiver's
//  own slot 0x3B (+0xEC); the escape-hatch helper (0x6F27A380) is not
//  otherwise identified.
struct SFlagObj6F3A15F0
{
    int RecheckMaskViaSlowPath(int one);   // 0x6F27A380 - not this agent's address
};

typedef unsigned int (__thiscall *GetMaskIndexFn)(void*);

int __stdcall TestFlagMaskWithRecheck_6F3A15F0(SFlagObj6F3A15F0* obj,
                                               unsigned short mask,
                                               int forceCheck)
{
    if (forceCheck || (*(unsigned int*)((char*)obj + 0x5C) & 0x1000000))
    {
        unsigned int index = ((GetMaskIndexFn)(*(void***)obj)[0xEC / 4])(obj);
        if (!((1 << index) & mask))
        {
            if (*(unsigned int*)((char*)obj + 0x5C) & 0x1000000)
            {
                int result = obj->RecheckMaskViaSlowPath(1);
                if (!(result & mask))
                    return 0;
            }
        }
    }
    return 1;
}

//----------------------------------------------------------------------------
//  Link-completion thunks - each of these addresses was declared (not
//  defined) elsewhere in this file because comments record it as another
//  parallel agent's own worklist target, but the two agents independently
//  invented different class/struct names for the same shared address, so
//  the declaring extern reference never actually resolves against the
//  sibling's real (differently-named) reconstruction. Defining a plain
//  redirect thunk here - the same `mov eax,<addr>/jmp eax` idiom this
//  repo already uses for any out-of-scope callee - makes this TU link
//  without re-claiming the address (no funcmap.py row points at these).
//----------------------------------------------------------------------------
__declspec(naked) int SFlagObj6F3A15F0::RecheckMaskViaSlowPath(int one)
{
    __asm { mov eax, 06F27A380h }
    __asm { jmp eax }
}
