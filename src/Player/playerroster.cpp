//============================================================================
//  0x6F3F6590 / 0x6F3F6600 / 0x6F3F66A0 - the three tally-list walks that
//  exist as real functions of their own.  (Three more of the same shape do
//  not: IDA models 0x6F3F64F0, 0x6F3F6540 and 0x6F3F6660 as tail *chunks*
//  of their one caller each, so they have no `### name @ 0x...` heading and
//  nothing in this pipeline could score them separately - they are written
//  out inside their forwarders instead, in playerscore_rosterwalks.cpp.)
//
//  All three walk the intrusive list at +0x08 from the tail, which is
//  Storm's ordinary encoding: a positive m_prevlink is the predecessor's
//  object pointer, a negative one the complement of the terminator's
//  address, so one sign test both steps and ends the walk.  The masked
//  entry (`setle` / `sub 1` / `and`) is the walk's *entry*; inside it the
//  field is used raw, exactly as Storm/storm.h's own note describes.
//
//  Own translation unit: 0x6F40B300 / 0x6F40B260 / 0x6F40B280 tail-jump
//  into these for real, and the two unit-type cost probes and the two
//  upgrade probes below have to stay real calls too.
//============================================================================
#include "playerscore.h"

//  0x6F29B800 / 0x6F29B830 - the two cost fields of a unit type
//  (Unit/unittypecosts.cpp).
int __fastcall UnitTypeCost20_6F29B800(unsigned int fourCC);
int __fastcall UnitTypeCost24_6F29B830(unsigned int fourCC);

//  0x6F2B01A0 - how many types the upgrade table says this upgrade covers,
//  and 0x6F2B00C0 - does it cover this one?  (Item/upgradeaffects.cpp.)
int __fastcall UpgradeAffectedTypeCount(unsigned int upgradeId);
int __fastcall UpgradeAffectsType(unsigned int typeId, unsigned int upgradeId);

//----------------------------------------------------------------------------
//  0x6F3F6590 - every entry flagged with either of the two 0x0C bits
//  contributes (its two counts, added) times (its type's two costs, added).
//
//  The running total lives in a *stack slot* on both sides (`add
//  [esp+var_4], eax`), not in a register: with the list, the entry, the
//  type id and the cost all live across the two calls there is nothing left
//  to keep it in.  That is the allocator's answer to this body, not
//  something the source asks for.
//----------------------------------------------------------------------------
int CPlayerTallyList::TotalTalliedCost()
{
    int total = 0;

    SPlayerTally* entry = Tail();
    if ((int)entry > 0)
    {
        for (;;)
        {
            SPlayerTally* tally = entry;
            if (tally->m_flags30 & 0x0C)
            {
                unsigned int typeId = tally->m_typeId;
                int cost = UnitTypeCost20_6F29B800(typeId);
                cost += UnitTypeCost24_6F29B830(typeId);
                total += (tally->m_count2C + tally->m_count1C) * cost;
            }

            SPlayerTally* next = Prev(entry);
            if ((int)next <= 0)
                break;
            entry = next;
        }
    }

    return total;
}

//----------------------------------------------------------------------------
//  0x6F3F6600 - the m_count1C total over the flag-4 entries whose type has
//  any cost at all.  The two costs are added and tested against zero in one
//  step, with the first spilled across the second call - which is what
//  naming both of them produces.
//----------------------------------------------------------------------------
int CPlayerTallyList::CountFlag4WithCost()
{
    int total = 0;

    SPlayerTally* entry = Tail();
    if ((int)entry > 0)
    {
        for (;;)
        {
            SPlayerTally* tally = entry;
            if (tally->m_flags30 & 4)
            {
                unsigned int typeId = tally->m_typeId;
                int first = UnitTypeCost20_6F29B800(typeId);
                int second = UnitTypeCost24_6F29B830(typeId);
                if (first + second != 0)
                    total += tally->m_count1C;
            }

            SPlayerTally* next = Prev(entry);
            if ((int)next <= 0)
                break;
            entry = next;
        }
    }

    return total;
}

//----------------------------------------------------------------------------
//  0x6F3F66A0 - the ratio.
//
//  The count is asked for first and the whole walk is skipped when it is
//  zero, which is the one place the answer is g_unk6FAAE574 unscaled.  The
//  divisor's own conversion happens *before* the numerator's because
//  `numerator / divisor` evaluates the argument before the object
//  expression - and the shipped stream has them in exactly that order.
//
//  The shipped code also writes `count` back into its own incoming
//  parameter slot and reads it out again after the walk (the
//  "writes its results back into its own parameter slots" idiom); nothing
//  in the source asks for that, and reassigning the parameter is not what
//  produces it - the local is live across the walk either way.
//----------------------------------------------------------------------------
CFloat CPlayerTallyList::UpgradeRatio(unsigned int upgradeId)
{
    int count = UpgradeAffectedTypeCount(upgradeId);
    if (count == 0)
        return g_unk6FAAE574;

    int sum = 0;

    SPlayerTally* entry = Tail();
    if ((int)entry > 0)
    {
        for (;;)
        {
            SPlayerTally* tally = entry;
            if (UpgradeAffectsType(tally->m_typeId, upgradeId))
                sum += tally->m_count1C;

            SPlayerTally* next = Prev(entry);
            if ((int)next <= 0)
                break;
            entry = next;
        }
    }

    return g_unk6FAAE574 * (CFloatFromInt(sum) / CFloatFromInt(count));
}

//----------------------------------------------------------------------------
//  0x6F3F6760 - see playerscore.h's own note.  Two tail-linked lists, both
//  folded through the same xor-rotate-3 the checksum family throughout
//  this batch uses.  The second list's own header (linkoffset/next/tail at
//  +0x30/+0x34/+0x38) is read through raw offset casts, exactly the shape
//  Tail()/Prev() give the first list at +0x08/+0x0C/+0x10 - not exposed as
//  a named member because nothing else in this class's already-scored
//  siblings (TotalTalliedCost and friends, above) ever reads it, and this
//  is the only function in the whole repo that does.
//----------------------------------------------------------------------------
unsigned int __fastcall PlayerTallyEntryChecksumFields(void* entry);

unsigned int CPlayerTallyList::ComputeChecksum() const
{
    unsigned int hash = 0;

    SPlayerTally* entry = Tail();
    while ((int)entry > 0)
    {
        unsigned int contribution = PlayerTallyEntryChecksumFields(entry);
        hash ^= contribution;
        hash = (hash << 3) | (hash >> 29);

        entry = Prev(entry);
    }

    const char* self = (const char*)this;
    int linkoffset2 = *(const int*)(self + 0x30);
    int prevlink2   = *(const int*)(self + 0x38);
    SPlayerTally* entry2 = prevlink2 > 0 ? (SPlayerTally*)prevlink2 : 0;

    while ((int)entry2 > 0)
    {
        unsigned int field18 = *(const unsigned int*)((const char*)entry2 + 0x18);
        hash ^= field18;
        hash = (hash << 3) | (hash >> 29);

        entry2 = *(SPlayerTally**)((char*)entry2 + linkoffset2 + 4);
    }

    return hash;
}
