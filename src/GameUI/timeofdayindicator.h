//============================================================================
//  CTimeOfDayIndicator - the day/night dial in the top bar - and CTimeCover,
//  the invisible frame over it that carries its two tooltips.
//
//  Both class names are the shipped ones: each constructor stamps its own
//  ??_7<Class>@@6B@ at +0x00 and a second ??_7<Class>@@6B@_0 at +0x0B4,
//  which is the two-vtable shape of a CFrame subclass (CLayer at 0,
//  CLayoutFrame at 0xB4) and settles the name and the base together.
//  CTimeOfDayIndicator's base is one further down: its constructor calls
//  CSpriteFrame's (0x6F60F030) and its own fields start where CSpriteFrame's
//  end, at +0x1B0.
//============================================================================
#ifndef TIMEOFDAYINDICATOR_H
#define TIMEOFDAYINDICATOR_H

#include "frame.h"

//----------------------------------------------------------------------------
//  The cover.  0x774 bytes, which is exactly what its constructor's own
//  allocation asks Storm for and what the two tooltip buffers plus CFrame's
//  0x168 add up to.
//----------------------------------------------------------------------------
class CTimeCover : public CFrame
{
public:
    //  0x6F33C600 - GameUI/timecoverctor.cpp.  `retn 4`.
    CTimeCover(void* parent);

    int  m_field168;            // +0x168
    int  m_field16C;            // +0x16C
    int  m_field170;            // +0x170
    char m_toolTip[0x200];      // +0x174
    char m_uberTip[0x400];      // +0x374
};

class CTimeOfDayIndicator : public CSpriteFrame
{
public:
    //  0x6F36BEB0 - GameUI/timeofdayindicatorctor.cpp.  `retn 4`.
    CTimeOfDayIndicator(CFrame* parent);

    //  +0x1B0 - the cover, anchored over the dial and owning both tooltips.
    CTimeCover* m_pCover;               // +0x1B0
    //  +0x1B4 - the constructor zeroes it and RefreshDialAnimation is the
    //  only reader: set, the dial plays the two-token animation; clear, the
    //  one-token one.  What writes it is outside this tree, so the name says
    //  what it selects and not what it means.
    int         m_extendedDialAnim;     // +0x1B4
    //  +0x1B8 / +0x1BC - [Misc] ScaledAnimTime and [Misc] DayHours, read
    //  once at construction.  Stored as plain floats: the config read hands
    //  back a CFloat and the shipped code copies it through the x87, which
    //  is a float assignment and not a four-byte class copy.
    float       m_scaledAnimTime;       // +0x1B8
    float       m_dayHours;             // +0x1BC

    //  0x6F33C6F0 - start the dial sprite's animation.  `retn 0`.
    //  GameUI/timeofdaydial.cpp.
    void RefreshDialAnimation();
};

#endif
