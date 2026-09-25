//============================================================================
//  0x6F301E40 - CCameraWar3::GetAgileTypeId() const, vtable slot 7.  Two
//  instructions: return the literal FourCC tag '+cam' (0x2B63616D, read
//  most-significant-byte-first) that SCheckedCameraSlot::Assign checks
//  candidates against (gameui.h's own note above CCameraWar3).
//============================================================================
#include "gameui.h"

unsigned int CCameraWar3::GetAgileTypeId() const
{
    return 0x2B63616D;
}
