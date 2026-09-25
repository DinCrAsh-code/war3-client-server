//============================================================================
//  Net::NetProviderBNET - vtable slots 22 (0x6F65E550), 23 (0x6F65DFB0),
//  24 (0x6F65F980), 25 (0x6F65DFD0) and 32 (0x6F65EE50).  A small family
//  of "copy a header out of the +0x6A0 OS handle" and "receive one
//  message" overrides sharing the +0x7B4../+0x7C5 header/payload fields
//  slot 8's own cleanup walk never touches.
//============================================================================

#include <windows.h>
extern "C" void* memcpy(void*, const void*, unsigned int);

//  netprovider_stubs.cpp / netprovider_slot25.cpp
namespace Net
{
class NetProvider
{
public:
    int SetCachedWord(unsigned short);
    int Method_25(void*, void*, int);
};
}

//  0x6F6D8950 - ConditionVariable::SignalOne, already reconstructed
//  (gameaction0x26_notify.cpp) under this exact declaration; NOT
//  redeclared naked here, just the struct + extern needed to call it.
struct ConditionVariable
{
    void SignalOne(unsigned int value);
};

//  batch-I-playertable-and-bnet: the shared BNET-socket-resolve wrapper
//  every one of this file's five real bodies below opens with.
//  sub_6F688F30 resolves a raw OS/Winsock handle against a shared flags
//  table (g_bnetSocketResolveTable), writing an int status to *outStatus
//  and returning either the resolved internal handle or 0 on failure
//  (the caller's own `edi = 5` default is a Storm-style Winsock-error-
//  family fallback returned on failure).  On success, `*outStatus` (-1 =
//  "no signal needed", 2 = the boundary between the array's two
//  ConditionVariable slots) selects and signals one of
//  g_bnetSocketSignalPair's two entries.  A whole companion OS-socket
//  subsystem (this resolver, its own flags table, the two Winsock-layer
//  receive/set primitives sub_6F69C190/sub_6F691700/sub_6F6A9AC0/
//  sub_6F6A9CE0 each real body below calls through it) - out of scope for
//  this networking-desync batch, same as ConditionVariable::NotifyOne
//  itself (gameaction0x26_notify.cpp).  `this`(ecx) = &g_bnetSocketResolveTable,
//  `edx` = each caller's own second incoming parameter, passed through
//  unread (confirmed dead inside this function's own body - the shipped
//  call site never reloads edx after the caller's own entry, so whatever
//  it held there is what arrives here); five stack args
//  (handle, arg1, outStatus, arg2, arg3) - every real call site below
//  always passes literal 0/0/1 for (arg1, arg2, arg3) - `retn 14h`.
//
//  **Bug fix, not a rewrite** (batch-K-netprovider-misc, found while
//  reconstructing SendBnetFramed_6F68A430 in netproviderbnet_seh.cpp,
//  a fifth real caller of this exact address): the previous declaration
//  modeled only 4 stack params (`arg1, outStatus, arg2, arg3` - no
//  `handle`), one short of the `retn 14h` the shipped callee actually
//  cleans, and it put `outStatus` at the *second* stack position where
//  the shipped body reads its out-pointer at the *third* (`arg_8`,
//  confirmed by both the callee's own two unconditional dereferences at
//  the fail and success tails, and by 0x6F68A4B0's own real disassembly -
//  NetProviderBnetHandleSetWord, one of this file's own four existing
//  callers - which pushes `1, 0, &var_4, 0, handle` in that order, i.e.
//  `handle` at `arg_0` and the real stack address at `arg_8`). Every one
//  of this file's own four existing call sites was passing `handle` as
//  the constant `0` it happened to already have there, and passing
//  `&status` at the position the shipped body never reads instead of the
//  position it dereferences unconditionally on both its success and
//  failure paths - i.e. every one of them handed the real, unhooked
//  `sub_6F688F30` a **null** out-pointer to write through. Live crash
//  class, invisible to verify.py (naked thunk, scores its own two
//  instructions only) and to thunk_abi_audit.py (this symbol has no
//  `; readable` comment in the listing - the same bare-C-linkage gap
//  ReallocProviderConfigArray's own fix, netprovider_copyconfig.cpp,
//  already worked around - so it fell into that tool's "no PROC in
//  build/*.asm" bucket instead of being checked at all).
//  Named (not anonymous) so its extern's decorated name is a normal MSVC
//  mangling: an unnamed struct/union/enum tag mangles to the literal text
//  `<unnamed-tag>`, which build_mix.py's gen_data.asm then has to spell as a
//  MASM identifier verbatim - `<`/`>` aren't legal there, so ml.exe fails
//  with "A2008: syntax error : unnamed-tag" on both the PUBLIC and the
//  absolute-value line for this symbol. The struct's own layout is still
//  opaque/placeholder-only; only the tag gained a name.
struct SBnetSocketResolveTable { char reserved[1]; };
extern SBnetSocketResolveTable g_bnetSocketResolveTable;        // stru_6FAD0090
extern ConditionVariable g_bnetSocketSignalPair[2];             // unk_6FAD00B0 (stride 0xC)

extern "C" __declspec(naked) void* __fastcall
ResolveBnetSocket_6F688F30(void* flagsTable, void* edxArg, void* handle,
                            int arg1, int* outStatus, int arg2, int arg3)
{
    __asm { mov eax, 0x6F688F30 }
    __asm { jmp eax }
}

//  The shared "resolve, call the real Winsock-layer primitive, translate
//  the status into a ConditionVariable signal" tail every one of the five
//  functions below repeats verbatim (netproviderbnet_recv.cpp's own dumps
//  for all five - 0x6F68A4B0/0x6F68B380/0x6F68A050/0x6F68A2B0 - are
//  byte-for-byte identical up through the resolve call and the signal
//  tail, differing only in which real primitive runs on the resolved
//  handle and how many of its own arguments get forwarded).  Written
//  once and reused rather than repeated five times: `resultFn` receives
//  the resolved handle and returns the raw eax status this tail signals
//  on, `fallback` is the caller's own default-5 return for a failed
//  resolve.
//  Not static: netproviderbnet_seh.cpp's own SendBnetFramed_6F68A430
//  (0x6F68A430) shares this exact same resolve/signal tail shape too.
int SignalBnetSocketStatus(int status)
{
    if (status != -1)
    {
        int index = (status >= 2) ? 1 : 0;
        g_bnetSocketSignalPair[index].SignalOne(index);
    }
    return status;
}

//  0x6F68A4B0 - forwards the cached word to the +0x6A0 OS handle through
//  the resolved socket's own SetWord-shaped primitive (sub_6F69C190,
//  `this` = resolvedHandle, one stack arg = value, retn 4 confirmed);
//  `__fastcall(ecx=handle, edx=value)`, retn 0 (no stack args of its
//  own - both arrive in registers).
extern "C" __declspec(naked) void __fastcall
SetWordOnResolvedSocket_6F69C190_thunk(void* resolvedHandle, void* /*unused_edx*/, unsigned short value)
{
    __asm { mov eax, 0x6F69C190 }
    __asm { jmp eax }
}

void __fastcall NetProviderBnetHandleSetWord(void* handle, unsigned short value)
{
    int status = -1;
    void* resolved = ResolveBnetSocket_6F688F30(&g_bnetSocketResolveTable,
        (void*)(unsigned int)value, handle, 0, &status, 0, 1);
    if (resolved != 0)
    {
        SetWordOnResolvedSocket_6F69C190_thunk(resolved, 0, value);
        SignalBnetSocketStatus(status);
    }
}

//  0x6F68B380 - same resolve/signal shape as NetProviderBnetHandleSetWord
//  above, forwarding through sub_6F691700 (`this` = resolvedHandle, one
//  stack arg = outPtr, retn 4 confirmed) instead of sub_6F69C190.
//  `__fastcall(ecx=handle, edx=outPtr)`, retn 0.  Shared by slots 24 and
//  32.
extern "C" __declspec(naked) void __fastcall
TouchResolvedSocket_6F691700_thunk(void* resolvedHandle, void* /*unused_edx*/, void* outPtr)
{
    __asm { mov eax, 0x6F691700 }
    __asm { jmp eax }
}

void __fastcall NetProviderBnetHandleTouch(void* handle, void* outPtr)
{
    int status = -1;
    void* resolved = ResolveBnetSocket_6F688F30(&g_bnetSocketResolveTable,
        outPtr, handle, 0, &status, 0, 1);
    if (resolved != 0)
    {
        TouchResolvedSocket_6F691700_thunk(resolved, 0, outPtr);
        SignalBnetSocketStatus(status);
    }
}

//  0x6F68A050 - the real three-argument receive-header pipeline behind
//  slot 24: resolve, then forward (arg0, payload, arg8) through
//  sub_6F6A9AC0 (`this` = resolvedHandle, three stack args, retn 0Ch
//  confirmed) and signal; a failed resolve returns the fixed fallback
//  code 5 instead (the same `edi = 5` default every one of these five
//  real bodies shares).  `__fastcall(ecx=handle, edx=arg0)` plus two
//  stack args, retn 8.
extern "C" __declspec(naked) int __fastcall
RecvHeaderOnResolvedSocket3_6F6A9AC0_thunk(void* resolvedHandle, void* arg0, void* payload, int arg8)
{
    __asm { mov eax, 0x6F6A9AC0 }
    __asm { jmp eax }
}

int __fastcall NetProviderBnetRecvHeader3(void* handle, void* arg0, void* payload, int arg8)
{
    int status = -1;
    void* resolved = ResolveBnetSocket_6F688F30(&g_bnetSocketResolveTable,
        arg0, handle, 0, &status, 0, 1);
    if (resolved == 0)
        return 5;

    int result = RecvHeaderOnResolvedSocket3_6F6A9AC0_thunk(resolved, arg0, payload, arg8);
    SignalBnetSocketStatus(status);
    return result;
}

//  0x6F68A2B0 - the five-argument receive-header pipeline behind slot 32,
//  same shape as NetProviderBnetRecvHeader3 above but forwarding
//  (arg0, arg4, payload, argC, arg10) through sub_6F6A9CE0 (`this` =
//  resolvedHandle, five stack args, retn 14h confirmed).
//  `__fastcall(ecx=handle, edx=arg0)` plus four stack args, retn 10h.
extern "C" __declspec(naked) int __fastcall
RecvHeaderOnResolvedSocket5_6F6A9CE0_thunk(void* resolvedHandle, void* arg0, int arg4, void* payload, int argC, int arg10)
{
    __asm { mov eax, 0x6F6A9CE0 }
    __asm { jmp eax }
}

int __fastcall NetProviderBnetRecvHeader5(void* handle, void* arg0, int arg4, void* payload, int argC, int arg10)
{
    int status = -1;
    void* resolved = ResolveBnetSocket_6F688F30(&g_bnetSocketResolveTable,
        arg0, handle, 0, &status, 0, 1);
    if (resolved == 0)
        return 5;

    int result = RecvHeaderOnResolvedSocket5_6F6A9CE0_thunk(resolved, arg0, arg4, payload, argC, arg10);
    SignalBnetSocketStatus(status);
    return result;
}

//  0x6F65AC40 - the on-failure path both slot 24 and slot 32 share;
//  `this` in ecx, two stack ints, `retn 8`. Thunked (out of scope - a
//  further, un-investigated diagnostic/teardown helper, not part of this
//  batch's own resolve/signal shape).
class NetProviderBnetRecvFail
{
public:
    void Fail(int arg0, int status);
};
__declspec(naked) void NetProviderBnetRecvFail::Fail(int, int)
{
    __asm { mov eax, 0x6F65AC40 }
    __asm { jmp eax }
}

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

namespace Net
{

class NetProviderBNET
{
public:
    //  0x6F65E550 - vtable slot 22.  Clamp `*inOutSize` to this object's
    //  own +0x6A4 field, write it back, then copy that many bytes out of
    //  +0x6A8 into `dest`.
    void Method_22(void* dest, unsigned int* inOutSize);

    //  0x6F65DFB0 - vtable slot 23.  Cache the word and forward it to the
    //  +0x6A0 OS handle.
    void Method_23(unsigned short newValue);

    //  0x6F65F980 - vtable slot 24.  Receive one header/payload pair
    //  through the +0x6A0 OS handle; always returns 1.
    int Method_24(void* arg0, void* buf, int arg8);

    //  0x6F65DFD0 - vtable slot 25.  A real `jmp sub_6F6570A0` tail call
    //  after forcing the third argument to 0 - not a wrapper that discards
    //  the base's own return value, a literal reuse of it (batch-K-
    //  netprovider-misc fixed Net::NetProvider::Method_25's own return
    //  type from `void` to the real `int` it always returns; this override
    //  has to follow suit or the two declarations mangle to different
    //  symbols and extract_gen.py can no longer tell which one is which).
    int Method_25(void* arg0, void* arg4, int arg8);

    //  0x6F65EE50 - vtable slot 32.  Same shape as Method_24 with two
    //  extra forwarded arguments; always returns 1.
    int Method_32(void* arg0, int arg4, void* buf, int argC, int arg10);
};

void NetProviderBNET::Method_22(void* dest, unsigned int* inOutSize)
{
    unsigned int maxSize = *(unsigned int*)((char*)this + 0x6A4);
    unsigned int size = *inOutSize;
    if (size >= maxSize)
        size = maxSize;
    *inOutSize = size;
    memcpy(dest, (char*)this + 0x6A8, size);
}

void NetProviderBNET::Method_23(unsigned short newValue)
{
    ((Net::NetProvider*)this)->SetCachedWord(newValue);
    void* handle = *(void**)((char*)this + 0x6A0);
    NetProviderBnetHandleSetWord(handle, newValue);
}

int NetProviderBNET::Method_24(void* arg0, void* buf, int arg8)
{
    typedef void (__thiscall *VtableA4Fn)(void*, int, int);
    VtableA4Fn fn = (VtableA4Fn)(*(void***)this)[0xA4 / 4];
    fn(this, 0, 2);

    void* handle = *(void**)((char*)this + 0x6A0);
    NetProviderBnetHandleTouch(handle, (char*)this + 0x6A4);

    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));

    handle = *(void**)((char*)this + 0x6A0);
    char* payload = (char*)buf + 0x10;
    int status = NetProviderBnetRecvHeader3(handle, arg0, payload, arg8);
    if (status == 1)
    {
        *(int*)((char*)this + 0x6E8) = 6;
        *(int*)((char*)this + 0x7B4) = ((int*)buf)[0];
        *(int*)((char*)this + 0x7B8) = ((int*)buf)[1];
        *(int*)((char*)this + 0x7BC) = ((int*)buf)[2];
        *(int*)((char*)this + 0x7C0) = ((int*)buf)[3];
        unsigned char length = *(unsigned char*)payload;
        *(unsigned char*)((char*)this + 0x7C4) = length;
        memcpy((char*)this + 0x7C5, payload + 1, length);
    }
    else
    {
        ((NetProviderBnetRecvFail*)this)->Fail(0, status);
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));
    return 1;
}

int NetProviderBNET::Method_25(void* arg0, void* arg4, int)
{
    return ((Net::NetProvider*)this)->Method_25(arg0, arg4, 0);
}

int NetProviderBNET::Method_32(void* arg0, int arg4, void* buf, int argC, int arg10)
{
    void* handle = *(void**)((char*)this + 0x6A0);
    NetProviderBnetHandleTouch(handle, (char*)this + 0x6A4);

    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));

    handle = *(void**)((char*)this + 0x6A0);
    char* payload = (char*)buf + 0x10;
    int status = NetProviderBnetRecvHeader5(handle, arg0, arg4, payload, argC, arg10);
    if (status == 1)
    {
        *(int*)((char*)this + 0x6E8) = 6;
        *(int*)((char*)this + 0x7B4) = ((int*)buf)[0];
        *(int*)((char*)this + 0x7B8) = ((int*)buf)[1];
        *(int*)((char*)this + 0x7BC) = ((int*)buf)[2];
        *(int*)((char*)this + 0x7C0) = ((int*)buf)[3];
        unsigned char length = *(unsigned char*)payload;
        *(unsigned char*)((char*)this + 0x7C4) = length;
        memcpy((char*)this + 0x7C5, payload + 1, length);
    }
    else
    {
        ((NetProviderBnetRecvFail*)this)->Fail(0, status);
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));
    return 1;
}

}  // namespace Net
