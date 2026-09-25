//============================================================================
//  Naked redirects for the two "get an ability manager" callees
//  unit_abilitybyfootprinttype.cpp declares and calls but does not
//  reconstruct - neither address (0x6F06E3C0, 0x6F06E970) has a dump
//  anywhere in this tree (`tools/dumpfn.py` reports nothing for either),
//  so neither can be named or reconstructed for real; both are NEED_DUMP
//  per tools/thunk_inventory.py's own bucket.
//
//  `retn 0`: both call sites in unit_abilitybyfootprinttype.cpp push zero
//  stack arguments before calling - the `push esi`/`push edi` immediately
//  ahead of each call is that caller's own callee-saved register spill
//  (popped back off after the call clobbers it with the return value),
//  not an argument to either callee. Since callee-cleanup only has to
//  agree with what its callers actually push, and this is the only
//  caller of either address anywhere in this repo, `retn 0` is exactly
//  what its own dump requires - not a guess.
//============================================================================

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

ADDR_THUNK(void* GetAbilityManagerVariantA_6F06E3C0(), 0x6F06E3C0)
ADDR_THUNK(void* GetAbilityManagerVariantB_6F06E970(), 0x6F06E970)

#undef ADDR_THUNK
