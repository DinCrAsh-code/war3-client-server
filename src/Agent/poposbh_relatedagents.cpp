//============================================================================
//  NIpse::CPoPosBh - vtable slot 4 (Slot4, 0x6F4AB430) and the related-agent
//  array teardown it calls (0x6F4AB2E0), plus the +0xBC sync-registration
//  clear (0x6F4AB130) shared with idx18's own still-`TODO` override.  See
//  poposbh.h for the class-level header comment.
//
//  This is the "array shrink/rebalance helpers" gap the earlier
//  cluster-H-remainder session left `Slot4` TODO over
//  (docs/targets/NTempestNIpse_clusterH_remainder.md): 0x6F4AB2E0's own
//  callees, 0x6F4AB0D0 and 0x6F4AAC40, turned out to be exactly
//  `SBhPoTickRelaySlotArray::RemoveRange`/`::ShrinkToFit`
//  (Missile/bhpoprojectile_slots.cpp) - the SAME growable-pointer-array
//  struct `CBhPoProjectile` keeps at its own +0x9C for its tick-relay
//  slots, embedded here at CPoPosBh's own +0x9C instead. The two classes
//  are not related by inheritance; they just both happen to carry one of
//  this engine's generic "growable array of raw pointers" utility structs,
//  the same way `SPositionRecordArray`'s own header comment already
//  documents for the analogous 0x2C-byte-record shape
//  (Widget/positionrecordarray.h). Confirmed, not assumed: `Slot4`'s own
//  teardown chain reaches `SBhPoTickRelaySlotArray::Grow` (0x6F4AB050) at
//  `this+0x9C` for the array's own storage growth on the `Load` side, and
//  `RemoveRange`/`ShrinkToFit` read/write the identical
//  `CMemBlock`-plus-growBy/capacity/count fields at the identical relative
//  offsets (`+0xC`=m_data, `+0x10`=m_size, `+0x14`=m_growBy internally
//  unused here, `+0x18`=m_capacity, `+0x1C`=m_count) that
//  `bhpoprojectile.h`'s own struct already names.
//============================================================================
#include "poposbh.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F4AB130 - clear the +0xBC sync registration's own "sample me" bit
//  (m_flags |= 0x10000, the same "retired" bit
//  `CBhPoProjectile::ResampleSyncChannel`'s own writeup documents reading
//  off `SSyncRegistration::m_flags`) and this object's own pointer to it.
//  No teardown call of its own - the registration record itself is
//  released by whatever actually owns the sync channel it was made
//  against, not by this class. `this` in ecx, no stack arguments, `retn`.
//----------------------------------------------------------------------------
void CPoPosBh::ClearBcRegistration()
{
    SSyncRegistration* reg = m_syncReg;
    if (reg != 0)
    {
        reg->m_flags |= 0x10000;
        m_syncReg = 0;
    }
}

//----------------------------------------------------------------------------
//  0x6F4AB2E0 - tear down every live slot of the +0x9C related-agent array,
//  then empty the array itself (`RemoveRange(0, m_count)` after the loop,
//  which - since `count` is passed as the array's own current `m_count` -
//  always removes every remaining slot rather than a sub-range), then, if
//  `shrink` is set, release the array's own backing storage too
//  (`ShrinkToFit`), and finally re-sample this projectile's own sync
//  channel against +0xC0 (`CBhPoProjectile::ResampleSyncChannel`, already
//  real, Missile/bhpoprojectile_slots.cpp) regardless of whether the array
//  had anything in it.
//
//  Each live entry gets: its own +0x24/+0x28 pair reset to {-1,-1} (a
//  "pending"/sentinel reset, the same shape `SHandleRegistrationRequest`'s
//  own -1 sentinels use elsewhere in this cluster), bit 0 of its own +0x30
//  cleared, and - only when `notify` is set - its own vtable slot 4
//  ("Slot4"-shaped) called with a literal 0, the SAME generic
//  `Slot4(int)` release convention this whole class family shares (see
//  popos.h's own `CPoPos::Slot4`). The entry's own concrete type is not
//  established by this dump - it is addressed purely through this shared
//  vtable-slot convention, exactly the way `CLAUDE.md`'s own examples
//  document for an unidentified receiver.
//----------------------------------------------------------------------------
typedef void (__thiscall *GenericSlot4Fn)(void*, int);

void CPoPosBh::TeardownRelatedAgents(int notify, int shrink)
{
    unsigned int i = 0;

    //  The shipped loop re-reads `m_relatedAgents.m_storage.m_data` (the
    //  array base) fresh at every one of the four points below rather than
    //  caching it once, and re-reads `m_relatedAgents.m_count` fresh as its
    //  own loop bound rather than caching that either - the same "reload
    //  the field every time instead of a local" idiom
    //  `SPositionRecordArraySink::AppendRecord`'s own header comment
    //  documents for the identical reason (Widget/positionrecordarray.cpp).
    if (m_relatedAgents.m_count > 0)
    {
        do
        {
            void** slot = (void**)m_relatedAgents.m_storage.m_data + i;
            if (*slot != 0)
            {
                void* entry = *(((void**)m_relatedAgents.m_storage.m_data) + i);
                *(unsigned int*)((char*)entry + 0x28) = 0xFFFFFFFF;
                *(unsigned int*)((char*)entry + 0x24) = 0xFFFFFFFF;

                entry = *(((void**)m_relatedAgents.m_storage.m_data) + i);
                *(unsigned int*)((char*)entry + 0x30) &= ~1u;

                if (notify != 0)
                {
                    entry = *(((void**)m_relatedAgents.m_storage.m_data) + i);
                    GenericSlot4Fn fn = (GenericSlot4Fn)(*(void***)entry)[4];
                    fn(entry, 0);
                }

                *(((void**)m_relatedAgents.m_storage.m_data) + i) = 0;
            }

            i += 1;
        } while (i < m_relatedAgents.m_count);
    }

    unsigned int finalCount = m_relatedAgents.m_count;
    if (finalCount != 0)
        m_relatedAgents.RemoveRange(0, finalCount);

    if (shrink != 0)
        m_relatedAgents.ShrinkToFit();

    ((CBhPoProjectile*)this)->ResampleSyncChannel((const CFloat*)&m_pdl);
}

}  // namespace NIpse
