//============================================================================
//  0x6F40FAF0 - see playerwar3_slot30.cpp's own header comment for the
//  whole chain.  Own translation unit, split from
//  playerwar3_buildintegerlistenerlevel0.cpp (0x6F02D800) specifically so
//  MSVC's /Ob2 cannot collapse this thin wrapper into its one caller -
//  the two are real, separately addressed functions in the shipped
//  binary, and same-TU inlining was measured to merge them into one body.
//============================================================================
#include "playerwar3.h"

void** CPlayerWar3::BuildIntegerListenerLevel1(void** outListener, int five,
                                                int six, void* owner,
                                                int zero)
{
    m_abilityEntries[five].BuildIntegerListener(outListener, -1, six, owner,
                                                 zero);
    return outListener;
}
