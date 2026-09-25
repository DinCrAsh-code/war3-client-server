//============================================================================
//  NTempest::CPresence - vtable slot 0, 0x6F951CA0 (`??_7CPresence@NTempest@
//  @6B@`).  Bottom-but-one of the CAgentBaseAbs -> NIpse::CRlAgent ->
//  NIpse::CPrRelation -> NTempest::CPresenceTagged -> NTempest::CPresence ->
//  NTempest::CLinkedNode<CPresence> chain (each `mdisp:0` except
//  CLinkedNode<CPresence>, which is a secondary base at `mdisp:4`).
//
//  Two functions here:
//
//  - `CPresence::`scalar deleting destructor'(unsigned int)` (0x6F4A3760) -
//    no SEH frame of its own, reconstructed as real C++.  Same shape as
//    `Net/netclientgame_dtor.cpp`'s `Game::DeleteSelf`: call the real
//    destructor, then `SMemFree` the object if bit 0 of the flags argument
//    is set.
//  - `CPresence::~CPresence()` (0x6F4A1CF0) - carries a genuine `/EHsc`-style
//    SEH frame (security-cookie XOR, `push -1`/`push offset SEH_...`/
//    `mov eax, large fs:0`) plus three out-of-line catch funclets at
//    0x6F831090-0x6F8310C4, none of which this build's fixed `/GS- /EHs-c-`
//    flags can reproduce in real C++ - the same reasoning already used for
//    `Net/netclientgame_dtor.cpp`'s `NetClientGameBaseDtor::Destroy` and for
//    `Pathfinding/cellbuffer.cpp`'s own `ReleaseCMemBlockBase`.  Reproduced
//    as a naked `mov eax,<addr>/jmp eax` redirect to the real, unhooked
//    body and filed `THUNK` (not a reconstruction of its own behaviour) per
//    CLAUDE.md's behaviour-verdict rule.
//
//  What `~CPresence()`'s own body (read off `agent_worktrees/funcs/
//  0x6F4A1CF0.json`'s raw_asm, no fresh IDA access this session) tells us
//  about CPresence's own layout, independent of not being able to compile
//  it: it releases three owned sub-objects, through the already-
//  reconstructed `ReleaseCMemBlockBase` (0x6F4B3770,
//  `Pathfinding/cellbuffer.cpp` - itself a naked thunk, out of scope here,
//  already documented there), before restamping `this`'s own vtable back to
//  `CPresence`'s and returning:
//
//    this+0xAC  - a CMemBlock/CEntity-family member, released directly.
//    this+0x44  - NTempest::CPriorityQ<NIpse::CLrOpenNode,NIpse::CLrOpenNode>:
//                 its own vtable is stamped in-line immediately before the
//                 release call, so this member's first 4 bytes (its own
//                 vtable pointer) sit at literal offset this+0x44.
//    this+0x24  - another CMemBlock/CEntity-family member, released
//                 directly (no vtable restamp of its own first).
//
//  *** Position-hash relevance (see the branch's task background: slot 0x3C
//  index 15, sub_6F4A60A0, folds three consecutive dwords at
//  this+0x44/+0x48/+0x4C into the desync checksum - CORRECTION,
//  2026-09-07, vtable-derived: this slot is NIpse::CPrRelation's own
//  override, inherited unchanged through CRlAgent and CAgentBaseAbs, not
//  CAgentBaseAbs's own slot as originally filed - see
//  Agent/agentbaseabscomputechecksum.cpp's own updated header) ***
//  Every class from CPresenceTagged up through CAgentBaseAbs sits at RTTI
//  `mdisp:0` relative to CPresence, so CPresence's own `this` is the *same
//  address* as CAgentBaseAbs's `this` - there is no pointer adjustment to
//  account for.  That means this+0x44 here is the *same byte* the checksum
//  function reads.  But it is NOT a free-standing float: it is the vtable
//  pointer of an embedded `CPriorityQ<CLrOpenNode,CLrOpenNode>` sub-object -
//  a build-time constant, identical on every client running the same
//  binary.  Hashing it detects nothing about game state (it can only ever
//  contribute a build/version-consistency term, not a per-instance
//  position).  This is evidence *against* CPresence owning the "position"
//  half of that hash, not for it - the checksum's [this+0x44] is CPresence's
//  own CPriorityQ member's vtable slot, not an x/y/z float. [this+0x48] and
//  [this+0x4C] (CPriorityQ's own second and third dwords - most likely an
//  array pointer and a count/capacity field for a priority-queue container,
//  not coordinates) are worth a future session's look, but nothing here
//  suggests they are positions either. Whichever sibling class (CAgentBaseAbs,
//  CRlAgent, CPrRelation or CPresenceTagged - each mdisp:0, so *also* sharing
//  this exact byte range) turns out to declare real fields overlapping this
//  span should be treated as a stronger candidate than CPresence.
//
//  CLinkedNode<CPresence> (the `mdisp:4` secondary base): no write to
//  this+0x04/this+0x08 and no call into a separate base-destructor appears
//  anywhere in ~CPresence() - and `sub_6F479940` (the EH funclet that
//  destroys `this+0`, i.e. what a fully-torn-down `~CPresence()` itself
//  degenerates to: `mov [ecx], CPresence's own vftable; retn`, nothing
//  else) shows CPresence's own destructor body has no further explicit
//  member/base teardown once its three CMemBlock/CPriorityQ members are
//  gone. That is consistent with `CLinkedNode<CPresence>` being exactly the
//  plain intrusive prev/next mixin the task background guessed - no
//  virtuals (confirmed: no `??_7` for it in `agent_worktrees/names.json`,
//  only the RTTI descriptors), and evidently no non-trivial destructor
//  either. No out-of-line `CLinkedNode<CPresence>::CLinkedNode()`-shaped
//  function (the templated "prev = this; next = this" self-link idiom used
//  elsewhere in this codebase, e.g. TRefCnt's circular-list head) turned up
//  under this investigation; `agent_worktrees/` has no class entry at all
//  for `CLinkedNode<CPresence>` (no vtable, so nothing for the operator's
//  bulk RTTI dump to have found), and the two constructor-shaped candidates
//  this session located that end by stamping `??_7CPresence@NTempest@@6B@`
//  (0x6F4A1CF0's *sibling* candidate at 0x6F49F190, and 0x6F4A1CF0 itself
//  read the other way) disagree on which member offsets they construct
//  (0x1C alone vs. 0x24/0x44/0xAC) - left unresolved this session; see the
//  branch write-up before trusting either as "the" CPresence constructor
//  without further xref work on their own callers.
//============================================================================

//  0x6F4A1CF0 - the real ~CPresence() body.  Real SEH frame + three catch
//  funclets this build's /GS- /EHs-c- cannot reproduce - see file header.
class CPresenceRealDtor
{
public:
    void Run();
};
__declspec(naked) void CPresenceRealDtor::Run()
{
    __asm { mov eax, 06F4A1CF0h }
    __asm { jmp eax }
}

void  __stdcall SMemFree(void* ptr, const char* logfilename,
                         int logline, unsigned int flags);

namespace NTempest {

class CPresence
{
public:
    //  0x6F4A3760 - vtable slot 0.
    void* ScalarDeletingDestructor(unsigned int flags);
};

void* CPresence::ScalarDeletingDestructor(unsigned int flags)
{
    ((CPresenceRealDtor*)this)->Run();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

}  // namespace NTempest
