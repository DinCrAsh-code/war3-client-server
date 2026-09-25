//============================================================================
//  Net::NetProviderLTCP - vtable slots 17 (0x6F663A40, RegisterNotifyTarget),
//  18 (0x6F6633F0, UpdateNotifyTarget) and 19 (0x6F663480,
//  UnregisterNotifyTarget) - one address neighbourhood, the same module,
//  all three walking the intrusive `LocalGameAd` list rooted at
//  `m_notifyList.m_terminator` (+0x6B0/+0x6B4, netproviderltcp.h) and, if the
//  listener thread is up (+0x69C), forwarding the same change into the
//  real notify-subsystem pipeline.
//
//  RegisterNotifyTarget's own list-splice reads as `m_notifyList.m_terminator.
//  Unlink()` (storm.h's own `TSLink<T>::Unlink()`, instruction for
//  instruction: the `if (m_next)` guard, the `m_prevlink <= 0` sign test
//  recovering the terminator's own predecessor either directly or via the
//  link-offset trick, and the final `m_next = m_prevlink = 0`) followed by
//  re-deriving the (possibly just-cleared) head and splicing the fresh node
//  in ahead of it - written out explicitly here rather than through
//  `TSList<T,0>::LinkToHead()` because the shipped instructions operate on
//  the terminator's own two fields the whole time, never on the new node's
//  own `TSLink` the way `LinkToHead()`'s own inlined body would.
//============================================================================
#include "netproviderltcp.h"

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  Storm.dll ordinal 401/403 - already declared this shape elsewhere in
//  this directory (netproviderloop_bindport.cpp, netproviderltcp_deleteself.cpp).
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);
void  __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

//  This class's own Storm allocation tag - the class's RTTI-derived mangled
//  name, per docs/msvc-vc8-idioms.md's "The Storm allocation tag names a
//  template argument you already have a class for" (here: a plain nested
//  struct, not a template).
static const char* const kLocalGameAdTag = ".?AULocalGameAd@NetProviderLTCP@@";

//  A plain 46-dword (0xB8-byte) POD block, used only to get this build's
//  own struct-assignment codegen to `rep movsd` for RegisterNotifyTarget's
//  own descriptor copy - see the call site's own comment.
struct DescriptorBlock { unsigned int dwords[0x2E]; };

//  0x6F6C5550 - RegisterNotifyTarget's own direct callee, stashed into the
//  fresh node's +0xD0.  Not itself a vtable slot and never otherwise
//  reached by this session's sixteen slots; a thunk to the real, unhooked
//  body.
__declspec(naked) void* __fastcall AcquireNotifyHandle()
{
    __asm { mov eax, 0x6F6C5550 }
    __asm { jmp eax }
}

//  0x6F6629C0/0x6F6628A0/0x6F662AE0 - RegisterNotifyTarget's/
//  UpdateNotifyTarget's/UnregisterNotifyTarget's own "tell the listener
//  thread about this change" forward, each reached only once the listener
//  handle (+0x69C) is live.  Genuinely below this session's sixteen vtable
//  slots (the same class of deep listener-thread machinery
//  netproviderltcp_route.cpp's own RouteDeliver thunk already documents) -
//  thunks to the real, unhooked bodies.
struct NotifyChangeArgs { void* a; void* b; void* c; };
__declspec(naked) int __fastcall PostRegisterChange(void* listenHandle, NotifyChangeArgs* args, int zero)
{
    __asm { mov eax, 0x6F6629C0 }
    __asm { jmp eax }
}
__declspec(naked) int __fastcall PostUpdateChange(void* listenHandle, NotifyChangeArgs* args, int zero)
{
    __asm { mov eax, 0x6F6628A0 }
    __asm { jmp eax }
}
__declspec(naked) int __fastcall PostUnregisterChange(void* listenHandle, void** target, int zero)
{
    __asm { mov eax, 0x6F662AE0 }
    __asm { jmp eax }
}

//  0x6F663A20/0x6F6636B0 - two callback addresses RegisterNotifyTarget
//  hands to the timer subsystem below, purely as data (their addresses are
//  taken; neither is ever called from here).  Bodies unreconstructed and
//  out of this session's scope - a thunk to each real, unhooked function
//  purely so this call site's `push offset` has a real symbol to name.
__declspec(naked) void NotifyTimerTick_6F663A20()
{
    __asm { mov eax, 0x6F663A20 }
    __asm { jmp eax }
}
__declspec(naked) void NotifyTimerSecondCallback_6F6636B0()
{
    __asm { mov eax, 0x6F6636B0 }
    __asm { jmp eax }
}

//  0x6F62ABF0 - RegisterNotifyTarget's own one-shot periodic-timer
//  registration, guarded by +0x6B8 so it only ever runs once per object.
//  The whole body is one instruction - a tail jump into sub_6F630760, a
//  genuinely deep timer-subsystem primitive well below this session's
//  sixteen vtable slots - reproduced directly rather than guessed at.
extern "C" __declspec(naked) void* __fastcall
RegisterTimerImpl(int zero, void* tickCallback, unsigned int intervalMs, void* secondCallback, int flags)
{
    __asm { mov eax, 0x6F630760 }
    __asm { jmp eax }
}
__declspec(naked) void* __fastcall RegisterNotifyTimer(int zero, void* tickCallback, unsigned int intervalMs, void* secondCallback, int flags)
{
    __asm { jmp RegisterTimerImpl }
}

//  critsec.cpp - already reconstructed.
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  0x6F4C34D0 - GetThreadLocalSlot, already reconstructed (Game/gamecontext.h).
void* __fastcall GetThreadLocalSlot(unsigned int slot);

//  0x6F62AF10 - CEventRegistry::AcquireBucket, already reconstructed
//  (Frame/frame.h).  g_eventRegistry/g_eventRegistryLocks are
//  stru_6FACEA68/unk_6FACEA88, the same pair netproviderbnet_timer.cpp's
//  own RegisterTimer/UnregisterTimer reach for the sibling timer wheel.
class CEventRegistry
{
public:
    void* AcquireBucket(void* key, int exclusive, int* outLock, int a, int b);
};
extern CEventRegistry g_eventRegistry;
extern char g_eventRegistryLocks[8][12];

//  0x6F6D8950 - ConditionVariable::SignalOne, already reconstructed
//  (gameaction0x26_notify.cpp).
struct ConditionVariable
{
    void SignalOne(unsigned int value);
};

//  0x6F631FD0 - posts the message proper through the bucket AcquireBucket
//  found: `this` = bucket, two stack args (`src`, `size`), `retn 8` read
//  off the real call site's own push count.  Deep timer-subsystem
//  machinery well below this session's scope - naked redirect, argument
//  count only.
class TimerMessagePoster
{
public:
    void PostTimerMessagePayload(void* src, unsigned int size);
};
__declspec(naked) void TimerMessagePoster::PostTimerMessagePayload(void*, unsigned int)
{
    __asm { mov eax, 0x6F631FD0 }
    __asm { jmp eax }
}

//  0x6F62B250 - stashes the registered timer handle (`eax` from
//  RegisterNotifyTimer above) into +0x6B8 and posts a small message
//  through it: `__fastcall(ecx=timerHandle, edx=messageType)` plus two
//  stack args (`src`, `size`), `retn 8`.  If `timerHandle` is null, first
//  falls back through GetThreadLocalSlot(0) the same way the sibling BNET
//  timer wheel's own RegisterTimer does; AcquireBucket the stripe, then
//  (unless the bucket's own +0x2C state is already 2) post the message and
//  SignalOne the paired lock stripe on an out-of-range status.
__declspec(naked) void __fastcall PostNotifyTimerMessage(void* timerHandle, unsigned int messageType, void* src, unsigned int size)
{
    __asm
    {
        sub     esp, 8
        push    ebx
        mov     eax, ecx
        xor     ebx, ebx
        test    eax, eax
        push    esi
        mov     [esp+10h], edx
        jnz     short have_handle
        call    GetThreadLocalSlot
have_handle:
        push    1
        push    0
        lea     ecx, [esp+18h]
        push    ecx
        push    0
        push    eax
        mov     ecx, offset g_eventRegistry
        call    CEventRegistry::AcquireBucket
        mov     esi, eax
        test    esi, esi
        jz      short done
        push    ebp
        push    edi
        lea     edi, [esi+10h]
        mov     ecx, edi
        call    EnterCritSec
        xor     edx, edx
        cmp     dword ptr [esi+2Ch], 2
        mov     ecx, edi
        setz    dl
        mov     ebp, edx
        call    LeaveCritSec
        pop     edi
        test    ebp, ebp
        pop     ebp
        jnz     short skip_post
        mov     eax, [esp+10h]
        mov     ecx, [esp+10h]
        mov     edx, [esp+10h]
        push    eax
        push    ecx
        mov     ecx, esi
        call    TimerMessagePoster::PostTimerMessagePayload
        mov     ebx, 1
skip_post:
        mov     eax, [esp+0Ch]
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
        mov     eax, ebx
        pop     ebx
        add     esp, 8
        retn    8
    }
}

namespace Net
{

void NetProviderLTCP::RegisterNotifyTarget(void* /*unusedArg0*/, void* target, void* arg8, void* descriptor,
                                            void* arg10, unsigned int arg14, unsigned short arg18)
{
    EnterCritSec(&m_lock);

    LocalGameAd* node = (LocalGameAd*)SMemAlloc(0xD8, kLocalGameAdTag, -2, 8);
    LocalGameAd* fresh;
    if (node)
    {
        node->m_link.m_next     = 0;
        node->m_link.m_prevlink = 0;
        *((char*)node + 0x10) = 0;
        *((char*)node + 0x30) = 0;
        *((char*)node + 0x40) = 0;
        fresh = node;
    }
    else
    {
        fresh = 0;
    }

    //  See the header note above.
    TSLink<LocalGameAd>* term = &m_notifyList.m_terminator;
    term->Unlink();

    //  Splice `fresh` in immediately ahead of whatever the terminator's own
    //  `m_next` now names.
    TSLink<LocalGameAd>* head = term->m_next;
    term->m_next = head;
    term->m_prevlink = head->m_prevlink;
    head->m_prevlink = (int)fresh;
    m_notifyList.m_terminator.m_next = term;

    fresh->m_target = target;
    *(void**)fresh->m_argCField = arg8;

    //  Bulk-copy a 0xB8-byte descriptor from `descriptor` into fresh+0x10 -
    //  a raw struct assignment rather than `memcpy`, which is what gets
    //  this build to the shipped `rep movsd` rather than a `call memcpy`
    //  (netprovider_assert.cpp's own AssertSlot39 note documents the same
    //  choice for the identical 46-dword copy size).
    *(DescriptorBlock*)fresh->m_reserved010 = *(DescriptorBlock*)descriptor;

    fresh->m_registered = 1;
    fresh->m_reservedCC = *(int*)((char*)descriptor + 0xB0);
    fresh->m_handle = AcquireNotifyHandle();
    fresh->m_reservedD4 = arg18;

    //  These four dwords land inside the just bulk-copied descriptor block
    //  (relative offsets 0x20..0x2C of it) and are zeroed again afterward.
    *(int*)(fresh->m_reserved010 + 0x20) = 0;
    *(int*)(fresh->m_reserved010 + 0x24) = 0;
    *(int*)(fresh->m_reserved010 + 0x28) = 0;
    *(int*)(fresh->m_reserved010 + 0x2C) = 0;

    if (m_listenHandle)
    {
        NotifyChangeArgs args = { arg10, (void*)arg14, target };
        PostRegisterChange(m_listenHandle, &args, 0);

        if (!m_notifyTimerHandle)
        {
            void* timer = RegisterNotifyTimer(0, (void*)&NotifyTimerTick_6F663A20, 0x1388,
                                               (void*)&NotifyTimerSecondCallback_6F6636B0, 0);
            m_notifyTimerHandle = timer;
            unsigned int size = 4;
            PostNotifyTimerMessage(timer, 0x13, &size, size);
        }
    }

    LeaveCritSec(&m_lock);
}

void NetProviderLTCP::UpdateNotifyTarget(void* /*unusedArg0*/, void* target, int arg8, int argC)
{
    EnterCritSec(&m_lock);

    LocalGameAd* node = m_notifyList.Tail();
    while ((int)node > 0)
    {
        if (node->m_target == target)
        {
            node->m_registered = arg8;
            node->m_reservedCC = argC;
            break;
        }
        node = (LocalGameAd*)node->m_link.m_prevlink;
    }

    if (m_listenHandle)
    {
        NotifyChangeArgs args = { target, (void*)(int)arg8, (void*)(int)argC };
        PostUpdateChange(m_listenHandle, &args, 0);
    }

    LeaveCritSec(&m_lock);
}

void NetProviderLTCP::UnregisterNotifyTarget(void* /*unusedArg0*/, void* target, void* /*unusedArg8*/)
{
    EnterCritSec(&m_lock);

    //  `found` starts null and is set only on a match, then the unlink/free
    //  below runs on it unconditionally - the dump has no not-found guard
    //  at all, so an unregister of a target that was never registered walks
    //  off the list with `found` still null and dereferences it. Preserved
    //  exactly rather than "fixed" with an `if`: this is what the shipped
    //  binary does, presumably relying on every real caller only ever
    //  unregistering a target it itself registered.
    LocalGameAd* node = m_notifyList.Tail();
    LocalGameAd* found = 0;
    while ((int)node > 0)
    {
        if (node->m_target == target)
        {
            found = node;
            break;
        }
        node = (LocalGameAd*)node->m_link.m_prevlink;
    }

    found->m_link.Unlink();
    SMemFree(found, kLocalGameAdTag, -2, 0);

    if (m_listenHandle)
        PostUnregisterChange(m_listenHandle, &target, 0);

    LeaveCritSec(&m_lock);
}

}  // namespace Net
