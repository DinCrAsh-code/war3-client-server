//============================================================================
//  0x6F2ABDC0 - hand a CFloat 3x3 matrix back as a hardware-float one.
//
//  Not a conversion: CFloat's storage already *is* an IEEE-754 single
//  (CFloat.h), so all nine elements are reinterpreted, not converted - the
//  shipped code has no `fild` anywhere, only `fld`/`fstp` pairs against the
//  same bit patterns.
//
//  Two shapes in here are read straight off the disassembly:
//
//  * The whole matrix is copied into a local first - nine `mov` pairs into
//    a contiguous 36-byte frame slot, scheduled out of order but landing in
//    member order - and the nine `fld`s then read *that*, not `*this`.
//    That is what a by-value `AsHardwareFloat(CFloat)` produces at each of
//    its nine inlined call sites: a non-trivially-copyable parameter has to
//    be materialised into a stack slot before its address can be taken.
//    jasssetitemposition.cpp's own by-reference spelling of the same helper
//    does not copy, and would emit `fld [ecx+N]` directly.
//  * The result is returned by value, not written through an out-pointer.
//    36 bytes goes back through a hidden buffer whose pointer the callee
//    also leaves in eax, which is exactly what CWidget::InitSprite does
//    with it: `mov esi, eax` straight into a `rep movsd` that pushes the
//    matrix on as SetItemPlacementOrientation's own by-value argument.
//
//  Its own translation unit: CWidget::InitSprite calls it for real, and the
//  two would otherwise share widget.cpp's module and be inlined together.
//============================================================================
#include "cmatrix3.h"
#include "itemplacementdispatch.h"

//  By reference - the copy the shipped code makes is the whole matrix, once,
//  not one per element.  See the file header.
static inline float AsHardwareFloat(const CFloat& value)
{
    return *(const float*)&value.m_bits;
}

SOrientationMatrix CMatrix3::ToOrientationMatrix() const
{
    //  The whole matrix is copied into a local before any element is read.
    //  Nine `mov` pairs, and every `fld` afterwards reads the local rather
    //  than `*this` - reading the members directly emits nine `fld [ecx+N]`
    //  and no copies at all, which is eighteen instructions short.
    CMatrix3 t = *this;

    SOrientationMatrix out;
    out.m[0] = AsHardwareFloat(t.m_00);
    out.m[1] = AsHardwareFloat(t.m_01);
    out.m[2] = AsHardwareFloat(t.m_02);
    out.m[3] = AsHardwareFloat(t.m_10);
    out.m[4] = AsHardwareFloat(t.m_11);
    out.m[5] = AsHardwareFloat(t.m_12);
    out.m[6] = AsHardwareFloat(t.m_20);
    out.m[7] = AsHardwareFloat(t.m_21);
    out.m[8] = AsHardwareFloat(t.m_22);
    return out;
}
