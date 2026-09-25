//============================================================================
//  0x6F0116E0 - SStormSingletonB::QueryNearestObstruction: cast the segment
//  `from` -> `to` at every registered candidate and hand back the nearest
//  hit's distance.
//
//  The running best starts at +infinity (flt_6FA4FA9C) and `hit` is set the
//  moment any candidate answers, before the distance is even compared - so a
//  hit at exactly +infinity still counts.  The distance is only published
//  when something was hit *and* the caller asked for it, which is why the
//  two tests are separate.
//
//  The `<=` in the running minimum is the shipped comparison
//  (`fcom st(1)` / `test ah,41h` / `jnz store`), not a `<`; see
//  docs/msvc-vc8-idioms.md, "A float comparison's status-word mask,
//  decoded".
//
//  Own translation unit: the per-candidate test is a real call into the
//  sprite layer, and QueryNearestObstruction's own caller is another module.
//============================================================================
#include "modelpick.h"
#include "stormsingletonb.h"

//  0x6F26BA80 - Misc/trivialpredicates.cpp: the candidate, unless its own
//  flag says to skip it.  Spelled exactly as that file defines it, `int`
//  return included: MSVC puts the return type in the mangled name, so a
//  `void*` here would be a second symbol nothing defines.
int __fastcall GetField0x28UnlessFlag_6F26BA80(const void* self);
//  0x6F4D2D70 is a real body now, in Render/spriteintersect.cpp, and its
//  receiver is a CSprite.  Declared from that module's own header rather
//  than re-declared with a `void*` here: MSVC puts the parameter types in
//  the mangled name, so the two spellings were two symbols with one readable
//  name and the call site's was defined nowhere.

//: 0x7F800000 - positive infinity, as the shipped constant pool holds it.
static const unsigned int kInfinityBits = 0x7F800000u;
#define kInfinity (*(const float*)&kInfinityBits)

int SStormSingletonB::QueryNearestObstruction(const float* from,
                                             const float* to,
                                             float* outDistance)
{
    float best = kInfinity;
    int   hit = 0;

    for (unsigned int i = 0; i < m_count; i++)
    {
        void* candidate =
            (void*)GetField0x28UnlessFlag_6F26BA80(m_data[i].m_pObject);
        if (candidate == 0)
            continue;

        float distance;
        if (SpriteIntersectSegment((CSprite*)candidate, from, to, 0.0f,
                                   &distance) == 0)
            continue;

        hit = 1;
        if (distance <= best)
            best = distance;
    }

    if (hit != 0 && outDistance != 0)
        *outDistance = best;

    return hit;
}
