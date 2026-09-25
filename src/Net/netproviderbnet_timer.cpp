//============================================================================
//  Net::NetProviderBNET - vtable slots 4 (0x6F65E3E0) and 5 (0x6F65DDD0).
//  A lazily-registered periodic callback: slot 4 registers it on first use
//  (and forwards on every call), slot 5 unregisters and clears it.
//============================================================================
#include <windows.h>
#include "conditionvariable.h"

//  0x6F65AB30 - SendStatusTarget::PostSendStatus, already reconstructed
//  in src/Net/netproviderltcp_send.cpp (a thunk there too, `this` in ecx).
class SendStatusTarget
{
public:
    void PostSendStatus(int code);
};

//  0x6F4C34D0 - GetThreadLocalSlot(0), already reconstructed
//  (Game/gamecontext.h).
void* __fastcall GetThreadLocalSlot(unsigned int slot);

//  0x6F62AF10 - CEventRegistry::AcquireBucket, already reconstructed
//  (Frame/frame.h).  g_eventRegistry is stru_6FACEA68, the same object
//  both this and CEventRegistry's own call sites reach.
class CEventRegistry
{
public:
    void* AcquireBucket(void* key, int exclusive, int* outLock, int a, int b);
};
extern CEventRegistry g_eventRegistry;   // stru_6FACEA68 (Frame/frame.h)

//  unk_6FACEA88 - the same eight stripes as g_eventRegistry's own, reached
//  as a global rather than through the registry (Frame/frame.h's own note).
extern char g_eventRegistryLocks[8][12];

//  0x6F6331D0 - registers the periodic callback proper against the bucket
//  AcquireBucket found: `__fastcall(ecx = bucket, edx = the NetProviderBNET
//  object)`, six further stack dwords (four zero placeholders, the callback
//  pointer, the interval float) - a deep timer-subsystem primitive this
//  session did not trace; `retn 18h` read off the shipped stream's own
//  epilogue.  Naked - the real stream's own register/stack shuffle is
//  reproduced directly below rather than re-derived through a C++ call,
//  since none of its six stack arguments' real meaning is established here.
extern "C" __declspec(naked) int __fastcall
RegisterPeriodicCallback(void* bucket, void* netProviderThis,
                          int a0, int a4, int a8, int aC,
                          void* callback, float interval)
{
    __asm { mov eax, 0x6F6331D0 }
    __asm { jmp eax }
}

//  0x6F62B510 - registers a periodic callback: AcquireBucket the stripe for
//  this thread's slot, then RegisterPeriodicCallback if a bucket came back;
//  on failure returns 0 without registering.  `__fastcall(ecx = callback
//  address, edx = 0)`, one stack float, `retn 4`.
__declspec(naked) int __fastcall
NetProviderBnetRegisterTimer(void* callback, int edxZero, float interval)
{
    __asm
    {
        push    ecx
        push    ebx
        push    esi
        push    edi
        mov     edi, ecx
        xor     ecx, ecx
        mov     esi, edx
        xor     ebx, ebx
        call    GetThreadLocalSlot
        push    1
        push    ebx
        lea     ecx, [esp+18h]
        push    ecx
        push    ebx
        push    eax
        mov     ecx, offset g_eventRegistry
        call    CEventRegistry::AcquireBucket
        test    eax, eax
        jz      short no_bucket
        fld     dword ptr [esp+10h]
        push    ebx
        push    ebx
        push    ebx
        push    ebx
        push    esi
        push    ecx
        mov     edx, edi
        fstp    dword ptr [esp]
        mov     ecx, eax
        call    RegisterPeriodicCallback
        mov     esi, eax
        mov     eax, [esp+10h]
        cmp     eax, 0FFFFFFFFh
        jz      short done
        cmp     eax, 8
        sbb     edx, edx
        and     eax, 7
        add     edx, 1
        lea     ecx, [eax+eax*2]
        push    edx
        lea     ecx, DWORD PTR g_eventRegistryLocks[ecx*4]
        call    ConditionVariable::SignalOne
done:
        pop     edi
        mov     eax, esi
        pop     esi
        pop     ebx
        pop     ecx
        retn    4
no_bucket:
        pop     edi
        pop     esi
        mov     eax, ebx
        pop     ebx
        pop     ecx
        retn    4
    }
}

void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  0x6F656BF0 - fetch the two out-values (a handle-ish dword, a status
//  dword) the pending-notify check needs, guarded by the object's own
//  critical section at +0x5F0: if the pending-record pointer at +0x61C is
//  non-NULL, copy its own +0x2C field into `outB` and its +0x28 field into
//  `outA` (real field-write order confirmed off the dump - `outB` is
//  written first, from `arg_4`).  `this` in ecx, two out-pointer stack
//  args, `retn 8`.  netremainder2 session (batch-K closure pass): its own
//  callees (EnterCritSec/LeaveCritSec) turned out already reconstructed
//  (Net/netclient_recvappend.cpp and others), so the "deep, out of scope"
//  call here is now a real reconstruction; the pending-record's own field
//  layout past +0x28/+0x2C stays opaque (no further reader in this file's
//  own call tree touches it).
class NetProviderBnetPendingNotifyQuery
{
public:
    int QueryPending(int* outA, int* outB);
};
int NetProviderBnetPendingNotifyQuery::QueryPending(int* outA, int* outB)
{
    CRITICAL_SECTION* cs = (CRITICAL_SECTION*)((char*)this + 0x5F0);
    EnterCritSec(cs);

    void* rec = *(void**)((char*)this + 0x61C);
    int has = (rec != 0);
    if (has)
    {
        *outB = *(int*)((char*)rec + 0x2C);
        *outA = *(int*)((char*)rec + 0x28);
    }

    LeaveCritSec(cs);
    return has;
}

//  0x6F68A430 - post the notification through the +0x6A0 OS handle once
//  QueryPending says there is one; `ecx` = handle, `edx` = QueryPending's
//  second out-value, five stack dwords, `retn 14h`.  Deep, out of scope -
//  naked redirect (argument count only, off the real call site's own push
//  count; real meaning not established).
extern "C" __declspec(naked) void __fastcall
PostPendingNotify(void* handle, int outB, int a, int b, int c, int code,
                   int firstValue)
{
    __asm { mov eax, 0x6F68A430 }
    __asm { jmp eax }
}

//  0x6F65E150 - slot 4's own tail-forward once the timer is (or already
//  was) registered: if a pending-notify query says there is one, post it
//  through the +0x6A0 OS handle.  `this` in ecx, `retn 0`.  Written naked
//  (matching this file's own transcription of RegisterTimer above) since
//  QueryPending/PostPendingNotify's own out-value shapes are not
//  established from this call tree alone.
class NetProviderBnetTimerForward
{
public:
    void Forward();
};
__declspec(naked) void NetProviderBnetTimerForward::Forward()
{
    __asm
    {
        sub     esp, 0Ch
        push    esi
        mov     esi, ecx
        cmp     dword ptr [esi+7D8h], 0
        push    edi
        mov     edi, 1
        jz      short no_pending
        lea     eax, [esp+8]
        push    eax
        lea     ecx, [esp+8]
        push    ecx
        mov     ecx, esi
        call    NetProviderBnetPendingNotifyQuery::QueryPending
        test    eax, eax
        jz      short done
        mov     eax, [esp+8]
        mov     ecx, [esi+6A0h]
        lea     edx, [esp+10h]
        push    edx
        mov     edx, [esp+8]
        push    0
        push    0
        push    14h
        push    eax
        call    PostPendingNotify
done:
        mov     eax, edi
        pop     edi
        pop     esi
        add     esp, 0Ch
        retn
no_pending:
        pop     edi
        xor     eax, eax
        pop     esi
        add     esp, 0Ch
        retn
    }
}

//  0x6F632FD0 - the unregister-side counterpart to RegisterPeriodicCallback:
//  `__fastcall(ecx = bucket, edx = the NetProviderBNET object)`, no stack
//  args, `retn 0` (tail-jumps into LeaveCritSec, which is itself `retn 0`).
extern "C" __declspec(naked) void __fastcall
UnregisterPeriodicCallback(void* bucket, void* netProviderThis)
{
    __asm { mov eax, 0x6F632FD0 }
    __asm { jmp eax }
}

//  0x6F62B710 - slot 5's own unregister call: AcquireBucket the stripe for
//  this thread's slot, then UnregisterPeriodicCallback and, on the same
//  out-of-range status the register path checks, SignalOne the matching
//  stripe lock.  `this` in ecx, `retn 0`.
class NetProviderBnetUnregisterTimer
{
public:
    void Unregister();
};
__declspec(naked) void NetProviderBnetUnregisterTimer::Unregister()
{
    __asm
    {
        push    ecx
        push    esi
        mov     esi, ecx
        xor     ecx, ecx
        call    GetThreadLocalSlot
        push    1
        push    0
        lea     ecx, [esp+10h]
        push    ecx
        push    0
        push    eax
        mov     ecx, offset g_eventRegistry
        call    CEventRegistry::AcquireBucket
        test    eax, eax
        jz      short done
        mov     edx, esi
        mov     ecx, eax
        call    UnregisterPeriodicCallback
        mov     eax, [esp+8]
        cmp     eax, 0FFFFFFFFh
        jz      short done
        cmp     eax, 8
        sbb     edx, edx
        and     eax, 7
        add     edx, 1
        lea     ecx, [eax+eax*2]
        push    edx
        lea     ecx, DWORD PTR g_eventRegistryLocks[ecx*4]
        call    ConditionVariable::SignalOne
done:
        pop     esi
        pop     ecx
        retn
    }
}

//  0x6F65CD70/0x6F65AB50 - PostStatusTarget::ResetRecycleState/
//  PostProviderStatusDword, already reconstructed in
//  src/Misc/unreconstructed_thunks.cpp.
class PostStatusTarget
{
public:
    void ResetRecycleState();
    void PostProviderStatusDword(int);
};

//  flt_6F9711C0 - a float constant this call tree only ever reads, never
//  computes; funcmap.py DATA row `NetProviderBnetTimerInterval`.
extern const float g_netProviderBnetTimerInterval;   // flt_6F9711C0

namespace Net
{

class NetProviderBNET
{
public:
    //  0x6F65E3E0 - vtable slot 4.  `retn 0`.
    void Method_4();

    //  0x6F65DDD0 - vtable slot 5.  `retn 0`.
    void Method_5();
};

void NetProviderBNET::Method_4()
{
    if (*(int*)((char*)this + 0x7D8) == 0)
    {
        ((SendStatusTarget*)this)->PostSendStatus(1);
        //  0x6F65E1B0 - the periodic-timer callback, taken by address
        //  only (never called from this call tree).
        float interval = g_netProviderBnetTimerInterval;
        int handle = NetProviderBnetRegisterTimer((void*)0x6F65E1B0, 0, interval);
        *(int*)((char*)this + 0x7D8) = handle;
    }
    ((NetProviderBnetTimerForward*)this)->Forward();
}

void NetProviderBNET::Method_5()
{
    if (*(int*)((char*)this + 0x7D8) != 0)
    {
        ((NetProviderBnetUnregisterTimer*)this)->Unregister();
        *(int*)((char*)this + 0x7D8) = 0;
        ((PostStatusTarget*)this)->ResetRecycleState();
        ((PostStatusTarget*)this)->PostProviderStatusDword(1);
    }
}

}  // namespace Net
