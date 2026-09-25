//============================================================================
//  0x6F3B2DD0 - 0x6F3B2E90: the seven JASS natives that answer a question
//  about a *type id* rather than about an object - the three item-type
//  predicates, the two food figures, the point value, and the one that
//  needs no table at all.
//
//  Six of the seven are two instructions: load the four-character type id
//  out of the caller's frame into ecx and tail-jump into the registry probe
//  that reads the field.  Each probe is __fastcall with the id in ecx, so
//  a `return Probe(id);` as the last statement is exactly the shipped
//  `mov ecx,[esp+4]` / `jmp` (docs/msvc-vc8-idioms.md - a call with the
//  same argument list as the last statement becomes a tail jump).
//
//  Two of those six cannot reach 1.000 and it is nothing to do with the
//  source.  IDA models 0x6F2B8E00 and 0x6F29C4D0 as *tail chunks* of the
//  functions that jump into them rather than as functions of their own, so
//  their bodies are appended to these two natives' listings after the
//  `retn` and their seventeen instructions land in the denominator.  A tail
//  call compiles to `jmp <symbol>`, not to an inlined copy, so nothing any
//  spelling does moves it - see docs/msvc-vc8-idioms.md, "A shared tail
//  chunk is not a score you can chase", and note that both chunks really
//  are shared (0x6F2B8E00 has two callers, 0x6F29C4D0 has three), which is
//  what rules out the inlining escape the *other* entry there allows.
//
//  One translation unit: none of the seven calls another, and every probe
//  they reach is a real call into a different module.
//============================================================================
#include "itemtypequeries.h"
#include "unittypequeries.h"

//----------------------------------------------------------------------------
//  0x6F3B2DD0 - `IsItemIdPowerup` "(I)B".
//----------------------------------------------------------------------------
int __cdecl JASS_IsItemIdPowerup(int itemId)
{
    return ItemTypeIsPowerup(itemId);
}

//----------------------------------------------------------------------------
//  0x6F3B2DE0 - `IsItemIdSellable` "(I)B".
//----------------------------------------------------------------------------
int __cdecl JASS_IsItemIdSellable(int itemId)
{
    return ItemTypeIsSellable(itemId);
}

//----------------------------------------------------------------------------
//  0x6F3B2DF0 - `IsItemIdPawnable` "(I)B".
//----------------------------------------------------------------------------
int __cdecl JASS_IsItemIdPawnable(int itemId)
{
    return ItemTypeIsPawnable(itemId);
}

//----------------------------------------------------------------------------
//  0x6F3B2E00 - `GetFoodMade` "(I)I".  The +0x60 word of the unit type.
//----------------------------------------------------------------------------
int __cdecl JASS_GetFoodMade(int unitId)
{
    return UnitTypeField60_6F29BB20(unitId);
}

//----------------------------------------------------------------------------
//  0x6F3B2E10 - `GetFoodUsed` "(I)I".  The +0x5C word.
//----------------------------------------------------------------------------
int __cdecl JASS_GetFoodUsed(int unitId)
{
    return UnitTypeField5C_6F29BAF0(unitId);
}

//----------------------------------------------------------------------------
//  0x6F3B2E20 - `GetUnitPointValueByType` "(I)I".  The +0x1D0 word.
//----------------------------------------------------------------------------
int __cdecl JASS_GetUnitPointValueByType(int unitId)
{
    return UnitTypePointValue_6F29C4D0(unitId);
}

//----------------------------------------------------------------------------
//  0x6F3B2E90 - `IsHeroUnitId` "(I)B", and the only one of the seven that
//  asks the registry nothing: a hero type id is one whose *first* character
//  is an upper-case letter, and that is the whole test.
//
//  `shr` and not `sar` on the way in, so the id is unsigned.  The rest is
//  MSVC's own range fold: `letter >= 'A' && letter <= 'Z'` becomes the bias
//  `add eax, -65` and the single unsigned `cmp ecx,eax` / `sbb eax,eax` /
//  `add eax,1` against 25.
//
//  Write the bias by hand and it does *not* match.  Every spelling of the
//  subtraction - `- 'A'`, `+ -65`, `+ 0xFFFFFFBFu`, `0u - 'A' + letter` -
//  compiles to `sub eax, 65`, and only the two-ended comparison produces
//  the shipped `add eax, -65`: the constant reaches the addition as the
//  negative lower bound of a range the optimiser formed, not as a literal
//  the source wrote.  (docs/msvc-vc8-idioms.md.)
//----------------------------------------------------------------------------
int __cdecl JASS_IsHeroUnitId(int unitId)
{
    unsigned int letter = (unsigned int)unitId >> 24;

    return letter >= 'A' && letter <= 'Z';
}

//============================================================================
//  The two handle-taking twins, 0x6F3C4FF0 and 0x6F3C5E80.
//
//  IDA pulls these into the same dumps because each ends in the *same* tail
//  jump the `...Id` native above ends in - so the shared chunk's seventeen
//  instructions land in their listings too, and neither can reach 1.000 for
//  the reason this file's header already gives.
//
//  The shape is one the registration table explains: a JASS `Hitem;` or
//  `Hunit;` is a handle, so these resolve it first and then ask the same
//  question of the type id at +0x30 of the resolved object.  A handle that
//  does not resolve answers 0, and the shipped code gets that for free - it
//  returns the resolver's own null in eax rather than branching to a
//  separate `xor eax,eax`.
//============================================================================
#include "itemhandlemain.h"
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F3C4FF0 - `IsItemSellable` "(Hitem;)B".
//----------------------------------------------------------------------------
int __cdecl JASS_IsItemSellable(int hItem)
{
    CAgent* item = ResolveItemHandle(hItem);

    if (item == 0)
        return 0;

    return ItemTypeIsSellable(*(unsigned int*)((char*)item + 0x30));
}

//----------------------------------------------------------------------------
//  0x6F3C5E80 - `GetUnitPointValue` "(Hunit;)I".
//----------------------------------------------------------------------------
int __cdecl JASS_GetUnitPointValue(int hUnit)
{
    CUnit* unit = ResolveUnitHandle(hUnit);

    if (unit == 0)
        return 0;

    return UnitTypePointValue_6F29C4D0(*(unsigned int*)((char*)unit + 0x30));
}
