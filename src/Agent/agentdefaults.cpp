//============================================================================
//  The three shared leaves in the 0x6F471xxx neighbourhood that CAgentWar3's
//  own default vtable slots forward to.  All three are reached with a real
//  `call`/`jmp` from a different module, so they need a translation unit of
//  their own to survive as calls at all - and they need to be in *one*
//  translation unit rather than one each only because none of them calls
//  another.
//
//  Two of them (0x6F471BE0 and 0x6F471C10) are the same two instructions
//  byte for byte.  The linker did not fold them, which is the useful part:
//  this image was linked without /OPT:ICF, so an address in
//  user_knowledge.json's vtable dumps is always the function the compiler
//  actually emitted there, never some unrelated twin it was folded onto.
//  Every "why is this CAgentWar3 method sitting in CWidget's address
//  neighbourhood" question in this family has to be answered some other
//  way (see agentwar3_flagbits.cpp).
//============================================================================
#include "agentwar3.h"

//----------------------------------------------------------------------------
//  0x6F471BE0 - the "one argument, always 0" default CAgentWar3's vtable
//  slot 8 tail-jumps to (agentwar3_thunks.cpp).
//----------------------------------------------------------------------------
int CAgent::Method_0x20(int)
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F471C10 - the "one argument, always 0" default CAgentWar3's
//  GetRecordVersion (slot 10) adds one to (agentwar3_slots.cpp).  Same two
//  instructions as CAgent::Method_0x20 above and a genuinely separate
//  function - see the file header.
//----------------------------------------------------------------------------
int CAgent::GetRecordVersion(int)
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F471BF0 - the default CAgentWar3's vtable slot 9 tail-jumps to
//  (agentwar3_thunks.cpp): ask the object for its own record version
//  through slot 10 (+0x28), throw the answer away and report 0.
//
//  A member of CAgentWar3 rather than a free function, even though it sits
//  in the 0x6F471xxx module with the other two: the shipped code takes
//  `this` in ecx and its one real argument on the stack (`retn 4`), which
//  is thiscall - a __fastcall free function would have taken that argument
//  in edx and cleaned nothing, and `ret` where the shipped code has `ret 4`
//  is the exact unbalanced-stack crash tools/abi_audit.py exists to catch.
//
//  The slot is reached through the vtable rather than by name because
//  GetRecordVersion is a *virtual* dispatch here: this default is shared,
//  and CItem overrides slot 10's neighbours around it.
//----------------------------------------------------------------------------
typedef int (__thiscall *RecordVersionFn)(void* self, int context);

int CAgent::Method_0x24(int context)
{
    ((RecordVersionFn)(*(void***)this)[0x28 / 4])(this, context);
    return 0;
}


//============================================================================
//  CAgent's own four slots in this same neighbourhood.  They were
//  __declspec(naked) redirects in vtablestubs.cpp until now; each is a real
//  body below, and the two empty ones carry the argument count their
//  shipped `retn` states rather than the `void()` the redirect let them get
//  away with (agent.h says why that mattered).
//============================================================================

//----------------------------------------------------------------------------
//  0x6F471BD0 - slot 7.  '+','a','g','a' low byte first: the same reversed
//  FourCC agiletype.h's kAgileRootTypeId spells, and the root of the family
//  CAgentWar3 ('+w3a'), CWar3Image ('+w3i'), CWidget ('+w3w') and
//  CSelectable ('+w3s') extend.  A bare `mov eax, imm32`.
//----------------------------------------------------------------------------
unsigned int CAgent::GetAgileTypeId() const
{
    return 0x2B616761;
}

//----------------------------------------------------------------------------
//  0x6F471C30 / 0x6F471C20 - slots 11 and 12.  Empty in every class in the
//  image that has them - CAgent through CItem, and CUnit and CDestructable
//  too - so there is no override anywhere to say what they are for, and
//  they keep slot-indexed names rather than invented ones.
//
//  Their argument counts are not a guess: `retn 8` and `retn 4`.
//----------------------------------------------------------------------------
void CAgent::Method_0x2C(int, int) {}
void CAgent::Method_0x30(int) {}

//----------------------------------------------------------------------------
//  0x6F471C40 - slot 13.  `retn`, no arguments.  CWar3Image (0x6F26B7D0)
//  and everything below it override this with real work; a bare agent has
//  nothing to deactivate.
//----------------------------------------------------------------------------
void CAgent::Deactivate() {}
