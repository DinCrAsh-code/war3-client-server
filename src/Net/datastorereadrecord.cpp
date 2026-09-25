//============================================================================
//  0x6F4D8140 - read a 3-byte-then-4-byte record out of a CDataStore into a
//  caller-owned buffer: three CDataStore::ReadByte calls at buf+0/+1/+2,
//  then one call into 0x6F4D77C0 for the four bytes starting at buf+4 (buf+3
//  itself is never written by this function - presumably padding a caller
//  elsewhere fills, or a field this call tree does not otherwise reach).
//
//  0x6F4D77C0 is not in this batch's own dump (no `### sub_6F4D77C0`
//  heading in the combined dump) and is not part of any of the 8
//  CItem-closure batches; declared as a naked, unclaimed redirect.  Its own
//  call site passes the destination in `edx` with nothing pushed - a
//  `__fastcall`-shaped second argument, unlike ReadByte's stack pointer -
//  so it is declared to match exactly that register convention.  `retn 0`
//  inferred from this function's own trailing plain `retn` (a tail call
//  would need to match).
//
//  `edx` (the destination buffer) is this function's own second argument;
//  ecx is the CDataStore.  Returns the same CDataStore* pointer (`this`),
//  matching ReadByte's own "always returns `this`" stream-`>>` shape.
//  `retn 0`.
//============================================================================
#include "cdatastore.h"

__declspec(naked) void __fastcall Thunk_0x6F4D77C0(void* /*store*/, void* /*dest*/)
{
    __asm
    {
        mov     eax, 06F4D77C0h
        jmp     eax
    }
}

CDataStore* __fastcall ReadDataStoreRecord(CDataStore* store, unsigned char* buf)
{
    store->ReadByte(buf);
    store->ReadByte(buf + 1);
    store->ReadByte(buf + 2);
    Thunk_0x6F4D77C0(store, buf + 4);
    return store;
}
