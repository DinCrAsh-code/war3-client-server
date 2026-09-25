//============================================================================
//  0x6F549C90 - append `count` fixed-shape source records onto a growable
//  table, growing it first if the new count would not fit.
//
//  Each source record is 4 dwords immediately followed by a length byte and
//  that many bytes of trailing data (`memcpy`d rather than fixed-width),
//  read relative to a cursor that always starts 0x10 bytes into the source
//  and steps forward by a fixed 0x19-byte stride regardless of how much of
//  a given record's trailing data the length byte actually claims - the
//  caller (CNetData::LoadReplayHeader, netdata_loadreplayheader.cpp) always
//  hands this one record at a time, from stack storage sized to fit that
//  worst case, so the stride's slack is never observed here.
//
//  The table itself is the plain growable-array header this codebase's
//  other TSArray-shaped containers use, but this one is not one of those
//  templates - a distinct, `Net::PLAYERDATA`-typed instance (the SMemFree
//  tag on the growth path CNetData::LoadReplayHeader's own header block
//  reads out of the RTTI-shaped `.?AUPLAYERDATA@Net@@` name) - so it is
//  modelled directly rather than forced through a template this session did
//  not confirm it instantiates.
//
//  sub_6F5388B0 (a default element-size/stride helper) and sub_6F540160
//  (the actual grow/reallocate) both have no dump body in this call tree,
//  so both stay naked thunks to the real, unhooked functions - each
//  `__thiscall(table, int)`, confirmed byte-for-byte against the shipped
//  range (`retn 4` on both).
//============================================================================
#include <string.h>    // memcpy
#include "netdata_appendplayerdata.h"

//  0x6F5388B0 / 0x6F540160 - neither has a dump body in this tree.  Both
//  call sites push their one argument (`push esi; mov ecx,edi; call`)
//  rather than putting it in edx, and `retn 4` is what a one-stack-arg
//  thiscall cleans - a two-register `__fastcall` would clean 0.  A real
//  member function is what gets MSVC to emit that: this toolchain rejects
//  `__thiscall` on a free function (`error C3865: can only be used on
//  native member functions`), so these two are declared as methods of a
//  throwaway subclass rather than free functions, the same reason
//  netdatareplaystate.cpp/netdatadiscardtemp.cpp's own `SNetDataStates`
//  is a method-only local view rather than a free-function pair.
struct SNetPlayerDataTableMethods : SNetPlayerDataTable
{
    int DefaultElemSize(int newCount);
    void Grow(int newCapacity);
};

__declspec(naked) int SNetPlayerDataTableMethods::DefaultElemSize(int newCount)
{
    __asm { mov eax, 0x6F5388B0 }
    __asm { jmp eax }
}

__declspec(naked) void SNetPlayerDataTableMethods::Grow(int newCapacity)
{
    __asm { mov eax, 0x6F540160 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F549C90 - a real member (`this` in ecx, both stack args, `retn 8`):
//  the shipped call site pushes both `count` and `srcRecords`, so a
//  two-register `__fastcall(table, count, srcRecords)` (edx = count, one
//  stack arg, `retn 4`) is the wrong ABI - caught from the diff (`ret 4`
//  against the dump's own `ret 8`) before this was ever hooked.
//----------------------------------------------------------------------------
int SNetPlayerDataTable::Append(unsigned int count, const void* srcRecords)
{
    unsigned int n = count;
    unsigned int newCount = (unsigned int)m_count + n;
    if (newCount > (unsigned int)m_capacity)
    {
        SNetPlayerDataTableMethods* methods = (SNetPlayerDataTableMethods*)this;
        int elemSize = m_elemSizeHint;
        if (elemSize == 0)
            elemSize = methods->DefaultElemSize(newCount);

        unsigned int grownCapacity = newCount;
        unsigned int rem = newCount % (unsigned int)elemSize;
        if (rem != 0)
            grownCapacity = newCount + ((unsigned int)elemSize - rem);

        methods->Grow(grownCapacity);
    }

    const char* src = (const char*)srcRecords + 0x10;
    for (unsigned int i = 0; i < n; ++i)
    {
        char* dst = (char*)m_data + (m_count + (int)i) * 0x19;
        if (dst != 0)
        {
            *(unsigned int*)(dst + 0x00) = *(const unsigned int*)(src - 0x10);
            *(unsigned int*)(dst + 0x04) = *(const unsigned int*)(src - 0x0C);
            *(unsigned int*)(dst + 0x08) = *(const unsigned int*)(src - 0x08);
            *(unsigned int*)(dst + 0x0C) = *(const unsigned int*)(src - 0x04);

            unsigned char len = *(const unsigned char*)src;
            *(unsigned char*)(dst + 0x10) = len;
            memcpy(dst + 0x11, src + 1, len);
        }
        src += 0x19;
    }

    m_count += (int)n;
    return m_count - (int)n;
}
