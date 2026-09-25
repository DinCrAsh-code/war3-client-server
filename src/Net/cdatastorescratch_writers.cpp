//============================================================================
//  A handful of small CDataStoreScratch writer helpers in the same
//  0x6F683xxx-0x684xxx neighbourhood, each building one fixed-shape chunk of
//  a Tock/GameAction-relay packet out of a small source struct. Field
//  offsets are exact; nothing about the source structs themselves (beyond
//  what each function actually touches) is asserted.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

void __stdcall SErrSetLastError(unsigned int code);

//----------------------------------------------------------------------------
//  0x6F683B00 - called directly (not through a vtable) from
//  ServerTock_0x27_PacketHandler's own local-CDataStore setup in the same
//  shape GameAction_0x26_PacketHandler's own ReadHeaderDword call has, but
//  its whole body is `return this` - Tock packets carry no checksum header
//  for this reader to actually consume, unlike GameAction's.
//----------------------------------------------------------------------------
CDataStore* __fastcall NoOpReadHeader(CDataStore* self, unsigned int* /*out*/)
{
    return self;
}

//----------------------------------------------------------------------------
//  0x6F683FC0 - write src[0] (a byte) then the dword at src+1.
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall WriteHeaderFrom(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteByte(src[0]);
    self->WriteDword(*(const unsigned int*)(src + 1));
    return self;
}

//----------------------------------------------------------------------------
//  0x6F684AC0 - write src[4] (a count, as a byte) then that many bytes
//  starting at src+8, one WriteByte call each. `unused` is a genuinely
//  unread trailing stack argument (retn 4 pops it, nothing in the body
//  reads it).
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall WriteCountedBytes(CDataStoreScratch* self, const unsigned char* src, int /*unused*/)
{
    self->WriteByte(src[4]);

    const unsigned char* p = src + 8;
    for (unsigned int i = 0; i < (unsigned int)src[4]; ++i)
        self->WriteByte(p[i]);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F684E00 - build one relay chunk: the dword at src+0, the byte at
//  src+4, `src[4]` raw bytes starting at src+5 (via WriteRaw), then a
//  fixed 0x10-count WriteCountedBytes block starting at src+0x65.
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall WritePacketFrom(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteDword(*(const unsigned int*)src);
    self->WriteByte(src[4]);
    self->TailCallWriteRaw(src + 5, src[4]);
    WriteCountedBytes(self, src + 0x65, 0x10);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F684C90 - write a byte-length-prefixed record plus one fixed trailing
//  byte: `src[0]` (the record's own byte count) as a leading WriteByte,
//  then that many raw bytes starting at `src+1` through WriteRaw, then the
//  fixed byte at `src+0x11` (a slot always reserved in the caller's own
//  local record regardless of how many data bytes it actually holds - see
//  Net::NetClient vtable slot 1's own root, netclient_slot1_send.cpp, for
//  the caller-side struct this reads). `retn` bare (no stack arguments to
//  clean) proves the `__fastcall(self, src)` shape, same as
//  WriteHeaderFrom/WriteCountedBytes above.
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall WriteLengthPrefixedRecord(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteByte(src[0]);
    self->WriteRaw(src + 1, src[0]);
    self->WriteByte(src[0x11]);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F6DAE20 - dispatch through vtable slot 11 (offset 0x2C) of `self`
//  with (arg0, edxArg), or SErrSetLastError(0x57) if `self` is null.  The
//  null path overwrites its *own incoming argument slot* with 0x57 and tail
//  `jmp`s, so SErrSetLastError inherits this function's return address and
//  reads 0x57 as its own argument - no push, no extra instruction.
//
//  **Both stack displacements here are `arg_0`-relative, and getting that
//  wrong is a live hang, not a scoring nit.**  IDA prints the dump's two
//  reads as `[esp+arg_0]` and `[esp+4+arg_0]`; `arg_0` is +4 (the slot above
//  the return address), so they are `[esp+4]` and `[esp+8]`, not `[esp]` and
//  `[esp+4]`.  This function had them both one slot low, which meant the
//  null path wrote 0x57 over its own *return address* and then jumped, and
//  the dispatch path passed that return address to the vtable call as `arg0`.
//  Hooked, that hangs the game before any tracepoint fires - confirmed live,
//  alone, in the crash sweep (docs/notes/mix-config-78-crashes.md).
//  `verify.py` showed both as ordinary displacement differences, which is
//  exactly what a harmless compiler temp-slot difference also looks like.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall DispatchVtableSlot11(void* /*self*/, int /*edxArg*/, unsigned int /*arg0*/)
{
    __asm
    {
        test    ecx, ecx
        jnz     short dispatch
        mov     dword ptr [esp+4], 57h
        jmp     SErrSetLastError
    dispatch:
        mov     eax, [ecx]
        push    esi
        mov     esi, [esp+8]
        push    esi
        push    edx
        mov     edx, [eax+2Ch]
        call    edx
        pop     esi
        retn    4
    }
}
