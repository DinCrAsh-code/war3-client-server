//============================================================================
//  0x6F4C3490 - ReleaseTlsIndexIfAllocated (gamemain-batch-3).  Free
//  Storm's own g_dwTlsIndex exactly once: dword_6FAB7BF8 is the "we
//  actually allocated one" flag, right next to g_dwTlsIndex itself
//  (dword_6FAB7BF4, Sync/tlsslot.cpp) - set by whatever allocation path
//  this call tree does not otherwise reach, and cleared here right after
//  the TlsFree so a second call is a no-op.
//============================================================================

extern unsigned int g_dwTlsIndex;          // dword_6FAB7BF4
extern int          g_tlsIndexAllocated;   // dword_6FAB7BF8

void __fastcall FreeThreadLocalIndex(unsigned int tlsIndex);   // 0x6F6D98D0, tlsfree.cpp

void ReleaseTlsIndexIfAllocated()
{
    if (g_tlsIndexAllocated)
    {
        FreeThreadLocalIndex(g_dwTlsIndex);
        g_tlsIndexAllocated = 0;
    }
}
