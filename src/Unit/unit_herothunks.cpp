//============================================================================
//  Naked redirects for the hero cluster's callees that this batch declares
//  and calls but does not reconstruct.  None is a reconstruction and none
//  is ever hookable.
//
//  Every argument count is the shipped call site's own push count, written
//  out here because none of these has a dump for tools/thunk_abi_audit.py
//  to check it against.  The two with no dump were settled by hand against
//  the image (`ida_query func_at` for the range, `ida_query bytes <end-3> 3`
//  for the tail) and carry the byte they really end with:
//
//      0x6F29BB80  ecx = type id, nothing pushed          -> retn 0 (image: C3)
//      0x6F419F80  ecx/edx only (a UnitQueryVisitFn)      -> retn 0 (image: C3)
//
//  0x6F0787D0 (SUnitAbilityHost::FindAbility) used to be a third thunk here;
//  it is a real reconstruction now, Unit/unit_findability.cpp.
//============================================================================

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

ADDR_THUNK(int __fastcall UnitTypeDeclaredLevel(unsigned int), 0x6F29BB80)
ADDR_THUNK(int __fastcall HeroCollectVisitor(void*, void*), 0x6F419F80)

#undef ADDR_THUNK
