//============================================================================
//  0x6F4AEE60 / 0x6F4A60A0 - the "tempest presence" checksum-fold callback
//  a `CAgentBaseAbs`-vtable object (RTTI-confirmed live, `0x6F9520D4`,
//  agent_worktrees/classes/0x6F9520D4.json) exposes at vtable slot 0x3C
//  (index 15) - the exact slot `TempestProviderListHash`
//  (Net/tempestproviderlisthash.cpp) calls on every live entry of the
//  tempest presence host's own array.
//
//  CORRECTION (2026-09-07, vtable-derived): neither function is
//  `CAgentBaseAbs`'s own. This session built the exact slot-target table
//  for every class in the CPresence -> ... -> CAgentBaseAbs chain by
//  reading each one's own `agent_worktrees/classes/<ADDR>.json` (now
//  available for the whole `NTempest`/`NIpse` chain after the operator's
//  2026-09-07 namespace/template vtable-dump fix - see CLAUDE.md's own
//  "Caveat for classes/" note). Walking slot 15 (+0x3C) top to bottom:
//  CPresence's own slot 15 is a no-op (`nullsub_1472`); `CPresenceTagged`
//  is the first class to override it, with `0x6F4AEE60`
//  (`NTempest::CPresenceTagged::FoldLinkFieldsIntoHash`,
//  Agent/presencetagged.h); `CPrRelation` overrides it again with
//  `0x6F4A60A0` (`CAgentRelation::ComputeChecksum`, `CAgentRelation` being
//  the established alias for `NIpse::CPrRelation`, agentrelation.h); and
//  neither `CRlAgent` nor `CAgentBaseAbs` ever touches the slot again. So a
//  live `CAgentBaseAbs` object's `vftable[0x3C]` really does read
//  `0x6F4A60A0` - nothing about *that* changes, and nothing about what gets
//  hashed changes either - but the class that actually installs each
//  address at that slot is `NTempest::CPresenceTagged` (for 0x6F4AEE60,
//  later shadowed) and `NIpse::CPrRelation`/`CAgentRelation` (for
//  0x6F4A60A0, the one a live object really dispatches to), not
//  `CAgentBaseAbs`. These were previously misfiled as
//  `CAgentBaseAbsRaw::FoldLinkFieldsIntoHash`/`::ComputeChecksum` on the
//  assumption that "one of CAgentBaseAbs's 19 vtable slots" meant
//  "CAgentBaseAbs's own method" - the same mistake this rename batch
//  corrects across Agent/agentbaseabs_related.h's own former
//  `CAgentBaseAbsRelated` struct. Bodies below are byte-for-byte unchanged;
//  only the owning class and the calling convention's home type changed
//  (both are now real class members instead of a raw-offset
//  `CAgentBaseAbsRaw` interface - `NTempest::CPresenceTagged` and
//  `NIpse::CPrRelation`/`CAgentRelation` are real classes this repo already
//  declares elsewhere, unlike `CAgentBaseAbs` itself, which still has no
//  full layout - only a handful of its 19 vtable slots are reconstructed,
//  docs/targets/vtables/CAgentBaseAbs.txt - so a raw-offset interface
//  remains the right shape only for genuinely CAgentBaseAbs-owned slots).
//
//  Calling convention, both functions: `this` in ecx, the `unsigned int*`
//  accumulator in a single stack argument (`retn 4`) - not a second
//  `__fastcall` register - matching the shape
//  docs/notes/checksum-provider-registry.md's own "TempestProviderListHash"
//  section already established for this exact slot from the caller's own
//  side (`lea ecx,[esp+8] ; push ecx ; mov ecx,eax`, i.e. the accumulator
//  address is pushed *before* `ecx` is overwritten with the callee's own
//  `this`).
//
//  ---------------------------------------------------------------------
//  What this pair actually folds into the hash, precisely, in order:
//  ---------------------------------------------------------------------
//    1. this+0x14, this+0x18      (via ComputeChecksum's helper call, below)
//    2. this+0x44
//    3. this+0x48
//    4. IF this+0x30 (a pointer, call it `linked`) is non-null:
//         linked+0x14, linked+0x18  -- folded once
//         linked+0x14, linked+0x18  -- folded again, from *genuinely
//                                       duplicated* inline code, not a loop
//                                       or a second call site (see below)
//    5. this+0x4C
//
//  Every one of the 7 dword folds above uses the identical byte-fold
//  idiom `Sync/ctimesynchashinto.cpp`/`Misc/genericfieldrecordhash.cpp`
//  already name for unrelated callers: each of a dword's 4 bytes runs
//  through `(b*0x7A2Du)^b` and the four results are added together (not
//  XORed) into the running accumulator, most-significant byte first. The
//  four-fold combine is by ADD, not XOR - the disassembly's own
//  `add [esi], ebx` / `add eax, ebx` chain, never a `xor` against the
//  accumulator itself (only the per-byte scramble's own internal step
//  uses `xor`).
//
//  **The duplicate fold at step 4 is real, not a transcription slip.**
//  Both dwords are read and folded twice, back to back, from two
//  byte-for-byte independent instruction sequences (0x6F4A614F-0x6F4A61E1,
//  then 0x6F4A61ED-0x6F4A627C) with a `test eax,eax ; jz` between them
//  (0x6F4A61E3) that can never actually branch - `eax` (the `linked`
//  pointer) is not reassigned anywhere between the first non-null check
//  at 0x6F4A6147 and this second test, so the second test is dead and the
//  fall-through into the duplicate block is unconditional once the first
//  one is taken. This reads as an artifact of the *original C++* calling
//  the same "fold these two fields" step twice (most likely two separate
//  calls to a small helper the compiler fully inlined at both call
//  sites, since `sub_6F4AEE60` below shows the identical two-dword fold
//  already exists as a real out-of-line function for `this` itself, just
//  never called a second time for `this`) rather than a codegen quirk -
//  transcribed here exactly as duplicated work, per CLAUDE.md's "identical
//  code, not equivalent code" rule.
//
//  ---------------------------------------------------------------------
//  Is this+0x44/0x48/0x4C really a 3D position? Not proven; here is
//  everything this session found, for and against:
//  ---------------------------------------------------------------------
//    FOR:  three CONSECUTIVE dwords, folded back-to-back in ascending
//          offset order, in a class whose one live-confirmed instance
//          derives a name ("Agent Base Abstract") suggesting a shared
//          spatial/presence interface; the class is registered into the
//          same "tempest presence" list a moving/positioned object would
//          plausibly need to publish itself into.
//    AGAINST / OPEN: none of `CAgentBaseAbs`'s other 17 reconstructed-or-
//          read vtable slots (0x00-0x30, 0x38-0x40, 0x48 - see
//          agent_worktrees/classes/0x6F9520D4.json) is a plain
//          accessor of +0x44/+0x48/+0x4C (no `mov eax,[ecx+44h] ; retn`
//          shape, no float-argument setter storing there) - the
//          independent confirmation CLAUDE.md's own assignment asked for
//          was not found. `CAgentBaseAbs` also does not appear to share
//          any inheritance chain with this repo's own already-typed
//          position classes (`Position`/`SmartPositionTrack`,
//          Widget/position.h / Pathfinding/positiontrack.h) - those
//          resolve their position through a `{handle, typeTag}` pair into
//          a live `CPathTrace`, a completely different shape from three
//          raw dwords stored inline, and `CAgent` itself (Agent/agent.h)
//          has no data field anywhere near +0x44 (its own +0x44/+0x48/+0x4C
//          are *vtable slot* offsets, an unrelated numbering). So the
//          "three consecutive dwords, early in a spatially-named class"
//          reading is suggestive, not independently corroborated, and
//          this function is named for what it does (fold this object's
//          own checksum-relevant fields into an accumulator - the same
//          `ComputeChecksum` role `CGameWar3`/`CUnitDatabase`/`CRandData`/
//          `CCheatData` each already have at this exact vtable slot
//          across this repo, see checksum-provider-registry.md) rather
//          than a `FoldPositionIntoHash` name that would overclaim.
//          **This is exactly the kind of thing a live x32dbg capture can
//          close**: watch `[this+0x44]`/`[this+0x48]`/`[this+0x4C]` (or
//          just dump them once with `this` pinned to a live unit) while
//          the unit moves - three fields that visibly track a moving
//          unit's coordinates would settle this outright.
//============================================================================
#include "agentrelation.h"
#include "presence.h"

//----------------------------------------------------------------------------
//  A single byte, widened to 32 bits, run through the `(b*0x7A2D)^b`
//  scramble - the same one Sync/ctimesynchashinto.cpp and
//  Misc/genericfieldrecordhash.cpp each already name for their own,
//  unrelated callers; kept as its own local copy here for the same
//  reason those two do (no shared translation unit).
//----------------------------------------------------------------------------
static inline unsigned int AgentBaseAbsByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

//----------------------------------------------------------------------------
//  Fold one dword's four bytes (most-significant first) into *hash,
//  combined by addition - the shape every one of the 7 call sites in this
//  file's two functions shares, transcribed once rather than seven times.
//----------------------------------------------------------------------------
static inline void AgentBaseAbsFoldDword(unsigned int* hash, unsigned int v)
{
    //  Matches the dump's own extraction exactly: `shr eax,0x10` then
    //  `movzx ah`/`movzx al` off the shifted value for the top two bytes,
    //  `movzx ch`/`movzx cl` off the unshifted value for the bottom two -
    //  the same shape Sync/ctimesynchashinto.cpp's own `CTimeSync_HashInto`
    //  already documents for an unrelated caller, not a uniform
    //  `(v>>n)&0xFF` four times.
    unsigned int hi = v >> 16;
    unsigned int folded = AgentBaseAbsByteFold((hi >> 8) & 0xFFu);
    folded += AgentBaseAbsByteFold(hi & 0xFFu);
    folded += AgentBaseAbsByteFold((v >> 8) & 0xFFu);
    folded += AgentBaseAbsByteFold(v & 0xFFu);
    *hash += folded;
}

//  0x6F4AEE60 - see Agent/presence.h's own declaration comment for
//  the vtable-derived ownership evidence (this method used to live in a
//  separate Agent/presencetagged.h until a same-session merge folded it
//  into presence.h's own, more complete `CPresenceTagged` - see that
//  header's own file comment). Folds `this+0x14` then `this+0x18`
//  into *hash. A real, separate out-of-line function (not inlined into its
//  one caller below, matching the dump's own `call sub_6F4AEE60`).
void NTempest::CPresenceTagged::FoldLinkFieldsIntoHash(unsigned int* hash)
{
    const char* base = (const char*)this;
    AgentBaseAbsFoldDword(hash, *(const unsigned int*)(base + 0x14));
    AgentBaseAbsFoldDword(hash, *(const unsigned int*)(base + 0x18));
}

//  0x6F4A60A0 - see agentrelation.h's own declaration comment for the
//  vtable-derived ownership evidence: `NIpse::CPrRelation`'s own vtable
//  slot 0x3C (index 15), the slot `TempestProviderListHash`'s per-entry
//  checksum-fold callback actually resolves to on a live CAgentBaseAbs
//  object. See this file's own header comment for the exact fold order,
//  the genuinely duplicated `linked+0x14`/`linked+0x18` block, and the open
//  question of whether `this+0x44/0x48/0x4C` is a 3D position.
void CAgentRelation::ComputeChecksum(unsigned int* hash)
{
    ((NTempest::CPresenceTagged*)this)->FoldLinkFieldsIntoHash(hash);

    const char* base = (const char*)this;
    AgentBaseAbsFoldDword(hash, *(const unsigned int*)(base + 0x44));
    AgentBaseAbsFoldDword(hash, *(const unsigned int*)(base + 0x48));

    void* linked = *(void* const*)(base + 0x30);
    if (linked != 0)
    {
        const char* linkedBase = (const char*)linked;

        //  First fold of linked+0x14/linked+0x18.
        AgentBaseAbsFoldDword(hash, *(const unsigned int*)(linkedBase + 0x14));
        AgentBaseAbsFoldDword(hash, *(const unsigned int*)(linkedBase + 0x18));

        //  Genuinely duplicated: the shipped body re-reads and re-folds
        //  the SAME two dwords a second time here, gated by a `test
        //  eax,eax`/`jz` that can never actually branch (see this file's
        //  own header comment) - kept as written rather than "cleaned up"
        //  into a loop, per CLAUDE.md's "say why when the source looks
        //  odd" rule.
        AgentBaseAbsFoldDword(hash, *(const unsigned int*)(linkedBase + 0x14));
        AgentBaseAbsFoldDword(hash, *(const unsigned int*)(linkedBase + 0x18));
    }

    AgentBaseAbsFoldDword(hash, *(const unsigned int*)(base + 0x4C));
}
