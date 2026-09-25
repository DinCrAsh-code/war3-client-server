//============================================================================
//  Three small Vec3-shaped leaves reached through
//  CWidget::NegateAndForwardOffset's closure (the vtable[0x128] "apply
//  offset" overrides transitively reach them).
//============================================================================
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F2AAE80 - copy three plain ints into three CFloats, one component at
//  a time.  `retn 4`, thiscall: `this` is the CFloat destination and the
//  one stack argument is the int source.  CFloatFromInt returns its result
//  through a hidden pointer (a class return under this ABI shifts the real
//  first argument into edx and takes the return buffer in ecx), which is
//  why each of the three calls addresses a different `ecx` (dest+0/+4/+8)
//  while reading the matching source int into edx.
//----------------------------------------------------------------------------
struct CFloatVec3
{
    CFloat* __thiscall ConvertFromIntVec3(const int* src);
    CFloat  m_v[3];
};

CFloat* __thiscall CFloatVec3::ConvertFromIntVec3(const int* src)
{
    m_v[0] = CFloatFromInt(src[0]);
    m_v[1] = CFloatFromInt(src[1]);
    m_v[2] = CFloatFromInt(src[2]);
    return m_v;
}

struct FVec3
{
    FVec3& __thiscall operator-=(const FVec3& o);
    float  __thiscall LengthSquared() const;

    float x, y, z;
};

//----------------------------------------------------------------------------
//  0x6F309B10 - componentwise Vec3 -=.  `retn 4`, thiscall: `this` is the
//  vector being modified in place, the one stack argument is a const
//  reference to the vector subtracted from it.  It is `operator-=`, not a
//  void helper: the shipped body opens with `mov eax, ecx` before touching
//  anything else, which is MSVC staging the `FVec3&` return value (in eax)
//  before `ecx` is freed up to hold the other operand's pointer instead.  A
//  void-returning spelling of the identical arithmetic never emits that
//  copy (there is nothing to stage into eax), so it scored 1/12 - the
//  return type is the whole difference.
//----------------------------------------------------------------------------
FVec3& __thiscall FVec3::operator-=(const FVec3& o)
{
    x -= o.x;
    y -= o.y;
    z -= o.z;
    return *this;
}

//----------------------------------------------------------------------------
//  0x6F2BAEC0 - a Vec3's squared length (dot product with itself).  `retn 0`,
//  thiscall, no stack arguments at all: the result comes back in ST(0).
//----------------------------------------------------------------------------
float __thiscall FVec3::LengthSquared() const
{
    return x * x + y * y + z * z;
}
