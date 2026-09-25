//============================================================================
//  0x6F27B460 / 0x6F27B4F0 - a small fixed-capacity (6-entry) unique-value
//  set living at this unit's own +0x2D8 (count) / +0x2E0..+0x2F4 (six
//  consecutive dwords).  Both addresses are read straight off `this`
//  through raw offsets rather than through unit.h's own m_animMode2D8 /
//  m_animName2E0 names: those two fields are already established from a
//  *different* investigation (PlaySpriteAnimation's mode/name arguments,
//  Unit/unit_playanim.cpp) and Unit/unit_save.cpp's own Save/Load already
//  treats +0x2E0..+0x2F4 as a raw six-dword run rather than through named
//  fields for exactly this reason - this pair reads the same bytes as a
//  count-plus-array instead.  Nothing here overrides that reading or
//  changes unit.h; it is the same "do not invent the surrounding layout"
//  discipline CLAUDE.md asks for, applied to two functions that turn out
//  to share bytes with an already-named pair rather than owning them.
//
//  0x6F27B460 - AddUnique(value): linear-scan the count-many live entries
//  for `value`; if found, or the set is already at capacity (6), return 0
//  without touching anything.  Otherwise append and return 1.
//
//  0x6F27B4F0 - Remove(value): linear-scan for `value`; if not found,
//  return 0.  Otherwise swap the last live entry into the removed slot,
//  shrink the count by one, and return 1 - not a stable-order removal.
//
//  Both reached from slot 3's own closure (0x6F2A7E60) with the constant
//  0x1B (27) as `value` at their one confirmed call site
//  (0x6F29D570 -> AddUnique(0x1B); a caller of Remove() also pushes a
//  computed order id rather than a literal). Nothing in this dump gives
//  the set itself a name beyond "field 0x2D8's set".
//============================================================================
#include "unit.h"

int CUnit::AddToField2D8Set(unsigned int value)
{
    unsigned int  count = *(unsigned int*)((char*)this + 0x2D8);
    unsigned int* items = (unsigned int*)((char*)this + 0x2E0);

    for (unsigned int i = 0; i < count; ++i)
    {
        if (items[i] == value)
            return 0;
    }

    if (count >= 6)
        return 0;

    *(unsigned int*)((char*)this + 0x2D8) = count + 1;
    items[count] = value;
    return 1;
}

//  Note the asymmetry with AddUnique above: this scan starts at index 1,
//  and `count <= 1` returns 0 unconditionally - index 0 is never matched
//  and never removed, whatever it holds.  That is what the shipped
//  instructions do (the loop's own `lea edx,[ecx+2E4h]` starts one dword
//  past +0x2E0, and the `count <= 1` guard shares its exit with "scanned
//  to the end without a match"), not a guess at a design intent; nothing
//  in this dump says why index 0 is exempt.
int CUnit::RemoveFromField2D8Set(unsigned int value)
{
    unsigned int  count = *(unsigned int*)((char*)this + 0x2D8);
    unsigned int* items = (unsigned int*)((char*)this + 0x2E0);

    if (count <= 1)
        return 0;

    unsigned int i = 1;
    while (items[i] != value)
    {
        ++i;
        if (i >= count)
            return 0;
    }

    unsigned int last = count - 1;
    *(unsigned int*)((char*)this + 0x2D8) = last;
    items[i] = items[last];
    return 1;
}
