//============================================================================
//  0x6F3677D0 - the selection host's own destructor tail: release the
//  sprite array at +0x180 (the same 0x2C-byte block/`ReleaseSpriteArrayTwice`
//  shape Containers/spritearrayreleasetwice.cpp already reconstructs at
//  0x6F35D440, reused here at a fresh offset), free a Storm HSPRITE__
//  handle at +0x178 if the base object still owns one, and tail into the
//  next base's own destructor.  `retn 0`.
//
//  CORRECTION: this was originally written as `TSExplicitList<T>::
//  UnlinkAll()` on the belief that +0x180 was a Storm.h explicit list -
//  that guessed at the field's shape from its offset alone rather than
//  checking the real disassembly, which link_check.py's missing-definition
//  check caught (nothing anywhere defines that invented template
//  instantiation, because it was never real). The actual call at +0x180 is
//  `lea ecx,[esi+180h] / call sub_6F35D440`, i.e. ReleaseSpriteArrayTwice
//  with `this` shifted by 0x180 - the same function, a different receiver
//  offset, not a list unlink at all.
//
//  sub_6F608330 is outside this batch's own dump (no heading reaches it),
//  so it is declared and called rather than guessed at - the same
//  "callee not in this dump yet" situation every batch runs into calling
//  into shared plumbing another slice owns.
//============================================================================
#include "storm.h"

//  Containers/spritearrayreleasetwice.cpp's own receiver type and function -
//  incomplete here (nothing in this TU reads through it), declared to match
//  that TU's mangled name exactly.
struct SSpriteArrayHost;
void __fastcall ReleaseSpriteArrayTwice(SSpriteArrayHost* self);

struct SSelectionHostTail
{
    char                              m_reserved00[0x178];
    void*                             m_hsprite;   // +0x178
    char                              m_reserved180[0x2C];  // +0x180, ReleaseSpriteArrayTwice's own block
};

//  0x6F608330 - outside this batch's own dump, the selection host's next
//  base class destructor; confirmed by this function's own xrefs
//  (`worktree_store.py xrefs 0x6F3677D0 --from` shows the real tail `jmp
//  sub_6F608330`). link_check.py caught this as declared and called but
//  never defined - a naked redirect until its own dump lands.
__declspec(naked) void __fastcall SelectionHostNextBaseDtor(void*)
{
    __asm { mov eax, 0x6F608330 }
    __asm { jmp eax }
}

//  Declared taking `void*`, not `SSelectionHostTail*`, so that
//  GameUI/selectionhostreleasefields.cpp's own forward declaration of the
//  same function (which has no reason to name this file's local struct)
//  mangles to the identical symbol - a mismatched pointee type here would
//  give the two translation units two different symbols that never link
//  (tools/link_check.py's first defect class).
void __fastcall SelectionHostBaseDtor(void* selfVoid)
{
    SSelectionHostTail* self = (SSelectionHostTail*)selfVoid;

    ReleaseSpriteArrayTwice((SSpriteArrayHost*)((char*)self + 0x180));

    if (self->m_hsprite)
        SMemFree(self->m_hsprite, ".PAUHSPRITE__@@", -2, 0);

    SelectionHostNextBaseDtor(self);
}
