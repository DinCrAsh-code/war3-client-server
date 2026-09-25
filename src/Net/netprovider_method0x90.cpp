//============================================================================
//  NetProvider__Method_0x90 (0x6F65D670, vtable+0x90 / method_index 36 -
//  user_knowledge.json's Net::NetProvider vtable dump, shared unoverridden
//  by NetProviderLTCP/NetProviderBNET/NetProviderLOOP) and its three direct,
//  small callees.
//
//  `this` (esi) reaches into two of its own fields this call tree never
//  otherwise touches: +0x2B4 (an interface pointer whose vtable slot+0x1C
//  is called - the actual work this method exists to do, entirely opaque
//  from this call tree) and +0x2B8/+0x2BC (a lazily-allocated 8-byte
//  {flag(0); Event event(4)} pair - see EventHolder below).
//============================================================================
#include "event.h"

//  sub_6F672A60 - a lazy double-checked-locking style singleton/registration
//  gate (EnterCritSec/LeaveCritSec around a global counter and two calls
//  through `this`'s own vtable+0x28/+0x3C) reached only from
//  CheckOrRegister below. Real body now lives in netprovider_register.cpp -
//  a follow-up session found its own dump turned out small and
//  self-contained, and it is the real install site for dispatcher1's own
//  event callback (docs/targets/Probably_W3GS_ActionHandler.md's "Client
//  vs server, corrected" section).
//  `this` in ecx, a second argument in edx, and one more on the stack - a
//  free `__fastcall` function taking the object explicitly (thiscall does
//  not use edx), not a plain member function.
int __fastcall NetProviderBase_Register(void* self, int edxArg, int stackArg);

//  [outer+0x2B8]/[outer+0x2BC] in Method_0x90 below - an 8-byte heap block
//  allocated with SMemAlloc(8, ".\\NetProvider.cpp", 0x6E6, 0) the first
//  time it is needed and freed with SMemFree(..., "delete", -1, 0) by
//  Cleanup() below - `m_flag` starts zeroed and is never otherwise written
//  in this call tree, so its purpose beyond "non-null once the event
//  exists" is unknown.
struct EventHolder
{
    int    m_flag;    // +0
    Event  m_event;   // +4

    //  sub_6F657620 - wait up to 10s, retrying (and calling the opaque
    //  nullsub_1627 hook between attempts) for as long as the wait keeps
    //  timing out (WAIT_TIMEOUT / STATUS_TIMEOUT == 0x102).
    void WaitRetry();
};

//  0x6F663C70 - a genuine no-op (`retn` with nothing before it) reached
//  only between retries above; not identified beyond that.
void NullHook_6F663C70();

void EventHolder::WaitRetry()
{
    while (m_event.Wait(10000) == 0x102)
        NullHook_6F663C70();
}

//  sub_6F656FE0 - `this`'s own +0x2B8 flag gates a one-time registration:
//  already registered (`*(this+0x2B8) != 0`) fails loudly (SErrSetLastError
//  with ERROR_INVALID_FUNCTION / 0x57, per this codebase's other
//  SErrSetLastError call sites) and returns false; otherwise
//  NetProviderBase::Register(0) is called and the flag is set.
struct NetProviderMethod0x90Self
{
    char             m_reserved000[0x2B8];
    int              m_registered;   // +0x2B8
    EventHolder*     m_eventHolder;  // +0x2BC
    void*            m_iface;        // +0x2B4 (interface whose vtable+0x1C
                                      //  Method_0x90 itself calls - see below)

    int  CheckOrRegister();          // sub_6F656FE0
    void Cleanup();                  // sub_6F65B700
};

//  Storm.dll ordinal 465. Must be `extern "C"` and at global scope: a
//  linkage specification is not allowed inside a function body, and without
//  it the call mangles as C++ and never matches the Storm import.
void __stdcall SErrSetLastError(unsigned int);

int NetProviderMethod0x90Self::CheckOrRegister()
{
    if (m_registered)
    {
        SErrSetLastError(0x57);
        return 0;
    }

    NetProviderBase_Register(this, 0, 0);
    m_registered = 1;
    return 1;
}

//  sub_6F65B700 - tear down the lazily-allocated EventHolder at +0x2BC if
//  one exists: wait on it once more (WaitRetry, above), close the event
//  and free the block, then null the field. Nothing here touches +0x2B8;
//  whatever relationship that flag has to this cleanup is not visible in
//  this call tree.
void NetProviderMethod0x90Self::Cleanup()
{
    EventHolder* holder = m_eventHolder;
    if (holder)
    {
        holder->WaitRetry();
        holder = m_eventHolder;
        if (holder)
        {
            holder->m_event.CloseForward();
            extern void __stdcall SMemFree(void*, const char*, int, unsigned int);
            SMemFree(holder, "delete", -1, 0);
        }
        m_eventHolder = 0;
    }
}
