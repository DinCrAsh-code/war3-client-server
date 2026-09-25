//============================================================================
//  The out-of-tree callees BuildAndRegisterEventBinding
//  (agenteventbinding.cpp) and the two functions in this session's
//  handle-field pair reach, as naked redirects to their shipped
//  addresses.
//
//  Every one of these is below this target's own depth, so none has a
//  dump body and `tools/thunk_abi_audit.py` cannot check any of them
//  against a `retn` - which is unknown, not clean.  What each argument
//  count *is* checked against is the shipped call site in this target's
//  own disassembly: how many words it pushes before the `call`, whether
//  it sets ecx, and whether it sets edx.  Those three facts fix the
//  convention, and they are quoted per entry below.
//
//  Its own translation unit rather than src/Misc/unreconstructed_thunks.cpp:
//  every declaration here is on a type declared locally in
//  agenteventbinding.cpp, and MSVC puts the class name in the mangled
//  name - so the definition has to be built against the same header, not
//  against a second copy of the type someone edits later.
//============================================================================
#include "agenteventbinding.h"
#include "CFloat.h"

//  No leading declaration in the macro: every signature here is already
//  declared in agenteventbinding.h, and a member function may not be
//  re-declared at namespace scope.
#define ADDR_THUNK_QUERY(sig, addr)                 \
    __declspec(naked) sig                           \
    { __asm { mov eax, addr } __asm { jmp eax } }

//----------------------------------------------------------------------------
//  0x6F28C830 - a real body now, in unitquerypool.cpp.  The redirect that
//  stood here was a second definition of the same symbol: verify.py scored
//  the real body and said nothing, and the .mix link downgrades LNK2005 to
//  LNK4006 under /FORCE:MULTIPLE and takes whichever object came first - so
//  the thunk could have shadowed the reconstruction that replaced it.  Do
//  not put it back; tools/link_check.py is what catches this.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  0x6F468FC0 - a real body now, in querymodegate.cpp.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  0x6F25D830 - a real body now, in src/Agent/unitqueryfoldmask.cpp.  The
//  redirect must not come back: with both in the build the .mix has one
//  symbol defined twice, which only tools/link_check.py sees (LNK2005 -
//  the real build's /FORCE:MULTIPLE downgrades it to LNK4006 and silently
//  picks whichever object came first on the link line).
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  0x6F2867E0 - a real body now: it is
//  TSGrowableArray<UnitQueryPredicateFn>::Grow, in
//  src/Containers/predarraygrow.cpp.  The `__fastcall` free function this
//  used to be declared as and the member it really is have the same ABI -
//  the array in ecx, nothing pushed, `retn 0` - so the one call site's
//  instruction stream is unchanged by the swap.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  The seven owner-relationship narrowings are real bodies now -
//  jassgamestatemasks.cpp, on jassrelationagent.cpp.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  The three enumerators - see agenteventbinding.cpp for why no two of
//  them have the same convention.
//----------------------------------------------------------------------------
//  0x6F476DC0 - a real body now, in src/Agent/unitquerycircle.cpp, and the
//  redirect must not come back: two definitions of one symbol is an
//  LNK2005 only tools/link_check.py sees.
//  0x6F467570 - a real body now, in src/Agent/agentqueryrect.cpp.
//  0x6F46D1B0 - a real body now, in src/Agent/unitqueryoftype.cpp, and the
//  redirect must not come back: two definitions of one symbol is an
//  LNK2005 only tools/link_check.py sees.

#undef ADDR_THUNK_QUERY
