//============================================================================
//  0x6F481250 / 0x6F481270 - two byte-identical plain accessors for the
//  tempest presence host global (`g_unk6FAB778C`): `mov eax, dword_6FAB778C
//  ; retn` (no `this`, no stack argument, result in eax).  Two separate
//  addresses with the identical body, not a linker-folding failure -
//  ordinary for this image, which was linked without `/OPT:ICF`
//  (`docs/msvc-vc8-idioms.md`'s own "same body, different callers" idiom -
//  see e.g. cdatastorescratch.h's WriteDword/WriteDwordAlt family for
//  another instance of it).
//
//  Investigated as part of "does any of the 8 unreconstructed
//  g_unk6FAB778C-touching functions perform the missing
//  STwoIndexedFreeLists::AllocateAt call" (checksum-provider-registry.md):
//  no - these are trivial getters, nothing more.
//
//  Neither address has a static caller anywhere in agent_worktrees'
//  own raw_asm-derived xref index - most likely reached only through a
//  function pointer or a vtable slot this repo's static xrefs tooling
//  cannot resolve (worktree_store.py's own docstring: an indirect call
//  target comes back null, never guessed).  Left unnamed beyond a plain
//  functional name for exactly that reason - no caller context to derive a
//  real method name from.
//============================================================================
#include "tempestpresencehost.h"

void* GetTempestPresenceHost()
{
    return g_unk6FAB778C;
}

void* GetTempestPresenceHostAlt()
{
    return g_unk6FAB778C;
}
