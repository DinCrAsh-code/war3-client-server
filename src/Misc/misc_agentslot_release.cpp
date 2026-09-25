//============================================================================
//  0x6F388920 - release the CAgent held in a one-pointer slot, if any:
//  decrement its refcount and, on the last reference, tail-call its own
//  vtable slot 0 (ReleaseSelf).  `this` in ecx, no stack arguments -
//  `retn 0` - the same three-instruction shape agenttypedslots.h's own
//  `Release()` bodies spell inline for every SCheckedXxxSlot, but reached
//  here as a genuine standalone leaf: three unrelated callers
//  (ResolvePlayerHandle - Jass/jassresolveplayerhandle.cpp, now itself a
//  real reconstruction whose own scope-exit `SCheckedPlayerSlot` release
//  compiles to the identical body; and two more out of this closure) all
//  tail-jump straight into this one address rather than each carrying
//  their own copy - the shipped compiler's own whole-program folding of
//  identical inline expansions, which a per-TU, non-whole-program build
//  cannot reproduce as a shared call target, only as its own address with
//  a matching body.
//
//  Own translation unit: a homeless leaf, src/README.md's "Misc/ is a real
//  answer, not a failure".
//============================================================================
#include "agent.h"

struct SAgentPtrSlot
{
    CAgent* m_value;

    void Release();
};

void SAgentPtrSlot::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}
