//============================================================================
//  0x6F4C3470 - allocate the TLS slot most of Sync/'s own lazy-singleton
//  machinery reads through GetThreadLocalChecked (see tlsslot.cpp's
//  g_dwTlsIndex, dword_6FAB7BF4 - the same slot, defined here since this is
//  the one call tree that actually allocates it).  0x6F6D9910 registers the
//  freshly allocated index with whatever bookkeeping table tracks live TLS
//  slots (outside this batch's own 43; declared and thunked to its own real
//  address so the call site matches the dump).
//============================================================================
#include <windows.h>

extern unsigned int g_dwTlsIndex;    // dword_6FAB7BF4 - Storm.dll's own live export
                                      // (see tlsslot.cpp / funcmap.py's own note); not
                                      // defined here, same as every other TU that uses it.

extern int g_tlsIndexAllocated;          // dword_6FAB7BF8

//  0x6F6D9910 - register a freshly allocated TLS index (edx left 0 in this
//  call tree's one call site, so its second parameter is never observed to
//  be anything else).
__declspec(naked) void __fastcall RegisterTlsIndex(unsigned int, int)
{
    __asm { mov eax, 06F6D9910h }
    __asm { jmp eax }
}

void __fastcall InitTlsIndex()
{
    unsigned int index = TlsAlloc();
    g_dwTlsIndex = index;
    RegisterTlsIndex(index, 0);
    g_tlsIndexAllocated = 1;
}
