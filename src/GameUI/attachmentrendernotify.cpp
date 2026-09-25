//============================================================================
//  0x6F02E160 - CAbility depth<=5 closure batch 1, slot 165 - previously
//  pending ("~2600-line closure reaching several unreconstructed GameUI
//  rendering functions - a real target in its own right, not an
//  accessor").  A fresh depth-5 dump narrows its own direct closure to
//  three callees: SAttachmentOwnerQuery::QueryOwner (already real,
//  attachmentownerquery.cpp) and two GameUI render-notify forwarders
//  (0x6F332D20/0x6F332DE0) that this file declares as naked thunks - each
//  of *their* own two callees (0x6F300710/0x6F300AB0/0x6F300C40) is
//  undumped GameUI rendering machinery well outside this batch's own
//  22-address assignment, so reconstructing this root does not pull them
//  in for real.
//
//  Ten stack arguments (`retn 0x28`), `this` in ecx.  Early-exits with
//  nothing done when m_field40 > 0 (a "already notified this frame"-
//  shaped guard).  Otherwise resolves an owner id (the last argument,
//  or m_typeId when that argument is -1) and an owner object (the
//  cached m_field30, refreshed through QueryOwner() when empty),
//  forwards eight of its ten arguments plus that owner/id pair into
//  0x6F332D20, re-resolves the owner a second time for 0x6F332DE0 (a
//  fresh QueryOwner() call, not a re-read of the same cache - the
//  shipped code really does call it twice), folds one argument into a
//  literal-looking flag word via the branchless
//  `sbb`/`and 0xFFFFFF23`/`add 0xDD` idiom, and finishes with a virtual
//  notify through this object's own vtable+0x298.
//
//  Every argument stays a raw `int`/`void*` slot named by its own stack
//  offset rather than a guessed semantic name: CLAUDE.md's own rule is
//  that only touched members get named, and nothing in this call tree's
//  own depth establishes what any of these ten values represent.
//============================================================================
#include "ability.h"

//  0x6F472890 - already real, attachmentownerquery.cpp.  Declared here on
//  a minimal stand-in the same way ability_depcheckalias.cpp already
//  does for the same address.
struct SAttachmentOwnerQuery { void* __thiscall QueryOwner(); };

//  0x6F332D20/0x6F332DE0 - out of scope for this batch; their own
//  callees have no dump anywhere in this repo.  Argument counts read
//  straight off this call tree's own push counts (eight stack dwords
//  plus ecx/edx for the first, two stack dwords plus ecx/edx for the
//  second) and now confirmed against the shipped tail bytes
//  (checksumE batch, `ida_query bytes` on the last three bytes of each
//  range): `retn 20h` at 0x6F332D20 and `retn 8` at 0x6F332DE0, both
//  matching the eight/two stack dwords these two __fastcall
//  declarations already push. The naked thunks themselves are declared
//  with the real parameter lists directly (not the `()`-then-cast shape
//  a previous pass left, which declared the thunk itself as a
//  zero-stack-argument __fastcall and only got the right ABI by luck of
//  every call site going through the correctly-typed function-pointer
//  cast) - `thunk_abi_audit.py` reads the thunk's *own* declared
//  signature, not how its one caller happens to invoke it.
__declspec(naked) void __fastcall RenderNotifyPrimaryThunk(
    void* /*owner*/, unsigned int /*ownerId*/,
    void* /*a0*/, void* /*a4*/, void* /*a8*/, void* /*aC*/, void* /*a14*/,
    void* /*a18*/, void* /*a1C*/, void* /*a20*/)
{
    __asm { mov eax, 0x6F332D20 }
    __asm { jmp eax }
}
__declspec(naked) void __fastcall RenderNotifySecondaryThunk(
    unsigned int /*ownerId*/, void* /*a0*/, unsigned int /*flag*/,
    void* /*owner2*/)
{
    __asm { mov eax, 0x6F332DE0 }
    __asm { jmp eax }
}

void CAbility::NotifyAttachmentRender(void* a0, void* a4, void* a8, void* aC,
                                       void* a10, void* a14, void* a18,
                                       void* a1C, void* a20, unsigned int ownerIdArg)
{
    if ((int)m_field40 > 0)
        return;

    unsigned int ownerId = (ownerIdArg != 0xFFFFFFFFu) ? ownerIdArg : m_typeId;

    void* owner = (void*)m_field30;
    if (owner == 0)
        owner = ((SAttachmentOwnerQuery*)this)->QueryOwner();

    RenderNotifyPrimaryThunk(owner, ownerId, a0, a4, a8, aC, a14, a18, a1C, a20);

    void* owner2 = (void*)m_field30;
    if (owner2 == 0)
        owner2 = ((SAttachmentOwnerQuery*)this)->QueryOwner();

    //  The branchless `neg`/`sbb`/`and 0xFFFFFF23`/`add 0xDD` fold:
    //  `(a10 != 0) ? 0 : 0xDD` - a10 nonzero clears the flag entirely,
    //  a10 zero (the caller left it unset) picks the literal 0xDD.
    unsigned int flag = (a10 != 0) ? 0u : 0xDDu;

    RenderNotifySecondaryThunk(ownerId, a0, flag, owner2);

    ((void(__thiscall*)(void*))(*(void***)this)[0x298 / 4])(this);
}
