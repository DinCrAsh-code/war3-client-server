//============================================================================
//  NetSessionClearTagAndNotify (0x6F53E610) - if this thread's own
//  `SNetSessionInfo` record exists, notify the (unidentified) per-thread
//  net provider below and reset the session tag pair back to "NONE"/0.
//  Zero arguments, `retn 0` - the one caller (TeardownNetSession,
//  netdata_sessionteardown.cpp) never sets up `ecx`/`edx` before the call.
//
//  Its own translation unit, separate from its one caller, for the same
//  cross-TU-inlining reason netdata_teardowndataslot.cpp's own header
//  explains.
//
//  `+0x228C` (the dword immediately past `m_sessionTag`, netdata.h) has no
//  established purpose beyond "the field this closure zeroes alongside the
//  tag" - CLAUDE.md's "only touched members get names" rule for a field
//  nothing else in this repo's closures reads, so it stays a raw offset
//  rather than a new named member.
//============================================================================
#include "netdata.h"
#include "gamecontext.h"

//============================================================================
//  0x6F6533B0 - a zero-argument per-thread "net" provider notify/release
//  step (thread-local slot 0x0E, a vtable dispatch through it, an
//  EventRegistry-style stripe publish and a `ConditionVariable::SignalOne`)
//  called from NetSessionClearTagAndNotify below.  Left a naked redirect:
//  its receiver's class is not established anywhere in this repo (nothing
//  names what lives in TLS slot 0x0E), so the two indirect calls in it
//  (`[eax+8]` off `[esi]`, then `[edx]` off `[esi]` again) would be guesses
//  at a vtable this session has no committed shape for - exactly the
//  "opens onto a subsystem with nothing solid to reconstruct from" case
//  CLAUDE.md's own loop says to leave documented rather than chase.
//============================================================================
#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig             \
    {                                 \
        __asm { mov eax, addr }       \
        __asm { jmp eax }             \
    }

ADDR_THUNK(void NetProviderNotifyRelease_6F6533B0(), 0x6F6533B0)

#undef ADDR_THUNK

void NetSessionClearTagAndNotify()
{
    SNetSessionInfo* records = (SNetSessionInfo*)NetDataRecords();

    if (records != 0)
    {
        NetProviderNotifyRelease_6F6533B0();
        records->m_sessionTag = kNetSessionTagNone;
        *(int*)((char*)records + 0x228C) = 0;
    }
}
