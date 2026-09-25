//============================================================================
//  Net::NetClient - the last five vtable slots (2, 3, 7, 8, 10 of 18),
//  networking wave 10 (agent-networking-wave3-2026-08-29).
//
//  All five carry the same real `__except_handler4`-shaped SEH cookie frame
//  (`push -1` / `push offset SEH_<addr>` / `mov eax, fs:[0]` / the
//  `dword_6FAAE140 ^ esp` cookie, matching docs/msvc-vc8-idioms.md's own
//  "An `__except_handler4`-shaped frame this toolchain cannot reproduce"
//  section byte for byte) except slot 10, which has the plain, EBP-based
//  `/GS`-only cookie over a large (0x164-byte) local buffer instead - the
//  same unreproducible-under-this-build's-/GS- shape LenPrefixedCopier's
//  own entry (netclient_notify family, slot 11) already documents.
//
//  Read whole-body (this session transcribed every one instruction by
//  instruction - see docs/targets/NetRouter_NetPlayer_NetClient_oneslot_sweep.md's
//  wave 10 section for the read-through), each is genuinely substantial,
//  novel machinery: slot 2 and slot 3 both allocate a fresh Storm-tagged
//  block, zero-construct it through sub_6F66F4F0 (which stamps
//  `??_7NetPlayer@Net@@6B@` at offset 0 and `??_7CDataStore@@6B@` at
//  offset 0x24 - i.e. this NetClient method is building a `Net::NetPlayer`
//  sub-object, not anything NetClient owns directly), then builds a SECOND,
//  much larger (0x290-byte) object through `sub_6F67D930` - which this
//  session identified for certain as `Net::NetClient::Game`'s own real
//  base constructor: it stamps this same tree's own
//  `??_7Game@NetClient@Net@@6B@` at offset 0 and
//  `??_7Game@NetClient@Net@@6B@_0` at offset 0x10, exactly the two vtables
//  vtable_classes.json's own `Net::NetClient::Game`/`Net::NetClient::Game_0`
//  entries already document as one object's two base sub-objects - and
//  goes on to construct nine more nested sub-objects of its own
//  (`TSingletonInstanceId<Game,1>`, `TExtraInstanceRecycler<SyncEvent>`,
//  two `TInstanceRecycler<...>` specialisations, `TInstanceId<Game>`, three
//  more embedded lists/critical sections). Slot 8 does the same two-object
//  build from a different, much larger source record. Slot 7 walks a
//  critical-section-guarded list at `stru_6FACFF70` (a *different*, not
//  yet named, global from the notify family's own `unk_6FACFF90` guard),
//  toggles Win32 events (`CreateEventA`/`WaitForSingleObject`/`SetEvent`
//  wrappers) with real timeouts, and ends by forwarding through the
//  already-reconstructed `UnregisterEventHandler` (slot 0x12,
//  eventunregister.cpp) - a teardown counterpart to slot 2/3/8's build.
//  Slot 10 shares the record-table-walk shape the notify family and
//  netclient_installsession.cpp already establish (`unk_6FACFF90`/
//  `dword_6FACFFA4`, the same `state in [4,6)` plus `+0x244` flag gate
//  InstallSession's own record check uses) but then builds a genuinely new
//  0x130-byte local record of its own and forwards it through seven more
//  never-before-seen callees.
//
//  None of that closure is itself reconstructed this session - guessing an
//  argument count wrong anywhere in it is exactly the live-crash class
//  thunk_abi_audit.py exists to catch, and every one of these five reaches
//  at least half a dozen addresses with no dump body in this tree at all
//  (`sub_6F656D70`, `sub_6F6578F0`, `sub_6F65B100`, `sub_6F67C700`,
//  `sub_6F43D5E0`, `sub_6F65B180`, `sub_6F677B70` for slot 10 alone). Each
//  of the five stays a whole-function `__declspec(naked)` redirect straight
//  to its own real, unhooked body - the exact same reasoning
//  `netproviderbnet_seh.cpp` (this class's own sibling, `Net::NetClient`'s
//  neighbour `Net::NetProviderBNET`) already uses for five of its own
//  slots, and the same convention that finished that class
//  (`missing_count: 0`, `status: "finished"` in vtable_classes.json) with
//  those five thunked rather than fully traced. Behaviourally identical to
//  the shipped function in every case (it *is* the shipped function); the
//  score is 0 by construction since none of the real body's own
//  instructions are reproduced - see each one's own `tools/funcmap.py`
//  `BEHAVIOUR` entry.
//
//  Argument *counts* below are load-bearing (each pushes exactly the bytes
//  the shipped body's own `retn <n>` cleans, confirmed from this session's
//  own read of each root's dump): slot 2 `retn 1Ch` (7 stack dwords), slot
//  3 `retn 24h` (9), slot 7 `retn 14h` (5), slot 8 `retn 8` (2), slot 10
//  `retn 4` (1) - all `this` in `ecx` (every one of the five reads `ecx`
//  directly in its own prologue, confirmed per-function). The *types* are
//  plain ints rather than a guess at real meaning, same as
//  netproviderbnet_seh.cpp's own five.
//============================================================================

class NetClientSelf
{
public:
    int  Method_2(int a0, int a4, int a8, int aC, int a10, int a14, int a18);   // 0x6F6835C0, slot 2, retn 1Ch
    int  Method_3(int a0, int a4, int a8, int aC, int a10, int a14, int a18,
                  int a1C, int a20);                                            // 0x6F683800, slot 3, retn 24h
    int  Method_7(int a0, int a4, int a8, int aC, int a10);                     // 0x6F680BE0, slot 7, retn 14h
    void Method_8(int a0, int a4);                                             // 0x6F6828D0, slot 8, retn 8
    void Method_10(int a0);                                                    // 0x6F67CB40, slot 10, retn 4
};

__declspec(naked) int NetClientSelf::Method_2(int, int, int, int, int, int, int)
{
    __asm { mov eax, 0x6F6835C0 }
    __asm { jmp eax }
}
__declspec(naked) int NetClientSelf::Method_3(int, int, int, int, int, int, int, int, int)
{
    __asm { mov eax, 0x6F683800 }
    __asm { jmp eax }
}
__declspec(naked) int NetClientSelf::Method_7(int, int, int, int, int)
{
    __asm { mov eax, 0x6F680BE0 }
    __asm { jmp eax }
}
__declspec(naked) void NetClientSelf::Method_8(int, int)
{
    __asm { mov eax, 0x6F6828D0 }
    __asm { jmp eax }
}
__declspec(naked) void NetClientSelf::Method_10(int)
{
    __asm { mov eax, 0x6F67CB40 }
    __asm { jmp eax }
}
