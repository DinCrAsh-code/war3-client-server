//============================================================================
//  Net::NetProvider - vtable slots 38, 39 and 40 (0x6F65AC90, 0x6F65B980,
//  0x6F65B8D0).  Three near-identical "build a fixed-size context block on
//  the stack, copy a name into a throwaway buffer, hand the block to an
//  assert/diagnostic poster" stubs - each posts a different code (0x12,
//  0x14, 0x15) through its own callee (sub_6F659020 / sub_6F659200 /
//  sub_6F659110).
//
//  All three callees are the same shape: a real __except_handler4-style
//  SEH frame around IsDebuggerPresent / SetUnhandledExceptionFilter /
//  TerminateProcess machinery (the CRT's own assert-and-die path), which
//  this build's fixed /GS- /EHs-c- cannot reproduce - the same
//  unreproducible-frame reasoning netprovider_deleteself.cpp's base
//  destructor thunk already documents - so all three stay thunks to their
//  real, unhooked bodies. `retn 8` on all three (confirmed from each
//  function's own trailing bytes, since none of their call trees surfaced
//  a `retn` in the walked disassembly): `this` plus two stack arguments
//  (a context code, a pointer to the block these three roots build).
//============================================================================

class NetProviderAssertSelf
{
public:
    void PostAssert38(int code, void* ctx);
    void PostAssert39(int code, void* ctx);
    void PostAssert40(int code, void* ctx);
};
__declspec(naked) void NetProviderAssertSelf::PostAssert38(int, void*)
{
    __asm { mov eax, 0x6F659020 }
    __asm { jmp eax }
}
__declspec(naked) void NetProviderAssertSelf::PostAssert39(int, void*)
{
    __asm { mov eax, 0x6F659200 }
    __asm { jmp eax }
}
__declspec(naked) void NetProviderAssertSelf::PostAssert40(int, void*)
{
    __asm { mov eax, 0x6F659110 }
    __asm { jmp eax }
}

void __stdcall Storm_501(char* dest, const char* src, unsigned int size);
extern "C" void* memcpy(void*, const void*, unsigned int);

//  IDA's `ValueName` (0x6F87529C) - the fallback name used when the
//  caller's own name pointer is null, the same shared empty-string global
//  netgameresultsubmit.cpp/backdropgenerate.cpp/gameuiplayername.cpp
//  already document.  No funcmap.DATA row for it exists yet (its own
//  GetPlayerDisplayName entry already lives with the same gap), so this
//  is the same accepted difference those three already carry: the shipped
//  code loads the *address* of the shared global, this build re-
//  materialises an equal empty string instead.

namespace Net
{

class NetProvider
{
public:
    //  0x6F65AC90 - vtable slot 38.
    void AssertSlot38(int a0, const void* a4, int a8, const char* name);
    //  0x6F65B980 - vtable slot 39.  Same shape, a wider (46-dword) copy.
    void AssertSlot39(int a0, int a4, const void* bigStruct);
    //  0x6F65B8D0 - vtable slot 40.
    void AssertSlot40(int a0, unsigned char a4, const void* record,
                      const void* bigStruct, int a16);
};

void NetProvider::AssertSlot38(int a0, const void* a4, int a8, const char* name)
{
    struct { int head; int mid[8]; int tail; } ctx;
    ctx.head = a0;
    memcpy(ctx.mid, a4, sizeof(ctx.mid));
    ctx.tail = a8;

    char nameBuf[0x20];
    Storm_501(nameBuf, name ? name : "", 0x20);

    ((NetProviderAssertSelf*)this)->PostAssert38(0x12, &ctx);
}

//  0x6F65B980 - vtable slot 39.  Two plain values plus a wholesale 46-dword
//  (184-byte) struct copy - almost certainly the same shape slot 38's own
//  8-dword copy is a smaller instance of, just a bigger payload.
void NetProvider::AssertSlot39(int a0, int a4, const void* bigStruct)
{
    struct { int head; int mid; int rest[46]; } ctx;
    ctx.head = a0;
    ctx.mid  = a4;
    memcpy(ctx.rest, bigStruct, sizeof(ctx.rest));

    ((NetProviderAssertSelf*)this)->PostAssert39(0x14, &ctx);
}

//  0x6F65B8D0 - vtable slot 40.  Five stack arguments: a value, a byte
//  flag, a pointer to a small record (four dwords - GUID-shaped - followed
//  by a one-byte length and that many bytes of trailing data, copied out
//  with a real `call memcpy` rather than `rep movsd`, the only one of
//  these three slots to do so), a source pointer for the same 46-dword
//  copy slot 39 makes, and one more plain value appended after it.  The
//  shipped frame packs all of this into one unaligned byte-addressed
//  block (`var_E3`, `var_D3`, `var_DF`, `var_D7`); this build's own local
//  layout is compiler-chosen either way (CLAUDE.md - stack-slot
//  displacement is "not source-addressable"), so the struct below matches
//  field order and content, not byte offsets.
void NetProvider::AssertSlot40(int a0, unsigned char a4, const void* record,
                               const void* bigStruct, int a16)
{
    struct
    {
        int           head;
        unsigned char flag;
        int           guid[4];
        unsigned char length;
        char          tail[0x10];
        int           rest[46];
        int           trailing;
    } ctx;
    ctx.head = a0;
    ctx.flag = a4;
    memcpy(ctx.guid, record, sizeof(ctx.guid));
    ctx.length = *((const unsigned char*)record + 0x10);
    memcpy(ctx.tail, (const char*)record + 0x11, ctx.length);
    memcpy(ctx.rest, bigStruct, sizeof(ctx.rest));
    ctx.trailing = a16;

    ((NetProviderAssertSelf*)this)->PostAssert40(0x15, &ctx);
}

}  // namespace Net
