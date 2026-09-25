//============================================================================
//  SHandleObjectRange / SHandleObjectRangeImpl - the published {low, high}
//  window on a resolved handle object, and the three methods that maintain
//  it.
//
//  Two names for one object, kept apart on purpose.  `SHandleObjectRange`
//  is the minimal declaration SUnitTrackedRef's three setters call through
//  (unittrackedref.cpp), and MSVC puts the class name in the mangled name,
//  so SetRange's definition has to stay on *that* type or those calls go
//  unresolved.  `SHandleObjectRangeImpl` is the same bytes with the fields
//  named.
//
//  The object is a CPathTrace: game.h calls the same +0x78/+0x80/+0x84
//  words RampBase()/RampMin()/RampMax(), and 0x6F4A90B0 - the
//  PrepareRangeUpdate below - calls CPathTrace::GetTravelDistance,
//  RampValueAt and RefreshSyncSample on `this`.  The two spellings are
//  kept because nothing already matching through m_value/m_low/m_high has
//  to move, which is the same reason game.h keeps two for +0x78.
//
//  This header exists because the three methods live in three translation
//  units - they call each other for real, so no two of them may be folded
//  together - and a per-file copy of the layout is how the offsets drift.
//============================================================================
#ifndef HANDLEOBJECTRANGE_H
#define HANDLEOBJECTRANGE_H

#include "CFloat.h"

struct SHandleObjectRange
{
    void SetRange(const CFloat* pair);   // 0x6F4A9390
};

struct SHandleObjectRangeImpl
{
    //  0x6F4A9390 - handleobjectrange.cpp.
    void SetRange(const CFloat* pair);

    //  0x6F4A90B0 - rangeprepareupdate.cpp.  `retn 0`: no arguments, the
    //  object arrives in ecx.
    void PrepareRangeUpdate();

    //  0x6F4A92B0 - rangeaddvalue.cpp.  `retn 4`.  The one mutator: settle
    //  the ramp, move m_value by `delta`, clamp it into [m_low, m_high] and
    //  notify.  SUnitTrackedRef::SetValue reaches "set to v" through it by
    //  passing v minus the current reading.
    void AddToValue(const CFloat* delta);

    //  0x6F4A9230 - rangenotifychanged.cpp.  `retn 0Ch`: the new value's
    //  address, the old value's address, and a listener that may be null.
    void NotifyRangeChanged(const CFloat* value, const CFloat* previous,
                            void* listener);

    //  0x6F4A91B0 - rangeannouncerate.cpp.  `retn 8`: one payload word
    //  (the new rate) and a listener that may be null - the same
    //  '^pro'-tagged single-listener-or-broadcast shape NotifyRangeChanged
    //  and SPublishedFieldOwner::NotifyField78Changed
    //  (Misc/handlefieldnotify.cpp) already establish, a third sibling in
    //  that family (tag2 = "`rle" where NotifyRangeChanged's own is
    //  "`rld").
    void AnnounceRateChange(const CFloat* value, void* listener);

    //  0x6F4A9320 - rangeadjustrate.cpp.  `retn 4`: settle the ramp, add
    //  `delta` to RampRate() (game.h), snap it to zero once it is within
    //  the leftover epsilon (g_unk6FAAE61C, floatnearlyequal.cpp) and
    //  announce the change with RampBase() as the payload and no listener.
    void AdjustRate(const CFloat* delta);

    char   m_reserved00[0x4F];
    //  0x4F - bit 0 suppresses the change notification entirely.  A byte,
    //  not a word: the shipped test is `test byte ptr [ecx+4Fh], 1`.
    unsigned char m_suppressNotify;   // 0x4F
    char   m_reserved50[0x28];
    CFloat m_value;     // 0x78 - the published value, clamped into the window
    //  0x7C - the same word game.h's CPathTrace calls RampRate() (its own
    //  +0x7C, CFloat& RampRate() { return m_origin.m_y; }) - named here
    //  too rather than left reserved, since rangeadjustrate.cpp reads and
    //  writes it directly on this same object.
    CFloat m_rate;      // 0x7C
    CFloat m_low;       // 0x80
    CFloat m_high;      // 0x84
};

#endif
