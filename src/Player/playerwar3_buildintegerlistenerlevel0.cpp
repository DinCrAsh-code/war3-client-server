//============================================================================
//  0x6F02D800 - see playerwar3_slot30.cpp's own header comment for the
//  whole chain.  Own translation unit (0x6F02xxxx, nowhere near
//  CPlayerWar3's own 0x6F40xxxx/0x6F41xxxx module) so MSVC's /Ob2 cannot
//  inline 0x6F40FAF0 (playerwar3_buildintegerlistenerlevel1.cpp) into this
//  caller - measured: same-TU inlining collapsed the two into one body,
//  which the dump's own two distinct addresses rule out.
//============================================================================
#include "playerwar3.h"

void** CPlayerWar3::BuildIntegerListenerLevel0(void** outListener, int six,
                                                void* owner, int zero)
{
    BuildIntegerListenerLevel1(outListener, 5, six, owner, zero);
    return outListener;
}
