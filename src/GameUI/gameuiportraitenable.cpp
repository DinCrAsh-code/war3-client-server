//============================================================================
//  0x6F33BD80 and 0x6F33BDB0 - the portrait panel's two halves of the same
//  control track.  Neither calls the other.
//
//  Both reach the frame base's own flag bit 2 - raised while the panel is
//  tracking, cleared when it stops - which is what makes this object a
//  CScreenFrame-derived one and what lets `this` be cast rather than a
//  second receiver type invented for it.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"

//----------------------------------------------------------------------------
//  0x6F33BD80 - adopt a portrait frame, or re-raise the tracking flag for
//  the one already there.  The null test is on the *argument* and the
//  re-read is of the member, which is why the member is loaded twice.
//----------------------------------------------------------------------------
void CGameUIPortraitPanel::SetPortraitFrame(void* frame)
{
    if (frame != 0)
        m_pFrame = frame;

    if (m_pFrame != 0)
        ((CSimpleFrame*)this)->RaiseFlagBit(2, 4);
}

//----------------------------------------------------------------------------
//  0x6F33BDB0 - stop tracking: drop the flag, then, if a selectable is still
//  held, post a CControlTrackEvent through the tracked frame's own vtable
//  slot 0x0C and clear the holder.
//
//  The event is a plain local record and *not* a class with a `virtual` in
//  it: declaring one would make this build stamp its own
//  `??_7CControlTrackEvent@@6B@`, which is a defect the instruction score
//  cannot see (both sides canonicalise to `mov [reg], SYM`) and which
//  tools/abi_audit.py reports as VTABLE SYMBOL / WRONG CLASS.  The real
//  vtable's address is written in as a literal instead, the way
//  Config/CMiscCustom.cpp already does it - 0x6F93A0A4, from
//  `ida_query names TrackEvent`.  The cost is one instruction: the shipped
//  operand is a named symbol and this one is a number.
//
//  Own translation unit: two real calls out of it and it is the same
//  shipped module as 0x6F33BD80 only by address, not by caller.
//============================================================================
struct SControlTrackEvent
{
    void* m_vtable;         // +0x00 - ??_7CControlTrackEvent@@6B@
    int   m_field04;
    void* m_pSelected;      // +0x08
    int   m_field0C;
    int   m_field10;
};

//  slot 0x0C of the tracked frame: post one event.
typedef void (__thiscall *PostTrackEventFn)(void* self, SControlTrackEvent* ev);

void CGameUIPortraitPanel::CancelControlTrack()
{
    ((CSimpleFrame*)this)->ClearFlagBit(2);

    void* selected = m_pSelected;

    if (selected != 0)
    {
        SControlTrackEvent ev;

        ev.m_field04 = 0;
        ev.m_field0C = 0;
        ev.m_vtable = (void*)0x6F93A0A4;
        ev.m_pSelected = selected;
        ev.m_field10 = 0;

        ((PostTrackEventFn)(*(void***)m_pFrame)[0x0C / 4])(m_pFrame, &ev);

        m_pSelected = 0;
    }
}
