//============================================================================
//  0x6F3026E0 - CCameraWar3::QueryControllerVec3: zero the caller's
//  three-float point and ask the live controller for its own slot 8 into it.
//
//  The three zeroes are one `fldz` and two `fst`s plus an `fstp`, which is
//  what three assignments of the same float constant give and what an
//  aggregate initialiser does not.
//
//  Own translation unit: the controller query is a real call.
//============================================================================
#include "gameui.h"

//  0x6F4E8260 - the controller-side query, `retn 4`: the controller in ecx,
//  the slot in edx and the caller's buffer on the stack.  Thunked in
//  gameui_thunks.cpp.
#include "spriteattach.h"

float* CCameraWar3::QueryControllerVec3(float* out)
{
    out[0] = 0.0f;
    out[1] = 0.0f;
    out[2] = 0.0f;

    QuerySlotVec3(m_pController, 8, out);
    return out;
}
