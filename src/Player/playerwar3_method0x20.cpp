//============================================================================
//  0x6F40F320 - CPlayerWar3::Method_0x20, vtable slot 8 (+0x20).  Same
//  SPositionRecordBlock/SPositionRecordSink shape Position::RecordSlot8
//  (positionrecordbuild.cpp) already established for CItem's own slot 8 -
//  reused directly rather than re-derived - just with a real loop over the
//  AbilityEntry array instead of one block, different tag/argC words, and
//  one extra block built after the loop.
//
//  `SPositionRecordSink::SubmitRecord` never reads `this` (see
//  positionrecordbuild.cpp's own note); the shipped body still passes each
//  array element's own address as `this`, so the cast is reproduced here
//  even though nothing inside the callee looks at it.
//============================================================================
#include "playerwar3.h"
#include "positionrecordblock.h"

//  Declared exactly as positionrecordbuild.cpp declares it - re-declaring
//  (not redefining) the same mangled `int __thiscall
//  SPositionRecordSink::SubmitRecord(int,int,void*,int)` symbol so the
//  linker resolves both call sites to the one real definition there.
//  link_check.py is what would catch this drifting.
struct SPositionRecordSink
{
    int __thiscall SubmitRecord(int a, int b, void* block, int d);
};

int CPlayerWar3::Method_0x20(int context)
{
    int count = CAgent::Method_0x20(context);

    //  A running pointer, incremented one AbilityEntry (0x10 bytes) per
    //  iteration - not array indexing recomputed from `this` each time -
    //  matching the shipped `add ebx, 10h` loop exactly.
    AbilityEntry* entry = m_abilityEntries;

    for (int i = 0; i < 0x19; ++i)
    {
        if (context != 0)
        {
            SPositionRecordBlock block;
            block.m_tag0 = 0x5E70726F;
            block.m_tag1 = 0x60666C67;
            block.m_argC = 0;
            block.m_global = 0;
            block.m_zero10 = 0;
            block.m_zero14 = 0;
            block.m_zero18 = 0;
            block.m_zero1C = 0;
            block.m_zero20 = 0;
            block.m_sentinel28 = -1;
            block.m_sentinel24 = -1;

            ((SPositionRecordSink*)entry)
                ->SubmitRecord(count, context, &block, 0);
        }

        ++count;
        ++entry;
    }

    if (context != 0)
    {
        SPositionRecordBlock block;
        block.m_tag0 = 0x5E70726F;
        block.m_tag1 = 0x60666C67;
        block.m_argC = 0x1775;
        block.m_global = 0;
        block.m_zero10 = 0;
        block.m_zero14 = 0;
        block.m_zero18 = 0;
        block.m_zero1C = 0;
        block.m_zero20 = 0;
        block.m_sentinel28 = -1;
        block.m_sentinel24 = -1;

        //  +0x40 + 25 * 0x10 == +0x1D0 - the 26th AbilityEntry, the one
        //  the loop above never reaches.
        ((SPositionRecordSink*)&m_abilityEntries[0x19])
            ->SubmitRecord(count, context, &block, 0);
    }

    return count + 1;
}
