//============================================================================
//  CAngle - see cameraangle.cpp for the full derivation.  Split into a
//  header because Misc/ccameraconstruct.cpp (CCamera::CCamera, 0x6F4E7790)
//  constructs four of these as real member sub-objects and needs the type,
//  not just the address of its constructor.
//============================================================================
#ifndef CAMERAANGLE_H
#define CAMERAANGLE_H

#include "storm.h"

//  Local stand-in for CBaseManaged, shared by every file in this family
//  that needs more than the bare vtable-stamp trick Widget/cdatamgrbasemanagedlist.cpp
//  and Misc/ccamera.cpp's own (separate, TU-local) stand-ins use: a real
//  TSLink<CBaseManaged> membership, because CAngle's own constructor's SEH
//  unwind path genuinely calls `m_link.Unlink()` on a half-constructed
//  object (see cameraangle.cpp).
class CBaseManaged
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}
    ~CBaseManaged() { m_link.Unlink(); }

    TSLink<CBaseManaged> m_link;    // +0x04
};

class CAngle : public CBaseManaged
{
public:
    CAngle(float value);
    void SetValue(const float* pValue);

    unsigned char m_reserved0C;         // +0x0C
    unsigned char m_flags;               // +0x0D
    unsigned char m_reserved0E[2];       // +0x0E
    int           m_field10;             // +0x10
    int           m_field14;             // +0x14
    float         m_rawValue;            // +0x18
    float         m_normalizedValue;     // +0x1C
    float         m_cos;                  // +0x20
    float         m_sin;                  // +0x24
};

#endif
