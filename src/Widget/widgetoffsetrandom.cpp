//============================================================================
//  0x6F30AE20 - reached through CWidget::NegateAndForwardOffset's closure.
//
//  A thiscall accessor over a small object whose owning class is not
//  established by this dump (depth-1 walk, no caller in tree): only the
//  three offsets this body actually touches are named.  +0x40 bit 0x1000 is
//  the same "randomize" flag CWidget::GetOffsetPosition tests on its own
//  m_flags (widgetoffsetposition.cpp), but the field layout here (+0x30/
//  +0x34 as a base/range float pair) does not match CWidget's own +0x30/+0x34
//  (m_footprintType / m_pathGridRegistration per widget.h), so this is a
//  different, smaller object - not CWidget itself.
//
//  When the flag is clear, the answer is the plain base value.  When it is
//  set, a global SHashState is advanced (sub_6F011390, already
//  SHashState::Advance - Math/hashstate.h/.cpp) and its low 23 bits forced
//  into a [1,2) float mantissa the same way RandomCFloat does
//  (Math/randomcfloat.cpp), but here with real x87 instructions rather than
//  through CFloat's own operators - this object's fields are plain hardware
//  floats, not the software CFloat type.  The raw hash value's *sign* then
//  picks which way the 1.0 subtraction goes (`uniform - 1.0` vs
//  `1.0 - uniform`), producing a signed offset in (-1, 1) whose sign matches
//  the hash draw - the natural C++ for that shape is the if/else below, not
//  a ternary: the compiler emits a real branch either way, and this is the
//  spelling that keeps the two arms in the shipped order (subtract-from vs
//  subtract-by).
//============================================================================
#include "hashstate.h"

extern SHashState g_widgetRandomHashState;   // unk_6FAB58CC

struct SOffsetRangeSource
{
    float RandomizeValue();

    char  m_reserved00[0x30];
    float m_base;    // +0x30
    float m_range;   // +0x34
    char  m_reserved38[0x40 - 0x38];
    unsigned int m_flags;   // +0x40
};

float SOffsetRangeSource::RandomizeValue()
{
    if (!(m_flags & 0x1000))
        return m_base;

    int raw = g_widgetRandomHashState.Advance();
    int bits = (raw & 0x7FFFFF) | 0x3F800000;
    float uniform = *(float*)&bits;

    float delta;
    if (raw < 0)
        delta = (float)(1.0 - uniform);
    else
        delta = (float)(uniform - 1.0);

    return m_base + delta * m_range;
}
