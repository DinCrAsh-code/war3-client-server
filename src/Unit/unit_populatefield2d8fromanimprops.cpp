//============================================================================
//  0x6F285910 - CUnit::PopulateField2D8FromAnimProps, from the CUnit
//  vtable closure walk (cunit_agent2_worklist.md).
//
//  Reads a data-source object's "animPropsCount"/"animProps" properties
//  (both resolved through the same named-property lookup, sub_6F32C880,
//  out of scope for this batch and not in any of the eight parallel
//  worklists either - declared opaque here) and hands each element, walked
//  *backwards* from the last to the first, to CUnit::AddToField2D8Set
//  (unit_field2d8set.cpp).  The reverse walk is the shipped `sub esi,1`
//  loop, not a stylistic choice - a forward `for` here compiles to a
//  different compare/branch pair.
//============================================================================
#include "unit.h"

//  0x6F32D3E0 - thiscall(ecx=dataSource), `retn 0`.  Resolves the
//  "animPropsCount" named property and returns its integer value, or 0 if
//  the data source has no such property.
//  0x6F32D460 - __fastcall(ecx=dataSource, edx=index), `retn 0`.
//  Resolves the "animProps" named property (an array) and returns the
//  element at `index`, bounds-checked against the array's own count.
//
//  Both declared as members of an opaque stand-in type rather than as
//  free functions: `__thiscall` on a free function is rejected by this
//  compiler (C3865), and the fastcall one still needs `ecx` to be the
//  data source, which a member call gives for free.
struct SAnimPropsSource
{
    int QueryAnimPropsCount();
    int __fastcall QueryAnimPropAt(int index);
};

void CUnit::PopulateField2D8FromAnimProps(void* dataSource)
{
    SAnimPropsSource* source = (SAnimPropsSource*)dataSource;
    int count = source->QueryAnimPropsCount();
    if (count != 0)
    {
        do
        {
            count -= 1;
            AddToField2D8Set(source->QueryAnimPropAt(count));
        } while (count != 0);
    }
}

//----------------------------------------------------------------------------
//  Link-completion thunks - both addresses are out of scope for every one
//  of the eight parallel agents this run (see the comment above the
//  struct), so nothing else in the tree defines them either. Naked
//  redirect thunks - the same idiom this repo uses for any other
//  out-of-scope callee - make this TU link without claiming either
//  address as a real reconstruction (no funcmap.py row points at these).
//----------------------------------------------------------------------------
__declspec(naked) int SAnimPropsSource::QueryAnimPropsCount()
{
    __asm { mov eax, 06F32D3E0h }
    __asm { jmp eax }
}

__declspec(naked) int __fastcall SAnimPropsSource::QueryAnimPropAt(int)
{
    __asm { mov eax, 06F32D460h }
    __asm { jmp eax }
}
