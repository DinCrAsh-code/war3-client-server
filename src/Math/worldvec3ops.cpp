//============================================================================
//  Two small CWorldVec3 leaves CCameraWar3::SnapPanTargetToGround
//  (0x6F3078B0, GameUI/cameracursorpreset.cpp) chains together to get a
//  squared distance moved: subtract two positions, then square-and-sum the
//  result.  Both are free/member wrappers around the existing CFloat
//  operators (Math/CFloat.h) - nothing here does arithmetic of its own, so
//  the exact ecx/edx/stack split each shipped call site uses is whatever
//  the already-established operator-/operator*/operator+ calling
//  convention produces for a hidden-return CFloat, not something this file
//  re-derives.
//
//  0x6F020210's own call site order (three operator- calls, highest
//  component first) is what pins operand direction down: the CCameraWar3
//  caller passes the *later*-fetched position as `edx` (this file's `a`)
//  and the *earlier* one on the stack (`b`), i.e. `a - b` is "new minus
//  old" at that call site - consistent with a squared-distance-moved check
//  where sign does not matter anyway.
//============================================================================
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F020210 - componentwise CWorldVec3 subtraction.  `retn 4`.
//----------------------------------------------------------------------------
CWorldVec3 __fastcall operator-(const CWorldVec3& a, const CWorldVec3& b)
{
    return CWorldVec3(a.m_x - b.m_x, a.m_y - b.m_y, a.m_z - b.m_z);
}

//----------------------------------------------------------------------------
//  0x6F0201B0 - CWorldVec3::LengthSquared.  `retn 4`.
//----------------------------------------------------------------------------
CFloat* __thiscall CWorldVec3::LengthSquared(CFloat* out) const
{
    CFloat sum = m_z * m_z;
    sum = m_y * m_y + sum;
    *out = m_x * m_x + sum;
    return out;
}
