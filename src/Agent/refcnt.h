//============================================================================
//  A small intrusive-refcount base class ("??_7TRefCnt@@6B@" in the target
//  dump's own vtable comment).  0x6F00C070 (defined inline below - see that
//  member's own comment for why), reached from the CWidget__Constructor
//  dump, supplied its destructor's body; nothing in either call tree
//  reaches the other virtual slot's body, only its layout.
//
//  vtable+0 is named `ReleaseSelf` here (not left as a placeholder): the
//  already-reconstructed `CAgent::ReleaseSelf` (agent.h, from a separate,
//  already-matched target) occupies exactly this slot once `CAgent` derives
//  from `CObserver`/`TRefCnt` (this dump's own sub_6F001730 shows
//  `CAgent`'s constructor building a `CObserver` base subobject first, then
//  overwriting the vtable) - it has to be the same name to land at the same
//  physical slot and keep that target's already-matched
//  `obj->vtbl[7]`-style offsets (GetAgileTypeId, slot 7) from shifting.
//  Given trivial (non-pure) bodies rather than `= 0`: the CWidget__Constructor
//  dump directly constructs a concrete `CObserver` (observer.h) - both as a
//  real base subobject and as a placement-newed member - and a class
//  embedded by value cannot leave any base virtual unresolved, even one
//  this call tree never calls through.  Nothing here claims to know what
//  either slot's real implementation does for a plain `CObserver`.
//============================================================================
#ifndef REFCNT_H
#define REFCNT_H

class TRefCnt
{
public:
    //====================================================================
    //  TRefCnt's vtable contribution.  Declaration order *is* vtable order
    //  (C++'s own rule), so this block is sorted by slot index and has
    //  to stay that way - inserting one line in the wrong place renames
    //  every slot after it.  Slots 0-1; see
    //  docs/targets/CWidget_CSelectable_CItem__vtables.md.
    //====================================================================
    //  slot 0 / +0x00 (0x6F00C060, sub_6F00C060)
    virtual void ReleaseSelf();
    //  slot 1 / +0x04 (0x6F00C080, sub_6F00C080)
    virtual TRefCnt* DeleteSelf(int flags);

    //  0x6F00C070 - defined inline just below (not out-of-line in its own
    //  .cpp the way most of this file's other members are) precisely
    //  *because* it has to be inlinable: the CItem__Constructor dump
    //  (0x6F2B7520, position.h) needs it collapsed into Position's own
    //  implicit destructor, which is only possible when this definition is
    //  visible in the same translation unit.  Reached two other ways in
    //  this codebase, both of which still match: as a real out-of-line
    //  `call` from CObserver::~CObserver's __finally trampoline
    //  (sub_6F62A9D0, observer.cpp - not reproducible at this toolchain's
    //  fixed /GS- /EHs-c-, see docs/msvc-vc8-idioms.md, "An
    //  `__except_handler4`-shaped frame this toolchain cannot reproduce"),
    //  and inlined directly into CObserver::CObserver's own happy path
    //  (which already matches).  A trivial destructor - resets the vtable
    //  pointer and nothing else.  Deliberately *not* virtual: a derived
    //  class's destructor calls its base class's destructor directly at
    //  compile time regardless (see CObserver::~CObserver, observer.cpp),
    //  and this dump's already-matched sibling target fixed CAgent's
    //  vtable at exactly two slots contributed by this base (ReleaseSelf,
    //  OnZeroRefCount) - a virtual destructor would add a third and shift
    //  every one of CAgent's own slots (GetAgileTypeId at 7, in
    //  particular) down by one.
    ~TRefCnt() {}

    int m_refcount;   // +4

    //  0x6F4C4030 - returns `this` unchanged. Bodily identical to a
    //  zero-delta this-adjuster thunk; nothing in this call tree gives it
    //  any other job, so it is named for the shape it has rather than one
    //  it might have had in a wider hierarchy.
    TRefCnt* Self();

    //  0x6F4C40A0 - drop one reference; once the count reaches 0, tail-call
    //  slot 1 to let the derived class destroy itself.
    //
    //  That call is spelled through the raw vtable offset rather than
    //  through `DeleteSelf` above, and the reason is in the shipped binary
    //  rather than here: this tail jump pushes *nothing*, while every
    //  deleting-destructor call site in the CWidget family pushes a flags
    //  word and its callee cleans it (`retn 4`).  Slot 1 is invoked with
    //  two different argument counts by the shipped code itself, so one
    //  C++ signature cannot spell both - the declaration above matches the
    //  five derived overrides, and this one call site keeps its own shape.
    void Release();

    //  0x6F4C40C0 - take one reference and return `this`; on a null `this`,
    //  report ERROR_INVALID_PARAMETER (0x57, 'W') through Storm's error
    //  channel and return null instead of touching the (nonexistent)
    //  refcount.
    //
    //  **It takes the object's Storm allocation tag in edx.**  Every one of
    //  the nine call sites in this repo's dumps loads it - `mov edx, offset
    //  aHtexture` / `aHfog` / `aHtextfont` / `aHcamera` / `aHtokenizer` /
    //  `aHlight` / `aHlistener` / `aHtextblock` / `aHmaterial` - and the
    //  body never reads it, which is what an argument only a debug build
    //  uses looks like at /O2.  `__fastcall` on a member is the spelling
    //  that puts the receiver in ecx and one argument in edx and still
    //  cleans nothing (docs/msvc-vc8-idioms.md, "A receiver in ecx with a
    //  second argument in edx is a free __fastcall"); declared without it,
    //  every one of those nine call sites loses its `mov edx`.
    TRefCnt* __fastcall AddRef(const char* tag);

    //  0x6F4C40E0 - the same "take a reference, return this" shape as
    //  AddRef(), but silent on a null `this` - no SErrSetLastError call,
    //  just a null return. Two call sites in this dump want the same
    //  operation with and without the diagnostic, so the shipped build
    //  carries both rather than making the louder one branch into the
    //  quieter one.
    TRefCnt* AddRefOrNull();
};

//  0x6F4C8870 / 0x6F4C8880 - see refcnt_release_forward.cpp.
void __fastcall ReleaseForward_6F4C8870(TRefCnt* self);
void __fastcall ReleaseForward_6F4C8880(TRefCnt* self);

#endif
