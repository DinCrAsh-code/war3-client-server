//============================================================================
//  Three more event-context accessors in the 0x6F3C2xxx module, none
//  calling another:
//
//      0x6F3C2020  GetEventGameState                id 0x80203
//      0x6F3C2050  GetEventPlayerState               id 0x8020B
//      0x6F3C2080  GetEventPlayerChatStringMatched   id 0x80260
//============================================================================
#include "jassevents.h"
#include "handleobject.h"
#include "jassnatives.h"   // MakeJassStringHandleFromCString

//----------------------------------------------------------------------------
//  0x6F3C2020 - the event's +0x40 state, returned as-is when it is 0, 1 or
//  2 (two separate range checks in the shipped code, not one - kept as two
//  `if`s rather than collapsed into `< 3`, since that is the instruction
//  shape the dump actually has), else 0.
//----------------------------------------------------------------------------
int GetEventGameState()
{
    //  Single-exit, "start at 0 and overwrite" shape - the same idiom
    //  jasseventaccessors.cpp's own header comment already names for this
    //  event-id-checked family.
    int result = 0;

    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) == 0x80203)
    {
        CScriptEvent* event = GetCurrentScriptEvent();
        if (event)
        {
            int state = event->m_gameState;
            if (state < 2)
                result = state;
            else if (state < 3)
                result = state;
        }
    }

    return result;
}

//----------------------------------------------------------------------------
//  0x6F3C2050 - the event's +0x44 state, a checked cast against 26 (the
//  same "compare, sbb, and" idiom docs/msvc-vc8-idioms.md already
//  catalogues): valid range returns itself, out of range returns 0.
//----------------------------------------------------------------------------
int GetEventPlayerState()
{
    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) != 0x8020B)
        return 0;

    CScriptEvent* event = GetCurrentScriptEvent();
    if (!event)
        return 0;

    int state = event->m_playerState;
    return ((unsigned int)state < 0x1A) ? state : 0;
}

//----------------------------------------------------------------------------
//  0x6F3C2080 - the event's +0x50 handle-bearing object (GetHandleOrZero's
//  own SHandleHolder shape), turned into a JASS string handle through
//  MakeJassStringHandleFromCString (0x6F3BAA20, jassmakestringhandle.cpp) -
//  the same tail-call shape GetSaveBasicFilename ends on.
//----------------------------------------------------------------------------
int GetEventPlayerChatStringMatched()
{
    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) != 0x80260)
        return 0;

    CScriptEvent* event = GetCurrentScriptEvent();
    if (!event)
        return 0;

    const char* matched =
        (const char*)GetHandleOrZero((SHandleHolder*)event->m_reservedChatString);

    return MakeJassStringHandleFromCString(matched);
}
