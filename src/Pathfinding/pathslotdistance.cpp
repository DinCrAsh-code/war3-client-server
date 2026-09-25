//============================================================================
//  0x6F49C8D0 - CPathClient::CheckSlotDistance: is `to` within `scale` of
//  the point the given slot's cursor is currently on?
//
//  Both arrays are indexed the way PointForSlot indexes them - `m_cache[slot]`
//  and `m_cursor[slot]` as the declared arrays - because that is what makes
//  MSVC emit the shipped `[this + (slot+2)*32]` and `[this + slot*4 + 0x74]`
//  addressing modes.  See PointForSlot's own note: the indices genuinely run
//  past the two declared entries and the spelling is deliberate.
//
//  The threshold is squared rather than the distance rooted - `*scale *
//  *scale` against CFloatDistanceSquared's own result - so there is no
//  square root anywhere on this path.
//============================================================================
#include "pathmove.h"

int CPathClient::CheckSlotDistance(const CGridVec2* to, int slot,
                                   const CFloat* scale)
{
    CFloat distSq = CFloatDistanceSquared(m_cache[slot].Points()[m_cursor[slot]],
                                          *to);
    //  Bound to a reference, and the answer computed as a `bool`: the
            //  shipped code reads the squared threshold back through the
            //  pointer operator* returned rather than out of a local slot,
            //  and widens its 0/1 answer with a `movzx` - which is what a
            //  bool-typed result converted to the int return type does.
    const CFloat& limit = *scale * *scale;

    //  Compared through the stored bit patterns rather than a by-value CFloat
    //  helper, the same way CPathClient::RunPathQuery compares its own pair:
    //  taking a CFloat by value copies it through a stack slot first, where
    //  the shipped code `fld`s both operands straight out of the objects.
    int within;
    if (*(const float*)&distSq.m_bits <= *(const float*)&limit.m_bits)
        within = 1;
    else
        within = 0;

    //  Narrowed and re-widened on purpose: both shipped exits materialise a
    //  whole-register 0 or 1 and then run it through `movzx eax, al`, which
    //  is what a byte-typed answer returned as an int does.  A `bool` local
    //  gets there in `al` and loses the two 32-bit forms.
    return (unsigned char)within;
}
