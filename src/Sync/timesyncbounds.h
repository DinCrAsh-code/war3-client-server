//============================================================================
//  A handle's position relative to the map bounds, extrapolated forward and
//  fed into the footprint-shape query (0x6F46xxxx-0x6F49xxxx neighbourhood).
//============================================================================
#ifndef TIMESYNCBOUNDS_H
#define TIMESYNCBOUNDS_H

#include "game.h"

//----------------------------------------------------------------------------
//  0x6F474250 - resolve `this`'s handle, take its position's distance from
//  the map origin on both axes, flush each to zero once it drops under a
//  small threshold (the same "(delta - k) sign bit" flush this binary's
//  other CFloat code uses - see src/softfloat.cpp, CFloatReciprocal), and
//  hand the pair on to sub_6F4737D0 along with the handle object and the
//  caller's own extra argument.
//
//  Left as a literal transliteration of the flush bit-trick rather than a
//  named helper: `((delta - 0x3000000) ^ delta) >> 31` does not reduce to
//  any of this codebase's already-named CFloat idioms, and nothing in this
//  call tree pins down what it is testing well enough to name it.
//----------------------------------------------------------------------------
struct SHandleWithType
{
    //  A member, not a free function: the shipped code's `retn 8` leaves
    //  both real arguments on the stack with only `this` in ecx.
    void FlushedOriginDelta(const CWorldVec3* pos, int flag);

    //  0x6F476C40 - see positionrangecheck.cpp.  A member for the same
    //  reason: `retn 10h` with four stack arguments and only `this` in
    //  ecx.  "Is (x, y) within `range` of whatever this handle names?"
    int IsWithinRange(const CFloat* range, const CFloat* x, const CFloat* y,
                      int fromTravelPoint);

    //  0x6F4762B0 - see widget_handlebits.cpp.  Also a member for the same
    //  reason: `retn 4` with only `this` in ecx.  Resolves m_handle through
    //  the global handle table and writes the low 24 bits of a colour-ish
    //  word hanging off the result.
    void SetLow24Bits(unsigned int bits);

    //  0x6F4762E0 - see widget_handlebits.cpp.  Also a member for the same
    //  reason: `retn 4` with only `this` in ecx.  Resolves m_handle the
    //  same way, then propagates a packed byte to a different field
    //  (+0xA8) on the resolved object.
    void PropagatePlacementIndex(unsigned int arg0);

    //  0x6F474830 - see handlemoved.h/notifyhandlemoved.cpp.  A thiscall
    //  member, not a __fastcall free function: both arguments are on the
    //  stack and it cleans eight bytes, where a __fastcall free function
    //  would have taken `facing` in edx and cleaned four.  Declared here
    //  rather than left in handlemoved.h's own now-merged copy of this
    //  struct - see that header for why the two had to become one.
    void NotifyHandleObjectMoved(const CFloat* facing, int republish);

    char         m_reserved00[8];
    unsigned int m_handle;    // +0x08
    int          m_typeTag;   // +0x0C
};

#endif
