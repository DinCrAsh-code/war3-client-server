//============================================================================
//  0x6F0013C0 - free every non-null pointer out of five small, fixed-size
//  module-slot arrays (two elements each, save the fifth which is a single
//  bare pointer) and zero each slot it frees.  Nothing in this call tree
//  reads any of the five back, so only the free/zero behaviour is
//  established; the slot count for each comes straight from the address
//  gap between its own bound constants (all of them are compile-time
//  address constants here, not stored pointers - a fixed array walked
//  downward, the same shape TSGrowableArray teardown loops elsewhere in
//  this repo use, just without the growable header).
//============================================================================

//  Storm ordinal 252 - unresolved real name, kept as the dump's own
//  spelling (see this repo's convention for every other not-yet-identified
//  ordinal).  A plain one-pointer free, `retn 0` at its own call sites.
//  Declared to match Game/gamerestartload.cpp's own signature exactly
//  (`int`, not `void` - a differing return type mangles to a different
//  symbol and only fails at link time).
int __stdcall Storm_252(void* ptr);

//  0x6F001240 - below this batch's own 43; declared and thunked to its own
//  real address.  No arguments at this call site (the leading `push esi`
//  is this function's own callee-saved register, not an argument to it).
__declspec(naked) void ClearMpqModuleReferenceCache()
{
    __asm { mov eax, 06F001240h }
    __asm { jmp eax }
}

//  One combined table, dword_6FAAE280 through dword_6FAAE2A8 (eleven
//  dwords) - a single base symbol rather than five overlapping ones, since
//  two of the five "bound" addresses this function's own dump names
//  (dword_6FAAE298, dword_6FAAE2A0) are themselves the base of another of
//  the five sub-ranges, and funcmap.DATA maps one address to one C++ name.
//  mpqmoduleregister.cpp (0x6F001020, this batch's own) reads the same
//  table's [4]/[5] elements and dword_6FAAE28C (index 3, here the "bound").
extern void* g_mpqModuleSlots[11];   // dword_6FAAE280 + index*4

static void FreeSlotRange(void** top, void** bottom)
{
    void** p = top;
    do
    {
        p -= 1;
        void* elem = *p;
        if (elem != 0)
        {
            Storm_252(elem);
            *p = 0;
        }
    } while (p != bottom);
}

void TeardownMpqModuleSlots()
{
    ClearMpqModuleReferenceCache();

    FreeSlotRange(g_mpqModuleSlots + 6, g_mpqModuleSlots + 4);    // 298 -> 290
    FreeSlotRange(g_mpqModuleSlots + 3, g_mpqModuleSlots + 1);    // 28C -> 284

    if (g_mpqModuleSlots[0] != 0)
    {
        Storm_252(g_mpqModuleSlots[0]);
        g_mpqModuleSlots[0] = 0;
    }

    FreeSlotRange(g_mpqModuleSlots + 8, g_mpqModuleSlots + 6);    // 2A0 -> 298
    FreeSlotRange(g_mpqModuleSlots + 10, g_mpqModuleSlots + 8);   // 2A8 -> 2A0
}
