//============================================================================
//  0x6F3056C0 - CCameraWar3::IsPanMovementActive.  `retn 0`.
//
//  m_pPanMovement (+0x464) is "the MovementModifier a timed pan installs on
//  m_target" (gameui.h); this leaf answers whether that modifier's handle
//  still resolves to a live object.  The field is declared `TRefCnt*` in
//  gameui.h and cast to `MovementModifier*` at the point of use, the same
//  way camerareleasepan.cpp already does - see positionmodifier.h for why
//  the handle/typeTag pair sits at exactly +0x08/+0x0C.
//
//  Two explicit `mov eax, 0/1; retn` arms rather than a folded boolean:
//  `A && B` compiled straight through gives this shape when the result is
//  used as a plain return value, not stored into a flag - see
//  docs/msvc-vc8-idioms.md for the general two-branch return pattern.
//============================================================================
#include "gameui.h"
#include "positionmodifier.h"

int CCameraWar3::IsPanMovementActive() const
{
    MovementModifier* move = (MovementModifier*)m_pPanMovement;
    if (move && LookupHandle(move->m_handle, move->m_typeTag))
        return 1;
    return 0;
}
