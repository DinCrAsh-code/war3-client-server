//============================================================================
//  A plain (hardware) 3-float vector - real x87 arithmetic, not CFloat.  See
//  fvec3normalize.cpp for Normalize()'s own body; split into its own header
//  so other modules that build these (e.g. itemgroundlookup.cpp) can share
//  the type without pulling fvec3normalize.cpp's own translation unit in.
//============================================================================
#ifndef FVEC3_H
#define FVEC3_H

class FVec3
{
public:
    void Normalize();
    //  0x6F301BE0 - GameUI/gameuivectorlength.cpp.
    float Length() const;

    float m_x, m_y, m_z;
};

#endif
