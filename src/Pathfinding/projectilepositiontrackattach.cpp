//============================================================================
//  0x6F47AAA0 - ProjectilePositionTrack's own override of vtable slot 3,
//  AttachFromTable.  By far the largest function in the whole Position
//  family (78 instructions vs ~17 for PositionTrack's own base version,
//  positiontrackcore.cpp): it resolves the handle-table entry the same
//  double-checked-bucket way the base does, copies the {handle, typeTag}
//  pair the same way - but the entry it resolves is a full
//  Missile/bhpoprojectile.h `CBhPoProjectile*`, not a small handle-only
//  record, and once resolved this override goes on to arm a fresh
//  tick-relay object into that projectile's own slot 0 (if it doesn't
//  already have one) and flip flag bits on two of its other owner objects.
//
//  Confirmed: `entry`'s own +0x14/+0x18 (read here as the generic
//  SIndexedTableEntry handle/typeTag pair the base class already
//  documents) are the exact same bytes as CBhPoProjectile's own
//  m_agentHandle/m_agentTypeTag - this override's own local-record builder
//  makes the identical bit-31-of-+0x14 test CTimeSync::GetChannel makes on
//  the same field (Sync/CSyncChannel.cpp), so the two readings agree rather
//  than collide; see bhpoprojectile.h's own note on this.
//
//  No refcount bump here, unlike the base's own AttachFromTable - this
//  override's own bytes simply do not touch `m_refcount`.
//============================================================================
#include "projectilepositiontrack.h"
#include "bhpoprojectile.h"
#include "agenttickrelay.h"
#include "positionrecordblock.h"

void ProjectilePositionTrack::AttachFromTable(int index,
                                              SIndexedHandleTable* table)
{
    if ((unsigned int)index < table->m_count)
    {
        if (table->m_buckets[index] != 0)
        {
            CBhPoProjectile* entry =
                (CBhPoProjectile*)table->m_buckets[index];
            if (entry != 0)
            {
                m_handle = entry->m_agentHandle;
                m_typeTag = entry->m_agentTypeTag;
            }

            //  Slot 0 of the resolved projectile's own tick-relay array -
            //  if it is already occupied, this override does nothing more.
            //  Written as a branch-then-merge (matching the shipped
            //  `ja`/`xor eax,eax`/`jmp` shape) rather than a ternary: the
            //  ternary spelling gets folded into a single fused
            //  branch-and-skip instead, which is not the shape here.
            CAgentTickRelay* slot0;
            if (entry->m_slots.m_count > 0)
                slot0 = *(CAgentTickRelay**)entry->m_slots.m_storage.m_data;
            else
                slot0 = 0;

            if (slot0 == 0)
            {
                //  An 11-dword local, the same shape SPositionRecordBlock
                //  already names (Widget/positionrecordblock.h) but with
                //  its own field values: both tags carry the identical
                //  magic word (0x5E626568), and the "local vs remote"
                //  sentinel is picked by the same bit-31 test
                //  CTimeSync::GetChannel makes on this same field.
                SPositionRecordBlock rec;
                rec.m_tag0     = 0x5E626568;
                rec.m_tag1     = 0x5E626568;
                rec.m_argC     = 0;
                rec.m_global   = (unsigned int)-1;
                rec.m_zero10   = 0;
                rec.m_zero14   = 0;
                rec.m_zero18   = 0;
                rec.m_zero1C   = 0;
                rec.m_zero20   = 0;
                rec.m_sentinel28 = -1;
                //  Branch, not a ternary/bitwise select - the shipped
                //  stream writes -1 unconditionally first and only
                //  overwrites it with -2 when the bit-31 test fires.
                rec.m_sentinel24 = -1;
                if (entry->m_agentHandle & 0x80000000)
                    rec.m_sentinel24 = -2;

                CBhPoProjectile* obj;
                LookupAndArmSlot(&obj, &rec, 1);

                entry->ArmTickRelaySlot((CAgentTickRelay*)obj, 0, 1, 1);
                ((CAgentTickRelay*)obj)->Arm(1, 1, 1);

                //  Flip flag bits on two more of `entry`'s own owner
                //  objects (+0x94/+0x98, bhpoprojectile.h) - not otherwise
                //  identified this session.  `entry->m_reserved98`'s own
                //  +0x34 is cleared in two separate steps (an AND of the
                //  low three bytes, then a byte store of the top one)
                //  rather than one dword store, and re-read from memory a
                //  third time for the final AND - all three literal to the
                //  shipped stream, not a stylistic choice.
                *(unsigned int*)((char*)entry->m_reserved98 + 0x34) &=
                    0xFF000000;
                ((unsigned char*)((char*)entry->m_reserved98 + 0x34))[3] = 0;
                *(unsigned int*)((char*)entry->m_reserved94 + 0x34) |=
                    0x4000000;
                *(unsigned int*)((char*)entry->m_reserved98 + 0x34) &=
                    0xFEFFFFFF;
            }
        }
    }
}
