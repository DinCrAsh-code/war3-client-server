//============================================================================
//  See cunit_agent3_extra_ctors.h.  Same __try/__finally shape as every
//  other converting constructor in this family (SCheckedLocationSlot::
//  SCheckedLocationSlot, checkedlocationslot.cpp).
//============================================================================
#include "cunit_agent3_extra_ctors.h"
#include <excpt.h>   // AbnormalTermination()

//  0x6F077660
SCheckedAbilitySlot_0x6F0755F0::SCheckedAbilitySlot_0x6F0755F0(CAgent* candidate)
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

//  0x6F280D20
SCheckedAbilitySlot_0x6F2754B0::SCheckedAbilitySlot_0x6F2754B0(CAgent* candidate)
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

//  0x6F281020
SCheckedAbilitySlot_0x6F2758B0::SCheckedAbilitySlot_0x6F2758B0(CAgent* candidate)
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
