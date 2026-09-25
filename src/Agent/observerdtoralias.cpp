//============================================================================
//  0x6F2F1690 - `jmp sub_6F62A9D0`, one instruction: a second entry point
//  into CObserver::~CObserver (0x6F62A9D0, already reconstructed -
//  Agent/observer.cpp).  Reached from at least eleven unrelated classes'
//  own constructors/destructors (CTargetMode, CEscMenu, CSelectMode,
//  CBuildMode, ...) that all destroy a CObserver base/member through this
//  one shared address rather than 0x6F62A9D0 directly - the shipped
//  linker's own identical-code-folding artifact (docs/msvc-vc8-idioms.md's
//  ICF note), not anything CItem-specific.  THUNK - nothing of its own to
//  reconstruct.
//============================================================================

//  A single relative `jmp` in the shipped stream (not the two-instruction
//  `mov eax,<addr>/jmp eax` ADDR_THUNK shape) - but an undefined
//  `extern "C" sub_<addr>` (the Misc/misc_tailjmp_thunks.cpp style) leaves
//  the real link (`tools/link_check.py`) with an unresolved external:
//  nothing on the real link line defines a bare `sub_<addr>` symbol, only
//  the literal address itself resolves.  Written the ADDR_THUNK way
//  instead - one extra instruction against the dump's own single `jmp`,
//  which does not matter for a THUNK's score.
__declspec(naked) void CObserverDtorAlias()
{
    __asm { mov eax, 0x6F62A9D0 }
    __asm { jmp eax }
}
