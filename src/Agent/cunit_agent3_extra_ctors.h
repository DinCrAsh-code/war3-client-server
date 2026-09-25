//============================================================================
//  cunit_agent3 batch - three more converting constructors from CUnit's own
//  closure, same shape as cunit_agent3_checkedslots_ctor.cpp's own family
//  (m_value = 0; __try { Assign(candidate); } __finally { if
//  (AbnormalTermination()) Release(); }) - but each one's own Assign
//  (0x6F0755F0, 0x6F2754B0, 0x6F2758B0) is agent7's own worklist target in
//  this parallel run, not this agent's, so it is declared here, not
//  defined.
//============================================================================
#ifndef CUNIT_AGENT3_EXTRA_CTORS_H
#define CUNIT_AGENT3_EXTRA_CTORS_H

#include "agent.h"

struct SCheckedAbilitySlot_0x6F0755F0
{
    //  0x6F077660
    SCheckedAbilitySlot_0x6F0755F0(CAgent* candidate);
    ~SCheckedAbilitySlot_0x6F0755F0() { Release(); }

    //  0x6F0755F0 - agent7's own target, declared not defined here.
    SCheckedAbilitySlot_0x6F0755F0* Assign(CAgent* candidate);

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

struct SCheckedAbilitySlot_0x6F2754B0
{
    //  0x6F280D20
    SCheckedAbilitySlot_0x6F2754B0(CAgent* candidate);
    ~SCheckedAbilitySlot_0x6F2754B0() { Release(); }

    //  0x6F2754B0 - agent7's own target, declared not defined here.
    SCheckedAbilitySlot_0x6F2754B0* Assign(CAgent* candidate);

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

struct SCheckedAbilitySlot_0x6F2758B0
{
    //  0x6F281020
    SCheckedAbilitySlot_0x6F2758B0(CAgent* candidate);
    ~SCheckedAbilitySlot_0x6F2758B0() { Release(); }

    //  0x6F2758B0 - agent7's own target, declared not defined here.
    SCheckedAbilitySlot_0x6F2758B0* Assign(CAgent* candidate);

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

#endif

//----------------------------------------------------------------------------
//  Link-completion thunks - each of these addresses was declared (not
//  defined) elsewhere in this file because comments record it as another
//  parallel agent's own worklist target, but the two agents independently
//  invented different class/struct names for the same shared address, so
//  the declaring extern reference never actually resolves against the
//  sibling's real (differently-named) reconstruction. Defining a plain
//  redirect thunk here - the same `mov eax,<addr>/jmp eax` idiom this
//  repo already uses for any out-of-scope callee - makes this TU link
//  without re-claiming the address (no funcmap.py row points at these).
//----------------------------------------------------------------------------
__declspec(naked) SCheckedAbilitySlot_0x6F0755F0* SCheckedAbilitySlot_0x6F0755F0::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F0755F0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_0x6F2754B0* SCheckedAbilitySlot_0x6F2754B0::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F2754B0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_0x6F2758B0* SCheckedAbilitySlot_0x6F2758B0::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F2758B0h }
    __asm { jmp eax }
}
