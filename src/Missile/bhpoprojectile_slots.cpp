//============================================================================
//  0x6F4AAC80/0x6F4AB050/0x6F4AB280/0x6F4AB1A0/0x6F4AB5D0 - one shipped
//  module: CBhPoProjectile's own growable array of tick-relay slots
//  (SBhPoTickRelaySlotArray, bhpoprojectile.h) and the three operations
//  built on top of it - release a slot, re-sample this projectile's own
//  sync channel, and arm a slot.
//
//  0x6F4AB0D0/0x6F4AAC40 (RemoveRange/ShrinkToFit) are NOT reached from
//  anything in this dump - they were found tracing
//  `NIpse::CPoPosBh::TeardownRelatedAgents` (Agent/poposbh_relatedagents.cpp),
//  which embeds this same struct at its own +0x9C.  Housed here rather
//  than in Agent/ because they are this struct's own methods, the same
//  reason FillNewSlots above is here and not in whichever class happens to
//  call it.
//============================================================================
#include "bhpoprojectile.h"
#include "agenttickrelay.h"
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F4AAC80 - write the one pointer *fillValue into every slot
//  [from, from+count), clamped to m_count.  `this` in ecx, `retn 0Ch` -
//  three stack arguments.
//----------------------------------------------------------------------------
void SBhPoTickRelaySlotArray::FillNewSlots(unsigned int from,
                                            void* const* fillValue,
                                            unsigned int count)
{
    unsigned int end = from + count;
    if (end >= m_count)
        end = m_count;

    if (from < end)
    {
        //  Re-reads `*fillValue` every iteration rather than hoisting it
        //  once - matching the shipped loop's own `mov ebx,[esi]` inside
        //  the loop body - because hoisting it lets the compiler recognise
        //  "broadcast one value" and fold the whole loop into `rep stosd`,
        //  which is not the shape here.
        void** const slots = (void**)m_storage.m_data;
        for (unsigned int i = from; i < end; ++i)
        {
            void* v = *fillValue;
            slots[i] = v;
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F4AB050 - grow (if `count` more slots do not fit past the current
//  m_count) and advance m_count by `count`, optionally broadcasting
//  *fillValue into every newly added slot.  `this` in ecx, `retn 8`.
//
//  `count == 0` short-circuits to `count + 1` (i.e. 1) without touching
//  anything - the shipped stream computes it that way rather than a
//  literal `1`, so this keeps the same expression.
//----------------------------------------------------------------------------
int SBhPoTickRelaySlotArray::Grow(void* fillValue, unsigned int count)
{
    if (count == 0)
        return count + 1;

    unsigned int newTotal = m_count + count;
    if (newTotal > m_capacity)
    {
        if (m_growBy == 0)
            return 0;

        unsigned int overflow = (m_count - m_capacity) + count;
        unsigned int grow = (m_growBy > overflow) ? m_growBy : overflow;

        if (!m_storage.SetSize((m_capacity + grow) * 4, 1))
            return 0;

        m_capacity += grow;
    }

    unsigned int oldCount = m_count;
    unsigned int newCount = oldCount + count;
    m_count = newCount;

    if (fillValue != 0)
        FillNewSlots(oldCount, (void* const*)fillValue, newCount);

    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4AB0D0 - remove `count` slots starting at `start`, clamped to what
//  actually fits before `m_count`, sliding every slot past the removed
//  range down over the hole and shrinking `m_count` by however many were
//  actually removed.  Not reached from anything in `CBhPoProjectile`'s own
//  dump - found instead tracing `NIpse::CPoPosBh::TeardownRelatedAgents`
//  (Agent/poposbh_relatedagents.cpp), which embeds this exact struct at
//  its own +0x9C.  `this` in ecx, `retn 8`.
//----------------------------------------------------------------------------
extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int size);

int SBhPoTickRelaySlotArray::RemoveRange(unsigned int start, unsigned int count)
{
    if (start >= m_count)
        return 0;

    unsigned int clampedCount = count;
    if (start + clampedCount > m_count)
        clampedCount = m_count - start;

    unsigned int tail = m_count - start - clampedCount;
    if (tail != 0)
    {
        void** data = (void**)m_storage.m_data;
        memmove(&data[start], &data[start + clampedCount], tail * 4);
    }

    m_count -= clampedCount;
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4AAC40 - shrink the backing storage down to exactly `m_count`
//  pointers (`m_capacity` follows it down to match), via the same
//  `CMemBlock::SetSize` `Grow()` above calls to grow the other way.
//  Return value (the byte delta `SetSize` freed) is computed but unread by
//  this struct's own one caller (`CPoPosBh::TeardownRelatedAgents`) - kept
//  anyway since the shipped stream computes it unconditionally.  `this` in
//  ecx, no stack arguments, `retn`.
//----------------------------------------------------------------------------
int SBhPoTickRelaySlotArray::ShrinkToFit()
{
    unsigned int oldSize = m_storage.m_size;
    m_storage.SetSize(m_count * 4, 1);
    m_capacity = m_count;
    return oldSize - m_storage.m_size;
}

//----------------------------------------------------------------------------
//  0x6F4AB280 - `this` here is the *projectile* (not the array): release
//  slot `index` on its own m_slots.  If it holds a live CAgentTickRelay,
//  retire both its registration references and clear its "armed" flag; if
//  `notify` is non-zero, call the relay's own vtable slot 0x10 with a
//  literal 0.  Either way, null the slot and re-sample this projectile's
//  own sync channel.  `retn 8`.
//----------------------------------------------------------------------------
typedef void (__thiscall *TickRelayNotifyFn)(void*, int);

void CBhPoProjectile::ReleaseTickRelaySlot(unsigned int index, int notify)
{
    if (index >= m_slots.m_count)
        return;

    CAgentTickRelay** slots = (CAgentTickRelay**)m_slots.m_storage.m_data;
    CAgentTickRelay* relay = slots[index];
    if (relay == 0)
        return;

    relay->m_flags &= ~1u;
    relay->m_agentTypeTag = (unsigned int)-1;
    relay->m_agentHandle = (unsigned int)-1;

    if (notify != 0)
    {
        void** vtbl = *(void***)relay;
        ((TickRelayNotifyFn)vtbl[4])(relay, 0);
    }

    ((CAgentTickRelay**)m_slots.m_storage.m_data)[index] = 0;
    ResampleSyncChannel((const CFloat*)&m_pendingSample);
}

//----------------------------------------------------------------------------
//  0x6F4AB1A0 - re-sample this projectile's own CSyncChannel registration.
//  If the currently cached registration is still live (not yet retired),
//  compute the fresh sample (channel->m_value + *value) and, if that would
//  be strictly less than the cached registration's own deadline, retire it
//  - otherwise leave it alone.  Either way, register a fresh sample and
//  cache whatever CSyncChannel::Register hands back.  `retn 4`.
//----------------------------------------------------------------------------
void CBhPoProjectile::ResampleSyncChannel(const CFloat* value)
{
    CSyncChannel* channel =
        (CSyncChannel*)g_pTimeSync->GetChannel((const CHandleObject*)this);

    SSyncRegistration* reg = (SSyncRegistration*)m_pendingRegistration;
    //  The retired-bit test reads byte +0x12 of the registration rather
    //  than the whole `m_flags` dword - the same single-byte-of-a-flags-
    //  word idiom `CAgentRelation::TakeRegistration`'s own writeup
    //  documents (agentrelationregistration.cpp) - bit 0 of that byte is
    //  bit 16 of `m_flags`, i.e. the 0x10000 "retired" bit.
    if (reg != 0 && !(((const unsigned char*)&reg->m_flags)[2] & 1))
    {
        CFloat sum = channel->m_value + *value;
        //  Re-reads `m_pendingRegistration` rather than reusing `reg` -
        //  matching the shipped stream's own re-fetch after the
        //  operator+ call clobbers it, rather than keeping a third
        //  register live across that call the whole way through.
        SSyncRegistration* reg2 = (SSyncRegistration*)m_pendingRegistration;
        if (*(const float*)&sum < *(const float*)&reg2->m_deadline)
            reg2->m_flags |= 0x10000;
    }

    int time = 0;
    m_pendingRegistration = channel->Register(this, &time, value);
}

//----------------------------------------------------------------------------
//  0x6F4AB5D0 - install `relay` at slot `index` (releasing whatever was
//  there first), growing the slot array if `index` does not fit yet, then
//  stamp the relay's own "armed" bit and {agentHandle, agentTypeTag} pair
//  from this projectile's own, and re-sample the sync channel.  The third
//  argument is read by no path in this dump.  `retn 10h`.
//----------------------------------------------------------------------------
void CBhPoProjectile::ArmTickRelaySlot(CAgentTickRelay* relay,
                                       unsigned int index, int /*unused*/,
                                       int notify)
{
    unsigned int idx = index;

    if (idx < m_slots.m_count &&
        ((CAgentTickRelay**)m_slots.m_storage.m_data)[idx] != 0)
        ReleaseTickRelaySlot(idx, notify);

    if (idx >= m_slots.m_count)
    {
        unsigned int need = idx - m_slots.m_count + 1;
        //  Reuse this argument's own now-dead stack slot as Grow()'s
        //  scratch fill value (the shipped stream captures &index before
        //  zeroing it in place) - the same argument-slot-as-scratch idiom
        //  positiontrackspeed.cpp's own writeup documents.
        index = 0;
        m_slots.Grow(&index, need);
    }

    CAgentTickRelay** slots = (CAgentTickRelay**)m_slots.m_storage.m_data;
    slots[idx] = relay;
    relay->m_flags |= 1;
    relay->m_agentHandle = m_agentHandle;
    relay->m_agentTypeTag = m_agentTypeTag;
    ResampleSyncChannel((const CFloat*)&m_pendingSample);
}
