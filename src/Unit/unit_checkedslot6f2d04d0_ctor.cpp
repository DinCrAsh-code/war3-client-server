//============================================================================
//  0x6F2D3800 - SCheckedSlot_6F2D04D0's constructor (checkedslot_6f2d04d0.h).
//  The same __try/Assign/__finally-Release shape every constructor in this
//  family has - see checkedfogmodifierslot.cpp.
//============================================================================
#include "checkedslot_6f2d04d0.h"
#include <excpt.h>

SCheckedSlot_6F2D04D0::SCheckedSlot_6F2D04D0(CAgent* candidate)
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

//  0x6F2D04D0 - out of this group's own worklist slice; a naked redirect
//  to the real, unhooked shipped body rather than a reconstruction of it.
__declspec(naked) SCheckedSlot_6F2D04D0* SCheckedSlot_6F2D04D0::Assign(CAgent*)
{
    __asm { mov eax, 06F2D04D0h }
    __asm { jmp eax }
}
