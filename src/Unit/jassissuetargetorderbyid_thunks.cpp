//============================================================================
//  0x6F3C89D0 / 0x6F3B3090 - JASS_IssueTargetOrderById and its own order
//  dispatcher.  NOT reconstructions - redirects into the real shipped
//  bodies, left thunks on purpose rather than guessed.
//
//  0x6F3C89D0 itself is simple (resolve the acting unit and the target
//  widget through ResolveUnitHandle/ResolveWidgetHandle, read the unit's
//  own Method_0x64() class tag, dispatch) and would reconstruct cleanly on
//  its own.  What makes it a thunk here is its one real callee: the two
//  are register-allocated together under whole-program optimisation, and
//  0x6F3B3090 trusts two of its four logical arguments - the target widget
//  and the unit's own class tag - to already be sitting in edi/ebx from
//  0x6F3C89D0's own earlier work, with no `mov edi,.../mov ebx,...`
//  anywhere in its own body to load them itself.  Only two arguments (the
//  unit, and an orderId/sourceRef slot this dump's one call site always
//  passes 0) are real stack words, and both are read at fixed `[esp+N]`
//  offsets, so a caller from a different translation unit that pushes a
//  conventional four-argument stack frame instead of the real edi/ebx-plus-
//  two-stack-words one shifts every one of those offsets and corrupts the
//  callee's own frame - exactly the live-crash class thunk_abi_audit.py
//  exists to catch.  Reconstructing 0x6F3B3090's own ~190-instruction body
//  (a CCommandBar order-type-registry lookup, then one of two
//  CheckTargetOrder/MakeOrderTargetAgent or CheckPointOrder/
//  MakeOrderPointAgent paths against the target's own smart position -
//  CWidget slot 0xB8 into CPathRef::SmartPtrToWorldVec3 - ending in
//  CUnit::SubmitOrder) is real work for a session with more of this
//  target's own budget left; reconstructing 0x6F3C89D0 alone without it
//  would just move the same live-crash risk into a *correct-looking*
//  ordinary C++ call this pipeline's own tools would not catch, so both
//  stay thunks together rather than one being guessed past the other.
//
//  Argument counts read off each address's own trailing bytes
//  (worktree_store.py bytes): both end in a bare `retn` (0 bytes, the
//  caller cleans its own pushed words), which is why both are declared
//  __cdecl here rather than any signature that would push a wrong byte
//  count into either.
//============================================================================
#define ADDR_THUNK_CDECL(sig, addr)         \
    __declspec(naked) sig                   \
    {                                       \
        __asm { mov eax, addr }             \
        __asm { jmp eax }                   \
    }

ADDR_THUNK_CDECL(int __cdecl JASS_IssueTargetOrderById(int hUnit, int orderId, int hTarget), 0x6F3C89D0)
ADDR_THUNK_CDECL(int __cdecl DispatchTargetOrder_6F3B3090(void), 0x6F3B3090)

#undef ADDR_THUNK_CDECL
