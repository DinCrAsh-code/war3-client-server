//============================================================================
//  0x6F012790 - the segment query CGameUI's screen projection uses to decide
//  whether a unit is occluded.
//
//  Two independent collections are asked - the large Storm singleton
//  (0x6F01F5A0) through its own 0x6F762310, and the small one (0x6F01F760)
//  through SStormSingletonB::QueryNearestObstruction - and the answer is the
//  *nearer* of the two hits.  A caller that gets 0 back must not read the
//  distance: neither query writes it when it misses.
//
//  Both distances start at +infinity and the picked one is stored through
//  `outDistance` at the very end, which is why both arms of the comparison
//  end in the same three instructions with only the `fstp` operand differing
//  (docs/msvc-vc8-idioms.md, "A pointer select over two floats").
//
//  Batch A reached this as a redirect and called it IsPointWithinRadius, on
//  the strength of its call sites pre-loading their `radius` with infinity;
//  what it actually computes is the nearest obstruction along the segment,
//  and the caller then compares that against its own length.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "stormsingletona.h"
#include "stormsingletonb.h"

SStormSingletonA* __fastcall GetStormSingletonA();      // 0x6F01F5A0

//: 0x7F800000 - positive infinity, as the shipped constant pool holds it.
static const unsigned int kInfinityBits = 0x7F800000u;
#define kInfinity (*(const float*)&kInfinityBits)

int __fastcall QueryNearestObstruction(const float* from, const float* to,
                                       float* outDistance, int flags)
{
    float hitPoint[3];
    float distanceA = kInfinity;
    float distanceB = kInfinity;

    hitPoint[0] = 0.0f;
    hitPoint[1] = 0.0f;
    hitPoint[2] = 0.0f;

    int hitA = GetStormSingletonA()
                   ->QueryObstruction(from, to, hitPoint, &distanceA, flags);

    int hitB = GetStormSingletonB()
                   ->QueryNearestObstruction(from, to, &distanceB);

    if (hitA == 0 && hitB == 0)
        return 0;

    *outDistance = (distanceB <= distanceA) ? distanceB : distanceA;
    return 1;
}
