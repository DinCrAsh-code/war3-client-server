//============================================================================
//  0x6F2F77A0 - SCheckedCameraSlot's constructor (agenttypedslots.h).
//
//  Own translation unit, separate from its own Assign: the constructor
//  issues a genuine `call sub_6F2F49E0`, so the two must not be in a
//  position to be inlined into each other.
//
//  The same __try/__finally every other constructor in this family has, and
//  the same shortfall: the shipped body is wrapped in the
//  __except_handler4-shaped, cookie-XORed, frame-pointer-omitted frame
//  docs/msvc-vc8-idioms.md records as unreproducible at this toolchain's
//  fixed /GS- /EHs-c-.  The eleven instructions inside it match; the frame
//  does not.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedCameraSlot::SCheckedCameraSlot(CAgent* candidate)
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
