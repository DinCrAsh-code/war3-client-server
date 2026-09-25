//============================================================================
//  A plain (hardware) plane: unit normal + signed distance, the same
//  four-float shape FVec3 (fvec3.h) uses for its first three fields - see
//  fplanefromthreepoints.cpp, which relies on that layout match to reuse
//  FVec3::Normalize() on a plane's own normal in place.
//============================================================================
#ifndef FPLANE_H
#define FPLANE_H

class FPlane
{
public:
    //  0x6F267580 - normal = normalize((b-a) x (c-a)), then
    //  m_d = -(normal . a).
    void FromThreePoints(const struct FVec3* a, const struct FVec3* b,
                         const struct FVec3* c);

    float m_x, m_y, m_z, m_d;
};

#endif
