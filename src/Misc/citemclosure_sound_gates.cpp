//============================================================================
//  A frontier of deep Storm sound-engine internals, reached from CItem's own
//  vtable closure (agent_worktrees/classes/0x6F9320B4.json's BFS, batch 8 of
//  8 - see docs/targets/CItem-callee-batch8.md) wandering into an unrelated
//  engine subsystem at depth 5, the same way Misc/citemclosure_terrain_gates
//  (.cpp/2.cpp)'s own NTempest/terrain frontier did out of a different pair
//  of CItem slots.  Every one of these six addresses (0x6F6CAA40-0x6F6D72A0)
//  is confirmed unrelated to CItem/CSelectable by its own evidence: the
//  "HSOUND" hash-table tag, the "All sounds disabled, ignoring play requ..."
//  diagnostic string, dword_6FAD1608/6FAD1664/6FAD1668's sound-config-global
//  shape, and a lpCriticalSection-guarded (Sync/critsec.cpp's own
//  EnterCritSec/LeaveCritSec) linked sound-channel free list threaded
//  through dword_6FA9F8F8/dword_6FA9F92C - none of it item-positioning or
//  selection logic, all of it exactly the "deep, non-interesting chunk"
//  CLAUDE.md's own "work outward... not into Storm's internals" already says
//  to stop at.  Same pattern as citemclosure_terrain_gates.cpp: a direct
//  redirect into the game's own code at its real address, so a build
//  hooking anything else in this target still calls the genuine original
//  rather than link-failing on an unresolved symbol.  0% instruction match
//  by construction and not worth chasing further - see each BEHAVIOUR entry
//  in tools/funcmap.py.
//
//  Declared signatures below exist only to give thunk_abi_audit.py the
//  right `retn <n>` - the byte count read straight off each dump's own
//  trailing `retn` line - not to claim a real calling convention: nothing in
//  src/ calls any of these (grep says zero references outside this file),
//  so the declaration only has to compile and clean the right number of
//  stack bytes.  All six are genuine `__thiscall` members (a real register
//  argument in `ecx` at every call site the wider dump shows), so each is
//  spelled as a member of its own placeholder struct rather than a free
//  function, with plain `int` stack parameters - the simplest spelling that
//  cleans exactly 4*N bytes regardless of what the shipped body's real
//  argument types turn out to be.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  0x6F6CAA40 - `retn 30h` (12 stack dwords): the sound-play entry point
//  itself.  Checks the "all sounds disabled" global, looks up (or creates)
//  an HSOUND-tagged record via the data allocator (Net/dataallocator.h's own
//  0x6F4C1AB0), and dispatches into sub_6F6CFA60/sub_6F6CEF40 under the
//  sound-engine critical section.
struct SSoundChannelThunk
{
    void __thiscall PlaySoundOnChannel(int, int, int, int, int, int, int,
                                        int, int, int, int, int);
};
ADDR_THUNK(void __thiscall SSoundChannelThunk::PlaySoundOnChannel(
               int, int, int, int, int, int, int, int, int, int, int, int),
           0x6F6CAA40)

//  0x6F6CFA60 - `retn 24h` (9 stack dwords): initialises a sound-channel
//  record's fields (+0x178/+0x13C/+0x140/+0x144/+0x148/+0x14C/+0x1B0) from
//  its arguments and copies the filename into a Storm_501 buffer at
//  `this+0x34`, then checks the file size (sub_6F6CE2B0) against a 32000
//  byte streaming threshold.
struct SSoundInitThunk
{
    int __thiscall InitSoundChannel(int, int, int, int, int, int, int, int, int);
};
ADDR_THUNK(int __thiscall SSoundInitThunk::InitSoundChannel(
               int, int, int, int, int, int, int, int, int),
           0x6F6CFA60)

//  0x6F6D5CE0 - a single-instruction `jmp sub_6F6D44C0` redirect (IDA's own
//  IDENTICAL-shaped IdentAlias, e.g. Agent/handleobject.cpp's
//  GetHandleOrZeroAlias): `retn 0`, no stack arguments, matching the target's
//  own bare `retn`.
struct SSoundReleaseAliasThunk
{
    void __thiscall ReleaseSoundChannelAlias();
};
ADDR_THUNK(void __thiscall SSoundReleaseAliasThunk::ReleaseSoundChannelAlias(),
           0x6F6D5CE0)

//  0x6F6D6EA0 - `retn 0`: lazily creates a per-object sound-list head
//  (sub_6F6C7F00) and links it into the global list at dword_6FAD1670 via
//  sub_6F6D6A90, storing the new head at `this+0x270`.
struct SSoundListInitThunk
{
    void __thiscall EnsureSoundList();
};
ADDR_THUNK(void __thiscall SSoundListInitThunk::EnsureSoundList(),
           0x6F6D6EA0)

//  0x6F6D6FB0 - `retn 0`: unlinks a sound-channel record from the two
//  process-wide doubly-linked lists at dword_6FA9F928/dword_6FA9F8F8
//  (relinking each list's own sentinel, off_6FA9F92C/off_6FA9F8FC) after
//  timestamping it via GetTickCount, then re-links it at the head of both.
struct SSoundListRelinkThunk
{
    void __thiscall RelinkSoundChannel();
};
ADDR_THUNK(void __thiscall SSoundListRelinkThunk::RelinkSoundChannel(),
           0x6F6D6FB0)

//  0x6F6D72A0 - `retn 10h` (4 stack dwords): dispatches a sound-channel
//  update to one of four state handlers (sub_6F6D7090/sub_6F6D66D0/
//  sub_6F6D7170/sub_6F6D60F0/sub_6F6D5F40) keyed off internal state fields
//  at +0x1AC/+0x16C/+0x14C, after releasing a prior filename buffer
//  (sub_6F318420) when the new one differs from the object's own +0x1B8
//  scratch buffer.
struct SSoundUpdateThunk
{
    int __thiscall UpdateSoundChannel(int, int, int, int);
};
ADDR_THUNK(int __thiscall SSoundUpdateThunk::UpdateSoundChannel(
               int, int, int, int),
           0x6F6D72A0)
