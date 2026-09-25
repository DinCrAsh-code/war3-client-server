//============================================================================
//  TeardownNetSession (0x6F5539F0) - the net-session teardown root: reset
//  both session-array indices (0 and 1) via NetSessionFullReset_6F54C4F0
//  below, clear the session tag (NetSessionClearTagAndNotify,
//  netdata_clearsessiontag.cpp), unregister the three Battle.net
//  timer/config handlers InitializeNetSessionConfig
//  (netdata_initconfig.cpp) registered, tear down the "net" checksum-
//  provider slot itself (TeardownNetDataSlot(2),
//  netdata_teardowndataslot.cpp) and finally tail-jump into the OS-net
//  shutdown alias (`OsNetShutdownAlias`, Os/osnetaliases.cpp).
//
//  Its three real callees each get their own translation unit rather than
//  living alongside this one, for the cross-TU-inlining reason
//  netdata_teardowndataslot.cpp's own header explains - the first attempt
//  at this batch shared one TU and the two calls in question inlined away
//  entirely, dropping the score to 0.60.
//
//  `this` is read once, from thread-local slot 0x0D's own `+0x10`/`+0x8`
//  chain (the same `SNetSessionInfo*` NetDataRecords() reads, spelled out
//  by hand here rather than through that inline - the dump's own `esi`
//  never carries a `SNetSessionInfo*` type through `NetSessionFullReset_
//  6F54C4F0`, which takes a plain index, not `this`), then reused for both
//  `NetSessionFullReset_6F54C4F0` calls (0 first, then 1).
//
//  The final `mov ecx, 2` right before the tail jump is a real argument,
//  not dead code: `OsNetShutdownAlias` (Os/osnetaliases.cpp) is
//  `__fastcall(unsigned int)`, so the literal 2 is passed straight through
//  as its one parameter.
//============================================================================
#include "netdata.h"
#include "gamecontext.h"

//============================================================================
//  0x6F54C4F0 - CNetData::CNetSession's own SEH-framed full session reset
//  (replay-state teardown, the +0xF38 embedded queue's own drain, the
//  turn-store/event-dispatch machinery below it).  Left a naked redirect
//  deliberately: its own callees include the +0xF38 event-queue cluster and
//  the CNetEvent/turn-store family other concurrent sessions in this same
//  neighbourhood are working (CLAUDE.md's own scope note for this batch) -
//  reconstructing it here would be exactly the kind of "wanders into
//  unrelated subsystem" CLAUDE.md's loop says to leave as a documented
//  TODO/thunk instead.  TeardownNetSession below calls it twice (once per
//  index) purely to tear down the session state that survives
//  TeardownNetDataSlot's own destroy-and-clear.
//
//  Wrapped as a one-method class rather than a free `__fastcall` function -
//  the same idiom Pathfinding/pathref_load.cpp's own
//  `SPathRefArrayGrow::SetAlloc` uses - because the real call sites never
//  set up `edx` at all (`this` in `ecx`, `index` pushed on the stack,
//  `retn 4`): a genuine two-register-argument `__fastcall` declaration
//  would force this build to materialise *some* value into `edx` for its
//  unused second parameter (an extra `xor edx,edx` neither call site
//  actually has), where a real thiscall member never touches `edx` at all.
//============================================================================
struct SNetSessionResetTarget
{
    void NetSessionFullReset_6F54C4F0(int index);
};

__declspec(naked) void SNetSessionResetTarget::NetSessionFullReset_6F54C4F0(int)
{
    __asm
    {
        mov eax, 06F54C4F0h
        jmp eax
    }
}

void __fastcall OsNetShutdownAlias(unsigned int);

void __fastcall UnregisterEventHandler(int slot, void* handler, void* a, int flags);

void NetSessionClearTagAndNotify();
void __fastcall TeardownNetDataSlot(int index, int);

//  0x6F5538F0 / 0x6F54CC80 / 0x6F54CCE0 - the three handlers
//  InitializeNetSessionConfig (netdata_initconfig.cpp) registers at slots
//  5/0x18/0x19.
void CNetData_PumpFromTls();
void __fastcall NetTickRateBump_6F54CC80(void* self);
void __fastcall NetTickRateDrop_6F54CCE0(void* self);

void __fastcall TeardownNetSession()
{
    SNetSessionResetTarget* records = (SNetSessionResetTarget*)NetDataRecords();

    records->NetSessionFullReset_6F54C4F0(0);
    records->NetSessionFullReset_6F54C4F0(1);

    NetSessionClearTagAndNotify();

    UnregisterEventHandler(0x19, (void*)NetTickRateDrop_6F54CCE0, 0, -1);
    UnregisterEventHandler(0x18, (void*)NetTickRateBump_6F54CC80, 0, -1);
    UnregisterEventHandler(5, (void*)CNetData_PumpFromTls, 0, -1);

    TeardownNetDataSlot(2, 0);

    OsNetShutdownAlias(2);
}
