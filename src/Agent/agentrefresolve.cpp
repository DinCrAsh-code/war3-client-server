//============================================================================
//  0x6F478630 - AgentRef::Resolve.  See agentref.h.
//
//  Two handle lookups in a row: this reference's own {handle, typeTag} pair
//  names an object that carries a second pair at +0x78/+0x7C, and it is the
//  object *that* names which comes back - null when either lookup fails, and
//  null again when the answer's +0x20 pending flag is set.
//
//  Both null exits share one `xor eax,eax` / `retn` in the shipped code,
//  which is what the single trailing `return 0` here is: writing the inner
//  failure as its own `return 0` splits it into two.
//
//  The pending-flag gate is branchless - `neg` / `sbb eax,eax` / `not` /
//  `and eax,ecx`, the complement of docs/msvc-vc8-idioms.md's
//  `(x != 0) ? C : 0` - so it is spelled as the conditional expression it is
//  and not as an `if`.
//
//  Own translation unit: AgentRef::QueryField0x54 (agentreffield54.cpp)
//  calls it for real, and is 0x70 bytes further along in the same module.
//============================================================================
#include "agentref.h"

//----------------------------------------------------------------------------
//  What this reference's own pair resolves to: an object holding a second
//  handle pair at +0x78.  Only those two words are in scope, so only they
//  are named - the surrounding layout is not recoverable from here.
//----------------------------------------------------------------------------
struct SHandleRefAt0x78Owner
{
    char            m_reserved00[0x78];
    unsigned int    m_handle;       // +0x78
    int             m_typeTag;      // +0x7C
};

SItemHandleObject* AgentRef::Resolve()
{
    SHandleRefAt0x78Owner* owner =
        (SHandleRefAt0x78Owner*)LookupHandle(m_handle, m_typeTag);
    if (owner == 0)
        return 0;

    SItemHandleObject* object =
        (SItemHandleObject*)LookupHandle(owner->m_handle, owner->m_typeTag);
    if (object == 0)
        return 0;

    return object->m_pendingFlag == 0 ? object : 0;
}
