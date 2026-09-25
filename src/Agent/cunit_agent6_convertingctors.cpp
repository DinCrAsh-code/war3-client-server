//============================================================================
//  Two converting constructors of the same "type-checked smart pointer to a
//  CAgent-derived object" family agenttypedslots.h documents - the "zero
//  the value, Assign inside a __try, Release in a __finally guarded by
//  AbnormalTermination()" shape every member of the family with a scoped
//  local already has (that header's own comment; SWidgetAgentSlot's own
//  ctor, widgetagentslot_ctor.cpp, spells the identical three statements).
//
//  Each one's own Assign is a distinct address below this agent-6 slice's
//  own BFS cut (0x6F0DF6C0, 0x6F275630) - matching the established
//  convention for a required-type constant with no established meaning
//  (checkedslot_batch25.cpp), each stays thunked here rather than named:
//  it hands off to the real, unhooked shipped Assign.  Not a
//  reconstruction - delete the thunk as the real body lands, the way this
//  whole codebase already does for every other member of this family.
//============================================================================
#include "agent.h"
#include <excpt.h>   // AbnormalTermination()

//----------------------------------------------------------------------------
//  0x6F0E7E30's own slot.  0x6F0DF6C0 - thunked; see this file's own header
//  comment.  Not in an anonymous namespace: tools/verify.py's funcmap
//  lookup matches on the plain demangled name, which an anonymous
//  namespace would suffix with a compiler-generated hash.
//----------------------------------------------------------------------------
struct SCheckedSlot_6F0E7E30
{
    SCheckedSlot_6F0E7E30(CAgent* candidate);

    SCheckedSlot_6F0E7E30* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

__declspec(naked) SCheckedSlot_6F0E7E30* SCheckedSlot_6F0E7E30::Assign(CAgent*)
{
    __asm { mov eax, 0x6F0DF6C0 }
    __asm { jmp eax }
}

SCheckedSlot_6F0E7E30::SCheckedSlot_6F0E7E30(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//----------------------------------------------------------------------------
//  0x6F280E40's own slot.  0x6F275630 - thunked; see this file's own header
//  comment.  Not in an anonymous namespace, for the reason
//  SCheckedSlot_6F0E7E30 above is not.
//----------------------------------------------------------------------------
struct SCheckedSlot_6F280E40
{
    SCheckedSlot_6F280E40(CAgent* candidate);

    SCheckedSlot_6F280E40* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

__declspec(naked) SCheckedSlot_6F280E40* SCheckedSlot_6F280E40::Assign(CAgent*)
{
    __asm { mov eax, 0x6F275630 }
    __asm { jmp eax }
}

SCheckedSlot_6F280E40::SCheckedSlot_6F280E40(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}
