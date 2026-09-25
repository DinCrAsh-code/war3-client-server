//============================================================================
//  0x6F27A320 - CUnit::QueryDetection: is this unit detected by one player,
//  for either of the two kinds of detection the caller asks about?
//
//  JASS's IsUnitDetected (jassnatives_unitquery.cpp) is the only caller in
//  this tree and it asks for both kinds at once, with -1.
//
//  The handle pair at CUnit+0x130 is `m_ref130`, and it is read exactly the
//  way itemhandleresolve.h documents every other one: the type tag at +0x04
//  first, the handle at +0x00 second, `and`ed together and refused when the
//  result is all-ones.  The resolved block then carries two per-player
//  arrays, at +0x2C and +0x74, and `kinds` picks which of them count - bit
//  0 the first, bit 1 the second.
//
//  Three shapes are the shipped ones and none of them is the obvious
//  spelling:
//
//   * `this + 0x130` is formed *before* the refusal test (`add ecx,130h` at
//     0x6F27A32C, the `cmp` at 0x6F27A332), which is what taking the
//     member's address into a named pointer does and what passing
//     `&m_ref130` at the call site does not.
//
//   * the two tests are accumulated with `|`, not short-circuited with
//     `||`.  The shipped code computes both `setnz`es and `or`s them, and
//     `||` over two side-effect-free operands branches instead.
//
//   * the answer is a literal 1 reached by a *branch*, not the accumulator
//     widened.  The tail is `test ecx,ecx` / `jz` / `mov eax,1`, and
//     `return found != 0;` gives `xor eax,eax` / `test` / `setne al`
//     instead - measured.
//
//  And the three refusals share one `xor eax,eax` / `retn 8` at the very
//  end, which the nested-guard shape below produces and three separate
//  `return 0;` statements do not (those hoist the zero into the middle -
//  the split jassnatives_unitquery.cpp wants and this function does not).
//============================================================================
#include "unit.h"
#include "itemhandleresolve.h"

//  The two per-player arrays inside the resolved detection block.  Only the
//  two offsets this function indexes are named; nothing else reads it.
struct SUnitDetectionMasks
{
    char m_reserved00[0x2C];
    int  m_byKindA[18];    // +0x2C
    int  m_byKindB[18];    // +0x74
};

int CUnit::QueryDetection(unsigned int playerIndex, int kinds)
{
    SOptionalHandleRef* ref = (SOptionalHandleRef*)&m_ref130;
    if ((ref->m_typeTag & (int)ref->m_handle) != -1)
    {
        SUnitDetectionMasks* masks =
            (SUnitDetectionMasks*)QueryHandleField0x54(ref);
        if (masks != 0)
        {
            int found = 0;
            if (kinds & 1)
                found = (masks->m_byKindA[playerIndex] != 0);
            if (kinds & 2)
                found |= (masks->m_byKindB[playerIndex] != 0);

            if (found)
                return 1;
        }
    }
    return 0;
}
