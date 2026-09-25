//============================================================================
//  Handle to a move in progress, resolved to a world position.
//============================================================================
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F4742F0 - resolve the handle, ask the move where it is now, and lift
//  that grid position into world space.
//----------------------------------------------------------------------------
CWorldVec3& CPathRef::ToWorldVec3(CWorldVec3& out) const
{
    const CPathTrace* trace =
        static_cast<const CPathTrace*>(LookupHandle(m_handle, m_typeTag));

    CGridVec2 point = trace->PointAtDistance(trace->GetTravelDistance());

    out = CWorldVec3(GridToWorld(point.m_x) + g_pGameData->m_originX,
                     GridToWorld(point.m_y) + g_pGameData->m_originY,
                     g_CFloatZero);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F4743A0 - the same thing, returned by value.
//----------------------------------------------------------------------------
CWorldVec3 CPathRef::SmartPtrToWorldVec3() const
{
    CWorldVec3 position;
    ToWorldVec3(position);
    return position;
}
