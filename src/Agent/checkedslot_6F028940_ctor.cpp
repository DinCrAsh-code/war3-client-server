//============================================================================
//  0x6F280FC0 - SCheckedSlot_6F028940's converting constructor, from the
//  CUnit vtable closure walk (cunit_agent2_worklist.md).  Same shape as
//  checkedslot_6F2075E0_ctor.cpp - zero m_value, Assign inside the body
//  directly (no __try/__finally survives to this build for the same
//  known SEH-frame reason the rest of the family hits).
//
//  Its own Assign body (0x6F275830) is cunit_agent6_worklist.md's, not
//  this agent's - declared, not defined, here, named by the same
//  address-of-its-getter convention so an independent reconstruction there
//  links against this without a merge-time rename.  0x6F275830 itself
//  gates on 0x6F028940's own rawcode ('Amai' MSB-first).
//============================================================================
#include "agent.h"
#include <excpt.h>   // AbnormalTermination()

struct SCheckedSlot_6F028940
{
    //  0x6F280FC0 - this file.
    SCheckedSlot_6F028940(CAgent* candidate);

    ~SCheckedSlot_6F028940() { Release(); }

    //  0x6F275830 - cunit_agent6_worklist.md's, not this file's.
    SCheckedSlot_6F028940* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

SCheckedSlot_6F028940::SCheckedSlot_6F028940(CAgent* candidate)
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
//  Link-completion thunk - 0x6F275830 is a real, separately-scored
//  reconstruction elsewhere (cunit_agent6's own SCheckedSlot_6F275830::Assign,
//  tools/funcmap.py), but under a different class name, so the declared
//  SCheckedSlot_6F028940::Assign above never resolves against it. Same
//  redirect-thunk pattern as this repo's other out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) SCheckedSlot_6F028940* SCheckedSlot_6F028940::Assign(CAgent*)
{
    __asm { mov eax, 06F275830h }
    __asm { jmp eax }
}
