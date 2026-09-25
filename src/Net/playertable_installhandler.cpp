//============================================================================
//  Net::NetRouter::PlayerTable::InstallHandler, 0x6F67F3F0 (150 instructions)
//
//  This is the function CLAUDE.md's own "open lead" pointed at: the single
//  place that installs BOTH of the two W3GS opcode dispatchers
//  (Probably_W3GS_ActionHandler @ 0x6F672340, "dispatcher1", NetRouter.cpp;
//  Probably_W3GS_ActionHandler_MaybeServerVersion @ 0x6F682300,
//  "dispatcher2", NetClient.cpp - see docs/targets/Probably_W3GS_
//  ActionHandler*.md) depending on whether the target player id equals this
//  machine's own player id.
//
//  `this` (ecx) is a genuinely new class this session named
//  Net::NetRouter::PlayerTable: an array-indexed table of per-player
//  connection records (Net::NetRouter::PlayerTable's own vtable,
//  ??_7PlayerTable@NetRouter@Net@@6B@ @ 0x6F9715C8, has exactly one slot - a
//  destructor - so every real method on this class, this one included, is
//  an ordinary non-virtual member; nothing here is vtable-dispatched).
//
//  Signature confirmed two ways: `edx` (the second argument) is passed
//  straight through, unmodified, as `this` to
//  NetProviderMethod0x90Self::CheckOrRegister (sub_6F656FE0, already
//  reconstructed - netprovider_method0x90.cpp) on the self-install path -
//  CheckOrRegister is a genuine thiscall member with no arguments of its
//  own, so `edx` can only be the receiver, `NetProviderMethod0x90Self*`.
//  The one stack argument (`arg_0`, retn 4) is a player id: it is compared
//  against `self->m_selfRecord->m_playerId` at function entry to choose
//  the branch, and against every record's own `m_playerId` in the walk
//  below.
//
//  ## The two install paths, and what they resolve
//
//  **Self path** (`playerId == self->m_selfRecord->m_playerId`): sets
//  `m_installedFlag`, lazily registers dispatcher1's own event callback
//  through NetProviderMethod0x90Self::CheckOrRegister -> NetProviderBase_
//  Register (netprovider_register.cpp) -> Probably_W3GS_ActionHandler, via
//  an 8-stack-argument helper (sub_6F672AF0, 542 instructions - genuinely
//  separate and well beyond this session's scope, kept a thunk) - then
//  walks the whole player record list clearing a "needs reset" bit
//  (flags & 2) on every record via ClearRecordConnection
//  (playertable_clearrecordconnection.cpp), accumulating a per-player
//  bitmask of which ones had it set, and hands that mask to two more
//  helpers (PlayerTable_Method679460/PlayerTable_Method67E9E0, both real
//  reconstructions - each reaches its own further unreconstructed callee).
//
//  **Other-player path**: walks the same player record list looking for a
//  record whose own `m_playerId` matches the argument. If none is found,
//  or the one found has no live connection (`m_connection == 0`), arms a
//  3-second retry (ArmRetryTimer(0xBB8), playertable_armretrytimer.cpp)
//  and gives up for now - this is the common "player hasn't connected yet"
//  case. If a live connection *is* found, it searches a second,
//  independent intrusive list (the "pending connect request" list at
//  +0x258/+0x25C, walked as a plain forward chain with no separate
//  link-offset field - the simpler of the two intrusive-list shapes this
//  codebase already documents, see src/Net/netprovider_pendinglist.cpp's
//  own header) for a request keyed to the same player id. Nothing found:
//  stash the id, arm a 10-second retry (ArmRetryTimer(0x2710)) and give
//  up. Found: bump the record's own refcount (InterlockedIncrementAt,
//  already reconstructed - Sync/critsec.cpp), install dispatcher2's own
//  callback chain through PlayerTable_InstallOtherCallback
//  (sub_6F683070, 151 instructions, now a real reconstruction - this is
//  the function the sibling dispatcher's own doc names as "reached from
//  sub_6F67F3F0"), and register the result into a fixed global dispatch
//  table (dword_6FACFFA8, funcmap.DATA row added this session).
//
//  ## What this resolves and what it still leaves open
//
//  This closes the "not yet reconciled" note in both dispatcher docs: the
//  self path and the other-player path are not "host" and "client" in the
//  sense either dispatcher's own name once suggested - every peer running
//  this code (host included) uses the self path for its OWN player record
//  (installing the NetRouter-side dispatcher on itself, i.e. "route what I
//  submit"), and the other-player path for each REMOTE record with a live
//  connection (installing the NetClient-side dispatcher, i.e. "receive
//  what that peer sends me"). Both paths run in the same process for every
//  peer; which dispatcher a given player id gets is a function of "is this
//  me", not "am I the host".
//
//  It does NOT locate the real client-side GameActionId-verb decode this
//  session's lead was ultimately chasing. dispatcher2 (NetClient.cpp) has
//  no case for PacketId.GameAction/Tock (already established), and this
//  function only ever installs dispatcher2 wholesale for a remote peer's
//  connection - it does not itself parse or forward any packet content.
//  The real decode point, if it exists as a distinct function at all,
//  remains unlocated; a future wave's most promising thread is
//  PlayerTable_InstallOtherCallback's own eight still-unidentified callees
//  (see its own header comment below).
//============================================================================
#include "playertable.h"
#include "crtsec.h"

//  PlayerTable_Method679460/PlayerTable_Method67E9E0 - defined further
//  below in this same file; forward-declared here so
//  PlayerTable_InstallOtherCallback (also below, but ahead of both in
//  source order) can call them.
void __fastcall PlayerTable_Method679460(void* self);
void __fastcall PlayerTable_Method67E9E0(void* self, unsigned int playerMask);

//----------------------------------------------------------------------------
//  sub_6F672AF0 (0x6F672AF0, 542 instructions) - dispatcher1's own install
//  chain (docs/targets/Probably_W3GS_ActionHandler.md's "Client vs server,
//  corrected" section: this function's own body carries three
//  ".\NetRouter.cpp" debug-assert strings and calls NetProviderBase_
//  Register). Genuinely separate and well beyond this session's scope - a
//  thunk to the real, unhooked body. `retn 20h` (8 stack dwords) read
//  straight off this address's own listing.
//----------------------------------------------------------------------------
extern "C" __declspec(naked) void __fastcall NetRouterPlayerTable_InstallSelfCallback(
    NetProviderMethod0x90Self* provider, void* selfPad05C,
    void* field140, void* field1D0, void* addrOfField1D4, void* addrOfField1D8,
    void* field19C, void* field1A8, PlayerRecordList* records, PlayerRecord* selfRecord)
{
    __asm { mov eax, 0x6F672AF0 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  Eight callees of sub_6F683070 below, none reached anywhere else in this
//  tree.  Six stay naked pass-through thunks (address-only or genuinely out
//  of scope); two (0x6F673790/0x6F673750) turned out to already be real,
//  scored reconstructions under different names elsewhere in the tree
//  (ClearField0x174_6F673790/ClearField0x170_6F673750,
//  src/Misc/trivialaccessors_06.cpp) - see this file's own note just below
//  the eight declarations for how the call site now reaches those instead
//  of a second, redundant naked body.
//
//  sub_6F6DAD90/sub_6F6DAD10/sub_6F6DAD70 - the same 8-instruction gate
//  three times over (`test ecx,ecx / jnz .. / push 57h / call Storm_465
//  (SErrSetLastError) / retn` on the taken side, `push edx / call <a
//  further, undumped helper> / retn` on the not-taken side) at three
//  different addresses with three different not-taken targets
//  (sub_6F6DA1C0/sub_6F6DA410/sub_6F6DA180 respectively, none reached by
//  this tree) - genuinely separate call targets, not the same function
//  three times. `retn` bare (0) at every one: `__fastcall(ecx,edx)`.
//  batch-J-w3gsaction-tiers: reconstructed for real as naked transcriptions
//  now (their own bodies are a plain gate, nothing SEH-shaped); the three
//  not-taken targets are each a genuinely separate, undumped ecx-object
//  helper (`thiscall(this), retn 4`, confirmed off sub_6F6DA1C0's own full
//  dump) and stay naked thunks.
//----------------------------------------------------------------------------
void __stdcall SErrSetLastError(unsigned int code);
extern "C" __declspec(naked) void __fastcall sub_6F6DA1C0(void* cond, unsigned int payload)
{
    __asm { mov eax, 0x6F6DA1C0 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) void __fastcall sub_6F6DA410(void* cond, unsigned int payload)
{
    __asm { mov eax, 0x6F6DA410 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) void __fastcall sub_6F6DA180(void* cond, unsigned int payload)
{
    __asm { mov eax, 0x6F6DA180 }
    __asm { jmp eax }
}

__declspec(naked) void __fastcall sub_6F6DAD90(void* cond, unsigned int payload)
{
    __asm
    {
        test    ecx, ecx
        jnz     short not_taken
        push    57h
        call    SErrSetLastError
        retn
    not_taken:
        push    edx
        call    sub_6F6DA1C0
        retn
    }
}
__declspec(naked) void __fastcall sub_6F6DAD10(void* cond, unsigned int payload)
{
    __asm
    {
        test    ecx, ecx
        jnz     short not_taken
        push    57h
        call    SErrSetLastError
        retn
    not_taken:
        push    edx
        call    sub_6F6DA410
        retn
    }
}
__declspec(naked) void __fastcall sub_6F6DAD70(void* cond, unsigned int payload)
{
    __asm
    {
        test    ecx, ecx
        jnz     short not_taken
        push    57h
        call    SErrSetLastError
        retn
    not_taken:
        push    edx
        call    sub_6F6DA180
        retn
    }
}

//  sub_6F679170 (0x6F679170, 80 instructions plus its own scope-table tail
//  at 0x6F844880) - the real `__except_handler4`-shaped frame
//  docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" section already documents: `push -1` /
//  `push offset SEH_6F679170` / `mov eax, fs:[0]` / a 0x5D8-byte reservation
//  and a `dword_6FAAE140`-cookie chain, ending in a tail-jump into
//  `__CxxFrameHandler3`. Builds some kind of formatted string
//  (`sub_6F2C9290`/`sub_6F4C2160`/`sub_6F4C1E70`/`sub_6F4C1BB0` - none
//  reached elsewhere in this tree) from the six-dword struct this
//  function's own caller builds just ahead of the call. Genuinely out of
//  scope for this session - a thunk. `retn` bare (0): `__fastcall(ecx,edx)`.
extern "C" __declspec(naked) void __fastcall sub_6F679170(void* self3C, void* argStruct)
{
    __asm { mov eax, 0x6F679170 }
    __asm { jmp eax }
}

//  sub_6F6795D0 (0x6F6795D0, 39 instructions) - walks the same
//  `self->m_records`-shaped list this file's self-install path already
//  walks (the `[esi+5Ch] & 2` / `[esi+3Ch]` shape ClearRecordConnection's
//  own body shares), conditionally calling a further undumped
//  `sub_6F678570`, then stamps a byte into `self->m_selfRecord+0x5D` (the
//  same field ClearRecordConnection sets to 0xFF). `retn` bare (0):
//  `__fastcall(ecx=self, edx=byte value in dl)`.  batch-J-w3gsaction-tiers:
//  reconstructed for real below as a naked transcription (the shipped body
//  reserves its scratch byte with a bare `push ecx`, never restoring a
//  meaningful ecx - `pop ecx` at the tail just balances the stack - so a
//  from-scratch C local could not promise the same slot without gambling
//  on the compiler's own frame layout).  `sub_6F678570` itself carries a
//  real `__except_handler4`-shaped SEH frame this build's fixed /GS-
//  /EHs-c- cannot reproduce (confirmed off its own dump) - stays a thunk.
extern "C" __declspec(naked) void __fastcall sub_6F678570(void*, void*)
{
    __asm { mov eax, 0x6F678570 }
    __asm { jmp eax }
}

__declspec(naked) void __fastcall sub_6F6795D0(void* self, unsigned int dlValue)
{
    __asm
    {
        push    ecx
        push    ebx
        push    esi
        push    edi
        mov     edi, ecx
        mov     eax, [edi+11Ch]
        xor     ecx, ecx
        test    eax, eax
        setle   cl
        mov     bl, dl
        mov     byte ptr [esp+0Fh], bl
        sub     ecx, 1
        and     ecx, eax
        mov     esi, ecx
        jle     short zero_case
    loop_top:
        test    byte ptr [esi+5Ch], 2
        jz      short skip_call
        mov     ecx, [esi+3Ch]
        lea     edx, [esp+0Fh]
        call    sub_6F678570
    skip_call:
        mov     eax, [edi+114h]
        add     eax, esi
        mov     esi, [eax+4]
        test    esi, esi
        jg      short loop_top
        mov     edx, [edi+148h]
        pop     edi
        pop     esi
        mov     [edx+5Dh], bl
        pop     ebx
        pop     ecx
        retn
    zero_case:
        mov     eax, [edi+148h]
        pop     edi
        pop     esi
        mov     [eax+5Dh], bl
        pop     ebx
        pop     ecx
        retn
    }
}

//  sub_6F65B0C0 (0x6F65B0C0, 7 instructions) - reserves one scratch byte on
//  its own stack (`push ecx`, never restoring a meaningful ecx - the same
//  idiom sub_6F6795D0's own header just above documents) and forwards its
//  address plus the literal `0x27` into the not-in-scope `sub_6F65A100`
//  (a real `__except_handler4`-shaped SEH frame this build's fixed
//  /GS- /EHs-c- cannot reproduce, confirmed off its own dump - `__stdcall`,
//  `retn 8`, two stack arguments; the caller's own return value is
//  discarded, so the declaration's return type does not matter).
//  `retn` bare (0): `__fastcall(ecx)`, reconstructed for real below.
extern "C" __declspec(naked) void __stdcall sub_6F65A100(void*, unsigned int)
{
    __asm { mov eax, 0x6F65A100 }
    __asm { jmp eax }
}

__declspec(naked) void __fastcall sub_6F65B0C0(void* provider)
{
    __asm
    {
        push    ecx
        lea     eax, [esp+3]
        push    eax
        push    27h
        call    sub_6F65A100
        pop     ecx
        retn
    }
}

//  sub_6F682E00 (0x6F682E00, 188 instructions) - a third, distinct W3GS
//  dispatcher-shaped function (not dispatcher1/dispatcher2, the two this
//  file's own header already names), stamped unconditionally into the
//  candidate's own vtable-shaped slot below. Its address is the only thing
//  taken here - never called from this function - so a plain `__stdcall`
//  redirect with the right byte count is enough to keep
//  `tools/thunk_abi_audit.py` honest even though nothing in this tree ever
//  calls it that way. `retn 1Ch` (7 stack dwords) read off its own listing.
extern "C" __declspec(naked) void __stdcall sub_6F682E00(
    void*, void*, void*, void*, void*, void*, void*)
{
    __asm { mov eax, 0x6F682E00 }
    __asm { jmp eax }
}

//  Probably_W3GS_ActionHandler_MaybeServerVersion (0x6F682300) - dispatcher2
//  itself (w3gsactionhandler2.cpp, already reconstructed). Only its address
//  is taken here (conditionally stamped into the same slot sub_6F682E00's
//  own address occupies unconditionally, just above it) - re-declared with
//  the exact real signature so the linker resolves the same symbol rather
//  than a second, differently-mangled one (tools/link_check.py's first
//  check).
int __stdcall Probably_W3GS_ActionHandler_MaybeServerVersion(
    unsigned int opcodeDword, unsigned int Src, void** arg0,
    unsigned int, unsigned int trailing);

//  Storm.dll ordinal 403 (SMemFree) and its own placeholder log-site
//  string, the same `aE_1`/`"e:\...` truncation
//  Net/actionqueuerecord_resize.cpp's own header already documents (IDA's
//  own truncated display of a string this call tree never reaches in
//  full) - re-declared locally per that file's own established, per-TU
//  convention rather than shared, since it is not exported data.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
static const char aE_1[] = "e:\\...";

//----------------------------------------------------------------------------
//  0x6F673790/0x6F673750 - batch-J-w3gsaction-tiers found these were two
//  C++ names on the same shipped addresses already: `ClearField0x174_
//  6F673790`/`ClearField0x170_6F673750` (src/Misc/trivialaccessors_06.cpp)
//  are real, scored reconstructions of exactly these two `mov dword ptr
//  [ecx+N], 0 / retn` bodies, and funcmap.py already points both addresses
//  at them. The naked pass-through redirects that used to live here under
//  local names (`sub_6F673790`/`sub_6F673750`) were dead weight - nothing
//  ever scored them, they only existed so the call site below had a
//  symbol - so they are gone; the call site now names the real functions
//  directly. MASM's inline assembler resolves a plain identifier to
//  whatever the compiler decorates it as, so `call
//  ClearField0x174_6F673790` below reaches the genuine (non-extern-"C",
//  thiscall-mangled) definition exactly the same way this file's own
//  `call ClearRecordConnection` already does two screens up.
//----------------------------------------------------------------------------
void __fastcall ClearField0x174_6F673790(void* self);   // trivialaccessors_06.cpp
void __fastcall ClearField0x170_6F673750(void* self);   // trivialaccessors_06.cpp

//----------------------------------------------------------------------------
//  0x6F683070 (151 instructions) - dispatcher2's own install chain, reached
//  from this function's other-player path.  Named as the "reached from
//  sub_6F67F3F0" function in docs/targets/Probably_W3GS_
//  ActionHandler_MaybeServerVersion.md.  Un-thunked this session
//  (batch-F-net-misc-2, resumed): a naked, instruction-for-instruction
//  transcription rather than a thunk, following the same numeric
//  `[esp+N]`-literal method RegisterPending's own reconstruction above
//  uses for its stack slots (IDA's own `var_NN`/`arg_NN` symbolic names
//  resolved to a plain hex offset per instruction, cross-checked against
//  every other reference to the same slot before landing).
//
//  Reads as ClearRecordConnection (sub_6F6737E0, already reconstructed) on
//  a session-mismatch gate (`edx != self->m_selfRecord` at entry - a
//  comparison against whatever `*outResult` already holds, not
//  necessarily 0: the caller's own stack slot for `outResult` is reused
//  from an earlier local rather than freshly zeroed, so this function's
//  own "record" argument to ClearRecordConnection is really just "self,
//  0" - unusual, but it is the shipped call site's own registers, faithfully
//  reproduced verbatim; nothing about a naked transcription depends on
//  reasoning out why the caller leaves it that way), then a
//  compute-a-player-bit / RegisterPending (now real) / InterlockedIncrementAt
//  sequence, then **stamps the two W3GS dispatcher vtable-slot function
//  pointers** - `sub_6F682E00` unconditionally and, conditionally,
//  `Probably_W3GS_ActionHandler_MaybeServerVersion` itself - into the
//  candidate's own object, an intrusive doubly-linked-list unlink of
//  `*outResult`'s own node (the same `TSLink<T>::Unlink()` shape
//  netproviderltcp_notify.cpp's own header documents), a field copy off
//  `self->m_selfRecord+0xA0`, and tail calls into PlayerTable_Method679460
//  and PlayerTable_Method67E9E0 (both already real, above) before a final
//  sub_6F65B0C0.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall PlayerTable_InstallOtherCallback(
    void* self, void** outResult, NetProviderMethod0x90Self* provider, PendingConnectRequest* pendingNode, int zero)
{
    __asm
    {
        sub     esp, 48h
        mov     eax, g_securityCookie
        xor     eax, esp
        mov     [esp+44h], eax
        mov     eax, [esp+4Ch]
        push    ebx
        push    ebp
        mov     ebp, [esp+58h]
        push    esi
        push    edi
        mov     edi, edx
        mov     edx, [edi]
        mov     esi, ecx
        mov     ecx, [esp+64h]
        xor     ebx, ebx
        cmp     edx, [esi+148h]
        mov     [esp+10h], eax
        mov     [esp+18h], ecx
        jz      short mergeAfterCacheUpdate
        mov     ecx, esi
        call    ClearRecordConnection
        mov     [esp+14h], eax
        mov     eax, [edi]
        movzx   ecx, byte ptr [eax+0B4h]
        sub     ecx, 1
        mov     ebx, 1
        shl     ebx, cl
        push    eax
        mov     ecx, offset g_netRouterDispatchTable
        call    NetRouterDispatchTable::RegisterPending
        mov     eax, [esi+148h]
        lea     ecx, [eax+10h]
        mov     [edi], eax
        call    InterlockedIncrementAt
        mov     edx, [edi]
        mov     eax, [esp+14h]
        mov     [edx+3Ch], eax
        mov     eax, [edi]
        mov     edx, [eax+0Ch]
        mov     ecx, [eax+3Ch]
        call    sub_6F6DAD90
    mergeAfterCacheUpdate:
        mov     ecx, [edi]
        mov     ecx, [ecx+3Ch]
        lea     edx, [esp+34h]
        call    sub_6F6DAD10
        mov     eax, [edi]
        mov     edx, [esp+44h]
        mov     [eax+0A0h], edx
        mov     ecx, [esp+48h]
        mov     [eax+0A4h], ecx
        mov     edx, [esp+4Ch]
        add     eax, 0A0h
        mov     [eax+8], edx
        mov     ecx, [esp+50h]
        mov     [eax+0Ch], ecx
        mov     edx, [esp+34h]
        mov     eax, [esp+38h]
        mov     ecx, [esp+3Ch]
        mov     [esi+150h], edx
        mov     edx, [esp+40h]
        mov     [esi+154h], eax
        mov     [esi+158h], ecx
        mov     [esi+15Ch], edx
        mov     eax, [edi]
        mov     ecx, [eax+3Ch]
        mov     edx, offset sub_6F682E00
        call    sub_6F6DAD70
        mov     ecx, [esp+18h]
        xor     eax, eax
        cmp     ecx, eax
        jz      short skipDispatcherStamp
        mov     dword ptr [ecx], offset Probably_W3GS_ActionHandler_MaybeServerVersion
    skipDispatcherStamp:
        mov     [esi+260h], al
        mov     [esi+261h], al
        mov     ecx, [esi+26Ch]
        cmp     ecx, eax
        jz      short afterAssertCleanup
        push    eax
        push    0FFFFFFFEh
        push    offset aE_1
        push    ecx
        call    SMemFree
        xor     eax, eax
    afterAssertCleanup:
        mov     ecx, esi
        mov     [esi+264h], eax
        mov     [esi+268h], eax
        mov     [esi+26Ch], eax
        call    ClearField0x174_6F673790
        call    ClearField0x170_6F673750
        mov     edx, [ebp+0]
        cmp     edx, eax
        jz      short afterUnlink
        mov     ecx, [ebp+4]
        cmp     ecx, eax
        jg      short computeCase2
        not     ecx
        jmp     short storeBack
    computeCase2:
        mov     eax, ebp
        sub     eax, [edx+4]
        add     ecx, eax
        xor     eax, eax
    storeBack:
        mov     [ecx], edx
        mov     ecx, [ebp+0]
        mov     edx, [ebp+4]
        mov     [ecx+4], edx
        mov     [ebp+0], eax
        mov     [ebp+4], eax
    afterUnlink:
        mov     [esi+274h], ebp
        mov     eax, [edi]
        mov     ecx, [ebp+8]
        mov     [eax+0B0h], ecx
        mov     eax, [edi]
        or      dword ptr [eax+80h], 10000h
        mov     eax, [esi+148h]
        mov     edx, [eax+0A0h]
        add     eax, 0A0h
        mov     [esp+1Ch], edx
        mov     ecx, [eax+4]
        mov     [esp+20h], ecx
        mov     edx, [eax+8]
        mov     ecx, [edi]
        mov     [esp+24h], edx
        mov     eax, [eax+0Ch]
        mov     [esp+28h], eax
        mov     eax, [esi+1D0h]
        mov     [esp+2Ch], eax
        sub     eax, [esi+1D4h]
        lea     edx, [esp+1Ch]
        mov     [esp+30h], eax
        mov     ecx, [ecx+3Ch]
        call    sub_6F679170
        mov     edx, [ebp+18h]
        mov     dl, [edx]
        mov     ecx, esi
        call    sub_6F6795D0
        mov     ecx, esi
        call    PlayerTable_Method679460
        mov     edx, ebx
        mov     ecx, esi
        call    PlayerTable_Method67E9E0
        mov     ecx, [esp+10h]
        call    sub_6F65B0C0
        mov     ecx, [esp+54h]
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        xor     ecx, esp
        call    SecurityCheckCookie
        add     esp, 48h
        retn    0Ch
    }
}

//----------------------------------------------------------------------------
//  sub_6F679460 (0x6F679460, 24 instructions) and sub_6F67E9E0
//  (0x6F67E9E0, 53 instructions) - both reached only from the self-install
//  path's own end, after the record-list reset walk. Each reaches its own
//  further unreconstructed callee (sub_6F663CC0/sub_6F678270 and
//  Notify_6F679630 respectively); those two are genuinely below this session's
//  scope and stay naked redirects (argument shape only, off each call
//  site's own register/push pattern), but the two outer functions
//  themselves are written out below as naked transcriptions of the real
//  instruction stream - not pass-through thunks - the same way this file's
//  own header describes the self/other-player paths.  Both `retn` with no
//  operand (thiscall/fastcall, no stack args); their own return values are
//  never used by this function.
//----------------------------------------------------------------------------

//  sub_6F663CC0 - PlayerTable_Method679460's own gate check: already
//  reconstructed as CheckAndAdvanceInterval (netclient_pendingflush.cpp).
//  `ecx`=state (here: &self->field_148->field_50), `edx`=intervalMs
//  (0x1F4), stack args right-to-left (outLongSilence, then allowEscalate)
//  - the pushed `outLongSilence` pointer names a caller-owned dword
//  pre-set to 2 that this call site never reads back afterward, so its
//  value is write-only here.
extern int __fastcall CheckAndAdvanceInterval(int* state, unsigned int intervalMs,
                                               int allowEscalate, int* outLongSilence);
//  sub_6F678270 - PlayerTable_Method679460's own tail notify: `__fastcall
//  (ecx=self->field_148+0x3C, edx=&value)`, `retn 0`.  Below this
//  session's scope - naked redirect.
extern "C" __declspec(naked) void __fastcall
sub_6F678270(void* obj, void* valuePtr)
{
    __asm { mov eax, 0x6F678270 }
    __asm { jmp eax }
}

//  Not extern "C" - the naked transcription is a real, scored
//  reconstruction now (funcmap.py maps sub_6F679460 to it), and the
//  pipeline's own extract_gen.py only picks a function's generated stream
//  up off the friendly-name comment MSVC's normal C++ decoration carries
//  after `PUBLIC ?Name@@...@Z` - `extern "C"` suppresses that comment the
//  same way it does for this file's still-thunked helpers above.
__declspec(naked) void __fastcall PlayerTable_Method679460(void* self)
{
    __asm
    {
        sub     esp, 8
        push    esi
        mov     esi, [ecx+148h]
        test    esi, esi
        jz      short done
        cmp     dword ptr [esi+3Ch], 0
        jz      short done
        lea     eax, [esp+8]
        push    eax
        push    0
        lea     ecx, [esi+50h]
        mov     edx, 1F4h
        mov     dword ptr [esp+10h], 2
        call    CheckAndAdvanceInterval
        test    eax, eax
        jz      short done
        mov     cx, [esi+48h]
        mov     [esp+4], cx
        mov     ecx, [esi+3Ch]
        lea     edx, [esp+4]
        call    sub_6F678270
done:
        pop     esi
        add     esp, 8
        retn
    }
}
//  0x6F679630 - PlayerTable_Method67E9E0's own per-node reset callee.
//  batch-J-w3gsaction-tiers found this was the SAME shipped address as
//  w3gsaction_tier18.cpp's own `Notify_6F679630` (a second naked
//  pass-through thunk under a different local name - the exact
//  "one shipped address, two C++ names" defect thunk_inventory's own notes
//  warn about); that file now carries the one real, scored reconstruction,
//  reused here by forward declaration instead of a second naked body.
//  `extern "C"`, matching that real definition's own linkage.
extern "C" void __fastcall Notify_6F679630(void* self, void* node);

//  0x6F67E9E0 - walks a second, independent list rooted at
//  `self->field_288` (masked the same branchless "clamp negative-or-zero
//  to zero" way PlayerTable_InstallHandler's own record-list walk clamps
//  its own `Tail()` - not a `TSList<T>::Tail()` call here, just the raw
//  field read inlined the identical way), the same +0x148/+0x3C target
//  gate PlayerTable_Method679460 checks first.  For each node whose own
//  type tag (+0x240) is 2, 3 or 4 and whose 1-based bit index (+0x244)
//  is set in the incoming `playerMask`, resets the node's own state
//  (+0x240=1, +0x244=0, +0x248=0) and hands it to Notify_6F679630.
void __fastcall PlayerTable_Method67E9E0(void* self, unsigned int playerMask)
{
    if (playerMask == 0)
        return;

    void* target = *(void**)((char*)self + 0x148);
    if (!target || *(int*)((char*)target + 0x3C) == 0)
        return;

    char* node = *(char**)((char*)self + 0x288);
    if ((int)node <= 0)
        node = 0;

    for (; (int)node > 0; node = *(char**)(node + 4))
    {
        int type = *(int*)(node + 0x240);
        if (type != 2 && type != 3 && type != 4)
            continue;

        unsigned char bitIndexPlus1 = *(unsigned char*)(node + 0x244);
        unsigned int bit = 1u << (bitIndexPlus1 - 1);
        if ((playerMask & bit) == 0)
            continue;

        *(int*)(node + 0x240) = 1;
        *(unsigned char*)(node + 0x244) = 0;
        *(int*)(node + 0x248) = 0;
        Notify_6F679630(self, node);
    }
}

//----------------------------------------------------------------------------
//  dword_6FACFFA8 - a fixed global object the other-player path registers
//  its own install result into. Its own type is not recoverable from this
//  call tree, so RegisterPending below is a naked transcription of the
//  real instruction stream rather than a typed re-derivation - the final
//  step is a hand-written vtable slot-0 dispatch through `*this` whose
//  real class has no committed vtable anywhere this session reaches -
//  guessing a typedef for it is exactly the risk CLAUDE.md's own
//  vtable_dispatch_audit.py note warns against, so the real call sequence
//  is reproduced directly instead. Its gate check (0x6F6C4070) is the
//  same already-reconstructed InterlockedDecrementAt (Sync/critsec.cpp)
//  netrouter_pregame_dtor.cpp's own note already names for this address;
//  the tail status-stripe SignalOne (0x6F6D8950) is the same
//  already-reconstructed ConditionVariable::SignalOne this file's sibling
//  timer code reaches. batch-J-w3gsaction-tiers: its two own callees
//  (sub_6F6777A0, a 5-stack-arg find-or-insert into a container at
//  `this+4`; sub_6F674CF0, a 1-arg node initializer on the same
//  sub-object) are reconstructed for real below too, as naked
//  transcriptions - the container's own real class is still unidentified,
//  so the instruction stream is reproduced directly rather than guessed
//  at through a typed re-derivation, the same reasoning RegisterPending
//  itself already uses one level up.
//----------------------------------------------------------------------------
long __fastcall InterlockedDecrementAt(volatile long* target);   // Sync/critsec.cpp

//  0x6F6D8950 - ConditionVariable::SignalOne, already reconstructed
//  (gameaction0x26_notify.cpp); same declaration shape this file's own
//  sibling timer code (netproviderbnet_timer.cpp/netproviderltcp_notify.cpp)
//  already uses.  0x6F6D9820 - ConditionVariable::Wait, same class,
//  already reconstructed for real elsewhere (see e.g.
//  g_netClientRecordTableGuard.Wait() in the w3gsaction_tier* files).
struct ConditionVariable
{
    void Wait(unsigned int value);       // sub_6F6D9820
    void SignalOne(unsigned int value);
};

//  0x6F6D9820 - ConditionVariable::Wait's own callee here is the class
//  itself (`this` = the sub-object at self+0x20), fastcall(this, stack
//  arg) - see the class declaration just above.
__declspec(naked) void* __fastcall
sub_6F6777A0(void* self4, void* /*unused_edx*/, int a, int b, void* c, int d, int e)
{
    __asm
    {
        push    ebx
        push    ebp
        push    esi
        mov     esi, [esp+10h]
        test    esi, esi
        push    edi
        mov     ebx, ecx
        jz      short returnNull
        mov     edi, [esp+18h]
        and     esi, 7
        lea     eax, [esi+esi*2]
        lea     ebp, [ebx+eax*4+20h]
        push    edi
        mov     ecx, ebp
        call    ConditionVariable::Wait
        lea     ecx, [esi+esi*2+21h]
        lea     eax, [ebx+ecx*4]
        mov     eax, [eax+4]
        xor     edx, edx
        test    eax, eax
        setle   dl
        sub     edx, 1
        and     eax, edx
        test    eax, eax
        jle     short notFound
    scanLoop:
        mov     ecx, [esp+14h]
        cmp     [eax+0Ch], ecx
        jz      short found
        mov     eax, [eax+8]
        test    eax, eax
        jg      short scanLoop
    notFound:
        push    edi
        mov     ecx, ebp
        call    ConditionVariable::SignalOne
    returnNull:
        mov     eax, [esp+1Ch]
        pop     edi
        pop     esi
        pop     ebp
        mov     dword ptr [eax], 0FFFFFFFFh
        xor     eax, eax
        pop     ebx
        retn    14h
    found:
        mov     edx, [esp+1Ch]
        neg     edi
        sbb     edi, edi
        and     edi, 8
        add     edi, esi
        mov     [edx], edi
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    14h
    }
}
__declspec(naked) void __fastcall
sub_6F674CF0(void* self4, void* /*unused_edx*/, void* node)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, [esp+0Ch]
        mov     esi, [edi+4]
        test    esi, esi
        lea     eax, [edi+4]
        jz      short skipUnlink
        mov     edx, [eax+4]
        test    edx, edx
        jg      short posCase
        not     edx
        jmp     short storeBack
    posCase:
        mov     ecx, eax
        sub     ecx, [esi+4]
        add     edx, ecx
    storeBack:
        mov     [edx], esi
        mov     ecx, [eax]
        mov     edx, [eax+4]
        mov     [ecx+4], edx
        mov     dword ptr [eax], 0
        mov     dword ptr [eax+4], 0
    skipUnlink:
        mov     dword ptr [edi+0Ch], 0
        pop     edi
        pop     esi
        retn    4
    }
}

__declspec(naked) void __thiscall NetRouterDispatchTable::RegisterPending(void*)
{
    __asm
    {
        mov     eax, [esp+4]
        push    ebp
        push    esi
        mov     esi, [eax+0Ch]
        push    edi
        mov     edi, ecx
        lea     ecx, [eax+10h]
        xor     ebp, ebp
        call    InterlockedDecrementAt
        test    eax, eax
        jnz     short skip_all
        push    ebx
        push    1
        push    eax
        lea     eax, [esp+1Ch]
        push    eax
        push    1
        lea     ebx, [edi+4]
        push    esi
        mov     ecx, ebx
        call    sub_6F6777A0
        mov     esi, eax
        test    esi, esi
        jz      short after_find
        cmp     dword ptr [esi+10h], ebp
        jnz     short have_status
        push    esi
        mov     ecx, ebx
        call    sub_6F674CF0
        mov     ebp, 1
have_status:
        mov     eax, [esp+14h]
        cmp     eax, 0FFFFFFFFh
        jz      short skip_signal
        cmp     eax, 8
        sbb     ecx, ecx
        and     eax, 7
        add     ecx, 1
        add     eax, 3
        lea     edx, [eax+eax*2]
        push    ecx
        lea     ecx, [edi+edx*4]
        call    ConditionVariable::SignalOne
skip_signal:
        test    ebp, ebp
        jz      short after_find
        mov     eax, [edi]
        mov     edx, [eax]
        push    esi
        mov     ecx, edi
        call    edx
after_find:
        pop     ebx
        pop     edi
        pop     esi
        mov     eax, ebp
        pop     ebp
        retn    4
skip_all:
        pop     edi
        pop     esi
        mov     eax, ebp
        pop     ebp
        retn    4
    }
}

//============================================================================
//  0x6F67F3F0 - the function itself. Free `__fastcall(self, provider,
//  playerId)`: `edx` (provider) is a real second argument forwarded
//  unmodified into NetProviderMethod0x90Self::CheckOrRegister, which plain
//  thiscall could not reach.
//============================================================================
void __fastcall PlayerTable_InstallHandler(PlayerTable* self, NetProviderMethod0x90Self* provider, unsigned char playerId)
{
    if (playerId == self->m_selfRecord->m_playerId)
    {
        self->m_installedFlag = 1;

        if (!provider->CheckOrRegister())
            return;

        NetRouterPlayerTable_InstallSelfCallback(provider, &self->m_field05C,
            self->m_field140, self->m_field1D0, &self->m_field1D4, &self->m_field1D8,
            self->m_field19C, self->m_field1A8, &self->m_records, self->m_selfRecord);

        unsigned int playerMask = 0;
        PlayerRecord* rec = self->m_records.Tail();
        if (rec)
        {
            do
            {
                if (rec->m_flags & 2)
                {
                    ClearRecordConnection(self, rec);
                    playerMask |= 1u << (rec->m_playerId - 1);
                }
                rec = self->m_records.Prev(rec);
            } while ((int)rec > 0);
        }

        PlayerTable_Method679460(self);
        PlayerTable_Method67E9E0(self, playerMask);
        return;
    }

    self->m_installedFlag = 0;

    PlayerRecord* found = 0;
    PlayerRecord* rec = self->m_records.Tail();
    if (rec)
    {
        for (;;)
        {
            if (rec->m_playerId == playerId)
            {
                found = rec;
                break;
            }
            rec = self->m_records.Prev(rec);
            if ((int)rec <= 0)
                break;
        }
    }

    if (!found || !found->m_connection)
    {
        ArmRetryTimer(self, 0xBB8);   // 3000ms - no record yet, or no live connection
        return;
    }

    int prevlink = (int)self->m_pendingTerm.m_prevlink;
    PendingConnectRequest* node = prevlink > 0 ? (PendingConnectRequest*)prevlink : 0;
    while (node)
    {
        if (node->m_idBuf[0] == playerId)
            break;
        node = node->m_prevlink;
        if ((int)node <= 0)
        {
            node = 0;
            break;
        }
    }

    if (!node)
    {
        self->m_pendingPlayerId = playerId;
        self->m_pendingGen = 0;
        ArmRetryTimer(self, 0x2710);   // 10000ms - connection is live, but no pending request queued yet
        return;
    }

    InterlockedIncrementAt(&found->m_refCount);
    void* outResult = 0;
    PlayerTable_InstallOtherCallback(self, &outResult, provider, node, 0);
    g_netRouterDispatchTable.RegisterPending(outResult);
}
