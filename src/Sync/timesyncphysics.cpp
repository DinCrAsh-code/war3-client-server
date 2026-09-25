//============================================================================
//  0x6F4A7380, 0x6F4A6FD0 - moving a path trace's origin and telling the two
//  spatial grids it is registered in.  The two grid updates themselves are
//  0x6F4A6D70/0x6F4A6E40 and live in pathtracegrid.cpp.
//============================================================================
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F4A7380
//----------------------------------------------------------------------------
void CPathTrace::ApplyDelta(const CGridVec2* delta, int notify)
{
    RecomputeOrigin(delta);

    if (notify != 0)
    {
        //  vtable slot 0x54 - a virtual observer callback whose concrete
        //  override is not in this call tree.  Modelled as a raw vtable
        //  read, same as sub_6F2AC220's own vtable calls (see widget.cpp),
        //  because this class carries no other virtuals in the dump to hang
        //  a declaration on without inventing slots 0-20.
        typedef void (__thiscall *NotifyFn)(void*, CGridVec2*);
        NotifyFn fn = (*(NotifyFn**)this)[0x54 / 4];
        fn(this, &m_origin);
    }
}

//----------------------------------------------------------------------------
//  0x6F4A6FD0 - origin += direction * distance + delta, then tell the two
//  observers past it about the move.
//----------------------------------------------------------------------------
void CPathTrace::RecomputeOrigin(const CGridVec2* delta)
{
    //  A reference to the returned temporary, not a copy: the shipped code
    //  keeps GetTravelDistance()'s own result pointer in a register and
    //  hands it to both multiplies.
    const CFloat& distance = GetTravelDistance();
    //  Spelled out rather than `m_direction * distance`: this one hands both
    //  multiplies the same CFloat object, where PointAtDistance's shipped
    //  code copies it once per component (see game.h's operator*).
    const CGridVec2& dir = m_direction;
    CGridVec2 offset(dir.m_x * distance, dir.m_y * distance);
    CGridVec2 next = m_origin + offset;
    m_origin = next + *delta;

    RefreshSyncSample();
    UpdatePathGridBox(&m_radius, &m_origin);
    UpdateCollisionGridBox(&m_radius, &m_origin);
}
