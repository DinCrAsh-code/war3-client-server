#include <windows.h>
//============================================================================
//  Net::NetProviderBNET - vtable slot 19, 0x6F65DF30.  Under the +0x6D0
//  lock: if a session-status field (+0x6E8) says 4 and the session id at
//  +0x7A8 matches the caller's own, tell the +0x6F0 descriptor block
//  through sub_6F65B870 ("code 7") and clear the status; separately (not
//  an `else`), if the session id still matches and a flag bit isn't set,
//  forward through the +0x6A0 OS handle and clear the session id.
//
//  Slots 17 (0x6F660530) and 18 (0x6F65E420) share this file's address
//  neighbourhood and touch the same fields, but each builds a CDataStore
//  on the stack (`??_7CDataStore@@6B@`) and forwards through several
//  further OS-layer helpers this session did not trace far enough to
//  reconstruct with confidence - both stay whole-function thunks to their
//  real, unhooked bodies rather than risk a wrong ABI on a guess.
//============================================================================

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  0x6F65B870 - Forward.  Real body in netproviderbnet_descriptorforward.cpp
//  (its own TU, address neighbourhood 0x6F65B8xx rather than this file's own
//  0x6F65Dxxx/0x6F65Exxx cluster - the "one TU per original module" rule)
//  so Method_19 below keeps a real, un-inlined `call` into it rather than
//  /Ob2 folding the 0xB8-byte copy straight into Method_19's own frame.
//  `this` in ecx, two stack args (`retn 8`) - `code` is arg_0 (stored as a
//  plain value, never dereferenced) and `dataPtr` is arg_4 (the one
//  `rep movsd` actually reads from), the reverse of a prior session's
//  declaration order.
class NetProviderBnetDescriptorForward
{
public:
    void Forward(int code, void* dataPtr);
};

//  netproviderbnet_recv.cpp's own shared BNET-socket-resolve wrapper -
//  ResolveBnetSocket_6F688F30/g_bnetSocketResolveTable/
//  g_bnetSocketSignalPair, all declared there (see that file's own header
//  comment for the batch-K-netprovider-misc ABI fix - the real callee
//  takes `handle` as a fifth *stack* argument, not folded into `edx`, and
//  its out-status pointer at the third stack position).  0x6F68A5A0 uses
//  the exact same resolve-forward-signal shape those four functions do;
//  `edx` here is provably dead inside the callee and Notify() below takes
//  no second parameter of its own to thread through, so 0 stands in.
extern "C" void* __fastcall
ResolveBnetSocket_6F688F30(void* flagsTable, void* edxArg, void* handle,
                            int arg1, int* outStatus, int arg2, int arg3);
struct ConditionVariable
{
    void SignalOne(unsigned int value);
};
//  Named, not anonymous - see netproviderbnet_recv.cpp's own declaration for
//  why (an anonymous tag's real MSVC mangling breaks ml.exe on gen_data.asm).
struct SBnetSocketResolveTable { char reserved[1]; };
extern SBnetSocketResolveTable g_bnetSocketResolveTable;
extern ConditionVariable g_bnetSocketSignalPair[2];

//  0x6F69C200 - the real Notify primitive on the resolved socket; `this`
//  = resolvedHandle, retn 0 (confirmed via `ida_query func_at`+`bytes`, no
//  dump body in this call tree) - a further, out-of-scope OS-socket leaf,
//  same class as netproviderbnet_recv.cpp's own sub_6F69C190/sub_6F691700/
//  sub_6F6A9AC0/sub_6F6A9CE0.
extern "C" __declspec(naked) int __fastcall
NotifyOnResolvedSocket_6F69C200_thunk(void* resolvedHandle)
{
    __asm { mov eax, 0x6F69C200 }
    __asm { jmp eax }
}

//  0x6F68A5A0 - `this` in ecx (the +0x6A0 OS handle value), `retn 0`.
//  Same resolve/forward/signal shape as netproviderbnet_recv.cpp's own
//  four functions: resolve through g_bnetSocketResolveTable, forward
//  through NotifyOnResolvedSocket_6F69C200_thunk on success (the fixed
//  fallback 5 on a failed resolve, same default every one of that file's
//  bodies shares), then signal g_bnetSocketSignalPair keyed off the
//  resolve's own status.
class NetProviderBnetHandleNotify
{
public:
    int Notify();
};
int NetProviderBnetHandleNotify::Notify()
{
    int status = -1;
    void* resolved = ResolveBnetSocket_6F688F30(&g_bnetSocketResolveTable, 0, this, 0, &status, 0, 1);
    int result = 5;
    if (resolved != 0)
    {
        result = NotifyOnResolvedSocket_6F69C200_thunk(resolved);
        if (status != -1)
        {
            int index = (status >= 2) ? 1 : 0;
            g_bnetSocketSignalPair[index].SignalOne(index);
        }
    }
    return result;
}

namespace Net
{

class NetProviderBNET
{
public:
    //  0x6F660530 - vtable slot 17.  `retn 1Ch` (seven stack dwords).
    //  Whole-function thunk (see header comment above) - argument *count*
    //  is load-bearing (thunk_abi_audit.py), the plain-int types are not
    //  a claim about real parameter meaning.
    int Method_17(int a0, int a4, int a8, int aC, int a10, int a14, int a18);

    //  0x6F65E420 - vtable slot 18.  `retn 10h` (four stack dwords).
    //  Whole-function thunk.
    void Method_18(int a0, int a4, int a8, int aC);

    //  0x6F65DF30 - vtable slot 19.
    void Method_19(int arg0, int session, int arg8);
};

__declspec(naked) int NetProviderBNET::Method_17(int, int, int, int, int, int, int)
{
    __asm { mov eax, 0x6F660530 }
    __asm { jmp eax }
}

__declspec(naked) void NetProviderBNET::Method_18(int, int, int, int)
{
    __asm { mov eax, 0x6F65E420 }
    __asm { jmp eax }
}

void NetProviderBNET::Method_19(int arg0, int session, int arg8)
{
    (void)arg0;
    (void)arg8;

    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));

    if (*(int*)((char*)this + 0x6E8) == 4)
    {
        if (*(int*)((char*)this + 0x7A8) == session)
        {
            ((NetProviderBnetDescriptorForward*)this)->Forward(7, (char*)this + 0x6F0);
            *(int*)((char*)this + 0x6E8) = 0;
        }
    }

    if (*(int*)((char*)this + 0x7A8) == session)
    {
        if ((*(int*)((char*)this + 0x7A4) & 0x1000) == 0)
        {
            void* handle = *(void**)((char*)this + 0x6A0);
            ((NetProviderBnetHandleNotify*)handle)->Notify();
        }
        *(int*)((char*)this + 0x7A8) = 0;
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));
}

}  // namespace Net
