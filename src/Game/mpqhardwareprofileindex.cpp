//============================================================================
//  0x6F009920 - pick and cache the hardware-compatibility profile index: the
//  last of indices 0/1 whose g_slotTableB3[index] (dword_6FAAE370,
//  0x6F009950's own output - see gamemainarchivehwcheck.cpp) is nonzero,
//  or -1 if neither is.  Computed once and cached in dword_6FA4F6EC.
//============================================================================

extern int g_hwProfileIndexCache;    // dword_6FA4F6EC

//  0x6F009910 - `mov eax, dword_6FAAE370[ecx*4] / retn` - below this
//  batch's own 43; declared and thunked to its own real address.
__declspec(naked) int __fastcall GetGpuCompatFlag(int /*index*/)
{
    __asm { mov eax, 06F009910h }
    __asm { jmp eax }
}

int __fastcall GetHardwareProfileIndex()
{
    int cached = g_hwProfileIndexCache;
    if (cached != -1)
        return cached;

    int best = cached;   // -1 unless a qualifying index is found below
    for (int index = 0; index < 2; index += 1)
    {
        if (GetGpuCompatFlag(index) != 0)
            best = index;
    }

    g_hwProfileIndexCache = best;
    return best;
}
